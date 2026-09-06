-- [DUNGLUYEN 01/09] Port 100% tu ban Linux \script\global\smelt_system.lua (jx_linux_y).
-- Khac Linux: (1) Huyen Hoa Than JX1 = (6,1,4125) (Linux 3507);
--             (2) JX1 khong co goi s2c 0xDC -> ma ket qua bao bang Msg2Player theo [ReturnInfo] smelt.ini/distill.ini (ban VNG);
--             (3) do dat qua give-box (OpenSmeltBox / doSmeltBox / doUnSmeltBox) thay goi c2s 182;
--             (4) AddItem cua JX1 can >= 7 tham so.
Include("\\script\\misc\\eventsys\\eventsys.lua")

SMELT_UNKNOWN_ERROR = 0
SMELT_OK = 1
SMELT_NO_MONEY = 2
SMELT_NOT_EQUIP = 3
SMELT_EQUIP_CANNOT_SMELT = 4
SMELT_EQUIP_FUSION_LIMIT = 5
SMELT_EQUIP_FUSION_QUALITY_LIMIT = 6
SMELT_EQUIP_MAGIC_EXIST = 7
SMELT_NOT_FUSION = 8
SMELT_FUSION_POS_ERROR = 9
SMELT_REMOVE_FAILED = 10
SMELT_LINGLONGISOPEN = 11
SMELT_BOXISLOCK = 12
SMELT_HAVE_EXPIREDTIME = 13
SMELT_HAVE_SHOPINFO = 14
UNSMELT_OK = 15
UNSMELT_NO_MONEY = 16
UNSMELT_NO_ROOM = 17
UNSMELT_NOT_EQUIP = 18
UNSMELT_EQUIP_NO_FUSION = 19
UNSMELT_NOT_XHT = 20
UNSMELT_LINGLONGISOPEN = 21
UNSMELT_BOXISLOCK = 22
UNSMELT_LOCKSOUL = 23

FUS_HHT_P = 4125            -- Huyen Hoa Than (Linux 3507)
FUS_NEED_MONEY = 1000000    -- 100 van luong (Linux)

-- [ReturnInfo] cua smelt.ini / distill.ini (ban VNG): khoa = ma + 1
tbSmeltReturnInfo = {
	[0] = "Dung luyÖn thÊt b¹i: ch­a t×m ra lçi!",
	[1] = "Dung luyÖn thµnh c«ng!",
	[2] = "Dung luyÖn thÊt b¹i: Ng©n l­îng kh«ng ®ñ!",
	[3] = "Dung luyÖn thÊt b¹i: H·y bá vµo trang bÞ Hoµng Kim hoÆc B¹ch Kim cÇn dung luyÖn, vµ 1 lo¹i V¨n C­¬ng nµo ®ã.",
	[4] = "Dung luyÖn thÊt b¹i: Trang bÞ bá vµo kh«ng thÓ dung luyÖn.",
	[5] = "Dung luyÖn thÊt b¹i: Trang bÞ nµy ®· dung luyÖn ®ñ l­îng V¨n C­¬ng. NÕu muèn thay ®æi lo¹i V¨n C­¬ng, h·y thÝ luyÖn l¹i V¨n C­¬ng hiÖn cã råi dung luyÖn l¹i.",
	[6] = "Dung luyÖn thÊt b¹i: Trang bÞ nµy kh«ng thÓ kÕ thõa dung luyÖn V¨n C­¬ng phÈm chÊt cao nh­ vËy.",
	[7] = "Dung luyÖn thÊt b¹i: Thuéc tÝnh cña V¨n C­¬ng muèn dung luyÖn ®· cã trªn trang bÞ!",
	[8] = "Dung luyÖn thÊt b¹i: Nguyªn liÖu dung luyÖn bá vµo kh«ng ph¶i V¨n C­¬ng!",
	[9] = "Dung luyÖn thÊt b¹i: V¨n C­¬ng kh«ng thÓ dung luyÖn trªn trang bÞ thuéc lo¹i nµy!",
	[10] = "Dung luyÖn thÊt b¹i: Ph¸t sinh lçi khi khÊu trõ nguyªn liÖu V¨n C­¬ng.",
	[11] = "Dung luyÖn thÊt b¹i: Linh Long Táa ®ang ë tr¹ng th¸i më!",
	[12] = "Dung luyÖn thÊt b¹i: R­¬ng chøa ®å ®ang ë tr¹ng th¸i khãa!",
	[13] = "Dung luyÖn thÊt b¹i: Kh«ng thÓ dung luyÖn ®èi víi trang bÞ cã thêi h¹n sö dông!",
	[14] = "Dung luyÖn thÊt b¹i: Kh«ng thÓ dung luyÖn ®èi víi trang bÞ ®ang bµy b¸n!",
	[15] = "ThÝ luyÖn thµnh c«ng!",
	[16] = "ThÝ luyÖn thÊt b¹i: Ng©n l­îng kh«ng ®ñ!",
	[17] = "ThÝ luyÖn thÊt b¹i: Hµnh trang kh«ng ®ñ chç!",
	[18] = "ThÝ luyÖn thÊt b¹i: H·y bá vµo trang bÞ Hoµng Kim hoÆc B¹ch Kim cÇn thÝ luyÖn, vµ 1 HuyÒn Háa Than.",
	[19] = "ThÝ luyÖn thÊt b¹i: Trang bÞ nµy ch­a qua dung luyÖn V¨n C­¬ng, kh«ng thÓ thÝ luyÖn!",
	[20] = "ThÝ luyÖn thÊt b¹i: Nguyªn liÖu bá vµo kh«ng ph¶i HuyÒn Háa Than!",
	[21] = "ThÝ luyÖn thÊt b¹i: Linh Long Táa ®ang ë tr¹ng th¸i më!",
	[22] = "ThÝ luyÖn thÊt b¹i: R­¬ng chøa ®å ®ang ë tr¹ng th¸i khãa!",
	[23] = "ThÝ luyÖn thÊt b¹i: VËt phÈm Táa Hån chØ cã thÓ do ng­êi së h÷u vËt phÈm thÝ luyÖn!",
}

function SmeltMsg(nResult)
	local sz = tbSmeltReturnInfo[nResult]
	if not sz then
		sz = tbSmeltReturnInfo[0]
	end
	Msg2Player(sz)
end

function DoSmeltEquip(nEquIdx, nFusionIdx)
	--Check equip bind state have time, by vng
	local nBindState = GetItemBindState(nEquIdx)
	if nBindState == -1 or nBindState > 0 then
		Msg2Player("Trang bÞ khãa cã thêi h¹n kh«ng thÓ dung luyÖn")
		return SMELT_EQUIP_CANNOT_SMELT
	end
	local nResult = SMELT_UNKNOWN_ERROR
	local nNeedMoney = FUS_NEED_MONEY

	if GetCash() < nNeedMoney then
		return SMELT_NO_MONEY
	end

	local nBillType = GetItemBillType(nEquIdx)
	if nBillType > 0 then
		if nBillType == 1 then
			return SMELT_HAVE_EXPIREDTIME
		elseif nBillType == 2 then
			return SMELT_HAVE_SHOPINFO
		end
	end

	nResult = SmeltEquip(nEquIdx, nFusionIdx, 1)
	if nResult ~= SMELT_OK then
		return nResult
	end

	if GetBoxLockState() ~= 0 then
		return SMELT_BOXISLOCK
	end

	Pay(nNeedMoney)

	local nFusionG, nFusionD, nFusionP = GetItemProp(nFusionIdx)
	local szFusionName = GetItemName(nFusionIdx) or ""

	nResult = SmeltEquip(nEquIdx, nFusionIdx, 0)
	if nResult == SMELT_OK then
		SyncItem(nEquIdx)
		if EventSys then
			EventSys:OnPlayerEvent("OnSmeltEquipment", PlayerIndex, nEquIdx, nFusionG, nFusionD, nFusionP)
		end
		local nEquG, nEquD, nEquP = GetItemProp(nEquIdx)
		WriteLog(format("[SmeltSys][SmeltEquip]\tName:%s\tAccount:%s\tEquip:%s\tEquipGDP:%d,%d,%d\tWenGang:%s\tWenGangGDP:%d,%d,%d\tMoney:%d",
			GetName(), GetAccount(), GetItemName(nEquIdx) or "", nEquG, nEquD, nEquP, szFusionName, nFusionG, nFusionD, nFusionP, nNeedMoney))
	end

	return nResult
end

function DoUnSmeltEquip(nEquIdx, nXHTIdx)
	local nResult = SMELT_UNKNOWN_ERROR
	local nNeedMoney = FUS_NEED_MONEY

	if GetCash() < nNeedMoney then
		return UNSMELT_NO_MONEY
	end

	local nG, nD, nP = GetItemProp(nXHTIdx)
	if nG ~= 6 or nD ~= 1 or nP ~= FUS_HHT_P then
		return UNSMELT_NOT_XHT
	end

	local nBindState = GetItemBindState(nEquIdx)
	if nBindState == -1 or nBindState > 0 then
		Msg2Player("Trang bÞ khãa cã thêi h¹n kh«ng thÓ dung luyÖn")
		return SMELT_UNKNOWN_ERROR
	end

	local tbFusionInfo = GetFusionInEquipInfo(nEquIdx)
	if not tbFusionInfo or next(tbFusionInfo) == nil then
		return UNSMELT_EQUIP_NO_FUSION
	end

	local nFusionNum = 0
	for i, v in pairs(tbFusionInfo) do
		nFusionNum = nFusionNum + 1
	end
	if CalcFreeItemCellCount() < nFusionNum then
		return UNSMELT_NO_ROOM
	end

	nResult = UnSmeltEquip(nEquIdx, 1)
	if nResult ~= UNSMELT_OK then
		return nResult
	end

	if GetBoxLockState() ~= 0 then
		return UNSMELT_BOXISLOCK
	end

	if RemoveItemByIndex(nXHTIdx) ~= 1 then
		return SMELT_UNKNOWN_ERROR
	end
	Pay(nNeedMoney)

	nResult = UnSmeltEquip(nEquIdx, 0)
	if nResult ~= UNSMELT_OK then
		return nResult
	end

	SyncItem(nEquIdx)
	for i, v in pairs(tbFusionInfo) do
		local nFusionIdx = AddItem(8, 1, i, 1, 0, 0, 0)
		if nFusionIdx and nFusionIdx > 0 then
			if UnSmeltIsBind(nFusionIdx) == 1 then
				SetItemBindState(nFusionIdx, -2)
			end
			SetFusionIsSmelted(nFusionIdx, 1)
			SetFusionMagicSeed(nFusionIdx, v)
			SyncItem(nFusionIdx)
			local nEquG, nEquD, nEquP = GetItemProp(nEquIdx)
			local nFusionG, nFusionD, nFusionP = GetItemProp(nFusionIdx)
			WriteLog(format("[SmeltSys][UnSmeltEquip]\tName:%s\tAccount:%s\tEquip:%s\tEquipGDP:%d,%d,%d\tWenGang:%s\tWenGangGDP:%d,%d,%d\tMoney:%d",
				GetName(), GetAccount(), GetItemName(nEquIdx) or "", nEquG, nEquD, nEquP, GetItemName(nFusionIdx) or "", nFusionG, nFusionD, nFusionP, nNeedMoney))
		end
	end

	if EventSys then
		EventSys:OnPlayerEvent("OnUnSmeltEquipment", PlayerIndex, nEquIdx)
	end
	return nResult
end

-- ================= give-box (thay goi c2s 182 / SmeltItem() cua Linux) =================
function FUS_MoBox()
	OpenSmeltBox("Dung luyÖn V¨n C­¬ng", "", "doSmeltBox")
end

-- menu NPC "Dung luyen Van Cuong" (Linux: onSmelt -> SmeltItem())
function onSmelt()
	FUS_MoBox()
end

-- Lay (trang bi, nguyen lieu) trong khay: Region.h 0 = trang bi, 2 = nguyen lieu (GetGiveItemSlot)
function FUS_LayDoTrongKhay(nCount)
	local nEqu, nMat = 0, 0
	nCount = nCount or 0
	for i = 1, nCount do
		local nIdx = GetGiveItemUnit(i)
		local nSlot = -1
		if GetGiveItemSlot then
			nSlot = GetGiveItemSlot(i)
		end
		if nIdx and nIdx > 0 then
			if nSlot == 0 then
				nEqu = nIdx
			elseif nSlot == 2 then	-- [DUNGLUYEN-PB 01/09] cot 2 (cot 1 bi trang bi rong 2 o chiem -> PlaceItem tu choi)
				nMat = nIdx
			end
		end
	end
	return nEqu, nMat
end

function doSmeltBox(nCount)
	if not PlayerIndex then
		return 0
	end
	local nEqu, nMat = FUS_LayDoTrongKhay(nCount)
	if nEqu <= 0 or nMat <= 0 then
		SmeltMsg(SMELT_NOT_EQUIP)
		FUS_MoBox()
		return 0
	end
	local nResult = DoSmeltEquip(nEqu, nMat)
	SmeltMsg(nResult)
	FUS_MoBox()
	return 1
end

function doUnSmeltBox(nCount)
	if not PlayerIndex then
		return 0
	end
	local nEqu, nMat = FUS_LayDoTrongKhay(nCount)
	if nEqu <= 0 or nMat <= 0 then
		SmeltMsg(UNSMELT_NOT_EQUIP)
		FUS_MoBox()
		return 0
	end
	local nResult = DoUnSmeltEquip(nEqu, nMat)
	SmeltMsg(nResult)
	FUS_MoBox()
	return 1
end
