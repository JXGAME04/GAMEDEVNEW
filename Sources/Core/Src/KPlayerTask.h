//---------------------------------------------------------------------------
// Sword3 Engine (c) 2002 by Kingsoft
//
// File:	KPlayerTask.h
// Date:	2020.10.05
// Code:	Fong Ki襲
// Desc:	PlayerTask Class
//---------------------------------------------------------------------------

#ifndef KPLAYERTASK_H
#define KPLAYERTASK_H

#define		MAX_TASK				3000
#define		MAX_TEMP_TASK	 3000

#ifdef _SERVER
#define TASKGLOBALVALUENUM 256
extern int		g_TaskGlobalValue[TASKGLOBALVALUENUM];

extern KTabFile g_WayPointTabFile;
extern KTabFile g_StationTabFile;
extern KTabFile g_DockTabFile;

extern KTabFile g_StationPriceTabFile;
extern KTabFile g_WayPointPriceTabFile;
extern KTabFile g_DockPriceTabFile;

extern int	*g_pStationPriceTab;
extern int  *g_pWayPointPriceTab;
extern int  *g_pDockPriceTab;

#endif

class KPlayerTask
{
public:
	int			m_nPlayerIndex; // new add by lzlsky301
	int			nSave[MAX_TASK];					// task n祔 l璾
	int			nClear[MAX_TEMP_TASK];				// task n祔 m蕋 kh玭g l璾
public:
	KPlayerTask();									// 构造函数
	void		Release();							// 清空
	void		ClearTempVar();						// 清除临时过程控制变量
	DWORD			GetSaveVal(int nNo);				// 得到任务完成情况
	void		SetSaveVal(int nNo, DWORD bFlag);	// 设定任务完成情况
	DWORD			GetClearVal(int nNo);				// 得到临时过程控制变量值
	void		SetClearVal(int nNo, DWORD nVal);		// 设定临时过程控制变量值
	const char*		GetSaveStr(int nNo);
#ifdef _SERVER
	void		Init(int nPlayerIdx);
#endif
};

#endif
