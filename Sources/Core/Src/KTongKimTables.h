// KTongKimTables.h - SINH TU DONG boi ReverseTools/gen_tongkim_tables.py - DUNG SUA TAY
// Nguon: script song cua may chu (E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/script)
// Marker la RAW TCVN3 trich thang tu Lua (byte-for-byte) - giong KDaTauTables.h.
#ifndef KTONGKIMTABLES_H
#define KTONGKIMTABLES_H

// ===== hang so tran (lib_tktc.lua) =====
#define TK_MAP_BAODANH	324	// map diem bao danh
#define TK_MAP_TRAN		379	// map chien truong
#define TK_LEVEL_MIN	80	// cap toi thieu de bao danh
#define TK_TIME_TRAI	90	// giay toi da o hau doanh (dong ho tu nem ra tran)
#define TK_TIME_TRAP	10	// giay dau trap tu choi cho qua

// ===== marker hoi thoai =====
static const char TKM_OPT_TONG[] = "Ta muèn ®Çu qu©n cho phe Tèng";
static const char TKM_OPT_KIM[] = "Ta muèn ®Çu qu©n cho phe kim";
static const char TKM_SAY_TONG[] = "Tèng: ";
static const char TKM_SAY_KIM[] = "Kim: ";
static const char TKM_OPT_TROLAI[] = "Trë l¹i chç lóc n·y";
static const char TKM_OPT_THANHTHI[] = "Nh÷ng thµnh thÞ ®· ®i qua";
static const char TKM_OPT_QUATONG[] = "§Õn ®iÓm b¸o danh phe Tèng (T)";
static const char TKM_OPT_QUAKIM[] = "§Õn ®iÓm b¸o danh phe Kim (K)";
static const char TKM_OPT_MUANHANH[] = "Mua nhanh Ngò Hoa Ngäc Lé Hoµn";
static const char TKM_OPT_MUADUOC[] = "Ta muèn mua d­îc phÈm";
static const char TKM_OPT_CHDIEM[] = "Cöa hµng ®iÓm Tèng Kim";
static const char TKM_MSG_BAODANH[] = "B¸o danh Tèng Kim ®· b¾t ®Çu thêi gian b¸o danh trong vßng ";
static const char TKM_MSG_KHOIDONG[] = "Tèng kim ®· ®­îc khëi ®éng ph­¬ng thøc ";
static const char TKM_MSG_SUKIENBD[] = "®ang ë giai ®o¹n b¸o danh";

// ===== khung gio mo tran (gio, phut) - theo dong ho MAY CHU =====
#define TK_GIO_COUNT	4
static const short g_TKGio[TK_GIO_COUNT][2] = {
	{ 12, 21 },
	{ 17, 50 },
	{ 20, 50 },
	{ 22, 50 },
};

// ===== toa do (don vi O; MPS = O * 32) =====
struct TKPoint { short x, y; };

// diem dap khi dung Tong Kim Chieu Thu (item/battles/rescript.lua)
static const TKPoint g_TKDapTong = { 1529, 3196 };
static const TKPoint g_TKDapKim  = { 1592, 3075 };
// NPC tren map bao danh (startgame.lua:84-96)
static const TKPoint g_TKNpcBdTong = { 1550, 3179 };	// Tong Binh Bao Danh (res 62)
static const TKPoint g_TKNpcBdKim  = { 1555, 3082 };	// Kim Binh Bao Danh (res 61)
static const TKPoint g_TKXaFuTong  = { 1535, 3153 };	// Xa Phu phe Tong (res 235)
static const TKPoint g_TKXaFuKim   = { 1568, 3075 };	// Xa Phu phe Kim (res 235)
static const TKPoint g_TKShopTong  = { 1546, 3158 };	// Quan Nhu Quan Tong (res 55)
static const TKPoint g_TKShopKim   = { 1580, 3074 };	// Quan Nhu Quan Kim (res 49)
// hau doanh 2 phe tren map tran (TAB_PHE_TONGKIM[..][9]) - DAO theo the tran
static const TKPoint g_TKHauDoanhA = { 1229, 3561 };
static const TKPoint g_TKHauDoanhB = { 1689, 3074 };
// Quan Y ban thuoc canh hau doanh (TKPOS_NPC_DIALOG, doi tu MPS ve O)
static const TKPoint g_TKQuanYA = { 1249, 3557 };
static const TKPoint g_TKQuanYB = { 1700, 3066 };
// vet trap RA TRAI (addtraptongkimtrungcap) - goc + 10 o cheo
static const TKPoint g_TKTrapA = { 1251, 3529 };
static const TKPoint g_TKTrapB = { 1661, 3098 };
#define TK_TRAP_LEN	10	// vet cheo dai 10 o (tam vet = goc + 5)

// 8 diem xuat quan cua moi ben (trap/tongratrai.lua, trap/kimratrai.lua)
#define TK_XQ_COUNT	8
static const TKPoint g_TKXuatQuanA[TK_XQ_COUNT] = {
	{ 1308, 3461 },
	{ 1311, 3442 },
	{ 1324, 3428 },
	{ 1337, 3420 },
	{ 1352, 3419 },
	{ 1350, 3439 },
	{ 1342, 3463 },
	{ 1321, 3468 },
};
static const TKPoint g_TKXuatQuanB[TK_XQ_COUNT] = {
	{ 1585, 3181 },
	{ 1570, 3173 },
	{ 1551, 3175 },
	{ 1544, 3204 },
	{ 1550, 3223 },
	{ 1569, 3227 },
	{ 1586, 3216 },
	{ 1592, 3198 },
};

// bang toa do binh doan (lib_tktc.lua) - dich di chuyen khi khong co dich quanh minh
#define TK_BINHA_COUNT	78
static const TKPoint g_TKBinhA[TK_BINHA_COUNT] = {
	{ 1368, 3391 },
	{ 1369, 3387 },
	{ 1370, 3383 },
	{ 1376, 3388 },
	{ 1381, 3384 },
	{ 1383, 3374 },
	{ 1383, 3367 },
	{ 1391, 3365 },
	{ 1396, 3363 },
	{ 1394, 3355 },
	{ 1396, 3348 },
	{ 1405, 3346 },
	{ 1403, 3357 },
	{ 1406, 3363 },
	{ 1414, 3365 },
	{ 1422, 3372 },
	{ 1426, 3379 },
	{ 1418, 3384 },
	{ 1414, 3379 },
	{ 1412, 3396 },
	{ 1419, 3403 },
	{ 1421, 3412 },
	{ 1416, 3420 },
	{ 1409, 3415 },
	{ 1403, 3420 },
	{ 1400, 3426 },
	{ 1403, 3433 },
	{ 1400, 3442 },
	{ 1397, 3450 },
	{ 1398, 3457 },
	{ 1391, 3462 },
	{ 1383, 3459 },
	{ 1377, 3462 },
	{ 1381, 3472 },
	{ 1381, 3480 },
	{ 1376, 3483 },
	{ 1367, 3478 },
	{ 1366, 3478 },
	{ 1360, 3487 },
	{ 1359, 3492 },
	{ 1351, 3497 },
	{ 1344, 3499 },
	{ 1346, 3494 },
	{ 1351, 3487 },
	{ 1354, 3482 },
	{ 1300, 3410 },
	{ 1303, 3402 },
	{ 1308, 3396 },
	{ 1306, 3388 },
	{ 1311, 3383 },
	{ 1317, 3380 },
	{ 1319, 3371 },
	{ 1320, 3363 },
	{ 1329, 3361 },
	{ 1338, 3357 },
	{ 1344, 3352 },
	{ 1352, 3349 },
	{ 1348, 3340 },
	{ 1335, 3350 },
	{ 1352, 3340 },
	{ 1359, 3340 },
	{ 1360, 3329 },
	{ 1368, 3328 },
	{ 1374, 3331 },
	{ 1379, 3326 },
	{ 1385, 3320 },
	{ 1394, 3319 },
	{ 1404, 3318 },
	{ 1411, 3327 },
	{ 1407, 3336 },
	{ 1401, 3339 },
	{ 1393, 3332 },
	{ 1385, 3333 },
	{ 1388, 3350 },
	{ 1392, 3356 },
	{ 1400, 3355 },
	{ 1407, 3361 },
	{ 1403, 3366 },
};
#define TK_BINHB_COUNT	139
static const TKPoint g_TKBinhB[TK_BINHB_COUNT] = {
	{ 1434, 3331 },
	{ 1436, 3320 },
	{ 1437, 3303 },
	{ 1444, 3302 },
	{ 1447, 3314 },
	{ 1444, 3326 },
	{ 1441, 3335 },
	{ 1441, 3344 },
	{ 1444, 3353 },
	{ 1451, 3354 },
	{ 1453, 3345 },
	{ 1452, 3336 },
	{ 1458, 3333 },
	{ 1463, 3338 },
	{ 1471, 3344 },
	{ 1477, 3342 },
	{ 1475, 3332 },
	{ 1470, 3323 },
	{ 1464, 3318 },
	{ 1461, 3308 },
	{ 1459, 3300 },
	{ 1451, 3291 },
	{ 1450, 3281 },
	{ 1453, 3271 },
	{ 1458, 3269 },
	{ 1466, 3273 },
	{ 1476, 3275 },
	{ 1480, 3281 },
	{ 1478, 3290 },
	{ 1477, 3297 },
	{ 1482, 3299 },
	{ 1491, 3297 },
	{ 1494, 3306 },
	{ 1491, 3314 },
	{ 1485, 3323 },
	{ 1483, 3332 },
	{ 1485, 3339 },
	{ 1493, 3340 },
	{ 1497, 3333 },
	{ 1497, 3323 },
	{ 1499, 3312 },
	{ 1506, 3304 },
	{ 1514, 3306 },
	{ 1519, 3313 },
	{ 1517, 3323 },
	{ 1512, 3331 },
	{ 1510, 3336 },
	{ 1518, 3334 },
	{ 1523, 3328 },
	{ 1525, 3318 },
	{ 1530, 3311 },
	{ 1534, 3320 },
	{ 1539, 3324 },
	{ 1542, 3316 },
	{ 1544, 3310 },
	{ 1549, 3317 },
	{ 1552, 3321 },
	{ 1557, 3318 },
	{ 1558, 3309 },
	{ 1559, 3303 },
	{ 1563, 3309 },
	{ 1568, 3312 },
	{ 1571, 3306 },
	{ 1571, 3296 },
	{ 1578, 3298 },
	{ 1581, 3304 },
	{ 1587, 3300 },
	{ 1590, 3295 },
	{ 1588, 3288 },
	{ 1585, 3282 },
	{ 1587, 3275 },
	{ 1592, 3272 },
	{ 1598, 3277 },
	{ 1602, 3282 },
	{ 1607, 3279 },
	{ 1607, 3270 },
	{ 1606, 3261 },
	{ 1607, 3254 },
	{ 1613, 3253 },
	{ 1619, 3253 },
	{ 1620, 3249 },
	{ 1617, 3241 },
	{ 1614, 3236 },
	{ 1606, 3243 },
	{ 1609, 3248 },
	{ 1609, 3239 },
	{ 1539, 3165 },
	{ 1535, 3157 },
	{ 1539, 3152 },
	{ 1540, 3144 },
	{ 1535, 3140 },
	{ 1530, 3145 },
	{ 1527, 3153 },
	{ 1523, 3158 },
	{ 1518, 3159 },
	{ 1516, 3154 },
	{ 1518, 3146 },
	{ 1512, 3142 },
	{ 1507, 3147 },
	{ 1507, 3154 },
	{ 1509, 3161 },
	{ 1509, 3169 },
	{ 1505, 3172 },
	{ 1500, 3169 },
	{ 1496, 3177 },
	{ 1496, 3186 },
	{ 1492, 3190 },
	{ 1487, 3185 },
	{ 1482, 3190 },
	{ 1485, 3197 },
	{ 1488, 3201 },
	{ 1487, 3207 },
	{ 1482, 3207 },
	{ 1477, 3202 },
	{ 1472, 3204 },
	{ 1469, 3213 },
	{ 1474, 3217 },
	{ 1477, 3223 },
	{ 1474, 3229 },
	{ 1469, 3228 },
	{ 1462, 3229 },
	{ 1461, 3238 },
	{ 1465, 3245 },
	{ 1470, 3252 },
	{ 1472, 3257 },
	{ 1477, 3260 },
	{ 1485, 3265 },
	{ 1490, 3272 },
	{ 1493, 3278 },
	{ 1497, 3283 },
	{ 1501, 3279 },
	{ 1502, 3271 },
	{ 1505, 3265 },
	{ 1511, 3265 },
	{ 1513, 3261 },
	{ 1514, 3256 },
	{ 1521, 3257 },
	{ 1524, 3257 },
	{ 1523, 3249 },
};

// ===== vat pham =====
// Tong Kim Chieu Thu (magicscript.txt) - vao map bao danh, TRU 1 cai moi lan dung
#define TK_ITEM_THU_G	6
#define TK_ITEM_THU_D	1
#define TK_ITEM_THU_P	154
// binh thuoc NPC Quan Y ban bang dong 'Mua nhanh' (quany.lua: AddItem(1,2,0,5,...))
#define TK_ITEM_MAU_G	1
#define TK_ITEM_MAU_D	2
#define TK_ITEM_MAU_P	0
#define TK_ITEM_MAU_L	5
// thuoc hoat dong Tong Kim: genre 6 / detail 1 / particular 177..194, moi vien 3 phut,
// chi dung duoc tren map tran (script/header/forbidmap.lua: checkSJMaps).
// cot 2: 1 = tang cong, 2 = phong thu, 3 = mau / toc do (loc theo o cau hinh)
#define TK_PILL_COUNT	18
static const short g_TKPill[TK_PILL_COUNT][2] = {
	{ 177, 1 },
	{ 178, 1 },
	{ 179, 1 },
	{ 180, 1 },
	{ 181, 1 },
	{ 182, 1 },
	{ 183, 1 },
	{ 184, 1 },
	{ 185, 3 },
	{ 186, 3 },
	{ 187, 2 },
	{ 188, 2 },
	{ 189, 1 },
	{ 190, 2 },
	{ 191, 2 },
	{ 192, 2 },
	{ 193, 2 },
	{ 194, 2 },
};

// res id NPC quan quan tren chien truong (lib_tktc.lua addnpcquaitktrungcap):
// Hieu Uy 632/638, Pho Tuong 634/640, Dai Tuong 635/641.
// CHU Y: server dang hardcode the thuc Cuu Sat nen hien KHONG spawn NPC nao.
#define TK_QUAN_COUNT	6
static const short g_TKQuanRes[TK_QUAN_COUNT] = { 632, 638, 634, 640, 635, 641 };

#endif // KTONGKIMTABLES_H
