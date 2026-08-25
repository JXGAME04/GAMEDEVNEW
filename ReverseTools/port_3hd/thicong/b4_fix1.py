# -*- coding: utf-8 -*-
"""B4 - VA DOT 1 (loi tu tim ra khi soat lai):

LOI 1 (CHAN): settings\\task\\missions.txt cua JX1 - mission 15 va 22 dang tro toi
  mission_trong.lua (RONG) => OpenMission(15)/OpenMission(22) mo mission nhung
  InitMission/RunMission/EndMission/OnLeave/JoinMission KHONG BAO GIO chay
  => Phong Lang Do va Vuot Ai KHONG HOAT DONG.
  Ban Linux: 15 -> missions\\fengling_ferry\\mission.lua
             22 -> missions\\challengeoftime\\mission_match.lua

LOI 2 (NANG): hd3_thuyenphu.lua goi GetNpcValue() KHONG tham so. JX1 LuaGetNpcParam
  (ScriptFuns.cpp) yeu cau >= 1 tham so, thieu thi 'return 0' (khong day gia tri nao)
  => BOATID = nil => guard ep ve 1 => CA 6 thuyen phu deu thanh thuyen 1,
  thuyen 2 va 3 khong bao gio dung duoc.
  Sua: GetNpcValue(NpcIndex).
"""
import io, os, shutil, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"

# ---------- LOI 1: missions.txt ----------
p = os.path.join(JX1, "settings", "task", "missions.txt")
bak = p + ".truoc_3hd_2508"
if not os.path.exists(bak):
    shutil.copyfile(p, bak)
d = io.open(p, encoding="latin-1", newline="").read()
nl = "\r\n" if "\r\n" in d else "\n"
lines = d.split(nl)

fixes = {
    "15": "\\script\\missions\\fengling_ferry\\mission.lua",
    "22": "\\script\\missions\\challengeoftime\\mission_match.lua",
}
n = 0
for i, ln in enumerate(lines):
    parts = ln.split("\t")
    if len(parts) >= 2 and parts[0].strip() in fixes:
        mid = parts[0].strip()
        if "mission_trong" in parts[1]:
            lines[i] = "%s\t%s" % (parts[0], fixes[mid])
            print("  mission %s: mission_trong.lua -> %s" % (mid, fixes[mid]))
            n += 1
        else:
            print("  mission %s: DA TRO TOI %s (bo qua)" % (mid, parts[1]))
if n:
    io.open(p, "w", encoding="latin-1", newline="").write(nl.join(lines))
    m = os.path.join(MIRROR, "settings", "task", "missions.txt")
    os.makedirs(os.path.dirname(m), exist_ok=True)
    shutil.copyfile(p, m)
print("LOI 1: sua %d dong (backup: %s)" % (n, os.path.basename(bak)))

# ---------- LOI 2: hd3_thuyenphu.lua ----------
rel = os.path.join("script", "missions", "fengling_ferry", "hd3_thuyenphu.lua")
p2 = os.path.join(JX1, rel)
d2 = io.open(p2, encoding="latin-1", newline="").read()
old = "BOATID = GetNpcValue()"
new = "BOATID = GetNpcValue(NpcIndex)\t-- PHAI truyen NpcIndex: LuaGetNpcParam thieu tham so -> tra 0 gia tri"
if old in d2:
    d2 = d2.replace(old, new)
    io.open(p2, "w", encoding="latin-1", newline="").write(d2)
    m2 = os.path.join(MIRROR, rel)
    os.makedirs(os.path.dirname(m2), exist_ok=True)
    io.open(m2, "w", encoding="latin-1", newline="").write(d2)
    print("LOI 2: da sua GetNpcValue() -> GetNpcValue(NpcIndex)")
else:
    print("LOI 2: khong tim thay (da sua roi?)")
