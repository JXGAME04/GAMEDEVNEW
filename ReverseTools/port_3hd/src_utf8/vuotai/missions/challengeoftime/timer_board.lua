-- description	: 通告定时器，每分钟触发一次
-- author		: wangbin
-- datetime		: 2005-06-07

Include("\\script\\missions\\challengeoftime\\include.lua")

function OnTimer()
	if (GetMSPlayerCount(MISSION_MATCH) ~= 0) then
		local time = GetMissionV(VARV_BOARD_TIMER) + INTERVAL_BOARD * 60;
		local batch = GetMissionV(VARV_NPC_BATCH);
		local count = GetMissionV(VARV_NPC_COUNT);
		SetMissionV(VARV_BOARD_TIMER, time);
		
		local msg = "<#> Nhiệm vụ 'Thách thức Thời gian' đã bắt đầu rồi" .. floor(time / 60) 
			.. "phút, bây giờ là "
			.. batch
			.. "đám quái, còn dư "
			.. count
			.. "Con quái.";
		Msg2MSAll(MISSION_MATCH, msg);
	end
end

