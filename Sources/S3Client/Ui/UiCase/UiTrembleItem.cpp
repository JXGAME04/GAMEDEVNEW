/*******************************************************************************
File        : UiTrembleItem.cpp
Creator     : Fong KiÒu
create data : 09-08-2020(mm-dd-yyyy)
Description : Kh¶m n¹m
********************************************************************************/

#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"
#include "GameDataDef.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"
#include "UiTrembleItem.h"
#include "UiSysMsgCentre.h"
#include "UiItem.h"

extern iCoreShell*		g_pCoreShell;

#define TREMBLE_ITEM_INI "UiTrembleItem.ini"
#define MAX_SPR_FRAME		25
#define MSG_NO_ITEM_EQUIP		"Ch­a ®Æt trang bÞ xanh vµo « kh¶m!"
#define MSG_NO_ITEM_GEM			"Ch­a ®Æt b¶o th¹ch vµo « kh¶m!"
//#define TREMBLE_SCRIPT_FILE		"tremble_item.lua"

KUiTrembleItem* KUiTrembleItem::ms_pSelf = NULL;

static struct UE_CTRL_MAP
{
	int				nPosition;
	const char*		pIniSection;
}

CtrlItemMap[_ITEM_TREMBLE_COUNT] =
{
	{ UIEP_BLUEITEM,	"Item"		},	
	{ UIEP_GEMLEVEL,	"GemLevel"	},
	{ UIEP_GEMSPIRIT,	"GemSpirit"	},	
	{ UIEP_GEMMETAL,	"GemGold"	},
	{ UIEP_GEMWOOD,		"GemWood"	},	
	{ UIEP_GEMWATER,	"GemWater"	},
	{ UIEP_GEMFIRE,		"GemFire"	},	
	{ UIEP_GEMEARTH,	"GemEarth"	},
};

KUiTrembleItem::KUiTrembleItem()
{
	m_nStatus = STATUS_WAITING_MATERIALS;
}

KUiTrembleItem::~KUiTrembleItem()
{

}
//------------------------------------------------
// Mo hop thoai
//------------------------------------------------
KUiTrembleItem* KUiTrembleItem::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiTrembleItem;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{	
		if(!KUiItem::GetIfVisible())
			KUiItem::OpenWindow();
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		ms_pSelf->m_nStatus = STATUS_WAITING_MATERIALS;
		ms_pSelf->m_TrembleEffect.Hide();
		ms_pSelf->UpdateData();
		ms_pSelf->BringToTop();
		ms_pSelf->Show();
		Wnd_GameSpaceHandleInput(false);
	}
	return ms_pSelf;
}
//----------------------------------------------
// Kiem tra xem hop thoai co mo hay khong
//----------------------------------------------
KUiTrembleItem* KUiTrembleItem::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}
//-----------------------------------------------
// Dong hop thoai
//-----------------------------------------------
void KUiTrembleItem::CloseWindow(bool bDestory)
{
	if (ms_pSelf)
	{
		ms_pSelf->Hide();
		if (bDestory)
		{
			ms_pSelf->Destroy();
			ms_pSelf = NULL;
		}
		g_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_tremble , 0); 
		//g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 5, (unsigned int)"OnCancel");
		Wnd_GameSpaceHandleInput(true);	
	}
}
//----------------------------------------------------
// Lam moi du lieu
//----------------------------------------------------
void KUiTrembleItem::UpdateData()
{
	UpdateAllItem();
}
//----------------------------------------------------
// Khoi tao hop thoai
//----------------------------------------------------
void KUiTrembleItem::Initialize()
{
	for (int i = 0; i < _ITEM_TREMBLE_COUNT; i ++)
	{
		m_ItemBox[i].SetObjectGenre(CGOG_ITEM);
		AddChild(&m_ItemBox[i]);
		m_ItemBox[i].SetContainerId((int)UOC_TREMBLE_ITEM);
	}
	AddChild(&m_Confirm);
	AddChild(&m_Cancel);
	AddChild(&m_TrembleEffect);
	
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}
//--------------------------------------------------
// Doc tap tin ini
//--------------------------------------------------
void KUiTrembleItem::LoadScheme(const char* pScheme)
{
	if(ms_pSelf)
	{
		char		Buff[128];
    	KIniFile	Ini;
    	sprintf(Buff, "%s\\%s", pScheme, TREMBLE_ITEM_INI);
    	if (Ini.Load(Buff))
    	{
			ms_pSelf->Init(&Ini, "Main");

			for (int i = 0; i < _ITEM_TREMBLE_COUNT; i ++)
			{
				ms_pSelf->m_ItemBox[i].Init(&Ini, CtrlItemMap[i].pIniSection);
			}
			ms_pSelf->m_TrembleEffect.Init(&Ini, "Effect");
			ms_pSelf->m_Confirm.Init(&Ini, "Assemble");
			ms_pSelf->m_Cancel.Init(&Ini, "Close");
		}
	}
}
//-------------------------------------------------
// Cac tuong tac hop thoai
//-------------------------------------------------
int KUiTrembleItem::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_Confirm)
		{	
			ProcessAssemble();		
		}
		else if(uParam == (unsigned int)&m_Cancel)
		{	
			CloseWindow();
		}
		break;
	case WND_N_ITEM_PICKDROP:
		if (g_UiBase.IsOperationEnable(UIS_O_MOVE_ITEM)) 
			OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		break;
	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return 1;
}
//----------------------------------------
// Ham bat dau kham
//----------------------------------------
void KUiTrembleItem::ApplyAssemble()
{
	m_nStatus = STATUS_BEGIN_TREMBLE;
}
//---------------------------------------------------------------------
// Ham xu ly kham
//---------------------------------------------------------------------
void KUiTrembleItem::ProcessAssemble()
{
	KUiDraggedObject pObj;
	BOOL bExistGem = false;
	
	pObj.uId = 0;
	m_ItemBox[UIEP_BLUEITEM].GetObject(pObj);
	if (pObj.uId <= 0)
	{
		KSystemMessage	Msg;
		Msg.byConfirmType = SMCT_NONE;
		Msg.eType = SMT_NORMAL;
		Msg.byPriority = 1;
		Msg.byParamSize = 0;
		strcpy(Msg.szMessage, MSG_NO_ITEM_EQUIP);
		KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
		return;
	}

	pObj.uId = 0;
	m_ItemBox[UIEP_GEMLEVEL].GetObject(pObj);
	if (pObj.uId > 0)
	{
		bExistGem = true;
	}

	pObj.uId = 0;
	m_ItemBox[UIEP_GEMSPIRIT].GetObject(pObj);
	if (pObj.uId > 0)
	{
		bExistGem = true;
	}

	pObj.uId = 0;
	m_ItemBox[UIEP_GEMMETAL].GetObject(pObj);
	if (pObj.uId > 0)
	{
		bExistGem = true;
	}

	pObj.uId = 0;
	m_ItemBox[UIEP_GEMWOOD].GetObject(pObj);
	if (pObj.uId > 0)
	{
		bExistGem = true;
	}

	pObj.uId = 0;
	m_ItemBox[UIEP_GEMWATER].GetObject(pObj);
	if (pObj.uId > 0)
	{
		bExistGem = true;
	}

	pObj.uId = 0;
	m_ItemBox[UIEP_GEMFIRE].GetObject(pObj);
	if (pObj.uId > 0)
	{
		bExistGem = true;
	}

	pObj.uId = 0;
	m_ItemBox[UIEP_GEMEARTH].GetObject(pObj);
	if (pObj.uId > 0)
	{
		bExistGem = true;
	}
	
	if(!bExistGem)
	{	
		KSystemMessage	Msg;
		Msg.byConfirmType = SMCT_NONE;
		Msg.eType = SMT_NORMAL;
		Msg.byPriority = 1;
		Msg.byParamSize = 0;
		strcpy(Msg.szMessage, MSG_NO_ITEM_GEM);
		KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
		return;
	}
	
	if(m_nStatus == STATUS_WAITING_MATERIALS)
	{	
		KSystemMessage	Msg;
		Msg.byConfirmType = SMCT_UI_ASKASSEMBLE;
		Msg.byParamSize = 0;
		Msg.byPriority = 255;
		Msg.eType = SMT_PLAYER;
		KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
		return;
	}

	//g_pCoreShell->OperationRequest(GOI_LOAD_BUTTON_SCRIPT, (unsigned int)TREMBLE_SCRIPT_FILE, (unsigned int)"TrembleItem"); 
	g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 5, (unsigned int)"TrembleItem");
}
//--------------------------------------------------------
// Ham chay thuc thi
//--------------------------------------------------------
void KUiTrembleItem::Breathe()
{	
	if(m_nStatus == STATUS_BEGIN_TREMBLE)
	{
		m_TrembleEffect.Show();
		m_TrembleEffect.SetFrame(0);
		m_nStatus = STATUS_TREMBLING;
	}
	else if(m_nStatus == STATUS_TREMBLING)
	{
		if(!PlayEffect())
		{
			m_nStatus = STATUS_CHANGING_ITEM;
			m_TrembleEffect.Hide();
		}
	}
	else if(m_nStatus == STATUS_CHANGING_ITEM)
	{	
		ProcessAssemble();
		m_nStatus = STATUS_WAITING_MATERIALS;
	}
}
//-----------------------------------------------------------
// Chay hieu ung kham
//-----------------------------------------------------------
int KUiTrembleItem::PlayEffect()
{
	if(m_TrembleEffect.GetCurrentFrame() >= MAX_SPR_FRAME)
	{
		m_TrembleEffect.SetFrame(0);
		return 0;
	}
	else
	{
		m_TrembleEffect.NextFrame();
		return 1;
	}
}

//------------------------------------------------
// Thay doi vat pham
//------------------------------------------------
void KUiTrembleItem::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	KUiObjAtContRegion	Drop, Pick;
	KUiDraggedObject	Obj;
	KWndWindow*			pWnd = NULL;

	UISYS_STATUS	eStatus = g_UiBase.GetStatus();
	if (pPickPos)
	{
		((KWndObjectBox*)(pPickPos->pWnd))->GetObject(Obj);
		Pick.Obj.uGenre = Obj.uGenre;
		Pick.Obj.uId = Obj.uId;
		Pick.Region.Width = Obj.DataW;
		Pick.Region.Height = Obj.DataH;
		Pick.Region.h = 0;
		Pick.eContainer = UOC_TREMBLE_ITEM;
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
		Drop.Region.h = 0;
		Drop.eContainer = UOC_TREMBLE_ITEM;
	}

	for (int i = 0; i < _ITEM_TREMBLE_COUNT; i++)
	{
		if (pWnd == (KWndWindow*)&m_ItemBox[i])
		{
			Drop.Region.v = Pick.Region.v = CtrlItemMap[i].nPosition;
			break;
		}
	}
		g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
		pPickPos ? (unsigned int)&Pick : 0,
		pDropPos ? (int)&Drop : 0);

}
//-------------------------------------
// Lam moi tat ca vat pham
//-------------------------------------
void KUiTrembleItem::UpdateAllItem()
{
	KUiObjAtRegion	Item[_ITEM_TREMBLE_COUNT];
	int nCount = g_pCoreShell->GetGameData(GDI_TREMBLE_ITEM, (unsigned int)&Item, 0);
	int	i;
	for (i = 0; i < _ITEM_TREMBLE_COUNT; i++)
		m_ItemBox[i].Celar();
	for (i = 0; i < nCount; i++)
	{
		if (Item[i].Obj.uGenre != CGOG_NOTHING)
			UpdateItem(&Item[i], true);
	}
}
//----------------------------------------------------------------
// Lam moi tung vat pham
//----------------------------------------------------------------
void KUiTrembleItem::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		for (int i = 0; i < _ITEM_TREMBLE_COUNT; i++)
		{
			if (CtrlItemMap[i].nPosition == pItem->Region.v)
			{
				if (bAdd)
					m_ItemBox[i].HoldObject(pItem->Obj.uGenre, pItem->Obj.uId,
						pItem->Region.Width, pItem->Region.Height);
				else
					m_ItemBox[i].HoldObject(CGOG_NOTHING, 0, 0, 0);
				break;
			}
		}
	}
}