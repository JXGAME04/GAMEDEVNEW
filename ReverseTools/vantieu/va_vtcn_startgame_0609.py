# -*- coding: utf-8 -*-
"""patch_startgame.py [VTCN 06/09] - startgame.lua: Include xaphu_thon5.lua + goi addnpcxaphuthon5() sau addnpcnamnhac().
Byte-safe latin-1, sao luu .truoc_vtcn_xaphu_0609."""
import io, os, shutil, sys
sys.stdout.reconfigure(encoding="utf-8")
p = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\startgame.lua"
s = io.open(p, "r", encoding="latin-1", newline="").read()
hb = sum(1 for c in s if ord(c) >= 0x80)
eol = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
reps = [
    ('Include("\\\\script\\\\startgame\\\\thon\\\\namnhactran.lua");' + eol,
     'Include("\\\\script\\\\startgame\\\\thon\\\\namnhactran.lua");' + eol +
     'Include("\\\\script\\\\startgame\\\\thon\\\\xaphu_thon5.lua");\t-- [VTCN 06/09] Xa Phu cho 5 thon/tran con thieu' + eol),
    ("\taddnpcnamnhac()" + eol,
     "\taddnpcnamnhac()" + eol + "\t" + eol + "\taddnpcxaphuthon5()\t-- [VTCN 06/09] Xa Phu Long Tuyen/Dao Huong/Vinh Lac/Chu Tien/Thach Co" + eol),
]
for old, new in reps:
    n = s.count(old)
    if n != 1:
        sys.exit("neo khop %d lan: %r" % (n, old[:60]))
    s = s.replace(old, new)
assert sum(1 for c in s if ord(c) >= 0x80) == hb
bak = p + ".truoc_vtcn_xaphu_0609"
if not os.path.isfile(bak):
    shutil.copyfile(p, bak)
io.open(p, "w", encoding="latin-1", newline="").write(s)
print("startgame.lua OK (eol %r)" % eol)
