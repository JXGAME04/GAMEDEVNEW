--ÆäËû¹¦ÄÜ²ú³ö
Include("\\script\\misc\\eventsys\\type\\func.lua")
Include("\\script\\lib\\awardtemplet.lua")
Include("\\script\\misc\\eventsys\\type\\npcdeath.lua")

MIN_FREE_ROOM_NEED = 1
MSG_FREE_ROOM_NEED_ERROR = "Tói kh«ng ®ñ chç sÏ kh«ng nhËn ®­îc Tinh Ngäc Nguyªn Th¹ch."
TB_ITEM_PROP = {6,1, 4885, 1,0,0} --ÐÇÓñÔ­Ê¯	6	1	4550	

--Ñ×µÛ±¦²Ø
function OnYanDiBaoZangLastBossDeath(nNpcIndex, nPlayerIndex)
	local nFreeRoom = CallPlayerFunction(nPlayerIndex, CalcFreeItemCellCount)
	if nFreeRoom < MIN_FREE_ROOM_NEED then
		CallPlayerFunction(nPlayerIndex, Msg2Player, MSG_FREE_ROOM_NEED_ERROR)
		return
	end
	
	local tbAwardItem = {tbProp=TB_ITEM_PROP,nCount=2}
	CallPlayerFunction(nPlayerIndex, tbAwardTemplet.Give, tbAwardTemplet, tbAwardItem, 1, {"xingyuyuanshi", "YDBZ_BOSS_DEATH_GET"})
end

--·çÁê¶É£¬Ë®ÔôBOSS
function OnFengLingDuShuiZeiDeath(nNpcIndex, nPlayerIndex)
	local nFreeRoom = CallPlayerFunction(nPlayerIndex, CalcFreeItemCellCount)
	if nFreeRoom < MIN_FREE_ROOM_NEED then
		CallPlayerFunction(nPlayerIndex, Msg2Player, MSG_FREE_ROOM_NEED_ERROR)
		return
	end
	
	local tbAwardItem = {
		{tbProp=TB_ITEM_PROP,nRate=60,nCount=1},
		{tbProp=TB_ITEM_PROP,nRate=40,nCount=2},
	}
	
	CallPlayerFunction(nPlayerIndex, tbAwardTemplet.Give, tbAwardTemplet, tbAwardItem, 1, {"xingyuyuanshi", "FENGLINGDU_DEATH_GET"})
end

--É±ÊÖÈÎÎñ
function OnKillKillerBossDeath(nTaskId, nTaskValue)
	if CalcFreeItemCellCount() < MIN_FREE_ROOM_NEED then
		Msg2Player(MSG_FREE_ROOM_NEED_ERROR)
		return
	end
	
	local tbAwardItem = {
		{tbProp=TB_ITEM_PROP,nRate=10,nCount=1},
	}
	tbAwardTemplet:Give(tbAwardItem, 1, {"xingyuyuanshi", "KILLER_DEATH_GET"})
end

--´³¹ØÈÎÎñ
function OnChuangGuanPass(nBatch, tbPlayerList, nLevel, nTime)
	if nBatch ~= 29 then
		return
	end
	
	local tbAwardItem = {
		{tbProp=TB_ITEM_PROP,nCount=2},
	}
	for i=1, getn(tbPlayerList) do
		local nPlayerIndex = tbPlayerList[i]
		local nFreeRoom = CallPlayerFunction(nPlayerIndex, CalcFreeItemCellCount)
		if nFreeRoom < MIN_FREE_ROOM_NEED then
			CallPlayerFunction(nPlayerIndex, Msg2Player, MSG_FREE_ROOM_NEED_ERROR)
		else
			CallPlayerFunction(nPlayerIndex, tbAwardTemplet.Give, tbAwardTemplet, tbAwardItem, 1, {"xingyuyuanshi", "CHUANGGUAN_PASS_GET"})
		end
	end
end

--ÎäÁÖÃËÖ÷ÈÎÎñ
function OnFinishWuLingMengZhuTask()
	if CalcFreeItemCellCount() < MIN_FREE_ROOM_NEED then
		Msg2Player(MSG_FREE_ROOM_NEED_ERROR)
		return
	end
	
	local tbAwardItem = {
		{tbProp=TB_ITEM_PROP,nRate=5,nCount=1},
	}
	tbAwardTemplet:Give(tbAwardItem, 1, {"xingyuyuanshi", "WULINMENGZHU_TASK_FINISH_GET"})
end

--½£Ú£ÃÔ¹¬µ¥ÈË
function OnSwordBurialMazeSingle()
	if CalcFreeItemCellCount() < MIN_FREE_ROOM_NEED then
		Msg2Player(MSG_FREE_ROOM_NEED_ERROR)
		return
	end
	local tbAwardItem = {
		{tbProp=TB_ITEM_PROP,nRate=7,nCount=1},
		{tbProp=TB_ITEM_PROP,nRate=2,nCount=2},
	}
	tbAwardTemplet:Give(tbAwardItem, 1, {"xingyuyuanshi", "SWORDBURIALMAZE_SINGLE"})
end

--½£Ú£ÃÔ¹¬¶àÈË
function OnSwordBurialMazeTeam()
	if CalcFreeItemCellCount() < MIN_FREE_ROOM_NEED then
		Msg2Player(MSG_FREE_ROOM_NEED_ERROR)
		return
	end
	local tbAwardItem = {
		{tbProp=TB_ITEM_PROP,nRate=7,nCount=1},
		{tbProp=TB_ITEM_PROP,nRate=2,nCount=2},
	}
	tbAwardTemplet:Give(tbAwardItem, 1, {"xingyuyuanshi", "SWORDBURIALMAZE_TEAM"})
end

--Àî°×BOSS
function OnLiBaiBossDeathProcess(nNpcIndex, nPlayerIndex)
	local nFreeRoom = CallPlayerFunction(nPlayerIndex, CalcFreeItemCellCount)
	if nFreeRoom < MIN_FREE_ROOM_NEED then
		CallPlayerFunction(nPlayerIndex, Msg2Player, MSG_FREE_ROOM_NEED_ERROR)
		return
	end
	
	local tbAwardItem = {tbProp=TB_ITEM_PROP, nCount=100}
	CallPlayerFunction(nPlayerIndex, tbAwardTemplet.Give, tbAwardTemplet, tbAwardItem, 1, {"xingyuyuanshi", "LIBAI_BOSS_DEATH_GET"})
end

--ÑªÕ½²ÔÀÇ
function OnGetBloodyBattlePrize(nBattleResult)
	if nBattleResult == 0 or nBattleResult == 1 then
		if CalcFreeItemCellCount() < MIN_FREE_ROOM_NEED then
			Msg2Player(MSG_FREE_ROOM_NEED_ERROR)
			return
		end
		local tbAwardItem = {tbProp=TB_ITEM_PROP,nCount=1}
		tbAwardTemplet:Give(tbAwardItem, 1, {"xingyuyuanshi", "SWORDBURIALMAZE_TEAM"})
	end
end


--°ï»á¸±±¾¹ÅËþ
function OnOpenTongGuaGoldBox()
	if CalcFreeItemCellCount() < MIN_FREE_ROOM_NEED then
		Msg2Player(MSG_FREE_ROOM_NEED_ERROR)
		return
	end
	
	local tbExtraAwardItem = {
		{tbProp=TB_ITEM_PROP,nRate=40,nCount=2},
		{tbProp=TB_ITEM_PROP,nRate=20,nCount=4},
	}
	tbAwardTemplet:Give(tbExtraAwardItem, 1, {"xingyuyuanshi", "OPEN_TONG_GUTA_GOLDBOX"})
end

function OnWorldBossDeath(nNpcIndex, nPlayerIndex)
	local nFreeRoom = CallPlayerFunction(nPlayerIndex, CalcFreeItemCellCount)
	if nFreeRoom < MIN_FREE_ROOM_NEED then
		CallPlayerFunction(nPlayerIndex, Msg2Player, MSG_FREE_ROOM_NEED_ERROR)
		return
	end
	
	local tbAwardItem = {tbProp=TB_ITEM_PROP, nCount=10}
	CallPlayerFunction(nPlayerIndex, tbAwardTemplet.Give, tbAwardTemplet, tbAwardItem, 1, {"xingyuyuanshi", "WORLD_BOSS_DEATH_GET"})
end

function OnOpenFuncAwardBoxCall()
	if CalcFreeItemCellCount() < MIN_FREE_ROOM_NEED then
		Msg2Player(MSG_FREE_ROOM_NEED_ERROR)
		return
	end
	
	local tbExtraAwardItem = {
		{tbProp=TB_ITEM_PROP,nRate=1,nCount=1},
	}
	tbAwardTemplet:Give(tbExtraAwardItem, 1, {"xingyuyuanshi", "OPEN_FUNC_AWARD_BOX"})
end

function RegisterFuncOutPutEvent()
	EventSys:GetType("YanDiBaoZang"):Reg("OnLastBigBossDeath", OnYanDiBaoZangLastBossDeath)
	EventSys:GetType("FengLingDu"):Reg("OnShuiZeiDeath", OnFengLingDuShuiZeiDeath)
	EventSys:GetType("KillerBoss"):Reg("OnKillBoss", OnKillKillerBossDeath)
	EventSys:GetType("ChuanGuan"):Reg("OnPass", OnChuangGuanPass)
	EventSys:GetType("WuLinMengZhu"):Reg("OnFinishTask", OnFinishWuLingMengZhuTask)
	EventSys:GetType("SwordBurialMaze"):Reg("OnSingleFinish", OnSwordBurialMazeSingle)  
	EventSys:GetType("SwordBurialMaze"):Reg("OnTeamFinish", OnSwordBurialMazeTeam)  
	EventSys:GetType("WorldBoss"):Reg("OnLiBaiBossDeath", OnLiBaiBossDeathProcess)
	EventSys:GetType("BloodyBattle"):Reg("OnGetPrize", OnGetBloodyBattlePrize)
	EventSys:GetType("TongGuTaAward"):Reg("OnOpenGoldBox", OnOpenTongGuaGoldBox)
	EventSys:GetType("WorldBoss"):Reg("OnBossDeath", OnWorldBossDeath)
	EventSys:GetType("OpenFuncAwardBox"):Reg("OpenAwardBoxEvent", OnOpenFuncAwardBoxCall)
end

RegisterFuncOutPutEvent()