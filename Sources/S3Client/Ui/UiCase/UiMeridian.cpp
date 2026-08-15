#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiBase.h"
#include "UiMeridian.h"

#include "../ShortcutKey.h"
#include "../../../core/src/coreshell.h"
#include <KTabFile.h>
extern iCoreShell* g_pCoreShell;

#define	SCHEME_INI				"Meridian\\Meridian.ini"
#define	SCHEME_INI_CONFIRM		"Meridian\\MeridianConfirm.ini"

KUiMeridian* KUiMeridian::m_pSelf = NULL;
KUiMeridianConfirm* KUiMeridianConfirm::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´̣¿ª´°¿Ú£¬·µ»ØÎ¨̉»µÄ̉»¸öÀà¶ÔÏóÊµÀư
//--------------------------------------------------------------------------
KUiMeridian* KUiMeridian::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiMeridian;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
		m_pSelf->Show();
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹Ø±Ơ´°¿Ú
//--------------------------------------------------------------------------
void KUiMeridian::CloseWindow()
{
	if (m_pSelf)
	{
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

KUiMeridian* KUiMeridian::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//³ơÊ¼»¯
void KUiMeridian::Initialize()
{
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	m_Style &= ~WND_S_VISIBLE;
	Wnd_AddWindow(this, WL_NORMAL);
	AddChild(&m_Background);
	AddChild(&m_LevelText);
	AddChild(&m_RankWorldText);
	int i = 0;
	for (i = 0; i < MAX_MERIDIAN+1; i++)
		AddChild(&m_Btn[i]);
	for (i = 0; i < 15; i++) {
		AddChild(&m_Connections[i]);
	}
	for (i = 0; i < MAX_MERIDIAN_LEVEL; i++) {
		AddChild(&m_MerLevelLabel[i]);
		AddChild(&m_Points[i]);
	}
	AddChild(&m_Close);

	memset(m_MeridianLevel, 0, sizeof(m_MeridianLevel));
	//Get Meridian data
	g_pCoreShell->GetGameData(GDI_PLAYER_MERIDIAN, (unsigned int)&m_MeridianLevel, sizeof(m_MeridianLevel));
}
extern int SCREEN_WIDTH;
//ÔØÈë½çĂæ·½°¸
void KUiMeridian::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	if (m_pSelf)
	{
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
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
}
void KUiMeridian::LoadScheme(class KIniFile* pIni)
{
	char	TabChat[32];
	m_Background.Init(pIni, "BackGround");
	int i = 0;
	for (i = 0; i < MAX_MERIDIAN+1; i++)
	{
		sprintf(TabChat, "Btn%d", i+1);
		m_Btn[i].Init(pIni, TabChat);
	}
	m_Close.Init(pIni, "CloseBtn");
	for (i = 0; i < 15; i++) {
		//m_Connections[i].Init(pIni, "ConnectionLine");
	}
}

//ÖØĐÂ³ơÊ¼»¯½çĂæ
void KUiMeridian::DefaultScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	if (m_pSelf)
	{
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
		{
			int	nValue1, nValue2;
			Ini.GetInteger("Main", "Left", 0, &nValue1);
			Ini.GetInteger("Main", "Top", 0, &nValue2);
			m_pSelf->SetPosition(nValue1, nValue2);
		}
	}
}

void KUiMeridian::UpdateMeridianLevel() {

	//Get Meridian data
	g_pCoreShell->GetGameData(GDI_PLAYER_MERIDIAN, (unsigned int)&m_MeridianLevel, sizeof(m_MeridianLevel));
}
void KUiMeridian::UpdateMeridian()
{
	//Update UI
	KIniFile	Ini;
	char		Buff[128];
	char		Buff1[128];
	char Scheme[256];
	char	Mer[32];
	char	Key[32];
	int left, top;
	int i = 0;
	int LR = 0; //0 L, 1 R
	g_UiBase.GetCurSchemePath(Scheme, 256);
	sprintf(Buff, "%s\\%s", Scheme, SCHEME_INI);
	
	//if (Ini.Load(Buff)) {
		for (i = 0; i < MAX_MERIDIAN_LEVEL; i++) {
			//sprintf_s(Mer, "Mer%dPoint_%d", btnNo, i);
			//m_Points[i].Init(&Ini, Mer);
			//m_Points[i].GetPosition(&left, &top);
			//m_Points[i].SetPosition(left + 33, top + 53);
			if (btnNo == 1) {
				//m_Points[i].SetPosition(left + 90, top + 53);
				if (m_MeridianLevel[i] == MAX_MERIDIAN_LEVEL / 2) {
					m_Points[i].SetFrame(1);
				}
			}
			if (btnNo >= 2 && m_MeridianLevel[btnNo - 2] > i && btnNo != 1) { //btnNo == 1 la Khi Doanh Dan Dien, xu ly rieng
				m_Points[i].SetFrame(1);
			}

			//Draw connection lines
			if (btnNo != 1 && i >= 1 && i <= 15) { //Not draw for Khi doanh dan dien
				m_Points[i-1].GetPosition(&left, &top);
				m_Connections[i - 1].SetPosition(left+5, top+3);
				int newleft, newtop = 0;
				m_Points[i].GetPosition(&newleft, &newtop);
				m_Connections[i - 1].SetEnd(newleft-left, newtop - top+3);
				if (m_Points[i].GetCurrentFrame() == 0) {//not active
					//set color for connection line
					m_Connections[i - 1].SetColor(0xff606060);
				}
				else {
					m_Connections[i - 1].SetColor(0xff00ffff);
				}
				m_Connections[i - 1].Show();
			}
			if (btnNo == 1 && i >= 1 && i <= 15) {
				m_Connections[i - 1].Hide();
			}

		}
	//}

}

void KUiMeridian::Breathe()
{
	int currentTick = GetTickCount();
	if (currentTick - this->lastTick >= 200) {
		this->lastTick = currentTick;
		UpdateData();
		UpdateMeridian();
	}
}

int KUiMeridian::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	KIniFile	Ini;
	char		Buff[128];
	char		Buff1[128];
	char Scheme[256];
	char	Mer[32];
	char	Key[32];
	int left, top;
	int i = 0;
	int LR = 0; //0 L, 1 R
	switch (uMsg)
	{
	case WND_M_MENUITEM_SELECTED:
		break;
	case WND_N_CHILD_MOVE:
	case WND_N_LIST_ITEM_SEL:
		break;
	case WND_M_POPUPMENU:
		break;
	case WND_N_BUTTON_CLICK:
	{
		//
		for (i = 0; i < MAX_MERIDIAN + 1; i++) {
			if (uParam == (unsigned int)(KWndWindow*)&m_Btn[i])
			{
				m_Btn[i].CheckButton(true);
				btnNo = i+1;
			}
			else {
				m_Btn[i].CheckButton(false);
			}
		}

		g_UiBase.GetCurSchemePath(Scheme, 256);
		sprintf(Buff, "%s\\%s", Scheme, SCHEME_INI);
		if (Ini.Load(Buff)) {
			for (i = 0; i < MAX_MERIDIAN_LEVEL; i++) {
				sprintf_s(Mer, "Mer%dPoint_%d", btnNo, i);
				m_Points[i].Hide();
				m_Points[i].Init(&Ini, Mer);
				m_Points[i].GetPosition(&left, &top);
				m_Points[i].SetPosition(left + 33, top + 53);
				if (btnNo == 1) {
					m_Points[i].SetPosition(left + 90, top + 53);
					if (m_MeridianLevel[i] == MAX_MERIDIAN_LEVEL / 2) {
						m_Points[i].SetFrame(1);
					}
				}
				if (btnNo >= 2 && m_MeridianLevel[btnNo - 2] > i && btnNo != 1) { //btnNo == 1 la Khi Doanh Dan Dien, xu ly rieng
					m_Points[i].SetFrame(1);
				}
				m_Points[i].Show();
				sprintf_s(Mer, "MerTxt");
				m_MerLevelLabel[i].Hide();
				m_MerLevelLabel[i].Init(&Ini, Mer);

				sprintf_s(Mer, "Mer%dLR", btnNo);
				sprintf_s(Key, "Acup_%d", i);
				Ini.GetInteger(Mer, Key, 0, &LR);

				sprintf_s(Mer, "Mer%dName", btnNo);
				sprintf_s(Key, "Name_%d", i);

				Ini.GetString(Mer, Key, "", Buff1, 128);
				m_MerLevelLabel[i].SetText(Buff1);
				m_Points[i].GetPosition(&left, &top);
				if (LR)
					left += 13;
				else
					left -= strlen(Buff1) * 9;
				m_MerLevelLabel[i].SetPosition(left, top);
				m_MerLevelLabel[i].Show();
			}
		}
		//Xy ly khi click
		if (Ini.Load(Buff)) {
			for (i = 0; i < MAX_MERIDIAN_LEVEL; i++) {
				if (uParam == (unsigned int)(KWndWindow*)&m_Points[i] && m_MeridianLevel[btnNo - 2] < i + 1 && btnNo != 1) {
					if (KUiMeridianConfirm::GetIfVisible())
					{
						KUiMeridianConfirm::CloseWindow();
						int left, top;
						KUiMeridian* self = KUiMeridian::GetIfVisible();
						self->GetPosition(&left, &top);
						self->SetPosition(left + 100, top);
					}
					else
					{
						int left, top;
						GetPosition(&left, &top);
						SetPosition(left - 100, top);
						KUiMeridianConfirm* pself = KUiMeridianConfirm::OpenWindow();
						char titleBuff[256];
						pself->SetMeridian(btnNo - 2, i + 1);
						sprintf_s(Mer, "Mer%dName", btnNo); //Huyet vi
						sprintf_s(Key, "Name_%d", i);
						Ini.GetString(Mer, Key, "", Buff1, 128);

						sprintf_s(Mer, "Btn%d", btnNo);				//Mach
						Ini.GetString(Mer, "Label", "", Buff, 128);
						sprintf_s(titleBuff, "§¶ Th«ng HuyÖt %s thuéc %s", Buff1, Buff);
						pself->SetTitle(titleBuff);
						pself->SetInfo();
					}
				}
			}
		}
		m_Btn[btnNo-1].CheckButton(true);
		if(uParam == (unsigned int)(KWndWindow*)&m_Close)
			KUiMeridian::CloseWindow();
		break;
	}
	default:
		return KWndWindow::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

KUiMeridianConfirm* KUiMeridianConfirm::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiMeridianConfirm;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
		m_pSelf->Show();
	return m_pSelf;
}

void KUiMeridianConfirm::CloseWindow()
{
	if (m_pSelf)
	{
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

KUiMeridianConfirm* KUiMeridianConfirm::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

void KUiMeridianConfirm::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	if (m_pSelf)
	{
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_CONFIRM);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->LoadScheme(&Ini);
		}
	}
}

void KUiMeridianConfirm::DefaultScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	if (m_pSelf)
	{
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_CONFIRM);
		if (Ini.Load(Buff))
		{
			int	nValue1, nValue2;
			Ini.GetInteger("Main", "Left", 0, &nValue1);
			Ini.GetInteger("Main", "Top", 0, &nValue2);
			m_pSelf->SetPosition(nValue1, nValue2);
		}
	}
}

void KUiMeridianConfirm::SetMeridian(int Type, int Level)
{
	this->m_nType = Type;
	this->m_nLevel = Level;
}

void KUiMeridianConfirm::Initialize()
{
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	m_Style &= ~WND_S_VISIBLE;
	Wnd_AddWindow(this, WL_TOPMOST);
	AddChild(&m_btnOk);
	AddChild(&m_btnClose);
	AddChild(&cbProtectionWay);
	AddChild(&cbEnhanceRateWay);
	AddChild(&Title);
	AddChild(&SuccessRate);
	AddChild(&Requirement);
	AddChild(&ProtectedTxt);
	AddChild(&EnhancedTxt);
	AddChild(&EffectTxt);
	int i = 0;
	for (i = 0; i < 2; i++) {
		AddChild(&ProtectTxt[i]);
	}
}

void KUiMeridianConfirm::Breathe()
{
}

void KUiMeridianConfirm::LoadScheme(KIniFile* pIni)
{
	m_btnOk.Init(pIni, "btnOk");
	m_btnClose.Init(pIni, "btnClose");
	cbNormalWay.Init(pIni, "cbNormalWay");
	cbProtectionWay.Init(pIni, "cbProtectionWay");
	cbEnhanceRateWay.Init(pIni, "cbEnhanceRateWay");
	int i = 0;
	char buff[32];
	for (i = 0; i < 2; i++) {
		sprintf_s(buff, "ProtectTxt%d", i+2);
		ProtectTxt[i].Init(pIni, buff);
	}
	Title.Init(pIni, "txtTitle");
	SuccessRate.Init(pIni, "SuccessRate");
	Requirement.Init(pIni, "Requirement");
	ProtectedTxt.Init(pIni, "ProtectedTxt");
	EnhancedTxt.Init(pIni, "EnhancedTxt");
	EffectTxt.Init(pIni, "EffectTxt");
}

int KUiMeridianConfirm::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int i = 0;
	int LR = 0; //0 L, 1 R
	switch (uMsg)
	{
	case WND_M_MENUITEM_SELECTED:
		break;
	case WND_N_CHILD_MOVE:
	case WND_N_LIST_ITEM_SEL:
		break;
	case WND_M_POPUPMENU:
		break;
	case WND_N_BUTTON_CLICK:
	{
		if (uParam == (unsigned int)(KWndWindow*)&m_btnOk) {
			//Xung huyet
			SetMeridianData	Data;

			Data.WayProtected = m_nWayProtect;
			Data.WayEnhanced = m_nWayEnhanced;
			Data.Type = m_nType;
			Data.Level = m_nLevel;
			g_pCoreShell->OperationRequest(GOI_SET_PLAYER_MERIDIAN, (unsigned int)&Data, 0);
			KUiMeridianConfirm::CloseWindow();

			int left, top;
			KUiMeridian* self = KUiMeridian::GetIfVisible();
			if (self) {
				self->GetPosition(&left, &top);
				self->SetPosition(left + 100, top);
			}
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_btnClose) {
			KUiMeridianConfirm::CloseWindow();

			int left, top;
			KUiMeridian* self = KUiMeridian::GetIfVisible();
			if (self) {
				self->GetPosition(&left, &top);
				self->SetPosition(left + 100, top);
			}
		}
		if (uParam == (unsigned int)(KWndWindow*)&cbEnhanceRateWay) {
			//Xung huyet Enhanced
			if (m_nWayEnhanced != enhancedway) {
				m_nWayEnhanced = enhancedway;
				m_nWayProtect = normalway;
				cbProtectionWay.CheckButton(false);
			}
			else {
				m_nWayEnhanced = normalway;
			}
		}
		if (uParam == (unsigned int)(KWndWindow*)&cbProtectionWay) {
			//Xung huyet Protect
			if (m_nWayProtect != protectedway) {
				m_nWayProtect = protectedway;
				m_nWayEnhanced = normalway;
				cbEnhanceRateWay.CheckButton(false);
			}
			else {
				m_nWayProtect = normalway;
			}
		}
		break;
	}
	default:
		return KWndWindow::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiMeridianConfirm::SetTitle(char* title)
{
	Title.SetText(title);
}

void KUiMeridianConfirm::SetInfo()
{
	KTabFile MeridiantSetting;
	char tmpStr[40];
	int RollbackLevel;
	int SuccessRate;
	int ChanNguyen;
	int HoMachDon;
	char tmpStr1[256];

	int TYLE_PHOTHONG[] = {	80,
							75,
							70,
							65,
							60,
							33,
							33,
							33,
							33,
							33,
							33,
							33,
							33,
							33,
							33,
							33,
							};
	int TYLE_HUYETVI[] = {	100,
							100,
							80,
							75,
							70,
							50,
							45,
							40,
							39,
							38,
							37,
							36,
							35,
							34,
							33,
							33,
							};
	int TYLE_LONGHON[] = {100,
							100,
							100,
							100,
							100,
							100,
							90,
							80,
							80,
							80,
							80,
							80,
							80,
							80,
							80,
							80,
							};

	MeridiantSetting.Load(MERIDIAN_SETTING_FILE);
	MeridiantSetting.GetString((m_nType) * MAX_MERIDIAN_LEVEL + m_nLevel + 1, 6, "", tmpStr, 40); //
	sprintf_s(tmpStr1, "T¨ng %s.", tmpStr);
	this->EffectTxt.SetText(tmpStr1);

	MeridiantSetting.GetInteger((m_nType) * MAX_MERIDIAN_LEVEL + m_nLevel + 1, 4, 0, &RollbackLevel);
	MeridiantSetting.GetInteger((m_nType) * MAX_MERIDIAN_LEVEL + m_nLevel + 1, 5, 0, &SuccessRate);
	MeridiantSetting.GetInteger((m_nType) * MAX_MERIDIAN_LEVEL + m_nLevel + 1, 10, 0, &ChanNguyen);
	MeridiantSetting.GetInteger((m_nType) * MAX_MERIDIAN_LEVEL + m_nLevel + 1, 11, 0, &HoMachDon);
	MeridiantSetting.GetString((m_nType)*MAX_MERIDIAN_LEVEL + RollbackLevel+1, 1, "", tmpStr, 40);

	if (RollbackLevel == 0)
		strcpy(tmpStr, "ban ®Çu");
	sprintf_s(tmpStr1, "TØ lÖ thµnh c«ng b×nh th­êng %d%%, Long §¬n %d%%, Long §»ng %d%%; thÊt b¹i trë l¹i huyÖt %s.", 
		TYLE_PHOTHONG[m_nLevel-1], TYLE_HUYETVI[m_nLevel-1], TYLE_LONGHON[m_nLevel-1], tmpStr);
	this->SuccessRate.SetText(tmpStr1);

	sprintf_s(tmpStr1, "§¶ th«ng huyÖt nµy cÇn %d Ch©n Nguyªn vµ %d Hé M¹ch §¬n.", ChanNguyen, HoMachDon);
	this->Requirement.SetText(tmpStr1);

	//sprintf_s(tmpStr1, "Dïng HuyÖt VÞ b¶o vÖ, khi xung huyÖt thÊt b¹i sÏ kh«ng bÞ lïi vÒ huyÖt vÞ thÊp h¬n.");
	//this->ProtectedTxt.SetText(tmpStr1);

	//sprintf_s(tmpStr1, "Dïng Long Hån hé thÓ sÏ ®¶m b¶o tØ lÖ thµnh c«ng.");
	//this->EnhancedTxt.SetText(tmpStr1);

}
