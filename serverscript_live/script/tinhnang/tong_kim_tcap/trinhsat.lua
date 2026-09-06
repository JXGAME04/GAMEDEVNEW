-- Author: Fong Ki“u
-- Date: 28/11/2020
-- Chuc nang: Trinh s∏t

Include("\\script\\lib\\lib_ham.lua")
-- [CFGTKT 30/08] hai tep duoi day la LA (khong Include gi).
Include("\\script\\cauhinh\\ch_lib.lua")
Include("\\script\\cauhinh\\ch_thuong.lua")
-- [CFGTKT 30/08] Bo doc cau hinh cho tep nay. Tra ve MAC DINH (= so cu)
-- khi bo cau hinh chua nap, nen kem nhat cung khong the doi hanh vi.
function TKT_CFG(szKhoa, macdinh)
	if (G_CFG ~= nil) then
		return G_CFG(szKhoa, macdinh)
	end
	return macdinh
end

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")

MONEY_TS = TKT_CFG("TKT_PHI_TRINHSAT", 500)

function main(NpcIndex)
	
	--dofile("script/tinhnang/tong_kim_tcap/trinhsat.lua")
	
	local phe = GetNpcValue(NpcIndex)
	if(phe == 1) then
		SayEx({
				"<color=red><npc><color>: <sex> xin ch‰n vﬁ tr› ta sœ d…n Æ≠Íng:",
				--format("TËng Pos 1/#tong_ratran(%d)",1),
				--format("TËng Pos 2/#tong_ratran(%d)",2),
				--format("TËng Pos 3/#tong_ratran(%d)",3),
				format("TËng Pos Random/#tong_ratran(%d)",0),
				"K’t thÛc ÆËi thoπi/no"})
	end
	if(phe == 2) then
		SayEx({
				"<color=red><npc><color>: <sex> xin ch‰n vﬁ tr› ta sœ d…n Æ≠Íng:",
				--format("Kim Pos 1/#kim_ratran(%d)",1),
				--format("Kim Pos 2/#kim_ratran(%d)",2),
				--format("Kim Pos 3/#kim_ratran(%d)",3),
				format("Kim Pos Random/#kim_ratran(%d)",0),
				"K’t thÛc ÆËi thoπi/no"})		
	end
	
end

function tong_ratran(nRanNum)
	local RANDOM_POS_TONG= {
		{1339,3355},
		{1410,3358},
		{1387,3463},
	}

	local RANDOM_POS_KIM= {
		{1470,3229},
		{1489,3190},
		{1589,3286},
	}	
	local nViTri = GetMissionV(M_VITRI_TRENDUOI)
	local RANDOM_POS = nil
	if(nViTri == 1) then
		RANDOM_POS = RANDOM_POS_TONG
	else
		RANDOM_POS = RANDOM_POS_KIM
	end
	if(RANDOM_POS == nil) then
		print("Khong tim thay vi tri trinh sat tong_ratran")
		return
	end		
	SubWorld = SubWorldID2Idx(MAP_TK_TC)
	if(GetCash() < MONEY_TS) then
		Talk(1,"","<color=red><npc><color>: <sex> kh´ng c„ ÆÒ "..MONEY_TS.." quan ti“n th◊ Æıng mong ta giÛp!")
		return
	end
	if (GetMSRestTime(MS_TONGKIM,1) > 0) then
		SetFightState(0)
		Talk(1,"","ßπi chi’n TËng Kim v…n ch≠a bæt Æ«u")
		return
	end		
	if(nRanNum or nRanNum == 0) then
		nRanNum = random(1,getn(RANDOM_POS))
	end
	SetPos(RANDOM_POS[nRanNum][1],RANDOM_POS[nRanNum][2])
	Pay(MONEY_TS)
	local nPlayerDataIdx = PIdx2MSDIdx(MS_TONGKIM,PlayerIndex)	
	-- SetPMParam(MS_TONGKIM,nPlayerDataIdx,0,1)--set online ms tong kim param 0 value 1
	SetPMParam(MS_TONGKIM,nPlayerDataIdx,1,1)--set online ms tong kim param 0 value 1
	SetDeathScript(ONDEATHQUANTONG)
	SetFightState(1)
	StopTimer()
	SetProtectTime(18*3)
	AddSkillState(963, 1, 0, 18*3)		
end

function kim_ratran(nRanNum)
	local RANDOM_POS_TONG= {
		{1339,3355},
		{1410,3358},
		{1387,3463},
	}

	local RANDOM_POS_KIM= {
		{1470,3229},
		{1489,3190},
		{1589,3286},
	}	
	local nViTri = GetMissionV(M_VITRI_TRENDUOI)
	local RANDOM_POS = nil
	if(nViTri == 1) then
		RANDOM_POS = RANDOM_POS_KIM
	else
		RANDOM_POS = RANDOM_POS_TONG
	end
	if(RANDOM_POS == nil) then
		print("Khong tim thay vi tri trinh sat kim_ratran")
		return
	end		
	SubWorld = SubWorldID2Idx(MAP_TK_TC)
	if(GetCash() < MONEY_TS) then
		Talk(1,"","<color=red><npc><color>:<sex> kh´ng c„ ÆÒ "..MONEY_TS.." quan ti“n th◊ Æıng mong ta giÛp!")
		return
	end	
	if (GetMSRestTime(MS_TONGKIM,1) > 0) then
		SetFightState(0)
		Talk(1,"","ßπi chi’n TËng Kim v…n ch≠a bæt Æ«u")
		return
	end		
	if(nRanNum or nRanNum == 0) then
		nRanNum = random(1, getn(RANDOM_POS))
	end
	SetPos(RANDOM_POS[nRanNum][1], RANDOM_POS[nRanNum][2])
	Pay(MONEY_TS)
	local nPlayerDataIdx = PIdx2MSDIdx(MS_TONGKIM, PlayerIndex)
	-- SetPMParam(MS_TONGKIM,nPlayerDataIdx,0,1)--set online ms tong kim param 0 value 1
	SetPMParam(MS_TONGKIM,nPlayerDataIdx,1,1)--set online ms tong kim param 0 value 1
	SetDeathScript(ONDEATHQUANKIM)
	SetFightState(1)	
	StopTimer()
	SetProtectTime(18*3)
	AddSkillState(963, 1, 0, 18*3)		
end


function no()
	
end
