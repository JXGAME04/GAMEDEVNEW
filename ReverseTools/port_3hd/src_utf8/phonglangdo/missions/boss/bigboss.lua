Include("\\script\\lib\\objbuffer_head.lua")
Include("\\script\\lib\\sharedata.lua")
Include("\\script\\missions\\basemission\\lib.lua")
Include("\\script\\activitysys\\playerfunlib.lua")
Include("\\script\\lib\\droptemplet.lua")
IncludeLib("LEAGUE")

BigBoss = {}

BigBoss.TSK_PLAYER_BOSSKILLED = 2598; -- 玩家击杀BOSS数量统计
BigBoss.TSK_BIGBOSS_REWARD = 2661; -- 记录玩家当天是否领奖和领奖类型 日期 | 获得闯关翻倍 | 获得宋金翻倍 |获得经验翻倍奖励 | 是否领奖
BigBoss.CAN_GET_REWARD_BIT = 1;
BigBoss.EXP_REWARD_BIT = 2;
BigBoss.SONGJIN_REWARD_BIT = 3;
BigBoss.CHUANGGUAN_REWARD_BIT = 4;

BigBoss.tbKillRecord = {};
BigBoss.IsBigBossDead = 0;
BigBoss.CallBackParam = {}

BigBoss.tbGlobalReward = 
{
	{szName="Hôm nay trận Tống Kim 21:00, điểm tích lũy sẽ được nhân đôi", nRate=50, pFun=function() PlayerFunLib:SetTaskBit(BigBoss.TSK_BIGBOSS_REWARD, BigBoss.SONGJIN_REWARD_BIT, 1); Msg2Player(format("Đại hiệp nhận được phần thưởng <color=yellow>%s<color>","Hôm nay trận Tống Kim 21:00, điểm tích lũy sẽ được nhân đôi"))end},
	{szName="Hôm nay vượt ải đợt 21:00, điểm kinh nghiệm sẽ được nhân đôi", nRate=50, pFun=function() PlayerFunLib:SetTaskBit(BigBoss.TSK_BIGBOSS_REWARD, BigBoss.CHUANGGUAN_REWARD_BIT, 1); Msg2Player(format("Đại hiệp nhận được phần thưởng <color=yellow>%s<color>","Hôm nay vượt ải đợt 21:00, điểm kinh nghiệm sẽ được nhân đôi"))end},
	--{szName="Nhân đôi kinh nghiệm khi đánh quái trong 1 giờ", nRate=25, pFun=function() AddSkillState(967, 1, 1, 64800); PlayerFunLib:SetTaskBit(BigBoss.TSK_BIGBOSS_REWARD, BigBoss.EXP_REWARD_BIT, 1); Msg2Player(format("Đại hiệp nhận được phần thưởng <color=yellow>%s<color>","Nhân đôi kinh nghiệm khi đánh quái trong 1 giờ"))end},
	--{nExp = 10000000, nRate=25},
}

BigBoss.tbKillerReward = 
{
	{tbProp = {6,1,2127,1,0,0}, nCount=1, nExpiredTime=43200},
	{tbProp = {4,239,1,1,0,0}, nCount=10},
	{tbProp = {4,238,1,1,0,0}, nCount=10},
	{tbProp = {4,240,1,1,0,0}, nCount=10},
	{tbProp = {4,353,1,1,0,0}, nCount=20},
	{tbProp = {0,11,450,1,0,0}, nCount=1, nExpiredTime=10080},
	{tbProp = {6,1,907,1,0,0}, nCount=8, nExpiredTime=10080},
	{tbProp = {6,1,1075,1,0,0}, nCount=8},
	{tbProp = {6,1,2126,1,0,0}, nCount=1, nExpiredTime=10080},
}

BigBoss.tbNormalDrop = 
{
	{tbProp = {6,1,1075,1,0,0}, nCount=3},
	{tbProp = {4,239,1,1,0,0}, nCount=20},
	{tbProp = {4,238,1,1,0,0}, nCount=20},
	{tbProp = {4,240,1,1,0,0}, nCount=20},
	{tbProp = {4,353,1,1,0,0}, nCount=20},
	{tbProp = {6,1,1672,1,0,0}, nCount=10},
	{tbProp = {0,11,450,1,0,0}, nCount=1, nExpiredTime=10080},
	{tbProp = {6,1,2115,1,0,0}, nCount=10},
	{tbProp = {6,1,2117,1,0,0}, nCount=10},
	{tbProp = {6,0,6,1,0,0}, nCount=20},
	{tbProp = {6,0,3,1,0,0}, nCount=20},
	{tbProp = {6,1,71,1,0,0}, nCount=20},
	{tbProp = {6,1,1765,1,0,0}, nCount=10},
	{tbProp = {6,1,26,1,0,0}, nCount=10},
	{tbProp = {6,1,22,1,0,0}, nCount=10},
}
--Modifiled by:PhucNG - 20160421 - change award from this boss [đại hoàng kim] Độc Cô Thiên Phong
BigBoss.tbVngNewAward = 
{
	[1] = {--Phần 1
		{szName="Kĩ năng cấp 150 cấp 22",tbProp={6,1,4149,1,0,0},nCount=1,nRate=45},
		{szName="Càn Khôn Song Tuyệt Bội",tbProp={6,1,2219,1,0,0},nCount=1,nRate=25,nExpiredTime=43200},
		{szName="Đại Thành Bí Kíp 150",tbProp={6,1,30446,1,0,0},nCount=1,nRate=0.7},
		{szName="Thiên Sơn Thánh Thủy (đại)",tbProp={6,1,30449,1,0,0},nCount=1,nRate=7.95,nExpiredTime=43200},
		{szName="Trang Sức Tinh Linh",tbProp={6,1,30563,1,0,0},nCount=1,nRate=3,nExpiredTime=10080},
		{szName="Trang Sức Chi Quang",tbProp={6,1,30593,1,0,0},nCount=1,nRate=6,nExpiredTime=10080},
		{szName="Đại Gia Hạn Phù",tbProp={6,1,30408,1,0,0},nCount=1,nRate=3,nExpiredTime=43200},
	},
	[2] = {--Phần 3
		{szName="Mặt nạ chiến trường Vương Giả",tbProp={0,11,647,1,0,0},nRate=60,nCount=1,nExpiredTime=10080},
		{szName="Mặt nạ chiến trường Bá Giả",tbProp={0,11,828,1,0,0},nCount=1,nRate=40,nExpiredTime=10080},
	},
	[3] = {--Phần 4
		{szName="Phi phong Ngự Phong (Trọng kích)",tbProp={0,3475},nCount=1,nRate=25,nQuality = 1,nExpiredTime=10080,},
		{szName="Phi phong Ngự Phong (Xác suất hóa giải sát thương)",tbProp={0,3474},nCount=1,nRate=25,nQuality = 1,nExpiredTime=10080,},
		{szName="Phi Phong Phệ Quang ( trọng kích)",tbProp={0,3478},nCount=1,nRate=25,nQuality = 1,nExpiredTime=10080,},
		{szName="Phi Phong Cấp Phệ Quang (hóa giải sát thương)",tbProp={0,3477},nCount=1,nRate=25,nQuality = 1,nExpiredTime=10080,},
	},
	[4] = {--Phần 5
		{szName="Túi Dược Phẩm",tbProp={6,1,30557,1,0,0},nCount=5,nRate=20},
		{szName="Hồi thiên tái tạo lễ bao",tbProp={6,1,2527,1,0,0},nCount=1,nRate=20},
		{szName="Càn Khôn Tạo Hóa Đan (đại) ",tbProp={6,1,215,1,0,0},nCount=5,nRate=20,CallBack=function(nItemIndex) SetItemStackCount(nItemIndex, 10) end},
		{szName="Kích Công Trợ Lực Hoàn",tbProp={6,1,2952,1,0,0},nCount=3,nRate=20},
		{szName="Âm Dương Hoạt Huyết Đơn",tbProp={6,1,2953,1,0,0},nCount=3,nRate=20},
	},
	[5] = {--Phần 3 -> Bang Hội
		{szName="Cống Hiến Đại Lễ Bao",tbProp={6,1,30215,1,0,0},nCount=10,nRate=40,nExpiredTime=10080},
		{szName="Kiến Thiết Lễ Bao",tbProp={6,1,30216,1,0,0},nCount=10,nRate=30,nExpiredTime=10080},
		{szName="Chiến Bị Lễ Bao",tbProp={6,1,30218,1,0,0},nCount=10,nRate=30,nExpiredTime=10080},
	},
	[6] = {--Phần 4 -> PET, EXP
		{szName="Thuốc tăng trưởng",tbProp={6,1,3454,1,0,0},nCount=5,nRate=10},
		{szName="Quả Hoàng Kim",tbProp={6,1,907,1,0,0},nCount=5,nRate=10,nExpiredTime=10080},
		{szName="Thiên Linh Đơn",tbProp={6,1,3022,1,0,0},nCount=10,nRate=50,nExpiredTime=10080},
	},
	[7] = {--Phần 5 Phần thưởng Đằng Long
		{szName="Đồ Phổ Đằng Long Khôi",tbProp={6,1,30528,1,0,0},nCount=1,nRate=1},
		{szName="Đồ Phổ Đằng Long Hài",tbProp={6,1,30530,1,0,0},nCount=1,nRate=1},
		{szName="Đồ Phổ Đằng Long Yêu Đái",tbProp={6,1,30531,1,0,0},nCount=1,nRate=1},
		{szName="Đồ Phổ Đằng Long Hộ Uyển",tbProp={6,1,30532,1,0,0},nCount=1,nRate=1},
		{szName="Đồ Phổ Đằng Long Hạng Liên",tbProp={6,1,30533,1,0,0},nCount=1,nRate=1},
		{szName="Đồ Phổ Đằng Long Bội",tbProp={6,1,30534,1,0,0},nCount=1,nRate=1},
		{szName="Đồ Phổ Đằng Long Thượng Giới Chỉ",tbProp={6,1,30535,1,0,0},nCount=1,nRate=1},
		{szName="Đồ Phổ Đằng Long Hạ Giới Chỉ",tbProp={6,1,30536,1,0,0},nCount=1,nRate=1},
		{szName="Đằng Long Thạch - Hạ",tbProp={6,1,30538,1,0,0},nCount=10,nRate=77},
		{szName="Tinh Sương Lệnh",tbProp={6,1,30506,1,0,0},nCount=1,nRate=2},
		{szName="Huyền Thiết",tbProp={6,1,30507,1,0,0},nCount=1,nRate=3},
	},
	[8] = {--Phần 4
		{szName="Ngũ Hành ấn 6",tbProp={0,3230},nCount=1,nRate=10,nQuality = 1,nExpiredTime=10080,},
		{szName="Ngũ Hành ấn 7",tbProp={0,3231},nCount=1,nRate=10,nQuality = 1,nExpiredTime=10080,},
		{szName="Ngũ Hành ấn 8",tbProp={0,3232},nCount=1,nRate=5,nQuality = 1,nExpiredTime=10080,},
		{szName="Ngũ Hành ấn 9",tbProp={0,3233},nCount=1,nRate=5,nQuality = 1,nExpiredTime=10080,},
	},
}

function AddKillRecord_CallBack(Param, ResultHandle)
	szName = BigBoss.CallBackParam[Param][1];
	nSecondes = BigBoss.CallBackParam[Param][2];
	BigBoss.CallBackParam[Param] = nil;
	if (OB_IsEmpty(ResultHandle) == 0) then
		BigBoss.tbKillRecord = ObjBuffer:PopObject(ResultHandle);
	end
	
	local nIdx = 0;
	
	local nIdx = 0;
	for i=1,getn(BigBoss.tbKillRecord) do
		if (BigBoss.tbKillRecord[i][2] > nSecondes) then
			nIdx = i;
			break;
		end
	end
	
	if (nIdx == 0) then
		nIdx = getn(BigBoss.tbKillRecord) + 1;
	end
	
	tinsert(BigBoss.tbKillRecord, nIdx, {szName, nSecondes});
	
	local nCount = 0;
	
	-- 保留最好成绩
	for i=1,getn(BigBoss.tbKillRecord) do
		if (BigBoss.tbKillRecord[i] and BigBoss.tbKillRecord[i][1] == szName) then
			nCount = nCount + 1;
			if (nCount > 1) then
				tremove(BigBoss.tbKillRecord, nIdx);
			end
		end
	end
	
	SaveShareData("FUNC_BIGBOSS_LADDER", 0, 0, BigBoss.tbKillRecord);
end

function BigBoss:AddKillRecord(szName, nSecondes)
	self.CallBackParam[getn(self.CallBackParam)+1] = {szName, nSecondes};
	
	LoadShareData("FUNC_BIGBOSS_LADDER", 0, 0, "AddKillRecord_CallBack", getn(self.CallBackParam));
end

function BigBoss:GetKillRecord(szCallBack, nParam)
	nParam = nParam or 0;
	LoadShareData("FUNC_BIGBOSS_LADDER", 0, 0, szCallBack, nParam);
end

function BigBoss:BigBossGlobalReward()
	self.CallBackParam[getn(self.CallBackParam) + 1] = PlayerIndex;
	RemoteExecute("\\script\\mission\\boss\\bigboss.lua", "IsBigBossDead", 0, "BigBossGlobalReward_CallBack", getn(self.CallBackParam));
end

function BigBossGlobalReward_CallBack(Param, ResultHandle)
	local Index = BigBoss.CallBackParam[Param]
	if (Index and Index > 0) then
		local OldPlayerIndex = PlayerIndex
		PlayerIndex = Index
		if (OB_IsEmpty(ResultHandle) == 0) then
			IsBigBossDead = ObjBuffer:PopObject(ResultHandle);
			if (IsBigBossDead == 1) then
				PlayerFunLib:AddTaskDaily(BigBoss.TSK_BIGBOSS_REWARD, 0);	-- 重置变量,以防意外
				if (PlayerFunLib:CheckTaskBit(BigBoss.TSK_BIGBOSS_REWARD, BigBoss.CAN_GET_REWARD_BIT, 1, "Hôm nay đại hiệp đã nhận thưởng rồi!") == 1) then
					PlayerFunLib:SetTaskBit(BigBoss.TSK_BIGBOSS_REWARD, BigBoss.CAN_GET_REWARD_BIT, 0);
					tbAwardTemplet:GiveAwardByList(BigBoss.tbGlobalReward, format("[%s] big boss global reward",date("%Y%m%d")));
				end
			else
				Talk(1, "", format("Hôm nay vẫn chưa đánh bại <color=red>%s<color>","Độc Cô Thiên Phong"));
			end
		end
		--
		PlayerIndex = OldPlayerIndex
		BigBoss.CallBackParam[Param] = nil
	end
end

function BigBoss:BigBossDeath(nNpcIndex)
	-- 记录时间
	local nTime = tonumber(GetLocalDate("%H%M%S"))-194500; -- 死亡时间 - 召唤时间
	
	-- 给最高伤害的人或队伍奖励
	local nTeamSize = GetTeamSize();
	local szName;
	
	if (nTeamSize > 1) then
		for i=1,nTeamSize do
			if(doFunByPlayer(GetTeamMember(i), IsCaptain)==1)then
				szName = doFunByPlayer(GetTeamMember(i), GetName);
			end
			doFunByPlayer(GetTeamMember(i), PlayerFunLib.AddExp, PlayerFunLib, 100000000, 0, format("%s phần thưởng","Phần thưởng kinh nghiệm cho người có công kích mạnh nhất vào Độc Cô Thiên Phong"));
		end
	else -- 一个人
		szName = GetName();
		PlayerFunLib:AddExp(100000000, 0, format("%s phần thưởng","Phần thưởng kinh nghiệm cho người có công kích mạnh nhất vào Độc Cô Thiên Phong"));
	end
	
	local tbRoundPlayer, nCount = GetNpcAroundPlayerList(nNpcIndex, 20);
	
	for i=1,nCount do
		doFunByPlayer(tbRoundPlayer[i], PlayerFunLib.AddExp, PlayerFunLib, 50000000, 0, format("%s phần thưởng","Phần thưởng kinh nghiệm cho người đứng gần khi đả bại Độc Cô Thiên Phong"));
	end
	
	--tbDropTemplet:GiveAwardByList(nNpcIndex, PlayerIndex, self.tbKillerReward, format("%s掉落","独孤天峰"), 1);
	
	--tbDropTemplet:GiveAwardByList(nNpcIndex, -1, self.tbNormalDrop, format("%s掉落","独孤天峰"), 1);
		tbDropTemplet:GiveAwardByList(nNpcIndex, PlayerIndex, self.tbVngNewAward, format("%s rớt","Độc Cô Thiên Phong"), 1);
	-- BOSS击杀统计
	local nCount = GetTask(self.TSK_PLAYER_BOSSKILLED);
	nCount = nCount + 1;
	SetTask(self.TSK_PLAYER_BOSSKILLED, nCount);
	
	-- BIGBOSS死亡
	local Handle = OB_Create()
	if (Handle > 0) then
		ObjBuffer:PushObject(Handle, 1)
		RemoteExecute("\\script\\mission\\boss\\bigboss.lua", "SetBigBossDead", Handle);
		OB_Release(Handle)
	end
	
	local szNews = format("Tổ đội <color=yellow>%s<color> đã tiêu diệt thành công  <color=yellow>Độc Cô Thiên Phong<color>, hãy nhanh chóng đến lễ quan nhận thưởng!",szName);
	AddGlobalNews(szNews);
	LG_ApplyDoScript(1, "", "", "\\script\\event\\msg2allworld.lua", "battle_msg2allworld", szNews , "", "");
	
	self:AddKillRecord(szName, nTime);
end

function BigBoss:RemoveSongJinBonus()
	PlayerFunLib:AddTaskDaily(self.TSK_BIGBOSS_REWARD, 0);	-- 重置变量,以防意外
	PlayerFunLib:SetTaskBit(self.TSK_BIGBOSS_REWARD, self.SONGJIN_REWARD_BIT, 0);
end

function BigBoss:RemoveChuangGuanBonus()
	PlayerFunLib:AddTaskDaily(self.TSK_BIGBOSS_REWARD, 0);	-- 重置变量,以防意外
	PlayerFunLib:SetTaskBit(self.TSK_BIGBOSS_REWARD, self.CHUANGGUAN_REWARD_BIT, 0);
end

function BigBoss:AddSongJinPoint(nBasePoint)
	PlayerFunLib:AddTaskDaily(self.TSK_BIGBOSS_REWARD, 0);	-- 重置变量,以防意外
	local nType = GetBit(PlayerFunLib:GetActivityTask(self.TSK_BIGBOSS_REWARD), self.SONGJIN_REWARD_BIT);
	if (nType == 1) then
		local nHour = tonumber(GetLocalDate("%H"));
		if (nHour <= 22) then -- 21点的宋金22:30结束
			nBasePoint = nBasePoint * 2;
		end
		
	end
	local nNowDate = tonumber(GetLocalDate("%Y%m%d%H%M"))
	if nNowDate >= 201201200000 and nNowDate < 201201252400 and nType ~= 1 then
		nBasePoint = nBasePoint * 2;
	end
	return nBasePoint;
end

function BigBoss:AddChuangGuanPoint(nBasePoint)
	PlayerFunLib:AddTaskDaily(self.TSK_BIGBOSS_REWARD, 0);	-- 重置变量,以防意外
	local nType = GetBit(PlayerFunLib:GetActivityTask(self.TSK_BIGBOSS_REWARD), self.CHUANGGUAN_REWARD_BIT);
	if (nType == 1) then
		nBasePoint = nBasePoint * 2;
	end
	local nNowDate = tonumber(GetLocalDate("%Y%m%d%H%M"))
	if nNowDate >= 201201200000 and nNowDate < 201201252400 and nType ~= 1 then
		nBasePoint = nBasePoint * 2;
	end
	return nBasePoint;
end

function BigBoss:MakeAllPlayerCanGetReward()
	local nIdx = GetFirstPlayerAtServer();
	while(nIdx > 0) do
		doFunByPlayer(nIdx, PlayerFunLib.AddTaskDaily, PlayerFunLib, self.TSK_BIGBOSS_REWARD, 0);
		doFunByPlayer(nIdx, PlayerFunLib.SetTaskBit, PlayerFunLib, self.TSK_BIGBOSS_REWARD, self.CAN_GET_REWARD_BIT, 1);
		nIdx = GetNextPlayerAtServer();
	end
end