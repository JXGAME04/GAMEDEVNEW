-- ============================================================================
-- CH_LICH.LUA - THOI GIAN moi hoat dong / su kien
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

-- Quy uoc dat ten khoa:  <TIENTO>_GIO_<viec>   gio dang HHMM (2000 = 20:00)
--                        <TIENTO>_THU_<viec>   thu trong tuan (0=CN..6=T7)
--                        <TIENTO>_PHUT_<viec>  do dai tinh bang phut
--                        <TIENTO>_BAT          cong tac 0/1
-- Tien to dang dung: TW_ (bang chien) BR_ (bach nhan) BW_ (ty vo)
--                    TC_ (thanh bao) YDBZ_ (viem de) - 5 nhom NAY DA CO SAN
--                    trong script\header\cauhinh_hoatdong.lua, KHONG chep
--                    lai vao day de tranh hai noi khai mot khoa.
-- Cac hoat dong con lai duoc do vao day o dot ke tiep (dang khao sat).

tbCFG_LICH = {
-- [CONGTAC 29/08] CONG TAC BAT/TAT HOAT DONG
-- 1 = bat, 0 = tat. Gia tri duoi day DUNG BANG trang thai dang chay
-- hom nay, nen file nay khong lam doi gi ca cho toi khi ban sua.
-- Sua xong phai KHOI DONG LAI GameServer.
--
-- LUU Y: hoat dong dang tat lau ngay co the chua duoc nap thu vien.
-- Bat len ma khong thay chay thi xem logs\hethong.log - se co dong
-- "<khoa> bat nhung ham <ten> chua nap".

BAT_LIENDAU           = 0,	-- Lien Dau
BAT_TONGKIM           = 1,	-- Tong Kim
BAT_CONGTHANH_VIET    = 0,	-- Cong Thanh ban Viet
BAT_HATHUYHOANG       = 0,	-- Ha Thuy Hoang
BAT_BOSS_HOANGKIM     = 0,	-- Boss Hoang Kim
BAT_VUOTAI_VIET       = 0,	-- Vuot Ai ban Viet
BAT_PHONGLANGDO_VIET  = 0,	-- Phong Lang Do ban Viet
BAT_HOADANG           = 0,	-- Hoa Dang / Trang Nguyen
BAT_LOIDAI_BANGHOI    = 0,	-- Loi Dai Bang Hoi
BAT_TRONG_BANGHOI     = 0,	-- Trong Bang Hoi
BAT_KIEMMONQUAN       = 0,	-- Kiem Mon Quan
BAT_VANTIEU_LOA       = 0,	-- loa Van Tieu (NPC van chay khong can khoa nay)
BAT_DUATOP            = 0,	-- Dua Top
BAT_SONGBAC           = 0,	-- Song Bac
BAT_LOIDAI_HONCHIEN   = 0,	-- Loi Dai Hon Chien
BAT_CHECK_KICK        = 0,	-- tu kich tai khoan
BAT_PUBG              = 0,	-- Sinh Ton (PUBG)
BAT_HD3               = 1,	-- cum 3 hoat dong Linux: Sat Thu / Phong Lang Do / Vuot Ai
BAT_VIEMDE            = 1,	-- Viem De Bao Tang
BAT_CTC_JX2           = 1,	-- Cong Thanh + Loi Dai bang hoi (ban JX2)

}
