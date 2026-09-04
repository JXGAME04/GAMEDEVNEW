// KMailClient.cpp - [MAIL 03/09] cau noi Lua <-> cua so THU phia client (viet lai theo VLTK 2.0).
// \script\ui\uimail.lua (chep tu client 2.0) goi cac ham C++ nay y het 2.0; ta chi doi thanh thong bao
// GDCNI_MAIL_UI sang S3Client (UiMail.cpp). Chieu nguoc: CoreShell.cpp case GOI_MAIL_UI -> MailUi_OnRequest
// -> chay "UIMail:xxx(...)" trong state cua uimail.lua (SP_RunClientLua, KScriptProtocol.cpp).
// Hop dong: KMailUiDef.h. Bien dich cho ca hai cau hinh nhung than chi co o client.
#include "KCore.h"
#include "KWin32.h"
#include "KEngine.h"
#include "KDebug.h"
#include "LuaLib.h"
#include "KMailUiDef.h"
#include "KScriptProtocol.h"
#include "KMailClient.h"
#ifndef _SERVER
#include "CoreShell.h"
#include "GameDataDef.h"
#include "KSubWorldSet.h"	// GetGameVersion cho ChatItem.m_wVersion
#include <stdio.h>
#include <string.h>
#include <time.h>

extern int CoreDataChanged(unsigned int uDataId, unsigned int uParam, int nParam);

#define UIMAIL_SCRIPT	"\\script\\ui\\uimail.lua"

static char s_szConfirmFunc[64] = "";	// ham Lua se goi khi nguoi choi bam dong y o hop xac nhan

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
	SP_ClientLog("[MAILUI] Lua->UI cmd=%d param=%d", nCmd, nParam);	// [D4] chan doan
	CoreDataChanged(GDCNI_MAIL_UI, (unsigned int)nCmd, nParam);
}

// AddMailHeader(nId, szSender, szTitle, bRead, bHasAward, nExpiredTime, nSendTime) - uimail.lua:60
static void sFillHeader(Lua_State* L, KMailUiHeader* p)
{
	memset(p, 0, sizeof(*p));
	p->nId = sArgInt(L, 1);
	sCopyStr(p->szSender, sizeof(p->szSender), sArgStr(L, 2));
	sCopyStr(p->szTitle, sizeof(p->szTitle), sArgStr(L, 3));
	p->bRead = sArgInt(L, 4);
	p->bHasAward = sArgInt(L, 5);
	p->nExpiredTime = sArgInt(L, 6);
	p->nSendTime = sArgInt(L, 7);
}

int LuaMail_AddMailHeader(Lua_State* L)
{
	KMailUiHeader h;
	sFillHeader(L, &h);
	sNotify(MAILUI_CMD_ADD_HEADER, (int)&h);
	return 0;
}

int LuaMail_SetMailHeader(Lua_State* L)
{
	KMailUiHeader h;
	sFillHeader(L, &h);
	sNotify(MAILUI_CMD_SET_HEADER, (int)&h);
	return 0;
}

int LuaMail_DeleteOneMail(Lua_State* L)
{
	sNotify(MAILUI_CMD_DEL_ONE, sArgInt(L, 1));
	return 0;
}

int LuaMail_CleanMailAll(Lua_State* L)
{
	sNotify(MAILUI_CMD_CLEAN_ALL, 0);
	return 0;
}

int LuaMail_CleanMailList(Lua_State* L)
{
	sNotify(MAILUI_CMD_CLEAN_LIST, 0);
	return 0;
}

int LuaMail_CleanMailDetail(Lua_State* L)
{
	sNotify(MAILUI_CMD_CLEAN_DETAIL, 0);
	return 0;
}

int LuaMail_UpdateMailCount(Lua_State* L)
{
	sNotify(MAILUI_CMD_UPDATE_COUNT, 0);
	return 0;
}

int LuaMail_NewMailUIEventArrival(Lua_State* L)
{
	sNotify(MAILUI_CMD_NEW_MAIL_BLINK, 0);
	return 0;
}

int LuaMail_SwitchMailManager(Lua_State* L)
{
	sNotify(MAILUI_CMD_SWITCH_MANAGER, 0);
	return 0;
}

int LuaMail_OpenMailWindow(Lua_State* L)
{
	sNotify(MAILUI_CMD_OPEN, sArgInt(L, 1));
	return 0;
}

int LuaMail_SetMailIconVisible(Lua_State* L)
{
	sNotify(MAILUI_CMD_ICON_VISIBLE, sArgInt(L, 1));
	return 0;
}

int LuaMail_SelectMail(Lua_State* L)
{
	sNotify(MAILUI_CMD_SELECT, sArgInt(L, 1));
	return 0;
}

int LuaMail_SetFilterText(Lua_State* L)
{
	sNotify(MAILUI_CMD_SET_FILTER_TEXT, sArgInt(L, 1));
	return 0;
}

// SetMailBntStatus(nAcceptShow, nAcceptEnable, nDeleteOneShow, nDeleteOneEnable, nDeleteAllShow, nDeleteAllEnable)
int LuaMail_SetMailBntStatus(Lua_State* L)
{
	KMailUiBtnStatus s;
	s.nAcceptShow = sArgInt(L, 1);
	s.nAcceptEnable = sArgInt(L, 2);
	s.nDelOneShow = sArgInt(L, 3);
	s.nDelOneEnable = sArgInt(L, 4);
	s.nDelAllShow = sArgInt(L, 5);
	s.nDelAllEnable = sArgInt(L, 6);
	sNotify(MAILUI_CMD_BTN_STATUS, (int)&s);
	return 0;
}

// UpdateMailDetail(nId, szSender, szTitle, szContent, 1, bHasAward, szTime, nAwardCount, szAwards)
// szAwards = "icon|ten|mo ta|so luong\n..." (uimail.lua PackAwardInfo - may chu mo ta san phan thuong)
int LuaMail_UpdateMailDetail(Lua_State* L)
{
	static KMailUiDetail d;		// ~8 KB, de tinh cho khoi phinh stack
	memset(&d, 0, sizeof(d));
	d.nId = sArgInt(L, 1);
	sCopyStr(d.szSender, sizeof(d.szSender), sArgStr(L, 2));
	sCopyStr(d.szTitle, sizeof(d.szTitle), sArgStr(L, 3));
	sCopyStr(d.szContent, sizeof(d.szContent), sArgStr(L, 4));
	d.bHasAward = sArgInt(L, 6);
	sCopyStr(d.szTime, sizeof(d.szTime), sArgStr(L, 7));
	d.nAwardCount = 0;
	const char* p = sArgStr(L, 9);
	while (p && *p && d.nAwardCount < MAILUI_MAX_AWARD)
	{
		const char* pEnd = strchr(p, '\n');
		int nLineLen = pEnd ? (int)(pEnd - p) : (int)strlen(p);
		char szLine[1024];
		if (nLineLen >= (int)sizeof(szLine))
			nLineLen = sizeof(szLine) - 1;
		memcpy(szLine, p, nLineLen);
		szLine[nLineLen] = 0;
		if (szLine[0])
		{
			KMailUiAward* pA = &d.Award[d.nAwardCount];
			char* szField[9] = { szLine, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
			int nField = 1;
			for (char* q = szLine; *q && nField < 9; q++)
			{
				if (*q == '|')
				{
					*q = 0;
					szField[nField++] = q + 1;
				}
			}
			if (strcmp(szField[0], "item") == 0 && nField >= 8)
			{
				// item|genre|detail|particular|level|series|luck|so luong -> ChatItem cho GDI_ITEM_CHAT
				pA->nKind = MAILAWARD_ITEM;
				memset(&pA->Item, 0, sizeof(pA->Item));
				pA->Item.m_nID = 0x7A000000 + d.nAwardCount + 1;	// khac 0 (CoreShell.cpp GDI_ITEM_CHAT tu choi id 0)
				pA->Item.m_btGenre = (BYTE)atoi(szField[1]);
				pA->Item.m_btDetail = (short)atoi(szField[2]);
				pA->Item.m_btParticur = (short)atoi(szField[3]);
				pA->Item.m_btLevel = (BYTE)atoi(szField[4]);
				pA->Item.m_btSeries = (BYTE)atoi(szField[5]);
				pA->Item.m_btLuck = (BYTE)atoi(szField[6]);
				pA->nCount = atoi(szField[7]);
				if (pA->nCount <= 0)
					pA->nCount = 1;
				pA->Item.m_bStack = (BYTE)(pA->nCount > 255 ? 255 : pA->nCount);
				pA->Item.m_wVersion = (WORD)g_SubWorldSet.GetGameVersion();
				pA->Item.m_nNature = 0;
				pA->Item.m_nGoldId = 0;
				pA->szIcon[0] = 0;
				pA->szName[0] = 0;
				pA->szDesc[0] = 0;
				d.nAwardCount++;
			}
			else if (strcmp(szField[0], "icon") == 0 && nField >= 2)
			{
				// icon|duong dan|ten|mo ta|so luong
				pA->nKind = MAILAWARD_ICON;
				sCopyStr(pA->szIcon, sizeof(pA->szIcon), szField[1]);
				sCopyStr(pA->szName, sizeof(pA->szName), szField[2]);
				sCopyStr(pA->szDesc, sizeof(pA->szDesc), szField[3]);
				pA->nCount = szField[4] ? atoi(szField[4]) : 1;
				if (pA->nCount <= 0)
					pA->nCount = 1;
				d.nAwardCount++;
			}
		}
		if (!pEnd)
			break;
		p = pEnd + 1;
	}
	if (d.nAwardCount == 0)
		d.nAwardCount = sArgInt(L, 8) > 0 && d.bHasAward ? 0 : 0;
	sNotify(MAILUI_CMD_UPDATE_DETAIL, (int)&d);
	return 0;
}

// FormatTime2String(szFmt, nTime) -> chuoi (strftime, gio may)
int LuaMail_FormatTime2String(Lua_State* L)
{
	const char* szFmt = sArgStr(L, 1);
	time_t t = (time_t)sArgInt(L, 2);
	char szBuf[64];
	szBuf[0] = 0;
	struct tm* pTm = localtime(&t);
	if (pTm && szFmt[0])
		strftime(szBuf, sizeof(szBuf) - 1, szFmt, pTm);
	Lua_PushString(L, szBuf);
	return 1;
}

// MailConfirm(szText, "Nhan dong y/HamLua", "Nhan huy/no") - thay ClientSay cua 2.0.
// Nut dong y -> MAILUI_OP_CONFIRM_RESULT(1) -> goi HamLua() trong uimail.lua.
int LuaMail_MailConfirm(Lua_State* L)
{
	KMailUiConfirm c;
	memset(&c, 0, sizeof(c));
	sCopyStr(c.szText, sizeof(c.szText), sArgStr(L, 1));
	const char* szYes = sArgStr(L, 2);
	const char* szNo = sArgStr(L, 3);
	const char* pSlash = strchr(szYes, '/');
	if (pSlash)
	{
		int n = (int)(pSlash - szYes);
		if (n >= (int)sizeof(c.szYes))
			n = sizeof(c.szYes) - 1;
		memcpy(c.szYes, szYes, n);
		c.szYes[n] = 0;
		sCopyStr(s_szConfirmFunc, sizeof(s_szConfirmFunc), pSlash + 1);
	}
	else
	{
		sCopyStr(c.szYes, sizeof(c.szYes), szYes);
		s_szConfirmFunc[0] = 0;
	}
	pSlash = strchr(szNo, '/');
	if (pSlash)
	{
		int n = (int)(pSlash - szNo);
		if (n >= (int)sizeof(c.szNo))
			n = sizeof(c.szNo) - 1;
		memcpy(c.szNo, szNo, n);
		c.szNo[n] = 0;
	}
	else
		sCopyStr(c.szNo, sizeof(c.szNo), szNo);
	sNotify(MAILUI_CMD_CONFIRM, (int)&c);
	return 0;
}

// Msg2Player(szText) phia client: thong bao he thong (khung chat), nhu may chu gui SHOW_MSG.
int LuaMail_Msg2Player(Lua_State* L)
{
	KSystemMessage sMsg;
	memset(&sMsg, 0, sizeof(sMsg));
	sCopyStr(sMsg.szMessage, sizeof(sMsg.szMessage), sArgStr(L, 1));
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	return 0;
}

// CoreShell.cpp: case GOI_MAIL_UI -> MailUi_OnRequest(uParam, nParam)
void MailUi_OnRequest(unsigned int uParam, int nParam)
{
	int nOp = MAILUI_OP_CODE(uParam);
	int nExtra = MAILUI_OP_EXTRA(uParam);
	char szCall[256];
	szCall[0] = 0;
	SP_ClientLog("[MAILUI] UI->Lua op=%d extra=%d param=%d", nOp, nExtra, nParam);	// [D4] chan doan
	switch (nOp)
	{
	case MAILUI_OP_SELECT:
		sprintf(szCall, "UIMail:OnSelect(%d)", nParam);
		break;
	case MAILUI_OP_CHECK:
		sprintf(szCall, "UIMail:OnMailCheck(%d, %d, %d)", nParam, nExtra & 1, (nExtra >> 1) & 1);
		break;
	case MAILUI_OP_DELETE_ONE:
		strcpy(szCall, "UIMail:RequestDeleteMail()");
		break;
	case MAILUI_OP_DELETE_CHECKED:
		strcpy(szCall, "UIMail:RequestDeleteAllCheckedMail()");
		break;
	case MAILUI_OP_ACCEPT:
		sprintf(szCall, "UIMail:RequestChangeState(%d, MAILDEF.tbState.DRAWED)", nParam);
		break;
	case MAILUI_OP_FILTER:
		sprintf(szCall, "UIMail:OnMailFilterSelected(%d)", nParam);
		break;
	case MAILUI_OP_AUTO_DELETE:
		strcpy(szCall, "UIMail:RequestAutoDeleteMails()");
		break;
	case MAILUI_OP_UPDATE:
		strcpy(szCall, "UIMail:OnUpdate()");
		break;
	case MAILUI_OP_ICON_CLICK:
		strcpy(szCall, "UIMail:OnMailIconClick()");
		break;
	case MAILUI_OP_REQUEST_LIST:
		strcpy(szCall, "UIMail:RequestMailHeaderList()");
		break;
	case MAILUI_OP_CHECK_ID_ON_OPEN:
		sprintf(szCall, "UIMail:CheckIdOnOpen(%d)", nParam);
		break;
	case MAILUI_OP_CONFIRM_RESULT:
		if (nParam == 1 && s_szConfirmFunc[0])
			sprintf(szCall, "%s()", s_szConfirmFunc);
		s_szConfirmFunc[0] = 0;
		break;
	case MAILUI_OP_CLOSE:
	default:
		break;
	}
	if (szCall[0])
		SP_RunClientLua(UIMAIL_SCRIPT, szCall);
}

#endif // !_SERVER
