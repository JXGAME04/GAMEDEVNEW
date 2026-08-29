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

// [WLLS 20/08] 3000 -> 4200: leaguematch dung task 4125 (WLLS_TASKID_SERIES_WIN,
// head.lua:79) - voi tran 3000 SetTask(4125) bi nuot IM LANG (KPlayerTask.cpp:79).
// Dinh dang luu la (id, value) THUA - KHONG phu thuoc MAX_TASK (KPlayerDBFuns.cpp:
// 1126-1140 chi ghi o khac 0; nap co guard id >= MAX_TASK) nen save cu doc binh
// thuong. Cung go ke tinh trang task 3179 cua citywar (KJx2CityWar.h:57).
// [KM 27/08] 4200 -> 4600: he kinh mach CHUAN dung task 4318 (diem Huyen Nguyen),
// 4319-4322 (cap 4 mach moi) va 4440/4441/4491 (dem so lan that bai - bao day).
// Voi tran 4200 cac SetTask nay bi nuot IM LANG (KPlayerTask.cpp:79) => ca he bao
// day chet o CA HAI phia. Dinh dang luu (id,value) THUA - khong phu thuoc MAX_TASK
// (xem chu thich dot 3000->4200 ngay tren).
// [BDH 27/08] 4600 -> 5200: cap dai 4600..5099 cho he BAN DONG HANH
// (so do o KPlayerPartner.h) + sua loi task 5100 (npc_chuyensinh.lua) dang bi
// SetSaveVal nuot im lang vi vuot tran cu. Dinh dang blob (id,value) THUA nen
// khong doi dinh dang DB. MAX_TEMP_TASK giu 4600 (tiet kiem RAM).
#define		MAX_TASK				5200
#define		MAX_TEMP_TASK	 4600

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
