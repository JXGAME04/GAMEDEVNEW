-- ================================================================================================
-- [HE THONG] script/timerserver_ctc.lua
-- Muc dich  : Dong ho 5 pha CONG THANH CHIEN tren GameServer (dot E6); goi tu timerserver.lua.
-- Duoc nap  : Include tu 3 tep (vd bangthanh_f.lua, citywar_e3.lua, timerserver.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Ham (dong): ctc6_lgname (12), ctc6_checkleague (18), ctc6_clearleague (36), ctc6_randomchallenger (59), ctc6_startsignup (96), ctc6_endsignup (109), ctc6_startwar (147), ctc6_startarena (164), ctc6_daily0h (172), CTC_JX2_Tick (186)
-- Sua nong  : CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- timerserver_ctc.lua - DOT E (E6): dong ho 5 pha cong thanh tren GS
-- (port nguyen ngu nghia cw_*_fun relay citywar_head.lua:99-145 +
-- citywar_clearchallenger.lua - ta 1 GS khong co relay-Lua).
-- timerserver.lua dofile lai file nay moi tick ~1 phut: g_CTC6_* CHI gan
-- trong ham; cua so PHA = dung GIO do (60 co hoi), lo ca gio = bo pha do
-- (nhu relay sap khi den gio - khong ban lai pha cu sau restart).
-- GM ep pha: CTC_JX2_Tick(nDyfW, nHr, nMi) tham so tuy chon.

-- lich TB_CITYWAR_ARRANGE (relay :22-30): {thu bao danh, thu danh} theo %w (0=CN)
TB_CTC6 = { {3,4}, {1,2}, {2,3}, {5,6}, {4,5}, {0,1}, {6,0} }
CTC6_LADDER = 10261

function ctc6_lgname(i)
	return GetCityAreaName(i)
end

-- port relay citywar_clearchallenger.lua:20-40: tao league 508/509 neu thieu
function ctc6_checkleague()
	for i = 1, 7 do
		local city = ctc6_lgname(i)
		local t = 508
		while (t <= 509) do
			local lid = LG_GetLeagueObj(t, city)
			if (lid == nil or lid == 0) then
				local h = LG_CreateLeagueObj()
				LG_SetLeagueInfo(h, t, city)
				LG_ApplyAddLeague(h, "", "")
				LG_FreeLeagueObj(h)
			end
			t = t + 1
		end
	end
end

-- port relay citywar_clearchallenger.lua:42-68: 0h go SACH member 509 roi 508
function ctc6_clearleague()
	for i = 1, 7 do
		local city = ctc6_lgname(i)
		local t = 509
		while (t >= 508) do
			local lid = LG_GetLeagueObj(t, city)
			if (lid ~= nil and lid ~= 0) then
				local n = LG_GetMemberCount(lid)
				local j = n - 1
				while (j >= 0) do
					local nm = LG_GetMemberInfo(lid, j)
					if (nm ~= nil and nm ~= "") then
						LGM_ApplyRemoveMember(t, city, nm, "", "", 0)
					end
					j = j - 1
				end
			end
			t = t - 1
		end
	end
end

-- port relay GetRandomChallenger :64-97 (max diem lenh, dong diem -> random)
function ctc6_randomchallenger(szCityName)
	local nlid = LG_GetLeagueObj(508, szCityName)
	local nmem = LG_GetMemberCount(nlid)
	if (nmem == 0) then
		return nil
	end
	if (nmem == 1) then
		local szM = LG_GetMemberInfo(nlid, 0)
		if (szM ~= nil and szM ~= "") then
			return szM
		end
		return nil
	end
	local tbMem = {}
	local nBest = -1
	local i = 0
	while (i < nmem) do
		local szM = LG_GetMemberInfo(nlid, i)
		if (szM ~= nil and szM ~= "") then
			local nc = LG_GetMemberTask(508, szCityName, szM, 1)
			if (nc > nBest) then
				tbMem = {}
				tbMem[1] = szM
				nBest = nc
			elseif (nc == nBest) then
				tbMem[getn(tbMem) + 1] = szM
			end
		end
		i = i + 1
	end
	if (getn(tbMem) == 0) then
		return nil
	end
	return tbMem[random(getn(tbMem))]
end

-- 18h (relay cw_startsignup_fun :99-108)
function ctc6_startsignup(i)
	Ladder_ClearLadder(CTC6_LADDER)
	LG_ApplySetLeagueTask(508, ctc6_lgname(i), 1, 1)	-- giu: NPC Su Gia (getSignUpState) mo menu 18-19h
	-- [LOI DAI CN 21/08] relay StartSignUp: xoa du lieu thanh cu, state 1, tin SIGNUP
	if (StartSignUp ~= nil) then
		StartSignUp(i)
	end
	AddLocalNews(format("B¸o danh c«ng thµnh chiÕn %s ®· b¾t ®Çu, c¸c bang héi ch­a chiÕm thµnh cÊp 18 trë lªn cã thÓ ®Õn c«ng thµnh quan t¹i c¸c T©n Thñ Th«n ®Ó b¸o danh tham gia.", ctc6_lgname(i)))
	WriteLog("[citywar] 18h mo bao danh thanh "..i)
end

-- 19h (relay cw_endsignup_fun :115-145) - GIU nguyen control flow goc ke ca
-- 2 nhanh return som KHONG ha co (quirk goc, xem DIEUTRA_LEAGUE muc 7)
function ctc6_endsignup(i, bToday)
	local szCity = ctc6_lgname(i)
	-- relay chay ham nay MOI NGAY cho MOI thanh (7 hang task 19h); ngay khong
	-- khop -> if-block bo qua nhung :144 van ha co - giu y het
	if (bToday ~= 1) then
		LG_ApplySetLeagueTask(508, szCity, 1, 0)
		return 0
	end
	-- [LOI DAI CN 21/08] relay EndSignUp: 0 bang -> roi, 1 bang -> khieu chien gia,
	-- >= 2 bang -> cat <= 16 theo phi, boc tham bang dau (state 2, cho StartArena 20h).
	-- Duong lenh bai League 508/509 (VN) ben duoi KHONG con chay.
	if (EndSignUp ~= nil) then
		EndSignUp(i)
		LG_ApplySetLeagueTask(508, szCity, 1, 0)
		return 1
	end
	local nlid = LG_GetLeagueObj(509, szCity)
	if (nlid == nil or nlid == 0) then
		AddLocalNews(format("C«ng thµnh chiÕn thµnh %s kh«ng cã bang héi nµo b¸o danh, tuÇn nµy v¹n sù th¸i b×nh", szCity))
		return 0
	end
	local szFirst = ctc6_randomchallenger(szCity)
	if (szFirst == nil or szFirst == "") then
		AddLocalNews(format("C«ng thµnh chiÕn thµnh %s kh«ng cã bang héi nµo b¸o danh, tuÇn nµy v¹n sù th¸i b×nh", szCity))
		return 0
	end
	local szOwner = GetCityOwner(i)
	if (szOwner == "" or szOwner == nil) then
		AppointViceroy(szCity, szFirst)
	else
		AppointChallenger(szCity, szFirst)
		AddLocalNews(format("Bang héi %s tranh ®ua lÖnh bµi thµnh c«ng, trë thµnh bang héi khiªu chiÕn thµnh %s", szFirst, szCity))
	end
	LG_ApplySetLeagueTask(508, szCity, 1, 0)
end

-- 20h (relay cw_start_fun -> StartCityWar): co khieu chien gia -> vao trang thai
-- DANG DANH; timer 18 (citywar_global\\timer.lua) poll 5' se OpenMission(7)
function ctc6_startwar(i)
	-- [LOI DAI CN 21/08] relay StartCityWar: doi challenger; thanh vo chu -> chiem luon
	-- (WAR_RESULT3); co chu -> state 5 (HaveBeginWar) de timer 18 mo mission 7
	if (StartCityWar ~= nil) then
		StartCityWar(i)
		WriteLog("[citywar] 20h StartCityWar thanh "..i)
		return
	end
	local szCong, szThu = GetCityWarBothSides(i)
	if (szCong ~= nil and szCong ~= "" and szThu ~= nil and szThu ~= "") then
		CTC_JX2_SetCityState(i, 5)
		WriteLog("[citywar] 20h khai chien thanh "..i)
	end
end

-- [LOI DAI CN 21/08] 20h NGAY BAO DANH: relay StartArena - loi dai bat dau, timer 18
-- (citywar_global\timer.lua) poll IsArenaBegin 5' mo mission 9 tren map 213+
function ctc6_startarena(i)
	if (StartArena ~= nil) then
		StartArena(i)
		WriteLog("[citywar] 20h StartArena thanh "..i)
	end
end

-- 0h don sach (relay clearchallenger 0h) + trang thai tran con sot
function ctc6_daily0h()
	local i = 1
	while (i <= 7) do
		-- [LOI DAI CN 21/08] CHI don tran cong thanh con treo (state 5). KHONG dong
		-- challenger: khieu chien gia chot 19h hom truoc, 20h HOM NAY moi danh.
		if (HaveBeginWar(i) == 1) then
			NotifyWarResult(i, 0)
		end
		i = i + 1
	end
	ctc6_clearleague()
	ctc6_checkleague()
	WriteLog("[citywar] 0h don league 508/509 + trang thai")
end

function CTC_JX2_Tick(nDyfW, nHr, nMi)
	if (nDyfW == nil) then
		local nYr, nMo, nDy, nH, nM, nSe, nDw = GetTimeNow()
		nDyfW = nDw
		nHr = nH
		nMi = nM
	end
	local nToday = tonumber(GetLocalDate("%Y%m%d"))
	-- 0h00-05h59: don ngay (1 lan/ngay)
	if (nHr >= 0 and nHr < 6 and g_CTC6_D0 ~= nToday) then
		g_CTC6_D0 = nToday
		ctc6_daily0h()
	end
	-- 18h: mo bao danh thanh co lich hom nay (1 lan/ngay)
	if (nHr == 18 and g_CTC6_D18 ~= nToday) then
		g_CTC6_D18 = nToday
		local i = 1
		while (i <= 7) do
			if (TB_CTC6[i][1] == nDyfW) then
				ctc6_startsignup(i)
			end
			i = i + 1
		end
	end
	-- 19h: chot bao danh (moi thanh, nhu relay chay task moi ngay)
	if (nHr == 19 and g_CTC6_D19 ~= nToday) then
		g_CTC6_D19 = nToday
		local i = 1
		while (i <= 7) do
			if (TB_CTC6[i][1] == nDyfW) then
				ctc6_endsignup(i, 1)
			else
				ctc6_endsignup(i, 0)
			end
			i = i + 1
		end
	end
	-- 20h: khai chien thanh co lich danh hom nay
	if (nHr == 20 and g_CTC6_D20 ~= nToday) then
		g_CTC6_D20 = nToday
		local i = 1
		while (i <= 7) do
			if (TB_CTC6[i][1] == nDyfW) then
				ctc6_startarena(i)		-- [LOI DAI CN] ngay bao danh: 20h loi dai
			end
			if (TB_CTC6[i][2] == nDyfW) then
				ctc6_startwar(i)		-- ngay danh: 20h cong thanh
			end
			i = i + 1
		end
	end
end
