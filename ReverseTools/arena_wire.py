# -*- coding: utf-8 -*-
"""LOI DAI CN (21/08) - noi day script trong cay chay that. latin-1, assert tung cho."""
import io, os, re, sys, shutil
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
SC = os.path.join(SRV, "script")
BK = os.path.join(SRV, "_backup_loidai_2108")
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hi(s): return sum(1 for c in s if ord(c) > 127)
def nlof(s): return "\r\n" if "\r\n" in s else "\n"
def rep1(s, old, new, nl):
    old = old.replace("\r\n", "\n").replace("\n", nl); new = new.replace("\r\n", "\n").replace("\n", nl)
    assert s.count(old) == 1, (s.count(old), old[:80])
    return s.replace(old, new, 1)
def backup(rel):
    d = os.path.join(BK, rel); os.makedirs(os.path.dirname(d), exist_ok=True)
    if not os.path.exists(d): shutil.copyfile(os.path.join(SC, rel), d)

# ---------- 1. timerserver_ctc.lua: lich CN 4 pha ----------
rel = "timerserver_ctc.lua"; backup(rel); p = os.path.join(SC, rel); s = rd(p); nl = nlof(s); h0 = hi(s)
s = rep1(s, """function ctc6_startsignup(i)
	Ladder_ClearLadder(CTC6_LADDER)
	LG_ApplySetLeagueTask(508, ctc6_lgname(i), 1, 1)
""", """function ctc6_startsignup(i)
	Ladder_ClearLadder(CTC6_LADDER)
	LG_ApplySetLeagueTask(508, ctc6_lgname(i), 1, 1)	-- giu: NPC Su Gia (getSignUpState) mo menu 18-19h
	-- [LOI DAI CN 21/08] relay StartSignUp: xoa du lieu thanh cu, state 1, tin SIGNUP
	if (StartSignUp ~= nil) then
		StartSignUp(i)
	end
""", nl)
s = rep1(s, """	if (bToday ~= 1) then
		LG_ApplySetLeagueTask(508, szCity, 1, 0)
		return 0
	end
	local nlid = LG_GetLeagueObj(509, szCity)
""", """	if (bToday ~= 1) then
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
""", nl)
s = rep1(s, """function ctc6_startwar(i)
	local szCong, szThu = GetCityWarBothSides(i)
	if (szCong ~= nil and szCong ~= "" and szThu ~= nil and szThu ~= "") then
		CTC_JX2_SetCityState(i, 2)
		WriteLog("[citywar] 20h khai chien thanh "..i)
	end
end
""", """function ctc6_startwar(i)
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
-- (citywar_global\\timer.lua) poll IsArenaBegin 5' mo mission 9 tren map 213+
function ctc6_startarena(i)
	if (StartArena ~= nil) then
		StartArena(i)
		WriteLog("[citywar] 20h StartArena thanh "..i)
	end
end
""", nl)
s = rep1(s, """function ctc6_daily0h()
	local i = 1
	while (i <= 7) do
		local szCong = GetCityWarBothSides(i)
		if ((szCong ~= nil and szCong ~= "") or HaveBeginWar(i) == 1) then
			NotifyWarResult(i, 0)
		end
		i = i + 1
	end
""", """function ctc6_daily0h()
	local i = 1
	while (i <= 7) do
		-- [LOI DAI CN 21/08] CHI don tran cong thanh con treo (state 5). KHONG dong
		-- challenger: khieu chien gia chot 19h hom truoc, 20h HOM NAY moi danh.
		if (HaveBeginWar(i) == 1) then
			NotifyWarResult(i, 0)
		end
		i = i + 1
	end
""", nl)
s = rep1(s, """	if (nHr == 20 and g_CTC6_D20 ~= nToday) then
		g_CTC6_D20 = nToday
		local i = 1
		while (i <= 7) do
			if (TB_CTC6[i][2] == nDyfW) then
				ctc6_startwar(i)
			end
			i = i + 1
		end
	end
""", """	if (nHr == 20 and g_CTC6_D20 ~= nToday) then
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
""", nl)
assert hi(s) == h0
wr(p, s); print("timerserver_ctc.lua ok")

# ---------- 2. timerserver.lua: CUTOVER (Include + goi CTC_JX2_Tick) ----------
rel = "timerserver.lua"; backup(rel); p = os.path.join(SC, rel); s = rd(p); nl = nlof(s); h0 = hi(s)
assert "timerserver_ctc.lua" not in s
s = rep1(s, 'Include("\\\\script\\\\tinhnang\\\\pubg\\\\pubgutils.lua")\n',
            'Include("\\\\script\\\\tinhnang\\\\pubg\\\\pubgutils.lua")\n\nInclude("\\\\script\\\\timerserver_ctc.lua")\t-- DOT E + LOI DAI CN (21/08): dong ho 18h/19h/20h cong thanh + loi dai bang hoi\n', nl)
s = rep1(s, """	-- sukien_liendau(nHr,nDy,nMi,nMo)
	sukien_tongkim(nHr,nMi)
""", """	-- sukien_liendau(nHr,nDy,nMi,nMo)
	sukien_tongkim(nHr,nMi)
	-- [21/08] cong thanh JX2 + loi dai bang hoi CN: 0h/18h/19h/20h (guard ngay trong ham)
	if (CTC_JX2_Tick ~= nil) then
		CTC_JX2_Tick(nDyfW, nHr, nMi)
	end
""", nl)
assert hi(s) == h0
wr(p, s); print("timerserver.lua ok (CUTOVER)")

# ---------- 3. infocenter_head.lua: ArenaMain them 2 muc ----------
rel = r"missions\citywar_global\infocenter_head.lua"; backup(rel); p = os.path.join(SC, rel); s = rd(p); nl = nlof(s); h0 = hi(s)
opt_signup = vn("Báo danh đấu thầu Lôi đài bang hội") + "/SignUpTheOne"
opt_enter = vn("Tham gia Lôi đài bang hội") + "/PreEnterGame"
old_a = '		Say(format("' + vn("Hiện tại công thành chiến thành <%s> đang cho báo danh, ngươi muốn đăng ký không?") + '",GetCityAreaName(nCityId)), 7, "' + vn("Báo danh công thành chiến") + '/SignUpCityWar", '
assert s.count(old_a) == 1, s.count(old_a)
new_a = '		Say(format("' + vn("Hiện tại công thành chiến thành <%s> đang cho báo danh, ngươi muốn đăng ký không?") + '",GetCityAreaName(nCityId)), 8, "' + opt_signup + '", "' + vn("Báo danh công thành chiến") + '/SignUpCityWar", '
s = s.replace(old_a, new_a, 1)
old_b = """		Say("%s",
			7,
			"%s/GiveTiaoZhanLing",""" % (vn("Đây là nơi nghị sự công thành chiến, ngươi đến có việc gì?"), vn("Ta đến giao lệnh bài"))
assert s.count(old_b.replace("\n", nl)) == 1, s.count(old_b.replace("\n", nl))
new_b = """		Say("%s",
			8,
			"%s",
			"%s/GiveTiaoZhanLing",""" % (vn("Đây là nơi nghị sự công thành chiến, ngươi đến có việc gì?"), opt_enter, vn("Ta đến giao lệnh bài"))
s = s.replace(old_b.replace("\n", nl), new_b.replace("\n", nl), 1)
# ghi chu dau ArenaMain
s = rep1(s, "function ArenaMain()\n", "-- [LOI DAI CN 21/08] them 'Bao danh dau thau Loi dai bang hoi' (18-19h, SignUpTheOne -> SignUpCityWarArena)\n-- va 'Tham gia Loi dai bang hoi' (PreEnterGame: IsArenaBegin -> EnterBattle map 213+)\nfunction ArenaMain()\n", nl)
assert hi(s) == h0 + hi(opt_signup) + hi(opt_enter)
wr(p, s); print("infocenter_head.lua ok")

# ---------- 4. camper.lua: GetJoinTongTime >= 7200 -> cap 90 ----------
rel = r"missions\citywar_arena\camper.lua"; backup(rel); p = os.path.join(SC, rel); s = rd(p); nl = nlof(s); h0 = hi(s)
old_c = "\t\tif (GetJoinTongTime() >= 7200) then\n"
assert s.count(old_c.replace("\n", nl)) == 2
s = s.replace(old_c.replace("\n", nl), "\t\tif (GetLevel() >= 90) then\t-- [LOI DAI CN 21/08] chinh sach chu game: cap 90 (goc: vao bang >= 7200 phut)".replace("\n", nl) + nl, 2)
old_m = '\t\t\tSay("' + vn("Thời gian bạn gia nhập bang hội quá ngắn, không thể tham gia chiến đấu!") + '", 0);'
assert s.count(old_m) == 2, s.count(old_m)
new_m = '\t\t\tSay("' + vn("Đẳng cấp chưa đủ 90, không thể tham gia chiến đấu!") + '", 0);'
s = s.replace(old_m, new_m, 2)
wr(p, s); print("camper.lua ok (byte cao %d -> %d)" % (h0, hi(s)))

# ---------- 5. citywar_boot.lua: NPC camper1/camper2 tren map 213..220 ----------
rel = r"startgame\citywar_boot.lua"; backup(rel); p = os.path.join(SC, rel); s = rd(p); nl = nlof(s); h0 = hi(s)
assert "CityWar_ArenaNpc" not in s
s = rep1(s, """	WriteLog("[citywar] boot: league 4/508/509 + GlbMission 8 OK, NPC infocenter="..nIC)
end
""", """	WriteLog("[citywar] boot: league 4/508/509 + GlbMission 8 OK, NPC infocenter="..nIC)
	CityWar_ArenaNpc()
end

-- [LOI DAI CN 21/08] NPC 2 phe tren 8 map loi dai 213..220 - goc nam trong npc-data
-- map (doc region maps.pak: tpl 178 o (1581,3257) camper1.lua, tpl 124 o (1603,3236)
-- camper2.lua) nhung engine ta bo NPC thoai trong map-data (NotAddNpcNormal=1) -> tu spawn.
function CityWar_ArenaNpc()
	local n = 0
	for m = 213, 220 do
		if (SubWorldID2Idx(m) >= 0) then
			AddNpcEx1({178},1,nil,m,1581*32,3257*32,"","\\\\script\\\\missions\\\\citywar_arena\\\\camper1.lua",nil,6)
			AddNpcEx1({124},1,nil,m,1603*32,3236*32,"","\\\\script\\\\missions\\\\citywar_arena\\\\camper2.lua",nil,6)
			n = n + 1
		end
	end
	WriteLog("[citywar] boot: NPC loi dai tren "..n.." map")
end
""", nl)
assert hi(s) == h0
wr(p, s); print("citywar_boot.lua ok")
print("OK")
