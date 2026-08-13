/*****************************************************************************************
//	KUiStoreBox.cpp
//	Copyright : Kingsoft 2003
//	Author	:   Fong Ki襲
//	CreateTime:	2020-4-21
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "UiStoreBox.h"
#include "UiGetMoney.h"
#include "UiItem.h"
#include "UiUnlockBox.h"
#include "UiChangePWBox.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include <crtdbg.h>

extern iCoreShell*		g_pCoreShell;

#define SCHEME_INI_ITEM	"UiStoreBox.ini"

KUiStoreBox* KUiStoreBox::m_pSelf = NULL;

enum WAIT_OTHER_WND_OPER_PARAM
{
	UISTOREBOX_WAIT_GETMONEY,
};

KUiStoreBox* KUiStoreBox::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

KUiStoreBox* KUiStoreBox::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiStoreBox;
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

void KUiStoreBox::CloseWindow()
{
	if (m_pSelf)
	{
		Wnd_GameSpaceHandleInput(true);
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

void KUiStoreBox::Initialize()
{
	AddChild(&m_Money);
	AddChild(&m_GetMoneyBtn);
	AddChild(&m_CloseBtn);
	AddChild(&m_ItemBox);
	AddChild(&m_BtnLock);
	AddChild(&m_ChangePWBtn);
	AddChild(&m_BtnBox);
	m_ItemBox.SetContainerId((int)UOC_STORE_BOX);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	m_nMoney = 0;
	Wnd_AddWindow(this);
}

void KUiStoreBox::UpdateData()
{
	m_ItemBox.Clear();
	m_nMoney = 0;
	m_Money.SetText("0");

	KUiObjAtRegion* pObjs = NULL;

	int nCount = g_pCoreShell->GetGameData(GDI_ITEM_IN_STORE_BOX, 0, 0);
	if (nCount == 0)
		return;

	if (pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount))
	{
		g_pCoreShell->GetGameData(GDI_ITEM_IN_STORE_BOX, (unsigned int)pObjs, nCount);//单线程执行，nCount值不变
		for (int i = 0; i < nCount; i++)
			UpdateItem(&pObjs[i], 2);
		free(pObjs);
		pObjs = NULL;
	}
}

void KUiStoreBox::Breathe()
{
	if (g_pCoreShell && g_pCoreShell->GetGameData(GDI_IS_CHEST_UNLOCKED, 0, 0))
		m_BtnLock.CheckButton(0);
	else
		m_BtnLock.CheckButton(1);
}

void KUiStoreBox::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
{
	bool open = false;
	if (pItem)
	{
		if (bAdd == 2) {
			open = true;
			bAdd -= 1;
		}
		if(open)
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
				m_ItemBox.AddObject(&Obj, 1);
			else
				m_ItemBox.RemoveObject(&Obj);
			if(!open)
				UiSoundPlayItem(Obj.uId);
		}
		else
		{
			UiSoundPlay(UI_SI_PICKPUT_ITEM);
			m_nMoney = pItem->Obj.uId;
			//m_Money.SetIntText(m_nMoney);
			m_Money.SetMoneyText(m_nMoney);
		}
	}
	else
		UpdateData();
}
extern int SCREEN_WIDTH;
void KUiStoreBox::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_ITEM);
	if (m_pSelf && Ini.Load(Buff))
	{
		if (SCREEN_WIDTH == 1024)
			m_pSelf->Init(&Ini, "Main1024");
		else
			m_pSelf->Init(&Ini, "Main");

		
		m_pSelf->m_Money.Init(&Ini, "Money");
		m_pSelf->m_GetMoneyBtn.Init(&Ini, "GetMoneyBtn");
		m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");
		m_pSelf->m_ItemBox.Init(&Ini, "ItemBox");
		m_pSelf->m_BtnLock.Init(&Ini, "BtnLock");
		m_pSelf->m_ChangePWBtn.Init(&Ini, "BtnPassword");
		m_pSelf->m_BtnBox.Init(&Ini, "BtnBox");
		m_pSelf->m_ItemBox.EnableTracePutPos(true);
	}
}

int KUiStoreBox::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
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
				uPr[1] = pos_repositoryroom;
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
		else if (uParam == (unsigned int)(KWndWindow*)&m_GetMoneyBtn)
		{
			if (g_pCoreShell->GetGameData(GDI_IS_CHEST_UNLOCKED, 0, 0))
			{
				KUiGetMoney::OpenWindow(0, m_nMoney, this, UISTOREBOX_WAIT_GETMONEY, &m_Money);
			}
			else
			{
				g_pCoreShell->OperationRequest(GOI_PLAYER_ACTION, CN_GH, 0);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_BtnLock)
		{
			if (g_pCoreShell->GetGameData(GDI_IS_CHEST_UNLOCKED, 0, 0))
			{
				g_pCoreShell->OperationRequest(GOI_CP_LOCK, 0, 0);// Ruong dang mo - Khoa ruong lai
			}
			else
			{
				KUiUnlockBox::OpenWindow();// Ruong dang khoa ~> Mo cua so nhap pass ruong
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_ChangePWBtn)
		{
			if (g_pCoreShell->GetGameData(GDI_IS_CHEST_UNLOCKED, 0, 0))
			{
				KUiChangePWBox::OpenWindow();
			}
			else
			{
				g_pCoreShell->OperationRequest(GOI_PLAYER_ACTION, CN_GH, 0);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_BtnBox)
		{	
			g_pCoreShell->OperationRequest(GOI_PLAYER_ACTION, EX_BOX, 0);
		}
		break;
	case WND_M_OTHER_WORK_RESULT:
		if (uParam == UISTOREBOX_WAIT_GETMONEY)
		{
			if (g_pCoreShell->GetGameData(GDI_IS_CHEST_UNLOCKED, 0, 0))
			{
				OnGetMoney(nParam);
			}
			else
			{
				g_pCoreShell->OperationRequest(GOI_PLAYER_ACTION, CN_GH, 0);
			}
		}
		break;
	default:
		return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiStoreBox::OnGetMoney(int nMoney)
{
	if (nMoney > 0)
	{
		g_pCoreShell->OperationRequest(GOI_MONEY_INOUT_STORE_BOX, false, nMoney);
	}
}

void KUiStoreBox::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
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
		Pick.eContainer = UOC_STORE_BOX;
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
		Drop.eContainer = UOC_STORE_BOX;
	}
	
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
		pPickPos ? (unsigned int)&Pick : 0,
		pDropPos ? (int)&Drop : 0);
}