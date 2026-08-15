--Author: Fong Ki?u
--Date: 11/07/2021
--Function: Xa Phu

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\log_game\\save_log.lua")
Include("\\script\\tinhnang\\datau\\lib_datau.lua")
Include("\\script\\event\\kiemmonquan\\lib_kmq.lua")
Include("\\script\\missions\\citywar_global\\station_ctc.lua")	-- DOT E (E5): GoCityWar/Attack/Defend

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



