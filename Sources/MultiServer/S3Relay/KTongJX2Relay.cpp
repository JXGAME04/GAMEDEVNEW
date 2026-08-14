// KTongJX2Relay.cpp - JX2 port: du lieu KV + quyen bang hoi phia Relay.
// - Bang: map field {WORD -> DWORD}, field 1..48 theo bang JX2, bien nhiem vu >= 1002.
//   Ngan quy la so 64-bit tach doi: field 3 (32 bit thap) + field 4 (32 bit cao).
//   Dat gia tri 0 = XOA khoa (dung ngu nghia KStructData::SetField cua JX2).
// - Thanh vien: map field {WORD -> DWORD} khoa 1..16 + bien nhiem vu >= 1001,
//   va danh sach quyen (set cac RightID).
// - Moi thay doi: ghi ngay xuong Berkeley DB + phat echo (op=SET, gia tri sau ghi)
//   toi MOI GameServer de ban sao hoi tu.
// Relay don luong (_WORKMODE_SINGLETHREAD) - KHONG dong cham g_cTongSet tu luong khac.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
#include "S3Relay.h"
#include "TongDB.h"
#include "KTongSet.h"
#include "TongConnect.h"
#include "TongServer.h"
#include "KTongJX2Relay.h"
#include <vector>
#include <stdio.h>
#include <time.h>

//////////////////////////////////////////////////////////////////////
// CTongControl - phan du lieu JX2
//////////////////////////////////////////////////////////////////////

void CTongControl::JX2_Reset()
{
	m_nJX2FieldCount = 0;
	memset(m_wJX2FieldKey, 0, sizeof(m_wJX2FieldKey));
	memset(m_dwJX2FieldVal, 0, sizeof(m_dwJX2FieldVal));
	m_mapJX2Member.clear();
	memset(m_szJX2Announce, 0, sizeof(m_szJX2Announce));
	memset(m_szJX2Recruit, 0, sizeof(m_szJX2Recruit));
	memset(m_szJX2Event, 0, sizeof(m_szJX2Event));
	memset(m_szJX2History, 0, sizeof(m_szJX2History));
	m_nJX2EventHead = 0;
	m_nJX2HistoryHead = 0;
}

// Tim vi tri khoa trong mang field cap bang; -1 = khong co
int CTongControl::JX2_FindFieldIdx(WORD wKey)
{
	for (int i = 0; i < m_nJX2FieldCount; i++)
	{
		if (m_wJX2FieldKey[i] == wKey)
			return i;
	}
	return -1;
}

DWORD CTongControl::JX2_GetField(WORD wKey)
{
	int nIdx = JX2_FindFieldIdx(wKey);
	if (nIdx < 0)
		return 0;	// khong co khoa == gia tri 0 (ngu nghia JX2)
	return m_dwJX2FieldVal[nIdx];
}

BOOL CTongControl::JX2_SetField(WORD wKey, DWORD dwValue)
{
	int nIdx = JX2_FindFieldIdx(wKey);
	if (dwValue == 0)
	{
		// gia tri 0 = xoa khoa (dung JX2: KStructData::SetField xoa node khi = 0)
		if (nIdx >= 0)
		{
			m_nJX2FieldCount--;
			m_wJX2FieldKey[nIdx] = m_wJX2FieldKey[m_nJX2FieldCount];
			m_dwJX2FieldVal[nIdx] = m_dwJX2FieldVal[m_nJX2FieldCount];
			m_wJX2FieldKey[m_nJX2FieldCount] = 0;
			m_dwJX2FieldVal[m_nJX2FieldCount] = 0;
		}
		return TRUE;
	}
	if (nIdx >= 0)
	{
		m_dwJX2FieldVal[nIdx] = dwValue;
		return TRUE;
	}
	if (m_nJX2FieldCount >= defTONG_JX2_MAX_FIELDS)
	{
		rTRACE("[TONGJX2] tong %s: het cho field (key %u)", m_szName, (DWORD)wKey);
		return FALSE;
	}
	m_wJX2FieldKey[m_nJX2FieldCount] = wKey;
	m_dwJX2FieldVal[m_nJX2FieldCount] = dwValue;
	m_nJX2FieldCount++;
	return TRUE;
}

BOOL CTongControl::JX2_AddField(WORD wKey, int nDelta, BOOL bUnsigned)
{
	DWORD dwCur = JX2_GetField(wKey);
	DWORD dwNew;
	if (bUnsigned)
		dwNew = dwCur + (DWORD)nDelta;
	else
		dwNew = (DWORD)((int)dwCur + nDelta);
	return JX2_SetField(wKey, dwNew);
}

__int64 CTongControl::JX2_GetMoney64()
{
	// field 3 = 32 bit thap, field 4 = 32 bit cao (bang field JX2)
	return ((__int64)JX2_GetField(4) << 32) | (__int64)JX2_GetField(3);
}

void CTongControl::JX2_SetMoney64(__int64 nMoney)
{
	JX2_SetField(3, (DWORD)(nMoney & 0xFFFFFFFF));
	JX2_SetField(4, (DWORD)((nMoney >> 32) & 0xFFFFFFFF));
}

CTongControl::JX2Member* CTongControl::JX2_GetMember(DWORD dwNameID, BOOL bCreate)
{
	std::map<DWORD, JX2Member>::iterator it = m_mapJX2Member.find(dwNameID);
	if (it != m_mapJX2Member.end())
		return &it->second;
	if (!bCreate)
		return NULL;
	JX2Member sNew;
	memset(&sNew, 0, sizeof(sNew));
	m_mapJX2Member[dwNameID] = sNew;
	return &m_mapJX2Member[dwNameID];
}

static int sJX2_FindMemberFieldIdx(CTongControl::JX2Member* pMem, WORD wKey)
{
	for (int i = 0; i < pMem->nFieldCount; i++)
	{
		if (pMem->wFieldKey[i] == wKey)
			return i;
	}
	return -1;
}

DWORD CTongControl::JX2_GetMemberField(DWORD dwNameID, WORD wKey)
{
	JX2Member* pMem = JX2_GetMember(dwNameID, FALSE);
	if (!pMem)
		return 0;
	int nIdx = sJX2_FindMemberFieldIdx(pMem, wKey);
	if (nIdx < 0)
		return 0;
	return pMem->dwFieldVal[nIdx];
}

BOOL CTongControl::JX2_SetMemberField(DWORD dwNameID, WORD wKey, DWORD dwValue)
{
	JX2Member* pMem = JX2_GetMember(dwNameID, dwValue != 0);
	if (!pMem)
		return dwValue == 0;	// xoa tren nguoi chua co du lieu = thanh cong
	int nIdx = sJX2_FindMemberFieldIdx(pMem, wKey);
	if (dwValue == 0)
	{
		if (nIdx >= 0)
		{
			pMem->nFieldCount--;
			pMem->wFieldKey[nIdx] = pMem->wFieldKey[pMem->nFieldCount];
			pMem->dwFieldVal[nIdx] = pMem->dwFieldVal[pMem->nFieldCount];
			pMem->wFieldKey[pMem->nFieldCount] = 0;
			pMem->dwFieldVal[pMem->nFieldCount] = 0;
		}
		return TRUE;
	}
	if (nIdx >= 0)
	{
		pMem->dwFieldVal[nIdx] = dwValue;
		return TRUE;
	}
	if (pMem->nFieldCount >= defTONG_JX2_MEMBER_FIELDS)
	{
		rTRACE("[TONGJX2] tong %s: thanh vien %u het cho field (key %u)", m_szName, dwNameID, (DWORD)wKey);
		return FALSE;
	}
	pMem->wFieldKey[pMem->nFieldCount] = wKey;
	pMem->dwFieldVal[pMem->nFieldCount] = dwValue;
	pMem->nFieldCount++;
	return TRUE;
}

BOOL CTongControl::JX2_AddMemberField(DWORD dwNameID, WORD wKey, int nDelta, BOOL bUnsigned)
{
	DWORD dwCur = JX2_GetMemberField(dwNameID, wKey);
	DWORD dwNew;
	if (bUnsigned)
		dwNew = dwCur + (DWORD)nDelta;
	else
		dwNew = (DWORD)((int)dwCur + nDelta);
	return JX2_SetMemberField(dwNameID, wKey, dwNew);
}

BOOL CTongControl::JX2_ChangeRight(DWORD dwNameID, DWORD dwRightID, BOOL bAdd)
{
	JX2Member* pMem = JX2_GetMember(dwNameID, bAdd);
	if (!pMem)
		return !bAdd;	// xoa quyen tren nguoi chua co du lieu = coi nhu xong
	int i;
	for (i = 0; i < pMem->nRightCount; i++)
	{
		if (pMem->dwRight[i] == dwRightID)
			break;
	}
	if (bAdd)
	{
		if (i < pMem->nRightCount)
			return TRUE;	// da co
		if (pMem->nRightCount >= defTONG_JX2_MEMBER_RIGHTS)
		{
			rTRACE("[TONGJX2] tong %s: thanh vien %u het cho quyen", m_szName, dwNameID);
			return FALSE;
		}
		pMem->dwRight[pMem->nRightCount] = dwRightID;
		pMem->nRightCount++;
		return TRUE;
	}
	// xoa
	if (i >= pMem->nRightCount)
		return TRUE;	// khong co san
	pMem->nRightCount--;
	pMem->dwRight[i] = pMem->dwRight[pMem->nRightCount];
	pMem->dwRight[pMem->nRightCount] = 0;
	return TRUE;
}

// Tra ve ten thanh vien theo NameID (tim trong bang chu / truong lao / doi truong / bang chung)
BOOL CTongControl::JX2_GetMemberName(DWORD dwNameID, char* pszOut, int nOutSize)
{
	if (!pszOut || nOutSize <= 0 || dwNameID == 0)
		return FALSE;
	int i;
	if (dwNameID == m_dwMasterID)
	{
		strncpy(pszOut, m_szMasterName, nOutSize - 1);
		pszOut[nOutSize - 1] = 0;
		return TRUE;
	}
	for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
	{
		if (m_dwDirectorID[i] == dwNameID)
		{
			strncpy(pszOut, m_szDirectorName[i], nOutSize - 1);
			pszOut[nOutSize - 1] = 0;
			return TRUE;
		}
	}
	for (i = 0; i < defTONG_MAX_MANAGER; i++)
	{
		if (m_dwManagerID[i] == dwNameID)
		{
			strncpy(pszOut, m_szManagerName[i], nOutSize - 1);
			pszOut[nOutSize - 1] = 0;
			return TRUE;
		}
	}
	for (i = 0; i < m_nMemberPointSize; i++)
	{
		if (m_psMember[i].m_dwNameID == 0)
			break;
		if (m_psMember[i].m_dwNameID == dwNameID)
		{
			strncpy(pszOut, m_psMember[i].m_szName, nOutSize - 1);
			pszOut[nOutSize - 1] = 0;
			return TRUE;
		}
	}
	return FALSE;
}

void CTongControl::JX2_SaveToStruct(TTongStruct* pStruct)
{
	if (!pStruct)
		return;
	pStruct->nJX2FieldCount = m_nJX2FieldCount;
	memcpy(pStruct->wJX2FieldKey, m_wJX2FieldKey, sizeof(pStruct->wJX2FieldKey));
	memcpy(pStruct->dwJX2FieldVal, m_dwJX2FieldVal, sizeof(pStruct->dwJX2FieldVal));
	memcpy(pStruct->szJX2Announce, m_szJX2Announce, sizeof(pStruct->szJX2Announce));
	memcpy(pStruct->szJX2Recruit, m_szJX2Recruit, sizeof(pStruct->szJX2Recruit));
	memcpy(pStruct->szJX2Event, m_szJX2Event, sizeof(pStruct->szJX2Event));
	memcpy(pStruct->szJX2History, m_szJX2History, sizeof(pStruct->szJX2History));
	pStruct->nJX2EventHead = m_nJX2EventHead;
	pStruct->nJX2HistoryHead = m_nJX2HistoryHead;
}

void CTongControl::JX2_LoadFromStruct(const TTongStruct* pStruct)
{
	if (!pStruct)
		return;
	int nCount = pStruct->nJX2FieldCount;
	if (nCount < 0)
		nCount = 0;
	if (nCount > defTONG_JX2_MAX_FIELDS)
		nCount = defTONG_JX2_MAX_FIELDS;
	m_nJX2FieldCount = nCount;
	memcpy(m_wJX2FieldKey, pStruct->wJX2FieldKey, sizeof(m_wJX2FieldKey));
	memcpy(m_dwJX2FieldVal, pStruct->dwJX2FieldVal, sizeof(m_dwJX2FieldVal));
	memcpy(m_szJX2Announce, pStruct->szJX2Announce, sizeof(m_szJX2Announce));
	m_szJX2Announce[defTONG_JX2_ANNOUNCE_LEN - 1] = 0;
	memcpy(m_szJX2Recruit, pStruct->szJX2Recruit, sizeof(m_szJX2Recruit));
	m_szJX2Recruit[defTONG_JX2_ANNOUNCE_LEN - 1] = 0;
	memcpy(m_szJX2Event, pStruct->szJX2Event, sizeof(m_szJX2Event));
	memcpy(m_szJX2History, pStruct->szJX2History, sizeof(m_szJX2History));
	m_nJX2EventHead = pStruct->nJX2EventHead;
	if (m_nJX2EventHead < 0 || m_nJX2EventHead >= defTONG_JX2_RECORD_NUM)
		m_nJX2EventHead = 0;
	m_nJX2HistoryHead = pStruct->nJX2HistoryHead;
	if (m_nJX2HistoryHead < 0 || m_nJX2HistoryHead >= defTONG_JX2_RECORD_NUM)
		m_nJX2HistoryHead = 0;
}

// Nap phan duoi JX2 cua 1 ban ghi thanh vien (goi tu CTongDB::SearchTong)
void CTongControl::JX2_LoadMemberFromStruct(const void* pMemberRecord)
{
	const TMemberStruct* pRec = (const TMemberStruct*)pMemberRecord;
	if (!pRec || !pRec->szName[0])
		return;
	int nFields = pRec->nJX2FieldCount;
	int nRights = pRec->nJX2RightCount;
	if (nFields <= 0 && nRights <= 0)
		return;		// khong co du lieu JX2 -> khong tao muc rong
	if (nFields < 0)
		nFields = 0;
	if (nFields > defTONG_JX2_MEMBER_FIELDS)
		nFields = defTONG_JX2_MEMBER_FIELDS;
	if (nRights < 0)
		nRights = 0;
	if (nRights > defTONG_JX2_MEMBER_RIGHTS)
		nRights = defTONG_JX2_MEMBER_RIGHTS;

	DWORD dwNameID = g_String2Id((char*)pRec->szName);
	JX2Member* pMem = JX2_GetMember(dwNameID, TRUE);
	if (!pMem)
		return;
	pMem->nFieldCount = nFields;
	memcpy(pMem->wFieldKey, pRec->wJX2FieldKey, sizeof(pMem->wFieldKey));
	memcpy(pMem->dwFieldVal, pRec->dwJX2FieldVal, sizeof(pMem->dwFieldVal));
	pMem->nRightCount = nRights;
	memcpy(pMem->dwRight, pRec->dwJX2Right, sizeof(pMem->dwRight));
}

// Gom danh sach thanh vien (bang chu + truong lao + doi truong + bang chung),
// khu trung theo NameID. pArr == NULL -> chi tra ve so luong can.
int CTongControl::JX2_CollectMembers(JX2MemberBrief* pArr, int nMax)
{
	int nCount = 0;
	int i, j;

	// bang chu
	if (m_dwMasterID != 0)
	{
		if (pArr && nCount < nMax)
		{
			pArr[nCount].dwNameID = m_dwMasterID;
			strncpy(pArr[nCount].szName, m_szMasterName, 31);
			pArr[nCount].szName[31] = 0;
			pArr[nCount].btFigure = 0;
			pArr[nCount].btSex = (BYTE)m_nMasterSex;
		}
		nCount++;
	}
	// truong lao
	for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
	{
		if (m_dwDirectorID[i] == 0)
			continue;
		if (pArr && nCount < nMax)
		{
			pArr[nCount].dwNameID = m_dwDirectorID[i];
			strncpy(pArr[nCount].szName, m_szDirectorName[i], 31);
			pArr[nCount].szName[31] = 0;
			pArr[nCount].btFigure = 1;
			pArr[nCount].btSex = (BYTE)m_nDirectorSex[i];
		}
		nCount++;
	}
	// doi truong (huong chu)
	for (i = 0; i < defTONG_MAX_MANAGER; i++)
	{
		if (m_dwManagerID[i] == 0)
			continue;
		if (pArr && nCount < nMax)
		{
			pArr[nCount].dwNameID = m_dwManagerID[i];
			strncpy(pArr[nCount].szName, m_szManagerName[i], 31);
			pArr[nCount].szName[31] = 0;
			pArr[nCount].btFigure = 2;
			pArr[nCount].btSex = (BYTE)m_nManagerSex[i];
		}
		nCount++;
	}
	// bang chung - bo qua ai da liet ke o tren
	for (i = 0; i < m_nMemberPointSize; i++)
	{
		if (m_psMember[i].m_dwNameID == 0)
			break;
		DWORD dwID = m_psMember[i].m_dwNameID;
		BOOL bDup = FALSE;
		if (dwID == m_dwMasterID)
			bDup = TRUE;
		for (j = 0; !bDup && j < defTONG_MAX_DIRECTOR; j++)
		{
			if (m_dwDirectorID[j] == dwID)
				bDup = TRUE;
		}
		for (j = 0; !bDup && j < defTONG_MAX_MANAGER; j++)
		{
			if (m_dwManagerID[j] == dwID)
				bDup = TRUE;
		}
		if (bDup)
			continue;
		if (pArr && nCount < nMax)
		{
			pArr[nCount].dwNameID = dwID;
			strncpy(pArr[nCount].szName, m_psMember[i].m_szName, 31);
			pArr[nCount].szName[31] = 0;
			pArr[nCount].btFigure = 3;
			pArr[nCount].btSex = (BYTE)m_psMember[i].m_nSex;
		}
		nCount++;
	}
	return nCount;
}

// Dung goi STONG_JX2_TONG_SYNC (thong tin bang + toan bo field) vao pBuffer.
// Tra ve so byte cua goi.
int CTongControl::JX2_BuildTongSync(void* pBuffer, int nBufSize, int nMemberTotal)
{
	int nNeed = (int)sizeof(STONG_JX2_TONG_SYNC) + m_nJX2FieldCount * 6;
	if (!pBuffer || nBufSize < nNeed)
		return 0;
	STONG_JX2_TONG_SYNC* pSync = (STONG_JX2_TONG_SYNC*)pBuffer;
	pSync->ProtocolFamily = pf_tong;
	pSync->ProtocolID = enumS2C_TONG_JX2_TONG_SYNC;
	pSync->m_dwTongNameID = m_dwNameID;
	memset(pSync->m_szTongName, 0, sizeof(pSync->m_szTongName));
	strncpy(pSync->m_szTongName, m_szName, 31);
	pSync->m_btCamp = (BYTE)m_nCamp;
	pSync->m_wMemberTotal = (WORD)nMemberTotal;
	pSync->m_wFieldCount = (WORD)m_nJX2FieldCount;
	memcpy(pSync->m_szAnnounce, m_szJX2Announce, sizeof(pSync->m_szAnnounce));
	BYTE* pOut = (BYTE*)(pSync + 1);
	for (int i = 0; i < m_nJX2FieldCount; i++)
	{
		*(WORD*)pOut = m_wJX2FieldKey[i];
		pOut += 2;
		*(DWORD*)pOut = m_dwJX2FieldVal[i];
		pOut += 4;
	}
	pSync->m_wLength = (WORD)nNeed;
	return nNeed;
}

// Dung 1 goi STONG_JX2_MEMBER_SYNC chua toi da nMaxPerPacket thanh vien,
// bat dau tu chi so *pnStartIdx cua mang pArr (da thu duoc tu JX2_CollectMembers).
// Tra ve so byte; 0 = da het.
int CTongControl::JX2_BuildMemberSync(void* pBuffer, int nBufSize,
	const JX2MemberBrief* pArr, int nTotal, int* pnStartIdx, int nMaxPerPacket)
{
	if (!pBuffer || !pArr || !pnStartIdx || *pnStartIdx >= nTotal)
		return 0;

	STONG_JX2_MEMBER_SYNC* pSync = (STONG_JX2_MEMBER_SYNC*)pBuffer;
	if (nBufSize < (int)sizeof(STONG_JX2_MEMBER_SYNC))
		return 0;
	pSync->ProtocolFamily = pf_tong;
	pSync->ProtocolID = enumS2C_TONG_JX2_MEMBER_SYNC;
	pSync->m_dwTongNameID = m_dwNameID;
	pSync->m_btCount = 0;

	BYTE* pOut = (BYTE*)(pSync + 1);
	int nUsed = (int)sizeof(STONG_JX2_MEMBER_SYNC);
	int i = *pnStartIdx;

	while (i < nTotal && pSync->m_btCount < nMaxPerPacket)
	{
		JX2Member* pMem = JX2_GetMember(pArr[i].dwNameID, FALSE);
		int nFields = pMem ? pMem->nFieldCount : 0;
		int nRights = pMem ? pMem->nRightCount : 0;
		int nItem = (int)sizeof(STONG_JX2_ONE_MEMBER) + nFields * 6 + nRights * 4;
		if (nUsed + nItem > nBufSize)
			break;

		STONG_JX2_ONE_MEMBER* pOne = (STONG_JX2_ONE_MEMBER*)pOut;
		pOne->m_dwMemberNameID = pArr[i].dwNameID;
		memset(pOne->m_szName, 0, sizeof(pOne->m_szName));
		strncpy(pOne->m_szName, pArr[i].szName, 31);
		pOne->m_btFigure = pArr[i].btFigure;
		pOne->m_btSex = pArr[i].btSex;
		pOne->m_btFieldCount = (BYTE)nFields;
		pOne->m_btRightCount = (BYTE)nRights;
		pOut += sizeof(STONG_JX2_ONE_MEMBER);
		int k;
		for (k = 0; k < nFields; k++)
		{
			*(WORD*)pOut = pMem->wFieldKey[k];
			pOut += 2;
			*(DWORD*)pOut = pMem->dwFieldVal[k];
			pOut += 4;
		}
		for (k = 0; k < nRights; k++)
		{
			*(DWORD*)pOut = pMem->dwRight[k];
			pOut += 4;
		}
		nUsed += nItem;
		pSync->m_btCount++;
		i++;
	}

	*pnStartIdx = i;
	if (pSync->m_btCount == 0)
		return 0;
	pSync->m_wLength = (WORD)nUsed;
	return nUsed;
}

//////////////////////////////////////////////////////////////////////
// CTongSet - tra cuu + dump toan bo
//////////////////////////////////////////////////////////////////////

// Duyet mang con tro bang theo chi so: tra ve (CTongControl*)-1 khi het mang,
// NULL khi o trong (de vong for cua bo hen gio khong can biet noi bo CTongSet)
CTongControl* CTongSet::JX2_GetByIndex(int nIndex)
{
	if (!m_pcTong || nIndex < 0 || nIndex >= m_nTongPointSize)
		return (CTongControl*)-1;
	return m_pcTong[nIndex];
}

CTongControl* CTongSet::JX2_FindByNameID(DWORD dwTongNameID)
{
	if (!m_pcTong || m_nTongPointSize <= 0 || dwTongNameID == 0)
		return NULL;
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == dwTongNameID)
			return m_pcTong[i];
	}
	return NULL;
}

// Gui toan bo du lieu JX2 (moi bang: TONG_SYNC + cac goi MEMBER_SYNC) roi SYNC_DONE.
// Goi khi 1 GameServer moi noi vao (OnClientConnectCreate) hoac khi GS xin GET_FULL.
void CTongSet::JX2_SendFullDump(CNetConnect* pConn)
{
	if (!pConn)
		return;
	BYTE	byBuffer[4096];
	DWORD	dwCount = 0;

	if (m_pcTong && m_nTongPointSize > 0)
	{
		for (int i = 0; i < m_nTongPointSize; i++)
		{
			CTongControl* pTong = m_pcTong[i];
			if (!pTong || !pTong->m_szName[0])
				continue;

			int nMemberTotal = pTong->JX2_CollectMembers(NULL, 0);
			int nBytes = pTong->JX2_BuildTongSync(byBuffer, sizeof(byBuffer), nMemberTotal);
			if (nBytes > 0)
				pConn->SendPackage(byBuffer, nBytes);

			if (nMemberTotal > 0)
			{
				std::vector<JX2MemberBrief> vecMember(nMemberTotal);
				int nGot = pTong->JX2_CollectMembers(&vecMember[0], nMemberTotal);
				int nStart = 0;
				while (nStart < nGot)
				{
					nBytes = pTong->JX2_BuildMemberSync(byBuffer, sizeof(byBuffer),
						&vecMember[0], nGot, &nStart, 4);
					if (nBytes <= 0)
						break;
					pConn->SendPackage(byBuffer, nBytes);
				}
			}
			if (pTong->m_szJX2Recruit[0])
			{
				// van an chieu mo khong nam trong TONG_SYNC - gui rieng 1 goi STRING_SYNC
				STONG_JX2_STRING_COMMAND sStr;
				memset(&sStr, 0, sizeof(sStr));
				sStr.ProtocolFamily = pf_tong;
				sStr.ProtocolID = enumS2C_TONG_JX2_STRING_SYNC;
				sStr.m_dwTongNameID = pTong->m_dwNameID;
				sStr.m_btKind = defTONG_JX2_STR_RECRUIT;
				memcpy(sStr.m_szText, pTong->m_szJX2Recruit, sizeof(sStr.m_szText));
				pConn->SendPackage(&sStr, sizeof(sStr));
			}
			{
				// gui so su kien + lich su (ring cu -> moi de GS append dung thu tu)
				STONG_JX2_STRING_COMMAND sStr;
				int r;
				for (r = 0; r < defTONG_JX2_RECORD_NUM * 2; r++)
				{
					BOOL bHist = (r >= defTONG_JX2_RECORD_NUM);
					int nIdx = r % defTONG_JX2_RECORD_NUM;
					int nHead = bHist ? pTong->m_nJX2HistoryHead : pTong->m_nJX2EventHead;
					const char* pszLine = bHist ?
						pTong->m_szJX2History[(nHead + nIdx) % defTONG_JX2_RECORD_NUM] :
						pTong->m_szJX2Event[(nHead + nIdx) % defTONG_JX2_RECORD_NUM];
					if (!pszLine[0])
						continue;
					memset(&sStr, 0, sizeof(sStr));
					sStr.ProtocolFamily = pf_tong;
					sStr.ProtocolID = enumS2C_TONG_JX2_STRING_SYNC;
					sStr.m_dwTongNameID = pTong->m_dwNameID;
					sStr.m_btKind = bHist ? defTONG_JX2_STR_HISTORY : defTONG_JX2_STR_EVENT;
					strncpy(sStr.m_szText, pszLine, defTONG_JX2_RECORD_LEN - 1);
					pConn->SendPackage(&sStr, sizeof(sStr));
				}
			}
			dwCount++;
		}
	}

	STONG_JX2_SYNC_DONE sDone;
	sDone.ProtocolFamily = pf_tong;
	sDone.ProtocolID = enumS2C_TONG_JX2_SYNC_DONE;
	sDone.m_dwTongCount = dwCount;
	pConn->SendPackage(&sDone, sizeof(sDone));
	rTRACE("[TONGJX2] dump %u tong -> GS %s", dwCount, _ip2a(pConn->GetIP()));
}

//////////////////////////////////////////////////////////////////////
// CTongServer - phat goi toi moi GameServer
//////////////////////////////////////////////////////////////////////

int CTongServer::BroadcastPackage(const void* pData, size_t nSize)
{
	if (!pData || nSize == 0)
		return 0;
	// Phat qua ban do theo ID KET NOI cua lop co (m_mapId2Connect), KHONG dung
	// m_mapIp2Connect: ban do do khoa bang DIA CHI IP, ma hai GameServer chay
	// cung mot may (GameServer_cfg.ini va GameServer1_cfg.ini deu tro
	// [Tong] Ip=127.0.0.1) se de len nhau - chi mot GS nhan duoc dong bo, GS con
	// lai vinh vien khong biet bang hoi doi gi. m_mapIp2Connect van giu nguyen
	// vi FindTongConnectByIP / FindPlayerByIpParam con can tra theo IP.
	return CNetServer::BroadPackage(pData, nSize) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////
// CTongDB - bao ton + cap nhat phan duoi JX2 cua ban ghi thanh vien
//////////////////////////////////////////////////////////////////////

// Giu lai phan duoi JX2 tu ban ghi cu (neu co) truoc khi ghi de.
// Goi o dau CTongDB::ChangeMember - moi cho goi ChangeMember hien co
// deu dung TMemberStruct tren stack CHUA khoi tao phan duoi.
void CTongDB::JX2_PreserveMemberTail(TMemberStruct* pMember)
{
	if (!pMember)
		return;
	// mac dinh: xoa trang phan duoi
	pMember->nJX2FieldCount = 0;
	memset(pMember->wJX2FieldKey, 0, sizeof(pMember->wJX2FieldKey));
	memset(pMember->dwJX2FieldVal, 0, sizeof(pMember->dwJX2FieldVal));
	pMember->nJX2RightCount = 0;
	memset(pMember->dwJX2Right, 0, sizeof(pMember->dwJX2Right));

	if (!pMember->szName[0])
		return;
	int nKeySize = (int)strlen(pMember->szName) + 1;
	ZCursor* cursor = m_MemberTable->search(pMember->szName, nKeySize);
	if (!cursor)
		return;
	if (cursor->size == (int)sizeof(TMemberStruct))
	{
		TMemberStruct* pOld = (TMemberStruct*)cursor->data;
		pMember->nJX2FieldCount = pOld->nJX2FieldCount;
		memcpy(pMember->wJX2FieldKey, pOld->wJX2FieldKey, sizeof(pMember->wJX2FieldKey));
		memcpy(pMember->dwJX2FieldVal, pOld->dwJX2FieldVal, sizeof(pMember->dwJX2FieldVal));
		pMember->nJX2RightCount = pOld->nJX2RightCount;
		memcpy(pMember->dwJX2Right, pOld->dwJX2Right, sizeof(pMember->dwJX2Right));
	}
	m_MemberTable->closeCursor(cursor);
}

// Ghi phan duoi JX2 cua 1 thanh vien xuong DB (doc ban ghi cu, giu nguyen
// phan than, thay phan duoi). Tra ve false neu thanh vien chua co ban ghi.
bool CTongDB::JX2_UpdateMemberTail(const char* szName, const void* pJX2Data)
{
	if (!szName || !szName[0] || !pJX2Data)
		return false;
	const CTongControl::JX2Member* pSrc = (const CTongControl::JX2Member*)pJX2Data;

	int nKeySize = (int)strlen(szName) + 1;
	ZCursor* cursor = m_MemberTable->search((char*)szName, nKeySize);
	if (!cursor)
	{
		rTRACE("[TONGJX2] JX2_UpdateMemberTail: %s chua co ban ghi thanh vien", szName);
		return false;
	}
	if (cursor->size != (int)sizeof(TMemberStruct))
	{
		m_MemberTable->closeCursor(cursor);
		rTRACE("[TONGJX2] JX2_UpdateMemberTail: ban ghi %s lech co", szName);
		return false;
	}
	TMemberStruct sRec;
	memcpy(&sRec, cursor->data, sizeof(TMemberStruct));
	m_MemberTable->closeCursor(cursor);

	sRec.nJX2FieldCount = pSrc->nFieldCount;
	memcpy(sRec.wJX2FieldKey, pSrc->wFieldKey, sizeof(sRec.wJX2FieldKey));
	memcpy(sRec.dwJX2FieldVal, pSrc->dwFieldVal, sizeof(sRec.dwJX2FieldVal));
	sRec.nJX2RightCount = pSrc->nRightCount;
	memcpy(sRec.dwJX2Right, pSrc->dwRight, sizeof(sRec.dwJX2Right));

	return m_MemberTable->add(sRec.szName, (int)strlen(sRec.szName) + 1,
		(char*)&sRec, (int)sizeof(TMemberStruct));
}

// Cap nhat chuc vu (MemberClass + nTitleIndex) cua ban ghi thanh vien,
// giu nguyen moi phan con lai (doc-sua-ghi nhu JX2_UpdateMemberTail).
bool CTongDB::JX2_UpdateMemberClass(const char* szName, int nClass, int nTitleIdx)
{
	if (!szName || !szName[0])
		return false;
	int nKeySize = (int)strlen(szName) + 1;
	ZCursor* cursor = m_MemberTable->search((char*)szName, nKeySize);
	if (!cursor)
		return false;
	if (cursor->size != (int)sizeof(TMemberStruct))
	{
		m_MemberTable->closeCursor(cursor);
		return false;
	}
	TMemberStruct sRec;
	memcpy(&sRec, cursor->data, sizeof(TMemberStruct));
	m_MemberTable->closeCursor(cursor);

	sRec.MemberClass = (TONG_MEMBER_FIGURE)nClass;
	sRec.nTitleIndex = nTitleIdx;
	return m_MemberTable->add(sRec.szName, (int)strlen(sRec.szName) + 1,
		(char*)&sRec, (int)sizeof(TMemberStruct));
}

//////////////////////////////////////////////////////////////////////
// Cac ham xu ly goi enumC2S_TONG_JX2_* (goi tu CTongConnect::Proc0_Tong)
//////////////////////////////////////////////////////////////////////

void JX2_ProcTongField(CTongConnect* pConn, const void* pData)
{
	STONG_JX2_FIELD_COMMAND* pCmd = (STONG_JX2_FIELD_COMMAND*)pData;
	CTongControl* pTong = g_cTongSet.JX2_FindByNameID(pCmd->m_dwTongNameID);
	if (!pTong)
		return;

	BOOL bOK = FALSE;
	switch (pCmd->m_btOp)
	{
	case defTONG_JX2_OP_SET:
		bOK = pTong->JX2_SetField(pCmd->m_wKey, pCmd->m_dwValue);
		break;
	case defTONG_JX2_OP_ADD:
		bOK = pTong->JX2_AddField(pCmd->m_wKey, (int)pCmd->m_dwValue, FALSE);
		break;
	case defTONG_JX2_OP_ADDU:
		bOK = pTong->JX2_AddField(pCmd->m_wKey, (int)pCmd->m_dwValue, TRUE);
		break;
	default:
		return;
	}
	if (!bOK)
		return;

	g_cTongDB.ChangeTong(*pTong);

	// echo gia tri SAU KHI ghi (op=SET) toi moi GS de ban sao hoi tu
	STONG_JX2_FIELD_COMMAND sSync = *pCmd;
	sSync.ProtocolFamily = pf_tong;
	sSync.ProtocolID = enumS2C_TONG_JX2_FIELD_SYNC;
	sSync.m_btOp = defTONG_JX2_OP_SET;
	sSync.m_dwValue = pTong->JX2_GetField(pCmd->m_wKey);
	g_TongServer.BroadcastPackage(&sSync, sizeof(sSync));
}

void JX2_ProcTongMoney(CTongConnect* pConn, const void* pData)
{
	STONG_JX2_MONEY_COMMAND* pCmd = (STONG_JX2_MONEY_COMMAND*)pData;
	CTongControl* pTong = g_cTongSet.JX2_FindByNameID(pCmd->m_dwTongNameID);
	if (!pTong)
		return;

	__int64 nNew;
	if (pCmd->m_btOp == defTONG_JX2_OP_SET)
	{
		if (pCmd->m_nValue < 0)
			return;		// JX2: SetMoney chan gia tri am
		nNew = pCmd->m_nValue;
	}
	else if (pCmd->m_btOp == defTONG_JX2_OP_ADD)
	{
		nNew = pTong->JX2_GetMoney64() + pCmd->m_nValue;
		if (nNew < 0)
			return;		// khong du tien: BO QUA lenh. Truoc day ket ve 0, tuc la
						// khoan tru vuot muc bi xoa mat -> nguoi choi nhan duoc
						// tien ma ngan quy khong bi tru du = TAO TIEN tu khong.
	}
	else
		return;

	pTong->JX2_SetMoney64(nNew);
	g_cTongDB.ChangeTong(*pTong);

	STONG_JX2_MONEY_COMMAND sSync = *pCmd;
	sSync.ProtocolFamily = pf_tong;
	sSync.ProtocolID = enumS2C_TONG_JX2_MONEY_SYNC;
	sSync.m_btOp = defTONG_JX2_OP_SET;
	sSync.m_nValue = nNew;
	g_TongServer.BroadcastPackage(&sSync, sizeof(sSync));
}

void JX2_ProcMemberField(CTongConnect* pConn, const void* pData)
{
	STONG_JX2_MEMBER_FIELD_COMMAND* pCmd = (STONG_JX2_MEMBER_FIELD_COMMAND*)pData;
	CTongControl* pTong = g_cTongSet.JX2_FindByNameID(pCmd->m_dwTongNameID);
	if (!pTong)
		return;

	BOOL bOK = FALSE;
	switch (pCmd->m_btOp)
	{
	case defTONG_JX2_OP_SET:
		bOK = pTong->JX2_SetMemberField(pCmd->m_dwMemberNameID, pCmd->m_wKey, pCmd->m_dwValue);
		break;
	case defTONG_JX2_OP_ADD:
		bOK = pTong->JX2_AddMemberField(pCmd->m_dwMemberNameID, pCmd->m_wKey, (int)pCmd->m_dwValue, FALSE);
		break;
	case defTONG_JX2_OP_ADDU:
		bOK = pTong->JX2_AddMemberField(pCmd->m_dwMemberNameID, pCmd->m_wKey, (int)pCmd->m_dwValue, TRUE);
		break;
	default:
		return;
	}
	if (!bOK)
		return;

	// tim ten thanh vien de ghi DB
	CTongControl::JX2Member* pMem = pTong->JX2_GetMember(pCmd->m_dwMemberNameID, FALSE);
	if (pMem)
	{
		char szName[64];
		if (pTong->JX2_GetMemberName(pCmd->m_dwMemberNameID, szName, sizeof(szName)))
			g_cTongDB.JX2_UpdateMemberTail(szName, pMem);
	}

	STONG_JX2_MEMBER_FIELD_COMMAND sSync = *pCmd;
	sSync.ProtocolFamily = pf_tong;
	sSync.ProtocolID = enumS2C_TONG_JX2_MEMBER_FIELD_SYNC;
	sSync.m_btOp = defTONG_JX2_OP_SET;
	sSync.m_dwValue = pTong->JX2_GetMemberField(pCmd->m_dwMemberNameID, pCmd->m_wKey);
	g_TongServer.BroadcastPackage(&sSync, sizeof(sSync));
}

void JX2_ProcRight(CTongConnect* pConn, const void* pData)
{
	STONG_JX2_RIGHT_COMMAND* pCmd = (STONG_JX2_RIGHT_COMMAND*)pData;
	CTongControl* pTong = g_cTongSet.JX2_FindByNameID(pCmd->m_dwTongNameID);
	if (!pTong)
		return;

	if (!pTong->JX2_ChangeRight(pCmd->m_dwMemberNameID, pCmd->m_dwRightID, pCmd->m_btAdd))
		return;

	CTongControl::JX2Member* pMem = pTong->JX2_GetMember(pCmd->m_dwMemberNameID, FALSE);
	if (pMem)
	{
		char szName[64];
		if (pTong->JX2_GetMemberName(pCmd->m_dwMemberNameID, szName, sizeof(szName)))
			g_cTongDB.JX2_UpdateMemberTail(szName, pMem);
	}

	STONG_JX2_RIGHT_COMMAND sSync = *pCmd;
	sSync.ProtocolFamily = pf_tong;
	sSync.ProtocolID = enumS2C_TONG_JX2_RIGHT_SYNC;
	g_TongServer.BroadcastPackage(&sSync, sizeof(sSync));
}

void JX2_ProcGetFull(CTongConnect* pConn)
{
	g_cTongSet.JX2_SendFullDump(pConn);
}

//////////////////////////////////////////////////////////////////////
// DOT 2 - bang cap, chuoi, so ghi chep, thao tac tong hop, bo hen gio
//////////////////////////////////////////////////////////////////////

// Bang cap bang (settings\tong\tong_level_data.txt). Thieu file thi dung
// 6 dong mac dinh trich tu JX2 (BANGHOI_JX2_PHANTICH.md 5.1).
struct JX2LevelRow
{
	int nWsUpperLv;		// tran cap tac phuong
	int nMaxWsNum;		// so tac phuong toi da
	int nWeekBuildUpper;// tran dong gop kien thiet / tuan
	int nUpgradeFund;	// quy kien thiet can de len cap KE TIEP
	int nUpWsNum;		// so tac phuong can co
	int nUpHiWsNum;		// so tac phuong cap cao can co
	int nUpHiWsLv;		// nguong "cap cao"
};
#define defJX2_MAX_LEVEL	64
static JX2LevelRow s_sJX2Level[defJX2_MAX_LEVEL];
static int s_nJX2LevelCount = 0;

static void sJX2_LoadLevelTable()
{
	if (s_nJX2LevelCount > 0)
		return;
	// mac dinh 6 dong dau theo JX2
	static const int nDef[6][8] = {
		{0, 0, 0, 3360, 6720, 0, 0, 0},
		{1, 3, 6, 3360, 21600, 3, 3, 3},
		{2, 6, 6, 6720, 93600, 3, 3, 6},
		{3, 8, 6, 10080, 205200, 4, 3, 8},
		{4, 9, 8, 16800, 16800, 5, 3, 9},
		{5, 10, 8, 25200, 350000, 5, 3, 10},
	};
	int i;
	for (i = 0; i < 6; i++)
	{
		s_sJX2Level[i].nWsUpperLv = nDef[i][1];
		s_sJX2Level[i].nMaxWsNum = nDef[i][2];
		s_sJX2Level[i].nWeekBuildUpper = nDef[i][3];
		s_sJX2Level[i].nUpgradeFund = nDef[i][4];
		s_sJX2Level[i].nUpWsNum = nDef[i][5];
		s_sJX2Level[i].nUpHiWsNum = nDef[i][6];
		s_sJX2Level[i].nUpHiWsLv = nDef[i][7];
	}
	s_nJX2LevelCount = 6;

	FILE* pFile = fopen("settings\\tong\\tong_level_data.txt", "rt");
	if (!pFile)
		return;
	char szLine[512];
	int nCount = 0;
	while (fgets(szLine, sizeof(szLine), pFile))
	{
		int v[8];
		if (szLine[0] == '/' || szLine[0] == '#' || szLine[0] == ';')
			continue;
		if (sscanf(szLine, "%d %d %d %d %d %d %d %d",
			&v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7]) != 8)
			continue;
		int nLevel = v[0];
		if (nLevel < 0 || nLevel >= defJX2_MAX_LEVEL)
			continue;
		s_sJX2Level[nLevel].nWsUpperLv = v[1];
		s_sJX2Level[nLevel].nMaxWsNum = v[2];
		s_sJX2Level[nLevel].nWeekBuildUpper = v[3];
		s_sJX2Level[nLevel].nUpgradeFund = v[4];
		s_sJX2Level[nLevel].nUpWsNum = v[5];
		s_sJX2Level[nLevel].nUpHiWsNum = v[6];
		s_sJX2Level[nLevel].nUpHiWsLv = v[7];
		if (nLevel + 1 > nCount)
			nCount = nLevel + 1;
	}
	fclose(pFile);
	if (nCount > s_nJX2LevelCount)
		s_nJX2LevelCount = nCount;
	rTRACE("[TONGJX2] bang cap bang: %d dong", s_nJX2LevelCount);
}

// Phat lai anh chup 1 bang (TONG_SYNC + toan bo MEMBER_SYNC) toi MOI GameServer.
// LUU Y: TONG_SYNC lam GS xoa danh sach thanh vien nen LUON phai kem MEMBER_SYNC.
static void sJX2_BroadcastTong(CTongControl* pTong)
{
	if (!pTong)
		return;
	BYTE byBuffer[4096];
	int nMemberTotal = pTong->JX2_CollectMembers(NULL, 0);
	int nBytes = pTong->JX2_BuildTongSync(byBuffer, sizeof(byBuffer), nMemberTotal);
	if (nBytes > 0)
		g_TongServer.BroadcastPackage(byBuffer, nBytes);
	if (nMemberTotal > 0)
	{
		std::vector<JX2MemberBrief> vecMember(nMemberTotal);
		int nGot = pTong->JX2_CollectMembers(&vecMember[0], nMemberTotal);
		int nStart = 0;
		while (nStart < nGot)
		{
			nBytes = pTong->JX2_BuildMemberSync(byBuffer, sizeof(byBuffer),
				&vecMember[0], nGot, &nStart, 4);
			if (nBytes <= 0)
				break;
			g_TongServer.BroadcastPackage(byBuffer, nBytes);
		}
	}
}

// Phat BE_CHANGED_CAMP cho tung thanh vien online sau khi doi phe kieu JX2.
// Sao dung khuon DBChangeCamp (KTongControl.cpp:2728-2820) tru phan tien JX1:
// thieu goi nay thi KPlayerTong::BeChangedCamp phia GS khong chay -> Npc camp
// (server lan client) giu phe cu -> mau ten tren dau nhan vat khong doi.
static void sJX2_SendCampSyncTo(const char* pszRole, int nCamp)
{
	if (!pszRole || !pszRole[0])
		return;
	CNetConnectDup conndup;
	CNetConnectDup tongconndup;
	DWORD nameid = 0;
	unsigned long param = 0;
	if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(pszRole), &conndup, NULL, &nameid, &param))
	{
		tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
		if (tongconndup.IsValid())
		{
			STONG_BE_CHANGED_CAMP_SYNC	sSync;
			sSync.ProtocolFamily = pf_tong;
			sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_CAMP;
			sSync.m_dwParam = param;
			sSync.m_btCamp = (BYTE)nCamp;
			tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
		}
		tongconndup.Clearup();
	}
	conndup.Clearup();
}

void CTongControl::JX2_BroadcastCampSync()
{
	int i;
	sJX2_SendCampSyncTo(m_szMasterName, m_nCamp);
	for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
	{
		if (m_dwDirectorID[i] == 0)
			break;
		sJX2_SendCampSyncTo(m_szDirectorName[i], m_nCamp);
	}
	for (i = 0; i < defTONG_MAX_MANAGER; i++)
	{
		if (m_dwManagerID[i] == 0)
			break;
		sJX2_SendCampSyncTo(m_szManagerName[i], m_nCamp);
	}
	if (m_psMember && m_nMemberPointSize > 0)
	{
		for (i = 0; i < m_nMemberPointSize; i++)
		{
			if (m_psMember[i].m_dwNameID == 0)
				break;
			sJX2_SendCampSyncTo(m_psMember[i].m_szName, m_nCamp);
		}
	}
}

BOOL CTongControl::JX2_SetString(int nKind, const char* pszText)

{
	if (!pszText)
		return FALSE;
	switch (nKind)
	{
	case defTONG_JX2_STR_ANNOUNCE:
		memset(m_szJX2Announce, 0, sizeof(m_szJX2Announce));
		strncpy(m_szJX2Announce, pszText, defTONG_JX2_ANNOUNCE_LEN - 1);
		return TRUE;
	case defTONG_JX2_STR_RECRUIT:
		memset(m_szJX2Recruit, 0, sizeof(m_szJX2Recruit));
		strncpy(m_szJX2Recruit, pszText, defTONG_JX2_ANNOUNCE_LEN - 1);
		return TRUE;
	case defTONG_JX2_STR_EVENT:
		memset(m_szJX2Event[m_nJX2EventHead], 0, defTONG_JX2_RECORD_LEN);
		strncpy(m_szJX2Event[m_nJX2EventHead], pszText, defTONG_JX2_RECORD_LEN - 1);
		m_nJX2EventHead = (m_nJX2EventHead + 1) % defTONG_JX2_RECORD_NUM;
		return TRUE;
	case defTONG_JX2_STR_HISTORY:
		memset(m_szJX2History[m_nJX2HistoryHead], 0, defTONG_JX2_RECORD_LEN);
		strncpy(m_szJX2History[m_nJX2HistoryHead], pszText, defTONG_JX2_RECORD_LEN - 1);
		m_nJX2HistoryHead = (m_nJX2HistoryHead + 1) % defTONG_JX2_RECORD_NUM;
		return TRUE;
	}
	return FALSE;
}

// Duoi thanh vien theo NameID. Dung luat JX2: chi duoi duoc doi truong (manager)
// va bang chung (member) - khong duoi bang chu / truong lao.
BOOL CTongControl::JX2_KickByNameID(DWORD dwNameID)
{
	if (dwNameID == 0 || dwNameID == m_dwMasterID)
		return FALSE;
	int i;
	for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
	{
		if (m_dwDirectorID[i] == dwNameID)
			return FALSE;	// truong lao khong duoi duoc (JX2: cam Figure 0 va 1)
	}
	char szName[64];
	szName[0] = 0;
	BOOL bFound = FALSE;
	for (i = 0; i < defTONG_MAX_MANAGER; i++)
	{
		if (m_dwManagerID[i] == dwNameID)
		{
			strncpy(szName, m_szManagerName[i], 63);
			szName[63] = 0;
			m_szManagerName[i][0] = 0;
			m_dwManagerID[i] = 0;
			m_nManagerNum--;
			bFound = TRUE;
			break;
		}
	}
	if (!bFound && m_psMember)
	{
		for (i = 0; i < m_nMemberPointSize; i++)
		{
			if (m_psMember[i].m_dwNameID == dwNameID)
			{
				strncpy(szName, m_psMember[i].m_szName, 63);
				szName[63] = 0;
				m_psMember[i].m_szName[0] = 0;
				m_psMember[i].m_dwNameID = 0;
				m_nMemberNum--;
				bFound = TRUE;
				break;
			}
		}
	}
	if (!bFound)
		return FALSE;

	m_mapJX2Member.erase(dwNameID);
	if (szName[0])
	{
		g_cTongDB.DelMember(szName);
		// bao cho nguoi bi duoi neu dang online (di qua GS cua ho)
		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(szName), &conndup, NULL, &nameid, &param))
		{
			CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
			{
				STONG_BE_KICKED_SYNC sSync;
				sSync.ProtocolFamily = pf_tong;
				sSync.ProtocolID = enumS2C_TONG_BE_KICKED;
				sSync.m_dwParam = param;
				sSync.m_btFigure = enumTONG_FIGURE_MEMBER;
				sSync.m_btPos = 0;
				strcpy(sSync.m_szName, szName);
				tongconndup.SendPackage((const void*)&sSync, sizeof(sSync));
			}
			tongconndup.Clearup();
			conndup.Clearup();
		}
	}
	return TRUE;
}

// Doi chuc vu thanh vien (thu tuc SetFigure JX2 0x81abb..):
// - khong dung duoc voi bang chu (chuyen nhuong di duong rieng)
// - dich 1 = truong lao (tran 7), 2 = doi truong (tran 56), 3 = bang chung
// - JX2 4.5: ha khoi truong lao la XOA SACH danh sach quyen da giao
BOOL CTongControl::JX2_SetFigureByNameID(DWORD dwNameID, int nNewFigure)
{
	if (dwNameID == 0 || dwNameID == m_dwMasterID)
		return FALSE;
	if (nNewFigure < 1 || nNewFigure > 3)
		return FALSE;

	char szName[64];
	szName[0] = 0;
	int nSex = 0;
	int nOldFigure = -1;
	int i;

	// tim + go khoi cho cu
	for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
	{
		if (m_dwDirectorID[i] == dwNameID)
		{
			strncpy(szName, m_szDirectorName[i], 63);
			szName[63] = 0;
			nSex = m_nDirectorSex[i];
			nOldFigure = 1;
			break;
		}
	}
	if (nOldFigure < 0)
	{
		for (i = 0; i < defTONG_MAX_MANAGER; i++)
		{
			if (m_dwManagerID[i] == dwNameID)
			{
				strncpy(szName, m_szManagerName[i], 63);
				szName[63] = 0;
				nSex = m_nManagerSex[i];
				nOldFigure = 2;
				break;
			}
		}
	}
	if (nOldFigure < 0 && m_psMember)
	{
		for (i = 0; i < m_nMemberPointSize; i++)
		{
			if (m_psMember[i].m_dwNameID == dwNameID)
			{
				strncpy(szName, m_psMember[i].m_szName, 63);
				szName[63] = 0;
				nSex = m_psMember[i].m_nSex;
				nOldFigure = 3;
				break;
			}
		}
	}
	if (nOldFigure < 0 || nOldFigure == nNewFigure || !szName[0])
		return FALSE;

	// kiem cho trong o dich TRUOC khi go cho cu
	int nDest = -1;
	if (nNewFigure == 1)
	{
		for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
		{
			if (m_dwDirectorID[i] == 0)
			{
				nDest = i;
				break;
			}
		}
		if (nDest < 0)
			return FALSE;	// du 7 truong lao
	}
	else if (nNewFigure == 2)
	{
		for (i = 0; i < defTONG_MAX_MANAGER; i++)
		{
			if (m_dwManagerID[i] == 0)
			{
				nDest = i;
				break;
			}
		}
		if (nDest < 0)
			return FALSE;	// du 56 doi truong
	}

	// go khoi cho cu
	if (nOldFigure == 1)
	{
		for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
		{
			if (m_dwDirectorID[i] == dwNameID)
			{
				m_szDirectorName[i][0] = 0;
				m_dwDirectorID[i] = 0;
				m_nDirectorSex[i] = 0;
				m_nDirectorNum--;
				break;
			}
		}
		// JX2 4.5: roi ghe truong lao -> mat trang moi quyen
		JX2Member* pMem = JX2_GetMember(dwNameID, FALSE);
		if (pMem)
		{
			pMem->nRightCount = 0;
			memset(pMem->dwRight, 0, sizeof(pMem->dwRight));
			g_cTongDB.JX2_UpdateMemberTail(szName, pMem);
		}
	}
	else if (nOldFigure == 2)
	{
		for (i = 0; i < defTONG_MAX_MANAGER; i++)
		{
			if (m_dwManagerID[i] == dwNameID)
			{
				m_szManagerName[i][0] = 0;
				m_dwManagerID[i] = 0;
				m_nManagerSex[i] = 0;
				m_nManagerNum--;
				break;
			}
		}
	}
	else if (m_psMember)
	{
		for (i = 0; i < m_nMemberPointSize; i++)
		{
			if (m_psMember[i].m_dwNameID == dwNameID)
			{
				m_psMember[i].m_szName[0] = 0;
				m_psMember[i].m_dwNameID = 0;
				m_nMemberNum--;
				break;
			}
		}
	}

	// dat vao cho moi
	int nClass;
	int nTitleIdx = 0;
	if (nNewFigure == 1)
	{
		strncpy(m_szDirectorName[nDest], szName, defTONG_STR_LENGTH - 1);
		m_szDirectorName[nDest][defTONG_STR_LENGTH - 1] = 0;
		m_dwDirectorID[nDest] = dwNameID;
		m_nDirectorSex[nDest] = nSex;
		m_nDirectorNum++;
		nClass = enumTONG_FIGURE_DIRECTOR;
		nTitleIdx = nDest;
	}
	else if (nNewFigure == 2)
	{
		strncpy(m_szManagerName[nDest], szName, defTONG_STR_LENGTH - 1);
		m_szManagerName[nDest][defTONG_STR_LENGTH - 1] = 0;
		m_dwManagerID[nDest] = dwNameID;
		m_nManagerSex[nDest] = nSex;
		m_nManagerNum++;
		nClass = enumTONG_FIGURE_MANAGER;
		nTitleIdx = nDest;
	}
	else
	{
		AddMember(szName, nSex, false);
		nClass = enumTONG_FIGURE_MEMBER;
	}

	g_cTongDB.JX2_UpdateMemberClass(szName, nClass, nTitleIdx);
	return TRUE;
}

// dem tac phuong: attr0 (ton tai) / dem theo cap >= nMinLevel
static int sJX2_CountWorkshop(CTongControl* pTong, int nMinLevel)
{
	int nCount = 0;
	for (int t = 1; t <= defTONG_JX2_WS_MAX_TYPE; t++)
	{
		WORD wBase = (WORD)(defTONG_JX2_WS_ATTR_BASE + t * 10);
		if (pTong->JX2_GetField(wBase) == 0)
			continue;
		if (nMinLevel <= 0 || (int)pTong->JX2_GetField((WORD)(wBase + 2)) >= nMinLevel)
			nCount++;
	}
	return nCount;
}

// Nang cap bang: kiem quy kien thiet (field 12) + so tac phuong theo bang cap;
// du thi tru quy, field 13 + 1, cap nhat tran tuan (field 42).
BOOL CTongControl::JX2_Upgrade()
{
	sJX2_LoadLevelTable();
	int nLevel = (int)JX2_GetField(13);
	int nNext = nLevel + 1;
	if (nNext >= s_nJX2LevelCount)
		return FALSE;
	JX2LevelRow* pRow = &s_sJX2Level[nLevel];	// dieu kien ghi o dong cap HIEN TAI
	if ((int)JX2_GetField(12) < pRow->nUpgradeFund)
		return FALSE;
	if (sJX2_CountWorkshop(this, 0) < pRow->nUpWsNum)
		return FALSE;
	if (sJX2_CountWorkshop(this, pRow->nUpHiWsLv) < pRow->nUpHiWsNum)
		return FALSE;
	JX2_AddField(12, -pRow->nUpgradeFund, FALSE);
	JX2_SetField(13, (DWORD)nNext);
	JX2_SetField(42, (DWORD)s_sJX2Level[nNext].nWeekBuildUpper);
	return TRUE;
}

BOOL CTongControl::JX2_Degrade()
{
	sJX2_LoadLevelTable();
	int nLevel = (int)JX2_GetField(13);
	if (nLevel <= 0)
		return FALSE;
	nLevel--;
	JX2_SetField(13, (DWORD)nLevel);
	if (nLevel < s_nJX2LevelCount)
		JX2_SetField(42, (DWORD)s_sJX2Level[nLevel].nWeekBuildUpper);
	return TRUE;
}

// Bao tri TUAN (JX2 tong.lua:106-131): don WeekGoal (22-28) -> LWeekGoal (29-35),
// xoa tuan moi; thanh vien: khoa 9 -> 10, 11 -> 12 roi xoa 9/11; Week + 1.
void CTongControl::JX2_WeeklyMaintain(int nTodayDay)
{
	static const WORD wWeek[7] = {22, 23, 24, 25, 26, 27, 28};
	static const WORD wLast[7] = {29, 30, 31, 32, 33, 34, 35};
	int i;
	for (i = 0; i < 7; i++)
	{
		JX2_SetField(wLast[i], JX2_GetField(wWeek[i]));
		JX2_SetField(wWeek[i], 0);
	}
	JX2_SetField(36, 0);		// CurWeekGoalLevel
	JX2_SetField(41, 0);		// WeekBuildFund
	JX2_AddField(21, 1, TRUE);	// Week + 1
	JX2_SetField(defTONGTSK_LAST_WM_DAY, (DWORD)nTodayDay);

	// don so lieu tuan cua tung thanh vien (engine JX2 tu lam - 2.2)
	std::map<DWORD, JX2Member>::iterator it;
	for (it = m_mapJX2Member.begin(); it != m_mapJX2Member.end(); ++it)
	{
		DWORD dwID = it->first;
		JX2_SetMemberField(dwID, 10, JX2_GetMemberField(dwID, 9));
		JX2_SetMemberField(dwID, 12, JX2_GetMemberField(dwID, 11));
		JX2_SetMemberField(dwID, 9, 0);
		JX2_SetMemberField(dwID, 11, 0);
		char szName[64];
		if (JX2_GetMemberName(dwID, szName, sizeof(szName)))
		{
			JX2Member* pMem = JX2_GetMember(dwID, FALSE);
			if (pMem)
				g_cTongDB.JX2_UpdateMemberTail(szName, pMem);
		}
	}
}

// Bao tri NGAY (JX2 tong.lua:321-460, dang C++):
// 1) tru chi phi duy tri (field 16) khoi ngan sach chien bi (field 15)
// 2) tinh tro cap: field 17 = chi phi * 7 / so thanh vien (khong tinh an si)
// 3) bao tri tuyet ky: du 6000 chien bi thi giu, thieu thi tat
// 4) sang tuan (thu Hai / ngay 0 / du 7 ngay) thi chay bao tri tuan
// 5) Day (field 20) + 1
void CTongControl::JX2_DailyMaintain(int nTodayDay, int nWeekday)
{
	int nConsume = (int)JX2_GetField(16);
	if (nConsume > 0)
	{
		int nWar = (int)JX2_GetField(15);
		nWar -= nConsume;
		if (nWar < 0)
			nWar = 0;
		JX2_SetField(15, (DWORD)nWar);
		JX2_AddField(defTONGTSK_WEEK_WFCONSUME, nConsume, FALSE);
	}
	int nMembers = 0;
	{
		std::map<DWORD, JX2Member>::iterator it;
		nMembers = (int)m_mapJX2Member.size();
		if (nMembers <= 0)
			nMembers = m_nMemberNum > 0 ? m_nMemberNum : 1;
	}
	if (nMembers > 0)
		JX2_SetField(17, (DWORD)(nConsume * 7 / nMembers));

	if (JX2_GetField(defTONGTSK_STUNT_ID) != 0)
	{
		int nWar = (int)JX2_GetField(15);
		if (nWar >= 6000)
		{
			JX2_SetField(15, (DWORD)(nWar - 6000));
			JX2_SetField(defTONGTSK_STUNT_ENABLED, 1);
		}
		else
			JX2_SetField(defTONGTSK_STUNT_ENABLED, 0);
	}

	int nLastWeekly = (int)JX2_GetField(defTONGTSK_LAST_WM_DAY);
	if (JX2_GetField(20) == 0 || nWeekday == 1 || nTodayDay - nLastWeekly >= 7)
	{
		if (nLastWeekly != nTodayDay)
			JX2_WeeklyMaintain(nTodayDay);
	}

	JX2_SetField(defTONGTSK_LAST_M_DAY, (DWORD)nTodayDay);
	JX2_AddField(20, 1, TRUE);	// Day + 1
}

// Quy bang -> kinh nghiem bang (JX2 [MoneyToExp], chu ky 750s - 5.4).
// Cau hinh doc tu relay_config.ini muc [tongjx2], mac dinh theo so do JX2.
void CTongControl::JX2_MoneyToExpTick()
{
	static int nMoneyLimit = -1;
	static int nNormalMoney, nNormalExp, nMemberLimit, nMemberLimitMoney, nLimitMoney, nLimitExp;
	if (nMoneyLimit < 0)
	{
		nMoneyLimit = gGetPrivateProfileIntEx("tongjx2", "MoneyLimit", "relay_config.ini", 1000000);
		nNormalMoney = gGetPrivateProfileIntEx("tongjx2", "NormalMoney", "relay_config.ini", 5000);
		nNormalExp = gGetPrivateProfileIntEx("tongjx2", "NormalExp", "relay_config.ini", 120);
		nMemberLimit = gGetPrivateProfileIntEx("tongjx2", "NormalMemberLimit", "relay_config.ini", 100);
		nMemberLimitMoney = gGetPrivateProfileIntEx("tongjx2", "NormalMemberLimitMoney", "relay_config.ini", 50);
		nLimitMoney = gGetPrivateProfileIntEx("tongjx2", "LimitMoney", "relay_config.ini", 0);
		nLimitExp = gGetPrivateProfileIntEx("tongjx2", "LimitExp", "relay_config.ini", 0);
	}
	__int64 nMoney = JX2_GetMoney64();
	if (nMoney <= 0)
		return;
	int nExp = (int)JX2_GetField(6);
	int nMembers = (int)m_mapJX2Member.size();
	if (nMoney > nMoneyLimit)
	{
		__int64 nDec = nNormalMoney;
		if (nMembers > nMemberLimit)
			nDec += (__int64)nMemberLimitMoney * (nMembers - nMemberLimit);
		nMoney -= nDec;
		if (nMoney < 0)
			nMoney = 0;
		nExp += nNormalExp;
	}
	else
	{
		nMoney -= nLimitMoney;
		if (nMoney < 0)
			nMoney = 0;
		nExp -= nLimitExp;
		if (nExp < 0)
			nExp = 0;
	}
	JX2_SetMoney64(nMoney);
	JX2_SetField(6, (DWORD)nExp);
}

// Phan phoi cong hien (JX2 6.4)
BOOL CTongControl::JX2_Distribute(int nOpCode, DWORD dwMemberNameID, int nOffer, int nFigure)
{
	if (nOffer <= 0)
		return FALSE;
	switch (nOpCode)
	{
	case defTONG_JX2_TOP_CONTRIBUTE:
		JX2_AddField(18, nOffer, FALSE);
		return TRUE;
	case defTONG_JX2_TOP_DIST_MEMBER:
		{
			if ((int)JX2_GetField(18) < nOffer)
				return FALSE;
			if (!JX2_GetMember(dwMemberNameID, FALSE))
				return FALSE;
			JX2_AddField(18, -nOffer, FALSE);
			JX2_AddMemberField(dwMemberNameID, 7, nOffer, FALSE);
			char szName[64];
			if (JX2_GetMemberName(dwMemberNameID, szName, sizeof(szName)))
			{
				JX2Member* pMem = JX2_GetMember(dwMemberNameID, FALSE);
				if (pMem)
					g_cTongDB.JX2_UpdateMemberTail(szName, pMem);
			}
			return TRUE;
		}
	case defTONG_JX2_TOP_DIST_GROUP:
	case defTONG_JX2_TOP_ADD_OFFER_FIG:
		{
			// gom danh sach thanh vien thuoc nhom chuc vu
			int nTotal = JX2_CollectMembers(NULL, 0);
			if (nTotal <= 0)
				return FALSE;
			std::vector<JX2MemberBrief> vec(nTotal);
			int nGot = JX2_CollectMembers(&vec[0], nTotal);
			int nInGroup = 0;
			int i;
			for (i = 0; i < nGot; i++)
			{
				if ((int)vec[i].btFigure == nFigure)
					nInGroup++;
			}
			if (nInGroup <= 0)
				return FALSE;
			if (nOpCode == defTONG_JX2_TOP_DIST_GROUP)
			{
				int nNeed = nOffer * nInGroup;
				if ((int)JX2_GetField(18) < nNeed)
					return FALSE;
				JX2_AddField(18, -nNeed, FALSE);
			}
			for (i = 0; i < nGot; i++)
			{
				if ((int)vec[i].btFigure != nFigure)
					continue;
				JX2_AddMemberField(vec[i].dwNameID, 7, nOffer, FALSE);
				JX2Member* pMem = JX2_GetMember(vec[i].dwNameID, FALSE);
				if (pMem)
					g_cTongDB.JX2_UpdateMemberTail(vec[i].szName, pMem);
			}
			return TRUE;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Proc goi dot 2
//////////////////////////////////////////////////////////////////////

void JX2_ProcString(CTongConnect* pConn, const void* pData)
{
	STONG_JX2_STRING_COMMAND* pCmd = (STONG_JX2_STRING_COMMAND*)pData;
	CTongControl* pTong = g_cTongSet.JX2_FindByNameID(pCmd->m_dwTongNameID);
	if (!pTong)
		return;
	char szText[defTONG_JX2_ANNOUNCE_LEN];
	memcpy(szText, pCmd->m_szText, sizeof(szText));
	szText[defTONG_JX2_ANNOUNCE_LEN - 1] = 0;
	if (!pTong->JX2_SetString(pCmd->m_btKind, szText))
		return;
	g_cTongDB.ChangeTong(*pTong);
	// dong bo MOI loai chuoi toi GS (announce/recruit ghi de, event/history append ring)
	{
		STONG_JX2_STRING_COMMAND sSync = *pCmd;
		sSync.ProtocolFamily = pf_tong;
		sSync.ProtocolID = enumS2C_TONG_JX2_STRING_SYNC;
		g_TongServer.BroadcastPackage(&sSync, sizeof(sSync));
	}
}

void JX2_ProcTongOp(CTongConnect* pConn, const void* pData)
{
	STONG_JX2_TONG_OP_COMMAND* pCmd = (STONG_JX2_TONG_OP_COMMAND*)pData;
	CTongControl* pTong = g_cTongSet.JX2_FindByNameID(pCmd->m_dwTongNameID);
	if (!pTong)
		return;

	BOOL bOK = FALSE;
	BOOL bMembersChanged = FALSE;
	switch (pCmd->m_btOpCode)
	{
	case defTONG_JX2_TOP_INIT:
		pTong->JX2_Reset();
		bOK = TRUE;
		bMembersChanged = TRUE;
		break;
	case defTONG_JX2_TOP_UPGRADE:
		bOK = pTong->JX2_Upgrade();
		break;
	case defTONG_JX2_TOP_DEGRADE:
		bOK = pTong->JX2_Degrade();
		break;
	case defTONG_JX2_TOP_MAINTAIN:
	case defTONG_JX2_TOP_WEEKLY:
		{
			time_t tNow = time(NULL);
			struct tm* pTm = localtime(&tNow);
			struct tm sMid = *pTm;
			sMid.tm_hour = 0; sMid.tm_min = 0; sMid.tm_sec = 0;
			int nToday = (int)(mktime(&sMid) / 86400);
			if (pCmd->m_btOpCode == defTONG_JX2_TOP_MAINTAIN)
				pTong->JX2_DailyMaintain(nToday, pTm->tm_wday);
			else
				pTong->JX2_WeeklyMaintain(nToday);
			bOK = TRUE;
			bMembersChanged = TRUE;
		}
		break;
	case defTONG_JX2_TOP_KICK:
		bOK = pTong->JX2_KickByNameID(pCmd->m_dwMemberNameID);
		bMembersChanged = bOK;
		break;
	case defTONG_JX2_TOP_SET_STUNT:
		pTong->JX2_SetField(defTONGTSK_STUNT_ID, (DWORD)pCmd->m_nParam1);
		pTong->JX2_SetField(defTONGTSK_STUNT_ENABLED, pCmd->m_nParam1 ? 1 : 0);
		bOK = TRUE;
		break;
	case defTONG_JX2_TOP_SET_MAP:
		pTong->JX2_SetField(45, (DWORD)pCmd->m_nParam1);
		bOK = TRUE;
		break;
	case defTONG_JX2_TOP_CREATE_MAP:
		pTong->JX2_SetField(45, (DWORD)pCmd->m_nParam1);
		pTong->JX2_SetField(46, (DWORD)pCmd->m_nParam1);
		bOK = TRUE;
		break;
	case defTONG_JX2_TOP_DELETE_MAP:
		pTong->JX2_SetField(45, 0);
		pTong->JX2_SetField(46, 0);
		bOK = TRUE;
		break;
	case defTONG_JX2_TOP_CONTRIBUTE:
	case defTONG_JX2_TOP_DIST_GROUP:
	case defTONG_JX2_TOP_DIST_MEMBER:
	case defTONG_JX2_TOP_ADD_OFFER_FIG:
		bOK = pTong->JX2_Distribute(pCmd->m_btOpCode, pCmd->m_dwMemberNameID,
			pCmd->m_nParam1, pCmd->m_nParam2);
		bMembersChanged = bOK && pCmd->m_btOpCode != defTONG_JX2_TOP_CONTRIBUTE;
		break;
	case defTONG_JX2_TOP_SET_FIGURE:
		bOK = pTong->JX2_SetFigureByNameID(pCmd->m_dwMemberNameID, pCmd->m_nParam1);
		bMembersChanged = bOK;
		break;
	case defTONG_JX2_TOP_SET_CAMP:
		// Doi phe kieu JX2: GameServer da kiem quyen va TRU NGAN QUY JX2 roi,
		// o day chi ghi phe. KHONG di qua CTongSet::ChangeCamp vi ham do kiem
		// tui tien JX1 m_dwMoney (luon 0 trong he JX2) nen se tu choi.
		if (pCmd->m_nParam1 >= 1 && pCmd->m_nParam1 <= 3)
		{
			pTong->JX2_SetCamp(pCmd->m_nParam1);
			pTong->JX2_BroadcastCampSync();
			bOK = TRUE;
		}
		break;
	case defTONG_JX2_TOP_FEATURE:
		// doi ngoai hinh toan bang: can duong ve client - lam o giai doan cua so client
		rTRACE("[TONGJX2] ChangeAllMemberFeature(%s, %d, %d) - ghi nhan, chua doi hinh",
			pTong->JX2_Name(), pCmd->m_nParam1, pCmd->m_nParam2);
		bOK = TRUE;
		break;
	default:
		return;
	}

	if (!bOK)
		return;
	g_cTongDB.ChangeTong(*pTong);
	sJX2_BroadcastTong(pTong);
	if (bMembersChanged)
	{
		// sJX2_BroadcastTong da gui kem danh sach thanh vien
	}
}

// Tick 30 giay tu WM_TIMER cua cua so chinh relay (don luong - an toan voi g_cTongSet)
void JX2_TimerTick()
{
	static time_t s_tLastM2E = 0;
	static int s_nM2EInterval = -1;
	if (s_nM2EInterval < 0)
		s_nM2EInterval = gGetPrivateProfileIntEx("tongjx2", "TimeLong", "relay_config.ini", 750);

	sJX2_LoadLevelTable();

	time_t tNow = time(NULL);
	struct tm* pTm = localtime(&tNow);
	struct tm sMid = *pTm;
	sMid.tm_hour = 0; sMid.tm_min = 0; sMid.tm_sec = 0;
	int nToday = (int)(mktime(&sMid) / 86400);
	int nWeekday = pTm->tm_wday;	// 1 = thu Hai

	BOOL bM2E = FALSE;
	if (s_tLastM2E == 0)
		s_tLastM2E = tNow;	// khong chay ngay khi vua bat relay
	else if (tNow - s_tLastM2E >= s_nM2EInterval)
	{
		bM2E = TRUE;
		s_tLastM2E = tNow;
	}

	int nMaintained = 0;
	for (int i = 0; ; i++)
	{
		CTongControl* pTong = g_cTongSet.JX2_GetByIndex(i);
		if (pTong == (CTongControl*)-1)
			break;	// het mang
		if (!pTong || !pTong->JX2_IsValid())
			continue;

		BOOL bChanged = FALSE;
		if ((int)pTong->JX2_GetField(defTONGTSK_LAST_M_DAY) != nToday)
		{
			pTong->JX2_DailyMaintain(nToday, nWeekday);
			bChanged = TRUE;
			nMaintained++;
		}
		if (bM2E)
		{
			pTong->JX2_MoneyToExpTick();
			bChanged = TRUE;
		}
		if (bChanged)
		{
			g_cTongDB.ChangeTong(*pTong);
			sJX2_BroadcastTong(pTong);
		}
	}
	if (nMaintained > 0)
		rTRACE("[TONGJX2] bao tri ngay cho %d bang (ngay %d)", nMaintained, nToday);
}
