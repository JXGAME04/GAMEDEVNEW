-- citywar_e3.lua - GM smoke test cho E3+E6 cong thanh (gan vao NPC test)
-- ASCII thuan (khong dau) de khoi vuong encoding. Xoa sau khi E8 xong.
-- Ep pha E6: ham tick nap vao CHINH state nay (kho league/city la C++ toan cuc
-- nen ket qua y het khi timerserver goi that).
Include("\\script\\timerserver_ctc.lua")

function main()
	Say("Test E2-E7 cong thanh", 14,
		"Xem 7 thanh/E3ShowAll",
		"Xem thanh dang dung (GetCityArea)/E3ShowHere",
		"AppointViceroy: bang TA lay thanh dang dung/E3Appoint",
		"AppointChallenger: bang TA khieu chien thanh dang dung/E3Challenge",
		"Ket tran: CONG thang/E3WinAttack",
		"Ket tran: THU thang/E3WinDefend",
		"EP PHA 18h (bao danh) thanh dang dung/E6Force18",
		"EP PHA 19h (boc tham)/E6Force19",
		"EP PHA 20h (khai chien)/E6Force20",
		"EP PHA 0h (don ngay)/E6Force0",
		"PHAT 10 Khieu chien lenh (6-1-1508)/E5GiveLenh",
		"Xem diem lenh bang TA o 7 thanh (league 508)/E5ShowPoint",
		"Test kho C++ E2 (GlbValue-OB-Ladder-League)/E2Store",
		"Thoi/OnCancel");
end

function E5GiveLenh()
	for i = 1, 10 do
		AddItem(6, 1, 1508, 1, 1, 1);
	end
	Say("Da phat 10 Khieu chien lenh (6,1,1508). Mang den NPC Su Gia Cong Thanh (Ba Lang Huyen) de nop.", 1, "Tro ve/main");
end

function E5ShowPoint()
	local szTong = GetTongName();
	if (szTong == nil or szTong == "") then
		Say("Phai co bang truoc", 0);
		return
	end
	local s = "Diem lenh cua bang ["..szTong.."]:\n";
	for i = 1, 7 do
		s = s..i.." <"..GetCityAreaName(i).."> = "..LG_GetMemberTask(508, GetCityAreaName(i), szTong, 1).."\n";
	end
	s = s.."Kho lenh 538 = "..LG_GetMemberTask(538, "tiaozhanling", szTong, 1);
	Say(s, 1, "Tro ve/main");
end

-- round-trip kho C++ E2: 2 store nay la TOAN CUC giua moi Lua state
function E2Store()
	SetGlbValue(1999, 12345);
	local a = GetGlbValue(1999);
	local h = OB_Create();
	OB_PushInt(h, 777);
	local b = OB_PopInt(h);
	OB_Release(h);
	Ladder_NewLadder(10999, GetName(), 111, 1);
	local n1, v1 = Ladder_GetLadderInfo(10999, 1);
	local lid = LG_GetLeagueObj(508, GetCityAreaName(1));
	Say("GlbValue(1999)="..a.." (cho 12345)\nOB pop="..b.." (cho 777)\nLadder10999 top1="..n1.."/"..v1.."\nLeague508 <"..GetCityAreaName(1).."> lid="..lid.." (>0 = boot da tao)", 1, "Tro ve/main");
end

-- ep pha theo LICH cua thanh dang dung: truyen dung thu bao danh/danh cua no
function E6Force18()
	local n = GetCityArea();
	if (n < 1 or n > 7) then
		Say("Dung trong thanh muon test", 0);
		return
	end
	g_CTC6_D18 = nil;
	CTC_JX2_Tick(TB_CTC6[n][1], 18, 0);
	E3ShowAll();
end

function E6Force19()
	local n = GetCityArea();
	if (n < 1 or n > 7) then
		Say("Dung trong thanh muon test", 0);
		return
	end
	g_CTC6_D19 = nil;
	CTC_JX2_Tick(TB_CTC6[n][1], 19, 0);
	E3ShowAll();
end

function E6Force20()
	local n = GetCityArea();
	if (n < 1 or n > 7) then
		Say("Dung trong thanh muon test", 0);
		return
	end
	g_CTC6_D20 = nil;
	CTC_JX2_Tick(TB_CTC6[n][2], 20, 0);
	E3ShowAll();
end

function E6Force0()
	g_CTC6_D0 = nil;
	CTC_JX2_Tick(0, 0, 0);
	E3ShowAll();
end

function E3ShowAll()
	local s = "";
	for i = 1, 7 do
		local o, m = GetCityOwner(i);
		s = s..i.." <"..GetCityAreaName(i).."> chu=["..o.."] thaithu=["..m.."] war="..HaveBeginWar(i).." signup="..IsSigningUp(i).."\n";
	end
	Say(s, 1, "Tro ve/main");
end

function E3ShowHere()
	local n = GetCityArea();
	if (n >= 1 and n <= 7) then
		Say("Dang dung o thanh so "..n..": "..GetCitySummary(n), 1, "Tro ve/main");
	else
		Say("Map nay khong phai thanh (GetCityArea="..n..")", 1, "Tro ve/main");
	end
end

function E3Appoint()
	local szTong = GetTongName();
	local n = GetCityArea();
	if (szTong == nil or szTong == "") then
		Say("Phai co bang truoc", 0);
		return
	end
	if (n < 1 or n > 7) then
		Say("Phai dung trong 1 trong 7 thanh", 0);
		return
	end
	AppointViceroy(GetCityAreaName(n), szTong);
	E3ShowAll();
end

function E3Challenge()
	local szTong = GetTongName();
	local n = GetCityArea();
	if (szTong == nil or szTong == "" or n < 1 or n > 7) then
		Say("Can bang + dung trong thanh", 0);
		return
	end
	AppointChallenger(GetCityAreaName(n), szTong);
	E3ShowAll();
end

function E3WinAttack()
	local n = GetCityArea();
	if (n >= 1 and n <= 7) then
		NotifyWarResult(n, 1);
	end
	E3ShowAll();
end

function E3WinDefend()
	local n = GetCityArea();
	if (n >= 1 and n <= 7) then
		NotifyWarResult(n, 0);
	end
	E3ShowAll();
end
