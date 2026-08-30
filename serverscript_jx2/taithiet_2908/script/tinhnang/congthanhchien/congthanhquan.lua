--Author: Fong KiÒu
--Date: 2021
--Fucntion: C«ng Thµnh Quan

Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")
Include("\\script\\tinhnang\\loidai\\lib_loidai.lua")

NOT_ENOUGH_LEVEL = "Ng­¬i kh«ng ®ñ "..ENOUGH_LEVEL.." cÊp ®é tham gia c«ng thµnh."
MAX_NUM_AWD = 200

function main()
	
	dofile("script/tinhnang/congthanhchien/congthanhquan.lua")

	local wday = tonumber(GetLocalDate("%w"))	
	local hour = tonumber(GetLocalDate("%H"))
	local nminus = tonumber(GetLocalDate("%M"))
		
	local tbOption = {}
	tinsert(tbOption,"§©y lµ n¬i nghÞ sù c«ng thµnh chiÕn, ng­¬i ®Õn cã viÖc g×?")
	-- [VACTC 30/08] dat ngoac sai: dieu kien cu la 'gio thuoc {18,19,20}
	-- VA phut < 30', nen muc nay BIEN MAT trong cac phut 30-59 cua ca ba
	-- gio. Y dinh la mot khoang lien tuc 18:00 -> 20:30.
	if (wday == 4 and hour >= 18 and (hour < 20 or (hour == 20 and nminus < 30))) then
		tinsert(tbOption,"B¸o danh L«i §µi/RegisterLD")
	end	
	-- [VACTC 30/08] cung loi dat ngoac: muc nay chi hien khi phut >= 30,
	-- nen khoang 21:00-21:29 bi mat. Y dinh la 20:30 -> het gio 21.
	if (wday == 4 and ((hour == 20 and nminus >= 30) or hour == 21)) then
		tinsert(tbOption,"Tham gia L«i §µi/PreEnterGame")
	end
	if (wday == 5 and hour >= 20 and hour <= 21) then
		tinsert(tbOption,"Tham gia c«ng thµnh chiÕn tr­êng/CTC")
	end
	tinsert(tbOption,"T×m hiÓu th«ng tin vÒ viÖc dù tuyÓn thi ®Êu L«i §µi/OnHelp")
	tinsert(tbOption,"T×m hiÓu quy t¾c ®¸nh C«ng Thµnh ChiÕn/OnHelp2")
	tinsert(tbOption, "L·nh th­ëng c«ng thµnh chiÕn /talk_citytong_award")
	tinsert(tbOption,"BiÕt råi, ta ®i tr­íc ®©y /OnCancel")
	SayEx(tbOption)
end

function PreEnterGame()
	local nSubWorldId = SubWorldID2Idx(MapTab[1][1])
	if nSubWorldId < 0 then	--chua mo map, ngung ham`
		Talk(1,"","Map L«i ®µi bang héi ch­a më.")
		return 
	end
	SubWorld = nSubWorldId	
	if(IsMission(MS_LOIDAIBH) == 0) then
		Talk(1,"","L«i ®µi bang héi ch­a khai më.")
		return 		
	end
	TongName = GetTongInfo(1)
	if (TongName ~= "") then --kh¸n gi¶ vµo xem
		for i = 0, 7 do
			if (IsArenaBegin(i) == 1) then
				Tong1, Tong2 = GetArenaBothSides(i)
				if (Tong1 == TongName or Tong2 == TongName) then
					EnterBattle(i)
					return
				end
			end
		end
	end
	EnterGame() --thµnh viªn bang ®· ®¨ng ký råi vµo lu«n
end

function EnterGame()
	ExtraArenaInfo = {"<#> (Kho¶ng trèng) ", "<#> (Kho¶ng trèng) ", "<#> (Kho¶ng trèng) ", "<#> (Kho¶ng trèng) ", "<#> (Kho¶ng trèng) ", "<#> (Kho¶ng trèng) ", "<#> (Kho¶ng trèng) ", "<#> (Kho¶ng trèng) "}
	for i = 0, 7 do
		if (IsArenaBegin(i) == 1) then
			Tong1, Tong2 = GetArenaBothSides(i)
			ExtraArenaInfo[i + 1] = " ("..Tong1.." vs "..Tong2..") "
		end
	end

	Say("B¹n muèn vµo c«ng thµnh chiÕn dù tuyÓn thi ®Êu l«i ®µi kh«ng??", 9, "<#> L«i ®µi 1"..ExtraArenaInfo[1].."/EnterBattle", "<#> L«i ®µi 2"..ExtraArenaInfo[2].."/EnterBattle", "<#> L«i ®µi 3"..ExtraArenaInfo[3].."/EnterBattle", "<#> L«i ®µi 4"..ExtraArenaInfo[4].."/EnterBattle", "<#> L«i ®µi 5"..ExtraArenaInfo[5].."/EnterBattle", "<#> L«i ®µi 6"..ExtraArenaInfo[6].."/EnterBattle", "<#> L«i ®µi 7"..ExtraArenaInfo[7].."/EnterBattle", "<#> L«i ®µi 8"..ExtraArenaInfo[8].."/EnterBattle","Kh«ng ®i n÷a/OnCancel")
end

function CTC()
	local nSubWorldId = SubWorldID2Idx(ID_MAP_CTC)
	if nSubWorldId < 0 then	--chua mo map, ngung ham`
		return
		print("Map CTC not Open")
	end
	SubWorld = nSubWorldId
	if(IsMission(MS_CTHANHCHIEN) == 0) then
		Talk(1,"","ChiÕn tr­êng c«ng thµnh ch­a khai më.")
		return 
	end	
	local nPThu  = GetMSPlayerCount(MS_CTHANHCHIEN,1)
	local nPCong = GetMSPlayerCount(MS_CTHANHCHIEN,2)
	SayEx({"HiÖn t¹i phe thñ thµnh bang <color=fire>"..NAME_BANGTHU.." : "..nPThu.."<color> ng­êi vµ phe c«ng thµnh bang <color=pink>"..NAME_BANGCONG.." : "..nPCong.."<color> ng­êi ", 
			"Ta muèn vµo Phe Thñ/vaophe_thu",
			"Ta muèn vµo phe C«ng/vaophe_cong",
			"BiÕt råi, ta ®i tr­íc ®©y /OnCancel"})
end

function vaophe_cong()
	local nSubWorldId = SubWorldID2Idx(ID_MAP_CTC)
	if nSubWorldId < 0 then	--chua mo map, ngung ham`
		return 
	end
	SubWorld = nSubWorldId
	if (GetLevel() < ENOUGH_LEVEL) then
		Talk(1,"",NOT_ENOUGH_LEVEL)
		return 
	end	
	local nTongName = GetTongInfo(1)
	if (nTongName ~= NAME_BANGCONG) then
		Talk(1,"",format("<sex> chØ cã thµnh viªn cña bang [%s] míi ®­îc vµo phe C«ng.", NAME_BANGCONG))
		return 
	end
	HauPhuongPheCong()
end

function vaophe_thu()
	local nSubWorldId = SubWorldID2Idx(ID_MAP_CTC)
	if nSubWorldId < 0 then	--chua mo map, ngung ham`
		return 
	end
	SubWorld = nSubWorldId
	if(IsMission(MS_CTHANHCHIEN) == 0) then
		Talk(1,"","ChiÕn tr­êng c«ng thµnh ch­a khai më.")
		return 
	end
	if (GetLevel() < ENOUGH_LEVEL) then
		Talk(1,"",NOT_ENOUGH_LEVEL)
		return 
	end
	local nTongName = GetTongInfo(1)
	if (nTongName ~= NAME_BANGTHU) then
		Talk(1,"",format("<sex> chØ cã thµnh viªn cña bang [%s] míi ®­îc vµo phe Thñ.", NAME_BANGTHU))
		return 
	end
	HauPhuongPheThu()
end

function OnHelp()
	Talk(2, "",	
	"L«i ®µi bang héi lµ n¬i häc hái vâ nghÖ gi÷a c¸c bang héi trªn giang hå! §©y sÏ lµ n¬i thi ®Êu c«ng b»ng",	
	"Muèn tham gia L«i ®µi bang héi, tr­íc hÕt ph¶i gÆp ta ®Ó xin phÐp! Ng­êi xin phÐp ph¶i lµ bang chñ cña bang héi thi ®Êu!"
	--"Sau khi tæ ®éi, ®éi tr­ëng 2 bªn sÏ ®Õn b¸o danh. Bang chñ hai bªn  mçi ng­êi ph¶i ®Æt c­îc thi ®Êu <color=yellow>100 v¹n l­îng<color>. ",	
	--"Sau khi ®Æt c­îc xong, xin phÐp thi ®Êu ®· hoµn thµnh. Trong lóc cã 2 bang héi kh¸c ®ang thi ®Êu, kh«ng cho phÐp b¸o danh",	
	--"Xin phÐp thµnh c«ng xong, thµnh viªn cña hai bang héi cã <color=yellow>10 phót<color> gia nhËp ®Êu tr­êng. Sau ®ã thi ®Êu sÏ chÝnh thøc b¾t ®Çu!" ,
	--"Th¾ng b¹i phô thuéc vµo ®iÓm tÝch lòy thi ®Êu cña hai bªn. Mçi thµnh viªn bang héi lµm träng th­¬ng ®èi ph­¬ng sÏ ®­îc 3 ®iÓm, ®èi ph­¬ng bÞ trõ 1 ®iÓm", 
	--"KÕt thóc thi ®Êu, bang héi nµo tÝch lòy ®iÓm cao h¬n sÏ giµnh th¾ng lîi!",	
	--"Ngoµi ra, trong lóc thi ®Êu mét bªn nµo kh«ng cßn thµnh viªn nµo ë l¹i ®Êu tr­êng th× th¾ng lîi sÏ thuéc vÒ bªn kia. ",
	--"Bang héi chiÕn th¾ng cã thÓ thu håi <color=yellow>100 v¹n l­îng<color> ®· ®ãng, ngoµi ra cßn ®­îc <color=yellow>80 v¹n l­îng<color> tiÒn th­ëng vµ <color=yellow>12.000 ®iÓm kinh nghiÖm bang héi<color>. ",
	--"Bang héi thua cuéc sÏ kh«ng lÊy l¹i ®­îc tiÒn ®· ®ãng, mµ cßn bÞ trõ <color=yellow>14.000 ®iÓm kinh nghiÖm bang héi<color>"
	)
end

function OnHelp2()
	Talk(3, "",	"Trong kho¶ng thêi gian tõ 18h00 ®Õn 19h00, bang chñ bang héi ch­a chiÕm thµnh cÊp 18 trë lªn cã thÓ ®Õn b¸o danh tham gia c«ng thµnh chiÕn vµo thø 5.",
	"ChØ cã bang chñ bang b¸o danh c«ng thµnh vµ b­íc qua thi ®Êu l«i ®µi, bang héi nµo dµnh chiÕn th¾ng sÏ nhËn quyÒn c«ng thµnh vµo tèi thø 6 lóc 20h.",
	"Yªu cÇu c¸c thµnh viªn tham gia cã mÆt ®óng giê nÕt trÔ giê sÏ kh«ng ®­îc phÐp tham dù.")
end

function talk_citytong_award()
	local city_owner = gettongownercity()
	if (check_award_condition(city_owner, 1) == 1) then
		Say("<#>PhÇn th­ëng bang chiÕm thµnh. Thµnh viªn bang chiÕm thµnh nhËn phÇn th­ëng huyÒn tinh cÊp 5, mçi ng­êi cã thÓ nhËn 1 viªn. Giíi h¹n nhËn "..MAX_NUM_AWD.." viªn huyÒn tinh cÊp 5. NÕu sau 18h00 vÉn ch­a nhËn hÕt th× bang chñ tr­íc 20h00 cã thÓ nhËn sè cßn l¹i.",
			3,
			"<#>Ta muèn nhËn huyÒn tinh kho¸ng th¹ch/#take_tong_award(1)",
			"<#>Ta lµ bang chñ, ®Õn ®Ó nhËn phÇn th­ëng cßn l¹i/take_tong_resaward",
			"<#> T¹m thêi ta kh«ng l·nh ®©u/OnCancel")
	end
end

function gettongownercity()
	local nTongName = GetTongInfo(1)
	if(BANGHOI_DANHSACH ~= nil and getn(BANGHOI_DANHSACH) >= 1) then
		if(BANGHOI_DANHSACH[1]  ~= nil) then
			if(nTongName == BANGHOI_DANHSACH[1][2]) then
				return BANGHOI_DANHSACH[1][2]
			end
		end
	end
	return ""
end

function check_award_condition(city_owner, show_talk)
	if (city_owner == "") then
		if (show_talk == 1) then
			Talk(1, "", "<#> ChØ cã bang ph¸i chiÕm ®­îc thµnh thÞ míi nhËn ®­îc phÇn th­ëng, h·y dÉn anh em cña m×nh ®i chiÕm mét thµnh nµo ®i!")
		end
		return 0
	end
	local wday = tonumber(GetLocalDate("%w"))	
	local hour = tonumber(GetLocalDate("%H"))
	if (wday ~= 1 or hour < 9 or hour >= 20) then
		if (show_talk == 1) then
			Talk(1, "", "<#> Mçi tuÇn tõ 9:00~20:00 lµ thêi gian l·nh th­ëng, ®Õn giê ®ã tíi ®i")
		end
		return 0
	end
	local nCountAwd = GetSoLuongThuongDaNhan()
	if(nCountAwd > MAX_NUM_AWD) then
		if (show_talk == 1) then
			Talk(1, "", "<#>PhÇn th­ëng tuÇn nµy ®· nhËn hÕt, tuÇn sau h·y ®Õn nhÐ.")
		end
		return 0
	end
	return 1		
end

function GetSoLuongThuongDaNhan()
	if(BANGHOI_DANHSACH ~= nil and getn(BANGHOI_DANHSACH) >= 1) then
		if(BANGHOI_DANHSACH[1]  ~= nil) then
			return BANGHOI_DANHSACH[1][4]
		end
	end
	return 0	
end

function SetSoLuongThuongNhanThem(nCountAwd)
	if(BANGHOI_DANHSACH ~= nil and getn(BANGHOI_DANHSACH) >= 1) then
		if(BANGHOI_DANHSACH[1]  ~= nil) then
			BANGHOI_DANHSACH[1][4] = nCountAwd
			local tbDataTongNew = TaoBang(BANGHOI_DANHSACH,"BANGHOI_DANHSACH","")
			SaveData("script/tinhnang/congthanhchien/danhsach_bang.lua",tbDataTongNew)
		end
	end
end

function take_tong_award(count)
	if (CalcFreeItemCellCount() < count) then
		Talk(1, "", "<#> Hµnh trang cña ng­¬i ®· ®Çy, chØnh lý l¹i xong míi ®Õn l·nh th­ëng.")
		return
	end
	
	local city_owner = gettongownercity()
	if (check_award_condition(city_owner, 0) == "") then
		return
	end
	
	local nH = tonumber(GetLocalDate("%H"));
	if (nH >= 18) then
		Talk(1, "", "<#>Thêi gian nhËn th­ëng cña mçi bang héi tõ 9h00 ®Õn 18h00, h·y ®Õn ®óng giê nµy ®Ó nhËn th­ëng. §èi víi phÇn th­ëng ch­a ®­îc nhËn, h·y th«ng b¸o cho bang chñ ®Õn nhËn tr­íc 20h00.")
		return 0
	end
	local today = tonumber(GetLocalDate("%Y%m%d"))
	local nMydate = GetTask(TSK_CITYTONG_DATE)
	if (nMydate ~= today) then
		AddItem(ITEM_TONG_AWD[1],ITEM_TONG_AWD[2],ITEM_TONG_AWD[3],ITEM_TONG_AWD[4],0,0,0)
		SetTask(TSK_CITYTONG_DATE, today)
		add_citybonus_task(city_owner, 1)
		local msg =	format("[Account:%s] (Name: %s) nhËn %d huyÒn tinh cÊp 5 tõ C«ng Thµnh Quan ",GetAccount(),GetName(),count)
		logHoatDong(msg)
		Talk(1, "", "<#>§©y lµ phÇn th­ëng cña ng­¬i, h·y nhËn lÊy.")
	else
		Talk(1, "", "<#>Ng­¬i ®· nhËn phÇn th­ëng tuÇn nµy råi, ®îi tuÇn sau nhÐ?")
	end
end

function take_tong_resaward()
	local city_owner = gettongownercity()
	if (check_award_condition(city_owner, 0) == "") then
		return
	end
	
	local nH = tonumber(GetLocalDate("%H"))
	if (nH < 18 or nH >= 20) then
		Talk(1, "", "<#>Bang chñ vµo mçi thø hai cã thÓ ®Õn nhËn phÇn th­ëng cßn l¹i tr­íc 18h00 ®Õn 20h00, h·y ®Õn ®óng giê ®Ó nhËn.")
		return 0
	end
	
	if (GetName() ~= GetTongInfo(2)) then
		Talk(1, "", "<#>ChØ cã bang chñ míi cã thÓ nhËn phÇn th­ëng bang héi cßn l¹i. H·y th«ng b¸o cho bang chñ quÝ bang ®Õn ®©y nhËn tr­íc 20h00.")
		return 0
	end
	
	local nFreecell = CalcFreeItemCellCount()
	if (nFreecell < 20) then
		Talk(1, "", "<#>Hµnh trang kh«ng ®ñ chç trèng, h·y s¾p xÕp l¹i hµnh trang.")
		return 0
	end
	
	local nCount = MAX_NUM_AWD - GetSoLuongThuongDaNhan()
	if(nCount <= 0) then
		Talk(1, "", "Quý bang ®· nhËn phÇn th­ëng tuÇn nµy råi, ®îi tuÇn sau nhÐ?")
		return
	end
	for i = 1, nCount do
		AddItem(ITEM_TONG_AWD[1],ITEM_TONG_AWD[2],ITEM_TONG_AWD[3],ITEM_TONG_AWD[4],0,0,0)
	end
	add_citybonus_task(city_index, nCount)
	local msg =	format("%s bang chñ [%s] nhËn %d huyÒn tinh cÊp 5 tõ C«ng Thµnh Quan phÇn th­ëng bang héi thèng trÞ %s tuÇn nµy.",GetName(),city_owner,nCount,CITYWAR_NAME)
	AddGlobalCountNews(msg,3)
	Msg2SubWorld(msg)
	logHoatDong(msg)	
	Say(format("§©y lµ phÇn th­ëng cña quÝ bang, %s viªn huyÒn tinh cßn l¹i!",nCount), 0)
end

function add_citybonus_task(city_owner, nCount)
	local nCountAwd = GetSoLuongThuongDaNhan()
	nCountAwd = nCountAwd + nCount
	SetSoLuongThuongNhanThem(nCountAwd)
end

function OnCancel()
	
end
