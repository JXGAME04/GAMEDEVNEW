-- [LOCAL54 06/09 toi] 5 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local YDBZ_cancel_match, YDBZ_ready_start_match, YDBZ_ready_match, YDBZ_ready_broad_talk, YDBZ_ready_close_match_timer
-- description	: 
-- author		: wangbin
-- datetime		: 2005-06-04
Include("\\script\\missions\\yandibaozang\\readymap\\include.lua")
Include("\\script\\missions\\yandibaozang\\include.lua")

-- 
-- by 
-- 2007.10.24
-- ÎÒ..
-- ..
-- ..

function YDBZ_cancel_match()
	-- Msg2SubWorld(date("[%H:%M:%S]") .. "")
end

-- 
function YDBZ_ready_start_match()
	-- 
	local oldworld = SubWorld
	tbReady.ReadyState = 3;

	local player_count = GetMSPlayerCount(YDBZ_READY_MISSION);
	local teams_count = GetMissionV(YDBZ_READY_TEAM)
	if (player_count == 0 or teams_count < YDBZ_TEAM_START_LIMIT) then
		-- 
		Msg2MSAll(YDBZ_READY_MISSION,"Do v× tæ ®éi kh«ng ®ñ, kh«ng thÓ më b¶o tµng Viªm §Õ")
		YDBZ_cancel_match();
	else
		broadcast("Ho¹t ®éng b¶o tµng Viªm §Õ ®· chÝnh thøc b¾t ®Çu, c¸c cao thñ ®ang trong tr¹ng th¸i thi ®Êu");
		SubWorld = oldworld
		YDBZ_ready_start_missions(YDBZ_MAP_MAP, SubWorld,YDBZ_MISSION_MATCH,YDBZ_READY_MISSION);	
		
		-- 
--		YDBZ_start_close_timer();
		
		-- 
		SubWorld = oldworld
		

		-- 
--		SetMissionV(VARV_NPC_BATCH, 1);	-- 
--		SetMissionV(VARV_MISSION_RESULT, 0);
--		SetMissionV(VARV_PLAYER_USE_INDEX, 0);
--		SetMissionV(VARV_NPC_USE_INDEX, 0);
		
--		-- 
--		save_player_info();
--    	
--		-- 
		tbReady.ReadyState = 0;
		SetMissionV(YDBZ_READY_TEAM, 0);
		--YDBZ_ready_close_match()
--    	
--		-- 1
--		create_batch_npc(1);
	end
	
	-- 
	--SetMissionV(VARV_PLAYER_COUNT, player_count);
	YDBZ_ready_close_match_timer()
	YDBZ_ready_close_match()
end
function YDBZ_ready_match()
	broadcast(format("Thêi gian b¸o danh v­ît ¶i b¶o tµng viªm ®Õ ®· kÕt thóc, xin mêi c¸c vâ l©m nh©n sü h·y chuÈn bÞ lÇn cuèi, cßn %s b¾t ®Çu.",YDBZ_READY_LIMIT_WAIT));
	Msg2MSAll(YDBZ_READY_MISSION,format("Thêi gian b¸o danh b¶o tµng Viªm §Õ kÕt thóc, xin mêi c¸c vÞ vâ l©m cao thñ chuÈn bÞ lÇn cuèi. Cßn <color=yellow>%s gi©y<color> b¾t ®Çu tranh ®o¹t.",YDBZ_READY_LIMIT_WAIT))
	StartMissionTimer(YDBZ_READY_MISSION, YDBZ_READY_TIMER, YDBZ_READY_LIMIT_WAIT * 18);
	tbReady.ReadyState =  2;
end

function YDBZ_ready_broad_talk()
	local broadstate = tbReady.nReadyBroadState
	local nlimittime = floor((YDBZ_READY_LIMIT_SIGNUP - (broadstate * YDBZ_READY_LIMIT_BROAD))/60)
	Msg2MSAll(YDBZ_READY_MISSION,format("V­ît ¶i b¶o tµng viªm ®Õ cßn <color=yellow>%s phót<color> kÕt thóc thêi gian b¸o danh, h·y nhanh nhanh b¸o danh.",nlimittime))
	tbReady.nReadyBroadState = broadstate + 1
end
-- 
function YDBZ_ready_close_match_timer()
	StopMissionTimer(YDBZ_READY_MISSION, YDBZ_READY_TIMER);
end

function OnTimer()
	local state = tbReady.ReadyState
	local broadstate = tbReady.nReadyBroadState
	if state == 1 then
		-- 
		if ( broadstate * YDBZ_READY_LIMIT_BROAD ) >= YDBZ_READY_LIMIT_SIGNUP then
			YDBZ_ready_close_match_timer();
			-- 
			YDBZ_ready_match();
			--print("")
		else
			YDBZ_ready_broad_talk()
		end
	elseif state == 2 then
		--print("")
		YDBZ_ready_close_match_timer();
		YDBZ_ready_start_match();
	end
--	start_board_timer();
end
