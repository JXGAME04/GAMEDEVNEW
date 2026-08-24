# -*- coding: utf-8 -*-
r"""Quet moi chuoi lua chon SayEx co NHIEU HON MOT dau '/'.
Engine tach bang strstr(pAnswer,"/") = dau '/' DAU TIEN -> nhan CAM chua '/'.
"""
import io, re, sys

FILES = [
    r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\hoatdong_admin.lua",
    r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\lenhbaiadmin.lua",
]

for p in FILES:
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    print("=== " + p.split("\\")[-1])
    for i, line in enumerate(s.split("\n")):
        # chi xet dong nam trong danh sach lua chon: co chuoi "..../TenHam"
        for m in re.finditer(r'"([^"]*)"', line):
            t = m.group(1)
            if t.count("/") >= 2 and re.search(r'/[A-Za-z_]\w*"?$', t):
                print("  LOI dong %d: %r" % (i+1, t))
            elif t.count("/") == 1 and not re.search(r'/[A-Za-z_]\w*$', t):
                # co '/' nhung khong phai dang '/Ham' -> co the la tieu de (an toan) hoac loi
                pass
