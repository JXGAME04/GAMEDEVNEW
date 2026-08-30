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

}
