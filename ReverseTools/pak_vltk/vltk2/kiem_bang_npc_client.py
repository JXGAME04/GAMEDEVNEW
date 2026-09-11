# -*- coding: utf-8 -*-
"""kiem_bang_npc_client.py - client co san bang mau NPC / bang vat the khong (de ve NPC trang tri tai cho)."""
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk\vltk2")
from doc_vatcanh import Kho, DUAN  # noqa: E402

BS = chr(92)
TEN = [BS + "settings" + BS + "NpcS.txt",
       BS + "settings" + BS + "npcs.txt",
       BS + "settings" + BS + "ImageNpcList.ini",
       BS + "settings" + BS + "obj" + BS + "ObjData.txt",
       BS + "settings" + BS + "ObjData.txt",
       BS + "settings" + BS + "npcres" + BS + "npcres.txt"]

k = Kho(DUAN)
for t in TEN:
    d = k.doc(t.encode("ascii"))
    print("%-36s %s" % (t, ("CO, %d byte" % len(d)) if d else "KHONG CO trong pak client"))
    if d and t.lower().endswith("npcs.txt"):
        ls = d.split(b"\r\n")
        print("   %d dong; dong 418-420:" % len(ls))
        for i in (418, 419, 420):
            if i < len(ls):
                print("     %d: %s" % (i, ls[i].decode("gbk", "replace")[:110]))
