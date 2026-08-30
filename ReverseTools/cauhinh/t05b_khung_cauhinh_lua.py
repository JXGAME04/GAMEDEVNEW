# -*- coding: utf-8 -*-
"""t05b_khung_cauhinh_lua.py - THI CONG DOT 3/b: KHUNG he cau hinh script.

NGUYEN TAC (rut ra tu chinh du an, khong bia):
 1) MO RONG cai da co, khong de them co che thu 9. Du an da co
    `script\\header\\cauhinh_hoatdong.lua` (bang tbCHD 533 dong + ham HD_CFG).
    He moi dung y khuon do va TRA DUOC ca khoa cu => script cu khong phai sua.
 2) MOI TEP CAU HINH LA "LA": TUYET DOI khong Include gi. Day la bai hoc that
    ghi ngay tren dau cauhinh_hoatdong.lua:17-18 - vong Include 23/08 lam
    GameServer boot stack overflow.
 3) Tach theo CHU DE de sua khong dam nhau, va de tep tinh nang chi nap dung
    phan no can:
        ch_chung.lua   thong so chung (gioi han, nguong, cong tac toan cuc)
        ch_lich.lua    THOI GIAN moi hoat dong / su kien
        ch_thuong.lua  BANG THUONG moi hoat dong / su kien
        ch_exp.lua     kinh nghiem + exp ky nang (phan chinh duoc bang script)
        ch_drop.lua    rot do (phan chinh duoc bang script)
 4) Mot ham tra duy nhat `G_CFG(khoa, macdinh)` - tra lan luot qua cac bang
    dang co mat, cuoi cung tra ve tbCHD cu. Bang nao chua nap thi bo qua im
    lang, khong loi.

TEP NAY CHI DUNG KHUNG + BO DOC. So lieu that duoc do vao o dot ke tiep
(dang chay khao sat 6 nhom hoat dong).

⚠️ LUU Y VE EXP/DROP: phan lon he so exp nam CUNG trong C++ va da duoc dua ra
`gamesetting.ini` nhom [Exp] o dot t05a - KHONG lap lai o day. ch_exp.lua chi
giu phan script that su doc duoc (exp thuong nhiem vu/hoat dong).

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
TM = os.path.join(S, "cauhinh")
NHAN = "[CAUHINH 29/08]"

DAU = """-- ============================================================================
-- %(ten)s - %(mota)s
-- %(nhan)s
--
-- ⚠️ TEP NAY LA "LA": CAM Include bat cu gi vao day.
--    (Bai hoc that: vong Include 23/08 lam GameServer boot stack overflow -
--     xem ghi chu dau script\\header\\cauhinh_hoatdong.lua)
--
-- CACH DUNG: sua so trong bang duoi day roi KHOI DONG LAI GameServer.
--    Muon doc trong script:  local x = G_CFG("TEN_KHOA", giatri_mac_dinh)
--    (nho Include \\script\\cauhinh\\ch_lib.lua truoc khi goi G_CFG)
-- ============================================================================

"""

TEP = {}

TEP["ch_lib.lua"] = (DAU % dict(
    ten="CH_LIB.LUA", mota="bo doc cau hinh dung chung", nhan=NHAN)) + """\
-- Tra cuu lan luot qua cac bang cau hinh DANG CO MAT trong state nay.
-- Bang nao chua nap thi bo qua - khong loi, khong canh bao.
-- Bang cuoi cung la tbCHD cua cauhinh_hoatdong.lua (tuong thich nguoc: moi
-- khoa TW_/BR_/BW_/TC_/YDBZ_ cu van tra duoc bang G_CFG).
function G_CFG(szKhoa, macdinh)
\tif (szKhoa == nil) then
\t\treturn macdinh
\tend
\tif (tbCFG_CHUNG ~= nil and tbCFG_CHUNG[szKhoa] ~= nil) then
\t\treturn tbCFG_CHUNG[szKhoa]
\tend
\tif (tbCFG_LICH ~= nil and tbCFG_LICH[szKhoa] ~= nil) then
\t\treturn tbCFG_LICH[szKhoa]
\tend
\tif (tbCFG_THUONG ~= nil and tbCFG_THUONG[szKhoa] ~= nil) then
\t\treturn tbCFG_THUONG[szKhoa]
\tend
\tif (tbCFG_EXP ~= nil and tbCFG_EXP[szKhoa] ~= nil) then
\t\treturn tbCFG_EXP[szKhoa]
\tend
\tif (tbCFG_DROP ~= nil and tbCFG_DROP[szKhoa] ~= nil) then
\t\treturn tbCFG_DROP[szKhoa]
\tend
\tif (tbCHD ~= nil and tbCHD[szKhoa] ~= nil) then
\t\treturn tbCHD[szKhoa]
\tend
\treturn macdinh
end

-- Ban NGHIEM NGAT: thieu khoa thi ghi log de con biet ma sua, roi van tra
-- gia tri mac dinh (khong bao gio nem loi lam dut ca hoat dong).
function G_CFG_BUOC(szKhoa, macdinh)
\tlocal ra = G_CFG(szKhoa, nil)
\tif (ra == nil) then
\t\tif (GhiLog ~= nil) then
\t\t\tGhiLog("CAUHINH", "thieu khoa: "..szKhoa)
\t\telse
\t\t\tprint("[CAUHINH] thieu khoa: "..szKhoa)
\t\tend
\t\treturn macdinh
\tend
\treturn ra
end

-- 1730 -> "17:30" (ghep vao cau thoai NPC)
function G_GioPhut(nHHMM)
\treturn format("%02d:%02d", floor(nHHMM/100), mod(nHHMM, 100))
end

-- Doi HHMM sang so phut trong ngay, de so sanh moc gio cho gon.
function G_HHMM2Phut(nHHMM)
\treturn floor(nHHMM/100) * 60 + mod(nHHMM, 100)
end

-- Kiem gio hien tai co nam trong khoang [nBatDau, nKetThuc] dang HHMM khong.
-- Khoang qua nua dem (vd 2300 -> 0100) cung dung.
function G_TrongKhoangGio(nBatDau, nKetThuc)
\tlocal _, _, _, nHr, nMi = GetTimeNow()
\tlocal nNay = nHr * 60 + nMi
\tlocal nA = G_HHMM2Phut(nBatDau)
\tlocal nB = G_HHMM2Phut(nKetThuc)
\tif (nA <= nB) then
\t\tif (nNay >= nA and nNay <= nB) then
\t\t\treturn 1
\t\tend
\t\treturn 0
\tend
\tif (nNay >= nA or nNay <= nB) then
\t\treturn 1
\tend
\treturn 0
end
"""

TEP["ch_chung.lua"] = (DAU % dict(
    ten="CH_CHUNG.LUA", mota="thong so chung toan game", nhan=NHAN)) + """\
tbCFG_CHUNG = {

-- ---------------------------------------------------------------------------
-- NHIP NAP LAI SCRIPT
-- ---------------------------------------------------------------------------
-- timerserver.lua:40 co dong `dofile("script/timerserver.lua")` tu nap lai
-- CHINH NO moi phut. Nho do sua script la an ngay khong can restart, nhung
-- moi lan la nap lai 33 Include (~103 tep, ~0,97 MB) trong 1 phut.
-- So duoi day = cu bao nhieu PHUT thi nap lai mot lan.
--   1 = y het hanh vi cu (mac dinh, khong doi gi)
--   5 = nhe hon 5 lan, sua script cho toi da 5 phut moi an
--   0 = TAT han (nhe nhat; sua script phai restart moi an)
CH_NAPLAI_PHUT = 1,

-- ---------------------------------------------------------------------------
-- GHI LOG
-- ---------------------------------------------------------------------------
-- Bat/tat log cua he cau hinh + he thuong (0/1). Duong log:
--   logs\\hethong.log  (ham GhiLog cua script - dang ky o ScriptFuns.cpp)
--   ScriptError.log    (loi runtime Lua, co ca stack traceback + tep:dong)
CH_LOG_CAUHINH = 1,
CH_LOG_THUONG = 1,

}
"""

TEP["ch_lich.lua"] = (DAU % dict(
    ten="CH_LICH.LUA", mota="THOI GIAN moi hoat dong / su kien", nhan=NHAN)) + """\
-- Quy uoc dat ten khoa:  <TIENTO>_GIO_<viec>   gio dang HHMM (2000 = 20:00)
--                        <TIENTO>_THU_<viec>   thu trong tuan (0=CN..6=T7)
--                        <TIENTO>_PHUT_<viec>  do dai tinh bang phut
--                        <TIENTO>_BAT          cong tac 0/1
-- Tien to dang dung: TW_ (bang chien) BR_ (bach nhan) BW_ (ty vo)
--                    TC_ (thanh bao) YDBZ_ (viem de) - 5 nhom NAY DA CO SAN
--                    trong script\\header\\cauhinh_hoatdong.lua, KHONG chep
--                    lai vao day de tranh hai noi khai mot khoa.
-- Cac hoat dong con lai duoc do vao day o dot ke tiep (dang khao sat).

tbCFG_LICH = {

}
"""

TEP["ch_thuong.lua"] = (DAU % dict(
    ten="CH_THUONG.LUA", mota="BANG THUONG moi hoat dong / su kien", nhan=NHAN)) + """\
-- Quy uoc: moi bang thuong la mang cac muc
--     { <so luong>, {<genre>, <detail>, <particular>}, "<ten de doi chieu>" }
-- Ten de doi chieu KHONG dung de tim vat pham - no chi de nguoi doc biet mon
-- gi, va de bo kiem `ReverseTools\\cauhinh\\ktr_cauhinh.py` doi chieu voi
-- settings\\item\\magicscript.txt roi BAO LOI neu lech.
--
-- ⚠️ Ma vat pham cua ban Linux (dai 30000+, hoac {6,0,x}) KHONG dung duoc o
--    JX1: magicscript cua du an CHI co detail = 1. Xem thieu_vatpham.txt.

tbCFG_THUONG = {

}
"""

TEP["ch_exp.lua"] = (DAU % dict(
    ten="CH_EXP.LUA", mota="kinh nghiem phan chinh duoc bang script", nhan=NHAN)) + """\
-- ⚠️ PHAN LON he so exp KHONG o day ma nam trong C++, va da duoc dua ra
--    settings\\gamesetting.ini nhom [Exp] (dot t05a 29/08):
--      MocCap1/2/3, HeSo1..4   he so theo cap (dang la 80/70/280/100)
--      VipCong                 VIP cong them
--      CsMap/CsLanToiThieu/CsDuoi/CsTren   luat chuyen sinh map 341
--      ChenhCapMax/ChiaKhiChenh/MienTruCap phat chenh cap
--      ToDoi2..ToDoi8/ToDoiKhac            % chia exp to doi
--    Sua o do, KHOI DONG LAI GameServer.
--
--    Nguong exp moi cap:      settings\\npc\\player\\level_exp.txt
--      (chu y: CO HAI ban level_exp.txt, ban DUOC DOC la ban trong thu muc
--       npc\\player - hang so o CoreUseNameDef.h:83. Ban settings\\player\\
--       level_exp.txt la MO COI, sua vo ich.)
--    Exp moi cap ky nang:     settings\\npc\\player\\magic_level_exp.txt
--    Toc do tang exp ky nang: gamesetting.ini Skill90Rate / Skill120Rate
--
-- Bang duoi day chi danh cho exp THUONG cua nhiem vu / hoat dong - phan that
-- su nam trong script. Duoc do vao o dot ke tiep.

tbCFG_EXP = {

}
"""

TEP["ch_drop.lua"] = (DAU % dict(
    ten="CH_DROP.LUA", mota="rot do phan chinh duoc bang script", nhan=NHAN)) + """\
-- ⚠️ DANH MUC vat pham + ti le rot nam o 49 tep settings\\droprate\\**\\*.ini
--    (nap lai NGAY moi lan rot do - KHONG can restart).
--    Viec CHON tep .ini nao theo cap quai nam o
--    script\\global\\LuaNpcMonsters\\Droprate_normal.lua:24-95
--    Xac suat rot cua quai thuong: Droprate_normal.lua:113-118 (hien la 3/31).
--
-- Bang duoi day danh cho cac hang so ti le trong script (lib_sukien.lua...).
-- Duoc do vao o dot ke tiep.

tbCFG_DROP = {

}
"""

TEP["ch_all.lua"] = """-- ============================================================================
-- CH_ALL.LUA - nap CA BO cau hinh
-- %s
--
-- Day la tep DUY NHAT trong thu muc nay duoc phep Include. Dung no khi mot
-- tinh nang can nhieu nhom cau hinh; con neu chi can mot nhom thi Include
-- thang tep do cho nhe.
-- ============================================================================

Include("\\\\script\\\\cauhinh\\\\ch_lib.lua")
Include("\\\\script\\\\cauhinh\\\\ch_chung.lua")
Include("\\\\script\\\\cauhinh\\\\ch_lich.lua")
Include("\\\\script\\\\cauhinh\\\\ch_thuong.lua")
Include("\\\\script\\\\cauhinh\\\\ch_exp.lua")
Include("\\\\script\\\\cauhinh\\\\ch_drop.lua")
""" % NHAN


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t05b_khung_cauhinh_lua - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))

    co = os.path.isdir(TM)
    print("  thu muc %s: %s" % (TM, "da co" if co else "se tao"))
    for ten, nd in sorted(TEP.items()):
        p = os.path.join(TM, ten)
        if os.path.isfile(p):
            print("  %-14s DA CO - bo qua (khong ghi de)" % ten)
            continue
        # chot: khong duoc co byte > 127 sai ma hoa. Ta viet ASCII + vai ky tu
        # tieng Viet co dau -> phai ma hoa TCVN3. O day noi dung dat toan ASCII
        # tru vai ky hieu; kiem tra va bao neu lot ky tu ngoai bang.
        xau = [c for c in nd if ord(c) > 127 and c not in "⚠️"]
        if xau:
            print("!!! LOI TO: %s co %d ky tu ngoai ASCII: %r"
                  % (ten, len(xau), xau[:8]))
            return 1
        print("  %-14s se tao (%d dong)" % (ten, nd.count("\n") + 1))
        if ghi:
            if not os.path.isdir(TM):
                os.makedirs(TM)
            # bo ky tu canh bao unicode -> thay bang ASCII cho an toan ma hoa
            nd_ghi = nd.replace("⚠️", "!!")
            with io.open(p, "wb") as f:
                f.write(nd_ghi.replace("\n", "\r\n").encode("latin-1"))
            print("     DA GHI %s" % p)

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
    else:
        print("\nDa tao khung. Buoc sau: do so lieu that vao ch_lich/ch_thuong/"
              "ch_exp/ch_drop.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
