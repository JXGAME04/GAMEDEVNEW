--Author: Fong KiÒu
--Date: 28/11/2020
--Function: Trap phe Tèng ra khái hËu doanh

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")

RANDOM_POS = {
	{1308,3461},
	{1311,3442},
	{1324,3428},
	{1337,3420},
	{1352,3419},
	{1350,3439},
	{1342,3463},
	{1321,3468}
}

function main(nPlayerIndex)
	PlayerIndex = nPlayerIndex
	local nRemain = GetRestTime() - (TIME_IN_TRAI-TIME_DELAY_RA_TRAI)*18 --60 giay chi lay 10 giay thoi
	if(nRemain > 0) then
		local nfRemain = floor(nRemain/18)
		if(nfRemain > 0) then
			Talk(1,"","Sau "..nfRemain.." gi©y n÷a <sex> cã thÓ vµo chiÕn tr­êng.")
		end
		return 
	end
	local nPlayerDataIdx = PIdx2MSDIdx(MS_TONGKIM,PlayerIndex)
	-- SetPMParam(MS_TONGKIM,nPlayerDataIdx,0,1)--set online ms tong kim param 0 value 1
	SetPMParam(MS_TONGKIM,nPlayerDataIdx,1,1)--set online ms tong kim param 0 value 1
	TK_GuiDiemChoToi(TKDIEM_KIND_CAPNHAT)	-- [TKDIEM 04/09] vua ra khoi hau doanh: hien bang diem Tong VS Kim
	TK_GuiThongTinChoToi()	-- [TKINFO 05/09] cua so thong tin tran (giai doan, giay con lai, top 5)
	tong_ratran()
	StopTimer()
end

function tong_ratran()
	local nRanNum = random(1,getn(RANDOM_POS))
	SetPos(RANDOM_POS[nRanNum][1],RANDOM_POS[nRanNum][2])
	local nViTri = GetMissionV(M_VITRI_TRENDUOI)
	if(nViTri == 1) then
		SetDeathScript(ONDEATHQUANTONG)
	else
		SetDeathScript(ONDEATHQUANKIM)
	end
	SetFightState(1)	
	SetProtectTime(18*3)
	AddSkillState(963, 1, 0, 18*3)
end