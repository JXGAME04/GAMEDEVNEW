//---------------------------------------------------------------------------
// File:	KPlayerAuto.h
// Date:	2020.01.05
// Code:	Fong KiÒu
//	File: KPlayerAuto.h
//---------------------------------------------------------------------------
#ifndef KPLAYERAUTO_H
#define KPLAYERAUTO_H

#ifndef _SERVER

#define	defARRAY_LAG							30
#define AUTO_REACTIVE						  1
#define USE_ITEM									  18 / AUTO_REACTIVE
#define	MAX_HIT_OBJECT						  10
#define	MAX_TIME_RUN_TO_NPC			 (5 * 18) / AUTO_REACTIVE
#define SOUND_RING_TDP						  "\\Sound\\ring-03a.wav"

class K2EquipmentArray
{
public:
	KLinkArray	m_Link;
	int		FindSame(int nIdx);
};

class KPlayerAuto
{
	friend class KPlayer;
private:
	BYTE	mfk_btState;
	BOOL	mfk_bActive;
	BOOL	mfk_bPause;
	int			m_nPlayerIndex;	
	int			m_nIndexFocus;
	int			m_nArrayFocus[8];//m¶ng qu¸i cÇn qu©y auto qu©y qu¸i quay quai
	int			m_nArrayIndex; //thø tù tõng con qu¸i trong m¶ng cÇn qu©y
	int			m_nObjectIndex;
	BYTE	m_nAttackType;
public:
	BOOL	m_bFightCheckBox;	//tù ®¸nh //--------------chiÕn ®Êu-------------------
	short		m_nFightRange;
	BYTE	m_bFightSelect;
	BOOL	m_bFightDistance;	//tiÕp cËn
	short		m_nFightDistance;
	BYTE	m_bFightDistanceSelect;
	BOOL	m_bFightSelfDef;	//tù vÖ
	short		m_nFightSelfDefValue;
	BYTE	m_bFightSelfDefSelect;
	int				m_ArrayStateSkill[defMAX_ARRAY_STATESKILL];
	int				m_LeftSkillID;
	int				m_RightSkillID;
	int				m_SkillSelfDefID; //tù vÖ
	int				m_SkillBossID;	//®¸nh boss
	BYTE		m_ShortcutKeySelect1;
	BYTE		m_ShortcutKeySelect2;
	short			m_nShortcutEditBox1;
	short			m_nShortcutEditBox2;
	BYTE		m_ShortcutOptSelect1;
	BYTE		m_ShortcutOptSelect2;
	BYTE	m_bSkillRightCheckBox;
	BYTE	m_bNRDoSkillCheckBox;
	BOOL	m_bReHPCheckBox;	//--------------phôc håi-------------------
	short		m_nReHPEditBox1;
	short		m_nReHPEditBox2;
	short		m_nReHPEditBox3;
	BOOL	m_bReMPCheckBox;
	short		m_nReMPEditBox1;
	short		m_nReMPEditBox2;
	short		m_nReMPEditBox3;
	BOOL	m_bTownHPVCheckBox;
	short		m_nTownHPVEditBox;
	BOOL	m_bTownMPVCheckBox;
	short		m_nTownMPVEditBox;
	//
	BOOL	m_bTownHPNCheckBox;
	BOOL	m_bTownMPNCheckBox;
	//
	BOOL	m_bTownFCellCheckBox;//day ruong
	BYTE	m_nTownFCellTxtSelect;
	//
	BOOL	m_bTowMoneyCheckBox;//tien nhieu
	int			 m_nTownMoneyEditBox;
	//
	BOOL	m_bTownAbradeCheckBox;//®å h­
	short		m_nTownAbradeEditBox;
	//
	BOOL	m_bUseHPCheckBox;	//c¾n m¸u khi ®Çy r­¬ng
	BOOL	m_bUseAntiToxicCheckBox;
	BOOL	m_bUseExpCheckBox;	//sö dông tiªn th¶o lé
	BOOL	m_bUseSkillCheckBox;	// sö dông x2 skill
	//
	BOOL	m_bBuffCheckBox;
	short		m_nBuffEditBox;
	BOOL	m_bBuffTeamCheckBox;
	//
	BOOL	m_bOpenBagHPCheckBox;
	short		m_nOpenBagHPEditBox;
	//
	BOOL	m_RingTDPCheckBox;
	//
	BOOL	m_bAutoPickCheckBox;		//--------------NhÆt ®å-------------------
	int			 m_nAutoPickEditBox;
	BOOL	m_bPickAllCheckBox;
	BOOL	m_bPickSpeCheckBox;
	BOOL	m_bNoneFightCheckBox;
	int			 m_nPickSelTxtSelect;
	int			 m_FilterMagic[defMAX_AUTO_FILTERL][2];
	BOOL	m_bPriceCellCheckBox;
	short		m_nPriceCellEditBox;
	BOOL	m_bLevelCheckBox;
	short		m_nLevelEditBox;
	BOOL	m_bRiAmPeCheckBox;//gi÷ trang søc
	short		m_nRiEditBox;
	short		m_nAmPeEditBox;
	BOOL	m_bSortBagCheckBox;
	short		m_nSortBagEditBox; //thêi gian xÕp ®å
	BOOL	m_bSortEquipment; //biÕn ®iÒu khiÓn khi ®ang xÕp hµnh trang sö dông néi bé ë class nµy
	BOOL	m_bNPickBackLCheckBox;
	char		m_BlackItemList[defMAX_AUTO_FILTERL][64]; //list blackitem tõ UI chuyÓn qua
	BOOL	m_bFillterItemCheckBox;
	BOOL	m_bKeepPurpleCheckBox;
	short		m_nKeepPurpleEditBox;
	K2EquipmentArray	m_sListEquipment;
	//
	BOOL	m_bFollowCheckBox; //--di chuyÓn
	short		m_nFollowEditBox;
	char		m_nFollowNameTxt[64];
	BOOL	m_bLoopPosCheckBox;
	int			 m_nXLoopPosTxt;
	int			 m_nYLoopPosTxt;
	short		m_nLoopPosRangEBox;
	BOOL	m_bConstPosCheckBox;//theo to¹ ®é
	int			 m_MoveMpsTrain[defMAX_AUTO_MOVEMPSL][3];
	int			 m_MoveStepTrain;
	BOOL	m_bMoveMapsCheckBox;
	short		m_nMoveMapID;
	BOOL	m_bIRMoveCheckBox; //lªn ngùa khi di chuyÓn
	BOOL	m_bFMORCheckBox;
	BOOL	m_bDamnMonterCheckBox;
	BOOL	m_bTalkAnnyCheckBox;//nãi vu v¬
	BOOL	m_bFlCaptainCheckBox;
	BOOL	m_bFlAnnyPTCheckBox;
	//
	BOOL	m_bReturnCheckBox;//--b¶n ®å--
	BOOL	m_bSellCheckBox;
	BOOL	m_bRepairCheckBox;
	BOOL	m_bQuickRepairCheckBox;
	BOOL	m_bGetMoneyCheckBox;
	short		m_nGetMoneyEditBox;
	int			 m_nPwdMoneyEditBox;
	BOOL	m_bBuyHPCheckBox;
	short		m_nBuyHPEditBox;
	char		m_BuyHPTxtSelect[64];
	BOOL	m_bBuyMPCheckBox;
	short		m_nBuyMPEditBox;
	char		m_BuyMPTxtSelect[64];
	BOOL	m_bBuyToxicCheckBox;
	short		m_nBuyToxicEditBox;
	char		m_BuyToxicTxtSelect[64];
	BOOL	m_bBuyTownCheckBox;
	short		m_nBuyTownEditBox;
	BOOL	m_bMapRunPosCheckBox;
	int			 m_nMapTxtId;
	int			 m_MoveMpsMap[defMAX_AUTO_MOVEMPSL][3]; //to¹ ®é ch¹y ra b·i
	int			 m_MoveMapStep;
	BOOL	m_bKeepMoneyCheckBox;
	short		m_nKeepMoneyEditBox;
	BOOL	m_bGetFYCheckBox;		//cÊt ®å vµo r­¬ng
	BOOL	m_bGoFarAwayCheckBox;
	BYTE	 m_GoFarAwayTxtSelect;
	//---tæ ®éi---
	BOOL	m_bAutoCTeamCheckBox;
	BOOL	m_bAlwayLeaderCheckBox;
	BOOL	m_bPTAllCheckBox;
	BOOL	m_bDecAllInviCheckBox;
	BOOL	m_bLeaveTeamCheckBox;
	char		m_AutoPT_PlayerList[defMAX_AUTO_MOVEMPSL][32];//qu¶n lý tæ ®éi
	short		m_nLeaveTeamEditBox1;
	short		m_nLeaveTeamEditBox2;
	int			  m_nLeaveTeamCountDown;
	BOOL	 m_bKickTeamCheckBox;
	short		m_nKickTeamEditBox;
	BOOL	m_bSCountTeamCheckBox;
	BOOL	m_bPTTongCheckBox;
	//Xu ly Lag cua 1 Npc bi tan cong
	int			 m_nLifeNpc;
	int			 m_nArrayLagNpc[defARRAY_LAG];
	int			 m_nTimeRunLag;
	int			 m_nTimeRunNpc;
	// Xu ly lag Object
	int			 m_nHitObject;
	// Hoi phuc
	int			 m_nCoolDownHP1;//b¬m hp1
	int			 m_nCoolDownHP2;//b¬m hp2
	int			 m_nCoolDownMP1;//b¬m nl1
	int			 m_nCoolDownMP2;//b¬m nl2
	int			 m_nCoolDownTW;//tg tdp
	int			 m_nCoolDownTX;//thêi gian gi¶i ®éc
	int			 m_nCoolDownBagHP;//thêi gian më tói d­îc phÈm
	int			 m_nCoolDownSC1;	//thêi gian lu©n chuyÓn hot key 1
	int			 m_nCoolDownSC2;	//thêi gian lu©n chuyÓn hot key 2
	int			 m_nCoolDownSortRE;//thêi gian xÕp hµnh trang
	int			 m_nCoolDownAutoMap; //thêi gian sö dông trong auto map

private:
	enum PLAYER_ATTACK_TYPE
	{
		ATTACK_TYPE_FREE = 0,				//tù do
		ATTACK_TYPE_AROUND_POINT,	//quanh ®iÓm
		ATTACK_TYPE_COORDINATE,			//to¹ ®é
		ATTACK_TYPE_FOLLOW_PEOPLE, //theo sau
		ATTACK_TYPE_MAP_MPS, //ch¹y ra b·i
	};
	//
	enum PLAYER_AUTO_STATE
	{
		STATE_WAITING_JOB = 0,
		STATE_ATTACK_NPC,
		STATE_PICKUP_OBJ,
	};
	//
	enum AUTO_COUNT_DOWN
	{
		TIME_USE_HP1 = 0,
		TIME_USE_HP2,
		TIME_USE_MP1,
		TIME_USE_MP2,
		TIME_USE_TW,
		TIME_USE_TOXIC,
		TIME_USE_BAG_HP,
		TIME_USE_SHORTCUT1,
		TIME_USE_SHORTCUT2,
		TIME_SORT_ROOM_E,
		TIME_AUTO_MAP,
	};

private:
	BOOL	FkAutoMoveMapMps(); //ch¹y to¹ ®é ra b·i
	int			 FkGetMoveMpsMapCount();//get sè to¹ ®é ra b·i
	void		CheckState();
	void		CheckAttackType();
	void		DoShortKeyShortCut();
	void		AutoMakeAwayNpc();//tr¸nh xa npc
	void		ReturnMap();
	void		FkAutoDamnMonter();//qu©y qu¸i
	void		FkAutoShitDown();
	void		MoveTo(int x, int y);
	BOOL	FkAutoMoveMps();
	int			 FkGetMoveMpsTrainCount();
	void		FkAutoIdleTalk();
	BOOL	BuffSkill();
	void		DoAutoX2();
	void		DoAutoX2Skill();
	void		DoActackNpc();
	void		FollowAtackNpc(int nTargetIndex);
	void		RefreshNpc();
	// Cac ham check Npc Lag
	void		AutoCheckNpcLag();
	void		ClearNpcArrLag();
	void		AddNpc2ArrLag(int nIdx);
	// Check lag Object
	void		RefreshObject();
	void		PlayerSetActackObject(int nIdx);
	void		AutoCheckObjectLag();
	void		DoActackObject();
	void		DoRestoreHP();
	BOOL	DoUseHeath();
	void		DoRestoreMP();
	BOOL	DoUseMana();
	void		DoUseItemHMPFCell();	//c¾n m¸u khi ®Çy r­¬ng
	void		DoRestoreToxic();
	BOOL	DoUseToxic();
	void		DoOpenHMPBag(); //tù më tói d­îc phÈm
	void		FKAutoFilterEquip();	// --- NhÆt ®å ---
	void		FKAutoFilterBlackItem();
	BOOL	IsEquipSatisfyCondition(int nIdx);
	BOOL	IsRAPEquip(BYTE btDetail);
	BOOL	IsBlackObjectName(int nObjIndex);
	BOOL	IsBlackItemName(int nItemIndex);
	BOOL	IsRowMagicItem(int nIdx, int row);//kiÓm tra item cã n dßng thuéc tÝnh xanh, vµng, tÝm, b¹ch kim
	void		SortEquipment();
	void		DoSortEquipment();
	void		Func_TwCheck();	//--- Di chuyÓn ----
	void		DoUseTownPortal();
	BOOL	myLockActionTabRecover(AUTO_COUNT_DOWN);
	void		mySetValueCountDown(AUTO_COUNT_DOWN, int);
	void		myCountDownTimerRecover();
	void		myCountDownTimerAutoMap();
	void		AutoSendMsg(char* pszMsg);
	void		DoAutoParty();//--- Tæ ®éi ---
	void		AutoParty();
	void		DoAutoKickTeamOut();
	//
	void		FkDoAutoMap(); //trong thµnh
	BOOL	FkAutoMapCheckWorld();
	void		FkAutoMapSellItem();
	void		FkAutoMapGetMoney();
	void		FkAutoMapRepairItem();
	void		FkAutoMapBuyItemMM();
	void		FkAutoMapBuyItemTDP();
	void		FkAutoMapHoldMoney();
	void		FkAutoMapStoreItem();
	void		FkAutoMapReturn();
	BOOL	FkAutoMapGotoPharmacies();//ch¹y ®Õn hiÖu thuèc
	BOOL	FkAutoMapGotoGroceryStore();//ch¹y ®Õn tiÖm t¹p ho¸
	BOOL	FkAutoMapGotoStoreBox();//ch¹y ®Õn r­¬ng
	BOOL	FkAutoMapGotoStation();//ch¹y ®Õn xa phu
	BOOL	FkAutoCheckItemSell(int nIdx);//kiÓm tra item ®­îc phÐp b¸n
	BOOL	FkAutoCheckItemRepair(int nIdx);//kiÓm tra item ®­îc söa
	BYTE	 FkAutoCountItemName(int nRoom, char* nszItemName);//®Õm sè item theo tªn trong hµnh trang Player
	BOOL	FkAutoCheckItemSellAll(); //b¸n hÕt item trong r­¬ng
	BOOL	FKAutoCheckStoreItemAll(); //cÊt hÕt item vµo r­¬ng
	BOOL	FKAutoCheckStoreItem(int nIdx);//kiÓm tra item ®­îc phÐp cÊt vµo r­¬ng
	BOOL	FkAutoCheckEquipMagic(int nIdx);//kiÓm tra item ®­îc phÐp b¸n theo magic
public:
	void		FkAutoMapSet_StepOne();
	int			 AllItemNameEquipRoom(FKUiObjectName *pList, int nCount);
private:
	BYTE	m_AutoMap_Status;
	enum	AUTO_MAP_STATUS
	{
		AUTO_MAP_STATUS_IDLE = 0,
		AUTO_MAP_STATUS_SELL,
		AUTO_MAP_STATUS_GETMONEY,
		AUTO_MAP_STATUS_REPAIR,
		AUTO_MAP_STATUS_BUYITEM_MM, //mua m¸u mana
		AUTO_MAP_STATUS_BUYITEM_TDP, //mua thæ ®Þa phï
		AUTO_MAP_STATUS_HOLDMONEY,
		AUTO_MAP_STATUS_STOREITEM,
		AUTO_MAP_STATUS_RETURN,
	};
public:
	KPlayerAuto();
	~KPlayerAuto();
	void		Release();
	void		Active();
	BOOL	IsActive() const { return mfk_btState; };
	void		InitFkAuto(BOOL m_bActive);
	void		PauseFkAuto(BOOL bPause);
	void		InitFkAutoPos(bool b);
	
	enum	FKAUTO_NPCTYPE
	{
		NPC_PHARMACIES,
		NPC_GROCERY,
		NPC_STORE_BOX,
		NPC_STATION
	};

	#define	FKAUTO_MAX_POS	7*4
	struct FK_AUTO_POS
	{
		int		 fk_mapID;
		int		 fk_npcType;
		int		 fk_nX[10];
		int		 fk_nY[10];
	};

	FK_AUTO_POS *fk_autopos;
	int			 GetFkAutoPos(int npcType);
};

#endif
#endif
