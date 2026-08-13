#ifndef KMISSION_H
#define KMISSION_H

#ifdef _SERVER
#include "KCore.h"
#include "KEngine.h"
#include "KLinkArrayTemplate.h"
#include "KTaskFuns.h"
#include "KPlayerDef.h"
#include "KSubWorld.h"
#include "KSubWorldSet.h"
#include "KRegion.h"
#include "KNpc.h"

#include "KMissionArray.h"
#include "KPlayerChat.h"
#include "KSortScript.h"
#include "TaskDef.h"

extern int g_PlayerTimerCallBackFun(void * pOwner, char * szScriptFileName);
extern int g_MissionTimerCallBackFun(void * pOwner, char * szScriptFileName);

#define MAX_TIMER_PERMISSION		10
#define MAX_PLAYER_MISSION			  600
#define MAX_NPC_MISSION					   5000

typedef struct 
{
	unsigned long m_ulPlayerIndex;
	unsigned long m_ulPlayerID;
	unsigned char m_ucPlayerGroup;
	unsigned long m_ulJoinTime;
	int			  m_nParam[MAX_MISSION_PARAM];
}TMissionPlayerInfo;

typedef struct 
{
	unsigned long m_ulNpcIndex;
	unsigned long m_ulNpcID;
	unsigned long m_ucNpcGroup;
	unsigned long m_ulJoinTime;
}TMissionNpcInfo;

template<class T , unsigned long ulSize>
class _KMissionPlayerArray:public KLinkArrayTemplate<T , ulSize>
{
public:
	unsigned long FindSame(T *pT)
	{
		int nIdx = 0;
		while(1)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
			if (!nIdx)
				break;
			if (pT->m_ulPlayerID == m_Data[nIdx].m_ulPlayerID)
			{
				m_Data[nIdx].m_ulPlayerIndex = pT->m_ulPlayerIndex;
				return nIdx;
			}
		}
		return 0;
	};

	int GetParam(unsigned long ulIndex, int nParam) const
	{
		if (ulIndex >= ulSize) 
			return 0;
		return m_Data[ulIndex].m_nParam[nParam];
	};

	BOOL SetParam(unsigned long ulIndex, int nParam, int nValue)
	{
		if (ulIndex >= ulSize) 
			return FALSE;
		
		m_Data[ulIndex].m_nParam[nParam] = nValue;
		if (nParam == MISSION_PARAM_AVAILABLE)
		{
			if (m_Data[ulIndex].m_ulPlayerIndex >= 0 && m_Data[ulIndex].m_ulPlayerIndex < MAX_PLAYER) {
				if (Player[m_Data[ulIndex].m_ulPlayerIndex].m_nIndex >= 0 && Player[m_Data[ulIndex].m_ulPlayerIndex].m_nIndex < MAX_NPC) {
					Npc[Player[m_Data[ulIndex].m_ulPlayerIndex].m_nIndex].m_nMissionGroup = nValue == MISSION_AVAILABLE_VALUE ? (int)m_Data[ulIndex].m_ucPlayerGroup : -1;
					Player[m_Data[ulIndex].m_ulPlayerIndex].SendMSGroup();
				}
				else
					return FALSE;
			}
			else
				return FALSE;
		}
		return TRUE;
	};
};

template<class T , unsigned long ulSize>
class _KMissionNpcArray:public KLinkArrayTemplate<T , ulSize>
{
public:
	unsigned long FindSame(T *pT)
	{
		int nIdx = 0;
		while(1)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
			if (!nIdx)
				break;
			
			
			if (pT->m_ulNpcID == m_Data[nIdx].m_ulNpcID)
			{
				m_Data[nIdx].m_ulNpcIndex = pT->m_ulNpcIndex;
				return nIdx;
			}
		}
		return 0;
	};
};

typedef _KMissionPlayerArray<TMissionPlayerInfo, MAX_PLAYER> KMissionPlayerArray;
typedef _KMissionNpcArray<TMissionNpcInfo, MAX_NPC_MISSION> KMissionNpcArray;
typedef KTimerFunArray <KTimerTaskFun, MAX_TIMER_PERMISSION, g_MissionTimerCallBackFun> KMissionTimerArray;
#define MAX_MISSION_VALUE_COUNT 100

class KMission
{
	char	m_MissionValue[MAX_MISSIONARRAY_VALUE_COUNT][16];
	unsigned long m_ulMissionId;
	bool m_bMissionLadder;//add by phong kiÒu using tèng kim
	char m_szMissionName[64];
	int	m_nLadderParam;
	int	m_nGlbLadderParam[MAX_GLBMISSION_PARAM];
	void * m_pOwner;
public:
	KMissionTimerArray	m_cTimerTaskSet;
	KMission()
	{
		m_pOwner = NULL;
		for (int i = 1; i < MAX_TIMER_PERMISSION; i ++)
		{
			KTimerTaskFun * pTimer = (KTimerTaskFun*)m_cTimerTaskSet.GetData(i);
			if (pTimer)
			pTimer->SetOwner(this);
		}
//		memset(m_MissionValueC, 0, sizeof(m_MissionValueC));
		memset(m_MissionValue, 0, sizeof(m_MissionValue));
		m_ulMissionId = 0;
		m_bMissionLadder = false;//add by phong kiÒu using tèng kim
		m_szMissionName[0] = 0;
		m_nLadderParam = 0;
		memset(m_nGlbLadderParam, 0, sizeof(m_nGlbLadderParam));

	};
	BOOL	Activate();
	
	void SetOwner(void * pOwner)
	{
		m_pOwner = pOwner;
	};
	
	void * GetOwner()
	{
		return m_pOwner;
	};
	
	void	Init()
	{
//		memset(m_MissionValueC, 0, sizeof(m_MissionValueC));
		memset(m_MissionValue, 0, sizeof(m_MissionValue));
		m_ulMissionId = 0;
		m_bMissionLadder = false;//add by phong kiÒu using tèng kim
		memset(m_szMissionName, 0, sizeof(m_szMissionName));
		m_nLadderParam = 0;
		memset(m_nGlbLadderParam, 0, sizeof(m_nGlbLadderParam));
	};

	BOOL SetMissionId(unsigned long ulMissionId)
	{
		m_ulMissionId = ulMissionId;
		return TRUE;
	}

	void SetMissionLadder(const char* szName, int nLadderParam, int* nGlbParam)//add by phong kiÒu using tèng kim
	{
		m_bMissionLadder=true;
		memcpy(m_szMissionName, szName, strlen(szName));
		m_nLadderParam = nLadderParam;
		for(int i=0;i<MAX_GLBMISSION_PARAM;i++)
		{
			m_nGlbLadderParam[i] = nGlbParam[i];
		}
	}

	unsigned long GetMissionId()
	{
		return m_ulMissionId;
	}

	const char* GetMissionName()//add by phong kiÒu using tèng kim
	{
		return m_szMissionName;
	}

	int GetMissionLadderParam()
	{
		return m_nLadderParam;
	}
	
	TMissionLadderInfo* GetMin(TMissionLadderInfo* const aMSLDList,const int n);

	void ListSort(TMissionLadderInfo* const aMSLDList,const int n);

	void DataCopy(TMissionLadderInfo* Desc, TMissionPlayerInfo* Source);

	unsigned long Msg2Group(const char * strMsg, unsigned char ucGroup)
	{
		if (!strMsg || !strMsg[0]) 
			return 0;
		int nCount = 0;
		int nIdx = 0;
		while(1)
		{
			nIdx =	m_MissionPlayer.m_UseIdx.GetNext(nIdx);
			if (!nIdx)
				break;
			
			if ((m_MissionPlayer.m_Data[nIdx].m_ucPlayerGroup == ucGroup) && 
			   (m_MissionPlayer.m_Data[nIdx].m_nParam[MISSION_PARAM_AVAILABLE] == MISSION_AVAILABLE_VALUE))		// co online
			{
				KPlayerChat::SendSystemInfo(1, m_MissionPlayer.m_Data[nIdx].m_ulPlayerIndex, MESSAGE_SYSTEM_ANNOUCE_HEAD, (char *) strMsg, strlen(strMsg));
				nCount ++;
			}
		}
		return nCount;
	}

	unsigned long Msg2All(const char * strMsg)
	{
		if (!strMsg || !strMsg[0]) 
			return 0;
		int nCount = 0;
		int nIdx = 0;
		while(1)
		{
			nIdx =	m_MissionPlayer.m_UseIdx.GetNext(nIdx);
			if (!nIdx)
				break;
			
			if (m_MissionPlayer.m_Data[nIdx].m_nParam[MISSION_PARAM_AVAILABLE] == MISSION_AVAILABLE_VALUE)
			{
				KPlayerChat::SendSystemInfo(1, m_MissionPlayer.m_Data[nIdx].m_ulPlayerIndex, MESSAGE_SYSTEM_ANNOUCE_HEAD, (char *) strMsg, strlen(strMsg));
				nCount ++;
			}
		}
		return nCount;
	}
	unsigned long Msg2Player(const char * strMsg, unsigned long ulPlayerIndex)
	{
		if (!strMsg || !strMsg[0]) 
			return 0;
		KPlayerChat::SendSystemInfo(1, ulPlayerIndex, MESSAGE_SYSTEM_ANNOUCE_HEAD, (char *) strMsg, strlen(strMsg));
	};

	unsigned long GetNextPlayerC(unsigned long ulIdx, unsigned char ucGroup, unsigned long &ulPlayerIndex)
	{
		ulPlayerIndex = 0;
		while(1)
		{
			ulIdx =	m_MissionPlayer.m_UseIdx.GetNext(ulIdx);
			if (ulIdx == 0) 
				return 0;
			
			if (ucGroup)
			{
				if ((m_MissionPlayer.m_Data[ulIdx].m_ucPlayerGroup == ucGroup) && 
				   (m_MissionPlayer.m_Data[ulIdx].m_nParam[MISSION_PARAM_AVAILABLE] == MISSION_AVAILABLE_VALUE)) 
				{
					ulPlayerIndex = m_MissionPlayer.m_Data[ulIdx].m_ulPlayerIndex;
					return ulIdx;
				}
			}
			else
			{
				if (m_MissionPlayer.m_Data[ulIdx].m_nParam[MISSION_PARAM_AVAILABLE] == MISSION_AVAILABLE_VALUE)
				{
					ulPlayerIndex = m_MissionPlayer.m_Data[ulIdx].m_ulPlayerIndex;
					return ulIdx;
				}
			}
		}
		return 0;
	}

	unsigned long GetNextPlayer(unsigned long ulIdx, unsigned long &ulPlayerIndex)
	{
		ulPlayerIndex = 0;
		while(1)
		{
			ulIdx =	m_MissionPlayer.m_UseIdx.GetNext(ulIdx);
			if (ulIdx == 0) 
				return 0;

			ulPlayerIndex = m_MissionPlayer.m_Data[ulIdx].m_ulPlayerIndex;	
			return ulIdx;
		}
		return 0;
	}


	unsigned long GetNextNpc(unsigned long ulIdx, unsigned long &ulNpcIndex)
	{
		ulNpcIndex = 0;
		while(1)
		{
			ulIdx =	m_MissionNpc.m_UseIdx.GetNext(ulIdx);
			if (ulIdx == 0) 
				return 0;

			ulNpcIndex = m_MissionNpc.m_Data[ulIdx].m_ulNpcIndex;	
			return ulIdx;
		}
		return 0;
	}
	
	unsigned long GetPlayerCount() const
	{ 
		return m_MissionPlayer.GetUsedCount();
	};

	unsigned long GetNpcCount() const
	{ 
		return m_MissionNpc.GetUsedCount();
	};

	void SetMission(unsigned long ulValueId, const char* szValue)
	{
		if (ulValueId >= MAX_MISSIONARRAY_VALUE_COUNT)
			return ;
		strcpy(m_MissionValue[ulValueId], szValue);
	};

	unsigned long GetGroupPlayerCount (unsigned char ucGroup ) const 
	{
		//if (ucGroup == 0)
		//return GetPlayerCount();

		unsigned long ulPlayerCount = 0;
		int nIdx = 0;
		while(1)
		{
			nIdx = m_MissionPlayer.m_UseIdx.GetNext(nIdx);
			if (!nIdx)
				break;

			if (ucGroup >= 0)
			{
				if ((m_MissionPlayer.m_Data[nIdx].m_ucPlayerGroup == ucGroup) && 
					(m_MissionPlayer.m_Data[nIdx].m_nParam[MISSION_PARAM_AVAILABLE] == MISSION_AVAILABLE_VALUE))
					ulPlayerCount ++;
			}
		}
		return ulPlayerCount;
	};

	unsigned long GetOnlinePlayerCount () const 
	{
		unsigned long ulPlayerCount = 0;
		int nIdx = 0;
		while(1)
		{
			nIdx = m_MissionPlayer.m_UseIdx.GetNext(nIdx);
			if (!nIdx)
				break;
			
			if (m_MissionPlayer.m_Data[nIdx].m_nParam[MISSION_PARAM_AVAILABLE] == MISSION_AVAILABLE_VALUE)	// them dk co online
				ulPlayerCount ++;
		}
		return ulPlayerCount;
	};
		
	int GetMissionValue(unsigned long ulValueId) const
	{
		if (ulValueId >= MAX_MISSION_VALUE_COUNT)
			return 0;
		int nResult = 0;
		if (m_MissionValue[ulValueId][0])
			nResult = atoi(m_MissionValue[ulValueId]);
		return nResult;
	};

	const char* GetMissionString(unsigned long ulValueId) const
	{
		if (ulValueId >= MAX_MISSIONARRAY_VALUE_COUNT)
			return "";
		return m_MissionValue[ulValueId];
	};

	BOOL	ExecuteScript(char * ScriptFileName, char * szFunName, int nParam);
	BOOL	ExecuteScript(DWORD dwScriptId,  char * szFunName, int nParam);
	BOOL StartTimer(unsigned char ucTimerId, unsigned long ulTimerInterval)
	{
		KTimerTaskFun Timer;
		Timer.SetTimer(ulTimerInterval, ucTimerId);
		Timer.SetOwner(this);
		m_cTimerTaskSet.Add(&Timer);
		return TRUE;
	}

	BOOL StopTimer(unsigned char ucTimerId)
	{
		KTimerTaskFun Timer;
		Timer.SetTimer(1, ucTimerId);
		unsigned long ulIndex = 0;
		KTimerTaskFun * pTimer = m_cTimerTaskSet.GetData(&Timer);
		if (pTimer)
			pTimer->CloseTimer();
		
	}

	DWORD GetTimerRestTimer(unsigned char ucTimerId)
	{
		KTimerTaskFun Timer;
		Timer.SetTimer(1, ucTimerId);
		unsigned long ulIndex = 0;
		KTimerTaskFun * pTimer = m_cTimerTaskSet.GetData(&Timer);
		if (pTimer)
			return pTimer->GetRestTime();
		return 0;
	}

	BOOL StopMission();
	
	KMissionPlayerArray m_MissionPlayer;
	KMissionNpcArray m_MissionNpc;
	TMissionLadderInfo m_MissionLadder[MISSION_STATNUM];//add by phong kiÒu using tèng kim

	unsigned long	AddPlayer(unsigned long ulPlayerIndex, unsigned long ulPlayerID, unsigned char ucPlayerGroup, int ulJoinTime = 0);
	BOOL	RemovePlayer(unsigned long ulPlayerIndex, unsigned long ulPlayerID);
	unsigned long	AddNpc(unsigned long ulNpcIndex, unsigned long ulNpcID, unsigned char ucNpcGroup, int ulJoinTime = 0);
	BOOL	RemoveNpc(unsigned long ulNpcIndex, unsigned long ulNpcID = 0);
	void 			SetPlayerParam(unsigned long ulIndex, int nParam, int nValue); //add by phong kiÒu using tèng kim
    void    UpRankAllParam(int nParam);
	unsigned long GetMissionPlayer_DataIndex(unsigned long ulPlayerIndex, unsigned long ulPlayerID)
	{
		if (ulPlayerIndex >= MAX_PLAYER || ulPlayerID == 0)
			return 0;
		TMissionPlayerInfo Info;
		Info.m_ulPlayerIndex = ulPlayerIndex;
		Info.m_ulPlayerID = ulPlayerID;
		return m_MissionPlayer.FindSame(&Info);
	}

	unsigned long GetMissionPlayer_PlayerIndex(unsigned long ulDataIndex)
	{
		if (ulDataIndex > m_MissionPlayer.GetTotalCount())
			return 0;
		return m_MissionPlayer.m_Data[ulDataIndex].m_ulPlayerIndex;		
	}
	
	unsigned long GetMissionPlayer_GroupId(unsigned long ulDataIndex)
	{
		if (ulDataIndex > m_MissionPlayer.GetTotalCount())
			return 0;
		return m_MissionPlayer.m_Data[ulDataIndex].m_ucPlayerGroup;
	}

	unsigned long GetMissionNpc_DataIndex(unsigned long ulNpcIndex, unsigned long ulNpcID)
	{
		if (ulNpcIndex == 0 || ulNpcID == 0 || ulNpcIndex > MAX_NPC) //#can kiem tra
			return 0;
		TMissionNpcInfo Info;
		Info.m_ulNpcIndex = ulNpcIndex;
		Info.m_ulNpcID = ulNpcID;
		return m_MissionNpc.FindSame(&Info);
	}

	unsigned long GetMissionNpc_NpcIndex(unsigned long ulDataIndex)
	{
		if (ulDataIndex > m_MissionNpc.GetTotalCount())
			return 0;
		return m_MissionNpc.m_Data[ulDataIndex].m_ulNpcIndex;		
	}

	unsigned long GetGroupNpcCount (unsigned char ucGroup ) const 
	{
		unsigned long ulGroupNpcCount = 0;
		int nIdx = 0;
		while(1)
		{
			nIdx = m_MissionNpc.m_UseIdx.GetNext(nIdx);
			if (!nIdx)
				break;

			if (m_MissionNpc.m_Data[nIdx].m_ucNpcGroup == ucGroup)
				ulGroupNpcCount ++;
		}
		return ulGroupNpcCount;
	};
	
	unsigned long GetMissionNpc_GroupId(unsigned long ulDataIndex)
	{
		if (ulDataIndex > m_MissionNpc.GetTotalCount())
			return 0;
		return m_MissionNpc.m_Data[ulDataIndex].m_ucNpcGroup;
	}
};
#endif
#endif
