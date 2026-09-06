-- Author: Fong KiÒu
-- Date: 07/2021
-- Function: <Death quai tong kim> Nguyªn So¸i Tèng Kim

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")

GOOD_SKILL_BOSS 			= "<color=green>Ng­êi ch¬i:<color=metal>%s %s: <color=purple>%s<color> ®· tiªu diÖt ®­îc Nguyªn So¸i."
TANG_QUANHAM_PLA 	= "Qu©n hµm cña b¹n ®· t¨ng lªn <color=metal>%s<color> hç trî <color=metal>%s <color>xin chóc mõng!"

function OnDeath(nNpcIndex,nDamageIndex)
	local nType = GetNpcValue(nNpcIndex) -- 1: tong nguyen soai, 2: kim nguyen soai
	if(nType == 1) then
		SetMission(M_ADDNPCA, 2) --so¸i phe tèng mÊt set value 2
	end
	if(nType == 2) then
		SetMission(M_ADDNPCB, 2) --so¸i phe kim mÊt set value 2
	end
	
	local nPlayerIndex = NpcIdx2PIdx(nDamageIndex)
	if (nPlayerIndex <= 0) then	-- neu la quai vat thi ngung
		return
	end
	DelNpc(nNpcIndex)
	DelMSNpc(MS_TONGKIM,nNpcIndex)
	PlayerIndex = nPlayerIndex
	local nPlayerDataIdx = PIdx2MSDIdx(MS_TONGKIM,PlayerIndex)
	local nPQHam = GetPMParam(MS_TONGKIM,nPlayerDataIdx,11)
	local nPhe = GetMSIdxGroup(MS_TONGKIM, nPlayerDataIdx)
	SetPMParam(MS_TONGKIM,nPlayerDataIdx,3,GetPMParam(MS_TONGKIM,nPlayerDataIdx,3)+1) --set param 3 - giet npc
	tanglevelquanham(nPhe,nPlayerDataIdx)
	Msg2MSAll(MS_TONGKIM,format(GOOD_SKILL_BOSS,TAB_PHE_TONGKIM[nPhe][2],QUANHAMTK[nPQHam][1],GetName()))
	
	-- OnTimer()
	-- OnMissionTimer()
end

function tanglevelquanham(nPhe,nPlayerDataIdx)
	--local nPoint = GetPMParam(MS_TONGKIM,nPlayerDataIdx,6)*2 --cò lÊy ®iÓm tÝch luü player hiÖn t¹i lµm ®iÓm céng khi giÕt so¸i
	local nPoint = TICHLUY_GIETSOAI
	local nLastRank = GetPMParam(MS_TONGKIM,nPlayerDataIdx,11)
	SetPMParam(MS_TONGKIM,nPlayerDataIdx,6, GetPMParam(MS_TONGKIM,nPlayerDataIdx,6) + nPoint)	-- cong diem tich luy
	SetPMParam(MS_TONGKIM,nPlayerDataIdx,14,GetPMParam(MS_TONGKIM,nPlayerDataIdx,14) + nPoint)--tÝch luü giet npc
	SetMission(TAB_PHE_TONGKIM[nPhe][4],GetMissionV(TAB_PHE_TONGKIM[nPhe][4]) + nPoint) -- cong diem tich luy
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

end

function OnRevive(nNpcIndex)
	--SetNpcBoss2(nNpcIndex,2)
end

function OnTimer(nNpcIndex)
--	DelNpc(nIndex)
end

