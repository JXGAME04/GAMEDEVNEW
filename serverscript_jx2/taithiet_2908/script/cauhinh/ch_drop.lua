-- ============================================================================
-- CH_DROP.LUA - rot do phan chinh duoc bang script
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

-- !! DANH MUC vat pham + ti le rot nam o 49 tep settings\droprate\**\*.ini
--    (nap lai NGAY moi lan rot do - KHONG can restart).
--    Viec CHON tep .ini nao theo cap quai nam o
--    script\global\LuaNpcMonsters\Droprate_normal.lua:24-95
--    Xac suat rot cua quai thuong: Droprate_normal.lua:113-118 (hien la 3/31).
--
-- Bang duoi day danh cho cac hang so ti le trong script (lib_sukien.lua...).
-- Duoc do vao o dot ke tiep.

tbCFG_DROP = {

}
