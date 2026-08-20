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

// duyet danh sach bang: 0 -> bang dau tien; tra 0 khi het
DWORD KTongJX2Mgr::NextTongID(DWORD dwPrev)
{
	std::map<DWORD, KTongJX2Tong>::iterator it;
	if (dwPrev == 0)
		it = m_mapTong.begin();
	else
	{
		it = m_mapTong.find(dwPrev);
		if (it == m_mapTong.end())
			return 0;
		++it;
	}
	return (it == m_mapTong.end()) ? 0 : it->first;
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
			// VIEC7 15/08: echo field TAC PHUONG ve toi -> day lai trang WS cho
			// nguoi vua bam (y het khuon RIGHT_SYNC ben duoi). Truoc day client
			// xin lai trang NGAY sau khi gui lenh, echo chua ve nen ve lai du
			// lieu CU -> "bam khong thay gi doi". Chi push cho dai khoa WS
			// (20010..20079) de khoi spam khi bao tri ghi hang loat field khac.
			if (pCmd->m_dwParam != 0 &&
				pCmd->m_wKey > defTONG_JX2_WS_ATTR_BASE &&
				pCmd->m_wKey < defTONG_JX2_WS_ATTR_BASE + (defTONG_JX2_WS_MAX_TYPE + 1) * 10)
				PushViewTo(pCmd->m_dwParam, pCmd->m_dwTongNameID,
					defTONG_JX2_PAGE_WS);
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
			// day nguoc trang Thanh vien (mang mat na quyen) cho nguoi vua bam
			PushViewTo(pCmd->m_dwParam, pCmd->m_dwTongNameID,
				defTONG_JX2_PAGE_MEMBER);
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

	case enumS2C_TONG_JX2_ZHAOMU_SYNC:
		{
			STONG_JX2_ZHAOMU_SYNC* pCmd = (STONG_JX2_ZHAOMU_SYNC*)pData;
			if (nSize < (int)(sizeof(STONG_JX2_ZHAOMU_SYNC) - sizeof(pCmd->m_sRec)))
				return;
			KTongJX2Tong* pTong = FindTong(pCmd->m_dwTongNameID);
			if (!pTong)
				return;
			// D3: kiem bien theo khuon MEMBER_SYNC - m_btCount la BYTE, phai
			// khop do dai goi thuc va tran 16 rec/goi
			if (pCmd->m_btCount > defTONG_JX2_ZM_PER_PACKET)
				return;
			if ((int)(sizeof(STONG_JX2_ZHAOMU_SYNC) - sizeof(pCmd->m_sRec)
				+ pCmd->m_btCount * sizeof(STONG_JX2_ONE_APPLY_REC)) > nSize)
				return;
			// goi dau (start 0) = dung lai tu dau; cac goi sau noi tiep
			if (pCmd->m_wStart == 0)
				pTong->btApplyCount = 0;
			for (int r = 0; r < (int)pCmd->m_btCount &&
				pTong->btApplyCount < 64; r++)
			{
				int a = pTong->btApplyCount;
				memcpy(pTong->szApplyName[a], pCmd->m_sRec[r].szName, 32);
				pTong->szApplyName[a][31] = 0;
				pTong->dwApplyID[a] = pCmd->m_sRec[r].dwNameID;
				pTong->wApplyLevel[a] = pCmd->m_sRec[r].wLevel;
				pTong->btApplySex[a] = pCmd->m_sRec[r].btSex;
				pTong->btApplyCount++;
			}
			// het chuoi goi -> day nguoc trang Chieu mo cho nguoi vua thao tac
			if ((int)pCmd->m_wStart + (int)pCmd->m_btCount >= (int)pCmd->m_wTotal)
				PushViewTo(pCmd->m_dwParam, pCmd->m_dwTongNameID,
					defTONG_JX2_PAGE_RECRUIT);
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
				// day nguoc trang Chieu mo cho nguoi vua bam Luu
				PushViewTo(pCmd->m_dwParam, pCmd->m_dwTongNameID,
					defTONG_JX2_PAGE_RECRUIT);
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

// DOT9 #28: sau khi relay chot, day NGUOC trang vua doi cho dung nguoi da
// bam. Client KHONG con phai tu xin lai (xin ngay sau khi gui = luon doc
// duoc chuoi CU vi relay chua kip ghi + ban sao chua kip nhan echo).
// Cau kiem bang: m_dwParam la chi so Player[] CUC BO cua GameServer goc,
// tren GS khac do la nguoi khac - nen bat buoc doi chieu NameID bang.
void KTongJX2Mgr::PushViewTo(DWORD dwPlayerIdx, DWORD dwTongNameID, int nPage)
{
	int nIdx = (int)dwPlayerIdx;
	if (nIdx <= 0 || nIdx >= MAX_PLAYER)
		return;
	if (Player[nIdx].m_nIndex <= 0)
		return;
	if (Player[nIdx].m_cTong.GetTongNameID() != dwTongNameID)
		return;	// nguoi o GS khac / da roi bang
	if (!g_pServer)
		return;
	BYTE byOut[2048];
	int nLen = BuildClientView(nIdx, nPage, 0, byOut, sizeof(byOut));
	if (nLen > 0)
		g_pServer->PackDataToClient(Player[nIdx].m_nNetConnectIdx, byOut, nLen);
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
	// PHAN BIEN D4: ap LAC QUAN vao ban sao ngay - script goc chay tren
	// relay nen check-roi-tru la nguyen tu; ta gui lenh di xa thi lan doc
	// ke tiep trong CUNG khung phai thay gia tri MOI (echo relay van la
	// chan ly, se ghi de sau).
	{
		KTongJX2Tong* pOpt = g_TongJX2.FindTong(dwTongID);
		if (pOpt)
		{
			DWORD dwCur = 0;
			std::map<WORD, DWORD>::iterator itF = pOpt->mapField.find(wKey);
			if (itF != pOpt->mapField.end())
				dwCur = itF->second;
			if (btOp == defTONG_JX2_OP_SET)
				pOpt->mapField[wKey] = dwValue;
			else if (btOp == defTONG_JX2_OP_ADDU)
			{
				// FIX 14/08: ban sao lac quan phai kep am Y HET relay
				// (KTongJX2Relay.cpp:97). Neu khong, client hien ~4,29 ty trong
				// khi relay da kep ve 0 -> hai con so lech nhau ca ngay.
				// dwValue la (DWORD)nDelta nen doc lai dau bang (int).
				int nSub = (int)dwValue;
				if (nSub < 0)
				{
					DWORD dwSub = (DWORD)(-nSub);
					pOpt->mapField[wKey] = (dwCur > dwSub) ? (dwCur - dwSub) : 0;
				}
				else
					pOpt->mapField[wKey] = dwCur + dwValue;
			}
			else
			{
				int nNew = (int)dwCur + (int)dwValue;
				if (nNew < 0)
					nNew = 0;	// khop kep am cua relay
				pOpt->mapField[wKey] = (DWORD)nNew;
			}
		}
	}
	g_NewProtocolProcess.PushMsgInTong((const void*)&sCmd, sizeof(sCmd));
}

// Bao relay them / xoa DON XIN VAO BANG (hang doi song o relay + DB,
// song qua restart GS; relay phat lai ZHAOMU_SYNC cho moi GS).
static void sSendZhaoMu(DWORD dwTongID, BYTE btOp, const char* pszName,
	DWORD dwNameID, WORD wLevel, BYTE btSex, DWORD dwParam)
{
	STONG_JX2_ZHAOMU_COMMAND sCmd;
	memset(&sCmd, 0, sizeof(sCmd));
	sCmd.ProtocolFamily = pf_tong;
	sCmd.ProtocolID = enumC2S_TONG_JX2_ZHAOMU;
	sCmd.m_dwTongNameID = dwTongID;
	sCmd.m_btOp = btOp;
	if (pszName)
		strncpy(sCmd.m_sRec.szName, pszName, sizeof(sCmd.m_sRec.szName) - 1);
	sCmd.m_sRec.dwNameID = dwNameID;
	sCmd.m_sRec.wLevel = wLevel;
	sCmd.m_sRec.btSex = btSex;
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
	if (pTong->btApplyCount >= 64)
		return 5;
	a = pTong->btApplyCount;
	memset(pTong->szApplyName[a], 0, 32);
	strncpy(pTong->szApplyName[a], Player[nPlayerIdx].m_PlayerName, 31);
	pTong->dwApplyID[a] = dwMyID;
	pTong->wApplyLevel[a] = (WORD)nLevel;
	pTong->btApplySex[a] = (BYTE)Npc[Player[nPlayerIdx].m_nIndex].m_nSex;
	pTong->btApplyCount++;
	// day len relay de don SONG QUA RESTART + moi GS deu thay;
	// ban chen cuc bo o tren chi de nguoi bam thay ngay (optimistic),
	// ZHAOMU_SYNC ve se dung lai toan bo replica theo ban chinh.
	sSendZhaoMu(pTong->dwNameID, defTONG_JX2_ZM_ADD,
		pTong->szApplyName[pTong->btApplyCount - 1],
		pTong->dwApplyID[pTong->btApplyCount - 1],
		pTong->wApplyLevel[pTong->btApplyCount - 1],
		pTong->btApplySex[pTong->btApplyCount - 1], 0);
	{
		// ghi so su kien (bang chu doc lai duoc sau khi vao game) + bao ngay
		// cho nhung nguoi dang online co quyen duyet
		char szLog[160];
		sprintf(szLog, "%s xin gia nhËp bang (cÊp %d)",
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
// PHAN BIEN D2: field 20 (Day) / 21 (Week) do RELAY so huu doc quyen
// (JX2_DailyMaintain/WeeklyMaintain) - ban goc tong.lua chay TREN relay
// nen chi cong mot lan; ta chay tren GS ma van gui ADD thi Day +2/ngay,
// Week +3/thu Hai. Hai ham nay thanh no-op co chu dich.
int LuaTONG_ApplyAddDay(Lua_State* L)
{
	Lua_PushNumber(L, 1);
	return 1;
}
int LuaTONG_ApplyAddWeek(Lua_State* L)
{
	Lua_PushNumber(L, 1);
	return 1;
}
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
	// dang goi 2 tham so (npc tong quan: TWS_GetFirstWorkshop(nTongID,
	// eType)) phai ton trong LOAI KHU - truoc day bo qua nen bang co >=2
	// phuong la moi NPC tong quan deu mo khu co so NHO NHAT (mo nham).
	// Dang 1 tham so (tong.lua:346 duyet tat ca) giu nguyen hanh vi cu.
	if (Lua_GetTopIndex(L) >= 2)
	{
		int nWant = (int)Lua_ValueToNumber(L, 2);
		if (nWant >= 1 && nWant <= defTONG_JX2_WS_MAX_TYPE)
		{
			Lua_PushNumber(L,
				g_TongJX2.GetField(dwTongID, sWsAttrField(nWant, 0)) ? nWant : 0);
			return 1;
		}
	}
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

// Duong script tung khu (thu tu TYPE cua workshops.txt; chu thuong vi
// bo nap boot g_StrLower duong dan truoc khi tinh ScriptID).
static const char* s_szWsScriptPath[8] = { "",
	"\\scriptjx2\\tong_vn\\workshop\\ws_bingjia.lua",
	"\\scriptjx2\\tong_vn\\workshop\\ws_tiangong.lua",
	"\\scriptjx2\\tong_vn\\workshop\\ws_mianju.lua",
	"\\scriptjx2\\tong_vn\\workshop\\ws_shilian.lua",
	"\\scriptjx2\\tong_vn\\workshop\\ws_tianyi.lua",
	"\\scriptjx2\\tong_vn\\workshop\\ws_liwu.lua",
	"\\scriptjx2\\tong_vn\\workshop\\ws_huodong.lua",
};

// Bao tri tac phuong: goi MAINTAIN_R(nTongID, nWS) cua DUNG script khu
// (tong.lua MAINTAIN_R goi ham nay cho tung khu - ban goc di qua relay,
// ta chay truc tiep). Truoc day la no-op nen san luong ngay luon 0.
int LuaTWS_ApplyMaintain(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	int nType = sWsType(L);
	if (!g_TongJX2.FindTong(dwTongID) || nType < 1 || nType > defTONG_JX2_WS_MAX_TYPE)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	KLuaScript* pScript = (KLuaScript*)g_GetScript(s_szWsScriptPath[nType]);
	if (!pScript)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nTop = 0;
	pScript->SafeCallBegin(&nTop);
	int nOk = pScript->CallFunction("MAINTAIN_R", 0, "dd", (int)dwTongID, nType) ? 1 : 0;
	pScript->SafeCallEnd(nTop);
	Lua_PushNumber(L, (double)nOk);
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
			// cat lat theo nStart (phan trang 8 don/trang - hang doi 64 don
			// o relay); duoi goi mang wStart/wTotal cho client hien so trang
			if (nStart < 0 || nStart >= (int)pTong->btApplyCount)
				nStart = 0;
			for (int a = nStart; a < (int)pTong->btApplyCount &&
				(int)pSync->m_btApplyCount < defTONG_JX2_APPLY_MAX; a++)
			{
				int w = pSync->m_btApplyCount;
				memcpy(pSync->m_sApply[w].m_szName, pTong->szApplyName[a], 32);
				pSync->m_sApply[w].m_szName[31] = 0;
				pSync->m_sApply[w].m_dwNameID = pTong->dwApplyID[a];
				pSync->m_sApply[w].m_wLevel = pTong->wApplyLevel[a];
				pSync->m_btApplyCount++;
			}
			pSync->m_wApplyStart = (WORD)nStart;
			pSync->m_wApplyTotal = (WORD)pTong->btApplyCount;
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
			// 2B 15/08: BO cau chung "Da thuc hien thao tac bang hoi." - do la
			// chuoi nguoi port tu them, quet toan bo ban goc Linux (.lua + 3 bang
			// chuoi) KHONG co cau tuong duong. Thanh cong thi ban goc hoac im
			// lang hoac co cau RIENG cua tung thao tac.
			if (pCmd->m_btOp == defTONG_JX2_COP_APPLY_JOIN)
				// stringtable_core.txt:942 MSG_TONG_APPLY_ADD
				pszMsg = "B¹n xin phÐp gia nhËp bang héi!";
			break;
		case 1:
			pszMsg = "Gãi tin bang héi kh«ng hîp lÖ.";
			break;
		case 7:
			// stringtable_core.txt:947 MSG_TONG_ADD_SUCCESS
			pszMsg = "B¹n gia nhËp bang héi!";
			break;
		case 2:
			pszMsg = "B¹n ch­a vµo bang héi.";
			break;
		case 3:
			// nguyen van ban Linux (MSG_TONG_NO_OPRIGHT)
			pszMsg = "Kh«ng cã quyÒn h¹n ®Ó thùc hiÖn thao t¸c nµy!";
			break;
		case 4:
			// 2B: tach theo thao tac, nguyen van ban goc
			if (pCmd->m_btOp == defTONG_JX2_COP_ADDRIGHT ||
				pCmd->m_btOp == defTONG_JX2_COP_DELRIGHT)
				// stringtable_core.txt:964 MSG_RIGHT_ASSIGN_NEED_DIRECTOR
				pszMsg = "§èi ph­¬ng kh«ng ph¶i lµ tr­ëng l·o, kh«ng thÓ tiÕp nhËn quyÒn h¹n!";
			else
				// stringtable_core.txt:965 MSG_TONG_TARGET_NOTMEMBER
				pszMsg = "§èi ph­¬ng kh«ng ph¶i lµ thµnh viªn bang héi!";
			break;
		case 5:
			if (pCmd->m_btOp == defTONG_JX2_COP_SET_FIGURE)
				// stringtable_core.txt:937 MSG_TONG_INSTATE_FAIL_ID2 (bo %s vi
				// cho nay khong co san ten nguoi bi bo nhiem)
				pszMsg = "§èi ph­¬ng kh«ng trùc tuyÕn, kh«ng thÓ nhiÖm mÖnh";
			else if (pCmd->m_btOp == defTONG_JX2_COP_GRANT)
				// stringtable_core.txt:46 G_PLAYERTONG_17
				pszMsg = "ChÞ cã thÓ ph¸t tiÒn cho c¸c héi viªn trùc tuyÕn (online)";
			else
				pszMsg = "Thao t¸c kh«ng hîp lÖ hoÆc môc tiªu kh«ng online.";
			break;
		case 6:
			// 2B: nguyen van tong_mix.lua ban goc, tach theo loai quy
			if (pCmd->m_btOp == defTONG_JX2_COP_DONATE)
				// tong_mix.lua:101
				pszMsg = "Kh«ng ®ñ ng©n l­îng!";
			else if (pCmd->m_btOp == defTONG_JX2_COP_GRANT ||
				pCmd->m_btOp == defTONG_JX2_COP_GRANT_GROUP)
				// tong_mix.lua:164
				pszMsg = "Ng©n s¸ch bang héi kh«ng ®ñ";
			else if (pCmd->m_btOp == defTONG_JX2_COP_UPGRADE)
				// tong_mix.lua:197
				pszMsg = "Ng©n s¸ch kiÕn thiÕt bang kh«ng ®ñ!";
			else
				// tong_mix.lua:354
				pszMsg = "Ng©n s¸ch cóng hiÕn kh«ng ®ñ!";
			break;
		case 9:
			// nguyen van ban Linux (workshop_head.lua)
			pszMsg = "T¸c ph­êng nµy ®· cã råi!";
			break;
		case 10:
			pszMsg = "Quý bang d­êng nh­ ch­a x©y dùng T¸c Ph­êng nµy.";
			break;
		case 11:
			// nguyen van ban Linux (MSG_TONG_AUTO_REFUSE_LEVEL)
			pszMsg = "§¼ng cÊp cña ng­¬i qu¸ thÊp, bang héi nµy tõ chèi ®Ò nghÞ gia nhËp cña ng­¬i";
			break;
		case 12:
			pszMsg = "B¹n ®· göi ®¬n xin gia nhËp bang héi nµy råi, xin chê duyÖt.";
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
			// D-4: phat tien + tran ngay bang CHINH LUA GOC (tong_mix.lua):
			// MEMBER_KICK_G_1 nFlag=2 (client da hoi xac nhan AskThenSendOp,
			// tuong ung nhanh "da qua hop thoai" cua ban goc): tran
			// MAX_KICK_COUNT/ngay + kiem du quy; roi MEMBER_KICK_R: tru 60%
			// cong hien vao quy du tru (lan sang kien thiet khi thieu) + 40%
			// vao chien bi + thong bao bang. Loi tu choi da duoc Lua nhan
			// truc tiep cho nguoi bam (Msg2Player) -> return 20.
			{
				KLuaScript* pKick = (KLuaScript*)g_GetScript(
					"\\scriptjx2\\tong_vn\\tong_mix.lua");
				if (pKick)
				{
					// dung bo global nhu KPlayer::ExecuteScript de Msg2Player/
					// Say trong Lua toi dung nguoi bam; ExecutorId = ban goc
					Lua_PushNumber(pKick->m_LuaState, nPlayerIdx);
					pKick->SetGlobalName(SCRIPT_PLAYERINDEX);
					Lua_PushNumber(pKick->m_LuaState,
						Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex);
					pKick->SetGlobalName(SCRIPT_SUBWORLDINDEX);
					Lua_PushNumber(pKick->m_LuaState, (double)pMe->dwNameID);
					lua_setglobal(pKick->m_LuaState, "ExecutorId");
					int nTop = 0;
					int nOk = 0;
					pKick->SafeCallBegin(&nTop);
					if (pKick->CallFunction("MEMBER_KICK_G_1", 1, "ddd",
						(int)dwTongID, (int)pCmd->m_dwTarget, 2))
						nOk = (int)Lua_ValueToNumber(pKick->m_LuaState, -1);
					pKick->SafeCallEnd(nTop);
					if (nOk != 1)
						return 20;
					nOk = 0;
					pKick->SafeCallBegin(&nTop);
					if (pKick->CallFunction("MEMBER_KICK_R", 1, "ddd",
						(int)dwTongID, (int)pCmd->m_dwTarget, 2))
						nOk = (int)Lua_ValueToNumber(pKick->m_LuaState, -1);
					pKick->SafeCallEnd(nTop);
					if (nOk != 1)
						return 20;
				}
			}
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
			if (nVan <= 0)
				return 5;
			// VIEC2 15/08: tran 10000/lan la chot chong tran cua ban port (goc
			// khong co tran tren). Truoc day return 5 -> roi vao cau "muc tieu
			// khong online" = bao sai hoan toan. Tu bao cau ro rang.
			if (nVan > 10000)
			{
				static const char szCap[] = "Mäi lÇn quyÒn gãp tèi ®a 10000 v¹n l­îng!";
				KPlayerChat::SendSystemInfo(1, nPlayerIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
					(char*)szCap, (int)strlen(szCap));
				return 20;
			}
			// VIEC2 15/08: 2 chot cua ban goc MONEY2BUILDFUND_G_1 (tong_mix.lua
			// :239-266) phai kiem TRUOC khi tru tien:
			// - an si (figure 4 = TONG_RETIRE, tong_header.lua:122) khong duoc gop
			// - tran cong hien tuan ca nhan 22.400 (MAX_WEEK_CONTRIBUTION)
			// He so goc: 1000 luong = 1 diem (COEF_CONTRIB_TO_VALUE) => 10 diem/van.
			int nCtb = nVan * 10;
			if (pMe && pMe->btFigure == 4)
			{
				static const char szRt[] = "Èn sü kh«ng thÓ ®ãng gãp!";
				KPlayerChat::SendSystemInfo(1, nPlayerIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
					(char*)szRt, (int)strlen(szRt));
				return 20;
			}
			if ((int)Player[nPlayerIdx].m_cTask.GetSaveVal(defTASK_JX2_WEEKLYOFFER) + nCtb > 22400)
			{
				static const char szWk[] = "Kh«ng thÓ ®ãng gãp, v× lµm cho ®iÓm cèng hiÕn tÝch lòy sÏ v­ît qu¸ giíi h¹n tuÇn!";
				KPlayerChat::SendSystemInfo(1, nPlayerIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
					(char*)szWk, (int)strlen(szWk));
				return 20;
			}
			if (!Player[nPlayerIdx].Pay(nVan * 10000))
				return 6;	// khong du tien
			{
				// Tran quyen gop kien thiet MOI TUAN (field 42, relay ghi theo
				// cot WEEK_BUILD_UPPER cua tong_level_data). Ban goc
				// (tong_mix.lua:205-236) KHONG bo phan vuot: phan lot duoi tran
				// vao quy kien thiet + quy tuan, phan VUOT chay sang quy du tru
				// kien thiet (field 19). Truoc day ta cong thang, tran vo nghia.
				DWORD dwUpper = GetField(dwTongID, 42);
				// PHAN BIEN M2: field 42 chi duoc relay ghi khi THANG CAP nen
				// bang chua tung thang cap co 42 = 0 -> tran vo hieu. Ban goc
				// khong luu field ma tra bang cap moi lan: lam dung vay khi 0.
				if (dwUpper == 0)
				{
					// cot WEEK_BUILD_UPPER cua tong_level_data.txt (cap 0-5)
					static const DWORD dwWk[6] =
						{3360, 3360, 6720, 10080, 16800, 25200};
					int nBLv = (int)GetField(dwTongID, 13);
					if (nBLv < 0)
						nBLv = 0;
					if (nBLv > 5)
						nBLv = 5;
					dwUpper = dwWk[nBLv];
				}
				DWORD dwWeek = GetField(dwTongID, 41);
				int nIn = nVan;
				int nOver = 0;
				if (dwUpper > 0)
				{
					int nRoom = (int)dwUpper - (int)dwWeek;
					if (nRoom < 0)
						nRoom = 0;
					if (nIn > nRoom)
					{
						nOver = nIn - nRoom;
						nIn = nRoom;
					}
				}
				if (nIn > 0)
				{
					sSendFieldCmd(dwTongID, 12, (DWORD)nIn, defTONG_JX2_OP_ADDU, dwParam);
					sSendFieldCmd(dwTongID, 41, (DWORD)nIn, defTONG_JX2_OP_ADDU, dwParam);
				}
				if (nOver > 0)
				{
					// contribution_entry.lua:35 - bao ro phan vuot chuyen di dau
					sSendFieldCmd(dwTongID, 19, (DWORD)nOver, defTONG_JX2_OP_ADDU, dwParam);
					KPlayerChat::SendSystemInfo(1, nPlayerIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
						(char*)"Ng©n s¸ch kiÕn thiÕt nhËn ®­îc v­ît qu¸ giíi h¹n tuÇn, phÇn d­ chuyÓn sang ng©n s¸ch kiÕn thiÕt dù bÞ.",
						(int)strlen("Ng©n s¸ch kiÕn thiÕt nhËn ®­îc v­ît qu¸ giíi h¹n tuÇn, phÇn d­ chuyÓn sang ng©n s¸ch kiÕn thiÕt dù bÞ."));
				}
			}
			// KHONG cong ngan quy (field 3/4) o day: ban goc MONEY2BUILDFUND chi
			// cong quy kien thiet; ngan quy co duong nap rieng COP_DEPOSIT_MONEY.
			// Truoc day cong ca hai = mot lan tra tien duoc ghi vao HAI tui.
			if (pMe)
			{
				// khoa 7 = cong hien tich luy; khoa 9 = muc tieu tuan nay;
				// khoa 11 = cong hien tuan nay. Truoc day chi ghi khoa 7 nen o
				// "Cong hien tuan" tren giao dien VINH VIEN bang 0 (client doc
				// khoa 9 - xem BuildClientView m_dwMyWeekOffer / m_dwWeekOffer).
				// VIEC2 15/08: cong hu CONG HIEN TIEU DUOC (truoc day chi ghi o
				// hien thi 7/9/11 nen gop tien ra 0 diem dung duoc). Nguyen van
				// ban goc: AddWeeklyOffer + AddCumulateOffer + AddContribution
				// (tong_mix.lua:259-262), moi hu +nCtb = nVan*10.
				{
					KPlayerTask* pTask = &Player[nPlayerIdx].m_cTask;
					pTask->SetSaveVal(defTASK_JX2_CONTRIBUTION,
						pTask->GetSaveVal(defTASK_JX2_CONTRIBUTION) + (DWORD)nCtb);
					pTask->SetSaveVal(defTASK_JX2_WEEKLYOFFER,
						pTask->GetSaveVal(defTASK_JX2_WEEKLYOFFER) + (DWORD)nCtb);
					pTask->SetSaveVal(defTASK_JX2_CUMULATEOFFER,
						pTask->GetSaveVal(defTASK_JX2_CUMULATEOFFER) + (DWORD)nCtb);
					// cau ca nhan nguyen van tong_mix.lua:263
					char szMe[224];
					sprintf(szMe, "B¹n ®ãng gãp <color=gold>%d<color>Ng©n l­îng ®Õn bang héi lËp quü, ®ång thêi nhËn ®­îc<color=green>%d<color>NhÊn vµo ®é cèng hiÕn",
						nVan * 10000, nCtb);
					KPlayerChat::SendSystemInfo(1, nPlayerIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
						szMe, (int)strlen(szMe));
				}
				sSendMemberFieldCmd(dwTongID, pMe->dwNameID, 7, (DWORD)nVan, defTONG_JX2_OP_ADD, dwParam);
				sSendMemberFieldCmd(dwTongID, pMe->dwNameID, 9, (DWORD)nVan, defTONG_JX2_OP_ADD, dwParam);
				sSendMemberFieldCmd(dwTongID, pMe->dwNameID, 11, (DWORD)nVan, defTONG_JX2_OP_ADD, dwParam);
			}
			{
				// nguyen van tong_mix.lua:242; Msg2Tong VO DIEU KIEN (:247),
				// so su kien chi ghi khi >= 100 van (:244 nOffer >= 1000000 luong)
				char szLog[160];
				sprintf(szLog, "%s ®· ®ãng gãp %d v¹n l­îng vµo ng©n s¸ch kiÕn thiÕt bang", Player[nPlayerIdx].m_PlayerName, nVan);
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
			// 2C 15/08: relay tu bao ket qua (thanh cong hoac ly do thieu gi)
			// tren kenh chat bang - vo boc KHONG duoc in them, neu khong se
			// bao 'da thuc hien' gia ngay ca khi relay tu choi.
			return 20;
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
			// FIX 14/08: quyen dung la 1002 "Bo nhiem" (blueprint goc trang
			// phan quyen ghi [BtnDepose] RightID=1002; client da dan nhan dung).
			// Truoc day gate NHAM 1101 = LIEN MINH nen truong lao phai tich o
			// "Lien minh" moi bo nhiem duoc. LUU Y VAN HANH: bang nao dang lam
			// vay se mat quyen sau restart -> bang chu tich lai o "Bo nhiem".
			if (!bMaster && !sJX2_HasRight(pMe, 1002))
				return 3;
			KTongJX2Member* pTarget = FindMember(pTong, pCmd->m_dwTarget);
			if (!pTarget || pTarget->btFigure == 0)
				return 4;
			if (pCmd->m_nParam1 < 1 || pCmd->m_nParam1 > 3)
				return 5;
			sSendTongOp(dwTongID, pCmd->m_dwTarget, defTONG_JX2_TOP_SET_FIGURE,
				pCmd->m_nParam1, 0, dwParam);
			// 2C 15/08: relay tu bao (thanh cong / du 7 truong lao / du 56 doi truong)
			return 20;
		}
	case defTONG_JX2_COP_SAVE_RECRUIT:
		{
			// quyen thu nhan 1901 hoac bang chu (JX2)
			if (!bMaster && !sJX2_HasRight(pMe, 1901))
				return 3;
			char szText[128];
			memcpy(szText, pCmd->m_szText, sizeof(szText));
			szText[127] = 0;
			{
				// Ban goc thu phi luu van an: [Main] SaveMessage cua trang chieu
				// mo ghi ro "phai tru tien quy bang hoi 1000 van". Truoc day ta
				// luu MIEN PHI.
				__int64 nGia = (__int64)1000 * 10000;	// 1000 van = 10 trieu luong
				__int64 nCo = (__int64)GetField(dwTongID, 3) |
					((__int64)GetField(dwTongID, 4) << 32);
				if (nCo < nGia)
					return 6;	// ngan quy bang khong du
				sSendMoneyCmd(dwTongID, -nGia, defTONG_JX2_OP_ADD, dwParam);
				// tru ngay tren ban sao de bam lien tuc khong qua duoc phep kiem
				__int64 nLai = nCo - nGia;
				pTong->mapField[3] = (DWORD)(nLai & 0xFFFFFFFF);
				pTong->mapField[4] = (DWORD)((nLai >> 32) & 0xFFFFFFFF);
			}
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
				const char* pszOK = "B¹n gia nhËp bang héi!";
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
			sSendZhaoMu(dwTongID, defTONG_JX2_ZM_DEL, szJoin, dwJoinID, 0, 0, dwParam);
			char szLog[160];
			sprintf(szLog, "%s duyÖt %s vµo bang",
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
					sprintf(szRef, "Bang héi %.31s ®· tõ chèi ®¬n xin gia nhËp cña b¹n.",
						pTong->szName);
					KPlayerChat::SendSystemInfo(1, nRefIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
						szRef, strlen(szRef));
				}
			}
			char szRefName[32];
			memcpy(szRefName, pTong->szApplyName[a], 32);
			szRefName[31] = 0;
			for (; a < (int)pTong->btApplyCount - 1; a++)
			{
				memcpy(pTong->szApplyName[a], pTong->szApplyName[a + 1], 32);
				pTong->dwApplyID[a] = pTong->dwApplyID[a + 1];
				pTong->wApplyLevel[a] = pTong->wApplyLevel[a + 1];
				pTong->btApplySex[a] = pTong->btApplySex[a + 1];
			}
			pTong->btApplyCount--;
			sSendZhaoMu(dwTongID, defTONG_JX2_ZM_DEL, szRefName,
				pCmd->m_dwTarget, 0, 0, dwParam);
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
					(char*)"Kh«ng thÓ thay ®æi phe ph¸i trong liªn minh cña bang héi", (int)strlen("Kh«ng thÓ thay ®æi phe ph¸i trong liªn minh cña bang héi"));
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
				static const char* szC[4] = {"", "ChÝnh ph¸i", "Tµ ph¸i", "Trung lËp"};
				char szLog[160];
				sprintf(szLog, "%s ®æi phe bang sang %s",
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
					sprintf(szLog, "%s ®· rót tõ ng©n quü bang héi %.0f l­îng",
						Player[nPlayerIdx].m_PlayerName, (double)nVan * 10000);
				else
					sprintf(szLog, "%s ph¸t cho %s %.0f l­îng",
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
				sprintf(szLog, "%s ®· ®ãng gãp %.0f l­îng vµo ng©n quü bang héi",
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
					(char*)"§· ë trong liªn minh, kh«ng thÓ t¸i lËp liªn minh! ", (int)strlen("§· ë trong liªn minh, kh«ng thÓ t¸i lËp liªn minh! "));
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
					(char*)"ChØ cã Bang chñ míi cã thÓ mêi gia nhËp liªn minh bang héi", (int)strlen("ChØ cã Bang chñ míi cã thÓ mêi gia nhËp liªn minh bang héi"));
				return 20;
			}
			if (GetField(dwTongID, 10) != 0)
			{
				// G_PLAYERTONG_12
				KPlayerChat::SendSystemInfo(1, nPlayerIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
					(char*)"Thµnh viªn cña liªn minh bang héi nµy khéng thÓ gia nhËp liªn minh bang héi kh¸c", (int)strlen("Thµnh viªn cña liªn minh bang héi nµy khéng thÓ gia nhËp liªn minh bang héi kh¸c"));
				return 20;
			}
			{
				// luat 3 ngay (field 49, ban goc gac o GS: 259200 giay)
				DWORD dwT = GetField(dwTongID, 49);
				if (dwT && time(NULL) - (time_t)dwT <= 259199)
				{
					char szMsg[160];
					sprintf(szMsg, "Sau khi rêi liªn minh 3 ngµy míi cã thÓ gia nhËp liªn minh míi, cÇn ®îi %d gi©y.",
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
					(char*)"Liªn minh xin gia nhËp kh«ng tån t¹i hoÆc ®· bÞ gi¶i t¸n!", (int)strlen("Liªn minh xin gia nhËp kh«ng tån t¹i hoÆc ®· bÞ gi¶i t¸n!"));
				return 20;
			}
			{
				// bao bang chu cua BANG MINH CHU lien minh (online): don xin vao
				char szMsg[160];
				sprintf(szMsg, "Bang héi %s xin gia nhËp liªn minh!", pTong->szName);
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
				sprintf(szMsg, " B¹n ®Õn gÆp %s xin phÐp gia nhËp liªn minh bang héi! ", pCmd->m_szText);
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
					(char*)"ChØ cã quèc v­¬ng míi thùc hiÖn ®­îc thao t¸c nµy!", (int)strlen("ChØ cã quèc v­¬ng míi thùc hiÖn ®­îc thao t¸c nµy!"));
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
				static const char* szNW[4] = {"?", "Thõa T­íng", "Nguyªn So¸i", "Tiªn Phong"};
				char szLog[160];
				if (bSet)
				{
					KTongJX2Member* pT2 = FindMember(pTong, pCmd->m_dwTarget);
					sprintf(szLog, "%s ®­îc phong lµm %s", pT2 ? pT2->szName : "?", szNW[nSlot]);
				}
				else
					sprintf(szLog, "C¸ch chøc %s", szNW[nSlot]);
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
			// PHAN BIEN B2: attr4 (cap SU DUNG) moi la so gameplay doc
			// (TWS_GetUseLevel) - de tu do la lach het rao phi/tran cua
			// TOP_WS_OP. Ban goc ep UseLevel <= Level (workshop_head.lua:97).
			int nUseLv = pCmd->m_nParam2;
			int nRealLv = (int)GetField(dwTongID, sWsAttrField(nType, 2));
			if (nUseLv < 1)
				nUseLv = 1;
			if (nUseLv > nRealLv)
				nUseLv = nRealLv;
			if (nUseLv < 1)
				return 10;	// khu chua co cap
			sSendFieldCmd(dwTongID, sWsAttrField(nType, 4), (DWORD)nUseLv,
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
			// FIX 14/08 ("nut tao lanh dia chua hoat dong"): client gui
			// COP_MAP_CREATE voi nParam1 = 0 (UiTongJX2.cpp SendOp(...,0,0,0)),
			// truoc day chuyen thang len relay nen relay ghi field 45/46 = 0 =
			// khong co lanh dia. Ban goc KHONG bam nut la xong: phai qua HOP
			// THOAI chon khu vuc (MAP_CREAT_G_1 -> PublicMap/SelectMap/BrowseMap
			// trong tong_vn\tong_mix.lua - da port san). Chi khi Lua chot lua
			// chon no moi goi TONG_ApplyCreatMap/SetTongMap de day len relay.
			// PHAI dung ExecuteScript2 (dat m_ActionScriptID) - neu dung khuon
			// g_GetScript nhu COP_KICK thi dap an dang "/#PublicMap(...)" cua
			// hop thoai KHONG dispatch duoc ve script.
			if (pCmd->m_btOp == defTONG_JX2_COP_MAP_CREATE && pCmd->m_nParam1 == 0)
			{
				if (nPlayerIdx > 0 && Player[nPlayerIdx].m_nIndex > 0)
					Player[nPlayerIdx].ExecuteScript2(
						(char*)"\\scriptjx2\\tong_vn\\tong_mix.lua",
						(char*)"MAP_CREAT_G_1", (int)dwTongID, 0);
				return 20;	// 20 = im lang: hop thoai Lua tu noi chuyen voi nguoi bam
			}
			BYTE btOp = defTONG_JX2_TOP_SET_MAP;
			if (pCmd->m_btOp == defTONG_JX2_COP_MAP_CREATE)
				btOp = defTONG_JX2_TOP_CREATE_MAP;
			else if (pCmd->m_btOp == defTONG_JX2_COP_MAP_DELETE)
				btOp = defTONG_JX2_TOP_DELETE_MAP;
			sSendTongOp(dwTongID, 0, btOp, pCmd->m_nParam1, pCmd->m_nParam2, dwParam);
			return 0;
		}
	case defTONG_JX2_COP_GRANT_GROUP:
		{
			// Phat theo CHUC VU (cua so blueprint phat ngan luong/cong hien):
			// m_szText = "elder|captain|member", nParam1: bit0-2 = so nhap la
			// TONG (chia deu) cho tung nhom, bit3 = chi nguoi online, bit4 =
			// ngan luong (0 = diem cong hien du tru).
			int nFlags = pCmd->m_nParam1;
			BOOL bMoney = (nFlags & 16) != 0;
			BOOL bOnline = (nFlags & 8) != 0;
			if (bMoney)
			{
				// nhu COP_PAY_MEMBER: bang chu hoac quyen Ngan quy 3001
				if (!bMaster && !sJX2_HasRight(pMe, 3001))
					return 3;
			}
			else if (!bMaster)
				return 3;	// phat cong hien: bang chu (nhu COP_GRANT)
			long nIn[3] = {0, 0, 0};
			{
				char szT[64];
				strncpy(szT, pCmd->m_szText, sizeof(szT) - 1);
				szT[sizeof(szT) - 1] = 0;
				sscanf(szT, "%ld|%ld|%ld", &nIn[0], &nIn[1], &nIn[2]);
			}
			if (nIn[0] < 0) nIn[0] = 0;
			if (nIn[1] < 0) nIn[1] = 0;
			if (nIn[2] < 0) nIn[2] = 0;
			if (!nIn[0] && !nIn[1] && !nIn[2])
				return 5;
			// gom danh sach tung nhom (figure 1/2/3); online loc bang chi so
			// Player cuc bo (dung cach TONG_JX2_ONE_MEMBER.m_btOnline lam)
			DWORD dwIDs[3][256];
			int nCnt[3] = {0, 0, 0};
			{
				std::map<DWORD, KTongJX2Member>::iterator itM;
				for (itM = pTong->mapMember.begin(); itM != pTong->mapMember.end(); ++itM)
				{
					int nFig = (int)itM->second.btFigure;
					if (nFig < 1 || nFig > 3)
						continue;
					if ((bOnline || bMoney) && sFindPlayerIdxByNameID(itM->first) <= 0)
						continue;	// ngan luong bat buoc online (Earn truc tiep)
					if (nCnt[nFig - 1] < 256)
						dwIDs[nFig - 1][nCnt[nFig - 1]++] = itM->first;
				}
			}
			// so moi dau tung nhom + tong can
			__int64 nPer[3];
			__int64 nNeed = 0;
			int g;
			for (g = 0; g < 3; g++)
			{
				if (nCnt[g] == 0 || nIn[g] == 0)
				{
					nPer[g] = 0;
					continue;
				}
				nPer[g] = (nFlags & (1 << g)) ? (nIn[g] / nCnt[g]) : nIn[g];
				nNeed += nPer[g] * nCnt[g];
			}
			if (nNeed <= 0)
				return 5;
			if (bMoney)
			{
				// don vi LUONG (blueprint: "Tong ngan quy phat ra: %u luong")
				__int64 nCo = (__int64)GetField(dwTongID, 3) |
					((__int64)GetField(dwTongID, 4) << 32);
				if (nCo < nNeed)
					return 6;	// (von cua bang hoi khong du)
				sSendMoneyCmd(dwTongID, -nNeed, defTONG_JX2_OP_ADD, dwParam);
				__int64 nLai = nCo - nNeed;
				pTong->mapField[3] = (DWORD)(nLai & 0xFFFFFFFF);
				pTong->mapField[4] = (DWORD)((nLai >> 32) & 0xFFFFFFFF);
				for (g = 0; g < 3; g++)
				{
					for (int m = 0; m < nCnt[g] && nPer[g] > 0; m++)
					{
						int nRIdx = sFindPlayerIdxByNameID(dwIDs[g][m]);
						if (nRIdx > 0)
							Player[nRIdx].Earn((int)nPer[g]);
					}
				}
			}
			else
			{
				// diem cong hien: kho du tru field 18; relay tru tung phan khi
				// nhan TOP_DIST_MEMBER - kiem tong truoc tren ban sao
				if ((__int64)GetField(dwTongID, 18) < nNeed)
					return 6;
				pTong->mapField[18] = (DWORD)((__int64)GetField(dwTongID, 18) - nNeed);
				for (g = 0; g < 3; g++)
				{
					for (int m = 0; m < nCnt[g] && nPer[g] > 0; m++)
						sSendTongOp(dwTongID, dwIDs[g][m], defTONG_JX2_TOP_DIST_MEMBER,
							(int)nPer[g], 0, dwParam);
				}
			}
			// thong bao theo khuon goc (tong_mix.lua:339): tung nhom mot cau
			{
				static const char* szFig[3] =
					{"Tr­ëng L·o", "§éi tr­ëng", "§Ö tö"};
				char szMsg[192];
				for (g = 0; g < 3; g++)
				{
					if (nCnt[g] == 0 || nPer[g] <= 0)
						continue;
					sprintf(szMsg, "%s Thµnh viªn trªn m¹ng %s(%d ng­êi)"
						" ®­îc ph©n ph¸t %d %s!",
						pMe->szName, szFig[g], nCnt[g], (int)nPer[g],
						bMoney ? "l­îng" : "®iÓm cèng hiÕn");
					sJX2_Msg2Tong(pTong, szMsg);
				}
			}
			return 0;
		}
	case defTONG_JX2_COP_ENTER_MAP:
		{
			// "Vao bon bang": KHONG kiem quyen - moi thanh vien deu vao duoc
			// lanh dia bang minh. Field 45 giu map bang (dat qua PublicMap /
			// TONG_ApplySetTongMap). Ban goc chan khi dang o trang thai chien
			// dau (tong_mix.lua:980 GetFightState).
			DWORD dwMap = GetField(dwTongID, 45);
			if (dwMap == 0)
			{
				KPlayerChat::SendSystemInfo(1, nPlayerIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
					(char*)"Bang héi ch­a cã khu vùc riªng.",
					(int)strlen("Bang héi ch­a cã khu vùc riªng."));
				return 20;
			}
			int nNpcIdx = Player[nPlayerIdx].m_nIndex;
			if (nNpcIdx <= 0)
				return 5;
			if (Npc[nNpcIdx].m_FightMode == 1)
			{
				KPlayerChat::SendSystemInfo(1, nPlayerIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
					(char*)"ChØ cã thÓ ®i vµo khu vùc bang héi tõ n¬i phi chiÕn ®Êu!",
					(int)strlen("ChØ cã thÓ ®i vµo khu vùc bang héi tõ n¬i phi chiÕn ®Êu!"));
				return 20;
			}
			if (g_SubWorldSet.SearchWorld((int)dwMap) < 0)
			{
				KPlayerChat::SendSystemInfo(1, nPlayerIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
					(char*)"B¶n ®å khu vùc bang héi ch­a ®­îc n¹p trªn m¸y chñ nµy.",
					(int)strlen("B¶n ®å khu vùc bang héi ch­a ®­îc n¹p trªn m¸y chñ nµy."));
				return 20;
			}
			// diem vao mac dinh cua ban goc (aMapEnterPosDef trong addtongnpc.lua)
			Npc[nNpcIdx].ChangeWorld((DWORD)dwMap, 1718 * 32, 3313 * 32);
			return 20;
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
				sprintf(szLog, "%s ®· ®ãng gãp %d ®iÓm cèng hiÕn vµo ng©n s¸ch cèng hiÕn bang", Player[nPlayerIdx].m_PlayerName, nVal);
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

// Su dung tac phuong - VONG GOI THAT theo kien truc ban goc:
//   use_g_*_ok (script da kiem phia GS) -> TWS_ApplyUse -> USE_R (tru tai
//   nguyen bang, ban goc chay o relay) -> USE_G_2 (phat vat pham).
// Ta 1 GameServer + relay khong co Lua nen chay ca hai chan NGAY TRONG
// lua_State cua chinh script goi (L chua dung USE_R/USE_G_2 cua file do).
// Truoc day ham nay NUOT lenh (khong goi gi) va cong nguoc +1 san luong
// -> Le vat ket co chong double-click (TaskTemp 196) vinh vien.
int LuaTWS_ApplyUse(Lua_State* L)
{
	DWORD dwTongID = sArgTongID(L);
	int nType = sWsType(L);
	int nArgc = Lua_GetTopIndex(L);
	int nChose = (nArgc >= 3) ? (int)Lua_ValueToNumber(L, 3) : 0;
	int nPass = (nArgc >= 3) ? 3 : 2;
	if (!g_TongJX2.FindTong(dwTongID) || !nType ||
		!g_TongJX2.GetField(dwTongID, sWsAttrField(nType, 0)))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	// chan 1: USE_R - tu choi la dung, khong phat gi
	Lua_GetGlobal(L, "USE_R");
	if (lua_isnil(L, -1))
	{
		lua_settop(L, -2);
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, (double)dwTongID);
	Lua_PushNumber(L, (double)nType);
	if (nPass >= 3)
		Lua_PushNumber(L, (double)nChose);
	// PHAN BIEN D3: USE_R loi runtime thi lua_call don sach stack va KHONG
	// push ket qua - doc -1 luc do la doc nham THAM SO CUOI cua chinh minh
	// (= 1 o cac khu 3 tham so) -> phat do MIEN PHI. Phai kiem status.
	if (Lua_Call(L, nPass, 1) != 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nOk = (int)Lua_ValueToNumber(L, -1);
	lua_settop(L, -2);
	if (nOk != 1)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	// chan 2: USE_G_2 - phat vat pham cho nguoi choi
	Lua_GetGlobal(L, "USE_G_2");
	if (lua_isnil(L, -1))
		lua_settop(L, -2);
	else
	{
		Lua_PushNumber(L, (double)dwTongID);
		Lua_PushNumber(L, (double)nType);
		if (nPass >= 3)
			Lua_PushNumber(L, (double)nChose);
		if (Lua_Call(L, nPass, 0) != 0)
			; // USE_G_2 loi: stack da duoc lua_call don, khong lam gi them
	}
	Lua_PushNumber(L, 1);
	return 1;
}

// Chay MAINTAIN_R cua tung khu cho MOI bang (sinh SAN LUONG NGAY - truoc
// day luon 0 nen ca 3 khu deu bi gate "san luong < 100" chan). Ban goc do
// relay hen gio goi; ta goi tu timerserver.lua moi ngay. Duong dan da
// duoc g_IniScriptEngine nap san vao cay script (chu thuong).

int LuaTWS_MaintainAll(Lua_State* L)
{
	int nRun = 0;
	DWORD dwTong = g_TongJX2.NextTongID(0);
	while (dwTong)
	{
		for (int t = 1; t <= defTONG_JX2_WS_MAX_TYPE; t++)
		{
			if (!g_TongJX2.GetField(dwTong, sWsAttrField(t, 0)))
				continue;
			KLuaScript* pScript = (KLuaScript*)g_GetScript(s_szWsScriptPath[t]);
			if (!pScript)
				continue;
			int nTop = 0;
			pScript->SafeCallBegin(&nTop);
			if (pScript->CallFunction("MAINTAIN_R", 0, "dd", (int)dwTong, t))
				nRun++;
			pScript->SafeCallEnd(nTop);
		}
		dwTong = g_TongJX2.NextTongID(dwTong);
	}
	Lua_PushNumber(L, (double)nRun);
	return 1;
}

// Bao tri NGAY + TUAN cua bang bang CHINH LUA GOC (tong.lua): tro cap,
// tam ngung, chi phi duy tri, muc tieu tuan + chon muc tieu moi - tat ca
// la ma nguyen van. Goi tu timerserver 06h05 (nWeekday: 1 = thu Hai ->
// chay WEEKLY_MAINTAIN_R truoc). Ban goc do relay hen gio; phan relay
// van giu: don thanh vien 9->10/11->12, reset 41, Week+1, Day+1.
int LuaTONG_DailyMaintainAll(Lua_State* L)
{
	int nWeekday = (Lua_GetTopIndex(L) >= 1) ? (int)Lua_ValueToNumber(L, 1) : -1;
	KLuaScript* pScript = (KLuaScript*)g_GetScript("\\scriptjx2\\tong_vn\\tong.lua");
	if (!pScript)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nRun = 0;
	DWORD dwTong = g_TongJX2.NextTongID(0);
	while (dwTong)
	{
		int nTop = 0;
		if (nWeekday == 1)
		{
			pScript->SafeCallBegin(&nTop);
			pScript->CallFunction("WEEKLY_MAINTAIN_R", 0, "d", (int)dwTong);
			pScript->SafeCallEnd(nTop);
		}
		pScript->SafeCallBegin(&nTop);
		if (pScript->CallFunction("MAINTAIN_R", 0, "d", (int)dwTong))
			nRun++;
		pScript->SafeCallEnd(nTop);
		dwTong = g_TongJX2.NextTongID(dwTong);
	}
	Lua_PushNumber(L, (double)nRun);
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

// SyncTaskValue(id) - day gia tri task hien tai xuong client. [TaskGuide]
// id >= 256 di kenh script-action UI_TASKVALUE; id < 256 dung goi TASK_VALUE_SYNC cu.
int LuaJX2_SyncTaskValue(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Lua_GetTopIndex(L) >= 1)
	{
		int nTaskId = (int)Lua_ValueToNumber(L, 1);
		Player[nPlayerIndex].SyncTaskValueToClient(nTaskId,
			(int)Player[nPlayerIndex].m_cTask.GetSaveVal(nTaskId));
	}
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

// Msg2PlayerByName(szName, szMsg) - nhan he thong cho nguoi choi theo TEN
// (tong_mix.lua MEMBER_KICK_R dung o duong tu choi). Offline thi bo qua.
int LuaJX2_Msg2PlayerByName(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;
	char* szName = (char*)Lua_ValueToString(L, 1);
	char* szMsg = (char*)Lua_ValueToString(L, 2);
	if (!szName || !szMsg || !szName[0] || !szMsg[0])
		return 0;
	int nIdx = sFindPlayerIdxByNameID(g_FileName2Id(szName));
	if (nIdx > 0)
		KPlayerChat::SendSystemInfo(1, nIdx, MESSAGE_SYSTEM_ANNOUCE_HEAD,
			szMsg, (int)strlen(szMsg));
	return 0;
}

// GetTongDuty() - chuc dai than quoc gia cua CHINH NGUOI GOI:
// 0 khong co / 1 Thua Tuong / 2 Nguyen Soai / 3 Tien Phong.
// Field 51/52/53 giu NameID nguoi dang giu tung chuc (dat boi COP 34/35).
int LuaJX2_GetTongDuty(Lua_State* L)
{
	int nPlayerIdx = GetPlayerIndex(L);
	int nRet = 0;
	if (nPlayerIdx > 0 && nPlayerIdx < MAX_PLAYER)
	{
		DWORD dwTongID = Player[nPlayerIdx].m_cTong.GetTongNameID();
		if (dwTongID)
		{
			DWORD dwMe = g_FileName2Id(Player[nPlayerIdx].m_PlayerName);
			for (int nSlot = 1; nSlot <= 3 && dwMe; nSlot++)
			{
				if (g_TongJX2.GetField(dwTongID, (WORD)(50 + nSlot)) == dwMe)
				{
					nRet = nSlot;
					break;
				}
			}
		}
	}
	Lua_PushNumber(L, (double)nRet);
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
		sprintf(szLog, "Bang chñ  ®· tuyªn chiÕn bang héi%s råi", szDest);
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

//////////////////////////////////////////////////////////////////////
// DOT E (E6): helper C cho KJx2CityWar khi doi chu thanh - ghi field 48
// (OccupyCityDay) theo duong script goc CITY_OCCUPY_R (tong_mix.lua:1074-1082:
// chu MOI = TONG_GetDay cua chinh bang do, chu CU = 0). Di qua sSendFieldCmd
// nen co ap lac quan + echo relay nhu moi field khac.
//////////////////////////////////////////////////////////////////////
void KTongJX2_SetOccupyCityDayC(DWORD dwTongID, int nDay)
{
	if (!dwTongID || !g_TongJX2.FindTong(dwTongID))
		return;
	sSendFieldCmd(dwTongID, 48, (DWORD)nDay, defTONG_JX2_OP_SET, 0);
}

DWORD KTongJX2_GetFieldC(DWORD dwTongID, WORD wKey)
{
	return g_TongJX2.GetField(dwTongID, wKey);
}

#endif // _SERVER
