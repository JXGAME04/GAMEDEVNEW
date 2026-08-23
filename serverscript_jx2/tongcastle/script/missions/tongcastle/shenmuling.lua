Include("\\script\\lib\\awardtemplet.lua")

tbTask = {
	["6,1,3205"] = 4058,
	["6,1,3206"] = 4059,
	["6,1,3207"] = 4060,
}

MAX_COUNT_PER_WEEK = 5

tbAward = {
	["6,1,3205"] = {nExp = 10000000},
	["6,1,3206"] = {nZhenYuanPoint = 20},
	["6,1,3207"] = {
			{nExp = 60000000},
			{nZhenYuanPoint = 120},
		},
}

function main(nItemIndex)
	local nG, nD, nP = GetItemProp(nItemIndex)
	local szGDP = format("%d,%d,%d", nG, nD, nP)
	
	local award = tbAward[szGDP]
	if not award then
		return 1
	end
	local nTaskId = tbTask[szGDP]
	local nTaskValue = GetTask(nTaskId)
	local nSaveWeek = GetByte(nTaskValue, 1)
	local nUseCount = GetByte(nTaskValue, 2)
	local nCurWeek = tonumber(GetLocalDate("%W"))
	if nSaveWeek ~= nCurWeek then
		nUseCount = 0
		nTaskValue = SetByte(nTaskValue, 1, nCurWeek)
	end
	
	if nUseCount >= MAX_COUNT_PER_WEEK then
		Talk(1, "", format("§¹o cô nµy mçi tuÇn chØ cã thÓ sö dông nhiÒu nhÊt %d c¸i.", MAX_COUNT_PER_WEEK))
		return 1
	end
	
	nTaskValue = SetByte(nTaskValue, 2, nUseCount + 1)
	SetTask(nTaskId, nTaskValue)
	tbAwardTemplet:Give(award, 1, {"TongCastle", "UseShenMuLing"})
	
end