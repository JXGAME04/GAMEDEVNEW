Include("\\script\\chienlenh\\cl_def.lua")	-- [CL 04/09] so hieu nhiem vu Chien Lenh
--Author: Fong KiÒu
--Date: 28/11/2020
--Function: Timer toan server

Include("\\script\\lib\\lib_ham.lua")
-- [CFGLDHC 30/08] Bo doc cau hinh cho tep nay (rieng phan Loi Dai Hon Chien).
-- Tra ve MAC DINH (= so cu) khi bo cau hinh chua nap.
-- [BOLDHC 30/08] da bo ham LDHC_CFG (6 dong).
-- Hoat dong Loi Dai Hon Chien bo han theo quyet dinh chu game 30/08.
-- Khong co ban Linux thay the. Ban goc o _dara\script\tinhnang\loidaihonchien.

Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_server.lua")
-- [WLLS port 20/08/2026] da go he lien_dau cu; sukien_liendau (dong 68) von da tat
-- [GOHECU 30/08] khong con ai dung sau khi go sukien_loidaibanghoi
-- Include("\\script\\tinhnang\\loidai\\lib_loidai.lua")
-- [GOHECU 30/08] khong con ai dung sau khi go sukien_congthanh
-- Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\mobinhtk.lua")
Include("\\script\\tinhnang\\boss_hoangkim\\lib_bosshk.lua")
-- [GOHECU 30/08] khong con ai dung sau khi go sukien_vuotai
-- Include("\\script\\tinhnang\\vuot_ai\\lib_vuotai.lua")
-- [GOHECU 30/08] khong con ai dung sau khi go sukien_phonglangdo
-- Include("\\script\\tinhnang\\phonglangdo\\lib_phonglangdo.lua")
Include("\\script\\event\\event_huyhoang_dungdb\\lib_huyhoang.lua")
Include("\\script\\event\\event_cauhoi\\lib.lua")
Include("\\script\\event\\trongbanghoi\\lib.lua")
Include("\\script\\event\\kiemmonquan\\lib_kmq.lua")
Include("\\script\\header\\tongkim.lua");
Include("\\script\\header\\loginao.lua");
Include("\\script\\songbac\\datasongbac.lua")
Include("\\script\\songbac\\datanganluong.lua")
Include("\\script\\songbac\\datatienvang.lua")
Include("\\script\\songbac\\chusongbac.lua")

Include("\\script\\tinhnang\\pubg\\pubg.lua")
Include("\\script\\tinhnang\\pubg\\pubgutils.lua")

Include("\\script\\timerserver_ctc.lua")
Include("\\script\\tinhnang\\3hoatdong\\hd3_driver.lua")	-- [3HD 25/08] lich Phong Lang Do + Vuot Ai (thay S3Relay)	-- DOT E + LOI DAI CN (21/08): dong ho 18h/19h/20h cong thanh + loi dai bang hoi
Include("\\script\\tinhnang\\viemde\\ydbz_driver.lua")	-- [VIEMDE 26/08] boot + lich Viem De Bao Tang (thay S3Relay)

Include("\\script\\partner\\partner_test_bdh.lua")
Include("\\script\\task\\partner\\train\\bdh_jitan_driver.lua")	-- [BDH-G5] te dan gio chan	-- [BDH 27/08] bo test tu dong - GO SAU NGHIEM THU

-- [NHIPNAP 29/08] bo doc cau hinh (hai tep nay deu la LA, khong Include gi)
Include("\\script\\cauhinh\\ch_lib.lua")
Include("\\script\\cauhinh\\ch_chung.lua")
Include("\\script\\cauhinh_web\\cfgw_driver.lua")	-- [CFGW 04/09] khai cau hinh len MySQL (gcfg) cho web admin + nhip 30 s do sua
Include("\\script\\tinhnang\\botauto\\bot_auto.lua")	-- [BOTAUTO 04/09] tu goi bot + phan viec

-- ============================================================================
-- [TOIUU 05/09] NAP LAI NHE + DO THOI GIAN RunTime
-- Truoc: RunTime dofile("script/timerserver.lua") MOI PHUT -> chay lai ~30 Include o dau tep
-- (lib_tktc, mobinhtk, bosshk, songbac data, pubg, hd3, ydbz, cfgw, bot_auto...) = bien dich lai
-- ca cay moi phut -> jx_perf SCRIPT_TIME max 20-40 ms MOI phut (rot khung hinh). Bien dich rieng
-- timerserver.lua chi 0,25 ms; doc + so sanh noi dung 0,03 ms/32 KB.
-- Nay: TS_CoThayDoi doc timerserver.lua + tep no Include (de quy 3 muc, remap script\lib -> scriptjx2
-- nhu engine), so voi ban cache; chi dofile khi co tep doi. Sua nong van co hieu luc trong 1 phut.
-- TS_GhiProf: ghi logs (GhiLog "PROF") khi mot lan RunTime >= TS_PROF_NGUONG ms, kem thoi gian tung khoi.
-- ============================================================================
g_tbTS_Cache = g_tbTS_Cache or {}
TS_tProf = TS_tProf or {}
TS_PROF_NGUONG = TS_PROF_NGUONG or 8

function TS_DocTep(szPath)
	local f = openfile(szPath, "rb")
	if (f == nil) then return nil end
	local s = read(f, "*a")
	closefile(f)
	return s
end

-- "\\script\\lib\\x.lua" -> "script/lib/x.lua"; thieu thi thu scriptjx2 nhu sJX2RemapScriptPath
function TS_DuongDan(szInc)
	-- trong TEP nguon, Include("\\script\\lib\\x.lua") la HAI ky tu gach cheo -> gop moi day \ hoac / thanh mot /
	local s = gsub(szInc, "[\\/]+", "/")
	s = gsub(s, "^/+", "")
	local f = openfile(s, "rb")
	if (f ~= nil) then closefile(f) return s end
	s = gsub(s, "^script/lib/", "scriptjx2/lib/")
	s = gsub(s, "^script/tong/", "scriptjx2/tong_vn/")
	return s
end

function TS_GomTep(szPath, tbDS, nMuc)
	if (nMuc > 3 or tbDS[szPath] ~= nil) then return end
	local s = TS_DocTep(szPath)
	if (s == nil) then return end
	tbDS[szPath] = s
	local sQuet = "\n" .. s
	local i = 1
	while 1 do
		local a, b, p = strfind(sQuet, "\n[ \t]*Include[ \t]*%([ \t]*\"([^\"]+)\"", i)
		if (a == nil) then break end
		TS_GomTep(TS_DuongDan(p), tbDS, nMuc + 1)
		i = b + 1
	end
end

-- 1 = co tep doi (hoac chua co cache) -> can dofile; 0 = khong doi
function TS_CoThayDoi(szPath)
	local tb = {}
	TS_GomTep(szPath, tb, 1)
	local nDoi = 0
	local nTep = 0
	for k, v in pairs(tb) do
		nTep = nTep + 1
		if (g_tbTS_Cache[k] ~= v) then nDoi = nDoi + 1 end
	end
	if (nDoi == 0 and nTep > 0) then return 0 end
	for k, v in pairs(tb) do g_tbTS_Cache[k] = v end
	if (GhiLog ~= nil) then
		GhiLog("PROF", format("timerserver nap lai: %d/%d tep doi", nDoi, nTep))
	end
	return 1
end

function TS_GhiProf(nTongMs)
	if (nTongMs < TS_PROF_NGUONG or GhiLog == nil) then return end
	local sz = ""
	for k, v in pairs(TS_tProf) do
		if (v >= 1) then sz = sz .. format(" %s=%d", k, v) end
	end
	GhiLog("PROF", format("RunTime %d ms:%s", nTongMs, sz))
end

function RunTime()
	-- [TOIUU 05/09] do thoi gian tung khoi (clock() = ms CPU); ghi log PROF khi tong >= TS_PROF_NGUONG ms
	TS_tProf = {}
	local tRun0 = clock()
	
	-- [NHIPNAP 29/08] NHIP TU NAP LAI. Dong dofile duoi day nap lai CHINH tep
	-- nay moi phut, keo theo ca 33 Include o dau tep (~103 tep, ~0,97 MB).
	-- Doi lai: sua script an ngay, khong can restart.
	-- CH_NAPLAI_PHUT o script\cauhinh\ch_chung.lua:
	--    1 = y het truoc day (mac dinh) | 5 = 5 phut/lan | 0 = tat han
	local nNhipNap = 1
	if (G_CFG ~= nil) then
		nNhipNap = G_CFG("CH_NAPLAI_PHUT", 1)
	end
	if (nNhipNap ~= nil and nNhipNap > 0) then
		-- [PBLUA 29/08] RunTime KHONG chay dung mot lan moi phut.
		-- CoreServerShell.cpp:1165-1171 goi khi (bo dem KHUNG % 18 == 0)
		-- VA giay == 0; khi may chu khung lai roi chay don thi nhieu boi
		-- so cua 18 cung roi vao giay 0 => RunTime chay 2-3 lan trong
		-- cung mot phut. Nen dung DAU MOC thay cho phep chia du.
		-- g_nMocNapLai CHI duoc gan trong ham (khong khai o cap tep) de
		-- song qua dofile - dung khuon ma tep nay da dung cho
		-- g_nTongMaintainDay.
		local nYrN, nMoN, nDyN, nHrN, nMiN = GetTimeNow()
		local nPhutNay = nDyN * 1440 + nHrN * 60 + nMiN
		if (g_nMocNapLai == nil) then
			g_nMocNapLai = -99999
		end
		if (nPhutNay - g_nMocNapLai >= nNhipNap) then
			g_nMocNapLai = nPhutNay
			-- [TOIUU 05/09] truoc: dofile MOI phut = chay lai ~30 Include o dau tep = bien dich lai
			-- ca cay -> dinh nhon 20-40 ms moi phut (jx_perf SCRIPT_TIME). Nay chi dofile khi noi dung
			-- timerserver.lua hoac tep no Include (3 muc) THAT SU doi (doc + so sanh ~1 ms). Van sua nong duoc.
			local tNap0 = clock()
			if (TS_CoThayDoi == nil or TS_CoThayDoi("script/timerserver.lua") == 1) then
				dofile("script/timerserver.lua")
				TS_nLanNap = (TS_nLanNap or 0) + 1
			end
			TS_tProf.naplai = (clock() - tNap0) * 1000
		end
	end
	--call(BDH_TestTick, {}, "x") -- [28/08] TAT: test nay PB_ClearBot() go sach bot moi phut
	--call(BDH_TalkTick, {}, "x") -- [28/08] TAT: cung ly do
	--call(BDH_PetTick, {}, "x")	-- [PETSYS] GO SAU NGHIEM THU	-- [BDH-GM] cap thu test - GO SAU NGHIEM THU	-- [BDH-G4] GO SAU KHI NGHIEM THU	-- [BDH 27/08] test tu dong (bao ve loi) - GO SAU NGHIEM THU
	TS_t0 = clock()
	call(BDH_JitanTick, {}, "x")	-- [BDH-G5] te dan
	TS_tProf.bdh = (clock() - TS_t0) * 1000
	TS_t0 = clock()
	call(CL_Tick_Wrap, {}, "x")	-- [CL 04/09] nhip phut: phut online + reset ngay/tuan + xa
	TS_tProf.cl = (clock() - TS_t0) * 1000
	TS_t0 = clock()
	call(BotAuto_Tick, {}, "x")	-- [BOTAUTO 04/09] tu goi bot, Da Tau, ban sap, Tong Kim theo gio
	TS_tProf.botauto = (clock() - TS_t0) * 1000
	
	local nYr,nMo,nDy,nHr,nMi,nSe,nDyfW = GetTimeNow()	
	--print(format("=> Player Online=%d Time=%d:%d nDyfW=%d <=", GetPlayerCount(), nHr, nMi, nDyfW)) 	-- print ra GS
	if (nHr == 0) and (nMi == 00) then
		AddGlobalCountNews("<color=blue>Ngµy míi ®· tíi, chóc c¸c b¹n mét ngµy míi may m¾n.<color>",3)
		Msg2SubWorld("<color=blue>Ngµy míi ®· tíi, chóc c¸c b¹n mét ngµy míi may m¾n.<color>")
		UpdateNgayMoiAllPlayer()
	end
	-- JX2: bao tri BANG hang ngay bang chinh Lua goc (tong.lua MAINTAIN_R
	-- + WEEKLY_MAINTAIN_R sang thu Hai): chi phi duy tri, tro cap, tam
	-- ngung, san luong tac phuong, muc tieu tuan + chon muc tieu moi.
	if (nHr == 6) and (nMi == 5) and (g_nTongMaintainDay ~= nDy) then
		g_nTongMaintainDay = nDy
		if (TONG_DailyMaintainAll ~= nil) then
			local nRun = TONG_DailyMaintainAll(nDyfW)
			print("[TONGJX2] bao tri bang: "..nRun)
		end
	end
	-- if (nHr == 19) and (nMi == 00) then
		-- AddGlobalCountNews("<color=blue>M¸y Chñ Ba L¨ng HuyÖn chÝnh thøc b¾t ®Çu ®ua TOP, chóc c¸c b¹n cã nh÷ng tr¶i nghiÖm vui vÎ.<color>",3)
		-- Msg2SubWorld("<color=blue>M¸y Chñ Ba L¨ng HuyÖn chÝnh thøc b¾t ®Çu ®ua TOP, chóc c¸c b¹n cã nh÷ng tr¶i nghiÖm vui vÎ.<color>")
	-- end
	-- if (nHr == 16) and (nMi > 45) and (nMi < 59) then
		-- AddGlobalCountNews("<color=blue>B¶o tr× ®Þnh kú vµo lóc 17h h»ng ngµy. Thêi gian b¶o tr× 5p Ae nhí ch¹y l¹i autoupdate ®Ó vµo game!<color>",3)
		-- Msg2SubWorld("<color=blue>B¶o tr× ®Þnh kú vµo lóc 17h h»ng ngµy. Thêi gian b¶o tr× 5p Ae nhí ch¹y l¹i autoupdate ®Ó vµo game!<color>")
		-- UpdateNgayMoiAllPlayer()
	-- end
	-- if (nMi == 0)	then
		-- ThongBao()
	-- end
	-- if (nMi == 05) or (nMi ==10) or (nMi == 15) or (nMi == 20) or (nMi == 25) or (nMi == 30) or (nMi == 35) or (nMi == 40) or (nMi == 45) or (nMi == 50) or (nMi == 55) or (nMi == 60) then
		-- Active_LogginAo();
		-- ThongBao();
	-- end
	-- [GOHECU 30/08] da go nhanh goi BAT_LIENDAU (9 dong). He lien_dau cu da doi di tu 20/08;
	-- ham nay Include thu muc KHONG CON TON TAI. Lien Dau that = ban WLLS,
	-- chay doc lap, khong qua cong tac BAT_LIENDAU.
	TS_t0 = clock()
	if (G_CFG("BAT_TONGKIM", 1) == 1) then
		if (sukien_tongkim ~= nil) then
			sukien_tongkim(nHr,nMi)
		else
			if (GhiLog ~= nil) then
				GhiLog("CAUHINH", "BAT_TONGKIM bat nhung ham sukien_tongkim chua nap")
			end
		end
	end
	-- [3HD 25/08] lich Phong Lang Do + Vuot Ai (thay tang S3Relay ban Linux)
	TS_tProf.tongkim = (clock() - TS_t0) * 1000
	TS_t0 = clock()
	if (G_CFG("BAT_HD3", 1) == 1 and HD3_Tick ~= nil) then HD3_Tick(nHr, nMi) end
	TS_tProf.hd3 = (clock() - TS_t0) * 1000
	TS_t0 = clock()
	if (G_CFG("BAT_VIEMDE", 1) == 1 and YDBZ_Tick ~= nil) then YDBZ_Tick(nHr, nMi) end	-- [VIEMDE 26/08] lich 8h25/10h25/14h25/16h25/18h25/20h25/22h25
	-- [21/08] cong thanh JX2 + loi dai bang hoi CN: 0h/18h/19h/20h (guard ngay trong ham)
	TS_tProf.viemde = (clock() - TS_t0) * 1000
	TS_t0 = clock()
	if (G_CFG("BAT_CTC_JX2", 1) == 1 and CTC_JX2_Tick ~= nil) then
		CTC_JX2_Tick(nDyfW, nHr, nMi)
	end
	-- [GOHECU 30/08] da go lich sukien_congthanh (khoa BAT_CONGTHANH_VIET). Ban thay: Cong Thanh ban JX2 (BAT_CTC_JX2 = 1, CTC_JX2_Tick).
	TS_tProf.ctc = (clock() - TS_t0) * 1000
	TS_t0 = clock()
	if (G_CFG("BAT_HATHUYHOANG", 0) == 1) then
		if (sukien_hathuyhoang ~= nil) then
			sukien_hathuyhoang(nHr,nMi)
		else
			if (GhiLog ~= nil) then
				GhiLog("CAUHINH", "BAT_HATHUYHOANG bat nhung ham sukien_hathuyhoang chua nap")
			end
		end
	end
	if (G_CFG("BAT_BOSS_HOANGKIM", 0) == 1) then
		if (sukien_bosshk ~= nil) then
			sukien_bosshk(nHr,nMi)
		else
			if (GhiLog ~= nil) then
				GhiLog("CAUHINH", "BAT_BOSS_HOANGKIM bat nhung ham sukien_bosshk chua nap")
			end
		end
	end
	-- [GOHECU 30/08] da go lich sukien_vuotai (khoa BAT_VUOTAI_VIET). Ban thay: Vuot Ai trong cum 3 hoat dong ban Linux (BAT_HD3 = 1, HD3_Tick).
	-- [GOHECU 30/08] da go lich sukien_phonglangdo (khoa BAT_PHONGLANGDO_VIET). Ban thay: Phong Lang Do trong cum 3 hoat dong ban Linux (BAT_HD3 = 1, HD3_Tick).
	if (G_CFG("BAT_HOADANG", 0) == 1) then
		if (sukien_trangnguyen ~= nil) then
			sukien_trangnguyen(nHr,nMi)
		else
			if (GhiLog ~= nil) then
				GhiLog("CAUHINH", "BAT_HOADANG bat nhung ham sukien_trangnguyen chua nap")
			end
		end
	end
	-- [GOHECU 30/08] da go lich sukien_loidaibanghoi (khoa BAT_LOIDAI_BANGHOI). Ban thay: Loi Dai bang hoi ban JX2 (CTC_JX2_Tick + TimerTask 16/17 citywar_arena).
	if (G_CFG("BAT_TRONG_BANGHOI", 0) == 1) then
		if (sukien_trongbanghoi ~= nil) then
			sukien_trongbanghoi(nHr,nMi)
		else
			if (GhiLog ~= nil) then
				GhiLog("CAUHINH", "BAT_TRONG_BANGHOI bat nhung ham sukien_trongbanghoi chua nap")
			end
		end
	end
	if (G_CFG("BAT_KIEMMONQUAN", 0) == 1) then
		if (sukien_kiemmonquan ~= nil) then
			sukien_kiemmonquan(nHr,nMi)
		else
			if (GhiLog ~= nil) then
				GhiLog("CAUHINH", "BAT_KIEMMONQUAN bat nhung ham sukien_kiemmonquan chua nap")
			end
		end
	end
	if (G_CFG("BAT_VANTIEU_LOA", 0) == 1) then
		if (sukien_vantieu ~= nil) then
			sukien_vantieu(nHr,nMi)
		else
			if (GhiLog ~= nil) then
				GhiLog("CAUHINH", "BAT_VANTIEU_LOA bat nhung ham sukien_vantieu chua nap")
			end
		end
	end
	TS_tProf.sukien = (clock() - TS_t0) * 1000
	TS_t0 = clock()
	if (G_CFG("BAT_DUATOP", 0) == 1) then
		if (XepHangDuaTop ~= nil) then
			XepHangDuaTop()
		else
			if (GhiLog ~= nil) then
				GhiLog("CAUHINH", "BAT_DUATOP bat nhung ham XepHangDuaTop chua nap")
			end
		end
	end
	TS_tProf.duatop = (clock() - TS_t0) * 1000
	TS_t0 = clock()
	if (G_CFG("BAT_SONGBAC", 0) == 1) then
		if (songbac ~= nil) then
			songbac(nHr,nMi)
		else
			if (GhiLog ~= nil) then
				GhiLog("CAUHINH", "BAT_SONGBAC bat nhung ham songbac chua nap")
			end
		end
	end
	-- [BOLDHC 30/08] da bo nhanh goi BAT_LOIDAI_HONCHIEN (9 dong).
	-- Hoat dong Loi Dai Hon Chien bo han theo quyet dinh chu game 30/08.
	-- Khong co ban Linux thay the. Ban goc o _dara\script\tinhnang\loidaihonchien.
	TS_tProf.songbac = (clock() - TS_t0) * 1000
	TS_t0 = clock()
	if (G_CFG("BAT_CHECK_KICK", 0) == 1) then
		if (check_and_kick ~= nil) then
			check_and_kick(nMi,nSe)
		else
			if (GhiLog ~= nil) then
				GhiLog("CAUHINH", "BAT_CHECK_KICK bat nhung ham check_and_kick chua nap")
			end
		end
	end
	TS_tProf.kick = (clock() - TS_t0) * 1000
	TS_t0 = clock()
	if (G_CFG("BAT_PUBG", 0) == 1) then
		if (pubg_runner ~= nil) then
			pubg_runner(nHr,nMi)
		else
			if (GhiLog ~= nil) then
				GhiLog("CAUHINH", "BAT_PUBG bat nhung ham pubg_runner chua nap")
			end
		end
	end
	TS_tProf.pubg = (clock() - TS_t0) * 1000
	TS_GhiProf((clock() - tRun0) * 1000)
	
end

function RunTimePUBG()
	-- pubg_runner(nHr,nMi)
end

function pubg_runner(nHr,nMi)
	local start_t = -6
	if(nMi == 14) then -- 
		AddGlobalCountNews("Thö th¸ch Sinh Tån b¾t ®Çu b¸o danh. Nh©n sÜ vâ l©m mau ®Õn ... ®Ó b¸o danh tham gia!",3)
		Msg2SubWorld("<color=green>Thö th¸ch Sinh Tån b¾t ®Çu b¸o danh. Nh©n sÜ vâ l©m mau ®Õn ... ®Ó b¸o danh tham gia!")
		
		local nSubWorldId
		nSubWorldId = SubWorldID2Idx(BIENKINHPUBG)
		if (nSubWorldId >= 0) then
			SubWorld = nSubWorldId
			if(IsMission(MS_PUBG) == 0) then
				OpenMission(MS_PUBG)
			end
			StartMissionTimer(MS_PUBG,14, MSTIME_PUBG_BD*60*18)--nMissionId, nTimerId, nTimeInterval so phut de chinh thuc bat dau 10*60*18
			print(format("===> Bao danh PUBG %d:%d Map[%d]<===", nHr, nMi, BIENKINHPUBG))
		else
			print(format("===> PUBG %d:%d chua mo Map[%d] <===", nHr, nMi, BIENKINHPUBG))
		end
	end
	if(nMi == 15) then
		AddGlobalCountNews("Thö th¸ch Sinh Tån ®· chÝnh thøc b¾t ®Çu! C¸c nh©n sÜ vâ l©m h·y cè g¾ng sèng sãt.", 3)--thong bao bat dau
		Msg2SubWorld("<color=green>PUBG ®· chÝnh thøc b¾t ®Çu! C¸c nh©n sÜ vâ l©m h·y cè g¾ng sèng sãt.")
		print(format("===> Pubg bat dau %d:%d <===", nHr , nMi))
	end
	
	-- process close PUBG boundary
	local nSubWorldId
	nSubWorldId = SubWorldID2Idx(BIENKINHPUBG)
	if (nSubWorldId >= 0) then
		SubWorld = nSubWorldId
		if(IsMission(MS_PUBG) ~= 0) then
			process()
			print("PUBG chay bo test.")
		end
	end
end

function split_lines(str)
    local lines = {}
    local pos = 1
    while (1) do
        local nl = strfind(str, "\n", pos)
        if not nl then
            tinsert(lines, strsub(str, pos))
            break
        end
        tinsert(lines, strsub(str, pos, nl - 1))
        pos = nl + 1
    end
    return lines
end

function check_and_kick(nMi,nSe)
	local filename = "dulieu/username_kick.txt"
    local file = openfile(filename, "r")


    local content = read(file, "*a")
    closefile(file)
	
	if not content or content == "" then
         -- print("Khong Phat Hien Tai Khoan Kich: " .. filename)
        return
    end

    local lines = split_lines(content)

    for i = 1, getn(lines) do
        local line = lines[i]
        if line and line ~= "" then
             -- print("Phat hien tai khoan can kich ")
            kichaccket()
            return  
        end
    end

    
end


-- Function to load accounts from file into a table
function load_accounts(filename)
    local accounts = {}
    local file = openfile(filename, "r")
    if not file then
        -- print("Error: Cannot open file: " .. filename)
        return accounts
    end
    
    local content = read(file, "*a")
    closefile(file)
    
    -- Process each line
    local lines = split_lines(content)

    for i = 1, getn(lines) do
        -- print(lines[i])
        if not lines[i] then break end

        line = lines[i]
        if line ~= "" then
            tinsert(accounts, line)
        end
    end

    return accounts
end


function kichaccket()
    local filename = "dulieu/username_kick.txt"
    local account_list = load_accounts(filename)

    for i = 1, getn(account_list) do
        local nTaiKhoan = account_list[i]
        -- print("Kick account: " .. nTaiKhoan)
        KickOutAccount("".. nTaiKhoan .. "") -- 
    end

    local clear_file = openfile(filename, "w")
    if clear_file then
        closefile(clear_file)
    else
        -- print("Warning: Could not clear file: " .. filename)
    end
 
end


-- Trim function
function trim(s)
    return gsub(s, "^%s*(.-)%s*$", "%1")
end

function XepHangDuaTopTG()
	local file_path = "C:serverdulieu\\topTG.txt"
	local file = openfile(file_path, "r")
	if not file then
		 Msg2Player("Cannot open file: " .. file_path)
	else
		 Msg2Player("opened file: " .. file_path)
	end

	-- Read entire content
	local content = read(file, "*a")
	closefile(file)
	
	--Msg2Player(content)
	-- Process each line
	local lines = split_lines(content)
	local players = {}
	local header_skipped = false
	for i = 1, getn(lines) do
	
		local line = lines[i]
		
		--Msg2Player(i.." "..line)
		local name    = trim(strsub(line, 1, 20))
		local account = trim(strsub(line, 21, 36))
		local level   = trim(strsub(line, 37, 42))
		local Fexp     = trim(strsub(line, 43, 58))
		local ip      = trim(strsub(line, 59))
		if name and account and level and Fexp and ip then
			tinsert(players, {
				name = name,
				account = account,
				level = level,
				Fexp = Fexp,
				ip = ip
			})
		end
	end
	Msg2SubWorld("<color=yellow>Danh S¸ch Top Cao Thñ: ")
	-- Print players
	for i = 1, getn(players) do
		local p = players[i]
		Msg2SubWorld("<color=blue>TOP " .. i .. ": <color=green>" .. p.name ..
           "<color=white> - Level <color=yellow>" .. p.level ..
           "<color=white>, Exp <color=yellow>" .. p.Fexp ..
           "<color=white>, IP <color=cyan>" .. p.ip)
	end
end

-- [BW 23/08] Hon Chien da doi san 209 -> 210 (mainloidai.lua:46, nhuong 209 cho Loi dai ty vo);
-- ham nay hien KHONG duoc goi (dong 88 da chu thich tu truoc) - dong bo 4 cho w==209 -> 210 de
-- neu chu game bat lai thi khong pha tran bw tren 209.
-- [BOLDHC 30/08] da bo ham LoiDaiHonChien (201 dong).
-- Hoat dong Loi Dai Hon Chien bo han theo quyet dinh chu game 30/08.
-- Khong co ban Linux thay the. Ban goc o _dara\script\tinhnang\loidaihonchien.

function songbac(nHr,nMi)
	if (nHr == 11 and nMi > 45) or (nHr == 22 and nMi > 45)  then
	Msg2SubWorld("<color=green>[BÇu Cua] <color=red>Ph¸t Tµi Ph¸t Léc t¹i <color=yellow>Ba L¨ng HuyÖn 197/203.<color=red> Thêi gian cßn l¹i "..(59-nMi).." phót n÷a")
	return
	end
	if (nHr == 12 and nMi == 0) or (nHr == 23 and nMi == 0) then
	SetGlbMissionV(11 , 1)
	Msg2SubWorld("<color=green>[BÇu Cua] <color=red> L­ît ®Æt c­îc BÇu Cua thø 1 b¾t ®Çu. H·y ®Õn <color=yellow>Ba L¨ng HuyÖn 197/203 <color=red>®Ó tham gia ®Æt c­îc !")
	return
	end
	if GetGlbMissionVC(11) > 0 and GetGlbMissionVC(11) < 13 then
if mod(nMi,5) == 0 or nMi == 59 then

	ketqua1 = RandomNew(1,6)
	ketqua2 = RandomNew(1,6)
	ketqua3 = RandomNew(1,6)
	
 	ms = GetGlbMissionVC(37)
	check = floor(ms / 1000)
	if check == 5 then
		ketqua1 = floor(mod(ms,1000) / 100)
		ketqua2 = floor(mod(ms,100) / 10)
		ketqua3 = floor(mod(ms,10))
	end
	SetGlbMissionV(37, 0)
	if ketqua1 < 1 or ketqua1 > 6 then
			ketqua1 = RandomNew(1,6)
	end
	if ketqua2 < 1 or ketqua2 > 6 then
			ketqua2 = RandomNew(1,6)
	end	
	if ketqua3 < 1 or ketqua3 > 6 then
			ketqua1 = RandomNew(1,6)
	end
	
	Msg2SubWorld("KÕt Qu¶: <color=yellow>"..CheckTen(ketqua1).." <color=red>- <color=green>"..CheckTen(ketqua2).." <color=red>- <color=blue>"..CheckTen(ketqua3).."")
		
if getn(DataSongBac) == 0 then
Msg2SubWorld("<color=green>[BÇu Cua] <color=red> L­ît thø "..GetGlbMissionVC(11).." kh«ng cã ai ®Æt c­îc.")
SetGlbMissionV(11, GetGlbMissionVC(11) + 1)
return
end

Msg2SubWorld("<color=pink>Danh s¸ch nh÷ng ng­êi tróng th­ëng l­ît thø: "..GetGlbMissionVC(11).."")

XacDinh_NganLuong = 1000000
XacDinh_Vang = 1000000

	for i=1,getn(DataSongBac) do
		num = 0
		if DataSongBac[i][2] == ketqua1 then
				num = num + 1
		end
		if DataSongBac[i][2] == ketqua2 then
				num = num + 1
		end
		if DataSongBac[i][2] == ketqua3 then
				num = num + 1
		end
		
			if DataSongBac[i][3] == 1 then
				XacDinh_NganLuong = XacDinh_NganLuong - DataSongBac[i][4]
			else
				XacDinh_Vang = XacDinh_Vang - DataSongBac[i][4]
			end
			
		if num > 0 then
			thutien = (num+1) * DataSongBac[i][4]
			if DataSongBac[i][3] == 1 then
				Msg2SubWorld("<color=yellow>["..DataSongBac[i][1].."] <color=red>cã "..num.." con "..CheckTen(DataSongBac[i][2]).." nhËn ®­îc: <color=green>"..thutien.." v¹n l­îng")
				XacDinh_NganLuong = XacDinh_NganLuong + thutien
				vt = CheckDataNganLuong(DataSongBac[i][1])
				if vt == 0 then
					DataNganLuong[getn(DataNganLuong)+1] = {DataSongBac[i][1],thutien}
				else
					DataNganLuong[vt][2] = DataNganLuong[vt][2] + thutien
				end
			else
				Msg2SubWorld("<color=yellow>["..DataSongBac[i][1].."] <color=red> cã "..num.." con "..CheckTen(DataSongBac[i][2]).." nhËn ®­îc: <color=yellow>"..thutien.." Xu")
				XacDinh_Vang = XacDinh_Vang + thutien
				
				vt = CheckDataTienVang(DataSongBac[i][1])
				if vt == 0 then
					DataTienVang[getn(DataTienVang)+1] = {DataSongBac[i][1],thutien}
				else
					DataTienVang[vt][2] = DataTienVang[vt][2] + thutien
				end
			end
		end
	end
	DataSongBac = {}
	thoigian = tonumber(date("%H%M%d%m"))
	if XacDinh_NganLuong > 1000000 then
		LoginLog = openfile("dulieu/SongBac/BauCua_KetQua_Van.txt", "a");
		if LoginLog then
		write(LoginLog,"["..thoigian.."] Thua: "..(XacDinh_NganLuong-1000000).." van luong\n");
		end
		closefile(LoginLog)
	else
		LoginLog = openfile("dulieu/SongBac/BauCua_KetQua_Van.txt", "a");
		if LoginLog then
		write(LoginLog,"["..thoigian.."] Thang: "..(1000000-XacDinh_NganLuong).." van luong\n");
		end
		closefile(LoginLog)
	end
	
	
	if XacDinh_Vang > 1000000 then
		LoginLog = openfile("dulieu/SongBac/BauCua_KetQua_Vang.txt", "a");
		if LoginLog then
		write(LoginLog,"["..thoigian.."] Thua: "..(XacDinh_Vang-1000000).." Xu\n");
		end
		closefile(LoginLog)
	else
		LoginLog = openfile("dulieu/SongBac/BauCua_KetQua_Vang.txt", "a");
		if LoginLog then
		write(LoginLog,"["..thoigian.."] Thang: "..(1000000-XacDinh_Vang).." Xu\n");
		end
		closefile(LoginLog)
	end
	
	SetGlbMissionV(11, GetGlbMissionVC(11) + 1)
	luubang()
end
end
end
function sukien_vantieu(nHr,nMi)
		if (nHr == 14 and  nMi > 50) and nHr < 21 then
			Msg2SubWorld("<color=green>[Sù KiÖn]<color><color=yellow>VËn Tiªu ChÝnh thøc b¾t ®Çu tõ  <color=green>15h ®Õn 21h<color> <color=yellow>nhanh ch©n tham gia t¹i Thµnh §« to¹ ®é 375 - 315!!!")
		end
end

function ThongBao()
	-- Msg2SubWorld("<color=yellow>*§óng 19h sÏ xuÊt hiÖn NPC chÝnh thøc sù kiÖn ®ua TOP !!!")
	-- Msg2SubWorld("<color=green>*NhËn Hç Trî CÊp 10 qua Map Hoa S¬n or ra ngoµi Ba L¨ng HuyÖn ®Ó train. ChØ cã lªn ®­îc c¸c map luyÖn c«ng ë trong ThÇn Hµnh Phï")
	Msg2SubWorld("<color=yellow>*KÕt thóc ®ua top 12h ngµy 13 th¸ng 9 n¨m 2024")
	Msg2SubWorld("<color=green>*PhÝm t¾t: <color=yellow>CTRL + F<color> më Autoingame, <color=yellow>F<color> më - t¾t Autoingame, <color=yellow>ALT + PhÝm C¸ch<color> hiÖn tªn item !!")
	Msg2SubWorld("<color=green>*PhÝm t¾t:  <color=yellow>SHIFT + chuét ph¶i<color> vµo trang bÞ nÐm nhanh trang bÞ cïng tªn ra khái hµnh trang, <color=yellow>CTRL + Z<color> më r­¬ng nhanh !!")
	Msg2SubWorld("<color=green>*Truy cËp web vlngaothe.com ®Ó xem l­u r­¬ng c¸c map train 8x 9x ®Ó cã thÓ quay l¹i ®iÓm cñ !")
end

function sukien_kiemmonquan(nHr,nMi)
	for i=1,getn(TAB_TIME_KMQ) do
		if(nHr == TAB_TIME_KMQ[i][1] and nMi == TAB_TIME_KMQ[i][2]) then
			local nSubWorldId
			for i=1, getn(MAP_KMQ) do
				nSubWorldId = SubWorldID2Idx(MAP_KMQ[i])
				if (nSubWorldId >= 0) then
					SubWorld = nSubWorldId
					if(IsMission(MS_KIEMMONQUAN) == 0) then
						OpenMission(MS_KIEMMONQUAN)
					end
					StartMissionTimer(MS_KIEMMONQUAN, 8, TIME_KMQKT * 60 * 18)--nMissionId, nTimerId, nTimeInterval so phut ket thuc mission
					print(format("===> Kiem Mon Quan %d:%d Map[%d]<===", nHr , nMi,MAP_KMQ[i]))
				else
					print(format("===> Kiem Mon Quan %d:%d chua mo Map[%d] <===", nHr , nMi, MAP_KMQ[i]))
				end
			end
		end
	end	
end

-- [GOHECU 30/08] da go ham sukien_liendau (32 dong). He lien_dau cu da doi di tu 20/08;
-- ham nay Include thu muc KHONG CON TON TAI. Lien Dau that = ban WLLS,
-- chay doc lap, khong qua cong tac BAT_LIENDAU.

function sukien_tongkim(nHr,nMi)
	--if (nSubWorldId >= 0) then
	for i=1,getn(TAB_TIME_TONG_KIM) do
	if (nHr == TAB_TIME_TONG_KIM[i][1] and nMi == TAB_TIME_TONG_KIM[i][2]) then --Check thoi gian
		Msg2SubWorld("<color=green>[Sù KiÖn]<color=yellow>TrËn ®¸nh Tèng Kim ®ang hÕt søc c¨ng th¼ng, tr­íc m¾t ®ang ë giai ®o¹n b¸o danh. §iÒu kiÖn tham gia: ®¼ng cÊp tõ 80. phÝ b¸o danh 2 v¹n l­îng")
		end
	end
	--end
	--Msg2SubWorld("<color=green>[Sù KiÖn]<color=yellow>TrËn ®¸nh Tèng Kim ®ang hÕt søc c¨ng th¼ng, tr­íc m¾t ®ang ë giai ®o¹n b¸o danh. §iÒu kiÖn tham gia: ®¼ng cÊp tõ 80. phÝ b¸o danh 2 v¹n l­îng")
	local nSubWorldId = SubWorldID2Idx(MAP_TK_TC)
	if (nSubWorldId >= 0) then
		for i=1,getn(TAB_TIME_TONG_KIM) do
			if (nHr == TAB_TIME_TONG_KIM[i][1] and nMi == TAB_TIME_TONG_KIM[i][2]) then --Check thoi gian
				DELDULIEU() -- xoa du lieu limited 1 account login TONGKIM

				AddGlobalCountNews("B¸o danh Tèng Kim ®· b¾t ®Çu thêi gian b¸o danh trong vßng "..TIME_BD_TK.." phót",3)		
				SubWorld = nSubWorldId
				local nPThuc = RandPThucTongKim() local nMSLadderParam = 6
				OpenMission(MS_TONGKIM,TAB_PHUONGTHUC[nPThuc][2],nMSLadderParam,1,2,3)
				SetMission(M_SOTRAN,TAB_TIME_TONG_KIM[i][4])
				SetMission(M_HINHTHUC, nPThuc)
				StartMissionTimer(MS_TONGKIM,1, TIME_BD_TK*1080)--so phut de chinh thuc bat dau
				if (nPThuc == PT_BaoVeNguyenSoai) then
					StartMissionTimer(MS_TONGKIM,2, TIME_NS_TK*1080)--thoi gian nguyen soai ra
				end
				StartMissionTimer(MS_TONGKIM,3, TIME_KT_TK*1080)--tong thoi gian ca tran	
				--AddGlobalCountNewsEx("Tèng kim ®· ®­îc khëi ®éng ph­¬ng thøc "..TAB_PHUONGTHUC[nPThuc][2].." ",3)
				AddGlobalCountNewsEx("Tèng kim ®· ®­îc khëi ®éng ph­¬ng thøc "..GetMissionName(MS_TONGKIM).." ",3)
				Msg2SubWorld("<color=green>[Sù KiÖn]<color><color=yellow>Tèng kim ®· ®­îc khëi ®éng ph­¬ng thøc  <color=green>"..GetMissionName(MS_TONGKIM).."<color> <color=yellow>nhanh ch©n b¸o danh. §iÒu kiÖn tham gia:2 v¹n l­îng")
				Msg2SubWorld("<color=green>[Sù KiÖn]<color=yellow>TrËn ®¸nh Tèng Kim ®ang hÕt søc c¨ng th¼ng, tr­íc m¾t ®ang ë giai ®o¹n b¸o danh. §iÒu kiÖn tham gia: ®¼ng cÊp tõ 80. phÝ b¸o danh 2 v¹n l­îng")
		    
				
				print(format("===>Tong Kim Dai Chien %d:%d phuong thuc %d<===",nHr,nMi,nPThuc))
				local nViTri = RandPosTK() --Ngau nhien vi tri tong kim ben tren va ben duoi
				SetMission(M_VITRI_TRENDUOI, nViTri)
				addnpctongkimtrungcap(nViTri)
			end
		end
		SubWorld = nSubWorldId
		local RestTK = GetMSRestTime(MS_TONGKIM,1)--thoi gian bao danh con lai
		local Minute
		if (RestTK > 0) then
			Minute = floor(RestTK/1080)
			-- if(Minute == 2 or Minute == 4) then
			if(Minute > 0) then
				AddGlobalCountNews("B¸o danh Tèng Kim ®· b¾t ®Çu thêi gian b¸o danh trong vßng "..TIME_BD_TK.." phót", 3)
				Msg2SubWorld(format("<color=green>[Sù KiÖn]<color=yellow>B¸o danh Tèng Kim ®· b¾t ®Çu thêi gian b¸o danh trong vßng <color>%d<color=yellow> phót<color>", Minute))
				--Msg2SubWorld(format("Thêi gian b¸o danh Tèng Kim cßn l¹i lµ <color=yellow>%d<color> phót.", Minute))
				AddGlobalCountNewsEx(format("Thêi gian b¸o danh Tèng Kim cßn l¹i lµ %d phót.", Minute), 3)
			end
		end
		RestTK = GetMSRestTime(MS_TONGKIM,3)--thoi gian con lai cua tran danh'
		if (RestTK > 0) then
			Minute = floor(RestTK/1080)
			if(Minute > 0) then
				local nTongAcc = GetMissionV(M_TICHLUYA)
				local nKimAcc  = GetMissionV(M_TICHLUYB)
				for dataindex=1,GetMSPlayerCount(MS_TONGKIM) do
					PlayerIndex = MSDIdx2PIdx(MS_TONGKIM, dataindex)
					if(GetPMParam(MS_TONGKIM, dataindex, 0) == 1) then --dang online
						-- UpRankParam(MS_TONGKIM, 1);
						Msg2Player(format("[Tèng %d] : [Kim  %d]",nTongAcc,nKimAcc)) --thong bao ®iÓm sè 2 bªn
					end
				end
			end
		end
	end
end



-- [GOHECU 30/08] da go ham sukien_loidaibanghoi (31 dong). Ban thay: Loi Dai bang hoi ban JX2 (CTC_JX2_Tick + TimerTask 16/17 citywar_arena).

-- [GOHECU 30/08] da go ham sukien_congthanh (57 dong). Ban thay: Cong Thanh ban JX2 (BAT_CTC_JX2 = 1, CTC_JX2_Tick).

function sukien_bosshk(nHr,nMi)
	for i=1,getn(TAB_TIME_BOSS_TIEU) do
		if(nHr == TAB_TIME_BOSS_TIEU[i][1] and TAB_TIME_BOSS_TIEU[i][2] == nMi) then
			addnpcbosstieuhk()
			print(format("===>Boss tieu hoang kim %d:%d <===",nHr,nMi))
		end
	end

	for z=1,getn(TAB_TIME_BOSS_DAI) do
		if(nHr == TAB_TIME_BOSS_DAI[z][1] and TAB_TIME_BOSS_DAI[z][2] == nMi) then
			addnpcbossdaihk()
			print(format("===>Boss dai hoang kim %d:%d <===",nHr,nMi))
		end
	end
	for y=1,getn(TAB_TIME_BOSS_SV) do
		if(nHr == TAB_TIME_BOSS_SV[y][1] and TAB_TIME_BOSS_SV[y][2] == nMi) then
			addnpcbossserver()
			print(format("===>Boss server hoang kim %d:%d <===",nHr,nMi))
		end
	end
end

function sukien_hathuyhoang(nHr , nMi) --Hat huy hoang
	for i=1,getn(TAB_TIME_HAT_HH) do	
		if(nHr == TAB_TIME_HAT_HH[i][1] and TAB_TIME_HAT_HH[i][2] == nMi) then
			Add_HatHuyHoang()
			Msg2SubWorld("<color=green>[Sù KiÖn]<color=yellow>'H¹t Huy Hoµng' b¾t ®Çu tíi giê thu ho¹ch. C¸c cao thñ h·y ®Õn gÆp Xa Phu T­¬ng D­¬ng ®Ó ®Õn KiÕm M«n Quan tham gia!")
			print(format("===>Hat huy hoang xuat hien %d:%d <===",nHr,nMi))
		end
	end
end

-- [GOHECU 30/08] da go ham sukien_vuotai (28 dong). Ban thay: Vuot Ai trong cum 3 hoat dong ban Linux (BAT_HD3 = 1, HD3_Tick).

function sukien_trangnguyen(nHr,nMi)
	for i=1,getn(TIME_CAU_HOI_TN) do
		if(nHr == TIME_CAU_HOI_TN[i][1] and nMi == TIME_CAU_HOI_TN[i][2]) then
			addnpccauhoi()
		end
	end	
end

-- [GOHECU 30/08] da go ham sukien_phonglangdo (40 dong). Ban thay: Phong Lang Do trong cum 3 hoat dong ban Linux (BAT_HD3 = 1, HD3_Tick).

function sukien_trongbanghoi(nHr,nMi)
	for j=1,getn(TIME_TRONGBH) do
		if(TIME_TRONGBH[j][1] == nHr and TIME_TRONGBH[j][2] == nMi) then
			add_trongbanghoi()
		end
		if(TIME_TRONGBH[j][3] == nHr and TIME_TRONGBH[j][4] == nMi) then
			local msgTBTrong = format("Cßn <color=yellow>5 phót<color> n÷a <color=pink>Trèng Kh¶i Hoµn<color> xuÊt hiÖn t¹i <color=yellow>BiÖn Kinh<color>, quý bang chñ cïng c¸c thµnh viªn bang mau mau ®Õn ®Ó chuÈn bÞ tranh ®o¹t. ")
			Msg2SubWorld(msgTBTrong)
			AddGlobalCountNews(msgTBTrong,3)--thong bao
			add_npctrongbanghoi()
		end
	end
end
mangtam_duatop = {}
mangIP_duatop = {} 
NgauNhienPhut = 0

function XepHangDuaTop()
    local nYr, nMo, nDy, nHr, nMi, nSe, nDyfW = GetTimeNow()	
    if nDy >= 01 and mod(nHr, 1) == 0 and nMi == 7 then
        mangtam_duatop = {}
        mangIP_duatop = {} 
        
        for p = 1, 40 do 
            mangtam_duatop[getn(mangtam_duatop) + 1] = {"", 0, 0}
        end
        
        for k = 1, GetPlayerCount() do
            PlayerIndex = k
            vt = check_min_mangtam_duatop_duatop()
            if (GetLevel() > mangtam_duatop[vt][2]) then
                mangtam_duatop[vt][1] = GetName()
                mangtam_duatop[vt][2] = GetLevel()
                mangtam_duatop[vt][3] = GetExp()
            
                mangIP_duatop[vt] = GetIP()
            elseif (GetLevel() == mangtam_duatop[vt][2]) then
                if (GetExp() > mangtam_duatop[vt][3]) then
                    mangtam_duatop[vt][1] = GetName()
                    mangtam_duatop[vt][2] = GetLevel()
                    mangtam_duatop[vt][3] = GetExp()
                   
                    mangIP_duatop[vt] = GetIP()
                end
            end
        end
        
        PlayerIndex = idx
        SapXepHangDuaTop()
    end
    
    if nDy >= 01 and mod(nHr, 1) == 0 and nMi == 7 then
        Msg2SubWorld("<color=yellow>Danh s¸ch Top Cao Thñ: ")
        thoigian = tonumber(date("%H%M%d%m"))
        LoginLog = openfile("dulieu/LichSuDuaTop.txt", "a");	
        NgauNhienPhut = RandomNew(1, 59)
        
        if LoginLog then
            for u = 1, getn(mangtam_duatop) do
                if mangtam_duatop[u][1] ~= "" then
                    
                    Msg2SubWorld(""..u..". ["..mangtam_duatop[u][1].."] - Lvl: "..mangtam_duatop[u][2].." - Exp: "..mangtam_duatop[u][3].." - IP: "..mangIP_duatop[u])
                    write(LoginLog,""..mangtam_duatop[u][1].." - "..mangtam_duatop[u][2].." - "..mangtam_duatop[u][3].." - IP: "..mangIP_duatop[u].." - Time: "..thoigian.."\n");
                end
            end
        end
        
        closefile(LoginLog)
   end
   end
   
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
