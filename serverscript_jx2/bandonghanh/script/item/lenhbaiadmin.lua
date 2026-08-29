-- Author: Fong KiÒu
-- Date: 28/11/2020
-- Function: LÖnh bµi Admin

Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\item\\test_kinhmach_admin.lua");
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\log_game\\save_log.lua")
Include("\\script\\global\\admin\\char.lua")
Include("\\script\\global\\admin\\item.lua")
Include("\\script\\global\\admin\\point.lua")
Include("\\script\\global\\admin\\quanly.lua")
Include("\\script\\tinhnang\\boss_hoangkim\\lib_bosshk.lua")
Include("\\script\\tinhnang\\boss_satthu\\lib_boss_st.lua")
Include("\\script\\tinhnang\\vuot_ai\\lib_vuotai.lua")
Include("\\script\\tinhnang\\phonglangdo\\lib_phonglangdo.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")
-- Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")
-- Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")
-- Include("\\script\\global\\vatpham.lua")
-- Include("\\script\\event\\event_huyhoang_dungdb\\commonscript.lua")
-- Include("\\script\\event\\kiemmonquan\\lib_kmq.lua")
-- Include("\\script\\tinhnang\\congthanhchien\\danhsach_bang.lua")
-- Include("\\script\\tinhnang\\loidai\\lib_loidai.lua")
-- Include("\\script\\tinhnang\\datau\\lib_datau.lua")
-- Include("\\script\\event\\trongbanghoi\\lib.lua")
-- Include("\\script\\event\\event_vantieu\\lib_vt.lua")
-- Include("script\\global\\trangbixanh.lua")
-- Include("script\\global\\trangbihoangkim.lua")
 -- Include("\\script\\startgame.lua")
Include("\\script\\header\\testgame.lua")
Include("\\script\\test\\bangthanh_f.lua")	-- BO TEST bang hoi + cong thanh (TX_Root)
Include("\\script\\item\\datau_admin.lua")	-- Da Tau: xoa phat + them luot huy
Include("\\script\\item\\simcity_admin.lua")
Include("\\script\\item\\liendau_admin.lua")
Include("\\script\\item\\hoatdong_admin.lua")	-- [24/08] Bang Chien / Bach Nhan / Ty Vo / Thanh Bao: bo test + config	-- Liªn ®Êu: bé test ®Çy ®ñ (21/08)	-- SimCity: menu admin bot gia lap
Include("\\script\\item\\hd3_admin.lua")	-- [3HD 25/08] menu test 3 hoat dong Linux
Include("\\script\\item\\test_hoatdong_admin.lua")
Include("\\script\\item\\test_loren_admin.lua")
Include("\\script\\item\\bdh_admin.lua")	-- [BDH 28/08] bo test Dong hanh	-- [27/08] BO TEST LO REN (ep do tim / do pho)	-- [26/08] BO TEST TOAN BO HOAT DONG

ADMIN_SUPPORT_TEXT = "Chøc n¨ng hç trî qu¶n trÞ viªn"
PLAYER_INFOMATION = "<sex> - Tªn: <color=pink>%s<color>.\n- Täa ®é: <color=gold>%s (%d/%d) (%d/%d)<color>.\n- PIndex: <color=green>%d<color>"
NOW_END_SAY = "KÕt thóc ®èi tho¹i./no"

function main(itemIdx)
	
	dofile("script/item/lenhbaiadmin.lua")
	
	--local nPas = GetTask(T_PASSWORD)
	--if nPas < 3 then		-- gioi han 3 lan nhap sai pass
	--	manager()
	--return
	--end
	
	chucnangadmin(itemIdx)
	
	--local nW,nX,nY = GetWorldPos()
	--local nNpcIdx = AddNpcEx5({1000},100,{0,1,2,3,4},nW,nX*32,nY*32, nil,nil,"§éc C« Quû KiÕm",5,0,800000,nil,50,20,500,1000,nil,90,2)
	--local nNpcIdx = AddNpcEx3({NPC_VUOTAI[27][1]},100,NPC_VUOTAI[27][3],nW,nX*32,nY*32,nil,nil,nil,5,0,50000,nil,1000,150,200,500,nil,25)
	--AddNpcSkillState(nNpcIdx,16, LEVEL_SKILL_PHANDON, 1, 60*18*5)
	
end

function ktraadmin()
	OpenGetString("NhËp mËt khÈu","checkadmin")
end

function checkadmin()
	local nPassword = GetStringFromUI() 
	if(nPassword == "159753") then
		Msg2Player("MËt khÈu chÝnh x¸c!")
		SetTask(T_PASSWORD,0)
	else
		Msg2Player("Sai mËt khÈu!")
		SetTask(T_PASSWORD,GetTask(T_PASSWORD)+1)
		KickOutSelf()
	end
end

function chucnangadmin(itemIdx)
	local nAdmin = admincheck()
	local nName = GetName()
	local nW,nX,nY = GetWorldPos()
	local nMapName = GetMapName(nW)	
	if (nAdmin == 1) then
		SayEx({format(PLAYER_INFOMATION,nName,nW,nX*32,nY*32,nX,nY,PlayerIndex),
		--"Vßng b¶o vÖ 30s/vongbaove30s",
		-- "danhhieubinhsi/danhhieubinhsi",
		-- "del_danhhieubinhsi/del_danhhieubinhsi",
		--"vongsangliendau/vongsangliendau",
		-- "del_vongsangliendau/del_vongsangliendau",
		--"IgnoreState/IgnoreState",
		-- "LÊy M¸u/laymau001",
		"Qu¶n lý m¸y chñ/system",
		"Chøc n¨ng chÝnh/manager",
		"N¹p l¹i script/reloadfileEvent",
		"Test bang héi - c«ng thµnh/TX_Root",
		"BOT ng­êi ch¬i (KPlayer)/PB_Menu",
		"D· TÈu: xo¸ ph¹t - thªm l­ît/DT_AdminMenu",
		"Liªn ®Êu: bé test/LD_AdminMenu",
		"Bé test BC - BN - Tû Vâ - Thµnh B¶o/HD_AdminMenu",
		--"NhËn trang bÞ Xanh/xanhtest",
		--"LayBinhSiHieuPhu/LayBinhSiHieuPhu",
		--"Demo Set Npc Time Idle/DemoSetNpcTimeIdle",
		--"Show Npc Time Idle/ShowNpcTimeIdle",
		--"FKAddAllNpc/FKAddAllNpc",
		--"Go Map 957/GoMap995VT",
	"Ho¹t ®éng Linux: S¸t Thñ - PL§ - V­ît ¶i/HD3_AdminMenu",
		"Bé test Viªm §Õ B¶o Tµng/TTHD_Root",
		"Bé test lß rÌn/LR_Root",
		"Bé test Kinh M¹ch/KM_TestRoot",
		"Bé test §ång hµnh/BDH_Root",
		"KÕt thóc ®èi tho¹i/no"})
	else
		RemoveItem(itemIdx, 1)
		Talk(1,"","VËt phÈm chØ dµnh cho Admin.")
	end
end

function reloadfileEvent()
-- addfullnpc()

-- ReLoadScript("\\script\\event\\trongbanghoi\\drum.lua")
-- ReLoadScript("\\script\\event\\trongbanghoi\\tungtung1.lua")
-- ReLoadScript("\\script\\tinhnang\\phonglangdo\\bossthuytacdaulinhpld.lua")
-- ReLoadScript("\\script\\tinhnang\\phonglangdo\\dietrenthuyen.lua")
-- ReLoadScript("\\script\\tinhnang\\boss_hoangkim\\bosslbdeath.lua")
-- ReLoadScript("\\script\\tinhnang\\datau\\lib_datau.lua")
-- ReLoadScript("\\script\\item\\lenhbaitanthu.lua")
-- ReLoadScript("\\script\\player\\playerlogin.lua")
-- ReLoadScript("\\script\\missions\\fengling_ferry\\mission.lua")
-- ReLoadScript("\\script\\missions\\fengling_ferry\\shuizeideath.lua")
ReLoadScript("\\script\\item\\test_loren_admin.lua")
-- [PETSYS 28/08] nap lai tron bo he Ban Dong Hanh PC
ReLoadScript("\\script\\petsys\\jx1_compat.lua")
ReLoadScript("\\script\\skill\\petsys\\aura.lua")
ReLoadScript("\\script\\petsys\\protocol_process_gs.lua")
ReLoadScript("\\script\\petsys\\head.lua")
ReLoadScript("\\script\\petsys\\common.lua")
ReLoadScript("\\script\\petsys\\dataload.lua")
ReLoadScript("\\script\\petsys\\summon.lua")
ReLoadScript("\\script\\petsys\\levelup.lua")
ReLoadScript("\\script\\petsys\\name.lua")
ReLoadScript("\\script\\petsys\\feature.lua")
ReLoadScript("\\script\\petsys\\delete.lua")
ReLoadScript("\\script\\petsys\\transferexp.lua")
ReLoadScript("\\script\\petsys\\lang.lua")
ReLoadScript("\\script\\petsys\\petcard.lua")
ReLoadScript("\\script\\petsys\\feed.lua")
ReLoadScript("\\script\\petsys\\xiuzhen.lua")
ReLoadScript("\\script\\item\\compound\\compound_ui.lua")
ReLoadScript("\\script\\item\\itemvalue\\itemvalue_header.lua")
ReLoadScript("\\script\\item\\compound\\compound_header.lua")
ReLoadScript("\\script\\item\\compound\\magic_distill.lua")

-- ReLoadScript("\\script\\item\\ib\\shenxingfu.lua")
-- ReLoadScript("\\script\\npcthon\\balanghuyen\\hotrotanthu.lua")
-- ReLoadScript("\\script\\item\\tuiduocpham.lua")
-- ReLoadScript("\\script\\item\\tuiduocphamtk.lua")
-- ReLoadScript("\\script\\item\\battles\\sj_neiduwan.lua")
-- ReLoadScript("\\script\\item\\battles\\sj_bingfangwan.lua")
-- ReLoadScript("\\script\\item\\battles\\sj_pufangwan.lua")
-- ReLoadScript("\\script\\item\\battles\\sj_feisuwan.lua")
-- ReLoadScript("\\script\\item\\battles\\sj_gaozhongwan.lua")
-- ReLoadScript("\\script\\item\\battles\\sj_gaoshanwan.lua")
-- ReLoadScript("\\script\\item\\battles\\sj_jiapaowan.lua")
-- ReLoadScript("\\script\\item\\battles\\sj_changmingwan.lua")
-- ReLoadScript("\\script\\item\\battles\\sj_neidianwan.lua")
-- ReLoadScript("\\script\\item\\battles\\sj_neihuowan.lua")
-- ReLoadScript("\\script\\item\\battles\\sj_neibingwan.lua")
-- ReLoadScript("\\script\\item\\battles\\sj_neipuwan.lua")
-- ReLoadScript("\\script\\item\\battles\\sj_waibingwan.lua")
-- ReLoadScript("\\script\\item\\battles\\sj_waiduwan.lua")
-- ReLoadScript("\\script\\item\\battles\\sj_waipuwan.lua")
-- ReLoadScript("\\script\\item\\battles\\sj_leifangwan.lua")
-- ReLoadScript("\\script\\item\\battles\\sj_huofangwan.lua")
-- ReLoadScript("\\script\\item\\battles\\sj_dufangwan.lua")
-- ReLoadScript("\\script\\global\\station.lua")
-- ReLoadScript("\\script\\global\\npcchucnang\\lequan.lua")
-- ReLoadScript("\\script\\header\\forbidmap.lua")
-- ReLoadScript("\\script\\global\\LuaNpcMonsters\\Droprate_normal.lua")
-- ReLoadScript("\\script\\item\\tuitanthu.lua")
-- ReLoadScript("\\script\\item\\xisui-jing.lua")
-- ReLoadScript("\\script\\item\\wulin-miji.lua")
-- ReLoadScript("\\script\\global\\npcchucnang\\dichquan.lua")
-- ReLoadScript("\\script\\global\\npcchucnang\\thoren.lua")
-- ReLoadScript("\\script\\global\\npcchucnang\\nangcapngua.lua")
-- XepHangDuaTop()
-- XepHangDuaTop()
Msg2Player("§· reload thµnh c«ng  ")
end

mangtam_duatop = {}
NgauNhienPhut = 0
function XepHangDuaTop()

local nYr,nMo,nDy,nHr,nMi,nSe,nDyfW = GetTimeNow()	
--if nDy >= 01 and mod(nHr,1) == 0 and nMi == 22 then
--if mod(phut,2) == 1 then
mangtam_duatop = {}
for p=1,40 do 
	mangtam_duatop[getn(mangtam_duatop) + 1] = {"",0,0}
end
for k=1,GetPlayerCount() do
PlayerIndex = k
--	if GetLevel() < 90 then
		vt = check_min_mangtam_duatop_duatop()
		if (GetLevel() > mangtam_duatop[vt][2]) then
			mangtam_duatop[vt][1] = GetName()
			mangtam_duatop[vt][2] = GetLevel()
			mangtam_duatop[vt][3] = GetExp()
		elseif (GetLevel() == mangtam_duatop[vt][2]) then
			if (GetExp() > mangtam_duatop[vt][3]) then
				mangtam_duatop[vt][1] = GetName()
				mangtam_duatop[vt][2] = GetLevel()
				mangtam_duatop[vt][3] = GetExp()
			end
		--end
--	end
end

PlayerIndex = idx
SapXepHangDuaTop()
end





--if nDy >= 01 and mod(nHr,1) == 0 and nMi == 22 then
--if mod(phut,2) == 0 then
Msg2SubWorld("<color=yellow>Danh s¸ch Top Cao Thñ: ")
thoigian = tonumber(date("%H%M%d%m"))
local nIP = GetIP()
LoginLog = openfile("dulieu/LichSuDuaTop.txt", "a");	
NgauNhienPhut = RandomNew(1,59)
if LoginLog then
for u=1,getn(mangtam_duatop) do
	if mangtam_duatop[u][1] ~= "" then
		Msg2SubWorld(""..u..". ["..mangtam_duatop[u][1].."] - Lvl: "..mangtam_duatop[u][2].." - Exp: "..mangtam_duatop[u][3].."")
		write(LoginLog,""..mangtam_duatop[u][1].." - "..mangtam_duatop[u][2].." - "..mangtam_duatop[u][3].." - Time: "..thoigian.." - IP: "..nIP.."\n");
	end
end
--end


closefile(LoginLog)
end
end

-- mangtam_duatop = {}

function check_min_mangtam_duatop_duatop()
nlevel = mangtam_duatop[1][2]
nexp = mangtam_duatop[1][3]
vt_min = 1
for i=2,getn(mangtam_duatop) do
	if nlevel > mangtam_duatop[i][2] then	
		nlevel = mangtam_duatop[i][2]
		nexp = mangtam_duatop[i][3]
		vt_min = i
	elseif nlevel == mangtam_duatop[i][2] then
		if nexp > mangtam_duatop[i][3] then
			nlevel = mangtam_duatop[i][2]
			nexp = mangtam_duatop[i][3]
			vt_min = i
		end	 
	end
end
return vt_min
end

tam_duatop = {"",0,0}
function SapXepHangDuaTop()
for i=1,getn(mangtam_duatop) do
	for j=1,getn(mangtam_duatop) do
		if mangtam_duatop[i][2] > mangtam_duatop[j][2] then
			tam_duatop = mangtam_duatop[i]
			mangtam_duatop[i] = mangtam_duatop[j]
			mangtam_duatop[j] = tam_duatop
		elseif mangtam_duatop[i][2] == mangtam_duatop[j][2] then
			if mangtam_duatop[i][3] > mangtam_duatop[j][3] then
				tam_duatop = mangtam_duatop[i]
				mangtam_duatop[i] = mangtam_duatop[j]
				mangtam_duatop[j] = tam_duatop
			end
		end
	end
end

end
function laymau001()
	local limitm = 50
	local timeexxp = 60*60*24*7
	for i=0,limitm do
		AddItem(1, 2, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, timeexxp, 0, 0, 0)
	end
	Talk(1,"","Xin chóc mõng <sex> ®· nhËn ®­îc "..limitm.." b×nh d­îc phÈm.")
end
function ai30()
	NewWorld(957, floor(51072/32), floor(102272/32))
end

function FKAddAllNpc()
	local tbOption = {}
	tinsert(tbOption, "Lùa chän add npc theo ID")
	tinsert(tbOption, format("Add Npc ID 1 den 50/#FKAddAllNpcOK(%d)", 1))
	tinsert(tbOption, format("Add Npc ID 50 den 100/#FKAddAllNpcOK(%d)", 50))
	tinsert(tbOption, format("Add Npc ID 100 den 150/#FKAddAllNpcOK(%d)", 100))
	tinsert(tbOption, format("Add Npc ID 150 den 200/#FKAddAllNpcOK(%d)", 150))
	tinsert(tbOption, format("Add Npc ID 200 den 250/#FKAddAllNpcOK(%d)", 200))
	tinsert(tbOption, format("Add Npc ID 250 den 300/#FKAddAllNpcOK(%d)", 250))
	tinsert(tbOption, format("Add Npc ID 300 den 350/#FKAddAllNpcOK(%d)", 300))
	tinsert(tbOption, format("Add Npc ID 350 den 400/#FKAddAllNpcOK(%d)", 350))
	tinsert(tbOption, format("Add Npc ID 400 den 450/#FKAddAllNpcOK(%d)", 400))
	tinsert(tbOption, format("Add Npc ID 450 den 500/#FKAddAllNpcOK(%d)", 450))
	tinsert(tbOption, "FKAddAllNpc2/FKAddAllNpc2")
	tinsert(tbOption, "KÕt thóc/no")
	SayEx(tbOption)
end

function FKAddAllNpc2()
	local tbOption = {}
	tinsert(tbOption, "Lùa chän add npc theo ID")
	tinsert(tbOption, "FKAddAllNpc1/FKAddAllNpc")
	tinsert(tbOption, format("Add Npc ID 50 den 100/#FKAddAllNpcOK(%d)", 500))
	tinsert(tbOption, format("Add Npc ID 100 den 150/#FKAddAllNpcOK(%d)", 550))
	tinsert(tbOption, format("Add Npc ID 150 den 200/#FKAddAllNpcOK(%d)", 600))
	tinsert(tbOption, format("Add Npc ID 200 den 250/#FKAddAllNpcOK(%d)", 650))
	tinsert(tbOption, format("Add Npc ID 250 den 300/#FKAddAllNpcOK(%d)", 700))
	tinsert(tbOption, format("Add Npc ID 300 den 350/#FKAddAllNpcOK(%d)", 750))
	tinsert(tbOption, format("Add Npc ID 350 den 400/#FKAddAllNpcOK(%d)", 800))
	tinsert(tbOption, format("Add Npc ID 400 den 450/#FKAddAllNpcOK(%d)", 850))
	tinsert(tbOption, format("Add Npc ID 450 den 500/#FKAddAllNpcOK(%d)", 900))
	tinsert(tbOption, format("Add Npc ID 950 den 1000/#FKAddAllNpcOK(%d)", 950))
	tinsert(tbOption, "KÕt thóc/no")
	SayEx(tbOption)
end

function FKAddAllNpcOK(nIdStart)
	local nW,nX,nY = GetWorldPos()
	local nCount = 0
	for i = nIdStart, nIdStart+50 do
		nCount = nCount + 1
		if(nCount < 25) then
			AddNpcEx1({i},1,nil,nW,(nX+nCount+1)*32,nY*32,nil,DOSCRIPTCOSOAI,format("%d",i),6)
		else
			AddNpcEx1({i},1,nil,nW,(nX+nCount-25+1)*32,(nY+5)*32,nil,DOSCRIPTCOSOAI,format("%d",i),6)
		end
	end
end

function LayBinhSiHieuPhu()
	AddItem(6,1,157,0,0,0)
end

function DemoSetNpcTimeIdle()
	SetNpcTimeIdle(60)
end

function ShowNpcTimeIdle()
	local nTime = GetNpcTimeIdle()
	Msg2Player("Thêi gian "..nTime.." gi©y")
end

function vongbaove30s()
	AddSkillState(963, 1, 0, 18*30, -1) 
	SetProtectTime(18*30)	
end

function TestViewVuotAi()

end

function demoSaveData()
	creattong(PlayerIndex)
	Say("§· save d÷ liÖu bang C«ng Thµnh.",0)
end

function demoViewInfoPheThu()
	local nTongID = BANGHOI_DANHSACH[1][1] 
	local nTongName = BANGHOI_DANHSACH[1][2] 
	local nTongMasterName = BANGHOI_DANHSACH[1][3]
	Talk(1,"",format("HiÖn t¹i thµnh <color=green>T­¬ng D­¬ng<color> bang <color=yellow> %s <color> ®ang thèng trÞ víi thñ lÜnh <color=pink> %s <color> ng­¬i muèn tranh ®o¹t th× ®îi ®Õn <color=red> thø 5 lóc 19h00 <color> gÆp C«ng Thµnh Quan ®Ó b¸o danh.", nTongName, nTongMasterName))
end

function demoUpdatePheCong()
	local nTongMasterName = GetTongInfo(2)
	local nTongID = GetTongInfo(0)
	local nTongName = GetTongInfo(1)
	local nAwd = 0
	local nThue = 0
	BANGHOI_DANHSACH[2] = {nTongID,nTongName,nTongMasterName,nAwd,nThue}
	local tbDataTongNew = TaoBang(BANGHOI_DANHSACH,"BANGHOI_DANHSACH","")
	SaveData("script/tinhnang/congthanhchien/danhsach_bang.lua",tbDataTongNew)
	Say("§· cËp nhËt d÷ liÖu Phe C«ng Thµnh.",0)	
end

function demoUpdatePheThu()
	local nTongMasterName = GetTongInfo(2)
	local nTongID = GetTongInfo(0)
	local nTongName = GetTongInfo(1)
	local nAwd = 0
	local nThue = 0
	BANGHOI_DANHSACH[1] = {nTongID,nTongName,nTongMasterName,nAwd,nThue}
	local tbDataTongNew = TaoBang(BANGHOI_DANHSACH,"BANGHOI_DANHSACH","")
	SaveData("script/tinhnang/congthanhchien/danhsach_bang.lua",tbDataTongNew)
	Say("§· cËp nhËt d÷ liÖu Phe Thñ Thµnh.",0)
end

function demoClearData()
	BANGHOI_DANHSACH = {}
	local tbDataTongNew = TaoBang(BANGHOI_DANHSACH,"BANGHOI_DANHSACH","")
	SaveData("script/tinhnang/congthanhchien/danhsach_bang.lua",tbDataTongNew)
	Say("§· xo¸ toµn bé d÷ liÖu C«ng Thµnh ChiÕn.",0)
end

function creattong(nPlayerIndex)
	PlayerIndex = nPlayerIndex
	local nTongMasterName = GetName()
	local nTongID = GetTongInfo(0)
	local nTongName = GetTongInfo(1)
	local nThuong = 0
	local nThue = 0
	capnhat_danhsach(nTongMasterName,nTongID,nTongName,nThuong,nThue)
end

function capnhat_danhsach(nTongMasterName,nTongID,nTongName,nThuong,nThue)
	local nSTT = FindTongInData(BANGHOI_DANHSACH, nTongID)
	if nSTT ~= nil then
		BANGHOI_DANHSACH[nSTT] = {nTongID,nTongName,nTongMasterName,nThuong,nThue}
	else
		nSTT = getn(BANGHOI_DANHSACH)+1
		BANGHOI_DANHSACH[nSTT] = {nTongID,nTongName,nTongMasterName,nThuong,nThue}
	end
	
	local tbDataTongNew = TaoBang(BANGHOI_DANHSACH,"BANGHOI_DANHSACH","")
	SaveData("script/tinhnang/congthanhchien/danhsach_bang.lua",tbDataTongNew)
	return nSTT
end

function FindTongInData(tbTongData, TongID)
	if getn(tbTongData) == 0 then
		return nil
	end
	for i= 1,getn(tbTongData) do
		if tbTongData[i][1] == TongID then
			return i
		end
	end
end

function danhhieubinhsi()
	local idRankBinhSi = 5
	local nTimeDay = -1
	local overLook = 1 --not save khi logout
	SetRankBattle(idRankBinhSi, nTimeDay, overLook)
end

function del_danhhieubinhsi()
	RemoveRankBattle()
end

function vongsangliendau()
	local nTimeDay = 30*24*60*1080
	SetPlayerTitle(215,nTimeDay, 0)
end

function del_vongsangliendau()
	--SetPlayerTitle(GetTask(T_TITLE_ID),1,1)
	RemovePlayerTitle()
end

function ReSetMask643()
	ReSetMask()
end

function SetMask643()
	SetMask(1448)
end

function themnpc()
	SetHorse(33)
end

function manager()
	SayEx({ADMIN_SUPPORT_TEXT,
	"Thö nghiÖm tÝnh n¨ng/hamlinhtinh",
	--"NhËn kü n¨ng/hskill",
	"NhËn trang bÞ/laydoxanh0",
	"NhËn ®iÓm/hotro",
	"NhËn vËt phÈm/nhanvatpham",
	--"Qu¶n lý server/system",
	NOW_END_SAY})
end

function hamlinhtinh()
	SayEx({ADMIN_SUPPORT_TEXT,
	"Tong kim/AdminTestTK",
	"Test Phong L¨ng §é/admintestPLD",
	"TÝnh n¨ng Boss/admintestboss",
	"Test Boss TiÓu/addnpcbosstieuhk",
	"Test Boss §¹i/addnpcbossdaihk",
	"Test Boss Server/addnpcbossserver",
	"Test Boss PLD/addnpcbosspld",
	NOW_END_SAY})
end
