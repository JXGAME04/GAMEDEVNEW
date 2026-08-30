-- ============================================================================
-- CH_CHUNG.LUA - thong so chung toan game
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
--   logs\hethong.log  (ham GhiLog cua script - dang ky o ScriptFuns.cpp)
--   ScriptError.log    (loi runtime Lua, co ca stack traceback + tep:dong)
CH_LOG_CAUHINH = 1,
CH_LOG_THUONG = 1,

}
