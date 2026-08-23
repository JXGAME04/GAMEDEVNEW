--Author: Fong KiÒu
--Date: 2021
--Function: Add Npc All Server
Include("\\script\\lib\\worldlibrary.lua");
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_server.lua")
Include("\\script\\startgame\\traplib.lua")
Include("\\script\\tinhnang\\boss_satthu\\lib_boss_st.lua")
Include("\\script\\tinhnang\\boss_hoangkim\\lib_bosshk.lua")
Include("\\script\\tinhnang\\phonglangdo\\lib_phonglangdo.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")
Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")
Include("\\script\\missions\\leaguematch\\wlls_autoexec.lua")	-- [WLLS port 20/08/2026] thay he lien_dau tu viet
Include("\\script\\event\\event_vantieu\\lib_vt.lua")
--Include("\\script\\startgame\\luyencong\\maccaoquat.lua")
Include("\\script\\event\\kiemmonquan\\lib_kmq.lua")
Include("\\script\\task\\tollgate\\tinsu_addnpc.lua")	-- [TIN SU 21/08] thay tinhnang\thienbaokho (Ken Nguyen) bang ban Linux
Include("\\script\\missions\\tongwar\\tongwar_autoexec.lua")	-- [TONGWAR 23/08] NPC/trap 6 map bao danh + NPC loi vao Bang Chien
Include("\\script\\missions\\bairenleitai\\bairen_boot.lua")	-- [BAIREN 23/08] Bach Nhan Loi Dai
Include("\\script\\startgame\\tongjx2npc.lua")	--NPC bang hoi JX2
Include("\\script\\startgame\\traplib.lua");
Include("\\script\\startgame\\thon\\balanghuyen.lua");
Include("\\script\\startgame\\thon\\giangtanthon.lua");
Include("\\script\\startgame\\thon\\longmontran.lua");
Include("\\script\\startgame\\thon\\namnhactran.lua");
Include("\\script\\startgame\\thanh\\tuongduong.lua");
Include("\\script\\startgame\\thanh\\bienkinh.lua");
Include("\\script\\startgame\\thanh\\daily.lua");
Include("\\script\\startgame\\thanh\\duongchau.lua");
Include("\\script\\startgame\\thanh\\laman.lua");
Include("\\script\\startgame\\thanh\\thanhdo.lua");
Include("\\script\\startgame\\thanh\\phuongtuong.lua");
Include("\\script\\startgame\\tinhnang\\daotaytuy.lua");
--Include("\\script\\startgame\\tinhnang\\bdtongkimcao.lua");
--Include("\\script\\startgame\\tinhnang\\dienvotruong.lua");
Include("\\script\\startgame\\khac\\satthu.lua");--boss sat thu
Include("\\script\\startgame\\khac\\othermap.lua");--add npc linh tinh
--Include("\\script\\startgame\\khac\\npcnhiemvu.lua");--add npc linh tinh
Include("\\script\\startgame\\monster\\trainingmap.lua");--add quai' luyen cong
Include("\\script\\test\\npcwalk.lua")
Include("\\script\\test\\npcchat.lua")

Include("\\script\\tinhnang\\pubg\\pubgaddnpc.lua");
Include("\\script\\tinhnang\\pubg\\pubgutils.lua")
Include("\\script\\test\\event_rank.lua")
Include("\\script\\startgame\\citywar_boot.lua")	-- DOT E (E5): boot cong thanh JX2

function OnGame()
	dofile("script/startgame/tongjx2npc.lua")
	JX2Tong_AddTerritoryNpc()
	CityWar_Boot()	-- DOT E (E5): league 4/508/509 + OpenGlbMission(8)
	local nYr,nMo,nDy,nHr,nMi,nSe,nDyfW = GetTimeNow()	
	print("[====== Lua Script StartGame!!! ======]")
	--JX2Tong_AddNpc()	--NPC bang hoi JX2 (TAT theo yeu cau - lam sau, tap trung client)
	local nNpcIdx
	--nNpcIdx = AddNpcEx1({198},1,nil,53,1582*32,3151*32,"",OTHER_THOREN,"Thî rÌn",6)
	
	
	
	
	
	
	
	if(SERVER_TEST == 1) then
		local nNpcIdxOne = AddNpcEx1({1596},1,nil,53,1632*32,3179*32,"","\\script\\npcthon\\balanghuyen\\hotrotest.lua","Hç Trî Test",6)
		SetNpcTimer(nNpcIdxOne, 18 * timer_sec_chat)
		nNpcIdx = AddNpcEx1({1596},1,nil,53,1619*32,3176*32,"","\\script\\npcthon\\balanghuyen\\hotrotest.lua","Hç Trî Test Game",6)
		SetNpcTimer(nNpcIdx, 18 * timer_sec_chat)
	else
		local nNpcIdxOne = AddNpcEx1({1596},1,nil,53,1632*32,3179*32,"","\\script\\npcthon\\balanghuyen\\hotrotanthu.lua","Hç Trî T©n Thñ.",6)
		SetNpcTimer(nNpcIdxOne, 18 * timer_sec_chat)
		nNpcIdx = AddNpcEx1({1596},1,nil,53,1619*32,3176*32,"","\\script\\npcthon\\balanghuyen\\hotrotanthu.lua","Hç Trî T©n Thñ",6)
		SetNpcTimer(nNpcIdx, 18 * timer_sec_chat)
	end

	-- AddNpcNew(305,1,53,1621*32,3184*32,"\\script\\event\\30thang4\\tuongquan.lua",6,"NPC EVENT GAME")
	nNpcIdx = AddNpcEx1({832},1,nil,176,1412*32,3312*32,"","\\script\\npcthanhthi\\laman\\volamminhchu.lua","Vâ L©m Minh Chñ",6) --vo lam minh chu lam an
	nNpcIdx = AddNpcEx1({95},1,nil,78,1603*32,3230*32,"","\\script\\npcthanhthi\\tuongduong\\thantuclaonhan.lua","Th¸n Tøc L·o Nh©n",6) --than tuc lao nhan
	----NPC Chuc nang phe Tong-----

	nNpcIdx = AddNpcNew(62,1,324,1550*32,3179*32,"\\script\\startgame\\tinhnang\\tongkim\\song_signup.lua",6,"Tèng Binh B¸o Danh");SetNpcValue(nNpcIdx, 1);--value 1 lµ bªn Tèng
	nNpcIdx = AddNpcNew(235,1,324,1535*32,3153*32,"\\script\\startgame\\tinhnang\\tongkim\\song_transport.lua",6,42);SetNpcValue(nNpcIdx, 1);
	nNpcIdx = AddNpcNew(55,1,324,1546*32,3158*32,"\\script\\startgame\\tinhnang\\tongkim\\song_shop.lua",6,881);SetNpcValue(nNpcIdx, 1);
	nNpcIdx = AddNpcNew(625,1,324,1561*32,3194*32,"\\script\\global\\npcchucnang\\ruongchua.lua",6); SetNpcValue(nNpcIdx, 1);
	--nNpcIdx = AddNpcNew(55,1,379,38930,113544,"\\script\\startgame\\tinhnang\\tongkim\\tongkimts.lua",6,"Tèng Qu©n tiÒn ph­¬ng ®Æc th¸m");SetNpcValue(nNpcIdx, 1);

	
	----NPC Chuc nang phe Kim-----

	nNpcIdx = AddNpcNew(61,1,324,1555*32,3082*32,"\\script\\startgame\\tinhnang\\tongkim\\jin_signup.lua",6,"Kim Binh B¸o Danh");--value 0 (mÆc ®Þnh) lµ bªn Kim
	nNpcIdx = AddNpcNew(235,1,324,1568*32,3075*32,"\\script\\startgame\\tinhnang\\tongkim\\jin_transport.lua",6,823);
	nNpcIdx = AddNpcNew(49,1,324,1580*32,3074*32,"\\script\\startgame\\tinhnang\\tongkim\\jin_shop.lua",6,824);   --- mo binh ok
	nNpcIdx = AddNpcNew(625,1,324,1593*32,3094*32,"\\script\\global\\npcchucnang\\ruongchua.lua",6); SetNpcValue(nNpcIdx, 2);
	--nNpcIdx = AddNpcNew(49,1,379,53557,97460,"\\script\\startgame\\tinhnang\\tongkim\\tongkimts.lua",6,"Kim Quèc tiÒn tuyÕn trinh s¸t");

	-- addnpcbosssatthu()
	addnpcphonglangdo()
	--addnpctongkimtrungcap()
	addtraptongkimtrungcap()
	wlls_autoexe()	-- [WLLS port] NPC lien dau 7 thanh + san dau
	DynamicExecute("\\script\\leaguematch\\gsdriver.lua", "WLLS_DriverInit")	-- [WLLS port] dong ho mua giai
	DynamicExecute("\\script\\missions\\tong\\tong_driver.lua", "TONG_DriverInit")	-- [TONG 21/08] lich 3 Hoat dong Phuong bang hoi (thay relay)
	DynamicExecute("\\script\\missions\\tongwar\\tongwar_driver.lua", "TONGWAR_DriverInit")	-- [TONGWAR 23/08] lich mua Bang Chien (thay relay)
	DynamicExecute("\\script\\missions\\bairenleitai\\bairen_boot.lua", "BairenLeitai_Init")	-- [BAIREN 23/08] thay activitysys/config/9 ServerStart
	addnpccongthanh()
	addtrapcongthanh()

	addnpckiemmonquan()
	
	tinsu_addnpc()	-- [TIN SU 21/08] NPC ai Thien Bao Kho ban Linux (thay addnpcthienbaokho)
	tongwar_addsignnpc()	-- [TONGWAR 23/08] Bang Chien
	
	--===== Add Trap=====--

	
	addfullnpc()
	
	addtrapother()
	
	testnew()
	print("[====== Npc Add xong ======]")
	local tmp = SubWorldID2Idx(BIENKINHPUBG)
	DelAllNpc(tmp)
	
	addpubgnpc()
	print("[====== Done Del/Add PUBG Npcs ======]")
	

end
npc_paths = {}          -- [nNpcIdx] = {{x1, y1}, {x2, y2}, ...}
npc_targets = {}        -- [nNpcIdx] = current waypoint index

function testnew()
	for i = 1, getn(tbAmbienceNpc) do
		local tbNpc = tbAmbienceNpc[i]
		local tbPos = tbNpc.tbPos
		local nMapId = tbNpc.nMapId
		local nNpcId = tbNpc.nNpcId
		local szName = tbNpc.szName or "NPC"
		local nCamp = GetCurCamp()

		
		local x = tbPos[1][1] * 32
		local y = tbPos[1][2] * 32

		local nNpcIdx = AddNpcEx3(
			{nNpcId}, 1, {0,1,2,3,4},
			nMapId, x, y,
			nil, nil,
			szName,
			nil, 0, 1000
		)

		if nNpcIdx and nNpcIdx > 0 then
			npc_paths[nNpcIdx] = tbPos
			npc_targets[nNpcIdx] = 1
			SetNpcParam(nNpcIdx, 1, tbNpc.nNpcId)
			SetNpcScript(nNpcIdx, "\\script\\test\\npcwalk.lua")
			SetNpcTimer(nNpcIdx, 18 * 3)
			SetNpcKind(nNpcIdx, 4)
			SetNpcActiveRange(nNpcIdx, 5400)
			
			local target = tbPos[1]
			NpcWalk(nNpcIdx, target[1] * 32, target[2] * 32)
		end
	end
end
function addfullnpc()
	addnpcbalang()
	
	addnpcgiangtan()
	
	addnpclongmon()
	
	addnpcnamnhac()
	
	addnpctuongduong()
	
	addnpcbienkinh()
	
	addnpcdaily()
	
	addnpcduongchau()
	
	addnpclaman()
	
	addnpcthanhdo()
	
	addnpcphuongtuong()
	

	
--phan add tong hop cac loai khac
	addnpcsatthu()

	addnpcother()
	
	addnpctaytuy()
	
	-- addtraptaytuy()
	--addnpcdienvotruong()
	
	--addnpcnhiemvu()
	--addtrainingnpc()
	addobjtuongduong()
	addnpcvantieu() -- van tieu
	-- addtrapvantieu() -- van tieu
end;