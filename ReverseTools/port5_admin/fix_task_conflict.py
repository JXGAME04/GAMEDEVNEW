# -*- coding: utf-8 -*-
r"""[24/08] VA 2 XUNG DOT TASK ID (quet toan bo 2809 tep, 353 id dang dung).

LOI 1 - NGHIEM TRONG (kinh te game):
  citywar_arena (Loi Dai Bang Hoi, mission 9 - DANG CHAY) dat:
     MS_TOTALPK = 250  ma 250 = T_RUT_KNB  (moc tich luy NAP KNB, lib_task.lua:138)
     MS_TOTALKO = 251  ma 251 = T_PLAYER_XU (XU cua nguoi choi, ENGINE dinh nghia
                                             GameDataDef.h TASKVALUE_STATTASK_XU)
  death.lua:17 SetTask(MS_TOTALPK, +1) va :36 SetTask(MS_TOTALKO, +1)
  => MOI LAN PK/KO o Loi Dai, nguoi choi TU DUOC CONG XU + tu day moc nap KNB.
  Va: doi sang 4126 / 4127 (dai 4126-4199 hoan toan trong).

LOI 2 - hien tuong chu game thay: "Diem tich luy Tin Su: 66770944".
  He Tin Su dung CUNG task 1205 cho HAI viec, HAI kieu ma hoa:
    - diem tich luy (so thuan)            : lib_messenger.lua:110,128; posthouse:278...
    - so luot lam trong ngay (yymmdd*256) : posthouse.lua:489,511,518,1092
  Khi bam NPC, get_task_daily(1205) thay phan ngay khac hom nay lien GHI DE
  1205 = 260824*256 = 66770944 => mat sach diem, va nguoc lai diem that lam
  bo dem luot/ngay sai.
  Va: bo dem ngay doi sang 4128; kem don gia tri rac cu (>= 1.000.000 chac chan
  la rac vi moc doi thuong cao nhat chi 1500).
"""
import io, os, shutil

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def bak(p):
    if not os.path.isfile(p + ".truoc_taskfix_2408"):
        shutil.copyfile(p, p + ".truoc_taskfix_2408")

# ---------- LOI 1: citywar_arena + lib_loidai ----------
for rel in (r"script\missions\citywar_arena\head.lua",
            r"script\tinhnang\loidai\lib_loidai.lua"):
    p = os.path.join(E, rel)
    if not os.path.isfile(p):
        print("bo qua (khong co): %s" % rel); continue
    s = rd(p)
    if "4126" in s:
        print("da va: %s" % os.path.basename(rel)); continue
    NL = "\r\n" if "\r\n" in s else "\n"
    n = 0
    for old, new, note in (
        ("MS_TOTALPK = 250", "MS_TOTALPK = 4126", "250 = T_RUT_KNB (moc nap KNB)"),
        ("MS_TOTALKO = 251", "MS_TOTALKO = 4127", "251 = T_PLAYER_XU (XU nguoi choi)"),
    ):
        if old in s:
            s = s.replace(old, new + "\t-- [FIX 24/08] truoc la " + note, 1)
            n += 1
    if n:
        bak(p); wr(p, s)
    print("%-28s doi %d hang" % (os.path.basename(rel), n))

# ---------- LOI 2: posthouse - bo dem ngay sang 4128 ----------
p = os.path.join(E, r"script\task\tollgate\messenger\posthouse.lua")
s = rd(p)
if "4128" in s:
    print("posthouse: da va")
else:
    NL = "\r\n" if "\r\n" in s else "\n"
    n = 0
    for old, new in (("get_task_daily(1205)", "get_task_daily(4128)"),
                     ("add_task_daily(1205, 1)", "add_task_daily(4128, 1)"),
                     ("add_task_daily(1205,1)", "add_task_daily(4128,1)")):
        c = s.count(old)
        if c:
            s = s.replace(old, new); n += c
    assert n >= 4, "chi doi duoc %d cho (mong >=4)" % n
    # don gia tri rac cua task 1205 (do bug cu ghi yymmdd*256)
    anchor = "function check_daily_task_count()"
    assert s.count(anchor) == 1
    don = ("-- [FIX 24/08] don gia tri RAC cua task 1205: bug cu dung chung 1205 cho ca" + NL +
           "-- diem tich luy VA bo dem ngay (yymmdd*256) nen diem bi ghi de thanh ~66 trieu." + NL +
           "-- Moc doi thuong cao nhat chi 1500 => >= 1.000.000 chac chan la rac." + NL +
           "function messenger_clean_point_rac()" + NL +
           "\tlocal nPoint = nt_getTask(1205)" + NL +
           "\tif (nPoint >= 1000000) then" + NL +
           "\t\tnt_setTask(1205, 0)" + NL +
           "\tend" + NL +
           "end" + NL + NL)
    s = s.replace(anchor, don + anchor, 1)
    # goi don ngay dau check_daily_task_count
    old_body = "function check_daily_task_count()" + NL
    s = s.replace(old_body, old_body + "\tmessenger_clean_point_rac()\t-- [FIX 24/08]" + NL, 1)
    bak(p); wr(p, s)
    print("posthouse.lua: doi %d cho sang 4128 + them don rac" % n)

print("XONG")
