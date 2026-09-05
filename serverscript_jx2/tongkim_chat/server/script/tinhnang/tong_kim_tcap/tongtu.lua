--Author: Fong KiÒu
--Date: 2021
--Function: Player phe tèng tö trËn
Include("\\script\\header\\tongkim.lua");
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")

TANG_QUANHAM_PLA = "Qu©n hµm cña b¹n ®· t¨ng lªn <color=metal>%s<color> hç trî <color=metal>%s <color>xin chóc mõng!"
KIMKILLTONG = "B¹n ®¸nh träng th­¬ng Tèng %s%s<color> %s nhËn ®­îc <color=green>%d<color> tÝch lòy."
PLAYER_DEATH_FLAG = "%s <color=purple>%s<color> ®· ®¸nh r¬i cê t¹i <color=yellow>%d/%d<color> h·y lªn ®o¹t l¹i nµo."

function OnPlayerDeath(nPlayerIndex,nLastDamageIndex) -- player, npc
	--dofile("script/tinhnang/tong_kim_tcap/tongtu.lua")
	PlayerIndex = nPlayerIndex
	local nPlayerDataIdx = PIdx2MSDIdx(MS_TONGKIM,nPlayerIndex)
	local nPhe_playerdie = GetMSIdxGroup(MS_TONGKIM, nPlayerDataIdx)
	local nOnAttack = GetPMParam(MS_TONGKIM,nPlayerDataIdx,1)
	local nDesPlayerIdx = NpcIdx2PIdx(nLastDamageIndex)
	local nPThuc  = GetMissionV(M_HINHTHUC) -- 1: cuu sat, 2: bao ve nguyen soai, 3: ®o¹t cê	
	SetTimer(TIME_IN_TRAI*18,2)--set thêi gian ë trong doanh tr¹i råi bÞ ®¸ ra ngoµi
	SetFightState(0)
	SetPKMode(1, 1)--ep kieu chien dau
	--Msg2Player("GetMSRestTime(MS_TONGKIM,1)="..GetMSRestTime(MS_TONGKIM,1))
	--if (GetMSRestTime(MS_TONGKIM,1) > 0) then--nÕu ch­a b¾t ®Çu trËn th× return
	--	return
	--end
	local nDesPlayerData = PIdx2MSDIdx(MS_TONGKIM,nDesPlayerIdx)
	local nPhe_playerkiller = GetMSIdxGroup(MS_TONGKIM, nDesPlayerData)
	--Msg2Player(format("tongtu nPhe_playerdie=%d nPhe_playerkiller=%d",nPhe_playerdie,nPhe_playerkiller))
	if(nPhe_playerdie == nPhe_playerkiller) then --nÕu cïng phe giÕt nhau return
		return
	end	
	if(nDesPlayerIdx <= 0) then --quai'
		if(nPlayerDataIdx > 0 and nOnAttack == 1) then
			SetPMParam(MS_TONGKIM,nPlayerDataIdx,4,GetPMParam(MS_TONGKIM,nPlayerDataIdx,4)+1)--tu vong
			SetPMParam(MS_TONGKIM,nPlayerDataIdx,7,0)--lien tram hien tai
			SetPMParam(MS_TONGKIM,nPlayerDataIdx,1,0)--set l¹i tr¹ng th¸i kh«ng chiÕn ®Êu vÒ hËu doanh tèng kim
			local nFlag = GetPMParam(MS_TONGKIM,nPlayerDataIdx,8) -- dang giu co
			if (nFlag == 1) then
				SetPMParam(MS_TONGKIM,nPlayerDataIdx,8,0)			   -- mat giu co
				local nW,nX,nY = GetWorldPos(nPlayerIndex)
				local nNpcIndex = AddNpcEx1({TAB_PHE_TONGKIM[1][7]},1,{0},MAP_TK_TC,nX*32,nY*32,nil,DOSCRIPTCO,nil,6)
				AddMSNpc(MS_TONGKIM,nNpcIndex)
				SetNpcValue(nNpcIndex,1)
				Msg2MSGroup(MS_TONGKIM,format(PLAYER_DEATH_FLAG,TAB_PHE_TONGKIM[nFlag][2],GetName(),floor(nX/8),floor(nY/16)),nFlag)
			end
		end
		return
	end
	local nDesOnAttack = GetPMParam(MS_TONGKIM,nDesPlayerData,1)
	--Msg2Player(format("tongtu nDesPlayerData:%d nOnAttack:%d nDesOnAttack:%d nPlayerDataIdx:%d",nDesPlayerData,nOnAttack,nDesOnAttack,nPlayerDataIdx))
	if(nPlayerDataIdx > 0 and nDesPlayerData > 0 and nOnAttack == 1 and nDesOnAttack == 1) then	--co ton tai. va ca 2 dang online trong chien truong tr¹ng th¸i chiÕn ®Êu
		local szName = GetName()
		local szRank = GetPMParam(MS_TONGKIM,nPlayerDataIdx,11)
		local nDieRank = GetPMParam(MS_TONGKIM,nPlayerDataIdx,11)	--cap bac nguoi chet
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,4,GetPMParam(MS_TONGKIM,nPlayerDataIdx,4)+1)--tu vong
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,7,0)--lien tram hien tai
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,1,0)--set l¹i tr¹ng th¸i kh«ng chiÕn ®Êu vÒ hËu doanh tèng kim
		PlayerIndex = nDesPlayerIdx
		local szDesName = GetName()
		local szDesRank = GetPMParam(MS_TONGKIM,nDesPlayerData,11)
		SetPMParam(MS_TONGKIM,nDesPlayerData,2,GetPMParam(MS_TONGKIM,nDesPlayerData,2) + 1)--so luong giet player
		local CurNewLT = GetPMParam(MS_TONGKIM,nDesPlayerData,7) + 1
		SetPMParam(MS_TONGKIM,nDesPlayerData,7,CurNewLT)--lien tram hien tai
		local MaxLT = GetPMParam(MS_TONGKIM,nDesPlayerData,5)
		if(CurNewLT > MaxLT) then
			SetPMParam(MS_TONGKIM,nDesPlayerData,5,CurNewLT)--max LT
		end
		local nPoint = 0
		local nPointLT = 0
		local nPointPlayer = 0
		if (nPThuc == PT_CuuSat) then
			nPoint = 30 + (30 * nDieRank)*2 + (30 * CurNewLT)
			nPointLT = (30 * CurNewLT)
			nPointPlayer = 30 + (30 * nDieRank)*2
		else
			nPoint = 30 + (30 * nDieRank) + (30 * CurNewLT)
			nPointLT = (30 * CurNewLT)
			nPointPlayer = 30 + (30 * nDieRank)
		end
		SetPMParam(MS_TONGKIM,nDesPlayerData,6,GetPMParam(MS_TONGKIM,nDesPlayerData,6) + nPoint) -- cong diem tich luy
		SetPMParam(MS_TONGKIM,nDesPlayerData,13,GetPMParam(MS_TONGKIM,nDesPlayerData,13) + nPointPlayer)--tÝch luü giet player
		SetPMParam(MS_TONGKIM,nDesPlayerData,15,GetPMParam(MS_TONGKIM,nDesPlayerData,15) + nPointLT)--tÝch luü giet player
		SetMission(M_TICHLUYB,GetMissionV(M_TICHLUYB)+nPoint)		-- cong diem tich luy	
		TK_GuiDiemPhe(TKDIEM_KIND_CAPNHAT)	-- [TKDIEM 04/09] bang diem Tong VS Kim tren client (moi lan giet)
		-- SetPMParam(MS_TONGKIM,nPlayerDataIdx,0,1)--set online ms tong kim param 0 value 1
		Msg2MSAll(MS_TONGKIM,format("%s ®¸nh träng th­¬ng Tèng %s%s<color> %s nhËn ®­îc <color=green>%d<color> tÝch lòy.",szDesName, QUANHAMTK[szRank][5],QUANHAMTK[szRank][1],szName,nPoint),nDesPlayerData)
		

		local nTotalAccum = GetPMParam(MS_TONGKIM,nDesPlayerData,6)--tong so tich luy
		for i=1,getn(TAB_QUANHAM) do
			if (nTotalAccum >= TAB_QUANHAM[i][1]) and (nTotalAccum <= TAB_QUANHAM[i][2]) then
				SetPMParam(MS_TONGKIM,nDesPlayerData,11,i)
			end
		end
		local nCurDescRank = GetPMParam(MS_TONGKIM,nDesPlayerData,11)
		if (szDesRank ~= nCurDescRank) then
			nCurDescRank = GetPMParam(MS_TONGKIM,nDesPlayerData,11)
			SetRankBattle(nCurDescRank+6, -1, 0) --ntime = -1 chet khong mat overlook = 0 thoat ra vao lai mat
			Msg2Player(format(TANG_QUANHAM_PLA,QUANHAMTK[nCurDescRank][1],QUANHAMTK[nCurDescRank][3]),nDesPlayerData)
		end
		local nFlag = GetPMParam(MS_TONGKIM,nPlayerDataIdx,8) -- dang giu co
		if (nFlag == 1) then
			SetPMParam(MS_TONGKIM,nPlayerDataIdx,8,0)			   -- mat giu co
			local nW,nX,nY = GetWorldPos(nPlayerIndex)
			local nNpcIndex = AddNpcEx1({TAB_PHE_TONGKIM[1][7]},1,{0},MAP_TK_TC,nX*32,nY*32,nil,DOSCRIPTCO,nil,6)
			AddMSNpc(MS_TONGKIM,nNpcIndex)
			SetNpcValue(nNpcIndex,1)
			Msg2MSGroup(MS_TONGKIM,format(PLAYER_DEATH_FLAG,TAB_PHE_TONGKIM[nFlag][2],szName,floor(nX/8),floor(nY/16)),nFlag);
		end			
	end	
end

function OnPlayerTimerIdle(nPlayerIndex)
	local nPlayerDataIdx
	PlayerIndex = nPlayerIndex
	local nW,nX,nY = GetWorldPos()
	nPlayerDataIdx = PIdx2MSDIdx(MS_TONGKIM,PlayerIndex)
	SetPMParam(MS_TONGKIM,nPlayerDataIdx, 0, 0)	--param 0 value 0 set offline tong kim

	Msg2MSAll(MS_TONGKIM, format("%s ®· bÞ ®Èy khái chiÕn tr­êng Tèng Kim.",GetName())) --thong bao day ra ngoai
	local nFlag = GetPMParam(MS_TONGKIM,nPlayerDataIdx,8) -- dang giu co
	if (nFlag > 0) then
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,8,0)			   -- mat giu co
		local nNpcIndex = AddNpcEx1({TAB_PHE_TONGKIM[nFlag][7]},1,{0},MAP_TK_TC,nX*32,nY*32,nil,DOSCRIPTCO,nil,6)
		AddMSNpc(MS_TONGKIM,nNpcIndex)
		SetNpcValue(nNpcIndex, nFlag)
		Msg2MSGroup(MS_TONGKIM,format(PLAYER_LOGOUT_FLAG,TAB_PHE_TONGKIM[nFlag][2],GetName(),floor(nX/8),floor(nY/16)),nFlag)
	end
	SetPKMode(0,0)	--phuc hoi pk tù do
	SetPunish(0)			--bËt l¹i tÝnh n¨ng chÕt mÊt exp vµ item object
	SetCreateTeam(1)
	SetCurCamp(GetCamp())
	SetDeathScript("")
	RemoveRankBattle()
	IgnoreState()	--xoa het trang thai skill tren nguoi

	SetTempRevPos(324, 49312, 101696)--thiet lap lai diem hoi sinh o diem bao danh Tong
	NewWorld(324,1541, 3178)
	SetFightState(0)
	local nTotalAccum = 0
	if(nPlayerDataIdx > 0) then

		SetPMParam(MS_TONGKIM,nPlayerDataIdx,7,0)--lien tram hien tai
		nTotalAccum = GetPMParam(MS_TONGKIM,nPlayerDataIdx,6)
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,6,0)
	end

	SetTask(T_SAVE_TK_KILLPLAYER, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 2))
	SetTask(T_SAVE_TK_KILLNPC, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 3))
	SetTask(T_SAVE_TK_DEATH, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 4))
	SetTask(T_SAVE_TK_MAXLT, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 5))
	SetTask(T_SAVE_TK_POINT, nTotalAccum) --save l¹i ®iÓm tÝch luü khi bÞ ®Èy ra ngoµi Tèng Kim
	SetTask(T_SAVE_TK_CURLT, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 7))
	SetTask(T_SAVE_TK_NUMLAG, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 9))
	SetTask(T_SAVE_TK_RANK, GetPMParam(MS_TONGKIM,nPlayerDataIdx,11)) --save l¹i rank hiÖn t¹i khi bÞ ®Èy ra ngoµi Tèng Kim
	SetTask(T_SAVE_TK_NHATBV, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 12))
	SetTask(T_SAVE_TK_POINTKILLPL, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 13))
	SetTask(T_SAVE_TK_POINTKILLNPC, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 14))
	SetTask(T_SAVE_TK_POINTLT, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 15))
	SetTask(T_SAVE_TK_POINTBV, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 16))
	SetTask(T_SAVE_TK_POINTFLAG, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 17))
end