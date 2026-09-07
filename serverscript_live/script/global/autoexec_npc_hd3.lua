-- ================================================================================================
-- [HE THONG] script/global/autoexec_npc_hd3.lua
-- Muc dich  : Tu dat NPC 3 hoat dong luc boot.
-- Duoc nap  : Include tu 1 tep (vd hd3_driver.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Ham (dong): add_dialognpc_hd3 (16)
-- Sua nong  : CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- ============================================================================
-- AUTOEXEC_NPC_HD3.LUA - 7 NPC 769 "Nhiep Thi Tran" cua he SAN BOSS SAT THU
-- Trich NGUYEN BYTE tu ban Linux (autoexec_npc.lua). add_dialognpc_hd3 dung
-- AddNpcEx (khong dung AddNpc - tranh loi ngu hanh Kim, xem BANGIAO 3HD).
-- Cot: {npcId, mapId, x, y, script, name}
-- ============================================================================
adddialognpc_hd3 = {
	{769,11,3210,4974,"\\script\\task\\tollgate\\killer\\nieshichen.lua","NhiÕp ThÝ TrÇn"},
	{769,1,1506,3198,"\\script\\task\\tollgate\\killer\\nieshichen.lua","NhiÕp ThÝ TrÇn"},
	{769,37,1647,3050,"\\script\\task\\tollgate\\killer\\nieshichen.lua","NhiÕp ThÝ TrÇn"},
	{769,176,1372,3010,"\\script\\task\\tollgate\\killer\\nieshichen.lua","NhiÕp ThÝ TrÇn"},
	{769,162,1573,3227,"\\script\\task\\tollgate\\killer\\nieshichen.lua","NhiÕp ThÝ TrÇn"},
	{769,78,1512,3206,"\\script\\task\\tollgate\\killer\\nieshichen.lua","NhiÕp ThÝ TrÇn"},
	{769,80,1700,2963,"\\script\\task\\tollgate\\killer\\nieshichen.lua","NhiÕp ThÝ TrÇn"},
}

function add_dialognpc_hd3(Tab)
	for i = 1, getn(Tab) do
		local it = Tab[i]
		local SId = SubWorldID2Idx(it[2])
		if (SId >= 0) then
			local npc = HD3_AddNpc(it[1], 1, SId, it[3]*32, it[4]*32, 0, it[6])
			if (npc ~= nil and npc > 0) then
				SetNpcScript(npc, it[5])
			end
		end
	end
end
