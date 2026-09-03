// KMailUiDef.h - [MAIL 03/09] hop dong giua Core (client) va S3Client cho cua so THU (viet lai theo VLTK 2.0).
// Core client nhan lenh tu Lua (uimail.lua goi 15 ham C++ giong 2.0: OpenMailWindow, AddMailHeader, ...)
// roi bao sang UI bang CoreDataChanged(GDCNI_MAIL_UI, uParam = MAILUI_CMD_*, nParam = con tro / so).
// UI bao nguoc ve Core bang g_pCoreShell->OperationRequest(GOI_MAIL_UI, uParam = MAILUI_OP_*, nParam),
// Core dich thanh loi goi Lua UIMail:xxx trong state \script\ui\uimail.lua (KMailClient.cpp).
// Con tro trong nParam CHI SONG trong loi goi - ben nhan phai chep ra.
#ifndef KMAILUIDEF_H
#define KMAILUIDEF_H

#include "GameDataDef.h"	// ChatItem

#define MAILUI_MAX_AWARD		8
#define MAILUI_MAX_MAIL			100		// MAILDEF.PLAYER_MAX_MAIL cua 2.0
#define MAILUI_SENDER_LEN		64
#define MAILUI_TITLE_LEN		128
#define MAILUI_TIME_LEN			32
#define MAILUI_ICON_LEN			128
#define MAILUI_AWARD_NAME_LEN	96
#define MAILUI_AWARD_DESC_LEN	512
#define MAILUI_CONTENT_LEN		2048

// Core -> UI (GDCNI_MAIL_UI, uParam)
enum MAILUI_CMD
{
	MAILUI_CMD_ICON_VISIBLE = 1,	// nParam = 0/1
	MAILUI_CMD_OPEN,				// nParam = nType (chua dung)
	MAILUI_CMD_ADD_HEADER,			// nParam = KMailUiHeader*
	MAILUI_CMD_SET_HEADER,			// nParam = KMailUiHeader*
	MAILUI_CMD_DEL_ONE,				// nParam = nId
	MAILUI_CMD_CLEAN_LIST,
	MAILUI_CMD_CLEAN_DETAIL,
	MAILUI_CMD_CLEAN_ALL,
	MAILUI_CMD_UPDATE_COUNT,
	MAILUI_CMD_BTN_STATUS,			// nParam = KMailUiBtnStatus*
	MAILUI_CMD_UPDATE_DETAIL,		// nParam = KMailUiDetail*
	MAILUI_CMD_NEW_MAIL_BLINK,		// nhap nhay bieu tuong
	MAILUI_CMD_SELECT,				// nParam = nId
	MAILUI_CMD_SWITCH_MANAGER,		// mo/dong cua so
	MAILUI_CMD_SET_FILTER_TEXT,		// nParam = chi so bo loc 1-based
	MAILUI_CMD_CONFIRM,				// nParam = KMailUiConfirm* (hop xac nhan Co/Khong)
	MAILUI_CMD_CLOSE,
};

// UI -> Core (GOI_MAIL_UI, uParam = op | (nExtra << 16), nParam = nId hoac gia tri)
enum MAILUI_OP
{
	MAILUI_OP_SELECT = 1,			// nParam = nId               -> UIMail:OnSelect(nId)
	MAILUI_OP_CHECK,				// nParam = nId, extra = state(bit0) | manual(bit1) -> UIMail:OnMailCheck
	MAILUI_OP_DELETE_ONE,			//                            -> UIMail:RequestDeleteMail()
	MAILUI_OP_DELETE_CHECKED,		//                            -> UIMail:RequestDeleteAllCheckedMail()
	MAILUI_OP_ACCEPT,				// nParam = nId               -> UIMail:RequestChangeState(nId, DRAWED)
	MAILUI_OP_FILTER,				// nParam = chi so 0-based    -> UIMail:OnMailFilterSelected(idx)
	MAILUI_OP_AUTO_DELETE,			// nParam = 0/1               -> UIMail:RequestAutoDeleteMails()
	MAILUI_OP_UPDATE,				//                            -> UIMail:OnUpdate()
	MAILUI_OP_ICON_CLICK,			//                            -> UIMail:OnMailIconClick()
	MAILUI_OP_REQUEST_LIST,			//                            -> UIMail:RequestMailHeaderList()
	MAILUI_OP_CHECK_ID_ON_OPEN,		// nParam = nId               -> UIMail:CheckIdOnOpen(nId)
	MAILUI_OP_CONFIRM_RESULT,		// nParam = 1 dong y / 0 huy  -> goi ham Lua da dat khi MailConfirm
	MAILUI_OP_CLOSE,				// cua so bi dong
};

#define MAILUI_OP_CODE(u)		((u) & 0xFFFF)
#define MAILUI_OP_EXTRA(u)		(((u) >> 16) & 0xFFFF)
#define MAILUI_OP_MAKE(op, x)	(((unsigned int)(op) & 0xFFFF) | (((unsigned int)(x) & 0xFFFF) << 16))

struct KMailUiHeader
{
	int		nId;
	char	szSender[MAILUI_SENDER_LEN];
	char	szTitle[MAILUI_TITLE_LEN];
	int		bRead;			// 1 = da doc
	int		bHasAward;		// 1 = con dinh kem chua nhan
	int		nExpiredTime;	// giay (time_t) het han
	int		nSendTime;		// giay (time_t) gui
};

// Loai phan thuong (uimail.lua PackAwardInfo -> KMailClient.cpp):
//   "icon|duong dan spr|ten|mo ta|so luong"            -> MAILAWARD_ICON (Ngan luong / xu / EXP)
//   "item|genre|detail|particular|level|series|luck|so luong" -> MAILAWARD_ITEM (client dung lai vat pham tam
//   bang duong GDI_ITEM_CHAT -> bieu tuong + chu giai that, y het KUiDiceItem)
#define MAILAWARD_ICON		0
#define MAILAWARD_ITEM		1

struct KMailUiAward
{
	int		nKind;						// MAILAWARD_*
	char	szIcon[MAILUI_ICON_LEN];	// duong dan .spr (kind ICON)
	char	szName[MAILUI_AWARD_NAME_LEN];
	char	szDesc[MAILUI_AWARD_DESC_LEN];
	int		nCount;
	ChatItem	Item;					// kind ITEM: mo ta de client dung lai (GameDataDef.h)
};

struct KMailUiDetail
{
	int		nId;
	char	szSender[MAILUI_SENDER_LEN];
	char	szTitle[MAILUI_TITLE_LEN];
	char	szTime[MAILUI_TIME_LEN];
	int		bHasAward;
	int		nAwardCount;
	char	szContent[MAILUI_CONTENT_LEN];
	KMailUiAward	Award[MAILUI_MAX_AWARD];
};

struct KMailUiBtnStatus
{
	int		nAcceptShow, nAcceptEnable;
	int		nDelOneShow, nDelOneEnable;
	int		nDelAllShow, nDelAllEnable;
};

struct KMailUiConfirm
{
	char	szText[512];
	char	szYes[64];		// nhan nut dong y (TCVN3, tu Lua)
	char	szNo[64];		// nhan nut huy
};

#endif // KMAILUIDEF_H
