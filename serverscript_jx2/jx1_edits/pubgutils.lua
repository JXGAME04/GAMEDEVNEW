Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_map.lua")

REVIVE_MAP = 53
REVIVE_X = 52032
REVIVE_Y = 101696
BIENKINHPUBG = 996
BIENKINHPUBGWAIT = 997

MISSION_BR_PARAM_KILLS = 1
MISSION_BR_PARAM_JOINED = 0
MISSION_BR_PARAM_LASTLAYER = 2
PUBG_TARGET = {
{48273, 93512},
{46551, 92293},
{45414, 93031},
{44905, 89815},
};

MSTIME_PUBG_BD = 1 -- bao danh 4
MSTIME_PUBG_KT = 5

PUBG_MIN_PLAYER = 1

function start_pubg()
	local w = 996 
	local x = 55232 --need gen random location
	local y = 99200
--need to move all player from 997 to 996 and set
	SubWorld = SubWorldID2Idx(BIENKINHPUBG)
	local nCount = GetMSPlayerCount(MS_PUBG)
	
	local target = PUBG_TARGET[random(1, getn(PUBG_TARGET))]
	print("PUBG target "..target[1].." "..target[2])
	GenNewPUBGMap(target[1], target[2])
	print(SubWorld.. " "..GetMSPlayerCount(MS_PUBG).." "..MS_PUBG)
	for dataindex=1, nCount do
		PlayerIndex = MSDIdx2PIdx(MS_PUBG, dataindex)
		print(PlayerIndex)
		NewWorld(w,floor(x/32),floor(y/32))
		SetProtectTime(18*180)
		AddSkillState(963, 1, 0, 18*180)
		AddSkillState(1560,30,1, 23*60*60*18,-1)
		SendClientPoint(target[1], target[2])
		Talk(1, "", "Cuéc chi?n Sinh Tån b¾t ®Çu, h·y cè g¾ng sèng s?t!")
	end
end

function process()
	local tmp = SubWorldID2Idx(BIENKINHPUBG)
	local playerCount = GetSubWorldPlayerCount(tmp)
	local nYr,nMo,nDy,nHr,nMi,nSe,nDyfW = GetTimeNow()

	if playerCount >= 2 then
		local nLayers = GetLayerCount()
		if nLayers <= 0 then return end

		local totalSeconds = 4 * 60  -- 900 seconds per round
		local roundStartMin = floor(nMi / 20) * 20
		local secondInRound = (nMi - roundStartMin) * 60 + nSe
		
		local nSubWorldId = SubWorldID2Idx(BIENKINHPUBG)
		if nSubWorldId < 0 then	--chua mo map, ngung ham`
			Talk(1,"", "Xin lçi! Khu vùc Sinh Tån ch­a më.")
			return 
		end
		
		SubWorld = nSubWorldId
		local nRestTime = tonumber(GetMSRestTime(MS_PUBG,15)/18)
		print("PUBG con "..nRestTime.." giay")
		secondInRound = totalSeconds - nRestTime
		print("PUBG da chay "..secondInRound.." giay")
		if secondInRound >= totalSeconds then return end

		local interval = 5
		local startSecond = secondInRound
		local endSecond = min(secondInRound + interval - 1, totalSeconds - 1)

		for i = 0, nLayers - 1 do
			local assignedSecond = floor(i * totalSeconds / nLayers)
			if assignedSecond >= startSecond and assignedSecond <= endSecond then
				UseLayer(nLayers - i)
			end
		end
	end
end
-- 2002 (ban phao)
NPC_ID = {1036, 1438, 1201, 1871, 1973, 1845, 1364, 1509, 1395, 1873, 1944, 1958, 1012, 582, 1474, 1896, 1679, 1243, 775, 1361, 1390, 1742, 1762, 1766};

function UseLayer(nLayer)
	print("PUBG add new layer"..nLayer)
	local nPointsInLayer = GetPointCountInLayer(nLayer)
	print("PUBG add layers with "..nPointsInLayer.." points")
	--Update client new layer
	local tmp = SubWorldID2Idx(BIENKINHPUBG)
	UpdatePubgCircle(tmp, nLayer)
	for nPoint = 0, nPointsInLayer-1 do
			
		local npcid = NPC_ID[random(1, getn(NPC_ID))]
		
		x, y = GetPoint(nLayer, nPoint)
		local nCamp = 1
		local n = 0
		nLifeNpc = 10000000
		print("PUBG add npc "..npcid.." x= "..floor(x).." y= "..floor(y))
		local nNpcIdx = AddNpcEx3({npcid},1,{0,1,2,3,4},BIENKINHPUBG,floor(x),floor(y),"","","Wall",nCamp,0,nLifeNpc,nil,nil,nil,nil,nil,nil,100)
		print("nNpcIdx= "..nNpcIdx)
		--SendClientPoint(x, y)
		SetNpcActiveRange(nNpcIdx, 100)
		SetNpcScript(nNpcIdx, "\\script\\test\\npcwalk.lua")
	end
end

function NgoaiBo(nPlayerIndex)
	PlayerIndex = nPlayerIndex
	AddSkillState(394, 50, 1, 50*18, 0)

end