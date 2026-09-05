--Author: Fong KiÒu
--Date: 28/11/2020
--Function: Trap phe kim ra khái hËu doanh

Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")
Include("\\script\\lib\\lib_task.lua")

RANDOM_POS_KIM={
	{1585,3181},
	{1570,3173},
	{1551,3175},
	{1544,3204},
	{1550,3223},
	{1569,3227},
	{1586,3216},
	{1592,3198}
}

function main(nPlayerIndex)
	PlayerIndex = nPlayerIndex
	local nRemain = GetRestTime() - (TIME_IN_TRAI-TIME_DELAY_RA_TRAI)*18 --60 giay chi lay 10 giay thoi
	if(nRemain > 0) then
		local nfRemain = floor(nRemain/18)
		if(nfRemain > 0) then
			Talk(1, "", "Sau "..nfRemain.." gi©y n÷a <sex> cã thÓ vµo chiÕn tr­êng.")
		end
		return 
	end
	local nPlayerDataIdx = PIdx2MSDIdx(MS_TONGKIM, PlayerIndex)
	-- SetPMParam(MS_TONGKIM,nPlayerDataIdx,0,1)--set online ms tong kim param 0 value 1
	SetPMParam(MS_TONGKIM,nPlayerDataIdx,1,1)--set online ms tong kim param 0 value 1
	TK_GuiDiemChoToi(TKDIEM_KIND_CAPNHAT)	-- [TKDIEM 04/09] vua ra khoi hau doanh: hien bang diem Tong VS Kim
	kim_ratran()
	StopTimer()
end

function kim_ratran()
	local nRanNum = random(1, getn(RANDOM_POS_KIM))
	SetPos(RANDOM_POS_KIM[nRanNum][1], RANDOM_POS_KIM[nRanNum][2])
	local nViTri = GetMissionV(M_VITRI_TRENDUOI)
	if(nViTri == 1) then
		SetDeathScript(ONDEATHQUANKIM)
	else
		SetDeathScript(ONDEATHQUANTONG)
	end
	
	SetFightState(1)	
	SetProtectTime(18*3)
	AddSkillState(963, 1, 0, 18*3)	
	
end