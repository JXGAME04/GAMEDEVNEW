
Include("\\script\\dailogsys\\dailogsay.lua")
Include("\\script\\missions\\yandibaozang\\head.lua")
Include("\\script\\missions\\yandibaozang\\include.lua")
Include("\\script\\missions\\yandibaozang\\readymap\\ready.lua")
Include("\\script\\missions\\yandibaozang\\readymap\\readyMap.lua")
Include("\\script\\missions\\yandibaozang\\readymap\\include.lua")
Include("\\script\\lib\\log.lua")
local ndebug = 0

function YDBZ_DebugDailog()
	
end

function main()
	local szTitle = "<npc>C¸ch ch¬i v­ît ¶i míi, néi dung míi, boss míi, b¶n ®å míi, v« vµn kú tr©n dÞ b¶o, rÊt thÝch hîp c¸c trang bÞ hoµng kim cña m«n ph¸i chØ cã t¹i b¶o tµng viªm ®Õ. C¸c h¹ ®· chuÈn bÞ ch­a?"
	
	local tbOpt = 
	{
		{"Xem t×nh h×nh tæ ®éi", YDBZ_view_player},
		{"Nguån gèc b¶o tµng viªm ®Õ", YDBZ_about},
		{" Nh©n tiÖn ghÐ qua th«i"},
	}
	if tbReady.nState == 1 then
		tinsert(tbOpt,1, {"B¸o danh tham gia", YDBZ_want_play})
	end
	
	if ndebug == 1 then
		tinsert(tbOpt,1, {"B¸o danh tham gia", YDBZ_DebugDailog})
	end
	
	CreateNewSayEx(szTitle, tbOpt)
end

function YDBZ_PlayerCheck()
	--local nFlag1 = DynamicExecuteByPlayer(PlayerIndex,"\\script\\vng_lib\\bittask_lib.lua", "tbVNG_BitTask_Lib:getBitTask", {nTaskID = 3071,nStartBit = 25,nBitCount = 1,nMaxValue = 1})
	--if nFlag1 ~= 0 then		
	--	return nil, format("<color=yellow>%s<color>", GetName())
	--end
	if GetCamp() == 0 then
		return nil, "Thµnh viªn trong tæ ®éi cã ng­êi ch­a gia nhËp m«n ph¸i, kh«ng thÓ tham gia ho¹t ®éng."
	end
	if(GetLevel() < YDBZ_LIMIT_PLAYER_LEVEL) then
		return nil, format("Trong ®éi ngò ng­êi ch¬i %s ®¼ng cÊp kh«ng ®¹t %d, kh«ng thÓ tham gia ho¹t ®éng.", GetName(), YDBZ_LIMIT_PLAYER_LEVEL)
	end
	
	--
	local ncount, ndaycount = YDBZ_UpdateTask()
	local nDoubleItemCount = CalcEquiproomItemCount(YDBZ_LIMIT_DOUBEL_ITEM[1][1],YDBZ_LIMIT_DOUBEL_ITEM[1][2],YDBZ_LIMIT_DOUBEL_ITEM[1][3],-1)
	local nItemCount = CalcEquiproomItemCount(YDBZ_LIMIT_ITEM[1][1],YDBZ_LIMIT_ITEM[1][2],YDBZ_LIMIT_ITEM[1][3],-1)
	
	if nDoubleItemCount < YDBZ_LIMIT_DOUBEL_ITEM[2] then
		if ncount >= YDBZ_LIMIT_WEEK_COUNT then
			return nil, format("trong ®éi ngò ng­êi ch¬i %s ®· ®Õn møc giíi h¹n tham gia trong tuÇn.", GetName())
		end
		if ndaycount >= YDBZ_LIMIT_DAY_COUNT then
			return nil, format("trong ®éi ngò ng­êi ch¬i %s ®· ®Õn møc giíi h¹n tham gia trong ngµy.", GetName())
		end
	else
		return 1
	end
	
	if nItemCount < YDBZ_LIMIT_ITEM[2] then
		return nil, format("%s ng­êi ch¬i kh«ng mang theo t%s.", GetName(), YDBZ_LIMIT_ITEM[3])
	end	
	YDBZ_sdl_setTaskByte(YDBZ_ITEM_YANDILING,1,0)
	YDBZ_sdl_setTaskByte(YDBZ_ITEM_YANDILING,2,0)
	return 1
end

function YDBZ_view_player()
	if (GetTeamSize() < YDBZ_TEAM_COUNT_LIMIT or GetTeamSize() > YDBZ_TEAM_COUNT_MAXLIMIT) then
		local szTitle = "<npc>Tæ ®éi tham gia b¶o tang viªm ®Õ cÇn Ýt nhÊt <color=red>"..YDBZ_TEAM_COUNT_LIMIT.."<color> ng­êi vµ kh«ng thÓ v­ît qu¸ <color=red>"..YDBZ_TEAM_COUNT_MAXLIMIT.."<color>. Tr­íc m¾t sè ng­êi trong tæ ®éi kh«ng phï hîp yªu cÇu."
		local tbOpt = 
		{
			{" BiÕt råi!"}
		}
		CreateNewSayEx(szTitle, tbOpt)
		return
	end
	
	if YDBZ_PartyCheck() == 1 then
		Talk(1, "", "Tæ ®éi cña c¸c h¹ phï hîp víi ®iÒu kiÖn tiÕn vµo b¶o tµng viªm ®Õ")
	end
end

function YDBZ_PartyCheck()
	local flag = nil 
	for i=1, GetTeamSize() do
		local nPlayerIndex = GetTeamMember(i)
		if nPlayerIndex > 0 then
			local ok, err = CallPlayerFunction(nPlayerIndex, YDBZ_PlayerCheck)
			if ok ~= 1 then
				Msg2Player(err)
				flag = 1
			end
		end
	end
	if flag == 1 then
		return
	else
		return 1
	end
end

function YDBZ_PlayerConsume()
	local nDoubleItemCount = CalcEquiproomItemCount(YDBZ_LIMIT_DOUBEL_ITEM[1][1],YDBZ_LIMIT_DOUBEL_ITEM[1][2],YDBZ_LIMIT_DOUBEL_ITEM[1][3],-1)
	local nItemCount = CalcEquiproomItemCount(YDBZ_LIMIT_ITEM[1][1],YDBZ_LIMIT_ITEM[1][2],YDBZ_LIMIT_ITEM[1][3],-1)
	if nDoubleItemCount >= YDBZ_LIMIT_DOUBEL_ITEM[2] then
		if ConsumeEquiproomItem(YDBZ_LIMIT_DOUBEL_ITEM[2], YDBZ_LIMIT_DOUBEL_ITEM[1][1],YDBZ_LIMIT_DOUBEL_ITEM[1][2],YDBZ_LIMIT_DOUBEL_ITEM[1][3],-1) == 1 then
			YDBZ_sdl_setTaskByte(YDBZ_ITEM_YANDILING,1,1)
			AddSkillState(461,1, 1,30*60*18,1)
			SetTask(YDBZ_ITEM_YANDILING_SUM,(GetTask(YDBZ_ITEM_YANDILING_SUM)+1))
			tbLog:PlayerActionLog("TinhNangKey","BaoDanhViemDe_SDViemDeLenh")
		else
			tbLog:PlayerActionLog("yandibaozang","consume error", "SDViemDeLenh", nDoubleItemCount)
		end
		return 1
	end
	if nItemCount >= YDBZ_LIMIT_ITEM[2] then
		if ConsumeEquiproomItem(YDBZ_LIMIT_ITEM[2], YDBZ_LIMIT_ITEM[1][1],YDBZ_LIMIT_ITEM[1][2],YDBZ_LIMIT_ITEM[1][3],-1) == 1 then
			tbLog:PlayerActionLog("TinhNangKey","BaoDanhViemDe_SDAnhHungThiep")
		else
			tbLog:PlayerActionLog("yandibaozang","consume error", "SDAnhHungThiep", nItemCount)
		end
		return 1
	end
	tbLog:PlayerActionLog("yandibaozang","consume error")
	return nil
end

function YDBZ_PartyConsume()
	for i=1, GetTeamSize() do
		local nPlayerIndex = GetTeamMember(i)
		if nPlayerIndex > 0 then
			if CallPlayerFunction(nPlayerIndex, YDBZ_PlayerConsume) ~= 1 then
				return nil
			end
		end
	end
	return 1
end
function YDBZ_want_play()
	if tbReady.nState ~= 1 then
		Talk(1, "", "xin lçi, vÉn ch­a ®Õn thêi gian b¸o danh.")
		return
	end
	if (IsCaptain() ~= 1) then
		Talk(1, "", "Tham gia ho¹t ®éng v­ît ¶i viªm ®Õ b¶o tµng cÇn ph¶i do ®éi tr­ëng b¸o danh míi vµo ®­îc.")
		return
	end
	if (GetTeamSize() < YDBZ_TEAM_COUNT_LIMIT or GetTeamSize() > YDBZ_TEAM_COUNT_MAXLIMIT) then
		Talk(1, "", "Tæ ®éi tham gia b¶o tang viªm ®Õ cÇn Ýt nhÊt <color=red>"..YDBZ_TEAM_COUNT_LIMIT.."<color> ng­êi vµ kh«ng thÓ v­ît qu¸ <color=red>"..YDBZ_TEAM_COUNT_MAXLIMIT.."<color>. Tr­íc m¾t sè ng­êi trong tæ ®éi kh«ng phï hîp yªu cÇu.")
		return 1
	end
	if YDBZ_PartyCheck() ~= 1 then
		return 
	end
	
	local nMapId = ReadyMap:GetFreeMap()
	if nMapId > 0 then
		if YDBZ_PartyConsume() ~= 1 then
			Msg2Player("tr¹ng th¸i bÊt th­êng, xin h·y liªn l¹c víi bé phËn ch¨m sãc kh¸ch hµng.")
			return
		end
		local nPartyId = tbReady:NewPartyId()		
		tbReady:BroadCast(format("tranh ®o¹t Kho B¸u Viªm §Õ s¾p diÔn ra, hiÖn t¹i cã %d chi ®éi tham gia.", tbReady.nMapCount))
		local nIdx = random(1,getn(tbReady.tbPos))
		local nPosX = tbReady.tbPos[nIdx][1]
		local nPosY = tbReady.tbPos[nIdx][2]
		
		local tbTeamMember = {}
		for i=1, GetTeamSize() do
			tbTeamMember[i] = GetTeamMember(i)
		end
		
		for i=1, getn(tbTeamMember) do
			local nPlayerIndex = tbTeamMember[i]
			if nPlayerIndex > 0 then
				local szName = CallPlayerFunction(nPlayerIndex, GetName)
				tbReady:AddPartyMember(nPartyId, szName)
				CallPlayerFunction(nPlayerIndex, NewWorld, nMapId, nPosX, nPosY)
			end
		end
	else
		Talk(1, "", "xin lçi, hiÖn t¹i khu vùc nµy kh«ng cã nhiÖm vô Kho B¸u Viªm §Õ. H·y ®îi ®Õn lóc cã th× quay l¹i tham gia nhÐ.~")
	end
end

function YDBZ_about()
	Say(format("C¸ch ch¬i v­ît ¶i míi, néi dung míi, boss míi, b¶n ®å míi, v« vµn kú tr©n dÞ b¶o, rÊt thÝch hîp c¸c trang bÞ hoµng kim cña m«n ph¸i chØ cã t¹i b¶o tµng viªm ®Õ! <enter>Mçi ngµy %s, h·y ®Õn tr­íc 5 phót ®Ó b¸o danh ë B×nh B×nh c« n­¬ng.", "8:30, 9;30, 10:30, 12:30, 14:30, 16:30, 20:30, 22:30"),0)
end