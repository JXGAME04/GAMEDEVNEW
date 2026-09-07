-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local tinsu_add_dialognpc
-- ============================================================
-- TIN SU (port Linux 21/08): dat NPC cho 3 ai Tin Su + quai/bao ruong cua ai
-- Linux: global\autoexec.lua main() goi add_alltollgatenpc() (task\tollgate\addtollgatenpc.lua)
-- va add_dialognpc(adddialognpc) voi bang trong global\autoexec_npc.lua. Du an khong co
-- autoexec.lua -> gom vao day, startgame.lua goi tinsu_addnpc() (thay addnpcthienbaokho() cu).
-- Bang tinsu_dialognpc = chep NGUYEN VAN dong 3-11 + 13-23 cua autoexec_npc.lua Linux
-- (map 387-394 du an chua co .wor -> SubWorldID2Idx < 0 -> tu bo qua, giong Linux).
-- ============================================================
Include("\\script\\task\\tollgate\\addtollgatenpc.lua")

tinsu_dialognpc = {
	{822,387,1306,2564,"\\script\\task\\tollgate\\messenger\\fengzhiqi\\messenger_flynpc.lua","C¸nh tr¾ng"},
	{822,388,1306,2564,"\\script\\task\\tollgate\\messenger\\fengzhiqi\\messenger_flynpc.lua","C¸nh tr¾ng"},
	{822,389,1306,2564,"\\script\\task\\tollgate\\messenger\\fengzhiqi\\messenger_flynpc.lua","C¸nh tr¾ng"},
	{832,390,1586,2600,"\\script\\task\\tollgate\\messenger\\shanshenmiao\\messenger_temnpc.lua","N¹p Lan Thanh Thµnh"},
	{832,391,1586,2600,"\\script\\task\\tollgate\\messenger\\shanshenmiao\\messenger_temnpc.lua","N¹p Lan Thanh Thµnh"},
	{832,392,1586,2600,"\\script\\task\\tollgate\\messenger\\shanshenmiao\\messenger_temnpc.lua","N¹p Lan Thanh Thµnh"},
	{842,393,1386,2442,"\\script\\task\\tollgate\\messenger\\qianbaoku\\messenger_turenpc.lua","Tiªu TrÊn"},
	{842,394,1386,2442,"\\script\\task\\tollgate\\messenger\\qianbaoku\\messenger_turenpc.lua","Tiªu TrÊn"},
	{842,395,1386,2442,"\\script\\task\\tollgate\\messenger\\qianbaoku\\messenger_turenpc.lua","Tiªu TrÊn"},
	{377,387,1570,3132,"\\script\\task\\tollgate\\messenger\\messenger_flyrukou.lua","DÞch quan"},
	{377,388,1570,3132,"\\script\\task\\tollgate\\messenger\\messenger_flyrukou.lua","DÞch quan"},
	{377,389,1570,3132,"\\script\\task\\tollgate\\messenger\\messenger_flyrukou.lua","DÞch quan"},
	{377,390,1320,3185,"\\script\\task\\tollgate\\messenger\\messenger_templerukou.lua","DÞch quan"},
	{377,391,1320,3185,"\\script\\task\\tollgate\\messenger\\messenger_templerukou.lua","DÞch quan"},
	{377,392,1320,3185,"\\script\\task\\tollgate\\messenger\\messenger_templerukou.lua","DÞch quan"},
	{377,393,1412,3203,"\\script\\task\\tollgate\\messenger\\messenger_turerukou.lua","DÞch quan"},
	{377,394,1412,3203,"\\script\\task\\tollgate\\messenger\\messenger_turerukou.lua","DÞch quan"},
	{377,395,1412,3203,"\\script\\task\\tollgate\\messenger\\messenger_turerukou.lua","DÞch quan"},
}

-- y het add_dialognpc() cua global\autoexec.lua Linux (dong 233-242)
function tinsu_add_dialognpc(Tab)
	for i = 1 , getn(Tab) do 
		local itemlist = Tab[i]
		local SId = SubWorldID2Idx(itemlist[2]);
		if (SId >= 0) then
			local npcindex = AddNpc(itemlist[1], 1, SId, itemlist[3] * 32, itemlist[4] * 32, 0, itemlist[6]);
			SetNpcScript(npcindex, itemlist[5]);
		end;
	end	
end;

function tinsu_addnpc()
	add_alltollgatenpc();				-- 9 Bao Ruong + 9 Bao Kho Thu Ho Gia map 395 (killbosshead.lua)
	tinsu_add_dialognpc(tinsu_dialognpc);	-- Tieu Tran + Dich quan trong ai
end
