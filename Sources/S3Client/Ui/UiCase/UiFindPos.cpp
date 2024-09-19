#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "UiFindPos.h"
#include "UiItem.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include <crtdbg.h>

extern iCoreShell*		g_pCoreShell;

#define SCHEME_INI_ITEM	"UiFindPos.ini"

KUiFindPos* KUiFindPos::m_pSelf = NULL;

KUiFindPos* KUiFindPos::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

KUiFindPos* KUiFindPos::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiFindPos;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->BringToTop();
		m_pSelf->Show();
		m_pSelf->UpdateData1();
		Wnd_GameSpaceHandleInput(false);
	}
	return m_pSelf;
}

void KUiFindPos::Show()
{
	KWndImage::Show();
	Wnd_SetFocusWnd(&m_PosInput);
	Wnd_SetExclusive((KWndWindow*)this);

	char szPos[32];
	sprintf(szPos, "199.199");
	LoadPosSetting(szPos); //add by phong kiÒu save pos minimap
	m_pSelf->m_PosInput.SetText(szPos);
}

//--------------------------------------------------------------------------
// Dong hop thoai 
//--------------------------------------------------------------------------
void KUiFindPos::CloseWindow()
{
	if (m_pSelf)
	{
		Wnd_GameSpaceHandleInput(true);
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

// -------------------------------------------------------------------------
// Khoi tao hop thoai
// -------------------------------------------------------------------------
void KUiFindPos::Initialize()
{
	//AddChild(&m_PosXInput);
	//AddChild(&m_PosYInput);
	AddChild(&m_PosInput);
	AddChild(&m_InfoText);
	//AddChild(&m_Text);
	AddChild(&m_OKBtn);
	AddChild(&m_CancelBtn);
	
	char schemePath[256];
	g_UiBase.GetCurSchemePath(schemePath, 256);
	LoadScheme(schemePath);

	Wnd_AddWindow(this);

	FXY = 0;
}

// -------------------------------------------------------------------------
// Tao layout hop thoai
// -------------------------------------------------------------------------
void KUiFindPos::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_ITEM);
	if (m_pSelf && Ini.Load(Buff))
	{
		m_pSelf->Init(&Ini, "Main");
		//m_pSelf->m_PosXInput.Init(&Ini, "PosXInput");
		//m_pSelf->m_PosYInput.Init(&Ini, "PosYInput");
		//m_pSelf->m_Text.Init(&Ini, "Text");
		//m_pSelf->m_Text.SetText("/");
		m_pSelf->m_PosInput.Init(&Ini, "StringInput"); 
		m_pSelf->m_PosInput.SetText("199/201"); //khëi t¹o mÆc ®Þnh to¹ ®é
		m_pSelf->m_InfoText.Init(&Ini, "InfoText");
		m_pSelf->m_OKBtn.Init(&Ini, "OkBtn");
		m_pSelf->m_CancelBtn.Init(&Ini, "CancelBtn");
	}
}

// -------------------------------------------------------------------------------
// Gui goi tin yeu cau tim toa do
// -------------------------------------------------------------------------------
void KUiFindPos::OnOK()
{
	int PosX = 199;
	int PosY = 201;
	char	szBuff1[16];
	m_PosInput.GetText(szBuff1, sizeof(szBuff1), true);
	if (isdigit(szBuff1[0]) == false || isdigit(szBuff1[1]) == false || isdigit(szBuff1[2]) == false || isdigit(szBuff1[4]) == false || isdigit(szBuff1[5]) == false || isdigit(szBuff1[6]) == false)
	{
		m_InfoText.SetText("Sai to¹ ®é ");
		return;
	}
	else if(!(szBuff1[3] == '/' || szBuff1[3] == '.' || szBuff1[3] == ' ' || szBuff1[3] == '-' || szBuff1[3] == '*' || szBuff1[3] == ';' || szBuff1[3] == ',' || szBuff1[3] == '|' ))
	{
		m_InfoText.SetText("Sai to¹ ®é ");
		return;
	}
	char	szBuffX[3];
	szBuffX[0] = szBuff1[0]; szBuffX[1] = szBuff1[1]; szBuffX[2] = szBuff1[2];
	PosX = atoi(szBuffX);
	char	szBuffY[3];
	szBuffY[0] = szBuff1[4]; szBuffY[1] = szBuff1[5]; szBuffY[2] = szBuff1[6];
	PosY = atoi(szBuffY);

	if (g_pCoreShell->GetFlagMode())
	{
		g_pCoreShell->SetFlagMode(false);
	}
	g_pCoreShell->SetPaintMode(1);
	g_pCoreShell->DirectFindPos(PosX, PosY, FALSE, TRUE);
	char szBuff[32];//add by phong kiÒu save pos minimap
	m_PosInput.GetText(szBuff, sizeof(szBuff), true);
	SavePosSetting(szBuff);
	CloseWindow();
}

void KUiFindPos::SavePosSetting(char *m_PosMiniMap)
{
	KIniFile* pConfigFile = NULL;
	pConfigFile = g_UiBase.GetAutoSettingFile();
	if(pConfigFile)
	{
		pConfigFile->WriteString("Fighting", "PosMiniMap", m_PosMiniMap);
		g_UiBase.CloseAutoSettingFile(true);
	}
}

void KUiFindPos::LoadPosSetting(char *m_PosMiniMap)
{
	KIniFile* pConfigFile = NULL;
	pConfigFile = g_UiBase.GetAutoSettingFile();
	if(pConfigFile)
	{
		pConfigFile->GetString("Fighting", "PosMiniMap", "199/201", m_PosMiniMap, 32);
		g_UiBase.CloseAutoSettingFile(true);
	}
}

int KUiFindPos::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_CancelBtn)
		{
			g_pCoreShell->DirectFindPos(0, 0, TRUE, TRUE);// Click len nut cancel ~> dong hop thoai
			CloseWindow();
			nRet = 1;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_OKBtn)
		{
			OnOK();// Click len nut OK ~> Gui goi tin tim toa do
			nRet = 1;
		}
		break;
	case WM_KEYDOWN:
		if (uParam == VK_RETURN)
		{
			OnOK();
			nRet = 1;
		}
		else if (uParam == VK_ESCAPE)
		{
			g_pCoreShell->DirectFindPos(0, 0, TRUE, TRUE);
			CloseWindow();
			nRet = 1;
		}
		break;
	case WND_N_EDIT_SPECIAL_KEY_DOWN:
		if (
			nParam == VK_RETURN 
			//&&uParam == (unsigned int)(KWndWindow*)&m_PosXInput 
			//&&uParam == (unsigned int)(KWndWindow*)&m_PosYInput
			)
		{
			OnOK();
			nRet = 1;
		}
		else if (nParam == VK_ESCAPE)
		{
			g_pCoreShell->DirectFindPos(0, 0, TRUE, TRUE);
			CloseWindow();
			nRet = 1;
		}
		else if (nParam == VK_TAB)
		{
			if(FXY == 0)
			{
				KWndImage::Show();
				Wnd_SetFocusWnd(&m_PosYInput);
				Wnd_SetExclusive((KWndWindow*)this);
				FXY = 1;
			}
			else
			{
				KWndImage::Show();
				Wnd_SetFocusWnd(&m_PosXInput);
				Wnd_SetExclusive((KWndWindow*)this);
				FXY = 0;
			}
		}
		break;
	case WND_N_EDIT_CHANGE:
		OnCheckInput();
		break;
	default:
		nRet =  KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

void KUiFindPos::OnCheckInput()
{
	/*int nPass = 0;
	char	szBuff1[16], szBuff2[16];
	// toa do x
	nPass = 0;
	nPass = m_PosXInput.GetIntNumber();
	if (nPass < 0)
		nPass = 0;
	while (nPass > CHEST_INPUT_MAX_VALUE)
		nPass = nPass / 10;	
	itoa(nPass, szBuff1, 10);
	m_PosXInput.GetText(szBuff2, sizeof(szBuff2), true);
	if (strcmp(szBuff1, szBuff2))
		m_PosXInput.SetIntText(nPass);
		
	// toa do y
	nPass = 0;
	nPass = m_PosYInput.GetIntNumber();
	if (nPass < 0)
		nPass = 0;
	while (nPass > CHEST_INPUT_MAX_VALUE)
		nPass = nPass / 10;	
	itoa(nPass, szBuff1, 10);
	m_PosYInput.GetText(szBuff2, sizeof(szBuff2), true);
	if (strcmp(szBuff1, szBuff2))
		m_PosYInput.SetIntText(nPass);*/
}

void KUiFindPos::UpdateData1()
{
	m_InfoText.SetText("Xin nhËp täa ®é");
}

void KUiFindPos::Breathe()
{
	UpdateData();
}