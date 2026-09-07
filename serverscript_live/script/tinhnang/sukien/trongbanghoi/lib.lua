-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local TBH_CFG
-- Author: Fong KiÒu
-- Date: 28/11/2020
-- Function: Lib trèng bang héi

Include("\\script\\lib\\lib_server.lua")
-- [CFGTAT 30/08] hai tep duoi day la LA (khong Include gi).
Include("\\script\\cauhinh\\ch_lib.lua")
Include("\\script\\cauhinh\\ch_thuong.lua")
-- [CFGTAT 30/08] Bo doc cau hinh cho tep nay. Tra ve MAC DINH (= so cu)
-- khi bo cau hinh chua nap, nen kem nhat cung khong the doi hanh vi.
function TBH_CFG(szKhoa, macdinh)
	if (G_CFG ~= nil) then
		return G_CFG(szKhoa, macdinh)
	end
	return macdinh
end

Include("\\script\\lib\\lib_map.lua")
Include("\\script\\log_game\\save_log.lua")

TIME_LIFE_TRONG   	= 1200* 18
TIME_LIFE_TRONG1 	= TBH_CFG("TBH_PHUT_TRONG_TON_TAI", 30)*60*18 --30p
TIME_LIFE_CTT			 	= 40*60*18 --40p
nStartTime 						 	= TBH_CFG("TBH_GIO_MO", 1900)
nCloseTime 					 	= TBH_CFG("TBH_GIO_DONG", 2359)
EXP_TBH_DAY 			 	= TBH_CFG("TBH_EXP_TRAN_NGAY", 200000000) --tèi ®a 200 tr exp ngµy
EXP_RAN1					 	= TBH_CFG("TBH_EXP_MIN", 1000000) 
EXP_RAN2					 	= TBH_CFG("TBH_EXP_MAX", 2000000)
LIMIT_LEVEL_TBH	 	= 80
SCRIPTNPCTRONGBH 	= "\\script\\event\\trongbanghoi\\congtontoan.lua"

TIME_TRONGBH = {
	{19,45, 19,40}
}

NPC_TRBH = {
	--{1556,80,4,355,1395,2740,0,"Tïng C¾c Tïng C¾c C¾c",0,"\\script\\event\\trongbanghoi\\tungtung.lua","main", 0},
    --{1556,80,4,355,1371,2763,0,"Tïng C¾c Tïng C¾c C¾c",0,"\\script\\event\\trongbanghoi\\tungtung.lua","main", 0},
	{1556,80,4,37,1569,3251,"Trèng Bang Héi","\\script\\event\\trongbanghoi\\tungtung.lua"}, --196/203
	--{1556,80,4,37,1585,3247,"Trèng Bang Héi","\\script\\event\\trongbanghoi\\tungtung.lua"} --198/202
}

function add_trongbanghoi() 
	local nNpcIdx
	for i = 1, getn(NPC_TRBH) do
		local npcType = NPC_TRBH[i][1]
		local nLevel = NPC_TRBH[i][2]
		local nSeries = {NPC_TRBH[i][3]}
		local nMapID = NPC_TRBH[i][4]
		local nX = NPC_TRBH[i][5] * 32
		local nY = NPC_TRBH[i][6] * 32
		local nszName = NPC_TRBH[i][7]
		local nszScript = NPC_TRBH[i][8]
		local nDropScript = ""
		local nMapIndex = SubWorldID2Idx(nMapID)
		if (nMapIndex >= 0 ) then
			nNpcIdx = AddNpcEx1({npcType},nLevel,nSeries,nMapID,nX,nY,nDropScript,nszScript,nSzName,6)
			if(nNpcIdx) then
				SetNpcTimer(nNpcIdx, TIME_LIFE_TRONG)
				local msgLog = format("<color=yellow>Trèng Kh¶i Hoµn<color> ®· xuÊt hiÖn t¹i <color=pink> %s (%d,%d)<color> quý nh©n sü mau mau tranh ®o¹t. ", GetMapName(nMapID), nX/8/32, nY/16/32)
				Msg2SubWorld(msgLog)
				AddGlobalCountNews(msgLog, 3)
				logHoatDong(msgLog)
			end
		end
	end
end

function add_npctrongbanghoi()
	local nNpcIdx = AddNpcEx1({203},1,nil,37, 1693*32, 3213*32,nil,SCRIPTNPCTRONGBH,"C«ng T«n To¶n",6)
	SetNpcTimer(nNpcIdx, TIME_LIFE_CTT)
end

