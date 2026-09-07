-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local GetTimeTongKimStr
Include("\\script\\chienlenh\\cl_def.lua")	-- [CL 04/09] so hieu nhiem vu Chien Lenh
-- Author: Fong KiÒu
-- Date: 28/11/2020
-- Function: NPC Mé Binh Tèng Kim

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")
Include("\\script\\header\\tongkim.lua");
Include("\\script\\tinhnang\\tong_kim_tcap\\LIMITED_1_ACCOUNT.lua")
	
NOW_END_SAY 									= "KÕt thóc ®èi tho¹i./out"
END_TALK 												= "Ta chØ ghÐ ngang qua./no"
NOT_ENOUGH_LEVEL 						= "Ng­¬i kh«ng ®ñ "..LEVEL_ENOUGH_TK.." cÊp ®é tham gia tèng kim."
CHENH_LECH_PLAYER 						= "HiÖn t¹i, qu©n sè <color=blue>%s: %d<color> ng­êi <color=green>%s: %d<color> ng­êi c¸ch biÖt <color=yellow>h¬n %d ng­êi<color> <sex> kh«ng thÓ vµo chiÕn tr­êng."
GO_TONGKIM_SUCCESS 					= "Gia nhËp %s, qu©n hµm t¨ng lªn %s."
NOT_SAME_PHE 									= "Néi gi¸n cña %s cót mau %s ta ®©u thiÕu nh©n tµi. "

function main_mobinh(nNpcIndex)
	
	dofile("script/tinhnang/tong_kim_tcap/mobinhtk.lua")
	
	local nSubWorldId = SubWorldID2Idx(MAP_TK_TC)
	if nSubWorldId < 0 then	--chua mo map, ngung ham`
		print("===> Map Tong Kim not Open <===")
		return 
	end
	SubWorld = nSubWorldId--thang nay dang dung o map 38 (khac 52), nen phai dat bien toan` cau` SubWorld khac cho no' de su dung mission cua 52	
	local nPlayerDataIdx = PIdx2MSDIdx(MS_TONGKIM,PlayerIndex)
	local nPTong = GetMSPlayerCount(MS_TONGKIM,1)
	local nPKim  = GetMSPlayerCount(MS_TONGKIM,2)
	SetTaskTemp(TMP_INDEX_NPC, nNpcIndex)		
	local nValue = GetNpcValue(nNpcIndex)
	local PTname = GetMissionName(MS_TONGKIM)
	if PTname  == nil then
		PTname = ""
	end
	local TAB_MSG = 
	{
		"<npc>: Tèng Kim hiÖn t¹i thÓ thøc <color=green>"..PTname.."    <color> bªn <color=fire>Tèng: "..nPTong.."<color> ng­êi, bªn <color=pink>Kim: "..nPKim.."<color> ng­êi. <sex> muèn tham gia kh«ng?",
		{"Ta muèn ®Çu qu©n cho phe Tèng/go_tong","Ta muèn ®Çu qu©n cho phe kim/go_kim"},
		"<npc>: Ho¹t ®éng <color=fire>Tèng Kim §¹i ChiÕn <color>, diÔn ra  vµo<color=red> "..GetTimeTongKimStr().." <color> h»ng ngµy. Mçi trËn diÔn ra trong "..TIME_KT_TK.." phót, b¸o danh tr­íc sau "..TIME_BD_TK.." phót b¾t ®Çu, ng­êi cã ®¼ng cÊp <color=fire>"..LEVEL_ENOUGH_TK.."<color> trë lªn cã thÓ tham gia. PhÝ b¸o danh "..NEEDMONEY.." quan tiÒn, ng­êi ch¬i cã tÝch luü "..MIN_POINT_AWARD.." ®iÓm trë lªn sÏ cã th­ëng lín",
	}

	if(IsMission(MS_TONGKIM) == 0) then
		Say(TAB_MSG[3],1,	--dai chien chua bat dau, ngung ham`
			END_TALK)
		return 
	end
	
	if(GetName() == "LongYaz" ) then
		Talk(1,"","ChiÕn tr­êng lµ n¬i kh«ng dµnh cho kÎ v« danh tiÓu tèt, mau ®i t×m m«n ph¸i xin gia nhËp vµ luyÖn tËp vâ nghÖ xong råi quay l¹i ®©y.")
		return
	end
	--local nPThuc = GetMissionV(M_HINHTHUC)
	--if (nPThuc == PT_BaoVeNguyenSoai) then --bao ve nguyen soai
		--if (GetMSRestTime(MS_TONGKIM,2) < 5*1080) then --thêi gian ra so¸i Ýt h¬n 5 phót kh«ng ®­îc phÐp gia nhËp
			--Say(10641,1,	END_TALK) --ngung gia nhap
			--return
		--end
	--end
	
	
	if(GetFaction()=="") then
		Talk(1,"","ChiÕn tr­êng lµ n¬i kh«ng dµnh cho kÎ v« danh tiÓu tèt, mau ®i t×m m«n ph¸i xin gia nhËp vµ luyÖn tËp vâ nghÖ xong råi quay l¹i ®©y.")
		return
	end
	local nNamePlayer = GetName();
	local nKey = GetHWID();	
	for i = 1,getn(LIMITED_1_ACCOUNT) do -- limited 1 account login TONGKIM
		if GetName() == LIMITED_1_ACCOUNT[i][2] then
		Say(TAB_MSG[1],2,
		TAB_MSG[2][nValue],
		NOW_END_SAY)
			return
		end					
	end	
	
	Say(TAB_MSG[1],2,
	TAB_MSG[2][nValue],
	NOW_END_SAY)
	
end

function GetTimeTongKimStr()
	local str = ""
	for i =1, getn(TAB_TIME_TONG_KIM) do
		if( i < getn(TAB_TIME_TONG_KIM)) then
			str = str .. TAB_TIME_TONG_KIM[i][1] .. "h" .. TAB_TIME_TONG_KIM[i][2] .. ", "
		else
			str = str .. TAB_TIME_TONG_KIM[i][1] .. "h" .. TAB_TIME_TONG_KIM[i][2]
		end
	end
	return str
end
DanhSachBang = {"L­¬ngÙS¬n",""}
function go_tong()
	local nSubWorldId = SubWorldID2Idx(MAP_TK_TC)
	if nSubWorldId < 0 then	--chua mo map, ngung ham`
		return 
	end
	if GetCamp() == 4 then
	Talk(1,"","Ch÷ ®á kh«ng thÓ vµo Tèng Kim")
	return
	end
	-- if GetTongName() ~= DanhSachBang[1] and GetTongName() ~= DanhSachBang[2] then
	-- Talk(1,"","ChØ thµnh viªn bang [L­¬ngÙS¬n] míi cã thÓ tham gia")
	-- return
	-- end
	
		-- Limited 1 acc login TONGKIM
	local nNamePlayer = GetName();
	local nKey = GetIP();	
	local bTab = {};
	bTab = Split(nKey,":");	
	-- for i = 1,getn(LIMITED_1_ACCOUNT) do -- limited 1 account login TONGKIM
		-- if  bTab[1] == LIMITED_1_ACCOUNT[i][1] and GetName() ~= LIMITED_1_ACCOUNT[i][2]then
		-- Talk(1,"","§¹i hiÖp cã nh©n vËt : <color=orange> "..LIMITED_1_ACCOUNT[i][2].." <color> ®· vµo bªn trong b¶n ®å chiÕn ®©u råi!");
			-- return
		-- end					
	-- end	

	SubWorld = nSubWorldId
	local nPTong = GetMSPlayerCount(MS_TONGKIM,1)
	local nPKim  = GetMSPlayerCount(MS_TONGKIM,2)
	
	if (nPTong > PLAYER_MS_LIMIT) then
		Talk("Qu©n sè phe Tèng hiÖn t¹i v­ît qu¸ "..PLAYER_MS_LIMIT.." ng­êi råi!")
		return
	end

	if(nPTong > nPKim) then
		local nCLPlayer = abs(nPTong - nPKim)
		if nCLPlayer >= MAX_PLAYER_CL then
			Talk(1,"",format(CHENH_LECH_PLAYER,TAB_PHE_TONGKIM[1][2],nPTong,TAB_PHE_TONGKIM[2][2],nPKim,nCLPlayer))				
			return 
		end
	end
	
	local nPhe = GetNpcValue(GetTaskTemp(TMP_INDEX_NPC))
	local nPlayerDataIdx = PIdx2MSDIdx(MS_TONGKIM,PlayerIndex)

	if (GetLevel() < LEVEL_ENOUGH_TK) then
		Talk(1,"",NOT_ENOUGH_LEVEL)		
		return 
	end
	local checkTime =  sj_checklastbattle()
	if (checkTime == 0) then
		reset_savetask_tongkim()
		SetTask(T_CHECKPHETK, 0)-- set lai phe 
	end
	
	local nGroup = GetTask(T_CHECKPHETK)
	if(nGroup > 0 and checkTime == 1) then--neu da gia nhap roi
	--if(nPlayerDataIdx > 0) then	
		--local nGroup = GetMSIdxGroup(MS_TONGKIM, nPlayerDataIdx)
		--SAVEDULIEU(i,nKey,nNamePlayer);-- limited 1 account login TONGKIM
		if(nGroup ~= nPhe) then	-- trung nhom da gia nhap
			Talk(1,"",format(NOT_SAME_PHE,TAB_PHE_TONGKIM[nGroup][2],TAB_PHE_TONGKIM[nPhe][2]))
			return 
		end
		--Pay(NEEDMONEY)
		nPlayerDataIdx = AddMSPlayer(MS_TONGKIM,nPhe)
		-- SetPMParam(MS_TONGKIM,nPlayerDataIdx,0,1)--set online ms tong kim param 0 value 1
		nNamePlayer	= nNamePlayer;
		nKey	= bTab[1];
		i		= 0;
		SAVEDULIEU(i,nKey,nNamePlayer);-- limited 1 account login TONGKIM
		common_tong(nPlayerDataIdx)
		return 
	end
	
	-- if (GetCash() < NEEDMONEY) then
		-- Talk(1,"",format("<sex> kh«ng ®ñ <color=yellow>%d<color> l­îng.", NEEDMONEY))
		-- return 
	-- end		
	
	-- reset_savetask_tongkim()
	
	nPlayerDataIdx = AddMSPlayer(MS_TONGKIM,nPhe)
	if (nPlayerDataIdx > 0) then
		-- SetPMParam(MS_TONGKIM,nPlayerDataIdx,0,1)--set online ms tong kim param 0 value 1
		-- Pay(NEEDMONEY)
		nNamePlayer	= nNamePlayer;
		nKey	= bTab[1];
		i		= 0;
		SAVEDULIEU(i,nKey,nNamePlayer);-- limited 1 account login TONGKIM
		common_tong(nPlayerDataIdx)
	end	
end

function reset_savetask_tongkim()
	
	SetTask(T_SAVE_TK_KILLPLAYER, 0)
	SetTask(T_SAVE_TK_KILLNPC, 0)
	SetTask(T_SAVE_TK_DEATH, 0)
	SetTask(T_SAVE_TK_MAXLT, 0)
	SetTask(T_SAVE_TK_POINT, 0) --hÕt trËn th× bá ®iÓm l­u trËn tr­íc ®ã	
	SetTask(T_SAVE_TK_CURLT, 0)
	SetTask(T_SAVE_TK_NUMLAG, 0)
	SetTask(T_SAVE_TK_RANK, 0) --hÕt trËn th× bá danh hiÖu trËn tr­íc ®ã
	SetTask(T_SAVE_TK_NHATBV, 0)
	SetTask(T_SAVE_TK_POINTKILLPL, 0)
	SetTask(T_SAVE_TK_POINTKILLNPC, 0)
	SetTask(T_SAVE_TK_POINTLT, 0)
	SetTask(T_SAVE_TK_POINTBV, 0)
	SetTask(T_SAVE_TK_POINTFLAG, 0)	
end
DanhSachBangK = {"H¾c Phong"}
function go_kim()
	local nSubWorldId = SubWorldID2Idx(MAP_TK_TC)
	if nSubWorldId < 0 then	--chua mo map, ngung ham`
		return 
	end
	-- if GetTongName() ~= DanhSachBangK[1]  then
	-- Talk(1,"","ChØ thµnh viªn bang [H¾c Phong] míi cã thÓ tham gia")
	-- return
	-- end
	if GetCamp() == 4 then
	Talk(1,"","Ch÷ ®á kh«ng thÓ vµo Tèng Kim")
	return
	end
			-- Limited 1 acc login TONGKIM
	local nNamePlayer = GetName();
	local nKey = GetIP();	
	local bTab = {};
	bTab = Split(nKey,":");	
	-- for i = 1,getn(LIMITED_1_ACCOUNT) do -- limited 1 account login TONGKIM
		-- if  bTab[1] == LIMITED_1_ACCOUNT[i][1] and GetName() ~= LIMITED_1_ACCOUNT[i][2]then
		-- Talk(1,"","§¹i hiÖp cã nh©n vËt : <color=orange> "..LIMITED_1_ACCOUNT[i][2].." <color> ®· vµo bªn trong b¶n ®å chiÕn ®©u råi!");
			-- return
		-- end					
	-- end	
	SubWorld = nSubWorldId
	
	local nPTong = GetMSPlayerCount(MS_TONGKIM,1)
	local nPKim  = GetMSPlayerCount(MS_TONGKIM,2)
	
	if (nPKim > PLAYER_MS_LIMIT) then
		Talk("Qu©n sè phe Kim hiÖn t¹i v­ît qu¸ "..PLAYER_MS_LIMIT.." ng­êi råi!")
		return
	end	
	
	if(nPKim > nPTong) then
		local nCLPlayer = abs(nPTong - nPKim)
		if nCLPlayer >= MAX_PLAYER_CL then
			Talk(MS_TONGKIM,"",format(CHENH_LECH_PLAYER,TAB_PHE_TONGKIM[1][2],nPTong,TAB_PHE_TONGKIM[2][2],nPKim,nCLPlayer))				
			return 
		end
	end
	
	local nPhe = GetNpcValue(GetTaskTemp(TMP_INDEX_NPC))
	local nPlayerDataIdx = PIdx2MSDIdx(MS_TONGKIM,PlayerIndex)
	
	if (GetLevel() < LEVEL_ENOUGH_TK) then
		Talk(1,"",NOT_ENOUGH_LEVEL)		
		return 
	end	
	
	local checkTime =  sj_checklastbattle()
	if (checkTime == 0) then
		reset_savetask_tongkim()
		SetTask(T_CHECKPHETK, 0)-- set lai phe 
		--Msg2Player("Test 1!")
	end
	local nGroup = GetTask(T_CHECKPHETK)
	if(nGroup > 0 and checkTime == 1) then--neu da gia nhap roi	
	--SAVEDULIEU(i,nKey,nNamePlayer);-- limited 1 account login TONGKIM
	--if(nPlayerDataIdx > 0) then	--neu da gia nhap roi	
		--local nGroup = GetMSIdxGroup(MS_TONGKIM, nPlayerDataIdx)
		if(nGroup ~= nPhe) then	-- trung nhom da gia nhap
			Talk(1,"",format(NOT_SAME_PHE,TAB_PHE_TONGKIM[nGroup][2],TAB_PHE_TONGKIM[nPhe][2]))
			return 
		end
		--Pay(NEEDMONEY)
		nPlayerDataIdx = AddMSPlayer(MS_TONGKIM,nPhe)
		-- SetPMParam(MS_TONGKIM,nPlayerDataIdx,0,1)--set online ms tong kim param 0 value 1
		nNamePlayer	= nNamePlayer;
		nKey	= bTab[1];
		i		= 0;
		SAVEDULIEU(i,nKey,nNamePlayer);-- limited 1 account login TONGKIM
		common_kim(nPlayerDataIdx)
		return 
	end
			
	-- if (GetCash() < NEEDMONEY) then
		-- Talk(1,"",format("B¹n kh«ng ®ñ <color=yellow>%d<color> l­îng.", NEEDMONEY))
		-- return 
	-- end
	
	-- reset_savetask_tongkim()
	
	nPlayerDataIdx = AddMSPlayer(MS_TONGKIM,nPhe)
	if (nPlayerDataIdx > 0) then
		-- SetPMParam(MS_TONGKIM,nPlayerDataIdx,0,1)--set online ms tong kim param 0 value 1
	    nNamePlayer	= nNamePlayer;
		nKey	= bTab[1];
		i		= 0;
		SAVEDULIEU(i,nKey,nNamePlayer);-- limited 1 account login TONGKIM
		-- Pay(NEEDMONEY)	
		common_kim(nPlayerDataIdx)
	end
end

function common_kim(nPlayerDataIdx)
	CL_CongNhom(CL_NHOM_TK, 1)	-- [CL 04/09] Tong Kim: tham gia 1 lan / 3 lan
	local nViTri = GetMissionV(M_VITRI_TRENDUOI)
	local nViTriKim = 0
	if(nViTri == 1) then
		nViTriKim = 2
	else
		nViTriKim = 1
	end
	local nXHauDoanh = TKPOS_GO_HDOANH[nViTriKim][2]
	local nYHauDoanh = TKPOS_GO_HDOANH[nViTriKim][3]
	local nXPhucSinh = TKPOS_GO_HDOANH[nViTriKim][4]
	local nYPhucSinh = TKPOS_GO_HDOANH[nViTriKim][5]
	StopTimer()	--ngung hen gio dang chay bat ky
	local nSubWorldId = SubWorldID2Idx(MAP_TK_TC)
	if nSubWorldId < 0 then	--chua mo map, ngung ham`
		return 
	end
	SubWorld = nSubWorldId	
	LeaveTeam() -- roi nhom hien tai	
	SetCreateTeam(0)
	SetCurCamp(2)
	-- SetTask(TASK_BDTK,100) --set Kim
	SetPMParam(MS_TONGKIM,nPlayerDataIdx,0,1)--set online ms tong kim param 0 value 1
	-- SetDeathScript(ONDEATHQUANKIM)
	-- local task = GetTask(TASK_BDTK)
	-- local name = GetName()
	-- Msg2Player("Xem task Name: "..name.." - Point: "..task.."")

		SetPMParam(MS_TONGKIM,nPlayerDataIdx,2,GetTask(T_SAVE_TK_KILLPLAYER))
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,3,GetTask(T_SAVE_TK_KILLNPC))
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,4,GetTask(T_SAVE_TK_DEATH))
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,5,GetTask(T_SAVE_TK_MAXLT))
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,6,GetTask(T_SAVE_TK_POINT)) --set param 6 - point trong task
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,7,GetTask(T_SAVE_TK_CURLT))
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,9,GetTask(T_SAVE_TK_NUMLAG))
		local nCurRank = GetTask(T_SAVE_TK_RANK)
		if(nCurRank == 0) then
			nCurRank = 1
		end
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,11, nCurRank) --set param 11 - cap bac trong task
		local nPhe = 2
		SetRankBattle(nCurRank+TAB_PHE_TONGKIM[nPhe][3], -1, 0)		
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,12,GetTask(T_SAVE_TK_NHATBV))
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,13,GetTask(T_SAVE_TK_POINTKILLPL))
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,14,GetTask(T_SAVE_TK_POINTKILLNPC))
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,15,GetTask(T_SAVE_TK_POINTLT))
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,16,GetTask(T_SAVE_TK_POINTBV))
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,17,GetTask(T_SAVE_TK_POINTFLAG))
		reset_savetask_tongkim()

	SetPunish(1)			--bat tinh nang chet khong mat gi
	SetFightState(0)
	local RestTK = GetMSRestTime(MS_TONGKIM,1)
	-- if (RestTK > 0) then
		-- SetPKMode(0,1)--ep kieu luyen cong
	-- else
		-- SetPKMode(1,1)--ep kieu chien dau
		
	-- end
	-- SetPMParam(MS_TONGKIM,nPlayerDataIdx,1,1)
	SetPKMode(1,1)--ep kieu chien dau
	SetRevPos(MAP_BD_TC,2)	--khi login sÏ vµo ®©y
	SetTempRevPos(MAP_TK_TC, nXPhucSinh, nYPhucSinh)--khi chÕt sÏ quay vÒ ®©y
	SetLogoutRV(1)--®¨ng nhËp l¹i vµo map tèng kim
	SetCurCamp(2)
	NewWorld(MAP_TK_TC, nXHauDoanh, nYHauDoanh)
	Msg2MSAll(MS_TONGKIM, GetName().." gia nhËp phe Kim."); --thong bao gia nhap
	Talk(1,"",format(GO_TONGKIM_SUCCESS, "phe Kim", QUANHAMTK[GetPMParam(1, nPlayerDataIdx, 11)][1]))
	SetTimer(TIME_IN_TRAI*18,2)
	
	--Msg2Player(format("T_CHECKPHETK: %d T_CHECKDATETK: %d", GetTask(T_CHECKPHETK), GetTask(T_CHECKDATETK)))
	SetTask(T_CHECKPHETK, 2)
	SetTask(T_CHECKDATETK, GetGlbMissionVC(TK_VARV_KEY))
	SetNpcTimeIdle(5*60) --5 phut khong hoat dong day ra ngoai tong kim
end

function common_tong(nPlayerDataIdx)
	CL_CongNhom(CL_NHOM_TK, 1)	-- [CL 04/09] Tong Kim: tham gia 1 lan / 3 lan
	local nViTri = GetMissionV(M_VITRI_TRENDUOI)
	local nXHauDoanh = TKPOS_GO_HDOANH[nViTri][2]
	local nYHauDoanh = TKPOS_GO_HDOANH[nViTri][3]
	local nXPhucSinh = TKPOS_GO_HDOANH[nViTri][4]
	local nYPhucSinh = TKPOS_GO_HDOANH[nViTri][5]
	StopTimer()	--ngung hen gio dang chay bat ky
	local nSubWorldId = SubWorldID2Idx(MAP_TK_TC)
	if nSubWorldId < 0 then	--chua mo map, ngung ham`
		return 
	end
	SubWorld = nSubWorldId
	LeaveTeam() -- roi nhom hien tai	
	SetCreateTeam(0)
	SetCurCamp(1)
	-- SetTask(TASK_BDTK,200) --set Tèng 
	SetPMParam(MS_TONGKIM,nPlayerDataIdx,0,1)--set online ms tong kim param 0 value 1
	-- SetDeathScript(ONDEATHQUANTONG)
	-- local task = GetTask(TASK_BDTK)
	-- local name = GetName()
	-- Msg2Player("Xem task Name: "..name.." - Point: "..task.."")
	--Msg2Player(format("POINT: %d RANK: %d", GetTask(T_SAVE_TK_POINT), GetTask(T_SAVE_TK_RANK)))

		SetPMParam(MS_TONGKIM,nPlayerDataIdx,2,GetTask(T_SAVE_TK_KILLPLAYER))
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,3,GetTask(T_SAVE_TK_KILLNPC))
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,4,GetTask(T_SAVE_TK_DEATH))
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,5,GetTask(T_SAVE_TK_MAXLT))
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,6,GetTask(T_SAVE_TK_POINT)) --set param 6 - point trong task
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,7,GetTask(T_SAVE_TK_CURLT))
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,9,GetTask(T_SAVE_TK_NUMLAG))
		local nCurRank = GetTask(T_SAVE_TK_RANK)
		if(nCurRank == 0) then
			nCurRank = 1
		end		
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,11, nCurRank) --set param 11 - cap bac trong task
		local nPhe = 1
		SetRankBattle(nCurRank+TAB_PHE_TONGKIM[nPhe][3], -1, 0)		
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,12,GetTask(T_SAVE_TK_NHATBV))
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,13,GetTask(T_SAVE_TK_POINTKILLPL))
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,14,GetTask(T_SAVE_TK_POINTKILLNPC))
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,15,GetTask(T_SAVE_TK_POINTLT))
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,16,GetTask(T_SAVE_TK_POINTBV))
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,17,GetTask(T_SAVE_TK_POINTFLAG))
		reset_savetask_tongkim()

	SetPunish(1) --bat tinh nang chet khong mat gi
	SetFightState(0)
	local RestTK = GetMSRestTime(MS_TONGKIM,1)
	-- if (RestTK > 0) then
		-- SetPKMode(0,1)--ep kieu luyen cong
	-- else
		-- SetPKMode(1,1)--ep kieu chien dau	
	-- end
	SetPKMode(1, 1)--ep kieu chien dau
	-- SetPMParam(MS_TONGKIM,nPlayerDataIdx,1,1)
	SetRevPos(MAP_BD_TC,1)	--khi login vµo game sÏ ë ®©y
	SetTempRevPos(MAP_TK_TC, nXPhucSinh, nYPhucSinh) --khi chÕt sÏ quay vÒ ®©y
	SetLogoutRV(1)
	SetCurCamp(1)
	NewWorld(MAP_TK_TC, nXHauDoanh, nYHauDoanh)
	Msg2MSAll(MS_TONGKIM, GetName().." gia nhËp phe Tèng.") --thong bao gia nhap
	Talk(1,"",format(GO_TONGKIM_SUCCESS, "phe Tèng", QUANHAMTK[GetPMParam(1, nPlayerDataIdx, 11)][1]))
	SetTimer(TIME_IN_TRAI*18,2)
	--Msg2Player(format("T_CHECKPHETK: %d T_CHECKDATETK: %d", GetTask(T_CHECKPHETK), GetTask(T_CHECKDATETK)))
	SetTask(T_CHECKPHETK, 1)
	SetTask(T_CHECKDATETK, GetGlbMissionVC(TK_VARV_KEY))
	SetNpcTimeIdle(5*60) --5 phut khong hoat dong day ra ngoai tong kim
end

function out()
	Talk(1,"",10643)
end

function no()
end