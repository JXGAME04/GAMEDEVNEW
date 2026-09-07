--Author: Fong KiÒu
--Date: 01/07/2021
--Function: Mission tèng kim

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\header\\tongkim.lua");
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")

PLAYER_LOGOUT_FLAG = "%s <color=purple>%s<color> ®· ®¸nh r¬i cê t¹i <color=yellow>%d/%d<color> h·y lªn ®o¹t l¹i nµo."

function BeginMission()
	local nSubWorldId = SubWorldID2Idx(MAP_TK_TC)
	if nSubWorldId < 0 then	--chua mo map, ngung ham`
		print("===> BeginMission Tong Kim Close by Map not Open <===")
		return
	end
	SubWorld = nSubWorldId
	SetMission(M_TICHLUYA, 	0)
	SetMission(M_TICHLUYB, 	0)
	SetGlbMissionV(TK_VARV_KEY,  tonumber(GetLocalDateEx()))
	
	print("===> BeginMission Tong Kim Open <===")
end

function EndMission()
	local nSubWorldId = SubWorldID2Idx(MAP_TK_TC)
	if nSubWorldId < 0 then	--chua mo map, ngung ham`
		print("===> EndMission Tong Kim Close by Map not Open <===")
		return 
	end
	SubWorld = nSubWorldId
	
	SetMission(M_HINHTHUC, 	0)
	SetMission(M_SOCOPHEA, 	0)
	SetMission(M_SOCOPHEB, 	0)
	SetMission(M_SOTRAN, 		0)
	SetMission(M_ADDNPCA, 	0)
	SetMission(M_ADDNPCB, 		0)
	SetMission(M_VITRI_TRENDUOI, 0)
	SetMission(M_NPCIDX_SOAITONG, 0)
	SetMission(M_NPCIDX_SOAIKIM, 0)
	print("===> EndMission Tong Kim  <===")
	
end

--loai bo player ra khoi mission thi se chay ham nay, thoat ra cung la loai bo
function OnLeave(nPlayerIndex)
	if(SubWorld == SubWorldID2Idx(MAP_TK_TC)) then
		local nPlayerDataIdx
		PlayerIndex = nPlayerIndex
		local nW, nX, nY = GetWorldPos()
		
		nPlayerDataIdx = PIdx2MSDIdx(MS_TONGKIM, PlayerIndex)
		SetPMParam(MS_TONGKIM, nPlayerDataIdx, 0, 0)	--param 0 value 0 set offline tong kim
		
		Msg2MSAll(MS_TONGKIM, format("%s ®· rêi khái chiÕn tr­êng Tèng Kim.", GetName())) --thong bao roi khoi~
		local nFlag = GetPMParam(MS_TONGKIM,nPlayerDataIdx, 8) -- dang giu co
		if (nFlag > 0) then
			SetPMParam(MS_TONGKIM, nPlayerDataIdx, 8, 0)			   -- mat giu co
			local nNpcIndex = AddNpcEx1({TAB_PHE_TONGKIM[nFlag][7]}, 1, {0}, MAP_TK_TC, nX*32, nY*32, nil, DOSCRIPTCO, nil, 6)
			AddMSNpc(MS_TONGKIM, nNpcIndex)
			SetNpcValue(nNpcIndex, nFlag)
			Msg2MSGroup(MS_TONGKIM, format(PLAYER_LOGOUT_FLAG, TAB_PHE_TONGKIM[nFlag][2], GetName(), floor(nX/8), floor(nY/16)), nFlag)
		end
		SetPKMode(0, 0)	--phuc hoi pk tù do
		SetPunish(0)			--bËt l¹i tÝnh n¨ng chÕt mÊt exp vµ item object
		SetCreateTeam(1)
		SetCurCamp(GetCamp())
		SetDeathScript("")
		RemoveRankBattle()
		SetNpcTimeIdle(0)
		IgnoreState()	--xoa het trang thai skill tren nguoi
		SetTempRevPos(324, 49312, 101696)--thiet lap lai diem hoi sinh o diem bao danh Tong
		if(nPlayerDataIdx > 0) then
			SetTask(T_SAVE_TK_KILLPLAYER, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 2))
			SetTask(T_SAVE_TK_KILLNPC, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 3))
			SetTask(T_SAVE_TK_DEATH, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 4))
			SetTask(T_SAVE_TK_MAXLT, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 5))
			SetTask(T_SAVE_TK_POINT, GetPMParam(MS_TONGKIM, nPlayerDataIdx, 6)) --save l¹i ®iÓm tÝch luü khi bÞ ®Èy ra ngoµi Tèng Kim
			SetTask(T_SAVE_TK_CURLT, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 7))
			SetTask(T_SAVE_TK_NUMLAG, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 9))
			SetTask(T_SAVE_TK_RANK, GetPMParam(MS_TONGKIM,nPlayerDataIdx,11)) --save l¹i rank hiÖn t¹i khi bÞ ®Èy ra ngoµi Tèng Kim
			SetTask(T_SAVE_TK_NHATBV, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 12))
			SetTask(T_SAVE_TK_POINTKILLPL, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 13))
			SetTask(T_SAVE_TK_POINTKILLNPC, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 14))
			SetTask(T_SAVE_TK_POINTLT, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 15))
			SetTask(T_SAVE_TK_POINTBV, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 16))
			SetTask(T_SAVE_TK_POINTFLAG, GetPMParam(MS_TONGKIM,nPlayerDataIdx, 17))
			--Msg2Player(format("T_SAVE_TK_POINT: %d T_SAVE_TK_RANK: %d", GetTask(T_SAVE_TK_POINT), GetTask(T_SAVE_TK_RANK)))
		end
	end
end

