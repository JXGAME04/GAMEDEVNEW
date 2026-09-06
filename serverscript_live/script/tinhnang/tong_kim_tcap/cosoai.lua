-- Author: Fong Ki“u
-- Date: 28/11/2016
-- Function: CÍ so∏i

Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_task.lua")

function main(NpcIndex)
	
	dofile("script/tinhnang/tong_kim_tcap/cosoai.lua")

	local nTime = GetMSRestTime(MS_TONGKIM,1)
	local nSecond = floor(nTime/18)
	if (nTime > 0) then
		SetFightState(1)
		Talk(1,"","ßπi chi’n TËng Kim cﬂn <color=red>"..nSecond.."<color> gi©y sœ bæt Æ«u")
		return
	end
	
	local m_npcSoai = 0
	if(nPhe == 1) then
		m_npcSoai = GetMission(M_NPCIDX_SOAITONG)
	else
		m_npcSoai = GetMission(M_NPCIDX_SOAIKIM)		
	end
	
	if(m_npcSoai > 0) then
		SetNpcWalkOriginPos(m_npcSoai)
		Msg2Player("m_npcSoai:".. m_npcSoai)
	end	
	
	Talk(1,"","B∏o !!! qu©n t◊nh Æang nguy c p.")
end

function no()
end