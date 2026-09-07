--Author: Fong KiÒu
--Date: 2021
--Function: Uû th¸c rêi m¹ng

Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\log_game\\save_log.lua")

LIMIT_LEVEL_UT = 10
LIMIT_LEVEL_UTMAX = 79
TAB_MAPUTNAME = { 
	{1, "Ph­îng T­êng"},
	{11, "Thµnh §«"},
	{37, "BiÖn Kinh"},
	{78, "T­¬ng D­¬ng"},
	{80, "D­¬ng Ch©u"},
	{162, "§¹i Lý"},	
	{176, "L©m an"},
}

function CheckMapNoFor_UT(nMap)
	for i = 1, getn(TAB_MAPUTNAME) do
		if (nMap == TAB_MAPUTNAME[i][1]) then
			return 1
		end
	end
	return 0
end

function main1()
	
	-- dofile("script/player/offline.lua")
	--Talk(1,"","Chøc n¨ng ®ang x©y dùng !") 
	
end

function main()
	
	dofile("script/player/offline.lua")

	if(GetFightState() == 1) then
		Talk(1,"", "Kh«ng thÓ uû th¸c ngoµi thµnh, ®¹i hiÖp di chuyÓn vµo bªn trong ®Ó uû th¸c")
		return
	end

	local nMap,x,y = GetWorldPos()

	if(CheckMapNoFor_UT(nMap) == 0) then
		Talk(1,"","Kh«ng thÓ uû th¸c ë b¶n ®å nµy, di chuyÓn ®Õn th«n hoÆc thµnh thÞ ®«ng ng­êi qua l¹i ®Ó bµy b¸n vµ uû th¸c.!!!") 
		return
	end

	SayEx({"§­¬ng ®­¬ng ñy th¸c nÕu nh­ cã thiªn tinh b¹ch c©u hoµn hiÖu qu¶ , ­u tiªn nªn lo¹i b¹ch c©u hoµn còng ®em khÊu trõ t­¬ng øng hiÖu qu¶ .",
		"B¾t ®Çu ñy th5¸c rêi m¹ng /UyThacBanHang", 
		"B¾t ®Çu ñy th¸c trùc tuyÕn /begin_onlinecommission2", 
		"Dõng l¹i ñy th¸c trùc tuyÕn /end_onlinecommission2", 
		"C¸ch tuyÕn thêi gian /offlineaward2", 
		"Ta suy nghÜ mét chót n÷a /kethtuc"})
end

function begin_onlinecommission2()
	if GetLevel() < LIMIT_LEVEL_UT then
		Talk(1,"","§¼ng cÊp nhá h¬n "..LIMIT_LEVEL_UT.." kh«ng thÓ Uû Th¸c")
		return 0
	end
	Talk(1,"","Ng­¬i ®ang trong tr¹ng th¸i Uû th¸c Online")
end

function end_onlinecommission2()
	if GetLevel() < LIMIT_LEVEL_UT then
		Talk(1,"","§¼ng cÊp nhá h¬n "..LIMIT_LEVEL_UT.." kh«ng thÓ Uû Th¸c")
		return 0
	end
	Talk(1,"","Ng­¬i ®· ng­ng Uû th¸c Online")
end

function offlineaward2()
	if GetLevel() < LIMIT_LEVEL_UT then
		Talk(1,"","§¼ng cÊp nhá h¬n "..LIMIT_LEVEL_UT.." kh«ng thÓ Uû Th¸c")
		return 0
	end
	Talk(1,"","TÝnh n¨ng ®ang ®uîc hoµn thiÖn")
end

function UyThacBanHang()

	
	local szAccount = GetAccount()
	local szName = GetName()
	local dwID = 0--GetHWID()
	local nLevel = GetLevel()
	local nIP = 0--GetIP()
	local nExtPoint = GetExtPoint()
	local nMoney = GetCash()
	local logmgs = date("%H:%M:%S_%d-%m-%y").."\tUû Th¸c Tµi kho¶n: "..szAccount.."\t Nh©n vËt: "..szName.."\t HWID ID: "..dwID.."\t CÊp: "..nLevel.."\t IP: "..nIP.."\t Xu: "..nExtPoint.."\t Tæng tiÒn v¹n: "..nMoney.."\n"
	logWriteLogin(logmgs)
	IsLixian()
	KickOutSelf2()
end

function kethtuc()
end