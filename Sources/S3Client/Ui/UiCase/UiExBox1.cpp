/*****************************************************************************************
//	Copyright : Kingsoft 2003
//	Author	:   Fong Ki“u
//	CreateTime:	2020-4-21
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "UiExBox1.h"
#include "UiExBox2.h"
#include "UiExBox3.h"
#include "UiGetMoney.h"
#include "UiItem.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include <crtdbg.h>

extern iCoreShell*		g_pCoreShell;

#define SCHEME_INI_ITEM	"UiExBoxMain.ini"
#define SCHEME_INI_EX1	"UiExBox1.ini"

KUiExBox1* KUiExBox1::m_pSelf = NULL;

KUiExBox1* KUiExBox1::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

KUiExBox1* KUiExBox1::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiExBox1;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		if (KUiItem::GetIfVisible() == NULL)
			KUiItem::OpenWindow();
		else
			UiSoundPlay(UI_SI_WND_OPENCLOSE);

		m_pSelf->UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();
		Wnd_GameSpaceHandleInput(false);
	}
	return m_pSelf;
}

KUiExBox1* KUiExBox1::OpenWindow2()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiExBox1;
		if (m_pSelf)
			m_pSelf->Initialize2();
	}
	if (m_pSelf)
	{
		if (KUiItem::GetIfVisible() == NULL)
			KUiItem::OpenWindow();
		else
			UiSoundPlay(UI_SI_WND_OPENCLOSE);

		m_pSelf->UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();
		Wnd_GameSpaceHandleInput(false);

	}
	return m_pSelf;
}

void KUiExBox1::CloseWindow()
{
	if (m_pSelf)
	{
		//Wnd_GameSpaceHandleInput(true);
		m_pSelf->Hide();
		m_pSelf = NULL;
	}
}

void KUiExBox1::Initialize()
{
	AddChild(&m_SelectionOne);
	AddChild(&m_BuyTwo);
	AddChild(&m_BuyThree);
	AddChild(&m_CloseBtn);
	AddChild(&m_ItemBoxMot);
	AddChild(&m_InfoTextPage);
	AddChild(&m_TextPage);

	m_ItemBoxMot.SetContainerId((int)UOC_EX_BOX1);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

void KUiExBox1::Initialize2()
{
	AddChild(&m_SelectionOne);
	AddChild(&m_BuyTwo);
	AddChild(&m_BuyThree);
	AddChild(&m_CloseBtn);
	AddChild(&m_ItemBoxMot);
	AddChild(&m_InfoTextPage);
	AddChild(&m_TextPage);

	m_ItemBoxMot.SetContainerId((int)UOC_EX_BOX1);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme2(Scheme);

	Wnd_AddWindow(this);
}

void KUiExBox1::UpdateData()
{
	m_ItemBoxMot.Clear();
	
	KUiObjAtRegion* pObjs = NULL;

	int nCount = g_pCoreShell->GetGameData(GDI_ITEM_IN_EX_BOX1, 0, 0);
	if (nCount == 0)
		return;

	if (pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount))
	{
		g_pCoreShell->GetGameData(GDI_ITEM_IN_EX_BOX1, (unsigned int)pObjs, nCount);//µ•œﬂ≥Ã÷¥––£¨nCount÷µ≤ª±‰
		for (int i = 0; i < nCount; i++)
			UpdateItem(&pObjs[i], 2);
		free(pObjs);
		pObjs = NULL;
	}
}

void KUiExBox1::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		bool open = false;
		if (bAdd == 2) {
			open = true;
			bAdd -= 1;
		}
		if (open)
			UiSoundPlay(UI_SI_PICKPUT_ITEM);
		if (pItem->Obj.uGenre != CGOG_MONEY)
		{
			KUiDraggedObject Obj;
			Obj.uGenre = pItem->Obj.uGenre;
			Obj.uId = pItem->Obj.uId;
			Obj.DataX = pItem->Region.h;
			Obj.DataY = pItem->Region.v;
			Obj.DataW = pItem->Region.Width;
			Obj.DataH = pItem->Region.Height;
			if (bAdd)
				m_ItemBoxMot.AddObject(&Obj, 1);
			else
				m_ItemBoxMot.RemoveObject(&Obj);
			if (!open)
				UiSoundPlayItem(Obj.uId);
		}	
	}
	else
		UpdateData();
}

void KUiExBox1::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_ITEM);
	if (m_pSelf && Ini.Load(Buff))
	{
		m_pSelf->Init(&Ini, "Main");
		m_pSelf->m_SelectionOne.Init(&Ini, "SelectionOne");
		m_pSelf->m_BuyTwo.Init(&Ini, "BuyTwo");
		m_pSelf->m_BuyThree.Init(&Ini, "BuyThree");
		m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");
		m_pSelf->m_ItemBoxMot.Init(&Ini, "ItemBoxMot");
		m_pSelf->m_ItemBoxMot.EnableTracePutPos(true);
		m_pSelf->m_InfoTextPage.Init(&Ini, "InfoTextPage");
		m_pSelf->m_InfoTextPage.SetText("R≠¨ng 1");
		m_pSelf->m_TextPage.Init(&Ini,"TextPage");
		char szTextEx[32];
		sprintf(szTextEx,"Bπn Æ∑ mÎ rÈng Æ≠Óc %d/3",g_pCoreShell->GetGameData(GDI_EXBOX_ID, 0, 0));
		m_pSelf->m_TextPage.SetText(szTextEx);	
	}
}

void KUiExBox1::LoadScheme2(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_EX1);
	if (m_pSelf && Ini.Load(Buff))
	{
		m_pSelf->Init(&Ini, "Main");
		m_pSelf->m_SelectionOne.Init(&Ini, "SelectionOne");
		m_pSelf->m_BuyTwo.Init(&Ini, "BuyTwo");
		m_pSelf->m_BuyThree.Init(&Ini, "BuyThree");
		m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");
		m_pSelf->m_ItemBoxMot.Init(&Ini, "ItemBoxMot");
		m_pSelf->m_ItemBoxMot.EnableTracePutPos(true);
		m_pSelf->m_InfoTextPage.Init(&Ini, "InfoTextPage");
		m_pSelf->m_InfoTextPage.SetText("R≠¨ng 1");
		m_pSelf->m_TextPage.Init(&Ini,"TextPage");
		char szTextEx[32];
		sprintf(szTextEx,"Bπn Æ∑ mÎ rÈng Æ≠Óc %d/3",g_pCoreShell->GetGameData(GDI_EXBOX_ID, 0, 0));
		m_pSelf->m_TextPage.SetText(szTextEx);
		
	}
}

int KUiExBox1::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
		case WND_N_RIGHT_CLICK_ITEM:
		{
			if (g_UiBase.GetStatus() == UIS_S_IDLE)
			{
				KUiDraggedObject* pItem = (KUiDraggedObject*)uParam;
				unsigned int uPr[2];
				uPr[0] = pItem->uId;
				uPr[1] = pos_exbox1room;
				g_pCoreShell->OperationRequest(GOI_EXCHANGEITEM,
								(unsigned int)&uPr, pos_equiproom);
			}
		}
		break;
	case WND_N_ITEM_PICKDROP:
			if (g_pCoreShell->GetGameData(GDI_IS_CHEST_UNLOCKED, 0, 0))
			{
				OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
				break;
			}
			g_pCoreShell->OperationRequest(GOI_PLAYER_ACTION, CN_GH, 0);
				break;
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn)
		{
			CloseWindow();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_BuyTwo)
		{
			g_pCoreShell->OperationRequest(GOI_PLAYER_ACTION, EX_BOX2, 0);
			
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_BuyThree)
		{
			g_pCoreShell->OperationRequest(GOI_PLAYER_ACTION, EX_BOX3, 0);
		}	
		break;
	default:
		return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiExBox1::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	if (g_UiBase.GetStatus() != UIS_S_IDLE)
		return;
	KUiObjAtContRegion	Pick, Drop;
	KUiDraggedObject	Obj;

	if (pPickPos)
	{
		_ASSERT(pPickPos->pWnd);		
		((KWndObjectMatrix*)(pPickPos->pWnd))->GetObject(
			Obj, pPickPos->h, pPickPos->v);
		Pick.Obj.uGenre = Obj.uGenre;
		Pick.Obj.uId = Obj.uId;
		Pick.Region.Width = Obj.DataW;
		Pick.Region.Height = Obj.DataH;
		Pick.Region.h = Obj.DataX;
		Pick.Region.v = Obj.DataY;
		Pick.eContainer = UOC_EX_BOX1;
	}

	if (pDropPos)
	{
		Wnd_GetDragObj(&Obj);
		Drop.Obj.uGenre = Obj.uGenre;
		Drop.Obj.uId = Obj.uId;
		Drop.Region.Width = Obj.DataW;
		Drop.Region.Height = Obj.DataH;
		Drop.Region.h = pDropPos->h;
		Drop.Region.v = pDropPos->v;
		Drop.eContainer = UOC_EX_BOX1;
	}
	
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
		pPickPos ? (unsigned int)&Pick : 0,
		pDropPos ? (int)&Drop : 0);
}