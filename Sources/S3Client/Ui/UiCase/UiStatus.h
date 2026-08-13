/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong Ki襲
//	CreateTime:	2020-9-2
*****************************************************************************************/
#pragma once

#include "../Elem/WndButton.h"
#include "../Elem/WndText.h"
#include "../Elem/WndShowAnimate.h"
#include "../elem/WndObjContainer.h"

enum UI_PLAYER_ATTRIBUTE;
struct KUiPlayerRuntimeInfo;
struct KUiObjAtRegion;
struct KUiPlayerAttribute;

#define _ITEM_COUNT 17

class KUiStatus : public KWndShowAnimate
{
public:
	//--------
	static KUiStatus*	OpenWindow();					//
	static KUiStatus*	GetIfVisible();					//
	static void			CloseWindow(bool bDestroy);		//
//	static void			LoadConfig(KIniFile* pIni);		//
	static void			LoadScheme(const char* pScheme);//

	void	UpdateBaseData();							//
	void	UpdateData();
	void	UpdateRuntimeInfo(KUiPlayerRuntimeInfo* pInfo);
	void	UpdateAllEquips();
	void	UpdateRuntimeAttribute(KUiPlayerAttribute* pInfo);
	void	Breathe();
	void	UpdateEquip(KUiObjAtRegion* pEquip, int bAdd);	//

private:
	KUiStatus() {}
	~KUiStatus() {}
	void	Initialize();								//	
	void	SwitchExpand(BOOL bShow);
	void	UseRemainPoint(UI_PLAYER_ATTRIBUTE type, int numpoint);	//升级某项属性 //void	UseRemainPoint(UI_PLAYER_ATTRIBUTE type);	//升级某项属性
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);	//窗口函数
	void	LoadScheme(class KIniFile* pIni);			//载入界面方案
	void	OnEquiptChanged(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);//响应界面操作引起装备的改变
	void	OnLockItem(ITEM_PICKDROP_PLACE* pItem, int lock);
private:
	static KUiStatus*	m_pSelf;

private:
	KWndButton  m_Avatar;
	KWndText80	m_ChooseAvatar;
	KWndText32	m_nPk;
	KWndText32	m_nRepute;
	KWndText32	m_nFuYuan;
	KWndText32  m_ReBorn;
	KWndText32	m_Agname;
	KWndText32	m_Name;
	KWndText32	m_Title;
	KWndText32	m_Money;
	KWndText32	m_Life;
	KWndText32	m_Mana;
	KWndText32	m_Experience;
	KWndText32	m_Angry;
	KWndText32	m_Stamina;
	int			m_nRemainPoint;
	KWndText32	m_RemainPoint;
	KWndText32	m_fkMateName;	//phu nh﹏ phu qu﹏

	KWndText32	m_Strength, m_Dexterity, m_Vitality, m_Energy;
	KWndButton	m_AddStrength, m_AddDexterity, m_AddVitality, m_AddEnergy;

	KWndText32	m_LeftDamage, m_RightDamage, m_Attack, m_Defence, m_MoveSpeed, m_AttackSpeed;
	KWndText32	m_PhyDef, m_CoolDef, m_LightDef, m_FireDef, m_PoisonDef;
	KWndText32	m_PhyDefPlus, m_CoolDefPlus, m_LightDefPlus, m_FireDefPlus, m_PoisonDefPlus;
	KWndText32	m_Level, m_StatusDesc;
	KWndText32	m_WorldRank;			//edit by phong kieu F3 World rank

	KWndButton	m_OpenItemPad;
	KWndButton	m_Close;
	KWndButton	m_Bind;
	KWndButton	m_UnBind;
	KWndButton	m_BtnLock;
	KWndButton	m_BtnSet1;
	KWndButton	m_BtnSet2;

	KWndButton	m_EquipExpandBtn;
	KWndImage	m_EquipExpandImg;
	KWndButton	m_MaskFeature;

	//
	KWndObjectBox	m_EquipBox[_ITEM_COUNT];

};
