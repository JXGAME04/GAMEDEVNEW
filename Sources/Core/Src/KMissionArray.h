#ifndef KMISSIONARRAY_H
#define KMISSIONARRAY_H

#include "KTaskFuns.h"
#include "KMission.h"

#define MAX_MISSIONARRAY_VALUE_COUNT 100
//管理Mission的集合

template <class T, size_t ulSize>
class KMissionArray : public KLinkArrayTemplate<T,  ulSize>
{
	int	 m_MissionValueC[MAX_MISSIONARRAY_VALUE_COUNT];
	char	m_MissionValue[MAX_MISSIONARRAY_VALUE_COUNT][16];
public:
	BOOL	Activate()
	{
		unsigned long ulActiveCount = GetUsedCount();
		if (ulActiveCount == 0)
			return FALSE;
		
		int nIdx = 0;		
		for (int i = 0; i < ulActiveCount ; i ++)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
			_ASSERT(nIdx);
			if (!nIdx)
			{
				break;
			}
			m_Data[nIdx].Activate();
		}
		return TRUE;
	};

	void SetMission(unsigned long ulValueId, const char* szValue)
	{
		if (ulValueId >= MAX_MISSIONARRAY_VALUE_COUNT)
			return ;
		strcpy(m_MissionValue[ulValueId], szValue);
	};
	
	int GetMissionValue(unsigned long ulValueId) const
	{
		if (ulValueId >= MAX_MISSIONARRAY_VALUE_COUNT)
			return 0;
		int nResult =0;
		if (m_MissionValue[ulValueId][0])
			nResult = atoi(m_MissionValue[ulValueId]);
		return nResult;
	};
	void SetMissionValue(unsigned long ulValueId, int szValue)
	{
		if (ulValueId >= MAX_MISSIONARRAY_VALUE_COUNT)
			return ;
		m_MissionValueC[ulValueId] = szValue;
	};

	int GetMissionValueC(unsigned long ulValueId) const
	{
		if (ulValueId >= MAX_MISSIONARRAY_VALUE_COUNT)
			return 0;
		return m_MissionValueC[ulValueId];
	};

	const char* GetMissionString(unsigned long ulValueId) const
	{
		if (ulValueId >= MAX_MISSIONARRAY_VALUE_COUNT)
			return "";
		return m_MissionValue[ulValueId];
	};

	// [MSFIND 05/09] Tra nhiem vu theo id KHONG dung KMission tam. Truoc: moi ham Lua nhiem vu viet
	// 'KMission Mission; Mission.SetMissionId(id); GetData(&Mission)' -> constructor KLinkArrayTemplate cap phat
	// 2 x new KLinkNode[] cho mang nguoi (MAX_PLAYER) + NPC (5000) + timer, chen toan bo o free, roi delete[]:
	// ~210 KB stack + 101 KB heap + ~6.500 vong moi loi goi (~40 us). Do 05/09: vong bang diem TK 2.400 loi goi
	// /lan chet = ~90 ms -> may chu bao hoa (ban giao BANGIAO_TONGKIM_CHAT_DIEM_0409.md muc 13f/13g).
	// FindSame chi so GetMissionId() va khong ghi nguoc -> ham nay tuong duong 100%.
	T* FindById(unsigned long ulMissionId)
	{
		int nIdx = 0;
		while(1)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
			if (!nIdx)
				break;
			if (m_Data[nIdx].GetMissionId() == ulMissionId)
				return &m_Data[nIdx];
		}
		return NULL;
	};
	unsigned long FindSame(T *pMission)
	{
		int nIdx = 0;
		while(1)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
			if (!nIdx)
				break;
			
			if (pMission->GetMissionId() == m_Data[nIdx].GetMissionId())
				return nIdx;
		}
		return 0;
	};

	BOOL RemovePlayer(unsigned long ulPlayerIndex, unsigned long ulPlayerID)
	{
		unsigned long ulActiveCount = GetUsedCount();
		if (ulActiveCount == 0)
			return FALSE;
		
		int nIdx = 0;		
		for (int i = 0; i < ulActiveCount ; i ++)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
			_ASSERT(nIdx);
			if (!nIdx)
			{
				break;
			}
			m_Data[nIdx].RemovePlayer(ulPlayerIndex, ulPlayerID);//#removeplayermissionarray
		}
		return TRUE;
	}
	
};


//管理定时触发器的集合
template <class T, size_t ulSize, PF_TimerCallBackFun CallFun>
class KTimerFunArray :public KLinkArrayTemplate < T, ulSize>
{
public:
	char m_szScriptFile[MAX_PATH];
	BOOL	Activate()
	{
		unsigned long ulActiveCount = GetUsedCount();
		if (ulActiveCount == 0)
			return FALSE;
		
		int nIdx = 0;		
		for (int i = 0; i < ulActiveCount ; i ++)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
		//	_ASSERT(nIdx);
			if (!nIdx)
			{
				break;
			}
			m_Data[nIdx].Activate(CallFun);
		}
		return TRUE;
	};

	unsigned long FindSame(T *pTask)
	{
		int nIdx = 0;
		while(1)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
			if (!nIdx)
				break;

			if (pTask->GetTaskId() == m_Data[nIdx].GetTaskId())
				return nIdx;
		}
		return 0;
	};

};
#endif
