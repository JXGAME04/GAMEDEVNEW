-- Author: Fong KiÒu
-- Date: 28/11/2020
-- Chuc nang: Trèng bang héi

Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\event\\trongbanghoi\\lib.lua")

function main(NpcIndex)

	-- dofile("script/event/trongbanghoi/tungtung.lua")
	
	if (0 == GetCamp()) then
		Talk(1,"","<sex> ch­a gia nhËp m«n ph¸i, kh«ng thÓ tham gia. ")
		return
	end

	if (0 == GetFightState() or GetLife(0) <= 0) then
		Talk(1,"","Tr¹ng th¸i phi chiÕn ®Êu kh«ng thÓ ®¸nh trèng. ")
		return
	end	
	
	--local nTongID = GetTongInfo(0)
	--local nTongName = GetTongInfo(1)
	local nTongMaster = GetTongInfo(2)
	if(GetName() ~= nTongMaster) then
		Talk(1,"","<color=green>ChØ cã bang chñ míi cã ®ñ t­ c¸ch ®¸nh trèng. ")
		return
	end
	
	SetPKMode(1,0)--chuyen pk chien dau
	SetFightState(1)--chuyen kieu chien dau
	
	PaceBar("§ang ®¸nh trèng...", 23, "FnDanhTrongOkay("..NpcIndex..")")
	
end

function FnDanhTrongOkay(NpcIndex)

	if(GetFightState() == 0) then 
		Talk(1,"","§ang ë chÕ ®é phi chiÕn ®Êu kh«ng thÓ nhÆt")
		return
	end --tr¹ng th¸i kh«ng chiÕn ®Êu, kh«ng pk kh«ng nhËn ®­îc trèng
	if(GetPKState() == 0) then
		Talk(1,"","§ang ë chÕ ®é luyÖn c«ng kh«ng thÓ nhÆt")
		return 
	end 
	if(GetLife(0) <= 0) then 
		Talk(1,"","Sinh lùc kh«ng ®ñ kh«ng thÓ nhÆt")
		return 
	end 

	if(FindAroundNpc(GetNpcID(NpcIndex)) <= 0) then
		Talk(1,"","ThËt ®¸ng tiÕc trèng ®· bÞ ng­êi kh¸c lÊy mÊt. ")
		return
	end
	
	DelNpc(NpcIndex)
	
	AddItem(6,1,2318,1,0,0,0) --trèng kh¶i hoµn
	local randomtg = random(1,6)
	if(randomtg == 1) then
		AddEventItem(353) AddEventItem(353) AddEventItem(353)
		logHoatDong("Ng­êi ch¬i <color=green>"..GetName().."<color> ®¸nh thñng trèng nhËn ®­îc 3 THBT")
	elseif(randomtg == 2) then
		AddEventItem(238) AddEventItem(239) AddEventItem(240)
		logHoatDong("Ng­êi ch¬i <color=green>"..GetName().."<color> ®¸nh thñng trèng nhËn ®­îc 3 Thuû Tinh")	
	elseif(randomtg == 3) then
		Earn(10000000)
		logHoatDong("Ng­êi ch¬i <color=green>"..GetName().."<color> ®¸nh thñng trèng nhËn ®­îc 1000000 v¹n")	
	elseif(randomtg == 4) then
		for pd = 1, 30 do
			AddItem(6,1,123,0,0,0,0)
		end
		logHoatDong("Ng­êi ch¬i <color=green>"..GetName().."<color> ®¸nh thñng trèng nhËn ®­îc 30 Phóc Duyªn §¹i")	
	elseif(randomtg == 5) then
		for vlmt = 1, 5 do
			AddItem(6,1,26,0,0,0,0)--vlmt
		end
		logHoatDong("Ng­êi ch¬i <color=green>"..GetName().."<color> ®¸nh thñng trèng nhËn ®­îc 5 VLMT")	
	elseif(randomtg == 6) then
		for vlmt = 1, 5 do
			AddItem(6,1,22,0,0,0,0)--ttk
		end	
		logHoatDong("Ng­êi ch¬i <color=green>"..GetName().."<color> ®¸nh thñng trèng nhËn ®­îc 5 TTK")				
	end

	local nExp = GetTongExp()
		SetTongExp(nExp + 500)
		UpdateTongExpAndLevel()
	local msgLog = "Ng­êi ch¬i <color=green>"..GetName().."<color> ®¸nh thñng trèng nhËn ®­îc nhiÒu chiÕn lîi phÈm."		
	Msg2SubWorld(msgLog)
end

function OnTimer(nIndex)
	DelNpc(nIndex)
end