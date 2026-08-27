# -*- coding: utf-8 -*-
"""VIEM DE - buoc 8b: sinh trinh dieu khien lich + dat NPC bao danh.

Ban Linux khong chay lich trong GameServer ma o tang S3Relay:
    gateway\\s3relay\\relaysetting\\task\\baotangviemde\\baotangviemde_HHMM.lua
      TaskTime(8,25) / (10,25) / (14,25) / (16,25) / (18,25) / (20,25) / (22,25)
      TaskContent(): GlobalExecute("dwf \\script\\missions\\yandibaozang\\
                     yandibaozang_trigger.lua YDBZ_OnTrigger()")
JX1 khong co tang do; cac dot port truoc da dung dong ho GameServer theo khuon
HD3_DriverInit / HD3_Tick (hd3_driver.lua). Tep nay theo DUNG khuon ay.

NPC bao danh: ban Linux dat qua script\\global\\autoexec_npc.lua:141-144
    {1320, 80, 4, 37, x, y, 0, "Binh Binh co nuong", 0, yandibaozang_main.lua}
  -> template 1320 (JX1 DA CO san trong npcs.txt: Kind=3 Camp=6 Series=0),
     map 37 = Bien Kinh, 4 toa do TRUNG KHOP voi tbReady:GetSignUpPos (noi day
     nguoi choi ra khi bi loai) - nen giu nguyen ca 4.
JX1 khong co autoexec_npc.lua nen dat NPC trong DriverInit, y het cach hd3_driver
dat Bac Dau lao nhan / thuyen phu.

*** TEP NAY CHUA DUOC MOC VAO startgame.lua / timerserver.lua ***
Viec moc la buoc CUOI CUNG (sau phan bien) - xem BANGIAO_VIEMDE_2608.md muc 9.
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bangtxt import uni2tcvn

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
GUONG = r"D:\GAMEDEVNEW\serverscript_jx2\viemde"
DICH = r"\script\tinhnang\viemde\ydbz_driver.lua"

L = []


def s(txt):
    L.append(uni2tcvn(txt))


s("-- ============================================================================")
s("-- YDBZ_DRIVER.LUA - Boot + Lich chay VIEM DE BAO TANG   (port 26/08)")
s("-- SINH TU DONG boi ReverseTools\\viemde\\v19_gen_driver.py - DUNG SUA TAY")
s("--")
s("-- Thay tang S3Relay cua ban Linux (gateway\\s3relay\\relaysetting\\task\\")
s("-- baotangviemde\\*.lua) bang dong ho GameServer - dung KHUON da chung cua cac")
s("-- dot port truoc (HD3_DriverInit / HD3_Tick trong hd3_driver.lua).")
s("--   * YDBZ_DriverInit()   : goi 1 lan trong OnGame (startgame.lua) - dat NPC.")
s("--   * YDBZ_Tick(nHr,nMi)  : goi moi phut trong RunTime (timerserver.lua) - lich.")
s("-- Cau hinh so lieu: script\\header\\cauhinh_hoatdong.lua khoi [7], tien to YDBZ_.")
s("--")
s("-- LOGIC 100% BAN LINUX: den gio thi goi thang YDBZ_OnTrigger() cua")
s("-- yandibaozang_trigger.lua, y het cau GlobalExecute cua ban Linux.")
s("-- ============================================================================")
s('Include("\\\\script\\\\header\\\\cauhinh_hoatdong.lua")')
s('Include("\\\\script\\\\missions\\\\yandibaozang\\\\yandibaozang_trigger.lua")')
s("")
s('YDBZ_SCRIPT_NPC = "\\\\script\\\\missions\\\\yandibaozang\\\\npc\\\\yandibaozang_main.lua"')
s("YDBZ_NPC_TPL    = 1320    -- \"Binh Binh co nuong\" (npcs.txt JX1: Kind=3 Camp=6 Series=0)")
s("YDBZ_NPC_LEVEL  = 80")
s("YDBZ_NPC_SERIES = 4       -- goc: autoexec_npc.lua:141 tham so 3")
s("")
s("-- 4 toa do (o) tren map 37 Bien Kinh - goc autoexec_npc.lua:141-144.")
s("-- TRUNG KHOP voi tbReady:GetSignUpPos (noi day nguoi choi ra khi bi loai).")
s("YDBZ_NPC_VITRI = {")
s("\t{1714, 3173},")
s("\t{1642, 3145},")
s("\t{1622, 3019},")
s("\t{1857, 2968},")
s("}")
s("")
s("-- ---------------------------------------------------------------------------")
s("-- BOOT: dat 4 NPC bao danh")
s("-- ---------------------------------------------------------------------------")
s("function YDBZ_DriverInit()")
s("\tif HD_CFG(\"YDBZ_BAT\", 1) ~= 1 then")
s('\t\tprint("[ViemDe] YDBZ_BAT = 0 - bo qua boot.")')
s("\t\treturn 0")
s("\tend")
s("\t-- Don NPC cu (chay lai DriverInit khong nhan ban NPC). HD3_DelNpcByScript")
s("\t-- dung strstr nen ten script phai KHONG la tien to cua tep khac (bay #13).")
s("\tif HD3_DelNpcByScript ~= nil then")
s('\t\tlocal nXoa = HD3_DelNpcByScript("yandibaozang_main")')
s("\t\tif nXoa > 0 then")
s('\t\t\tprint("[ViemDe] don "..nXoa.." NPC bao danh cu.")')
s("\t\tend")
s("\tend")
s("\tlocal nMapId = HD_CFG(\"YDBZ_NPC_MAP\", 37)")
s("\tlocal nIdx = SubWorldID2Idx(nMapId)")
s("\tif nIdx < 0 then")
s('\t\tprint("[ViemDe] ban do "..nMapId.." CHUA NAP - khong dat duoc NPC bao danh.")')
s("\t\treturn 0")
s("\tend")
s("\tlocal nDat = 0")
s("\tlocal i")
s("\tfor i = 1, getn(YDBZ_NPC_VITRI) do")
s("\t\tlocal t = YDBZ_NPC_VITRI[i]")
s("\t\tlocal npc = nil")
s("\t\tif HD3_AddNpc ~= nil then")
s('\t\t\tnpc = HD3_AddNpc(YDBZ_NPC_TPL, YDBZ_NPC_LEVEL, nIdx, t[1]*32, t[2]*32, YDBZ_NPC_SERIES, "%s")' % "Bình Bình cô nương")
s("\t\telse")
s("\t\t\t-- lui ve AddNpcEx: (tpl, cap, ngu hanh, chi so ban do, x, y, phe, ten)")
s('\t\t\tnpc = AddNpcEx(YDBZ_NPC_TPL, YDBZ_NPC_LEVEL, YDBZ_NPC_SERIES, nIdx, t[1]*32, t[2]*32, 6, "%s")' % "Bình Bình cô nương")
s("\t\tend")
s("\t\tif npc ~= nil and npc > 0 then")
s("\t\t\tSetNpcScript(npc, YDBZ_SCRIPT_NPC)")
s("\t\t\tnDat = nDat + 1")
s("\t\tend")
s("\tend")
s('\tprint("[ViemDe] da dat "..nDat.."/"..getn(YDBZ_NPC_VITRI).." NPC bao danh tren ban do "..nMapId..".")')
s("\treturn nDat")
s("end")
s("")
s("-- ---------------------------------------------------------------------------")
s("-- LICH: goi moi phut")
s("-- ---------------------------------------------------------------------------")
s("YDBZ_MOC_CUOI = -1   -- chong goi hai lan trong cung mot phut")
s("")
s("function YDBZ_Tick(nHr, nMi)")
s("\tif HD_CFG(\"YDBZ_BAT\", 1) ~= 1 then")
s("\t\treturn 0")
s("\tend")
s("\tlocal nMoc = nHr * 100 + nMi")
s("\tif nMoc == YDBZ_MOC_CUOI then")
s("\t\treturn 0")
s("\tend")
s('\tlocal szGio = HD_CFG("YDBZ_GIO", "0825,1025,1425,1625,1825,2025,2225")')
s("\t-- bao dau phay hai dau roi tim \",HHMM,\" - khop TRON MOC, khong the trung")
s("\t-- mot phan cua moc khac du nguoi van hanh go dang khac (vd \"825\").")
s('\tlocal nTim = strfind(","..szGio..",", ","..format("%04d", nMoc)..",", 1, 1)')
s("\tif nTim == nil then")
s("\t\treturn 0")
s("\tend")
s("\tYDBZ_MOC_CUOI = nMoc")
s('\tprint(format("[ViemDe] den gio %02d:%02d - mo bao danh.", nHr, nMi))')
s("\tif YDBZ_OnTrigger == nil then")
s('\t\tprint("[ViemDe] KHONG THAY YDBZ_OnTrigger - kiem Include yandibaozang_trigger.lua.")')
s("\t\treturn 0")
s("\tend")
s("\tYDBZ_OnTrigger()")
s("\treturn 1")
s("end")
s("")

noi_dung = "\r\n".join(L)
for goc in (SRV, GUONG):
    p = goc + DICH
    os.makedirs(os.path.dirname(p), exist_ok=True)
    tam = p + ".dangghi"
    with open(tam, "wb") as f:
        f.write(noi_dung.encode("latin-1"))
    os.replace(tam, p)
    print("da ghi %s (%d dong)" % (p, len(L)))
