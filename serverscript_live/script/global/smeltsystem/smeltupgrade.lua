-- [DUNGLUYEN 01/09] Port tu ban Linux \script\global\smeltsystem\smeltupgrade.lua. Khac: duong bang phang,
-- AddItem 7 tham so, ConsumeItem tra so mon, RemoveItemByIndex(idx, soluong), GetGiveItemUnit tra 1 muc/don vi.
Include("\\script\\activitysys\\playerfunlib.lua")
Include("\\script\\dailogsys\\dailogsay.lua")
Include("\\script\\misc\\eventsys\\eventsys.lua")
Include("\\script\\global\\smeltsystem\\lang.lua")

--纹钢和纹晶升级对应表，每一项由三部分组成，分别是：纹钢,纹晶以及升级后的纹钢
--其中纹晶部分第一项为纹晶的P,第二项标识该纹晶是否允许被升级
--例：假如纹钢(8,1,11)+纹晶(6,1,1111)可以合成纹钢(8,1,15),其中纹晶(6,1,1111)不允许被用来进行纹晶的升级，则对应的项为：
--{{11}, 	{1111, 0,} 	{15}}
--local tbSmeltUpgradeTable = {
--	{tbWenGang={nP=1},	tbWenJing={nP=3557,bUpgrade=1},	tbResWenJing={nP=3558,bBind=0}, 	tbResWenGang={nP=2,bBind=0}},
--	{tbWenGang={nP=2},	tbWenJing={nP=3558,bUpgrade=1},	tbResWenJing={nP=3559,bBind=0}, 	tbResWenGang={nP=3,bBind=0}},
--	{tbWenGang={nP=3},	tbWenJing={nP=3559,bUpgrade=1},	tbResWenJing={nP=3560,bBind=0}, 	tbResWenGang={nP=4,bBind=0}},
--	{tbWenGang={nP=4},	tbWenJing={nP=3560,bUpgrade=1},	tbResWenJing={nP=3561,bBind=0}, 	tbResWenGang={nP=5,bBind=0}},
--	{tbWenGang={nP=5},	tbWenJing={nP=3561,bUpgrade=1},	tbResWenJing={nP=3562,bBind=0}, 	tbResWenGang={nP=6,bBind=1}},
--	{tbWenGang={nP=6},	tbWenJing={nP=3562,bUpgrade=1},	tbResWenJing={nP=3563,bBind=0}, 	tbResWenGang={nP=7,bBind=1}},
--	{tbWenGang={nP=7},	tbWenJing={nP=3563,bUpgrade=1},	tbResWenJing={nP=3564,bBind=0}, 	tbResWenGang={nP=8,bBind=1}},
--	{tbWenGang={nP=8},	tbWenJing={nP=3564,bUpgrade=1},	tbResWenJing={nP=3565,bBind=0}, 	tbResWenGang={nP=9,bBind=1}},
--	{tbWenGang={nP=9},	tbWenJing={nP=3565,bUpgrade=0},						tbResWenGang={nP=10,bBind=1}},
--}

WEN_JING_MONEY = 200000


tbSmeltUpgradeTable = {}

function LoadDataFromFile()
	local szFilePath = "\\settings\\item\\smeltupgradetable.txt"	-- [DUNGLUYEN 01/09] JX1 thu muc phang
	local szKey = szFilePath
	if TabFile_Load(szFilePath, szKey) == 0 then
		print(format("load %s failed!\n", szFilePath))
		return 0
	end

	local nRowCount = TabFile_GetRowCount(szKey)
	for nRow = 2, nRowCount do
		local tbSubLine = {}
		tbSubLine.tbWenGang = {}
		tbSubLine.tbWenJing = {}
		tbSubLine.tbResWenJing = {}
		tbSubLine.tbResWenGang = {}

		tbSubLine.tbWenGang.nP = tonumber(TabFile_GetCell(szKey, nRow, 4))
		if tbSubLine.tbWenGang.nP == "" then
			tbSubLine.tbWenGang = nil
		end

		tbSubLine.tbWenJing.nP = tonumber(TabFile_GetCell(szKey, nRow, 8))
		tbSubLine.tbWenJing.bUpgrade = tonumber(TabFile_GetCell(szKey, nRow, 9))
		if tbSubLine.tbWenJing.nP == "" or tbSubLine.tbWenJing.bUpgrade == "" then
			tbSubLine.tbWenJing = nil
		end

		tbSubLine.tbResWenJing.nP = tonumber(TabFile_GetCell(szKey, nRow, 13))
		tbSubLine.tbResWenJing.bBind = tonumber(TabFile_GetCell(szKey, nRow, 14))
		if tbSubLine.tbResWenJing.nP == "" or tbSubLine.tbResWenJing.bBind == "" then
			tbSubLine.tbResWenJing = nil
		end

		tbSubLine.tbResWenGang.nP = tonumber(TabFile_GetCell(szKey, nRow, 18))
		tbSubLine.tbResWenGang.bBind = tonumber(TabFile_GetCell(szKey, nRow, 19))
		if tbSubLine.tbResWenGang.nP == "" or tbSubLine.tbResWenGang.bBind == "" then
			tbSubLine.tbResWenGang = nil
		end

		tbSmeltUpgradeTable[nRow - 1] = tbSubLine
	end

	TabFile_UnLoad(szKey);
end

-- [DUNGLUYEN 01/09] onSmelt() dinh nghia o smelt_system.lua (mo give-box thay SmeltItem cua Linux)

--返回nType类型的道具nP在纹钢纹晶升级对应表中的索引，其中nType=1表示纹钢，nType=2表示纹晶，nType=3表示升级后的纹晶，如果找不到记录返回-1
function GetItemIdx(nType, nP)
	local nIndex = -1
	
	for i=1,getn(tbSmeltUpgradeTable) do
		if nType == 1 then
			if tbSmeltUpgradeTable[i].tbWenGang ~= nil and tbSmeltUpgradeTable[i].tbWenGang.nP == nP then
				nIndex = i
		break
		end
		elseif nType == 2 then
		if tbSmeltUpgradeTable[i].tbWenJing ~= nil and tbSmeltUpgradeTable[i].tbWenJing.nP == nP then
			nIndex = i
		break
		end
		elseif nType == 3 then
		if tbSmeltUpgradeTable[i].tbResWenJing ~= nil and tbSmeltUpgradeTable[i].tbResWenJing.nP == nP then
			nIndex = i
		break
		end
		end
	end

	return nIndex
end

--纹晶拆解
function onDegradeWenJing()
	g_GiveItemUI(FUSION_MSG.DegradeWenJing, format(FUSION_MSG.DegradeWenJingTips, WEN_JING_MONEY), {HandIn_DegradeWenJing, {}},{},1)
end

function HandIn_DegradeWenJing(nCount)
	-- Check ItemNum
	if nCount == 0 then
		Msg2Player(FUSION_MSG.NoMaterial)
	return 0
	end

	--拿到提交框中的物品，进行归类
	local tbItemNum = {}
	local tbDaDem = {}	-- [DUNGLUYEN 01/09] JX1 GetGiveItemUnit tra 1 muc cho MOI don vi cua chong
	for i=1, nCount do
		local nItemIndex = GetGiveItemUnit(i)
		if not tbDaDem[nItemIndex] then
		tbDaDem[nItemIndex] = 1
		local nG, nD, nP = GetItemProp(nItemIndex)
		local nStackCount = GetItemStackCount(nItemIndex)
		local nBindState = GetItemBindState(nItemIndex)
	
		if nG ~= 6 or nD ~= 1 then
			Msg2Player(FUSION_MSG.NotWenJing)
			return 0
		end
	
		if tbItemNum[nP] == nil then
			tbItemNum[nP] = nStackCount
		else
			tbItemNum[nP] = tbItemNum[nP] + nStackCount
		end
		end
	end

	--将归类好的物品集合中的物品逐一检查，看是否在纹晶升级表中存在
	local tbIndex = {}
	local nTotalCount = 0
	local nBagCell = 0
	for k,v in pairs(tbItemNum) do
		tbIndex[k] = GetItemIdx(3, k)
		if tbIndex[k] < 0 or  tbSmeltUpgradeTable[tbIndex[k]].tbResWenJing == nil then
			Msg2Player(FUSION_MSG.ItemCantDegrade)
			return 0
		end

		if tbSmeltUpgradeTable[tbIndex[k]].tbWenJing.bUpgrade ~= 1 then
			Msg2Player(FUSION_MSG.WenJingCantDegrade)
			return 0
		end

		nTotalCount = nTotalCount + v
		nBagCell = nBagCell + ceil((v*2)/200)
	end

	if CalcFreeItemCellCount() < nBagCell then
		Msg2Player(format(FUSION_MSG.DegradeNeedBag, nBagCell))
	return 0
	end

	if GetCash() < nTotalCount * WEN_JING_MONEY then
		Msg2Player(format(FUSION_MSG.WenJingNotEnoughMoney, nTotalCount*WEN_JING_MONEY))
	return 0
	end

	--检查物品集合中的每一件物品是否都在表里存在对应的未升级前纹晶
	for k,v in pairs(tbItemNum) do
		local nWenJingP =  tbSmeltUpgradeTable[tbIndex[k]].tbWenJing.nP

		if nWenJingP == nil then
			Msg2Player(FUSION_MSG.ItemCantDegrade)
			return 0
		end
	end

	--删除提交物品
	local tbDaXoa = {}
	for i=1, nCount do
		local nItemIndex = GetGiveItemUnit(i)
		if nItemIndex > 0 and not tbDaXoa[nItemIndex] then
			tbDaXoa[nItemIndex] = 1
			RemoveItemByIndex(nItemIndex, GetItemStackCount(nItemIndex))	-- [DUNGLUYEN 01/09] JX1: mac dinh chi tru 1 don vi
		end
	end

	--遍历物品集合给予玩家升级后的纹晶
	for k,v in pairs(tbItemNum) do
		local nWenJingP =  tbSmeltUpgradeTable[tbIndex[k]].tbWenJing.nP
		local bResBind = tbSmeltUpgradeTable[tbIndex[k]].tbResWenJing.bBind

		Pay(v*WEN_JING_MONEY)
		local nBindState = nil
		if bResBind == 1 then
			nBindState = -2
		end
		
		PlayerFunLib:GetItem({tbProp={6,1,nWenJingP,1,0,0},nBindState=nBindState,},v*2,format("[SemltSystem] %s Degrade ID:%d WenJing:%d ",GetName(),nWenJingP,v))
		AddStatData("ronglian_wenjingchai", v)
	end

	Msg2Player(FUSION_MSG.WenJingDegradeOK)
end

--纹晶升级
function onUpgradeWenJing()
	g_GiveItemUI(FUSION_MSG.WenJingUpgrade, format(FUSION_MSG.WenJingUpgradeTips, WEN_JING_MONEY), {HandIn_UpgradeWenJing, {}},{},1)
end

function HandIn_UpgradeWenJing(nCount)
	-- Check ItemNum
	if nCount == 0 then
		Msg2Player(FUSION_MSG.NoMaterial)
		return 0
	end

	--拿到提交框中的物品，进行归类
	local tbItemNum = {}
	local tbDaDem = {}	-- [DUNGLUYEN 01/09] JX1 GetGiveItemUnit tra 1 muc cho MOI don vi cua chong
	for i=1, nCount do
	local nItemIndex = GetGiveItemUnit(i)
	if not tbDaDem[nItemIndex] then
	tbDaDem[nItemIndex] = 1
	local nG, nD, nP = GetItemProp(nItemIndex)
	local nStackCount = GetItemStackCount(nItemIndex)
	local nBindState = GetItemBindState(nItemIndex)

	if nG ~= 6 or nD ~= 1 then
		Msg2Player(FUSION_MSG.DegradeNotWenJing)
		return 0
	end

	if tbItemNum[nP] == nil then
		tbItemNum[nP] = nStackCount
		else
		tbItemNum[nP] = tbItemNum[nP] + nStackCount
		end
	end
	end

	--将归类好的物品集合中的物品逐一检查，看是否在纹晶升级表中存在以及数量是否符合规则
	local tbIndex = {}
	local nTotalCount = 0
	local nBagCell = 0
	for k,v in pairs(tbItemNum) do
		tbIndex[k] = GetItemIdx(2, k)
	if tbIndex[k] < 0 or  tbSmeltUpgradeTable[tbIndex[k]].tbResWenJing == nil then
			Msg2Player(FUSION_MSG.ItemCantUpgrade)
		return 0
		end

	if tbSmeltUpgradeTable[tbIndex[k]].tbWenJing.bUpgrade ~= 1 then
		Msg2Player(FUSION_MSG.WenJingCantUpgrade)
		return 0
	end

	if v/2 ~= floor(v/2) then
		Msg2Player(FUSION_MSG.AmountNotEven)
		return 0
	end
	nTotalCount = nTotalCount + v/2
	nBagCell = nBagCell + ceil((v/2)/200)
	end

	if CalcFreeItemCellCount() < nBagCell then
		Msg2Player(format(FUSION_MSG.UpgradeNeedBag, nBagCell))
	return 0
	end

	if GetCash() < nTotalCount * WEN_JING_MONEY then
		Msg2Player(format(FUSION_MSG.WenJingUpgradeNeedMoney, nTotalCount*WEN_JING_MONEY))
	return 0
	end

	--检查物品集合中的每一件物品是否都在表里存在对应的升级后的纹晶
	for k,v in pairs(tbItemNum) do
		local nResWenJingP =  tbSmeltUpgradeTable[tbIndex[k]].tbResWenJing.nP

	if nResWenJingP == nil then
		Msg2Player(FUSION_MSG.ItemCantUpgrade)
		return 0
		end
	end

	--删除提交物品
	local tbDaXoa = {}
	for i=1, nCount do
		local nItemIndex = GetGiveItemUnit(i)
		if nItemIndex > 0 and not tbDaXoa[nItemIndex] then
			tbDaXoa[nItemIndex] = 1
			RemoveItemByIndex(nItemIndex, GetItemStackCount(nItemIndex))	-- [DUNGLUYEN 01/09] JX1: mac dinh chi tru 1 don vi
		end
	end

	--遍历物品集合给予玩家升级后的纹晶
	for k,v in pairs(tbItemNum) do
		local nResWenJingP =  tbSmeltUpgradeTable[tbIndex[k]].tbResWenJing.nP
		local bResBind = tbSmeltUpgradeTable[tbIndex[k]].tbResWenJing.bBind
		Pay((v/2)*WEN_JING_MONEY)
		local nBindState = nil
		if bResBind == 1 then
			nBindState = -2
		end
		PlayerFunLib:GetItem({tbProp={6,1,nResWenJingP,1,0,0},},v/2,format("[SemltSystem] %s Degrade ID:%d WenJing:%d ",GetName(),nResWenJingP,v/2))
		AddStatData("ronglian_wenjingup", v/2)
	end

	Msg2Player(FUSION_MSG.WenJingUpgradeOK)
end

--纹钢升级
function main(nItemIndex)
	local tbOpt = {}
	local szTitleMsg = format(FUSION_MSG.UpgradeWenGang)

	tinsert(tbOpt, {FUSION_MSG.Sure, HandIn_Main, {nItemIndex},})
	tinsert(tbOpt, {FUSION_MSG.Cancel,	 exit,{},})
	CreateNewSayEx(szTitleMsg, tbOpt)

	return 1
end

function HandIn_Main(nItemIndex)
	if IsMyItem(nItemIndex) ~= 1 then
		return 0
	end
	local nG, nD, nP = GetItemProp(nItemIndex)

	if nG ~= 6 or nD ~= 1 then
		return 0
	end

	local nIndex = GetItemIdx(2, nP)
	if nIndex < 0 then
		Msg2Player(FUSION_MSG.WenJingCantUpgradeWenGang)
		return 0
	end

	if tbSmeltUpgradeTable[nIndex].tbWenGang == nil or tbSmeltUpgradeTable[nIndex].tbResWenGang == nil then
		Msg2Player(FUSION_MSG.WenJingCantUpgradeWenGang)
		return 0
	end

	if CalcFreeItemCellCount() < 1 then
		Msg2Player(format(FUSION_MSG.WenGangUpgradeNeedBag, 1))
	return 0
	end

	local nNeedWenGangP = tbSmeltUpgradeTable[nIndex].tbWenGang.nP
	local nResWenGangP =  tbSmeltUpgradeTable[nIndex].tbResWenGang.nP
	local bResBind = tbSmeltUpgradeTable[nIndex].tbResWenGang.bBind

	if nNeedWenGangP == nil or nResWenGangP == nil then
		Msg2Player(FUSION_MSG.WenJingCantUpgradeWenGang)
		return 0
	end

	local nNeedWenGangNum = CalcEquiproomItemCount(8, 1, nNeedWenGangP, -1)	-- [DUNGLUYEN-PB 01/09] hanh trang + tui mo rong, cung pham vi ConsumeItem(3,...)
	if nNeedWenGangNum == nil or  nNeedWenGangNum <= 0 then
		Msg2Player(FUSION_MSG.NoWenGang)
	return 0
	end

	if ConsumeItem(3, 1, 8, 1, nNeedWenGangP, -1) >= 1 then	-- [DUNGLUYEN 01/09] JX1 tra SO MON da xoa
		Msg2Player(FUSION_MSG.WenGangUpgradeOK)
		local nStackCount = GetItemStackCount(nItemIndex)
		if nStackCount == 1 then
			RemoveItemByIndex(nItemIndex)
		else
			SetItemStackCount(nItemIndex, nStackCount-1)
		end
	
		local nIdx = AddItem(8,1,nResWenGangP,1,0,0,0)	-- [DUNGLUYEN 01/09] JX1 AddItem can >= 7 tham so
		if bResBind == 1 then
			SetItemBindState(nIdx,-2)
		end
		EventSys:OnPlayerEvent("OnUpgradeWenGang", PlayerIndex, nIdx)
		AddStatData("ronglian_wengangup")
	end

	return 1
end

function exit()
end

LoadDataFromFile()
