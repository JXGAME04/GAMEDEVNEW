-- ============================================================================
-- CH_THUONG.LUA - BANG THUONG moi hoat dong / su kien
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

-- Quy uoc: moi bang thuong la mang cac muc
--     { <so luong>, {<genre>, <detail>, <particular>}, "<ten de doi chieu>" }
-- Ten de doi chieu KHONG dung de tim vat pham - no chi de nguoi doc biet mon
-- gi, va de bo kiem `ReverseTools\cauhinh\ktr_cauhinh.py` doi chieu voi
-- settings\item\magicscript.txt roi BAO LOI neu lech.
--
-- !! Ma vat pham cua ban Linux (dai 30000+, hoac {6,0,x}) KHONG dung duoc o
--    JX1: magicscript cua du an CHI co detail = 1. Xem thieu_vatpham.txt.

tbCFG_THUONG = {

}
