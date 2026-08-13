/*****************************************************************************************
// Hop thoai che tao trang bi tim
// Author: Fong KiÒu
// Date: 2021
------------------------------------------------------------------------------------------
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../elem/wnds.h"
#include "../Elem/PopupMenu.h"
#include "KEngine.h"
#include "UiItem.h"
#include "UiCompoundItem.h"
#include "UiSysMsgCentre.h"
#include "../../../core/src/coreshell.h"
#include "../../../Engine/src/Text.h"
#include "../UiBase.h"
#include "crtdbg.h"
#include "../UiSoundSetting.h"

#include "../../../Represent/iRepresent/iRepresentShell.h"
extern iRepresentShell*	g_pRepresentShell;

extern iCoreShell*		g_pCoreShell;

KUiCompoundItem* KUiCompoundItem::m_pSelf = NULL;

#define 	SCHEME_INI_SHEET			"khamnam/Khamnammain.ini"
#define 	SCHEME_INI_COMPE			"khamnam/Tinhluyen.ini"
#define 	SCHEME_INI_BUILD			"khamnam/Chetao.ini"
#define 	SCHEME_INI_OUTIN			"khamnam/Lay.ini"
#define		SEL_COM_MENU				1
#define		MAX_SPR_FRAME				25
#define		CASH_COMPOUND				5000			
#define		COMPOUND_SCRIPT_FILE		"compound_item.lua"
#define		CASH_FORGE					10000			
#define		FORGE_SCRIPT_FILE			"forge_item.lua"
#define		DISTILL_SCRIPT_FILE			"distill_item.lua"
#define		ENCHASE_SCRIPT_FILE			"enchase_item.lua"
KUiCompoundItem				g_ComItem;

static struct UE_CTRL_MAP
{
	int				nPosition;
	const char*		pIniSection;
}

CtrlCompOneItemMap[_ITEM_COMP_COUNT] =
{
	{ UIEP_BOX1,		"Ore1"	},	
	{ UIEP_BOX2,		"Ore2"	},
	{ UIEP_BOX3,		"Ore3"	},	
}, 

CtrlCompTwoItemMap[_ITEM_COMP_COUNT] =
{
	{ UIEP_BOX1,	"Ore1"	},	
	{ UIEP_BOX2,	"Ore2"	},
	{ UIEP_BOX3,	"Ore3"	},	
},

CtrlCompThreeItemMap[_ITEM_COMP_COUNT] =
{
	{ UIEP_BOX1,	"Ore1"	},	
	{ UIEP_BOX2,	"Ore2"	},
	{ UIEP_BOX3,	"Ore3"	},	
},


CtrlDistillItemMap[_ITEM_OUTIN_COUNT] =
{
	{ UIEP_ITEM_,		"BigBox"	},	
	{ UIEP_CRYOLITE,	"SmallBox1"	},
	{ UIEP_PROPMINE,	"SmallBox2"	},	
	{ UIEP_CONS1,		"Consume1"	},	
	{ UIEP_CONS2,		"Consume2"	},	
	{ UIEP_CONS3,		"Consume3"	},
	{ UIEP_CONS4,		"Consume4"	},	
	{ UIEP_CONS5,		"Consume5"	},
	{ UIEP_CONS6,		"Consume6"	},	
	{ UIEP_CONS7,		"Consume7"	},
	{ UIEP_CONS8,		"Consume8"	},	
},

CtrlForgeItemMap[_ITEM_FORGE_COUNT] =
{
	{ UIEP_ITEMFOR,		"BigBox"	},	
	{ UIEP_CRYOLITEFOR,	"SmallBox"	},
},

CtrlEnchaseItemMap[_ITEM_OUTIN_COUNT] =
{
	{ UIEP_ITEM_,		"BigBox"	},	
	{ UIEP_CRYOLITE,	"SmallBox1"	},
	{ UIEP_PROPMINE,	"SmallBox2"	},	
	{ UIEP_CONS1,		"Consume1"	},	
	{ UIEP_CONS2,		"Consume2"	},	
	{ UIEP_CONS3,		"Consume3"	},
	{ UIEP_CONS4,		"Consume4"	},	
	{ UIEP_CONS5,		"Consume5"	},
	{ UIEP_CONS6,		"Consume6"	},	
	{ UIEP_CONS7,		"Consume7"	},
	{ UIEP_CONS8,		"Consume8"	},	
};
//************************************************* GIAO DIEN CHINH ************************************************
//-----------------------------------------
// Kiem tra hop thoai co dang mo hay khong
//-----------------------------------------
KUiCompoundItem* KUiCompoundItem::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}
//--------------------------------------
// Mo hop thoai
//--------------------------------------
KUiCompoundItem* KUiCompoundItem::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiCompoundItem;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->m_CompoundPadBtn.CheckButton(TRUE);
		m_pSelf->m_DistillPadBtn.CheckButton(FALSE);
		m_pSelf->m_AtlasPadBtn.CheckButton(FALSE);
		m_pSelf->m_EnchasePadBtn.CheckButton(FALSE);
		m_pSelf->m_ForgePadBtn.CheckButton(FALSE);
		m_pSelf->m_CompoundOnePad.UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();

		if (KUiItem::GetIfVisible() == NULL)
			KUiItem::OpenWindow();
		else
			UiSoundPlay(UI_SI_WND_OPENCLOSE);
		//Wnd_GameSpaceHandleInput(false);
	}
	return m_pSelf;
}
//-------------------------------------------------
// Dong hop thoai
//-------------------------------------------------
void KUiCompoundItem::CloseWindow(bool bDestory)
{
	if (m_pSelf)
	{	
		switch(m_pSelf->nWindow)
		{	
			case WINDOWS_COMP:
				//g_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_compone, 0); 
				break;
			case WINDOWS_COMP2:
				//g_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_comptwo, 0); 
				break;
			case WINDOWS_COMP3:
				//g_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_compthree, 0); 
				break;
			case WINDOWS_DISTILL:
				//g_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_distill, 0);
				break;
			case WINDOWS_FORGE:
				//g_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_forge, 0); 
				break;
			case WINDOWS_ENCHASE:
				//g_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_enchase, 0); 
				break;
			default:
				break;
		}

		m_pSelf->Hide();
		if (bDestory)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}

		Wnd_GameSpaceHandleInput(true);
	}
}
//-------------------------------------------
// Khoi tao cac bien
//-------------------------------------------
void KUiCompoundItem::Initialize()
{
	AddChild(&m_CompoundPadBtn);
	AddChild(&m_DistillPadBtn);
	AddChild(&m_EnchasePadBtn);
	AddChild(&m_ForgePadBtn);
	AddChild(&m_AtlasPadBtn);

	AddChild(&m_Close);
	
	m_CompoundOnePad.Initialize();
	AddPage(&m_CompoundOnePad,&m_CompoundPadBtn);

	m_CompoundTwoPad.Initialize();
	AddPage(&m_CompoundTwoPad,&m_CompoundPadBtn);

	m_CompoundThreePad.Initialize();
	AddPage(&m_CompoundThreePad,&m_CompoundPadBtn);


	m_DistillPad.Initialize();
	AddPage(&m_DistillPad,&m_DistillPadBtn);

	m_ForgePad.Initialize();
	AddPage(&m_ForgePad,&m_ForgePadBtn);

	m_EnchasePad.Initialize();
	AddPage(&m_EnchasePad,&m_EnchasePadBtn);

	//m_AtlasPad.Initialize();
	//AddPage(&m_AtlasPad,&m_AtlasPadBtn);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	
	Wnd_AddWindow(this);
}
//----------------------------------------------------
// Doc tap tin ini
//----------------------------------------------------
void KUiCompoundItem::LoadScheme( const char* pScheme )
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_SHEET);
	if (m_pSelf && Ini.Load(Buff))
	{
		m_pSelf->Init(&Ini, "Main");
		m_pSelf->m_CompoundPadBtn.Init(&Ini, "CompoundBtn");
		m_pSelf->m_DistillPadBtn.Init(&Ini, "DistillBtn");
		m_pSelf->m_ForgePadBtn.Init(&Ini, "ForgeBtn");
		m_pSelf->m_EnchasePadBtn.Init(&Ini, "EnchaseBtn");
		m_pSelf->m_AtlasPadBtn.Init(&Ini, "AtlasBtn");
		m_pSelf->m_Close           .Init(&Ini, "CloseBtn");
		
		m_pSelf->m_CompoundOnePad.LoadScheme(pScheme);
		m_pSelf->nWindow = WINDOWS_COMP;
	}	
}
//--------------------------------------------------------
// Ham chay thuc thi
//--------------------------------------------------------
void KUiCompoundItem::Breathe()
{	
	switch(m_pSelf->nWindow)
	{	
		case WINDOWS_COMP:
			m_CompoundOnePad.Breathe();
			break;
		case WINDOWS_COMP2:
			m_CompoundTwoPad.Breathe();
			break;
		case WINDOWS_COMP3:
			m_CompoundThreePad.Breathe();
			break;
		case WINDOWS_DISTILL:
			m_DistillPad.Breathe();
			break;
		case WINDOWS_FORGE:
			m_ForgePad.Breathe();
			break;
		case WINDOWS_ENCHASE:
			m_EnchasePad.Breathe();
			break;
		default:
			break;
	}
}
//------------------------------------------------------------------------------
// Tuong tac hop thoai
//------------------------------------------------------------------------------
int KUiCompoundItem::WndProc( unsigned int uMsg, unsigned int uParam, int nParam )
{
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_Close)
		{
			CloseWindow();
		}
		else if(uParam == (unsigned int)&m_CompoundPadBtn)
		{
			PopUpContextComMenu();
		}
		else if (uParam == (unsigned int)&m_DistillPadBtn)
		{
			ShowWindow(WINDOWS_DISTILL);
			m_pSelf->nWindow = WINDOWS_DISTILL;
		}
		else if (uParam == (unsigned int)&m_ForgePadBtn)
		{
			ShowWindow(WINDOWS_FORGE);
			m_pSelf->nWindow = WINDOWS_FORGE;
		}
		else if (uParam == (unsigned int)&m_EnchasePadBtn)
		{
		
			ShowWindow(WINDOWS_ENCHASE);
			m_pSelf->nWindow = WINDOWS_ENCHASE;
		}
		else if (uParam == (unsigned int)&m_AtlasPadBtn)
		{
			ShowWindow(WINDOWS_ATLAS);
			m_pSelf->nWindow = WINDOWS_ATLAS;
		}
		break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if (HIWORD(nParam) == SEL_COM_MENU && short(LOWORD(nParam)) >= 0)
			{	
				ProcessCom(LOWORD(nParam));
			}
		}
		break;
		
	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return 1;
}
//--------------------------------------------------
// Hien lua chon
//--------------------------------------------------
char g_WindowComName[][64] = 
{
	"LuyÖn huyÒn tinh kho¸ng th¹ch",
	"Th¨ng cÊp huyÒn tinh kho¸ng th¹ch",
	"Th¨ng cÊp kho¸ng th¹ch thuéc tÝnh",
};

void KUiCompoundItem::PopUpContextComMenu()
{
	int nActionDataCount = sizeof(g_WindowComName) / 32;
	KPopupMenuData* pMenuData = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pMenuData == NULL)
		return;
	KPopupMenu::InitMenuData(pMenuData, nActionDataCount);
	pMenuData->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	pMenuData->usMenuFlag &= ~PM_F_HAVE_HEAD_TAIL_IMG;

	int nMenuCount = 0;
	int i;
	pMenuData->nItemHeight = 0;
	for (i = 0; i < nActionDataCount; i++)
	{
		if (i == CREATE_CRYOLITE || i == UP_CRYOLITE || i == UP_PROPMINE)
		{
			strcpy(pMenuData->Items[nMenuCount].szData, g_WindowComName[i]);
			pMenuData->Items[nMenuCount].uDataLen = strlen(g_WindowComName[i]);
			pMenuData->Items[nMenuCount].uID = i;
			nMenuCount++;
		}
	}
	pMenuData->nNumItem = nMenuCount;

	pMenuData->nX = 151;
	pMenuData->nY = 142;
	KPopupMenu::Popup(pMenuData, &g_ComItem, SEL_COM_MENU);
}
//--------------------------------------------------
// Xu ly lua chon
//--------------------------------------------------
void KUiCompoundItem::ProcessCom(int nWindow)
{
	switch(nWindow)
	{
		case CREATE_CRYOLITE:
			ShowWindow(WINDOWS_COMP);
			m_pSelf->nWindow = WINDOWS_COMP;
			break;
		case UP_CRYOLITE:
			ShowWindow(WINDOWS_COMP2);
			m_pSelf->nWindow = WINDOWS_COMP2;
			break;
		case UP_PROPMINE:
			ShowWindow(WINDOWS_COMP3);
			m_pSelf->nWindow = WINDOWS_COMP3;
			break;
		default:
			break;
	}
}
//--------------------------------------------------
// Hien thi hop thoai
//--------------------------------------------------
void KUiCompoundItem::ShowWindow(int nNum /*= 0*/)
{
	switch(nNum)
	{
	case 0:
		m_pSelf->m_CompoundOnePad.Show();
		m_pSelf->m_CompoundPadBtn.CheckButton(TRUE);
		m_pSelf->m_CompoundTwoPad.Hide();
		m_pSelf->m_CompoundThreePad.Hide();
		m_pSelf->m_DistillPad.Hide();
		m_pSelf->m_DistillPadBtn.CheckButton(FALSE);
		m_pSelf->m_ForgePad.Hide();
		m_pSelf->m_ForgePadBtn.CheckButton(FALSE);
		m_pSelf->m_EnchasePad.Hide();
		m_pSelf->m_EnchasePadBtn.CheckButton(FALSE);
//		m_pSelf->m_AtlasPad.Hide();
//		m_pSelf->m_AtlasPadBtn.CheckButton(FALSE);
		m_pSelf->m_CompoundOnePad.UpdateData();
		break;
	case 1:
		m_pSelf->m_CompoundTwoPad.Show();
		m_pSelf->m_CompoundPadBtn.CheckButton(TRUE);
		m_pSelf->m_CompoundOnePad.Hide();
		m_pSelf->m_CompoundThreePad.Hide();
		m_pSelf->m_DistillPad.Hide();
		m_pSelf->m_DistillPadBtn.CheckButton(FALSE);
		m_pSelf->m_ForgePad.Hide();
		m_pSelf->m_ForgePadBtn.CheckButton(FALSE);
		m_pSelf->m_EnchasePad.Hide();
		m_pSelf->m_EnchasePadBtn.CheckButton(FALSE);
//		m_pSelf->m_AtlasPad.Hide();
//		m_pSelf->m_AtlasPadBtn.CheckButton(FALSE);
		m_pSelf->m_CompoundTwoPad.UpdateData();
		break;
	case 2:
		m_pSelf->m_CompoundThreePad.Show();
		m_pSelf->m_CompoundPadBtn.CheckButton(TRUE);
		m_pSelf->m_CompoundOnePad.Hide();
		m_pSelf->m_CompoundTwoPad.Hide();
		m_pSelf->m_DistillPad.Hide();
		m_pSelf->m_DistillPadBtn.CheckButton(FALSE);
		m_pSelf->m_ForgePad.Hide();
		m_pSelf->m_ForgePadBtn.CheckButton(FALSE);
		m_pSelf->m_EnchasePad.Hide();
		m_pSelf->m_EnchasePadBtn.CheckButton(FALSE);
//		m_pSelf->m_AtlasPad.Hide();
//		m_pSelf->m_AtlasPadBtn.CheckButton(FALSE);
		m_pSelf->m_CompoundThreePad.UpdateData();
		break;
	case 3:
		m_pSelf->m_DistillPad.Show();
		m_pSelf->m_DistillPadBtn.CheckButton(TRUE);
		m_pSelf->m_CompoundOnePad.Hide();
		m_pSelf->m_CompoundTwoPad.Hide();
		m_pSelf->m_CompoundThreePad.Hide();
		m_pSelf->m_CompoundPadBtn.CheckButton(FALSE);
		m_pSelf->m_ForgePad.Hide();
		m_pSelf->m_ForgePadBtn.CheckButton(FALSE);
		m_pSelf->m_EnchasePad.Hide();
		m_pSelf->m_EnchasePadBtn.CheckButton(FALSE);
//		m_pSelf->m_AtlasPad.Hide();
//		m_pSelf->m_AtlasPadBtn.CheckButton(FALSE);
		m_pSelf->m_DistillPad.UpdateData();
		break;
	case 4:
		m_pSelf->m_ForgePad.Show();
		m_pSelf->m_ForgePadBtn.CheckButton(TRUE);
		m_pSelf->m_CompoundOnePad.Hide();
		m_pSelf->m_CompoundTwoPad.Hide();
		m_pSelf->m_CompoundThreePad.Hide();
		m_pSelf->m_CompoundPadBtn.CheckButton(FALSE);
		m_pSelf->m_DistillPad.Hide();
		m_pSelf->m_DistillPadBtn.CheckButton(FALSE);
		m_pSelf->m_EnchasePad.Hide();
		m_pSelf->m_EnchasePadBtn.CheckButton(FALSE);
//		m_pSelf->m_AtlasPad.Hide();
//		m_pSelf->m_AtlasPadBtn.CheckButton(FALSE);
		m_pSelf->m_ForgePad.UpdateData();
		break;
	case 5:
		m_pSelf->m_EnchasePad.Show();
		m_pSelf->m_EnchasePadBtn.CheckButton(TRUE);
		m_pSelf->m_CompoundOnePad.Hide();
		m_pSelf->m_CompoundTwoPad.Hide();
		m_pSelf->m_CompoundThreePad.Hide();
		m_pSelf->m_CompoundPadBtn.CheckButton(FALSE);
		m_pSelf->m_DistillPad.Hide();
		m_pSelf->m_DistillPadBtn.CheckButton(FALSE);
		m_pSelf->m_ForgePad.Hide();
		m_pSelf->m_ForgePadBtn.CheckButton(FALSE);
		m_pSelf->m_EnchasePad.UpdateData();
		break;
	case 6:
//		m_pSelf->m_AtlasPad.Show();
//		m_pSelf->m_AtlasPadBtn.CheckButton(TRUE);
		m_pSelf->m_CompoundOnePad.Hide();
		m_pSelf->m_CompoundTwoPad.Hide();
		m_pSelf->m_CompoundThreePad.Hide();
		m_pSelf->m_CompoundPadBtn.CheckButton(FALSE);
		m_pSelf->m_DistillPad.Hide();
		m_pSelf->m_DistillPadBtn.CheckButton(FALSE);
		m_pSelf->m_ForgePad.Hide();
		m_pSelf->m_ForgePadBtn.CheckButton(FALSE);
		m_pSelf->m_EnchasePad.Hide();
		m_pSelf->m_EnchasePadBtn.CheckButton(FALSE);
		break;
	}
}
//-------------------------------------------------------
// Lam moi vat pham
//-------------------------------------------------------
void KUiCompoundItem::UpdateItem(KUiObjAtRegion *pItem, int bAdd)
{	
	KUiObjAtContRegion* pObject = (KUiObjAtContRegion*)pItem;
	switch(pObject->eContainer)
	{
		case UOC_COMPONE_ITEM:
			{
				m_pSelf->m_CompoundOnePad.UpdateItem(pItem, bAdd);
			}
			break;
		case UOC_COMPTWO_ITEM:
			{
				m_pSelf->m_CompoundTwoPad.UpdateItem(pItem, bAdd);
			}
			break;
		case UOC_COMPTHREE_ITEM:
			{
				m_pSelf->m_CompoundThreePad.UpdateItem(pItem, bAdd);
			}
			break;
		case UOC_DISTILL_ITEM:
			{
				m_pSelf->m_DistillPad.UpdateItem(pItem, bAdd);
			}
			break;
		case UOC_FORGE_ITEM:
			{
				m_pSelf->m_ForgePad.UpdateItem(pItem, bAdd);
			}
			break;
		case UOC_ENCHASE_ITEM:
			{
				m_pSelf->m_EnchasePad.UpdateItem(pItem, bAdd);
			}
			break;
		default:
			break;
	}

}
//****************************************************** CHE TAO HUYEN TINH ******************************************

//--------------------------
// Khoi tao hop thoai
//--------------------------
KUiCompoundOne::KUiCompoundOne()
{
	m_nStatus = STATUS_WAITING_MATERIALS;
}
//--------------------------------
// Khoi tao cac bien
//--------------------------------
void KUiCompoundOne::Initialize()
{	
	for (int i = 0; i < _ITEM_COMP_COUNT; i ++)
	{
		m_ItemBox[i].SetObjectGenre(CGOG_ITEM);
		AddChild(&m_ItemBox[i]);
		m_ItemBox[i].SetContainerId((int)UOC_COMPONE_ITEM);
	}

	AddChild(&m_Compound);
	AddChild(&m_Cancel);

	AddChild(&m_Guide);
	AddChild(&m_ListScroll);
	 
	AddChild(&m_Pos1);
	AddChild(&m_Pos2);
	AddChild(&m_Pos3);

	AddChild(&m_CompEffect1);
	AddChild(&m_CompEffect2);
	AddChild(&m_CompEffect3);

	m_Guide.SetScrollbar(&m_ListScroll);

	
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	
	char		Buff[512];
	KIniFile	Ini;
	
	sprintf(Buff, "%s\\%s", Scheme, SCHEME_INI_SHEET);
	if (Ini.Load(Buff))
	{	
		int nLen = sizeof(Buff);

		ZeroMemory(Buff, nLen);
		Ini.GetString("RuleInfo","Compound","",Buff, nLen);
		
		nLen = TEncodeText(Buff, strlen(Buff));

		m_Guide.AddOneMessage(Buff, nLen);

		ZeroMemory(Buff, nLen);

	}

	Wnd_AddWindow(this);
}
//---------------------------------------------------
// Doc tap tin ini
//---------------------------------------------------
void KUiCompoundOne::LoadScheme( const char* pScheme )
{
	char		Buff[128],Buffer[64];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_COMPE);
	if (Ini.Load(Buff))
	{
		KWndImage::Init(&Ini, "Main");
		for (int i = 0; i < _ITEM_COMP_COUNT; i ++)
		{
			m_ItemBox[i].Init(&Ini, CtrlCompOneItemMap[i].pIniSection);
		}
		m_Compound.Init(&Ini,"CompoundBtn");
		m_Cancel.Init(&Ini,"CancleBtn");
		m_Guide.Init(&Ini,"GuideList");
		m_ListScroll.Init(&Ini,"GuideList_Scroll");

		int nX,nY,nColor;
		Ini.GetInteger2("Box_0","Pos",&nX,&nY);

		if (Ini.GetString("TextColor", "Font", "", Buffer, sizeof(Buffer)))
		{
			nColor = (::GetColor(Buffer) & 0xFFFFFF);
		}
		m_Pos1.SetPosition(nX - 14, nY - 4);
		m_Pos1.SetText("NhÉn");
		m_Pos1.SetTextColor(nColor);
		m_Pos1.BringToTop();

		Ini.GetInteger2("Box_1","Pos",&nX,&nY);

		m_Pos2.SetPosition(nX - 48, nY - 4);
		m_Pos2.SetText("D©y chuyÒn/Hé phï");
		m_Pos2.SetTextColor(nColor);
		m_Pos2.BringToTop();

		Ini.GetInteger2("Box_2","Pos",&nX,&nY);
		
		m_Pos3.SetPosition(nX - 48, nY - 4);
		m_Pos3.SetText("Ngäc béi/H­¬ng nang");
		m_Pos3.SetTextColor(nColor);
		m_Pos3.BringToTop();

		m_CompEffect1.Init(&Ini,"Effect_0");
		m_CompEffect2.Init(&Ini,"Effect_1");
		m_CompEffect3.Init(&Ini,"Effect_2");

		m_CompEffect1.Hide();
		m_CompEffect2.Hide();
		m_CompEffect3.Hide();
	}	
}
//--------------------------------------------------------
// Ham chay thuc thi
//--------------------------------------------------------
void KUiCompoundOne::Breathe()
{	
	if(m_nStatus == STATUS_BEGIN_COMPOUND)
	{
		m_CompEffect1.Show();
		m_CompEffect1.SetFrame(0);
		m_CompEffect2.Show();
		m_CompEffect2.SetFrame(0);
		m_CompEffect3.Show();
		m_CompEffect3.SetFrame(0);
		m_nStatus = STATUS_COMPOUNDING;
	}
	else if(m_nStatus == STATUS_COMPOUNDING)
	{
		if(!PlayEffect())
		{
			m_nStatus = STATUS_CHANGING_ITEM;
			m_CompEffect1.Hide();
			m_CompEffect2.Hide();
			m_CompEffect3.Hide();
		}
	}
	else if(m_nStatus == STATUS_CHANGING_ITEM)
	{	
		ProcessComp();
		m_nStatus = STATUS_WAITING_MATERIALS;
	}
}
//-----------------------------------------------------------
// Chay hieu ung kham
//-----------------------------------------------------------
int KUiCompoundOne::PlayEffect()
{
	/*if(m_CompEffect1.GetCurrentFrame() >= MAX_SPR_FRAME)
	{
		m_CompEffect1.SetFrame(0);
		m_CompEffect2.SetFrame(0);
		m_CompEffect3.SetFrame(0);
		return 0;
	}
	else
	{
		m_CompEffect1.NextFrame();
		m_CompEffect2.NextFrame();
		m_CompEffect3.NextFrame();
		return 1;
	}*/
	return 1;
}
//-----------------------------------------------------------------------------
// Tuong tac hop thoai
//-----------------------------------------------------------------------------
int KUiCompoundOne::WndProc( unsigned int uMsg, unsigned int uParam, int nParam )
{
	switch(uMsg)
	{
	case WND_N_SCORLLBAR_POS_CHANGED:
		if(uParam == (unsigned int)&m_ListScroll)
		{
			m_Guide.SetFirstShowLine(nParam);
		}
		break;
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_Cancel)
		{
			OnCancel();
		}
		else if(uParam == (unsigned int)&m_Compound)
		{
			ProcessComp();
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
//------------------------------------
// Tinh luyen
//------------------------------------
void KUiCompoundOne::ProcessComp()
{
	KUiDraggedObject pObj;
	int nMoneyHold = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);

	memset(&pObj, 0, sizeof(pObj));
	m_ItemBox[UIEP_BOX1].GetObject(pObj);
	if (pObj.uId <= 0)
	{	
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","1","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}

	memset(&pObj, 0, sizeof(pObj));
	m_ItemBox[UIEP_BOX2].GetObject(pObj);
	if (pObj.uId <= 0)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","2","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}

	memset(&pObj, 0, sizeof(pObj));
	m_ItemBox[UIEP_BOX3].GetObject(pObj);
	if (pObj.uId <= 0)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","3","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}

	if(nMoneyHold < CASH_COMPOUND)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","4","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}

	if(m_nStatus == STATUS_WAITING_MATERIALS)
	{	
		m_nStatus = STATUS_BEGIN_COMPOUND;
		return;
	}
	
	//g_pCoreShell->OperationRequest(GOI_LOAD_BUTTON_SCRIPT, (unsigned int)COMPOUND_SCRIPT_FILE, (unsigned int)"CompOneItem");
	
	char		Buff[64];
	KIniFile	Ini;
	
	sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
	if (Ini.Load(Buff))
	{
		ZeroMemory(Buff,sizeof(Buff));
		Ini.GetString("ReturnInfo","5","",Buff,sizeof(Buff));
		
		m_Guide.Clear();
		m_Guide.AddOneMessage(Buff,sizeof(Buff));
	}
	memset(&pObj, 0, sizeof(pObj));
	nMoneyHold = 0;
	ZeroMemory(Buff,sizeof(Buff));
	return;
}
//------------------------------------
// Huy bo
//------------------------------------
void KUiCompoundOne::OnCancel()
{	
	if(g_pCoreShell)
	{
		//g_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_compone, 0); 
	}
}
//------------------------------------------------
// Thay doi vat pham
//------------------------------------------------
void KUiCompoundOne::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
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
		Pick.eContainer = UOC_COMPONE_ITEM;
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
		Drop.eContainer = UOC_COMPONE_ITEM;
	}

	for (int i = 0; i < _ITEM_COMP_COUNT; i++)
	{
		if (pWnd == (KWndWindow*)&m_ItemBox[i])
		{
			Drop.Region.v = Pick.Region.v = CtrlCompOneItemMap[i].nPosition;
			break;
		}
	}
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
	pPickPos ? (unsigned int)&Pick : 0,
	pDropPos ? (int)&Drop : 0);

}
//----------------------------------------------------
// Lam moi du lieu
//----------------------------------------------------
void KUiCompoundOne::UpdateData()
{
	UpdateAllItem();
	
}
//-------------------------------------
// Lam moi tat ca vat pham
//-------------------------------------
void KUiCompoundOne::UpdateAllItem()
{
	/*KUiObjAtRegion	Item[_ITEM_COMP_COUNT];
	int nCount = g_pCoreShell->GetGameData(GDI_COMPONE_ITEM, (unsigned int)&Item, 0);
	int	i;
	for (i = 0; i < _ITEM_COMP_COUNT; i++)
		m_ItemBox[i].Celar();
	for (i = 0; i < nCount; i++)
	{
		if (Item[i].Obj.uGenre != CGOG_NOTHING)
			UpdateItem(&Item[i], true);
	}*/
}
//----------------------------------------------------------------
// Lam moi tung vat pham
//----------------------------------------------------------------
void KUiCompoundOne::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		for (int i = 0; i < _ITEM_COMP_COUNT; i++)
		{
			if (CtrlCompOneItemMap[i].nPosition == pItem->Region.v)
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

//--------------------------------
// Ve hop thoai
//--------------------------------
void KUiCompoundOne::PaintWindow()
{
	KWndPage::PaintWindow();
}
//****************************************************** NANG CAP HUYEN TINH ******************************************

//--------------------------
// Khoi tao hop thoai
//--------------------------
KUiCompoundTwo::KUiCompoundTwo()
{
	m_nStatus = STATUS_WAITING_MATERIALS;
}
//--------------------------------
// Khoi tao cac bien
//--------------------------------
void KUiCompoundTwo::Initialize()
{
	for (int i = 0; i < _ITEM_COMP_COUNT; i ++)
	{
		m_ItemBox[i].SetObjectGenre(CGOG_ITEM);
		AddChild(&m_ItemBox[i]);
		m_ItemBox[i].SetContainerId((int)UOC_COMPTWO_ITEM);
	}
	AddChild(&m_Compound);
	AddChild(&m_Cancel);

	AddChild(&m_Guide);
	AddChild(&m_ListScroll);
	 
	AddChild(&m_Pos1);
	AddChild(&m_Pos2);
	AddChild(&m_Pos3);

	AddChild(&m_CompEffect1);
	AddChild(&m_CompEffect2);
	AddChild(&m_CompEffect3);

	m_Guide.SetScrollbar(&m_ListScroll);

	
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	
	char		Buff[512];
	KIniFile	Ini;
	
	sprintf(Buff, "%s\\%s", Scheme, SCHEME_INI_SHEET);
	if (Ini.Load(Buff))
	{
		int nLen = sizeof(Buff);

		ZeroMemory(Buff, nLen);
		Ini.GetString("RuleInfo","UpCryolite","",Buff, nLen);
		
		nLen = TEncodeText(Buff, strlen(Buff));

		m_Guide.AddOneMessage(Buff, nLen);

		ZeroMemory(Buff, nLen);

	}

	Wnd_AddWindow(this);
}
//---------------------------------------------------
// Doc tap tin ini
//---------------------------------------------------
void KUiCompoundTwo::LoadScheme( const char* pScheme )
{
	char		Buff[128],Buffer[64];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_COMPE);
	if (Ini.Load(Buff))
	{
		KWndImage::Init(&Ini, "Main");
		for (int i = 0; i < _ITEM_COMP_COUNT; i ++)
		{
			m_ItemBox[i].Init(&Ini, CtrlCompTwoItemMap[i].pIniSection);
		}
		m_Compound.Init(&Ini,"CompoundBtn");
		m_Cancel.Init(&Ini,"CancleBtn");
		m_Guide.Init(&Ini,"GuideList");
		m_ListScroll.Init(&Ini,"GuideList_Scroll");

		int nX,nY,nColor;
		Ini.GetInteger2("Box_0","Pos",&nX,&nY);

		if (Ini.GetString("TextColor", "Font", "", Buffer, sizeof(Buffer)))
		{
			nColor = (::GetColor(Buffer) & 0xFFFFFF);
		}

		m_Pos1.SetPosition(nX - 30, nY - 4);
		m_Pos1.SetText("HuyÒn tinh 1");
		m_Pos1.SetTextColor(nColor);
		m_Pos1.BringToTop();

		Ini.GetInteger2("Box_1","Pos",&nX,&nY);

		m_Pos2.SetPosition(nX - 30, nY - 4);
		m_Pos2.SetText("HuyÒn tinh 2");
		m_Pos2.SetTextColor(nColor);
		m_Pos2.BringToTop();

		Ini.GetInteger2("Box_2","Pos",&nX,&nY);
		
		m_Pos3.SetPosition(nX - 30, nY - 4);
		m_Pos3.SetText("HuyÒn tinh 3");
		m_Pos3.SetTextColor(nColor);
		m_Pos3.BringToTop();

		m_CompEffect1.Init(&Ini,"Effect_0");
		m_CompEffect2.Init(&Ini,"Effect_1");
		m_CompEffect3.Init(&Ini,"Effect_2");

		m_CompEffect1.Hide();
		m_CompEffect2.Hide();
		m_CompEffect3.Hide();
		
	}	
}
//--------------------------------------------------------
// Ham chay thuc thi
//--------------------------------------------------------
void KUiCompoundTwo::Breathe()
{	
	if(m_nStatus == STATUS_BEGIN_COMPOUND)
	{
		m_CompEffect1.Show();
		m_CompEffect1.SetFrame(0);
		m_CompEffect2.Show();
		m_CompEffect2.SetFrame(0);
		m_CompEffect3.Show();
		m_CompEffect3.SetFrame(0);
		m_nStatus = STATUS_COMPOUNDING;
	}
	else if(m_nStatus == STATUS_COMPOUNDING)
	{
		if(!PlayEffect())
		{
			m_nStatus = STATUS_CHANGING_ITEM;
			m_CompEffect1.Hide();
			m_CompEffect2.Hide();
			m_CompEffect3.Hide();
		}
	}
	else if(m_nStatus == STATUS_CHANGING_ITEM)
	{	
		ProcessComp();
		m_nStatus = STATUS_WAITING_MATERIALS;
	}
}
//-----------------------------------------------------------
// Chay hieu ung nang cap
//-----------------------------------------------------------
int KUiCompoundTwo::PlayEffect()
{
	/*if(m_CompEffect1.GetCurrentFrame() >= MAX_SPR_FRAME)
	{
		m_CompEffect1.SetFrame(0);
		m_CompEffect2.SetFrame(0);
		m_CompEffect3.SetFrame(0);
		return 0;
	}
	else
	{
		m_CompEffect1.NextFrame();
		m_CompEffect2.NextFrame();
		m_CompEffect3.NextFrame();
		return 1;
	}*/
	return 1;
}
//-----------------------------------------------------------------------------
// Tuong tac hop thoai
//-----------------------------------------------------------------------------
int KUiCompoundTwo::WndProc( unsigned int uMsg, unsigned int uParam, int nParam )
{
	switch(uMsg)
	{
	case WND_N_SCORLLBAR_POS_CHANGED:
		if(uParam == (unsigned int)&m_ListScroll)
		{
			m_Guide.SetFirstShowLine(nParam);
		}
		break;
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_Cancel)
		{
			OnCancel();
		}
		else if(uParam == (unsigned int)&m_Compound)
		{
			ProcessComp();
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
//------------------------------------
// Nang cap
//------------------------------------
void KUiCompoundTwo::ProcessComp()
{
	KUiDraggedObject pObj;
	int nDetail[_ITEM_COMP_COUNT];
	int nMoneyHold = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	
	memset(&pObj, 0, sizeof(pObj));
	nDetail[UIEP_BOX1] = 0;
	m_ItemBox[UIEP_BOX1].GetObject(pObj);
	if (pObj.uId <= 0)
	{	
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","6","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}
	
	//nDetail[UIEP_BOX1] = g_pCoreShell->GetGameData(GDI_ITEM_DETAIL,(unsigned int)(&pObj), 0);

	memset(&pObj, 0, sizeof(pObj));
	nDetail[UIEP_BOX2] = 0;
	m_ItemBox[UIEP_BOX2].GetObject(pObj);
	if (pObj.uId <= 0)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","6","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}
	//nDetail[UIEP_BOX2] = g_pCoreShell->GetGameData(GDI_ITEM_DETAIL,(unsigned int)(&pObj), 0);

	memset(&pObj, 0, sizeof(pObj));
	nDetail[UIEP_BOX3] = 0;
	m_ItemBox[UIEP_BOX3].GetObject(pObj);
	if (pObj.uId <= 0)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","6","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}
	//nDetail[UIEP_BOX3] = g_pCoreShell->GetGameData(GDI_ITEM_DETAIL,(unsigned int)(&pObj), 0);

	memset(&pObj, 0, sizeof(pObj));
	if(nDetail[UIEP_BOX1] != nDetail[UIEP_BOX2] ||  nDetail[UIEP_BOX1] != nDetail[UIEP_BOX3] || 
		nDetail[UIEP_BOX2] != nDetail[UIEP_BOX3])
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","7","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}

	if(nDetail[UIEP_BOX1] >= 13 || nDetail[UIEP_BOX2] >= 13 || nDetail[UIEP_BOX3] >= 13)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","8","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}

	if(nMoneyHold < CASH_COMPOUND)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","4","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}

	if(m_nStatus == STATUS_WAITING_MATERIALS)
	{	
		m_nStatus = STATUS_BEGIN_COMPOUND;
		return;
	}
	
	//g_pCoreShell->OperationRequest(GOI_LOAD_BUTTON_SCRIPT, (unsigned int)COMPOUND_SCRIPT_FILE, (unsigned int)"CompTwoItem");
	
	char		Buff[64];
	KIniFile	Ini;
	
	sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
	if (Ini.Load(Buff))
	{
		ZeroMemory(Buff,sizeof(Buff));
		Ini.GetString("ReturnInfo","9","",Buff,sizeof(Buff));
		
		m_Guide.Clear();
		m_Guide.AddOneMessage(Buff,sizeof(Buff));
	}
	memset(&pObj, 0, sizeof(pObj));
	memset(&nDetail, 0, sizeof(nDetail));
	nMoneyHold = 0;
	ZeroMemory(Buff,sizeof(Buff));
	return;
}
//------------------------------------
// Huy bo
//------------------------------------
void KUiCompoundTwo::OnCancel()
{	
	if(g_pCoreShell)
	{
		//g_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_comptwo, 0); 
	}
}
//------------------------------------------------
// Thay doi vat pham
//------------------------------------------------
void KUiCompoundTwo::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
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
		Pick.eContainer = UOC_COMPTWO_ITEM;
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
		Drop.eContainer = UOC_COMPTWO_ITEM;
	}

	for (int i = 0; i < _ITEM_COMP_COUNT; i++)
	{
		if (pWnd == (KWndWindow*)&m_ItemBox[i])
		{
			Drop.Region.v = Pick.Region.v = CtrlCompTwoItemMap[i].nPosition;
			break;
		}
	}
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
	pPickPos ? (unsigned int)&Pick : 0,
	pDropPos ? (int)&Drop : 0);

}
//----------------------------------------------------
// Lam moi du lieu
//----------------------------------------------------
void KUiCompoundTwo::UpdateData()
{
	UpdateAllItem();
	
}
//-------------------------------------
// Lam moi tat ca vat pham
//-------------------------------------
void KUiCompoundTwo::UpdateAllItem()
{
	/*KUiObjAtRegion	Item[_ITEM_COMP_COUNT];
	int nCount = g_pCoreShell->GetGameData(GDI_COMPTWO_ITEM, (unsigned int)&Item, 0);
	int	i;
	for (i = 0; i < _ITEM_COMP_COUNT; i++)
		m_ItemBox[i].Celar();
	for (i = 0; i < nCount; i++)
	{
		if (Item[i].Obj.uGenre != CGOG_NOTHING)
			UpdateItem(&Item[i], true);
	}*/
}
//----------------------------------------------------------------
// Lam moi tung vat pham
//----------------------------------------------------------------
void KUiCompoundTwo::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		for (int i = 0; i < _ITEM_COMP_COUNT; i++)
		{
			if (CtrlCompTwoItemMap[i].nPosition == pItem->Region.v)
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
//--------------------------------
// Ve hop thoai
//--------------------------------
void KUiCompoundTwo::PaintWindow()
{
	KWndPage::PaintWindow();
}

//****************************************************** NANG CAP KHOANG THACH ******************************************

//--------------------------
// Khoi tao hop thoai
//--------------------------
KUiCompoundThree::KUiCompoundThree()
{
	m_nStatus = STATUS_WAITING_MATERIALS;
}
//--------------------------------
// Khoi tao cac bien
//--------------------------------
void KUiCompoundThree::Initialize()
{	
	for (int i = 0; i < _ITEM_COMP_COUNT; i ++)
	{
		m_ItemBox[i].SetObjectGenre(CGOG_ITEM);
		AddChild(&m_ItemBox[i]);
		m_ItemBox[i].SetContainerId((int)UOC_COMPTWO_ITEM);
	}
	AddChild(&m_Compound);
	AddChild(&m_Cancel);

	AddChild(&m_Guide);
	AddChild(&m_ListScroll);
	 
	AddChild(&m_Pos1);
	AddChild(&m_Pos2);
	AddChild(&m_Pos3);

	AddChild(&m_CompEffect1);
	AddChild(&m_CompEffect2);
	AddChild(&m_CompEffect3);

	m_Guide.SetScrollbar(&m_ListScroll);

	
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	
	char		Buff[512];
	KIniFile	Ini;
	
	sprintf(Buff, "%s\\%s", Scheme, SCHEME_INI_SHEET);
	if (Ini.Load(Buff))
	{
		int nLen = sizeof(Buff);

		ZeroMemory(Buff, nLen);
		Ini.GetString("RuleInfo","UpPropMine","",Buff, nLen);
		
		nLen = TEncodeText(Buff, strlen(Buff));

		m_Guide.AddOneMessage(Buff, nLen);

		ZeroMemory(Buff, nLen);
	}

	Wnd_AddWindow(this);
}
//---------------------------------------------------
// Doc tap tin ini
//---------------------------------------------------
void KUiCompoundThree::LoadScheme( const char* pScheme )
{
	char		Buff[128],Buffer[64];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_COMPE);
	if (Ini.Load(Buff))
	{
		KWndImage::Init(&Ini, "Main");
		for (int i = 0; i < _ITEM_COMP_COUNT; i ++)
		{
			m_ItemBox[i].Init(&Ini, CtrlCompTwoItemMap[i].pIniSection);
		}
		m_Compound.Init(&Ini,"CompoundBtn");
		m_Cancel.Init(&Ini,"CancleBtn");
		m_Guide.Init(&Ini,"GuideList");
		m_ListScroll.Init(&Ini,"GuideList_Scroll");
		
		int nX,nY,nColor;
		Ini.GetInteger2("Box_0","Pos",&nX,&nY);

		if (Ini.GetString("TextColor", "Font", "", Buffer, sizeof(Buffer)))
		{
			nColor = (::GetColor(Buffer) & 0xFFFFFF);
		}
		m_nSelect = 0;
		m_Pos1.SetPosition(nX - 30, nY - 4);
		m_Pos1.SetText("Kho¸ng th¹ch 1");
		m_Pos1.SetTextColor(nColor);
		m_Pos1.BringToTop();

		Ini.GetInteger2("Box_1","Pos",&nX,&nY);

		m_Pos2.SetPosition(nX - 30, nY - 4);
		m_Pos2.SetText("Kho¸ng th¹ch 2");
		m_Pos2.SetTextColor(nColor);
		m_Pos2.BringToTop();

		Ini.GetInteger2("Box_2","Pos",&nX,&nY);
		
		m_Pos3.SetPosition(nX - 30, nY - 4);
		m_Pos3.SetText("Kho¸ng th¹ch 3");
		m_Pos3.SetTextColor(nColor);
		m_Pos3.BringToTop();

		m_CompEffect1.Init(&Ini,"Effect_0");
		m_CompEffect2.Init(&Ini,"Effect_1");
		m_CompEffect3.Init(&Ini,"Effect_2");

		m_CompEffect1.Hide();
		m_CompEffect2.Hide();
		m_CompEffect3.Hide();
		
	}	
}
//--------------------------------------------------------
// Ham chay thuc thi
//--------------------------------------------------------
void KUiCompoundThree::Breathe()
{	
	if(m_nStatus == STATUS_BEGIN_COMPOUND)
	{
		m_CompEffect1.Show();
		m_CompEffect1.SetFrame(0);
		m_CompEffect2.Show();
		m_CompEffect2.SetFrame(0);
		m_CompEffect3.Show();
		m_CompEffect3.SetFrame(0);
		m_nStatus = STATUS_COMPOUNDING;
	}
	else if(m_nStatus == STATUS_COMPOUNDING)
	{
		if(!PlayEffect())
		{
			m_nStatus = STATUS_CHANGING_ITEM;
			m_CompEffect1.Hide();
			m_CompEffect2.Hide();
			m_CompEffect3.Hide();
		}
	}
	else if(m_nStatus == STATUS_CHANGING_ITEM)
	{	
		ProcessComp();
		m_nStatus = STATUS_WAITING_MATERIALS;
	}
}
//-----------------------------------------------------------
// Chay hieu ung nang cap
//-----------------------------------------------------------
int KUiCompoundThree::PlayEffect()
{
	/*if(m_CompEffect1.GetCurrentFrame() >= MAX_SPR_FRAME)
	{
		m_CompEffect1.SetFrame(0);
		m_CompEffect2.SetFrame(0);
		m_CompEffect3.SetFrame(0);
		return 0;
	}
	else
	{
		m_CompEffect1.NextFrame();
		m_CompEffect2.NextFrame();
		m_CompEffect3.NextFrame();
		return 1;
	}*/
	return 1;
}
//-----------------------------------------------------------------------------
// Tuong tac hop thoai
//-----------------------------------------------------------------------------
int KUiCompoundThree::WndProc( unsigned int uMsg, unsigned int uParam, int nParam )
{
	switch(uMsg)
	{
	case WND_N_SCORLLBAR_POS_CHANGED:
		if(uParam == (unsigned int)&m_ListScroll)
		{
			m_Guide.SetFirstShowLine(nParam);
		}
		break;
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_Cancel)
		{
			OnCancel();		
		}
		else if(uParam == (unsigned int)&m_Compound)
		{
			ProcessComp();			
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
//------------------------------------
// Nang cap
//------------------------------------
void KUiCompoundThree::ProcessComp()
{
	KUiDraggedObject pObj;
	int nDetail[_ITEM_COMP_COUNT];
	int nSeries[_ITEM_COMP_COUNT];
	int nLevel[_ITEM_COMP_COUNT];
	int nOption[_ITEM_COMP_COUNT];
	int nMoneyHold = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	
	memset(&pObj, 0, sizeof(pObj));
	nDetail[UIEP_BOX1] = 0;
	nSeries[UIEP_BOX1] = 0;
	nLevel[UIEP_BOX1] = 0;
	nOption[UIEP_BOX1] = 0;
	m_ItemBox[UIEP_BOX1].GetObject(pObj);
	if (pObj.uId <= 0)
	{	
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","10","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}
	//nDetail[UIEP_BOX1] = g_pCoreShell->GetGameData(GDI_ITEM_DETAIL,(unsigned int)(&pObj), 0);
	//nSeries[UIEP_BOX1] = g_pCoreShell->GetGameData(GDI_ITEM_SERIES,(unsigned int)(&pObj), 0);
	//nLevel[UIEP_BOX1] = g_pCoreShell->GetGameData(GDI_ITEM_LEVEL,(unsigned int)(&pObj), 0);
	//nOption[UIEP_BOX1] = g_pCoreShell->GetGameData(GDI_ITEM_MAGICTYPE,(unsigned int)(&pObj), 0);

	memset(&pObj, 0, sizeof(pObj));
	nDetail[UIEP_BOX2] = 0;
	nSeries[UIEP_BOX2] = 0;
	nLevel[UIEP_BOX2] = 0;
	nOption[UIEP_BOX2] = 0;
	m_ItemBox[UIEP_BOX2].GetObject(pObj);
	if (pObj.uId <= 0)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","10","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}
	//nDetail[UIEP_BOX2] = g_pCoreShell->GetGameData(GDI_ITEM_DETAIL,(unsigned int)(&pObj), 0);
	//nSeries[UIEP_BOX2] = g_pCoreShell->GetGameData(GDI_ITEM_SERIES,(unsigned int)(&pObj), 0);
	//nLevel[UIEP_BOX2] = g_pCoreShell->GetGameData(GDI_ITEM_LEVEL,(unsigned int)(&pObj), 0);
	//nOption[UIEP_BOX2] = g_pCoreShell->GetGameData(GDI_ITEM_MAGICTYPE,(unsigned int)(&pObj), 0);

	memset(&pObj, 0, sizeof(pObj));
	nDetail[UIEP_BOX3] = 0;
	nSeries[UIEP_BOX3] = 0;
	nLevel[UIEP_BOX3] = 0;
	nOption[UIEP_BOX3] = 0;
	m_ItemBox[UIEP_BOX3].GetObject(pObj);
	if (pObj.uId <= 0)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","10","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}
	//nDetail[UIEP_BOX3] = g_pCoreShell->GetGameData(GDI_ITEM_DETAIL,(unsigned int)(&pObj), 0);
	//nSeries[UIEP_BOX3] = g_pCoreShell->GetGameData(GDI_ITEM_SERIES,(unsigned int)(&pObj), 0);
	//nLevel[UIEP_BOX3] = g_pCoreShell->GetGameData(GDI_ITEM_LEVEL,(unsigned int)(&pObj), 0);
	//nOption[UIEP_BOX3] = g_pCoreShell->GetGameData(GDI_ITEM_MAGICTYPE,(unsigned int)(&pObj), 0);

	if(nDetail[UIEP_BOX1] != nDetail[UIEP_BOX2] ||  nDetail[UIEP_BOX1] != nDetail[UIEP_BOX3] || 
		nDetail[UIEP_BOX2] != nDetail[UIEP_BOX3])
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","11","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}
	
	
	if(nSeries[UIEP_BOX1] != nSeries[UIEP_BOX2] ||  nSeries[UIEP_BOX1] != nSeries[UIEP_BOX3] || 
		nSeries[UIEP_BOX2] != nSeries[UIEP_BOX3])
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","12","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}

	if(nLevel[UIEP_BOX1] != nLevel[UIEP_BOX2] ||  nLevel[UIEP_BOX1] != nLevel[UIEP_BOX3] || 
		nLevel[UIEP_BOX2] != nLevel[UIEP_BOX3])
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","13","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}

	if(nOption[UIEP_BOX1] != nOption[UIEP_BOX2] ||  nOption[UIEP_BOX1] != nOption[UIEP_BOX3] || 
		nOption[UIEP_BOX2] != nOption[UIEP_BOX3])
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","14","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}
	
	if(nLevel[UIEP_BOX1] >= 10 || nLevel[UIEP_BOX2] >= 10 || nLevel[UIEP_BOX3] >= 10)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","15","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}

	if(nMoneyHold < CASH_COMPOUND)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","4","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}

	if(m_nStatus == STATUS_WAITING_MATERIALS)
	{	
		m_nStatus = STATUS_BEGIN_COMPOUND;
		return;
	}
	//g_pCoreShell->OperationRequest(GOI_LOAD_BUTTON_SCRIPT, (unsigned int)COMPOUND_SCRIPT_FILE, (unsigned int)"CompThreeItem");
	
	char		Buff[64];
	KIniFile	Ini;
	
	sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_COMPE);
	if (Ini.Load(Buff))
	{
		ZeroMemory(Buff,sizeof(Buff));
		Ini.GetString("ReturnInfo","16","",Buff,sizeof(Buff));
		
		m_Guide.Clear();
		m_Guide.AddOneMessage(Buff,sizeof(Buff));
	}
	memset(&pObj, 0, sizeof(pObj));
	memset(&nDetail, 0, sizeof(nDetail));
	memset(&nSeries, 0, sizeof(nSeries));
	memset(&nLevel, 0, sizeof(nLevel));
	memset(&nOption, 0, sizeof(nOption));
	nMoneyHold = 0;
	ZeroMemory(Buff,sizeof(Buff));
	return;
}
//------------------------------------
// Huy bo
//------------------------------------
void KUiCompoundThree::OnCancel()
{	
	if(g_pCoreShell)
	{
		//g_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_compthree, 0); 
	}
}
//------------------------------------------------
// Thay doi vat pham
//------------------------------------------------
void KUiCompoundThree::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
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
		Pick.eContainer = UOC_COMPTHREE_ITEM;
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
		Drop.eContainer = UOC_COMPTHREE_ITEM;
	}

	for (int i = 0; i < _ITEM_COMP_COUNT; i++)
	{
		if (pWnd == (KWndWindow*)&m_ItemBox[i])
		{
			Drop.Region.v = Pick.Region.v = CtrlCompThreeItemMap[i].nPosition;
			break;
		}
	}
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
	pPickPos ? (unsigned int)&Pick : 0,
	pDropPos ? (int)&Drop : 0);

}
//----------------------------------------------------
// Lam moi du lieu
//----------------------------------------------------
void KUiCompoundThree::UpdateData()
{
	UpdateAllItem();
	
}
//-------------------------------------
// Lam moi tat ca vat pham
//-------------------------------------
void KUiCompoundThree::UpdateAllItem()
{
	/*KUiObjAtRegion	Item[_ITEM_COMP_COUNT];
	int nCount = g_pCoreShell->GetGameData(GDI_COMPTHREE_ITEM, (unsigned int)&Item, 0);
	int	i;
	for (i = 0; i < _ITEM_COMP_COUNT; i++)
		m_ItemBox[i].Celar();
	for (i = 0; i < nCount; i++)
	{
		if (Item[i].Obj.uGenre != CGOG_NOTHING)
			UpdateItem(&Item[i], true);
	}*/
}
//----------------------------------------------------------------
// Lam moi tung vat pham
//----------------------------------------------------------------
void KUiCompoundThree::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		for (int i = 0; i < _ITEM_COMP_COUNT; i++)
		{
			if (CtrlCompThreeItemMap[i].nPosition == pItem->Region.v)
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
//--------------------------------
// Ve hop thoai
//--------------------------------
void KUiCompoundThree::PaintWindow()
{
	KWndPage::PaintWindow();
}

//***************************************** RUT OPTION *********************************************

//---------------------------
// Khoi tao hop thoai
//---------------------------
KUiDistill::KUiDistill()
{
	m_nStatus = STATUS_WAITING_MATERIALS;
}
//---------------------------------------------------
// Khoi tao cac bien
//---------------------------------------------------
void KUiDistill::Initialize()
{
	for (int i = 0; i < _ITEM_OUTIN_COUNT; i ++)
	{
		m_ItemBox[i].SetObjectGenre(CGOG_ITEM);
		AddChild(&m_ItemBox[i]);
		m_ItemBox[i].SetContainerId((int)UOC_DISTILL_ITEM);
	}

	AddChild(&m_Distill);
	AddChild(&m_Cancle);
	
	AddChild(&m_Guide);
	AddChild(&m_ListScroll);

	AddChild(&m_Pos1);
	AddChild(&m_Pos2);
	AddChild(&m_Pos3);
	AddChild(&m_Pos4);

	AddChild(&m_DistillEffect1);
	AddChild(&m_DistillEffect2);

	m_Guide.SetScrollbar(&m_ListScroll);
	
	
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	
	char		Buff[512];
	KIniFile	Ini;
	
	sprintf(Buff, "%s\\%s", Scheme, SCHEME_INI_SHEET);
	if (Ini.Load(Buff))
	{
		int nLen = sizeof(Buff);

		ZeroMemory(Buff, nLen);
		Ini.GetString("RuleInfo","Distill","",Buff, nLen);
		
		nLen = TEncodeText(Buff, strlen(Buff));

		m_Guide.AddOneMessage(Buff, nLen);

		ZeroMemory(Buff, nLen);
	}
	Wnd_AddWindow(this);
}
//---------------------------------------------------
// Doc tap tin ini
//---------------------------------------------------
void KUiDistill::LoadScheme( const char* pScheme )
{
	char		Buff[128],Buffer[64];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_OUTIN);
	if (Ini.Load(Buff))
	{
		KWndImage::Init(&Ini, "Main");
		for (int i = 0; i < _ITEM_OUTIN_COUNT; i ++)
		{
			m_ItemBox[i].Init(&Ini, CtrlDistillItemMap[i].pIniSection);
		}
		m_Distill.Init(&Ini,"DistillBtn");
		m_Cancle.Init(&Ini,"CancleBtn");
		m_Guide.Init(&Ini,"GuideList");
		m_ListScroll.Init(&Ini,"GuideList_Scroll");
			
		int nX,nY,nColor;
		Ini.GetInteger2("EquipPos","Pos",&nX,&nY);
		if (Ini.GetString("TextColor", "Font", "", Buffer, sizeof(Buffer)))
		{
			nColor = (::GetColor(Buffer) & 0xFFFFFF);
		}

		m_Pos1.SetPosition(nX - 28, nY - 4);
		m_Pos1.SetTextColor(nColor);
		m_Pos1.BringToTop();
		m_Pos1.SetText("Trang bÞ");

		Ini.GetInteger2("CryolitePos","Pos",&nX,&nY);
		m_Pos2.SetPosition(nX - 28, nY - 4);
		m_Pos2.SetTextColor(nColor);
		m_Pos2.BringToTop();
		m_Pos2.SetText("HuyÒn tinh");

		Ini.GetInteger2("PropMinePos","Pos",&nX,&nY);
		m_Pos3.SetPosition(nX - 28, nY - 4);
		m_Pos3.SetTextColor(nColor);
		m_Pos3.BringToTop();
		m_Pos3.SetText("Kho¸ng th¹ch");

		Ini.GetInteger2("ConsumePos","Pos",&nX,&nY);
		m_Pos4.SetPosition(nX - 28, nY - 4);
		m_Pos4.SetTextColor(nColor);
		m_Pos4.BringToTop();
		m_Pos4.SetText("Nguyªn liÖu");

		m_DistillEffect1.Init(&Ini,"EquipEffect");
		m_DistillEffect2.Init(&Ini,"ConsumeEffect");
	
		m_DistillEffect1.Hide();
		m_DistillEffect2.Hide();
	}	
}
//--------------------------------------------------------
// Ham chay thuc thi
//--------------------------------------------------------
void KUiDistill::Breathe()
{	
	if(m_nStatus == STATUS_BEGIN_DISTILL)
	{
		m_DistillEffect1.Show();
		m_DistillEffect1.SetFrame(0);
		m_DistillEffect2.Show();
		m_DistillEffect2.SetFrame(0);
		m_nStatus = STATUS_DISTILLING;
	}
	else if(m_nStatus == STATUS_DISTILLING)
	{
		if(!PlayEffect())
		{
			m_nStatus = STATUS_CHANGING_ITEM;
			m_DistillEffect1.Hide();
			m_DistillEffect2.Hide();
		}
	}
	else if(m_nStatus == STATUS_CHANGING_ITEM)
	{	
		ProcessDistill();
		m_nStatus = STATUS_WAITING_MATERIALS;
	}
}
//-----------------------------------------------------------
// Chay hieu ung che tao
//-----------------------------------------------------------
int KUiDistill::PlayEffect()
{
	/*if(m_DistillEffect1.GetCurrentFrame() >= MAX_SPR_FRAME)
	{
		m_DistillEffect1.SetFrame(0);
		m_DistillEffect2.SetFrame(0);
		return 0;
	}
	else
	{
		m_DistillEffect1.NextFrame();
		m_DistillEffect2.NextFrame();
		return 1;
	}*/
	return 1;
}
//----------------------------------------------------------------------------
// Tuong tac hop thoai
//----------------------------------------------------------------------------
int KUiDistill::WndProc( unsigned int uMsg, unsigned int uParam, int nParam )
{	
	switch(uMsg)
	{
		case WND_N_SCORLLBAR_POS_CHANGED:
			if(uParam == (unsigned int)&m_ListScroll)
			{
				m_Guide.SetFirstShowLine(nParam);
			}
			break;
		case WND_N_BUTTON_CLICK:
			if(uParam == (unsigned int)&m_Cancle)
			{
				OnCancel();
			}
			else if(uParam == (unsigned int)&m_Distill)
			{ 
				ProcessDistill();
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
//------------------------------------
// Rut option
//------------------------------------
void KUiDistill::ProcessDistill()
{
	KUiDraggedObject pObj;
	int nDetail, nSeries[2], nOption[6];
	int nMoneyHold = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);

	memset(&pObj, 0, sizeof(pObj));
	nSeries[0] = 0;
	memset(&nOption, 0, sizeof(nOption));
	m_ItemBox[UIEP_ITEM_].GetObject(pObj);
	if (pObj.uId <= 0)
	{	
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","1","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}
	//nSeries[0] = g_pCoreShell->GetGameData(GDI_ITEM_SERIES,(unsigned int)(&pObj), 0);
	//nOption[0] = g_pCoreShell->GetGameData(GDI_ITEM_MAGICTYPE,(unsigned int)(&pObj), 0);
	//nOption[1] = g_pCoreShell->GetGameData(GDI_ITEM_MAGICTYPE,(unsigned int)(&pObj), 1); 
	//nOption[2] = g_pCoreShell->GetGameData(GDI_ITEM_MAGICTYPE,(unsigned int)(&pObj), 2); 
	//nOption[3] = g_pCoreShell->GetGameData(GDI_ITEM_MAGICTYPE,(unsigned int)(&pObj), 3);
	//nOption[4] = g_pCoreShell->GetGameData(GDI_ITEM_MAGICTYPE,(unsigned int)(&pObj), 4); 
	//nOption[5] = g_pCoreShell->GetGameData(GDI_ITEM_MAGICTYPE,(unsigned int)(&pObj), 5); 
	
	memset(&pObj, 0, sizeof(pObj));
	m_ItemBox[UIEP_CRYOLITE].GetObject(pObj);
	if (pObj.uId <= 0)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","2","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}

	memset(&pObj, 0, sizeof(pObj));
	nSeries[1] = 0;
	m_ItemBox[UIEP_PROPMINE].GetObject(pObj);
	if (pObj.uId <= 0)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","3","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}
	//nSeries[1] = g_pCoreShell->GetGameData(GDI_ITEM_SERIES,(unsigned int)(&pObj), 0);
	//nDetail = g_pCoreShell->GetGameData(GDI_ITEM_DETAIL,(unsigned int)(&pObj), 0);

	nDetail = 0;
	switch(nDetail)
	{
		case 0:
			break;
		case 1:
			{
				if(!nOption[2])
				{
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","4","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
			}
			break;
		case 2:
			{
				if(!nOption[4])
				{
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","5","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
			}
		case 3:
		case 4:
		case 5: 
		case 6:
		case 7:
			{
				if(!nOption[1])
				{
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","6","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
			}
			break;
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
			{
				if(!nOption[3])
				{
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","7","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
			}
			break;
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
			{
				if(!nOption[5])
				{
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","8","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
			}
			break;
		default:
			break;
	}

	if((nSeries[0] != nSeries[1]) && nDetail > 2)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","9","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}

	if(nMoneyHold < CASH_COMPOUND)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","10","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}

	if(m_nStatus == STATUS_WAITING_MATERIALS)
	{	
		m_nStatus = STATUS_BEGIN_DISTILL;
		return;
	}
	
	//g_pCoreShell->OperationRequest(GOI_LOAD_BUTTON_SCRIPT, (unsigned int)DISTILL_SCRIPT_FILE, (unsigned int)"DistillItem");
	
	char		Buff[64];
	KIniFile	Ini;
	
	sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
	if (Ini.Load(Buff))
	{
		ZeroMemory(Buff,sizeof(Buff));
		Ini.GetString("ReturnInfo","11","",Buff,sizeof(Buff));
		
		m_Guide.Clear();
		m_Guide.AddOneMessage(Buff,sizeof(Buff));
	}
	memset(&pObj, 0, sizeof(pObj));
	memset(&nSeries, 0, sizeof(nSeries));
	memset(&nOption, 0, sizeof(nOption));
	nDetail = 0;
	nMoneyHold = 0;
	ZeroMemory(Buff,sizeof(Buff));
	return;
}
//------------------------------------
// Huy bo
//------------------------------------
void KUiDistill::OnCancel()
{	
	if(g_pCoreShell)
	{
		//g_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_distill, 0); 
	}
}
//------------------------------------------------
// Thay doi vat pham
//------------------------------------------------
void KUiDistill::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
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
		Pick.eContainer = UOC_DISTILL_ITEM;
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
		Drop.eContainer = UOC_DISTILL_ITEM;
	}

	for (int i = 0; i < _ITEM_OUTIN_COUNT; i++)
	{
		if (pWnd == (KWndWindow*)&m_ItemBox[i])
		{
			Drop.Region.v = Pick.Region.v = CtrlDistillItemMap[i].nPosition;
			break;
		}
	}
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
	pPickPos ? (unsigned int)&Pick : 0,
	pDropPos ? (int)&Drop : 0);

}
//----------------------------------------------------
// Lam moi du lieu
//----------------------------------------------------
void KUiDistill::UpdateData()
{
	UpdateAllItem();
	
}
//-------------------------------------
// Lam moi tat ca vat pham
//-------------------------------------
void KUiDistill::UpdateAllItem()
{
	/*KUiObjAtRegion	Item[_ITEM_OUTIN_COUNT];
	int nCount = g_pCoreShell->GetGameData(GDI_DISTILL_ITEM, (unsigned int)&Item, 0);
	int	i;
	for (i = 0; i < _ITEM_OUTIN_COUNT; i++)
		m_ItemBox[i].Celar();
	for (i = 0; i < nCount; i++)
	{
		if (Item[i].Obj.uGenre != CGOG_NOTHING)
			UpdateItem(&Item[i], true);
	}*/
}
//----------------------------------------------------------------
// Lam moi tung vat pham
//----------------------------------------------------------------
void KUiDistill::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		for (int i = 0; i < _ITEM_OUTIN_COUNT; i++)
		{
			if (CtrlDistillItemMap[i].nPosition == pItem->Region.v)
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
//-------------------------------
// Ve hop thoai
//-------------------------------
void KUiDistill::PaintWindow()
{
	KWndPage::PaintWindow();
}
//***************************************** CHE TAO TRANG BI TIM *****************************************
//--------------------
// Khoi tao hop thoai
//--------------------
KUiForge::KUiForge()
{
	m_nStatus = STATUS_WAITING_MATERIALS;
}
//-----------------------------
// Khoi tao cac bien
//-----------------------------
void KUiForge::Initialize()
{
	for (int i = 0; i < _ITEM_FORGE_COUNT; i ++)
	{
		m_ItemBox[i].SetObjectGenre(CGOG_ITEM);
		AddChild(&m_ItemBox[i]);
		m_ItemBox[i].SetContainerId((int)UOC_FORGE_ITEM);
	}
	AddChild(&m_ForgeBtn);
	AddChild(&m_Cancle);
	
	AddChild(&m_Guide);
	AddChild(&m_ListScroll);

	AddChild(&m_Pos1);
	AddChild(&m_Pos2);

	AddChild(&m_ForgeEffect);

	m_Guide.SetScrollbar(&m_ListScroll);
	
	
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	
	char		Buff[512];
	KIniFile	Ini;
	
	sprintf(Buff, "%s\\%s", Scheme, SCHEME_INI_SHEET);
	if (Ini.Load(Buff))
	{
		int nLen = sizeof(Buff);

		ZeroMemory(Buff, nLen);
		Ini.GetString("RuleInfo","Forge","",Buff, nLen);
		
		nLen = TEncodeText(Buff, strlen(Buff));

		m_Guide.AddOneMessage(Buff, nLen);

		ZeroMemory(Buff, nLen);
		
	}

	Wnd_AddWindow(this);
}
//------------------------------------------------
// Doc tap tin ini
//------------------------------------------------
void KUiForge::LoadScheme( const char* pScheme )
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_BUILD);
	if (Ini.Load(Buff))
	{
		KWndImage::Init(&Ini, "Main");
		for (int i = 0; i < _ITEM_COMP_COUNT; i ++)
		{
			m_ItemBox[i].Init(&Ini, CtrlForgeItemMap[i].pIniSection);
		}
		m_ForgeBtn.Init(&Ini,"ForgeBtn");
		m_Cancle.Init(&Ini,"CancleBtn");
		m_Guide.Init(&Ini,"GuideList");
		m_ListScroll.Init(&Ini,"GuideList_Scroll");
	
		int nX,nY,nColor;
		Ini.GetInteger2("EquipPos","Pos",&nX,&nY);
		if (Ini.GetString("TextColor", "Font", "", Buff, sizeof(Buff)))
		{
			nColor = (::GetColor(Buff) & 0xFFFFFF);
		}
		
		m_Pos1.SetPosition(nX - 32,nY - 4);
		m_Pos1.SetTextColor(nColor);
		m_Pos1.BringToTop();
		m_Pos1.SetText("Trang bÞ xanh");
		
		Ini.GetInteger2("CryolitePos","Pos",&nX,&nY);
		m_Pos2.SetPosition(nX - 28,nY - 4);
		m_Pos2.SetTextColor(nColor);
		m_Pos2.BringToTop();
		m_Pos2.SetText("HuyÒn tinh");

		m_ForgeEffect.Init(&Ini,"EquipEffect");
	
		m_ForgeEffect.Hide();
		
	}	
}
//--------------------------------------------------------
// Ham chay thuc thi
//--------------------------------------------------------
void KUiForge::Breathe()
{	
	if(m_nStatus == STATUS_BEGIN_FORGE)
	{
		m_ForgeEffect.Show();
		m_ForgeEffect.SetFrame(0);
		m_nStatus = STATUS_GORGING;
	}
	else if(m_nStatus == STATUS_GORGING)
	{
		if(!PlayEffect())
		{
			m_nStatus = STATUS_CHANGING_ITEM;
			m_ForgeEffect.Hide();
		}
	}
	else if(m_nStatus == STATUS_CHANGING_ITEM)
	{	
		ProcessForge();
		m_nStatus = STATUS_WAITING_MATERIALS;
	}
}
//-----------------------------------------------------------
// Chay hieu ung che tao
//-----------------------------------------------------------
int KUiForge::PlayEffect()
{
	/*if(m_ForgeEffect.GetCurrentFrame() >= MAX_SPR_FRAME)
	{
		m_ForgeEffect.SetFrame(0);
		return 0;
	}
	else
	{
		m_ForgeEffect.NextFrame();
		return 1;
	}*/
	return 1;
}
//---------------------------------------------------------------------------
// Tuong tac hop thoai
//---------------------------------------------------------------------------
int KUiForge::WndProc( unsigned int uMsg, unsigned int uParam, int nParam )
{
	switch(uMsg)
	{
	case WND_N_SCORLLBAR_POS_CHANGED:
		if(uParam == (unsigned int)&m_ListScroll)
		{
			m_Guide.SetFirstShowLine(nParam);
		}
		break;
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_Cancle)
		{
			OnCancel();
		}
		else if(uParam == (unsigned int)&m_ForgeBtn)
		{
			ProcessForge();
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
//------------------------------------
// Che tao
//------------------------------------
void KUiForge::ProcessForge()
{
	KUiDraggedObject pObj;
	int nMoneyHold = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);

	memset(&pObj, 0, sizeof(pObj));
	m_ItemBox[UIEP_ITEMFOR].GetObject(pObj);
	if (pObj.uId <= 0)
	{	
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_BUILD);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","1","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}

	memset(&pObj, 0, sizeof(pObj));
	m_ItemBox[UIEP_CRYOLITEFOR].GetObject(pObj);
	if (pObj.uId <= 0)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_BUILD);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","2","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}

	if(nMoneyHold < CASH_FORGE)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_BUILD);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","3","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}

	if(m_nStatus == STATUS_WAITING_MATERIALS)
	{	
		m_nStatus = STATUS_BEGIN_FORGE;
		return;
	}
	
	//g_pCoreShell->OperationRequest(GOI_LOAD_BUTTON_SCRIPT, (unsigned int)FORGE_SCRIPT_FILE, (unsigned int)"ForgeItem");
	
	char		Buff[64];
	KIniFile	Ini;
	
	sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_BUILD);
	if (Ini.Load(Buff))
	{
		ZeroMemory(Buff,sizeof(Buff));
		Ini.GetString("ReturnInfo","4","",Buff,sizeof(Buff));
		
		m_Guide.Clear();
		m_Guide.AddOneMessage(Buff,sizeof(Buff));
	}
	memset(&pObj, 0, sizeof(pObj));
	nMoneyHold = 0;
	ZeroMemory(Buff,sizeof(Buff));
	return;
}
//------------------------------------
// Huy bo
//------------------------------------
void KUiForge::OnCancel()
{	
	if(g_pCoreShell)
	{
		//g_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_forge, 0); 
	}
}
//------------------------------------------------
// Thay doi vat pham
//------------------------------------------------
void KUiForge::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
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
		Pick.eContainer = UOC_FORGE_ITEM;
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
		Drop.eContainer = UOC_FORGE_ITEM;
	}

	for (int i = 0; i < _ITEM_FORGE_COUNT; i++)
	{
		if (pWnd == (KWndWindow*)&m_ItemBox[i])
		{
			Drop.Region.v = Pick.Region.v = CtrlForgeItemMap[i].nPosition;
			break;
		}
	}
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
	pPickPos ? (unsigned int)&Pick : 0,
	pDropPos ? (int)&Drop : 0);

}
//----------------------------------------------------
// Lam moi du lieu
//----------------------------------------------------
void KUiForge::UpdateData()
{
	UpdateAllItem();
	
}
//-------------------------------------
// Lam moi tat ca vat pham
//-------------------------------------
void KUiForge::UpdateAllItem()
{
	/*KUiObjAtRegion	Item[_ITEM_FORGE_COUNT];
	int nCount = g_pCoreShell->GetGameData(GDI_FORGE_ITEM, (unsigned int)&Item, 0);
	int	i;
	for (i = 0; i < _ITEM_FORGE_COUNT; i++)
		m_ItemBox[i].Celar();
	for (i = 0; i < nCount; i++)
	{
		if (Item[i].Obj.uGenre != CGOG_NOTHING)
			UpdateItem(&Item[i], true);
	}*/
}
//----------------------------------------------------------------
// Lam moi tung vat pham
//----------------------------------------------------------------
void KUiForge::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		for (int i = 0; i < _ITEM_FORGE_COUNT; i++)
		{
			if (CtrlForgeItemMap[i].nPosition == pItem->Region.v)
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
//--------------------------
// Ve hop thoai
//--------------------------
void KUiForge::PaintWindow()
{
	KWndPage::PaintWindow();
}
//**************************************** KHAM NAM OPTION ****************************************

//--------------------------
// Khoi tao hop thoai
//--------------------------
KUiEnchase::KUiEnchase()
{
	m_nStatus = STATUS_WAITING_MATERIALS;
}
//-------------------------------
// Khoi tao cac bien
//-------------------------------
void KUiEnchase::Initialize()
{
	for (int i = 0; i < _ITEM_OUTIN_COUNT; i ++)
	{
		m_ItemBox[i].SetObjectGenre(CGOG_ITEM);
		AddChild(&m_ItemBox[i]);
		m_ItemBox[i].SetContainerId((int)UOC_ENCHASE_ITEM);
	}

	AddChild(&m_Enchase);
	AddChild(&m_Cancle);
	
	AddChild(&m_Guide);
	AddChild(&m_ListScroll);

	AddChild(&m_Pos1);
	AddChild(&m_Pos2);
	AddChild(&m_Pos3);
	AddChild(&m_Pos4);

	AddChild(&m_EnchaseEffect1);
	AddChild(&m_EnchaseEffect2);

	m_Guide.SetScrollbar(&m_ListScroll);
	
	
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	
	char		Buff[600];
	KIniFile	Ini;
	
	sprintf(Buff, "%s\\%s", Scheme, SCHEME_INI_SHEET);
	if (Ini.Load(Buff))
	{
		int nLen = sizeof(Buff);

		ZeroMemory(Buff, nLen);
		Ini.GetString("RuleInfo","Enchase","",Buff, nLen);
		
		nLen = TEncodeText(Buff, strlen(Buff));

		m_Guide.AddOneMessage(Buff, nLen);

		ZeroMemory(Buff, nLen);
	}
	Wnd_AddWindow(this);
}
//----------------------------------------------------
// Doc tap tin ini
//----------------------------------------------------
void KUiEnchase::LoadScheme( const char* pScheme )
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_OUTIN);
	if (Ini.Load(Buff))
	{
		KWndImage::Init(&Ini, "Main");
		for (int i = 0; i < _ITEM_OUTIN_COUNT; i ++)
		{
			m_ItemBox[i].Init(&Ini, CtrlEnchaseItemMap[i].pIniSection);
		}
		m_Enchase.Init(&Ini,"DistillBtn");
		m_Cancle.Init(&Ini,"CancleBtn");
		m_Guide.Init(&Ini,"GuideList");
		m_ListScroll.Init(&Ini,"GuideList_Scroll");
	
		int nX,nY,nColor;
		Ini.GetInteger2("EquipPos","Pos",&nX,&nY);
		if (Ini.GetString("TextColor", "Font", "", Buff, sizeof(Buff)))
		{
			nColor = (::GetColor(Buff) & 0xFFFFFF);
		}
		
		m_Pos1.SetPosition(nX - 32,nY - 4);
		m_Pos1.SetTextColor(nColor);
		m_Pos1.BringToTop();
		m_Pos1.SetText("Trang bÞ tÝm");
		
		Ini.GetInteger2("CryolitePos","Pos",&nX,&nY);
		m_Pos2.SetPosition(nX - 28,nY - 4);
		m_Pos2.SetTextColor(nColor);
		m_Pos2.BringToTop();
		m_Pos2.SetText("HuyÒn tinh");
		
		Ini.GetInteger2("PropMinePos","Pos",&nX,&nY);
		m_Pos3.SetPosition(nX - 28,nY - 4);
		m_Pos3.SetTextColor(nColor);
		m_Pos3.BringToTop();
		m_Pos3.SetText("Kho¸ng th¹ch");
		
		Ini.GetInteger2("ConsumePos","Pos",&nX,&nY);
		m_Pos4.SetPosition(nX - 32,nY - 4);
		m_Pos4.SetTextColor(nColor);
		m_Pos4.BringToTop();
		m_Pos4.SetText("Nguyªn liÖu");

		m_EnchaseEffect1.Init(&Ini,"EquipEffect");
		m_EnchaseEffect2.Init(&Ini,"ConsumeEffect");
	
		m_EnchaseEffect1.Hide();
		m_EnchaseEffect2.Hide();
	}	
}
//--------------------------------------------------------
// Ham chay thuc thi
//--------------------------------------------------------
void KUiEnchase::Breathe()
{	
	if(m_nStatus == STATUS_BEGIN_ENCHASE)
	{
		m_EnchaseEffect1.Show();
		m_EnchaseEffect1.SetFrame(0);
		m_EnchaseEffect2.Show();
		m_EnchaseEffect2.SetFrame(0);
		m_nStatus = STATUS_ENCHASING;
	}
	else if(m_nStatus == STATUS_ENCHASING)
	{
		if(!PlayEffect())
		{
			m_nStatus = STATUS_CHANGING_ITEM;
			m_EnchaseEffect1.Hide();
			m_EnchaseEffect2.Hide();
		}
	}
	else if(m_nStatus == STATUS_CHANGING_ITEM)
	{	
		ProcessEnchase();
		m_nStatus = STATUS_WAITING_MATERIALS;
	}
}
//-----------------------------------------------------------
// Chay hieu ung kham nam
//-----------------------------------------------------------
int KUiEnchase::PlayEffect()
{
	/*if(m_EnchaseEffect1.GetCurrentFrame() >= MAX_SPR_FRAME)
	{
		m_EnchaseEffect1.SetFrame(0);
		m_EnchaseEffect2.SetFrame(0);
		return 0;
	}
	else
	{
		m_EnchaseEffect1.NextFrame();
		m_EnchaseEffect2.NextFrame();
		return 1;
	}*/
	return 1;
}
//-----------------------------------------------------------------------------
// Tuong tac hop thoai
//-----------------------------------------------------------------------------
int KUiEnchase::WndProc( unsigned int uMsg, unsigned int uParam, int nParam )
{
	switch(uMsg)
	{
	case WND_N_SCORLLBAR_POS_CHANGED:
		if(uParam == (unsigned int)&m_ListScroll)
		{
			m_Guide.SetFirstShowLine(nParam);
		}
		break;
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_Cancle)
		{
			OnCancel();
		}
		else if(uParam == (unsigned int)&m_Enchase)
		{
			ProcessEnchase();	
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
//------------------------------------
// Kham nam
//------------------------------------
void KUiEnchase::ProcessEnchase()
{
	KUiDraggedObject pObj1, pObj2;
	int nDetail, nSeries[2], nOption[6], nOpProp;
	BOOL bCanSet = FALSE;
	int nMoneyHold = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);

	memset(&pObj1, 0, sizeof(pObj1));
	nSeries[0] = 0;
	memset(&nOption, 0, sizeof(nOption));
	m_ItemBox[UIEP_ITEM_].GetObject(pObj1);
	if (pObj1.uId <= 0)
	{	
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","12","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}
	//nSeries[0] = g_pCoreShell->GetGameData(GDI_ITEM_SERIES,(unsigned int)(&pObj1), 0);
	//nOption[0] = g_pCoreShell->GetGameData(GDI_ITEM_MAGICTYPE,(unsigned int)(&pObj1), 0);
	//nOption[1] = g_pCoreShell->GetGameData(GDI_ITEM_MAGICTYPE,(unsigned int)(&pObj1), 1); 
	//nOption[2] = g_pCoreShell->GetGameData(GDI_ITEM_MAGICTYPE,(unsigned int)(&pObj1), 2); 
	//nOption[3] = g_pCoreShell->GetGameData(GDI_ITEM_MAGICTYPE,(unsigned int)(&pObj1), 3);
	//nOption[4] = g_pCoreShell->GetGameData(GDI_ITEM_MAGICTYPE,(unsigned int)(&pObj1), 4); 
	//nOption[5] = g_pCoreShell->GetGameData(GDI_ITEM_MAGICTYPE,(unsigned int)(&pObj1), 5); 

	memset(&pObj2, 0, sizeof(pObj2));
	m_ItemBox[UIEP_CRYOLITE].GetObject(pObj2);
	if (pObj2.uId <= 0)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","13","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}

	memset(&pObj2, 0, sizeof(pObj2));
	nSeries[1] = 0;
	m_ItemBox[UIEP_PROPMINE].GetObject(pObj2);
	if (pObj2.uId <= 0)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","14","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}
	//nSeries[1] = g_pCoreShell->GetGameData(GDI_ITEM_SERIES,(unsigned int)(&pObj2), 0);
	//nDetail = g_pCoreShell->GetGameData(GDI_ITEM_DETAIL,(unsigned int)(&pObj2), 0);
	//nOpProp = g_pCoreShell->GetGameData(GDI_ITEM_MAGICTYPE,(unsigned int)(&pObj2), 0);
	//bCanSet = g_pCoreShell->GetGameData(GDI_OPTION_CAN_SET,(unsigned int)(&pObj1), nOpProp);
	
	nDetail = 0;
	switch(nDetail)
	{
		case 0:
			{
				if(nOption[0] != 53)
				{
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","16","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
			}
			break;
		case 1:
			{
				if(!nOption[2])
				{
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","15","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
				if(nOption[2] != 53)
				{
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","16","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
				if(nOption[0] == 53 || nOption[1] == 53)
				{
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","17","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
			}
			break;
		case 2:
			{
				if(!nOption[4])
				{
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","15","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
				if(nOption[4] != 53)
				{	
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","16","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
				if(nOption[0] == 53 || nOption[1] == 53 || nOption[2] == 53 || nOption[3] == 53)
				{
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","17","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
			}
			break;
		case 3:
		case 4:
		case 5: 
		case 6:
		case 7:
			{
				if(!nOption[1])
				{
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","15","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
				if(nOption[1] != 53)
				{
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","16","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
				if(nOption[0] == 53)
				{
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","17","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
			}
			break;
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
			{
				if(!nOption[3])
				{
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","15","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
				if(nOption[3] != 53)
				{
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","16","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
				if(nOption[0] == 53 || nOption[1] == 53 || nOption[2] == 53)
				{
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","17","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
			}
			break;
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
			{
				if(!nOption[5])
				{
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","15","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
				if(nOption[5] != 53)
				{
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","16","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
				if(nOption[0] == 53 || nOption[1] == 53 || nOption[2] == 53 || nOption[3] == 53 || nOption[4] == 53)
				{
					char		Buff[64];
					KIniFile	Ini;
					
					sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
					if (Ini.Load(Buff))
					{
						ZeroMemory(Buff,sizeof(Buff));
						Ini.GetString("ReturnInfo","17","",Buff,sizeof(Buff));
						
						m_Guide.Clear();
						m_Guide.AddOneMessage(Buff,sizeof(Buff));
					}
					ZeroMemory(Buff,sizeof(Buff));
					return;
				}
			}
			break;
		default:
			break;
	}

	if((nSeries[0] != nSeries[1]) && nDetail > 3)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","18","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}
	
	if(!bCanSet)
	{
		char		Buff[128];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","19","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}
	if(nMoneyHold < CASH_FORGE)
	{
		char		Buff[64];
		KIniFile	Ini;
		
		sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
		if (Ini.Load(Buff))
		{
			ZeroMemory(Buff,sizeof(Buff));
			Ini.GetString("ReturnInfo","20","",Buff,sizeof(Buff));
			
			m_Guide.Clear();
			m_Guide.AddOneMessage(Buff,sizeof(Buff));
		}
		ZeroMemory(Buff,sizeof(Buff));
		return;
	}

	if(m_nStatus == STATUS_WAITING_MATERIALS)
	{	
		m_nStatus = STATUS_BEGIN_ENCHASE;
		return;
	}
	
	//g_pCoreShell->OperationRequest(GOI_LOAD_BUTTON_SCRIPT, (unsigned int)ENCHASE_SCRIPT_FILE, (unsigned int)"EnchaseItem");
	
	char		Buff[64];
	KIniFile	Ini;
	
	sprintf(Buff, "%s\\%s", "Ui\\Ui3", SCHEME_INI_OUTIN);
	if (Ini.Load(Buff))
	{
		ZeroMemory(Buff,sizeof(Buff));
		Ini.GetString("ReturnInfo","21","",Buff,sizeof(Buff));
		
		m_Guide.Clear();
		m_Guide.AddOneMessage(Buff,sizeof(Buff));
	}
	memset(&pObj1, 0, sizeof(pObj1));
	memset(&pObj2, 0, sizeof(pObj2));
	memset(&nSeries, 0, sizeof(nSeries));
	memset(&nOption, 0, sizeof(nOption));
	nDetail = 0;
	nOpProp = 0;
	bCanSet = 0;
	nMoneyHold = 0;
	ZeroMemory(Buff,sizeof(Buff));
	return;
}
//------------------------------------
// Huy bo
//------------------------------------
void KUiEnchase::OnCancel()
{	
	if(g_pCoreShell)
	{
		//g_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_enchase, 0); 
	}
}
//------------------------------------------------
// Thay doi vat pham
//------------------------------------------------
void KUiEnchase::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
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
		Pick.eContainer = UOC_ENCHASE_ITEM;
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
		Drop.eContainer = UOC_ENCHASE_ITEM;
	}

	for (int i = 0; i < _ITEM_OUTIN_COUNT; i++)
	{
		if (pWnd == (KWndWindow*)&m_ItemBox[i])
		{
			Drop.Region.v = Pick.Region.v = CtrlEnchaseItemMap[i].nPosition;
			break;
		}
	}
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
	pPickPos ? (unsigned int)&Pick : 0,
	pDropPos ? (int)&Drop : 0);

}
//----------------------------------------------------
// Lam moi du lieu
//----------------------------------------------------
void KUiEnchase::UpdateData()
{
	UpdateAllItem();
	
}
//-------------------------------------
// Lam moi tat ca vat pham
//-------------------------------------
void KUiEnchase::UpdateAllItem()
{
	/*KUiObjAtRegion	Item[_ITEM_OUTIN_COUNT];
	int nCount = g_pCoreShell->GetGameData(GDI_ENCHASE_ITEM, (unsigned int)&Item, 0);
	int	i;
	for (i = 0; i < _ITEM_OUTIN_COUNT; i++)
		m_ItemBox[i].Celar();
	for (i = 0; i < nCount; i++)
	{
		if (Item[i].Obj.uGenre != CGOG_NOTHING)
			UpdateItem(&Item[i], true);
	}*/
}
//----------------------------------------------------------------
// Lam moi tung vat pham
//----------------------------------------------------------------
void KUiEnchase::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		for (int i = 0; i < _ITEM_OUTIN_COUNT; i++)
		{
			if (CtrlEnchaseItemMap[i].nPosition == pItem->Region.v)
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
//----------------------------
// Ve hop thoai
//----------------------------
void KUiEnchase::PaintWindow()
{
	KWndPage::PaintWindow();
}
