-- citywar_e3.lua - GM smoke test cho E3 KJx2CityWar (gan vao NPC test hoac goi tay)
-- ASCII thuan (khong dau) de khoi vuong encoding. Xoa sau khi E8 xong.

function main()
	Say("Test E3 cong thanh (KJx2CityWar)", 7,
		"Xem 7 thanh/E3ShowAll",
		"Xem thanh dang dung (GetCityArea)/E3ShowHere",
		"AppointViceroy: bang TA lay thanh dang dung/E3Appoint",
		"AppointChallenger: bang TA khieu chien thanh dang dung/E3Challenge",
		"Ket tran: CONG thang/E3WinAttack",
		"Ket tran: THU thang/E3WinDefend",
		"Thoi/OnCancel");
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
