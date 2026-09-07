-- [LOCAL54 06/09 toi] 2 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local go_thu, go_cong
Include("\\script\\chienlenh\\cl_def.lua")	-- [CL 04/09] so hieu nhiem vu Chien Lenh
--Author: Fong Ki“u
--Date: 28/11/2020
--Function: V÷ binh qu©n doanh check trap tı hÀu doanh vµo Æi”m tÀp k’t trong chi’n tr≠Íng

Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")

GO_CONGTHANH_SUCCESS 			= "<sex> Æ∑ gia nhÀp %s, phong qu©n hµm %s."

function main()
	
	dofile("script/tinhnang/congthanhchien/vebinhquandoanh.lua")
	
	SubWorldOld = SubWorld
	WorldID = SubWorldIdx2ID(SubWorld)
	idx = SubWorldID2Idx(ID_MAP_CTC)
	if (idx < 0) then
		return
	end
	SubWorld = idx
	if(IsMission(MS_CTHANHCHIEN) == 0) then
		Talk(1,"","Chi’n tr≠Íng c´ng thµnh ch≠a khai mÎ.")
		return 
	end	
	--if (GetMSRestTime(MS_CTHANHCHIEN,1) > 0) then 
	--	Say("Phe ta hi÷n Æang tÀp hÓp chu»n bﬁ vµo chi’n tr≠Íng! Xin m‰i ng≠Íi h∑y b◊nh t‹nh, chu»n bﬁ tinh th«n!", 0)
	--	SubWorld = SubWorldOld
	--	return
	--end
	local nTongName = GetTongInfo(1)	
	if (WorldID == ID_MAP_CTC_HDPT) then --tham gia Æi”m tÀp k’t tı hÀu doanh phe thÒ	
		if (nTongName ~= NAME_BANGTHU) then
			Talk(1,"",format("<sex> chÿ c„ thµnh vi™n cÒa bang [%s] mÌi Æ≠Óc vµo phe ThÒ.", NAME_BANGTHU))
			return 
		end
		if (GetMSPlayerCount(MISSIONID,1) >= MAX_CAMP1COUNT) then
			Say("Hi÷n giÍ sË ng≠Íi tham chi’n bang hÈi thÒ thµnh Æ∑ Æ«y, tπm thÍi kh´ng th” gia nhÀp!",0) 
			return
		end
		if (GetMSPlayerCount(MISSIONID,3) >= MAX_CAMP3COUNT) then
			Say("Hi÷n giÍ sË ngh‹a s‹ giÛp ÆÏ thÒ thµnh Æ∑ Æ«y, tπm thÍi kh´ng th” gia nhÀp!",0)
			return
		end		
		CheckAndJoin(1)
		SubWorld = SubWorldOld
	else																		--tham gia Æi”m tÀp k’t ra tı hÀu doanh phe c´ng
		if (nTongName ~= NAME_BANGCONG) then
			Talk(1,"",format("<sex> chÿ c„ thµnh vi™n cÒa bang [%s] mÌi Æ≠Óc vµo phe C´ng.", NAME_BANGCONG))
			return 
		end
		if (GetMSPlayerCount(MISSIONID,2) >= MAX_CAMP2COUNT) then
			Say("Hi÷n giÍ sË ng≠Íi tham chi’n bang hÈi c´ng thµnh Æ∑ Æ«y, tπm thÍi kh´ng th” gia nhÀp!",0) 
			return
		end
		if (GetMSPlayerCount(MISSIONID,4) >= MAX_CAMP4COUNT) then
			Say("Hi÷n giÍ sË ngh‹a s‹ giÛp ÆÏ thÒ thµnh Æ∑ Æ«y, tπm thÍi kh´ng th” gia nhÀp!",0)
			return
		end		
		CheckAndJoin(2)
		SubWorld = SubWorldOld
	end
end

function CheckAndJoin(Camp)
	if(Camp == 1) then
		local nPlayerDataIdx = PIdx2MSDIdx(MS_CTHANHCHIEN,PlayerIndex)
		if(nPlayerDataIdx > 0) then	--neu da gia nhap roi
			SetPMParam(MS_CTHANHCHIEN,nPlayerDataIdx,0,1)
			go_thu(nPlayerDataIdx)
			return 
		end
		nPlayerDataIdx = AddMSPlayer(MS_CTHANHCHIEN,1)
		if (nPlayerDataIdx and nPlayerDataIdx > 0) then
			go_thu(nPlayerDataIdx)
		end
	else
		local nPlayerDataIdx = PIdx2MSDIdx(MS_CTHANHCHIEN,PlayerIndex)
		if(nPlayerDataIdx > 0) then	--neu da gia nhap roi	
			SetPMParam(MS_CTHANHCHIEN,nPlayerDataIdx,0,1)
			go_cong(nPlayerDataIdx)
			return 
		end
		nPlayerDataIdx = AddMSPlayer(MS_CTHANHCHIEN,2)
		if (nPlayerDataIdx and nPlayerDataIdx > 0) then	
			go_cong(nPlayerDataIdx)
		end
	end
end

function go_thu(nPlayerDataIdx)
	CL_Xong(CL_NV.CTC_VAO)	-- [CL 04/09] Cong Thanh: vao tran (phe thu)
	local nSubWorldId = SubWorldID2Idx(ID_MAP_CTC)
	if nSubWorldId < 0 then	--chua mo map, ngung ham`
		return 
	end
	SubWorld = nSubWorldId
	StopTimer()	--ngung hen gio dang chay bat ky
	SetCreateTeam(0)
	LeaveTeam()	--roi nhom, giai tan nhom
	local nBRank = GetTongInfo(5)
	SetPMParam(MS_CTHANHCHIEN,nPlayerDataIdx,11,nBRank+2)
	SetRankBattle(nBRank+12+2, -1, 0) --ntime = -1 chet khong mat overlook = 0 thoat ra vao lai mat
	SetRevPos(TAB_PHE_CONGTHANH[1][6][1],TAB_PHE_CONGTHANH[1][6][2]) -- set phuc sinh tπi hÀu doanh phe thÒ
	SetTempRevPos(TAB_PHE_CONGTHANH[1][3][1],TAB_PHE_CONGTHANH[1][3][2]*32,TAB_PHE_CONGTHANH[1][3][3]*32) --set ve thanh duong suc ngay c´ng thµnh chi’n phe thÒ
	SetLogoutRV(1)--dung diem phuc sinh dang nhap
	CongThanhPheThu()
	SetCurCamp(1) --set chinh phai
	SetPunish(1)	--bat tinh nang chet khong mat' gi`
	local strQuanHam = TAB_RANKTHU[GetPMParam(MS_CTHANHCHIEN,nPlayerDataIdx,11)][2]
	Talk(1,"",format(GO_CONGTHANH_SUCCESS,TAB_PHE_CONGTHANH[1][2],strQuanHam))
	local RestTK = GetMSRestTime(MS_CTHANHCHIEN,1)
	if (RestTK > 0) then
		SetPKMode(0,1)--ep kieu luyen cong
		Msg2Player("Sau ".. floor((RestTK/18)/60).." phÛt trÀn chi’n sœ bæt Æ«u.")
	else
		SetPKMode(1,1)--ep kieu chien dau
	end	
	Msg2MSAll(MS_CTHANHCHIEN, format("%s %s Æ∑ tham gia phe ThÒ.",strQuanHam, GetName())) --thong bao gia nhap
	--UpdatePlayerCountMSCTC()
end

function go_cong(nPlayerDataIdx)
	CL_Xong(CL_NV.CTC_VAO)	-- [CL 04/09] Cong Thanh: vao tran (phe cong)
	local nSubWorldId = SubWorldID2Idx(ID_MAP_CTC)
	if nSubWorldId < 0 then	--chua mo map, ngung ham`
		return 
	end
	SubWorld = nSubWorldId 
	StopTimer()	--ngung hen gio dang chay bat ky
	SetCreateTeam(0)
	LeaveTeam()	--roi nhom, giai tan nhom
	local nBRank = GetTongInfo(5)
	SetPMParam(MS_CTHANHCHIEN,nPlayerDataIdx,11,nBRank+2)
	SetRankBattle(nBRank+17+2, -1, 0) --ntime = -1 chet khong mat overlook = 0 thoat ra vao lai mat
	SetRevPos(TAB_PHE_CONGTHANH[2][6][1],TAB_PHE_CONGTHANH[2][6][2]) --set phuc sinh tπi hÀu doanh phe c´ng
	SetTempRevPos(TAB_PHE_CONGTHANH[2][3][1],TAB_PHE_CONGTHANH[2][3][2]*32,TAB_PHE_CONGTHANH[2][3][3]*32) --set ve thanh duong suc ngay c´ng thµnh chi’n phe c´ng
	SetLogoutRV(1)--dung diem phuc sinh dang nhap
	CongThanhPheCong()
	SetCurCamp(2) --set ta phai
	SetPunish(1)	--bat tinh nang chet khong mat' gi`
	local strQuanHam = TAB_RANKCONG[GetPMParam(MS_CTHANHCHIEN,nPlayerDataIdx,11)][2]
	Talk(1,"",format(GO_CONGTHANH_SUCCESS,TAB_PHE_CONGTHANH[2][2],strQuanHam))
	local RestTK = GetMSRestTime(MS_CTHANHCHIEN,1)
	if (RestTK > 0) then
		SetPKMode(0,1)--ep kieu luyen cong
		Msg2Player("Sau ".. floor((RestTK/18)/60).." phÛt trÀn chi’n sœ bæt Æ«u.")	
	else
		SetPKMode(1,1)--ep kieu chien dau
	end
	Msg2MSAll(MS_CTHANHCHIEN, format("%s %s Æ∑ tham gia phe C´ng.",strQuanHam, GetName())) --thong bao gia nhap
	--UpdatePlayerCountMSCTC()
end

