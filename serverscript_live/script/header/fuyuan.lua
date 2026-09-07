-- ================================================================================================
-- [HE THONG] script/header/fuyuan.lua
-- Muc dich  : He PHUC DUYEN.
-- Duoc nap  : Include tu 7 tep (vd nhadich_dailao.lua, nhamon.lua, truyennhan.lua, fuyuanlu_big.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Include   : lib_task.lua
-- Ham (dong): FuYuan_Start (14), FuYuan_Pause (21), FuYuan_Resume (41), FuYuan_Gain (48), FuYuan_GetDepositTime (89), FuYuan_Get (128), FuYuan_Set (132), FuYuan_Add (140), FuYuan_Reduce (148), IsFuYuanStarted (156), IsFuYuanAvailable (165), IsFuYuanPaused (173)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
--Author: Fong KiÒu
--Date: 2021
--Function: Lib phóc duyªn

Include("\\script\\lib\\lib_task.lua")

TIME_UNIT = 3600;
TIME_PER_FUYUAN = TIME_UNIT;
TIME_FUYUAN_THRESHOLD1 = 2 * TIME_UNIT;
TIME_FUYUAN_THRESHOLD2 = 4 * TIME_UNIT;
FUYUAN_EXTRA = 2;
TIMES_IN_DAY_EXTRA = 2;
FUYUAN_MAX_GAIN = 100;

function FuYuan_Start()
	SetTask( TASKID_FY_START_ONLINE_TIME, GetGameTime() );
	SetTask( TASKID_FY_ADDITIONAL_TIME, 0 );
	SetTask( TASKID_GAIN_LAST_DATE, date("%Y%m%d") );
	SetTask( TASKID_GAIN_TIMES_IN_DAY, 0 );	
end

function FuYuan_Pause()
	if( IsFuYuanAvailable() ~= 1 ) then
		return 0;
	end
	local nFYStartOnlineTime = GetTask( TASKID_FY_START_ONLINE_TIME );
	local nFYAdditionalTime = GetTask( TASKID_FY_ADDITIONAL_TIME );
	if( IsFuYuanPaused() ~= 1 ) then
		if (nFYAdditionalTime < 0) then
			nFYAdditionalTime = 0
		end		
		local nFYDiffer = GetGameTime() - nFYStartOnlineTime;
		if (nFYDiffer < 0) then
			nFYDiffer = 0
		end				
		local nFYTotalTime = nFYDiffer + nFYAdditionalTime;		
		SetTask( TASKID_FY_ADDITIONAL_TIME, nFYTotalTime );
		SetTask( TASKID_FY_START_ONLINE_TIME, -1 );
	end
end

function FuYuan_Resume()
	if( IsFuYuanAvailable() ~= 1 or IsFuYuanPaused() ~= 1 ) then
		return 0;
	end
	SetTask( TASKID_FY_START_ONLINE_TIME, GetGameTime() );
end

function FuYuan_Gain()
	if( IsFuYuanAvailable() ~= 1 ) then
		return -1;
	end
	
	local nFuYuanGained = 0;
	local nFYStartOnlineTime;
	local nFYTotalTime, nFYValidTime = FuYuan_GetDepositTime();
	local nCurrentOnlineTime = GetGameTime();
	local nFYGainLastDate = GetTask( TASKID_GAIN_LAST_DATE );
	local nFYGainTimesInDay = GetTask( TASKID_GAIN_TIMES_IN_DAY )
	
	if( nFYValidTime < TIME_FUYUAN_THRESHOLD1 ) then
		return 0;
	end
		
	nFuYuanGained = floor( nFYValidTime / TIME_PER_FUYUAN );
	nFYGainTimesInDay = nFYGainTimesInDay + 1;
	nFYStartOnlineTime = nCurrentOnlineTime - mod( nFYValidTime, TIME_UNIT );
	
	if( tonumber(date("%Y%m%d")) == nFYGainLastDate ) then
		if( nFYGainTimesInDay == TIMES_IN_DAY_EXTRA ) then
			nFuYuanGained = nFuYuanGained + FUYUAN_EXTRA;
		end
	else
		nFYGainLastDate = date("%Y%m%d");
		nFYGainTimesInDay = 1;
	end
	
	if( nFuYuanGained >= FUYUAN_MAX_GAIN ) then
		FuYuan_Add( FUYUAN_MAX_GAIN );
	else
		FuYuan_Add( nFuYuanGained );
	end
	SetTask( TASKID_FY_START_ONLINE_TIME, nFYStartOnlineTime );
	SetTask( TASKID_FY_ADDITIONAL_TIME, 0 );
	SetTask( TASKID_GAIN_LAST_DATE, nFYGainLastDate );
	SetTask( TASKID_GAIN_TIMES_IN_DAY, nFYGainTimesInDay );
	return 1;
end

function FuYuan_GetDepositTime()
	if( IsFuYuanAvailable() ~= 1 ) then
		return 0, 0;
	end
	local nCurrentOnlineTime = GetGameTime();
	local nFYStartOnlineTime = GetTask( TASKID_FY_START_ONLINE_TIME );
	local nFYAdditionalTime = GetTask( TASKID_FY_ADDITIONAL_TIME );
	local nFYTotalTime;
	local nFYValidTime;
	if( IsFuYuanPaused() == 1 ) then
		if( GetTeamSize() > 1 ) then
			return 0, 0;
		else
			nFYStartOnlineTime = nCurrentOnlineTime;
			FuYuan_Resume();
		end
	end
		
	if (nFYAdditionalTime < 0) then
		nFYAdditionalTime = 0;
		SetTask(TASKID_FY_ADDITIONAL_TIME, 0);
	end
	
	if ((nCurrentOnlineTime - nFYStartOnlineTime) < 0) then
		SetTask(TASKID_FY_START_ONLINE_TIME, nCurrentOnlineTime);				
		nFYStartOnlineTime = nCurrentOnlineTime;
	end
	
	nFYTotalTime = ( nCurrentOnlineTime - nFYStartOnlineTime ) + nFYAdditionalTime;	
			
	if( nFYTotalTime <= TIME_FUYUAN_THRESHOLD2 ) then
		nFYValidTime = nFYTotalTime;
	else
		local nHour = floor( nFYTotalTime / TIME_UNIT );
		nFYValidTime = ( nHour - floor( ( nFYTotalTime - TIME_FUYUAN_THRESHOLD2 ) / ( 2 * TIME_UNIT ) ) ) * TIME_UNIT + mod( nFYTotalTime, TIME_PER_FUYUAN );
	end
	return nFYTotalTime, nFYValidTime;
end

function FuYuan_Get()
	return GetFuYuan()
end

function FuYuan_Set(value)
	if( IsFuYuanAvailable() ~= 1 ) then
		return 0;
	end
	SetFuYuan(value);
	return 1;
end

function FuYuan_Add( value )
	local nResult = FuYuan_Set( FuYuan_Get() + value );
	if(  nResult == 1 ) then
		Msg2Player("B¹n nhËn ®­îc "..value.." ®iÓm phóc duyªn");
	end
	return nResult;
end

function FuYuan_Reduce( value )
	local nResult = FuYuan_Set( FuYuan_Get() - value );
	if(  nResult == 1 ) then
		Msg2Player("B¹n ®· tèn "..value.." ®iÓm phóc duyªn");
	end
	return nResult;
end

function IsFuYuanStarted()
	local nFYStartOnlineTime = GetTask( TASKID_FY_START_ONLINE_TIME );
	if( nFYStartOnlineTime == 0 ) then
		return 0;
	else
		return 1;
	end
end

function IsFuYuanAvailable()
	if(IsFuYuanStarted() ~= 1 ) then
		return 0;
	else
		return 1;
	end
end

function IsFuYuanPaused()
	local nFYStartOnlineTime = GetTask( TASKID_FY_START_ONLINE_TIME );
	if( nFYStartOnlineTime < 0 ) then
		return 1;
	else
		return 0;
	end
end
