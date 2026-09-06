--hoangnhk
Include("\\script\\header\\fuyuan.lua");
Include("\\script\\lib\\lib_task.lua")

function main()
	-- dofile("script/global/npcchucnang/truyennhan.lua")
	Say("<color=green><npc>: Chän ",3,
	-- "NhËn ®iÓm phóc duyªn cña b¹n/nhanpd",
	"NhËn ®iÓm danh väng (ch¬i nhiÒu giê)/nhandv",
	-- "KiÓm tra thêi gian tİch lòy Online/tichluy",
	"Liªn quan ®Õn phóc duyªn/timhieu",
	"Tho¸t/no")
end

function nhanpd()
	if(IsFuYuanAvailable() ~= 1) then
		FuYuan_Start();
		FuYuan_Add(2);
		Say("Ng­¬i míi s¬ nhËp giang hå, ®©y lµ mét lÔ vËt nhá, h·y nhËn lÊy.",0);
	return end;
	local nResult = FuYuan_Gain();
	if( nResult == 0 ) then
		Say("Ng­¬i x«ng pha giang hå nh­ng vÉn ch­a ®ñ giê tİch lòy!",0);
	elseif( nResult == 1 ) then
		Say("Ng­¬i x«ng pha giang hå ®· l©u, còng cã 1 chót c«ng lao, ®©y phÇn th­ëng cña ng­¬i.",0);
	end
end;

function nhandv()
if (GetGameTime() - GetTask(TASK_THOIGIAN9) >= 43200) then
	SetTask(TASK_THOIGIAN9,GetGameTime())
	Talk(1,"","Ta nh×n thÊy sù cè g¾ng ë trong m¾t cña ng­¬i, ng­¬i tiÕn bé ta còng rÊt vui mõng. H·y tiÕp tôc cè g¾ng lªn!")
	i = random(0,99)
	if (i < 19) then
		AddRepute(1)
		Msg2Player("Danh väng t¨ng lªn 1 ®iÓm.")
	elseif (i < 36) then
		AddRepute(2)
		Msg2Player("Danh väng t¨ng lªn 2 ®iÓm.")
	elseif (i < 51) then
		AddRepute(3)
		Msg2Player("Danh väng t¨ng lªn 3 ®iÓm.")
	elseif (i < 64) then
		AddRepute(4)
		Msg2Player("Danh väng t¨ng lªn 4 ®iÓm.")
	elseif (i < 75) then
		AddRepute(5)
		Msg2Player("Danh väng t¨ng lªn 5 ®iÓm.")
	elseif (i < 84) then
		AddRepute(6)
		Msg2Player("Danh väng t¨ng lªn 6 ®iÓm.")
	elseif (i < 91) then
		AddRepute(7)
		Msg2Player("Danh väng t¨ng lªn 7 ®iÓm.")
	elseif (i < 96) then
		AddRepute(8)
		Msg2Player("Danh väng t¨ng lªn 8 ®iÓm.")
	elseif (i < 99) then
		AddRepute(9)
		Msg2Player("Danh väng t¨ng lªn 9 ®iÓm.")
	else
		AddRepute(10)
		Msg2Player("Danh väng t¨ng lªn 10 ®iÓm.")
	end
else
	Say("Ng­¬i vÉn ch­a ®ñ ®iÒu kiÖn ®Ó nhËn danh väng. H·y cè g¾ng h¬n n÷a!",0)
end
end;

function tichluy()
	if(IsFuYuanAvailable() ~= 1) then
		FuYuan_Start();
		FuYuan_Add(2);
		Say("Ng­¬i míi s¬ nhËp giang hå, ®©y lµ mét lÔ vËt nhá, h·y nhËn lÊy.",0);
	return end;
	local nFYTotalTime, nFYValidTime = FuYuan_GetDepositTime();
	--Say("Ng­¬i míi s¬ nhËp giang hå, ®©y lµ mét lÔ vËt nhá, h·y nhËn lÊy.",0);
	Msg2Player(format("KÓ tõ lÇn nhËn phóc duyªn tr­íc, ng­¬i ®· tİch lòy <color=Yellow>%d g %d phót %d gi©y<color> thêi gian Online",nFYValidTime/60/60/18,mod(nFYValidTime/60/18,60),mod(nFYValidTime/18,60)));
end;

function minhnguyet()
NewWorld(520,1601,3268);
SetFightState(0);
Msg2Player("Xin h·y ngåi yªn, chóng ta ®i "..GetWorldName(520).." nµo");
end;

function timhieu()
	Talk( 3, "", "Phóc duyªn tİch lòy: Th«ng qua tİch lòy thêi gian ch¬i, cã thÓ ®æi ®iÓm phóc duyªn. Cã thÓ tíi T­¬ng D­¬ng thÇn bİ th­¬ng nh©n LiÔu Êt ®æi c¸c lo¹i vËt phÈm ®Æc thï.","Phóc duyªn ®æi: tİch lòy hai giê online trë lªn , ®i c¸c ®¹i thµnh thŞ t×m Vâ L©m Minh Chñ TruyÒn Nh©n lµ ®­îc ®æi, mét ngµy ®­îc ®æi 2 lÇn, tİch lòy ë tuyÕn thêi gian 4 giê trë lªn, mçi giê ®æi 1 ®iÓm phóc duyªn.", "H¹n chÕ ®iÒu kiÖn: Ngåi tï kh«ng ®­îc tİch lòy ®iÓm phóc duyªn." );
end;

function no()
end;
