if 1 ~= MODEL_GAMECLIENT then
	return
end

Include("\\script\\ui\\manage.lua")


local tbClass = {}
tbClass.UIGROUP	= "UI_GREATTEAMFIGHT_FIELDREPORT"
tbClass.UICOMP	= {
	Countdown = "Countdown",
	StageName = "StageName",
	Remain = "Remain",
	Members = "Members",

	SchemeName = "SchemeName",
	FightRound = "FightRound",
}

tbClass.tbStage = {
	szName = "",
	nEndTime = 0,
	szSchemeName = "",
	nCurFightTimes = 0,
	nTotalFightTimes = 0,
}

tbClass.tbDesc = {
	["B¸o danh"] = "L­u ı:<enter> C¸c thµnh viªn tæ ®éi tham chiÕn sÏ ®­îc vµo ®Êu tr­êng.<enter> ChiÕn ®éi tham gia §Êu §iÓm c¸ nh©n, hoÆc tæ ®éi nh­ng kh«ng ®ñ 3 ng­êi, hÖ thèng tù ®éng ghĞp ®éi tr­íc thi ®Êu.<enter> Thµnh viªn chiÕn ®éi Tranh B¸ lÊy ®éi h×nh ghĞp trËn ®Çu lµm chuÈn, kh«ng thay ®æi, ®éi thua sÏ bŞ lo¹i!",
	["Thi ®Êu"] = "L­u ı:<enter> C¸c ®éi cã sè trËn th¾ng trong §Êu §iÓm b»ng nhau, sÏ ­u tiªn ghĞp ®Êu víi nhau.<enter> §éi th¾ng trËn Tranh B¸ sÏ ®Êu víi nhau, ®éi thua sÏ bŞ lo¹i, kh«ng thÓ tiÕp tôc thi ®Êu.",
	["NghØ ng¬i"] = "L­u ı:<enter> C¸c ®éi cã sè trËn th¾ng trong §Êu §iÓm b»ng nhau, sÏ ­u tiªn ghĞp ®Êu víi nhau.<enter> §éi th¾ng trËn Tranh B¸ sÏ ®Êu víi nhau, ®éi thua sÏ bŞ lo¹i, kh«ng thÓ tiÕp tôc thi ®Êu.",
	["KÕt thóc"] = "Chó ı:<enter>- §Êu §iÓm Thø 3 kÕt thóc, Bang Chñ hoÆc Tr­ëng L·o cã thÓ ®Õn L©m An LuËn KiÕm Qu¶n Sù nhËn th­ëng h¹ng ®iÓm Bang!<enter>- Th­ëng §Êu Tranh B¸ göi qua th­, h·y chó ı nhËn!",
	["Giai ®o¹n chuÈn bŞ"] = "L­u ı:<enter> §õng rêi khái khu vùc thi ®Êu, nÕu kh«ng sÏ kh«ng nhËn ®­îc th­ëng §Êu §iÓm!<enter> NÕu chiÕn ®éi Tranh B¸ bŞ lo¹i, phÇn th­ëng ho¹t ®éng sÏ ®­îc göi qua th­, h·y chó ı nhËn!",
	["ChiÕn ®Êu nöa ®Çu trËn"] = "L­u ı:<enter> §õng rêi khái khu vùc thi ®Êu, nÕu kh«ng sÏ kh«ng nhËn ®­îc th­ëng §Êu §iÓm!<enter> NÕu chiÕn ®éi Tranh B¸ bŞ lo¹i, phÇn th­ëng ho¹t ®éng sÏ ®­îc göi qua th­, h·y chó ı nhËn!",
	["NghØ gi÷a hiÖp"] = "L­u ı:<enter> H·y nghØ ng¬i, chuÈn bŞ cho trËn sau!<enter> Rêi khái khu vùc thi ®Êu sÏ kh«ng nhËn ®­îc th­ëng §Êu §iÓm!<enter> NÕu chiÕn ®éi Tranh B¸ bŞ lo¹i, phÇn th­ëng ho¹t ®éng sÏ ®­îc göi qua th­!",
	["ChiÕn ®Êu nöa cuèi trËn"] = "L­u ı:<enter> §õng rêi khái khu vùc thi ®Êu, nÕu kh«ng sÏ kh«ng nhËn ®­îc th­ëng §Êu §iÓm!<enter> NÕu chiÕn ®éi Tranh B¸ bŞ lo¹i, phÇn th­ëng ho¹t ®éng sÏ ®­îc göi qua th­, h·y chó ı nhËn!",
}

tbClass.tbControls = {}

function tbClass:OnCreate()
	self.tbControls.Countdown = WndText:CreateByInstance(self.UIGROUP, self.UICOMP.Countdown);
	self.tbControls.StageName = WndText:CreateByInstance(self.UIGROUP, self.UICOMP.StageName);
	self.tbControls.Members = WndText:CreateByInstance(self.UIGROUP, self.UICOMP.Members);

	self.tbControls.SchemeName = WndText:CreateByInstance(self.UIGROUP, self.UICOMP.SchemeName);
	self.tbControls.FightRound = WndText:CreateByInstance(self.UIGROUP, self.UICOMP.FightRound);

	self.tbControls.Remain = WndText:CreateByInstance(self.UIGROUP, self.UICOMP.Remain);
end

function tbClass:OnDestroy()
	self.tbControls = {}
end

function tbClass:UpdateStage(tbData)
	local szStage = tbData.szStage;
	local nTime = tbData.nTime;
	local szSchemeName = tbData.szSchemeName;
	local nCurFightTimes = tbData.nCurFightTimes;
	local nTotalFightTimes = tbData.nTotalFightTimes;
	
	local nCurTime = GetCurServerTime();

	self.tbStage.szStage = szStage;
	self.tbStage.nEndTime = nCurTime + nTime;
	self.tbStage.szSchemeName = szSchemeName;
	self.tbStage.nCurFightTimes = nCurFightTimes;
	self.tbStage.nTotalFightTimes = nTotalFightTimes;

	self.nFixTime = nCurTime;
	self:Refresh();
end

function tbClass:UpdateMembers(tbData)
	self.tbControls.Members:SetRichText(format("<color=yellow>Thµnh viªn: <color>%s", tbData.szMember));
end

function tbClass:OnBreath()
	self:Refresh();
end

function tbClass:Refresh()

	local nCurTime = GetCurServerTime();
	if self.nFixTime <= 0 then
		self.tbStage.nEndTime = self.tbStage.nEndTime + nCurTime;
		self.nFixTime = nCurTime;
	end
	local nCountdown = self.tbStage.nEndTime - nCurTime;

	if nCountdown < 0 then
		UiManage:CloseWindow(self.UIGROUP);
		return
	end

	self.tbControls.StageName:SetRichText(format("<color=yellow>Giai ®o¹n: <color>%s", self.tbStage.szStage));
	self.tbControls.Countdown:SetRichText(format("<color=yellow>Cßn: <color><color=green>%d<color> gi©y", nCountdown));
	if self.tbStage.szSchemeName then
		self.tbControls.SchemeName:SetRichText(format("<color=yellow>%s: <color>", self.tbStage.szSchemeName));
	end
	if self.tbStage.nCurFightTimes and self.tbStage.nTotalFightTimes then
		self.tbControls.FightRound:SetRichText(format("TrËn <color=green>%d<color>/%d", self.tbStage.nCurFightTimes, self.tbStage.nTotalFightTimes));
	end
	if self.tbDesc[self.tbStage.szStage] then
		self.tbControls.Remain:SetRichText(self.tbDesc[self.tbStage.szStage]);
	end
end

UiManage:RegisterClass(tbClass);

