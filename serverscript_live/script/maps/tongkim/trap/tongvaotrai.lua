-- Author: Fong KiÒu
-- Date: 28/11/2016

Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")

function main(nPlayerIndex)
	PlayerIndex = nPlayerIndex
	local nPlayerDataIdx = PIdx2MSDIdx(MS_TONGKIM, PlayerIndex)
	local nPhe = GetMSIdxGroup(MS_TONGKIM, nPlayerDataIdx)
	local nViTri = GetMissionV(M_VITRI_TRENDUOI)
	if(nViTri == 1) then
		if (nPhe ~= 1) then
			Msg2Player("<color=pink>Kim binh<color> kh«ng thÓ ®ét nhËp vµo doanh tr¹i Tèng.")
			return 
		end
	else
		if (nPhe ~= 2) then
			Msg2Player("<color=pink>Tèng binh<color> kh«ng thÓ ®ét nhËp vµo doanh tr¹i Kim.")
			return 
		end	
	end

	local nFlag = GetPMParam(MS_TONGKIM, nPlayerDataIdx, 8)	-- trang thai mang co tren nguoi
	if (nFlag == 1) then
		ReSetMask()
		RestoreRunSpeed()
		SetPMParam(MS_TONGKIM,nPlayerDataIdx, 8, 0)			   -- mat giu co
		local nRand = random(1, getn(COTONG_TOADO))
		local nNpxIdx = AddNpcEx1({TAB_PHE_TONGKIM[1][7]},1,{0},MAP_TK_TC,COTONG_TOADO[nRand][1]*32,COTONG_TOADO[nRand][2]*32,nil,DOSCRIPTCO,nil,6)
		AddMSNpc(MS_TONGKIM, nNpcIdx)
		SetNpcValue(nNpcIdx, 1)
	end
	SetPMParam(MS_TONGKIM, nPlayerDataIdx, 1, 0)		-- ngung chien dau	
	SetFightState(0)
	SetPos(1251, 3537)
	SetTimer(TIME_IN_TRAI*18, 2)
end