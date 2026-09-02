# -*- coding: ascii -*-
"""vhtd_khoiphuc_script.py - KHOI PHUC cac tep script may chu ve ban luu .truoc_vhtd_0209 (truoc dot Vu Hon/Tieu Dao) va xoa 3 tep moi,
de chay lai vhtd_faction_script.py tu dau (tool viet theo anchor, khong tu sua duoc noi dung da chen).
CHI dung cho 18 tep script cua vhtd_faction_script.py - KHONG dung den skills.txt/missles.txt/MagicDesc/magicscript/UI.
DUNG: python vhtd_khoiphuc_script.py
"""
import os, shutil
SCR = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
BAK = ".truoc_vhtd_0209"
FILES = [r"header\factionhead.lua", r"global\hocvocong.lua", r"lib\lib_faction.lua", r"item\skillbook.lua", r"item\lvl120skillbook.lua",
         r"item\lenhbaitanthu.lua", r"skill\skillfaction.lua", r"skill\nangskillkieumoi5x.lua", r"skill\nangskillkieumoi9x.lua",
         r"skill\nangskillkieumoitp.lua", r"item\daithanhbk90.lua", r"item\daithanhbk120.lua", r"npcthon\balanghuyen\hotrotest.lua",
         r"global\skills_table.lua", "startgame.lua"]
NEW = [r"npcthon\npcmonphai\vuhon.lua", r"npcthon\npcmonphai\tieudao.lua", r"global\vhtd\npc_vhtd.lua"]
n = 0
for rel in FILES:
    p = os.path.join(SCR, rel)
    if os.path.exists(p + BAK):
        shutil.copy2(p + BAK, p); n += 1; print("  khoi phuc", rel)
    else:
        print("  (khong co ban luu)", rel)
for rel in NEW:
    p = os.path.join(SCR, rel)
    if os.path.exists(p): os.remove(p); print("  xoa", rel)
print("xong: %d tep khoi phuc" % n)
