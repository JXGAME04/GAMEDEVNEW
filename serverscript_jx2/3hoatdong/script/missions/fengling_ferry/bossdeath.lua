Include("\\script\\global\\signet_head.lua")
Include("\\script\\missions\\fengling_ferry\\fld_head.lua")
Include("\\script\\event\\jiefang_jieri\\200904\\shuizei\\shuizei.lua");
Include("\\script\\event\\change_destiny\\mission.lua");	-- 逆天改命
Include("\\script\\misc\\eventsys\\type\\func.lua")
Include("\\script\\header\\cauhinh_hoatdong.lua")	-- [3HD] noi cauhinh
SIGNET_DROPCOUNT = 2
SIGNET_DROPCOUNT = HD_CFG("HD3_PLD_SO_AN_BOSS", SIGNET_DROPCOUNT)	-- so Thi Gia Chi An roi tu boss dau linh

function OnDeath( nNpcIndex )
	local x, y, world = GetNpcPos(nNpcIndex);
	local maxcount = SIGNET_DROPCOUNT;
	local _, nTongID = GetTongName();
	if (nTongID > 0) then
		if (IsDoubledMoneyBox(nTongID)) then
			maxcount = getSignetDropRate(SIGNET_DROPCOUNT);
		end;
	end;
	local nCurtime = GetCurServerTime() + 7 * 24 *60 * 60
	local nYear = tonumber(FormatTime2String("%Y", nCurtime))
	local nMonth = tonumber(FormatTime2String("%m", nCurtime))
	local nDay = tonumber(FormatTime2String("%d", nCurtime))
	for i = 1, maxcount do
		local nItemIdx = DropItem(nNpcIndex, 6, 1, 1095, 1, 0, 0);
		SetSpecItemParam(nItemIdx, 1, nCurtime);
		SetSpecItemParam(nItemIdx, 2, nYear)
		SetSpecItemParam(nItemIdx, 3, nMonth)
		SetSpecItemParam(nItemIdx, 4, nDay)
		SyncItem(nItemIdx);
	end;
	
	if (jf0904_shuizei_IsActtime() == 1) then
		local nItemIdx = DropItem(nNpcIndex, 6, 1, 2024, 1, 0, 0);
		
		local nCurtime = tonumber(GetLocalDate("%H%M"));
		local nRestMin = 24 * 60 - (floor(nCurtime/100)*60+floor(mod(nCurtime, 100)));
		ITEM_SetExpiredTime(nItemIdx, nRestMin);
		SyncItem(nItemIdx);
		
		local nRate = HD_CFG("HD3_PLD_TILE_HAILONG", 0.005);
		local nCurRate	= random(10000000)
		if (nCurRate <= 10000000 * nRate) then		
			local x, y, world = GetNpcPos(nNpcIndex);
			local nItemIdx = DropItem(nNpcIndex, 6, 1, 2124, 1, 0, 0);
		end
	end
	
	-- 逆天改命
	tbChangeDestiny:completeMission_WaterThief();
	EventSys:GetType("FengLingDu"):OnEvent("OnShuiZeiDeath", nNpcIndex, PlayerIndex)
end

-- [3HD 25/08 C41] Engine JX1 goi OnRevive cho MOI NPC co script chet moi lan
-- hoi sinh; ban Linux khong dinh nghia (engine JX2 khong goi) => ScriptError
-- "attempt to call a nil value" lap lai moi nhip. Ham rong = giu nguyen hanh vi.
function OnRevive()
end
