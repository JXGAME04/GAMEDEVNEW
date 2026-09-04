-- Ò°ÛÅÈÎÎñÁ´½Å±¾
-- Edited by peres
-- 2004/12/24 Ê¥µ®½ÚÇ°Ò¹

IncludeLib("FILESYS");
IncludeLib("RELAYLADDER");	--ÅÅĞĞ°ñ
Include("\\script\\task\\newtask\\tasklink\\tasklink_head.lua"); -- ÈÎÎñÁ´µÄÍ·ÎÄ¼ş
Include("\\script\\task\\newtask\\tasklink\\tasklink_award.lua"); -- ÈÎÎñÁ´µÄ½±ÀøÍ·ÎÄ¼ş
Include("\\script\\event\\storm\\function.lua")	--Storm
Include("\\script\\lib\\awardtemplet.lua")
Include("\\script\\lib\\log.lua")
Include("\\script\\activitysys\\g_activity.lua")
Include("\\script\\activitysys\\playerfunlib.lua")
-- [JX1 PORT 15/08/2026] JX1 moi file .lua co Lua state rieng nen handler awardtype
-- phai nap ngay trong state nay; phai nam SAU moi Include khac vi awardtemplet.lua
-- bi include lai (qua dailogsay) se reset bang TYPE.
Include("\\script\\lib\\awardtype\\exp.lua")
Include("\\script\\lib\\awardtype\\item_jx1.lua")

--Storm ÕæÕıµÄ½ÓĞÂÈÎÎñ
function storm_goon_start(gameid, b_nonext)
	
	local nNum     = GetTask(ID_TASKLINK_LIMITNUM);
	
		-- Ã¿ÌìµÄÏŞÖÆ´ÎÊı +1
		nNum = nNum + 1;
		SetTask(ID_TASKLINK_LIMITNUM, nNum);
		
	if b_nonext then return end
	tl_dealtask()
	Task_MainDialog()
end

-- ¼ì²âÈÎÎñÊÇ·ñÒÑ¾­¹ıÁËÃ¿ÌìµÄÏŞÖÆ
function checkTask_Limit()
	
-- Ã¿ÌìÏŞÖÆ´ÎÊıµÄ´¦Àí
local nNowDate = tonumber(GetLocalDate("%y%m%d"));
local nOldDate = GetTask(ID_TASKLINK_LIMITDATE);
local nNum     = GetTask(ID_TASKLINK_LIMITNUM);
	
	if nNowDate~=nOldDate then
		
		SetTask(ID_TASKLINK_LIMITDATE, tonumber(GetLocalDate("%y%m%d")) );
		SetTask(ID_TASKLINK_LIMITNUM, 0);
		SetTask(ID_TASKLINK_LIMITCancelCount, 0);
		return 1;
		
	else
		-- 40 ´ÎµÄÏŞÖÆ
		if nNum>=40 then
			Say("Ha! Ha! VŞ thiÕu hŞªp nµy! Mçi ngµy lµm 40 lÇn lµ ®ñ råi! Ngµy mai trë l¹i nhĞ!", 0);
			return 0;
		end;
		
		return 1;

	end;
	
end;
	
	
--Task_BuyGoods = {}
--Task_FindGoods = {}
--Task_ShowGoods = {}
--Task_FindMaps = {}
--Task_UpGround = {}
--Task_WorldMaps = {}
--Task_Level = {}
--Task_MainLevelRate = {}


-- ¹¹ÔìÈÎÎñÄÚ´æ±äÁ¿
Task_BuyGoods = AssignValue(Task_BuyGoods,TL_BUYGOODS)
Task_FindGoods = AssignValue(Task_FindGoods,TL_FINDGOODS)
Task_ShowGoods = AssignValue(Task_ShowGoods,TL_SHOWGOODS)
Task_FindMaps = AssignValue(Task_FindMaps,TL_FINDMAPS)
Task_UpGround = AssignValue(Task_UpGround,TL_UPGROUND)
Task_WorldMaps = AssignValue(Task_WorldMaps,TL_WORLDMAPS)

Task_MainTaskLink = AssignValue_TaskLink(Task_MainTaskLink,TL_LEVELLINK)
Task_MainLevelRate = AssignValue_TaskRate(Task_MainLevelRate,TL_MAINTASKLEVEL)

-- ¹¹Ôì½±ÀøÄÚ´æ±äÁ¿
Task_AwardBasic = AssignValue_Award(Task_AwardBasic,TL_AWARDBASIC)

-- ¹¹ÔìÍê³É´ÎÊıµÄ½±Àø
Task_AwardLink = AssignValue_LinkAward(TL_AWARDLINK)

Task_AwardLoop = AssignValue_Award(Task_AwardLoop,TL_AWARDLOOP)

-- ¹¹Ôì¶Ô»°ÄÚÈİÄÚ´æ±äÁ¿
Task_TalkGoods = AssignValue_TaskTalk(Task_TalkGoods,TL_TASKGOODSTALK)
Task_TalkBuy = AssignValue_TaskTalk(Task_TalkBuy,TL_TASKBUYTALK)
Task_TalkShow = AssignValue_TaskTalk(Task_TalkShow,TL_TASKSHOWTALK)
Task_TalkFind = AssignValue_TaskTalk(Task_TalkFind,TL_TASKFINDMAPS)
Task_TalkUp = AssignValue_TaskTalk(Task_TalkUp,TL_TASKUPGROUNDTALK)
Task_TalkWorld = AssignValue_TaskTalk(Task_TalkWorld,TL_TASKWORLDTALK)


function Task_NewVersionAward()
	
	local nNum = GetTask(ID_TASKLINK_LIMITNUM);
	local nCancelNum = GetTask(ID_TASKLINK_LIMITCancelCount);
	DynamicExecuteByPlayer(PlayerIndex, "\\script\\huoyuedu\\huoyuedu.lua", "tbHuoYueDu:AddHuoYueDu", "yesourenwu")
	G_ACTIVITY:OnMessage("FinishYesou", nNum, nCancelNum);
	
	-- Ã¿ÌìÍê³ÉµÚ30¸ö¶îÍâ½±Àø
	if (nNum - nCancelNum) == 30 then
		Msg2Player("H«m nay hoµn thµnh nhiÖm vô D· TÈu lÇn thø 30, nhËn ®­îc kinh nghiÖm thªm!");
		tbAwardTemplet:GiveAwardByList({nExp_tl = 30000000}, "PhÇn th­ëng kinh nghiÖm hoµn thµnh nhiÖm vô D· TÈu h»ng ngµy")
	end
	
	-- Ã¿ÌìÍê³ÉµÚ40¸ö¶îÍâ½±Àø
	if (nNum == 40 and nCancelNum) == 0 then
		Msg2Player("H«m nay hoµn thµnh liªn tôc nhiÖm vô D· TÈu lÇn thø 40, nhËn ®­îc kinh nghiÖm thªm!");
		tbAwardTemplet:GiveAwardByList({nExp_tl = 100000000}, "PhÇn th­ëng kinh nghiÖm khi hoµn thµnh nhiÖm vô D· TÈu h»ng ngµy")
		-- [16/08/2026 chu game chot] du 40 nhiem vu/ngay khong huy: tang them 5 ruong (nhu ruong Linux)
		if (CalcFreeItemCellCount() < 6) then
			Msg2Player("CHU Y: hanh trang sap day - hay chua it nhat 6 o trong de nhan du 5 ruong!");
		end
		-- [MAIL 03/09 D9] 5 ruong moc 40 nhiem vu/ngay gui qua thu
		Include("\\script\\mail\\mailmanager.lua")
		MailManager_SendRewardTemplet("datau", nil, "Th­ëng 40 nhiÖm vô D· TÈu trong ngµy", "Hoµn thµnh 40 nhiÖm vô D· TÈu trong ngµy kh«ng huû, 5 B¶o r­¬ng thÇn bİ ®İnh kÌm trong th­.<enter>Tr©n träng", {{tbProp = {6, 1, 2383, 1, 0, 0}, nCount = 5}}, 30)
		Msg2Player("Hoan thanh 40 nhiem vu, nhan them 5 B¶o r­¬ng thÇn bİ cña D· TÈu!");
		-- [PB 19/08] Bang chu TOAN SERVER (chu game: "cho toan bo nguoi trong game thay").
		-- AddGlobalCountNews(chuoi, giay) - tran 255 byte (ScriptFuns.cpp:1186), giu duoi 240.
		AddGlobalCountNews(" <color=Yellow>"..GetName().." <color>".."®· hoµn thµnh"..
			" <color=AYellow>40 <color>".."nhiÖm vô D· TÈu trong ngµy, nhËn"..
			" <color=Gold>".."100 triÖu kinh nghiÖm vµ 5 B¶o r­¬ng thÇn bİ".." <color>", 10)
	end	
	
end


function main()
	-- [MO LAI 15/08/2026] VNG dong NPC ngay 20141030 bang "do return end" o day; da go de khoi phuc ban goc.
	
	--ÓĞÓÛÀ¼½Ú»î¶¯ÆÚ¼ä¶Ô»°¡£
	local nDate = tonumber(GetLocalDate("%y%m%d"));
	if (nDate >= 60808 and nDate <= 60815) then
		local tab_Content = {
			"Xem cßn nhiÖm vô g× ®Ó lµm kh«ng/tasklink_entence",
		}
		tinsert(tab_Content, "Ta muèn lµm mét vßng Liªn Hoa/menglanjie");
		tinsert(tab_Content, "Rêi khái/Task_Wait");
		Say("MÊy n¨m nay chiÕn tranh triÒn miªn, d©n t×nh thèng khæ. Ph­¬ng tr­îng ThiÕu L©m tù muèn tæ chøc thä trai mõng LÔ Vu Lan ®Ó mäi ng­êi h­ëng chót gi©y phót yªn b×nh", getn(tab_Content), tab_Content);
		return
	end;
	
	tasklink_entence();
end;

function menglanjie()
	if (GetExtPoint(0) <= 0 or GetLevel() < 30) then
		Say("Xin lçi! ChØ cã ng­êi ch¬i tõ cÊp <color=red>30, ®· n¹p thÎ <color> míi cã thÓ tham gia ho¹t ®éng nµy", 0)
		return 
	end;
	local tab_Content = {
		"Ta muèn kÕt vßng Kim Liªn Hoa [cÇn 9 Kim Liªn Hoa]/#process(1)",
		"Ta muèn hîp thµnh vßng Méc Liªn Hoa [cÇn 9 Méc Liªn Hoa]/#process(2)",
		"Ta muèn hîp thµnh vßng Thñy Liªn Hoa [cÇn 9 Thñy Liªn Hoa]/#process(3)",
		"Ta muèn hîp thµnh vßng Háa Liªn Hoa [cÇn 9 Háa Liªn Hoa]/#process(4)",
		"Ta muèn hîp thµnh vßng Thæ Liªn Hoa [cÇn 9 Thæ Liªn Hoa]/#process(5)",
		"ChØ ®Õn th¨m «ng chót th«i!/Task_Wait"
	}
	Say("ChØ cÇn ng­¬i ®i thu thËp ®ñ sè Liªn Hoa vµ <color=yellow>"..MAKING_COST.."<color> l­îng ta sÏ gióp ng­¬i kÕt vßng hoa tuyÖt ®Ñp. Nh­ng l­u ı: mçi ngµy chØ cã thÓ kÕt ®­îc 2 vßng Liªn Hoa cïng thuéc tİnh mµ th«i.", getn(tab_Content), tab_Content);
end;

--{name, product}
MAKING_COST = 10000;
MAKING_COUNT = 9;
tab_Flower = {
	{1126, "Kim", 1131, 1760, 1761}, {1127, "Méc", 1132, 1762, 1763}, {1128, "Thñy ", 1133, 1764, 1765}, {1129, "Háa", 1134, 1766, 1767}, {1130, "Thæ ", 1135, 1768, 1769}
}

function process(nIdx)
	if (nIdx < 1 or nIdx > getn(tab_Flower)) then
		return
	end;
	
	local nIndex;
	if (nIdx == 1) then
		nIndex = 1;
	elseif (nIdx == 2) then
		nIndex = 2;
	elseif (nIdx == 3) then
		nIndex = 3;
	elseif (nIdx == 4) then
		nIndex = 4;
	elseif (nIdx == 5) then
		nIndex = 5;
	end;
	
	Say("Muèn lµm 1 vßng <color=yellow>"..tab_Flower[nIdx][2].." Liªn Hoa<color> cÇn cã 9 <color=yellow>"..tab_Flower[nIdx][2].."Liªn Hoa<color>. Ng­¬i x¸c ®Şnh lµm chø?", 2, "§óng! Xin l·o bèi træ tµi!/#make_round("..nIndex..")", "Ta sÏ quay l¹i sau!/Task_Wait");
end;

function make_round(nIdx)
	if (GetCash() < MAKING_COST) then
		Say("Xin t×m ®ñ 10000 l­îng råi h·y quay l¹i! Ta ë ®©y chê!", 1, "§­îc th«i! Ta ®i lÊy thªm tiÒn!/Task_Wait");
		return
	end;
	
	local nCount = CalcEquiproomItemCount(6,1,tab_Flower[nIdx][1],-1);
	if (nCount < 9) then
		Say("Ng­¬i h×nh nh­ ch­a ®ñ <color=yellow>"..tab_Flower[nIdx][2].." Liªn Hoa<color=yellow>. Ch­a ®ñ 9 <color=yellow>"..tab_Flower[nIdx][2].." Liªn Hoa<color> th× ta kh«ng thÓ gióp ng­¬i kÕt vßng "..tab_Flower[nIdx][2].." Liªn Hoa hoµn", 1, "§Ó ta ®i chuÈn bŞ ®·!/Task_Wait");
		return
	end;
	
	local nDate = tonumber(GetLocalDate("%y%m%d"));
	if (nDate ~= GetTask(tab_Flower[nIdx][4])) then
		SetTask(tab_Flower[nIdx][4], nDate);
		SetTask(tab_Flower[nIdx][5], 0);
	end;
	
	local nTimes = GetTask(tab_Flower[nIdx][5]);
	if (nTimes >= 2) then
		Say(tab_Flower[nIdx][2].."H«m nay ®· kÕt thµnh c«ng 2 vßng råi! Mai h·y ®Õn nhĞ!", 0)
		return
	end;
	
	Pay(MAKING_COST);
	ConsumeEquiproomItem(9, 6, 1, tab_Flower[nIdx][1], -1);
	SetTask(tab_Flower[nIdx][5], nTimes + 1);
	AddItem(6, 1, tab_Flower[nIdx][3], 1, 0, 0, 0);
	Say("Vßng "..tab_Flower[nIdx][2].." Liªn Hoa cña ng­¬i ®· kÕt xong. H·y mang nã ®Õn chç LÔ Quan ®i!", 1, "NhËn vßng hoa/Task_Wait");
	Msg2Player("B¹n nhËn ®­îc mét"..tab_Flower[nIdx][2].." Liªn Hoa hoµn");
end;
    
-- ÈÎÎñÁ´µÄÈë¿Ú
function tasklink_entence()

_TaskLinkDebug() -- ¶ÔÓÚÈÎÎñÁ´ÎŞ·¨½øĞĞÏÂÈ¥µÄ BUG ĞŞ¸´

local myTaskTimes = tl_gettaskstate(1) -- Íæ¼Ò½øĞĞµ½µÄ´ÎÊı
local myTaskLinks = tl_gettaskstate(2) -- Íæ¼Ò½øĞĞµ½µÄÁ´Êı
local myTaskLoops = tl_gettaskstate(3) -- Íæ¼Ò½øĞĞµ½µÄ»·Êı
local myTaskCancel = tl_gettaskstate(4) -- Íæ¼Ò¿ÉÒÔÈ¡ÏûµÄ´ÎÊı

local myCountLinks
local myCountTimes

local myCanceled = nt_getTask(1036)
local myTaskTime = GetGameTime() -- »ñÈ¡ÓÎÏ·Ê±¼ä£¬ÓÃÒÔÅĞ¶ÏÍæ¼ÒÊÇ·ñ±»·â
local n = myTaskTime - nt_getTask(1029)
local myTaskInfo = ""


-- Èç¹ûÍæ¼Òµ±Ç°µÄ×´Ì¬Îª»¹Î´·¢½±µÄ»°£¬ÔòÖØĞÂ·¢½±
if (tl_gettaskcourse() == 2) then
	Task_GiveAward()
	return
end

	if ( myCanceled == 10) then
		-- [JX1 PORT 16/08/2026] goc Linux GetGameTime = GIAY (phat 605s ~ 10 phut);
		-- JX1 GetGameTime = TICK 18/giay (KSubWorldSet.h:34 m_nLoopRate) -> quy doi 605*18.
		if (n < 10890) then
			Task_Punish()
			return
		else
			myCanceled = 0
			nt_setTask(1036,0)
			Task_Confirm()
		end
	end

	if (tl_gettaskcourse() == 0) then
		-- Èç¹û»¹Ã»ÓĞ¿ªÊ¼Ò°ÛÅµÄÈÎÎñÁ´µÄ»°
		Say(" VŞ nµy"..GetPlayerSex().."xem ra ®· b«n ba giang hå ®­îc mét thêi gian dµi råi nhØ, cã muèn tham gia kh¶o nghiÖm nhiÖm vô liªn tôc cña ta kh«ng?",3,"§­îc th«i! Ta kh«ng tin cã nhiÖm vô nµo lµm khã dÔ ®­îc ta/Task_Confirm","Ta muèn biÕt kh¶o nghiÖm cña ng­¬i nãi cã néi dung ra sao/Task_Info","Ta bËn råi, kh«ng r¶nh ngåi t¸n gÉu víi «ng/Task_Exit");
	end
	
	if (tl_gettaskcourse() == 3) then

		-- ¼ì²âÊÇ·ñ³¬¹ıÃ¿ÌìµÄÏŞÖÆ
		if checkTask_Limit()~=1 then return end;
	
		myCountLinks = tl_counttasklinknum(2)
		myCountTimes = tl_counttasklinknum(1)
		
		Say(":<enter>VŞ "..GetPlayerSex().."®· hoµn thµnh <color=yellow>"..myCountTimes.."<color>, nhiÖm vô nµy ngµy h«m nay cã thÓ thùc hiÖn l¹i<color=yellow>"..(40 - GetTask(ID_TASKLINK_LIMITNUM)).."<color>, ng­¬i cã muèn lµm n÷a kh«ng?",2,"§­¬ng nhiªn, mau cho ta biÕt nhiÖm vô tiÕp theo lµ g× /Task_TaskProcess","§Ó ta nghØ ng¬i mét l¸t ®·! Ta bËn råi/Task_Wait");
	elseif (tl_gettaskcourse() == 1) then
		Task_MainDialog()
	end

end

-- ´ğÓ¦ÁËÒ°ÛÅµÄ¿¼Ñé£¬ÕıÊ½¿ªÊ¼ÈÎÎñÁ´
function Task_Confirm()

local myTaskID

-- ÉèÖÃÈÎÎñ±äÁ¿µÄÊıÖµ£¬1020 ºÅ±äÁ¿ÎªÈÎÎñÁ´×´Ì¬×¨ÓÃ±äÁ¿£¬ÆäÖĞµÄËÄ¸ö×Ö½Ú·Ö±ğ¶¨ÒåÎª£º| ¢Ù´ÎÊı | ¢ÚÁ´Êı | ¢Û»·Êı | ¢ÜÈ¡ÏûÊ£Óà´ÎÊı |

tl_settaskstate(1,0) -- ÕıÔÚ½øĞĞµÚÒ»´ÎÈÎÎñ
tl_settaskstate(2,tl_getfirstlink()) -- ÕıÔÚ½øĞĞµÚÒ»Á´ÈÎÎñ
tl_settaskstate(3,tl_getfirstloop()) -- ÕıÔÚ½øĞĞµÚÒ»»·ÈÎÎñ

tl_settaskstate(4,0) -- Ê£Óà 0 ´ÎÈ¡ÏûÈÎÎñµÄ»ú»á
nt_setTask(DEBUG_TASKVALUE, 0);

tl_settaskstate(6,0)

storm_ask2start(4)	--Storm ¿ªÊ¼ÌôÕ½
tbLog:PlayerActionLog("TinhNangKey","NhanNhiemVuDaTau")
end


-- ½øĞĞÏÂÒ»´ÎÈÎÎñ
function Task_TaskProcess()

local myTaskTimes = tl_gettaskstate(1) -- Íæ¼Ò½øĞĞµ½µÄ´ÎÊı
local myTaskLinks = tl_gettaskstate(2) -- Íæ¼Ò½øĞĞµ½µÄÁ´Êı
local myTaskLoops = tl_gettaskstate(3) -- Íæ¼Ò½øĞĞµ½µÄ»·Êı
local myTaskCancel = tl_gettaskstate(4) -- Íæ¼Ò¿ÉÒÔÈ¡ÏûµÄ´ÎÊı


-- ¼ì²âÊÇ·ñ³¬¹ıÃ¿ÌìµÄÏŞÖÆ
if checkTask_Limit()~=1 then return end;

tl_taskprocess() -- Ê×ÏÈÖ´ĞĞÏÂÒ»ÂÖº¯Êı

storm_goon_start()	--Storm»Ö¸´

end


-- ÈÎÎñÁ´µÄÖ÷Òª¿ØÖÆÃæ°æ
function Task_MainDialog()

local myTaskInfo = tl_gettaskinfo() -- ÈÎÎñµÄÏêÏ¸ĞÅÏ¢
local myTaskTotalNum = tl_counttasklinknum(1) + 1; -- ×ÜÈÎÎñ´ÎÊıÍ³¼Æ
local myTitleText = "";

if (myTaskInfo == nil) then
	myTaskInfo = ""
end

-- ÉèÖÃÈÎÎñ×´Ì¬ÎªÒÑ¾­½ÓÁËÈÎÎñµ«Î´Íê³É
tl_settaskcourse(1)


if myTaskTotalNum==0 or myTaskTotalNum==nil then
	myTitleText = "D· TÈu:<enter><enter>"..myTaskInfo;
else
	myTitleText = "D· TÈu:<enter><enter> §©y lµ <color=green>"..myTaskTotalNum.."<color> nhiÖm vô thø, "..myTaskInfo;
end;

Say(myTitleText,
	4,
	"BiÕt råi, ®Ó ta hoµn thµnh nhiÖm vô xong míi l¹i t×m ng­¬i/Task_Wait",
	"ta ®· hoµn thµnh nhiÖm vô lÇn nµy, xin h·y kiÓm tra l¹i!/Task_Accept",
	"NhiÖm vô lÇn nµy khã qu¸, Ta muèn hñy bá kh«ng lµm n÷a/Task_CancelConfirm",
	"Ta muèn biÕt kh¶o nghiÖm cña ng­¬i nãi cã néi dung ra sao/Task_Info"
	);

-- ÉèÖÃÈ¡ÏûÈÎÎñµÄ±ê¼ÇÎª¿ÉÒÔÈ¡Ïû
nt_setTask(1045, 1);

end


-- È¡ÏûÈÎÎñÊ±¸øÓèÍæ¼ÒÒ»´ÎÈ·ÈÏµÄ»ú»á
function Task_CancelConfirm()

local myTaskTimes = tl_gettaskstate(1) -- Íæ¼Ò½øĞĞµ½µÄ´ÎÊı
local myTaskLinks = tl_gettaskstate(2) -- Íæ¼Ò½øĞĞµ½µÄÁ´Êı
local myTaskLoops = tl_gettaskstate(3) -- Íæ¼Ò½øĞĞµ½µÄ»·Êı
local myTaskCancel = tl_gettaskstate(4) -- Íæ¼Ò¿ÉÒÔÈ¡ÏûµÄ´ÎÊı

local myCountTimes = tl_counttasklinknum(1);  -- Íæ¼Ò½øĞĞµÄÈÎÎñ×ÜÊı

if (myTaskCancel==0) then
	
	Say(" HiÖn t¹i b¹n kh«ng cã c¬ héi nµo ®Ó hñy bá nhiÖm vô, b¹n chØ cã thÓ lµm l¹i tõ ®Çu th«i! §ång thêi phÇn th­ëng tİch lòy cña b¹n lóc tr­íc sÏ ®­îc tİnh l¹i tõ ®Çu.",
		3,
		"§óng, ta kh«ng muèn lµm nhiÖm vô quû qu¸i nµy ®©u/Task_NormalCancel",
		"Ta muèn sö dông 100 m¶nh s¬n Hµ X· T¾c ®Ó hñy bá nhiÖm vô lÇn nµy/#Task_Cancel(2)",
		"Uhm! §Ó ta suy nghÜ l¹i ®·/Task_Wait");
	
else
	
	Say(" HiÖn t¹i b¹n cßn"..myTaskCancel.." sè lÇn c¬ héi hñy bá nhiÖm vô, b¹n x¸c ®Şnh hñy bá nhiÖm vô lÇn nµy ®óng kh«ng?",
		2,
		"§óng, ta kh«ng muèn lµm nhiÖm vô quû qu¸i nµy ®©u/#Task_Cancel(1)",
		"Uhm! §Ó ta suy nghÜ l¹i ®·/Task_Wait");
	
end

end


-- Õı³£µÄÈ¡Ïû·½Ê½ÔÙÈ·ÈÏÒ»´Î
function Task_NormalCancel()

Say(" B¹n suy nghÜ kü hñy bá nhiÖm vô lÇn nµy ®óng kh«ng?",2,"§õng l«i th«i n÷a! ta kh«ng muèn lµm nhiÖm vô quû qu¸i nµy ®©u/#Task_Cancel(1)","Th«i ®Ó ta suy nghÜ l¹i ®·!/Task_Wait");

end;


-- 1000 ´ÎÒÔÉÏµÄÈÎÎñÈ¡ÏûÊ±ĞèÒª·ÅË®¾§
function Task_TotalCancel()
	GiveItemUI("Giao thñy tinh cho D· TÈu", "NÕu ng­¬i ®· quyÕt hñy bá chuçi nhiÖm vô, ta còng kh«ng ng¨n c¶n, h·y ®Æt vµo viªn Lôc Thñy Tinh!", "Task_TotalCancel_Main", "Task_Wait");
end;


function Task_TotalCancel_Main(nCount)
	
	local nGenre,nDetail,nParticular,nLevel,nGoodsFive,nLuck = 0,0,0,0,0,0;
	local nIndex = 0;
	
	if nCount~=1 then
		Say("D· TÈu: Ng­¬i ®­a g× cho ta vËy? Ta chØ cÇn <color=yellow>1 viªn Lôc Thñy Tinh<color> th«i!", 0);
		return
	end;
	
	nIndex = GetGiveItemUnit(1);
	nGenre,nDetail,nParticular,nLevel,nGoodsFive,nLuck = GetItemProp(nIndex);
	
	if nGenre==4 and nDetail==240 and nParticular==1 then
		Task_Cancel(1);  -- È«²¿È¡Ïû
	else
		Say("D· TÈu: Ng­¬i ®­a g× cho ta vËy? Ta chØ cÇn <color=yellow>1 viªn Lôc Thñy Tinh<color> th«i!", 0);
		return		
	end;
	
end;


-- ÏÔÊ¾Íæ¼Òµ±Ç°µÄÈÎÎñËù½øĞĞµÄ³Ì¶È£¨µ÷ÊÔÓÃ£©
function Task_ProcessInfo()

local myTaskTimes = tl_gettaskstate(1) -- Íæ¼Ò½øĞĞµ½µÄ´ÎÊı
local myTaskLinks = tl_gettaskstate(2) -- Íæ¼Ò½øĞĞµ½µÄÁ´Êı
local myTaskLoops = tl_gettaskstate(3) -- Íæ¼Ò½øĞĞµ½µÄ»·Êı
local myTaskCancel = tl_gettaskstate(4) -- Íæ¼Ò¿ÉÒÔÈ¡ÏûµÄ´ÎÊı

local myTaskType = tl_getplayertasktype()

local myTimes = tl_gettaskstate(1)
local myLinks = tl_gettaskstate(2)

local myCountTimes = tl_counttasklinknum(1)

-- local myTaskValue1 = tonumber(TabFile_GetCell(tl_gettasktextID(myTaskType),tl_gettasktablecol(),"TaskValue1"))
-- local myTaskValue2 = tonumber(TabFile_GetCell(tl_gettasktextID(myTaskType),tl_gettasktablecol(),"TaskValue2"))
	

-- local myMainValue = myTaskValue1 + (myTaskValue2 * (1+(myCountLinks+myTimes)*0.1))

-- local myMainValueText1 = "ÄãÄ¿Ç°µÄÈÎÎñÎïÆ·¼ÛÖµÎª: "..myTaskValue1.."  ÈÎÎñ¼ÛÖµÎª: "..myTaskValue2.."<enter>".."ÄãÏÖÔÚµÄÈÎÎñ×Ü¼ÛÖµÁ¿Îª: "..myMainValue

--	Say("Ò°ÛÅ£ºÄãÏÖÔÚ½øĞĞµ½ÁËµÚ "..myTaskLoops.." »·ÖĞµÄµÚ "..myTaskLinks.." Á´ÖĞµÄµÚ "..myTaskTimes.." ´Î¡£<enter>ÄãÁ¬Ğø½øĞĞµÄ´ÎÊıÎª£º"..tl_counttasklinknum(1).." ´Î<enter>ÄãÁ¬Ğø½øĞĞµÄÁ´ÊıÎª£º"..tl_counttasklinknum(2).." Á´<enter>"..myMainValueText1,0);

	Say(" HiÖn t¹i ng­êi ®· hoµn thµnh <color=yellow>"..myCountTimes.."<color> sè lÇn nhiÖm vô ta giao cho, cÇn cè g¾ng h¬n nhĞ!", 0);

end


-- Íæ¼ÒÒÑ¾­Íê³ÉÈÎÎñÇ°À´¸øÒ°ÛÅÑéÊÕ
function Task_Accept()
	--2007-09-19 Ôö¼ÓÎïÆ·½±ÀøÊ±µÄ±³°ü¿Õ¼äÅĞ¶Ï
	-- [PB 19/08] Moc set (1000/3000/6000/7000) phat toi 9 mon MOT LUC va chi ban DUNG
	-- MOT LAN tai dung con so do => tui day la MAT VINH VIEN. Chan tu khau nop.
	local nCanO = 5 + tl_getlinkawardslots(tl_counttasklinknum(1) + 1)
	if (CalcFreeItemCellCount() < nCanO) then
		Say("Hµnh trang ®· ®Çy, cÇn İt nhÊt"..nCanO.."« trèng - h·y s¾p xÕp l¹i cho ng¨n n¾p.",0);
		return
	end;
local myTaskType = tl_getplayertasktype()

	if (myTaskType == 1) then
		GiveItemUI("NhiÖm vô t×m vËt phÈm"," ña? Nh÷ng thø ta cÇn ng­¬i t×m ®­îc cho ta ch­a?","Task_Accept_01","Task_Wait");
	elseif (myTaskType == 2) then
		GiveItemUI("NhiÖm vô mua vËt phÈm"," ña? Nh÷ng thø ta cÇn ng­¬i t×m ®­îc cho ta ch­a?","Task_Accept_02","Task_Wait");
	elseif (myTaskType == 3) then
		GiveItemUI("NhiÖm vô hiÓn thŞ vËt phÈm"," ña? Nh÷ng thø ta cÇn ng­¬i t×m ®­îc cho ta ch­a?","Task_Accept_03","Task_Wait");
	elseif (myTaskType == 4) then
		Task_Accept_04()
	elseif (myTaskType == 5) then
		Task_Accept_05()
	elseif (myTaskType == 6) then
		Task_Accept_06()
	else -- Òì³£´¦Àí
		Say(" Nh÷ng thø ng­¬i giao cho ta ch­a ®¹t ®óng yªu cÇu, cè g¾ng h¬n nhĞ!",0);
	end

end


-- ÈÎÎñÒ»µÄÅĞ¶Ï´¦Àí
function Task_Accept_01(nCount)
local myTaskGoods
local ItemGenre,DetailType,ParticularType,Level,nSeries,Luck

if ( nCount > 1 ) then
	Say(" VŞ nµy"..GetPlayerSex()..", Ng­¬i bá nhiÒu ®å v« nh­ vËy xem tíi ta hoa c¶ m¾t, tõ tõ th«i nµo!",0);
	return 0
elseif ( nCount == 0) then
	Say(" VŞ nµy"..GetPlayerSex()..", ng­¬i cã thËt ®· bá vµo thø ta cÇn kh«ng? Kh«ng ph¶i l·o phu hoa m¾t chø?",0);
	return 0
end

	ItemGenre,DetailType,ParticularType,Level,nSeries,Luck = GetItemProp(GetGiveItemUnit(1))
--	magictype , p1, p2, p3 = GetItemMagicAttrib(nItemIndex, 1)
	myTaskGoods = {ItemGenre,DetailType,ParticularType,nSeries,Level}
	
--	tl_print ("Ò°ÛÅ¼ìÑéÁËÎïÆ·£º"..ItemGenre..DetailType..ParticularType..nSeries..Level)
	
	if (tl_checktask(myTaskGoods) == 1) then
		RemoveItemByIndex(GetGiveItemUnit(1)) -- É¾³ıÍæ¼ÒÉíÉÏµÄÎïÆ·
		-- [PB 20/08] Dong hop giao + tra do con lai ve tui.
		-- OnOk cua hop (UiAffairItem.cpp:176) CHI chay ham Lua, khong dong cua so
		-- va khong thu hoi do; chi OnCancel moi thu hoi. Loai 1/2 co xoa do nen o
		-- giao rong di, rieng loai 3 (Khoe - khong tieu do) bi ket mon trong hop.
		-- EndGiveBox -> CloseWindow -> OnCancel: dong hop VA tra do ve tui.
		EndGiveBox()
		Task_AwardRecord()
		Task_GiveAward()
		-- ·¢½±´¦Àí
	else
		Say(" Nh÷ng viÖc b¹n lµm ch­a ®¹t ®óng yªu cÇu, cè g¾ng h¬n nhĞ!",0);
	end
end


-- ÈÎÎñ¶şµÄÅĞ¶Ï´¦Àí
function Task_Accept_02(nCount)

local myTaskGoods
local ItemGenre, DetailType, ParticularType, Level, nSeries, Luck
local magictype,p1,p2,p3
local i,n,m = 0,0,0

if ( nCount > 1 ) then
	Say(" VŞ nµy"..GetPlayerSex()..", Ng­¬i bá nhiÒu ®å v« nh­ vËy xem tíi ta hoa c¶ m¾t, tõ tõ th«i nµo!",0);
	return 0
elseif ( nCount == 0) then
	Say(" VŞ nµy"..GetPlayerSex()..", ng­¬i cã thËt ®· bá vµo thø ta cÇn kh«ng? Kh«ng ph¶i l·o phu hoa m¾t chø?",0);
	return 0
end

	for i=1,6 do
		ItemGenre,DetailType,ParticularType,Level,nSeries,Luck = GetItemProp(GetGiveItemUnit(1))
		magictype , p1, p2, p3 = GetItemMagicAttrib(GetGiveItemUnit(1), i)
		myTaskGoods = {ItemGenre,DetailType,ParticularType,Level,nSeries,magictype,p1,p2,p3}
--		tl_print("¼ìÑéÁËÄ§·¨ÊôĞÔ "..i.." :".." Ä§·¨ ID Îª: "..magictype.."  Ä§·¨²ÎÊı1Îª: "..p1.."  Ä§·¨²ÎÊı2Îª: "..p2.." Ä§·¨²ÎÊı3Îª: "..p3);
		n = tl_checktask(myTaskGoods)
		if (n == 1) then
			m = 1
		end
	end
	
	if (m == 1) then
		RemoveItemByIndex(GetGiveItemUnit(1)) -- É¾³ıÍæ¼ÒÉíÉÏµÄÎïÆ·
		-- [PB 20/08] Dong hop giao + tra do con lai ve tui.
		-- OnOk cua hop (UiAffairItem.cpp:176) CHI chay ham Lua, khong dong cua so
		-- va khong thu hoi do; chi OnCancel moi thu hoi. Loai 1/2 co xoa do nen o
		-- giao rong di, rieng loai 3 (Khoe - khong tieu do) bi ket mon trong hop.
		-- EndGiveBox -> CloseWindow -> OnCancel: dong hop VA tra do ve tui.
		EndGiveBox()
		Task_AwardRecord()
		Task_GiveAward()
	else
		Say(" Nh÷ng viÖc b¹n lµm ch­a ®¹t ®óng yªu cÇu, cè g¾ng h¬n nhĞ!",0);
	end
	
end


-- ÈÎÎñÈıµÄÅĞ¶Ï´¦Àí
function Task_Accept_03(nCount)

local myTaskGoods
local magictype,p1,p2,p3
local i,n,m = 0,0,0

if ( nCount > 1 ) then
	Say(" VŞ nµy"..GetPlayerSex()..", Ng­¬i bá nhiÒu ®å v« nh­ vËy xem tíi ta hoa c¶ m¾t, tõ tõ th«i nµo!",0);
	return 0
elseif ( nCount == 0) then
	Say(" VŞ nµy"..GetPlayerSex()..", ng­¬i cã thËt ®· bá vµo thø ta cÇn kh«ng? Kh«ng ph¶i l·o phu hoa m¾t chø?",0);
	return 0
end

	for i=1,6 do -- Ñ­»·Àú±éËùÓĞµÄÄ§·¨ÊôĞÔ¿´¿´ÊÇ·ñÓĞºÏÊÊµÄ
		magictype,p1,p2,p3 = GetItemMagicAttrib(GetGiveItemUnit(1),i)
		myTaskGoods = {magictype,p1,p2,p3}
		n = tl_checktask(myTaskGoods)
		if (n == 1) then
			m = 1
		end
	end	

	if (m == 1) then
		-- [PB 20/08] Dong hop giao + tra do con lai ve tui.
		-- OnOk cua hop (UiAffairItem.cpp:176) CHI chay ham Lua, khong dong cua so
		-- va khong thu hoi do; chi OnCancel moi thu hoi. Loai 1/2 co xoa do nen o
		-- giao rong di, rieng loai 3 (Khoe - khong tieu do) bi ket mon trong hop.
		-- EndGiveBox -> CloseWindow -> OnCancel: dong hop VA tra do ve tui.
		EndGiveBox()
		Task_AwardRecord()
		Task_GiveAward()
		-- ·¢½±´¦Àí
	else
		Say(" Nh÷ng viÖc b¹n lµm ch­a ®¹t ®óng yªu cÇu, cè g¾ng h¬n nhĞ!",0);
	end
	
end


-- ÈÎÎñËÄµÄÅĞ¶Ï´¦Àí
function Task_Accept_04()

	if (tl_checktask() == 1) then
		Task_AwardRecord()
		Task_GiveAward()
		-- ·¢½±´¦Àí
	else
		Say(" Nh÷ng viÖc b¹n lµm ch­a ®¹t ®óng yªu cÇu, cè g¾ng h¬n nhĞ!",0);
	end
	
end


-- ÈÎÎñÎåµÄÅĞ¶Ï´¦Àí
function Task_Accept_05()

	if (tl_checktask() == 1) then
		Task_AwardRecord()
		Task_GiveAward()
		-- ·¢½±´¦Àí
	else
		Say(" Nh÷ng viÖc b¹n lµm ch­a ®¹t ®óng yªu cÇu, cè g¾ng h¬n nhĞ!",0);
	end
	
end



-- ÈÎÎñÁùµÄÅĞ¶Ï´¦Àí
function Task_Accept_06()
	
	if (tl_checktask()==1) then
		Task_AwardRecord()
		Task_GiveAward()
		return 1
	else
		Say(" Hahaha! VŞ nµy"..GetPlayerSex()..", ta tuy bÊt tµi, nh÷ng còng hiÓu ®­îc ch÷ tİn trªn giang hå, ng­¬i cßn ch­a thu thËp ®ñ m¶nh s¬n Hµ X· T¾c mµ ta yªu cÇu sao cã thÓ ®Õn l·nh th­ëng ®©y?",0);
		return 0
	end

end



-- Íæ¼ÒÑ¡ÔñÈ¡ÏûÈÎÎñµÄ´¦Àí
-- ´«Èë²ÎÊı nType£ºÈ¡ÏûÈÎÎñµÄ·½·¨£¬1ÎªÓÃÈ¡Ïû»ú»áÀ´È¡Ïû£¬2ÎªÓÃ 100 ÕÅÉ½ºÓÉçğ¢Í¼ËéÆ¬À´È¡Ïû
function Task_Cancel(nType)

local myTaskTimes = tl_gettaskstate(1) -- Íæ¼Ò½øĞĞµ½µÄ´ÎÊı
local myTaskLinks = tl_gettaskstate(2) -- Íæ¼Ò½øĞĞµ½µÄÁ´Êı
local myTaskLoops = tl_gettaskstate(3) -- Íæ¼Ò½øĞĞµ½µÄ»·Êı
local myTaskCancel = tl_gettaskstate(4) -- Íæ¼Ò¿ÉÒÔÈ¡ÏûµÄ´ÎÊı
local myCanceled = nt_getTask(1036) -- Íæ¼ÒÒÑ¾­¶ñÒâÈ¡ÏûÁË¶àÉÙ´Î
local myMapNum = nt_getTask(1027) -- ¿´¿´Íæ¼ÒÉíÉÏÓĞ¶àÉÙ¸öÉ½ºÓÉçğ¢Í¼²ĞÆ¬

local myNewCancel = GetTask(DEBUG_TASKVALUE);  -- ±¸·İµÄÈ¡Ïû»ú»á

local nTotalTaskNum = tl_counttasklinknum(1); -- »ñÈ¡µ±Ç°Íæ¼ÒÒ»¹²×öÁË¶àÉÙ´ÎÈÎÎñ

-- Èç¹û²»ÊÇ´ÓÖ÷Ãæ°æ½øÈëµÄ×´Ì¬£¬Ôò²»ÄÜ½øĞĞÒÔÏÂµÄÈ¡Ïû¹ı³Ì
if nt_getTask(1045)~=1 then
	return
end;

-- ÅĞ¶ÏÊÇ·ñ´æÔÚË¢È¡Ïû»ú»áµÄĞĞÎª
if _CancelTaskDebug()~=1 then
	Say("Uhm! B¹n trÎ nµy h×nh nh­ kh«ng cßn c¬ héi hñy bá ", 0);
	return
end;

-- ¼ì²âÊÇ·ñ³¬¹ıÃ¿ÌìµÄÏŞÖÆ
if checkTask_Limit()~=1 then return end;

	if (nType==2) then
		if (myMapNum>=100) then
			myMapNum = myMapNum - 100;
			nt_setTask(1027, myMapNum);
			myTaskCancel = myTaskCancel + 1;
			Msg2Player("B¹n ®· sö dông 100 m¶nh s¬n Hµ X· T¾c ®Ó hñy bá nhiÖm vô nµy!");
			Msg2Player("M¶nh s¬n Hµ X· T¾c hiÖn t¹i cña b¹n cßn d­ "..myMapNum.." TÊm!");
		else
			Say(" ng­¬i cã ®óng ®· mang <color=yellow>100<color> m¶nh s¬n Hµ X· T¾c kh«ng? Ta cã nh×n lÇm kh«ng vËy?",0);
			return
		end;
	end;

	if (myTaskTimes == 0) and (myTaskLinks == tl_getfirstlink()) then
	
		if (myTaskCancel == 0) then
		
			myCanceled = myCanceled + 1
			nt_setTask(1036,myCanceled)
	
			if ( myCanceled > 2) then -- Èç¹ûÍæ¼ÒÁ¬ĞøÔÚ³õÆÚÈ¡ÏûÁËÈı´ÎÈÎÎñ£¬Ôò×÷´¦·£
				myCanceled = 10	
				nt_setTask(1036,myCanceled)
				nt_setTask(1029,GetGameTime())
				Task_Punish()
				
				return
				
			end
			
		end
		
		-- ÖØĞÂËæ»úÅÉ·¢ĞÂµÄÈÎÎñ
		SetTask(ID_TASKLINK_LIMITCancelCount, GetTask(ID_TASKLINK_LIMITCancelCount) + 1); -- Ôö¼Óµ±ÌìµÄÈ¡Ïû´ÎÊı
		storm_ask2start(4)	--Storm ¿ªÊ¼ÌôÕ½
		
		return
		
	end

	if (myTaskCancel >= 1) then -- Èç¹ûÍæ¼Ò»¹ÓĞ»ú»áÈ¡ÏûµÄ»°Ôò¼ÌĞøËæ»úµ±Ç°µÈ¼¶µÄÈÎÎñ
	
		-- Ğ´ÈëÈ¡ÏûÈÎÎñÊ±µÄ LOG
		_WriteCancelLog(nType, nTotalTaskNum, myTaskCancel);
		
		myTaskCancel = myTaskCancel - 1
		
		tl_settaskstate(4,myTaskCancel)
		
		nt_setTask(DEBUG_TASKVALUE, myTaskCancel);
			
	else
		-- Ğ´ÈëÈ¡ÏûÈÎÎñÊ±µÄ LOG
		_WriteCancelLog(nType, nTotalTaskNum, myTaskCancel);
	
		tl_settaskstate(1,0) -- ÕıÔÚ½øĞĞµÚÒ»´ÎÈÎÎñ
		tl_settaskstate(2,tl_getfirstlink()) -- ÕıÔÚ½øĞĞµÚÒ»Á´ÈÎÎñ
		tl_settaskstate(3,0) -- ´ÓµÚ 0 »·¿ªÊ¼
		
		tl_settaskstate(4,0) -- Ê£Óà 0 ´ÎÈ¡ÏûÈÎÎñµÄ»ú»á
		nt_setTask(DEBUG_TASKVALUE, 0);
		
		tl_settaskstate(6,0) -- µ±Ç°µÄÁ´½øĞĞÁË 0 ´Î
		nt_setTask(1036,0) -- ³Í·£´ÎÊıÀÛ»ı±äÎª 0 
		-- ÔÚÕâÀï¼ÇÂ¼Ò»ÏÂÈÎÎñµÄ×ÜÊı
		nt_setTask(1044, tl_counttasklinknum(1));
		
		Msg2Player("<color=yellow>Chuçi nhiÖm vô D· TÈu ®· xãa bá hoµn toµn, b©y giê sÏ ph¶i lµm l¹i tõ nhiÖm vô ®Çu tiªn<color>!");
	end
	
	-- ÉèÖÃÈ¡ÏûÈÎÎñµÄ±ê¼ÇÎª²»¿ÉÒÔÈ¡Ïû
	nt_setTask(1045, 2);
	
	-- ÖØĞÂËæ»úÅÉ·¢ĞÂµÄÈÎÎñ
	SetTask(ID_TASKLINK_LIMITCancelCount, GetTask(ID_TASKLINK_LIMITCancelCount) + 1); -- Ôö¼Óµ±ÌìµÄÈ¡Ïû´ÎÊı
	storm_ask2start(4)	--Storm ¿ªÊ¼ÌôÕ½
end;


-- ÌıÒ°ÛÅ½âÊÍÕû¸öÈÎÎñÁ´µÄ¹ı³ÌÓë·½·¨
function Task_Info()
	Talk(4,
		"tasklink_entence",
		" LÇn nµy ta ®· s¾p xÕp c¸c lo¹i kh¶o nghiÖm nhiÖm vô nhá ®Ó thö th¸ch vâ thuËt còng nh­ tİnh kiªn nhÉn cña c¸c vŞ anh hïng hµo kiÖt ®©y.",
		" Mçi lÇn hoµn thµnh nhiÖm vô ®Òu cã <color=red>phÇn th­ëng t­¬ng øng<color> cho c¸c vŞ, nÕu cã thÓ liªn tôc hoµn thµnh nhiÖm vô ®­îc ®Æt ra th× cã ®­îc <color=red>lÔ vËt tèt h¬n<color>d©ng tÆng riªng, nÕu nh­ ai ®ã cã thÓ ®¹t ®Õn <color=red>8000<color> nhiÖm vô sÏ nhËn ®­îc <color=red>sù ®Òn ®¸p ®Õn bÊt ngê<color>!",
		" NÕu nh­ ng­¬i bá qua mét nhiÖm vô trong sè nhiÖm vô th× tİnh l¹i tõ nhiÖm vô ban ®Çu. Nh­ng mµ, ta cã lóc còng cho c¬ héi hñy bá nhiÖm vô, ®Ó xem ng­¬i n¾m b¾t ra sao th«i!",
		" Sao råi? Ng­êi thanh niªn cã muèn tiÕp nhËn sù kh¶o nghiÖm cña ta kh«ng?"
		);
end


-- ¾Ü¾øÁËÒ°ÛÅµÄ¿¼ÑéÌáÒé
function Task_Exit()

	if (GetSex() == 0) then
		Say(" Hahaha! §îi sau khi vŞ ®¹i hiÖp nµy th«ng qua kh¶o nghiÖm xong nhËn ®­îc c¸c phÇn th­ëng phong phó, tù nhiªn l¹i biÕt ®Õn t×m ta th«i!",0);
	else
		Say(" Hahaha! §îi sau khi vŞ n÷ hiÖp nµy th«ng qua kh¶o nghiÖm xong nhËn ®­îc c¸c phÇn th­ëng phong phó, tù nhiªn l¹i biÕt ®Õn t×m ta th«i!",0);
	end

end


-- ÔÚ×öÈÎÎñµÄ¹ı³ÌÖĞÍË³öÒ°ÛÅµÄ¶Ô»°£¬²»×÷ÈÎºÎ´¦Àí
function Task_Wait()

end


function Task_Punish()
	Say(" VŞ nµy"..GetPlayerSex().."Cã ph¶i gÊp l¾m kh«ng, kh¶o nghiÖm cña ta s¾p xÕp khã ®Õn nh­ vËy ­? LÇn sau ®Õn vËy!",0);
	return 0
end



--  //////////////////////////////////////////////////////////////
-- //                     ·¢Óè½±ÀøµÄ´¦Àí                       //
--//////////////////////////////////////////////////////////////

function Task_GiveAward()

	local i
	
	local myAward
	local myLinkAward,myLoopAward
	
	local myGoodsText = ""
	local ShowText = {"","",""}

	local nTotalTaskNum = tl_counttasklinknum(1); 

	-- [PB 19/08] Chan truoc khi mo cua so thuong: phai du o cho CA phan thuong
	-- chon o cua so LAN phan thuong moc phat ngay sau do (PayPlayerLinkAward).
	local nCanO = 5 + tl_getlinkawardslots(nTotalTaskNum)
	if (CalcFreeItemCellCount() < nCanO) then
		Say("Hµnh trang ®· ®Çy, cÇn İt nhÊt"..nCanO.."« trèng míi nhËn ®­îc phÇn th­ëng - h·y dän tói råi nãi chuyÖn l¹i.",0);
		return
	end
-- »ñÈ¡µ±Ç°Íæ¼ÒÒ»¹²×öÁË¶àÉÙ´ÎÈÎÎñ
	-- [JX1 PORT 15/08/2026] item goc 6/1/2374 ben JX2 = "Bao ruong than bi cua Da Tau";
	-- 6/1/2374 ben JX1 lai la "Bang bach kim" (vat lieu quy) -> phat nham la su co kinh te.
	-- TAM KHOA moc-10 bang co DATAU_MOC10_BAT; chu game chon item ruong roi thay id + dat DATAU_MOC10_BAT = 1.
	-- [16/08/2026 chu game chot] moc moi-10-nhiem-vu = 3x Boss Trieu Hoan Phu (6/1/1023).
	-- (goc Linux phat 1 ruong 2374; ruong do = item 2383 ben JX1, gio la thuong moc-40)
	if (nTotalTaskNum ~= 0 and mod(nTotalTaskNum, 10) == 0 and GetTask(TKS_TASKLINK_SPITEM) ~= nTotalTaskNum) then
		-- [PB 17/08] tui day thi KHONG set co + khong phat -> nguoi choi don tui
		-- roi gap lai NPC van nhan duoc (co chi set khi phat that)
		if (CalcFreeItemCellCount() < 3) then
			Msg2Player("Hanh trang khong du 3 o trong de nhan thuong moc 10 nhiem vu, hay don tui roi noi chuyen lai voi Da Tau!");
			return
		end
		SetTask(TKS_TASKLINK_SPITEM, nTotalTaskNum);
		local tbItem = {tbProp = {6, 1, 1023, 1, 0, 0}, nCount = 3}
		tbAwardTemplet:GiveAwardByList(tbItem, "seasonnpc_10task")
		Msg2Player(format("Chóc mõng ®¹i hiÖp ®· hoµn thµnh liªn tiÕp %d nhiÖm vô D· TÈu, nhËn ®­îc phÇn th­ëng %s!", 10, "3 Boss TriÖu Ho¸n Phï"));
	end

	--tl_print ("¸øÁ´½±ÀøºÍ»·½±Àø·¢½±Íê±Ï£¡£¡£¡");
	local nTongValue;
	myAward, nTongValue = tl_giveplayeraward(1);
	if (not nTongValue) then
		nTongValue = 0;
	end	
	local nBeishu = greatnight_huang_event(4);
	if (nBeishu > 0) then
		nTongValue = floor(nTongValue / nBeishu);
	end;
	
	for i=1,3 do
	--	tl_print ("µÃµ½½ğÇ®½±Àø£¡");
		if (myAward[i][1] == 1) then
			ShowText[i] = "NhËn ®­îc"..myAward[i][9].."/3".."/"..myAward[i][2].."/SelectAward_Money"
		elseif (myAward[i][1] == 2) then
			ShowText[i] = "NhËn ®­îc"..myAward[i][9].."/4".."/"..myAward[i][2].."/SelectAward_Exp"
		elseif (myAward[i][1] == 3) then
			myGoodsText = myAward[i][3]..","..myAward[i][4]..","..myAward[i][5]..","..myAward[i][6]..","..myAward[i][7]..","..myAward[i][8]
			ShowText[i] = "NhËn ®­îc"..myAward[i][9].."/5".."/"..myGoodsText.."/mySG"
		elseif (myAward[i][1] == 4) then
			if (myAward[i][10]==1) then
				myGoodsText = myAward[i][3]..","..myAward[i][4]..","..myAward[i][5]..","..myAward[i][6]..","..myAward[i][7]..","..myAward[i][8]
				ShowText[i] = myAward[i][9].."/6".."/"..myGoodsText.."/mySG"
			elseif (myAward[i][10]==2) then
				ShowText[i] = myAward[i][9].."/6".."/"..myAward[i][2].."/SelectAward_Exp"
			elseif (myAward[i][10]==3) then
				ShowText[i] = myAward[i][9].."/6".."/"..myAward[i][2].."/SelectAward_Money"
			end
			
		elseif (myAward[i][1] == 5) then
			ShowText[i] = "NhËn ®­îc"..myAward[i][9].."/7".."/"..myAward[i][1].."/SelectAward_Cancel"
		end
		
	end
	
	tl_print(ShowText[1])
	tl_print(ShowText[2])
	tl_print(ShowText[3])
	
	Prise( "Ng­¬i vÊt v¶ qu¸, xin mêi vŞ "..GetPlayerSex().."Chän mãn m×nh thİch ®i!",ShowText[1],ShowText[2],ShowText[3] );
	
	--tl_print ("·¢½±Íê±Ï£¡£¡£¡£¡£¡");

end



-- ÔÚÒÑ¾­È·ÈÏÁËÈÎÎñÍê³Éµ«ÊÇ»¹Î´·¢½±Ê±µÄ±äÁ¿´¦Àí£¬ÒÔ·ÀÍæ¼ÒË¢½±
function Task_AwardRecord()
	--Storm ¼Ó»ı·Ö
	local filename = tl_gettasktextID(tl_getplayertasktype())
	local tabcol = tl_gettasktablecol()
	local myTaskValue1 = tonumber(TabFile_GetCell(filename,tabcol,"TaskValue1"))
	local myTaskValue2 = tonumber(TabFile_GetCell(filename,tabcol,"TaskValue2"))
	local myMainValue = myTaskValue1 + myTaskValue2
	storm_addpoint(4, myMainValue)
	
	tl_settaskcourse(2) -- ÈÎÎñµÄ½øÕ¹³Ì¶ÈÎª2£¬¼´ÊÇ»¹Ã»ÓĞ·¢½±
	nt_setTask(1037, GetGameTime() + tonumber(GetLocalDate("%H%M%S")));
end



-- Ñ¡ÔñÁËÎïÆ·µÄ½±Àø
-- PS£ºÒòÎª¿¼ÂÇµ½ÈÎÎñÃæ°æµÄ´«Èë×Ö·û²»ÄÜ³¬¹ı 32 ¸ö£¬ËùÒÔÕâ¸ö¹ı³Ì¼òĞ´³É mySG
function mySG(myQuality,myGenre,myDetail,myParticular,myLevel,myFive)

-- ·ÀÖ¹¶à¸öÃæ°æµ¯³öÀ´Ë¢½±ÀøµÄ´¦Àí
if (tl_gettaskcourse() == 3) then
	Say("Ng­¬i ®· l·nh th­ëng råi! §Şnh g¹t giµ nµy µ?",0);
	return
end
	--2007-09-19 Ôö¼ÓÎïÆ·½±ÀøÊ±µÄ±³°ü¿Õ¼äÅĞ¶Ï
	-- [PB 19/08] Con phai du cho ca phan thuong moc phat ngay sau day (PayPlayerLinkAward).
	local nCanO = 5 + tl_getlinkawardslots(tl_counttasklinknum(1))
	if (CalcFreeItemCellCount() < nCanO) then
		Say("Hµnh trang ®· ®Çy, cÇn İt nhÊt"..nCanO.."« trèng - h·y s¾p xÕp l¹i cho ng¨n n¾p.",0);
		return
	end;
	
--	tl_print("µÃµ½ÁË½±ÀøÎïÆ·±àºÅ£º"..myQuality..myGenre..myDetail..myParticular..myLevel..myGoodsFive);
	local nItemIndex = 0
	if (myQuality == 0) then
		nItemIndex =  AddItem(myGenre,myDetail,myParticular,myLevel,myFive,0,0)
		if (GetProductRegion() == "vn") then
			-- LLG_ALLINONE_TODO_20070802
			if (myDetail==238) or (myDetail==239) or (myDetail==240) or (myDetail==252) then
				WriteLog(" [Ghi nhí nhËn phÇn th­ëng]"..date(" [%y n¨m %m th¸ng %d ngµy  %H giê %M phót]")..": Tµi kho¶n"..GetAccount()..", nh©n vËt"..GetName().."Trong nhiÖm vô liªn tiÕp nhËn ®­îc phÇn th­ëng lµ b¶o th¹ch, sè thø tù lo¹i b¶o th¹ch lµ:"..myDetail)
			elseif (myGenre == 6 and myDetail == 1 and myParticular ==1475) then
				local nLastTime = FormatTime2Number(GetCurServerTime() + 7*24*60*60);
				local nYear = floor(nLastTime / 100000000);
				local nMMDD = mod(floor(nLastTime / 10000) , 10000);
				SetItemMagicLevel(nItemIndex, 1, GetTask(TSK_TASKLINK_SEANSONPOINT));
				SetItemMagicLevel(nItemIndex, 2, nYear);
				SetItemMagicLevel(nItemIndex, 3, nMMDD);
				SyncItem(nItemIndex);
				WriteLog(format("[Log phÇn th­ëng chuçi nhiÖm vô D· TÈu]%s: Tµi kho¶n %s, nh©n vËt %s hoµn thµnh chuçi nhiÖm vô D· TÈu nhËn ®­îc ®iÓm Tİch lòy D· TÈu (®iÓm kinh nghiÖm: %d)",
					date(" [%y n¨m %m th¸ng %d ngµy  %H giê %M phót]"),
					GetAccount(),
					GetName(),
					GetTask(TSK_TASKLINK_SEANSONPOINT)
				))
				SetTask(TSK_TASKLINK_SEANSONPOINT, 0);
			end
		else
			WriteLog(" [Ghi nhí nhËn phÇn th­ëng]"..date(" [%y n¨m %m th¸ng %d ngµy  %H giê %M phót]").." [mËt m·:"..GetAccount().."] [nh©n vËt:"..GetName().."]".."NhËn ®­îc 1 "..szName);
		end
	else
		nItemIndex = AddItem2(2, 0, myGenre, 0, 0, 0)
		AddGlobalNews("Ng­êi ch¬i "..GetName().." ®· hoµn thµnh nhiÖm vô D· TÈu nªn ®· nhËn ®­îc 1 trang bŞ Hoµng Kim!!!");
		WriteLog(" [Ghi nhí nhËn phÇn th­ëng]"..date(" [%y n¨m %m th¸ng %d ngµy  %H giê %M phót]")..": Tµi kho¶n"..GetAccount()..", nh©n vËt"..GetName().."Trong nhiÖm vô liªn tiÕp nhËn ®­îc phÇn th­ëng lµ trang bŞ Hoµng Kim sè thø tù lµ:"..myGenre)
	end
	Msg2Player("B¹n nhËn ®­îc mét phÇn th­ëng nhiÖm vô!");
	
	-- [PB 19/08] bao cho ca map biet (chu game: "cho nhieu nguoi thay")
	tl_thongbao_vatpham(nItemIndex)
	tl_settaskcourse(3)
	PayPlayerLinkAward();
	
	local nBeishu = greatnight_huang_event(4);
	local nTongValueGift = GetTaskTemp(TASKID_TONG_TASKLINKTEMP);
	if (nBeishu > 0) then
		nTongValueGift = floor(nTongValueGift / nBeishu);
	end;
	tongaward_tasklink(nTongValueGift);	--¼ÓÉÏÃÅÅÉ½±Àø
	Task_NewVersionAward()
end

-- Ñ¡ÔñÁË½ğÇ®µÄ½±Àø
function SelectAward_Money(nAward)

-- ·ÀÖ¹¶à¸öÃæ°æµ¯³öÀ´Ë¢½±ÀøµÄ´¦Àí
if (tl_gettaskcourse() == 3) then
	Say("Ng­¬i ®· l·nh th­ëng råi! §Şnh g¹t giµ nµy µ?",0);
	return
end

	Earn(nAward)
	Msg2Player("B¹n nhËn ®­îc <color=green>"..nAward.."<color> l­îng  b¹c");
	
	tl_settaskcourse(3)	
	PayPlayerLinkAward();
	
	-- Èç¹û½±ÀøµÄ½ğÇ®ÊıÁ¿´óÓÚ 30W£¬Ôò¼ÇLOG
	if nAward>=300000 then
		WriteLog(" [Ghi nhí chuçi nhiÖm vô]"..
				 date(" [%y n¨m %m th¸ng %d ngµy  %H giê %M phót]")..
				 " [Tµi kho¶n]"..GetAccount()..
				 " [Nh©n vËt]"..GetName()..
				 "Trong nhiÖm vô liªn tiÕp nhËn ®­îc<money>"..nAward.."</money>phÇn th­ëng lµ l­îng b¹c!");
	end;
	local nBeishu = greatnight_huang_event(4);
	local nTongValueGift = GetTaskTemp(TASKID_TONG_TASKLINKTEMP);
	if (nBeishu > 0) then
		nTongValueGift = floor(nTongValueGift / nBeishu);
	end;
	tongaward_tasklink(nTongValueGift);	--¼ÓÉÏÃÅÅÉ½±Àø
	Task_NewVersionAward()
end

-- Ñ¡ÔñÁË¾­ÑéÖµµÄ½±Àø
function SelectAward_Exp(nAward)

-- ·ÀÖ¹¶à¸öÃæ°æµ¯³öÀ´Ë¢½±ÀøµÄ´¦Àí
if (tl_gettaskcourse() == 3) then
	Say("Ng­¬i ®· l·nh th­ëng råi! §Şnh g¹t giµ nµy µ?",0);
	return
end

	tl_addPlayerExp(nAward)
	Msg2Player("B¹n nhËn ®­îc <color=green>"..nAward.."<color> ®iÓm kinh nghiÖm");
	
	tl_settaskcourse(3)
	PayPlayerLinkAward();
	local nBeishu = greatnight_huang_event(4);
	local nTongValueGift = GetTaskTemp(TASKID_TONG_TASKLINKTEMP);
	if (nBeishu > 0) then
		nTongValueGift = floor(nTongValueGift / nBeishu);
	end;
	tongaward_tasklink(nTongValueGift);	--¼ÓÉÏÃÅÅÉ½±Àø
	Task_NewVersionAward()
end

-- Ñ¡ÔñÔÙËæ»úÒ»´ÎµÄ½±Àø
function SelectAward_Change(nAward)

-- ·ÀÖ¹¶à¸öÃæ°æµ¯³öÀ´Ë¢½±ÀøµÄ´¦Àí
if (tl_gettaskcourse() == 3) then
	Say("Ng­¬i ®· l·nh th­ëng råi! §Şnh g¹t giµ nµy µ?",0);
	return
end

	Msg2Player("B¹n cã thªm c¬ héi nhËn ®­îc mét phÇn th­ëng ngÉu nhiªn!");
	
	tl_settaskcourse(3)
	PayPlayerLinkAward();

end

-- Ñ¡ÔñÁËÈ¡ÏûµÄ»ú»á
function SelectAward_Cancel(nAward)

local myCancel = 0;
local myNewCancel = GetTask(DEBUG_TASKVALUE);

	-- ·ÀÖ¹¶à¸öÃæ°æµ¯³öÀ´Ë¢½±ÀøµÄ´¦Àí
	if (tl_gettaskcourse() == 3) then
		Say("Ng­¬i ®· l·nh th­ëng råi! §Şnh g¹t giµ nµy µ?",0);
		return
	end


	
	myCancel = tl_gettaskstate(4)
	myCancel = myCancel + 1
	
	if myNewCancel==0 then
		myNewCancel = myCancel;
	else
		myNewCancel = myNewCancel + 1;		
	end;
	
	if myNewCancel<=254 then
		-- ÔÙ´æÒ»´ÎÁíÍâµÄ±äÁ¿
		nt_setTask(DEBUG_TASKVALUE, myNewCancel);
	end;
	
	tl_settaskstate(4, myCancel);
	
	if myCancel<=254 then
		Msg2Player("B¹n nhËn ®­îc <color=green>1 c¬ héi hñy bá nhiÖm vô <color>!");
	end;

	WriteLog(" [Ghi nhí nhËn phÇn th­ëng]"..
			 date(" [%yn¨m%mth¸ng%dngµy%Hgiê%Mphót%Sgi©y]")..
			 " [Tµi kho¶n]"..GetAccount()..
			 " [Nh©n vËt]"..GetName()..
			 "B¹n nhËn ®­îc c¬ héi hñy bá nhiÖm vô, hiÖn t¹i cßn cã thÓ c¬ héi hñy <"..myCancel.."> lÇn.");
	
	tl_settaskcourse(3)	
	PayPlayerLinkAward();
	
	local nBeishu = greatnight_huang_event(4);
	local nTongValueGift = GetTaskTemp(TASKID_TONG_TASKLINKTEMP);
	if (nBeishu > 0) then
		nTongValueGift = floor(nTongValueGift / nBeishu);
	end;
	tongaward_tasklink(nTongValueGift);	--¼ÓÉÏÃÅÅÉ½±Àø
	Task_NewVersionAward()
end


-- ·¢¸øÍæ¼ÒÁ´½±Àø¡¢»·½±Àø»òÕß»Æ½ğ×°±¸
function PayPlayerLinkAward()

local myGolden = {2,6,11,16,21,22,26,31,39,40,42,46,51,54,56,60,61,67,71,76,81,87,92,94,96,101,106,107,115,119,121,122,126,132,136,144,145,146} -- 20 »·½±Àø»Æ½ğ×°±¸µÄ±àºÅ
local nGoldenID = 0

local myTaskTimes = tl_gettaskstate(1)
local myTaskLinks = tl_gettaskstate(2)
local myTaskLoops = tl_gettaskstate(3)

local nTotalTask = tl_counttasklinknum(1);

-- ÉèÖÃÈÎÎñÍê³É´ÎÊıÅÅÃû
Ladder_NewLadder(10118, GetName(), nTotalTask, 1);

-- ÔÚÕâÀï·¢Óè¹Ì¶¨ÈÎÎñ´ÎÊıµÄ½±Àø
tl_getlinkaward(Task_AwardLink, nTotalTask);

-- Ô½ÄÏ°æÍê³É 8000 ´ÎÈÎÎñ²»×öÈÎºÎ´¦Àí
if (nTotalTask == 8000) then

--	nGoldenID = myGolden[random(getn(myGolden))]
	
--	-- Âú 8000 ´ÎÈÎÎñ½±Àø 1E ½ğÇ®
--	Earn(100000000);
	
--	AddGoldItem( 0, nGoldenID )
	
--	WriteLog("[ÈÎÎñÁ´½±Àø¼ÇÂ¼]"..date("[%yÄê%mÔÂ%dÈÕ%HÊ±%M·Ö]").."£ºÕËºÅ"..GetAccount().."£¬½ÇÉ«"..GetName().."ÔÚÈÎÎñÁ´½±ÀøÖĞÒòÎªÍê³É 8000 ´ÎÈÎÎñµÃµ½ÁË»Æ½ğ×°±¸Ò»¼ş£¬»Æ½ğ×°±¸±àºÅÎª£º"..nGoldenID)
	
--	AddGlobalCountNews("¹«¸æ£ºÍæ¼Ò "..GetName().." ÒòÎªÍê³ÉÁË 8000 ´ÎÈÎÎñÔÚÒ°ÛÅ´¦µÃµ½ÁËÃÅÅÉ´ó»Æ½ğ×°±¸Ò»¼şºÍ½£ÏÀ±ÒÒ»ÒÚÁ½£¡£¡£¡", 3);
	
--	for i=1,3 do
--		Msg2Player("¹§Ï²Äã£¡£¡ÒòÎªÄãÁ¬ĞøÍê³ÉÁË 8000 ´ÎÈÎÎñËùÒÔµÃµ½ÁËÒ»¸ö¼«Æ·½±ÀøºÍÒ»ÒÚÁ½½£ÏÀ±Ò£¡£¡£¡");
--	end

	return
	
end

storm_ask2start(4, 1)	--Storm ¿ªÊ¼ÌôÕ½

end


--/////ÈÎÎñÁ´µÄÎŞ·¨½øĞĞÏÂÈ¥µÄ BUG µÄĞŞ²¹/////

function _TaskLinkDebug()

	if (nt_getTask(1030)==0) and (tl_gettaskcourse()==1) then
		nt_setTask(1030,2);
		tl_savetalkvalue(1,2);
		tl_savetalkvalue(2,2);
		tl_savetalkvalue(3,2);
		tl_savetalkvalue(4,2);
		tl_savetalkvalue(5,2);
		tl_savetalkvalue(6,2);
	end

end


--  ////////////////////////////////////////////////
-- ///// ÅĞ¶ÏÍæ¼ÒÊÇ·ñË¢È¡Ïû»ú»áµÄ¹ı³Ì /////////////
--////////////////////////////////////////////////

function _CancelTaskDebug()

local nOrgTask = GetTask(DEBUG_TASKVALUE);
local nDecTask = tl_gettaskstate(4);
local nDiff    = 0;

	if nOrgTask==nDecTask or nOrgTask==0 then
		return 1;
	else
		WriteLog(" [B¶ng nhiÖm vô ®· ghi nhí sai]"..
				 date(" [%yn¨m%mth¸ng%dngµy%Hgiê%Mphót%Sgi©y]")..
				 " [Tµi kho¶n]"..GetAccount()..
				 " [Nh©n vËt]"..GetName()..
				 "C¬ héi hñy bá kh«ng c©n b»ng, hiÖn t¹i c¬ héi hñy bá cßn <"..nDecTask.."> lÇn, phÇn c¬ héi hñy bá cßn <"..nOrgTask..">.");
		return 0;
	end;

end;

--  /////Êä³öÈÎÎñÈ¡ÏûÊ±µÄ LOG //////////////////////
-- ///// ´«ÈëµÄ²ÎÊıÎªÈÎÎñ´ÎÊıºÍÈ¡ÏûÈÎÎñµÄ»ú»á /////
--////////////////////////////////////////////////

function _WriteCancelLog(nType, nTime, nCancel)

	if nType==1 then
		WriteLog(" [Hñy bá kû lôc nhiÖm vô liªn tiÕp]"..date(" [%y n¨m %m th¸ng %d ngµy  %H giê %M phót]")..": Tµi kho¶n"..GetAccount()..", nh©n vËt"..GetName().."T¹i nhiÖm vô thø "..nTime.."Hñy bá c¬ héi cßn d­ "..nCancel.."giê hñy bá mét lÇn nhiÖm vô.");
	else
		WriteLog(" [Hñy bá kû lôc nhiÖm vô liªn tiÕp]"..date(" [%y n¨m %m th¸ng %d ngµy  %H giê %M phót]")..": Tµi kho¶n"..GetAccount()..", nh©n vËt"..GetName().."T¹i nhiÖm vô thø "..nTime.." dïng 100 tÊm s¬n Hµ X· T¾c hñy bá nhiÖm vô.");
	end;

end;



-- [JX1 PORT] spawn Ba Lang Huyen co SetNpcTimer (tinh nang NPC noi chuyen cua he cu);
-- ban goc khong co OnTimer -> them rong de khoi ScriptError moi lan timer no.
function OnTimer()
end;


-- ============================================================================
-- [JX1 PORT 16/08/2026] CUA SO THUONG 3 RUONG SPR CO SAN CUA JX1
-- Client co 2 cua so (GameSpaceChangedNotify.cpp:561):
--   nType <= 4 -> KUiDaTau  : nut [Exp][Money][Random] -> finish_exp/finish_money/quest_random
--   nType >  4 -> KUiDaTau1 : nut [Point][Lucky][Item] -> finish_point/finish_lucky/finish_item
-- Server dinh tuyen nut bam ve DUNG state hoi thoai nay (KProtocolProcess.cpp
-- case 3/4 + OpenQuestFinish luu m_ActionScriptID). Vi vay dinh nghia Prise
-- o day (de len ham C cung ten) de dung UI SPR thay hop thoai chu.
-- Kinh te GIU NGUYEN: 3 phan thuong van boc bang cong thuc tasklink goc;
-- nut bam chi goi lai dung ham goc (SelectAward_* / mySG) voi dung tham so.
-- Bo 3 loai luon vua 1 cua so tru toi da 1 nut mang icon xap xi (phan
-- thuong nhan duoc van dung nhu da boc).
-- ============================================================================

tbPriseMap = tbPriseMap or {}

-- tach "nhan/icon/thamso/TenHam": lay 2 truong cuoi (thamso, TenHam)
function Prise_Parse(szOpt)
	local nLast = nil
	local nPrev = nil
	local nPos = 0
	while 1 do
		local nS = strfind(szOpt, "/", nPos + 1, 1)
		if not nS then
			break
		end
		nPrev = nLast
		nLast = nS
		nPos = nS
	end
	if not nLast or not nPrev then
		return nil
	end
	return strsub(szOpt, nLast + 1), strsub(szOpt, nPrev + 1, nLast - 1)
end

-- "a,b,c,..." -> bang so (toi da 6, thieu = 0)
function Prise_Params(szParam)
	local tbNum = {}
	local nPos = 0
	while getn(tbNum) < 6 do
		local nS = strfind(szParam, ",", nPos + 1, 1)
		local szTok
		if nS then
			szTok = strsub(szParam, nPos + 1, nS - 1)
			nPos = nS
		else
			szTok = strsub(szParam, nPos + 1)
		end
		tinsert(tbNum, tonumber(szTok) or 0)
		if not nS then
			break
		end
	end
	while getn(tbNum) < 6 do
		tinsert(tbNum, 0)
	end
	return tbNum
end

function Prise(szMsg, szOpt1, szOpt2, szOpt3)
	local tbOpt = {szOpt1, szOpt2, szOpt3}
	local tbAward = {}
	local i, k
	for i = 1, 3 do
		if tbOpt[i] and tbOpt[i] ~= "" then
			local szFn, szParam = Prise_Parse(tbOpt[i])
			if szFn then
				local szNut = ""
				local nNhomA = 0
				if szFn == "SelectAward_Exp" then
					szNut = "finish_exp"
					nNhomA = 1
				elseif szFn == "SelectAward_Money" then
					szNut = "finish_money"
					nNhomA = 1
				elseif szFn == "SelectAward_Change" then
					szNut = "quest_random"
					nNhomA = 1
				elseif szFn == "mySG" then
					szNut = "finish_item"
				elseif szFn == "SelectAward_Cancel" then
					szNut = "finish_lucky"
				end
				tinsert(tbAward, {szFn = szFn, tbP = Prise_Params(szParam), szNut = szNut, nNhomA = nNhomA})
			end
		end
	end
	if getn(tbAward) == 0 then
		return
	end
	-- chon cua so theo da so nhom (3 loai phan biet -> toi da 1 nut lech icon)
	local nA = 0
	for i = 1, getn(tbAward) do
		nA = nA + tbAward[i].nNhomA
	end
	local nType, tbNut
	if nA >= 2 then
		nType = 1
		tbNut = {"finish_exp", "finish_money", "quest_random"}
	else
		nType = 5
		tbNut = {"finish_point", "finish_lucky", "finish_item"}
	end
	-- gan nut: uu tien nut dung loai, phan thua vao nut trong
	local tbMap = {}
	local tbConLai = {}
	for i = 1, getn(tbAward) do
		local tbA = tbAward[i]
		local bTuNhien = 0
		for k = 1, 3 do
			if tbNut[k] == tbA.szNut and not tbMap[tbA.szNut] then
				bTuNhien = 1
			end
		end
		if bTuNhien == 1 then
			tbMap[tbA.szNut] = tbA
		else
			tinsert(tbConLai, tbA)
		end
	end
	for i = 1, getn(tbConLai) do
		for k = 1, 3 do
			if not tbMap[tbNut[k]] then
				tbMap[tbNut[k]] = tbConLai[i]
				break
			end
		end
	end
	tbMap.szChu = GetName()
	tbPriseMap[PlayerIndex] = tbMap
	-- goi tin client m_szNotice chi 64 byte -> cat 60
	local szShort = szMsg or ""
	if strlen(szShort) > 60 then
		-- [PB 17/08] tranh chat doi ky tu TCVN3/GBK 2-byte o bien cat
		local nCat = 60
		local nHi = 0
		local q
		for q = 1, 60 do
			if strbyte(szShort, q) >= 128 then
				nHi = nHi + 1
			end
		end
		if mod(nHi, 2) == 1 then
			nCat = 59
		end
		szShort = strsub(szShort, 1, nCat)
	end
	tbMap.szMsgLuu = szShort
	tbMap.nTypeLuu = nType
	OpenQuestFinish(szShort, nType)
end

-- bo phat theo nut: tra ve dung ham goc voi dung tham so da boc
function Prise_Chon(szNut)
	local tbMap = tbPriseMap[PlayerIndex]
	if not tbMap then
		return
	end
	-- [PB 17/08] slot PlayerIndex tai su dung boi nguoi khac -> khong tra nham
	if (tbMap.szChu and tbMap.szChu ~= GetName()) then
		tbPriseMap[PlayerIndex] = nil
		return
	end
	local tbA = tbMap[szNut]
	if not tbA then
		-- [PB 17/08] nut khong anh xa: whitelist C++ da bi xoa sau cu bam -> mo lai
		-- cua so de nguoi choi chon nut khac (khong mat thuong)
		if (tbMap.szMsgLuu and tbMap.nTypeLuu) then
			OpenQuestFinish(tbMap.szMsgLuu, tbMap.nTypeLuu)
		end
		return
	end
	tbPriseMap[PlayerIndex] = nil
	if tbA.szFn == "mySG" then
		mySG(tbA.tbP[1], tbA.tbP[2], tbA.tbP[3], tbA.tbP[4], tbA.tbP[5], tbA.tbP[6])
	elseif tbA.szFn == "SelectAward_Money" then
		SelectAward_Money(tbA.tbP[1])
	elseif tbA.szFn == "SelectAward_Exp" then
		SelectAward_Exp(tbA.tbP[1])
	elseif tbA.szFn == "SelectAward_Change" then
		SelectAward_Change(tbA.tbP[1])
	elseif tbA.szFn == "SelectAward_Cancel" then
		SelectAward_Cancel(tbA.tbP[1])
	end
end

function finish_exp()
	Prise_Chon("finish_exp")
end
function finish_money()
	Prise_Chon("finish_money")
end
function quest_random()
	Prise_Chon("quest_random")
end
function finish_point()
	Prise_Chon("finish_point")
end
function finish_lucky()
	Prise_Chon("finish_lucky")
end
function finish_item()
	Prise_Chon("finish_item")
end

-- [PB 19/08] He bot KPlayer nho: Task_Cancel can THAM SO (1 = huy thuong) ma
-- KPlayer::ExecuteScript khong truyen so duoc -> boc lai o day cho bot goi.
function PB_BotCancel()
	Task_Cancel(1)
end

-- [PB 19/08 toi] "Huy rieng bot khong ton gi": tl_dealtask cap nhiem vu MOI de
-- thang len nhiem vu dang cam - khong cham 2420/2797/1036/luot huy/lan-trong-link
-- (tl_taskprocess moi tang cac bo dem chuoi; dealtask chi chon loai/dong + ghi
-- 1031/1032/1025 + course=1). CHI he bot goi - nguoi that van di Task_Cancel
-- co phat nhu cu.
function PB_BotDoiNhiemVu()
	tl_dealtask()
end

-- ============================================================
-- [TaskGuide 19/08/2026] Nut 'Bo nhiem vu' tren bang Chi nam nhiem vu (F11).
-- Mo dung hop xac nhan huy CHUAN (Task_CancelConfirm): du luat tru luot huy,
-- phat huy lau, huy bang 100 manh SHXT - y het dung truoc NPC chon muc huy.
-- Duoc goi tu xa qua UI_CMD case 6 (KProtocolProcess.cpp::UiCommandScript).
-- ============================================================
function tg_quit()
	if tl_gettaskcourse() ~= 1 then
		return
	end
	nt_setTask(1045, 1)
	Task_CancelConfirm()
end
