-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local WhichWarBegin
-- station_ctc.lua - DOT E (E5): port 3 ham vao tran cong thanh tu station.lua
-- JX2 goc (D:\ServerLinux) - THAN HAM TRICH BYTE-EXACT, giu nguyen TCVN3.
-- station.lua JX1 cua ta GIU NGUYEN vai tro cu; file nay duoc no Include them.
Include("\\script\\missions\\citywar_global\\head.lua");
Include("\\script\\missions\\citywar_city\\head.lua");
-- TV_VALUE: goc KHONG dinh nghia o dau ca (bug goc) -> GetTask(nil)=GetTask(0)
-- luon lech gia tri kiem => nhanh 'than phan' luon TU CHOI nhu goc. Dinh nghia = 0
-- de khoi doc bien nil ma HANH VI van y het (KEHOACH muc 3: CAM doi thanh TV_TASKVALUE).
TV_VALUE = 0

function WhichWarBegin()
	for i = 1,7 do
		if (HaveBeginWar(i) ~= 0) then
			return i;
		end;
	end;
	return 0;
end;

function CancelGoCityWar()
	Say("ChiÕn tr­êng §ao KiÕm v« t×nh!  Xin nghÜa sÜ h·y quay vÒ ®Ó b¶o toµn tÝnh m¹ng", 0);
end;

function GoCityWar()
	CityID = WhichWarBegin();
	if (CityID >= 1 and CityID <= 7) then
		Tong1, Tong2 = GetCityWarBothSides(CityID);
		str = format("HiÖn t¹i %s ®ang tiÕn hµnh c«ng thµnh chiÕn, bªn c«ng<%s>, bªn thñ<%s>, xin hái ng­¬i muèn tham chiÕn bªn nµo?", GetCityAreaName(CityID), Tong1, Tong2);
		Say(str , 3, "Bªn c«ng/GoCityWarAttack", "Bªn thñ /GoCityWarDefend", "Kh«ng bªn nµo hÕt/CancelGoCityWar");
	end;
end;

--È¥¹¥³ÇÕ½ÊØ·½Èë¿Ú
function GoCityWarDefend()
	CityID = WhichWarBegin();
	if (CityID == 0 ) then 
		return
	end;

	TongName, result = GetTong()
	Tong1, Tong2 = GetCityWarBothSides(CityID);
	if (Tong2 ~= TongName and GetItemCountEx(CardTab[CityID * 2]) < 1) then 
		if (GetTask(TV_CITYID) ~= CityID or GetTask(TV_VALUE) ~= 1 or GetTask(TV_TASKID) ~= MISSIONID) then
			Say("Th©n phËn nghÜa sÜ ch­a phï hîp!  T¹i h¹ kh«ng d¸m m¹o muéi ®­a vµo!  Xin nghÜa sÜ quay vÒ! ", 0);
			return
		end;
	end;
	
	if (random(0,1) == 1) then
		NewWorld(222, 1614, 3172);
	else
		NewWorld(222, 1629, 3193);
	end;
end;

--È¥¹¥³ÇÕ½¹¥·½Èë¿Ú
function GoCityWarAttack()
	CityID = WhichWarBegin();
	if (CityID == 0) then 
		return
	end;

	TongName, result = GetTong()
	Tong1, Tong2 = GetCityWarBothSides(CityID);
	if (Tong1 ~= TongName and GetItemCountEx(CardTab[CityID * 2 - 1]) < 1) then
		if (GetTask(TV_CITYID) ~= CityID or GetTask(TV_VALUE) ~= 2 or GetTask(TV_TASKID) ~= MISSIONID) then
			Say("Th©n phËn nghÜa sÜ ch­a phï hîp!  T¹i h¹ kh«ng d¸m m¹o muéi ®­a vµo!  Xin nghÜa sÜ quay vÒ! ", 0);
			return
		end
	end
	
	if (random(0,1) == 1) then
		NewWorld(223, 1614, 3172);
	else
		NewWorld(223, 1629, 3193);
	end;

end;

