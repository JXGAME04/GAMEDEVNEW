-- ================================================================================================
-- [HE THONG] script/cauhinh/ch_exp.lua
-- Muc dich  : CAU HINH KINH NGHIEM (ExpRate, he so).
-- Duoc nap  : Include tu 2 tep (vd ch_all.lua, exp_award.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Ham       : (khong co - tep du lieu/cau hinh)
-- Sua nong  : CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- ============================================================================
-- CH_EXP.LUA - kinh nghiem phan chinh duoc bang script
-- [CAUHINH 29/08]
--
-- !! TEP NAY LA "LA": CAM Include bat cu gi vao day.
--    (Bai hoc that: vong Include 23/08 lam GameServer boot stack overflow -
--     xem ghi chu dau script\header\cauhinh_hoatdong.lua)
--
-- CACH DUNG: sua so trong bang duoi day roi KHOI DONG LAI GameServer.
--    Muon doc trong script:  local x = G_CFG("TEN_KHOA", giatri_mac_dinh)
--    (nho Include \script\cauhinh\ch_lib.lua truoc khi goi G_CFG)
-- ============================================================================

-- !! PHAN LON he so exp KHONG o day ma nam trong C++, va da duoc dua ra
--    settings\gamesetting.ini nhom [Exp] (dot t05a 29/08):
--      MocCap1/2/3, HeSo1..4   he so theo cap (dang la 80/70/280/100)
--      VipCong                 VIP cong them
--      CsMap/CsLanToiThieu/CsDuoi/CsTren   luat chuyen sinh map 341
--      ChenhCapMax/ChiaKhiChenh/MienTruCap phat chenh cap
--      ToDoi2..ToDoi8/ToDoiKhac            % chia exp to doi
--    Sua o do, KHOI DONG LAI GameServer.
--
--    Nguong exp moi cap:      settings\npc\player\level_exp.txt
--      (chu y: CO HAI ban level_exp.txt, ban DUOC DOC la ban trong thu muc
--       npc\player - hang so o CoreUseNameDef.h:83. Ban settings\player\
--       level_exp.txt la MO COI, sua vo ich.)
--    Exp moi cap ky nang:     settings\npc\player\magic_level_exp.txt
--    Toc do tang exp ky nang: gamesetting.ini Skill90Rate / Skill120Rate
--
-- Bang duoi day chi danh cho exp THUONG cua nhiem vu / hoat dong - phan that
-- su nam trong script. Duoc do vao o dot ke tiep.

tbCFG_EXP = {
-- [CFGTRAN 29/08] TRAN EXP/NGAY TU BAO RUONG (tran kinh te an)
-- Ap cho CA CHIN loai bao ruong. Don vi: TRIEU exp.
-- Nguon: script\vng_event\change_request_baoruong\exp_award.lua

BRXP_MOC_CS       = 4    ,	-- moc chuyen sinh: <= so nay dung tran CS4, +1 dung CS5
BRXP_TRAN_CS4     = 50   ,	-- tran exp/ngay tu MOI bao ruong (trieu) - chuyen sinh thap
BRXP_TRAN_CS5     = 80   ,	-- tran exp/ngay (trieu) - chuyen sinh dung moc+1
BRXP_TRAN_CS6     = 100  ,	-- tran exp/ngay (trieu) - chuyen sinh cao hon

}
