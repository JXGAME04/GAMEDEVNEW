-- [LOCAL54 06/09 toi] 3 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local nosave, trumeridian, thanhcongvaopubg
--Author: Fong KiÒu
--Date: 07/07/2021
--Function: Hç trî t©n thñ

Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_vatpham.lua")
Include("script\\global\\trangbixanh.lua")
Include("script\\global\\trangbihoangkim.lua")
Include("script\\global\\trangbisieunhan.lua")
Include("script\\global\\hocvocong.lua")
Include("script\\global\\npcchucnang\\lapbang.lua")
Include("script\\global\\vatpham.lua")
Include("script\\global\\skills_table.lua")
Include("script\\global\\npcchucnang\\phantang.lua")--tÈy tuû
Include("\\script\\log_game\\save_log.lua")
Include("\\script\\event\\kiemmonquan\\lib_kmq.lua")
Include("script\\header\\testgame.lua")
Include("script\\test\\npcchat.lua")

Include("\\script\\tinhnang\\pubg\\pubg.lua")
Include("\\script\\tinhnang\\pubg\\pubgutils.lua")
-- Include("\\script\\header\\factionhead.lua")
Include("\\script\\lib\\lib_task.lua")

function main(sel)

	dofile("script/npcthon/balanghuyen/hotrotest.lua")

	SayEx({"<color=green><npc>: <color>Chèn th«n lµng yªn tÜnh nµy, kh«ng muèn cã sù quÊy ph¸. <sex> ®Õn ®©y ®Ó lµm g× ?",
	-- "NhËn hç trî t©n thñ/nhanhotrotanthu",
--	"NhËn mÆt n¹/addmatnatest",
	"LËp bang/main2",
	"Tham gia PUBG/thachthuc",
	"NhËn ngùa/nhanngua2",
	"Häc vâ c«ng/hocvocong",
	"NhËn tiÒn v¹n/money",
	-- "NhËn HKMP/NhanHkmp",
	"NhËn Nguyªn LiÖu M¹ch/kinhmach",
	"NhËn vËt phÈm/layvatpham",
	"NhËn Hoµng Kim/laydohoangkim",
	-- "NhËn B¹ch Kim/NhanBachKims",
	"NhËn trang bÞ Xanh/xanhtest",
	"Th¨ng cÊp vµ kinh nghiÖm /nhanNcap",
--	"NhËn Ngò Hoa Ngäc Lé Hoµn/laymau",	
	"Trang 2/checkAdminP2",
--	"§i KiÕm M«n Quan/GoMap995",
	"Ta chØ ®Õn th¨m «ng./no"})
	
end

function thachthuc()
	--local nLanVADay = GetTask(T_NVVATRONGNGAY) + 1
	Say("<npc>: Tham gia PUBG khong?",2,
	"Tham gia PUBG/khieuchien",
	"Kh«ng tham gia/no")
end

function khieuchien()	
	local nMapNo = 0
	local tasskTemp = GetTaskTemp(TMP_INDEX_NPC)
	local nParam = GetNpcValue(tasskTemp)
	
	if (GetGlbMissionV(nParam) == 0) then --
		nMapNo = nParam
	end
	

	local nSubWorldId = SubWorldID2Idx(BIENKINHPUBG)
	if nSubWorldId < 0 then	--chua mo map, ngung ham`
		Talk(1,"", "Xin Lçi ! Khu vùc Pubg ch­a më !")
		return 
	end
	
	SubWorld = nSubWorldId
	local nRestTime = GetMSRestTime(MS_PUBG,14)
	if(IsMission(MS_PUBG) == 0 or nRestTime <= 0) then
		Talk(1,"", "Xin lçi PUBG ch­a tíi thêi gian b¸o danh ! .")--thoi gian chua toi hoac qua gio`
		return 
	end

	thanhcongvaopubg()
end

function save()
	local idx = PlayerIndex
	SetPlayerSaveState(PlayerIndex, 1)
end

function nosave()
	local idx = PlayerIndex
	SetPlayerSaveState(PlayerIndex, 0)
end

function trumeridian()
for i = 0,8 do 
	local nMeridianType = i;
	local nMeridianLevel = 0;
	SetMeridian(nMeridianType, nMeridianLevel);
end
end

function thanhcongvaopubg()
	local nParam = GetNpcValue(GetTaskTemp(TMP_INDEX_NPC))
	local nSubWorldId = SubWorldID2Idx(BIENKINHPUBG)
	local w = 997 --sanh cho`
	local x = 55232
	local y = 99200
	
	if nSubWorldId < 0 then	--chua mo map, ngung ham`
		Talk(1,"", "<npc>: Xin Lçi ! Khu vùc Pubg ch­a më !")
		return 
	end	
	SubWorld = nSubWorldId
	local nPIndex
	local nPlayerDataIdx
	
	SetTask(T_NVPUBGTRONGNGAY, GetTask(T_NVPUBGTRONGNGAY) + 1)
	SetTaskTemp(TMP_MAP_PUBG, nParam)
	
	--Pay(MONEYNHANNV)
	StopTimer()	--ngung hen gio dang chay bat ky
	
	nosave()
	if(NewWorld(w,floor(x/32),floor(y/32)) > 0) then
		--start manipulate
		SubWorld = SubWorldID2Idx(BIENKINHPUBG)
		local nPlayerDataIdx = PIdx2MSDIdx(MS_PUBG,PlayerIndex)
		nPlayerDataIdx = AddMSPlayer(MS_PUBG,0) --group 0
		-- print(SubWorld.. " "..GetMSPlayerCount(MS_PUBG))
		
		SetPMParam(MS_PUBG,nPlayerDataIdx,MISSION_BR_PARAM_JOINED,1);
		
		SetPMParam(MS_PUBG, nPlayerDataIdx, MISSION_BR_PARAM_KILLS, 0);
		
	
		SetDeathScript("\\script\\tinhnang\\pubg\\pubgdeath.lua")
		--remove all Equip
		RemoveAllItem()
		--Set Trang thai PK
		SetFightState(1) 
		SetPKMode(1,1)
		SetLevel(90)
		DelAllMagic() 
		ForceClearStateSkillEffect()
		trumeridian()
		ResetAP()
		--lay it mau
		laymau()
		-- AddItemSL(4821,1,-2)
		SetProtectTime(18*300)
		AddSkillState(963, 1, 0, 18*300)
		--TODO Add skills
		--1. Check phai
		local player_Faction = GetFaction();
		if (player_Faction == "") then
			-- print("Chua gia nhap phai")
			series = GetSeries()
			local tbPh = {series*2, series*2+1}	-- [HOASON 01/09c] he Thuy (2) co them Hoa Son (10)
			if (series == 2) then tinsert(tbPh, 10) end
			if (series == 3) then tinsert(tbPh, 11) end	-- [VHTD 02/09] he Hoa co them Vu Hon (11)
			if (series == 4) then tinsert(tbPh, 12) end	-- [VHTD 02/09] he Tho co them Tieu Dao (12)
			gianhapmonphai(tbPh[random(1, getn(tbPh))])
		end
		--2. Add skills
		
		local nCurFac = GetFirstAddFaction();
		hockynangpubg(nCurFac+1)
		hockhinhcong()
		local nW,nX,nY = GetWorldPos()
		local nYr,nMo,nDy,nHr,nMi,nSe,nDyfW = GetTimeNow()	
		
		local nTimerID = 12 --pubg.lua trong timertask
		SetTimer(30*18,nTimerID) --30s chay ham OnTimer Report Time
		local nRestTime = GetMSRestTime(MS_PUBG,1)
		
		SetCurCamp(4) --sat thu
		SetCamp(4)
		-- Msg2SubWorld(format("Cßn %d gi©y sÏ chÝnh thøc b¾t ®Çu pubg.",floor(nRestTime/18))
	else
		Talk(1,"", "<npc>: Kh«ng thÓ vµo map PUBG!")
	end
	
	
end
function ResetJN()

	      local  a = GetLevel()       
          local  p = GetRestSP()          
		  local  c = RollbackSkill(0)		
          local  h = GetTask(T_VLMT)			
		  local  r = GetReBornJPoint()  
		  local  n = GetReBornQPoint()   
		  AddMagicPoint(a-p-1+h+r,1)
         			   
          KickOutSelf()                 
end;
function ResetAP()

local as={
{35,25,25,15},
{20,35,20,25},
{25,25,25,25},
{30,20,30,20},
{20,15,25,40},
}

player_series = GetSeries() + 1;
SetBasePoint(as[player_series][1],as[player_series][2],as[player_series][3],as[player_series][4]);  

local nXiSuiPonit=GetTask(T_TTK)*5        
AddProp(nXiSuiPonit)

ResetBaseAttrib(0,as[player_series][1])
ResetBaseAttrib(1,as[player_series][2])
ResetBaseAttrib(2,as[player_series][3])
ResetBaseAttrib(3,as[player_series][4])

end;
function kinhmach()
 AddItemSL(4844,5000,0)
 AddItemSL(4847,500,0)
 AddItemSL(4848,500,0)
 AddItemSL(4849,500,0)
end

function hotrocap10()
	local nLevel = 80
	local maxLevel = 80
	if(GetLevel() + nLevel > maxLevel) then
		nLevel = maxLevel - GetLevel()
		LEVERUP(nLevel)
	else
		LEVERUP(nLevel)
	end
	if(nLevel > 0) then
		Talk(1,"","Xin chóc mõng <sex> ®· t¨ng thªm "..nLevel.." cÊp")
	end
end

function LEVERUP(nLevel)
	for i = 1, nLevel do 
		AddOwnExp(100000000000)
	end
end

function checkAdminP2()
	SayEx({"<color=green><npc>: <color>Chèn th«n lµng yªn tÜnh nµy, kh«ng muèn cã sù quÊy ph¸. <sex> ®Õn ®©y ®Ó lµm g× ?",
	"Trang 1/main",
	"NhËn ®iÓm/nhandiemgm",
	"Häc Khinh c«ng/hockhinhcong",
	-- "Xem s¸ch vë ®· häc/sachvodahoc",
	"§æi mµu PK/thaydoimau",
	format("NhËn tiÒn xu/#TestGetNumCoin(%d)", 100000),
	"TÈy ®iÓm kü n¨ng/DoClearSkill",
	"TÈy ®iÓm tiÒm n¨ng/DoClearProp", 
	"KÕt thóc!/no"})
end

function nhanNcap()
	SayEx({"<npc>: Chµo <player> nhËn cÊp",
	"T¨ng 10 cÊp/#tangncap(10)",
	"T¨ng 50 cÊp/#tangncap(50)",
	"T¨ng 150 cÊp/#tangncap(150)",	
	"NhËn ®iÓm EXP/tangexp",
	"Trë l¹i/main"})
end

function nhandiemgm()
	SayEx({"Chµo <player> nhËn ®iÓm: ",
	"Tµi L·nh §¹o/TaiLanhDao",
	"§iÓm danh väng/ThemDanhVong",
	"§iÓm phóc duyªn/ThemPhucDuyen",
	"NhËn §iÓm Exp Bang Héi /expbanghoi",
	 "§iÓm tiÒm n¨ng/ThemTiemNang",
	 "§iÓm kü n¨ng/ThemKyNang",
	"KÕt thóc!/no"})
end
function expbanghoi()
local nExp = GetTongExp()
SetTongExp(nExp + 10000000)

UpdateTongExpAndLevel()
end
function nhanngua2()
	SayEx({"Xin chµo <sex> muèn lÊy lo¹i thÇn m· nµo ?",
	"¤ v©n ®¹p tuyÕt/ovandaptuyet",
	"XÝch thè/xichtho",
	"TuyÕt ¶nh/tuyetanh",
	"§Ých l«/dichlo",
	"Sieu Quang/sieuquan",
	"ChiÕu d¹ ngäc s­ tö/chieudangocsutu",
	"KÕt thóc ®èi tho¹i/no"})
end

function addmatnatest()
	local nrandom = random(1, 829)
	local nrandom2 = nrandom + 10
	for i = nrandom, nrandom2 do
		AddItem(0,11,0,i,0,0,0)
	end
end

function ovandaptuyet()
	AddItem(0,10,5,6,0,0,10)
end

function xichtho()
	AddItem(0,10,5,7,0,0,10)
end

function tuyetanh()
	AddItem(0,10,5,8,0,0,10)
end

function dichlo()
	AddItem(0,10,5,9,0,0,10)
end

function chieudangocsutu()
	AddItem(0,10,5,10,0,0,10)
	-- AddItem(0,10,11,10,0,0,10)
end
function sieuquan()
	AddItem(0,10,11,10,0,0,10)
end

function bontieu()
	AddItem(0,10,6,10,0,0,10)
end

function phieuvu()
	AddItem(0,10,7,10,0,0,10)
end

function sachvodahoc()
	Msg2Player("Häc tÈy tuû kinh sè lÇn: "..GetTask(T_TTK))
	Msg2Player("Häc vâ l©m mËt tÞch sè lÇn: "..GetTask(T_VLMT))
end

function thaydoimau()
	local tbOption = {}
	tinsert(tbOption, "Xin mêi <sex> chän:")
--	tinsert(tbOption, "Ch÷ tr¾ng/#fkchangecamp(0)")
	tinsert(tbOption, "Ch¸nh ph¸i/#fkchangecamp(1)")
	tinsert(tbOption, "Tµ ph¸i/#fkchangecamp(2)")
	tinsert(tbOption, "Trung lËp/#fkchangecamp(3)")
	tinsert(tbOption, "S¸t thñ/#fkchangecamp(4)")
	-- tinsert(tbOption, "S¸t thñ 2/#fkchangecamp(5)")
	--tinsert(tbOption, "Ch÷ hång/#fkchangecamp(6)")
	tinsert(tbOption, "KÕt thóc/no")
	SayEx(tbOption)
end

function fkchangecamp(n_Camp)
	SetCurCamp(n_Camp)
	SetCamp(n_Camp)
	-- if(n_Camp == 4) then
		-- SetCamp(n_Camp)
		-- SetFaction("")	
	-- end
end

function TestGetNumCoin(numxu)
	SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) + numxu)
	Msg2Player("Kh¸ch quan ®· nhËn ®­îc "..numxu.."  Xu.")	
end

function TaiLanhDao()
	AddLeadExp(1000000000)
	Talk(1,"","Xin chóc mõng <sex> ®· nhËn ®­îc ®iÓm l·nh ®¹o .")
end

function ThemPhucDuyen()
	AddFuYuan(1000)
	Talk(1,"","Xin chóc mõng <sex> ®· nhËn ®­îc 1000 ®iÓm phóc duyªn .")
end

function ThemDanhVong()
	AddRepute(1000)
	Talk(1,"","Xin chóc mõng <sex> ®· nhËn ®­îc 1000 ®iÓm danh väng .")
end

function laymau()
	local limitm = 20
	local timeexxp = 60*60*24*7
	for i=0,limitm do
		AddItem(1, 2, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, timeexxp, 0, 0, 0)
	end
	Talk(1,"","Xin chóc mõng <sex> ®· nhËn ®­îc "..limitm.." b×nh d­îc phÈm.")
end

function tangexp()
	local nMaxLevel = 151
	if(GetLevel() > nMaxLevel) then
		Say("Vâ c«ng cña vÞ <sex> ®· ®¹t ®¼ng cÊp th­îng thõa kh«ng thÓ nhËn thªm ®iÓm", 0)
		return
	end
	local nExpAdd = 1000000000
	AddOwnExp(nExpAdd)
	Talk(1,"","Xin chóc mõng <player> ®· nhËn ®­îc "..nExpAdd.." ®iÓm kinh nghiÖm")
end

function money()
	Earn(1000000000)
	Talk(1,"","<sex> nhËn ®­îc 1.000 v¹n l­îng.")
end

function tangncap(nLevel)
	local maxLevel = 200
	if(GetLevel() + nLevel > maxLevel) then
		nLevel = maxLevel - GetLevel()
		LEVERUP(nLevel)
	else
		LEVERUP(nLevel)
	end
	if(nLevel > 0) then
		Talk(1,"","Xin chóc mõng <sex> ®· t¨ng thªm "..nLevel.." cÊp")
	else
		Talk(1,"","<sex> ®· ®¹t ®­îc ®¼ng cÊp th­îng thõa råi")
	end
end

function LEVERUP(nLevel)
	for i = 1, nLevel do 
		AddOwnExp(100000000000)
	end
end

function KickOutAllClient()
	Msg2Player("Kich toan bo ra khoi server")
	local PlayerIndexOld = PlayerIndex
	for i=1, MAX_PLAYER do
		PlayerIndex = i
		if(PlayerIndexOld ~= PlayerIndex) then
			KickOutSelf2(PlayerIndex)
		end
	end
	PlayerIndex = PlayerIndexOld
	Msg2Player("Kich toan bo ra khoi server Completed")
end

function no()
end

Include("\\script\\test\\npcchat.lua")
-- Constants


function OnTimer(nNpcIndex, nTimeOut)
	NpcChatWalkTest(nNpcIndex)
	SetNpcTimer(nNpcIndex, 18 * timer_sec_chat)
end


