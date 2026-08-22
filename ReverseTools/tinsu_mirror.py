# -*- coding: utf-8 -*-
"""Mirror dot Tin Su (cay chay that -> repo D:\GAMEDEVNEW\serverscript_jx2\tinsu\)."""
import io, os, shutil, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
SP = r"C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\51bb04de-d5c8-4a11-ab83-a6aaceecb2ed\scratchpad"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
DST = r"D:\GAMEDEVNEW\serverscript_jx2\tinsu"
n = 0
def cp(rel_src, rel_dst=None):
    global n
    s = os.path.join(SRV, rel_src); d = os.path.join(DST, rel_dst or rel_src)
    assert os.path.isfile(s), s
    os.makedirs(os.path.dirname(d), exist_ok=True)
    shutil.copyfile(s, d); n += 1
# 85 tep chep tu Linux (da va) + tinsu_addnpc + honnguyenchandon + checkinmap
for rel in io.open(os.path.join(SP, "tinsu_missing.txt")).read().split("\n"):
    rel = rel.strip()
    if not rel or "partner_master_main" in rel: continue
    cp(os.path.join("script", rel.replace("/", os.sep)))
for rel in (r"script\task\tollgate\tinsu_addnpc.lua", r"script\item\event\kinhmach\honnguyenchandon.lua",
            r"script\vng_feature\checkinmap.lua",
            r"settings\task\tollgate\messenger\messenger_tollprize.txt", r"settings\task\tollgate\messenger\tollgate_allprize.txt"):
    cp(rel)
# tep du an da sua (jx1_edits)
for rel in (r"script\startgame.lua", r"script\global\npcchucnang\dichquan.lua", r"script\global\npcchucnang\xaphu.lua",
            r"script\global\station.lua", r"settings\maplist.ini", r"settings\item\magicscript.txt"):
    cp(rel, os.path.join("jx1_edits", rel))
print("mirror", n, "tep ->", DST)
