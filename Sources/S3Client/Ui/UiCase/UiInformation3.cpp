#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "UiInformation3.h"
#include "../../../core/src/coreshell.h"
#include "../../core/src/GameDataDef.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../Engine/src/Text.h"
//#include "../../../Engine/src/KDebug.h"

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI	"UiInformation3.ini"

KUiInformation3* KUiInformation3::m_pSelf = NULL;
//--------------------------------------------------------------------------
//	Open
//--------------------------------------------------------------------------
KUiInformation3* KUiInformation3::OpenWindow(const char* pszInitString, BYTE nImage)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiInformation3;
		if (m_pSelf)
		{
			m_pSelf->Initialize();
			m_pSelf->nTime = 0;				
		}	
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->ShowUi(pszInitString, nImage);
		m_pSelf->nTime = 0;	
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

KUiInformation3*	KUiInformation3::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	Khoi tao
//--------------------------------------------------------------------------
void KUiInformation3::Initialize()
{
	AddChild(&m_Information);
	AddChild(&m_Image);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	Wnd_AddWindow(this);
}

//--------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------
void KUiInformation3::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if (Ini.Load(Buff))
	{
		KWndShowAnimate::Init(&Ini, "Main");
		m_Information .Init(&Ini, "Info");	
		Ini.GetInteger("Info", "LifeTime", 0, &m_nLifeTime);		
	}
}

//--------------------------------------------------------------------------
//	Su kien
//--------------------------------------------------------------------------
int KUiInformation3::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		break;
	case WM_KEYDOWN:
		break;	
	default:
		return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

//--------------------------------------------------------------------------
//	Close
//--------------------------------------------------------------------------
void KUiInformation3::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy)
		{
			m_pSelf->nTime = 0;			
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		else
		{
			m_pSelf->nTime = 0;			
			m_pSelf->Hide();
		}	
	}
}

//--------------------------------------------------------------------------
//	Breathe chay lien tuc
//--------------------------------------------------------------------------
void KUiInformation3::Breathe()
{
	if (IsVisible())
		nTime++;
	if (nTime == m_nLifeTime*18)	
		CloseWindow(false);
}

//--------------------------------------------------------------------------
//	Show
//--------------------------------------------------------------------------
void KUiInformation3::ShowUi(const char* pszInitString, BYTE nImage)
{
	char szBuff[128],szImage[64];	
	int		nBtnPosX,nBtnPosY;
	if (pszInitString)
	{
		strcpy(szBuff, pszInitString);
		int nLen = strlen(szBuff);		
		nLen = TEncodeText(szBuff, nLen);				
		m_Information.SetText(szBuff,nLen);
	}

	KIniFile* pSetting = g_UiBase.GetGameSettingFile();
	if (pSetting)
	{
		sprintf(szBuff, "Image_%d", nImage);		
		pSetting->GetString("BattleRank", szBuff, "", szImage, sizeof(szImage));
		sprintf(szBuff, "Pos_%d", nImage);
		pSetting->GetInteger2("BattleRank", szBuff, &nBtnPosX, &nBtnPosY);
		m_Image.SetImage(ISI_T_SPR, szImage);
		m_Image.SetPosition(nBtnPosX,nBtnPosY);
	}

}