tbLianSai_config = {}

tbLianSai_config.szRealyScript = "\\script\\event\\liansai\\liansai_manager.lua"
tbLianSai_config.tbBaseDate = {2018,4,2}

-- ±ÈÈüÃ»¿ªÊ¼¡¢¿ªÊ¼¡¢×¼±¸ÆÚ¡¢Õ½¶·ÆÚ
tbLianSai_config.STATE_NONE = 0
tbLianSai_config.STATE_START = 1
tbLianSai_config.STATE_READY = 2
tbLianSai_config.STATE_FIGHT = 3

-- ±ÈÈüÃ»¿ªÊ¼¡¢³õÈü¡¢¾öÈü
tbLianSai_config.PHASE_NONE = 0
tbLianSai_config.PHASE_NORMAL = 1
tbLianSai_config.PHASE_FINAL = 2

TSK_LIANSAI_AWARDFLAG = 4391

FIGHTTYPE_NONE = 0
FIGHTTYPE_SINGLE = 1
FIGHTTYPE_FACTIONSIGNLE = 2
FIGHTTYPE_TRIPLE = 3
FIGHTTYPE_DOUBLE = 4
FIGHTTYPE_OFFLINE = 5
FIGHTTYPE_GUARD = 6 

tbLianSai_config.tbBattleType = {
    [1] = FIGHTTYPE_DOUBLE,
    [2] = FIGHTTYPE_GUARD,
    [3] = FIGHTTYPE_SINGLE,
    [4] = FIGHTTYPE_TRIPLE,
}

tbLianSai_config.nGuardBuffId = 2151
tbLianSai_config.LS_LGTASK_OBJ = 600
tbLianSai_config.LS_MEMBERTASK_JOB = 1
tbLianSai_config.LS_MEMBERTASK_GUARD = 2
tbLianSai_config.LS_LGTASK_WINCOUNT = 1
tbLianSai_config.LS_LGTASK_LOSECOUNT = 2
tbLianSai_config.LS_LGTASK_SCORE = 3
tbLianSai_config.LS_LGTASK_FINALROUND =4

-- ¾öÈüÆ¥Åä¹æÔò
MATCHRULE_RANDOME = 0   -- Ëæ»úÆ¥Åä
MATCHRULE_FIRST_TO_LAST = 1   -- Ê×Î²Æ¥Åä
tbLianSai_config.FinalMathchRule = MATCHRULE_RANDOME

-- ¸÷ÀàÊ±¼ä
tbLianSai_config.nSendMailDay = 5  -- ÖÜÎå·¢ÓÊ¼þÍ¨Öª¾öÈüÃûµ¥
tbLianSai_config.nCreateTrans = 6
tbLianSai_config.nCreateLevel = 150
tbLianSai_config.tbStartSignTime = {1, 1000} --ÖÜ1 10µã 0·Ö
tbLianSai_config.tbEndSignTime = {5, 1920} --ÖÜÎå 18µã 51·Ö
tbLianSai_config.nFirstFightTime = 1900 -- ³£¹æÈüµÚÒ»´ÎÕ½¶·¿ªÊ¼Ê±¼ä
tbLianSai_config.nLadderId = 10197

tbLianSai_config.tbNormalMatchDay = {1,3,5} -- ³£¹æÈüÊ±¼äÖÜ1,3,5
tbLianSai_config.tbNormalMatchTime = {1850,1926} -- ³£¹æÈü´Ó18µã50·Öµ½19µã26·Ö
tbLianSai_config.nFinalMatchDay = 6 -- ¾öÈüÔÚÖÜÁù,¸Ä³ÉÖÜÈÕÇëÌî0
tbLianSai_config.tbFinalMatchTime = {1500,1555} -- ¾öÈü´Ó15µã0·Öµ½15µã55·Ö

tbLianSai_config.tbItemWhiteList = {
    ["1,8,0,5"] = "§¹i Håi Thiªn Hoµn",
    ["1,2,0,5"] = "Ngò Hoa Ngäc Lé Hoµn",
    ["1,8,0,4"] = "Håi Thiªn T¸i T¹o §¬n",
}

--Î¥½ûÒ©Æ·
tbLianSai_config.tbRemoveItemBuff = {
	{	"C«ng Tèc hoµn", {6, 1, 218, -1, 0, 0}, 511	},
	{	"Bµo Tèc hoµn", {6, 1, 219, -1, 0, 0}, 512	},
	{	"Phæ Phßng hoµn", {6, 1, 220, -1, 0, 0}, 513	},
	{	"§éc Phßng hoµn", {6, 1, 221, -1, 0, 0}, 514	},
	{	"B¨ng Phßng hoµn", {6, 1, 222, -1, 0, 0}, 515	},
	{	"Háa Phßng hoµn", {6, 1, 223, -1, 0, 0}, 516	},
	{	"L«i Phßng hoµn", {6, 1, 224, -1, 0, 0}, 517	},
	{	"Gi¶m Th­¬ng hoµn", {6, 1, 225, -1, 0, 0}, 518	},
	{	"Gi¶m H«n hoµn", {6, 1, 226, -1, 0, 0}, 519	},
	{	"Gi¶m §éc hoµn", {6, 1, 227, -1, 0, 0}, 520	},
	{	"Gi¶m B¨ng hoµn", {6, 1, 228, -1, 0, 0}, 521	},
	{	"Phæ C«ng hoµn", {6, 1, 229, -1, 0, 0}, 522	},
	{	"§éc C«ng hoµn", {6, 1, 230, -1, 0, 0}, 523	},
	{	"B¨ng C«ng hoµn", {6, 1, 231, -1, 0, 0}, 524	},
	{	"Háa C«ng hoµn", {6, 1, 232, -1, 0, 0}, 525	},
	{	"L«i C«ng hoµn", {6, 1, 233, -1, 0, 0}, 526	},
	{	"Tr­êng MÖnh hoµn", {6, 1, 234, -1, 0, 0}, 527	},
	{	"Tr­êng Néi hoµn", {6, 1, 235, -1, 0, 0}, 528	},
	{	"Yªn Hång ®an", {6, 1, 115, -1, 0, 0}, 450	},
	{	"X¸ Lam ®an", {6, 1, 116, -1, 0, 0}, 451	},
	{	" Néi Phæ hoµn", {6, 1, 117, -1, 0, 0}, 453	},
	{	" Néi §éc hoµn", {6, 1, 118, -1, 0, 0}, 454	},
	{	" Néi B¨ng hoµn", {6, 1, 119, -1, 0, 0}, 455	},
	{	" Néi Háa hoµn", {6, 1, 120, -1, 0, 0}, 456	},
	{	" Néi §iÖn hoµn", {6, 1, 121, -1, 0, 0}, 457	},
	{	"Tr­êng MÖnh hoµn", {6, 0, 1, -1, 0, 0}, 256	},
	{	"Gia Bµo hoµn", {6, 0, 2, -1, 0, 0}, 257	},
	{	"§¹i Lùc hoµn", {6, 0, 3, -1, 0, 0}, 258	},
	{	"Cao ThiÓm hoµn", {6, 0, 4, -1, 0, 0}, 259	},
	{	"Cao Trung hoµn", {6, 0, 5, -1, 0, 0}, 260	},
	{	"Phi Tèc hoµn", {6, 0, 6, -1, 0, 0}, 261	},
	{	"B¨ng Phßng hoµn", {6, 0, 7, -1, 0, 0}, 262	},
	{	"L«i Phßng hoµn", {6, 0, 8, -1, 0, 0}, 263	},
	{	"Háa Phßng hoµn", {6, 0, 9, -1, 0, 0}, 264	},
	{	"§éc Phßng hoµn", {6, 0, 10, -1, 0, 0}, 265	},
	{	"B¸nh ch­ng H¹t dÎ", {6, 0, 60, -1, 0, 0}, 401	},
	{	"B¸nh ch­ng ThÞt heo", {6, 0, 61, -1, 0, 0}, 402	},
	{	"B¸nh ch­ng ThÞt bß", {6, 0, 62, -1, 0, 0}, 403	},
	{	"B¸ch Qu¶ Lé", {6, 1, 73, -1, 0, 0}, 442	},
	{	"C¸t t­êng hång bao", {6, 1, 19, -1, 0, 0}, 442	},
	{	"Hoµng Kim B¶o H¹p",	{6,	1,	69,	-1,	0,	0},	442},
	{	"TÝn Sø Méc yªu bµi",	{6,	1,	885,	-1,	0	,0},	542},
	{	"TÝn Sø §ång yªu bµi",	{6,	1,	886,	-1,	0	,0},	543},
	{	"TÝn Sø Ng©n yªu bµi",	{6,	1,	887,	-1,	0	,0},	544},
	{	"TÝn Sø Kim yªu bµi",	{6,	1,	888,	-1,	0	,0},	545},
	{	"Ngù Tø TÝn Sø yªu bµi",	{6,	1,	889,	-1,	0	,0},	546},
	{	"Bao D­îc hoµn ",	{6,	1,	910,	-1,	0	,0},	635},
	{	"Hép lÔ vËt Hång bao thÇn bÝ",	{6,	1,	1074,	-1,	0	,0},	635},
	{	"Hép lÖ vËt [qu¶ Huy Hoµng]",	{6,	1,	1075,	-1,	0	,0},	635},
	{	"H×nh ném",		{6,	1,	1389,	-1,	0,	0},	635	},
	--tinhpn 20100720: forbid item
	{	"CÈm nang thay ®æi trêi ®Êt", {6, 1, 1781, -1, 0, 0},635},	
	{	"Méc ChÕ B¶o H¹p", {6, 1, 2318, -1, 0, 0},635},	
	{	"§ång ChÕ B¶o H¹p", {6, 1, 2319, -1, 0, 0},635},	
	{	"Ng©n ChÕ B¶o H¹p", {6, 1, 2320, -1, 0, 0},635},	
	{	"Hoµng Kim B¶o H¹p", {6, 1, 2321, -1, 0, 0},635},	
	{	"B¶o r­¬ng B¹ch Kim", {6, 1, 2322, -1, 0, 0},635},	
	{	"S¸t Thñ BÝ B¶o", {6, 1, 2347, -1, 0, 0},635},	
	{	"B¶o r­¬ng thÇn bÝ cña D· TÈu", {6, 1, 2374, -1, 0, 0},635},	
	{	"ChÝ T«n BÝ B¶o", {6, 1, 2375, -1, 0, 0},635},	
	{	"Tµi B¶o Thñy TÆc", {6, 1, 2376, -1, 0, 0},635},	
	{	"Håi thiªn t¸i t¹o lÔ bao", {6, 1, 2527, -1, 0, 0},635},
	{	"Phi tèc hoµn LÔ bao", {6, 1, 2520, -1, 0, 0},635},
	{	"§¹i Lùc hoµn LÔ bao", {6, 1, 2517, -1, 0, 0},635},
	--Updated by DinhHQ - 20110425
	{	"C«ng Thµnh ChiÕn LÔ Bao", {6, 1, 2377, -1, 0, 0},635},
	{	"§éc phßng hoµn lÔ bao", {6, 1, 2524, -1, 0, 0},635},
	{	"Tr­êng mÖnh hoµn lÔ bao", {6, 1, 2515, -1, 0, 0},635},
	{	"Gia tèc hoµn lÔ bao", {6, 1, 2516, -1, 0, 0},635},
	{	"Cao thiÓm hoµn lÔ bao", {6, 1, 2518, -1, 0, 0},635},
	{	"Cao trung hoµn lÔ bao", {6, 1, 2519, -1, 0, 0},635},
	{	"Phi tèc hoµn LÔ bao", {6, 1, 2520, -1, 0, 0},635},
	{	"B¨ng phßng hoµn lÔ bao", {6, 1, 2521, -1, 0, 0},635},
	{	"L«i phßng hoµn lÔ bao", {6, 1, 2522, -1, 0, 0},635},
	{	"Háa phßng hoµn lÔ bao", {6, 1, 2523, -1, 0, 0},635},
	{	"§éc phßng hoµn lÔ bao", {6, 1, 2524, -1, 0, 0},635},
	{	"LÖnh bµi vi s¬n ®¶o lÔ bao", {6, 1, 2525, -1, 0, 0},635},
	{	"Hoµn Hån §¬n LÔ Bao", {6, 1, 2830, -1, 0, 0},635},
	{	"Tiªu Diªu T¸n", {6, 1, 2831, -1, 0, 0},635},
	{	"Hoµn Hån §¬n", {6, 1, 2837, -1, 0, 0},635},
	--Update by ThanhLD, 20140924
	{	"Tói D­îc PhÈm", {6, 1, 30557, -1, 0, 0},635},
	{	"Tói D­îc PhÈm §Æc BiÖt", {6, 1, 30243, -1, 0, 0},635},
	{	"LÔ Bao T«n Quý Ngµy", {6, 1, 4845, -1, 0, 0},635},
}

tbLianSai_config.tbRemoveSkillBuff = {
	{"BÊt ®éng Minh V­¬ng ",	15},
	{"Nh­ Lai Thiªn DiÖp ",	273},
	{"TÜnh T©m QuyÕt",	33},
	{"Kim Chung Tr¸o",	42},
	{"Cöu Thiªn Cuång L«i ",	67},
	{"XÝch DiÖm Thùc Thiªn",	70},
	{"B¨ng Lam HuyÒn Tinh",	64},
	{"Xuyªn Y Ph¸ Gi¸p ",	356},
	{"V¹n §éc Thùc T©m",	73},
	{"Xuyªn T©m §éc ThÝch",	72},
	{"§o¹n C©n Hñ Cèt ",	390},
	{"B¨ng T©m Tr¸i ¶nh",	269},
	{"Hé ThÓ Hµn B¨ng ",	100},
	{"TuyÕt ¶nh",	109},
	--´Ë¼¼ÄÜÒÑ¸ÄÎª±»¶¯¼¼ÄÜ£¬²»ÔÙÇå³ý	{"´ò¹·Õó",	124},
	{"Ho¹t BÊt L­u Thñ ",	277},
	{"Tóy §iÖp Cuång Vò ",	130},
	{"Háa Liªn PhÇn Hoa",	136},
	{"¶o ¶nh Phi Hå ",	137},
	{"Thiªn Ma Gi¶i ThÓ ",	150},
	{"Täa Väng V« Ng· ",	157},
	{"Thanh Phong phï ",	171},
	{"Ki B¸n phï ",	174},
	{"NhÊt KhÝ Tam Thanh",	178},
	{"B¾c Minh §¸o H¶i",	393},
	{"Thiªn Thanh §Þa Träc",	173},
	{"Khi Hµn Ng¹o TuyÕt",	175},
	{"Tóy Tiªn T¸ Cèt",	394},
	{"lÖnh bµi kü n¨ng 1",	631},
	{"lÖnh bµi kü n¨ng 2",	632},
	{"lÖnh bµi kü n¨ng 3",	633},
	{"lÖnh bµi kü n¨ng 4",	634},
	{"lÖnh bµi kü n¨ng 5",	635},
	-- ¹úÕ½ËÎ½ð¼¼ÄÜ²»ÄÜÔÚÁªÈüÖÐÊ¹ÓÃ=====>
	{"Kü n¨ng Tiªn Phong",	958},
	{"Kü n¨ng Nguyªn So¸i",959},		
	{"Kü n¨ng Thõa T­íng",	960},
	{"Kü n¨ng Hoµng §Õ",	961},
	{"Kü n¨ng Thiªn Tö",	962},
	{"Hoµn Hån §¬n kü n¨ng",	462},
	{"Tiªu Diªu T¸n kü n¨ng",	1052},
	-- ¹úÕ½ËÎ½ð¼¼ÄÜ²»ÄÜÔÚÁªÈüÖÐÊ¹ÓÃ<=====
	--CËp nhËt thªm tr¹ng th¸i ©m d?¬ng ho¹t huyÕt ?¬n v? kÝch c«ng tr? lùc hoµn - Modified By DinhHQ - 20110810
	{"KÝch C«ng Trî Lùc Hoµn",	1120},
	{"KÝch C«ng Trî Lùc Hoµn",261},		
	{"KÝch C«ng Trî Lùc Hoµn",	258},
	{"KÝch C«ng Trî Lùc Hoµn",	260},
	{"¢m D­¬ng Ho¹t HuyÕt §¬n",	1121},
	{"¢m D­¬ng Ho¹t HuyÕt §¬n",	256},
	{"¢m D­¬ng Ho¹t HuyÕt §¬n",	259},
	{"¢m D­¬ng Ho¹t HuyÕt §¬n",	257},
}

-- ³£¹æÈü±ÈÈüÊ±¼äÌáÊ¾
tbLianSai_config.tbNormalMatchTimeTips = {
    "18h50 - 19h00",    -- ×¼±¸Ê±¼ä
    "19h00 - 19h05",    -- µÚÒ»³¡
    "19h07 - 19h12",    -- µÚ¶þ³¡
    "19h14 - 19h19",    -- µÚÈý³¡
    "19h21 - 19h26",    -- µÚËÄ³¡
}

-- ¾öÈü±ÈÈüÊ±¼äÌáÊ¾
tbLianSai_config.tbFinalMatchTimeTips = {
    "15h00 - 15h10",    -- ×¼±¸Ê±¼ä
    "15h10 - 15h15",    -- µÚÒ»³¡
    "15h20 - 15h25",    -- µÚ¶þ³¡
    "15h30 - 15h35",    -- µÚÈý³¡
    "15h40 - 15h45",    -- µÚËÄ³¡
    "15h50 - 15h55",    -- µÚÎå³¡
}

-- tbLianSai_config.nLianSaiMapId = 1052
tbLianSai_config.nReadyMapId = 1047
tbLianSai_config.tbReadyPos = {1523, 3024}
tbLianSai_config.tbLianSaiPos = {{1517, 3033}, {1484,3033}}
tbLianSai_config.tbBattleMapId = {
    [FIGHTTYPE_SINGLE] = 1048,
    [FIGHTTYPE_FACTIONSIGNLE] = 1048,
    [FIGHTTYPE_TRIPLE] = 1053,
    [FIGHTTYPE_DOUBLE] = 1053,
    [FIGHTTYPE_OFFLINE] = 1053,
    [FIGHTTYPE_GUARD] = 1053,
}

function tbLianSai_config:CheckInLianSaiMap()
    local _, _, nMapIndex = GetPos()
    local nMapId = SubWorldIdx2MapCopy(nMapIndex)
    if nMapId == 1047 or nMapId == 1048 or nMapId == 1053 then
        return 1
    end
end

SZ_ENTER_POS = "LIANSAI_ENTERPOS"
SZ_ENTER_POS_3V3 = "LIANSAI_ENTERPOS3V3"
tbLianSai_config.tbBattleMapEnterPos = {
    [FIGHTTYPE_SINGLE] = SZ_ENTER_POS,
    [FIGHTTYPE_FACTIONSIGNLE] = SZ_ENTER_POS,
    [FIGHTTYPE_TRIPLE] = SZ_ENTER_POS_3V3,
    [FIGHTTYPE_DOUBLE] = SZ_ENTER_POS_3V3,
    [FIGHTTYPE_OFFLINE] = SZ_ENTER_POS_3V3,
    [FIGHTTYPE_GUARD] = SZ_ENTER_POS_3V3,
}

tbLianSai_config.nMiddleLevel = 3  --¾»Ê¤³¡´Î
tbLianSai_config.nSeniorLevel = 6

tbLianSai_config.tbPerMapBattle = {-- Ã¿¸öÕ½¶·µØÍ¼×î¶àÈÝÄÉµÄÕ½¶·³¡
    [FIGHTTYPE_SINGLE] = 100,
    [FIGHTTYPE_FACTIONSIGNLE] = 100,
    [FIGHTTYPE_TRIPLE] = 36,
    [FIGHTTYPE_DOUBLE] = 36,
    [FIGHTTYPE_OFFLINE] = 36,
    [FIGHTTYPE_GUARD] = 36,
}

tbLianSai_config.nFinalTeamCount = 16

tbLianSai_config.szDeathScript = "\\script\\event\\liansai\\player_script.lua"

tbLianSai_config.tbReadyMapNPC = {
    {236, 0, 1455, 3017, "\\script\\event\\liansai\\npc\\chefu.lua", "Xa phu"},
    --{625, 0, 1461, 3022, "\\script\\event\\liansai\\npc\\chu_wu_xiang.lua", "ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½"},
    --{55, 0, 1466, 3028, "\\script\\event\\liansai\\npc\\medicine.lua", "Ò©ï¿½ï¿½"},
    {1783,  0, 1472, 3033, "\\script\\event\\liansai\\npc\\gamblenpc.lua", "Quan §Æt C­îc"},

    {236, 0, 1519, 3018, "\\script\\event\\liansai\\npc\\chefu.lua", "Xa phu"},
    --{625, 0, 1519, 3031, "\\script\\event\\liansai\\npc\\chu_wu_xiang.lua", "ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½"},
    --{55, 0, 1529, 3018, "\\script\\event\\liansai\\npc\\medicine.lua", "Ò©ï¿½ï¿½"},
    {1783,  0, 1529, 3031, "\\script\\event\\liansai\\npc\\gamblenpc.lua", "Quan §Æt C­îc"},
}

tbLianSai_config.tbSignPos = {78, 1566, 3239}

BATTLE_NONE = 0
BATTLE_WIN = 1
BATTLE_LOST = 2

TEAM_A = 1
TEAM_B = 2

CYCLE_START = 1
CYCLE_PREREPORT = 2
CYCLE_FIGHTREADY = 3
CYCLE_FIGHT = 4
CYCLE_REST = 5
CYCLE_END = 6
CYCLE_CLEAN = 7

TIMESPAN = 1
PERUNIT = 60 / TIMESPAN
ENDCYCLEUNIT = 50 * PERUNIT -- Ô¤±¨Àë±ÈÈü¿ªÊ¼»¹ÓÐ¶à¾Ã£¬Ð´¸ö»î¶¯½áÊøÊ±¼ä±£ÏÕÒ»µã£¬±ÜÃâ¼ÆËã¿ªÊ¼Ê±¼äÎÞÏÞÑ­»·£¬ºÍCYCLE_CLEANÊ±¼äÒ»ÖÂ
FIGHTSPAN = 4.5 * PERUNIT -- Õ½¶·Ê±¼ä£¬Ëã»ý·ÖÓÃ£¬ÏÂÃæ¸ÄÁËÊ±¼äÒª×¢ÒâÕâÀïÒ²ÐèÒª¸Ä

tbLianSai_config.tbBattleAward = {
    [1] = { --Õ½¶·Ê¤ÀûÍ¨ÓÃ½±Àø
        {nExp = 2000000},
    },
    [2] = { --Õ½¶·Ê§°ÜÍ¨ÓÃ½±Àø
        {nExp = 1000000},
    },
}

-- Ä¿Ç°Ö»¸øÖØ·ê»ý·Ö£¬Ã»ÓÐÅÐ¶Ï±³°ü£¬ºóÃæÀ©Õ¹½±ÀøÐèÒª×¢ÒâÅÐ¶Ï
tbLianSai_config.tbGambleAward = {
    {szName = "Th­ëng dù ®o¸n", tbProp = {6,1,5112,1,0,0}, nBindState = -2, nExpiredTime = 7*24*60, nCount = 1},
}

tbLianSai_config.tbNormalAward = {
    [1] = { -- ÌÔÌ­Èü³õ¼¶±¦Ïä
        {szName = "LÔ Bao Liªn ChiÕn.", tbProp = {6,1,4417,1,0,0}, nBindState = -2, nExpiredTime = 7*24*60, nCount = 2},
        {nZhenYuanPoint = 500},
        {nExp = 100000000},
    },
    [2] = { -- ÌÔÌ­ÈüÖÐ¼¶±¦Ïä
        {szName = "LÔ Bao Liªn ChiÕn.", tbProp = {6,1,4417,1,0,0}, nBindState = -2, nExpiredTime = 7*24*60, nCount = 5},
        {nZhenYuanPoint = 1000},
        {nExp = 200000000},
    },
}

tbLianSai_config.tbFinalAward32 = {
    [1] = { -- ÁªÈü¾öÈüµÚ1ÂÖ
        tbItem = {
            {szName = "LÔ Bao Liªn ChiÕn.", tbProp = {6,1,4417,1,0,0}, nBindState = -2, nCount = 2, nExpiredTime = 7*24*60},
            {nZhenYuanPoint = 500},
            {nExp = 200000000},
        },
    },
    [2] = { -- ÁªÈü¾öÈüµÚ2ÂÖ
        tbItem = {
            {szName = "LÔ Bao Liªn ChiÕn.", tbProp = {6,1,4417,1,0,0}, nBindState = -2, nCount = 4, nExpiredTime = 7*24*60},
            {nZhenYuanPoint = 1000},
            {nExp = 300000000},
        },
        nTitleID = 469
    },
    [3] = {
        tbItem = {
            {szName = "LÔ Bao Liªn ChiÕn.", tbProp = {6,1,4417,1,0,0}, nBindState = -2, nCount = 6, nExpiredTime = 7*24*60},
            {nZhenYuanPoint = 1500},
            {nExp = 400000000},
            {szName = "R­¬ng trang søc Tinh Linh (Tïy chän)", tbProp = {6,1,30653,1,0,0}, nCount = 1, nExpiredTime = 7*24*60},
        },
        nTitleID = 468
    },
    [4] = {
        tbItem = {
            {szName = "LÔ Bao Liªn ChiÕn.", tbProp = {6,1,4417,1,0,0}, nBindState = -2, nCount = 8, nExpiredTime = 7*24*60},
            {nZhenYuanPoint = 2000},
            {nExp = 500000000},
            {szName = "R­¬ng trang søc NhËm Tiªu Dao (NgÉu nhiªn)", tbProp = {6,1,30655,1,0,0}, nCount = 1, nExpiredTime = 7*24*60},
        },
        nTitleID = 467
    },
    [5] = {
        tbItem = {
            {szName = "LÔ Bao Liªn ChiÕn.", tbProp = {6,1,4417,1,0,0}, nBindState = -2, nCount = 10, nExpiredTime = 7*24*60},
            {nZhenYuanPoint = 3000},
            {nExp = 1000000000},
            {szName = "R­¬ng trang søc NhËm Tiªu Dao (Tïy chän)", tbProp = {6,1,30671,1,0,0}, nCount = 1, nExpiredTime = 7*24*60},

        },
        nTitleID = 466
    },
    [6] = {-- ÁªÈü¾öÈüµÚ×îºóÒ»ÂÖ
        tbItem = {
            {szName = "LÔ Bao Liªn ChiÕn.", tbProp = {6,1,4417,1,0,0}, nBindState = -2, nCount = 15, nExpiredTime = 7*24*60},
            {nZhenYuanPoint = 4000},
            {nExp = 2000000000},
            {szName = "R­¬ng trang søc NhËm Tiªu Dao (Tïy chän)", tbProp = {6,1,30671,1,0,0}, nCount = 1, nExpiredTime = 7*24*60},
        },
        nTitleID = 465
    },
}

tbLianSai_config.tbFinalAward16 = {
    [1] = { -- ÁªÈü¾öÈüµÚ1ÂÖ
    tbItem = {
        {szName = "LÔ Bao Liªn ChiÕn.", tbProp = {6,1,4417,1,0,0}, nBindState = -2, nCount = 4, nExpiredTime = 7*24*60},
        {nZhenYuanPoint = 1000},
        {nExp = 300000000},
    },
    nTitleID = 469
    },
    [2] = { -- ÁªÈü¾öÈüµÚ2ÂÖ
    tbItem = {
        {szName = "LÔ Bao Liªn ChiÕn.", tbProp = {6,1,4417,1,0,0}, nBindState = -2, nCount = 6, nExpiredTime = 7*24*60},
        {nZhenYuanPoint = 1500},
        {nExp = 400000000},
        {szName = "R­¬ng trang søc Tinh Linh (Tïy chän)", tbProp = {6,1,30563,1,0,0}, nCount = 1, nExpiredTime = 7*24*60},
    },
    nTitleID = 468
    },
    [3] = { -- ÁªÈü¾öÈüµÚ3ÂÖ
    tbItem = {
        {szName = "LÔ Bao Liªn ChiÕn.", tbProp = {6,1,4417,1,0,0}, nBindState = -2, nCount = 8, nExpiredTime = 7*24*60},
        {nZhenYuanPoint = 2000},
        {nExp = 500000000},
        {szName = "R­¬ng trang søc NhËm Tiªu Dao (NgÉu nhiªn)", tbProp = {6,1,30658,1,0,0}, nCount = 1, nExpiredTime = 7*24*60},
    },
    nTitleID = 467
    },
    [4] = { -- ÁªÈü¾öÈüµÚ4ÂÖ
    tbItem = {
        {szName = "LÔ Bao Liªn ChiÕn.", tbProp = {6,1,4417,1,0,0}, nBindState = -2, nCount = 10, nExpiredTime = 7*24*60},
        {nZhenYuanPoint = 3000},
        {nExp = 1000000000},
        {szName = "R­¬ng trang søc NhËm Tiªu Dao (Tïy chän)", tbProp = {6,1,30671,1,0,0}, nCount = 1, nExpiredTime = 7*24*60},

    },
    nTitleID = 466
    },
    [5] = { -- ÁªÈü¾öÈüµÚ5ÂÖ ¹Ú¾ü
      tbItem = {
        {szName = "LÔ Bao Liªn ChiÕn.", tbProp = {6,1,4417,1,0,0}, nBindState = -2, nCount = 15, nExpiredTime = 7*24*60},
        {nZhenYuanPoint = 4000},
        {nExp = 2000000000},
        {szName = "R­¬ng trang søc NhËm Tiªu Dao (Tïy chän)", tbProp = {6,1,30671,1,0,0}, nCount = 1, nExpiredTime = 7*24*60},
    },
    nTitleID = 465
    },
}

tbLianSai_config.tbFinalAward = tbLianSai_config.nFinalTeamCount == 32 and tbLianSai_config.tbFinalAward32 or tbLianSai_config.tbFinalAward16

tbLianSai_config.tbBattleCycle_real = {  -- ÕýÊ½ÈüÓÃÊ±¼ä
    [0] = CYCLE_START,
    [5 *    PERUNIT] = CYCLE_PREREPORT,
    [9 *    PERUNIT] = CYCLE_PREREPORT,
    [9.5 *  PERUNIT] = CYCLE_PREREPORT,
    [10 *   PERUNIT] = CYCLE_FIGHTREADY,
    [10.5 * PERUNIT] = CYCLE_FIGHT,
    [15 *   PERUNIT] = CYCLE_REST,
    ---------------------------------------
    [16 *   PERUNIT] = CYCLE_PREREPORT,
    [16.5 * PERUNIT] = CYCLE_PREREPORT,
    [17 *   PERUNIT] = CYCLE_FIGHTREADY,
    [17.5 * PERUNIT] = CYCLE_FIGHT,
    [22 *   PERUNIT] = CYCLE_REST,
    ---------------------------------------
    [23 *   PERUNIT] = CYCLE_PREREPORT,
    [23.5 * PERUNIT] = CYCLE_PREREPORT,
    [24 *   PERUNIT] = CYCLE_FIGHTREADY,
    [24.5 * PERUNIT] = CYCLE_FIGHT,
    [29 *   PERUNIT] = CYCLE_REST,
    ---------------------------------------
    [30 *   PERUNIT] = CYCLE_PREREPORT,
    [30.5 * PERUNIT] = CYCLE_PREREPORT,
    [31 *   PERUNIT] = CYCLE_FIGHTREADY,
    [31.5 * PERUNIT] = CYCLE_FIGHT,
    [36 *   PERUNIT] = CYCLE_END,
    [40 *   PERUNIT] = CYCLE_CLEAN,
}

tbLianSai_config.tbBattleCycle_test = {   -- ÕýÊ½ÈüÓÃ²âÊÔÊ±¼ä
    [0] = CYCLE_START,
    [0.5 *    PERUNIT] = CYCLE_PREREPORT,
    [1 *   PERUNIT] = CYCLE_FIGHTREADY,
    [1.5 * PERUNIT] = CYCLE_FIGHT,
    [2 *   PERUNIT] = CYCLE_REST,
    [2.5 *   PERUNIT] = CYCLE_PREREPORT,
    [3 * PERUNIT] = CYCLE_PREREPORT,
    [3.5 *   PERUNIT] = CYCLE_FIGHTREADY,
    [4 * PERUNIT] = CYCLE_FIGHT,
    [4.5 *   PERUNIT] = CYCLE_REST,
    [5 *   PERUNIT] = CYCLE_PREREPORT,
    [5.5 * PERUNIT] = CYCLE_PREREPORT,
    [6 *   PERUNIT] = CYCLE_FIGHTREADY,
    [6.5 * PERUNIT] = CYCLE_FIGHT,
    [7 *   PERUNIT] = CYCLE_REST,
    [7.5 *   PERUNIT] = CYCLE_PREREPORT,
    [8 * PERUNIT] = CYCLE_PREREPORT,
    [8.5 *   PERUNIT] = CYCLE_FIGHTREADY,
    [9 * PERUNIT] = CYCLE_FIGHT,
    [9.5 *   PERUNIT] = CYCLE_END,
    [10 *   PERUNIT] = CYCLE_CLEAN,
}

tbLianSai_config.tbFinalBattleCycle_real16 = {  -- ÕýÊ½¾öÈüÓÃÊ±¼ä
    [0] = CYCLE_START,
    [5 *    PERUNIT] = CYCLE_PREREPORT,
    [9 *    PERUNIT] = CYCLE_PREREPORT,
    [9.5 *  PERUNIT] = CYCLE_PREREPORT,
    [10 *   PERUNIT] = CYCLE_FIGHTREADY,
    [10.5 * PERUNIT] = CYCLE_FIGHT,
    [15 *   PERUNIT] = CYCLE_REST,
    -------------------------------------
    [19 *   PERUNIT] = CYCLE_PREREPORT,
    [19.5 * PERUNIT] = CYCLE_PREREPORT,
    [20 *   PERUNIT] = CYCLE_FIGHTREADY,
    [20.5 * PERUNIT] = CYCLE_FIGHT,
    [25 *   PERUNIT] = CYCLE_REST,
    -------------------------------------
    [29 *   PERUNIT] = CYCLE_PREREPORT,
    [29.5 * PERUNIT] = CYCLE_PREREPORT,
    [30 *   PERUNIT] = CYCLE_FIGHTREADY,
    [30.5 * PERUNIT] = CYCLE_FIGHT,
    [35 *   PERUNIT] = CYCLE_REST,
    -------------------------------------
    [39 *   PERUNIT] = CYCLE_PREREPORT,
    [39.5 * PERUNIT] = CYCLE_PREREPORT,
    [40 *   PERUNIT] = CYCLE_FIGHTREADY,
    [40.5 * PERUNIT] = CYCLE_FIGHT,
    [45 *   PERUNIT] = CYCLE_END,
    [50 *   PERUNIT] = CYCLE_CLEAN,
}

tbLianSai_config.tbFinalBattleCycle_real32 = {  -- ÕýÊ½¾öÈüÓÃÊ±¼ä
    [0] = CYCLE_START,
    [5 *    PERUNIT] = CYCLE_PREREPORT,
    [9 *    PERUNIT] = CYCLE_PREREPORT,
    [9.5 *  PERUNIT] = CYCLE_PREREPORT,
    [10 *   PERUNIT] = CYCLE_FIGHTREADY,
    [10.5 * PERUNIT] = CYCLE_FIGHT,
    [15 *   PERUNIT] = CYCLE_REST,
    -------------------------------------
    [19 *   PERUNIT] = CYCLE_PREREPORT,
    [19.5 * PERUNIT] = CYCLE_PREREPORT,
    [20 *   PERUNIT] = CYCLE_FIGHTREADY,
    [20.5 * PERUNIT] = CYCLE_FIGHT,
    [25 *   PERUNIT] = CYCLE_REST,
    -------------------------------------
    [29 *   PERUNIT] = CYCLE_PREREPORT,
    [29.5 * PERUNIT] = CYCLE_PREREPORT,
    [30 *   PERUNIT] = CYCLE_FIGHTREADY,
    [30.5 * PERUNIT] = CYCLE_FIGHT,
    [35 *   PERUNIT] = CYCLE_REST,
    -------------------------------------
    [39 *   PERUNIT] = CYCLE_PREREPORT,
    [39.5 * PERUNIT] = CYCLE_PREREPORT,
    [40 *   PERUNIT] = CYCLE_FIGHTREADY,
    [40.5 * PERUNIT] = CYCLE_FIGHT,
    [45 *   PERUNIT] = CYCLE_REST,
    -------------------------------------
    [49 *   PERUNIT] = CYCLE_PREREPORT,
    [49.5 * PERUNIT] = CYCLE_PREREPORT,
    [50 *   PERUNIT] = CYCLE_FIGHTREADY,
    [50.5 * PERUNIT] = CYCLE_FIGHT,
    [55 *   PERUNIT] = CYCLE_END,
    [60 *   PERUNIT] = CYCLE_CLEAN,
}

tbLianSai_config.tbFinalBattleCycle_test32 = {     --¾öÈüÊ±¼ä²âÊÔÓÃ
    [0] = CYCLE_START,
    [0.5 *    PERUNIT] = CYCLE_PREREPORT,
    [1 *   PERUNIT] = CYCLE_FIGHTREADY,
    [1.5 * PERUNIT] = CYCLE_FIGHT,
    [2 *   PERUNIT] = CYCLE_REST,
    -------------------------------------
    [2.5 *   PERUNIT] = CYCLE_PREREPORT,
    [3 *   PERUNIT] = CYCLE_FIGHTREADY,
    [3.5 * PERUNIT] = CYCLE_FIGHT,
    [4 *   PERUNIT] = CYCLE_REST,
    -------------------------------------
    [4.5 *   PERUNIT] = CYCLE_PREREPORT,
    [5 *   PERUNIT] = CYCLE_FIGHTREADY,
    [5.5 * PERUNIT] = CYCLE_FIGHT,
    [6 *   PERUNIT] = CYCLE_REST,
    -------------------------------------
    [6.5 *   PERUNIT] = CYCLE_PREREPORT,
    [7 *   PERUNIT] = CYCLE_FIGHTREADY,
    [7.5 * PERUNIT] = CYCLE_FIGHT,
    [8 *   PERUNIT] = CYCLE_REST,
    -------------------------------------
    [8.5 * PERUNIT] = CYCLE_PREREPORT,
    [9 *   PERUNIT] = CYCLE_FIGHTREADY,
    [9.5 * PERUNIT] = CYCLE_FIGHT,
    [10 *   PERUNIT] = CYCLE_END,
    [10.5 *   PERUNIT] = CYCLE_CLEAN,
}

tbLianSai_config.tbFinalBattleCycle_test16 = {     --¾öÈüÊ±¼ä²âÊÔÓÃ
    [0] = CYCLE_START,
    [0.5 *    PERUNIT] = CYCLE_PREREPORT,
    [1 *   PERUNIT] = CYCLE_FIGHTREADY,
    [1.5 * PERUNIT] = CYCLE_FIGHT,
    [2 *   PERUNIT] = CYCLE_REST,
    -------------------------------------
    [2.5 *   PERUNIT] = CYCLE_PREREPORT,
    [3 *   PERUNIT] = CYCLE_FIGHTREADY,
    [3.5 * PERUNIT] = CYCLE_FIGHT,
    [4 *   PERUNIT] = CYCLE_REST,
    -------------------------------------
    [4.5 *   PERUNIT] = CYCLE_PREREPORT,
    [5 *   PERUNIT] = CYCLE_FIGHTREADY,
    [5.5 * PERUNIT] = CYCLE_FIGHT,
    [6 *   PERUNIT] = CYCLE_REST,
    -------------------------------------
    [6.5 * PERUNIT] = CYCLE_PREREPORT,
    [7 *   PERUNIT] = CYCLE_FIGHTREADY,
    [7.5 * PERUNIT] = CYCLE_FIGHT,
    [8 *   PERUNIT] = CYCLE_END,
    [8.5 *   PERUNIT] = CYCLE_CLEAN,
}

tbLianSai_config.tbBattleCycle = tbLianSai_config.tbBattleCycle_real
tbLianSai_config.tbFinalBattleCycle = tbLianSai_config.nFinalTeamCount == 32 and tbLianSai_config.tbFinalBattleCycle_real32 or tbLianSai_config.tbFinalBattleCycle_real16

function UseRealTime()
    tbLianSai_config.tbBattleCycle = tbLianSai_config.tbBattleCycle_real
    tbLianSai_config.tbFinalBattleCycle = tbLianSai_config.nFinalTeamCount == 32 and tbLianSai_config.tbFinalBattleCycle_real32 or tbLianSai_config.tbFinalBattleCycle_real16
end
function UseTestTime()
    tbLianSai_config.tbBattleCycle = tbLianSai_config.tbBattleCycle_test
    tbLianSai_config.tbFinalBattleCycle = tbLianSai_config.nFinalTeamCount == 32 and tbLianSai_config.tbFinalBattleCycle_test32 or tbLianSai_config.tbFinalBattleCycle_test16
end

function SetFinalTeamCount(nTeamCount)
    if nTeamCount ~= 16 and nTeamCount ~= 32 then
        return
    end
    tbLianSai_config.nFinalTeamCount = nTeamCount
    tbLianSai_config.tbFinalBattleCycle = tbLianSai_config.nFinalTeamCount == 32 and tbLianSai_config.tbFinalBattleCycle_real32 or tbLianSai_config.tbFinalBattleCycle_real16
    tbLianSai_config.tbFinalAward = tbLianSai_config.nFinalTeamCount == 32 and tbLianSai_config.tbFinalAward32 or tbLianSai_config.tbFinalAward16
end

tbLianSai_config.tbAward = {
    ["normal"] = {
        tbAward = {
            {szName = "HiÖp Kh¸ch §¬n", tbProp = {6,1,5109,1,0,0}},
        },
        szMailAward = "Item:6,1,5109,1,1;",
    },
    ["final"] = {
        tbAward = {
            {szName = "HiÖp Kh¸ch §¬n", tbProp = {6,1,5109,1,0,0}},
        },
        szMailAward = "Item:6,1,5109,1,1;",
    },
    ["gamble"] = {
        tbAward = {
            {szName = "HiÖp Kh¸ch §¬n", tbProp = {6,1,5109,1,0,0}},
        },
        szMailAward = "Item:6,1,5109,1,1;",
    },
}

-- function tbLianSaiManager:GetGuardLeader(szName)
--     local nType = self:CalcLianSaiFightType()
--     if nType ~= FIGHTTYPE_GUARD then
--         return
--     end

--     local nLid, _, szTeamName, nMcount = self:GetTeamGroupInfo(szName)
--     if LianSaiCheck(nLid) then
--         for nMember = 0, nMcount - 1 do
--             local szRoleName = LG_GetMemberInfo(nLid, nMember)
--             local nGuard = LG_GetMemberTask(tbLianSai_config.LS_LGTASK_OBJ, szTeamName, szRoleName, tbLianSai_config.LS_MEMBERTASK_GUARD)
--             if nGuard == 1 then
--                 return szRoleName
--             end
--         end
--     end
--     return ""
-- end