#ifndef KTASKFUNCS_H
#define KTASKFUNCS_H

#include "KSubWorldSet.h"
typedef int (* PF_TimerCallBackFun)(void * pOwner, char * szFileName);

class KPlayer;

class KTaskFun
{
public:
	virtual BOOL	SaveTask(KPlayer *) = 0;
	virtual BOOL	LoadTask(KPlayer *) = 0;
};

class KTimerTaskFun :public KTaskFun
{
public:
	KTimerTaskFun();
	
	KTimerTaskFun(void * pOwner)
	{
		m_pTimerOwner = pOwner;
	};

	void SetOwner(void * pOwner) { m_pTimerOwner = pOwner;};
	static BOOL	Init();

	BOOL	SaveTask(KPlayer *);
	BOOL	LoadTask(KPlayer *);
	void *  m_pTimerOwner;
	
	BOOL	SetTimer(size_t ulTime, unsigned short usTimerTaskId)
	{
		if (ulTime == 0) 
		{
			CloseTimer();
			return FALSE;
		}

		m_dwIntervalTime = ulTime;
		m_dwTimeTaskTime = g_SubWorldSet.GetGameTime() + ulTime;
		m_dwTimerTaskId = usTimerTaskId;
		return TRUE;
	};
	
	void	CloseTimer()
	{
		m_dwTimeTaskTime = 0;
		m_dwTimerTaskId = 0;
		m_dwIntervalTime = 0;
	};

	unsigned long	GetRestTime() const
	{
		DWORD dwGameTime = g_SubWorldSet.GetGameTime();
		return (dwGameTime > m_dwTimeTaskTime) ? 0 : (m_dwTimeTaskTime - dwGameTime);
	};
	
	void	GetTimerTaskScript(char * szScriptFileName, unsigned short usTimerTaskId, size_t nScriptFileLen);
	BOOL	Activate(PF_TimerCallBackFun TimerCallBackFun);
	static KTabFile m_TimerTaskTab;
	DWORD GetTaskId(){return m_dwTimerTaskId;};
private:
	DWORD	m_dwTimeTaskTime;
	DWORD	m_dwTimerTaskId;
	DWORD	m_dwIntervalTime;
};


typedef KTaskFun KTask;

extern KTimerTaskFun g_TimerTask;
#endif
