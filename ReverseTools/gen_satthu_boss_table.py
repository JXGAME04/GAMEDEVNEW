# -*- coding: utf-8 -*-
"""SINH TU DONG (v2) cac bang boss he San Boss Sat Thu - DUNG SUA TAY cac file sinh ra:
  1) Sources/S3Client/Ui/UiCase/UiTaskGuideSatThuBoss.h  (ten + noi o, tu killer.txt)
  2) Sources/Core/Src/KSatThuBossPos.h                   (map/x/y, tu killbosshead.lua)
  3) bin/server/script/task/tollgate/killer/hd3_st_goboss.lua (+ guong repo)
     - dich vu "xe toi cho boss" (khuon cu nhieptran.lua denchobossST: Pay + NewWorld
       + SetFightState(1)), gia HD_CFG("HD3_ST_TIEN_XE", 1000).
Chay lai khi killer.txt / killbosshead.lua doi.
"""
import io, re, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"

# ---------- doc killer.txt ----------
d = io.open(SRV + r"\settings\task\tollgate\killer\killer.txt", encoding="latin-1", newline="").read().split("\n")
rows = {}
for l in d[1:]:
    c = l.rstrip("\r").split("\t")
    if len(c) < 3 or not c[0].isdigit():
        continue
    nid = int(c[0])
    info = c[2]
    cut = info.find(" . ")
    if cut > 0:
        info = info[:cut].strip()
    rows[nid] = (c[1].strip(), info)
assert len(rows) == 160, len(rows)

# ---------- doc killbosshead addkillertasknpc ----------
kb = io.open(SRV + r"\script\task\tollgate\killbosshead.lua", encoding="latin-1", newline="").read().split("\n")
start = None
for i, l in enumerate(kb):
    if l.startswith("addkillertasknpc"):
        start = i; break
pos = []
for l in kb[start:start + 400]:
    m = re.match(r"\s*\{(\d+),(\d+),(\d+),(\d+),(\d+),(\d+),\"([^\"]*)\"", l)
    if m:
        pos.append((int(m.group(3)), int(m.group(4)), int(m.group(5)), m.group(7)))
    if l.startswith("}"):
        break
assert len(pos) == 160, len(pos)
# doi chieu ten hang i voi killer.txt (canh bao neu lech)
mis = 0
for i in range(160):
    if pos[i][3].strip() != rows[i + 1][0]:
        mis += 1
print("ten lech giua 2 bang:", mis, "/160")

esc = lambda s: s.replace("\\", "\\\\").replace('"', '\\"')

# ---------- 1) UiTaskGuideSatThuBoss.h ----------
L = []
L.append("// ============================================================================")
L.append("// UiTaskGuideSatThuBoss.h - SINH TU DONG tu settings/task/tollgate/killer/")
L.append("// killer.txt boi ReverseTools/gen_satthu_boss_table.py - DUNG SUA TAY.")
L.append("// 160 boss he San Boss Sat Thu (task 1082 = chi so dong). Chuoi TCVN3 tho.")
L.append("// ============================================================================")
L.append("#ifndef UITASKGUIDE_SATTHU_BOSS_H")
L.append("#define UITASKGUIDE_SATTHU_BOSS_H")
L.append("")
L.append("#define ST3_BOSS_MAX 160")
L.append("")
L.append("static const char* const s_szST3BossName[ST3_BOSS_MAX + 1] = {")
L.append('\t"",')
for i in range(1, 161):
    L.append('\t"%s",' % esc(rows[i][0]))
L.append("};")
L.append("")
L.append("static const char* const s_szST3BossInfo[ST3_BOSS_MAX + 1] = {")
L.append('\t"",')
for i in range(1, 161):
    L.append('\t"%s",' % esc(rows[i][1]))
L.append("};")
L.append("")
L.append("#endif")
L.append("")
io.open(r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiTaskGuideSatThuBoss.h", "w", encoding="latin-1", newline="\r\n").write("\n".join(L))
print("1) UiTaskGuideSatThuBoss.h ok")

# ---------- 2) KSatThuBossPos.h (Core - CoreShell dung cho auto dan duong) ----------
L = []
L.append("// ============================================================================")
L.append("// KSatThuBossPos.h - SINH TU DONG tu script/task/tollgate/killbosshead.lua")
L.append("// (bang addkillertasknpc) boi ReverseTools/gen_satthu_boss_table.py - DUNG SUA TAY.")
L.append("// Vi tri spawn 160 boss (map id + o cell). Chi CLIENT dung (dan duong F11).")
L.append("// ============================================================================")
L.append("#ifndef KSATTHU_BOSS_POS_H")
L.append("#define KSATTHU_BOSS_POS_H")
L.append("")
L.append("#define ST3_POS_MAX 160")
L.append("")
for nm, idx in (("Map", 0), ("X", 1), ("Y", 2)):
    L.append("static const short s_nST3Boss%s[ST3_POS_MAX + 1] = {" % nm)
    vals = ["0"] + [str(pos[i][idx]) for i in range(160)]
    for j in range(0, 161, 16):
        L.append("\t" + ", ".join(vals[j:j + 16]) + ",")
    L.append("};")
    L.append("")
L.append("#endif")
L.append("")
io.open(r"D:\GAMEDEVNEW\Sources\Core\Src\KSatThuBossPos.h", "w", encoding="latin-1", newline="\r\n").write("\n".join(L))
print("2) KSatThuBossPos.h ok")

# ---------- 3) hd3_st_goboss.lua ----------
L = []
L.append("-- ============================================================================")
L.append("-- HD3_ST_GOBOSS.LUA - SINH TU DONG boi ReverseTools/gen_satthu_boss_table.py")
L.append("-- tu bang addkillertasknpc (killbosshead.lua) - DUNG SUA TAY.")
L.append("-- Dich vu 'xe toi cho boss' cho nut F11 (khuon cu nhieptran.lua denchobossST:")
L.append("-- kiem tien -> Pay -> NewWorld -> SetFightState(1)). Gia: HD3_ST_TIEN_XE.")
L.append("-- ============================================================================")
L.append('Include("\\\\script\\\\header\\\\cauhinh_hoatdong.lua")')
L.append("")
L.append("tbST3BossPos = {")
for i in range(160):
    L.append("\t{%d,%d,%d}," % (pos[i][0], pos[i][1], pos[i][2]))
L.append("}")
L.append("")
L.append("function st3_goboss()")
L.append("\tlocal nBoss = GetTask(1082)")
L.append("\tif (nBoss < 1 or nBoss > getn(tbST3BossPos)) then")
L.append('\t\tMsg2Player("' + V("Chưa nhận nhiệm vụ sát thủ.") + '")')
L.append("\t\treturn")
L.append("\tend")
L.append('\tlocal nTien = HD_CFG("HD3_ST_TIEN_XE", 1000)')
L.append("\tif (GetCash() < nTien) then")
L.append('\t\tMsg2Player("' + V("Không đủ") + '"..nTien.."' + V(" lượng để thuê xe tới chỗ sát thủ.") + '")')
L.append("\t\treturn")
L.append("\tend")
L.append("\tPay(nTien)")
L.append("\tlocal t = tbST3BossPos[nBoss]")
L.append("\tNewWorld(t[1], t[2] + 3, t[3] + 3)")
L.append("\tSetFightState(1)")
L.append('\tMsg2Player("' + V("Đã tới gần mục tiêu - cẩn thận!") + '")')
L.append("end")
L.append("")
rel = r"script\task\tollgate\killer\hd3_st_goboss.lua"
body = "\r\n".join(L)
io.open(SRV + "\\" + rel, "w", encoding="latin-1", newline="").write(body)
import os
os.makedirs(os.path.dirname(MIRROR + "\\" + rel), exist_ok=True)
io.open(MIRROR + "\\" + rel, "w", encoding="latin-1", newline="").write(body)
print("3) hd3_st_goboss.lua ok (song + guong)")
