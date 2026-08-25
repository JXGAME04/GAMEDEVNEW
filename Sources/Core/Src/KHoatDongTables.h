// KHoatDongTables.h - SINH TU DONG boi ReverseTools/gen_hoatdong_tables.py - DUNG SUA TAY
// Nguon: script + settings song cua may chu (E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server)
// Marker la RAW TCVN3 trich byte-for-byte. Toa do don vi O (mps = O * 32).
#ifndef KHOATDONGTABLES_H
#define KHOATDONGTABLES_H

// ===== BACH NHAN LOI DAI =====
#define HD_BN_MAP		960	// map Loi Dai Hoang Thanh Tu
#define HD_BN_MAP_NPC	176	// thanh co NPC loi vao (Lam An)
#define HD_BN_TSK_LUOT	2709	// task luot exp trong ngay (YYMMDD*256+luot)
#define HD_BN_TSK_MAP	2323	// task 2323/2324/2325 = noi dung truoc khi vao

struct HDPoint { short x, y; };
static const HDPoint g_HDBNNpc = { 1464, 3223 };	// NPC loi vao (enternpc.txt)
static const HDPoint g_HDBNRevive = { 1749, 3135 };	// diem hoi sinh chung
static const HDPoint g_HDBNXaPhu = { 1750, 3159 };	// Xa phu roi map (chefu.txt)

// 5 dai: tam dai (InPos - cung la o trap) + 4 diem bi day ra
static const HDPoint g_HDBNDaiIn[5] = {
	{ 1784, 3099 },
	{ 1744, 3061 },
	{ 1822, 3138 },
	{ 1705, 3101 },
	{ 1781, 3178 },
};
static const HDPoint g_HDBNDaiOut[5][4] = {
	{ { 1776, 3087 }, { 1776, 3115 }, { 1796, 3086 }, { 1796, 3116 } },
	{ { 1737, 3051 }, { 1737, 3077 }, { 1757, 3050 }, { 1758, 3077 } },
	{ { 1816, 3125 }, { 1814, 3153 }, { 1836, 3127 }, { 1835, 3153 } },
	{ { 1699, 3087 }, { 1698, 3116 }, { 1720, 3088 }, { 1719, 3117 } },
	{ { 1774, 3192 }, { 1776, 3163 }, { 1795, 3164 }, { 1795, 3190 } },
};

// 15 diem Co Thu (buff x2) co the xuat hien (drummer.txt)
#define HD_BN_COTHU_COUNT	15
static const HDPoint g_HDBNCoThu[HD_BN_COTHU_COUNT] = {
	{ 1668, 3103 },
	{ 1711, 3064 },
	{ 1731, 3086 },
	{ 1747, 3024 },
	{ 1745, 3137 },
	{ 1770, 3049 },
	{ 1766, 3082 },
	{ 1762, 3124 },
	{ 1762, 3188 },
	{ 1783, 3214 },
	{ 1803, 3079 },
	{ 1807, 3124 },
	{ 1809, 3161 },
	{ 1831, 3100 },
	{ 1852, 3131 },
};

// ===== BANG CHIEN (VO LAM DE NHAT BANG) =====
#define HD_BC_MAP_NPC	53	// Ba Lang Huyen
#define HD_BC_TSK_CHET	2370	// so lan chet tran nay
#define HD_BC_TSK_TRAN	2376	// tran mang tran nay
static const HDPoint g_HDBCNpc = { 1628, 3173 };	// NPC Vo Lam Truyen Nhan
static const HDPoint g_HDBCRej = { 1619, 3175 };	// o bi day ve khi trap tu choi
#define HD_BC_SIGN_COUNT	6
static const short g_HDBCSignMap[HD_BC_SIGN_COUNT] = { 608, 609, 610, 611, 612, 613 };
static const short g_HDBCFightMap[3] = { 605, 606, 607 };
#define HD_BC_TRAP_COUNT	36
static const HDPoint g_HDBCTrap[HD_BC_TRAP_COUNT] = {	// o trap khu bao danh
	{ 1614, 3182 },
	{ 1615, 3182 },
	{ 1615, 3183 },
	{ 1624, 3173 },
	{ 1625, 3173 },
	{ 1623, 3174 },
	{ 1624, 3174 },
	{ 1622, 3175 },
	{ 1623, 3175 },
	{ 1624, 3175 },
	{ 1621, 3176 },
	{ 1622, 3176 },
	{ 1623, 3176 },
	{ 1624, 3176 },
	{ 1620, 3177 },
	{ 1621, 3177 },
	{ 1622, 3177 },
	{ 1623, 3177 },
	{ 1624, 3177 },
	{ 1619, 3178 },
	{ 1620, 3178 },
	{ 1621, 3178 },
	{ 1624, 3178 },
	{ 1618, 3179 },
	{ 1619, 3179 },
	{ 1620, 3179 },
	{ 1617, 3180 },
	{ 1618, 3180 },
	{ 1619, 3180 },
	{ 1616, 3181 },
	{ 1617, 3181 },
	{ 1618, 3181 },
	{ 1616, 3182 },
	{ 1617, 3182 },
	{ 1617, 3183 },
	{ 1617, 3184 },
};

// ===== marker thoai / thong bao (RAW TCVN3) =====
static const char HDM_OPT_LOIDAI[] = "L«i §µi Hoµng Thµnh T­";
static const char HDM_OPT_VAOLOIDAI[] = "Ta muèn vµo L«i §µi Hoµng Thµnh T­";
static const char HDM_SAY_CHUAGIO[] = "Thêi gian më L«i §µi mçi ngµy";
static const char HDM_SAY_THIEUCAP[] = "míi cã thÓ tham gia";
static const char HDM_MSG_COTHU[] = "Cæ Thñ ®· xuÊt hiÖn mäi n";
static const char HDM_OPT_THAMGIA[] = "Ta muèn tham gia thi ®Êu";
static const char HDM_SAY_CHUABATDAU[] = "TrËn ®Êu Vâ L©m §Ö NhÊt Bang vÉn ch­a b¾t ®Çu";
static const char HDM_SAY_TAPHOP[] = "Phe ta hiÖn ®ang tËp hîp chuÈn bÞ vµo ®Êu tr­êng";
static const char HDM_SAY_HETMANG[] = "Sè lÇn tö vong ®· v­ît qu¸";
static const char HDM_SAY_DAYNGUOI[] = "] sè ng­êi tham gia ®· v­ît qu¸";
static const char HDM_SAY_SAIKHU[] = "Khu vùc chuÈn bÞ cho liªn minh cña ng­¬i kh«ng ph¶i ë ®©y, h·y chän l¹i chÝnh x¸c khu vùc t¹i Vâ L©m Minh Chñ.";
static const char HDM_SAY_CHUATOI[] = "Ch­a ®Õn thêi ®iÓm vµo ®Êu tr­êng, h·y ®îi trong gi©y l¸t";
static const char HDM_MSG_VAODAI[] = "Ng­¬i ®· vµo L«i §µi";
static const char HDM_MSG_ROIDAI[] = "Ng­¬i ®· rêi khái L«i §µi";
static const char HDM_MSG_DANGDANH[] = "L«i §µi ®· tiÕn hµnh tû vâ, xin h·y ®îi l­ît b¸o danh sau.";
static const char HDM_MSG_KETTHUC[] = "L«i §µi Hoµng Thµnh T­ h«m nay kÕt thóc !";

// ===== TIN SU (nhiem vu dua tin qua Thien Bao Kho map 395) =====
#define HD_TS_MAP_AI	395	// ai Thien Bao Kho
#define HD_TS_TSK_MA	1201	// ma 5 ruong phai mo (theo THU TU)
#define HD_TS_TSK_DA	1202	// cac ruong da mo (prefix cua 1201)
#define HD_TS_TSK_TT	1203	// 0 khong/that bai; 10 da nhan; 20 trong ai; 21 tam ngung; 25/30 xong
#define HD_TS_TSK_TUYEN	1204	// 1 = Thanh Do->Dai Ly, 2 = nguoc lai
#define HD_TS_TSK_NGAY	4128	// so luot hom nay (YYMMDD*256 + n)
static const HDPoint g_HDTSDq11 = { 3018, 5087 };	// Dich quan Thanh Do
static const HDPoint g_HDTSDq162 = { 1676, 3115 };	// Dich quan Dai Ly
static const HDPoint g_HDTSDqAi = { 1412, 3203 };	// Dich quan trong ai 395
static const HDPoint g_HDTSTieuTran = { 1386, 2442 };	// Tieu Tran - NPC CUA RA (sau trong ai)
static const HDPoint g_HDBCXaPhu = { 1597, 3139 };	// Xa phu khu bao danh BC (ve thanh)
// tuyen: { tuyen, map nhan, map dich, x dich, y dich } (citygo posthouse.lua)
static const short g_HDTSVe[2][5] = {
	{ 1, 11, 162, 1674, 3132 },
	{ 2, 162, 11, 3021, 5090 },
};
// 9 Bao ruong map 395 (ruong i+1; Thu Ho Gia dung canh, lech +2 o x)
static const HDPoint g_HDTSRuong[9] = {
	{ 1415, 3082 },
	{ 1437, 3040 },
	{ 1409, 2978 },
	{ 1377, 2940 },
	{ 1409, 2870 },
	{ 1378, 2821 },
	{ 1420, 2769 },
	{ 1375, 2737 },
	{ 1419, 2668 },
};

static const char HDM_OPT_TINSU[] = "NhiÖm vô TÝn Sø";
static const char HDM_OPT_TSNHAN[] = "Ta b»ng lßng!";
static const char HDM_OPT_TSGIAO[] = "Ta ®Õn ®Ó giao nhiÖm vô TÝn sø";
static const char HDM_OPT_TSTHUONG[] = "NhËn l·nh phÇn th­ëng";
static const char HDM_OPT_TSXAPHU[] = "§i n¬i ®Æc biÖt lµm NhiÖm vô TÝn Sø";
static const char HDM_OPT_TSMUON[] = "Muèn";
static const char HDM_OPT_TSBATDAU[] = "B¾t ®Çu nhiÖm vô";
static const char HDM_OPT_TSTIEPTUC[] = "TiÕp tôc nhiÖm vô";
static const char HDM_OPT_TSROI[] = "Rêi khái khu vùc";
static const char HDM_OPT_TSMUONRA[] = "Ta muèn ®i ra";
static const char HDM_OPT_TSDUNGRA[] = "§óng! Ta muèn rêi khái";
static const char HDM_OPT_TSCHUAXONG[] = "Ta ch­a hoµn thµnh kh¶o nghiÖm, ta muèn ®i ra mét chót";
static const char HDM_SAY_TSDOITRUONG[] = "kh«ng ph¶i ®éi tr­ëng";
static const char HDM_SAY_BCKHONGBANG[] = "Ch­a gia nhËp bang";
static const char HDM_SAY_BCKHONGLM[] = "kh«ng cã liªn minh";
static const char HDM_SAY_BCTHIEUCAP[] = "Ph¶i ®¹t cÊp ";
static const char HDM_SAY_BCDOCAM[] = "Kh«ng ®­îc phÐp mang vËt phÈm";
static const char HDM_MSG_BNMET[] = "Ta mÖt råi";
static const char HDM_MSG_DADOC[] = "§· ®äc sí";
static const char HDM_MSG_TSCHUAHA[] = "ch­a h¹ ®­îc ng­êi gi÷ r­¬ng";
static const char HDM_MSG_NGHERO[] = "VËy ng­¬i ®· nghe râ råi";
static const char HDM_SAY_TSHETLUOT[] = "H«m nay ng­¬i ®· vÊt v¶ råi. Ngµy mai h·y quay l¹i nhÐ";
static const char HDM_SAY_TSMETMOI[] = "H«m nay ng­¬i ®· mÖt mái råi";
static const char HDM_SAY_TSQUAMET[] = "h«m nay ng­¬i ®· qu¸ mÖt råi";
static const char HDM_SAY_TSDANGCO[] = "kh«ng thÓ tiÕp nhËn nhiÖm vô gièng nhau";
static const char HDM_SAY_TSTHIEUCAP[] = "ch­a ®ñ 90";
static const char HDM_SAY_TSTHIEUO[] = "kh«ng ®ñ chç trèng";

// ten NPC (ha thuong CHI ASCII nhu g_StrLower)
static const char HDM_NPC_BNVAO[] = "quan nh¾c nhë hoµng thµnh t­";
static const char HDM_NPC_BCVAO[] = "vâ l©m truyÒn nh©n";
static const char HDM_NPC_COTHU[] = "cæ thñ";
static const char HDM_NPC_DICHQUAN[] = "dÞch quan";
static const char HDM_NPC_TSRUONG[] = "b¶o r­¬ng";
static const char HDM_NPC_TSGIU[] = "b¶o khè thñ hé gi¶";
static const char HDM_NPC_TIEUTRAN[] = "tiªu trÊn";

#endif // KHOATDONGTABLES_H
