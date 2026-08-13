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
#include "KWin32.h"

#ifdef _SERVER

#include "KEngine.h"
#include "KDebug.h"
#include "LuaLib.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KProtocolProcess.h"
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

#endif // _SERVER
