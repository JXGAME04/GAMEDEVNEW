-- ==========================================================================
-- composeclass.lua - PORT tu ban Linux sang JX1  (Viem De, 26/08)
-- Sinh boi ReverseTools\viemde\v21_port_composeclass.py - DUNG SUA TAY
--
-- Nguyen van ban Linux, CHI khac mot cho: bo Include \script\lib\string.lua
-- va mang theo DUNG mot ham cua no ma tep nay can (transferDigit2CnNum).
-- Ly do: string.lua dinh nghia replace/split/join/trim - JX1 DA CO cac ham
-- do trong \script\lib\common.lua, chep ca tep vao se che mat ban cua JX1.
-- ==========================================================================

--UI
--tbComposeClass:GetMaterialList(tbMaterial) 
--  /
--
-- tbComposeClass:Compose(tbFormula, szLogTitle, pFun, ...)
--01
--
--tbFormula
--tbFormula = {tbMaterial£¬tbProduct}
--
--tbMaterial \script\lib\awardtemplet.lua tbItem
--
--tbProduct  \script\lib\awardtemplet.lua nil





Include("\\script\\task\\system\\task_string.lua");
Include("\\script\\lib\\awardtemplet.lua")
Include("\\script\\lib\\baseclass.lua")
-- [JX1 26/08] BO Include string.lua (xem dau tep). Mang theo dung 1 ham:
function transferDigit2CnNum(nDigit)
	local tbModelUnit = {
		[1]	= "",
		[2]	= " v¹n",
		[3] = " tr¨m triÖu", 	
	};
	
	local nDigitTmp = nDigit;	-- ,
	local n4LenNum	= 0;		-- nDigit4,n4LenNum
	local nPreNum	= 0;		-- 4n4LenNum
	local szCnNum	= "";		-- 
	local szNumTmp	= "";		-- ,
	
	if (nDigit == 0) then
		szCnNum = "0";
		return szCnNum;
	end

	if (nDigit < 0) then
		nDigitTmp = -nDigit;
	end
	
	-- ,,,nDigit1,4
	for i = 1, getn(tbModelUnit) do
		szNumTmp	= "";
		n4LenNum	= mod(nDigitTmp , 10000);
		if (n4LenNum ~= 0) then
			--szNumTmp = transfer4LenDigit2CnNum(n4LenNum);					-- 
			szNumTmp = n4LenNum
			szNumTmp = szNumTmp..tbModelUnit[i];								-- 
		end
		szCnNum	= szNumTmp..szCnNum;
		
		nPreNum	= n4LenNum;
		nDigitTmp = floor(nDigitTmp / 10000);
		if (nDigitTmp == 0) then
			break;
		end
	end
	if (nDigit < 0) then
		szCnNum = "-"..szCnNum;
	end
	return szCnNum;
end

if not tbComposeClass then
	
tbComposeClass = tbBaseClass:new()


--:
--
--ÎÞ
function tbComposeClass:_init(szClassName, tbFormula, szLogTitle)
	szClassName = szClassName or "tbComposeClass"
	
	self.szClassName = szClassName
	self.tbFormula = tbFormula

	self.szLogTitle = szLogTitle or "MÆc ®inh hîp thµnh"
	setglobal(szClassName, self)
	
	self:MakeAskNumberFunction()
	
end

--
--
--(1/0)
function tbComposeClass:CheckMaterial(tbMaterial, nComposeCount)
	local i
	local flag = 1
	local tbCount  = {}
	local tbMaxSetCount = {}
	nComposeCount = nComposeCount or 1
	
	for i=1,getn(tbMaterial) do
		local tbItem = tbMaterial[i]
		if tbItem.tbProp then
			tbItem.nCount = tbItem.nCount or 1
			local nItemNeedCount = tbItem.nCount * nComposeCount
			if nItemNeedCount > 0 then
				local tbProp = tbItem.tbProp
				tbProp[4] = tbProp[4] or -1		
				local nCurCount = CalcEquiproomItemCount(tbProp[1], tbProp[2], tbProp[3], tbProp[4])
				tbCount[i] = nCurCount
				tbMaxSetCount[i] = floor(nCurCount / nItemNeedCount)
				if nCurCount < nItemNeedCount then
					flag =  0;
				end	
			end			
		elseif tbItem.nJxb then
			local nCash = GetCash()
			tbCount[i] = nCash
			local nNeedJxb = tbItem.nJxb * nComposeCount
			tbMaxSetCount[i] = floor(nCash / nNeedJxb)
			if nCash < nNeedJxb then
				flag = 0
			end
		elseif tbItem.pGetCount then
			local nCurCount = tbItem:pGetCount()
			tbItem.nCount = tbItem.nCount or 1
			local nNeedCount = tbItem.nCount * nComposeCount
			tbCount[i] = nCurCount
			tbMaxSetCount[i] = floor(nCurCount / nNeedCount)
			if nCurCount < nNeedCount then
				flag =  0;
			end	
		end
	end
	return flag, tbCount, tbMaxSetCount;
end

--
--
--
function tbComposeClass:CanMakeMaxCount(tbMaterial)
	local _, _, tbMaxCount = self:CheckMaterial(tbMaterial, 1)
	local nMinCount = tbMaxCount[1]
	for i=2, getn(tbMaxCount) do
		if nMinCount > tbMaxCount[i] then
			nMinCount = tbMaxCount[i]
		end
	end
	return nMinCount;
end

--
--
--(1/0)
function tbComposeClass:ConsumeMaterial(tbMaterial, nConsumeCount, szLogTitle)
	local i
	nConsumeCount = nConsumeCount or 1
	for i=1,getn(tbMaterial) do
		local tbItem = tbMaterial[i]
		if tbItem.tbProp then				
			tbItem.nCount = tbItem.nCount or 1
			local nConsumeItemCount = tbItem.nCount * nConsumeCount
			if nConsumeItemCount > 0 then
				local tbProp = tbItem.tbProp
				tbProp[4] = tbProp[4] or -1
				--print(tbProp[1], tbProp[2], tbProp[3], tbProp[4])
				if ConsumeEquiproomItem(nConsumeItemCount, tbProp[1], tbProp[2], tbProp[3], tbProp[4]) ~= 1 then
					return 0;
				end
				self:ConsumeLog(nConsumeItemCount.." "..tbItem.szName, szLogTitle)
			end
		elseif tbItem.nJxb then
			local nConsumeJxb = tbItem.nJxb * nConsumeCount
			if Pay(nConsumeJxb) == 0 then
				return 0;
			else
				Msg2Player(format("CÇn <color=yellow>%d<color> l­îng", nConsumeJxb))
				self:ConsumeLog("Jxb "..nConsumeCount.." * "..tbItem.nJxb, szLogTitle)
			end
		elseif tbItem.pConsume then
			local nConsumeCount = tbItem.nCount * nConsumeCount
			if tbItem:pConsume(nConsumeCount) ~= 1 then
				return 0;
			end
			self:ConsumeLog(nConsumeCount.." "..tbItem.szName, szLogTitle)
		end
	end
	return 1;
end


--
--
--
function tbComposeClass:GetMaterialList(tbMaterial)
	local szList = format("%-20s  %s","vËt phÈm ","Sè l­îng")
	local _, tbCount, tbMaxCount = self:CheckMaterial(tbMaterial, 1)
	local i;
	for i=1,getn(tbMaterial) do
		
		local szColor = "<color=green>"
		if tbMaxCount[i] < 1 then
			szColor = "<color=red>"
		end
		local tbItem = tbMaterial[i]
		if tbItem.nJxb then
			szList = format("%s<enter><color=yellow>%-20s<color>  %s(%d/%d)<color>",szList,"Ng©n l­îng", szColor, tbCount[i], tbMaterial[i].nJxb)
		elseif tbItem.szName and tbItem.nCount then
			szList = format("%s<enter><color=yellow>%-20s<color>  %s(%d/%d)<color>",szList,tbMaterial[i].szName, szColor, tbCount[i], tbMaterial[i].nCount)
		end
	end
	return szList
end



--
--tablestring
--(1/0)
function tbComposeClass:Compose(tbFormula, szLogTitle, nComposeCount, pFun, ...) local arg = {n = select("#", ...), ...};
	--
	tbFormula	= tbFormula or self.tbFormula
	szLogTitle	= szLogTitle or self.szLogTitle
	
	
	local tbMaterial	= tbFormula.tbMaterial
	local tbProduct		= tbFormula.tbProduct
	
	nComposeCount = nComposeCount or 1
	
	
	if tbFormula.pLimitFun then
		if tbFormula:pLimitFun(nComposeCount) ~= 1 then
			return 0
		end
	end
	
	local nFreeItemCellLimit = tbFormula.nFreeItemCellLimit or 1
	
	nFreeItemCellLimit = ceil(nFreeItemCellLimit * nComposeCount)
	
	if CalcFreeItemCellCount() < nFreeItemCellLimit then
		Say(format("§Ó b¶o ®¶m an toµn tµi s¶n, xin h·y ®¶m b¶o hµnh trang cßn thõa %d « trèng.", nFreeItemCellLimit))
		return 0
	end
	
	
	if self:CheckMaterial(tbMaterial, nComposeCount) ~=1 then
		local szMsg = tbFormula.szFailMsg or "<color=red>§¹i hiÖp mang nguyªn liÖu kh«ng ®ñ råi!<color>"
		Talk(1, "", szMsg)
		return 0;
	end

	if self:ConsumeMaterial(tbMaterial, nComposeCount, szLogTitle) ~= 1 then
		--Say("",0)
		Msg2Player("ChÕ t¹o thÊt b¹i, mÊt ®i mét sè nguyªn liÖu.")
		return 0;
	end
	if type(tbProduct) == "table" then
		tbAwardTemplet:GiveAwardByList(tbProduct, szLogTitle, nComposeCount)
	end
	
	if type(pFun) == "function" then
		call(pFun, arg)
	end 
	return 1;
end

--
--()table
--
function tbComposeClass:ComposeDailog(tbFormula, szFunctionFormat, bIsAskNumber)
	--
	tbFormula			= tbFormula or self.tbFormula
	local szDefaultFunctionFormat = format("#%s:Compose(nil,nil, 1)", self.szClassName)
	if bIsAskNumber then
		szDefaultFunctionFormat = format("#%s:AskNumber()", self.szClassName)
	end
	szFunctionFormat	= szFunctionFormat or szDefaultFunctionFormat
	local tbMaterial = tbFormula.tbMaterial
	local tbProduct = tbFormula.tbProduct
	local szComposeTitle = tbFormula.szComposeTitle or format("§æi %s", tbProduct.szName)
	local szMsg = format("<dec>%s cÇn: <enter>%s", szComposeTitle ,self:GetMaterialList(tbMaterial))
	local tbSay = 
	{
		szMsg,
		format("X¸c nhËn/%s", szFunctionFormat ),
		"Hñy bá /OnCancel"
	}
	CreateTaskSay(tbSay)
end


--
--string
--
function tbComposeClass:ConsumeLog(szItemName, szLogTitle)
	WriteLog(format("[%s]\t%s\tAccount:%s\tName:%s\t consume %s.",szLogTitle,GetLocalDate("%Y-%m-%d %H:%M"), GetAccount(), GetName(), szItemName))
end


function tbComposeClass:AskNumber()	
	local nMaxCount = self:CanMakeMaxCount(self.tbFormula.tbMaterial)
	if nMaxCount < 1 then
		local szMsg = self.tbFormula.szFailMsg or "<color=red>§¹i hiÖp mang nguyªn liÖu kh«ng ®ñ råi!<color>"
		Talk(1, "", szMsg)
	else
		AskClientForNumber(format("%s__AskNumberCallBack", self.szClassName), 1, nMaxCount, "Xin mêi nhËp sè");
	end
end

function tbComposeClass:ComposeCountComfirm(nCount)
	
	local nMaxCount = self:CanMakeMaxCount(self.tbFormula.tbMaterial)
	if nCount > nMaxCount then
		nCount = nMaxCount
	end
	
--	local nFreeItemCellLimit = self.tbFormula.nFreeItemCellLimit or 1
--	
--	nFreeItemCellLimit = ceil(nFreeItemCellLimit * nCount)
--	
--	if CalcFreeItemCellCount() < nFreeItemCellLimit then
--		return Say(format("%d", nFreeItemCellLimit))
--	end

	if self:Compose(nil, nil, nCount) == 0 then
		return 0;
	end
	
	
	return 1;
end

function tbComposeClass:MakeAskNumberFunction()
	local szFunctionName = format("%s__AskNumberCallBack", self.szClassName)
	local szMsg = format("function %s(nCount) local self = getglobal([[%s]]) return self:ComposeCountComfirm(nCount) end",
					szFunctionName, self.szClassName)
	dostring(szMsg)
end


function tbComposeClass:GetProductName(tbFormula)
	tbFormula	= tbFormula or self.tbFormula
	local tbProduct		= tbFormula.tbProduct
	return tbProduct.szName
end

function tbComposeClass:GetFormulaByString(tbFormula)
	tbFormula	= tbFormula or self.tbFormula
	local tbMaterial = tbFormula.tbMaterial
	local szMsg = nil
	for i=1, getn(tbMaterial) do
		local szName
		if tbMaterial[i].nJxb then
			szName = transferDigit2CnNum(tbMaterial[i].nJxb).." l­îng"
		else
			szName = tbMaterial[i].szName.."X"..(tbMaterial[i].nCount or 1)
		end
		
		if not szMsg then
			szMsg = szName
		else
			szMsg = szMsg.."+"..szName	
		end
	end
	return szMsg
end


tbComposeClass:_init()

end