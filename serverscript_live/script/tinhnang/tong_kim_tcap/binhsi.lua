--Author: Fong KiÒu
-- Date: 01/07/2021
-- Function:  <Death quai tong kim>

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")

TANG_QUANHAM_PLA = "Qu©n hµm ®· t¨ng lªn <color=metal>%s<color> hç trî <color=metal>%s <color>xin chóc mõng!"
POINTADD = "TÝch luü <color=green>+%d<color> hiÖn cã <color=cyan>%d<color> ®iÓm."

function OnDeath(nNpcIndex,nDamageIndex)
	local nType = GetNpcValue(nNpcIndex)	-- 1: quai, 2: hieu uy, 3: thong linh, 4: pho tuong, 5:dai tuong, 6:binh si hieu phu
	local nPlayerIndex = NpcIdx2PIdx(nDamageIndex)
	if(nType == 6) then
		DelNpc(nNpcIndex)
		return
	end	
	if (nPlayerIndex == 0) then	-- neu la quai vat thi ngung
		return 
	end		
	PlayerIndex = nPlayerIndex
	local nPlayerDataIdx = PIdx2MSDIdx(MS_TONGKIM,PlayerIndex)
	local nPhe = GetMSIdxGroup(MS_TONGKIM, nPlayerDataIdx)
	SetPMParam(MS_TONGKIM,nPlayerDataIdx,3,GetPMParam(MS_TONGKIM,nPlayerDataIdx,3)+1) --set param 3 - giet npc		
	tanglevelquanham(nPhe,nType,nPlayerDataIdx)
	
end

function tanglevelquanham(nPhe,nType,nPlayerDataIdx)
	local nPoint = TICHLUYTK[nType]
	local nLastRank = GetPMParam(MS_TONGKIM,nPlayerDataIdx,11)
	SetPMParam(MS_TONGKIM,nPlayerDataIdx,6,GetPMParam(MS_TONGKIM,nPlayerDataIdx,6) + nPoint)		-- cong diem tich luy
	SetPMParam(MS_TONGKIM,nPlayerDataIdx,14,GetPMParam(MS_TONGKIM,nPlayerDataIdx,14) + nPoint)--tÝch luü giet npc
	SetMission(TAB_PHE_TONGKIM[nPhe][4],GetMissionV(TAB_PHE_TONGKIM[nPhe][4])+nPoint) -- cong diem tich luy 
	for i=1,getn(TAB_QUANHAM) do
		if (GetPMParam(MS_TONGKIM,nPlayerDataIdx,6) >= TAB_QUANHAM[i][1]) and (GetPMParam(MS_TONGKIM,nPlayerDataIdx,6) <= TAB_QUANHAM[i][2]) then
			SetPMParam(MS_TONGKIM,nPlayerDataIdx,11,i)
		end
	end
	if (nLastRank ~= GetPMParam(MS_TONGKIM,nPlayerDataIdx,11)) then
		local nCurRank = GetPMParam(MS_TONGKIM,nPlayerDataIdx,11)
		SetRankBattle(nCurRank+TAB_PHE_TONGKIM[nPhe][3], -1, 0) --ntime = -1 chet khong mat overlook = 0 thoat ra vao lai mat
		Msg2Player(format(TANG_QUANHAM_PLA,QUANHAMTK[nCurRank][1],QUANHAMTK[nCurRank][3]))	
	end
	Msg2Player(format(POINTADD,nPoint,GetPMParam(MS_TONGKIM,nPlayerDataIdx,6)))

end

function OnRevive(nNpcIndex)
--		--SetNpcBoss2(nNpcIndex,0)
end


