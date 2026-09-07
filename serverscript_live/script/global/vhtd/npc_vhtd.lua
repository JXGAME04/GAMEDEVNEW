-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local vhtd_bynpc
-- [VHTD 02/09d] NPC Vu Hon (11) / Tieu Dao (12) - TOA DO LINUX: script\wumumenpai\npc_wumumenpai.lua (6 thon + map 1042),
-- script\xiaoyao\npc\npc_xiaoyao.lua (8 thon + map xiaoyao; Linux 1046 = map JX1 1057, cung .wor). id NPC Linux = id JX1 (VLTK).
-- Bo: Linux 2476 Cat Khang / 2477 Thu Nhue (JX1 la Nam/Nu De Tu Vu Hon), De Tu Kha Nghi 2501, Thi The 2495 (NPC nhiem vu Linux).
vhtd_parserby = {
{2467,53,1624,3197,"\\script\\npcthon\\npcmonphai\\vuhon.lua","Vò Hån §­êng HËu Qu©n"},
{2467,20,3556,6187,"\\script\\npcthon\\npcmonphai\\vuhon.lua","Vò Hån §­êng HËu Qu©n"},
{2467,99,1654,3196,"\\script\\npcthon\\npcmonphai\\vuhon.lua","Vò Hån §­êng HËu Qu©n"},
{2467,100,1616,3172,"\\script\\npcthon\\npcmonphai\\vuhon.lua","Vò Hån §­êng HËu Qu©n"},
{2467,101,1690,3114,"\\script\\npcthon\\npcmonphai\\vuhon.lua","Vò Hån §­êng HËu Qu©n"},
{2467,121,1962,4511,"\\script\\npcthon\\npcmonphai\\vuhon.lua","Vò Hån §­êng HËu Qu©n"},
{2607,53,1609,3215,"\\script\\npcthon\\npcmonphai\\tieudao.lua","Tiªu Dao MËt Sø"},
{2607,20,3575,6222,"\\script\\npcthon\\npcmonphai\\tieudao.lua","Tiªu Dao MËt Sø"},
{2607,99,1691,3193,"\\script\\npcthon\\npcmonphai\\tieudao.lua","Tiªu Dao MËt Sø"},
{2607,100,1641,3209,"\\script\\npcthon\\npcmonphai\\tieudao.lua","Tiªu Dao MËt Sø"},
{2607,101,1703,3125,"\\script\\npcthon\\npcmonphai\\tieudao.lua","Tiªu Dao MËt Sø"},
{2607,121,1966,4480,"\\script\\npcthon\\npcmonphai\\tieudao.lua","Tiªu Dao MËt Sø"},
{2607,153,1650,3201,"\\script\\npcthon\\npcmonphai\\tieudao.lua","Tiªu Dao MËt Sø"},
{2607,174,1579,3267,"\\script\\npcthon\\npcmonphai\\tieudao.lua","Tiªu Dao MËt Sø"},
{2468,1042,1693,3046,"\\script\\npcthon\\npcmonphai\\vuhon.lua","Nh¹c L«i"},
{2469,1042,1683,3048,"\\script\\global\\vhtd\\npc_chao.lua","Ng­u Th«ng"},
{2470,1042,1693,3059,"\\script\\global\\vhtd\\npc_chao.lua","Hµn Th­îng §øc"},
{2471,1042,1749,3254,"\\script\\global\\vhtd\\npc_chao.lua","Thi Phong"},
{2472,1042,1775,3197,"\\script\\global\\vhtd\\npc_chao.lua","Thang Anh"},
{2473,1042,1601,3020,"\\script\\global\\vhtd\\npc_chao.lua","Hµn Khëi Ph­îng"},
{2474,1042,1591,2999,"\\script\\global\\vhtd\\npc_chao.lua","Nh¹c §Þnh"},
{2475,1042,1625,3130,"\\script\\global\\vhtd\\npc_chao.lua","Ngò Liªn"},
{2480,1042,1787,3205,"\\script\\global\\vhtd\\npc_chao.lua","Y S­"},
{2481,1042,1806,3237,"\\script\\global\\vhtd\\npc_chao.lua","D­ §¹i Chïy"},
{2482,1042,1673,3304,"\\script\\global\\vhtd\\npc_chao.lua","Nh¹c NhÞ N­¬ng"},
{2483,1042,1649,3023,"\\script\\global\\vhtd\\npc_chao.lua","Nh¹c Phi Y Quan Gia"},
{2490,1042,1552,3067,"\\script\\global\\vhtd\\npc_chao.lua","LuyÖn Vâ Trµng Gi¸o Quan"},
{2494,1042,1574,3210,"\\script\\global\\vhtd\\npc_chao.lua","Manh Mèi"},
{1846,1042,1773,3268,"\\script\\global\\vhtd\\thuyenphu_vuhon.lua","ThuyÒn Phu"},
{1846,1042,1688,3314,"\\script\\global\\vhtd\\thuyenphu_vuhon.lua","ThuyÒn Phu"},
{625,1042,1558,3195,"\\script\\global\\npcchucnang\\ruongchua.lua","R­¬ng Chøa §å"},
{2608,1057,1629,3216,"\\script\\npcthon\\npcmonphai\\tieudao.lua","V¨n B¸n S¬n"},
{2611,1057,1750,3225,"\\script\\global\\vhtd\\npc_chao.lua","Ch­ëng M«n Lý SÊu Ngäc"},
{2609,1057,1645,3281,"\\script\\global\\vhtd\\npc_chao.lua","H­íng Nam Chi"},
{2610,1057,1666,3112,"\\script\\global\\vhtd\\npc_chao.lua","LiÔu Tam BiÕn"},
{2612,1057,1578,3251,"\\script\\global\\vhtd\\npc_chao.lua","Bïi Linh Lung"},
{2613,1057,1697,3151,"\\script\\global\\vhtd\\npc_chao.lua","CÇm §ång"},
{2614,1057,1696,3168,"\\script\\global\\vhtd\\npc_chao.lua","KiÕm §ång"},
{1846,1057,1726,3426,"\\script\\global\\vhtd\\thuyenphu_tieudao.lua","ThuyÒn Phu"},
{625,1057,1674,3323,"\\script\\global\\npcchucnang\\ruongchua.lua","R­¬ng Chøa §å"},
}

function add_npc_vhtd()
	vhtd_bynpc(vhtd_parserby)
end

function vhtd_bynpc(Tab)
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
