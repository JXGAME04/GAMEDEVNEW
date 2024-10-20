/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong Ki“u
//	CreateTime:	2020-8-16
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include <crtdbg.h>
#include "UiPlayerBar.h"
#include "UiStatus.h"
#include "UiSkillTree.h"
#include "UiEscDlg.h"
#include "UiSelColor.h"
#include "UiMsgCentrePad.h"
#include "UiFaceSelector.h"
#include "UiMiniMap.h"
#include "UiHelper.h"
#include "UiTongManager.h"
#include "UiAuto.h"
#include "UiAutoPlay.h"
#include "../Elem/MouseHover.h"
#include "../ShortcutKey.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/gamedatadef.h"
#include "../../../core/src/skilldef.h"
#include "../../../core/src/MsgGenreDef.h"
#include "../../../core/src/CoreObjGenreDef.h"
#include "../../../core/src/coreshell.h"
#include "../../TextCtrlCmd/TextCtrlCmd.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Engine/src/Text.h"
#include "../../core/src/GameDataDef.h"
#include "../../../Headers/KRelayProtocol.h"
#include "../../../Headers/KProtocolDef.h"
#include "../../../Headers/KProtocol.h"
#include "../../../Engine/src/KDebug.h"
#include "../TrayMode.h"
#include "malloc.h"

#include "UiChatCentre.h"
#include "../UiChatPhrase.h"
#include "../../Login/Login.h"

extern KUiChatPhrase g_UiChatPhrase;
extern iCoreShell*		g_pCoreShell;
extern iRepresentShell*	g_pRepresentShell;

#define	SCHEME_INI			"UiPlayerBar.ini"		//edit by phong kieu file ini quy dinh Ui player bar
#define	SCHEME_INI_MINI		"UiPlayerBarMini.ini"		//file ini quy dinh Ui toi uu hoa game mini
#define GAME_LOGO				"V‚ L©m Truy“n K˙"
#define	SWITCH_LOGO_INTERVAL	5000
#define	SCHEME_INI_STATE_POS	"\\Ui\\StatePos.ini"
#define	SEL_CHANNEL_MENU		1
#define	SEL_PHRASE_MENU			2

extern HINSTANCE    hInst;

const char*		s_TimeName[12] =
{
	"Nˆa Æ™m",
	"Gµ g∏y",
	"Rπng s∏ng",
	"M∆t trÍi m‰c",
	"Khi ®n",
	"G„c trong",
	"BuÊi tr≠a",
	"X’ chi“u",
	"GiÍ th©n",
	"NhÀp ngµy",
	"Hoµng h´n",
	"Nh©n Æﬁnh",
};

const char*		s_TimeStone[12] = 
{
	"T›.",
	"Sˆu",
	"D«n",
	"M∑o",
	"Th◊n",
	"Tﬁ.",
	"Ng‰.",
	"MÔi",
	"Th©n",
	"DÀu",
	"Tu t",
	"HÓi"
};

const char*		s_TimeLine[4] = { "MÈt", "Hai", "Ba", "BËn" };

int GetFormatedTimeString(int nTime, char* pString)
{
	nTime += 60;
	return sprintf(pString, "%s%sThÍi gian ±%s khæc",
		s_TimeName[(nTime / 120) % 12], s_TimeStone[(nTime / 120) % 12],
		s_TimeLine[(nTime % 120) / 30]);
}

#include "time.h"

int GetFormatedTimeString(struct tm* ptm, char* pString)
{
	return sprintf(pString, "%d:%02d | %d/%d/%d",
			ptm->tm_hour,
			ptm->tm_min,	
			ptm->tm_mday,
			ptm->tm_mon + 1,
			ptm->tm_year + 1900);
}

GameWorld_DateTime::GameWorld_DateTime()
{
	m_bTrueWorld = true;
	m_bLogo    = false;
	m_uLastSwitchTime = IR_GetCurrentTime();
	m_nNetStatus = 1; //mac dinh la mau vang
}

IMPLEMENT_COMCLASS(GameWorld_DateTime)
void GameWorld_DateTime::OnButtonClick()
{
	m_bTrueWorld = !m_bTrueWorld;
	UpdateData();
}

int	GameWorld_DateTime::Init(KIniFile* pIniFile, const char* pSection)
{
	if(KWndLabeledButton::Init(pIniFile, pSection))
	{
		pIniFile->GetInteger(pSection, "SmoothPing", 0, &m_nSmoothPing);
		pIniFile->GetInteger(pSection, "CrowdPing", 0, &m_nCrowdPing);
		pIniFile->GetInteger(pSection, "BlockPing", 0, &m_nBlockPing);

		pIniFile->GetString(pSection, "SmoothMsg", "", m_szSmoothMsg, sizeof(m_szSmoothMsg));
		pIniFile->GetString(pSection, "CrowdMsg", "", m_szCrowdMsg, sizeof(m_szCrowdMsg));
		pIniFile->GetString(pSection, "BlockMsg", "", m_szBlockMsg, sizeof(m_szBlockMsg));
		pIniFile->GetString(pSection, "GameLogo", GAME_LOGO, m_szLogo, sizeof(m_szLogo));

		char Buff[16];
		pIniFile->GetString(pSection, "Color", "0,0,0", Buff, sizeof(Buff));
		m_uDefaultColor = GetColor(Buff);
		pIniFile->GetString(pSection, "SmoothColor", "0,0,0", Buff, sizeof(Buff));
		m_uSmoothColor = GetColor(Buff);
		pIniFile->GetString(pSection, "CrowdColor", "0,0,0", Buff, sizeof(Buff));
		m_uCrowdColor = GetColor(Buff);
		pIniFile->GetString(pSection, "BlockColor", "0,0,0", Buff, sizeof(Buff));
		m_uBlockColor = GetColor(Buff);
	}
	return 1;
}

int GameWorld_DateTime::GetNetStatus()
{
	return m_nNetStatus;
}

void GameWorld_DateTime::UpdateData()
{
	char szTime[64];
	if(!m_bLogo && !m_bNetStatus)
	{
        if (!m_bTrueWorld && g_pCoreShell)
	    {
    		KUiSceneTimeInfo Info;
		    memset(&Info, 0, sizeof(KUiSceneTimeInfo));
			g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO, (unsigned int)&Info, 0);
		    int nLen = GetFormatedTimeString(Info.nGameSpaceTime, szTime);
	    }
	    else
	    {		
    		time_t curtm = ::time(NULL);
		    struct tm* ptm = localtime(&curtm);
		    int nLen = GetFormatedTimeString(ptm, szTime);
	    }
		SetLabelColor(m_uDefaultColor);
		SetLabel(szTime);
	}

	if (IR_IsTimePassed(SWITCH_LOGO_INTERVAL, m_uLastSwitchTime))
	{
		if(m_bLogo)
		{
			m_bLogo = false;
			UpdateNetStatus();
			m_bNetStatus = true;
		}
		else if(m_bNetStatus)
		{
			m_bNetStatus = false;
		}
		else
		{
			SetLabelColor(m_uDefaultColor);
			SetLabel(m_szLogo);
			m_bLogo = true;
		}
	}

	if (m_Flag & WNDBTN_F_OVER)
	{
		m_bLogo = false;
		m_bNetStatus = false;
		m_uLastSwitchTime = IR_GetCurrentTime();
	}

	NextFrame();
}

void GameWorld_DateTime::UpdateNetStatus()
{
	int i, nPing;
	char Number[32], Buff[32];
	unsigned int uDefaultColor;
	//
	if(g_pCoreShell) 
	{
		nPing = g_pCoreShell->GetPing();
	}
	itoa(nPing, Number, 10);
	if(nPing >= 0 && nPing <= m_nSmoothPing)
	{
		i = strlen(m_szSmoothMsg);
		memcpy(Buff, m_szSmoothMsg, i);
		Buff[i] = ' ';
		memcpy(Buff + i + 1, Number, strlen(Number) + 1);
		uDefaultColor = m_uSmoothColor;
		m_nNetStatus = 2; //mau xanh
	}
	else if(nPing > m_nSmoothPing && nPing <= m_nCrowdPing)
	{
		i = strlen(m_szCrowdMsg);
		memcpy(Buff, m_szCrowdMsg, i);
		Buff[i] = ' ';
		memcpy(Buff + i + 1, Number, strlen(Number) + 1);
		uDefaultColor = m_uCrowdColor;
		m_nNetStatus = 1; //mau vang
	}
	else
	{
		i = strlen(m_szBlockMsg);
		memcpy(Buff, m_szBlockMsg, i);
		Buff[i] = ' ';
		memcpy(Buff + i + 1, Number, strlen(Number) + 1);
		uDefaultColor = m_uBlockColor;
		m_nNetStatus = 0; //mau do mang yeu
	}
	//
	SetLabelColor(uDefaultColor);
	SetLabel(Buff);
	//
	KUiPlayerBar* pBar = KUiPlayerBar::GetIfVisible();
	if (pBar)
	{
		pBar->ChangeWifiStatus(m_nNetStatus);
	}
}

int	KImmediaItem::Init(KIniFile* pIniFile, const char* pSection)
{
	int nRet = KWndObjectBox::Init(pIniFile, pSection);
	if (nRet)
	{
		char ChName[64];
		pIniFile->GetString(pSection, "TextColor", "255, 255, 0", ChName, sizeof(ChName));	//m∑ mµu vœ sË l≠Óng item trong ´
		m_TextColor = GetColor(ChName);
		pIniFile->GetString(pSection, "TextColorIndex", "162, 206, 176", ChName, sizeof(ChName)); //m∑ mµu vœ ´ ph›m tæt
		m_TextColorIndex = GetColor(ChName);
		return true;
	}
	return nRet;
}

void KImmediaItem::PaintWindow()
{
	KWndObjectBox::PaintWindow();
	if (g_pCoreShell)
	{
		int nNum = g_pCoreShell->GetGameData(GDI_IMMEDIATEITEM_NUM, m_nIndex, 0);

		if (m_nOrder)
		{
			if (nNum)
			{
				char szNum[4];
				sprintf(szNum, "%d", m_nIndex + 1);
				g_pRepresentShell->OutputText(12, szNum, KRF_ZERO_END, m_nAbsoluteLeft + 7, m_nAbsoluteTop + 4, m_TextColorIndex);
			}
		}
		
		if (nNum > 1 && nNum < 1000)
		{
			int nFontSize = 12;
			char szNum[4];
			int nLen = sprintf(szNum, "%d", nNum);
			szNum[3] = 0;
			g_pRepresentShell->OutputText(nFontSize, szNum, KRF_ZERO_END, m_nAbsoluteLeft + m_Width - nLen * (nFontSize + 1) / 2, m_nAbsoluteTop + m_Height - nFontSize - 1, m_TextColor);
		}
	}
}

KUiPlayerBar* KUiPlayerBar::m_pSelf = NULL;
KUiPlayerBar::KStatePos*	KUiPlayerBar::ms_pStateList = NULL;
int	KUiPlayerBar::ms_nNumState = 0;

KUiPlayerBar::KUiPlayerBar()
{
	m_cPreMsgCounter = 0;
	m_cLatestMsgIndex= 0;
	m_bMiniMode = false;
	for (int i = 0; i < MAX_RECENT_MSG_COUNT; i++) 
	{
		m_RecentMsg[i][0] = 0;
	}
	m_szSelfName[0] = 0;

	m_nCurChannel = -1;
	m_nRecentPlayerName = 0;

	memset(m_RecentPlayerName, 0, sizeof(m_RecentPlayerName));

	m_nExperienceFull = 0;
	m_nCurLevelExperience = 0;
	m_nExperience = 0;
	//chat item
	ZeroMemory(m_ChatItemInfo,sizeof(m_ChatItemInfo));//
	ZeroMemory(m_ItemName,sizeof(m_ItemName));//
//	ministate	
	m_nCurrIndex = -1;
	nX = 0;
	nY = 0;
	nBegin = 0;
	nEnd = 0;	
}

KUiPlayerBar::~KUiPlayerBar()
{
}

KUiPlayerBar* KUiPlayerBar::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

KUiPlayerBar* KUiPlayerBar::OpenWindow()
{
	KUiChatCentre::OpenWindow(false);

	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiPlayerBar;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		m_pSelf->UpdateData();
		m_pSelf->Show();
	}

	m_pSelf->m_nCurChannel = -1;
	m_pSelf->m_nRecentPlayerName = 0;

	return m_pSelf;
}

void KUiPlayerBar::OnSwitchSize()
{
	m_bMiniMode = !m_bMiniMode;
	char Scheme[128];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
}

void KUiPlayerBar::UpdateData()
{
	//_ASSERT(g_pCoreShell);
	KUiPlayerBaseInfo	Info;
	memset(&Info, 0, sizeof(KUiPlayerBaseInfo));
	g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (int)&Info, 0);
	strcpy(m_szSelfName, Info.Name);

	int nA;
	int nA2;
	UpdateXXXNumber(nA, nA2);

	KUiPlayerImmedItemSkill immedItemSkillInfo;
	memset(&immedItemSkillInfo, 0, sizeof(KUiPlayerImmedItemSkill));
	g_pCoreShell->GetGameData(GDI_PLAYER_IMMED_ITEMSKILL, (int)&immedItemSkillInfo, 0);
	for (int i = 0; i < UPB_IMMEDIA_ITEM_COUNT; i++)
	{
		m_ImmediaItem[i].HoldObject(immedItemSkillInfo.ImmediaItem[i].uGenre, immedItemSkillInfo.ImmediaItem[i].uId, 0, 0);
	}
	
	m_ImmediaSkill[0].HoldObject(immedItemSkillInfo.IMmediaSkill[0].uGenre, immedItemSkillInfo.IMmediaSkill[0].uId, 0, 0);
	m_ImmediaSkill[1].HoldObject(immedItemSkillInfo.IMmediaSkill[1].uGenre, immedItemSkillInfo.IMmediaSkill[1].uId, 0, 0);

	int nM;
	int nL;
	UpdateRuntimeAttribute(nM, nL);
}

void KUiPlayerBar::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		m_pSelf->m_cLatestMsgIndex = 0;
		m_pSelf->m_cPreMsgCounter = 0;
		for(int i = 0; i < MAX_RECENT_MSG_COUNT; i++)
			m_pSelf->m_RecentMsg[i][0] = 0;

		if (bDestroy == false)
			m_pSelf->Hide();
		else
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

void KUiPlayerBar::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s" , pScheme, m_pSelf->m_bMiniMode ? SCHEME_INI_MINI : SCHEME_INI);
		if (Ini.Load(Buff))
			m_pSelf->LoadScheme(&Ini);
	}
}

unsigned int s_CanFocusColor = 0;
unsigned int s_NotCanFocusColor = 0;

void KUiPlayerBar::LoadScheme(KIniFile* pIni)
{
	_ASSERT(pIni);
	int i = 0;
	Init(pIni, $Main);

	m_ChatBar  .Init(pIni, "InputBack");
	m_ShorcutKeyBar  .Init(pIni, "ShorcutKeyBar");
	m_DateTime.Init(pIni, "DateTime");
	
	char	szBuffer[16];
	for (i = 0; i < UPB_IMMEDIA_ITEM_COUNT; i++)
	{
		sprintf(szBuffer, "Item_%d", i);
		m_pSelf->m_ImmediaItem[i].Init(pIni, szBuffer);
		pIni->GetInteger(szBuffer, "Order", 1, &m_pSelf->m_ImmediaItem[i].m_nOrder);
		m_pSelf->m_ImmediaItem[i].m_nIndex = i;
	}
	
	m_ImmediaSkill[0].Init(pIni, "ImediaLeftSkill");
	m_ImmediaSkill[1].Init(pIni, "ImediaRightSkill");
	m_ImmediaSkill[0].EnablePickPut(false);
	m_ImmediaSkill[1].EnablePickPut(false);

	m_InputEdit	.Init(pIni, "InputEdit");
	s_CanFocusColor = m_InputEdit.GetFocusBkColor();

	if (pIni->GetString("InputEdit", "FocusNoCanBKColor", "0,0,0", szBuffer, sizeof(szBuffer)))
	{
		s_NotCanFocusColor = (GetColor(szBuffer) & 0xffffff);
		int	nValue;
		if (pIni->GetInteger("InputEdit", "FocusNoCanBKColorAlpha", 128, &nValue))
		{
			if (nValue > 255 || nValue < 0)
				s_NotCanFocusColor |= 0x16000000;
			else
				s_NotCanFocusColor |= (((255 - nValue) << 21) & 0xff000000);
		}
	}
	else
	{
		KRColor nColor;
		nColor.Color_dw = s_CanFocusColor;
		nColor.Color_b.a = 12;
		nColor.Color_b.r = 108;
		s_NotCanFocusColor = nColor.Color_dw;
	}

	m_Face    .Init(pIni, "Face");
	m_SendBtn	.Init(pIni, "SendBtn");
	m_ChannelSwitchBtn.Init(pIni, "ChannelBtn");
	m_ChannelOpenBtn.Init(pIni, "OpenChannelBtn");
	m_Friend.Init(pIni, "Friend");
	m_Options.Init(pIni, "Options");
	m_ChatRoom.Init(pIni, "ChatRoom");
	m_Status.Init(pIni, "Status");
	m_Items.Init(pIni, "Items");
	m_ItemEx.Init(pIni, "ItemEx");
	m_Skills.Init(pIni, "Skills");
	m_Team.Init(pIni, "Team");
	m_Faction.Init(pIni, "Faction");	
	m_Market.Init(pIni, "Market");	
	m_WifiStatus.Init(pIni, "WifiStatus");
	m_WifiStatus.SetFrame(2);
	m_Auto.Init(pIni, "Auto");
	m_SwitchBtn .Init(pIni, "SwitchSizeBtn");
    m_HideWindow.Init(pIni, "HideWindow");
	pIni->GetInteger2("BuffPos", "XY", &nX, &nY);
	pIni->GetInteger2("BuffPos", "End", &nBegin, &nEnd);

	int nMaxItemRow = 10;
	int nHeightAdd = 27;
	int nCurRow = 1;
	int nX1 = nX;

	pIni->GetInteger2("BuffPos", "MaxIHeight", &nMaxItemRow, &nHeightAdd);

	for (i = 0; i < MAX_BUTTON_STATE; i++)
	{
		m_pSelf->m_StateImg[i].Init(pIni, "BuffImage");
		m_pSelf->m_StateImg[i].SetPosition(nX, nY);
		m_pSelf->m_StateLife[i].Init(pIni, "txtBuffTime");
		m_pSelf->m_StateLife[i].SetPosition(nX, nY+nEnd);
		nX += nBegin;
		//add by Fong Ki“u
		if(i == (nMaxItemRow * nCurRow) - 1)
		{
			nX = nX1;
			nY = nY + nEnd + nHeightAdd;
			nCurRow = nCurRow + 1;
		}
	}		
	
	// chat item
	for (i = 0; i < MAX_ITEMBUTTON; i++)
	{
		m_ItemBtn[i].Init(pIni, "ChatItem");
		m_ItemBtn[i].SetPosition(INVISIBLE_POS);
	}
	
}

void KUiPlayerBar::Initialize()
{
	int i = 0;
	AddChild(&m_DateTime);
	AddChild(&m_ChatBar);
	AddChild(&m_ShorcutKeyBar);

	for (i = 0; i < UPB_IMMEDIA_ITEM_COUNT; i++)
	{
		m_ImmediaItem[i].SetObjectGenre(CGOG_ITEM);
		m_ImmediaItem[i].HoldObject(CGOG_NOTHING, 0, 0, 0);
		AddChild(&m_ImmediaItem[i]);
		m_ImmediaItem[i].SetContainerId((int)UOC_IMMEDIA_ITEM);
	}
	
	AddChild(&m_ImmediaSkill[0]);
	AddChild(&m_ImmediaSkill[1]);
	m_ImmediaSkill[0].SetContainerId((int)UOC_IMMEDIA_SKILL);
	m_ImmediaSkill[1].SetContainerId((int)UOC_IMMEDIA_SKILL);

	AddChild(&m_InputEdit);
	AddChild(&m_ChannelSwitchBtn);
	AddChild(&m_Face);
	AddChild(&m_SendBtn);
	AddChild(&m_SwitchBtn);
	AddChild(&m_ChannelOpenBtn);
	AddChild(&m_Friend);
	AddChild(&m_Options);
	AddChild(&m_ChatRoom);
	AddChild(&m_Status);
	AddChild(&m_Items);
	AddChild(&m_ItemEx);
	AddChild(&m_Skills);
	AddChild(&m_Team);
	AddChild(&m_Faction);	
	AddChild(&m_Market);
	AddChild(&m_WifiStatus);
	AddChild(&m_Auto);	
	AddChild(&m_HideWindow);		
	
	for (i = 0; i < MAX_BUTTON_STATE; i++)
	{
		AddChild(&m_StateImg[i]);
		AddChild(&m_StateLife[i]);
	}
	
	for (i = 0; i < MAX_ITEMBUTTON; i++)
		AddChild(&m_ItemBtn[i]);	

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	m_pSelf->LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

int KUiPlayerBar::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_M_POPUPMENU:
		if (uParam == (unsigned int)(KWndWindow*)(&m_InputEdit))
		{
			PopupPhraseMenu(LOWORD(nParam), HIWORD(nParam), true);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_ChannelSwitchBtn)
		{
			int x, y;
			m_ChannelSwitchBtn.GetAbsolutePos(&x, &y);
			PopupChannelMenu(x, y);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_ChannelOpenBtn)
		{
		}
		break;
	case WND_N_ITEM_PICKDROP:
		if (g_UiBase.IsOperationEnable(UIS_O_MOVE_ITEM))
			OnObjPickedDropped((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		break;
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_Face)
			KUiFaceSelector::OpenWindow(this, 0);
		else if (uParam == (unsigned int)(KWndWindow*)&m_Market)
		{	
			g_pCoreShell->OperationRequest(GOI_SUPERSHOP, 0, 0);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_Friend)
			KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_FRIEND);
		else if (uParam == (unsigned int)(KWndWindow*)&m_Options)
			KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_SYSTEM);	
		//else if (uParam == (unsigned int)(KWndWindow*)&m_ChatRoom)		
		//	g_pCoreShell->OperationRequest(GOI_MSG_HTCN, 0, 0);
		else if (uParam == (unsigned int)(KWndWindow*)&m_Status)
			KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_STATUS);	
		else if (uParam == (unsigned int)(KWndWindow*)&m_Items)
			KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_ITEMS);
		//else if (uParam == (unsigned int)(KWndWindow*)&m_ItemEx)	
		//	g_pCoreShell->OperationRequest(GOI_ITEM_EX, 0, 0);		
		else if (uParam == (unsigned int)(KWndWindow*)&m_Skills)
			KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_SKILLS);	
		else if (uParam == (unsigned int)(KWndWindow*)&m_Team)
			KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_TEAM);	
		else if (uParam == (unsigned int)(KWndWindow*)&m_Faction)
			KUiTongManager::OpenWindow(NULL);				
		else if (uParam == (unsigned int)(KWndWindow*)&m_SendBtn)
			OnSend(false);
		else if (uParam == (unsigned int)(KWndWindow*)&m_ChannelSwitchBtn)
		{
			int x, y;
			m_ChannelSwitchBtn.GetAbsolutePos(&x, &y);
			PopupChannelMenu(x, y);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_ChannelOpenBtn)
		{
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_SwitchBtn)
			OnSwitchSize();
        else if (uParam == (unsigned int)(KWndWindow*)&m_HideWindow)
		{
			gTrayMode.HideNotify(hInst);			
		}	
		break;
	case WND_N_LEFT_CLICK_ITEM:
		if (nParam == (int)(KWndWindow*)&m_ImmediaSkill[0])
			KUiSkillTree::OpenWindow(true);
		else if (nParam == (int)(KWndWindow*)&m_ImmediaSkill[1])
			KUiSkillTree::OpenWindow(false);
		break;
	case WND_N_RIGHT_CLICK_ITEM:
		if (g_UiBase.IsOperationEnable(UIS_O_USE_ITEM))
		{
			for (int i = 0; i < UPB_IMMEDIA_ITEM_COUNT; i++)
				if (nParam == (int)(KWndWindow*)&m_ImmediaItem[i])
					OnUseItem(i);
		}
		break;
	case WND_N_EDIT_SPECIAL_KEY_DOWN:
		if (nParam == VK_RETURN)
		{
			OnSend((GetKeyState(VK_CONTROL) & 0x8000));
			Wnd_SetFocusWnd(NULL);
			nRet = 1;
		}
		else if (nParam == VK_UP || nParam == VK_DOWN)
		{
			if (GetKeyState(VK_CONTROL) & 0x8000)
			{
				int x, y;
				m_InputEdit.GetAbsolutePos(&x, &y);
				int nCX = 0;
				m_InputEdit.GetCaretPosition(&nCX, NULL);
				PopupPhraseMenu(x + nCX, y, nParam == VK_DOWN);
			}
			else
			{
				InputRecentMsg(nParam == VK_UP);
				nRet = 1;
			}
		}
		else if (nParam == VK_PRIOR || nParam == VK_NEXT)
		{
			if (m_nCurChannel >= 0)
			{
				int nToChannel;
				if (nParam == VK_PRIOR)
					nToChannel = (m_nCurChannel - 1 >= 0) ? m_nCurChannel - 1 : KUiMsgCentrePad::GetChannelCount() + m_nRecentPlayerName - 1;
				else
					nToChannel = (DWORD(m_nCurChannel + 1) < (KUiMsgCentrePad::GetChannelCount() + m_nRecentPlayerName)) ? m_nCurChannel + 1 : 0;

				if (nToChannel != m_nCurChannel)
				{
					InputCurrentChannel(nToChannel, true);
				}
			}				
			nRet = 1;
		}	
		else if (nParam == VK_ESCAPE)
		{
			Wnd_SetFocusWnd(NULL);
			nRet = 1;
		}
		break;
	case WND_N_EDIT_CHANGE:
		m_cPreMsgCounter = 0;
		break;		
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if (HIWORD(nParam) == SEL_CHANNEL_MENU)
			{
				if (short(LOWORD(nParam)) >= 0)
				{
					InputCurrentChannel(LOWORD(nParam), true);
				}

				KPopupMenu::Cancel();
			}
			else if (HIWORD(nParam) == SEL_PHRASE_MENU)
			{
				if (short(LOWORD(nParam)) >= 0)
				{
					char szPhrase[64];
					int nLen = g_UiChatPhrase.GetPhrase(LOWORD(nParam), szPhrase);
					m_InputEdit.InsertString(szPhrase, nLen);
				}
			}
		}
		break;
	case WND_M_MENUITEMRIGHT_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if (HIWORD(nParam) == SEL_CHANNEL_MENU)
			{
				if (short(LOWORD(nParam)) >= 0)
				{
					if (KUiMsgCentrePad::CheckChannel(LOWORD(nParam), !KUiMsgCentrePad::GetChannelSubscribe(LOWORD(nParam))))
					{
						KPopupMenuData* p = KPopupMenu::GetMenuData();
						if (p)
						{
							PopupChannelMenu(p->nXX, p->nYY);
						}
					}
					else
						KPopupMenu::Cancel();
				}
				else
					KPopupMenu::Cancel();
			}
		}
		break;
	case WM_MOUSEMOVE:
		{
			int x = (int)(short)(LOWORD(nParam));
			int y = (int)(short)(HIWORD(nParam));
			int nIndex = GetStateAtPos(x, y);
			if (nIndex != m_nCurrIndex)
			{
				m_nCurrIndex = nIndex;
				//UpdateStateTip(x, y);
			}
		}
		nRet = 1;
		break;
	case WND_N_BUTTON_OVER:
		if (m_nCurrIndex >= 0)
		{
			m_nCurrIndex = -1;
			//UpdateStateTip(0, 0);
		}
		break;		
	case WND_M_OTHER_WORK_RESULT:
		if (nParam)
		{
			m_InputEdit.InsertString((const char*)nParam, -1);
			Wnd_SetFocusWnd(&m_InputEdit);
		}
		break;
	default:
		nRet = KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

void KUiPlayerBar::PaintWindow()
{
	KWndImage::PaintWindow();

	int nChannelDataCount = KUiMsgCentrePad::GetChannelCount() + m_nRecentPlayerName;
	if (m_nCurChannel < 0 || m_nCurChannel >= nChannelDataCount)
	{
		SetCurrentChannel(0);
	}
}

void KUiPlayerBar::ChangeWifiStatus(int nStatus)
{
	m_WifiStatus.SetFrame(nStatus);
}

void KUiPlayerBar::PopupChannelMenu(int x, int y)
{
	int nChannelDataCount = KUiMsgCentrePad::GetChannelCount();
	if (nChannelDataCount + m_nRecentPlayerName <= 0)
		return;

	KPopupMenuData* pMenuData = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nChannelDataCount + m_nRecentPlayerName));
	if (pMenuData == NULL)
	{
		return;
	}

	KPopupMenu::InitMenuData(pMenuData, nChannelDataCount + m_nRecentPlayerName);
	pMenuData->usMenuFlag &= ~PM_F_HAVE_HEAD_TAIL_IMG;
	pMenuData->usMenuFlag |= (PM_F_AUTO_DEL_WHEN_HIDE | PM_F_CANCEL_BY_CALLER);

	pMenuData->nItemTitleIndent = 0;
	pMenuData->byItemTitleUpSpace = 0;

	int i;
	pMenuData->nItemHeight = 0;
	WORD nHei = 0;
	char szText[64];
	for (i = 0; i < nChannelDataCount; i++)
	{
		WORD nPicIndex = -1;
		int nOffset = 0;
		KRColor uColor;
		KRColor uBKColor;
		short nCheckPicIndex = -1;
		if (KUiMsgCentrePad::GetChannelMenuinfo(i, &nPicIndex, &nHei, &uColor, &uBKColor, szText, &nCheckPicIndex))
		{
			pMenuData->Items[i].szData[0] = KTC_INLINE_PIC;
			*((WORD*)(pMenuData->Items[i].szData + 1)) = nPicIndex;
			pMenuData->Items[i].szData[3] = uColor.Color_b.a;
			pMenuData->Items[i].szData[4] = uColor.Color_b.r;
			pMenuData->Items[i].szData[5] = uColor.Color_b.g;
			pMenuData->Items[i].szData[6] = uColor.Color_b.b;
			nOffset = 7;
			if (nHei > pMenuData->nItemHeight)
				pMenuData->nItemHeight = nHei;
			strcpy(pMenuData->Items[i].szData + nOffset, szText);
			pMenuData->Items[i].uDataLen = strlen(szText) + nOffset;
			
			pMenuData->Items[i].szData[pMenuData->Items[i].uDataLen++] = KTC_TAB;

			if (nCheckPicIndex != -1)
			{
				pMenuData->Items[i].szData[pMenuData->Items[i].uDataLen] = KTC_INLINE_PIC;
				*((WORD*)(pMenuData->Items[i].szData + pMenuData->Items[i].uDataLen + 1)) = nCheckPicIndex;
				pMenuData->Items[i].uDataLen += 3;
			}
		}
		pMenuData->Items[i].uBgColor = uBKColor.Color_dw;
		pMenuData->usMenuFlag |= PM_F_TAB_SPLIT_ITEM_TEXT;
	}
	if (nHei > 0)
        pMenuData->nItemRightWidth = MENU_ITEM_DEFAULT_RIGHT_WIDTH;
	else
		pMenuData->nItemRightWidth = 0;

	for (; i < nChannelDataCount + m_nRecentPlayerName; i++)
	{
		if (m_RecentPlayerName[i - nChannelDataCount][0] != 0)
		{
			WORD nPicIndex = -1;
			int nOffset = 0;
			KRColor uColor;
			KRColor uBKColor;
			if (KUiMsgCentrePad::GetPeopleMenuinfo(m_RecentPlayerName[i - nChannelDataCount], &nPicIndex, &nHei, &uColor, &uBKColor))
			{
				pMenuData->Items[i].szData[0] = KTC_INLINE_PIC;
				*((WORD*)(pMenuData->Items[i].szData + 1)) = nPicIndex;
				pMenuData->Items[i].szData[3] = uColor.Color_b.a;
				pMenuData->Items[i].szData[4] = uColor.Color_b.r;
				pMenuData->Items[i].szData[5] = uColor.Color_b.g;
				pMenuData->Items[i].szData[6] = uColor.Color_b.b;
				nOffset = 7;
				if (nHei > pMenuData->nItemHeight)
					pMenuData->nItemHeight = nHei;
			}
			strcpy(pMenuData->Items[i].szData + nOffset, m_RecentPlayerName[i - nChannelDataCount]);
			pMenuData->Items[i].uDataLen = strlen(m_RecentPlayerName[i - nChannelDataCount]) + nOffset;
			pMenuData->Items[i].uBgColor = uBKColor.Color_dw;
		}
		else
		{
			break;
		}
	}
	pMenuData->nNumItem = i;

	pMenuData->nX = x;
	pMenuData->nY = y;
	pMenuData->nSelectedItem = m_nCurChannel;
	KPopupMenu::Popup(pMenuData, (KWndWindow*)this, SEL_CHANNEL_MENU);
}

void KUiPlayerBar::PopupPhraseMenu(int x, int y, bool bFirstItem)
{
	int nDataCount = g_UiChatPhrase.GetPhraseCount();
	if (nDataCount <= 0)
		return;
	KPopupMenuData* pMenuData = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nDataCount));
	if (pMenuData == NULL)
	{
		return;
	}

	KPopupMenu::InitMenuData(pMenuData, nDataCount);
	pMenuData->usMenuFlag &= ~PM_F_HAVE_HEAD_TAIL_IMG;
	pMenuData->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	pMenuData->nItemTitleIndent = 0;
	pMenuData->byItemTitleUpSpace = 0;

	int i;
	for (i = 0; i < nDataCount; i++)
	{
		pMenuData->Items[i].uDataLen = g_UiChatPhrase.GetPhrase(i, pMenuData->Items[i].szData);
	}
	pMenuData->nNumItem = nDataCount;

	pMenuData->nX = x;
	pMenuData->nY = y;
	pMenuData->nSelectedItem = bFirstItem ? 0 : nDataCount - 1;
	KPopupMenu::Popup(pMenuData, (KWndWindow*)this, SEL_PHRASE_MENU);
}

#include "../ChatFilter.h"
extern CChatFilter g_ChatFilter;

void KUiPlayerBar::OnSend(BOOL bDirectSend)
{
	char	Buffer[512];
	int     nName = 0;
	int	    nMsgLength = m_InputEdit.GetText(Buffer, sizeof(Buffer), true);

	if (nMsgLength <= 0)
		return;
	
	if (strstr(Buffer, "%dbao"))
	{
		m_InputEdit.ClearText();
		g_pCoreShell->OperationRequest(GOI_PLAYER_ACTIONCHAT, (unsigned int)&Buffer, NULL);
		return;
	}	
		
	memcpy(m_RecentMsg[m_cLatestMsgIndex], Buffer, nMsgLength);

	m_RecentMsg[m_cLatestMsgIndex][nMsgLength] = 0;
	m_cLatestMsgIndex = (m_cLatestMsgIndex + 1) % MAX_RECENT_MSG_COUNT;
	m_cPreMsgCounter = 0;

	if (!bDirectSend)
	{
		if (TextMsgFilter(Buffer, nMsgLength))	
		{
			m_InputEdit.ClearText();
			return;
		}
	}

	if (g_pCoreShell == NULL)
		return;

	int bChannel = false;
	int nDestChannel = -1;
	char Name[32];
	Name[0] = 0;
	if (Buffer[nName] == TEXT_CTRL_CHAT_PREFIX || Buffer[nName] == TEXT_CTRL_CHANNEL_PREFIX)
	{
		bChannel = (Buffer[nName] == TEXT_CTRL_CHANNEL_PREFIX);
		while (nName < nMsgLength)
		{
			if (Buffer[nName] == ' ')
			{
				Buffer[nName] = 0;
				nName++;
				break;
			}
			nName++;
		}

		if (bChannel)
		{
			ReplaceSpecialName(Name, 31, Buffer + 1);
			nDestChannel = GetChannelIndex(Name);
		}
		else
		{
			strncpy(Name, Buffer + 1, 31);
		}
		Name[31] = 0;
	}

	if (Name[0] == 0)	
	{
		bChannel = true;
		int nChannelDataCount = KUiMsgCentrePad::GetChannelCount();
		if (m_nCurChannel >= 0 && m_nCurChannel < nChannelDataCount)
		{
			nDestChannel = m_nCurChannel;
		}
		else
		{
			for (int n = 0; n < nChannelDataCount; n++)
			{
				if (KUiMsgCentrePad::IsChannelType(n, KUiMsgCentrePad::ch_Screen))
				{
					nDestChannel = n;
					break;
				}
			}
		}

		if (nDestChannel >= 0)
			strncpy(Name, KUiMsgCentrePad::GetChannelTitle(nDestChannel), 31);
		
		Name[31] = 0;
	}
	else	
	{
		Buffer[nName - 1] = ' ';
	}

	nMsgLength -= nName;
	
	if (bChannel && !KUiMsgCentrePad::IsChannelType(nDestChannel, KUiMsgCentrePad::ch_GM))	
	{
		if (nMsgLength > 3 && Buffer[nName] == '?' && (Buffer[nName + 1] == 'g' || Buffer[nName + 1] == 'G') && (Buffer[nName + 2] == 'm' || Buffer[nName + 2] == 'M'))
		return;
	}

	if (!g_ChatFilter.IsTextPass(Buffer + nName))
	{
		char szWarning[] = "Xin vui lﬂng kh´ng sˆ dÙng tı ng˜ kh´ng th›ch hÓp trong th≠!"; 
		KUiMsgCentrePad::SystemMessageArrival(szWarning, sizeof(szWarning));
		return;
	}

	char szEvent[1024];
	if (bChannel)
		sprintf(szEvent, APP_CHAT, Name, Buffer + nName);
	else
		sprintf(szEvent, APP_SAY, Name, Buffer + nName);

	szEvent[1023] = 0;

	if (g_UiBase.NotifyEvent(szEvent) == 0)
		return;

	char	Buffer2[1536];
	memset(Buffer2, 0, sizeof(Buffer2));

   	nMsgLength = KUiFaceSelector::ConvertFaceText(Buffer2, Buffer + nName, nMsgLength+1);

	//chat item
	if(m_ItemName[0])
	{
		char * pInsert = strstr(Buffer2,m_ItemName);
		char * pInsert2 = strstr(Buffer2,m_ItemName);
		if(pInsert)
		{
			int nL = 0;
			while(1)
			{
				if(*pInsert == '>') break;
				nL++;
				pInsert++;
			}

			nL++;
			pInsert++;
			int nLen = strlen(m_ChatItemInfo);
			int nMove = nLen - nL;
			if(nMove + strlen(Buffer2) > 1535) return; //gioi han pham vi chuoi~
			int nAfterLen = strlen(pInsert);
			if(nMove > 0)
			memmove(pInsert+nMove,pInsert,nAfterLen);
			memcpy(pInsert2,m_ChatItemInfo,nLen);
			nMsgLength += nMove;
		}
	}
	// end

	nMsgLength = TEncodeText(Buffer2, nMsgLength);

	if (bChannel)
	{
		DWORD nChannelID = KUiMsgCentrePad::GetChannelID(nDestChannel);
		if (nChannelID != -1)
		{
			KUiMsgCentrePad::CheckChannel(nDestChannel, true);
			OnSendChannelMessage(nChannelID, Buffer2, nMsgLength);
			m_InputEdit.ClearText();
			InputCurrentChannel(nDestChannel, true);
		}
	}
	else if (!IsSelfName(Name))
	{
		OnSendSomeoneMessage(Name, Buffer2, nMsgLength);
		
		int nAdd = AddRecentPlayer(Name);
		if (nAdd >= 0)
		{
			m_InputEdit.ClearText();
			InputCurrentChannel(nAdd, true);
		}
	}
}

BOOL KUiPlayerBar::IsCanSendMessage(const char* Buffer, int nLen, char* szDestName, int nChannelID)
{
	if (m_pSelf)
	{
		if (Buffer && nLen > 0 && !g_ChatFilter.IsTextPass(Buffer))
			return FALSE;

		char szEvent[1024];
		if (nChannelID >= 0)
			sprintf(szEvent, APP_CHAT, szDestName, Buffer);
		else
			sprintf(szEvent, APP_SAY, szDestName, Buffer);

		szEvent[1023] = 0;

		if (g_UiBase.NotifyEvent(szEvent) == 0)
			return FALSE;
	}
	return TRUE;
}

void KUiPlayerBar::OnSendChannelMessage(DWORD nChannelID, const char* Buffer, int nLen)				
{
	if (nChannelID != -1 && Buffer && nLen > 0)
	{
		int nLeft = KUiMsgCentrePad::PushChannelData(nChannelID, Buffer, nLen);
		if (nLeft < 0)
		{
			char szWarning[] = "Thi™n l˝ truy“n ©m kh´ng c∏ch nµo sˆ dÙng li™n tÙc, mÍi bπn l y h¨i chËc l∏t!";
			KUiMsgCentrePad::SystemMessageArrival(szWarning, sizeof(szWarning));
		}
		else if (nLeft > 0)
		{
			char szWarning[64];
			sprintf(szWarning, "Th´ng b∏o sœ Æ≠Óc gˆi sau %d gi©y!", (nLeft + 999 ) / 1000);
			KUiMsgCentrePad::SystemMessageArrival(szWarning, strlen(szWarning) + 1);
		}
	}
}

void KUiPlayerBar::OnDirectSendChannelMessage(DWORD nChannelID, BYTE cost, const char* Buffer, int nLen)				
{
	if (g_pCoreShell && nChannelID != -1 && Buffer && nLen > 0)
	{
		size_t chatsize = sizeof(CHAT_CHANNELCHAT_CMD) + nLen;
		size_t pckgsize = sizeof(tagExtendProtoHeader) + chatsize;

		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
		pExHeader->ProtocolType = c2s_extendchat;
		pExHeader->wLength = pckgsize - 1;

		CHAT_CHANNELCHAT_CMD* pCccCmd = (CHAT_CHANNELCHAT_CMD*)(pExHeader + 1);
		pCccCmd->ProtocolType = chat_channelchat;
		pCccCmd->wSize = chatsize - 1;
		pCccCmd->packageID = -1;
		pCccCmd->filter = 1;
		pCccCmd->channelid = nChannelID;
		pCccCmd->cost = cost;
		pCccCmd->sentlen = nLen;
		memcpy(pCccCmd + 1, Buffer, nLen);

		g_pCoreShell->SendNewDataToServer(pExHeader, pckgsize);
	}
}

void KUiPlayerBar::OnSendSomeoneMessage(const char* Name, const char* Buffer, int nLen)				
{
	if (g_pCoreShell && Name && Name[0] != 0 && Buffer && nLen > 0 && m_pSelf)
	{
		size_t chatsize = sizeof(CHAT_SOMEONECHAT_CMD) + nLen;
		size_t pckgsize = sizeof(tagExtendProtoHeader) + chatsize;

		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
		pExHeader->ProtocolType = c2s_extendchat;
		pExHeader->wLength = pckgsize - 1;

		CHAT_SOMEONECHAT_CMD* pCscCmd = (CHAT_SOMEONECHAT_CMD*)(pExHeader + 1);
		pCscCmd->ProtocolType = chat_someonechat;
		pCscCmd->wSize = chatsize - 1;
		pCscCmd->packageID = -1;
		strcpy(pCscCmd->someone, Name);
		pCscCmd->sentlen = nLen;
		memcpy(pCscCmd + 1, Buffer, nLen);

		g_pCoreShell->SendNewDataToServer(pExHeader, pckgsize);
	}
}

void KUiPlayerBar::InputRecentMsg(bool bPrior)
{
	int nCounter;
	if (bPrior)
		nCounter = m_cPreMsgCounter - 1;
	else
		nCounter = m_cPreMsgCounter + 1;
	if (nCounter < 0 && nCounter >= - MAX_RECENT_MSG_COUNT)
	{
		int nIndex = m_cLatestMsgIndex + nCounter;
		if (nIndex < 0)
			nIndex += 8;
		if (m_RecentMsg[nIndex][0])
		{
			m_InputEdit.SetText(m_RecentMsg[nIndex]);
			m_cPreMsgCounter = nCounter;
		}
	}
	else if (nCounter >= 0)
	{
		m_InputEdit.ClearText();
		m_cPreMsgCounter = 0;
	}
}

void KUiPlayerBar::OnObjPickedDropped(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	KUiObjAtContRegion	Pick, Drop;
	KUiDraggedObject	Obj;
	KWndWindow*			pWnd = NULL;

	if (pPickPos)
	{
		_ASSERT(pPickPos->pWnd);
		((KWndObjectBox*)(pPickPos->pWnd))->GetObject(Obj);
		Pick.Obj.uGenre = Obj.uGenre;
		Pick.Obj.uId = Obj.uId;
		Pick.Region.Width = Obj.DataW;
		Pick.Region.Height = Obj.DataH;
		Pick.Region.v = 0;
		Pick.eContainer = UOC_IMMEDIA_ITEM;
		pWnd = pPickPos->pWnd;
	}
	else if (pDropPos)
	{
		pWnd = pDropPos->pWnd;
	}
	else
		return;

	if (pDropPos)
	{
		Wnd_GetDragObj(&Obj);
		Drop.Obj.uGenre = Obj.uGenre;
		Drop.Obj.uId = Obj.uId;
		Drop.Region.Width = Obj.DataW;
		Drop.Region.Height = Obj.DataH;
		Drop.Region.v = 0;
		Drop.eContainer = UOC_IMMEDIA_ITEM;
		if(Obj.uGenre == CGOG_SKILL_FIGHT)
			Drop.eContainer = UOC_IMMEDIA_SKILL;
	}
	int i = 0;
	for (i = 0; i < UPB_IMMEDIA_ITEM_COUNT; i++)
	{
		if (pWnd == (KWndWindow*)&m_ImmediaItem[i])
		{
			Drop.Region.h =	Pick.Region.h = i;
			break;
		}
	}
	_ASSERT(i < UPB_IMMEDIA_ITEM_COUNT);
	
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, 
		pPickPos ? (unsigned int)&Pick : 0,
		pDropPos ? (int)&Drop : 0);
}

void KUiPlayerBar::OnUseItem(int nIndex)
{
	if (m_pSelf && nIndex >= 0 && nIndex < UPB_IMMEDIA_ITEM_COUNT)
	{
		KUiDraggedObject Obj;
		m_pSelf->m_ImmediaItem[nIndex].GetObject(Obj);
		KUiObjAtRegion Info;
		{
			Info.Obj.uGenre = Obj.uGenre;
			Info.Obj.uId = Obj.uId;
			Info.Region.h = nIndex;
			Info.Region.v = 0;
			Info.Region.Width = Info.Region.Height = 0;
		}
		g_pCoreShell->OperationRequest(GOI_USE_ITEM, (int)&Info, UOC_IMMEDIA_ITEM);
	}
}

int KUiPlayerBar::GetChannelIndex(const char* pTitle)
{
	if (pTitle)
	{
		int nChannelDataCount = KUiMsgCentrePad::GetChannelCount();
		int n = KUiMsgCentrePad::GetChannelIndex((char*)pTitle);
		if (n >= 0 && n < nChannelDataCount)
			return n;
		
		n = nChannelDataCount;
		for (; n < nChannelDataCount + m_nRecentPlayerName; n++)
		{
			if (m_RecentPlayerName[n - nChannelDataCount][0] == 0)
				break;
			if (strcmp(m_RecentPlayerName[n - nChannelDataCount], pTitle) == 0)
			{
				return n;
			}
		}
	}
	return -1;
}

void KUiPlayerBar::SetCurrentChannel(int nIndex)
{
	if (!m_pSelf)
		return;
	if (nIndex < 0)
		return;

	if (nIndex == m_pSelf->m_nCurChannel)
		return;
	
	int nChannelDataCount = KUiMsgCentrePad::GetChannelCount();
	if (nIndex < nChannelDataCount)
	{
		m_pSelf->m_nCurChannel = nIndex;
		WORD nPicIndex = -1;
		if (KUiMsgCentrePad::GetChannelMenuinfo(m_pSelf->m_nCurChannel, &nPicIndex))
		{
			char buffer[3];
			buffer[0] = KTC_INLINE_PIC;
			*((WORD*)(buffer + 1)) = nPicIndex;
			m_pSelf->m_ChannelSwitchBtn.SetText(buffer, 3);
		}
	}
	else if (nIndex < nChannelDataCount + m_pSelf->m_nRecentPlayerName)
	{
		m_pSelf->m_nCurChannel = nIndex;
		WORD nPicIndex = -1;
		if (KUiMsgCentrePad::GetPeopleMenuinfo(m_pSelf->m_RecentPlayerName[m_pSelf->m_nCurChannel - nChannelDataCount], &nPicIndex))
		{
			char buffer[3];
			buffer[0] = KTC_INLINE_PIC;
			*((WORD*)(buffer + 1)) = nPicIndex;
			m_pSelf->m_ChannelSwitchBtn.SetText(buffer, 3);
		}
		else
			m_pSelf->m_ChannelSwitchBtn.SetText(m_pSelf->m_RecentPlayerName[m_pSelf->m_nCurChannel - nChannelDataCount]);
	}
}

void KUiPlayerBar::InputCurrentChannel(int nIndex, bool bFocus)
{
	if (nIndex < 0)
		return;
	int nChannelDataCount = KUiMsgCentrePad::GetChannelCount();
	if (nIndex < nChannelDataCount)
	{
		InputNameMsg(true, KUiMsgCentrePad::GetChannelTitle(nIndex), bFocus);
		SetCurrentChannel(nIndex);
	}
	else if (nIndex < nChannelDataCount + m_nRecentPlayerName)
	{
		InputNameMsg(false, m_RecentPlayerName[nIndex - nChannelDataCount], bFocus);
		SetCurrentChannel(nIndex);
	}
}

void KUiPlayerBar::SwitchChannel(BOOL bUp)
{
	if(m_pSelf)
	{
		int nNewChannelID;
		int nCount = KUiMsgCentrePad::GetChannelCount();
		if(bUp)
		{
			if(m_pSelf->m_nCurChannel < 1)
		    {
			    nNewChannelID = nCount - 1;
		    }
		    else
		    {
    			nNewChannelID = m_pSelf->m_nCurChannel - 1;
		    }
		}
		else
		{
		    if(m_pSelf->m_nCurChannel > nCount - 2)
		    {
			    nNewChannelID = 0;
		    }
		    else
		    {
    			nNewChannelID = m_pSelf->m_nCurChannel + 1;
		    }
		}
		m_pSelf->InputCurrentChannel(nNewChannelID, FALSE);
	}
}

int KUiPlayerBar::GetCurChannel()
{
	if(m_pSelf)
	    return m_pSelf->m_nCurChannel;
	else
		return -1;
}

void KUiPlayerBar::UpdateXXXNumber(int& nMana, int& nFullMana)
{
	
	KUiPlayerRuntimeInfo	Info;
	memset(&Info, 0, sizeof(KUiPlayerRuntimeInfo));

	g_pCoreShell->GetGameData(GDI_PLAYER_RT_INFO, (int)&Info, 0);

	m_nExperienceFull = Info.nExperienceFull;
	m_nCurLevelExperience = Info.nCurLevelExperience;
	m_nExperience = Info.nExperience;

	KUiStatus* pStatus = KUiStatus::GetIfVisible();
	if (pStatus)
		pStatus->UpdateRuntimeInfo(&Info);

	KUiSceneTimeInfo	Spot;
	g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO, (unsigned int)&Spot, 0);
	KUiMiniMap::UpdateSceneTimeInfo(&Spot);
	KUiAuto::UpdateSceneTimeInfo(&Spot);
	KUiAutoPlay::UpdateSceneTimeInfo(&Spot);

	nMana = max(Info.nMana, 0);
	nFullMana = max(Info.nManaFull, 0);
}

void KUiPlayerBar::UpdateRuntimeAttribute(int& nMoney, int& nLevel)
{
	KUiPlayerAttribute	Info;
	memset(&Info, 0, sizeof(KUiPlayerAttribute));
	g_pCoreShell->GetGameData(GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&Info, 0);
	KUiStatus* pStatus = KUiStatus::GetIfVisible();
	if (pStatus)
	{
		pStatus->UpdateRuntimeAttribute(&Info);
		pStatus->UpdateRuntimeAttribute(&Info);
	}

	nMoney = Info.nMoney;
	nLevel = Info.nLevel;
}

void KUiPlayerBar::UpdateItem(int nIndex, unsigned int uGenre, unsigned int uId)
{
	if (nIndex >= 0 && nIndex < UPB_IMMEDIA_ITEM_COUNT)
	{
		UiSoundPlay(UI_SI_PICKPUT_ITEM);
		m_ImmediaItem[nIndex].HoldObject(uGenre, uId, 0, 0);
	}
}

void KUiPlayerBar::UpdateSkill(int nIndex, unsigned int uGenre, unsigned int uId)
{
	if (nIndex == 0 || nIndex == 1)
	{
		m_ImmediaSkill[nIndex].HoldObject(uGenre, uId, 0, 0);
	}
}

void KUiPlayerBar::Breathe()
{
	if (g_LoginLogic.GetStatus() != LL_S_IN_GAME)
		return;

	m_DateTime.UpdateData();
	int nMana;
	int nFullMana;
	UpdateXXXNumber(nMana, nFullMana);
	int nMoney;
	int nLevel;
	UpdateRuntimeAttribute(nMoney, nLevel);

	DWORD dwID;
	BYTE cost;
	char* Buffer;
	int nLen;
	int nUseMana = 0;
	int nUseMoney = 0;
	int nUseLevel = 0;

	while(KUiMsgCentrePad::GetChannelData(dwID, cost, Buffer, nLen))
	{
		int nRet = IsHasCost(cost, nMoney, nLevel, nMana, nFullMana, nUseMoney, nUseLevel, nUseMana);

		bool bSend = !nRet;
		char szSystem[256];
		szSystem[0] = 0;

		if (!bSend)
		{
			if (nRet == 2 && nUseLevel > 0)
				sprintf(szSystem, "ßºng c p cÒa bπn kh´ng ÆÒ %d c p, kh´ng c∏ch nµo sˆ dÙng thi™n l˝ truy“n ©m!", nUseLevel);

			if (nRet == 1 && nUseMoney > 0)
				sprintf(szSystem, "Ti“n cÒa bπn kh´ng ÆÒ %d l≠Óng, kh´ng c∏ch nµo sˆ dÙng thi™n l˝ truy“n ©m!", nUseMoney);

			if (nRet == 3 && nUseMana > 0)
				sprintf(szSystem, "NÈi l˘c cÒa bπn kh´ng ÆÒ %d Æi”m, kh´ng c∏ch nµo sˆ dÙng thi™n l˝ truy“n ©m!", nUseMana);
		}
		else
		{
			if (nUseMoney > 0)
				sprintf(szSystem, "Bπn sˆ dÙng thi™n l˝ truy“n ©m tÊn hao ng©n l≠Óng %d l≠Óng!", nUseMoney);
			if (nUseMana > 0)
				sprintf(szSystem, "Bπn sˆ dÙng thi™n l˝ truy“n ©m tÊn hao nÈi l˘c %d Æi”m!", nUseMana);
		}

		if (bSend)
			OnDirectSendChannelMessage(dwID, cost, Buffer, nLen);

		KUiMsgCentrePad::PopChannelData(dwID);
		
		if (szSystem[0])
			KUiMsgCentrePad::SystemMessageArrival(szSystem, strlen(szSystem));
	}
	
	if (m_nCurChannel >= 0 && m_nCurChannel < (int)KUiMsgCentrePad::GetChannelCount() && Wnd_GetFocusWnd() == (KWndWindow*)&m_InputEdit)
	{
		cost = KUiMsgCentrePad::GetChannelCost(KUiMsgCentrePad::GetChannelID(m_nCurChannel));
		if (IsHasCost(cost, nMoney, nLevel, nMana, nFullMana,
			nUseMoney, nUseLevel, nUseMana) != 0)
			m_InputEdit.SetFocusBkColor(s_NotCanFocusColor);
		else
			m_InputEdit.SetFocusBkColor(s_CanFocusColor);
	}
	
	KStateTempNode* pNode = NULL;	
	int nCount = g_pCoreShell->GetGameData(GDI_NPC_STATE_SKILL, 0, 0);
	for (int i = 0; i < MAX_BUTTON_STATE - 5; i++)
	{
		if (i < nCount)
		{
			pNode = (KStateTempNode*)malloc(sizeof(KStateTempNode) * nCount);
			g_pCoreShell->GetGameData(GDI_NPC_STATE_SKILL, (unsigned int)pNode, nCount);
			if(nCount > MAX_BUTTON_STATE - 5) 
				nCount = MAX_BUTTON_STATE - 5;//add by phong ki“u
			for (i = 0; i < nCount; i++)
			{ 
				if(!ms_pStateList[pNode[i].nSkillId].szImage[0])//fix by phong ki“u icon m∆c Æﬁnh cho mini skill
				{
					m_StateImg[i].SetImage(ISI_T_SPR, "\\spr\\item\\medecine\\obj-potion-15.spr", true);
				}
				else
				{
					m_StateImg[i].SetImage(ISI_T_SPR, ms_pStateList[pNode[i].nSkillId].szImage, true);
				}

				m_StateImg[i].Show();
				char szToolTip[128] = "";
				int fkH = pNode[i].nLeftTime/18/60/60;
				int fkM = pNode[i].nLeftTime/18/60%60;
				int fkS = pNode[i].nLeftTime/18%60;
				sprintf(szToolTip, "%s \n%s \n %02dh:%02dm:%02ds", ms_pStateList[pNode[i].nSkillId].szName, ms_pStateList[pNode[i].nSkillId].szDesc, fkH, fkM, fkS);
				m_StateImg[i].SetToolTipInfo(szToolTip, strlen(szToolTip));
				//m_StateLife[i].SetTimeText(pNode[i].nLeftTime);
				m_StateLife[i].SetTimeText2(pNode[i].nLeftTime);
				m_StateLife[i].Show();
			}

			free(pNode);
			pNode = NULL;

		}
		m_StateImg[i].Hide();
		m_StateLife[i].Hide();
	}
}

int KUiPlayerBar::IsHasCost(BYTE cost, int nMoney, int nLevel, int nMana, int nFullMana, int& nUseMoney, int& nUseLevel, int& nUseMana)
{
	if (cost == 0)
	{
		nUseMoney = 0;
		nUseLevel = 0;
		nUseMana = 0;
	}
	else if (cost == 1)
	{
		if (nMoney < 10)
		{
			nUseMoney = 10;
			return 1;
		}
		nUseMoney = 10;
		nUseLevel = 0;
		nUseMana = 0;
	}
	else if (cost == 2)
	{
		if (nLevel < 10)
		{
			nUseLevel = 10;
			return 2;
		}

		if (nMana < nFullMana / 2)
		{
			nUseMana = nFullMana / 2;
			return 3;
		}
		nUseMoney = 0;
		nUseLevel = 0;
		nUseMana = nFullMana / 2;
	}
	else if (cost == 3)
	{
		if (nMana < nFullMana / 10)
		{
			nUseMana = nFullMana / 10;
			return 3;
		}
		nUseMoney = 0;
		nUseLevel = 0;
		nUseMana = nFullMana / 10;
	}
	else if (cost == 4)
	{
		if (nLevel < 20)
		{
			nUseLevel = 20;
			return 2;
		}

		if (nMana < nFullMana * 4 / 5)
		{
			nUseMana = nFullMana * 4 / 5;
			return 3;
		}
		nUseMoney = 0;
		nUseLevel = 0;
		nUseMana = nFullMana * 4 / 5;
	}
	else
	{
		nUseMoney = 0;
		nUseLevel = 0;
		nUseMana = 0;
	}
	return 0;
}

void KUiPlayerBar::InputNameMsg(char bChannel, const char* szName, bool bFocus)
{
	if (m_pSelf == NULL)
		return;

	if (bFocus)
		Wnd_SetFocusWnd(&m_pSelf->m_InputEdit);
	
	if (szName == NULL || szName[0] == 0)	
		return;

	if (bChannel)
		return;

	char Buffer[512];
	int nLen = m_pSelf->m_InputEdit.GetText(Buffer, sizeof(Buffer), false);
	if (nLen < 0)
		nLen = 0;
	Buffer[nLen] = 0;

	int nName = 0;
	if (Buffer[nName] == TEXT_CTRL_CHAT_PREFIX)
	{
		while (nName <= nLen)
		{
			if (Buffer[nName] == ' ' ||
				Buffer[nName] == 0)
			{
				Buffer[nName] = 0;
				break;
			}
			nName++;
		}

		if (nName > nLen)	
			nName = 0;
	}
	
	if (bChannel)
	{
		m_pSelf->m_InputEdit.SetText("");
		//m_pSelf->m_InputEdit.SetText("&");
		//m_pSelf->m_InputEdit.InsertString(szName, strlen(szName));
		//m_pSelf->m_InputEdit.InsertString(" ", 1);
		m_pSelf->m_InputEdit.InsertString(Buffer + nName, nLen - nName);
	}
	else
	{
		if (nName > 0 && (KShortcutKeyCentre::FindCommand(Buffer + 1) >= 0 || KShortcutKeyCentre::ms_FunsMap.find(Buffer + 1) != KShortcutKeyCentre::ms_FunsMap.end()))	
		{	
			m_pSelf->m_InputEdit.InsertString(" ", 1);
			m_pSelf->m_InputEdit.InsertString(szName, strlen(szName));
		}
		else
		{
			m_pSelf->m_InputEdit.SetText("/");
			m_pSelf->m_InputEdit.InsertString(szName, strlen(szName));
			m_pSelf->m_InputEdit.InsertString(" ", 1);
			m_pSelf->m_InputEdit.InsertString(Buffer + nName, nLen - nName);
		}
	}
}

int	KUiPlayerBar::FindRecentPlayer(const char* szName)
{
	if (szName == NULL || szName[0] == 0)
		return -1;
	for (int n = 0; n < m_pSelf->m_nRecentPlayerName; n++)
	{
		if (m_pSelf->m_RecentPlayerName[n][0] == 0)
		{
			break;
		}
		if (strcmp(m_pSelf->m_RecentPlayerName[n], szName) == 0)
		{
			return n;
		}
	}

	return -1;
}

char* KUiPlayerBar::GetRecentPlayerName(int nIndex)
{
	int nChannelCount = KUiMsgCentrePad::GetChannelCount();
	if(m_pSelf && nIndex >= nChannelCount && nIndex < m_pSelf->m_nRecentPlayerName + nChannelCount)
	{
		return m_pSelf->m_RecentPlayerName[nIndex - nChannelCount];
	}
	else
	{
		return "";
	}
}

int KUiPlayerBar::AddRecentPlayer(const char* szName)
{
	if (!m_pSelf)
		return -1;
	if (szName == NULL || szName[0] == 0)
		return -1;
	if (strcmp(m_pSelf->m_szSelfName, szName) == 0)
		return -1;
	int n = m_pSelf->GetChannelIndex(szName);
	if (n >= 0)
		return n;
	n = 0;
	int nChannelDataCount = KUiMsgCentrePad::GetChannelCount();
	for (; n < MAX_RECENTPLAYER_COUNT; n++)
	{
		if (m_pSelf->m_RecentPlayerName[n][0] == 0)
		{
			strncpy(m_pSelf->m_RecentPlayerName[n], szName, 32);
			m_pSelf->m_nRecentPlayerName++;
			return n + nChannelDataCount;
		}
	}

	if (n >= MAX_RECENTPLAYER_COUNT)
	{
		memmove(m_pSelf->m_RecentPlayerName[0], m_pSelf->m_RecentPlayerName[1], sizeof(m_pSelf->m_RecentPlayerName) - 32);
		n = MAX_RECENTPLAYER_COUNT - 1;
		strncpy(m_pSelf->m_RecentPlayerName[n], szName, 32);
		return n + nChannelDataCount;
	}

	return -1;
}

void KUiPlayerBar::ReplaceSpecialName(char* szDest, size_t nDestSize, char* szSrc)
{
	KUiMsgCentrePad::ReplaceChannelName(szDest, nDestSize, szSrc);
}

bool KUiPlayerBar::GetExp(double& nFull, double& nCurrLevelExp, double& nCurrentExp)
{
	if (m_pSelf)
	{
		nFull = m_pSelf->m_nExperienceFull;
		nCurrLevelExp = m_pSelf->m_nCurLevelExperience;
		nCurrentExp = m_pSelf->m_nExperience;
		return true;
	}
	return false;
}

BOOL KUiPlayerBar::LoadPrivateSetting(KIniFile* pFile)
{
	if (pFile && g_pCoreShell)
	{
		int nName = 0;
		int nLife = 0;
		int nNameObj = 0;
		int nPK = 0;

		if (pFile->GetInteger("Player", "ShowLife", 0, (int*)(&nLife)))
			g_pCoreShell->OperationRequest(GOI_SHOW_PLAYERS_LIFE, 0, nLife);
		if (pFile->GetInteger("Player", "ShowName", 0, (int*)(&nName)))
			g_pCoreShell->OperationRequest(GOI_SHOW_PLAYERS_NAME, 0, nName);
		if (pFile->GetInteger("Player", "ShowObjName", 0, (int*)(&nNameObj)))
			g_pCoreShell->OperationRequest(GOI_SHOW_OBJ_NAME, 0, nNameObj);
		//if (pFile->GetInteger("Player", "PK", enumPKNormal, (int*)(&nPK)))
		//	g_pCoreShell->OperationRequest(GOI_PK_SETTING, 0, nPK);
	}
	return TRUE;
}

int	KUiPlayerBar::SavePrivateSetting(KIniFile* pFile)
{
	if (pFile && g_pCoreShell)
	{
		int nLife = g_pCoreShell->GetGameData(GDI_SHOW_PLAYERS_LIFE, 0, 0);
		pFile->WriteInteger("Player", "ShowLife", nLife);
		int nName = g_pCoreShell->GetGameData(GDI_SHOW_PLAYERS_NAME, 0, 0);
		pFile->WriteInteger("Player", "ShowName", nName);
		int nNameObj = g_pCoreShell->GetGameData(GDI_SHOW_OBJ_NAME, 0, 0);
		pFile->WriteInteger("Player", "ShowObjName", nNameObj);
		//int nPK = g_pCoreShell->GetGameData(GDI_PK_SETTING, 0, 0);
		//pFile->WriteInteger("Player", "PK", nPK);
	}
	return 1;
}

// chat item
void KUiPlayerBar::SetChatItem(ChatItem CItem, unsigned int uId)
{
	if(!uId) return;

	if (m_pSelf)
	{
		ZeroMemory(m_pSelf->m_ChatItemInfo,sizeof(m_pSelf->m_ChatItemInfo));
		ZeroMemory(m_pSelf->m_ItemName,sizeof(m_pSelf->m_ItemName));

		char Buffer[16];
		int nOffset = 0;
		m_pSelf->m_ChatItemInfo[nOffset] = '[';
		nOffset++;

		itoa(CItem.m_nID, Buffer, 10);									//1 m_nID
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;

		itoa(CItem.m_btGenre, Buffer, 10);								//2 m_btGenre
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;

		itoa(CItem.m_btDetail, Buffer, 10);								//3 m_btDetail
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;

		itoa(CItem.m_btParticur, Buffer, 10);						//4 m_btParticur
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;

		itoa(CItem.m_btSeries, Buffer, 10);								//5 m_btSeries
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;

		itoa(CItem.m_btLevel, Buffer, 10);								//6 m_btLevel
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;

		itoa(CItem.m_btLuck, Buffer, 10);								//7 m_btLuck
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;

		itoa(CItem.m_wVersion, Buffer, 10);							//8 m_wVersion
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;

		sprintf(Buffer, "%lu", CItem.m_dwRandomSeed);		//9 m_dwRandomSeed
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;

		itoa(CItem.m_nIdx, Buffer, 10);									//10 m_nIdx
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;

		itoa(CItem.m_uPrice, Buffer, 10);								//11 m_uPrice
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;

		itoa(CItem.m_bX, Buffer, 10);									//12 m_bX
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;

		itoa(CItem.m_bY, Buffer, 10);									//13 m_bY
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;

		itoa(CItem.m_bPoint, Buffer, 10);								//14 m_bPoint
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;

		itoa(CItem.m_bStack, Buffer, 10);								//15 m_bStack
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;

		itoa(CItem.m_bEnChance, Buffer, 10);						//16 m_bEnChance
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;

		itoa(CItem.m_nGoldId, Buffer, 10);								//17 m_nGoldId
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;

		itoa(CItem.m_YearExp, Buffer, 10);								//18 m_YearExp
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;

		itoa(CItem.m_Lock, Buffer, 10);										//19 m_Lock
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;

		//
		itoa(CItem.m_nDurability, Buffer, 10);								//20 m_nDurability
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;
		//

		itoa(CItem.m_HLock, Buffer, 10);								//21 m_HLock
		strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
		nOffset += strlen(Buffer);
		m_pSelf->m_ChatItemInfo[nOffset] = ',';
		nOffset++;

		for ( int i = 0; i < 6 ; i++)
		{
			sprintf(Buffer, "%d", CItem.m_btMagicLevel[i]);			//22 m_btMagicLevel[0] 23 24 25 26 27 m_btMagicLevel[5]
			strcat(&m_pSelf->m_ChatItemInfo[nOffset],Buffer);
			nOffset += strlen(Buffer);
			if (i == 15) break;
			if (i < 5)
			{
				m_pSelf->m_ChatItemInfo[nOffset] = ',';
				nOffset++;
			}
		}
		m_pSelf->m_ChatItemInfo[nOffset] = ']';
		nOffset++;

		m_pSelf->m_ChatItemInfo[nOffset] = '\0';
		char szName[64];
		char szName2[64];
		g_pCoreShell->GetGameData(GDI_ITEM_NAME, (unsigned int)&szName, uId);
		int nLen = strlen(szName);
		szName2[0] = '<';
		strncpy(szName2+1,szName,nLen);
		strncpy(szName2+1+nLen,">",1);
		nLen += 2;
		szName2[nLen] = '\0';
		memcpy(m_pSelf->m_ItemName,szName2,64);
		m_pSelf->m_InputEdit.InsertString(szName2, nLen);
	}
}

void KUiPlayerBar::SetItemBtnInfo(int nBtnNo, ChatItem * pItem)
{
	if(nBtnNo >=0 && nBtnNo < MAX_ITEMBUTTON && g_pCoreShell && m_pSelf && pItem)
	{
		m_pSelf->m_ItemBtn[nBtnNo].SetItemInfo(pItem);
		int nIdx = g_pCoreShell->GetGameData(GDI_ITEM_CHAT, true, (int)&m_pSelf->m_ItemBtn[nBtnNo].m_Item);
		if(!nIdx) 
			return;
		char szName[64];
		char szName2[64];
		int nColor = g_pCoreShell->GetTypeItem(nIdx);//edit by phong kieu mau itemchat color chatitem
		switch(nColor)
		{
		case green_item:
			m_pSelf->m_ItemBtn[nBtnNo].SetColor(0xff6464ff,0xff000000,0xff00ffff,0xff000000,0xffff6699,0xff000000);
			break;
		case gold_item:
			m_pSelf->m_ItemBtn[nBtnNo].SetColor(0xffffd94e,0xff000000,0xff00ffff,0xff000000,0xffff6699,0xff000000);
			break;	
		case purple_item:
			m_pSelf->m_ItemBtn[nBtnNo].SetColor(0xffcc33ff,0xff000000,0xff00ffff,0xff000000,0xffff6699,0xff000000);
			break;	
		default:
			m_pSelf->m_ItemBtn[nBtnNo].SetColor(0xffffffff,0xff000000,0xff00ffff,0xff000000,0xffff6699,0xff000000);		
			break;	
		}
		g_pCoreShell->GetGameData(GDI_ITEM_NAME, (unsigned int)&szName, nIdx);
		int nLen = strlen(szName);
		szName2[0] = '<';
		strncpy(szName2+1,szName,nLen);
		strncpy(szName2+1+nLen,">",1);
		nLen += 2;
		szName2[nLen] = '\0';
		m_pSelf->m_ItemBtn[nBtnNo].SetSize(nLen*6,14);
		m_pSelf->m_ItemBtn[nBtnNo].SetText(szName2,nLen);
		m_pSelf->m_ItemBtn[nBtnNo].m_bHaveItem = true;
		g_pCoreShell->GetGameData(GDI_ITEM_CHAT, false, nIdx);
	}
}

void KUiPlayerBar::ClearItemBtn()
{
	if(m_pSelf)
		for (int j = 0; j < MAX_ITEMBUTTON; j++)
		{
			if(m_pSelf->m_ItemBtn[j].m_X == INVISIBLE_POS_X && m_pSelf->m_ItemBtn[j].m_Y == INVISIBLE_POS_Y) 
				continue;
			m_pSelf->m_ItemBtn[j].SetPosition(INVISIBLE_POS);
			m_pSelf->m_ItemBtn[j].Hide();
		}
}

void KUiPlayerBar::SetItemBtnPos(int nBtnNo,int X, int Y)
{
	if(nBtnNo >=0 && nBtnNo < MAX_ITEMBUTTON)
	{
		if(m_pSelf)
		{
			if(m_pSelf->m_ItemBtn[nBtnNo].m_bHaveItem == false) 
				return;
			if(m_pSelf->m_ItemBtn[nBtnNo].m_X == INVISIBLE_POS_X == X && m_pSelf->m_ItemBtn[nBtnNo].m_Y == INVISIBLE_POS_Y == Y) 
				return;
			m_pSelf->m_ItemBtn[nBtnNo].SetPosition(X,Y);
			m_pSelf->m_ItemBtn[nBtnNo].m_X = X;
			m_pSelf->m_ItemBtn[nBtnNo].m_Y = Y;
			if(m_pSelf->m_ItemBtn[nBtnNo].m_X == INVISIBLE_POS_X && m_pSelf->m_ItemBtn[nBtnNo].m_Y == INVISIBLE_POS_Y)
				m_pSelf->m_ItemBtn[nBtnNo].Hide();
			else 
				m_pSelf->m_ItemBtn[nBtnNo].Show();
		}
	}
}

void KUiPlayerBar::LoadStatePos()
{
	if (ms_pStateList == NULL)
	{
		free(ms_pStateList);
		ms_pStateList = NULL;
	}
	ms_nNumState = 0;

	KIniFile Ini;
	if (!Ini.Load(SCHEME_INI_STATE_POS)) return;

	int  nCount, i;
	char szSection[16];
	int	 *nTrim;
	Ini.GetInteger("BuffList", "BuffCount", 0, &nCount);
	if (nCount <= 0) return;

	ms_pStateList = (KStatePos*)calloc(MAX_SKILL, sizeof(KStatePos));
	if (ms_pStateList)
	{
		ms_nNumState = nCount;
		for (i = 0; i < nCount; i++)
		{
			sprintf(szSection, "Buff_%d_ID", i);
			Ini.GetInteger("BuffList", szSection, 0, &ms_pStateList[i].wSkillID);
			nTrim = &(ms_pStateList[i].wSkillID);
			
			sprintf(szSection, "Buff_%d_Name", i);
			Ini.GetString("BuffList", szSection, "", ms_pStateList[*nTrim].szName, sizeof(ms_pStateList[*nTrim].szName));

			sprintf(szSection, "Buff_%d_Image", i);
			Ini.GetString("BuffList", szSection, "", ms_pStateList[*nTrim].szImage, sizeof(ms_pStateList[*nTrim].szImage));
				
			m_pSelf->ms_pStateList[i].szDesc[0] = 0;
			sprintf(szSection, "Buff_%d_Desc", i);
			Ini.GetString("BuffList", szSection, "", ms_pStateList[*nTrim].szDesc, sizeof(ms_pStateList[*nTrim].szDesc));
		}
	}
}

int KUiPlayerBar::GetStateAtPos(int x, int y)
{
	int nIndex = -1;
	if (PtInWindow(x, y))
	{
		nIndex = 0;
		int nCount = g_pCoreShell->GetGameData(GDI_NPC_STATE_SKILL, 0, 0);		
	   	if (nIndex >= nCount)
			nIndex = -1;
	}
	return nIndex;
}

void KUiPlayerBar::UpdateStateTip(int x, int y)
{
	KStateTempNode* pNode = NULL;	
	if (m_nCurrIndex >= 0)
	{
		char szToolTip[64] = "";
		int nCount = g_pCoreShell->GetGameData(GDI_NPC_STATE_SKILL, 0, 0);	
		if (nCount)
		{
			pNode = (KStateTempNode*)malloc(sizeof(KStateTempNode) * nCount);
			g_pCoreShell->GetGameData(GDI_NPC_STATE_SKILL, (unsigned int)pNode, nCount);
			sprintf(szToolTip, "%s \n%s", ms_pStateList[pNode[m_nCurrIndex].nSkillId].szName, ms_pStateList[pNode[m_nCurrIndex].nSkillId].szDesc);
			free(pNode);
			pNode = NULL;
		}				
		g_MouseOver.SetMouseHoverInfo((void*)(KWndWindow*)this, m_nCurrIndex,
			x, y, false, true);
		g_MouseOver.SetMouseHoverTitle(szToolTip, strlen(szToolTip), 0xffffffff);
	}
	else
	{
		g_MouseOver.CancelMouseHoverInfo();
	}
}

void KUiPlayerBar::Clear()
{
	if (m_pSelf)
		m_pSelf->CloseWindow(true);
	if (ms_pStateList)
	{
		free (ms_pStateList);
		ms_pStateList = NULL;
	}
	ms_nNumState = 0;
}