-- ================================================================================================
-- [HE THONG] script/vng_lib/bittask_lib.lua
-- Muc dich  : Bit task (VNG).
-- Duoc nap  : Include tu 4 tep (vd activitydetail.lua, ipbonus_2_head.lua, exp_award.lua, give_support_item.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Include   : functionlib.lua
-- Ham (dong): tbVNG_BitTask_Lib:getBitTask (5), tbVNG_BitTask_Lib:setBitTask (9), tbVNG_BitTask_Lib:addTask (16), tbVNG_BitTask_Lib:isMaxBitTaskValue (24), tbVNG_BitTask_Lib:IsMaxValue (30), tbVNG_BitTask_Lib:CheckBitTaskValue (37)
-- Sua nong  : CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
Include("\\script\\activitysys\\functionlib.lua")
if not tbVNG_BitTask_Lib then
	tbVNG_BitTask_Lib = {}
end

function tbVNG_BitTask_Lib:getBitTask(tbTaskInfo)
	return GetBitTask(tbTaskInfo.nTaskID, tbTaskInfo.nStartBit, tbTaskInfo.nBitCount) 
end
function tbVNG_BitTask_Lib:setBitTask(tbTaskInfo, nValue)
	SetBitTask(tbTaskInfo.nTaskID, tbTaskInfo.nStartBit, tbTaskInfo.nBitCount, nValue) 
	if self:getBitTask(tbTaskInfo) ~= nValue then
		return nil
	end
	return 1
end
function tbVNG_BitTask_Lib:addTask(tbTaskInfo, nValue)
	local nCurValue = self:getBitTask(tbTaskInfo)
	local nNextValue = nCurValue + nValue
	if nNextValue > tbTaskInfo.nMaxValue then
		nNextValue = tbTaskInfo.nMaxValue
	end
	self:setBitTask(tbTaskInfo, nNextValue)
end
function tbVNG_BitTask_Lib:isMaxBitTaskValue(tbTaskInfo)
	if (self:getBitTask(tbTaskInfo) >= tbTaskInfo.nMaxValue) then
		return 1;
	else return 0;
	end
end

function tbVNG_BitTask_Lib:IsMaxValue(tbTaskInfo)
	if (self:getBitTask(tbTaskInfo) >= tbTaskInfo.nMaxValue) then
		return 1;
	else return nil;
	end
end

function tbVNG_BitTask_Lib:CheckBitTaskValue(tbTaskInfo, nValue, szFailMsg, szOption)
	nValue = lib:NumberParamTrans(nValue)	
	local nBitTaskValue =  self:getBitTask(tbTaskInfo)
	if lib:OptionFunction(nBitTaskValue, nValue, szOption) then
		return 1
	else
		lib:ShowMessage(szFailMsg)
	end	
end