// KCongThanhTables.h - SINH TU DONG boi ReverseTools/gen_congthanh_tables.py - DUNG SUA TAY
// Nguon: script song cua may chu (E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/script) + Region_S.dat map 221/222/223
// Marker la RAW TCVN3 trich thang tu Lua (byte-for-byte) - giong KTongKimTables.h.
// O trap doc tu du lieu vung (client KHONG nap trap - KRegion::GetTrap tra 0 ngoai _SERVER).
#ifndef KCONGTHANHTABLES_H
#define KCONGTHANHTABLES_H

// ===== map / NPC / hang so (head.lua, trap.lua, MapList.ini) =====
#define CT_MAP_TRAN	221	// chien truong cong thanh
#define CT_MAP_HP_THU	222	// hau phuong ben THU
#define CT_MAP_HP_CONG	223	// hau phuong ben CONG
#define CT_NPC_CONG	532	// NPC cong thanh (DOORNPCID)
#define CT_NPC_TRU_THU	528	// Long tru ben thu dang giu (STONENPCID1)
#define CT_NPC_TRU_CONG	530	// Long tru ben cong da chiem (STONENPCID2)
#define CT_SO_CONG	3
#define CT_SO_TRU	3
#define CT_NGAY_BANG	5	// phai o trong bang du N ngay moi duoc tham chien (GetJoinTongTime)

// ===== marker hoi thoai / tin (RAW TCVN3) =====
static const char CTM_OPT_DICHIENTRUONG[] = "§i ChiÕn tr­êng c«ng thµnh";
static const char CTM_OPT_CONG[] = "Bªn c«ng";
static const char CTM_OPT_THU[] = "Bªn thñ";
static const char CTM_OPT_KHONGBEN[] = "Kh«ng bªn nµo hÕt";
static const char CTM_SAY_BENCONG[] = "bªn c«ng<";
static const char CTM_SAY_BENTHU[] = "bªn thñ<";
static const char CTM_SAY_THANPHAN[] = "Th©n phËn nghÜa sÜ ch­a phï hîp";
static const char CTM_SAY_TAPHOP[] = "Phe ta hiÖn ®ang tËp hîp chuÈn bÞ vµo ®Êu tr­êng";
static const char CTM_SAY_QUANGAN[] = "Thêi gian b¹n gia nhËp bang héi qu¸ ng¾n";
static const char CTM_SAY_KHONGLENHBAI[] = "Ng­¬i kh«ng cã lÖnh bµi lµm sao vµo ®­îc";
static const char CTM_SAY_DAYNGUOI[] = "®· ®Çy";
static const char CTM_OPT_ROIDAUTRUONG[] = "Rêi khái ®Êu tr­êng";
static const char CTM_OPT_THANHTHI[] = "Nh÷ng thµnh thÞ ®· ®i qua";
static const char CTM_NEWS_KHAICHIEN[] = "c«ng thµnh chiÕn chÝnh thøc b¾t ®Çu";
static const char CTM_NEWS_CHUNGCUOC[] = "Chung cuéc";
static const char CTM_OPT_LOIDAI[] = "Tham gia L«i ®µi bang héi";

// ===== toa do (don vi O; MPS = O * 32) =====
struct CTPoint { short x, y; };
static const CTPoint g_CTDoanhThu  = { 1533, 3211 };	// CS_CampPos1 - doanh ben thu (map 221)
static const CTPoint g_CTDoanhCong = { 1903, 3608 };	// CS_CampPos2 - doanh ben cong
static const CTPoint g_CTRaThu  = { 1571, 3263 };	// ctrap1.lua SetPos - diem ra tran ben thu
static const CTPoint g_CTRaCong = { 1866, 3547 };	// ctrap2.lua SetPos - diem ra tran ben cong
static const CTPoint g_CTNgoai = { 1613, 3185 };	// OuterPos - diem dap tren hau phuong sau tran
// 3 cong thanh (DoorPos, tu MPS) - NPC 532 camp 1, chi ben cong danh duoc
static const CTPoint g_CTCong[CT_SO_CONG] = {
	{ 1796, 3342 },
	{ 1728, 3409 },
	{ 1662, 3473 },
};
// 3 Long tru (StonePos, tu MPS) - NPC 528 (thu giu) / 530 (cong chiem)
static const CTPoint g_CTTru[CT_SO_TRU] = {
	{ 1726, 3279 },
	{ 1661, 3345 },
	{ 1593, 3406 },
};
// Xa Phu tren hau phuong 222/223 (chefu.lua, tu Npc_S.dat)
#define CT_XAPHU_HP_COUNT	2
static const CTPoint g_CTXaPhuHP[CT_XAPHU_HP_COUNT] = {
	{ 1612, 3164 },
	{ 1636, 3191 },
};
// o trap TAP KET tren hau phuong 222/223 (zhongzhuan_map/trap.lua -> JoinCamp -> map 221)
#define CT_TRAPHP_COUNT	32
static const CTPoint g_CTTrapHP[CT_TRAPHP_COUNT] = {
	{ 1573, 3157 },
	{ 1573, 3158 },
	{ 1574, 3156 },
	{ 1574, 3157 },
	{ 1575, 3155 },
	{ 1575, 3156 },
	{ 1576, 3154 },
	{ 1576, 3155 },
	{ 1577, 3153 },
	{ 1577, 3154 },
	{ 1578, 3152 },
	{ 1578, 3153 },
	{ 1579, 3151 },
	{ 1579, 3152 },
	{ 1580, 3150 },
	{ 1580, 3151 },
	{ 1634, 3222 },
	{ 1635, 3221 },
	{ 1635, 3222 },
	{ 1636, 3220 },
	{ 1636, 3221 },
	{ 1637, 3219 },
	{ 1637, 3220 },
	{ 1638, 3218 },
	{ 1638, 3219 },
	{ 1639, 3217 },
	{ 1639, 3218 },
	{ 1640, 3216 },
	{ 1640, 3217 },
	{ 1641, 3215 },
	{ 1641, 3216 },
	{ 1642, 3215 },
};
// o trap cua doanh ben THU tren map 221 (ctrap1/1b/1c.lua -> ra tran)
#define CT_TRAPTHU_COUNT	79
static const CTPoint g_CTTrapThu[CT_TRAPTHU_COUNT] = {
	{ 1534, 3280 },
	{ 1535, 3278 },
	{ 1535, 3279 },
	{ 1535, 3280 },
	{ 1536, 3277 },
	{ 1536, 3278 },
	{ 1537, 3276 },
	{ 1537, 3277 },
	{ 1538, 3275 },
	{ 1538, 3276 },
	{ 1539, 3274 },
	{ 1539, 3275 },
	{ 1540, 3273 },
	{ 1540, 3274 },
	{ 1541, 3272 },
	{ 1541, 3273 },
	{ 1542, 3271 },
	{ 1542, 3272 },
	{ 1543, 3271 },
	{ 1544, 3271 },
	{ 1557, 3270 },
	{ 1558, 3269 },
	{ 1558, 3270 },
	{ 1559, 3268 },
	{ 1559, 3269 },
	{ 1560, 3267 },
	{ 1560, 3268 },
	{ 1561, 3266 },
	{ 1561, 3267 },
	{ 1562, 3265 },
	{ 1562, 3266 },
	{ 1562, 3267 },
	{ 1563, 3264 },
	{ 1563, 3265 },
	{ 1564, 3263 },
	{ 1564, 3264 },
	{ 1565, 3262 },
	{ 1565, 3264 },
	{ 1566, 3263 },
	{ 1572, 3255 },
	{ 1573, 3254 },
	{ 1573, 3255 },
	{ 1574, 3253 },
	{ 1574, 3254 },
	{ 1574, 3255 },
	{ 1575, 3229 },
	{ 1575, 3252 },
	{ 1575, 3253 },
	{ 1575, 3254 },
	{ 1576, 3251 },
	{ 1576, 3252 },
	{ 1576, 3253 },
	{ 1577, 3229 },
	{ 1577, 3250 },
	{ 1577, 3251 },
	{ 1577, 3252 },
	{ 1578, 3229 },
	{ 1578, 3230 },
	{ 1578, 3249 },
	{ 1578, 3250 },
	{ 1578, 3251 },
	{ 1579, 3229 },
	{ 1579, 3230 },
	{ 1579, 3231 },
	{ 1579, 3248 },
	{ 1579, 3249 },
	{ 1579, 3250 },
	{ 1580, 3231 },
	{ 1580, 3232 },
	{ 1580, 3248 },
	{ 1580, 3249 },
	{ 1581, 3232 },
	{ 1581, 3233 },
	{ 1582, 3233 },
	{ 1582, 3234 },
	{ 1583, 3234 },
	{ 1583, 3235 },
	{ 1584, 3235 },
	{ 1585, 3235 },
};
// o trap cua doanh ben CONG tren map 221 (ctrap2/2b/2c.lua -> ra tran)
#define CT_TRAPCONG_COUNT	75
static const CTPoint g_CTTrapCong[CT_TRAPCONG_COUNT] = {
	{ 1856, 3598 },
	{ 1857, 3597 },
	{ 1857, 3598 },
	{ 1858, 3596 },
	{ 1858, 3597 },
	{ 1859, 3569 },
	{ 1859, 3595 },
	{ 1859, 3596 },
	{ 1860, 3568 },
	{ 1860, 3569 },
	{ 1860, 3594 },
	{ 1860, 3595 },
	{ 1861, 3567 },
	{ 1861, 3568 },
	{ 1861, 3569 },
	{ 1861, 3593 },
	{ 1861, 3594 },
	{ 1862, 3566 },
	{ 1862, 3567 },
	{ 1862, 3592 },
	{ 1862, 3593 },
	{ 1863, 3565 },
	{ 1863, 3566 },
	{ 1863, 3591 },
	{ 1863, 3592 },
	{ 1864, 3564 },
	{ 1864, 3565 },
	{ 1864, 3589 },
	{ 1864, 3590 },
	{ 1864, 3591 },
	{ 1865, 3563 },
	{ 1865, 3564 },
	{ 1865, 3590 },
	{ 1865, 3591 },
	{ 1866, 3562 },
	{ 1866, 3563 },
	{ 1867, 3561 },
	{ 1867, 3562 },
	{ 1868, 3560 },
	{ 1868, 3562 },
	{ 1875, 3553 },
	{ 1875, 3554 },
	{ 1876, 3552 },
	{ 1876, 3553 },
	{ 1877, 3551 },
	{ 1877, 3552 },
	{ 1878, 3550 },
	{ 1878, 3551 },
	{ 1879, 3549 },
	{ 1879, 3550 },
	{ 1880, 3548 },
	{ 1880, 3549 },
	{ 1881, 3547 },
	{ 1881, 3548 },
	{ 1882, 3546 },
	{ 1882, 3547 },
	{ 1883, 3546 },
	{ 1901, 3553 },
	{ 1901, 3554 },
	{ 1902, 3552 },
	{ 1902, 3553 },
	{ 1903, 3551 },
	{ 1903, 3552 },
	{ 1904, 3550 },
	{ 1904, 3551 },
	{ 1905, 3549 },
	{ 1905, 3550 },
	{ 1906, 3548 },
	{ 1906, 3549 },
	{ 1907, 3547 },
	{ 1907, 3548 },
	{ 1908, 3546 },
	{ 1908, 3547 },
	{ 1909, 3546 },
	{ 1910, 3546 },
};
// o trap RA KHOI tran ben thu (trap1.lua -> ve 222) - tranh dam vao
#define CT_TRAPRATHU_COUNT	20
static const CTPoint g_CTTrapRaThu[CT_TRAPRATHU_COUNT] = {
	{ 1522, 3207 },
	{ 1522, 3208 },
	{ 1523, 3206 },
	{ 1523, 3207 },
	{ 1524, 3205 },
	{ 1524, 3206 },
	{ 1525, 3204 },
	{ 1525, 3205 },
	{ 1526, 3203 },
	{ 1526, 3204 },
	{ 1527, 3202 },
	{ 1527, 3203 },
	{ 1528, 3201 },
	{ 1528, 3202 },
	{ 1529, 3200 },
	{ 1529, 3201 },
	{ 1530, 3199 },
	{ 1530, 3200 },
	{ 1531, 3198 },
	{ 1531, 3199 },
};
// o trap RA KHOI tran ben cong (trap2.lua -> ve 223) - tranh dam vao
#define CT_TRAPRACONG_COUNT	35
static const CTPoint g_CTTrapRaCong[CT_TRAPRACONG_COUNT] = {
	{ 1648, 3328 },
	{ 1648, 3329 },
	{ 1648, 3330 },
	{ 1648, 3331 },
	{ 1649, 3328 },
	{ 1649, 3329 },
	{ 1649, 3330 },
	{ 1650, 3328 },
	{ 1650, 3329 },
	{ 1651, 3328 },
	{ 1904, 3622 },
	{ 1904, 3623 },
	{ 1904, 3624 },
	{ 1905, 3622 },
	{ 1905, 3623 },
	{ 1906, 3620 },
	{ 1906, 3621 },
	{ 1906, 3622 },
	{ 1907, 3620 },
	{ 1907, 3621 },
	{ 1908, 3618 },
	{ 1908, 3619 },
	{ 1908, 3620 },
	{ 1909, 3617 },
	{ 1909, 3618 },
	{ 1909, 3619 },
	{ 1910, 3616 },
	{ 1910, 3617 },
	{ 1910, 3618 },
	{ 1911, 3615 },
	{ 1911, 3616 },
	{ 1911, 3617 },
	{ 1912, 3615 },
	{ 1912, 3616 },
	{ 1913, 3614 },
};

#endif // KCONGTHANHTABLES_H
