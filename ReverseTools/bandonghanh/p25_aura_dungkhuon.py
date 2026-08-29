# -*- coding: utf-8 -*-
r"""[PETSYS 28/08] Viet lai aura.lua DUNG KHUON skill script chuan:
- Chep NGUYEN bo ham khung (SkillExpFunc/Line/Link/GetSkillLevelData/Param2String
  ...) tu file mau \script\skill\partner\resistance\<bai du bat xam>.lua
- SKILLS = { pet_aura_k = { <attrib> = {m1, m2, m3} } } - MOI attrib 3 mang diem
  (param1 theo cap tu pet_skill_def; param2/3 = -1).
Ghi de server + client. GO marker cu khong can (ghi truc tiep).
"""
import glob
import io
import os
import re
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"

# ---- 1. lay khung tu file mau (giu nguyen byte GBK comment) ----
mau = glob.glob(os.path.join(SV, "script", "skill", "partner", "resistance", "*.lua"))[0]
khung = open(mau, "rb").read().decode("latin-1")
# cat bo bang SKILLS cua mau: thay block "SKILLS={...}" dau tien bang cua ta
i = khung.find("SKILLS=")
assert i > 0
# tim '}' dong bang SKILLS (dem ngoac)
j = i + khung[i:].find("{")
depth = 0
k = j
while k < len(khung):
    if khung[k] == "{":
        depth += 1
    elif khung[k] == "}":
        depth -= 1
        if depth == 0:
            break
    k += 1
k += 1

# ---- 2. dung bang SKILLS moi tu pet_skill_def ----
rows = [r.split(b"\t") for r in
        open(os.path.join(SV, "settings", "petsys", "pet_skill_def.txt"), "rb").read().split(b"\n")
        if r.strip()]
DAY = [[], [], [], []]
ATTRIB = [None] * 4
for r in rows[1:]:
    lv = int(r[0])
    for q in range(4):
        a = r[1 + q * 5].decode("latin-1").strip()
        p1 = r[2 + q * 5].decode("latin-1").strip()
        if a not in ("-1", "") and ATTRIB[q] is None:
            ATTRIB[q] = a
        v = 0 if p1 in ("-1", "") else int(p1)
        DAY[q].append((lv, v))

L = ["SKILLS={"]
for q in range(4):
    diem = ",".join("{%d,%d}" % (lv, v) for lv, v in DAY[q])
    L.append("\tpet_aura_%d={" % (q + 1))
    L.append("\t\t%s={" % ATTRIB[q])
    L.append("\t\t\t{%s}," % diem)
    L.append("\t\t\t{{1,-1},{20,-1}},")
    L.append("\t\t\t{{1,-1},{20,-1}}")
    L.append("\t\t}")
    L.append("\t},")
L.append("}")
bang = "\r\n".join(L)

moi = khung[:i] + bang + khung[k:]
moi = ("-- [PETSYS 28/08] 4 aura Ban Dong Hanh - khung chep nguyen tu:\r\n-- " +
       os.path.basename(mau) + " (chi thay bang SKILLS)\r\n" + moi)

for base in (SV, CL):
    p = os.path.join(base, "script", "skill", "petsys", "aura.lua")
    io.open(p, "w", encoding="latin-1", newline="").write(moi)
print("ghi aura.lua dung khuon (server+client), dai", len(moi))
# kiem: co GetSkillLevelData + SKILLS pet_aura_1
assert "GetSkillLevelData" in moi and "pet_aura_1" in moi
print("kiem: GetSkillLevelData + pet_aura_1 OK")
