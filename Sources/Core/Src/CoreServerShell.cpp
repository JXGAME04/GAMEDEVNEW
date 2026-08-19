/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong KiÒu
//	CreateTime:	2020-12-20
------------------------------------------------------------------------------------------
*****************************************************************************************/
#include "KCore.h"
#include "CoreServerShell.h"
#include "KThread.h"
#include "KPlayer.h"
#include "KItemList.h"
#include "KItemSet.h"
#include "KSubWorldSet.h"
#include "KProtocolProcess.h"
#include "KNewProtocolProcess.h"
#include "KTongJX2.h"	// JX2 port
#include "KJx2League.h"	// DOT E cong thanh: GLOBAL mission timer
#include "KJx2CityWar.h"	// DOT E cong thanh: state 7 thanh (E3)
#include "KSimCity.h"	// Port SimCity: nhip di chuyen bot
#include "KPlayerBot.h"	// Bot la KPlayer that
#include "KPlayerSet.h"
#include "KLadder.h"

#ifdef _STANDALONE
#include "KLadder.cpp"
#endif

//#include "KNetServer.h"
//#include "../MultiServer/Heaven/Interface/iServer.h"
#ifdef _STANDALONE
#include "IClient.h"
#else
#include "../../lib/S3DBInterface.h"
#include "../../Headers/IClient.h"
#include "../../../Headers/IClient.h"
#include "../../../Headers/KGmProtocol.h"
#endif

#include "LuaFuns.h"
#include "KSortScript.h"
#include "KSubWorld.h"

#include "malloc.h"
#include <BauCua.h>

class CoreServerShell : public iCoreServerShell
{
public:
	int  GetLoopRate();
	void GetGuid(int nIndex, void* pGuid);
	DWORD GetExchangeMap(int nIndex);
	bool IsPlayerLoginTimeOut(int nIndex);
	void RemovePlayerLoginTimeOut(int nIndex);
	bool IsPlayerExchangingServer(int nIndex);
	void ProcessClientMessage(int nIndex, const char* pChar, int nSize);
	void ProcessNewClientMessage(IClient*, DWORD, DWORD, int nIndex, const char* pChar, int nSize);
	void SendNetMsgToTransfer(IClient* pClient);
	void SendNetMsgToChat(IClient* pClient);
	void SendNetMsgToTong(IClient* pClient);
	void ProcessBroadcastMessage(const char* pChar, int nSize);
	void ProcessExecuteMessage(const char* pChar, int nSize);
	int GetClientNetConnectIdx(int nIndex);
	void ClientDisconnect(int nIndex);
	void RemoveQuitingPlayer(int nIndex);
	void* SavePlayerDataAtOnce(int nIndex);
	bool IsCharacterQuiting(int nIndex);
	bool IsCharacterNeedSave(int nIndex);
	BYTE GetCharacterLixian(int nIndex);						//#Uy thac
	void SetCharacterLixianCompleted(int nIndex);
	int SetCharacterLixianEnd(const char* pAccName);
	bool CheckProtocolSize(const char* pChar, int nSize);
	bool PlayerDbLoading(int nPlayerIndex, int bSyncEnd, int& nStep, unsigned int& nParam);
	int AttachPlayer(const unsigned long lnID, GUID* pGuid, char* sHWID);
	void GetPlayerIndexByGuid(GUID* pGuid, int* pnIndex, int* plnID);
	int AddPlayerToWorld(int nIndex);
	void AddPlayerToWorld2(int nIndex, bool value);
	void* PreparePlayerForExchange(int nIndex);
	void PreparePlayerForLoginFailed(int nIndex);
	void RemovePlayerForExchange(int nIndex);
	void RemovePlayerForLimit(int nIndex);
	void RecoverPlayerExchange(int nIndex);
	int  AddCharacter(int nExtPoint, int nChangeExtPoint, void* pBuffer, GUID* pGuid);
	int	 AddTempTaskValue(int nIndex, const char* pData);
	int	 OperationRequest(unsigned int uOper, intptr_t uParam, int nParam);
	int	 GetConnectInfo(KCoreConnectInfo* pInfo);
	//BOOL ValidPingTime(int nIndex);
	int	 GetGameData(unsigned int uDataId, intptr_t uParam, intptr_t nParam);
	int  Breathe();
	void Release();
	void SetSaveStatus(int nIndex, UINT uStatus);
	UINT GetSaveStatus(int nIndex);
	BOOL GroupChat(IClient* pClient, DWORD FromIP, unsigned long FromRelayID, DWORD channid, BYTE tgtcls, DWORD tgtid, const void* pData, size_t size);
	void SetLadder(void* pData, size_t uSize);
	BOOL PayForSpeech(int nIndex, int nType);
	void SetExtPoint(int nIndex, int nExtPoint);
private:
	int	 OnLunch(LPVOID pServer);
	int	 OnShutdown();
};

static CoreServerShell	g_CoreServerShell;

CORE_API void g_InitCore(char * nParmName);

#ifndef _STANDALONE
extern "C" __declspec(dllexport)
#endif
iCoreServerShell* CoreGetServerShell(const char* gamepass)
{
	g_InitCore((char*)gamepass);
	return &g_CoreServerShell;
}

void CoreServerShell::Release()
{
	g_ReleaseCore();
}

int CoreServerShell::GetLoopRate()
{
	return g_SubWorldSet.m_nLoopRate;
}

int	 CoreServerShell::GetConnectInfo(KCoreConnectInfo* pInfo)
{
	if (pInfo)
		pInfo->nNumPlayer = PlayerSet.GetPlayerNumber();
	return 1;
}

int CoreServerShell::AddCharacter(int nExtPoint, int nChangeExtPoint, void* pBuffer, GUID* pGuid)
{
	int nIdx = 0;
	const TRoleData* pData = (const TRoleData*)pBuffer;

	if (pData && pData->BaseInfo.szName[0])
	{
		nIdx = PlayerSet.Add((char*)pData->BaseInfo.szName, pGuid);
		if (nIdx <= 0 || nIdx >= MAX_PLAYER)
			return 0;
		strcpy(Player[nIdx].m_AccoutName, pData->BaseInfo.caccname);
		strcpy(Player[nIdx].m_PlayerName, pData->BaseInfo.szName);
		DWORD	dwLen = pData->dwDataLen;
//		_ASSERT(dwLen < 64 * 1024);
		ZeroMemory(Player[nIdx].m_SaveBuffer, sizeof(Player[nIdx].m_SaveBuffer));
		memcpy(Player[nIdx].m_SaveBuffer, pBuffer, dwLen);

		Player[nIdx].m_pStatusLoadPlayerInfo = Player[nIdx].m_SaveBuffer;
		// À©Õ¹µã£¬ÓÃÓÚ»î¶¯
		Player[nIdx].SetExtPoint(nExtPoint, nChangeExtPoint);
		return nIdx;
	}
	return 0;
}

bool CoreServerShell::PlayerDbLoading(int nPlayerIndex, int bSyncEnd, int& nStep, unsigned int& nParam)
{
	TRoleData* pData = (TRoleData *)Player[nPlayerIndex].m_pStatusLoadPlayerInfo;
	
	if (bSyncEnd)
	{
		Player[nPlayerIndex].m_pStatusLoadPlayerInfo = NULL;
		nStep = 0;
		nParam = 0;

		return true;
	}
	else if (pData)	
	{
//		if (0 == Player[nPlayerIndex].LoadDBPlayerInfo((BYTE *)pData, nStep, nParam))
//		{
//			// °ÑÍæ¼ÒµÄµÇÈë×´Ì¬ÉèÖÃÎªÎ´µÇÈë£¬µÈ´ýÊ±ÑÓ×Ô¶¯Çå³ý
//			Player[nPlayerIndex].m_nNetConnectIdx = -1;
//			Player[nPlayerIndex].m_dwLoginTime = -1;
//			return false;
//		}
//		else
//			return true;
		return Player[nPlayerIndex].LoadDBPlayerInfo((BYTE *)pData, nStep, nParam);
	}
	return false;
}

int CoreServerShell::AddPlayerToWorld(int nIndex)
{
//	int nIndex = PlayerSet.FindClient(lnID);
	return Player[nIndex].LaunchPlayer();
}

void CoreServerShell::AddPlayerToWorld2(int nIndex, bool value)
{
	Player[nIndex].LaunchPlayer2(value);
}

void CoreServerShell::ProcessClientMessage(int nIndex, const char* pChar, int nSize)
{
	PlayerSet.ProcessClientMessage(nIndex, pChar, nSize);
}

void CoreServerShell::ProcessNewClientMessage(IClient* pTransfer,
									   DWORD dwFromIP, DWORD dwFromRelayID,
									   int nPlayerIndex,
									   const char* pChar, int nSize)
{
	g_NewProtocolProcess.ProcessNetMsg(pTransfer, dwFromIP, dwFromRelayID,
										nPlayerIndex, (BYTE*)pChar, nSize);

}

void CoreServerShell::SendNetMsgToTransfer(IClient* pClient)
{
	g_NewProtocolProcess.SendNetMsgToTransfer(pClient);
}

void CoreServerShell::SendNetMsgToChat(IClient* pClient)
{
	g_NewProtocolProcess.SendNetMsgToChat(pClient);
}

void CoreServerShell::SendNetMsgToTong(IClient* pClient)
{
	g_NewProtocolProcess.SendNetMsgToTong(pClient);
}

void CoreServerShell::ProcessBroadcastMessage(const char* pChar, int nSize)
{
	g_NewProtocolProcess.BroadcastLocalServer(pChar, nSize);
}

void CoreServerShell::ProcessExecuteMessage(const char* pChar, int nSize)
{
	g_NewProtocolProcess.ExecuteLocalServer(pChar, nSize);
}
int CoreServerShell::GetClientNetConnectIdx(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
	{
		return -1;
	}
	return Player[nIndex].m_nNetConnectIdx;
}
void CoreServerShell::ClientDisconnect(int nIndex)
{
//	PlayerSet.Remove(nClient);
	PlayerSet.PrepareRemove(nIndex);
}

void CoreServerShell::RemoveQuitingPlayer(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	if (Player[nIndex].IsWaitingRemove())
	{
		PlayerSet.RemoveQuiting(nIndex);
	}
}
//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´ÓÓÎÏ·ÊÀ½ç»ñÈ¡Êý¾Ý
//	²ÎÊý£ºunsigned int uDataId --> ±íÊ¾»ñÈ¡ÓÎÏ·Êý¾ÝµÄÊý¾ÝÏîÄÚÈÝË÷Òý£¬ÆäÖµÎªÃ·¾ÙÀàÐÍ
//							GAMEDATA_INDEXµÄÈ¡ÖµÖ®Ò»¡£
//		  unsigned int uParam  --> ÒÀ¾ÝuDataIdµÄÈ¡ÖµÇé¿ö¶ø¶¨
//		  int nParam --> ÒÀ¾ÝuDataIdµÄÈ¡ÖµÇé¿ö¶ø¶¨
//	·µ»Ø£ºÒÀ¾ÝuDataIdµÄÈ¡ÖµÇé¿ö¶ø¶¨¡£
//--------------------------------------------------------------------------
int	CoreServerShell::GetGameData(unsigned int uDataId, intptr_t uParam, intptr_t nParam)
{
	int nRet = 0;
	switch(uDataId)
	{
	case SGDI_CHARACTER_ACCOUNT:
		if (uParam)
		{
			nRet = PlayerSet.GetPlayerAccount(nParam, (char *)uParam);
			if (nRet == FALSE)
				((char *)uParam)[0] = 0;
		}
		break;
	case SGDI_CHARACTER_HWID:
		if (uParam)
		{
			nRet = PlayerSet.GetPlayerHWID(nParam, (char *)uParam);
			if (nRet == FALSE)
				((char *)uParam)[0] = 0;
		}
		break;
	case SGDI_CHARACTER_NAME:
		if (uParam)
		{
			nRet = PlayerSet.GetPlayerName(nParam, (char*)uParam);
			if (nRet == FALSE)
				((char *)uParam)[0] = 0;
		}
		break;
	case SGDI_CHARACTER_EXTPOINTCHANGED:
		if (uParam)
		{
			if (uParam >= MAX_PLAYER)
			{
				nRet = 0;
				break;
			}
			nRet = Player[uParam].GetExtPointChanged();
		}
		break;
	case SGDI_CHARACTER_EXTPOINT:
		if (uParam)
		{
			if (uParam >= MAX_PLAYER)
			{
				nRet = 0;
				break;
			}
			nRet = Player[uParam].GetExtPoint();
		}
		break;
	case SGDI_LOADEDMAP_ID:
		if (uParam)
		{
			int i;
			int nMax = nParam / sizeof(int32_t);;
			if(nMax < MAX_SUBWORLD) nMax = MAX_SUBWORLD;
			for (i = 0; i < nMax; i++)
			{
				if (SubWorld[i].m_SubWorldID != -1)
				{
					((int32_t *)uParam)[i] = (int)SubWorld[i].m_SubWorldID;//edit by phong kieu fix loi load map lon hon 255
					//int32_t tmp = static_cast<int32_t>(SubWorld[i].m_SubWorldID);
					//reinterpret_cast<int32_t*>(uParam)[i] = tmp;
				}
				else
				{
					nRet = i;
					break;
				}
			}
		}
		break;
	case SGDI_CHARACTER_ID:
		if (uParam)
		{
			if (uParam >= MAX_PLAYER)
			{
				nRet = 0;
				break;
			}

			nRet = Player[uParam].m_dwID;
			break;
		}
		break;
	case SGDI_GET_ITEM_ID:
		if (uParam)
		{
			if (uParam >= MAX_PLAYER)
			{
				nRet = 0;
				break;
			}

			nRet = ItemSet.SearchID(nParam);
			break;
		}
		break;
	case SGDI_TONG_JX2VIEW:	// JX2 port
		return g_TongJX2.BuildClientView((int)(nParam & 0xFFFF),
			(int)((nParam >> 16) & 0xF), (int)((nParam >> 20) & 0xFFF),
			(void*)uParam, 2048);
	case SGDI_TONG_JX2OP:	// JX2 port
		return g_TongJX2.DoClientOp((int)nParam, (const void*)uParam);
	case SGDI_PBOT_IS_BOT:	// khe Player[] nay co phai bot khong
		return PB_IsBot((int)nParam);
	case SGDI_PBOT_WHISPER:	// tin nhan mat gui toi mot bot -> bot tu tra loi
		return PB_WhisperReply((const PB_WHISPER*)uParam);
	case SGDI_CHARACTER_NETID:
		if (uParam)
		{
			if (uParam >= MAX_PLAYER)
			{
				nRet = -1;
				break;
			}
			nRet = Player[uParam].m_nNetConnectIdx;
		}
		break;
	case SGDI_CHARACTER_SEX:
		{
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Npc[Player[nParam].m_nIndex].m_nSex;
		}
		break;
	// ´«Èë°ï»á½¨Á¢²ÎÊý£¬·µ»ØÌõ¼þÊÇ·ñ³ÉÁ¢
	// uParam : struct STONG_SERVER_TO_CORE_APPLY_CREATE point
	// return : Ìõ¼þÊÇ·ñ³ÉÁ¢
	case SGDI_TONG_APPLY_CREATE:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_APPLY_CREATE	*pApply = (STONG_SERVER_TO_CORE_APPLY_CREATE*)uParam;

			int		nPlayerIdx;
			int		nCamp;
			char	szTongName[defTONG_NAME_MAX_LENGTH + 2];

			szTongName[sizeof(szTongName) - 1] = 0;
			strcpy_s(szTongName, pApply->m_szTongName);
			nPlayerIdx = pApply->m_nPlayerIdx;
			nCamp = pApply->m_nCamp;

			if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
			{
				nRet = FALSE;
				break;
			}
			nRet = Player[nPlayerIdx].m_cTong.CheckCreateCondition(nCamp, szTongName);
		}
		break;

	// ÉêÇë¼ÓÈë°ï»á
	// uParam : struct STONG_SERVER_TO_CORE_APPLY_ADD point
	case SGDI_TONG_APPLY_ADD:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_APPLY_ADD	*pAdd = (STONG_SERVER_TO_CORE_APPLY_ADD*)uParam;
			if (pAdd->m_nPlayerIdx <= 0 || pAdd->m_nPlayerIdx >= MAX_PLAYER)
				break;
			Player[pAdd->m_nPlayerIdx].m_cTong.TransferAddApply(pAdd->m_dwNpcID);
		}
		break;

	// ÅÐ¶Ï¼ÓÈë°ï»áÌõ¼þÊÇ·ñºÏÊÊ
	// uParam : ´«ÈëµÃ char point £¬ÓÃÓÚ½ÓÊÕ°ï»áÃû³Æ
	// nParam : struct STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION point
	case SGDI_TONG_CHECK_ADD_CONDITION:
		{
			nRet = 0;
			STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION	*pAdd;
			pAdd = (STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION*)nParam;
			if (pAdd->m_nSelfIdx <= 0 || pAdd->m_nSelfIdx >= MAX_PLAYER)
				break;
			if (pAdd->m_nTargetIdx <= 0 || pAdd->m_nTargetIdx >= MAX_PLAYER || g_FileName2Id(Npc[Player[pAdd->m_nTargetIdx].m_nIndex].Name) != pAdd->m_dwNameID)
				break;
			if (Player[pAdd->m_nSelfIdx].m_cTong.CheckAddCondition(pAdd->m_nTargetIdx))
			{
				Player[pAdd->m_nSelfIdx].m_cTong.GetTongName((char*)uParam);
				nRet = 1;
			}
		}
		break;

	// »ñµÃ°ï»áÐÅÏ¢
	// uParam : ´«ÈëµÄ STONG_SERVER_TO_CORE_GET_INFO point
	case SGDI_TONG_GET_INFO:
		{
			STONG_SERVER_TO_CORE_GET_INFO	*pInfo = (STONG_SERVER_TO_CORE_GET_INFO*)uParam;
			switch (pInfo->m_nInfoID)
			{
			case enumTONG_APPLY_INFO_ID_SELF:
				{
					if (pInfo->m_nSelfIdx <= 0 || pInfo->m_nSelfIdx >= MAX_PLAYER)
						break;
					Player[pInfo->m_nSelfIdx].m_cTong.SendSelfInfo();
				}
				break;
			case enumTONG_APPLY_INFO_ID_MASTER:
				break;
			case enumTONG_APPLY_INFO_ID_DIRECTOR:
				break;
			case enumTONG_APPLY_INFO_ID_MANAGER:
				{
					nRet = 0;
					if (pInfo->m_nSelfIdx <= 0 || pInfo->m_nSelfIdx >= MAX_PLAYER)
						break;
					if (Player[pInfo->m_nSelfIdx].m_cTong.CanGetManagerInfo((DWORD)pInfo->m_nParam1))
						nRet = 1;
				}
				break;
			case enumTONG_APPLY_INFO_ID_MEMBER:
				{
					nRet = 0;
					if (pInfo->m_nSelfIdx <= 0 || pInfo->m_nSelfIdx >= MAX_PLAYER)
						break;
					if (Player[pInfo->m_nSelfIdx].m_cTong.CanGetMemberInfo((DWORD)pInfo->m_nParam1))
						nRet = 1;
				}
				break;
			case enumTONG_APPLY_INFO_ID_ONE:
				break;

			case enumTONG_APPLY_INFO_ID_TONG_HEAD:
				{
					nRet = 0;
					if (pInfo->m_nSelfIdx <= 0 || pInfo->m_nSelfIdx >= MAX_PLAYER)
						break;
					int		nPlayer;
					if (Npc[Player[pInfo->m_nSelfIdx].m_nIndex].m_dwID == (DWORD)pInfo->m_nParam1)
						nPlayer = pInfo->m_nSelfIdx;
					else
						nPlayer = Player[pInfo->m_nSelfIdx].FindAroundPlayer((DWORD)pInfo->m_nParam1);
					if (nPlayer == -1)
						break;
					nRet = Player[nPlayer].m_cTong.GetTongNameID();
				}
				break;
			}
		}
		break;

	// ÅÐ¶ÏÊÇ·ñÓÐÈÎÃüÈ¨Àû
	// uParam : ´«ÈëµÄ TONG_APPLY_INSTATE_COMMAND point
	// nParam : PlayerIndex
	case SGDI_TONG_INSTATE_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_INSTATE_COMMAND	*pApply = (TONG_APPLY_INSTATE_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckInstatePower(pApply);
		}
		break;

	// ±»ÈÎÃü£¬°ï»áÊý¾Ý±ä»¯
	// uParam : ´«ÈëµÄ STONG_SERVER_TO_CORE_BE_INSTATED point
	case SGDI_TONG_BE_INSTATED:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_BE_INSTATED	*pInstated = (STONG_SERVER_TO_CORE_BE_INSTATED*)uParam;
			if (pInstated->m_nPlayerIdx <= 0 || pInstated->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pInstated->m_nPlayerIdx].m_nIndex <= 0)
				break;
			Player[pInstated->m_nPlayerIdx].m_cTong.BeInstated(pInstated);
		}
		break;

	// ÅÐ¶ÏÊÇ·ñÓÐÌßÈËÈ¨Àû
	// uParam : ´«ÈëµÄ TONG_APPLY_KICK_COMMAND point
	// nParam : PlayerIndex
	case SGDI_TONG_KICK_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_KICK_COMMAND	*pKick = (TONG_APPLY_KICK_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckKickPower(pKick);
		}
		break;

	// ±»Ìß³ö°ï»á
	// uParam : ´«ÈëµÄ STONG_SERVER_TO_CORE_BE_KICKED point
	case SGDI_TONG_BE_KICKED:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_BE_KICKED	*pKicked = (STONG_SERVER_TO_CORE_BE_KICKED*)uParam;
			if (pKicked->m_nPlayerIdx <= 0 || pKicked->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pKicked->m_nPlayerIdx].m_nIndex <= 0)
				break;
			Player[pKicked->m_nPlayerIdx].m_cTong.BeKicked(pKicked);
			Player[pKicked->m_nPlayerIdx].UpdataCurData();
		}
		break;

	// Àë¿ª°ï»á
	// uParam : ´«ÈëµÄ TONG_APPLY_LEAVE_COMMAND point
	// nParam : PlayerIndex
	case SGDI_TONG_LEAVE_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_LEAVE_COMMAND	*pLeave = (TONG_APPLY_LEAVE_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckLeavePower(pLeave);
		}
		break;

	// Àë¿ª°ï»á
	// uParam : ´«ÈëµÄ STONG_SERVER_TO_CORE_LEAVE point
	case SGDI_TONG_LEAVE:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_LEAVE	*pLeave = (STONG_SERVER_TO_CORE_LEAVE*)uParam;
			if (pLeave->m_nPlayerIdx <= 0 || pLeave->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pLeave->m_nPlayerIdx].m_nIndex <= 0)
				break;
			Player[pLeave->m_nPlayerIdx].m_cTong.Leave(pLeave);
			Player[pLeave->m_nPlayerIdx].UpdataCurData();
		}
		break;

	// Àë¿ª°ï»áÅÐ¶Ï
	// uParam : ´«ÈëµÄ TONG_APPLY_CHANGE_MASTER_COMMAND point
	// nParam : PlayerIndex
	case SGDI_TONG_CHANGE_MASTER_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_CHANGE_MASTER_COMMAND	*pChange = (TONG_APPLY_CHANGE_MASTER_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckChangeMasterPower(pChange);
		}
		break;

	// ÄÜ·ñ½ÓÊÜ´«Î»ÅÐ¶Ï
	// uParam : ´«ÈëµÄ STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER point
	case SGDI_TONG_GET_MASTER_POWER:
		if (uParam)
		{
			nRet = 0;
			STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER	*pCheck = (STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER*)uParam;
			if (pCheck->m_nPlayerIdx <= 0 || pCheck->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pCheck->m_nPlayerIdx].m_nIndex <= 0)
				break;
			nRet = Player[pCheck->m_nPlayerIdx].m_cTong.CheckGetMasterPower(pCheck);
		}
		break;

	// ´«Î»µ¼ÖÂÉí·Ý¸Ä±ä
	// uParam : ´«ÈëµÄ STONG_SERVER_TO_CORE_CHANGE_AS point
	case SGDI_TONG_CHANGE_AS:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_CHANGE_AS	*pAs = (STONG_SERVER_TO_CORE_CHANGE_AS*)uParam;
			if (pAs->m_nPlayerIdx <= 0 || pAs->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pAs->m_nPlayerIdx].m_nIndex <= 0)
				break;
			Player[pAs->m_nPlayerIdx].m_cTong.ChangeAs(pAs);
		}
		break;

	// °ïÖ÷»»ÁË
	// uParam : ´«ÈëµÄ STONG_SERVER_TO_CORE_CHANGE_MASTER point
	case SGDI_TONG_CHANGE_MASTER:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_CHANGE_MASTER	*pChange = (STONG_SERVER_TO_CORE_CHANGE_MASTER*)uParam;
			int nIdx;
			nIdx = PlayerSet.GetFirstPlayer();
			while (nIdx)
			{
				if (Player[nIdx].m_cTong.GetTongNameID() == pChange->m_dwTongNameID)
				{
					Player[nIdx].m_cTong.ChangeMaster(pChange->m_szName);
				}
				nIdx = PlayerSet.GetNextPlayer();
			}
		}
		break;

	// »ñµÃ°ï»áÃû×Ö·û´®×ª»»³ÉµÄ dword
	// nParam : PlayerIndex
	case SGDI_TONG_GET_TONG_NAMEID:
		{
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.GetTongNameID();
		}
		break;

	// µÇÂ½Ê±ºò»ñµÃ°ï»áÐÅÏ¢
	// uParam : ´«ÈëµÄ STONG_SERVER_TO_CORE_LOGIN point
	case SGDI_TONG_LOGIN:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_LOGIN	*pLogin = (STONG_SERVER_TO_CORE_LOGIN*)uParam;
			if (pLogin->m_dwParam <= 0 || pLogin->m_dwParam >= MAX_PLAYER)
				break;
			if (Player[pLogin->m_dwParam].m_nIndex <= 0)
				break;
			Player[pLogin->m_dwParam].m_cTong.Login(pLogin);
			Player[pLogin->m_dwParam].UpdataCurData();
			//update full status
			int nIdx = PlayerSet.GetFirstPlayer();
			while (nIdx)
			{
				if (Player[nIdx].m_cTong.GetTongNameID() == Player[pLogin->m_dwParam].m_cTong.m_dwTongNameID)
				{
					Player[nIdx].m_cTong.ChangeFullStatus(pLogin->m_bIsFull);
				}
				nIdx = PlayerSet.GetNextPlayer();
			}
		}
		break;
		
	// Í¨Öªcore·¢ËÍÄ³Íæ¼ÒµÄ°ï»áÐÅÏ¢
	// nParam : player index
	case SGDI_TONG_SEND_SELF_INFO:
		{
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			Player[nParam].m_cTong.SendSelfInfo();
		}
		break;

	case SGDI_TONG_CHANGE_TITLE_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_CHANGE_TITLE_COMMAND	*pChange = (TONG_APPLY_CHANGE_TITLE_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			
			nRet = Player[nParam].m_cTong.CheckChangeTitlePower(pChange);
		}
		break;
	case SGDI_TONG_GET_TITLE_POWER:
		if (uParam)
		{
			nRet = 0;
			STONG_SERVER_TO_CORE_CHECK_GET_TITLE_POWER	*pCheck = (STONG_SERVER_TO_CORE_CHECK_GET_TITLE_POWER*)uParam;
			if (pCheck->m_nPlayerIdx <= 0 || pCheck->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pCheck->m_nPlayerIdx].m_nIndex <= 0)
				break;
			nRet = Player[pCheck->m_nPlayerIdx].m_cTong.CheckGetTitlePower(pCheck);
		}
		break;
	case SGDI_TONG_CHANGE_SEX_TITLE_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_CHANGE_SEX_TITLE_COMMAND	*pChange = (TONG_APPLY_CHANGE_SEX_TITLE_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckChangeSexTitlePower(pChange);
		}
		break;

	case SGDI_TONG_BE_CHANGED_TITLE:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_BE_CHANGED_TITLE	*pSync = (STONG_SERVER_TO_CORE_BE_CHANGED_TITLE*)uParam;
			if (pSync->m_nPlayerIdx <= 0 || pSync->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pSync->m_nPlayerIdx].m_nIndex <= 0)
				break;
			Player[pSync->m_nPlayerIdx].m_cTong.BeChangedTitle(pSync);
		}
		break;

	case SGDI_TONG_MONEY_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_SAVE_COMMAND	*pSave = (TONG_APPLY_SAVE_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckMoneyPower(pSave);
		}
		break;
		
	case SGDI_TONG_CHANGE_MONEY:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_MONEY	*pChange = (STONG_SERVER_TO_CORE_MONEY*)uParam;
			switch(pChange->nType)
			{
			case 0://Save
				{
					int nIdx;
					Player[pChange->m_nPlayerIdx].Pay(pChange->m_nMoney);
					nIdx = PlayerSet.GetFirstPlayer();
					while (nIdx)
					{
						if (Player[nIdx].m_cTong.GetTongNameID() == pChange->m_dwTongNameID)
						{
							Player[nIdx].m_cTong.ChangeMoney(pChange->m_dwMoney);
						}
						nIdx = PlayerSet.GetNextPlayer();
					}
				}
				break;
			case 1://Rut
				{
					int nIdx;
					Player[pChange->m_nPlayerIdx].Earn(pChange->m_nMoney);
					nIdx = PlayerSet.GetFirstPlayer();
					while (nIdx)
					{
						if (Player[nIdx].m_cTong.GetTongNameID() == pChange->m_dwTongNameID)
						{
							Player[nIdx].m_cTong.ChangeMoney(pChange->m_dwMoney);
						}
						nIdx = PlayerSet.GetNextPlayer();
					}
				}
				break;
			case 2://Phat
				{
					int nIdx, nCount = 0;
					nIdx = PlayerSet.GetFirstPlayer();
					while (nIdx)
					{
						if (Player[nIdx].m_cTong.GetTongNameID() == pChange->m_dwTongNameID) //&& Player[nIdx].m_cTong.GetFigure() == enumTONG_FIGURE_MANAGER)
						{
							nCount++;
						}
						nIdx = PlayerSet.GetNextPlayer();
					}

					if (nCount)
					{
						int nMoney = pChange->m_nMoney/nCount;

						nIdx = PlayerSet.GetFirstPlayer();
						while (nIdx)
						{
							if (Player[nIdx].m_cTong.GetTongNameID() == pChange->m_dwTongNameID)// && Player[nIdx].m_cTong.GetFigure() == enumTONG_FIGURE_MANAGER)
							{
								Player[nIdx].Earn(nMoney);
							}
							nIdx = PlayerSet.GetNextPlayer();
						}
						Player[pChange->m_nPlayerIdx].Earn(pChange->m_nMoney - (nMoney * nCount));

						nIdx = PlayerSet.GetFirstPlayer();
						while (nIdx)
						{
							if (Player[nIdx].m_cTong.GetTongNameID() == pChange->m_dwTongNameID)
							{
								Player[nIdx].m_cTong.ChangeMoney(pChange->m_dwMoney);
							}
							nIdx = PlayerSet.GetNextPlayer();
						}
					}
				}
				break;
			default:
				break;
			}
		}
		break;
	case SGDI_TONG_CHANGE_FULL:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_FULL* pChange = (STONG_SERVER_TO_CORE_FULL*)uParam;
			int nIdx;
			nIdx = PlayerSet.GetFirstPlayer();
			while (nIdx)
			{
				if (Player[nIdx].m_cTong.GetTongNameID() == pChange->m_dwTongNameID)
				{
					Player[nIdx].m_cTong.ChangeFullStatus(pChange->m_bIsFull);
				}
				nIdx = PlayerSet.GetNextPlayer();
			}
		}
		break;
	case SGDI_TONG_APPLY_CHANGE_CAMP:
		if (uParam)
		{
			TONG_APPLY_CHANGE_CAMP_COMMAND	*pChange = (TONG_APPLY_CHANGE_CAMP_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
			{
				nRet = FALSE;
				break;
			}
			nRet = Player[nParam].m_cTong.CheckChangeCampCondition(pChange);
		}
		break;
	case SGDI_TONG_APPLY_CHANGE_RECRUIT:
		if (uParam)
		{
			TONG_APPLY_CHANGE_RECRUIT_COMMAND	*pChange = (TONG_APPLY_CHANGE_RECRUIT_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
			{
				nRet = FALSE;
				break;
			}
			nRet = Player[nParam].m_cTong.CheckChangeRecutCondition(pChange);
		}
		break;
	case SGDI_TONG_APPLY_CHANGE_LEVEL:
		if (uParam)
		{
			TONG_APPLY_CHANGE_LEVEL_COMMAND	*pChange = (TONG_APPLY_CHANGE_LEVEL_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
			{
				nRet = FALSE;
				break;
			}
			nRet = Player[nParam].m_cTong.CheckChangeLevelCondition(pChange);
		}
		break;
	case SGDI_TONG_APPLY_CHANGE_WAYEDIT:
		if (uParam)
		{
			TONG_APPLY_CHANGE_WAYEDIT_COMMAND	*pChange = (TONG_APPLY_CHANGE_WAYEDIT_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
			{
				nRet = FALSE;
				break;
			}
			nRet = Player[nParam].m_cTong.CheckChangeWayEditCondition(pChange);
		}
		break;
	case SGDI_TONG_APPLY_CHANGE_NEXTTARGET:
		if (uParam)
		{
			TONG_APPLY_CHANGE_NEXTTARGET_COMMAND	*pChange = (TONG_APPLY_CHANGE_NEXTTARGET_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
			{
				nRet = FALSE;
				break;
			}
			nRet = Player[nParam].m_cTong.CheckChangeNextTargetCondition(pChange);
		}
		break;
	case SGDI_TONG_BE_CHANGED_CAMP:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_BE_CHANGED_CAMP	*pSync = (STONG_SERVER_TO_CORE_BE_CHANGED_CAMP*)uParam;
			if (pSync->m_nPlayerIdx <= 0 || pSync->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pSync->m_nPlayerIdx].m_nIndex <= 0)
				break;
			Player[pSync->m_nPlayerIdx].m_cTong.BeChangedCamp(pSync);
		}
		break;

	case SGDI_TONG_BE_CHANGED_RECRUIT:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_BE_CHANGED_RECRUIT	*pSync = (STONG_SERVER_TO_CORE_BE_CHANGED_RECRUIT*)uParam;
			if (pSync->m_nPlayerIdx <= 0 || pSync->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pSync->m_nPlayerIdx].m_nIndex <= 0)
				break;
			Player[pSync->m_nPlayerIdx].m_cTong.BeChangedRecruit(pSync);
		}
		break;
	case SGDI_TONG_BE_CHANGED_LEVEL:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_BE_CHANGED_LEVEL* pSync = (STONG_SERVER_TO_CORE_BE_CHANGED_LEVEL*)uParam;
			if (pSync->m_nPlayerIdx <= 0 || pSync->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pSync->m_nPlayerIdx].m_nIndex <= 0)
				break;
			Player[pSync->m_nPlayerIdx].m_cTong.SetTongLevel(pSync->m_nLevel);
			Player[pSync->m_nPlayerIdx].UpdataCurData();
		}
		break;
	case SGDI_TONG_BE_CHANGED_EXP:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_BE_CHANGED_EXP* pSync = (STONG_SERVER_TO_CORE_BE_CHANGED_EXP*)uParam;
			if (pSync->m_nPlayerIdx <= 0 || pSync->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pSync->m_nPlayerIdx].m_nIndex <= 0)
				break;
			Player[pSync->m_nPlayerIdx].m_cTong.SetTongExp(pSync->m_nExp);
		}
		break;
	case SGDI_STOP_GAMESERVER:
		nRet = PlayerSet.GetStopGameServer();
		break;
	default:
		break;
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÏòÓÎÏ··¢ËÍ²Ù×÷
//	²ÎÊý£ºunsigned int uDataId --> CoreÍâ²¿¿Í»§¶ÔcoreµÄ²Ù×÷ÇëÇóµÄË÷Òý¶¨Òå
//							ÆäÖµÎªÃ·¾ÙÀàÐÍGAMEOPERATION_INDEXµÄÈ¡ÖµÖ®Ò»¡£
//		  unsigned int uParam  --> ÒÀ¾ÝuOperIdµÄÈ¡ÖµÇé¿ö¶ø¶¨
//		  int nParam --> ÒÀ¾ÝuOperIdµÄÈ¡ÖµÇé¿ö¶ø¶¨
//	·µ»Ø£ºÈç¹û³É¹¦·¢ËÍ²Ù×÷ÇëÇó£¬º¯Êý·µ»Ø·Ç0Öµ£¬·ñÔò·µ»Ø0Öµ¡£
//--------------------------------------------------------------------------
int	CoreServerShell::OperationRequest(unsigned int uOper, intptr_t uParam, int nParam)
{
	int nRet = 1;
	switch(uOper)
	{		
	case SSOI_BROADCASTING:
		nRet = PlayerSet.Broadcasting((char*)uParam, nParam);
		break;
	case SSOI_LAUNCH:	//Æô¶¯·þÎñ
		nRet = OnLunch((LPVOID)uParam);
		break;
	case SSOI_SHUTDOWN:	//¹Ø±Õ·þÎñ
		nRet = OnShutdown();
		break;
	case SSOI_RELOAD_WELCOME_MSG:
		PlayerSet.ReloadWelcomeMsg();
		break;

	case SSOI_TONG_JX2_SYNC:	// JX2 port: goi dong bo bang hoi tu relay
		if (uParam)
			g_TongJX2.OnRelayPacket((const void*)uParam, nParam);
		break;

	// ==== Bot la KPlayer that. Vo mong, chuyen thang sang KPlayerBot. ====
	case SSOI_PBOT_SET_SENDER:
		PB_SetSender((PB_DbSender)uParam);
		break;

	case SSOI_PBOT_SPAWN:
		if (uParam)
		{
			// nParam <= 1: sinh dung tai khoan do. nParam > 1: khong dung o day
			// (dai tai khoan di qua PB_SpawnRange tu Lua/lenh GM).
			return PB_Spawn((const char*)uParam);
		}
		break;

	case SSOI_PBOT_ROLELIST_RES:
		if (uParam)
			PB_OnRoleList((const PB_DB_RESULT*)uParam);
		break;

	case SSOI_PBOT_ROLEDATA_RES:
		if (uParam)
			PB_OnRoleData((const PB_DB_RESULT*)uParam);
		break;

	case SSOI_PBOT_REMOVE_ALL:
		return PB_RemoveAll();
	case SSOI_PBOT_SAVE_FAILED:
		// (18/08 phan bien) Goddess vut bai luu cua khe nay - neu la bot thi log + luu bu
		return PB_OnSaveFailed((int)uParam);

	// relay °ï»á´´½¨³É¹¦£¬Í¨Öª core ½øÐÐÏàÓ¦µÄ´¦Àí
	case SSOI_TONG_CREATE:
		{
			STONG_SERVER_TO_CORE_CREATE_SUCCESS	*pCreate = (STONG_SERVER_TO_CORE_CREATE_SUCCESS*)uParam;
			if (pCreate->m_nPlayerIdx <= 0 || pCreate->m_nPlayerIdx >= MAX_PLAYER)
			{
				nRet = 0;
				break;
			}
			if (Player[pCreate->m_nPlayerIdx].m_nIndex)
			{
				DWORD	dwID = g_FileName2Id(Npc[Player[pCreate->m_nPlayerIdx].m_nIndex].Name);
				if (dwID != pCreate->m_dwPlayerNameID)
				{
					nRet = 0;
					break;
				}
			}
			else
			{
				nRet = 0;
				break;
			}
			nRet = Player[pCreate->m_nPlayerIdx].CreateTong(pCreate->m_nCamp, pCreate->m_szTongName);
		}
		break;

	case SSOI_TONG_REFUSE_ADD:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_REFUSE_ADD	*pRefuse = (STONG_SERVER_TO_CORE_REFUSE_ADD*)uParam;
			if (pRefuse->m_nSelfIdx > 0 && pRefuse->m_nSelfIdx <= MAX_PLAYER)
			{
				Player[pRefuse->m_nSelfIdx].m_cTong.SendRefuseMessage(pRefuse->m_nTargetIdx, pRefuse->m_dwNameID);
			}
		}
		break;

	case SSOI_TONG_ADD:
		if (uParam)
		{
			nRet = 0;
			STONG_SERVER_TO_CORE_ADD_SUCCESS	*pAdd = (STONG_SERVER_TO_CORE_ADD_SUCCESS*)uParam;
			if (pAdd->m_nPlayerIdx <= 0 || pAdd->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pAdd->m_nPlayerIdx].m_nIndex <= 0)
				break;
			if (g_FileName2Id(Npc[Player[pAdd->m_nPlayerIdx].m_nIndex].Name) != pAdd->m_dwPlayerNameID)
				break;
			Player[pAdd->m_nPlayerIdx].m_cTong.AddTong(
				pAdd->m_nCamp,
				pAdd->m_szTongName,
				pAdd->m_szMasterName,
				pAdd->m_szTitleName);
			Player[pAdd->m_nPlayerIdx].m_cTong.SetTongLevel(pAdd->m_nTongLevel);
			Player[pAdd->m_nPlayerIdx].m_cTong.SetTongExp(pAdd->m_nTongExp);
			Player[pAdd->m_nPlayerIdx].UpdataCurData();
		}
		break;

	default:
		nRet = 0;
		break;
	}	
	return nRet;
}

int CoreServerShell::OnLunch(LPVOID pServer)
{
	g_SetServer(pServer);
	//g_SetFilePath("\\script");	

	KLuaScript * pStartScript =(KLuaScript*) g_GetScript("\\script\\startgame.lua");
	int i = 0;
	
	if (!pStartScript)
		g_DebugLog("Load ServerScript failed!");
	else
	{	
		pStartScript->CallFunction("OnGame",0,"");
	}

	PlayerSet.ReloadWelcomeMsg();

	pTimeScript =(KLuaScript*) g_GetScript("\\script\\timerserver.lua");

	return true;
}

int CoreServerShell::OnShutdown()
{
	return true;
}

int CoreServerShell::Breathe()
{
	if (pTimeScript)
	{
		if (!(g_SubWorldSet.GetGameTime() % GAME_FPS))
		{
			SYSTEMTIME aSysTime;
			GetSystemTime(&aSysTime);
			if(aSysTime.wSecond == 0)
			{
				pTimeScript->CallFunction("RunTime", 0, "");
			}
		//	else if(aSysTime.wSecond % 5 == 0) {
			//	pTimeScript->CallFunction("RunTimePUBG", 0, "");
			//}
		}
	}

	KJx2GlbMission_Breathe();	// DOT E: nhip timer GLOBAL mission (cong thanh 5')
	KJx2CityWar_Breathe();	// DOT E (E3): mot lan sau boot - ghi chu thanh/thue vao KSubWorld

	// ---- [SvPerf] nhiet ke KHUNG SERVER (chu game 18/08: "1000 bot rat lag,
	// can gan log xem loi do dau"). [BotPerf] chi do PB_Breathe (~9%/loi) nen
	// nguon lag nam ngoai no - do not tung tang moi 10 giay de chi mat.
	{
		static __int64 s_tSC = 0, s_tPB = 0, s_tMsg = 0, s_tMain = 0;
		static DWORD s_dwSvMoc = 0;
		static LARGE_INTEGER s_liF = { 0 };
		if (s_liF.QuadPart == 0)
			QueryPerformanceFrequency(&s_liF);
		LARGE_INTEGER t0, t1, t2, t3, t4;

		QueryPerformanceCounter(&t0);
		SC_Breathe();	// Port SimCity: nhip di chuyen bot
		QueryPerformanceCounter(&t1);
		PB_Breathe();	// Bot KPlayer that: rut hang doi sinh + het han cho
		QueryPerformanceCounter(&t2);
		g_SubWorldSet.MessageLoop();
		QueryPerformanceCounter(&t3);
		g_SubWorldSet.MainLoop();
		QueryPerformanceCounter(&t4);

		s_tSC   += t1.QuadPart - t0.QuadPart;
		s_tPB   += t2.QuadPart - t1.QuadPart;
		s_tMsg  += t3.QuadPart - t2.QuadPart;
		s_tMain += t4.QuadPart - t3.QuadPart;
		const DWORD dwNow = GetTickCount();
		if (s_dwSvMoc == 0)
			s_dwSvMoc = dwNow;
		else if (dwNow - s_dwSvMoc >= 10000 && s_liF.QuadPart > 0)
		{
			PB_LogNgoai("[SvPerf] 10s: SimCity %d ms | BotKPlayer %d ms |"
			            " MessageLoop %d ms | MainLoop(engine) %d ms\n",
			            (int)(s_tSC * 1000 / s_liF.QuadPart),
			            (int)(s_tPB * 1000 / s_liF.QuadPart),
			            (int)(s_tMsg * 1000 / s_liF.QuadPart),
			            (int)(s_tMain * 1000 / s_liF.QuadPart));
			s_tSC = s_tPB = s_tMsg = s_tMain = 0;
			s_dwSvMoc = dwNow;
		}
	}
	g_BauCua.run();
	return true;
}

bool CoreServerShell::CheckProtocolSize(const char* pChar, int nSize)
{
	WORD wCheckSize;
	BYTE nProtocol = (BYTE)pChar[0];

	if (nProtocol >= c2s_end || nProtocol <= c2s_gameserverbegin)
	{
		g_DebugLog("[error]NetServer:Invalid Protocol!");
		return false;
	}

	if (g_nProtocolSize[nProtocol - c2s_gameserverbegin - 1] == -1)
	{
		wCheckSize = *(WORD*)&pChar[1] + PROTOCOL_MSG_SIZE;
	}
	else
	{
		wCheckSize = g_nProtocolSize[nProtocol - c2s_gameserverbegin - 1];
	}
	if (wCheckSize != nSize && wCheckSize != nSize + 4) //4 bytes difference between 32bit client and 64bit server
														//this is temporary check
	{
		g_DebugLog("[error]ÍøÂç½ÓÊÕÐ­Òé´óÐ¡²»Æ¥Åä");
#ifndef _WIN32
		printf("[error]ÍøÂç½ÓÊÕÐ­Òé´óÐ¡²»Æ¥Åä<%d>, should %d, but %d\n", nProtocol, wCheckSize, nSize);
#endif
		return false;
	}
	return true;
}

int CoreServerShell::AttachPlayer(const unsigned long lnID, GUID* pGuid, char* sHWID)
{
	return PlayerSet.AttachPlayer(lnID, pGuid, sHWID);
}

void* CoreServerShell::SavePlayerDataAtOnce(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
	{
		return NULL;
	}

	if (Player[nIndex].Save())
	{
		Player[nIndex].m_uMustSave = SAVE_REQUEST;
		// (18/08) Bot KPlayer: dong dau PB_BLOB_DAU vao truong chet irevivaly de
		// phien sau PB_OnRoleData nhan ra "bot cu" (giu ky nang/phai/vi tri).
		// Nguoi that luon ghi 0 vao truong nay (KPlayerDBFuns.cpp SavePlayerBaseInfo)
		// nen khong dung ai. Dong dau TRUOC khi GameServer tinh CRC nen CRC van khop.
		if (PB_IsBot(nIndex))
			((TRoleData*)Player[nIndex].m_SaveBuffer)->BaseInfo.irevivaly = PB_BLOB_DAU;
		return &Player[nIndex].m_SaveBuffer;
	}
	else
	{
		return NULL;
	}
}

bool CoreServerShell::IsCharacterQuiting(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
	{
		return FALSE;
	}	

	return Player[nIndex].IsWaitingRemove();
}

//implement IsCharacterNeedSave
bool CoreServerShell::IsCharacterNeedSave(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
	{
		return FALSE;
	}
	return Player[nIndex].IsPlayerSaveEnabled();
}

BYTE CoreServerShell::GetCharacterLixian(int nIndex)//#uy thac
{
	if(nIndex <= 0 || nIndex >= MAX_PLAYER)
		return FALSE;

	return Player[nIndex].m_nLixian;
}

int CoreServerShell::SetCharacterLixianEnd(const char* pAccName)//#uy thac
{
	int nIndex = PlayerSet.GetFirstPlayer();
	while(nIndex > 0)
	{
		if (strcmpi(Player[nIndex].m_AccoutName, pAccName) == 0 && Player[nIndex].m_nLixian == 2) //#uy thac
		{
			Player[nIndex].m_nLixian = 0;
			Player[nIndex].m_bIsQuiting = TRUE;
			printf("=> Account [%s] Player [%s] ket thuc uy thac <= \n", Player[nIndex].m_AccoutName, Player[nIndex].m_PlayerName);
			return Player[nIndex].m_nNetConnectIdx;
		}
		nIndex = PlayerSet.GetNextPlayer();
	}
	return -1;
}

void CoreServerShell::SetCharacterLixianCompleted(int nIndex)//#uy thac
{
	if(nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	Player[nIndex].m_nLixian = 2;	//®· th«ng b¸o cho s3relay tho¸t ra ®Ó uû th¸c kh«ng tÝnh limit n÷a
	Player[nIndex].m_bIsQuiting = FALSE;
}

bool CoreServerShell::IsPlayerLoginTimeOut(int nIndex)
{
	return Player[nIndex].IsLoginTimeOut();
}

void CoreServerShell::RemovePlayerLoginTimeOut(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].IsLoginTimeOut())
	{
		PlayerSet.RemoveLoginTimeOut(nIndex);
	}
}

int CoreServerShell::AddTempTaskValue(int nIndex, const char* pData)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return 0;

	return Player[nIndex].AddTempTaskValue((void *)pData);
}

void CoreServerShell::GetPlayerIndexByGuid(GUID* pGuid, int* pnIndex, int* plnID)
{
	*pnIndex = PlayerSet.GetPlayerIndexByGuid(pGuid);
	if (*pnIndex)
	{
		*plnID = Player[*pnIndex].m_nNetConnectIdx;
	}
	else
	{
		*plnID = -1;
	}

	if (*plnID == -1)
	{
		*pnIndex = 0;
	}
}

void* CoreServerShell::PreparePlayerForExchange(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return NULL;
	PlayerSet.PrepareExchange(nIndex);
	return &Player[nIndex].m_SaveBuffer;
}

bool CoreServerShell::IsPlayerExchangingServer(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return false;

	return Player[nIndex].IsExchangingServer();
}

void CoreServerShell::RemovePlayerForExchange(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	PlayerSet.RemoveExchanging(nIndex);
}

void CoreServerShell::RemovePlayerForLimit(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	BYTE	NetCommand = (BYTE)s2c_exitgame;
	g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &NetCommand, sizeof(BYTE));
}

void CoreServerShell::GetGuid(int nIndex, void* pGuid)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	memcpy(pGuid, &Player[nIndex].m_Guid, sizeof(GUID));
}

DWORD CoreServerShell::GetExchangeMap(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return -1;

	return Player[nIndex].m_sExchangePos.m_dwMapID;
}

void CoreServerShell::RecoverPlayerExchange(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	//
	Player[nIndex].m_bExchangeServer = FALSE;
	if (Player[nIndex].m_nIndex > 0)
	{
		KNpc* pNpc = &Npc[Player[nIndex].m_nIndex];
		pNpc->m_bExchangeServer = FALSE;
		pNpc->m_FightMode = pNpc->m_OldFightMode;
		
		//Khong chuyen duoc map ve Ba Lang Huyen
		/*
		int arrSubWorldID[10] = {20,53,54,99,100,101,121,153,174,175};
		for(int i=0;i< 10 ;i++)
		{
			int nTargetSubWorld = g_SubWorldSet.SearchWorld(arrSubWorldID[i]);
			if(nTargetSubWorld!= -1)
			{
				switch(arrSubWorldID[i])
				{
					case 20:
						pNpc->ChangeWorld(20,3552*32,6194*32);
						break;
					case 53:
						pNpc->ChangeWorld(53,1622*32,3189*32);
						break;
					case 54:
						pNpc->ChangeWorld(54,1650*32,3169*32);
						break;
					case 99:
						pNpc->ChangeWorld(99,1628*32,3203*32);
						break;
					case 100:
						pNpc->ChangeWorld(100,1628*32,3203*32);
						break;
					case 101:
						pNpc->ChangeWorld(101,1688*32,3154*32);
						break;
					case 121:
						pNpc->ChangeWorld(121,1951*32, 4509*32);
						break;
					case 153:
						pNpc->ChangeWorld(153,1605*32,3220*32);
						break;
					case 174:
						pNpc->ChangeWorld(174,1573*32,3203*32);
						break;
					case 175:
						pNpc->ChangeWorld(175,1673*32,3168*32);
						break;
				}
				pNpc->SetFightMode(FALSE);
				break;
			}
		}
		*/
	}
	Player[nIndex].Earn(Player[nIndex].m_nPrePayMoney);
	Player[nIndex].m_nPrePayMoney = 0;
}

void CoreServerShell::SetSaveStatus(int nIndex, UINT uStatus)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	Player[nIndex].m_uMustSave = uStatus;
}

UINT CoreServerShell::GetSaveStatus(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return SAVE_IDLE;

	return Player[nIndex].m_uMustSave;
}

void CoreServerShell::PreparePlayerForLoginFailed(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	PlayerSet.PrepareLoginFailed(nIndex);
}

BOOL CoreServerShell::GroupChat(IClient* pClient, DWORD FromIP, unsigned long FromRelayID, DWORD channid, BYTE tgtcls, DWORD tgtid, const void* pData, size_t size)
{
	switch(tgtcls)
	{
	case tgtcls_team:
		{{
		if (tgtid < 0 || tgtid >= MAX_TEAM)
			return FALSE;
		size_t pckgsize = sizeof(tagExtendProtoHeader) + size;
#ifdef WIN32
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
#else
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)(new char[pckgsize]);
#endif
		pExHeader->ProtocolType = s2c_extendchat;
		pExHeader->wLength = pckgsize - 1;
		memcpy(pExHeader + 1, pData, size);
		int nTargetIdx;
		nTargetIdx = g_Team[tgtid].m_nCaptain;
//		if (FromRelayID != Player[nTargetIdx].m_nNetConnectIdx)
			g_pServer->SendData(Player[nTargetIdx].m_nNetConnectIdx, pData, size);
		for (int i = 0; i <	MAX_TEAM_MEMBER; i++)
		{
			nTargetIdx = g_Team[tgtid].m_nMember[i];
			if (nTargetIdx < 0)
				continue;
//			if (FromRelayID != Player[nTargetIdx].m_nNetConnectIdx)
				g_pServer->PackDataToClient(Player[nTargetIdx].m_nNetConnectIdx, pExHeader, pckgsize);
		}
#ifndef WIN32
		delete ((char*)pExHeader);
#endif
		}}
		break;
	case tgtcls_fac:
		{{
		size_t pckgsize = sizeof(tagExtendProtoHeader) + size;
#ifdef WIN32
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
#else
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)(new char[pckgsize]);
#endif
		pExHeader->ProtocolType = s2c_extendchat;
		pExHeader->wLength = pckgsize - 1;
		memcpy(pExHeader + 1, pData, size);
		int nTargetIdx;
		nTargetIdx = PlayerSet.GetFirstPlayer();
		while (nTargetIdx)
		{
			if (Player[nTargetIdx].m_cFaction.m_nCurFaction == tgtid
			)//				&& FromRelayID != Player[nTargetIdx].m_nNetConnectIdx)
				g_pServer->PackDataToClient(Player[nTargetIdx].m_nNetConnectIdx, pExHeader, pckgsize);

			nTargetIdx = PlayerSet.GetNextPlayer();
		}
#ifndef WIN32
		delete ((char*)pExHeader);
#endif
		}}
		break;

	case tgtcls_tong:
		{{
		size_t pckgsize = sizeof(tagExtendProtoHeader) + size;
#ifdef WIN32
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
#else
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)(new char[pckgsize]);
#endif
		pExHeader->ProtocolType = s2c_extendchat;
		pExHeader->wLength = pckgsize - 1;
		memcpy(pExHeader + 1, pData, size);

		int nTargetIdx;
		nTargetIdx = PlayerSet.GetFirstPlayer();
		while (nTargetIdx)
		{
			if (Player[nTargetIdx].m_cTong.GetTongNameID() == tgtid
			)//				&& FromRelayID != Player[nTargetIdx].m_nNetConnectIdx)
				g_pServer->PackDataToClient(Player[nTargetIdx].m_nNetConnectIdx, pExHeader, pckgsize);

			nTargetIdx = PlayerSet.GetNextPlayer();
		}
#ifndef WIN32
		delete ((char*)pExHeader);
#endif
		}}
		break;
	case tgtcls_msgr:
	{
		{
			size_t pckgsize = sizeof(tagExtendProtoHeader) + size;
#ifdef WIN32
			tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
#else
			tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)(new char[pckgsize]);
#endif
			pExHeader->ProtocolType = s2c_extendchat;
			pExHeader->wLength = pckgsize - 1;
			memcpy(pExHeader + 1, pData, size);

			int nTargetIdx;
			nTargetIdx = PlayerSet.GetFirstPlayer();
			while (nTargetIdx)
			{
				if (Npc[Player[nTargetIdx].m_nIndex].m_nMissionGroup == tgtid)
					g_pServer->PackDataToClient(Player[nTargetIdx].m_nNetConnectIdx, pExHeader, pckgsize);
				nTargetIdx = PlayerSet.GetNextPlayer();
			}
#ifndef WIN32
			delete ((char*)pExHeader);
#endif
		}
	}
	break;
	case tgtcls_scrn:
		{{
//		int nMaxRelayPlayer = (1024 - 32 - sizeof(CHAT_GROUPMAN) - size) / sizeof(WORD);
//		if (nMaxRelayPlayer <= 0)
//			return FALSE;
		int idxNPC = Player[tgtid].m_nIndex;
		int idxSubWorld = Npc[idxNPC].m_SubWorldIndex;
		int idxRegion = Npc[idxNPC].m_RegionIndex;
//		_ASSERT(idxSubWorld >= 0 && idxRegion >= 0);
		int nOX = Npc[idxNPC].m_MapX;
		int nOY = Npc[idxNPC].m_MapY;
		int nTX = 0;
		int nTY = 0;
		if (idxSubWorld < 0 || idxRegion < 0)
			return FALSE;
//		size_t basesize = sizeof(CHAT_GROUPMAN) + size;
//		BYTE buffer[1024];
//
//		CHAT_GROUPMAN* pCgc = (CHAT_GROUPMAN*)buffer;
//		pCgc->ProtocolType = chat_groupman;
//		pCgc->wChatLength = size;
//		pCgc->byHasIdentify = false;
//
//		void* pExPckg = pCgc + 1;
//		memcpy(pExPckg, pData, size);
//
//		WORD* pPlayers = (WORD*)((BYTE*)pExPckg + size);
//
//
//		pCgc->wPlayerCount = 0;
		size_t pckgsize = sizeof(tagExtendProtoHeader) + size;
#ifdef WIN32
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
#else
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)(new char[pckgsize]);
#endif
		pExHeader->ProtocolType = s2c_extendchat;
		pExHeader->wLength = pckgsize - 1;
		memcpy(pExHeader + 1, pData, size);

		#define	MAX_SYNC_RANGE	23
		static POINT POff[9] = 
		{
			{0, 0},
			{0, 32},
			{-16, 32},
			{-16, 0},
			{-16, -32},
			{0, -32},
			{16, -32},
			{16, 0},
			{16, 32},
		};

		KRegion* pRegionBase = &SubWorld[idxSubWorld].m_Region[idxRegion];

		for (int i = -1; i < 8; i++)
		{
			KRegion* pRegion = NULL;
			if (i < 0)
				pRegion = pRegionBase;
			else
			{
				if (pRegionBase->m_nConnectRegion[i] < 0)
					continue;
				pRegion = &SubWorld[idxSubWorld].m_Region[pRegionBase->m_nConnectRegion[i]];
			}
			if (pRegion == NULL)
				continue;


			KIndexNode *pNode = (KIndexNode *)pRegion->m_PlayerList.GetHead();
			while(pNode)
			{
//				_ASSERT(pNode->m_nIndex > 0 && pNode->m_nIndex < MAX_PLAYER);

				//if (FromRelayID != Player[pNode->m_nIndex].m_nNetConnectIdx)
				{
					int nTargetNpc = Player[pNode->m_nIndex].m_nIndex;
					if (nTargetNpc > 0)
					{
						nTX = Npc[nTargetNpc].m_MapX + POff[i + 1].x;
						nTY = Npc[nTargetNpc].m_MapY + POff[i + 1].y;
						
						if ((nTX - nOX) * (nTX - nOX) + (nTY - nOY) * (nTY - nOY) < MAX_SYNC_RANGE * MAX_SYNC_RANGE)
							g_pServer->PackDataToClient(Player[pNode->m_nIndex].m_nNetConnectIdx, pExHeader, pckgsize);
					}

//					pPlayers[pCgc->wPlayerCount] = (WORD)Player[pNode->m_nIndex].m_nNetConnectIdx;
//					++ pCgc->wPlayerCount;
//
//					if (pCgc->wPlayerCount >= nMaxRelayPlayer)
//					{
//						size_t pckgsize = basesize + sizeof(WORD) * pCgc->wPlayerCount;
//						pCgc->wSize = pckgsize - 1;
//						
//						pClient->SendPackToServer(pCgc, pckgsize);
//
//						pCgc->wPlayerCount = 0;
//					}
				}

				pNode = (KIndexNode *)pNode->GetNext();
			}
		}

//		if (pCgc->wPlayerCount > 0)
//		{
//			size_t pckgsize = basesize + sizeof(WORD) * pCgc->wPlayerCount;
//			pCgc->wSize = pckgsize - 1;
//
//			pClient->SendPackToServer(pCgc, pckgsize);
//		}

#ifndef WIN32
		delete (char*)pExHeader;
#endif
		}}
		break;
	case tgtcls_bc:
		{{
		size_t pckgsize = sizeof(tagExtendProtoHeader) + size;
#ifdef WIN32
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
#else
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)(new char[pckgsize]);
#endif
		pExHeader->ProtocolType = s2c_extendchat;
		pExHeader->wLength = pckgsize - 1;
		memcpy(pExHeader + 1, pData, size);

		int nTargetIdx;
		nTargetIdx = PlayerSet.GetFirstPlayer();
		while (nTargetIdx)
		{
			g_pServer->PackDataToClient(Player[nTargetIdx].m_nNetConnectIdx, pExHeader, pckgsize);

			nTargetIdx = PlayerSet.GetNextPlayer();
		}
#ifndef WIN32
		delete ((char*)pExHeader);
#endif
		}}
		break;


	default:
		break;
	}
	return TRUE;
}

void CoreServerShell::SetLadder(void* pData, size_t uSize)
{
	Ladder.Init(pData, uSize);
}

BOOL CoreServerShell::PayForSpeech(int nIndex, int nType)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return FALSE;
	

	int	nMoney = 0;
	int nNpcIdx = Player[nIndex].m_nIndex;
	int Map = SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_SubWorldID;

	if (Map == 209)
		return FALSE;

	if (nNpcIdx <= 0)
		return FALSE;
	if (Player[nIndex].m_nForbiddenFlag & KPlayer::FF_CHAT)	//cÊm chat, cam chat
		return FALSE;
	int nLevel = Npc[nNpcIdx].m_Level;
	int nMaxMana = Npc[nNpcIdx].m_CurrentManaMax;

	switch (nType)
	{
	case 0:		//Ãâ·Ñ
		return TRUE;
		break;
	case 1:		//10ÔªÃ¿¾ä
		{
			nMoney = 10;
			return Player[nIndex].Pay(nMoney);
		}
		break;
	case 2:		//2: <10Lv ? ²»ÄÜËµ : MaxMana/2/¾ä
		{
			if (nLevel < 10)
				return FALSE;
			return Npc[nNpcIdx].Cost(attrib_mana_v, nMaxMana / 2);
		}
		break;
	case 3:		//3: MaxMana/10/¾ä
		{
			return Npc[nNpcIdx].Cost(attrib_mana_v, nMaxMana / 10);
		}
		break;
	case 4:		//4: <20Lv ? ²»ÄÜËµ : MaxMana*4/5/¾ä
		{
			if (nLevel < 20)
				return FALSE;
			return Npc[nNpcIdx].Cost(attrib_mana_v, nMaxMana * 4 / 5);
		}
		break;
	default:
		return FALSE;	//²»ÈÏÊ¶µÄÀà±ð²»·¢ËÍ
	}
}

void CoreServerShell::SetExtPoint(int nIndex, int nExtPoint)
{
 	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
 	{
 		return;
 	}
	Player[nIndex].SetExtPoint(nExtPoint, 0);
}