#!/usr/bin/env python
# -*- coding: utf-8 -*-
r"""extract_984.py - trich TOAN BO muc cua map 984 (\maps\特殊用地\banghuichengbao) tu pak
Linux sang mot pak MOI (maps_banghuichengbao.pak), copy NGUYEN BYTE (giu co/nen).

Bo cuc pak (Sources\Engine\Src\XPackFile.cpp:13-33, da doc that):
  header 32 byte : 'PACK'(4) uCount(4) uIndexTableOffset(4) uDataOffset(4) uCrc32(4) reserved(12)
  index 16 byte/muc: uId(4) uOffset(4) lSize(4, co goc) lCompressSizeFlag(4: byte cao = kieu nen
                     TYPE_NONE 0 / TYPE_UCL 0x01 / 0x20 (ban VNG, XPackFile.cpp:258 chap nhan),
                     3 byte thap = co sau nen)
  XPackFile::FindElemFile tim NHI PHAN theo uId (XPackFile.cpp:275-292) => bang index PHAI SAP
  TANG theo uId. uCrc32 engine KHONG kiem (XPackFile.cpp:98-106) -> ghi 0.
Bam ten: KPakList::FileNameToId (KPakList.cpp:72-85): ha A-Z, byte >=0x80 la SO AM, & 0xFFFFFFFF
  truoc % 0x8000000B, nhan 0xFFFFFFEF, cuoi cung ^ 0x12345678.

Cach chon muc: doc .wor (giai nen UCL nrv2b) -> [MAIN] rect=l,t,r,b -> quet cua so region TUYET DOI
  (KSubWorld.cpp:1789-1797: RegionBegin = rect.left/top; KRegion::Load/LoadObject: v_%03d\%03d_<ten>)
  voi 10 mau ten tep region (SceneDataDef.h:12-47) + mot so tep cap thu muc.
Bo sung: --extra-scan quet them 0..999 x 0..999 de chac khong sot (1 phut 30).

  python extract_984.py                      -> ghi port_tongcastle\out\maps_banghuichengbao.pak + danh sach
  python extract_984.py --verify <pak>       -> doi chieu pak da co (vd ban o bin\server\Pak) voi Linux
  python extract_984.py --out <duong dan pak>

KHONG tu ghi vao cay chay that. Chep tay sang bin\server\Pak + them dong vao package.ini
([Package] N=maps_banghuichengbao.pak - KSOServer.cpp:465 g_PakList.Open("\\package.ini"),
KPakList::Open chi nap cac pak LIET KE trong ini, KHONG quet thu muc).
"""
import io, os, struct, sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BS = chr(92)
HERE = os.path.dirname(os.path.abspath(__file__))
PAK_LINUX = r"D:\ServerLinux\server1\pak\maps.pak"
OUT_DEFAULT = os.path.join(HERE, "out", "maps_banghuichengbao.pak")
MAPDIR_B = b"\xcc\xd8\xca\xe2\xd3\xc3\xb5\xd8" + b"\\banghuichengbao"   # 特殊用地\banghuichengbao (GBK)
BASE_B = b"\\maps\\" + MAPDIR_B

REGION_FILES = [b"Region_S.dat", b"Region_C.dat", b"OBSTACLE.DAT", b"Trap.dat", b"Npc_S.dat",
                b"Npc_C.dat", b"Obj_S.dat", b"Obj_C.dat", b"Ground.dat", b"BuildinObj.Dat"]
DIR_FILES = [b".wor", b".ini", b".txt", b".map", b".mps"]
DIR_SUB = [b"Trap.txt", b"Trap.ini", b"ObstacleTable.txt", b"Obstacle.txt", b"NpcList.txt", b"Npc.txt",
           b"map.ini", b"MapInfo.ini", b"minimap.spr", b"Ground.dat", b"BuildinObj.Dat", b"bg.ini",
           b"weather.ini", b"Region.ini", b"Sound.ini"]


def name2id(name):
    i = 0
    idx = 0
    for b in name:
        c = b
        if 0x41 <= b <= 0x5A:
            c = b + 0x20
        elif b >= 0x80:
            c = b - 0x100
        idx += 1
        i = ((((i + idx * c) & 0xFFFFFFFF) % 0x8000000B) * 0xFFFFFFEF) & 0xFFFFFFFF
    return (i ^ 0x12345678) & 0xFFFFFFFF


def nrv2b_decompress(src, dst_size):
    dst = bytearray()
    st = {"bb": 0, "ip": 0}
    last_off = 1

    def getbit():
        if st["bb"] & 0x7F:
            st["bb"] = (st["bb"] * 2) & 0xFFFFFFFF
        else:
            st["bb"] = src[st["ip"]] * 2 + 1
            st["ip"] += 1
        return (st["bb"] >> 8) & 1

    while True:
        while getbit():
            dst.append(src[st["ip"]])
            st["ip"] += 1
        off = 1
        while True:
            off = off * 2 + getbit()
            if getbit():
                break
        if off == 2:
            off = last_off
        else:
            off = (off - 3) * 256 + src[st["ip"]]
            st["ip"] += 1
            if off == 0xFFFFFFFF or off < 0:
                break
            off += 1
            last_off = off
        ln = getbit()
        ln = ln * 2 + getbit()
        if ln == 0:
            ln = 1
            while True:
                ln = ln * 2 + getbit()
                if getbit():
                    break
            ln += 2
        if off > 0xD00:
            ln += 1
        pos = len(dst) - off
        assert pos >= 0
        for _ in range(ln + 1):
            dst.append(dst[pos])
            pos += 1
    assert len(dst) == dst_size, (len(dst), dst_size)
    return bytes(dst)


def load_index(pak):
    f = open(pak, "rb")
    hdr = f.read(32)
    sig, cnt, ioff, doff, crc = struct.unpack("<4sIIII", hdr[:20])
    assert sig == b"PACK", sig
    f.seek(ioff)
    raw = f.read(16 * cnt)
    tbl = {}
    order = []
    for k in range(cnt):
        uid, off, size, flag = struct.unpack_from("<IIiI", raw, k * 16)
        tbl[uid] = (off, size, flag)
        order.append(uid)
    return f, tbl, order, (cnt, ioff, doff)


def read_raw(f, off, size, flag):
    method = flag & 0xFF000000
    csize = flag & 0x00FFFFFF
    f.seek(off)
    if method == 0:
        return f.read(size)
    return f.read(csize)


def read_elem(f, off, size, flag):
    raw = read_raw(f, off, size, flag)
    method = flag & 0xFF000000
    if method == 0:
        return raw
    assert method in (0x01000000, 0x20000000), hex(method)
    return nrv2b_decompress(raw, size)


def parse_rect(wor):
    txt = wor.decode("latin-1")
    for ln in txt.replace("\r", "").split("\n"):
        if ln.strip().lower().startswith("rect="):
            v = ln.split("=", 1)[1].split(",")
            return [int(x) for x in v[:4]]
    return None


def candidates(rect, extra=False):
    out = {}
    for suf in DIR_FILES:
        out[BASE_B + suf] = name2id(BASE_B + suf)
    for fn in DIR_SUB:
        k = BASE_B + b"\\" + fn
        out[k] = name2id(k)
    if rect:
        l, t, r, b = rect
        ys = range(max(0, t - 2), b + 3)
        xs = range(max(0, l - 2), r + 3)
    else:
        ys = xs = range(0)
    for y in ys:
        for x in xs:
            for fn in REGION_FILES:
                k = BASE_B + (b"\\v_%03d\\%03d_" % (y, x)) + fn
                out[k] = name2id(k)
    if extra:
        for y in range(1000):
            for x in range(1000):
                for fn in REGION_FILES:
                    k = BASE_B + (b"\\v_%03d\\%03d_" % (y, x)) + fn
                    out.setdefault(k, name2id(k))
    return out


def collect(extra=False):
    f, tbl, order, hdr = load_index(PAK_LINUX)
    wor_uid = name2id(BASE_B + b".wor")
    assert wor_uid in tbl, "THIEU .wor trong pak Linux"
    wor = read_elem(f, *tbl[wor_uid])
    rect = parse_rect(wor)
    print("wor: %d byte, rect=%s" % (len(wor), rect))
    cand = candidates(rect, extra)
    hits = []
    for k, uid in cand.items():
        if uid in tbl:
            off, size, flag = tbl[uid]
            hits.append((uid, k, off, size, flag))
    # loai trung uid (2 ten bam trung nhau - chi giu ten dau tien theo thu tu quet)
    seen = {}
    for h in hits:
        seen.setdefault(h[0], h)
    hits = sorted(seen.values(), key=lambda h: h[0])
    return f, hits, wor, rect


def write_pak(f, hits, out):
    os.makedirs(os.path.dirname(out), exist_ok=True)
    blobs = []
    for uid, k, off, size, flag in hits:
        blobs.append((uid, size, flag, read_raw(f, off, size, flag)))
    cnt = len(blobs)
    data_off = 32
    idx_off = data_off + sum(len(b[3]) for b in blobs)
    with open(out, "wb") as o:
        o.write(struct.pack("<4sIIII", b"PACK", cnt, idx_off, data_off, 0) + b"\0" * 12)
        cur = data_off
        index = []
        for uid, size, flag, raw in blobs:
            o.write(raw)
            index.append(struct.pack("<IIiI", uid, cur, size, flag))
            cur += len(raw)
        assert cur == idx_off
        for e in index:
            o.write(e)
    return cnt, idx_off + 16 * cnt


def verify(pak, hits, f_lnx):
    f2, tbl2, order2, hdr2 = load_index(pak)
    print("pak kiem: %s  count=%d  index sap tang=%s" % (pak, hdr2[0], order2 == sorted(order2)))
    miss = 0
    diff = 0
    for uid, k, off, size, flag in hits:
        if uid not in tbl2:
            miss += 1
            print("  THIEU  %08X %s" % (uid, k.decode("gbk", "replace")))
            continue
        o2, s2, fl2 = tbl2[uid]
        a = read_raw(f_lnx, off, size, flag)
        b = read_raw(f2, o2, s2, fl2)
        if s2 != size or fl2 != flag or a != b:
            diff += 1
            print("  KHAC   %08X size %d/%d flag %08X/%08X %s" % (uid, size, s2, flag, fl2, k.decode("gbk", "replace")))
    extra = set(tbl2) - set(h[0] for h in hits)
    print("  => %d muc Linux; thieu %d; khac %d; pak kiem co them %d uid khong thuoc danh sach" % (len(hits), miss, diff, len(extra)))


if __name__ == "__main__":
    args = sys.argv[1:]
    extra = "--extra-scan" in args
    out = OUT_DEFAULT
    if "--out" in args:
        out = args[args.index("--out") + 1]
    f, hits, wor, rect = collect(extra)
    lst = os.path.join(HERE, "out", "danhsach_984.txt")
    os.makedirs(os.path.dirname(lst), exist_ok=True)
    with io.open(lst, "w", encoding="utf-8") as o:
        o.write("# map 984 trong %s : %d muc, rect=%s\n" % (PAK_LINUX, len(hits), rect))
        for uid, k, off, size, flag in hits:
            o.write("%08X\t%d\t%08X\t%s\n" % (uid, size, flag, k.decode("gbk", "replace")))
    print("%d muc; danh sach -> %s" % (len(hits), lst))
    kinds = {}
    for uid, k, off, size, flag in hits:
        tail = k.rsplit(b"_", 1)[-1] if b"\\v_" in k else k.rsplit(b"\\", 1)[-1]
        kinds[tail] = kinds.get(tail, 0) + 1
    for t, n in sorted(kinds.items()):
        print("   %-16s %d" % (t.decode("latin-1"), n))
    if "--verify" in args:
        verify(args[args.index("--verify") + 1], hits, f)
    else:
        cnt, total = write_pak(f, hits, out)
        print("ghi %s : %d muc, %d byte" % (out, cnt, total))
