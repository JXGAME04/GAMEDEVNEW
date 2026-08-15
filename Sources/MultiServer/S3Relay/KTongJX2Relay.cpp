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
	{
		// FIX 14/08: nhanh khong dau TRUOC DAY khong kep am -> tru qua so du
		// wrap thanh ~4,29 ty. Khong phai loi ly thuyet: script goc goi so am
		// cho chinh 3 field nay moi ngay (scriptjx2\tong_vn\tong.lua:152 quy
		// kien thiet, :274 va :380 quy chien bi, :517 chi phi lap tac phuong)
		// nen bang nao co quy nho hon phi duy tri se bong thanh 4,29 ty.
		// Van giu duoc pham vi khong dau khi CONG (den 4 ty), chi kep khi TRU.
		if (nDelta < 0)
		{
			DWORD dwSub = (DWORD)(-nDelta);
			dwNew = (dwCur > dwSub) ? (dwCur - dwSub) : 0;
		}
		else
			dwNew = dwCur + (DWORD)nDelta;
	}
	else
	{
		// kep am: tru qua so du -> 0, khong de wrap thanh DWORD khong lo
		int nNew = (int)dwCur + nDelta;
		if (nNew < 0)
			nNew = 0;
		dwNew = (DWORD)nNew;
	}
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
	{
		// kep am: tru qua so du -> 0, khong de wrap thanh DWORD khong lo
		int nNew = (int)dwCur + nDelta;
		if (nNew < 0)
			nNew = 0;
		dwNew = (DWORD)nNew;
	}
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
	if (m_psMember)
	for (i = 0; i < m_nMemberPointSize; i++)
	{
		// 2A 15/08: khong duoc dung o TRONG dau tien - chinh viec thang chuc
		// tao ra lo o giua mang (JX2_SetFigureByNameID xoa o cu roi AddMember
		// vao o khac), nen 'break' lam danh sach thanh vien CUT tu do tro di.
		if (m_psMember[i].m_dwNameID == 0)
			continue;
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

// khai bao truoc: dump-on-connect dung truoc khi dinh nghia
static void sJX2_BroadcastZhaoMu(CTongControl* pTong, DWORD dwParam,
	CNetConnect* pOnly = NULL);	// pOnly: chi gui 1 GS (dump-on-connect)

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

// tim bang theo TEN (m_pcTong la private nen phai la method)
CTongControl* CTongSet::JX2_FindByName(const char* szTongName)
{
	if (!szTongName || !szTongName[0] || !m_pcTong || m_nTongPointSize <= 0)
		return NULL;
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && strcmp(m_pcTong[i]->JX2_Name(), szTongName) == 0)
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
			if (!pTong || !pTong->JX2_Name()[0])
				continue;

			int nMemberTotal = pTong->JX2_CollectMembers(NULL, 0);
			int nBytes = pTong->JX2_BuildTongSync(byBuffer, sizeof(byBuffer), nMemberTotal);
			if (nBytes > 0)
				pConn->SendPackage(byBuffer, nBytes);
			// hang doi don xin vao bang (phat cho MOI GS - goi broadcast
			// nho gon; GS khac nhan trung cung vo hai vi la anh chup day du)
			sJX2_BroadcastZhaoMu(pTong, 0, pConn);	// D11: chi GS vua noi

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
			if (pTong->JX2_LeagueName()[0])
			{
				// ten lien minh: goi STRING kind UNION (GS giu vao szUnionName)
				STONG_JX2_STRING_COMMAND sStr;
				memset(&sStr, 0, sizeof(sStr));
				sStr.ProtocolFamily = pf_tong;
				sStr.ProtocolID = enumS2C_TONG_JX2_STRING_SYNC;
				sStr.m_dwTongNameID = pTong->m_dwNameID;
				sStr.m_btKind = defTONG_JX2_STR_UNION;
				strncpy(sStr.m_szText, pTong->JX2_LeagueName(), sizeof(sStr.m_szText) - 1);
				pConn->SendPackage(&sStr, sizeof(sStr));
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

// Bang cap TAC PHUONG (settings\tong\workshop\workshop_level_data.txt):
// LEVEL / UPGRADE_FUND / MAINTAIN_FUND / OPEN_FUND. He so tung khu lay tu
// workshops.txt cot COEFFICIENT (Binh giap 1.4 / Thien Cong 1.0 / Mat na
// 1.2 / Luyen tap 1.0 / Thien y 0.8 / Le vat 1.0 / Hoat dong 0.6).
#define defJX2_MAX_WSLEVEL	16
static int s_nJX2WsUpFund[defJX2_MAX_WSLEVEL];
static int s_nJX2WsOpenFund[defJX2_MAX_WSLEVEL];
static int s_nJX2WsLevelCount = 0;
// x100 de tinh nguyen: 140 = 1.4
static const int s_nJX2WsCoef[8] = {0, 140, 100, 120, 100, 80, 100, 60};

static void sJX2_LoadWsLevelTable()
{
	if (s_nJX2WsLevelCount > 0)
		return;
	// mac dinh theo bang JX2 (dung khi thieu file)
	static const int nDef[11][3] = {
		{0, 400, 0}, {1, 800, 80}, {2, 1200, 160}, {3, 1920, 240},
		{4, 2400, 384}, {5, 2880, 480}, {6, 6000, 576}, {7, 12000, 1200},
		{8, 14400, 240}, {9, 30000, 2880}, {10, 2000000000, 6000},
	};
	int i;
	for (i = 0; i < 11; i++)
	{
		s_nJX2WsUpFund[i] = nDef[i][1];
		s_nJX2WsOpenFund[i] = nDef[i][2];
	}
	s_nJX2WsLevelCount = 11;
	FILE* pFile = fopen("settings\\tong\\workshop\\workshop_level_data.txt", "rt");
	if (!pFile)
		return;
	char szLine[512];
	while (fgets(szLine, sizeof(szLine), pFile))
	{
		int nLv, nUp, nOpen;
		float fMaintain;
		if (sscanf(szLine, "%d %d %f %d", &nLv, &nUp, &fMaintain, &nOpen) != 4)
			continue;
		if (nLv < 0 || nLv >= defJX2_MAX_WSLEVEL)
			continue;
		s_nJX2WsUpFund[nLv] = nUp;
		s_nJX2WsOpenFund[nLv] = nOpen;
		if (nLv + 1 > s_nJX2WsLevelCount)
			s_nJX2WsLevelCount = nLv + 1;
	}
	fclose(pFile);
}

// chi phi = floor(UPGRADE_FUND[cap dang co] * he so khu) - wsGetUpgradeCostFund
static int sJX2_WsUpgradeCost(int nType, int nCurLevel)
{
	sJX2_LoadWsLevelTable();
	if (nType < 1 || nType > 7 || nCurLevel < 0 || nCurLevel >= s_nJX2WsLevelCount)
		return -1;
	return (int)(((__int64)s_nJX2WsUpFund[nCurLevel] * s_nJX2WsCoef[nType]) / 100);
}

static int sJX2_WsOpenCost(int nType, int nCurLevel)
{
	sJX2_LoadWsLevelTable();
	if (nType < 1 || nType > 7 || nCurLevel < 0 || nCurLevel >= s_nJX2WsLevelCount)
		return -1;
	return (int)(((__int64)s_nJX2WsOpenFund[nCurLevel] * s_nJX2WsCoef[nType]) / 100);
}

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

// DOT9 #26: hai truong nay von duoc NAP ma KHONG AI DOC - do la ly do
// "khong dem so khu" va "khong co tran cap".
static int sJX2_WsUpperLevel(int nBuildLevel)
{
	sJX2_LoadLevelTable();
	if (nBuildLevel < 0)
		nBuildLevel = 0;
	if (nBuildLevel >= s_nJX2LevelCount)
		nBuildLevel = s_nJX2LevelCount - 1;
	return s_sJX2Level[nBuildLevel].nWsUpperLv;
}

static int sJX2_WsMaxNum(int nBuildLevel)
{
	sJX2_LoadLevelTable();
	if (nBuildLevel < 0)
		nBuildLevel = 0;
	if (nBuildLevel >= s_nJX2LevelCount)
		nBuildLevel = s_nJX2LevelCount - 1;
	return s_sJX2Level[nBuildLevel].nMaxWsNum;
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

// Phat ten lien minh hien tai (m_szLeagueTName) toi moi GS bang goi STRING
// kind UNION - dung chung duong voi announce/recruit.
void CTongControl::JX2_SendUnionNameSync()
{
	STONG_JX2_STRING_COMMAND sSync;
	memset(&sSync, 0, sizeof(sSync));
	sSync.ProtocolFamily = pf_tong;
	sSync.ProtocolID = enumS2C_TONG_JX2_STRING_SYNC;
	sSync.m_dwTongNameID = m_dwNameID;
	sSync.m_btKind = defTONG_JX2_STR_UNION;
	strncpy(sSync.m_szText, m_szLeagueTName, sizeof(sSync.m_szText) - 1);
	g_TongServer.BroadcastPackage(&sSync, sizeof(sSync));
}

BOOL CTongControl::JX2_SetString(int nKind, const char* pszText)
{
	if (!pszText)
		return FALSE;
	switch (nKind)
	{
	case defTONG_JX2_STR_UNION:
		strncpy(m_szLeagueTName, pszText, sizeof(m_szLeagueTName) - 1);
		m_szLeagueTName[sizeof(m_szLeagueTName) - 1] = 0;
		return TRUE;
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
// 2C 15/08: khai bao truoc - than ham nam duoi (sJX2_SayTong)
static void sJX2_SayTong(CTongControl* pTong, const char* pszMsg);

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
		{
			// 2C: nguyen van tong_mix.lua:646
			sJX2_SayTong(this, "S\350 l\255\356ng tr\255\353ng l\267o \256\267 \256\271t gi\355i h\271n!");
			return FALSE;
		}
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
		{
			// 2C: nguyen van tong_mix.lua:653
			sJX2_SayTong(this, "S\350 l\255\356ng \256\351i tr\255\353ng \256\267 \256\271t gi\355i h\271n!");
			return FALSE;
		}
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

	// 2C 15/08: bao THANH CONG - van goc relay stringtable_relay.txt:98-102
	{
		char szOk[160];
		if (nClass == enumTONG_FIGURE_DIRECTOR)
			sprintf(szOk, "%s \256\255\356c nh\313n ch\370c v\364 Tr\255\353ng l\267o!", szName);
		else if (nClass == enumTONG_FIGURE_MANAGER)
			sprintf(szOk, "%s \256\255\356c \256\266m nh\313n l\265 nh\343m tr\255\353ng", szName);
		else
			sprintf(szOk, "%s tr\353 v\322 l\265m bang ch\370ng", szName);
		sJX2_SayTong(this, szOk);
	}

	// 2A 15/08: bao NGAY cho nguoi bi doi chuc vu - truoc day khong gui gi nen
	// nhan/quyen JX1 cua ho giu gia tri cu toi khi dang nhap lai (chu game bao
	// "doi xong phai out ra vao lai moi cap nhat"). Duong nay da chay that o
	// KTongControl.cpp:653-690; GameServer nhan o KSOServer.cpp:1522 ->
	// KPlayerTong::BeInstated -> SendSelfInfo.
	// CHU Y: gui nClass (he JX1: MEMBER=0..MASTER=3) chu KHONG phai nNewFigure
	// (he JX2 dem nguoc: 0=bang chu..3=bang chung). Gui nham = leo quyen.
	if (szName[0])
	{
		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(szName), &conndup, NULL, &nameid, &param))
		{
			CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
			{
				STONG_BE_INSTATED_SYNC sSync;
				memset(&sSync, 0, sizeof(sSync));
				sSync.ProtocolFamily = pf_tong;
				sSync.ProtocolID = enumS2C_TONG_BE_INSTATED;
				sSync.m_dwParam = param;
				sSync.m_btFigure = (BYTE)nClass;
				sSync.m_btPos = (BYTE)nTitleIdx;
				strncpy(sSync.m_szName, szName, sizeof(sSync.m_szName) - 1);
				// lay title y nhu GetLoginData (KTongControl.cpp:1876-1914)
				switch (nClass)
				{
				case enumTONG_FIGURE_DIRECTOR:
					GetDirectorTitle(sSync.m_szTitle, nTitleIdx);
					break;
				case enumTONG_FIGURE_MANAGER:
					GetManagerTitle(sSync.m_szTitle, nTitleIdx);
					break;
				default:
					GetMemberTitle(sSync.m_szTitle, nSex);
					break;
				}
				tongconndup.SendPackage((const void*)&sSync, sizeof(sSync));
			}
			tongconndup.Clearup();
			conndup.Clearup();
		}
	}
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
	char szWhy[192];
	// 2C 15/08: truoc day moi nhanh chi 'return FALSE' im lang, ma Core da tra 0
	// tu truoc nen nguoi bam luon thay 'da thuc hien' = BAO THANH CONG GIA.
	// Chuoi nguyen van UPGRADE_G_1 (scriptjx2\tong_vn\tong.lua:570-589).
	if (nNext >= s_nJX2LevelCount)
	{
		sJX2_SayTong(this, "\247\274ng c\312p ki\325n thi\325t \256\267 \256\271t \256\325n c\312p cao nh\312t!");
		return FALSE;
	}
	JX2LevelRow* pRow = &s_sJX2Level[nLevel];	// dieu kien ghi o dong cap HIEN TAI
	if ((int)JX2_GetField(12) < pRow->nUpgradeFund)
	{
		sprintf(szWhy, "\247\274ng c\312p ki\325n thi\325t n\251ng l\252n c\312p ti\325p theo c\312n ng\251n s\270ch ki\325n thi\325t: %d v\271n (\256ang c\343 %d)",
			pRow->nUpgradeFund, (int)JX2_GetField(12));
		sJX2_SayTong(this, szWhy);
		return FALSE;
	}
	if (sJX2_CountWorkshop(this, 0) < pRow->nUpWsNum)
	{
		sprintf(szWhy, "\247\323 n\251ng l\252n c\312p k\325 ti\325p c\312n ph\266i x\251y d\371ng %d T\270c Ph\255\352ng (\256ang c\343 %d)",
			pRow->nUpWsNum, sJX2_CountWorkshop(this, 0));
		sJX2_SayTong(this, szWhy);
		return FALSE;
	}
	if (sJX2_CountWorkshop(this, pRow->nUpHiWsLv) < pRow->nUpHiWsNum)
	{
		sprintf(szWhy, "\247\274ng c\312p ki\325n thi\325t n\251ng l\252n c\312p ti\325p theo c\312n %d T\270c Ph\255\352ng \256\271t c\312p %d (\256ang c\343 %d)",
			pRow->nUpHiWsNum, pRow->nUpHiWsLv, sJX2_CountWorkshop(this, pRow->nUpHiWsLv));
		sJX2_SayTong(this, szWhy);
		return FALSE;
	}
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
	// Snapshot muc tieu tuan 22-28 -> 29-35 + co hoan thanh + CHON MUC TIEU
	// MOI: gio do WEEKLY_MAINTAIN_R cua tong.lua lam (ma goc nguyen van,
	// GS goi sang thu Hai). Relay chi giu phan "engine tu lam": don thanh
	// vien + reset quy tuan + dem tuan/ngay.
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
	// Tru chi phi duy tri + tro cap (17) + tam ngung: GIO do CHINH LUA GOC
	// lam (tong.lua MAINTAIN_R chay tren GS, dot C) - relay khong tru nua
	// de khoi TRU DOI. Tuyet ky 6000/ngay cung bo: ban goc thu theo TUAN
	// trong Maintain_Stunt, se lam dung o dot tuyet ky.

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

// Liet ke cac bang co field 10 == dwUnionID (lien minh la TRANG THAI DAN XUAT
// tu field cua tung bang: 10 = UnionID, 50 = co minh chu, 49 = gio roi;
// ten luu o m_szLeagueTName - deu da nam trong TTongStruct nen khong can DB moi).
int CTongSet::JX2_UnionMembers(DWORD dwUnionID, CTongControl** ppOut, int nMax)
{
	int nCount = 0;
	if (!dwUnionID || !m_pcTong)
		return 0;
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		CTongControl* pT = m_pcTong[i];
		if (!pT || !pT->JX2_Name()[0])
			continue;
		if (pT->JX2_GetField(10) != dwUnionID)
			continue;
		if (ppOut && nCount < nMax)
			ppOut[nCount] = pT;
		nCount++;
	}
	return nCount;
}

// Noi tren kenh chat cua mot bang (nhu DBChangeCamp lam voi doi phe)
static void sJX2_SayTong(CTongControl* pTong, const char* pszMsg)
{
	if (!pTong || !pszMsg || !pszMsg[0])
		return;
	char szCh[96];
	sprintf(szCh, "\\O%u", pTong->JX2_NameID());
	DWORD dwChann = g_ChannelMgr.GetChannelID(szCh, 0);
	if (dwChann != (DWORD)-1)
		g_ChannelMgr.SayOnChannel(dwChann, TRUE, std::string(),
			std::string(defTONG_NAME_SAY_ON_CHANNEL), std::string(pszMsg));
}

// [LevelUnionNum] tongset.ini ban goc (doc byte tho :206-307):
// cap bang minh chu 0-9 -> 3 bang, 10-29 -> 4, 30-49 -> 5, 50-69 -> 6,
// 70-89 -> 7, 90+ -> 8.
static int sJX2_UnionCap(int nBuildLevel)
{
	// [LevelUnionNum] goc chay theo cap bang 0-100 (3..8 bang); he ta chi
	// co cap KIEN THIET JX2 0-5 (tong_level_data) dang van hanh -> anh xa
	// giu dung khoang gia tri goc: cap 0->3 ... cap 5->8.
	int nCap = 3 + nBuildLevel;
	if (nCap < 3) nCap = 3;
	if (nCap > 8) nCap = 8;
	return nCap;
}

// Go mot bang khoi lien minh: xoa field 10, dong dau field 49 = now (luat
// cho 3 ngay cua ban goc - GS kiem), xoa ten, luu DB + phat lai cho GS.
static void sJX2_UnionDetach(CTongControl* pT)
{
	pT->JX2_SetField(10, 0);
	pT->JX2_SetField(50, 0);
	pT->JX2_SetField(49, (DWORD)time(NULL));
	pT->JX2_LeagueName()[0] = 0;
	g_cTongDB.ChangeTong(*pT);
	sJX2_BroadcastTong(pT);
	pT->JX2_SendUnionNameSync();
}

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

// Phat hang doi don cua MOT bang toi moi GameServer (chia goi 16 don).
// dwParam = chi so nguoi bam o GS goc (PushViewTo day nguoc trang).
static void sJX2_BroadcastZhaoMu(CTongControl* pTong, DWORD dwParam,
	CNetConnect* pOnly)
{
	if (!pTong)
		return;
	static TZhaoMuStruct sList[64];
	int nTotal = g_cTongDB.GetZhaoMuList(pTong->JX2_Name(), sList, 64);
	int nSent = 0;
	do
	{
		STONG_JX2_ZHAOMU_SYNC sSync;
		memset(&sSync, 0, sizeof(sSync));
		sSync.ProtocolFamily = pf_tong;
		sSync.ProtocolID = enumS2C_TONG_JX2_ZHAOMU_SYNC;
		sSync.m_dwTongNameID = pTong->JX2_NameID();
		sSync.m_wStart = (WORD)nSent;
		sSync.m_wTotal = (WORD)nTotal;
		sSync.m_dwParam = dwParam;
		int nThis = nTotal - nSent;
		if (nThis > defTONG_JX2_ZM_PER_PACKET)
			nThis = defTONG_JX2_ZM_PER_PACKET;
		for (int r = 0; r < nThis; r++)
		{
			strncpy(sSync.m_sRec[r].szName, sList[nSent + r].szName, 31);
			sSync.m_sRec[r].dwNameID = sList[nSent + r].dwNameID;
			sSync.m_sRec[r].wLevel = sList[nSent + r].wLevel;
			sSync.m_sRec[r].btSex = sList[nSent + r].btSex;
		}
		sSync.m_btCount = (BYTE)nThis;
		int nLen = (int)(sizeof(sSync) - sizeof(sSync.m_sRec)
			+ nThis * sizeof(STONG_JX2_ONE_APPLY_REC));
		sSync.m_wLength = (WORD)nLen;
		if (pOnly)
			pOnly->SendPackage((PBYTE)&sSync, nLen);
		else
			g_TongServer.BroadcastPackage(&sSync, nLen);
		nSent += nThis;
	} while (nSent < nTotal);
}

void JX2_ProcZhaoMu(CTongConnect* pConn, const void* pData)
{
	STONG_JX2_ZHAOMU_COMMAND* pCmd = (STONG_JX2_ZHAOMU_COMMAND*)pData;
	CTongControl* pTong = g_cTongSet.JX2_FindByNameID(pCmd->m_dwTongNameID);
	if (!pTong)
		return;
	char szOldTong[32];
	szOldTong[0] = 0;
	{
		// D2: mot nguoi chi co MOT don toan cuc (khoa = ten) - phai biet
		// don cu thuoc bang nao de (a) DEL khong xoa nham don bang khac,
		// (b) ADD thay don thi phat lai hang doi cua BANG CU (het ma).
		TZhaoMuStruct sOld;
		if (g_cTongDB.GetZhaoMu(pCmd->m_sRec.szName, &sOld))
			strncpy(szOldTong, sOld.szTong, 31);
		szOldTong[31] = 0;
	}
	if (pCmd->m_btOp == defTONG_JX2_ZM_DEL &&
		szOldTong[0] && strcmp(szOldTong, pTong->JX2_Name()) != 0)
		return;	// don dang thuoc bang khac - bang nay khong duoc xoa
	if (pCmd->m_btOp == defTONG_JX2_ZM_ADD)
	{
		TZhaoMuStruct sRec;
		memset(&sRec, 0, sizeof(sRec));
		strncpy(sRec.szTong, pTong->JX2_Name(), 31);
		strncpy(sRec.szName, pCmd->m_sRec.szName, 31);
		sRec.dwNameID = pCmd->m_sRec.dwNameID;
		sRec.wLevel = pCmd->m_sRec.wLevel;
		sRec.btSex = pCmd->m_sRec.btSex;
		sRec.dwApplyTime = (unsigned long)time(NULL);
		g_cTongDB.ChangeZhaoMu(&sRec);	// khoa = ten -> tu thay don cu
	}
	else if (pCmd->m_btOp == defTONG_JX2_ZM_DEL)
		g_cTongDB.DelZhaoMu(pCmd->m_sRec.szName);
	sJX2_BroadcastZhaoMu(pTong, pCmd->m_dwParam);
	if (pCmd->m_btOp == defTONG_JX2_ZM_ADD &&
		szOldTong[0] && strcmp(szOldTong, pTong->JX2_Name()) != 0)
	{
		// don cu bi thay: bang cu cung phai duoc cap nhat (het don ma)
		CTongControl* pOld = g_cTongSet.JX2_FindByName(szOldTong);
		if (pOld)
			sJX2_BroadcastZhaoMu(pOld, 0);
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
			else if ((int)pTong->JX2_GetField(defTONGTSK_LAST_WM_DAY) != nToday)
				// PHAN BIEN D2: khong dedup thi thu Hai don tuan HAI lan (tick
				// relay 00h0x + TOP_WEEKLY tu Lua 06h05) -> thong ke tuan truoc
				// cua thanh vien (khoa 10/12) bi de bang 6 gio dau tuan nay.
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
			{
				// ghi NHAT KY su kien (chu game bao doi phe khong thay hien):
				// append ring + echo STRING_SYNC ve GS y nhu JX2_ProcString
				// DUNG thu tu bang client UiTongJX2.cpp:1571: 1=Chinh 2=Ta 3=Trung lap
				static const char* szCampNm[4] = {"?",
					"Ch\335nh ph\270i", "T\265 ph\270i", "Trung l\313p"};
				char szEv[96];
				sprintf(szEv, "Bang h\351i \256\267 \256\346i sang phe %s",
					szCampNm[pCmd->m_nParam1]);
				pTong->JX2_SetString(defTONG_JX2_STR_EVENT, szEv);
				STONG_JX2_STRING_COMMAND sSync;
				memset(&sSync, 0, sizeof(sSync));
				sSync.ProtocolFamily = pf_tong;
				sSync.ProtocolID = enumS2C_TONG_JX2_STRING_SYNC;
				sSync.m_dwTongNameID = pCmd->m_dwTongNameID;
				sSync.m_btKind = defTONG_JX2_STR_EVENT;
				strncpy(sSync.m_szText, szEv, sizeof(sSync.m_szText) - 1);
				g_TongServer.BroadcastPackage(&sSync, sizeof(sSync));
			}
			bOK = TRUE;
		}
		break;
	case defTONG_JX2_TOP_UNION_CREATE:
		{
			// R1 ban goc (s3relay_y 0x80d4d70): ten <= 31 byte; chua o lien
			// minh; khong trung ten bang/lien minh khac; MIEN PHI.
			char szNm[32];
			strncpy(szNm, pCmd->m_szName, sizeof(szNm) - 1);
			szNm[sizeof(szNm) - 1] = 0;
			if (!szNm[0] || pTong->JX2_GetField(10) != 0)
				break;
			DWORD dwUid = g_String2Id(szNm);
			if (dwUid == 0 || g_cTongSet.JX2_FindByNameID(dwUid) ||
				g_cTongSet.JX2_UnionMembers(dwUid, NULL, 0) > 0)
			{
				// MSG_TONG_CREATE_UNION_ERROR5
				sJX2_SayTong(pTong, "\247\267 c\343 bang h\351i ho\306c li\252n minh bang h\351i tr\357ng t\252n n\265y! ");
				break;
			}
			pTong->JX2_SetField(10, dwUid);
			pTong->JX2_SetField(50, 1);
			strncpy(pTong->JX2_LeagueName(), szNm, 31);
			pTong->JX2_LeagueName()[31] = 0;
			{
				// G_TONG_CREATE_UNION (stringtable_relay.txt:93)
				char szMsg[160];
				sprintf(szMsg, "Bang ch\361 B\346n bang l\313p n\252n m\351t li\252n minh bang h\351i %s! ", szNm);
				sJX2_SayTong(pTong, szMsg);
			}
			pTong->JX2_SendUnionNameSync();
			bOK = TRUE;
		}
		break;
	case defTONG_JX2_TOP_UNION_JOIN:
		{
			// R2 ban goc (0x80d4ef8): pTong = bang MINH CHU duyet; m_szName =
			// bang xin vao. Tran so bang theo cap minh chu; ngan quy MOI ben
			// phai > 999999 luong va tru 100 van (1.000.000 luong) MOI ben.
			if (!pTong->JX2_GetField(50) || !pTong->JX2_GetField(10))
				break;
			CTongControl* pB = g_cTongSet.JX2_FindByNameID(g_String2Id(pCmd->m_szName));
			if (!pB || pB == pTong)
			{
				sJX2_SayTong(pTong, "Kh\253ng t\327m th\312y bang n\265y!");
				break;
			}
			if (pB->JX2_GetField(10) != 0)
			{
				// G_PLAYERTONG_12
				sJX2_SayTong(pTong, "Th\265nh vi\252n c\361a li\252n minh bang h\351i n\265y kh\351ng th\323 gia nh\313p li\252n minh bang h\351i kh\270c");
				break;
			}
			{
				DWORD dwT49 = pB->JX2_GetField(49);
				if (dwT49 && time(NULL) - (time_t)dwT49 <= 259199)
				{
					char szMsg[160];
					sprintf(szMsg, "Bang h\351i %s r\352i li\252n minh ch\255a \256\361 3 ng\265y!", pB->JX2_Name());
					sJX2_SayTong(pTong, szMsg);
					break;
				}
			}
			DWORD dwUid = pTong->JX2_GetField(10);
			if (pB->JX2_GetField(54) != dwUid)
			{
				// bang B CHUA nop don xin vao lien minh nay - khong duoc ep
				char szMsg[160];
				sprintf(szMsg, "Bang h\351i %s ch\255a xin gia nh\313p li\252n minh c\361a b\346n bang!", pB->JX2_Name());
				sJX2_SayTong(pTong, szMsg);
				break;
			}
			if (g_cTongSet.JX2_UnionMembers(dwUid, NULL, 0) >= sJX2_UnionCap((int)pTong->JX2_GetField(13)))
			{
				sJX2_SayTong(pTong, "Li\252n minh \256\267 \256\361 s\350 bang theo c\312p bang minh ch\361!");
				break;
			}
			if (pTong->JX2_GetMoney64() <= 999999)
			{
				sJX2_SayTong(pTong, "Li\252n minh th\312t b\271i! Ng\251n qu\374 bang h\351i li\252n minh kh\253ng \256\361!");
				break;
			}
			if (pB->JX2_GetMoney64() <= 999999)
			{
				sJX2_SayTong(pTong, "Li\252n minh th\312t b\271i! Ng\251n qu\374 bang h\351i mu\350n li\252n minh kh\253ng \256\361!");
				break;
			}
			pTong->JX2_SetMoney64(pTong->JX2_GetMoney64() - 1000000);
			pB->JX2_SetMoney64(pB->JX2_GetMoney64() - 1000000);
			pB->JX2_SetField(10, dwUid);
			pB->JX2_SetField(54, 0);	// don da duoc duyet - xoa
			strncpy(pB->JX2_LeagueName(), pTong->JX2_LeagueName(), 31);
			pB->JX2_LeagueName()[31] = 0;
			{
				char szMsg[220];
				CTongControl* apM[16];
				int nM = g_cTongSet.JX2_UnionMembers(dwUid, apM, 16);
				int q;
				// G_TONG_JOIN_SELFUNION toi cac bang thanh vien khac
				sprintf(szMsg, "Bang h\351i %s gia nh\313p b\346n bang c\343 li\252n minh %s!", pB->JX2_Name(), pTong->JX2_LeagueName());
				for (q = 0; q < nM && q < 16; q++)
					if (apM[q] != pTong && apM[q] != pB)
						sJX2_SayTong(apM[q], szMsg);
				// G_TONG_JOIN_SELFUNION_MASTER (%d = 100 van)
				sprintf(szMsg, "Bang h\351i %s gia nh\313p b\346n bang c\343 li\252n minh %s, b\346n bang b\336 kh\312u tr\365 %d v\271n trong ng\251n qu\374!", pB->JX2_Name(), pTong->JX2_LeagueName(), 100);
				sJX2_SayTong(pTong, szMsg);
				// G_TONG_JOIN_UNION
				sprintf(szMsg, "B\346n bang ti\252u hao %d v\271n ng\251n qu\374, gia nh\313p li\252n minh %s!", 100, pTong->JX2_LeagueName());
				sJX2_SayTong(pB, szMsg);
			}
			g_cTongDB.ChangeTong(*pB);
			sJX2_BroadcastTong(pB);
			pB->JX2_SendUnionNameSync();
			bOK = TRUE;	// duoi ham luu + phat pTong (da tru tien)
		}
		break;
	case defTONG_JX2_TOP_UNION_LEAVE:
		{
			DWORD dwUid = pTong->JX2_GetField(10);
			if (!dwUid)
				break;
			if (pTong->JX2_GetField(50))
			{
				// minh chu rut = GIAI TAN (R3 subop 2 + tip nut ban goc)
				CTongControl* apM[16];
				int nM = g_cTongSet.JX2_UnionMembers(dwUid, apM, 16);
				for (int q = 0; q < nM && q < 16; q++)
				{
					if (apM[q] == pTong)
						continue;
					// G_TONG_DISMISS_UNION
					sJX2_SayTong(apM[q], "Minh ch\361 gi\266i t\270n li\252n minh bang h\351i!");
					sJX2_UnionDetach(apM[q]);
				}
				sJX2_SayTong(pTong, "Minh ch\361 gi\266i t\270n li\252n minh bang h\351i!");
			}
			else
			{
				// bao cac bang con lai: G_TONG_LEAVE_UNION_ELSE
				char szMsg[160];
				CTongControl* apM[16];
				int nM = g_cTongSet.JX2_UnionMembers(dwUid, apM, 16);
				sprintf(szMsg, "Bang h\351i %s r\352i kh\341i li\252n minh c\361a b\346n bang!", pTong->JX2_Name());
				for (int q = 0; q < nM && q < 16; q++)
					if (apM[q] != pTong)
						sJX2_SayTong(apM[q], szMsg);
				// G_TONG_LEAVE_UNION (dau cach cuoi nguyen van)
				sJX2_SayTong(pTong, "B\346n bang r\352i kh\341i bang h\351i li\252n minh! ");
			}
			pTong->JX2_SetField(10, 0);
			pTong->JX2_SetField(50, 0);
			pTong->JX2_SetField(49, (DWORD)time(NULL));
			pTong->JX2_LeagueName()[0] = 0;
			pTong->JX2_SendUnionNameSync();
			bOK = TRUE;
		}
		break;
	case defTONG_JX2_TOP_UNION_KICK:
		{
			if (!pTong->JX2_GetField(50))
				break;
			CTongControl* pB = g_cTongSet.JX2_FindByNameID(g_String2Id(pCmd->m_szName));
			if (!pB || pB == pTong || pB->JX2_GetField(10) != pTong->JX2_GetField(10))
			{
				sJX2_SayTong(pTong, "Kh\253ng t\327m th\312y bang n\265y trong li\252n minh!");
				break;
			}
			{
				char szMsg[160];
				CTongControl* apM[16];
				int nM = g_cTongSet.JX2_UnionMembers(pTong->JX2_GetField(10), apM, 16);
				// G_TONG_KICK_UNION_ELSE
				sprintf(szMsg, "Minh ch\361 \256u\346i bang h\351i %s ra kh\341i li\252n minh!", pB->JX2_Name());
				for (int q = 0; q < nM && q < 16; q++)
					if (apM[q] != pB)
						sJX2_SayTong(apM[q], szMsg);
			}
			// ban VN cua G_TONG_KICK_UNION con nguyen tieng Trung -> tu dich
			sJX2_SayTong(pB, "Minh ch\361 tr\364c xu\312t b\346n bang kh\341i li\252n minh!");
			sJX2_UnionDetach(pB);
			bOK = TRUE;
		}
		break;
	case defTONG_JX2_TOP_WS_OP:
		{
			// 4 dieu kien ban goc (workshop_logic.lua logicWorkShopLevelUp /
			// logicWorkShopOpen / logicWorkShopLearn) + TRU QUY KIEN THIET that.
			// Truoc day GS ghi thang field cap: mien phi, khong tran, vo han.
			int nType = pCmd->m_nParam1;
			int nAct = pCmd->m_nParam2;
			if (nType < 1 || nType > defTONG_JX2_WS_MAX_TYPE)
				break;
			WORD wBase = (WORD)(defTONG_JX2_WS_ATTR_BASE + nType * 10);
			int nBuildLv = (int)pTong->JX2_GetField(13);
			int nFund = (int)pTong->JX2_GetField(12);
			BOOL bExist = pTong->JX2_GetField(wBase) != 0;
			int nCurLv = (int)pTong->JX2_GetField((WORD)(wBase + 2));
			// dieu kien 3 ban goc: bang dang tam ngung thi cam moi thao tac
			if (pTong->JX2_GetField(44) != 0)
			{
				sJX2_SayTong(pTong, "Bang h\351i \256ang t\271m ng\365ng ho\271t \256\351ng, kh\253ng th\323 thao t\270c t\270c ph\255\352ng!");
				break;
			}
			if (nAct == 0)
			{
				// LAP KHU: chan so khu toi da theo cap kien thiet + tru phi
				if (bExist)
					break;
				if (sJX2_CountWorkshop(pTong, 0) >= sJX2_WsMaxNum(nBuildLv))
				{
					sJX2_SayTong(pTong, "S\350 t\270c ph\255\352ng \256\267 \256\271t gi\355i h\271n theo \256\274ng c\312p ki\325n thi\325t bang!");
					break;
				}
				int nCost = sJX2_WsUpgradeCost(nType, 0);
				if (nCost < 0 || nFund < nCost)
				{
					sJX2_SayTong(pTong, "Ng\251n s\270ch ki\325n thi\325t bang kh\253ng \256\361 \256\323 x\251y t\270c ph\255\352ng n\265y.");
					break;
				}
				pTong->JX2_AddField(12, -nCost, FALSE);
				{
					char szMsg[128];
					sprintf(szMsg, "\247\267 x\251y t\270c ph\255\352ng (chi %d ng\251n s\270ch ki\325n thi\325t)", nCost);
					sJX2_SayTong(pTong, szMsg);
				}
				pTong->JX2_SetField(wBase, 1);
				pTong->JX2_SetField((WORD)(wBase + 2), 1);	// cap 1
				pTong->JX2_SetField((WORD)(wBase + 1), 1);	// mo
				// attr4 = UseLevel: ban goc doi >= 1 moi cho dung khu
				pTong->JX2_SetField((WORD)(wBase + 4), 1);
				bOK = TRUE;
				break;
			}
			if (!bExist)
				break;
			if (nAct == 1 || nAct == 2)
			{
				// MO KHU ton phi OPEN_FUND (ban goc logicWorkShopOpen); DONG mien phi
				if (nAct == 1)
				{
					if (pTong->JX2_GetField((WORD)(wBase + 1)) != 0)
						break;	// dang mo roi
					int nCost = sJX2_WsOpenCost(nType, nCurLv);
					if (nCost < 0 || nFund < nCost)
					{
						sJX2_SayTong(pTong, "Ng\251n s\270ch ki\325n thi\325t bang kh\253ng \256\361 \256\323 m\353 t\270c ph\255\352ng n\265y.");
						break;
					}
					if (nCost > 0)
						pTong->JX2_AddField(12, -nCost, FALSE);
				}
				pTong->JX2_SetField((WORD)(wBase + 1), (nAct == 1) ? 1 : 0);
				sJX2_SayTong(pTong, (nAct == 1) ?
					"\247\267 m\353 t\270c ph\255\352ng." : "\247\267 \256\343ng t\270c ph\255\352ng.");
				bOK = TRUE;
				break;
			}
			if (nAct == 3)
			{
				// NANG CAP: dieu kien 1 = tran cap theo WORKSHOP_UPPER_LEVEL cua
				// cap kien thiet bang; dieu kien 2 = du quy. EP nToLevel =
				// nCurLevel + 1 (ban goc co lo hong chi tru tien 1 cap).
				int nToLv = nCurLv + 1;
				if (nToLv > sJX2_WsUpperLevel(nBuildLv))
				{
					sJX2_SayTong(pTong, "T\270c ph\255\352ng n\265y \256\267 \256\271t \256\325n c\312p cao nh\312t, mu\350n n\251ng c\312p c\307n n\251ng \256\274ng c\312p ki\325n thi\325t tr\255\355c!");
					break;
				}
				int nCost = sJX2_WsUpgradeCost(nType, nCurLv);
				if (nCost < 0 || nFund < nCost)
				{
					sJX2_SayTong(pTong, "Ng\251n s\270ch ki\325n thi\325t bang kh\253ng \256\361, kh\253ng th\323 n\251ng c\312p t\270c ph\255\352ng n\265y.");
					break;
				}
				pTong->JX2_AddField(12, -nCost, FALSE);
				pTong->JX2_SetField((WORD)(wBase + 2), (DWORD)nToLv);
				if ((int)pTong->JX2_GetField((WORD)(wBase + 4)) < nToLv)
					pTong->JX2_SetField((WORD)(wBase + 4), (DWORD)nToLv);
				{
					char szMsg[160];
					sprintf(szMsg, "T\270c ph\255\352ng th\250ng l\252n c\312p %d (chi %d ng\251n s\270ch ki\325n thi\325t)", nToLv, nCost);
					sJX2_SayTong(pTong, szMsg);
				}
				bOK = TRUE;
				break;
			}
		}
		break;
	case defTONG_JX2_TOP_UNION_APPLY:
		// bang nay vua nop don xin vao lien minh nParam1 - luu de minh chu
		// CHI duyet duoc bang da xin (chong ep bang khac vao + tru oan tien)
		pTong->JX2_SetField(54, (DWORD)pCmd->m_nParam1);
		bOK = TRUE;
		break;
	case defTONG_JX2_TOP_MINISTER:
		{
			// dai than quoc gia: field 51/52/53 = NameID nguoi giu chuc
			// (1 Thua Tuong / 2 Nguyen Soai / 3 Tien Phong)
			if (pCmd->m_nParam1 < 1 || pCmd->m_nParam1 > 3)
				break;
			pTong->JX2_SetField((WORD)(50 + pCmd->m_nParam1),
				pCmd->m_nParam2 ? pCmd->m_dwMemberNameID : 0);
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
