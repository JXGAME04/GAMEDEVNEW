# -*- coding: utf-8 -*-
"""VIEM DE - buoc 4b: phan giai 5 ma TRUNG TEN o JX1 bang cach so THUOC TINH
(gia, cap, anh, kich thuoc, script) voi dong tuong ung cua ban Linux.

Khong doan: chon ma co nhieu thuoc tinh trung nhat, va IN RA bang so sanh de
chu game kiem lai. Neu hai ung vien hoa nhau thi bao ro la KHONG QUYET DUOC.
"""
import io
import re
import sys
import importlib.util
import unicodedata

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

spec = importlib.util.spec_from_file_location("dec2", r"D:\GAMEDEVNEW\ReverseTools\port_3hd\dec2.py")
dec2 = importlib.util.module_from_spec(spec)
spec.loader.exec_module(dec2)

REV = {}
for cp in range(0x20, 0x2000):
    ch = chr(cp)
    try:
        b = unicode_to_tcvn3_bytes(ch)
    except Exception:
        continue
    if len(b) == 1 and b[0] >= 0x80:
        REV.setdefault(b[0], ch)


def tv(s):
    return "".join(REV.get(ord(c), c) if ord(c) >= 0x80 else c for c in s)


def chuan(s):
    s = unicodedata.normalize("NFD", s.lower())
    s = "".join(c for c in s if unicodedata.category(c) != "Mn")
    s = s.replace("\u0111", "d")
    return re.sub(r"[^a-z0-9]+", " ", s).strip()


JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item\magicscript.txt"
LNX = r"D:\ServerLinux\server1\settings\item\004\magicscript.txt"

# cot muon so: ten cot JX1 -> chi so cot Linux (Linux dung ten cot tieng Trung
# nhung THU TU cot giong het, xem dong tieu de da giai ma)
COT = ["ImageName", "ObjIdx", "Width", "Height", "Price", "Level", "Script"]
# thu tu cot chuan cua magicscript:
# 0 Name 1 Genre 2 DetailType 3 ParticularType 4 ImageName 5 ObjIdx 6 Width
# 7 Height 8 Intro 9 Series 10 Price 11 Level 12 Stack 13 Script...
IDX = {"ImageName": 4, "ObjIdx": 5, "Width": 6, "Height": 7,
       "Series": 9, "Price": 10, "Level": 11, "Stack": 12, "Script": 13}

CAN_XU = ["6,1,3203", "6,1,4134", "6,1,30229", "6,1,30289", "6,1,3810", "6,1,3811"]


def doc_jx1():
    rows = [r.rstrip("\r").split("\t") for r in io.open(JX1, encoding="latin-1").read().split("\n")]
    ra = []
    for c in rows[1:]:
        if len(c) < 14:
            continue
        ra.append([tv(x) for x in c])
    return ra


def doc_lnx():
    ra = {}
    for r in open(LNX, "rb").read().split(b"\n")[1:]:
        c = dec2.decline2(r.rstrip(b"\r")).split("\t")
        if len(c) < 14:
            continue
        ra["%s,%s,%s" % (c[1], c[2], c[3])] = c
    return ra


def main():
    jx1 = doc_jx1()
    lnx = doc_lnx()
    theo_ten = {}
    for c in jx1:
        theo_ten.setdefault(chuan(c[0]), []).append(c)

    for ma in CAN_XU:
        L = lnx.get(ma)
        if not L:
            print("=== %s : khong co o ban Linux, bo qua" % ma)
            continue
        ung = theo_ten.get(chuan(L[0]), [])
        print()
        print("=== Linux %-10s  %s ===" % (ma, L[0]))
        if len(ung) <= 1:
            print("    chi 1 ung vien -> %s" % (",".join(ung[0][1:4]) if ung else "<< JX1 KHONG CO"))
            continue
        print("    %-12s %-9s %-40s %-6s %-5s %-4s %-4s %s"
              % ("nguon", "ma", "anh", "gia", "cap", "W", "H", "script"))
        print("    %-12s %-9s %-40s %-6s %-5s %-4s %-4s %s"
              % ("LINUX", ma, L[IDX["ImageName"]][-38:], L[IDX["Price"]], L[IDX["Level"]],
                 L[IDX["Width"]], L[IDX["Height"]], L[IDX["Script"]][-30:]))
        diem = []
        for c in ung:
            d = 0
            ly = []
            for k in ("ImageName", "Price", "Level", "Width", "Height", "Script"):
                if L[IDX[k]].strip() and L[IDX[k]].strip() == c[IDX[k]].strip():
                    d += 1
                    ly.append(k)
            diem.append((d, c, ly))
            print("    %-12s %-9s %-40s %-6s %-5s %-4s %-4s %s"
                  % ("JX1", ",".join(c[1:4]), c[IDX["ImageName"]][-38:], c[IDX["Price"]],
                     c[IDX["Level"]], c[IDX["Width"]], c[IDX["Height"]], c[IDX["Script"]][-30:]))
        diem.sort(key=lambda x: -x[0])
        if len(diem) >= 2 and diem[0][0] == diem[1][0]:
            print("    >> KHONG QUYET DUOC: %d ung vien cung %d thuoc tinh trung -- CAN CHU GAME CHON"
                  % (sum(1 for d in diem if d[0] == diem[0][0]), diem[0][0]))
        else:
            print("    >> CHON %s  (trung %d thuoc tinh: %s)"
                  % (",".join(diem[0][1][1:4]), diem[0][0], ", ".join(diem[0][2])))


main()
