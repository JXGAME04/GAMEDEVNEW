# -*- coding: ascii -*-
"""
gen_datau_spots.py - SINH BANG DIEM QUAI CHO AUTO DA TAU (loai 4)

VI SAO CAN: client CHI thay NPC da duoc may chu dong bo (~2 man hinh). Khi Xa Phu
tha nhan vat xuong map nhiem vu, quanh do thuong KHONG co quai nao trong tam dong
bo -> vong quet cua auto (DT_FindFarMob) khong thay gi -> dung yen / di mo.
Nguon SU THAT ve cho quai dung o dau la file add NPC cua may chu nam TRONG PAK:
    \\maps\\<duong dan map>\\v_<nY>\\<nX>_Region_S.dat  (muc REGION_NPC_FILE_INDEX)
Bo sinh nay doc thang pak do, gom toa do quai (kind_normal) cua 14 map nhiem vu
Da Tau roi ket thanh cum, xuat ra Sources/Core/Src/KDaTauSpots.h de engine di den
DUNG cho co quai thay vi do dam.

Doc:
  - dinh dang pak  : Sources/Engine/Src/XPackFile.cpp (header 32B, index 16B/muc)
  - bam ten tep    : KPakList::FileNameToId (KPakList.cpp:72-85)
  - giai nen       : ucl_nrv2b_decompress_8 (Sources/Engine/Src/ucl/n2b_d.c + getbit.h)
  - duong dan map  : KSubWorld.cpp:1756 + KRegion::LoadObject (KRegion.cpp:136-180)
  - cau truc NPC   : Scene/SceneDataDef.h (KNpcFileHead + KSPNpc)
Chay: python D:\\GAMEDEVNEW\\ReverseTools\\gen_datau_spots.py
"""
import os, struct, sys

SRV  = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
PAKS = ["maps.pak", "maps_error.pak", "namcung.pak", "maps_tieu_bang_chien.pak"]
INI  = os.path.join(SRV, "settings", "MapList.ini")
OUT  = r"D:\GAMEDEVNEW\Sources\Core\Src\KDaTauSpots.h"
TBL  = r"D:\GAMEDEVNEW\Sources\Core\Src\KDaTauTables.h"

M32 = 0xFFFFFFFF
KIND_NORMAL = 0          # GameDataDef.h:1369 kind_normal = quai thuong
CUM = 1024               # canh o gom cum (mps) ~ 32 o luoi
MAX_SPOT = 24            # so cum giu lai moi map
MIN_QUAI = 3             # cum it hon nay thi bo (le te)

# ---------------------------------------------------------------- pak

def name_to_id(s):
    """KPakList::FileNameToId - char la SIGNED, so hoc 32-bit khong dau."""
    uid = 0
    idx = 0
    for b in s:
        c = b if b < 0x80 else b - 0x100
        if 0x41 <= b <= 0x5A:                    # g_StrLower: 'A'-'Z' -> thuong
            c = b + 0x20
        idx += 1
        uid = ((((uid + idx * c) & M32) % 0x8000000b) * 0xffffffef) & M32
    return uid ^ 0x12345678


def ucl_nrv2b_decompress_8(src, out_len):
    """Ban Python cua ucl_nrv2b_decompress_8 (n2b_d.c + getbit_8 trong getbit.h)."""
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

    def get(self, name_bytes):
        u = name_to_id(name_bytes)
        e = self.idx.get(u)
        if not e:
            return None
        off, size, cflag = e
        method = (cflag >> 24) & 0xFF
        csize = cflag & 0xFFFFFF
        self.f.seek(off)
        if method == 0:                      # TYPE_NONE
            return self.f.read(size)
        raw = self.f.read(csize)             # TYPE_UCL(0x01) / VNG(0x20) deu la nrv2b
        return ucl_nrv2b_decompress_8(raw, size)


# ---------------------------------------------------------------- du lieu

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


def doc_map_nhiemvu(path):
    """Lay 14 map loai 4 tu chinh bang da sinh truoc do (g_DTQuestMap)."""
    txt = open(path, "rb").read().decode("latin-1")
    i = txt.find("g_DTQuestMap[]")
    j = txt.find("};", i)
    rows = []
    for line in txt[i:j].split("\n"):
        line = line.strip()
        if not line.startswith("{"):
            continue
        p = line.strip("{},\r\n\t ").split(",")
        if len(p) < 3:
            continue
        try:
            rows.append((int(p[0]), int(p[1]), int(p[2])))
        except ValueError:
            pass
    return rows


def doc_npc_region(blob):
    """Region_S.dat hop nhat -> danh sach (x, y, kind, level, ten) cua NPC trong region."""
    if len(blob) < 4:
        return []
    (num_sect,) = struct.unpack_from("<I", blob, 0)
    if num_sect < 3 or num_sect > 64:
        return []
    head_size = 4 + 8 * num_sect
    if len(blob) < head_size:
        return []
    # REGION_NPC_FILE_INDEX = 2 (SceneDataDef.h:30)
    off, length = struct.unpack_from("<II", blob, 4 + 8 * 2)
    beg = head_size + off
    if length < 12 or beg + length > len(blob):
        return []
    (num_npc,) = struct.unpack_from("<I", blob, beg)
    p = beg + 12                            # KNpcFileHead = uNumNpc + uReserved[2]
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


def main():
    if not os.path.exists(INI):
        print("KHONG THAY %s - can cay may chu" % INI)
        return 1
    paks = []
    for pk in PAKS:
        full = os.path.join(SRV, "Pak", pk)
        if os.path.exists(full):
            paks.append(Pak(full))
    if not paks:
        print("KHONG THAY pak nao trong %s\\Pak" % SRV)
        return 1
    print("pak nap: %s" % ", ".join("%s(%d muc)" % (p.name, len(p.idx)) for p in paks))

    duong = doc_maplist(INI)
    maps = doc_map_nhiemvu(TBL)
    print("map nhiem vu (g_DTQuestMap): %d" % len(maps))

    ket = []          # (mapid, [(x, y, num), ...])
    tong_quai = 0
    for mid, ax, ay in maps:
        p = duong.get(mid)
        if not p:
            print("  map %3d: KHONG co trong MapList.ini - bo qua" % mid)
            ket.append((mid, []))
            continue
        quai = []
        nregion = 0
        loi = 0
        for ny in range(0, 256):
            for nx in range(0, 256):
                nm = b"\\maps\\%s\\v_%03d\\%03d_region_s.dat" % (p, ny, nx)
                blob = None
                for pk in paks:
                    try:
                        blob = pk.get(nm)
                    except Exception:
                        blob = None
                        loi += 1
                    if blob:
                        break
                if not blob:
                    continue
                nregion += 1
                for (px, py, kind, lvl, name) in doc_npc_region(blob):
                    if kind != KIND_NORMAL:
                        continue
                    if px <= 0 or py <= 0:
                        continue
                    quai.append((px, py))
        # gom cum theo o CUM x CUM
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
        # gan neo nhiem vu len dau (diem Xa Phu tha xuong) neu no gan mot cum
        ds.sort(key=lambda t: (t[0] - ax * 32) ** 2 + (t[1] - ay * 32) ** 2)
        ket.append((mid, ds))
        tong_quai += len(quai)
        if quai:
            xs = [q[0] for q in quai]
            ys = [q[1] for q in quai]
            print("  map %3d: %4d region, %5d quai, %2d cum | quai x=%d..%d y=%d..%d"
                  " | neo nhiem vu (%d,%d)%s"
                  % (mid, nregion, len(quai), len(ds), min(xs), max(xs), min(ys), max(ys),
                     ax * 32, ay * 32,
                     "" if (min(xs) <= ax * 32 <= max(xs)) else "  <-- NEO NGOAI VUNG QUAI"))
        else:
            print("  map %3d: %4d region, KHONG co quai (%d loi giai nen)" % (mid, nregion, loi))

    # ---- xuat header ----
    dong = []
    dong.append("// KDaTauSpots.h - SINH TU DONG boi ReverseTools/gen_datau_spots.py")
    dong.append("// Toa do CUM QUAI cua 14 map nhiem vu Da Tau loai 4, doc thang tu file")
    dong.append("// add NPC cua may chu trong pak (\\maps\\<map>\\v_NNN\\NNN_Region_S.dat).")
    dong.append("// Don vi: MPS tuyet doi (dung thang, KHONG nhan 32 nua).")
    dong.append("// SUA TAY LA VO ICH - chay lai bo sinh khi may chu doi bang map.")
    dong.append("#ifndef KDATAUSPOTS_H")
    dong.append("#define KDATAUSPOTS_H")
    dong.append("")
    dong.append("struct DTSpotRow { int nMapId; int nX; int nY; int nNum; };")
    dong.append("static const DTSpotRow g_DTSpot[] = {")
    for mid, ds in ket:
        for (x, y, num) in ds:
            dong.append("\t{ %d, %d, %d, %d }," % (mid, x, y, num))
    dong.append("};")
    dong.append("static const int g_nDTSpotCount = sizeof(g_DTSpot)/sizeof(g_DTSpot[0]);")
    dong.append("")
    dong.append("#endif")
    data = ("\r\n".join(dong) + "\r\n").encode("ascii")
    open(OUT, "wb").write(data)
    tong_cum = sum(len(d[1]) for d in ket)
    print("OK: ghi %s (%d dong cum / %d map, %d quai da quet)"
          % (OUT, tong_cum, len(ket), tong_quai))
    return 0


if __name__ == "__main__":
    sys.exit(main())
