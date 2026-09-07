-- ================================================================================================
-- [HE THONG] script/vng_lib/extpoint.lua
-- Muc dich  : Diem mo rong (VNG).
-- Duoc nap  : Include tu 3 tep (vd activitydetail.lua, check_func.lua, lenhbai_def.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Include   : extpoint_head.lua, log.lua
-- Ham (dong): tbExtPointLib:GetBitValue (3), tbExtPointLib:SetBitValue (8), tbExtPointLib:GetBitValueByVersion (35), tbExtPointLib:SetBitValueByVersion (42), tbExtPointLib:CheckBitVersion (52)
-- Sua nong  : CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
Include("\\script\\vng_lib\\extpoint_head.lua")
Include("\\script\\lib\\log.lua")

function tbExtPointLib:GetBitValue(nExtPoint, nBitPos)
	local nVal = GetExtPoint(nExtPoint)
	return(GetBit(nVal, nBitPos))
end

function tbExtPointLib:SetBitValue(nExtPoint, nBitPos, nBitValue)
--Bæ sung kiÓm tra gi¸ trÞ extpoint ©m - Modified by DinhHQ - 20110814	
	local nVal = GetExtPoint(nExtPoint)
	local nOldVal = nVal
	local nReturnValue = nil
	if nVal < 0 then
		return 0
	end
	if (nBitValue == 0) then		
		nVal = SetBit(nVal,nBitPos, 0)
		nVal = GetExtPoint(nExtPoint) - nVal
		nReturnValue = PayExtPoint(nExtPoint, nVal)		
	elseif (nBitValue == 1) then
		nVal = SetBit(nVal, nBitPos, 1)
		nVal = nVal - GetExtPoint(nExtPoint)
		nReturnValue =  AddExtPoint(nExtPoint, nVal)
	end
	local nNewValue = GetExtPoint(nExtPoint)
	if nReturnValue == 1 then
		tbLog:PlayerActionLog("Extpoint Changed Successfully", "Old Value: "..nOldVal, "New Value: "..nNewValue)
	else
		tbLog:PlayerActionLog("Extpoint Changed Fail", "Old Value: "..nOldVal, "New Value: "..nNewValue)
	end
	return nReturnValue
end
--format table tbExtPointLib = {nID= x, nBit = y, nVer = z}
function tbExtPointLib:GetBitValueByVersion(tbExtpoint)
	if self:CheckBitVersion(tbExtpoint) == 1 then
		return self:GetBitValue(tbExtpoint.nID, tbExtpoint.nBit)
	else		
		return nil
	end
end

function tbExtPointLib:SetBitValueByVersion(tbExtpoint, nBitValue)
	if self:CheckBitVersion(tbExtpoint) ~= 1 then
		return nil
	else		
		if self:SetBitValue(tbExtpoint.nID, tbExtpoint.nBit, nBitValue) > 0 then
			return 1
		end
	end
end

function tbExtPointLib:CheckBitVersion(tbExtpoint)
	if not tbExtpoint or type(tbExtpoint) ~= "table" then
		return nil
	end
	local nID = tbExtpoint.nID
	local nBit = tbExtpoint.nBit
	local nVer = tbExtpoint.nVer
	if self.tbList[nID][nBit] == nVer then
		return 1
	end
	error("Bit version is mismatched!!!")
	return nil
end