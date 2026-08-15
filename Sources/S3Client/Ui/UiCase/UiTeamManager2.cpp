//=====================================
// File name    :
// Develop      : Anthony
//=====================================

#include "KWin32.h"
#include "KIniFile.h"
#include "KWin32Wnd.h"
#include "UiTeamManager2.h"
#include "../UiBase.h"
#include "../UiShell.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "UiLoginBg.h"
#include "UiSysMsgCentre.h"
#include "../../../core/src/CoreShell.h"
#include <crtdbg.h>
extern iCoreShell*		g_pCoreShell;
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
extern iRepresentShell*	g_pRepresentShell;

#define SCHEME_INI_TEAMMNG2 "UiTeamManager2.ini"

KUiTeamManager2 *KUiTeamManager2::m_pSelf = NULL;

KUiTeamManager2::KUiTeamManager2()
{
	memset(&m_Info, 0, sizeof(KUiPlayerTeam));
	m_pPlayersList = NULL;
	eShowTMG = 2;				//trang thai chua load ini
	nCountS = 0;
	nPainTMG = NULL;
}

KUiTeamManager2::~KUiTeamManager2()
{

}

KUiTeamManager2* KUiTeamManager2::OpenWindow()
{
    if (m_pSelf == NULL)
    {
        m_pSelf = new KUiTeamManager2;
        if (m_pSelf)
            m_pSelf->Initialize();
    }
    if (m_pSelf)
    {
		m_pSelf->UpdateData(NULL);
        m_pSelf->Show();
    }
    return m_pSelf;
}

void KUiTeamManager2::CloseWindow()
{
    if (m_pSelf)
    {
		m_pSelf->Clear();
        m_pSelf->Destroy();
        m_pSelf = NULL;
    }
}

void KUiTeamManager2::Clear()
{
	memset(&m_Info, 0, sizeof(KUiPlayerTeam));
	if (m_pPlayersList)
	{
		free(m_pPlayersList);
		m_pPlayersList = NULL;
	}
	if(nPainTMG)
	{
		free(nPainTMG);
		nPainTMG = NULL;
	}
}

void KUiTeamManager2::SetFactionIcon(BYTE faction, KWndButton &btn)
{
	switch(faction)
	{
		case 0:
			btn.SetImage(ISI_T_SPR,"\\spr\\Ui4\\主界面\\组队预览\\icon_zd_sl.spr",true);
			break;
		case 1:
			btn.SetImage(ISI_T_SPR, "\\spr\\Ui4\\主界面\\组队预览\\icon_zd_tw.spr",true); 
			break;
		case 2:
			btn.SetImage(ISI_T_SPR, "\\spr\\Ui4\\主界面\\组队预览\\icon_zd_tm.spr",true); 
			break;
		case 3:
			btn.SetImage(ISI_T_SPR, "\\spr\\Ui4\\主界面\\组队预览\\icon_zd_wd.spr",true); 
			break;
		case 4:
			btn.SetImage(ISI_T_SPR, "\\spr\\Ui4\\主界面\\组队预览\\icon_zd_em.spr",true); 
			break;
		case 5:
			btn.SetImage(ISI_T_SPR, "\\spr\\Ui4\\主界面\\组队预览\\icon_zd_cy.spr",true); 
			break;
		case 6:
			btn.SetImage(ISI_T_SPR, "\\spr\\Ui4\\主界面\\组队预览\\icon_zd_gb.spr",true); 
			break;
		case 7:
			btn.SetImage(ISI_T_SPR, "\\spr\\Ui4\\主界面\\组队预览\\icon_zd_tr.spr",true); 
			break;
		case 8:
			btn.SetImage(ISI_T_SPR, "\\spr\\Ui4\\主界面\\组队预览\\icon_zd_wu.spr",true); 
			break;
		case 9:
			btn.SetImage(ISI_T_SPR, "\\spr\\Ui4\\主界面\\组队预览\\icon_zd_kl.spr",true); 
			break;
		case 10:
			btn.SetImage(ISI_T_SPR, "\\spr\\Ui4\\主界面\\组队预览\\icon_zd_hsp.spr",true); 
			break;
		default:
			btn.SetImage(ISI_T_SPR, "\\spr\\Ui4\\主界面\\组队预览\\icon_zd_new.spr",true); 
			break;
	}
}

void KUiTeamManager2::UpdateData(KUiPlayerTeam* pInfo)
{
	m_btnSwitch.Hide(); m_btnCaptainFlag.Hide();
	for(int i =0; i < 8; i++)
	{
		a_LeaderAbility[i].Hide();
		a_IconHead[i].Hide();
		a_btnBackGround[i].Hide();
	}

	Clear();

	if (pInfo)
		m_Info = *pInfo;
	else if (!g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&m_Info, 0))
	{
		memset(&m_Info, 0, sizeof(m_Info));
	}

	if (m_Info.cNumMember > 0)
	{
		m_btnSwitch.Show(); 

		if(eShowTMG==2)
		{
			KIniFile* pConfigFile = NULL;
			pConfigFile = g_UiBase.GetAutoSettingFile();
			if (pConfigFile)
			{
				BOOL	m_bHSTeamMNG;
				pConfigFile->GetInteger("Fighting", "HSBtnTeamMNG", 0, (int*)(&m_bHSTeamMNG));
				if (m_bHSTeamMNG)
				{
					eShowTMG = 1;
				}
				else
				{
					eShowTMG = 0;
				}
				m_btnSwitch.CheckButton(m_bHSTeamMNG);//add by anthony change button + -
			}
		}

		if(eShowTMG==0) return;

		m_pPlayersList = (KUiPlayerItem*)malloc(sizeof(KUiPlayerItem) * (m_Info.cNumMember));
		if (m_pPlayersList)
		{
			int nCount = g_pCoreShell->TeamOperation(TEAM_OI_GD_MEMBER_LIST, (unsigned int)m_pPlayersList, m_Info.cNumMember);
			_ASSERT(nCount == m_Info.cNumMember);
			nCountS = nCount;
			m_btnCaptainFlag.Show();
			for(int i =0; i < nCount; i++)
			{
				a_LeaderAbility[i].SetText(m_pPlayersList[i].Name); a_LeaderAbility[i].Show();
				SetFactionIcon(m_pPlayersList[i].nFaction,a_IconHead[i]); a_IconHead[i].Show();
				a_btnBackGround[i].Show();
			}
		}
		else
		{
			m_Info.cNumMember = 0;
		}

		nPainTMG = (KUiPlayerPaintTeamMNG*)malloc(sizeof(KUiPlayerPaintTeamMNG));
		if(nPainTMG)
		{
			nPainTMG->nCountS = nCountS;
			nPainTMG->nWeightOffset = nWeightOffset;
			nPainTMG->nHeightOffset_life = nHeightOffset_life;
			nPainTMG->nHeightOffset_mana = nHeightOffset_mana;
			nPainTMG->nOffSet_member = nOffSet_member;
			nPainTMG->nWid = nWid;
			nPainTMG->nHei_life = nHei_life;
			nPainTMG->nHei_mana = nHei_mana;
		}
	}
}

void KUiTeamManager2::PaintWindow()
{
	if (g_pRepresentShell == NULL)
		return;

	g_pCoreShell->TeamOperation(TEAM_OI_GET_NPC_MAP_POS, (unsigned int)m_pPlayersList, (unsigned int)nPainTMG);

}

KUiTeamManager2* KUiTeamManager2::GetUiTeamManager()
{
    if (m_pSelf == NULL)
    {
        m_pSelf = new KUiTeamManager2;
        if (m_pSelf)
            m_pSelf->Initialize();
    }
    if (m_pSelf)
    {
		
    }
    return m_pSelf;
}



void KUiTeamManager2::Initialize()
{
	// team_mananager
	
	AddChild(&m_btnSwitch);
	AddChild(&m_btnCaptainFlag);
	for (int i = 0; i < 8; i++)
	{
		AddChild(&a_LeaderAbility[i]);
		AddChild(&a_btnBackGround[i]);
		AddChild(&a_IconHead[i]);
	}

    char Scheme[128];
    g_UiBase.GetCurSchemePath(Scheme, 128);
    m_pSelf->LoadScheme(Scheme);
    Wnd_AddWindow(this, WL_TOPMOST);
}

void KUiTeamManager2::LoadScheme(const char *pScheme)
{
    char Buff[128];
    KIniFile    Ini;
    sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_TEAMMNG2);
    if (Ini.Load(Buff))
    {
		Ini.GetInteger("Main", "nWeightOffset",  325, &nWeightOffset);
		Ini.GetInteger("Main", "nHeightOffset_life",  269, &nHeightOffset_life);
		Ini.GetInteger("Main", "nHeightOffset_mana",  265, &nHeightOffset_mana);
		Ini.GetInteger("Main", "nOffSet_member",  38, &nOffSet_member);
		Ini.GetInteger("Main", "nWid",  58, &nWid);
		Ini.GetInteger("Main", "nHei_life",  7, &nHei_life);
		Ini.GetInteger("Main", "nHei_mana",  4, &nHei_mana);

        KWndShowAnimate::Init(&Ini, "Main");
		// team_mananager
		m_btnSwitch		.Init(&Ini, "ButtonSwitch");
		m_btnCaptainFlag	.Init(&Ini, "CaptainFlag");
		for (int i = 0; i < 8; i++)
		{
			char steam[32];
			memset(steam,0,sizeof(steam));
			sprintf(steam, "Head%d",i);
			m_pSelf->a_IconHead[i].Init(&Ini, steam);
			sprintf(steam, "BackGround%d",i);
			m_pSelf->a_btnBackGround[i].Init(&Ini, steam);
			sprintf(steam, "Name%d",i);
			m_pSelf->a_LeaderAbility[i].Init(&Ini, steam);
		}		
    }
}

KUiTeamManager2* KUiTeamManager2::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

int KUiTeamManager2::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
    int nRet = 0;
    switch (uMsg)
    {
		case WND_N_BUTTON_CLICK:
		{
			if (uParam == (unsigned int)(KWndWindow*)&m_btnSwitch)
			{
				if(eShowTMG == 1)
					eShowTMG = 0;
				else if(eShowTMG ==0)
					eShowTMG = 1;

				m_btnSwitch.CheckButton(eShowTMG);//add by anthony change button + -

				KIniFile* pConfigFile = NULL;
				pConfigFile = g_UiBase.GetAutoSettingFile();
				if (pConfigFile)
				{
					if(eShowTMG==1)
					{
						pConfigFile->WriteInteger("Fighting", "HSBtnTeamMNG", 1);
					}
					else
					{
						pConfigFile->WriteInteger("Fighting", "HSBtnTeamMNG", 0);
					}
				}
			}
		}
        break;
    
    default:
        break;
    }
    return nRet;
}