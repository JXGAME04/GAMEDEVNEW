if 1 ~= MODEL_GAMECLIENT then
	return
end

Include("\\script\\ui\\manage.lua")


local tbClass = {}
tbClass.UIGROUP	= "UI_GREATTEAMFIGHT_FIGHTRESULT"
tbClass.UICOMP	= {
	EnemySideTime = "EnemySideTime",
	EnemySideDamage = "EnemySideDamage",
	EnemySideKill = "EnemySideKill",
	EnemySideIcon = "EnemySideIcon",
	EnemySideAlive = "EnemySideAlive",

	SelfSideTime = "SelfSideTime",
	SelfSideScore = "SelfSideScore",
	SelfSideGlory = "SelfSideGlory",
	SelfSideDamage = "SelfSideDamage",
	SelfSideKill = "SelfSideKill",
	SelfSideIcon = "SelfSideIcon",
	SelfSideAlive = "SelfSideAlive",

	SeasonGlory = "SeasonGlory",
	SeasonScore = "SeasonScore",
	FightCount = "FightCount",

	Close = "Close"
}

tbClass.szWinIcon = "\\spr\\Ui4\\÷˜ΩÁ√Ê\\œ¿∑Â¬€Ω£vng\\ §±Í ∂.spr";
tbClass.szLoseIcon = "\\spr\\Ui4\\÷˜ΩÁ√Ê\\œ¿∑Â¬€Ω£vng\\∏∫±Í ∂.spr";


tbClass.tbControls = {}


function tbClass:OnCreate()
	self.tbControls.EnemySideTime = WndText:CreateByInstance(self.UIGROUP, self.UICOMP.EnemySideTime);
	self.tbControls.EnemySideDamage = WndText:CreateByInstance(self.UIGROUP, self.UICOMP.EnemySideDamage);
	self.tbControls.EnemySideKill = WndText:CreateByInstance(self.UIGROUP, self.UICOMP.EnemySideKill);
	self.tbControls.EnemySideAlive = WndText:CreateByInstance(self.UIGROUP, self.UICOMP.EnemySideAlive);
	self.tbControls.EnemySideIcon = WndImage:CreateByInstance(self.UIGROUP, self.UICOMP.EnemySideIcon);

	self.tbControls.SelfSideTime = WndText:CreateByInstance(self.UIGROUP, self.UICOMP.SelfSideTime);
	self.tbControls.SelfSideScore = WndText:CreateByInstance(self.UIGROUP, self.UICOMP.SelfSideScore);
	self.tbControls.SelfSideGlory = WndText:CreateByInstance(self.UIGROUP, self.UICOMP.SelfSideGlory);
	self.tbControls.SelfSideDamage = WndText:CreateByInstance(self.UIGROUP, self.UICOMP.SelfSideDamage);
	self.tbControls.SelfSideKill = WndText:CreateByInstance(self.UIGROUP, self.UICOMP.SelfSideKill);
	self.tbControls.SelfSideAlive = WndText:CreateByInstance(self.UIGROUP, self.UICOMP.SelfSideAlive);
	self.tbControls.SelfSideIcon = WndImage:CreateByInstance(self.UIGROUP, self.UICOMP.SelfSideIcon);

	self.tbControls.SeasonGlory = WndText:CreateByInstance(self.UIGROUP, self.UICOMP.SeasonGlory);
	self.tbControls.SeasonScore = WndText:CreateByInstance(self.UIGROUP, self.UICOMP.SeasonScore);
	self.tbControls.FightCount = WndText:CreateByInstance(self.UIGROUP, self.UICOMP.FightCount);
end

function tbClass:OnDestroy()
	self.tbControls = {}
end

function tbClass:OnOpen(tbData)
	if not tbData then
		return 0;
	end

	local tbSelfSide = tbData["selfside"];
	local tbEnemySide = tbData["enemyside"];

	self.tbControls.EnemySideTime:SetRichText(format("ThÍi gian Æ u: <color=green>%d<color>", tbEnemySide.nTime));
	self.tbControls.EnemySideDamage:SetRichText(format("S∏t th≠¨ng trÀn nµy: <color=green>%d<color>", tbEnemySide.nDamage));
	self.tbControls.EnemySideKill:SetRichText(format("SË di÷t Æﬁch: <color=green>%d<color>", tbEnemySide.nKill));
	self.tbControls.EnemySideAlive:SetRichText(format("SË ng≠Íi sËng s„t: <color=green>%d<color>", tbEnemySide.nAlive));
	if tbEnemySide.bWin == 1 then
		self.tbControls.EnemySideIcon:SetImage(self.szWinIcon);
	else
		self.tbControls.EnemySideIcon:SetImage(self.szLoseIcon);
	end

	self.tbControls.SelfSideTime:SetRichText(format("ThÍi gian Æ u: <color=green>%d<color>", tbSelfSide.nTime));
	self.tbControls.SelfSideDamage:SetRichText(format("S∏t th≠¨ng trÀn nµy: <color=green>%d<color>", tbSelfSide.nDamage));
	self.tbControls.SelfSideKill:SetRichText(format("SË di÷t Æﬁch: <color=green>%d<color>", tbSelfSide.nKill));
	self.tbControls.SelfSideAlive:SetRichText(format("SË ng≠Íi sËng s„t: <color=green>%d<color>", tbSelfSide.nAlive));
	self.tbControls.SelfSideScore:SetRichText(format("ßi”m t›ch lÚy: <color=green>%d<color>", tbSelfSide.nScore));
	self.tbControls.SelfSideGlory:SetRichText(format("Vinh Quang: <color=green>%d<color>", tbSelfSide.nGlory));
	if tbSelfSide.bWin == 1 then
		self.tbControls.SelfSideIcon:SetImage(self.szWinIcon);
	else
		self.tbControls.SelfSideIcon:SetImage(self.szLoseIcon);
	end

	self.tbControls.SeasonGlory:SetRichText(format("ßi”m Vinh Quang tu«n: <color=green>%d<color>", tbData.nSeasonGlory));
	self.tbControls.SeasonScore:SetRichText(format("ßi”m Hi÷p Phong tu«n: <color=green>%d<color>", tbData.nSeasonScore));
	self.tbControls.FightCount:SetRichText(format("Chi’n t›ch hoπt ÆÈng l«n nµy: Thæng <color=green>%d<color> Thua <color=green>%d<color>", tbData.nRoundWinFight, tbData.nRoundTotalFight - tbData.nRoundWinFight));

	return 1;
end

function tbClass:OnLBClick(szWnd, nParam)
	if szWnd == self.UICOMP.Close then
		UiManage:CloseWindow(self.UIGROUP);
	end
end

UiManage:RegisterClass(tbClass);
