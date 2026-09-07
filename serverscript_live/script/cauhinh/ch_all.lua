-- ================================================================================================
-- [HE THONG] script/cauhinh/ch_all.lua
-- Muc dich  : GOM cau hinh: Include tat ca ch_*.lua. Duoc timerserver/lib_server nap; sua nong duoc (1 phut).
-- Duoc nap  : Include tu 1 tep (vd cfgw_driver.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Include   : ch_lib.lua, ch_chung.lua, ch_lich.lua, ch_thuong.lua, ch_thuong_lib.lua, ch_exp.lua, ch_drop.lua
-- Ham       : (khong co - tep du lieu/cau hinh)
-- Sua nong  : CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- ============================================================================
-- CH_ALL.LUA - nap CA BO cau hinh
-- [CAUHINH 29/08]
--
-- Day la tep DUY NHAT trong thu muc nay duoc phep Include. Dung no khi mot
-- tinh nang can nhieu nhom cau hinh; con neu chi can mot nhom thi Include
-- thang tep do cho nhe.
-- ============================================================================

Include("\\script\\cauhinh\\ch_lib.lua")
Include("\\script\\cauhinh\\ch_chung.lua")
Include("\\script\\cauhinh\\ch_lich.lua")
Include("\\script\\cauhinh\\ch_thuong.lua")
-- [PHANBIEN 29/08] thieu dong nay thi ai lam dung quang cao "nap CA BO"
-- roi goi G_TraoThuong se dut ngay luc trao thuong.
Include("\\script\\cauhinh\\ch_thuong_lib.lua")
Include("\\script\\cauhinh\\ch_exp.lua")
Include("\\script\\cauhinh\\ch_drop.lua")
