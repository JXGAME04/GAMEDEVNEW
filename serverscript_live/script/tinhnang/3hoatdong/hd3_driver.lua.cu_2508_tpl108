-- ============================================================================
-- HD3_DRIVER.LUA - Boot + Lich chay 3 hoat dong ban Linux (port 25/08)
--   (A) SAN BOSS SAT THU   (B) PHONG LANG DO   (C) VUOT AI
--
-- Thay tang S3Relay ban Linux (gateway\s3relay\relaysetting\task\*.lua) bang
-- dong ho GameServer - dung KHUON DA CHUNG cua cac dot port truoc
-- (TONG_DriverInit / TONGWAR_DriverInit / BairenLeitai_Init).
--   * HD3_DriverInit()  : goi 1 lan trong OnGame (startgame.lua) - dung NPC.
--   * HD3_Tick(nHr,nMi) : goi moi phut trong RunTime (timerserver.lua) - lich.
-- Cau hinh so lieu: script\header\cauhinh_hoatdong.lua (khoi [6], tien to HD3_).
--
-- LUU Y KIEN TRUC: logic 100% ban Linux (goi thang fenglingdu_main / OnTrigger
-- / add_killertasknpc). Chi VI TRI NPC loi vao (thuyen phu + Nhiep Thi Tran)
-- lay tu toa do da kiem dung tren JX1 (khong co bang vi tri rieng trong goc
-- Linux nen tai su dung toa do JX1 - day la DU LIEU, khong phai logic).
-- ============================================================================
Include("\\script\\header\\cauhinh_hoatdong.lua")
-- LUU Y: killbosshead.lua (384 KB) + autoexec_npc_hd3.lua duoc Include LAZY ben
-- trong HD3_DriverInit (chi startgame goi) de timerserver KHONG phai nap 384 KB
-- vao state rieng cua no (JX1 moi tep .lua mot Lua_State).

HD3_PLD_ENTRY = "\\script\\missions\\fengling_ferry\\fldmap_boat1.lua"
HD3_VA_TRIGGER = "\\settings\\trigger_challengeoftime.lua"
HD3_VA_RANK   = "\\script\\missions\\challengeoftime\\rank_perday.lua"

-- Toa do 6 thuyen phu (3 Nam bo, 3 Bac bo) - map 336, dv O. Tai su dung tu ban
-- JX1 (lib_phonglangdo TAB_NPCCHUCNANG - toa do dung, script tro sang Linux).
-- [3HD 25/08 C47] TACH bo NAM / bo BAC. Truoc day ca 6 NPC deu mang script len
-- thuyen nen thuyen phu bo Bac hanh xu y het bo Nam (chu game bao loi).
-- Bo NAM = 3 BEN cua ban Linux (huichengfu.lua:12-14 'Ben 1/2/3').
-- Bo BAC = 3 cho cap ben (fld_head.lua:15 northMAP_POS nam sat 3 diem nay).
HD3_PLD_BEN_NAM = {
	{1147, 3018, 336, 1}, {1280, 2907, 336, 2}, {1535, 2808, 336, 3},
}
-- Ban Linux KHONG co NPC o bo Bac. Giu lai theo du an cu cho tien duong ve;
-- muon dung 100% Linux thi dat HD3_PLD_CO_THUYENPHU_BAC = 0 trong cauhinh.
HD3_PLD_BEN_BAC = {
	{1324, 2886, 336, 2}, {1493, 2809, 336, 3}, {1173, 2981, 336, 1},
}

-- ============================================================================
-- BOOT
-- ============================================================================
-- [3HD 25/08 C16] don NPC CU cua ban Viet theo SCRIPT DOI THOAI (chac chan
-- khong dung NPC moi du trung ten). Goi luc boot VA moi phut (tu lanh: NPC
-- cu co the sinh SAU DriverInit boi du lieu region / hoi sinh).
HD3_SCRIPT_CU = {
	"nhieptran.lua",		-- Nhiep Thi Tran ban Viet (boss satthu + vuot ai cu)
	"tinhnang\\phonglangdo\\thuyenphu.lua",	-- thuyen phu Nam cu
	"tinhnang\\phonglangdo\\thuyenphubac.lua",	-- thuyen phu Bac cu
}
function HD3_DonNpcCu(bNoiLuc)
	if (HD3_DelNpcByScript == nil) then return end
	local nTong = 0
	for i = 1, getn(HD3_SCRIPT_CU) do
		nTong = nTong + HD3_DelNpcByScript(HD3_SCRIPT_CU[i])
	end
	-- [C19] lop CHAC CHAN: xoa NPC trung ten KHONG mang script cua minh
	-- (bat duoc NPC cu bat ke no bind script gi / khong script)
	if (HD3_DelNpcByNameEx ~= nil) then
		nTong = nTong + HD3_DelNpcByNameEx("NhiÕp ThÝ TrÇn", 0, "nieshichen")
		nTong = nTong + HD3_DelNpcByNameEx("ThuyÒn phu", 336, "hd3_thuyenphu")
	end
	if (nTong > 0 or bNoiLuc == 1) then
		print("[3HD] Don NPC cu (theo script): xoa "..nTong..".")
	end
end
function HD3_DriverInit()
	-- [3HD 25/08 r2] GUARD: neu DLL dang chay CHUA co HD3_AddNpc (chua swap
	-- CoreServer.dll.moi_2508_3hoatdong) thi THOAT EM DEM - khong duoc de loi
	-- lan len startgame.lua lam chet cac buoc boot phia sau (tinsu_addnpc...).
	if (HD3_AddNpc == nil) then
		print("[3HD] BO QUA HD3_DriverInit: DLL chua co HD3_AddNpc - can swap CoreServer.dll moi.")
		return
	end
	-- [3HD 25/08 C14] don NPC "Nhiep Thi Tran" CU cua ban Viet (nam trong du
	-- lieu map tinh) TRUOC khi sinh NPC 769 Linux - de dung 100% ban Linux.
	-- Guard nil: DLL cu chua co ham thi bo qua (khong lam chet boot).
	if (HD3_DelNpcByName ~= nil) then
		local nXoaCu = HD3_DelNpcByName("NhiÕp ThÝ TrÇn")
		print("[3HD] Da xoa "..nXoaCu.." NPC Nhiep Thi Tran cu (ban Viet).")
	end
	HD3_DonNpcCu(1)	-- [C16] xoa theo script - lop chac chan
	-- [C23] IDEMPOTENT: don sach NPC do CHINH MINH sinh truoc khi sinh lai.
	-- Nut admin HD3_ADM_ST_Boot goi lap HD3_DriverInit tung lam NHAN BAN toan bo
	-- NPC (2 Nhiep Thi Tran / 2 boss cung toa do chu game bao 14:0x).
	if (HD3_DelNpcByScript ~= nil) then
		local nSach = HD3_DelNpcByScript("\tollgate\killer\\")
		nSach = nSach + HD3_DelNpcByScript("hd3_thuyenphu")
		if (nSach > 0) then
			print("[3HD] Don "..nSach.." NPC cu cua chinh he truoc khi sinh lai (goi lap an toan).")
		end
	end
	-- nap lazy (chi luc boot): 384 KB killbosshead + bang NPC 769
	Include("\\script\\global\\autoexec_npc_hd3.lua")
	Include("\\script\\task\\tollgate\\killbosshead.lua")
	-- (A) Sat thu: 7 NPC 769 + 160 boss (ban Linux). Chi nhom cap 90 phat thuong.
	if (add_dialognpc_hd3 ~= nil) then
		add_dialognpc_hd3(adddialognpc_hd3)     -- NPC 769 Nhiep Thi Tran
	end
	if (add_killertasknpc ~= nil) then
		add_killertasknpc(addkillertasknpc)     -- boss sat thu (killbosshead)
	end
	-- [3HD 25/08 C15] xoa 6 NPC thuyen phu CU cua ban Viet (dung TRUNG toa do
	-- voi NPC moi -> user click trung NPC cu, vao PLD ngoai mission => khong
	-- danh duoc quai vi thieu SetFightState(1) cua mission). GIOI HAN map 336:
	-- template "Thuyen phu *" con dung cho ben do thon o map khac.
	if (HD3_DelNpcByName ~= nil) then
		local nXoaTP = HD3_DelNpcByName("ThuyÒn phu", 336)
		print("[3HD] Da xoa "..nXoaTP.." NPC thuyen phu cu (ban Viet, map 336).")
	end
	-- (B) Phong Lang Do: 6 thuyen phu (Linux fld_head.lua fld_wanttakeboat).
	HD3_PLD_AddBoatNpc()
	-- [BAC DAU 25/08] NPC Bac Dau lao nhan o 7 thanh (he Bac Dau lenh bai).
	HD3_BD_AddNpc()
	-- (C) Vuot Ai: NPC bao danh Nhiep Thi Tran (npcNhiepThiTran.lua) da gan qua
	--     station.lua khi client bam Dich Quan 7 thanh - khong can sinh rieng.
	-- [3HD 25/08] CHAN: g_IniScriptEngine (KSortScript.cpp:51-66) chi nap
	-- \script va \scriptjx2\tong_vn. Hai tep trigger nam o \settings nen
	-- KHONG co trong cay => DynamicExecute tra NULL => lich Vuot Ai khong chay.
	-- ReLoadScript = UnLoadScript + LoadScriptToSortListA (THEM MOI vao cay).
	ReLoadScript("\\settings\\trigger_include.lua")
	ReLoadScript("\\settings\\trigger_challengeoftime.lua")

	print("[3HD] HD3_DriverInit xong (boot 3 hoat dong Linux).")
end


-- ============================================================================
-- BAC DAU LENH BAI - NPC 'Bac Dau lao nhan'. Ban Linux dat NPC qua activitysys
-- config 1046 (DA BI CHU THICH, tuc ban Linux cung khong chay) nen o day tu dat.
-- Moi NPC mang so thu tu thanh (SetNpcValue) de lam nhiem vu di duong.
-- ============================================================================
function HD3_BD_AddNpc()
	if (HD_CFG("HD3_BD_BAT", 1) ~= 1) then
		return
	end
	Include("\\script\\tinhnang\\3hoatdong\\beidou\\bd_activity.lua")
	if (tbBeidou == nil or tbBeidou.THANH == nil) then
		return
	end
	if (HD3_DelNpcByScript ~= nil) then
		HD3_DelNpcByScript("bd_npc")
	end
	local n = 0
	for i = 1, getn(tbBeidou.THANH) do
		local t = tbBeidou.THANH[i]
		local nIdx = SubWorldID2Idx(t[2])
		if (nIdx >= 0) then
			local npc = HD3_AddNpc(108, 1, nIdx, t[3]*32, t[4]*32, 0, "B¾c §Èu l·o nh©n")
			if (npc ~= nil and npc > 0) then
				SetNpcScript(npc, "\\script\\tinhnang\\3hoatdong\\beidou\\bd_npc.lua")
				SetNpcValue(npc, i)
				n = n + 1
			end
		end
	end
	print("[3HD] Bac Dau: sinh "..n.." NPC Bac Dau lao nhan.")
end
-- moi thuyen phu: template 240, script wrapper dat BOATID roi goi fld_wanttakeboat
-- [3HD 25/08 C47] sinh rieng tung bo: bo Nam len thuyen, bo Bac ve Nam.
function HD3_PLD_AddBoatNpcTab(tb, szScript)
	for i = 1, getn(tb) do
		local t = tb[i]
		local nIdx = SubWorldID2Idx(t[3])
		if (nIdx >= 0) then
			local npc = HD3_AddNpc(240, 1, nIdx, t[1]*32, t[2]*32, 1, "ThuyÒn phu")
			if (npc ~= nil and npc > 0) then
				SetNpcScript(npc, szScript)
				SetNpcValue(npc, t[4])   -- bo Nam: BOATID 1/2/3; bo Bac: so hieu ben ve
			end
		end
	end
end

function HD3_PLD_AddBoatNpc()
	HD3_PLD_AddBoatNpcTab(HD3_PLD_BEN_NAM, "\\script\\missions\\fengling_ferry\\hd3_thuyenphu.lua")
	if (HD_CFG("HD3_PLD_CO_THUYENPHU_BAC", 1) == 1) then
		HD3_PLD_AddBoatNpcTab(HD3_PLD_BEN_BAC, "\\script\\missions\\fengling_ferry\\hd3_thuyenphubac.lua")
	end
end

-- ============================================================================
-- LICH (thay TaskContent cua relay)
-- ============================================================================
-- moc gio (HHMM) trong danh sach va dung phut :00
function HD3_InList(nHHMM, tb)
	if (tb == nil) then return 0 end
	for i = 1, getn(tb) do
		if (tb[i] == nHHMM) then return 1 end
	end
	return 0
end

function HD3_Tick(nHr, nMi)
	HD3_DonNpcCu(0)	-- [C16] tu lanh: NPC cu sinh muon/hoi sinh la bi don ngay trong 1 phut
	local nHHMM = nHr * 100 + nMi

	-- (B) Phong Lang Do - relay fengling_ferry.lua: moi gio phut :00
	if (nMi == 0) and (HD3_InList(nHHMM, HD_CFG("HD3_PLD_GIO", {})) == 1) then
		DynamicExecute(HD3_PLD_ENTRY, "fenglingdu_main")
		print(format("[3HD] Phong Lang Do khai cuoc %02d:%02d", nHr, nMi))
	end

	-- (C) Vuot Ai - relay challengeoftime.lua: moi gio phut :00
	if (nMi == 0) and (HD3_InList(nHHMM, HD_CFG("HD3_VA_GIO", {})) == 1) then
		DynamicExecute(HD3_VA_TRIGGER, "OnTrigger")
		AddLocalCountNews("Thêi gian b¸o danh 'Th¸ch thøc thêi gian' ®· b¾t ®Çu, c¸c ®éi tr­ëng h·y mau ®Õn NhiÕp ThÝ TrÇn ë c¸c thµnh thÞ b¸o danh. Thêi gian b¸o danh lµ 10 phót.", 2)	-- chuoi goc relay challengeoftime.lua
		print(format("[3HD] Vuot Ai bao danh %02d:%02d", nHr, nMi))
	end

	-- (C) Vuot Ai - bang xep hang ngay: relay challegeoftime-dailyrank.lua
	if (nHHMM == HD_CFG("HD3_VA_GIO_XEPHANG", 0)) then
		HD3_VA_DailyRank()
	end
end

-- trao thuong doi hang 1 cua ngay (Ladder 10235) roi xoa bang
function HD3_VA_DailyRank()
	local name, value = Ladder_GetLadderInfo(10235, 1)
	if (name ~= nil and name ~= "") then
		value = value * (-1)
		local szTime = format("%s phót %s gi©y", floor(value/60), floor(mod(value, 60)))
		local szMsg = format("Chóc mõng ®éi <%s> ®· hoµn thµnh <th¸ch ®Êu thêi gian> víi thêi gian ng¾n nhÊt <%s>", name, szTime)	-- chuoi goc relay dailyrank
		AddGlobalNews(szMsg, 10)	-- dung ham goc Linux (JX1 co san)
		Msg2SubWorld(szMsg)
	end
	Ladder_ClearLadder(10235)
end

-- ============================================================================
-- ADMIN (Lenh Bai Admin - hd3_admin.lua goi)
-- ============================================================================
function HD3_Adm_PLD_Now()
	DynamicExecute(HD3_PLD_ENTRY, "fenglingdu_main")
end
function HD3_Adm_VA_Now()
	DynamicExecute(HD3_VA_TRIGGER, "OnTrigger")
end
function HD3_Adm_VA_Rank()
	HD3_VA_DailyRank()
end
