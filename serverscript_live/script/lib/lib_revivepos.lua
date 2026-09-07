-- ================================================================================================
-- [HE THONG] script/lib/lib_revivepos.lua
-- Muc dich  : DIEM HOI SINH theo map (du lieu).
-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng)
-- Ham       : (khong co - tep du lieu/cau hinh)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- Author: LEERIM
-- Date: 28/11/2016
-- Lib: Diem phuc sinh toan server
THANH_ARRAY = {
	{15,	"Ph­îng T­êng phñ "	,
		{
			{51104,102592,"Ph­îng T­êng trung t©m",7},
			{52448,104704,"Ph­îng T­êng §«ng",3},
			{50304,100032,"Ph­îng T­êng T©y",4},
			{49376,103264,"Ph­îng T­êng Nam",5},
			{53184,101024,"Ph­îng T­êng B¾c",6}
		}
	},
--	{11 ,	"Thµnh §« phñ "	,
--		{
--			{100608,162336,"Thµnh §« trung t©m",30},
--			{101824,165792,"Thµnh §« §«ng",26},
--			{97504,159456,"Thµnh §« T©y",27},
--			{96160,163392,"Thµnh §« Nam",28},
--			{103840,160896,"Thµnh §« B¾c",29}
--		}
--	},
--	{162 ,	"§¹i Lý phñ "	,
--		{
--			{63 ,"§¹i Lý trung t©m"},
--			{64	,"§¹i Lý B¾c"}
--		}
--	},
--	{37 ,	"BiÖn Kinh phñ "	,
--		{
--			{55232,99200,"BiÖn Kinh trung t©m",18},
--			{54272,102976,"BiÖn Kinh §«ng",15},
--			{51904,96512,"BiÖn Kinh T©y",16},
--			{59392,94912,"BiÖn Kinh B¾c",17}
--		}
--	},
	{78 ,	"T­¬ng D­¬ng phñ "	,
		{
			{50464,103616,"T­¬ng D­¬ng trung t©m",13},
			{50336,107424,"T­¬ng D­¬ng §«ng",9},
			{47168,101664,"T­¬ng D­¬ng T©y",10},
			{46528,104640,"T­¬ng D­¬ng Nam",11},
			{54528,103552,"T­¬ng D­¬ng B¾c",12}
		}
	},
	{80 ,	"D­¬ng Ch©u phñ "	,
		{
			{56256,96960,"D­¬ng Ch©u trung t©m",24},
			{54912,103456,"D­¬ng Ch©u §«ng",20},
			{53024,96800,"D­¬ng Ch©u T©y",23},
			{51776,101824,"D­¬ng Ch©u Nam",22},
			{59648,97152,"D­¬ng Ch©u B¾c",21}

		}
	},
--	{16 ,	"L©m An phñ ",
--		{
--			{44640,106176,"L©m An Nam"},
--			{53920,105120,"L©m An §«ng"},
--			{50208,94016,"L©m An B¾c"}
--		}
--	}
};
--diem phuc sinh thon tran
THON_ARRAY = {
--{20,110656,197888,	"Giang T©n Th«n" ,1 },
--{121,64000,143200,	"Long M«n TrÊn"	 ,1 },
{53,52032,101696 ,	"Ba L¨ng HuyÖn"	 ,0 }
--{54 			 ,	"Nam Nh¹c TrÊn"	 ,17},
--{174 			 ,	"Long TuyÒn Th«n",66},
--{101 			 ,	"§¹o H­¬ng Th«n" ,47},
--{99 			 ,	"VÜnh L¹c TrÊn"	 ,43},
--{100 			 ,	"Chu Tiªn TrÊn"	 ,45},
--{153 			 ,	"Th¹ch Cæ TrÊn"	 ,59},
--{175 			 ,	"T©y S¬n Th«n"	 ,1 },
};
--diem phuc sinh mon phai
MONPHAI_ARRAY = {
{103	,	"ThiÕu L©m ph¸i"		 ,52},
{59		,	"Thiªn V­¬ng bang"		 ,21},
{25		,	"§­êng M«n"				 ,15},
{183	,	"Ngò §éc gi¸o"			 ,71},
{13		,	"Nga My ph¸i"			 ,13},
{154	,	"Thóy Yªn m«n"			 ,61},
{115	,	"C¸i Bang"				 ,53},
{49		,	"Thiªn NhÉn gi¸o"		 ,28},
{81		,	"Vâ §ang ph¸i"			 ,40},
{131	,	"C«n L«n ph¸i"			 ,57},
};
--noi den ban do cac loai
PLACES_ARRAY1 = {
1,			--Hoa S¬n
150,		--Phôc Ng­u S¬n
48,			--TuyÕt B¸o §éng
209,		--D­îc V­¬ng §éng
19,			--TÇn L¨ng
75,			--Kháa Lang §éng
196,		--L©m Du Quan
226,		--Mª Cung Kª Qu¸n §éng
};

PLACES_ARRAY2 = {
195,		--Sa M¹c §Þa BiÓu
197,		--Ch©n Nói Tr­êng B¹ch
201,		--Tr­êng B¹ch s¬n b¾c
198,		--Tr­êng B¹ch S¬n Nam
227,		--Sa M¹c 1
228,		--Sa M¹c 2
229,		--Sa M¹c 3
225,		--"M¹c Cao QuËt
230,		--"Tr­êng Ca M«n
};