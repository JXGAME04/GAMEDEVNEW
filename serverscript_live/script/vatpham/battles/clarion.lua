--Function:Binh si hieu phu binh sü hiÖu phï vËt phÈm tèng kim
--Author: Fong KiÒu
--Date: 2021

Include("\\script\\header\\forbidmap.lua")
Include("\\script\\lib\\worldlibrary.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")
Include("\\script\\lib\\lib_task.lua")

MAX_CALLNPCCOUNT = 20

function main(nItemIdx)
	
	dofile("script/item/battles/clarion.lua")
	
	if (GetMSRestTime(MS_TONGKIM,1) > 0) then
		Talk(1,"","Kh«ng thÓ sö dông khi ®¹i chiÕn ch­a b¾t ®Çu!")
		return				
	end
	
	if (GetFightState() == 0) then
		Talk(1,"","Kh«ng thÓ sö dông trong t×nh tr¹ng phi chiÕn ®Êu!")
		return
	end
	
	local W,X,Y = GetWorldPos()
	local nMapId = W
	if (checkSJMaps(nMapId) ~= 1) then
		Msg2Player("B¹n kh«ng thÓ sö dông vËt phÈm nµy ë ®©y")
		return
	end
	
	if (GetCurCamp() == 1) then
		if (GetMissionV(MS_CALLNPCCOUNT_S) >= MAX_CALLNPCCOUNT) then
			Msg2Player("HiÖn t¹i sè l­îng hiÖu gi¸c ChiÕn tr­êng sö dông ®· v­ît møc, kh«ng thÓ sö dông tiÕp n÷a. ")
			return
		else
			SetMission(MS_CALLNPCCOUNT_S, GetMissionV(MS_CALLNPCCOUNT_S) + 1)
		end
	elseif (GetCurCamp() == 2) then
		if (GetMissionV(MS_CALLNPCCOUNT_J) >= MAX_CALLNPCCOUNT) then
			Msg2Player("HiÖn t¹i sè l­îng hiÖu gi¸c ChiÕn tr­êng sö dông ®· v­ît møc, kh«ng thÓ sö dông tiÕp n÷a. ")
			return
		else
			SetMission(MS_CALLNPCCOUNT_J, GetMissionV(MS_CALLNPCCOUNT_J) + 1)		
		end
	end
	
	if( GetCurCamp() == 1) then
		CallSjNpc( 682, 100, W, X, Y, "Tèng binh", 1)
		CallSjNpc( 682, 100, W, X, Y, "Tèng binh", 1)
	elseif( GetCurCamp() == 2) then
		CallSjNpc( 688, 100, W, X, Y, "Kim binh", 2)
		CallSjNpc( 688, 100, W, X, Y, "Kim binh", 2)
	end	
	RemoveItem(nItemIdx,1,1)
end	

function CallSjNpc(NpcId, NpcLevel, W, X, Y, Name, nPhe)
	local npcName = GetName().." " .. Name
	local nNpcIdx = AddNpcEx3({NpcId},NpcLevel,{0,1,2,3,4},W,(X+random(1,2))*32,(Y+random(1,2))*32,DROPRATETONGKIM,ONDEATHQUAITK, npcName, nPhe,0,36000,nil,500,50,100,200,nil,20)
	AddMSNpc(MS_TONGKIM, nNpcIdx, nPhe)
	SetNpcValue(nNpcIdx, 6)
end