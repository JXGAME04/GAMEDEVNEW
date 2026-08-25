# -*- coding: utf-8 -*-
"""C40 - chu bao: "chua mo hoat dong Phong Lang Do ma dung Lenh bai Thuy Tac van
vao tham gia duoc - dung ra phai toi gio hoat dong moi vao duoc".

DO THUC TE:
  * fenglingdu_main() (khai cuoc, moi gio phut :00) goi CloseMission -> OpenMission.
    Trong JX1: OpenMission = TAO mission, CloseMission = XOA mission
    (ScriptFuns.cpp LuaInitMission / LuaCloseMission).
  * => CHUA KHAI CUOC thi mission KHONG TON TAI.
  * fld_TakeBoat chan bang: t = 10 - GetMissionV(MS_TIMEACC_1MIN); if (t <= 0) return 0
    Nhung mission khong ton tai => GetMissionV tra 0 => t = 10 > 0 => KHONG CHAN,
    nguoi choi VAN len thuyen. Te hon: AddMSPlayer(MISSIONID,1) THAT BAI (khong co
    mission) nhung NewWorld(boatmapid,...) VAN CHAY => nguoi choi bi day len map
    thuyen MA KHONG THUOC MISSION => khong duoc SetFightState(1), khong nhan thong
    bao, ket tren thuyen ma khong lam gi duoc.
  * Lenh bai Thuy Tac (6,1,3363) khong co script rieng (cot Script = "0") - no chi
    la VE bi tru trong gio ton phi, khong phai item "vao hoat dong".

VA (toi thieu, dung tinh than ban Linux "chi chay dung gio"): chan ngay dau
fld_TakeBoat bang IsMission(MISSIONID) - ham co san cua JX1 tra 1 khi mission DANG
TON TAI (tuc DA KHAI CUOC va CHUA ket thuc). Dat SAU khi da doi SubWorld sang map
thuyen (IsMission doc theo SubWorld hien tai).
"""
import io, os, sys, shutil
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
rel = r"script\missions\fengling_ferry\fld_head.lua"
p = os.path.join(JX1, rel)
d = io.open(p, encoding="latin-1", newline="").read()
if "IsMission(MISSIONID)" in d:
    print("da va roi"); raise SystemExit

nl = "\r\n" if "\r\n" in d else "\n"
old = "\tt = 10 - GetMissionV(MS_TIMEACC_1MIN)"
assert d.count(old) == 1, d.count(old)
new = nl.join([
    "\t-- [3HD 25/08 C40] CHUA KHAI CUOC thi mission KHONG TON TAI (fenglingdu_main",
    "\t-- moi goi OpenMission = TAO mission). Khi do GetMissionV tra 0 => t = 10 =>",
    "\t-- chan ben duoi vo hieu => nguoi choi len duoc thuyen ma AddMSPlayer that bai",
    "\t-- (ngoai mission, khong duoc SetFightState(1), ket tren thuyen).",
    "\t-- IsMission tra 1 CHI KHI mission dang ton tai = da toi gio khai cuoc.",
    "\tif (IsMission(MISSIONID) == 0) then",
    "\t\tSay(\"" + V("Chưa tới giờ thuyền chạy. Mỗi giờ đúng phút 00 thuyền mới khai cuộc, hãy quay lại lúc đó.") + "\", 0)",
    "\t\tSubWorld = oldsubworldindex",
    "\t\tPlayerIndex = orgplayerindex",
    "\t\treturn 0",
    "\tend",
    old,
])
d = d.replace(old, new)
io.open(p, "w", encoding="latin-1", newline="").write(d)
dst = os.path.join(MIR, rel)
os.makedirs(os.path.dirname(dst), exist_ok=True)
shutil.copyfile(p, dst)
print("da chan len thuyen khi chua khai cuoc (IsMission) + dong bo guong")
