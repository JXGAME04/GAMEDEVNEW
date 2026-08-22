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
#include "../KMeridian.h"

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
#define		BASE_FANGYU_ALL_MAX			    75        //?????

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
	UI_OPENTONGJX2,	// JX2 port: mo cua so bang hoi kieu JX2
	UI_TASKVALUE,	// [TaskGuide] (int id, int value); id = -1 -> xoa bang task (client cu bo qua)
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
	int				m_nSubWorldID;		// 重生点地图
	int				m_ReviveID;			// 重生点索引
	int				m_nMpsX;			// 重生点地图位置 x
	int				m_nMpsY;			// 重生点地图位置 y
} PLAYER_REVIVAL_POS;

typedef struct
{
	int				m_nSubWorldId;		// 传送门世界ID
	int				m_nTime;			// 传送门保持时间
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
	int		FindSameGerne(int nGerne);
};

#ifndef _SERVER

struct ExtAuto
{
	UINT uLTime1;
	UINT uLTime2;
	UINT uMTime1;
	UINT uMTime2;
	UINT uUnFightTime;
	UINT uChatTime;
	UINT uPoisonTime;
	UINT uExp2Time;
	UINT uSkillExp2Time;
	UINT uBaseBuffTime;
	UINT uOpenBagTime;
	UINT uCLBuffTime;
	int  nAuraTime;
	BOOL bChangeAura;
	BOOL bPrevFightState;
	BOOL bJustTP;
	BOOL bJustDis;
	UINT uNpcID;
	UINT uFDelayTime;
	UINT uHorseTime;
	int  nOldLife;
	UINT uChSkillTime;
	BOOL bChSkill;
	int  nCurObjID;
	int  nCurObjLoop;
	BOOL bLBObjDown;
	UINT uFtNextTime;
	UINT umObjIncId;
	UINT uARTimeItem;
	UINT uARTimeBox;
	UINT uTNextProc;
	UINT uTNextInvite;
	UINT uTNextJoin;
	UINT uTNextLeave;
	UINT uTNextRemove;
	UINT uTNextRepair;
	UINT uTNextReturn;
	int	 nHomeStep;
	int	 nSubStep;
	int  nCurShop;
	UINT uSyncTime;
	UINT uTFollMove1;
	UINT uTFollMove2;
	UINT uTOutMove;
	int  nCurMoveRet;
	int  nCoordStep;
	int  nTempX;
	int  nTempY;
	UINT uTJustMove;
	BOOL bReachDes;
	int  nCurEncircle;
	UINT uTEncircle;
	autoCoord sEncircle[9];
	// == Da Tau (18/08/2026) - trang thai may ATYPE_DATAU, POD (ATYPE_CLEAR memset) ==
	int  nDTEngaged;     // 0 tha may; 1 lam viec thanh (chan MOVE/RETURN); 2 farm map nv
	int  nDTPhase;       // pha lon DTP_*
	int  nDTStep;        // buoc con trong pha
	UINT uDTNext;        // moc thoi gian hanh dong ke tiep
	UINT uDTPath;        // moc thoi gian FindPath ke tiep
	UINT uDTDlgSeen;     // seq hoi thoai da xu ly
	UINT uDTTalkSeen;
	UINT uDTFinSeen;
	UINT uDTBoxSeen;
	UINT uDTMsgSeen;
	int  nDTQType;       // loai nhiem vu 1..6 (0 chua biet)
	int  nDTCand[8];     // dong bang ung vien (buy/find/show)
	int  nDTCandNum;
	int  nDTCandCur;
	int  nDTReqNum;      // T4 so cuon / T5 gia tri / T6 so manh
	int  nDTMapId;       // T4 map dich
	int  nDTBook;        // T4 1=dia do chi 2=mat chi
	int  nDTProg;        // T4 so tam hien co (-1 chua biet)
	int  nDTAnchorX;     // T4 toa do neo farm (mps)
	int  nDTAnchorY;
	int  nDTStatType;    // T5: 2 exp 3 danhvong 4 phucduyen 5 pk 6 tongkim
	double dDTExpBase;   // T5 exp baseline
	int  nDTHaveBase;
	int  nDTItemIdx;     // item dinh nop
	int  nDTRetry;       // dem thu lai
	int  nDTShopTry;     // option shop da thu
	int  nDTUsedPD;      // so Phuc Duyen Lo da dung
	int  nDTFuYuanPrev;  // fuyuan sau lan dung item truoc
	UINT uDTHoldUntil;   // treo den thoi diem nay
	int  nDTDoneDay;     // yymmdd ngay da du 40
	UINT uDTStatusTime;  // throttle bao trang thai
	int  nDTUnknown;     // dem hoi thoai LA khong nhan dang (khong reset khi mo lai)
	int  nDTXaFuTry;     // dem lan thu xa phu that bai (chan ket vong)
	int  nDTHoldFreeze;  // 1 = hold 'du 40/ngay': dung yen neu nguoi choi tat luyen cong
	UINT uDTFarmStall;   // moc thoi gian tien do farm gan nhat (T4/T5exp watchdog)
	UINT uDTRoamNext;    // [DaTau] nhip chay tim quai khi farm T4
	int  nDTRoamStep;    // [DaTau] cum quai dang nham (KDaTauSpots.h)
	int  nDTBackXaFu;    // [DaTau] 1 = phu ve thanh xong thi ra Xa Phu di lai map
	int  nDTRwTry;       // [DaTau] so nut ruong thuong da thu (cua so mo lai = nut sai)
	int  nDTLBTry;       // [DaTau 20/08] so lan da dung Lenh bai hoan thanh cho nhiem vu nay
	UINT uDTTienSeen;    // [DaTau r4] seq kenh tien do (szTien) da doc
	int nDTPhaseBack;    // [DaTau r5d] pha di cho dang do bi cua so thuong TRE cuop
	ExtAuto()
	{
		uLTime1 = 0;
		uLTime2 = 0;
		uMTime1 = 0;
		uMTime2 = 0;
		uUnFightTime = 0;
		uChatTime = 0;
		uPoisonTime = 0;
		uExp2Time = 0;
		uSkillExp2Time = 0;
		uBaseBuffTime = 0;
		uOpenBagTime = 0;
		uCLBuffTime = 0;
		nAuraTime = 0;
		bChangeAura = 0;
		bPrevFightState = 0;
		bJustTP = 0;
		bJustDis = 0;
		uNpcID = 0;
		uFDelayTime = 0;
		nOldLife = 0;
		uHorseTime = 0;
		uChSkillTime = 0;
		bChSkill = 0;
		nCurObjID = 0;
		nCurObjLoop = 0;
		bLBObjDown = 0;
		uFtNextTime = 0;
		umObjIncId = 0;
		uARTimeItem = 0;
		uARTimeBox = 0;
		uTNextProc = 0;
		uTNextInvite = 0;
		uTNextJoin = 0;
		uTNextLeave = 0;
		uTNextRemove = 0;
		uTNextRepair = 0;
		uTNextReturn = 0;
		nHomeStep = 0;
		nSubStep = 0;
		nCurShop = 0;
		uSyncTime = 0;
		uTFollMove1 = 0;
		uTFollMove2 = 0;
		uTOutMove = 0;
		nCurMoveRet = 0;
		nCoordStep = 0;
		nTempX = 0;
		nTempY = 0;
		bReachDes = FALSE;
		uTEncircle = 0;
		uTJustMove = 0;
		nDTEngaged = 0;
		nDTPhase = 0;
		nDTStep = 0;
		uDTNext = 0;
		uDTPath = 0;
		uDTDlgSeen = 0;
		uDTTalkSeen = 0;
		uDTFinSeen = 0;
		uDTBoxSeen = 0;
		uDTMsgSeen = 0;
		nDTQType = 0;
		nDTCandNum = 0;
		nDTCandCur = 0;
		nDTReqNum = 0;
		nDTMapId = 0;
		nDTBook = 0;
		nDTProg = -1;
		nDTAnchorX = 0;
		nDTAnchorY = 0;
		nDTStatType = 0;
		dDTExpBase = 0;
		nDTHaveBase = 0;
		nDTItemIdx = 0;
		nDTRetry = 0;
		nDTShopTry = 0;
		nDTUsedPD = 0;
		nDTFuYuanPrev = 0;
		uDTHoldUntil = 0;
		nDTDoneDay = 0;
		uDTStatusTime = 0;
		nDTUnknown = 0;
		nDTXaFuTry = 0;
		nDTHoldFreeze = 0;
		uDTFarmStall = 0;
		uDTRoamNext = 0;
		nDTRoamStep = 0;
		nDTBackXaFu = 0;
		nDTRwTry = 0;
	}
};

struct ExtAutoObjTime
{
	UINT nTotalTime;
	UINT nPickTime;
	UINT nID;
	short nChecked;
	short bItem;
};

struct ExtAutoTeamRecv
{
	UINT uTime;
	char szName[32];
};

#endif

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
	int				m_nChangeExtPoint;			// 变动的点数
#endif
	int				m_ImageId;			// bien id npc image
	int				m_nExtPoint;				// 活动点数
	int				m_nPhysicsSkillID;		//当前玩家的物理攻击技能
	int				m_nPeapleIdx;
	int				m_nObjectIdx;
	int				m_nPickObjectIdx;
	int				m_nPlayerIndex;				// 本实例在 Player 数组中的位置
	KCacheNode *	m_pLastScriptCacheNode;

public:

	int m_nMeridianStrength;
	int m_nMeridianDexterity;
	int m_nMeridianVitality;
	int m_nMeridianEngergy;
	int 			m_nActiveEquipNum;			//bo trang bi dang kich hoat
#ifdef _SERVER
	PLAYER_EXCHANGE_POS		m_sExchangePos;
	KTimerTaskFun	m_TimerTask;
	BOOL			m_bIsQuiting;
	BOOL			m_bForeQuit;
	BYTE			m_nLixian;											//#uy thac
	UINT			m_uMustSave;
	DWORD			m_ulLastSaveTime;
	DWORD			m_dwLoginTime;
//	DWORD			m_uLastPingTime;
	void*			m_pStatusLoadPlayerInfo;	//加载玩家信息时用
	BYTE*			m_pCurStatusOffset;			//二进制时，记录读到指针位置了
	BOOL			m_bFinishLoading;			//完成加载
	BYTE			m_SaveBuffer[64 * 1024 * 5];	//保存缓冲 
	int				m_nLastNetOperationTime;	//最后一次网络操作时间
	BOOL			m_bSleepMode;
	KList			m_PlayerWayPointList;		//
	KList			m_PlayerStationList;
	int				m_nViewEquipTime;			// 最后一次察看他人装备的时间
	int				m_nPrePayMoney;
	int				m_nTimeRide;//edit by phong kieu len xuong ngua
	bool		m_nLicReg;
	char			m_szTaskExcuteFun[32];
	enum
	{
		FF_CHAT = 0x01,
	};
	int				m_nForbiddenFlag;			// 禁止标志
	BOOL 			m_bEnableSave;
	int				m_nArrangeTimeItem;
	int				m_nArrangeTimeBox;
	int				m_nCmpArrayItem[EQUIPMENT_ROOM_WIDTH*EQUIPMENT_ROOM_HEIGHT];
	int				m_nCmpArrayBox[EQUIPMENT_ROOM_WIDTH*EQUIPMENT_ROOM_HEIGHT];
	int				m_nCmpArrayExBox[EQUIPMENT_ROOM_WIDTH*EQUIPMENT_ROOM_HEIGHT];
#endif

#ifndef _SERVER 
	KEquipmentArray	m_sListEquipment;
    int                m_nImageNpcID; //id npc 
#endif 

#ifndef _SERVER
	int				m_RunStatus;				// 是跑还是走
	DWORD			m_dwNextLevelLeadExp;		// 统率力下一级经验值
	int				m_nSendMoveFrames;			// 用于控制客户端向服务器发送移动(走或跑)协议的频率，使之不能发送大量的移动协议，减小带宽压力
	int				m_nLastNpcIndex;
	BOOL			m_bHideNpc;
	BOOL			m_bHidePlayer;
	BOOL			m_bHideSkill;
	TMissionLadderSelfInfo m_MissionData;		//add by phong ki襲 using t鑞g kim
	TMissionLadderInfo m_MissionRank[MISSION_STATNUM];
#endif

	KIndexNode		m_Node;
	GUID			m_Guid;
	BOOL			m_bExchangeServer;
	int				m_DebugMode;
	DWORD			m_dwID;						// 玩家的32位ID
	int				m_nIndex;					// 玩家的Npc编号
	int				m_nNetConnectIdx;			// 第几个网络连接
	char			m_nPlayerHWID[64];			// Player HWID 64 len edit by phong kieu
	KItemList		m_ItemList;					// 玩家的装备列表
	BuySellInfo		m_BuyInfo;					// 进行的交易列表
	KPlayerMenuState	m_cMenuState;			// 是否处于交易或队伍开放状态
	KTrade			m_cTrade;					// 交易模块
	int				m_nAttributePoint;			// 未分配属性点
	int				m_nSkillPoint;				// 未分配技能点
	int				m_nStrength;				// 玩家的基本力量（决定基本伤害）
	int				m_nDexterity;				// 玩家的基本敏捷（决定命中、体力）
	int				m_nVitality;				// 玩家的基本活力（决定生命、体力）
	int				m_nEngergy;					// 玩家的基本精力（决定内力）
	int				m_nLucky;					// 玩家的基本运气（影响得到装备的好坏）
	BYTE			m_btChatSpecialChannel;		
	int				m_nUpExp;
	int				m_nCurStrength;				// Suc manh
	int				m_nCurDexterity;			// Than phap
	int				m_nCurVitality;				// Sinh khi
	int				m_nCurEngergy;				// Noi cong
	int				m_nCurLucky;				// May man
	double				m_nExp;						// 当前经验值(当前等级在npc身上)
	double				m_nNextLevelExp;			// 下一级经验值
	int				m_CUnlocked;				// trang thai khoa ruong player
	char			szStringInput[64];
	int				m_nStringNum;
	DWORD			m_dwLeadExp;				// 统率力经验值
	DWORD			m_dwLeadLevel;				// 统率力等级
	char			m_PlayerName[32];
	char			m_AccoutName[32];			//edit by phong kieu khai bao bien m_AccoutName
	BYTE		m_ImagePlayer;
	char			m_PlayerMateName[32];//#MateName
	KPlayerTeam		m_cTeam;					// 玩家的组队信息
	KPlayerFaction	m_cFaction;					// 玩家的门派信息
	KPlayerChat		m_cChat;
	KPlayerTask		m_cTask;					// 玩家任务系统(变量)
	KPlayerPK		m_cPK;						// PK关系处理
	KPlayerRepute	m_cRepute;						// danh vong
	KPlayerFuYuan	m_cFuYuan;						// phuc duyen
	KPlayerReBorn	m_cReBorn;						// trung sinh
	KPlayerTong		m_cTong;					
	KMeridian		m_cMeridian;					//Kinh mach
#ifndef _SERVER
	KPlayerAuto		m_cAuto;			//fkauto
	KAutoMove		m_cAutoMove;
#endif
	DWORD			m_dwDeathScriptId;			
	DWORD			m_dwDamageScriptId;			
	DWORD			m_dwGiveBoxId;
	DWORD			m_dwTimeBoxId;
	DWORD			m_dwStrBoxId;
#ifdef _SERVER
	// [WLLS 20/08] lien dau leaguematch (THICONG_LIENDAU_PORT.md)
	BYTE			m_bWllsAskStrArg;		// AskClientForString: cb nhan chuoi nhap lam doi so 1
	BYTE			m_bWllsDisableStall;	// DisabledStall(1): cam bay ban
	BYTE			m_bWllsForbidTrade;		// ForbitTrade(1): cam giao dich
	BYTE			m_bWllsForbidStamina;	// ForbitStamina(1): dong bang the luc
	BYTE			m_bWllsDmgCounterOn;	// ST_StartDamageCounter dang dem
	int				m_nWllsDmgCounter;		// tong mat mau ganh chiu (truoc hap thu noi luc)
	char			m_szWllsStrTask[8][64];	// GetStringTask/SetStringTask (phien lam viec, KHONG luu DB)
	int				m_nWllsLastDiagNpc;		// idx NPC vua bam thoai (GetLastDiagNpc doc - engine khong bom global NpcIndex)
	// [TONG 21/08] port 3 Hoat dong Phuong bang hoi (missions\tong) - xem ReverseTools\dac_ta_17_ham_hoatdong_phuong.json
	BYTE			m_bTongForbidSkill;		// ForbitSkill(1): cam moi chieu - chan tai KSkill::CanCastSkill
	BYTE			m_bTongForbidAura;		// ForbitAura(1): cam bat vong sang - chan tai KProtocolProcess::ChangeAuraSkill
	BYTE			m_bTongForbidEnmity;	// ForbidEnmity(1): cam GUI yeu cau cuu sat - c2sPKApplyEnmity tu choi im lang
	int				m_nTongForbidSkillId[8];	// SetAForbitSkill(id,1): cam rieng tung chieu (0 = o trong)
	struct { int nSkillId; int nLevel; } m_TongTempMagic[8];	// AddTempMagic: ban ghi de ap lai sau UpdataCurData
	void			TongTempMagicRecord(int nSkillId, int nLevel);	// cong don; ve 0 thi xoa ban ghi
	void			TongTempMagicReapply();							// goi trong UpdataCurData sau khi xoa TempSkill
	int				TongIsForbidSkill(int nSkillId);				// 1 = bi cam (toan bo hoac rieng)
#endif
	DWORD			m_dwNumberBoxId;
	DWORD			m_dwLogoutScriptID;
	DWORD			m_dwRewardId;
	DWORD			m_dwRewardExId;
	DWORD			m_dwTrembleItemId;//kh秏 n筸
	
	char			m_szTaskAnswerFun[MAX_ANSWERNUM][32];
	int				m_nAvailableAnswerNum;
	bool			m_bWaitingPlayerFeedBack;	
//	DWORD			m_dwOutOfDateFeedBackTime;	
	BYTE			m_btTryExecuteScriptTimes;	
	//char			m_CurScriptName[128];
	int				m_nWorldStat;						//x誴 h筺g th?gi韎
	int				m_nSectStat;
	int				m_nTimeChangePK;
// auto playgame
	BOOL			m_bPriorityUseMouse;
	BOOL			m_bSpeedControl;
#ifndef _SERVER
	ExtAuto			m_sExtAuto;
	std::map<UINT, UINT>	m_mAutoExcludeNpcID;
	std::map<int, ExtAutoObjTime>		m_mAutoIDObj;
	std::map<UINT, UINT>	m_mAutoIDTeam;
	std::map<int, ExtAutoTeamRecv>	m_mAutoTeamRecv;
	std::vector<UINT>	m_vAutoTeamKick;
#else
	int				m_nAutoRevTime;
#endif
private:	
	BOOL			m_bActiveAuto;
#ifndef _SEVER
	BOOL			m_bSortEquipment;
	int				m_nReturnPortalStep;
	int				m_nCurReturnPortalSec;
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
	BOOL			m_bAutoAccecptAll;//t?ng m阨 t蕋 c?
	BOOL			m_bAutoMove;
	BOOL			m_bPickItem;
	BYTE			m_btPickUpKind;
	BYTE			m_btDurabilityItem;
	BYTE			m_btDurabilityValue;
	BYTE			m_AutoUseTTL;
	int				m_AuraSkill[2];
	BOOL			m_bActiveSwitchAura;
	int 			m_AutoLifeReplenishP; //nga my buff theo ph莕 tr╩ m竨
	BOOL 			m_AutoLifeReplenish;
	BOOL			m_AutoBuffTeam; //nga my buff m竨 cho t?i
	int				m_MoveMps[defMAX_AUTO_MOVEMPSL][3];
	int				m_MoveStep;
	char				m_AutoPT_PlayerList[defMAX_AUTO_MOVEMPSL][32];//qu秐 l?t?i
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
	BOOL			m_bSaveJewelry;//gi?trang s鴆
	//BOOL			m_Auto_PickInFightState;//nh苩 trong th祅h
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
	void			SetPlayerIndex(int nNo);					// 设定 m_nPlayerIndex
	void			GetAboutPos(KMapPos *pMapPos);			// 获得玩家附近一个空位置
	int				GetPlayerIndex();							// 获得本实例在 Player 数组中的位置
	DWORD			GetPlayerID(){return m_dwID;};
	BYTE			GetFirstAddFaction()
	{
		return (BYTE)this->m_cFaction.m_nFirstAddFaction;
	}
	void			GetFactionName(char *lpszName, int nSize);	// 获得当前门派名称 not end
	void			GetFactionValueName(char *lpszName, int nSize);	// 获得当前门派名称 not end
	int				GetFactionNo();
	void			ChatFriendOnLine(DWORD dwID, int nFriendIdx);// 获得通知某好友上线了
	BOOL			ExecuteScript(char * ScriptFileName, char * szFunName, int nParam = 0, bool bGlobal = true);
#ifdef _SERVER
	BOOL			ExecuteItemScriptJX2(char * ScriptFileName, int nItemIdx, int* pnRet);	// [JX2 ITEM 21/08] main(idx) -> gia tri tra
#endif
	BOOL			ExecuteScript(char * ScriptFileName, char * szFunName, char * szParams, bool bGlobal = true);
	BOOL			ExecuteScript(DWORD dwScriptId, char * szFunName, char *  szParams, bool bGlobal = true);
	BOOL			ExecuteScript(DWORD dwScriptId,  char * szFunName, int nParam, bool bGlobal = true);

	BOOL			ExecuteScript2(char * ScriptFileName, char * szFunName, int nParam1 = 0, int nParam2 = 0, bool bGlobal = true);
	BOOL			ExecuteScript2(char * ScriptFileName, char * szFunName, char * szParams1, char * szParams2, bool bGlobal = true);
	BOOL			ExecuteScript2(DWORD dwScriptId, char * szFunName, char *  szParams1, char *  szParams2, bool bGlobal=true);
	BOOL			ExecuteScript2(DWORD dwScriptId,  char * szFunName, int nParam1, int nParam2, bool bGlobal = true);

	BOOL			ExecuteScript3(char * ScriptFileName, char * szFunName, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0);
	BOOL			ExecuteScript3(char * ScriptFileName, char * szFunName, char * szParams1, char * szParams2, char * szParams3);
	BOOL			ExecuteScript3(DWORD dwScriptId, char * szFunName, char *  szParams1, char *  szParams2, char * szParams3);
	BOOL			ExecuteScript3(DWORD dwScriptId,  char * szFunName, int nParam1, int nParam2, int nParam3);

	BOOL			DoScript(char * ScriptCommand);				//执行某个脚本指令
	void			ChangeCurStrength(int nData);			// 改变当前力量(当 nData 小于 0 时，减少)
	void			ChangeCurDexterity(int nData);			// 改变当前敏捷(当 nData 小于 0 时，减少)
	void			ChangeCurVitality(int nData);			// 改变当前活力(当 nData 小于 0 时，减少)
	void			ChangeCurEngergy(int nData);			// 改变当前精力(当 nData 小于 0 时，减少)
	BOOL			ExecuteScript(char * ScriptFileName);
	void			Release();
	void			Active();								
	void			ProcessMsg(KWorldMsgNode *lpMsg);		
	LPSTR			GetPlayerName() { return m_PlayerName; };
	LPSTR			GetPlayerAccount() { return m_AccoutName; };//edit by phong kieu them ham getplayeraccount
	BOOL			NewPlayerGetBaseAttribute(int Series);	// 新玩家登陆时根据五行属性产生 力量 敏捷 活力 精力 四项数值
	void			AddBaseLucky(int nData);				// 增加基本运气
	void			AddExp(int nExp, int nTarLevel, BOOL bCheck = FALSE);		// 增加经验(原始数据，还未经过处理)
	void			AddSelfExp(int nExp, int nTarLevel);
	void			DirectAddExp(double nExp);					// 直接增加经验值，不考虑其他因素
	void			LevelUp();								// 升一级
	void			AddLeadExp(int nExp);					// 增加统率力经验
	void			UpdataCurData();
	void			ReCalcTongBenefit();					//Recalculate Tong Benefit for Members
	void			ReCalcEquip();							// 重新计算身上的装备
	void			ReCalcState();							// 重新计算身上的状态
	void			ReCalcMeridian();
	void			ChangePlayerCamp(int nCamp);			// 改变玩家阵营
	void			Revive(int nType);						// 重生	
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
	void 			ProcessDouble(); //#time x2 Exp ch誸 kh玭g m蕋
#ifndef _SERVER
	void			SortEquipment();
	void			SetSortEquipment(BOOL bFlag);
	void			AutoFllowPeople(int h, int k);
	BOOL			CheckEquip(BYTE btDetail);
	BOOL			CheckEquipMagic(int nIdx);
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
	void			GetEchoDamage(int* nMin, int* nMax, int nType);// 获取界面需要显示的伤害值
	void			ProcessInputMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);// 处理键盘鼠标消息
	void			RecvSyncData();								// 接收同步数据
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
	void			ApplyTeamInfo(DWORD dwNpcID);				// 向服务器申请查询某个npc所在队伍的信息
	void			ApplySelfTeamInfo();						// 向服务器申请查询玩家自身的队伍情况
	BOOL			ApplyCreateTeam();
	BOOL			ApplyTeamOpenClose(BOOL bFlag);				// 队长向服务器申请开放、关闭队伍是否允许加入成员状态
	void			ApplyAddTeam(int nNpcIndex);				// 玩家向服务器申请加入某个队伍
	void			AcceptTeamMember(DWORD dwNpcID);			// 玩家通知服务器接受某个npc为队伍成员
	void			TeamDropApplyOne(DWORD dwNpcID);			// 队长删除加入队伍申请列表中的某个npc
	void			LeaveTeam();								// 通知服务器本玩家离开队伍
	void			TeamKickMember(DWORD dwNpcID);				// 队长通知服务器踢除某个队员
	void			ApplyTeamChangeCaptain(DWORD dwNpcID);		// 队长向服务器申请把自己的队长身份交给别的队员
	void			ApplyTeamDismiss();							// 
	void			ApplySetPK(BOOL bPK);						// pk
	void			ApplyFactionData();							// 
	void			SendChat(KUiMsgParam *pMsg, char *lpszSentence);// 
	void			ApplyAddBaseAttribute(int nAttribute, int nNo);// 队长向服务器申请增加四项属性中某一项的点数(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
	BOOL			ApplyAddSkillLevel(int nSkillID, int nAddPoint);// 
	BOOL			ApplyUseItem(int nItemID, ItemPos SrcPos, int nTargetPos=0);	// 
	BOOL			ApplyAutoMoveItem(int nItemID, ItemPos SrcPos); // chuy觧 item t?h祅h trang v祇 rng
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
	void			GambleApplyOpen(char* lpszSentence, int nLength);
	void			GambleApplyClose();
	void			GambleApplyStart(int nNpcIdx);
	BOOL			GambleMoveMoney(int nMoney);
	void			GambleDecision(int nDecision, BYTE btChoose = -1);				// if nDecision == 0   if nDecision == 1   if nDecision == 2 
	void			GambleApplyLock(int nLockOrNot);
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
	void			s2cGambleChangeState(BYTE* pMsg);
	void			s2cGambleMoneySync(BYTE* pMsg);
	void			s2cGambleDecision(BYTE* pMsg);
	void			SyncCurPlayer(BYTE* pMsg);
	void			s2cLevelUp(BYTE* pMsg);
	void			s2cGetCurAttribute(BYTE* pMsg);
	void			s2cSetExp(double nExp);
	void			s2cSyncMoney(BYTE* pMsg);
	void			s2cTradeApplyStart(BYTE* pMsg);
	void			s2cGambleApplyStart(BYTE* pMsg);
	void			CheckObject(int nIdx);
	BOOL			AutoUseItem(int nGenre, int nDetailType, int nParticular);
	BOOL			AutoCheckItem(int nGenre, int nDetailType, int nParticular);
	void			SetExtPoint(int nChangePoint)
	{
		m_nExtPoint = nChangePoint;
	}
	int				FindTargetNpc(int nVision, BOOL bFightBack, int nFBVision, int nSelBoss,
					BOOL bTGNpc = TRUE, const short* pSerOrder = NULL, BOOL bFoll = FALSE,
					int nPointX = 0, int nPointY = 0);
#endif

	int				GetExtPoint();
	void			SwitchEquipSet(int setnum);

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
	void			c2sSetMeridian(SetMeridianData Data);
	void			GetLoginRevivalPos(int *lpnSubWorld, int *lpnMpsX, int *lpnMpsY);		// 获取玩家登入重生点位置
	void			GetDeathRevivalPos(int *lpnSubWorld, int *lpnMpsX, int *lpnMpsY);		// 获取玩家死亡重生点位置
	void			SetRevivalPos(int nSubWorld, int nRevalId);
	// DOT E (E4): doc diem hoi sinh ben vung cho GetPlayerRev (member private,
	// than inline trong class duoc phep tham chieu member khai bao sau)
	int				GetLoginRevivalWorld() { return m_sLoginRevivalPos.m_nSubWorldID; }
	int				GetLoginRevivalID() { return m_sLoginRevivalPos.m_ReviveID; }								// 设定玩家重生点ID
	BOOL			Save();									// 保存玩家数据
	BOOL			CanSave();
	void			ProcessUser();
	BOOL			SendSyncData(int &nStep, unsigned int &nParam);	// 发送同步数据
	BOOL			SendSyncData_Skill();					// 发送同步数据 - 技能
	void			SendCurNormalSyncData();				// 发送平时给自己的同步数据
	void			SetChatForbiddenFlag(int nFlag)
	{
		m_nForbiddenFlag = nFlag;
	};

	BOOL			AddFaction(char *lpszFactionName);		// 加入门派
	BOOL			AddFaction(int nFactionID);				// 加入门派
	BOOL			LeaveCurFaction();						// 离开门派
	BOOL			CurFactionOpenSkill(int nLevel);		// 开放当前门派某个等级的技能

	void			TaskClearTempVal();						// 清空任务临时变量
	DWORD				TaskGetSaveVal(int nNo);				// 得到任务完成情况
	void			TaskSetSaveVal(int nNo, DWORD bFlag);	// 设定任务完成情况
	DWORD				TaskGetClearVal(int nNo);				// 得到任务临时过程控制变量值
	void			TaskSetClearVal(int nNo, DWORD nVal);		// 设定任务临时过程控制变量值
	//void			SetTimeTaskTime(DWORD time){m_dwTimeTaskTime = time;};
	void			BuyItem(BYTE* pProtocol);
	void			SellItem(BYTE* pProtocol);
	void			AutoSellItem(BYTE* pProtocol);
	void			QuitGame(int nQuitType);				// 退出游戏
	void			S2CSendTeamInfo(BYTE* pProtocol);		// 收到客户端查询某个npc组队信息的申请后，向这个客户端发送队伍信息
	void			SendSelfTeamInfo();						// 服务器向客户端发送队伍信息
	BOOL			CreateTeam(BYTE* pProtocol);			
	BOOL			SetTeamState(BYTE* pProtocol);			// 收到客户端请求开放、关闭本player队伍是否允许队员加入状态
	BOOL			S2CSendAddTeamInfo(BYTE* pProtocol);	// 收到客户端请求加入一支队伍
	BOOL			AddTeamMember(BYTE* pProtocol);			// 队长通知服务器接受某个npc为队伍成员
	void			LeaveTeam(BYTE* pProtocol);				// 收到客户端队员通知离开队伍
	void			TeamKickOne(BYTE* pProtocol);			// 收到客户端队长通知踢出某个队员
	void			TeamChangeCaptain(BYTE* pProtocol);		// 收到客户端队长通知把队长身份交给某个队员
	void			TeamDismiss(BYTE* pProtocol);			// 收到客户端队长请求解散队伍
	void			SetPK(BYTE* pProtocol);					// 收到客户端请求设定PK状态
	void			SendFactionData(BYTE* pProtocol);		// 收到客户端请求获得门派数据
	void			ServerSendChat(BYTE* pProtocol);		// 收到客户端发来的聊天语句
	void			AddBaseAttribute(BYTE* pProtocol);		// 收到客户端要求增加基本属性点(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
	void			ResetBaseAttribute(BYTE* pProtocol);	// ham moi
	void			AddSkillPoint(BYTE* pProtocol);			// 收到客户端要求增加某个技能的点数
	void			IncSkillLevel(int nSkillId, int nAddLevel);
	void			IncSkillExp(int nSkillId, int nAddExp);
	BOOL			ServerPickUpItem(BYTE* pProtocol);		// 收到客户端消息鼠标点击某个obj拣起装备或金钱
	void			EatItem(BYTE* pProtocol);				// 收到客户端消息吃药
	void			ServerMoveItem(BYTE* pProtocol);		// 收到客户端消息移动物品
	void			ServerThrowAwayItem(BYTE* pProtocol);
	void ServerThrowAllItem();
	// 
	void			ServerThrowAllItem(BYTE* pProtocol);
	void			ChatSetTakeChannel(BYTE* pProtocol);	// 收到客户端消息设定聊天订阅频道
	void			ChatTransmitApplyAddFriend(BYTE* pProtocol);// 收到客户端请求转发聊天添加好友信息
	BOOL			ChatAddFriend(BYTE* pProtocol);			// 收到客户端消息添加聊天好友
	void			ChatRefuseFriend(BYTE* pProtocol);		// 收到客户端消息拒绝添加聊天好友
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
	void			GambleApplyOpen(BYTE* pProtocol);		// 
	void			GambleApplyClose(BYTE* pProtocol);		// 
	void			GambleApplyStart(BYTE* pProtocol);		// 
	void			GambleMoveMoney(BYTE* pProtocol);
	void			GambleDecision(BYTE* pProtocol);			// 
	void			c2sGambleReplyStart(BYTE* pProtocol);
	void			SyncGambleState();
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
	void SetEnablePlayerSave(bool enable)
	{
		m_bEnableSave = enable;
	}
	bool IsPlayerSaveEnabled() const
	{
		return m_bEnableSave;
	}
	int			AutoArrangeItem(int nType = 0);
#endif

private:
	void			CalcCurLucky();							// 计算当前运气
	void			LevelAddBaseLifeMax();					// 等级升一级后增加最大生命点
	void			LevelAddBaseManaMax();					// 等级升一级后增加最大内力点
	void			LevelAddBaseStaminaMax();				// 等级升一级后增加最大体力点

	void			SendFactionData();						// 向客户端发送门派数据
	void			SyncCurrentBaseAttriibute(int type,int attribute,int curAttribute);	// ham moi
	void			AddBaseStrength(int nData);				// 增加基本力量
	void			AddBaseDexterity(int nData);			// 增加基本敏捷
	void			AddBaseVitality(int nData);				// 增加基本活力
	void			AddBaseEngergy(int nData);				// 增加基本精力

	void			ResetBaseStrength(int nData);			// 增加基本力量
	void			ResetBaseDexterity(int nData);			// 增加基本敏捷
	void			ResetBaseVitality(int nData);			// 增加基本活力
	void			ResetBaseEngergy(int nData);			// 增加基本精力

	void			SetNpcPhysicsDamage();					// 由当前力量计算对应npc的物理伤害(PhysicsDamage)
	void			SetNpcAttackRating();					// 由当前敏捷计算对应npc的攻击命中率(AttackRating)
	void			SetNpcDefence();						// 由当前敏捷计算对应npc的防御力

#ifndef _SERVER
	void			ProcessMouse(int x, int y, int Key, MOUSE_BUTTON nButton);
	void			OnButtonUp(int x, int y, MOUSE_BUTTON nButton);				// 处理鼠标键抬起
	void			OnButtonDown(int x, int y, int Key, MOUSE_BUTTON nButton);		// 处理鼠标键按下
	void			OnButtonMove(int x, int y, int Key, MOUSE_BUTTON nButton);		// 处理鼠标键按下后移动
	void			OnMouseMove(int x, int y);									// 处理鼠标移动
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
	void			SyncTaskValueToClient(int nTaskId, int nValue);	// [TaskGuide] server->client (client build: rong)
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
	int			LaunchPlayer();
	void			LaunchPlayer2(bool value);
	BOOL			Pay(int nMoney, bool Gamble = false);
	BOOL			Earn(int nMoney, bool Gamble = false);
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

