// ============================================================================
// KTuiDuocPham.h - SINH TU DONG boi ReverseTools/gen_tuiduocpham.py - DUNG SUA TAY.
// Danh sach BAN DO KHONG MO DUOC "Tui duoc pham" (genre 6 / detail 1 /
// particular 4813) - doc thang tu ham main() cua
//   bin\server\script\item\tuiduocpham.lua
// (script do liet ke tay chu KHONG goi checkSJMaps, nen phai lay dung tu no).
//
// Dung o ATYPE_OPENBAG: dang dung tren map trong danh sach nay thi ImKHONG gui
// lenh mo - khong thi cu 3 giay lai an mot cau "Ban do hien tai... khong the mo!".
// ============================================================================
#ifndef KTUIDUOCPHAM_H
#define KTUIDUOCPHAM_H

#define TUIDP_ITEM_G	6
#define TUIDP_ITEM_D	1
#define TUIDP_ITEM_P	4813

#define TUIDP_CAM_COUNT	7
static const short g_TuiDPCam[TUIDP_CAM_COUNT][2] = {
	{ 44, 44 },
	{ 175, 175 },
	{ 197, 197 },
	{ 208, 223 },
	{ 336, 339 },
	{ 341, 342 },
	{ 375, 399 },
};

// 1 = ban do nay KHONG mo duoc tui duoc pham
static inline int TuiDP_CamMap(int nMapId)
{
	for (int i = 0; i < TUIDP_CAM_COUNT; ++i)
		if (nMapId >= (int)g_TuiDPCam[i][0] && nMapId <= (int)g_TuiDPCam[i][1])
			return 1;
	return 0;
}

#endif // KTUIDUOCPHAM_H
