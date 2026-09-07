-- Author: Fong KiÒu
-- Date: 28/11/2020
-- Chuc nang: item Trèng bang héi

Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\event\\trongbanghoi\\lib.lua")

function main(itemIdx)

	-- dofile("script/event/trongbanghoi/drum.lua")
	
	if (0 == GetCamp()) then
		Talk(1,"","<sex> ch­a gia nhËp m«n ph¸i. ")
		return
	end

	if (1 == GetFightState()) then
		Talk(1,"","Xin h·y sö dông t¹i khu vùc phi chiÕn ®Êu. ")
		return
	end
	
	if (GetLevel() < 80) then
		Talk(1,"","§¼ng cÊp cña c¸c h¹ kh«ng ®ñ. ")
		return
	end
	
	local nTime = tonumber(GetLocalDate("%H%M"));
	if nTime < nStartTime or nTime > nCloseTime then
		Talk(1, "", "Xin h·y sö dông trong thêi gian ho¹t ®éng!")
		return
	end	
	
	local nTongID = GetTongInfo(0)
	local nTongName = GetTongInfo(1)
	local nTongMaster = GetTongInfo(2)
	if(GetName() ~= nTongMaster) then
		Talk(1,"","§¹i hiÖp kh«ng ph¶i lµ bang chñ. ")
		return
	end
	
	local nMapID, nX, nY = GetWorldPos()
	if(nMapID ~= 54) then
		Talk(1,"","Xin h·y mang trèng vÒ Nam Nh¹c TrÊn ®Ó më. ")
		return
	end
	local npcType = NPC_TRBH[1][1]
	local nLevel = NPC_TRBH[1][2]
	local nSeries = {NPC_TRBH[1][3]}
	local nDropScript = ""
	local nszScript = "\\script\\event\\trongbanghoi\\tungtung1.lua"
	local nSzName = "Trèng bang "..nTongName
	local nNpcIdx = AddNpcEx1({npcType},nLevel,nSeries,nMapID,nX*32,nY*32,nDropScript,nszScript,nSzName,6)
	if(nNpcIdx > 0) then
		SetNpcTimer(nNpcIdx, TIME_LIFE_TRONG1)
		--Msg2Player(format("%d", nTongID))
		SetNpcParam(nNpcIdx, 1, nTongID) --set trèng param id bang
		local msgLog = format("Bang chñ bang <color=yellow> "..nTongName.." <color> ®¸nh Trèng Kh¶i Hoµn t¹i <color=pink> %s <color> quý nh©n sü cïng bang mau ®Õn tham gia. ", GetMapName(nMapID))
		Msg2SubWorld(msgLog)
		Msg2SubWorld(msgLog)
		Msg2SubWorld(msgLog)
		logHoatDong(msgLog)
		RemoveItem(itemIdx, 1)
	end
end

function OnTimer(nIndex)
	DelNpc(nIndex)
end
