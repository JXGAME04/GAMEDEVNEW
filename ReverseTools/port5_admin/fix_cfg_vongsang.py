# -*- coding: utf-8 -*-
r"""[25/08] Ghi muc DANH HIEU + VONG SANG vao file cau hinh trung tam.

CO Y KHONG tao khoa Lua cho vong sang: engine lay vong sang tu BANG
settings\PlayerTitle.txt (cot ExtSkill1/ExtSkillLevel1, KNpc.cpp:11016-11023) va tu
cast lai MOI LAN dang nhap (KPlayer.cpp:6897). Neu them mot nguon thu hai trong Lua
thi thanh HAI NGUON cho cung mot thu - sua mot noi se sai noi con lai. Nen o day chi
ghi RO duong dan + gia tri dang chay de chu game biet cho ma chinh.
"""
import io, os, shutil

P = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\header\cauhinh_hoatdong.lua"
ENC = "latin-1"
s = io.open(P, "r", encoding=ENC, newline="").read()
NL = "\r\n" if "\r\n" in s else "\n"

if "DANH HIEU + VONG SANG" in s:
    print("da them truoc do"); raise SystemExit

neo = "-- [ENGINE] Khong chinh o day:" + NL + \
      "--   * Lich tick driver: settings\\TimerTask.txt dong khoa 54 (30 giay/lan)."
assert s.count(neo) == 1, "neo = %d" % s.count(neo)

them = (
"-- --- DANH HIEU + VONG SANG cua Bang Chien ---------------------------------" + NL +
"-- [BANG] KHONG co khoa Lua o day, va do la CO Y: engine lay ca hai tu BANG" + NL +
"--   settings\\PlayerTitle.txt (tra theo DONG = id + 1) va tu cast lai moi lan" + NL +
"--   dang nhap (KNpc.cpp:11016-11023, KPlayer.cpp:6897). Them nguon thu hai trong" + NL +
"--   Lua se thanh HAI NGUON cho cung mot thu -> sua mot noi sai noi con lai." + NL +
"--   Muon doi thi sua THANG bang do, CA BAN SERVER LAN BAN CLIENT:" + NL +
"--     dong 288-292 (id 287-291) = quan ham 1..5, cot ExtSkill1 = 830 831 832 833 834" + NL +
"--                                 cot ExtSkillLevel1 = 2 4 6 8 10" + NL +
"--     dong 293-296 (id 292-295) = 4 danh hieu bang, ExtSkill1 = 1169 cap 1" + NL +
"--   Cot TitleName la TEN hien tren dau nhan vat; de trong = khong hien gi." + NL +
"--   Bo vong sang: dat ExtSkill1 = 0. Doi vong sang: xem bang tra o duoi." + NL +
"--   Cac vong sang CO SAN trong ban nay (skill -> anh, deu ve DUOI CHAN):" + NL +
"--     830..834 = 5 cap quan ham chien truong (title_new_zw/yl/bj/ab/dg.spr)" + NL +
"--     1169     = vong tron Vo Lam Dai Hoi (title_yl.spr) - id 286 THDNB dang dung" + NL +
"--     1557..1563 = bo vong sang VIP (vongsangvip1..7.spr)" + NL +
"--   Bang tra anh: settings\\npcres\\state_magic_table_name.txt, DONG = StateSpecialId + 1" + NL +
"--   (cot 3 = \"Foot\" ve duoi chan, \"Head\" ve tren dau)." + NL +
"--   LUU Y: doi ten danh hieu thi CLIENT phai thoat ra vao lai moi thay - bang nay" + NL +
"--   client chi nap MOT LAN (KNpc.cpp:6049-6056 s_bInfoTabsLoaded)." + NL + NL)

s = s.replace(neo, them + neo, 1)
q = P + ".truoc_vongsang_2508"
if not os.path.isfile(q): shutil.copyfile(P, q)
io.open(P, "w", encoding=ENC, newline="").write(s)
print("da them muc DANH HIEU + VONG SANG vao " + os.path.basename(P))
