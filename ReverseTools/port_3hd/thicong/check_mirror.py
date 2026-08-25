# -*- coding: utf-8 -*-
"""Kiem GUONG repo (serverscript_jx2\\3hoatdong) co khop BYTE voi cay song E:\\ khong.
Luat du an: 'up git la up CHINH FILE DA SUA' -> guong phai giong het cay song.
Liet ke: THIEU o guong / LECH BYTE / THUA o guong.
"""
import io, os, sys, filecmp
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"

# moi tep trong guong phai co ban tuong ung o cay song va GIONG BYTE
missing, diff, ok = [], [], 0
mirror_rels = []
for dp, dn, fs in os.walk(MIRROR):
    for f in fs:
        full = os.path.join(dp, f)
        rel = os.path.relpath(full, MIRROR)
        mirror_rels.append(rel)
        live = os.path.join(JX1, rel)
        if not os.path.isfile(live):
            missing.append(rel)
        elif open(full, "rb").read() != open(live, "rb").read():
            diff.append(rel)
        else:
            ok += 1

print("Tep trong guong:", len(mirror_rels))
print("  GIONG BYTE cay song :", ok)
print("  LECH BYTE           :", len(diff))
print("  KHONG CO o cay song :", len(missing))
for r in diff:
    a = os.path.getsize(os.path.join(MIRROR, r))
    b = os.path.getsize(os.path.join(JX1, r))
    print("    LECH  %-62s guong=%d song=%d" % (r, a, b))
for r in missing:
    print("    THIEU-O-SONG  %s" % r)

# nguoc lai: tep da sua tren cay song (theo manifest + danh sach sua) co trong guong khong
HERE = os.path.dirname(os.path.abspath(__file__))
man = io.open(os.path.join(HERE, "b1_manifest.txt"), encoding="utf-8").read().split("\n")
extra = [
    r"script\startgame.lua", r"script\timerserver.lua",
    r"script\header\cauhinh_hoatdong.lua", r"script\item\lenhbaiadmin.lua",
    r"script\item\hd3_admin.lua", r"script\global\autoexec_npc_hd3.lua",
    r"script\missions\fengling_ferry\hd3_thuyenphu.lua",
    r"script\tinhnang\3hoatdong\hd3_driver.lua",
    r"settings\task\missions.txt",
    r"script\event\jiefang_jieri\200904\shuizei\shuizei.lua",
]
want = set(x.strip().replace("/", os.sep) for x in man if x.strip()) | set(extra)
have = set(mirror_rels)
notmir = sorted(want - have)
print()
print("Tep DA SUA/CHEP nhung THIEU trong guong:", len(notmir))
for r in notmir:
    print("    THIEU-GUONG  %s" % r)
