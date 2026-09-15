# -*- coding: latin-1 -*-
r"""[ONENDEN 14/09] Sinh \system\spr\RegionTileDefault.spr - o nen mac dinh con THIEU trong du lieu goc.

TRIEU CHUNG (chu bao 22:1x 14/09): "vao game qua map bi den man va di chuyen cung bi chop man den".

NGUYEN NHAN (do va tra tung buoc, khong doan):
  - KScenePlaceRegionC::PrerenderGround ghep nen mot vung: xoa anh 512x512 ve 0 (den) roi ve tung o nen
    bang DrawPrimitivesOnImage. O nao GetImage tra NULL thi BO QUA (KRepresentShell3.cpp:3035) -> o do
    giu nguyen mau den.
  - Nhieu vung cua ban do tro toi "\system\spr\RegionTileDefault.spr" (o nen MAC DINH ma trinh soan ban do
    dat cho o chua ve). Tep nay KHONG CO o dau: khong trong 13 pak dien thoai, khong trong 41 pak cay client
    PC, khong co thu muc \system tren ca hai cay. Ban PC cung den y het - day la thieu du lieu goc, khong
    phai loi cua ban mobile.
  - Nhat ky chi thay vai dong "LoadImage FAIL ... RegionTileDefault.spr" vi TextureResMgr::GetImage nho muc
    NULL va CHI thu nap lai moi 10 giay (REP3_RELOAD_COOLDOWN), 3 lan hong thi 10 phut -> hang tram o bi bo
    chi ghi duoc 3-4 dong.

DO DUOC (doc that tu pak, giai nen UCL - scratchpad\onen4.py / vung_den_o_dau.py):
  map 78  Tuong Duong : 6/727 vung co o thieu - (86,92) 64/64, (79,109) 56/58, (121,97) 53/64,
                        (173,0) 48/64, (98,104) 1/64 (o den giua thanh), (81,108) 1/19
  map 379 Xung phong  : 6/305 vung - (103,101) 63/64, (89,108) 63/64, (90,107) 59/64, (86,110) 55/64,
                        (73,112) 54/64, (68,210) 38/64
  map 93 / map 324    : 0 o thieu (nen chu khong thay den o hai ban do Tong Kim truoc do)
  Khop voi may that: dong [PGND-V] cua Fold 7 luc 22:23 co "66 anh ... bo 56" va "64 anh ... bo 19",
  dung bang co cac vung tren; cac phien truoc do (chi o map 324/379 vung sach) deu "bo 0".

CACH SUA: dat CHINH tep ma engine hoi. Lay mot o nen 64x64 co san lam anh mac dinh:
  \游戏资源\室外地表\中型地表图素\黄稀.spr  (o nen dung NHIEU NHAT quanh cac vung den:
  361 luot, gap 3,6 lan o dung thu hai) - 64x64, 27 khung, 256 mau. Cac o thieu deu xin KHUNG 0.
  Ket qua: o den thanh nen dat nhu vung ben canh thay vi lo den. Khong doi ma nguon, khong doi hanh vi ve.

CHAY:
  python android\sinh_o_nen_mac_dinh.py                 # sinh vao lop ghi de cua kho ma nguon
  python android\sinh_o_nen_mac_dinh.py --dich <thumuc>  # sinh them vao mot thu muc du lieu (vd dt_v4)
Sau khi ghi vao D:\jx1_android_data_dt_v4 PHAI chay:  python android\may_chu_tai_du_lieu.py --chi-manifest
"""
import argparse
import glob
import os
import struct
import sys

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SEP = chr(92)

# \游戏资源\室外地表\中型地表图素\黄稀.spr  (GBK, viet bang byte de khong phu thuoc bang ma cua trinh soan)
TEN_NGUON = (SEP + "\xd3\xce\xcf\xb7\xd7\xca\xd4\xb4" + SEP + "\xca\xd2\xcd\xe2\xb5\xd8\xb1\xed"
             + SEP + "\xd6\xd0\xd0\xcd\xb5\xd8\xb1\xed\xcd\xbc\xcb\xd8" + SEP + "\xbb\xc6\xcf\xa1.spr")
TEN_DICH = os.path.join("system", "spr", "RegionTileDefault.spr")

KHO_PAK = [
    r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\data",
    r"D:\jx1_android_data_dt_v4\data",
]


def _fid(ten):
    """KPakList::FileNameToId - char CO DAU tren MSVC, chi ha A-Z."""
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


def _giai(f, off, size, cf):
    """XPACK_METHOD: byte cao = kieu nen (0 khong, 1 UCL, 2 bzip2), 3 byte thap = co sau nen."""
    cs = cf & 0xFFFFFF
    fl = cf >> 24
    if fl == 0x20:
        fl = 1
    fl &= 0x0F
    f.seek(off)
    blob = f.read(cs if cs else max(size, 0))
    if fl == 0 or cs in (0, size):
        return blob[:size] if size > 0 else blob
    if fl == 1:
        sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
        import ucl
        return ucl.nrv2b_decompress_8(blob, size)
    if fl == 2:
        import bz2
        return bz2.decompress(blob)
    return None


def lay_anh_nguon():
    uid = _fid(TEN_NGUON)
    for thu_muc in KHO_PAK:
        for p in sorted(glob.glob(os.path.join(thu_muc, "*.pak"))):
            with open(p, "rb") as f:
                h = f.read(16)
                if len(h) < 16:
                    continue
                sig, count, ioff, doff = struct.unpack("<IIII", h)
                if sig != 0x4B434150:
                    continue
                f.seek(ioff)
                raw = f.read(count * 16)
                for i in range(count):
                    u, off, size, cf = struct.unpack_from("<IIiI", raw, i * 16)
                    if u != uid:
                        continue
                    d = _giai(f, off, size, cf)
                    if d and len(d) >= 32 and d[:3] == b"SPR":
                        return d, p
    return None, None


def kiem(d):
    w, h, cx, cy, khung, mau, huong, nhip = struct.unpack_from("<8H", d, 4)
    if khung < 1:
        raise SystemExit("HONG: anh nguon khong co khung nao")
    if w != 64 or h != 64:
        raise SystemExit("HONG: o nen trung binh phai la 64x64, dang la %dx%d" % (w, h))
    return w, h, khung, mau


def ghi(d, thu_muc, ha_chu_thuong=False):
    """ha_chu_thuong: thu muc du lieu DA SINH (dt_v4) deu la ten thuong (chuan_bi_du_lieu.ps1 ha A-Z),
    va KFile::Open tren Android/iOS thu duong dan TOAN CHU THUONG truoc (KFile.cpp:110-119)."""
    p = os.path.join(thu_muc, TEN_DICH.lower() if ha_chu_thuong else TEN_DICH)
    os.makedirs(os.path.dirname(p), exist_ok=True)
    if os.path.isfile(p) and open(p, "rb").read() == d:
        print("  bo qua (da dung): %s" % p)
        return
    with open(p, "wb") as f:
        f.write(d)
    print("  da ghi: %s (%d byte)" % (p, len(d)))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dich", action="append", default=[],
                    help="thu muc du lieu ghi them (vd D:\\jx1_android_data_dt_v4)")
    a = ap.parse_args()
    d, tu = lay_anh_nguon()
    if d is None:
        raise SystemExit("HONG: khong tim thay anh nguon trong cac kho pak: %s" % ", ".join(KHO_PAK))
    w, h, khung, mau = kiem(d)
    print("anh nguon: %s -> %dx%d, %d khung, %d mau, %d byte" % (tu, w, h, khung, mau, len(d)))
    ghi(d, os.path.join(GOC, "android", "du_lieu_ghi_de"))
    for t in a.dich:
        ghi(d, t, ha_chu_thuong=True)
    print("xong.")


if __name__ == "__main__":
    main()
