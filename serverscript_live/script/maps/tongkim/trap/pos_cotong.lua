-- Author: Fong KiÒu
-- Date: 28/11/2020

Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")

TANG_QUANHAM_PLA = "Qu©n hµm cña b¹n ®· t¨ng lªn <color=metal>%s<color> hç trî <color=metal>%s <color>xin chóc mõng!"

function main(nPlayerIndex)
	PlayerIndex = nPlayerIndex
	local nPlayerDataIdx = PIdx2MSDIdx(MS_TONGKIM,PlayerIndex)
	local nFlag = GetPMParam(MS_TONGKIM,nPlayerDataIdx,8) -- dang giu co
	if (nFlag == 1) then
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,6,GetPMParam(1,nPlayerDataIdx,6)+FLAG_POINT) -- cong diem tich luy
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,17,GetPMParam(MS_TONGKIM,nPlayerDataIdx,17) + FLAG_POINT)--tÝch luü nhÆt cê
		SetMission(M_TICHLUYA, GetMissionV(M_TICHLUYA)+FLAG_POINT) -- cong diem tich luy
		SetMission(M_SOCOPHEA, GetMissionV(M_SOCOPHEA)+1)
		local TAB_MSG = 
		{
			"<color=pink>B¹n ®· c¾m cê thµnh c«ng nhËn ®­îc <color=green>"..FLAG_POINT.."<color> ®iÓm hiÖn t¹i b¹n cã <color=green>"..GetPMParam(1,nPlayerDataIdx,6).."<color>. ",
			"Ng­êi ch¬i <color=green>"..GetName().."<color> ®· c¾m cê thµnh c«ng, phe Tèng ®· c¾m ®­îc <color=yellow>"..GetMissionV(M_SOCOPHEA).."<color> c©y cê.",
		}
		Msg2Player(TAB_MSG[1])
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,8,0)	-- mat giu co
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,9,GetPMParam(1,nPlayerDataIdx,9)+1)-- so lan cam co
		ReSetMask()
		RestoreRunSpeed()
		Msg2MSGroup(MS_TONGKIM,TAB_MSG[2],1)
		
		checkqhamdoatco(1,nPlayerDataIdx)
		SetDeathScript(ONDEATHQUANTONG)

		local nRand = random(1,getn(COTONG_TOADO))
		local nNpcIdx = AddNpcEx1({TAB_PHE_TONGKIM[1][7]},1,{0},MAP_TK_TC,COTONG_TOADO[nRand][1]*32,COTONG_TOADO[nRand][2]*32,nil,DOSCRIPTCO,nil,6)
		AddMSNpc(MS_TONGKIM,nNpcIdx)
		SetNpcValue(nNpcIdx, 1)
	end
end

function checkqhamdoatco(nPhe,nPlayerDataIdx)
	local nLRank = GetPMParam(MS_TONGKIM,nPlayerDataIdx,11)			 -- 1
	local nTichLuy = GetPMParam(MS_TONGKIM,nPlayerDataIdx,6)
	for i=1,getn(TAB_QUANHAM) do
		if (nTichLuy >= TAB_QUANHAM[i][1]) and (nTichLuy <= TAB_QUANHAM[i][2]) then
			SetPMParam(MS_TONGKIM,nPlayerDataIdx,11,i)
		end
	end
	local nCRank = GetPMParam(MS_TONGKIM,nPlayerDataIdx,11)
	if (nLRank ~= nCRank) then
		nCRank = GetPMParam(MS_TONGKIM,nPlayerDataIdx,11)	
		SetRankBattle(nCRank+TAB_PHE_TONGKIM[nPhe][3], -1, 0) --ntime = -1 chet khong mat overlook = 0 thoat ra vao lai mat
		Msg2Player(format(TANG_QUANHAM_PLA,QUANHAMTK[nCRank][1],QUANHAMTK[nCRank][3]))
	end
end