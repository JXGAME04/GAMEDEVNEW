--Author: Fong Ki?u

Include("\\script\\missions\\clearskill\\head.lua")
Include("\\script\\missions\\clearskill\\clearhole.lua")
Include("\\script\\lib\\lib_task.lua")


function main()
	
	dofile("script/global/npcchucnang/phantang.lua")

	--if (CSP_CheckValid() == 0) then
	--	Msg2Player("Kh«ng thÓ ®?n ®¶o tÈy tñy ")
	--	Say("GM: <color=red>B¹n kh«ng thÓ vµo tÈy tñy ®¶o, ®· ®­îc hÖ thèng l­u l¹i, vui lßng liªn hÖ GM ®Ó ®­îc hç trî! <color> ", 1, "Rêi khâi tÈy tñy ®¶o/LeaveHere_yes")
	--	return
	--end
	
	local nLevel = GetLevel()
	if (nLevel < CSP_NEEDLEVEL) then
		Talk(1, "", "CÊp bËc cña ng­¬i ch­a ®ñ"..CSP_NEEDLEVEL.."Kh«ng thÓ rêi khái tÈy tñy ®¶o. ")
		return
	end

	local nType = GetTask(T_TAYTUY_TYPE)
	if (nType == CSP_CTYPE_SKILL) then --chØ tÈy ®iÓm kü n¨ng
		local tbOpt = 
		{
			"TÈy ®iÓm kü n¨ng/DoClearSkill",
			-- "T¨ng ®iÓm ti?m n¨ng/add_prop",
			"Kh«ng tÈy/OnCancel",
			-- "Liªn quan ®?n tÈy tñy/Help",
			"Rêi khái tÈy tñy ®¶o /LeaveHere",
		}
		Say("Ng­¬i muèn tÈy <color=blue> §iÓm kü n¨ng<color>. §ång ? tÈy ? ", getn(tbOpt), tbOpt)
	elseif (nType == CSP_CTYPE_PROP) then --chØ tÈy ®iÓm ti?m n¨ng
		local tbOpt = 
		{
			"TÈy ®iÓm tiÒm n¨ng/DoClearProp",
			-- "T¨ng ®iÓm ti?m n¨ng/add_prop",
			"Kh«ng tÈy/OnCancel",
			-- "Liªn quan ®?n tÈy tñy /Help",
			"Rêi ®¶o tÈy tñy /LeaveHere",
		}
		Say("Ng­¬i muèn tÈy<color=blue> ®iÓm tiÒm n¨ng<color>. §ång ı tÈy ? ", getn(tbOpt), tbOpt)
	elseif (nType == CSP_CTYPE_ALL) then --tÈy c¸c lo¹i ®iÓm
		local tbOpt = 
		{
			 "TÈy ®iÓm kü n¨ng/DoClearSkill",
			 "TÈy ®iÓm tiÒm n¨ng/DoClearProp", 
			 --"T¨ng ®iÓm ti?m n¨ng/add_prop", 
			 "Kh«ng tÈy /OnCancel", 
			 -- "Liªn quan ®?n tÈy tñy/Help", 
			 "Rêi ®¶o tÈy tñy/LeaveHere"
		}
		Say("Ng­¬i ®ång ? tÈy tñy? ", getn(tbOpt),tbOpt)
	end	
end

function add_prop()
	Say("Ng­¬i ph¶i hiÓu râ c¸ch t¨ng ®iÓm", 4,
			"Søc m¹nh/add_prop_str",
			"Th©n ph¸p/add_prop_dex",
			"Sinh kh?/add_prop_vit",
			"Néi c«ng/add_prop_eng")
end

function add_prop_str()
	AskClientForNumber("enter_str_num", 0, GetProp(), "Mêi nhËp chØ sè søc m¹nh: ")
end

function add_prop_dex()
	AskClientForNumber("enter_dex_num", 0, GetProp(), "Mêi nhËp chØ sè th©n ph¸p: ")
end

function add_prop_vit()
	AskClientForNumber("enter_vit_num", 0, GetProp(), "Mêi nhËp chØ sè sinh kh?:")
end

function add_prop_eng()
	AskClientForNumber("enter_eng_num", 0, GetProp(), "Mêi nhËp chØ sè néi c«ng: ")
end

function enter_str_num(n_key)
	if (n_key < 0 or n_key > GetProp()) then
		return
	end
	AddStrg(n_key)
end

function enter_dex_num(n_key)
	if (n_key < 0 or n_key > GetProp()) then
		return
	end
	AddDex(n_key)
end

function enter_vit_num(n_key)
	if (n_key < 0 or n_key > GetProp()) then
		return
	end
	AddVit(n_key)
end

function enter_eng_num(n_key)
	if (n_key < 0 or n_key > GetProp()) then
		return
	end
	AddEng(n_key)
end

function DoClearSkill()
	Say("Ng­¬i ®ång ı tÈy kü n¨ng ", 2, "TÈy kü n¨ng/DoClearSkillCore","Kh«ng tÈy/OnOLai")
end



function DoClearSkillCore()
	
--	if (Pay(100000) == 0) then
--		Say("TÈy tñy thµnh c«ng ng­¬i c? thÓ t¨ng l¹i ®iÓm", 0)
--		return
--	end
	diemkn = GetMagicPoint()
	AddMagicPoint(-diemkn)
	i = HaveMagic(210)		-- skill khinh kong
	j = HaveMagic(400)		-- skill be kiep phu ban
	local rAll = 0
	n = RollbackSkill(rAll)		
	x = 0
	if (i ~= -1) then x = x + i end		
	if (j ~= -1) then x = x + j end
	rollback_point = n - x		
	
	if (rollback_point + GetMagicPoint() < 0) then
		 rollback_point = -1 * GetMagicPoint()
	end
	
	if (rollback_point < 0) then			--fix by phong kieu
		rollback_point = 0
	end
	rollback_point = (GetLevel()-1)+GetTask(T_VLMT) 
	AddMagicPoint(rollback_point)
	-- Msg2Player(format("210i=%d, 400j=%d", i, j))
	if (i ~= -1) then AddMagic(210, i) end			
	if (j ~= -1) then AddMagic(400, j) end			
	Msg2Player("TÈy tñy thµnh c«ng ng­¬i cã thÓ t¨ng l¹i ®iÓm "..rollback_point.." §iÓm kü n¨ng cã thÓ ph©n phèi l¹i. ")
	Talk(1,"KickOutSelf","TÈy tñy thµnh c«ng ng­¬i cã thÓ t¨ng l¹i ®iÓm "..rollback_point.." §iÓm kü n¨ng cã thÓ ph©n phèi l¹i. ")
end;

function DoClearProp()
	Say("Ng­¬i ®ång ı tÈy ®iÓm tiÒm n¨ng? ", 2, "TÈy ®iÓm tiÒm n¨ng/TayTiemNang", "Kh«ng tÈy/OnCancel")
end

function DoClearPropCore()
	ResetProp();
a = GetSeries()
if a == 0 then
RestAP(35,25,25,15)
elseif a == 1 then
RestAP(20,35,20,25)
elseif a == 2 then
RestAP(25,25,25,25)
elseif a == 3 then
RestAP(30,20,30,20)
elseif a == 4 then
RestAP(20,15,25,40)
end

	diem = (GetLevel()-1)*5 + GetTask(T_TTK)*5 
	AddPropPoint(diem);
	
	--Talk(1,"",format("TÈy tñy thµnh c«ng ng­¬i cã thÓ ph©n phèi l¹i <color=Yellow>%d<color> ®iÓm tiÒm n¨ng",nPnt));
end

function TayTiemNang()
	
-- a = GetSeries()
-- if a == 0 then
-- RestAP(35,25,25,15)
-- elseif a == 1 then
-- RestAP(20,35,20,25)
-- elseif a == 2 then
-- RestAP(25,25,25,25)
-- elseif a == 3 then
-- RestAP(30,20,30,20)
-- elseif a == 4 then
-- RestAP(20,15,25,40)
-- end
ResetProp()
diemtn = GetProp()
AddPropPoint(-diemtn)
diem = (GetLevel()-1)*5 + GetTask(T_TTK)*5 
AddPropPoint(diem)
KickOutSelf()
end


function Help()
	local strHelp = {
		"Sau khi tÈy tñy ng­¬i c? thÓ ra ngoµi ®¸nh méc nhËn,hoÆc c? thÓ lËp nh?m vµo s¬n ®éng thùc hµnh chi?n ®Êu,sau ®? rêi ®¶o míi c? thÓ xem lµ ®¾c ®¹o. ",
		"ChØ cÇn ng­¬i kh«ng rêi ®¶o, ta c? thÓ gióp ng­¬i tÈy tñy ",
		"Bªn ngoµi c? c¸c cét gæ 3000 sinh lùc ;  bao c¸t 10000 sinh lùc ; Méc nh©n 30000 sinh lùc ®Ó ng­¬i tËp luyÖn hiÖu qu¶! ",
		"N?u nh­ muèn thùc hµnh c? thÓ vµo s¬n ®éng n?u nh­ c? ch?t c?ng kh«ng mÊt m¸t g× ",
		"Trong s¬n ®éng rÊt hÑp nhi?u nhÊt chØ chøa 20 ng­êi n?u ®· ®ñ ng­êi th× kh«ng nªn vµo trong. ",
		"Trong s¬n ®éng rÊt hÑp nhi?u nhÊt chØ chøa 20 ng­êi n?u ®· ®ñ ng­êi th× kh«ng nªn vµo trong. ",
		"Cuèi cïng lóc nµo khi thÊy kü n¨ng ®· thµnh th¹o th× c? thÓ gÆp ta ®­a v? chæ cñ. "
	}
	Talk(6, "", strHelp[1], strHelp[2], strHelp[3], strHelp[4], strHelp[6], strHelp[7])
end

function LeaveHere()
	Say("Ng­¬i cho r»ng ®· thµnh th¹o muèn rêi ®¶o? ",2, "Muèn rêi khái/LeaveHere2", "ViÖc nµy ta sÏ suy nghÜ l¹i/OnOLai")
end

function LeaveHere2()
	Say("Muèn rêi khái ®·o th× khi quay vÒ rÊt khã", 2, "Ta muèn rêi khái n¬i quû qu¸i nµy/LeaveHere_yes", "Kh«ng ta sÏ ë l¹i/OnOLai")
end

function LeaveHere_yes()
	W,X,Y = GetWorldPos()
	LeaveTeam()
	--CSP_LeaveClearMap(W)
	
	NewWorld(78, 1592, 3377) SetRevPos(78,29) --ve vµ l­u r­¬ng tuong duong
end

function OnOLai()
end

function OnCancel()
end
