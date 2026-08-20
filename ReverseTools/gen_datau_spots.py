# -*- coding: ascii -*-
"""
gen_datau_spots.py - SINH TOA DO QUAI CHO AUTO DA TAU (ban 2, 19/08/2026)

VI SAO: client CHI thay NPC may chu da dong bo (~2 man hinh). Cho Xa Phu tha xuong
map nhiem vu thuong KHONG co con quai nao trong tam do -> auto quet rong -> di mo.
Nguon SU THAT ve cho quai la file add NPC cua may chu NAM TRONG PAK:
    \\maps\\<duong dan map>\\v_<nY>\\<nX>_Region_S.dat   (muc REGION_NPC_FILE_INDEX)

BAN 2 khac ban 1:
  * Quet TOAN BO map trong TL_MAPTRAPINDEX (map_index.lua ~204 map) chu khong chi 14 map
    dang bat trong tasklink_findmaps.txt - de doi bang nhiem vu la auto van co du lieu.
  * Xuat ra FILE TXT doc luc chay: bin\\client\\settings\\datau_toado.txt (sua tay duoc,
    khong phai dung lai DLL). Header KDaTauSpots.h van sinh de lam ban DU PHONG khi
    thieu tep txt.
  * Tim region nhanh: gieo mam thua roi loang (BFS) thay vi quet 65536 o moi map;
    bo qua region chi co vat can (kich thuoc goc == 2100 byte) truoc khi giai nen.

Doc: XPackFile.cpp (dinh dang pak), KPakList::FileNameToId (bam ten), ucl/n2b_d.c
     (giai nen nrv2b), KRegion::LoadObject (duong dan), SceneDataDef.h (KSPNpc),
     KSubWorld.cpp:1756 (\\maps\\<ten>), map_index.lua (TL_MAPTRAPINDEX).
Chay: python D:\\GAMEDEVNEW\\ReverseTools\\gen_datau_spots.py [--maps 1,78,79]
"""
import os, struct, sys

SRV  = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI  = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
PAKS = ["maps.pak", "maps_error.pak", "namcung.pak", "maps_tieu_bang_chien.pak"]
INI  = os.path.join(SRV, "settings", "MapList.ini")
MAPIDX = os.path.join(SRV, "script", "task", "newtask", "map_index.lua")
FINDMAPS = os.path.join(SRV, "settings", "task", "tasklink_findmaps.txt")
OUT_H   = r"D:\GAMEDEVNEW\Sources\Core\Src\KDaTauSpots.h"
OUT_TXT = os.path.join(CLI, "settings", "datau_toado.txt")
OUT_TXT2 = r"D:\GAMEDEVNEW\ReverseTools\datau_toado.txt"

M32 = 0xFFFFFFFF
KIND_NORMAL = 0          # GameDataDef.h:1369
CUM = 1024               # canh o gom cum (mps)
MAX_SPOT = 24            # so cum giu moi map
MIN_QUAI = 2             # cum it hon nay thi bo
# Nguong bo qua region: PHAI nho hon "header 52 + KNpcFileHead 12 + 1 KSPNpc 60".
# (Da do thuc te: o map 1 co 8 region CO NPC ma kich thuoc chi <= 2100 byte - loc theo
#  2100 lam mat 16 con quai. Dung loc theo kich thuoc "region chi co vat can" nua.)
CHI_VATCAN = 124
SO_TIEN_TRINH = 8        # quet song song cho nhanh (moi tien trinh mo pak rieng)
# Chi so region trong ten tep co cong offset m_nRegionBeginX/Y nen phai quet rong.
# Da do: map that cham toi 251; de 384 cho du bien (bo loc chu ky loai het va cham).
DAI_QUET = 384

# ---------------------------------------------------------------- bam ten

def hstep(uid, idx, bs):
    """Chay tiep ham bam KPakList::FileNameToId tren mot doan byte."""
    for b in bs:
        c = b if b < 0x80 else b - 0x100          # char CO DAU
        if 0x41 <= b <= 0x5A:                      # g_StrLower
            c = b + 0x20
        idx += 1
        uid = ((((uid + idx * c) & M32) % 0x8000000b) * 0xffffffef) & M32
    return uid, idx


def name_to_id(s):
    uid, idx = hstep(0, 0, s)
    return uid ^ 0x12345678


def ucl_nrv2b_decompress_8(src, out_len):
    """Ban Python cua ucl_nrv2b_decompress_8 (ucl/n2b_d.c + getbit_8)."""
    dst = bytearray(out_len)
    ilen = 0
    olen = 0
    bb = 0
    last_m_off = 1
    n = len(src)

    def getbit():
        nonlocal bb, ilen
        if bb & 0x7f:
            bb = (bb * 2) & M32
        else:
            bb = (src[ilen] * 2 + 1) & M32
            ilen += 1
        return (bb >> 8) & 1

    while True:
        while getbit():
            if ilen >= n or olen >= out_len:
                raise ValueError("tran vung khi giai nen")
            dst[olen] = src[ilen]
            olen += 1
            ilen += 1
        m_off = 1
        while True:
            m_off = m_off * 2 + getbit()
            if getbit():
                break
        if m_off == 2:
            m_off = last_m_off
        else:
            if ilen >= n:
                raise ValueError("het nguon giua chung")
            m_off = (m_off - 3) * 256 + src[ilen]
            ilen += 1
            if m_off == 0xFFFFFFFF:
                break
            m_off += 1
            last_m_off = m_off
        m_len = getbit()
        m_len = m_len * 2 + getbit()
        if m_len == 0:
            m_len = 1
            while True:
                m_len = m_len * 2 + getbit()
                if getbit():
                    break
            m_len += 2
        if m_off > 0xd00:
            m_len += 1
        if m_off > olen or olen + m_len + 1 > out_len:
            raise ValueError("tro nguoc/tran khi chep lai")
        pos = olen - m_off
        dst[olen] = dst[pos]
        olen += 1
        pos += 1
        while m_len > 0:
            dst[olen] = dst[pos]
            olen += 1
            pos += 1
            m_len -= 1
    if olen != out_len:
        raise ValueError("giai nen ra %d byte, cho %d" % (olen, out_len))
    return bytes(dst)


class Pak(object):
    def __init__(self, path):
        self.f = open(path, "rb")
        head = self.f.read(32)
        if head[:4] != b"PACK":
            raise ValueError("%s khong phai pak PACK" % path)
        count, index_off, data_off, crc = struct.unpack_from("<IIII", head, 4)
        self.f.seek(index_off)
        raw = self.f.read(count * 16)
        self.idx = {}
        for i in range(count):
            uid, off, size, cflag = struct.unpack_from("<IIiI", raw, i * 16)
            self.idx[uid] = (off, size, cflag)
        self.name = os.path.basename(path)

    def info(self, uid):
        return self.idx.get(uid)

    def doc(self, uid):
        e = self.idx.get(uid)
        if not e:
            return None
        off, size, cflag = e
        method = (cflag >> 24) & 0xFF
        csize = cflag & 0xFFFFFF
        self.f.seek(off)
        if method == 0:
            return self.f.read(size)
        return ucl_nrv2b_decompress_8(self.f.read(csize), size)


# ---------------------------------------------------------------- du lieu nguon

def doc_maplist(path):
    out = {}
    for line in open(path, "rb").read().split(b"\n"):
        line = line.strip()
        if not line or line[:1] in (b";", b"["):
            continue
        if b"=" not in line:
            continue
        k, v = line.split(b"=", 1)
        if k.isdigit():
            out[int(k)] = v.strip()
    return out


def doc_mapindex(path):
    """TL_MAPTRAPINDEX: {id, "ten", X, Y} - X/Y don vi O LUOI."""
    import re
    raw = open(path, "rb").read()
    rows = []
    seen = set()
    for m in re.finditer(rb'\{\s*(\d+)\s*,\s*"([^"]*)"\s*,\s*(\d+)\s*,\s*(\d+)\s*\}', raw):
        mid = int(m.group(1))
        if mid in seen:
            continue
        seen.add(mid)
        rows.append((mid, m.group(2), int(m.group(3)), int(m.group(4))))
    return rows


def doc_findmaps(path):
    """Map DANG BAT cho nhiem vu loai 4 (cot MapID)."""
    ids = set()
    try:
        data = open(path, "rb").read().split(b"\n")
    except IOError:
        return ids
    if not data:
        return ids
    head = data[0].split(b"\t")
    try:
        col = head.index(b"MapID")
    except ValueError:
        return ids
    for line in data[1:]:
        p = line.split(b"\t")
        if len(p) > col and p[col].strip().isdigit():
            ids.add(int(p[col].strip()))
    return ids


def doc_npc_region(blob):
    """Region_S.dat hop nhat -> [(x, y, kind, level, ten)]; None neu KHONG phai region.

    PHAI kiem that chat: ham bam ten tep cua pak chi 31 bit nen quet 100-150 nghin ten
    se DUNG DO vai chuc id cua tep KHAC (do da kiem chung: cac 'region' rot ra ngoai dai
    hop le dung bang so va cham du kien). Nhan dang region that bang chu ky:
      * so muc = REGION_ELEM_FILE_COUNT = 6
      * muc vat can (REGION_OBSTACLE_FILE_INDEX = 0) dai dung 2048 byte
        (REGION_GRID_WIDTH 16 * REGION_GRID_HEIGHT 32 * 4)
      * moi muc nam gon trong tep
    """
    if len(blob) < 4:
        return None
    (num_sect,) = struct.unpack_from("<I", blob, 0)
    if num_sect != 6:                       # REGION_ELEM_FILE_COUNT (SceneDataDef.h:35)
        return None
    head_size = 4 + 8 * num_sect
    if len(blob) < head_size:
        return None
    muc = []
    for k in range(num_sect):
        off, length = struct.unpack_from("<II", blob, 4 + 8 * k)
        if head_size + off + length > len(blob):
            return None
        muc.append((off, length))
    off, length = muc[2]                    # REGION_NPC_FILE_INDEX = 2
    beg = head_size + off
    if length < 12:
        return []                           # region that nhung khong co NPC
    (num_npc,) = struct.unpack_from("<I", blob, beg)
    if num_npc > 4096:
        return None
    p = beg + 12
    out = []
    for _ in range(num_npc):
        if p + 60 > len(blob):
            break
        tpl, px, py = struct.unpack_from("<iii", blob, p)
        name = blob[p + 16:p + 48].split(b"\x00")[0]
        lvl, frame, head_img, kind = struct.unpack_from("<hhhh", blob, p + 48)
        camp, series, slen = struct.unpack_from("<BBH", blob, p + 56)
        p += 60 + slen
        out.append((px, py, kind, lvl, name))
    return out


# ---------------------------------------------------------------- quet mot map

def quet_map(paks, duongdan):
    """Quet DAY DU 256x256 chi so region (chi so co cong offset m_nRegionBeginX/Y nen
    khong the doan dai hep). Nhanh nho: (a) bam TANG DAN - phan '\\maps\\<map>\\v_NNN\\'
    chi bam mot lan cho moi hang, (b) region chi co vat can (size == 2100) thi khong
    giai nen. Tra (danh sach (x,y) quai, so region, so region co NPC)."""
    st0 = hstep(0, 0, b"\\maps\\" + duongdan + b"\\v_")
    quai = []
    nReg = 0
    nCoNpc = 0
    nVaCham = 0
    nMax = 0
    for ny in range(0, DAI_QUET):
        u1, i1 = hstep(st0[0], st0[1], b"%03d\\" % ny)
        for nx in range(0, DAI_QUET):
            uid, _ = hstep(u1, i1, b"%03d_region_s.dat" % nx)
            uid ^= 0x12345678
            pk = None
            for p in paks:
                if p.info(uid):
                    pk = p
                    break
            if pk is None:
                continue
            off, size, cflag = pk.info(uid)
            if size < CHI_VATCAN:
                nVaCham += 1
                continue                 # nho hon ca header+1 NPC -> khong phai region
            try:
                blob = pk.doc(uid)
            except Exception:
                nVaCham += 1             # giai nen hong = tep khac trung id
                continue
            ds = doc_npc_region(blob)
            if ds is None:
                nVaCham += 1             # khong dung chu ky region = trung id
                continue
            # PHEP KIEM TUYET DOI chong va cham bam: NPC cua region (nx,ny) BAT BUOC nam
            # trong o cua chinh no - X trong [nx*512, +512), Y trong [ny*1024, +1024)
            # (REGION_GRID_WIDTH 16 o * 32 mps = 512; REGION_GRID_HEIGHT 32 o * 32 = 1024).
            # Tep khac trung id thi toa do "npc" cua no roi lung tung -> loai ca region.
            if ds:
                nTrong = 0
                for (px, py, kind, lvl, ten) in ds:
                    if (nx * 512 <= px < (nx + 1) * 512
                     and ny * 1024 <= py < (ny + 1) * 1024):
                        nTrong += 1
                if nTrong * 2 < len(ds):     # qua nua NPC nam ngoai o -> khong phai region nay
                    nVaCham += 1
                    continue
            nReg += 1
            if ds:
                nCoNpc += 1
                # chi region DA KIEM CHUNG (co NPC dung o) moi tinh vao bien quet
                if nx > nMax:
                    nMax = nx
                if ny > nMax:
                    nMax = ny
            for (px, py, kind, lvl, ten) in ds:
                if kind != KIND_NORMAL:
                    continue
                if px <= 0 or py <= 0:
                    continue
                quai.append((px, py))
    return quai, nReg, nCoNpc, nVaCham, nMax


def gom_cum(quai, ax_mps, ay_mps):
    cum = {}
    for (px, py) in quai:
        k = (px // CUM, py // CUM)
        c = cum.setdefault(k, [0, 0, 0])
        c[0] += px
        c[1] += py
        c[2] += 1
    ds = []
    for k, c in cum.items():
        if c[2] < MIN_QUAI:
            continue
        ds.append((c[0] // c[2], c[1] // c[2], c[2]))
    ds.sort(key=lambda t: -t[2])
    ds = ds[:MAX_SPOT]
    # cum gan diem Xa Phu tha xuong len truoc (di gan nhat truoc)
    ds.sort(key=lambda t: (t[0] - ax_mps) ** 2 + (t[1] - ay_mps) ** 2)
    return ds


_PAKS = []


def _mo_pak():
    """Moi tien trinh con mo bo pak cua rieng no (khong chia se file handle)."""
    global _PAKS
    _PAKS = []
    for pk in PAKS:
        full = os.path.join(SRV, "Pak", pk)
        if os.path.exists(full):
            _PAKS.append(Pak(full))


def _quet_mot_map(viec):
    mid, duongdan = viec
    quai, nreg, nnpc, nvacham, nmax = quet_map(_PAKS, duongdan)
    return (mid, quai, nreg, nnpc, nvacham, nmax)


def main():
    chon = None
    for i, a in enumerate(sys.argv):
        if a == "--maps" and i + 1 < len(sys.argv):
            chon = set(int(x) for x in sys.argv[i + 1].split(","))

    _mo_pak()
    if not _PAKS:
        print("KHONG THAY pak nao trong %s\\Pak" % SRV)
        return 1
    print("pak: %s" % ", ".join("%s(%d)" % (p.name, len(p.idx)) for p in _PAKS))

    duong = doc_maplist(INI)
    dsmap = doc_mapindex(MAPIDX)
    batmap = doc_findmaps(FINDMAPS)
    print("TL_MAPTRAPINDEX: %d map | tasklink_findmaps.txt (dang BAT): %d map -> %s"
          % (len(dsmap), len(batmap), sorted(batmap)))

    viecs = []
    thongtin = {}
    for (mid, ten, cx, cy) in dsmap:
        if chon and mid not in chon:
            continue
        p = duong.get(mid)
        if not p:
            print("  map %3d: khong co trong MapList.ini - bo qua" % mid)
            continue
        viecs.append((mid, p))
        thongtin[mid] = (ten, cx, cy)

    ket = []
    tong_quai = 0
    from multiprocessing import Pool
    print("quet %d map bang %d tien trinh..." % (len(viecs), SO_TIEN_TRINH))
    sys.stdout.flush()
    with Pool(processes=SO_TIEN_TRINH, initializer=_mo_pak) as pool:
        for (mid, quai, nreg, nnpc, nvacham, nmax) in pool.imap_unordered(_quet_mot_map, viecs):
            ten, cx, cy = thongtin[mid]
            ds = gom_cum(quai, cx * 32, cy * 32)
            ket.append((mid, ten, cx, cy, ds, len(quai)))
            tong_quai += len(quai)
            canhbao = ""
            if nmax >= DAI_QUET - 4:
                canhbao = "  <-- CHAM BIEN QUET, TANG DAI_QUET!"
            print("  map %3d %-28s %5d region, %5d quai, %2d cum (bo %d id trung, imax=%d)%s%s"
                  % (mid, ten.decode("latin-1")[:28], nreg, len(quai), len(ds),
                     nvacham, nmax, "  [DANG BAT]" if mid in batmap else "", canhbao))
            sys.stdout.flush()
    ket.sort(key=lambda t: t[0])

    # ---------------- xuat TXT (client nap luc chay) ----------------
    d = []
    d.append(b"# datau_toado.txt - TOA DO CUM QUAI CAC MAP NHIEM VU DA TAU")
    d.append(b"# SINH TU DONG boi ReverseTools/gen_datau_spots.py - doc file add NPC cua")
    d.append(b"# may chu trong pak (\\maps\\<map>\\v_NNN\\NNN_Region_S.dat).")
    d.append(b"# Auto Da Tau (CoreClient.dll) nap tep nay luc chay de biet cho nao co quai")
    d.append(b"# ma chay toi danh - sua tay duoc, khong can dung lai DLL.")
    d.append(b"#")
    d.append(b"# Cot:  MapID <tab> X <tab> Y <tab> SoQuai      (X/Y = MPS tuyet doi)")
    d.append(b"# Dong bat dau bang # la chu thich.")
    d.append(b"")
    for (mid, ten, cx, cy, ds, nq) in ket:
        d.append(b"# ---- map %d: %s (%d quai, neo nhiem vu %d,%d) %s"
                 % (mid, ten, nq, cx * 32, cy * 32,
                    b"[DANG BAT]" if mid in batmap else b""))
        for (x, y, num) in ds:
            d.append(b"%d\t%d\t%d\t%d" % (mid, x, y, num))
    data = b"\r\n".join(d) + b"\r\n"
    for path in (OUT_TXT, OUT_TXT2):
        try:
            thumuc = os.path.dirname(path)
            if thumuc and not os.path.isdir(thumuc):
                os.makedirs(thumuc)
            open(path, "wb").write(data)
            print("OK ghi %s (%d byte)" % (path, len(data)))
        except IOError as e:
            print("KHONG ghi duoc %s: %s" % (path, e))

    # ---------------- xuat HEADER (ban du phong khi thieu txt) ----------------
    h = []
    h.append("// KDaTauSpots.h - SINH TU DONG boi ReverseTools/gen_datau_spots.py")
    h.append("// BAN DU PHONG: engine uu tien doc settings\\datau_toado.txt luc chay;")
    h.append("// thieu tep do thi dung bang nay (chi cac map DANG BAT cho nhiem vu loai 4).")
    h.append("// Don vi: MPS tuyet doi. SUA TAY LA VO ICH - chay lai bo sinh.")
    h.append("#ifndef KDATAUSPOTS_H")
    h.append("#define KDATAUSPOTS_H")
    h.append("")
    h.append("struct DTSpotRow { int nMapId; int nX; int nY; int nNum; };")
    h.append("static const DTSpotRow g_DTSpot[] = {")
    nrow = 0
    for (mid, ten, cx, cy, ds, nq) in ket:
        if batmap and mid not in batmap:
            continue
        for (x, y, num) in ds:
            h.append("\t{ %d, %d, %d, %d }," % (mid, x, y, num))
            nrow += 1
    if nrow == 0:
        h.append("\t{ 0, 0, 0, 0 },")
    h.append("};")
    h.append("static const int g_nDTSpotCount = sizeof(g_DTSpot)/sizeof(g_DTSpot[0]);")
    h.append("")
    h.append("#endif")
    open(OUT_H, "wb").write(("\r\n".join(h) + "\r\n").encode("ascii"))
    print("OK ghi %s (%d dong du phong)" % (OUT_H, nrow))
    print("TONG: %d map co du lieu, %d quai da quet, %d dong toa do"
          % (len(ket), tong_quai, sum(len(k[4]) for k in ket)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
