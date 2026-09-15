# -*- coding: latin-1 -*-
r"""[ONENDEN 14/09] Soi o nen THIEU TEP cua mot ban do: vung nao se ra mang den tren man hinh.

Dung de kiem chung / kiem lai sau nay. Doc THAT tu pak (co giai nen UCL/bzip2), khong quet byte tho.

Bo cuc da doc tu ma nguon:
  KScenePlaceRegionC::Load:87        "<map>\v_%03d\%03d_" + REGION_COMBIN_FILE_NAME_CLIENT ("Region_C.dat")
  = uMaxElemFile(u32) + KCombinFileSection[uMaxElemFile]{uOffset,uLength}; doan nen = chi so 4
  KScenePlaceRegionC::LoadGroundLayer: KGroundFileHead{uNumGrunode,uNumObject,uObjectDataOffset}
  + uNumGrunode x KSPRCrunode{h,v,nFrame,nFileNameLen : 4 x u16} + ten (nFileNameLen byte)
  KRepresentShell3.cpp:3035          GetImage NULL hoac nFrame >= so khung -> BO O DO (giu mau den)

Dung:
  python android\soi_o_nen_thieu.py --map 78 93 379           # so hieu trong settings\MapList.ini
  python android\soi_o_nen_thieu.py --map 78 --du-lieu D:\jx1_android_data_dt_v4
"""
import argparse
import glob
import io
import os
import struct
import sys

SEP = chr(92)
GROUND_IDX = 4
MAPLIST = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\settings\MapList.ini"


def _fid(ten):
    b = ten.encode("latin-1")
    if b[:1] not in (b"\x5c", b"/"):
        b = b"\x5c" + b
    uid = 0
    for i, x in enumerate(b):
        c = x - 256 if x >= 128 else x
        if 65 <= x <= 90:
            c = x + 32
        uid = (((uid + (i + 1) * c) & 0xFFFFFFFF) % 0x8000000B) * 0xFFFFFFEF & 0xFFFFFFFF
    return uid ^ 0x12345678


def mo_kho(thu_muc):
    ra = []
    for p in sorted(glob.glob(os.path.join(thu_muc, "*.pak"))):
        f = open(p, "rb")
        h = f.read(16)
        if len(h) < 16:
            f.close()
            continue
        sig, count, ioff, doff = struct.unpack("<IIII", h)
        if sig != 0x4B434150:
            f.close()
            continue
        f.seek(ioff)
        raw = f.read(count * 16)
        idx = {}
        for i in range(count):
            u, off, size, cf = struct.unpack_from("<IIiI", raw, i * 16)
            idx[u] = (off, size, cf)
        ra.append((os.path.basename(p), f, idx))
    return ra


def tim(kho, ten):
    u = _fid(ten)
    for nb, f, idx in kho:
        if u in idx:
            return nb, f, idx[u]
    return None, None, None


def giai(f, vt):
    off, size, cf = vt
    cs = cf & 0xFFFFFF
    fl = cf >> 24
    if fl == 0x20:
        fl = 1
    fl &= 0x0F
    f.seek(off)
    blob = f.read(cs if cs else max(size, 0))
    if fl == 0 or cs in (0, size):
        return blob[:size] if size > 0 else blob
    try:
        if fl == 1:
            sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
            import ucl
            return ucl.nrv2b_decompress_8(blob, size)
        if fl == 2:
            import bz2
            return bz2.decompress(blob)
    except Exception:
        return None
    return None


def lop_nen(d):
    """tra danh sach (ten o nen, chi so khung) hoac None neu khong phai tep vung"""
    if not d or len(d) < 4:
        return None
    n = struct.unpack_from("<I", d, 0)[0]
    if n == 0 or n > 64:
        return None
    sec = [struct.unpack_from("<II", d, 4 + i * 8) for i in range(n)]
    if GROUND_IDX >= n:
        return None
    off, ln = sec[GROUND_IDX]
    if not ln:
        return []
    g = 4 + 8 * n + off
    if g + 12 > len(d):
        return None
    nGru, nObj, oObj = struct.unpack_from("<III", d, g)
    if nGru > 100000:
        return None
    ra = []
    p = g + 12
    for _ in range(nGru):
        if p + 8 > len(d):
            break
        h, v, fr, ln2 = struct.unpack_from("<HHHH", d, p)
        p += 8
        if ln2 > 160 or p + ln2 > len(d):
            break
        ra.append((d[p:p + ln2].decode("latin-1").rstrip(chr(0)), fr))
        p += ln2
    return ra


def co_tep_roi(du_lieu, ten):
    """KPakFile::Open thu DIA truoc roi moi den pak; KFile::Open tren Android thu ten TOAN CHU THUONG
    truoc (KFile.cpp:110-119). Nen mot tep roi trong thu muc du lieu THANG pak."""
    rel = ten.lstrip(SEP).replace(SEP, os.sep)
    for v in (rel.lower(), rel):
        if os.path.isfile(os.path.join(du_lieu, v)):
            return True
    return False


def duong_map(so):
    s = io.open(MAPLIST, encoding="latin-1", newline="").read().split("\n")
    dau = "%d=" % so
    ten = "%d_name=" % so
    d = n = None
    for L in s:
        if L.startswith(dau):
            d = L[len(dau):].strip()
        elif L.startswith(ten):
            n = L[len(ten):].strip()
    return d, n


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--map", type=int, nargs="+", required=True)
    ap.add_argument("--du-lieu", default=r"D:\jx1_android_data_dt_v4")
    a = ap.parse_args()
    kho = mo_kho(os.path.join(a.du_lieu, "data"))
    if not kho:
        raise SystemExit("khong mo duoc pak nao trong %s" % os.path.join(a.du_lieu, "data"))
    print("mo %d pak trong %s" % (len(kho), a.du_lieu))
    for so in a.map:
        d, n = duong_map(so)
        if not d:
            print("map %d: khong co trong MapList.ini" % so)
            continue
        goc = SEP + "maps" + SEP + d.lstrip(SEP)
        mau = goc + SEP + "v_%03d" + SEP + "%03d_Region_C.dat"
        tong_vung = tong_o = 0
        xau = []
        thieu_ten = {}
        for y in range(256):
            for x in range(256):
                nb, f, vt = tim(kho, mau % (y, x))
                if not nb:
                    continue
                o = lop_nen(giai(f, vt))
                if not o:
                    continue
                tong_vung += 1
                tong_o += len(o)
                th = 0
                for t, fr in o:
                    if tim(kho, t)[0] is None and not co_tep_roi(a.du_lieu, t):
                        th += 1
                        thieu_ten[t] = thieu_ten.get(t, 0) + 1
                if th:
                    xau.append((x, y, th, len(o)))
        print("")
        print("map %d [%s]: %d vung, %d o nen, %d vung co o thieu"
              % (so, (n or "?"), tong_vung, tong_o, len(xau)))
        xau.sort(key=lambda v: -v[2])
        for x, y, th, tt in xau[:10]:
            print("    vung (%3d,%3d): %3d/%3d o THIEU (%.0f %%)" % (x, y, th, tt, 100.0 * th / tt))
        for t, c in sorted(thieu_ten.items(), key=lambda kv: -kv[1]):
            print("    tep thieu: %-52s %d o" % (t.encode("latin-1").decode("gbk", "replace"), c))
        if not xau:
            print("    khong con o nen nao thieu tep.")


if __name__ == "__main__":
    main()
