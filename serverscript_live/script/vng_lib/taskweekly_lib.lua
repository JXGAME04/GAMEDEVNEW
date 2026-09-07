-- ================================================================================================
-- [HE THONG] script/vng_lib/taskweekly_lib.lua
-- Muc dich  : Dem hoat dong theo tuan vao task.
-- Duoc nap  : Include tu 1 tep (vd activitydetail.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Include   : functionlib.lua
-- Ham (dong): VngTaskWeekly:GetWeeklyCount (4), VngTaskWeekly:AddWeeklyCount (16), VngTaskWeekly:CheckTaskWeekly (24)
-- Sua nong  : CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
--L­u sè lÇn ho¹t ®éng trong tuÇn vµo task - Created by DinhHQ - 20110709
Include("\\script\\activitysys\\functionlib.lua")
VngTaskWeekly = {}

function VngTaskWeekly:GetWeeklyCount(nTaskID)
	local nTaskVal = GetTask(nTaskID)
	local nCurDate = tonumber(GetLocalDate("%y%W"))
	local nDate = mod(nTaskVal, 10000)
	if nDate ~= nCurDate then
		nTaskVal = nCurDate
		nDate = nCurDate
		SetTask(nTaskID, nCurDate)
	end
	return ((nTaskVal - nDate)/10000)
end

function VngTaskWeekly:AddWeeklyCount(nTaskID, nValue)
	local nCount = self:GetWeeklyCount(nTaskID)
	nCount = nCount + nValue
	local nCurDate = tonumber(GetLocalDate("%y%W"))
	local nTaskVal = nCount * 10000 + nCurDate	
	SetTask(nTaskID, nTaskVal)
end

function VngTaskWeekly:CheckTaskWeekly(nTaskId, nValue, szFailMsg, szOption)
	nValue = lib:NumberParamTrans(nValue)	
	local nTaskValue =  VngTaskWeekly:GetWeeklyCount(nTaskId)		
	if lib:OptionFunction(nTaskValue, nValue, szOption) then
		return 1
	else
		lib:ShowMessage(szFailMsg)
	end	
end