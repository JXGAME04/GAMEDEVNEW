# -*- coding: ascii -*-
"""Dung lai b1_manifest.txt: chep lai tu dau tu thu muc guong (nguon su that),
roi go dong config\\41\\extend.lua (tep da co y go bo o dot va)."""
import io, os

HERE = os.path.dirname(os.path.abspath(__file__))
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
p = os.path.join(HERE, "b1_manifest.txt")

# 4 tep MOI tu viet + cac tep SUA khong thuoc manifest goc
NOT_COPIED = {
    r"script\startgame.lua", r"script\timerserver.lua",
    r"script\header\cauhinh_hoatdong.lua", r"script\header\forbidmap.lua",
    r"script\item\lenhbaiadmin.lua", r"script\item\hd3_admin.lua",
    r"script\global\autoexec_npc_hd3.lua",
    r"script\missions\fengling_ferry\hd3_thuyenphu.lua",
    r"script\tinhnang\3hoatdong\hd3_driver.lua",
    r"settings\task\missions.txt", r"settings\TimerTask.txt",
}

rows = []
for dp, dn, fs in os.walk(MIRROR):
    for f in fs:
        rel = os.path.relpath(os.path.join(dp, f), MIRROR)
        if rel in NOT_COPIED:
            continue
        rows.append(rel)
rows.sort()
io.open(p, "w", encoding="utf-8", newline="\n").write("\n".join(rows))
print("manifest dung lai:", len(rows), "tep chep")
bad = [r for r in rows if "config" + os.sep + "41" in r]
print("con config\\41 trong manifest:", bad)
