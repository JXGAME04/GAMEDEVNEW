Include("\\script\\header\\taskid.lua")	-- [BOTBANG 02/09] TASK_DUNGCHUNG2 (4134) truoc day nil trong state script mon phai -> xuatsu() ghi nham task 0; dat TRUOC lib_task.lua de 3 hang trung ten (TASK_NVST/TASK_TANTHU/TASK_THOIGIAN9) giu gia tri cua lib_task nhu cu
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\header\\map_helper.lua")
--Include("\\script\\npckhac\\hoanhson\\doccokiem.lua");

FACTION_INFO = {
	{0, "ÉÙÁÖÅÉ", 1, 72, "Hoan nghªnh b¹n gia nhËp ThiÕu L©m, trë thµnh T¶o §Şa t¨ng! Häc ®­îc Hµng Long BÊt Vò, Kim Cang Phôc Ma ", "Gia NhËp ThiÕu L©m ph¸i, trë thµnh T¶o §Şa t¨ng"},
	{0, "ÌìÍõ°ï", 3, 79, "Hoan nghªnh b¹n gia nhËp Thiªn V­¬ng bang! H·y khëi ®Çu tõ mét ng­êi ThŞ vÖ!", "Gia nhËp Thiªn V­¬ng Bang, trë thµnh ThŞ VÖ"},
	{1, "ÌÆÃÅ", 3, 76, "Hoan nghªnh b¹n gia nhËp §­êng m«, trë thµnh T¹p dŞch! Häc ®­îc vâ c«ng Tİch LŞch §¬n", "Gia nhËp §­êng M«n, trë thµnh T¹p dŞch"},
	{1, "Îå¶¾½Ì", 2, 80, "Hoan nghªnh b¹n Gia nhËp Ngò §éc gi¸o, trë thµnh Ngò §éc §ång Tö! Häc ®­îc §éc Sa Ch­ëng, HuyÕt §ao §éc S¸t.", "Gia nhËp Ngò §éc gi¸o, trë thµnh Ngò §éc §ång Tö"},
	{2, "¶ëáÒÅÉ", 1, 74, "Hoan nghªnh b¹n gia nhËp Nga Mi ph¸i, trë thµnh Bè Y Ni. Häc ®­îc NhÊt DiÖp Tri Thu, Phiªu TuyÕt Xuyªn V©n", "Gia nhËp Nga Mi Ph¸i, trë thµnh Bè Y Ni"},
	{2, "´äÑÌÃÅ", 3, 77, "Hoan nghªnh b¹n gia nhËp Thóy Yªn m«n! Trë thµnh Hoa Tú. Häc ®­îc Phong Hoa TuyÕt NguyÖt, Phong QuyÓn Tµn TuyÕt", "Gia nhËp Thóy Yªn m«n, trë thµnh Hoa Tú"},
	{3, "Ø¤°ï", 1, 78, "Hoan nghªnh b¹n gia nhËp C¸i bang, trë thµnh §Ö tö kh«ng tói! Häc ®­îc KiÕn Nh©n ThÇn Thñ, Diªn M«n Th¸c B¸t", "Gia nhËp C¸i bang, trë thµnh §Ö tö kh«ng tói"},
	{3, "ÌìÈÌ½Ì", 2, 81, "Hoan nghªnh b¹n gia nhËp Thiªn nhÉn gi¸o, Häc ®­îc Tµn D­¬ng Nh­ HuyÕt, §¬n ChØ LiÖt DiÖm", "Gia nhËp Thiªn nhÉn gi¸o."},
	{4, "Îäµ±ÅÉ", 1, 73, "Hoan nghªn b¹n gia nhËp Vâ §ang ph¸i! H·y b¾t ®Çu tõ mét §¹o ®ång! Häc ®­îc vâ c«ng Né L«i ChØ, Th­¬ng h¶i Minh NguyÖt!", "Gia nhËp Vâ §ang ph¸i, trë thµnh §¹o ®ång"},
	{4, "À¥ÂØÅÉ", 3, 75, "Hoan nghªnh b¹n gia nhËp C«n L«n ph¸i! trë thµnh PhÊt trÇn ®Ö tö! Häc ®­îc H« Phong Ph¸p, Cuång L«i ChÊn §Şa", "Gia nhËp C«n L«n ph¸i, trë thµnh PhÊt TrÇn ®Ö tö"},
	{2, "»ªÉ½ÅÉ", 3, 89, "Hoan nghªnh b¹n gia nhËp Hoa S¬n ph¸i trë thµnh Th­ §ång! Häc ®­îc vâ c«ng B¹ch Hång Qu¸n NhËt, Thanh Phong Tèng S¶ng", "Gia nhËp Hoa S¬n ph¸i, trë thµnh Th­ §ång"},	-- [HOASON 01/09] Hoa Son id 10
	{3, "Îä»êÌÃ", 1, 90, "Hoan nghªnh b¹n gia nhËp Vò Hån §­êng, trë thµnh HËu Qu©n! Häc ®­îc vâ c«ng Vò Hån ThuÉn Ph¸p, Vò Hån §ao Ph¸p", "Gia nhËp Vò Hån §­êng"},	-- [VHTD 02/09] Vu Hon id 11 (he Hoa, chinh phai, rank 90 Hau Quan - client VLTK faction_settings)
	{4, "åĞÒ£ÅÉ", 3, 98, "Hoan nghªnh b¹n gia nhËp Tiªu Dao ph¸i, trë thµnh Tiªu Dao C­ SÜ! Häc ®­îc vâ c«ng Tiªu Dao KiÕm Ph¸p, Tiªu Dao CÇm Ph¸p", "Gia nhËp Tiªu Dao ph¸i"},	-- [VHTD 02/09] Tieu Dao id 12 (he Tho, trung lap, rank 98 Tieu Dao Cu Si)
}

function gianhapmonphai(newfac)
	local nCurFac = newfac+1;
if check_yes(FACTION_INFO[nCurFac][1]) == 1 then
	SetFaction(FACTION_INFO[nCurFac][2])
	SetCamp(FACTION_INFO[nCurFac][3])
	SetCurCamp(FACTION_INFO[nCurFac][3])
	SetRank(FACTION_INFO[nCurFac][4])
	hockynang(nCurFac)
	Msg2Player(FACTION_INFO[nCurFac][5])
	--Msg2Faction(""..GetName().." tõ h«m nay gia nhËp "..GetFactionName()..", xin b¸i kiÕn c¸c vŞ huynh tû! Mong c¸c vŞ quan t©m chØ gi¸o!") 
end
end;

function xuatsu()
	SetCamp(4);
	SetCurCamp(4);
	LeaveTeam();
	SetTask(TASK_DUNGCHUNG2,1);
	--Msg2Faction(""..GetFactionName().." "..GetName().." thµnh tµi xuÊt s­, b¸i biÖt ch­ vŞ ®ång m«n phiªu b¹t giang hå ") 
end;

function sr2name(series)
	local nSeries = 0;
	if (series==nil) then
		nSeries = GetSeries();
	else
		nSeries = series;
	end;
	if (nSeries == 0) then
		return "<color=Metal>hÖ Kim<color>";
	elseif (nSeries == 1) then
		return "<color=Wood>hÖ Méc<color>";
	elseif (nSeries == 2) then
		return "<color=Water>hÖ Thñy<color>";
	elseif (nSeries == 3) then
		return "<color=Fire>hÖ Háa<color>";
	elseif (nSeries == 4) then
		return "<color=Earth>hÖ Thæ<color>";
	else
		return "<color=White>V« HÖ<color>";
	end;
end

function hotrokn()
	local nCurFac = GetFactionNo();
	if(nCurFac < 0 or nCurFac > 12) then	-- [HOASON 01/09] 13 phai
	Msg2Player("B¹n ch­a gia nhËp m«n ph¸i!")
	return end
	local nTaskVal = GetTask(TASK_TANTHU6);
	if(GetNumber(1,nTaskVal,7) > 0) then
	Talk(1, "", 15278)
	return end
	-- SetTask(TASK_TANTHU, SetNumber(1,nTaskVal,7,1));
	show_kynang90(nCurFac+1)
end;


function trungphansumon()
	if GetTongFlag() > 0 then
		Talk(1,"", "Thµnh viªn Bang héi kh«ng thÓ trïng ph¶n s­ m«n")	
		return 0;
	end
	local nCurFac=GetFactionNo();
	if(nCurFac==-1) then return end;
	LeaveTeam();
	SetCamp(FACTION_INFO[nCurFac+1][3]);
	SetCurCamp(FACTION_INFO[nCurFac+1][3]);
	SetTask(TASK_DUNGCHUNG2,0);
end;

function check_yes(series)			-- °´¼¼ÄÜÈ·¶¨ÈÎÎñ±äÁ¿µÄ¼ì²é¡£¡£¡£ÉĞÎ´ÏàÓ¦¸üĞÂ
	if (GetSeries() ~= series) then
		Talk(1,"","Ngò hµnh cña ng­¬i kh¸c ngò hµnh "..sr2name(series).." víi bän ta!  §Õn ®©y lµm g×? H·y lªn diÔn ®µn häc hái ®i!")
		return 0;
	end
	return 1;
end;

function hockynang(nCurFac)
	for i=1,getn(SKILLNORMAL[nCurFac]) do
    AddMagic(SKILLNORMAL[nCurFac][i][1],SKILLNORMAL[nCurFac][i][2])
	end
end;

function show_kynang90(nCurFac)
	local nTarFac = GetFactionNo()+1;
	if(nCurFac ~= nil) then
		nTarFac = nCurFac;
	end;
	local nskillcount = getn(SKILL90_ARRAY[nTarFac])
	if(GetLevel() < 80) then
		Talk(1,"",14576)
	return end
	local skilltab = {};
	local count = 1;
	for i=1,nskillcount do
		skilltab[count] = format(SKILL90_ARRAY[nTarFac][i][1].."/selskill(%d,%d)", i, nTarFac);
		count = count + 1;
	end
	if(count == 1) then
	return end
	skilltab[count] = "§ãng/no";
	Say("Ng­¬i muèn häc kü n¨ng nµo:",count,skilltab);
end;

function show_kynang150(nCurFac)
	local nTarFac = GetFactionNo()+1;
	if(nCurFac ~= nil) then
		nTarFac = nCurFac;
	end;
	local nskillcount = getn(SKILL150_ARRAY[nTarFac])
	if(GetLevel() < 80) then
		Talk(1,"",14576)
	return end
	local skilltab = {};
	local count = 1;
	for i=1,nskillcount do
		skilltab[count] = format(SKILL150_ARRAY[nTarFac][i][1].."/selskill(%d,%d)", i, nTarFac);
		count = count + 1;
	end
	if(count == 1) then
	return end
	skilltab[count] = "§ãng/no";
	Say("Ng­¬i muèn häc kü n¨ng nµo:",count,skilltab);
	
end;

function selskill(sel, nCurFac)
	if(HaveMagic(SKILL90_ARRAY[nCurFac][sel][2]) ~= -1) then
			Talk(1,"",15467)
	return end
	AddMagic(SKILL90_ARRAY[nCurFac][sel][2],SKILL90_ARRAY[nCurFac][sel][3]);
	local str = "B¹n ®· häc ®­îc kü n¨ng "..SKILL90_ARRAY[nCurFac][sel][1];
	Msg2Player(str);
	SetTask(TASK_TANTHU6, SetNumber(1,nTaskVal,7,1));
end;

function no()
end;

SKILLNORMAL = {
[1]={--thieu lam
	{14	, 0}, ---Han Long Bat Vu
    {10	, 0}, --- Kim Cang Phuc Ma
    {4	, 0}, --- Thieu Lam Con Phap
    {6	, 0}, --- Thieu Lam Dao Phap
    {8	, 0}, --- Thieu Lam Quyen Phap
    {15	, 0}, --- Bat Dong Minh Vuong
    {16	, 0}, --- La Han Tran
    {20	, 0}, --- Su Tu Hong
    {271, 0}, --- Long Trao ho trao
    {11	, 0}, --- Hoanh Tao Luc Hop
    {19	, 0}, --- Ma Ha Vo Luong
    {21	, 0}, --- Dich Can Kinh
    {273, 0} --- Nhu Lai Thien Diep-Tran phai
},
[2]={--thien vuong
    {34, 0}, --- Kinh loi Tram
    {30, 0}, --- Hoi Phong Lac Nhan
    {29, 0}, --- Tram Long Quyet
    {24, 0}, --- Thien Vuong Dao phap
    {23, 0}, --- Thien Vuong Thuong phap
    {26, 0}, ---Thien Vuong Chuy phap
    {33, 0}, ---Tinh Tam Quyet
    {37, 0}, ---Bat Phong Tram
    {35, 0}, ---Duong Quang Tam Diep
    {31, 0}, ---Hang Van Quyet
    {40, 0}, ---Doan Hon Thich
    {42, 0}, ---Kim Chung Trao
    {32, 0}, --- Vo Tam Tram
    {41, 0}, --- Huyet Chien Bat Phuong
    {324, 0}, ---Thua Long Quyet
    {36	, 0} ---Thien Vuong Chien Y -Tran phai
},
[3]={--duong mon
    {45	, 0}, ---Phich Lich Don
    {347	, 0}, ---Dia Diem Hoa
    {43	, 0}, ---Duong Mon Am Khi
    {303	, 0}, ---Doc Thich Cot
    {50	, 0}, ---Truy Tam Tien
    {47	, 0}, ---Doat Hon Tieu
    {54	, 0}, ---Man Thien Hoa Vu
    {343	, 0}, ---Xuyen Tam Thich
    {345	, 0}, ---Han Bang Thich
    {349	, 0}, ---Loi Kich Thuat
    {249	, 0}, ---Tieu Ly Phi Dao
    {341	, 0}, ---Tan Hoa Tieu
    {58	, 0}, ---Thien La Dia Vong
    {48	, 0} ---Tam Nhan - tran phai
},
[4]={--ngu doc
    {63, 0}, ---Doc Sa Chuong
    {65, 0}, ---Huyet Dao Doc Sat
    {62, 0}, ---Ngu Doc Chuong Phap
    {60, 0}, ---Ngu Doc Dao phap
    {67, 0}, ---Cuu Thien Cuong Loi
    {70, 0}, ---Xich Diem Thuc Thien
    {66, 0}, ---Tap Nan Duoc kinh
    {68, 0}, ---U Minh Kho Lau
    {384,0}, ---Bach Doc Xuyen Tam
    {64	, 0}, ---Bang Lam Huyen Tinh
    {69	, 0}, ---Vo Hinh Doc
    {356	, 0}, ---Xuyen Y Pha Giap
    {73	, 0}, ---Van Co Thuc Tam
    {72	, 0}, ---Xuyen Tam Doc Thich
    {71	, 0}, ---Thien Cuong Dia Sat
    {74	, 0}, ---Chu Cap Thanh Minh
    {75	, 0} ---Ngu Doc Ky Kinh - tran phai
},
[5]={--nga my
    {85, 0}, ---Nhat Diep Tri Thu
    {80, 0}, --- Phieu Tuyet Xuyen Van
    {77, 0}, ---Nga My Kiem Phap
    {79, 0}, ---Nga My Chuong Phap
    {93, 0}, ---Tu Hang Pho Do
    {385,0}, ---Thoi Song Vong Nguyet
    {82, 0}, ---Tu Tuong Dong Quy
    {89, 0}, ---Mong Diep
    {86, 0}, ---Luu Thuy
    {92, 0}, ---Phat Tam Tu Huu
    {88, 0}, --- Bat Diet Bat Tuyet
    {91, 0}, --- Phat Quang Pho Chieu
    {282,0}, ---Thanh Am Phan Xuong
    {252,0} ---Phat Phap Vo Bien - tran phai
},
[6]={--thuy yen
    {99	, 0}, -- Phong Hoa Tuyet Nguyet
    {102	, 0}, -- Phong Quyen Tan Tuyet
    {95	, 0}, -- Thuy Yen Dao Phap
    {97	, 0}, -- Thuy Yen Song Dao
    {269	, 0}, -- Bang Tam Trai Anh
    {105	, 0}, -- Vu Da Le Hoa
    {113	, 0}, -- Phu Van Tan Tuyet
    {100	, 0},	-- Ho The Han Bang
    {109	, 0}, -- Tuyet Anh
    {108	, 0},	-- Muc Da Luu Tinh
    {111	, 0}, -- Bich Hai Trieu Sinh
    {114	, 0} -- Bang Cot Tuyet Tam - tran phai
},
[7]={--cai bang
    {122,0}, ---Kien Nhan Than Thu
    {119,0}, ---Dieu mon Thac Bat
    {116,0}, ---Cai Bang Chuong Phap
    {115,0}, ---Cai Bang Bong Phap
    {129,0}, ---Hoa Hiem Vi Di
    {274,0}, ---Giang Long Chuong
    {124,0}, ---Da Cau Tran (Da cau bong phap)
    {277,0}, ---Hoat Bat Luu Thu
    {128,0}, ---Khang Long Huu Hoi
    {125,0}, ---Bong Da Ac Cau
    {360,0}, ---Tieu Dieu Cong
    {130,0} ---Tuy Diep Cuong Vu -tran phai
},
[8]={--thien nhan
    {135,0}, ---Tan Duong Nhu Huyet
    {145,0}, ---Don Chi Liet Diem
    {132,0}, ---Thien Nhan Mau Phap
	{131,0}, ---Thien Nhan Dao Phap
	{136,0}, ---Hoa lien Phan Hoa
	{137,0}, ---Ao Anh Phi Ho
	{141,0}, ---Liet Hoa Tinh Thien
	{138,0}, ---Thoi Son Dien Hai
	{140,0}, ---Phi Hong Vo Tich
	{364,0}, ---Bi To Thanh Phong
	{143,0}, ---Lich Ma Doat Hon
	{142,0}, ---Thau Thien Hoan Nhat
	{148,0}, ---Ma Diem That Sat
	{150,0} ---Thien Ma Giai The -tran phai
},
[9]={--vo dang
	{153,0}, ---No Loi Chi
	{155,0}, ---Thuong Hai Minh Nguyet
	{152,0}, ---Vo Dang Quyen Phap
	{151,0},---Vo Dang Kiem Phap
	{159,0},---That Tinh Tran
	{164,0},---Bac Cap Nhi Phuc
	{158,0},---Kiem Phi Kinh Thien
	{160,0},---The Van Tung
	{157,0},---Toa Vong Vo Nga
	{165,0},---Vo Nga Vo Kiem
	{267,0},---Tam Hoan Thao Nguyet
	{166,0}---Thai Cuc Than Cong - tran phai
},
[10]={--con lon
	{169,0}, ---Ho Phong Phap
	{179,0},---Cuong Loi Chan Dia
	{167,0},---Con Lon Dao Phap
	{168,0},---Con Lon Kiem Phap
	{392,0},---Thuc Phuoc Chu
	{171,0},---Thanh Phong Phu
	{174,0},---Ki Ban Phu
	{178,0},---Nhat Khi Tam Thanh
	{172,0},---Thien Te Tan Loi
	{393,0},---Bac Minh Dao Hai
	{173,0},---Thien Thanh Dia Troc
	{175,0},---Khi Han Ngao Tuyet
	{181,0},---Khi Tam Phu
	{176,0},---Cuong Phong Sau Dien
	{90,  0},---Me Tung Ao Anh
	{182,0},---Ngu Loi Chanh Phap
	{275,0},---Suong Ngao Con Luan - tran phai
	{630,0}---Huyen Thien Vo cuc
},
[11]={--hoa son [HOASON 01/09] (Linux add_hs 10..70)
	{1347,0},---Bach Hong Quan Nhat
	{1372,0},---Thanh Phong Tong Sang
	{1349,0},---Kiem Tong Tong Quyet
	{1374,0},---Long Nhieu Than
	{1350,0},---Duong Ngo Kiem Phap
	{1375,0},---Hai Nap Bach Xuyen
	{1351,0},---Kim Nhan Hoanh Khong
	{1376,0},---Long Huyen Kiem Khi
	{1354,0},---Hi Di Kiem Phap
	{1378,0},---Khi Chan Son Ha
	{1355,0},---Thien Than Dao Huyen
	{1379,0},---Khi Quan Truong Hong
	{1358,0},---Huyen Nhan Van Yen - tran phai
	{1360,0},---Thuong Tung Nghenh Khach
	{1380,0}---Ma Van Kiem Khi
},
[12]={--vu hon [VHTD 02/09] (client VLTK wuhuntang.lua tbSkillID 10..70)
	{1972,0},---Doat co
	{1974,0},---Truong Anh Huy Xich
	{1964,0},---Vu Hon Thuan Phap
	{1975,0},---Vu Hon Dao Phap
	{1976,0},---Cong Trung Binh Tinh
	{1965,0},---Thinh Anh De Nhue Lu
	{1977,0},---Ham Son Kich
	{1963,0},---Trieu Thien Khuyet
	{1979,0},---No Phach Thien Nhai
	{1980,0},---Co Tuong Bach Chien Ham
	{1982,0},---Vu Muc Di Thu
	{1971,0},---Tri Dung Sieu Luan
	{1981,0}---Co Xan Lo Nhuc
},
[13]={--tieu dao [VHTD 02/09] (client VLTK xiaoyao.lua tbSkillID 10..70)
	{2114,0},---Ngo Cau Suong Tuyet
	{2136,0},---Tuy y Khuc
	{2135,1},---Cong kich vat ly cam (don thuong Moc Cam, EqtLimit 103) [VHTD 02/09g]
	{2115,0},---Tieu Dao Kiem Phap
	{2137,0},---Tieu Dao Cam Phap
	{2116,0},---Di Cung Hoan Vu
	{2118,0},---Tap Dap Luu Tinh
	{2138,0},---Lac Nhan Binh Sa
	{2120,0},---y Kiem Phat Thien
	{2140,0},---Quang Lang Tan
	{2121,0},---Ngan Yen Bach Ma
	{2123,0},---Thien Ly Doc Hanh
	{2122,0}---Hiep Cot Nhu Tinh
}
};

SKILL90_ARRAY={
[1]={
	{"§¹t Ma §é Giang",318,1},
	{"Hoµnh T¶o Thiªn Qu©n",319,1},
	{"V« T­íng Tr¶m",321,1}
},
[2]={
	{"Ph¸ Thiªn Tr¶m",322,1},
	{"Truy Tinh Trôc NguyÖt",323,1},
	{"Truy Phong QuyÕt",325,1}
},
[3]={
	{"NhiÕp Hån NguyÖt ¶nh",339,1},
	{"Cöu Cung Phi Tinh",342,1},
	{"B¹o Vò Lª Hoa",302,1},
	{"Lo¹n Hoµn Kİch",351,0}
},
[4]={
	{"¢m Phong Thùc Cèt",353,1},
	{"HuyÒn ¢m Tr¶m",355,1},
	{"§o¹n C©n Hñ Cèt",390,0}
},
[5]={
	{"Tam Nga TÒ TuyÕt",328,1},
	{"Phong S­¬ng To¸i ¶nh",380,1},
	{"Phæ §é Chóng Sinh",332,0}
},
[6]={
	{"B¨ng Tung V« ¶nh",336,1},
	{"B¨ng T©m Tiªn Tö",337,1}
},
[7]={
	{"Phi Long T¹i Thiªn",357,1},
	{"Thiªn H¹ V« CÈu",359,1}
},
[8]={
	{"V©n Long Kİch",361,1},
	{"Thiªn Ngo¹i L­u Tinh",362,1},
	{"NhiÕp Hån Lo¹n T©m",391,0}
},
[9]={
	{"Thiªn §Şa V« Cùc",365,1},
	{"Nh©n KiÕm Hîp NhÊt",368,1}
},
[10]={
	{"Ng¹o TuyÕt Tiªu Phong",372,1},
	{"L«i §éng Cöu Thiªn",375,1},
	{"Tóy Tiªn T¸ Cèt",394,0}
},
[11]={
	{"§o¹t MÖnh Liªn Hoµn Tam Tiªn KiÕm",1364,1},
	{"Ph¸ch Th¹ch Ph¸ Ngäc",1382,1}
},
[12]={ -- [VHTD 02/09]
	{"TrÊn Biªn Thïy",1967,1},
	{"Kh¸t Èm Hung N« HuyÕt",1983,1}
},
[13]={ -- [VHTD 02/09]
	{"KiÕm QuyÕt Phï V©n",2124,1},
	{"Cao S¬n L­u Thñy",2141,1}
}
};

SKILL120AR=
{
	709,708,710,711,712,713,714,715,716,717,1365,1984,2127 -- 1365 = Tu Ha Kiem Khi (Hoa Son); 1984 Vu Hon, 2127 Tieu Dao [VHTD 02/09]
};

SKILL150_ARRAY={
[1]={
	{"§¹i Lùc Kim Cang Ch­ëng",1055,1},
	{"Vi §µ HiÕn Xö",1056,1},
	{"Tam Giíi Quy ThiÒn",1057,1}
},
[2]={
	{"Hµo Hïng Tr¶m",1058,1},
	{"Tung Hoµnh B¸t Hoang",1059,1},
	{"B¸ V­¬ng T¹m Kim",1060,1}
},
[3]={
	{"V« ¶nh Xuyªn",1069,1},
	{"ThiÕt Liªn Tø S¸t",1070,1},
	{"Cµn Kh«n NhÊt TrŞch",1071,1},
	{"Tİch LŞch Lo¹n Hoµn Kİch",1110,0}
},
[4]={
	{"H×nh Tiªu Cèt LËp",1066,1},
	{"U Hån PhÖ ¶nh",1067,1},
},
[5]={
	{"KiÕm Hoa V·n Tinh",1061,1},
	{"B¨ng Vò L¹c Tinh",1062,1},
},
[6]={
	{"B¨ng T­íc Ho¹t Kú",1063,1},
	{"B¨ng Ng­ng Hµn Yªn",1065,1}
},
[7]={
	{"Thêi Thõa Lôc Long",1073,1},
	{"Bæng Huınh L­îc §Şa",1074,1}
},
[8]={
	{"Giang H¶i Né Lan",1075,1},
	{"TËt Háa LiÖu Nguyªn",1076,1},
},
[9]={
	{"T¹o Hãa Th¸i Thanh",1078,1},
	{"KiÕm Thïy Tinh Hµ",1079,1}
},
[10]={
	{"Cöu Thiªn C­¬ng Phong",1080,1},
	{"Thiªn L«i ChÊn Nh¹c",1081,1},
},
[11]={
	{"Cöu KiÕm Hîp NhÊt",1369,1},
	{"ThÇn Quang Toµn NhiÔu",1384,1}
},
[12]={ -- [VHTD 02/09]
	{"Huy S­ DiÖt Lç",1969,1},
	{"Trõ Gian DiÖt NŞnh",1985,1}
},
[13]={ -- [VHTD 02/09]
	{"ThËp Bé NhÊt S¸t",2129,1},
	{"Mai Hoa Tam Léng",2142,1}
}
};

FACTION_PROP={
{35,25,25,15},
{20,35,20,25},
{25,25,25,25},
{30,20,30,20},
{20,15,25,40}
};

SERIES_BASE={
{16,204,100},
{77,103,100},
{77,153,100},
{41,213,100},
{163,76,100}
};