/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong KiÒu
//	CreateTime:	2020-9-2
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../elem/wnds.h"
#include "UiItem.h"
#include "UiStatus.h"
#include "UiTradeConfirmWnd.h"		//edit by phong kieu sua do F3
#include "UiUnlockBox.h"
#include "UiPlayerBar.h"
#include "UiAddPoint.h"
#include "UiChooseFace.h"
#include "../ShortcutKey.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/gamedatadef.h"
#include "../UiBase.h"

extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI		"UiStatus.ini"

KUiStatus* KUiStatus::m_pSelf = NULL;

// -------------------------------------------------------------------------
static struct UE_CTRL_MAP
{
	int				nPosition;
	const char*		pIniSection;
}CtrlItemMap[_ITEM_COUNT] =
{
	{ UIEP_HEAD,		"Cap"		},	//×°±¸-Ã±×Ó
	{ UIEP_HAND,		"Weapon"	},	//×°±¸-ÎäÆ÷
	{ UIEP_NECK,		"Necklace"	},	//×°±¸-ÏîÁ´
	{ UIEP_FINESSE,		"Bangle"	},	//×°±¸-ÊÖïí
	{ UIEP_BODY,		"Cloth"		},	//×°±¸-ÒÂ·þ
	{ UIEP_WAIST,		"Sash"		},	//×°±¸-Ñü´ø
	{ UIEP_FINGER1,		"Ring1"		},	//×°±¸-½äÖ¸
	{ UIEP_FINGER2,		"Ring2"		},	//×°±¸-½äÖ¸
	{ UIEP_WAIST_DECOR,	"Pendant"	},	//×°±¸-Ñü×º
	{ UIEP_FOOT,		"Shoes"		},	//×°±¸-Ð¬×Ó
	{ UIEP_HORSE,		"Horse"		},	//×°±¸-Âí
	{ UIEP_MASK,		"Mask"		},
	{ UIEP_FIFONG,		"Mantle"	},
	{ UIEP_SIGNET,		"Signet"	},	//Equipment-Horse
	{ UIEP_SHIPIN,		"Shipin"	},	//Equipment-Horse
	{ UIEP_HOODS,		"Hoods"		},	//Equipment-Horse
	{ UIEP_CLOAK,		"Cloak"		},	//Equipment-Horse
};

KUiStatus* KUiStatus::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

KUiStatus* KUiStatus::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiStatus;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();
		if (g_pCoreShell->GetGameData(GDI_EQUIPMENT_SETNUM, 0, 0) == 1)
			m_pSelf->m_BtnSet1.SetFrame(0);
		else
			m_pSelf->m_BtnSet2.SetFrame(0);
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹Ø±Õ´°¿Ú£¬Í¬Ê±¿ÉÒÔÑ¡ÔòÊÇ·ñÉ¾³ý¶ÔÏóÊµÀý
//--------------------------------------------------------------------------
void KUiStatus::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy == false)
			m_pSelf->Hide();
		else
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}

		g_UiBase.SetStatus(UIS_S_IDLE);
		KUiItem::OnNpcTradeMode(false);
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º³õÊ¼»¯
//--------------------------------------------------------------------------
void KUiStatus::Initialize()
{
	AddChild(&m_Agname);
	AddChild(&m_Name);
	AddChild(&m_Title);

	AddChild(&m_Money);

	AddChild(&m_Life);
	AddChild(&m_Mana);
	AddChild(&m_Stamina);
	AddChild(&m_Angry);
	AddChild(&m_Experience);

	AddChild(&m_RemainPoint);
	AddChild(&m_fkMateName);
	AddChild(&m_Strength);
	AddChild(&m_Dexterity);
	AddChild(&m_Vitality);
	AddChild(&m_Energy);

	AddChild(&m_AddStrength);
	AddChild(&m_AddDexterity);
	AddChild(&m_AddVitality);
	AddChild(&m_AddEnergy);

	AddChild(&m_LeftDamage);
	AddChild(&m_RightDamage);
	AddChild(&m_Attack);
	AddChild(&m_Defence);
	AddChild(&m_MoveSpeed);
	AddChild(&m_AttackSpeed);

	AddChild(&m_PhyDef);
	AddChild(&m_CoolDef);
	AddChild(&m_LightDef);
	AddChild(&m_FireDef);
	AddChild(&m_PoisonDef);
	AddChild(&m_PhyDefPlus);
	AddChild(&m_CoolDefPlus);
	AddChild(&m_LightDefPlus);
	AddChild(&m_FireDefPlus);
	AddChild(&m_PoisonDefPlus);
	AddChild(&m_Level);
	AddChild(&m_WorldRank);//edit by phong kieu F3 worldrank
	AddChild(&m_StatusDesc);
	AddChild(&m_ChooseAvatar);
	AddChild(&m_Avatar);
	AddChild(&m_nPk);
	AddChild(&m_nRepute);
	AddChild(&m_nFuYuan);
	AddChild(&m_ReBorn);

	for (int i = 0; i < _ITEM_COUNT; i ++)
	{
		m_EquipBox[i].SetObjectGenre(CGOG_ITEM);
		AddChild(&m_EquipBox[i]);
		m_EquipBox[i].SetContainerId((int)UOC_EQUIPTMENT);
	}

	AddChild(&m_EquipExpandBtn);
	AddChild(&m_MaskFeature);

	AddChild(&m_OpenItemPad);
	AddChild(&m_BtnLock);
	AddChild(&m_BtnSet1);
	AddChild(&m_BtnSet2);
	AddChild(&m_Bind);
	AddChild(&m_UnBind);
	AddChild(&m_Close);

	SwitchExpand(TRUE);
	Wnd_AddWindow(this);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
}

void KUiStatus::SwitchExpand(BOOL bShow)
{
	m_EquipExpandBtn.CheckButton(bShow);
	bShow ? m_MaskFeature.Show() : m_MaskFeature.Hide();
	bShow ? m_EquipExpandImg.Show() : m_EquipExpandImg.Hide();
	for (int i = UIEP_FIFONG; i < _ITEM_COUNT; i++)
		bShow ? m_EquipBox[i].Show() : m_EquipBox[i].Hide();
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë´°¿ÚµÄ½çÃæ·½°¸
//--------------------------------------------------------------------------
void KUiStatus::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
			m_pSelf->LoadScheme(&Ini);	
	}
}
extern int SCREEN_WIDTH;
void KUiStatus::LoadScheme(class KIniFile* pIni)
{
	if (g_pCoreShell->GetGameData(GDI_PLAYER_IS_MALE, 0, 0))
	{;
		if (SCREEN_WIDTH == 1024)
			Init(pIni, "Male1024");
		else
			Init(pIni, "Male");
	}
	else
	{
		if (SCREEN_WIDTH == 1024)
			Init(pIni, "Female1024");
		else
			Init(pIni, "Female");
		
	}

	m_ChooseAvatar.Init(pIni, "ClickHere");
	m_Avatar  .Init(pIni, "ClickHere");
	int nX = 9;
	int nY = 133;
	pIni->GetInteger("Face", "Left", 9, &nX);
	pIni->GetInteger("Face", "Top", 133, &nY);
	m_Avatar.SetPosition(nX, nY);
	m_Agname  .Init(pIni, "Agname");
	m_Name    .Init(pIni, "Name");
	m_Title   .Init(pIni, "Title");
	m_Money  .Init(pIni, "Money");
	m_Life		.Init(pIni, "Life");
	m_Mana		.Init(pIni, "Mana");
	m_Stamina	.Init(pIni, "Stamina");
	m_Angry		.Init(pIni, "Angry");		
	m_Experience.Init(pIni, "Exp");
	m_RemainPoint.Init(pIni, "RemainPoint");
	m_fkMateName.Init(pIni, "MateName");
	m_Strength   .Init(pIni, "Strength");
	m_Dexterity  .Init(pIni, "Dexterity");
	m_Vitality   .Init(pIni, "Vitality");
	m_Energy     .Init(pIni, "Energy");	
	m_AddStrength .Init(pIni, "AddStrength");
	m_AddDexterity.Init(pIni, "AddDexterity");
	m_AddVitality .Init(pIni, "AddVitality");
	m_AddEnergy   .Init(pIni, "AddEnergy");
	m_LeftDamage .Init(pIni, "LeftDamage");
	m_RightDamage.Init(pIni, "RightDamage");
	m_Attack     .Init(pIni, "Attack");
	m_Defence    .Init(pIni, "Defense");
	m_MoveSpeed  .Init(pIni, "MoveSpeed");
	m_AttackSpeed.Init(pIni, "AttackSpeed");
	m_PhyDef	 .Init(pIni, "ResistPhy");
	m_CoolDef    .Init(pIni, "ResistCold");
	m_LightDef   .Init(pIni, "ResistLighting");
	m_FireDef    .Init(pIni, "ResistFire");
	m_PoisonDef  .Init(pIni, "ResistPoison");
	m_PhyDefPlus .Init(pIni, "ResistPhyPlus");
	m_CoolDefPlus.Init(pIni, "ResistColdPlus");
	m_LightDefPlus.Init(pIni, "ResistLightingPlus");
	m_FireDefPlus .Init(pIni, "ResistFirePlus");
	m_PoisonDefPlus.Init(pIni, "ResistPoisonPlus");
	m_Level		 .Init(pIni, "Level");
	m_WorldRank		 .Init(pIni, "WorldRank");//edit by phong kieu F3 worlrank
	m_StatusDesc .Init(pIni, "Status");
	m_Close	 .Init(pIni, "Close");
	m_nPk        .Init(pIni, "PKValue");
	m_nRepute    .Init(pIni, "Prestige");
	m_nFuYuan    .Init(pIni, "FuYuan");
	m_ReBorn    .Init(pIni, "TrungSinh");
	m_OpenItemPad.Init(pIni, "Item");
	m_BtnLock.Init(pIni, "BtnLock");
	m_BtnSet1.Init(pIni, "BtnSet1");
	m_BtnSet2.Init(pIni, "BtnSet2");
	m_Bind.Init(pIni, "BtnBind");
	m_UnBind.Init(pIni, "BtnUnBind");

	for (int i = 0; i < _ITEM_COUNT; i ++)
	{
		m_EquipBox[i].Init(pIni, CtrlItemMap[i].pIniSection);
	}

	m_EquipExpandBtn.Init(pIni, "EquipExpandBtn");
	m_EquipExpandImg.Init(pIni, "EquipExpandImg");
	m_MaskFeature.Init(pIni, "MaskFeature");
}

void KUiStatus::Breathe()
{
	if (g_pCoreShell && g_pCoreShell->GetGameData(GDI_IS_CHEST_UNLOCKED, 0, 0))
		m_BtnLock.CheckButton(0);
	else
		m_BtnLock.CheckButton(1);

	BYTE nNumIcon = g_pCoreShell->GetGameData(GDI_IS_CHECK_IMAGE, 0, 0);
	if (nNumIcon == 1)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu1.spr");
	else if (nNumIcon == 2)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu2.spr");
	else if (nNumIcon == 3)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu3.spr");
	else if (nNumIcon == 4)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu4.spr");
	else if (nNumIcon == 5)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu5.spr");
	else if (nNumIcon == 6)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu6.spr");
	else if (nNumIcon == 7)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu7.spr");
	else if (nNumIcon == 8)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu8.spr");
	else if (nNumIcon == 9)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu9.spr");
	else if (nNumIcon == 10)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu10.spr");
	else if (nNumIcon == 11)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu11.spr");
	else if (nNumIcon == 12)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu12.spr");
	else if (nNumIcon == 13)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam1.spr");
	else if (nNumIcon == 14)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam2.spr");
	else if (nNumIcon == 15)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam3.spr");
	else if (nNumIcon == 16)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam4.spr");
	else if (nNumIcon == 17)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam5.spr");
	else if (nNumIcon == 18)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam6.spr");
	else if (nNumIcon == 19)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam7.spr");
	else if (nNumIcon == 20)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam8.spr");
	else if (nNumIcon == 21)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam9.spr");
	else if (nNumIcon == 22)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam10.spr");
	else if (nNumIcon == 23)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam11.spr");
	else if (nNumIcon == 24)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam12.spr");
	else if (nNumIcon == 25)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam13.spr");
	else if (nNumIcon == 26)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam14.spr");
	else if (nNumIcon == 27)
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam15.spr");	

	if(nNumIcon > 0)
	{
		m_ChooseAvatar.SetText("");
	}
}

int KUiStatus::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	int numpoint = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_Close)
		{
			Hide();
			g_UiBase.SetStatus(UIS_S_IDLE);
			KUiItem::OnNpcTradeMode(false);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_Avatar)
		{
			if (g_pCoreShell->GetGameData(GDI_PLAYER_IS_MALE, 0, 0))
			{
				KUiChooseFace::OpenWindow(2);
			}
			else
			{	
				KUiChooseFace::OpenWindow(1);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_OpenItemPad)
			KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_ITEMS);
		else if (uParam == (unsigned int)(KWndWindow*)&m_Bind)
		{
			KUiItem::OnNpcTradeMode(true);
			g_UiBase.SetStatus(UIS_S_TRADE_LOCKITEM);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_UnBind)
		{
			KUiItem::OnNpcTradeMode(true);
			g_UiBase.SetStatus(UIS_S_TRADE_UNLOCKITEM);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_BtnLock)
		{
			if (g_pCoreShell->GetGameData(GDI_IS_CHEST_UNLOCKED, 0, 0))
			{
				// Ruong dang mo - Khoa ruong lai
				g_pCoreShell->OperationRequest(GOI_CP_LOCK, 0, 0);
			}
			else
			{
				// Ruong dang khoa ~> Mo cua so nhap pass ruong
				KUiUnlockBox::OpenWindow();
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_BtnSet1)
		{
			if (g_pCoreShell->GetGameData(GDI_EQUIPMENT_SETNUM, 0, 0) != 1) {
				g_pCoreShell->OperationRequest(GOI_CP_SWITCH_EQUIPSET, 0, 1); //request switch equip set
				m_BtnSet2.SetFrame(1);
				UiSoundPlay(UI_SI_SWITCH_EQUIP);
			}
			m_BtnSet1.SetFrame(0);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_BtnSet2)
		{
			if (g_pCoreShell->GetGameData(GDI_EQUIPMENT_SETNUM, 0, 0) != 2) {
				g_pCoreShell->OperationRequest(GOI_CP_SWITCH_EQUIPSET, 0, 2); //request switch equip set
				m_BtnSet1.SetFrame(1);
				UiSoundPlay(UI_SI_SWITCH_EQUIP);
			}
			m_BtnSet2.SetFrame(0);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_EquipExpandBtn)
			SwitchExpand(m_EquipExpandBtn.IsButtonChecked());
		else if (uParam == (unsigned int)(KWndWindow*)&m_MaskFeature)
			g_pCoreShell->OperationRequest(GOI_MASKFEATURE, 0, 0);
		else if (m_nRemainPoint > numpoint)//else if (m_nRemainPoint > 0)
		{
			if (uParam == (unsigned int)(KWndWindow*)&m_AddStrength)
				KUiAddPoint::OpenWindow(UIPA_STRENGTH);
				//UseRemainPoint(UIPA_STRENGTH,numpoint);//UseRemainPoint(UIPA_STRENGTH);
			else if (uParam == (unsigned int)(KWndWindow*)&m_AddDexterity)
				KUiAddPoint::OpenWindow(UIPA_DEXTERITY);
				//UseRemainPoint(UIPA_DEXTERITY,numpoint);//UseRemainPoint(UIPA_DEXTERITY);
			else if (uParam == (unsigned int)(KWndWindow*)&m_AddVitality)
				KUiAddPoint::OpenWindow(UIPA_VITALITY);
				//UseRemainPoint(UIPA_VITALITY,numpoint);//UseRemainPoint(UIPA_VITALITY);
			else if (uParam == (unsigned int)(KWndWindow*)&m_AddEnergy)
				KUiAddPoint::OpenWindow(UIPA_ENERGY);
				//UseRemainPoint(UIPA_ENERGY,numpoint);//UseRemainPoint(UIPA_ENERGY);
		}
		break;
	case WND_N_RIGHT_CLICK_ITEM:
		{
			KUiDraggedObject* pItem = (KUiDraggedObject*)uParam;
			KUiObjAtContRegion	Obj;
			Obj.Obj.uGenre = pItem->uGenre;
			Obj.Obj.uId = pItem->uId;
			Obj.Region.Width = pos_equiproom;
			if (g_UiBase.IsOperationEnable(UIS_O_USE_ITEM))
				g_pCoreShell->OperationRequest(GOI_USE_ITEM, (unsigned int)(&Obj), UOC_EQUIPTMENT);
		}break;
	case WND_N_ITEM_PICKDROP:
		if (g_pCoreShell->GetGameData(GDI_IS_CHEST_UNLOCKED, 0, 0))
		{
			if (g_UiBase.IsOperationEnable(UIS_O_MOVE_ITEM) || g_UiBase.GetStatus() == UIS_S_TRADE_REPAIR)
			{
				OnEquiptChanged((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
			}
			else if (g_UiBase.GetStatus() == UIS_S_TRADE_LOCKITEM)
			{
				OnLockItem((ITEM_PICKDROP_PLACE*)uParam, 1);
			} 
			else if (g_UiBase.GetStatus() == UIS_S_TRADE_UNLOCKITEM)
			{
				OnLockItem((ITEM_PICKDROP_PLACE*)uParam, 0);
			} 
			break;
		}
		g_pCoreShell->OperationRequest(GOI_PLAYER_ACTION, CN_GH, 0);
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

void KUiStatus::OnLockItem(ITEM_PICKDROP_PLACE* pItem, int lock)
{
	if (pItem == NULL || g_pCoreShell == NULL)
		return;
	KUiDraggedObject	Obj;
	((KWndObjectBox*)(pItem->pWnd))->GetObject(Obj);
	//Obj.Obj.uGenre = pItem->uGenre;
	//Obj.Obj.uId = pItem->uId;
	//Obj.Region.h = pItem->DataX;
	//Obj.Region.v = pItem->DataY;
	//Obj.Region.Width  = pItem->DataW;
	//Obj.Region.Height = pItem->DataH;
	//Obj.eContainer = UOC_ITEM_TAKE_WITH;
	g_pCoreShell->OperationRequest(GOI_LOCK_PLAYER_ITEM, (unsigned int)(&Obj), lock);
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÉý¼¶Ä³ÏîÊôÐÔ
//--------------------------------------------------------------------------
void KUiStatus::UseRemainPoint(UI_PLAYER_ATTRIBUTE type, int numpoint)
{
	g_pCoreShell->OperationRequest(GOI_TONE_UP_ATTRIBUTE, type, numpoint);//g_pCoreShell->OperationRequest(GOI_TONE_UP_ATTRIBUTE, type, 0);
	m_nRemainPoint = m_nRemainPoint - numpoint;//m_nRemainPoint--;
	m_RemainPoint.SetIntText(m_nRemainPoint);
	m_AddStrength.Enable(m_nRemainPoint);
	m_AddDexterity.Enable(m_nRemainPoint);
	m_AddVitality.Enable(m_nRemainPoint);
	m_AddEnergy.Enable(m_nRemainPoint);
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¸üÐÂ»ù±¾Êý¾Ý£¨ÈËÃûµÈ²»Ò×±äÊý¾Ý£©
//--------------------------------------------------------------------------
void KUiStatus::UpdateBaseData()
{
	KUiPlayerBaseInfo	Info;
	memset(&Info, 0, sizeof(KUiPlayerBaseInfo));
	g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (int)&Info, 0);
	m_Agname.SetText(Info.Agname);
	m_Name  .SetText(Info.Name);
	m_Title .SetText(Info.Title);
	if(Info.szMateName[0])
		m_fkMateName.SetText(Info.szMateName);
	else
		m_fkMateName.SetText("V« hÖ");
}

void KUiStatus::UpdateRuntimeInfo(KUiPlayerRuntimeInfo* pInfo)
{
	if (pInfo)
	{
		m_Life		.Set2IntText(pInfo->nLife, pInfo->nLifeFull, '/');
		m_Mana		.Set2IntText(pInfo->nMana, pInfo->nManaFull, '/');
		m_Stamina	.Set2IntText(pInfo->nStamina, pInfo->nStaminaFull, '/');
		m_Angry		.Set2IntText(pInfo->nAngry, pInfo->nAngryFull, '/');
		m_Experience.Set2DoubleText(pInfo->nExperience, pInfo->nExperienceFull, '/');
		//Info.byAction & PA_RIDE
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¸üÐÂÊý¾Ý
//--------------------------------------------------------------------------
void KUiStatus::UpdateData()
{
	UpdateAllEquips();
	UpdateBaseData();

	KUiPlayerAttribute	Info;
	memset(&Info, 0, sizeof(KUiPlayerAttribute));
	g_pCoreShell->GetGameData(GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&Info, 0);
	UpdateRuntimeAttribute(&Info);
}

void KUiStatus::UpdateAllEquips()
{
	KUiObjAtRegion	Equips[_ITEM_COUNT];
	int nCount = g_pCoreShell->GetGameData(GDI_EQUIPMENT, (unsigned int)&Equips, 0);
	int	i;
	for (i = 0; i < _ITEM_COUNT; i++)
		m_EquipBox[i].Celar();
	for (i = 0; i < nCount; i++)
	{
		if (Equips[i].Obj.uGenre != CGOG_NOTHING)
			UpdateEquip(&Equips[i], true);
	}
}

void KUiStatus::UpdateRuntimeAttribute(KUiPlayerAttribute* pInfo)
{
	if (pInfo)	
	{
		//m_Money.SetIntText(pInfo->nMoney);
		m_Money.SetMoneyText(pInfo->nMoney);

		m_nRemainPoint = pInfo->nBARemainPoint;
		m_AddStrength.Enable(m_nRemainPoint);
		m_AddDexterity.Enable(m_nRemainPoint);
		m_AddVitality.Enable(m_nRemainPoint);
		m_AddEnergy.Enable(m_nRemainPoint);
		m_RemainPoint.SetIntText(pInfo->nBARemainPoint);
		m_Strength   .SetIntText(pInfo->nStrength);
		m_Dexterity  .SetIntText(pInfo->nDexterity);
		m_Vitality   .SetIntText(pInfo->nVitality);
		m_Energy     .SetIntText(pInfo->nEnergy);

		m_LeftDamage.Set2IntText(pInfo->nKillMIN,pInfo->nKillMAX,'/');
		m_RightDamage.Set2IntText(pInfo->nRightKillMin, pInfo->nRightKillMax, '/');
		m_Attack.SetIntText(pInfo->nAttack);
		m_Defence.SetIntText(pInfo->nDefence);
		m_MoveSpeed.SetIntText(pInfo->nMoveSpeed);
		m_AttackSpeed.Set2IntText(pInfo->nAttackSpeed,pInfo->nCastSpeed, '/');
		
		//edit by phong kieu hien thi khang tinh tren 75
		m_PhyDef.Set6IntText(pInfo->nPhyDef, '%');
		m_CoolDef.Set6IntText(pInfo->nCoolDef, '%');
		m_LightDef.Set6IntText(pInfo->nLightDef, '%');
		m_FireDef.Set6IntText(pInfo->nFireDef, '%');
		m_PoisonDef.Set6IntText(pInfo->nPoisonDef, '%');

		if (pInfo->nPhyDefPlus)
			m_PhyDefPlus.SetResistPlus(pInfo->nPhyDefPlus, '+');
		else
			m_PhyDefPlus.Clear();
		if (pInfo->nCoolDefPlus)
			m_CoolDefPlus.SetResistPlus(pInfo->nCoolDefPlus, '+');
		else
			m_CoolDefPlus.Clear();
		if (pInfo->nLightDefPlus)
			m_LightDefPlus.SetResistPlus(pInfo->nLightDefPlus, '+');
		else
			m_LightDefPlus.Clear();
		if (pInfo->nFireDefPlus)
			m_FireDefPlus.SetResistPlus(pInfo->nFireDefPlus, '+');
		else
			m_FireDefPlus.Clear();
		if (pInfo->nPoisonDefPlus)
			m_PoisonDefPlus.SetResistPlus(pInfo->nPoisonDefPlus, '+');
		else
			m_PoisonDefPlus.Clear();

		m_Level.SetIntText(pInfo->nLevel);			
		m_StatusDesc.SetText(pInfo->StatusDesc);
		m_nPk.SetIntText(pInfo->nPKValue);
		m_nRepute.SetIntText(pInfo->nRepute);
		m_nFuYuan.SetIntText(pInfo->nFuYuan);
		m_ReBorn .Set4IntText(pInfo->nReBorn);

		if (g_pCoreShell)
		{
			KUiPlayerBaseInfo	Info;
			memset(&Info, 0, sizeof(KUiPlayerBaseInfo));
			g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (unsigned int)&Info, 0);
			
			if(Info.nRankInWorld > 0 && Info.nRankInWorld < 1000)//edit by phong kieu set hang the gioi mac dinh 0 thanh dau ?
			{
				m_WorldRank.SetIntText(Info.nRankInWorld);			//Eidt by phong kieu F3 worldRank
			}
			else
			{
				m_WorldRank.SetText(" ? ", 3);			//Eidt by phong kieu F3 worldRank
			}
		}
	}
}

void KUiStatus::OnEquiptChanged(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	KUiObjAtContRegion     Drop, Pick;
    KUiDraggedObject       Obj;
    KWndWindow*                    pWnd = NULL;
    UISYS_STATUS   eStatus = g_UiBase.GetStatus();
    if (pPickPos)
    {
		//_ASSERT(pPickPos->pWnd);
        ((KWndObjectBox*)(pPickPos->pWnd))->GetObject(Obj);
        Pick.Obj.uGenre = Obj.uGenre;
        Pick.Obj.uId = Obj.uId;
        Pick.Region.Width = Obj.DataW;
        Pick.Region.Height = Obj.DataH;
        Pick.Region.h = 0;
        Pick.eContainer = UOC_EQUIPTMENT;
        pWnd = pPickPos->pWnd;
	}
	else if (pDropPos)
    {
		pWnd = pDropPos->pWnd;
	}
	else
	{
		return;
	}

	if (pDropPos)
	{
		Wnd_GetDragObj(&Obj);
		Drop.Obj.uGenre = Obj.uGenre;
		Drop.Obj.uId = Obj.uId;
		Drop.Region.Width = Obj.DataW;
		Drop.Region.Height = Obj.DataH;
		Drop.Region.h = 0;
		Drop.eContainer = UOC_EQUIPTMENT;
	}

	for (int i = 0; i < _ITEM_COUNT; i++)
	{
		if (pWnd == (KWndWindow*)&m_EquipBox[i])
		{
			Drop.Region.v = Pick.Region.v = CtrlItemMap[i].nPosition;
			break;
		}
	}

	if (eStatus == UIS_S_TRADE_REPAIR) // doan da fix
	{
		KUiItemBuySelInfo      Price = { 0 };
		{
			if (g_pCoreShell->GetGameData(GDI_REPAIR_ITEM_PRICE, (unsigned int)(&Pick), (int)(&Price)))
			{
				if(Price.nCurPrice >0) //edit by phong kieu fix doan nay
				{
					KUiTradeConfirm::OpenWindow(&Pick, &Price, TCA_REPAIR);
				}
			}
		}
	}
	else
	{
		//_ASSERT(i < _ITEM_COUNT);
		g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, pPickPos ? (unsigned int)&Pick : 0, pDropPos ? (int)&Drop : 0);
		UiSoundPlayItem(Obj.uId);
	}
}

void KUiStatus::UpdateEquip(KUiObjAtRegion* pEquip, int bAdd)
{
	if (pEquip)
	{
		for (int i = 0; i < _ITEM_COUNT; i++)
		{
			if (CtrlItemMap[i].nPosition == pEquip->Region.v)
			{
				if (bAdd)
					m_EquipBox[i].HoldObject(pEquip->Obj.uGenre, pEquip->Obj.uId, pEquip->Region.Width, pEquip->Region.Height);
				else
					m_EquipBox[i].HoldObject(CGOG_NOTHING, 0, 0, 0);
				break;
			}
		}
	}
}
