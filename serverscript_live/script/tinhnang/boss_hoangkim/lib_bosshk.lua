-- Author: Fong Ki“u
-- Date: 28/11/2020
--Function: Lib boss ti”u

Include("\\script\\lib\\lib_server.lua")
-- [CFGBHK 30/08] hai tep duoi day la LA (khong Include gi).
Include("\\script\\cauhinh\\ch_lib.lua")
Include("\\script\\cauhinh\\ch_chung.lua")
-- [CFGBHK 30/08] Bo doc cau hinh cho tep nay. Tra ve MAC DINH (= so cu)
-- khi bo cau hinh chua nap, nen kem nhat cung khong the doi hanh vi.
function BHK_CFG(szKhoa, macdinh)
	if (G_CFG ~= nil) then
		return G_CFG(szKhoa, macdinh)
	end
	return macdinh
end

Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\log_game\\save_log.lua")
Include("\\script\\tinhnang\\boss_hoangkim\\dsbosshk.lua")

COMMON_INFO_MIN 				= "<color=green>%s <color>xu t hi÷n tπi <color=green>%s <color>"
COMMON_INFO_MAX 			= "<color=purple>%s <color>xu t hi÷n tπi <color=green>%s <color>"
TIME_LIFE_BOSS 						= BHK_CFG("BHK_PHUT_BOSS_TON_TAI", 120)*60*18 -- 120 phÛt
BOSSDAI_EXP 							= BHK_CFG("BHK_EXP_NPC_BOSS_DAI", 0)
BOSSTIEU_EXP 							= BHK_CFG("BHK_EXP_NPC_BOSS_TIEU", 0)
DROPRATEHKTIEU 					= "\\script\\tinhnang\\boss_hoangkim\\drophktieu.lua"
DROPRATEHKDAI 					= "\\script\\tinhnang\\boss_hoangkim\\drophkdai.lua"
DEATHFILEHKTIEU 					= "\\script\\tinhnang\\boss_hoangkim\\deathhktieu.lua"
DEATHFILEHKDAI 					= "\\script\\tinhnang\\boss_hoangkim\\deathhkdai.lua"
DROPRATEHKSV						= "\\script\\tinhnang\\boss_hoangkim\\drophksv.lua"
DEATHFILEHKSV			 			= "\\script\\tinhnang\\boss_hoangkim\\deathhksv.lua"
DROPRATEHKPLD						= "\\script\\tinhnang\\boss_hoangkim\\dropbosspld.lua"
DEATHFILEHKPLD			 			= "\\script\\tinhnang\\boss_hoangkim\\deathbosspld.lua"
LIFE_BOSS_TIEU 						= BHK_CFG("BHK_MAU_BOSS_TIEU", 18000000)		--mac dinh 8000000
LIFE_BOSS_DAIH 						= BHK_CFG("BHK_MAU_BOSS_DAI", 1)		--mac dinh 18000000
NETRANHBOSS 							= BHK_CFG("BHK_NETRANH_BOSS", 5000)
PHSLBOSS 									= BHK_CFG("BHK_HOIMAU_BOSS", 300)

BOSSTIEU_ARRAY={
{513, "Di÷u Nh≠", 2, {1,6}},
{523, "Li‘u Thanh Thanh", 1, {2,5}},
{511, "Tr≠¨ng T´ng Ch›nh", 4, {3,4}}
}

BOSSDAI_ARRAY={
{"Huy“n Gi∏c ßπi S≠",1365,"000",{1,6}},
{"ß≠Íng Phi Y’n",1366,"111",{2,5}},
{"Tı ßπi Nhπc",1367,"444",{3,4}}
};

BOSSSV_ARRAY = {
	-- {"L˝ Thu ThÒy"			,	743 ,{2}}, 
	-- {"Gia LuÀt Tﬁ Lﬁ"		,		563 ,{3}}, 
	-- {"Ti™u Phong"			,		583 ,{3}},  
	-- {"Tı ßπi Nhπc"			,		458 ,{4}},   
	 {"Boss Cˆu Thi™n"	,	random(2009,2010),{random(0,4)}},  
}
BOSSSV_ARRAYPLD = {
	 {"Boss Phong L®ng ßÈ"	,	random(2009,2010),{random(0,4)}},  
}

TAB_TIME_BOSS_TIEU ={
	-- {11,00},
	--{13,00},
	{15,00},
	--{17,00},
	{20,00},
	--{22,00},
}

TAB_TIME_BOSS_DAI ={
	-- {15,30},
	{11,00},
	-- {18,00},
	{17,30},
	-- {22,30},
}


TAB_TIME_BOSS_SV ={
	{12,00},
	--{17,30},
	{19,30},
--	{23,00},
	--{22,30},
}

TAB_POS_BOSSSV = {
{53,48384,105728}
}

TAB_POS_BOSSPLD = {
{336,35488,91296}
}


function addnpcbosspld()
	local nNpcIndex = 0
	local nPos = 1
	local nMsg = " "
	for i=1,getn(BOSSSV_ARRAYPLD) do
		nPos = random(1,getn(TAB_POS_BOSSPLD))
		nNpcIndex = AddNpcNew(BOSSSV_ARRAYPLD[i][2],100,TAB_POS_BOSSPLD[nPos][1],
		TAB_POS_BOSSPLD[nPos][2],TAB_POS_BOSSPLD[nPos][3],DEATHFILEHKPLD,
	    5,BOSSSV_ARRAYPLD[i][1],1,random(0,4),BOSSDAI_EXP,LIFE_BOSS_DAIH,PHSLBOSS*3,10000,NETRANHBOSS*2,nil,nil,nil,nil,80,2,DROPRATEHKPLD);
		if(nNpcIndex > 0) then
			SetNpcTimer(nNpcIndex, TIME_LIFE_BOSS)
			SetNpcDamageReduction(nNpcIndex,70)
			nMsg = format(COMMON_INFO_MAX,BOSSSV_ARRAYPLD[i][1],GetMapName(TAB_POS_BOSSPLD[nPos][1]),floor(TAB_POS_BOSSPLD[nPos][2]/8/32),floor(TAB_POS_BOSSPLD[nPos][3]/16/32))
			Msg2SubWorld(nMsg)
			logHoatDong(date("%H:%M:%S")..nMsg.."\t\n")
		end
	end
end

TAB_POS_BOSSDAI = {
{75,58912,99104},
{227,50912,100288},
{93,53760,96320},
{93,51424,104544},
{227,47392,108480},
{75,54272,100320}
}

POSITION_ARRAY={
{78,1740*32,3323*32},
{37,1559*32,3449*32},
{1,1550*32,2790*32},
{37,1655*32,2627*32},
{80,1389*32,3410*32},
{78,1677*32,3541*32}
};

QY_NORMALBOSS_INFO={
	[1]	= {n_level = 90, 	n_series = {1},	n_npcid = 523,	n_mapid = 25,	tb_coords = {{531,300}, {482,331}},	sz_name = "Li‘u Thanh Thanh"},
	[2]	= {n_level = 90, 	n_series = {2}, 	n_npcid = 513,	n_mapid = 13,	tb_coords = {{285,302}, {218,312}},	sz_name = "Di÷u Nh≠ "},
	[3]	= {n_level = 90, 	n_series = {4}, 	n_npcid = 511,	n_mapid = 81,	tb_coords = {{219,210}, {232,191}},	sz_name = "Tr≠¨ng T´ng Ch›nh"},
	--[4]	= {n_level = 90,	n_series = {1}, 	n_npcid = 1358, n_mapid = 183,	tb_coords = {{204,214}, {183,167}},	sz_name = "T©y V≠¨ng Tµ ßÈc"},
	--[5]	= {n_level = 90,	n_series = {2}, 	n_npcid = 1360, n_mapid = 154,	tb_coords = {{39,107}, {69,82}}, 	sz_name = "Do∑n Thanh V©n"},
	--[6]	= {n_level = 90,	n_series = {3}, 	n_npcid = 1361, n_mapid = 115,	tb_coords = {{195,205},{180,176}},	sz_name = "Hæc Y S∏t ThÒ"},
	--[7]	= {n_level = 90,	n_series = {0}, 	n_npcid = 1356, n_mapid = 59, 	tb_coords = {{188,195},{237,192}}, 	sz_name = "Ngπo Thi™n T≠Ìng Qu©n"},
	--[8]	= {n_level = 90,	n_series = {3}, 	n_npcid = 1362, n_mapid = 45, 	tb_coords = {{208,202},{199,192}}, 	sz_name = "ThÀp Ph≠¨ng C©u Di÷t"},
	--[9]	= {n_level = 90,	n_series = {4}, 	n_npcid = 1364,	n_mapid = 131,	tb_coords = {{173,208},{202,190}}, 	sz_name = "Thanh Y Tˆ"},
	--[10]= {n_level = 90,	n_series = {0}, 	n_npcid = 1355,	n_mapid = 103,	tb_coords = {{180,220},{199,180}}, 	sz_name = "Tﬁnh Th´ng"},
};

function LuuDanhSach()

BANGI = TaoBang(BOSSHK,"BOSSHK")
SaveData("script/tinhnang/boss_hoangkim/dsbosshk.lua",BANGI)
end

function checkbang(tenbang)
if getn(BOSSHK) == 0 then
return 0
end
for i=1,getn(BOSSHK) do
	if tenbang == BOSSHK[i][1] then
		return i
	end
end
return 0
end


function admintestboss()
	local tbOption = {
		"Function: Ch¯c n®ng ki”m tra nhanh b∂n ÆÂ Boss !",
        --"ß’n M™ Cung K’ Qu∏n/dimecungkequan",
		"ß’n Ki’m C∏c T©y Nam Pos HLP/kiemcactaynam",
		--"ß’n Kim Quang ßÈng/kimquangdong",
		--"ß’n Sa Mπc 3/disamac3",
		"Test Boss Ti”u/addnpcbosstieuhk",
		"Test Boss ßπi/addnpcbossdaihk",
	}
	for i = 1, getn(QY_NORMALBOSS_INFO) do
		tinsert(tbOption, format("ß’n %s pos 1/gobosstieufn(%d,%d,%d)", QY_NORMALBOSS_INFO[i].sz_name, QY_NORMALBOSS_INFO[i].n_mapid, QY_NORMALBOSS_INFO[i].tb_coords[1][1],QY_NORMALBOSS_INFO[i].tb_coords[1][2]))
		tinsert(tbOption, format("ß’n %s pos 2/gobosstieufn(%d,%d,%d)", QY_NORMALBOSS_INFO[i].sz_name, QY_NORMALBOSS_INFO[i].n_mapid, QY_NORMALBOSS_INFO[i].tb_coords[2][1],QY_NORMALBOSS_INFO[i].tb_coords[2][2]))
	end
	tinsert(tbOption, "Tho∏t/Exit")
    SayEx(tbOption)	
end

function gobosstieufn(w,x,y)
	NewWorld(w, x*8, y*16) SetFightState(1)
end

function kimquangdong()
	NewWorld(4, 1675, 3101) SetFightState(1)
end

function kiemcactaynam()
	NewWorld(19, 3254,4270) SetFightState(1)
end

function dimecungkequan()
	NewWorld(91, 1781,2906) SetFightState(1)
end

function disamac3()
	NewWorld(227, 1711, 3205) SetFightState(1)
end

function addnpcbosstieuhk2()
	local nNpcIndex = 0
	for i = 1, getn(QY_NORMALBOSS_INFO) do
		local nlvl, nseries, nid, nmap, tb_coords, szname	= 	QY_NORMALBOSS_INFO[i].n_level, QY_NORMALBOSS_INFO[i].n_series, QY_NORMALBOSS_INFO[i].n_npcid, QY_NORMALBOSS_INFO[i].n_mapid, QY_NORMALBOSS_INFO[i].tb_coords, QY_NORMALBOSS_INFO[i].sz_name
		local nworldidx = SubWorldID2Idx(nmap)
		if (nworldidx >= 0) then
			local nIdx		= random(getn(tb_coords))
			local nx, ny	= tb_coords[nIdx][1] * 8 * 32, tb_coords[nIdx][2] * 16 * 32
			nNpcIndex = AddNpcEx3({nid},100, nseries, nmap, nx, ny, DROPRATEHKTIEU, DEATHFILEHKTIEU, szname, 5, BOSSTIEU_EXP, LIFE_BOSS_TIEU,
			PHSLBOSS, 8000, NETRANHBOSS, 
			nil, nil, 
			nil, 80)			
			if(nNpcIndex > 0) then
				SetNpcTimer(nNpcIndex, TIME_LIFE_BOSS)
				local fkmgs = format("**** %s:%s,%d,%d,%d ****", "BOSS Tieu Hoang Kim", szname, nmap, tb_coords[nIdx][1], tb_coords[nIdx][2])
				logHoatDong(fkmgs)
				print(fkmgs)
			end
		end
	end
	Msg2SubWorld("Boss Ti”u Hoµng Kim Æ∑ xu t hi÷n tπi c∏c vﬁ tr› Î m´n ph∏i.h∑y mau mau t◊m di÷t chÛng !!!")
	--SubWorld = nOldSubWorld
end

function addnpcbosstieuhk()
	local nNpcIndex = 0
	local nPos = 1
	for i=1,getn(BOSSTIEU_ARRAY) do
		nPos = random(1,2)
		nPos = BOSSTIEU_ARRAY[i][4][nPos]
		nNpcIndex = AddNpcNew(BOSSTIEU_ARRAY[i][1],100,POSITION_ARRAY[nPos][1],
		POSITION_ARRAY[nPos][2],POSITION_ARRAY[nPos][3],DEATHFILEHKTIEU,
	    5,BOSSTIEU_ARRAY[i][2],0,BOSSTIEU_ARRAY[i][3],BOSSTIEU_EXP,LIFE_BOSS_TIEU,PHSLBOSS,8000,NETRANHBOSS,nil,nil,nil,nil,80,2,DROPRATEHKTIEU)
		if (nNpcIndex > 0) then
			SetNpcTimer(nNpcIndex, TIME_LIFE_BOSS)
			nMsg = format(COMMON_INFO_MIN,BOSSTIEU_ARRAY[i][2],GetMapName(POSITION_ARRAY[nPos][1]))
			Msg2SubWorld(nMsg)
			logHoatDong(date("%H:%M:%S")..nMsg.."\t\n")
		end
	end
end



-- function addnpcbossdaihk()
	-- local nNpcIndex = 0;
	-- local nPos = 1;
	-- for i=1,getn(BOSSDAI_ARRAY) do
		-- nPos = RANDOM(1,2);
		-- nPos = BOSSDAI_ARRAY[i][4][nPos];
		-- nNpcIndex = AddNpcNew(BOSSDAI_ARRAY[i][2],100,TAB_POS_BOSSDAI[nPos][1],
		-- TAB_POS_BOSSDAI[nPos][2],TAB_POS_BOSSDAI[nPos][3],DEATHFILEHKDAI,
	    -- 5,BOSSDAI_ARRAY[i][1],1,BOSSDAI_ARRAY[i][3],BOSSDAI_EXP,LIFE_BOSS_DAIH,PHSLBOSS*2,10000,NETRANHBOSS,nil,nil,nil,nil,80,2,DROPRATEHKDAI);
		-- if(nNpcIndex > 0) then
			-- SetNpcTimer(nNpcIndex, TIME_LIFE_BOSS)
			-- nMsg = format(COMMON_INFO_MAX,BOSSDAI_ARRAY[i][1],GetMapName(TAB_POS_BOSSDAI[nPos][1]),floor(TAB_POS_BOSSDAI[nPos][2]/8/32),floor(TAB_POS_BOSSDAI[nPos][3]/16/32))
			-- Msg2SubWorld(nMsg)
			-- logHoatDong(date("%H:%M:%S")..nMsg.."\t\n")
		-- end
	-- end
-- end;


function addnpcbossserver()
	local nNpcIndex = 0
	local nPos = 1
	local nMsg = " "
	for i=1,getn(BOSSSV_ARRAY) do
		nPos = random(1,getn(TAB_POS_BOSSSV))
		nNpcIndex = AddNpcNew(BOSSSV_ARRAY[i][2],100,TAB_POS_BOSSSV[nPos][1],
		TAB_POS_BOSSSV[nPos][2],TAB_POS_BOSSSV[nPos][3],DEATHFILEHKSV,
	    5,BOSSSV_ARRAY[i][1],1,random(0,4),BOSSDAI_EXP,LIFE_BOSS_DAIH,PHSLBOSS*3,10000,NETRANHBOSS*2,nil,nil,nil,nil,80,2,DROPRATEHKSV);
		if(nNpcIndex > 0) then
			SetNpcTimer(nNpcIndex, TIME_LIFE_BOSS)
			SetNpcDamageReduction(nNpcIndex,70)
			-- SetNpcAIType(nNpcIndex,3)
			nMsg = format(COMMON_INFO_MAX,BOSSSV_ARRAY[i][1],GetMapName(TAB_POS_BOSSSV[nPos][1]),floor(TAB_POS_BOSSSV[nPos][2]/8/32),floor(TAB_POS_BOSSSV[nPos][3]/16/32))
			Msg2SubWorld(nMsg)
			logHoatDong(date("%H:%M:%S")..nMsg.."\t\n")
		end
	end
end
-- Script Boss Hoµng Kim by Jx Ti™u Phong


QY_GOLDBOSS_APOS_INFO = {
{"CÊ B∏ch", 566, 95, 0, {"city","city","city"}}, -- √˚◊÷,–ŒœÛ,µ»º∂,ŒÂ––,{ø…ƒ‹≥ˆœ÷µƒµÿÕº}
-- {"Huy“n Gi∏c ßπi S≠", 740, 95, 0, {"Nhπn Thπch ÆÈng","Thanh kh™ ÆÈng"}},
-- {"ß≠Íng Phi Y’n", 1366, 95, 1, {"Phong L®ng ÆÈ","Kho∂ Lang ÆÈng",}},
{"Lam Y Y", 582, 95, 1, {"city","city","city"}},
-- {"Hµ Linh Phi™u", 568, 95, 2, {"city","city","city"}},
{"Y™n Hi”u Tr∏i", 744, 95, 2, {"Sa Mπc s¨n ÆÈng 1","Sa Mπc s¨n ÆÈng 3"}},
	{"Mπnh Th≠¨ng L≠¨ng", 583, 95, 3, {"Sa Mπc s¨n ÆÈng 1","Sa Mπc s¨n ÆÈng 2"}},
--	{"Gia LuÀt Tﬁ Ly", 563, 95, 3, {"L≠Ïng ThÒy ÆÈng","D≠¨ng Trung ÆÈng","city"}},
{"ßπo Thanh Ch©n Nh©n", 562, 95, 4, {"Tr≠Íng Bπch s¨n Nam","Mπc Cao QuÀt"}},
{"Tuy“n C¨ Tˆ", 747, 95, 4, {"Tr≠Íng Bπch s¨n Bæc","city"}},
-- {"V≠¨ng T∏", 739, 95, 0, {"VÚ L®ng ÆÈng","PhÔ Dung ÆÈng"}},
{"Huy“n Nan ßπi S≠", 1365, 95, 0, {"Phong L®ng ÆÈ","Kho∂ Lang ÆÈng","city"}},
{"ß≠Íng B t Nhi‘m", 741, 95, 1, {"Phong L®ng ÆÈ","Kho∂ Lang ÆÈng"}},
--{"Bπch Doanh Doanh", 742, 95, 1, {"Thanh kh™ ÆÈng","Sa mπc Æﬁa bi”u","city"}},
{"Thanh Tuy÷t S≠ Th∏i", 743, 95, 2, {"Tr≠Íng Bπch s¨n Bæc","D≠¨ng Trung ÆÈng"}},
--{"Chung Linh TÛ", 567, 95, 2, {"Phi Thi™n ÆÈng","V´ Danh ÆÈng","city"}},
	-- {"Hµ Nh©n Ng∑", 745, 95, 3, {"Nhπn Thπch ÆÈng","L≠Ïng ThÒy ÆÈng","city"}},
	{"ßoan MÈc Du÷", 565, 95, 3, {"Phong L®ng ÆÈ","city","city"}},
--{"Tı ßπi Nhπc", 1367, 95, 4, {"Mπc Bæc Th∂o Nguy™n","VÚ L®ng ÆÈng","city"}},
--{"Thanh Li™n Tˆ", 1368, 95, 4, {"Tr≠Íng Bπch s¨n Bæc","Sa Mπc s¨n  ÆÈng 3","city"}},
-- {"Thanh Tuy÷t S≠ Th∏i", 743, 95, 2, {"Sa Mπc s¨n  ÆÈng 1","Sa Mπc s¨n  ÆÈng 3","city"}},
--{"Hµn Ngu DËt", 748, 95, 1, {"Phong L®ng ÆÈ","Kho∂ Lang ÆÈng","city"}},					
}
QY_GOLDBOSS_POS = 
{
["city"] =        {
{78,  {{1753,3121},{1670,3462},{1369,3411},{1379,3141}}, "T≠¨ng D≠¨ng"     },	
{1,   {{1799,3265},{1835,3024},{1436,2953},{1404,3338}}, "Ph≠Óng T≠Íng"     },	
{11,  {{2966,4807},{2999,5266},{3252,5349},{3346,4797}}, "Thµnh ß´"     },	
{80,  {{1934,2861},{1723,3339},{1598,3313},{1538,3041}}, "D≠¨ng Ch©u"     },	
{37,  {{1770,3403},{1520,3281},{1535,2902},{2017,2679}}, "Bi÷n Kinh"     },	
{162, {{1404,3417},{1388,3149},{1723,3501},{1785,2986}}, "ßπi L˝"    },	
{176, {{1162,2781},{1832,3342},{1247,3354},{1754,2805}}, "L©m An"     },},
["PhÔ Dung ÆÈng"] =      {{202, {{1647,3100},{1535,3032},{1618,2950},{1528,2768}}, "PhÔ Dung ÆÈng"     },},
["S¨n B∂o ÆÈng"] =      {{76,  {{1583,2989},{1767,3086},{1671,3220},{1551,3144}}, "S¨n B∂o ÆÈng"     },},
["Nhπn Thπch ÆÈng"] =      {{10,  {{1667,3055},{1773,2935},{1836,3082},{1750,3193}}, "Nhπn Thπch ÆÈng"     },},
["Thanh kh™ ÆÈng"] =      {{198, {{1547,2814},{1640,2918},{1687,3025},{1624,3072}}, "Thanh Kh™ ßÈng"     },},
["Phong L®ng ÆÈ"] =  {{336, {{1113,3134},{1241,3038},{1422,3113},{1475,2929}}, "Phong L®ng ÆÈ" },},
["Kho∂ Lang ÆÈng"] =      {{75,  {{1717,3094},{1614,3191},{1843,3169},{1793,3030}}, "Kho∂ Lang ÆÈng"     },},
["VÚ L®ng ÆÈng"] =      {{199, {{1445,2959},{1602,3295},{1696,3034},{1852,3174}}, "VÚ L®ng ÆÈng"     },},
["Phi Thi™n ÆÈng"] =      {{204, {{1624,3379},{1818,3460},{1527,3529},{1403,3407}}, "Phi Thi™n ÆÈng"     },},
["Tr≠Íng Bπch s¨n Bæc"] =  {{322, {{1938,3957},{1697,3511},{1595,3976},{2046,3351}}, "Tr≠Íng Bπch s¨n Bæc" },},
["V´ Danh ÆÈng"] =      {{203, {{1626,3049},{1471,2992},{1590,3282},{1792,3234}}, "V´ Danh ÆÈng"     },},
["Sa Mπc s¨n ÆÈng 1"] =   {{225, {{1289,3261},{1486,3180},{1401,3284},{1603,3221}}, "Sa Mπc s¨n  ÆÈng 1"  },},
["Sa Mπc s¨n ÆÈng 2"] =   {{226, {{1723,3210},{1723,3210},{1723,3210},{1723,3210}}, "Sa Mπc s¨n  ÆÈng 2"  },},
["Sa Mπc s¨n ÆÈng 3"] =   {{227, {{1628,3277},{1658,3050},{1365,3160},{1498,3398}}, "Sa Mπc s¨n  ÆÈng 3"  },},
["L≠Ïng ThÒy ÆÈng"] =      {{181, {{1635,3079},{1415,3041},{1504,3099},{1616,2951}}, "L≠Ïng ThÒy ÆÈng"     },},
["D≠¨ng Trung ÆÈng"] =      {{205, {{1612,3364},{1585,3226},{1495,3110},{1275,3059}}, "D≠¨ng Trung ÆÈng"     },},
["Tr≠Íng Bπch s¨n Nam"] =  {{321, {{1474,3008},{1065,3059},{1007,2626},{1413,2440}}, "Tr≠Íng Bπch s¨n Nam" },},
["Mπc Cao QuÀt"] =      {{340, {{1893,3275},{1762,2967},{1288,3304},{1447,2798}}, "Mπc Cao QuÀt"     },},
["T©y S¨n Æ∂o"] =      {{342, {{1212,2419},{1253,2924},{1516,2802},{1529,2477}}, "T©y S¨n Æ∂o"     },},
["Phong L®ng ÆÈ"] =  {{336, {{1531,2535},{1140,2639},{1166,2824},{1425,2793}}, "Phong L®ng ÆÈ" },},
["Sa mπc Æﬁa bi”u"] =    {{224, {{1739,2857},{1622,2974},{1784,3096},{1447,3040}}, "Sa mπc Æﬁa bi”u"   },},
["Mπc Bæc Th∂o Nguy™n"] =    {{341, {{1538,2429},{1258,2648},{1331,3182},{1484,3040}}, "Mπc Bæc Th∂o Nguy™n"   },},
["Ki’m C∏c T©y Nam"] =    {{19, {{3233,4272},{3241,4279},{3233,4272},{3241,4279}}, "Ki’m C∏c T©y Nam"   },},
}

function addnpcbossdaihk()
	local tbUsedMapId = {}

	for i = 1, getn(QY_GOLDBOSS_APOS_INFO) do
		local tbBoss = QY_GOLDBOSS_APOS_INFO[i]
		local szName = tbBoss[1]
		local nTemplateId = tbBoss[2]
		local nLevel = tbBoss[3]
		local nSeries = tbBoss[4]

		if nTemplateId == 748 then
			nSeries = random(0, 4)
		end

		local tbMapList = tbBoss[5]
		if tbMapList and getn(tbMapList) > 0 then
			local tryCount = 10
			local found = 0

			for _ = 1, tryCount do
				local szMap = tbMapList[random(1, getn(tbMapList))]
				local tbMapInfo = QY_GOLDBOSS_POS[szMap]

				if tbMapInfo and getn(tbMapInfo) > 0 then
					local mapId = tbMapInfo[1][1]

					if not tbUsedMapId[mapId] then
						local tbPosList = tbMapInfo[1][2]
						local szMapName = tbMapInfo[1][3]
						local tbRandPos = tbPosList[random(1, getn(tbPosList))]
						local x = tbRandPos[1]
						local y = tbRandPos[2]

						local nNpcId = AddNpcNew(
							nTemplateId, nLevel, mapId, x * 32, y * 32, DEATHFILEHKDAI,
							5, szName, 1, nSeries, BOSSDAI_EXP, LIFE_BOSS_DAIH, PHSLBOSS * 2, 10000, NETRANHBOSS,
							nil, nil, nil, nil, 80, 2, DROPRATEHKDAI
						)

						if nNpcId > 0 then
							SetNpcTimer(nNpcId, TIME_LIFE_BOSS)
							local nMsg = format(COMMON_INFO_MAX, szName, szMapName)
							Msg2SubWorld(nMsg)
							logHoatDong(date("%H:%M:%S") .. nMsg .. "\t\n")
							tbUsedMapId[mapId] = 1
							found = 1
							break
						end
					end
				end
			end

			-- if found == 0 then
				-- logHoatDong("[WARN] KhÙng tÏm du?c map chua cÛ boss cho: " .. szName .. "\n")
			-- end
		end
	end
end

