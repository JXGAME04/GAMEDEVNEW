-- SINH TU DONG [BDH-G5] bdh_killhook.lua - dem giet quai cho cuon tu luyen dong hanh
-- Duoc danhquai.lua Include + goi trong OnDeathMonsterDaTau (PlayerIndex DA duoc dat).
-- event_killnpc.txt: cot 4=TaskID(dem) 6=KillCount 7=MapID 8=NpcTemplateID
Include("\\script\\task\\partner\\bdh_compat.lua")
Include("\\script\\item\\reward_partner.lua")

function BDH_OnKillNpc(nNpcIdx)
	local nEvent = GetTask(1237)
	if (nEvent < 6 or nEvent > 386) then
		return
	end
	local nRow = nEvent + 1
	local nTpl = tonumber(TabFile_GetCell("bdh_killevent", nRow, 8, "-1"))
	local nNeed = tonumber(TabFile_GetCell("bdh_killevent", nRow, 6, "0"))
	if (nTpl == nil or nNeed == nil or nNeed <= 0) then
		return
	end
	if (nTpl ~= -1 and GetNpcSettingIdx(nNpcIdx) ~= nTpl) then
		return
	end
	local nCount = GetTask(1236) + 1
	SetTask(1236, nCount)
	if (nCount >= nNeed) then
		reward_killfinish(PlayerIndex, 1236, nEvent)
	end
end
