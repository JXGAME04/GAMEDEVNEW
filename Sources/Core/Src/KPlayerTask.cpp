//---------------------------------------------------------------------------
// Sword3 Engine (c) 2002 by Kingsoft
//
// File:	KPlayerTask.cpp
// Date:	2020.10.05
// Code:	Fong KiÒu
// Desc:	PlayerTask Class
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	"KPlayerTask.h"
#include	"KPlayer.h"

#ifdef _SERVER
int			g_TaskGlobalValue[TASKGLOBALVALUENUM]; 

KTabFile	g_WayPointTabFile;
KTabFile	g_StationTabFile;
KTabFile	g_DockTabFile;
KTabFile	g_StationPriceTabFile;
KTabFile	g_WayPointPriceTabFile;
KTabFile	g_DockPriceTabFile;

int	*		g_pStationPriceTab = NULL;
int	*		g_pWayPointPriceTab = NULL;
int	*		g_pDockPriceTab = NULL;

#endif
//---------------------------------------------------------------------------
//	¹¦ÄÜ£º¹¹Ôìº¯Êý
//---------------------------------------------------------------------------
KPlayerTask::KPlayerTask() // fix by lzlsky301
{	
#ifdef _SERVER
	m_nPlayerIndex = 0;
#endif
	Release();
}

#ifdef _SERVER // new add by lzlsky301
void KPlayerTask::Init(int nPlayerIdx)
{
	this->m_nPlayerIndex = nPlayerIdx;
}
#endif

//---------------------------------------------------------------------------
//	¹¦ÄÜ£ºÇå¿Õ
//---------------------------------------------------------------------------
void	KPlayerTask::Release()
{
	memset(nSave, 0, sizeof(nSave));
	memset(nClear, 0, sizeof(nClear));
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£ºÇå³ýÁÙÊ±¹ý³Ì¿ØÖÆ±äÁ¿
//---------------------------------------------------------------------------
void	KPlayerTask::ClearTempVar()
{
	memset(nClear, 0, sizeof(nClear));
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£ºµÃµ½ÈÎÎñÍê³ÉÇé¿ö
//---------------------------------------------------------------------------
DWORD		KPlayerTask::GetSaveVal(int nNo)
{
	if (nNo < 0 || nNo >= MAX_TASK)
		return 0;
	return nSave[nNo];
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£ºÉè¶¨ÈÎÎñÍê³ÉÇé¿ö
//---------------------------------------------------------------------------
void	KPlayerTask::SetSaveVal(int nNo, DWORD bFlag)
{
	if (nNo < 0 || nNo >= MAX_TASK)
		return;
	nSave[nNo] = bFlag;
#ifdef _SERVER
	TASK_VALUE_SYNC	sValue;
	sValue.ProtocolType = s2c_taskvalue;
	sValue.nTaskId = nNo;
	sValue.nTaskValue = bFlag;
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(TASK_VALUE_SYNC));
#endif
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£ºµÃµ½ÁÙÊ±¹ý³Ì¿ØÖÆ±äÁ¿Öµ
//---------------------------------------------------------------------------
DWORD		KPlayerTask::GetClearVal(int nNo)
{
	if (nNo < 0 || nNo >= MAX_TEMP_TASK)
		return 0;
	return nClear[nNo];
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£ºÉè¶¨ÁÙÊ±¹ý³Ì¿ØÖÆ±äÁ¿Öµ
//---------------------------------------------------------------------------
void	KPlayerTask::SetClearVal(int nNo, DWORD nVal)
{
	if (nNo < 0 || nNo >= MAX_TEMP_TASK)
		return;
	nClear[nNo] = nVal;
}

const char*		KPlayerTask::GetSaveStr(int nNo)
{	
	if (nNo < 0 || nNo >= MAX_TASK)
		return "";
	return "Ch­a râ";
}
