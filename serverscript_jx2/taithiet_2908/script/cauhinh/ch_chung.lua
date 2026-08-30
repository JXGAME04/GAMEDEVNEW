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

-- [CFGNEN 30/08] HE SO NEN TOAN CUC (script\lib\lib_server.lua)
-- 
-- Day la tep 14 dong khai cac he so nhan vao rat nhieu noi khac.
-- Moi so DUNG BANG gia tri dang chay. Sua xong KHOI DONG LAI GameServer.
-- 
-- !! GLB_CHE_DO_TEST dang la 1: hai NPC 'Ho Tro Test' o lang tan thu (ban do 53)
-- !! mo menu GM day du cho MOI nguoi choi - 1 ty luong + 100.000 Xu + len
-- !! thang cap 200, khong kiem tai khoan GM, khong gioi han so lan.
-- !! Dat 0 de doi ve NPC tan thu that. Xem BAOCAO_LOHONG_2908.md muc 1.
-- 
-- !! GLB_GIO_MO_SERVER bi TRUNG TEN voi mot ban trong script\lib\worldlibrary.lua:4
-- !! (gia tri cu 2104011900). Ban do van chay nhung khong bao gio thang.

GLB_GIO_MO_SERVER         = 2506251900  ,	-- moc gio mo cua, dang yymmddHHMM. 7 cho trong game doc de chan nguoi choi
GLB_CHE_DO_TEST           = 1           ,	-- 1 = MO che do thu nghiem. !! Dang mo: hai NPC 'Ho Tro Test' o lang tan thu phat do GM cho MOI nguoi choi. Dat 0 de tat
GLB_TILE_TIEN             = 1           ,	-- he so nhan tien thuong toan server (1 = giu nguyen)
GLB_TILE_EXP              = 20          ,	-- he so nhan kinh nghiem toan server. NUT CHINH de dieu chinh toc do len cap
GLB_SATTHUONG_BOSS_HK     = 1           ,	-- he so sat thuong Boss Hoang Kim tieu. (dang tat cung hoat dong)
GLB_SATTHUONG_QUAI_PLD    = 1           ,	-- he so sat thuong quai Thuy Tac tren thuyen Phong Lang Do ban Viet cu. (dang tat)
GLB_SATTHUONG_BOSS_TTDL   = 1           ,	-- he so sat thuong boss Thuy Tac Dau Linh ban Viet cu. (dang tat)
GLB_MANH_BOSS_SATTHU      = 1           ,	-- he so mau + sat thuong 20 boss Sat Thu ban Viet cu. (dang tat)
GLB_MANH_BOSS_VUOTAI      = 1           ,	-- he so mau + sat thuong quai va boss Vuot Ai ban Viet cu. (dang tat)
GLB_MANH_NGUYENSOAI_TK    = 1           ,	-- he so mau + sat thuong Nguyen Soai hai phe trong Tong Kim - DANG CHAY
GLB_MAX_DONG_THUOCTINH    = 10          ,	-- tran so dong thuoc tinh do roi tu quai. !! khong tep script nao doc so nay - so that dang chan nam trong ma C++

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
