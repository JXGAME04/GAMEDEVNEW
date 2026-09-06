--Author: Fong KiÒu
--Date: 19/08/2021
--Function: Npc Tiªu S­

Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\lib\\lib_sukien.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\event\\event_vantieu\\lib_vt.lua")

TIEUXABIHUY 								= "Ta kh«ng t×m thÊy tiªu xa cña ng­¬i."
NONHANNV 									= "Ng­¬i ch­a nhËn nhiÖm vô ¸p tiªu."
END_TALK 										= "Ta chØ ghÐ ngang qua./no"
NOT_ROAD 										= "Ng­¬i ®· ®i sai lé tr×nh."

function main(nNpcIndex)

	SetTaskTemp(TMP_INDEX_NPC, nNpcIndex)	

	-- dofile("script/event/event_vantieu/tieusu.lua")

	Say("Ng­¬i t×m ta cã viÖc g× ?", 2,
	"Ta ®Õn giao tiªu xa tõ Song ¦ng tiªu côc/tratieuxa",
	END_TALK)
	
end

function tratieuxa()
	
	local dwNpcID = GetTask(T_NPCID)
	local nNpcIdx = FindNpcFrID(dwNpcID)
	local nNpcKind = GetTask(T_LOAITIEUXA)
	local nSTP = GetTask(T_NHANNVVTIEU)
	local nParam = GetNpcValue(GetTaskTemp(TMP_INDEX_NPC))
	local msgLog = format("<color=green> %s <color> ®· vËn tiªu thµnh c«ng, nhanh chãng trë vÒ phôc mÖnh. ", GetName())
	
	local nBossVT = GetTask(TSK_DANH_BOSS_VT)
	if(nBossVT < nNpcKind) then
		Talk(1,"","<sex> ph¶i tiªu diÖt ®­îc "..nNpcKind.." boss trªn ®­êng råi quay l¹i ®©y.")
		return
	end
	
	if (nSTP == 0) then
		Talk(1,"",NONHANNV)
		return
	 end
	
	if (nSTP == 2) then
		Talk(1,"",msgLog)
		return
	 end	
	
	if (nNpcIdx <= 0) then
		Talk(1,"",TIEUXABIHUY)
		return 
	end

	if (GetNpcValue(nNpcIdx) ~= nParam) then
		Talk(1,"",NOT_ROAD)
		return 
	end			
	
	local nNearNpcIdx = FindNearNpc(1,KIND_TIEUXA[nNpcKind][2],750)
	if (nNearNpcIdx ~= nNpcIdx) then
		Talk(1,"",TIEUXABIHUY)
		return 
	end	
	
	SetTask(T_NHANNVVTIEU,2)
	SetTask(T_NPCID,0)
	DelNpc(nNpcIdx)
	AddSkillState(509,1,0,108*2,-1)
	Talk(1,"", msgLog)
	Msg2SubWorld(msgLog)
	SetTask(TSK_DANH_BOSS_VT, 0)
end

function no()
end