# -*- coding: ascii -*-
"""Chep hd3_driver.lua vao JX1 + MIRROR (thu muc tinhnang\\3hoatdong)."""
import os, shutil
SRC = r"C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\0064b491-1b33-4139-acb8-6928fda45bcd\scratchpad\hd3_driver.lua"
JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
rel = os.path.join("script", "tinhnang", "3hoatdong", "hd3_driver.lua")
for base in (JX1, MIRROR):
    dst = os.path.join(base, rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(SRC, dst)
    print("ghi", dst, "->", os.path.isfile(dst), os.path.getsize(dst), "byte")
