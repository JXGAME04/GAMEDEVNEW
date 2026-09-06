-- Author: Fong Ki“u
-- Date: 28/11/2016
-- Function: CÍ so∏i

Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_task.lua")

function main(NpcIndex)
	
	dofile("script/tinhnang/tong_kim_tcap/cosoaitong.lua")

	local nTime = GetMSRestTime(MS_TONGKIM,1)
	local nSecond = floor(nTime/18)
	if (nTime > 0) then
		SetFightState(1)
		Talk(1,"","ßπi chi’n TËng Kim cﬂn <color=red>"..nSecond.."<color> gi©y sœ bæt Æ«u")
		return
	end
	
	local m_npcSoai = 0
	local nPlayerDataIdx = PIdx2MSDIdx(MS_TONGKIM, PlayerIndex)	
	local nPhe = GetMSIdxGroup(MS_TONGKIM, nPlayerDataIdx)	
	if(nPhe == 2) then
		return
	end
	m_npcSoai = GetMissionV(M_NPCIDX_SOAITONG)
	if(m_npcSoai > 0) then
		SetNpcWalkOriginPos(m_npcSoai)
		Msg2Player("m_npcSoai:".. m_npcSoai)
	end	
	
	--Talk(1,"","B∏o !!! qu©n t◊nh Æang nguy c p.")
end

function no()
end