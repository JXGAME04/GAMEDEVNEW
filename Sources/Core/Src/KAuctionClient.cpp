// KAuctionClient.cpp - [DAUGIA 04/09 A3] cau noi Lua <-> cua so DAU GIA phia client (viet lai theo VLTK 2.0).
// \script\ui\uiauction_house.lua (chep tu client JX1 cu, sua cho JX1) goi cac ham C++ nay y het 2.0; ta doi thanh
// thong bao GDCNI_AUCTION_UI sang S3Client (UiAuction.cpp). Chieu nguoc: CoreShell.cpp case GOI_AUCTION_UI ->
// AuctionUi_OnRequest -> chay "UIAuctionHouse:xxx(...)" trong state cua uiauction_house.lua (SP_RunClientLua).
// Hop dong: KAuctionUiDef.h. Khac 2.0: vat pham truyen bang 6 so (genre,detail,particular,level,series,luck)
// -> ChatItem de client dung lai bieu tuong + chu giai that (GDI_ITEM_CHAT), giong hop thu.
#include "KCore.h"
#include "KWin32.h"
#include "KEngine.h"
#include "KDebug.h"
#include "LuaLib.h"
#include "KAuctionUiDef.h"
#include "KScriptProtocol.h"
#include "KAuctionClient.h"

#ifndef _SERVER
#include "CoreShell.h"
#include "GameDataDef.h"
#include "KSubWorldSet.h"	// GetGameVersion cho ChatItem.m_wVersion
#include <stdio.h>
#include <string.h>
#include <time.h>

extern int CoreDataChanged(unsigned int uDataId, unsigned int uParam, int nParam);

#define UIAUCTION_SCRIPT	"\\script\\ui\\uiauction_house.lua"

static int  s_bOpen = 0;
static char s_szConfirmFunc[64] = "";

static void sCopyStr(char* pDst, int nSize, const char* pSrc)
{
	if (!pDst || nSize <= 0)
		return;
	if (!pSrc)
	{
		pDst[0] = 0;
		return;
	}
	strncpy(pDst, pSrc, nSize - 1);
	pDst[nSize - 1] = 0;
}

static const char* sArgStr(Lua_State* L, int n)
{
	return (Lua_GetTopIndex(L) >= n && Lua_IsString(L, n)) ? Lua_ValueToString(L, n) : "";
}

static int sArgInt(Lua_State* L, int n)
{
	return (Lua_GetTopIndex(L) >= n && Lua_IsNumber(L, n)) ? (int)Lua_ValueToNumber(L, n) : 0;
}

static void sNotify(int nCmd, int nParam)
{
	SP_ClientLog("[AUCUI] Lua->UI cmd=%d param=%d", nCmd, nParam);
	CoreDataChanged(GDCNI_AUCTION_UI, (unsigned int)nCmd, nParam);
}

int AuctionUi_IsOpen()
{
	return s_bOpen;
}

void AuctionUi_SetOpen(int bOpen)
{
	s_bOpen = bOpen ? 1 : 0;
}

// ---------------------------------------------------------------- cua so
int LuaAuc_OpenAuctionWindow(Lua_State* L)
{
	s_bOpen = 1;
	sNotify(AUCUI_CMD_OPEN, 0);
	return 0;
}

int LuaAuc_CloseAuctionWindow(Lua_State* L)
{
	s_bOpen = 0;
	sNotify(AUCUI_CMD_CLOSE, 0);
	return 0;
}

int LuaAuc_SwitchAuctionWindow(Lua_State* L)
{
	sNotify(AUCUI_CMD_SWITCH, sArgInt(L, 1));
	return 0;
}

int LuaAuc_AuctionUiIsOpen(Lua_State* L)
{
	Lua_PushNumber(L, s_bOpen);
	return 1;
}

int LuaAuc_NewAuctionEventArrival(Lua_State* L)
{
	sNotify(AUCUI_CMD_NEW_EVENT, 0);
	return 0;
}

int LuaAuc_SetAuctionIconVisible(Lua_State* L)
{
	sNotify(AUCUI_CMD_ICON_VISIBLE, sArgInt(L, 1));
	return 0;
}

// ---------------------------------------------------------------- phien
// AuctionAddActivity(nType, szName, nStartTime)
int LuaAuc_AuctionAddActivity(Lua_State* L)
{
	KAucUiActivity a;
	memset(&a, 0, sizeof(a));
	a.nType = sArgInt(L, 1);
	sCopyStr(a.szName, sizeof(a.szName), sArgStr(L, 2));
	a.nStartTime = sArgInt(L, 3);
	sNotify(AUCUI_CMD_ADD_ACTIVITY, (int)&a);
	return 0;
}

// AuctionDelActivity(nType, szName)
int LuaAuc_AuctionDelActivity(Lua_State* L)
{
	KAucUiActivity a;
	memset(&a, 0, sizeof(a));
	a.nType = sArgInt(L, 1);
	sCopyStr(a.szName, sizeof(a.szName), sArgStr(L, 2));
	sNotify(AUCUI_CMD_DEL_ACTIVITY, (int)&a);
	return 0;
}

int LuaAuc_AuctionClearActivityList(Lua_State* L)
{
	sNotify(AUCUI_CMD_CLEAR_ACTIVITY, 0);
	return 0;
}

// AuctionOnActivitySelect(nType, szName)
int LuaAuc_AuctionOnActivitySelect(Lua_State* L)
{
	KAucUiActivity a;
	memset(&a, 0, sizeof(a));
	a.nType = sArgInt(L, 1);
	sCopyStr(a.szName, sizeof(a.szName), sArgStr(L, 2));
	sNotify(AUCUI_CMD_SELECT_ACTIVITY, (int)&a);
	return 0;
}

// ---------------------------------------------------------------- vat pham
// 6 so + so luong + ten -> ChatItem (nhu KMailClient.cpp) bat dau tu tham so nFrom
static void sFillItem(Lua_State* L, int nFrom, KAucUiItem* p)
{
	memset(&p->Item, 0, sizeof(p->Item));
	p->Item.m_nID = 0x7B000000 + (p->nId & 0xFFFFFF);	// khac 0 (CoreShell.cpp GDI_ITEM_CHAT tu choi id 0)
	p->Item.m_btGenre = (BYTE)sArgInt(L, nFrom);
	p->Item.m_btDetail = (short)sArgInt(L, nFrom + 1);
	p->Item.m_btParticur = (short)sArgInt(L, nFrom + 2);
	p->Item.m_btLevel = (BYTE)sArgInt(L, nFrom + 3);
	p->Item.m_btSeries = (BYTE)sArgInt(L, nFrom + 4);
	p->Item.m_btLuck = (BYTE)sArgInt(L, nFrom + 5);
	p->nCount = sArgInt(L, nFrom + 6);
	if (p->nCount < 1)
		p->nCount = 1;
	p->Item.m_bStack = (BYTE)(p->nCount > 255 ? 255 : p->nCount);
	p->Item.m_wVersion = (WORD)g_SubWorldSet.GetGameVersion();
	p->Item.m_nNature = 0;
	p->Item.m_nGoldId = 0;
	sCopyStr(p->szName, sizeof(p->szName), sArgStr(L, nFrom + 7));
	sCopyStr(p->szCurrency, sizeof(p->szCurrency), sArgStr(L, nFrom + 8));
}

// AuctionAddEnglishItem(nType, nId, nStart, nGuaranteed, nRange, nCurrency, nMax, nSelf, nRemaining, bMine, g,d,p,l,s,k, nCount, szName)
static void sFillEnglish(Lua_State* L, KAucUiItem* p)
{
	memset(p, 0, sizeof(*p));
	p->nKind = AUCUI_KIND_ENGLISH;
	p->nType = sArgInt(L, 1);
	p->nId = sArgInt(L, 2);
	p->nStartTime = sArgInt(L, 3);
	p->nGuaranteed = sArgInt(L, 4);
	p->nRange = sArgInt(L, 5);
	p->nCurrency = sArgInt(L, 6);
	p->nMax = sArgInt(L, 7);
	p->nSelf = sArgInt(L, 8);
	p->nRemaining = sArgInt(L, 9);
	p->bMine = sArgInt(L, 10);
}

// AuctionAddDutchItem(nType, nId, nStart, nCur, nGuaranteed, nCurrency, nRemaining, nNext, nTotalRemaining, bMine, g,d,p,l,s,k, nCount, szName)
static void sFillDutch(Lua_State* L, KAucUiItem* p)
{
	memset(p, 0, sizeof(*p));
	p->nKind = AUCUI_KIND_DUTCH;
	p->nType = sArgInt(L, 1);
	p->nId = sArgInt(L, 2);
	p->nStartTime = sArgInt(L, 3);
	p->nCur = sArgInt(L, 4);
	p->nGuaranteed = sArgInt(L, 5);
	p->nCurrency = sArgInt(L, 6);
	p->nRemaining = sArgInt(L, 7);
	p->nNext = sArgInt(L, 8);
	p->nTotalRemaining = sArgInt(L, 9);
	p->bMine = sArgInt(L, 10);
}

int LuaAuc_AuctionAddEnglishItem(Lua_State* L)
{
	KAucUiItem it;
	sFillEnglish(L, &it);
	sFillItem(L, 11, &it);
	sNotify(AUCUI_CMD_ADD_ITEM, (int)&it);
	return 0;
}

int LuaAuc_AuctionAddDutchItem(Lua_State* L)
{
	KAucUiItem it;
	sFillDutch(L, &it);
	sFillItem(L, 11, &it);
	sNotify(AUCUI_CMD_ADD_ITEM, (int)&it);
	return 0;
}

// AuctionSetEnglishItem(...) 10 tham so dau - khong doi vat pham
int LuaAuc_AuctionSetEnglishItem(Lua_State* L)
{
	KAucUiItem it;
	sFillEnglish(L, &it);
	sCopyStr(it.szCurrency, sizeof(it.szCurrency), sArgStr(L, 11));
	sNotify(AUCUI_CMD_SET_ITEM, (int)&it);
	return 0;
}

int LuaAuc_AuctionSetDutchItem(Lua_State* L)
{
	KAucUiItem it;
	sFillDutch(L, &it);
	sCopyStr(it.szCurrency, sizeof(it.szCurrency), sArgStr(L, 11));
	sNotify(AUCUI_CMD_SET_ITEM, (int)&it);
	return 0;
}

// AuctionEndItem(nType, nId)
int LuaAuc_AuctionEndItem(Lua_State* L)
{
	sNotify(AUCUI_CMD_END_ITEM, sArgInt(L, 2));
	return 0;
}

int LuaAuc_AuctionClearItemList(Lua_State* L)
{
	sNotify(AUCUI_CMD_CLEAR_ITEM, 0);
	return 0;
}

// ---------------------------------------------------------------- thanh vien (phien bang hoi)
// AuctionAddActivityMember(szName, nLevel, nFigure, bOnline)
int LuaAuc_AuctionAddActivityMember(Lua_State* L)
{
	KAucUiMember m;
	memset(&m, 0, sizeof(m));
	sCopyStr(m.szName, sizeof(m.szName), sArgStr(L, 1));
	m.nLevel = sArgInt(L, 2);
	m.nFigure = sArgInt(L, 3);
	m.bOnline = sArgInt(L, 4);
	sNotify(AUCUI_CMD_ADD_MEMBER, (int)&m);
	return 0;
}

int LuaAuc_AuctionClearMemberList(Lua_State* L)
{
	sNotify(AUCUI_CMD_CLEAR_MEMBER, 0);
	return 0;
}

// AuctionSetSalaryAndCount(nCount, nSalary)
int LuaAuc_AuctionSetSalaryAndCount(Lua_State* L)
{
	KAucUiSalary s;
	s.nCount = sArgInt(L, 1);
	s.nSalary = sArgInt(L, 2);
	sNotify(AUCUI_CMD_SET_SALARY, (int)&s);
	return 0;
}

// AuctionSetCurrentPageTxt(nType, szText)
int LuaAuc_AuctionSetCurrentPageTxt(Lua_State* L)
{
	char szTxt[64];
	sCopyStr(szTxt, sizeof(szTxt), sArgStr(L, 2));
	sNotify(AUCUI_CMD_SET_PAGE_TXT, (int)szTxt);
	return 0;
}

int LuaAuc_AuctionClearAll(Lua_State* L)
{
	sNotify(AUCUI_CMD_CLEAR_ALL, 0);
	return 0;
}

// AuctionSetMoney(nNganLuong, nXu)
int LuaAuc_AuctionSetMoney(Lua_State* L)
{
	KAucUiSalary s;
	s.nCount = sArgInt(L, 1);
	s.nSalary = sArgInt(L, 2);
	sNotify(AUCUI_CMD_SET_MONEY, (int)&s);
	return 0;
}

// AuctionConfirm(szText, szYesFunc) -> dung hop Co/Khong cua thu (GDCNI_MAIL_UI MAILUI_CMD_CONFIRM) qua Lua MailConfirm
// De khong phu thuoc KMailClient, o day chi luu ten ham; script goi MailConfirm(szText, "Xac nhan/AuctionConfirmYes", ...).
int LuaAuc_AuctionConfirm(Lua_State* L)
{
	sCopyStr(s_szConfirmFunc, sizeof(s_szConfirmFunc), sArgStr(L, 2));
	Lua_PushString(L, s_szConfirmFunc);
	return 1;
}

int LuaAuc_GetLocalTime(Lua_State* L)
{
	Lua_PushNumber(L, (double)time(NULL));
	return 1;
}

extern int LuaMail_Msg2Player(Lua_State* L);
int LuaAuc_PopBlackTips(Lua_State* L)
{
	return LuaMail_Msg2Player(L);
}

// ---------------------------------------------------------------- UI -> Lua
void AuctionUi_OnRequest(unsigned int uParam, int nParam)
{
	int nOp = (int)uParam;
	const KAucUiReq* pReq = (const KAucUiReq*)nParam;
	char szCall[512];
	szCall[0] = 0;
	if (nOp != AUCUI_OP_TICK)
		SP_ClientLog("[AUCUI] UI->Lua op=%d param=%d", nOp, nParam);
	switch (nOp)
	{
	case AUCUI_OP_ICON_CLICK:
		sprintf(szCall, "UIAuctionHouse:OnAuctionIconClick(%d)", nParam);
		break;
	case AUCUI_OP_PAGE_BTN:
		sprintf(szCall, "UIAuctionHouse:OnPageBtnClick(%d)", nParam);
		break;
	case AUCUI_OP_SELECT_ACTIVITY:
		if (pReq)
			sprintf(szCall, "UIAuctionHouse:OnSelect([[%.60s]])", pReq->szName);
		break;
	case AUCUI_OP_PREV_PAGE:
		strcpy(szCall, "UIAuctionHouse:OnClickPrevPageBtn()");
		break;
	case AUCUI_OP_NEXT_PAGE:
		strcpy(szCall, "UIAuctionHouse:OnClickNextPageBtn()");
		break;
	case AUCUI_OP_OFFER_ENGLISH:
		if (pReq)
			sprintf(szCall, "UIAuctionHouse:RequestOfferEnglishPrice(%d, %d)", pReq->nId, pReq->nPrice);
		break;
	case AUCUI_OP_OFFER_DUTCH:
		if (pReq)
			sprintf(szCall, "UIAuctionHouse:RequestOfferDutchPrice(%d, %d)", pReq->nId, pReq->nPrice);
		break;
	case AUCUI_OP_GET_BACK:
		if (pReq)
			sprintf(szCall, "UIAuctionHouse:RequestGetBackItem(%d)", pReq->nId);
		break;
	case AUCUI_OP_REFUND:
		if (pReq)
			sprintf(szCall, "UIAuctionHouse:RequestRefund(%d)", pReq->nId);
		break;
	case AUCUI_OP_MEMBER_LIST:
		strcpy(szCall, "UIAuctionHouse:RequestMemberList()");
		break;
	case AUCUI_OP_CONFIRM_RESULT:
		if (nParam == 1 && s_szConfirmFunc[0])
			sprintf(szCall, "%s()", s_szConfirmFunc);
		s_szConfirmFunc[0] = 0;
		break;
	case AUCUI_OP_CLOSE:
		s_bOpen = 0;
		strcpy(szCall, "UIAuctionHouse:OnWindowClosed()");
		break;
	case AUCUI_OP_RESET:
		s_bOpen = 0;
		strcpy(szCall, "UIAuctionHouse:Reset()");
		break;
	case AUCUI_OP_TICK:
		strcpy(szCall, "UIAuctionHouse:OnTick()");
		break;
	default:
		break;
	}
	if (szCall[0])
		SP_RunClientLua(UIAUCTION_SCRIPT, szCall);
}

#endif // !_SERVER
