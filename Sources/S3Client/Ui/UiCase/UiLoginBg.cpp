/*****************************************************************************************
//	界面--login过程背景窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-12-16
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiLoginBg.h"

#define	SCHEME_INI				"UiLoginBg.ini"

KUiLoginBackGround* KUiLoginBackGround::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiLoginBackGround* KUiLoginBackGround::OpenWindow(const char* pszConfig)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiLoginBackGround;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		SetConfig(pszConfig);
		m_pSelf->Show();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口，同时可以选则是否删除对象实例
//--------------------------------------------------------------------------
void KUiLoginBackGround::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy == false)
		{
			m_pSelf->Hide();
		}
		else
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiLoginBackGround::Initialize()
{
	AddChild(&m_VersionWnd);
	m_szConfig[0] = 0;
	for (int i = 0; i < MAX_NUM_BUTTERFLY; i++)
	{
		m_uAdviceIntervalMin[i] = 0;
		m_uInterval[i] = 0;
		m_uLastSleepTime[i] = 0;
		AddChild(&m_Butterflys[i]); 
	}

	char	szVersion[80] = "";
	KIniFile* pSetting = g_UiBase.GetCommConfigFile();
	if (pSetting)
	{
		if (pSetting)
			pSetting->GetString("Main", "GameVersion", "", szVersion, sizeof(szVersion));
		g_UiBase.CloseCommConfigFile();
	}
	m_VersionWnd.SetText(szVersion);

	Wnd_AddWindow(this, WL_LOWEST);
}

//设置图形
void KUiLoginBackGround::SetConfig(const char* pszConfig)
{
	if (m_pSelf == NULL || pszConfig == NULL || pszConfig[0] == 0 ||
		strcmp(pszConfig, m_pSelf->m_szConfig) == 0)
	{
		return;
	}

	strncpy(m_pSelf->m_szConfig, pszConfig, sizeof(m_pSelf->m_szConfig));

	char Buff[256];
	KIniFile	Ini;
	g_UiBase.GetCurSchemePath(Buff, 256);
	strcat(Buff, "\\");
	strcat(Buff, SCHEME_INI);
	if (Ini.Load(Buff))
		m_pSelf->LoadScheme(&Ini);
}

//--------------------------------------------------------------------------
//  Hien lai nen login sau khi phat .jxr (KUiLogin::RestoreAfterReplay goi).
//  SetConfig se bo qua vi config khong doi, nen phai Show() truc tiep.
//--------------------------------------------------------------------------
void KUiLoginBackGround::Restore()
{
	if (m_pSelf)
		m_pSelf->Show();
}

void KUiLoginBackGround::LoadScheme(KIniFile* pIni)
{
	if (pIni)
	{
		char	Buff[128];
		strcpy(Buff, m_szConfig);
		Init(pIni, Buff);
		m_VersionWnd.Init(pIni, "VersionText");
		for (int i = 0; i < MAX_NUM_BUTTERFLY; i++)
		{
			sprintf(Buff, "%s_Butterfly_%d", m_szConfig, i);
			m_Butterflys[i].Init(pIni, Buff);
			m_uAdviceIntervalMin[i] = m_uAdviceIntervalMax[i] = 0;
			pIni->GetInteger2(Buff, "Interval", (int*)&m_uAdviceIntervalMin[i],
				(int*)&m_uAdviceIntervalMax[i]);
			if (m_uAdviceIntervalMin[i] && m_uAdviceIntervalMax[i] < m_uAdviceIntervalMin[i])
			{
				m_uAdviceIntervalMax[i] = m_uAdviceIntervalMin[i];
			}

			m_uInterval[i] = 0;
		}
	}
}

//活动函数
void KUiLoginBackGround::Breathe()
{
	for (int i = 0; i < MAX_NUM_BUTTERFLY; i++)
	{
		if (m_uAdviceIntervalMin[i])
		{
			if (m_uInterval[i] == 0)
			{
				if (m_Butterflys[i].NextFrame())
				{
					m_uLastSleepTime[i] = UiIR_GetCurrentTime();
					m_uInterval[i] = m_uAdviceIntervalMin[i] +
						(rand() % (m_uAdviceIntervalMax[i] - m_uAdviceIntervalMin[i]));
				}
			}
			else if (IR_IsTimePassed(m_uInterval[i], m_uLastSleepTime[i]))
			{
				m_Butterflys[i].UpdateTimer();
				m_uInterval[i] = 0;
			}
		}
		else
		{
			m_Butterflys[i].NextFrame();
		}
	}
}

#ifdef JX_ANDROID
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
extern iRepresentShell* g_pRepresentShell;	// [DANGNHAP 12/09] nhu WndImage.cpp
// [DANGNHAP 12/09] Chu: 'phan dang nhap phai lam cho vua kich thuoc man hinh tuy loai may'. Anh nen 800x600 (4:3) tren man 16:9..21:9:
// ve ban keo toan man lam nen toi (khong con dai den hai ben), roi ve ban that vua chieu cao, can giua (khong meo, khong cat).
void KUiLoginBackGround::PaintWindow()
{
	KRPosition2 oOff = {0, 0}, oCo = {0, 0};
	if (!g_pRepresentShell || !m_Image.szImage[0] || SCREEN_WIDTH <= 0 || SCREEN_HEIGHT <= 0
		|| !g_pRepresentShell->GetImageFrameParam(m_Image.szImage, m_Image.nFrame, &oOff, &oCo, m_Image.nType)
		|| oCo.nX <= 0 || oCo.nY <= 0)
	{
		KWndImage::PaintWindow();
		return;
	}
	KWndWindow::PaintWindow();
	KUiImageRef a = m_Image;
	unsigned int uMauGoc = a.Color.Color_dw;
	a.oPosition.nX = 0; a.oPosition.nY = 0; a.oPosition.nZ = 0;
	a.oEndPos.nX = SCREEN_WIDTH; a.oEndPos.nY = SCREEN_HEIGHT; a.oEndPos.nZ = 0;
	a.Color.Color_dw = 0xff484848;	// lop nen keo toan man, toi di
	g_pRepresentShell->DrawPrimitives(1, &a, RU_T_IMAGE_STRETCH, true);
	int nCao = SCREEN_HEIGHT;
	int nRong = oCo.nX * nCao / oCo.nY;
	if (nRong > SCREEN_WIDTH)
	{
		nRong = SCREEN_WIDTH;
		nCao = oCo.nY * nRong / oCo.nX;
	}
	int nX = (SCREEN_WIDTH - nRong) / 2, nY = (SCREEN_HEIGHT - nCao) / 2;
	a.Color.Color_dw = uMauGoc ? uMauGoc : 0xffffffff;
	a.oPosition.nX = nX; a.oPosition.nY = nY;
	a.oEndPos.nX = nX + nRong; a.oEndPos.nY = nY + nCao;
	g_pRepresentShell->DrawPrimitives(1, &a, RU_T_IMAGE_STRETCH, true);
}
#endif
