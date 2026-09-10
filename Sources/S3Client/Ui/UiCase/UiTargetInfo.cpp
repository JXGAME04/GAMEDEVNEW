//=====================================
// File name    :
// Develop      : Anthony
//=====================================

#include "KWin32.h"
#include "KIniFile.h"
#include "KWin32Wnd.h"
#include "UiTargetInfo.h"
#include "../UiBase.h"
#include "../UiShell.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "UiLoginBg.h"
#ifdef JX_ANDROID
#include "KDebug.h"
#include "UiGame.h"		// [ANDROID 09/09 MENU] PopUpContextPeopleMenu
#endif
#include "UiSysMsgCentre.h"
#include "../../../core/src/CoreShell.h"
#include <crtdbg.h>
extern iCoreShell*		g_pCoreShell;
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
extern iRepresentShell*	g_pRepresentShell;

#define SCHEME_INI_TARGETINFO "KUiTargetInfo.ini"

KUiTargetInfo *KUiTargetInfo::m_pSelf = NULL;

KUiTargetInfo::KUiTargetInfo()
{
	memset(&m_Info, 0, sizeof(KUiPlayerTeam));
	m_pPlayersList = NULL;
	eShowTMG = 2;				//trang thai chua load ini
	nCountS = 0;
	nPainTMG = NULL;
}

KUiTargetInfo::~KUiTargetInfo()
{

}

KUiTargetInfo* KUiTargetInfo::OpenWindow()
{
    if (m_pSelf == NULL)
    {
        m_pSelf = new KUiTargetInfo;
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

void KUiTargetInfo::CloseWindow()
{
    if (m_pSelf)
    {
		m_pSelf->Clear();
        m_pSelf->Destroy();
        m_pSelf = NULL;
    }
}

void KUiTargetInfo::Clear()
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

void KUiTargetInfo::SetFactionIcon(BYTE series, KWndButton &btn)
{
	switch(series)
	{
	case 0:
		btn.SetImage(ISI_T_SPR, "\\Spr\\Ui3\\UiTargetInfo\\kim.spr", true);
		break;
	case 1:
		btn.SetImage(ISI_T_SPR, "\\Spr\\Ui3\\UiTargetInfo\\moc.spr", true);
		break;
	case 2:
		btn.SetImage(ISI_T_SPR, "\\Spr\\Ui3\\UiTargetInfo\\thuy.spr", true);
		break;
	case 3:
		btn.SetImage(ISI_T_SPR, "\\Spr\\Ui3\\UiTargetInfo\\hoa.spr", true);
		break;
	case 4:
		btn.SetImage(ISI_T_SPR, "\\Spr\\Ui3\\UiTargetInfo\\tho.spr", true);
	default:
		break;
	}
}

void KUiTargetInfo::UpdateData(KUiTargetDetailInfo* pInfo)
{

	m_btnSwitch.Hide();
	m_btnCaptainFlag.Hide();
	a_IconHead.Hide();
	a_btnBackGround.Hide();
	m_pLifePercent.Hide();
	m_pTargetName.Hide();

	Clear();

	if (!g_pCoreShell->CheckMapLoiDai())
		return;
		
	if (pInfo)
		m_Info = *pInfo;
	else if (!g_pCoreShell->GetGameData(NPC_OI_TARGET_INFO, (KUPARAM)&m_Info, 0)) //m_Info.sTargetName = NULL if no target
	{
		memset(&m_Info, 0, sizeof(m_Info));
	}
	if(strlen(m_Info.sTargetName) > 0)
	{
		m_pPlayersList = (KUiPlayerItem*)malloc(sizeof(KUiPlayerItem)); //1 target
		if (m_pPlayersList)
		{
			memset(m_pPlayersList, 0, sizeof(m_pPlayersList));
			int nCount = 1;// g_pCoreShell->TeamOperation(TEAM_OI_GD_MEMBER_LIST, (KUPARAM)m_pPlayersList, m_Info.cNumMember);
			nCountS = 1;
			//m_btnCaptainFlag.Show();
			strcpy_s(m_pPlayersList[0].Name, sizeof(m_pPlayersList[0].Name), m_Info.sTargetName);
			m_pPlayersList[0].nFaction = m_Info.Series;
			m_pPlayersList[0].nPercenLife = m_Info.nLifePercent;

			SetFactionIcon(m_pPlayersList[0].nFaction,a_IconHead); a_IconHead.Show();
			a_btnBackGround.Show();
			
			m_pLifePercent.SetIntText(m_Info.nLifePercent, '%'); m_pLifePercent.Show();
			m_pTargetName.SetText(m_pPlayersList[0].Name); m_pTargetName.Show();
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

void KUiTargetInfo::PaintWindow()
{
	if (g_pRepresentShell == NULL)
		return;

	g_pCoreShell->OperationRequest(GOI_DRAW_TARGET_INFO, (KUPARAM)m_pPlayersList, (KUPARAM)nPainTMG);

}

KUiTargetInfo* KUiTargetInfo::GetUiTargetInfo()
{
    if (m_pSelf == NULL)
    {
        m_pSelf = new KUiTargetInfo;
        if (m_pSelf)
            m_pSelf->Initialize();
    }
    if (m_pSelf)
    {
		
    }
    return m_pSelf;
}



void KUiTargetInfo::Initialize()
{
	// team_mananager
	
	AddChild(&m_btnSwitch);
	AddChild(&m_btnCaptainFlag);
	AddChild(&a_btnBackGround);
	AddChild(&a_IconHead);
	AddChild(&m_pLifePercent);
	AddChild(&m_pTargetName);

    char Scheme[128];
    g_UiBase.GetCurSchemePath(Scheme, 128);
    m_pSelf->LoadScheme(Scheme);
    Wnd_AddWindow(this, WL_LOWEST);
}

void KUiTargetInfo::LoadScheme(const char *pScheme)
{
    char Buff[128];
    KIniFile    Ini;
    sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_TARGETINFO);
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
#ifdef JX_ANDROID
        // [ANDROID 09/09 MENU] O [Main] cua kuitargetinfo.ini chi la mot o 27x23, con than thanh
        // (BackGround0, Head0...) la cac o CON nam ngoai o do. Nen cham vao than thanh se ROI RA
        // NGOAI cua so, xuong ban do -> bo chon muc tieu thay vi mo menu (da do tan mat).
        // Bat co "do trung theo ca dam con" de ca than thanh nhan duoc cham.
        m_Style |= WND_S_SIZE_WITH_ALL_CHILD;
#endif
		// team_mananager
		m_btnSwitch		.Init(&Ini, "ButtonSwitch");
		m_btnCaptainFlag	.Init(&Ini, "CaptainFlag");
		char steam[32];
		int i = 0;
		memset(steam,0,sizeof(steam));
		sprintf(steam, "Head%d",i);
		m_pSelf->a_IconHead.Init(&Ini, steam);
		sprintf(steam, "BackGround%d",i);
		m_pSelf->a_btnBackGround.Init(&Ini, steam);
		m_pSelf->m_pLifePercent.Init(&Ini, "LifePercent");
		m_pSelf->m_pTargetName.Init(&Ini, "Name");
    }
}

KUiTargetInfo* KUiTargetInfo::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

#ifdef JX_ANDROID
//--------------------------------------------------------------------------
// [ANDROID 09/09 MENU] Cham vao thanh thong tin muc tieu -> ra danh sach tuy chon.
//
// Duong di tren dien thoai: cham doi tuong -> thanh nay hien len (da co san) -> cham vao thanh
// -> danh sach giao dich / to doi / ket ban / xem trang bi. Tren ban PC danh sach do mo bang
// Ctrl + chuot phai; ngon tay khong co phim Ctrl nen di duong nay.
//
// Cach dung KUiPlayerItem tu TEN muc tieu lay y het UiMsgCentrePad.cpp (bam vao ten nguoi trong
// khung chat): hoi Core bang FindSpecialNPC, khong thay thi dien tay ten va de nIndex = -1.
// Chi nguoi choi moi co danh sach nay; muc tieu la quai / NPC thuong thi khong lam gi.
//--------------------------------------------------------------------------
void KUiTargetInfo::MoMenuMucTieu()
{
	if (g_pCoreShell == NULL || m_Info.sTargetName[0] == 0)
		return;
	KUiPlayerItem SelectPlayer;
	memset(&SelectPlayer, 0, sizeof(SelectPlayer));
	int nKind = -1;
	int nThay = g_pCoreShell->FindSpecialNPC(m_Info.sTargetName, &SelectPlayer, nKind);
	g_DebugLog("[MENU] cham thanh muc tieu: ten=\"%s\" thay=%d kind=%d", m_Info.sTargetName, nThay, nKind);
	if (!(nThay && nKind == kind_player))
		return;		// khong phai nguoi choi thi khong co danh sach tuy chon
	// Dat danh sach NGAY DUOI than thanh, khong de len thanh: o [Main] chi la mot o 27x23 con than
	// thanh la cac o CON (BackGround0, Head0...), nen phai lay khung bao ca dam con moi ra dung day.
	int x = 0, y = 0;
	RECT rc;
	GetAllChildLayoutRect(&rc);
	if (rc.right > rc.left && rc.bottom > rc.top)
	{
		x = rc.left;
		y = rc.bottom + 4;
	}
	else
	{
		GetAbsolutePos(&x, &y);
		y += m_Height;
	}
	PopUpContextPeopleMenu(SelectPlayer, x, y);
}
#endif

int KUiTargetInfo::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
    int nRet = 0;
    switch (uMsg)
    {
		case WND_N_BUTTON_CLICK:
		{
#ifdef JX_ANDROID
			// [ANDROID 09/09 MENU] bam vao nen hoac anh dau cua thanh = mo danh sach tuy chon
			if (uParam == (KUPARAM)(KWndWindow*)&a_btnBackGround ||
				uParam == (KUPARAM)(KWndWindow*)&a_IconHead)
			{
				MoMenuMucTieu();
				break;
			}
#endif
			if (uParam == (KUPARAM)(KWndWindow*)&m_btnSwitch)
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