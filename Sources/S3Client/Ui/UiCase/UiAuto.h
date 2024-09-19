#pragma once

#include "../elem/wndpage.h"
#include "../elem/wndbutton.h"
#include "../elem/wndtext.h"
#include "../../../core/src/gamedatadef.h"
#include "../Elem/WndMessageListBox.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndEdit.h"
#include "../Elem/WndPureTextBtn.h"
#include "../Elem/WndLabeledButton.h"
#include "../Elem/WndShadow.h"
#include "../Elem/WndList2.h"

class KUiOther : public KWndPage
{
public:
	void					Initialize();
	void					LoadScheme();
	void					SaveProSetting();
	void					LoadProSettting();

private:
	int						WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
public:

private:
	KWndButton				m_SortITemBtn;
	KWndButton				m_OpenTuiDPBtn;
	KWndButton				m_BanItemBtn;	

	BOOL					IsSortItem;
	BOOL					IsOpenTuiDP;
	BOOL					IsBanItem;
};

class KUiPick : public KWndPage
{
public:
	void					Initialize();
	void					LoadScheme();
	void					LoadPickSetting();
	void					SavePickSetting();

private:
	int						WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void					OnPick();
	void					OnGiveItem();//gi÷ trang søc
	//void					OnPickInFightState();//nhÆt trong thµnh
	void					OnFillterItem();
	void					OnSelectPickType(int nIndex);
	void 					SetPick(int nIndex, int nSel);
	void					SetFilterMagicList();
	int 					FilterSameMagic(const char* szMagic);
	int 					FindSameMagic(const char* szMagic);
public:
	BOOL					IsPickItem;	
private:	
	KWndLabeledButton		m_FilterDelBtn;
	KWndLabeledButton		m_FilterDelAll;
	KWndLabeledButton		m_FilterAddBtn;
	
	KWndList2				m_FilterL;
	KWndScrollBar			m_FilterL_Scroll;
	KWndButton				m_PickBtn;		
	KWndButton				m_PickListBtn;

	KWndButton				m_PutBtn;		
	KWndButton				m_PutListBtn;

	KWndButton				m_ReturnBtn;//gi÷ trang søc
	//KWndButton				m_PickInFightState;//nhÆt trong thµnh

	KWndEdit32				m_FillterEdit;
	KWndText80				m_FilterEdit;
	int						m_CurrentPickKind;
	KWndText80				m_PickTypeTxt;
	int						m_CurrentSelMagic;
	int						m_FilterMagicIndex[defMAX_AUTO_FILTERL];
	char					m_cFilterMagic[defMAX_AUTO_FILTERL][32];
	int						m_FilterMagicV[defMAX_AUTO_FILTERL];
	int						m_nSelectIndex;
};

enum STT_Magic
{
	enumMagic_ID1,
	enumMagic_ID2,
	enumMagic_ID3,
};

enum Sel_Magic
{
	enumSel_Magic_Aura1,
	enumSel_Magic_Aura2,
	enumSel_Magic_Num,
};

enum Sel_Support_Magic
{
	enumSel_Support_Magic1 = 0,
	enumSel_Support_Magic2,
	enumSel_Support_Magic3,	
	enumSel_Support_Magic_Num,
};

class KUiFight : public KWndPage
{
public:
	void				Initialize();
	void				LoadScheme();
	void				LoadFightSetting();
	void				SaveFightSetting();

private:
	int					WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	BOOL				FilterSameMps(int nSubW, int nMpsX, int nMpsY);
	BOOL				InsertMoveMpsList(int nSubWorldId, int nMpsX, int nMpsY);
	void				SetMoveMpsList();
public:
	int					nRangeNum;
	int					nFightNum;
	void				OnAutoAttack();
	BOOL 				IsCheck;
private:	
	void				OnFollowPeople();
	void				OnBuffSkill1();
	void				OnBuffSkill2();
	void				OnBuffSkill3();	
	void				OnAutoRightSkill();//®¸nh chiªu bªn ph¶i
	void				PopUpFollow();
	void				PopUpAutoAttack();	
	void				OnActiveMove();
	void				OnFightRadius();
	void				OnArrayRadius();		
	void				ProcessPopUpFollow(int nAction);
	void				SupportSkillPopup(int nIndex);
	void				SetSupportSkill(int nIndex, int nID);
	void				ProcessPopUpAttack(int nAction);	

	KWndEdit32			m_Fight;
	KWndEdit32			m_Range;
	KWndButton			m_FightCBtn;
	KWndButton			m_AutoAttackBtn;
	KWndButton			m_AutoAttackList;
	KWndText80			m_AutoAttackEdit;	
	KWndButton			m_QDiemBtn;
	KWndText80			m_QDiemXY;
	KWndButton			m_FollowBtn;
	KWndButton			m_FollowListBtn;
	char				m_cFollowName[32];	
	KWndText80			m_NameFollowPeople;
	KWndButton			m_DiChuyenToaDoBtn;	
	
	KWndLabeledButton	m_CoordGetBtn;
	KWndLabeledButton	m_CoordDelBtn;
	KWndLabeledButton	m_CoordDelAllBtn;
	KWndList2			m_CoordL;
	KWndScrollBar		m_CoordL_Scroll;	
	
	KWndButton			m_Support1Btn;	
	KWndButton			m_Support2Btn;	
	KWndButton			m_Support3Btn;	
	KWndButton			m_AutoRightSkill;//§¸nh chiªu bªn ph¶i	
	KWndText80			m_Support1Edit;	
	KWndText80			m_Support2Edit;	
	KWndText80			m_Support3Edit;		
	KWndButton			m_Support1List;	
	KWndButton			m_Support2List;	
	KWndButton			m_Support3List;		
	int 				m_nAction;
	int					m_CurrentSelMagic[defMAX_ARRAY_STATESKILL];
	int					m_MoveMpsList[defMAX_AUTO_MOVEMPSL][3];
public:
	KUiSceneTimeInfo	m_CurrentMps;
};

class KUiRecover : public KWndPage
{
public:
	void				Initialize();
	void				LoadScheme();
	void				LoadRecoverSetting();
	void				SaveRecoverSetting();
private:
	int					WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void				OnBuff();
	void 				OnDurability();
	void				OnCheckInput();
	void				OnCheckLife();
	void				OnCheckMana();
	void				OnCheckLife2();
	void				OnCheckMana2();
	void				OnUseTTL();
	void				OnUseGD();
	void				OnMoreMoney();
	void				OnNoLife();
	void				OnNoMana();
	void				PopUpChooseTeam();
	//void				ProcessPopUpChooseTeam(int nAction);
	void				OnChooseTeam();
	void				OnActiveChangeAura();	
	void				AuraSkillPopup(int nIndex);
	void				SetAuraSkill(int nIndex, int nID);	
public:
	int					nLifeNum;
	int					nManaNum;
	int					nTownNum;
	int					nTown1Num;
	int					nMoneyNum;
	int					nDurabilityNum;
	int					nBuffTxtNum;
	int					nLifeTxtNum;
	int					nManaTxtNum;
	BOOL				IsCheck;
	BOOL					IsReturnPortal;
	BOOL					IsBuyItem;
	BOOL					IsInventoryMI;
	BOOL					IsRepairItem;
private:
	KWndButton				m_InventoryMIBtn;
	KWndButton				m_RepairBtn;	
	KWndEdit32			m_Life;
	KWndEdit32			m_Mana;	
	KWndEdit32			m_MoreMoney;
	KWndEdit32			m_DurabilityTxt;
	KWndEdit32			m_BuffNMTxt;
	KWndEdit32			m_LifeTxt;
	KWndEdit32			m_ManaTxt;
	
	KWndButton				m_BuyItemBtn;
	KWndButton				m_ReturnPortalBtn;
	KWndButton			m_LifeBtn2;	
	KWndButton			m_ManaBtn2;
	KWndEdit32			m_LifeTxt2;
	KWndEdit32			m_ManaTxt2;
	
	//KWndText80			m_NameChooseTeam;
	
	KWndButton			m_LifeBtn;	
	KWndButton			m_ManaBtn;
	KWndButton			m_NoLifeBtn;	
	KWndButton			m_NoManaBtn;
	KWndButton			m_DurabilityBtn;
	KWndButton			m_MoreMoneyBtn;	
	KWndButton			m_TeamBtn;
	KWndButton			m_TeamBtnMore;//qu¶n lý tæ ®éi
	//KWndButton			m_TeamListBtn;
	KWndButton			m_GDBtn;
	KWndButton			m_TTLBtn;	
	KWndButton			m_BuffBtn;	
	
	KWndButton			m_ChuyenVongSangBtn;		
	KWndText80			m_Aura1Edit;	
	KWndText80			m_Aura2Edit;	
	KWndButton			m_Aura1Btn;	
	KWndButton			m_Aura2Btn;	
	
	int 				m_nAction;
	int					m_CurrentSelMagic[enumSel_Support_Magic_Num];	
};

class KUiAuto : public KWndPageSet
{
public:
	static KUiAuto* OpenWindow();		
	static KUiAuto* GetUiAuto();
	static KUiAuto* GetIfVisible();
	static void		CloseWindow();
	static void		LoadScheme();
	static void		UpdateSceneTimeInfo(KUiSceneTimeInfo* pInfo);
	
	BOOL	IsAuto;	
	void	OnSave();	
	void	OnStart();
	void	OnStop();
	KUiAuto() {}
	void	UpdateButton(BOOL IsAuto);	
private:
	~KUiAuto() {}
	void	Initialize();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
private:
	static KUiAuto* m_pSelf;
private:
	KUiOther			m_OtherPad;
	KUiPick				m_PickPad;
	KUiFight			m_FightPad;
	KUiRecover			m_RecoverPad;

	KWndLabeledButton	m_OtherBtn;	
	KWndLabeledButton	m_PickBtn;	
	KWndLabeledButton	m_AutoProBtn;	
	KWndLabeledButton	m_FightBtn;	
	KWndLabeledButton	m_RecoverBtn;	

	KWndButton			m_CloseBtn;	
	KWndButton			m_StartBtn;	
	BOOL				m_bIsCheck;
};
