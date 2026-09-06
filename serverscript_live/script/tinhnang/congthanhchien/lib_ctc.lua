--Author: Fong KiÒu
--Date: 28/11/2020
--Function: Lib C«ng Thµnh ChiÕn

Include("\\script\\lib\\lib_ham.lua")
-- [CFGCTC 30/08] hai tep duoi day la LA (khong Include gi).
Include("\\script\\cauhinh\\ch_lib.lua")
Include("\\script\\cauhinh\\ch_chung.lua")
-- [CFGCTC 30/08] Bo doc cau hinh cho tep nay. Tra ve MAC DINH (= so cu)
-- khi bo cau hinh chua nap, nen kem nhat cung khong the doi hanh vi.
function CTC_CFG(szKhoa, macdinh)
	if (G_CFG ~= nil) then
		return G_CFG(szKhoa, macdinh)
	end
	return macdinh
end

Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\log_game\\save_log.lua")
Include("\\script\\tinhnang\\congthanhchien\\danhsach_bang.lua")

function GetNameBangCong()
	if(BANGHOI_DANHSACH ~= nil and getn(BANGHOI_DANHSACH) > 1) then
		return BANGHOI_DANHSACH[2][2]
	end
	return "0"
end

function GetNameBangThu()
	if(BANGHOI_DANHSACH ~= nil and getn(BANGHOI_DANHSACH) >= 1) then
		return BANGHOI_DANHSACH[1][2]
	end
	return "0"	
end

function GetIdBangCong()
	if(BANGHOI_DANHSACH ~= nil and getn(BANGHOI_DANHSACH) > 1) then
		return BANGHOI_DANHSACH[2][1]
	end
	return 0
end

function GetIdBangThu()
	if(BANGHOI_DANHSACH ~= nil and getn(BANGHOI_DANHSACH) >= 1) then
		return BANGHOI_DANHSACH[1][1]
	end
	return 0	
end

function GetMasterBangCong()
	if(BANGHOI_DANHSACH ~= nil and getn(BANGHOI_DANHSACH) > 1) then
		return BANGHOI_DANHSACH[2][3]
	end
	return "0"
end

function GetMasterBangThu()
	if(BANGHOI_DANHSACH ~= nil and getn(BANGHOI_DANHSACH) >= 1) then
		return BANGHOI_DANHSACH[1][3]
	end
	return "0"	
end

function GetThueSuat()
	if(BANGHOI_DANHSACH ~= nil and getn(BANGHOI_DANHSACH) >= 1) then
		return BANGHOI_DANHSACH[1][5]
	end
	return 0
end

function SetThueSuat(nNum)
	if(BANGHOI_DANHSACH ~= nil and getn(BANGHOI_DANHSACH) >= 1) then
		BANGHOI_DANHSACH[1][5] = nNum
		local tbDataTongNew = TaoBang(BANGHOI_DANHSACH,"BANGHOI_DANHSACH","")
		SaveData("script/tinhnang/congthanhchien/danhsach_bang.lua",tbDataTongNew)
		local nTongName = GetTongInfo(1)
		local msg = format("Bang chñ %s bang <color=green> %s <color> ®· ®iÒu chØnh thuÕ suÊt míi <color=yellow> %d %s <color>¸p ®Æt lªn thµnh %s. ", GetName(), nTongName, nNum,"%", CITYWAR_NAME)
		Talk(1,"",msg)
		logHoatDong(msg)
		Msg2SubWorld(msg)
	end
end

MIN_TAX 												= 0
MAX_TAX 												= 20
CITYWAR_NAME									= "T­¬ng D­¬ng"
NAME_BANGCONG 								= GetNameBangCong()
NAME_BANGTHU									= GetNameBangThu()
ID_BANGCONG 										= GetIdBangCong()
ID_BANGTHU											= GetIdBangThu()
MASTER_BANGCONG 							= GetMasterBangCong()
MASTER_BANGTHU								= GetMasterBangThu()
ENOUGH_LEVEL									= 80
TICH_LUY_THUONG_CT 					= 100
ID_MAP_CTC											= 221
ID_MAP_CTC_HDPT								= 222
ID_MAP_CTC_HDPC								= 223
CTC_MINUS_BD										= 10
CTC_MINUS_KT										= 60
CITYWAR_TOP10_EXP 						= CTC_CFG("CTC_EXP_TOP10_BANSAO", 3000000) -- th­ëng exp cho TOP 10 c«ng thµnh
MAX_CAMP1COUNT 							= CTC_CFG("CTC_MAX_NGUOI_PHE_THU_VEBINH", 50) --sè ng­êi tèi ®a phe thñ
MAX_CAMP2COUNT 							= CTC_CFG("CTC_MAX_NGUOI_PHE_CONG_VEBINH", 50) --sè ng­êi tèi ®a phe c«ng
MAX_CAMP3COUNT 							= CTC_CFG("CTC_MAX_LIENMINH_THU_VEBINH", 5) --sè ng­êi phe thñ liªn minh tham gia thªm
MAX_CAMP4COUNT 							= CTC_CFG("CTC_MAX_LIENMINH_CONG_VEBINH", 5) --sè ng­êi phe c«ng liªn minh tham gia thªm
ITEM_TONG_AWD								= {6,1,146,5}
KILL_PLAYER_POINT							= 30
KILL_TRU_POINT									= CTC_CFG("CTC_DIEM_PHA_LONGTRU", 300)

TAB_TIME_CONG_THANH = {
	--1 time loi dai, 2 minus loi dai, 3 day loi dai, 4 time cong thanh, 5 minus cong thanh, 6 day cong thanh day ®­îc tÝnh 0 ®Õn 6 t­¬ng øng chñ nhËt ®Õn thø 7
	{20,30,4,20,30,5}, --l«i ®µi thø 5 vµ c«ng thµnh thø 6 lóc 20h30
}

TAB_PHE_CONGTHANH = {
	--		2         3                       4              5             6
	{1,"phe Thñ" , {ID_MAP_CTC,1285,2934}, {ID_MAP_CTC_HDPT,1614,3172}, {78,1565,3227}, {ID_MAP_CTC_HDPT,1}},
	{2,"phe C«ng", {ID_MAP_CTC,1631,3274}, {ID_MAP_CTC_HDPC, 1614, 3172}, {1,1499,3172}, {ID_MAP_CTC_HDPC,2}},
}

DATA_CONGTHANH = {
	{1, 1477, 3087, "TÞnh D­¬ng M«n", M_CTHANHVO_1},	-- tÞnh d­¬ng m«n
	{2, 1541, 3024, "§Þnh Xuyªn M«n", M_CTHANHVO_2},	-- dinh xuyen
	{3, 1413, 3148, "B×nh Giang M«n", M_CTHANHVO_3},	-- binh giang
}

DATA_TRUTHANH = {
	{1, 1415, 3024, "LËp D­¬ng Long Trô" },	-- lap duong
	{2, 1476, 2956, "§Þnh Xuyªn Long Trô"},-- dinh xuyen
	{3, 1356, 3091, "B×nh Giang Long Trô"},	-- binh giang
}

HOVE_NPC ={
	{1590,3424},
	{1582,3417},
	{1581,3399},
	{1590,3394},
	{1602,3406},
	{1598,3420},
	{1655,3359},
	{1646,3352},
	{1646,3336},
	{1655,3331},
	{1665,3339},
	{1662,3355},
	{1722,3293},
	{1713,3285},
	{1713,3268},
	{1724,3264},
	{1733,3274},
	{1729,3286}
}

TAB_RANKCONG ={
	{1,"Hé Quèc nghÜa sÜ"},		-- ansy
	{2,"Chiªu Vâ HiÖu óy"},
	{3,"Trung Lang T­íng"},
	{4,"Phiªu Kú §¹i T­íng Qu©n"},
	{5,"§¹i So¸i"},
}

TAB_RANKTHU ={
	{6,"Kþ §« Uý"},		-- ansy
	{7,"Thanh Long T­íng"},
	{8,"XÝch Long T­íng"},
	{9,"ChiÕn Quèc Tiªn Phong Sø"},
	{10,"Chñ T­íng"},
}

TAB_QUANHAM = {
	{100},
	{200},		
	{400},
	{600},
	{800},
}

DEATHFILE_PCONG 			= "\\script\\tinhnang\\congthanhchien\\congtu.lua"
DEATHFILE_PTHU  				= "\\script\\tinhnang\\congthanhchien\\thutu.lua"
DEATHFILE_CONG 				= "\\script\\tinhnang\\congthanhchien\\death_cong.lua"
DEATHFILE_TRU					= "\\script\\tinhnang\\congthanhchien\\death_tru.lua"
FILE_VBINHMATDAO 			= "\\script\\tinhnang\\congthanhchien\\vebinhmatdao.lua"
FILE_RUONGCHUA 				= "\\script\\tinhnang\\congthanhchien\\ruongchua.lua"
FILE_XAPHU 							= "\\script\\tinhnang\\congthanhchien\\xaphu.lua"
FILE_QUANQUAN 				= "\\script\\tinhnang\\congthanhchien\\quanquan.lua"
FILE_QUANY 							= "\\script\\tinhnang\\congthanhchien\\quany.lua"
-- [PB 30/08] hang tro vao khoang khong: congthanhquan.lua da doi sang
-- _dara (mo coi - ca hai cho dung o dong 243-244 deu da comment tu truoc).
-- FILE_CTHANHQUAN 			= "\\script\\tinhnang\\congthanhchien\\congthanhquan.lua"
FILE_NHAMON 						= "\\script\\tinhnang\\congthanhchien\\nhamon.lua" --ch­a sö dông
DEATHFILE_CONGCU 			= "\\script\\tinhnang\\congthanhchien\\death_npc.lua" --ch­a sö dông

function addtrucongthanh()
	local nNpcIdx
--	ho ve
	--for i=1,getn(HOVE_NPC) do
	--	nNpcIdx = AddNpcEx3({682},100,{0,1,2,3,4},ID_MAP_CTC,HOVE_NPC[i][1]*32,HOVE_NPC[i][2]*32,nil,DEATHFILE_CONGCU,"Long Trô VÖ Binh",1,0,100000,nil,1000,200,200,250,2000,30)
	--	AddMSNpc(MS_CTHANHCHIEN,nNpcIdx,1)
	--	SetNpcValue(nNpcIdx,5)
	--end	
	
-- cong thanh
	for i=1,getn(DATA_CONGTHANH) do
		nNpcIdx = AddNpcEx3({532},1,{0,1,2,3,4},ID_MAP_CTC,DATA_CONGTHANH[i][2]*32,DATA_CONGTHANH[i][3]*32,nil,DEATHFILE_CONG,DATA_CONGTHANH[i][4],1,0,500000)
		AddMSNpc(MS_CTHANHCHIEN,nNpcIdx,1)
		SetNpcValue(nNpcIdx,DATA_CONGTHANH[i][1])
		SetNpcTimer(nNpcIdx,CTC_MINUS_KT*60*18)
	end

-- tru thanh
	for i=1,getn(DATA_TRUTHANH) do
		nNpcIdx = AddNpcEx3({530},1,{0,1,2,3,4},ID_MAP_CTC,DATA_TRUTHANH[i][2]*32,DATA_TRUTHANH[i][3]*32,nil,DEATHFILE_TRU,DATA_TRUTHANH[i][4],1,0,500000)
		AddMSNpc(MS_CTHANHCHIEN,nNpcIdx,1)
		SetNpcValue(nNpcIdx,DATA_TRUTHANH[i][1])
		SetNpcTimer(nNpcIdx,CTC_MINUS_KT*60*18)
	end

	SetMission(M_LONGTRULD, 		1) --set tru thuoc ve phe thu
	SetMission(M_LONGTRUDX, 		1) --set tru thuoc ve phe thu
	SetMission(M_LONGTRUBG, 		1) --set tru thuoc ve phe thu
end

function addnpccongthanh()
	local nNpcIdx
	--AddNpcEx1({55} ,1,nil,ID_MAP_CTC,1923*32,3578*32,"",FILE_QUANY,"Tïy qu©n d­îc y",6)	-- cong quan y
	--AddNpcEx1({49} ,1,nil,ID_MAP_CTC,1547*32,3219*32,"",FILE_QUANY,"Tïy qu©n d­îc y",6)	-- thu quan y
	
	--nNpcIdx = AddNpcEx1({625},1,nil,ID_MAP_CTC,1882*32,3581*32,"",FILE_RUONGCHUA,nil,6) SetNpcValue(nNpcIdx,51);	-- ruong
	--nNpcIdx = AddNpcEx1({625},1,nil,ID_MAP_CTC,1540*32,3245*32,"",FILE_RUONGCHUA,nil,6) SetNpcValue(nNpcIdx,52);	-- ruong

	nNpcIdx = AddNpcEx1({625},1,nil,222,1613*32,3185*32,"",FILE_RUONGCHUA,nil,6) SetNpcValue(nNpcIdx,52);	-- ruong
	nNpcIdx = AddNpcEx1({625},1,nil,223,1613*32,3185*32,"",FILE_RUONGCHUA,nil,6) SetNpcValue(nNpcIdx,52);	-- ruong
	
	--AddNpcEx1({62} ,1,nil,ID_MAP_CTC,1561*32,3233*32,"",FILE_VBINHMATDAO,"VÖ binh mËt ®¹o",6)

	--AddNpcEx1({203},1,nil,ID_MAP_CTC_HDPC,1614*32,3162*32,"",FILE_QUANY,"Tïy qu©n d­îc y",6)	-- cong quan y
	--AddNpcEx1({203},1,nil,ID_MAP_CTC_HDPT,1614*32,3162*32,"",FILE_QUANY,"Tïy qu©n d­îc y",6)	-- thu quan y
	
	--AddNpcEx1({55} ,1,nil,ID_MAP_CTC_HDPC,1603*32,3152*32,"",FILE_QUANQUAN,"Qu©n nhu quan",6)
	--AddNpcEx1({49} ,1,nil,ID_MAP_CTC_HDPT,1603*32,3152*32,"",FILE_QUANQUAN,"Qu©n nhu quan",6)
	
	--AddNpcEx1({237},1,nil,ID_MAP_CTC_HDPC,1639*32,3191*32,"",FILE_XAPHU,"Xa phu",6)		-- xa phu cong
	--AddNpcEx1({237},1,nil,ID_MAP_CTC_HDPT,1639*32,3191*32,"",FILE_XAPHU,"Xa phu",6)		-- xa phu thu

	--nNpcIdx = AddNpcEx1({373},1,nil,78,1574*32,3233*32,"",FILE_CTHANHQUAN,"C«ng thµnh quan",6) SetNpcValue(nNpcIdx,1)	-- cong thanh quan tuong duong
	--nNpcIdx = AddNpcEx1({373},1,nil,1,1591*32,3200*32,"",FILE_CTHANHQUAN,"C«ng thµnh quan",6) SetNpcValue(nNpcIdx,2)	-- cong thanh quan phuong tuong

	--AddNpcEx1({51} ,1,nil,78,1593*32,3206*32 ,"",FILE_NHAMON,"Quan sai",6)
	--AddNpcEx1({51} ,1,nil,78,1600*32,3214*32 ,"",FILE_NHAMON,"Quan sai",6)	
	
	--AddNpcEx1({1645},1,nil,80,1773*32,3017*32,"",FILE_NHAMON,"Quan sai",6)
	--AddNpcEx1({1645},1,nil,80,1779*32,3023*32,"",FILE_NHAMON,"Quan sai",6)
	
	--AddNpcEx1({1645},1,nil,11,3147*32,5048*32,"",FILE_NHAMON,"Quan sai",6);	-- thanh do
	--AddNpcEx1({1645},1,nil,11,3152*32,5053*32,"",FILE_NHAMON,"Quan sai",6);	-- thanh do

	--AddNpcEx1({1645},1,nil,37,1787*32,3031*32,"",FILE_NHAMON,"Quan sai",6);	-- bien kinh
	--AddNpcEx1({1645},1,nil,37,1792*32,3035*32,"",FILE_NHAMON,"Quan sai",6);	-- bien kinh
	
	--AddNpcEx1({1645},1,nil,1,1575*32,3184*32,"",FILE_NHAMON,"Quan sai",6) --phuong tuong
	--AddNpcEx1({1645},1,nil,1,1579*32,3180*32,"",FILE_NHAMON,"Quan sai",6) --phuong tuong
end

function addobjcongthanh()
end

function addtrapcongthanh()
	--AddTrapEx1(ID_MAP_CTC,1569,3283,30,"\\script\\maps\\congthanhchien\\trap\\thu_congchinh.lua")
	--AddTrapEx1(ID_MAP_CTC,1539,3372,15,"\\script\\maps\\congthanhchien\\trap\\thu_congphu_1.lua")
	--AddTrapEx1(ID_MAP_CTC,1670,3256,17,"\\script\\maps\\congthanhchien\\trap\\thu_congphu_2.lua")
	
	--AddTrapEx1(ID_MAP_CTC,1793,3502,30,"\\script\\maps\\congthanhchien\\trap\\cong_congchinh.lua")
	--AddTrapEx1(ID_MAP_CTC,1730,3544,15,"\\script\\maps\\congthanhchien\\trap\\cong_congphu_1.lua")
	--AddTrapEx1(ID_MAP_CTC,1860,3416,17,"\\script\\maps\\congthanhchien\\trap\\cong_congphu_2.lua")
	
	--AddTrapEx2(ID_MAP_CTC,1745,3507,5,"\\script\\maps\\congthanhchien\\trap\\matdao_1.lua")
	--AddTrapEx2(ID_MAP_CTC,1837,3403,5,"\\script\\maps\\congthanhchien\\trap\\matdao_2.lua")
	--AddTrapEx2(ID_MAP_CTC,1587,3469,5,"\\script\\maps\\congthanhchien\\trap\\matdao_3.lua")
	
	-- [GOHECU 30/08] bay he Cong Thanh CU tren ban do 221 - dung ban do Cong Thanh JX2 dang chay; doc GetMissionV cua mission 2 (khong con mo) nen luon = 0 va SetPos day nguoi choi lui giua tran
	-- AddTrapEx1(ID_MAP_CTC,1472,3085,30,"\\script\\maps\\congthanhchien\\trap\\chancong_1.lua") -- lap duong, tinh duong
	-- [GOHECU 30/08] bay he Cong Thanh CU tren ban do 221 - dung ban do Cong Thanh JX2 dang chay; doc GetMissionV cua mission 2 (khong con mo) nen luon = 0 va SetPos day nguoi choi lui giua tran
	-- AddTrapEx1(ID_MAP_CTC,1535,3029,30,"\\script\\maps\\congthanhchien\\trap\\chancong_2.lua") -- dinh xuyen
	-- [GOHECU 30/08] bay he Cong Thanh CU tren ban do 221 - dung ban do Cong Thanh JX2 dang chay; doc GetMissionV cua mission 2 (khong con mo) nen luon = 0 va SetPos day nguoi choi lui giua tran
	-- AddTrapEx1(ID_MAP_CTC,1409,3149,30,"\\script\\maps\\congthanhchien\\trap\\chancong_3.lua") -- binh giang

	--AddObstacleEx1(ID_MAP_CTC,1472,3085,30, 1)
	--AddObstacleEx1(ID_MAP_CTC,1535,3029,30, 1)
	--AddObstacleEx1(ID_MAP_CTC,1409,3149,30, 1)
end

function UpdateMisionValueTopCTC()
	local nDesPlayerIdx = PlayerIndex
	local nDesPlayerData = PIdx2MSDIdx(MS_CTHANHCHIEN,nDesPlayerIdx)
	local nTotalAccum = GetPMParam(MS_CTHANHCHIEN,nDesPlayerData,6)--tong so tich luy
	local nTongAcc = GetMissionV(M_TICHLUYA)
	local nKimAcc  = GetMissionV(M_TICHLUYB)
	updatetopten(nTotalAccum, nDesPlayerData)
	local nCount = GetMSPlayerCount(MS_CTHANHCHIEN)
	for i=1,nCount do
		PlayerIndex = MSDIdx2PIdx(MS_CTHANHCHIEN, i)
		if(GetPMParam(MS_CTHANHCHIEN, i, 0) == 1) then --dang online
			--UpdateRankCTC()
			local nPlayerPoint = GetPMParam(MS_CTHANHCHIEN,PlayerIndex,6)
			--UpdateBattleBox(MS_CTHANHCHIEN, nTongAcc, nKimAcc, nPlayerPoint, 6)--6 lµ type ®Ó hiÓn thÞ trong client --Point c¸ nh©n vµ Point T and Point K
		end		
	end
	PlayerIndex = nDesPlayerIdx--tr¶ l¹i PlayerIndex sau khi Update toµn bé c¸c Player trong Mission Tèng Kim
end

function UpdateRankCTC()
	local nRankAcc,nRankData
	for i=1,10 do
		nRankAcc = GetMissionV(i)
		Msg2Player("GetMissionV(i)="..GetMissionV(i))
		if(nRankAcc > 0) then
			nRankData = GetMissionV(i+10)
			if(GetPMParam(MS_CTHANHCHIEN, nRankData, 0) == 1) then
				--UpdateBattleBox(MS_CTHANHCHIEN,5,nRankData,i)
			end
		end
	end	
end

function CongThanhPheThu()
	NewWorld(221,1285,2934) SetFightState(0)
end

function CongThanhPheCong()
	NewWorld(221,1631,3274) SetFightState(0)
end

function HauPhuongPheThu()
	NewWorld(222, 1614, 3172) SetFightState(0)
end

function HauPhuongPheCong()
	NewWorld(223, 1614, 3172) SetFightState(0)
end

function LapDuongTru()
	NewWorld(221, DATA_TRUTHANH[1][2],DATA_TRUTHANH[1][3]) SetFightState(1)
end

function DinhXuyenTru()
	NewWorld(221, DATA_TRUTHANH[2][2],DATA_TRUTHANH[2][3]) SetFightState(1)
end

function BinhGiangTru()
	NewWorld(221, DATA_TRUTHANH[3][2],DATA_TRUTHANH[3][3]) SetFightState(1)
end

function AdminTestCTC()
	SayEx({ADMIN_SUPPORT_TEXT, 
	"HËu Ph­¬ng phe Thñ/HauPhuongPheThu",
	"HËu Ph­¬ng phe C«ng/HauPhuongPheCong",
	"C«ng thµnh chiÕn phe Thñ/CongThanhPheThu",
	"C«ng thµnh chiÕn phe C«ng/CongThanhPheCong",
	"LËp D­¬ng Trô/LapDuongTru",	
	"§Þnh Xuyªn Trô/DinhXuyenTru",
	"B×nh Giang Trô/BinhGiangTru",
	NOW_END_SAY})
end

function citywar_awardplayer(nTotalAccum)
	if(nTotalAccum < 1000) then
		return
	end
	-- [MAIL 03/09 D9] thuong ket thuc Cong Thanh gui qua thu
	Include("\\script\\mail\\mailmanager.lua")
	MailManager_SendRewardTemplet("congthanh", nil, "Th­ëng C«ng Thµnh ChiÕn", "§¹i hiÖp ®¹t trªn 1000 ®iÓm c«ng tr¹ng, phÇn th­ëng ®Ýnh kÌm trong th­.<enter>Tr©n träng", {{tbProp = {6, 1, 1075, 1, 1, 0}, nCount = 1}}, 30)
	local szLog = format("B¹n nhËn ®­îc phÇn th­ëng C«ng thµnh chiÕn——%s","LÔ hép qu¶ huy hoµng")
	Msg2Player(szLog)
	logHoatDong(format("[citywar_award_Player point up 1000]\t%s\tName:%s\tAccount:%s\t%s",GetLocalDate("%Y-%m-%d_%H:%M"),GetName(),GetAccount(),szLog))
end

function UpdatePlayerCountMSCTC() --update player dang tham gia
	local oldPlayerIdx = PlayerIndex
	local nPTong = GetMSPlayerCount(MS_CTHANHCHIEN,1)
	local nPKim  = GetMSPlayerCount(MS_CTHANHCHIEN,2)
	for i=1,GetMSPlayerCount(MS_CTHANHCHIEN) do
		if(GetPMParam(MS_CTHANHCHIEN, i, 0) == 1) then		
			PlayerIndex = MSDIdx2PIdx(MS_CTHANHCHIEN, i)
			--UpdateBattleBox(MS_CTHANHCHIEN,2,nPTong,nPKim)
		end
	end
	PlayerIndex = oldPlayerIdx
end

function CTCCheckWinEnd()
	local nThuCount = 0
	local nCongCount = 0
	local nTruLD = GetMissionV(M_LONGTRULD)
	local nTruDX = GetMissionV(M_LONGTRUDX)
	local nTruBG = GetMissionV(M_LONGTRUBG)
	if(nTruLD == 1) then
		nThuCount = nThuCount + 1
	else
		nCongCount = nCongCount + 1
	end
	if(nTruDX == 1) then
		nThuCount = nThuCount + 1
	else
		nCongCount = nCongCount + 1		
	end
	if(nTruBG == 1) then
		nThuCount = nThuCount + 1
	else
		nCongCount = nCongCount + 1		
	end
	if(nThuCount > nCongCount) then
		return 1 --phe thñ win
	else
		return 2 --phe c«ng win
	end
end

function CTCCheckIsCongWin()
	local nCount = 0
	local nTruLD = GetMissionV(M_LONGTRULD)
	local nTruDX = GetMissionV(M_LONGTRUDX)
	local nTruBG = GetMissionV(M_LONGTRUBG)
	if(2 == nTruLD) then
		nCount = nCount + 1
	end
	if(2 == nTruDX) then
		nCount = nCount + 1
	end
	if(2 == nTruBG) then
		nCount = nCount + 1
	end
	if(nCount == 3) then
		StopMissionTimer(MS_CTHANHCHIEN,2)--t¾t hÑn giê sè 2 kÕt thóc trËn
		local nCount = GetMSPlayerCount(MS_CTHANHCHIEN)
		local nTotalAccum = 0
		for i=1,nCount do
			if(GetPMParam(MS_CTHANHCHIEN, i, 0) == 1) then --dang online thi tinh diem / ben trong map 69
				PlayerIndex = MSDIdx2PIdx(MS_CTHANHCHIEN, i)
				local nPhe = GetMSIdxGroup(MS_CTHANHCHIEN, i)	
				SetPKMode(0,0)--phuc hoi pk tu do
				SetFightState(0)--phi chien dau
				SetPunish(0)
				SetLogoutRV(0)
				SetCreateTeam(1)
				nTotalAccum = GetPMParam(MS_CTHANHCHIEN, i , 6)
				local nBattleRank = GetPMParam(MS_CTHANHCHIEN, i, 11)
				RemoveRankBattle()
				SetTempRevPos(53, 51904, 102048)--ket thuc thiet lap diem hoi sinh o Ba Lang Huyen
				if(nPlayerCamp == nPhe) then --nam trong phe chien thang, thuong them tich luy
					local nAccum = TICH_LUY_THUONG_CT * nBattleRank
					nTotalAccum = nTotalAccum + nAccum
				end
				SetTask(T_TICHLUYCT,GetTask(T_TICHLUYCT)+nTotalAccum)
				if(nTotalAccum > 0) then
					Talk(1,"","Xin chóc mõng! <sex> nhËn ®­îc phÇn th­ëng <color=yellow>"..nTotalAccum.."<color> ®iÓm c«ng tr¹ng.")
				end
				citywar_awardplayer(nTotalAccum)
				SetCurCamp(GetCamp())--phuc hoi phe ban dau	
				SetDeathScript("") --set script khi chet
				NewWorld(TAB_PHE_CONGTHANH[nPhe][4][1],TAB_PHE_CONGTHANH[nPhe][4][2],TAB_PHE_CONGTHANH[nPhe][4][3])--trë vÒ hËu ph­¬ng cña phe m×nh
			end
		end
		for i=11,20 do--thuong top 10 CTC
			local nPlayerDataIdx = GetMissionV(i)
			local nTopPlayerIdx = MSDIdx2PIdx(MS_CTHANHCHIEN, nPlayerDataIdx)
			if (nTopPlayerIdx > 0) then
				PlayerIndex = nTopPlayerIdx
				AddOwnExp(CITYWAR_TOP10_EXP)
				local szLog = format("%d ®iÓm kinh nghiÖm", CITYWAR_TOP10_EXP)
				Msg2Player(szLog)
				logHoatDong(format("[citywar_award_top10]\t%s\tName:%s\tAccount:%s\t%s\t\n",GetLocalDate("%Y-%m-%d_%H:%M"),GetName(),GetAccount(),szLog))
			end
		end	
		local msg = format("Phe tÊn c«ng %s ®· chiÕm gi÷ hoµn toµn 3 long trô giµnh th¾ng lîi! NhËn ®­îc quyÒn thèng trÞ %s. ", NAME_BANGCONG, CITYWAR_NAME)
		AddGlobalNews(msg)
		Msg2SubWorld(msg)
		logHoatDong(msg)
		for i=1,20 do
			SetMission(i,0)
		end	
		CloseMission(MS_CTHANHCHIEN)	--dong mission		
	end
end

function WriteInfoTongWinData(nTongID,nTongName,nTongMasterName,nCountAwd, nThue)
	BANGHOI_DANHSACH[1] = {nTongID,nTongName,nTongMasterName,nCountAwd, nThue}
	BANGHOI_DANHSACH[2] = {0,"0","0",0, 0}
	local tbDataTongNew = TaoBang(BANGHOI_DANHSACH,"BANGHOI_DANHSACH","")
	SaveData("script/tinhnang/congthanhchien/danhsach_bang.lua",tbDataTongNew)
	logHoatDong("WriteInfoTongWinData §· cËp nhËt d÷ liÖu Phe Thñ Thµnh vµ xo¸ phe c«ng thµnh.")
end

function ClearTong2Data()
	BANGHOI_DANHSACH[1][4] = 0 --set th­ëng phe thñ 0
	BANGHOI_DANHSACH[2] = {0,"0","0",0,0} --xo¸ phe c«ng khái danh s¸ch
	local tbDataTongNew = TaoBang(BANGHOI_DANHSACH,"BANGHOI_DANHSACH","")
	SaveData("script/tinhnang/congthanhchien/danhsach_bang.lua",tbDataTongNew)
	logHoatDong("ClearTong2Data §· cËp nhËt d÷ liÖu xo¸ phe c«ng thµnh.")	
end

function CTCGetTruChiemGiu(nCamp)
	local nCount = 0
	local nTruLD = GetMissionV(M_LONGTRULD)
	local nTruDX = GetMissionV(M_LONGTRUDX)
	local nTruBG = GetMissionV(M_LONGTRUBG)	
	if(nCamp == nTruLD) then
		nCount = nCount + 1
	end
	if(nCamp == nTruDX) then
		nCount = nCount + 1
	end
	if(nCamp == nTruBG) then
		nCount = nCount + 1
	end
	return nCount
end

function updatetopten(nTotalAccum,nDesPlayerData)
	local i
	local nRankAcc = 0
	local nData = 0
	--tim xem co trong bang chua?
	for i=11,20 do
		if(nDesPlayerData == GetMissionV(i)) then
			nData = i
			break	--[LUA54] truoc la 'i = 21': Lua 4 thoat vong, 5.4 gan khong tac dung
		end
	end
	--check diem tich luy co kha nang vao bang ko? va vi tri thu may?
	for i=1,10 do
		if(nTotalAccum > GetMissionV(i)) then
			nRankAcc = i
			break	--[LUA54] truoc la 'i = 11': Lua 4 thoat vong, 5.4 gan khong tac dung
		end
		if(nData == i+10 and nRankAcc < 0) then
			nRankAcc = i
			break	--[LUA54] truoc la 'i = 11': Lua 4 thoat vong, 5.4 gan khong tac dung
		end
	end
	
	if(nRankAcc <= 0) then --neu ko du diem de vao bang thi ngung
		return 0 
	end
	--du diem vao bang~ thi` sap xep lai bang~
	if(nRankAcc == 10 or nData-10 == nRankAcc) then	--cuoi' bang~ hoac trung vi tri'
		SetMission(nRankAcc,nTotalAccum)	--chen data moi vao
		SetMission(nRankAcc+10,nDesPlayerData)
		return 1
	 end

	local RankTab = {}
	local DataTab = {}
	local count = 0;
	--sao luu data cu~
	if(nData == 0) then
		for i=nRankAcc,9 do
			count = count + 1
			RankTab[count] = GetMissionV(i)
			DataTab[count] = GetMissionV(i+10)
		end
	else
		for i=nRankAcc,nData-11 do		-- 5 15 4
			count = count + 1
			RankTab[count] = GetMissionV(i)
			DataTab[count] = GetMissionV(i+10)
		end
		for i=nData-9,10 do			-- 15 5 10
			count = count + 1
			RankTab[count] = GetMissionV(i)
			DataTab[count] = GetMissionV(i+10)
		end
	end
	SetMission(nRankAcc,nTotalAccum)	--chen data moi vao
	SetMission(nRankAcc+10,nDesPlayerData)	--chen data moi vao
	for i=nRankAcc+1,10 do
		SetMission(i,RankTab[i-nRankAcc])	--di chuyen data cu~ xuong 1 bac
		SetMission(i+10,DataTab[i-nRankAcc])	--di chuyen data cu~ xuong 1 bac
	end
	for i=0,9 do
		Msg2Player("Hang "..(i+1).." Acc "..GetMissionV(i).." Data "..GetMissionV(i+10))
	end
	return 1
end

-- 0: online
-- 1: chien dau
-- 2: giet dich
-- 3: giet npc
-- 4: tu vong
-- 5: lien tram cao nhat
-- 6: tich luy tam
-- 7: lien tram hien tai
-- 8: dang giu co
-- 9: pha tru/ pha cong
-- 10: xep hang
-- 11: quan ham