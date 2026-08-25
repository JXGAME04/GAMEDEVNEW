# -*- coding: utf-8 -*-
"""SINH TU DONG UiTaskGuideSatThuBoss.h tu settings/task/tollgate/killer/killer.txt
(giong khuon gen_datau_tables.py). Chay lai bat cu luc nao killer.txt doi.
Nguon: cay song E:\...\bin\server (killer.txt la du lieu van hanh).
Ten/vi tri giu NGUYEN BYTE TCVN3 (doc/ghi latin-1, khong decode).
Info cat tai " . " dau tien (bo cau khich "Ta khong tin nguoi thang duoc han!").
"""
import io, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

SRC = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\task\tollgate\killer\killer.txt"
OUT = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiTaskGuideSatThuBoss.h"

d = io.open(SRC, encoding="latin-1", newline="").read().split("\n")
rows = {}
for l in d[1:]:
    c = l.rstrip("\r").split("\t")
    if len(c) < 3 or not c[0].isdigit():
        continue
    nid = int(c[0])
    name = c[1].strip()
    info = c[2]
    cut = info.find(" . ")
    if cut > 0:
        info = info[:cut].strip()
    rows[nid] = (name, info)
nmax = max(rows) if rows else 0
assert nmax == 160 and len(rows) == 160, (nmax, len(rows))

esc = lambda s: s.replace("\\", "\\\\").replace('"', '\\"')
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
io.open(OUT, "w", encoding="latin-1", newline="\r\n").write("\n".join(L))
print("da sinh", OUT, "-", len(rows), "boss")
