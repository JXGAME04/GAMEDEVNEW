-- ================================================================================================
-- [HE THONG] script/global/hocvocong.lua
-- Muc dich  : HOC VO CONG (1070 dong): hoc/nang ky nang tai NPC.
-- Duoc nap  : Include tu 4 tep (vd quanly.lua, lenhbaitanthu.lua, hotrotanthu.lua, hotrotest.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Ham (dong): ThemKyNang (4), ThemTiemNang (9), hocvocong (14), hocvocong2 (31), hockynangpubg (44), hvctvb (55), hvctl (60), hvcdm (64), hvc5doc (68), hvcnm (72), hvcty (76), hvctn (80), hvccb (84), hvcvd (88), hvccl (92), hvchs (96), hvcwh (100), hvcxy (103), HoTroSkill (219), hockhinhcong (245), hocskill90123 (250), kynangthieulamtest (269), kynangngamytest (287), del_all_skill (298) ... (47 ham)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
--Author: Fong KiÒu
--Date: 2021
--Function: Lib häc kü n¨ng vâ c«ng

function ThemKyNang()
	AddMagicPoint(100)
	Msg2Player("§· nhËn ®­îc 100 ®iÓm kü n¨ng")
end

function ThemTiemNang()
	AddProp(10000)
	Msg2Player("§· nhËn ®­îc 10000 ®iÓm tiÒm n¨ng")
end

function hocvocong()
	local fa1 = GetFactionValueName()
	if(fa1 == "none") then
		Say("Gia nhËp m«n ph¸i míi cã thÓ häc vâ c«ng",1,"KÕt thóc/no");
		return
	end
	Say("Chµo: ",8,
	"Häc vâ c«ng m«n ph¸i ThiÕu L©m/hvctl",
	"Häc vâ c«ng m«n ph¸i Thiªn Vu¬ng/hvctvb",
	"Häc vâ c«ng m«n ph¸i §­êng M«n/hvcdm",
	"Häc vâ c«ng m«n ph¸i Ngò §éc/hvc5doc",
	"Häc vâ c«ng m«n ph¸i Nga My/hvcnm",
	"Häc vâ c«ng m«n ph¸i Thuý Yªn/hvcty",
	"Trang 2/hocvocong2",
	"KÕt thóc!/ketthuc")
end

function hocvocong2()
	Say("Chµo: ",9,
	"Trang 1/hocvocong",
	"Häc vâ c«ng m«n ph¸i Thiªn NhÉn/hvctn",
	"Häc vâ c«ng m«n ph¸i C¸i Bang/hvccb",
	"Häc vâ c«ng m«n ph¸i Vâ §ang/hvcvd",
	"Häc vâ c«ng m«n ph¸i C«n Lu©n/hvccl",
	"Häc vâ c«ng m«n ph¸i Hoa S¬n/hvchs",
	"Häc vâ c«ng m«n ph¸i Vò Hån/hvcwh",
	"Häc vâ c«ng m«n ph¸i Tiªu Dao/hvcxy",
	"KÕt thóc!/ketthuc")
end
function hockynangpubg(nCurFac)

	for i=1,getn(SKILLNORMAL[nCurFac]) do
		AddMagic(SKILLNORMAL[nCurFac][i][1],SKILLNORMAL[nCurFac][i][2])
	end
	-- for i=1,getn(SKILL90_ARRAY[nCurFac]) do
		-- AddMagic(SKILL90_ARRAY[nCurFac][i][1],SKILL90_ARRAY[nCurFac][i][2])
	-- end
	show_kynang90(nCurFac)
	-- show_kynang150()
end

function hvctvb()
	--HoTroSkill("tianwang")
	hockynang(2)
end
function hvctl()
	--HoTroSkill("shaolin")
	hockynang(1)
end
function hvcdm()
	--HoTroSkill("tangmen")
	hockynang(3)
end
function hvc5doc()
	--HoTroSkill("wudu")
	hockynang(4)
end
function hvcnm()
	--HoTroSkill("emei")
	hockynang(5)
end
function hvcty()
	--HoTroSkill("cuiyan")
	hockynang(6)
end
function hvctn()
	--HoTroSkill("tianren")
	hockynang(8)
end
function hvccb()
	--HoTroSkill("gaibang")
	hockynang(7)
end
function hvcvd()
	--HoTroSkill("wudang")
	hockynang(9)
end
function hvccl()
	--HoTroSkill("kunlun")
	hockynang(10)	-- [HOASON 01/09c] Con Lon = SKILLNORMAL[10] (ban .truoc_hoason_0109 la 10; 11 nay la Hoa Son)
end
function hvchs()	-- [HOASON 01/09c] Hoa Son = SKILLNORMAL[11]
	--HoTroSkill("huashan")
	hockynang(11)
end
function hvcwh()	-- [VHTD 02/09] Vu Hon = SKILLNORMAL[12]
	hockynang(12)
end
function hvcxy()	-- [VHTD 02/09] Tieu Dao = SKILLNORMAL[13]
	hockynang(13)
end

tbAllSkill2 = {
	shaolin = {
		[1] = {3,5},
		[2] = {7,8,9},
		[3] = {10},
		[4] = {11},
		[5] = {12},
		[6] = {13,15,17},
		[7] = {18,19},
		[9] = {20,22,24},
	},
	tianwang = {
		[1] = {32,34,36},
		[2] = {29,30,31},
		[3] = {38},
		[4] = {39,41,43},
		[5] = {45},
		[6] = {47},
		[7] = {48,49,51,277},
		[9] = {55,57,59},
	},
	tangmen = {
		[1] = {61},
		[2] = {63,64},
		[3] = {66},
		[4] = {68,70,72,74},
		[5] = {76},
		[6] = {78},
		[7] = {80,81,83,85},
		[9] = {88,91,94,96},
	},
	wudu = {
		[1] = {98,99},
		[2] = {101,102,103},
		[3] = {104,105},
		[4] = {106,107,109,110},
		[5] = {111,112},
		[6] = {113},
		[7] = {114,115,116},
		[9] = {118,120,123},
	},
	emei = {
		[1] = {125,127},
		[2] = {128,129},
		[3] = {130},
		[4] = {131,133,134},
		[5] = {136},
		[6] = {138},
		[7] = {140,141,143,144},
		[9] = {323,149,151},
	},
	cuiyan = {
		[1] = {154,156},
		[2] = {157,158},
		[3] = {159},
		[4] = {160,162},
		[5] = {163},
		[6] = {164},
		[7] = {165,166,168},
		[9] = {169,172},
	},
	gaibang = {
		[1] = {176,177},
		[2] = {179,180},
		[3] = {181},
		[4] = {182,183},
		[5] = {185},
		[6] = {186,188},
		[7] = {190,191},
		[9] = {325,328},
	},
	tianren = {
		[1] = {197,199},
		[2] = {200,201,202},
		[3] = {203},
		[4] = {204,206,207},
		[5] = {208},
		[6] = {209},
		[7] = {210,211,213},
		[9] = {214,216,218},
	},
	wudang = {
		[1] = {220,221},
		[2] = {223,224},
		[3] = {225},
		[4] = {227,228},
		[5] = {279},
		[6] = {280},
		[7] = {281,233,234},
		[9] = {236,239},
	},
	kunlun = {
		[1] = {250,252},
		[2] = {253,254,256,257},
		[3] = {255},
		[4] = {258,259,260,261},
		[5] = {262,263},
		[6] = {264,266},
		[7] = {267,268,275},
		[9] = {269,272,274},
	},
	huashan = {	-- [HOASON 01/09c]
		[1] = {1347,1372},
		[2] = {1349,1374},
		[3] = {1350,1375},
		[4] = {1351,1376},
		[5] = {1354,1378},
		[6] = {1355,1379},
		[7] = {1358,1360,1380},
		[9] = {1364,1382,1365},
	},
}

function HoTroSkill(szFaction)

--local fa1 = GetFactionValueName()
--if(fa1 ~= szFaction) then
--	Say("§¹i hiÖp kh«ng thuéc m«n ph¸i "..szFaction.." kh«ng thÓ häc ®ùoc vâ c«ng",1,"KÕt thóc/no");
--	return
--end
	if tbAllSkill[szFaction] == nil then
		return
	end
	for i=1, 9 do
		if tbAllSkill[szFaction][i] ~= nil then
			for j=1, getn(tbAllSkill[szFaction][i]) do
				--if HaveMagic(tbAllSkill[szFaction][i][j]) == -1 then
						if(i>=9) then
							AddMagic(tbAllSkill[szFaction][i][j],20)
						else
							AddMagic(tbAllSkill[szFaction][i][j])
						end
				--end
			end
		end
	end
	Msg2Player("§· häc ®­îc kü n¨ng m«n ph¸i")
end

function hockhinhcong()
	AddMagic(210,1)
	Msg2Player("§· häc ®­îc kü n¨ng khinh c«ng")
end

function hocskill90123()
	--AddMagic(115,20);
	--AddMagic(116,20);
	--AddMagic(124,20);
	--AddMagic(125,20);
	--AddMagic(127,20);
	--AddMagic(128,20);
	--AddMagic(129,20);
	AddMagic(130,30);
	AddMagic(357,20);
	AddMagic(362,20);
	AddMagic(365,20);
	AddMagic(210,1); -- khinh cong
	AddMagic(160,30); -- the van tung
	AddMagic(150,30); -- thien ma giai the
	AddMagic(86,30); -- luu thuy
	Say("§· häc ®­îc kü n¨ng 90",0)
end

function kynangthieulamtest()
	AddMagic(9,19);				--thieu lam quyen phap
	--AddMagic(11,19);					--hoanh tao luc hop
	AddMagic(3,19);			--han long bat vu
	--AddMagic(15,19);			--bat dong minh vuong
	AddMagic(11,19);			--la han tran
	AddMagic(13,19);			--long trao ho trao
	--AddMagic(19,19);			--ma ha vo luong
	--AddMagic(21,19);			--dich can kinh
	AddMagic(18,29);			--nhu lai thien diep
	AddMagic(20,19);		--dat ma do giang
	AddMagic(149,19);		--phong suong
	AddMagic(239,19);		--nhan kiem hop nhat
	AddMagic(183,19);		--da cau tran
	AddMagic(261,19);		--thien thanh dia troc
	AddMagic(110,19);		--thien thanh dia troc
end

function kynangngamytest()
	AddMagic(77,20);
	AddMagic(80,20);
	AddMagic(82,20);
	AddMagic(86,20);
	AddMagic(89,20);
	--AddMagic(91,20);
	AddMagic(252,30);
	AddMagic(380,20);
end

function del_all_skill()
	DelMagic(210)
	Msg2Player("§· xo¸ hÕt kü n¨ng")
end

function hocskill90()
	Say("Ng­¬i muèn häc kü n¨ng m«n ph¸i nµo?",8,
	"del_all_skill/del_all_skill",
	"ThiÕu l©m/kynangthieulamtest","Thiªn v­¬ng/skillthienvuong",
	"Ngò ®éc/skillngudoc","§­êng m«n/skillduongmon",
	"Nga my/skillngamy","Thóy yªn/skillthuyyen",
	"Trang 2/hocskill902");
end

function hocskill902()
	Say("Ng­¬i muèn häc kü n¨ng m«n ph¸i nµo?",9,
	"Trang 1/hocskill90",
	"C¸i bang/skillcaibang","Thiªn nhÉn/skillthiennhan",
	"Vâ ®ang/skillvodang","C«n lu©n 90/skillconluan","Hoa S¬n 90/skillhoason",
	"Vò Hån 90/skillvuhon","Tiªu Dao 90/skilltieudao",
	"Trë l¹i/main");
end


function skillhoason()	-- [HOASON 01/09c]
	add_hs(150);	-- [VHTD 02/09f] 150 = hoc TAT CA (10..70, 90, 120, 150) de test 1 lan
	AddMagic(1364,20) AddMagic(1382,20)	-- [VHTD 02/09f] Hoa Son 90 (add_hs ban Linux khong co bac 90+)
	AddMagic(1365,20)	-- 120
	AddMagic(1369,20) AddMagic(1384,20)	-- 150
	AddMagic(1370,20)	-- tien giai
	Say("Ng­¬i ®· häc ®­îc toµn bé kü n¨ng (nhËp m«n ®Õn 150)",0)
end

function skillvuhon()	-- [VHTD 02/09]
	add_wh(150);	-- [VHTD 02/09f] 150 = hoc TAT CA (10..70, 90, 120, 150) de test 1 lan
	Say("Ng­¬i ®· häc ®­îc toµn bé kü n¨ng (nhËp m«n ®Õn 150)",0)
end

function skilltieudao()	-- [VHTD 02/09]
	add_xy(150);	-- [VHTD 02/09f] 150 = hoc TAT CA (10..70, 90, 120, 150) de test 1 lan
	Say("Ng­¬i ®· häc ®­îc toµn bé kü n¨ng (nhËp m«n ®Õn 150)",0)
end

function skillconluan()
	add_kl(90);
	Say("Ng­¬i ®· häc ®­îc skill 90",0)
end

function skillngudoc()
end

function skillvodang()
	AddMagic(153,60) --no loi chi
	AddMagic(155,60) --thuong hai minh nguyet
	AddMagic(152,60) --vo dang quyen phap 
	AddMagic(151,60) --vo dang kiem phap
	AddMagic(159,60) --that tinh tran
	AddMagic(164,60) --bac cap nhi phuc
	AddMagic(158,60) --kiem phi kinh thien
	AddMagic(160,60) --the van tung
	AddMagic(157,60) --toa vong vo nga
	AddMagic(165,60) --vo nga vo kiem
	AddMagic(267,60) --tam hoan thao nguyet
	AddMagic(166,60) --thai cuc than cong
	AddMagic(365,60) --thien dia vo cuc
	AddMagic(368,60) --nhan kiem hop nhat
	Say("Ng­¬i ®· häc ®­îc skill 90",0)
end

function skillthiennhan()
	AddMagic(135,60) --tan duong nhu huyet
	AddMagic(145,60) --don chi liet diem
	AddMagic(132,60) --thien nhan mau phap
	AddMagic(131,60) --thien nhan dao phap
	AddMagic(136,60) --hoa lien phan hoa
	AddMagic(137,60) --ao anh phi ho
	AddMagic(141,60) --lien hoa tinh thien
	AddMagic(138,60) --thoi son dien hai
	AddMagic(140,60) --phi hong vo tich
	AddMagic(364,60) --bi to thanh phong
	AddMagic(143,60) --lich ma doat hon
	AddMagic(142,60) --thau thien hoan nhat
	AddMagic(148,60) --ma diem that sat
	AddMagic(150,60) --thien ma giai the
	AddMagic(361,60) --van long kich
	AddMagic(362,60) --thien ngoai luu tinh
	AddMagic(391,60) --nhiep hon loan tam
	Say("Ng­¬i ®· häc ®­îc skill 90",0)
end

function skillcaibang()
	AddMagic(119,60) --dien mon that bat
	AddMagic(122,60) --kien nhan than thu
	AddMagic(115,60) --cai bang bong phap
	AddMagic(116,60) --cai bang chuong phap
	AddMagic(129,60) --hoa hiem di vi
	AddMagic(274,60) --giang long chuong
	AddMagic(124,60) --da cau bong
	AddMagic(128,20) --khang long huu hoi
	AddMagic(125,60) --bong da ac cau
	AddMagic(360,60) --tieu dieu cong
	AddMagic(130,60) --tuy diep cuong vu
	AddMagic(357,20) --phi long tai thien
	AddMagic(359,60) --thien ha vo cau
	Say("Ng­¬i ®· häc ®­îc skill 90",0)
end

function skillthuyyen()
	AddMagic(99,60) --phong hoa tuyet nguyet
	AddMagic(102,60) --phong quyen tan nguyet
	AddMagic(95,60) --thuy yen dao phap
	AddMagic(97,60) --thuy yen song dao
	AddMagic(269,60) --bang tam trai anh
	AddMagic(105,60) --vu da le hoa
	AddMagic(113,60) --phu van tan nguyet
	AddMagic(100,60) --ho the han bang
	AddMagic(109,60) --tuyet anh
	AddMagic(108,60) --muc da luu tinh
	AddMagic(111,60) --bich hai chieu sinh
	AddMagic(114,60) --bang cot tuyet tam
	AddMagic(336,60) --bang tung vo anh
	AddMagic(337,60) --bang tam tien su
	Say("Ng­¬i ®· häc ®­îc skill 90",0)
end

function skillngamy()
	AddMagic(85,60) --nhat diep tri nhu
	AddMagic(80,60) --phieu tuyet xuyen van
	AddMagic(77,60) --nga mi kiem phap
	AddMagic(79,60) --nga mi chuong phap
	AddMagic(93,60) --tu hang pho do
	AddMagic(385,60) --thoi song vong nguyet
	AddMagic(82,60) --tu tuong dong quy 
	AddMagic(89,60) --mong diep
	AddMagic(86,60) --luu thuy
	AddMagic(92,60) --phat tam tu huu
	AddMagic(88,60) --bat diet bat tuyet
	AddMagic(91,60) --phat quang pho chieu
	AddMagic(282,60) --thanh am phan xuong
	AddMagic(252,60) --phat phap vo bien
	AddMagic(328,60) --tam nga te nguyet
	AddMagic(380,60) --phong suong toai anh
	AddMagic(332,60) --pho do chung sinh
	Say("Ng­¬i ®· häc ®­îc skill 90",0)
end

function skillduongmon()
	AddMagic(45,60) --tich lich don
	AddMagic(347,60) --dia diem hoa 
	AddMagic(43,60) --duong mon am khi
	AddMagic(303,60) --doc thich cot
	AddMagic(50,60) --truy tam tien
	AddMagic(47,60) --doat hon tieu
	AddMagic(54,60) --man thien hoa vu
	AddMagic(343,60) --xuyen tam thich 
	AddMagic(345,60) --han bang thich
	AddMagic(349,60) --loi kich thuat
	AddMagic(249,60) --tieu li phi dao
	AddMagic(341,60) --tan hoa tieu
	AddMagic(58,60) --thien la dia vong
	AddMagic(48,60) --tam nhan
	AddMagic(339,60) --nhiep hon nguyet dao
	AddMagic(342,60) --cuu cung phi tinh 
	AddMagic(302,60) --bao bu le hoa
	AddMagic(351,60) --loan hoan kich
	Say("Ng­¬i ®· häc ®­îc skill 90",0)
end

function skillngudoc()
	AddMagic(65,60) --huy?t dao d?c sát
	AddMagic(63,60) --huy?t sa chu?ng
	AddMagic(62,60) --ngu d?c chu?ng pháp
	AddMagic(60,60) --ngu d?c dao pháp
	AddMagic(67,60) --cuu thiên cu?ng lôi
	AddMagic(70,60) --xich diem thuc thiên
	AddMagic(66,60) --tap nan duoc kinh
	AddMagic(68,60) --u minh kho lau
	AddMagic(384,60) --bach d?c xuyên tâm
	AddMagic(64,60) --bang lam huyen tinh
	AddMagic(69,60) --Vo hinh d?c
	AddMagic(356,60) --xuyên y phá giáp
	AddMagic(73,60) --van d?c th?c tâm
	AddMagic(72,60) --xuyên tâm d?c thích
	AddMagic(71,60) --thiên cu?ng d?a sát
	AddMagic(74,60) --chu c?p thanh minh
	AddMagic(75,60) --ngu d?c kí kinh
	AddMagic(353,60) --am phong thuc c?t
	AddMagic(355,60) --huyen âm tr?m
	AddMagic(390,60) --doan canhu cot
	Say("Ng­¬i ®· häc ®­îc skill 90",0)
end

function skillthienvuong()
	AddMagic(34,60) --kinh lôi tr?m
	AddMagic(30,60) --h?i phong l?c nhân
	AddMagic(29,60) --tram long quy?t
	AddMagic(24,60) --thiên vuong dao tr?m
	AddMagic(23,60) --thiên vuong dao pháp
	AddMagic(26,60) --thiên vuong tru? pháp
	AddMagic(33,60) --tinh tam quy?t
	AddMagic(37,60) --bat long tram
	AddMagic(35,60) --duong quan tam diep
	AddMagic(31,60) --hang van quyet
	AddMagic(40,60) --doan hon thich
	AddMagic(42,60) --kim chung trao
	AddMagic(32,60) --vo tam tram
	AddMagic(41,60) --thuyet chien bat phuong
	AddMagic(324,60) --thua long quyet
	AddMagic(36,60) --thien vuong chien y
	AddMagic(322,60) --pha thien tram
	AddMagic(323,60) --truy tinh truc nguyet
	AddMagic(325,60) --truy phong nguyet
	Say("Ng­¬i ®· häc ®­îc skill 90",0)
end

function skillthieulam()
	AddMagic(14,60) --hàng long bát vu
	AddMagic(10,60) --kim cang ph?c ma
	AddMagic(8,60) --thi?u lâm quy?n pháp
	AddMagic(4,60) --thi?u lâm côn pháp
	AddMagic(6,60) --thi?u lâm dao pháp
	AddMagic(15,60) --bát d?ng minh vuong
	AddMagic(16,60) --la hán tr?n 
	AddMagic(20,60) --su t? h?ng
	AddMagic(17,60) --long tr?o h? tr?o 
	AddMagic(11,60) --hoành t?o l?c h?p
	AddMagic(19,60) --ma h? vô luong
	AddMagic(21,60) --d?ch cân kinh
	AddMagic(273,60) --nhu lai thiên di?p
	AddMagic(318,60) --d?t ma d? giáng
	AddMagic(319,60) --hoành t?o thiên quân
	AddMagic(321,60) --vô tu?ng tr?m
	Say("Ng­¬i ®· häc ®­îc skill 90",0)
end


SKILLNORMAL = {
	[1]={--thieu lam
		{14	, 0}, ---Han Long Bat Vu
		{10	, 0}, --- Kim Cang Phuc Ma
		{4		, 0}, --- Thieu Lam Con Phap
		{6		, 0}, --- Thieu Lam Dao Phap
		{8		, 0}, --- Thieu Lam Quyen Phap
		{15	, 0}, --- Bat Dong Minh Vuong
		{16	, 0}, --- La Han Tran
		{20	, 0}, --- Su Tu Hong
		{271	, 0}, --- Long Trao ho trao
		{11	, 0}, --- Hoanh Tao Luc Hop
		{19	, 0}, --- Ma Ha Vo Luong
		{21	, 0}, --- Dich Can Kinh
		{273	, 0} --- Nhu Lai Thien Diep-Tran phai
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
	[11]={--hoa son [HOASON 01/09c] (Linux add_hs 10..70; bang RIENG cua hocvocong.lua)
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
	[12]={--vu hon [VHTD 02/09]
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
	[13]={--tieu dao [VHTD 02/09]
		{2114,0},---Ngo Cau Suong Tuyet
		{2136,0},---Tuy y Khuc
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
}

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
		{"Lo¹n Hoµn KÝch",351,0}
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
		{"V©n Long KÝch",361,1},
		{"Thiªn Ngo¹i L­u Tinh",362,1},
		{"NhiÕp Hån Lo¹n T©m",391,0}
	},
	[9]={
		{"Thiªn §Þa V« Cùc",365,1},
		{"Nh©n KiÕm Hîp NhÊt",368,1}
	},
	[10]={
		{"Ng¹o TuyÕt Tiªu Phong",372,1},
		{"L«i §éng Cöu Thiªn",375,1},
		{"Tóy Tiªn T¸ Cèt",394,0}
	},
	[11]={ -- [HOASON 01/09]
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

SKILL120AR={
	709,708,710,711,712,713,714,715,716,717,1365,1984,2127 -- [VHTD 02/09]
}

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
		{"Cµn Kh«n NhÊt TrÞch",1071,1},
		{"TÝch LÞch Lo¹n Hoµn KÝch",1110,0}
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
		{"Thñy Anh Man Tó",1065,1}
	},
	[7]={
		{"Thêi Thõa Lôc Long",1073,1},
		{"Bæng Huýnh L­îc §Þa",1074,1}
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
		{"Trõ Gian DiÖt NÞnh",1985,1}
	},
	[13]={ -- [VHTD 02/09]
		{"ThËp Bé NhÊt S¸t",2129,1},
		{"Mai Hoa Tam Léng",2142,1}
	}
}

function hockynang(nCurFac)
	local fa1 = GetFirstAddFaction() + 1
	if(fa1 ~= nCurFac) then
		Say("§¹i hiÖp kh«ng thuéc m«n ph¸i "..fa1.." kh«ng thÓ häc ®ùoc vâ c«ng", 1, "KÕt thóc/no")
		return
	end

	for i=1,getn(SKILLNORMAL[nCurFac]) do
		AddMagic(SKILLNORMAL[nCurFac][i][1],SKILLNORMAL[nCurFac][i][2])
	end
	show_kynang90(nCurFac)
end

function show_kynang90(nCurFac)
	local nTarFac = GetFactionNo()+1
	if(nCurFac ~= nil) then
		nTarFac = nCurFac
	end
	local nskillcount = getn(SKILL90_ARRAY[nTarFac])
	if(GetLevel() < 80) then
		Talk(1,"","Ng­êi ch¬i ph¶i ®¹t ®¼ng cÊp 80 trë lªn míi cã thÓ nhËn kü n¨ng .")
		return 
	end
	for i=1,getn(SKILL90_ARRAY[nTarFac]) do
		AddMagic(SKILL90_ARRAY[nCurFac][i][2],20)
		local str = "B¹n ®· häc ®­îc kü n¨ng "..SKILL90_ARRAY[nCurFac][i][1]
		Msg2Player(str)
	end

	AddMagic(SKILL120AR[nCurFac], 20)
	AddMagic(210,1)
	
	for i=1,getn(SKILL150_ARRAY[nTarFac]) do
		AddMagic(SKILL150_ARRAY[nCurFac][i][2], 20)
	end

--	local skilltab = {};
--	local count = 1;
--	for i=1,nskillcount do
--		skilltab[count] = format(SKILL90_ARRAY[nTarFac][i][1].."/selskill(%d,%d)", i, nTarFac);
--		count = count + 1;
--	end
--	if(count == 1) then
--	return end
--	skilltab[count] = "§ãng/no";
--	Say("Ng­¬i muèn häc kü n¨ng nµo:",count,skilltab);
end
FACTION_TO_SKILL150 = {
	[1]  = 1, 
	[2]  = 2,
	[3]  = 3, 
	[4]  = 4, 
	[5]  = 5, 
	[6]  = 6, 
	[7]  = 7,  
	[8]  = 8,  
	[9]  = 9,  
	[10] = 10, 
	[11] = 11, 
	[12] = 12, -- [VHTD 02/09]
	[13] = 13,
}
function show_kynang150Chon()
	local nFaction = GetFactionNo()+1
	local nTarFac = FACTION_TO_SKILL150[nFaction]
	local nTienDong = 500
	local TIENDONG_ID = 4835     
	local nDSK = GetTask(TASK_DSK)	    
	local nSoDSK = 5000	 
	local nTD  = GetItemCount(0,6,1, TIENDONG_ID, -1, -1, pos_equiproom)
	
	if (not nTarFac) or (not SKILL150_ARRAY[nTarFac]) then
		Talk(1,"","Kh«ng x¸c ®Þnh ®­îc m«n ph¸i!")
		return
	end

	if GetLevel() < 150 then
		Talk(1,"",14576)
		return
	end

	
	if nDSK < nSoDSK then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nSoDSK.." §iÓm Sù KiÖn.")
		return
	end
	if nTD < nTienDong then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nTienDong.." TiÒn §ång.")
		return
	end
	
	local tb = SKILL150_ARRAY[nTarFac]

	local nskillcount = 0
	while tb[nskillcount + 1] do
		nskillcount = nskillcount + 1
	end

	local skilltab = {}
	local count = 1
	for i = 1, nskillcount do
		skilltab[count] = format("%s/selskill150new(%d,%d)", tb[i][1], i, nTarFac)
		count = count + 1
	end

	skilltab[count] = "§ãng/no"
	Say("Ng­¬i muèn häc kü n¨ng nµo:", count, skilltab)
end


function selskill150new(nIndex, nFaction)

	local tbSkill = SKILL150_ARRAY[nFaction][nIndex]
	if not tbSkill then
		return
	end

	local szName = tbSkill[1]
	local nSkillId = tbSkill[2]
	local nCanLearn = tbSkill[3]


	if nCanLearn ~= 1 then
		Talk(1,"","Kü n¨ng nµy ch­a ®­îc më.")
		return
	end


	Say(
		format("Ng­¬i cã ch¾c muèn häc %s kh«ng?", szName),
		2,
		{
			format("X¸c nhËn/hocskill150new(%d,%s)", nSkillId, szName),
			"Hñy/no"
		}
	)
end

function hocskill150new(nSkillId)
	local szName = ""
	local nTienDong = 500
	local TIENDONG_ID = 4835     
	local nDSK = GetTask(TASK_DSK)	    
	local nSoDSK = 5000	 
	local nTD  = GetItemCount(0,6,1, TIENDONG_ID, -1, -1, pos_equiproom)
	
	if nDSK < nSoDSK then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nSoDSK.." §iÓm Sù KiÖn.")
		return
	end
	if nTD < nTienDong then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nTienDong.." TiÒn §ång.")
		return
	end
	
	for fac, tb in pairs(SKILL150_ARRAY) do
		local i = 1
		while tb[i] do
			if tb[i][2] == nSkillId then
				szName = tb[i][1]
				break
			end
			i = i + 1
		end
	end
			DelItem(0, -1, 6,1 ,TIENDONG_ID, -1, -1, pos_equiproom, nTienDong)
			SetTask(TASK_DSK, GetTask(TASK_DSK) - nSoDSK)

	AddMagic(nSkillId, 20)

	Talk(
		1,
		"",
		"<color=yellow>B¹n ®· häc thµnh c«ng kü n¨ng:<color>\n<color=green>"..szName.." <color>"
	)
end






function no()
end

-- (20/08 - chu game: "chi duoc nhan ky nang 90 thoi") Ham RIENG cho bot KPlayer:
-- copy dung vong skill 90 cua show_kynang90, KHONG kem SKILL120AR + chieu 210
-- nhu ham NPC goc. nCurFac 1-based (m_nCurFaction + 1).
function bot_hoc90(nCurFac)
	if (GetLevel() < 80) then
		return
	end
	if (nCurFac == nil or SKILL90_ARRAY[nCurFac] == nil) then
		return
	end
	for i=1,getn(SKILL90_ARRAY[nCurFac]) do
		AddMagic(SKILL90_ARRAY[nCurFac][i][2],20)
	end
end

-- [SKILL120 06/09] chu game: "bot len 120 se co skill 120 full skill". Ham RIENG cho bot
-- (KPlayerBot.cpp pb_TrangBiTheoCap buoc 1c goi khi bot dat cap 120): hoc ky nang 120
-- cua phai (SKILL120AR - mot chieu/phai) o cap 20 = max (skills.txt MaxLevel 20), dung
-- dong AddMagic(SKILL120AR[nCurFac], 20) cua show_kynang90. KHONG kem 210 khinh cong /
-- SKILL150_ARRAY. nCurFac 1-based (m_nCurFaction + 1).
function bot_hoc120(nCurFac)
	if (GetLevel() < 120) then
		return
	end
	if (nCurFac == nil or SKILL120AR[nCurFac] == nil) then
		return
	end
	AddMagic(SKILL120AR[nCurFac], 20)
end
