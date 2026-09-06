#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "../Elem/MouseHover.h"
#include "UiBattleReport.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../Engine/src/Text.h"
#include "../../../core/src/coreshell.h"
#include "../../../Engine/src/KDebug.h"
#include <crtdbg.h>

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell* g_pCoreShell;

KUiBattleReport* KUiBattleReport::m_pSelf = NULL;
static BATTLE_MODE	s_eBattleMode = BATTLE_M_BIG;//BATTLE_M_SMALL; //ChÕ ®é mÆc ®Þnh khi më Report lªn lÇn ®Çu tiªn

#define		SCHEME_INI_BR_SMALL			"UiBattleSmall.ini"
#define		SCHEME_INI_BR_BIG			"UiBattleBig.ini"

KUiBattleReport::KUiBattleReport()
{
	szRank[0][0] = 0;	
}

KUiBattleReport::~KUiBattleReport()
{
	szRank[0][0] = 0;	
}


KUiBattleReport* KUiBattleReport::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiBattleReport;
		if (m_pSelf)
			m_pSelf->Initialize();
	}

	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->Show();
		m_pSelf->BringToTop();
	}
	return m_pSelf;
}

void KUiBattleReport::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		else
			m_pSelf->Hide();
	}
}


KUiBattleReport* KUiBattleReport::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	else
		return NULL;
}


void KUiBattleReport::Initialize()
{
	AddChild(&m_NumPlayerT);	
	AddChild(&m_NumPlayerK);		
	AddChild(&m_TimeRemaining);	
	AddChild(&m_PointPlayerT);
	AddChild(&m_PointPlayerK);
	AddChild(&m_PointPlayer);
	AddChild(&m_SwitchBtn);	
	AddChild(&m_CloseBtn);	// [TKINFO 06/09] phai AddChild TRUOC LoadScheme
	for (int i = 0; i < MAX_TOP_INFO; i++)
	{
		if (i < 10)
		{
			AddChild(&m_Team[i]);
			AddChild(&m_Group[i]);	
			AddChild(&m_Name[i]);
		}
		AddChild(&m_Point[i]);
		AddChild(&m_PlayerPK[i]);
		AddChild(&m_KillNpc[i]);
		AddChild(&m_Death[i]);
		AddChild(&m_CurrentKill[i]);	
		AddChild(&m_CurrentKillMax[i]);
		AddChild(&m_NumRobFlag[i]);
	}		
	
	char Scheme[128];
	g_UiBase.GetCurSchemePath(Scheme, 128);
	LoadScheme(Scheme);

	m_Style &= ~WND_S_VISIBLE;
	Wnd_AddWindow(this, WL_TOPMOST);
}

void KUiBattleReport::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		szBuff[128];
		KIniFile	Ini;
		sprintf(szBuff, "%s\\%s", pScheme, s_eBattleMode == BATTLE_M_SMALL ? SCHEME_INI_BR_SMALL : SCHEME_INI_BR_BIG);
		if (Ini.Load(szBuff))
			m_pSelf->LoadScheme(&Ini, s_eBattleMode);
	}
}

void KUiBattleReport::LoadScheme(KIniFile* pIni, BATTLE_MODE eMode)
{
	if (pIni == NULL)
		return;
	int i;	
	char szBuff[128];		
	Init(pIni, "Main");
	m_NumPlayerT.Init(pIni, "NumPlayerT");
	m_NumPlayerK.Init(pIni, "NumPlayerK");		
	m_TimeRemaining.Init(pIni, "TimeRemaining");
	m_PointPlayerT.Init(pIni, "PointPlayerT");
	m_PointPlayerK.Init(pIni, "PointPlayerK");
	m_PointPlayer.Init(pIni, "PointPlayer");
	m_SwitchBtn.Init(pIni, "SwitchBtn");	
	m_CloseBtn.Init(pIni, "CloseBtn");	// [TKINFO 06/09] co trong CA HAI ini Big + Small
		
	for (i = 0; i < MAX_BATTLE_GROUP; i++)
	{
		sprintf(szBuff, "%d", i);
		pIni->GetString("BattleRank", szBuff, "", szRank[i], sizeof(szRank[i]));
	}				
	
	switch (eMode)
	{
	case BATTLE_M_BIG:
		for (i = 0; i < MAX_TOP_INFO; i++)
		{	
			if (i < 10)
			{
				sprintf(szBuff,"Team_%d",i);
				m_Team[i].Init(pIni, szBuff);
				m_Team[i].Show();	
				
				sprintf(szBuff,"Group_%d",i);
				m_Group[i].Init(pIni, szBuff);
				m_Group[i].Show();		
				
				sprintf(szBuff,"Name_%d",i);
				m_Name[i].Init(pIni, szBuff);
				m_Name[i].Show();
			}
			
			sprintf(szBuff,"Point_%d",i);
			m_Point[i].Init(pIni, szBuff);
			m_Point[i].Show();

			sprintf(szBuff,"PlayerPK_%d",i);
			m_PlayerPK[i].Init(pIni, szBuff);
			m_PlayerPK[i].Show();

			sprintf(szBuff,"KillNpc_%d",i);
			m_KillNpc[i].Init(pIni, szBuff);
			m_KillNpc[i].Show();

			sprintf(szBuff,"Death_%d",i);
			m_Death[i].Init(pIni, szBuff);
			m_Death[i].Show();

			sprintf(szBuff,"CurrentKill_%d",i);
			m_CurrentKill[i].Init(pIni, szBuff);
			m_CurrentKill[i].Show();

			sprintf(szBuff,"CurrentKillMax_%d",i);
			m_CurrentKillMax[i].Init(pIni, szBuff);
			m_CurrentKillMax[i].Show();
			
			sprintf(szBuff,"NumRobFlag_%d",i);
			m_NumRobFlag[i].Init(pIni, szBuff);
			m_NumRobFlag[i].Show();
		}
		break;
	case BATTLE_M_SMALL:
		for (i = 0; i < MAX_TOP_INFO; i++)
		{	
			if (i < 10)
			{
				m_Team[i].Hide();	
				
				sprintf(szBuff,"Group_%d",i);
				m_Group[i].Init(pIni, szBuff);
				m_Group[i].Show();		
				
				sprintf(szBuff,"Name_%d",i);
				m_Name[i].Init(pIni, szBuff);
				m_Name[i].Show();
			}
			
			if (i == 10)
			{
				sprintf(szBuff,"Point_%d",i);
				m_Point[i].Init(pIni, szBuff);
				m_Point[i].Show();
			}
			else
				m_Point[i].Hide();
			
			m_PlayerPK[i].Hide();
			m_KillNpc[i].Hide();
			m_Death[i].Hide();
			m_CurrentKill[i].Hide();
			m_CurrentKillMax[i].Hide();
			m_NumRobFlag[i].Hide();
		}
		break;		
	}
}

int KUiBattleReport::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_SwitchBtn)
			SetMode(s_eBattleMode == BATTLE_M_SMALL ? BATTLE_M_BIG : BATTLE_M_SMALL);
		// [TKINFO 06/09] nut dong cua rieng bang chien bao. Truoc day cua so nay khong co duong dong nao:
		// [SwitchBtn] chi doi che do To/Nho, con nhanh VK_ESCAPE ben duoi hau nhu khong bao gio chay vi
		// KShortcutKeyCentre an phim ESC truoc (Wnds.cpp) - ESC thuc te dong SACH moi cua so qua UiShell.
		else if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn)
			CloseWindow(false);	// false = chi an, giu du lieu (true se Destroy va bo m_pSelf)
		break;
	case WM_KEYDOWN:
		if (uParam == VK_ESCAPE)
			CloseWindow(false);
		break;		
	case WM_LBUTTONDOWN:	
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MOUSEHOVER:
	case WND_N_CHILD_MOVE:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
	case WM_RBUTTONUP:
		break;
	default:
		nRet = KWndWindow::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}

void KUiBattleReport::UpdateRankWorld(const char* pszWorldRank, BYTE nType)
{
	if (m_pSelf == NULL)
	{
		return;
	}
	// [TKINFO 06/09] kind 9 = het tran / roi tran (script Tong Kim da gui san): dong luon bang chien bao,
	// truoc day no treo lai tren man hinh cho toi khi nguoi choi bam ESC (ma ESC dong SACH moi cua so).
	if (nType == 9)
	{
		CloseWindow(false);
		return;
	}
	// kind 7/8 la cua cua so Thong Tin Tran (KUiTongKimInfo), khong phai cua bang nay -> ra som,
	// tranh strcpy KHONG kiem tra do dai o duoi chep chuoi 127 byte vao szString[128] ma khong dung den.
	if (pszWorldRank == NULL || nType > 6)
		return;
	char szString[128];
	// [TKINFO 06/09] pszWorldRank tro THANG vao bo dem nhan goi (szBattleDesc[128] khong bao dam ket thuc NUL)
	// -> strcpy khong kiem do dai co the tran ngan xep. Chep co chan nhu KUiTongKimInfo::SetRows.
	strncpy(szString, pszWorldRank, sizeof(szString) - 1);
	szString[sizeof(szString) - 1] = 0;
	switch (nType)
	{
		case 1:
		{
			char Buff[32];
			int  nReport[1];
			memset(nReport, 0, sizeof(nReport));
			nReport[0] = atoi(szString);
			sprintf(Buff, "%d'", nReport[0]);
			m_pSelf->m_TimeRemaining.SetText(Buff);
			break;
		}
		case 2:
		{
			int  nReport[2];
			int j = 0;
			memset(nReport, 0, sizeof(nReport));
			char* chars_array = strtok(szString, "|");
			while (j < 2)
			{
				nReport[j] = atoi(chars_array);
				chars_array = strtok(NULL, "|");
				j++;
			}
			m_pSelf->m_NumPlayerT.SetIntText(nReport[0]);
			m_pSelf->m_NumPlayerK.SetIntText(nReport[1]);
			break;
		}
		case 3: // the gioi
		{
			char szName[32];
			int  nReport[9];
			int j = 0;
			memset(nReport, 0, sizeof(nReport));
			char* chars_array = strtok(szString, "|");
			strcpy(szName, chars_array);
			while (j < 9)
			{
				chars_array = strtok(NULL, "|");
				nReport[j] = atoi(chars_array);
				j++;
			}
			BYTE i = nReport[0] - 1;
			m_pSelf->m_Team[i].SetIntText(nReport[0]);
			m_pSelf->m_Group[i].SetText(m_pSelf->szRank[nReport[1]]);
			m_pSelf->m_Name[i].SetText(szName);

			m_pSelf->m_Point[i].SetIntText(nReport[4]);
			m_pSelf->m_PlayerPK[i].SetIntText(nReport[2]);
			m_pSelf->m_KillNpc[i].SetIntText(nReport[3]);
			m_pSelf->m_Death[i].SetIntText(nReport[5]);
			m_pSelf->m_CurrentKill[i].SetIntText(nReport[6]);
			m_pSelf->m_CurrentKillMax[i].SetIntText(nReport[7]);
			m_pSelf->m_NumRobFlag[i].SetIntText(nReport[8]);
			break;
		}
		case 4:
		{
			m_pSelf->Clear();
			break;
		}
		case 5: // ca nhan
		{
			char szName[32];
			int  nReport[9];
			int j = 0;
			memset(nReport, 0, sizeof(nReport));
			char* chars_array = strtok(szString, "|");
			strcpy(szName, chars_array);
			while (j < 9)
			{
				chars_array = strtok(NULL, "|");
				nReport[j] = atoi(chars_array);
				j++;
			}
			BYTE i = nReport[0] - 1;
			m_pSelf->m_Point[i].SetIntText(nReport[4]);
			m_pSelf->m_PlayerPK[i].SetIntText(nReport[2]);
			m_pSelf->m_KillNpc[i].SetIntText(nReport[3]);
			m_pSelf->m_Death[i].SetIntText(nReport[5]);
			m_pSelf->m_CurrentKill[i].SetIntText(nReport[6]);
			m_pSelf->m_CurrentKillMax[i].SetIntText(nReport[7]);
			m_pSelf->m_NumRobFlag[i].SetIntText(nReport[8]);
			break;
		}
		case 6: //Point c¸ nh©n vµ Point T and Point K
		{
			int  nReport[3];
			int j = 0;
			memset(nReport, 0, sizeof(nReport));
			char* chars_array = strtok(szString, "|");
			while (j < 3)
			{
				nReport[j] = atoi(chars_array);
				chars_array = strtok(NULL, "|");
				j++;
			}
			m_pSelf->m_PointPlayerT.SetIntText(nReport[0]);
			m_pSelf->m_PointPlayerK.SetIntText(nReport[1]);
			m_pSelf->m_PointPlayer.SetIntText(nReport[2]);
			break;
		}
	}
}

void KUiBattleReport::SetMode(BATTLE_MODE eMode)
{
	if (eMode == s_eBattleMode &&
		(eMode < BATTLE_M_SMALL || eMode > BATTLE_M_COUNT))
		return;
	if (eMode != s_eBattleMode)
		s_eBattleMode = eMode;
		
	char Scheme[128];	
	switch(s_eBattleMode = eMode)
	{
	case BATTLE_M_SMALL:
	case BATTLE_M_BIG:
		g_UiBase.GetCurSchemePath(Scheme, 128);
		LoadScheme(Scheme);
		break;
	}
}

void KUiBattleReport::Clear()
{
	m_NumPlayerT.SetIntText(0);
	m_NumPlayerK.SetIntText(0);	
	m_TimeRemaining.SetIntText(0);
	for (int i = 0; i < MAX_TOP_INFO; i++)
	{
		if (i < 10)
		{
			m_Team[i].SetIntText(i+1);	
			m_Group[i].SetText("");			
			m_Name[i].SetText(szRank[0]);
		}
		m_Point[i].SetIntText(0);
		m_PlayerPK[i].SetIntText(0);
		m_KillNpc[i].SetIntText(0);
		m_Death[i].SetIntText(0);
		m_CurrentKill[i].SetIntText(0);
		m_CurrentKillMax[i].SetIntText(0);	
		m_NumRobFlag[i].SetIntText(0);			
	}
}
