// KTongJX2.cpp - JX2 port: ban sao du lieu bang hoi tren GameServer + 49 ham Lua.
// Xem KTongJX2.h ve mo hinh. Cac ham dat ten va chu ky DUNG THEO JX2
// (sigspec_tong.md + BANGHOI_JX2_PHANTICH.md):
//   - Tham so dau tien cua moi ham TONG_* la nTongID (= NameID ten bang).
//   - Ho TONGM_*: tham so 2 la SO -> tra theo NameID; la CHUOI -> tra theo ten.
//   - Gia tri 0 == khong co khoa (SetField xoa khoa khi = 0).
//   - Apply* CHI gui goi len Relay; ban sao cap nhat khi echo quay ve.
//////////////////////////////////////////////////////////////////////

// LUU Y: Core build voi PCH "Use" qua KCore.h - moi thu TRUOC dong include nay
// deu bi compiler bo qua, nen KCore.h PHAI dung dau tien.
#include "KCore.h"
#include "KPlayerChat.h"
#include "KWin32.h"

#ifdef _SERVER

#include "KEngine.h"
#include "KDebug.h"
#include "LuaLib.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KProtocolProcess.h"
#include <KProtocol.h>
#include <KProtocolDef.h>
#include <KTongProtocol.h>
#include "KNewProtocolProcess.h"
#include "KTongJX2.h"

KTongJX2Mgr g_TongJX2;

extern int GetPlayerIndex(Lua_State* L);

//////////////////////////////////////////////////////////////////////
// Bo may ban sao
//////////////////////////////////////////////////////////////////////

KTongJX2Tong* KTongJX2Mgr::FindTong(DWORD dwTongNameID)
{
	if (dwTongNameID == 0)
		return NULL;
	std::map<DWORD, KTongJX2Tong>::iterator it = m_mapTong.find(dwTongNameID);
	if (it == m_mapTong.end())
		return NULL;
	return &it->second;
}

KTongJX2Member* KTongJX2Mgr::FindMember(KTongJX2Tong* pTong, DWORD dwMemberNameID)
{
	if (!pTong || dwMemberNameID == 0)
		return NULL;
	std::map<DWORD, KTongJX2Member>::iterator it = pTong->mapMember.find(dwMemberNameID);
	if (it == pTong->mapMember.end())
		return NULL;
	return &it->second;
}

DWORD KTongJX2Mgr::GetField(DWORD dwTongNameID, WORD wKey)
{
	KTongJX2Tong* pTong = FindTong(dwTongNameID);
	if (!pTong)
		return 0;
	std::map<WORD, DWORD>::iterator it = pTong->mapField.find(wKey);
	if (it == pTong->mapField.end())
		return 0;
	return it->second;
}

DWORD KTongJX2Mgr::GetMemberField(KTongJX2Member* pMember, WORD wKey)
{
	if (!pMember)
		return 0;
	std::map<WORD, DWORD>::iterator it = pMember->mapField.find(wKey);
	if (it == pMember->mapField.end())
		return 0;
	return it->second;
}

// dat / xoa 1 field trong map (0 = xoa - dung ngu nghia JX2)
static void sSetMapField(std::map<WORD, DWORD>& mapField, WORD wKey, DWORD dwValue)
{
	if (dwValue == 0)
		mapField.erase(wKey);
	else
		mapField[wKey] = dwValue;
}

void KTongJX2Mgr::OnRelayPacket(const void* pData, int nSize)
{
	if (!pData || nSize < (int)sizeof(EXTEND_HEADER))
		return;
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;

	switch (pHeader->ProtocolID)
	{
	case enumS2C_TONG_JX2_FIELD_SYNC:
		{
			if (nSize < (int)sizeof(STONG_JX2_FIELD_COMMAND))
				return;
			STONG_JX2_FIELD_COMMAND* pCmd = (STONG_JX2_FIELD_COMMAND*)pData;
			KTongJX2Tong* pTong = FindTong(pCmd->m_dwTongNameID);
			if (!pTong)
				return;
			sSetMapField(pTong->mapField, pCmd->m_wKey, pCmd->m_dwValue);
		}
		break;

	case enumS2C_TONG_JX2_MONEY_SYNC:
		{
			if (nSize < (int)sizeof(STONG_JX2_MONEY_COMMAND))
				return;
			STONG_JX2_MONEY_COMMAND* pCmd = (STONG_JX2_MONEY_COMMAND*)pData;
			KTongJX2Tong* pTong = FindTong(pCmd->m_dwTongNameID);
			if (!pTong)
				return;
			__int64 nMoney = pCmd->m_nValue;
			sSetMapField(pTong->mapField, 3, (DWORD)(nMoney & 0xFFFFFFFF));
			sSetMapField(pTong->mapField, 4, (DWORD)((nMoney >> 32) & 0xFFFFFFFF));
		}
		break;

	case enumS2C_TONG_JX2_MEMBER_FIELD_SYNC:
		{
			if (nSize < (int)sizeof(STONG_JX2_MEMBER_FIELD_COMMAND))
				return;
			STONG_JX2_MEMBER_FIELD_COMMAND* pCmd = (STONG_JX2_MEMBER_FIELD_COMMAND*)pData;
			KTongJX2Tong* pTong = FindTong(pCmd->m_dwTongNameID);
			if (!pTong)
				return;
			KTongJX2Member* pMember = FindMember(pTong, pCmd->m_dwMemberNameID);
			if (!pMember)
				return;
			sSetMapField(pMember->mapField, pCmd->m_wKey, pCmd->m_dwValue);
		}
		break;

	case enumS2C_TONG_JX2_RIGHT_SYNC:
		{
			if (nSize < (int)sizeof(STONG_JX2_RIGHT_COMMAND))
				return;
			STONG_JX2_RIGHT_COMMAND* pCmd = (STONG_JX2_RIGHT_COMMAND*)pData;
			KTongJX2Tong* pTong = FindTong(pCmd->m_dwTongNameID);
			if (!pTong)
				return;
			KTongJX2Member* pMember = FindMember(pTong, pCmd->m_dwMemberNameID);
			if (!pMember)
				return;
			if (pCmd->m_btAdd)
				pMember->setRight.insert(pCmd->m_dwRightID);
			else
				pMember->setRight.erase(pCmd->m_dwRightID);
		}
		break;

	case enumS2C_TONG_JX2_TONG_SYNC:
		{
			if (nSize < (int)sizeof(STONG_JX2_TONG_SYNC))
				return;
			STONG_JX2_TONG_SYNC* pSync = (STONG_JX2_TONG_SYNC*)pData;
			if (pSync->m_wLength != nSize)
				return;
			int nNeed = (int)sizeof(STONG_JX2_TONG_SYNC) + (int)pSync->m_wFieldCount * 6;
			if (nSize < nNeed)
				return;

			KTongJX2Tong& sTong = m_mapTong[pSync->m_dwTongNameID];
			sTong.dwNameID = pSync->m_dwTongNameID;
			memcpy(sTong.szName, pSync->m_szTongName, sizeof(sTong.szName));
			sTong.szName[31] = 0;
			sTong.btCamp = pSync->m_btCamp;
			memcpy(sTong.szAnnounce, pSync->m_szAnnounce, sizeof(sTong.szAnnounce));
			sTong.szAnnounce[sizeof(sTong.szAnnounce) - 1] = 0;
			sTong.mapField.clear();
			// dump lai tu dau -> danh sach thanh vien se toi ngay sau goi nay
			sTong.mapMember.clear();
			// dwTaskTemp giu nguyen neu bang da co (bo dem cuc bo cua GS nay)

			BYTE* pIn = (BYTE*)(pSync + 1);
			for (int i = 0; i < (int)pSync->m_wFieldCount; i++)
			{
				WORD wKey = *(WORD*)pIn;
				pIn += 2;
				DWORD dwVal = *(DWORD*)pIn;
				pIn += 4;
				sSetMapField(sTong.mapField, wKey, dwVal);
			}
		}
		break;

	case enumS2C_TONG_JX2_MEMBER_SYNC:
		{
			if (nSize < (int)sizeof(STONG_JX2_MEMBER_SYNC))
				return;
			STONG_JX2_MEMBER_SYNC* pSync = (STONG_JX2_MEMBER_SYNC*)pData;
			if (pSync->m_wLength != nSize)
				return;
			KTongJX2Tong* pTong = FindTong(pSync->m_dwTongNameID);
			if (!pTong)
				return;

			BYTE* pIn = (BYTE*)(pSync + 1);
			BYTE* pEnd = (BYTE*)pData + nSize;
			for (int i = 0; i < (int)pSync->m_btCount; i++)
			{
				if (pIn + sizeof(STONG_JX2_ONE_MEMBER) > pEnd)
					return;
				STONG_JX2_ONE_MEMBER* pOne = (STONG_JX2_ONE_MEMBER*)pIn;
				pIn += sizeof(STONG_JX2_ONE_MEMBER);
				int nPayload = (int)pOne->m_btFieldCount * 6 + (int)pOne->m_btRightCount * 4;
				if (pIn + nPayload > pEnd)
					return;

				KTongJX2Member& sMember = pTong->mapMember[pOne->m_dwMemberNameID];
				sMember.dwNameID = pOne->m_dwMemberNameID;
				memcpy(sMember.szName, pOne->m_szName, sizeof(sMember.szName));
				sMember.szName[31] = 0;
				sMember.btFigure = pOne->m_btFigure;
				sMember.btSex = pOne->m_btSex;
				sMember.mapField.clear();
				sMember.setRight.clear();
				int k;
				for (k = 0; k < (int)pOne->m_btFieldCount; k++)
				{
					WORD wKey = *(WORD*)pIn;
					pIn += 2;
					DWORD dwVal = *(DWORD*)pIn;
					pIn += 4;
					sSetMapField(sMember.mapField, wKey, dwVal);
				}
				for (k = 0; k < (int)pOne->m_btRightCount; k++)
				{
					sMember.setRight.insert(*(DWORD*)pIn);
					pIn += 4;
				}
			}
		}
		break;

	case enumS2C_TONG_JX2_TONG_REMOVE_SYNC:
		{
			if (nSize < (int)sizeof(STONG_JX2_TONG_REMOVE_SYNC))
				return;
			STONG_JX2_TONG_REMOVE_SYNC* pCmd = (STONG_JX2_TONG_REMOVE_SYNC*)pData;
			m_mapTong.erase(pCmd->m_dwTongNameID);
		}
		break;

	case enumS2C_TONG_JX2_SYNC_DONE:
		{
			m_bSynced = TRUE;
			g_DebugLog("[TONGJX2] da nhan xong dump: %d bang", (int)m_mapTong.size());
		}
		break;

	case enumS2C_TONG_JX2_STRING_SYNC:
		{
			if (nSize < (int)sizeof(STONG_JX2_STRING_COMMAND))
				return;
			STONG_JX2_STRING_COMMAND* pCmd = (STONG_JX2_STRING_COMMAND*)pData;
			KTongJX2Tong* pTong = FindTong(pCmd->m_dwTongNameID);
			if (!pTong)
				return;
			if (pCmd->m_btKind == defTONG_JX2_STR_ANNOUNCE)
			{
				memcpy(pTong->szAnnounce, pCmd->m_szText, sizeof(pTong->szAnnounce));
				pTong->szAnnounce[sizeof(pTong->szAnnounce) - 1] = 0;
			}
			else if (pCmd->m_btKind == defTONG_JX2_STR_RECRUIT)
			{
				memcpy(pTong->szRecruit, pCmd->m_szText, sizeof(pTong->szRecruit));
				pTong->szRecruit[sizeof(pTong->szRecruit) - 1] = 0;
			}
			else if (pCmd->m_btKind == defTONG_JX2_STR_UNION)
			{
				memcpy(pTong->szUnionName, pCmd->m_szText, sizeof(pTong->szUnionName));
				pTong->szUnionName[sizeof(pTong->szUnionName) - 1] = 0;
			}
			else if (pCmd->m_btKind == defTONG_JX2_STR_EVENT)
			{
				memset(pTong->szEvent[pTong->nEventHead], 0, 96);
				strncpy(pTong->szEvent[pTong->nEventHead], pCmd->m_szText, 95);
				pTong->nEventHead = (pTong->nEventHead + 1) % 16;
			}
			else if (pCmd->m_btKind == defTONG_JX2_STR_HISTORY)
			{
				memset(pTong->szHistory[pTong->nHistoryHead], 0, 96);
				strncpy(pTong->szHistory[pTong->nHistoryHead], pCmd->m_szText, 95);
				pTong->nHistoryHead = (pTong->nHistoryHead + 1) % 16;
			}
		}
		break;

	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////
// Ho tro gui lenh Apply len Relay
//////////////////////////////////////////////////////////////////////

static void sSendFieldCmd(DWORD dwTongID, WORD wKey, DWORD dwValue, BYTE btOp, DWORD dwParam)
{
	STONG_JX2_FIELD_COMMAND sCmd;
	sCmd.ProtocolFamily = pf_tong;
	sCmd.ProtocolID = enumC2S_TONG_JX2_FIELD;
	sCmd.m_dwTongNameID = dwTongID;
	sCmd.m_wKey = wKey;
	sCmd.m_dwValue = dwValue;
	sCmd.m_btOp = btOp;
	sCmd.m_dwParam = dwParam;
	g_NewProtocolProcess.PushMsgInTong((const void*)&sCmd, sizeof(sCmd));
}

static void sSendMoneyCmd(DWORD dwTongID, __int64 nValue, BYTE btOp, DWORD dwParam)
{
	STONG_JX2_MONEY_COMMAND sCmd;
	sCmd.ProtocolFamily = pf_tong;
	sCmd.ProtocolID = enumC2S_TONG_JX2_MONEY;
	sCmd.m_dwTongNameID = dwTongID;
	sCmd.m_nValue = nValue;
	sCmd.m_btOp = btOp;
	sCmd.m_dwParam = dwParam;
	g_NewProtocolProcess.PushMsgInTong((const void*)&sCmd, sizeof(sCmd));
}

static void sSendMemberFieldCmd(DWORD dwTongID, DWORD dwMemberID, WORD wKey, DWORD dwValue, BYTE btOp, DWORD dwParam)
{
	STONG_JX2_MEMBER_FIELD_COMMAND sCmd;
	sCmd.ProtocolFamily = pf_tong;
	sCmd.ProtocolID = enumC2S_TONG_JX2_MEMBER_FIELD;
	sCmd.m_dwTongNameID = dwTongID;
	sCmd.m_dwMemberNameID = dwMemberID;
	sCmd.m_wKey = wKey;
	sCmd.m_dwValue = dwValue;
	sCmd.m_btOp = btOp;
	sCmd.m_dwParam = dwParam;
	g_NewProtocolProcess.PushMsgInTong((const void*)&sCmd, sizeof(sCmd));
}

static void sSendRightCmd(DWORD dwTongID, DWORD dwMemberID, DWORD dwRightID, BYTE btAdd, DWORD dwParam)
{
	STONG_JX2_RIGHT_COMMAND sCmd;
	sCmd.ProtocolFamily = pf_tong;
	sCmd.ProtocolID = enumC2S_TONG_JX2_RIGHT;
	sCmd.m_dwTongNameID = dwTongID;
	sCmd.m_dwMemberNameID = dwMemberID;
	sCmd.m_dwRightID = dwRightID;
	sCmd.m_btAdd = btAdd;
	sCmd.m_dwParam = dwParam;
	g_NewProtocolProcess.PushMsgInTong((const void*)&sCmd, sizeof(sCmd));
}

#define defTASK_JX2_CONTRIBUTION	2801	// diem cong hien tieu duoc
#define defTASK_JX2_WEEKLYOFFER		2802	// cong hien tuan nay (tran script tu kiem)
#define defTASK_JX2_CUMULATEOFFER	2803	// cong hien tich luy tron doi
#define defTASK_JX2_WEEKGOALOFFER	2804	// cong hien muc tieu tuan

// (defTASK_JX2_* dung o DoClientOp)
// Tim index nguoi choi online theo NameID (0 = khong online tren GS nay)
static int sFindPlayerIdxByNameID(DWORD dwNameID)
{
	int nIndex = PlayerSet.GetFirstPlayer();
	while (nIndex > 0)
	{
		if (g_FileName2Id(Player[nIndex].m_PlayerName) == dwNameID)
			return nIndex;
		nIndex = PlayerSet.GetNextPlayer();
	}
	return 0;
}

// Xin vao bang (khong can thuoc bang): du cap tu-nhan vao thang, duoi cap tu choi,
// con lai vao danh sach cho duyet. Tra 0=ok/queue, 5=that bai, 6=bi tu choi cap
static int sJX2_DoApplyJoin(int nPlayerIdx, DWORD dwTongID);

// Gui goi them thanh vien kieu JX1 goc len relay (theo mau ACCEPT_ADD trong KSOServer)
static void sJX2_SendAddMember(KTongJX2Tong* pTong, int nJoinIdx)
{
	STONG_ADD_MEMBER_COMMAND sAdd;
	memset(&sAdd, 0, sizeof(sAdd));
	sAdd.ProtocolFamily = pf_tong;
	sAdd.ProtocolID = enumC2S_TONG_ADD_MEMBER;
	sAdd.m_dwParam = (DWORD)nJoinIdx;
	sAdd.m_dwPlayerNameID = g_FileName2Id(Player[nJoinIdx].m_PlayerName);
	sAdd.m_btSex = (BYTE)Npc[Player[nJoinIdx].m_nIndex].m_nSex;
	sAdd.m_btTongNameLength = (BYTE)strlen(pTong->szName);
	sAdd.m_btPlayerNameLength = (BYTE)strlen(Player[nJoinIdx].m_PlayerName);
	memcpy(sAdd.m_szBuffer, pTong->szName, sAdd.m_btTongNameLength);
	memcpy(&sAdd.m_szBuffer[sAdd.m_btTongNameLength], Player[nJoinIdx].m_PlayerName, sAdd.m_btPlayerNameLength);
	sAdd.m_wLength = (WORD)(sizeof(STONG_ADD_MEMBER_COMMAND) - sizeof(sAdd.m_szBuffer)
		+ sAdd.m_btTongNameLength + sAdd.m_btPlayerNameLength);
	g_NewProtocolProcess.PushMsgInTong((const void*)&sAdd, (int)sAdd.m_wLength);
}

// Ban theo TEN: duyet duoc don cua nguoi da THOAT GAME (relay giai lenh nay theo
// ten nhan vat - TongConnect.cpp:322-327 goi g_cTongSet.AddMember(szPlayerName,
// szTongName, m_btSex)), nen khong bat buoc nguoi xin phai dang online.
// dwEchoIdx = chi so nguoi choi de relay bao ve, 0 khi ho da offline.
static void sJX2_SendAddMemberByName(KTongJX2Tong* pTong, const char* pszName,
	DWORD dwNameID, BYTE btSex, DWORD dwEchoIdx)
{
	if (!pTong || !pszName || !pszName[0])
		return;
	STONG_ADD_MEMBER_COMMAND sAdd;
	memset(&sAdd, 0, sizeof(sAdd));
	sAdd.ProtocolFamily = pf_tong;
	sAdd.ProtocolID = enumC2S_TONG_ADD_MEMBER;
	sAdd.m_dwParam = dwEchoIdx;
	sAdd.m_dwPlayerNameID = dwNameID;
	sAdd.m_btSex = btSex;
	sAdd.m_btTongNameLength = (BYTE)strlen(pTong->szName);
	sAdd.m_btPlayerNameLength = (BYTE)strlen(pszName);
	memcpy(sAdd.m_szBuffer, pTong->szName, sAdd.m_btTongNameLength);
	memcpy(&sAdd.m_szBuffer[sAdd.m_btTongNameLength], pszName, sAdd.m_btPlayerNameLength);
	sAdd.m_wLength = (WORD)(sizeof(STONG_ADD_MEMBER_COMMAND) - sizeof(sAdd.m_szBuffer)
		+ sAdd.m_btTongNameLength + sAdd.m_btPlayerNameLength);
	g_NewProtocolProcess.PushMsgInTong((const void*)&sAdd, (int)sAdd.m_wLength);
}

// Bao cho bang chu / nguoi co quyen thu nhan (1901) dang online biet co don moi.
// Truoc day nop don xong TUYET DOI im lang: nguoi xin thay "da gui don" con
// bang chu khong he hay biet, phai tu mo trang Chieu mo moi thay.
static void sJX2_NotifyApply(KTongJX2Tong* pTong, const char* pszMsg);
static void sSendStringCmd(DWORD dwTongID, BYTE btKind, const char* pszText, DWORD dwParam);

static int sJX2_DoApplyJoin(int nPlayerIdx, DWORD dwTongID)
{
	KTongJX2Tong* pTong = g_TongJX2.FindTong(dwTongID);
	if (!pTong || nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER ||
		Player[nPlayerIdx].m_nIndex <= 0 || Player[nPlayerIdx].m_cTong.m_nFlag)
		return 5;
	int nLevel = Npc[Player[nPlayerIdx].m_nIndex].m_Level;
	DWORD dwRefuse = g_TongJX2.GetField(dwTongID, 66);
	if (dwRefuse && nLevel < (int)dwRefuse)
		return 11;	// duoi nguong cap - truoc day tra 6 nen bao nham "khong du tien"
	// KHONG dung co dong/mo tuyen cua he JX1 (m_nRecruit) o day nua:
	//  - JX2 khong co cong tac do; viec chan nguoi xin lam bang HAI NGUONG CAP
	//    (field 65 tu dong nhan / field 66 tu choi duoi cap), dung nhu trang
	//    chieu mo ban Linux. Muon dong tuyen thi dat nguong tu choi that cao.
	//  - m_nRecruit mac dinh 0 = DONG, va relay khong bao gio luu no
	//    (KTongControl.cpp:77 va :123 deu dat 0), nen cong cu lam MOI don bi
	//    tu choi thang khi bang chu dang online => "xin vao bang khong hien".
	DWORD dwAuto = g_TongJX2.GetField(dwTongID, 65);
	if (dwAuto && nLevel >= (int)dwAuto)
	{
		sJX2_SendAddMember(pTong, nPlayerIdx);
		// ghi luon "Ngay gia nhap" (khoa 2) - client hien o panel thanh vien
		sSendMemberFieldCmd(dwTongID, g_FileName2Id(Player[nPlayerIdx].m_PlayerName),
			2, (DWORD)time(NULL), defTONG_JX2_OP_SET, (DWORD)nPlayerIdx);
		return 7;	// vao thang - de vo boc bao 'da gia nhap'
	}
	DWORD dwMyID = g_FileName2Id(Player[nPlayerIdx].m_PlayerName);
	int a;
	for (a = 0; a < (int)pTong->btApplyCount; a++)
	{
		if (pTong->dwApplyID[a] == dwMyID)
			return 12;	// da nop don truoc do - truoc day tra 0 nen bao y het lan dau
	}
	if (pTong->btApplyCount >= 8)
		return 5;
	a = pTong->btApplyCount;
	memset(pTong->szApplyName[a], 0, 32);
	strncpy(pTong->szApplyName[a], Player[nPlayerIdx].m_PlayerName, 31);
	pTong->dwApplyID[a] = dwMyID;
	pTong->wApplyLevel[a] = (WORD)nLevel;
	pTong->btApplySex[a] = (BYTE)Npc[Player[nPlayerIdx].m_nIndex].m_nSex;
	pTong->btApplyCount++;
	{
		// ghi so su kien (bang chu doc lai duoc sau khi vao game) + bao ngay
		// cho nhung nguoi dang online co quyen duyet
		char szLog[160];
		sprintf(szLog, "%s xin gia nh\313p bang (c\312p %d)",
			Player[nPlayerIdx].m_PlayerName, nLevel);
		sSendStringCmd(dwTongID, defTONG_JX2_STR_EVENT, szLog, (DWORD)nPlayerIdx);
		sJX2_NotifyApply(pTong, szLog);
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Ho tro doc tham so Lua
//////////////////////////////////////////////////////////////////////

static DWORD sArgTongID(Lua_State* L)
{
	if (!Lua_IsNumber(L, 1))
		return 0;
	return (DWORD)Lua_ValueToNumber(L, 1);
}

// Tham so thanh vien (vi tri nArg): SO -> NameID; CHUOI -> bam ten ra NameID
static DWORD sArgMemberID(Lua_State* L, int nArg)
{
	if (Lua_IsNumber(L, nArg))
		return (DWORD)Lua_ValueToNumber(L, nArg);
	if (Lua_IsString(L, nArg))
	{
		const char* pszName = (const char*)Lua_ValueToString(L, nArg);
		if (pszName && pszName[0])
			return g_FileName2Id((LPSTR)pszName);
	}
	return 0;
}

static DWORD sLuaPlayerParam(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
		return (DWORD)nPlayerIndex;
	return 0;
}

// nguoi choi co ten nay dang online tren GS NAY? (gioi han: chua thay online lien GS)
static BOOL sIsRoleOnlineLocal(const char* pszName)
{
	if (!pszName || !pszName[0])
		return FALSE;
	int nIndex = PlayerSet.GetFirstPlayer();
	while (nIndex > 0)
	{
		if (strcmp(Player[nIndex].m_PlayerName, pszName) == 0)
			return TRUE;
		nIndex = PlayerSet.GetNextPlayer();
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Ham Lua - nhom tra cuu bang
//////////////////////////////////////////////////////////////////////

// TONG_IsExist(nTongID) -> 0/1
int LuaTONG_IsExist(Lua_State* L)
{
	Lua_PushNumber(L, g_TongJX2.FindTong(sArgTongID(L)) ? 1 : 0);
	return 1;
}

// TONG_GetName(nTongID) -> string (khong co -> chuoi rong)
int LuaTONG_GetName(Lua_State* L)
{
	KTongJX2Tong* pTong = g_TongJX2.FindTong(sArgTongID(L));
	Lua_PushString(L, pTong ? pTong->szName : (char*)"");
	return 1;
}

// TONG_Name2ID(szTongName) -> nTongID (0 = khong ton tai)
int LuaTONG_Name2ID(Lua_State* L)
{
	if (!Lua_IsString(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	const char* pszName = (const char*)Lua_ValueToString(L, 1);
	if (!pszName || !pszName[0])
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	DWORD dwID = g_FileName2Id((LPSTR)pszName);
	Lua_PushNumber(L, g_TongJX2.FindTong(dwID) ? (double)dwID : 0);
	return 1;
}

// TONG_GetTongCount() -> n
int LuaTONG_GetTongCount(Lua_State* L)
{
	Lua_PushNumber(L, (double)g_TongJX2.m_mapTong.size());
	return 1;
}

// TONG_GetFirstTong() -> nTongID (0 = het)
int LuaTONG_GetFirstTong(Lua_State* L)
{
	if (g_TongJX2.m_mapTong.empty())
		Lua_PushNumber(L, 0);
	else
		Lua_PushNumber(L, (double)g_TongJX2.m_mapTong.begin()->first);
	return 1;
}

// TONG_GetNextTong(nTongID) -> nTongID (0 = het)
int LuaTONG_GetNextTong(Lua_State* L)
{
	DWORD dwID = sArgTongID(L);
	std::map<DWORD, KTongJX2Tong>::iterator it = g_TongJX2.m_mapTong.upper_bound(dwID);
	if (it == g_TongJX2.m_mapTong.end())
		Lua_PushNumber(L, 0);
	else
		Lua_PushNumber(L, (double)it->first);
	return 1;
}

// TONG_GetTongByRoleName(szRoleName) -> nTongID (QUET TUYEN TINH - cam goi trong vong lap)
int LuaTONG_GetTongByRoleName(Lua_State* L)
{
	if (!Lua_IsString(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	const char* pszName = (const char*)Lua_ValueToString(L, 1);
	if (!pszName || !pszName[0])
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	DWORD dwMemberID = g_FileName2Id((LPSTR)pszName);
	std::map<DWORD, KTongJX2Tong>::iterator it;
	for (it = g_TongJX2.m_mapTong.begin(); it != g_TongJX2.m_mapTong.end(); ++it)
	{
		if (it->second.mapMember.find(dwMemberID) != it->second.mapMember.end())
		{
			Lua_PushNumber(L, (double)it->first);
			return 1;
		}
	}
	Lua_PushNumber(L, 0);
	return 1;
}

// TONG_GetMemberCount(nTongID [, nFigure = -1]) -> n
int LuaTONG_GetMemberCount(Lua_State* L)
{
	KTongJX2Tong* pTong = g_TongJX2.FindTong(sArgTongID(L));
	if (!pTong)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nFigure = -1;
	if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2))
		nFigure = (int)Lua_ValueToNumber(L, 2);
	if (nFigure < 0)
	{
		Lua_PushNumber(L, (double)pTong->mapMember.size());
		return 1;
	}
	int nCount = 0;
	std::map<DWORD, KTongJX2Member>::iterator it;
	for (it = pTong->mapMember.begin(); it != pTong->mapMember.end(); ++it)
	{
		if ((int)it->second.btFigure == nFigure)
			nCount++;
	}
	Lua_PushNumber(L, nCount);
	return 1;
}

// TONG_GetOnlineCount(nTongID [, nFigure]) -> n  (gioi han: chi dem online tren GS nay)
int LuaTONG_GetOnlineCount(Lua_State* L)
{
	KTongJX2Tong* pTong = g_TongJX2.FindTong(sArgTongID(L));
	if (!pTong)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nFigure = -1;
	if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2))
		nFigure = (int)Lua_ValueToNumber(L, 2);
	int nCount = 0;
	int nIndex = PlayerSet.GetFirstPlayer();
	while (nIndex > 0)
	{
		DWORD dwID = g_FileName2Id(Player[nIndex].m_PlayerName);
		std::map<DWORD, KTongJX2Member>::iterator it = pTong->mapMember.find(dwID);
		if (it != pTong->mapMember.end())
		{
			if (nFigure < 0 || (int)it->second.btFigure == nFigure)
				nCount++;
		}
		nIndex = PlayerSet.GetNextPlayer();
	}
	Lua_PushNumber(L, nCount);
	return 1;
}

// TONG_GetFirstMember(nTongID, nFigure) -> memberID (nFigure am = tat ca; 0 = het)
int LuaTONG_GetFirstMember(Lua_State* L)
{
	KTongJX2Tong* pTong = g_TongJX2.FindTong(sArgTongID(L));
	int nFigure = -1;
	if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2))
		nFigure = (int)Lua_ValueToNumber(L, 2);
	if (pTong)
	{
		std::map<DWORD, KTongJX2Member>::iterator it;
		for (it = pTong->mapMember.begin(); it != pTong->mapMember.end(); ++it)
		{
			if (nFigure < 0 || (int)it->second.btFigure == nFigure)
			{
				Lua_PushNumber(L, (double)it->first);
				return 1;
			}
		}
	}
	Lua_PushNumber(L, 0);
	return 1;
}

// TONG_GetNextMember(nTongID, nMemberID, nFigure) -> memberID (0 = het)
int LuaTONG_GetNextMember(Lua_State* L)
{
	KTongJX2Tong* pTong = g_TongJX2.FindTong(sArgTongID(L));
	DWORD dwCur = 0;
	if (Lua_IsNumber(L, 2))
		dwCur = (DWORD)Lua_ValueToNumber(L, 2);
	int nFigure = -1;
	if (Lua_GetTopIndex(L) >= 3 && Lua_IsNumber(L, 3))
		nFigure = (int)Lua_ValueToNumber(L, 3);
	if (pTong)
	{
		std::map<DWORD, KTongJX2Member>::iterator it = pTong->mapMember.upper_bound(dwCur);
		for (; it != pTong->mapMember.end(); ++it)
		{
			if (nFigure < 0 || (int)it->second.btFigure == nFigure)
			{
				Lua_PushNumber(L, (double)it->first);
				return 1;
			}
		}
	}
	Lua_PushNumber(L, 0);
	return 1;
}

// TONG_GetMaster(nTongID) -> string ten bang chu (khong co -> chuoi rong)
int LuaTONG_GetMaster(Lua_State* L)
{
	KTongJX2Tong* pTong = g_TongJX2.FindTong(sArgTongID(L));
	if (pTong)
	{
		std::map<DWORD, KTongJX2Member>::iterator it;
		for (it = pTong->mapMember.begin(); it != pTong->mapMember.end(); ++it)
		{
			if (it->second.btFigure == 0)
			{
				Lua_PushString(L, it->second.szName);
				return 1;
			}
		}
	}
	Lua_PushString(L, (char*)"");
	return 1;
}

//////////////////////////////////////////////////////////////////////
// Ham Lua - field bang (doc)
//////////////////////////////////////////////////////////////////////

// TONG_GetTaskValue(nTongID, nTaskID) -> int CO dau
int LuaTONG_GetTaskValue(Lua_State* L)
{
	WORD wKey = 0;
	if (Lua_IsNumber(L, 2))
		wKey = (WORD)Lua_ValueToNumber(L, 2);
	Lua_PushNumber(L, (double)(int)g_TongJX2.GetField(sArgTongID(L), wKey));
	return 1;
}

// TONG_GetUTaskValue(nTongID, nTaskID) -> DWORD khong dau
int LuaTONG_GetUTaskValue(Lua_State* L)
{
	WORD wKey = 0;
	if (Lua_IsNumber(L, 2))
		wKey = (WORD)Lua_ValueToNumber(L, 2);
	Lua_PushNumber(L, (double)g_TongJX2.GetField(sArgTongID(L), wKey));
	return 1;
}

// TONG_GetTaskTemp(nTongID, nIdx 0..255) -> number (bo dem cuc bo, khong dong bo)
int LuaTONG_GetTaskTemp(Lua_State* L)
{
	KTongJX2Tong* pTong = g_TongJX2.FindTong(sArgTongID(L));
	int nIdx = -1;
	if (Lua_IsNumber(L, 2))
		nIdx = (int)Lua_ValueToNumber(L, 2);
	if (!pTong || nIdx < 0 || nIdx > 255)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, (double)pTong->dwTaskTemp[nIdx]);
	return 1;
}

// TONG_SetTaskTemp(nTongID, nIdx, nValue) - mat khi restart, lech giua cac GS (dung JX2)
int LuaTONG_SetTaskTemp(Lua_State* L)
{
	KTongJX2Tong* pTong = g_TongJX2.FindTong(sArgTongID(L));
	int nIdx = -1;
	if (Lua_IsNumber(L, 2))
		nIdx = (int)Lua_ValueToNumber(L, 2);
	if (!pTong || nIdx < 0 || nIdx > 255 || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	pTong->dwTaskTemp[nIdx] = (DWORD)Lua_ValueToNumber(L, 3);
	Lua_PushNumber(L, 1);
	return 1;
}

// cac ham doc field co ten rieng (bang field JX2 - BANGHOI_JX2_PHANTICH.md 2.1)
static int sPushField(Lua_State* L, WORD wKey)
{
	Lua_PushNumber(L, (double)(int)g_TongJX2.GetField(sArgTongID(L), wKey));
	return 1;
}

int LuaTONG_GetCredit(Lua_State* L)			{ return sPushField(L, 5); }
int LuaTONG_GetExp(Lua_State* L)			{ return sPushField(L, 6); }
int LuaTONG_GetUnionID(Lua_State* L)		{ return sPushField(L, 10); }
int LuaTONG_GetWarState(Lua_State* L)		{ return sPushField(L, 11); }
int LuaTONG_GetBuildFund(Lua_State* L)		{ return sPushField(L, 12); }
int LuaTONG_GetBuildLevel(Lua_State* L)		{ return sPushField(L, 13); }
int LuaTONG_GetWarBuildFund(Lua_State* L)	{ return sPushField(L, 15); }
int LuaTONG_GetMaintainFund(Lua_State* L)	{ return sPushField(L, 16); }
int LuaTONG_GetPerStandFund(Lua_State* L)	{ return sPushField(L, 17); }
int LuaTONG_GetStoredOffer(Lua_State* L)	{ return sPushField(L, 18); }
int LuaTONG_GetStoredBuildFund(Lua_State* L){ return sPushField(L, 19); }
int LuaTONG_GetDay(Lua_State* L)			{ return sPushField(L, 20); }
int LuaTONG_GetWeek(Lua_State* L)			{ return sPushField(L, 21); }
int LuaTONG_GetWeekBuildFund(Lua_State* L)	{ return sPushField(L, 41); }
int LuaTONG_GetWeekBuildUpper(Lua_State* L)	{ return sPushField(L, 42); }
int LuaTONG_GetTotalBuildFund(Lua_State* L)	{ return sPushField(L, 43); }
int LuaTONG_GetPauseState(Lua_State* L)		{ return sPushField(L, 44); }
int LuaTONG_GetTongMap(Lua_State* L)		{ return sPushField(L, 45); }
int LuaTONG_GetTongMapTemplate(Lua_State* L){ return sPushField(L, 46); }
int LuaTONG_GetTongMapBan(Lua_State* L)		{ return sPushField(L, 47); }
int LuaTONG_GetOccupyCityDay(Lua_State* L)	{ return sPushField(L, 48); }

// TONG_GetMoney(nTongID) -> number (int64 ghep field 4 cao + field 3 thap)
int LuaTONG_GetMoney(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	__int64 nMoney = ((__int64)g_TongJX2.GetField(dwTongID, 4) << 32)
		| (__int64)g_TongJX2.GetField(dwTongID, 3);
	Lua_PushNumber(L, (double)nMoney);
	return 1;
}

// TONG_GetStandFund(nTongID) -> (soTV - soAnSi) * PerStandFund (cong thuc JX2, khong phai field)
int LuaTONG_GetStandFund(Lua_State* L)
{
	KTongJX2Tong* pTong = g_TongJX2.FindTong(sArgTongID(L));
	if (!pTong)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nRetire = 0;
	std::map<DWORD, KTongJX2Member>::iterator it;
	for (it = pTong->mapMember.begin(); it != pTong->mapMember.end(); ++it)
	{
		if (it->second.btFigure == 4)
			nRetire++;
	}
	DWORD dwPer = 0;
	std::map<WORD, DWORD>::iterator itF = pTong->mapField.find(17);
	if (itF != pTong->mapField.end())
		dwPer = itF->second;
	Lua_PushNumber(L, (double)(((int)pTong->mapMember.size() - nRetire) * (int)dwPer));
	return 1;
}

//////////////////////////////////////////////////////////////////////
// Ham Lua - field bang (ghi, ho Apply: CHI gui len Relay)
//////////////////////////////////////////////////////////////////////

// TONG_ApplySetTaskValue(nTongID, nTaskID, nValue) -> 0/1
int LuaTONG_ApplySetTaskValue(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	if (!g_TongJX2.FindTong(dwTongID) || !Lua_IsNumber(L, 2) || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	WORD wKey = (WORD)Lua_ValueToNumber(L, 2);
	DWORD dwValue = (DWORD)Lua_ValueToNumber(L, 3);
	// toi uu JX2: gia tri khong doi thi khong gui goi
	if (g_TongJX2.GetField(dwTongID, wKey) == dwValue)
	{
		Lua_PushNumber(L, 1);
		return 1;
	}
	sSendFieldCmd(dwTongID, wKey, dwValue, defTONG_JX2_OP_SET, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// TONG_ApplyAddTaskValue(nTongID, nTaskID, nDelta) -> 0/1 (co dau)
int LuaTONG_ApplyAddTaskValue(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	if (!g_TongJX2.FindTong(dwTongID) || !Lua_IsNumber(L, 2) || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	WORD wKey = (WORD)Lua_ValueToNumber(L, 2);
	int nDelta = (int)Lua_ValueToNumber(L, 3);
	if (nDelta != 0)
		sSendFieldCmd(dwTongID, wKey, (DWORD)nDelta, defTONG_JX2_OP_ADD, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// TONG_ApplyAddUTaskValue(nTongID, nTaskID, nDelta) -> 0/1 (khong dau)
int LuaTONG_ApplyAddUTaskValue(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	if (!g_TongJX2.FindTong(dwTongID) || !Lua_IsNumber(L, 2) || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	WORD wKey = (WORD)Lua_ValueToNumber(L, 2);
	DWORD dwDelta = (DWORD)Lua_ValueToNumber(L, 3);
	if (dwDelta != 0)
		sSendFieldCmd(dwTongID, wKey, dwDelta, defTONG_JX2_OP_ADDU, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// TONG_ApplySetMoney(nTongID, nMoney) -> 0/1 (chan gia tri am - dung JX2)
int LuaTONG_ApplySetMoney(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	if (!g_TongJX2.FindTong(dwTongID) || !Lua_IsNumber(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	double dMoney = Lua_ValueToNumber(L, 2);
	if (dMoney < 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendMoneyCmd(dwTongID, (__int64)dMoney, defTONG_JX2_OP_SET, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// TONG_ApplyAddMoney(nTongID, nDelta) -> 0/1 (am duoc; ket qua duoi 0 se bi chan ve 0)
int LuaTONG_ApplyAddMoney(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	if (!g_TongJX2.FindTong(dwTongID) || !Lua_IsNumber(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	double dDelta = Lua_ValueToNumber(L, 2);
	if (dDelta != 0)
		sSendMoneyCmd(dwTongID, (__int64)dDelta, defTONG_JX2_OP_ADD, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

//////////////////////////////////////////////////////////////////////
// Ham Lua - thanh vien
//////////////////////////////////////////////////////////////////////

static KTongJX2Member* sResolveMember(Lua_State* L, KTongJX2Tong** ppTong)
{
	KTongJX2Tong* pTong = g_TongJX2.FindTong(sArgTongID(L));
	if (ppTong)
		*ppTong = pTong;
	if (!pTong)
		return NULL;
	return g_TongJX2.FindMember(pTong, sArgMemberID(L, 2));
}

// TONGM_IsExist(nTongID, member) -> 0/1 (LUON day so, khong bao gio nil - dung JX2)
int LuaTONGM_IsExist(Lua_State* L)
{
	Lua_PushNumber(L, sResolveMember(L, NULL) ? 1 : 0);
	return 1;
}

// TONGM_GetName(nTongID, nMemberID) -> string
int LuaTONGM_GetName(Lua_State* L)
{
	KTongJX2Member* pMember = sResolveMember(L, NULL);
	Lua_PushString(L, pMember ? pMember->szName : (char*)"");
	return 1;
}

// TONGM_GetFigure(nTongID, member) -> 0..4 (-1 neu khong co)
int LuaTONGM_GetFigure(Lua_State* L)
{
	KTongJX2Member* pMember = sResolveMember(L, NULL);
	Lua_PushNumber(L, pMember ? (double)pMember->btFigure : -1);
	return 1;
}

// TONGM_GetSex(nTongID, member) -> 0/1 (khoa 1 neu co, khong thi lay tu ho so)
int LuaTONGM_GetSex(Lua_State* L)
{
	KTongJX2Member* pMember = sResolveMember(L, NULL);
	if (!pMember)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	DWORD dwSex = g_TongJX2.GetMemberField(pMember, 1);
	if (dwSex == 0)
		dwSex = pMember->btSex;
	Lua_PushNumber(L, (double)dwSex);
	return 1;
}

// TONGM_GetOnline(nTongID, member) -> 0/1 (gioi han: chi biet online tren GS nay)
int LuaTONGM_GetOnline(Lua_State* L)
{
	KTongJX2Member* pMember = sResolveMember(L, NULL);
	Lua_PushNumber(L, (pMember && sIsRoleOnlineLocal(pMember->szName)) ? 1 : 0);
	return 1;
}

// TONGM_GetOffer(nTongID, member) -> DWORD (khoa 7 - cong hien tich luy)
int LuaTONGM_GetOffer(Lua_State* L)
{
	KTongJX2Member* pMember = sResolveMember(L, NULL);
	Lua_PushNumber(L, (double)g_TongJX2.GetMemberField(pMember, 7));
	return 1;
}

// TONGM_GetTaskValue(nTongID, member, wKey) -> int CO dau
int LuaTONGM_GetTaskValue(Lua_State* L)
{
	KTongJX2Member* pMember = sResolveMember(L, NULL);
	WORD wKey = 0;
	if (Lua_IsNumber(L, 3))
		wKey = (WORD)Lua_ValueToNumber(L, 3);
	Lua_PushNumber(L, (double)(int)g_TongJX2.GetMemberField(pMember, wKey));
	return 1;
}

// TONGM_GetUTaskValue(nTongID, member, wKey) -> DWORD
int LuaTONGM_GetUTaskValue(Lua_State* L)
{
	KTongJX2Member* pMember = sResolveMember(L, NULL);
	WORD wKey = 0;
	if (Lua_IsNumber(L, 3))
		wKey = (WORD)Lua_ValueToNumber(L, 3);
	Lua_PushNumber(L, (double)g_TongJX2.GetMemberField(pMember, wKey));
	return 1;
}

// TONGM_CheckRight(nTongID, member, nRightID) -> 0/1
// DUNG JX2 (13.2): khong tim thay thanh vien HOAC Figure == 0 -> tra 1 NGAY.
int LuaTONGM_CheckRight(Lua_State* L)
{
	KTongJX2Member* pMember = sResolveMember(L, NULL);
	if (!pMember || pMember->btFigure == 0)
	{
		Lua_PushNumber(L, 1);
		return 1;
	}
	DWORD dwRightID = 0;
	if (Lua_IsNumber(L, 3))
		dwRightID = (DWORD)Lua_ValueToNumber(L, 3);
	Lua_PushNumber(L, pMember->setRight.count(dwRightID) ? 1 : 0);
	return 1;
}

// TONGM_ApplySetTaskValue(nTongID, member, wKey, nValue) -> 0/1
int LuaTONGM_ApplySetTaskValue(Lua_State* L)
{
	KTongJX2Tong* pTong = NULL;
	KTongJX2Member* pMember = sResolveMember(L, &pTong);
	if (!pTong || !pMember || !Lua_IsNumber(L, 3) || !Lua_IsNumber(L, 4))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	WORD wKey = (WORD)Lua_ValueToNumber(L, 3);
	DWORD dwValue = (DWORD)Lua_ValueToNumber(L, 4);
	if (g_TongJX2.GetMemberField(pMember, wKey) == dwValue)
	{
		Lua_PushNumber(L, 1);
		return 1;
	}
	sSendMemberFieldCmd(pTong->dwNameID, pMember->dwNameID, wKey, dwValue,
		defTONG_JX2_OP_SET, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// TONGM_ApplyAddTaskValue(nTongID, member, wKey, nDelta) -> 0/1
int LuaTONGM_ApplyAddTaskValue(Lua_State* L)
{
	KTongJX2Tong* pTong = NULL;
	KTongJX2Member* pMember = sResolveMember(L, &pTong);
	if (!pTong || !pMember || !Lua_IsNumber(L, 3) || !Lua_IsNumber(L, 4))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	WORD wKey = (WORD)Lua_ValueToNumber(L, 3);
	int nDelta = (int)Lua_ValueToNumber(L, 4);
	if (nDelta != 0)
		sSendMemberFieldCmd(pTong->dwNameID, pMember->dwNameID, wKey, (DWORD)nDelta,
			defTONG_JX2_OP_ADD, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// TONGM_ApplyAddOffer(nTongID, member, nDelta) -> 0/1 (khoa 7, cho phep am)
int LuaTONGM_ApplyAddOffer(Lua_State* L)
{
	KTongJX2Tong* pTong = NULL;
	KTongJX2Member* pMember = sResolveMember(L, &pTong);
	if (!pTong || !pMember || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nDelta = (int)Lua_ValueToNumber(L, 3);
	if (nDelta != 0)
		sSendMemberFieldCmd(pTong->dwNameID, pMember->dwNameID, 7, (DWORD)nDelta,
			defTONG_JX2_OP_ADD, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// TONGM_ApplySetRetireDate(nTongID, member, nDate) = SetTaskValue khoa 16
int LuaTONGM_ApplySetRetireDate(Lua_State* L)
{
	KTongJX2Tong* pTong = NULL;
	KTongJX2Member* pMember = sResolveMember(L, &pTong);
	if (!pTong || !pMember || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendMemberFieldCmd(pTong->dwNameID, pMember->dwNameID, 16,
		(DWORD)Lua_ValueToNumber(L, 3), defTONG_JX2_OP_SET, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// TONGM_ApplySetLastOnlineDate(nTongID, member, nDate) = SetTaskValue khoa 15
int LuaTONGM_ApplySetLastOnlineDate(Lua_State* L)
{
	KTongJX2Tong* pTong = NULL;
	KTongJX2Member* pMember = sResolveMember(L, &pTong);
	if (!pTong || !pMember || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendMemberFieldCmd(pTong->dwNameID, pMember->dwNameID, 15,
		(DWORD)Lua_ValueToNumber(L, 3), defTONG_JX2_OP_SET, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// TONGM_ApplyAddRight(nTongID, member, nRightID) -> 0/1
int LuaTONGM_ApplyAddRight(Lua_State* L)
{
	KTongJX2Tong* pTong = NULL;
	KTongJX2Member* pMember = sResolveMember(L, &pTong);
	if (!pTong || !pMember || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendRightCmd(pTong->dwNameID, pMember->dwNameID,
		(DWORD)Lua_ValueToNumber(L, 3), 1, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// TONGM_ApplyDelRight(nTongID, member, nRightID) -> 0/1
int LuaTONGM_ApplyDelRight(Lua_State* L)
{
	KTongJX2Tong* pTong = NULL;
	KTongJX2Member* pMember = sResolveMember(L, &pTong);
	if (!pTong || !pMember || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendRightCmd(pTong->dwNameID, pMember->dwNameID,
		(DWORD)Lua_ValueToNumber(L, 3), 0, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

//////////////////////////////////////////////////////////////////////
// DOT 2 - phan con lai cua 150 ham + 29 ham TWS_ (tac phuong)
//////////////////////////////////////////////////////////////////////

static void sSendStringCmd(DWORD dwTongID, BYTE btKind, const char* pszText, DWORD dwParam)
{
	STONG_JX2_STRING_COMMAND sCmd;
	memset(&sCmd, 0, sizeof(sCmd));
	sCmd.ProtocolFamily = pf_tong;
	sCmd.ProtocolID = enumC2S_TONG_JX2_STRING;
	sCmd.m_dwTongNameID = dwTongID;
	sCmd.m_btKind = btKind;
	if (pszText)
		strncpy(sCmd.m_szText, pszText, defTONG_JX2_ANNOUNCE_LEN - 1);
	sCmd.m_dwParam = dwParam;
	g_NewProtocolProcess.PushMsgInTong((const void*)&sCmd, sizeof(sCmd));
}

static void sSendTongOp(DWORD dwTongID, DWORD dwMemberID, BYTE btOpCode, int nParam1, int nParam2, DWORD dwParam, const char* pszName = NULL)
{
	STONG_JX2_TONG_OP_COMMAND sCmd;
	memset(&sCmd, 0, sizeof(sCmd));
	sCmd.ProtocolFamily = pf_tong;
	sCmd.ProtocolID = enumC2S_TONG_JX2_TONG_OP;
	sCmd.m_dwTongNameID = dwTongID;
	sCmd.m_dwMemberNameID = dwMemberID;
	sCmd.m_btOpCode = btOpCode;
	sCmd.m_nParam1 = nParam1;
	sCmd.m_nParam2 = nParam2;
	sCmd.m_dwParam = dwParam;
	if (pszName)
	{
		strncpy(sCmd.m_szName, pszName, sizeof(sCmd.m_szName) - 1);
		sCmd.m_szName[sizeof(sCmd.m_szName) - 1] = 0;
	}
	g_NewProtocolProcess.PushMsgInTong((const void*)&sCmd, sizeof(sCmd));
}

// ApplySet<X>(nTongID, nValue) voi field co dinh
static int sApplySetFieldArg2(Lua_State* L, WORD wKey)
{
	DWORD dwTongID = sArgTongID(L);
	if (!g_TongJX2.FindTong(dwTongID) || !Lua_IsNumber(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	DWORD dwValue = (DWORD)Lua_ValueToNumber(L, 2);
	if (g_TongJX2.GetField(dwTongID, wKey) != dwValue)
		sSendFieldCmd(dwTongID, wKey, dwValue, defTONG_JX2_OP_SET, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// ApplyAdd<X>(nTongID, nDelta) voi field co dinh
static int sApplyAddFieldArg2(Lua_State* L, WORD wKey, BOOL bUnsigned)
{
	DWORD dwTongID = sArgTongID(L);
	if (!g_TongJX2.FindTong(dwTongID) || !Lua_IsNumber(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nDelta = (int)Lua_ValueToNumber(L, 2);
	if (nDelta != 0)
		sSendFieldCmd(dwTongID, wKey, (DWORD)nDelta,
			bUnsigned ? defTONG_JX2_OP_ADDU : defTONG_JX2_OP_ADD, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

#define DEF_TONG_GETF(FuncName, nField) \
	int LuaTONG_Get##FuncName(Lua_State* L) { return sPushField(L, nField); }
#define DEF_TONG_SETF(FuncName, nField) \
	int LuaTONG_ApplySet##FuncName(Lua_State* L) { return sApplySetFieldArg2(L, nField); }
#define DEF_TONG_ADDF(FuncName, nField, bUns) \
	int LuaTONG_ApplyAdd##FuncName(Lua_State* L) { return sApplyAddFieldArg2(L, nField, bUns); }

// ---- getters theo bang field JX2 (2.1) ----
DEF_TONG_GETF(SelfCamp, 1)
DEF_TONG_GETF(CurCamp, 2)
DEF_TONG_GETF(ExpLevel, 6)			// theo tai lieu: cung o field 6 voi GetExp
DEF_TONG_GETF(Premium, 14)
DEF_TONG_GETF(CurWeekGoalLevel, 36)
DEF_TONG_GETF(WeekGoalEvent, 22)
DEF_TONG_GETF(WeekGoalLevel, 23)
DEF_TONG_GETF(WeekGoalTotal, 24)
DEF_TONG_GETF(WeekGoalPlayer, 25)
DEF_TONG_GETF(WeekGoalValue, 26)
DEF_TONG_GETF(WeekGoalPriceTong, 27)
DEF_TONG_GETF(WeekGoalPricePlayer, 28)
DEF_TONG_GETF(LWeekGoalEvent, 29)
DEF_TONG_GETF(LWeekGoalLevel, 30)
DEF_TONG_GETF(LWeekGoalTotal, 31)
DEF_TONG_GETF(LWeekGoalPlayer, 32)
DEF_TONG_GETF(LWeekGoalValue, 33)
DEF_TONG_GETF(LWeekGoalPriceTong, 34)
DEF_TONG_GETF(LWeekGoalPricePlayer, 35)

// ---- setters ----
DEF_TONG_SETF(BuildFund, 12)
DEF_TONG_SETF(CurWeekGoalLevel, 36)
DEF_TONG_SETF(Day, 20)
DEF_TONG_SETF(Week, 21)
DEF_TONG_SETF(MaintainFund, 16)
DEF_TONG_SETF(OccupyCityDay, 48)
DEF_TONG_SETF(PauseState, 44)
DEF_TONG_SETF(PerStandFund, 17)
DEF_TONG_SETF(StoredBuildFund, 19)
DEF_TONG_SETF(StoredOffer, 18)
DEF_TONG_SETF(TotalBuildFund, 43)
DEF_TONG_SETF(WarBuildFund, 15)
DEF_TONG_SETF(WeekBuildFund, 41)
DEF_TONG_SETF(WeekBuildUpper, 42)
DEF_TONG_SETF(WeekGoalEvent, 22)
DEF_TONG_SETF(WeekGoalLevel, 23)
DEF_TONG_SETF(WeekGoalTotal, 24)
DEF_TONG_SETF(WeekGoalPlayer, 25)
DEF_TONG_SETF(WeekGoalValue, 26)
DEF_TONG_SETF(WeekGoalPriceTong, 27)
DEF_TONG_SETF(WeekGoalPricePlayer, 28)
DEF_TONG_SETF(LWeekGoalEvent, 29)
DEF_TONG_SETF(LWeekGoalLevel, 30)
DEF_TONG_SETF(LWeekGoalTotal, 31)
DEF_TONG_SETF(LWeekGoalPlayer, 32)
DEF_TONG_SETF(LWeekGoalValue, 33)
DEF_TONG_SETF(LWeekGoalPriceTong, 34)
DEF_TONG_SETF(LWeekGoalPricePlayer, 35)
DEF_TONG_SETF(TongMap, 45)
DEF_TONG_SETF(TongMapBan, 47)

// ---- adders (AddBuildFund/AddWarBuildFund/AddPerStandFund la cong KHONG dau - JX2 vfunc +0xb0) ----
DEF_TONG_ADDF(BuildFund, 12, TRUE)
DEF_TONG_ADDF(WarBuildFund, 15, TRUE)
DEF_TONG_ADDF(PerStandFund, 17, TRUE)
DEF_TONG_ADDF(Day, 20, TRUE)
DEF_TONG_ADDF(Week, 21, TRUE)
DEF_TONG_ADDF(StoredBuildFund, 19, FALSE)
DEF_TONG_ADDF(StoredOffer, 18, FALSE)
DEF_TONG_ADDF(TotalBuildFund, 43, FALSE)
DEF_TONG_ADDF(WeekBuildFund, 41, FALSE)
DEF_TONG_ADDF(WeekGoalValue, 26, FALSE)
DEF_TONG_ADDF(LWeekGoalValue, 33, FALSE)

// TONG_GetAnnouncement(nTongID) -> string (bo sung ngoai 150 - phuc vu cua so client)
int LuaTONG_GetAnnouncement(Lua_State* L)
{
	KTongJX2Tong* pTong = g_TongJX2.FindTong(sArgTongID(L));
	Lua_PushString(L, pTong ? pTong->szAnnounce : (char*)"");
	return 1;
}

// TONG_ApplySetAnnouncement(nTongID, szText) -> 0/1
int LuaTONG_ApplySetAnnouncement(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	if (!g_TongJX2.FindTong(dwTongID) || !Lua_IsString(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendStringCmd(dwTongID, defTONG_JX2_STR_ANNOUNCE,
		(const char*)Lua_ValueToString(L, 2), sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// TONG_ApplyAddEventRecord(nTongID, szText) / TONG_ApplyAddHistoryRecord(nTongID, szText)
int LuaTONG_ApplyAddEventRecord(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	if (!g_TongJX2.FindTong(dwTongID) || !Lua_IsString(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendStringCmd(dwTongID, defTONG_JX2_STR_EVENT,
		(const char*)Lua_ValueToString(L, 2), sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaTONG_ApplyAddHistoryRecord(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	if (!g_TongJX2.FindTong(dwTongID) || !Lua_IsString(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendStringCmd(dwTongID, defTONG_JX2_STR_HISTORY,
		(const char*)Lua_ValueToString(L, 2), sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// TONG_AddTaskTemp(nTongID, nIdx, nDelta) - bo dem cuc bo
int LuaTONG_AddTaskTemp(Lua_State* L)
{
	KTongJX2Tong* pTong = g_TongJX2.FindTong(sArgTongID(L));
	int nIdx = -1;
	if (Lua_IsNumber(L, 2))
		nIdx = (int)Lua_ValueToNumber(L, 2);
	if (!pTong || nIdx < 0 || nIdx > 255 || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	pTong->dwTaskTemp[nIdx] += (int)Lua_ValueToNumber(L, 3);
	Lua_PushNumber(L, 1);
	return 1;
}

// TONG_WriteLog(szText) - ghi file logs\tong_jx2.log phia GameServer
int LuaTONG_WriteLog(Lua_State* L)
{
	if (!Lua_IsString(L, 1))
		return 0;
	const char* pszText = (const char*)Lua_ValueToString(L, 1);
	if (!pszText || !pszText[0])
		return 0;
	FILE* pFile = fopen("logs\\tong_jx2.log", "at");
	if (!pFile)
		pFile = fopen("tong_jx2.log", "at");
	if (pFile)
	{
		time_t tNow = time(NULL);
		struct tm* pTm = localtime(&tNow);
		fprintf(pFile, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
			pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday,
			pTm->tm_hour, pTm->tm_min, pTm->tm_sec, pszText);
		fclose(pFile);
	}
	return 0;
}

// ---- nhom thao tac tong hop (vong doi / phan phoi / ban do / tuyet ky) ----

static int sTongOpSimple(Lua_State* L, BYTE btOpCode)
{
	DWORD dwTongID = sArgTongID(L);
	if (!g_TongJX2.FindTong(dwTongID))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendTongOp(dwTongID, 0, btOpCode, 0, 0, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaTONG_ApplyInit(Lua_State* L)			{ return sTongOpSimple(L, defTONG_JX2_TOP_INIT); }
int LuaTONG_ApplyUpgrade(Lua_State* L)		{ return sTongOpSimple(L, defTONG_JX2_TOP_UPGRADE); }
int LuaTONG_ApplyDegrade(Lua_State* L)		{ return sTongOpSimple(L, defTONG_JX2_TOP_DEGRADE); }
int LuaTONG_ApplyMaintain(Lua_State* L)		{ return sTongOpSimple(L, defTONG_JX2_TOP_MAINTAIN); }
int LuaTONG_ApplyWeeklyMaintain(Lua_State* L){ return sTongOpSimple(L, defTONG_JX2_TOP_WEEKLY); }
int LuaTONG_ApplyDeleteMap(Lua_State* L)	{ return sTongOpSimple(L, defTONG_JX2_TOP_DELETE_MAP); }

// TONG_ApplyKickMember(nTongID, member, nParam) / TONG_ApplyDeleteMember - duoi khoi bang
static int sTongOpKick(Lua_State* L)
{
	KTongJX2Tong* pTong = NULL;
	KTongJX2Member* pMember = sResolveMember(L, &pTong);
	if (!pTong || !pMember)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendTongOp(pTong->dwNameID, pMember->dwNameID, defTONG_JX2_TOP_KICK, 0, 0, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}
int LuaTONG_ApplyKickMember(Lua_State* L)	{ return sTongOpKick(L); }
int LuaTONG_ApplyDeleteMember(Lua_State* L)	{ return sTongOpKick(L); }

// TONG_ApplySetStunt(nTongID, nStuntID)
int LuaTONG_ApplySetStunt(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	if (!g_TongJX2.FindTong(dwTongID) || !Lua_IsNumber(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendTongOp(dwTongID, 0, defTONG_JX2_TOP_SET_STUNT, (int)Lua_ValueToNumber(L, 2), 0, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// TONG_ApplyCreatMap(nTongID, nMapTemplate)
int LuaTONG_ApplyCreatMap(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	if (!g_TongJX2.FindTong(dwTongID) || !Lua_IsNumber(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendTongOp(dwTongID, 0, defTONG_JX2_TOP_CREATE_MAP, (int)Lua_ValueToNumber(L, 2), 0, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// TONG_ChangeAllMemberFeature(nTongID, nFeature, nGiay)
int LuaTONG_ChangeAllMemberFeature(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	if (!g_TongJX2.FindTong(dwTongID) || !Lua_IsNumber(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nTime = 0;
	if (Lua_IsNumber(L, 3))
		nTime = (int)Lua_ValueToNumber(L, 3);
	sSendTongOp(dwTongID, 0, defTONG_JX2_TOP_FEATURE, (int)Lua_ValueToNumber(L, 2), nTime, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// TONG_ContributeOffer(nTongID, nExecutorID, nOffer) - cong vao quy du tru (field 18)
int LuaTONG_ContributeOffer(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	if (!g_TongJX2.FindTong(dwTongID) || Lua_GetTopIndex(L) < 3 || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nOffer = (int)Lua_ValueToNumber(L, 3);
	if (nOffer <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendTongOp(dwTongID, sArgMemberID(L, 2), defTONG_JX2_TOP_CONTRIBUTE, nOffer, 0, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// TONG_DistributeOfferToGroup(nTongID, nFigure, nOffer) - tru quy du tru, phat cho nhom
int LuaTONG_DistributeOfferToGroup(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	if (!g_TongJX2.FindTong(dwTongID) || !Lua_IsNumber(L, 2) || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendTongOp(dwTongID, 0, defTONG_JX2_TOP_DIST_GROUP,
		(int)Lua_ValueToNumber(L, 3), (int)Lua_ValueToNumber(L, 2), sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// TONG_DistributeOfferToMember(nTongID, member, nOffer)
int LuaTONG_DistributeOfferToMember(Lua_State* L)
{
	KTongJX2Tong* pTong = NULL;
	KTongJX2Member* pMember = sResolveMember(L, &pTong);
	if (!pTong || !pMember || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendTongOp(pTong->dwNameID, pMember->dwNameID, defTONG_JX2_TOP_DIST_MEMBER,
		(int)Lua_ValueToNumber(L, 3), 0, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// ---- TONGM_ dot 2 ----

#define DEF_TONGM_GETK(FuncName, nKey) \
	int LuaTONGM_Get##FuncName(Lua_State* L) \
	{ \
		KTongJX2Member* pMember = sResolveMember(L, NULL); \
		Lua_PushNumber(L, (double)(int)g_TongJX2.GetMemberField(pMember, nKey)); \
		return 1; \
	}

DEF_TONGM_GETK(JoinTime, 2)
DEF_TONGM_GETK(JoinDay, 3)
DEF_TONGM_GETK(Money, 4)
DEF_TONGM_GETK(LWeekGoalOffer, 10)
DEF_TONGM_GETK(LWeeklyOffer, 12)
DEF_TONGM_GETK(LastOnlineDate, 15)
DEF_TONGM_GETK(RetireDate, 16)

// TONGM_ApplySet<X>(nTongID, member, nValue) voi khoa co dinh
static int sApplySetMemberKeyArg3(Lua_State* L, WORD wKey)
{
	KTongJX2Tong* pTong = NULL;
	KTongJX2Member* pMember = sResolveMember(L, &pTong);
	if (!pTong || !pMember || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendMemberFieldCmd(pTong->dwNameID, pMember->dwNameID, wKey,
		(DWORD)Lua_ValueToNumber(L, 3), defTONG_JX2_OP_SET, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaTONGM_ApplySetLWeekGoalOffer(Lua_State* L)	{ return sApplySetMemberKeyArg3(L, 10); }
int LuaTONGM_ApplySetLWeeklyOffer(Lua_State* L)		{ return sApplySetMemberKeyArg3(L, 12); }

// TONGM_ApplyAddUTaskValue(nTongID, member, wKey, nDelta) - cong khong dau
int LuaTONGM_ApplyAddUTaskValue(Lua_State* L)
{
	KTongJX2Tong* pTong = NULL;
	KTongJX2Member* pMember = sResolveMember(L, &pTong);
	if (!pTong || !pMember || !Lua_IsNumber(L, 3) || !Lua_IsNumber(L, 4))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	WORD wKey = (WORD)Lua_ValueToNumber(L, 3);
	DWORD dwDelta = (DWORD)Lua_ValueToNumber(L, 4);
	if (dwDelta != 0)
		sSendMemberFieldCmd(pTong->dwNameID, pMember->dwNameID, wKey, dwDelta,
			defTONG_JX2_OP_ADDU, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// TONGM_ApplyAddOfferEx(nTongID, nFigure, nOffer) - cong cong hien cho TUNG nguoi nhom chuc vu
int LuaTONGM_ApplyAddOfferEx(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	if (!g_TongJX2.FindTong(dwTongID) || !Lua_IsNumber(L, 2) || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendTongOp(dwTongID, 0, defTONG_JX2_TOP_ADD_OFFER_FIG,
		(int)Lua_ValueToNumber(L, 3), (int)Lua_ValueToNumber(L, 2), sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

//////////////////////////////////////////////////////////////////////
// TWS_ - tac phuong (workshop), ma hoa vao dai field cua bang:
//   thuoc tinh = defTONG_JX2_WS_ATTR_BASE + nType*10 + attr
//   bien nhiem vu = defTONG_JX2_WS_TASK_BASE + nType*1000 + key (key < 1000)
//////////////////////////////////////////////////////////////////////

static int sWsType(Lua_State* L)
{
	if (!Lua_IsNumber(L, 2))
		return 0;
	int nType = (int)Lua_ValueToNumber(L, 2);
	if (nType < 1 || nType > defTONG_JX2_WS_MAX_TYPE)
		return 0;
	return nType;
}

static WORD sWsAttrField(int nType, int nAttr)
{
	return (WORD)(defTONG_JX2_WS_ATTR_BASE + nType * 10 + nAttr);
}

static int sWsPushAttr(Lua_State* L, int nAttr)
{
	int nType = sWsType(L);
	if (!nType)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, (double)(int)g_TongJX2.GetField(sArgTongID(L), sWsAttrField(nType, nAttr)));
	return 1;
}

// ApplySet attr voi gia tri o arg3
static int sWsApplySetAttrArg3(Lua_State* L, int nAttr)
{
	DWORD dwTongID = sArgTongID(L);
	int nType = sWsType(L);
	if (!g_TongJX2.FindTong(dwTongID) || !nType || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendFieldCmd(dwTongID, sWsAttrField(nType, nAttr),
		(DWORD)Lua_ValueToNumber(L, 3), defTONG_JX2_OP_SET, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaTWS_IsExist(Lua_State* L)
{
	int nType = sWsType(L);
	Lua_PushNumber(L, (nType && g_TongJX2.GetField(sArgTongID(L), sWsAttrField(nType, 0))) ? 1 : 0);
	return 1;
}

int LuaTWS_IsOpen(Lua_State* L)			{ return sWsPushAttr(L, 1); }
int LuaTWS_GetLevel(Lua_State* L)		{ return sWsPushAttr(L, 2); }
int LuaTWS_GetDayOutput(Lua_State* L)	{ return sWsPushAttr(L, 3); }
int LuaTWS_GetUseLevel(Lua_State* L)	{ return sWsPushAttr(L, 4); }
int LuaTWS_GetUseLevelSet(Lua_State* L)	{ return sWsPushAttr(L, 5); }

int LuaTWS_GetType(Lua_State* L)
{
	int nType = sWsType(L);
	if (nType && g_TongJX2.GetField(sArgTongID(L), sWsAttrField(nType, 0)))
		Lua_PushNumber(L, nType);
	else
		Lua_PushNumber(L, 0);
	return 1;
}

int LuaTWS_GetWorkshopCount(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	int nCount = 0;
	for (int t = 1; t <= defTONG_JX2_WS_MAX_TYPE; t++)
	{
		if (g_TongJX2.GetField(dwTongID, sWsAttrField(t, 0)))
			nCount++;
	}
	Lua_PushNumber(L, nCount);
	return 1;
}

int LuaTWS_GetFirstWorkshop(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	for (int t = 1; t <= defTONG_JX2_WS_MAX_TYPE; t++)
	{
		if (g_TongJX2.GetField(dwTongID, sWsAttrField(t, 0)))
		{
			Lua_PushNumber(L, t);
			return 1;
		}
	}
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaTWS_GetNextWorkshop(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	int nCur = sWsType(L);
	for (int t = nCur + 1; t >= 1 && t <= defTONG_JX2_WS_MAX_TYPE; t++)
	{
		if (g_TongJX2.GetField(dwTongID, sWsAttrField(t, 0)))
		{
			Lua_PushNumber(L, t);
			return 1;
		}
	}
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaTWS_GetBuildingNpc(Lua_State* L)
{
	KTongJX2Tong* pTong = g_TongJX2.FindTong(sArgTongID(L));
	int nType = sWsType(L);
	if (!pTong || !nType)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, (double)pTong->dwWsNpc[nType]);
	return 1;
}

// GS-cuc bo (NPC cong trinh chi co nghia tren GS dang giu ban do)
int LuaTWS_SetBuildingNpc(Lua_State* L)
{
	KTongJX2Tong* pTong = g_TongJX2.FindTong(sArgTongID(L));
	int nType = sWsType(L);
	if (!pTong || !nType || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	pTong->dwWsNpc[nType] = (DWORD)Lua_ValueToNumber(L, 3);
	Lua_PushNumber(L, 1);
	return 1;
}

static WORD sWsTaskField(int nType, int nKey)
{
	return (WORD)(defTONG_JX2_WS_TASK_BASE + nType * 1000 + nKey);
}

int LuaTWS_GetTaskValue(Lua_State* L)
{
	int nType = sWsType(L);
	int nKey = Lua_IsNumber(L, 3) ? (int)Lua_ValueToNumber(L, 3) : -1;
	if (!nType || nKey < 0 || nKey >= 1000)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, (double)(int)g_TongJX2.GetField(sArgTongID(L), sWsTaskField(nType, nKey)));
	return 1;
}

int LuaTWS_GetUTaskValue(Lua_State* L)
{
	int nType = sWsType(L);
	int nKey = Lua_IsNumber(L, 3) ? (int)Lua_ValueToNumber(L, 3) : -1;
	if (!nType || nKey < 0 || nKey >= 1000)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, (double)g_TongJX2.GetField(sArgTongID(L), sWsTaskField(nType, nKey)));
	return 1;
}

static int sWsApplyTask(Lua_State* L, BYTE btOp)
{
	DWORD dwTongID = sArgTongID(L);
	int nType = sWsType(L);
	int nKey = Lua_IsNumber(L, 3) ? (int)Lua_ValueToNumber(L, 3) : -1;
	if (!g_TongJX2.FindTong(dwTongID) || !nType || nKey < 0 || nKey >= 1000 || !Lua_IsNumber(L, 4))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendFieldCmd(dwTongID, sWsTaskField(nType, nKey),
		(DWORD)Lua_ValueToNumber(L, 4), btOp, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaTWS_ApplySetTaskValue(Lua_State* L)	{ return sWsApplyTask(L, defTONG_JX2_OP_SET); }
int LuaTWS_ApplyAddTaskValue(Lua_State* L)	{ return sWsApplyTask(L, defTONG_JX2_OP_ADD); }
int LuaTWS_ApplyAddUTaskValue(Lua_State* L)	{ return sWsApplyTask(L, defTONG_JX2_OP_ADDU); }

int LuaTWS_ApplySetDayOutput(Lua_State* L)	{ return sWsApplySetAttrArg3(L, 3); }
int LuaTWS_ApplySetUseLevel(Lua_State* L)	{ return sWsApplySetAttrArg3(L, 4); }
int LuaTWS_ApplySetUseLevelSet(Lua_State* L){ return sWsApplySetAttrArg3(L, 5); }

int LuaTWS_ApplyAddDayOutput(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	int nType = sWsType(L);
	if (!g_TongJX2.FindTong(dwTongID) || !nType || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendFieldCmd(dwTongID, sWsAttrField(nType, 3),
		(DWORD)(int)Lua_ValueToNumber(L, 3), defTONG_JX2_OP_ADD, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// TWS_ApplyAdd(nTongID, nType) - lap tac phuong (ton tai = 1, cap = 1)
int LuaTWS_ApplyAdd(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	int nType = sWsType(L);
	if (!g_TongJX2.FindTong(dwTongID) || !nType)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	if (g_TongJX2.GetField(dwTongID, sWsAttrField(nType, 0)))
	{
		Lua_PushNumber(L, 0);	// da co
		return 1;
	}
	DWORD dwParam = sLuaPlayerParam(L);
	sSendFieldCmd(dwTongID, sWsAttrField(nType, 0), 1, defTONG_JX2_OP_SET, dwParam);
	sSendFieldCmd(dwTongID, sWsAttrField(nType, 2), 1, defTONG_JX2_OP_SET, dwParam);
	sSendFieldCmd(dwTongID, sWsAttrField(nType, 1), 1, defTONG_JX2_OP_SET, dwParam);
	Lua_PushNumber(L, 1);
	return 1;
}

// TWS_ApplyRemove(nTongID, nType) - do bo (xoa 6 thuoc tinh)
int LuaTWS_ApplyRemove(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	int nType = sWsType(L);
	if (!g_TongJX2.FindTong(dwTongID) || !nType)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	DWORD dwParam = sLuaPlayerParam(L);
	for (int a = 0; a <= 5; a++)
		sSendFieldCmd(dwTongID, sWsAttrField(nType, a), 0, defTONG_JX2_OP_SET, dwParam);
	Lua_PushNumber(L, 1);
	return 1;
}

static int sWsSetOpen(Lua_State* L, DWORD dwValue)
{
	DWORD dwTongID = sArgTongID(L);
	int nType = sWsType(L);
	if (!g_TongJX2.FindTong(dwTongID) || !nType)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	if (!g_TongJX2.GetField(dwTongID, sWsAttrField(nType, 0)))
	{
		Lua_PushNumber(L, 0);	// chua lap
		return 1;
	}
	sSendFieldCmd(dwTongID, sWsAttrField(nType, 1), dwValue, defTONG_JX2_OP_SET, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaTWS_ApplyOpen(Lua_State* L)	{ return sWsSetOpen(L, 1); }
int LuaTWS_ApplyClose(Lua_State* L)	{ return sWsSetOpen(L, 0); }

int LuaTWS_ApplyUpgrade(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	int nType = sWsType(L);
	if (!g_TongJX2.FindTong(dwTongID) || !nType ||
		!g_TongJX2.GetField(dwTongID, sWsAttrField(nType, 0)))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendFieldCmd(dwTongID, sWsAttrField(nType, 2), 1, defTONG_JX2_OP_ADD, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaTWS_ApplyDegrade(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	int nType = sWsType(L);
	if (!g_TongJX2.FindTong(dwTongID) || !nType)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	if ((int)g_TongJX2.GetField(dwTongID, sWsAttrField(nType, 2)) <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendFieldCmd(dwTongID, sWsAttrField(nType, 2), (DWORD)-1, defTONG_JX2_OP_ADD, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

// bao tri tac phuong: chi phi da nam trong bao tri ngay cua bang
int LuaTWS_ApplyMaintain(Lua_State* L)
{
	Lua_PushNumber(L, g_TongJX2.FindTong(sArgTongID(L)) ? 1 : 0);
	return 1;
}

//////////////////////////////////////////////////////////////////////
// Phuc vu cua so client JX2 (goi tu KSOServer qua GetGameData)
//////////////////////////////////////////////////////////////////////

// 14 RightID DUNG THU TU ban goc (doc byte tho blueprint trang phan quyen;
// bo 1000/2007 khong ton tai trong ban goc, them 1002/1004/1903/2003).
// Thu tu nay = thu tu BIT tren day - client s_dwRightId PHAI trung khop.
static const DWORD s_dwJX2RightList[defTONG_JX2_RIGHT_COUNT] =
	{1002, 1003, 1004, 1901, 1903, 1101, 2001,
	 1902, 2004, 9001, 3001, 2005, 2003, 2006};

static WORD sJX2_RightMask(KTongJX2Member* pMember)
{
	if (!pMember)
		return 0;
	if (pMember->btFigure == 0)
		return 0xFFFF;	// bang chu: moi quyen (dung JX2 13.2)
	WORD wMask = 0;
	for (int i = 0; i < defTONG_JX2_RIGHT_COUNT; i++)
	{
		if (pMember->setRight.count(s_dwJX2RightList[i]))
			wMask |= (WORD)(1 << i);
	}
	return wMask;
}

// bang + ho so thanh vien cua nguoi choi nPlayerIdx (theo ten nhan vat)
static KTongJX2Tong* sJX2_PlayerTong(int nPlayerIdx, KTongJX2Member** ppMe)
{
	if (ppMe)
		*ppMe = NULL;
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return NULL;
	DWORD dwTongID = Player[nPlayerIdx].m_cTong.GetTongNameID();
	KTongJX2Tong* pTong = g_TongJX2.FindTong(dwTongID);
	if (!pTong)
		return NULL;
	if (ppMe)
	{
		DWORD dwMyID = g_FileName2Id(Player[nPlayerIdx].m_PlayerName);
		*ppMe = g_TongJX2.FindMember(pTong, dwMyID);
	}
	return pTong;
}

int KTongJX2Mgr::BuildClientView(int nPlayerIdx, int nPage, int nStart, void* pOut, int nOutSize)
{
	if (!pOut)
		return 0;
	KTongJX2Member* pMe = NULL;
	KTongJX2Tong* pTong = sJX2_PlayerTong(nPlayerIdx, &pMe);
	if (nPage == defTONG_JX2_PAGE_TONGLIST)
	{
		// danh sach bang: KHONG can thuoc bang
		if (nOutSize < (int)sizeof(TONG_JX2_TONGLIST_SYNC))
			return 0;
		TONG_JX2_TONGLIST_SYNC* pSync = (TONG_JX2_TONGLIST_SYNC*)pOut;
		memset(pSync, 0, sizeof(TONG_JX2_TONGLIST_SYNC));
		pSync->ProtocolType = s2c_extendtong;
		pSync->m_btMsgId = enumTONG_SYNC_ID_JX2;
		pSync->m_btPage = defTONG_JX2_PAGE_TONGLIST;
		pSync->m_wStart = (WORD)nStart;
		int nIdx = 0;
		std::map<DWORD, KTongJX2Tong>::iterator it;
		for (it = m_mapTong.begin(); it != m_mapTong.end(); ++it)
		{
			KTongJX2Tong* pT = &it->second;
			if (nIdx >= nStart && pSync->m_btCount < defTONG_JX2_LIST_ROWS)
			{
				TONG_JX2_ONE_TONG* pOne = &pSync->m_sTong[pSync->m_btCount];
				memcpy(pOne->m_szName, pT->szName, 32);
				pOne->m_szName[31] = 0;
				KTongJX2Member* pMaster = NULL;
				std::map<DWORD, KTongJX2Member>::iterator im;
				for (im = pT->mapMember.begin(); im != pT->mapMember.end(); ++im)
				{
					if (im->second.btFigure == 0)
					{
						pMaster = &im->second;
						break;
					}
				}
				if (pMaster)
				{
					memcpy(pOne->m_szMaster, pMaster->szName, 32);
					pOne->m_szMaster[31] = 0;
				}
				pOne->m_dwNameID = pT->dwNameID;
				pOne->m_btCamp = pT->btCamp;
				pOne->m_btLevel = (BYTE)GetField(pT->dwNameID, 13);
				pOne->m_wMember = (WORD)pT->mapMember.size();
				pSync->m_btCount++;
			}
			nIdx++;
		}
		pSync->m_wTotal = (WORD)nIdx;
		pSync->m_wLength = sizeof(TONG_JX2_TONGLIST_SYNC) - 1;
		return (int)sizeof(TONG_JX2_TONGLIST_SYNC);
	}
	if (nPage == defTONG_JX2_PAGE_OTHERZM)
	{
		// trang 2x2 xem chieu mo bang khac: KHONG can thuoc bang, 4 bang/goi
		if (nOutSize < (int)sizeof(TONG_JX2_OTHERZM_SYNC))
			return 0;
		TONG_JX2_OTHERZM_SYNC* pSync = (TONG_JX2_OTHERZM_SYNC*)pOut;
		memset(pSync, 0, sizeof(TONG_JX2_OTHERZM_SYNC));
		pSync->ProtocolType = s2c_extendtong;
		pSync->m_btMsgId = enumTONG_SYNC_ID_JX2;
		pSync->m_btPage = defTONG_JX2_PAGE_OTHERZM;
		pSync->m_wStart = (WORD)nStart;
		int nIdx = 0;
		std::map<DWORD, KTongJX2Tong>::iterator it;
		for (it = m_mapTong.begin(); it != m_mapTong.end(); ++it)
		{
			KTongJX2Tong* pT = &it->second;
			if (nIdx >= nStart && pSync->m_btCount < 4)
			{
				TONG_JX2_ONE_ZM* pOne = &pSync->m_sZM[pSync->m_btCount];
				memcpy(pOne->m_szName, pT->szName, 32);
				pOne->m_szName[31] = 0;
				KTongJX2Member* pMaster = NULL;
				std::map<DWORD, KTongJX2Member>::iterator im;
				for (im = pT->mapMember.begin(); im != pT->mapMember.end(); ++im)
				{
					if (im->second.btFigure == 0)
					{
						pMaster = &im->second;
						break;
					}
				}
				if (pMaster)
				{
					memcpy(pOne->m_szMaster, pMaster->szName, 32);
					pOne->m_szMaster[31] = 0;
				}
				pOne->m_dwNameID = pT->dwNameID;
				pOne->m_btCamp = pT->btCamp;
				pOne->m_btLevel = (BYTE)GetField(pT->dwNameID, 13);
				pOne->m_wMember = (WORD)pT->mapMember.size();
				pOne->m_btTendency = (BYTE)GetField(pT->dwNameID, 60);
				pOne->m_btAct[0] = (BYTE)GetField(pT->dwNameID, 61);
				pOne->m_btAct[1] = (BYTE)GetField(pT->dwNameID, 62);
				pOne->m_btAct[2] = (BYTE)GetField(pT->dwNameID, 63);
				pOne->m_btAct[3] = (BYTE)GetField(pT->dwNameID, 64);
				pOne->m_btAutoLv = (BYTE)GetField(pT->dwNameID, 65);
				pOne->m_btRefuseLv = (BYTE)GetField(pT->dwNameID, 66);
				memcpy(pOne->m_szRecruit, pT->szRecruit, sizeof(pOne->m_szRecruit));
				pOne->m_szRecruit[sizeof(pOne->m_szRecruit) - 1] = 0;
				pSync->m_btCount++;
			}
			nIdx++;
		}
		pSync->m_wTotal = (WORD)nIdx;
		pSync->m_wLength = sizeof(TONG_JX2_OTHERZM_SYNC) - 1;
		return (int)sizeof(TONG_JX2_OTHERZM_SYNC);
	}
	if (!pTong)
		return 0;

	switch (nPage)
	{
	case defTONG_JX2_PAGE_INFO:
		{
			if (nOutSize < (int)sizeof(TONG_JX2_INFO_SYNC))
				return 0;
			TONG_JX2_INFO_SYNC* pSync = (TONG_JX2_INFO_SYNC*)pOut;
			memset(pSync, 0, sizeof(TONG_JX2_INFO_SYNC));
			pSync->ProtocolType = s2c_extendtong;
			pSync->m_btMsgId = enumTONG_SYNC_ID_JX2;
			pSync->m_btPage = defTONG_JX2_PAGE_INFO;
			strncpy(pSync->m_szTongName, pTong->szName, 31);
			pSync->m_btCamp = pTong->btCamp;
			pSync->m_nLevel = (int)GetField(pTong->dwNameID, 13);
			pSync->m_nExp = (int)GetField(pTong->dwNameID, 6);
			pSync->m_nMoney = ((__int64)GetField(pTong->dwNameID, 4) << 32)
				| (__int64)GetField(pTong->dwNameID, 3);
			pSync->m_dwBuildFund = GetField(pTong->dwNameID, 12);
			pSync->m_dwWeekBuild = GetField(pTong->dwNameID, 41);
			pSync->m_dwWeekUpper = GetField(pTong->dwNameID, 42);
			pSync->m_dwWarFund = GetField(pTong->dwNameID, 15);
			pSync->m_dwMaintain = GetField(pTong->dwNameID, 16);
			pSync->m_dwPerStand = GetField(pTong->dwNameID, 17);
			pSync->m_dwStoredOffer = GetField(pTong->dwNameID, 18);
			pSync->m_dwStoredBuild = GetField(pTong->dwNameID, 19);
			pSync->m_nDay = (int)GetField(pTong->dwNameID, 20);
			pSync->m_nWeek = (int)GetField(pTong->dwNameID, 21);
			pSync->m_dwStuntID = GetField(pTong->dwNameID, defTONGTSK_STUNT_ID);
			pSync->m_dwStuntOn = GetField(pTong->dwNameID, defTONGTSK_STUNT_ENABLED);
			pSync->m_wMemberTotal = (WORD)pTong->mapMember.size();
			pSync->m_btMyFigure = pMe ? pMe->btFigure : 3;
			pSync->m_dwMyOffer = GetMemberField(pMe, 7);
			pSync->m_wMyRights = sJX2_RightMask(pMe);
			strncpy(pSync->m_szSelf, Player[nPlayerIdx].m_PlayerName, 31);
			pSync->m_dwMyWeekOffer = GetMemberField(pMe, 9);
			// "Dang cap" bang (khac "Dang cap kien thiet" = m_nLevel field 13)
			pSync->m_nTongLevel = Player[nPlayerIdx].m_cTong.GetTongLevel();
			pSync->m_dwUnionID = GetField(pTong->dwNameID, 10);
			strncpy(pSync->m_szUnionName, pTong->szUnionName, sizeof(pSync->m_szUnionName) - 1);
			pSync->m_bUnionLeader = GetField(pTong->dwNameID, 50) ? 1 : 0;
			memcpy(pSync->m_szAnnounce, pTong->szAnnounce, sizeof(pSync->m_szAnnounce));
			pSync->m_szAnnounce[127] = 0;
			// tim ten bang chu
			std::map<DWORD, KTongJX2Member>::iterator it;
			for (it = pTong->mapMember.begin(); it != pTong->mapMember.end(); ++it)
			{
				if (it->second.btFigure == 0)
				{
					strncpy(pSync->m_szMaster, it->second.szName, 31);
					break;
				}
			}
			pSync->m_wLength = sizeof(TONG_JX2_INFO_SYNC) - 1;
			return (int)sizeof(TONG_JX2_INFO_SYNC);
		}

	case defTONG_JX2_PAGE_MEMBER:
	case defTONG_JX2_PAGE_RIGHT:
		{
			if (nOutSize < (int)sizeof(TONG_JX2_MEMBER_SYNC))
				return 0;
			TONG_JX2_MEMBER_SYNC* pSync = (TONG_JX2_MEMBER_SYNC*)pOut;
			memset(pSync, 0, sizeof(TONG_JX2_MEMBER_SYNC));
			pSync->ProtocolType = s2c_extendtong;
			pSync->m_btMsgId = enumTONG_SYNC_ID_JX2;
			pSync->m_btPage = (BYTE)nPage;
			pSync->m_wStart = (WORD)nStart;

			int nIdx = 0;
			std::map<DWORD, KTongJX2Member>::iterator it;
			for (it = pTong->mapMember.begin(); it != pTong->mapMember.end(); ++it)
			{
				KTongJX2Member* pMember = &it->second;
				// trang RIGHT: chi liet ke bang chu + truong lao
				if (nPage == defTONG_JX2_PAGE_RIGHT && pMember->btFigure > 1)
					continue;
				if (nIdx < nStart)
				{
					nIdx++;
					continue;
				}
				if (pSync->m_btCount < defTONG_JX2_VIEW_MEMBERS)
				{
					TONG_JX2_ONE_MEMBER* pOne = &pSync->m_sMember[pSync->m_btCount];
					strncpy(pOne->m_szName, pMember->szName, 31);
					pOne->m_dwNameID = pMember->dwNameID;
					pOne->m_btFigure = pMember->btFigure;
					int nOnIdx = sFindPlayerIdxByNameID(pMember->dwNameID);
					pOne->m_btOnline = (nOnIdx > 0) ? 1 : 0;
					pOne->m_dwOffer = GetMemberField(pMember, 7);
					pOne->m_wRights = sJX2_RightMask(pMember);
					pOne->m_dwJoinTime = GetMemberField(pMember, 2);
					pOne->m_dwWeekOffer = GetMemberField(pMember, 9);
					pOne->m_dwLastActive = GetMemberField(pMember, 15);
					pOne->m_dwWeekGoal = GetMemberField(pMember, 10);
					if (nOnIdx > 0 && Player[nOnIdx].m_nIndex > 0)
					{
						// online: lay truc tiep + cap nhat cache de nguoi offline van xem duoc
						pOne->m_btLevel = (BYTE)Npc[Player[nOnIdx].m_nIndex].m_Level;
						pOne->m_btFaction = (BYTE)Player[nOnIdx].m_cFaction.m_nFirstAddFaction;
						if (GetMemberField(pMember, 1010) != (DWORD)pOne->m_btLevel)
							sSendMemberFieldCmd(pTong->dwNameID, pMember->dwNameID, 1010,
								(DWORD)pOne->m_btLevel, defTONG_JX2_OP_SET, 0);
						if (GetMemberField(pMember, 1011) != (DWORD)pOne->m_btFaction)
							sSendMemberFieldCmd(pTong->dwNameID, pMember->dwNameID, 1011,
								(DWORD)pOne->m_btFaction, defTONG_JX2_OP_SET, 0);
						DWORD dwNow = (DWORD)time(NULL);
						if (dwNow > pOne->m_dwLastActive + 600)
						{
							// online: cap nhat moc hoat dong gan day (toi da 10 phut/lan)
							sSendMemberFieldCmd(pTong->dwNameID, pMember->dwNameID, 15,
								dwNow, defTONG_JX2_OP_SET, 0);
							pOne->m_dwLastActive = dwNow;
						}
					}
					else
					{
						pOne->m_btLevel = (BYTE)GetMemberField(pMember, 1010);
						pOne->m_btFaction = (BYTE)GetMemberField(pMember, 1011);
					}
					// "Ngay gia nhap" (khoa 2) truoc day KHONG CHO NAO GHI nen
					// client luon hien dau gach, va con keo theo dong "Diem
					// cong hien trung binh hang ngay" sai (so ngay bi ep = 1).
					// Tu lanh: thay ai thieu thi dat mot lan, luu ben vung.
					if (pOne->m_dwJoinTime == 0)
					{
						DWORD dwJoin = (DWORD)time(NULL);
						sSendMemberFieldCmd(pTong->dwNameID, pMember->dwNameID, 2,
							dwJoin, defTONG_JX2_OP_SET, 0);
						pOne->m_dwJoinTime = dwJoin;
					}
					pSync->m_btCount++;
				}
				nIdx++;
			}
			pSync->m_wTotal = (WORD)nIdx;
			pSync->m_wLength = sizeof(TONG_JX2_MEMBER_SYNC) - 1;
			return (int)sizeof(TONG_JX2_MEMBER_SYNC);
		}

	case defTONG_JX2_PAGE_WS:
		{
			if (nOutSize < (int)sizeof(TONG_JX2_WS_SYNC))
				return 0;
			TONG_JX2_WS_SYNC* pSync = (TONG_JX2_WS_SYNC*)pOut;
			memset(pSync, 0, sizeof(TONG_JX2_WS_SYNC));
			pSync->ProtocolType = s2c_extendtong;
			pSync->m_btMsgId = enumTONG_SYNC_ID_JX2;
			pSync->m_btPage = defTONG_JX2_PAGE_WS;
			for (int t = 1; t <= defTONG_JX2_WS_MAX_TYPE; t++)
			{
				WORD wBase = (WORD)(defTONG_JX2_WS_ATTR_BASE + t * 10);
				pSync->m_sWs[t].btExist = GetField(pTong->dwNameID, wBase) ? 1 : 0;
				pSync->m_sWs[t].btOpen = GetField(pTong->dwNameID, (WORD)(wBase + 1)) ? 1 : 0;
				pSync->m_sWs[t].wLevel = (WORD)GetField(pTong->dwNameID, (WORD)(wBase + 2));
				pSync->m_sWs[t].dwOutput = GetField(pTong->dwNameID, (WORD)(wBase + 3));
				pSync->m_sWs[t].dwUseLevel = GetField(pTong->dwNameID, (WORD)(wBase + 4));
			}
			pSync->m_wLength = sizeof(TONG_JX2_WS_SYNC) - 1;
			return (int)sizeof(TONG_JX2_WS_SYNC);
		}

	case defTONG_JX2_PAGE_RECORD:
		{
			if (nOutSize < (int)sizeof(TONG_JX2_RECORD_SYNC))
				return 0;
			TONG_JX2_RECORD_SYNC* pSync = (TONG_JX2_RECORD_SYNC*)pOut;
			memset(pSync, 0, sizeof(TONG_JX2_RECORD_SYNC));
			pSync->ProtocolType = s2c_extendtong;
			pSync->m_btMsgId = enumTONG_SYNC_ID_JX2;
			pSync->m_btPage = defTONG_JX2_PAGE_RECORD;
			pSync->m_btKind = (nStart == 1) ? 1 : 0;
			memcpy(pSync->m_szAnnounce, pTong->szAnnounce, sizeof(pSync->m_szAnnounce));
			pSync->m_szAnnounce[sizeof(pSync->m_szAnnounce) - 1] = 0;
			{
				// doc ring tu MOI -> CU
				int nHead = pSync->m_btKind ? pTong->nHistoryHead : pTong->nEventHead;
				for (int r = 1; r <= 16 && pSync->m_btCount < defTONG_JX2_RECORD_LINES; r++)
				{
					const char* pszLine = pSync->m_btKind ?
						pTong->szHistory[(nHead - r + 32) % 16] :
						pTong->szEvent[(nHead - r + 32) % 16];
					if (!pszLine[0])
						continue;
					strncpy(pSync->m_szLine[pSync->m_btCount], pszLine, 95);
					pSync->m_btCount++;
				}
			}
			pSync->m_wLength = sizeof(TONG_JX2_RECORD_SYNC) - 1;
			return (int)sizeof(TONG_JX2_RECORD_SYNC);
		}

	case defTONG_JX2_PAGE_RECRUIT:
		{
			if (nOutSize < (int)sizeof(TONG_JX2_RECRUIT_SYNC))
				return 0;
			TONG_JX2_RECRUIT_SYNC* pSync = (TONG_JX2_RECRUIT_SYNC*)pOut;
			memset(pSync, 0, sizeof(TONG_JX2_RECRUIT_SYNC));
			pSync->ProtocolType = s2c_extendtong;
			pSync->m_btMsgId = enumTONG_SYNC_ID_JX2;
			pSync->m_btPage = defTONG_JX2_PAGE_RECRUIT;
			memcpy(pSync->m_szRecruit, pTong->szRecruit, sizeof(pSync->m_szRecruit));
			pSync->m_szRecruit[sizeof(pSync->m_szRecruit) - 1] = 0;
			pSync->m_btTendency = (BYTE)GetField(pTong->dwNameID, 60);
			pSync->m_btAct[0] = (BYTE)GetField(pTong->dwNameID, 61);
			pSync->m_btAct[1] = (BYTE)GetField(pTong->dwNameID, 62);
			pSync->m_btAct[2] = (BYTE)GetField(pTong->dwNameID, 63);
			pSync->m_btAct[3] = (BYTE)GetField(pTong->dwNameID, 64);
			pSync->m_btAutoLv = (BYTE)GetField(pTong->dwNameID, 65);
			pSync->m_btRefuseLv = (BYTE)GetField(pTong->dwNameID, 66);
			for (int a = 0; a < (int)pTong->btApplyCount && a < defTONG_JX2_APPLY_MAX; a++)
			{
				memcpy(pSync->m_sApply[a].m_szName, pTong->szApplyName[a], 32);
				pSync->m_sApply[a].m_szName[31] = 0;
				pSync->m_sApply[a].m_dwNameID = pTong->dwApplyID[a];
				pSync->m_sApply[a].m_wLevel = pTong->wApplyLevel[a];
				pSync->m_btApplyCount++;
			}
			pSync->m_wLength = sizeof(TONG_JX2_RECRUIT_SYNC) - 1;
			return (int)sizeof(TONG_JX2_RECRUIT_SYNC);
		}
	}
	return 0;
}

// co quyen (theo mat na 12 quyen) hoac la bang chu
// LUU Y: pMe == NULL nghia la KHONG TIM THAY ho so thanh vien trong ban sao,
// tuc la nguoi do CHUA CHAC thuoc bang -> phai tra FALSE. Truoc day tra TRUE
// ("coi nhu bang chu") khien bat ky ai vua vao bang, hoac ban sao chua kip dong
// bo, deu di qua het moi cong kiem quyen (rut ngan quy, duoi nguoi, doi chuc).
static BOOL sJX2_HasRight(KTongJX2Member* pMe, DWORD dwRightID)
{
	if (!pMe)
		return FALSE;
	if (pMe->btFigure == 0)
		return TRUE;
	return pMe->setRight.count(dwRightID) ? TRUE : FALSE;
}

// Bao cho moi nguoi dang online cua bang co quyen duyet don (bang chu figure 0
// hoac co quyen 1901 MEMBER_KICK/thu nhan) rang vua co don xin gia nhap.
static void sJX2_NotifyApply(KTongJX2Tong* pTong, const char* pszMsg)
{
	if (!pTong || !pszMsg || !pszMsg[0])
		return;
	int nLen = (int)strlen(pszMsg);
	int nIdx = PlayerSet.GetFirstPlayer();
	while (nIdx > 0)
	{
		if (Player[nIdx].m_nIndex > 0 &&
			Player[nIdx].m_cTong.GetTongNameID() == pTong->dwNameID)
		{
			KTongJX2Member* pM = g_TongJX2.FindMember(pTong,
				g_FileName2Id(Player[nIdx].m_PlayerName));
			if (pM && (pM->btFigure == 0 || sJX2_HasRight(pM, 1901)))
				KPlayerChat::SendSystemInfo(1, nIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
					(char*)pszMsg, nLen);
		}
		nIdx = PlayerSet.GetNextPlayer();
	}
}

// Msg2Tong cua ban goc: phat mot dong he thong toi MOI thanh vien online
// cua bang (tong_mix.lua goi VO DIEU KIEN truoc khi ghi so co nguong).
static void sJX2_Msg2Tong(KTongJX2Tong* pTong, const char* pszMsg)
{
	if (!pTong || !pszMsg || !pszMsg[0])
		return;
	int nLen = (int)strlen(pszMsg);
	int nIdx = PlayerSet.GetFirstPlayer();
	while (nIdx > 0)
	{
		if (Player[nIdx].m_nIndex > 0 &&
			Player[nIdx].m_cTong.GetTongNameID() == pTong->dwNameID)
			KPlayerChat::SendSystemInfo(1, nIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
				(char*)pszMsg, nLen);
		nIdx = PlayerSet.GetNextPlayer();
	}
}

// Ma tra ve 20 = "im lang": handler da tu SendSystemInfo cau tra loi rieng
// (vi du loi 3 ngay lien minh co %d giay con lai) - vo boc khong in gi them.
// Vo boc: goi than xu ly roi BAO KET QUA cho nguoi choi (he cu im lang lam
// nguoi choi tuong nut khong chay)
int KTongJX2Mgr::DoClientOp(int nPlayerIdx, const void* pData)
{
	int nRet = DoClientOpBody(nPlayerIdx, pData);
	if (nPlayerIdx > 0 && nPlayerIdx < MAX_PLAYER && pData)
	{
		const TONG_JX2OP_COMMAND* pCmd = (const TONG_JX2OP_COMMAND*)pData;
		const char* pszMsg = NULL;
		switch (nRet)
		{
		case 20:
			break;	// handler da tu nhan tin rieng - khong in gi them
		case 0:
			if (pCmd->m_btOp == defTONG_JX2_COP_APPLY_JOIN)
				pszMsg = "\247\267 g\366i \256\254n xin gia nh\313p, ch\352 bang h\351i duy\326t.";
			else
				pszMsg = "\247\267 th\371c hi\326n thao t\270c bang h\351i.";
			break;
		case 1:
			pszMsg = "G\343i tin bang h\351i kh\253ng h\356p l\326.";
			break;
		case 7:
			pszMsg = "\247\267 gia nh\313p bang h\351i!";
			break;
		case 2:
			pszMsg = "B\271n ch\255a v\265o bang h\351i.";
			break;
		case 3:
			// nguyen van ban Linux (MSG_TONG_NO_OPRIGHT)
			pszMsg = "Kh\253ng c\343 quy\322n h\271n \256\323 th\371c hi\326n thao t\270c n\265y!";
			break;
		case 4:
			pszMsg = "Kh\253ng t\327m th\312y m\364c ti\252u.";
			break;
		case 5:
			pszMsg = "Thao t\270c kh\253ng h\356p l\326 ho\306c m\364c ti\252u kh\253ng online.";
			break;
		case 6:
			pszMsg = "Kh\253ng \256\361 ti\322n ho\306c t\265i nguy\252n.";
			break;
		case 9:
			// nguyen van ban Linux (workshop_head.lua)
			pszMsg = "T\270c ph\255\352ng n\265y \256\267 c\343 r\345i!";
			break;
		case 10:
			pszMsg = "Qu\375 bang d\255\352ng nh\255 ch\255a x\251y d\371ng T\270c Ph\255\352ng n\265y.";
			break;
		case 11:
			// nguyen van ban Linux (MSG_TONG_AUTO_REFUSE_LEVEL)
			pszMsg = "\247\274ng c\312p c\361a ng\255\254i qu\270 th\312p, bang h\351i n\265y t\365 ch\350i \256\322 ngh\336 gia nh\313p c\361a ng\255\254i";
			break;
		case 12:
			pszMsg = "B\271n \256\267 g\366i \256\254n xin gia nh\313p bang h\351i n\265y r\345i, xin ch\352 duy\326t.";
			break;
		}

		if (pszMsg)
			KPlayerChat::SendSystemInfo(1, nPlayerIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
				(char*)pszMsg, strlen(pszMsg));
	}
	return (nRet == 7) ? 0 : nRet;
}

int KTongJX2Mgr::DoClientOpBody(int nPlayerIdx, const void* pData)
{
	if (!pData || nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return 1;
	const TONG_JX2OP_COMMAND* pCmd = (const TONG_JX2OP_COMMAND*)pData;
	if (pCmd->m_btOp == defTONG_JX2_COP_APPLY_JOIN)
		return sJX2_DoApplyJoin(nPlayerIdx, pCmd->m_dwTarget);	// khong can thuoc bang
	KTongJX2Member* pMe = NULL;
	KTongJX2Tong* pTong = sJX2_PlayerTong(nPlayerIdx, &pMe);
	if (!pTong)
		return 2;
	// Khong tim thay ho so thanh vien = ban sao chua dong bo xong. Truoc day
	// truong hop nay duoc coi la BANG CHU (pMe == NULL -> bMaster = TRUE) nen
	// nguoi vua vao bang co the rut sach ngan quy, duoi nguoi, doi chuc vu.
	if (!pMe)
		return 2;
	DWORD dwTongID = pTong->dwNameID;
	DWORD dwParam = (DWORD)nPlayerIdx;
	BOOL bMaster = (pMe->btFigure == 0);

	switch (pCmd->m_btOp)
	{
	case defTONG_JX2_COP_KICK:
		{
			if (!bMaster && !sJX2_HasRight(pMe, 1901))
				return 3;
			KTongJX2Member* pTarget = FindMember(pTong, pCmd->m_dwTarget);
			if (!pTarget || pTarget->btFigure <= 1)
				return 4;	// JX2: cam duoi bang chu / truong lao
			sSendTongOp(dwTongID, pCmd->m_dwTarget, defTONG_JX2_TOP_KICK, 0, 0, dwParam);
			return 0;
		}
	case defTONG_JX2_COP_ADDRIGHT:
	case defTONG_JX2_COP_DELRIGHT:
		{
			// JX2 4.4: nguoi thao tac = bang chu hoac co quyen 1002 (Bo nhiem -
			// mien nhiem, dung nghia ban goc; 1000 khong co trong blueprint)
			if (!bMaster && !sJX2_HasRight(pMe, 1002))
				return 3;
			KTongJX2Member* pTarget = FindMember(pTong, pCmd->m_dwTarget);
			if (!pTarget || pTarget->btFigure != 1)
				return 4;
			sSendRightCmd(dwTongID, pCmd->m_dwTarget, (DWORD)pCmd->m_nParam1,
				pCmd->m_btOp == defTONG_JX2_COP_ADDRIGHT ? 1 : 0, dwParam);
			return 0;
		}
	case defTONG_JX2_COP_SETANN:
		{
			if (!bMaster)
				return 3;
			char szText[128];
			memcpy(szText, pCmd->m_szText, sizeof(szText));
			szText[127] = 0;
			sSendStringCmd(dwTongID, defTONG_JX2_STR_ANNOUNCE, szText, dwParam);
			return 0;
		}
	case defTONG_JX2_COP_DONATE:
		{
			int nVan = pCmd->m_nParam1;
			if (nVan <= 0 || nVan > 10000)
				return 5;
			if (!Player[nPlayerIdx].Pay(nVan * 10000))
				return 6;	// khong du tien
			sSendFieldCmd(dwTongID, 12, (DWORD)nVan, defTONG_JX2_OP_ADDU, dwParam);
			sSendFieldCmd(dwTongID, 41, (DWORD)nVan, defTONG_JX2_OP_ADDU, dwParam);
			// KHONG cong ngan quy (field 3/4) o day: ban goc MONEY2BUILDFUND chi
			// cong quy kien thiet; ngan quy co duong nap rieng COP_DEPOSIT_MONEY.
			// Truoc day cong ca hai = mot lan tra tien duoc ghi vao HAI tui.
			if (pMe)
			{
				// khoa 7 = cong hien tich luy; khoa 9 = muc tieu tuan nay;
				// khoa 11 = cong hien tuan nay. Truoc day chi ghi khoa 7 nen o
				// "Cong hien tuan" tren giao dien VINH VIEN bang 0 (client doc
				// khoa 9 - xem BuildClientView m_dwMyWeekOffer / m_dwWeekOffer).
				sSendMemberFieldCmd(dwTongID, pMe->dwNameID, 7, (DWORD)nVan, defTONG_JX2_OP_ADD, dwParam);
				sSendMemberFieldCmd(dwTongID, pMe->dwNameID, 9, (DWORD)nVan, defTONG_JX2_OP_ADD, dwParam);
				sSendMemberFieldCmd(dwTongID, pMe->dwNameID, 11, (DWORD)nVan, defTONG_JX2_OP_ADD, dwParam);
			}
			{
				// nguyen van tong_mix.lua:242; Msg2Tong VO DIEU KIEN (:247),
				// so su kien chi ghi khi >= 100 van (:244 nOffer >= 1000000 luong)
				char szLog[160];
				sprintf(szLog, "%s \256\267 \256\343ng g\343p %d v\271n l\255\356ng v\265o ng\251n s\270ch ki\325n thi\325t bang", Player[nPlayerIdx].m_PlayerName, nVan);
				sJX2_Msg2Tong(pTong, szLog);
				if (nVan >= 100)
					sSendStringCmd(dwTongID, defTONG_JX2_STR_EVENT, szLog, dwParam);
			}
			return 0;
		}
	case defTONG_JX2_COP_WS_ADD:
	case defTONG_JX2_COP_WS_OPEN:
	case defTONG_JX2_COP_WS_CLOSE:
	case defTONG_JX2_COP_WS_UP:
		{
			// DOT9 #25/#26: 4 thao tac nay TRUOC DAY ghi thang field tren
			// duong GS -> khong tran cap, khong tru quy, khong dem so khu
			// (cap khu len VO HAN va MIEN PHI). Nay day sang RELAY - noi
			// giu quy that - de kiem 4 dieu kien ban goc roi moi ghi.
			if (!bMaster && !sJX2_HasRight(pMe, 9001))
				return 3;
			int nType = pCmd->m_nParam1;
			if (nType < 1 || nType > defTONG_JX2_WS_MAX_TYPE)
				return 5;
			WORD wBase = (WORD)(defTONG_JX2_WS_ATTR_BASE + nType * 10);
			BOOL bExist = GetField(dwTongID, wBase) != 0;
			if (pCmd->m_btOp == defTONG_JX2_COP_WS_ADD)
			{
				if (bExist)
					return 9;	// khu DA CO san
			}
			else if (!bExist)
				return 10;	// khu CHUA LAP
			int nAct = 0;	// 0 lap / 1 mo / 2 dong / 3 nang cap
			if (pCmd->m_btOp == defTONG_JX2_COP_WS_OPEN)
				nAct = 1;
			else if (pCmd->m_btOp == defTONG_JX2_COP_WS_CLOSE)
				nAct = 2;
			else if (pCmd->m_btOp == defTONG_JX2_COP_WS_UP)
				nAct = 3;
			sSendTongOp(dwTongID, 0, defTONG_JX2_TOP_WS_OP, nType, nAct, dwParam);
			return 20;	// relay bao ket qua tren kenh chat bang
		}
	case defTONG_JX2_COP_SETSTUNT:
		{
			if (!bMaster && !sJX2_HasRight(pMe, 2006))
				return 3;
			sSendTongOp(dwTongID, 0, defTONG_JX2_TOP_SET_STUNT, pCmd->m_nParam1, 0, dwParam);
			return 0;
		}
	case defTONG_JX2_COP_UPGRADE:
		{
			if (!bMaster && !sJX2_HasRight(pMe, 2001))
				return 3;
			sSendTongOp(dwTongID, 0, defTONG_JX2_TOP_UPGRADE, 0, 0, dwParam);
			return 0;
		}
	case defTONG_JX2_COP_DEGRADE:
		{
			if (!bMaster)
				return 3;
			sSendTongOp(dwTongID, 0, defTONG_JX2_TOP_DEGRADE, 0, 0, dwParam);
			return 0;
		}
	case defTONG_JX2_COP_GRANT:
		{
			if (!bMaster)
				return 3;
			if (pCmd->m_nParam1 <= 0)
				return 5;
			sSendTongOp(dwTongID, pCmd->m_dwTarget, defTONG_JX2_TOP_DIST_MEMBER,
				pCmd->m_nParam1, 0, dwParam);
			return 0;
		}
	case defTONG_JX2_COP_SET_FIGURE:
		{
			// JX2 4.3: quyen quan ly 1101 (hoac bang chu); khong doi duoc bang chu
			if (!bMaster && !sJX2_HasRight(pMe, 1101))
				return 3;
			KTongJX2Member* pTarget = FindMember(pTong, pCmd->m_dwTarget);
			if (!pTarget || pTarget->btFigure == 0)
				return 4;
			if (pCmd->m_nParam1 < 1 || pCmd->m_nParam1 > 3)
				return 5;
			sSendTongOp(dwTongID, pCmd->m_dwTarget, defTONG_JX2_TOP_SET_FIGURE,
				pCmd->m_nParam1, 0, dwParam);
			return 0;
		}
	case defTONG_JX2_COP_SAVE_RECRUIT:
		{
			// quyen thu nhan 1901 hoac bang chu (JX2)
			if (!bMaster && !sJX2_HasRight(pMe, 1901))
				return 3;
			char szText[128];
			memcpy(szText, pCmd->m_szText, sizeof(szText));
			szText[127] = 0;
			sSendStringCmd(dwTongID, defTONG_JX2_STR_RECRUIT, szText, dwParam);
			sSendFieldCmd(dwTongID, 60, (DWORD)(pCmd->m_nParam1 & 15), defTONG_JX2_OP_SET, dwParam);
			sSendFieldCmd(dwTongID, 61, (DWORD)((pCmd->m_nParam1 >> 4) & 15), defTONG_JX2_OP_SET, dwParam);
			sSendFieldCmd(dwTongID, 62, (DWORD)((pCmd->m_nParam1 >> 8) & 15), defTONG_JX2_OP_SET, dwParam);
			sSendFieldCmd(dwTongID, 63, (DWORD)((pCmd->m_nParam1 >> 12) & 15), defTONG_JX2_OP_SET, dwParam);
			sSendFieldCmd(dwTongID, 64, (DWORD)((pCmd->m_nParam1 >> 16) & 15), defTONG_JX2_OP_SET, dwParam);
			sSendFieldCmd(dwTongID, 65, (DWORD)(pCmd->m_nParam2 & 255), defTONG_JX2_OP_SET, dwParam);
			sSendFieldCmd(dwTongID, 66, (DWORD)((pCmd->m_nParam2 >> 8) & 255), defTONG_JX2_OP_SET, dwParam);
			return 0;
		}
	case defTONG_JX2_COP_ACCEPT_APPLY:
		{
			if (!bMaster && !sJX2_HasRight(pMe, 1901))
				return 3;
			int a;
			for (a = 0; a < (int)pTong->btApplyCount; a++)
			{
				if (pTong->dwApplyID[a] == pCmd->m_dwTarget)
					break;
			}
			if (a >= (int)pTong->btApplyCount)
				return 4;
			// Duyet duoc CA KHI nguoi xin da thoat game: relay them thanh vien
			// theo TEN chu khong theo chi so nguoi choi. Truoc day bat buoc ho
			// phai online cung GameServer, nen don thuong ket lai vinh vien.
			char szJoin[32];
			memcpy(szJoin, pTong->szApplyName[a], 32);
			szJoin[31] = 0;
			DWORD dwJoinID = pTong->dwApplyID[a];
			BYTE btJoinSex = pTong->btApplySex[a];
			int nJoinIdx = sFindPlayerIdxByNameID(dwJoinID);
			if (nJoinIdx > 0 && Player[nJoinIdx].m_nIndex > 0 &&
				Player[nJoinIdx].m_cTong.m_nFlag)
				return 5;	// dang online va DA co bang khac
			sJX2_SendAddMemberByName(pTong, szJoin, dwJoinID, btJoinSex,
				(DWORD)(nJoinIdx > 0 ? nJoinIdx : 0));
			// ghi luon "Ngay gia nhap" (khoa 2) cho nguoi vua duoc duyet
			sSendMemberFieldCmd(dwTongID, dwJoinID, 2, (DWORD)time(NULL),
				defTONG_JX2_OP_SET, dwParam);
			// bao cho CHINH NGUOI VUA DUOC NHAN (truoc day chi nhanh tu choi
			// moi bao, nguoi duoc nhan khong he biet minh da vao bang)
			if (nJoinIdx > 0 && Player[nJoinIdx].m_nIndex > 0)
			{
				const char* pszOK = "B\271n gia nh\313p bang h\351i!";
				KPlayerChat::SendSystemInfo(1, nJoinIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
					(char*)pszOK, strlen(pszOK));
			}
			for (; a < (int)pTong->btApplyCount - 1; a++)
			{
				memcpy(pTong->szApplyName[a], pTong->szApplyName[a + 1], 32);
				pTong->dwApplyID[a] = pTong->dwApplyID[a + 1];
				pTong->wApplyLevel[a] = pTong->wApplyLevel[a + 1];
				pTong->btApplySex[a] = pTong->btApplySex[a + 1];
			}
			pTong->btApplyCount--;
			char szLog[160];
			sprintf(szLog, "%s duy\326t %s v\265o bang",
				Player[nPlayerIdx].m_PlayerName, szJoin);
			sSendStringCmd(dwTongID, defTONG_JX2_STR_EVENT, szLog, dwParam);
			return 0;
		}
	case defTONG_JX2_COP_REFUSE_APPLY:
		{
			if (!bMaster && !sJX2_HasRight(pMe, 1901))
				return 3;
			int a;
			for (a = 0; a < (int)pTong->btApplyCount; a++)
			{
				if (pTong->dwApplyID[a] == pCmd->m_dwTarget)
					break;
			}
			if (a >= (int)pTong->btApplyCount)
				return 4;
			{
				// bao cho nguoi xin biet don bi tu choi (neu ho dang online)
				int nRefIdx = sFindPlayerIdxByNameID(pCmd->m_dwTarget);
				if (nRefIdx > 0 && Player[nRefIdx].m_nIndex > 0)
				{
					char szRef[160];
					sprintf(szRef, "Bang h\351i %.31s \256\267 t\365 ch\350i \256\254n xin gia nh\313p c\361a b\271n.",
						pTong->szName);
					KPlayerChat::SendSystemInfo(1, nRefIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
						szRef, strlen(szRef));
				}
			}
			for (; a < (int)pTong->btApplyCount - 1; a++)
			{
				memcpy(pTong->szApplyName[a], pTong->szApplyName[a + 1], 32);
				pTong->dwApplyID[a] = pTong->dwApplyID[a + 1];
				pTong->wApplyLevel[a] = pTong->wApplyLevel[a + 1];
				pTong->btApplySex[a] = pTong->btApplySex[a + 1];
			}
			pTong->btApplyCount--;
			return 0;
		}
	case defTONG_JX2_COP_LEAVE_WORD:
		{
			// moi thanh vien duoc luu loi vao so su kien
			char szWord[128];
			memcpy(szWord, pCmd->m_szText, sizeof(szWord));
			szWord[127] = 0;
			if (!szWord[0])
				return 5;
			char szLine[160];
			sprintf(szLine, "%s: %.90s", Player[nPlayerIdx].m_PlayerName, szWord);
			sSendStringCmd(dwTongID, defTONG_JX2_STR_EVENT, szLine, dwParam);
			return 0;
		}
	case defTONG_JX2_COP_CHANGE_CAMP:
		{
			// DOI PHE kieu JX2. KHONG dung duong JX1 (ApplyTongChangeCamp) vi ca
			// BA cong kiem tien cua no deu doc tui tien JX1 m_dwMoney - tui do
			// TACH RIENG khoi ngan quy JX2 (field 3/4) va luon bang 0, nen bang
			// dang co rat nhieu tien van bi bao "khong du tien".
			if (!bMaster && !sJX2_HasRight(pMe, 1003))
				return 3;
			int nCamp = pCmd->m_nParam1;
			if (nCamp < 1 || nCamp > 3)
				return 5;
			if (GetField(dwTongID, 10) != 0)
			{
				// tong.lua:823/:851 - dang trong lien minh thi CAM doi phe
				// (G_PLAYERTONG_5)
				KPlayerChat::SendSystemInfo(1, nPlayerIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
					(char*)"Kh\253ng th\323 thay \256\346i phe ph\270i trong li\252n minh c\361a bang h\351i", (int)strlen("Kh\253ng th\323 thay \256\346i phe ph\270i trong li\252n minh c\361a bang h\351i"));
				return 20;
			}
			if ((int)pTong->btCamp == nCamp)
				return 5;	// dang o phe do roi
			__int64 nGia = (__int64)PlayerSet.m_sTongParam.m_nMoneyChangeCamp;
			__int64 nCo = (__int64)GetField(dwTongID, 3) |
				((__int64)GetField(dwTongID, 4) << 32);
			if (nCo < nGia)
				return 6;	// ngan quy bang khong du
			sSendMoneyCmd(dwTongID, -nGia, defTONG_JX2_OP_ADD, dwParam);
			{
				// tru ngay tren ban sao de bam lien tuc khong qua duoc phep kiem
				__int64 nLai = nCo - nGia;
				pTong->mapField[3] = (DWORD)(nLai & 0xFFFFFFFF);
				pTong->mapField[4] = (DWORD)((nLai >> 32) & 0xFFFFFFFF);
			}
			sSendTongOp(dwTongID, 0, defTONG_JX2_TOP_SET_CAMP, nCamp, 0, dwParam);
			{
				static const char* szC[4] = {"", "Ch\335nh ph\270i", "T\265 ph\270i", "Trung l\313p"};
				char szLog[160];
				sprintf(szLog, "%s \256\346i phe bang sang %s",
					Player[nPlayerIdx].m_PlayerName, szC[nCamp]);
				sSendStringCmd(dwTongID, defTONG_JX2_STR_EVENT, szLog, dwParam);
			}
			return 0;
		}
	case defTONG_JX2_COP_PAY_MEMBER:
	case defTONG_JX2_COP_DRAW_MONEY:
		{
			// phat tien tu ngan quy: quyen 3001 hoac bang chu; rut: chi bang chu
			if (pCmd->m_btOp == defTONG_JX2_COP_DRAW_MONEY && !bMaster)
				return 3;
			if (!bMaster && !sJX2_HasRight(pMe, 3001))
				return 3;
			int nVan = pCmd->m_nParam1;
			if (nVan <= 0 || nVan > 10000)
				return 5;
			__int64 nMoney = (__int64)GetField(dwTongID, 3) |
				((__int64)GetField(dwTongID, 4) << 32);
			if (nMoney < (__int64)nVan * 10000)
				return 6;	// ngan quy khong du
			int nTargetIdx;
			DWORD dwTargetID;
			if (pCmd->m_btOp == defTONG_JX2_COP_DRAW_MONEY)
			{
				nTargetIdx = nPlayerIdx;
				dwTargetID = g_FileName2Id(Player[nPlayerIdx].m_PlayerName);
			}
			else
			{
				dwTargetID = pCmd->m_dwTarget;
				if (!FindMember(pTong, dwTargetID))
					return 4;
				nTargetIdx = sFindPlayerIdxByNameID(dwTargetID);
				if (nTargetIdx <= 0)
					return 5;	// nguoi nhan phai online
			}
			sSendMoneyCmd(dwTongID, -(__int64)nVan * 10000, defTONG_JX2_OP_ADD, dwParam);
			{
				// TRU NGAY tren ban sao cuc bo. Lenh tren chi la yeu cau gui len
				// relay; ban sao chi doi khi relay phat echo ve (den 750 giay).
				// Neu khong tru truoc thi bam lien tuc se qua duoc phep kiem
				// "ngan quy khong du" nhieu lan = NHAN DOI TIEN.
				__int64 nLeft = nMoney - (__int64)nVan * 10000;
				if (nLeft < 0)
					nLeft = 0;
				pTong->mapField[3] = (DWORD)(nLeft & 0xFFFFFFFF);
				pTong->mapField[4] = (DWORD)((nLeft >> 32) & 0xFFFFFFFF);
			}
			Player[nTargetIdx].Earn(nVan * 10000);
			// Nguyen van ban Linux, don vi LUONG (khong phai van): rut =
			// tong_mix.lua:77 (Msg2Tong :79 VO DIEU KIEN, ghi so :80 khi
			// abs >= 1000000 luong = 100 van). "Phat tien" khong co ham goc -
			// mo phong mau :377, gate mau la > 100 (strict).
			{
				char szLog[160];
				if (pCmd->m_btOp == defTONG_JX2_COP_DRAW_MONEY)
					sprintf(szLog, "%s \256\267 r\363t t\365 ng\251n qu\374 bang h\351i %.0f l\255\356ng",
						Player[nPlayerIdx].m_PlayerName, (double)nVan * 10000);
				else
					sprintf(szLog, "%s ph\270t cho %s %.0f l\255\356ng",
						Player[nPlayerIdx].m_PlayerName, Player[nTargetIdx].m_PlayerName, (double)nVan * 10000);
				sJX2_Msg2Tong(pTong, szLog);
				if (nVan >= 100)	// ca rut lan phat: nguong 100 van (khop prefill hop nhap)
					sSendStringCmd(dwTongID, defTONG_JX2_STR_EVENT, szLog, dwParam);
			}
			return 0;
		}
	case defTONG_JX2_COP_DEPOSIT_MONEY:
		{
			// Nap tien ca nhan vao ngan quy bang = MONEYFUND_ADD ban goc
			// (tong_mix.lua:96-108): moi thanh vien duoc nap, tran ngan quy
			// nMax = 2000000000 luong (:101), ghi so khi >= 100 van (:80).
			int nVan = pCmd->m_nParam1;
			if (nVan <= 0 || nVan > 200000)
				return 5;
			// tong_mix.lua:101-105 chi co tran MOI LAN nap (2e9 luong =
			// 200000 van, da chan bang nVan o tren) - KHONG co tran tong quy.
			__int64 nCo = (__int64)GetField(dwTongID, 3) |
				((__int64)GetField(dwTongID, 4) << 32);
			if (!Player[nPlayerIdx].Pay(nVan * 10000))
				return 6;	// khong du tien
			sSendMoneyCmd(dwTongID, (__int64)nVan * 10000, defTONG_JX2_OP_ADD, dwParam);
			{
				// cong ngay tren ban sao de o Ngan quy tren giao dien cap nhat
				__int64 nMoi = nCo + (__int64)nVan * 10000;
				pTong->mapField[3] = (DWORD)(nMoi & 0xFFFFFFFF);
				pTong->mapField[4] = (DWORD)((nMoi >> 32) & 0xFFFFFFFF);
			}
			{
				char szLog[160];
				sprintf(szLog, "%s \256\267 \256\343ng g\343p %.0f l\255\356ng v\265o ng\251n qu\374 bang h\351i",
					Player[nPlayerIdx].m_PlayerName, (double)nVan * 10000);
				sJX2_Msg2Tong(pTong, szLog);
				if (nVan >= 100)
					sSendStringCmd(dwTongID, defTONG_JX2_STR_EVENT, szLog, dwParam);
			}
			return 0;
		}
	case defTONG_JX2_COP_UNION_CREATE:
		{
			// chi bang chu; chua o lien minh; ten <= 31; MIEN PHI (ban goc)
			if (!bMaster)
				return 3;
			if (GetField(dwTongID, 10) != 0)
			{
				// MSG_TONG_CREATE_UNION_ERROR2
				KPlayerChat::SendSystemInfo(1, nPlayerIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
					(char*)"\247\267 \353 trong li\252n minh, kh\253ng th\323 t\270i l\313p li\252n minh! ", (int)strlen("\247\267 \353 trong li\252n minh, kh\253ng th\323 t\270i l\313p li\252n minh! "));
				return 20;
			}
			if (!pCmd->m_szText[0])
				return 5;
			sSendTongOp(dwTongID, 0, defTONG_JX2_TOP_UNION_CREATE, 0, 0, dwParam, pCmd->m_szText);
			return 20;	// relay bao ket qua tren kenh bang
		}
	case defTONG_JX2_COP_UNION_APPLY:
		{
			// bang chu xin cho BANG MINH vao lien minh cua bang szText
			if (!bMaster)
			{
				// G_PLAYERTONG_11
				KPlayerChat::SendSystemInfo(1, nPlayerIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
					(char*)"Ch\330 c\343 Bang ch\361 m\355i c\343 th\323 m\352i gia nh\313p li\252n minh bang h\351i", (int)strlen("Ch\330 c\343 Bang ch\361 m\355i c\343 th\323 m\352i gia nh\313p li\252n minh bang h\351i"));
				return 20;
			}
			if (GetField(dwTongID, 10) != 0)
			{
				// G_PLAYERTONG_12
				KPlayerChat::SendSystemInfo(1, nPlayerIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
					(char*)"Th\265nh vi\252n c\361a li\252n minh bang h\351i n\265y kh\351ng th\323 gia nh\313p li\252n minh bang h\351i kh\270c", (int)strlen("Th\265nh vi\252n c\361a li\252n minh bang h\351i n\265y kh\351ng th\323 gia nh\313p li\252n minh bang h\351i kh\270c"));
				return 20;
			}
			{
				// luat 3 ngay (field 49, ban goc gac o GS: 259200 giay)
				DWORD dwT = GetField(dwTongID, 49);
				if (dwT && time(NULL) - (time_t)dwT <= 259199)
				{
					char szMsg[160];
					sprintf(szMsg, "Sau khi r\352i li\252n minh 3 ng\265y m\355i c\343 th\323 gia nh\313p li\252n minh m\355i, c\307n \256\356i %d gi\251y.",
						(int)(259200 - (time(NULL) - (time_t)dwT)));
					KPlayerChat::SendSystemInfo(1, nPlayerIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
						szMsg, (int)strlen(szMsg));
					return 20;
				}
			}
			KTongJX2Tong* pDest = FindTong(g_FileName2Id((char*)pCmd->m_szText));
			if (!pDest)
				return 4;
			DWORD dwUid = GetField(pDest->dwNameID, 10);
			if (!dwUid)
			{
				// MSG_TONG_JOIN_UNION_ERROR1
				KPlayerChat::SendSystemInfo(1, nPlayerIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
					(char*)"Li\252n minh xin gia nh\313p kh\253ng t\345n t\271i ho\306c \256\267 b\336 gi\266i t\270n!", (int)strlen("Li\252n minh xin gia nh\313p kh\253ng t\345n t\271i ho\306c \256\267 b\336 gi\266i t\270n!"));
				return 20;
			}
			{
				// bao bang chu cua BANG MINH CHU lien minh (online): don xin vao
				char szMsg[160];
				sprintf(szMsg, "Bang h\351i %s xin gia nh\313p li\252n minh!", pTong->szName);
				std::map<DWORD, KTongJX2Tong>::iterator itU;
				for (itU = m_mapTong.begin(); itU != m_mapTong.end(); ++itU)
				{
					if (GetField(itU->second.dwNameID, 10) != dwUid ||
						!GetField(itU->second.dwNameID, 50))
						continue;
					sJX2_NotifyApply(&itU->second, szMsg);
					break;
				}
				// G_PLAYERTONG_13 cho nguoi xin
				sprintf(szMsg, " B\271n \256\325n g\306p %s xin ph\320p gia nh\313p li\252n minh bang h\351i! ", pCmd->m_szText);
				KPlayerChat::SendSystemInfo(1, nPlayerIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
					szMsg, (int)strlen(szMsg));
			}
			// ghi don len relay (field 54) - minh chu chi duyet duoc bang DA xin
			sSendTongOp(dwTongID, 0, defTONG_JX2_TOP_UNION_APPLY, (int)dwUid, 0, dwParam);
			return 20;
		}
	case defTONG_JX2_COP_UNION_ACCEPT:
		{
			// minh chu duyet bang szText vao lien minh (relay kiem + tru tien)
			if (!bMaster)
				return 3;
			if (!GetField(dwTongID, 10) || !GetField(dwTongID, 50))
				return 3;
			if (!pCmd->m_szText[0])
				return 5;
			sSendTongOp(dwTongID, 0, defTONG_JX2_TOP_UNION_JOIN, 0, 0, dwParam, pCmd->m_szText);
			return 20;
		}
	case defTONG_JX2_COP_UNION_LEAVE:
		{
			if (!bMaster)
				return 3;
			if (!GetField(dwTongID, 10))
				return 5;
			sSendTongOp(dwTongID, 0, defTONG_JX2_TOP_UNION_LEAVE, 0, 0, dwParam);
			return 20;
		}
	case defTONG_JX2_COP_UNION_KICK:
		{
			if (!bMaster)
				return 3;
			if (!GetField(dwTongID, 10) || !GetField(dwTongID, 50))
				return 3;
			if (!pCmd->m_szText[0])
				return 5;
			sSendTongOp(dwTongID, 0, defTONG_JX2_TOP_UNION_KICK, 0, 0, dwParam, pCmd->m_szText);
			return 20;
		}
	case defTONG_JX2_COP_MINISTER_SET:
	case defTONG_JX2_COP_MINISTER_FIRE:
		{
			// dai than quoc gia (trang con 4): chi bang chu ("quoc chu" ban goc
			// = bang chu bang chiem thanh - he cong thanh chua co nen gac bang
			// chu). field 51/52/53 = NameID; hieu ung skill NW chua port.
			if (!bMaster)
			{
				// MSG_NW_INSTATE_NOTKING
				KPlayerChat::SendSystemInfo(1, nPlayerIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
					(char*)"Ch\330 c\343 qu\350c v\255\254ng m\355i th\371c hi\326n \256\255\356c thao t\270c n\265y!", (int)strlen("Ch\330 c\343 qu\350c v\255\254ng m\355i th\371c hi\326n \256\255\356c thao t\270c n\265y!"));
				return 20;
			}
			int nSlot = pCmd->m_nParam1;
			if (nSlot < 1 || nSlot > 3)
				return 5;
			BOOL bSet = (pCmd->m_btOp == defTONG_JX2_COP_MINISTER_SET);
			if (bSet && !FindMember(pTong, pCmd->m_dwTarget))
				return 4;
			sSendTongOp(dwTongID, bSet ? pCmd->m_dwTarget : 0, defTONG_JX2_TOP_MINISTER,
				nSlot, bSet ? 1 : 0, dwParam);
			{
				// ghi so su kien voi ten chuc nguyen van (G_NWTITLE_*)
				static const char* szNW[4] = {"?", "Th\365a T\255\355ng", "Nguy\252n So\270i", "Ti\252n Phong"};
				char szLog[160];
				if (bSet)
				{
					KTongJX2Member* pT2 = FindMember(pTong, pCmd->m_dwTarget);
					sprintf(szLog, "%s \256\255\356c phong l\265m %s", pT2 ? pT2->szName : "?", szNW[nSlot]);
				}
				else
					sprintf(szLog, "C\270ch ch\370c %s", szNW[nSlot]);
				sJX2_Msg2Tong(pTong, szLog);
				sSendStringCmd(dwTongID, defTONG_JX2_STR_EVENT, szLog, dwParam);
			}
			return 0;
		}
	case defTONG_JX2_COP_WS_SETLV:
		{
			if (!bMaster && !sJX2_HasRight(pMe, 9001))
				return 3;
			int nType = pCmd->m_nParam1;
			if (nType < 1 || nType > defTONG_JX2_WS_MAX_TYPE)
				return 5;
			sSendFieldCmd(dwTongID, sWsAttrField(nType, 4), (DWORD)pCmd->m_nParam2,
				defTONG_JX2_OP_SET, dwParam);
			return 0;
		}
	case defTONG_JX2_COP_WS_DEL:
		{
			if (!bMaster && !sJX2_HasRight(pMe, 9001))
				return 3;
			int nType = pCmd->m_nParam1;
			if (nType < 1 || nType > defTONG_JX2_WS_MAX_TYPE)
				return 5;
			for (int nAttr = 0; nAttr <= 5; nAttr++)
				sSendFieldCmd(dwTongID, sWsAttrField(nType, nAttr), 0,
					defTONG_JX2_OP_SET, dwParam);	// SET 0 = xoa khoa
			return 0;
		}
	case defTONG_JX2_COP_MAP_SET:
	case defTONG_JX2_COP_MAP_CREATE:
	case defTONG_JX2_COP_MAP_DELETE:
		{
			// quyen lanh dia 2004 hoac bang chu
			if (!bMaster && !sJX2_HasRight(pMe, 2004))
				return 3;
			BYTE btOp = defTONG_JX2_TOP_SET_MAP;
			if (pCmd->m_btOp == defTONG_JX2_COP_MAP_CREATE)
				btOp = defTONG_JX2_TOP_CREATE_MAP;
			else if (pCmd->m_btOp == defTONG_JX2_COP_MAP_DELETE)
				btOp = defTONG_JX2_TOP_DELETE_MAP;
			sSendTongOp(dwTongID, 0, btOp, pCmd->m_nParam1, pCmd->m_nParam2, dwParam);
			return 0;
		}
	case defTONG_JX2_COP_STORE_OFFER:
		{
			// cat cong hien ca nhan (vi SaveVal 2801) vao quy du tru bang (field 18)
			int nVal = pCmd->m_nParam1;
			if (nVal <= 0 || nVal > 100000)
				return 5;
			int nCur = (int)Player[nPlayerIdx].m_cTask.GetSaveVal(defTASK_JX2_CONTRIBUTION);
			if (nCur < nVal)
				return 6;	// khong du cong hien
			Player[nPlayerIdx].m_cTask.SetSaveVal(defTASK_JX2_CONTRIBUTION, (DWORD)(nCur - nVal));
			sSendTongOp(dwTongID, 0, defTONG_JX2_TOP_CONTRIBUTE, nVal, 0, dwParam);
			{
				// tong_mix.lua:297 (Msg2Tong :301 vo dieu kien, ghi so :298
				// khi nOffer >= 100)
				char szLog[160];
				sprintf(szLog, "%s \256\267 \256\343ng g\343p %d \256i\323m c\350ng hi\325n v\265o ng\251n s\270ch c\350ng hi\325n bang", Player[nPlayerIdx].m_PlayerName, nVal);
				sJX2_Msg2Tong(pTong, szLog);
				if (nVal >= 100)
					sSendStringCmd(dwTongID, defTONG_JX2_STR_EVENT, szLog, dwParam);
			}
			return 0;
		}
	}
	return 1;
}

// TONG_ApplyJoin(nTongID) - nguoi choi xin vao bang: du cap tu-nhan thi vao thang,
// khong thi vao danh sach cho duyet cua trang chieu mo. Tra: 2 vao thang / 1 da ghi
// don / 0 that bai / -1 bi tu choi vi duoi cap.
int LuaTONG_ApplyJoin(Lua_State* L)
{
	// Dung CHUNG than xu ly voi duong cua so client (sJX2_DoApplyJoin) de hai
	// duong khong bao gio lech nhau nua - truoc day day la ban chep tay thu hai
	// va da bi bo sot khi sua duong kia (ghi gioi tinh, bao bang chu, so su kien).
	DWORD dwTongID = sArgTongID(L);
	int nPlayerIndex = GetPlayerIndex(L);
	int nRet = sJX2_DoApplyJoin(nPlayerIndex, dwTongID);
	// doi ma tra ve theo giao uoc cu cua script:
	//  2 = vao thang / 1 = da vao danh sach cho / -1 = duoi cap bi tu choi / 0 = that bai
	if (nRet == 7)
		Lua_PushNumber(L, 2);
	else if (nRet == 11)
		Lua_PushNumber(L, -1);		// duoi nguong cap (truoc la ma 6)
	else if (nRet == 0 || nRet == 12)
		Lua_PushNumber(L, 1);		// 12 = da nop don truoc do
	else
		Lua_PushNumber(L, 0);
	return 1;
}

// TONG_GetApplyCount(nTongID) -> so don dang cho duyet
int LuaTONG_GetApplyCount(Lua_State* L)
{
	KTongJX2Tong* pTong = g_TongJX2.FindTong(sArgTongID(L));
	Lua_PushNumber(L, pTong ? (double)pTong->btApplyCount : 0);
	return 1;
}

// su dung tac phuong: cong 1 vao san luong ngay
int LuaTWS_ApplyUse(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	int nType = sWsType(L);
	if (!g_TongJX2.FindTong(dwTongID) || !nType ||
		!g_TongJX2.GetField(dwTongID, sWsAttrField(nType, 0)))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendFieldCmd(dwTongID, sWsAttrField(nType, 3), 1, defTONG_JX2_OP_ADDU, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

//////////////////////////////////////////////////////////////////////
// DOT 3 - ham engine ma script bang hoi JX2 goi nhung JX1 chua co
// (do tu 733 ham duoc goi trong script\tong Linux - xem jx2_tong_calls)
//////////////////////////////////////////////////////////////////////

// Vi cong hien nguoi choi (JX2 6.1 - tang NHAN VAT, tieu duoc), luu bang
// bien nhiem vu ben (SaveVal) de song qua relog:
// (defTASK_JX2_* don len truoc DoClientOp - xem tren)

static int sPushSaveVal(Lua_State* L, int nTaskID)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, (double)(int)Player[nPlayerIndex].m_cTask.GetSaveVal(nTaskID));
	return 1;
}

// cong delta (am duoc, khong cho xuong duoi 0 -> tra 0 neu khong du)
static int sAddSaveVal(Lua_State* L, int nTaskID)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || !Lua_IsNumber(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nDelta = (int)Lua_ValueToNumber(L, 1);
	int nCur = (int)Player[nPlayerIndex].m_cTask.GetSaveVal(nTaskID);
	if (nDelta < 0 && nCur + nDelta < 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Player[nPlayerIndex].m_cTask.SetSaveVal(nTaskID, (DWORD)(nCur + nDelta));
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaJX2_GetContribution(Lua_State* L)	{ return sPushSaveVal(L, defTASK_JX2_CONTRIBUTION); }
int LuaJX2_AddContribution(Lua_State* L)	{ return sAddSaveVal(L, defTASK_JX2_CONTRIBUTION); }
int LuaJX2_GetWeeklyOffer(Lua_State* L)		{ return sPushSaveVal(L, defTASK_JX2_WEEKLYOFFER); }
int LuaJX2_AddWeeklyOffer(Lua_State* L)		{ return sAddSaveVal(L, defTASK_JX2_WEEKLYOFFER); }
int LuaJX2_GetCumulateOffer(Lua_State* L)	{ return sPushSaveVal(L, defTASK_JX2_CUMULATEOFFER); }
int LuaJX2_AddCumulateOffer(Lua_State* L)	{ return sAddSaveVal(L, defTASK_JX2_CUMULATEOFFER); }
int LuaJX2_GetWeekGoalOffer(Lua_State* L)	{ return sPushSaveVal(L, defTASK_JX2_WEEKGOALOFFER); }
int LuaJX2_AddWeekGoalOffer(Lua_State* L)	{ return sAddSaveVal(L, defTASK_JX2_WEEKGOALOFFER); }

// SetWeeklyOffer(n) - dat thang (engine JX2 dung khi don tuan)
int LuaJX2_SetWeeklyOffer(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || !Lua_IsNumber(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Player[nPlayerIndex].m_cTask.SetSaveVal(defTASK_JX2_WEEKLYOFFER,
		(DWORD)(int)Lua_ValueToNumber(L, 1));
	Lua_PushNumber(L, 1);
	return 1;
}

// ---- thoi gian / tien ich ----

// GetCurServerTime() -> giay epoch (script JX2 cong tru truc tiep 7*24*3600)
int LuaJX2_GetCurServerTime(Lua_State* L)
{
	Lua_PushNumber(L, (double)time(NULL));
	return 1;
}

// FormatTime2String(epoch) -> "YYYY-MM-DD HH:MM:SS"
int LuaJX2_FormatTime2String(Lua_State* L)
{
	time_t tVal = Lua_IsNumber(L, 1) ? (time_t)Lua_ValueToNumber(L, 1) : time(NULL);
	struct tm* pTm = localtime(&tVal);
	char szBuf[32];
	if (pTm)
		sprintf(szBuf, "%04d-%02d-%02d %02d:%02d:%02d",
			pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday,
			pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
	else
		szBuf[0] = 0;
	Lua_PushString(L, szBuf);
	return 1;
}

// FormatTime2Number(epoch) -> so YYYYMMDD
int LuaJX2_FormatTime2Number(Lua_State* L)
{
	time_t tVal = Lua_IsNumber(L, 1) ? (time_t)Lua_ValueToNumber(L, 1) : time(NULL);
	struct tm* pTm = localtime(&tVal);
	int nNum = 0;
	if (pTm)
		nNum = (pTm->tm_year + 1900) * 10000 + (pTm->tm_mon + 1) * 100 + pTm->tm_mday;
	Lua_PushNumber(L, nNum);
	return 1;
}

// String2Id(sz) -> ma bam ten (cung cong thuc voi relay g_String2Id)
int LuaJX2_String2Id(Lua_State* L)
{
	if (!Lua_IsString(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, (double)g_FileName2Id((LPSTR)Lua_ValueToString(L, 1)));
	return 1;
}

static void sJX2_ScriptLog(const char* pszFile, const char* pszText)
{
	if (!pszText)
		return;
	FILE* pFile = fopen(pszFile, "at");
	if (!pFile)
		return;
	time_t tNow = time(NULL);
	struct tm* pTm = localtime(&tNow);
	fprintf(pFile, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
		pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday,
		pTm->tm_hour, pTm->tm_min, pTm->tm_sec, pszText);
	fclose(pFile);
}

// OutputMsg(sz) - JX2 in console; JX1 ghi debug log + file
int LuaJX2_OutputMsg(Lua_State* L)
{
	if (!Lua_IsString(L, 1))
		return 0;
	const char* psz = (const char*)Lua_ValueToString(L, 1);
	g_DebugLog("[TONGJX2-script] %s", psz);
	sJX2_ScriptLog("logs\\script_jx2.log", psz);
	return 0;
}

// WriteLog(sz) - log chung cua script
int LuaJX2_WriteLog(Lua_State* L)
{
	if (Lua_IsString(L, 1))
		sJX2_ScriptLog("logs\\script_jx2.log", (const char*)Lua_ValueToString(L, 1));
	return 0;
}

// WriteStringToFile(szPath, szText) - ghi noi (append)
int LuaJX2_WriteStringToFile(Lua_State* L)
{
	if (!Lua_IsString(L, 1) || !Lua_IsString(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	FILE* pFile = fopen((const char*)Lua_ValueToString(L, 1), "ab");
	if (!pFile)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	const char* psz = (const char*)Lua_ValueToString(L, 2);
	fwrite(psz, 1, strlen(psz), pFile);
	fclose(pFile);
	Lua_PushNumber(L, 1);
	return 1;
}

// GlobalExecute(szLua) - JX2 chay cau lenh tren moi GameServer; JX1 mot GS -> chay ngay
int LuaJX2_GlobalExecute(Lua_State* L)
{
	if (!Lua_IsString(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	lua_dostring(L, (const char*)Lua_ValueToString(L, 1));
	Lua_PushNumber(L, 1);
	return 1;
}

// SyncTaskValue(...) - JX2 day bien nhiem vu xuong client UI; JX1 chua co kenh nay
int LuaJX2_SyncTaskValue(Lua_State* L)
{
	Lua_PushNumber(L, 1);
	return 1;
}

// AskClientForNumber(...) - can hop nhap so phia client (lam o giai doan cua so)
int LuaJX2_AskClientForNumber(Lua_State* L)
{
	Lua_PushNumber(L, -1);
	return 1;
}

// ---- cap nguoi choi ve bang (doc ban sao JX2) ----

static KTongJX2Member* sJX2_MyMember(Lua_State* L, KTongJX2Tong** ppTong)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (ppTong)
		*ppTong = NULL;
	if (nPlayerIndex <= 0)
		return NULL;
	KTongJX2Member* pMe = NULL;
	KTongJX2Tong* pTong = sJX2_PlayerTong(nPlayerIndex, &pMe);
	if (ppTong)
		*ppTong = pTong;
	return pMe;
}

// GetTong() -> nTongID cua ban than (0 = chua vao bang)
int LuaJX2_GetTong(Lua_State* L)
{
	KTongJX2Tong* pTong = NULL;
	sJX2_MyMember(L, &pTong);
	Lua_PushNumber(L, pTong ? (double)pTong->dwNameID : 0);
	return 1;
}

// GetTongMemberID() -> NameID cua ban than trong bang (0 = chua vao bang)
int LuaJX2_GetTongMemberID(Lua_State* L)
{
	KTongJX2Member* pMe = sJX2_MyMember(L, NULL);
	Lua_PushNumber(L, pMe ? (double)pMe->dwNameID : 0);
	return 1;
}

// GetTongFigure() -> 0..4 (-1 = chua vao bang)
int LuaJX2_GetTongFigure(Lua_State* L)
{
	KTongJX2Member* pMe = sJX2_MyMember(L, NULL);
	Lua_PushNumber(L, pMe ? (double)pMe->btFigure : -1);
	return 1;
}

// CheckTongMasterPower() -> 1 neu la bang chu
int LuaJX2_CheckTongMasterPower(Lua_State* L)
{
	KTongJX2Member* pMe = sJX2_MyMember(L, NULL);
	Lua_PushNumber(L, (pMe && pMe->btFigure == 0) ? 1 : 0);
	return 1;
}

// GetNpcTong(nNpcIdx) - NPC JX1 khong thuoc bang
int LuaJX2_GetNpcTong(Lua_State* L)
{
	Lua_PushNumber(L, 0);
	return 1;
}

// SetTongMaster(...) - doi bang chu qua script: JX1 dung flow cua so cu (chua ho tro)
int LuaJX2_SetTongMaster(Lua_State* L)
{
	g_DebugLog("[TONGJX2-script] SetTongMaster: dung cua so doi chuc cua JX1");
	Lua_PushNumber(L, 0);
	return 1;
}

// GetTongLogData() - JX2 doc so ghi chep engine; JX1 xem logs\tong_jx2.log
int LuaJX2_GetTongLogData(Lua_State* L)
{
	Lua_PushString(L, (char*)"");
	return 1;
}

// TongClaimWar(nTongID) - tuyen chien: dat WarState (field 11) + ghi su kien
int LuaJX2_TongClaimWar(Lua_State* L)
{
	DWORD dwTongID = 0;
	if (Lua_IsNumber(L, 1))
		dwTongID = (DWORD)Lua_ValueToNumber(L, 1);
	if (dwTongID == 0)
	{
		KTongJX2Tong* pTong = NULL;
		sJX2_MyMember(L, &pTong);
		if (pTong)
			dwTongID = pTong->dwNameID;
	}
	if (!g_TongJX2.FindTong(dwTongID))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendFieldCmd(dwTongID, 11, 1, defTONG_JX2_OP_SET, sLuaPlayerParam(L));
	{
		// Nguyen van tong.lua:742 (HAI dau cach sau "chu", KHONG cach truoc
		// ten bang dich). Tham so 2 tuy chon = NameID bang bi tuyen chien.
		const char* szDest = "";
		if (Lua_IsNumber(L, 2))
		{
			KTongJX2Tong* pDest = g_TongJX2.FindTong((DWORD)Lua_ValueToNumber(L, 2));
			if (pDest)
				szDest = pDest->szName;
		}
		char szLog[160];
		sprintf(szLog, "Bang ch\361  \256\267 tuy\252n chi\325n bang h\351i%s r\345i", szDest);
		sSendStringCmd(dwTongID, defTONG_JX2_STR_EVENT, szLog, sLuaPlayerParam(L));
	}
	Lua_PushNumber(L, 1);
	return 1;
}

// GetTongMTask(wKey) / SetTongMTask(wKey, nVal) / AddTongMTask(wKey, nDelta)
// = bien nhiem vu thanh vien cua CHINH MINH (JX2 2.2 - kho thu hai; JX1 hop nhat
// vao kho thanh vien dong bo relay de khong lech nhau nhu JX2)
int LuaJX2_GetTongMTask(Lua_State* L)
{
	KTongJX2Member* pMe = sJX2_MyMember(L, NULL);
	WORD wKey = Lua_IsNumber(L, 1) ? (WORD)Lua_ValueToNumber(L, 1) : 0;
	Lua_PushNumber(L, (double)(int)g_TongJX2.GetMemberField(pMe, wKey));
	return 1;
}

int LuaJX2_SetTongMTask(Lua_State* L)
{
	KTongJX2Tong* pTong = NULL;
	KTongJX2Member* pMe = sJX2_MyMember(L, &pTong);
	if (!pTong || !pMe || !Lua_IsNumber(L, 1) || !Lua_IsNumber(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendMemberFieldCmd(pTong->dwNameID, pMe->dwNameID,
		(WORD)Lua_ValueToNumber(L, 1), (DWORD)Lua_ValueToNumber(L, 2),
		defTONG_JX2_OP_SET, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaJX2_AddTongMTask(Lua_State* L)
{
	KTongJX2Tong* pTong = NULL;
	KTongJX2Member* pMe = sJX2_MyMember(L, &pTong);
	if (!pTong || !pMe || !Lua_IsNumber(L, 1) || !Lua_IsNumber(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sSendMemberFieldCmd(pTong->dwNameID, pMe->dwNameID,
		(WORD)Lua_ValueToNumber(L, 1), (DWORD)(int)Lua_ValueToNumber(L, 2),
		defTONG_JX2_OP_ADD, sLuaPlayerParam(L));
	Lua_PushNumber(L, 1);
	return 1;
}

#endif // _SERVER
