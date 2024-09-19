//---------------------------------------------------------------------------
// Sword3 Engine (c) 2020 KingSoft
// File:	UiAutoPlay.h
// Date:	06/06/2020
// Code:	Fong KiÒu
// Desc:	Header File
//---------------------------------------------------------------------------
#pragma once

#include "../Elem/Wndpage.h"
#include "../Elem/WndLabeledButton.h"
#include "../Elem/WndPureTextBtn.h"
#include "../Elem/WndEdit.h"
#include "../Elem/WndList.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndList2.h"

//=KUiAutoPlayFight==========================================================================
class KUiAutoPlayFight : public KWndPage
{
public:
	KUiAutoPlayFight();
	void	Initialize();
	void	OnActive();
	void	LoadScheme(const char* pScheme);
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	UpdateData();
	void	ProcessFightOption(BYTE btSelect);
	void	PopupFightOptionMenu();
	void	ProcessFightDistance(BYTE btSelect);
	void	PopupFightDistanceMenu();
	void	ProcessFightSelfDef(BYTE btSelect);
	void	PopupFightSelfDefMenu();
	void	PopupSupportSkill(int nIndex);
	void	ProcessSupportSkill(int nIndex, int nID);
	void	PopupLeftSkill();
	void	ProcessLeftSkill(int nID);
	void	PopupRightSkill();
	void	ProcessRightSkill(int nID);
	void	PopupAuraSkill1();
	void	ProcessAuraSkill1(int nID);
	void	PopupAuraSkill2();
	void	ProcessAuraSkill2(int nID);
	void	PopupShortKey1Menu();
	void	ProcessShortKey1(BYTE btSelect);
	void	PopupShortKey2Menu();
	void	ProcessShortKey2(BYTE btSelect);
	void	PopupShortCut1Menu();
	void	ProcessShortCut1(BYTE btSelect);
	void	PopupShortCut2Menu();
	void	ProcessShortCut2(BYTE btSelect);
	void	SaveConfig();
	void	LoadConfig();
private:
	KWndButton	m_FightCheckBox; //checkbox tù ®¸nh
	KWndEdit32	m_FightRangeEditBox;	//ph¹m vi tù ®¸nh
	KWndText32	m_FightOptionTxt;	//text lùa chän tù ®¸nh
	KWndButton	m_FightMenuDownBtn; //button lùa chän dropdown tù ®¸nh
	BOOL	m_bFightCheckBox;			//check tù ®¸nh
	short	m_nFightRange;		//ph¹m vi tù ®¸nh value
	BYTE	m_bFightSelect;		//lùa chän phÇn tù ®¸nh
	//
	KWndButton	m_FightDistanceCheckBox;	//tiÕp cËp
	KWndEdit32	m_FightDistanceEditBox;//ph¹m vi tiÕp cËn
	KWndText32	m_FightDistanceOptionTxt;	//text lùa chän tiÕp cËn
	KWndButton	m_FightMenuDistanceDownBtn; //button lùa chän dropdown tiÕp cËn
	BOOL	m_bFightDistance;	//check tiÕp cËn
	short	m_nFightDistance;	//ph¹m vi tiÕp cËn value
	BYTE	m_bFightDistanceSelect;//lùa chän khi tiÕp cËn
	//
	KWndButton	m_FightSelfDefCheckBox;//tù vÖ
	KWndEdit32	m_FightSelfDefEditBox;	//kho¶ng c¸ch tù vÖ
	KWndText32	m_FightSelfDefOptionTxt;	//text lùa chän tù vÖ
	KWndButton	m_FightMenuSelfDefDownBtn; //button lùa chän dropdown tù vÖ
	BOOL	m_bFightSelfDef; //tù vÖ
	short	m_nFightSelfDefValue;//gi¸ trÞ kho¶ng c¸ch tù vÖ
	BYTE	m_bFightSelfDefSelect;//lùa chän khi tù vÖ
	//
	KWndText32	m_BuffSkillTxt1;
	KWndButton	m_BuffSkillMenuDown1Btn; //support1
	//
	KWndText32	m_BuffSkillTxt2;
	KWndButton	m_BuffSkillMenuDown2Btn; //support2
	//
	KWndText32	m_BuffSkillTxt3;
	KWndButton	m_BuffSkillMenuDown3Btn; //support3
	int					m_CurrentSelMagic[defMAX_ARRAY_STATESKILL];
	//
	KWndText32	m_LeftSkillTxt;
	KWndButton	m_LeftSkillMenuDown; //skillleft
	int		m_nLeftSkillID;
	//
	KWndText32	m_RightSkillTxt;
	KWndButton	m_RightSkillMenuDown; //skillright
	int		m_nRightSkillID;
	//
	KWndText32	m_AuraSkillTxt1;
	KWndButton	m_AuraSkillMenuDown1; //skillaura1
	int		m_nSkillSelfDefID;
	//
	KWndText32	m_AuraSkillTxt2;
	KWndButton	m_AuraSkillMenuDown2; //skillaura2
	int		m_SkillBossID;
	//
	KWndText32	m_ShortcutKeyTxt1;
	KWndButton	m_ShortcutKeyMenuDown1; //shortkey1
	BYTE	m_ShortcutKeySelect1;
	//
	KWndText32	m_ShortcutKeyTxt2;
	KWndButton	m_ShortcutKeyMenuDown2; //shortkey2
	BYTE	m_ShortcutKeySelect2;
	//
	KWndEdit32	m_ShortcutEditBox1;
	int			m_nShortcutEditBox1;
	KWndEdit32	m_ShortcutEditBox2;
	int			m_nShortcutEditBox2;
	//
	KWndText32	m_ShortcutOptTxt1;
	KWndButton	m_ShortcutOptMenuDown1; //shortcut1
	BYTE	m_ShortcutOptSelect1;
	//
	KWndText32	m_ShortcutOptTxt2;
	KWndButton	m_ShortcutOptMenuDown2; //shortcut2
	BYTE	m_ShortcutOptSelect2;
	//
	KWndButton	m_SkillRightCheckBox;
	BOOL	m_bSkillRightCheckBox;
	//
	KWndButton	m_NRDoSkillCheckBox;
	BOOL	m_bNRDoSkillCheckBox;

private:
	enum SELECT_FIGHT_DISTANCE
	{
		D_BoQua = 0,
		D_TranhBoss,
		D_UuTienBoss,
		D_ChiDanhBoss,
	};
	enum SELECT_FIGHT_SELFDEF
	{
		D_TranhXa = 0,
		D_ThoDiaPhu,
		D_ThoatGame,
		D_DanhTra,
	};
};

//=KUiAutoPlayRestore==========================================================================
class KUiAutoPlayRestore: public KWndPage
{
public:
	KUiAutoPlayRestore();
	void	Initialize();
	void	OnActive();
	void	LoadScheme(const char* pScheme);
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	UpdateData();
	void	PopupFCellMenu();
	void	ProcessFCell(BYTE btSelect);
	void	SaveConfig();
	void	LoadConfig();
private:
	KWndButton	m_ReHPCheckBox;	//b¬m SL
	BOOL			m_bReHPCheckBox;
	KWndEdit32	m_ReHPEditBox1, m_ReHPEditBox2, m_ReHPEditBox3;
	short	m_nReHPEditBox1;
	short	m_nReHPEditBox2;
	short	m_nReHPEditBox3;
	//
	KWndButton	m_ReMPCheckBox;	//b¬m NL
	BOOL			m_bReMPCheckBox;
	KWndEdit32	m_ReMPEditBox1, m_ReMPEditBox2, m_ReMPEditBox3;
	short	m_nReMPEditBox1;
	short	m_nReMPEditBox2;
	short	m_nReMPEditBox3;

	KWndButton	m_TownHPVCheckBox;	//thæ ®Þa phï SL <
	KWndEdit32	m_TownHPVEditBox;
	BOOL	m_bTownHPVCheckBox;
	short		m_nTownHPVEditBox;

	KWndButton	m_TownMPVCheckBox;	//thæ ®Þa phï NL <
	KWndEdit32	m_TownMPVEditBox;
	BOOL	m_bTownMPVCheckBox;
	short		m_nTownMPVEditBox;
	
	KWndButton	m_TownHPNCheckBox, m_TownMPNCheckBox;
	BOOL	m_bTownHPNCheckBox, m_bTownMPNCheckBox;

	KWndButton m_TownFCellCheckBox;
	KWndText32	m_TownFCellTxt;//hÕt chç 1 «
	KWndButton	m_TownFCellMenuDown;
	BOOL	m_bTownFCellCheckBox;
	BYTE	m_nTownFCellTxtSelect;

	KWndButton	m_TowMoneyCheckBox;//sè tiÒn
	KWndEdit32	m_TownMoneyEditBox;
	BOOL	m_bTowMoneyCheckBox;
	short		m_nTownMoneyEditBox;

	KWndButton	m_TownAbradeCheckBox;//®å h­
	KWndEdit32 m_TownAbradeEditBox;
	BOOL		m_bTownAbradeCheckBox;
	short			m_nTownAbradeEditBox;

	KWndButton	m_UseHPCheckBox;	//c¾n m¸u ®Çy r­¬ng
	BOOL			m_bUseHPCheckBox;
	KWndButton	m_UseAntiToxicCheckBox;//gi¶i ®éc
	BOOL			m_bUseAntiToxicCheckBox;
	KWndButton	m_UseExpCheckBox;	//sö dông exp x2
	BOOL			m_bUseExpCheckBox;
	KWndButton	m_UseSkillCheckBox;//sö dông skill exp x2
	BOOL			m_bUseSkillCheckBox;
	//
	KWndButton	m_BuffCheckBox;//nga my buff
	BOOL			m_bBuffCheckBox;
	KWndEdit32	m_BuffEditBox;
	short				m_nBuffEditBox;
	KWndButton	m_BuffTeamCheckBox;
	BOOL			m_bBuffTeamCheckBox;
	//
	KWndButton	m_OpenBagHPCheckBox;
	BOOL	m_bOpenBagHPCheckBox;
	KWndEdit32	m_OpenBagHPEditBox;
	short		m_nOpenBagHPEditBox;
	//
	KWndButton	m_RingTDPCheckBox;
	BOOL	m_bRingTDPCheckBox;

private:

};

//=KUiAutoPlayPick==========================================================================
class KUiAutoPlayPick : public KWndPage
{
public:
	KUiAutoPlayPick();
	void	Initialize();
	void	OnActive();
	void	LoadScheme(const char* pScheme);	
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	UpdateData();
	void	PopupPickMenu();
	void	ProcessPick(BYTE btSelect);	
	void	PopupListMagicMenu();
	void	ProcessListMagic(BYTE btSelect);
	void	SetFilterMagicList();
	int 	FilterSameMagic(const char* szMagic);
	int 	FindSameMagic(const char* szMagic);
	void	SaveConfig();
	void	LoadConfig();
private:
	//
	KWndButton	m_AutoPickCheckBox;
	KWndEdit32	m_AutoPickEditBox;
	KWndButton	m_PickAllCheckBox;
	KWndButton	m_PickSpeCheckBox;
	KWndButton	m_NoneFightCheckBox;
	BOOL	m_bAutoPickCheckBox; //nhÆt ®å active
	int		m_nAutoPickEditBox;
	BOOL	m_bPickAllCheckBox;//kh«ng bá sãt
	BOOL	m_bPickSpeCheckBox; //tiÒn vµ ®Æc phÈm
	BOOL	m_bNoneFightCheckBox; //trong thµnh
	//
	KWndText32	m_PickSelTxt;
	KWndButton	m_PickSelMenuDown;
	int		m_nPickSelTxtSelect;
	//
	KWndText32	m_PickOptionTxt;	//chän magic item
	KWndButton	m_PickOptionMenuDown;
	KWndEdit32	m_PickOptionEditBox;
	BYTE	m_nPickOptionTxtSelect;
	short		m_nPickOptionEditBox;
	int						m_FilterMagicIndex[defMAX_AUTO_FILTERL];
	char					m_cFilterMagic[defMAX_AUTO_FILTERL][32];
	int						m_FilterMagicV[defMAX_AUTO_FILTERL];
	//
	KWndList2			m_PickOptionList;
	KWndScrollBar		m_PickOptionScroll;
	//
	KWndLabeledButton	m_AddBtn;
	KWndLabeledButton	m_DelBtn;
	KWndLabeledButton	m_DelAllBtn;
	//
	KWndButton	m_PriceCellCheckBox;
	BOOL		m_bPriceCellCheckBox;
	KWndEdit32	m_PriceCellEditBox;
	short			m_nPriceCellEditBox;
	KWndButton	m_LevelCheckBox;
	BOOL		m_bLevelCheckBox;
	KWndEdit32	m_LevelEditBox;
	short			m_nLevelEditBox;
	//
	KWndButton	m_RiAmPeCheckBox;
	BOOL			m_bRiAmPeCheckBox;
	KWndEdit32	m_RiEditBox;
	KWndEdit32	m_AmPeEditBox;
	short	m_nRiEditBox;
	short	m_nAmPeEditBox;
	//
	KWndButton m_SortBagCheckBox;
	BOOL		m_bSortBagCheckBox;
	KWndEdit32	m_SortBagEditBox;
	short			m_nSortBagEditBox;
	//
	KWndButton m_NPickBackLCheckBox; //kh«ng nhÆt vËt phÈm ®en
	BOOL	m_bNPickBackLCheckBox;
	KWndButton	m_NPickBackLBtn;
	//
	KWndButton m_FillterItemCheckBox; //nhÆt r¸c vµ läc ®å cïng mét nót
	BOOL		m_bFillterItemCheckBox;
	//
	KWndButton m_KeepPurpleCheckBox;//nhÆt ®å tÝm
	BOOL		m_bKeepPurpleCheckBox;
	KWndEdit32 m_KeepPurpleEditBox;
	short			m_nKeepPurpleEditBox;
private:
	enum SELECT_PICK_KIND
	{
		PickAll = 0,
		PickItem1,
		PickItem2,
		PickItem3,
		PickItem4,
		PickItem5,
		PickItem6,
		PickItem7,
		PickItem8,
	};
};

//=KUiAutoPlayMove==========================================================================
class KUiAutoPlayMove: public KWndPage
{
public:
	KUiAutoPlayMove();
	void	Initialize();
	void	OnActive();
	void	LoadScheme(const char* pScheme);	
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	UpdateData();
	void	SaveConfig();
	void	LoadConfig();
	BOOL	InsertMoveMpsList(int nSubWorldId, int nMpsX, int nMpsY);
	void		SetMoveMpsList();
	void		PopUpFollow();
	void		ProcessPopUpFollow(int nAction);
private:
	KWndButton	m_FollowCheckBox;
	KWndText32	m_FollowNameTxt;
	KWndButton	m_FollowNameMenuDown;
	KWndEdit32	m_FollowEditBox;
	BOOL		m_bFollowCheckBox;
	char		m_nFollowNameTxt[64];
	short			m_nFollowEditBox;
	//
	KWndButton	m_LoopPosCheckBox;//quanh ®iÓm
	KWndText32	m_LoopPosTxt;
	KWndEdit32	m_LoopPosRangEBox;
	BOOL			m_bLoopPosCheckBox;
	int					m_nXLoopPosTxt;
	int					m_nYLoopPosTxt;
	short				m_nLoopPosRangEBox;
	//
	KWndButton	m_ConstPosCheckBox;//to¹ ®é
	BOOL	m_bConstPosCheckBox;
	KWndList2	m_ConstPosList;
	KWndScrollBar	m_ConstPosScroll;
	KWndLabeledButton m_AddBtn;
	KWndLabeledButton	m_DelBtn;
	KWndLabeledButton	m_DelAllBtn;
	int					m_MoveMpsTrain[defMAX_AUTO_MOVEMPSL][3];
public:
	KUiSceneTimeInfo	m_CurrentMps;
private:
	//
	KWndButton	m_MoveMapsCheckBox;//b¶n ®å
	BOOL			m_bMoveMapsCheckBox;
	KWndText32	m_MoveMapNameTxt;
	short				m_nMoveMapID;
	char				m_charMoveMapNameTxt[64];
	//
	KWndButton	m_IRMoveCheckBox;//lªn ngùa khi di chuyÓn
	KWndButton	m_FMORCheckBox; //®¸nh qu¸i trªn ®­êng ®i
	KWndButton	m_DamnMonterCheckBox;//qu©y qu¸i
	KWndButton	m_TalkAnnyCheckBox;//nãi vu v¬
	BOOL	m_bIRMoveCheckBox;
	BOOL	m_bFMORCheckBox;
	BOOL	m_bDamnMonterCheckBox;
	BOOL	m_bTalkAnnyCheckBox;
	//
	KWndButton	m_FlCaptainCheckBox;
	KWndButton	m_FlAnnyPTCheckBox;
	BOOL	m_bFlCaptainCheckBox;			//theo sau ®éi tr­ëng
	BOOL	m_bFlAnnyPTCheckBox;
};

//=KUiAutoPlayMap==========================================================================
class KUiAutoPlayMap: public KWndPage
{
public:
	KUiAutoPlayMap();
	void	Initialize();
	void	OnActive();
	void	LoadScheme(const char* pScheme);
	KUiSceneTimeInfo	m_CurrentMps;
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	UpdateData();
	void	LoadConfig();
	void	SaveConfig();
	void	PopUpBuyHPSelect();
	void	ProcessBuyHPSelect(int nAction);
	void	PopUpBuyMPSelect();
	void	ProcessBuyMPSelect(int nAction);
	void	PopUpBuyToxicSelect();
	void	ProcessBuyToxicSelect(int nAction);
	void	PopUpStationSelect();
	void	ProcessStationSelect(int nAction);
	BOOL	InsertMoveMpsList(int nSubWorldId, int nMpsX, int nMpsY);
	void		SetMoveMpsList();
private:
	KWndButton	m_ReturnCheckBox;
	KWndButton	m_SellCheckBox;
	KWndButton	m_RepairCheckBox;
	KWndButton	m_QuickRepairCheckBox;
	BOOL		m_bReturnCheckBox;
	BOOL		m_bSellCheckBox;
	BOOL		m_bRepairCheckBox;
	BOOL		m_bQuickRepairCheckBox;
	//
	KWndButton	m_GetMoneyCheckBox;
	KWndEdit32	m_GetMoneyEditBox;
	KWndEdit32	m_PwdMoneyEditBox;
	BOOL	m_bGetMoneyCheckBox;
	short		m_nGetMoneyEditBox;
	int			  m_nPwdMoneyEditBox;
	//
	KWndButton	m_BuyHPCheckBox;
	KWndText32	m_BuyHPTxt;
	KWndButton	m_BuyHPMenuDown;
	KWndEdit32	m_BuyHPEditBox;
	BOOL		m_bBuyHPCheckBox;
	short			m_nBuyHPEditBox;
	BYTE		m_BuyHPTxtSelect;
	//
	KWndButton	m_BuyMPCheckBox;
	KWndText32	m_BuyMPTxt;
	KWndButton	m_BuyMPMenuDown;
	KWndEdit32	m_BuyMPEditBox;
	BOOL		m_bBuyMPCheckBox;
	short			m_nBuyMPEditBox;
	BYTE		m_BuyMPTxtSelect;
	//
	KWndButton	m_BuyToxicCheckBox;
	KWndText32	m_BuyToxicTxt;
	KWndButton	m_BuyToxicMenuDown;
	KWndEdit32	m_BuyToxicEditBox;
	BOOL		m_bBuyToxicCheckBox;
	short			m_nBuyToxicEditBox;
	BYTE		m_BuyToxicTxtSelect;
	//
	KWndButton	m_BuyTownCheckBox;
	KWndEdit32	m_BuyTownEditBox;
	BOOL			m_bBuyTownCheckBox;
	short				m_nBuyTownEditBox;
	//
	KWndText32	m_MapTxt;
	KWndButton	m_MapRunPosCheckBox;
	BOOL			m_bMapRunPosCheckBox;
	short				m_nMapTxtId;
	char				m_cMapTxtName[64];
	//
	KWndList2	m_ConstPosList;
	KWndScrollBar	m_ConstPosScroll;
	int					m_MoveMpsMap[defMAX_AUTO_MOVEMPSL][3];
	//
	KWndLabeledButton m_AddBtn;
	KWndLabeledButton	m_DelBtn;
	KWndLabeledButton	m_DelAllBtn;
	//
	KWndButton	m_KeepMoneyCheckBox;
	KWndEdit32	m_KeepMoneyEditBox;
	KWndButton	m_GetFYCheckBox;
	BOOL	m_bKeepMoneyCheckBox;
	short		m_nKeepMoneyEditBox;
	BOOL	m_bGetFYCheckBox;
	//
	KWndButton	m_GoFarAwayCheckBox;
	KWndText32	m_GoFarAwayTxt;
	KWndButton	m_GoFarAwayMenuDown;
	BOOL		m_bGoFarAwayCheckBox;
	BYTE		m_GoFarAwayTxtSelect;
};

//=KUiAutoPlayTeam==========================================================================
class KUiAutoPlayTeam: public KWndPage
{
public:
	KUiAutoPlayTeam();
	void	Initialize();
	void	OnActive();
	void	LoadScheme(const char* pScheme);	
	void	SaveConfig();
	void	LoadConfig();
	void	PopUpTeamPlayerName();
	void	ProcessTeamPlayerName(int nAction);
	BOOL InsertPlayerListTeam(char *playerName);
	void	SetPlayerListTeam();
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	UpdateData();
private:
	KWndButton	m_AutoCTeamCheckBox;
	KWndButton	m_AlwayLeaderCheckBox;
	KWndButton	m_PTAllCheckBox;
	KWndButton	m_DecAllInviCheckBox;
	BOOL		m_bAutoCTeamCheckBox;
	BOOL		m_bAlwayLeaderCheckBox;
	BOOL		m_bPTAllCheckBox;
	BOOL		m_bDecAllInviCheckBox;
	//
	KWndText32	m_TPlayerNameTxt;
	KWndButton	m_TPNameMenuDown;
	char				m_cTPlayerNameTxt[64];
	//
	KWndList2	m_ConstPosList;
	KWndScrollBar	m_ConstPosScroll;
	char		m_TeamPlayerList[defMAX_AUTO_MOVEMPSL][32];
	//
	KWndLabeledButton	m_AddBtn;
	KWndLabeledButton	m_DelBtn;
	KWndLabeledButton	m_DelAllBtn;
	//
	KWndButton	m_LeaveTeamCheckBox;
	KWndEdit32	m_LeaveTeamEditBox1;
	KWndEdit32	m_LeaveTeamEditBox2;
	BOOL		m_bLeaveTeamCheckBox;
	short			m_nLeaveTeamEditBox1;
	short			m_nLeaveTeamEditBox2;
	//
	KWndButton	m_KickTeamCheckBox;
	KWndEdit32	m_KickTeamEditBox;
	BOOL			m_bKickTeamCheckBox;
	short				m_nKickTeamEditBox;
	//
	KWndButton	m_SCountTeamCheckBox;
	KWndButton	m_PTTongCheckBox;
	BOOL			m_bSCountTeamCheckBox;
	BOOL			m_bPTTongCheckBox;
private:

};

//=KUiAutoPlayBlackItem==========================================================================
class KUiAutoPlayBlackItem: public KWndPage
{
public:
	KUiAutoPlayBlackItem();
	void	Initialize();
	void	OnActive();
	void	LoadScheme(const char* pScheme);	
	void	SaveConfig();
	void	LoadConfig();
	void	PopUpBlackItemName();
	void	ProcessBlackItemName(int nAction);
	BOOL InsertItemListBlack(char *ItemName);
	void	SetItemListBlack();
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	UpdateData();
private:

	KWndText32	m_BlackItemNameTxt;
	KWndButton	m_BlackItemMenuDown;
	char				m_cBlackItemNameTxt[64];
	//
	KWndList2	m_BlackItemConstPosList;
	KWndScrollBar	m_BlackItemConstPosScroll;
	char		m_BlackItemList[defMAX_AUTO_MOVEMPSL][64];
	//
	KWndLabeledButton	m_BlackItemAddBtn;
	KWndLabeledButton	m_BlackItemDelBtn;
	KWndLabeledButton	m_BlackItemCloseBtn;
};

//=KUiAutoPlay==========================================================================
class KUiAutoPlay : public KWndPageSet
{
public:
	static KUiAutoPlay*	OpenWindow();					
	static KUiAutoPlay*	GetIfVisible();					
	static void			CloseWindow(bool bDestroy = false);			
	static void			LoadScheme(const char* pScheme);
	static BOOL		GetActive()
	{
		if(m_pSelf)
			return m_pSelf->m_bActive;
		return FALSE;
	};

	static void	OnActive(BOOL b);
	static void	UpdateSceneTimeInfo(KUiSceneTimeInfo* pInfo);
	static void ShowPageBlackItem(bool b);
private:
	KUiAutoPlay();
	~KUiAutoPlay() {}
	void	Initialize();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);	
	void	LoadScheme(class KIniFile* pIni);
	void	ShowPage(int nSubPage);
	void	UpdateData();
	void	OnSave();
	void	SaveConfig();
	void	LoadConfig();
private:
	static KUiAutoPlay*	m_pSelf;

private:
	KUiAutoPlayFight	m_AutoFightPad;//ChiÕn ®Êu
	KWndLabeledButton	m_AutoFightBtn;
	
	KUiAutoPlayRestore		m_AutoRestorePad;
	KWndLabeledButton	m_AutoRestoreBtn;//Phôc håi
	
	KUiAutoPlayPick		m_AutoPickPad;//NhÆt ®å
	KWndLabeledButton	m_AutoPickBtn;
	
	KUiAutoPlayMove		m_AutoMovePad;
	KWndLabeledButton	m_AutoMoveBtn;//Di chuyÓn
	
	KUiAutoPlayMap		m_AutoMapPad;
	KWndLabeledButton	m_AutoMapBtn;//B¶n ®å
	
	KUiAutoPlayTeam		m_AutoTeamPad;
	KWndLabeledButton	m_AutoTeamBtn;//Tæ ®éi

	KUiAutoPlayBlackItem m_AutoBlackItemPad; //VËt phÈm ®en
	KWndLabeledButton	m_AutoBlackItemBtn;
	
	KWndLabeledButton	m_ActiveBtn;//kÝch ho¹t // dõng
	KWndButton			m_CloseBtn;//®ãng UI autoplay

private:
	enum AUTOPLAY_SUBPAGE
	{
		AP_FIGHT = 1,
		AP_RESTORE,
		AP_PICK,
		AP_MOVE,
		AP_MAP,
		AP_TEAM,
		AP_BLACKITEM,
	};

	BOOL		m_bActive;
	BYTE		m_nPadActive;
};
