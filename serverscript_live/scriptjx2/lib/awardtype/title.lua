Include("\\script\\lib\\awardtemplet.lua")
Include("\\script\\missions\\leaguematch\\head.lua")
Include("\\script\\lib\\log.lua")
--Ghi transaction log - Modified By DinhHQ - 20110816
Include("\\script\\vng_lib\\VngTransLog.lua")
IncludeLib("TITLE")
TitleType = {}
TitleType.nPak = curpack()


function TitleType:Give(tbItem, nAwardCount, tbLogTitle)
	if (not tbItem.nTitleId) or (not tbItem.nTime) or (not tbItem.nTimeType)then
		return 
	end
	
	local nPlayerIndex = PlayerIndex
	local nPak = usepack(self.nPak)
	CallPlayerFunction(nPlayerIndex, Title_AddTitle, tbItem.nTitleId, tbItem.nTimeType, tbItem.nTime)
	CallPlayerFunction(nPlayerIndex, self.WriteLog, self, 1, tbLogTitle)
	CallPlayerFunction(nPlayerIndex, Msg2Player, format("NhÀn Æ≠Óc danh hi÷u [%s]", Title_GetTitleName(tbItem.nTitleId)))
	usepack(nPak)
end

function TitleType:WriteLog(nAmount, tbLogTitle)
	local szCode = ""
	if tbLogTitle then
		local szEventName = tbLogTitle[1] or ""
		local szAction = tbLogTitle[2] or "award"
		local szName = "nTitleId"
		call(tbLog.PlayerAwardLog, {tbLog, szEventName, szAction, szName, szCode, nAmount}, "x")
		if tbLogTitle[3] and type(tbLogTitle[3]) == "table" then
			local tb = tbLogTitle[3]
			local strAction = tb.strAction or szAction
			call(tbVngTransLog.Write, {tbVngTransLog, tb.strFolder, tb.nPromID, strAction, nAmount.." "..szName, tb.nResult}, "x")
		end
	end
end

tbAwardTemplet:RegType("nTitleId", TitleType)