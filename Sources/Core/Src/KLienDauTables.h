// KLienDauTables.h - SINH TU DONG boi ReverseTools/gen_liendau_tables.py - DUNG SUA TAY
// Nguon: script song cua may chu (E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/script)
// Marker la RAW TCVN3 trich thang tu Lua (byte-for-byte) - giong KTongKimTables.h.
#ifndef KLIENDAUTABLES_H
#define KLIENDAUTABLES_H

// ===== hang so he Lien dau (missions/leaguematch/head.lua) =====
#define LD_LEVEL_JUNIOR	80	// cap toi thieu hang Kiet xuat
#define LD_LEVEL_SENIOR	120	// cap toi thieu hang Vo lam
#define LD_PREP_SEC	240	// giay o khu chuan bi truoc khi vao san (PREP_FREQ*PREP_TOTAL)
#define LD_FIGHT_SEC	600	// giay toi da mot tran (FIGHT_FREQ*FIGHT_TOTAL)
#define LD_PHUT_LUOT	15	// so phut mot luot (WLLS_MATCHTIME - doi duoc o wlls_config.lua)

struct LDPoint { short x, y; };

// ===== toa do chuan trong hoi truong / khu chuan bi (don vi O) =====
static const LDPoint g_LDPosHall = { 1523, 3024 };	// diem dap khi vao hoi truong
static const LDPoint g_LDPosPrep = { 1596, 2977 };	// diem dap khu chuan bi

// ===== bang map (hoi truong / khu chuan bi / dau truong) =====
#define G_LDHALL_COUNT	21
static const short g_LDHall[G_LDHALL_COUNT] = {
	396, 398, 400, 402, 404, 406, 408, 410, 412, 414,
	540, 542, 544, 546, 548, 550, 552, 554, 556, 558,
	991,
};
#define G_LDPREP_COUNT	21
static const short g_LDPrep[G_LDPREP_COUNT] = {
	560, 561, 562, 563, 564, 565, 566, 567, 568, 569,
	570, 571, 572, 573, 574, 575, 576, 577, 578, 579,
	993,
};
#define G_LDARENA_COUNT	21
static const short g_LDArena[G_LDARENA_COUNT] = {
	397, 399, 401, 403, 405, 407, 409, 411, 413, 415,
	541, 543, 545, 547, 549, 551, 553, 555, 557, 559,
	992,
};

// ===== 7 thanh co Su gia lien dau: { mapid, hang(1 Kiet xuat/2 Vo lam), x, y } =====
#define LD_CITY_COUNT	7
static const short g_LDCity[LD_CITY_COUNT][4] = {
	{ 37, 1, 1777, 3063 },
	{ 78, 1, 1546, 3117 },
	{ 176, 1, 1463, 3247 },
	{ 1, 2, 1673, 3219 },
	{ 11, 2, 3214, 5149 },
	{ 80, 2, 1753, 3035 },
	{ 162, 2, 1599, 3150 },
};

// ===== NPC trong hoi truong / khu chuan bi (don vi O) =====
#define LD_SIGNUP_COUNT	3
static const LDPoint g_LDNpcSignup[LD_SIGNUP_COUNT] = {	// Quan vien hoi truong
	{ 1514, 3015 },
	{ 1534, 3050 },
	{ 1484, 2999 },
};
static const LDPoint g_LDNpcXaPhu = { 1461, 3022 };	// Xa phu hoi truong
static const LDPoint g_LDNpcRuong = { 1522, 3083 };	// Ruong chua do (OpenBox)
static const LDPoint g_LDNpcThuoc = { 1556, 3050 };	// Chu duoc diem
static const LDPoint g_LDNpcThiVe = { 1584, 2965 };	// Thi ve hoi truong (khu chuan bi)

// ===== Xa phu hoi truong: dong thoai -> map thanh =====
#define LD_VE_COUNT	7
static const short g_LDVeMap[LD_VE_COUNT] = { 1, 11, 162, 37, 78, 80, 176 };
static const char* const g_LDVeOpt[LD_VE_COUNT] = {
	"Ph­îng T­êng Phñ",
	"Thµnh §« Phñ",
	"§¹i Lý",
	"BiÖn Kinh Phñ",
	"T­¬ng D­¬ng Phñ",
	"D­¬ng Ch©u Phñ",
	"L©m An Phñ",
};

// ===== marker hoi thoai / thong bao (RAW TCVN3) =====
static const char LDM_OPT_KHUTHIDAU[] = "Ta muèn ®Õn khu thi ®Êu h¹ng ";
static const char LDM_OPT_CHIENDOI[] = "ChiÕn ®éi h¹ng ";
static const char LDM_OPT_THUONGLD[] = "Ta muèn l·nh phÇn th­ëng liªn ®Êu";
static const char LDM_OPT_LAPDOI[] = "Ta muèn lËp chiÕn ®éi!";
static const char LDM_OPT_LAPNHOM[] = "Ta muèn lËp nhãm!";
static const char LDM_OPT_XEPHANG[] = "Ta muèn l·nh phÇn th­ëng xÕp h¹ng!";
static const char LDM_OPT_LANHGIAI[] = "Ta muèn l·nh gi¶i th­ëng!";
static const char LDM_OPT_DANHHIEU[] = "Ta muèn l·nh gi¶i th­ëng danh hiÖu!";
static const char LDM_OPT_THAMCHIEN[] = "Ta muèn tham chiÕn!";
static const char LDM_OPT_SANSANG[] = "Ta ®· s½n sµng!";
static const char LDM_OPT_XACNHAN[] = "X¸c nhËn";
static const char LDM_SAY_DOCAM[] = "Ng­¬i thËt to gan! D¸m kh«ng tu©n lÖnh cña Vâ L©m Minh Chñ, tù ý ®em d­îc phÈm cÊm vµo, mau rêi khái ®©y!";
static const char LDM_SAY_DONGCUA[] = "Hiªn t¹i lµ thêi gian liªn ®Êu t¹m nghØ, héi tr­êng sÏ t¹m ®ãng cöa, b¹n h·y chê ®Õn kú thi ®Êu h·y quay l¹i!";
static const char LDM_SAY_TAMNGHI[] = "Xin h·y t¹m nghØ ë ®©y";
static const char LDM_SAY_DANGDAU[] = "VÉn ®ang tiÕn hµnh thi ®Êu";
static const char LDM_SAY_CHUATHIDAU[] = "B©y giê b¹n ng­¬i ch­a thi ®Êu";
static const char LDM_MSG_BAODANH[] = "®ang trong giai ®o¹n b¸o danh. C¸c ®éi muèn tham gia h·y nhanh ch©n ®Õn b¸o danh";
static const char LDM_MSG_BATDAU[] = "thi ®Êu chÝnh thøc b¾t ®Çu";
static const char LDM_OPT_LAP[] = "Ta muèn lËp";

// ten NPC de DT_FindNpcName (ha thuong CHI ASCII nhu g_StrLower - byte co dau giu nguyen)
static const char LDM_NPC_SUGIA[] = "sø gi¶";
static const char LDM_NPC_QUANVIEN[] = "quan viªn héi tr­êng";
static const char LDM_NPC_XAPHU[] = "xa phu";
static const char LDM_NPC_RUONG[] = "r­¬ng chøa ®å";
static const char LDM_NPC_THIVE[] = "thÞ vÖ héi tr­êng";

// ===== do CAM mang vao san (WLLS_FORBID_ITEM) - auto phai cat vao ruong truoc =====
// { genre, detail, particular }
#define LD_FORBID_COUNT	77
static const short g_LDForbid[LD_FORBID_COUNT][3] = {
	{ 6, 1, 218 },
	{ 6, 1, 219 },
	{ 6, 1, 220 },
	{ 6, 1, 221 },
	{ 6, 1, 222 },
	{ 6, 1, 223 },
	{ 6, 1, 224 },
	{ 6, 1, 225 },
	{ 6, 1, 226 },
	{ 6, 1, 227 },
	{ 6, 1, 228 },
	{ 6, 1, 229 },
	{ 6, 1, 230 },
	{ 6, 1, 231 },
	{ 6, 1, 232 },
	{ 6, 1, 233 },
	{ 6, 1, 234 },
	{ 6, 1, 235 },
	{ 6, 1, 115 },
	{ 6, 1, 116 },
	{ 6, 1, 117 },
	{ 6, 1, 118 },
	{ 6, 1, 119 },
	{ 6, 1, 120 },
	{ 6, 1, 121 },
	{ 6, 0, 1 },
	{ 6, 0, 2 },
	{ 6, 0, 3 },
	{ 6, 0, 4 },
	{ 6, 0, 5 },
	{ 6, 0, 6 },
	{ 6, 0, 7 },
	{ 6, 0, 8 },
	{ 6, 0, 9 },
	{ 6, 0, 10 },
	{ 6, 0, 60 },
	{ 6, 0, 61 },
	{ 6, 0, 62 },
	{ 6, 1, 73 },
	{ 6, 1, 19 },
	{ 6, 1, 69 },
	{ 6, 1, 885 },
	{ 6, 1, 886 },
	{ 6, 1, 887 },
	{ 6, 1, 888 },
	{ 6, 1, 889 },
	{ 6, 1, 910 },
	{ 6, 1, 1074 },
	{ 6, 1, 1075 },
	{ 6, 1, 1389 },
	{ 6, 1, 1781 },
	{ 6, 1, 2318 },
	{ 6, 1, 2319 },
	{ 6, 1, 2320 },
	{ 6, 1, 2321 },
	{ 6, 1, 2322 },
	{ 6, 1, 2347 },
	{ 6, 1, 2374 },
	{ 6, 1, 2375 },
	{ 6, 1, 2376 },
	{ 6, 1, 2527 },
	{ 6, 1, 2520 },
	{ 6, 1, 2517 },
	{ 6, 1, 2377 },
	{ 6, 1, 2524 },
	{ 6, 1, 2515 },
	{ 6, 1, 2516 },
	{ 6, 1, 2518 },
	{ 6, 1, 2519 },
	{ 6, 1, 2521 },
	{ 6, 1, 2522 },
	{ 6, 1, 2523 },
	{ 6, 1, 2525 },
	{ 6, 1, 2830 },
	{ 6, 1, 2831 },
	{ 6, 1, 2837 },
	{ 6, 1, 30557 },
};

#endif // KLIENDAUTABLES_H
