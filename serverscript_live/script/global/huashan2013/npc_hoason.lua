-- [HOASON 01/09] NPC Hoa Son - RUT GON theo y chu (khong nhiem vu, nhan ky nang nhu 9 phai co san).
-- Toa do 8 thon + map 987 lay nguyen bang Linux script\global\huashan2013\npc_hoason.lua (hoason_parserby).
-- Quai/NPC nhiem vu Linux (2090 luyen kiem, 2092 cuong dao, 2097 Son Phi, 2103/2108/2110, Van Tu Vien...) KHONG spawn;
-- kho luu: ReverseTools\phai3\hoason_thicong\huashan2013_nhiemvu_linux_khongdung.
hoason_parserby = {
{2096,53,1632,3191,"\\script\\npcthon\\npcmonphai\\hoason.lua","Hoa S¬n KiÕm Kh¸ch"},
{2096,20,3567,6190,"\\script\\npcthon\\npcmonphai\\hoason.lua","Hoa S¬n KiÕm Kh¸ch"},
{2096,99,1641,3189,"\\script\\npcthon\\npcmonphai\\hoason.lua","Hoa S¬n KiÕm Kh¸ch"},
{2096,100,1653,3129,"\\script\\npcthon\\npcmonphai\\hoason.lua","Hoa S¬n KiÕm Kh¸ch"},
{2096,101,1693,3167,"\\script\\npcthon\\npcmonphai\\hoason.lua","Hoa S¬n KiÕm Kh¸ch"},
{2096,121,1966,4508,"\\script\\npcthon\\npcmonphai\\hoason.lua","Hoa S¬n KiÕm Kh¸ch"},
{2096,153,1638,3240,"\\script\\npcthon\\npcmonphai\\hoason.lua","Hoa S¬n KiÕm Kh¸ch"},
{2096,174,1596,3271,"\\script\\npcthon\\npcmonphai\\hoason.lua","Hoa S¬n KiÕm Kh¸ch"},
{2098,987,1420,3049,"\\script\\npcthon\\npcmonphai\\hoason.lua","Nam Cung TuyÕt"},	-- chuong mon, cung menu nhu De Tu o thon
{2096,987,1358,3051,"\\script\\npcthon\\npcmonphai\\hoason.lua","LËn H¹o Thiªn"},
{625,987,1391,3114,"\\script\\global\\npcchucnang\\ruongchua.lua","R­¬ng chøa ®å"},
}

function add_npc_hoason()
	hoason_hoasonbynpc(hoason_parserby)
end

function hoason_hoasonbynpc(Tab)
	for i = 1 , getn(Tab) do
		local SId = SubWorldID2Idx(Tab[i][2]);
		if (SId >= 0) then
			local npcindex = AddNpc(Tab[i][1],1,SId,Tab[i][3]*32,Tab[i][4]*32,1,Tab[i][6]);
			if (npcindex > 0) then
				SetNpcScript(npcindex, Tab[i][5]);
				SetNpcValue(npcindex, 10);
			end
		end;
	end
end;
