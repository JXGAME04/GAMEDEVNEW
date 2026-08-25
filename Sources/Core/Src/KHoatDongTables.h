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

// ten NPC (ha thuong CHI ASCII nhu g_StrLower)
static const char HDM_NPC_BNVAO[] = "quan nh¾c nhë hoµng thµnh t­";
static const char HDM_NPC_BCVAO[] = "vâ l©m truyÒn nh©n";
static const char HDM_NPC_COTHU[] = "cæ thñ";

#endif // KHOATDONGTABLES_H
