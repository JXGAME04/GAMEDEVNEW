COEF_CONTRIB_TO_VALUE	=	1000		--¹±Ï×¶È¼ÛÖµÁ¿×ª»¯ÏµÊý
MAX_SHITU_VALUE_STORE	=	10000000	--×î´óÊ¦áã¹ØÏµ¼ÛÖµÁ¿
MAX_WEEK_CONTRIBUTION	=	22400		--ÖÜÀÛ»ý¹±Ï×¶ÈÉÏÏÞ(10w/h*4h/1000*80%*7d)
MAX_KICK_COUNT			=	20			--Ã¿Ìì×î´óµÄÌÞ³ý°ï»á³ÉÔ±µÄ¸öÊý

--¹±Ï×¶ÈÈë¿Ú»î¶¯ÀàÐÍ
EVE_NOEVENT		=	0
EVE_SOJIN		=	1	--ËÎ½ð
EVE_XINSHI		=	2	--ÐÅÊ¹
EVE_CHUNGGUAN	=	3	--´³¹Ø
EVE_YESHOU		=	4	--Ò°ÛÅ
EVE_WULIN		=	5	--ÎäÁÖÁªÈü
EVE_FENGHUO		=	6	--·é»ð
EVE_HUADENG		=	7	--»¨µÆ
EVE_ZHONGZI		=	8	--ÖÖ×Ó
EVE_SHUIJI		=	9	--Ëæ»úÈÎÎñ
EVE_XIULIAN		=	10	--Í¬°éÐÞÁ¶
EVE_JUQING		=	11	--Í¬°é¾çÇé
EVE_JUANKUAN	=	12	--¾è¿î
EVE_KILLER		=	13	--É±ÊÖboss
EVE_FENGLINGDU	=	14	--·çÁê¶É¶É´¬
EVE_LONGMAI		=	15	--ÁúÂö

--ÖÜÄ¿±êÏà¹Ø
MIN_WEEKGOAL_MEMBER			=	100																--°ï»áÖÜÄ¿±ê×îÉÙÈËÊý
DAILY_PRICE			=	50	--Ã¿Ìì°ïÖÚ¿Éµ½¼ÀÌ³ÁìÈ¡µÄ½±Àø
TB_WEEKGOAL_PRICE_BASE	=	{ 2.5,	4.5,	6.75 }	--Èý¸öÄÑ¶È¼¶±ðµÄ½±Àø»ùÊý
TB_WEEKGOAL_CHANGE		=	{ 4,	8,		16 }	-- Èý¸öÄÑ¶È¼¶±ðµÄÖÜÊÖ¶¯Ð¡Ê±Êý
WEEKGOAL_VALUE_PERSON		=	150 * 8															--ÖÜÄ¿±ê¸öÈË¹±Ï×¶ÈÐèÍê³ÉÁ¿
WEEKGOAL_VALUE_TONG_SYNC	=	WEEKGOAL_VALUE_PERSON / 10										--ÖÜÄ¿±ê°ï»á¹±Ï×¶ÈÀÛ»ýÖµÍ¬²½ãÐÖµ
WEEKGOAL_PRICE_PERSON		=	1000															--ÖÜÄ¿±ê¸öÈË½±Àø¾­ÑéÖµ£¨µ¥Î»£ºÍò£©
WEEKGOAL_PRICE_TONG			=	10																--ÖÜÄ¿±ê°ï»á½±ÀøÕÙ»½ÁîÅÆÊý
TB_WEEKGOAL_TYPE_ID		= 	{ EVE_SOJIN,	EVE_XINSHI,	EVE_CHUNGGUAN,	EVE_YESHOU }	--ÖÜÄ¿±êÀàÐÍ
TB_WEEKGOAL_TYPE_NAME	=	{ "ChiÕn tr­êng Tèng Kim",	"NhiÖm vô TÝn Sø ",	"Th¸ch thøc thêi gian",		"Chuçi nhiÖm vô D· TÈu" }	--ÖÜÄ¿±êÀàÐÍÃû³Æ

--Íæ¼ÒÈÎÎñ±äÁ¿¶¨Òå
TASKID_CONTRIVALUE			=	2361			--¹±Ï×¼ÛÖµÁ¿
TASKID_STORED_OFFER			=	2389			--Íæ¼Òºó±¸¹±Ï×¶È
TASKID_STUNT_LASTWEEK		=	2443			--ÌØ¼¼ÁìÈ¡µÄ°ï»áÖÜÆÚ
TASKID_STUNT_LASTDAY		=	1580			--×îºóÒ»´ÎÁìÈ¡°ï»áÌØ¼¼µÄ°ï»áÌìÊý
TASKID_CITY_LASTDAY			=	1581			--×îºóÒ»´ÎÁìÈ¡³ÇÊÐÌØ¼¼µÄ°ï»áÌìÊý
TASKID_STUNT_TAKED			=	2444			--ÌØ¼¼ÖÜÆÚÄÚÊÇ·ñÁìÈ¡¹ý
TASKID_MONEYBOX_LASTTIME	=	2446			--ÌÕÖìÇ®´üÐ§Á¦½ØÖ¹Ê±¼ä
TASKID_PANAXBOX_OPEND		=	1577			--²Î¹Þ¿ªÆô¹Ø±Õ
TASKID_PANAXBOX_LASTTIME	=	1578			--²Î¹ÞÊ£ÓàÊ¹ÓÃÊ±¼ä
TASKID_CITYST_TAKED			=	1579			--ÊÇ·ñÁìÈ¡ÁË³ÇÊÐÌØ¼¼
TASKID_LASTHERO_DATE		=	2497			--×îºóÒ»´ÎÁìÈ¡·é»ðÓ¢ÐÛÁîµÄÈÕÆÚ£¨ÁìÈ¡Ê±ÅÐ¶Ï£¬¸üÐÂ£©	
TASKID_LASTHERO_TAKED		=	2498			--Ã¿ÈÕÁìÈ¡·é»ðÓ¢ÐÛÁîµÄ¸öÊý£¨µÝÔö£¬ÁìÈ¡Ê±ÅÐ¶Ï¸üÐÂ£©
TASKID_HEROLING_COUNT		=	2499			--ÀÛ»ýµÄÓ¢ÐÛÁî¸öÊý£¨¿É+¿É-£©
TASKID_CONTRIBUTE_USED		=	2517			--ÀÛ»ýÏûºÄµÄÖÜ¹±Ï×¶È£¨µÝÔö£¬ÁìÈ¡Ê±ÅÐ¶Ï¸üÐÂ£©

--°ï»á³ÉÔ±ÈÎÎñ±äÁ¿¶¨Òå
TONGMTSK_WEEK_GOAL_PRICE	=	1001			--ÖÜÄ¿±êÁì½±¼ÇÂ¼
TONGMTSK_DAILY_PRICE		=	1002			--¼ÀÌ³Áì½±¼ÇÂ¼
TONGMTSK_TOMASTER			=	1003			--Ê¦áã¹ØÏµ¼ÛÖµÁ¿
TONGMTSK_SELFCOMMEND_TIME	=	1037			--[°ïÖ÷×Ô¼ö]²ÎÓë°ïÖ÷×Ô¼öµÄÊ±¼ä£¬µ±×îÖÕÓÐ¶à¸ö¾ºÍ¶µÚÒ»Ãû£¬ÔòÒÔÊ±¼äÏÈºóÅÅÐò
TONGMTSK_MASTERCANDIDATE	=	1038			--[°ïÖ÷×Ô¼ö]ÊÇ(1)·ñ(0)ÊÇ°ïÖ÷ºòÑ¡ÈË
TONGMTSK_INICONTRIBUTIVENESS=	1039			--[°ïÖ÷×Ô¼ö]ÉêÇë°ïÖ÷Ê±µÄ¹±Ï×¶È
TONGMTSK_CONTRIBUTIVENESS	=	1040			--[°ïÖ÷×Ô¼ö]¾ºÑ¡µÄ¸öÈË¹±Ï×¶È

--°ï»áÈÎÎñ±äÁ¿¶¨Òå
TONGTSK_WEEKGOAL_COMPLETE	=	1006			--°ï»áÉÏÖÜÄ¿±êÊÇ·ñÍê³É
TONGTSK_LAST_LEVELUP_DAY	=	1007			--°ï»á½¨ÉèµÈ¼¶ÉÏ´ÎÌáÉýÊ±¼ä
TONGTSK_WEEKGOAL_PRICE_WEEK	=	1009			--¼ÇÂ¼ÖÜÄ¿±êÒÑÁì¹ý½±µÄÉÏÒ»¸öÖÜÊý
TONGTSK_LAST_DEGRADE_DAY	=	1010			--°ï»áÉÏ´Î½µ¼¶Ê±¼ä
TONGTSK_STUNT_ID			=	1011			--°ï»áÌØ¼¼±àºÅ
TONGTSK_STUNT_MEMLIMIT		=	1012			--°ï»áÌØ¼¼Ã¿¸öÊ±¶ÎÄÜ¹»ÁìÈ¡ÈËÊý
TONGTSK_STUNT_MAINPERIOD	=	1013			--°ï»áÌØ¼¼Î¬»¤ÖÜÆÚ£¬ÒÔÌì¼ÆËã£¬ÒÔ½ØÖ¹ÈÕÆÚ
TONGTSK_STUNT_ENABLED		=	1014			--°ï»áÌØ¼¼ÊÇ·ñ¼¤»î£¬1-¼¤»î£¬0-ÔÝÍ£
TONGTSK_STUNT_SWICTH		=	1021			--ÏÂÒ»ÖÜÆÚ½«Òª¸ü¸ÄµÄ°ï»áÌØ¼¼
TONGTSK_TOTEMINDEX			=	{ 1022, 1023 }	--¼ÇÂ¼Í¼ÌÚÖ®ÖùNPCµÄindex£¬1027Îª×ó±ßµÄ£¬1028ÎªÓÒ±ßµÄ¡£ÓÒ±ßµÄÔÚ°ï»áÎª5¼¶µÄÕ¼³Ç°ïÊ±±»¸ü»»Îª³ÇÊÐÍ¼ÌÚÖ®Öù
TONGTSK_CITYST_MEMLIMIT		=	1034			--³ÇÊÐÌØ¼¼ÄÜ¹»ÁìÈ¡ÈËÊý
TONGTSK_MONEYBOX_DROPMAX	=	1035			--ÌÕÖìÇ®´ü·­±¶µôÂäboss±¦Ó¡µÄÏÞÖÆ
TONGTSK_STUNT_PAUSE			=	1036			--°ï»áÔÝÍ£Î¬»¤ÌØ¼¼
TONGTSK_OPEN_DATE			=	1041			--[°ïÖ÷×Ô¼ö]°ïÖ÷×Ô¼ö¿ªÆôµÄÊ±¼ä£¬Èç¹ûÎª0±íÊ¾Ã»ÓÐ¿ªÆô
TONGTSK_LAST_WM_DAY			=	1050			--°ï»áÉÏ´ÎÖÜÎ¬»¤ÈÕ×Ó
TONGTSK_LUP_ORDER			=	1051			--°ï»áÉý¼¶Ãû´Î
TONGTSK_LUP_PRICE			=	1052			--°ï»áÉý¼¶½±Àø
TONGTSK_HEROLING			=	1043			--°ï»áÃ¿ÖÜÀÛ»ýµÄÓ¢ÐÛÁî£¨Ã¿ÖÜÇå¿Õ£¬µÝÔö£©
TONGTSK_USEHEROLING			=	1044			--°ï»áÏûºÄµÄÓ¢ÐÛÁî£¨Ã¿ÖÜÇå¿Õ£¬µÝÔö£©
TONGTSK_MAP_EXPIRE_DATE		=	1045			--°ï»áµØÍ¼µÄÓÐÐ§ÈÕÆÚ
TONGTSK_MEMBER_KICK_COUNT	=	1046			--ÌÞ³ý°ï»á³ÉÔ±µÄ¼ÆÊý
TONGTSK_MEMBER_KICK_DATE	=	1047			--ÌÞ³ý°ï»á³ÉÔ±µÄ¼ÆÊýÈÕÆÚ

TONGTSK_CLAIMWAR_DATE   = 1053         --Ä³°ï»áÐûÕ½ÈÕÆÚ, add by wdb
TONGTSK_CLAIMWAR_TIMES  = 1054		   --Ä³°ï»áÔÚÄ³ÌìÐûÕ½µÄ´ÎÊý





--ÓÃÓÚLog
TONGTSK_WEEK_BFADD			=	1100
TONGTSK_WEEK_BFCONSUME		=	1101
TONGTSK_WEEK_WFADD			=	1102
TONGTSK_WEEK_WFCONSUME		=	1103
TONGTSK_WEEK_WSCONSUME		=	1104
TONGTSK_MONEYFUND2BF		=	1105
TONGTSK_MONEY2BF			=	1106
TONGTSK_BF2WF				=	1107

--ÓÃÓÚ»î¶¯µÄÁÙÊ±°ï»áÈÎÎñ±äÁ¿
TONGTSK_Gongde_meici = 1205
TONGTSK_Gongde_meici_flag = 1206 -- ¼ÇÂ¼ÄêÔÂÈÕÊ±
TONGTSK_Gongde_meizhou = 1208 --
TONGTSK_Gongde_meizhou_flag = 1209 -- ¼ÇÂ¼%j
TONGTSK_Gongde_Total = 1210

TONGTSK_TriumphDrum_LastUseDay = 1211	-- ÉÏ´ÎÊ¹ÓÃÈÕÆÚ
TONGTSK_TriumphDrum_BeUsed = 1212		-- ÊÇ·ñÊ¹ÓÃ¹ý

--VNG
--TONGTSK_VNG_CUSTOM1 = 1213
TONGTSK_VNG_CUSTOM2 = 1214 -- thÎ t©n thñ
TONGTSK_VNG_CUSTOM3 = 1215
TONGTSK_VNG_CUSTOM4 = 1216



--°ï»áÁÙÊ±ÈÎÎñ±äÁ¿¶¨Òå
--¹±Ï×¶ÈÈë¿ÚÊý¾Ý»º´æ
TONG_TEMPSTART				=	100				--ÁÙÊ±±äÁ¿ID±ØÐë´óÓÚ´ËÖµ
TONG_WEEKTEMP				=	101				--ÖÜÄ¿±êÀÛ»ý¹±Ï×¶È»º´æ
TONG_STORETEMP				=	102				--´¢±¸¹±Ï×¶È»º´æ
TONG_FUNDTEMP				=	103				--»ù½ð»º´æ
TONG_TMPWEEKGOALPRICE		=	104				--ÖÜÄ¿±êÁì½±ÁÙÊ±

--ÍËÒþÏà¹Ø
TONGMEMBER_RETIRE_MAX_RATE		=	0.5			-- ÍËÒþÈËÊý×î´ó±ÈÀý
TONGMEMBER_RETIRE_REQUIRE_DAY	=	7			-- ÃüÁîÍËÒþÐèÒª°ïÖÚ7Ìì²»ÔÚÏß²Å¿É½øÐÐ
TONGMEMBER_UNRETIRE_REQUIRE_DAY	=	7			-- »Ö¸´ÍËÒþÐèÒªÔÚÍËÒþ7Ììºó²Å¿É½øÐÐ

-- °ï»áÖ°ÎñID
TONG_MASTER		= 0		--°ïÖ÷
TONG_ELDER		= 1		--³¤ÀÏ
TONG_MANAGER	= 2		--¶Ó³¤
TONG_MASS		= 3		--°ïÖÚ
TONG_RETIRE		= 4		--ÒþÊ¿

-- °ï»áÖ°ÎñÃû³Æ
TB_TONG_FIGURE_NAME =
{
	[TONG_MASTER]	=	"Bang chñ ",
	[TONG_ELDER]	=	"Tr­ëng L·o",
	[TONG_MANAGER]	=	"§éi tr­ëng",
	[TONG_MASS]		=	"§Ö tö ",
	[TONG_RETIRE]	=	"Èn sü",
};

MAX_ELDER_COUNT		=	7		-- ×î´ó³¤ÀÏÊýÁ¿
MAX_MANAGER_COUNT	=	56		-- ×î´ó¶Ó³¤ÊýÁ¿


-- °ï»áÈ¨ÏÞID
RIGHTID_WORKSHOP	=	9001	-- ×÷·»¹ÜÀí
RIGHTID_WEEKGOAL	=	2005;	-- ÖÜÄ¿±ê¹ÜÀí
RIGHTID_CITY		=	2003	-- ³ÇÊÐ¹ÜÀí
RIGHTID_STUNT		=	2006	-- ÌØ¼¼¹ÜÀí

DYNMAP_ID_BASE		=	70000	-- ¶¯Ì¬µØÍ¼ÆðÊ¼Öµ

-- ÌØ¼¼Ïà¹Ø
TB_STUNT_RIGHT_DS	=	{ "CÊp l·nh ®¹o", "TÊt c¶ thµnh viªn" }
TB_TOTEMPOLE_POS	=	{ {1665, 3253}, {1657, 3262} }
TB_CITYID_STUNTID	=
{
	[1] = {" Ph­îng T­êng", 5},
	[2] = {" Thµnh §«", 1},
	[3] = {" §¹i Lý ", 6},
	[4] = {" BiÖn Kinh", 7},
	[5] = {" T­¬ng D­¬ng", 3},
	[6] = {" D­¬ng Ch©u", 2},
	[7] = {" L©m An", 4},
}
TB_STUNT_PAUSESTATE	=
{
--[stateid] µ±Ç°×´Ì¬ ÏÂÖÜ×´Ì¬ ×¼Ðí¸Ä±äµÄ×´Ì¬ Î¬»¤ºóµÄ×´Ì¬
	[0] = {" §ãng", " §ãng", 1, 0},
	[1] = {" §ãng", "Khai më", 0, 3},
	[2] = {"Khai më", " §ãng", 3, 0},
	[3] = {"Khai më", "Khai më", 2, 3},
}

-- °ïÖú×Ô¼öÏà¹Ø
CAST_DURATION		=	14;				--¾ºÍ¶ÈÕÆÚ
MASTER_ASIDE_TIME	=	30;				--°ïÖ÷Àë¿ªµÄÊ±¼ä
FORETASKVALUE		=	2240;			--Ç°ÌáÈÎÎñÄ¿±êÖµ