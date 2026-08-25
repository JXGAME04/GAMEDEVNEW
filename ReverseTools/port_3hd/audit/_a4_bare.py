# -*- coding: utf-8 -*-
"""DUONG SOT: tim MOI dang KHAC 6,1,<pt> trong 101 tep manifest (ke ca .txt/.ini)
   dung particular NGUON (Linux) cua 42 bo — dang tran, bien trung gian, chuoi."""
import io, os, re, sys, json
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\port_3hd")
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
from dec2 import decline2

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
LNX = r"D:\ServerLinux\server1"
HERE = r"D:\GAMEDEVNEW\ReverseTools\port_3hd\thicong"
remap = json.load(io.open(os.path.join(HERE, "remap_resolved.json"), encoding="utf-8"))
parts = {}
for s, d in remap.items():
    parts[s.split(",")[2]] = d.split(",")[2]
manifest = [x for x in io.open(os.path.join(HERE, "b1_manifest.txt"), encoding="utf-8").read().split("\n") if x.strip()]

# tep MOI do phien viet ra
extra = [r"script\tinhnang\3hoatdong\hd3_driver.lua", r"script\item\hd3_admin.lua",
         r"script\global\autoexec_npc_hd3.lua", r"script\missions\fengling_ferry\hd3_thuyenphu.lua",
         r"script\header\cauhinh_hoatdong.lua", r"script\item\lenhbaiadmin.lua",
         r"settings\timerserver.lua", r"script\global\startgame.lua"]

TRIP = re.compile(r"(?<![0-9])6(\s*,\s*)1(\s*,\s*)(\d+)(?![0-9])")
for rel in manifest + extra:
    p = os.path.join(JX1, rel.replace("/", os.sep))
    if not os.path.isfile(p):
        continue
    d = open(p, "rb").read().decode("latin-1")
    for i, ln in enumerate(d.split("\n"), 1):
        # bo qua khi da la bo 3 hop le (6,1,pt) -> da xu ly
        masked = TRIP.sub("###", ln)
        for pt, dst in parts.items():
            for m in re.finditer(r"(?<![0-9.])%s(?![0-9.])" % pt, masked):
                st = max(0, m.start() - 34)
                print("%-58s :%-5d pt=%s (dich %s)  ...%s..." %
                      (rel, i, pt, dst, decline2(masked[st:m.end() + 34].strip().encode("latin-1"))))
