Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
Include("\\script\\global\\skills_table.lua")
-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\misc\\daiyitoushi\\toushi_function.lua")	-- ´øÒÕÍ¶Ê¦

-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\task\\150skilltask\\g_task.lua")
Include("\\script\\dailogsys\\g_dialog.lua")
Include("\\script\\misc\\eventsys\\type\\npc.lua")
Include("\\script\\activitysys\\g_activity.lua")
function main()
	local nNpcIndex = GetLastDiagNpc()
	local szNpcName = GetNpcName(nNpcIndex)
	if NpcName2Replace then
		szNpcName = NpcName2Replace(szNpcName)
	end
	local tbDailog = DailogClass:new(szNpcName)
	tbDailog.szTitleMsg = "<npc>GÇn ®©y ta cã rÊt nhiÒu viÖc gi¶i quyÕt, ng­¬i ®Õn ®©y cã viÖc g×."
	G_TASK:OnMessage(" Hoa S¬n", tbDailog, "DialogWithNpc")
	EventSys:GetType("AddNpcOption"):OnEvent(szNpcName, tbDailog, nNpcIndex)
	G_ACTIVITY:OnMessage("ClickNpc", tbDailog)
	tbDailog:AddOptEntry("ChuyÓn m«n ph¸i.", daiyitoushi_main, {10})
	tbDailog:AddOptEntry("Muèn thØnh gi¸o viÖc kh¸c", common_talk)
	tbDailog:Show() 
end

function common_talk()
    local UTask_hs = GetTask(3481)

    if (GetLevel() < 50 and UTask_hs > 0) or (UTask_hs < 60*256 and UTask_hs > 0) then --ÊÇ»ªÉ½µÜ×Ó£¬²»Âú60¼¶£¬»òÕßÊÇ»ªÉ½µÜ×Ó£¬Ã»Íê³É50¼¶ÈÎÎñ
	    Talk(1, "", "Nam Cung TuyÖt: §· gia nhËp vµo ph¸i Hoa S¬n ta, th× ph¶i ch¨m chØ luyÖn tËp, nh­ vËy míi xøng danh lµ ®Ö tö Hoa S¬n.")
    elseif GetLevel() >= 50 and UTask_hs == 60*256 then
	    Say("Nam Cung TuyÖt: §Ö tö bæn ph¸i, phµm tôc xuÊt s­, cÇn ph¶i tr·i qua mét lo¹t c¸c kh¶o nghiÖm, ng­¬i cã muèn tiÕp nhËn kh¶o nghiÖm kh«ng?", 2, "TiÕp nhËn kiÓm tra /task60_go", "§Ó ta suy nghÜ l¹i/no")
    elseif UTask_hs < 60*256+90 and UTask_hs > 60*256 then
	    if CalcItemCount(3, 6, 1, 4955, -1) < 1  then
		    if CalcFreeItemCellCount() < 1 then
			    Msg2Player("Hµnh trang ®· ®Çy, kh«ng thÓ nhËn vËt phÈm nhiÖm vô. ")
		    else
			    local nItemIdx = AddItem(6,1,4955,1,0,0)
			    SetItemBindState(nItemIdx,-2)
			    Msg2Player("NhËn ®­îc cÈm nang.")
			    Talk(1, "", "Nam Cung TuyÖt: Mãn ®å quan träng thÕ nµy mµ ng­¬i còng lµm mÊt? LÇn sau ph¶i cÈn thËn ®Êy.")
		    end
	    else
		    Talk(1, "", "Nam Cung TuyÖt: Mau ®i ®i.")
	    end
    elseif UTask_hs == 60*256+90 then
	    task60_1()
    elseif (GetSeries() == 2) and (GetCamp() == 4) and (GetLevel() >= 60) and (UTask_hs == 70*256) then --and ((GetTask(4) < 5*256) or (GetTask(4) == 75*256)) then		-- ÖØ·µÊ¦ÃÅÈÎÎñ
	    Talk(5, "task_chongfanshimen_go", "Ng­êi ch¬i: Ch­ëng m«n ……", "Nam Cung TuyÖt: Ng­¬i xuèng nói tr·i nghiÖm l©u nh­ vËy, c¶m gi¸c thÕ nµo?", "Ng­êi ch¬i: §Ö tö tõ khi xuèng nói, lµ phiªu b¹t giang hå, c¶m nhËn s©u s¾c con ng­êi trong giang hå, th©n bÊt do kû, lßng lu«n nhí vÒ nh÷ng ngµy th¸ng t¹i s­ m«n...", "Nam Cung TuyÖt: NÕu ®· nh­ vËy, ta cho phÐp ng­¬i quay l¹i m«n ph¸i, chØ cÇn ng­¬i muèn, ph¸i Hoa S¬n lu«n më cöa ®ãn nhËn.", "Ng­êi ch¬i: §Ö tö ®ång ý! Ngoµi ra, sau khi h¹ s¬n, ®Ö tö ®· tÝch gãp ®­îc 1 Ýt, qu©n tö ¸i tµi, thñ chi h÷u ®¹o, sè tiÒn nµy lµ do c¸c b»ng h÷u trong giang hå tÆng trong nh÷ng lÇn ®Ö tö hµnh hiÖp tr­îng nghÜa. §Ö tö vèn kh«ng dïng tíi, muèn quyªn gãp cho s­ m«n, còng lµ thµnh ý cña ®Ö tö.")
    elseif (GetCamp() == 4) and ((UTask_hs == 70*256+10) or (UTask_hs == 70*256+20)) then		-- ÖØ·µÊ¦ÃÅÈÎÎñÖÐ
	    Say("Nam Cung TuyÖt: ®· mang 50 l­îng ®Õn ch­a?",2,"§· mang ®Õn råi./return_complete","VÉn ch­a ®ñ/no")
    elseif (UTask_hs == 80*256) then						-- ÖØ·µºóµÄ×ÔÓÉ³öÈë
	    Say("Nam Cung TuyÖt: L¹i muèn h¹ s¬n tr·i nghiÖm sao?",2,"V©ng, mong Ch­ëng m«n cho phÐp./goff_yes","Kh«ng, ta tù thÊy c«ng phutËp luyÖn vÉn ch­a ®ñ. /no")
    else
	    Talk(1, "", "Nam Cung TuyÖt: ph¸i Hoa S¬n ta vèn dÜ danh tiÕng ®· l©u, lÇn nµy t¸i xuÊt giang hå, ph¶i cho thÕ nh©n thÊy ®­îc thanh uy cña Hoa S¬n.")
    end
end

function task60_1()
	Talk(1, "task60_finish", "Nam Cung TuyÖt: Tèt l¾m, xem ra ng­¬i ®· hoµn thµnh kh¶o nghiÖm, cã thÓ xuÊt s­ su«n sÎ råi!")
end

function task60_go()
	if CalcFreeItemCellCount() < 1 then
		Msg2Player("Hµnh trang ®· ®Çy, kh«ng thÓ nhËn vËt phÈm nhiÖm vô. ")
	else
		SetTask(3481, 60*256+10)
		local nItemIdx = AddItem(6,1,4955,1,0,0)
		SetItemBindState(nItemIdx,-2)
		Msg2Player("NhËn ®­îc cÈm nang.")
		Talk(1, "", "Nam Cung TuyÖt: trong cÈm nang nµy cã 3 m·nh giÊy, mçi m·nh giÊy lµ mét nhiÖm vô, sau khi hoµn thµnh 1 nhiÖm vô míi cã thÓ më ra m·nh giÊy thø 2 cø nh­ thÕ mµ hoµn thµnh, ng­¬i mau ®i ®i.")
		Msg2Player("GÆp Nam Cung TuyÖt, tiÕp nhËn nhiÖm vô xuÊt s­. kiÓm tra cÈm nang. ")
		AddNote("GÆp Nam Cung TuyÖt, tiÕp nhËn nhiÖm vô xuÊt s­. kiÓm tra cÈm nang. ")
	end
end

function task60_finish()
	ConsumeItem(-1, 1, 6, 1, 4955, -1)
	SetTask(3481, 70*256)

	SetFaction("")
	SetRank(88)
	SetCamp(4)
	SetCurCamp(4)
	Msg2Player("Håi ®¸p Nam Cung TuyÖt, hoµn thµnh nhiÖm vô xuÊt s­. Ng­¬i ®· xuÊt s­ thµnh c«ng, NhËn ®­îc danh hiÖu Lôc NghÖ TruyÒn Nh©n.")
	AddNote("Håi ®¸p Nam Cung TuyÖt, hoµn thµnh nhiÖm vô xuÊt s­. Ng­¬i ®· xuÊt s­ thµnh c«ng, NhËn ®­îc danh hiÖu Lôc NghÖ TruyÒn Nh©n.")
	AddRepute(120)
end

function task_chongfanshimen_go()
	Say("Nam Cung TuyÖt: Tèt l¾m, h·y ®em 50 l­îng tíi ®©y.",2,"§­îc!/return_yes","§Ó ta suy nghÜ l¹i./no")
end

function return_yes()
	Talk(1, "", "Ng­êi ch¬i: §­îc, §Ö tö sÏ ®i lÊy 50 l­îng. ")
	SetTask(3481,70*256+20)
	AddNote("Cèng hiÕn 50 l­îng lµ cã thÓ trë l¹i s­ m«n.")
	Msg2Player("Cèng hiÕn 50 l­îng lµ cã thÓ trë l¹i s­ m«n.")
end

function return_complete()
	if(GetCash() >= 50) then
		Talk(1,"","Nam Cung TuyÖt: Giê ta phong con lµm Th¸i Häc C¸c Chñ, vµ truyÒn thô tuyÖt häc bæn m«n: Th­¬ng Tïng Nghªnh Kh¸ch, Ma V©n KiÕm KhÝ, HuyÒn Nh·n V©n Yªn.")
		Pay(50)
		SetTask(3481,80*256)
		SetFaction("huashan")
		SetCamp(3)
		SetCurCamp(3)
		SetRank(89)
		add_hs(70)			-- µ÷ÓÃskills_table.luaÖÐµÄº¯Êý£¬²ÎÊýÎªÑ§µ½¶àÉÙ¼¶¼¼ÄÜ¡£
		Msg2Player("Ng­¬i ®· trë l¹i s­ m«n, NhËn ®­îc danh hiÖu Th¸i Häc C¸c chñ, häc ®­îc vâ c«ng Th­¬ng Tïng Nghªnh Kh¸ch, Ma V©n KiÕm KhÝ, HuyÒn Nh·n V©n Yªn.")
		AddNote("Ng­¬i ®· trë l¹i s­ m«n, NhËn ®­îc danh hiÖu Th¸i Häc C¸c chñ, häc ®­îc vâ c«ng Th­¬ng Tïng Nghªnh Kh¸ch, Ma V©n KiÕm KhÝ, HuyÒn Nh·n V©n Yªn.")
		DynamicExecuteByPlayer(PlayerIndex, "\\script\\activitysys\\config\\21\\extend.lua", "pActivity:ChongFanShiMen", "chongfanshimen")
	else
		Talk(2,"","Nam Cung TuyÖt: Ng­¬i ch­a mang ®ñ 50 l­îng.","Ng­êi ch¬i: Ta l¹i t×m thö xem sao. ")
	end
end

function goff_yes()
	Talk(1,"","Nam Cung TuyÖt: §­îc, ng­¬i ®i ®i, vµo chèn giang hå ph¶i cÈn thËn.")
	SetTask(3481,70*256)
	AddNote("Ta rêi ph¸i Hoa S¬n, t¸i xuÊt giang hå.")
	Msg2Player("Ta rêi ph¸i Hoa S¬n, t¸i xuÊt giang hå.")
	SetFaction("")
	SetCamp(4)
	SetCurCamp(4)
	DynamicExecuteByPlayer(PlayerIndex, "\\script\\activitysys\\config\\21\\extend.lua", "pActivity:ChongFanShiMen", "zaicichushi")
end

function no()
end
