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

//////////////////////////////////////////////////////////////////////
// CTongControl - phan du lieu JX2
//////////////////////////////////////////////////////////////////////

void CTongControl::JX2_Reset()
{
	m_nJX2FieldCount = 0;
	memset(m_wJX2FieldKey, 0, sizeof(m_wJX2FieldKey));
	memset(m_dwJX2FieldVal, 0, sizeof(m_dwJX2FieldVal));
	m_mapJX2Member.clear();
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
	// Thu ban sao ket noi trong khoa doc, gui ngoai khoa (theo mau FindTongConnectByIP)
	std::vector<CNetConnectDup> vecConn;
	{
		AUTOLOCKREAD(m_lockIpMap);
		for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); ++it)
		{
			CTongConnect* pTongConn = it->second;
			if (pTongConn)
				vecConn.push_back(CNetConnectDup(*pTongConn));
		}
	}
	int nSent = 0;
	for (size_t i = 0; i < vecConn.size(); i++)
	{
		if (vecConn[i].IsValid())
		{
			vecConn[i].SendPackage(pData, nSize);
			nSent++;
		}
		vecConn[i].Clearup();
	}
	return nSent;
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
			nNew = 0;
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
