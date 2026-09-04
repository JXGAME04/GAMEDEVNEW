// KAuctionUiDef.h - [DAUGIA 04/09 A3] hop dong giua Core (client) va S3Client cho cua so DAU GIA (viet lai theo VLTK 2.0).
// Core client nhan lenh tu Lua (\script\ui\uiauction_house.lua goi 21 ham C++ giong 2.0: OpenAuctionWindow,
// AuctionAddEnglishItem, ...) roi bao sang UI bang CoreDataChanged(GDCNI_AUCTION_UI, uParam = AUCUI_CMD_*, nParam).
// UI bao nguoc ve Core bang g_pCoreShell->OperationRequest(GOI_AUCTION_UI, uParam = AUCUI_OP_*, nParam = KAucUiReq*),
// Core dich thanh loi goi Lua UIAuctionHouse:xxx trong state cua uiauction_house.lua (KAuctionClient.cpp).
// Con tro trong nParam CHI SONG trong loi goi - ben nhan phai chep ra.
#ifndef KAUCTIONUIDEF_H
#define KAUCTIONUIDEF_H

#include "GameDataDef.h"	// ChatItem

#define AUCUI_NAME_LEN		64
#define AUCUI_ITEM_NAME_LEN	64
#define AUCUI_MAX_ACTIVITY	32
#define AUCUI_MAX_ITEM		20		// AUCTION_DEF.nMaxItemPerPage
#define AUCUI_MAX_MEMBER	64
// [A22 04/09] BA hang cung luc (chu: "hien thi khong du").
// Buoc hang ha xuong 107 trong 4 tep ini hang (muc [Main] Height): noi dung sau nhat trong mot
// hang o y = 106 nen khong de nhau, va ba hang ket thuc o 371 - vua tren nut chuyen trang (374).
// De buoc 118 nhu truoc thi ba hang cham 393 va de len nut do (chu thay la "chen ra ngoai ban").
// So mon moi trang (AUCTION_DEF.nMaxItemPerPage) PHAI bang so nay.
#define AUCUI_ROW_COUNT		3		// hang vat pham hien cung luc (321 / 107)
#define AUCUI_ACT_ROW_COUNT	14		// hang phien hien cung luc (355 / 25)

// loai phien (AUCTION_DEF.tbAuctionTypeEnum)
#define AUCUI_TYPE_TONG		1
#define AUCUI_TYPE_WORLD	2
#define AUCUI_TYPE_PERSONAL	3
// kieu dau (AUCTION_DEF.tbItemTypeEnum)
#define AUCUI_KIND_ENGLISH	1
#define AUCUI_KIND_DUTCH	2
// tien
#define AUCUI_CUR_MONEY		1		// Ngan luong
#define AUCUI_CUR_XU		2		// Xu

struct KAucUiActivity
{
	int		nType;
	char	szName[AUCUI_NAME_LEN];
	int		nStartTime;
};

struct KAucUiItem
{
	int		nType;					// AUCUI_TYPE_*
	int		nKind;					// AUCUI_KIND_*
	int		nId;
	int		nStartTime;
	int		nCurrency;				// AUCUI_CUR_*
	int		bMine;					// 1 = mon cua chinh minh (ca nhan) -> nut "lay lai"
	int		bEnded;					// [A18] mon da ket thuc - giu nguyen cho, chi an nut (dung nhu ban goc)
	// kieu Anh
	int		nGuaranteed;			// gia khoi diem
	int		nRange;					// buoc gia moi lan
	int		nMax;					// gia cao nhat hien tai
	int		nBuyNow;				// [A20] gia MUA NGAY (chi dong ky gui ca nhan; 0 = khong co)
	int		nSelf;					// gia minh da tra
	int		nRemaining;				// giay con lai (tuyet doi: time_t)
	// kieu Ha Lan / ky gui
	int		nCur;					// gia hien tai
	int		nNext;					// gia lan giam sau
	int		nTotalRemaining;		// time_t ket thuc
	// vat pham
	ChatItem	Item;
	char	szName[AUCUI_ITEM_NAME_LEN];
	int		nCount;
	char	szCurrency[32];		// ten loai tien (TCVN3, script gui)
};

struct KAucUiMember
{
	char	szName[AUCUI_NAME_LEN];
	int		nLevel;
	int		nFigure;
	int		bOnline;
};

struct KAucUiSalary
{
	int		nCount;
	int		nSalary;
};

// UI -> Core: du lieu yeu cau (nParam cua GOI_AUCTION_UI)
struct KAucUiReq
{
	int		nType;
	int		nId;
	int		nPrice;
	char	szName[AUCUI_NAME_LEN];
};

// Core -> UI (GDCNI_AUCTION_UI, uParam)
enum AUCUI_CMD
{
	AUCUI_CMD_OPEN = 1,				// nParam = 0
	AUCUI_CMD_CLOSE,
	AUCUI_CMD_SWITCH,				// nParam = nType
	AUCUI_CMD_NEW_EVENT,			// nhap nhay bieu tuong
	AUCUI_CMD_ADD_ACTIVITY,			// nParam = KAucUiActivity*
	AUCUI_CMD_DEL_ACTIVITY,			// nParam = KAucUiActivity* (chi ten)
	AUCUI_CMD_CLEAR_ACTIVITY,
	AUCUI_CMD_SELECT_ACTIVITY,		// nParam = KAucUiActivity* (chi ten)
	AUCUI_CMD_ADD_ITEM,				// nParam = KAucUiItem*
	AUCUI_CMD_SET_ITEM,				// nParam = KAucUiItem*
	AUCUI_CMD_END_ITEM,				// nParam = nId
	AUCUI_CMD_CLEAR_ITEM,
	AUCUI_CMD_ADD_MEMBER,			// nParam = KAucUiMember*
	AUCUI_CMD_CLEAR_MEMBER,
	AUCUI_CMD_SET_SALARY,			// nParam = KAucUiSalary*
	AUCUI_CMD_SET_PAGE_TXT,			// nParam = const char*
	AUCUI_CMD_CLEAR_ALL,
	AUCUI_CMD_ICON_VISIBLE,			// nParam = 0/1
	AUCUI_CMD_SET_MONEY,			// nParam = KAucUiSalary* (nCount = Ngan luong, nSalary = Xu) hien o tab ca nhan
	// [A6] bat/tat che do KY GUI cho hop dua vat pham (KUiAffairItem): nParam = 0/1
	AUCUI_CMD_PUTON_MODE,
};

// UI -> Core (GOI_AUCTION_UI, uParam = op, nParam = KAucUiReq* hoac so)
enum AUCUI_OP
{
	AUCUI_OP_ICON_CLICK = 1,		// nParam = 1 mo / 0 dong           -> UIAuctionHouse:OnAuctionIconClick(n)
	AUCUI_OP_PAGE_BTN,				// nParam = nType                    -> OnPageBtnClick(nType)
	AUCUI_OP_SELECT_ACTIVITY,		// nParam = KAucUiReq* (szName)      -> OnSelect(szName)
	AUCUI_OP_PREV_PAGE,				//                                   -> OnClickPrevPageBtn()
	AUCUI_OP_NEXT_PAGE,				//                                   -> OnClickNextPageBtn()
	AUCUI_OP_OFFER_ENGLISH,			// nParam = KAucUiReq* (nId, nPrice) -> RequestOfferEnglishPrice(nId, nPrice)
	AUCUI_OP_OFFER_DUTCH,			// nParam = KAucUiReq* (nId, nPrice) -> RequestOfferDutchPrice(nId, nPrice)
	AUCUI_OP_GET_BACK,				// nParam = KAucUiReq* (nId)         -> RequestGetBackItem(nId)
	AUCUI_OP_REFUND,				// nParam = KAucUiReq* (nId)         -> RequestRefund(nId)
	AUCUI_OP_MEMBER_LIST,			//                                   -> RequestMemberList()
	AUCUI_OP_CONFIRM_RESULT,		// nParam = 1 dong y / 0 huy         -> goi ham Lua da dat khi AuctionConfirm
	AUCUI_OP_CLOSE,					// cua so bi dong                    -> OnWindowClosed()
	AUCUI_OP_RESET,					// thoat game                        -> UIAuctionHouse:Reset()
	AUCUI_OP_TICK,					// moi giay khi cua so mo            -> UIAuctionHouse:OnTick()
	AUCUI_OP_PUT_ON,				// nParam = nType (nut goc phai)     -> UIAuctionHouse:OnPutOnClick(nType)
	AUCUI_OP_SET_PRICE,				// nParam = KAucUiReq* (nPrice, nType = loai tien) -> UIAuctionHouse:OnSetPrice(nPrice, nCur)
};

#endif // KAUCTIONUIDEF_H
