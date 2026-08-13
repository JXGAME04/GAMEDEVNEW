/*****************************************************************************************
//	ΩÁ√Ê--¥¢ŒÔœ‰ΩÁ√Ê
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-4-21
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

#define SCHEME_INI_ITEM	"UiExBox3.ini"

KUiExBox3* KUiExBox3::m_pSelf = NULL;

enum WAIT_OTHER_WND_OPER_PARAM
{
	UISTOREBOX_WAIT_GETMONEY,
};

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫»Áπ˚¥∞ø⁄’˝±ªœ‘ æ£¨‘Ú∑µªÿ µ¿˝÷∏’Î
//--------------------------------------------------------------------------
KUiExBox3* KUiExBox3::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫¥Úø™¥∞ø⁄£¨∑µªÿŒ®“ªµƒ“ª∏ˆ¿‡∂‘œÛ µ¿˝
//--------------------------------------------------------------------------
KUiExBox3* KUiExBox3::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiExBox3;
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
		Wnd_GameSpaceHandleInput(false);  // kh´ng cho ph–p nh©n vÀt di chuy”n
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫πÿ±’¥∞ø⁄
//--------------------------------------------------------------------------
void KUiExBox3::CloseWindow()
{
	if (m_pSelf)
	{
		//Wnd_GameSpaceHandleInput(true); // cho ph–p nh©n vÀt di chuy”n
		m_pSelf->Hide();
		m_pSelf = NULL;
	}
}

// -------------------------------------------------------------------------
// π¶ƒ‹	: ≥ı ºªØ
// -------------------------------------------------------------------------
void KUiExBox3::Initialize()
{
	AddChild(&m_SelectionThree);
	AddChild(&m_BuyOne);
	AddChild(&m_BuyTwo);
	AddChild(&m_CloseBtn);
	AddChild(&m_ItemBoxBa);
	AddChild(&m_InfoTextPage);
	AddChild(&m_TextPage);


	

	m_ItemBoxBa.SetContainerId((int)UOC_EX_BOX3);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫ππ‘Ï∫Ø ˝
//--------------------------------------------------------------------------
void KUiExBox3::UpdateData()
{
	m_ItemBoxBa.Clear();
	
	KUiObjAtRegion* pObjs = NULL;

	int nCount = g_pCoreShell->GetGameData(GDI_ITEM_IN_EX_BOX3, 0, 0);
	if (nCount == 0)
		return;

	if (pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount))
	{
		g_pCoreShell->GetGameData(GDI_ITEM_IN_EX_BOX3, (unsigned int)pObjs, nCount);//µ•œﬂ≥Ã÷¥––£¨nCount÷µ≤ª±‰
		for (int i = 0; i < nCount; i++)
			UpdateItem(&pObjs[i], 2);
		free(pObjs);
		pObjs = NULL;
	}
}

// -------------------------------------------------------------------------
// π¶ƒ‹	: ŒÔ∆∑±‰ªØ∏¸–¬
// -------------------------------------------------------------------------
void KUiExBox3::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
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
				m_ItemBoxBa.AddObject(&Obj, 1);
			else
				m_ItemBoxBa.RemoveObject(&Obj);
			if (!open)
				UiSoundPlayItem(Obj.uId);
		}
		/*else
		{
			m_nMoney = pItem->Obj.uId;
			m_Money.Set3IntText(m_nMoney);
		}*/
	}
	else
		UpdateData();
}

// -------------------------------------------------------------------------
// π¶ƒ‹	: ‘ÿ»ÎΩÁ√Ê∑Ω∞∏
// -------------------------------------------------------------------------
void KUiExBox3::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_ITEM);
	if (m_pSelf && Ini.Load(Buff))
	{
		m_pSelf->Init(&Ini, "Main");
		m_pSelf->m_SelectionThree.Init(&Ini, "SelectionThree");
		m_pSelf->m_BuyOne.Init(&Ini, "BuyOne");
		m_pSelf->m_BuyTwo.Init(&Ini, "BuyTwo");
		m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");
		m_pSelf->m_ItemBoxBa.Init(&Ini, "ItemBoxBa");
		m_pSelf->m_ItemBoxBa.EnableTracePutPos(true);
		m_pSelf->m_InfoTextPage.Init(&Ini, "InfoTextPage");
		m_pSelf->m_InfoTextPage.SetText("R≠¨ng 3");
		m_pSelf->m_TextPage.Init(&Ini,"TextPage");
		char szTextEx[32];
		sprintf(szTextEx,"Bπn Æ∑ mÎ rÈng Æ≠Óc %d/3",g_pCoreShell->GetGameData(GDI_EXBOX_ID, 0, 0));
		m_pSelf->m_TextPage.SetText(szTextEx);
	}
}

// -------------------------------------------------------------------------
// π¶ƒ‹	: ¥∞ø⁄∫Ø ˝
// -------------------------------------------------------------------------
int KUiExBox3::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
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
				uPr[1] = pos_exbox3room;
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
			CloseWindow();
		else if (uParam == (unsigned int)(KWndWindow*)&m_BuyOne)
		{
			KUiExBox1::OpenWindow2();
				CloseWindow();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_BuyTwo)
			KUiExBox2::OpenWindow();
			CloseWindow();
		
		break;
	default:
		return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}



void KUiExBox3::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
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
		Pick.eContainer = UOC_EX_BOX3;
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
		Drop.eContainer = UOC_EX_BOX3;
	}
	
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
		pPickPos ? (unsigned int)&Pick : 0,
		pDropPos ? (int)&Drop : 0);
}