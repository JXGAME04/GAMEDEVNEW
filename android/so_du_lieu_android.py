# -*- coding: utf-8 -*-
r"""[ANDROID 10/09] Tim tep CHI-ANDROID trong D:\jx1_android_data: khac hoac khong co trong cay client PC.

Thu muc du lieu Android duoc chuan_bi_du_lieu.ps1 sinh tu cay client PC (giu nguyen mtime) nen KHONG the
dua vao ngay sua. Cach chac: so noi dung voi cay nguon (NTFS khong phan biet hoa/thuong nen ten chu thuong
van mo duoc). Chi quet cac nhanh hay bi sua tay (ui, spr/ui*, spr/npcres, spr/*.spr, settings, script)
- maps/data qua lon va khong ai sua cho Android.

Dung: python android\so_du_lieu_android.py  [-Nguon ...] [-Dich ...]
Ket qua: in ra danh sach "THIEU o PC" (tep them rieng cho Android) va "KHAC" (sua rieng cho Android).
Moi tep trong hai danh sach nay PHAI co trong android\du_lieu_ghi_de\ (xem BANGIAO §12.11).
"""
import hashlib
import os
import sys

NGUON = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
DICH = r"D:\jx1_android_data"
GHI_DE = os.path.join(os.path.dirname(os.path.abspath(__file__)), "du_lieu_ghi_de")
NHANH = ["ui", "spr/ui3", "spr/ui4", "spr/uinew", "spr/npcres", "settings", "script"]
BO_QUA_DUOI = (".log", ".bak", ".tmp", ".__tmp__")

a = sys.argv[1:]
for i in range(0, len(a) - 1, 2):
    if a[i].lower() == "-nguon":
        NGUON = a[i + 1]
    if a[i].lower() == "-dich":
        DICH = a[i + 1]


def bam(p):
    h = hashlib.md5()
    with open(p, "rb") as f:
        for kh in iter(lambda: f.read(1 << 20), b""):
            h.update(kh)
    return h.hexdigest()


thieu, khac, so = [], [], 0
for nh in NHANH:
    goc = os.path.join(DICH, nh.replace("/", os.sep))
    if not os.path.isdir(goc):
        continue
    for thu_muc, _, cac_tep in os.walk(goc):
        for t in cac_tep:
            if t.lower().endswith(BO_QUA_DUOI):
                continue
            p = os.path.join(thu_muc, t)
            rel = os.path.relpath(p, DICH)
            q = os.path.join(NGUON, rel)
            so += 1
            if not os.path.isfile(q):
                thieu.append(rel)
            elif os.path.getsize(p) != os.path.getsize(q):
                khac.append(rel)
            elif os.path.getsize(p) < (2 << 20) and bam(p) != bam(q):   # tep to (npcres) chi so co
                khac.append(rel)
# tep o goc (config.ini, package.ini)
for t in ["config.ini", "package.ini"]:
    p, q = os.path.join(DICH, t), os.path.join(NGUON, t)
    if os.path.isfile(p) and (not os.path.isfile(q) or bam(p) != bam(q)):
        khac.append(t)

print("da so %d tep trong %s" % (so, ", ".join(NHANH)))
print("\n== THIEU o cay PC (them rieng cho Android): %d" % len(thieu))
for r in sorted(thieu):
    co = "  [co trong lop ghi de]" if os.path.isfile(os.path.join(GHI_DE, r)) else "  <<< CHUA CO trong lop ghi de"
    print("  %-60s %8d%s" % (r, os.path.getsize(os.path.join(DICH, r)), co))
print("\n== KHAC cay PC (sua rieng cho Android): %d" % len(khac))
for r in sorted(khac):
    co = "  [co trong lop ghi de]" if os.path.isfile(os.path.join(GHI_DE, r)) else "  <<< CHUA CO trong lop ghi de"
    print("  %-60s %8d%s" % (r, os.path.getsize(os.path.join(DICH, r)), co))
