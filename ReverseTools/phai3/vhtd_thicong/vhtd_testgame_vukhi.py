# -*- coding: ascii -*-
"""vhtd_testgame_vukhi.py [VHTD 02/09d] - NPC test Ba Lang Huyen (header\\testgame.lua, ham selvk 'Vu khi he X'):
  vong can chien for i=0,5 -> 0,8 (them Particular 6 Trien Thu, 7 Dao Thuan, 8 Thuan Dao - meleeweapon.txt 61..90)
  vong tam xa  for i=0,2 -> 0,3 (them Particular 3 Moc Cam - rangeweapon.txt 31..40)
Doc/ghi latin-1, ban luu .truoc_vhtd_0209, idempotent. DUNG: python vhtd_testgame_vukhi.py [--kiem]
"""
import io, os, sys, shutil
KIEM = "--kiem" in sys.argv
P = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\header\testgame.lua"
BAK = ".truoc_vhtd_0209"
s = io.open(P, "r", encoding="latin-1", newline="").read()
nl = "\r\n" if "\r\n" in s else "\n"
i0 = s.find("function selvk(")
i1 = s.find(nl + "function ", i0 + 10)
if i0 < 0 or i1 < 0: raise SystemExit("khong thay selvk")
body = s[i0:i1]
n = 0
for old, new, tag in (
    ("for i=0,5 do" + nl + "\tgenre,detail,parti,level,series = 0,0,i,10,sel;",
     "for i=0,8 do\t-- [VHTD 02/09d] 0..5 vu khi cu + 6 Trien Thu, 7 Dao Thuan, 8 Thuan Dao (Vu Hon; meleeweapon.txt 61..90)" + nl + "\tgenre,detail,parti,level,series = 0,0,i,10,sel;",
     "can chien 0..8"),
    ("for i=0,2 do" + nl + "\tgenre,detail,parti,level,series = 0,1,i,10,sel;",
     "for i=0,3 do\t-- [VHTD 02/09d] 0..2 vu khi tam xa cu + 3 Moc Cam (Tieu Dao; rangeweapon.txt 31..40)" + nl + "\tgenre,detail,parti,level,series = 0,1,i,10,sel;",
     "tam xa 0..3")):
    if new in body: print("  [=] selvk da ap (%s)" % tag); continue
    if body.count(old) != 1: raise SystemExit("anchor %s: %d lan" % (tag, body.count(old)))
    body = body.replace(old, new); n += 1; print("  [+] selvk: %s" % tag)
s2 = s[:i0] + body + s[i1:]
if s2 != s:
    if sum(1 for c in s2 if ord(c) >= 0x80) != sum(1 for c in s if ord(c) >= 0x80): raise SystemExit("lech byte cao")
    if not KIEM:
        if not os.path.exists(P + BAK): shutil.copy2(P, P + BAK)
        io.open(P, "w", encoding="latin-1", newline="").write(s2)
    print("  => ghi testgame.lua (%d cho)%s" % (n, " KIEM" if KIEM else ""))
print("XONG.")
