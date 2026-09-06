-- Author: Fong KiÒu
-- Date: 28/11/2020

Include("\\Script\\lib\\lib_ham.lua")
Include("\\Script\\lib\\lib_map.lua")
Include("\\Script\\lib\\lib_task.lua")
Include("script\\global\\hocvocong.lua")

ADMIN_SUPPORT_TEXT 			= "Chøc n¨ng hç trî qu¶n trÞ viªn"
NOW_END_SAY 							= "KÕt thóc ®èi tho¹i./no"
PLAYER_KO_ONLINE 					= "Nh©n vËt kh«ng trªn m¹ng."

function system()
	SayEx({ADMIN_SUPPORT_TEXT,
	"Th«ng tin l­îng ng­êi ch¬i/showplayer",
	"Tµi kho¶n trªn m¹ng/accountonl",	
	"Chän thµnh viªn/chonthanhvien",
	-- "Chän thµnh viªn 1/kickacccung",
	"Di chuyÓn map/dichuyenmap",
	"Tµng h×nh/anthan",
	"HiÖn h×nh/hienthan",
	NOW_END_SAY})
end


function kickacccung()
OpenGetString("Tªn Tµi Kho¶n","kickacccung2")
end

function kickacccung2(name)
gmidx = PlayerIndex
for i=1,GetCountPlayerMax() do
PlayerIndex = i
	if GetAccount() == name then
		KickOutSelf2()
	end
end
PlayerIndex = gmidx
Msg2Player("Da Kick Tat ca tai khoan account:")
end



function showplayer()
	SayEx({ADMIN_SUPPORT_TEXT,
	"001 - 050/Show",	
	"051 - 100/Show",
	"101 - 150/Show",
	"151 - 200/Show",	
	"201 - 250/Show",	
	"251 - 300/Show",
	"301 - 350/Show",
	"351 - 400/Show",	
	"401 - 450/Show",
	"451 - 500/Show",	
	NOW_END_SAY})
end

function dichuyenmap()
	SayEx({ADMIN_SUPPORT_TEXT,
	"Ba l¨ng huyÖn/movemap",		
	NOW_END_SAY})
end

function anthan()
	AddSkillState(733,20,1,24*60*60*18,-1)
	SetCurCamp(0)
end

function hienthan()
	AddSkillState(733,20,1,2,1)
	SetCurCamp(GetCamp())
end

function movemap(nSel)
	local nRow = nSel+1
	local TAB_MOVE = {
		{53,1581,3242},	
		{70,1606,3178},
		{71,1606,3178},
		{69,1898,3575},
		{69,1538,3221},
		{52,1229,3561},	
		{52,1689,3074},	
		{52,1308,3461},	
		{52,1585,3181},		
	}
	
	NewWorld(TAB_MOVE[nRow][1],TAB_MOVE[nRow][2],TAB_MOVE[nRow][3])
	SetFightState(0)
end

function chonthanhvien()
	SayEx({ADMIN_SUPPORT_TEXT,
	"T×m b»ng tªn nh©n vËt/timtennv",
	"T×m b»ng ID nh©n vËt/timtennv",
	NOW_END_SAY})
end

function accountonl()
	local nPCount = GetPlayerCount();
	Msg2Player("§ang online: "..nPCount..".")
end

function dichuyenbalang()
	NewWorld(53,1581,3242)
	SetFightState(0)
	SetPKMode(0,0)
end

function timtennv(nSel)
	local nRow = nSel+1;
	if nRow == 1 then
		OpenGetString("Tªn nh©n vËt","timtennhanvat")
	elseif nRow == 2 then
		OpenGetNumber("ID nh©n vËt","timtenfrid")
	end
end

function timtennhanvat()
	local nName = GetStringFromUI()
	local nPlayerIndex = FindPlayer(nName)
	if (nPlayerIndex == 0) then
		Msg2Player(PLAYER_KO_ONLINE)
	return end
	SetTaskTemp(TMP_INDEX_PLAYER,nPlayerIndex)
	Msg2Player("B¹n chän nh©n vËt:<color=pink> "..nPlayerIndex.."#"..nName.."")
	thongtin()
end

function timtenfrid()
	local nPId = GetNumberFromUI()
	if (nPId <= 0) then
		return 
	end	
	local nName = GetName(nPId)
	if (nName == "") then
		Msg2Player(PLAYER_KO_ONLINE)
		return 
	end
	SetTaskTemp(TMP_INDEX_PLAYER,nPId)
	Msg2Player("B¹n chän nh©n vËt:<color=pink> "..nPId.."#"..nName.."")
	thongtin()
end

function thongtin()
	SayEx({ADMIN_SUPPORT_TEXT,
   "Th«ng tin nh©n vËt/invest",
   "Tù th«ng tin b¶n th©n/investex", 
   "T¾t cöa sæ trß ch¬i/tatcuaso",
   "T¾t trß chuyÖn/camchat",
   "Më trß chuyÖn/mochat",   
   "Gi¶i kÑt/giaiket",   
   "Add/addddiem",     
   "Di chuyÓn vÒ ba l¨ng/movetobalang",
   "Di chuyÓn tíi gÇn/movetonear",  
   NOW_END_SAY})	
end

function invest()
	local nbIdxTam 	= PlayerIndex
	PlayerIndex 	= GetTaskTemp(TMP_INDEX_PLAYER)
	local Name 		= GetName()
	local TK 		= GetAccount()
	local IP 		= GetIP()
	local Level 	= GetLevel()
	local Exp 		= GetExp()
	local Tien 		= GetCash()+GetSaveMoney()
	local Pass		= GetTaskTemp(TMP_MAP_VUOTAI)
	local Mau 		= GetLife(0)
	local Mau2 		= GetLife(2)
	local Mana 		= GetMana(0)
	local Mana2 	= GetMana(2)
	local VLMT 		= GetTask(T_VLMT)
	local TTK 		= GetTask(T_TTK)
	local CN 		= GetTask(T_CONGNGUYET)
	local PN 		= GetTask(T_PHUNGNGUYET)
	local DiemPD	= GetFuYuan()
	local DiemDV	= GetRepute()
	local KNB		= 0
	local TienD		= 0
	local TienXu	= GetExtPoint()
	local nW,nX,nY 	= GetWorldPos()
	local nMapName	= GetMapName(nW)
	local nReborn	= GetReBorn()
	PlayerIndex = nbIdxTam
	SetTaskTemp(TMP_INDEX_PLAYER,0);		
	Talk(3,"","- Tªn:<color=yellow> "..Name.." <color>\n- Tµi kho¶n: <color=blue>"..TK.."<color>\n- IP: <color=green>"..IP.."<color>\n- CÊp: <color=fire>"..Level.."<color>, kinh nghiÖm: <color=fire>"..Exp.."<color>\n- TiÒn: <color=yellow>"..Tien.."<color>, xu: <color=fire>"..TienXu.."",
	"- MËt khÈu r­¬ng: <color=yellow>"..Pass.."<color>\n- M¸u: <color=fire>"..Mau.."/"..Mau2.."<color>, mana: <color=blue>"..Mana.."/"..Mana2.."<color>\n- Täa ®é: <color=green>"..nMapName.." :: ("..floor(nX/8).."/"..floor(nY/16)..")<color>\n- Phóc duyªn: <color=green>"..DiemPD.."<color>\n- Danh väng: <color=green>"..DiemDV.."",
	"- Kim nguyªn b¶o: <color=yellow>"..KNB.."<color>\n- TiÒn xu: <color=fire>"..TienD.."<color>\n- C.sinh: <color=purple>"..nReborn.."<color>")
end

function investex()
	local nbIdxTam 	= PlayerIndex
	PlayerIndex 	= GetTaskTemp(TMP_INDEX_PLAYER)
	local Name 		= GetName()
	local TK 		= GetAccount()
	local IP 		= GetIP()
	local Level 	= GetLevel()
	local Exp 		= GetExp()
	local Tien 		= GetCash()+GetSaveMoney()
	local Pass		= 123456
	local Mau 		= GetLife(0)
	local Mau2 		= GetLife(2)
	local Mana 		= GetMana(0)
	local Mana2 	= GetMana(2)
	local VLMT 		= GetTask(T_VLMT)
	local TTK 		= GetTask(T_TTK)
	local CN 		= GetTask(T_CONGNGUYET)
	local PN 		= GetTask(T_PHUNGNGUYET)
	local DiemPD	= GetFuYuan()
	local DiemDV	= GetRepute()
	local KNB		= 0
	local TienD		= 0
	local TienXu	= GetExtPoint()
	local nW,nX,nY 	= GetWorldPos()
	local nMapName	= GetMapName(nW)
	local nReborn	= GetReBorn()
	Talk(3,"","- Tªn:<color=yellow> "..Name.." <color>\n- Tµi kho¶n: <color=blue>"..TK.."<color>\n- IP: <color=green>"..IP.."<color>\n- CÊp: <color=fire>"..Level.."<color>, kinh nghiÖm: <color=fire>"..Exp.."<color>\n- TiÒn: <color=yellow>"..Tien.."<color>, xu: <color=fire>"..TienXu.."",
	"- MËt khÈu r­¬ng: <color=yellow>"..Pass.."<color>\n- M¸u: <color=fire>"..Mau.."/"..Mau2.."<color>, mana: <color=blue>"..Mana.."/"..Mana2.."<color>\n- Täa ®é: <color=green>"..nMapName.." :: ("..floor(nX/8).."/"..floor(nY/16)..")<color>\n- Phóc duyªn: <color=green>"..DiemPD.."<color>\n- Danh väng: <color=green>"..DiemDV.."",
	"- Kim nguyªn b¶o: <color=yellow>"..KNB.."<color>\n- TiÒn xu: <color=fire>"..TienD.."<color>\n- C.sinh: <color=purple>"..nReborn.."<color>")	
	PlayerIndex = nbIdxTam
	SetTaskTemp(TMP_INDEX_PLAYER,0)
end

function tatcuaso()
	local nbIdxTam = PlayerIndex
	PlayerIndex = GetTaskTemp(TMP_INDEX_PLAYER)
	KickOutSelf2()
	PlayerIndex = nbIdxTam
	SetTaskTemp(TMP_INDEX_PLAYER,0)
end

function camchat()
	local nbIdxTam = PlayerIndex
	PlayerIndex = GetTaskTemp(TMP_INDEX_PLAYER)
	SetChatFlag(1)
	PlayerIndex = nbIdxTam
	SetTaskTemp(TMP_INDEX_PLAYER,0)
end

function mochat()
	local nbIdxTam = PlayerIndex
	PlayerIndex = GetTaskTemp(TMP_INDEX_PLAYER)
	SetChatFlag(0)
	PlayerIndex = nbIdxTam
	SetTaskTemp(TMP_INDEX_PLAYER,0)
end

function movetobalang()
	local nbIdxTam = PlayerIndex
	PlayerIndex = GetTaskTemp(TMP_INDEX_PLAYER)
	NewWorld(53,1581,3242)
	SetFightState(0)
	SetPKMode(0,0)
	SetRevPos(53,19); --sau khi ®¨ng nhËp l¹i vÒ ba l¨ng huyÖn
	SetTempRevPos(53, 51904, 102048)--ket thuc thiet lap diem hoi sinh o Ba Lang Huyen
	KickOutSelf()
	PlayerIndex = nbIdxTam
	SetTaskTemp(TMP_INDEX_PLAYER,0)
end
function giaiket()
	local nbIdxTam = PlayerIndex
	PlayerIndex = GetTaskTemp(TMP_INDEX_PLAYER)
	KickOutSelf()
	PlayerIndex = nbIdxTam
	SetTaskTemp(TMP_INDEX_PLAYER,0)
end
FactionName="¶ëáÒÅÉ"
function addddiem()
	local nbIdxTam = PlayerIndex
	PlayerIndex = GetTaskTemp(TMP_INDEX_PLAYER)
	-- AddItem(0,10,8,10,0,0,0)
	-- AddItem(0,10,6,10,0,0,0)
	-- AddItem2(2, 0, 50, 0, 0, 0)
	-- SetExtPoint(30)
	-- AddItemSL(4850,100,0) -- ruong trang bi xanh
	SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) + 5000)
	Earn(50000000)
	 -- if (GetLevel() < 120) then
		 -- for i = GetLevel(), 120-1 do 
		 -- AddOwnExp(100000000000)
		 -- end
	 -- end
	 -- AddMagicPoint(200)
	 -- AddProp(1000)
-- SetTask(TASK_DSK, GetTask(TASK_DSK) + 7500)
	-- AddItemSL(22,15,0)
	-- AddItemSL(26,15,0)
	-- AddItemSL(4835,100,0)
-- AddItemSL(4826,2000,0) -- event
-- AddItemSL(4826,2000,0)
-- AddItemSL(4826,2000,0)
-- AddItemSL(4826,2000,0)
-- AddItemSL(4826,2000,0)
-- AddItemSL(4826,2000,0)
-- AddItemSL(4844,5000,0) -- ho mach do
-- AddItemSL(4844,5000,0) -- ho mach do
-- AddItemSL(4844,5000,0) -- ho mach do
-- AddItemSL(4844,5000,0) -- ho mach do
-- AddItemSL(4844,5000,0) -- ho mach do
-- AddItemSL(4844,5000,0) -- ho mach do
-- AddItemSL(4844,5000,0) -- ho mach do
-- AddItemSL(4844,5000,0) -- ho mach do

-- AddItemSL(4847,5000,0)  -- chan nguyen don
-- AddItemSL(4847,5000,0)  -- chan nguyen don
-- AddItemSL(4847,5000,0)  -- chan nguyen don
-- AddItemSL(4847,5000,0)  -- chan nguyen don
-- AddItemSL(1023,200,-2) -- LB bosss
-- AddItemSL(2433,10,0) -- dtbk9x
-- AddItemSL(2434,1,0) -- dtbk12x
-- AddItemSL(1126,1,0) -- bk12x
-- SetTask(TASK_NAPTHE,3)
-- SetTask(T_RUT_KNB,20)

-- SetTask(T_RUT_KNB, GetTask(T_RUT_KNB) + 20)
-- mIndex = AddItem(0,10,5,10,0,0,10)


	PlayerIndex = nbIdxTam
	SetTaskTemp(TMP_INDEX_PLAYER,0)
end

function taykynang()
	
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


function movetonear()
	local nbIdxTam = PlayerIndex
	PlayerIndex = GetTaskTemp(TMP_INDEX_PLAYER)
	local nW,nX,nY = GetWorldPos()
	PlayerIndex = nbIdxTam
	NewWorld(nW,nX,nY)
	SetTaskTemp(TMP_INDEX_PLAYER,0)
end

function Show(nSel)
	local nRow = nSel+1
	local TAB_TT = {
		{1	,50 },
		{51 ,100},
		{101,150},
		{151,200},
		{201,250},	
		{251,300},	
		{301,350},	
		{351,400},	
		{401,450},
		{451,500},		
	}
	local nPCount = GetPlayerCount();
	local nCount = TAB_TT[nRow][2];	
	if (nPCount < TAB_TT[nRow][2]) then
		nCount = nPCount;
	end
	for i = TAB_TT[nRow][1],nCount do
	local nbIdxTam = PlayerIndex;
	PlayerIndex = i;
	local nName = GetName();
	local nW,nX,nY = GetWorldPos();
	PlayerIndex = nbIdxTam;
	Msg2Player(format("%03d :: %s :: %d :: %d :: %d.",i,nName,nW,floor(nX/8),floor(nY/16)));
	end
end

function no()
end