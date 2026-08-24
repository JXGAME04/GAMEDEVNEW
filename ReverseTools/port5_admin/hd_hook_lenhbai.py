# -*- coding: utf-8 -*-
import io, os, shutil

p = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\lenhbaiadmin.lua"
s = io.open(p, "r", encoding="latin-1", newline="").read()
NL = "\r\n" if "\r\n" in s else "\n"
if "hoatdong_admin" not in s:
    old = 'Include("\\\\script\\\\item\\\\liendau_admin.lua")'
    assert s.count(old) == 1, "inc anchor = %d" % s.count(old)
    s = s.replace(old, old + NL + 'Include("\\\\script\\\\item\\\\hoatdong_admin.lua")\t-- [24/08] Bang Chien / Bach Nhan / Ty Vo / Thanh Bao: bo test + config', 1)
    old = '/LD_AdminMenu",'
    assert s.count(old) == 1, "menu anchor = %d" % s.count(old)
    s = s.replace(old, old + NL + '\t\t"Hoat dong 23-24/08 (BC-BN-TV-TB): bo test/HD_AdminMenu",', 1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("lenhbaiadmin: menu OK")
else:
    print("lenhbaiadmin: da co")

d = r"D:\GAMEDEVNEW\serverscript_jx2\port5_admin"
if not os.path.isdir(d): os.makedirs(d)
shutil.copyfile(p, d + r"\lenhbaiadmin.lua")
E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
shutil.copyfile(E + r"\script\item\hoatdong_admin.lua", d + r"\hoatdong_admin.lua")
shutil.copyfile(E + r"\script\header\cauhinh_hoatdong.lua", d + r"\cauhinh_hoatdong.lua")
print("mirror port5_admin OK")
