#ifndef KPlayerH
#define	KPlayerH
//#include <objbase.h>
#include "KWorldMsg.h"
#include "KMessage.h"
#include "KInventory.h"
#include "KPlayerFaction.h"
#include "KPlayerChat.h"
#include "KPlayerTask.h"
#include "KPlayerMenuState.h"
#include "KPlayerTrade.h"
#include "KPlayerTeam.h"
#include "KPlayerPK.h"
#include "KPlayerRepute.h"
#include "KPlayerFuYuan.h"
#include "KPlayerReBorn.h"
#include "KPlayerAuto.h" //fkauto
#include "kautoai.h"
#include "KPlayerTong.h"
#include "KItemList.h"
#include "KNpc.h"
#include "KSkills.h"
#include "KPlayerDef.h"

#define		MAX_ANSWERNUM					50
#define		PLAYER_LIFE_REPLENISH			0
#define		PLAYER_MANA_REPLENISH			0
#define		PLAYER_STAMINA_GAIN				1
#define		PLAYER_STAMINA_LOSS				4

#define		STRENGTH_SET_DAMAGE_VALUE		5
#define		DEXTERITY_SET_DAMAGE_VALUE		5

#define		MAX_AVENGE_NUM					4

#define		MAX_ARRAY_NPC_AUTO				50
#define		AUTO_TIME_LAG					50
#define		AUTO_COUNT_LAG					50
#define		AUTO_TIME_RESET_LAG				10000
#define		MAX_ARRAY_STATESKILL			3

enum	UIInfo 
{
	UI_SELECTDIALOG,
	UI_SAYNEW,
	UI_TRADEDIALOG,
	UI_TALKDIALOG,
	UI_NOTEINFO,
	UI_MSGINFO,
	UI_NEWSINFO,
	UI_NEWSINFO_1,
	UI_PLAYMUSIC,
	UI_OPENTONGUI,
};

enum
{
	Step_Training,
	Step_Repair,
	Step_InventoryMoney,
	Step_InventoryItem,
	Step_Return,
	Step_Completed,
};

typedef struct PLAYER_REVIVAL_POS_DATA
{
	int				m_nSubWorldID;		// ÖØÉúµãµØÍ¼
	int				m_ReviveID;			// ÖØÉúµãË÷Òı
	int				m_nMpsX;			// ÖØÉúµãµØÍ¼Î»ÖÃ x
	int				m_nMpsY;			// ÖØÉúµãµØÍ¼Î»ÖÃ y
} PLAYER_REVIVAL_POS;

typedef struct
{
	int				m_nSubWorldId;		// ´«ËÍÃÅÊÀ½çID
	int				m_nTime;			// ´«ËÍÃÅ±£³ÖÊ±¼ä
	int				m_nMpsX;
	int				m_nMpsY;
} PLAYER_TOWNPORTAL_POS;

typedef struct 
{
	DWORD			m_dwMapID;
	int				m_nX;
	int				m_nY;
} PLAYER_EXCHANGE_POS;

typedef struct
{
	int		m_nBuyIdx;
	DWORD	m_SubWorldID;
	int		m_nMpsX;
	int		m_nMpsY;
	void	Clear() {m_nBuyIdx = -1; m_SubWorldID = -1; m_nMpsX = 0; m_nMpsY = 0;}
} BuySellInfo2;
class KIniFile;

class KEquipmentArray
{
public:
	KLinkArray	m_Link;
	int		FindSame(int nIdx);
};

#ifdef TOOLVERSION
class CORE_API KPlayer
#else
class KPlayer
#endif
{

private:

#ifndef _SERVER
	int				m_nLeftSkillID;
	int				m_nLeftSkillLevel;
	int				m_nRightSkillID;
	int				m_nRightSkillLevel;
	BOOL			m_MouseDown[2];
#endif

#ifdef _SERVER
	PLAYER_REVIVAL_POS		m_sLoginRevivalPos;	
	PLAYER_REVIVAL_POS		m_sDeathRevivalPos;	
	PLAYER_TOWNPORTAL_POS	m_sPortalPos;		
	BOOL			m_bUseReviveIdWhenLogin;
	int 			m_nChestPW;					//mat khau ruong
	int				m_nChangeExtPoint;			// ±ä¶¯µÄµãÊı
#endif
	int				m_ImageId;			// bien id npc image
	int				m_nExtPoint;				// »î¶¯µãÊı
	int				m_nPhysicsSkillID;		//µ±Ç°Íæ¼ÒµÄÎïÀí¹¥»÷¼¼ÄÜ
	int				m_nPeapleIdx;
	int				m_nObjectIdx;
	int				m_nPickObjectIdx;
	int				m_nPlayerIndex;				// ±¾ÊµÀıÔÚ Player Êı×éÖĞµÄÎ»ÖÃ
	KCacheNode *	m_pLastScriptCacheNode;

public:
#ifdef _SERVER
	PLAYER_EXCHANGE_POS		m_sExchangePos;
	KTimerTaskFun	m_TimerTask;
	BOOL			m_bIsQuiting;
	BYTE			m_nLixian;											//#uy thac
	UINT			m_uMustSave;
	DWORD			m_ulLastSaveTime;
	DWORD			m_dwLoginTime;
//	DWORD			m_uLastPingTime;
	void*			m_pStatusLoadPlayerInfo;	//¼ÓÔØÍæ¼ÒĞÅÏ¢Ê±ÓÃ
	BYTE*			m_pCurStatusOffset;			//¶ş½øÖÆÊ±£¬¼ÇÂ¼¶Áµ½Ö¸ÕëÎ»ÖÃÁË
	BOOL			m_bFinishLoading;			//Íê³É¼ÓÔØ
	BYTE			m_SaveBuffer[64 * 1024];	//±£´æ»º³å 
	int				m_nLastNetOperationTime;	//×îºóÒ»´ÎÍøÂç²Ù×÷Ê±¼ä
	BOOL			m_bSleepMode;
	KList			m_PlayerWayPointList;		//
	KList			m_PlayerStationList;
	int				m_nViewEquipTime;			// ×îºóÒ»´Î²ì¿´ËûÈË×°±¸µÄÊ±¼ä
	int				m_nPrePayMoney;
	int				m_nTimeRide;//edit by phong kieu len xuong ngua
	bool		m_nLicReg;
	enum
	{
		FF_CHAT = 0x01,
	};
	int				m_nForbiddenFlag;			// ½ûÖ¹±êÖ¾
#endif

#ifndef _SERVER 
    int                m_nImageNpcID; //id npc 
#endif 

#ifndef _SERVER
	int				m_RunStatus;				// ÊÇÅÜ»¹ÊÇ×ß
	DWORD			m_dwNextLevelLeadExp;		// Í³ÂÊÁ¦ÏÂÒ»¼¶¾­ÑéÖµ
	int				m_nSendMoveFrames;			// ÓÃÓÚ¿ØÖÆ¿Í»§¶ËÏò·şÎñÆ÷·¢ËÍÒÆ¶¯(×ß»òÅÜ)Ğ­ÒéµÄÆµÂÊ£¬Ê¹Ö®²»ÄÜ·¢ËÍ´óÁ¿µÄÒÆ¶¯Ğ­Òé£¬¼õĞ¡´ø¿íÑ¹Á¦
	int				m_nLastNpcIndex;
	BOOL			m_bHideNpc;
	BOOL			m_bHidePlayer;
	BOOL			m_bHideSkill;
	TMissionLadderSelfInfo m_MissionData;		//add by phong kiÒu using tèng kim
	TMissionLadderInfo m_MissionRank[MISSION_STATNUM];
#endif

	KIndexNode		m_Node;
	GUID			m_Guid;
	BOOL			m_bExchangeServer;
	int				m_DebugMode;
	DWORD			m_dwID;						// Íæ¼ÒµÄ32Î»ID
	int				m_nIndex;					// Íæ¼ÒµÄNpc±àºÅ
	int				m_nNetConnectIdx;			// µÚ¼¸¸öÍøÂçÁ¬½Ó
	char			m_nPlayerHWID[64];			// Player HWID 64 len edit by phong kieu
	KItemList		m_ItemList;					// Íæ¼ÒµÄ×°±¸ÁĞ±í
	BuySellInfo		m_BuyInfo;					// ½øĞĞµÄ½»Ò×ÁĞ±í
	KPlayerMenuState	m_cMenuState;			// ÊÇ·ñ´¦ÓÚ½»Ò×»ò¶ÓÎé¿ª·Å×´Ì¬
	KTrade			m_cTrade;					// ½»Ò×Ä£¿é
	int				m_nAttributePoint;			// Î´·ÖÅäÊôĞÔµã
	int				m_nSkillPoint;				// Î´·ÖÅä¼¼ÄÜµã
	int				m_nStrength;				// Íæ¼ÒµÄ»ù±¾Á¦Á¿£¨¾ö¶¨»ù±¾ÉËº¦£©
	int				m_nDexterity;				// Íæ¼ÒµÄ»ù±¾Ãô½İ£¨¾ö¶¨ÃüÖĞ¡¢ÌåÁ¦£©
	int				m_nVitality;				// Íæ¼ÒµÄ»ù±¾»îÁ¦£¨¾ö¶¨ÉúÃü¡¢ÌåÁ¦£©
	int				m_nEngergy;					// Íæ¼ÒµÄ»ù±¾¾«Á¦£¨¾ö¶¨ÄÚÁ¦£©
	int				m_nLucky;					// Íæ¼ÒµÄ»ù±¾ÔËÆø£¨Ó°ÏìµÃµ½×°±¸µÄºÃ»µ£©
	BYTE			m_btChatSpecialChannel;		
	int				m_nUpExp;
	int				m_nCurStrength;				// Suc manh
	int				m_nCurDexterity;			// Than phap
	int				m_nCurVitality;				// Sinh khi
	int				m_nCurEngergy;				// Noi cong
	int				m_nCurLucky;				// May man
	double				m_nExp;						// µ±Ç°¾­ÑéÖµ(µ±Ç°µÈ¼¶ÔÚnpcÉíÉÏ)
	double				m_nNextLevelExp;			// ÏÂÒ»¼¶¾­ÑéÖµ
	int				m_CUnlocked;				// trang thai khoa ruong player
	char			szStringInput[64];
	int				m_nStringNum;
	DWORD			m_dwLeadExp;				// Í³ÂÊÁ¦¾­ÑéÖµ
	DWORD			m_dwLeadLevel;				// Í³ÂÊÁ¦µÈ¼¶
	char			m_PlayerName[32];
	char			m_AccoutName[32];			//edit by phong kieu khai bao bien m_AccoutName
	BYTE		m_ImagePlayer;
	char			m_PlayerMateName[32];//#MateName
	KPlayerTeam		m_cTeam;					// Íæ¼ÒµÄ×é¶ÓĞÅÏ¢
	KPlayerFaction	m_cFaction;					// Íæ¼ÒµÄÃÅÅÉĞÅÏ¢
	KPlayerChat		m_cChat;
	KPlayerTask		m_cTask;					// Íæ¼ÒÈÎÎñÏµÍ³(±äÁ¿)
	KPlayerPK		m_cPK;						// PK¹ØÏµ´¦Àí
	KPlayerRepute	m_cRepute;						// danh vong
	KPlayerFuYuan	m_cFuYuan;						// phuc duyen
	KPlayerReBorn	m_cReBorn;						// trung sinh
	KPlayerTong		m_cTong;					
#ifndef _SERVER
	KPlayerAuto		m_cAuto;			//fkauto
	KAutoMove		m_cAutoMove;
#endif
	DWORD			m_dwDeathScriptId;			
	DWORD			m_dwDamageScriptId;			
	DWORD			m_dwGiveBoxId;
	DWORD			m_dwTimeBoxId;
	DWORD			m_dwStrBoxId;
	DWORD			m_dwNumberBoxId;
	DWORD			m_dwLogoutScriptID;
	DWORD			m_dwRewardId;
	DWORD			m_dwRewardExId;
	DWORD			m_dwTrembleItemId;//kh¶m n¹m
	
	char			m_szTaskAnswerFun[MAX_ANSWERNUM][32];
	int				m_nAvailableAnswerNum;
	bool			m_bWaitingPlayerFeedBack;	
//	DWORD			m_dwOutOfDateFeedBackTime;	
	BYTE			m_btTryExecuteScriptTimes;	
	//char			m_CurScriptName[128];
	int				m_nWorldStat;						//xÕp h¹ng thÕ giíi
	int				m_nSectStat;
	int				m_nTimeChangePK;
// auto playgame
	BOOL			m_bPriorityUseMouse;
private:	
	BOOL			m_bActiveAuto;
#ifndef _SEVER
	BOOL			m_bSortEquipment;
	int				m_nReturnPortalStep;
	int				m_nCurReturnPortalSec;
	KEquipmentArray	m_sListEquipment;
	int				m_nTimeCheckFollowPeopleIdx;
	int				m_nTimePriorityUseMouse;
	int				m_LifeAuto;
	int				m_ManaAuto;
	int				m_TownAuto;
	int				m_nTimeBuffSkillAuto;
	int				m_nPickAreaItem;
	int				m_nCheckTimeBuffSkillAuto;
	int				m_PosXAuto;
	int				m_PosYAuto;
	BOOL			m_bActacker;
	int				m_Actacker;
	//int				m_TimeUseItem;
	int				m_ArrayNpcNeast[MAX_ARRAY_NPC_AUTO];
	int				m_ArrayNpcLag[MAX_ARRAY_NPC_AUTO];
	int				m_Count_Acttack_Lag;
	int				m_ArrayTimeNpcLag[MAX_ARRAY_NPC_AUTO];
	int				m_ArrayTimeObjectLag[MAX_ARRAY_NPC_AUTO];
	int				m_nLifeLag;
	int				m_nTimeRunLag;
	BOOL			m_bObject;
	int				m_nObject;
	int				m_ArrayObjectNeast[MAX_ARRAY_NPC_AUTO];
	int				m_ArrayObjectLag[MAX_ARRAY_NPC_AUTO];
public:
	BOOL			m_bBuffSkill[3];
	BOOL			m_AutoAttack;
	BOOL			m_bFollowPeople;
	int				m_FollowPeopleIdx;
	char			m_FollowPeopleName[32];
	BOOL			m_bAutoParty;
	BOOL			m_bActiveAutoParty;
	BOOL			m_bAutoAccecptAll;//tù ®éng mêi tÊt c¶
	BOOL			m_bAutoMove;
	BOOL			m_bPickItem;
	BYTE			m_btPickUpKind;
	BYTE			m_btDurabilityItem;
	BYTE			m_btDurabilityValue;
	BYTE			m_AutoUseTTL;
	int				m_AuraSkill[2];
	BOOL			m_bActiveSwitchAura;
	int 			m_AutoLifeReplenishP; //nga my buff theo phÇn tr¨m m¸u
	BOOL 			m_AutoLifeReplenish;
	BOOL			m_AutoBuffTeam; //nga my buff m¸u cho tæ ®éi
	int				m_MoveMps[defMAX_AUTO_MOVEMPSL][3];
	int				m_MoveStep;
	char				m_AutoPT_PlayerList[defMAX_AUTO_MOVEMPSL][32];//qu¶n lı tæ ®éi
	BOOL			m_AutoMove;
	BOOL			m_bFilterEquipment;
	int				m_FilterMagic[defMAX_AUTO_FILTERL][2];
	unsigned int	m_SpaceActionTime;
	BOOL			m_SaveRAP;
	BOOL			m_EatLife;
	BOOL			m_EatMana;
	unsigned int	m_LifeTimeUse;
	unsigned int	m_ManaTimeUse;
	int				m_LifeAutoV;
	int				m_ManaAutoV;
	unsigned int	m_LifeCountDown;
	unsigned int	m_ManaCountDown;
	unsigned int	m_PortalCountDown;
	unsigned int    m_CheckTSCountDown;
	unsigned int	m_AntiPoisonCountDown;
	unsigned int	m_TTLCountDown;
	unsigned int	m_TuiDPhamCountDown;
	unsigned int	m_ReturnSetpCountDown;
	unsigned int	m_SortEQCountDown;
	BOOL			m_TPLife;
	int				m_TPLifeV;
	BOOL			m_TPMana;
	int				m_TPManaV;
	BOOL			m_TPNotMedicineBlood;
	BOOL			m_TPNotMedicineMana;
	BOOL			m_TPHightMoney;
	int				m_TPHightMoneyV;
	BOOL			m_AutoAntiPoison;
	BOOL			m_AutoTTL;
	BOOL			m_bAttackAround;
	int				m_RadiusAuto;
	int				m_DistanceAuto;
	BOOL			m_bFightDistance;
	int				m_ArrayStateSkill[MAX_ARRAY_NPC_AUTO];
	BOOL			m_bAttackPeople;
	BOOL			m_bAttackNpc;
	BOOL			m_bBuyEquip;
	BOOL			m_bInventoryMoney;
	BOOL			m_bInventoryItem;
	BOOL			m_bRepairEquip;
	BOOL			m_bReturnPortal;
	BOOL			m_Auto_TuiDuocPham;
	BOOL			m_Auto_BanItem;
	BOOL			m_bSaveJewelry;//gi÷ trang søc
	//BOOL			m_Auto_PickInFightState;//nhÆt trong thµnh
	BOOL			m_Auto_SkillRight;
	BOOL			m_bSortEquipment_Active;
	int					  m_dwEquipExpandTime;//thoi gian su dung hanh trang mo rong
#endif
// end
public:

	KPlayer();
	~KPlayer();
	// auto playgame
	void			SetAutoFlag(BOOL nbAuto);
	BOOL			GetAutoFlag() { return m_bActiveAuto; };
	BOOL			GetFightFlag() { return m_AutoAttack; };
	// end
	void			SetPlayerIndex(int nNo);					// Éè¶¨ m_nPlayerIndex
	void			GetAboutPos(KMapPos *pMapPos);			// »ñµÃÍæ¼Ò¸½½üÒ»¸ö¿ÕÎ»ÖÃ
	int				GetPlayerIndex();							// »ñµÃ±¾ÊµÀıÔÚ Player Êı×éÖĞµÄÎ»ÖÃ
	DWORD			GetPlayerID(){return m_dwID;};
	BYTE			GetFirstAddFaction()
	{
		return (BYTE)this->m_cFaction.m_nFirstAddFaction;
	}
	void			GetFactionName(char *lpszName, int nSize);	// »ñµÃµ±Ç°ÃÅÅÉÃû³Æ not end
	void			GetFactionValueName(char *lpszName, int nSize);	// »ñµÃµ±Ç°ÃÅÅÉÃû³Æ not end
	int				GetFactionNo();
	void			ChatFriendOnLine(DWORD dwID, int nFriendIdx);// »ñµÃÍ¨ÖªÄ³ºÃÓÑÉÏÏßÁË
	BOOL			ExecuteScript(char * ScriptFileName, char * szFunName, int nParam = 0);
	BOOL			ExecuteScript(char * ScriptFileName, char * szFunName, char * szParams);
	BOOL			ExecuteScript(DWORD dwScriptId, char * szFunName, char *  szParams);
	BOOL			ExecuteScript(DWORD dwScriptId,  char * szFunName, int nParam);

	BOOL			ExecuteScript2(char * ScriptFileName, char * szFunName, int nParam1 = 0, int nParam2 = 0);
	BOOL			ExecuteScript2(char * ScriptFileName, char * szFunName, char * szParams1, char * szParams2);
	BOOL			ExecuteScript2(DWORD dwScriptId, char * szFunName, char *  szParams1, char *  szParams2);
	BOOL			ExecuteScript2(DWORD dwScriptId,  char * szFunName, int nParam1, int nParam2);

	BOOL			ExecuteScript3(char * ScriptFileName, char * szFunName, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0);
	BOOL			ExecuteScript3(char * ScriptFileName, char * szFunName, char * szParams1, char * szParams2, char * szParams3);
	BOOL			ExecuteScript3(DWORD dwScriptId, char * szFunName, char *  szParams1, char *  szParams2, char * szParams3);
	BOOL			ExecuteScript3(DWORD dwScriptId,  char * szFunName, int nParam1, int nParam2, int nParam3);

	BOOL			DoScript(char * ScriptCommand);				//Ö´ĞĞÄ³¸ö½Å±¾Ö¸Áî
	void			ChangeCurStrength(int nData);			// ¸Ä±äµ±Ç°Á¦Á¿(µ± nData Ğ¡ÓÚ 0 Ê±£¬¼õÉÙ)
	void			ChangeCurDexterity(int nData);			// ¸Ä±äµ±Ç°Ãô½İ(µ± nData Ğ¡ÓÚ 0 Ê±£¬¼õÉÙ)
	void			ChangeCurVitality(int nData);			// ¸Ä±äµ±Ç°»îÁ¦(µ± nData Ğ¡ÓÚ 0 Ê±£¬¼õÉÙ)
	void			ChangeCurEngergy(int nData);			// ¸Ä±äµ±Ç°¾«Á¦(µ± nData Ğ¡ÓÚ 0 Ê±£¬¼õÉÙ)
	BOOL			ExecuteScript(char * ScriptFileName);
	void			Release();
	void			Active();								
	void			ProcessMsg(KWorldMsgNode *lpMsg);		
	LPSTR			GetPlayerName() { return m_PlayerName; };
	LPSTR			GetPlayerAccount() { return m_AccoutName; };//edit by phong kieu them ham getplayeraccount
	BOOL			NewPlayerGetBaseAttribute(int Series);	// ĞÂÍæ¼ÒµÇÂ½Ê±¸ù¾İÎåĞĞÊôĞÔ²úÉú Á¦Á¿ Ãô½İ »îÁ¦ ¾«Á¦ ËÄÏîÊıÖµ
	void			AddBaseLucky(int nData);				// Ôö¼Ó»ù±¾ÔËÆø
	void			AddExp(int nExp, int nTarLevel, BOOL bCheck = FALSE);		// Ôö¼Ó¾­Ñé(Ô­Ê¼Êı¾İ£¬»¹Î´¾­¹ı´¦Àí)
	void			AddSelfExp(int nExp, int nTarLevel);
	void			DirectAddExp(double nExp);					// Ö±½ÓÔö¼Ó¾­ÑéÖµ£¬²»¿¼ÂÇÆäËûÒòËØ
	void			LevelUp();								// ÉıÒ»¼¶
	void			AddLeadExp(int nExp);					// Ôö¼ÓÍ³ÂÊÁ¦¾­Ñé
	void			UpdataCurData();
	void			ReCalcEquip();							// ÖØĞÂ¼ÆËãÉíÉÏµÄ×°±¸
	void			ReCalcState();							// ÖØĞÂ¼ÆËãÉíÉÏµÄ×´Ì¬
	void			ChangePlayerCamp(int nCamp);			// ¸Ä±äÍæ¼ÒÕóÓª
	void			Revive(int nType);						// ÖØÉú	
	BOOL			CheckTrading();
	void			SetFirstDamage();
	void			SetBaseAttackRating();
	void			SetBaseDefence();
	void			SetBaseResistData();
	void			SetBaseSpeedAndRadius();
	int			ResetProp();// tay tiem nang
	int				GetPhysicsSkillId(){return m_nPhysicsSkillID;};
	void            CheckRideHorse(BOOL nFlagRide); // len xuong ngua
	void			SetImageNpcId(int nNumber); // set hinh anh npc cho say new
	void			SetPhysicsSkillId(int nPhysicsSkillID)
	{
		if (nPhysicsSkillID <= 0) return ;
		ISkill * pISkill =  g_SkillManager.GetSkill(nPhysicsSkillID, 1);
		if (!pISkill) 
            return ;
		
        if (nPhysicsSkillID > 0 && pISkill->IsPhysical())
			m_nPhysicsSkillID = nPhysicsSkillID;
	};
	void 			ProcessDouble(); //#time x2 Exp chÕt kh«ng mÊt
#ifndef _SERVER
	void		   SortEquipment();
	void		   SetSortEquipment(BOOL bFlag);
	void			AutoFllowPeople(int h, int k);
	BOOL		CheckEquip(BYTE btDetail);
	BOOL		CheckEquipMagic(int nIdx);
	BOOL			ReturnFromPortal();
	BOOL			InventoryItem();
	BOOL			AutoSellItem();
	int				GetTargetNpc() { return m_nPeapleIdx; };
	int				GetTargetObj() { return m_nObjectIdx; };
	void			SetTargetNpc(int n) { m_nPeapleIdx = n; };
	void			SetTargetObj(int n) { m_nObjectIdx = n; };
	void			FindSelectNpc(int x, int y, int nRelation);
	void			FindSelectObject(int x, int y);
	void			Walk(int nDir, int nSpeed);	
	void			TurnLeft();
	void			TurnRight();
	void			TurnBack();
	void			DrawSelectInfo();
	void			DrawSelectInfo_Backup();
	BOOL			ConformIdx(int nIdx);	
	void			GetEchoDamage(int* nMin, int* nMax, int nType);// »ñÈ¡½çÃæĞèÒªÏÔÊ¾µÄÉËº¦Öµ
	void			ProcessInputMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);// ´¦Àí¼üÅÌÊó±êÏûÏ¢
	void			RecvSyncData();								// ½ÓÊÕÍ¬²½Êı¾İ
	void			GetAutoQDXY(char* szBufferXY)
	{
		if(m_PosXAuto == 0 || m_PosYAuto == 0)
		{
			int nDesX = 0;
			int nDesY = 0;
			Npc[m_nIndex].GetMpsPos(&nDesX, &nDesY);
			m_PosXAuto = nDesX;
			m_PosYAuto = nDesY;
		}
		sprintf(szBufferXY, "  [%d/%d]", m_PosXAuto/8/32, m_PosYAuto/16/32);
	};
	void			ApplyTeamInfo(DWORD dwNpcID);				// Ïò·şÎñÆ÷ÉêÇë²éÑ¯Ä³¸önpcËùÔÚ¶ÓÎéµÄĞÅÏ¢
	void			ApplySelfTeamInfo();						// Ïò·şÎñÆ÷ÉêÇë²éÑ¯Íæ¼Ò×ÔÉíµÄ¶ÓÎéÇé¿ö
	BOOL			ApplyCreateTeam();
	BOOL			ApplyTeamOpenClose(BOOL bFlag);				// ¶Ó³¤Ïò·şÎñÆ÷ÉêÇë¿ª·Å¡¢¹Ø±Õ¶ÓÎéÊÇ·ñÔÊĞí¼ÓÈë³ÉÔ±×´Ì¬
	void			ApplyAddTeam(int nNpcIndex);				// Íæ¼ÒÏò·şÎñÆ÷ÉêÇë¼ÓÈëÄ³¸ö¶ÓÎé
	void			AcceptTeamMember(DWORD dwNpcID);			// Íæ¼ÒÍ¨Öª·şÎñÆ÷½ÓÊÜÄ³¸önpcÎª¶ÓÎé³ÉÔ±
	void			TeamDropApplyOne(DWORD dwNpcID);			// ¶Ó³¤É¾³ı¼ÓÈë¶ÓÎéÉêÇëÁĞ±íÖĞµÄÄ³¸önpc
	void			LeaveTeam();								// Í¨Öª·şÎñÆ÷±¾Íæ¼ÒÀë¿ª¶ÓÎé
	void			TeamKickMember(DWORD dwNpcID);				// ¶Ó³¤Í¨Öª·şÎñÆ÷Ìß³ıÄ³¸ö¶ÓÔ±
	void			ApplyTeamChangeCaptain(DWORD dwNpcID);		// ¶Ó³¤Ïò·şÎñÆ÷ÉêÇë°Ñ×Ô¼ºµÄ¶Ó³¤Éí·İ½»¸ø±ğµÄ¶ÓÔ±
	void			ApplyTeamDismiss();							// 
	void			ApplySetPK(BOOL bPK);						// pk
	void			ApplyFactionData();							// 
	void			SendChat(KUiMsgParam *pMsg, char *lpszSentence);// 
	void			ApplyAddBaseAttribute(int nAttribute, int nNo);// ¶Ó³¤Ïò·şÎñÆ÷ÉêÇëÔö¼ÓËÄÏîÊôĞÔÖĞÄ³Ò»ÏîµÄµãÊı(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
	BOOL			ApplyAddSkillLevel(int nSkillID, int nAddPoint);// 
	BOOL			ApplyUseItem(int nItemID, ItemPos SrcPos);	// 
	BOOL			ApplyAutoMoveItem(int nItemID, ItemPos SrcPos); // chuyÓn item tõ hµnh trang vµo r­¬ng
	void			PickUpObj(int nObjIndex);					// 
	void			ObjMouseClick(int nObjIndex);				// 
	void			MoveItem(ItemPos DownPos, ItemPos UpPos);	// DownPos UpPos
	int				ThrowAwayItem();
	int				ThrowAllItem(int nId, int nUgen, int nRegion);
	void			ChatAddFriend(int nPlayerIdx);				// 
	void			ChatRefuseFriend(int nPlayerIdx);			// 
	void			TradeApplyOpen(char *lpszSentence, int nLength);
	void			TradeApplyClose();
	void			TradeApplyStart(int nNpcIdx);
	BOOL			TradeMoveMoney(int nMoney);						
	void			TradeDecision(int nDecision);				// if nDecision == 0   if nDecision == 1   if nDecision == 2 
	void			TradeApplyLock(int nLockOrNot);				// 
	void			SetChatCurChannel(int nChannelNo);			// 
	void			TeamInviteAdd(DWORD dwNpcID);				// 
	void			SetLeftSkill(int nSkillID);
	void			SetRightSkill(int nSkillID);
	void			UpdateWeaponSkill();
	int				GetLeftSkill(){return m_nLeftSkillID;};
	int				GetRightSkill(){return m_nRightSkillID;};
	void			SetDefaultImmedSkill();
	void			s2cApplyAddTeam(BYTE* pProtocol);			
	void			s2cTradeChangeState(BYTE* pMsg);
	void			s2cTradeMoneySync(BYTE* pMsg);
	void			s2cTradeDecision(BYTE* pMsg);				
	void			SyncCurPlayer(BYTE* pMsg);
	void			s2cLevelUp(BYTE* pMsg);
	void			s2cGetCurAttribute(BYTE* pMsg);
	void			s2cSetExp(double nExp);
	void			s2cSyncMoney(BYTE* pMsg);
	void			s2cTradeApplyStart(BYTE* pMsg);
	void			CheckObject(int nIdx);
	BOOL			AutoUseItem(int nGenre, int nDetailType, int nParticular);
	BOOL			AutoCheckItem(int nGenre, int nDetailType, int nParticular);
	void			SetExtPoint(int nChangePoint)
	{
		m_nExtPoint = nChangePoint;
	}
#endif

	int				GetExtPoint();

#ifdef _SERVER
	void			AddSkillExp120(int nExp);
	void			AddSkillExp90(int nExp);
	void			SetExtPoint(int nPoint, int nChangePoint);
	BOOL			PayExtPoint(int nPoint);
	int				GetExtPointChanged();
	void			AddExtPoint(int nPoint, int nChangePoint);
	void			RepairItem(DWORD dwItemID);
	void			ServerBreakItem(DWORD dwItemID, int nNum, bool isbreakall);
	BOOL		PrePay(int nMoney);
	void			SetLastNetOperationTime(int nTime);
	int				FindAroundPlayer(DWORD dwNpcID);		
	int				FindAroundNpc(DWORD dwNpcID);		
	BOOL			CheckPlayerAround(int nPlayerIdx);	
	int				FindNearNpc(int nNpcSettingIdx, int nDistance = 0);
	int				FindNearNpc(const char* szName, int nDistance = 0);
	int				FindNearNpc(DWORD dwID, int nDistance = 0);
	BOOL			IsExchangingServer();
	void			TobeExchangeServer(DWORD dwMapID, int nX, int nY);
//	void			UpdateEnterGamePos(DWORD dwSubWorldID, int nX, int nY, int nFightMode);
	BOOL			IsWaitingRemove();
	BOOL			IsLoginTimeOut();
	void			WaitForRemove();
	void			LoginTimeOut();
	void			UseTownPortal();
	void			BackToTownPortal(int nIdSubWorld = 0);
	void			GetLoginRevivalPos(int *lpnSubWorld, int *lpnMpsX, int *lpnMpsY);		// »ñÈ¡Íæ¼ÒµÇÈëÖØÉúµãÎ»ÖÃ
	void			GetDeathRevivalPos(int *lpnSubWorld, int *lpnMpsX, int *lpnMpsY);		// »ñÈ¡Íæ¼ÒËÀÍöÖØÉúµãÎ»ÖÃ
	void			SetRevivalPos(int nSubWorld, int nRevalId);								// Éè¶¨Íæ¼ÒÖØÉúµãID
	BOOL			Save();									// ±£´æÍæ¼ÒÊı¾İ
	BOOL			CanSave();
	void			ProcessUser();
	BOOL			SendSyncData(int &nStep, unsigned int &nParam);	// ·¢ËÍÍ¬²½Êı¾İ
	BOOL			SendSyncData_Skill();					// ·¢ËÍÍ¬²½Êı¾İ - ¼¼ÄÜ
	void			SendCurNormalSyncData();				// ·¢ËÍÆ½Ê±¸ø×Ô¼ºµÄÍ¬²½Êı¾İ
	void			SetChatForbiddenFlag(int nFlag)
	{
		m_nForbiddenFlag = nFlag;
	};

	BOOL			AddFaction(char *lpszFactionName);		// ¼ÓÈëÃÅÅÉ
	BOOL			AddFaction(int nFactionID);				// ¼ÓÈëÃÅÅÉ
	BOOL			LeaveCurFaction();						// Àë¿ªÃÅÅÉ
	BOOL			CurFactionOpenSkill(int nLevel);		// ¿ª·Åµ±Ç°ÃÅÅÉÄ³¸öµÈ¼¶µÄ¼¼ÄÜ

	void			TaskClearTempVal();						// Çå¿ÕÈÎÎñÁÙÊ±±äÁ¿
	DWORD				TaskGetSaveVal(int nNo);				// µÃµ½ÈÎÎñÍê³ÉÇé¿ö
	void			TaskSetSaveVal(int nNo, DWORD bFlag);	// Éè¶¨ÈÎÎñÍê³ÉÇé¿ö
	DWORD				TaskGetClearVal(int nNo);				// µÃµ½ÈÎÎñÁÙÊ±¹ı³Ì¿ØÖÆ±äÁ¿Öµ
	void			TaskSetClearVal(int nNo, DWORD nVal);		// Éè¶¨ÈÎÎñÁÙÊ±¹ı³Ì¿ØÖÆ±äÁ¿Öµ
	//void			SetTimeTaskTime(DWORD time){m_dwTimeTaskTime = time;};
	void			BuyItem(BYTE* pProtocol);
	void			SellItem(BYTE* pProtocol);
	void			AutoSellItem(BYTE* pProtocol);
	void			QuitGame(int nQuitType);				// ÍË³öÓÎÏ·
	void			S2CSendTeamInfo(BYTE* pProtocol);		// ÊÕµ½¿Í»§¶Ë²éÑ¯Ä³¸önpc×é¶ÓĞÅÏ¢µÄÉêÇëºó£¬ÏòÕâ¸ö¿Í»§¶Ë·¢ËÍ¶ÓÎéĞÅÏ¢
	void			SendSelfTeamInfo();						// ·şÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍ¶ÓÎéĞÅÏ¢
	BOOL			CreateTeam(BYTE* pProtocol);			
	BOOL			SetTeamState(BYTE* pProtocol);			// ÊÕµ½¿Í»§¶ËÇëÇó¿ª·Å¡¢¹Ø±Õ±¾player¶ÓÎéÊÇ·ñÔÊĞí¶ÓÔ±¼ÓÈë×´Ì¬
	BOOL			S2CSendAddTeamInfo(BYTE* pProtocol);	// ÊÕµ½¿Í»§¶ËÇëÇó¼ÓÈëÒ»Ö§¶ÓÎé
	BOOL			AddTeamMember(BYTE* pProtocol);			// ¶Ó³¤Í¨Öª·şÎñÆ÷½ÓÊÜÄ³¸önpcÎª¶ÓÎé³ÉÔ±
	void			LeaveTeam(BYTE* pProtocol);				// ÊÕµ½¿Í»§¶Ë¶ÓÔ±Í¨ÖªÀë¿ª¶ÓÎé
	void			TeamKickOne(BYTE* pProtocol);			// ÊÕµ½¿Í»§¶Ë¶Ó³¤Í¨ÖªÌß³öÄ³¸ö¶ÓÔ±
	void			TeamChangeCaptain(BYTE* pProtocol);		// ÊÕµ½¿Í»§¶Ë¶Ó³¤Í¨Öª°Ñ¶Ó³¤Éí·İ½»¸øÄ³¸ö¶ÓÔ±
	void			TeamDismiss(BYTE* pProtocol);			// ÊÕµ½¿Í»§¶Ë¶Ó³¤ÇëÇó½âÉ¢¶ÓÎé
	void			SetPK(BYTE* pProtocol);					// ÊÕµ½¿Í»§¶ËÇëÇóÉè¶¨PK×´Ì¬
	void			SendFactionData(BYTE* pProtocol);		// ÊÕµ½¿Í»§¶ËÇëÇó»ñµÃÃÅÅÉÊı¾İ
	void			ServerSendChat(BYTE* pProtocol);		// ÊÕµ½¿Í»§¶Ë·¢À´µÄÁÄÌìÓï¾ä
	void			AddBaseAttribute(BYTE* pProtocol);		// ÊÕµ½¿Í»§¶ËÒªÇóÔö¼Ó»ù±¾ÊôĞÔµã(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
	void			ResetBaseAttribute(BYTE* pProtocol);	// ham moi
	void			AddSkillPoint(BYTE* pProtocol);			// ÊÕµ½¿Í»§¶ËÒªÇóÔö¼ÓÄ³¸ö¼¼ÄÜµÄµãÊı
	void			IncSkillLevel(int nSkillId, int nAddLevel);
	void			IncSkillExp(int nSkillId, int nAddExp);
	BOOL			ServerPickUpItem(BYTE* pProtocol);		// ÊÕµ½¿Í»§¶ËÏûÏ¢Êó±êµã»÷Ä³¸öobj¼ğÆğ×°±¸»ò½ğÇ®
	void			EatItem(BYTE* pProtocol);				// ÊÕµ½¿Í»§¶ËÏûÏ¢³ÔÒ©
	void			ServerMoveItem(BYTE* pProtocol);		// ÊÕµ½¿Í»§¶ËÏûÏ¢ÒÆ¶¯ÎïÆ·
	void			ServerThrowAwayItem(BYTE* pProtocol);	// 
	void			ServerThrowAllItem(BYTE* pProtocol);
	void			ChatSetTakeChannel(BYTE* pProtocol);	// ÊÕµ½¿Í»§¶ËÏûÏ¢Éè¶¨ÁÄÌì¶©ÔÄÆµµÀ
	void			ChatTransmitApplyAddFriend(BYTE* pProtocol);// ÊÕµ½¿Í»§¶ËÇëÇó×ª·¢ÁÄÌìÌí¼ÓºÃÓÑĞÅÏ¢
	BOOL			ChatAddFriend(BYTE* pProtocol);			// ÊÕµ½¿Í»§¶ËÏûÏ¢Ìí¼ÓÁÄÌìºÃÓÑ
	void			ChatRefuseFriend(BYTE* pProtocol);		// ÊÕµ½¿Í»§¶ËÏûÏ¢¾Ü¾øÌí¼ÓÁÄÌìºÃÓÑ
	void			ChatResendAllFriend(BYTE* pProtocol);
	void			ChatSendOneFriendData(BYTE* pProtocol);
	void			ChatDeleteFriend(BYTE* pProtocol);		// 
	void			ChatRedeleteFriend(BYTE* pProtocol);	// 
	void			TradeApplyOpen(BYTE* pProtocol);		// 
	void			TradeApplyClose(BYTE* pProtocol);		// 
	void			TradeApplyStart(BYTE* pProtocol);		// 
	void			TradeMoveMoney(BYTE* pProtocol);		
	void			TradeDecision(BYTE* pProtocol);			// 
	void			c2sTradeReplyStart(BYTE* pProtocol);
	void			SyncTradeState();						// 
	void			SendEquipItemInfo(int nTargetPlayer);	// 
	void			RecoveryBox(DWORD dwID, int nX, int nY);
	void			GetCityOwnTong();
	PLAYER_REVIVAL_POS* GetDeathRevivalPos() 
	{
		return &m_sDeathRevivalPos;
	};
	BOOL			IsUseReviveIdWhenLogin() { return m_bUseReviveIdWhenLogin; };
	void			SetLoginType(BOOL bUseReviveId) { m_bUseReviveIdWhenLogin = bUseReviveId; };
	void			SetChestLock(BOOL unlockFlag);
	BOOL			CheckChestPW(int iPassword);
	void			SetChestPW(int newPW);
	void			SetChestPW2(int newPW);
	void			SetNewPW(int resetPW);
	BOOL			CreateTong(int nCamp, char *lpszTongName);
	void			SetNumImg(int nNumber);
	void			SendSellItemInfo(int nTargetPlayer, int nPrcess, BOOL bUpdate = FALSE);	
	int				GetItemIdxBymItemListIdx(int midx);
	void			SendSellItemCount(int nIndex);
	int				GetTradeCount();
	void			Change_PK_Status(int nId);
	void			SendMSGroup();
	void			SendMSRank(TMissionLadderSelfInfo* SelfData, TMissionLadderInfo* RankData);
#endif

private:
	void			CalcCurLucky();							// ¼ÆËãµ±Ç°ÔËÆø
	void			LevelAddBaseLifeMax();					// µÈ¼¶ÉıÒ»¼¶ºóÔö¼Ó×î´óÉúÃüµã
	void			LevelAddBaseManaMax();					// µÈ¼¶ÉıÒ»¼¶ºóÔö¼Ó×î´óÄÚÁ¦µã
	void			LevelAddBaseStaminaMax();				// µÈ¼¶ÉıÒ»¼¶ºóÔö¼Ó×î´óÌåÁ¦µã

	void			SendFactionData();						// Ïò¿Í»§¶Ë·¢ËÍÃÅÅÉÊı¾İ
	void			SyncCurrentBaseAttriibute(int type,int attribute,int curAttribute);	// ham moi
	void			AddBaseStrength(int nData);				// Ôö¼Ó»ù±¾Á¦Á¿
	void			AddBaseDexterity(int nData);			// Ôö¼Ó»ù±¾Ãô½İ
	void			AddBaseVitality(int nData);				// Ôö¼Ó»ù±¾»îÁ¦
	void			AddBaseEngergy(int nData);				// Ôö¼Ó»ù±¾¾«Á¦

	void			ResetBaseStrength(int nData);			// Ôö¼Ó»ù±¾Á¦Á¿
	void			ResetBaseDexterity(int nData);			// Ôö¼Ó»ù±¾Ãô½İ
	void			ResetBaseVitality(int nData);			// Ôö¼Ó»ù±¾»îÁ¦
	void			ResetBaseEngergy(int nData);			// Ôö¼Ó»ù±¾¾«Á¦

	void			SetNpcPhysicsDamage();					// ÓÉµ±Ç°Á¦Á¿¼ÆËã¶ÔÓ¦npcµÄÎïÀíÉËº¦(PhysicsDamage)
	void			SetNpcAttackRating();					// ÓÉµ±Ç°Ãô½İ¼ÆËã¶ÔÓ¦npcµÄ¹¥»÷ÃüÖĞÂÊ(AttackRating)
	void			SetNpcDefence();						// ÓÉµ±Ç°Ãô½İ¼ÆËã¶ÔÓ¦npcµÄ·ÀÓùÁ¦

#ifndef _SERVER
	void			ProcessMouse(int x, int y, int Key, MOUSE_BUTTON nButton);
	void			OnButtonUp(int x, int y, MOUSE_BUTTON nButton);				// ´¦ÀíÊó±ê¼üÌ§Æğ
	void			OnButtonDown(int x, int y, int Key, MOUSE_BUTTON nButton);		// ´¦ÀíÊó±ê¼ü°´ÏÂ
	void			OnButtonMove(int x, int y, int Key, MOUSE_BUTTON nButton);		// ´¦ÀíÊó±ê¼ü°´ÏÂºóÒÆ¶¯
	void			OnMouseMove(int x, int y);									// ´¦ÀíÊó±êÒÆ¶¯
	int				NetCommandPlayerTalk(BYTE* pProtocol);
	//friend			LuaInitStandAloneGame(Lua_State * L);
#endif

private:
	void			S2CExecuteScript(char * ScriptName, char * szParam);

#ifdef _SERVER

private:
	int				LoadPlayerBaseInfo(BYTE * pRoleBuffer, BYTE * &pRoleBaseBuffer, unsigned int &nParam );
	int				LoadPlayerItemList(BYTE * pRoleBuffer, BYTE * &pItemBuffer, unsigned int &nParam );
	int				LoadPlayerFightSkillList(BYTE * pRoleBuffer, BYTE * &pSkillBuffer, unsigned int &nParam);
	int				LoadPlayerStateSkillList(BYTE * pRoleBuffer, BYTE * &pSkillBuffer, unsigned int &nParam);
	int				LoadPlayerTaskList(BYTE * pRoleBuffer, BYTE * &pTaskBuffer, unsigned int &nParam);
	int				SavePlayerBaseInfo(BYTE * pRoleBuffer);
	int				SavePlayerItemList(BYTE * pRoleBuffer);
	int				SavePlayerFightSkillList(BYTE * pRoleBuffer);
	int				SavePlayerTaskList(BYTE * pRoleBuffer);
	int				SavePlayerStateSkillList(BYTE * pRoleBuffer);
#endif

public:
	void			SetNpcDamageAttrib();
	void			DoScriptAction(PLAYER_SCRIPTACTION_SYNC * pUIInfo); 
	void			ProcessPlayerSelectFromUI(BYTE* pProtocol);			
#ifndef _SERVER
	void			DialogNpc(int nIndex);
	void			OnSelectFromUI(PLAYER_SELECTUI_COMMAND * pSelectUI, UIInfo eUIInfo);			
	void			OnScriptAction(PLAYER_SCRIPTACTION_SYNC * );
#endif
#ifdef _SERVER
	void			RestoreLiveData();						
	void			SetTimer(DWORD nTime, int nTimeTaskId);
	void			CloseTimer();							
	int				AddDBPlayer(char * szPlayerName, int sex, DWORD * pdwID );
	int				LoadDBPlayerInfo(BYTE * pPlayerInfo,  int &nStep, unsigned int &nParam);
	BOOL			GetNewPlayerFromIni(KIniFile * pIniFile, BYTE * pRoleBuffer);
	int				UpdateDBPlayerInfo(BYTE * pPlayerInfo);
	int				DeletePlayer(char * szPlayerName = NULL);
	void			LaunchPlayer();
	void			LaunchPlayer2(bool value);
	BOOL			Pay(int nMoney);
	BOOL			Earn(int nMoney);
	void			DialogNpc(BYTE * pProtocol);
	int				AddTempTaskValue(void* pData);
#endif
#ifndef _SERVER
	void			SendInfoAuto();
	void			PlayerAuto();
	void			MoveTo(int nX, int nY);
	void			ClearArrayNpcLag()
	{
		for (int i=0; i < MAX_ARRAY_NPC_AUTO; i++)
		{
			m_ArrayNpcLag[i] = 0;
		}
	};
	void			ClearArrayObjectLag()
	{
		for (int i=0; i < MAX_ARRAY_NPC_AUTO; i++)
		{
			m_ArrayObjectLag[i] = 0;
		}
	};
	void			ClearArrayTimeNpcLag()
	{
		for (int i=0; i < MAX_ARRAY_NPC_AUTO; i++)
		{
			m_ArrayTimeNpcLag[i] = 0;
		}
	};
	void			ClearArrayTimeObjectLag()
	{
		for (int i=0; i < MAX_ARRAY_NPC_AUTO; i++)
		{
			m_ArrayTimeObjectLag[i] = 0;
		}
	};
	void			ClearArrayNpcNeast()
	{
		for (int i=0; i < MAX_ARRAY_NPC_AUTO; i++)
		{
			m_ArrayNpcNeast[i] = 0;
		}
	};
	void			ClearArrayObjectNeast()
	{
		for (int i=0; i < MAX_ARRAY_NPC_AUTO; i++)
		{
			m_ArrayObjectNeast[i] = 0;
		}
	};
	void			ClearArrayStateSkill()
	{
		for (int i=0; i < MAX_ARRAY_NPC_AUTO; i++)
		{
			m_ArrayStateSkill[i] = 0;
		}
	};
	
	void			PlayerFollowActack(int i);
	void			AutoCheckUseItem();
	void			PlayerEatAItem();
	void			AutoReturn();
	void			AutoAddNpc2Array(int nRelation);
	int				FindNearNpc2Array(int nRelation);
	void			FindObjectNearPlayer();
	BOOL			AutoBuffSkillState();
	int				FindNearObject2Array();
	void			PlayerFollowObject(int nObject);
	void			PlayerUseItem(int type);
	int				SearchItemPosByItemIdx(int sIdx);
	void			AutoParty();
	void			PlayerFilterEquip();
	BOOL			IsRAPEquip(BYTE btDetail);
	BOOL			IsEquipSatisfyCondition(int nIdx);
	void 			SetRepairItem(BYTE btFlag) { m_btDurabilityItem = btFlag; };
	BYTE			GetRepairItem() { return m_btDurabilityItem; };
	void 			SetAutoUseTLL(BYTE btFlag) { m_AutoUseTTL = btFlag; };
	BYTE			GetAutoUseTLL() { return m_AutoUseTTL; };
	BOOL			IsNotValidNpc(int nIndex);
	BOOL			IsNotValidObject(int nObject);	
	void			PlayerSwitchAura();
	BOOL			PlayerMoveMps();
	int				GetMoveMpsCount();
#endif

#ifdef _SERVER
	void			SetBaseStrength(int nData);
	void			SetBaseDexterity(int nData);
	void			SetBaseVitality(int nData);
	void			SetBaseEngergy(int nData);
#endif
};

#ifdef TOOLVERSION
extern CORE_API KPlayer	Player[MAX_PLAYER];
#else
extern KPlayer	Player[MAX_PLAYER];
#endif
#endif //KPlayerH

