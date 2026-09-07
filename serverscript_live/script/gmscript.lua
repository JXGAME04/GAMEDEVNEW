-- ================================================================================================
-- [HE THONG] script/gmscript.lua
-- Muc dich  : Diem ha canh lenh GM 'dw ...' (GlobalExecute) - port WLLS 20/08.
-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng) + C++ goi truc tiep theo ten tep
-- Include   : wlls_gmscript.lua, tongwar_gmscript.lua
-- Ham       : (khong co - tep du lieu/cau hinh)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- gmscript.lua - diem ha canh cua GlobalExecute "dw ..." (WLLS port 20/08/2026).
-- Linux goc: \script\gmscript.lua:7 Include wlls_gmscript.lua; cac chuoi "dw"
-- (nay qua KJx2DeferredExec, hoan 1 tick) chay trong state cua file nay.
Include("\\script\\missions\\leaguematch\\wlls_gmscript.lua");
Include("\\script\\event\\tongwar\\tongwar_gmscript.lua");	-- [TONGWAR 23/08] dw tongwar_start/tongwar_gw_say ha canh state nay (Linux gmscript.lua:9)
