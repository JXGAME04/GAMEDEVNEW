// -------------------------------------------------------------------------
//	File: UiReconnect.cpp
//	Author: Fong KiÒu
//	Date: 2021
//	Tù connect l¹i khi rít m¹ng	
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "UiReconnect.h"
#include "UiInformation.h"
//#include "UiAuto.h"
//#include "UiAutoPlay.h"
#include "UiSysMsgCentre.h"
#include "../UiShell.h"
#include "../UiCase/UiInit.h"
#include "../Elem/UiImage.h"
#include "../UiBase.h"
#include "../../Login/Login.h"
#include "../../../Engine/Src/Text.h"
#include <crtdbg.h>

#define	MSG_RECONNECTING				"KÕt nèi l¹i lÇn thø <color=red>%d<color> sau\n%s gi©y..."
#define	MSG_WAIT_TO_RECONNECT			"Sau %d gi©y sÏ tiÕn hµnh kÕt nèi ®Õn server lÇn thø <color=red>%d<color>"
#define	MSG_WAIT_NEXT_GROUP_RECONNECT	"Sau %d gi©y n÷a míi cã thÓ kÕt nèi server"
#define	MSG_HIDE_RECONNECT_MSG_ID		"21"

#define RECONNECT_GROUP_INTERVAL		60000

#define RECONNECT_INTERVAL				10000

#define INVISIBLE_RECONNECT_TIMES		3

#define INVISIBLE_RECONNECT_INTERVAL	5000

#define	RECONNECT_QUIT_BTN_LABEL	"Tho¸t"

#define	RECONNECT_START_AT_ONCE		"KÕt nèi l¹i"

KReconnectWnd*	KReconnectWnd::m_pSelf = NULL;

bool KReconnectWnd::LaunchReconnect(int nMaxTimes)
{
	_ASSERT(m_pSelf == NULL);
	if (m_pSelf == NULL)
		m_pSelf = new KReconnectWnd;
	if (m_pSelf)
	{
		m_pSelf->m_bStop = false;
		Wnd_AddWindow(m_pSelf, WL_TOPMOST);
		if (nMaxTimes >= 1)
			m_pSelf->m_nMaxReconnectTimes = nMaxTimes;
		else
			m_pSelf->m_nMaxReconnectTimes = 1;
		m_pSelf->FirstReconnect();
	}
	else
	{
		Exit(true);
	}
	return (m_pSelf != NULL);
}

bool KReconnectWnd::IsReconnecttingGoingOn()
{
	return (m_pSelf != NULL);
}

void KReconnectWnd::FirstReconnect()
{
	m_nReconnectTimes = 0;
	m_bWaitToReconnect = true;
	m_uWaitStartTime = IR_GetCurrentTime();
	m_uToWaitTime = rand () % INVISIBLE_RECONNECT_INTERVAL + 1000;

	KIniFile* pConfig = g_UiBase.GetCommConfigFile();
	m_nHideTimes = INVISIBLE_RECONNECT_TIMES;
	if (pConfig)
	{
		pConfig->GetInteger("Main", "AutoReconnectHideTimes", 3, &m_nHideTimes);
		if (m_nHideTimes < 0)
			m_nHideTimes = 0;
	}
}

bool KReconnectWnd::StartReconnect()
{
	if (m_nHideTimes > 0 ||
		(++m_nReconnectTimes) < m_nMaxReconnectTimes)
	{
		if (m_nHideTimes > 0)
		{
			KSystemMessage	Msg;
			Msg.byConfirmType = SMCT_NONE;
			Msg.byParamSize = 0;
			Msg.byPriority = 0;
			Msg.eType = SMT_NORMAL;
			Msg.uReservedForUi = 0;

			KIniFile*	pIni = g_UiBase.GetCommConfigFile();
			if (pIni)
			{
				if (pIni->GetString("InfoString", MSG_HIDE_RECONNECT_MSG_ID,"", Msg.szMessage, sizeof(Msg.szMessage)))
				{
					KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
				}
				g_UiBase.CloseCommConfigFile();
			}
		}

		g_LoginLogic.ReturnToIdle();
		g_LoginLogic.AutoLogin();
		m_bWaitToReconnect = false;
	}
	else
	{
		m_nReconnectTimes = 0;
		m_pSelf->m_bWaitToReconnect = true;
		m_pSelf->m_uWaitStartTime = IR_GetCurrentTime();
		m_pSelf->m_uToWaitTime = RECONNECT_GROUP_INTERVAL;
	}
	return true;
}

void KReconnectWnd::Exit(bool bQuitGame)
{
	if (m_pSelf)
	{
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
	if (bQuitGame)
	{
		g_LoginLogic.ReturnToIdle();
		UiEndGame();
		KUiInit::OpenWindow(true, false);
	}
}

void KReconnectWnd::Breathe()
{
	if (m_bStop)
		return;
	char	szInfo[256];
	int		nInfoLen;
	if (m_bWaitToReconnect)
	{
		unsigned int uRemain;
		if (uRemain = IR_GetRemainTime(m_uToWaitTime, m_uWaitStartTime))
		{
			if (m_nHideTimes == 0)
			{
				if (uRemain < RECONNECT_INTERVAL)
				{
					nInfoLen = sprintf(szInfo, MSG_WAIT_TO_RECONNECT,
						(uRemain + 999)/ 1000, m_nReconnectTimes + 1);
				}
				else
				{
					nInfoLen = sprintf(szInfo, MSG_WAIT_NEXT_GROUP_RECONNECT,
						(uRemain + 999) / 1000);
				}			
				nInfoLen = TEncodeText(szInfo, nInfoLen);
				g_UiInformation.Show(szInfo, RECONNECT_QUIT_BTN_LABEL, RECONNECT_START_AT_ONCE, this, 0, nInfoLen);
			}
		}
		else
		{
			StartReconnect();
		}
		return;
	}

	LOGIN_LOGIC_STATUS eStatus = g_LoginLogic.GetStatus();
	const char* pReconnectMsg = NULL;
	switch (eStatus)
	{
	case LL_S_IN_GAME:
		g_UiInformation.Close();
		UiResumeGame();
		//KUiAuto::LoadScheme();//fix by phong kiÒu khi rít m¹ng mÊt auto party
		//char Scheme[256];
		//g_UiBase.GetCurSchemePath(Scheme, 256);//rot mang mat to doi
		//KUiAutoPlay::OnActive(TRUE);
		Exit(false);
		break;
	case LL_S_IDLE:	
		{
			LOGIN_LOGIC_RESULT_INFO eResult= g_LoginLogic.GetResult();
			if (eResult == LL_R_ACCOUNT_NOT_ENOUGH_POINT)
			{
				m_bStop = true;
			}
			else
			{
				m_bWaitToReconnect = true;
				m_uWaitStartTime = IR_GetCurrentTime();
				if (m_nHideTimes > 0)
				{
					m_nHideTimes--;
					m_uToWaitTime = INVISIBLE_RECONNECT_INTERVAL;
				}
				else
				{
					m_uToWaitTime = RECONNECT_INTERVAL;
					pReconnectMsg = "KÕt nèi thÊt b¹i";
				}
			}
		}
		break;
	case LL_S_ACCOUNT_CONFIRMING:	
		pReconnectMsg = "§ang tiÕn hµnh kiÓm tra tµi kho¶n vµ mËt khÈu";
		break;
	case LL_S_WAIT_ROLE_LIST:	
		pReconnectMsg = "T¶i d÷ liÖu th«ng tin vÒ m¸y...";
		break;
	case LL_S_WAIT_TO_LOGIN_GAMESERVER:	
		pReconnectMsg = "KÕt nèi vµo trß ch¬i";
		break;
	case LL_S_ENTERING_GAME:		
		pReconnectMsg = "§ang vµo trß ch¬i...";
		break;
	}

	if (m_bStop)
	{
		pReconnectMsg = "Kh«ng ®ñ thêi gian ®Ó ch¬i!.H·y n¹p card ®Ó tiÕp tôc trß ch¬i!";
		nInfoLen = strlen(pReconnectMsg);
		g_UiInformation.Show(pReconnectMsg, RECONNECT_QUIT_BTN_LABEL, NULL, this, 0, nInfoLen);
	}
	else if (pReconnectMsg && m_nHideTimes == 0)
	{
		nInfoLen = sprintf(szInfo, MSG_RECONNECTING,
				m_nReconnectTimes + 1, pReconnectMsg);
		nInfoLen = TEncodeText(szInfo, nInfoLen);
		g_UiInformation.Show(szInfo, RECONNECT_QUIT_BTN_LABEL, NULL, this, 0, nInfoLen);
	}
}

int KReconnectWnd::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (uMsg == WND_M_OTHER_WORK_RESULT)
	{
		if (nParam == 1)	
		{
			StartReconnect();
			Breathe();
		}
		else
			Exit(true);		
	}
	return 0;
}
