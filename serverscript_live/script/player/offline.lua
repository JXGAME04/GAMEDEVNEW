Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\log_game\\save_log.lua")
Include("\\Script\\lib\\lib_task.lua")

TAB_MAPUTNAME = { 
	{1, "Ph­îng T­êng"},
	{11, "Thµnh §«"},
	{37, "BiÖn Kinh"},
	{78, "T­¬ng D­¬ng"},
	{80, "D­¬ng Ch©u"},
	{162, "§¹i Lý"},	
	{176, "L©m an"},
	{53, "Ba Lang Huyen"},
}

function CheckMapNoFor_UT(nMap)
	for i = 1, getn(TAB_MAPUTNAME) do
		if (nMap == TAB_MAPUTNAME[i][1]) then
			return 1
		end
	end
	return 0
end


function main()
	dofile("script/player/offline.lua")
-- Talk(1,"","§ang trong thêi gian ®ua Top kh«ng thÓ tham gia ho¹t ®éng .!!!") 
	offline()
	XoaTimeAm()
end


function offline()
		local nMap,x,y = GetWorldPos()
		if(CheckMapNoFor_UT(nMap) == 0) then
		Talk(1,"","Kh«ng thÓ uû th¸c ë b¶n ®å nµy, di chuyÓn ®Õn th«n hoÆc thµnh thÞ ®«ng ng­êi qua l¹i ®Ó bµy b¸n vµ uû th¸c.!!!") 
		return
		end
		if GetTask(LoaiBCH) == 0 then
			Say("<color=red>HÖ Thèng<color>: sau khi chän sö dông, b¹n cã thÓ tho¸t nh©n vËt ®Ó ñy th¸c nhËn ®iÓm kinh nghiÖm.\n\n- Sau khi online b¹n cã thÓ tïy chän nhËn hoÆc kh«ng  nhËn ®iÓm kinh nghiÖm ®· ñy th¸c.\n- §¹i B¹ch CÇu Hoµn cña b¹n cßn <color=blue>"..giodbch().."<color> giê <color=blue>"..phutdbch().."<color> phót",3,
				"B¾t ®Çu ñy th¸c rêi m¹ng/startutdbch",
				"NhËn kinh nghiÖm tÝch lòy/nhanexputoffdbch",
				"Tho¸t/no")
				SetTask(TaskDBCH,GetTask(TaskDBCH)+480)
		else
			Talk(1,"","Ng­¬i ®· kÝch ho¹t ñy th¸c, h·y mau tho¸t nh©n vËt.")
		end
	
end

function SuDungBCH()
	Say("<color=red>HÖ Thèng<color>: sau khi chän sö dông, b¹n cã thÓ tho¸t nh©n vËt ®Ó ñy th¸c nhËn ®iÓm kinh nghiÖm.\n- §¹i B¹ch CÇu Hoµn cña b¹n cßn <color=blue>"..giobch().."<color> giê <color=blue>"..phutbch().."<color> phót",3,
		"B¾t ®Çu ñy th¸c/startutbch",
		"NhËn kinh nghiÖm tÝch lòy/nhanexputoffbch",
		"Tho¸t/no")
end

function SuDungDBCH()
	Say("<color=red>HÖ Thèng<color>: sau khi chän sö dông, b¹n cã thÓ tho¸t nh©n vËt ®Ó ñy th¸c nhËn ®iÓm kinh nghiÖm.\n- §¹i B¹ch CÇu Hoµn cña b¹n cßn <color=blue>"..giodbch().."<color> giê <color=blue>"..phutdbch().."<color> phót",3,
		"B¾t ®Çu ñy th¸c/startutdbch",
		"NhËn kinh nghiÖm tÝch lòy/nhanexputoffdbch",
		"Tho¸t/no")
end

function no()
end
-----------------------------------------------------------------
function XoaTimeAm()
	if GetTask(TaskUyThacOffline1) < 0 then
	SetTask(TaskUyThacOffline1,0)
	end
	
	if GetTask(TaskUyThacOffline4) < 0 then
	SetTask(TaskUyThacOffline4,0)
	end
end

function giobch()
if GetTask(TaskBCH) < 60 then
return 0
else
gio = floor((GetTask(TaskBCH)/60),2)
return gio
end
end

function phutbch()
if GetTask(TaskBCH) < 60 then
return GetTask(TaskBCH)
else
gio = floor((GetTask(TaskBCH)/60),2)
phut = GetTask(TaskBCH) - gio*60
return phut
end
end
-----------------------------------------------------------------
function giodbch()
if GetTask(TaskDBCH) < 60 then
return 0
else
gio = floor((GetTask(TaskDBCH)/60),2)
return gio
end
end

function phutdbch()
if GetTask(TaskDBCH) < 60 then
return GetTask(TaskDBCH)
else
gio = floor((GetTask(TaskDBCH)/60),2)
phut = GetTask(TaskDBCH) - gio*60
return phut
end
end
-----------------------------------------------------------------
function startutbch()
if GetLevel() >= 80 then
	-- if GetTask(TaskUyThacOffline1) >= 15 then
	-- nhanexputoffbch()
	-- Msg2Player("<color=yellow>B¹n nhËn thµnh c«ng kinh nghiÖm cña lÇn ñy th¸c tr­íc tõ B¹ch CÇu Hoµn.")
	-- else
		if GetTask(TaskBCH) >= 1 then
		ngay = tonumber(date("%d"))
		thang = tonumber(date("%m"))
		nam = tonumber(date("%y"))
		gio = tonumber(date("%H"))
		phut = tonumber(date("%M"))
		SetTask(TaskUyThacOffline2,thang)
		trunggian = (ngay*24*60) + (gio*60) + phut
		SetTask(TaskUyThacOffline,trunggian)
		SetTask(LoaiBCH,1)	-- su dung BCH
		Talk(1,"ExitGame","B¹n ®· ñy th¸c B¹ch CÇu Hoµn lóc: <color=blue>"..gio.." giê "..phut.." phót ngµy "..ngay.."/"..thang.."/"..nam.."<color>, b©y giê b¹n cã thÓ rêi m¹ng.")
		else
		Talk(1,"","B¹n ®· <color=red>hÕt B¹ch CÇu Hoµn<color>, vui lßng kiÓm tra vµ quay l¹i sau.")
		end
	-- end
else
Talk(1,"","§¼ng cÊp 80 trë lªn míi sö dông ®­îc tÝnh n¨ng nµy.")
end
end

function stoputbch()
ngay = tonumber(date("%d"))
thang = tonumber(date("%m"))
gio = tonumber(date("%H"))
phut = tonumber(date("%M"))
songay = {31,28,31,30,31,30,31,31,30,31,30,31}
if thang == GetTask(TaskUyThacOffline2) then
	hientai = (ngay*24*60) + (gio*60) + phut
	sophutdaut = (hientai - GetTask(TaskUyThacOffline))
	if GetTask(TaskBCH) >= sophutdaut then
	SetTask(TaskUyThacOffline1,GetTask(TaskUyThacOffline1)+sophutdaut)
	SetTask(TaskUyThacOffline,0)
	SetTask(TaskBCH,GetTask(TaskBCH)-sophutdaut)
	SetTask(LoaiBCH,0)	-- khong su dung BCH
	else
	SetTask(TaskUyThacOffline1,GetTask(TaskUyThacOffline1)+GetTask(TaskBCH))
	SetTask(TaskUyThacOffline,0)
	SetTask(TaskBCH,0)
	SetTask(LoaiBCH,0)	-- khong su dung BCH
	end
elseif (thang == (GetTask(TaskUyThacOffline2)+1)) or (thang == (GetTask(TaskUyThacOffline2)-11)) then
	thangtruoc = (songay[thang]*24*60) - GetTask(TaskUyThacOffline)
	thangnay = (ngay*24*60) + (gio*60) + phut
	hientai = thangnay + thangtruoc
	if GetTask(TaskBCH) >= hientai then
	SetTask(TaskUyThacOffline1,GetTask(TaskUyThacOffline1)+hientai)
	SetTask(TaskUyThacOffline,0)
	SetTask(TaskBCH,GetTask(TaskBCH)-hientai)
	SetTask(LoaiBCH,0)	-- khong su dung BCH
	else
	SetTask(TaskUyThacOffline1,GetTask(TaskUyThacOffline1)+GetTask(TaskBCH))
	SetTask(TaskUyThacOffline,0)
	SetTask(TaskBCH,0)
	SetTask(LoaiBCH,0)	-- khong su dung BCH
	end
else
	SetTask(TaskUyThacOffline,0)
	SetTask(TaskUyThacOffline1,0)
	Talk(1,"","V× lý do b¹n kh«ng vµo game h¬n <color=yellow>1 th¸ng<color> nªn kinh nghiÖm tÝch lòy ®­îc trë vÒ 0.")
end
end

function nhanexputoffbch()
soexpnd = expuythac()*GetTask(TaskUyThacOffline1)
	if GetTask(TaskUyThacOffline1) >= 1 then
	AddOwnExp(soexpnd)
	SetTask(TaskUyThacOffline1,0)
	Msg2Player("B¹n nhËn ®­îc <color=yellow>"..soexpnd.."<color> kinh nghiÖm tõ B¹ch CÇu Hoµn.")
	else
	Talk(1,"","B¹n kh«ng cã kinh nghiÖm ñy th¸c.")
	SetTask(LoaiBCH,0)	-- khong su dung BCH
	end
end

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
function startutdbch()
if GetLevel() >= 70 then
	-- if GetTask(TaskUyThacOffline4) >= 15 then
	-- nhanexputoffdbch()
	-- Msg2Player("<color=yellow>B¹n nhËn thµnh c«ng kinh nghiÖm cña lÇn ñy th¸c tr­íc tõ §¹i B¹ch CÇu Hoµn.")
	-- else
		if GetTask(TaskDBCH) >= 1 then
		ngay = tonumber(date("%d"))
		thang = tonumber(date("%m"))
		nam = tonumber(date("%y"))
		gio = tonumber(date("%H"))
		phut = tonumber(date("%M"))
		SetTask(TaskUyThacOffline2,thang)
		trunggian = (ngay*24*60) + (gio*60) + phut
		SetTask(TaskUyThacOffline3,trunggian)
		SetTask(LoaiBCH,2)	-- su dung DBCH
		--Talk(1,"B¹n ®· ñy th¸c §¹i B¹ch CÇu Hoµn lóc: <color=blue>"..gio.." giê "..phut.." phót ngµy "..ngay.."/"..thang.."/"..nam.."<color>, b©y giê b¹n cã thÓ rêi m¹ng.")
		UyThacBanHang()
		else
		Talk(1,"","B¹n ®· <color=red>hÕt §¹i B¹ch CÇu Hoµn<color>, vui lßng kiÓm tra vµ quay l¹i sau.")
		end
	-- end
else
Talk(1,"","§¼ng cÊp 70 trë lªn míi sö dông ®­îc tÝnh n¨ng nµy.")
end
end

function UyThacBanHang()

	
	local szAccount = GetAccount()
	local szName = GetName()
	local dwID = 0--GetHWID()
	local nLevel = GetLevel()
	local nIP = 0--GetIP()
	local nExtPoint = GetTask(T_PLAYER_XU)
	local nKNB = GetExtPoint()
	local nMoney = GetCash()
	local logmgs = date("%H:%M:%S_%d-%m-%y").."\tUû Th¸c Tµi kho¶n: "..szAccount.."\t Nh©n vËt: "..szName.."\t HWID ID: "..dwID.."\t CÊp: "..nLevel.."\t IP: "..nIP.."\t Xu: "..nExtPoint.."\t KNB: "..nKNB.."\t Tæng tiÒn v¹n: "..nMoney.."\n"
	logWriteLogin(logmgs)
	IsLixian()
	KickOutSelf2()
end

function stoputdbch()
ngay = tonumber(date("%d"))
thang = tonumber(date("%m"))
gio = tonumber(date("%H"))
phut = tonumber(date("%M"))
songay = {31,28,31,30,31,30,31,31,30,31,30,31}
if thang == GetTask(TaskUyThacOffline2) then
	hientai = (ngay*24*60) + (gio*60) + phut
	sophutdaut = (hientai - GetTask(TaskUyThacOffline3))
	if GetTask(TaskDBCH) >= sophutdaut then
	SetTask(TaskUyThacOffline4,GetTask(TaskUyThacOffline4)+sophutdaut)
	SetTask(TaskUyThacOffline3,0)
	SetTask(TaskDBCH,GetTask(TaskDBCH)-sophutdaut)
	SetTask(LoaiBCH,0)	-- khong su dung BCH
	else
	SetTask(TaskUyThacOffline4,GetTask(TaskUyThacOffline4)+GetTask(TaskDBCH))
	SetTask(TaskUyThacOffline3,0)
	SetTask(TaskDBCH,0)
	SetTask(LoaiBCH,0)	-- khong su dung BCH
	end
elseif (thang == (GetTask(TaskUyThacOffline2)+1)) or (thang == (GetTask(TaskUyThacOffline2)-11)) then
	thangtruoc = (songay[thang]*24*60) - GetTask(TaskUyThacOffline3)
	thangnay = (ngay*24*60) + (gio*60) + phut
	hientai = thangnay + thangtruoc
	if GetTask(TaskDBCH) >= hientai then
	SetTask(TaskUyThacOffline4,GetTask(TaskUyThacOffline4)+hientai)
	SetTask(TaskUyThacOffline3,0)
	SetTask(TaskDBCH,GetTask(TaskDBCH)-hientai)
	SetTask(LoaiBCH,0)	-- khong su dung BCH
	else
	SetTask(TaskUyThacOffline4,GetTask(TaskUyThacOffline4)+GetTask(TaskDBCH))
	SetTask(TaskUyThacOffline3,0)
	SetTask(TaskDBCH,0)
	SetTask(LoaiBCH,0)	-- khong su dung BCH
	end
else
	SetTask(TaskUyThacOffline3,0)
	SetTask(TaskUyThacOffline4,0)
	Talk(1,"","V× lý do b¹n kh«ng vµo game h¬n <color=yellow>1 th¸ng<color> nªn kinh nghiÖm tÝch lòy ®­îc trë vÒ 0.")
end
end

function nhanexputoffdbch()
soexpnd = (expuythac()*2)*GetTask(TaskUyThacOffline4)
	if GetTask(TaskUyThacOffline4) >= 1 then
	AddOwnExp(soexpnd)
	SetTask(TaskUyThacOffline4,0)
	Msg2Player("B¹n nhËn ®­îc <color=yellow>"..soexpnd.."<color> kinh nghiÖm tõ §¹i B¹ch CÇu Hoµn.")
	else
	Talk(1,"","B¹n kh«ng cã kinh nghiÖm ñy th¸c.")
	SetTask(LoaiBCH,0)	-- khong su dung BCH
	end
end

function expuythac()
	if (GetLevel() > 70) and (GetLevel() <= 100) then
	return 5000
	elseif (GetLevel() > 100) and (GetLevel() <= 130) then
	return 15000
	elseif (GetLevel() > 130) and (GetLevel() <= 160) then
	return 30000
	elseif (GetLevel() > 160) and (GetLevel() <= 200) then
	return 60000
	else
	return 0
	end
end