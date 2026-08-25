Include("\\script\\lib\\objbuffer_head.lua")
Include("\\script\\lib\\sharedata.lua")
Include("\\script\\missions\\basemission\\lib.lua")
Include("\\script\\activitysys\\playerfunlib.lua")
Include("\\script\\lib\\droptemplet.lua")
IncludeLib("LEAGUE")

BigBoss = {}

BigBoss.TSK_PLAYER_BOSSKILLED = 2598; -- ÕÊº“ª˜…±BOSS ˝¡øÕ≥º∆
BigBoss.TSK_BIGBOSS_REWARD = 2661; -- º«¬ºÕÊº“µ±ÃÏ «∑Ò¡ÏΩ±∫Õ¡ÏΩ±¿‡–Õ »’∆⁄ | ªÒµ√¥≥πÿ∑≠±∂ | ªÒµ√ÀŒΩ∑≠±∂ |ªÒµ√æ≠—È∑≠±∂Ω±¿¯ |  «∑Ò¡ÏΩ±
BigBoss.CAN_GET_REWARD_BIT = 1;
BigBoss.EXP_REWARD_BIT = 2;
BigBoss.SONGJIN_REWARD_BIT = 3;
BigBoss.CHUANGGUAN_REWARD_BIT = 4;

BigBoss.tbKillRecord = {};
BigBoss.IsBigBossDead = 0;
BigBoss.CallBackParam = {}

BigBoss.tbGlobalReward = 
{
	{szName="H´m nay trÀn TËng Kim 21:00, Æi”m t›ch lÚy sœ Æ≠Óc nh©n Æ´i", nRate=50, pFun=function() PlayerFunLib:SetTaskBit(BigBoss.TSK_BIGBOSS_REWARD, BigBoss.SONGJIN_REWARD_BIT, 1); Msg2Player(format("ßπi hi÷p nhÀn Æ≠Óc ph«n th≠Îng <color=yellow>%s<color>","H´m nay trÀn TËng Kim 21:00, Æi”m t›ch lÚy sœ Æ≠Óc nh©n Æ´i"))end},
	{szName="H´m nay v≠Ót ∂i ÆÓt 21:00, Æi”m kinh nghi÷m sœ Æ≠Óc nh©n Æ´i", nRate=50, pFun=function() PlayerFunLib:SetTaskBit(BigBoss.TSK_BIGBOSS_REWARD, BigBoss.CHUANGGUAN_REWARD_BIT, 1); Msg2Player(format("ßπi hi÷p nhÀn Æ≠Óc ph«n th≠Îng <color=yellow>%s<color>","H´m nay v≠Ót ∂i ÆÓt 21:00, Æi”m kinh nghi÷m sœ Æ≠Óc nh©n Æ´i"))end},
	--{szName="Nh©n Æ´i kinh nghi÷m khi Æ∏nh qu∏i trong 1 giÍ", nRate=25, pFun=function() AddSkillState(967, 1, 1, 64800); PlayerFunLib:SetTaskBit(BigBoss.TSK_BIGBOSS_REWARD, BigBoss.EXP_REWARD_BIT, 1); Msg2Player(format("ßπi hi÷p nhÀn Æ≠Óc ph«n th≠Îng <color=yellow>%s<color>","Nh©n Æ´i kinh nghi÷m khi Æ∏nh qu∏i trong 1 giÍ"))end},
	--{nExp = 10000000, nRate=25},
}

BigBoss.tbKillerReward = 
{
	{tbProp = {6,1,2136,1,0,0}, nCount=1, nExpiredTime=43200},
	{tbProp = {4,239,1,1,0,0}, nCount=10},
	{tbProp = {4,238,1,1,0,0}, nCount=10},
	{tbProp = {4,240,1,1,0,0}, nCount=10},
	{tbProp = {4,353,1,1,0,0}, nCount=20},
	{tbProp = {0,11,450,1,0,0}, nCount=1, nExpiredTime=10080},
	{tbProp = {6,1,908,1,0,0}, nCount=8, nExpiredTime=10080},
	{tbProp = {6,1,1076,1,0,0}, nCount=8},
	{tbProp = {6,1,2135,1,0,0}, nCount=1, nExpiredTime=10080},
}

BigBoss.tbNormalDrop = 
{
	{tbProp = {6,1,1076,1,0,0}, nCount=3},
	{tbProp = {4,239,1,1,0,0}, nCount=20},
	{tbProp = {4,238,1,1,0,0}, nCount=20},
	{tbProp = {4,240,1,1,0,0}, nCount=20},
	{tbProp = {4,353,1,1,0,0}, nCount=20},
	{tbProp = {6,1,1681,1,0,0}, nCount=10},
	{tbProp = {0,11,450,1,0,0}, nCount=1, nExpiredTime=10080},
	{tbProp = {6,1,2124,1,0,0}, nCount=10},
	{tbProp = {6,1,2135,1,0,0}, nCount=10},
	{tbProp = {6,0,6,1,0,0}, nCount=20},
	{tbProp = {6,0,3,1,0,0}, nCount=20},
	{tbProp = {6,1,71,1,0,0}, nCount=20},
	{tbProp = {6,1,1182,1,0,0}, nCount=10},
	{tbProp = {6,1,26,1,0,0}, nCount=10},
	{tbProp = {6,1,22,1,0,0}, nCount=10},
}
--Modifiled by:PhucNG - 20160421 - change award from this boss [Æπi hoµng kim] ßÈc C´ Thi™n Phong
BigBoss.tbVngNewAward = 
{
	[1] = {--Ph«n 1
		{szName="K‹ n®ng c p 150 c p 22",tbProp={6,1,4755,1,0,0},nCount=1,nRate=45},
		{szName="Cµn Kh´n Song Tuy÷t BÈi",tbProp={6,1,2228,1,0,0},nCount=1,nRate=25,nExpiredTime=43200},
		{szName="ßπi Thµnh B› K›p 150",tbProp={6,1,30446,1,0,0},nCount=1,nRate=0.7},
		{szName="Thi™n S¨n Th∏nh ThÒy (Æπi)",tbProp={6,1,30449,1,0,0},nCount=1,nRate=7.95,nExpiredTime=43200},
		{szName="Trang S¯c Tinh Linh",tbProp={6,1,30563,1,0,0},nCount=1,nRate=3,nExpiredTime=10080},
		{szName="Trang S¯c Chi Quang",tbProp={6,1,30593,1,0,0},nCount=1,nRate=6,nExpiredTime=10080},
		{szName="ßπi Gia Hπn PhÔ",tbProp={6,1,30408,1,0,0},nCount=1,nRate=3,nExpiredTime=43200},
	},
	[2] = {--Ph«n 3
		{szName="M∆t nπ chi’n tr≠Íng V≠¨ng Gi∂",tbProp={0,11,647,1,0,0},nRate=60,nCount=1,nExpiredTime=10080},
		{szName="M∆t nπ chi’n tr≠Íng B∏ Gi∂",tbProp={0,11,828,1,0,0},nCount=1,nRate=40,nExpiredTime=10080},
	},
	[3] = {--Ph«n 4
		{szName="Phi phong Ng˘ Phong (Tr‰ng k›ch)",tbProp={0,3475},nCount=1,nRate=25,nQuality = 1,nExpiredTime=10080,},
		{szName="Phi phong Ng˘ Phong (X∏c su t h„a gi∂i s∏t th≠¨ng)",tbProp={0,3474},nCount=1,nRate=25,nQuality = 1,nExpiredTime=10080,},
		{szName="Phi Phong Ph÷ Quang ( tr‰ng k›ch)",tbProp={0,3478},nCount=1,nRate=25,nQuality = 1,nExpiredTime=10080,},
		{szName="Phi Phong C p Ph÷ Quang (h„a gi∂i s∏t th≠¨ng)",tbProp={0,3477},nCount=1,nRate=25,nQuality = 1,nExpiredTime=10080,},
	},
	[4] = {--Ph«n 5
		{szName="TÛi D≠Óc Ph»m",tbProp={6,1,30557,1,0,0},nCount=5,nRate=20},
		{szName="HÂi thi™n t∏i tπo l‘ bao",tbProp={6,1,2536,1,0,0},nCount=1,nRate=20},
		{szName="Cµn Kh´n Tπo H„a ßan (Æπi) ",tbProp={6,1,214,1,0,0},nCount=5,nRate=20,CallBack=function(nItemIndex) SetItemStackCount(nItemIndex, 10) end},
		{szName="K›ch C´ng TrÓ L˘c Hoµn",tbProp={6,1,3570,1,0,0},nCount=3,nRate=20},
		{szName="¢m D≠¨ng Hoπt Huy’t ß¨n",tbProp={6,1,3571,1,0,0},nCount=3,nRate=20},
	},
	[5] = {--Ph«n 3 -> Bang HÈi
		{szName="CËng Hi’n ßπi L‘ Bao",tbProp={6,1,30215,1,0,0},nCount=10,nRate=40,nExpiredTime=10080},
		{szName="Ki’n Thi’t L‘ Bao",tbProp={6,1,30216,1,0,0},nCount=10,nRate=30,nExpiredTime=10080},
		{szName="Chi’n Bﬁ L‘ Bao",tbProp={6,1,30218,1,0,0},nCount=10,nRate=30,nExpiredTime=10080},
	},
	[6] = {--Ph«n 4 -> PET, EXP
		{szName="ThuËc t®ng tr≠Îng",tbProp={6,1,4072,1,0,0},nCount=5,nRate=10},
		{szName="Qu∂ Hoµng Kim",tbProp={6,1,908,1,0,0},nCount=5,nRate=10,nExpiredTime=10080},
		{szName="Thi™n Linh ß¨n",tbProp={6,1,3640,1,0,0},nCount=10,nRate=50,nExpiredTime=10080},
	},
	[7] = {--Ph«n 5 Ph«n th≠Îng ßªng Long
		{szName="ßÂ PhÊ ßªng Long Kh´i",tbProp={6,1,30528,1,0,0},nCount=1,nRate=1},
		{szName="ßÂ PhÊ ßªng Long Hµi",tbProp={6,1,30530,1,0,0},nCount=1,nRate=1},
		{szName="ßÂ PhÊ ßªng Long Y™u ß∏i",tbProp={6,1,30531,1,0,0},nCount=1,nRate=1},
		{szName="ßÂ PhÊ ßªng Long HÈ Uy”n",tbProp={6,1,30532,1,0,0},nCount=1,nRate=1},
		{szName="ßÂ PhÊ ßªng Long Hπng Li™n",tbProp={6,1,30533,1,0,0},nCount=1,nRate=1},
		{szName="ßÂ PhÊ ßªng Long BÈi",tbProp={6,1,30534,1,0,0},nCount=1,nRate=1},
		{szName="ßÂ PhÊ ßªng Long Th≠Óng GiÌi Chÿ",tbProp={6,1,30535,1,0,0},nCount=1,nRate=1},
		{szName="ßÂ PhÊ ßªng Long Hπ GiÌi Chÿ",tbProp={6,1,30536,1,0,0},nCount=1,nRate=1},
		{szName="ßªng Long Thπch - Hπ",tbProp={6,1,30538,1,0,0},nCount=10,nRate=77},
		{szName="Tinh S≠¨ng L÷nh",tbProp={6,1,30506,1,0,0},nCount=1,nRate=2},
		{szName="Huy“n Thi’t",tbProp={6,1,30507,1,0,0},nCount=1,nRate=3},
	},
	[8] = {--Ph«n 4
		{szName="NgÚ Hµnh  n 6",tbProp={0,3230},nCount=1,nRate=10,nQuality = 1,nExpiredTime=10080,},
		{szName="NgÚ Hµnh  n 7",tbProp={0,3231},nCount=1,nRate=10,nQuality = 1,nExpiredTime=10080,},
		{szName="NgÚ Hµnh  n 8",tbProp={0,3232},nCount=1,nRate=5,nQuality = 1,nExpiredTime=10080,},
		{szName="NgÚ Hµnh  n 9",tbProp={0,3233},nCount=1,nRate=5,nQuality = 1,nExpiredTime=10080,},
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
	
	-- ±£¡Ù◊Ó∫√≥…º®
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
				PlayerFunLib:AddTaskDaily(BigBoss.TSK_BIGBOSS_REWARD, 0);	-- ÷ÿ÷√±‰¡ø,“‘∑¿“‚Õ‚
				if (PlayerFunLib:CheckTaskBit(BigBoss.TSK_BIGBOSS_REWARD, BigBoss.CAN_GET_REWARD_BIT, 1, "H´m nay Æπi hi÷p Æ∑ nhÀn th≠Îng rÂi!") == 1) then
					PlayerFunLib:SetTaskBit(BigBoss.TSK_BIGBOSS_REWARD, BigBoss.CAN_GET_REWARD_BIT, 0);
					tbAwardTemplet:GiveAwardByList(BigBoss.tbGlobalReward, format("[%s] big boss global reward",date("%Y%m%d")));
				end
			else
				Talk(1, "", format("H´m nay v…n ch≠a Æ∏nh bπi <color=red>%s<color>","ßÈc C´ Thi™n Phong"));
			end
		end
		--
		PlayerIndex = OldPlayerIndex
		BigBoss.CallBackParam[Param] = nil
	end
end

function BigBoss:BigBossDeath(nNpcIndex)
	-- º«¬º ±º‰
	local nTime = tonumber(GetLocalDate("%H%M%S"))-194500; -- À¿Õˆ ±º‰ - ’ŸªΩ ±º‰
	
	-- ∏¯◊Ó∏ﬂ…À∫¶µƒ»ÀªÚ∂”ŒÈΩ±¿¯
	local nTeamSize = GetTeamSize();
	local szName;
	
	if (nTeamSize > 1) then
		for i=1,nTeamSize do
			if(doFunByPlayer(GetTeamMember(i), IsCaptain)==1)then
				szName = doFunByPlayer(GetTeamMember(i), GetName);
			end
			doFunByPlayer(GetTeamMember(i), PlayerFunLib.AddExp, PlayerFunLib, 100000000, 0, format("%s ph«n th≠Îng","Ph«n th≠Îng kinh nghi÷m cho ng≠Íi c„ c´ng k›ch mπnh nh t vµo ßÈc C´ Thi™n Phong"));
		end
	else -- “ª∏ˆ»À
		szName = GetName();
		PlayerFunLib:AddExp(100000000, 0, format("%s ph«n th≠Îng","Ph«n th≠Îng kinh nghi÷m cho ng≠Íi c„ c´ng k›ch mπnh nh t vµo ßÈc C´ Thi™n Phong"));
	end
	
	local tbRoundPlayer, nCount = GetNpcAroundPlayerList(nNpcIndex, 20);
	
	for i=1,nCount do
		doFunByPlayer(tbRoundPlayer[i], PlayerFunLib.AddExp, PlayerFunLib, 50000000, 0, format("%s ph«n th≠Îng","Ph«n th≠Îng kinh nghi÷m cho ng≠Íi Æ¯ng g«n khi Æ∂ bπi ßÈc C´ Thi™n Phong"));
	end
	
	--tbDropTemplet:GiveAwardByList(nNpcIndex, PlayerIndex, self.tbKillerReward, format("%sµÙ¬‰","∂¿π¬ÃÏ∑Â"), 1);
	
	--tbDropTemplet:GiveAwardByList(nNpcIndex, -1, self.tbNormalDrop, format("%sµÙ¬‰","∂¿π¬ÃÏ∑Â"), 1);
		tbDropTemplet:GiveAwardByList(nNpcIndex, PlayerIndex, self.tbVngNewAward, format("%s rÌt","ßÈc C´ Thi™n Phong"), 1);
	-- BOSSª˜…±Õ≥º∆
	local nCount = GetTask(self.TSK_PLAYER_BOSSKILLED);
	nCount = nCount + 1;
	SetTask(self.TSK_PLAYER_BOSSKILLED, nCount);
	
	-- BIGBOSSÀ¿Õˆ
	local Handle = OB_Create()
	if (Handle > 0) then
		ObjBuffer:PushObject(Handle, 1)
		RemoteExecute("\\script\\mission\\boss\\bigboss.lua", "SetBigBossDead", Handle);
		OB_Release(Handle)
	end
	
	local szNews = format("TÊ ÆÈi <color=yellow>%s<color> Æ∑ ti™u di÷t thµnh c´ng  <color=yellow>ßÈc C´ Thi™n Phong<color>, h∑y nhanh ch„ng Æ’n l‘ quan nhÀn th≠Îng!",szName);
	AddGlobalNews(szNews);
	LG_ApplyDoScript(1, "", "", "\\script\\event\\msg2allworld.lua", "battle_msg2allworld", szNews , "", "");
	
	self:AddKillRecord(szName, nTime);
end

function BigBoss:RemoveSongJinBonus()
	PlayerFunLib:AddTaskDaily(self.TSK_BIGBOSS_REWARD, 0);	-- ÷ÿ÷√±‰¡ø,“‘∑¿“‚Õ‚
	PlayerFunLib:SetTaskBit(self.TSK_BIGBOSS_REWARD, self.SONGJIN_REWARD_BIT, 0);
end

function BigBoss:RemoveChuangGuanBonus()
	PlayerFunLib:AddTaskDaily(self.TSK_BIGBOSS_REWARD, 0);	-- ÷ÿ÷√±‰¡ø,“‘∑¿“‚Õ‚
	PlayerFunLib:SetTaskBit(self.TSK_BIGBOSS_REWARD, self.CHUANGGUAN_REWARD_BIT, 0);
end

function BigBoss:AddSongJinPoint(nBasePoint)
	PlayerFunLib:AddTaskDaily(self.TSK_BIGBOSS_REWARD, 0);	-- ÷ÿ÷√±‰¡ø,“‘∑¿“‚Õ‚
	local nType = GetBit(PlayerFunLib:GetActivityTask(self.TSK_BIGBOSS_REWARD), self.SONGJIN_REWARD_BIT);
	if (nType == 1) then
		local nHour = tonumber(GetLocalDate("%H"));
		if (nHour <= 22) then -- 21µ„µƒÀŒΩ22:30Ω· ¯
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
	PlayerFunLib:AddTaskDaily(self.TSK_BIGBOSS_REWARD, 0);	-- ÷ÿ÷√±‰¡ø,“‘∑¿“‚Õ‚
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