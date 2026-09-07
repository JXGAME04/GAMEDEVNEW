--Author: Fong KiÒu
--Function: ThÇn hµnh phï
--Date: 21/06/2021

-- Include("\\script\\header\\revivepos_head.lua")
Include("\\script\\header\\forbidmap.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_server.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")

SELECT_GO = "ThÇn hµnh phï, ®i ®Õn n¬i ng­¬i muèn."
SELECT_REVIVE = "ThiÕt ®Æt ®iÓm håi sinh cho lÇn sau sö dông thæ ®Þa phï."
SELECT_OUT = "Rêi khái/no"
SUCCESS = "ThiÕt ®Æt ®iÓm håi sinh t¹i %s thµnh c«ng."
PLACE_TOGO = "H·y lùa chän n¬i cÇn ®Õn"

THON_TT_MP_ARRAY={
	[1] = {1591,3170},		--phuong tuong phu
	[78] = {1565,3219},		--tuong duong phu
	[11] = {3124,5117},		--thanh do
	[162] = {1576,3133},	--dai ly
	[37] = {1722,3081},		--bien kinh
	[80] = {1756,3006},		--duong chau
	[176] = {1573,2933},	--lam an

	[20] = {3552,6194},		--giang tan thon
	[53] = {1622,3189},		--ba lang huyen
	[99] = {1628,3203},		--vinh lac tran
	[101] = {1688,3154},	--dao huong thon
	[100] = {1628,3203},	--chu tien tran
	[121] = {1951, 4509},	--long mon tran 121, 1715, 4394
	[153] = {1605,3220},	--thac co tran
	[174] = {1573,3203},	--long tuyen thon
	[54] = {1650,3169},		--nam nhac tran
	[175] = {1673,3168},	--tay son thon

	[103] = {1668,3131},	--thieu lam
	[59] = {1650,3169},		--thien vuong
	[183] = {1417,3540},	--ngu doc
	[25] = {4046,5170},		--duong mon
	[13] = {1899,4978},		--nga my
	[154] = {382,1369},		--thuy yen
	[115] = {1523,3780},	--cai bang
	[45] = {1603,3191},		--thien nhan
	[81] = {1690,3169},		--vo dang
	[131] = {1526,3152},	--con lon
};

RUONG_ARRAY= {
	{{1591,3170},{1571,3118},{1658,3156},{1545,3218},{1630,3260}},--phuong tuong
	{{3155,5075},{3185,5180},{3001,5104},{3052,4981},{3236,5028}},--thanh do
	{{1576,3133},{1625,3151}},--dai ly
	{{1722,3081},{1613,3013},{1694,3207},{1853,2955}},--bien kinh
	{{1565,3219},{1576,3342},{1452,3262},{1481,3164},{1686,3235}},--tuong duong
	{{1756,3006},{1661,2985},{1626,3176},{1709,3227},{1856,3032}},--duong chau
	{{1573,2933},{1687,3277},{1391,3312}}--lam an
};

THUOC_ARRAY= {
	{1603,3193},--phuong tuong
	{3140,5133},--thanh do
	{1497,3209},--dai ly
	{1776,3091},--bien kinh
	{1610,3245},--tuong duong
	{1775,3080},--duong chau
	{1616,2978}--lam an
};

TAPHOA_ARRAY= {
	{1561,3205},--phuong tuong
	{3094,5136},--thanh do
	{1537,3203},--dai ly
	{1786,3101},--bien kinh
	{1629,3262},--tuong duong
	{1701,3021},--duong chau
	{1339,3167}--lam an
};
THANH_ARRAY = {
{1 ,	"Ph­îng T­êng Phñ"	,{
		{5	,"Ph­îng T­êng Trung T©m"},
		{1	,"Ph­îng T­êng §«ng"},
		{2	,"Ph­îng T­êng T©y"},
		{3	,"Ph­îng T­êng Nam"},
		{4	,"Ph­îng T­êng B¾c"}
}},
{11 ,	"Thµnh §« Phñ"	,{
		{5 	,"Thµnh §« Trung T©m"},
		{6 	,"Thµnh §« §«ng"},
		{7 	,"Thµnh §« T©y"},
		{8 	,"Thµnh §« Nam"},
		{9 	,"Thµnh §« B¾c"}
}},
{162 ,	"§¹i Lý Phñ"	,{
		{63 	,"§¹i Lý Trung T©m"},
		{64	,"§¹i Lý B¾c"}
}},
{37 ,	"BiÖn Kinh Phñ"	,{
		{23	,"BiÖn Kinh Trung T©m"},
		{24	,"BiÖn Kinh §«ng"},
		{25	,"BiÖn Kinh T©y"},
		{26	,"BiÖn Kinh B¾c"}
}},
{78 ,	"T­¬ng D­¬ng Phñ"	,{
		{29	,"T­¬ng D­¬ng Trung T©m"},
		{30	,"T­¬ng D­¬ng §«ng"},
		{31	,"T­¬ng D­¬ng Nam"},
		{32	,"T­¬ng D­¬ng T©y"},
		{33	,"T­¬ng D­¬ng B¾c"}
}},
{80 ,	"D­¬ng Ch©u Phñ"	,{
		{34	,"D­¬ng Ch©u Trung T©m"},
		{35	,"D­¬ng Ch©u §«ng"},
		{36	,"D­¬ng Ch©u B¾c"},
		{37	,"D­¬ng Ch©u Nam"},
		{38	,"D­¬ng Ch©u T©y"}
}},
{176 ,	"L©m An Phñ"	,{
		{67	,"L©m An Nam"},
		{68	,"L©m An §«ng"},
		{69	,"L©m An B¾c"}
}}
};
--diem phuc sinh thon tran
THON_ARRAY = {
{53 ,	"Ba L¨ng HuyÖn"	,19},
{20 ,	"Giang T©n Th«n"	,10},
{121 ,	"Long M«n TrÊn"	,55},
{54 ,	"Nam Nh¹c TrÊn"	,17},
--{174 ,	"Long TuyÒn Th«n"	,66},
--{101 ,	"§¹o H­¬ng Th«n"	,47},
--{99 ,	"VÜnh L¹c TrÊn"	,43},
--{100 ,	"Chu Tiªn TrÊn"	,45},
--{153 ,	"Th¹ch Cæ TrÊn"	,59},
--{175 ,	"T©y S¬n Th«n"		,1}
};
--diem phuc sinh mon phai
MONPHAI_ARRAY = {
{103	,	"ThiÕu L©m ph¸i"			,52},
{59		,	"Thiªn V­¬ng bang"	,21},
{25		,	"§­êng M«n"				,15},
{183	,	"Ngò §éc gi¸o"				,71},
{13		,	"Nga My ph¸i"				,13},
{154	,	"Thóy Yªn m«n"			,61},
{115	,	"C¸i Bang"						,53},
{45		,	"Thiªn NhÉn gi¸o"		,28},
{81		,	"Vâ §ang ph¸i"				,40},
{131	,	"C«n L«n ph¸i"				,57}
};

TRAIN_ARRAY1= {
[1] = {
	"KiÕm C¸c T©y Nam [20]/selluyen1", -- doi map khac map nay hien tai la map 40
	"Vò L¨ng S¬n [20]/selluyen1",
	"Phôc Ng­u §«ng [30]/selluyen1",
	"Thôc C­¬ng S¬n [30]/selluyen1",
	"Phôc Ng­u T©y [40]/selluyen1",
	"Hoµng Hµ Nguyªn §Çu [40]/selluyen1",
	"Trang TiÕp/luyencong2",
	"Ta kh«ng muèn ®i/no"
	},
[2] = {
	{20,19,3102,3963},
	{20,70,1608,3230},
	{30,90, 1651,3571},
	{30,92,1632,3290},
	{40,41,2078,2805},
	{40,122,1612,3323}
	}
};

TRAIN_ARRAY2={
[1] = {
	"L­u Tiªn §éng [50]/selluyen2",
	"¸c b¸ ®Þa ®¹o [50]/selluyen2",
	"Thiªn T©m Th¸p tÇng 3 [60]/selluyen2",  -- map 7x doi map khac
	"Hoµnh S¬n ph¸i [60]/selluyen2",
	"L©m Du Quan [70]/selluyen2",
	"L·o Hæ §éng [70]/selluyen2",
	"Ch©n nói Tr­êng B¹ch [80]/selluyen2",
	"Sa M¹c §Þa BiÓu [80]/selluyen2",
	"Sa M¹c 1 [90]/selluyen2",
	"Sa M¹c 2 [90]/selluyen2",
	"Sa M¹c 3 [90]/selluyen2",
	"Tr­êng B¹ch S¬n Nam [90]/selluyen2",
	"Tr­êng B¹ch s¬n b¾c [90]/selluyen2",
	"Kháa Lang ®éng [90]/selluyen2",
	"TiÕn cóc ®éng [90]/selluyen2",
	"C¸n viªn ®éng [90]/selluyen2",
	"Phong L¨ng ®é [90]/selluyen2",
	"M¹c Cao QuËt [90]/selluyen2",
	"Ta kh«ng muèn ®i/no"
},
	
[2] = {
	{50,125,1809,3208},
	{50,163,1558,3199},
	{60,166,1649,3231},
	{60,56,1493,3530},
	{70,319,1630,3592},
	{70,123,1698,3374},
	{80,320,1146,3130},
	{80,224,1621,3214},
	{90,226}, --Sa M¹c 1
    {90,227}, --Sa M¹c 2
    {90,228}, --Sa M¹c 4
	{90,198},
	{90,201},
	{90,74},
	{90,147},
	{90,221},
	{90,224},
	{90,225}
}
};


print("=====[ Script Than Hanh Phu loaded ]=====")

function main(nIndex)

	  -- dofile("script/item/ib/shenxingfu.lua")
local nCurTime = tonumber(GetLocalDate("%y%m%d%H%M"))
if(nCurTime < SERVER_OPEN) then
Talk(1,"","§óng 20h míi Open Server .")

return 1
end
	local nSubWorldID = GetWorldPos()
	
	if (GetTaskTemp(99) == 1 ) or ( nSubWorldID >= 387 and nSubWorldID <= 395)then
		Talk(1,"","HiÖn t¹i ng­¬i kh«ng thÓ sö dông thÇn hµnh phï!")
		return
	end
	
	if (nSubWorldID >= 375 and nSubWorldID <= 386) then
		Talk(1,"","B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng thuéc khu vùc ®Æc thï, kh«ng thÓ sö dông thÇn hµnh phï.")
		return
	end
	
	if (nSubWorldID >= 416 and nSubWorldID <= 511) then
		Talk(1,"","B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng thuéc khu vùc ®Æc thï, kh«ng thÓ sö dông thÇn hµnh phï.")
		return
	end
	
	if (nSubWorldID == 995 or nSubWorldID == 324 or nSubWorldID == 44 or nSubWorldID == 197 or nSubWorldID == 208 or nSubWorldID == 209 or nSubWorldID == 210 or nSubWorldID == 211 or nSubWorldID == 212 or (nSubWorldID >= 213 and nSubWorldID <= 223)	or nSubWorldID == 336  or nSubWorldID == 342	or nSubWorldID == 175	or nSubWorldID == 337	or nSubWorldID == 338	or nSubWorldID == 339 or ( nSubWorldID >= 387 and  nSubWorldID <= 395 ) )then 
		Talk(1,"","B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng thuéc khu vùc ®Æc thï, kh«ng thÓ sö dông thÇn hµnh phï.")
		return
	end

	if (CheckAllMaps(nSubWorldID) == 1) then
		Talk(1,"","B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng thuéc khu vùc ®Æc thï, kh«ng thÓ sö dông thÇn hµnh phï.")
		return
	end;
	
	if (GetLevel() < 10) then
		Talk(1,"","Ng­êi ch¬i ph¶i ®¹t ®¼ng cÊp 10 trë lªn míi cã thÓ sö dông thÇn hµnh phï.")
		return
	end
	
	SayEx({SELECT_GO,
	"ThiÕt ®Æt ®iÓm håi sinh ®Ó sö dông thæ ®Þa phï/diemphucsinh",
	"Sö dông thuËt thÇn hµnh ®i ®Õn n¬i chØ ®Þnh/chondiadiem1",
	"§Õn r­¬ng chøa ®å/ruong",
	"§Õn hiÖu thuèc/hieuthuoc",
	"§Õn t¹p hãa/taphoa",
	-- "Quay l¹i n¬i cò/quaylai",
	--"§i Vi S¬n §¶o/divisondao",
	"§i Map VIP Train/dimacbacthaon",
	SELECT_OUT})
	
	CheckPlayerTitle()
end

function dimacbacthaon()
	local nXu = 10
	if(GetTask(T_PLAYER_XU) < nXu) then
		Talk(1,"","<sex> kh«ng ®ñ "..nXu.." Xu mµ muèn lõa ta sao?")
		return
	end
	if (GetTask(TASK_NEWTHOREN7) == 2) then
		local tbPos = { 
			{1265,2493}, 
			{1281,2502}, 
			{1291,2546}, 
			{1289,2482}, 
		}
		SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) -  nXu)
		local nrand = random(getn(tbPos))
		if(NewWorld(341, tbPos[nrand][1], tbPos[nrand][2]) > 0) then
			SetFightState(0)
		end
	else
		Talk(1,"","Ng­¬i ch­a ®¨ng ký VIP th¸ng !")
	end
end

--call from source
function GotoMapId(idx)
	if(GetItemCount(0,6,1,1271,-1,-1) <= 0) then
		Msg2Player("§Ó sö dông ThÇn Hµnh Chi ThuËt trªn ng­êi cÇn cã ThÇn Hµnh Phï, cã ph¶i ®Ó quªn trong r­¬ng kh«ng?")
	return end
	local nCurTime = tonumber(GetLocalDate("%y%m%d%H%M"))
	if(nCurTime < SERVER_OPEN) then
		Msg2Player("§óng 20h míi Open Server .")
		return
	end
	local nSubWorldID = GetWorldPos()
	
	if (GetTaskTemp(99) == 1 ) or ( nSubWorldID >= 387 and nSubWorldID <= 395)then
		Msg2Player("HiÖn t¹i ng­¬i kh«ng thÓ sö dông thÇn hµnh phï!")
		return
	end
	
	if (nSubWorldID >= 375 and nSubWorldID <= 386) then
		Msg2Player("B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng thuéc khu vùc ®Æc thï, kh«ng thÓ sö dông thÇn hµnh phï.")
		return
	end
	
	if (nSubWorldID >= 416 and nSubWorldID <= 511) then
		Msg2Player("B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng thuéc khu vùc ®Æc thï, kh«ng thÓ sö dông thÇn hµnh phï.")
		return
	end
	
	if (nSubWorldID == 995 or nSubWorldID == 324 or nSubWorldID == 44 or nSubWorldID == 197 or nSubWorldID == 208 or nSubWorldID == 209 or nSubWorldID == 210 or nSubWorldID == 211 or nSubWorldID == 212 or (nSubWorldID >= 213 and nSubWorldID <= 223)	or nSubWorldID == 336 or nSubWorldID == 341 or nSubWorldID == 342	or nSubWorldID == 175	or nSubWorldID == 337	or nSubWorldID == 338	or nSubWorldID == 339 or ( nSubWorldID >= 387 and  nSubWorldID <= 395 ) )then 
		Msg2Player("B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng thuéc khu vùc ®Æc thï, kh«ng thÓ sö dông thÇn hµnh phï.")
		return
	end

	if (CheckAllMaps(nSubWorldID) == 1) then
		Msg2Player("B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng thuéc khu vùc ®Æc thï, kh«ng thÓ sö dông thÇn hµnh phï.")
		return
	end;
	
	if (GetLevel() < 10) then
		Msg2Player("Ng­êi ch¬i ph¶i ®¹t ®¼ng cÊp 10 trë lªn míi cã thÓ sö dông thÇn hµnh phï.")
		return
	end

	local ar = 0;
	for i=1,getn(tab_lv90map) do
		if(tab_lv90map[i][1] == idx) then
		ar = i
		break end
	end
	if(ar > 0) then
		gopos_step3lv90(ar)
	return end
	for i=1,getn(tab_lv80map) do
		if(tab_lv80map[i][1] == idx) then
		ar = i
		break end
	end
	if(ar > 0) then
		gopos_step3lv80(ar)
	return end
	for i=1,getn(tab_lv70map) do
		if(tab_lv70map[i][1] == idx) then
		ar = i
		break end
	end
	if(ar > 0) then
		gopos_step3lv70(ar)
	return end
	for i=1,getn(tab_lv60map) do
		if(tab_lv60map[i][1] == idx) then
		ar = i
		break end
	end
	if(ar > 0) then
		gopos_step3lv60(ar)
	return end
	for i=1,getn(tab_lv50map) do
		if(tab_lv50map[i][1] == idx) then
		ar = i
		break end
	end
	if(ar > 0) then
		gopos_step3lv50(ar)
	return end
	for i=1,getn(tab_lv40map) do
		if(tab_lv40map[i][1] == idx) then
		ar = i
		break end
	end
	if(ar > 0) then
		gopos_step3lv40(ar)
	return end
	for i=1,getn(tab_lv30map) do
		if(tab_lv30map[i][1] == idx) then
		ar = i
		break end
	end
	if(ar > 0) then
		gopos_step3lv30(ar)
	return end
	for i=1,getn(tab_lv20map) do
		if(tab_lv20map[i][1] == idx) then
		ar = i
		break end
	end
	if(ar > 0) then
		gopos_step3lv20(ar)
	return end
end;

function gotoluyencong()
	
	local tab_Content = {

		"B¶n ®å luyÖn c«ng 90 trë lªn./#gopos_step2lv90()",
		"B¶n ®å LuyÖn c«ng 20 ®Õn 80./#luyencongtanthu()",
		"Rêi khái/no",

	}
	Say("ThÇn hµnh phï, ®i ®Õn n¬i ng­¬i muèn.", getn(tab_Content), tab_Content);
end;

function luyencongtanthu()
	local tab_Content = {
		"Di chuyÓn ®Õn b¶n ®å luyÖn c«ng cÊp 20 /gopos_step2lv20",
		"Di chuyÓn ®Õn b¶n ®å luyÖn c«ng cÊp 30 /gopos_step2lv30",
		"Di chuyÓn ®Õn b¶n ®å luyÖn c«ng cÊp 40 /gopos_step2lv40",
		"Di chuyÓn ®Õn b¶n ®å luyÖn c«ng cÊp 50 /gopos_step2lv50",
		"Di chuyÓn ®Õn b¶n ®å luyÖn c«ng cÊp 60 /gopos_step2lv60",
		"Di chuyÓn ®Õn b¶n ®å luyÖn c«ng cÊp 70 /gopos_step2lv70",
		"Di chuyÓn ®Õn b¶n ®å luyÖn c«ng cÊp 80 /gopos_step2lv80",
		"Ta ch­a muèn ®i ®©u c¶./no",
	}
	Say("Lùa chän b¶n ®å luyÖn c«ng cÇn thiÕt.", getn(tab_Content), tab_Content);
end;

tab_lv90map = {
		{875,1576,3177	,"H¾c Sa ®éng",},
		{322,1589,3164	,"Tr­êng B¹ch S¬n B¾c",},
		{321,967,2313	,"Tr­êng B¹ch S¬n Nam",},
		{75,1811,3012	,"Kho¶ Lang ®éng",},
		{225,1474,3275	,"Sa M¹c Mª Cung 1",},
		{226,1560,3184	,"Sa M¹c Mª Cung 2",},
		{227,1588,3237	,"Sa M¹c Mª Cung 3",},
		{336,1124,3187	,"Phong L¨ng ®é",},
		{340,1845,3438	,"M¹c Cao QuËt",},
		{144,1691,3020	,"D­îc V­¬ng ®éng tÇng 4",},
		{93,1529,3166	,"TiÕn Cóc §éng MËt Cung",},
		{124,1675,3418	,"C¸n Viªn §éng Mª Cung",},
		{152,1672,3361	,"TuyÕt B¸o §éng TÇng 8",},
--		{917,1816,3392	,"Ph¸ch HuyÕt Cèc",},
--		{918,1816,3392	,"¸c Nh©n Cèc",},
--		{919,1608,3168	,"Thùc Cèt Nhai",},
--		{920,1608,3168	,"H¾c Méc Nhai",},
--		{921,1560,3104	,"Thiªn Phô S¬n",},
--		{922,1560,3104	,"Bµn Long S¬n",},
--		{923,2008,4080	,"§Þa MÉu S¬n",},
--		{924,2008,4080	,"UyÓn Ph­îng S¬n",},
--		{949,1602,3199	,"Mª Cung KiÕm Gia",},
--		{950,1592,3195	,"¸c Lang Cèc",},
--		{325,1569,3086	,"½ð·½±¨Ãû´¦",},
--		{325,1541,3178	,"ËÎ·½±¨Ãû´¦",},
	}


function gopos_step2lv90(ns, ne)
	local n_count = getn(tab_lv90map);
	local tab_Content = {};
	for i = 1, 13 do
		tinsert(tab_Content, tab_lv90map[i][4].."/#gopos_step3lv90( "..i..")");
	end
	
	
	tinsert(tab_Content, "Hñy bá/no");
	Say("ThÇn Hµnh Phï, di chuyÓn ®Õn n¬i cÇn ®Õn.", getn(tab_Content), tab_Content);
end

function gopos_step3lv90(nIdx)
if GetLevel() < 90 then
Talk(1,"",15645)--phia truoc nguy hiem
return 1
end
	NewWorld(tab_lv90map[nIdx][1], tab_lv90map[nIdx][2], tab_lv90map[nIdx][3])
	SetFightState(1);
	Msg2Player("Ngåi cho ch¾c nhÐ! ta sÏ ®­a ng­¬i ®Õn <color=yellow>"..tab_lv90map[nIdx][4].."<color>.");
	SetProtectTime(18*3) --ÈýÃë±£»¤Ê±¼ä
	AddSkillState(963, 1, 0, 18*3)
end

tab_lv80map = {
		{224,1622,3118	,"Sa M¹c ®Þa biÓu",},
		{198,1521,2947	,"Thanh Khª §éng",},
		{320,1147,3123	,"Ch©n nói Tr­êng B¹ch",},
		{181,1425,2999	,"L­ìng Thñy §éng",},
		-- {201,1616,3195	,"B¨ng Hµ §éng",},
	}

function gopos_step2lv80(ns, ne)
	local n_count = getn(tab_lv80map);
	local tab_Content = {};
	for i = 1, 4 do
		tinsert(tab_Content, tab_lv80map[i][4].."/#gopos_step3lv80( "..i..")");
	end
	
	
	tinsert(tab_Content, "Hñy bá/no");
	Say("ThÇn Hµnh Phï, di chuyÓn ®Õn n¬i cÇn ®Õn.", getn(tab_Content), tab_Content);
end


function gopos_step3lv80(nIdx)
if GetLevel() < 80 then
Talk(1,"",15645)--phia truoc nguy hiem
return 1
end
	NewWorld(tab_lv80map[nIdx][1], tab_lv80map[nIdx][2], tab_lv80map[nIdx][3])
	SetFightState(1);
	Msg2Player("Ngåi yªn! Chóng ta ®i!"..tab_lv80map[nIdx][4].."!");
	SetProtectTime(18*3) --ÈýÃë±£»¤Ê±¼ä
	AddSkillState(963, 1, 0, 18*3)
end
---------------------map luyen cong 70----------------------------
tab_lv70map = {
		{319,1630,3587	,"L©m Du Quan",},
		{123,1702,3350	,"L·o Hæ §éng",},
		{206,1603,3215	,"TÇn L¨ng tÇng 2",},
		
	}

function gopos_step2lv70(ns, ne)
	local n_count = getn(tab_lv70map);
	local tab_Content = {};
	for i = 1, 3 do
		tinsert(tab_Content, tab_lv70map[i][4].."/#gopos_step3lv70( "..i..")");
	end
	
	
	tinsert(tab_Content, "Hñy bá/no");
	Say("ThÇn Hµnh Phï, di chuyÓn ®Õn n¬i cÇn ®Õn.", getn(tab_Content), tab_Content);
end


function gopos_step3lv70(nIdx)
if GetLevel() < 70 then
Talk(1,"",15645)--phia truoc nguy hiem
return 1
end
	NewWorld(tab_lv70map[nIdx][1], tab_lv70map[nIdx][2], tab_lv70map[nIdx][3])
	SetFightState(1);
	Msg2Player("Ngåi yªn! Chóng ta ®i!"..tab_lv70map[nIdx][4].."!");
	SetProtectTime(18*3) --ÈýÃë±£»¤Ê±¼ä
	AddSkillState(963, 1, 0, 18*3)
end
---------------------map luyen cong 60----------------------------
tab_lv60map = {
		{79,1600,3206	,"T­¬ng D­¬ng Nha M«n MËt §¹o",},
		{56,1516,3443	,"Hoµnh S¬n Ph¸i",},
		{166,1649,3231	,"Thiªn T©m Th¸p tÇng 3",},
		
	}

function gopos_step2lv60(ns, ne)
	local n_count = getn(tab_lv60map);
	local tab_Content = {};
	for i = 1, 3 do
		tinsert(tab_Content, tab_lv60map[i][4].."/#gopos_step3lv60( "..i..")");
	end
	
	
	tinsert(tab_Content, "Hñy bá/no");
	Say("ThÇn Hµnh Phï, di chuyÓn ®Õn n¬i cÇn ®Õn.", getn(tab_Content), tab_Content);
end


function gopos_step3lv60(nIdx)
if GetLevel() < 60 then
Talk(1,"",15645)--phia truoc nguy hiem
return 1
end
	NewWorld(tab_lv60map[nIdx][1], tab_lv60map[nIdx][2], tab_lv60map[nIdx][3])
	SetFightState(1);
	Msg2Player("Ngåi yªn! Chóng ta ®i!"..tab_lv60map[nIdx][4].."!");
	SetProtectTime(18*3) --ÈýÃë±£»¤Ê±¼ä
	AddSkillState(963, 1, 0, 18*3)
end
---------------------map luyen cong 50----------------------------
tab_lv50map = {
		{182,1777,2982	,"NghiÖt Long §éng",},
		{164,1611,3187	,"Thiªn T©m Th¸p",},
		
	}

function gopos_step2lv50(ns, ne)
	local n_count = getn(tab_lv50map);
	local tab_Content = {};
	for i = 1, 2 do
		tinsert(tab_Content, tab_lv50map[i][4].."/#gopos_step3lv50( "..i..")");
	end
	
	
	tinsert(tab_Content, "Hñy bá/no");
	Say("ThÇn Hµnh Phï, di chuyÓn ®Õn n¬i cÇn ®Õn.", getn(tab_Content), tab_Content);
end


function gopos_step3lv50(nIdx)
if GetLevel() < 50 then
Talk(1,"",15645)--phia truoc nguy hiem
return 1
end
	NewWorld(tab_lv50map[nIdx][1], tab_lv50map[nIdx][2], tab_lv50map[nIdx][3])
	SetFightState(1);
	Msg2Player("Ngåi yªn! Chóng ta ®i!"..tab_lv50map[nIdx][4].."!");
	SetProtectTime(18*3) --ÈýÃë±£»¤Ê±¼ä
	AddSkillState(963, 1, 0, 18*3)
end
---------------------map luyen cong 40----------------------------
tab_lv40map = {
		{21,2622,4502	,"Thanh Thµnh S¬n",},
		{167,1575,3239	,"§iÓm Th­¬ng S¬n",},

	}

function gopos_step2lv40(ns, ne)
	local n_count = getn(tab_lv40map);
	local tab_Content = {};
	for i = 1, 2 do
		tinsert(tab_Content, tab_lv40map[i][4].."/#gopos_step3lv40( "..i..")");
	end
	
	
	tinsert(tab_Content, "Hñy bá/no");
	Say("ThÇn Hµnh Phï, di chuyÓn ®Õn n¬i cÇn ®Õn.", getn(tab_Content), tab_Content);
end


function gopos_step3lv40(nIdx)
if GetLevel() < 40 then
Talk(1,"",15645)--phia truoc nguy hiem
return 1
end
	NewWorld(tab_lv40map[nIdx][1], tab_lv40map[nIdx][2], tab_lv40map[nIdx][3])
	SetFightState(1);
	Msg2Player("Ngåi yªn! Chóng ta ®i!"..tab_lv40map[nIdx][4].."!");
	SetProtectTime(18*3) --ÈýÃë±£»¤Ê±¼ä
	AddSkillState(963, 1, 0, 18*3)
end
---------------------map luyen cong 30----------------------------
tab_lv30map = {
		{193,1938,2845	,"Vò Di S¬n",},
		{170,1612,3187	,"Thæ PhØ §éng",},
	}

function gopos_step2lv30(ns, ne)
	local n_count = getn(tab_lv30map);
	local tab_Content = {};
	for i = 1, 2 do
		tinsert(tab_Content, tab_lv30map[i][4].."/#gopos_step3lv30( "..i..")");
	end
	
	
	tinsert(tab_Content, "Hñy bá/no");
	Say("ThÇn Hµnh Phï, di chuyÓn ®Õn n¬i cÇn ®Õn.", getn(tab_Content), tab_Content);
end


function gopos_step3lv30(nIdx)
if GetLevel() < 30 then
Talk(1,"",15645)--phia truoc nguy hiem
return 1
end
	NewWorld(tab_lv30map[nIdx][1], tab_lv30map[nIdx][2], tab_lv30map[nIdx][3])
	SetFightState(1);
	Msg2Player("Ngåi yªn! Chóng ta ®i!"..tab_lv30map[nIdx][4].."!");
	SetProtectTime(18*3) --ÈýÃë±£»¤Ê±¼ä
	AddSkillState(963, 1, 0, 18*3)
end
---------------------map luyen cong 20----------------------------
tab_lv20map = {
		{19,3102,3963	,"KiÕm C¸c T©y Nam",},
		{7,2276,2825	,"TÇn L¨ng tÇng 1",},

	}

function gopos_step2lv20(ns, ne)
	local n_count = getn(tab_lv20map);
	local tab_Content = {};
	for i = 1, 2 do
		tinsert(tab_Content, tab_lv20map[i][4].."/#gopos_step3lv20( "..i..")");
	end
	
	
	tinsert(tab_Content, "Hñy bá/no");
	Say("ThÇn Hµnh Phï, di chuyÓn ®Õn n¬i cÇn ®Õn.", getn(tab_Content), tab_Content);
end


function gopos_step3lv20(nIdx)
if GetLevel() < 20 then
Talk(1,"",15645)--phia truoc nguy hiem
return 1
end
	NewWorld(tab_lv20map[nIdx][1], tab_lv20map[nIdx][2], tab_lv20map[nIdx][3])
	SetFightState(1);
	Msg2Player("Ngåi yªn! Chóng ta ®i!"..tab_lv20map[nIdx][4].."!");
	SetProtectTime(18*3) --ÈýÃë±£»¤Ê±¼ä
	AddSkillState(963, 1, 0, 18*3)
end

function go_HSBattle()
	Msg2Player("Ngåi yªn chóng ta ®i Hoa S¬n")
	NewWorld(2,2605,3592)
	SetFightState(1)
	SetProtectTime(18*3)
	AddSkillState(963, 1, 0, 18*3)
end


function chondiadiem1()
	Say(SELECT_GO,6,
	SELECT_OUT,
	"Thµnh thÞ /thanhthi",
	"Th«n trang/thon",
	--"M«n ph¸i/monphai",
	"ChiÕn tr­êng Tèng Kim/tongkim",
--	"B¶n ®å luyÖn c«ng/luyencong1",
	"B¶n ®å luyÖn c«ng tõ 20 ®Õn 90/gotoluyencong",
	"§i Hoa S¬n (MiÔn phÝ)/go_HSBattle"
	)
end;


function thanhthi()
	local count = 1;
	local CityTab = {};
	
	for i=1,getn(THANH_ARRAY) do
		CityTab[count] = THANH_ARRAY[i][2] .."/chonthanh";
		count = count + 1;
	end
	if(count == 1) then
	return end;
	CityTab[count] = SELECT_OUT;
	Say(SELECT_GO,count,CityTab)
end;

function chonthanh(nNo)
	local nSel = nNo + 1;
	local count = 1;
	local CityPosTab = {};
	SetTaskTemp(SEL_MOVEARRAY,nSel);
	for i=1,getn(THANH_ARRAY[nSel][3]) do
		CityPosTab[count] = THANH_ARRAY[nSel][3][i][2] .."/posthanh";
		count = count + 1;
	end
	if(count == 1) then
	return end
	CityPosTab[count] = SELECT_OUT;
	Say(SELECT_GO,count,CityPosTab);
end;


function luyencong1()
	Say(SELECT_GO,getn(TRAIN_ARRAY1[1]),
	TRAIN_ARRAY1[1])
	
end

function luyencong2(nMapBegin)
	Say(SELECT_GO,getn(TRAIN_ARRAY2[1]),
	TRAIN_ARRAY2[1])
	
end

function selluyen1(sel)
	local nSel = sel + 1
	if(GetLevel() < TRAIN_ARRAY1[2][nSel][1]) then
	Talk(1,"",15645)--phia truoc nguy hiem
	return end
	if(NewWorld(TRAIN_ARRAY1[2][nSel][2],TRAIN_ARRAY1[2][nSel][3],
	TRAIN_ARRAY1[2][nSel][4]) ) then
		SetFightState(1) 		
		SetProtectTime(18*3)
		AddSkillState(963, 1, 0, 18*3)
	end
end

function selluyen2(sel)
	local nSel = sel + 1
	if(GetLevel() < TRAIN_ARRAY2[2][nSel][1]) then
	Talk(1,"",15645)--phia truoc nguy hiem
	return end
	if(getn(TRAIN_ARRAY2[2][nSel]) == 2) then
		local nW, nX, nY = GetWayPointPos(TRAIN_ARRAY2[2][nSel][2])
		--nFightState = GetWayPointFight(TRAIN_ARRAY2[2][nSel][2])
		nResult = NewWorld(nW, nX, nY)
		AddTermini(TRAIN_ARRAY2[2][nSel][2])
		--if (nResult == 0) then
		SetFightState(1) 		
		SetProtectTime(18*3)
		AddSkillState(963, 1, 0, 18*3)
		--end
	elseif (getn(TRAIN_ARRAY2[2][nSel]) == 4) then
		if(NewWorld(TRAIN_ARRAY2[2][nSel][2],TRAIN_ARRAY2[2][nSel][3],
		TRAIN_ARRAY2[2][nSel][4]) ) then
		SetFightState(1) 		
		SetProtectTime(18*3)
		AddSkillState(963, 1, 0, 18*3)
		end
	end
	--	SetFightState(1) 		
		--SetProtectTime(18*3)
		--AddSkillState(963, 1, 0, 18*3)
end





function divisondao()
	if(NewWorld(342, 1178, 2412) > 0) then
		SetFightState(0) 
	end
end

function quaylai()
	local w,x,y = GetWorldPos()
	--Msg2Player(w)
	--Msg2Player(x)
	--Msg2Player(y)
	ReturnFromPortal()
end

function ruong()
	if(GetFightState() == 1) then
		Talk(1,"",10196)
		return 
	end
	local w,x,y = GetWorldPos()
	local nRand
	if(w == 1) then --phuong tuong
		nRand = random(1,getn(RUONG_ARRAY[1]))
		SetPos(RUONG_ARRAY[1][nRand][1],RUONG_ARRAY[1][nRand][2])
	elseif(w == 11) then	--thanh do
		nRand = random(1,getn(RUONG_ARRAY[2]))
		SetPos(RUONG_ARRAY[2][nRand][1],RUONG_ARRAY[2][nRand][2])
	elseif(w == 162) then	--dai ly
		nRand = random(1,getn(RUONG_ARRAY[3]))
		SetPos(RUONG_ARRAY[3][nRand][1],RUONG_ARRAY[3][nRand][2])
	elseif(w == 37) then	--bien kinh
		nRand = random(1,getn(RUONG_ARRAY[4]))
		SetPos(RUONG_ARRAY[4][nRand][1],RUONG_ARRAY[4][nRand][2])
	elseif(w == 78) then	--tuong duong
		nRand = random(1,getn(RUONG_ARRAY[5]))
		SetPos(RUONG_ARRAY[5][nRand][1],RUONG_ARRAY[5][nRand][2])
	elseif(w == 80) then	--duong chau
		nRand = random(1,getn(RUONG_ARRAY[6]))
		SetPos(RUONG_ARRAY[6][nRand][1],RUONG_ARRAY[6][nRand][2])
	elseif(w == 176) then	--lam an
		nRand = random(1,getn(RUONG_ARRAY[7]))
		SetPos(RUONG_ARRAY[7][nRand][1],RUONG_ARRAY[7][nRand][2])
	else
		--Talk(1,"",10196)
		return
	end
end

function hieuthuoc()
	if(GetFightState() == 1) then
		Talk(1,"",10196)
		return
	end
	local w,x,y = GetWorldPos()
	if(w == 1) then --phuong tuong
		SetPos(THUOC_ARRAY[1][1],THUOC_ARRAY[1][2])
	elseif(w == 11) then	--thanh do
		SetPos(THUOC_ARRAY[2][1],THUOC_ARRAY[2][2])
	elseif(w == 162) then	--dai ly
		SetPos(THUOC_ARRAY[3][1],THUOC_ARRAY[3][2])
	elseif(w == 37) then	--bien kinh
		SetPos(THUOC_ARRAY[4][1],THUOC_ARRAY[4][2])
	elseif(w == 78) then	--tuong duong
		SetPos(THUOC_ARRAY[5][1],THUOC_ARRAY[5][2])
	elseif(w == 80) then	--duong chau
		SetPos(THUOC_ARRAY[6][1],THUOC_ARRAY[6][2])
	elseif(w == 176) then	--lam an
		SetPos(THUOC_ARRAY[7][1],THUOC_ARRAY[7][2])
	else
		Talk(1,"",10196)
		return 
	end
end

function taphoa()
	if(GetFightState() == 1) then
		Talk(1,"",10196)
		return 
	end
	local w,x,y = GetWorldPos()
	if(w == 1) then --phuong tuong
		SetPos(TAPHOA_ARRAY[1][1],TAPHOA_ARRAY[1][2])
	elseif(w == 11) then	--thanh do
		SetPos(TAPHOA_ARRAY[2][1],TAPHOA_ARRAY[2][2])
	elseif(w == 162) then	--dai ly
		SetPos(TAPHOA_ARRAY[3][1],TAPHOA_ARRAY[3][2])
	elseif(w == 37) then	--bien kinh
		SetPos(TAPHOA_ARRAY[4][1],TAPHOA_ARRAY[4][2])
	elseif(w == 78) then	--tuong duong
		SetPos(TAPHOA_ARRAY[5][1],TAPHOA_ARRAY[5][2])
	elseif(w == 80) then	--duong chau
		SetPos(TAPHOA_ARRAY[6][1],TAPHOA_ARRAY[6][2])
	elseif(w == 176) then	--lam an
		SetPos(TAPHOA_ARRAY[7][1],TAPHOA_ARRAY[7][2])
	else
		Talk(1,"",10196)
		return 
	end
end

function chondiadiem()
	SayEx({SELECT_GO,
	SELECT_OUT,
	"Thµnh thÞ /thanhthi",
	"Th«n trang/thon",
	--"M«n ph¸i/monphai",
	"ChiÕn tr­êng Tèng Kim/tongkim",
	"B¶n ®å luyÖn c«ng/luyencong1"})
end

function thanhthi()
	if (GetLevel() < 10) then
        Msg2Player("B¹n ph¶i ®¹t cÊp 10 míi cã thÓ ®i ®Õn Thµnh ThÞ ")
		return 
	end
	local count = 1
	local CityTab = {}
	for i=1,getn(THANH_ARRAY) do
		CityTab[count] = THANH_ARRAY[i][2] .."/chonthanh"
		count = count + 1
	end
	if(count == 1) then
		return 
	end
	CityTab[count] = SELECT_OUT
	Say(SELECT_GO, count, CityTab)
end

function chonthanh(nNo)
	local nSel = nNo + 1
	local count = 1
	local CityPosTab = {}
	SetTaskTemp(SEL_MOVEARRAY,nSel)
	--for i=1, 1 do--chän 1 vÞ trÝ trong thµnh
	for i=1, getn(THANH_ARRAY[nSel][3]) do--chän 4 h­íng trong thµnh
		CityPosTab[count] = THANH_ARRAY[nSel][3][count][2] .."/posthanh"
		count = count + 1
	end
	if(count == 1) then
		return 
	end
	CityPosTab[count] = SELECT_OUT
	Say(SELECT_GO, count, CityPosTab)
end

function posthanh(nNo)
	local nSel = nNo + 1
	local nMoveAR = GetTaskTemp(SEL_MOVEARRAY)
	local w,x,y = Rev2Pos(THANH_ARRAY[nMoveAR][1],THANH_ARRAY[nMoveAR][3][nSel][1])
	if(x ~= nil and x > 0) then
		if(NewWorld(w,floor(x/32),floor(y/32))) then
			SetFightState(0)
			Msg2Player("Xin h·y ngåi yªn, chóng ta ®i "..THANH_ARRAY[nMoveAR][3][nSel][2].." nµo!")
		end
	end
end

function GetRevPos(positon, mapid)
	x = THON_TT_MP_ARRAY[mapid][1]
	y = THON_TT_MP_ARRAY[mapid][2]
	x = x*32
	y = y*32
	return x, y
end

function thon()
	local count = 1
	local CityTab = {}
	for i=1,getn(THON_ARRAY) do
		CityTab[count] = THON_ARRAY[i][2] .."/chonthon"
		count = count + 1
	end
	if(count == 1) then
		return 
	end
	CityTab[count] = SELECT_OUT
	Say(SELECT_GO,count,CityTab)
end

function chonthon(nNo)
	local nSel = nNo + 1
	local x,y = GetRevPos(THON_ARRAY[nSel][3],THON_ARRAY[nSel][1])
	if(x ~= nil and x > 0) then
		if(NewWorld(THON_ARRAY[nSel][1],floor(x/32),floor(y/32))) then
			SetFightState(0)  
		end
	end
end

function monphai()
	--if(1 == 1) then
	--	Talk(1,"","Chøc n¨ng t¹m thêi ch­a më!")
	--	return
	--end
	local count = 1
	local CityTab = {}
	for i=1,getn(MONPHAI_ARRAY) do
		CityTab[count] = MONPHAI_ARRAY[i][2] .."/chonmonphai"
		count = count + 1
	end
	if(count == 1) then
		return 
	end
	CityTab[count] = SELECT_OUT
	Say(SELECT_GO,count,CityTab)
end

function chonmonphai(nNo)
	local nSel = nNo +1
	local x,y = GetRevPos(MONPHAI_ARRAY[nSel][3],MONPHAI_ARRAY[nSel][1])
	if(x ~= nil and x > 0) then
		if(NewWorld(MONPHAI_ARRAY[nSel][1],floor(x/32),floor(y/32)) > 0) then
			SetFightState(0) 
		end
	end
end

function tongkim()
	SayEx({PLACE_TOGO,
	"B¸o danh [phe Tèng]/ditongtc",
	"B¸o danh [phe Kim ]/dikimtc",	
	--"§µi So¸i bªn Tèng/namtongtuongquan",
	--"§µi So¸i bªn Kim/dikim",
	--"§iÓm Tèng Kim NgÉu Nhiªn/tongkimngaunhien",
	--"§iÓm b¸o danh bªn Tèng cò/ditongcu",
	--"§iÓm b¸o danh bªn Kim cò/dikimcu",
	SELECT_OUT})
end

function tongkimngaunhien()
	NewWorld(44, random(886, 1585), random(3215, 4499))
end

function namtongtuongquan()
	NewWorld(44, 1069, 4127)
end

function dikimcu()
	if(NewWorld(44, 1585, 3215) > 0) then SetFightState(0) end
end

function ditongcu()
	if(NewWorld(44, 886, 4499) > 0) then SetFightState(0)  end
end

function ditong()
	if(NewWorld(325, 1539, 3186) > 0) then SetFightState(0) end
end

function dikim()
	if(NewWorld(325,1581,3093) > 0) then SetFightState(0) end
end

function bando()
	SayEx({"Chän b¶n ®å: ",
	SELECT_OUT,
	"B¶n ®å luyÖn c«ng cÊp 20/bando2x",
	"B¶n ®å luyÖn c«ng cÊp 30/bando3x",
	"B¶n ®å luyÖn c«ng cÊp 40/bando4x",
	"B¶n ®å luyÖn c«ng cÊp 50/bando5x",
	"B¶n ®å luyÖn c«ng cÊp 60/bando6x",
	"B¶n ®å luyÖn c«ng cÊp 70/bando7x",
	"B¶n ®å luyÖn c«ng cÊp 80/bando8x",
	"B¶n ®å luyÖn c«ng cÊp 90/bando9x"})
end

function bando2x()
	if (GetLevel() < 10) then
        Msg2Player("B¹n ch­a ®ñ ®¼ng cÊp 10 ®Ó ®i ®Õn n¬i luyÖn c«ng cÊp 20")
		return
	end 
	local count = 1
	local CityTab = {}
	for i=1,getn(PLACES_ARRAY2X) do
		CityTab[count] = GetWayPointName(PLACES_ARRAY2X[i]).."/chonbando2x"
		count = count + 1
	end
	if(count == 1) then
		return 
	end
	CityTab[count] = SELECT_OUT
	Say(PLACE_TOGO, count, CityTab)
end

function bando3x()
	if (GetLevel() < 20) then
        Msg2Player("B¹n ch­a ®ñ ®¼ng cÊp 20  ®Ó ®i ®Õn n¬i luyÖn c«ng cÊp 30")
		return 
	end
	local count = 1
	local CityTab = {}
	for i=1,getn(PLACES_ARRAY3X) do
		CityTab[count] = GetWayPointName(PLACES_ARRAY3X[i]).."/chonbando3x"
		count = count + 1
	end
	if(count == 1) then
		return 
	end
	CityTab[count] = SELECT_OUT
	Say(PLACE_TOGO, count, CityTab)
end

function bando4x()
	if (GetLevel() < 30) then
        Msg2Player("B¹n ch­a ®ñ ®¼ng cÊp 30 ®Ó ®i ®Õn n¬i luyÖn c«ng cÊp 40")
		return
	end
	local count = 1
	local CityTab = {}
	for i=1,getn(PLACES_ARRAY4X) do
		CityTab[count] = GetWayPointName(PLACES_ARRAY4X[i]).."/chonbando4x"
		count = count + 1
	end
	if(count == 1) then
		return 
	end
	CityTab[count] = SELECT_OUT
	Say(PLACE_TOGO, count, CityTab)
end

function bando5x()
	if (GetLevel() < 40) then
        Msg2Player("B¹n ch­a ®ñ ®¼ng cÊp 40 ®Ó ®i ®Õn n¬i luyÖn c«ng cÊp 50")
		return 
	end
	local count = 1
	local CityTab = {}
	for i=1,getn(PLACES_ARRAY5X) do
		CityTab[count] = GetWayPointName(PLACES_ARRAY5X[i]).."/chonbando5x"
		count = count + 1
	end
	if(count == 1) then
		return 
	end
	CityTab[count] = SELECT_OUT
	Say(PLACE_TOGO, count, CityTab)
end

function bando6x()
	if (GetLevel() < 50) then
        Msg2Player("B¹n ch­a ®ñ ®¼ng cÊp 50 ®Ó ®i ®Õn n¬i luyÖn c«ng cÊp 60")
		return 
	end
	local count = 1
	local CityTab = {}
	for i=1,getn(PLACES_ARRAY6X) do
		CityTab[count] = GetWayPointName(PLACES_ARRAY6X[i]).."/chonbando6x"
		count = count + 1
	end
	if(count == 1) then
		return 
	end
	CityTab[count] = SELECT_OUT
	Say(PLACE_TOGO, count, CityTab)
end

function bando7x()
	if (GetLevel() < 60) then
		Msg2Player("B¹n ch­a ®ñ ®¼ng cÊp 60 ®Ó ®i ®Õn n¬i luyÖn c«ng cÊp 70")
		return 
	end
	local count = 1
	local CityTab = {}
	for i=1,getn(PLACES_ARRAY7X) do
		CityTab[count] = GetWayPointName(PLACES_ARRAY7X[i]).."/chonbando7x"
		count = count + 1
	end
	if(count == 1) then
		return 
	end
	CityTab[count] = SELECT_OUT
	Say(PLACE_TOGO, count, CityTab)
end

function bando8x()
	if (GetLevel() < 70) then
		Msg2Player("B¹n ch­a ®ñ ®¼ng cÊp 70 ®Ó ®i ®Õn n¬i luyÖn c«ng cÊp 80")
		return 
	end
	local count = 1
	local CityTab = {}
	for i=1,getn(PLACES_ARRAY8X) do
		CityTab[count] = GetWayPointName(PLACES_ARRAY8X[i]).."/chonbando8x"
		count = count + 1
	end
	if(count == 1) then
		return 
	end
	CityTab[count] = SELECT_OUT
	Say(PLACE_TOGO, count, CityTab)
end

function bando9x()
	if (GetLevel() < 80) then
		Msg2Player("B¹n ch­a ®ñ ®¼ng cÊp 80 ®Ó ®i ®Õn n¬i luyÖn c«ng cÊp 90")
		return 
	end
	local count = 1
	local CityTab = {}
	for i=1,getn(PLACES_ARRAY) do
		CityTab[count] = GetWayPointName(PLACES_ARRAY[i]).."/chonbando9x"
		count = count + 1
	end
	if(count == 1) then
		return 
	end
	CityTab[count] = SELECT_OUT
	Say(PLACE_TOGO,count,CityTab)
end

function chonbando2x(nNo)
	local nSel = nNo + 1
	local w,x,y = GetWayPointPos(PLACES_ARRAY2X[nSel])
	if(NewWorld(w,x,y) > 0) then
		SetFightState(1)
		SetProtectTime(18*3)
		AddSkillState(963, 1, 0, 18*3) 		
	end
end

function chonbando3x(nNo)
	local nSel = nNo + 1
	local w,x,y = GetWayPointPos(PLACES_ARRAY3X[nSel])
	if(NewWorld(w,x,y) > 0) then
		SetFightState(1) 		
		SetProtectTime(18*3)
		AddSkillState(963, 1, 0, 18*3) 				
	end
end

function chonbando4x(nNo)
	local nSel = nNo + 1
	local w,x,y = GetWayPointPos(PLACES_ARRAY4X[nSel])
	if(NewWorld(w,x,y) > 0) then
		SetFightState(1) 		
		SetProtectTime(18*3)
		AddSkillState(963, 1, 0, 18*3) 				
	end
end

function chonbando5x(nNo)
	local nSel = nNo + 1
	local w,x,y = GetWayPointPos(PLACES_ARRAY5X[nSel])
	if(NewWorld(w,x,y) > 0) then
		SetFightState(1) 		
		SetProtectTime(18*3)
		AddSkillState(963, 1, 0, 18*3) 				
	end
end

function chonbando6x(nNo)
	local nSel = nNo + 1
	local w,x,y = GetWayPointPos(PLACES_ARRAY6X[nSel])
	if(NewWorld(w,x,y) > 0) then
		SetFightState(1) 		
		SetProtectTime(18*3)
		AddSkillState(963, 1, 0, 18*3) 				
	end
end

function chonbando7x(nNo)
	local nSel = nNo + 1
	local w,x,y = GetWayPointPos(PLACES_ARRAY7X[nSel])
	if(NewWorld(w,x,y) > 0) then
		SetFightState(1) 		
		SetProtectTime(18*3)
		AddSkillState(963, 1, 0, 18*3) 				
	end
end

function chonbando8x(nNo)
	local nSel = nNo + 1
	local w,x,y = GetWayPointPos(PLACES_ARRAY8X[nSel])
	if(NewWorld(w,x,y) > 0) then
		SetFightState(1) 		
		SetProtectTime(18*3)
		AddSkillState(963, 1, 0, 18*3) 				
	end
end

function chonbando9x(nNo)
	local nSel = nNo + 1
	local w,x,y = GetWayPointPos(PLACES_ARRAY[nSel])
	if(NewWorld(w,x,y) > 0) then
		SetFightState(1) 		
		SetProtectTime(18*3)
		AddSkillState(963, 1, 0, 18*3)
	end
end

function diemphucsinh()
	Say(SELECT_REVIVE,3,
	SELECT_OUT,
	"Thµnh thÞ /psthanhthi",
	"Th«n trang/psthon"
	--"M«n ph¸i/psmonphai"
	)
end;

function psthanhthi()
	local count = 1
	local CityTab = {}
	for i=1,getn(THANH_ARRAY) do
		CityTab[count] = THANH_ARRAY[i][2] .."/pschonthanh"
		count = count + 1
	end
	if(count == 1) then
		return 
	end
	CityTab[count] = SELECT_OUT
	Say(SELECT_REVIVE, count, CityTab)
end

function pschonthanh(nNo)
	local nSel = nNo + 1
	local count = 1
	local CityPosTab = {}
	SetTaskTemp(SEL_MOVEARRAY,nSel)
	--for i=1,1 do --chØ cho mét vÞ trÝ trung t©m
	for i=1, getn(THANH_ARRAY[nSel][3]) do --c¸c vÞ trÝ ®«ng t©y nam b¾c trong thµnh
		CityPosTab[count] = THANH_ARRAY[nSel][3][i][2] .."/psposthanh"
		count = count + 1
	end
	if(count == 1) then
		return 
	end
	CityPosTab[count] = SELECT_OUT
	Say(SELECT_REVIVE, count, CityPosTab)
end

function psposthanh(nNo)
	local nSel = nNo + 1
	local nMoveAR = GetTaskTemp(SEL_MOVEARRAY)
	SetRevPos(THANH_ARRAY[nMoveAR][1],THANH_ARRAY[nMoveAR][3][nSel][1])
	Talk(1,"", format(SUCCESS,THANH_ARRAY[nMoveAR][3][nSel][2]))
end

function psthon()
	local count = 1
	local CityTab = {}
	for i=1,getn(THON_ARRAY) do
		CityTab[count] = THON_ARRAY[i][2] .."/pschonthon"
		count = count + 1
	end
	if(count == 1) then
		return 
	end
	CityTab[count] = SELECT_OUT
	Say(SELECT_REVIVE, count, CityTab)
end

function pschonthon(nNo)
	local nSel = nNo + 1
	SetRevPos(THON_ARRAY[nSel][1],THON_ARRAY[nSel][3])
	Talk(1,"", format(SUCCESS,THON_ARRAY[nSel][2]))
end

function psmonphai()
	--if(1 == 1) then
	--	Talk(1,"","Chøc n¨ng t¹m thêi ch­a më!")
	--	return 
	--end
	local count = 1
	local CityTab = {}
	for i=1,getn(MONPHAI_ARRAY) do
		CityTab[count] = MONPHAI_ARRAY[i][2] .."/pschonmonphai"
		count = count + 1
	end
	if(count == 1) then
		return 
	end
	CityTab[count] = SELECT_OUT
	Say(SELECT_REVIVE,count,CityTab)
end

function pschonmonphai(nNo)
	local nSel = nNo + 1
	SetRevPos(MONPHAI_ARRAY[nSel][1],MONPHAI_ARRAY[nSel][3])
	Talk(1,"", format(SUCCESS,MONPHAI_ARRAY[nSel][2]))
end

function no()
end
