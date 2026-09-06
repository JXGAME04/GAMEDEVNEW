// ============================================================================
// KVanTieuPos.h - SINH TU DONG boi ReverseTools/vantieu/gen_vantieu_chinam.py tu
// script/activitysys/config/129/extend.lua (tbBJPoints, tbBJPathLevel) va
// script/startgame/lmbj_addnpc.lua cua may chu - DUNG SUA TAY. Chi CLIENT dung
// (dan duong F11 muc Van tieu: CoreShell.cpp TG_VanTieu* + UiTaskGuide.cpp).
// ============================================================================
#ifndef KVANTIEU_POS_H
#define KVANTIEU_POS_H

#define VT_TPL_TIEUSU     2145   // Long Mon Tieu Su (npcs.txt dong 2146)
#define VT_TPL_CHUONGQUY  2157   // Ong chu Tieu cuc Luc Tam Can
#define VT_TPL_TIEPDAN    2230   // Tiep Dan ap Tieu Bang (7 thanh)
#define VT_TPL_NHANHANG   2231   // Nhan Hang ap Tieu Bang (23 hang)
#define VT_MAP_LONGMON    121
#define VT_CQ_X           1989    // o cell cua Ong chu Tieu cuc
#define VT_CQ_Y           4476

// tbBJPoints: 1..14 = Tieu Su (nhanh ca nhan); 15..44 = diem nhanh bang (Tiep Dan + Nhan Hang)
#define VT_DIEM_SO 44
static const short s_nVTDiemMap[VT_DIEM_SO + 1] = { 0, 176, 176, 11, 20, 78, 101, 80, 162, 153, 1, 99, 37, 100, 174, 80, 116, 93, 94, 176, 181, 180, 182, 37, 42, 45, 204, 103, 11, 10, 23, 22, 5, 78, 91, 83, 205, 1, 201, 2, 4, 162, 168, 171, 203 };
static const short s_nVTDiemX[VT_DIEM_SO + 1] = { 0, 1448, 1534, 3061, 3516, 1537, 1625, 1786, 1639, 1677, 1597, 1673, 1690, 1622, 1596, 1678, 1741, 1620, 1638, 1399, 1627, 1548, 2107, 1703, 1642, 1702, 1621, 1638, 3205, 1778, 1854, 1865, 1597, 1569, 1602, 1660, 1587, 1559, 1736, 2406, 1672, 1650, 1653, 1494, 1505 };
static const short s_nVTDiemY[VT_DIEM_SO + 1] = { 0, 3335, 2974, 4992, 6164, 3182, 3209, 3037, 3215, 3240, 3093, 3229, 3150, 3113, 3271, 3147, 3031, 3182, 3106, 3140, 3052, 3176, 3412, 3019, 3029, 3076, 3375, 2930, 5078, 3065, 3123, 3274, 3592, 3280, 2810, 2924, 3221, 3258, 3235, 3713, 3096, 3247, 3138, 2982, 3102 };

// tbBJPathLevel: 1..26 = tuyen ca nhan (sao 1..9); 27..49 = tuyen bang (10 sao)
#define VT_TUYEN_SO 49
static const unsigned char s_nVTTuyenDau[VT_TUYEN_SO + 1] = { 0, 1, 3, 5, 7, 8, 10, 12, 7, 1, 12, 12, 12, 12, 10, 5, 5, 10, 3, 10, 3, 7, 6, 7, 6, 10, 10, 15, 15, 15, 19, 19, 19, 23, 23, 23, 23, 28, 28, 28, 28, 33, 33, 33, 37, 37, 37, 41, 41, 41 };
static const unsigned char s_nVTTuyenCuoi[VT_TUYEN_SO + 1] = { 0, 2, 4, 6, 6, 9, 11, 13, 5, 14, 5, 6, 5, 6, 3, 13, 13, 4, 11, 4, 11, 12, 13, 12, 13, 5, 5, 16, 17, 18, 20, 21, 22, 24, 25, 26, 27, 29, 30, 31, 32, 34, 35, 36, 38, 39, 40, 42, 43, 44 };
static const unsigned char s_nVTTuyenSao[VT_TUYEN_SO + 1] = { 0, 1, 2, 2, 2, 2, 2, 2, 3, 3, 4, 4, 5, 5, 5, 5, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10 };

// 7 diem Tiep Dan ap Tieu Bang (chi so trong tbBJPoints)
#define VT_TIEPDAN_SO 7
static const unsigned char s_nVTTiepDanDiem[VT_TIEPDAN_SO] = { 15, 19, 23, 28, 33, 37, 41 };

#endif
