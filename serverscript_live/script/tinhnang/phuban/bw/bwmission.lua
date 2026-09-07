-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local bw_group_to_fight
Include("\\script\\missions\\bw\\bwhead.lua")
function InitMission()
	for i = 1, 40 do 
		SetMissionV(i,0);--允许报名了
	end;
	
	for i = 1, 10 do 
		SetMissionS(i, "")
	end;
	SetMissionV(MS_STATE, 1);
	SetMissionV(BW_KEY, random(10000000)+1);
	StartMissionTimer(BW_MISSIONID, BW_SMALLTIME_ID, TIMER_1);
	StartMissionTimer(BW_MISSIONID, BW_TOTALTIME_ID, TIMER_2);
	CreateChannel("L玦 i nhi襲 ngi b猲 Gi竝", 11);
	CreateChannel("L玦 i nhi襲 ngi b猲 蕋", 11);
	
	for i = 1, getn(CS_CamperPos) do
		local nNpcIdx = AddNpc(332, 1, SubWorld, CS_CamperPos[i][1] * 32, CS_CamperPos[i][2] * 32, 1, CS_CamperPos[i][3]);
		SetNpcScript(nNpcIdx, "\\script\\missions\\bw\\bwcamper.lua");
	end
end;

function RunMission()
	bw_group_to_fight(1);	--the param stand for group ID;
	bw_group_to_fight(2);

	SetMissionV(MS_STATE, 2);
end;

function EndMission()
	for i = 1, 40 do 
		SetMissionV(i , 0);
	end;
	
	for i  = 1, 10 do 
		SetMissionS(i, "")
	end;
	
	GameOver();
	StopMissionTimer(BW_MISSIONID, BW_SMALLTIME_ID);
	StopMissionTimer(BW_MISSIONID, BW_TOTALTIME_ID);
	DeleteChannel("L玦 i nhi襲 ngi b猲 Gi竝");
	DeleteChannel("L玦 i nhi襲 ngi b猲 蕋");
	local subworldid = SubWorldIdx2ID(SubWorld)
	ClearMapNpc(subworldid);
end;

function OnLeave(RoleIndex)
	PlayerIndex = RoleIndex;
	-- [BW 23/08] CHAN DE QUY: engine JX1 goi OnLeave TRUOC khi xoa entry (KMission.cpp:185/188
	-- 'Fix by Fong Kieu') -> LeaveGame():72 DelMSPlayer se vao lai OnLeave vo han (tran C stack).
	-- LeaveGame ha TaskTemp(200)=0 (bwhead:61) TRUOC DelMSPlayer nen nhanh long nhau return o day;
	-- nguoi da roi tran (200==0) cung khong bi NewWorld keo lai.
	if (GetTaskTemp(200) ~= 1) then
		return
	end

	str2 = GetName().."R阨 kh醝 u trng";
	LeaveGame();
	NewWorld(GetTask(BW_SIGNPOSWORLD), GetTask(BW_SIGNPOSX), GetTask(BW_SIGNPOSY));
end;

--将一个组的人都变成战斗状态
function bw_group_to_fight(nGroupID)
	if (nGroupID < 0) then
		return
	end;
	local OldPlayer = PlayerIndex;

	local idx = 0;
	local pidx, i;
	for i = 1, 400 do
		idx , pidx = GetNextPlayer(BW_MISSIONID, idx, nGroupID);
		if (pidx > 0) then
		   	PlayerIndex = pidx;
		   	SetFightState(1);
		end;
		if (idx == 0) then
			break;
		end;
	end;
	PlayerIndex = OldPlayer;
end;