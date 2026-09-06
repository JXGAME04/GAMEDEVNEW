--hoangnhk
Include("\\script\\header\\taskid.lua");
Include("\\script\\global\\npcchucnang\\nangcapngua.lua")
function main(NpcIndex)
dofile("script/global/npcchucnang/banngua.lua")
	SetTaskTemp(NPCINDEX, NpcIndex)
	local OK_TRADE = "Giao dÞch/giaodich";
	local OK_NCNGUACD = "N©ng cÊp ngùa th­êng lªn ChiÕu D¹/nangcap1";
	local OK_NCNGUAPV = "N©ng cÊp ChiÕu D¹ lªn Phi V©n/nangcap2";
	local OK_NCNGUABT = "N©ng cÊp Phi V©n lªn B«n Tiªu/nangcap3";
	local OK_NCNGUAPVU = "N©ng cÊp B«n Tiªu lªn Phiªn Vò/nangcap4";
	local OK_NCNGUASIEUQUANG = "N©ng cÊp Phiªn Vò lªn Siªu Quang/nangcap5";
	local NOT_TRADE = "Kh«ng giao dÞch/no";
	local w,x,y = GetWorldPos();
	if (w == 1) then--phuong tuong
	Say(12234,7,
		OK_TRADE,
		OK_NCNGUACD,
		OK_NCNGUAPV,
		OK_NCNGUABT,
		OK_NCNGUAPVU,
		OK_NCNGUASIEUQUANG,
		NOT_TRADE)
	return end
	if (w == 11) then--thanh do
	Say(12584,7,
		OK_TRADE,
		OK_NCNGUACD,
		OK_NCNGUAPV,
		OK_NCNGUABT,
		OK_NCNGUAPVU,
		OK_NCNGUASIEUQUANG,
		NOT_TRADE)
	return end
	if (w == 162) then--dai ly
	Say(12234,7,
		OK_TRADE,
		OK_NCNGUACD,
		OK_NCNGUAPV,
		OK_NCNGUABT,
		OK_NCNGUAPVU,
		OK_NCNGUASIEUQUANG,
		NOT_TRADE)
	return end
	if (w == 78) then--tuong duong
	Say(15266,7,
		OK_TRADE,
		OK_NCNGUACD,
		OK_NCNGUAPV,
		OK_NCNGUABT,
		OK_NCNGUAPVU,
		OK_NCNGUASIEUQUANG,
		NOT_TRADE)
	return end
	if (w == 37) then--bien kinh
	Say(14788,7,
		OK_TRADE,
		OK_NCNGUACD,
		OK_NCNGUAPV,
		OK_NCNGUABT,
		OK_NCNGUAPVU,
		OK_NCNGUASIEUQUANG,
		NOT_TRADE)
	return end
	if (w == 80) then--duong chau
	Say(15452,7,
		OK_TRADE,
		OK_NCNGUACD,
		OK_NCNGUAPV,
		OK_NCNGUABT,
		OK_NCNGUAPVU,
		OK_NCNGUASIEUQUANG,
		NOT_TRADE)
	return end
	if (w == 176) then--lam an
	Say(10830,7,
		OK_TRADE,
		OK_NCNGUACD,
		OK_NCNGUAPV,
		OK_NCNGUABT,
		OK_NCNGUAPVU,
		OK_NCNGUASIEUQUANG,
		NOT_TRADE)
	return end
	Say(15452,7,
		OK_TRADE,
		OK_NCNGUACD,
		OK_NCNGUAPV,
		OK_NCNGUABT,
		OK_NCNGUAPVU,
		OK_NCNGUASIEUQUANG,
		NOT_TRADE)
end;

function giaodich()
	local nParam = GetNpcValue(GetTaskTemp(NPCINDEX))
	if(nParam ~= nil) then
	Sale(nParam)
	end
end;

function no()
end;
