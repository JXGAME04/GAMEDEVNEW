#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "../Elem/MouseHover.h"
#include "../elem/PopupMenu.h"
#include "UiAutoParty.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../Engine/src/Text.h"
#include "../../../Engine/src/KDebug.h"
#include "../../../core/src/coreshell.h"
#include <crtdbg.h>
#include "UiSysMsgCentre.h"

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*		g_pCoreShell;

KUiPlayerItem*	m_pNearbyPlayersList2;
int nX = 0, nY = 0;

/*
Author: Fong KiÒu
Function: Ui Auto Qu¶n lý tæ ®éi
File: UiAutoParty.cpp
Date: 2021
*/

#define	SCHEME_INI_AUTO_PARTY				"\\Ui\\Ui3\\UiAutoParty.ini"
#define	MENU_SELECT_FOLLOW_PEOPLE	0x002

KUiAutoParty* KUiAutoParty::m_pSelf = NULL;

KUiAutoParty::KUiAutoParty()
{

}

KUiAutoParty::~KUiAutoParty()
{
}

KUiAutoParty* KUiAutoParty::OpenWindow(bool bshow)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiAutoParty;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_POPUP_OUTGAME_WND);
		if(bshow)
			m_pSelf->Show();
	}
	return m_pSelf;
}

void KUiAutoParty::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf->SaveAutoPartySetting();
			m_pSelf = NULL;
		}
		else
		{
			m_pSelf->Hide();
		}
	}
}

void KUiAutoParty::Initialize()
{
	AddChild(&m_Close);
	AddChild(&m_InsertMem);
	AddChild(&m_DeleteMem);
	AddChild(&m_AutoPartyAllBtn);
	AddChild(&m_AutoPartyLeadBtn);
	AddChild(&m_AutoPartyRefuInviteBtn);//tõ chèi mäi lêi mêi
	AddChild(&m_AutoPartyPlayerListBtn);
	AddChild(&m_AutoPartyLeaveTeam1Btn);
	AddChild(&m_AutoPartyLeaveTeam2Btn);
	AddChild(&m_CoordL);	
	AddChild(&m_CoordL_Scroll);
	m_CoordL.SetScrollbar(&m_CoordL_Scroll);
	memset(m_MoveMpsList, 0, sizeof(m_MoveMpsList));
	AddChild(&m_NameFollowPeople);

	LoadScheme();
	Wnd_AddWindow(this, WL_TOPMOST);

	memset(m_cFollowName, 0, sizeof(m_cFollowName));
}

void KUiAutoParty::SaveAutoPartySetting()
{
	KIniFile* pConfigFile = NULL;
	pConfigFile = g_UiBase.GetAutoSettingFile();
	if (pConfigFile)
	{
		if (m_AutoPartyAllBtn.IsButtonChecked()) //PT tÊt c¶
			pConfigFile->WriteInteger("Fighting", "AutoPTAll", 1);
		else
			pConfigFile->WriteInteger("Fighting", "AutoPTAll", 0);

		if (m_AutoPartyLeadBtn.IsButtonChecked()) //Lu«n lµm nhãm tr­ëng
			pConfigFile->WriteInteger("Fighting", "AutoPTLead", 1);
		else
			pConfigFile->WriteInteger("Fighting", "AutoPTLead", 0);

		if (m_AutoPartyRefuInviteBtn.IsButtonChecked()) //Tõ chèi mäi lêi mêi
			pConfigFile->WriteInteger("Fighting", "AutoPTRefuInvite", 1);
		else
			pConfigFile->WriteInteger("Fighting", "AutoPTRefuInvite", 0);

		char szKeyName[16];
		for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i++)
		{
			sprintf(szKeyName, "TeamPlayerN%d", i);
			pConfigFile->WriteString("Fighting", szKeyName, m_MoveMpsList[i]);	
		}	
	}
	g_UiBase.CloseAutoSettingFile(true);
}

void KUiAutoParty::LoadAutoPartySetting()
{
	BOOL m_bAutoPartyAllBtn, m_bAutoPartyLeadBtn, m_bAutoPartyRefuInviteBtn;
	KIniFile* pConfigFile = NULL;
	pConfigFile = g_UiBase.GetAutoSettingFile();
	if (pConfigFile)
	{
		pConfigFile->GetInteger("Fighting", "AutoPTAll", 0, (int*)(&m_bAutoPartyAllBtn));//PT tÊt c¶
		if (m_bAutoPartyAllBtn)
			m_AutoPartyAllBtn.CheckButton(true);
		else
			m_AutoPartyAllBtn.CheckButton(false);
		g_pCoreShell->OperationRequest(GOI_AUTO_PLAY,eAutoPTAll,m_bAutoPartyAllBtn);

		pConfigFile->GetInteger("Fighting", "AutoPTLead", 0, (int*)(&m_bAutoPartyLeadBtn));
		if (m_bAutoPartyLeadBtn)
		{
			m_AutoPartyLeadBtn.CheckButton(true);												//Lu«n lµm nhãm tr­ëng
			g_pCoreShell->OperationRequest(GOI_AUTO_PLAY,39,FALSE);	
			g_pCoreShell->OperationRequest(GOI_AUTO_PLAY,38,TRUE);
		}
		else
		{
			m_AutoPartyLeadBtn.CheckButton(false);												//tù nhËn lêi mêi
			g_pCoreShell->OperationRequest(GOI_AUTO_PLAY,39,TRUE);
			g_pCoreShell->OperationRequest(GOI_AUTO_PLAY,38,FALSE);	
		}

		pConfigFile->GetInteger("Fighting", "AutoPTRefuInvite", 0, (int*)(&m_bAutoPartyRefuInviteBtn));//tõ chèi mäi lêi mêi
		if (m_bAutoPartyRefuInviteBtn)
		{
			m_AutoPartyRefuInviteBtn.CheckButton(true);
			g_pCoreShell->TeamOperation(TEAM_OI_REFUSE_INVITE, 0, 1);//1 bËt tù ®éng tõ chèi lêi mêi
		}
		else
		{
			m_AutoPartyRefuInviteBtn.CheckButton(false);
			g_pCoreShell->TeamOperation(TEAM_OI_REFUSE_INVITE, 0, 0);//0 t¾t tù ®éng tõ chèi lêi mêi
		}

		char szKeyName[16];
		for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i++)
		{
			sprintf(szKeyName, "TeamPlayerN%d", i);
			pConfigFile->GetString("Fighting", szKeyName, "", m_MoveMpsList[i], 32);//fix by phong kiÒu read ghi vµo m_MoveMpsList vÞ trÝ i
		}
		SetPlayerListTeam();	
	}
	g_UiBase.CloseAutoSettingFile(true);
}

KUiAutoParty*	KUiAutoParty::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	else
		return NULL;
}

void KUiAutoParty::LoadScheme()
{
	KIniFile	Ini;
	if (m_pSelf && Ini.Load(SCHEME_INI_AUTO_PARTY))
	{
		m_pSelf->Init(&Ini, "MainParty");
		m_pSelf->m_Close.Init(&Ini, "CloseBtn");
		m_pSelf->m_InsertMem.Init(&Ini, "InsertMemBtn");
		m_pSelf->m_DeleteMem.Init(&Ini, "DeleteMemBtn");

		m_pSelf->m_AutoPartyAllBtn.Init(&Ini, "AutoPartyAll");
		m_pSelf->m_AutoPartyLeadBtn.Init(&Ini, "AutoPartyLead");
		m_pSelf->m_AutoPartyRefuInviteBtn.Init(&Ini, "AutoPTTCLM");//tõ chèi mäi lêi mêi
		m_pSelf->m_AutoPartyPlayerListBtn.Init(&Ini, "AutoPartyPlayerList");
		m_pSelf->m_CoordL.Init(&Ini, "CoordL");			
		m_pSelf->m_CoordL_Scroll.Init(&Ini, "CoordL_Scroll");
		m_pSelf->m_AutoPartyLeaveTeam1Btn.Init(&Ini, "AutoPartyLeaveTeam1");
		m_pSelf->m_AutoPartyLeaveTeam2Btn.Init(&Ini, "AutoPartyLeaveTeam2");

		Ini.GetInteger2("PopupPosition","PopUpListPlayer",&nX, &nY);

		m_pSelf->LoadAutoPartySetting();
		
		m_pSelf->m_NameFollowPeople.Init(&Ini, "NameFollowPeople");	
		m_pSelf->m_NameFollowPeople.SetText(MSG_NON_SETTINGS);
	}
}

void KUiAutoParty::PopUpInsertPlayerTeam()
{
	if (m_pNearbyPlayersList2)
	{
		free (m_pNearbyPlayersList2);
		m_pNearbyPlayersList2 = NULL;
	}
	int nActionDataCount = g_pCoreShell->GetGameData(GDI_NEARBY_PLAYER_LIST, 0, 0);
	if (nActionDataCount > 0) 
	{
		struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
		if (pSelUnitMenu == NULL)
			return;
		KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
		pSelUnitMenu->nNumItem = 0;
		pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;

		m_pNearbyPlayersList2 = (KUiPlayerItem*)malloc(sizeof(KUiPlayerItem) * nActionDataCount);
		if (m_pNearbyPlayersList2)
		{
			g_pCoreShell->GetGameData(GDI_NEARBY_PLAYER_LIST, (unsigned int)m_pNearbyPlayersList2, nActionDataCount);
			for (int i = 0; i< nActionDataCount;i++)
			{
				strncpy(pSelUnitMenu->Items[i].szData, m_pNearbyPlayersList2[i].Name, 63);
				pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
				pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
				pSelUnitMenu->nNumItem++;
			}
		}
		int Left, Top, nWidth;
		GetSize(&nWidth, NULL);
		GetAbsolutePos(&Left, &Top);
		pSelUnitMenu->nX = Left + nX;
		pSelUnitMenu->nY = Top + nY;
		KPopupMenu::Popup(pSelUnitMenu, this, MENU_SELECT_FOLLOW_PEOPLE);
	} 
	else 
	{
		KSystemMessage	Msg;
		Msg.byConfirmType = SMCT_NONE;
		Msg.byParamSize = 0;
		Msg.byPriority = 0;
		Msg.eType = SMT_NORMAL;
		Msg.uReservedForUi = 0;
		strcpy(Msg.szMessage, "Xung quanh kh«ng cã ai c¶.");
		KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
	}
}

int KUiAutoParty::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int	nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		OnClickButton((KWndButton*)(KWndWindow*)uParam);
		break;
	case WM_KEYDOWN:
		break;
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)&m_CoordL_Scroll)
			m_CoordL.SetTopItemIndex(nParam);
		break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if (HIWORD(nParam) == MENU_SELECT_FOLLOW_PEOPLE && (short)(LOWORD(nParam)) >= 0)
				ProcessPopUpSelectInsertPlayerJoinTeam(LOWORD(nParam));
		}
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

void KUiAutoParty::ProcessPopUpSelectInsertPlayerJoinTeam(int nAction)
{
	strcpy(m_cFollowName, m_pNearbyPlayersList2[nAction].Name);
	m_NameFollowPeople.SetText(m_cFollowName);
}

void KUiAutoParty::OnClickButton(KWndButton* pBtn)
{
	if (pBtn == &m_Close)
	{
		CloseWindow(true);
	}
	else if(pBtn == &m_AutoPartyAllBtn)
	{
		g_pCoreShell->OperationRequest(GOI_AUTO_PLAY,eAutoPTAll, m_AutoPartyAllBtn.IsButtonChecked()); //pt tÊt c¶
	}
	else if(pBtn == &m_AutoPartyLeadBtn)//lu«n lµm nhãm tr­ëng
	{
		g_pCoreShell->OperationRequest(GOI_AUTO_PLAY,39, !m_AutoPartyLeadBtn.IsButtonChecked());	
		g_pCoreShell->OperationRequest(GOI_AUTO_PLAY,38,m_AutoPartyLeadBtn.IsButtonChecked());
	}
	else if(pBtn == &m_AutoPartyRefuInviteBtn)//tõ chèi mäi lêi mêi
	{
		g_pCoreShell->TeamOperation(TEAM_OI_REFUSE_INVITE, 0, m_AutoPartyRefuInviteBtn.IsButtonChecked());//0 t¾t tù ®éng tõ chèi lêi mêi 1 tõ chèi lêi mêi
	}
	else if(pBtn == &m_InsertMem)
	{
		if(m_cFollowName[0])
			InsertPlayerListTeam(m_cFollowName);
	}
	else if(pBtn == &m_DeleteMem)
	{
		int nSel = m_CoordL.GetCurSel();
		int nCount = 0;
		int i;
		memset(m_MoveMpsList[nSel], 0, sizeof(m_MoveMpsList[nSel]));

		for (i = 0; i < defMAX_AUTO_MOVEMPSL; i++)
		{
			if (m_MoveMpsList[i][0])
			{
				nCount++;
			}
		}
			
		for (i = nSel; i < nCount; i++)			
		{
			strcpy(m_MoveMpsList[i], m_MoveMpsList[i+1]);
			memset(m_MoveMpsList[i+1], 0, sizeof(m_MoveMpsList[i+1]));
		}			
		nCount--;			
							
		SetPlayerListTeam();
	}
	else if (pBtn == &m_AutoPartyPlayerListBtn)
	{
		PopUpInsertPlayerTeam();
	}
}

BOOL KUiAutoParty::InsertPlayerListTeam(char *playerName)
{
	int i = 0;
	for (i = 0; i < defMAX_AUTO_MOVEMPSL; i ++)
	{
		if (strcmp(m_MoveMpsList[i], playerName) == 0)
		{
			i = defMAX_AUTO_MOVEMPSL;
			break;
		}
		else if (!m_MoveMpsList[i][0])
		{
			break;
		}
	}
	if (i < defMAX_AUTO_MOVEMPSL)
	{
		strcpy(m_MoveMpsList[i],playerName);
		SetPlayerListTeam();
	}
	return TRUE;
}

void KUiAutoParty::SetPlayerListTeam()
{
	char szList[32];
	char szListView[32];
	m_CoordL.ResetContent();
	for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i ++)
	{
		g_pCoreShell->SetMoveMap(GAUTO_AUTO_PT_PLAYERTEAM, i,(unsigned int)&m_MoveMpsList[i]);//qu¶n lý tæ ®éi

		if(m_MoveMpsList[i][0])
		{
			sprintf(szList, "%s", m_MoveMpsList[i]);
			sprintf(szListView, "%s", m_MoveMpsList[i]);
			m_CoordL.AddString(i, szListView);
		}
	}
}