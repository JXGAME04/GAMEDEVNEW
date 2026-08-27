--Author: Fong KiÒu
--Date: 28/11/2020
--Function: Timer toan server

Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_server.lua")
-- [WLLS port 20/08/2026] da go he lien_dau cu; sukien_liendau (dong 68) von da tat
Include("\\script\\tinhnang\\loidai\\lib_loidai.lua")
Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\mobinhtk.lua")
Include("\\script\\tinhnang\\boss_hoangkim\\lib_bosshk.lua")
Include("\\script\\tinhnang\\vuot_ai\\lib_vuotai.lua")
Include("\\script\\tinhnang\\phonglangdo\\lib_phonglangdo.lua")
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

function RunTime()
	
	 dofile("script/timerserver.lua")
	
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
	-- sukien_liendau(nHr,nDy,nMi,nMo)
	sukien_tongkim(nHr,nMi)
	-- [3HD 25/08] lich Phong Lang Do + Vuot Ai (thay tang S3Relay ban Linux)
	if (HD3_Tick ~= nil) then HD3_Tick(nHr, nMi) end
	if (YDBZ_Tick ~= nil) then YDBZ_Tick(nHr, nMi) end	-- [VIEMDE 26/08] lich 8h25/10h25/14h25/16h25/18h25/20h25/22h25
	-- [21/08] cong thanh JX2 + loi dai bang hoi CN: 0h/18h/19h/20h (guard ngay trong ham)
	if (CTC_JX2_Tick ~= nil) then
		CTC_JX2_Tick(nDyfW, nHr, nMi)
	end
	-- sukien_congthanh(nDyfW,nHr,nMi)
	-- sukien_hathuyhoang(nHr,nMi)
	-- sukien_bosshk(nHr,nMi)
	-- sukien_vuotai(nHr,nMi)
	-- sukien_phonglangdo(nHr,nMi)
	-- sukien_trangnguyen(nHr,nMi)
	-- sukien_loidaibanghoi(nDyfW,nHr,nMi)
	-- sukien_trongbanghoi(nHr,nMi)
	-- sukien_kiemmonquan(nHr,nMi)
	-- sukien_vantieu(nHr,nMi)
	-- XepHangDuaTop()
	-- songbac(nHr,nMi)
	-- LoiDaiHonChien(nHr,nMi)
	-- check_and_kick(nMi,nSe) -- check kich tai khoan
	-- pubg_runner(nHr,nMi)
	
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
	local file_path = "C:\server\dulieu\\topTG.txt"
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
function LoiDaiHonChien(nHr,nMi)
-- LOI DAI HON CHIEN

if (nHr == 21 or nHr == 22) and nMi < 35 then
	Msg2SubWorld("HiÖn ®ang trong thêi gian b¸o danh <color=yellow>L«i §µi Hçn ChiÕn<color=red>. H·y ®Õn <color=green>Ba L¨ng HuyÖn 204/201 gÆp L«i §µi Hæn ChuyÕn <color=red>®Ó ®¨ng ký tham gia. Thêi gian cßn l¹i: <color=pink>"..(10 - nMi).." Phót!")
end
if (nHr == 16 or nHr == 22) and nMi == 52 then
local soluongthamgia = 0
local idxtemp = PlayerIndex
for i=1,GetCountPlayerMax() do
PlayerIndex = i
	local w, _, _ = GetWorldPos()
	if w == 210 then
					soluongthamgia = soluongthamgia + 1
					SetCurCamp(4) -- chuyÓn qua mµu ®á
					SetFightState(1)  -- tr¹ng th¸i chiÕn ®Êu
					SetTaskTemp(1,0)
					SetPKMode(2,1) -- chuyÓn sang ®å s¸t
					AddSumExp(50000000)
					AddItemSL(4844,100,0)
					

					SetTask(TASK_DSK, GetTask(TASK_DSK) + 20)
					Msg2Player("B¹n nhËn ®­îc 50.000.000 kinh nghiÖm")
					Msg2Player("B¹n nhËn ®­îc 20 ®iÓm sù kiÖn")
					Msg2SubWorld("Chóc mõng "..GetName().." tham gia l«i ®µi hçn chiÕn nhËn ®­îc 50.000.000 K×nh nghiÖm vµ 20 ®iÓm sù kiÖn!!! ")
					Msg2Player("TrËn ®Êu b¾t ®Çu ............")
					
	end
end
 PlayerIndex = idxtemp
Msg2SubWorld("<color=pink>L«i §µi Hçn ChiÕn b¾t ®Çu, h·y quyÕt ®Êu ®Õn khi cßn ng­êi cuèi cïng sèng sãt")
Msg2SubWorld("Sè l­îng tham gia trËn nµy lµ: <color=yellow>"..soluongthamgia.." ng­êi. <color=red>Thêi gian thi ®Êu tèi ®a: <color=yellow>20 Phót")
end
if (nHr == 16 or nHr == 22) and nMi > 11 and nMi <= 54 then
	local count = 0
	local lastName = ""
	local idxtemp = PlayerIndex
	for i = 1, GetCountPlayerMax() do
		PlayerIndex = i
		local w, _, _ = GetWorldPos()
		if w == 210 and GetFightState() == 1 then
			count = count + 1
			lastName = GetName()
		end
	end
	PlayerIndex = idxtemp

	if count == 1 then
		Msg2SubWorld("<color=yellow>C«ng bè kÕt qu¶: ")
		NguoiThangCuoc = lastName

		for i = 1, GetCountPlayerMax() do
			PlayerIndex = i
			local w, _, _ = GetWorldPos()
			if w == 210 then
				SetDeathScript("");
				SetCurCamp(GetCamp())
				NewWorld(53,1619, 3185)
				ReSetMask();
				SetFightState(0)
				SetPKMode(0,0) -- tr¶ l¹i pk tù do
				SetPunish(0);
				SetCreateTeam(1);
			end
		end

		idxtemp = PlayerIndex
		for i = 1, GetCountPlayerMax() do
			PlayerIndex = i
			if GetName() == NguoiThangCuoc then
				Msg2SubWorld("<color=pink>ChØ cßn <color=yellow>"..GetName().."<color> sèng sãt ! KÕt thóc trËn L«i §µi Hçn ChiÕn")
				AddSumExp(500000000)
				for i=1,5 do
					AddItem(4,random(753,770),0,0,0,0,0)
				end
				Msg2SubWorld("B¹n nhËn ®­îc 5 m¶nh hoµng kim AB")
				Msg2SubWorld("- 500 triÖu kinh nghiÖm")
				local sx = RandomNew(1,100)
				if sx < 30 then
					AddItem(4,353,0,0,0,0,0 )
					Msg2SubWorld("- 1 Tinh Hång B¶o Th¹ch")
				elseif sx < 50 then
					for i=238,240 do
					AddItem(4,i,0,0,0,0,0 ) end
					Msg2SubWorld("- 1 Thuû Tinh Random")
				elseif sx < 70 then
					for t=1,2 do
						AddItem(4,353,0,0,0,0,0 )
					end
					Msg2SubWorld("- 2 Tinh Hång B¶o Th¹ch")		
				elseif sx < 90 then	
					for t=1,5 do
						AddItem(6,1,71,0,0,0,0)
					end
					Msg2SubWorld("- 5 Tiªn Th¶o Lé")		
				elseif sx < 95 then
					AddItem(6,1,26,0,0,0,0)
					Msg2SubWorld("- 1 Vâ L©m MËt TÞch")
				else
					AddItem(6,1,4822,0,0,0,0)
					Msg2SubWorld("- 1 TÈy Tñy Kinh")
				end
			end
		end
		PlayerIndex = idxtemp
	end
end
MangTam = {}
if (nHr == 16 or nHr == 22)  and nMi == 56 then
idxtemp = PlayerIndex
for i=1,GetCountPlayerMax() do
PlayerIndex = i
	w,x,y = GetWorldPos()
	if w == 210 then
			if GetFightState() == 1 then
				MangTam[getn(MangTam)+1] = {GetName(), GetTaskTemp(50)}
				thoigian = tonumber(date("%H%M%d%m"))
				LoginLog = openfile("dulieu/LoiDaiHonChien_MangTam.txt", "a");
				if LoginLog then
				write(LoginLog,""..GetAccount().." - "..GetName().." - "..GetTaskTemp(50).." - Time: "..thoigian.."\n");
				end
				closefile(LoginLog)
				SetDeathScript("");
				SetCurCamp(GetCamp())
				NewWorld(53,1619, 3185)
				ReSetMask();
				SetFightState(0)
				SetPKMode(0,0) -- tr¶ l¹i pk tù do
				SetPunish(0);
				SetCreateTeam(1);
			end
	end
end
PlayerIndex = idxtemp	
NguoiThangCuoc = ""
if getn(MangTam) == 0 then
	Msg2SubWorld("L«i §µi Hçn ChiÕn kh«ng cã ai tham gia hoÆc ®· tho¸t khái game. TrËn ®Êu kh«ng cã ng­êi th¾ng cuéc")	
elseif getn(MangTam) == 1 then
	NguoiThangCuoc = MangTam[1][1]
else
	Msg2SubWorld("<color=yellow>C«ng bè kÕt qu¶: ")
	name = MangTam[1][1]
	dame = MangTam[1][2]
	Msg2SubWorld("- "..MangTam[1][1].." - Tæng dame: "..MangTam[1][2].."")
	for k=2,getn(MangTam) do
			Msg2SubWorld("- "..MangTam[k][1].." - Tæng dame: "..MangTam[k][2].."")
			if MangTam[k][2] > dame then
				name = MangTam[k][1]
				dame = MangTam[k][2]
			end
	end	
	NguoiThangCuoc = name
end
Msg2SubWorld("TrËn L«i §µi Hçn ChiÕn ngµy h«m nay ®· kÕt thóc. PhÇn th¾ng thuéc vÒ: <color=yellow>"..NguoiThangCuoc.." ")
idxtemp = PlayerIndex
for i=1,GetCountPlayerMax() do
	PlayerIndex = i
	if GetName() == NguoiThangCuoc then
		Msg2SubWorld("<color=pink>Chóc mõng "..GetName().." nhËn ®­îc:")
		AddSumExp(500000000)
			for i=1,5 do
			AddItem(4,random(903,942),0,0,0,0,0 )
			end
		Msg2SubWorld("B¹n nhËn ®­îc 5 m¶nh hoµng kim")
		Msg2SubWorld("- 500 triÖu kinh nghiÖm")
		sx = RandomNew(1,100)
		if sx < 30 then
			AddItem(4,353,0,0,0,0,0 )
			Msg2SubWorld("- 1 Tinh Hång B¶o Th¹ch")
		elseif sx < 50 then
			for i=238,240 do
			AddItem(4,i,0,0,0,0,0 ) end
			Msg2SubWorld("- 1 Thuû Tinh Random")
		elseif sx < 70 then
			for t=1,2 do
				AddItem(4,353,0,0,0,0,0 )
			end
			Msg2SubWorld("- 2 Tinh Hång B¶o Th¹ch")		
		elseif sx < 90 then	
			for t=1,5 do
				AddItem(6,1,71,0,0,0,0)
			end
			Msg2SubWorld("- 5 Tiªn Th¶o Lé")		
		elseif sx < 95 then
			AddItem(6,1,26,0,0,0,0)
			Msg2SubWorld("- 1 Vâ L©m MËt TÞch")
		else
			AddItem(6,1,4822,0,0,0,0)
			Msg2SubWorld("- 1 TÈy Tñy Kinh")
		end
	
		
		
	end
end
PlayerIndex = idxtemp

end

end

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

function sukien_liendau(nHr,nDy,nMi,nMo)
local nYr,nMo,nDy,nHr,nMi,nSe,nDyfW = GetTimeNow()	
	if (nDy >= 8 and nDy <= 28) then
		if (nHr == TIME_LIEN_DAU[1] and (nMi == 0 or nMi == 15 or nMi == 30 or nMi == 45)) then
			AddGlobalCountNews("HiÖn ®ang trong gian ®o¹n Vâ l©m liªn ®Êu, h«m nay lóc <color=yellow>"..TIME_LIEN_DAU[1]..":00<color> cã tiÕn hµnh <color=yellow>4<color> trËn. Quý nh©n sü cã thÓ ®Õn gÆp Quan viªn b¸o danh tham gia. ",3)
			Msg2SubWorld("HiÖn ®ang trong gian ®o¹n Vâ l©m liªn ®Êu, h«m nay lóc <color=yellow>"..TIME_LIEN_DAU[1]..":00<color> cã tiÕn hµnh <color=yellow>4<color> trËn. Quý nh©n sü cã thÓ ®Õn gÆp Quan viªn b¸o danh tham gia. ")
		
		end
	end
	if(nDy >= 8 and nDy <= 28) then --xo¸ d÷ liÖu liªn ®Êu th¸ng tr­íc ngµy 9
		if(nMo == 12) then
			nMo = 1
		else 
			nMo = nMo -1
		end
		
		Include("\\script\\tinhnang\\lien_dau\\data\\"..nMo.."\\danhsachtrung.lua")
		Include("\\script\\tinhnang\\lien_dau\\data\\"..nMo.."\\danhsachcao.lua")
		if(getn(TAB_DANHSACHTRUNG) > 0) then
			TAB_DANHSACHTRUNG = {}
			local DanhSachTrungLD = TaoBang(TAB_DANHSACHTRUNG,"TAB_DANHSACHTRUNG")
			SaveData("script/tinhnang/lien_dau/data/"..nMo.."/danhsachtrung.lua",DanhSachTrungLD)			
			print(format("===>Del data lien dau Trung Cap thang %d<===", nMo))
		end
		if(getn(TAB_DANHSACHCAO) > 0) then
			TAB_DANHSACHCAO = {}
			local DanhSachCaoLD = TaoBang(TAB_DANHSACHCAO,"TAB_DANHSACHCAO")
			SaveData("script/tinhnang/lien_dau/data/"..nMo.."/danhsachcao.lua",DanhSachCaoLD)
			print(format("===>Del data lien dau Cao Cap thang %d<===", nMo))
		end		
	end --end xo¸ d÷ liÖu liªn ®Êu th¸ng tr­íc
end

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



function sukien_loidaibanghoi(nDyfW,nHr,nMi)
	local szMsg = format("HiÖn t¹i ®ang tiÕn hµnh tranh ®Êu c«ng thµnh %s.", CITYWAR_NAME)
	szMsg = format("%s Bang héi ch­a chiÕm thµnh cÊp 18 trë lªn tham gia, 20h30 thø 5 c¸c bang héi sÏ b­íc vµo thi ®Êu l«i ®µi thÓ thøc 16 vs 16 bang nµo cã ®iÓm cao nhÊt sÏ nhËn ®­îc quyÒn c«ng thµnh chiÕn ngµy h«m sau. ",szMsg)
	local nSubWorldId = SubWorldID2Idx(MapTab[1][1])
	if (nSubWorldId >= 0) then
		for i=1,getn(TIME_LOI_DAIBH) do
			SubWorld = nSubWorldId
			if (nHr == TIME_LOI_DAIBH[i][1] and nMi == TIME_LOI_DAIBH[i][2] and nDyfW == TIME_LOI_DAIBH[i][5]) then 
				ClearTongDataLD()
				AddGlobalCountNews(format("HiÖn t¹i ®ang tiÕn hµnh b¸o danh c«ng thµnh %s thêi gian ®Õn 20:30", CITYWAR_NAME),1)
				Msg2SubWorld(format("HiÖn t¹i ®ang tiÕn hµnh b¸o danh c«ng thµnh <color=yellow>%s<color> thêi gian ®Õn 20:30", CITYWAR_NAME))
				print(format("===>ClearTongDataLD Loi dai bang hoi %d:%d bat dau bao danh%d<===",nHr,nMi,nDyfW))
			end
			if (nHr == TIME_LOI_DAIBH[i][3] and nMi == TIME_LOI_DAIBH[i][4] and nDyfW == TIME_LOI_DAIBH[i][5]) then 
				OpenMission(MS_LOIDAIBH)
				StartMissionTimer(MS_LOIDAIBH,6, 10*1080)--thoi gian bat dau sau 10p
				StartMissionTimer(MS_LOIDAIBH,7, 20*1080)--thoi gian ket thuc sau 20p				
				AddGlobalCountNews(szMsg,1)
				print(format("===>Loi dai bang hoi %d:%d bat dau tranh dau%d<===",nHr,nMi,nDyfW))
			end
			local RestSeconBD = GetMSRestTime(MS_LOIDAIBH, 6) / 18
			local RestSeconKT = GetMSRestTime(MS_LOIDAIBH, 7) / 18
			if(RestSeconBD > 0) then
				Msg2MSAll(MS_LOIDAIBH,format("L«i ®µi bang b¾t ®Çu sau %d gi©y.", RestSeconBD))
			end
			if(RestSeconBD == 0) then
				Msg2MSAll(MS_LOIDAIBH,format("L«i ®µi bang kÕt thóc sau %d gi©y.", RestSeconKT))
			end
		end
	end
end

function sukien_congthanh(nDyfW,nHr,nMi)
	local sThongbao = "C«ng Thµnh ChiÕn thµnh <color=red> %s <color> ®· s½n sµng víi sù tham chiÕn cña phe thñ <color=fire> %s <color> vµ phe c«ng <color=pink> %s <color> quý nh©n sü hai bªn nhanh chãng di chuyÓn vµo khu vùc chê."
	local nSubWorldId = SubWorldID2Idx(ID_MAP_CTC)
	if (nSubWorldId >= 0) then
		for i=1,getn(TAB_TIME_CONG_THANH) do 
			if (nHr == TAB_TIME_CONG_THANH[i][4] and nMi == TAB_TIME_CONG_THANH[i][5] and nDyfW == TAB_TIME_CONG_THANH[i][6]) then --Check thoi gian		
				SubWorld = nSubWorldId
				if(NAME_BANGTHU == "0") then
					logHoatDong("CTC "..CITYWAR_NAME.." kh«ng cã phe thñ kÕt thóc")
					AddGlobalCountNews(format("Thµnh %s m­a thuËn giã hoµ ch­a cã bang héi cai trÞ. Quý bang héi b¸o danh ®Ó tranh ®o¹t. ",CITYWAR_NAME),3)
				elseif(NAME_BANGCONG == "0") then
					ClearTong2Data() --xo¸ phe c«ng khái db vµ update phe thñ nhËn th­ëng 0
					logHoatDong("CTC "..CITYWAR_NAME.." kh«ng cã phe c«ng kÕt thóc phe thñ tiÕp tôc thèng trÞ")
					AddGlobalCountNews(format("Thµnh %s m­a thuËn giã hoµ víi sù thèng trÞ cña %s bang. ",CITYWAR_NAME,NAME_BANGTHU),3)
					Msg2SubWorld(format("Thµnh <color=yellow>%s <color>m­a thuËn giã hoµ víi sù thèng trÞ cña<color=yellow> %s<color> bang.", CITYWAR_NAME,NAME_BANGTHU))
				else
					local nMSLadderParam = 6
					OpenMission(MS_CTHANHCHIEN, "C«ng Thµnh ChiÕn",nMSLadderParam,1,2,3)--add by phong kiÒu cã MissionLadder
					StartMissionTimer(MS_CTHANHCHIEN,1, CTC_MINUS_BD*1080)--so phut de chinh thuc bat dau
					StartMissionTimer(MS_CTHANHCHIEN,2, CTC_MINUS_KT*1080)--thoi gian toan tran				
					AddGlobalCountNews(format(sThongbao,CITYWAR_NAME,NAME_BANGTHU, NAME_BANGCONG),3)
					Msg2SubWorld(format(sThongbao, CITYWAR_NAME,NAME_BANGTHU, NAME_BANGCONG))
					print(format("===>Cong Thanh Chien %d:%d bat dau bao danh %d<===",nHr,nMi,nDyfW))
				end
			end
		end
		SubWorld = nSubWorldId
		local RestTK = GetMSRestTime(MS_CTHANHCHIEN,1)--thoi gian bao danh con lai
		local Minute
		if (RestTK > 0) then
			Minute = floor(RestTK/1080)
			if(Minute == 7 or Minute == 3) then
				AddGlobalCountNews(format(sThongbao,CITYWAR_NAME,NAME_BANGTHU,NAME_BANGCONG),3)
				AddGlobalCountNewsEx(format("Thêi gian vµo chiÕn tr­êng c«ng thµnh cßn l¹i %d phót.",Minute),3)
				Msg2SubWorld(format("Thêi gian vµo chiÕn tr­êng c«ng thµnh cßn l¹i <color=yellow>%d<color> phót.", Minute))
			end
		end
		RestTK = GetMSRestTime(MS_CTHANHCHIEN,2)--thoi gian con lai cua tran danh'
		if (RestTK > 0) then
			Minute = floor(RestTK/1080)
			if(Minute > 0) then
				local nTongAcc = GetMSPlayerCount(MS_CTHANHCHIEN,1)
				local nKimAcc  = GetMSPlayerCount(MS_CTHANHCHIEN,2)
				local nTongPoint = GetMissionV(M_TICHLUYA)
				local nKimPoint  = GetMissionV(M_TICHLUYB)
				local nTruThu = CTCGetTruChiemGiu(1)
				local nTruCong = CTCGetTruChiemGiu(2)
				--UpdatePlayerCountMSCTC()
				for i=1,GetMSPlayerCount(MS_CTHANHCHIEN) do
					PlayerIndex = MSDIdx2PIdx(MS_CTHANHCHIEN, i)
					-- UpdateBattleBox(MS_CTHANHCHIEN,Minute,1)
					Msg2Player(format("[Thñ %d-%d-%d] : [C«ng %d-%d-%d] %d'",nTongAcc,nTongPoint,nTruThu,nKimAcc,nKimPoint,nTruCong,Minute)) --th«ng b¸o qu©n sè 2 bªn sè phót cßn l¹i
				end
			end
		end
	end
end

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

function sukien_vuotai(nHr,nMi)
	for i=1,getn(TAB_TIME_VUOT_AI) do
		if(nHr == TAB_TIME_VUOT_AI[i][1] and nMi == TAB_TIME_VUOT_AI[i][2]) then
			AddGlobalCountNews("NhiÖm vô 'Th¸ch thøc thêi gian' b¾t ®Çu b¸o danh. C¸c ®éi tr­ëng h·y ®Õn gÆp NhiÕp ThÝ TrÇn ë thÊt ®¹i thµnh thÞ ®Ó b¸o danh tham gia!",3)
			Msg2SubWorld("<color=green>[Sù KiÖn]<color=yellow>'Th¸ch thøc thêi gian' b¾t ®Çu b¸o danh. C¸c ®éi tr­ëng h·y ®Õn gÆp NhiÕp ThÝ TrÇn ë thÊt ®¹i thµnh thÞ ®Ó b¸o danh tham gia!")
		    
			local nSubWorldId
			for i=1,getn(MAP_VUOTAI) do
				nSubWorldId = SubWorldID2Idx(MAP_VUOTAI[i])
				if (nSubWorldId >= 0) then
					SubWorld = nSubWorldId
					if(IsMission(MS_VUOTAI) == 0) then
						OpenMission(MS_VUOTAI)
					end
					StartMissionTimer(MS_VUOTAI,1, MSTIME_VUOT_AI_BD*60*18)--nMissionId, nTimerId, nTimeInterval so phut de chinh thuc bat dau 10*60*18
					print(format("===> Bao danh vuot ai %d:%d Map[%d]<===", nHr, nMi, MAP_VUOTAI[i]))
				else
					print(format("===> Vuot ai %d:%d chua mo Map[%d] <===", nHr, nMi, MAP_VUOTAI[i]))
				end
			end
		end
		if(nHr == TAB_TIME_VUOT_AI[i][1] and nMi == MSTIME_VUOT_AI_BD) then
			AddGlobalCountNews("Th¸ch thøc thêi gian ®· chÝnh thøc b¾t ®Çu! C¸c ®éi nhãm ®ang ra søc hoµn thµnh nhiÖm vô.", 3)--thong bao bat dau
			Msg2SubWorld("<color=green>[Sù KiÖn]<color=yellow>Th¸ch thøc thêi gian ®· chÝnh thøc b¾t ®Çu! C¸c ®éi nhãm ®ang ra søc hoµn thµnh nhiÖm vô.")
			print(format("===> Vuot ai bat dau chien dau %d:%d <===", nHr , nMi))
		end		
	end
end

function sukien_trangnguyen(nHr,nMi)
	for i=1,getn(TIME_CAU_HOI_TN) do
		if(nHr == TIME_CAU_HOI_TN[i][1] and nMi == TIME_CAU_HOI_TN[i][2]) then
			addnpccauhoi()
		end
	end	
end

function sukien_phonglangdo(nHr,nMi)
	local nSubWorldId
	nSubWorldId = SubWorldID2Idx(337)
	for i=1,getn(TAB_TIME_PLD) do
		if(nHr == TAB_TIME_PLD[i][1] and nMi == TAB_TIME_PLD[i][2]) then--Check thoi gian
			Msg2SubWorld("<color=green>[Sù KiÖn]<color=yellow> §ua Phong L¨ng §é sÏ b¾t ®Çu trong vµi phót n÷a, mäi ng­êi h·y nhanh chanh ®Õn Xa Phu - Thµnh ThÞ ®Ó di chuyÓn")
			if (nSubWorldId >= 0) then
				SubWorld = nSubWorldId
				if(IsMission(MS_PLANGDO) ==0) then
					OpenMission(MS_PLANGDO)
				end
				StartMissionTimer(MS_PLANGDO,4, TIME_PLD_BD)--so phut de chinh thuc bat dau
				AddGlobalCountNews(10137,3)
				Msg2SubWorld("<color=green>[Sù KiÖn]<color=yellow> §ua Phong L¨ng §é sÏ b¾t ®Çu trong vµi phót n÷a, mäi ng­êi h·y nhanh chanh ®Õn Xa Phu - Thµnh ThÞ ®Ó di chuyÓn")
				print(format("===> Phong Lang Do Bao Danh %d:%d <===",nHr,nMi))
			else
				print(format("===> Phong Lang Do Bao Danh Close Map not Open %d:%d <===",nHr,nMi))
			end
		end
		for j=1,getn(TAB_TIME_TTDL_PLD) do
			if(nHr == TAB_TIME_PLD[i][1] and nMi == TAB_TIME_TTDL_PLD[j]) then	
				nSubWorldId = SubWorldID2Idx(337)
				if (nSubWorldId >= 0) then
					SubWorld = nSubWorldId
					if(IsMission(MS_PLANGDO) == 0) then
						print(format("===> Phong Lang Do BOSS Close Mission not Open %d:%d <===",nHr,nMi))
					else
						if(nMi == TAB_TIME_TTDL_PLD[3]) then --boss ®¹i thuû tÆc ra tun cuèi thø 3
							addthuytacdaulinhpld(3)
							print(format("===> Phong Lang Do BOSS Dai Thuy Tac Dau Linh %d:%d <===",nHr,nMi))
						else
							addthuytacdaulinhpld(2)
							print(format("===> Phong Lang Do BOSS Thuy Tac Dau Linh %d:%d <===",nHr,nMi))
						end
					end
				end
			end
		end
	end
end

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
