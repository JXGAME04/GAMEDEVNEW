// -------------------------------------------------------------------------
//	File : UiOptions2.cpp
//	Date: 2020-9-11 15:05:15
//	Author: Fong Ki“u
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/wnds.h"
#include "UiOptions2.h"
#include "UiInit.h"
#include "../ShortcutKey.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../UiBase.h"
#include "../Elem/PopupMenu.h"
#include "../UiSoundSetting.h"
#include "../UiChatPhrase.h"
#ifdef JX_MOBILE
#include "../../Platform/JxLiaCanh.h"	// [CAMERA 13/09 TUYCHON] JxLia_DatTuyChon
void JxNhip_VeNen(int nX, int nY, int nRong, int nCao);	// Platform/JxPerfHudAndroid.cpp: nen mo (nhu hang FPS cua KUiOptions)
extern "C" void JxHaoQuang_DatBat(int nQuai, int nTrangBi);	// [HAOQUANG 14/09] Core/Src/KNpc.cpp: vong hao quang quai / trang bi
extern "C" void JxVatRoi_DatBat(int nBat);					// Core/Src/KObj.cpp: cot sang + loe vat pham roi
#undef MAX_TOGGLE_BTN_COUNT
#define MAX_TOGGLE_BTN_COUNT 10	// [HAOQUANG 14/09] 10 nut; [CAMERA 13/09 TUYCHON] lop KUiOptions2 (header tra lai 4 cho KUiOptions) - dat SAU include cuoi
#endif
extern iCoreShell*	g_pCoreShell;

#define SCHEME_INI_OPTION		"UiOptions2.ini"
#define OPTIONS_SAVE_SECTION2	"Options2"

const char* ls_ToggleOptionName2[OPTION_INDEX_COUNT2] = 
{
	"GiamPlayer",
	"GiamNpc",
	"GiamMap",
	"GiamSkill",
#ifdef JX_MOBILE
	"LiaCanh", "NhinRong", "LiaVeNhanh",	// [CAMERA 13/09 TUYCHON] luu UiCommon.ini [Options2]
	"HaoQuangQuai", "HaoQuangTrangBi", "SangVatRoi",	// [HAOQUANG 14/09]
#endif
};

KUiOptions2* KUiOptions2::m_pSelf = NULL;

KUiOptions2::KUiOptions2()
{
	for(int i = 0; i < MAX_TOGGLE_BTN_COUNT; i++)
		m_ToggleItemList[i].bInvalid = false;
}

KUiOptions2* KUiOptions2::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

KUiOptions2* KUiOptions2::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiOptions2;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		LoadSetting(true, true);
		UiSoundPlay(UI_SI_POPUP_OUTGAME_WND);
		m_pSelf->Show();
		m_pSelf->BringToTop();
	}

	return m_pSelf;
}

void KUiOptions2::CloseWindow()
{
	if (m_pSelf)
	{
		m_pSelf->StoreSetting();
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

void KUiOptions2::Initialize()
{
	AddChild(&m_CloseBtn);
	for(int i = 0; i < MAX_TOGGLE_BTN_COUNT; i++) 
	{
		AddChild(&m_ToggleBtn[i]);
		AddChild(&m_StatusImage[i]);
	}
		
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	m_pSelf->LoadScheme(Scheme);
	Wnd_AddWindow(this);
}
extern int SCREEN_WIDTH;
void KUiOptions2::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_OPTION);
	if (m_pSelf && Ini.Load(Buff))
	{
		m_pSelf->LoadScheme(&Ini);
		if (SCREEN_WIDTH == 1024) {
			int nX, nY;
			int dX, dY;
			dX = (1024 - 800) / 2 + 8;
			dY = 0;

			m_pSelf->GetPosition(&nX, &nY);
			m_pSelf->SetPosition(nX + dX, nY + dY);
		}
	}
}

void KUiOptions2::LoadScheme(KIniFile* pIni)
{
	Init(pIni, "Main");	
	m_CloseBtn.Init(pIni, "CloseBtn");

	m_StatusImage[0].Init(pIni, "ToggleStatus");
	pIni->GetInteger("ToggleStatus", "NotCheckFrame", 0, &m_nStatusDisableFrame);
	pIni->GetInteger("ToggleStatus", "CheckFrame",    0, &m_nStatusEnableFrame);
	pIni->GetInteger("ToggleStatus", "DisableFrame",  0, &m_nStatusInvalidFrame);

	int	nIntervalH, nIntervalV, nCountPerLine;
	m_ToggleBtn[0].Init(pIni, "ToggleBtn");
	pIni->GetInteger2("ToggleBtn", "Interval", &nIntervalH, &nIntervalV);
	pIni->GetInteger("ToggleBtn", "CountPerLine", 1, &nCountPerLine);
	int	i, nBtnX, nBtnY, nImgX, nImgY;
	m_ToggleBtn[0].GetPosition(&nBtnX, &nBtnY);
	m_StatusImage[0].GetPosition(&nImgX, &nImgY);
	if (nCountPerLine < 1)
		nCountPerLine = 1;
	for (i = 1; i < MAX_TOGGLE_BTN_COUNT; i++)
	{
		int dx = (i % nCountPerLine) * nIntervalH;
		int dy = (i / nCountPerLine) * nIntervalV;
		m_ToggleBtn[0].Clone(&m_ToggleBtn[i]);
		m_ToggleBtn[i].SetPosition(nBtnX + dx,	nBtnY + dy);
		m_StatusImage[0].Clone(&m_StatusImage[i]);
		m_StatusImage[i].SetPosition(nImgX + dx, nImgY + dy);
	}

	for (i = 0; i < OPTION_INDEX_COUNT2; i++)
	{
		char	szKey[16];
		pIni->GetString("ToggleOptionsName", itoa(i, szKey, 10), "", m_ToggleItemList[i].szName, sizeof(m_ToggleItemList[i].szName));
	}

	m_nFirstControlableIndex = 0;
	m_nToggleBtnValidCount = (OPTION_INDEX_COUNT2 <= MAX_TOGGLE_BTN_COUNT) ? OPTION_INDEX_COUNT2 : MAX_TOGGLE_BTN_COUNT;
	UpdateAllToggleBtn();
	UpdateAllStatusImg();

	char		Buff[128];
	pIni->GetString("Settings", "EnableColor", "", Buff, sizeof(Buff));
	m_pSelf->m_uEnableTextColor = GetColor(Buff);
	pIni->GetString("Settings", "DisableColor", "", Buff, sizeof(Buff));
	m_pSelf->m_uDisableTextColor = GetColor(Buff);
	pIni->GetString("Settings", "InvalidColor", "", Buff, sizeof(Buff));
	m_pSelf->m_uInvalidTextColor = GetColor(Buff);
}

void KUiOptions2::ToggleOption(int nIndex)
{
	if (nIndex < OPTION_I_START2 || nIndex >= OPTION_INDEX_COUNT2)
		return;
	if (m_ToggleItemList[nIndex].bInvalid)
		return;
	bool bEnable = m_ToggleItemList[nIndex].bEnable = !m_ToggleItemList[nIndex].bEnable;
	switch(nIndex)
	{
	case OPTION_I_GIAMPLAYER:	
		if (g_pCoreShell)
			g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_QUALITY_GIAMPLAYER, bEnable);
		break;
	case OPTION_I_MATNPC:		
		if (g_pCoreShell)
			g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_QUALITY_MATNPC, bEnable);
		break;
	case OPTION_I_MATPLAYER:	
		if (g_pCoreShell)
			g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_QUALITY_MATPLAYER, bEnable);
		break;
	case OPTION_I_GIAMSKILL:
		if (g_pCoreShell)
			g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_QUALITY_GIAMSKILL, bEnable);
		break;
#ifdef JX_MOBILE
	case OPTION_I_LIA:
	case OPTION_I_NHINRONG:
	case OPTION_I_VENHANH:	// [CAMERA 13/09 TUYCHON] ap ngay; luu khi dong cua so (StoreSetting)
		JxLia_DatTuyChon(m_ToggleItemList[OPTION_I_LIA].bEnable, m_ToggleItemList[OPTION_I_NHINRONG].bEnable, m_ToggleItemList[OPTION_I_VENHANH].bEnable);
		break;
	case OPTION_I_HQQUAI:
	case OPTION_I_HQTRANGBI:	// [HAOQUANG 14/09] ap ngay
		JxHaoQuang_DatBat(m_ToggleItemList[OPTION_I_HQQUAI].bEnable, m_ToggleItemList[OPTION_I_HQTRANGBI].bEnable);
		break;
	case OPTION_I_SANGVATROI:
		JxVatRoi_DatBat(bEnable);
		break;
#endif
	}
	UpdateAllStatusImg();
}

void KUiOptions2::UpdateAllToggleBtn()
{
	int	i = 0;
	for (; i < m_nToggleBtnValidCount; i++)
	{
		m_ToggleBtn[i].Show();
		m_StatusImage[i].Show();
		int nIndex = m_nFirstControlableIndex + i;
		m_ToggleBtn[i].SetLabel(m_ToggleItemList[nIndex].szName);
		if (m_ToggleItemList[nIndex].bInvalid == false)
		{
			m_ToggleBtn[i].Enable(true);
			m_ToggleBtn[i].SetLabelColor(m_ToggleItemList[nIndex].bEnable ?
					m_uEnableTextColor : m_uDisableTextColor);
		}
		else
		{
			m_ToggleBtn[i].Enable(false);
			m_ToggleBtn[i].SetLabelColor(m_uInvalidTextColor);
		}
	}
	for (; i < MAX_TOGGLE_BTN_COUNT; i++)
	{
		m_ToggleBtn[i].Hide();
		m_StatusImage[i].Hide();
	}
}

void KUiOptions2::LoadSetting(bool bReload, bool bUpdateOption)
{
	int nSettingSet  = 0;
	int i;

	int bOptionsEnable[OPTION_INDEX_COUNT2] =
	{
		true, true, true, true
	};

	if (bReload == false && m_pSelf)
	{
		for (i = 0; i < OPTION_INDEX_COUNT2; i++)
			bOptionsEnable[i] = m_pSelf->m_ToggleItemList[i].bEnable;
	}
	else
	{
		KIniFile* pSetting = g_UiBase.GetAutoSettingFile();
		if (pSetting)
		{
			for (i = 0; i < OPTION_INDEX_COUNT2; i++)
				pSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[i], false, &bOptionsEnable[i]);
#ifdef JX_MOBILE
			pSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[OPTION_I_LIA], true, &bOptionsEnable[OPTION_I_LIA]);	// [CAMERA 13/09 TUYCHON] mac dinh BAT (chua co khoa)
			pSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[OPTION_I_NHINRONG], true, &bOptionsEnable[OPTION_I_NHINRONG]);
			pSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[OPTION_I_HQQUAI], true, &bOptionsEnable[OPTION_I_HQQUAI]);	// [HAOQUANG 14/09] mac dinh BAT
			pSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[OPTION_I_HQTRANGBI], true, &bOptionsEnable[OPTION_I_HQTRANGBI]);
			pSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[OPTION_I_SANGVATROI], true, &bOptionsEnable[OPTION_I_SANGVATROI]);
#endif
			g_UiBase.CloseAutoSettingFile(true);
		}		
		else
		{
			// [X64 08/09 NGOAIHINH FIX] Chua biet tai khoan (vua khoi dong UiInit.cpp:69, WM_ACTIVATEAPP UiShell.cpp:430 truoc dang nhap)
			// -> khong co tep cau hinh -> KHONG day mac dinh {true,true,true,true} (giam chi tiet nguoi choi / an NPC / an nguoi choi /
			// giam skill) xuong Core. Truoc day no lam Option.GetLow(LowPlayer)=1 -> SyncPlayer gan m_MaskType = ManTypeNameIdx (94)
			// cho CHINH MINH -> ve bang mau NPC enemy067 ('nhu dang mang mat na') toi lan dong bo sau (mac lai do). Do that x64 13:5x 08/09.
			return;
		}
	}

	if (bUpdateOption && g_pCoreShell)
	{
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_QUALITY_GIAMPLAYER, bOptionsEnable[OPTION_I_GIAMPLAYER]);
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_QUALITY_MATNPC, bOptionsEnable[OPTION_I_MATNPC]);
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_QUALITY_MATPLAYER, bOptionsEnable[OPTION_I_MATPLAYER]);
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_QUALITY_GIAMSKILL, bOptionsEnable[OPTION_I_GIAMSKILL]);
#ifdef JX_MOBILE
		JxLia_DatTuyChon(bOptionsEnable[OPTION_I_LIA], bOptionsEnable[OPTION_I_NHINRONG], bOptionsEnable[OPTION_I_VENHANH]);	// [CAMERA 13/09 TUYCHON] luc vao the gioi + moi lan nap lai
		JxHaoQuang_DatBat(bOptionsEnable[OPTION_I_HQQUAI], bOptionsEnable[OPTION_I_HQTRANGBI]);	// [HAOQUANG 14/09]
		JxVatRoi_DatBat(bOptionsEnable[OPTION_I_SANGVATROI]);
#endif
	}

	if (m_pSelf)
	{
		if (bReload)
		{
			for (i = 0; i < OPTION_INDEX_COUNT2; i++)
				m_pSelf->m_ToggleItemList[i].bEnable = bOptionsEnable[i];
			m_pSelf->UpdateAllToggleBtn();
			m_pSelf->UpdateAllStatusImg();
		}
	}
}

void KUiOptions2::UpdateAllStatusImg()
{
	int nFrame;
	for (int i = 0; i < m_nToggleBtnValidCount; i++)
	{
		if (m_ToggleItemList[m_nFirstControlableIndex + i].bInvalid == false)
		{
			nFrame = m_ToggleItemList[m_nFirstControlableIndex + i].bEnable ?
				m_nStatusEnableFrame : m_nStatusDisableFrame;
		}
		else
		{
			nFrame = m_nStatusInvalidFrame;
		}
		m_StatusImage[i].SetFrame(nFrame);
	}
}

int	 KUiOptions2::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	int nRet = 0;
	
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (KUPARAM)(KWndWindow*)&m_CloseBtn)
		{
			CloseWindow();
		}
		else
		{
			for (int i = 0; i < m_nToggleBtnValidCount; i++)
			{
				if (uParam == (KUPARAM)(KWndWindow*)&m_ToggleBtn[i])
				{
					ToggleOption(m_nFirstControlableIndex + i);
				}
			}
		}
		break;
	case WM_KEYDOWN:
		if (uParam  == VK_ESCAPE)
			CloseWindow();
		nRet = true;
		break;
	default:
		nRet = KWndWindow::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}

#ifdef JX_MOBILE
// [CAMERA 13/09 TUYCHON] nen mo sau cac hang cong tac: main2.spr trong suot o vung tren (cho hang Do sang / Nhac / Am thanh cua cua so chinh)
void KUiOptions2::PaintWindow()
{
	KWndImage::PaintWindow();
	if (m_nToggleBtnValidCount > 0)
	{
		int nX0 = 0, nY0 = 0, nX1 = 0, nY1 = 0, nRong = 0, nCao = 0, nW = 0, nH = 0;
		m_ToggleBtn[0].GetPosition(&nX0, &nY0);
		m_ToggleBtn[m_nToggleBtnValidCount - 1].GetPosition(&nX1, &nY1);
		m_ToggleBtn[0].GetSize(&nRong, &nCao);
		GetSize(&nW, &nH);
		JxNhip_VeNen(m_nAbsoluteLeft + 4, m_nAbsoluteTop + nY0 - 6, nW - 8, nY1 - nY0 + nCao + 12);
	}
}
#endif
void KUiOptions2::StoreSetting()
{
	KIniFile* pSetting = NULL;
	pSetting = g_UiBase.GetAutoSettingFile();

	if (pSetting)
	{
		for (int i = 0; i < OPTION_INDEX_COUNT2; i++)
		{
			pSetting->WriteInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[i], m_ToggleItemList[i].bEnable);
		}
	}
	g_UiBase.CloseAutoSettingFile(true);
}

