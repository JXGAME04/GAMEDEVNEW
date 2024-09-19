/********************************************************************
File        : UiTongManager.cpp
Creator     : Fong KiÒu
create data : 08-29-2020(mm-dd-yyyy)
Description : Qu¶n lý bang héi
*********************************************************************/

#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"
#include "KPlayerDef.h"

#include "../elem/wnds.h"
#include "../elem/wndmessage.h"

#include "../UiBase.h"
#include "../UiSoundSetting.h"

#include "../../../Engine/src/KFilePath.h"

#include "UiPopupPasswordQuery.h"
#include "UiTongAssignBox.h"
#include "UiTongManager.h"
#include "UiInformation.h"
#include "UiTongGetString.h"
#include "UiTongChangeTitle.h"		
#include "UiGetMoney.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
//#include "../../../Engine/src/KDebug.h"

KUiTongManager* KUiTongManager::ms_pSelf = NULL;
KTongInfo       KUiTongManager::m_TongData;
int             KUiTongManager::m_nElderDataIndex = 0;
int             KUiTongManager::m_nMemberDataIndex = 0;
int             KUiTongManager::m_nCaptainDataIndex = 0;

#define TONG_MANAGER_INI "UiTongManager.ini"
#define TONG_DATA_TEMP_FILE "TongDataTempFile.ini"

enum INPUT_STRING_PARAM
{
	ISP_NEW_TITLE		= 0x100,
	ISP_NEW_MALETITLE	= 0x200,
	ISP_NEW_FEMALETITLE	= 0x300,
	ISP_CHANGE_CAMP_JUSTIE	= 0x400,
	ISP_CHANGE_CAMP_EVIL	= 0x500,
	ISP_CHANGE_CAMP_BALANCE	= 0x600,
};

#define TONG_REQUEST_INTERVAL 100000
extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell* g_pCoreShell;
extern KUiInformation g_UiInformation;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KUiTongManager::KUiTongManager()
{
	m_szPassword[0] = 0;
	m_nSelectIndex = -1;
	m_nCurrentCheckBox = -1;
	m_TongData.szName[0] = 0;
	m_szTargetPlayerName[0] = 0;
	m_uLastRequestTongDataTime = 0;
	m_uLastRequestElderListTime = 0;
	m_uLastRequestMemberListTime = 0;
	m_uLastRequestCaptainListTime = 0;
}

KUiTongManager::~KUiTongManager()
{

}

KUiTongManager* KUiTongManager::OpenWindow()
{
	if(g_pCoreShell)
	{
    	if (ms_pSelf == NULL)
    	{
		    ms_pSelf = new KUiTongManager;
		    if (ms_pSelf)
    			ms_pSelf->Initialize();
    	}
    	if (ms_pSelf)
    	{
		    UiSoundPlay(UI_SI_WND_OPENCLOSE);
			ms_pSelf->BringToTop();
			ms_pSelf->Show();		
	    }
	}
	return ms_pSelf;
}

KUiTongManager* KUiTongManager::OpenWindow(char* pszPlayerName)
{
	OpenWindow();
	if(ms_pSelf)
	{
		ms_pSelf->ArrangeComposition(pszPlayerName);
	}
	return ms_pSelf;
}


void KUiTongManager::ArrangeComposition(char* pszPlayerName)
{
	if(g_pCoreShell)
	{
    	KUiPlayerItem Player;
		int nKind;

    	ClearTongData();
    	memset(&Player, 0, sizeof(KUiPlayerItem));
    	if(pszPlayerName && pszPlayerName[0])
    	{
			if(g_pCoreShell->FindSpecialNPC(pszPlayerName, &Player, nKind))
				m_nIndex = Player.nIndex;
	    }
	    else
	    {
    		KUiPlayerBaseInfo Me;
		    g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (unsigned int)&Me, 0);
			if(g_pCoreShell->FindSpecialNPC(Me.Name, &Player, nKind))
				m_nIndex = Player.nIndex;
	    }
		if(m_nIndex >= 0)
		{
			strcpy(m_szPlayerName, Player.Name);
	        ms_pSelf->UpdateBtnTheme(0, TRUE);

	        g_pCoreShell->TongOperation(GTOI_REQUEST_PLAYER_TONG, (unsigned int)&Player, (int)TRUE);
		}
		else
		{
			CloseWindow();
		}
	}
}


KUiTongManager* KUiTongManager::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}


void KUiTongManager::CloseWindow(bool bDestory)
{
	if (ms_pSelf)
	{
		ms_pSelf->Hide();
		if (bDestory)
		{
			ms_pSelf->Destroy();
			ms_pSelf = NULL;
		}
	}
}


void KUiTongManager::Initialize()
{
	AddChild(&m_TongBg);

	AddChild(&m_MemberCount);
	AddChild(&m_MasterName);
	AddChild(&m_Alignment);
	AddChild(&m_TongLevel);
	AddChild(&m_TongLeague);//lien minh
	AddChild(&m_TongName);
	AddChild(&m_Money);

	AddChild(&m_BtnDismiss);
	AddChild(&m_BtnRecruit);
	AddChild(&m_BtnAssign);
	AddChild(&m_BtnDemise);
	AddChild(&m_BtnLeave);

	AddChild(&m_BtnCaptainList);
	AddChild(&m_BtnMemberList);
	AddChild(&m_BtnElderList);
	AddChild(&m_BtnLeagueList); //lien minh
	AddChild(&m_BtnCancel);
	AddChild(&m_BtnApply);

	AddChild(&m_List);
//	AddChild(&m_List2);
	AddChild(&m_ListScroll);
	m_List.SetScrollbar(&m_ListScroll);
//	m_List2.SetScrollbar(&m_ListScroll);

	AddChild(&m_BtnRefresh);
	AddChild(&m_BtnClose);
	
	AddChild(&m_MemberText);
	AddChild(&m_RankNameText);
	
	AddChild(&m_OnlineBtn);
//	AddChild(&m_Next);
	
	AddChild(&m_MoneyGuide);
	AddChild(&m_AligGuide);
	AddChild(&m_GetMoneyBtn);
	AddChild(&m_DistriMoneyBtn);
	AddChild(&m_DonateMoneyBtn);
	AddChild(&m_FaceBtn);
	AddChild(&m_HealBtn);
	AddChild(&m_CenterBtn);
	
	
	AddChild(&m_InfoGuide);
	AddChild(&m_StatusGuide);
	AddChild(&m_StatusText);
	AddChild(&m_WayGuide);
	AddChild(&m_WayEdit);
	AddChild(&m_NextTarget);
	AddChild(&m_NextTargetEdit);
	AddChild(&m_ExpGuide);
	AddChild(&m_ExpText);
	AddChild(&m_CityGuide);
	AddChild(&m_CityText);
	//AddChild(&m_UpdateInfoBtn);
	
	AddChild(&m_ChangeTitle);
	AddChild(&m_ChangeMaleTitle);
	AddChild(&m_ChangeFemaleTitle);	

	IsOnline = 0;
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

void KUiTongManager::PaintWindow()
{
	KWndImage::PaintWindow();
	/*if (g_pRepresentShell) //vÏ line border trong phÇn qu¶n lý bang héi
	{
		KRURect	RectInfo;
		GetAbsolutePos(&RectInfo.oPosition.nX, &RectInfo.oPosition.nY);
		RectInfo.oPosition.nX += 20;
		RectInfo.oPosition.nY += 88;
		RectInfo.Color.Color_dw = 0x636563;
		GetSize(&RectInfo.oEndPos.nX, &RectInfo.oEndPos.nY);
		RectInfo.oEndPos.nX = RectInfo.oPosition.nX + 177;
		RectInfo.oEndPos.nY = RectInfo.oPosition.nY + 150;
		g_pRepresentShell->DrawPrimitives(1, &RectInfo, RU_T_RECT, true);	
		
		KRURect	LineTong1;
		LineTong1.oPosition.nX = RectInfo.oPosition.nX + 182;
		LineTong1.oPosition.nY = RectInfo.oPosition.nY - 3;
		LineTong1.oEndPos.nZ = LineTong1.oPosition.nZ = 0;
		LineTong1.oEndPos.nX = LineTong1.oPosition.nX;
		LineTong1.oEndPos.nY = LineTong1.oPosition.nY + 297;
		LineTong1.Color.Color_dw = 0x000000;
		g_pRepresentShell->DrawPrimitives(1, &LineTong1, RU_T_RECT, true);			

		KRURect	LineTong2;
		LineTong2.oPosition.nX = RectInfo.oPosition.nX + 184;
		LineTong2.oPosition.nY = RectInfo.oPosition.nY - 3;
		LineTong2.oEndPos.nZ = LineTong2.oPosition.nZ = 0;
		LineTong2.oEndPos.nX = LineTong2.oPosition.nX;
		LineTong2.oEndPos.nY = LineTong2.oPosition.nY + 297;
		LineTong2.Color.Color_dw = 0x000000;
		g_pRepresentShell->DrawPrimitives(1, &LineTong2, RU_T_RECT, true);	
		
		KRURect	LineInfo;
		LineInfo.oPosition.nX = RectInfo.oPosition.nX;
		LineInfo.oPosition.nY = RectInfo.oPosition.nY + 22;
		LineInfo.oEndPos.nZ = LineInfo.oPosition.nZ = 0;
		LineInfo.oEndPos.nX = RectInfo.oEndPos.nX;
		LineInfo.oEndPos.nY = LineInfo.oPosition.nY;
		LineInfo.Color.Color_dw = 0x636563;
		g_pRepresentShell->DrawPrimitives(1, &LineInfo, RU_T_RECT, true);	
		
		KRURect	RectCity;
		GetAbsolutePos(&RectCity.oPosition.nX, &RectCity.oPosition.nY);
		RectCity.oPosition.nX = RectInfo.oPosition.nX;
		RectCity.oPosition.nY = RectInfo.oEndPos.nY + 8;
		RectCity.Color.Color_dw = 0x636563;
		GetSize(&RectCity.oEndPos.nX, &RectCity.oEndPos.nY);
		RectCity.oEndPos.nX = RectInfo.oEndPos.nX;
		RectCity.oEndPos.nY = RectCity.oPosition.nY + 58;
		g_pRepresentShell->DrawPrimitives(1, &RectCity, RU_T_RECT, true);	
		
		KRURect	RectMoneyTong;
		GetAbsolutePos(&RectMoneyTong.oPosition.nX, &RectMoneyTong.oPosition.nY);
		RectMoneyTong.oPosition.nX = RectCity.oPosition.nX;
		RectMoneyTong.oPosition.nY = RectCity.oEndPos.nY + 5;
		RectMoneyTong.Color.Color_dw = 0x636563;
		GetSize(&RectMoneyTong.oEndPos.nX, &RectMoneyTong.oEndPos.nY);
		RectMoneyTong.oEndPos.nX = RectMoneyTong.oPosition.nX + 112;
		RectMoneyTong.oEndPos.nY = RectMoneyTong.oPosition.nY + 60;
		g_pRepresentShell->DrawPrimitives(1, &RectMoneyTong, RU_T_RECT, true);
		
		KRURect	RectCamp;
		GetAbsolutePos(&RectCamp.oPosition.nX, &RectCamp.oPosition.nY);
		RectCamp.oPosition.nX = RectCity.oPosition.nX + 77;
		RectCamp.oPosition.nY = RectCity.oEndPos.nY + 15;
		RectCamp.Color.Color_dw = 0x636563;
		GetSize(&RectCamp.oEndPos.nX, &RectCamp.oEndPos.nY);
		RectCamp.oEndPos.nX = RectInfo.oEndPos.nX;
		RectCamp.oEndPos.nY = RectCamp.oPosition.nY + 60;
		g_pRepresentShell->DrawPrimitives(1, &RectCamp, RU_T_RECT, true);
	}*/
}

void KUiTongManager::LoadScheme(const char* pScheme)
{
	if(ms_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, TONG_MANAGER_INI);

		if(Ini.Load(Buff))
		{
			ms_pSelf->Init(&Ini, "Main");
			ms_pSelf->m_TongBg.Init(&Ini, "TongBg");
			ms_pSelf->m_TongName.Init(&Ini, "TextTongName");
	        ms_pSelf->m_MasterName.Init(&Ini, "TextMasterName");
	        ms_pSelf->m_Alignment.Init(&Ini, "Faction");
			ms_pSelf->m_TongLevel.Init(&Ini, "TextLevel"); 
			ms_pSelf->m_TongLeague.Init(&Ini, "TextLeague"); //lien minh
	        ms_pSelf->m_MemberCount.Init(&Ini, "MemberCount");
	        ms_pSelf->m_Money.Init(&Ini, "Money");

	        ms_pSelf->m_BtnDismiss.Init(&Ini, "BtnDismiss");
	        ms_pSelf->m_BtnAssign.Init(&Ini, "BtnAssign");
	        ms_pSelf->m_BtnDemise.Init(&Ini, "BtnDemise");
	        ms_pSelf->m_BtnLeave.Init(&Ini, "BtnLeave");
	        ms_pSelf->m_BtnRecruit.Init(&Ini, "BtnRecruit");

	        ms_pSelf->m_BtnApply.Init(&Ini, "BtnApply");
	        ms_pSelf->m_BtnCancel.Init(&Ini, "BtnCancel");
			ms_pSelf->m_BtnElderList.Init(&Ini, "BtnDirectorList");
	        ms_pSelf->m_BtnCaptainList.Init(&Ini, "BtnManagerList");
	        ms_pSelf->m_BtnMemberList.Init(&Ini, "BtnMemberList");
			ms_pSelf->m_BtnLeagueList.Init(&Ini, "BtnLeagueList");

			ms_pSelf->m_List.Init(&Ini, "List");
//			ms_pSelf->m_List2.Init(&Ini, "List2");
			ms_pSelf->m_ListScroll.Init(&Ini, "ListScrollbar");
			
			ms_pSelf->m_BtnRefresh.Init(&Ini, "BtnRefresh");
			ms_pSelf->m_BtnClose.Init(&Ini, "BtnClose");
			
			ms_pSelf->m_MemberText.Init(&Ini, "MemberText");
			ms_pSelf->m_RankNameText.Init(&Ini, "RankNameText");
			
			ms_pSelf->m_OnlineBtn.Init(&Ini, "OnlineBtn");
			ms_pSelf->m_Next.Init(&Ini, "Next");
			
	        ms_pSelf->m_MoneyGuide.Init(&Ini, "MoneyGuide");
	        ms_pSelf->m_AligGuide.Init(&Ini, "AligGuide");
	        ms_pSelf->m_GetMoneyBtn.Init(&Ini, "GetMoneyBtn");
			ms_pSelf->m_DistriMoneyBtn.Init(&Ini, "DistriMoneyBtn");
	        ms_pSelf->m_DonateMoneyBtn.Init(&Ini, "DonateMoneyBtn");
	        ms_pSelf->m_FaceBtn.Init(&Ini, "FaceBtn");
			ms_pSelf->m_HealBtn.Init(&Ini, "HealBtn");
			ms_pSelf->m_CenterBtn.Init(&Ini, "CenterBtn");				
			
	        ms_pSelf->m_InfoGuide.Init(&Ini, "InfoGuide");
	        ms_pSelf->m_StatusGuide.Init(&Ini, "StatusGuide");
	        ms_pSelf->m_StatusText.Init(&Ini, "StatusText");
			ms_pSelf->m_WayGuide.Init(&Ini, "WayGuide");
	        ms_pSelf->m_WayEdit.Init(&Ini, "WayEdit");
	        ms_pSelf->m_NextTarget.Init(&Ini, "NextTarget");
			ms_pSelf->m_NextTargetEdit.Init(&Ini, "NextTargetEdit");
			ms_pSelf->m_ExpGuide.Init(&Ini, "ExpGuide");		
			ms_pSelf->m_ExpText.Init(&Ini, "ExpText");
			ms_pSelf->m_CityGuide.Init(&Ini, "CityGuide");
			ms_pSelf->m_CityText.Init(&Ini, "CityText");
			
			//ms_pSelf->m_UpdateInfoBtn.Init(&Ini, "UpdateInfoBtn");
			
			ms_pSelf->m_ChangeTitle.Init(&Ini, "Title");
			ms_pSelf->m_ChangeMaleTitle.Init(&Ini, "BtnMaleTitle");
			ms_pSelf->m_ChangeFemaleTitle.Init(&Ini, "BtnFemaleTitle");			
			
			Ini.GetString("Main", "StringDismiss", "", ms_pSelf->m_szDismiss, sizeof(ms_pSelf->m_szDismiss));
			Ini.GetString("Main", "StringAssign", "", ms_pSelf->m_szAssign, sizeof(ms_pSelf->m_szAssign));
			Ini.GetString("Main", "StringDemise", "", ms_pSelf->m_szDemise, sizeof(ms_pSelf->m_szDemise));
			Ini.GetString("Main", "StringLeave", "", ms_pSelf->m_szLeave, sizeof(ms_pSelf->m_szLeave));

			Ini.GetString("Main", "StringJustice", "", ms_pSelf->m_szJustice, sizeof(ms_pSelf->m_szJustice));
			Ini.GetString("Main", "StringBalance", "", ms_pSelf->m_szBalance, sizeof(ms_pSelf->m_szBalance));
			Ini.GetString("Main", "StringEvil", "", ms_pSelf->m_szEvil, sizeof(ms_pSelf->m_szEvil));

			Ini.GetString("Main", "StringConfirm", "", ms_pSelf->m_szConfirm, sizeof(ms_pSelf->m_szConfirm));
			Ini.GetString("Main", "StringCancel", "", ms_pSelf->m_szCancel, sizeof(ms_pSelf->m_szCancel));
		}
	}
}

int KUiTongManager::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_LIST_ITEM_SEL:
		if(uParam == (unsigned int)&m_List)
		{
			if(nParam == -1)
			{
				m_BtnDismiss.Enable(FALSE);
				m_BtnAssign.Enable(FALSE);
				m_BtnDemise.Enable(FALSE);
				m_ChangeTitle.Enable(FALSE);
			}
			else
			{
				m_BtnDismiss.Enable(TRUE);
				m_BtnAssign.Enable(TRUE);
				m_BtnDemise.Enable(TRUE);
				if (m_nCurrentCheckBox != enumTONG_FIGURE_MEMBER)
					m_ChangeTitle.Enable(TRUE);
				else
					m_ChangeTitle.Enable(FALSE);	
			}
			m_nSelectIndex = nParam;
		}
		break;

	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_BtnDismiss)
		{
			PopupConfirmWindow(m_szDismiss, RESULT_T_DISMISS);
		}
		else if(uParam == (unsigned int)&m_BtnAssign)
		{
			KPilgarlicItem *pNode = GetSelectedPlayer();

			KUiTongAssignBox::OpenWindow();
			KUiTongAssignBox::LinkToMainWindow(this);
			KUiTongAssignBox::ArrangeData(m_nRelation, pNode->Name, pNode->szAgname, m_nCurrentCheckBox, m_nIndex);
		}
		else if(uParam == (unsigned int)&m_BtnDemise)
		{
			PopupConfirmWindow(m_szDemise, RESULT_T_DEMISE);
		}
		else if(uParam == (unsigned int)&m_BtnLeave)
		{
			PopupConfirmWindow(m_szLeave, RESULT_T_LEAVE);
		}
		else if(uParam == (unsigned int)&m_BtnRecruit)
		{
			g_pCoreShell->TongOperation(GTOI_TONG_RECRUIT, m_BtnRecruit.IsButtonChecked(), 0);
		}
		else if(uParam == (unsigned int)&m_BtnApply)
		{
			OnAction(TONG_ACTION_APPLY);
		}
		else if(uParam == (unsigned int)&m_BtnCancel || uParam == (unsigned int)&m_BtnClose)
		{
			CloseWindow();
		}
		else if(uParam == (unsigned int)&m_BtnRefresh)
		{
			OnRefresh();
		}
		else if (uParam == (unsigned int)&m_OnlineBtn)
		{
			OnCheckOnline();					
		}				
		else if(uParam == (unsigned int)&m_BtnElderList)
		{
			UpdateListCheckButton(enumTONG_FIGURE_DIRECTOR);
			if(m_ElderData.GetCount() == 0)
			    LoadData(enumTONG_FIGURE_DIRECTOR);
		}
		else if(uParam == (unsigned int)&m_BtnCaptainList)
		{
			UpdateListCheckButton(enumTONG_FIGURE_MANAGER);
			if(m_CaptainData.GetCount() == 0)
			    LoadData(enumTONG_FIGURE_MANAGER);
		}
		else if(uParam == (unsigned int)&m_BtnMemberList)
		{
			UpdateListCheckButton(enumTONG_FIGURE_MEMBER);
			if(m_MemberData.GetCount() == 0)
			    LoadData(enumTONG_FIGURE_MEMBER);
		}
		else if(uParam == (unsigned int)&m_BtnLeagueList)//lien minh
		{
			UpdateListCheckButton(enumTONG_FIGURE_LEAGUE);
			g_pCoreShell->OperationRequest(GOI_PLAYER_ACTION, HT_CN, 0);
		}
		else if (uParam == (unsigned int)&m_DistriMoneyBtn)
		{
			KUiGetMoney::OpenWindow(0,1000000000, this, RESULT_T_SND_MONEY, NULL);
		}
		else if (uParam == (unsigned int)&m_GetMoneyBtn)
		{
			KUiGetMoney::OpenWindow(0,1000000000, this, RESULT_T_GET_MONEY, NULL);
		}
		else if (uParam == (unsigned int)&m_DonateMoneyBtn)
		{
			KUiGetMoney::OpenWindow(0,1000000000, this, RESULT_T_SAVE_MONEY, NULL);
		}
		else if (uParam == (unsigned int)&m_ChangeTitle)
		{
			KUiTongChangeTitle::OpenWindow("", "", (KWndWindow*)this, ISP_NEW_TITLE, 4, 16);			
		}							
		else if (uParam == (unsigned int)&m_ChangeFemaleTitle)
		{
			KUiTongChangeTitle::OpenWindow("", "", (KWndWindow*)this, ISP_NEW_FEMALETITLE, 4, 16);	
		}							
		else if (uParam == (unsigned int)&m_ChangeMaleTitle)
		{
			KUiTongChangeTitle::OpenWindow("", "", (KWndWindow*)this, ISP_NEW_MALETITLE, 4, 16);				
		}							
		else if (uParam == (unsigned int)&m_FaceBtn)
		{
			char TargetString[128];
			sprintf(TargetString, "Muèn ®æi thµnh Ch¸nh Ph¸i cña bang héi cÇn %d l­îng tõ tr­¬ng môc ng©n khè ®Ó ®æi ", 100000);
			UIMessageBox(TargetString, this, "X¸c nhËn ", "Hñy bá ", ISP_CHANGE_CAMP_JUSTIE);		
		}							
		else if (uParam == (unsigned int)&m_HealBtn)
		{
			char TargetString[128];
			sprintf(TargetString, "Muèn ®æi thµnh Tµ Ph¸i cña bang héi cÇn %d l­îng tõ tr­¬ng môc ng©n khè ®Ó ®æi ",100000);
			UIMessageBox(TargetString, this, "X¸c nhËn ", "Hñy bá ", ISP_CHANGE_CAMP_EVIL);
		}							
		else if (uParam == (unsigned int)&m_CenterBtn)
		{
			char TargetString[128];
			sprintf(TargetString, "Muèn ®æi thµnh Trung LËp cña bang héi cÇn %d l­îng tõ tr­¬ng môc ng©n khè ®Ó ®æi ",100000);
			UIMessageBox(TargetString, this, "X¸c nhËn ", "Hñy bá ", ISP_CHANGE_CAMP_BALANCE);
		}	
		break;

	case WND_M_OTHER_WORK_RESULT:
		Show();
		if(nParam && uParam < 4)
		{
			strcpy(m_szTargetPlayerName, (char *)nParam);
		}
		if(uParam == RESULT_T_DISMISS && nParam)
		{
			OnAction(TONG_ACTION_DISMISS);
		}
		else if(uParam == RESULT_T_ASSIGN && nParam)
		{
			OnAction(TONG_ACTION_ASSIGN);
		}
		else if(uParam == RESULT_T_PASSWORD_TO_DEMISE)
		{
			strcpy(m_szPassword, (const char *)nParam);
			PopupConfirmWindow(m_szDemise, RESULT_T_DEMISE);
		}
		else if(uParam == RESULT_T_DEMISE)
		{
			if(nParam)
			{
			    OnAction(TONG_ACTION_DEMISE);
			}
			else
			{
				memset(m_szPassword, 0, sizeof(m_szPassword));
			}
		}
		else if(uParam == RESULT_T_LEAVE && nParam)
		{
			OnAction(TONG_ACTION_LEAVE);
		}
		else if (uParam == RESULT_T_SAVE_MONEY)
		{
			if (nParam > 0)
				OnMoney(TONG_ACTION_SAVE, nParam);
		}
		else if (uParam == RESULT_T_GET_MONEY)
		{
			if (nParam > 0)
				OnMoney(TONG_ACTION_GET, nParam);
		}
		else if (uParam == RESULT_T_SND_MONEY)
		{
			if (nParam > 0)
				OnMoney(TONG_ACTION_SND, nParam);
		}
		else if (uParam == ISP_NEW_TITLE)
		{
			if(nParam)
			{
				strcpy(m_szPassword, (const char *)nParam);
				OnAction(TONG_ACTION_CHANGE_TITLE);
			}
			else
			{
				memset(m_szPassword, 0, sizeof(m_szPassword));
			}
		}			
		else if (uParam == ISP_NEW_MALETITLE)
		{
			if(nParam)
			{
				strcpy(m_szPassword, (const char *)nParam);
				OnAction(TONG_ACTION_CHANGE_MALE_TITLE);
			}
			else
			{
				memset(m_szPassword, 0, sizeof(m_szPassword));
			}
		}			
		else if (uParam == ISP_NEW_FEMALETITLE)
		{
			if(nParam)
			{
				strcpy(m_szPassword, (const char *)nParam);
				OnAction(TONG_ACTION_CHANGE_FEMALE_TITLE);
			}
			else
			{
				memset(m_szPassword, 0, sizeof(m_szPassword));
			}
		}			
		else if (uParam == ISP_CHANGE_CAMP_JUSTIE)
		{
			if (!nParam)
				OnAction(TONG_ACTION_CHANGE_CAMP_JUSTIE);
		}			
		else if (uParam == ISP_CHANGE_CAMP_EVIL)
		{
			if (!nParam)
				OnAction(TONG_ACTION_CHANGE_CAMP_EVIL);
		}			
		else if (uParam == ISP_CHANGE_CAMP_BALANCE)
		{
			if (!nParam)
				OnAction(TONG_ACTION_CHANGE_CAMP_BALANCE);
		}			
		m_szTargetPlayerName[0] = 0;
		break;

	case WND_N_SCORLLBAR_POS_CHANGED:
		if(uParam == (unsigned int)&m_ListScroll)
		{
    		m_List.SetTopItemIndex(nParam);
	//		m_List2.SetTopItemIndex(nParam);
		}
		break;

	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return 1;
}


/*********************************************************************
* ¹¦ÄÜ£º¸üÐÂ°´Å¥ÅäÖÃ·½°¸
**********************************************************************/
void KUiTongManager::UpdateBtnTheme(int nType, BOOL IsDissable)
{
	m_BtnDismiss.Hide();
	m_BtnRecruit.Hide();
	m_BtnAssign.Enable(false);
	m_BtnDemise.Hide();
	m_BtnLeave.Hide();

	m_BtnCaptainList.Hide();
	m_BtnMemberList.Hide();
	m_BtnElderList.Hide();
	m_BtnCancel.Hide();
	m_BtnApply.Hide();
	m_BtnLeagueList.Hide();

	m_BtnRefresh.Hide();
	m_DonateMoneyBtn.Enable(false);
	m_DistriMoneyBtn.Enable(false);
	m_GetMoneyBtn.Enable(false);
	//m_UpdateInfoBtn.Enable(false);
	m_FaceBtn.Enable(false);
	m_HealBtn.Enable(false);
	m_CenterBtn.Enable(false);
	m_NextTargetEdit.Enable(false);
	m_ChangeMaleTitle.Enable(false);
	m_ChangeTitle.Enable(false);
	m_ChangeFemaleTitle.Enable(false);
	if(IsDissable)
	{
		return;
	}

	switch(nType)
	{ 
	case enumTONG_FIGURE_MASTER:
		m_BtnDismiss.Show();
		m_BtnRecruit.Show();
		m_BtnAssign.Enable(true);
		m_BtnDemise.Show();

		m_BtnCaptainList.Show();
	    m_BtnMemberList.Show();
		m_BtnLeagueList.Show();
	    m_BtnElderList.Show();
		m_NextTargetEdit.Enable(true);
		//m_UpdateInfoBtn.Enable(true);
		m_DonateMoneyBtn.Enable(true);
		m_DistriMoneyBtn.Enable(true);
		m_GetMoneyBtn.Enable(true);
		m_ChangeTitle.Enable(true);
		m_ChangeMaleTitle.Enable(true);
		m_ChangeFemaleTitle.Enable(true);
		m_FaceBtn.Enable(true);
		m_HealBtn.Enable(true);
		m_CenterBtn.Enable(true);
		m_List.Enable(true);
//		m_List2.Enable(true);			
		switch(m_TongData.nFaction)
		{
		case camp_justice:
			m_FaceBtn.Enable(false);
			break;

		case camp_balance:
			m_CenterBtn.Enable(false);	
			break;

		case camp_evil:
			m_HealBtn.Enable(false);
			break;
		}		
		break;

	case enumTONG_FIGURE_DIRECTOR:
		m_BtnDismiss.Show();
		m_BtnRecruit.Show();
		m_BtnAssign.Enable(true);
		m_BtnLeave.Show();

		m_BtnCaptainList.Show();
	    m_BtnMemberList.Show();
		m_BtnLeagueList.Show();
	    m_BtnElderList.Show();
		
		m_DonateMoneyBtn.Enable(true);
		m_DistriMoneyBtn.Enable(true);
		m_GetMoneyBtn.Enable(true);
		m_List.Enable(true);
//		m_List2.Enable(true);
		m_FaceBtn.Enable(true);//më tÝnh n¨ng thay ®æi mµu cho tr­ëng l·o
		m_HealBtn.Enable(true);
		m_CenterBtn.Enable(true);
		break;

	case enumTONG_FIGURE_MANAGER:
		m_BtnRecruit.Show();
		m_BtnLeave.Show();

	    m_BtnElderList.Show();
		
		m_DonateMoneyBtn.Enable(true);
		m_DistriMoneyBtn.Enable(false);
		m_GetMoneyBtn.Enable(false);
		m_List.Enable(false);
//		m_List2.Enable(false);
		break;

	case enumTONG_FIGURE_MEMBER:
		m_BtnLeave.Show();

	    m_BtnElderList.Show();
		
		m_DonateMoneyBtn.Enable(true);
		m_DistriMoneyBtn.Enable(false);
		m_GetMoneyBtn.Enable(false);
		m_List.Enable(false);
//		m_List2.Enable(false);
		break;

	default:
		m_BtnElderList.Show();
		m_BtnElderList.CheckButton(true);

		KUiPlayerItem Player;
		memset(&Player, 0, sizeof(KUiPlayerItem));
		strcpy(Player.Name, m_szPlayerName);
		//int res = g_pCoreShell->TongOperation(GTOI_TONG_GET_RECRUIT, (unsigned int)&Player, 0);
		//if(res)
		//{
			m_BtnApply.Show();
		//}
		m_BtnCancel.Show();
		break;
	}

	m_BtnRefresh.Show();

	if(g_pCoreShell && nType >= enumTONG_FIGURE_MANAGER && nType <= enumTONG_FIGURE_MASTER)
	{
		KUiPlayerItem Player;
		memset(&Player, 0, sizeof(KUiPlayerItem));
		strcpy(Player.Name, m_szPlayerName);
		m_BtnRecruit.CheckButton(g_pCoreShell->TongOperation(GTOI_TONG_GET_RECRUIT, (unsigned int)&Player, 0));
	}
}

void KUiTongManager::UpdateTongInfoView()
{
	char Buff[16];
	m_TongName.SetText(m_TongData.szName);
	m_MasterName.SetText(m_TongData.szMasterName);
	switch(m_TongData.nFaction)
	{
	case camp_justice:
		m_Alignment.SetText(m_szJustice);
		break;

	case camp_balance:
		m_Alignment.SetText(m_szBalance);
		break;

	case camp_evil:
		m_Alignment.SetText(m_szEvil);
		break;
	}

	int memCount = m_TongData.nMemberCount + m_TongData.nManagerCount + m_TongData.nDirectorCount + 1;
	m_MemberCount.SetText(itoa(memCount, Buff, 10));
	// == t×nh tr¹ng bang héi ==
	if(!m_TongData.nStatusGuide)
		m_StatusText.SetText("Kh«ng tuyÓn");
	else
	{
		if(m_TongData.nStatusGuide == 1)
			m_StatusText.SetText("§ang tuyÓn ng­êi");
		else
			m_StatusText.SetText("§ang ®ãng");
	}
	// == ®¼ng cÊp bang héi ==
	if(!m_TongData.nTongLevel)
		m_TongLevel.SetText("1");		
	else
		m_TongLevel.SetIntText(m_TongData.nTongLevel);
	// == liªn minh bang héi ==
	if(!m_TongData.szLeagueTName[0])
		m_TongLeague.SetText("V« hÖ");
	else
		m_TongLeague.SetText(m_TongData.szLeagueTName);
	// == tiªu chÝ bang héi ==
	if(!m_TongData.szWayEdit[0])
		m_WayEdit.SetText("V« hÖ");	
	else
		m_WayEdit.SetText(m_TongData.szWayEdit);
	// == môc tiªu hiÖn thêi ==
	if(!m_TongData.szNextTargetEdit[0])
		m_NextTargetEdit.SetText("V« hÖ");	
	else
		m_NextTargetEdit.SetText(m_TongData.szNextTargetEdit);
	// == ®iÓm kinh nghiÖm bang héi ==
	if(!m_TongData.nExpGuide)
		m_ExpText.SetText("0 ®iÓm");	
	else
		m_ExpText.SetIntText(m_TongData.nExpGuide);
	// == thµnh thÞ chiÕm h÷u ==
	if(!m_TongData.nCityGuide)
		m_CityText.SetText("V« hÖ");	
	else
	{
		if(m_TongData.nCityGuide == 1)
			m_CityText.SetText("Ph­îng T­êng");
		else if(m_TongData.nCityGuide == 2)
			m_CityText.SetText("T­¬ng D­¬ng");
		else if(m_TongData.nCityGuide == 3)
			m_CityText.SetText("Thµnh §«");
		else if(m_TongData.nCityGuide == 4)
			m_CityText.SetText("D­¬ng Ch©u");
		else if(m_TongData.nCityGuide == 5)
			m_CityText.SetText("§¹i Lý");
		else if(m_TongData.nCityGuide == 6)
			m_CityText.SetText("BiÖn Kinh");
		else if(m_TongData.nCityGuide == 7)
			m_CityText.SetText("L©m An");
		else
			m_CityText.SetText("Kh«ng x¸c ®Þnh");
	}
	//
	char szPrice[32];
	if (m_TongData.nMoney < 10000)
	{
		sprintf(szPrice, "%d l­îng", m_TongData.nMoney);
		m_Money.SetText(szPrice);
	}
	else
	{
		if (m_TongData.nMoney%10000 == 0)
		{
			sprintf(szPrice, "%d v¹n l­îng", m_TongData.nMoney/10000);
			m_Money.SetText(szPrice);
		}
		else
		{
			sprintf(szPrice, "%d v¹n %d l­îng", m_TongData.nMoney/10000, m_TongData.nMoney%10000);
			m_Money.SetText(szPrice);
		}
	}
}

void KUiTongManager::UpdateTongListView()
{
	KLinkStruct<KPilgarlicItem> *pData = GetCurrentSelectedData();

	if(pData)
	{
    	int nViewCount = m_List.GetCount();
    	int nDataCount = pData->GetCount();
		if(nDataCount > nViewCount)
		{
			KPilgarlicItem *pNode = pData->GetItem(nViewCount);
			if(pNode)
			{
				char Buff[64], Buff1[64];
				for(int i = nViewCount; i < nDataCount;i++)
				{
					sprintf(Buff, "%s", pNode->Name);			// ten
					int nLen = strlen(Buff);
					
					for (int i = nLen; i < 18; i ++)
						strcat(Buff, " ");
						
					if (nLen = 17)
					{
						sprintf(Buff1, "%s", pNode->szAgname);		// danh hieu
						strcat(Buff, Buff1);	
					}

					if (IsOnline == FALSE)
					{
						if (pNode->btOnline)
						m_List.AddString(i, Buff);	
						else
						m_List.AddString(i, Buff, 1);
					}
					else
					{
						if (pNode->btOnline)
						m_List.AddString(i, Buff);	
					}		


					pNode = pData->NextItem();
				}
			}
		}
	}
}

KLinkStruct<KPilgarlicItem>* KUiTongManager::GetCurrentSelectedData()
{
	switch(m_nCurrentCheckBox)
	{
	case enumTONG_FIGURE_DIRECTOR:
		return &m_ElderData;
		break;

	case enumTONG_FIGURE_MANAGER:
		return &m_CaptainData;
		break;

	case enumTONG_FIGURE_MEMBER:
		return &m_MemberData;
		break;

	default:
		return NULL;
		break;
	}
}

KPilgarlicItem* KUiTongManager::GetSelectedPlayer()
{
	KLinkStruct<KPilgarlicItem>* pData = NULL;
	KPilgarlicItem* pNode = NULL;

	pData = GetCurrentSelectedData();
	if(pData)
	{
    	pNode = pData->GetItem(m_nSelectIndex);
	}
	return pNode;
}

void KUiTongManager::UpdateListCheckButton(int nType)
{
   	m_BtnElderList.CheckButton(FALSE);
   	m_BtnMemberList.CheckButton(FALSE);
   	m_BtnCaptainList.CheckButton(FALSE);
	m_BtnLeagueList.CheckButton(FALSE);

   	switch(nType)
   	{
   	case enumTONG_FIGURE_DIRECTOR:
	    m_BtnElderList.CheckButton(TRUE);
	    break;

    case enumTONG_FIGURE_MANAGER:
   		m_BtnCaptainList.CheckButton(TRUE);
	    break;

    case enumTONG_FIGURE_MEMBER:
   		m_BtnMemberList.CheckButton(TRUE);
	    break;

	case enumTONG_FIGURE_LEAGUE:
		m_BtnLeagueList.CheckButton(TRUE);
		break;

	default:
		return;
    }
	if(nType != m_nCurrentCheckBox)
	{
		m_nCurrentCheckBox = nType;
		m_nSelectIndex = -1;
		m_List.ResetContent();
//		m_List2.ResetContent();
		UpdateTongListView();
	}
}

void KUiTongManager::ClearTongData()
{
	m_MemberCount.SetText("");
	m_MasterName.SetText("");
	m_Alignment.SetText("");
	m_TongName.SetText("");
	m_Money.SetText("");
	m_TongLevel.SetText("");
	m_TongLeague.SetText("");//lien minh

	m_List.ResetContent();
//	m_List2.ResetContent();

	m_CaptainData.Clear();
	m_MemberData.Clear();
	m_ElderData.Clear();

	m_nElderDataIndex = 0;
	m_nMemberDataIndex = 0;
	m_nCaptainDataIndex = 0;
}


/*********************************************************************
* ¹¦ÄÜ£ºµ¯³öÈ·ÈÏ´°¿Ú
**********************************************************************/
void KUiTongManager::PopupConfirmWindow(const char* pszInfo, unsigned int uHandleID)
{
	//UIMessageBox(pszInfo, this, m_szConfirm, m_szCancel, uHandleID);
	KPilgarlicItem *pPlayer = GetSelectedPlayer();
	if(pPlayer)
	    KUiTongGetString::OpenWindow(pszInfo, pPlayer->Name, this, uHandleID, 1, 31);
	else
		KUiTongGetString::OpenWindow(pszInfo, m_szPlayerName, this, uHandleID, 1, 31);
	Hide();
}


/*********************************************************************
* ¹¦ÄÜ£º½ÓÊÜÐÂµÄÊý¾ÝµÄÊý¾Ý½Ó¿Ú
**********************************************************************/
void KUiTongManager::NewDataArrive(KUiGameObjectWithName *ParamInfo, KTongMemberItem *pIncome)
{
	KLinkStruct<KPilgarlicItem> *pData;
	int *pNum, nCount;

	if(ParamInfo->nData == enumTONG_FIGURE_DIRECTOR)
	{
		if(ms_pSelf)
		{
		    pData = &ms_pSelf->m_ElderData;
		}
		pNum = &m_nElderDataIndex;
		nCount = m_TongData.nDirectorCount;
	}
	else if(ParamInfo->nData == enumTONG_FIGURE_MANAGER)
	{
		if(ms_pSelf)
		{
		    pData = &ms_pSelf->m_CaptainData;
		}
		pNum = &m_nCaptainDataIndex;
		nCount = m_TongData.nManagerCount;
	}
	else if(ParamInfo->nData == enumTONG_FIGURE_MEMBER)
	{
		if(ms_pSelf)
		{
		    pData = &ms_pSelf->m_MemberData;
		}
		pNum = &m_nMemberDataIndex;
		nCount = m_TongData.nMemberCount;
	}
	else
	{
		return;
	}
	if(*pNum != ParamInfo->nParam)
	{
		return;
	}

	if(ParamInfo->uParam)
	{
		*pNum += ParamInfo->uParam;
		if(!strcmp(m_TongData.szName, ParamInfo->szName) && ms_pSelf)
		{
			KPilgarlicItem Add;

			for(int i = 0;i < ParamInfo->uParam;i++)
			{
				if(!pIncome[i].Name[0])
				{
					continue;
				}
				memcpy(&Add, pIncome + i, sizeof(KTongMemberItem));
				Add.nHashID = StringToHash(Add.Name);
				pData->AddItem(Add);
			}
			ms_pSelf->UpdateTongListView();
		}
		SaveNewData(ParamInfo, pIncome);
		if(*pNum < nCount)
		{
			RequestData(ParamInfo->nData, *pNum);
		}
	}
}


/*********************************************************************
* ¹¦ÄÜ£º²éÑ¯ÌØ¶¨Î»ÖÃµÄÇ°Ãæ£¬Ä³¸öÃû×ÖÊÇ·ñÒÑ¾­´æÔÚ
**********************************************************************/
int KUiTongManager::IsPlayerExist(KPilgarlicItem *MeToCmp, KLinkStruct<KPilgarlicItem> *pData)
{
	unsigned int nID;
	KPilgarlicItem *pNode;

	nID = MeToCmp->nHashID;
	pNode = pData->Begin();
	while(pNode || pNode != MeToCmp)
	{
		if(pNode->nHashID == nID)
		{
			if(!strcmp(MeToCmp->Name, pNode->Name))
			{
				return 1;
			}
		}
		pNode = pData->NextItem();
	}
	return 0;
}

void KUiTongManager::TongInfoArrive(KUiPlayerRelationWithOther* Relation, KTongInfo *pTongInfo)
{
	if(ms_pSelf && !strcmp(Relation->Name, ms_pSelf->m_szPlayerName))
	{
		memcpy(&ms_pSelf->m_TongData, pTongInfo, sizeof(KTongInfo));
		ms_pSelf->m_nRelation = Relation->nRelation;

		ms_pSelf->UpdateTongInfoView();
		ms_pSelf->UpdateBtnTheme(Relation->nRelation);
		if(Relation->nRelation <= enumTONG_FIGURE_MASTER && Relation->nRelation >= enumTONG_FIGURE_MEMBER)
		{
			ms_pSelf->m_Relation = (TONG_MEMBER_FIGURE)Relation->nRelation;
    		ms_pSelf->UpdateListCheckButton(enumTONG_FIGURE_DIRECTOR);
		}
	}
}


/*********************************************************************
* ¹¦ÄÜ£º°ÑÐÂµ½µÄÁÐ±íÐÅÏ¢´æµ½ÁÙÊ±ÎÄ¼þ
**********************************************************************/
void KUiTongManager::SaveNewData(KUiGameObjectWithName *ParamInfo, KTongMemberItem *pIncome)
{
	char szSection[32];
	KIniFile Ini;

	if(ParamInfo->nData == enumTONG_FIGURE_DIRECTOR)
	{
		sprintf(szSection, "%s_Director", ParamInfo->szName);
	}
	else if(ParamInfo->nData == enumTONG_FIGURE_MANAGER)
	{
		sprintf(szSection, "%s_Manager", ParamInfo->szName);
	}
	else if(ParamInfo->nData == enumTONG_FIGURE_MEMBER)
	{
		sprintf(szSection, "%s_Member", ParamInfo->szName);
	}
	if(Ini.Load(TONG_DATA_TEMP_FILE))
	{
		int nCount, nDataCount;
		char szKey[32];

		Ini.GetInteger(szSection, "Count", 0, &nCount);
		nDataCount = nCount + ParamInfo->nParam;
		for(int i = nCount;i < nDataCount;i++)
		{
			sprintf(szKey, "%d_Name", i);
			Ini.WriteString(szSection, szKey, pIncome[i - nCount].Name);

			sprintf(szKey, "%d_AgName", i);
			Ini.WriteString(szSection, szKey, pIncome[i - nCount].szAgname);

			sprintf(szKey, "%d_Hash", i);
			Ini.WriteInteger(szSection, szKey, StringToHash(pIncome[i - nCount].Name));
		}
		Ini.Save(TONG_DATA_TEMP_FILE);
	}
}


/*********************************************************************
* ¹¦ÄÜ£º°ÑÖ¸¶¨ÀàÐÍµÄÁÐ±íÊý¾ÝÕû¸ö´æ´¢ÆðÀ´
**********************************************************************/
void KUiTongManager::SaveWholeData(int nType)
{
	if(ms_pSelf && ms_pSelf->m_TongData.szName[0])
	{
    	KLinkStruct<KPilgarlicItem> *pData = NULL;
		char szSection[32];
		switch(nType)
		{
		case enumTONG_FIGURE_DIRECTOR:
			pData = &ms_pSelf->m_ElderData;
			sprintf(szSection, "%s_Director", ms_pSelf->m_TongData.szName);
			break;

		case enumTONG_FIGURE_MANAGER:
			pData = &ms_pSelf->m_CaptainData;
			sprintf(szSection, "%s_Manager", ms_pSelf->m_TongData.szName);
			break;

		case enumTONG_FIGURE_MEMBER:
			pData = &ms_pSelf->m_MemberData;
			sprintf(szSection, "%s_Member", ms_pSelf->m_TongData.szName);
			break;
		}
		if(pData)
		{
			KPilgarlicItem *pNode = pData->Reset();
			int nCount = pData->GetCount();
			char szKey[32];
			KIniFile Ini;
		/*----------------------------------*/
			Ini.Load(TONG_DATA_TEMP_FILE);
			Ini.EraseSection(szSection);
			for(int i = 0;i < nCount;i++)
			{
				sprintf(szKey, "%d_Name", i);
			    Ini.WriteString(szSection, szKey, pNode->Name);

			    sprintf(szKey, "%d_AgName", i);
			    Ini.WriteString(szSection, szKey, pNode->szAgname);

			    sprintf(szKey, "%d_Hash", i);
			    Ini.WriteInteger(szSection, szKey, pNode->nHashID);

				pNode = pData->NextItem();
			}
		}
	}
}


/*********************************************************************
* ¹¦ÄÜ£ºÔØÈë×ÊÁÏ
**********************************************************************/
void KUiTongManager::LoadData(int nType)
{
	if(m_TongData.szName[0])
	{
    	char szSection[32];

	    switch(nType)
	    {
	    case enumTONG_FIGURE_DIRECTOR:
    		sprintf(szSection, "%s_Director", m_TongData.szName);
		    if(!(m_nElderDataIndex = LoadDataHandler(&m_ElderData, szSection)) &&
				m_nElderDataIndex < m_TongData.nDirectorCount)
			{
				m_nElderDataIndex = 0;
    			RequestData(enumTONG_FIGURE_DIRECTOR, m_nElderDataIndex);
			}
		    break;

	    case enumTONG_FIGURE_MANAGER:
    		sprintf(szSection, "%s_Manager", m_TongData.szName);
		    if(!(m_nCaptainDataIndex = LoadDataHandler(&m_CaptainData, szSection)))
			{
				m_nCaptainDataIndex = 0;
    			RequestData(enumTONG_FIGURE_MANAGER, m_nCaptainDataIndex);
			}
		    break;

	    case enumTONG_FIGURE_MEMBER:
    		sprintf(szSection, "%s_Member", m_TongData.szName);
		    if(!(m_nMemberDataIndex = LoadDataHandler(&m_MemberData, szSection)))
			{
				m_nMemberDataIndex = 0;
    			RequestData(enumTONG_FIGURE_MEMBER, m_nMemberDataIndex);
			}
		    break;

		default:
			break;
	    }
	}
}


/*********************************************************************
* ¹¦ÄÜ£ºÔØÈë×ÊÁÏµÄ´¦Àíº¯Êý
**********************************************************************/
int KUiTongManager::LoadDataHandler(KLinkStruct<KPilgarlicItem> *pData,
									const char* szSection)
{
	char szFileName[256];
	KIniFile Ini;

	sprintf(szFileName, "%s\\%s", g_UiBase.GetUserTempDataFolder(), TONG_DATA_TEMP_FILE);
	if(Ini.Load(szFileName) && m_TongData.szName[0] && Ini.IsSectionExist(szSection))
	{
		int nCount, i;
		char szKey[32];
		KPilgarlicItem AddItem;

		pData->Clear();
		Ini.GetInteger(szSection, "Count", 0, &nCount);

		for(i = 0;i < nCount;i++)
		{
			sprintf(szKey, "%d_Name", i);
			Ini.GetString(szSection, szKey, "", AddItem.Name, sizeof(AddItem.Name));

			sprintf(szKey, "%d_AgName", i);
			Ini.GetString(szSection, szKey, "", AddItem.szAgname, sizeof(AddItem.szAgname));

			sprintf(szKey, "%d_Hash", i);
			Ini.GetInteger(szSection, szKey, 0, &AddItem.nHashID);

			pData->AddItem(AddItem);
		}
		UpdateTongListView();
		return nCount;
	}
	return 0;
}


/*********************************************************************
* ¹¦ÄÜ£ºÏòCore·¢³öÊý¾ÝÇëÇó
**********************************************************************/
void KUiTongManager::RequestData(int nType, int nIndex)
{
	if(g_pCoreShell)
	{
    	KUiGameObjectWithName ParamInfo;
    	strcpy(ParamInfo.szName, m_TongData.szName);
    	ParamInfo.nData = nType;
    	ParamInfo.nParam = nIndex;
    	g_pCoreShell->TongOperation(GTOI_REQUEST_TONG_DATA, (unsigned int)&ParamInfo, 0);
	}
}


/*********************************************************************
* ¹¦ÄÜ£ºÉ¾³ýÁÙÊ±ÎÄ¼þ
**********************************************************************/
int KUiTongManager::RemoveTempFile()
{
	char szPath[256], szFile[128];

	sprintf(szFile, "%s\\%s", g_UiBase.GetUserTempDataFolder(), TONG_DATA_TEMP_FILE);
	g_GetFullPath(szPath, szFile);
	return remove(szPath);
}


/*********************************************************************
* ¹¦ÄÜ£º¸÷ÖÖ°ï»á²Ù×÷µÄ´¦Àíº¯Êý
**********************************************************************/
void KUiTongManager::OnAction(int nType)
{
	if(g_pCoreShell)
	{
		KPilgarlicItem *pNode = GetSelectedPlayer();
		KTongOperationParam Param;
		KTongMemberItem Player;

		memset(&Player, 0, sizeof(KTongMemberItem));
		Param.eOper = (TONG_ACTION_TYPE)nType;
		if(nType == TONG_ACTION_LEAVE)
		{
			Param.nData[0] = m_nRelation;
			Param.nData[1] = -1;
		}
		else
		{
			Param.nData[0] = m_nCurrentCheckBox;
			Param.nData[1] = m_nIndex;
		}
		strcpy(Param.szPassword, m_szPassword);

		if(m_szTargetPlayerName[0])
		{
			strcpy(Player.Name, m_szTargetPlayerName);
		}
	    else if(pNode)
	    {
		    strcpy(Player.Name, pNode->Name);
	    }
		else
		{
			strcpy(Player.Name, m_szPlayerName);
		}
		// == add by Fong KiÒu
		if(nType == TONG_ACTION_CHANGE_WAYEDIT)
		{
			char	szBuff[32];
			m_WayEdit.GetText(szBuff, sizeof(szBuff), true);
			strcpy(Param.lpszParam, szBuff);
		}
		if(nType == TONG_ACTION_CHANGE_NEXTTARGET)
		{
			char	szBuff[32];
			m_NextTargetEdit.GetText(szBuff, sizeof(szBuff), true);
			strcpy(Param.lpszParam, szBuff);
		}
		g_pCoreShell->TongOperation(GTOI_TONG_ACTION, (unsigned int)&Param, (int)&Player);
		memset(m_szPassword, 0, sizeof(m_szPassword));
	}
}


/*********************************************************************
* ¹¦ÄÜ£ºÏìÓ¦Ë¢ÐÂ°´Å¥µÄ²Ù×÷º¯Êý
**********************************************************************/
void KUiTongManager::OnRefresh()
{
	if(m_TongData.szName[0])
	{
		int nType;
		KIniFile Ini;
		char szSection[32];

		if(m_nCurrentCheckBox == enumTONG_FIGURE_DIRECTOR)
		{
			sprintf(szSection, "%s_Director", m_TongData.szName);
			m_ElderData.Clear();
			m_nElderDataIndex = 0;
		}
		else if(m_nCurrentCheckBox == enumTONG_FIGURE_MANAGER)
		{
			sprintf(szSection, "%s_Manager", m_TongData.szName);
			m_CaptainData.Clear();
			m_nCaptainDataIndex = 0;
		}
		else if(m_nCurrentCheckBox == enumTONG_FIGURE_MEMBER)
		{
			sprintf(szSection, "%s_Member", m_TongData.szName);
			m_MemberData.Clear();
			m_nMemberDataIndex = 0;
		}
		else
		{
			return;
		}
		nType = m_nCurrentCheckBox;

	    if(Ini.Load(TONG_DATA_TEMP_FILE))
	    {
    		Ini.EraseSection(szSection);
			Ini.Save(TONG_DATA_TEMP_FILE);
	    }

		m_List.ResetContent();
//		m_List2.ResetContent();
		LoadData(nType);
		// == Add By Fong Kieu ==
		OnAction(TONG_ACTION_CHANGE_WAYEDIT);
		OnAction(TONG_ACTION_CHANGE_NEXTTARGET);
		// == End Add By Fong Kieu ==
	}
}
void KUiTongManager::OnCheckOnline()
{
	if (IsOnline == FALSE)
	{
		IsOnline = TRUE;
		m_List.ResetContent();
		UpdateTongListView();
	}
	else
	{
		IsOnline = FALSE;
		m_List.ResetContent();
		UpdateTongListView();
	}	
}	
/*********************************************************************
* ¹¦ÄÜ£º¶Ô½á¹ûµÄÏìÓ¦Èë¿Ú
**********************************************************************/
void KUiTongManager::ResponseResult(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(ms_pSelf)
	{
    	switch(pResult->nData)
    	{
    	case TONG_ACTION_DISMISS:
			ms_pSelf->ResponseDismiss(pResult, nbIsSucceed);
		    break;

	    case TONG_ACTION_DEMISE:
			ms_pSelf->ResponseDemise(pResult, nbIsSucceed);
    		break;

    	case TONG_ACTION_LEAVE:
			ms_pSelf->ResponseLeave(pResult, nbIsSucceed);
		    break;

		case TONG_ACTION_ASSIGN:
			ms_pSelf->ResponseAssign(pResult, nbIsSucceed);
		    break;

		case TONG_ACTION_APPLY:
			ms_pSelf->ResponseApply(pResult, nbIsSucceed);
			break;
	    }
	}
	else
	{
		RemoveTempFile();
	}
}


/*********************************************************************
* ¹¦ÄÜ£ºÏìÓ¦Àë°ïºóµÄ²Ù×÷
**********************************************************************/
void KUiTongManager::ResponseLeave(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(nbIsSucceed)
	{
    	RemoveTempFile();
    	CloseWindow();
	}
}


/*********************************************************************
* ¹¦ÄÜ£ºÏìÓ¦ÌßÈËºóµÄ²Ù×÷
**********************************************************************/
void KUiTongManager::ResponseDismiss(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(nbIsSucceed)
	{
		if(strcmp(pResult->szName, m_szPlayerName) == 0)
		{
			RemoveTempFile();
			CloseWindow();
		}
		else
		{
			KLinkStruct<KPilgarlicItem> *pForDelete;
			pForDelete = FindPlayer(pResult->szName);
			if(pForDelete)
			{
				pForDelete->RemoveItem();
				SaveWholeData(pResult->nParam);
				m_List.ResetContent();
//				m_List2.ResetContent();
				UpdateTongListView();
			}
		}
	}
}


/*********************************************************************
* ¹¦ÄÜ£ºÏìÓ¦´«Î»ºóµÄ²Ù×÷
**********************************************************************/
void KUiTongManager::ResponseDemise(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(nbIsSucceed)
	{
		RemoveTempFile();
		ArrangeComposition(NULL);
	}
}


/*********************************************************************
* ¹¦ÄÜ£ºÏìÓ¦´«Î»ºóµÄ²Ù×÷
**********************************************************************/
void KUiTongManager::ResponseAssign(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(nbIsSucceed)
	{
		if(!strcmp(pResult->szName, m_szPlayerName))
		{
			RemoveTempFile();
		    ArrangeComposition(NULL);
		}
		else
		{
			KLinkStruct<KPilgarlicItem> *pPlayer = FindPlayer(pResult->szName);
			if(pPlayer)
			{
				KPilgarlicItem Add, *pNode = pPlayer->Current();
				memcpy(&Add, pNode, sizeof(KPilgarlicItem));
				pPlayer->RemoveItem();
				switch(pResult->nParam)
				{
				case enumTONG_FIGURE_DIRECTOR:
					m_ElderData.AddItem(Add);
					break;

				case enumTONG_FIGURE_MANAGER:
					m_CaptainData.AddItem(Add);
					break;

				case enumTONG_FIGURE_MEMBER:
					m_MemberData.AddItem(Add);
					break;

				default:
					return;
				}
				SaveWholeData(pResult->nParam);
				SaveWholeData(pResult->uParam);
				m_List.ResetContent();
//				m_List2.ResetContent();
				UpdateTongListView();
			}
			//////////
		}
		/////////
	}
	////////
}


/*********************************************************************
* ¹¦ÄÜ£ºÏìÓ¦ÉêÇëºóµÄ²Ù×÷
**********************************************************************/
void KUiTongManager::ResponseApply(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(nbIsSucceed)
	{
		if(!strcmp(m_szPlayerName, pResult->szName))
		{
			RemoveTempFile();
			ArrangeComposition(NULL);
		}
		else
		{
			KPilgarlicItem Add;
			strcpy(Add.Name, pResult->szName);
			strcpy(Add.szAgname, pResult->szString);
			Add.nHashID = StringToHash(Add.Name);
			m_MemberData.AddItem(Add);
			SaveWholeData(enumTONG_FIGURE_MEMBER);
			m_List.ResetContent();
//			m_List2.ResetContent();
			UpdateTongListView();
		}
	}
}


/*********************************************************************
* ¹¦ÄÜ£º²éÑ¯ÌØ¶¨Ãû×ÖµÄÍæ¼ÒµÄÊý¾ÝÎ»ÖÃ
**********************************************************************/
KLinkStruct<KPilgarlicItem>* KUiTongManager::FindPlayer(char *pszName)
{
	KLinkStruct<KPilgarlicItem> *pData = NULL;
	if(pszName && pszName[0])
	{
    	pData = HandleFind(&m_MemberData, pszName);
    	if(!pData)
    	{
		    pData = HandleFind(&m_CaptainData, pszName);
		    if(!pData)
		    {
    			pData = HandleFind(&m_ElderData, pszName);
		    }
	    }
	}
	return pData;
}


/*********************************************************************
* ¹¦ÄÜ£º²éÕÒÍæ¼Ò´¦Àíº¯Êý
**********************************************************************/
KLinkStruct<KPilgarlicItem>* KUiTongManager::HandleFind(KLinkStruct<KPilgarlicItem> *pData, char *pszName)
{
	KLinkStruct<KPilgarlicItem> *pReturn = NULL;
	KPilgarlicItem *pNode = pData->Reset();
	int nID = StringToHash(pszName);
	int nCount = pData->GetCount();

	for(int i = 0;i < nCount;i++)
	{
		if(nID == pNode->nHashID)
		{
			if(strcmp(pszName, pNode->Name) == 0)
			{
				pReturn = pData;
				break;
			}
		}
		pNode = pData->NextItem();
	}
	return pReturn;
}

//-----------------------------------------------------
// Xu ly su kien ngan quy
//-----------------------------------------------------
void KUiTongManager::OnMoney(int nType ,int nMoney)
{	
	if (g_pCoreShell)
	{
		KUiPlayerItem Param;
		strcpy(Param.Name,m_szPlayerName);
		Param.nIndex = 0;
		Param.nData = nMoney;
		Param.uId = 0;
		g_pCoreShell->TongOperation(GTOI_TONG_MONEY_ACTION, (unsigned int)&Param, nType);
	}
}


unsigned long StringToHash(const char *pString, BOOL bIsCaseSensitive)
{
	if(pString && pString[0])
	{
		unsigned long id = 0;
		const char *ptr;
		int index = 0;

		if(bIsCaseSensitive)
		{
			ptr = pString;

			while(*ptr)
			{
    			id = (id + (++index) * (*ptr)) % 0x8000000b * 0xffffffef;
		        ptr++;
	        }
		}
		else
		{
			char Buff[256];
			strcpy(Buff, pString);
			strlwr(Buff);
			ptr = Buff;

        	while(*ptr)
		    {
    			id = (id + (++index) * (*ptr)) % 0x8000000b * 0xffffffef;
		        ptr++;
	        }
		}
		return (id ^ 0x12345678);
	}

	return 0;
}

