Include("\\script\\lib\\file.lua")	-- [KIEM CHUNG 24/08] tep nay goi GetTabFileHeight/
-- GetTabFileData nhung truoc day KHONG Include gi: chi chay nho state startgame vo tinh co
-- ham do qua duong vong bairen_boot. Tat Bach Nhan la trap 6 map Bang Chien hong am tham.
-- (engine tu anh xa script\lib\file.lua -> scriptjx2\lib\file.lua, xem sJX2RemapScriptPath)
tbTONGWAR_MAP = { 608, 609, 610, 611, 612, 613 }
tbTONGWAR_NPC = {
	{ 393, 1597, 3139, "Xa phu",		"\\script\\missions\\tongwar\\npc\\chefu.lua" },
	{ 625, 1590, 3132, "R­¬ng chøa ®å",	"\\script\\missions\\tongwar\\npc\\openbox.lua" },
	{ 389, 1580, 3151, "Chñ d­îc ®iÕm",		"\\script\\missions\\tongwar\\npc\\doctor.lua" },
--	{389, 1589, 3148, "trap", "\\script\\missions\\tongwar\\trap\\tongwar_trap.lua"}
	
}
TONGWAR_TRAPTXT = "\\settings\\maps\\tongwar\\signup_trap.txt"
function tongwar_addsignnpc()
local oldSubWorld = SubWorld
	for i = 1, getn( tbTONGWAR_MAP ) do
		local sidx = SubWorldID2Idx( tbTONGWAR_MAP[i] )
		if ( sidx >= 0 ) then
			SubWorld = sidx
			for j = 1, getn(tbTONGWAR_NPC) do
				local npcidx = AddNpc( tbTONGWAR_NPC[j][1], 1, sidx, tbTONGWAR_NPC[j][2]*32, tbTONGWAR_NPC[j][3]*32, 1, tbTONGWAR_NPC[j][4] )
				if (npcidx > 0) then
					SetNpcScript(npcidx, tbTONGWAR_NPC[j][5]);
				end
			end
			maphigh = GetTabFileHeight(TONGWAR_TRAPTXT)
			for j = 1, maphigh do
				x = GetTabFileData(TONGWAR_TRAPTXT, j + 1, 1);
				y = GetTabFileData(TONGWAR_TRAPTXT, j + 1, 2);
				AddMapTrap(tbTONGWAR_MAP[i], x,y, "\\script\\missions\\tongwar\\trap\\tongwar_trap.lua");
			end;
			SubWorld = oldSubWorld
		end
	end
	-- [TONGWAR 23/08] goc: NPC "Su gia Thien Ha De Nhat Bang" chi hien 07-18/04/2014 (su kien hau mua
	-- VNG; npc_shizhe.lua giu nguyen trang). Loi vao thay the: NPC "Vo Lam Truyen Nhan" tpl 53
	-- (Linux KHONG co loi vao - tongWar_Start 0 call site), dat Ba Lang Huyen canh Su Gia Cong Thanh.
	local sidx = SubWorldID2Idx(53);
	if (sidx >= 0) then
		SubWorld = sidx;
		local npcidx = AddNpc(53, 1, sidx, 1628*32, 3173*32, 1, "Vâ L©m TruyÒn Nh©n");
		if (npcidx > 0) then
			SetNpcScript(npcidx, "\\script\\event\\tongwar\\npc_tongwar.lua");
		end;
		SubWorld = oldSubWorld;
	end;
end