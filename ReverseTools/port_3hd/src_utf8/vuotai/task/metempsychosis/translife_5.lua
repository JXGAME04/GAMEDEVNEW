-----------------------------------------------------------------------------
-- Translife 5
--
-- Last modified        2012-8-16.
-- Written by           liujun4.
-- Email                dearliujun@gmail.com.
-- Copyright            Kingsoft. 
-----------------------------------------------------------------------------

Include("\\script\\misc\\eventsys\\type\\func.lua")
Include("\\script\\misc\\eventsys\\type\\harvestplant.lua")
Include("\\script\\lib\\awardtemplet.lua")
Include("\\script\\missions\\challengeoftime\\include.lua")
Include("\\script\\task\\metempsychosis\\task_head.lua")
Include("\\script\\missions\\yandibaozang\\head.lua")

local tbAwardItem = {szName="Vô Cực Tiên Đơn", tbProp={6,1,3277,1,0,0},nBindState = -2,}
local nAwardCount = 5
local nFinishSongJin = 300
local nChuanguanLimit = 17
local nYDBZguoguanLimit = 6
local nTranslifeCount = 4
local nLevelLimit = TB_LEVEL_LIMIT[nTranslifeCount+1]

TRANSLIFE5_SKILL = 1262

-----------------------------------------------------------------------------
-- FinishSongJin. 
--
-- @param tbPlayerAll        All player who finished songjin.
-----------------------------------------------------------------------------

function TRANSLIFE5_onFinishSongJin(nBattleLevel, tbPlayerAll)
	for i=1, getn(tbPlayerAll) do
		CallPlayerFunction(tbPlayerAll[i].nIndex, TRANSLIFE5_onFinishSongJinAward)
	end
end


-----------------------------------------------------------------------------
-- Chuanguan. if nChuanguanCount >= 17, then give award.
--
-- @param nChuanguanCount   
-- @param tbAllPlayer     
-----------------------------------------------------------------------------

function TRANSLIFE5_onChuanguan(nChuanguanCount, tbAllPlayer, n_level)
	if nChuanguanCount == %nChuanguanLimit then
		for i = 1, getn(tbAllPlayer) do 
			CallPlayerFunction(tbAllPlayer[i], TRANSLIFE5_onChuanguanAward)
		end
	end
end


-----------------------------------------------------------------------------
-- YDBZguoguan. if nGuoGuanCount >= 6, then give award
--
-- @param nPlayerIndex       
-- @param nGuoGuanCount      
-----------------------------------------------------------------------------

function TRANSLIFE5_onYDBZguoguan(nGuoGuanCount)
	
	if nGuoGuanCount == %nYDBZguoguanLimit then
		--if GetByte(GetTask(YDBZ_ITEM_YANDILING), 1) == 1 then
			if CalcFreeItemCellCount() > 0 then
				TRANSLIFE5_award()
			end
		--end
	end
end


-----------------------------------------------------------------------------
-- Ontianlu_tree. 
--
-- @param nNpcIndex       
-- @param nPlayerIndex      
-----------------------------------------------------------------------------

function TRANSLIFE5_ontianlu_tree(nNpcIndex)
	local nCount = GetTask(TSK_TRANSLIFE_5)
	if nCount > 0 then
		SetTask(TSK_TRANSLIFE_5, nCount+1)
		Msg2Player(format("Đại hiệp ngươi đã trồng được %d cây.", nCount))
	end
end


-----------------------------------------------------------------------------
-- OnFinishSongJinAward. 
-- Giving award when player gain points above than 300.
-----------------------------------------------------------------------------

function TRANSLIFE5_onFinishSongJinAward()
	if GetTask(751) >= %nFinishSongJin then
		TRANSLIFE5_award()
	end
end


-----------------------------------------------------------------------------
-- OnChuanguanAward. 
-- Giving award when player used longxuewan.
-----------------------------------------------------------------------------

function TRANSLIFE5_onChuanguanAward()
	if GetTask(%TSK_Longxuewan_avail) >= 0 then
		TRANSLIFE5_award()
	end
end


-----------------------------------------------------------------------------
-- Giving award when player take the task of translife_5
-----------------------------------------------------------------------------

function TRANSLIFE5_award()
	if GetTask(TSK_TRANSLIFE_5) > 0 then
		tbAwardTemplet:Give(%tbAwardItem, %nAwardCount,{%tbAwardItem.szName})
	end
end


-----------------------------------------------------------------------------
-- Dialog option.      
-----------------------------------------------------------------------------

function TRANSLIFE5_wantGetTranslifeTask()
CreateTaskSay({format("<dec><npc>Chỉ có số lần trùng sinh là %d lần, hơn nữa đẳng cấp đạt đến %d mới có thể nhận nhiệm vụ này, ngươi có quyết định nhận nhiệm vụ này không? ", %nTranslifeCount, %nLevelLimit),
		" Sử dụng/TRANSLIFE5_sureGetTranslifeTask",
		"Để ta suy nghĩ lại/OnCancel"})
end


-----------------------------------------------------------------------------
-- Get translife task.      
-----------------------------------------------------------------------------

function TRANSLIFE5_sureGetTranslifeTask()
	local nCurTranslifecount = ST_GetTransLifeCount()
	if nCurTranslifecount ~= %nTranslifeCount then
		CreateTaskSay({format("<dec><npc>".."Số lần trùng sinh hiện tại của ngươi là %d lần, không thể nhận nhiệm vụ này!", %nTranslifeCount), " Biết rồi!/OnCancel"})
		return 
	end
	local nCurLevel = GetLevel()
	if nCurLevel < %nLevelLimit then
		CreateTaskSay({format("<dec><npc>".."Đẳng cấp của ngươi vẫn chưa đủ %d cấp!", %nLevelLimit), " Biết rồi!/OnCancel"})
		return 
	end	
	if GetTask(TSK_TRANSLIFE_5) == 0 then
		SetTask(TSK_TRANSLIFE_5, 1)
	end
	Msg2Player("Ngươi đã thành công nhận nhiệm vụ trùng sinh 5.")
end


-----------------------------------------------------------------------------
-- Check conditions of translife_5: 400个无极仙丹，150个黑龙仙丹，400棵转生树，
-- 一部《北斗长生书》
--
-- @return      0 for false, 1 for true       
-----------------------------------------------------------------------------

function TRANSLIFE5_canTranslife()
	for i=1,getn(TBITEMNEED_5) do
		local tbProb = TBITEMNEED_5[i].tbProb
		if CalcItemCount(3,tbProb[1], tbProb[2],tbProb[3], -1) < TBITEMNEED_5[i].nCount then
			CreateTaskSay({TB_TRANSLIFE_ERRORMSG[15 + i], "Được rồi./OnCancel"})
			return 0
		end
	end
--	if GetTask(TSK_TRANSLIFE_5)-1 < N_TRANSLIFE_TREE_MIN then
--		CreateTaskSay({TB_TRANSLIFE_ERRORMSG[19], "Được rồi./OnCancel"})
--		return 0
--	end
	--Translife 5 need 5000000 jxb - modified by DinhHQ - 20130531
	--Translife 5 bỏ chuyển sinh thứ 5 - modified by NgaVN - 20140620
--	if GetCash() < 10000000 then
--		CreateTaskSay({"Lần chuyển sinh thứ 5 yêu cầu phải có 1000 vạn làm chi phí, xin hãy chuẩn bị đủ rồi đến tìm ta", "Được rồi./OnCancel"})
--		return 0
--	end
	return 1
end


-----------------------------------------------------------------------------
-- Translife succeed, do some operation.      
-----------------------------------------------------------------------------

function TRANSLIFE5_translifeOperation()
	SetTask(TSK_TRANSLIFE_5, 0)   -- 5转任务设置为未接收
	for i=1,getn(TBITEMNEED_5) do -- 扣除5转道具
		local tbProb = TBITEMNEED_5[i].tbProb
		ConsumeItem(3,TBITEMNEED_5[i].nCount, tbProb[1], tbProb[2], tbProb[3], -1)
	end
	AddMagic(%TRANSLIFE5_SKILL, 1)   -- 奖励五转技能
end


-----------------------------------------------------------------------------
-- Query the number of tree.      
-----------------------------------------------------------------------------

function TRANSLIFE5_queryTreeNumber()
	local ntranscount = ST_GetTransLifeCount()
	if GetTask(TSK_TRANSLIFE_5) == 0 then
		CreateTaskSay({"<dec><npc>Ngươi vẫn chưa nhận nhiệm vụ trùng sinh 5, không có số lượng trồng cây để kiểm tra!", " Biết rồi!/OnCancel"})
		return 
	end
	local nTreeNumber = GetTask(TSK_TRANSLIFE_5) - 1
	CreateTaskSay({format("<dec><npc>Hiện tại ngươi đã trồng được<color=red>%d<color>Cây Trùng Sinh.", nTreeNumber),
		 " Biết rồi!/OnCancel"})
end


-----------------------------------------------------------------------------
-- Registe some messages to eventsys.
-----------------------------------------------------------------------------

function TRANSLIFE5_init()
	EventSys:GetType("SongJin"):Reg("OnFinish", TRANSLIFE5_onFinishSongJin)  -- 宋金
	EventSys:GetType("ChuanGuan"):Reg("OnPass", TRANSLIFE5_onChuanguan)  -- 闯关（使用龙血丸）
	EventSys:GetType("YanDiBaoZang"):Reg("OnPass", TRANSLIFE5_onYDBZguoguan)  -- 炎帝闯关
	EventSys:GetType("HarvestPlants"):Reg("tianlu_tree", TRANSLIFE5_ontianlu_tree)  -- 种树
end

--Make change translife for free - Modified by DinhHQ - 20130531
--TRANSLIFE5_init()