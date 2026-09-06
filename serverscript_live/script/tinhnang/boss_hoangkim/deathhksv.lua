--Author: Fong KiÒu
--Date: 2021
--Function: Boss ®¹i hoµng kim chÕt

Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_server.lua")
Include("script\\global\\vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\tinhnang\\boss_hoangkim\\lib_bosshk.lua")

KILLBOSSEXPAWARD 			= 20000000 * EXP_RATE
KILLBOSSNEAREXPAWARD 	= 5000000 * EXP_RATE
PHAMVI_HUONGEXP 				= 200



function OnDeath(nNpcIndex, nDamageIndex)
	DelNpc(nNpcIndex)
	
	local nPlayerIndex = NpcIdx2PIdx(nDamageIndex)
	if not nPlayerIndex or nPlayerIndex <= 0 then return end
	
	
	PlayerIndex = nPlayerIndex	
	local nTeamId = GetTeam()
	idx = PlayerIndex
	tong = GetTongName()
	name = GetName()
	local w1,x1,y1 = GetWorldPos(nNpcIndex)
	Msg2SubWorld("<color=green>Chóc mõng ®¹i hiÖp ["..GetName().."] Bang Héi ["..GetTongName().."] ®· tiªu diÖt Boss M¸y Chñ Ký Sù CTC vµ nhËn ®­îc:")
	Msg2SubWorld("<color=yellow>- 50 Xu")
	Msg2SubWorld("<color=yellow>- 1 TÈy Tñy Kinh")
	Msg2SubWorld("<color=yellow>- 1 Vâ L©m MËt TÞch")
	AddItem(6,1,26,0,0,0,0)
	AddItem(6,1,22,0,0,0,0)
	AddItemSL(4850,5,0) -- ruong trang bi xanh
	
local naddtungphaiList = {253, 258, 263, 239, 243, 248, 313, 308, 318, 298, 303, 268, 276, 283, 288, 331, 333, 338, 343, 353, 358, 363, 368}

local randIndex = random(1, getn(naddtungphaiList))  
local itemid = naddtungphaiList[randIndex]           
AddItemSL(itemid, 1, 0)   -- manh do pho hkmp 
local nVIP = GetTask(TASK_NEWTHOREN7)

Msg2Player("Test <color=green>"..nVIP.." <color=red> c¸i!")
	SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) + 50)
	local nExp = GetTongExp()
	SetTongExp(nExp + 100)
	UpdateTongExpAndLevel()

--Msg2SubWorld("<color=yellow>[HÖ Thèng] <color=red>Chóc mõng ®¹i hiÖp ["..GetName().."] ®· tiªu diÖt "..GetNpcName(idx).." . ")
bang = GetTongName()
--if w == 217 then
	manhngua1 = 0
	manhngua2 = 0
	manhab = random(5,10)
	manhdq = random(1,3)
	tienvan = 0
	manhngua3 = 0
	knbb = 1
	if bang ~= "" then
		--Msg2SubWorld("<color=green>Toµn bé thµnh viªn bang héi ["..bang.."] nhËn ®­îc 6.000.000 kinh nghiÖm")
		vt = checkbang(bang)
		if  vt == 0 then
			BOSSHK[getn(BOSSHK)+1] = {bang,manhngua1,manhngua2,manhab,manhdq,tienvan,manhngua3,knbb}
			Msg2SubWorld("<color=green>Bang héi "..bang.." nhËn ®­îc: "..knbb.." KNB, "..manhab.." M¶nh Hoµng Kim, "..manhdq.." Thuû Tinh")
			inlog("Bang héi "..bang.." nhËn ®­îc:  "..knbb.." KNB, "..manhab.." M¶nh Hoµng Kim, "..manhdq.." Thuû Tinh")
		else
			BOSSHK[vt][2] = BOSSHK[vt][2] + manhngua1
			BOSSHK[vt][3] = BOSSHK[vt][3] + manhngua2
			BOSSHK[vt][4] = BOSSHK[vt][4] + manhab
			BOSSHK[vt][5] = BOSSHK[vt][5] + manhdq
			BOSSHK[vt][6] = BOSSHK[vt][6] + tienvan
			BOSSHK[vt][7] = BOSSHK[vt][7] + manhngua3
			BOSSHK[vt][8] = BOSSHK[vt][8] + knbb
			Msg2SubWorld("<color=green>Bang héi "..bang.." nhËn ®­îc: "..knbb.." KNB, "..manhab.." M¶nh Hoµng Kim, "..manhdq.." Thuû Tinh")
			inlog("Bang héi "..bang.." nhËn ®­îc:  "..knbb.." KNB, "..manhab.." M¶nh Hoµng Kim, "..manhdq.." Thuû Tinh")
	end
		LuuDanhSach()
	else
		
		
		--Msg2SubWorld("<color=green>Bang héi "..bang.." nhËn ®­îc: "..knbb.." KNB, "..manhab.." M¶nh Hoµng Kim, "..manhdq.." Thuû Tinh")
		--inlog("Bang héi "..bang.." nhËn ®­îc:  "..knbb.." KNB, "..manhab.." M¶nh Hoµng Kim, "..manhdq.." Thuû Tinh")
	end
local nTongExp = 0
local nTongMotMinh = 500000000* nVIP
if nTeamId == nil then
  
	AddSumExp(nTongMotMinh)
	
	Msg2Player("B¹n chØ s¨n boss 1 m×nh, nhËn ®­îc "..nTongMotMinh.."  kinh nghiÖm")
	Msg2SubWorld("§¹i hiÖp ["..GetName().."] ®· ®¬n th©n tiªu diÖt boss, nhËn ®­îc 500.000.000 kinh nghiÖm ")
	
	for i=1,GetPlayerCount() do
	PlayerIndex = i
		local szName = GetName()
			if szName and szName ~= "" and szName ~= name then
			w,x,y = GetWorldPos()
				if w == w1 and abs(x - x1) < 40 and abs(y - y1) < 80 then
					AddSumExp(200000000* nVIP)
					Msg2Player("B¹n ë trong khu vùc s¨n boss, nhËn ®­îc 200.000.000 kinh nghiÖm")
			end
		end
	end
else
	for i=1,GetPlayerCount() do
	PlayerIndex = i
		local szName = GetName()
			if szName and szName ~= "" then
			w,x,y = GetWorldPos()
				if GetTeam() == nTeamId then
					AddSumExp(300000000 * nVIP)
					Msg2Player("B¹n thuéc nhãm tiªu diÖt Boss, nhËn ®­îc 300.000.000 kinh nghiÖm")
				elseif w == w1 and abs(x - x1) < 40 and abs(y - y1) < 80 then
					AddSumExp(100000000* nVIP)
					Msg2Player("B¹n ë trong khu vùc s¨n boss, nhËn ®­îc 100.000.000 kinh nghiÖm")
				end
		end
	end
end

	
PlayerIndex = idx
end

function inlog(string)
thoigian = tonumber(date("%H%M%d%m"))
LoginLog = openfile("dulieu/LogQuanLyBoss.txt", "a");
if LoginLog then
write(LoginLog,""..GetAccount().." - "..GetName().." - "..string.." - Time: "..thoigian.."\n");
end
closefile(LoginLog)
end

function OnRevive(nNpcIndex)
		local Series = GetNpcSeries(nNpcIndex)
	local nSTVL, nDoc, nBang, nHoa, nLoi = 0,0,0,0,0
	if(Series==0) then	--KIM
		SetNpcSkill(nNpcIndex, 321, 1, 1);
		SetNpcSkill(nNpcIndex, 319, 1, 2);
		SetNpcSkill(nNpcIndex, 322, 1, 3);
		SetNpcSkill(nNpcIndex, 325, 1, 4);
		nSTVL = 0
	elseif(Series==1) then	--MOC
		SetNpcSkill(nNpcIndex, 339, 1, 1);
		SetNpcSkill(nNpcIndex, 342, 1, 2);
		SetNpcSkill(nNpcIndex, 302, 1, 3);
		SetNpcSkill(nNpcIndex, 390, 1, 4);
		nDoc = 0
	elseif(Series==2) then	--THUY
		SetNpcSkill(nNpcIndex, 328, 1, 1);
		SetNpcSkill(nNpcIndex, 380, 1, 2);
		SetNpcSkill(nNpcIndex, 336, 1, 3);
		SetNpcSkill(nNpcIndex, 337, 1, 4);
		nBang = 0
	elseif(Series==3) then	--HOA
		SetNpcSkill(nNpcIndex, 357, 1, 1);
		SetNpcSkill(nNpcIndex, 359, 1, 2);
		SetNpcSkill(nNpcIndex, 361, 1, 3);
		SetNpcSkill(nNpcIndex, 362, 1, 4);
		nHoa = 0
	elseif(Series==4) then	--THO
		SetNpcSkill(nNpcIndex, 365, 1, 1);
		SetNpcSkill(nNpcIndex, 368, 1, 2);
		SetNpcSkill(nNpcIndex, 372, 1, 3);
		SetNpcSkill(nNpcIndex, 375, 1, 4);
		nLoi = 0
	else					--KHONG CO HE, truong hop nay la add sai hay sao do
		--SetNpcSkill(nNpcIndex, 1055, 20, 1)
		--SetNpcSkill(nNpcIndex, 1057, 20, 2)
		--SetNpcSkill(nNpcIndex, 1058, 20, 3)
		--SetNpcSkill(nNpcIndex, 1060, 20, 4)
		nSTVL = 0
	end
		--SetNpcReplenish(nNpcIndex,1);--phuc hoi sinh luc	
		SetNpcSpeed(nNpcIndex, 20)--toc do di chuyen tang len
		SetNpcActiveRange(nNpcIndex, 600)
		SetNpcHitRecover(nNpcIndex,100);--cao nhat la 100(ko giat) --tuy cam hung
		-- SetNpcDmgEx(nNpcIndex,nSTVL, nDoc, nBang, nHoa, nLoi ,0)
		-- SetNpcDmgEx(nNpcIndex,nSTVL, nDoc, nBang, nHoa, nLoi ,1)
		SetNpcResist(nNpcIndex, 95, 120, 95, 120, 95)--khang' cac loai
		SetNpcBoss(nNpcIndex, 4)
end

function OnTimer(nNpcIndex)
	DelNpc(nNpcIndex)
end
	
	
function main()

tenbang = GetTongName()
if tenbang == "" then
Talk(1,"","Ng­¬i kh«ng cã bang héi, tíi t×m ta lµm g×?")
return
end

vt = checkbang(tenbang)
if vt == 0 then
Talk(1,"","Bang héi cña ng­¬i kh«ng ký göi vËt phÈm chç ta ")
return
end

if (GetName() == GetTongInfo(2))  then
SayEx({"<color=green><npc>: <color>Ta qu¶n lý vËt phÈm s¨n Boss cña Bang Héi:",
"Rót KNB /rutmanh",
"Rót M¶nh Hoµng Kim /rutmanh",
"Rót Thuû Tinh /rutmanh",
"Tho¸t./no"})
else
SayEx({"<color=green><npc>: <color>Ta qu¶n lý vËt phÈm s¨n Boss cña Bang Héi:",
"Xem KNB /xemmanh",
"Xem M¶nh Hoµng Kim/xemmanh",
"Xem Thuû Tinh /xemmanh",
"Tho¸t./no"})
end
end

function no()
end

function xemmanh(nsel)
i = nsel + 1
vt = checkbang(tenbang)
soluong = 0
name = ""
if i == 1 then
soluong = BOSSHK[vt][8]
name = "KNB"
elseif i == 2 then
soluong = BOSSHK[vt][4]
name = "M¶nh Hoµng Kim"
elseif i == 3 then
soluong = BOSSHK[vt][5]
name = "Thuû Tinh"
end

if name == "" then
Talk(1,"","Lçi, b¸o GM gÊp")
return
end
Talk(1,"","HiÖn t¹i bang héi ®ang cã: <color=yellow>"..soluong.." "..name.."")

end 

function rutmanh(nsel)
i = nsel+1
vt = checkbang(tenbang)


vtnum = 0
name = ""
if i == 1 then
vtnum = 8
name = "KNB"
itemmin = 343
itemmax = 343
elseif i == 2 then
vtnum = 4
name = "M¶nh Hoµng Kim"
itemmin = 903 
itemmax = 942
elseif i == 3 then
vtnum = 5
name = "Thuû Tinh"
itemmin = 238
itemmax = 240
end
if name == "" then
Talk(1,"","Lçi, b¸o GM gÊp")
return
end

if i == 1 then
	if BOSSHK[vt][vtnum] == 0 then
		Talk(1,"","HiÖn t¹i kh«ng cßn "..name.." göi ë chç ta")
		return
	end
	num = 0
	for t=1,BOSSHK[vt][vtnum] do
		if CalcFreeItemCellCount() > 10 then
			BOSSHK[vt][vtnum] = BOSSHK[vt][vtnum] - 1
			AddEventItem(random(itemmin,itemmax),1)
			--SaveNow()
			num = num + 1
		end
	end
	Msg2Player("B¹n nhËn ®­îc "..num.." "..name.." ")
	Msg2SubWorld("<color=yellow>Bang chñ "..GetName().." ®· rót "..num.." "..name.." ")
	inlog("Rut "..num.." "..name.."")
elseif i >= 2 then
		if BOSSHK[vt][vtnum] == 0 then
		Talk(1,"","HiÖn t¹i kh«ng cßn "..name.." göi ë chç ta")
		return
	end
	num = 0
	for t=1,BOSSHK[vt][vtnum] do
		if CalcFreeItemCellCount() > 10 then
			BOSSHK[vt][vtnum] = BOSSHK[vt][vtnum] - 1
			AddItem(4,random(itemmin,itemmax),0,0,0,0,0)
			--SaveNow()
			num = num + 1
		end
	end
	Msg2Player("B¹n nhËn ®­îc "..num.." "..name.." ")
	Msg2SubWorld("<color=yellow>Bang chñ "..GetName().." ®· rót "..num.." "..name.." ")
	inlog("Rut "..num.." "..name.."")
	
else
Talk(1,"","Ch­a lµm")	
end
LuuDanhSach()

end