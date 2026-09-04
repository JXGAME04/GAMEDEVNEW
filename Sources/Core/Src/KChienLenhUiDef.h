// KChienLenhUiDef.h - [CL 04/09 DOT2] hop dong giua Core (client) va S3Client cho cua so CHIEN LENH.
// Khuon y het KAuctionUiDef.h: Core client nhan lenh tu Lua (\script\ui\uichienlenh.lua) roi bao sang UI
// bang CoreDataChanged(GDCNI_CHIENLENH_UI, uParam = CLUI_CMD_*, nParam). UI bao nguoc ve Core bang
// g_pCoreShell->OperationRequest(GOI_CHIENLENH_UI, uParam = CLUI_OP_*, nParam), Core dich thanh loi goi
// Lua UIChienLenh:xxx (KChienLenhClient.cpp). Con tro trong nParam CHI SONG trong loi goi - ben nhan phai chep ra.
//
// VI SAO MOI SO LIEU DI QUA GIAO THUC: Lua phia client KHONG doc duoc bien nhiem vu (GetTask trong
// #ifdef _SERVER; GetBitTask goi GetPlayerIndex(L) doc bien Lua toan cuc ma chi may chu dat -> tra 0 IM LANG),
// va trang thai Chien Lenh nam o MySQL. Nen may chu day xuong, client chi ve.
#ifndef KCHIENLENHUIDEF_H
#define KCHIENLENHUIDEF_H

#include "GameDataDef.h"	// ChatItem

#define CLUI_MAX_AWARD		64
#define CLUI_MAX_MISSION	20
#define CLUI_COL_COUNT		9		// 9 cot moc thuong moi trang (AWARD_ELEM_COUNT cua 2.0)
#define CLUI_ROW_COUNT		4		// hang nhiem vu thay duoc cung luc: khung 173 px / buoc 40 px
#define CLUI_TITLE_LEN		160
#define CLUI_TIPS_LEN		256
#define CLUI_NAME_LEN		64

// nhanh thuong
#define CLUI_BRANCH_LOW		0		// Thuong (mien phi)
#define CLUI_BRANCH_VIP		1		// Hao Hoa
// loai nhiem vu
#define CLUI_KIND_WEEK		1
#define CLUI_KIND_DAY		2
// trang thai nhiem vu
#define CLUI_NV_CHUA		0		// chua xong
#define CLUI_NV_XONG		1		// xong, chua linh
#define CLUI_NV_LINH		2		// da linh

// Core -> UI (GDCNI_CHIENLENH_UI, uParam)
enum CLUI_CMD
{
	CLUI_CMD_OPEN = 1,			// nParam = 0
	CLUI_CMD_CLOSE,
	CLUI_CMD_ICON_VISIBLE,		// nParam = 0/1
	CLUI_CMD_SET_INFO,			// nParam = KCLUiInfo*
	CLUI_CMD_SET_AWARD,			// nParam = KCLUiAward*  (moi moc mot lan)
	CLUI_CMD_SET_MISSION,		// nParam = KCLUiMission* (moi nhiem vu mot lan)
	CLUI_CMD_CLEAR,				// xoa danh sach moc + nhiem vu (truoc khi day lai)
	CLUI_CMD_REFRESH,			// ve lai sau khi day xong
	CLUI_CMD_MSG,				// nParam = const char* (thong bao ngan tren cua so)
};

// UI -> Core (GOI_CHIENLENH_UI, uParam)
enum CLUI_OP
{
	CLUI_OP_OPEN = 1,			// nguoi choi bam bieu tuong -> xin day du lieu
	CLUI_OP_CLOSE,
	CLUI_OP_GET_AWARD,			// nParam = KCLUiReq* {nIdx, nBranch}
	CLUI_OP_GET_MISSION,		// nParam = nId
	CLUI_OP_GOTO_MISSION,		// nParam = nId (nut "Den")
	CLUI_OP_BUY_VIP,			// nut "Mua Chien Lenh"
	CLUI_OP_HELP,
	CLUI_OP_RESET,				// EXIT_GAME
};

struct KCLUiInfo
{
	int		nScore;
	int		nCap;				// diem / level_score
	int		nVip;
	int		nGotLow;			// da nhan nhanh Thuong toi moc thu may
	int		nGotVip;
	int		nSoMoc;
	int		nLevelScore;
	int		nCloseTime;			// unix, de ve dong ho dem nguoc
	int		nDangMo;
	int		nVipPriceXu;
};

struct KCLUiAward
{
	int		nIdx;				// 1..24
	int		nNeedScore;
	int		nCap;
	int		nBranch;			// CLUI_BRANCH_*
	int		nCount;				// so luong (ve nhan so)
	char	szName[CLUI_NAME_LEN];
	ChatItem Item;				// bieu tuong + chu giai that (GDI_ITEM_CHAT), nhu hop thu
};

struct KCLUiMission
{
	int		nId;				// 1..20
	int		nKind;				// CLUI_KIND_*
	int		nScore;
	int		nTarget;
	int		nProg;
	int		nState;				// CLUI_NV_*
	char	szTitle[CLUI_TITLE_LEN];
	char	szTips[CLUI_TIPS_LEN];
};

struct KCLUiReq
{
	int		nA;
	int		nB;
};

#endif // KCHIENLENHUIDEF_H
