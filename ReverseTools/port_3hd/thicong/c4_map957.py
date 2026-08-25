# -*- coding: ascii -*-
"""C4 - A7-N2: nap map 957 (Mat Phong cua ai - che do chuangguan30) + bang NPC.

MapList.ini cua JX1 DA CO du block 957 (ke ca NewWorldParam USETOWNP_OFF|HEART_OFF
- engine da doc tu WLLS 20/08, KSubWorld.cpp:1819-1825). Chi thieu:
  1) dong World910=957 + Count=911 trong Maps\\WorldSet_GameServer.ini
  2) settings\\maps\\liandandong\\npc_3.txt (bang NPC cua Mat Phong, tu goc A Linux)
"""
import io, os, shutil, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
LNX = r"D:\ServerLinux\server1"

# ---- 1) WorldSet ----
p = os.path.join(JX1, "Maps", "WorldSet_GameServer.ini")
d = io.open(p, encoding="latin-1", newline="").read()
if "=957" in d:
    print("WorldSet: da co 957")
else:
    bak = p + ".truoc_3hd_2508"
    if not os.path.exists(bak):
        shutil.copyfile(p, bak)
    assert d.count("Count=910") == 1, "Count?"
    d = d.replace("Count=910", "Count=911")
    anchor = "World909=984 --- Bang hoi thanh bao [TONGCASTLE 23/08]\r\n"
    assert d.count(anchor) == 1, d.count(anchor)
    d = d.replace(anchor, anchor + "World910=957 --- Mat Phong cua ai (chuangguan30) [3HD 25/08]\r\n")
    io.open(p, "w", encoding="latin-1", newline="").write(d)
    m = os.path.join(MIRROR, "Maps", "WorldSet_GameServer.ini")
    os.makedirs(os.path.dirname(m), exist_ok=True)
    io.open(m, "w", encoding="latin-1", newline="").write(d)
    print("WorldSet: +World910=957, Count=911 (backup .truoc_3hd_2508)")

# ---- 2) npc_3.txt ----
src = os.path.join(LNX, "settings", "maps", "liandandong", "npc_3.txt")
dst = os.path.join(JX1, "settings", "maps", "liandandong", "npc_3.txt")
if os.path.exists(dst):
    print("npc_3.txt: da co")
else:
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(src, dst)
    m = os.path.join(MIRROR, "settings", "maps", "liandandong", "npc_3.txt")
    os.makedirs(os.path.dirname(m), exist_ok=True)
    shutil.copyfile(src, m)
    print("npc_3.txt: da chep (%d byte)" % os.path.getsize(dst))
