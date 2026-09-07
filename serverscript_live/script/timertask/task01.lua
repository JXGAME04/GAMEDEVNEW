-- ================================================================================================
-- [HE THONG] script/timertask/task01.lua
-- Muc dich  : VIEC THEO LICH goi tu timerserver.lua (RunTime): taskNN.lua = nhom viec so NN (task04-07 rong tu 30/08).
-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng) + C++ goi truc tiep theo ten tep
-- Include   : lib_task.lua, lib_tktc.lua, lib_ctc.lua, lib_vuotai.lua, save_log.lua, tongkim.lua
-- Ham (dong): OnTimer (13), OnMissionTimer (18), ontime_tongkim (26)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
--Author: Fong KiÒu
--Date: 2021
--Function: 

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")
-- [GOHECU 30/08] he cu da go
-- Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")
-- [GOHECU 30/08] he cu da go
-- Include("\\script\\tinhnang\\vuot_ai\\lib_vuotai.lua")
Include("\\script\\log_game\\save_log.lua")
Include("\\script\\header\\tongkim.lua");

function OnTimer()
	-- [GOHECU 30/08] than cu chi phuc vu Vuot Ai ban Viet (MAP_VUOTAI) - da go.
	StopTimer()
end

function OnMissionTimer() --nIndex la so id nhiem vu, TongKim la mission 1
	-- [GOHECU 30/08] da go 2 nhanh chet: ID_MAP_CTC (Cong Thanh cu) va MAP_VUOTAI
	-- (Vuot Ai cu). Chi con Tong Kim - hoat dong dang chay.
	if (SubWorld == SubWorldID2Idx(MAP_TK_TC)) then
		ontime_tongkim()
	end
end;

function ontime_tongkim()
	StopMissionTimer(MS_TONGKIM,1)--tat hen gio so 1
	local nTongc = GetMSPlayerCount(MS_TONGKIM,1)
	local nKimc  = GetMSPlayerCount(MS_TONGKIM,2)
	local nCount = GetMSPlayerCount(MS_TONGKIM)
	if (nTongc == 0 and nKimc == 0) then
		for dataindex = 1, nCount do
			if(GetPMParam(MS_TONGKIM, dataindex, 0) == 1) then --dang online
				PlayerIndex = MSDIdx2PIdx(MS_TONGKIM, dataindex)
				local nPhe = GetMSIdxGroup(MS_TONGKIM, dataindex)
				SetPKMode(0,0)--phuc hoi pk tu do
				SetFightState(0)--phi chien dau
				SetPunish(0)		
				RemoveRankBattle()
				SetNpcTimeIdle(0)
				IgnoreState()	--xoa het trang thai skill tren nguoi
				--SetTempRevPos(53, 51904, 102048)--ket thuc thiet lap diem hoi sinh o Ba Lang Huyen
				SetLogoutRV(0)
				SetDeathScript("")				
				SetCurCamp(GetCamp())--phuc hoi phe ban dau
				NewWorld(TAB_PHE_TONGKIM[nPhe][6][1],TAB_PHE_TONGKIM[nPhe][6][2],TAB_PHE_TONGKIM[nPhe][6][3])
			end
		end
		DELDULIEU() -- xoa du lieu limited 1 account login TONGKIM
		DelAllNpc(SubWorld)
		CloseMission(MS_TONGKIM)
		AddGlobalCountNews(13910, 3) --1 ben ko co nguoi
		return 
	end
	AddGlobalCountNews(10651, 3) --chien tran da bat dau
	for dataindex=1, nCount do
		if(GetPMParam(MS_TONGKIM, dataindex, 0) == 1) then --dang online
			PlayerIndex = MSDIdx2PIdx(MS_TONGKIM, dataindex)
			SetPKMode(1, 1)--ep kieu chien dau
			-- SetPMParam(MS_TONGKIM,dataindex,1,1)
			Talk(1, "", "Tèng Kim §¹i chiÕn chÝnh thøc b¾t ®Çu!")
		end
	end
	local nPThuc = GetMissionV(M_HINHTHUC)
	if (nPThuc == PT_CuuSat) then
		-- cuu sat kh«ng add npc hay object g× hÕt
	elseif (nPThuc == PT_BaoVeNguyenSoai) then
		addnpcquaitktrungcap()	 -- bao ve nguyen soai
	elseif (nPThuc == PT_DoatCo) then	
		addnpcquaitktrungcap()		-- doat co
		addnpccotongkim()
	end	
end

-- [GOHECU 30/08] da go ham ontime_congthanh (43 dong) - he cu

-- [GOHECU 30/08] da go ham ontime_vuotai (15 dong) - he cu