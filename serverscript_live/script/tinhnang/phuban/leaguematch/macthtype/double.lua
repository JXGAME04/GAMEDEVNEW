--==============Ë«ÈËÈüÏà¹ØÊý¾Ý===============
Include("\\script\\missions\\leaguematch\\tb_head.lua")

local tbLianSaiLiBao = {6, 1, 4416,1,0,0}

--È«²¿Ë«ÈËÈüÏà¹ØÊý¾Ý
local WLLS_DATA = {

	name = "Song ®Êu",
	
	--µØÍ¼ÐÅÏ¢
	match_type = {
		{
			name = "",
			level = 1,
			ladder = 10196,
			map = {
				{396, 560, 397, "(1)"},
				{398, 561, 399, "(2)"},
				{400, 562, 401, "(3)"},
				{402, 563, 403, "(4)"},
			},
		},
		{
			name = "",
			level = 2,
			ladder = 10197,
			map = {
				{540, 570, 541, "(1)"},
				{542, 571, 543, "(2)"},
				{544, 572, 545, "(3)"},
				{546, 573, 547, "(4)"},
			},
		},
	},
	
	award_rank = {
		{	--ÅÅÐÐ½±Àø£¬ÐÂÐã
			{1, 5000},	--µÚ1Ãû
			{2, 4000},	--µÚ2Ãû
			{3, 3500},	--µÚ3Ãû
			{4, 3000},	--µÚ4Ãû
			{8, 2000},	--µÚ5-8Ãû
			{16, 1500},	--µÚ9-16Ãû
			{32, 1000},	--µÚ17-32Ãû
			{64, 500},	--µÚ33-64Ãû
			{128, 250},	--µÚ65-128Ãû
		}, {	--ÁªÈü
			{1, 10000, nil, {tbProp=tbLianSaiLiBao,nCount=20}, {tbProp={6,1,4550,1,0,0}, nCount=20}},	--µÚ1Ãû
			{2, 7000, nil, {tbProp=tbLianSaiLiBao,nCount=14}, {tbProp={6,1,4550,1,0,0}, nCount=14}},	--µÚ2Ãû
			{3, 6000, nil, {tbProp=tbLianSaiLiBao,nCount=12}, {tbProp={6,1,4550,1,0,0}, nCount=12}},	--µÚ3Ãû
			{4, 5000, nil, {tbProp=tbLianSaiLiBao,nCount=10}, {tbProp={6,1,4550,1,0,0}, nCount=10}},	--µÚ4Ãû
			{8, 3000, nil, {tbProp=tbLianSaiLiBao,nCount=6}, {tbProp={6,1,4550,1,0,0}, nCount=6}},	--µÚ5-8Ãû
			{16, 2500, nil, {tbProp=tbLianSaiLiBao,nCount=5}, {tbProp={6,1,4550,1,0,0}, nCount=5}},	--µÚ9-16Ãû
			{32, 2000, nil, {tbProp=tbLianSaiLiBao,nCount=4}, {tbProp={6,1,4550,1,0,0}, nCount=4}},	--µÚ17-32Ãû
			{64, 1500, nil, {tbProp=tbLianSaiLiBao,nCount=3}, {tbProp={6,1,4550,1,0,0}, nCount=3}},	--µÚ33-64Ãû
			{128, 1000, nil, {tbProp=tbLianSaiLiBao,nCount=2}, {tbProp={6,1,4550,1,0,0}, nCount=2}},	--µÚ65-128Ãû
		}
	},
	
	max_member = 2,	--Ã¿¸öÕ½¶ÓµÄ×î´óÈËÊý
	
	text_main = {	--officerÖ÷¶Ô»°£¨Ä©Î²²îÒì²¿·Ö£©
		"Vâ l©m liªn ®Êu kiÖt xuÊt lÇn nµy lµ <color=red>Song nh©n ®Êu<color>",
		"Vâ l©m liªn ®Êu kiÖt xuÊt lÇn nµy lµ <color=red>Song nh©n ®Êu<color>",
	},
	
	--====Functions====
	--ÎÒµÄÕ½¶Ó
	npc_mylg = function()
		local str_des = wlls_get_desc(1)
		local n_lid, n_mtype, n_job, str_lgname, n_memcount = wlls_lg_info()
		if (FALSE(n_lid)) then
			wlls_descript("<enter>  LÇn nµy"..str_des.."lµ <color=red>Song ®Êu<color>. B¹n cã thÓ tù chän ®éi thi ®Êu cho m×nh hay gia nhËp vµo ®éi thi ®Êu kh¸c. Sau khi ®éi tr­ëng lËp nhãm víi ng­êi ch¬i kh¸c ®Õn"..str_des.."§èi tho¹i víi Quan viªn, chän '§éi thi ®Êu"..str_des.."cña t«i', chän nh÷ng ®éi h÷u gia nhËp trong ®éi m×nh lµ ®­îc. Thµnh viªn ®éi thi ®Êu nhiÒu nhÊt <color=red>2 ng­êi<color>",
				"Ta muèn lËp"..str_des.."nhãm/wlls_want2create",
				"Ta chØ ®Õn xem th«i/OnCancel")
			return
		end
	
		local n_mytype = LG_GetLeagueTask(n_lid, WLLS_LGTASK_STYPE)
		local n_level, n_group = wlls_getcityinfo()
		local n_next = GetGlbValue(GLB_WLLS_NEXT)
		local str = "<enter>ChiÕn ®éi cña b¹n hiÖn lµ:<color=yellow>"..str_lgname.."<color><enter>"
		.. "§éi tr­ëng ®éi chiÕn ®Êu:"..join(wlls_get_mems(n_lid), "Thµnh viªn ®éi chiÕn ®Êu:") .. "<enter>"
		if (n_mytype ~= n_next) then
			str = str .. "<enter>    <color=yellow>Chó ý: H¹n chÕ thi ®Êu mïa sau lµ <color><color=red>"..WLLS_TAB[n_next].name.."<color><color=yellow>, nÕu muèn thi ®Êu mïa sau, xin h·y lËp chiÕn ®éi míi.<color><enter>"
		end
		local tb_option = {}
		if (n_job == 1 and n_mytype == n_next) then
			str = str.."<enter>   B¹n cã thÓ chän <color=red> gia nhËp vµo ®éi kh¸c<color> hoÆc <color=red>rêi khái ®éi thi ®Êu<color>.<enter><enter>"..
			"Qu¸ tr×nh thµnh viªn lËp nhãm: Trong <color=red>thêi gian nghØ ng¬i vµ thêi gian thi ®Êu<color>, b¹n cã thÓ cïng víi ng­êi ch¬i kh¸c lËp nhãm, chän nh÷ng ®éi h÷u gia nhËp vµo ®éi thi ®Êu.<enter>"..
			"Rêi khái ®éi thi ®Êu: Trong <color=red>thêi gian nghØ ng¬i<color>, b¹n cã thÓ rêi khái ®éi thi ®Êu; trong thêi gian thi ®Êu nÕu ®éi thi ®Êu cña b¹n<color=red> ch­a gia nhËp thi ®Êu kú tr­íc<color>, b¹n cã thÓ rêi khái ®éi thi ®Êu; nÕu ®· cã tham gia th× kh«ng thÓ rêi ®éi. T­ c¸ch ®éi tr­ëng sÏ ®­îc chuyÓn giao l¹i cho thµnh viªn kh¸c, <color=yellow>nÕu kh«ng cã thµnh viªn, chiÕn ®éi sÏ bÞ gi¶i t¸n<color>."
			tb_option[getn(tb_option)+1] = "§Ó ®éi h÷u cña Tagia nhËp vµo ®éi thi ®Êu nµy/wlls_want2addmember"
		else
			str = str.."<enter>       B¹n cã thÓ chän rêi khái nhãm.<enter><enter>"..
			"Rêi khái nhãm:   Trong <color=red>thêi gian nghØ ng¬i<color>, b¹n cã thÓ rêi khái ®éi thi ®Êu; trong thêi gian thi ®Êu nÕu ®éi thi ®Êu m×nh <color=red>ch­a tham gia gi¶i ®Êu tr­íc<color>, b¹n còng cã thÓ rêi khái ®éi thi ®Êu; cßn nÕu ®· gia nhËp thi ®Êu kh«ng thÓ rêi khái ®éi."
		end
		tb_option[getn(tb_option)+1] = "Rêi khái ®éi/wlls_want2leaveleague"
		tb_option[getn(tb_option)+1] = "Xem chiÕn tÝch cña ®éi/wlls_query_mylg"
		tb_option[getn(tb_option)+1] = "Ta chØ tiÖn ghÐ qua/OnCancel"
		wlls_descript(str, tb_option)
	end,
	
}	--WLLS_DATA

tmp_process_data(WLLS_DATA)	
wllstab_additem(1, WLLS_DATA)
