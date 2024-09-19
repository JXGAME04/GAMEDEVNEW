//---------------------------------------------------------------------------
// Sword3 Engine (c) 2020 KingSoft
// File:	UiAutoPlay.cpp
// Date:	06/06/2020
// Code:	Fong KiÒu
// Desc:	KUiAutoPlay Class
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../Elem/PopupMenu.h"
#include "UiAutoPlay.h"
#include "../UiSoundSetting.h"
#include "../../../Core/Src/CoreShell.h"
#include "../UiBase.h"
#include "UiSysMsgCentre.h"

extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI					"UiAutoPlay.ini" //main
#define	SCHEME_FIGHT_INI			"UiAutoPlay_Fight.ini" //chiÕn ®Êu
#define	SCHEME_RESTORE_INI				"UiAutoPlay_Restore.ini" //phôc håi
#define	SCHEME_PICK_INI				"UiAutoPlay_Pick.ini" //nhÆt ®å
#define	SCHEME_MOVE_INI				"UiAutoPlay_Move.ini" //di chuyÓn
#define	SCHEME_MAP_INI				"UiAutoPlay_Map.ini" //map
#define	SCHEME_TEAM_INI				"UiAutoPlay_Team.ini" //tæ ®éi
#define	SCHEME_BLACKITEM_INI				"UiAutoPlay_BlackItem.ini" //vËt phÈm ®en

KUiPlayerItem*	m_fkNearbyPlayersList;

//=KUiAutoPlayFight==========================================================================
#define	SELECT_TYPE_FIGHT_OPTION					0x001
#define	SELECT_TYPE_FIGHT_DISTANCE				0x002
#define	SELECT_TYPE_FIGHT_SELFDEF				0x003
#define	SELECT_TYPE_FIGHT_SUPPORT1				0x004
#define	SELECT_TYPE_FIGHT_SUPPORT2				0x005
#define	SELECT_TYPE_FIGHT_SUPPORT3				0x006
#define	SELECT_TYPE_FIGHT_SKILLLEFT				0x007
#define	SELECT_TYPE_FIGHT_SKILLRIGHT			0x008
#define	SELECT_TYPE_FIGHT_AURA1				0x009
#define	SELECT_TYPE_FIGHT_AURA2				0x0010
#define	SELECT_TYPE_FIGHT_SHORTKEY1				0x0011
#define	SELECT_TYPE_FIGHT_SHORTKEY2				0x0012
#define	SELECT_TYPE_FIGHT_SHORTCUT1			0x0013
#define	SELECT_TYPE_FIGHT_SHORTCUT2			0x0014
#define	SELECT_TYPE_MAP_STATION					0x0015
#define	SELECT_TYPE_MAP_BUY_HP					0x0016
#define	SELECT_TYPE_MAP_BUY_MP					0x0017
#define	SELECT_TYPE_MAP_BUY_TOXIC			0x0018

char g_ArraySelectFightOption[][64] = 
{
	"Tù ®éng",
	"Lªn ngùa",
	"Xuèng ngùa", 
};

char g_ArraySelectFightDistanceOption[][64] = 
{
	"Bá qua",
	"Tr¸nh boss",
	"¦u tiªn boss", 
	"ChØ ®¸nh boss", 
};

char g_ArraySelectFightSelfDefOption[][64] = 
{
	"Tr¸nh xa",
	"Thæ ®Þa phï",
	"Tho¸t game", 
	"§¸nh tr¶", 
};

enum STT_Magic
{
	enumMagic_ID1,
	enumMagic_ID2,
	enumMagic_ID3,
};
enum Sel_Support_Magic
{
	enumSel_Support_Magic1,
	enumSel_Support_Magic2,
	enumSel_Support_Magic3,	
	enumSel_Support_Magic_Num,
};

char g_ArraySelectShortKeyOption[][64] = 
{
	"Bá qua",
	"PhÝm Q/F2",
	"PhÝm W/F3",
	"PhÝm E/F4",
	"PhÝm A/F5",
	"PhÝm S/F6",
};

char g_ArraySelectShortCut1Option[][64] = 
{
	"Bá qua",
	"XuÊt chiªu",
	"Thêi gian",
	"Sinh lùc <",
	"Néi lùc <",
	"Cast bïa",
};

char g_ArraySelectShortCut2Option[][64] = 
{
	"Bá qua",
	"XuÊt chiªu",
	"Thêi gian",
	"Sinh lùc >",
	"Néi lùc >",
	"Cast bïa",
};

KUiAutoPlayFight::KUiAutoPlayFight() //load data tõ auto config lªn
{	
	m_bFightCheckBox	= TRUE; //set mÆc ®Þnh tù ®¸nh true
	m_nFightRange	= 600; //ph¹m vi tù ®¸nh
	m_bFightSelect			= 0;
	//
	m_bFightDistance	= TRUE; //tiÕp cËn true
	m_nFightDistance	= 75; //ph¹m vi tiÕp cËn
	m_bFightDistanceSelect = 0;
	//
	m_bFightSelfDef			= TRUE; //tù vÖ
	m_nFightSelfDefValue	= 600; //kho¶ng c¸ch tù vÖ
	m_bFightSelfDefSelect = 0;
	//
	memset(m_CurrentSelMagic, 0, sizeof(m_CurrentSelMagic));
	//
	m_nLeftSkillID = 0;
	m_nRightSkillID = 0;
	//
	m_nSkillSelfDefID = 0;
	m_SkillBossID = 0;
	//
	m_ShortcutKeySelect1 = 0;
	m_ShortcutKeySelect2 = 0;
	//
	m_nShortcutEditBox1 = 0;
	m_nShortcutEditBox2 = 0;
	//
	m_ShortcutOptSelect1 = 0;
	m_ShortcutOptSelect2 = 0;
	//
	m_bSkillRightCheckBox = FALSE;
	//
	m_bNRDoSkillCheckBox = TRUE;

}

void KUiAutoPlayFight::Initialize()
{	
	AddChild(&m_FightCheckBox);//tù ®¸nh
	AddChild(&m_FightRangeEditBox);
	AddChild(&m_FightOptionTxt);
	AddChild(&m_FightMenuDownBtn);
	//
	AddChild(&m_FightDistanceCheckBox);//tiÕp cËn
	AddChild(&m_FightDistanceEditBox);
	AddChild(&m_FightDistanceOptionTxt);
	AddChild(&m_FightMenuDistanceDownBtn);
	//
	AddChild(&m_FightSelfDefCheckBox);//tù vÖ
	AddChild(&m_FightSelfDefEditBox);
	AddChild(&m_FightSelfDefOptionTxt);
	AddChild(&m_FightMenuSelfDefDownBtn);
	//
	AddChild(&m_BuffSkillTxt1);	//support1
	AddChild(&m_BuffSkillMenuDown1Btn);
	//
	AddChild(&m_BuffSkillTxt2);	//support2
	AddChild(&m_BuffSkillMenuDown2Btn);
	//
	AddChild(&m_BuffSkillTxt3);	//support3
	AddChild(&m_BuffSkillMenuDown3Btn);
	//
	AddChild(&m_LeftSkillTxt);	//leftskill
	AddChild(&m_LeftSkillMenuDown);
	//
	AddChild(&m_RightSkillTxt);	//rightskill
	AddChild(&m_RightSkillMenuDown);
	//
	AddChild(&m_AuraSkillTxt1);	//auraskill1
	AddChild(&m_AuraSkillMenuDown1);
	//
	AddChild(&m_AuraSkillTxt2);	//auraskill2
	AddChild(&m_AuraSkillMenuDown2);
	//
	AddChild(&m_ShortcutKeyTxt1);	//shortkey1
	AddChild(&m_ShortcutKeyMenuDown1);
	//
	AddChild(&m_ShortcutKeyTxt2);	//shortkey2
	AddChild(&m_ShortcutKeyMenuDown2);
	//
	AddChild(&m_ShortcutOptTxt1);	//shortcut1
	AddChild(&m_ShortcutOptMenuDown1);
	//
	AddChild(&m_ShortcutOptTxt2);	//shortcut2
	AddChild(&m_ShortcutOptMenuDown2);
	//
	AddChild(&m_ShortcutEditBox1);	//shortcutedit1
	AddChild(&m_ShortcutEditBox2);	//shortcutedit2
	//
	AddChild(&m_SkillRightCheckBox);
	//
	AddChild(&m_NRDoSkillCheckBox);
	//
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

void KUiAutoPlayFight::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;

	sprintf(Buff, "%s\\%s", pScheme, SCHEME_FIGHT_INI);

	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		m_FightCheckBox.Init(&Ini, "FightCheckBox");//tù ®¸nh
		m_FightRangeEditBox.Init(&Ini, "FightEditBox");
		m_FightOptionTxt.Init(&Ini, "FightOptionTxt");
		m_FightMenuDownBtn.Init(&Ini, "FightMenuDown");
		//
		m_FightDistanceCheckBox.Init(&Ini, "DistanceCheckBox");
		m_FightDistanceEditBox.Init(&Ini, "DistanceEditBox");//tiÕp cËn
		m_FightDistanceOptionTxt.Init(&Ini, "DistanceOptionTxt");
		m_FightMenuDistanceDownBtn.Init(&Ini, "DistanceMenuDown");
		//
		m_FightSelfDefCheckBox.Init(&Ini, "SelfDefCheckBox");//tù vÖ
		m_FightSelfDefEditBox.Init(&Ini, "SelfDefEditBox");
		m_FightSelfDefOptionTxt.Init(&Ini, "SelfDefOptionTxt");
		m_FightMenuSelfDefDownBtn.Init(&Ini, "SelfDefMenuDown");
		//
		m_BuffSkillTxt1.Init(&Ini, "BuffSkillTxt1");	//support1
		m_BuffSkillMenuDown1Btn.Init(&Ini, "BuffSkillMenuDown1");
		//
		m_BuffSkillTxt2.Init(&Ini, "BuffSkillTxt2");	//support2
		m_BuffSkillMenuDown2Btn.Init(&Ini, "BuffSkillMenuDown2");
		//
		m_BuffSkillTxt3.Init(&Ini, "BuffSkillTxt3");	//support3
		m_BuffSkillMenuDown3Btn.Init(&Ini, "BuffSkillMenuDown3");
		//
		m_LeftSkillTxt.Init(&Ini, "LeftSkillTxt");	//leftskill
		m_LeftSkillMenuDown.Init(&Ini, "LeftSkillMenuDown");
		//
		m_RightSkillTxt.Init(&Ini, "RightSkillTxt");	//rightskill
		m_RightSkillMenuDown.Init(&Ini, "RightSkillMenuDown");
		//
		m_AuraSkillTxt1.Init(&Ini, "AuraSkillTxt1");	//aura1
		m_AuraSkillMenuDown1.Init(&Ini, "AuraSkillMenuDown1");
		//
		m_AuraSkillTxt2.Init(&Ini, "AuraSkillTxt2");	//aura2
		m_AuraSkillMenuDown2.Init(&Ini, "AuraSkillMenuDown2");
		//
		m_ShortcutKeyTxt1.Init(&Ini, "ShortcutKeyTxt1");	//shortkey1
		m_ShortcutKeyMenuDown1.Init(&Ini, "ShortcutKeyMenuDown1");
		//
		m_ShortcutKeyTxt2.Init(&Ini, "ShortcutKeyTxt2");	//shortkey2
		m_ShortcutKeyMenuDown2.Init(&Ini, "ShortcutKeyMenuDown2");
		//
		m_ShortcutOptTxt1.Init(&Ini, "ShortcutOptTxt1");	//shortcut1
		m_ShortcutOptMenuDown1.Init(&Ini, "ShortcutOptMenuDown1");
		//
		m_ShortcutOptTxt2.Init(&Ini, "ShortcutOptTxt2");	//shortcut2
		m_ShortcutOptMenuDown2.Init(&Ini, "ShortcutOptMenuDown2");
		//
		m_ShortcutEditBox1.Init(&Ini, "ShortcutEditBox1");	//shortcutedit1
		m_ShortcutEditBox2.Init(&Ini, "ShortcutEditBox2");//shortcutedit2
		//
		m_SkillRightCheckBox.Init(&Ini, "SkillRightCheckBox");
		//
		m_NRDoSkillCheckBox.Init(&Ini, "NRDoSkillCheckBox");

	}
	UpdateData();
}

int KUiAutoPlayFight::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		{
			if (uParam == (unsigned int)(KWndWindow*)&m_FightCheckBox)//tù ®¸nh
			{
				m_bFightCheckBox = !m_bFightCheckBox;
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_FightDistanceCheckBox)//tiÕp cËn
			{
				m_bFightDistance = !m_bFightDistance;
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_FightSelfDefCheckBox)//tù vÖ
			{
				m_bFightSelfDef = !m_bFightSelfDef;
			}
			else if(uParam == (unsigned int)(KWndWindow*)&m_FightMenuDownBtn)//tù ®¸nh
			{
				PopupFightOptionMenu();
			}
			else if(uParam == (unsigned int)(KWndWindow*)&m_FightMenuDistanceDownBtn)//tiÕp cËn
			{
				PopupFightDistanceMenu();
			}
			else if(uParam == (unsigned int)(KWndWindow*)&m_FightMenuSelfDefDownBtn)//tù vÖ
			{
				PopupFightSelfDefMenu();
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_BuffSkillMenuDown1Btn)//support1
			{
				PopupSupportSkill(enumMagic_ID1);
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_BuffSkillMenuDown2Btn)//support2
			{
				PopupSupportSkill(enumMagic_ID2);
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_BuffSkillMenuDown3Btn)//support3
			{
				PopupSupportSkill(enumMagic_ID3);
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_LeftSkillMenuDown)//leftskill
			{
				PopupLeftSkill();
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_RightSkillMenuDown)//right
			{
				PopupRightSkill();
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_AuraSkillMenuDown1)//aura1
			{
				PopupAuraSkill1();
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_AuraSkillMenuDown2)//aura2
			{
				PopupAuraSkill2();
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_ShortcutKeyMenuDown1)//shortkey1
			{
				PopupShortKey1Menu();
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_ShortcutKeyMenuDown2)//shortkey2
			{
				PopupShortKey2Menu();
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_ShortcutOptMenuDown1)//shortcut1
			{
				PopupShortCut1Menu();
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_ShortcutOptMenuDown2)//shortkey2
			{
				PopupShortCut2Menu();
			}
			if (uParam == (unsigned int)(KWndWindow*)&m_SkillRightCheckBox)
			{
				m_bSkillRightCheckBox = !m_bSkillRightCheckBox;
			}
			if (uParam == (unsigned int)(KWndWindow*)&m_NRDoSkillCheckBox)
			{
				m_bNRDoSkillCheckBox = !m_bNRDoSkillCheckBox;
			}
			OnActive();
		}
		break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if (HIWORD(nParam) == SELECT_TYPE_FIGHT_OPTION && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessFightOption(BYTE(nParam));
			}
			else if (HIWORD(nParam) == SELECT_TYPE_FIGHT_DISTANCE && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessFightDistance(BYTE(nParam));
			}
			else if (HIWORD(nParam) == SELECT_TYPE_FIGHT_SELFDEF && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessFightSelfDef(BYTE(nParam));
			}
			else if (HIWORD(nParam) == SELECT_TYPE_FIGHT_SUPPORT1 && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessSupportSkill(enumMagic_ID1, LOWORD(nParam));
			}
			else if (HIWORD(nParam) == SELECT_TYPE_FIGHT_SUPPORT2 && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessSupportSkill(enumMagic_ID2, LOWORD(nParam));
			}
			else if (HIWORD(nParam) == SELECT_TYPE_FIGHT_SUPPORT3 && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessSupportSkill(enumMagic_ID3, LOWORD(nParam));
			}
			//
			else if (HIWORD(nParam) == SELECT_TYPE_FIGHT_SKILLLEFT && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessLeftSkill(LOWORD(nParam));
			}
			else if (HIWORD(nParam) == SELECT_TYPE_FIGHT_SKILLRIGHT && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessRightSkill(LOWORD(nParam));
			}
			else if (HIWORD(nParam) == SELECT_TYPE_FIGHT_AURA1 && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessAuraSkill1(LOWORD(nParam));
			}
			else if (HIWORD(nParam) == SELECT_TYPE_FIGHT_AURA2 && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessAuraSkill2(LOWORD(nParam));
			}
			//
			else if (HIWORD(nParam) == SELECT_TYPE_FIGHT_SHORTKEY1 && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessShortKey1(BYTE(nParam));
			}
			else if (HIWORD(nParam) == SELECT_TYPE_FIGHT_SHORTKEY2 && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessShortKey2(BYTE(nParam));
			}
			else if (HIWORD(nParam) == SELECT_TYPE_FIGHT_SHORTCUT1 && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessShortCut1(BYTE(nParam));
			}
			else if (HIWORD(nParam) == SELECT_TYPE_FIGHT_SHORTCUT2 && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessShortCut2(BYTE(nParam));
			}
			OnActive();
		}
		break;
	default:
		nRet = KWndPage::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

void KUiAutoPlayFight::UpdateData()//update c¸c gi¸ trÞ t¹i ui
{
	LoadConfig();

	m_FightCheckBox.CheckButton(m_bFightCheckBox);//tù ®¸nh
	m_FightRangeEditBox.SetIntText(m_nFightRange);
	m_FightOptionTxt.SetText(g_ArraySelectFightOption[m_bFightSelect]);
	ProcessFightOption(m_bFightSelect);
	//
	m_FightDistanceCheckBox.CheckButton(m_bFightDistance);//tiÕp cËn
	m_FightDistanceEditBox.SetIntText(m_nFightDistance);
	m_FightDistanceOptionTxt.SetText(g_ArraySelectFightDistanceOption[m_bFightDistanceSelect]);
	ProcessFightDistance(m_bFightDistanceSelect);
	//
	m_FightSelfDefCheckBox.CheckButton(m_bFightSelfDef);//tù vÖ
	m_FightSelfDefEditBox.SetIntText(m_nFightSelfDefValue);
	m_FightSelfDefOptionTxt.SetText(g_ArraySelectFightSelfDefOption[m_bFightSelfDefSelect]);
	ProcessFightSelfDef(m_bFightSelfDefSelect);

	for (int i = 0; i < enumSel_Support_Magic_Num; i ++)
	{
		ProcessSupportSkill(i, m_CurrentSelMagic[i]);
	}

	ProcessLeftSkill(m_nLeftSkillID);
	ProcessRightSkill(m_nRightSkillID);
	ProcessAuraSkill1(m_nSkillSelfDefID);
	ProcessAuraSkill2(m_SkillBossID);
	//
	ProcessShortKey1(m_ShortcutKeySelect1);
	ProcessShortKey2(m_ShortcutKeySelect2);
	//
	ProcessShortCut1(m_ShortcutOptSelect1);
	ProcessShortCut2(m_ShortcutOptSelect2);
	//
	m_ShortcutEditBox1.SetIntText(m_nShortcutEditBox1);
	m_ShortcutEditBox2.SetIntText(m_nShortcutEditBox2);
	//
	m_SkillRightCheckBox.CheckButton(m_bSkillRightCheckBox);
	//
	m_NRDoSkillCheckBox.CheckButton(m_bNRDoSkillCheckBox);
}

void KUiAutoPlayFight::OnActive()//göi gi¸ trÞ ui qua core
{
	if(KUiAutoPlay::GetActive() == TRUE)
	{
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_FIGHT_B, m_bFightCheckBox, 0);
		m_nFightRange = m_FightRangeEditBox.GetIntNumber(); //ph¹m vi chiÕn ®Êu
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_FIGHT_V, m_nFightRange, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_FIGHT_S, m_bFightSelect, 0);
		//
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_DISTANCE_B, m_bFightDistance, 0);
		m_nFightDistance = m_FightDistanceEditBox.GetIntNumber(); //ph¹m vi tiÕp cËn
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_DISTANCE_V, m_nFightDistance, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_DISTANCE_S, m_bFightDistanceSelect, 0);
		//
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SELFDEF_B, m_bFightSelfDef, 0);
		m_nFightSelfDefValue = m_FightSelfDefEditBox.GetIntNumber(); //ph¹m vi tù vÖ
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SELFDEF_V, m_nFightSelfDefValue, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SELFDEF_S, m_bFightSelfDefSelect, 0);

		//for (int i = 0; i < enumSel_Support_Magic_Num; i ++)
		//{
		//	ProcessSupportSkill(i, m_CurrentSelMagic[i]);
		//}

		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SUPPORT_SKILL, 1, m_CurrentSelMagic[enumSel_Support_Magic1]);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SUPPORT_SKILL, 2, m_CurrentSelMagic[enumSel_Support_Magic2]);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SUPPORT_SKILL, 3, m_CurrentSelMagic[enumSel_Support_Magic3]);

		//ProcessLeftSkill(m_nLeftSkillID);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SUPPORT_SKILL, 4, m_nLeftSkillID);
		//ProcessRightSkill(m_nRightSkillID);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SUPPORT_SKILL, 5, m_nRightSkillID);
		//ProcessAuraSkill1(m_nSkillSelfDefID);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SUPPORT_SKILL, 6, m_nSkillSelfDefID);
		//ProcessAuraSkill2(m_SkillBossID);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SUPPORT_SKILL, 7, m_SkillBossID);
	
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SHORTKEY, 1, m_ShortcutKeySelect1);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SHORTKEY, 2, m_ShortcutKeySelect2);

		m_nShortcutEditBox1 = m_ShortcutEditBox1.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SHORTCUT_EDIT, 1, m_nShortcutEditBox1);
		//
		m_nShortcutEditBox2 = m_ShortcutEditBox2.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SHORTCUT_EDIT, 2, m_nShortcutEditBox2);

		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SHORTCUT, 1, m_ShortcutOptSelect1);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SHORTCUT, 2, m_ShortcutOptSelect2);

		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_USKILL_RIGHT_B, m_bSkillRightCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_NR_DO_SKILL_B, m_bNRDoSkillCheckBox, 0);
	}

	SaveConfig();
}
//tù ®¸nh
void KUiAutoPlayFight::PopupFightOptionMenu()
{
	int nActionDataCount = sizeof(g_ArraySelectFightOption) / 32;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;

	for (int i = 0; i < nActionDataCount; i++)
	{
		if ((i == F_Auto) || (i == F_IsRide) || (i == F_IsNotRide))
		{
			strncpy(pSelUnitMenu->Items[i].szData, g_ArraySelectFightOption[i], 63);
			pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
			pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->nNumItem++;
		}
	}
	int x, y;
	m_FightMenuDownBtn.GetAbsolutePos(&x, &y);
	pSelUnitMenu->nX = x;
	pSelUnitMenu->nY = y;
	KPopupMenu::Popup(pSelUnitMenu, this, SELECT_TYPE_FIGHT_OPTION);
}

void KUiAutoPlayFight::ProcessFightOption(BYTE btSelect)
{	
	m_bFightSelect = btSelect;
	m_FightOptionTxt.SetText(g_ArraySelectFightOption[m_bFightSelect]);	
}

void KUiAutoPlayFight::PopupFightDistanceMenu()	//tiÕp cËn
{
	int nActionDataCount = sizeof(g_ArraySelectFightDistanceOption) / 32;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;

	for (int i = 0; i < nActionDataCount; i++)
	{
		if ((i == D_BoQua) || (i == D_TranhBoss) || (i == D_UuTienBoss) || (i == D_ChiDanhBoss))
		{
			strncpy(pSelUnitMenu->Items[i].szData, g_ArraySelectFightDistanceOption[i], 63);
			pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
			pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->nNumItem++;
		}
	}
	int x, y;
	m_FightMenuDistanceDownBtn.GetAbsolutePos(&x, &y);
	pSelUnitMenu->nX = x;
	pSelUnitMenu->nY = y;
	KPopupMenu::Popup(pSelUnitMenu, this, SELECT_TYPE_FIGHT_DISTANCE);
}

void KUiAutoPlayFight::ProcessFightDistance(BYTE btSelect)
{	
	m_bFightDistanceSelect = btSelect;
	m_FightDistanceOptionTxt.SetText(g_ArraySelectFightDistanceOption[m_bFightDistanceSelect]);	
}
//tù vÖ
void KUiAutoPlayFight::PopupFightSelfDefMenu()
{
	int nActionDataCount = sizeof(g_ArraySelectFightSelfDefOption) / 32;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;

	for (int i = 0; i < nActionDataCount; i++)
	{
		if ((i == D_TranhXa) || (i == D_ThoDiaPhu) || (i == D_ThoatGame) || (i == D_DanhTra))
		{
			strncpy(pSelUnitMenu->Items[i].szData, g_ArraySelectFightSelfDefOption[i], 63);
			pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
			pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->nNumItem++;
		}
	}
	int x, y;
	m_FightMenuSelfDefDownBtn.GetAbsolutePos(&x, &y);
	pSelUnitMenu->nX = x;
	pSelUnitMenu->nY = y;
	KPopupMenu::Popup(pSelUnitMenu, this, SELECT_TYPE_FIGHT_SELFDEF);
}

void KUiAutoPlayFight::ProcessFightSelfDef(BYTE btSelect)
{	
	m_bFightSelfDefSelect = btSelect;
	m_FightSelfDefOptionTxt.SetText(g_ArraySelectFightSelfDefOption[m_bFightSelfDefSelect]);	
}

void KUiAutoPlayFight::PopupSupportSkill(int nIndex)
{
	int nActionDataCount = g_pCoreShell->FindSkillInfo(0,0) + 1;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	for (int i = 0; i < nActionDataCount; i++)
	{
		if (i == 0)
		{
			strncpy(pSelUnitMenu->Items[i].szData, MSG_NON_SETTINGS, sizeof(MSG_NON_SETTINGS));
			pSelUnitMenu->Items[i].uID = 0;
		}
		else
		{
			int _nID = g_pCoreShell->FindSkillInfo(0,i);
			g_pCoreShell->GetSkillName(_nID, pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->Items[i].uID = _nID;
		}
		pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
		pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
		pSelUnitMenu->nNumItem++;
	}

	int nX = 0, nY = 0;
	switch (nIndex)
	{
	case enumMagic_ID1:
		m_BuffSkillMenuDown1Btn.GetAbsolutePos(&nX, &nY);
		pSelUnitMenu->nX = nX;
		pSelUnitMenu->nY = nY;
		KPopupMenu::Popup(pSelUnitMenu, this, SELECT_TYPE_FIGHT_SUPPORT1);
		break;
	case enumMagic_ID2:
		m_BuffSkillMenuDown2Btn.GetAbsolutePos(&nX, &nY);
		pSelUnitMenu->nX = nX;
		pSelUnitMenu->nY = nY;
		KPopupMenu::Popup(pSelUnitMenu, this, SELECT_TYPE_FIGHT_SUPPORT2);
		break;
	case enumMagic_ID3:
		m_BuffSkillMenuDown3Btn.GetAbsolutePos(&nX, &nY);
		pSelUnitMenu->nX = nX;
		pSelUnitMenu->nY = nY;
		KPopupMenu::Popup(pSelUnitMenu, this, SELECT_TYPE_FIGHT_SUPPORT3);
		break;		
	}
}

void KUiAutoPlayFight::ProcessSupportSkill(int nIndex, int nID)
{
	switch (nIndex)
	{
	case enumMagic_ID1:
		if (nID > 0)
		{
			m_CurrentSelMagic[enumSel_Support_Magic1] = nID;
			char Name[128];
			g_pCoreShell->GetSkillName(nID, Name);
			m_BuffSkillTxt1.SetText(Name);
		}
		else
		{
			m_CurrentSelMagic[enumSel_Support_Magic1] = 0;
			m_BuffSkillTxt1.SetText(MSG_NON_SETTINGS);
		}	
		//g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SUPPORT_SKILL, 1, m_CurrentSelMagic[enumSel_Support_Magic1]);
		break;
	case enumMagic_ID2:
		if (nID > 0)
		{
			m_CurrentSelMagic[enumSel_Support_Magic2] = nID;
			char Name[128];
			g_pCoreShell->GetSkillName(nID, Name);
			m_BuffSkillTxt2.SetText(Name);
		}
		else
		{
			m_CurrentSelMagic[enumSel_Support_Magic2] = 0;
			m_BuffSkillTxt2.SetText(MSG_NON_SETTINGS);
		}	
		//g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SUPPORT_SKILL, 2, m_CurrentSelMagic[enumSel_Support_Magic2]);
		break;
	case enumMagic_ID3:
		if (nID > 0)
		{
			m_CurrentSelMagic[enumSel_Support_Magic3] = nID;
			char Name[128];
			g_pCoreShell->GetSkillName(nID, Name);
			m_BuffSkillTxt3.SetText(Name);
		}
		else
		{
			m_CurrentSelMagic[enumSel_Support_Magic3] = 0;
			m_BuffSkillTxt3.SetText(MSG_NON_SETTINGS);
		}	
		//g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SUPPORT_SKILL, 3, m_CurrentSelMagic[enumSel_Support_Magic3]);
		break;		
	}
}

void KUiAutoPlayFight::PopupLeftSkill()
{
	int nActionDataCount = g_pCoreShell->FindSkillInfo(1,0) + 1;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	for (int i = 0; i < nActionDataCount; i++)
	{
		if (i == 0)
		{
			strncpy(pSelUnitMenu->Items[i].szData, MSG_NON_SETTINGS, sizeof(MSG_NON_SETTINGS));
			pSelUnitMenu->Items[i].uID = 0;
		}
		else
		{
			int _nID = g_pCoreShell->FindSkillInfo(1,i);
			g_pCoreShell->GetSkillName(_nID, pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->Items[i].uID = _nID;
		}
		pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
		pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
		pSelUnitMenu->nNumItem++;
	}

	int nX = 0, nY = 0;
	m_LeftSkillMenuDown.GetAbsolutePos(&nX, &nY);
	pSelUnitMenu->nX = nX;
	pSelUnitMenu->nY = nY;
	KPopupMenu::Popup(pSelUnitMenu, this, SELECT_TYPE_FIGHT_SKILLLEFT);
}

void KUiAutoPlayFight::ProcessLeftSkill(int nID)
{
	if (nID > 0)
	{
		m_nLeftSkillID = nID;
		char Name[128];
		g_pCoreShell->GetSkillName(nID, Name);
		m_LeftSkillTxt.SetText(Name);
	}
	else
	{
		m_nLeftSkillID = 0;
		m_LeftSkillTxt.SetText(MSG_NON_SETTINGS);
	}	
	//g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SUPPORT_SKILL, 4, m_nLeftSkillID);
}

void KUiAutoPlayFight::PopupRightSkill()
{
	int nActionDataCount = g_pCoreShell->FindSkillInfo(3,0) + 1;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	for (int i = 0; i < nActionDataCount; i++)
	{
		if (i == 0)
		{
			strncpy(pSelUnitMenu->Items[i].szData, MSG_NON_SETTINGS, sizeof(MSG_NON_SETTINGS));
			pSelUnitMenu->Items[i].uID = 0;
		}
		else
		{
			int _nID = g_pCoreShell->FindSkillInfo(3,i);
			g_pCoreShell->GetSkillName(_nID, pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->Items[i].uID = _nID;
		}
		pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
		pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
		pSelUnitMenu->nNumItem++;
	}

	int nX = 0, nY = 0;
	m_RightSkillMenuDown.GetAbsolutePos(&nX, &nY);
	pSelUnitMenu->nX = nX;
	pSelUnitMenu->nY = nY;
	KPopupMenu::Popup(pSelUnitMenu, this, SELECT_TYPE_FIGHT_SKILLRIGHT);
}

void KUiAutoPlayFight::ProcessRightSkill(int nID)
{
	if (nID > 0)
	{
		m_nRightSkillID = nID;
		char Name[128];
		g_pCoreShell->GetSkillName(nID, Name);
		m_RightSkillTxt.SetText(Name);
	}
	else
	{
		m_nRightSkillID = 0;
		m_RightSkillTxt.SetText(MSG_NON_SETTINGS);
	}	
	//g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SUPPORT_SKILL, 5, m_nRightSkillID);
}

void KUiAutoPlayFight::PopupAuraSkill1()
{
	int nActionDataCount = g_pCoreShell->FindSkillInfo(3,0) + 1;//tÊt c¶ skill
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	for (int i = 0; i < nActionDataCount; i++)
	{
		if (i == 0)
		{
			strncpy(pSelUnitMenu->Items[i].szData, MSG_NON_SETTINGS, sizeof(MSG_NON_SETTINGS));
			pSelUnitMenu->Items[i].uID = 0;
		}
		else
		{
			int _nID = g_pCoreShell->FindSkillInfo(3,i);
			g_pCoreShell->GetSkillName(_nID, pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->Items[i].uID = _nID;
		}
		pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
		pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
		pSelUnitMenu->nNumItem++;
	}

	int nX = 0, nY = 0;
	m_AuraSkillMenuDown1.GetAbsolutePos(&nX, &nY);
	pSelUnitMenu->nX = nX;
	pSelUnitMenu->nY = nY;
	KPopupMenu::Popup(pSelUnitMenu, this, SELECT_TYPE_FIGHT_AURA1);
}

void KUiAutoPlayFight::ProcessAuraSkill1(int nID)
{
	if (nID > 0)
	{
		m_nSkillSelfDefID = nID;
		char Name[128];
		g_pCoreShell->GetSkillName(nID, Name);
		m_AuraSkillTxt1.SetText(Name);
	}
	else
	{
		m_nSkillSelfDefID = 0;
		m_AuraSkillTxt1.SetText(MSG_NON_SETTINGS);
	}	
	//g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SUPPORT_SKILL, 6, m_nSkillSelfDefID);
}

void KUiAutoPlayFight::PopupAuraSkill2()
{
	int nActionDataCount = g_pCoreShell->FindSkillInfo(3,0) + 1;//tÊt c¶ skill
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	for (int i = 0; i < nActionDataCount; i++)
	{
		if (i == 0)
		{
			strncpy(pSelUnitMenu->Items[i].szData, MSG_NON_SETTINGS, sizeof(MSG_NON_SETTINGS));
			pSelUnitMenu->Items[i].uID = 0;
		}
		else
		{
			int _nID = g_pCoreShell->FindSkillInfo(3,i);
			g_pCoreShell->GetSkillName(_nID, pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->Items[i].uID = _nID;
		}
		pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
		pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
		pSelUnitMenu->nNumItem++;
	}

	int nX = 0, nY = 0;
	m_AuraSkillMenuDown2.GetAbsolutePos(&nX, &nY);
	pSelUnitMenu->nX = nX;
	pSelUnitMenu->nY = nY;
	KPopupMenu::Popup(pSelUnitMenu, this, SELECT_TYPE_FIGHT_AURA2);
}

void KUiAutoPlayFight::ProcessAuraSkill2(int nID)
{
	if (nID > 0)
	{
		m_SkillBossID = nID;
		char Name[128];
		g_pCoreShell->GetSkillName(nID, Name);
		m_AuraSkillTxt2.SetText(Name);
	}
	else
	{
		m_SkillBossID = 0;
		m_AuraSkillTxt2.SetText(MSG_NON_SETTINGS);
	}	
	//g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_SUPPORT_SKILL, 7, m_SkillBossID);
}

void KUiAutoPlayFight::PopupShortKey1Menu()
{
	int nActionDataCount = sizeof(g_ArraySelectShortKeyOption) / 32;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;

	for (int i = 0; i < nActionDataCount; i++)
	{
		if ((i == 0) || (i == 1) || (i == 2) || (i == 3) || (i == 4) || (i == 5))
		{
			strncpy(pSelUnitMenu->Items[i].szData, g_ArraySelectShortKeyOption[i], 63);
			pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
			pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->nNumItem++;
		}
	}
	int x, y;
	m_ShortcutKeyMenuDown1.GetAbsolutePos(&x, &y);
	pSelUnitMenu->nX = x;
	pSelUnitMenu->nY = y;
	KPopupMenu::Popup(pSelUnitMenu, this, SELECT_TYPE_FIGHT_SHORTKEY1);
}

void KUiAutoPlayFight::ProcessShortKey1(BYTE btSelect)
{
	m_ShortcutKeySelect1 = btSelect;
	m_ShortcutKeyTxt1.SetText(g_ArraySelectShortKeyOption[m_ShortcutKeySelect1]);	
}

void KUiAutoPlayFight::PopupShortKey2Menu()
{
	int nActionDataCount = sizeof(g_ArraySelectShortKeyOption) / 32;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;

	for (int i = 0; i < nActionDataCount; i++)
	{
		if ((i == 0) || (i == 1) || (i == 2) || (i == 3) || (i == 4) || (i == 5))
		{
			strncpy(pSelUnitMenu->Items[i].szData, g_ArraySelectShortKeyOption[i], 63);
			pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
			pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->nNumItem++;
		}
	}
	int x, y;
	m_ShortcutKeyMenuDown2.GetAbsolutePos(&x, &y);
	pSelUnitMenu->nX = x;
	pSelUnitMenu->nY = y;
	KPopupMenu::Popup(pSelUnitMenu, this, SELECT_TYPE_FIGHT_SHORTKEY2);
}

void KUiAutoPlayFight::ProcessShortKey2(BYTE btSelect)
{	
	m_ShortcutKeySelect2 = btSelect;
	m_ShortcutKeyTxt2.SetText(g_ArraySelectShortKeyOption[m_ShortcutKeySelect2]);	
}

void KUiAutoPlayFight::PopupShortCut1Menu()
{
	int nActionDataCount = sizeof(g_ArraySelectShortCut1Option) / 32;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;

	for (int i = 0; i < nActionDataCount; i++)
	{
		if ((i == 0) || (i == 1) || (i == 2) || (i == 3) || (i == 4) || (i == 5))
		{
			strncpy(pSelUnitMenu->Items[i].szData, g_ArraySelectShortCut1Option[i], 63);
			pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
			pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->nNumItem++;
		}
	}
	int x, y;
	m_ShortcutOptMenuDown1.GetAbsolutePos(&x, &y);
	pSelUnitMenu->nX = x;
	pSelUnitMenu->nY = y;
	KPopupMenu::Popup(pSelUnitMenu, this, SELECT_TYPE_FIGHT_SHORTCUT1);
}

void KUiAutoPlayFight::ProcessShortCut1(BYTE btSelect)
{
	m_ShortcutOptSelect1 = btSelect;
	m_ShortcutOptTxt1.SetText(g_ArraySelectShortCut1Option[m_ShortcutOptSelect1]);
}

void KUiAutoPlayFight::PopupShortCut2Menu()
{
	int nActionDataCount = sizeof(g_ArraySelectShortCut2Option) / 32;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;

	for (int i = 0; i < nActionDataCount; i++)
	{
		if ((i == 0) || (i == 1) || (i == 2) || (i == 3) || (i == 4) || (i == 5))
		{
			strncpy(pSelUnitMenu->Items[i].szData, g_ArraySelectShortCut2Option[i], 63);
			pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
			pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->nNumItem++;
		}
	}
	int x, y;
	m_ShortcutOptMenuDown2.GetAbsolutePos(&x, &y);
	pSelUnitMenu->nX = x;
	pSelUnitMenu->nY = y;
	KPopupMenu::Popup(pSelUnitMenu, this, SELECT_TYPE_FIGHT_SHORTCUT2);
}

void KUiAutoPlayFight::ProcessShortCut2(BYTE btSelect)
{
	m_ShortcutOptSelect2 = btSelect;
	m_ShortcutOptTxt2.SetText(g_ArraySelectShortCut2Option[m_ShortcutOptSelect2]);	
}

void KUiAutoPlayFight::SaveConfig()
{
	KIniFile* pConfigFile = NULL;
	pConfigFile = g_UiBase.GetAutoSettingFile();
	if (pConfigFile)
	{
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_bFightCheckBox", m_bFightCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_nFightRange", m_nFightRange);
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_bFightSelect", m_bFightSelect);
		//
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_bFightDistance", m_bFightDistance);
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_nFightDistance", m_nFightDistance);
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_bFightDistanceSelect", m_bFightDistanceSelect);
		//
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_bFightSelfDef", m_bFightSelfDef);
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_nFightSelfDefValue", m_nFightSelfDefValue);
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_bFightSelfDefSelect", m_bFightSelfDefSelect);
		//
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_CurrentSelMagic1", m_CurrentSelMagic[enumSel_Support_Magic1]);
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_CurrentSelMagic2", m_CurrentSelMagic[enumSel_Support_Magic2]);
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_CurrentSelMagic3", m_CurrentSelMagic[enumSel_Support_Magic3]);
		//
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_nLeftSkillID", m_nLeftSkillID);
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_nRightSkillID", m_nRightSkillID);
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_nSkillSelfDefID", m_nSkillSelfDefID);
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_SkillBossID", m_SkillBossID);
		//
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_ShortcutKeySelect1", m_ShortcutKeySelect1);
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_ShortcutKeySelect2", m_ShortcutKeySelect2);
		//
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_nShortcutEditBox1", m_nShortcutEditBox1);
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_nShortcutEditBox2", m_nShortcutEditBox2);
		//
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_ShortcutOptSelect1", m_ShortcutOptSelect1);
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_ShortcutOptSelect2", m_ShortcutOptSelect2);
		//
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_bSkillRightCheckBox", m_bSkillRightCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayFight", "m_bNRDoSkillCheckBox", m_bNRDoSkillCheckBox);
	}
	g_UiBase.CloseAutoSettingFile(true);
}

void KUiAutoPlayFight::LoadConfig()
{
	KIniFile* pConfigFile = NULL;
	pConfigFile = g_UiBase.GetAutoSettingFile();
	int nTempValue = 0;
	if (pConfigFile)
	{
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_bFightCheckBox", 1, (int*)(&nTempValue)); m_bFightCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_nFightRange", 600, (int*)(&nTempValue)); m_nFightRange = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_bFightSelect", 0, (int*)(&nTempValue)); m_bFightSelect = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_bFightDistance", 1, (int*)(&nTempValue)); m_bFightDistance = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_nFightDistance", 75, (int*)(&nTempValue)); m_nFightDistance = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_bFightDistanceSelect", 0, (int*)(&nTempValue)); m_bFightDistanceSelect = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_bFightSelfDef", 0, (int*)(&nTempValue)); m_bFightSelfDef = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_nFightSelfDefValue", 600, (int*)(&nTempValue)); m_nFightSelfDefValue = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_bFightSelfDefSelect", 0, (int*)(&nTempValue)); m_bFightSelfDefSelect = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_CurrentSelMagic1", 0, (int*)(&nTempValue)); m_CurrentSelMagic[enumSel_Support_Magic1] = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_CurrentSelMagic2", 0, (int*)(&nTempValue)); m_CurrentSelMagic[enumSel_Support_Magic2] = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_CurrentSelMagic3", 0, (int*)(&nTempValue)); m_CurrentSelMagic[enumSel_Support_Magic3] = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_nLeftSkillID", 0, (int*)(&nTempValue)); m_nLeftSkillID = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_nRightSkillID", 0, (int*)(&nTempValue)); m_nRightSkillID = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_nSkillSelfDefID", 0, (int*)(&nTempValue)); m_nSkillSelfDefID = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_SkillBossID", 0, (int*)(&nTempValue)); m_SkillBossID = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_ShortcutKeySelect1", 0, (int*)(&nTempValue)); m_ShortcutKeySelect1 = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_ShortcutKeySelect2", 0, (int*)(&nTempValue)); m_ShortcutKeySelect2 = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_nShortcutEditBox1", 0, (int*)(&nTempValue)); m_nShortcutEditBox1 = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_nShortcutEditBox2", 0, (int*)(&nTempValue)); m_nShortcutEditBox2 = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_ShortcutOptSelect1", 0, (int*)(&nTempValue)); m_ShortcutOptSelect1 = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_ShortcutOptSelect2", 0, (int*)(&nTempValue)); m_ShortcutOptSelect2 = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_bSkillRightCheckBox", 0, (int*)(&nTempValue)); m_bSkillRightCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayFight", "m_bNRDoSkillCheckBox", 1, (int*)(&nTempValue)); m_bNRDoSkillCheckBox = nTempValue;
	}
	g_UiBase.CloseAutoSettingFile(true);
}

//=KUiAutoPlayRestore==========================================================================
char g_ArrayFCellOption[][64] = 
{
	"HÕt 1x1 «",
	"HÕt 1x2 «",
	"HÕt 1x3 «",
	"HÕt 1x4 «",
	"HÕt 2x1 «",
	"HÕt 2x2 «",
	"HÕt 2x3 «",
	"HÕt 2x4 «", 
};

KUiAutoPlayRestore::KUiAutoPlayRestore()
{
	m_bReHPCheckBox = TRUE;
	m_nReHPEditBox1 = 100;
	m_nReHPEditBox2 = 200;
	m_nReHPEditBox3 = 3000;

	m_bReMPCheckBox = TRUE;
	m_nReMPEditBox1 = 100;
	m_nReMPEditBox2 = 200;
	m_nReMPEditBox3 = 3000;

	m_bTownHPVCheckBox = TRUE;
	m_nTownHPVEditBox = 100;

	m_bTownMPVCheckBox = TRUE;
	m_nTownMPVEditBox = 100;

	m_bTownHPNCheckBox = TRUE;
	m_bTownMPNCheckBox = TRUE;

	m_bTownFCellCheckBox = FALSE;
	m_nTownFCellTxtSelect = 0;

	m_bTowMoneyCheckBox = TRUE;
	m_nTownMoneyEditBox = 20;

	m_bTownAbradeCheckBox = TRUE;
	m_nTownAbradeEditBox = 5;

	m_bUseHPCheckBox = FALSE;
	m_bUseAntiToxicCheckBox = TRUE;
	m_bUseExpCheckBox = FALSE;
	m_bUseSkillCheckBox = FALSE;

	m_bBuffCheckBox = FALSE;
	m_nBuffEditBox = 60;
	m_bBuffTeamCheckBox = FALSE;

	m_bOpenBagHPCheckBox = TRUE;
	m_nOpenBagHPEditBox = 6;//cßn 6 b×nh tù më tói m¸u

	m_bRingTDPCheckBox = FALSE;
}

void KUiAutoPlayRestore::Initialize()
{
	AddChild(&m_ReHPCheckBox);
	AddChild(&m_ReHPEditBox1);
	AddChild(&m_ReHPEditBox2);
	AddChild(&m_ReHPEditBox3);
	//
	AddChild(&m_ReMPCheckBox);
	AddChild(&m_ReMPEditBox1);
	AddChild(&m_ReMPEditBox2);
	AddChild(&m_ReMPEditBox3);
	//
	AddChild(&m_TownHPVCheckBox);
	AddChild(&m_TownHPVEditBox);
	//
	AddChild(&m_TownMPVCheckBox);
	AddChild(&m_TownMPVEditBox);
	//
	AddChild(&m_TownHPNCheckBox);
	AddChild(&m_TownMPNCheckBox);
	AddChild(&m_TownFCellCheckBox);
	//
	AddChild(&m_TownFCellTxt);
	AddChild(&m_TownFCellMenuDown);
	//
	AddChild(&m_TowMoneyCheckBox);
	AddChild(&m_TownMoneyEditBox);
	//
	AddChild(&m_TownAbradeCheckBox);
	AddChild(&m_TownAbradeEditBox);
	//
	AddChild(&m_UseHPCheckBox);
	AddChild(&m_UseAntiToxicCheckBox);
	AddChild(&m_UseExpCheckBox);
	AddChild(&m_UseSkillCheckBox);
	//
	AddChild(&m_BuffCheckBox);
	AddChild(&m_BuffEditBox);
	AddChild(&m_BuffTeamCheckBox);
	//
	AddChild(&m_OpenBagHPCheckBox);
	AddChild(&m_OpenBagHPEditBox);
	//
	AddChild(&m_RingTDPCheckBox);
	
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

void KUiAutoPlayRestore::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;

	sprintf(Buff, "%s\\%s", pScheme, SCHEME_RESTORE_INI);

	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		m_ReHPCheckBox.Init(&Ini, "ReHPCheckBox");
		m_ReHPEditBox1.Init(&Ini, "ReHPEditBox1");
		m_ReHPEditBox2.Init(&Ini, "ReHPEditBox2");
		m_ReHPEditBox3.Init(&Ini, "ReHPEditBox3");
		//
		m_ReMPCheckBox.Init(&Ini, "ReMPCheckBox");
		m_ReMPEditBox1.Init(&Ini, "ReMPEditBox1");
		m_ReMPEditBox2.Init(&Ini, "ReMPEditBox2");
		m_ReMPEditBox3.Init(&Ini, "ReMPEditBox3");
		//
		m_TownHPVCheckBox.Init(&Ini, "TownHPVCheckBox");
		m_TownHPVEditBox.Init(&Ini, "TownHPVEditBox");
		//
		m_TownMPVCheckBox.Init(&Ini, "TownMPVCheckBox");
		m_TownMPVEditBox.Init(&Ini, "TownMPVEditBox");
		//
		m_TownHPNCheckBox.Init(&Ini, "TownHPNCheckBox");
		m_TownMPNCheckBox.Init(&Ini, "TownMPNCheckBox");
		m_TownFCellCheckBox.Init(&Ini, "TownFCellCheckBox");
		//
		m_TownFCellTxt.Init(&Ini, "TownFCellTxt");
		m_TownFCellMenuDown.Init(&Ini, "TownFCellMenuDown");
		//
		m_TowMoneyCheckBox.Init(&Ini, "TowMoneyCheckBox");
		m_TownMoneyEditBox.Init(&Ini, "TownMoneyEditBox");
		//
		m_TownAbradeCheckBox.Init(&Ini, "TownAbradeCheckBox");
		m_TownAbradeEditBox.Init(&Ini, "TownAbradeEditBox");
		//
		m_UseHPCheckBox.Init(&Ini, "UseHPCheckBox");
		m_UseAntiToxicCheckBox.Init(&Ini, "UseAntiToxicCheckBox");
		m_UseExpCheckBox.Init(&Ini, "UseExpCheckBox");
		m_UseSkillCheckBox.Init(&Ini, "UseSkillCheckBox");
		//
		m_BuffCheckBox.Init(&Ini, "BuffCheckBox");
		m_BuffEditBox.Init(&Ini, "BuffEditBox");
		m_BuffTeamCheckBox.Init(&Ini, "BuffTeamCheckBox");
		//
		m_OpenBagHPCheckBox.Init(&Ini, "OpenBagHPCheckBox");
		m_OpenBagHPEditBox.Init(&Ini, "OpenBagHPEditBox");
		//
		m_RingTDPCheckBox.Init(&Ini, "RingTDPCheckBox");
	}
	UpdateData();
}

void KUiAutoPlayRestore::UpdateData()
{
	LoadConfig();

	m_ReHPCheckBox.CheckButton(m_bReHPCheckBox);
	m_ReHPEditBox1.SetIntText(m_nReHPEditBox1);
	m_ReHPEditBox2.SetIntText(m_nReHPEditBox2);
	m_ReHPEditBox3.SetIntText(m_nReHPEditBox3);
	//
	m_ReMPCheckBox.CheckButton(m_bReMPCheckBox);
	m_ReMPEditBox1.SetIntText(m_nReMPEditBox1);
	m_ReMPEditBox2.SetIntText(m_nReMPEditBox2);
	m_ReMPEditBox3.SetIntText(m_nReMPEditBox3);
	//
	m_TownHPVCheckBox.CheckButton(m_bTownHPVCheckBox);
	m_TownHPVEditBox.SetIntText(m_nTownHPVEditBox);
	//
	m_TownMPVCheckBox.CheckButton(m_bTownMPVCheckBox);
	m_TownMPVEditBox.SetIntText(m_nTownMPVEditBox);
	//
	m_TownHPNCheckBox.CheckButton(m_bTownHPNCheckBox);
	m_TownMPNCheckBox.CheckButton(m_bTownMPNCheckBox);
	//
	m_TownFCellCheckBox.CheckButton(m_bTownFCellCheckBox);
	ProcessFCell(m_nTownFCellTxtSelect);
	//
	m_TowMoneyCheckBox.CheckButton(m_bTowMoneyCheckBox);
	m_TownMoneyEditBox.SetIntText(m_nTownMoneyEditBox);
	//
	m_TownAbradeCheckBox.CheckButton(m_bTownAbradeCheckBox);
	m_TownAbradeEditBox.SetIntText(m_nTownAbradeEditBox);
	//
	m_UseHPCheckBox.CheckButton(m_bUseHPCheckBox);
	m_UseAntiToxicCheckBox.CheckButton(m_bUseAntiToxicCheckBox);
	m_UseExpCheckBox.CheckButton(m_bUseExpCheckBox);
	m_UseSkillCheckBox.CheckButton(m_bUseSkillCheckBox);
	//
	m_BuffCheckBox.CheckButton(m_bBuffCheckBox);
	m_BuffEditBox.SetIntText(m_nBuffEditBox);
	m_BuffTeamCheckBox.CheckButton(m_bBuffTeamCheckBox);
	//
	m_OpenBagHPCheckBox.CheckButton(m_bOpenBagHPCheckBox);
	m_OpenBagHPEditBox.SetIntText(m_nOpenBagHPEditBox);
	//
	m_RingTDPCheckBox.CheckButton(m_bRingTDPCheckBox);
}

int KUiAutoPlayRestore::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_ReHPCheckBox)//b¬m sl
		{
			m_bReHPCheckBox = !m_bReHPCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_ReMPCheckBox)//b¬m nl
		{
			m_bReMPCheckBox = !m_bReMPCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TownHPVCheckBox)//phï sl <
		{
			m_bTownHPVCheckBox = !m_bTownHPVCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TownMPVCheckBox)//phï nl <
		{
			m_bTownMPVCheckBox = !m_bTownMPVCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TownHPNCheckBox)//phï hÕt b×nh sl
		{
			m_bTownHPNCheckBox = !m_bTownHPNCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TownMPNCheckBox)//phï hÕt b×nh nl
		{
			m_bTownMPNCheckBox = !m_bTownMPNCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TownFCellCheckBox)//phï hÕt kho¶ng trèng
		{
			m_bTownFCellCheckBox = !m_bTownFCellCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TowMoneyCheckBox)
		{
			m_bTowMoneyCheckBox = !m_bTowMoneyCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TownAbradeCheckBox)
		{
			m_bTownAbradeCheckBox = !m_bTownAbradeCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TownFCellMenuDown)
		{
			PopupFCellMenu();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_UseHPCheckBox)
		{
			m_bUseHPCheckBox = !m_bUseHPCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_UseAntiToxicCheckBox)
		{
			m_bUseAntiToxicCheckBox = !m_bUseAntiToxicCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_UseExpCheckBox)
		{
			m_bUseExpCheckBox = !m_bUseExpCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_UseSkillCheckBox)
		{
			m_bUseSkillCheckBox = !m_bUseSkillCheckBox;
		}				
		else if (uParam == (unsigned int)(KWndWindow*)&m_BuffCheckBox)
		{
			m_bBuffCheckBox = !m_bBuffCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_BuffTeamCheckBox)
		{
			m_bBuffTeamCheckBox = !m_bBuffTeamCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_OpenBagHPCheckBox)
		{
			m_bOpenBagHPCheckBox = !m_bOpenBagHPCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_RingTDPCheckBox)
		{
			m_bRingTDPCheckBox = !m_bRingTDPCheckBox;
		}
		OnActive();
		break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if (HIWORD(nParam) == 1 && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessFCell(BYTE(nParam));
			}
		}
		OnActive();
		break;
	default:
		nRet = KWndPage::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}

void KUiAutoPlayRestore::OnActive()
{
	if(KUiAutoPlay::GetActive() == TRUE)
	{
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_HP_B, m_bReHPCheckBox, 0);//b¬m sl
		m_nReHPEditBox1 = m_ReHPEditBox1.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_HP_1, m_nReHPEditBox1, 0);
		m_nReHPEditBox2 = m_ReHPEditBox2.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_HP_2, m_nReHPEditBox2, 0);
		m_nReHPEditBox3 = m_ReHPEditBox3.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_HP_3, m_nReHPEditBox3, 0);

		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MP_B, m_bReMPCheckBox, 0);//b¬m nl
		m_nReMPEditBox1 = m_ReMPEditBox1.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MP_1, m_nReMPEditBox1, 0);
		m_nReMPEditBox2 = m_ReMPEditBox2.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MP_2, m_nReMPEditBox2, 0);
		m_nReMPEditBox3 = m_ReMPEditBox3.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MP_3, m_nReMPEditBox3, 0);
		

		m_nTownHPVEditBox = m_TownHPVEditBox.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_TP_1, m_bTownHPVCheckBox, m_nTownHPVEditBox);
		m_nTownMPVEditBox = m_TownMPVEditBox.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_TP_2, m_bTownMPVCheckBox, m_nTownMPVEditBox);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_TP_3, m_bTownHPNCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_TP_4, m_bTownMPNCheckBox, 0);

		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_TP_5, m_bTownFCellCheckBox, m_nTownFCellTxtSelect);
		//
		m_nTownMoneyEditBox = m_TownMoneyEditBox.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_TP_6, m_bTowMoneyCheckBox, m_nTownMoneyEditBox);
		//
		m_nTownAbradeEditBox = m_TownAbradeEditBox.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_TP_7, m_bTownAbradeCheckBox, m_nTownAbradeEditBox);
		//
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_USEM_FCELL, m_bUseHPCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_ANTI_TOXIC, m_bUseAntiToxicCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_UX2ITEM, m_bUseExpCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_UX2SKILL, m_bUseSkillCheckBox, 0);
		//
		m_nBuffEditBox = m_BuffEditBox.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_NMBUFF, m_bBuffCheckBox, m_nBuffEditBox);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_BUFF_TEAM, m_bBuffTeamCheckBox, 0);
		//
		m_nOpenBagHPEditBox = m_OpenBagHPEditBox.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_OPENBAGHP, m_bOpenBagHPCheckBox, m_nOpenBagHPEditBox);
		//
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_RING_TDP, m_bRingTDPCheckBox, 0);
	}		

	SaveConfig();
}

void KUiAutoPlayRestore::PopupFCellMenu()
{
	int nActionDataCount = sizeof(g_ArrayFCellOption) / 32;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;

	for (int i = 0; i < nActionDataCount; i++)
	{
		if ((i == 0) || (i == 1) || (i == 2) || (i == 3) || (i == 4) || (i == 5) || (i == 6) || (i == 7))
		{
			strncpy(pSelUnitMenu->Items[i].szData, g_ArrayFCellOption[i], 63);
			pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
			pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->nNumItem++;
		}
	}
	int x, y;
	m_TownFCellMenuDown.GetAbsolutePos(&x, &y);
	pSelUnitMenu->nX = x;
	pSelUnitMenu->nY = y;
	KPopupMenu::Popup(pSelUnitMenu, this, 1);
}

void KUiAutoPlayRestore::ProcessFCell(BYTE btSelect)
{
	m_nTownFCellTxtSelect = btSelect;
	m_TownFCellTxt.SetText(g_ArrayFCellOption[m_nTownFCellTxtSelect]);	
}

void KUiAutoPlayRestore::LoadConfig()
{
	KIniFile* pConfigFile = NULL;
	pConfigFile = g_UiBase.GetAutoSettingFile();
	int nTempValue = 0;
	if (pConfigFile)
	{
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_bReHPCheckBox", 1, (int*)(&nTempValue)); m_bReHPCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_nReHPEditBox1", 30, (int*)(&nTempValue)); m_nReHPEditBox1 = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_nReHPEditBox2", 40, (int*)(&nTempValue)); m_nReHPEditBox2 = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_nReHPEditBox3", 2000, (int*)(&nTempValue)); m_nReHPEditBox3 = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_bReMPCheckBox", 1, (int*)(&nTempValue)); m_bReMPCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_nReMPEditBox1", 30, (int*)(&nTempValue)); m_nReMPEditBox1 = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_nReMPEditBox2", 40, (int*)(&nTempValue)); m_nReMPEditBox2 = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_nReMPEditBox3", 2000, (int*)(&nTempValue)); m_nReMPEditBox3 = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_bTownHPVCheckBox", 0, (int*)(&nTempValue)); m_bTownHPVCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_nTownHPVEditBox", 20, (int*)(&nTempValue)); m_nTownHPVEditBox = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_bTownMPVCheckBox", 0, (int*)(&nTempValue)); m_bTownMPVCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_nTownMPVEditBox", 20, (int*)(&nTempValue)); m_nTownMPVEditBox = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_bTownHPNCheckBox", 0, (int*)(&nTempValue)); m_bTownHPNCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_bTownMPNCheckBox", 0, (int*)(&nTempValue)); m_bTownMPNCheckBox = nTempValue;
		//		
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_bTownFCellCheckBox", 0, (int*)(&nTempValue)); m_bTownFCellCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_nTownFCellTxtSelect", 0, (int*)(&nTempValue)); m_nTownFCellTxtSelect = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_bTowMoneyCheckBox", 0, (int*)(&nTempValue)); m_bTowMoneyCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_nTownMoneyEditBox", 20, (int*)(&nTempValue)); m_nTownMoneyEditBox = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_bTownAbradeCheckBox", 0, (int*)(&nTempValue)); m_bTownAbradeCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_nTownAbradeEditBox", 5, (int*)(&nTempValue)); m_nTownAbradeEditBox = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_bUseHPCheckBox", 0, (int*)(&nTempValue)); m_bUseHPCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_bUseAntiToxicCheckBox", 0, (int*)(&nTempValue)); m_bUseAntiToxicCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_bUseExpCheckBox", 0, (int*)(&nTempValue)); m_bUseExpCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_bUseSkillCheckBox", 0, (int*)(&nTempValue)); m_bUseSkillCheckBox = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_bBuffCheckBox", 0, (int*)(&nTempValue)); m_bBuffCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_nBuffEditBox", 80, (int*)(&nTempValue)); m_nBuffEditBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_bBuffTeamCheckBox", 0, (int*)(&nTempValue)); m_bBuffTeamCheckBox = nTempValue;
		//
		//
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_bOpenBagHPCheckBox", 0, (int*)(&nTempValue)); m_bOpenBagHPCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_nOpenBagHPEditBox", 6, (int*)(&nTempValue)); m_nOpenBagHPEditBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayRestore", "m_bRingTDPCheckBox", 0, (int*)(&nTempValue)); m_bRingTDPCheckBox = nTempValue;
	}
	g_UiBase.CloseAutoSettingFile(true);
}

void KUiAutoPlayRestore::SaveConfig()
{
	KIniFile* pConfigFile = NULL;
	pConfigFile = g_UiBase.GetAutoSettingFile();
	if (pConfigFile)
	{
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_bReHPCheckBox", m_bReHPCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_nReHPEditBox1", m_nReHPEditBox1);
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_nReHPEditBox2", m_nReHPEditBox2);
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_nReHPEditBox3", m_nReHPEditBox3);
		//
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_bReMPCheckBox", m_bReMPCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_nReMPEditBox1", m_nReMPEditBox1);
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_nReMPEditBox2", m_nReMPEditBox2);
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_nReMPEditBox3", m_nReMPEditBox3);
		//
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_bTownHPVCheckBox", m_bTownHPVCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_nTownHPVEditBox", m_nTownHPVEditBox);
		//
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_bTownMPVCheckBox", m_bTownMPVCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_nTownMPVEditBox", m_nTownMPVEditBox);
		//
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_bTownHPNCheckBox", m_bTownHPNCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_bTownMPNCheckBox", m_bTownMPNCheckBox);
		//
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_bTownFCellCheckBox", m_bTownFCellCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_nTownFCellTxtSelect", m_nTownFCellTxtSelect);
		//
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_bTowMoneyCheckBox", m_bTowMoneyCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_nTownMoneyEditBox", m_nTownMoneyEditBox);
		//
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_bTownAbradeCheckBox", m_bTownAbradeCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_nTownAbradeEditBox", m_nTownAbradeEditBox);
		//
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_bUseHPCheckBox", m_bUseHPCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_bUseAntiToxicCheckBox", m_bUseAntiToxicCheckBox);
		//
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_bUseExpCheckBox", m_bUseExpCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_bUseSkillCheckBox", m_bUseSkillCheckBox);
		//
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_bBuffCheckBox", m_bBuffCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_nBuffEditBox", m_nBuffEditBox);
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_bBuffTeamCheckBox", m_bBuffTeamCheckBox);
		//
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_bOpenBagHPCheckBox", m_bOpenBagHPCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_nOpenBagHPEditBox", m_nOpenBagHPEditBox);
		pConfigFile->WriteInteger("KUiAutoPlayRestore", "m_bRingTDPCheckBox", m_bRingTDPCheckBox);
	}
	g_UiBase.CloseAutoSettingFile(true);
}

//=KUiAutoPlayPick==========================================================================
char g_ArraySelectPick[][64] = 
{
	"TÊt c¶ vËt phÈm",
	"VËt phÈm 1x1 « ",
	"VËt phÈm 1x2 « ",
	"VËt phÈm 1x3 « ",
	"VËt phÈm 1x4 « ",
	"VËt phÈm 2x1 « ",
	"VËt phÈm 2x2 « ",
	"VËt phÈm 2x3 « ",
	"VËt phÈm 2x4 « ", 
};

char g_ArrayData2_Magic[defMAX_AUTO_FILTERL][2][64] = 
{
	"Sinh lùc(®)", 		"lifemax_v",
	"Néi lùc(®)", 		"manamax_v",
	"ThÓ lùc(®)", 		"staminamax_v",
	"Phôc håi sinh lùc(®)", 	"lifereplenish_v",
	"Phôc håi néi lùc(®)", 	"manareplenish_v",
	"Phôc håi thÓ lùc(®)", 	"staminareplenish_v",
	"Hót sinh lùc(%)", 	"steallifeenhance_p",
	"Hót néi lùc(%)", 	"stealmanaenhance_p",
	"Hót thÓ lùc(%)", 	"stealstaminaenhance_p",
	"Søc m¹nh(®)", 		"strength_v",
	"Th©n ph¸p(®)", 	"dexterity_v",
	"Sinh khÝ(®)", 		"vitality_v",
	"Néi c«ng(®)", 		"energy_v",
	"Phßng thñ(%)", 	"physicsres_p",
	"Kh¸ng ®éc(%)", 	"poisonres_p",
	"Kh¸ng b¨ng(%)", 	"coldres_p",
	"Kh¸ng háa(%)", 	"fireres_p",
	"Kh¸ng l«i(%)", 	"lightingres_p",
	"Kh¸ng tÊt c¶(%)", 	"allres_p",
	"Tèc ®é ch¹y(%)", 	"fastwalkrun_p",
	"Tèc ®é ®¸nh(%)", 	"attackspeed_v",
	"Ph¶n ®ßn cËn(®)", 	"meleedamagereturn_v",
	"STVL ngo¹i(®)", 	"addphysicsdamage_v",
	"B¨ng s¸t ngo¹i(®)", "addcolddamage_v",
	"§éc s¸t ngo¹i(®)", "addpoisondamage_v",
	"STVL ngo¹i(%)", 	"addphysicsdamage_p",
	"CHSTTNL(%)", 		"damage2addmana_p",
	"May m¾n(%)", 		"lucky_v",
	"Bá qua nÐ tr¸nh(%)", "ignoredefense_p",
	"§é chÝnh x¸c(®)", 	"attackrating_v",
	"STLV néi(®)", 		"addphysicsmagic_v",
	"B¨ng s¸t néi(®)", 	"addcoldmagic_v",
	"Háa s¸t néi(®)", 	"addfiremagic_v",
	"L«i s¸t néi(®)", 	"addlightingmagic_v",
	"§éc s¸t néi(®)", 	"addpoisonmagic_v",
	"Gi¶m chËm(%)", 	"freezetimereduce_p",
	"Gi¶m tróng ®éc(%)", "poisontimereduce_p",
	"Gi¶m cho¸ng(%)", 	"stuntimereduce_p",
	"Gi¶m th­¬ng(%)", 	"fasthitrecover_v",		//thêi gian phôc håi
	"Kü n¨ng vèn cã(®)", "allskill_v",
};

#define	MSG_FORMAT_MAGIC				"%s >= %d"
#define	SELECT_TYPE_PICK_OPTION					0x001
#define	SELECT_TYPE_LISTMAGIC						0x002

KUiAutoPlayPick::KUiAutoPlayPick()
{	
	m_bAutoPickCheckBox = TRUE;
	m_bPickAllCheckBox = TRUE;
	m_bPickSpeCheckBox = TRUE;
	m_bNoneFightCheckBox = TRUE;
	m_nPickSelTxtSelect = 0;
	m_nPickOptionTxtSelect = 0;
	memset(m_FilterMagicIndex, 0, sizeof(m_FilterMagicIndex));
	memset(m_cFilterMagic, 0, sizeof(m_cFilterMagic));
	memset(m_FilterMagicV, 0, sizeof(m_FilterMagicV));
	m_bPriceCellCheckBox = FALSE;
	m_nPriceCellEditBox = 100;
	m_bLevelCheckBox = 1;
	m_nLevelEditBox = 2;
	m_bRiAmPeCheckBox = FALSE;
	m_nRiEditBox = 1;
	m_nAmPeEditBox = 10;
	m_bSortBagCheckBox = TRUE;
	m_nSortBagEditBox = 30;//phót tù xÕp ®å
	m_bNPickBackLCheckBox = TRUE;
	m_bFillterItemCheckBox = FALSE;
	m_bKeepPurpleCheckBox = TRUE;
	m_nKeepPurpleEditBox = 6;
}

void KUiAutoPlayPick::Initialize()
{	
	AddChild(&m_AutoPickCheckBox);
	AddChild(&m_AutoPickEditBox);
	AddChild(&m_PickAllCheckBox);
	AddChild(&m_PickSpeCheckBox);
	AddChild(&m_NoneFightCheckBox);
	//
	AddChild(&m_PickSelTxt);
	AddChild(&m_PickSelMenuDown);
	//
	AddChild(&m_PickOptionTxt);
	AddChild(&m_PickOptionMenuDown);
	AddChild(&m_PickOptionEditBox);
	//
	AddChild(&m_PickOptionList);	
	AddChild(&m_PickOptionScroll);
	m_PickOptionList.SetScrollbar(&m_PickOptionScroll);	
	//
	AddChild(&m_AddBtn);
	AddChild(&m_DelBtn);
	//
	AddChild(&m_PriceCellCheckBox);
	AddChild(&m_PriceCellEditBox);
	AddChild(&m_LevelCheckBox);
	AddChild(&m_LevelEditBox);
	//
	AddChild(&m_RiAmPeCheckBox);
	AddChild(&m_RiEditBox);
	AddChild(&m_AmPeEditBox);
	//
	AddChild(&m_SortBagCheckBox);
	AddChild(&m_SortBagEditBox);
	//
	AddChild(&m_NPickBackLCheckBox);
	AddChild(&m_NPickBackLBtn);
	//
	AddChild(&m_FillterItemCheckBox);//läc item
	//
	AddChild(&m_KeepPurpleCheckBox);
	AddChild(&m_KeepPurpleEditBox);
	
	
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

void KUiAutoPlayPick::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;

	sprintf(Buff, "%s\\%s", pScheme, SCHEME_PICK_INI);

	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		//
		m_AutoPickCheckBox.Init(&Ini, "AutoPickCheckBox");
		m_AutoPickEditBox.Init(&Ini, "AutoPickEditBox");
		m_PickAllCheckBox.Init(&Ini, "PickAllCheckBox");
		m_PickSpeCheckBox.Init(&Ini, "PickSpeCheckBox");
		m_NoneFightCheckBox.Init(&Ini, "NoneFightCheckBox");
		//
		m_PickSelTxt.Init(&Ini, "PickSelTxt");
		m_PickSelMenuDown.Init(&Ini, "PickSelMenuDown");
		//
		m_PickOptionTxt.Init(&Ini, "PickOptionTxt");
		m_PickOptionMenuDown.Init(&Ini, "PickOptionMenuDown");
		m_PickOptionEditBox.Init(&Ini, "PickOptionEditBox");
		//
		m_PickOptionList.Init(&Ini, "PickOptionList");			
		m_PickOptionScroll.Init(&Ini, "PickOptionScroll");
		//
		m_AddBtn.Init(&Ini, "AddBtn");			
		m_DelBtn.Init(&Ini, "DelBtn");
		//
		m_PriceCellCheckBox.Init(&Ini, "PriceCellCheckBox");
		m_PriceCellEditBox.Init(&Ini, "PriceCellEditBox");
		m_LevelCheckBox.Init(&Ini, "LevelCheckBox");
		m_LevelEditBox.Init(&Ini, "LevelEditBox");
		//
		m_RiAmPeCheckBox.Init(&Ini, "RiAmPeCheckBox");
		m_RiEditBox.Init(&Ini, "RiEditBox");
		m_AmPeEditBox.Init(&Ini, "AmPeEditBox");
		//
		m_SortBagCheckBox.Init(&Ini, "SortBagCheckBox");
		m_SortBagEditBox.Init(&Ini, "SortBagEditBox");
		//
		m_NPickBackLCheckBox.Init(&Ini, "NPickBackLCheckBox");
		m_NPickBackLBtn.Init(&Ini, "NPickBackLBtn");
		//
		m_FillterItemCheckBox.Init(&Ini, "FillterItemCheckBox");
		//
		m_KeepPurpleCheckBox.Init(&Ini, "KeepPurpleCheckBox");
		m_KeepPurpleEditBox.Init(&Ini, "KeepPurpleEditBox");
	}
	UpdateData();
}

void KUiAutoPlayPick::UpdateData()
{
	//
	m_AutoPickCheckBox.CheckButton(m_bAutoPickCheckBox);
	m_AutoPickEditBox.SetIntText(m_nAutoPickEditBox);
	m_PickAllCheckBox.CheckButton(m_bPickAllCheckBox);
	m_PickSpeCheckBox.CheckButton(m_bPickSpeCheckBox);
	m_NoneFightCheckBox.CheckButton(m_bNoneFightCheckBox);
	//
	m_PickSelTxt.SetText(g_ArraySelectPick[m_nPickSelTxtSelect]); //chän vËt phÈm theo « vÝ dô 1x2 «
	//
	m_PickOptionTxt.SetText(g_ArrayData2_Magic[m_nPickOptionTxtSelect][0]);
	m_PickOptionEditBox.SetIntText(m_nPickOptionEditBox);
	//
	m_PriceCellCheckBox.CheckButton(m_bPriceCellCheckBox);
	m_PriceCellEditBox.SetIntText(m_nPriceCellEditBox);
	//
	m_LevelCheckBox.CheckButton(m_bLevelCheckBox);
	m_LevelEditBox.SetIntText(m_nLevelEditBox);
	//
	m_RiAmPeCheckBox.CheckButton(m_bRiAmPeCheckBox);
	m_RiEditBox.SetIntText(m_nRiEditBox);
	m_AmPeEditBox.SetIntText(m_nAmPeEditBox);
	//
	m_SortBagCheckBox.CheckButton(m_bSortBagCheckBox);
	m_SortBagEditBox.SetIntText(m_nSortBagEditBox);
	//
	m_NPickBackLCheckBox.CheckButton(m_bNPickBackLCheckBox);//vËt phÈm ®en
	//
	m_FillterItemCheckBox.CheckButton(m_bFillterItemCheckBox);//nhÆt r¸c
	//
	m_KeepPurpleCheckBox.CheckButton(m_bKeepPurpleCheckBox);//®å tÝm
	m_KeepPurpleEditBox.SetIntText(m_nKeepPurpleEditBox);

	LoadConfig();
}

int KUiAutoPlayPick::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)(KWndWindow*)&m_AutoPickCheckBox)
		{
			m_bAutoPickCheckBox = !m_bAutoPickCheckBox;
		}
		else if(uParam == (unsigned int)(KWndWindow*)&m_PickAllCheckBox)
		{
			m_bPickAllCheckBox = !m_bPickAllCheckBox;
		}
		else if(uParam == (unsigned int)(KWndWindow*)&m_PickSpeCheckBox)
		{
			m_bPickSpeCheckBox = !m_bPickSpeCheckBox;
		}
		else if(uParam == (unsigned int)(KWndWindow*)&m_NoneFightCheckBox)
		{
			m_bNoneFightCheckBox = !m_bNoneFightCheckBox;
		}
		else if(uParam == (unsigned int)(KWndWindow*)&m_PickSelMenuDown)
		{
			PopupPickMenu();
		}
		else if(uParam == (unsigned int)(KWndWindow*)&m_PickOptionMenuDown)
		{
			PopupListMagicMenu();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_AddBtn)
		{
			int i = FilterSameMagic(g_ArrayData2_Magic[m_nPickOptionTxtSelect][1]);

			if (i >= 0 && i < defMAX_AUTO_FILTERL)
			{
				strcpy(m_cFilterMagic[i], g_ArrayData2_Magic[m_nPickOptionTxtSelect][1]);
				m_FilterMagicV[i] = m_PickOptionEditBox.GetIntNumber();
			}
			else
			{
				for (i = 0; i < defMAX_AUTO_FILTERL; i++)
				{
					if (!m_cFilterMagic[i][0])
					{
						strcpy(m_cFilterMagic[i], g_ArrayData2_Magic[m_nPickOptionTxtSelect][1]);
						m_FilterMagicV[i] = m_PickOptionEditBox.GetIntNumber();
						break;
					}
				}
			}
			SetFilterMagicList();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_DelBtn)
		{
			int nSel = m_PickOptionList.GetCurSel();
			if(nSel < 0) nSel = 0; //fix by phong kiÒu
			memset(m_cFilterMagic[m_FilterMagicIndex[nSel]], 0, sizeof(m_cFilterMagic[nSel]));
			m_FilterMagicV[m_FilterMagicIndex[nSel]] = 0;
			SetFilterMagicList();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_DelAllBtn)
		{
			memset(m_cFilterMagic, 0, sizeof(m_cFilterMagic));
			memset(m_FilterMagicV, 0, sizeof(m_FilterMagicV));
			SetFilterMagicList();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_PriceCellCheckBox)
		{
			m_bPriceCellCheckBox = !m_bPriceCellCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_LevelCheckBox)
		{
			m_bLevelCheckBox = !m_bLevelCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_RiAmPeCheckBox)
		{
			m_bRiAmPeCheckBox = !m_bRiAmPeCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_SortBagCheckBox)
		{
			m_bSortBagCheckBox = !m_bSortBagCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_NPickBackLCheckBox)
		{
			m_bNPickBackLCheckBox = !m_bNPickBackLCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_NPickBackLBtn)//thªm bít vËt phÈm ®en
		{
			KUiAutoPlay::ShowPageBlackItem(true);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_FillterItemCheckBox)
		{
			m_bFillterItemCheckBox = !m_bFillterItemCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_KeepPurpleCheckBox)
		{
			m_bKeepPurpleCheckBox = !m_bKeepPurpleCheckBox;
		}	
		OnActive();
		break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if (HIWORD(nParam) == SELECT_TYPE_PICK_OPTION && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessPick(BYTE(nParam));
			}
			if (HIWORD(nParam) == SELECT_TYPE_LISTMAGIC && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessListMagic(BYTE(nParam));
			}
		}
		OnActive();
		break;
	case WND_N_LIST_ITEM_SEL:
		if(uParam == (unsigned int)&m_PickOptionList)
		{
			//m_nPickOptionTxtSelect = nParam; //kh«ng sö dông n÷a
		}
		break;
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)&m_PickOptionScroll)
			m_PickOptionList.SetTopItemIndex(nParam);
		break;
	default:
		nRet = KWndPage::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

void KUiAutoPlayPick::PopupPickMenu()
{
	int nActionDataCount = sizeof(g_ArraySelectPick) / 32;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;

	for (int i = 0; i < nActionDataCount; i++)
	{
		if ((i == PickAll) || 
			(i == PickItem1) || 
			(i == PickItem2) || 
			(i == PickItem3) || 
			(i == PickItem4) ||
			(i == PickItem5) || 
			(i == PickItem6) || 
			(i == PickItem7) || 
			(i == PickItem8)
			)
		{
			strncpy(pSelUnitMenu->Items[i].szData, g_ArraySelectPick[i], 63);
			pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
			pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->nNumItem++;
		}
	}
	int x, y;
	m_PickSelMenuDown.GetAbsolutePos(&x, &y);
	pSelUnitMenu->nX = x;
	pSelUnitMenu->nY = y;
	KPopupMenu::Popup(pSelUnitMenu, this, SELECT_TYPE_PICK_OPTION);
}

void KUiAutoPlayPick::ProcessPick(BYTE btSelect)
{	
	m_nPickSelTxtSelect = btSelect;
	m_PickSelTxt.SetText(g_ArraySelectPick[btSelect]);	
}

void KUiAutoPlayPick::PopupListMagicMenu()
{
	int nActionDataCount = sizeof(g_ArrayData2_Magic) / 32;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;

	for (int i = 0; i < nActionDataCount; i++)
	{
		if (i >= 0 && i < defMAX_AUTO_FILTERL)
		{
			strncpy(pSelUnitMenu->Items[i].szData, g_ArrayData2_Magic[i][0], 63);
			pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
			pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->nNumItem++;
		}
	}
	int x, y;
	m_PickOptionMenuDown.GetAbsolutePos(&x, &y);
	pSelUnitMenu->nX = x;
	pSelUnitMenu->nY = y;
	KPopupMenu::Popup(pSelUnitMenu, this, SELECT_TYPE_LISTMAGIC);
}

void KUiAutoPlayPick::ProcessListMagic(BYTE btSelect)
{	
	m_nPickOptionTxtSelect = btSelect;
	m_PickOptionTxt.SetText(g_ArrayData2_Magic[btSelect][0]);	
}

void KUiAutoPlayPick::SetFilterMagicList()
{
	memset(m_FilterMagicIndex, 0, sizeof(m_FilterMagicIndex));
	m_PickOptionList.ResetContent();

	char Buff[64];
	int Count = 0;
	for (int i = 0; i < defMAX_AUTO_FILTERL; i++)
	{
		g_pCoreShell->FkAutoSetFillterMagic(NULL, 0, i);
		if (m_cFilterMagic[i][0])
		{
			g_pCoreShell->FkAutoSetFillterMagic((unsigned int)m_cFilterMagic[i], m_FilterMagicV[i], Count);

			sprintf(Buff, MSG_FORMAT_MAGIC, g_ArrayData2_Magic[FindSameMagic(m_cFilterMagic[i])][0], m_FilterMagicV[i]);
			m_PickOptionList.AddString(Count, Buff);
			m_FilterMagicIndex[Count] = i;
			Count++;
		}
	}
}

int KUiAutoPlayPick::FilterSameMagic(const char* szMagic)
{
	for (int i = 0; i < defMAX_AUTO_FILTERL; i ++)
	{
		if (strcmp(szMagic, m_cFilterMagic[i]) == 0)
			return i;
	}
	return -1;
}

int KUiAutoPlayPick::FindSameMagic(const char* szMagic)
{
	for (int i = 0; i < defMAX_AUTO_FILTERL; i ++)
	{
		if (strcmp(szMagic, g_ArrayData2_Magic[i][1]) == 0)
			return i;
	}
	return -1;
}

void KUiAutoPlayPick::OnActive()
{
	if(KUiAutoPlay::GetActive() == TRUE)
	{
		m_nAutoPickEditBox = m_AutoPickEditBox.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_OBJ_1, m_bAutoPickCheckBox, m_nAutoPickEditBox);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_OBJ_2, m_bPickAllCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_OBJ_3, m_bPickSpeCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_OBJ_4, m_bNoneFightCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_OBJ_5, m_nPickSelTxtSelect, 0);
		m_nPriceCellEditBox = m_PriceCellEditBox.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_OBJ_6, m_bPriceCellCheckBox, m_nPriceCellEditBox);
		m_nLevelEditBox = m_LevelEditBox.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_OBJ_7, m_bLevelCheckBox, m_nLevelEditBox);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_OBJ_8, m_bRiAmPeCheckBox, 0);//gi÷ trang søc
		m_nRiEditBox = m_RiEditBox.GetIntNumber();
		m_nAmPeEditBox = m_AmPeEditBox.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_OBJ_9, m_nRiEditBox, m_nAmPeEditBox);
		m_nSortBagEditBox = m_SortBagEditBox.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_OBJ_10, m_bSortBagCheckBox, m_nSortBagEditBox);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_OBJ_11, m_bNPickBackLCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_OBJ_12, m_bFillterItemCheckBox, 0);
		m_nKeepPurpleEditBox = m_KeepPurpleEditBox.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_OBJ_13, m_bKeepPurpleCheckBox, m_nKeepPurpleEditBox);
		m_nPickOptionEditBox = m_PickOptionEditBox.GetIntNumber();
	}
	SaveConfig();
}

void KUiAutoPlayPick::SaveConfig()
{
	KIniFile* pConfigFile = NULL;
	pConfigFile = g_UiBase.GetAutoSettingFile();
	if (pConfigFile)
	{
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_bAutoPickCheckBox", m_bAutoPickCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_nAutoPickEditBox", m_nAutoPickEditBox);
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_bPickAllCheckBox", m_bPickAllCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_bPickSpeCheckBox", m_bPickSpeCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_bNoneFightCheckBox", m_bNoneFightCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_nPickSelTxtSelect", m_nPickSelTxtSelect);
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_nPickOptionTxtSelect", m_nPickOptionTxtSelect);
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_nPickOptionEditBox", m_nPickOptionEditBox);
		char szKeyName[10];
		int Count = 0;
		for (int i = 0; i < defMAX_AUTO_FILTERL; i++)
		{
			sprintf(szKeyName, "Name%d", i);
			pConfigFile->WriteString("FilterMagic", szKeyName, m_cFilterMagic[i]);
			sprintf(szKeyName, "Value%d", i);
			pConfigFile->WriteInteger("FilterMagic", szKeyName, m_FilterMagicV[i]);
		}
		pConfigFile->WriteInteger("FilterMagic", "m_nPickSelTxtSelect", m_nPickSelTxtSelect);
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_bPriceCellCheckBox", m_bPriceCellCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_nPriceCellEditBox", m_nPriceCellEditBox);
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_bLevelCheckBox", m_bLevelCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_nLevelEditBox", m_nLevelEditBox);
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_bRiAmPeCheckBox", m_bRiAmPeCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_nRiEditBox", m_nRiEditBox);
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_nAmPeEditBox", m_nAmPeEditBox);
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_bSortBagCheckBox", m_bSortBagCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_nSortBagEditBox", m_nSortBagEditBox);
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_bNPickBackLCheckBox", m_bNPickBackLCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_bFillterItemCheckBox", m_bFillterItemCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_bKeepPurpleCheckBox", m_bKeepPurpleCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayPick", "m_nKeepPurpleEditBox", m_nKeepPurpleEditBox);
	}
	g_UiBase.CloseAutoSettingFile(true);
}

void KUiAutoPlayPick::LoadConfig()
{
	KIniFile* pConfigFile = NULL;
	pConfigFile = g_UiBase.GetAutoSettingFile();
	int nTempValue = 0;
	if (pConfigFile)
	{
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_bAutoPickCheckBox", 1, (int*)(&nTempValue)); m_bAutoPickCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_nAutoPickEditBox", 500, (int*)(&nTempValue)); m_nAutoPickEditBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_bPickAllCheckBox", 0, (int*)(&nTempValue)); m_bPickAllCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_bPickSpeCheckBox", 1, (int*)(&nTempValue)); m_bPickSpeCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_bNoneFightCheckBox", 0, (int*)(&nTempValue)); m_bNoneFightCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_nPickSelTxtSelect", 0, (int*)(&nTempValue)); m_nPickSelTxtSelect = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_nPickOptionTxtSelect", 0, (int*)(&nTempValue)); m_nPickOptionTxtSelect = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_nPickOptionEditBox", 0, (int*)(&nTempValue)); m_nPickOptionEditBox = nTempValue;
		memset(m_cFilterMagic, 0, sizeof(m_cFilterMagic));
		memset(m_FilterMagicV, 0, sizeof(m_FilterMagicV));
		if (pConfigFile->IsSectionExist("FilterMagic"))
		{
			char szKeyName[10];
			for (int i = 0; i < defMAX_AUTO_FILTERL; i++)
			{
				sprintf(szKeyName, "Name%d", i);
				pConfigFile->GetString("FilterMagic", szKeyName, "", m_cFilterMagic[i], 32 /*sizeof(m_cFilterMagic[i])*/);//fix by phong kiÒu
				sprintf(szKeyName, "Value%d", i);
				pConfigFile->GetInteger("FilterMagic", szKeyName, 0, &m_FilterMagicV[i]);
			}
		}	
		SetFilterMagicList();
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_bPriceCellCheckBox", 0, (int*)(&nTempValue)); m_bPriceCellCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_nPriceCellEditBox", 100, (int*)(&nTempValue)); m_nPriceCellEditBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_bLevelCheckBox", 0, (int*)(&nTempValue)); m_bLevelCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_nLevelEditBox", 1, (int*)(&nTempValue)); m_nLevelEditBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_bRiAmPeCheckBox", 0, (int*)(&nTempValue)); m_bRiAmPeCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_nRiEditBox", 1, (int*)(&nTempValue)); m_nRiEditBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_nAmPeEditBox", 10, (int*)(&nTempValue)); m_nAmPeEditBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_bSortBagCheckBox", 0, (int*)(&nTempValue)); m_bSortBagCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_nSortBagEditBox", 6, (int*)(&nTempValue)); m_nSortBagEditBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_bNPickBackLCheckBox", 0, (int*)(&nTempValue)); m_bNPickBackLCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_bFillterItemCheckBox", 0, (int*)(&nTempValue)); m_bFillterItemCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_bKeepPurpleCheckBox", 0, (int*)(&nTempValue)); m_bKeepPurpleCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayPick", "m_nKeepPurpleEditBox", 6, (int*)(&nTempValue)); m_nKeepPurpleEditBox = nTempValue;
		
	}
	g_UiBase.CloseAutoSettingFile(true);
}

//=KUiAutoPlayMove==========================================================================
KUiAutoPlayMove::KUiAutoPlayMove()
{
	m_bFollowCheckBox = TRUE;
	m_nFollowEditBox = 200;
	strcpy(m_nFollowNameTxt, MSG_NON_SETTINGS);
	//
	m_bLoopPosCheckBox = TRUE;//quanh ®iÓm x y
	m_nXLoopPosTxt = 0;
	m_nYLoopPosTxt = 0;
	m_nLoopPosRangEBox = 600;
	//
	m_bConstPosCheckBox = TRUE; //theo to¹ ®é
	memset(m_MoveMpsTrain, 0, sizeof(m_MoveMpsTrain));
	//
	m_bMoveMapsCheckBox = TRUE;//theo b¶n ®å
	m_nMoveMapID = 53;
	strcpy(m_charMoveMapNameTxt,"Ba L¨ng HuyÖn");
	//
	m_bIRMoveCheckBox	= TRUE;
	m_bFMORCheckBox = TRUE;
	m_bDamnMonterCheckBox = TRUE;
	m_bIRMoveCheckBox = TRUE;
	//
	m_bFlCaptainCheckBox = TRUE;
	m_bFlAnnyPTCheckBox = TRUE;
}

void KUiAutoPlayMove::Initialize()
{
	AddChild(&m_FollowCheckBox);
	//
	AddChild(&m_FollowNameTxt);
	AddChild(&m_FollowNameMenuDown);
	AddChild(&m_FollowEditBox);
	//
	AddChild(&m_LoopPosCheckBox);
	AddChild(&m_LoopPosTxt);
	AddChild(&m_LoopPosRangEBox);
	//
	AddChild(&m_ConstPosCheckBox);
	AddChild(&m_ConstPosList);
	AddChild(&m_ConstPosScroll);
	m_ConstPosList.SetScrollbar(&m_ConstPosScroll);
	AddChild(&m_AddBtn);
	AddChild(&m_DelBtn);
	AddChild(&m_DelAllBtn);
	//
	AddChild(&m_MoveMapsCheckBox);
	AddChild(&m_MoveMapNameTxt);
	//
	AddChild(&m_IRMoveCheckBox);
	AddChild(&m_FMORCheckBox);
	AddChild(&m_DamnMonterCheckBox);
	AddChild(&m_TalkAnnyCheckBox);
	//
	AddChild(&m_FlCaptainCheckBox);
	AddChild(&m_FlAnnyPTCheckBox);
	
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

void KUiAutoPlayMove::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;

	sprintf(Buff, "%s\\%s", pScheme, SCHEME_MOVE_INI);

	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");

		m_FollowCheckBox.Init(&Ini, "FollowCheckBox");
		//
		m_FollowNameTxt.Init(&Ini, "FollowNameTxt");
		m_FollowNameMenuDown.Init(&Ini, "FollowNameMenuDown");
		m_FollowEditBox.Init(&Ini, "FollowEditBox");
		//
		m_LoopPosCheckBox.Init(&Ini, "LoopPosCheckBox");
		m_LoopPosTxt.Init(&Ini, "LoopPosTxt");
		m_LoopPosRangEBox.Init(&Ini, "LoopPosRangEBox");
		//
		m_ConstPosCheckBox.Init(&Ini, "ConstPosCheckBox");
		m_ConstPosList.Init(&Ini, "ConstPosList");
		m_ConstPosScroll.Init(&Ini, "ConstPosScroll");
		m_AddBtn.Init(&Ini, "AddBtn");
		m_DelBtn.Init(&Ini, "DelBtn");
		m_DelAllBtn.Init(&Ini, "DelAllBtn");
		//
		m_MoveMapsCheckBox.Init(&Ini, "MoveMapsCheckBox");
		m_MoveMapNameTxt.Init(&Ini, "MoveMapNameTxt");
		//
		m_IRMoveCheckBox.Init(&Ini, "IRMoveCheckBox");
		m_FMORCheckBox.Init(&Ini, "FMORCheckBox");
		m_DamnMonterCheckBox.Init(&Ini, "DamnMonterCheckBox");
		m_TalkAnnyCheckBox.Init(&Ini, "TalkAnnyCheckBox");
		//
		m_FlCaptainCheckBox.Init(&Ini, "FlCaptainCheckBox");
		m_FlAnnyPTCheckBox.Init(&Ini, "FlAnnyPTCheckBox");
	}
	UpdateData();
}

void KUiAutoPlayMove::UpdateData()
{
	LoadConfig();

	m_FollowCheckBox.CheckButton(m_bFollowCheckBox);
	m_FollowNameTxt.SetText(m_nFollowNameTxt);
	m_FollowEditBox.SetIntText(m_nFollowEditBox);
	//
	m_LoopPosCheckBox.CheckButton(m_bLoopPosCheckBox);
	m_LoopPosTxt.Set2IntText(m_nXLoopPosTxt/8/32, m_nYLoopPosTxt/16/32, '/');
	m_LoopPosRangEBox.SetIntText(m_nLoopPosRangEBox);
	//
	m_ConstPosCheckBox.CheckButton(m_bConstPosCheckBox);
	//
	m_MoveMapsCheckBox.CheckButton(m_bMoveMapsCheckBox);
	m_MoveMapNameTxt.SetText(m_charMoveMapNameTxt);
	//
	m_IRMoveCheckBox.CheckButton(m_bIRMoveCheckBox);
	m_FMORCheckBox.CheckButton(m_bFMORCheckBox);
	m_DamnMonterCheckBox.CheckButton(m_bDamnMonterCheckBox);
	m_TalkAnnyCheckBox.CheckButton(m_bTalkAnnyCheckBox);
	//
	m_FlCaptainCheckBox.CheckButton(m_bFlCaptainCheckBox);
	m_FlAnnyPTCheckBox.CheckButton(m_bFlAnnyPTCheckBox);
}

int KUiAutoPlayMove::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_FollowCheckBox)
		{
			m_bFollowCheckBox = !m_bFollowCheckBox;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_FollowNameMenuDown)
		{
			PopUpFollow();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_LoopPosCheckBox)
		{
			m_bLoopPosCheckBox = !m_bLoopPosCheckBox;
			if(m_bLoopPosCheckBox)
			{
				m_nXLoopPosTxt = m_CurrentMps.nScenePos0*32;
				m_nYLoopPosTxt = m_CurrentMps.nScenePos1*64;
				m_LoopPosTxt.Set2IntText(m_nXLoopPosTxt/8/32, m_nYLoopPosTxt/16/32, '/');
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_ConstPosCheckBox)
		{
			m_bConstPosCheckBox = !m_bConstPosCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_AddBtn)
		{
			InsertMoveMpsList(m_CurrentMps.nSceneId, m_CurrentMps.nScenePos0*32, m_CurrentMps.nScenePos1*64);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_MoveMapsCheckBox)
		{
			m_bMoveMapsCheckBox = !m_bMoveMapsCheckBox;
			if(m_bMoveMapsCheckBox)
			{
				m_nMoveMapID = m_CurrentMps.nSceneId;
				strcpy(m_charMoveMapNameTxt, m_CurrentMps.szSceneName);
				m_MoveMapNameTxt.SetText(m_charMoveMapNameTxt);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_DelBtn)
		{
			int nSel = m_ConstPosList.GetCurSel();
			int nCount = 0;
			int i;
			memset(m_MoveMpsTrain[nSel], 0, sizeof(m_MoveMpsTrain[nSel]));
			for (i = 0; i < defMAX_AUTO_MOVEMPSL; i++)
			{
				if (m_MoveMpsTrain[i][0])
				{
					nCount++;
				}
			}
			for (i = nSel; i < nCount; i++)			
			{
				for (int j = 0; j < 3; j++)
				{
					m_MoveMpsTrain[i][j] = m_MoveMpsTrain[i+1][j];
					m_MoveMpsTrain[i+1][j] = 0;				
				}		
			}			
			nCount--;			
			SetMoveMpsList();			
		}		
		else if (uParam == (unsigned int)(KWndWindow*)&m_DelAllBtn)
		{
			memset(m_MoveMpsTrain, 0, sizeof(m_MoveMpsTrain));
			SetMoveMpsList();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_IRMoveCheckBox)
		{
			m_bIRMoveCheckBox = !m_bIRMoveCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_FMORCheckBox)
		{
			m_bFMORCheckBox = !m_bFMORCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_DamnMonterCheckBox)
		{
			m_bDamnMonterCheckBox = !m_bDamnMonterCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TalkAnnyCheckBox)
		{
			m_bTalkAnnyCheckBox = !m_bTalkAnnyCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_FlCaptainCheckBox)
		{
			m_bFlCaptainCheckBox = !m_bFlCaptainCheckBox;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_FlAnnyPTCheckBox)
		{
			m_bFlAnnyPTCheckBox = !m_bFlAnnyPTCheckBox;
		}
		OnActive();
		break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if (HIWORD(nParam) == 1 && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessPopUpFollow(BYTE(nParam));
			}
		}
		OnActive();
		break;
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)&m_ConstPosScroll)
			m_ConstPosList.SetTopItemIndex(nParam);
		break;
	default:
		nRet = KWndPage::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}

void KUiAutoPlayMove::OnActive()
{
	if(KUiAutoPlay::GetActive() == TRUE)
	{
		m_nFollowEditBox = m_FollowEditBox.GetIntNumber(); //ph¹m vi theo sau
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MOVE_1, m_bFollowCheckBox, m_nFollowEditBox);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MOVE_2, (unsigned int)&m_nFollowNameTxt, 0);
		//Quanh ®iÓm // Kho¶ng c¸ch quanh ®iÓm // nX // nY
		m_nLoopPosRangEBox = m_LoopPosRangEBox.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MOVE_3, m_bLoopPosCheckBox, m_nLoopPosRangEBox);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MOVE_4, m_nXLoopPosTxt, m_nYLoopPosTxt);
		//
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MOVE_5, m_bConstPosCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MOVE_6, m_bMoveMapsCheckBox, m_nMoveMapID);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MOVE_7, m_bIRMoveCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MOVE_8, m_bFMORCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MOVE_9, m_bDamnMonterCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MOVE_10, m_bTalkAnnyCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MOVE_11, m_bFlCaptainCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MOVE_12, m_bFlAnnyPTCheckBox, 0);
	}
	SaveConfig();
}

void KUiAutoPlayMove::SaveConfig()
{
	KIniFile* pConfigFile = NULL;
	pConfigFile = g_UiBase.GetAutoSettingFile();
	if (pConfigFile)
	{
		pConfigFile->WriteInteger("KUiAutoPlayMove", "m_bFollowCheckBox", m_bFollowCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayMove", "m_nFollowEditBox", m_nFollowEditBox);
		pConfigFile->WriteString("KUiAutoPlayMove", "m_nFollowNameTxt", m_nFollowNameTxt);
		//
		pConfigFile->WriteInteger("KUiAutoPlayMove", "m_bLoopPosCheckBox", m_bLoopPosCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayMove", "m_nXLoopPosTxt", m_nXLoopPosTxt);
		pConfigFile->WriteInteger("KUiAutoPlayMove", "m_nYLoopPosTxt", m_nYLoopPosTxt);
		pConfigFile->WriteInteger("KUiAutoPlayMove", "m_nLoopPosRangEBox", m_nLoopPosRangEBox);
		//
		pConfigFile->WriteInteger("KUiAutoPlayMove", "m_bConstPosCheckBox", m_bConstPosCheckBox);
		char szKeyName[10];
		for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i++)
		{
			sprintf(szKeyName, "MapID%d", i);
			pConfigFile->WriteInteger("KUiAutoPlayMove", szKeyName, m_MoveMpsTrain[i][0]);
			sprintf(szKeyName, "MapX%d", i);
			pConfigFile->WriteInteger("KUiAutoPlayMove", szKeyName, m_MoveMpsTrain[i][1]);
			sprintf(szKeyName, "MapY%d", i);
			pConfigFile->WriteInteger("KUiAutoPlayMove", szKeyName, m_MoveMpsTrain[i][2]);				
		}
		pConfigFile->WriteInteger("KUiAutoPlayMove", "m_bMoveMapsCheckBox", m_bMoveMapsCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayMove", "m_nMoveMapID", m_nMoveMapID);
		pConfigFile->WriteString("KUiAutoPlayMove", "m_charMoveMapNameTxt", m_charMoveMapNameTxt);
		pConfigFile->WriteInteger("KUiAutoPlayMove", "m_bIRMoveCheckBox", m_bIRMoveCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayMove", "m_bFMORCheckBox", m_bFMORCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayMove", "m_bDamnMonterCheckBox", m_bDamnMonterCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayMove", "m_bTalkAnnyCheckBox", m_bTalkAnnyCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayMove", "m_bFlCaptainCheckBox", m_bFlCaptainCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayMove", "m_bFlAnnyPTCheckBox", m_bFlAnnyPTCheckBox);
	}
	g_UiBase.CloseAutoSettingFile(true);
}

void KUiAutoPlayMove::LoadConfig()
{
	KIniFile* pConfigFile = NULL;
	pConfigFile = g_UiBase.GetAutoSettingFile();
	int nTempValue = 0;
	if (pConfigFile)
	{
		pConfigFile->GetInteger("KUiAutoPlayMove", "m_bFollowCheckBox", 0, (int*)(&nTempValue)); m_bFollowCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMove", "m_nFollowEditBox", 200, (int*)(&nTempValue)); m_nFollowEditBox = nTempValue;
		pConfigFile->GetString("KUiAutoPlayMove", "m_nFollowNameTxt", MSG_NON_SETTINGS, m_nFollowNameTxt, 64);
		//
		pConfigFile->GetInteger("KUiAutoPlayMove", "m_bLoopPosCheckBox", 0, (int*)(&nTempValue)); m_bLoopPosCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMove", "m_nXLoopPosTxt", 200*8*32, (int*)(&nTempValue)); m_nXLoopPosTxt = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMove", "m_nYLoopPosTxt", 200*16*32, (int*)(&nTempValue)); m_nYLoopPosTxt = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMove", "m_nLoopPosRangEBox", 600, (int*)(&nTempValue)); m_nLoopPosRangEBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMove", "m_bConstPosCheckBox", 0, (int*)(&nTempValue)); m_bConstPosCheckBox = nTempValue;
		char szKeyName[10];
		for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i++)
		{
			sprintf(szKeyName, "MapID%d", i);
			pConfigFile->GetInteger("KUiAutoPlayMove", szKeyName, 0, (int*)(&m_MoveMpsTrain[i][0]));
			sprintf(szKeyName, "MapX%d", i);
			pConfigFile->GetInteger("KUiAutoPlayMove", szKeyName, 0, (int*)(&m_MoveMpsTrain[i][1]));
			sprintf(szKeyName, "MapY%d", i);
			pConfigFile->GetInteger("KUiAutoPlayMove", szKeyName, 0, (int*)(&m_MoveMpsTrain[i][2]));			
		}
		SetMoveMpsList();
		pConfigFile->GetInteger("KUiAutoPlayMove", "m_bMoveMapsCheckBox", 0, (int*)(&nTempValue)); m_bMoveMapsCheckBox = nTempValue;
		pConfigFile->GetString("KUiAutoPlayMove", "m_charMoveMapNameTxt", "Ba L¨ng HuyÖn", m_charMoveMapNameTxt, 64);
		pConfigFile->GetInteger("KUiAutoPlayMove", "m_nMoveMapID", 0, (int*)(&nTempValue)); m_nMoveMapID = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMove", "m_bIRMoveCheckBox", 0, (int*)(&nTempValue)); m_bIRMoveCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMove", "m_bFMORCheckBox", 0, (int*)(&nTempValue)); m_bFMORCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMove", "m_bDamnMonterCheckBox", 0, (int*)(&nTempValue)); m_bDamnMonterCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMove", "m_bTalkAnnyCheckBox", 0, (int*)(&nTempValue)); m_bTalkAnnyCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMove", "m_bFlCaptainCheckBox", 0, (int*)(&nTempValue)); m_bFlCaptainCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMove", "m_bFlAnnyPTCheckBox", 0, (int*)(&nTempValue)); m_bFlAnnyPTCheckBox = nTempValue;
	}
	g_UiBase.CloseAutoSettingFile(true);
}

BOOL KUiAutoPlayMove::InsertMoveMpsList(int nSubWorldId, int nMpsX, int nMpsY)
{
	for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i ++)
	{
		if (m_MoveMpsTrain[i][0] == nSubWorldId && 
			m_MoveMpsTrain[i][1] == nMpsX && 
			m_MoveMpsTrain[i][2] == nMpsY)
		{
			i = defMAX_AUTO_MOVEMPSL;
			break;
		}
		else if (m_MoveMpsTrain[i][0] == 0 && 
				m_MoveMpsTrain[i][1] == 0 && 
				m_MoveMpsTrain[i][2] == 0)
		{
			break;
		}
	}
	if (i < defMAX_AUTO_MOVEMPSL)
	{
		m_MoveMpsTrain[i][0] = nSubWorldId;
		m_MoveMpsTrain[i][1] = nMpsX;
		m_MoveMpsTrain[i][2] = nMpsY;
		SetMoveMpsList();
	}
	return TRUE;
}

void KUiAutoPlayMove::SetMoveMpsList()
{
	char szList[32];
	char szListView[32];
	m_ConstPosList.ResetContent();
	for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i ++)
	{
		g_pCoreShell->SetMoveMap(GAUTO_AUTO_MOVEMPSID, i, m_MoveMpsTrain[i][0]); //to¹ ®é train
		g_pCoreShell->SetMoveMap(GAUTO_AUTO_MOVEMPSX, i, m_MoveMpsTrain[i][1]);
		g_pCoreShell->SetMoveMap(GAUTO_AUTO_MOVEMPSY, i, m_MoveMpsTrain[i][2]);

		if(m_MoveMpsTrain[i][0])
		{
			sprintf(szList, "%d:%d/%d", m_MoveMpsTrain[i][0], m_MoveMpsTrain[i][1], m_MoveMpsTrain[i][2]);
			sprintf(szListView, "%d:%d/%d", m_MoveMpsTrain[i][0], m_MoveMpsTrain[i][1]/8/32, m_MoveMpsTrain[i][2]/16/32);
			m_ConstPosList.AddString(i, szListView);
		}
	}
}

void KUiAutoPlayMove::PopUpFollow()
{
	if (m_fkNearbyPlayersList)
	{
		free (m_fkNearbyPlayersList);
		m_fkNearbyPlayersList = NULL;
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

		m_fkNearbyPlayersList = (KUiPlayerItem*)malloc(sizeof(KUiPlayerItem) * nActionDataCount);
		if (m_fkNearbyPlayersList)
		{
			g_pCoreShell->GetGameData(GDI_NEARBY_PLAYER_LIST, (unsigned int)m_fkNearbyPlayersList, nActionDataCount);
			for (int i = 0; i< nActionDataCount;i++)
			{
				strncpy(pSelUnitMenu->Items[i].szData, m_fkNearbyPlayersList[i].Name, 63);
				pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
				pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
				pSelUnitMenu->nNumItem++;
			}
		}
		int x, y;
		m_FollowNameMenuDown.GetAbsolutePos(&x, &y);
		pSelUnitMenu->nX = x;
		pSelUnitMenu->nY = y;
		KPopupMenu::Popup(pSelUnitMenu, this, 1);
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

void KUiAutoPlayMove::ProcessPopUpFollow(int nAction)
{
	strcpy(m_nFollowNameTxt, m_fkNearbyPlayersList[nAction].Name);
	m_FollowNameTxt.SetText(m_nFollowNameTxt);
}

//=KUiAutoPlayMap==========================================================================
char g_ArraySelectStationOption[][64] = 
{
	"Trë l¹i ®Þa ®iÓm cò",
	"N¬i ®· ®i qua 1",
	"N¬i ®· ®i qua 2",
	"N¬i ®· ®i qua 3",
	"Kh«ng ®i ®©u c¶",
};
char g_ArraySelectBuyHPOption[][64] = 
{
	"Kim S¸ng D­îc (tiÓu)",
	"Kim S¸ng D­îc (trung)",
	"Kim S¸ng D­îc (®¹i)",
	"Håi thiªn §an",
	"Cöu ChuyÓn Hoµn Hån §an",
	//"Ngò Hoa Ngäc Lé Hoµn",
};
char g_ArraySelectBuyMPOption[][64] = 
{
	"Ng­ng ThÇn §an (tiÓu)",
	"Ng­ng ThÇn §an (trung)",
	"Ng­ng ThÇn §an (®¹i)",
	"§¹i Bæ T¸n",
	"¤ §Çu Hoµn ThÇn §an",
	//"Ngò Hoa Ngäc Lé Hoµn",
};
char g_ArraySelectBuyToxicOption[][64] = 
{
	"Tam Hoa Tan",
	"Thanh Ng­u Gi¶i §éc T¸n",
	"Ho¾c H­¬ng ChÝnh KhÝ T¸n",
	"Tam ThuÇn T¸n",
	"Gi¶i ®éc t¸n",
};

KUiAutoPlayMap::KUiAutoPlayMap()
{
	m_bReturnCheckBox = FALSE;
	m_bSellCheckBox = FALSE;
	m_bRepairCheckBox = FALSE;
	m_bQuickRepairCheckBox = FALSE;
	m_bGetMoneyCheckBox = FALSE;
	m_nGetMoneyEditBox = 20;
	m_nPwdMoneyEditBox = 123456;
	//
	m_bBuyHPCheckBox = FALSE;
	m_nBuyHPEditBox = 6;
	m_BuyHPTxtSelect = 0;
	m_bBuyMPCheckBox = FALSE;
	m_nBuyMPEditBox = 6;
	m_BuyMPTxtSelect = 0;
	//
	m_bBuyToxicCheckBox = FALSE;
	m_nBuyToxicEditBox = 6;
	m_BuyToxicTxtSelect = 0;
	//
	m_bBuyTownCheckBox = FALSE;
	m_nBuyTownEditBox = 3;
	//
	m_bMapRunPosCheckBox = FALSE;
	m_nMapTxtId = 53;
	strcpy(m_cMapTxtName, "Ba L¨ng HuyÖn");
	memset(m_MoveMpsMap, 0, sizeof(m_MoveMpsMap));
	//
	m_bKeepMoneyCheckBox = FALSE;
	m_nKeepMoneyEditBox = 20;
	m_bGetFYCheckBox = FALSE;
	m_bGoFarAwayCheckBox = FALSE;
	m_GoFarAwayTxtSelect = 0;
}

void KUiAutoPlayMap::Initialize()
{
	AddChild(&m_ReturnCheckBox);
	AddChild(&m_SellCheckBox);
	AddChild(&m_RepairCheckBox);
	AddChild(&m_QuickRepairCheckBox);
	//
	AddChild(&m_GetMoneyCheckBox);
	AddChild(&m_GetMoneyEditBox);
	AddChild(&m_PwdMoneyEditBox);
	//
	AddChild(&m_BuyHPCheckBox);
	AddChild(&m_BuyHPTxt);
	AddChild(&m_BuyHPMenuDown);
	AddChild(&m_BuyHPEditBox);
	//
	AddChild(&m_BuyMPCheckBox);
	AddChild(&m_BuyMPTxt);
	AddChild(&m_BuyMPMenuDown);
	AddChild(&m_BuyMPEditBox);
	//
	AddChild(&m_BuyToxicCheckBox);
	AddChild(&m_BuyToxicTxt);
	AddChild(&m_BuyToxicMenuDown);
	AddChild(&m_BuyToxicEditBox);
	//
	AddChild(&m_BuyTownCheckBox);
	AddChild(&m_BuyTownEditBox);
	//
	AddChild(&m_MapTxt);
	AddChild(&m_MapRunPosCheckBox);
	//
	AddChild(&m_ConstPosList);
	AddChild(&m_ConstPosScroll);
	m_ConstPosList.SetScrollbar(&m_ConstPosScroll);
	//
	AddChild(&m_AddBtn);
	AddChild(&m_DelBtn);
	AddChild(&m_DelAllBtn);
	//
	AddChild(&m_KeepMoneyCheckBox);
	AddChild(&m_KeepMoneyEditBox);
	AddChild(&m_GetFYCheckBox);
	//
	AddChild(&m_GoFarAwayCheckBox);
	AddChild(&m_GoFarAwayTxt);
	AddChild(&m_GoFarAwayMenuDown);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

void KUiAutoPlayMap::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;

	sprintf(Buff, "%s\\%s", pScheme, SCHEME_MAP_INI);

	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");

		m_ReturnCheckBox.Init(&Ini, "ReturnCheckBox");
		m_SellCheckBox.Init(&Ini, "SellCheckBox");
		m_RepairCheckBox.Init(&Ini, "RepairCheckBox");
		m_QuickRepairCheckBox.Init(&Ini, "QuickRepairCheckBox");
		//
		m_GetMoneyCheckBox.Init(&Ini, "GetMoneyCheckBox");
		m_GetMoneyEditBox.Init(&Ini, "GetMoneyEditBox");
		m_PwdMoneyEditBox.Init(&Ini, "PwdMoneyEditBox");
		//
		m_BuyHPCheckBox.Init(&Ini, "BuyHPCheckBox");
		m_BuyHPTxt.Init(&Ini, "BuyHPTxt");
		m_BuyHPMenuDown.Init(&Ini, "BuyHPMenuDown");
		m_BuyHPEditBox.Init(&Ini, "BuyHPEditBox");
		//
		m_BuyMPCheckBox.Init(&Ini, "BuyMPCheckBox");
		m_BuyMPTxt.Init(&Ini, "BuyMPTxt");
		m_BuyMPMenuDown.Init(&Ini, "BuyMPMenuDown");
		m_BuyMPEditBox.Init(&Ini, "BuyMPEditBox");
		//
		m_BuyToxicCheckBox.Init(&Ini, "BuyToxicCheckBox");
		m_BuyToxicTxt.Init(&Ini, "BuyToxicTxt");
		m_BuyToxicMenuDown.Init(&Ini, "BuyToxicMenuDown");
		m_BuyToxicEditBox.Init(&Ini, "BuyToxicEditBox");
		//
		m_BuyTownCheckBox.Init(&Ini, "BuyTownCheckBox");
		m_BuyTownEditBox.Init(&Ini, "BuyTownEditBox");
		//
		m_MapTxt.Init(&Ini, "MapTxt");
		m_MapRunPosCheckBox.Init(&Ini, "MapRunPosCheckBox");
		//
		m_ConstPosList.Init(&Ini, "ConstPosList");
		m_ConstPosScroll.Init(&Ini, "ConstPosScroll");
		//
		m_AddBtn.Init(&Ini, "AddBtn");
		m_DelBtn.Init(&Ini, "DelBtn");
		m_DelAllBtn.Init(&Ini, "DelAllBtn");
		//
		m_KeepMoneyCheckBox.Init(&Ini, "KeepMoneyCheckBox");
		m_KeepMoneyEditBox.Init(&Ini, "KeepMoneyEditBox");
		m_GetFYCheckBox.Init(&Ini, "GetFYCheckBox");
		//
		m_GoFarAwayCheckBox.Init(&Ini, "GoFarAwayCheckBox");
		m_GoFarAwayTxt.Init(&Ini, "GoFarAwayTxt");
		m_GoFarAwayMenuDown.Init(&Ini, "GoFarAwayMenuDown");
	}
	UpdateData();
}

void KUiAutoPlayMap::UpdateData()
{
	LoadConfig();

	m_ReturnCheckBox.CheckButton(m_bReturnCheckBox);
	m_SellCheckBox.CheckButton(m_bSellCheckBox);
	m_RepairCheckBox.CheckButton(m_bRepairCheckBox);
	m_QuickRepairCheckBox.CheckButton(m_bQuickRepairCheckBox);
	//
	m_GetMoneyCheckBox.CheckButton(m_bGetMoneyCheckBox);
	m_GetMoneyEditBox.SetIntText(m_nGetMoneyEditBox);
	m_PwdMoneyEditBox.SetIntText(m_nPwdMoneyEditBox);
	//
	m_BuyHPCheckBox.CheckButton(m_bBuyHPCheckBox);
	m_BuyHPEditBox.SetIntText(m_nBuyHPEditBox);
	ProcessBuyHPSelect(m_BuyHPTxtSelect);
	//
	m_BuyMPCheckBox.CheckButton(m_bBuyMPCheckBox);
	m_BuyMPEditBox.SetIntText(m_nBuyMPEditBox);
	ProcessBuyMPSelect(m_BuyMPTxtSelect);
	//
	m_BuyToxicCheckBox.CheckButton(m_bBuyToxicCheckBox);
	m_BuyToxicEditBox.SetIntText(m_nBuyToxicEditBox);
	ProcessBuyToxicSelect(m_BuyToxicTxtSelect);
	//
	m_BuyTownCheckBox.CheckButton(m_bBuyTownCheckBox);
	m_BuyTownEditBox.SetIntText(m_nBuyTownEditBox);
	//
	m_MapRunPosCheckBox.CheckButton(m_bMapRunPosCheckBox);
	m_MapTxt.SetText(m_cMapTxtName);
	//
	m_KeepMoneyCheckBox.CheckButton(m_bKeepMoneyCheckBox);
	m_KeepMoneyEditBox.SetIntText(m_nKeepMoneyEditBox);
	m_GetFYCheckBox.CheckButton(m_bGetFYCheckBox);
	//
	m_GoFarAwayCheckBox.CheckButton(m_bGoFarAwayCheckBox);
	ProcessStationSelect(m_GoFarAwayTxtSelect);
}

int KUiAutoPlayMap::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_ReturnCheckBox)
		{
			m_bReturnCheckBox = !m_bReturnCheckBox;
		}
		else if(uParam == (unsigned int)&m_SellCheckBox)
		{
			m_bSellCheckBox = !m_bSellCheckBox;
		}
		else if(uParam == (unsigned int)&m_RepairCheckBox)
		{
			m_bRepairCheckBox = !m_bRepairCheckBox;
		}
		else if(uParam == (unsigned int)&m_QuickRepairCheckBox)
		{
			m_bQuickRepairCheckBox = !m_bQuickRepairCheckBox;
		}
		else if(uParam == (unsigned int)&m_GetMoneyCheckBox)
		{
			m_bGetMoneyCheckBox = !m_bGetMoneyCheckBox;
		}
		else if(uParam == (unsigned int)&m_BuyHPMenuDown)
		{
			PopUpBuyHPSelect();
		}
		else if(uParam == (unsigned int)&m_BuyHPCheckBox)
		{
			m_bBuyHPCheckBox = !m_bBuyHPCheckBox;
		}
		else if(uParam == (unsigned int)&m_BuyMPMenuDown)
		{
			PopUpBuyMPSelect();
		}
		else if(uParam == (unsigned int)&m_BuyMPCheckBox)
		{
			m_bBuyMPCheckBox = !m_bBuyMPCheckBox;
		}
		else if(uParam == (unsigned int)&m_BuyToxicMenuDown)
		{
			PopUpBuyToxicSelect();
		}
		else if(uParam == (unsigned int)&m_BuyToxicCheckBox)
		{
			m_bBuyToxicCheckBox = !m_bBuyToxicCheckBox;
		}
		else if(uParam == (unsigned int)&m_BuyTownCheckBox)
		{
			m_bBuyTownCheckBox = !m_bBuyTownCheckBox;
		}
		else if(uParam == (unsigned int)&m_MapRunPosCheckBox)
		{
			m_bMapRunPosCheckBox = !m_bMapRunPosCheckBox;
			if(m_bMapRunPosCheckBox)
			{
				m_nMapTxtId = m_CurrentMps.nSceneId;
				strcpy(m_cMapTxtName, m_CurrentMps.szSceneName);
				m_MapTxt.SetText(m_cMapTxtName);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_AddBtn)
		{
			InsertMoveMpsList(m_CurrentMps.nSceneId, m_CurrentMps.nScenePos0*32, m_CurrentMps.nScenePos1*64);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_DelBtn)
		{
			int nSel = m_ConstPosList.GetCurSel();
			int nCount = 0;
			int i;
			memset(m_MoveMpsMap[nSel], 0, sizeof(m_MoveMpsMap[nSel]));
			for (i = 0; i < defMAX_AUTO_MOVEMPSL; i++)
			{
				if (m_MoveMpsMap[i][0])
				{
					nCount++;
				}
			}
			for (i = nSel; i < nCount; i++)			
			{
				for (int j = 0; j < 3; j++)
				{
					m_MoveMpsMap[i][j] = m_MoveMpsMap[i+1][j];
					m_MoveMpsMap[i+1][j] = 0;				
				}		
			}			
			nCount--;			
			SetMoveMpsList();			
		}		
		else if (uParam == (unsigned int)(KWndWindow*)&m_DelAllBtn)
		{
			memset(m_MoveMpsMap, 0, sizeof(m_MoveMpsMap));
			SetMoveMpsList();
		}
		else if(uParam == (unsigned int)&m_KeepMoneyCheckBox)
		{
			m_bKeepMoneyCheckBox = !m_bKeepMoneyCheckBox;
		}
		else if(uParam == (unsigned int)&m_GetFYCheckBox)
		{
			m_bGetFYCheckBox = !m_bGetFYCheckBox;
		}
		else if(uParam == (unsigned int)&m_GoFarAwayCheckBox)
		{
			m_bGoFarAwayCheckBox = !m_bGoFarAwayCheckBox;
		}
		else if(uParam == (unsigned int)&m_GoFarAwayMenuDown)
		{
			PopUpStationSelect();
		}
		OnActive();
		break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if (HIWORD(nParam) == SELECT_TYPE_MAP_STATION && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessStationSelect(BYTE(nParam));
			}
			else if (HIWORD(nParam) == SELECT_TYPE_MAP_BUY_HP && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessBuyHPSelect(BYTE(nParam));
			}
			else if (HIWORD(nParam) == SELECT_TYPE_MAP_BUY_MP && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessBuyMPSelect(BYTE(nParam));
			}
			else if (HIWORD(nParam) == SELECT_TYPE_MAP_BUY_TOXIC && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessBuyToxicSelect(BYTE(nParam));
			}
			OnActive();
		}
		break;
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)&m_ConstPosScroll)
			m_ConstPosList.SetTopItemIndex(nParam);
		break;
	default:
		nRet = KWndPage::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}

void KUiAutoPlayMap::OnActive()
{
	if(KUiAutoPlay::GetActive() == TRUE)
	{
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MAP_1, m_bReturnCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MAP_2, m_bSellCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MAP_3, m_bRepairCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MAP_4, m_bQuickRepairCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MAP_5, m_bGetMoneyCheckBox, 0);
		m_nGetMoneyEditBox = m_GetMoneyEditBox.GetIntNumber();
		m_nPwdMoneyEditBox = m_PwdMoneyEditBox.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MAP_6, m_nGetMoneyEditBox, m_nPwdMoneyEditBox);
		//
		m_nBuyHPEditBox = m_BuyHPEditBox.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MAP_7, m_bBuyHPCheckBox, m_nBuyHPEditBox);
		//
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MAP_8, (unsigned int)&g_ArraySelectBuyHPOption[m_BuyHPTxtSelect], 0);
		//
		m_nBuyMPEditBox = m_BuyMPEditBox.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MAP_9, m_bBuyMPCheckBox, m_nBuyMPEditBox);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MAP_10, (unsigned int)&g_ArraySelectBuyMPOption[m_BuyMPTxtSelect], 0);
		//
		m_nBuyToxicEditBox = m_BuyToxicEditBox.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MAP_11, m_bBuyToxicCheckBox, m_nBuyToxicEditBox);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MAP_12, (unsigned int)&g_ArraySelectBuyToxicOption[m_BuyToxicTxtSelect], 0);
		//
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MAP_13, m_bBuyTownCheckBox, m_nBuyTownEditBox);
		m_nKeepMoneyEditBox = m_KeepMoneyEditBox.GetIntNumber();
		//
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MAP_14, m_bMapRunPosCheckBox, m_nMapTxtId);
		//
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MAP_15, m_bKeepMoneyCheckBox, m_nKeepMoneyEditBox);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MAP_16, m_bGetFYCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_MAP_17, m_bGoFarAwayCheckBox, m_GoFarAwayTxtSelect);
	}
	SaveConfig();
}

void KUiAutoPlayMap::LoadConfig()
{
	KIniFile* pConfigFile = NULL;
	pConfigFile = g_UiBase.GetAutoSettingFile();
	int nTempValue = 0;
	if (pConfigFile)
	{
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_bReturnCheckBox", 0, (int*)(&nTempValue)); m_bReturnCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_bSellCheckBox", 0, (int*)(&nTempValue)); m_bSellCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_bRepairCheckBox", 1, (int*)(&nTempValue)); m_bRepairCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_bQuickRepairCheckBox", 0, (int*)(&nTempValue)); m_bQuickRepairCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_bGetMoneyCheckBox", 0, (int*)(&nTempValue)); m_bGetMoneyCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_nGetMoneyEditBox", 20, (int*)(&nTempValue)); m_nGetMoneyEditBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_nPwdMoneyEditBox", 123456, (int*)(&nTempValue)); m_nPwdMoneyEditBox = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_bBuyHPCheckBox", 0, (int*)(&nTempValue)); m_bBuyHPCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_nBuyHPEditBox", 6, (int*)(&nTempValue)); m_nBuyHPEditBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_BuyHPTxtSelect", 0, (int*)(&nTempValue)); m_BuyHPTxtSelect = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_bBuyMPCheckBox", 0, (int*)(&nTempValue)); m_bBuyMPCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_nBuyMPEditBox", 6, (int*)(&nTempValue)); m_nBuyMPEditBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_BuyMPTxtSelect", 0, (int*)(&nTempValue)); m_BuyMPTxtSelect = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_bBuyToxicCheckBox", 0, (int*)(&nTempValue)); m_bBuyToxicCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_nBuyToxicEditBox", 6, (int*)(&nTempValue)); m_nBuyToxicEditBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_BuyToxicTxtSelect", 0, (int*)(&nTempValue)); m_BuyToxicTxtSelect = nTempValue;
		//
		
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_bBuyTownCheckBox", 0, (int*)(&nTempValue)); m_bBuyTownCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_nBuyTownEditBox", 3, (int*)(&nTempValue)); m_nBuyTownEditBox = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_bMapRunPosCheckBox", 0, (int*)(&nTempValue)); m_bMapRunPosCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_nMapTxtId", 53, (int*)(&nTempValue)); m_nMapTxtId = nTempValue;
		pConfigFile->GetString("KUiAutoPlayMap", "m_cMapTxtName", "Ba L¨ng HuyÖn", m_cMapTxtName, 64);
		//load to¹ ®é ra b·i
		char szKeyName[10];
		for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i++)
		{
			sprintf(szKeyName, "TrainID%d", i);
			pConfigFile->GetInteger("KUiAutoPlayMap", szKeyName, 0, (int*)(&m_MoveMpsMap[i][0]));
			sprintf(szKeyName, "TrainX%d", i);
			pConfigFile->GetInteger("KUiAutoPlayMap", szKeyName, 0, (int*)(&m_MoveMpsMap[i][1]));
			sprintf(szKeyName, "TrainY%d", i);
			pConfigFile->GetInteger("KUiAutoPlayMap", szKeyName, 0, (int*)(&m_MoveMpsMap[i][2]));			
		}
		SetMoveMpsList();
		//
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_bKeepMoneyCheckBox", 1, (int*)(&nTempValue)); m_bKeepMoneyCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_nKeepMoneyEditBox", 1, (int*)(&nTempValue)); m_nKeepMoneyEditBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_bGetFYCheckBox", 0, (int*)(&nTempValue)); m_bGetFYCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_bGoFarAwayCheckBox", 0, (int*)(&nTempValue)); m_bGoFarAwayCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayMap", "m_GoFarAwayTxtSelect", 0, (int*)(&nTempValue)); m_GoFarAwayTxtSelect = nTempValue;
	}
	g_UiBase.CloseAutoSettingFile(true);
}

void KUiAutoPlayMap::SaveConfig()
{
	KIniFile* pConfigFile = NULL;
	pConfigFile = g_UiBase.GetAutoSettingFile();
	int nTempValue = 0;
	if (pConfigFile)
	{
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_bReturnCheckBox", m_bReturnCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_bSellCheckBox", m_bSellCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_bRepairCheckBox", m_bRepairCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_bQuickRepairCheckBox", m_bQuickRepairCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_bGetMoneyCheckBox", m_bGetMoneyCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_nGetMoneyEditBox", m_nGetMoneyEditBox);
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_nPwdMoneyEditBox", m_nPwdMoneyEditBox);
		//
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_bBuyHPCheckBox", m_bBuyHPCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_nBuyHPEditBox", m_nBuyHPEditBox);
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_BuyHPTxtSelect", m_BuyHPTxtSelect);
		//
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_bBuyMPCheckBox", m_bBuyMPCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_nBuyMPEditBox", m_nBuyMPEditBox);
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_BuyMPTxtSelect", m_BuyMPTxtSelect);
		//
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_bBuyToxicCheckBox", m_bBuyToxicCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_nBuyToxicEditBox", m_nBuyToxicEditBox);
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_BuyToxicTxtSelect", m_BuyToxicTxtSelect);
		//
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_bBuyTownCheckBox", m_bBuyTownCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_nBuyTownEditBox", m_nBuyTownEditBox);
		//
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_bMapRunPosCheckBox", m_bMapRunPosCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_nMapTxtId", m_nMapTxtId);
		pConfigFile->WriteString("KUiAutoPlayMap", "m_cMapTxtName", m_cMapTxtName);
		//save to¹ ®é ra b·i
		char szKeyName[16];
		for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i++)
		{
			sprintf(szKeyName, "TrainID%d", i);
			pConfigFile->WriteInteger("KUiAutoPlayMap", szKeyName, m_MoveMpsMap[i][0]);
			sprintf(szKeyName, "TrainX%d", i);
			pConfigFile->WriteInteger("KUiAutoPlayMap", szKeyName, m_MoveMpsMap[i][1]);
			sprintf(szKeyName, "TrainY%d", i);
			pConfigFile->WriteInteger("KUiAutoPlayMap", szKeyName, m_MoveMpsMap[i][2]);
		}
		//
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_bKeepMoneyCheckBox", m_bKeepMoneyCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_nKeepMoneyEditBox", m_nKeepMoneyEditBox);
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_bGetFYCheckBox", m_bGetFYCheckBox);
		//
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_bGoFarAwayCheckBox", m_bGoFarAwayCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayMap", "m_GoFarAwayTxtSelect", m_GoFarAwayTxtSelect);
	}
	g_UiBase.CloseAutoSettingFile(true);
}

void KUiAutoPlayMap::PopUpBuyHPSelect()
{
	int nActionDataCount = sizeof(g_ArraySelectBuyHPOption) / 32;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;

	for (int i = 0; i < nActionDataCount; i++)
	{
		if ((i == 0) || (i == 1) || (i == 2) || (i == 3) || (i == 4))
		{
			strncpy(pSelUnitMenu->Items[i].szData, g_ArraySelectBuyHPOption[i], 63);
			pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
			pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->nNumItem++;
		}
	}
	int x, y;
	m_BuyHPMenuDown.GetAbsolutePos(&x, &y);
	pSelUnitMenu->nX = x;
	pSelUnitMenu->nY = y;
	KPopupMenu::Popup(pSelUnitMenu, this, SELECT_TYPE_MAP_BUY_HP);
}

void KUiAutoPlayMap::ProcessBuyHPSelect(int nAction)
{
	m_BuyHPTxtSelect = nAction;
	m_BuyHPTxt.SetText(g_ArraySelectBuyHPOption[m_BuyHPTxtSelect]);
}

void KUiAutoPlayMap::PopUpBuyMPSelect()
{
	int nActionDataCount = sizeof(g_ArraySelectBuyMPOption) / 32;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;

	for (int i = 0; i < nActionDataCount; i++)
	{
		if ((i == 0) || (i == 1) || (i == 2) || (i == 3) || (i == 4))
		{
			strncpy(pSelUnitMenu->Items[i].szData, g_ArraySelectBuyMPOption[i], 63);
			pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
			pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->nNumItem++;
		}
	}
	int x, y;
	m_BuyMPMenuDown.GetAbsolutePos(&x, &y);
	pSelUnitMenu->nX = x;
	pSelUnitMenu->nY = y;
	KPopupMenu::Popup(pSelUnitMenu, this, SELECT_TYPE_MAP_BUY_MP);
}

void KUiAutoPlayMap::ProcessBuyMPSelect(int nAction)
{
	m_BuyMPTxtSelect = nAction;
	m_BuyMPTxt.SetText(g_ArraySelectBuyMPOption[m_BuyMPTxtSelect]);
}

void KUiAutoPlayMap::PopUpBuyToxicSelect()
{
	int nActionDataCount = sizeof(g_ArraySelectBuyToxicOption) / 32;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;

	for (int i = 0; i < nActionDataCount; i++)
	{
		if ((i == 0) || (i == 1) || (i == 2) || (i == 3) || (i == 4))
		{
			strncpy(pSelUnitMenu->Items[i].szData, g_ArraySelectBuyToxicOption[i], 63);
			pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
			pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->nNumItem++;
		}
	}
	int x, y;
	m_BuyToxicMenuDown.GetAbsolutePos(&x, &y);
	pSelUnitMenu->nX = x;
	pSelUnitMenu->nY = y;
	KPopupMenu::Popup(pSelUnitMenu, this, SELECT_TYPE_MAP_BUY_TOXIC);
}

void KUiAutoPlayMap::ProcessBuyToxicSelect(int nAction)
{
	m_BuyToxicTxtSelect = nAction;
	m_BuyToxicTxt.SetText(g_ArraySelectBuyToxicOption[m_BuyToxicTxtSelect]);
}

void KUiAutoPlayMap::PopUpStationSelect()
{
	int nActionDataCount = sizeof(g_ArraySelectStationOption) / 32;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;

	for (int i = 0; i < nActionDataCount; i++)
	{
		if ((i == 0) || (i == 1) || (i == 2) || (i == 3))
		{
			strncpy(pSelUnitMenu->Items[i].szData, g_ArraySelectStationOption[i], 63);
			pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
			pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->nNumItem++;
		}
	}
	int x, y;
	m_GoFarAwayMenuDown.GetAbsolutePos(&x, &y);
	pSelUnitMenu->nX = x;
	pSelUnitMenu->nY = y;
	KPopupMenu::Popup(pSelUnitMenu, this, SELECT_TYPE_MAP_STATION);
}

void KUiAutoPlayMap::ProcessStationSelect(int nAction)
{
	m_GoFarAwayTxtSelect = nAction;
	m_GoFarAwayTxt.SetText(g_ArraySelectStationOption[m_GoFarAwayTxtSelect]);	
}

BOOL KUiAutoPlayMap::InsertMoveMpsList(int nSubWorldId, int nMpsX, int nMpsY)
{
	for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i ++)
	{
		if (m_MoveMpsMap[i][0] == nSubWorldId && 
			m_MoveMpsMap[i][1] == nMpsX && 
			m_MoveMpsMap[i][2] == nMpsY)
		{
			i = defMAX_AUTO_MOVEMPSL;
			break;
		}
		else if (m_MoveMpsMap[i][0] == 0 && 
				m_MoveMpsMap[i][1] == 0 && 
				m_MoveMpsMap[i][2] == 0)
		{
			break;
		}
	}
	if (i < defMAX_AUTO_MOVEMPSL)
	{
		m_MoveMpsMap[i][0] = nSubWorldId;
		m_MoveMpsMap[i][1] = nMpsX;
		m_MoveMpsMap[i][2] = nMpsY;
		SetMoveMpsList();
	}
	return TRUE;
}

void KUiAutoPlayMap::SetMoveMpsList()
{
	char szList[32];
	char szListView[32];
	m_ConstPosList.ResetContent();
	for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i ++)
	{
		g_pCoreShell->SetMoveMap(GAUTO_AUTO_MOVETPSID, i, m_MoveMpsMap[i][0]);//to¹ ®é ra b·i
		g_pCoreShell->SetMoveMap(GAUTO_AUTO_MOVETPSX, i, m_MoveMpsMap[i][1]);
		g_pCoreShell->SetMoveMap(GAUTO_AUTO_MOVETPSY, i, m_MoveMpsMap[i][2]);

		if(m_MoveMpsMap[i][0])
		{
			sprintf(szList, "%d:%d/%d", m_MoveMpsMap[i][0], m_MoveMpsMap[i][1], m_MoveMpsMap[i][2]);
			sprintf(szListView, "%d:%d/%d", m_MoveMpsMap[i][0], m_MoveMpsMap[i][1]/8/32, m_MoveMpsMap[i][2]/16/32);
			m_ConstPosList.AddString(i, szListView);
		}
	}
}

//=KUiAutoPlayTeam==========================================================================
KUiAutoPlayTeam::KUiAutoPlayTeam()
{
	m_bAutoCTeamCheckBox = FALSE;
	m_bAlwayLeaderCheckBox = FALSE;
	m_bPTAllCheckBox = FALSE;
	m_bDecAllInviCheckBox = FALSE;

	strcpy(m_cTPlayerNameTxt, "FongÙKiÒu");
	memset(m_TeamPlayerList, 0, sizeof(m_TeamPlayerList));

	m_bLeaveTeamCheckBox = FALSE;
	m_nLeaveTeamEditBox1 = 1;
	m_nLeaveTeamEditBox2 = 2;

	m_bKickTeamCheckBox = FALSE;
	m_nKickTeamEditBox = 1;

	m_bSCountTeamCheckBox = FALSE;
	m_bPTTongCheckBox = FALSE;
}

void KUiAutoPlayTeam::Initialize()
{
	AddChild(&m_AutoCTeamCheckBox);
	AddChild(&m_AlwayLeaderCheckBox);
	AddChild(&m_PTAllCheckBox);
	AddChild(&m_DecAllInviCheckBox);
	//
	AddChild(&m_TPlayerNameTxt);
	AddChild(&m_TPNameMenuDown);
	//
	AddChild(&m_ConstPosList);
	AddChild(&m_ConstPosScroll);
	m_ConstPosList.SetScrollbar(&m_ConstPosScroll);
	//
	AddChild(&m_AddBtn);
	AddChild(&m_DelBtn);
	AddChild(&m_DelAllBtn);
	//
	AddChild(&m_LeaveTeamCheckBox);
	AddChild(&m_LeaveTeamEditBox1);
	AddChild(&m_LeaveTeamEditBox2);
	//
	AddChild(&m_KickTeamCheckBox);
	AddChild(&m_KickTeamEditBox);
	//
	AddChild(&m_SCountTeamCheckBox);
	AddChild(&m_PTTongCheckBox);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

void KUiAutoPlayTeam::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;

	sprintf(Buff, "%s\\%s", pScheme, SCHEME_TEAM_INI);

	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		m_AutoCTeamCheckBox.Init(&Ini, "AutoCTeamCheckBox");
		m_AlwayLeaderCheckBox.Init(&Ini, "AlwayLeaderCheckBox");
		m_PTAllCheckBox.Init(&Ini, "PTAllCheckBox");
		m_DecAllInviCheckBox.Init(&Ini, "DecAllInviCheckBox");
		//
		m_TPlayerNameTxt.Init(&Ini, "TPlayerNameTxt");
		m_TPNameMenuDown.Init(&Ini, "TPNameMenuDown");
		//
		m_ConstPosList.Init(&Ini, "ConstPosList");
		m_ConstPosScroll.Init(&Ini, "ConstPosScroll");
		//
		m_AddBtn.Init(&Ini, "AddBtn");
		m_DelBtn.Init(&Ini, "DelBtn");
		m_DelAllBtn.Init(&Ini, "DelAllBtn");
		//
		m_LeaveTeamCheckBox.Init(&Ini, "LeaveTeamCheckBox");
		m_LeaveTeamEditBox1.Init(&Ini, "LeaveTeamEditBox1");
		m_LeaveTeamEditBox2.Init(&Ini, "LeaveTeamEditBox2");
		//
		m_KickTeamCheckBox.Init(&Ini, "KickTeamCheckBox");
		m_KickTeamEditBox.Init(&Ini, "KickTeamEditBox");
		//
		m_SCountTeamCheckBox.Init(&Ini, "SCountTeamCheckBox");
		m_PTTongCheckBox.Init(&Ini, "PTTongCheckBox");
	}
	UpdateData();
}

void KUiAutoPlayTeam::UpdateData()
{
	LoadConfig();

	m_AutoCTeamCheckBox.CheckButton(m_bAutoCTeamCheckBox);
	m_AlwayLeaderCheckBox.CheckButton(m_bAlwayLeaderCheckBox);
	m_PTAllCheckBox.CheckButton(m_bPTAllCheckBox);
	m_DecAllInviCheckBox.CheckButton(m_bDecAllInviCheckBox);
	//
	m_TPlayerNameTxt.SetText(m_cTPlayerNameTxt);
	//
	m_LeaveTeamCheckBox.CheckButton(m_bLeaveTeamCheckBox);
	m_LeaveTeamEditBox1.SetIntText(m_nLeaveTeamEditBox1);
	m_LeaveTeamEditBox2.SetIntText(m_nLeaveTeamEditBox2);
	//
	m_KickTeamCheckBox.CheckButton(m_bKickTeamCheckBox);
	m_KickTeamEditBox.SetIntText(m_nKickTeamEditBox);
	//
	m_SCountTeamCheckBox.CheckButton(m_bSCountTeamCheckBox);
	m_PTTongCheckBox.CheckButton(m_bPTTongCheckBox);
}

int KUiAutoPlayTeam::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_AutoCTeamCheckBox)
		{
			m_bAutoCTeamCheckBox = !m_bAutoCTeamCheckBox;
		}
		else if(uParam == (unsigned int)&m_AlwayLeaderCheckBox)
		{
			m_bAlwayLeaderCheckBox = !m_bAlwayLeaderCheckBox;
		}
		else if(uParam == (unsigned int)&m_PTAllCheckBox)
		{
			m_bPTAllCheckBox = !m_bPTAllCheckBox;
		}
		else if(uParam == (unsigned int)&m_DecAllInviCheckBox)
		{
			m_bDecAllInviCheckBox = !m_bDecAllInviCheckBox;
		}
		else if(uParam == (unsigned int)&m_TPNameMenuDown)
		{
			PopUpTeamPlayerName();
		}
		else if(uParam == (unsigned int)&m_AddBtn)
		{
			if(m_cTPlayerNameTxt[0])
				InsertPlayerListTeam(m_cTPlayerNameTxt);			
		}
		else if(uParam == (unsigned int)&m_DelBtn)
		{
			int nSel = m_ConstPosList.GetCurSel();
			int nCount = 0;
			int i;
			memset(m_TeamPlayerList[nSel], 0, sizeof(m_TeamPlayerList[nSel]));

			for (i = 0; i < defMAX_AUTO_MOVEMPSL; i++)
			{
				if (m_TeamPlayerList[i][0])
				{
					nCount++;
				}
			}
				
			for (i = nSel; i < nCount; i++)			
			{
				strcpy(m_TeamPlayerList[i], m_TeamPlayerList[i+1]);
				memset(m_TeamPlayerList[i+1], 0, sizeof(m_TeamPlayerList[i+1]));
			}			
			nCount--;			
								
			SetPlayerListTeam();
		}
		else if(uParam == (unsigned int)&m_DelAllBtn)
		{
			memset(m_TeamPlayerList, 0, sizeof(m_TeamPlayerList));
			SetPlayerListTeam();
		}
		else if(uParam == (unsigned int)&m_LeaveTeamCheckBox)
		{
			m_bLeaveTeamCheckBox = !m_bLeaveTeamCheckBox;
		}
		else if(uParam == (unsigned int)&m_KickTeamCheckBox)
		{
			m_bKickTeamCheckBox = !m_bKickTeamCheckBox;
		}
		else if(uParam == (unsigned int)&m_SCountTeamCheckBox)
		{
			m_bSCountTeamCheckBox = !m_bSCountTeamCheckBox;
		}
		else if(uParam == (unsigned int)&m_PTTongCheckBox)
		{
			m_bPTTongCheckBox = !m_bPTTongCheckBox;
		}				
		OnActive();
		break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if (HIWORD(nParam) == 1 && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessTeamPlayerName(BYTE(nParam));
			}
		}
		OnActive();
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)&m_ConstPosScroll)
			m_ConstPosList.SetTopItemIndex(nParam);
		break;
	default:
		nRet = KWndPage::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}

void KUiAutoPlayTeam::OnActive()
{
	if(KUiAutoPlay::GetActive() == TRUE)
	{
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_TEAM_1, m_bAutoCTeamCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_TEAM_2, m_bAlwayLeaderCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_TEAM_3, m_bPTAllCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_TEAM_4, m_bDecAllInviCheckBox, 0);
		//
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_TEAM_5, m_bLeaveTeamCheckBox, 0);
		m_nLeaveTeamEditBox1 = m_LeaveTeamEditBox1.GetIntNumber();
		m_nLeaveTeamEditBox2 = m_LeaveTeamEditBox2.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_TEAM_6, m_nLeaveTeamEditBox1, m_nLeaveTeamEditBox2);
		//
		m_nKickTeamEditBox = m_KickTeamEditBox.GetIntNumber();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_TEAM_7, m_bKickTeamCheckBox, m_nKickTeamEditBox);
		//
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_TEAM_8, m_bSCountTeamCheckBox, 0);
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_TEAM_9, m_bPTTongCheckBox, 0);
	}
	SaveConfig();
}

void KUiAutoPlayTeam::SaveConfig()
{
	KIniFile* pConfigFile = NULL;
	pConfigFile = g_UiBase.GetAutoSettingFile();
	int nTempValue = 0;
	if (pConfigFile)
	{
		pConfigFile->WriteInteger("KUiAutoPlayTeam", "m_bAutoCTeamCheckBox", m_bAutoCTeamCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayTeam", "m_bAlwayLeaderCheckBox", m_bAlwayLeaderCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayTeam", "m_bPTAllCheckBox", m_bPTAllCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayTeam", "m_bDecAllInviCheckBox", m_bDecAllInviCheckBox);
		//
		pConfigFile->WriteString("KUiAutoPlayTeam", "m_cTPlayerNameTxt", m_cTPlayerNameTxt);
		char szKeyName[16];
		for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i++)
		{
			sprintf(szKeyName, "TeamPlayerN%d", i);
			pConfigFile->WriteString("KUiAutoPlayTeam", szKeyName, m_TeamPlayerList[i]);	
		}
		//
		pConfigFile->WriteInteger("KUiAutoPlayTeam", "m_bLeaveTeamCheckBox", m_bLeaveTeamCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayTeam", "m_nLeaveTeamEditBox1", m_nLeaveTeamEditBox1);
		pConfigFile->WriteInteger("KUiAutoPlayTeam", "m_nLeaveTeamEditBox2", m_nLeaveTeamEditBox2);
		//
		pConfigFile->WriteInteger("KUiAutoPlayTeam", "m_bKickTeamCheckBox", m_bKickTeamCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayTeam", "m_nKickTeamEditBox", m_nKickTeamEditBox);
		//
		pConfigFile->WriteInteger("KUiAutoPlayTeam", "m_bSCountTeamCheckBox", m_bSCountTeamCheckBox);
		pConfigFile->WriteInteger("KUiAutoPlayTeam", "m_bPTTongCheckBox", m_bPTTongCheckBox);

	}
	g_UiBase.CloseAutoSettingFile(true);
}

void KUiAutoPlayTeam::LoadConfig()
{
	KIniFile* pConfigFile = NULL;
	pConfigFile = g_UiBase.GetAutoSettingFile();
	int nTempValue = 0;
	if (pConfigFile)
	{
		pConfigFile->GetInteger("KUiAutoPlayTeam", "m_bAutoCTeamCheckBox", 0, (int*)(&nTempValue)); m_bAutoCTeamCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayTeam", "m_bAlwayLeaderCheckBox", 0, (int*)(&nTempValue)); m_bAlwayLeaderCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayTeam", "m_bPTAllCheckBox", 0, (int*)(&nTempValue)); m_bPTAllCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayTeam", "m_bDecAllInviCheckBox", 0, (int*)(&nTempValue)); m_bDecAllInviCheckBox = nTempValue;
		//
		pConfigFile->GetString("KUiAutoPlayTeam", "m_cTPlayerNameTxt", "FongÙKiÒu", m_cTPlayerNameTxt, 64);
		char szKeyName[16];
		for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i++)
		{
			sprintf(szKeyName, "TeamPlayerN%d", i);
			pConfigFile->GetString("KUiAutoPlayTeam", szKeyName, "", m_TeamPlayerList[i], 32);//fix by phong kiÒu read ghi vµo m_MoveMpsList vÞ trÝ i
		}
		SetPlayerListTeam();	
		//
		pConfigFile->GetInteger("KUiAutoPlayTeam", "m_bLeaveTeamCheckBox", 0, (int*)(&nTempValue)); m_bLeaveTeamCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayTeam", "m_nLeaveTeamEditBox1", 1, (int*)(&nTempValue)); m_nLeaveTeamEditBox1 = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayTeam", "m_nLeaveTeamEditBox2", 6, (int*)(&nTempValue)); m_nLeaveTeamEditBox2 = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayTeam", "m_bKickTeamCheckBox", 0, (int*)(&nTempValue)); m_bKickTeamCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayTeam", "m_nKickTeamEditBox", 1, (int*)(&nTempValue)); m_nKickTeamEditBox = nTempValue;
		//
		pConfigFile->GetInteger("KUiAutoPlayTeam", "m_bSCountTeamCheckBox", 0, (int*)(&nTempValue)); m_bSCountTeamCheckBox = nTempValue;
		pConfigFile->GetInteger("KUiAutoPlayTeam", "m_bPTTongCheckBox", 0, (int*)(&nTempValue)); m_bPTTongCheckBox = nTempValue;
	}
	g_UiBase.CloseAutoSettingFile(true);
}

void KUiAutoPlayTeam::PopUpTeamPlayerName()
{
	if (m_fkNearbyPlayersList)
	{
		free (m_fkNearbyPlayersList);
		m_fkNearbyPlayersList = NULL;
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

		m_fkNearbyPlayersList = (KUiPlayerItem*)malloc(sizeof(KUiPlayerItem) * nActionDataCount);
		if (m_fkNearbyPlayersList)
		{
			g_pCoreShell->GetGameData(GDI_NEARBY_PLAYER_LIST, (unsigned int)m_fkNearbyPlayersList, nActionDataCount);
			for (int i = 0; i< nActionDataCount;i++)
			{
				strncpy(pSelUnitMenu->Items[i].szData, m_fkNearbyPlayersList[i].Name, 63);
				pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
				pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
				pSelUnitMenu->nNumItem++;
			}
		}
		int x, y;
		m_TPNameMenuDown.GetAbsolutePos(&x, &y);
		pSelUnitMenu->nX = x;
		pSelUnitMenu->nY = y;
		KPopupMenu::Popup(pSelUnitMenu, this, 1);
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

void KUiAutoPlayTeam::ProcessTeamPlayerName(int nAction)
{
	strcpy(m_cTPlayerNameTxt, m_fkNearbyPlayersList[nAction].Name);
	m_TPlayerNameTxt.SetText(m_cTPlayerNameTxt);
}

BOOL KUiAutoPlayTeam::InsertPlayerListTeam(char *playerName)
{
	for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i ++)
	{
		if (strcmp(m_TeamPlayerList[i], playerName) == 0)
		{
			i = defMAX_AUTO_MOVEMPSL;
			break;
		}
		else if (!m_TeamPlayerList[i][0])
		{
			break;
		}
	}
	if (i < defMAX_AUTO_MOVEMPSL)
	{
		strcpy(m_TeamPlayerList[i],playerName);
		SetPlayerListTeam();
	}
	return TRUE;
}

void KUiAutoPlayTeam::SetPlayerListTeam()
{
	char szList[32];
	char szListView[32];
	m_ConstPosList.ResetContent();
	for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i ++)
	{
		g_pCoreShell->SetMoveMap(GAUTO_AUTO_PT_PLAYERTEAM, i,(unsigned int)&m_TeamPlayerList[i]);//qu¶n lý tæ ®éi

		if(m_TeamPlayerList[i][0])
		{
			sprintf(szList, "%s", m_TeamPlayerList[i]);
			sprintf(szListView, "%s", m_TeamPlayerList[i]);
			m_ConstPosList.AddString(i, szListView);
		}
	}
}

//=KUiAutoPlayBlackItem==========================================================================
KUiAutoPlayBlackItem::KUiAutoPlayBlackItem()
{
	strcpy(m_cBlackItemNameTxt, "Khiªu chiÕn lÖnh");
	memset(m_BlackItemList, 0, sizeof(m_BlackItemList));
}

void KUiAutoPlayBlackItem::Initialize()
{
	AddChild(&m_BlackItemNameTxt);
	AddChild(&m_BlackItemMenuDown);
	//
	AddChild(&m_BlackItemConstPosList);
	AddChild(&m_BlackItemConstPosScroll);
	m_BlackItemConstPosList.SetScrollbar(&m_BlackItemConstPosScroll);
	//
	AddChild(&m_BlackItemAddBtn);
	AddChild(&m_BlackItemDelBtn);
	AddChild(&m_BlackItemCloseBtn);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

void KUiAutoPlayBlackItem::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;

	sprintf(Buff, "%s\\%s", pScheme, SCHEME_BLACKITEM_INI);

	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		//
		m_BlackItemNameTxt.Init(&Ini, "BlackItemNameTxt");
		m_BlackItemMenuDown.Init(&Ini, "BlackItemMenuDown");
		//
		m_BlackItemConstPosList.Init(&Ini, "BlackItemConstPosList");
		m_BlackItemConstPosScroll.Init(&Ini, "BlackItemConstPosScroll");
		//
		m_BlackItemAddBtn.Init(&Ini, "BlackItemAddBtn");
		m_BlackItemDelBtn.Init(&Ini, "BlackItemDelBtn");
		m_BlackItemCloseBtn.Init(&Ini, "BlackItemCloseBtn");
		//
	}
	UpdateData();
}

void KUiAutoPlayBlackItem::UpdateData()
{
	LoadConfig();
	//
	m_BlackItemNameTxt.SetText(m_cBlackItemNameTxt);
	//
}

int KUiAutoPlayBlackItem::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_BlackItemMenuDown)
		{
			PopUpBlackItemName();
		}
		else if(uParam == (unsigned int)&m_BlackItemAddBtn)
		{
			if(m_cBlackItemNameTxt[0])
				InsertItemListBlack(m_cBlackItemNameTxt);					
		}
		else if(uParam == (unsigned int)&m_BlackItemDelBtn)
		{	
			int nSel = m_BlackItemConstPosList.GetCurSel();
			int nCount = 0;
			int i;
			memset(m_BlackItemList[nSel], 0, sizeof(m_BlackItemList[nSel]));

			for (i = 0; i < defMAX_AUTO_MOVEMPSL; i++)
			{
				if (m_BlackItemList[i][0])
				{
					nCount++;
				}
			}
				
			for (i = nSel; i < nCount; i++)			
			{
				strcpy(m_BlackItemList[i], m_BlackItemList[i+1]);
				memset(m_BlackItemList[i+1], 0, sizeof(m_BlackItemList[i+1]));
			}			
			nCount--;			
								
			SetItemListBlack();	
		}
		else if(uParam == (unsigned int)&m_BlackItemCloseBtn)
		{
			KUiAutoPlay::ShowPageBlackItem(false);
		}
		OnActive();
		break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if (HIWORD(nParam) == 1 && (short)(LOWORD(nParam)) >= 0)
			{
				ProcessBlackItemName(BYTE(nParam));
			}
		}
		OnActive();
		break;
	default:
		break;
	}
	return nRet;
}

FKUiObjectName*	m_fkItemEquipRoomList;
void KUiAutoPlayBlackItem::PopUpBlackItemName()
{
	if (m_fkItemEquipRoomList)
	{
		free (m_fkItemEquipRoomList);
		m_fkItemEquipRoomList = NULL;
	}
	int nActionDataCount = g_pCoreShell->GetGameData(GDI_ITEM_EQUIP_ROOM_LIST, 0, 0);
	if (nActionDataCount > 0) 
	{
		struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
		if (pSelUnitMenu == NULL)
			return;
		KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
		pSelUnitMenu->nNumItem = 0;
		pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;

		m_fkItemEquipRoomList = (FKUiObjectName*)calloc(nActionDataCount, sizeof(FKUiObjectName));
		if (m_fkItemEquipRoomList)
		{
			g_pCoreShell->GetGameData(GDI_ITEM_EQUIP_ROOM_LIST, (unsigned int)m_fkItemEquipRoomList, nActionDataCount);
			for (int i = 0; i< nActionDataCount;i++)
			{
				if(m_fkItemEquipRoomList[i].FkName[0]) //fix by phong kiÒu lçi item name rçng
				{
					strncpy(pSelUnitMenu->Items[i].szData, m_fkItemEquipRoomList[i].FkName, 63);
					pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
					pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
					pSelUnitMenu->nNumItem++;
				}
			}
		}
		int x, y;
		m_BlackItemMenuDown.GetAbsolutePos(&x, &y);
		pSelUnitMenu->nX = x;
		pSelUnitMenu->nY = y;
		KPopupMenu::Popup(pSelUnitMenu, this, 1);
	} 
	else 
	{
		KSystemMessage	Msg;
		Msg.byConfirmType = SMCT_NONE;
		Msg.byParamSize = 0;
		Msg.byPriority = 0;
		Msg.eType = SMT_NORMAL;
		Msg.uReservedForUi = 0;
		strcpy(Msg.szMessage, "Trong hµnh trang kh«ng cã vËt phÈm.");
		KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
	}
}

void KUiAutoPlayBlackItem::ProcessBlackItemName(int nAction)
{
	strcpy(m_cBlackItemNameTxt, m_fkItemEquipRoomList[nAction].FkName);
	m_BlackItemNameTxt.SetText(m_cBlackItemNameTxt);
}

BOOL KUiAutoPlayBlackItem::InsertItemListBlack(char *ItemName)
{
	for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i ++)
	{
		if (strcmp(m_BlackItemList[i], ItemName) == 0)
		{
			i = defMAX_AUTO_MOVEMPSL;
			break;
		}
		else if (!m_BlackItemList[i][0])
		{
			break;
		}
	}
	if (i < defMAX_AUTO_MOVEMPSL)
	{
		strcpy(m_BlackItemList[i], ItemName);
		SetItemListBlack();
	}
	return TRUE;
}

void KUiAutoPlayBlackItem::SetItemListBlack()
{
	char szList[64]; //fix by Fong Kieu
	char szListView[64];
	m_BlackItemConstPosList.ResetContent();
	for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i ++)
	{
		g_pCoreShell->SetMoveMap(GAUTO_AUTO_BLACK_ITEM, i,(unsigned int)&m_BlackItemList[i]);//qu¶n lý vËt phÈm ®en

		if(m_BlackItemList[i][0])
		{
			sprintf(szList, "%s", m_BlackItemList[i]);
			sprintf(szListView, "%s", m_BlackItemList[i]);
			m_BlackItemConstPosList.AddString(i, szListView);
		}
	}
}

void KUiAutoPlayBlackItem::OnActive()
{
	if(KUiAutoPlay::GetActive() == TRUE)
	{

	}
	SaveConfig();
}

void KUiAutoPlayBlackItem::SaveConfig()
{
	KIniFile* pConfigFile = NULL;
	pConfigFile = g_UiBase.GetAutoSettingFile();
	int nTempValue = 0;
	if (pConfigFile)
	{
		//
		pConfigFile->WriteString("KUiAutoPlayBlackItem", "m_cBlackItemNameTxt", m_cBlackItemNameTxt);
		char szKeyName[16];
		for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i++)
		{
			sprintf(szKeyName, "BlackItemN%d", i);
			pConfigFile->WriteString("KUiAutoPlayBlackItem", szKeyName, m_BlackItemList[i]);	
		}
		//
	}
	g_UiBase.CloseAutoSettingFile(true);
}

void KUiAutoPlayBlackItem::LoadConfig()
{
	KIniFile* pConfigFile = NULL;
	pConfigFile = g_UiBase.GetAutoSettingFile();
	int nTempValue = 0;
	if (pConfigFile)
	{
		pConfigFile->GetString("KUiAutoPlayBlackItem", "m_cBlackItemNameTxt", "Khiªu chiÕn lÖnh", m_cBlackItemNameTxt, 64);
		char szKeyName[16];
		for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i++)
		{
			sprintf(szKeyName, "BlackItemN%d", i);
			pConfigFile->GetString("KUiAutoPlayBlackItem", szKeyName, "", m_BlackItemList[i], 64);
		}
		SetItemListBlack();	
	}
	g_UiBase.CloseAutoSettingFile(true);
}

//=KUiAutoPlay==============================================
KUiAutoPlay* KUiAutoPlay::m_pSelf = NULL;
KUiAutoPlay::KUiAutoPlay()
{
	m_bActive = FALSE;
	m_nPadActive = AP_FIGHT;
}

KUiAutoPlay* KUiAutoPlay::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

KUiAutoPlay* KUiAutoPlay::OpenWindow()
{	
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiAutoPlay;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		char Scheme[256]; //add by phong kiÒu reload ®Ó design sau nµy kh«ng dïng n÷a
		g_UiBase.GetCurSchemePath(Scheme, 256);
		LoadScheme(Scheme);
		m_pSelf->m_AutoFightPad.LoadScheme(Scheme);
		m_pSelf->m_AutoRestorePad.LoadScheme(Scheme);
		m_pSelf->m_AutoPickPad.LoadScheme(Scheme);
		m_pSelf->m_AutoMovePad.LoadScheme(Scheme);
		m_pSelf->m_AutoMapPad.LoadScheme(Scheme);
		m_pSelf->m_AutoTeamPad.LoadScheme(Scheme);
		m_pSelf->m_AutoBlackItemPad.LoadScheme(Scheme);//vËt phÈm ®en
		
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

void KUiAutoPlay::CloseWindow(bool bDestroy)
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
	}
}

void KUiAutoPlay::UpdateData()
{
	LoadConfig();
	ShowPage(m_nPadActive);
}

void KUiAutoPlay::ShowPageBlackItem(bool b)
{
	if(b)
	{
		m_pSelf->m_AutoBlackItemPad.Show();
	}
	else
	{
		m_pSelf->m_AutoBlackItemPad.Hide();
	}
}

void KUiAutoPlay::ShowPage(int nSubPage)
{	
	m_AutoFightPad.Hide(); //chiÕn ®Êu
	m_AutoFightBtn.CheckButton(false);

	m_AutoRestorePad.Hide(); //phôc håi
	m_AutoRestoreBtn.CheckButton(false);	

	m_AutoPickPad.Hide();//nhÆt ®å
	m_AutoPickBtn.CheckButton(false);

	m_AutoMovePad.Hide();//di chuyÓn
	m_AutoMoveBtn.CheckButton(false);

	m_AutoMapPad.Hide(); //b¶n ®å
	m_AutoMapBtn.CheckButton(false);

	m_AutoTeamPad.Hide(); //tæ ®éi
	m_AutoTeamBtn.CheckButton(false);

	m_AutoBlackItemPad.Hide(); //vËt phÈm ®en
	m_AutoBlackItemBtn.CheckButton(false);

	switch(nSubPage)
	{
		case AP_FIGHT:
			m_AutoFightPad.Show();
			m_AutoFightBtn.CheckButton(true);
			break;
		case AP_RESTORE:
			m_AutoRestorePad.Show();
			m_AutoRestoreBtn.CheckButton(true);
			break;
		case AP_PICK:
			m_AutoPickPad.Show();
			m_AutoPickBtn.CheckButton(true);
			break;
		case AP_MOVE:
			m_AutoMovePad.Show();
			m_AutoMoveBtn.CheckButton(true);
			break;
		case AP_MAP:
			m_AutoMapPad.Show();
			m_AutoMapBtn.CheckButton(true);
			break;
		case AP_TEAM:
			m_AutoTeamPad.Show();
			m_AutoTeamBtn.CheckButton(true);
			break;
		default:
			break;
	}

	m_nPadActive = nSubPage; //add by phong kiÒu save active page
}

void KUiAutoPlay::Initialize()
{	
	AddChild(&m_AutoFightBtn); //chiÕn ®Êu
	m_AutoFightPad.Initialize();
	AddPage(&m_AutoFightPad, &m_AutoFightBtn);
	
	AddChild(&m_AutoRestoreBtn); //phôc håi
	m_AutoRestorePad.Initialize();
	AddPage(&m_AutoRestorePad, &m_AutoRestoreBtn);

	AddChild(&m_AutoPickBtn); //nhÆt ®å
	m_AutoPickPad.Initialize();
	AddPage(&m_AutoPickPad, &m_AutoPickBtn);

	AddChild(&m_AutoMoveBtn); //di chuyÓn
	m_AutoMovePad.Initialize();
	AddPage(&m_AutoMovePad, &m_AutoMoveBtn);

	AddChild(&m_AutoMapBtn); //map
	m_AutoMapPad.Initialize();
	AddPage(&m_AutoMapPad, &m_AutoMapBtn);

	AddChild(&m_AutoTeamBtn); //tæ ®éi
	m_AutoTeamPad.Initialize();
	AddPage(&m_AutoTeamPad, &m_AutoTeamBtn);

	AddChild(&m_AutoBlackItemBtn); //vËt phÈm ®en
	m_AutoBlackItemPad.Initialize();
	AddPage(&m_AutoBlackItemPad, &m_AutoBlackItemBtn);

	AddChild(&m_ActiveBtn);
	AddChild(&m_CloseBtn);

	Wnd_AddWindow(this);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
}

void KUiAutoPlay::LoadScheme(const char* pScheme)
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

void KUiAutoPlay::LoadScheme(class KIniFile* pIni)
{	
	Init(pIni, "Main");
	m_AutoFightBtn.Init(pIni,"AutoFight"); //chiÕn ®Êu
	m_AutoRestoreBtn.Init(pIni,"AutoRestore"); //phôc håi
	m_AutoPickBtn.Init(pIni,"AutoPick");	//nhÆt ®å
	m_AutoMoveBtn.Init(pIni,"AutoMove"); //di chuyÓn
	m_AutoMapBtn.Init(pIni,"AutoMap"); //map
	m_AutoTeamBtn.Init(pIni,"AutoTeam"); //tæ ®éi
	//m_AutoBlackItemBtn.Init(pIni,"AutoTeam"); //vËt phÈm ®en kh«ng add vµo
	
	m_ActiveBtn.Init(pIni,"ActiveBtn");
	m_CloseBtn.Init(pIni,"CloseBtn");
	//
	UpdateData();
}

int KUiAutoPlay::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_ActiveBtn)
		{
			OnActive(m_bActive);
		}
		else if(uParam == (unsigned int)&m_CloseBtn)
		{
			OnSave();
		}
		else if(uParam == (unsigned int)&m_AutoFightBtn) //chiÕn ®Êu
		{
			ShowPage(AP_FIGHT);
		}
		else if(uParam == (unsigned int)&m_AutoRestoreBtn) //phôc håi
		{
			ShowPage(AP_RESTORE);
		}
		else if(uParam == (unsigned int)&m_AutoPickBtn) //nhÆt ®å
		{
			ShowPage(AP_PICK);
		}
		else if(uParam == (unsigned int)&m_AutoMoveBtn) //di chuyÓn
		{
			ShowPage(AP_MOVE);
		}
		else if(uParam == (unsigned int)&m_AutoMapBtn) //di chuyÓn
		{
			ShowPage(AP_MAP);
		}
		else if(uParam == (unsigned int)&m_AutoTeamBtn) //tæ ®éi
		{
			ShowPage(AP_TEAM);
		}
		SaveConfig();
		break;
	default:
		nRet = KWndPageSet::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

void KUiAutoPlay::OnActive(BOOL b)
{
	if(!m_pSelf)
		return;

	if(b == TRUE)
	{	
		m_pSelf->m_bActive = FALSE;
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_ACTIVE, FALSE, 0);
		m_pSelf->m_ActiveBtn.CheckButton(FALSE);
	}
	else
	{
		m_pSelf->m_bActive = TRUE;
		m_pSelf->m_ActiveBtn.CheckButton(TRUE);
		m_pSelf->m_AutoFightPad.OnActive();
		m_pSelf->m_AutoRestorePad.OnActive();
		m_pSelf->m_AutoPickPad.OnActive();
		m_pSelf->m_AutoMovePad.OnActive();
		m_pSelf->m_AutoMapPad.OnActive();
		m_pSelf->m_AutoTeamPad.OnActive();
		m_pSelf->m_AutoBlackItemPad.OnActive();
		g_pCoreShell->AutoPlayOperation(AUTOPLAY_OI_ACTIVE, TRUE, 0);
	}	
}

void KUiAutoPlay::OnSave()
{
	CloseWindow();
}

void KUiAutoPlay::UpdateSceneTimeInfo(KUiSceneTimeInfo* pInfo)
{
	if (m_pSelf && pInfo)
	{
		m_pSelf->m_AutoMovePad.m_CurrentMps = *pInfo;
		m_pSelf->m_AutoMapPad.m_CurrentMps = *pInfo;
	}
}

void KUiAutoPlay::LoadConfig()
{
	KIniFile* pConfigFile = NULL;
	pConfigFile = g_UiBase.GetAutoSettingFile();
	int nTempValue = 0;
	if (pConfigFile)
	{
		pConfigFile->GetInteger("KUiAutoPlay", "m_nPadActive", AP_FIGHT, (int*)(&nTempValue)); m_nPadActive = nTempValue;
	}
	g_UiBase.CloseAutoSettingFile(true);
}

void KUiAutoPlay::SaveConfig()
{
	KIniFile* pConfigFile = NULL;
	pConfigFile = g_UiBase.GetAutoSettingFile();
	if (pConfigFile)
	{
		pConfigFile->WriteInteger("KUiAutoPlay", "m_nPadActive", m_nPadActive);
	}
	g_UiBase.CloseAutoSettingFile(true);
}