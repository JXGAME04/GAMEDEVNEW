
-- ÎåÐÐÓ¡Éý¼¶
-- 2011.04.25

--Change the name "Ngò hµnh Ên" to "Hoµng kim Ên" - Modified by DinhHQ - 20110605

Include("\\script\\lib\\composeex.lua")
Include("\\script\\misc\\eventsys\\type\\npc.lua")
Include("\\script\\lib\\log.lua")

tbWuxingyin = tbActivityCompose:new()

local tbSuccessRate = {
	[1] = 35,		
	[2] = 45,
	[3] = 55,
	[4] = 65,
	[5] = 75,
	}
	
local tbLogAction = {
	[1] = "HoangKimAnCuongHoaCap2ThanhCong",
	[2] = "HoangKimAnCuongHoaCap3ThanhCong",
	[3] = "HoangKimAnCuongHoaCap4ThanhCong",
	[4] = "HoangKimAnCuongHoaCap5ThanhCong",
	[5] = "HoangKimAnCuongHoaCap6ThanhCong",
	[6] = "HoangKimAnNhuocHoaCap2ThanhCong",
	[7] = "HoangKimAnNhuocHoaCap3ThanhCong",
	[8] = "HoangKimAnNhuocHoaCap4ThanhCong",
	[9] = "HoangKimAnNhuocHoaCap5ThanhCong",
	[10] = "HoangKimAnNhuocHoaCap6ThanhCong",
	[11] = "HoangKimAnCuongHoaCap2ThatBai",
	[12] = "HoangKimAnCuongHoaCap3ThatBai",
	[13] = "HoangKimAnCuongHoaCap4ThatBai",
	[14] = "HoangKimAnCuongHoaCap5ThatBai",
	[15] = "HoangKimAnCuongHoaCap6ThatBai",
	[16] = "HoangKimAnNhuocHoaCap2ThatBai",
	[17] = "HoangKimAnNhuocHoaCap3ThatBai",
	[18] = "HoangKimAnNhuocHoaCap4ThatBai",
	[19] = "HoangKimAnNhuocHoaCap5ThatBai",
	[20] = "HoangKimAnNhuocHoaCap6ThatBai",
	}	

local tbMaterialA = {
		[1]={szName="Hoµng kim Ên ( c­êng hãa cÊp 1)",tbProp={0,3205,},nQuality=1,},		-- level 1
		[2]={szName="Hoµng kim Ên ( c­êng hãa cÊp 2)",tbProp={0,3206,},nQuality=1,},
		[3]={szName="Hoµng kim Ên ( c­êng hãa cÊp 3)",tbProp={0,3207,},nQuality=1,},
		[4]={szName="Hoµng kim Ên ( c­êng hãa cÊp 4)",tbProp={0,3208,},nQuality=1,},
		[5]={szName="Hoµng kim Ên ( c­êng hãa cÊp 5)",tbProp={0,3209,},nQuality=1,},
		[6]={szName="Hoµng kim Ên ( nh­îc hãa cÊp 1)",tbProp={0,3215,},nQuality=1,},		-- level 1	
		[7]={szName="Hoµng kim Ên ( nh­îc hãa cÊp 2)",tbProp={0,3216,},nQuality=1,},
		[8]={szName="Hoµng kim Ên ( nh­îc hãa cÊp 3)",tbProp={0,3217,},nQuality=1,},
		[9]={szName="Hoµng kim Ên ( nh­îc hãa cÊp 4)",tbProp={0,3218,},nQuality=1,},
		[10]={szName="Hoµng kim Ên ( nh­îc hãa cÊp 5)",tbProp={0,3219,},nQuality=1,},
		}

local tbAdditive = {szName="V­¬ng ThiÕt T­îng LÖnh Phï",tbProp={6,1,4889,},}	-- ÍõÌú½³Áî·û

local tbFormulaList = {
	nWidth = 1,
	nHeight = 1,
	nFreeItemCellLimit = 1,
	
	tbMaterial = {},
	
	tbProduct = {
		[1]={szName="Hoµng kim Ên ( c­êng hãa cÊp 2)",tbProp={0,3206,},nQuality=1,nExpiredTime=7*24*60,},		-- level 2
		[2]={szName="Hoµng kim Ên ( c­êng hãa cÊp 3)",tbProp={0,3207,},nQuality=1,nExpiredTime=7*24*60,},
		[3]={szName="Hoµng kim Ên ( c­êng hãa cÊp 4)",tbProp={0,3208,},nQuality=1,nExpiredTime=7*24*60,},
		[4]={szName="Hoµng kim Ên ( c­êng hãa cÊp 5)",tbProp={0,3209,},nQuality=1,nExpiredTime=7*24*60,},
		[5]={szName="Hoµng kim Ên ( c­êng hãa cÊp 6 )",tbProp={0,3210,},nQuality=1,nExpiredTime=30*24*60,},
		[6]={szName="Hoµng kim Ên ( nh­îc hãa cÊp 2)",tbProp={0,3216,},nQuality=1,nExpiredTime=7*24*60,},		-- level 2
		[7]={szName="Hoµng kim Ên ( nh­îc hãa cÊp 3)",tbProp={0,3217,},nQuality=1,nExpiredTime=7*24*60,},
		[8]={szName="Hoµng kim Ên ( nh­îc hãa cÊp 4)",tbProp={0,3218,},nQuality=1,nExpiredTime=7*24*60,},
		[9]={szName="Hoµng kim Ên ( nh­îc hãa cÊp 5)",tbProp={0,3219,},nQuality=1,nExpiredTime=7*24*60,},
		[10]={szName="Hoµng kim Ên ( nh­îc hãa cÊp 6 )",tbProp={0,3220,},nQuality=1,nExpiredTime=30*24*60,},
		}
	}
	
function tbWuxingyin:GetRealLevel(nLevel)
	local nMaxLevel = 5
	if nLevel > nMaxLevel then
		nLevel = nLevel - nMaxLevel
	end
	return nLevel
end

-- ÅÐ¶ÏÊÇ·ñÉý¼¶³É¹¦
function tbWuxingyin:CanCompose(nLevel)
	nLevel = self:GetRealLevel(nLevel)
	local nNumber = random(1,100)
	if nNumber <= tbSuccessRate[nLevel] then
		return 1
	end
	return 0	
end

function tbWuxingyin:CalcItemCount(tbRoomItems, tbItem)
	local nTotalCount = 0
	for i=1, getn(tbRoomItems) do
		local nItemIndex = tbRoomItems[i]
		if nItemIndex and nItemIndex > 0 then
			local nCount = GetItemStackCount(nItemIndex)
			local nItemBindState = GetItemBindState(nItemIndex)	--»ñÈ¡°ó¶¨×´Ì¬,°ó¶¨µÄÎïÆ·²»¼ÆËã
			if nItemBindState == 0 and self:CheckItem(tbItem ,nItemIndex) == 1 then
				nTotalCount = nTotalCount + nCount
			end
		end
	end
	return nTotalCount	
end

-- ÅÐ¶¨UI¸øÓë½çÃæÖÐÊÇ·ñÓÐtbItem
function tbWuxingyin:CheckItemLegal(tbItem)
	local tbRoomItems = self:GetRoomItems(self.nRoomType)
	local nItemNeedCount = tbItem.nCount or 1
	if nItemNeedCount > 0 then
		local tbProp = tbItem.tbProp
		tbProp[4] = tbProp[4] or -1		
		local nCurCount = self:CalcItemCount(tbRoomItems, tbItem)
		if nCurCount < nItemNeedCount then
			return 0
		end	
	end			
	return 1
end

-- ¼ì²éÊÇ·ñºÏ·¨£¬ÈôºÏ·¨£¬·µ»ØºÏ³ÉµÄµÈ¼¶
function tbWuxingyin:CheckLegal()
	local tbItem = self.tbAdditive

	-- ÅÐ¶Ï²ÄÁÏµÄµÈ¼¶£¬ÊÇ·ñ°ó¶¨
	local tbMaterial = self.tbFormula.tbMaterial
	for i=1, getn(tbMaterial) do
		local tbSubMaterial = tbMaterial[i]
		local bLeap = 1
		for j=1, getn(tbSubMaterial) do
			tbItem = tbSubMaterial[j]
			if self:CheckItemLegal(tbItem) ~= 1 then
				bLeap = 0
				break
			end
		end
		
		if bLeap == 1 then
			return i
		end
	end
	return 0
end

-- Éý¼¶ÖÐÐèÒªÔö¼ÓÅÐ¶¨ºÏ³É¸ÅÂÊ£¬ÒÑ¾­ÐÞ¸Ä²ÄÁÏ±íµÈ
function tbWuxingyin:Compose(nComposeCount, nLevel)
	
	local tbMaterial	= self.tbFormula.tbMaterial[nLevel]	-- ¶¯Ì¬Éú³ÉºÏ³ÉµÄ²ÄÁÏ±í
	local tbProduct		= self.tbFormula.tbProduct[nLevel]
	
	nComposeCount = nComposeCount or 1
	
	if type(self.tbFormula.pLimitFun) == "function" then
		if self.tbFormula:pLimitFun(nComposeCount) ~= 1 then
			return 0
		end
	end
	
	local nFreeItemCellLimit = self.tbFormula.nFreeItemCellLimit or 1
	nFreeItemCellLimit = ceil(nFreeItemCellLimit * nComposeCount)
	if self.tbFormula.nWidth ~= 0 and self.tbFormula.nHeight ~= 0 and CountFreeRoomByWH(self.tbFormula.nWidth, self.tbFormula.nHeight, nFreeItemCellLimit) < nFreeItemCellLimit then
		Say(format("§Ó b¶o ®¶m tµi s¶n cña ®¹i hiÖp, xin h·y ®Ó trèng %d %dx%d hµnh trang", nFreeItemCellLimit, self.tbFormula.nWidth, self.tbFormula.nHeight))
		return 0
	end
	
	if self:CheckMaterial(tbMaterial, nComposeCount) ~=1 then
		local szMsg = self.tbFormula.szFailMsg or "<color=red>§¹i hiÖp mang nguyªn liÖu kh«ng ®ñ råi!<color>"
		Talk(1, "", szMsg)
		return 0;
	end
	
	-- ¶¯Ì¬ÉèÖÃÏûºÄ±í
	local bSuccess = self:CanCompose(nLevel)
	
	local tbConsume = {}
	if bSuccess ~= 1 then
		tinsert(tbConsume, tbMaterial[2])		-- ºÏ³ÉÊ§°ÜÖ»ÏûºÄÍõÌú½³Áî·û	
	else
		tbConsume = tbMaterial
	end
--	print("compose nLevel = " .. tostring(nLevel))
	if self:ConsumeMaterial(tbConsume, nComposeCount, self.szLogTitle) ~= 1 then
		Msg2Player("ChÕ t¹o thÊt b¹i, mÊt ®i mét sè nguyªn liÖu.")
		return 0;
	end
	
	if bSuccess == 1 then
		if type(tbProduct) == "table" then
			tbAwardTemplet:GiveAwardByList(tbProduct, self.szLogTitle, nComposeCount)
		end
		
		if type(self.tbFormula.pProductFun) == "function" then
			self.tbFormula:pProductFun(nComposeCount)
		end
	else
		local szMsg = format("<color=red>Hîp thµnh thÊt b¹i, mÊt ®i")
		for i=1,getn(tbConsume) do
			szMsg = szMsg .. tbConsume[i].szName
		end
		szMsg = szMsg .. "<color>"
		Msg2Player(szMsg)
	end
	
	self:Log(nLevel, bSuccess)		-- ÊÂ¼þÈÕÖ¾
	return bSuccess
end

function tbWuxingyin:Log(nLevel, bSuccess)
	if bSuccess == 0 then
		nLevel = nLevel + 10
	end
	tbLog:PlayerActionLog("HoangKimAn", tbLogAction[nLevel])		-- ÈÕÖ¾
end

-- ÐÞ¸ÄÉý¼¶ÖÐµÄÌáÊ¾µÀ¾ßÃû
function tbWuxingyin:ComposeGiveUI()
	
	local szTitle = "N©ng cÊp Hoµng kim Ên"
	local szContent = ""
	
	szContent = gsub(szContent, "<color=?%w*>", "")
	
	szContent = gsub(szContent, "%((%d+)/(%d+)%)", "%2")
	
	g_GiveItemUI(szTitle, szContent, {self.GiveUIOk, {self}}, nil, self.bAccessBindItem)
end

-- ÅÐ¶¨Ëù¸øµÄÎïÆ·ÊÇ·ñ·ûºÏÒªÇó
function tbWuxingyin:GiveUIOk(nCount)
	local nLevel = self:CheckLegal()
--	print("nLeve = " .. tonumber(nLevel))
	if nLevel > 0 then
		self:Compose(1, nLevel)
	else
		CreateNewSayEx("<color=red>§¹i hiÖp mang nguyªn liÖu kh«ng ®ñ råi!<color>", {})
	end
	return 
end


function tbWuxingyin:Register()
	local tbOpt = {}
	-- Éý¼¶ÎåÐÐÓ¡µÄ¶Ô»°ÄÚÈÝ£¬°üÀ¨Éý¼¶¹«Ê½µÈ
	local szMsg = "V­¬ng ThiÕt T­îng LÖnh Phï cã x¸c suÊt nhÊt ®Þnh t¨ng mét cÊp cña Hoµng kim Ên, nÕu t¨ng cÊp thÊt b¹i th× sÏ mÊt ®i V­¬ng ThiÕt T­îng LÖnh Phï, h×nh thøc cô thÓ nh­ sau:\n"
	for i=1,5 do 
		szMsg = szMsg .. format("Hoµng kim Ên %d cÊp ( nh­îc hãa/c­êng hãa) +1 V­¬ng ThiÕt T­îng LÖnh Phï = Hoµng kim Ên %d cÊp ( nh­îc hãa/c­êng hãa )\n",i,(i+1))
	end
	
	tbFormulaList.tbMaterial = {}
	local tbProduct = tbFormulaList.tbProduct
	
	for i=1,getn(tbMaterialA) do
		
		local tbMaterial = {}
		tinsert(tbMaterial, tbMaterialA[i])
		tinsert(tbMaterial, tbAdditive)
		tinsert(tbFormulaList.tbMaterial, tbMaterial)		-- ºÏ³É¹«Ê½µÄ²ÄÁÏ±í
		
	end
	
	local p = tbWuxingyin:new(tbFormulaList, "shengjiwuxingyin", INVENTORY_ROOM.room_giveitem)
	
	tinsert(tbOpt, {"N©ng cÊp Hoµng kim Ên", p.ComposeGiveUI, {p}})
	tinsert(tbOpt, {"Hñy bá "})
	nItem2EquipRegId = pEventType:Reg("Thî rÌn trong thµnh", "N©ng cÊp Hoµng kim Ên", CreateNewSayEx, {szMsg, tbOpt})
end


tbWuxingyin:Register()

-- ---------------------------------------------------------------------------
-- [JX1 29/08] Mo THANG bang nang cap Hoang Kim an, khong di qua eventsys.
--
-- Ban Linux tu gan minh bang pEventType:Reg("Tho ren trong thanh", ...) trong
-- Register() ngay tren. Cach do KHONG chay duoc tren JX1: Reg chi ghi vao bang
-- MapEvnent cua CHINH Lua_State nay, ma JX1 cho MOI tep .lua mot Lua_State
-- rieng, nen thoren.lua (hay lenh bai admin) khong bao gio doc duoc dang ky do.
-- Ban Linux dung mot state chung nen chay duoc.
--
-- Vi vay them mot ham PHANG: tep nao Include tep nay thi goi thang WXY_MoBang().
--
-- BAT BUOC goi tu chinh state DA Include tep nay. KHONG dung DynamicExecute:
-- ham do chay bang lua_dostring nen KHONG gan m_ActionScriptID, ma GiveItemUI
-- lai buoc hop give theo m_ActionScriptID -> callback se ve nham script va
-- G_PlayerGiveItemUIData ben kia rong.
g_pWuXingYin = nil;

function WXY_MoBang()
	if not g_pWuXingYin then
		-- %tbFormulaList: bien cuc bo cua tep nay, da duoc Register() o tren dien
		-- day tbMaterial luc nap, nen den day la dung du 10 cong thuc.
		g_pWuXingYin = tbWuxingyin:new(tbFormulaList, "shengjiwuxingyin", INVENTORY_ROOM.room_giveitem);
	end
	if not g_pWuXingYin then
		return 0;
	end
	g_pWuXingYin:ComposeGiveUI();
	return 1;
end
