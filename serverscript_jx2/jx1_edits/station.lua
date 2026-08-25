--Author: Fong Ki?u
--Date: 11/07/2021
--Function: Xa Phu

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\log_game\\save_log.lua")
Include("\\script\\task\\newtask\\map_index.lua") -- [DA TAU 17/08] tl_getMapInfo cho he tasklink moi
Include("\\script\\tinhnang\\datau\\lib_datau.lua")
Include("\\script\\event\\kiemmonquan\\lib_kmq.lua")
Include("\\script\\missions\\citywar_global\\station_ctc.lua")
Include("\\script\\task\\tollgate\\messenger\\wagoner.lua")	-- [TIN SU 21/08] messenger_wagoner (Linux station.lua muc 9)	-- DOT E (E5): GoCityWar/Attack/Defend

STATION_ARRAY = {
	{1 ,1  ,10072},
	{2 ,11 ,15647},
	{3 ,162,15648},
	{4 ,37 ,10072},
	{5 ,78 ,15646},
	{6 ,80 ,15649},
	{7 ,176,10072},
	{8 ,20 ,10072},
	{9 ,121,10072},
	{10,53 ,15652},
	{11,54 ,10072},
	{12,174,15650},
	{13,101,10072},
	{14,99 ,10072},
	{15,100,10072},
	{16,153,15651}
}

function WayPointFun()
	p1 = GetWayPoint(1) --®iÓm ®· ®i qua 1
	p2 = GetWayPoint(2) --®iÓm ®· ®i qua 2
	p3 = GetWayPoint(3) --®iÓm ®· ®i qua 3
	HaveBin={}
	i = 1
	if (p1 ~= 0)  then
		name = GetWayPointName(p1)
		name = name .."/SelWayPoint"
		HaveBin[i]= name
		i = i + 1
	end
	if (p2 ~= 0)  then
		name = GetWayPointName(p2)
		name = name .."/SelWayPoint"
		HaveBin[i]= name
		i = i + 1
	end
	if (p3 ~= 0)  then
		name = GetWayPointName(p3)
		name = name .."/SelWayPoint"
		HaveBin[i]= name
		i = i + 1
	end
	HaveBin[i] = "Kh«ng muèn ®i ®©u c¶/OnCancel"
	if (i == 1) then 
		Say("ThËt xin lçi ta chØ c? thÓ ®­a <sex> tíi n¬i <sex> ®· ®i qua",0)
	else
		Say("Mêi lùa chän", i, HaveBin)
	end
end

function WharfFun()
	Count = GetWharfCount(CurWharf)
	Num = Count
	WharfB = {}
	nValidCount = 1
	for i = 1, Num do
		WharfId = GetWharf(CurWharf, i)
		if (WharfId ~= 0) then
			nPrice = GetWharfPrice(CurWharf, WharfId);
			WharfB[nValidCount] = GetWharfName(WharfId) .. "[" .. nPrice .. " l­îng]" .. "/SelWharf"
			nValidCount = nValidCount + 1
		end
	end
	WharfB[nValidCount] = "OnCancel/OnCancel"
	if (nValidCount == 1) then 
		Say(13964 , 0)
	else
		Say(13965 , nValidCount, WharfB)
	end
end

function SelWharf(nSel)
	nWharfId = GetWharf( CurWharf, nSel + 1)
	nPrice = 0
	if (nWharfId ~= 0) then 
		nPrice = GetWharfPrice(CurWharf, nWharfId)
	else
		return
	end
	if (PrePay(nPrice) == 1) then
		Msg2Player("GetWharfName "..GetWharfName(nWharfId))
		nW , nX, nY = GetWharfPos(nWharfId)
		nResult = NewWorld(nW, nX , nY )
		if (nResult == 2) then
			Msg2Player("nResult == 2 ...")
		end
		if (nResult == 0) then
			Msg2Player("nResult == 0")
		end
	else
		Say(13966 , 0)
	end
end

function GetCurStation()
	local w,x,y = GetWorldPos()
	for i=1, getn(STATION_ARRAY) do
		--Msg2Player(format("w: %d STATION_ARRAY[i][2]:%d", w, STATION_ARRAY[i][2]))
		if(w == STATION_ARRAY[i][2]) then --n?u t×m thÊy thay ®æi CurStation
			CurStation = i
			--Msg2Player("Change CurStation: "..CurStation)
		end
	end
end

function StationFun()
	GetCurStation()--míi thªm vµo x¸c ®?nh CurStation theo map ®ang ®øng
	for j = 1, 15 do
		AddStation(j)
	end
	Count = GetStationCount()
	Num = Count
	if (Count > 6) then 
		--Num = 6
	end
	StationB = {}
	nValidCount = 1
	for i = 1, Num do
	StationId = GetStation(i, CurStation)
		if (StationId ~= 0) then
			nPrice = GetPrice2Station(CurStation, StationId)
			StationB[nValidCount] = GetStationName(StationId) .. "[" .. nPrice .. " l­îng]" .. "/SelStation"
			nValidCount = nValidCount + 1
		end
	end
	StationB[nValidCount] = "Kh«ng ®i/OnCancel"
	if (nValidCount == 1) then 
		Say("ThËt xin lçi ta chØ c? thÓ ®­a <sex> tíi n¬i <sex> ®· ®i qua", 0)
	else
		Say("Xin mêi chän ®?a ®iÓm", nValidCount, StationB)
	end
end

function SelStation(nSel)
	Count = GetStationCount()
	nStation = GetStation(nSel + 1, CurStation)
	nPrice = GetPrice2Station(CurStation, nStation)
	if (PrePay(nPrice) == 1) then
		nW , nX, nY = GetStationPos(nStation)
		nResult = NewWorld(nW, nX , nY )
		Msg2Player("Ngåi yªn chóng ta ®i: "..GetStationName(nStation))
		if (nResult == 2) then
			--Msg2Player("(nResult  = 2 ...")
		end
		if (nResult == 0) then
			--Msg2Player("§­êng ®i ph?a tr­íc kh«ng th«ng")
		end
	else
		Say("ThËt ng¹i kh«ng c? ti?n th× kh«ng thÓ ®i ®©u ®­îc", 0)
	end
end

function TownPortalFun()
	ReturnFromPortal()
end


function SelWayPoint(nSel)
	local nWayPoint = GetWayPoint(nSel + 1);
	if (nWayPoint ~= 0) then
	local nW, nX, nY = GetWayPointPos(nWayPoint)
	nFightState = GetWayPointFight(nWayPoint)
	nResult = NewWorld(nW, nX, nY)
	SetFightState(1) 		
	SetProtectTime(18*3)
	AddSkillState(963, 1, 0, 18*3)
	--if (nResult == 1) then
	--SetFightState(nFightState)
--	end
	end
end;
STRTAYDIEMTIEMNANG 	= "TÈy ®iÓm ti?m n¨ng cÇn 6 Tinh Hång B¶o Th¹ch"
STRTAYKYNANG					= "TÈy ®iÓm kü n¨ng cÇn 1 bé Thñy Tinh"

function didaotaytuy()
	local tbOption = {
		"<color=wood>Xa phu:<color> Xin chµo <color=red>"..GetName().."<color> <sex> c? thÓ chän ph­¬ng thøc tÈy tuû bªn d­íi nµy. Lùa chän cÈn thËn nhÇm lÉn ta kh«ng gióp ®­îc ®©u nh?! \n\n",
		""..STRTAYDIEMTIEMNANG.."/okaytaytuy(1)",
		""..STRTAYKYNANG.."/okaytaytuy(2)",
		"TÈy ®iÓm kü n¨ng vµ ®iÓm ti?m n¨ng/tayall",
	}
	if(GetTask(T_TAYTUYFREE) == 0) then
		tinsert(tbOption, "Sö dông lÇn tÈy tuû miÔn ph?./okaytaytuy(3)")
	end
	tinsert(tbOption, "K?t thóc ®èi tho¹i./no")
	SayEx(tbOption)
end

function okaytaytuy(nType)
	if(nType == 3) then
		if(GetTask(T_TAYTUYFREE) == 0) then
			SetTask(T_TAYTUYFREE, GetTask(T_TAYTUYFREE)+1)
			SetTask(T_TAYTUY_TYPE,CSP_CTYPE_ALL)--tÈy c¶ 2 lo¹i ®iÓm
			NewWorld(242, 1613, 3197)
			SetFightState(0)
			SetRevPos(1)--l­u r­¬ng region 1 map tÈy tuû
			local mgsLog = format("%s ®· dïng lÇn tÈy tuû miÔn ph? ®Ó lªn ®¶o.",GetName())
			logHoatDong(mgsLog) Msg2Player(mgsLog)
			return
		else
			Talk(1,"","<sex> ®· h?t sè lÇn tÈy tuû miÔn ph?.")
		end		
	end
	if(nType == 1) then
		local nCount = 6
		if (GetItemCount(0,4,353,0,-1,-1,pos_equiproom) < nCount) then -- tinh hong bao thach
			Talk(1,"","<sex> kh«ng c? ®ñ "..nCount.." Tinh Hång B¶o Th¹ch.")
			return 
		end
		local nResDel = DelItem(0,nLine,4,353,0,-1,-1,ROOME,nCount) -- tinh hong bao thach
		if(nResDel >= nCount) then
			NewWorld(242, 1613, 3197) 
			SetFightState(0)
			SetRevPos(1)--l­u r­¬ng region 1 map tÈy tuû
			SetTask(T_TAYTUY_TYPE,CSP_CTYPE_PROP)--tÈy ®iÓm ti?m n¨ng
			local mgsLog = format("%s ®· dïng "..nCount.." viªn tinh hång b¶o th¹ch ®Ó lªn ®¶o.",GetName())
			logHoatDong(mgsLog) Msg2Player(mgsLog)
		end		
	end
	if(nType == 2) then
		local nCount = 1
		if (GetItemCount(0,4,238,0,-1,-1,pos_equiproom) < nCount) then -- lam thuû tinh
			Talk(1,"","<sex> kh«ng c? ®ñ "..nCount.." Lam Thuû Tinh.")
			return 
		end
		if (GetItemCount(0,4,239,0,-1,-1,pos_equiproom) < nCount) then -- tö thuû tinh
			Talk(1,"","<sex> kh«ng c? ®ñ "..nCount.." Tö Thuû Tinh.")
			return 
		end
		if (GetItemCount(0,4,240,0,-1,-1,pos_equiproom) < nCount) then -- lôc thuû tinh
			Talk(1,"","<sex> kh«ng c? ®ñ "..nCount.." Lôc Thuû Tinh.")
			return 
		end
		local nResDel1 = DelItem(0,nLine,4,238,0,-1,-1,ROOME,nCount) -- lam thuû tinh
		local nResDel2 = DelItem(0,nLine,4,239,0,-1,-1,ROOME,nCount) -- tö thuû tinh
		local nResDel3 = DelItem(0,nLine,4,240,0,-1,-1,ROOME,nCount) -- lôc thuû tinh
		if(nResDel1 >= 1 and nResDel2>=1 and nResDel3>=1) then
			NewWorld(242, 1613, 3197) 
			SetFightState(0)
			SetRevPos(1)--l­u r­¬ng region 1 map tÈy tuû
			SetTask(T_TAYTUY_TYPE,CSP_CTYPE_SKILL)--tÈy ®iÓm kü n¨ng
			local mgsLog = format("%s ®· dïng 3 viªn thuû tinh ®Ó lªn ®¶o.",GetName())
			logHoatDong(mgsLog) Msg2Player(mgsLog)			
		end
	end
end

function tayall()
	local nCount = 1
	local nCount2 = 6
	if (GetItemCount(0,4,238,0,-1,-1,pos_equiproom) < nCount) then -- lam thuû tinh
		Talk(1,"","<sex> kh«ng c? ®ñ "..nCount.." Lam Thuû Tinh.")
		return 
	end
	if (GetItemCount(0,4,239,0,-1,-1,pos_equiproom) < nCount) then -- tö thuû tinh
		Talk(1,"","<sex> kh«ng c? ®ñ "..nCount.." Tö Thuû Tinh.")
		return 
	end
	if (GetItemCount(0,4,240,0,-1,-1,pos_equiproom) < nCount) then -- lôc thuû tinh
		Talk(1,"","<sex> kh«ng c? ®ñ "..nCount.." Lôc Thuû Tinh.")
		return 
	end
	if (GetItemCount(0,4,353,0,-1,-1,pos_equiproom) < nCount2) then -- tinh hong bao thach
		Talk(1,"","<sex> kh«ng c? ®ñ "..nCount2.." Tinh Hång B¶o Th¹ch.")
		return 
	end	
	local nResDel1 = DelItem(0,nLine,4,238,0,-1,-1,ROOME,nCount) -- lam thuû tinh
	local nResDel2 = DelItem(0,nLine,4,239,0,-1,-1,ROOME,nCount) -- tö thuû tinh
	local nResDel3 = DelItem(0,nLine,4,240,0,-1,-1,ROOME,nCount) -- lôc thuû tinh
	local nResDel4 = DelItem(0,nLine,4,353,0,-1,-1,ROOME,nCount2) -- tinh hång b¶o th¹ch
	if(nResDel1 >= 1 and nResDel2>=1 and nResDel3>=1 and nResDel4 >=1) then
		NewWorld(242,1613,3197) SetFightState(0)
		SetRevPos(1)--l­u r­¬ng region 1 map tÈy tuû
		SetTask(T_TAYTUY_TYPE,CSP_CTYPE_ALL)--tÈy ®iÓm kü n¨ng
		local mgsLog = format("%s ®· dïng 3 viªn thuû tinh vµ 6 tinh hång b¶o th¹ch ®Ó lªn ®¶o.",GetName())
		logHoatDong(mgsLog) Msg2Player(mgsLog)			
	end	
end

function go_HSBattle()
	Msg2Player("Ngåi yªn chóng ta ®i Hoa S¬n")
	NewWorld(2,2605,3592)
	SetFightState(1)
	SetProtectTime(18*3)
	AddSkillState(963, 1, 0, 18*3)
end

function wuda()
	Say("§?n m«n ph¸i",6,"Vâ ®ang/wud","Thiªn v­¬ng/tiw","Nga my/emi","§­êng m«n/tag","Thiªn nhÉn/tir","Kh«ng ®i/canc")
end

function wud()
	Say("§i vâ ®ang")
	NewWorld(81, 1574, 3224)
end

function tiw()
	Say("§i thiªn v­¬ng")
	NewWorld(59, 1425, 3472)
end

function emi()
	Say("§i nga my")
	NewWorld(13, 1898, 4978)
end

function tag()
	Say("§i ®­êng m«n")
	NewWorld(25, 3982, 5235)
end

function tir()
	Say("§i thiªn nhÉn")
	NewWorld(49, 1644, 3215)
end

function Cancel()
end

function  OnCancel()
end

function godatau()
	-- [DA TAU 17/08/2026] he tasklink moi (ban goc Linux): loai 4 luu map dich o task 1031.
	-- Dung dung du lieu + hanh vi cua tl_moveToTaskMap_Confirm (map_index.lua):
	-- NewWorld toi toa do TL_MAPTRAPINDEX, SetFightState(1), that bai thi bao va thoi.
	if (GetTask(1021) == 4 and GetTask(1031) > 0) then
		local nMapIDMoi = GetTask(1031)
		local szTenMap, nXMoi, nYMoi = tl_getMapInfo(nMapIDMoi)
		if (szTenMap == 0 or szTenMap == nil or szTenMap == "") then
			Talk(1,"","Xin lçi! N¬i quý kh¸ch muèn ®Õn qu¸ hiÓm trë! Hay lµ tù ®i vËy!")
			return
		end
		local nKetQua = NewWorld(nMapIDMoi, nXMoi, nYMoi)
		SetFightState(1)
		if (nKetQua == 0) then
			SetFightState(0)
			Msg2Player("Xin lçi! HiÖn vÉn ch­a chuyÓn ®i!")
			return
		end
		Msg2Player("Ngåi yªn chóng ta ®i "..szTenMap)
		return
	end
	-- nhanh he CU (Fong Kieu 2021) giu nguyen cho nhan vat con du lieu cu
	if (GetTask(T_TIMDOCHI) >= 1 or GetTask(T_TIMMATCHI) >= 1 or GetTask(T_DanhQuai) >= 1) then
		local a = GetTask(T_TIMDOCHI)
		local b = GetTask(T_TIMMATCHI)
		local c = GetTask(T_DanhQuai)
		local nMapID = 53
		local nX = 1342
		local nY = 3346
		local nMapName = "Ba L¨ng HuyÖn"
		if(a > 0) then
			nMapID = DIADOCHI[a][4]
			nX = DIADOCHI[a][5]
			nY = DIADOCHI[a][6]
			nMapName = DIADOCHI[a][2]
		elseif(b > 0) then
			nMapID = MATCHI[b][4]
			nX = MATCHI[b][5]
			nY = MATCHI[b][6]
			nMapName = MATCHI[b][2]
		elseif(c > 0) then
			nMapID = DANHQUAI[c][4]
			nX = DANHQUAI[c][5]
			nY = DANHQUAI[c][6]			
			nMapName = DANHQUAI[c][2]
		end
		NewWorld(nMapID, nX, nY)
		SetFightState(1)
		--Msg2Player(format("nMapID=%d, nX=%d, nY=%d",nMapID,nX,nY))
		Msg2Player("Ngåi yªn chóng ta ®i ".. nMapName)
	else
		Talk(1,"","Kh«ng nhËn nhiÖm vô mµ d¸m l?a ta µ!")
	end
end




-- ============================================================================
-- (19/08 dem - chu game) LEN BAN DO LUYEN CONG 20-90 tu Xa Phu.
-- Toa do CHEP NGUYEN VAN tu item Than Hanh Phu (script/item/ib/shenxingfu.lua
-- TRAIN_ARRAY1/2; cac dong dang waypoint da tra bang settings/WayPoint.txt) -
-- diem dap CHINH THONG cua game, khong dung toa do tay nua.
-- Bot KPlayer di bo toi Xa Phu roi goi botlc_go(n) y het nguoi choi bam menu.
-- (20/08) BOT_LC chi con la MENU NGUOI CHOI. Bot dung bang BOT_BAI o duoi.
-- ============================================================================
BOT_LC = {
[1] = {
	"Kiem Cac Tay Nam [20]/sellc",
	"Vo Lang Son [20]/sellc",
	"Phuc Nguu Dong [30]/sellc",
	"Thuc Cuong Son [30]/sellc",
	"Phuc Nguu Tay [40]/sellc",
	"Hoang Ha Nguyen Dau [40]/sellc",
	"Luu Tien Dong [50]/sellc",
	"Oc Ba Dia Dao [50]/sellc",
	"Thien Tam Thap tang 3 [60]/sellc",
	"Hoanh Son Phai [60]/sellc",
	"Lam Du Quan [70]/sellc",
	"Lao Ho Dong [70]/sellc",
	"Chan nui Truong Bach [80]/sellc",
	"Sa Mac Dia Bieu [80]/sellc",
	"Truong Bach Son Nam [90]/sellc",
	"Truong Bach Son Bac [90]/sellc",
	"Khoa Lang Dong [90]/sellc",
	"Tien Cuc Dong [90]/sellc",
	"Can Vien Dong [90]/sellc",
	"Phong Lang Do [90]/sellc",
	"Mac Cao Quat [90]/sellc",
	"Ta khong muon di/no"
},
[2] = {
	{20,19,3102,3963},
	{20,70,1608,3230},
	{30,90,1651,3571},
	{30,92,1632,3290},
	{40,41,2078,2805},
	{40,122,1612,3323},
	{50,125,1809,3208},
	{50,163,1558,3199},
	{60,166,1649,3231},
	{60,56,1493,3530},
	{70,319,1630,3592},
	{70,123,1698,3374},
	{80,320,1146,3130},
	{80,224,1621,3214},
	{90,321,966,2296},
	{90,322,1582,3147},
	{90,75,1816,3009},
	{90,93,1526,3172},
	{90,124,1672,3420},
	{90,336,1112,3189},
	{90,340,1853,3446}
}
};

-- ============================================================================
-- [XAPHU 25/08] Menu luyen cong DU 39 map (nguon: BOT_BAI - da gop tu Than
-- Hanh Phu). Chia TRANG THEO MOC de khong vo tran 511B/goi thoai. Ten CO DAU
-- (TCVN3), index KHOP TUNG DONG voi BOT_BAI (bang do giu nguyen cho bot).
-- ============================================================================
XP_TEN = {
	"Hoa S¬n",	-- 1
	"KiÕm C¸c T©y Nam",
	"T©n Lang tÇng 1",
	"Vò L¨ng S¬n",
	"Vò Di S¬n",
	"Thæ PhØ §éng",	-- 6
	"Phôc Ng­u §«ng",
	"Thôc C­¬ng S¬n",
	"Thanh Thµnh S¬n",
	"Diªm Th­¬ng S¬n",
	"Phôc Ng­u T©y",	-- 11
	"Hoµng Hµ Nguyªn §Çu",
	"NghiÖt Long §éng",
	"Thiªn T©m Th¸p tÇng 1",
	"L­u Tiªn §éng",
	"¸c B¸ §Þa §¹o",	-- 16
	"T­¬ng D­¬ng MËt §¹o",
	"Hoµnh S¬n ph¸i",
	"Thiªn T©m Th¸p tÇng 3",
	"L©m Du Quan",
	"L·o Hæ §éng",	-- 21
	"T©n Lang tÇng 2",
	"Sa M¹c §Þa BiÓu",
	"Thanh Khª §éng",
	"Ch©n nói Tr­êng B¹ch",
	"L­¬ng Thñy §éng",	-- 26
	"H¾c Sa §éng",
	"Tr­êng B¹ch S¬n B¾c",
	"Tr­êng B¹ch S¬n Nam",
	"Kháa Lang §éng",
	"Sa M¹c Mª Cung 1",	-- 31
	"Sa M¹c Mª Cung 2",
	"Sa M¹c Mª Cung 3",
	"Phong L¨ng §é",
	"M¹c Cao QuËt",
	"D­îc V­¬ng §éng tÇng 4",	-- 36
	"TiÕn Cóc §éng",
	"C¸n Viªn §éng",
	"TuyÕt B¸o §éng tÇng 8",
}

function LuyenCongFun()
	if (GetLevel() < 20) then
		Say("Ch­a ®ñ 20 cÊp, h·y luyÖn ë Hoa S¬n tr­íc ®·.", 0)
		return
	end
	Say("Muèn tíi b¶n ®å luyÖn c«ng mèc nµo?", 6,
		"Mèc 20 - 30/#xp_moc(20,30)",
		"Mèc 40 - 50/#xp_moc(40,50)",
		"Mèc 60 - 70/#xp_moc(60,70)",
		"Mèc 80/#xp_moc(80,80)",
		"Mèc 90/#xp_moc(90,90)",
		"Kh«ng ®i/no")
end

function xp_moc(nMoc1, nMoc2)
	local tb = {}
	for i = 2, getn(BOT_BAI) do	-- bo dong 1 (Hoa Son - loi di rieng)
		if (BOT_BAI[i][1] >= nMoc1 and BOT_BAI[i][1] <= nMoc2) then
			tinsert(tb, XP_TEN[i].." ["..BOT_BAI[i][1].."]".."/#xp_go("..i..")")
		end
	end
	tinsert(tb, "Quay l¹i/LuyenCongFun")
	tinsert(tb, "Kh«ng ®i/no")
	Say("Muèn tíi b¶n ®å nµo? (sè trong ngoÆc lµ cÊp tèi thiÓu)", getn(tb), tb)
end

function xp_go(nIdx)
	if (nIdx < 2 or nIdx > getn(BOT_BAI)) then
		return
	end
	if (GetLevel() < BOT_BAI[nIdx][1]) then
		Talk(1,"","Ch­a ®ñ cÊp tíi b¶n ®å nµy, phÝa tr­íc nguy hiÓm.")
		return
	end
	if (NewWorld(BOT_BAI[nIdx][2], BOT_BAI[nIdx][3], BOT_BAI[nIdx][4])) then
		SetFightState(1)
		SetProtectTime(18*3)
		AddSkillState(963, 1, 0, 18*3)
	end
end

function sellc(nSel)
	botlc_go(nSel + 1)
end

-- ============================================================================
-- (20/08) BANG BAI LUYEN CUA BOT - 39 bai.
-- PHAI KHOP THU TU voi mang s_bai trong Sources/Core/Src/KPlayerBot.cpp:
--     BOT_BAI[i+1]  <=>  s_bai[i]      (macro PB_BAI_LUA)
-- Toa do chep nguyen van tu cac bang tab_lv20map .. tab_lv90map cua
-- script/item/ib/shenxingfu.lua (Than Hanh Phu - dung duong nguoi choi that
-- di), hop them cac map chi co trong BOT_LC o tren. Moc cap giu nguyen moc
-- chinh thong cua tung map: 20 co 3 bai, 30/40/50 co 4, 60/70 co 3, 80 co 4,
-- 90 co 13.
-- Bang nay RIENG cho bot, KHONG dua vao menu nguoi choi: 39 dong se lam vo
-- goi hop thoai (tran 511 byte).
-- ============================================================================
BOT_BAI = {
-- moc 10
	{10,2,2605,3592},	-- Hoa Son (chi giu chi so - bot di go_HSBattle)
-- moc 20
	{20,19,3102,3963},	-- Kiem Cac Tay Nam
	{20,7,2276,2825},	-- Tan Lang tang 1
	{20,70,1608,3230},	-- Vu Lang Son
-- moc 30
	{30,193,1938,2845},	-- Vu Di Son
	{30,170,1612,3187},	-- Tho Phi Dong
	{30,90,1651,3571},	-- Phuc Nguu Dong
	{30,92,1632,3290},	-- Thuc Cuong Son
-- moc 40
	{40,21,2622,4502},	-- Thanh Thanh Son
	{40,167,1575,3239},	-- Diem Thuong Son
	{40,41,2078,2805},	-- Phuc Nguu Tay
	{40,122,1612,3323},	-- Hoang Ha Nguyen Dau
-- moc 50
	{50,182,1777,2982},	-- Nghiet Long Dong
	{50,164,1611,3187},	-- Thien Tam Thap tang 1
	{50,125,1809,3208},	-- Luu Tien Dong
	{50,163,1558,3199},	-- Oc Ba Dia Dao
-- moc 60
	{60,79,1600,3206},	-- Tuong Duong Mat Dao
	{60,56,1516,3443},	-- Hoanh Son Phai
	{60,166,1649,3231},	-- Thien Tam Thap tang 3
-- moc 70
	{70,319,1630,3587},	-- Lam Du Quan
	{70,123,1702,3350},	-- Lao Ho Dong
	{70,206,1603,3215},	-- Tan Lang tang 2
-- moc 80
	{80,224,1622,3118},	-- Sa Mac Dia Bieu
	{80,198,1521,2947},	-- Thanh Khe Dong
	{80,320,1147,3123},	-- Chan nui Truong Bach
	{80,181,1425,2999},	-- Luong Thuy Dong
-- moc 90
	{90,875,1576,3177},	-- Hac Sa Dong
	{90,322,1589,3164},	-- Truong Bach Son Bac
	{90,321,967,2313},	-- Truong Bach Son Nam
	{90,75,1811,3012},	-- Khoa Lang Dong
	{90,225,1474,3275},	-- Sa Mac Me Cung 1
	{90,226,1560,3184},	-- Sa Mac Me Cung 2
	{90,227,1588,3237},	-- Sa Mac Me Cung 3
	{90,336,1124,3187},	-- Phong Lang Do
	{90,340,1845,3438},	-- Mac Cao Quat
	{90,144,1691,3020},	-- Duoc Vuong Dong tang 4
	{90,93,1529,3166},	-- Tien Cuc Dong
	{90,124,1675,3418},	-- Can Vien Dong
	{90,152,1672,3361},	-- Tuyet Bao Dong tang 8
}

-- Bot goi ham nay y het nguoi choi bam menu Xa Phu (cung khuon botlc_go).
function bot_bai_go(nIdx)
	if (nIdx < 1 or nIdx > getn(BOT_BAI)) then
		return
	end
	if (GetLevel() < BOT_BAI[nIdx][1]) then
		return
	end
	if (NewWorld(BOT_BAI[nIdx][2], BOT_BAI[nIdx][3], BOT_BAI[nIdx][4])) then
		SetFightState(1)
		SetProtectTime(18*3)
		AddSkillState(963, 1, 0, 18*3)
	end
end

function botlc_go(nIdx)
	if (nIdx < 1 or nIdx > getn(BOT_LC[2])) then
		return
	end
	if (GetLevel() < BOT_LC[2][nIdx][1]) then
		Talk(1,"","Chua du cap toi ban do nay, phia truoc nguy hiem.")
		return
	end
	if (NewWorld(BOT_LC[2][nIdx][2], BOT_LC[2][nIdx][3], BOT_LC[2][nIdx][4])) then
		SetFightState(1)
		SetProtectTime(18*3)
		AddSkillState(963, 1, 0, 18*3)
	end
end
