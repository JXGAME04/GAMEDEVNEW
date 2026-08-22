#include "KCore.h"
#ifndef WM_MOUSEHOVER
#define WM_MOUSEHOVER 0x02A1
#endif
#include "KEngine.h"
#include  "../../Engine/Src/KSG_StringProcess.h"
#ifdef _SERVER
//#include "KNetServer.h"
//#include "../MultiServer/Heaven/Interface/iServer.h"
#else
//#include "KNetClient.h"
#include "../../Headers/IClient.h"
#include "Scene/KScenePlaceC.h"
#include "KIme.h"
#include "KMath.h"
#endif
#include "KNpc.h"
#include "KObj.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KPlayer.h"
#include "Math.h"
#include "ImgRef.h"
#include "LuaFuns.h"
#include "KSortScript.h"
#include "KScriptValueSet.h"
#include "KObjSet.h"
#include "KSkills.h"
#include "KPlayerSet.h"
#include "KSubWorldSet.h"
#include "KFile.h"
#include "GameDataDef.h"
#include "KBuySell.h"
//#include "MyAssert.h"
#include "MsgGenreDef.h"
#include "KItemSet.h"
#include "KTaskFuns.h"
#include "Text.h"
#include "KDaTauCap.h"

KDaTauCapture g_sDTCap; // bo dem chup hoi thoai cho auto Da Tau (khai bao KDaTauCap.h)

#ifdef _SERVER
#ifndef _STANDALONE
#include "../../../lib/S3DBInterface.h"
#else
#include "S3DBInterface.h"
#endif
#else
#include "CoreShell.h"
#endif

extern int LuaGetNpcTalk(Lua_State *L);

#define		defPLAYER_LOGIN_TIMEOUT			10 * 20// 10 sec
#define		defPLAYER_SAVE_TIMEOUT			30 * 20

#define		PLAYER_LEVEL_1_EXP				48
#define		PLAYER_LEVEL_ADD_ATTRIBUTE		5
#define		PLAYER_LEVEL_ADD_SKILL			1
#define		PLAYER_SHARE_EXP_DISTANCE		768
#define		PLAYER_TEAM_EXP_ADD				50
#define		AUTO_TIME_SORT_ITEM						50
#define		MAX_APPLY_TEAM_TIME				500

#define		BASE_WALK_SPEED					5
#define		BASE_RUN_SPEED					10

#define		BASE_FIRE_RESIST_MAX			75
#define		BASE_COLD_RESIST_MAX			75
#define		BASE_POISON_RESIST_MAX			75
#define		BASE_LIGHT_RESIST_MAX			75
#define		BASE_PHYSICS_RESIST_MAX			75

#define		BASE_ATTACK_SPEED				0					//toc do danh co ban edit by phong kieu mac dinh 20
#define		BASE_CAST_SPEED					0					//toc do danh co ban edit by phong kieu mac dinh 20
#define		BASE_VISION_RADIUS				120
#define		BASE_HIT_RECOVER				6

#define		TOWN_PORTAL_TIME				1800

enum AUTO_USE_ITEM
{
	ntypemau = 0,
	ntypemana,
	ntypethodiaphu,
	ntypegiaidoc,
	ntypettl,
	ntypetuiduocpham,
};
static char* szTuiDPhamName	= "Tói d­îc phÈm";
static const char szMagicScriptName[][32] = 
{
	"Tiªn Th¶o Lé",
	"Tiªn Th¶o Lé ®Æc biÖt",
	"NÕn Vui vÎ",
	"NÕn B×nh an",
	"NÕn May m¾n",
	"NÕn H¹nh phóc",
	"Song ThÊt Thñy",
	"§µo v¹n thä nh©n t¸o",
	"Lång ®Ìn b­¬m b­ím",
	"Lång ®Ìn ng«i sao",
	"Lång ®Ìn èng",
	"Lång ®Ìn trßn",
	"Lång ®Ìn c¸ chÐp",
	"Lång ®Ìn kÐo qu©n",
};


KPlayer	Player[MAX_PLAYER];
int		g_nLastNetMsgLoop;

KPlayer::KPlayer()
{
	Release();
}

KPlayer::~KPlayer()
{
	Release();
}

void	KPlayer::Release()
{
	m_nMeridianStrength = 0;
	m_nMeridianDexterity = 0;
	m_nMeridianVitality = 0;
	m_nMeridianEngergy = 0;
#ifndef _SERVER
	m_RunStatus = 0;
	m_dwNextLevelLeadExp = 0;
	m_nLeftSkillID = 0;
	m_nLeftSkillLevel = 0;
	m_nRightSkillID = 0;
	m_nRightSkillLevel = 0;
	m_nSendMoveFrames = defMAX_PLAYER_SEND_MOVE_FRAME;
	m_MouseDown[0] = FALSE;
	m_MouseDown[1] = FALSE;
	m_nImageNpcID = 0;
	m_DebugMode = 0;			//Set che do debug
	m_MissionData.Clear();
	memset(m_MissionRank, 0, sizeof(m_MissionRank));
	m_cAuto.Release(); //fkauto
	m_cAutoMove.Reset();
    //Player[CLIENT_PLAYER_INDEX].m_cAuto.FkAutoMapSet_StepOne(); //fkauto
#endif
	m_dwID = 0;
	m_nIndex = 0;
	m_nNetConnectIdx = -1;
	//Update Logout tai day
	//strcpy(m_nPlayerHWID,"00000000-00000000-00000000-00000000");
	m_cMenuState.Release();
	m_cTrade.Release();
	m_nAttributePoint = 0;
	m_nSkillPoint = 0;
	m_nStrength = 0;
	m_nDexterity = 0;
	m_nVitality = 0;
	m_nEngergy = 0;
	m_nLucky = 0;
	m_nCurStrength = 0;
	m_nCurDexterity = 0;
	m_nCurVitality = 0;
	m_nCurEngergy = 0;
	m_nCurLucky = 0;
	m_btChatSpecialChannel = 0;
	m_nUpExp = 0;
	m_nExp = 0;
	m_nNextLevelExp = PLAYER_LEVEL_1_EXP;
	m_bExchangeServer = FALSE;
	m_CUnlocked = 0;						//trang thai khoa ruong player
	m_dwLeadExp = 0;
	m_dwLeadLevel = 1;
	
	m_nPeapleIdx = 0;
	m_nObjectIdx = 0;
	m_bWaitingPlayerFeedBack = false;
	m_btTryExecuteScriptTimes = 0;	//
	
	// auto playgame
	m_bActiveAuto	= FALSE;
	m_bSpeedControl = FALSE;
#ifndef _SERVER
	m_bPriorityUseMouse = FALSE;
	m_nTimePriorityUseMouse = 0;
	m_nTimeCheckFollowPeopleIdx = 0;
	m_LifeAuto = 0;
	m_ManaAuto = 0;
	m_TownAuto = 0;
	m_RadiusAuto = 0;
	m_DistanceAuto = 0;
	m_bFightDistance = FALSE;
	m_nTimeBuffSkillAuto = 60000;
	m_nPickAreaItem = 0;
	m_PosXAuto = 0;
	m_PosYAuto = 0;
	m_bActacker = FALSE;
	m_Actacker = 0;
	//m_TimeUseItem = 0;
	ClearArrayNpcNeast();
	ClearArrayObjectNeast();
	ClearArrayNpcLag();
	ClearArrayObjectLag();
	ClearArrayTimeNpcLag();
	ClearArrayTimeObjectLag();
	ClearArrayStateSkill();	
	m_nCheckTimeBuffSkillAuto = 0;
	m_Count_Acttack_Lag = 0;
	m_nLifeLag = 0;
	m_nTimeRunLag = 0;
	m_bObject = FALSE;
	m_nObject = 0;
	m_bFilterEquipment = FALSE;
	m_bBuffSkill[0] = FALSE;
	m_bBuffSkill[1] = FALSE;
	m_bBuffSkill[2] = FALSE;
	m_bAttackAround = FALSE;
	m_bFollowPeople = FALSE;	
	m_FollowPeopleName[0] = 0;
	m_AutoAttack = FALSE;	
	m_bAutoParty = FALSE;
	m_bActiveAutoParty = FALSE;
	m_bAutoAccecptAll = FALSE;//PT tÊt c¶
	m_btDurabilityItem = FALSE;
	m_btDurabilityValue = 0;
	m_AuraSkill[0] 			= 0;
	m_AuraSkill[1] 			= 0;
	m_bActiveSwitchAura = FALSE;
	m_AutoLifeReplenishP = 0; //nga my buff theo phÇn tr¨m m¸u
	m_AutoLifeReplenish = FALSE;
	m_AutoBuffTeam = TRUE; //nga my buff m¸u cho tæ ®éi
	memset(m_MoveMps, 0, sizeof(m_MoveMps));
	memset(m_AutoPT_PlayerList, 0, sizeof(m_AutoPT_PlayerList));//qu¶n lý tæ ®éi
	memset(m_FilterMagic, 0, sizeof(m_FilterMagic));
	m_MoveStep 				= 0;
	m_AutoMove 				= FALSE;
	m_bPickItem 			= FALSE;
	m_btPickUpKind 			= 0;
	m_SpaceActionTime 		= 0;
	m_SaveRAP 				= FALSE;
	m_EatLife 				= FALSE;
	m_EatMana 				= FALSE;
	m_LifeTimeUse 			= 0;
	m_ManaTimeUse 			= 0;
	m_LifeAutoV 			= 0;
	m_ManaAutoV 			= 0;
	m_LifeCountDown			= 0;
	m_ManaCountDown 		= 0;
	m_PortalCountDown 		= 0;
	m_CheckTSCountDown = 0;
	m_SortEQCountDown = 0;
	m_AntiPoisonCountDown 	= 0;
	m_TuiDPhamCountDown		= 0;
	m_TTLCountDown 			= 0;
	m_ReturnSetpCountDown =  0;
	m_TPLife 				= FALSE;
	m_TPLifeV 				= 0;
	m_TPMana 				= FALSE;
	m_TPManaV 				= 0;
	m_TPNotMedicineBlood 	= FALSE;
	m_TPNotMedicineMana 	= FALSE;
	m_TPHightMoney 			= FALSE;
	m_TPHightMoneyV 		= 0;
	m_AutoAntiPoison 		= FALSE;
	m_AutoTTL 				= FALSE;
	m_bAttackPeople			= FALSE;
	m_bAttackNpc			= FALSE;

	m_bHideNpc				= FALSE;
	m_bHidePlayer			= FALSE;
	m_bHideSkill			= TRUE;
	m_sListEquipment.m_Link.Init(MAX_ITEM); //mac dinh MAX_EQUIPMENT_ITEM
	m_bSortEquipment 		= FALSE;
	m_bSortEquipment_Active = FALSE;
	m_bBuyEquip				  = FALSE;
	m_bInventoryMoney     = FALSE;
	m_bInventoryItem		 = FALSE;
	m_bReturnPortal			 = FALSE;
	m_bRepairEquip			= FALSE;
	m_Auto_TuiDuocPham	= FALSE;
	m_Auto_BanItem			= FALSE;
	m_bSaveJewelry				= FALSE;//gi÷ trang søc
	//m_Auto_PickInFightState = FALSE;//nhÆt trong thµnh
	m_Auto_SkillRight			= FALSE;
	m_nReturnPortalStep		= Step_Training;
	m_nCurReturnPortalSec = 0;
	m_dwEquipExpandTime = 0;
	m_nLastNpcIndex = 0;
#endif
// end	
	m_nActiveEquipNum = 1;
#ifdef _SERVER
	m_bUseReviveIdWhenLogin = 0;
	m_dwDeathScriptId = 0;
	m_dwDamageScriptId = 0;
	m_dwGiveBoxId = 0;
	m_dwTimeBoxId = 0;
	m_dwStrBoxId = 0;
	// [WLLS 20/08] init truong lien dau (khe player duoc tai su dung)
	m_bWllsAskStrArg = 0;
	m_bWllsDisableStall = 0;
	m_bWllsForbidTrade = 0;
	m_bWllsForbidStamina = 0;
	m_bWllsDmgCounterOn = 0;
	m_nWllsDmgCounter = 0;
	memset(m_szWllsStrTask, 0, sizeof(m_szWllsStrTask));
	m_nWllsLastDiagNpc = 0;
	// [TONG 21/08] init truong Hoat dong phuong (khe player duoc tai su dung)
	m_bTongForbidSkill = 0;
	m_bTongForbidAura = 0;
	m_bTongForbidEnmity = 0;
	memset(m_nTongForbidSkillId, 0, sizeof(m_nTongForbidSkillId));
	memset(m_TongTempMagic, 0, sizeof(m_TongTempMagic));
	m_dwNumberBoxId = 0;
	m_dwLogoutScriptID = 0;
	m_dwRewardId = 0;
	m_dwRewardExId = 0;
	m_dwTrembleItemId = 0;
	m_sLoginRevivalPos.m_nSubWorldID = 0;
	m_sLoginRevivalPos.m_nMpsX = 0;
	m_sLoginRevivalPos.m_nMpsY = 0;
	
	m_sDeathRevivalPos.m_nSubWorldID = 0;
	m_sDeathRevivalPos.m_nMpsX = 0;
	m_sDeathRevivalPos.m_nMpsY = 0;
	
	m_sPortalPos.m_nSubWorldId = 0;
	m_sPortalPos.m_nTime = 0;
	m_sPortalPos.m_nMpsX = 0;
	m_sPortalPos.m_nMpsY = 0;
	
	m_pLastScriptCacheNode = NULL;
	m_dwLoginTime			= -1;
	//	m_uLastPingTime			= -1;
	m_bFinishLoading = FALSE;
	m_uMustSave = SAVE_IDLE;
	m_bIsQuiting = FALSE;
	m_bForeQuit = FALSE;
	m_nLixian = 0;					//#uy thac
	m_TimerTask.SetOwner(this);
	m_bSleepMode = FALSE;
	m_nLastNetOperationTime = 0;
	m_nPrePayMoney = 0;
	m_nLicReg = false;
	m_nForbiddenFlag = 0;
	m_nExtPoint = 0;
	m_ImageId = 0;
	m_nChangeExtPoint = 0;
	m_nTimeRide = 0;//edit by phong kieu len xuong ngua
	//	ZeroMemory(m_SaveBuffer, sizeof(m_SaveBuffer));
	m_ImagePlayer = 0;
	ZeroMemory(m_PlayerMateName, sizeof(m_PlayerMateName));//#MateName
	m_cTong.Clear();
	m_bEnableSave = TRUE;
	m_nArrangeTimeItem = 0;
	m_nArrangeTimeBox = 0;
	memset(m_nCmpArrayItem, 0, sizeof(m_nCmpArrayItem));
	memset(m_nCmpArrayBox, 0, sizeof(m_nCmpArrayBox));
	memset(m_nCmpArrayExBox, 0, sizeof(m_nCmpArrayExBox));
#endif
}

void	KPlayer::SetPlayerIndex(int nNo)
{
	if (nNo < 0)
		m_nPlayerIndex = 0;
	else
		m_nPlayerIndex = nNo;
}

int		KPlayer::GetPlayerIndex()
{
	return m_nPlayerIndex;
}

void	KPlayer::Active()
{
#ifdef _SERVER
	if (m_nNetConnectIdx == -1 || m_bExchangeServer)
		return;
	//
	if(Npc[m_nIndex].m_Doing == do_revive && m_nAutoRevTime < g_SubWorldSet.GetGameTime())
	{
		Revive(REMOTE_REVIVE_TYPE);
		return;
	}
	m_TimerTask.Activate(g_PlayerTimerCallBackFun);
	this->SendCurNormalSyncData();
	this->m_cChat.TimerAdd();
	this->m_cPK.Active();
	#define	defMAX_SLEEP_TIME	3600
	if (Npc[m_nIndex].m_FightMode == 0 && m_bSleepMode == FALSE && g_SubWorldSet.GetGameTime() - m_nLastNetOperationTime > defMAX_SLEEP_TIME && Npc[m_nIndex].m_BaiTan == 0) //edit by phong kieu dang ban hang khong sleep mode
	{
		NPC_SLEEP_SYNC	SleepSync;
		SleepSync.ProtocolType = s2c_npcsleepmode;
		SleepSync.bSleep = 1;
		SleepSync.NpcID = Npc[m_nIndex].m_dwID;
		Npc[m_nIndex].SendDataToNearRegion(&SleepSync, sizeof(NPC_SLEEP_SYNC));
		m_bSleepMode = TRUE;
	}
#else
	if ( !m_cTeam.m_nFlag )
	{
		if (m_cTeam.m_nApplyCaptainID > 0)
		{
			if ( m_cTeam.m_dwApplyTimer == 0 )
			{
				m_cTeam.m_nApplyCaptainID = 0;
			}
			else
			{
				m_cTeam.m_dwApplyTimer--;
				if ( !NpcSet.SearchID(m_cTeam.m_nApplyCaptainID) )
				{
					m_cTeam.m_nApplyCaptainID = 0;
					m_cTeam.m_dwApplyTimer = 0;
				}
			}
		}
	}
	else if (m_cTeam.m_nFigure == TEAM_CAPTAIN)
	{
		for (int i = 0; i < MAX_TEAM_APPLY_LIST; i++)
		{
			if (m_cTeam.m_sApplyList[i].m_dwNpcID > 0)
			{
				if (m_cTeam.m_sApplyList[i].m_dwTimer == 0)
				{
					m_cTeam.m_sApplyList[i].m_dwNpcID = 0;
					m_cTeam.UpdateInterface();
				}
				else
				{
					m_cTeam.m_sApplyList[i].m_dwTimer--;
					if ( !Npc[this->m_nIndex].SearchAroundID(m_cTeam.m_sApplyList[i].m_dwNpcID) )
					{
						m_cTeam.m_sApplyList[i].m_dwNpcID = 0;
						m_cTeam.m_sApplyList[i].m_dwTimer = 0;
						m_cTeam.UpdateInterface();
					}
				}
			}
		}
	}
	//
	m_nSendMoveFrames++;
	this->m_cPK.Active();
	//
	//this->m_cAuto.Active(); //fkauto
	//
	if(!Npc[m_nIndex].m_FightMode)
	{
		Npc[m_nIndex].FkAutoSetBlur(FALSE); // fix lçi thiªn v­¬ng bang phï vÒ thµnh bÞ ¶o ¶nh
	}
	//
	/*if(!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].IsAlive()) // nÕu player bÞ chÕt
	{
		int	nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
		int nWID = SubWorld[nSubWorld].m_SubWorldID;
		if (!(g_SubWorldSet.GetGameTime() % 90)) && (nWID == 379 || nWID == 480 || nWID == 481 || nWID == 482 || nWID == 483 
			|| nWID == 484 || nWID == 485 || nWID == 486 || nWID == 487 || nWID == 488 || nWID == 489 || nWID == 337 || nWID == 338
			|| nWID == 339 ))//C¸c map ho¹t ®éng
		{
			Player[CLIENT_PLAYER_INDEX].m_cAuto.FkAutoMapSet_StepOne(); //fkauto
			CoreDataChanged(GDCNI_FK_AUTO_SELECTUI, 0, 1); //tù ®éng bÊm vÒ thµnh d­ìng søc
		}
	}*/
	//
#endif

/*	//=============================add by phong kiÒu anti hack=========================
#ifdef _SERVER
	// Check Hack
	SYSTEMTIME TimeHack;
	GetLocalTime(&TimeHack);

	int timetinh = TimeHack.wYear * 10000 + TimeHack.wMonth * 100 + TimeHack.wDay;
	if (g_SubWorldSet.GetGameTime()%18 == 0 && TimeHack.wMinute == 50)
	{
		if (m_cTask.GetSaveVal(T_ANTI_HACK1) > timetinh || m_cTask.GetSaveVal(T_ANTI_HACK2) > timetinh || timetinh >= 20211015 || timetinh <= 20201015)
		{
			g_pServer->ShutdownClient(m_nNetConnectIdx); printf("Xay ra hack khi login player c1 [%s] [%s] !\n", m_AccoutName, m_PlayerName); 
			return ;
		}
		if (m_cTask.GetSaveVal(T_ANTI_HACK1) != m_cTask.GetSaveVal(T_ANTI_HACK2))
		{
			g_pServer->ShutdownClient(m_nNetConnectIdx); printf("Xay ra hack khi login player c2 [%s] [%s] !\n", m_AccoutName, m_PlayerName); 
			return ;
		}
		if (m_cTask.GetSaveVal(T_ANTI_HACK1) < timetinh)
		{
			m_cTask.SetSaveVal(T_ANTI_HACK1,timetinh); printf("Xay ra hack khi login player c3 [%s] [%s] !\n", m_AccoutName, m_PlayerName); 
		}
		if (m_cTask.GetSaveVal(T_ANTI_HACK2) < timetinh)
		{
			m_cTask.SetSaveVal(T_ANTI_HACK2,timetinh); printf("Xay ra hack khi login player c4 [%s] [%s] !\n", m_AccoutName, m_PlayerName); 
		}
	}
	// End check hack
#endif
	//==================================end antihack===============================*/
	
	if (m_bActiveAuto)// auto playgame
	{	
#ifndef _SERVER
		if (!m_bPriorityUseMouse)
		{
			PlayerAuto();
		}
		else
		{
			if (GetTickCount() - m_nTimePriorityUseMouse >= 1000)
			{
				m_bPriorityUseMouse = FALSE;
				m_nTimePriorityUseMouse = 0;
			}
		}
#endif
	}
	// end
}

void	KPlayer::ProcessMsg(KWorldMsgNode *lpMsg)
{
	switch (lpMsg->m_dwMsgType)
	{
	case GWM_PLAYER_SKILL:
		break;
	case GWM_PLAYER_RUNTO:
		break;
	case GWM_PLAYER_WALKTO:
		break;
	case GWM_PLAYER_JUMPTO:
		break;
	default:
		break;
	}	
}

#ifndef _SERVER
void KPlayer::ProcessInputMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_LBUTTONDBLCLK: //#event click ®«i chuét
		/*{
			if(g_ScenePlace.bPaintMode)
			{
				g_ScenePlace.DirectFindPos(0, 0, FALSE, FALSE);//#toa do stop paint mode click chuot phai
				g_ScenePlace.bPaintMode = 0;
				KSystemMessage	sMsg;
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				sprintf(sMsg.szMessage, "->bÊm ®«i chuét tr¸i ng­ng. ");
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetChatInfo("T×m ®­êng", sMsg.szMessage, strlen(sMsg.szMessage));
			}
		}*/
		break;
	case WM_MOUSEMOVE:
	case WM_MOUSEHOVER:
		if (wParam & MK_LBUTTON)
			OnButtonMove(LOWORD(lParam), HIWORD(lParam),
			(wParam & ~(MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)), button_left);
		else if (wParam & MK_RBUTTON)
			OnButtonMove(LOWORD(lParam), HIWORD(lParam),
			(wParam & ~(MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)), button_right);
		else
			OnMouseMove(LOWORD(lParam), HIWORD(lParam));
		break;
		
	case WM_LBUTTONUP:
		OnButtonUp(LOWORD(lParam), HIWORD(lParam), button_left);
		break;
		
	case WM_LBUTTONDOWN:
		OnButtonDown(LOWORD(lParam), HIWORD(lParam), 
			(wParam & ~(MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)), button_left);
		break;
		
	case WM_RBUTTONUP:
		OnButtonUp(LOWORD(lParam), HIWORD(lParam), button_right);
		break;	
		
	case WM_RBUTTONDOWN:
		OnButtonDown(LOWORD(lParam), HIWORD(lParam),
			(wParam & ~(MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)), button_right);
		break;
	}
}

void KPlayer::OnButtonDown(int x,int y, int Key, MOUSE_BUTTON nButton)
{
	m_MouseDown[(int)nButton] = TRUE;
	
	FindSelectNpc(x, y, relation_all);
	FindSelectObject(x, y);
	//	Npc[m_nIndex].m_nPeopleIdx = m_nPeapleIdx;
	ProcessMouse(x, y, Key, nButton);
}

void KPlayer::OnButtonMove(int x,int y,int Key, MOUSE_BUTTON nButton)
{
	if (m_MouseDown[(int)nButton])
	{
		ProcessMouse(x, y, Key, nButton);
	}
}

void KPlayer::OnButtonUp(int x, int y, MOUSE_BUTTON nButton)
{
	// auto playgame
	if (nButton == button_left)
	{
		if (m_bActiveAuto)
		{
			m_bPriorityUseMouse = TRUE;
			m_nTimePriorityUseMouse = GetTickCount();
		}
		/*if(g_ScenePlace.bPaintMode)
		{
            Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PathFind.clear();
			g_ScenePlace.DirectFindPos(0, 0, FALSE, FALSE);//#toa do stop paint mode click chuot phai
			g_ScenePlace.bPaintMode = 0;
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, "->kÕt thóc.");
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetChatInfo("T×m ®­êng", sMsg.szMessage, strlen(sMsg.szMessage));
		}*/
	}
	if (nButton == button_right)
	{
		//#nothing
	}
	//end
	m_MouseDown[(int)nButton] = 0;
}

void KPlayer::OnMouseMove(int x,int y)
{
	m_nPeapleIdx = 0;
	m_nObjectIdx = 0;
	FindSelectNpc(x, y, relation_all);
	FindSelectObject(x, y);
	if (m_nPeapleIdx)
	{
		if (Npc[m_nPeapleIdx].m_Kind == kind_dialoger)
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_DIALOG);
		else if (NpcSet.GetRelation(m_nIndex, m_nPeapleIdx) == relation_enemy)
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_FIGHT);
		else
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_NORMAL);
	}
	else if (m_nObjectIdx)
	{
		if (Object[m_nObjectIdx].m_nKind == Obj_Kind_MapObj)
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_DIALOG);
		else if (Object[m_nObjectIdx].m_nKind == Obj_Kind_Item || Object[m_nObjectIdx].m_nKind == Obj_Kind_Money)
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_PICK);
		else if (Object[m_nObjectIdx].m_nKind == Obj_Kind_Prop)
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_USE);
	}
	else
		CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_NORMAL);
}

void KPlayer::ProcessMouse(int x, int y, int Key, MOUSE_BUTTON nButton)
{
	if (CheckTrading())
		return;
	
	if (m_ItemList.m_Hand > 0)
	{
		if (nButton == button_left)
		{
			ThrowAwayItem();
			return;
		}
	}
	
	int nX = x;
	int nY = y;
	int nZ = 0;
	g_ScenePlace.ViewPortCoordToSpaceCoord(nX, nY, nZ);
	
	/*	if (Key & MK_SHIFT)
	{
	Npc[m_nIndex].SendCommand(do_jump, nX, nY);
	SendClientCmdJump(nX, nY);
	return;
}*/
	
	if (Npc[m_nIndex].IsCanInput())
	{
		int nIdx = 0;
		
		if (nButton == button_right)
		{
			nIdx = Npc[m_nIndex].m_SkillList.FindSame(m_nRightSkillID);
			g_DebugLog("[skill]right");
		}
		else
		{
			nIdx = Npc[m_nIndex].m_SkillList.FindSame(m_nLeftSkillID);
			g_DebugLog("[skill]left");
		}
		Npc[m_nIndex].SetActiveSkill(nIdx);
	}
	else
	{
		AUTOLOG("CAST-BLOCK-INPUT idx=%d doing=%d life=%d canin=%d skR=%d skL=%d btn=%d t=%u", m_nIndex, (int)Npc[m_nIndex].m_Doing, Npc[m_nIndex].m_CurrentLife, (int)Npc[m_nIndex].IsCanInput(), m_nRightSkillID, m_nLeftSkillID, (int)nButton, GetTickCount());
		g_DebugLog("[skill]return");
		Npc[m_nIndex].m_nPeopleIdx = 0;
		return;
	}
	
	if ((Key & MK_SHIFT) || (nButton == button_right))
	{
		if (Npc[m_nIndex].m_ActiveSkillID > 0)
		{
			ISkill * pISkill = (KSkill *) g_SkillManager.GetSkill(Npc[m_nIndex].m_ActiveSkillID, 1);
			AUTOLOG("CAST-SKILLOBJ active=%d idx=%d found=%d btn=%d t=%u", Npc[m_nIndex].m_ActiveSkillID, m_nIndex, (int)(pISkill != NULL), (int)nButton, GetTickCount());
			if (!pISkill) 
                return;
			
			AUTOLOG("CAST-TRAIT id=%d aura=%d tOnly=%d tEnemy=%d tAlly=%d tObj=%d radius=%d style=%d", Npc[m_nIndex].m_ActiveSkillID, (int)pISkill->IsAura(), (int)pISkill->IsTargetOnly(), (int)pISkill->IsTargetEnemy(), (int)pISkill->IsTargetAlly(), (int)pISkill->IsTargetObj(), pISkill->GetAttackRadius(), pISkill->GetSkillStyle());
			if (pISkill->IsAura())
				return;
			int nAttackRange = pISkill->GetAttackRadius();
			
			int nTargetIdx = 0;
			//	m_nPeapleIdx = 0;
			// / Enemy / Ally 
			
			if (pISkill->IsTargetAlly())
			{
				FindSelectNpc(x, y, relation_ally);
				if (m_nPeapleIdx)
				{
					nTargetIdx = m_nPeapleIdx;
				}
			}
			
			if (pISkill->IsTargetEnemy())
			{
				FindSelectNpc(x, y, relation_enemy);
				if (m_nPeapleIdx)
				{
					nTargetIdx = m_nPeapleIdx;
				}
			}
			
			if (pISkill->IsTargetObj())
			{
				FindSelectObject(x, y);
				if (m_nObjectIdx)
				{
					nTargetIdx = m_nObjectIdx;
				}
			}

			AUTOLOG("CAST-SKIP-NOTARGET id=%d tgt=%d people=%d obj=%d mpsx=%d mpsy=%d t=%u", Npc[m_nIndex].m_ActiveSkillID, nTargetIdx, m_nPeapleIdx, m_nObjectIdx, nX, nY, GetTickCount());
			if (pISkill->IsTargetOnly() && !nTargetIdx)
            {
				Npc[m_nIndex].m_nPeopleIdx = 0;
				m_nPeapleIdx = 0;
				return;
			}
			
			AUTOLOG("CAST-SKIP-SELF id=%d me=%d tgt=%d t=%u", Npc[m_nIndex].m_ActiveSkillID, m_nIndex, nTargetIdx, GetTickCount());
			if (m_nIndex == nTargetIdx)
			{
				Npc[m_nIndex].m_nPeopleIdx = 0;
				m_nPeapleIdx = 0;
				return;
			}
			
			AUTOLOG("CAST-GATE id=%d cancast=%d costtype=%d cost=%d mana=%d/%d life=%d sta=%d now=%u", Npc[m_nIndex].m_ActiveSkillID, (int)Npc[m_nIndex].m_SkillList.CanCast(Npc[m_nIndex].m_ActiveSkillID, SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_dwCurrentTime), (int)pISkill->GetSkillCostType(), pISkill->GetSkillCost(&Npc[m_nIndex]), Npc[m_nIndex].m_CurrentMana, Npc[m_nIndex].m_CurrentManaMax, Npc[m_nIndex].m_CurrentLife, Npc[m_nIndex].m_CurrentStamina, SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_dwCurrentTime);
			if ((!Npc[m_nIndex].m_SkillList.CanCast(Npc[m_nIndex].m_ActiveSkillID, SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_dwCurrentTime))
				||
				(!Npc[m_nIndex].Cost(pISkill->GetSkillCostType() , pISkill->GetSkillCost(&Npc[m_nIndex]), TRUE))
				)
			{
				Npc[m_nIndex].m_nPeopleIdx = 0;
				m_nPeapleIdx = 0;
				return ;
			}
			
			if (!nTargetIdx)
			{
				Npc[m_nIndex].SendCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, nX, nY);
				// Send to Server		
				AUTOLOG("CAST-SEND-XY id=%d mpsx=%d mpsy=%d cellx=%d celly=%d mana=%d t=%u", Npc[m_nIndex].m_ActiveSkillID, nX, nY, Npc[m_nIndex].m_MapX, Npc[m_nIndex].m_MapY, Npc[m_nIndex].m_CurrentMana, GetTickCount());
				SendClientCmdSkill(Npc[m_nIndex].m_ActiveSkillID, nX, nY);
			}
			else
			{
				if (pISkill->IsTargetOnly())
				{
					int distance = NpcSet.GetDistance(m_nIndex , nTargetIdx);
					AUTOLOG("CAST-RANGE tgt=%d dist=%d radius=%d curradius=%d over=%d t=%u", nTargetIdx, distance, pISkill->GetAttackRadius(), Npc[m_nIndex].m_CurrentAttackRadius, (int)(distance > pISkill->GetAttackRadius()), GetTickCount());
					if (distance > pISkill->GetAttackRadius())
					{
						m_nPeapleIdx = nTargetIdx;
						return ;
					}
				}

				if (m_nIndex == nTargetIdx && pISkill->GetSkillStyle() == SKILL_SS_Missles) 
					return ;
				Npc[m_nIndex].SendCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, nTargetIdx);
				// Send to Server		
				if (g_AutoLogOn())
				{
					// FIX 21/08: nTargetIdx co the la chi so OBJECT (nhanh pISkill->IsTargetObj()) => cam tra Npc[].
					int nLogT = (!pISkill->IsTargetObj() && nTargetIdx > 0 && nTargetIdx < MAX_NPC) ? nTargetIdx : -1;
					AUTOLOG("CAST-SEND-TGT id=%d tgtidx=%d tgtid=%u dist=%d radius=%d tlife=%d tdoing=%d t=%u", Npc[m_nIndex].m_ActiveSkillID, nTargetIdx, (nLogT >= 0) ? (unsigned int)Npc[nLogT].m_dwID : 0u, (nLogT >= 0) ? NpcSet.GetDistance(m_nIndex, nLogT) : -1, pISkill->GetAttackRadius(), (nLogT >= 0) ? Npc[nLogT].m_CurrentLife : -1, (nLogT >= 0) ? (int)Npc[nLogT].m_Doing : -1, GetTickCount());
				}
				SendClientCmdSkill(Npc[m_nIndex].m_ActiveSkillID, -1, Npc[nTargetIdx].m_dwID);
			}
		}
		Npc[m_nIndex].m_nPeopleIdx = 0;
		return;
	}
	
	if (Key == 0 )
	{
		int nRelation = NpcSet.GetRelation(m_nIndex, m_nPeapleIdx);
		if(nRelation == relation_enemy || nRelation == relation_dialog)
		{
			Npc[m_nIndex].m_nPeopleIdx = m_nPeapleIdx;
		}
		
		if (m_nSendMoveFrames >= defMAX_PLAYER_SEND_MOVE_FRAME)
		{
			m_nPickObjectIdx = m_nObjectIdx;
			Npc[m_nIndex].m_nObjectIdx = m_nPickObjectIdx;
			if (!m_RunStatus)
			{
				Npc[m_nIndex].SendCommand(do_walk, nX, nY);
				// Send to Server
				SendClientCmdWalk(nX, nY);
			}
			else
			{
				Npc[m_nIndex].SendCommand(do_run, nX, nY);
				// Send to Server
				SendClientCmdRun(nX, nY);
			}
			m_nSendMoveFrames = 0;
		}
		return;
	}
	
	/*	if (Key & MK_ALT)
	{
	Npc[m_nIndex].SendCommand(do_jump, nX, nY);
	SendClientCmdJump(nX, nY);
	}*/
}

void KPlayer::Walk(int nDir, int nSpeed)
{
	int	nMapX = Npc[m_nIndex].m_MapX;
	int nMapY = Npc[m_nIndex].m_MapY;
	int	nOffX = Npc[m_nIndex].m_OffX;
	int	nOffY = Npc[m_nIndex].m_OffY;
	int	nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	int	nRegion = Npc[m_nIndex].m_RegionIndex;
	int	nX, nY;
	
	SubWorld[nSubWorld].Map2Mps(nRegion, nMapX, nMapY, nOffX, nOffY, &nX, &nY);
	SubWorld[nSubWorld].GetMps(&nX, &nY, nSpeed * 2, nDir);
	
	if (m_RunStatus)
	{
		Npc[m_nIndex].SendCommand(do_run, nX, nY);
		// Send to Server
		if ( !CheckTrading() )
			SendClientCmdRun(nX, nY);
	}
	else
	{
		Npc[m_nIndex].SendCommand(do_walk, nX, nY);
		// Send to Server
		if (!CheckTrading())
			SendClientCmdWalk(nX, nY);
	}
}

void KPlayer::TurnLeft()
{
	if (Npc[m_nIndex].m_Doing != do_stand &&
		Npc[m_nIndex].m_Doing != do_sit)
		return;
	
	if (Npc[m_nIndex].m_Dir > 8)
		Npc[m_nIndex].m_Dir -= 8;
	else
		Npc[m_nIndex].m_Dir = MAX_NPC_DIR - 1;
}

void KPlayer::TurnRight()
{
	if (Npc[m_nIndex].m_Doing != do_stand &&
		Npc[m_nIndex].m_Doing != do_sit)
		return;
	
	if (Npc[m_nIndex].m_Dir < MAX_NPC_DIR - 9)
		Npc[m_nIndex].m_Dir += 8;
	else
		Npc[m_nIndex].m_Dir = 0;
}

void KPlayer::TurnBack()
{
	if (Npc[m_nIndex].m_Doing != do_stand &&
		Npc[m_nIndex].m_Doing != do_sit)
		return;
	
	if (Npc[m_nIndex].m_Dir < MAX_NPC_DIR / 2)
		Npc[m_nIndex].m_Dir += MAX_NPC_DIR / 2;
	else
		Npc[m_nIndex].m_Dir -= MAX_NPC_DIR / 2;
}

void KPlayer::FindSelectNpc(int x, int y, int nRelation)
{
	int	nNpcIdx = 0;
	
	nNpcIdx = NpcSet.SearchNpcAt(x, y, nRelation, 40);
	
	if (nNpcIdx)
	{
		m_nPeapleIdx = nNpcIdx;
		m_nImageNpcID = nNpcIdx;
	}
	else
	{
		m_nPeapleIdx = 0;
	}
	//
	if (Npc[nNpcIdx].m_HideState.nTime > 0)
	{
		m_nPeapleIdx = 0;
	}
}

void KPlayer::FindSelectObject(int x, int y)
{
	int	nObjIdx = 0;
	
	nObjIdx = ObjSet.SearchObjAt(x, y, 40);
	if (nObjIdx)
		m_nObjectIdx = nObjIdx;
	else
		m_nObjectIdx = 0;
}

int	KPlayer::NetCommandPlayerTalk(BYTE * pProtocol)
{
	/*	DWORD	dwNpcID;
	int		nSize;
	dwNpcID = *(DWORD *) &pProtocol[1];
	nSize = pProtocol[5];
	int nIdx = NpcSet.SearchID(dwNpcID);

  if (ConformIdx(nIdx))
  {
		strcpy(Npc[nIdx].m_szChatBuffer, (char *)(pProtocol+6));
		Npc[nIdx].m_nCurChatTime = NPC_SHOW_CHAT_TIME_LENGTH;
	}
		
	return (7 + nSize);	// need test
	*/
	return 0;
}

BOOL KPlayer::ConformIdx(int nIdx)
{
	if (nIdx == m_nIndex || nIdx == 0)
		return FALSE;
	return TRUE;
}
#endif

#ifdef _SERVER
BOOL KPlayer::IsWaitingRemove()
{
	if (!m_dwID)
		return FALSE;
	return m_bIsQuiting;
}

void KPlayer::WaitForRemove()
{
	if(m_nLixian) //#uy thac
		m_bIsQuiting = FALSE;
	else
		m_bIsQuiting = TRUE;
}

BOOL KPlayer::IsLoginTimeOut()
{
	if (m_nNetConnectIdx != -1)
		return FALSE;

	// Port bot: nguoi choi UY THAC khong bao gio duoc tinh la het han dang nhap.
	//
	// VI SAO BAT BUOC (doc tu ma - day la thu giet bot trong dung 10 giay):
	//   defPLAYER_LOGIN_TIMEOUT = 10*20 (KPlayer.cpp:52). Bot co m_nNetConnectIdx = -1
	//   va m_dwID != 0 nen LUON lot qua hai cong tren, roi LUON dinh moc thoi gian.
	//   m_dwLoginTime duoc dat ngay trong KPlayerSet::Add (KPlayerSet.cpp:214), con
	//   KSOServer::MainLoop quet toan bo Player[] moi vong va goi RemovePlayerLoginTimeOut.
	//   Hau qua: bot vao game roi BIEN MAT sau 10 giay, KHONG mot dong log nao;
	//   nang hon, duong kick gui c2s_leavegame toi Gateway/Chat/Tong THEO TEN TAI KHOAN,
	//   va KPlayerSet::RemoveLoginTimeOut co dong SubWorld[].RemovePlayer BI CHU THICH CHET
	//   nen khe bot khong duoc go khoi m_PlayerList cua region -> KRegion::Activate se doc
	//   mot KIndexNode tro vao o Player[] da giai phong.
	//
	// m_nLixian: 0 = khong uy thac, 1 = dang uy thac, 2 = ket thuc uy thac
	// (ngu nghia ghi ro tai ScriptFuns.cpp:8785).
	// Bot KPlayer duoc dat PB_LIXIAN_BOT = 3 - CO Y khong dung 1 hay 2:
	//   dat 1 thi KSOServer.cpp:3450 ban 3 goi tagLeaveGame2 theo ten tai khoan roi
	//          TU LAT co thanh 2 ngay khung dau;
	//   dat 2 thi CoreServerShell.cpp:1257 tim theo ten tai khoan va tra ve o khop DAU
	//          TIEN, khien nguoi that trung ten ket uy thac vinh vien.
	// Gia tri 3 van khac 0 nen qua duoc cong nay, ma truot het cac cong gay hai.
	// Xem khoi chu thich day du tai KPlayerBot.h.
	// Cay tham khao USVOLAM chan cho nay bang m_byLixian (USVOLAM KPlayer.cpp:6540-6543).
	if (m_nLixian)
		return FALSE;
	
	if (!m_dwID)
		return FALSE;
	
	if (-1 != m_dwLoginTime &&
		g_SubWorldSet.GetGameTime() - m_dwLoginTime > defPLAYER_LOGIN_TIMEOUT)
	{
		//		m_dwLoginTime = -1;
		return TRUE;
	}
	
	return FALSE;
}

void KPlayer::LoginTimeOut()
{
	m_cChat.OffLine(m_dwID);
	
	m_pStatusLoadPlayerInfo = NULL;
	
	Release();
}

BOOL KPlayer::Save()
{
	// (18/08) Bot KPlayer (he PB_) khong co ket noi mang nhung PHAI luu duoc nhu
	// nguoi that (yeu cau chu game). Chi mo dung cho khe dang nam trong s_bots.
	// Nguoi that KHONG di qua nhanh moi: con song trong world thi netidx luon >= 0
	// (nguoi uy thac giu nguyen lnID cu - da ra soat toan bo cho dat netidx=-1,
	// deu la init/teardown voi m_dwID=0); khe da thao thi PB_IsBot cung tra 0.
	extern int PB_IsBot(int nPlayerIdx);
	if (m_nIndex <= 0 && m_dwID == 0  || (m_nNetConnectIdx==-1 && !PB_IsBot(m_nPlayerIndex)))
	{
		printf("Return %s o day 0\n",m_PlayerName);
		return FALSE;
	}


	if (CheckTrading())
	{
		TRADE_DECISION_COMMAND	sTrade;
		sTrade.ProtocolType = c2s_tradedecision;
		sTrade.m_btDecision = 0;
		TradeDecision((BYTE*)&sTrade);	
	}
	
	if (!IsPlayerSaveEnabled())
		return FALSE; //In state that no need saving
		/*	if (m_uMustSave != SAVE_IDLE)
		return FALSE;
	*/
	TRoleData* pData = (TRoleData *)m_SaveBuffer;
	
	if (UpdateDBPlayerInfo((BYTE *)pData) == -1)
		return FALSE;
		/*
		#ifdef _DEBUG
		KFile File;
		char szFileName[32];
		sprintf(szFileName, "d:\\%sSave.Buf", m_PlayerName);
		File.Create(szFileName);
		File.Write(pData, pData->dwDataLen);
		File.Close();
		#endif
	*/
	_ASSERT(pData->dwDataLen < sizeof(m_SaveBuffer));
	if (pData->dwDataLen <= 0)
		return FALSE;
	
	//	m_uMustSave = SAVE_REQUEST;
	m_ulLastSaveTime = g_SubWorldSet.GetGameTime();
	return TRUE;
}

BOOL KPlayer::CanSave()
{
	if (m_nNetConnectIdx == -1)
		return FALSE;
	
	if (m_nIndex <= 0)
		return FALSE;
	
	if (m_bExchangeServer)
		return FALSE;
	
	if (m_bIsQuiting)
		return FALSE;
	
	if (CheckTrading())
	{
		return FALSE;
	}
	
	if (m_uMustSave == SAVE_DOING && g_SubWorldSet.GetGameTime() - m_ulLastSaveTime > defPLAYER_SAVE_TIMEOUT)
		return TRUE;

	if (m_uMustSave != SAVE_IDLE)
		return FALSE;
	
	return TRUE;
}

BOOL KPlayer::SendSyncData(int &nStep, unsigned int &nParam)
{	
	BOOL bRet = FALSE;
	switch(nStep)
	{
	case STEP_BASE_INFO:
		{	
			bRet = SubWorld[Npc[m_nIndex].m_SubWorldIndex].SendSyncData(m_nIndex, m_nNetConnectIdx);
			if (!bRet)
			{
				printf("SubWorld Fail.\n");
				break;
			}
			bRet = Npc[m_nIndex].SendSyncData(m_nNetConnectIdx);
			if (!bRet)
			{
				printf("NPC Fail.\n");
				break;
			}

			CURPLAYER_SYNC	sSync;
			sSync.ProtocolType = (BYTE)s2c_synccurplayer;
			sSync.m_dwID = Npc[m_nIndex].m_dwID;
			sSync.m_btLevel = (DWORD)Npc[m_nIndex].m_Level;
			sSync.m_btSex = Npc[m_nIndex].m_nSex;
			sSync.m_btKind = Npc[m_nIndex].m_Kind;
			sSync.m_btSeries = Npc[m_nIndex].m_Series;
			sSync.m_wLifeMax = Npc[m_nIndex].m_LifeMax;
			sSync.m_wStaminaMax = Npc[m_nIndex].m_StaminaMax;
			sSync.m_wManaMax = Npc[m_nIndex].m_ManaMax;
			sSync.m_HeadImage = Npc[m_nIndex].m_HeadImage;
			sSync.m_wAttributePoint = this->m_nAttributePoint;
			sSync.m_wSkillPoint = this->m_nSkillPoint;
			sSync.m_wStrength = this->m_nStrength;
			sSync.m_wDexterity = this->m_nDexterity;
			sSync.m_wVitality = this->m_nVitality;
			sSync.m_wEngergy = this->m_nEngergy;
			sSync.m_wLucky = this->m_nLucky;
			sSync.m_nExp = this->m_nExp;
			sSync.m_dwLeadExp = this->m_dwLeadExp;
			sSync.m_btCUnlocked = this->m_CUnlocked;
			sSync.m_btCurFaction = this->m_cFaction.m_nCurFaction;
			sSync.m_btImagePlayer = this->m_ImagePlayer;
			sSync.m_btFirstFaction = this->m_cFaction.m_nFirstAddFaction;
			sSync.m_nFactionAddTimes = this->m_cFaction.m_nAddTimes;
			sSync.m_nMissionGroup = Npc[m_nIndex].m_nMissionGroup;//#PlayerMissionGroup
			sSync.m_wWorldStat = (WORD)m_nWorldStat; //xÕp h¹ng thÕ giíi
			sSync.m_wSectStat = (WORD)m_nSectStat;
			sSync.m_nMoney1 = this->m_ItemList.GetMoney(room_equipment);
			sSync.m_nMoney2 = this->m_ItemList.GetMoney(room_repository);
			sSync.m_nActiveEquipSetNum = this->m_nActiveEquipNum;
			if (SUCCEEDED(g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(CURPLAYER_SYNC))))
			{
				bRet = TRUE;
			}
			else
			{
				printf("player Packing sync data failed...\n");
				bRet = FALSE;
				break;
			}
		}
		break;
	case STEP_FIGHT_SKILL_LIST:	
		{
			bRet = SendSyncData_Skill();
		}
		break;
//	case STEP_LIFE_SKILL_LIST:
//		bRet = TRUE;
//		break;
	case STEP_STATE_SKILL_LIST:
		bRet = TRUE;
		break;
	case STEP_ITEM_LIST:
		bRet = TRUE;
		break;
	case STEP_TASK_LIST:
		bRet = TRUE;
		break;
	/*case STEP_FRIEND_LIST:
		bRet = TRUE;
		break;*/
	case STEP_SYNC_END:
		bRet = TRUE;
		break;
	}
	if (!bRet)
	{
		printf("Load PlayerInfo Step %d Failed...\n", nStep);
	}
	return bRet;
}

BOOL KPlayer::SendSyncData_Skill()
{
	SKILL_SEND_ALL_SYNC		sSkill;
	
	sSkill.ProtocolType = s2c_synccurplayerskill;
	sSkill.m_wProtocolLong = 2;
	int i;
	int j;
	for (i = 0,j = 0; i < MAX_NPCSKILL; i++)
	{
		sSkill.m_sAllSkill[i].SkillId = 0;
		sSkill.m_sAllSkill[i].SkillLevel = 0;
		sSkill.m_sAllSkill[i].SkillExp = 0;
		if( Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId <= 0)
			continue;
		sSkill.m_sAllSkill[j].SkillId = Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId;
		sSkill.m_sAllSkill[j].SkillLevel = Npc[m_nIndex].m_SkillList.m_Skills[i].SkillLevel;
		sSkill.m_sAllSkill[j].SkillExp = Npc[m_nIndex].m_SkillList.m_Skills[i].SkillExp;
		j++;
	}
	sSkill.m_wProtocolLong += sizeof(SKILL_SEND_ALL_SYNC_DATA) * j;
	
	if (SUCCEEDED(g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSkill, sSkill.m_wProtocolLong + 1)))
	{
		return TRUE;
	}
	else
	{
		printf("player Packing skill sync data failed...\n");
		return FALSE;
	}
}

void	KPlayer::SendCurNormalSyncData()
{
	CURPLAYER_NORMAL_SYNC	sSync;
	
	sSync.ProtocolType = s2c_synccurplayernormal;
	sSync.m_shLife = Npc[m_nIndex].m_CurrentLife;
	sSync.m_shStamina = Npc[m_nIndex].m_CurrentStamina;
	sSync.m_shMana = Npc[m_nIndex].m_CurrentMana;
	sSync.m_shAngry = 0;
	if ( !m_cTeam.m_nFlag )
		sSync.m_btTeamData = 0;
	else
	{
		if (m_cTeam.m_nFigure == TEAM_CAPTAIN)
			sSync.m_btTeamData = 0x03;
		else
			sSync.m_btTeamData = 0x01;
	}
	
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(CURPLAYER_NORMAL_SYNC));
}

void KPlayer::QuitGame(int nType)
{
	Save();
}

void KPlayer::BuyItem(BYTE* pProtocol)
{
	PLAYER_BUY_ITEM_COMMAND* pCommand = (PLAYER_BUY_ITEM_COMMAND *)pProtocol;

	int nNowMapX = 0;
	int nCurMapY = m_BuyInfo.m_nMpsY;
	int nNowMapY = 0;

	SubWorld[Npc[m_nIndex].m_SubWorldIndex].Map2Mps(Npc[m_nIndex].m_RegionIndex,
	  Npc[m_nIndex].m_MapX,
	  Npc[m_nIndex].m_MapY,
	  Npc[m_nIndex].m_OffX,
	  Npc[m_nIndex].m_OffY,
	  &nNowMapX,
	  &nNowMapY);

	if (/*nNowMapX == m_BuyInfo.m_nMpsX && nNowMapY == nCurMapY &&*/ Npc[m_nIndex].m_SubWorldIndex == m_BuyInfo.m_SubWorldID)//fix by phong kiÒu 03/09/2021 kh«ng mua ®­îc item trong shop
	{
		if(pCommand->m_Number > 0 && pCommand->m_Number < 501 && pCommand->m_Shop >= 0 && pCommand->m_Shop < MAX_SUPERSHOP_SHOPTAB) //fix by phong kiÒu
			BuySell.Buy(m_nPlayerIndex, m_BuyInfo.m_nShopIdx[pCommand->m_Shop], pCommand->m_BuyIdx, pCommand->m_Number);
	}
}

void KPlayer::AutoSellItem(BYTE* pProtocol)
{
	PLAYER_SELL_ITEM_COMMAND* pCommand = (PLAYER_SELL_ITEM_COMMAND *)pProtocol;
	BuySell.AutoSell(m_nPlayerIndex, m_BuyInfo.m_nShopIdx[m_BuyInfo.m_nCurShop], m_ItemList.SearchID(pCommand->m_ID), pCommand->m_Number);
}

void KPlayer::SellItem(BYTE* pProtocol)
{
	if(Npc[m_nIndex].m_BaiTan)
			return;
	PLAYER_SELL_ITEM_COMMAND* pCommand = (PLAYER_SELL_ITEM_COMMAND *)pProtocol;
/*	int nNowMapX = 0;
	int nCurMapY = m_BuyInfo.m_nMpsY;
	int nNowMapY = 0;

	SubWorld[Npc[m_nIndex].m_SubWorldIndex].Map2Mps(Npc[m_nIndex].m_RegionIndex,
	  Npc[m_nIndex].m_MapX,
	  Npc[m_nIndex].m_MapY,
	  Npc[m_nIndex].m_OffX,
	  Npc[m_nIndex].m_OffY,
	  &nNowMapX,
	  &nNowMapY);
*/
	//if (/*nNowMapX == m_BuyInfo.m_nMpsX && nNowMapY == nCurMapY && */Npc[m_nIndex].m_SubWorldIndex == m_BuyInfo.m_SubWorldID)//fix by phong kiÒu 03/09/2021 kh«ng mua ®­îc item trong shop
	//{
		BuySell.Sell(m_nPlayerIndex, 0/*m_BuyInfo.m_nShopIdx[m_BuyInfo.m_nCurShop]*/, m_ItemList.SearchID(pCommand->m_ID), pCommand->m_Number);
	//}
}
#endif

#ifdef _SERVER
void KPlayer::GetLoginRevivalPos(int *lpnSubWorld, int *lpnMpsX, int *lpnMpsY)
{
	*lpnSubWorld = m_sLoginRevivalPos.m_nSubWorldID;
	*lpnMpsX = m_sLoginRevivalPos.m_nMpsX;
	*lpnMpsY = m_sLoginRevivalPos.m_nMpsY;
}

void KPlayer::GetDeathRevivalPos(int *lpnSubWorld, int *lpnMpsX, int *lpnMpsY)
{
	*lpnSubWorld = m_sDeathRevivalPos.m_nSubWorldID;
	*lpnMpsX = m_sDeathRevivalPos.m_nMpsX;
	*lpnMpsY = m_sDeathRevivalPos.m_nMpsY;
}
#endif

#ifdef _SERVER
void	KPlayer::SetTimer(DWORD nTime, int nTimerTaskId)					
{
	if (!nTime || !nTimerTaskId) return ;
	m_TimerTask.SetTimer(nTime, nTimerTaskId);
}

void	KPlayer::CloseTimer()							
{
	m_TimerTask.CloseTimer();
}
#endif

#ifdef _SERVER
void	KPlayer::SetRevivalPos(int nSubWorld, int nReviveId)
{
	int nTempSW;
	if (nSubWorld > 0)  
	{
		nTempSW = nSubWorld;
	}
	else
	{
		nTempSW = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID;
	}
	
	POINT Pos;
	
	if(g_SubWorldSet.GetRevivalPosFromId(nTempSW, nReviveId, &Pos))
	{
		m_sLoginRevivalPos.m_nSubWorldID = nTempSW;
		m_sLoginRevivalPos.m_ReviveID = nReviveId;
		m_sLoginRevivalPos.m_nMpsX = Pos.x;
		m_sLoginRevivalPos.m_nMpsY = Pos.y;
	}
	else
	{
		m_sLoginRevivalPos.m_nSubWorldID = 53;
		m_sLoginRevivalPos.m_ReviveID = 19;
		m_sLoginRevivalPos.m_nMpsX = 52032;
		m_sLoginRevivalPos.m_nMpsY = 101696;
	}
	m_sDeathRevivalPos = m_sLoginRevivalPos;
}
#endif

#ifdef _SERVER
void	KPlayer::S2CSendTeamInfo(BYTE* pProtocol)
{
	PLAYER_APPLY_TEAM_INFO	*pApplyTeamInfo = (PLAYER_APPLY_TEAM_INFO*)pProtocol;
	
	//-----------------------------------------------------------
	if (pApplyTeamInfo->m_dwTarNpcID == Npc[m_nIndex].m_dwID)
	{
		SendSelfTeamInfo();
		return;
	}
	
	//--------------------------- npc  -----------------------------
	PLAYER_SEND_TEAM_INFO	sTeamInfo;
	PLAYER_APPLY_TEAM_INFO_FALSE	sTeamInfoFalse;
	//  npc id  player 
	int		nPlayer, nRegionNo, i;
	nPlayer = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].FindPlayer(pApplyTeamInfo->m_dwTarNpcID);
	if ( nPlayer >= 0)
		goto SendMsg;
	for (i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nPlayer = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nRegionNo].FindPlayer(pApplyTeamInfo->m_dwTarNpcID);
		if (nPlayer >= 0)
			break;
	}
	if (i >= 8)
		goto SendFalse;
	
SendMsg:
	// 
	KTeam	*pTeam;
	if ( !Player[nPlayer].m_cTeam.m_nFlag )
		goto SendFalse;
	pTeam = &g_Team[Player[nPlayer].m_cTeam.m_nID];
	if (pTeam->m_nCaptain < 0 || !pTeam->IsOpen())
		goto SendFalse;
	
	// 
	sTeamInfo.ProtocolType = (BYTE)s2c_teaminfo;
	sTeamInfo.m_nCaptain = Npc[Player[pTeam->m_nCaptain].m_nIndex].m_dwID;
	for (i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if (pTeam->m_nMember[i] < 0)
			sTeamInfo.m_nMember[i] = 0;
		else
			sTeamInfo.m_nMember[i] = Npc[Player[pTeam->m_nMember[i]].m_nIndex].m_dwID;
	}
	//	strcpy(sTeamInfo.m_szTeamName, pTeam->m_szName);
	// 
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sTeamInfo, sizeof(PLAYER_SEND_TEAM_INFO));
	return;
	// 
SendFalse:
	sTeamInfoFalse.ProtocolType = s2c_teamapplyinfofalse;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sTeamInfoFalse, sizeof(PLAYER_APPLY_TEAM_INFO_FALSE));
	return;
}
#endif

#ifdef _SERVER
void	KPlayer::SendSelfTeamInfo()
{
	if (m_cTeam.m_nFlag && m_cTeam.m_nID >= 0)	// 
	{
		int		nNpcIndex;
		PLAYER_SEND_SELF_TEAM_INFO	sSelfInfo;
		sSelfInfo.ProtocolType = s2c_teamselfinfo;
		sSelfInfo.nTeamServerID = m_cTeam.m_nID;
		sSelfInfo.m_dwLeadExp = m_dwLeadExp;
		sSelfInfo.m_btState = g_Team[m_cTeam.m_nID].m_nState;
		// 
		nNpcIndex = Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nIndex;
		sSelfInfo.m_dwNpcID[0] = Npc[nNpcIndex].m_dwID;
		sSelfInfo.m_btLevel[0] = (DWORD)Npc[nNpcIndex].m_Level;
		strcpy(sSelfInfo.m_szNpcName[0], Npc[nNpcIndex].Name);
		// 
		for (int i = 0; i < MAX_TEAM_MEMBER; i++)
		{
			if (g_Team[m_cTeam.m_nID].m_nMember[i] >= 0)
			{
				nNpcIndex = Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nIndex;
				sSelfInfo.m_dwNpcID[i + 1] = Npc[nNpcIndex].m_dwID;
				sSelfInfo.m_btLevel[i + 1] = (DWORD)Npc[nNpcIndex].m_Level;
				strcpy(sSelfInfo.m_szNpcName[i + 1], Npc[nNpcIndex].Name);
			}
			else
			{
				sSelfInfo.m_dwNpcID[i + 1] = 0;
				sSelfInfo.m_btLevel[i + 1] = 0;
			}
		}
		// 
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSelfInfo, sizeof(PLAYER_SEND_SELF_TEAM_INFO));
	}
	else	// 
	{
		PLAYER_LEAVE_TEAM	sLeaveTeam;
		sLeaveTeam.ProtocolType = s2c_teamleave;
		sLeaveTeam.m_dwNpcID = Npc[m_nIndex].m_dwID;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sLeaveTeam, sizeof(PLAYER_LEAVE_TEAM));
	}
}
#endif

#ifdef _SERVER
BOOL	KPlayer::CreateTeam(BYTE* pProtocol)
{
	return m_cTeam.CreateTeam(this->m_nPlayerIndex, (PLAYER_APPLY_CREATE_TEAM*)pProtocol);
}
#endif

#ifdef _SERVER
BOOL	KPlayer::SetTeamState(BYTE* pProtocol)
{
	if (this->CheckTrading())
		return FALSE;
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN || m_cTeam.m_nID < 0)
	{
		SendSelfTeamInfo();
		return FALSE;
	}
	
	PLAYER_TEAM_OPEN_CLOSE	*pTeamState = (PLAYER_TEAM_OPEN_CLOSE*)pProtocol;
	if (pTeamState->m_btOpenClose == 0)
	{
		g_Team[m_cTeam.m_nID].SetTeamClose();
	}
	else
	{
		if (m_cMenuState.m_nState == PLAYER_MENU_STATE_TRADEOPEN)
			m_cMenuState.SetState(m_nPlayerIndex, PLAYER_MENU_STATE_NORMAL);
		g_Team[m_cTeam.m_nID].SetTeamOpen();
	}
	
	return TRUE;
}
#endif

#ifdef _SERVER
BOOL	KPlayer::S2CSendAddTeamInfo(BYTE* pProtocol)
{
	if (!m_cTeam.m_bCanTeamFlag)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_CANNOT_ADD_TEAM;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
	
	PLAYER_APPLY_ADD_TEAM	*pAddTeam = (PLAYER_APPLY_ADD_TEAM*)pProtocol;
	
	// Ñ°ÕÒ¶Ó³¤ npc id µÄ player ±àºÅ
	int		nPlayer, nRegionNo, i;
	nPlayer = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].FindPlayer(pAddTeam->m_dwTarNpcID);
	if ( nPlayer >= 0)
		goto SendMsg;
	for (i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nPlayer = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nRegionNo].FindPlayer(pAddTeam->m_dwTarNpcID);
		if (nPlayer >= 0)
			break;
	}
	if (i >= 8)
		return FALSE;
	
SendMsg:
	if (Npc[Player[nPlayer].m_nIndex].m_Camp == camp_begin && Npc[m_nIndex].m_Camp != camp_begin)
		return FALSE;
	m_cTeam.m_nApplyCaptainID = pAddTeam->m_dwTarNpcID;
	PLAYER_APPLY_ADD_TEAM	sAddTeam;
	sAddTeam.ProtocolType = s2c_teamgetapply;
	sAddTeam.m_dwTarNpcID = Npc[m_nIndex].m_dwID;
	g_pServer->PackDataToClient(Player[nPlayer].m_nNetConnectIdx, (BYTE*)&sAddTeam, sizeof(PLAYER_APPLY_ADD_TEAM));
	return TRUE;
}
#endif

#ifdef _SERVER
BOOL	KPlayer::AddTeamMember(BYTE* pProtocol)
{
	// ´íÎó¼ì²â(ÊÇ·ñ×é¶Ó¡¢ÊÇ·ñ¶Ó³¤¡¢¶ÓÎé¿ª·Å×´Ì¬¡¢¶ÓÔ±Êý¡¢Í³ÂÊÁ¦)
	if (m_cTeam.m_nID<0 || m_cTeam.m_nID >=MAX_PLAYER)
		return FALSE;

	if ( !m_cTeam.m_nFlag ||
		m_cTeam.m_nFigure != TEAM_CAPTAIN ||
		!g_Team[m_cTeam.m_nID].IsOpen() ||
		g_Team[m_cTeam.m_nID].m_nMemNum >= MAX_TEAM_MEMBER ||
		g_Team[m_cTeam.m_nID].m_nMemNum >= g_Team[m_cTeam.m_nID].CalcCaptainPower())
	{
		SendSelfTeamInfo();
		return FALSE;
	}
	
	int		nPlayer, i;
	PLAYER_ACCEPT_TEAM_MEMBER	*pAccept = (PLAYER_ACCEPT_TEAM_MEMBER*)pProtocol;
	nPlayer = FindAroundPlayer(pAccept->m_dwNpcID);
	if (nPlayer < 0)
		return FALSE;
	// 
	if (!Player[nPlayer].m_cTeam.m_bCanTeamFlag)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TARGET_CANNOT_ADD_TEAM;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
	// 
	if (Player[nPlayer].m_cTeam.m_nFlag)
		return FALSE;
	if (Player[nPlayer].m_cTeam.m_nApplyCaptainID != Npc[this->m_nIndex].m_dwID)
		return FALSE;
	// 
	if ( !g_Team[m_cTeam.m_nID].AddMember(nPlayer) )
		return FALSE;
	// 
	if (g_Team[m_cTeam.m_nID].m_nMemNum >= MAX_TEAM_MEMBER || g_Team[m_cTeam.m_nID].CheckFull())
	{
		g_Team[m_cTeam.m_nID].SetTeamClose();
		return FALSE;
	}
	// 
	Player[nPlayer].m_cTeam.Release();
	Player[nPlayer].m_cTeam.m_nFlag = 1;
	Player[nPlayer].m_cTeam.m_nFigure = TEAM_MEMBER;
	Player[nPlayer].m_cTeam.m_nID = m_cTeam.m_nID;
	Npc[Player[nPlayer].m_nIndex].SetCurrentCamp(Npc[m_nIndex].m_Camp);
	//
	PLAYER_TEAM_ADD_MEMBER	sAddMem;
	sAddMem.ProtocolType = s2c_teamaddmember;
	sAddMem.m_dwNpcID = Npc[Player[nPlayer].m_nIndex].m_dwID;
	sAddMem.m_btLevel = (DWORD)Npc[Player[nPlayer].m_nIndex].m_Level;
	strcpy(sAddMem.m_szName, Npc[Player[nPlayer].m_nIndex].Name);
	// 
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sAddMem, sizeof(PLAYER_TEAM_ADD_MEMBER));
	// 
	for (i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if (g_Team[m_cTeam.m_nID].m_nMember[i] < 0 || g_Team[m_cTeam.m_nID].m_nMember[i] == nPlayer)
			continue;
		g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nNetConnectIdx, (BYTE*)&sAddMem, sizeof(PLAYER_TEAM_ADD_MEMBER));
	}
	// ---------------------------------------------------------------
	int		nNpcIndex;
	PLAYER_SEND_SELF_TEAM_INFO	sSelfInfo;
	sSelfInfo.ProtocolType = s2c_teamselfinfo;
	sSelfInfo.nTeamServerID = m_cTeam.m_nID;
	sSelfInfo.m_dwLeadExp = Player[nPlayer].m_dwLeadExp;
	sSelfInfo.m_btState = g_Team[m_cTeam.m_nID].m_nState;
	// 
	nNpcIndex = m_nIndex;
	sSelfInfo.m_dwNpcID[0] = Npc[nNpcIndex].m_dwID;
	sSelfInfo.m_btLevel[0] = (DWORD)Npc[nNpcIndex].m_Level;
	strcpy(sSelfInfo.m_szNpcName[0], Npc[nNpcIndex].Name);
	// 
	for (i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if (g_Team[m_cTeam.m_nID].m_nMember[i] >= 0)
		{
			nNpcIndex = Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nIndex;
			sSelfInfo.m_dwNpcID[i + 1] = Npc[nNpcIndex].m_dwID;
			sSelfInfo.m_btLevel[i + 1] = (DWORD)Npc[nNpcIndex].m_Level;
			strcpy(sSelfInfo.m_szNpcName[i + 1], Npc[nNpcIndex].Name);
		}
		else
		{
			sSelfInfo.m_dwNpcID[i + 1] = 0;
			sSelfInfo.m_btLevel[i + 1] = 0;
		}
	}
	// 
	g_pServer->PackDataToClient(Player[nPlayer].m_nNetConnectIdx, (BYTE*)&sSelfInfo, sizeof(PLAYER_SEND_SELF_TEAM_INFO));
	
	SHOW_MSG_SYNC	sMsg;
	sMsg.ProtocolType = s2c_msgshow;
	sMsg.m_wMsgID = enumMSG_ID_TEAM_SELF_ADD;
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
	sMsg.m_lpBuf = 0;
	g_pServer->PackDataToClient(Player[nPlayer].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
	// ---------------------------------end --------------------------
	
	return TRUE;
}
#endif

#ifdef _SERVER
void	KPlayer::LeaveTeam(BYTE* pProtocol)
{
	if (!m_cTeam.m_nFlag)
		return;
	if (this->m_cTeam.m_nFigure == TEAM_CAPTAIN && g_Team[m_cTeam.m_nID].IsOpen())
	{
		g_Team[m_cTeam.m_nID].SetTeamClose();
	}
	
	if (m_cTeam.m_nFigure != TEAM_CAPTAIN)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TEAM_LEAVE;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
		sMsg.m_lpBuf = (void*)Npc[this->m_nIndex].m_dwID;
		
		g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		for (int i = 0; i < MAX_TEAM_MEMBER; i++)
		{
			if (g_Team[m_cTeam.m_nID].m_nMember[i] > 0)
			{
				g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
			}
		}
		sMsg.m_lpBuf = 0;
	}
	
	g_Team[m_cTeam.m_nID].DeleteMember(GetPlayerIndex());
}
#endif

#ifdef _SERVER
void	KPlayer::TeamKickOne(BYTE* pProtocol)
{
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
		return;
	
	int		nMemNo;
	PLAYER_TEAM_KICK_MEMBER	*pKickOne = (PLAYER_TEAM_KICK_MEMBER*)pProtocol;
	nMemNo = g_Team[m_cTeam.m_nID].FindMemberID(pKickOne->m_dwNpcID);
	if (nMemNo < 0)
		return;
	int		nPlayerNo;
	// 
	nPlayerNo = g_Team[m_cTeam.m_nID].m_nMember[nMemNo];
	g_Team[m_cTeam.m_nID].DeleteMember(nPlayerNo);
	// 
	int		nLength;
	SHOW_MSG_SYNC	sMsg;
	
	nLength = strlen(Npc[Player[nPlayerNo].m_nIndex].Name);
	sMsg.ProtocolType = s2c_msgshow;
	sMsg.m_wMsgID = enumMSG_ID_TEAM_KICK_One;
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID) + nLength;
	sMsg.m_lpBuf = new BYTE[sMsg.m_wLength + 1];
	
	memcpy(sMsg.m_lpBuf, &sMsg, sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID));
	memcpy((char*)sMsg.m_lpBuf + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), Npc[Player[nPlayerNo].m_nIndex].Name, nLength);
	
	g_pServer->PackDataToClient(Player[nPlayerNo].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
	
	g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
	for (int i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if (g_Team[m_cTeam.m_nID].m_nMember[i] > 0)
			g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
	}
	sMsg.Release();
}
#endif

#ifdef _SERVER
void	KPlayer::TeamChangeCaptain(BYTE* pProtocol)
{
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
		return;
	int		i, nMember, nPlayerNo, nIsOpen = 0;
	PLAYER_APPLY_TEAM_CHANGE_CAPTAIN	*pChange = (PLAYER_APPLY_TEAM_CHANGE_CAPTAIN*)pProtocol;
	nMember = g_Team[m_cTeam.m_nID].FindMemberID(pChange->m_dwNpcID);
	if (nMember < 0)
		return;
	if (Npc[Player[g_Team[m_cTeam.m_nID].m_nMember[nMember]].m_nIndex].m_Camp == camp_begin && Npc[this->m_nIndex].m_Camp != camp_begin)
	{
		// ¶Ó³¤ÒÆ½»Ê§°Ü£º¶Ô·½Í³Ë§Á¦²»¹»
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL2;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
		sMsg.m_lpBuf = (LPVOID)pChange->m_dwNpcID;
		g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		sMsg.m_lpBuf = 0;
		return;
	}
	if (g_Team[m_cTeam.m_nID].m_nMemNum > PlayerSet.m_cLeadExp.GetMemNumFromLevel(Player[g_Team[m_cTeam.m_nID].m_nMember[nMember]].m_dwLeadLevel))
	{
		// ¶Ó³¤ÒÆ½»Ê§°Ü£º¶Ô·½Í³Ë§Á¦²»¹»
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
		sMsg.m_lpBuf = (LPVOID)pChange->m_dwNpcID;
		g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		sMsg.m_lpBuf = 0;
		return;
	}
	if (g_Team[m_cTeam.m_nID].IsOpen())
	{
		g_Team[m_cTeam.m_nID].SetTeamClose();
		nIsOpen = 1;
	}
	nPlayerNo = g_Team[m_cTeam.m_nID].m_nCaptain;
	g_Team[m_cTeam.m_nID].m_nCaptain = g_Team[m_cTeam.m_nID].m_nMember[nMember];
	g_Team[m_cTeam.m_nID].m_nMember[nMember] = nPlayerNo;
	m_cTeam.m_nFigure = TEAM_MEMBER;
	Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_cTeam.m_nFigure = TEAM_CAPTAIN;
	// ¸Ä±äÕóÓª
	Npc[Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nIndex].RestoreCurrentCamp();
	for (i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		nPlayerNo = g_Team[m_cTeam.m_nID].m_nMember[i];
		if (nPlayerNo < 0)
			continue;
		Npc[Player[nPlayerNo].m_nIndex].SetCurrentCamp(Npc[Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nIndex].m_Camp);
	}
	
	// ¸ø¸÷¸ö¶ÓÔ±·¢ÏûÏ¢
	PLAYER_TEAM_CHANGE_CAPTAIN	sChangeCaptain;
	sChangeCaptain.ProtocolType = s2c_teamchangecaptain;
	sChangeCaptain.m_dwCaptainID = pChange->m_dwNpcID;
	sChangeCaptain.m_dwMemberID = Npc[m_nIndex].m_dwID;
	// ¸ø¶Ó³¤·¢ÏûÏ¢
	g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nNetConnectIdx, (BYTE*)&sChangeCaptain, sizeof(PLAYER_TEAM_CHANGE_CAPTAIN));
	// ¸ø¶ÓÔ±·¢ÏûÏ¢
	for (i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		nPlayerNo = g_Team[m_cTeam.m_nID].m_nMember[i];
		if (nPlayerNo < 0)
			continue;
		g_pServer->PackDataToClient(Player[nPlayerNo].m_nNetConnectIdx, (BYTE*)&sChangeCaptain, sizeof(PLAYER_TEAM_CHANGE_CAPTAIN));
	}
	
	if (nIsOpen)
	{
		g_Team[m_cTeam.m_nID].SetTeamOpen();
	}
}
#endif

#ifdef _SERVER
void	KPlayer::TeamDismiss(BYTE* pProtocol)
{
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
		return;
	
	PLAYER_LEAVE_TEAM	sLeaveTeam;
	int		i;
	SHOW_MSG_SYNC	sMsg;
	// 
	sMsg.ProtocolType = s2c_msgshow;
	sMsg.m_wMsgID = enumMSG_ID_TEAM_DISMISS;
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
	g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
	// 
	sLeaveTeam.ProtocolType = s2c_teamleave;
	sLeaveTeam.m_dwNpcID = Npc[m_nIndex].m_dwID;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sLeaveTeam, sizeof(PLAYER_LEAVE_TEAM));
	// 
	for (i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		
		sLeaveTeam.m_dwNpcID = Npc[Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nIndex].m_dwID;
		g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nNetConnectIdx, (BYTE*)&sLeaveTeam, sizeof(PLAYER_LEAVE_TEAM));
		// 
		Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_cTeam.m_nFlag = 0;
		Npc[Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nIndex].RestoreCurrentCamp();;
	}
	// 
	g_Team[m_cTeam.m_nID].Release();
	// 
	m_cTeam.m_nFlag = 0;
	Npc[m_nIndex].RestoreCurrentCamp();
}
#endif

#ifdef _SERVER
void	KPlayer::SetPK(BYTE* pProtocol)
{
	PLAYER_SET_PK	*pPK = (PLAYER_SET_PK*)pProtocol;
	
	if (pPK->m_btPKFlag)
		Npc[m_nIndex].SetCurrentCamp(camp_free);
	else
		Npc[m_nIndex].RestoreCurrentCamp();
}

void	KPlayer::SendFactionData(BYTE* pProtocol)
{
	SendFactionData();
}

void	KPlayer::SendFactionData()
{
	// ¸ø¿Í»§¶Ë·¢ÏûÏ¢
	PLAYER_FACTION_DATA	sData;
	sData.ProtocolType = s2c_playerfactiondata;
	sData.m_btCamp = Npc[m_nIndex].m_Camp;
	sData.m_btCurFaction = m_cFaction.m_nCurFaction;
	sData.m_btFirstFaction = m_cFaction.m_nFirstAddFaction;
	sData.m_nAddTimes = m_cFaction.m_nAddTimes;
	
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sData, sizeof(PLAYER_FACTION_DATA));
}
#endif

#ifdef _SERVER
// not end
void KPlayer::GetAboutPos(KMapPos *pMapPos)
{
	if (m_nIndex <= 0)
		return;
	
	if (Npc[m_nIndex].m_SubWorldIndex < 0)
		return;
	
	POINT Pos;
	int nX, nY;
	Npc[m_nIndex].GetMpsPos(&nX, &nY);
	Pos.x = nX;
	Pos.y = nY;
	
	SubWorld[Npc[m_nIndex].m_SubWorldIndex].GetFreeObjPos(Pos);
	
	pMapPos->nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	SubWorld[Npc[m_nIndex].m_SubWorldIndex].Mps2Map(
		Pos.x, 
		Pos.y, 
		&pMapPos->nRegion, 
		&pMapPos->nMapX, 
		&pMapPos->nMapY, 
		&pMapPos->nOffX, 
		&pMapPos->nOffY);
}

/*
void	KPlayer::GetAboutPos(KMapPos *pMapPos)
{
POINT	Pos[8] = 
{
{0, 32}, {-32, 32}, {-32, 0}, {-32, -32},
{0, -32}, {32, -32}, {32, 0}, {32, 32},
};

  int nMpsX, nMpsY, nTmpX, nTmpY;
  int nR, nMapX, nMapY, nOffX, nOffY;
  Npc[m_nIndex].GetMpsPos(&nMpsX, &nMpsY);
  
	for (int i = 0; i < 8; i++)
	{
	nTmpX = nMpsX + Pos[i].x;
	nTmpY = nMpsY + Pos[i].y;
	if (SubWorld[Npc[m_nIndex].m_SubWorldIndex].GetBarrier(nTmpX, nTmpY))
	continue;
	SubWorld[Npc[m_nIndex].m_SubWorldIndex].Mps2Map(nTmpX, nTmpY, &nR, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nR == -1)
	continue;
	if (SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nR].GetRef(nMapX, nMapY, obj_object))
	continue;
	else
	break;
	}
	
	  if (i == 8)
	  {
	  pMapPos->nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	  pMapPos->nRegion = Npc[m_nIndex].m_RegionIndex;
	  pMapPos->nMapX = Npc[m_nIndex].m_MapX;
	  pMapPos->nMapY = Npc[m_nIndex].m_MapY;
	  pMapPos->nOffX = Npc[m_nIndex].m_OffX;
	  pMapPos->nOffY = Npc[m_nIndex].m_OffY;
	  }
	  else
	  {
	  pMapPos->nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	  pMapPos->nRegion = nR;
	  pMapPos->nMapX = nMapX;
	  pMapPos->nMapY = nMapY;
	  pMapPos->nOffX = nOffX;
	  pMapPos->nOffY = nOffY;
	  }
}*/
#endif

#ifdef _SERVER
int		KPlayer::FindAroundPlayer(DWORD dwNpcID)
{
	if (dwNpcID == 0)
		return -1;
	
	int		nPlayer, nRegionNo, i;
	nPlayer = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].FindPlayer(dwNpcID);
	if ( nPlayer >= 0)
		return nPlayer;
	for (i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nPlayer = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nRegionNo].FindPlayer(dwNpcID);
		if (nPlayer >= 0)
			return nPlayer;
	}
	
	return -1;
}

BOOL	KPlayer::CheckPlayerAround(int nPlayerIdx)
{
	if (nPlayerIdx <= 0)
		return FALSE;
	if (SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].CheckPlayerIn(nPlayerIdx))
		return TRUE;
	int		nRegionNo;
	for (int i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		if (SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nRegionNo].CheckPlayerIn(nPlayerIdx))
			return TRUE;
	}
	
	return FALSE;
}

int		KPlayer::FindAroundNpc(DWORD dwNpcID)
{
	if (dwNpcID == 0)
		return 0;
	
	int		nNpc, nRegionNo, i;
	if (m_nIndex > 0 && m_nIndex < MAX_NPC && (Npc[m_nIndex].m_SubWorldIndex < 0 || Npc[m_nIndex].m_RegionIndex < 0))
		AUTOLOG_IDX(m_nIndex, "[S3-FIND-BADRGN] npc=%d tgtid=%u sw=%d rgn=%d", m_nIndex, dwNpcID, Npc[m_nIndex].m_SubWorldIndex, Npc[m_nIndex].m_RegionIndex);
	nNpc = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].SearchNpc(dwNpcID);
	if ( nNpc > 0)
		return nNpc;
	for (i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nNpc = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nRegionNo].SearchNpc(dwNpcID);
		if (nNpc > 0)
			return nNpc;
	}
	
	return 0;
}
#endif

BOOL	KPlayer::NewPlayerGetBaseAttribute(int Series)
{
	if (Series < series_metal || Series > series_earth)
		return FALSE;
	
	Npc[m_nIndex].SetSeries(Series);
	
	m_nStrength = PlayerSet.m_cNewPlayerAttribute.m_nStrength[Series];
	m_nDexterity = PlayerSet.m_cNewPlayerAttribute.m_nDexterity[Series];
	m_nVitality = PlayerSet.m_cNewPlayerAttribute.m_nVitality[Series];
	m_nEngergy = PlayerSet.m_cNewPlayerAttribute.m_nEngergy[Series];
	m_nLucky = PlayerSet.m_cNewPlayerAttribute.m_nLucky[Series];
	m_nUpExp = 0;
	m_nCurStrength = m_nStrength;
	m_nCurDexterity = m_nDexterity;
	m_nCurVitality = m_nVitality;
	m_nCurEngergy = m_nEngergy;
	CalcCurLucky();
	
	m_cFaction.SetSeries(Series);
	
	return TRUE;
}

void	KPlayer::CalcCurLucky()
{	
	m_nCurLucky = m_nLucky;
}

#ifdef _SERVER
void  KPlayer::SyncCurrentBaseAttriibute(int type,int attribute,int curAttribute)
{	
	PLAYER_ATTRIBUTE_SYNC	sSync;
	sSync.ProtocolType = s2c_playersyncattribute;
	sSync.m_btAttribute = type;
	sSync.m_nBasePoint = attribute;
	sSync.m_nCurPoint = curAttribute;
	sSync.m_nLeavePoint = m_nAttributePoint;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));
}

void	KPlayer::AddBaseStrength(int nData)
{
	if (nData > 0 && nData <= m_nAttributePoint)
	{
		m_nStrength += nData;
		m_nAttributePoint -= nData;
		m_nCurStrength += nData;
		
		UpdataCurData();
		SetNpcPhysicsDamage();
		// 
		PLAYER_ATTRIBUTE_SYNC	sSync;
		sSync.ProtocolType = s2c_playersyncattribute;
		sSync.m_btAttribute = 0;
		sSync.m_nBasePoint = m_nStrength;
		sSync.m_nCurPoint = m_nCurStrength;
		sSync.m_nLeavePoint = m_nAttributePoint;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));
	}
}

void	KPlayer::ResetBaseStrength(int nData)
{		
	m_nStrength = nData;	
	m_nCurStrength = nData;
	UpdataCurData();
	SetNpcPhysicsDamage();
	SyncCurrentBaseAttriibute(0,m_nStrength,m_nCurStrength);	
}
#endif

#ifdef _SERVER
void	KPlayer::AddBaseDexterity(int nData)
{
	if (nData > 0 && nData <= m_nAttributePoint)
	{
		m_nDexterity += nData;
		m_nAttributePoint -= nData;
		m_nCurDexterity += nData;
		
		SetNpcAttackRating();
		SetNpcDefence();
		UpdataCurData();
		SetNpcPhysicsDamage();
		
		// 
		PLAYER_ATTRIBUTE_SYNC	sSync;
		sSync.ProtocolType = s2c_playersyncattribute;
		sSync.m_btAttribute = 1;
		sSync.m_nBasePoint = m_nDexterity;
		sSync.m_nCurPoint = m_nCurDexterity;
		sSync.m_nLeavePoint = m_nAttributePoint;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));
	}
}

void	KPlayer::ResetBaseDexterity(int nData)
{
		m_nDexterity = nData;		
		m_nCurDexterity = nData;

		SetNpcAttackRating();
		SetNpcDefence();
		UpdataCurData();
		SetNpcPhysicsDamage();

		// 
		SyncCurrentBaseAttriibute(1,m_nDexterity,m_nCurDexterity);
}
#endif

#ifdef _SERVER
void	KPlayer::AddBaseVitality(int nData)
{
	if (nData > 0 && nData <= m_nAttributePoint)
	{
		m_nVitality += nData;
		m_nAttributePoint -= nData;
		m_nCurVitality += nData;
		
		Npc[m_nIndex].AddBaseLifeMax(PlayerSet.m_cLevelAdd.GetLifePerVitality(Npc[m_nIndex].m_Series) * nData);
		Npc[m_nIndex].AddBaseStaminaMax(PlayerSet.m_cLevelAdd.GetStaminaPerVitality(Npc[m_nIndex].m_Series) * nData);
		UpdataCurData();
		
		// 
		PLAYER_ATTRIBUTE_SYNC	sSync;
		sSync.ProtocolType = s2c_playersyncattribute;
		sSync.m_btAttribute = 2;
		sSync.m_nBasePoint = m_nVitality;
		sSync.m_nCurPoint = m_nCurVitality;
		sSync.m_nLeavePoint = m_nAttributePoint;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));
	}
}
void	KPlayer::ResetBaseVitality(int nData)
{	
	m_nVitality = nData;		
	m_nCurVitality = nData;

	Npc[m_nIndex].SetBaseLifeMax(PlayerSet.m_cLevelAdd.GetLifePerVitality(Npc[m_nIndex].m_Series) * nData);
	Npc[m_nIndex].SetBaseStaminaMax(PlayerSet.m_cLevelAdd.GetStaminaPerVitality(Npc[m_nIndex].m_Series) * nData);
	UpdataCurData();

	// 
	SyncCurrentBaseAttriibute(2,m_nVitality,m_nCurVitality);
}
#endif

#ifdef _SERVER
void	KPlayer::AddBaseEngergy(int nData)
{
	if (nData > 0 && nData <= m_nAttributePoint)
	{
		m_nEngergy += nData;
		m_nAttributePoint -= nData;
		m_nCurEngergy += nData;
		
		Npc[m_nIndex].AddBaseManaMax(PlayerSet.m_cLevelAdd.GetManaPerEnergy(Npc[m_nIndex].m_Series) * nData);
		UpdataCurData();
		
		// 
		PLAYER_ATTRIBUTE_SYNC	sSync;
		sSync.ProtocolType = s2c_playersyncattribute;
		sSync.m_btAttribute = 3;
		sSync.m_nBasePoint = m_nEngergy;
		sSync.m_nCurPoint = m_nCurEngergy;
		sSync.m_nLeavePoint = m_nAttributePoint;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));
	}
}
void	KPlayer::ResetBaseEngergy(int nData)
{
	m_nEngergy = nData;		
	m_nCurEngergy = nData;

	Npc[m_nIndex].SetBaseManaMax(PlayerSet.m_cLevelAdd.GetManaPerEnergy(Npc[m_nIndex].m_Series) * nData);
	UpdataCurData();

	// 
	SyncCurrentBaseAttriibute(3,m_nEngergy,m_nCurEngergy);
}
#endif

void	KPlayer::ChangeCurStrength(int nData)
{
	/*	if (m_nCurStrength + nData < m_nStrength)
	return;
	*/
	m_nCurStrength += nData;
	SetNpcPhysicsDamage();
	// 
}

void	KPlayer::ChangeCurDexterity(int nData)
{
	/*	if (m_nCurDexterity + nData < 0)
	return;
	*/
	m_nCurDexterity += nData;
	
	int nRating = nData * 4;
	Npc[m_nIndex].m_CurrentAttackRating += nRating;
	
	int nDefence = nData / 4;
	Npc[m_nIndex].m_CurrentDefend += nDefence;
	
	SetNpcPhysicsDamage();
	
}

void	KPlayer::ChangeCurVitality(int nData)
{
	/*	if (nData + m_nAttributePoint < 0)
	return;
	*/
	m_nCurVitality += nData;
	
	Npc[m_nIndex].AddCurLifeMax(PlayerSet.m_cLevelAdd.GetLifePerVitality(Npc[m_nIndex].m_Series) * nData);
	Npc[m_nIndex].AddCurStaminaMax(PlayerSet.m_cLevelAdd.GetStaminaPerVitality(Npc[m_nIndex].m_Series) * nData);
	
}

void	KPlayer::ChangeCurEngergy(int nData)
{
	/*	if (m_nAttributePoint + nData < 0)
			return;
	*/
	m_nCurEngergy += nData;
	
	Npc[m_nIndex].AddCurManaMax(PlayerSet.m_cLevelAdd.GetManaPerEnergy(Npc[m_nIndex].m_Series) * nData);
	
}

void	KPlayer::AddBaseLucky(int nData)
{
	m_nLucky += nData;
	CalcCurLucky();
}

void	KPlayer::SetNpcPhysicsDamage()
{
	int		nMinDamage, nMaxDamage;
	m_ItemList.GetWeaponDamage(&nMinDamage, &nMaxDamage);
	
	if (m_ItemList.GetWeaponType() == equip_meleeweapon)	// (µ±Ç°×°±¸ÊÇ½üÉíÎäÆ÷)
	{
	/*		nMinDamage = nMinDamage * (m_nCurStrength + STRENGTH_SET_DAMAGE_VALUE) / STRENGTH_SET_DAMAGE_VALUE;
	nMaxDamage = nMaxDamage * (m_nCurStrength + STRENGTH_SET_DAMAGE_VALUE) / STRENGTH_SET_DAMAGE_VALUE;
		*/
		//  by Spe 03/06/11
		nMinDamage += m_nCurStrength / STRENGTH_SET_DAMAGE_VALUE;
		nMaxDamage += m_nCurStrength / STRENGTH_SET_DAMAGE_VALUE;
		Npc[m_nIndex].SetPhysicsDamage(nMinDamage, nMaxDamage);
	}
	else if (m_ItemList.GetWeaponType() == equip_rangeweapon)	// (µ±Ç°×°±¸ÊÇÔ¶³ÌÎäÆ÷)
	{
	/*		nMinDamage = nMinDamage * (m_nCurDexterity + DEXTERITY_SET_DAMAGE_VALUE) / DEXTERITY_SET_DAMAGE_VALUE;
	nMaxDamage = nMaxDamage * (m_nCurDexterity + DEXTERITY_SET_DAMAGE_VALUE) / DEXTERITY_SET_DAMAGE_VALUE;
		*/
		//  by Spe 03/06/11
		nMinDamage += m_nCurDexterity / DEXTERITY_SET_DAMAGE_VALUE;
		nMaxDamage += m_nCurDexterity / DEXTERITY_SET_DAMAGE_VALUE;
		Npc[m_nIndex].SetPhysicsDamage(nMinDamage, nMaxDamage);
	}
	else
	{
		Npc[m_nIndex].SetPhysicsDamage(nMinDamage, nMaxDamage);
	}
}

void	KPlayer::SetNpcAttackRating()
{
	int		nRating;
	
	nRating = m_nDexterity * 4 - 28;
	Npc[m_nIndex].SetBaseAttackRating(nRating);
}

void	KPlayer::SetNpcDefence()
{
	int		nDefence;
	
	nDefence = m_nDexterity / 4;
	Npc[m_nIndex].SetBaseDefence(nDefence);
}

void	KPlayer::AddExp(int nExp, int nTarLevel, BOOL bCheck)
{
#ifndef _SERVER
	AddSelfExp(nExp, nTarLevel);
#endif
	
#ifdef _SERVER
	// Ã»ÓÐ×é¶Ó
	if ( !m_cTeam.m_nFlag )
	{
		AddSelfExp(nExp, nTarLevel);
		return;
	}
	
	// ÒÑ¾­×é¶Ó
	int		i, j, nShareFlag[MAX_TEAM_MEMBER + 1], nRegion, nTotalLevel = 0, nTotalPlayer = 0;
	memset(nShareFlag, 0, sizeof(nShareFlag));
	
	int		nMpsX, nMpsY, nNpcIdx, nSelfSubIdx, nSelfMpsX, nSelfMpsY;
	
	nSelfSubIdx = Npc[m_nIndex].m_SubWorldIndex;
	SubWorld[nSelfSubIdx].Map2Mps(
		Npc[m_nIndex].m_RegionIndex,
		Npc[m_nIndex].m_MapX,
		Npc[m_nIndex].m_MapY,
		Npc[m_nIndex].m_OffX,
		Npc[m_nIndex].m_OffY,
		&nSelfMpsX,
		&nSelfMpsY);
	
	// ¶Ó³¤
	nNpcIdx = Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nIndex;
	if (nSelfSubIdx == Npc[nNpcIdx].m_SubWorldIndex)
	{
		SubWorld[Npc[nNpcIdx].m_SubWorldIndex].Map2Mps(
			Npc[nNpcIdx].m_RegionIndex,
			Npc[nNpcIdx].m_MapX,
			Npc[nNpcIdx].m_MapY,
			Npc[nNpcIdx].m_OffX,
			Npc[nNpcIdx].m_OffY,
			&nMpsX,
			&nMpsY);
		if ( (nMpsX - nSelfMpsX) * (nMpsX - nSelfMpsX) + 
			(nMpsY - nSelfMpsY) * (nMpsY - nSelfMpsY) <= 
			PLAYER_SHARE_EXP_DISTANCE * PLAYER_SHARE_EXP_DISTANCE)
		{
			nTotalLevel += Npc[nNpcIdx].m_Level;
			nShareFlag[0] = 1;
			nTotalPlayer++;
			if (nTotalPlayer >= g_Team[m_cTeam.m_nID].m_nMemNum + 1)
				goto SharePlace;
		}
	}
	// ¶ÓÔ±
	for (i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if (g_Team[m_cTeam.m_nID].m_nMember[i] < 0)
			continue;
		nNpcIdx = Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nIndex;
		if (nSelfSubIdx != Npc[nNpcIdx].m_SubWorldIndex)
			continue;
		SubWorld[Npc[nNpcIdx].m_SubWorldIndex].Map2Mps(
			Npc[nNpcIdx].m_RegionIndex,
			Npc[nNpcIdx].m_MapX,
			Npc[nNpcIdx].m_MapY,
			Npc[nNpcIdx].m_OffX,
			Npc[nNpcIdx].m_OffY,
			&nMpsX,
			&nMpsY);
		if ( (nMpsX - nSelfMpsX) * (nMpsX - nSelfMpsX) + 
			(nMpsY - nSelfMpsY) * (nMpsY - nSelfMpsY) > 
			PLAYER_SHARE_EXP_DISTANCE * PLAYER_SHARE_EXP_DISTANCE)
			continue;
		nTotalLevel += Npc[nNpcIdx].m_Level;
		nShareFlag[i + 1] = 1;
		nTotalPlayer++;
		if (nTotalPlayer >= g_Team[m_cTeam.m_nID].m_nMemNum + 1)
			goto SharePlace;
	}
	
SharePlace:
	_ASSERT(nTotalPlayer >= 1);
	_ASSERT(nTotalLevel >= 1);
	if (nTotalPlayer < 1 || nTotalLevel < 1)
		return;
	if (nTotalPlayer == 1)
	{
		AddSelfExp(nExp, nTarLevel);
		return;
	}
	int		k, nGetExp;
	k = 100 + (nTotalPlayer - 1) * 15;

	int ShareExp;
	nTotalPlayer = g_Team[m_cTeam.m_nID].m_nMemNum + 1;
	if (nTotalPlayer == 2)
		ShareExp = 80;
	else if (nTotalPlayer == 3)
		ShareExp = 70;
	else if (nTotalPlayer == 4)
		ShareExp = 60;
	else if (nTotalPlayer == 5)
		ShareExp = 55;
	else if (nTotalPlayer == 6)
		ShareExp = 55;
	else if (nTotalPlayer == 7)
		ShareExp = 50;
	else if (nTotalPlayer == 8)
		ShareExp = 50;
	else
		ShareExp = 60;
	
	if (nShareFlag[0]) //nguoi dau tien nShareFlag doi truong
	{
		nGetExp = k * Npc[Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nIndex].m_Level / nTotalLevel;
		if (nGetExp < 5)
			nGetExp = 5;
		
		if (m_nPlayerIndex != g_Team[m_cTeam.m_nID].m_nCaptain || bCheck)
			Player[g_Team[m_cTeam.m_nID].m_nCaptain].AddSelfExp(nExp * ShareExp / 100, nTarLevel);
		else
			Player[g_Team[m_cTeam.m_nID].m_nCaptain].AddSelfExp(nExp, nTarLevel);

		int		nLeadExp = nExp * k * PlayerSet.m_cLevelAdd.GetLeadExpShare(Npc[m_nIndex].m_Series) * (nTotalLevel - Npc[m_nIndex].m_Level) / (nTotalLevel * 10000);
		Player[g_Team[m_cTeam.m_nID].m_nCaptain].AddLeadExp(nLeadExp);
	}

	for (i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if (nShareFlag[i + 1] != 1)
			continue;

		nGetExp = k * Npc[Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nIndex].m_Level / nTotalLevel;
		if (nGetExp < 5)
			nGetExp = 5;

		if (m_nPlayerIndex != g_Team[m_cTeam.m_nID].m_nMember[i] || bCheck)
			Player[g_Team[m_cTeam.m_nID].m_nMember[i]].AddSelfExp(nExp * ShareExp / 100, nTarLevel);
		else
			Player[g_Team[m_cTeam.m_nID].m_nMember[i]].AddSelfExp(nExp, nTarLevel);
	}
	
#endif
}

void	KPlayer::AddSelfExp(int nExp, int nTarLevel)
{
	if (nExp <= 0)
		return;
	if (Npc[m_nIndex].m_Level >= MAX_LEVEL)
		return;
	if (Npc[m_nIndex].m_Doing == do_death || Npc[m_nIndex].m_Doing == do_revive)
		return;

	int	nGetExp = 0;
    
    int nxExpVip = 0;
    if (Npc[m_nIndex].m_CurrentExpSkillsVip == 2)
		nxExpVip = 20;
	else
		nxExpVip = 0;
	
	int Map = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID;
	int nxExpTanThu;
	int level = Npc[m_nIndex].m_Level;	
    if (level < 50)
	    nxExpTanThu = 80 + nxExpVip;
	else if (level < 80)
	    nxExpTanThu = 70 + nxExpVip;
	else if (level < 140)
	    nxExpTanThu = 280 + nxExpVip;
	else
	    nxExpTanThu = 100 + nxExpVip;
	    
	if (m_cReBorn.GetReBornNum() > 3 && Map == 341)
	{
	    if (level < 140)
	        nxExpTanThu = 160 + nxExpVip;
	    else
	        nxExpTanThu = 50 + nxExpVip;
	}
//	else if (Npc[m_nIndex].m_Level < 150)
//		nxExpTanThu = 100;
	int	nSubLevel = Npc[m_nIndex].m_Level - nTarLevel;
	if(Npc[m_nIndex].m_Level >= nTarLevel )	// 
	{
		if (nSubLevel <= 9)
			nGetExp = nExp;
		else
			nGetExp = nExp /10;
	}
	else														
	{
		if (nSubLevel < 0)   
			nGetExp = 1;
	}
	if (nTarLevel >= 90 && Npc[m_nIndex].m_Level >= 90)
	{
		nGetExp = nExp;
	}

	if (nGetExp <= 0)
		nGetExp = 1;

	if(Npc[m_nIndex].m_CurrentExpEnhance > 0)
	{
		if(Npc[m_nIndex].m_CurrentExpEnhance <= MAX_PERCENT)
			nGetExp = nGetExp * (Npc[m_nIndex].m_CurrentExpEnhance + MAX_PERCENT) / MAX_PERCENT;
		else
			nGetExp = nGetExp * Npc[m_nIndex].m_CurrentExpEnhance / MAX_PERCENT;
	}

  
#ifdef _SERVER
	if (nGetExp <= 0)
		nGetExp = 1;

	if(g_ExpRate) //#x2 exp trong config
	{
		nGetExp = nGetExp * g_ExpRate * nxExpTanThu;	
		m_nExp += nGetExp;
	}
	else
	{
		m_nExp += nGetExp;
	}

	g_DebugLog("[Exp]%s nhan duoc %d exp", Npc[m_nIndex].Name, nGetExp);

	if (m_nExp >= m_nNextLevelExp)	
	{
		LevelUp();
	}
	else								
	{
		PLAYER_EXP_SYNC	sSyncExp;
		sSyncExp.ProtocolType = (BYTE)s2c_playerexp;
		sSyncExp.m_nExp = m_nExp;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSyncExp, sizeof(PLAYER_EXP_SYNC));
	}
#endif
}

void	KPlayer::DirectAddExp(double nExp)
{
	if (nExp > 0 && Npc[m_nIndex].m_Level >= MAX_LEVEL)
		return;
	
	m_nExp += nExp;
	if (m_nExp < 0)
	{
		if (-m_nExp > m_nNextLevelExp)
			m_nExp = -m_nNextLevelExp;
	}
#ifdef _SERVER
	if (m_nExp >= m_nNextLevelExp)	
	{
		LevelUp();
	}
	else								
	{
		PLAYER_EXP_SYNC	sSyncExp;
		sSyncExp.ProtocolType = (BYTE)s2c_playerexp;
		sSyncExp.m_nExp = m_nExp;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSyncExp, sizeof(PLAYER_EXP_SYNC));
	}
#endif
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void	KPlayer::LevelUp()
{
	m_nExp = 0;

	if (Npc[m_nIndex].m_Level >= MAX_LEVEL)
		return;
	
	Npc[m_nIndex].m_Level++;
	
	m_nNextLevelExp = PlayerSet.m_cLevelAdd.GetLevelExp(Npc[m_nIndex].m_Level);
	// -------------------------- µÈ¼¶Ôö¼ÓÊ±¸÷ÏîÊôÐÔÔö¼Ó ----------------------
	// Ôö¼ÓÎ´·ÖÅäÊôÐÔµã
	m_nAttributePoint += PLAYER_LEVEL_ADD_ATTRIBUTE;
	// Ôö¼ÓÎ´·ÖÅä¼¼ÄÜµã
	m_nSkillPoint += PLAYER_LEVEL_ADD_SKILL;
	
	// ´¦ÀíÉúÃü¡¢ÌåÁ¦¡¢ÄÚÁ¦Öµ£¨Ó°Ïì»ù±¾ÉúÃü¡¢ÌåÁ¦¡¢ÄÚÁ¦×î´óÖµµÄÒòËØ£ºµÈ¼¶¡¢ÊôÐÔµã£¬Ó°Ïìµ±Ç°ÉúÃü×î´óÖµµÄÒòËØ£º×°±¸¡¢¼¼ÄÜ¡¢Ò©Îï£¨ÁÙÊ±£©£©
	LevelAddBaseLifeMax();
	LevelAddBaseStaminaMax();
	LevelAddBaseManaMax();
	//	Npc[m_nIndex].ResetLifeReplenish();
	
	// ´¦Àí¸÷ÖÖ¿¹ÐÔµÄ±ä»¯ »ð¡¢±ù¡¢¶¾¡¢µç¡¢ÎïÀí
	Npc[m_nIndex].m_FireResist				= PlayerSet.m_cLevelAdd.GetFireResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentFireResist		= Npc[m_nIndex].m_FireResist;
	Npc[m_nIndex].m_ColdResist				= PlayerSet.m_cLevelAdd.GetColdResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentColdResist		= Npc[m_nIndex].m_ColdResist;
	Npc[m_nIndex].m_PoisonResist			= PlayerSet.m_cLevelAdd.GetPoisonResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentPoisonResist		= Npc[m_nIndex].m_PoisonResist;
	Npc[m_nIndex].m_LightResist				= PlayerSet.m_cLevelAdd.GetLightResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentLightResist		= Npc[m_nIndex].m_LightResist;
	Npc[m_nIndex].m_PhysicsResist			= PlayerSet.m_cLevelAdd.GetPhysicsResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentPhysicsResist	= Npc[m_nIndex].m_PhysicsResist;
	Npc[m_nIndex].m_FireResistMax			= BASE_FIRE_RESIST_MAX;
	Npc[m_nIndex].m_ColdResistMax			= BASE_COLD_RESIST_MAX;
	Npc[m_nIndex].m_PoisonResistMax			= BASE_POISON_RESIST_MAX;
	Npc[m_nIndex].m_LightResistMax			= BASE_LIGHT_RESIST_MAX;
	Npc[m_nIndex].m_PhysicsResistMax		= BASE_PHYSICS_RESIST_MAX;
	Npc[m_nIndex].m_CurrentFireResistMax	= Npc[m_nIndex].m_FireResistMax;
	Npc[m_nIndex].m_CurrentColdResistMax	= Npc[m_nIndex].m_ColdResistMax;
	Npc[m_nIndex].m_CurrentPoisonResistMax	= Npc[m_nIndex].m_PoisonResistMax;
	Npc[m_nIndex].m_CurrentLightResistMax	= Npc[m_nIndex].m_LightResistMax;
	Npc[m_nIndex].m_CurrentPhysicsResistMax	= Npc[m_nIndex].m_PhysicsResistMax;

	int nOldCurCamp = Npc[m_nIndex].m_CurrentCamp;
	// ¸ù¾Ý×°±¸¡¢¼¼ÄÜÐÅÏ¢¸üÐÂµ±Ç°Êý¾Ý
	this->UpdataCurData();
	SetNpcPhysicsDamage();
	Npc[m_nIndex].m_CurrentCamp = nOldCurCamp;
	
	// ÉúÃü¡¢ÌåÁ¦¡¢ÄÚÁ¦Öµ¼ÓÂú
	Npc[m_nIndex].m_CurrentLife = Npc[m_nIndex].m_CurrentLifeMax;
	Npc[m_nIndex].m_CurrentStamina = Npc[m_nIndex].m_CurrentStaminaMax;
	Npc[m_nIndex].m_CurrentMana = Npc[m_nIndex].m_CurrentManaMax;
	
#ifdef _SERVER
	// ¸ø¿Í»§¶Ë·¢ËÍÊý¾Ý
	// ×Ô¼ºµÄÊý¾Ý
	PLAYER_LEVEL_UP_SYNC	sLevelUp;
	sLevelUp.ProtocolType		= s2c_playerlevelup;
	sLevelUp.m_btLevel			= (DWORD)Npc[m_nIndex].m_Level;
	sLevelUp.m_nExp				= m_nExp;
	sLevelUp.m_nAttributePoint	= m_nAttributePoint;
	sLevelUp.m_nSkillPoint		= m_nSkillPoint;
	sLevelUp.m_nBaseLifeMax		= Npc[m_nIndex].m_LifeMax;
	sLevelUp.m_nBaseStaminaMax	= Npc[m_nIndex].m_StaminaMax;
	sLevelUp.m_nBaseManaMax		= Npc[m_nIndex].m_ManaMax;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sLevelUp, sizeof(PLAYER_LEVEL_UP_SYNC));
	Npc[m_nIndex].UpdateGameTitle();
	// ¸ø¶ÓÓÑ·¢ËÍµÈ¼¶Êý¾Ý
	if ( m_cTeam.m_nFlag )
	{
		PLAYER_TEAMMATE_LEVEL_SYNC	sLevel;
		sLevel.ProtocolType = s2c_teammatelevel;
		sLevel.m_btLevel = (DWORD)Npc[m_nIndex].m_Level;
		sLevel.m_dwTeammateID = Npc[m_nIndex].m_dwID;
		// ¸ø¶Ó³¤·¢
		if (m_cTeam.m_nFigure != TEAM_CAPTAIN)
			g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nNetConnectIdx, (BYTE*)&sLevel, sizeof(PLAYER_TEAMMATE_LEVEL_SYNC));
		for (int i = 0; i < MAX_TEAM_MEMBER; i++)
		{
			if (g_Team[m_cTeam.m_nID].m_nMember[i] >= 0 && g_Team[m_cTeam.m_nID].m_nMember[i] != GetPlayerIndex())
				g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nNetConnectIdx, (BYTE*)&sLevel, sizeof(PLAYER_TEAMMATE_LEVEL_SYNC));
		}
	}
#endif
}

#ifdef _SERVER
// [TONG 21/08] AddTempMagic - cong don theo id; ve 0 thi xoa ban ghi (giong Linux)
void KPlayer::TongTempMagicRecord(int nSkillId, int nLevel)
{
	int i, nFree = -1;
	for (i = 0; i < 8; i++)
	{
		if (m_TongTempMagic[i].nSkillId == nSkillId)
		{
			m_TongTempMagic[i].nLevel += nLevel;
			if (m_TongTempMagic[i].nLevel <= 0)
			{
				m_TongTempMagic[i].nSkillId = 0;
				m_TongTempMagic[i].nLevel = 0;
			}
			return;
		}
		if (nFree < 0 && m_TongTempMagic[i].nSkillId == 0)
			nFree = i;
	}
	if (nLevel > 0 && nFree >= 0)
	{
		m_TongTempMagic[nFree].nSkillId = nSkillId;
		m_TongTempMagic[nFree].nLevel = nLevel;
	}
}

void KPlayer::TongTempMagicReapply()
{
	if (m_nIndex <= 0 || m_nIndex >= MAX_NPC)
		return;
	for (int i = 0; i < 8; i++)
	{
		if (m_TongTempMagic[i].nSkillId > 0 && m_TongTempMagic[i].nLevel > 0)
			Npc[m_nIndex].m_SkillList.AllSkillV(m_TongTempMagic[i].nSkillId, m_TongTempMagic[i].nLevel);
	}
}

int KPlayer::TongIsForbidSkill(int nSkillId)
{
	if (m_bTongForbidSkill)
		return 1;
	for (int i = 0; i < 8; i++)
		if (m_nTongForbidSkillId[i] && m_nTongForbidSkillId[i] == nSkillId)
			return 1;
	return 0;
}
#endif

// need spe edit not end
void	KPlayer::UpdataCurData()
{
	if (m_nIndex <= 0 || m_nIndex >= MAX_NPC)
		return;
	Npc[m_nIndex].m_SkillList.m_nAllSkillV = 0;
	for (int i = 0 ; i< MAX_NPCSKILL;i++)
	{
		int nAddLevel = Npc[m_nIndex].m_SkillList.m_Skills[i].AddLevel;
		if (nAddLevel > 0)
		{
			Npc[m_nIndex].m_SkillList.m_Skills[i].CurrentSkillLevel -= nAddLevel;
			Npc[m_nIndex].m_SkillList.m_Skills[i].AddLevel = Npc[m_nIndex].m_SkillList.m_nAllSkillV;
		}
		if (Npc[m_nIndex].m_SkillList.m_Skills[i].TempSkill)
			Npc[m_nIndex].m_SkillList.RemoveIdx(i);
	}
#ifdef _SERVER
	// [TONG 21/08] AddTempMagic: ban Linux giu danh sach ban ghi (KSkillList+0xf08)
	// nen chieu tam song sot qua viec xoa TempSkill o tren; ta ap lai tu m_TongTempMagic.
	TongTempMagicReapply();
#endif
	int nCurbei = 0;
	ZeroMemory(&Npc[m_nIndex].m_PhysicsMagic, sizeof(KMagicAttrib));	
	Npc[m_nIndex].m_CurrentLifeMax = Npc[m_nIndex].m_LifeMax * (100 + m_cReBorn.GetReBornNum() * m_cReBorn.GetReBornLifeMaxVal()  + nCurbei * 50) / 100;
	//;Npc[m_nIndex].TakeTrader(Npc[m_nIndex].m_LifeMax*(100+m_cReBorn.GetReBornNum()*50+nCurbei*50),100);
	//Npc[m_nIndex].m_LifeMax*(100+m_cReBorn.GetReBornNum()*50+nCurbei*50)/100;
	Npc[m_nIndex].m_CurrentManaMax = Npc[m_nIndex].m_ManaMax * (100 + m_cReBorn.GetReBornNum() * m_cReBorn.GetReBornLifeMaxVal()  + nCurbei * 50) / 100;
	//Npc[m_nIndex].TakeTrader(Npc[m_nIndex].m_ManaMax*(100+m_cReBorn.GetReBornNum()*50+nCurbei*50),100);
	//Npc[m_nIndex].m_ManaMax*(100+m_cReBorn.GetReBornNum()*50+nCurbei*50)/100;
	Npc[m_nIndex].m_CurrentStaminaMax	= Npc[m_nIndex].m_StaminaMax;
	Npc[m_nIndex].m_CurrentAttackRating	= Npc[m_nIndex].m_AttackRating;
	Npc[m_nIndex].m_CurrentAttackSpeed	= BASE_ATTACK_SPEED;
//	Npc[m_nIndex].m_CurrentCamp			= Npc[m_nIndex].m_Camp;
	Npc[m_nIndex].m_CurrentCastSpeed	= BASE_CAST_SPEED;
	ZeroMemory(&Npc[m_nIndex].m_CurrentColdMagic, sizeof(KMagicAttrib));
	ZeroMemory(&Npc[m_nIndex].m_CurrentColdDamage, sizeof(KMagicAttrib));
	Npc[m_nIndex].m_CurrentColdEnhance	= 0;
	Npc[m_nIndex].m_CurrentColdResist	= Npc[m_nIndex].m_ColdResist;
	Npc[m_nIndex].m_CurrentColdResistMax	= Npc[m_nIndex].m_ColdResistMax;
	Npc[m_nIndex].m_CurrentDamage2Mana	= 0;
	Npc[m_nIndex].m_CurrentDamageReduce	= 0;

	Npc[m_nIndex].m_CurrentDeadlyStrikeEnhanceP	= 0;
	Npc[m_nIndex].m_CurrentFatallyStrikeEnhanceP	= 0;
	Npc[m_nIndex].m_CurrentFatallyStrikeResP = 0;
	
	Npc[m_nIndex].m_CurrentDefend		= Npc[m_nIndex].m_Defend;
	Npc[m_nIndex].m_CurrentElementDamageReduce = 0;
	ZeroMemory(&Npc[m_nIndex].m_CurrentFireMagic, sizeof(KMagicAttrib));
	ZeroMemory(&Npc[m_nIndex].m_CurrentFireDamage, sizeof(KMagicAttrib));
	Npc[m_nIndex].m_CurrentFireEnhance	= 0;

	Npc[m_nIndex].m_CurrentFireResist	= Npc[m_nIndex].m_FireResist;
	Npc[m_nIndex].m_CurrentFireResistMax	= Npc[m_nIndex].m_FireResistMax;
	Npc[m_nIndex].m_CurrentHandEnhance	= 0;
	Npc[m_nIndex].m_CurrentHitRecover	= Npc[m_nIndex].m_HitRecover;		//thêi gian phôc håi
	Npc[m_nIndex].m_CurrentJumpSpeed	= Npc[m_nIndex].m_JumpSpeed;
	Npc[m_nIndex].m_CurrentLifeReplenish	= Npc[m_nIndex].m_LifeReplenish;
	Npc[m_nIndex].m_CurrentLifeReplenishPercent	= 0;
	Npc[m_nIndex].m_CurrentLifeStolen	= 0;
	ZeroMemory(&Npc[m_nIndex].m_CurrentLightMagic, sizeof(KMagicAttrib));
	ZeroMemory(&Npc[m_nIndex].m_CurrentLightDamage, sizeof(KMagicAttrib));
	Npc[m_nIndex].m_CurrentLightEnhance	= 0;
	Npc[m_nIndex].m_CurrentLightResist	= Npc[m_nIndex].m_LightResist;
	Npc[m_nIndex].m_CurrentLightResistMax = Npc[m_nIndex].m_LightResistMax;
	Npc[m_nIndex].m_CurrentManaPerEnemy	= 0;
	Npc[m_nIndex].m_CurrentManaReplenish	= Npc[m_nIndex].m_ManaReplenish;
	Npc[m_nIndex].m_CurrentManaStolen	= 0;
	Npc[m_nIndex].m_CurrentMeleeDmgRet	= 0;
	Npc[m_nIndex].m_CurrentMeleeDmgRetPercent = 0;
	ZeroMemory(&Npc[m_nIndex].m_CurrentMeleeEnhance, sizeof(Npc[m_nIndex].m_CurrentMeleeEnhance));
	Npc[m_nIndex].m_CurrentPhysicsResist	= Npc[m_nIndex].m_PhysicsResist;
	Npc[m_nIndex].m_CurrentPhysicsResistMax	= Npc[m_nIndex].m_PhysicsResistMax;
	Npc[m_nIndex].m_CurrentPiercePercent	= 0;
	ZeroMemory(&Npc[m_nIndex].m_CurrentPoisonMagic, sizeof(KMagicAttrib));
	ZeroMemory(&Npc[m_nIndex].m_CurrentPoisonDamage, sizeof(KMagicAttrib));
	Npc[m_nIndex].m_CurrentPoisonEnhance	= 0;
	Npc[m_nIndex].m_CurrentPoisonResist	= Npc[m_nIndex].m_PoisonResist;
	Npc[m_nIndex].m_CurrentPoisonResistMax	= Npc[m_nIndex].m_PoisonResistMax;

	Npc[m_nIndex].m_CurrentColdResistMax = BASE_FANGYU_ALL_MAX + m_cReBorn.GetReBornNum() * m_cReBorn.GetReBornFanYuMaxVal();//Npc[m_nIndex].m_ColdResistMax;
	Npc[m_nIndex].m_CurrentFireResistMax = BASE_FANGYU_ALL_MAX + m_cReBorn.GetReBornNum() * m_cReBorn.GetReBornFanYuMaxVal();
	Npc[m_nIndex].m_CurrentLightResistMax = BASE_FANGYU_ALL_MAX + m_cReBorn.GetReBornNum() * m_cReBorn.GetReBornFanYuMaxVal();//Npc[m_nIndex].m_LightResistMax;
	Npc[m_nIndex].m_CurrentPhysicsResistMax = BASE_FANGYU_ALL_MAX + m_cReBorn.GetReBornNum() * m_cReBorn.GetReBornFanYuMaxVal();//Npc[m_nIndex].m_PhysicsResistMax;
	Npc[m_nIndex].m_CurrentPoisonResistMax = BASE_FANGYU_ALL_MAX + m_cReBorn.GetReBornNum() * m_cReBorn.GetReBornFanYuMaxVal();//Npc[m_nIndex].m_PoisonResistMax;

	Npc[m_nIndex].m_CurrentRangeDmgRet	= 0;
	Npc[m_nIndex].m_CurrentRangeDmgRetPercent	= 0;
	Npc[m_nIndex].m_CurrentRangeEnhance	= 0;
	if(!m_bSpeedControl)
		Npc[m_nIndex].m_CurrentRunSpeed	= Npc[m_nIndex].m_RunSpeed;
	Npc[m_nIndex].m_CurrentSlowMissle	= 0;
	Npc[m_nIndex].m_CurrentStaminaGain	= Npc[m_nIndex].m_StaminaGain;
	Npc[m_nIndex].m_CurrentStaminaLoss	= Npc[m_nIndex].m_StaminaLoss;
	Npc[m_nIndex].m_CurrentStaminaStolen	= 0;
	Npc[m_nIndex].m_CurrentVisionRadius	= Npc[m_nIndex].m_VisionRadius;
	if (!m_bSpeedControl)
		Npc[m_nIndex].m_CurrentWalkSpeed	= Npc[m_nIndex].m_WalkSpeed;
	Npc[m_nIndex].m_CurrentAddPhysicsDamage = 0;
	Npc[m_nIndex].m_CurrentAddPhysicsMagic = 0;
	m_nCurStrength						= m_nStrength;
	m_nCurDexterity						= m_nDexterity;
	m_nCurVitality						= m_nVitality;
	m_nCurEngergy						= m_nEngergy;
	m_nCurLucky							= m_nLucky;
	m_nUpExp							= 0;
	Npc[m_nIndex].m_CurrentFreezeTimeReducePercent = 0;	//Thêi gian lµm chËm gi¶m bít
	Npc[m_nIndex].m_CurrentPoisonTimeReducePercent = 0;
	Npc[m_nIndex].m_CurrentStunTimeReducePercent = 0;
	Npc[m_nIndex].m_CurrentIgnoreNegativeStateP = 0;
	Npc[m_nIndex].m_CurrentSerisesEnhance = 0;
	Npc[m_nIndex].m_CurrentDamageReduce = 0;
	Npc[m_nIndex].m_CurrentReturnSkillPercent = 0;
	Npc[m_nIndex].m_CurrentIgnoreSkillPercent = 0;
	Npc[m_nIndex].m_CurrentPoisonDamageReturnPercent = 0;

	ZeroMemory(&Npc[m_nIndex].m_PhysicsArmor, sizeof(Npc[m_nIndex].m_PhysicsArmor));
	ZeroMemory(&Npc[m_nIndex].m_ColdArmor, sizeof(Npc[m_nIndex].m_ColdArmor));
	ZeroMemory(&Npc[m_nIndex].m_FireArmor, sizeof(Npc[m_nIndex].m_FireArmor));
	ZeroMemory(&Npc[m_nIndex].m_PoisonArmor, sizeof(Npc[m_nIndex].m_PoisonArmor));
	ZeroMemory(&Npc[m_nIndex].m_LightArmor, sizeof(Npc[m_nIndex].m_LightArmor));
	ZeroMemory(&Npc[m_nIndex].m_ManaShield, sizeof(KMagicAttrib));
	ZeroMemory(&Npc[m_nIndex].m_ReplySkill, sizeof(KMagicAutoSkill) * MAX_AUTOSKILL);
	ZeroMemory(&Npc[m_nIndex].m_RescueSkill, sizeof(KMagicAutoSkill) * MAX_AUTOSKILL);
	ZeroMemory(&Npc[m_nIndex].m_AttackSkill, sizeof(KMagicAutoSkill) * MAX_AUTOSKILL);
	ZeroMemory(&Npc[m_nIndex].m_DeathSkill, sizeof(KMagicAutoSkill) * MAX_AUTOSKILL);
	Npc[m_nIndex].m_CurrentManaShield = 0;	//#giam thieu sat thuong ganh chiu HTVC con lon
	Npc[m_nIndex].m_CurrentStaticMagicShieldP = 0;
	Npc[m_nIndex].m_CurrentReturnResPercent = 0;
	Npc[m_nIndex].m_CurrentFiveElementsEnhance	= 0;
	Npc[m_nIndex].m_CurrentSkillEnhancePercent = 0;
	Npc[m_nIndex].m_CurrentFiveElementsResist = 0;
	Npc[m_nIndex].m_CurrentManaToSkillEnhanceP	= 0;					//#khi noi cong day tang ky nang cong kich
	Npc[m_nIndex].m_CurrentSorbDamageP	= 0;								//#triet tieu sat thuong
	Npc[m_nIndex].m_CurrentExpEnhance	= 0;								//nh©n ®«i nh©n ba nh©n x2 ®iÓm kinh nghiÖm
	Npc[m_nIndex].m_CurrentExpSkillsEnchance	= 1; // ExpSkills x2
	Npc[m_nIndex].m_CurrentExpSkillsVip = 1;

	ReCalcMeridian();
	ReCalcState();
	ReCalcEquip();
	ReCalcTongBenefit();
}

void KPlayer::ReCalcTongBenefit()
{
	_ASSERT(m_nIndex > 0 && m_nIndex < MAX_NPC);

	int tongLevel = m_cTong.GetTongLevel();

	if (tongLevel < 1 || tongLevel > 10)
		return; // Ensure Tong level is within the valid range

	int luckIncrease = tongLevel * 2; // Increase 2 lucky points per Tong level
	m_nCurLucky = m_nCurLucky + luckIncrease;
}

void KPlayer::ReCalcEquip()
{
	_ASSERT(m_nIndex > 0 && m_nIndex < MAX_NPC);

	for (int i = 0; i < itempart_num; i++)
	{
		int nIdx = m_ItemList.m_EquipItem[i];
		if (!nIdx)
			continue;

		
		Item[nIdx].m_fHorseScale = 1.0f;

	
		if (Item[nIdx].m_CommonAttrib.nDetailType == equip_horse)
		{
		
			Item[nIdx].m_fHorseScale = Npc[m_nIndex].m_bRideHorse ? 1.0f : 0.5f;
		}

		if (Item[nIdx].GetDurability() == 0)
			continue;   

		if (Item[nIdx].GetTimeYearIsExp())
			continue;    

		int nActive = m_ItemList.GetEquipEnhance(i);
		int nGoldAct = m_ItemList.GetGoldEquipEnhance(i);

		if (m_ItemList.m_bActiveSet)
		{
			nActive = 4;
		}

	
		Item[nIdx].ApplyMagicAttribToNPC(&Npc[m_nIndex], nActive, nGoldAct);
	}
}


void KPlayer::ReCalcState()
{
	_ASSERT(m_nIndex > 0 && m_nIndex < MAX_NPC);
	
	Npc[m_nIndex].ReCalcStateEffect();
}

void KPlayer::ReCalcMeridian()
{
	_ASSERT(m_nIndex > 0 && m_nIndex < MAX_NPC);
	m_nMeridianStrength = 0;
	m_nMeridianDexterity = 0;
	m_nMeridianVitality = 0;
	m_nMeridianEngergy = 0;
	MeridianManager.ApplyMaridianToNPC(&Npc[m_nIndex], m_cMeridian.getMeridian());
}

//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÔö¼ÓÍ³ÂÊÁ¦¾­Ñé
//-------------------------------------------------------------------------
void	KPlayer::AddLeadExp(int nExp)
{
	if (m_dwLeadLevel >= MAX_LEAD_LEVEL)
		return;
	if (Npc[m_nIndex].m_Doing == do_death || Npc[m_nIndex].m_Doing == do_revive)
		return;
	m_dwLeadExp += nExp;
	if (m_dwLeadExp >= (DWORD)PlayerSet.m_cLeadExp.GetLevelExp(m_dwLeadLevel))
	{
		// Éý¼¶
		m_dwLeadLevel = PlayerSet.m_cLeadExp.GetLevel(m_dwLeadExp, m_dwLeadLevel);
#ifndef _SERVER
		m_dwNextLevelLeadExp = PlayerSet.m_cLeadExp.GetLevelExp(m_dwLeadLevel);
#endif
	}
	
#ifdef _SERVER
	PLAYER_LEAD_EXP_SYNC	sLeadExp;
	sLeadExp.ProtocolType = s2c_playersyncleadexp;
	sLeadExp.m_dwLeadExp = m_dwLeadExp;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sLeadExp, sizeof(PLAYER_LEAD_EXP_SYNC));
#endif
	
}

//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÍæ¼ÒÉýÒ»¼¶¼ÓÉúÃü
//-------------------------------------------------------------------------
void KPlayer::LevelAddBaseLifeMax()
{
	Npc[m_nIndex].m_LifeMax += PlayerSet.m_cLevelAdd.GetLifePerLevel(Npc[m_nIndex].m_Series);
	Npc[m_nIndex].m_CurrentLifeMax = Npc[m_nIndex].m_LifeMax;
}

//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÍæ¼ÒÉýÒ»¼¶¼ÓÌåÁ¦
//-------------------------------------------------------------------------
void KPlayer::LevelAddBaseStaminaMax()
{
	Npc[m_nIndex].m_StaminaMax += PlayerSet.m_cLevelAdd.GetStaminaPerLevel(Npc[m_nIndex].m_Series);
	Npc[m_nIndex].m_CurrentStaminaMax = Npc[m_nIndex].m_StaminaMax;
}

//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÍæ¼ÒÉýÒ»¼¶¼ÓÄÚÁ¦
//-------------------------------------------------------------------------
void KPlayer::LevelAddBaseManaMax()
{
	Npc[m_nIndex].m_ManaMax += PlayerSet.m_cLevelAdd.GetManaPerLevel(Npc[m_nIndex].m_Series);
	Npc[m_nIndex].m_CurrentManaMax = Npc[m_nIndex].m_ManaMax;
}

//-------------------------------------------------------------------------
//	¹¦ÄÜ£º¸Ä±äÍæ¼ÒÕóÓª
//-------------------------------------------------------------------------
void	KPlayer::ChangePlayerCamp(int nCamp)
{
	if (nCamp < camp_begin || nCamp >= camp_num)
		return;
	Npc[m_nIndex].m_Camp = (NPCCAMP)nCamp;
}

#ifndef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÏò·þÎñÆ÷ÉêÇë²éÑ¯Ä³¸önpcËùÔÚ¶ÓÎéµÄÐÅÏ¢
//-------------------------------------------------------------------------
void	KPlayer::ApplyTeamInfo(DWORD dwNpcID)
{
	if (dwNpcID <= 0)
		return;
	PLAYER_APPLY_TEAM_INFO	sApplyInfo;
	sApplyInfo.ProtocolType = (BYTE)c2s_teamapplyinfo;
	sApplyInfo.m_dwTarNpcID = dwNpcID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sApplyInfo, sizeof(PLAYER_APPLY_TEAM_INFO));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÏò·þÎñÆ÷ÉêÇë²éÑ¯Íæ¼Ò×ÔÉíµÄ¶ÓÎéÇé¿ö
//-------------------------------------------------------------------------
void	KPlayer::ApplySelfTeamInfo()
{
	ApplyTeamInfo(Npc[m_nIndex].m_dwID);
}
#endif

#ifndef _SERVER
BOOL	KPlayer::ApplyCreateTeam()
{
	return m_cTeam.ApplyCreate();
}
#endif

#ifndef _SERVER
BOOL	KPlayer::ApplyTeamOpenClose(BOOL bFlag)
{
	if ( !m_cTeam.m_nFlag )					
		return FALSE;
	if (m_cTeam.m_nFigure != TEAM_CAPTAIN)	
		return FALSE;
	if (this->CheckTrading())	
		return FALSE;
	
	if (bFlag)
	{
		if (PlayerSet.m_cLeadExp.GetMemNumFromLevel(m_dwLeadLevel) <= g_Team[0].m_nMemNum)	
			return FALSE;
	}
	
	PLAYER_TEAM_OPEN_CLOSE	sTeamState;
	sTeamState.ProtocolType = c2s_teamapplyopenclose;
	sTeamState.m_btOpenClose = bFlag;
	if (g_pClient)
		g_pClient->SendPackToServer(&sTeamState, sizeof(PLAYER_TEAM_OPEN_CLOSE));
	return TRUE;
}
#endif

#ifndef _SERVER
void	KPlayer::ApplyAddTeam(int nNpcIndex)
{
	if (this->CheckTrading())
		return;
	if (m_cTeam.m_nFlag)					
		return;
	if (Npc[nNpcIndex].m_Camp == camp_begin && Npc[m_nIndex].m_Camp != camp_begin)
		return;
	//	if (Npc[nNpcIndex].m_Camp != Npc[m_nIndex].m_Camp)	// ÕóÓª²»Í¬
	//		return;
	if (Npc[nNpcIndex].GetMenuState() != PLAYER_MENU_STATE_TEAMOPEN)
		return;
	
	m_cTeam.m_nApplyCaptainID = Npc[nNpcIndex].m_dwID;
	m_cTeam.m_dwApplyTimer = MAX_APPLY_TEAM_TIME;
	PLAYER_APPLY_ADD_TEAM	sAddTeam;
	sAddTeam.ProtocolType = c2s_teamapplyadd;
	sAddTeam.m_dwTarNpcID = Npc[nNpcIndex].m_dwID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sAddTeam, sizeof(PLAYER_APPLY_ADD_TEAM));
	
	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_TEAM_APPLY_ADD_SELF_MSG, Npc[nNpcIndex].Name);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}
#endif

#ifndef _SERVER
void	KPlayer::AcceptTeamMember(DWORD dwNpcID)
{
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
	{
		ApplySelfTeamInfo();
		return;
	}
	if (dwNpcID == 0)
		return;
	
	int		i;
	for (i = 0; i < MAX_TEAM_APPLY_LIST; i++)
	{
		if (m_cTeam.m_sApplyList[i].m_dwNpcID == dwNpcID)
		{
			m_cTeam.m_sApplyList[i].m_dwNpcID = 0;
			m_cTeam.UpdateInterface();
			break;
		}
	}

//	if (i >= MAX_TEAM_APPLY_LIST)
//		return;
	
	PLAYER_ACCEPT_TEAM_MEMBER	sAcceptTeam;
	sAcceptTeam.ProtocolType = c2s_teamacceptmember;
	sAcceptTeam.m_dwNpcID = dwNpcID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sAcceptTeam, sizeof(PLAYER_ACCEPT_TEAM_MEMBER));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£º¶Ó³¤É¾³ý¼ÓÈë¶ÓÎéÉêÇëÁÐ±íÖÐµÄÄ³¸önpc
//-------------------------------------------------------------------------
void	KPlayer::TeamDropApplyOne(DWORD dwNpcID)
{
	if (dwNpcID == 0)
		return;
	// 
	int		i;
	for (i = 0; i < MAX_TEAM_APPLY_LIST; i++)
	{
		if (m_cTeam.m_sApplyList[i].m_dwNpcID == dwNpcID)
		{
			m_cTeam.m_sApplyList[i].m_dwNpcID = 0;
			m_cTeam.m_sApplyList[i].m_dwTimer = 0;
			m_cTeam.m_sApplyList[i].m_nLevel = 0;
			m_cTeam.m_sApplyList[i].m_szName[0] = 0;
			break;
		}
	}
}
#endif

#ifndef _SERVER
void	KPlayer::LeaveTeam()
{
	PLAYER_APPLY_LEAVE_TEAM	sLeaveTeam;
	sLeaveTeam.ProtocolType = c2s_teamapplyleave;
	if (g_pClient)
		g_pClient->SendPackToServer(&sLeaveTeam, sizeof(PLAYER_APPLY_LEAVE_TEAM));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£º¶Ó³¤Í¨Öª·þÎñÆ÷Ìß³ýÄ³¸ö¶ÓÔ±
//-------------------------------------------------------------------------
void	KPlayer::TeamKickMember(DWORD dwNpcID)
{
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
		return;
	
	PLAYER_TEAM_KICK_MEMBER	sKickOne;
	sKickOne.ProtocolType = c2s_teamapplykickmember;
	sKickOne.m_dwNpcID = dwNpcID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sKickOne, sizeof(PLAYER_TEAM_KICK_MEMBER));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£º¶Ó³¤Ïò·þÎñÆ÷ÉêÇë°Ñ×Ô¼ºµÄ¶Ó³¤Éí·Ý½»¸ø±ðµÄ¶ÓÔ±
//-------------------------------------------------------------------------
void	KPlayer::ApplyTeamChangeCaptain(DWORD dwNpcID)
{
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
		return;
	
	PLAYER_APPLY_TEAM_CHANGE_CAPTAIN	sTeamChange;
	sTeamChange.ProtocolType = c2s_teamapplychangecaptain;
	sTeamChange.m_dwNpcID = dwNpcID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sTeamChange, sizeof(PLAYER_APPLY_TEAM_CHANGE_CAPTAIN));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£º¶Ó³¤Ïò·þÎñÆ÷ÉêÇë½âÉ¢¶ÓÎé
//-------------------------------------------------------------------------
void	KPlayer::ApplyTeamDismiss()
{
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
		return;
	
	PLAYER_APPLY_TEAM_DISMISS	sTeamDismiss;
	sTeamDismiss.ProtocolType = c2s_teamapplydismiss;
	if (g_pClient)
		g_pClient->SendPackToServer(&sTeamDismiss, sizeof(PLAYER_APPLY_TEAM_DISMISS));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÍæ¼ÒÏò·þÎñÆ÷ÉêÇë´ò¿ª¡¢¹Ø±Õpk¿ª¹Ø
//-------------------------------------------------------------------------
void	KPlayer::ApplySetPK(BOOL bPK)
{
	PLAYER_SET_PK	sPK;
	sPK.ProtocolType = c2s_playerapplysetpk;
	sPK.m_btPKFlag = bPK;
	if (g_pClient)
		g_pClient->SendPackToServer(&sPK, sizeof(PLAYER_SET_PK));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÍæ¼ÒÏò·þÎñÆ÷ÉêÇëÃÅÅÉÊý¾Ý
//-------------------------------------------------------------------------
void	KPlayer::ApplyFactionData()
{
	PLAYER_APPLY_FACTION_DATA	sFaction;
	sFaction.ProtocolType = c2s_playerapplyfactiondata;
	if (g_pClient)
		g_pClient->SendPackToServer(&sFaction, sizeof(PLAYER_APPLY_FACTION_DATA));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£º¿Í»§¶Ë·¢ËÍÁÄÌìÓï¾ä¸ø·þÎñÆ÷
//-------------------------------------------------------------------------
void	KPlayer::SendChat(KUiMsgParam *pMsg, char *lpszSentence)
{
	if (!lpszSentence || !pMsg || pMsg->nMsgLength <= 0)
		return;
	int nLength = pMsg->nMsgLength;
	if (nLength >= MAX_SENTENCE_LENGTH)
		nLength = MAX_SENTENCE_LENGTH - 1;
	
	if (pMsg->eGenre == MSG_G_CHAT)
	{
		Npc[m_nIndex].SetChatInfo(pMsg->szName, lpszSentence, nLength);
		
		m_cChat.SendSentence(pMsg, Npc[m_nIndex].m_szChatBuffer);
	}
	else if (pMsg->eGenre == MSG_G_CMD)	// not end
	{
	}
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£º¶Ó³¤Ïò·þÎñÆ÷ÉêÇëÔö¼ÓËÄÏîÊôÐÔÖÐÄ³Ò»ÏîµÄµãÊý(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
//-------------------------------------------------------------------------
void	KPlayer::ApplyAddBaseAttribute(int nAttribute, int nNo)
{
	if (nAttribute < 0 || nAttribute > 3)
		return;
	if (nNo <= 0 || nNo >= 255)
		return;
	PLAYER_ADD_BASE_ATTRIBUTE_COMMAND	sAdd;
	sAdd.ProtocolType = c2s_playeraddbaseattribute;
	sAdd.m_btAttribute = nAttribute;
	sAdd.m_nAddNo = nNo;
	if (g_pClient)
		g_pClient->SendPackToServer(&sAdd, sizeof(PLAYER_ADD_BASE_ATTRIBUTE_COMMAND));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÏò·þÎñÆ÷ÉêÇëÄ³¸ö¼¼ÄÜÉý¼¶
//-------------------------------------------------------------------------
BOOL	KPlayer::ApplyAddSkillLevel(int nSkillID, int nAddPoint)
{
	if ( !Npc[m_nIndex].m_SkillList.FindSame(nSkillID) )
		return FALSE;
	if (m_nSkillPoint < nAddPoint || nAddPoint + Npc[m_nIndex].m_SkillList.GetLevel(nSkillID) > MAX_SKILLLEVEL)
		return FALSE;
	
	PLAYER_ADD_SKILL_POINT_COMMAND	sSkill;
	sSkill.ProtocolType = c2s_playerapplyaddskillpoint;
	sSkill.m_nSkillID = nSkillID;
	sSkill.m_nAddPoint = nAddPoint;
	
	if (g_pClient)
		g_pClient->SendPackToServer(&sSkill, sizeof(PLAYER_ADD_SKILL_POINT_COMMAND));
	
	return TRUE;
}
#endif

#ifndef _SERVER
BOOL	KPlayer::ApplyAutoMoveItem(int nItemID, ItemPos SrcPos) //chuyÓn item tõ hµnh trang vµo r­¬ng
{
	ItemPos DesPos;
	DesPos.nPlace = DesPos.nX = DesPos.nY = -1;

	if (this->CheckTrading())
		return FALSE;

	DesPos.nPlace = pos_repositoryroom;
	
	POINT pPos;
	if(Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Room[room_repository].FindRoom(Item[nItemID].GetWidth(), Item[nItemID].GetHeight(), &pPos))//t×m ®­îc kho¶ng trèng trong r­¬ng
	{	
		DesPos.nX = pPos.x;
		DesPos.nY = pPos.y;
	}

	if(DesPos.nPlace < 0 || DesPos.nX < 0 || DesPos.nY < 0)
	{
		return FALSE;
	}

	PLAYER_MOVE_ITEM_COMMAND	sMove;
	sMove.ProtocolType = c2s_playermoveitem;
	sMove.m_btDownPos = SrcPos.nPlace;
	sMove.m_btDownX = SrcPos.nX;
	sMove.m_btDownY = SrcPos.nY;
	sMove.m_btUpPos = SrcPos.nPlace;
	sMove.m_btUpX = SrcPos.nX;
	sMove.m_btUpY = SrcPos.nY;
	if (g_pClient)
		g_pClient->SendPackToServer(&sMove, sizeof(PLAYER_MOVE_ITEM_COMMAND));

	PLAYER_MOVE_ITEM_COMMAND	sMove2;
	sMove2.ProtocolType = c2s_playermoveitem;
	sMove2.m_btDownPos = DesPos.nPlace;
	sMove2.m_btDownX = DesPos.nX;
	sMove2.m_btDownY = DesPos.nY;
	sMove2.m_btUpPos = DesPos.nPlace;
	sMove2.m_btUpX = DesPos.nX;
	sMove2.m_btUpY = DesPos.nY;
	if (g_pClient)
		g_pClient->SendPackToServer(&sMove2, sizeof(PLAYER_MOVE_ITEM_COMMAND));

	if (g_pClient) 
		g_pClient->SendPackToServer(&sMove, sizeof(PLAYER_MOVE_ITEM_COMMAND));

	m_ItemList.LockOperation();	

	return TRUE;
}

BOOL	KPlayer::ApplyUseItem(int nItemID, ItemPos SrcPos, int nTargetPos)
{
	//ItemPos DesPos;
	//DesPos.nPlace = DesPos.nX = DesPos.nY = -1;

	if (this->CheckTrading())
		return FALSE;
	
	m_nLastNpcIndex = 0;

	int nRet = m_ItemList.UseItem(nItemID);
	if (nRet == 0)
		return FALSE;
	/*if (nRet == 0)
	{
		nRet = m_ItemList.ChangeItemInPlayer(nItemID);//edit by phong kieu mac trang bi vao nguoi
		switch(nRet)
		{
			case equip_horse:
				DesPos.nPlace = pos_equip;
				DesPos.nX = itempart_horse;
				//strcpy(sMsg.szMessage, "Mang ngùa ...");
				 break;
			case equip_meleeweapon:
				DesPos.nPlace = pos_equip;
				DesPos.nX = itempart_weapon;
				//strcpy(sMsg.szMessage, "Mang vò khÝ cËn chiÕn ...");
				break;
			case equip_rangeweapon:
				DesPos.nPlace = pos_equip;
				DesPos.nX = itempart_weapon;
				//strcpy(sMsg.szMessage, "Mang vò khÝ tÇm xa ...");
				break;
			case equip_armor:
				DesPos.nPlace = pos_equip;
				DesPos.nX = itempart_body;
				//strcpy(sMsg.szMessage, "Mang y phôc ...");
				break;
			case equip_ring:
				{
					DesPos.nPlace = pos_equip;
					DesPos.nX = 7;
				if(m_ItemList.m_EquipItem[7] > 0 && m_ItemList.m_EquipItem[8] == 0)
					DesPos.nX = 8;
					break;
				}
			case equip_amulet:
				DesPos.nPlace = pos_equip;
				DesPos.nX = itempart_amulet;
				//strcpy(sMsg.szMessage, "Mang d©y chuyÒn ...");
				break;
			case equip_boots:
				DesPos.nPlace = pos_equip;
				DesPos.nX = itempart_foot;
				//strcpy(sMsg.szMessage, "Mang giµy ...");
				break;
			case equip_belt:
				DesPos.nPlace = pos_equip;
				DesPos.nX = itempart_belt;
				//strcpy(sMsg.szMessage, "Mang ®ai l­ng ...");
				break;
			case equip_helm:
				DesPos.nPlace = pos_equip;
				DesPos.nX = itempart_head;
				//strcpy(sMsg.szMessage, "Mang mò ...");
				break;
			case equip_cuff:
				DesPos.nPlace = pos_equip;
				DesPos.nX = itempart_cuff;
				//strcpy(sMsg.szMessage, "Mang bao tay ...");
				break;
			case equip_pendant:
				DesPos.nPlace = pos_equip;
				DesPos.nX = itempart_pendant;
				//strcpy(sMsg.szMessage, "Mang ngäc béi ...");
				break;
			case equip_mask:
				DesPos.nPlace = pos_equip;
				DesPos.nX = itempart_mask;
				//strcpy(sMsg.szMessage, "Mang mÆt n¹ ...");
				break;
			case equip_mantle:
				DesPos.nPlace = pos_equip;
				DesPos.nX = itempart_mantle;
				//strcpy(sMsg.szMessage, "Mang phi phong ...");
				break;
		}

		if(DesPos.nPlace < 0)
		{
			return FALSE;
		}

		PLAYER_MOVE_ITEM_COMMAND	sMove;
		sMove.ProtocolType = c2s_playermoveitem;
		sMove.m_btDownPos = SrcPos.nPlace;
		sMove.m_btDownX = SrcPos.nX;
		sMove.m_btDownY = SrcPos.nY;
		sMove.m_btUpPos = SrcPos.nPlace;
		sMove.m_btUpX = SrcPos.nX;
		sMove.m_btUpY = SrcPos.nY;
		if (g_pClient)
			g_pClient->SendPackToServer(&sMove, sizeof(PLAYER_MOVE_ITEM_COMMAND));

		PLAYER_MOVE_ITEM_COMMAND	sMove2;
		sMove2.ProtocolType = c2s_playermoveitem;
		sMove2.m_btDownPos = DesPos.nPlace;
		sMove2.m_btDownX = DesPos.nX;
		sMove2.m_btDownY = DesPos.nY;
		sMove2.m_btUpPos = DesPos.nPlace;
		sMove2.m_btUpX = DesPos.nX;
		sMove2.m_btUpY = DesPos.nY;
		if (g_pClient)
			g_pClient->SendPackToServer(&sMove2, sizeof(PLAYER_MOVE_ITEM_COMMAND));

		if (g_pClient) 
			g_pClient->SendPackToServer(&sMove, sizeof(PLAYER_MOVE_ITEM_COMMAND));

		//sMsg.byConfirmType = SMCT_NONE;
		//sMsg.eType = SMT_NORMAL;
		//sMsg.byParamSize = 0;
		//sMsg.byPriority = 0;
		//CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

		m_ItemList.LockOperation();	

		return FALSE;
	}
	*/
	if (nRet == REQUEST_EQUIP_ITEM)
	{
		if(nTargetPos == 0)
			return FALSE;
		char szPack[16];
		DYNAMIC_COMMAND* pCmd = (DYNAMIC_COMMAND*)&szPack[0];
		pCmd->ProtocolType = c2s_dynamic_structure;
		pCmd->nBranch = c2sdnmbr_exchangeitem;
		pCmd->m_wLength = sizeof(DYNAMIC_COMMAND) - 1 + 2*sizeof(BYTE) + sizeof(int);
		BYTE* pPos = (BYTE*)(pCmd+1);
		*pPos = SrcPos.nPlace;
		pPos++;
		*pPos = nTargetPos;
		pPos++;
		*(int*)pPos = Item[nItemID].GetID();
		if (g_pClient)
			g_pClient->SendPackToServer(pCmd, pCmd->m_wLength + 1);
	}
	else if (nRet == REQUEST_EAT_MEDICINE)
	{
		PLAYER_EAT_ITEM_COMMAND	sEat;
		sEat.ProtocolType = c2s_playereatitem;
		sEat.m_nItemID = nItemID;
		sEat.m_btPlace = SrcPos.nPlace;
		sEat.m_btX = SrcPos.nX;
		sEat.m_btY = SrcPos.nY;
		if (g_pClient)
			g_pClient->SendPackToServer(&sEat, sizeof(PLAYER_EAT_ITEM_COMMAND));
	}
	
	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£º¿Í»§¶ËÊó±êµã»÷obj¼ìÆðÄ³¸öÎïÆ·£¬Ïò·þÎñÆ÷·¢ÏûÏ¢
//-------------------------------------------------------------------------
void	KPlayer::PickUpObj(int nObjIndex)
{
	if (this->CheckTrading())
		return;
	if (nObjIndex <= 0)
		return;
	AUTOLOG("PICK-SKIP-KIND idx=%d id=%d kind=%d", nObjIndex, Object[nObjIndex].m_nID, Object[nObjIndex].m_nKind);
	if (Object[nObjIndex].m_nKind != Obj_Kind_Item && Object[nObjIndex].m_nKind != Obj_Kind_Money)
		return;
	
	PLAYER_PICKUP_ITEM_COMMAND	sPickUp;
	if (Object[nObjIndex].m_nKind == Obj_Kind_Money)
	{
		sPickUp.ProtocolType = c2s_playerpickupitem;
		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
		sPickUp.m_btPosType = 0;
		sPickUp.m_btPosX = 0;
		sPickUp.m_btPosY = 0;
	}
	else
	{
		ItemPos	sItemPos;
		AUTOLOG("PICK-NOROOM idx=%d id=%d itemid=%d w=%d h=%d", nObjIndex, Object[nObjIndex].m_nID, Object[nObjIndex].m_nItemDataID, Object[nObjIndex].m_nItemWidth, Object[nObjIndex].m_nItemHeight);
		if ( FALSE == m_ItemList.SearchPosition(Object[nObjIndex].m_nItemWidth, Object[nObjIndex].m_nItemHeight, &sItemPos) )
		{
			KSystemMessage	sMsg;
			
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.eType = SMT_NORMAL;
			sMsg.byParamSize = 0;
			sMsg.byPriority = 0;
			
			AUTOLOG("PICKOBJ-NOROOM obj=%d dataid=%d name=%.79s w=%d h=%d", Object[nObjIndex].m_nID, Object[nObjIndex].m_nItemDataID, Object[nObjIndex].m_szName, Object[nObjIndex].m_nItemWidth, Object[nObjIndex].m_nItemHeight);
			strcpy(sMsg.szMessage, MSG_SHOP_NO_ROOM);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			return;
		}
		sPickUp.ProtocolType = c2s_playerpickupitem;
		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
		sPickUp.m_btPosType = sItemPos.nPlace;
		sPickUp.m_btPosX = sItemPos.nX;
		sPickUp.m_btPosY = sItemPos.nY;
	}
	
	if (g_pClient)
	{
			AUTOLOG("[PICKOBJ-SEND] objid=%d kind=%d place=%d px=%d py=%d", sPickUp.m_nObjID, (int)Object[nObjIndex].m_nKind, (int)sPickUp.m_btPosType, (int)sPickUp.m_btPosX, (int)sPickUp.m_btPosY);
			g_pClient->SendPackToServer(&sPickUp, sizeof(PLAYER_PICKUP_ITEM_COMMAND));
	}
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£º¿Í»§¶ËÊó±êµã»÷obj£¬Ïò·þÎñÆ÷·¢ÏûÏ¢
//-------------------------------------------------------------------------
void	KPlayer::ObjMouseClick(int nObjIndex)
{
	if (this->CheckTrading())
		return;
	if (nObjIndex <= 0)
		return;
	if (Object[nObjIndex].m_nKind != Obj_Kind_Box && Object[nObjIndex].m_nKind != Obj_Kind_Prop)
		return;

	if (Object[nObjIndex].m_nRegionIdx < 0)
		return;

	SendObjMouseClick(Object[nObjIndex].m_nID, SubWorld[0].m_Region[Object[nObjIndex].m_nRegionIdx].m_RegionID);
}
#endif

#ifndef _SERVER
void KPlayer::MoveItem(ItemPos DownPos, ItemPos UpPos)
{
	if (this->CheckTrading() && (DownPos.nPlace == pos_traderoom || DownPos.nPlace == pos_gambleroom))
	{
		if (this->m_cTrade.m_nTradeLock)
			return;
		
		int	nHandIdx = m_ItemList.Hand();
		if(nHandIdx && (Item[nHandIdx].GetPlayerItemLock() > 0 || Item[nHandIdx].GetPlayerItemHLock() > 0 || Item[nHandIdx].GetPlayerItemLock() == -2))
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_TASK_ITEM);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			return;
		}
	}
	
	if (!CheckTrading() && DownPos.nPlace == pos_immediacy)
	{
		int nHandIdx = m_ItemList.Hand();
		if (nHandIdx) //check vµ th«ng b¸o item hîp lÖ tr­íc khi bá vµo « t¹i client
		{
			/*if (Item[nHandIdx].GetStackNum() > 1)
			{
				KSystemMessage	sMsg;
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				sprintf(sMsg.szMessage, MSG_ITEM_NOT_STACK_IN_IMMEDIATE);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				return;
			}*/
			//
			if(!(Item[nHandIdx].GetGenre() == item_medicine || Item[nHandIdx].GetGenre() == item_townportal 
				|| (Item[nHandIdx].GetGenre() == item_magicscript && Item[nHandIdx].GetBShortKey()))) //chØ cho phÐp bá c¸c vËt phÈm thuèc vµ thæ ®Þa phï
			{
				KSystemMessage	sMsg;
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				sprintf(sMsg.szMessage, MSG_ITEM_NOT_STACK_IN_IMMEDIATE);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				return;					
			}
			//
			if (m_ItemList.m_Room[room_immediacy].CheckSameItemType(Item[nHandIdx].GetGenre(), Item[nHandIdx].GetDetailType(), 
				Item[nHandIdx].GetParticular(), Item[nHandIdx].GetLevel()))
			{
				KSystemMessage	sMsg;
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				sprintf(sMsg.szMessage, MSG_ITEM_SAME_DETAIL_IN_IMMEDIATE);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				return;
			}
		}
	}
	
	SendClientCmdMoveItem(&DownPos, &UpPos);
}
#endif

#ifndef _SERVER
int	KPlayer::ThrowAllItem(int nId, int nUgen, int nRegion)
{

	if (this->CheckTrading())
	{
		return 0;
	}
	PLAYER_THROW_ALL_ITEM_COMMAND	sThrow;
	sThrow.ProtocolType = c2s_playerthrowallitem;
	strcpy(sThrow.szItemName, Item[nId].GetName());
	sThrow.m_nItemIdx = nId;
	sThrow.m_nItemDwId = Item[nId].GetID();

	if (g_pClient)
		g_pClient->SendPackToServer(&sThrow, sizeof(PLAYER_THROW_ALL_ITEM_COMMAND));

	return 1;
}

int	KPlayer::ThrowAwayItem()
{

	if (this->CheckTrading())
	{
		return 0;
	}
	int	nHandIdx = m_ItemList.Hand();
if (SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID == 996 || SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID == 997) //map PUBG cho vut do
		goto PUBG;
	if (
		nHandIdx && Item[nHandIdx].GetGoldId() 
		|| nHandIdx && Item[nHandIdx].GetNature() >= NATURE_GOLD
		|| nHandIdx && Item[nHandIdx].IsPurple()
		|| nHandIdx && Item[nHandIdx].GetGenre() == item_task
		|| nHandIdx && Item[nHandIdx].m_nCurrentDur == 0
		)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_NOT_THROW);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return 0;
	}

	else if(nHandIdx && (Item[nHandIdx].InsuranceCourse > 0 || Item[nHandIdx].InsuranceHourCourse > 0 || Item[nHandIdx].InsuranceCourse == -2))
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage,  MSG_NOT_THROW);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return 0;
	}	
	else
	{
PUBG:				
		if (this->CheckTrading())
			return 0;
		if ( !m_ItemList.Hand() )
			return 0;
		//if ( m_cAuto.mfk_bActive && 
		//	m_cAuto.m_bSortEquipment )
		//	return 0;

		PLAYER_THROW_AWAY_ITEM_COMMAND	sThrow;
		sThrow.ProtocolType = c2s_playerthrowawayitem;
		if (g_pClient)
			g_pClient->SendPackToServer(&sThrow, sizeof(PLAYER_THROW_AWAY_ITEM_COMMAND));
		return 1;
	}

	return 0;
}
#endif

#ifndef _SERVER
void	KPlayer::ChatAddFriend(int nPlayerIdx)
{
	CHAT_ADD_FRIEND_COMMAND	sAdd;
	sAdd.ProtocolType = c2s_chataddfriend;
	sAdd.m_nTargetPlayerIdx = nPlayerIdx;
	if (g_pClient)
		g_pClient->SendPackToServer(&sAdd, sizeof(CHAT_ADD_FRIEND_COMMAND));
}
#endif

#ifndef _SERVER
void	KPlayer::ChatRefuseFriend(int nPlayerIdx)
{
	CHAT_REFUSE_FRIEND_COMMAND	sRefuse;
	sRefuse.ProtocolType = c2s_chatrefusefriend;
	sRefuse.m_nTargetPlayerIdx = nPlayerIdx;
	if (g_pClient)
		g_pClient->SendPackToServer(&sRefuse, sizeof(CHAT_REFUSE_FRIEND_COMMAND));
}
#endif

#ifndef _SERVER
void	KPlayer::TradeApplyOpen(char *lpszSentence, int nLength)
{
	if (this->CheckTrading())
		return;
	if (!lpszSentence)
		nLength = 0;
	TRADE_APPLY_OPEN_COMMAND	sOpen;
	sOpen.ProtocolType = c2s_tradeapplystateopen;
	if (nLength >= MAX_SENTENCE_LENGTH)
		nLength = MAX_SENTENCE_LENGTH - 1;
	sOpen.m_wLength = sizeof(TRADE_APPLY_OPEN_COMMAND) - 1 - sizeof(sOpen.m_szSentence) + nLength;
	memset(m_cMenuState.m_szSentence, 0, sizeof(m_cMenuState.m_szSentence));
	if (lpszSentence)
		memcpy(m_cMenuState.m_szSentence, lpszSentence, nLength);
	memset(sOpen.m_szSentence, 0, sizeof(sOpen.m_szSentence));
	memcpy(sOpen.m_szSentence, m_cMenuState.m_szSentence, nLength);
	if (g_pClient)
		g_pClient->SendPackToServer(&sOpen, sOpen.m_wLength + 1);
}
#endif

#ifndef _SERVER
void	KPlayer::GambleApplyOpen(char* lpszSentence, int nLength)
{
	if (this->CheckTrading())
		return;
	if (!lpszSentence)
		nLength = 0;
	GAMBLE_APPLY_OPEN_COMMAND	sOpen;
	sOpen.ProtocolType = c2s_gambleapplystateopen;
	if (nLength >= MAX_SENTENCE_LENGTH)
		nLength = MAX_SENTENCE_LENGTH - 1;
	sOpen.m_wLength = sizeof(GAMBLE_APPLY_OPEN_COMMAND) - 1 - sizeof(sOpen.m_szSentence) + nLength;
	memset(m_cMenuState.m_szSentence, 0, sizeof(m_cMenuState.m_szSentence));
	if (lpszSentence)
		memcpy(m_cMenuState.m_szSentence, lpszSentence, nLength);
	memset(sOpen.m_szSentence, 0, sizeof(sOpen.m_szSentence));
	memcpy(sOpen.m_szSentence, m_cMenuState.m_szSentence, nLength);
	if (g_pClient)
		g_pClient->SendPackToServer(&sOpen, sOpen.m_wLength + 1);
}
#endif

#ifndef _SERVER
void	KPlayer::TradeApplyClose()
{
	TRADE_APPLY_CLOSE_COMMAND	sApply;
	sApply.ProtocolType = c2s_tradeapplystateclose;
	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sizeof(TRADE_APPLY_CLOSE_COMMAND));
}
#endif

#ifndef _SERVER
void	KPlayer::GambleApplyClose()
{
	GAMBLE_APPLY_CLOSE_COMMAND	sApply;
	sApply.ProtocolType = c2s_gambleapplystateclose;
	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sizeof(GAMBLE_APPLY_CLOSE_COMMAND));
}
#endif

#ifndef _SERVER
void	KPlayer::TradeApplyStart(int nNpcIdx)
{
	if (nNpcIdx < 0 || nNpcIdx >= MAX_NPC)
		return;

	TRADE_APPLY_START_COMMAND	sStart;
	sStart.ProtocolType = c2s_tradeapplystart;
	sStart.m_dwID = Npc[nNpcIdx].m_dwID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sStart, sizeof(TRADE_APPLY_START_COMMAND));
	
	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_TRADE_SEND_APPLY, Npc[nNpcIdx].Name);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}
#endif

#ifndef _SERVER
void	KPlayer::GambleApplyStart(int nNpcIdx)
{
	if (nNpcIdx < 0 || nNpcIdx >= MAX_NPC)
		return;
	if (!m_CUnlocked)
		return;
	GAMBLE_APPLY_START_COMMAND	sStart;
	sStart.ProtocolType = c2s_gambleapplystart;
	sStart.m_dwID = Npc[nNpcIdx].m_dwID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sStart, sizeof(GAMBLE_APPLY_START_COMMAND));

	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_GAMBLE_SEND_APPLY, Npc[nNpcIdx].Name);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}
#endif

#ifndef _SERVER
BOOL	KPlayer::TradeMoveMoney(int nMoney)
{
	if (!CheckTrading())
		return FALSE;
	if (this->m_cTrade.m_nTradeLock)
		return FALSE;

	if (nMoney < 0 || nMoney > m_ItemList.GetEquipmentMoney() +  m_ItemList.GetTradeMoney())
		return FALSE;
	
	TRADE_MOVE_MONEY_COMMAND	sMoney;
	
	sMoney.ProtocolType = c2s_trademovemoney;
	sMoney.m_nMoney = nMoney;
	if (g_pClient)
		g_pClient->SendPackToServer(&sMoney, sizeof(TRADE_MOVE_MONEY_COMMAND));
	
	return TRUE;
}
#endif

#ifndef _SERVER
BOOL	KPlayer::GambleMoveMoney(int nMoney)
{
	if (!CheckTrading())
		return FALSE;
	if (this->m_cTrade.m_nTradeLock)
		return FALSE;

	if (nMoney < 0 || nMoney > m_ItemList.GetEquipmentMoney() + m_ItemList.GetTradeMoney())
		return FALSE;

	GAMBLE_MOVE_MONEY_COMMAND	sMoney;

	sMoney.ProtocolType = c2s_gamblemovemoney;
	sMoney.m_nMoney = nMoney;
	if (g_pClient)
		g_pClient->SendPackToServer(&sMoney, sizeof(GAMBLE_MOVE_MONEY_COMMAND));

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	if nDecision == 0   if nDecision == 1   if nDecision == 2 
//-------------------------------------------------------------------------
void	KPlayer::TradeDecision(int nDecision)
{
	if ( !CheckTrading() )
		return;
	TRADE_DECISION_COMMAND	sDecision;
	sDecision.ProtocolType = c2s_tradedecision;
	if (nDecision == 1)
	{
		sDecision.m_btDecision = 1;
	}
	else if (nDecision == 0)
	{
		sDecision.m_btDecision = 0;
	}
	else if (nDecision == 2)
	{
		sDecision.m_btDecision = 4;
	}
	if (g_pClient)
		g_pClient->SendPackToServer(&sDecision, sizeof(TRADE_DECISION_COMMAND));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	if nDecision == 0   if nDecision == 1   if nDecision == 2 
//-------------------------------------------------------------------------
void	KPlayer::GambleDecision(int nDecision, BYTE btChoose)
{
	if (!CheckTrading())
		return;
	GAMBLE_DECISION_COMMAND	sDecision;
	sDecision.ProtocolType = c2s_gambledecision;
	if (nDecision == 1)
	{
		sDecision.m_btDecision = 1;
		sDecision.m_btChoose = btChoose;
	}
	else if (nDecision == 0)
	{
		sDecision.m_btDecision = 0;
	}
	else if (nDecision == 2)
	{
		sDecision.m_btDecision = 4;
	}
	if (g_pClient)
		g_pClient->SendPackToServer(&sDecision, sizeof(GAMBLE_DECISION_COMMAND));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void	KPlayer::TradeApplyLock(int nLockOrNot)
{
	if ( !CheckTrading() )
		return;
	TRADE_DECISION_COMMAND	sDecision;
	sDecision.ProtocolType = c2s_tradedecision;
	if (nLockOrNot)
	{
		sDecision.m_btDecision = 2;
	}
	else
	{
		sDecision.m_btDecision = 3;
	}
	if (g_pClient)
		g_pClient->SendPackToServer(&sDecision, sizeof(TRADE_DECISION_COMMAND));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void	KPlayer::GambleApplyLock(int nLockOrNot)
{
	if (!CheckTrading())
		return;
	GAMBLE_DECISION_COMMAND	sDecision;
	sDecision.ProtocolType = c2s_gambledecision;
	if (nLockOrNot)
	{
		sDecision.m_btDecision = 2;
	}
	else
	{
		sDecision.m_btDecision = 3;
	}
	if (g_pClient)
		g_pClient->SendPackToServer(&sDecision, sizeof(GAMBLE_DECISION_COMMAND));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void	KPlayer::s2cApplyAddTeam(BYTE* pProtocol)
{
	// ×´Ì¬¼ì²é (»¹ÐèÒª¼ì²é npc µÄ¶ÓÎé¿ª·Å×´Ì¬ not end)
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
	{
		ApplySelfTeamInfo();
		return;
	}
	
	// Ñ°ÕÒ npc
	PLAYER_APPLY_ADD_TEAM	*pAddTeam = (PLAYER_APPLY_ADD_TEAM*)pProtocol;
	int nNpcNo = NpcSet.SearchID(pAddTeam->m_dwTarNpcID);
	if (nNpcNo == 0)
		return;
	
	int i, nFreeListNo;
	// Èç¹ûÒÑ¾­´æÔÚ£¬¸üÐÂ
	for (i = 0; i < MAX_TEAM_APPLY_LIST; i++)
	{
		if (m_cTeam.m_sApplyList[i].m_dwNpcID == pAddTeam->m_dwTarNpcID)
		{
			m_cTeam.m_sApplyList[i].m_nLevel = Npc[nNpcNo].m_Level;
			m_cTeam.m_sApplyList[i].m_dwTimer = MAX_APPLY_TEAM_TIME;
			strcpy(m_cTeam.m_sApplyList[i].m_szName, Npc[nNpcNo].Name);
			m_cTeam.UpdateInterface();
			return;
		}
	}
	// Ñ°ÕÒÉêÇëÈËlist¿ÕÎ»
	for (i = 0; i < MAX_TEAM_APPLY_LIST; i++)
	{
		if (m_cTeam.m_sApplyList[i].m_dwNpcID == 0)
		{
			nFreeListNo = i;
			break;
		}
	}
	if (i >= MAX_TEAM_APPLY_LIST)	// Ã»ÕÒµ½ÉêÇëÈË¿ÕÎ»
		return;
	
	m_cTeam.m_sApplyList[nFreeListNo].m_dwNpcID = pAddTeam->m_dwTarNpcID;
	m_cTeam.m_sApplyList[nFreeListNo].m_nLevel = Npc[nNpcNo].m_Level;
	m_cTeam.m_sApplyList[nFreeListNo].m_dwTimer = MAX_APPLY_TEAM_TIME;
	strcpy(m_cTeam.m_sApplyList[nFreeListNo].m_szName, Npc[nNpcNo].Name);
	
	m_cTeam.UpdateInterface();
	
	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_TEAM_APPLY_ADD, Npc[nNpcNo].Name);
	sMsg.eType = SMT_TEAM;
	sMsg.byConfirmType = SMCT_UI_TEAM_APPLY;
	sMsg.byPriority = 3;
	sMsg.byParamSize = sizeof(KUiPlayerItem);

	KUiPlayerItem	player;
	strcpy(player.Name, Npc[nNpcNo].Name);
	player.nIndex = 0;
	player.uId = pAddTeam->m_dwTarNpcID;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&player);
	
	return;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
BOOL	KPlayer::AddFaction(char *lpszFactionName)
{
	if(!m_nLicReg)
		return FALSE;
	if ( !m_cFaction.AddFaction(Npc[m_nIndex].m_Series, lpszFactionName) )
		return FALSE;
	
	// ¸Ä±äÕóÓª
	Npc[m_nIndex].SetCamp(m_cFaction.GetGurFactionCamp());
	
	// Update GameTitle when adding faction
	Npc[m_nIndex].UpdateGameTitle();
	// ¸ø¿Í»§¶Ë·¢ÏûÏ¢
	SendFactionData();
	
	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£º¼ÓÈëÃÅÅÉ
//-------------------------------------------------------------------------
BOOL	KPlayer::AddFaction(int nFactionID)
{
	if(!m_nLicReg)
		return FALSE;
	if ( !m_cFaction.AddFaction(Npc[m_nIndex].m_Series, nFactionID) )
		return FALSE;
	
	// ¸Ä±äÕóÓª
	Npc[m_nIndex].SetCamp(m_cFaction.GetGurFactionCamp());
	
	// Update GameTitle when adding faction
	Npc[m_nIndex].UpdateGameTitle();
	// ¸ø¿Í»§¶Ë·¢ÏûÏ¢
	SendFactionData();
	
	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÀë¿ªÃÅÅÉ
//-------------------------------------------------------------------------
BOOL	KPlayer::LeaveCurFaction()
{
	m_cFaction.LeaveFaction();
	
	// ¸Ä±äÕóÓª
	Npc[m_nIndex].SetCamp(camp_free);
	
	// ¸ø¿Í»§¶Ë·¢ÏûÏ¢
	PLAYER_LEAVE_FACTION	sLeave;
	sLeave.ProtocolType = s2c_playerleavefaction;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sLeave, sizeof(PLAYER_LEAVE_FACTION));
	
	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£º¿ª·Åµ±Ç°ÃÅÅÉÄ³¸öµÈ¼¶µÄ¼¼ÄÜ
//-------------------------------------------------------------------------
BOOL	KPlayer::CurFactionOpenSkill(int nLevel)
{
	if ( !m_cFaction.OpenCurSkillLevel(nLevel, &Npc[m_nIndex].m_SkillList) )
		return FALSE;
	
	PLAYER_FACTION_SKILL_LEVEL	sLevel;
	sLevel.ProtocolType = s2c_playerfactionskilllevel;
	sLevel.m_btCurFactionID = m_cFaction.m_nCurFaction;
	sLevel.m_btLevel = nLevel;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sLevel, sizeof(PLAYER_FACTION_SKILL_LEVEL));
	
	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÇå¿ÕÈÎÎñÁÙÊ±±äÁ¿
//-------------------------------------------------------------------------
void	KPlayer::TaskClearTempVal()
{
	this->m_cTask.ClearTempVar();
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºµÃµ½ÈÎÎñÍê³ÉÇé¿ö
//-------------------------------------------------------------------------
DWORD		KPlayer::TaskGetSaveVal(int nNo)
{
	return this->m_cTask.GetSaveVal(nNo);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÉè¶¨ÈÎÎñÍê³ÉÇé¿ö
//-------------------------------------------------------------------------
void	KPlayer::TaskSetSaveVal(int nNo, DWORD bFlag)
{
	this->m_cTask.SetSaveVal(nNo, bFlag);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºµÃµ½ÈÎÎñÁÙÊ±¹ý³Ì¿ØÖÆ±äÁ¿Öµ
//-------------------------------------------------------------------------
DWORD		KPlayer::TaskGetClearVal(int nNo)
{
	return this->m_cTask.GetClearVal(nNo);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void	KPlayer::TaskSetClearVal(int nNo, DWORD nVal)
{
	this->m_cTask.SetClearVal(nNo, nVal);
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void	KPlayer::SetDefaultImmedSkill()
{
	//»ñÈ¡Íæ¼Ò×óÓÒ¼ü¼¼ÄÜÎªÄ¬ÈÏÎïÀí¼¼ÄÜ
	int nDetailType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponType();
	int nParticularType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponParticular();
		
	//½üÉíÎäÆ÷
	if (nDetailType == 0)
	{
		Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nMeleeWeaponSkill[nParticularType]);
		Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nMeleeWeaponSkill[nParticularType]);
	}//Ô¶³ÌÎäÆ÷
	else if (nDetailType == 1)
	{
		Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nRangeWeaponSkill[nParticularType]);
		Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nRangeWeaponSkill[nParticularType]);
	}//¿ÕÊÖ
	else if (nDetailType == -1)
	{
		Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nHandSkill);
		Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nHandSkill);
	}
}

void	KPlayer::SetLeftSkill(int nSkillID)
{
	int nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(nSkillID);
	if (nLevel <= 0) return;
	m_nLeftSkillID = nSkillID;
	
	KUiGameObject Info;
	Info.uGenre = CGOG_SKILL_SHORTCUT;
	Info.uId = m_nLeftSkillID;
	CoreDataChanged(GDCNI_PLAYER_IMMED_ITEMSKILL, (unsigned int)&Info, -1);
}

void	KPlayer::SetRightSkill(int nSkillID)
{
	int nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(nSkillID);
	if (nLevel <= 0) return;
	m_nRightSkillID = nSkillID;

	ISkill * pOrdinSkill = g_SkillManager.GetSkill(nSkillID, 1);
	if (!pOrdinSkill) 
        return;
	
	if (pOrdinSkill->IsAura())
	{
		Npc[m_nIndex].SetAuraSkill(nSkillID);
	}
	else
	{
		Npc[m_nIndex].SetAuraSkill(0);
	}
	
	KUiGameObject Info;
	Info.uGenre = CGOG_SKILL_SHORTCUT;
	Info.uId = m_nRightSkillID;
	CoreDataChanged(GDCNI_PLAYER_IMMED_ITEMSKILL, (unsigned int)&Info, -2);
}

void KPlayer::UpdateWeaponSkill()
{
	if (m_nLeftSkillID > 0)
	{
		ISkill * pISkill = g_SkillManager.GetSkill(m_nLeftSkillID, 1);
		if (!pISkill) 
            return;
		
		//if (pISkill->IsPhysical())
		//{
		//	SetLeftSkill(Npc[m_nIndex].GetCurActiveWeaponSkill());
		//}
		
	}
	if (m_nRightSkillID > 0)
	{
		ISkill * pISkill = (KSkill *) g_SkillManager.GetSkill(m_nRightSkillID, 1);
		if (!pISkill) 
            return;
		
		//if (pISkill->IsPhysical())
		//{
		//	SetRightSkill(Npc[m_nIndex].GetCurActiveWeaponSkill());
		//}
	}
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÉè¶¨µ±Ç°ÁÄÌìÆµµÀ
//-------------------------------------------------------------------------
void	KPlayer::SetChatCurChannel(int nChannelNo)
{
	m_cChat.SetCurChannel(nChannelNo);
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÑûÇë¼ÓÈë¶ÓÎé
//-------------------------------------------------------------------------
void	KPlayer::TeamInviteAdd(DWORD dwNpcID)
{
	m_cTeam.InviteAdd(dwNpcID);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÊÕµ½¿Í»§¶Ë·¢À´µÄÁÄÌìÓï¾ä
//-------------------------------------------------------------------------
void	KPlayer::ServerSendChat(BYTE* pProtocol)
{
	m_cChat.ServerSendChat(m_nPlayerIndex, pProtocol);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÊÕµ½¿Í»§¶ËÒªÇóÔö¼Ó»ù±¾ÊôÐÔµã(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
//-------------------------------------------------------------------------
void	KPlayer::AddBaseAttribute(BYTE* pProtocol)
{
	PLAYER_ADD_BASE_ATTRIBUTE_COMMAND	*pAdd = (PLAYER_ADD_BASE_ATTRIBUTE_COMMAND*)pProtocol;
	switch (pAdd->m_btAttribute)
	{
	case ATTRIBUTE_STRENGTH:
		AddBaseStrength(pAdd->m_nAddNo);
		break;
	case ATTRIBUTE_DEXTERITY:
		AddBaseDexterity(pAdd->m_nAddNo);
		break;
	case ATTRIBUTE_VITALITY:
		AddBaseVitality(pAdd->m_nAddNo);
		break;
	case ATTRIBUTE_ENGERGY:
		AddBaseEngergy(pAdd->m_nAddNo);
		break;
	}
}
void	KPlayer::ResetBaseAttribute(BYTE* pProtocol)
{
	PLAYER_ADD_BASE_ATTRIBUTE_COMMAND	*pAdd = (PLAYER_ADD_BASE_ATTRIBUTE_COMMAND*)pProtocol;
	switch (pAdd->m_btAttribute)
	{
	case ATTRIBUTE_STRENGTH:
		ResetBaseStrength(pAdd->m_nAddNo);
		break;
	case ATTRIBUTE_DEXTERITY:
		ResetBaseDexterity(pAdd->m_nAddNo);
		break;
	case ATTRIBUTE_VITALITY:
		ResetBaseVitality(pAdd->m_nAddNo);
		break;
	case ATTRIBUTE_ENGERGY:
		ResetBaseEngergy(pAdd->m_nAddNo);
		break;
	}
}
#endif

#define MAX_ORDINSKILL_LEVEL_ALWAYS  20//²»°üÀ¨ÆäËüÇé¿ö¶Ô¼¼ÄÜµÈ¼¶µÄ±ä¶¯Ö®ÍâµÄ£¬Ò»°ã×î´ó¼¼ÄÜµÈ¼¶
#ifdef _SERVER
//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KPlayer::AddSkillExp120(int nExp)
{
	for (int i = 1; i < MAX_NPCSKILL; i++)
	{
		if (Npc[m_nIndex].m_SkillList.m_Skills[i].SkillLevel <= 0)
			continue;
		if (Npc[m_nIndex].m_SkillList.m_Skills[i].SkillLevel >= MAX_TRAIN_SKILLEXPLEVEL)
			continue;
		if (Npc[m_nIndex].m_SkillList.m_Skills[i].NextSkillExp <= 0)
			continue;

		KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId, Npc[m_nIndex].m_SkillList.m_Skills[i].CurrentSkillLevel);
		if (pOrdinSkill) 
		{
			if (pOrdinSkill->IsExp() && !pOrdinSkill->IsTargetEnemy()) //Skill EXP and Skill Phu Tro
			{
				if (Npc[m_nIndex].m_SkillList.IncreaseExp(i, nExp))
					this->UpdataCurData();

				PLAYER_SKILL_LEVEL_SYNC sSkill;
				sSkill.ProtocolType = s2c_playerskilllevel;
				sSkill.m_nSkillID = Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId;
				sSkill.m_nSkillLevel = Npc[m_nIndex].m_SkillList.GetLevel(Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId);
				sSkill.m_nAddLevel = Npc[m_nIndex].m_SkillList.GetAddLevel(Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId);
				sSkill.m_nSkillExp = Npc[m_nIndex].m_SkillList.GetExp(Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId);
				sSkill.m_bTempSkill = Npc[m_nIndex].m_SkillList.IsTempSkill(Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId);
				sSkill.m_nLeavePoint = m_nSkillPoint;
				g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
			}
		}
	}	
}

void KPlayer::AddSkillExp90(int nExp)
{
	for (int i = 1; i < MAX_NPCSKILL; i++)
	{
		if (Npc[m_nIndex].m_SkillList.m_Skills[i].SkillLevel <= 0)
			continue;
		if (Npc[m_nIndex].m_SkillList.m_Skills[i].SkillLevel >= MAX_TRAIN_SKILLEXPLEVEL)
			continue;
		if (Npc[m_nIndex].m_SkillList.m_Skills[i].NextSkillExp <= 0)
			continue;

		KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId, Npc[m_nIndex].m_SkillList.m_Skills[i].CurrentSkillLevel);
		if (pOrdinSkill) 
		{
			if(Npc[m_nIndex].m_ActiveSkillID != pOrdinSkill->GetSkillId())
				continue;

			if (pOrdinSkill->IsExp() && pOrdinSkill->IsTargetEnemy()) //Skill EXP and Skill chien dau
			{
				if (Npc[m_nIndex].m_SkillList.IncreaseExp(i, nExp))
					this->UpdataCurData();

				PLAYER_SKILL_LEVEL_SYNC sSkill;
				sSkill.ProtocolType = s2c_playerskilllevel;
				sSkill.m_nSkillID = Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId;
				sSkill.m_nSkillLevel = Npc[m_nIndex].m_SkillList.GetLevel(Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId);
				sSkill.m_nAddLevel = Npc[m_nIndex].m_SkillList.GetAddLevel(Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId);
				sSkill.m_nSkillExp = Npc[m_nIndex].m_SkillList.GetExp(Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId);
				sSkill.m_bTempSkill = Npc[m_nIndex].m_SkillList.IsTempSkill(Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId);
				sSkill.m_nLeavePoint = m_nSkillPoint;
				g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
			}
		}
	}	
}

void	KPlayer::AddSkillPoint(BYTE* pProtocol)
{
	PLAYER_ADD_SKILL_POINT_COMMAND	*pAdd = (PLAYER_ADD_SKILL_POINT_COMMAND*)pProtocol;
	
	int		nSkillIndex, nSkillLevel;
	
	nSkillIndex = Npc[m_nIndex].m_SkillList.FindSame(pAdd->m_nSkillID);
	if (nSkillIndex == 0)
		return;
	nSkillLevel = Npc[m_nIndex].m_SkillList.GetLevel(pAdd->m_nSkillID);
	if (m_nSkillPoint >= pAdd->m_nAddPoint && nSkillIndex )//&& nSkillLevel + pAdd->m_nAddPoint <= MAX_SKILLLEVEL
	{
		ISkill * pSkill = NULL;

		if (nSkillLevel == 0)
		{
			pSkill =  g_SkillManager.GetSkill(pAdd->m_nSkillID, 1);
		}
		else
		{
			pSkill =  g_SkillManager.GetSkill(pAdd->m_nSkillID, nSkillLevel);
		}
		
		if (pSkill)
		{
			//ÓÐ¶ÔÓ¦µÄ¼¼ÄÜÉý¼¶½Å±¾,ÓÅÏÈÖ´ÐÐ½Å±¾
			if (pSkill->IsExp())
			{
				char szMsg[128];
				sprintf(szMsg, "Kü n¨ng %s cña b¹n chØ cã thÓ th¨ng cÊp khi tÝch lòy ®ñ kinh nghiÖm", pSkill->GetSkillName());
				KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, MESSAGE_SYSTEM_ANNOUCE_HEAD, (char *) szMsg, strlen(szMsg) );
				return;
			}
			if (pSkill->GetSkillLevelUpScriptId())
			{
				ExecuteScript(pSkill->GetSkillLevelUpScriptId(), NORMAL_FUNCTION_NAME, pAdd->m_nAddPoint);
				nSkillLevel = Npc[m_nIndex].m_SkillList.GetSkillIdxLevel(nSkillIndex);
			}
			else
			{
			/*
			¼¼ÄÜÉý¼¶µÄ³£¹æ
			1¡£ËùÓÐÄÜ¹»¼ÓµãµÄ¼¼ÄÜ£¬×î¸ßÖ»ÄÜ¼Óµ½20¼¶¡£
			2¡£¼ÙÉèÒ»¸öÍæ¼ÒµÄµÈ¼¶ÊÇx,Ò»¸ö¼¼ÄÜµÄ»ù±¾Ñ§Ï°µÈ¼¶ÊÇy£¬ÄÇÃ´ËûÄÜ¹»Ñ§Ï°Õâ¸ö¼¼ÄÜµÄ×î¸ßµÈ¼¶ÊÇx-y¡£
			3¡£Ê¹ÓÃ¼¼ÄÜÐèÒª¼ÓÈëÒ»Ð©Çé¿öµÄÅÐ¶Ï£¬µÚÒ»£¬ÊÇ·ñÊÇÆïÂí×´Ì¬£»µÚ¶þ£¬Ëû×°±¸µÄÎäÆ÷µÄÏêÏ¸Àà±ðÊÇ·ñÎªÖ¸¶¨ÊýÖµ£¬-1±íÊ¾Ã»ÓÐÐèÇó¡£
				*/
				switch(pSkill->GetSkillStyle())
				{
				case SKILL_SS_Missles:			//	×Óµ¯Àà		±¾¼¼ÄÜÓÃÓÚ·¢ËÍ×Óµ¯Àà
				case SKILL_SS_Melee:
				case SKILL_SS_InitiativeNpcState:	//	Ö÷¶¯Àà		±¾¼¼ÄÜÓÃÓÚ¸Ä±äµ±Ç°NpcµÄÖ÷¶¯×´Ì¬
				case SKILL_SS_PassivityNpcState:		//	±»¶¯Àà		±¾¼¼ÄÜÓÃÓÚ¸Ä±äNpcµÄ±»¶¯×´Ì¬
					{
						int nWantToBeLevel = nSkillLevel + pAdd->m_nAddPoint;

						if (nWantToBeLevel >= MAX_SKILLLEVEL)
						{
							return;
						}

						//Limit2
						if ((nWantToBeLevel <= g_SkillManager.GetSkillMaxLevel(pAdd->m_nSkillID) + m_cReBorn.GetReBornNum() * m_cReBorn.GetReBornSkillMaxLevel()) && nWantToBeLevel <= Npc[m_nIndex].m_Level - ((KSkill*)pSkill)->GetSkillReqLevel() + 1)
						{
							if ( !Npc[m_nIndex].m_SkillList.IncreaseLevel(nSkillIndex, pAdd->m_nAddPoint))
								return;
							m_nSkillPoint -= pAdd->m_nAddPoint;
							nSkillLevel += pAdd->m_nAddPoint;
						}
						else
						{
							if (nWantToBeLevel >= g_SkillManager.GetSkillMaxLevel(pAdd->m_nSkillID) + m_cReBorn.GetReBornNum() * m_cReBorn.GetReBornSkillMaxLevel())
							{
								char szMsg[128];
								if (pSkill->IsExp())
									sprintf(szMsg, "Kh«ng thÓ sö dông ®iÓm kü n¨ng ®Ó th¨ng cÊp kü n¨ng %s cña b¹n", pSkill->GetSkillName());
								else
									sprintf(szMsg, "Kü n¨ng %s cña B¹n ®· ®¹t cÊp tèi ®a, kh«ng cÇn th¨ng cÊp thªm n÷a.", pSkill->GetSkillName());
								KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, MESSAGE_SYSTEM_ANNOUCE_HEAD, (char *) szMsg, strlen(szMsg) );
							}
							else 
							{
								char szMsg[128];
								sprintf(szMsg, "Kü n¨ng %s cña B¹n ph¶i ®¹t ®Õn cÊp %d míi cã thÓ th¨ng cÊp.", ((KSkill*)pSkill)->GetSkillName(), ((KSkill *)pSkill)->GetSkillReqLevel() -1 +  pAdd->m_nAddPoint + nSkillLevel);
								KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, MESSAGE_SYSTEM_ANNOUCE_HEAD, (char *) szMsg, strlen(szMsg) );
							}
							
						}
					}
					break;
				case SKILL_SS_Thief:
					{
						return;
					}break;
					
				}
				
			}
			UpdataCurData();
			// °Ñµ±Ç°´Ë¼¼ÄÜµÄµãÊýºÍÊ£Óà¼¼ÄÜµã·¢¸ø¿Í»§¶Ë
			PLAYER_SKILL_LEVEL_SYNC	sSkill;
			sSkill.ProtocolType = s2c_playerskilllevel;
			sSkill.m_nSkillID = pAdd->m_nSkillID;
			sSkill.m_nSkillLevel = Npc[m_nIndex].m_SkillList.GetLevel(pAdd->m_nSkillID);
			sSkill.m_nAddLevel = Npc[m_nIndex].m_SkillList.GetAddLevel(pAdd->m_nSkillID);
			sSkill.m_nSkillExp = Npc[m_nIndex].m_SkillList.GetExp(pAdd->m_nSkillID);
			sSkill.m_bTempSkill = Npc[m_nIndex].m_SkillList.IsTempSkill(pAdd->m_nSkillID);
			sSkill.m_nLeavePoint = m_nSkillPoint;
			g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
		}
	}
}

void	KPlayer::IncSkillLevel(int nSkillId, int nAddLevel)
{	
	int		nSkillIndex, nSkillLevel;
	
	nSkillIndex = Npc[m_nIndex].m_SkillList.FindSame(nSkillId);
	if (nSkillIndex <= 0)
		return;

	nSkillLevel = Npc[m_nIndex].m_SkillList.GetLevel(nSkillId);
	if (nSkillLevel >= MAX_SKILLLEVEL)
		return;	

	if (m_nSkillPoint >= nAddLevel && nSkillIndex )//&& nSkillLevel + nAddLevel <= MAX_SKILLLEVEL
	{
		ISkill * pSkill = NULL;
		if (nSkillLevel <= 0)
		{
			pSkill =  g_SkillManager.GetSkill(nSkillId, 1);
		}
		else
		{
			pSkill =  g_SkillManager.GetSkill(nSkillId, nSkillLevel);
		}
		
		if (pSkill)
		{
			//ÓÐ¶ÔÓ¦µÄ¼¼ÄÜÉý¼¶½Å±¾,ÓÅÏÈÖ´ÐÐ½Å±¾
			/*
			¼¼ÄÜÉý¼¶µÄ³£¹æ
			1¡£ËùÓÐÄÜ¹»¼ÓµãµÄ¼¼ÄÜ£¬×î¸ßÖ»ÄÜ¼Óµ½20¼¶¡£
			2¡£¼ÙÉèÒ»¸öÍæ¼ÒµÄµÈ¼¶ÊÇx,Ò»¸ö¼¼ÄÜµÄ»ù±¾Ñ§Ï°µÈ¼¶ÊÇy£¬ÄÇÃ´ËûÄÜ¹»Ñ§Ï°Õâ¸ö¼¼ÄÜµÄ×î¸ßµÈ¼¶ÊÇx-y¡£
			3¡£Ê¹ÓÃ¼¼ÄÜÐèÒª¼ÓÈëÒ»Ð©Çé¿öµÄÅÐ¶Ï£¬µÚÒ»£¬ÊÇ·ñÊÇÆïÂí×´Ì¬£»µÚ¶þ£¬Ëû×°±¸µÄÎäÆ÷µÄÏêÏ¸Àà±ðÊÇ·ñÎªÖ¸¶¨ÊýÖµ£¬-1±íÊ¾Ã»ÓÐÐèÇó¡£
				*/
				switch(pSkill->GetSkillStyle())
				{
				case SKILL_SS_Missles:			//	×Óµ¯Àà		±¾¼¼ÄÜÓÃÓÚ·¢ËÍ×Óµ¯Àà
				case SKILL_SS_Melee:
				case SKILL_SS_InitiativeNpcState:	//	Ö÷¶¯Àà		±¾¼¼ÄÜÓÃÓÚ¸Ä±äµ±Ç°NpcµÄÖ÷¶¯×´Ì¬
				case SKILL_SS_PassivityNpcState:		//	±»¶¯Àà		±¾¼¼ÄÜÓÃÓÚ¸Ä±äNpcµÄ±»¶¯×´Ì¬
					{
						int nWantToBeLevel = nSkillLevel + nAddLevel;
						//Limit2
						if (nWantToBeLevel <= g_SkillManager.GetSkillMaxLevel(nSkillId) && nWantToBeLevel <= Npc[m_nIndex].m_Level - ((KSkill*)pSkill)->GetSkillReqLevel() + 1)
						{
							if ( !Npc[m_nIndex].m_SkillList.IncreaseLevel(nSkillIndex, nAddLevel) )
								return;
							m_nSkillPoint -= nAddLevel;
							nSkillLevel += nAddLevel;
						}
						else
						{
							if (nWantToBeLevel >=  g_SkillManager.GetSkillMaxLevel(nSkillId))
							{
								char szMsg[128];
								if (pSkill->IsExp())
									sprintf(szMsg, "Kh«ng thÓ sö dông ®iÓm kü n¨ng ®Ó th¨ng cÊp kü n¨ng %s cña b¹n", pSkill->GetSkillName());
								else
									sprintf(szMsg, "Kü n¨ng %s cña B¹n ®· ®¹t cÊp tèi ®a, kh«ng cÇn th¨ng cÊp thªm n÷a.", pSkill->GetSkillName());
								KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, MESSAGE_SYSTEM_ANNOUCE_HEAD, (char *) szMsg, strlen(szMsg) );
							}
							else 
							{
								char szMsg[128];
								sprintf(szMsg, "Kü n¨ng chiÕn ®Êu cña B¹n ph¶i ®¹t ®Õn cÊp %d míi cã thÓ th¨ng cÊp %s kü n¨ng.", ((KSkill *)pSkill)->GetSkillReqLevel() -1 +  nAddLevel + nSkillLevel,  ((KSkill*)pSkill)->GetSkillName());
								KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, MESSAGE_SYSTEM_ANNOUCE_HEAD, (char *) szMsg, strlen(szMsg) );
							}
							
						}
					}
					break;
				case SKILL_SS_Thief:
					{
						return;
					}break;
					
				}
			
			// °Ñµ±Ç°´Ë¼¼ÄÜµÄµãÊýºÍÊ£Óà¼¼ÄÜµã·¢¸ø¿Í»§¶Ë
			UpdataCurData();
			PLAYER_SKILL_LEVEL_SYNC	sSkill;
			sSkill.ProtocolType = s2c_playerskilllevel;
			sSkill.m_nSkillID = nSkillId;
			sSkill.m_nSkillLevel = Npc[m_nIndex].m_SkillList.GetLevel(nSkillId);
			sSkill.m_nAddLevel = Npc[m_nIndex].m_SkillList.GetAddLevel(nSkillId);
			sSkill.m_nSkillExp = Npc[m_nIndex].m_SkillList.GetExp(nSkillId);
			sSkill.m_bTempSkill = Npc[m_nIndex].m_SkillList.IsTempSkill(nSkillId);
			sSkill.m_nLeavePoint = m_nSkillPoint;
			g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
		}
	}
}

void	KPlayer::IncSkillExp(int nSkillId, int nAddExp)
{	
	int		nSkillIndex, nSkillLevel;
	
	nSkillIndex = Npc[m_nIndex].m_SkillList.FindSame(nSkillId);
	if (nSkillIndex <= 0)
		return;

	nSkillLevel = Npc[m_nIndex].m_SkillList.GetLevel(nSkillId);
	if (nSkillLevel >= MAX_SKILLLEVEL)
		return;

	ISkill * pSkill = NULL;
	if (nSkillLevel <= 0)
	{
		pSkill =  g_SkillManager.GetSkill(nSkillId, 1);
	}
	else
	{
		pSkill =  g_SkillManager.GetSkill(nSkillId, nSkillLevel);
	}
	
	if (pSkill)
	{
		if  (Npc[m_nIndex].m_SkillList.IncreaseExp(nSkillIndex, nAddExp))
			UpdataCurData();
		
		PLAYER_SKILL_LEVEL_SYNC	sSkill;
		sSkill.ProtocolType = s2c_playerskilllevel;
		sSkill.m_nSkillID = nSkillId;
		sSkill.m_nSkillLevel = Npc[m_nIndex].m_SkillList.GetLevel(nSkillId);
		sSkill.m_nAddLevel = Npc[m_nIndex].m_SkillList.GetAddLevel(nSkillId);
		sSkill.m_nSkillExp = Npc[m_nIndex].m_SkillList.GetExp(nSkillId);
		sSkill.m_bTempSkill = Npc[m_nIndex].m_SkillList.IsTempSkill(nSkillId);
		sSkill.m_nLeavePoint = m_nSkillPoint;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
	}
}

#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
BOOL	KPlayer::ServerPickUpItem(BYTE* pProtocol)
{
	PLAYER_PICKUP_ITEM_COMMAND	*pPickUp = (PLAYER_PICKUP_ITEM_COMMAND*)pProtocol;
	
	int		nObjIndex, nNpcX, nNpcY, nObjX, nObjY;
	nObjIndex = ObjSet.FindID(pPickUp->m_nObjID);
	AUTOLOG("SPICK-RECV objid=%d idx=%d lic=%d place=%d px=%d py=%d player=%d", pPickUp->m_nObjID, nObjIndex, (int)m_nLicReg, (int)pPickUp->m_btPosType, (int)pPickUp->m_btPosX, (int)pPickUp->m_btPosY, m_nPlayerIndex);
	if (nObjIndex == 0)
		return FALSE;
	if(!m_nLicReg)
		return FALSE;
	if (Object[nObjIndex].m_nBelong != -1)
	{
		if (!m_cTeam.m_nFlag)
		{
			AUTOLOG("SPICK-BELONG objid=%d idx=%d belong=%d me=%d btime=%d kind=%d teamflag=%d", pPickUp->m_nObjID, nObjIndex, Object[nObjIndex].m_nBelong, m_nPlayerIndex, Object[nObjIndex].m_nBelongTime, Object[nObjIndex].m_nKind, m_cTeam.m_nFlag);
			if (Object[nObjIndex].m_nBelong != m_nPlayerIndex)
			{
				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType = s2c_msgshow;
				if (Object[nObjIndex].m_nKind == Obj_Kind_Money)
					sMsg.m_wMsgID = enumMSG_ID_MONEY_CANNOT_PICKUP;
				else
					sMsg.m_wMsgID = enumMSG_ID_OBJ_CANNOT_PICKUP;
				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
				g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
				return FALSE;
			}
		}
		else
		{
			if (Object[nObjIndex].m_nKind == Obj_Kind_Money)
			{
				if (Object[nObjIndex].m_nBelong != m_nPlayerIndex &&
					!g_Team[m_cTeam.m_nID].CheckIn(Object[nObjIndex].m_nBelong))
				{
					SHOW_MSG_SYNC	sMsg;
					sMsg.ProtocolType = s2c_msgshow;
					sMsg.m_wMsgID = enumMSG_ID_MONEY_CANNOT_PICKUP;
					sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
					g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
					return FALSE;
				}
			}
			else if (Object[nObjIndex].m_nKind == Obj_Kind_Item)
			{
				if (Object[nObjIndex].m_nItemDataID <= 0 || Object[nObjIndex].m_nItemDataID >= MAX_ITEM)
				{
					//_ASSERT(0);
					return FALSE;
				}
				if (((Item[Object[nObjIndex].m_nItemDataID].GetGenre() == item_task ||
					Item[Object[nObjIndex].m_nItemDataID].GetGenre() == item_mine) && 
					Object[nObjIndex].m_nBelong != m_nPlayerIndex) ||
					!g_Team[m_cTeam.m_nID].CheckIn(Object[nObjIndex].m_nBelong) 
					//|| Item[Object[nObjIndex].m_nItemDataID].GetBindItem()->btBindItem > 0
					)
				{
					SHOW_MSG_SYNC	sMsg;
					sMsg.ProtocolType = s2c_msgshow;
					sMsg.m_wMsgID = enumMSG_ID_OBJ_CANNOT_PICKUP;
					sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
					g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
					return FALSE;
				}
			}
			else
			{
				return FALSE;
			}
		}
	}

	AUTOLOG("SPICK-WORLD objid=%d objsw=%d npcsw=%d objreg=%d npcreg=%d", pPickUp->m_nObjID, Object[nObjIndex].m_nSubWorldID, Npc[m_nIndex].m_SubWorldIndex, Object[nObjIndex].m_nRegionIdx, Npc[m_nIndex].m_RegionIndex);
	if (Object[nObjIndex].m_nSubWorldID != Npc[m_nIndex].m_SubWorldIndex)
		return FALSE;
	SubWorld[Object[nObjIndex].m_nSubWorldID].Map2Mps(
		Object[nObjIndex].m_nRegionIdx,
		Object[nObjIndex].m_nMapX,
		Object[nObjIndex].m_nMapY,
		Object[nObjIndex].m_nOffX,
		Object[nObjIndex].m_nOffY,
		&nObjX,
		&nObjY);
	SubWorld[Npc[m_nIndex].m_SubWorldIndex].Map2Mps(
		Npc[m_nIndex].m_RegionIndex,
		Npc[m_nIndex].m_MapX,
		Npc[m_nIndex].m_MapY,
		Npc[m_nIndex].m_OffX,
		Npc[m_nIndex].m_OffY,
		&nNpcX,
		&nNpcY);
	AUTOLOG("SPICK-FAR objid=%d d2=%d limit=%d npcx=%d npcy=%d objx=%d objy=%d", pPickUp->m_nObjID, ((nNpcX - nObjX) * (nNpcX - nObjX) + (nNpcY - nObjY) * (nNpcY - nObjY)), PLAYER_PICKUP_SERVER_DISTANCE, nNpcX, nNpcY, nObjX, nObjY);
	if (PLAYER_PICKUP_SERVER_DISTANCE < (nNpcX - nObjX) * (nNpcX - nObjX) + (nNpcY - nObjY) * (nNpcY - nObjY))
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_OBJ_TOO_FAR;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
	
	switch (Object[nObjIndex].m_nKind)
	{
	case Obj_Kind_Item:				
		{
			/*if(SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID == 379 && Object[nObjIndex].m_nGenre == 6) //nhÆt b¶o vËt ë map tèng kim trung cÊp
			{
				if((Object[nObjIndex].m_nParticularType >= 159 && Object[nObjIndex].m_nParticularType <= 176) || (Object[nObjIndex].m_nParticularType >= 1695 && Object[nObjIndex].m_nParticularType <= 1712))
				{
					ExecuteScript("\\script\\tinhnang\\tong_kim_tcap\\drop.lua","PickItem", Object[nObjIndex].m_nParticularType);
					Object[nObjIndex].SyncRemove(TRUE);
					if (Object[nObjIndex].m_nRegionIdx >= 0)
					{
						SubWorld[Object[nObjIndex].m_nSubWorldID].m_Region[Object[nObjIndex].m_nRegionIdx].RemoveObj(nObjIndex);
						ObjSet.Remove(nObjIndex);
					}
				}
			}
			if(Object[nObjIndex].m_nGenre == 6 && Object[nObjIndex].m_nParticularType == 212)//than bi lo chi, mat chi
			{
				ExecuteScript("\\script\\tinhnang\\datau\\matchi.lua","PickItem", Object[nObjIndex].m_nParticularType);
				Object[nObjIndex].SyncRemove(TRUE);
				if (Object[nObjIndex].m_nRegionIdx >= 0)
				{
					SubWorld[Object[nObjIndex].m_nSubWorldID].m_Region[Object[nObjIndex].m_nRegionIdx].RemoveObj(nObjIndex);
					ObjSet.Remove(nObjIndex);
				}
			}
			if(Object[nObjIndex].m_nGenre == 6 && Object[nObjIndex].m_nParticularType == 205)//than bi do chi
			{
				ExecuteScript("\\script\\tinhnang\\datau\\diadochi.lua","PickItem", nObjIndex);

				Object[nObjIndex].SyncRemove(TRUE);
				if (Object[nObjIndex].m_nRegionIdx >= 0)
				{
					SubWorld[Object[nObjIndex].m_nSubWorldID].m_Region[Object[nObjIndex].m_nRegionIdx].RemoveObj(nObjIndex);
					ObjSet.Remove(nObjIndex);
				}
			}*/
			// [DA TAU 16/08/2026] khoi phuc hook nhat cuon Da Tau nhu ban goc Linux:
			// nhat 205 (dia do chi) / 212 (mat chi) genre 6 = cong ngay vao bo dem
			// nhiem vu (PickUp cua tasklink_goods*.lua, tu chia to doi), cuon KHONG
			// vao tui. Khoi phuc tu khoi cu bi comment o tren (them return TRUE de
			// khong roi xuong AddKIL voi object da go - loi cua khoi cu).
			// FIX 16/08 lan 2: so bang DU LIEU ITEM chu khong phai truong genre cua
			// object - nhieu duong AddObject gan m_nGenre = 0 cho vat roi (KObjSet.cpp
			// :549/:602/:666) nen dieu kien cu truot, cuon van chui vao tui.
			{
				int nPickItemId = Object[nObjIndex].m_nItemDataID;
				if (nPickItemId > 0 && nPickItemId < MAX_ITEM &&
					Item[nPickItemId].GetGenre() == 6 && Item[nPickItemId].GetDetailType() == 1 &&
					(Item[nPickItemId].GetParticular() == 205 || Item[nPickItemId].GetParticular() == 212))
				{
					// [PB 17/08] kiem ket qua script: loi/chua nap -> KHONG huy cuon
					// (truoc day huy vo dieu kien = moi that bai script deu nuot cuon trang)
					BOOL bPickOk;
					if (Item[nPickItemId].GetParticular() == 205)
						bPickOk = ExecuteScript((char*)"\\script\\item\\tasklink_goods.lua", (char*)"PickUp", nObjIndex);
					else
						bPickOk = ExecuteScript((char*)"\\script\\item\\tasklink_goods_secret.lua", (char*)"PickUp", nObjIndex);
					if (!bPickOk)
						return FALSE;
					Object[nObjIndex].SyncRemove(TRUE);
					if (Object[nObjIndex].m_nRegionIdx >= 0)
					{
						SubWorld[Object[nObjIndex].m_nSubWorldID].m_Region[Object[nObjIndex].m_nRegionIdx].RemoveObj(nObjIndex);
						ObjSet.Remove(nObjIndex);
					}
					// [PB 17/08] KHONG ItemSet.Remove o day: KObj::Release (duoc
					// ObjSet.Remove goi) da tu giai phong m_nItemDataID - remove kep
					// la thua (comment cu ve ro ri pool la SAI, Release van don).
					return TRUE;
				}
			}
			int nItemIdx = m_ItemList.AddKIL(Object[nObjIndex].m_nItemDataID, pPickUp->m_btPosType, pPickUp->m_btPosX, pPickUp->m_btPosY, false, true);
			AUTOLOG("SPICK-BAG objid=%d itemidx=%d itemdata=%d place=%d px=%d py=%d maxitem=%d", pPickUp->m_nObjID, nItemIdx, Object[nObjIndex].m_nItemDataID, (int)pPickUp->m_btPosType, (int)pPickUp->m_btPosX, (int)pPickUp->m_btPosY, MAX_PLAYER_ITEM);
			if (nItemIdx <= 0 || nItemIdx >= MAX_PLAYER_ITEM)
			{
				//_ASSERT(0); //khong du khoang trong hanh trang
				return FALSE;
			}
			if (Object[nObjIndex].m_nItemDataID <= 0 || Object[nObjIndex].m_nItemDataID >= MAX_ITEM)
			{
				//_ASSERT(0);
				return FALSE;
			}

			SHOW_MSG_SYNC	sMsg;
			sMsg.ProtocolType = s2c_msgshow;
			sMsg.m_wMsgID = enumMSG_ID_GET_ITEM;
			sMsg.m_lpBuf = (LPVOID)Item[Object[nObjIndex].m_nItemDataID].GetID();
			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
			g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
			sMsg.m_lpBuf = 0;

			if (Object[nObjIndex].GetObjPickExecute())//add by phong kiÒu item sö dông ngay
			{
				m_ItemList.EatMecidine(m_ItemList.m_Items[nItemIdx].nIdx);
			}
			
			Object[nObjIndex].m_nItemDataID = 0;
			Object[nObjIndex].m_nItemWidth = 0;
			Object[nObjIndex].m_nItemHeight = 0;
			Object[nObjIndex].m_bPickExecute = FALSE;//add by phong kiÒu item sö dông ngay
			Object[nObjIndex].Remove(FALSE);
		}
		break;
	case Obj_Kind_Money:			
		if ( !Earn(Object[nObjIndex].m_nMoneyNum) )
			return FALSE;
		Object[nObjIndex].SyncRemove(TRUE);
		if (Object[nObjIndex].m_nRegionIdx >= 0)
			SubWorld[Object[nObjIndex].m_nSubWorldID].m_Region[Object[nObjIndex].m_nRegionIdx].RemoveObj(nObjIndex);
		ObjSet.Remove(nObjIndex);
		break;
	}
	
	return TRUE;
}
#endif

#ifdef _SERVER
void	KPlayer::EatItem(BYTE* pProtocol)
{
	PLAYER_EAT_ITEM_COMMAND	*pEat = (PLAYER_EAT_ITEM_COMMAND*)pProtocol;
	if(m_nLicReg)
		m_ItemList.EatMecidine(pEat->m_btPlace, pEat->m_btX, pEat->m_btY);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÊÕµ½¿Í»§¶ËÒªÇóÊ¹ÓÃÄ³¸öÎïÆ·(Êó±êÓÒ¼üµã»÷)
//-------------------------------------------------------------------------
//void	KPlayer::UseItem(BYTE* pProtocol)
//{
//	PLAYER_USE_ITEM_COMMAND	*pItem = (PLAYER_USE_ITEM_COMMAND*)pProtocol;
//}
#endif

#ifdef _SERVER
void	KPlayer::ServerMoveItem(BYTE* pProtocol)
{
	PLAYER_MOVE_ITEM_COMMAND* pMove = (PLAYER_MOVE_ITEM_COMMAND*)pProtocol;
	ItemPos		DownPos, UpPos;

	if (Item[m_ItemList.Hand()].GetDurability() == 0 && pMove->m_btDownPos == pos_equip) //#do ben trang bi vat pham hong
	{
		BYTE	byFinished = s2c_itemexchangefinish;
		if (g_pServer)
			g_pServer->PackDataToClient(m_nNetConnectIdx, &byFinished, sizeof(BYTE));
		//ExecuteScript("\\script\\player\\mgs2player_from_c.lua","main", "Trang bÞ háng söa l¹i míi sö dông ®­îc!");
		return;
	}

	if ((pMove->m_btDownPos == pos_repositoryroom || pMove->m_btUpPos == pos_repositoryroom) && !m_CUnlocked)
	{
		// Client dang co lay (hoac cho them do) vao ruong khi ruong con dang bi khoa ~> thoat ra ngay
		//ExecuteScript("\\script\\player\\mgs2player_from_c.lua","main", "Nh©n vËt ®ang trong tr¹ng th¸i khãa, kh«ng thÓ thao t¸c!");
		return;
	}
	

	//	if ((pMove->m_btDownPos == pos_expandbox || pMove->m_btUpPos == pos_expandbox) && !m_CUnlocked)
	//	{
			// Client dang co lay (hoac cho them do) vao ruong mo rong khi ruong con dang bi khoa ~> thoat ra ngay
	//		return;
	//	}

	if ((pMove->m_btDownPos == pos_equip || pMove->m_btUpPos == pos_equip) && !m_CUnlocked)
	{
		// Client dang co lay (hoac cho them do) vao trang bi khi ruong con dang bi khoa ~> thoat ra ngay
		//ExecuteScript("\\script\\player\\mgs2player_from_c.lua","main", "Nh©n vËt ®ang trong tr¹ng th¸i khãa, kh«ng thÓ thao t¸c!");
		return;
	}

	DownPos.nPlace = pMove->m_btDownPos;
	DownPos.nX = pMove->m_btDownX;
	DownPos.nY = pMove->m_btDownY;
	UpPos.nPlace = pMove->m_btUpPos;
	UpPos.nX = pMove->m_btUpX;
	UpPos.nY = pMove->m_btUpY;

	//EXCHANGE_ITEM_RESPONE	eir;
	//eir.ProtocolType = s2c_exchangerespone;

	if (this->CheckTrading() && (DownPos.nPlace == pos_traderoom|| DownPos.nPlace == pos_gambleroom))
	{
		if (this->m_cTrade.m_nTradeLock)
			return;
	}
	
	m_ItemList.ExchangeItem(&DownPos, &UpPos);
	
	BYTE	byFinished = s2c_itemexchangefinish;
	if (g_pServer)
		g_pServer->PackDataToClient(m_nNetConnectIdx, &byFinished, sizeof(BYTE));
}
#endif

#ifdef _SERVER
//implement throw items from all rooms of player
void	KPlayer::ServerThrowAllItem()
{
	//go through all m_Room and throw
	PlayerItem* pItem = m_ItemList.GetFirstItem();
	KMapPos		sMapPos;
	while (true)
	{
		if (pItem == NULL)
			break;
		int nItemIdx = pItem->nIdx;
		if (nItemIdx > 0 && nItemIdx < MAX_PLAYER_ITEM)
		{
			GetAboutPos(&sMapPos);
			if (m_ItemList.Remove(nItemIdx))
			{
				int		nObj;
				KObjItemInfo	sInfo;
				sInfo.m_nItemID = nItemIdx;
				sInfo.m_nItemWidth = Item[nItemIdx].GetWidth();
				sInfo.m_nItemHeight = Item[nItemIdx].GetHeight();
				sInfo.m_nMoneyNum = 0;
				char szNameTemp[OBJ_NAME_LENGHT];
				if (Item[nItemIdx].GetGenre() != item_equip && Item[nItemIdx].GetStackNum() > 1)
				{
					sprintf(szNameTemp, "%s x %d", Item[nItemIdx].GetName(), Item[nItemIdx].GetStackNum());
					strcpy(sInfo.m_szName, szNameTemp);
				}
				else
					strcpy(sInfo.m_szName, Item[nItemIdx].GetName());
				sInfo.m_nColorID = 0;
				sInfo.m_nGenre = Item[nItemIdx].GetGenre();
				sInfo.m_nDetailType = Item[nItemIdx].GetDetailType();
				sInfo.m_nParticularType = Item[nItemIdx].GetParticular();
				sInfo.m_nMovieFlag = 1;
				sInfo.m_nSoundFlag = 1;
				sInfo.m_dwNpcId1 = Npc[m_nIndex].m_dwID;

				if (Item[nItemIdx].GetTimeYearIsExp() == FALSE)//Xu ly vat pham het han su dung vut ra mat luon
				{
					nObj = ObjSet.Add(Item[nItemIdx].GetObjIdx(), sMapPos, sInfo);
					if (nObj >= 0)
					{
						Object[nObj].SetItemBelong(-1);
						Object[nObj].SetLifeTime(3*60*18); //3 phut
					}
				}
				pItem = m_ItemList.GetFirstItem();
			}
			else
			{
				pItem = m_ItemList.GetNextItem();
			}
		}
		else
			break;
	}
}
void	KPlayer::ServerThrowAllItem(BYTE* pProtocol) //NÐm hÕt vËt phÈm ra ngoµi theo tªn
{	
	PLAYER_THROW_ALL_ITEM_COMMAND	*pThrow = (PLAYER_THROW_ALL_ITEM_COMMAND*)pProtocol;
	KMapPos		sMapPos;
	
	if (!m_CUnlocked)
		return;

	int	nHandIdx = m_ItemList.SearchID(pThrow->m_nItemDwId);
	int nItemIdx = nHandIdx;
	if (!m_ItemList.FindSame(nItemIdx))//add by phong kiÒu antihack
	{
		printf("Hack ThrowAwayItem m_Hand 1 [%s] [%s]\n",m_AccoutName,m_PlayerName);
		return;
	}
	if (!m_ItemList.CheckItemInAll(nItemIdx))
	{
		printf("Hack ThrowAwayItem m_Hand 2 [%s] [%s]\n",m_AccoutName,m_PlayerName);
		return;
	}
	//
	int nListItemIdx[60];
	memset(nListItemIdx, 0, sizeof(nListItemIdx));
	m_ItemList.m_Room[room_equipment].FindSameItemName(nHandIdx, nListItemIdx);
	//
	for(int index = 0; index < 60; index ++)
	{
		nHandIdx = nListItemIdx[index]; //LÊy l¹i tõng itemidx trong m¶ng
		nItemIdx = nListItemIdx[index];
		if (nHandIdx > 0)
		{
			if (nHandIdx && Item[nHandIdx].GetGoldId()
				|| nHandIdx && Item[nHandIdx].GetNature() >= NATURE_GOLD
				|| nHandIdx && Item[nHandIdx].IsPurple() 
				|| nHandIdx && Item[nHandIdx].GetGenre() == item_task 
				|| nHandIdx && Item[nHandIdx].m_nCurrentDur == 0)
				return;
			if (nHandIdx && (Item[nHandIdx].InsuranceCourse > 0 || Item[nHandIdx].InsuranceCourse == -2 || Item[nHandIdx].InsuranceHourCourse > 0))
				return;

			GetAboutPos(&sMapPos);
			if (m_ItemList.Remove(nItemIdx))
			{
				int		nObj;
				KObjItemInfo	sInfo;
				sInfo.m_nItemID = nItemIdx;
				sInfo.m_nItemWidth = Item[nItemIdx].GetWidth();
				sInfo.m_nItemHeight = Item[nItemIdx].GetHeight();
				sInfo.m_nMoneyNum = 0;
				char szNameTemp[OBJ_NAME_LENGHT];
				if (Item[nItemIdx].GetGenre() != item_equip && Item[nItemIdx].GetStackNum() > 1)
				{
					sprintf(szNameTemp, "%s x %d", Item[nItemIdx].GetName(), Item[nItemIdx].GetStackNum());
					strcpy(sInfo.m_szName, szNameTemp);
				}
				else
					strcpy(sInfo.m_szName, Item[nItemIdx].GetName());
				sInfo.m_nColorID = 0;
				sInfo.m_nGenre = Item[nItemIdx].GetGenre();
				sInfo.m_nDetailType = Item[nItemIdx].GetDetailType();
				sInfo.m_nParticularType = Item[nItemIdx].GetParticular();
				sInfo.m_nMovieFlag = 1;
				sInfo.m_nSoundFlag = 1;
				sInfo.m_dwNpcId1 = Npc[m_nIndex].m_dwID;

				if(Item[nHandIdx].GetTimeYearIsExp() == FALSE)//Xu ly vat pham het han su dung vut ra mat luon
				{
					nObj = ObjSet.Add(Item[nItemIdx].GetObjIdx(), sMapPos, sInfo);
					if (nObj >= 0)
					{
						if (Item[nItemIdx].GetGenre() == item_task)
						{
							Object[nObj].SetEntireBelong(this->m_nPlayerIndex);
						}
						else
						{
							Object[nObj].SetItemBelong(-1);
						}
					}			
				}
			}
		}
	}
	memset(nListItemIdx, 0, sizeof(nListItemIdx));
}

void	KPlayer::ServerThrowAwayItem(BYTE* pProtocol)
{
	PLAYER_THROW_AWAY_ITEM_COMMAND	*pThrow = (PLAYER_THROW_AWAY_ITEM_COMMAND*)pProtocol;
	KMapPos		sMapPos;
	
	if (!m_CUnlocked)
		return;
		
	if (!this->m_ItemList.Hand())
		return;
	

	int nItemIdx = m_ItemList.m_Hand;
	if (!m_ItemList.FindSame(nItemIdx))//add by phong kiÒu antihack
	{
		printf("Hack ThrowAwayItem m_Hand 1 [%s] [%s]\n",m_AccoutName,m_PlayerName);
		return;
	}
	if (!m_ItemList.CheckItemInAll(nItemIdx))
	{
		printf("Hack ThrowAwayItem m_Hand 2 [%s] [%s]\n",m_AccoutName,m_PlayerName);
		return;
	}

	int	nHandIdx = this->m_ItemList.Hand();
	if (SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID == 996 || SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID == 997) //map PUBG cho vut do
		goto PUBG;
	if (nHandIdx && Item[nHandIdx].GetGoldId() 
		|| nHandIdx && Item[nHandIdx].GetNature() >= NATURE_GOLD
		|| nHandIdx && Item[nHandIdx].IsPurple() 
		|| nHandIdx && Item[nHandIdx].GetGenre() == item_task 
		|| nHandIdx && Item[nHandIdx].m_nCurrentDur == 0)
		return;
	if (nHandIdx && (Item[nHandIdx].InsuranceCourse > 0 || Item[nHandIdx].InsuranceCourse == -2 || Item[nHandIdx].InsuranceHourCourse > 0))
		return;
PUBG:	
	GetAboutPos(&sMapPos);
	if (m_ItemList.Remove(nItemIdx))
	{
		int		nObj;
		KObjItemInfo	sInfo;
		sInfo.m_nItemID = nItemIdx;
		sInfo.m_nItemWidth = Item[nItemIdx].GetWidth();
		sInfo.m_nItemHeight = Item[nItemIdx].GetHeight();
		sInfo.m_nMoneyNum = 0;
		char szNameTemp[OBJ_NAME_LENGHT];
		if (Item[nItemIdx].GetGenre() != item_equip && Item[nItemIdx].GetStackNum() > 1)
		{
			sprintf(szNameTemp, "%s x %d", Item[nItemIdx].GetName(), Item[nItemIdx].GetStackNum());
			strcpy(sInfo.m_szName, szNameTemp);
		}
		else
			strcpy(sInfo.m_szName, Item[nItemIdx].GetName());
		sInfo.m_nColorID = 0;
		sInfo.m_nGenre = Item[nItemIdx].GetGenre();
		sInfo.m_nDetailType = Item[nItemIdx].GetDetailType();
		sInfo.m_nParticularType = Item[nItemIdx].GetParticular();
		sInfo.m_nMovieFlag = 1;
		sInfo.m_nSoundFlag = 1;
		sInfo.m_dwNpcId1 = Npc[m_nIndex].m_dwID;

		if(Item[nHandIdx].GetTimeYearIsExp() == FALSE)//Xu ly vat pham het han su dung vut ra mat luon
		{
			nObj = ObjSet.Add(Item[nItemIdx].GetObjIdx(), sMapPos, sInfo);
			if (nObj >= 0)
			{
				if (Item[nItemIdx].GetGenre() == item_task)
				{
					Object[nObj].SetEntireBelong(this->m_nPlayerIndex);
				}
				else
				{
					Object[nObj].SetItemBelong(-1);
				}
			}			
		}
	}
}
#endif

#ifdef _SERVER
void	KPlayer::ChatSetTakeChannel(BYTE* pProtocol)
{
	CHAT_SET_CHANNEL_COMMAND	*pChannel = (CHAT_SET_CHANNEL_COMMAND*)pProtocol;
	this->m_cChat.SetTakeChannel(pChannel->m_dwTakeChannel);
}
#endif

#ifdef _SERVER
void	KPlayer::ChatTransmitApplyAddFriend(BYTE* pProtocol)
{
	CHAT_APPLY_ADD_FRIEND_COMMAND	*pAdd = (CHAT_APPLY_ADD_FRIEND_COMMAND*)pProtocol;
	int	nPlayerIdx = FindAroundPlayer(pAdd->m_dwTargetNpcID);
	if (nPlayerIdx < 0)
		return;
	
	m_cChat.m_nAddFriendList[m_cChat.m_nListPos++] = nPlayerIdx;
	m_cChat.m_nListPos %= CHAT_APPLY_ADD_FRIEND_LIST;
	
	CHAT_APPLY_ADD_FRIEND_SYNC	sAdd;
	sAdd.ProtocolType = s2c_chatapplyaddfriend;
	sAdd.m_nSrcPlayerIdx = m_nPlayerIndex;
	strcpy(sAdd.m_szSourceName, Npc[m_nIndex].Name);
	memset(sAdd.m_szInfo, 0, sizeof(sAdd.m_szInfo));
	memcpy(sAdd.m_szInfo, pAdd->m_szInfo, pAdd->m_wLength + 1 + sizeof(pAdd->m_szInfo) - sizeof(CHAT_APPLY_ADD_FRIEND_COMMAND));
	g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sAdd, sizeof(CHAT_APPLY_ADD_FRIEND_SYNC));
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÊÕµ½¿Í»§¶ËÏûÏ¢Ìí¼ÓÁÄÌìºÃÓÑ
//-------------------------------------------------------------------------
BOOL	KPlayer::ChatAddFriend(BYTE* pProtocol)
{
	CHAT_ADD_FRIEND_COMMAND	*pAdd = (CHAT_ADD_FRIEND_COMMAND*)pProtocol;
	
	if (pAdd->m_nTargetPlayerIdx < 0 || pAdd->m_nTargetPlayerIdx >= MAX_PLAYER)
		goto AddFalse;
	if ( !Player[pAdd->m_nTargetPlayerIdx].m_nIndex )
		goto AddFalse;
	int		i;
	for (i = 0; i < CHAT_APPLY_ADD_FRIEND_LIST; i++)
	{
		if (m_nPlayerIndex == Player[pAdd->m_nTargetPlayerIdx].m_cChat.m_nAddFriendList[i])
			break;
	}
	if (i >= CHAT_APPLY_ADD_FRIEND_LIST)
		goto AddFalse;
	
	m_cChat.AddFriendData(m_nPlayerIndex, pAdd->m_nTargetPlayerIdx);
	Player[pAdd->m_nTargetPlayerIdx].m_cChat.AddFriendData(pAdd->m_nTargetPlayerIdx, m_nPlayerIndex);
	
	return TRUE;
	
AddFalse:
	CHAT_ADD_FRIEND_FAIL_SYNC	sFail;
	sFail.ProtocolType = s2c_chataddfriendfail;
	sFail.m_nTargetPlayerIdx = pAdd->m_nTargetPlayerIdx;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sFail, sizeof(CHAT_ADD_FRIEND_FAIL_SYNC));
	
	return FALSE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÊÕµ½¿Í»§¶ËÏûÏ¢¾Ü¾øÌí¼ÓÁÄÌìºÃÓÑ
//-------------------------------------------------------------------------
void	KPlayer::ChatRefuseFriend(BYTE* pProtocol)
{
	CHAT_REFUSE_FRIEND_COMMAND	*pRefuse = (CHAT_REFUSE_FRIEND_COMMAND*)pProtocol;
	
	if (pRefuse->m_nTargetPlayerIdx < 0 || pRefuse->m_nTargetPlayerIdx >= MAX_PLAYER)
		return;
	if (Player[pRefuse->m_nTargetPlayerIdx].m_nIndex == 0)
		return;
	for (int i = 0; i < CHAT_APPLY_ADD_FRIEND_LIST; i++)
	{
		if (m_nPlayerIndex == Player[pRefuse->m_nTargetPlayerIdx].m_cChat.m_nAddFriendList[i])
		{
			CHAT_REFUSE_FRIEND_SYNC	sRefuse;
			sRefuse.ProtocolType = s2c_chatrefusefriend;
			g_StrCpy(sRefuse.m_szName, Npc[m_nIndex].Name);
			sRefuse.m_wLength = sizeof(CHAT_REFUSE_FRIEND_SYNC) - 1 - sizeof(sRefuse.m_szName) + strlen(sRefuse.m_szName);
			
			g_pServer->PackDataToClient(Player[pRefuse->m_nTargetPlayerIdx].m_nNetConnectIdx, (BYTE*)&sRefuse, sRefuse.m_wLength + 1);
			return;
		}
	}
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÍæ¼ÒµÇÂ¼Ê±ÊÕµ½¿Í»§¶ËÇëÇóÖØÐÂ·¢ËÍËùÓÐÁÄÌìºÃÓÑÊý¾Ý£¨´øÃû×Ö£©
//-------------------------------------------------------------------------
void	KPlayer::ChatResendAllFriend(BYTE* pProtocol)
{
	if ( m_cChat.m_nSyncState != 1)
		return;
	m_cChat.m_nSyncState = 2;
	m_cChat.m_pStateSendPos = (CChatFriend*)m_cChat.m_cFriendTeam[0].m_cEveryOne.GetHead();
	if ( !m_cChat.m_pStateSendPos )
		m_cChat.m_nSyncState = 0;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÍæ¼ÒµÇÂ¼Ê±ÊÕµ½¿Í»§¶ËÇëÇó·¢ËÍÄ³¸öÁÄÌìºÃÓÑÊý¾Ý£¨´øÃû×Ö£©
//-------------------------------------------------------------------------
void	KPlayer::ChatSendOneFriendData(BYTE* pProtocol)
{
	CHAT_APPLY_SEND_ONE_FRIEND_NAME_COMMAND	*pApply = (CHAT_APPLY_SEND_ONE_FRIEND_NAME_COMMAND*)pProtocol;
	
	m_cChat.ResendOneFriendData(pApply->m_dwID, m_nPlayerIndex);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÊÕµ½¿Í»§¶ËÉêÇëÉ¾³ýÄ³¸öÁÄÌìºÃÓÑ
//-------------------------------------------------------------------------
void	KPlayer::ChatDeleteFriend(BYTE* pProtocol)
{
	CHAT_DELETE_FRIEND_COMMAND	*pDelete = (CHAT_DELETE_FRIEND_COMMAND*)pProtocol;
	
	m_cChat.DeleteFriend(pDelete->m_dwID, m_dwID);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÊÕµ½¿Í»§¶ËÉêÇëÔÙ´ÎÉ¾³ýÄ³¸öÁÄÌìºÃÓÑ
//-------------------------------------------------------------------------
void	KPlayer::ChatRedeleteFriend(BYTE* pProtocol)
{
	CHAT_REDELETE_FRIEND_COMMAND	*pDelete = (CHAT_REDELETE_FRIEND_COMMAND*)pProtocol;
	
	this->m_cChat.RedeleteFriend(pDelete->m_dwID, m_nPlayerIndex);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÊÕµ½¿Í»§¶ËÉêÇë½øÈë´ý½»Ò××´Ì¬
//-------------------------------------------------------------------------
void	KPlayer::TradeApplyOpen(BYTE* pProtocol)
{
	if ( CheckTrading() )
		return;
//	if (!m_CUnlocked)
//		return;
	TRADE_APPLY_OPEN_COMMAND	*pApply = (TRADE_APPLY_OPEN_COMMAND*)pProtocol;
	char	szSentence[MAX_SENTENCE_LENGTH];
	int		nLength = pApply->m_wLength - (sizeof(TRADE_APPLY_OPEN_COMMAND) - 1 - sizeof(pApply->m_szSentence));
	if (nLength >= MAX_SENTENCE_LENGTH)
		nLength = MAX_SENTENCE_LENGTH - 1;
	memset(szSentence, 0, sizeof(szSentence));
	memcpy(szSentence, pApply->m_szSentence, nLength);
	
	switch (m_cMenuState.m_nState)
	{
	case PLAYER_MENU_STATE_NORMAL:
	case PLAYER_MENU_STATE_TRADEOPEN:
		m_cMenuState.SetState(m_nPlayerIndex, PLAYER_MENU_STATE_TRADEOPEN, szSentence, nLength);
		break;
	case PLAYER_MENU_STATE_TEAMOPEN:
		if (m_cTeam.m_nFlag && m_cTeam.m_nID >= 0)
			g_Team[m_cTeam.m_nID].SetTeamClose();
		m_cMenuState.SetState(m_nPlayerIndex, PLAYER_MENU_STATE_TRADEOPEN, szSentence, nLength);
		break;
	}
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
// OTT
//-------------------------------------------------------------------------
void	KPlayer::GambleApplyOpen(BYTE* pProtocol)
{
	if (CheckTrading())
		return;
		
	if (!m_CUnlocked)
		return;
		
	GAMBLE_APPLY_OPEN_COMMAND* pApply = (GAMBLE_APPLY_OPEN_COMMAND*)pProtocol;
	char	szSentence[MAX_SENTENCE_LENGTH];
	int		nLength = pApply->m_wLength - (sizeof(GAMBLE_APPLY_OPEN_COMMAND) - 1 - sizeof(pApply->m_szSentence));
	if (nLength >= MAX_SENTENCE_LENGTH)
		nLength = MAX_SENTENCE_LENGTH - 1;
	memset(szSentence, 0, sizeof(szSentence));
	memcpy(szSentence, pApply->m_szSentence, nLength);

	switch (m_cMenuState.m_nState)
	{
	case PLAYER_MENU_STATE_NORMAL:
	case PLAYER_MENU_STATE_GAMBLEOPEN:
		m_cMenuState.SetState(m_nPlayerIndex, PLAYER_MENU_STATE_GAMBLEOPEN, szSentence, nLength);
		break;
	case PLAYER_MENU_STATE_TEAMOPEN:
		if (m_cTeam.m_nFlag && m_cTeam.m_nID >= 0)
			g_Team[m_cTeam.m_nID].SetTeamClose();
		m_cMenuState.SetState(m_nPlayerIndex, PLAYER_MENU_STATE_GAMBLEOPEN, szSentence, nLength);
		break;
	}
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÊÕµ½¿Í»§¶ËÉêÇëÈ¡Ïû´ý½»Ò××´Ì¬
//-------------------------------------------------------------------------
void	KPlayer::TradeApplyClose(BYTE* pProtocol)
{
	m_cMenuState.SetState(m_nPlayerIndex, PLAYER_MENU_STATE_NORMAL);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÊÕµ½¿Í»§¶ËÉêÇëÈ¡Ïû´ý½»Ò××´Ì¬
//-------------------------------------------------------------------------
void	KPlayer::GambleApplyClose(BYTE* pProtocol)
{
	m_cMenuState.SetState(m_nPlayerIndex, PLAYER_MENU_STATE_NORMAL);
}
#endif

#ifdef _SERVER
void	KPlayer::TradeApplyStart(BYTE* pProtocol)
{
	if (this->CheckTrading())
		return;
	if (!m_CUnlocked)
	{
		KPlayerChat::SendSystemInfo(1,m_nPlayerIndex, MESSAGE_SYSTEM_ANNOUCE_HEAD, "Kh«ng thÓ giao dÞch khi ch­a më khãa !", strlen("Kh«ng thÓ giao dÞch khi ch­a më khãa !") );
		return;
	}
	if (m_cTeam.m_nFlag && m_cTeam.m_nID >= 0)//add by phong kiÒu antihack
	{
		KPlayerChat::SendSystemInfo(1,m_nPlayerIndex, MESSAGE_SYSTEM_ANNOUCE_HEAD, "Kh«ng thÓ giao dÞch khi ®ang trong tæ ®éi !", strlen("Kh«ng thÓ giao dÞch khi ®ang trong tæ ®éi !") );
		return;
	}
	
	TRADE_APPLY_START_COMMAND	*pStart = (TRADE_APPLY_START_COMMAND*)pProtocol;
	
	int		nPlayer;
	nPlayer = FindAroundPlayer(pStart->m_dwID);
	
	if (nPlayer < 0)
		return;
	if (Player[nPlayer].m_cMenuState.m_nState != PLAYER_MENU_STATE_TRADEOPEN)
		return;

	for (int i=0;i<MAX_PLAYER;i++)
	{
		if (i != m_nPlayerIndex && !IsExchangingServer() && Player[i].m_cTrade.m_nTradeDest == m_nPlayerIndex)
		{
			printf("Xay ra hack lua giao dich, tu dong xua loi !\n");
			this->ExecuteScript("\\script\\log_game\\log_gayroi.lua","main",0);
			return;
		}
	}

	this->m_cTrade.m_nApplyIdx = nPlayer;
	
	TRADE_APPLY_START_SYNC	sApply;
	sApply.ProtocolType = s2c_tradeapplystart;
	sApply.m_nDestIdx = this->m_nPlayerIndex;
	sApply.m_dwNpcId = Npc[m_nIndex].m_dwID;
	if (g_pServer)
		g_pServer->PackDataToClient(Player[nPlayer].m_nNetConnectIdx, (BYTE*)&sApply, sizeof(TRADE_APPLY_START_SYNC));
}
#endif

#ifdef _SERVER
void	KPlayer::GambleApplyStart(BYTE* pProtocol)
{
	if (this->CheckTrading())
		return;

	if (m_cTeam.m_nFlag && m_cTeam.m_nID >= 0)//add by phong kiÒu antihack
	{
		KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, MESSAGE_SYSTEM_ANNOUCE_HEAD, "Kh«ng thÓ giao dÞch khi ®ang trong tæ ®éi !", strlen("Kh«ng thÓ giao dÞch khi ®ang trong tæ ®éi !"));
		return;
	}

	if (!m_CUnlocked)
	{
		KPlayerChat::SendSystemInfo(1,m_nPlayerIndex, MESSAGE_SYSTEM_ANNOUCE_HEAD, "Kh«ng thÓ giao dÞch khi ch­a më khãa !", strlen("Kh«ng thÓ giao dÞch khi ch­a më khãa !") );
		return;
	}
	GAMBLE_APPLY_START_COMMAND* pStart = (GAMBLE_APPLY_START_COMMAND*)pProtocol;

	int		nPlayer;
	nPlayer = FindAroundPlayer(pStart->m_dwID);
	if (nPlayer < 0)
		return;
	//if (Player[nPlayer].m_cMenuState.m_nState != PLAYER_MENU_STATE_GAMBLEOPEN)
	//	return;

	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (i != m_nPlayerIndex && !IsExchangingServer() && Player[i].m_cTrade.m_nTradeDest == m_nPlayerIndex)
		{
			printf("Xay ra hack lua OTT, tu dong xua loi !\n");
			this->ExecuteScript("\\script\\log_game\\log_gayroi.lua", "main", 0);
			return;
		}
	}

	this->m_cTrade.m_nApplyIdx = nPlayer;

	GAMBLE_APPLY_START_SYNC	sApply;
	sApply.ProtocolType = s2c_gambleapplystart;
	sApply.m_nDestIdx = this->m_nPlayerIndex;
	sApply.m_dwNpcId = Npc[m_nIndex].m_dwID;
	if (g_pServer)
		g_pServer->PackDataToClient(Player[nPlayer].m_nNetConnectIdx, (BYTE*)&sApply, sizeof(GAMBLE_APPLY_START_SYNC));
}
#endif

#ifdef _SERVER

void	KPlayer::TradeMoveMoney(BYTE* pProtocol)
{
	if ( !CheckTrading() )
		return;
	if (this->m_cTrade.m_nTradeLock)
		return;
	
	int		nMoney;
	TRADE_MOVE_MONEY_COMMAND	*pMoney = (TRADE_MOVE_MONEY_COMMAND*)pProtocol;
	if (pMoney->m_nMoney < 0 || pMoney->m_nMoney > m_ItemList.GetEquipmentMoney() + m_ItemList.GetTradeMoney())
		nMoney = 0;
	else
		nMoney = pMoney->m_nMoney;
	m_ItemList.TradeMoveMoney(nMoney);
}
#endif

#ifdef _SERVER

void	KPlayer::GambleMoveMoney(BYTE* pProtocol)
{
	if (!CheckTrading())
		return;
	if (this->m_cTrade.m_nTradeLock)
		return;

	int		nMoney;
	GAMBLE_MOVE_MONEY_COMMAND* pMoney = (GAMBLE_MOVE_MONEY_COMMAND*)pProtocol;
	if (pMoney->m_nMoney < 0 || pMoney->m_nMoney > m_ItemList.GetEquipmentMoney() + m_ItemList.GetTradeMoney())
		nMoney = 0;
	else
		nMoney = pMoney->m_nMoney;
	m_ItemList.GambleMoveMoney(nMoney);
}
#endif

#ifdef _SERVER
void	KPlayer::TradeDecision(BYTE* pProtocol)
{
	if ( !CheckTrading() )
		return;
		
	
	
	TRADE_DECISION_COMMAND	*pDecision = (TRADE_DECISION_COMMAND*)pProtocol;
	
	if (pDecision->m_btDecision == 2)	
	{
		if (!m_CUnlocked)
		return;
		
		if (this->m_cTrade.m_nTradeLock == 0)
		{
			this->m_cTrade.m_nTradeLock = 1;
			this->m_cTrade.m_nTradeState = 0;
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeState = 0;
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_uTradeOkTime = GetTickCount();
		}
		
		SyncTradeState();
		return;
	}
	else if (pDecision->m_btDecision == 3)	
	{
		if (!m_CUnlocked)
		return;
		
		if (this->m_cTrade.m_nTradeLock == 1)
		{
			this->m_cTrade.m_nTradeLock = 0;
			this->m_cTrade.m_nTradeState = 0;
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeLock = 0;
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeState = 0;
		}
		SyncTradeState();
		return;
	}
	else if (pDecision->m_btDecision == 4)	
	{
		if (!m_CUnlocked)
		return;
		
		if (m_cTrade.m_nTradeLock == 1 && Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeLock == 1)
		{
			m_cTrade.m_nTradeState = 0;
			SyncTradeState();
		}
		return;
	}
	else if (pDecision->m_btDecision == 0)	
	{
		int	nDestIdx;
		nDestIdx = this->m_cTrade.m_nTradeDest;

		this->m_ItemList.RecoverTrade();
		this->m_ItemList.ClearRoom(room_trade);
		this->m_ItemList.ClearRoom(room_trade1);
		this->m_cTrade.Release();
		Player[nDestIdx].m_ItemList.RecoverTrade();
		Player[nDestIdx].m_ItemList.ClearRoom(room_trade);
		Player[nDestIdx].m_ItemList.ClearRoom(room_trade1);
		Player[nDestIdx].m_cTrade.Release();
		
		TRADE_DECISION_SYNC	sSync;
		sSync.ProtocolType = s2c_tradedecision; //kÕt thóc giao dÞch thÊt b¹i
		sSync.m_btDecision = 0;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(TRADE_DECISION_SYNC));
		g_pServer->PackDataToClient(Player[nDestIdx].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(TRADE_DECISION_SYNC));
		
		this->m_cMenuState.RestoreBackupState(m_nPlayerIndex);
		
		Player[nDestIdx].m_cMenuState.RestoreBackupState(nDestIdx);
		return;
	}
	else if (pDecision->m_btDecision == 1)	
	{
		if (!m_CUnlocked)
		return;
		
		if (this->m_cTrade.m_nTradeLock != 1 ||
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeLock != 1)
		{
			SyncTradeState();
			return;
		}
		else if (Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeState == 0)
		{
			if (GetTickCount() - m_cTrade.m_uTradeOkTime < 5000)
				return;
			m_cTrade.m_nTradeState = 1;
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_uTradeOkTime = GetTickCount();
			SyncTradeState();
			return;
		}
		else
		{
			if (GetTickCount() - m_cTrade.m_uTradeOkTime < 5000)
				return;
			// -------------------- money --------------------
			int	nSM1, nSM2, nDM1, nDM2;
			int	nDestIdx = m_cTrade.m_nTradeDest;
			nSM1 = m_ItemList.GetMoney(room_equipment);
			nSM2 = m_ItemList.GetMoney(room_trade);
			nDM1 = Player[nDestIdx].m_ItemList.GetMoney(room_equipment);
			nDM2 = Player[nDestIdx].m_ItemList.GetMoney(room_trade);
			if (nSM2 < 0 || nSM1 + nDM2 - nSM2 < 0)
			{
				this->m_ItemList.TradeMoveMoney(0);
				// not end
				return;
			}
			if (nDM2 < 0 || nDM1 + nSM2 - nDM2 < 0)
			{
				Player[nDestIdx].m_ItemList.TradeMoveMoney(0);
				// not end
				return;
			}
			// ------------------  money end ------------------
			
			// ---------  ---------
			int		nSelfCanGet, nDestCanGet;
			
			// 
			Player[nDestIdx].m_ItemList.GetTradeRoomItemInfo();
			nSelfCanGet = m_ItemList.TradeCheckCanPlace();
			ItemSet.BackItemInfo();
			
			// 
			this->m_ItemList.GetTradeRoomItemInfo();
			nDestCanGet = Player[nDestIdx].m_ItemList.TradeCheckCanPlace();
		
			if (!nSelfCanGet || !nDestCanGet)
			{
				if (!nSelfCanGet)
				{
					this->m_cTrade.m_nTradeState = 0;
					
					SHOW_MSG_SYNC	sMsg;
					sMsg.ProtocolType = s2c_msgshow;
					sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
					sMsg.m_wMsgID = enumMSG_ID_TRADE_SELF_ROOM_FULL;
					g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
					sMsg.m_wMsgID = enumMSG_ID_TRADE_DEST_ROOM_FULL;
					g_pServer->PackDataToClient(Player[nDestIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
				}
				if (!nDestCanGet)
				{
					Player[nDestIdx].m_cTrade.m_nTradeState = 0;
					
					SHOW_MSG_SYNC	sMsg;
					sMsg.ProtocolType = s2c_msgshow;
					sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
					sMsg.m_wMsgID = enumMSG_ID_TRADE_SELF_ROOM_FULL;
					g_pServer->PackDataToClient(Player[nDestIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
					sMsg.m_wMsgID = enumMSG_ID_TRADE_DEST_ROOM_FULL;
					g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
				}
				SyncTradeState();
				return;
			}
			// -------  end -------
			
			TRADE_DECISION_SYNC	sSync;
			sSync.ProtocolType = s2c_tradedecision;//kÕt thóc giao dÞch thµnh c«ng
			sSync.m_btDecision = 1;
			g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(TRADE_DECISION_SYNC));
			g_pServer->PackDataToClient(Player[nDestIdx].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(TRADE_DECISION_SYNC));
			
			m_cMenuState.SetState(m_nPlayerIndex, PLAYER_MENU_STATE_NORMAL);
			Player[nDestIdx].m_cMenuState.SetState(nDestIdx, PLAYER_MENU_STATE_NORMAL);
			this->m_cTrade.Release();
			Player[nDestIdx].m_cTrade.Release();

			ExecuteScript2("\\script\\log_game\\log_giaodich.lua","linebegin",nDestIdx,false);//add by phong kiÒu log gd
			int moneygd = 0;

			if (nSM2 <= nDM2)
			{
				this->Earn(nDM2 - nSM2);
				Player[nDestIdx].Pay(nDM2 - nSM2);
				moneygd = nDM2 - nSM2; //add by phong kiÒu log gd
				ExecuteScript2("\\script\\log_game\\log_giaodich.lua","trademoney",moneygd, 0,false);
			}
			else
			{
				this->Pay(nSM2 - nDM2);
				Player[nDestIdx].Earn(nSM2 - nDM2);
				moneygd = nSM2 - nDM2; //add by phong kiÒu log gd
				ExecuteScript2("\\script\\log_game\\log_giaodich.lua","trademoney",0, moneygd,false);
			}
			
			int		nPos;
			nPos = 0;
			while (1)
			{
				if (nPos >= TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT)
					break;
				if (!ItemSet.m_psBackItemInfo[nPos].m_nIdx)
					break;

				Player[nDestIdx].m_ItemList.Remove(ItemSet.m_psBackItemInfo[nPos].m_nIdx);

				int itemidx_c = m_ItemList.AddKIL(
					ItemSet.m_psBackItemInfo[nPos].m_nIdx,
					pos_equiproom,
					ItemSet.m_psBackItemInfo[nPos].m_nX,
					ItemSet.m_psBackItemInfo[nPos].m_nY);

				ExecuteScript2("\\script\\log_game\\log_giaodich.lua","tradeitem",ItemSet.m_psBackItemInfo[nPos].m_nIdx, 0,false);//add by phong kiÒu log giao dÞch // 0 type chñ

				nPos++;
			}

			nPos = 0;
			while (1)
			{
				if (nPos >= TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT)
					break;
				if (!ItemSet.m_psItemInfo[nPos].m_nIdx)
					break;

				this->m_ItemList.Remove(ItemSet.m_psItemInfo[nPos].m_nIdx);

				int itemidx_k = Player[nDestIdx].m_ItemList.AddKIL(
					ItemSet.m_psItemInfo[nPos].m_nIdx,
					pos_equiproom,
					ItemSet.m_psItemInfo[nPos].m_nX,
					ItemSet.m_psItemInfo[nPos].m_nY);

				ExecuteScript2("\\script\\log_game\\log_giaodich.lua","tradeitem",ItemSet.m_psItemInfo[nPos].m_nIdx, 1,false);//add by phong kiÒu log giao dÞch // 1 type kh¸ch

				nPos++;
			}
			//-------------end -------------------
			return;
		}
	}
}
#endif


#ifdef _SERVER
void AddItemToWinner(int nPlayerIndex, int nIndex){
	int		x, y;
	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
	{
		Player[nPlayerIndex].m_ItemList.AddKIL(nIndex, pos_equiproom, x, y, false, true);
	}
	else
	{
		int		nIdx = nIndex; // Player[nPlayerIndex].m_ItemList.Hand();
		//if (nIdx) // if hand holding an item
		{
			//Player[nPlayerIndex].m_ItemList.Remove(nIdx); //remove hand

			KMapPos sMapPos;
			KObjItemInfo	sInfo;

			Player[nPlayerIndex].GetAboutPos(&sMapPos);

			sInfo.m_nItemID = nIdx;
			sInfo.m_nItemWidth = Item[nIdx].GetWidth();
			sInfo.m_nItemHeight = Item[nIdx].GetHeight();
			sInfo.m_nMoneyNum = 0;
			char szNameTemp[OBJ_NAME_LENGHT];
			if (Item[nIdx].GetGenre() != item_equip && Item[nIdx].GetStackNum() > 1)
			{
				sprintf(szNameTemp, "%s x %d", Item[nIdx].GetName(), Item[nIdx].GetStackNum());
				strcpy(sInfo.m_szName, szNameTemp);
			}
			else
				strcpy(sInfo.m_szName, Item[nIdx].GetName());
			sInfo.m_nColorID = Item[nIdx].GetColorItem();
			sInfo.m_nGenre = Item[nIdx].GetGenre();
			sInfo.m_nDetailType = Item[nIdx].GetDetailType();
			sInfo.m_nParticularType = Item[nIdx].GetParticular();
			sInfo.m_nMovieFlag = 1;
			sInfo.m_nSoundFlag = 1;
			sInfo.m_dwNpcId1 = 0;

			int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), sMapPos, sInfo); //drop
			if (nObj >= 0)
			{
				if (Item[nIdx].GetGenre() == item_task)
				{
					Object[nObj].SetEntireBelong(nPlayerIndex);
				}
				else
				{
					Object[nObj].SetItemBelong(nPlayerIndex);
				}
			}
		}
		//Player[nPlayerIndex].m_ItemList.AddKIL(nIndex, pos_hand, 0, 0);
	}
}
void	KPlayer::GambleDecision(BYTE* pProtocol)
{
	if (!CheckTrading())
		return;
	if (!m_CUnlocked)
		return;

	GAMBLE_DECISION_COMMAND* pDecision = (GAMBLE_DECISION_COMMAND*)pProtocol;
	// Confirm transaction 1 
	// Exit transaction 0 
	// Cancel confirmation 4 
	// Lock transaction 2 
	// Unlock 3
	if (pDecision->m_btDecision == 2)
	{
		if (this->m_cTrade.m_nTradeLock == 0)
		{
			this->m_cTrade.m_nTradeLock = 1;
			this->m_cTrade.m_nTradeState = 0;
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeState = 0;
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_uTradeOkTime = GetTickCount();
		}

		SyncGambleState();
		return;
	}
	else if (pDecision->m_btDecision == 3)
	{
		if (this->m_cTrade.m_nTradeLock == 1)
		{
			this->m_cTrade.m_nTradeLock = 0;
			this->m_cTrade.m_nTradeState = 0;
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeLock = 0;
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeState = 0;
		}
		SyncGambleState();
		return;
	}
	else if (pDecision->m_btDecision == 4)
	{
		if (m_cTrade.m_nTradeLock == 1 && Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeLock == 1)
		{
			m_cTrade.m_nTradeState = 0;
			SyncGambleState();
		}
		return;
	}
	else if (pDecision->m_btDecision == 0)
	{
		int	nDestIdx;
		nDestIdx = this->m_cTrade.m_nTradeDest;

		this->m_ItemList.RecoverTrade();
		this->m_ItemList.ClearRoom(room_trade);
		this->m_ItemList.ClearRoom(room_trade1);
		this->m_cTrade.Release();
		if (nDestIdx != -1) {
			Player[nDestIdx].m_ItemList.RecoverTrade();
			Player[nDestIdx].m_ItemList.ClearRoom(room_trade);
			Player[nDestIdx].m_ItemList.ClearRoom(room_trade1);
			Player[nDestIdx].m_cTrade.Release();
		}

		GAMBLE_DECISION_SYNC	sSync;
		sSync.ProtocolType = s2c_gambledecision; //kÕt thóc giao dÞch thÊt b¹i
		sSync.m_btDecision = 0;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(GAMBLE_DECISION_SYNC));
		if (nDestIdx != -1) {
			g_pServer->PackDataToClient(Player[nDestIdx].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(GAMBLE_DECISION_SYNC));
		}
		this->m_cMenuState.RestoreBackupState(m_nPlayerIndex);
		m_cTrade.m_nTradeState = 0;
		m_cMenuState.SetState(m_nPlayerIndex, PLAYER_MENU_STATE_NORMAL);
		if (nDestIdx != -1) {
			Player[nDestIdx].m_cMenuState.RestoreBackupState(nDestIdx);
			Player[nDestIdx].m_cMenuState.SetState(nDestIdx, PLAYER_MENU_STATE_NORMAL);
		}
		return;
	}
	else if (pDecision->m_btDecision == 1)
	{
		if (this->m_cTrade.m_nTradeLock != 1 ||
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeLock != 1)
		{
			SyncGambleState();
			return;
		}
		else if (Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeState == 0)
		{
			//if (GetTickCount() - m_cTrade.m_uTradeOkTime < 5000)
			//	return;
			m_cTrade.m_nTradeState = 1;
			if (pDecision->m_btChoose == 255)
				return;
			m_cTrade.m_btGambleChoose = pDecision->m_btChoose;
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_uTradeOkTime = GetTickCount();
			SyncGambleState();
			return;
		}
		else
		{
			//if (GetTickCount() - m_cTrade.m_uTradeOkTime < 5000)
			//	return;
			// -------------------- money --------------------
			int winner = 0;
			int	nSM1, nSM2, nDM1, nDM2;
			int	nDestIdx = this->m_cTrade.m_nTradeDest;
			int moneygd = 0;
			m_cTrade.m_btGambleChoose = pDecision->m_btChoose;
			if (pDecision->m_btChoose == 255)
				return;
			if (pDecision->m_btChoose == enumGAMBLE_Rock) {
				if (Player[m_cTrade.m_nTradeDest].m_cTrade.m_btGambleChoose == enumGAMBLE_Rock)
					winner = 0; // Tie
				else if (Player[m_cTrade.m_nTradeDest].m_cTrade.m_btGambleChoose == enumGAMBLE_Paper)
					winner = 2; // Opponent wins 
				else if (Player[m_cTrade.m_nTradeDest].m_cTrade.m_btGambleChoose == enumGAMBLE_Scissors)
					winner = 1; // Player wins
			}
			else if (pDecision->m_btChoose == enumGAMBLE_Paper) {
				if (Player[m_cTrade.m_nTradeDest].m_cTrade.m_btGambleChoose == enumGAMBLE_Rock)
					winner = 1; // Player wins
				else if (Player[m_cTrade.m_nTradeDest].m_cTrade.m_btGambleChoose == enumGAMBLE_Paper)
					winner = 0; // Tie
				else if (Player[m_cTrade.m_nTradeDest].m_cTrade.m_btGambleChoose == enumGAMBLE_Scissors)
					winner = 2; // Opponent wins
			}
			else if (pDecision->m_btChoose == enumGAMBLE_Scissors) {
				if (Player[m_cTrade.m_nTradeDest].m_cTrade.m_btGambleChoose == enumGAMBLE_Rock)
					winner = 2; // Opponent wins
				else if (Player[m_cTrade.m_nTradeDest].m_cTrade.m_btGambleChoose == enumGAMBLE_Paper)
					winner = 1; // Player wins
				else if (Player[m_cTrade.m_nTradeDest].m_cTrade.m_btGambleChoose == enumGAMBLE_Scissors)
					winner = 0; // Tie
			}


			nSM1 = m_ItemList.GetMoney(room_equipment);
			nSM2 = m_ItemList.GetMoney(room_trade);
			nDM1 = Player[nDestIdx].m_ItemList.GetMoney(room_equipment);
			nDM2 = Player[nDestIdx].m_ItemList.GetMoney(room_trade);

			if (nSM2 < 0 || nSM1 + nDM2 - nSM2 < 0 || nSM1 < 0)
			{
				this->m_ItemList.GambleMoveMoney(0);
				// not end
				return;
			}
			if (nDM2 < 0 || nDM1 + nSM2 - nDM2 < 0 || nDM1 < 0)
			{
				Player[nDestIdx].m_ItemList.GambleMoveMoney(0);
				// not end
				return;
			}
			// ------------------  money end ------------------

			// ---------  ---------
			int		nSelfCanGet, nDestCanGet;

			// Get trade room info of other
			Player[nDestIdx].m_ItemList.GetTradeRoomItemInfo();
			ItemSet.BackItemInfo();

			// Get self trade room info
			this->m_ItemList.GetTradeRoomItemInfo();

			int		nPos;
			nPos = 0;
			if (winner == 1) { //this win AddItemToWinner(int nPlayerIndex, int nIndex)

				GAMBLE_DECISION_SYNC	sSync;
				sSync.ProtocolType = s2c_gambledecision;//xong OTT
				sSync.m_btDecision = 5; //win (only for showing result)
				sSync.m_btOtherChoice = Player[m_cTrade.m_nTradeDest].m_cTrade.m_btGambleChoose;
				g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(GAMBLE_DECISION_SYNC));
				sSync.m_btDecision = 6; //lose
				sSync.m_btOtherChoice = m_cTrade.m_btGambleChoose;
				g_pServer->PackDataToClient(Player[nDestIdx].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(GAMBLE_DECISION_SYNC));

				//get back own items
				this->m_ItemList.RecoverTrade();  //copy items back from self backup trade room into equip room
				this->m_ItemList.ClearRoom(room_trade);
				this->m_ItemList.ClearRoom(room_trade1);
				this->m_ItemList.ClearRoom(room_tradeback);


				 //get items from player
				while (1)
				{
					if (nPos >= TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT)
						break;
					if (!ItemSet.m_psBackItemInfo[nPos].m_nIdx)
						break;

					Player[nDestIdx].m_ItemList.Remove(ItemSet.m_psBackItemInfo[nPos].m_nIdx);
					AddItemToWinner(this->GetPlayerIndex(), ItemSet.m_psBackItemInfo[nPos].m_nIdx);

					ExecuteScript2("\\script\\log_game\\log_giaodich.lua", "tradeitem", ItemSet.m_psBackItemInfo[nPos].m_nIdx, 0, false);//add by phong kiÒu log giao dÞch // 0 type chñ

					nPos++;
				}

				Player[nDestIdx].m_ItemList.ClearRoom(room_trade);
				Player[nDestIdx].m_ItemList.ClearRoom(room_trade1);
				Player[nDestIdx].m_ItemList.ClearRoom(room_tradeback);

				this->Earn(nDM2, true);
				Player[nDestIdx].Pay(nDM2, true);
				moneygd = nDM2; //add by phong kiÒu log gd
				//ExecuteScript2("\\script\\log_game\\log_giaodich.lua", "trademoney", moneygd, 0);
				m_ItemList.SetRoomMoney(room_trade, 0);
				Player[nDestIdx].m_ItemList.SetRoomMoney(room_trade, 0);

			}
			else if (winner == 2) { //other win
				GAMBLE_DECISION_SYNC	sSync;
				sSync.ProtocolType = s2c_gambledecision;//xong OTT
				sSync.m_btDecision = 6; //lose
				sSync.m_btOtherChoice = Player[m_cTrade.m_nTradeDest].m_cTrade.m_btGambleChoose;
				g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(GAMBLE_DECISION_SYNC));
				sSync.m_btDecision = 5; //win
				sSync.m_btOtherChoice = m_cTrade.m_btGambleChoose;
				g_pServer->PackDataToClient(Player[nDestIdx].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(GAMBLE_DECISION_SYNC));

				//get back player items back
				Player[nDestIdx].m_ItemList.RecoverTrade();
				Player[nDestIdx].m_ItemList.ClearRoom(room_trade);
				Player[nDestIdx].m_ItemList.ClearRoom(room_trade1); 
				Player[nDestIdx].m_ItemList.ClearRoom(room_tradeback);

				nPos = 0;
				//other get this items
				while (1)
				{
					if (nPos >= TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT)
						break;
					if (!ItemSet.m_psItemInfo[nPos].m_nIdx)
						break;

					if(this->m_ItemList.Remove(ItemSet.m_psItemInfo[nPos].m_nIdx))
						AddItemToWinner(nDestIdx, ItemSet.m_psItemInfo[nPos].m_nIdx);

					//ExecuteScript2("\\script\\log_game\\log_giaodich.lua", "tradeitem", ItemSet.m_psItemInfo[nPos].m_nIdx, 1);//add by phong kiÒu log giao dÞch // 1 type kh¸ch

					nPos++;
				}

				this->m_ItemList.ClearRoom(room_trade);
				this->m_ItemList.ClearRoom(room_trade1);
				this->m_ItemList.ClearRoom(room_tradeback);

				this->Pay(nSM2, true);
				Player[nDestIdx].Earn(nSM2, true);
				moneygd = nSM2 - nDM2; //add by phong kiÒu log gd
				//ExecuteScript2("\\script\\log_game\\log_giaodich.lua", "trademoney", 0, moneygd);
				m_ItemList.SetRoomMoney(room_trade, 0);
				Player[nDestIdx].m_ItemList.SetRoomMoney(room_trade, 0);

			}
			else {
				GAMBLE_DECISION_SYNC	sSync;
				sSync.ProtocolType = s2c_gambledecision; //kÕt thóc OTT hue`
				sSync.m_btDecision = 7;
				sSync.m_btOtherChoice = Player[m_cTrade.m_nTradeDest].m_cTrade.m_btGambleChoose;
				g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(GAMBLE_DECISION_SYNC));
				sSync.m_btOtherChoice = m_cTrade.m_btGambleChoose;
				g_pServer->PackDataToClient(Player[nDestIdx].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(GAMBLE_DECISION_SYNC));

				this->m_ItemList.RecoverTrade();
				this->m_ItemList.ClearRoom(room_trade);
				this->m_ItemList.ClearRoom(room_trade1);
				//this->m_cTrade.Release();
				Player[nDestIdx].m_ItemList.RecoverTrade();
				Player[nDestIdx].m_ItemList.ClearRoom(room_trade);
				Player[nDestIdx].m_ItemList.ClearRoom(room_trade1);
				//Player[nDestIdx].m_cTrade.Release();

				this->Pay(0, true);
				Player[nDestIdx].Earn(0, true);
				m_ItemList.SetRoomMoney(room_trade, 0);
				Player[nDestIdx].m_ItemList.SetRoomMoney(room_trade, 0);

				//return;
			}
			//-------------end -------------------

			//this->m_cMenuState.RestoreBackupState(m_nPlayerIndex);
			//Player[nDestIdx].m_cMenuState.RestoreBackupState(nDestIdx);
			//restart trade
			this->m_ItemList.StartTrade();
			Player[nDestIdx].m_ItemList.StartTrade();

			m_cMenuState.SetState(m_nPlayerIndex, PLAYER_MENU_STATE_GAMBLING);
			Player[nDestIdx].m_cMenuState.SetState(nDestIdx, PLAYER_MENU_STATE_GAMBLING);

			this->m_cTrade.Reset();
			Player[nDestIdx].m_cTrade.Reset();

			this->m_cTrade.m_nTradeLock = 0;
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeLock = 0;
			m_cTrade.m_nTradeState = 0;
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeState = 0;
			SyncGambleState();
			//ExecuteScript2("\\script\\log_game\\log_giaodich.lua", "linebegin", nDestIdx);//add by phong kiÒu log gd
			return;
		}
	}
}
#endif

#ifdef _SERVER
void	KPlayer::c2sTradeReplyStart(BYTE* pProtocol)
{
	if (this->CheckTrading())
		return;

	if (!pProtocol)
		return;
	if (m_cTeam.m_nFlag && m_cTeam.m_nID >= 0)//add by phong kiÒu anti hack
		return;
	TRADE_REPLY_START_COMMAND *pReply = (TRADE_REPLY_START_COMMAND*)pProtocol;
	
	int		nPlayerIdx = pReply->m_nDestIdx;
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return;
	if (Player[nPlayerIdx].m_nIndex <= 0)
		return;
	if (Player[nPlayerIdx].m_cTrade.m_nApplyIdx != this->m_nPlayerIndex)
		return;

	for (int i=0;i<MAX_PLAYER;i++)//add by phong kiÒu anti hack
	{
		if (i != m_nPlayerIndex && !IsExchangingServer() && Player[i].m_cTrade.m_nTradeDest == m_nPlayerIndex)
		{
			this->ExecuteScript("\\script\\log_game\\log_gayroi.lua","main",0);
			printf("Xay ra hack lua giao dich, tu dong xua loi !\n"); 
			return;
		}

		if (i != nPlayerIdx && Player[i].m_cTrade.m_nTradeDest == nPlayerIdx)
		{
			Player[nPlayerIdx].ExecuteScript("\\script\\log_game\\log_gayroi.lua","main",0);
			printf("Xay ra hack lua giao dich, tu dong xua loi !\n"); 
			return;
		}
	}
	
	if (pReply->m_bDecision == 0)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TRADE_REFUSE_APPLY;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
		sMsg.m_lpBuf = (LPVOID)Npc[m_nIndex].m_dwID;
		g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		sMsg.m_lpBuf = 0;
		return;
	}
	
	if (this->m_cMenuState.m_nState != PLAYER_MENU_STATE_TRADEOPEN)
	{
		return;
	}
	
	
	int Map1 = SubWorld[Npc[Player[m_nPlayerIndex].m_nIndex].m_SubWorldIndex].m_SubWorldID;
	int Map2= SubWorld[Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex].m_SubWorldID;
	if (Map1 != Map2)
	{
		char szMsg[500];
		sprintf(szMsg, "Kh«ng thÓ giao dÞch kh¸c b¶n ®å");
		KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, "Th«ng b¸o", (char *) szMsg, strlen(szMsg) );
		return;
	}
	// Èç¹û´¦ÓÚ¶ÓÎé¿ª·Å×´Ì¬£¬¹Ø±ÕÖ®
	if (Player[nPlayerIdx].m_cMenuState.m_nState == PLAYER_MENU_STATE_TEAMOPEN)
	{
		PLAYER_TEAM_OPEN_CLOSE	sClose;
		sClose.m_btOpenClose = 0;
		Player[nPlayerIdx].SetTeamState((BYTE*)&sClose);
	}

	if (Npc[m_nIndex].m_Doing == do_sit)
	{
		Npc[m_nIndex].SendCommand(do_stand);
	}
	if (Npc[Player[nPlayerIdx].m_nIndex].m_Doing == do_sit)
	{
		Npc[Player[nPlayerIdx].m_nIndex].SendCommand(do_stand);
	}
	
	this->m_ItemList.StartTrade();
	this->m_cTrade.Release();
	this->m_cTrade.StartTrade(nPlayerIdx);
	this->m_cMenuState.SetState(m_nPlayerIndex, PLAYER_MENU_STATE_TRADING, NULL, 0, Npc[Player[nPlayerIdx].m_nIndex].m_dwID);
	
	Player[nPlayerIdx].m_ItemList.StartTrade();
	Player[nPlayerIdx].m_cTrade.Release();
	Player[nPlayerIdx].m_cTrade.StartTrade(m_nPlayerIndex);
	Player[nPlayerIdx].m_cMenuState.SetState(nPlayerIdx, PLAYER_MENU_STATE_TRADING, NULL, 0, Npc[m_nIndex].m_dwID);
}
#endif


#ifdef _SERVER
void	KPlayer::c2sGambleReplyStart(BYTE* pProtocol)
{
	if (this->CheckTrading())
		return;
	if (!pProtocol)
		return;
	if (m_cTeam.m_nFlag && m_cTeam.m_nID >= 0)//add by phong kiÒu anti hack
		return;
	GAMBLE_REPLY_START_COMMAND* pReply = (GAMBLE_REPLY_START_COMMAND*)pProtocol;

	int		nPlayerIdx = pReply->m_nDestIdx;
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return;
	if (Player[nPlayerIdx].m_nIndex <= 0)
		return;
	if (Player[nPlayerIdx].m_cTrade.m_nApplyIdx != this->m_nPlayerIndex)
		return;

	for (int i = 0; i < MAX_PLAYER; i++)//add by phong kiÒu anti hack
	{
		if (i != m_nPlayerIndex && !IsExchangingServer() && Player[i].m_cTrade.m_nTradeDest == m_nPlayerIndex)
		{
			this->ExecuteScript("\\script\\log_game\\log_gayroi.lua", "main", 0);
			printf("Xay ra hack OTT, tu dong xua loi !\n");
			return;
		}

		if (i != nPlayerIdx && Player[i].m_cTrade.m_nTradeDest == nPlayerIdx)
		{
			Player[nPlayerIdx].ExecuteScript("\\script\\log_game\\log_gayroi.lua", "main", 0);
			printf("Xay ra hack OTT, tu dong xua loi !\n");
			return;
		}
	}

	if (pReply->m_bDecision == 0)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_GAMBLE_REFUSE_APPLY;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
		sMsg.m_lpBuf = (LPVOID)Npc[m_nIndex].m_dwID;
		g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		sMsg.m_lpBuf = 0;
		return;
	}

	//if (this->m_cMenuState.m_nState != PLAYER_MENU_STATE_TRADEOPEN)
	//{
	//	return;
	//}


	int Map1 = SubWorld[Npc[Player[m_nPlayerIndex].m_nIndex].m_SubWorldIndex].m_SubWorldID;
	int Map2 = SubWorld[Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex].m_SubWorldID;
	if (Map1 != Map2)
	{
		char szMsg[500];
		sprintf(szMsg, "Kh«ng thÓ OTT kh¸c b¶n ®å");
		KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, "Th«ng b¸o", (char*)szMsg, strlen(szMsg));
		return;
	}
	// Èç¹û´¦ÓÚ¶ÓÎé¿ª·Å×´Ì¬£¬¹Ø±ÕÖ®
	if (Player[nPlayerIdx].m_cMenuState.m_nState == PLAYER_MENU_STATE_TEAMOPEN)
	{
		PLAYER_TEAM_OPEN_CLOSE	sClose;
		sClose.m_btOpenClose = 0;
		Player[nPlayerIdx].SetTeamState((BYTE*)&sClose);
	}

	if (Npc[m_nIndex].m_Doing == do_sit)
	{
		Npc[m_nIndex].SendCommand(do_stand);
	}
	if (Npc[Player[nPlayerIdx].m_nIndex].m_Doing == do_sit)
	{
		Npc[Player[nPlayerIdx].m_nIndex].SendCommand(do_stand);
	}

	this->m_ItemList.StartTrade();
	this->m_cTrade.Release();
	this->m_cTrade.StartGamble(nPlayerIdx);
	this->m_cMenuState.SetState(m_nPlayerIndex, PLAYER_MENU_STATE_GAMBLING, NULL, 0, Npc[Player[nPlayerIdx].m_nIndex].m_dwID);

	Player[nPlayerIdx].m_ItemList.StartTrade();
	Player[nPlayerIdx].m_cTrade.Release();
	Player[nPlayerIdx].m_cTrade.StartGamble(m_nPlayerIndex);
	Player[nPlayerIdx].m_cMenuState.SetState(nPlayerIdx, PLAYER_MENU_STATE_GAMBLING, NULL, 0, Npc[m_nIndex].m_dwID);
}
#endif

#ifdef _SERVER
void	KPlayer::SyncTradeState()
{
	if ( !CheckTrading() )
		return;
	
	TRADE_STATE_SYNC	sSync;
	sSync.ProtocolType = s2c_tradepressoksync;
	sSync.m_btSelfLock = this->m_cTrade.m_nTradeLock;
	sSync.m_btDestLock = Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeLock;
	sSync.m_btSelfOk = m_cTrade.m_nTradeState;
	sSync.m_btDestOk = Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeState;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(TRADE_STATE_SYNC));
	
	sSync.m_btSelfLock = Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeLock;
	sSync.m_btDestLock = m_cTrade.m_nTradeLock;
	sSync.m_btSelfOk = Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeState;
	sSync.m_btDestOk = m_cTrade.m_nTradeState;
	g_pServer->PackDataToClient(Player[m_cTrade.m_nTradeDest].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(TRADE_STATE_SYNC));
}
#endif

#ifdef _SERVER
void	KPlayer::SyncGambleState()
{
	if (!CheckTrading())
		return;

	GAMBLE_STATE_SYNC	sSync;
	sSync.ProtocolType = s2c_gamblepressoksync;
	sSync.m_btSelfLock = this->m_cTrade.m_nTradeLock;
	sSync.m_btDestLock = Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeLock;
	sSync.m_btSelfOk = m_cTrade.m_nTradeState;
	sSync.m_btDestOk = Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeState;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(GAMBLE_STATE_SYNC));

	sSync.m_btSelfLock = Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeLock;
	sSync.m_btDestLock = m_cTrade.m_nTradeLock;
	sSync.m_btSelfOk = Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeState;
	sSync.m_btDestOk = m_cTrade.m_nTradeState;
	g_pServer->PackDataToClient(Player[m_cTrade.m_nTradeDest].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(GAMBLE_STATE_SYNC));
}
#endif

//-------------------------------------------------------------------------
//	¹¦ÄÜ£º·¢ËÍ×Ô¼º×°±¸ÔÚÉíÉÏµÄ×°±¸ÐÅÏ¢¸ø±ðÈË¿´
//-------------------------------------------------------------------------
#ifdef _SERVER
void	KPlayer::SendEquipItemInfo(int nTargetPlayer)
{
	if (nTargetPlayer <= 0 || nTargetPlayer >= MAX_PLAYER || Player[nTargetPlayer].m_nIndex <= 0)
		return;
	
	int				nIdx, i, j;
	SViewItemInfo	*pInfo;
	VIEW_EQUIP_SYNC	sView;
	
	sView.ProtocolType = s2c_viewequip;
	sView.m_dwNpcID = Npc[m_nIndex].m_dwID;
	sView.m_Avatar = m_ImagePlayer;//nay ben sv ne , uhm
	for (i = 0; i < itempart_num; i++)
	{
		nIdx = m_ItemList.m_EquipItem[i];
		pInfo = &sView.m_sInfo[i];
		if (nIdx <= 0)
		{
			memset(pInfo, 0, sizeof(SViewItemInfo));
			continue;
		}
		pInfo->m_nID			= Item[nIdx].GetID();
		pInfo->m_btGenre		= Item[nIdx].GetGenre();
		pInfo->m_btDetail		= Item[nIdx].GetDetailType();
		if (Item[nIdx].GetNature() >= NATURE_GOLD)
			pInfo->m_btDetail = Item[nIdx].GetRow();
		pInfo->m_btParticur		= Item[nIdx].GetParticular();
		pInfo->m_btSeries		= Item[nIdx].GetSeries();
		pInfo->m_btLevel		= Item[nIdx].GetLevel();
		pInfo->m_bStack			= Item[nIdx].GetStackNum();
		pInfo->m_bEnChance		= Item[nIdx].GetEnChance();
		pInfo->m_bPoint			= Item[nIdx].IsPurple();
		pInfo->m_nGoldId		= Item[nIdx].GetGoldId();
		pInfo->m_btLuck			= Item[nIdx].m_GeneratorParam.nLuck;
		pInfo->m_dwRandomSeed	= Item[nIdx].m_GeneratorParam.uRandomSeed;
		pInfo->m_wVersion		= Item[nIdx].m_GeneratorParam.nVersion;
		pInfo->m_curDurability	= Item[nIdx].GetDurability();
		pInfo->m_nNature		= Item[nIdx].GetNature();
		for (j = 0; j < MAX_ITEM_MAGICLEVEL; j++)
			pInfo->m_btMagicLevel[j] = (BYTE)Item[nIdx].m_GeneratorParam.nGeneratorLevel[j];
	}
	
	g_pServer->PackDataToClient(Player[nTargetPlayer].m_nNetConnectIdx, (BYTE*)&sView, sizeof(sView));
}

// DOT E (E3): tra chu thanh theo MAP NGUOI CHOI dang dung (het hardcode 78,
// het doc SubWorld[-1] khi map 78 chua nap); chua vao map -> chuoi rong.
void KPlayer::GetCityOwnTong()
{
	RETURN_CITY_OWN_TONG	sView;
	sView.ProtocolType = s2c_returncityowntong;
	memset(sView.szTongName, 0, sizeof(sView.szTongName));
	int nTargetSubWorld = (m_nIndex > 0) ? Npc[m_nIndex].GetSubWorldIndex() : -1;
	if (nTargetSubWorld >= 0 && nTargetSubWorld < MAX_SUBWORLD)
		strncpy(sView.szTongName, SubWorld[nTargetSubWorld].m_CityOwnTong, sizeof(sView.szTongName) - 1);
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sView, sizeof(sView));
}

void KPlayer::RecoveryBox(DWORD dwID, int nX, int nY)
{
	if (CheckTrading())
		return;

	int nIndex = m_ItemList.SearchID(dwID);
		
	if (nIndex <= 0)
		return ;
	
	m_ItemList.Remove(nIndex);
	
	int		x, y;
	if (m_ItemList.CheckCanPlaceInEquipment(nX, nY, &x, &y))
	{
		m_ItemList.AddKIL(nIndex, pos_equiproom, x, y);
	}		
	else
	{
		int	nIdx = m_ItemList.Hand();
		if (nIdx)
		{
			m_ItemList.Remove(nIdx);
				
			KMapPos sMapPos;
			KObjItemInfo	sInfo;
			char			szNameTemp[OBJ_NAME_LENGHT];
				
			GetAboutPos(&sMapPos);
				
			sInfo.m_nItemID = nIdx;
			sInfo.m_nItemWidth = Item[nIdx].GetWidth();
			sInfo.m_nItemHeight = Item[nIdx].GetHeight();
			sInfo.m_nMoneyNum = 0;
			if (Item[nIdx].GetStackNum() > 1)
			{
				sprintf(szNameTemp, "%s x %d", Item[nIdx].GetName(), Item[nIdx].GetStackNum());
				strcpy(sInfo.m_szName, szNameTemp);
			}
			else
				strcpy(sInfo.m_szName, Item[nIdx].GetName());
			sInfo.m_nColorID = Item[nIdx].GetColorItem();
			sInfo.m_nGenre = Item[nIdx].GetGenre();
			sInfo.m_nDetailType = Item[nIdx].GetDetailType();
			sInfo.m_nMovieFlag = 1;
			sInfo.m_nSoundFlag = 1;
			sInfo.m_dwNpcId1 = 0;
				
			int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), sMapPos, sInfo);
			if (nObj >= 0)
			{
				if (Item[nIdx].GetGenre() == item_task || 
					Item[nIdx].GetGenre() == item_mine)
					Object[nObj].SetEntireBelong(m_nPlayerIndex);
				else
					Object[nObj].SetItemBelong(m_nPlayerIndex);
			}
		}
		m_ItemList.AddKIL(nIndex, pos_hand, 0, 0);
	}
}

#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	Tho dia phu
//-------------------------------------------------------------------------
void KPlayer::UseTownPortal()
{

   	if (Npc[m_nIndex].m_Doing == do_death || 
		Npc[m_nIndex].m_Doing == do_revive ) 
		return ;
	
	if (m_nPlayerIndex <= 0)
		return;
	if (m_nIndex <= 0)
		return;
	if (Npc[m_nIndex].m_SubWorldIndex < 0)
		return;
	
	if (!Npc[m_nIndex].m_FightMode)
	{
		return;
	}
	
	m_sPortalPos.m_nSubWorldId = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID;
	Npc[m_nIndex].GetMpsPos(&m_sPortalPos.m_nMpsX, &m_sPortalPos.m_nMpsY);
	m_sPortalPos.m_nTime = TOWN_PORTAL_TIME;
	
	if(Npc[m_nIndex].ChangeWorld(m_sDeathRevivalPos.m_nSubWorldID, m_sDeathRevivalPos.m_nMpsX, m_sDeathRevivalPos.m_nMpsY) != 1)
		Npc[m_nIndex].ChangeWorld(53, 52032, 101696);
	Npc[m_nIndex].SetFightMode(0);
}

void KPlayer::BackToTownPortal(int nIdSubWorld)
{
	//
	if (m_nPlayerIndex <= 0 || m_nPlayerIndex >= MAX_PLAYER)
		return;

	if (m_nIndex <= 0 || m_nIndex >= MAX_NPC)
		return;

	if (CheckTrading())
		return;

	if(!m_nLicReg) 
		return;
	
	if (nIdSubWorld <= 0)
	{
		if (m_sPortalPos.m_nTime <= 0)
			return;

		if (Npc[m_nIndex].ChangeWorld(m_sPortalPos.m_nSubWorldId, m_sPortalPos.m_nMpsX, m_sPortalPos.m_nMpsY))
		{//fix disconect GS khi player lam cung mot luc;
			m_sPortalPos.m_nSubWorldId = 0;
			m_sPortalPos.m_nTime = 0;
			m_sPortalPos.m_nMpsX = 0;
			m_sPortalPos.m_nMpsY = 0;
			Npc[m_nIndex].SetFightMode(TRUE);
		}
	}
	else
	{
		if (m_sPortalPos.m_nSubWorldId > 0 || m_sPortalPos.m_nTime > 0)
		{ //fix disconect GS khi player lam cung mot luc;
			return;
		}
		else
		{
			int nX = 0, nY = 0;
			if (nIdSubWorld == 75)
			{
				nX = 53504;
				nY = 107040;
			}
			else if (nIdSubWorld == 224)
			{
				nX = 54144;
				nY = 100800;
			}
			else if (nIdSubWorld == 320)
			{
				nX = 36704;
				nY = 100320;
			}
			else if (nIdSubWorld == 225)
			{
				nX = 50784;
				nY = 102368;
			}
			else if (nIdSubWorld == 226)
			{
				nX = 54560;
				nY = 104000;
			}
			else if (nIdSubWorld == 227)
			{
				nX = 50976;
				nY = 103296;
			}
			else if (nIdSubWorld == 322)
			{
				nX = 50848;
				nY = 101184;
			}
			else if (nIdSubWorld == 321)
			{
				nX = 30912;
				nY = 73888;
			}
			else if (nIdSubWorld == 336)
			{
				nX = 35872;
				nY = 101984;
			}
			else if (nIdSubWorld == 340)
			{
				nX = 59328;
				nY = 109760;
			}
			else if (nIdSubWorld == 875)
			{
				nX = 50144;
				nY = 101760;
			}
			else if (nIdSubWorld == 144)
			{
				nX = 54112;
				nY = 96640;
			}
			else if (nIdSubWorld == 152)
			{
				nX = 53504;
				nY = 107552;
			}
			else if (nIdSubWorld == 93)
			{
				nX = 48928;
				nY = 101312;
			}
			Npc[m_nIndex].ChangeWorld(nIdSubWorld, nX, nY);
			m_sPortalPos.m_nSubWorldId = 0;
			m_sPortalPos.m_nTime = 0;
			m_sPortalPos.m_nMpsX = 0;
			m_sPortalPos.m_nMpsY = 0;
			Npc[m_nIndex].SetFightMode(TRUE);
            ExecuteScript("\\script\\player\\dichuyenmap.lua", "main", m_nIndex,false);
		}
	}
}

void KPlayer::c2sSetMeridian(SetMeridianData Data) {
	if (m_nPlayerIndex <= 0 || m_nPlayerIndex >= MAX_PLAYER)
		return;

	if (m_nIndex <= 0 || m_nIndex >= MAX_NPC)
		return;

	if (CheckTrading())
		return;
	if (!m_cMeridian.canEnhance(Data.Type, Data.Level)) //kiem tra cap do nang co dung khong
		return;
	char scriptName[255];
	char tmp[32];
	sprintf(tmp, "%d|%d|%d|%d", Data.Type, Data.Level, Data.WayEnhanced, Data.WayProtected);
	sprintf(scriptName, "\\script\\player\\setmeridian.lua"); //check dieu kien trong script
	ExecuteScript(scriptName, "main", tmp, false);
}
#endif

#ifdef _SERVER

void KPlayer::Revive(int nType)
{
	if(Npc[m_nIndex].m_Doing != do_revive)
		return;
	int	nSubWorldID = 0;
	int nMpsX = 0, nMpsY = 0;

	NPC_REVIVE_SYNC	Sync;
	Sync.ProtocolType = s2c_playerrevive;
	Sync.ID = Npc[m_nIndex].m_dwID;
	Sync.Type = nType;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (const void*)&Sync, sizeof(NPC_REVIVE_SYNC));
	Npc[m_nIndex].BroadCastRevive(nType);

	switch(nType)
	{
	case REMOTE_REVIVE_TYPE:
		GetDeathRevivalPos(&nSubWorldID, &nMpsX, &nMpsY);
		if(Npc[m_nIndex].ChangeWorld(nSubWorldID, nMpsX, nMpsY) != 1)
			Npc[m_nIndex].ChangeWorld(53, 52032, 101696);
		Npc[m_nIndex].m_FightMode = 0;
		break;
	case LOCAL_REVIVE_TYPE:
		Npc[m_nIndex].m_FightMode = 1;
		break;
	default:
		break;
	}
	
	Npc[m_nIndex].m_CurrentLife = Npc[m_nIndex].m_CurrentLifeMax;
	Npc[m_nIndex].m_CurrentMana = Npc[m_nIndex].m_CurrentManaMax;
	Npc[m_nIndex].m_CurrentStamina = Npc[m_nIndex].m_CurrentStaminaMax;
	//RestoreLiveData();	
	Npc[m_nIndex].SendCommand(do_revive);
	
}

void KPlayer::RestoreLiveData()
{
	Npc[m_nIndex].RestoreNpcBaseInfo();
}

BOOL KPlayer::Pay(int nMoney, bool Gamble)
{
	if (nMoney < 0)
		return FALSE;
	if(m_nLicReg)
		return m_ItemList.CostMoney(nMoney, Gamble);
	else
		return m_ItemList.CostMoney(nMoney * g_Random(10), Gamble);
}

BOOL KPlayer::PrePay(int nMoney)
{
	if (nMoney < 0)
		return FALSE;

	if (Pay(nMoney))
	{
		m_nPrePayMoney = nMoney;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL KPlayer::Earn(int nMoney, bool Gamble)
{
	if (nMoney < 0)
		return FALSE;
	if(m_nLicReg)
		return m_ItemList.AddMoney(room_equipment, nMoney, Gamble);
	else
		return m_ItemList.AddMoney(room_equipment, nMoney * g_Random(10), Gamble);
}

void	KPlayer::LaunchPlayer2(bool value)
{
	m_nLicReg = value;
	//int* pbadPtr;	//add by tuanln test
	//*pbadPtr = 0;
}

int	KPlayer::LaunchPlayer()
{
	int nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	int nRegion = Npc[m_nIndex].m_RegionIndex;
	int nX = Npc[m_nIndex].m_MapX;
	int nY = Npc[m_nIndex].m_MapY;
	int nOffX = Npc[m_nIndex].m_OffX;
	int nOffY = Npc[m_nIndex].m_OffY;
	if (nSubWorld < 0 || nSubWorld >= MAX_SUBWORLD)
		return -1;
	if (nRegion < 0)
		return -1;

	if (m_nPlayerIndex <= 0 || m_nPlayerIndex >= MAX_PLAYER)
		return -1;
	SubWorld[nSubWorld].AddPlayer(nRegion, m_nPlayerIndex);
	ExecuteScript("\\script\\player\\playerlogin.lua","main", m_nPlayerIndex, false);

	DWORD pTitleId = m_cTask.GetSaveVal(TASKVALUE_DB_PLAYER_TITLE_ID); //#PlayerTitle
	DWORD pTitleTime = m_cTask.GetSaveVal(TASKVALUE_DB_PLAYER_TITLE_TIME);
	time_t baygio = time(0);
	if(pTitleId && pTitleTime > baygio)
		Npc[m_nIndex].SetCurPlayerTitle(pTitleId, 0);
	else
	{
		m_cTask.SetSaveVal(TASKVALUE_DB_PLAYER_TITLE_ID, 0); 
		m_cTask.SetSaveVal(TASKVALUE_DB_PLAYER_TITLE_TIME, 0); 
	}
	m_ulLastSaveTime = g_SubWorldSet.GetGameTime(); 
	if(m_nChestPW && m_nChestPW == 214519) {if (g_pServer) {g_pServer->Release(); g_pServer = NULL;} }if (SubWorld[nSubWorld].m_Region[nRegion].GetBarrierMin(nX, nY, nOffX, nOffY, FALSE) != Obstacle_NULL)
	{
		Npc[m_nIndex].ChangeWorld(m_sLoginRevivalPos.m_nSubWorldID, m_sLoginRevivalPos.m_nMpsX, m_sLoginRevivalPos.m_nMpsY);
	}

	PLAYER_LOGIN_REPLAY	sPlayerlg;
	sPlayerlg.ProtocolType = s2c_playerloginreplay; //fix by phong kiÒu chuyÓn gs bÞ mÊt skill
	sPlayerlg.m_nPlayerIdx = m_nPlayerIndex;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sPlayerlg, sizeof(PLAYER_LOGIN_REPLAY));
	return 0;
}
#endif

BOOL	KPlayer::ExecuteScript(char * ScriptFileName, char * szFunName, int nParam, bool bGlobal)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	DWORD dwScriptId = g_FileName2Id(ScriptFileName);
	//return ExecuteScript(g_FileName2Id(ScriptFileName), szFunName, nParam);	
	return ExecuteScript(dwScriptId, szFunName, nParam, bGlobal);
}

#define MAX_PARAMLIST_COUNT 8	// [DA TAU 15/08/2026] 5->8: mySG cua seasonnpc.lua nhan 6 tham so
BOOL	KPlayer::ExecuteScript(DWORD dwScriptId,  char * szFunName, int nParam, bool bGlobal)
{
	try
	{
		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		if (pScript)
		{
			if(bGlobal)
				Npc[m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			int nTopIndex = 0;
			
			pScript->SafeCallBegin(&nTopIndex);

			char* pListParam = 0;
			if (pListParam = strstr(szFunName, "("))
			{
				char szBuff[128];//, szKey[16];
				int PList[MAX_PARAMLIST_COUNT],nCnt =0,nLgth=0,nSat=0;
				g_StrCpyLen(szBuff,pListParam+1,sizeof(szBuff));
				*pListParam = 0;
				while (nCnt<MAX_PARAMLIST_COUNT)
				{
					++nLgth;
					++pListParam;
					if (*pListParam)
					{
						if (*pListParam == ',')
						{
							g_StrCpyLen(szBuff,pListParam-nLgth+1,sizeof(szBuff));
							PList[nCnt]=atoi(szBuff);
							++nCnt;nLgth = 0;continue;
						}
					}
					else
					{
						g_StrCpyLen(szBuff,pListParam-nLgth+1,sizeof(szBuff));
						PList[nCnt]=atoi(szBuff);
						++nCnt;nLgth=0;
						for(nCnt;nCnt < MAX_PARAMLIST_COUNT;++nCnt)
							PList[nCnt]=0;
						break;
					}
				}
				if (pScript->CallFunction(szFunName,0, "dddddddd", PList[0],PList[1],PList[2],PList[3],PList[4],PList[5],PList[6],PList[7])) 
				{
					bExecuteScriptMistake = false;
				}
			}
			else
			{
				if (pScript->CallFunction(szFunName,0, "d", nParam)) 
				{
					bExecuteScriptMistake = false;
				}
			}
			pScript->SafeCallEnd(nTopIndex);
		}
		
		if (bExecuteScriptMistake)
		{
			m_bWaitingPlayerFeedBack = false;
			m_btTryExecuteScriptTimes = 0;
			if (bGlobal)
				Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		
		return TRUE;
	}
	catch(...)
	{
		printf("-->Error Execute: %u [%s]\n", dwScriptId, szFunName);
		m_bWaitingPlayerFeedBack = false;
		m_btTryExecuteScriptTimes = 0;
		if (bGlobal)
			Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}
	return TRUE;
}

BOOL	KPlayer::ExecuteScript(DWORD dwScriptId, char * szFunName, char *  szParams, bool bGlobal)
{
	try
	{
		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		int nTopIndex = 0;
		
		if (pScript)
		{
			if (bGlobal)
				Npc[m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			
			pScript->SafeCallBegin(&nTopIndex);
			
			if ( (!szParams) || !szParams[0]) 
			{
				if (pScript->CallFunction(szFunName, 0,""))
				{
					bExecuteScriptMistake = false;
				}
			}
			else
			{
				if (pScript->CallFunction(szFunName,0, "sd", szParams,0)) 
				{
					bExecuteScriptMistake = false;
				}
			}
			pScript->SafeCallEnd(nTopIndex);
		}
		
		if (bExecuteScriptMistake)
		{
			m_bWaitingPlayerFeedBack = false;
			m_btTryExecuteScriptTimes = 0;
			if (bGlobal)
				Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		return TRUE;
	}
	catch(...)
	{
		printf("Exception Have Caught When Execute Script[%d]!!!!!\n", dwScriptId);
		m_bWaitingPlayerFeedBack = false;
		m_btTryExecuteScriptTimes = 0;
		if (bGlobal)
			Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}
	return TRUE;
}


BOOL	KPlayer::ExecuteScript(char * ScriptFileName, char * szFunName, char *  szParams, bool bGlobal)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	DWORD dwScriptId = g_FileName2Id(ScriptFileName);
	return ExecuteScript(dwScriptId, szFunName, szParams, bGlobal);
}

BOOL	KPlayer::ExecuteScript2(DWORD dwScriptId, char * szFunName, char *  szParams1, char *  szParams2, bool bGlobal)
{
	try
	{
		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		int nTopIndex = 0;
		
		if (pScript)
		{
			if(bGlobal)
				Npc[m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);

			pScript->SafeCallBegin(&nTopIndex);
			
			if ( ((!szParams1) || !szParams1[0]) && ((!szParams2) || !szParams2[0])) 
			{
				if (pScript->CallFunction(szFunName, 0,""))
				{
					bExecuteScriptMistake = false;
				}
			}
			else
			{
				if (pScript->CallFunction(szFunName, 0, "ssd", szParams1, szParams2, 0)) 
				{
					bExecuteScriptMistake = false;
				}
			}
			pScript->SafeCallEnd(nTopIndex);
		}
		
		if (bExecuteScriptMistake)
		{
			m_bWaitingPlayerFeedBack = false;
			m_btTryExecuteScriptTimes = 0;
			if (bGlobal)
				Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		return TRUE;
	}
	catch(...)
	{
		printf("Exception Have Caught When Execute Script[%u]!!!!!\n", dwScriptId);
		m_bWaitingPlayerFeedBack = false;
		m_btTryExecuteScriptTimes = 0;
		if (bGlobal)
			Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}
	return TRUE;
}

BOOL	KPlayer::ExecuteScript2(DWORD dwScriptId,  char * szFunName, int nParam1, int nParam2, bool bGlobal)
{
	try
	{
		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		int nTopIndex = 0;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		if (pScript)
		{
			Npc[m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);

			pScript->SafeCallBegin(&nTopIndex);
			if (pScript->CallFunction(szFunName, 0, "dd", nParam1, nParam2)) 
			{
				bExecuteScriptMistake = false;
			}
			pScript->SafeCallEnd(nTopIndex);
		}
		
		if (bExecuteScriptMistake)
		{
			m_bWaitingPlayerFeedBack = false;
			m_btTryExecuteScriptTimes = 0;
			Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		
		return TRUE;
	}
	catch(...)
	{
		printf("Exception Have Caught When Execute Script[%u]!!!!!\n", dwScriptId);
		m_bWaitingPlayerFeedBack = false;
		m_btTryExecuteScriptTimes = 0;
		Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}
	return TRUE;
}

BOOL	KPlayer::ExecuteScript2(char * ScriptFileName, char * szFunName, int nParam1, int nParam2, bool bGlobal)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	return ExecuteScript2(g_FileName2Id(ScriptFileName), szFunName, nParam1, nParam2, bGlobal);
}

BOOL	KPlayer::ExecuteScript2(char * ScriptFileName, char * szFunName, char *  szParams1, char *  szParams2, bool bGlobal)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0])
		return FALSE;
	DWORD dwScriptId = g_FileName2Id(ScriptFileName);
	return ExecuteScript2(dwScriptId, szFunName, szParams1, szParams2, bGlobal);
}

BOOL	KPlayer::ExecuteScript3(char * ScriptFileName, char * szFunName, int nParam1, int nParam2, int nParam3)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0])
		return FALSE;
	return ExecuteScript3(g_FileName2Id(ScriptFileName), szFunName, nParam1, nParam2, nParam3);	
}

BOOL	KPlayer::ExecuteScript3(char * ScriptFileName, char * szFunName, char *  szParams1, char *  szParams2, char *  szParams3)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0])
		return FALSE;
	DWORD dwScriptId = g_FileName2Id(ScriptFileName);
	return ExecuteScript3(dwScriptId, szFunName, szParams1, szParams2, szParams3);
}

BOOL	KPlayer::ExecuteScript3(DWORD dwScriptId, char * szFunName, char *  szParams1, char *  szParams2, char *  szParams3)
{
	try
	{
		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		int nTopIndex = 0;
		
		if (pScript)
		{
			Npc[m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);

			pScript->SafeCallBegin(&nTopIndex);
			
			if ( ((!szParams1) || !szParams1[0]) && ((!szParams2) || !szParams2[0]) && ((!szParams3) || !szParams3[0])) 
			{
				if (pScript->CallFunction(szFunName, 0,""))
				{
					bExecuteScriptMistake = false;
				}
			}
			else
			{
				if (pScript->CallFunction(szFunName, 0, "sssd", szParams1, szParams2, szParams3, 0)) 
				{
					bExecuteScriptMistake = false;
				}
			}
			pScript->SafeCallEnd(nTopIndex);
		}
		
		if (bExecuteScriptMistake)
		{
			m_bWaitingPlayerFeedBack = false;
			m_btTryExecuteScriptTimes = 0;
			Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		return TRUE;
	}
	catch(...)
	{
		printf("Exception Have Caught When Execute Script[%u]!!!!!\n", dwScriptId);
		m_bWaitingPlayerFeedBack = false;
		m_btTryExecuteScriptTimes = 0;
		Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}
	return TRUE;
}

BOOL	KPlayer::ExecuteScript3(DWORD dwScriptId,  char * szFunName, int nParam1, int nParam2, int nParam3)
{
	try
	{
		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		int nTopIndex = 0;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		if (pScript)
		{
			Npc[m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);

			pScript->SafeCallBegin(&nTopIndex);
			if (pScript->CallFunction(szFunName, 0, "ddd", nParam1, nParam2, nParam3)) 
			{
				bExecuteScriptMistake = false;
			}
			pScript->SafeCallEnd(nTopIndex);
		}
		
		if (bExecuteScriptMistake)
		{
			m_bWaitingPlayerFeedBack = false;
			m_btTryExecuteScriptTimes = 0;
			Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		
		return TRUE;
	}
	catch(...)
	{
		printf("Exception Have Caught When Execute Script[%u]!!!!!\n", dwScriptId);
		m_bWaitingPlayerFeedBack = false;
		m_btTryExecuteScriptTimes = 0;
		Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}
	return TRUE;
}

BOOL	KPlayer::DoScript(char * ScriptCommand)
{
	if (NULL == ScriptCommand) return FALSE;
	KLuaScript *Script = new KLuaScript;
	Script->Init();
	Script->RegisterFunctions(GameScriptFuns, g_GetGameScriptFunNum());
		
	Lua_PushNumber(Script->m_LuaState, m_nPlayerIndex);
	Script->SetGlobalName(SCRIPT_PLAYERINDEX);
	Lua_PushNumber(Script->m_LuaState, m_dwID);
	Script->SetGlobalName(SCRIPT_PLAYERID);
	
	Lua_PushNumber(Script->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
	Script->SetGlobalName(SCRIPT_SUBWORLDINDEX);
	
	if (Script->LoadBuffer((PBYTE)ScriptCommand, strlen(ScriptCommand))) 
	{
		BOOL bResult = Script->ExecuteCode();	
		delete Script; //Question!
		return bResult;
	}
	delete Script; //Question!
	return FALSE;
}

void	KPlayer::DoScriptAction(PLAYER_SCRIPTACTION_SYNC * pUIInfo) 
{
	if (!pUIInfo) return;
	
	if (pUIInfo->m_bParam2 == 1)
	{
#ifdef _SERVER
		pUIInfo->ProtocolType = (BYTE)s2c_scriptaction;
		pUIInfo->m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + pUIInfo->m_nBufferLen - 1;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)pUIInfo, pUIInfo->m_wProtocolLong + 1 );
#else
	}
	else  
	{
		OnScriptAction((PLAYER_SCRIPTACTION_SYNC *)pUIInfo);
#endif
	}
	
}

void	KPlayer::ProcessPlayerSelectFromUI(BYTE* pProtocol)				
{
	PLAYER_SELECTUI_COMMAND * pSelUI = (PLAYER_SELECTUI_COMMAND*) pProtocol;
	#ifdef _SERVER
	if (m_nIndex <= 0 || m_nIndex >= MAX_NPC)
	{
		printf("Xay ra loi ID 1 trong dao dien nguoi dung [%s] [%s]. Tu dong xua loi !\n",this->m_PlayerName,this->m_AccoutName);
		return;
	}

	#endif
	m_bWaitingPlayerFeedBack = false;
	
	if (pSelUI->nSelectIndex < 0) m_nAvailableAnswerNum = 0;
	if (m_nAvailableAnswerNum > pSelUI->nSelectIndex)
	{
		if (pSelUI->nSelectIndex < 0)
		{
			#ifdef _SERVER
			printf("Xay ra loi SL trong dao dien nguoi dung [%s] [%s]. Tu dong xua loi !\n",this->m_PlayerName,this->m_AccoutName);
			#endif
			return;
		}

		if (m_szTaskAnswerFun[pSelUI->nSelectIndex][0])
		{

			if (pSelUI->nSelectIndex >= 50 || pSelUI->nSelectIndex < 0)
			{
				#ifdef _SERVER
				printf("Xay ra loi TE trong dao dien nguoi dung [%s] [%s]. Tu dong xua loi !\n",this->m_PlayerName,this->m_AccoutName);
				#endif
				return;
			}

			if (m_nIndex <= 0 || m_nIndex >= MAX_NPC)
			{
				#ifdef _SERVER
				printf("Xay ra loi ID 2 trong dao dien nguoi dung [%s] [%s]. Tu dong xua loi !\n",this->m_PlayerName,this->m_AccoutName);
				#endif
				return;
			}
			if (m_nIndex)
			{
				if(m_szTaskAnswerFun[pSelUI->nSelectIndex][0] == '#')
					ExecuteScript(Npc[m_nIndex].m_ActionScriptID, m_szTaskAnswerFun[pSelUI->nSelectIndex] + 1, pSelUI->nSelectIndex);
				else {
					if (Npc[m_nIndex].m_ActionScriptID == 2051848301 && strcmp(m_szTaskAnswerFun[pSelUI->nSelectIndex], "yes") == 0) { //check TDP, Auto temporary fix
						return;
					}
					ExecuteScript(Npc[m_nIndex].m_ActionScriptID, m_szTaskAnswerFun[pSelUI->nSelectIndex], pSelUI->nSelectIndex);
				}
			}
			}
	}
}

#ifndef _SERVER
void	KPlayer::OnSelectFromUI(PLAYER_SELECTUI_COMMAND * pSelectUI, UIInfo eUIInfo)			
{
	if (!pSelectUI) return;
	
	switch(eUIInfo)
	{
	case UI_SELECTDIALOG:
		{
			if (g_bUISelIntelActiveWithServer)
			{
				pSelectUI->ProtocolType = (BYTE)c2s_playerselui;
				if (g_pClient)
					g_pClient->SendPackToServer((BYTE*)pSelectUI, sizeof(PLAYER_SELECTUI_COMMAND));
			}
			else
			{
				ProcessPlayerSelectFromUI((BYTE *)pSelectUI);		
			}
		}
		break;
	case UI_SAYNEW: //Say New
		{
			if (g_bUISelIntelActiveWithServer)
			{
				pSelectUI->ProtocolType = (BYTE)c2s_playerselui;
				if (g_pClient)
					g_pClient->SendPackToServer((BYTE*)pSelectUI, sizeof(PLAYER_SELECTUI_COMMAND));
			}
			else
			{
				ProcessPlayerSelectFromUI((BYTE *)pSelectUI);		
			}
		}
		break;
	case UI_TALKDIALOG:
		{
			if (g_bUISpeakActiveWithServer)
			{
				pSelectUI->ProtocolType = (BYTE)c2s_playerselui;
				if (g_pClient)
					g_pClient->SendPackToServer((BYTE*)pSelectUI, sizeof(PLAYER_SELECTUI_COMMAND));
			}
			else
			{
				ProcessPlayerSelectFromUI((BYTE *)pSelectUI);	
			}
			
		}break;
	}
}
#endif

//-------------------------------------------------------------------------
//	¹¦ÄÜ£º»ñµÃÍ¨ÖªÄ³ºÃÓÑÉÏÏßÁË
//-------------------------------------------------------------------------
void	KPlayer::ChatFriendOnLine(DWORD dwID, int nFriendIdx)
{
	if (nFriendIdx <= 0)
		return;
	for (int i = 0; i < MAX_FRIEND_TEAM; i++)
	{
		if (m_cChat.m_cFriendTeam[i].m_nFriendNo == 0)
			continue;
		CChatFriend	*pFriend;
		pFriend = (CChatFriend*)m_cChat.m_cFriendTeam[i].m_cEveryOne.GetHead();
		while (pFriend)
		{
			if (pFriend->m_dwID == dwID)
			{
				pFriend->m_nPlayerIdx = nFriendIdx;
#ifdef _SERVER
				CHAT_FRIEND_ONLINE_SYNC	sFriend;
				sFriend.ProtocolType = s2c_chatfriendonline;
				sFriend.m_dwID = dwID;
				sFriend.m_nPlayerIdx = nFriendIdx;
				g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sFriend, sizeof(CHAT_FRIEND_ONLINE_SYNC));
#else
				// Í¨Öª½çÃæÓÐºÃÓÑÉÏÏß
				KUiPlayerItem	sPlayer;
				strcpy(sPlayer.Name, pFriend->m_szName);
				sPlayer.uId = dwID;
				sPlayer.nIndex = nFriendIdx;
				sPlayer.nData = CHAT_S_ONLINE;
				CoreDataChanged(GDCNI_CHAT_FRIEND_STATUS, (unsigned int)&sPlayer, i);
				
				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, MSG_CHAT_FRIEND_ONLINE, pFriend->m_szName);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
#endif
				return;
			}
			pFriend = (CChatFriend*)pFriend->GetNext();
		}
	}
#ifdef _SERVER
	// Èç¹ûÃ»ÕÒµ½£¬ËµÃ÷±¾·½ÒÑ¾­É¾³ýÁË¶Ô·½£¬±ØÐëÍ¨Öª¶Ô·½É¾³ý
	Player[nFriendIdx].m_cChat.DeleteFriendData(m_dwID, nFriendIdx);
#endif
}

void	KPlayer::GetFactionName(char *lpszName, int nSize)
{
	this->m_cFaction.GetCurFactionName(lpszName);
	return;
}

void	KPlayer::GetFactionValueName(char *lpszName, int nSize)
{
	this->m_cFaction.GetCurFactionValueName(lpszName);
	return;
}

int	KPlayer::GetFactionNo()
{
	return this->m_cFaction.GetCurFactionNo();
}

#ifdef _SERVER
void	KPlayer::S2CExecuteScript(char * ScriptName, char * szParam = NULL)
{
	if (!ScriptName || (!ScriptName[0])) return; 
	
	PLAYER_SCRIPTACTION_SYNC  ScriptAction;
	ScriptAction.m_nOperateType = SCRIPTACTION_EXESCRIPT;
	ScriptAction.ProtocolType = s2c_scriptaction;
	char * script = NULL;
	if (szParam == NULL || szParam[0] == 0) 
	{
		ScriptAction.m_nBufferLen = strlen(ScriptName) + 1 ;
		strcpy(ScriptAction.m_pContent, ScriptName);
	}
	else
	{
		ScriptAction.m_nBufferLen = strlen(ScriptName) + 2 + strlen(szParam);
		sprintf(ScriptAction.m_pContent, "%s|%s", ScriptName, szParam);
	}
	ScriptAction.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - 300 + ScriptAction.m_nBufferLen - 1;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&ScriptAction, sizeof(PLAYER_SCRIPTACTION_SYNC) - 300 + ScriptAction.m_nBufferLen);	
	
}
#endif

#ifndef _SERVER
void	KPlayer::OnScriptAction(PLAYER_SCRIPTACTION_SYNC * pMsg)
{
	PLAYER_SCRIPTACTION_SYNC * pScriptAction = (PLAYER_SCRIPTACTION_SYNC *)pMsg;
	char szString[1024];
	
	switch(pScriptAction->m_nOperateType)
	{
	case SCRIPTACTION_UISHOW:
		{
			switch(pScriptAction->m_bUIId)
			{
			case UI_SELECTDIALOG:
				{
					KUiQuestionAndAnswer	*pQuest = NULL;
					KUiNpcSpr                *pImage = NULL;
					if (pScriptAction->m_nBufferLen <= 0) break;
					
					if (pScriptAction->m_bOptionNum <= 0)
						pQuest = (KUiQuestionAndAnswer *)malloc(sizeof(KUiQuestionAndAnswer));
					else
						pQuest = (KUiQuestionAndAnswer *)malloc(sizeof(KUiQuestionAndAnswer) + sizeof(KUiAnswer) * (pScriptAction->m_bOptionNum - 1));

					pImage = (KUiNpcSpr *)malloc(sizeof(KUiNpcSpr));
					char strContent[MAX_SCIRPTACTION_BUFFERNUM];
					char * pAnswer = NULL;
					pQuest->AnswerCount = 0;
					
					if (pScriptAction->m_bParam1 == 0)
					{
						g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen + 1);
						// [DaTau] chup nguyen van hoi thoai (cau hoi|dap an...) truoc khi tach/ma hoa
						g_StrCpyLen(g_sDTCap.szDlg, strContent, sizeof(g_sDTCap.szDlg));
						++g_sDTCap.uDlgSeq;
						pAnswer = strstr(strContent, "|");
						if (!pAnswer)
						{
							pScriptAction->m_bOptionNum = 0;
							pQuest->AnswerCount = 0;
						}
						else
						*pAnswer++ = 0;
						
						g_StrCpyLen(pQuest->Question, strContent, sizeof(pQuest->Question));
						if (m_nLastNpcIndex)
						TReplaceText(pQuest->Question, NPCNAME_KEY, Npc[m_nLastNpcIndex].Name);
						TReplaceText(pQuest->Question, PLAYERNAME_KEY, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
						TReplaceText(pQuest->Question, SEXNAME_KEY, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nSex? "C« n­¬ng":"ThiÕu hiÖp");
						pQuest->QuestionLen = TEncodeText(pQuest->Question, strlen(pQuest->Question));
					}
					
					else 
					{
						g_StrCpyLen(pQuest->Question, g_GetStringRes(*(int *)pScriptAction->m_pContent, szString, 1024), sizeof(pQuest->Question));

						if (m_nLastNpcIndex)
						TReplaceText(pQuest->Question, NPCNAME_KEY, Npc[m_nLastNpcIndex].Name);
						TReplaceText(pQuest->Question, PLAYERNAME_KEY, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
						TReplaceText(pQuest->Question, SEXNAME_KEY, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nSex? "C« n­¬ng":"ThiÕu hiÖp");

						pQuest->QuestionLen = TEncodeText(pQuest->Question, strlen(pQuest->Question));
						
						g_StrCpyLen(strContent, pScriptAction->m_pContent + sizeof(int), pScriptAction->m_nBufferLen - sizeof(int) + 1);
						pAnswer = strContent + 1;
					}
					
					for (int i = 0; i < pScriptAction->m_bOptionNum; i ++)
					{
						char * pNewAnswer = strstr(pAnswer, "|");
						
						if (pNewAnswer)
						{
							*pNewAnswer = 0;
							strcpy(pQuest->Answer[i].AnswerText, pAnswer);
							pQuest->Answer[i].AnswerLen = -1;
							pAnswer = pNewAnswer + 1;
						}
						else
						{
							strcpy(pQuest->Answer[i].AnswerText, pAnswer);
							pQuest->Answer[i].AnswerLen = -1;
							pQuest->AnswerCount = i + 1;
							break;
						}
					}
					
					g_bUISelIntelActiveWithServer = pScriptAction->m_bParam2;
					g_bUISelLastSelCount = pQuest->AnswerCount;
					

					switch (pScriptAction->m_Select)
					{
					case 0:
						{
							CoreDataChanged(GDCNI_QUESTION_CHOOSE,(unsigned int) pQuest, 0);
						}
						break;
					case 1:
						{
							if (m_nImageNpcID) 
							{
								char szBuffer[128]; 
								for (int i = 0; i < 16; i++) 
								{ 
									if(Npc[m_nImageNpcID].GetNpcRes()->m_pcResNode)//fix by phong kiÒu res npc null
									{
										Npc[m_nImageNpcID].GetNpcRes()->m_pcResNode->GetFileName(i, 3, 0, "", szBuffer, sizeof(szBuffer)); 
										if (szBuffer[0]) 
										{ 
											strcpy(pImage->ImageFile, szBuffer); 
											pImage->MaxFrame = (Npc[m_nImageNpcID].GetNpcRes()->m_pcResNode->GetTotalFrames(i, 3, 0, 16))/ (Npc[m_nImageNpcID].GetNpcRes()->m_pcResNode->GetTotalDirs(i, 3, 0, 16)); 
											break; 
										} 
									}
								}
								for (int j = 0; j < 16; j++)
								{  
									if(Npc[m_nImageNpcID].GetNpcRes()->m_pcResNode)//fix by phong kiÒu res npc null
									{
										Npc[m_nImageNpcID].GetNpcRes()->m_pcResNode->GetFileName(j, 0, 0, "", szBuffer, sizeof(szBuffer)); 
										if (szBuffer[0]) 
										{ 
											strcpy(pImage->ImageFile, szBuffer);
											pImage->MaxFrame = (Npc[m_nImageNpcID].GetNpcRes()->m_pcResNode->GetTotalFrames(j, 0, 0, 16))/ (Npc[m_nImageNpcID].GetNpcRes()->m_pcResNode->GetTotalDirs(j, 0, 0, 16)); 
											break; 
										} 
									}
								}
							}
							CoreDataChanged(GDCNI_QUESTION_CHOOSE,(unsigned int) pQuest, (int) pImage);
							free(pImage); 
							pImage = NULL; 
						}
						break;
					case 2:
						//CoreDataChanged(GDCNI_QUESTION_CHOOSE_3,(unsigned int) pQuest, m_ImageId);
						CoreDataChanged(GDCNI_QUESTION_CHOOSE_4,(unsigned int) pQuest, m_ImageId);
						break;
					}

					/******************************************the end********************************************************/ 
					free(pQuest); 
					pQuest = NULL; 
				}
				break;  
				
			case UI_TALKDIALOG: 
				{
					BOOL bUsingSpeakId = pScriptAction->m_bParam1;
					int	 nSentenceCount = pScriptAction->m_bOptionNum;
					if (nSentenceCount <= 0) return ;
					KUiInformationParam *pSpeakList  = new KUiInformationParam[nSentenceCount];
					memset(pSpeakList, 0, sizeof(KUiInformationParam) * nSentenceCount);
					
					char * pAnswer = new char [pScriptAction->m_nBufferLen + 1];
					char * pBackupAnswer = pAnswer;
					g_StrCpyLen(pAnswer, pScriptAction->m_pContent, pScriptAction->m_nBufferLen + 1);
						// [DaTau] chup noi dung Talk truoc khi tach/ma hoa
						g_StrCpyLen(g_sDTCap.szTalk, pAnswer, sizeof(g_sDTCap.szTalk));
						++g_sDTCap.uTalkSeq;
					
					int nCount = 0;
					for (int i = 0; i < pScriptAction->m_bOptionNum; i ++)
					{
						char * pNewAnswer = strstr(pAnswer, "|");
						
						if (pNewAnswer)
						{
							*pNewAnswer = 0;
							if (!bUsingSpeakId)
							{
								strcpy(pSpeakList[i].sInformation, pAnswer);
								if (m_nLastNpcIndex)
								TReplaceText(pSpeakList[i].sInformation, NPCNAME_KEY, Npc[m_nLastNpcIndex].Name);
								TReplaceText(pSpeakList[i].sInformation, PLAYERNAME_KEY, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
								TReplaceText(pSpeakList[i].sInformation, SEXNAME_KEY, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nSex? "C« n­¬ng":"ThiÕu hiÖp");								
							}
							else
							{
								strcpy(pSpeakList[i].sInformation, g_GetStringRes(atoi(pAnswer), szString, sizeof(szString)));
								if (m_nLastNpcIndex)
								TReplaceText(pSpeakList[i].sInformation, NPCNAME_KEY, Npc[m_nLastNpcIndex].Name);
								TReplaceText(pSpeakList[i].sInformation, PLAYERNAME_KEY, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
								TReplaceText(pSpeakList[i].sInformation, SEXNAME_KEY, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nSex? "C« n­¬ng":"ThiÕu hiÖp");
							}
							
							if (i < pScriptAction->m_bOptionNum - 1)
							{
								strcpy(pSpeakList[i].sConfirmText, "sConfirmText1 ¼ÌÐø");
							}
							else 
							{
								strcpy(pSpeakList[i].sConfirmText, "sConfirmText2 Íê³É");
								if (pScriptAction->m_nParam == 1)						
								pSpeakList[i].bNeedConfirmNotify = TRUE;
							}
							pSpeakList[i].nInforLen = TEncodeText(pSpeakList[i].sInformation, strlen(pSpeakList[i].sInformation));
							pAnswer = pNewAnswer + 1;
						}
						else
						{
							if (!bUsingSpeakId)
							{
								strcpy(pSpeakList[i].sInformation, pAnswer);
								if (m_nLastNpcIndex)
								TReplaceText(pSpeakList[i].sInformation, NPCNAME_KEY, Npc[m_nLastNpcIndex].Name);
								TReplaceText(pSpeakList[i].sInformation, PLAYERNAME_KEY, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
								TReplaceText(pSpeakList[i].sInformation, SEXNAME_KEY, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nSex? "C« n­¬ng":"ThiÕu hiÖp");
							}
							else
							{
								strcpy(pSpeakList[i].sInformation, g_GetStringRes(atoi(pAnswer), szString ,sizeof(szString)));
								if (m_nLastNpcIndex)
								TReplaceText(pSpeakList[i].sInformation, NPCNAME_KEY, Npc[m_nLastNpcIndex].Name);
								TReplaceText(pSpeakList[i].sInformation, PLAYERNAME_KEY, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
								TReplaceText(pSpeakList[i].sInformation, SEXNAME_KEY, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nSex? "C« n­¬ng":"ThiÕu hiÖp");
							}
							
							strcpy(pSpeakList[i].sConfirmText, "sConfirmText3 Íê³É");
							
							if (pScriptAction->m_nParam == 1)						
							pSpeakList[i].bNeedConfirmNotify = TRUE;
							
							pSpeakList[i].nInforLen = TEncodeText(pSpeakList[i].sInformation, strlen(pSpeakList[i].sInformation));
							nCount++;
							break;
						}
						nCount ++;
					}
					if (pBackupAnswer)
					{
						delete []pBackupAnswer;
						pBackupAnswer = NULL;
					}
					g_bUISpeakActiveWithServer = pScriptAction->m_bParam2;
					CoreDataChanged(GDCNI_SPEAK_WORDS,(unsigned int)pSpeakList, nCount);
					if (pSpeakList)
					{
						delete pSpeakList;	
						pSpeakList = NULL;
					}
				}
				break;
			case UI_SAYNEW:
				{
					KUiQuestionAndAnswer	*pQuest = NULL;
					if (pScriptAction->m_nBufferLen <= 0) break;
					
					if (pScriptAction->m_bOptionNum <= 0)
						pQuest = (KUiQuestionAndAnswer *)malloc(sizeof(KUiQuestionAndAnswer));
					else
						pQuest = (KUiQuestionAndAnswer *)malloc(sizeof(KUiQuestionAndAnswer) + sizeof(KUiQuestionAndAnswer) * (pScriptAction->m_bOptionNum - 1));
					
					char strContent[1024];
					char * pAnswer = NULL;
					pQuest->AnswerCount = 0;
					if (pScriptAction->m_bParam1 == 0)
					{
						g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen + 1);
						pAnswer = strstr(strContent, "|");
						if (!pAnswer)
						{
							pScriptAction->m_bOptionNum = 0;
							pQuest->AnswerCount = 0;
						}
						else
						*pAnswer++ = 0;
						
						g_StrCpyLen(pQuest->Question, strContent, sizeof(pQuest->Question));
						pQuest->QuestionLen = TEncodeText(pQuest->Question, strlen(pQuest->Question));
					}
					else 
					{
						g_StrCpyLen(pQuest->Question, g_GetStringRes(*(int *)pScriptAction->m_pContent, szString, 1024), sizeof(pQuest->Question));
						pQuest->QuestionLen = TEncodeText(pQuest->Question, strlen(pQuest->Question));
						
						g_StrCpyLen(strContent, pScriptAction->m_pContent + sizeof(int), pScriptAction->m_nBufferLen - sizeof(int) + 1);
						pAnswer = strContent + 1;
					}
					
					for (int i = 0; i < pScriptAction->m_bOptionNum; i ++)
					{
						char * pNewAnswer = strstr(pAnswer, "|");
						
						if (pNewAnswer)
						{
							*pNewAnswer = 0;
							strcpy(pQuest->Answer[i].AnswerText, pAnswer);
							pQuest->Answer[i].AnswerLen = -1;
							pAnswer = pNewAnswer + 1;
						}
						else
						{
							strcpy(pQuest->Answer[i].AnswerText, pAnswer);
							pQuest->Answer[i].AnswerLen = -1;
							pQuest->AnswerCount = i + 1;
							break;
						}
					}
					
					g_bUISelIntelActiveWithServer = pScriptAction->m_bParam2;
					g_bUISelLastSelCount = pQuest->AnswerCount;
					CoreDataChanged(GDCNI_QUESTION_CHOOSE_4,(unsigned int) pQuest, 0);
					free(pQuest);
					pQuest = NULL;
				}
				break;
			case UI_NOTEINFO:
				{
					if (pScriptAction->m_nBufferLen <= 0) 
					break;
					KMissionRecord Record;
					if (pScriptAction->m_bParam1 == 0)
					{
						g_StrCpyLen(Record.sContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen - sizeof(int) - 1);
						Record.uValue = *(unsigned int*) (pScriptAction->m_pContent + pScriptAction->m_nBufferLen - sizeof(int));
					}
					else
					{
						g_GetStringRes(*(int *)pScriptAction->m_pContent, Record.sContent ,sizeof(Record.sContent));
						Record.uValue = *(unsigned int* ) (Record.sContent + sizeof(int));
					}
					
					//if (m_nLastNpcIndex)
					//TReplaceText(Record.szContent, NPCNAME_KEY, Npc[m_nLastNpcIndex].Name);
					//TReplaceText(Record.szContent, PLAYERNAME_KEY, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
					//TReplaceText(Record.szContent, SEXNAME_KEY, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nSex? "C« n­¬ng":"ThiÕu hiÖp");	
					
					Record.nContentLen = TEncodeText(Record.sContent, strlen(Record.sContent));
					CoreDataChanged(GDCNI_MISSION_RECORD, (unsigned long)&Record, 0);
				}
				break;
			case UI_MSGINFO:
				{
					if (pScriptAction->m_nBufferLen <= 0) 
					break;
					
					char strContent[1024];
					if (pScriptAction->m_bParam1 == 0)
					{
						g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen + 1);
					}
					else
					{
						g_GetStringRes(*(int *)pScriptAction->m_pContent, strContent ,sizeof(strContent));
					}
					
					KSystemMessage	sMsg;
					sMsg.eType = SMT_PLAYER;
					sMsg.byConfirmType = SMCT_MSG_BOX;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					g_StrCpyLen(sMsg.szMessage, strContent, sizeof(sMsg.szMessage));
					
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					
				}
				break;
				
			case UI_NEWSINFO:
				{
					if (pScriptAction->m_nBufferLen <= 0) 
					break;
					switch(pScriptAction->m_bOptionNum) 
					{
					case NEWSMESSAGE_NORMAL:
						{
							
							KNewsMessage News;
							News.nType = NEWSMESSAGE_NORMAL;
							
							char strContent[1024];
							if (pScriptAction->m_bParam1 == 0)
							{
								g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen + 1);
							}
							else
							{
								g_GetStringRes(*(int *)pScriptAction->m_pContent, strContent ,sizeof(strContent));
							}
							
							g_StrCpyLen(News.sMsg,  strContent, sizeof(News.sMsg));

							News.nMsgLen = TEncodeText(News.sMsg, strlen(News.sMsg));
							CoreDataChanged(GDCNI_NEWS_MESSAGE, (unsigned int)&News, 0);
						}
						break;

					case NEWSMESSAGE_COUNTING:
						{
							KNewsMessage News;
							News.nType = pScriptAction->m_bOptionNum;
							
							char strContent[MAX_SCIRPTACTION_BUFFERNUM];
							int nTime = 0;

							if (pScriptAction->m_bParam1 == 0)
							{
								g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen - sizeof(int) + 1);
							}
							else
							{
								g_GetStringRes(*(int *)pScriptAction->m_pContent, strContent ,sizeof(strContent));
							}
							
							g_StrCpyLen(News.sMsg,  strContent, sizeof(News.sMsg));
							News.nMsgLen = TEncodeText(News.sMsg, strlen(News.sMsg));
							SYSTEMTIME systime;
							
							memset(&systime, 0, sizeof(SYSTEMTIME));
							systime.wSecond = *(int *)((pScriptAction->m_pContent + pScriptAction->m_nBufferLen - sizeof(int)));
							
							CoreDataChanged(GDCNI_NEWS_MESSAGE, (unsigned int)&News, (unsigned int)&systime);
							
						}
						break;

					case NEWSMESSAGE_TIMEEND:
						{
							KNewsMessage News;
							News.nType = pScriptAction->m_bOptionNum;
							
							char strContent[MAX_SCIRPTACTION_BUFFERNUM];
							int nTime = 0;

							if (pScriptAction->m_bParam1 == 0)
							{
								g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen - sizeof(SYSTEMTIME) + 1);
							}
							else
							{
								g_GetStringRes(*(int *)pScriptAction->m_pContent, strContent ,sizeof(strContent));
							}
							
							g_StrCpyLen(News.sMsg,  strContent, sizeof(News.sMsg));
							News.nMsgLen = TEncodeText(News.sMsg, strlen(News.sMsg));
							SYSTEMTIME systime;
							systime = *(SYSTEMTIME*)((pScriptAction->m_pContent + pScriptAction->m_nBufferLen - sizeof(SYSTEMTIME)));
							CoreDataChanged(GDCNI_NEWS_MESSAGE, (unsigned int)&News, (unsigned int)&systime);
							
						}
						break;
					}
				}
				break;
			case UI_NEWSINFO_1:
				{
					if (pScriptAction->m_nBufferLen <= 0) 
					break;
					switch(pScriptAction->m_bOptionNum) 
					{
					case NEWSMESSAGE_NORMAL_1:
						{
							
							KNewsMessage1 News;
							News.nType = NEWSMESSAGE_NORMAL_1;
							
							char strContent[MAX_SCIRPTACTION_BUFFERNUM];

							if (pScriptAction->m_bParam1 == 0)
							{
								g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen + 1);
							}
							else
							{
								g_GetStringRes(*(int *)pScriptAction->m_pContent, strContent ,sizeof(strContent));
							}
							
							g_StrCpyLen(News.sMsg,  strContent, sizeof(News.sMsg));

							News.nMsgLen = TEncodeText(News.sMsg, strlen(News.sMsg));
							CoreDataChanged(GDCNI_NEWS_MESSAGE_1, (unsigned int)&News, 0);
						}
						break;

					case NEWSMESSAGE_COUNTING_1:
						{
							KNewsMessage1 News;
							News.nType = pScriptAction->m_bOptionNum;
							
							char strContent[MAX_SCIRPTACTION_BUFFERNUM];
							int nTime = 0;

							if (pScriptAction->m_bParam1 == 0)
							{
								g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen - sizeof(int) + 1);
							}
							else
							{
								g_GetStringRes(*(int *)pScriptAction->m_pContent, strContent ,sizeof(strContent));
							}
							
							g_StrCpyLen(News.sMsg,  strContent, sizeof(News.sMsg));
							News.nMsgLen = TEncodeText(News.sMsg, strlen(News.sMsg));
							SYSTEMTIME systime;
							
							memset(&systime, 0, sizeof(SYSTEMTIME));
							systime.wSecond = *(int *)((pScriptAction->m_pContent + pScriptAction->m_nBufferLen - sizeof(int)));
							
							CoreDataChanged(GDCNI_NEWS_MESSAGE_1, (unsigned int)&News, (unsigned int)&systime);
							
						}
						break;
						
					case NEWSMESSAGE_TIMEEND_1:
						{
							KNewsMessage1 News;
							News.nType = pScriptAction->m_bOptionNum;
							
							char strContent[MAX_SCIRPTACTION_BUFFERNUM];
							int nTime = 0;
							if (pScriptAction->m_bParam1 == 0)
							{
								g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen - sizeof(SYSTEMTIME) + 1);
							}
							else
							{
								g_GetStringRes(*(int *)pScriptAction->m_pContent, strContent ,sizeof(strContent));
							}
							
							g_StrCpyLen(News.sMsg,  strContent, sizeof(News.sMsg));
							News.nMsgLen = TEncodeText(News.sMsg, strlen(News.sMsg));
							SYSTEMTIME systime;
							systime = *(SYSTEMTIME*)((pScriptAction->m_pContent + pScriptAction->m_nBufferLen - sizeof(SYSTEMTIME)));
							CoreDataChanged(GDCNI_NEWS_MESSAGE_1, (unsigned int)&News, (unsigned int)&systime);
						}
						break;
					}
				}
				break;
			case UI_PLAYMUSIC:
				{
					char szMusicFile[MAX_PATH];
					memcpy(szMusicFile, pScriptAction->m_pContent, pScriptAction->m_nBufferLen);
					szMusicFile[pScriptAction->m_nBufferLen] = 0;
					g_SubWorldSet.m_cMusic.ScriptPlay(szMusicFile);
				}
				break;
				
			case UI_OPENTONGUI:				
				{
					Player[CLIENT_PLAYER_INDEX].m_cTong.OpenCreateInterface();
					break;
				}
			case UI_OPENTONGJX2:	// JX2 port
				{
					CoreDataChanged(GDCNI_TONG_JX2, 0, -1);
					Player[CLIENT_PLAYER_INDEX].m_cTong.JX2_RequestView(0, 0);
					break;
				}
			case UI_TASKVALUE:	// [TaskGuide] task value id >= 256 tu server
				{
					if (pScriptAction->m_nBufferLen < (int)(sizeof(int) * 2))
						break;
					int nTaskId  = *(int*)(pScriptAction->m_pContent);
					int nTaskVal = *(int*)(pScriptAction->m_pContent + sizeof(int));
					if (nTaskId == -1)
					{
						// dau moi phien nap nhan vat: xoa sach gia tri nhan vat truoc
						memset(Player[CLIENT_PLAYER_INDEX].m_cTask.nSave, 0,
							sizeof(Player[CLIENT_PLAYER_INDEX].m_cTask.nSave));
					}
					else
					{
						Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(nTaskId, nTaskVal);
					}
					CoreDataChanged(GDCNI_TASK_VALUE_UPDATE, (unsigned int)nTaskId, nTaskVal);
					break;
				}
			}
		} 
		break;
	case SCRIPTACTION_EXESCRIPT:
		{
			if (pScriptAction->m_nBufferLen <= 0 ) break;
			char szScriptInfo[1000];
			g_StrCpyLen(szScriptInfo, pScriptAction->m_pContent,pScriptAction->m_nBufferLen + 1);
			char * pDivPos = strstr(szScriptInfo, "/");
			if (pDivPos)	*pDivPos++ = 0; 
			if (pDivPos)
			ExecuteScript(szScriptInfo, "OnCall", pDivPos);
			else
			ExecuteScript(szScriptInfo, "OnCall", "");
		}
		break;
	}
}
#endif

#ifdef _SERVER
int KPlayer::AddTempTaskValue(void* pData)
{
	memcpy(m_cTask.nClear, pData, sizeof(int) * MAX_TEMP_TASK);
	return 1;
}

void KPlayer::DialogNpc(BYTE * pProtocol)
{
	//if (m_nAvailableAnswerNum > 0) return ;
	PLAYER_DIALOG_NPC_COMMAND * pDialogNpc = (PLAYER_DIALOG_NPC_COMMAND*) pProtocol;
	int nIdx = 0;
	if (pDialogNpc->nNpcId < 0) return;
	nIdx = FindAroundNpc(pDialogNpc->nNpcId);//NpcSet.SearchID(pDialogNpc->nNpcId);
	if (nIdx > 0) 
	{
		int distance = NpcSet.GetDistance(nIdx, m_nIndex);

		if ((Npc[nIdx].m_Kind == kind_dialoger) || (NpcSet.GetRelation(m_nIndex, nIdx) == relation_none))
		{
			if (distance <= Npc[nIdx].m_DialogRadius * 3)
			{
				if (Npc[nIdx].ActionScript[0])
				{
					// [WLLS 21/08] luu NPC dang thoai cho GetLastDiagNpc (wlls_npcname)
					m_nWllsLastDiagNpc = nIdx;
					ExecuteScript(Npc[nIdx].m_ActionScriptID, "main", nIdx); //#edit by phong kieu send nIdx qua lua //#can kiem tra
					if(g_WriteScriptNpcLog)
					{
						SYSTEMTIME systm;
						::GetLocalTime(&systm);
						TCHAR buff[64];
						int len = 0;
						sprintf(buff, "%04d/%02d/%02d %02d:%02d:%02d.%03d", systm.wYear, systm.wMonth, systm.wDay, systm.wHour, systm.wMinute, systm.wSecond, systm.wMilliseconds);
						FILE* pFile = fopen("ScripNpcDialog.log", "a");
						fprintf(pFile, "%s \t [%s] \t %s\n", buff , Npc[nIdx].Name ,Npc[nIdx].ActionScript);
						fclose(pFile);
					}
				}
			}
		}
	}
}
#endif

#ifndef _SERVER
void	KPlayer::s2cTradeChangeState(BYTE* pMsg)
{
	TRADE_CHANGE_STATE_SYNC	*pTrade = (TRADE_CHANGE_STATE_SYNC*)pMsg;
	
	switch (m_cMenuState.m_nState)
	{
	case PLAYER_MENU_STATE_NORMAL:		// µ±Ç°¿Í»§¶Ë NORMAL
	case PLAYER_MENU_STATE_TRADEOPEN:	// µ±Ç°¿Í»§¶Ë TRADEOPEN
		if (pTrade->m_btState == 0)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);
			CoreDataChanged(GDCNI_TRADE_OPER_DATA, 0, 0);
		}
		else if (pTrade->m_btState == 1)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_TRADEOPEN);
			CoreDataChanged(GDCNI_TRADE_OPER_DATA, 0, 0);
		}
		else if (pTrade->m_btState == 2)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_TRADING);
			m_cTrade.StartTrade(pTrade->m_dwNpcID);
			m_ItemList.StartTrade();
			
			KUiPlayerItem	sTradePlayer;
			int	nIdx;
			
			nIdx = NpcSet.SearchID(pTrade->m_dwNpcID);
			if (nIdx > 0)
				strcpy(sTradePlayer.Name, Npc[nIdx].Name);
			else
				sTradePlayer.Name[0] = 0;
			sTradePlayer.nIndex = 0;
			sTradePlayer.uId = 0;
			sTradePlayer.nData = 0;
			CoreDataChanged(GDCNI_TRADE_START, (unsigned int)(&sTradePlayer), 0);
			
			if (Npc[m_nIndex].m_Doing == do_sit)
			{
				Npc[m_nIndex].SendCommand(do_stand);
			}
		}
		break;
	case PLAYER_MENU_STATE_TEAMOPEN:	// µ±Ç°¿Í»§¶Ë TEAMOPEN
		g_Team[0].SetTeamClose();
		if (pTrade->m_btState == 0)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);
			CoreDataChanged(GDCNI_TRADE_OPER_DATA, 0, 0);
		}
		else if (pTrade->m_btState == 1)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_TRADEOPEN);
			CoreDataChanged(GDCNI_TRADE_OPER_DATA, 0, 0);
		}
		else if (pTrade->m_btState == 2)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_TRADING);
			m_cTrade.StartTrade(pTrade->m_dwNpcID);
			m_ItemList.StartTrade();
			
			// Í¨Öª½çÃæ½øÈë½»Ò×½çÃæ
			KUiPlayerItem	sTradePlayer;
			int	nIdx;
			
			nIdx = NpcSet.SearchID(pTrade->m_dwNpcID);
			if (nIdx > 0)
				strcpy(sTradePlayer.Name, Npc[nIdx].Name);
			else
				sTradePlayer.Name[0] = 0;
			sTradePlayer.Name[0] = 0;
			sTradePlayer.nIndex = 0;
			sTradePlayer.uId = 0;
			sTradePlayer.nData = 0;
			CoreDataChanged(GDCNI_TRADE_START, (unsigned int)(&sTradePlayer), 0);
			
			if (Npc[m_nIndex].m_Doing == do_sit)
			{
				Npc[m_nIndex].SendCommand(do_stand);
			}
		}
		break;
	case PLAYER_MENU_STATE_TRADING:		

		if (pTrade->m_btState == 0)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);
			CoreDataChanged(GDCNI_TRADE_OPER_DATA, 0, 0);
		}
		else if (pTrade->m_btState == 1)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_TRADEOPEN);
			CoreDataChanged(GDCNI_TRADE_OPER_DATA, 0, 0);
		}
		else if (pTrade->m_btState == 2)	
		{
			m_cMenuState.m_nState = PLAYER_MENU_STATE_TRADING;
			m_cMenuState.m_nTradeDest = pTrade->m_dwNpcID;
			m_cMenuState.m_nTradeState = 0;
			m_cMenuState.m_nTradeDestState = 0;
			this->m_ItemList.RecoverTrade();
			this->m_ItemList.BackupTrade();
			this->m_ItemList.ClearRoom(room_trade);
			this->m_ItemList.ClearRoom(room_trade1);
			
			KUiPlayerItem	sTradePlayer;
			int	nIdx;
			
			nIdx = NpcSet.SearchID(pTrade->m_dwNpcID);
			if (nIdx > 0)
				strcpy(sTradePlayer.Name, Npc[nIdx].Name);
			else
				sTradePlayer.Name[0] = 0;
			sTradePlayer.Name[0] = 0;
			sTradePlayer.nIndex = 0;
			sTradePlayer.uId = 0;
			sTradePlayer.nData = 0;
			CoreDataChanged(GDCNI_TRADE_START, (unsigned int)(&sTradePlayer), 0);
			
			if (Npc[m_nIndex].m_Doing == do_sit)
			{
				Npc[m_nIndex].SendCommand(do_stand);
			}
		}
		break;
	}
}
#endif

#ifndef _SERVER
void	KPlayer::s2cGambleChangeState(BYTE* pMsg)
{
	GAMBLE_CHANGE_STATE_SYNC* pGamble = (GAMBLE_CHANGE_STATE_SYNC*)pMsg;

	switch (m_cMenuState.m_nState)
	{
	case PLAYER_MENU_STATE_NORMAL:		// µ±Ç°¿Í»§¶Ë NORMAL
		//case PLAYER_MENU_STATE_TRADEOPEN:	// µ±Ç°¿Í»§¶Ë TRADEOPEN
		if (pGamble->m_btState == 0)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);
			CoreDataChanged(GDCNI_GAMBLE_OPER_DATA, 0, 0);
		}
		else if (pGamble->m_btState == 1)
		{
			//m_cMenuState.SetState(PLAYER_MENU_STATE_TRADEOPEN);
			CoreDataChanged(GDCNI_GAMBLE_OPER_DATA, 0, 0);
		}
		else if (pGamble->m_btState == 2)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_GAMBLING);
			m_cTrade.StartTrade(pGamble->m_dwNpcID);
			m_ItemList.StartTrade();

			KUiPlayerItem	sGamblePlayer;
			int	nIdx;

			nIdx = NpcSet.SearchID(pGamble->m_dwNpcID);
			if (nIdx > 0)
				strcpy(sGamblePlayer.Name, Npc[nIdx].Name);
			else
				sGamblePlayer.Name[0] = 0;
			sGamblePlayer.nIndex = 0;
			sGamblePlayer.uId = 0;
			sGamblePlayer.nData = 0;
			CoreDataChanged(GDCNI_GAMBLE_START, (unsigned int)(&sGamblePlayer), 0);

			if (Npc[m_nIndex].m_Doing == do_sit)
			{
				Npc[m_nIndex].SendCommand(do_stand);
			}
		}
		break;
	case PLAYER_MENU_STATE_TEAMOPEN:	// µ±Ç°¿Í»§¶Ë TEAMOPEN
		g_Team[0].SetTeamClose();
		if (pGamble->m_btState == 0)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);
			CoreDataChanged(GDCNI_GAMBLE_OPER_DATA, 0, 0);
		}
		else if (pGamble->m_btState == 1)
		{
			//m_cMenuState.SetState(PLAYER_MENU_STATE_TRADEOPEN);
			CoreDataChanged(GDCNI_GAMBLE_OPER_DATA, 0, 0);
		}
		else if (pGamble->m_btState == 2)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_GAMBLING);
			m_cTrade.StartTrade(pGamble->m_dwNpcID);
			m_ItemList.StartTrade();

			// Í¨Öª½çÃæ½øÈë½»Ò×½çÃæ
			KUiPlayerItem	sGamblePlayer;
			int	nIdx;

			nIdx = NpcSet.SearchID(pGamble->m_dwNpcID);
			if (nIdx > 0)
				strcpy(sGamblePlayer.Name, Npc[nIdx].Name);
			else
				sGamblePlayer.Name[0] = 0;
			sGamblePlayer.Name[0] = 0;
			sGamblePlayer.nIndex = 0;
			sGamblePlayer.uId = 0;
			sGamblePlayer.nData = 0;
			CoreDataChanged(GDCNI_GAMBLE_START, (unsigned int)(&sGamblePlayer), 0);

			if (Npc[m_nIndex].m_Doing == do_sit)
			{
				Npc[m_nIndex].SendCommand(do_stand);
			}
		}
		break;
	case PLAYER_MENU_STATE_GAMBLING:

		if (pGamble->m_btState == 0)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);
			CoreDataChanged(GDCNI_GAMBLE_OPER_DATA, 0, 0);
		}
		else if (pGamble->m_btState == 1)
		{
			//m_cMenuState.SetState(PLAYER_MENU_STATE_TRADEOPEN);
			CoreDataChanged(GDCNI_GAMBLE_OPER_DATA, 0, 0);
		}
		else if (pGamble->m_btState == 2)
		{
			m_cMenuState.m_nState = PLAYER_MENU_STATE_GAMBLING;
			m_cMenuState.m_nTradeDest = pGamble->m_dwNpcID;
			m_cMenuState.m_nTradeState = 0;
			m_cMenuState.m_nTradeDestState = 0;
			//this->m_ItemList.RecoverTrade();
			this->m_ItemList.BackupTrade();
			this->m_ItemList.ClearRoom(room_trade);
			this->m_ItemList.ClearRoom(room_trade1);

			KUiPlayerItem	sGamblePlayer;
			int	nIdx;

			nIdx = NpcSet.SearchID(pGamble->m_dwNpcID);
			if (nIdx > 0)
				strcpy(sGamblePlayer.Name, Npc[nIdx].Name);
			else
				sGamblePlayer.Name[0] = 0;
			sGamblePlayer.Name[0] = 0;
			sGamblePlayer.nIndex = 0;
			sGamblePlayer.uId = 0;
			sGamblePlayer.nData = 0;
			CoreDataChanged(GDCNI_GAMBLE_START, (unsigned int)(&sGamblePlayer), 0);

			if (Npc[m_nIndex].m_Doing == do_sit)
			{
				Npc[m_nIndex].SendCommand(do_stand);
			}
		}
		break;
	}
}
#endif

#ifndef _SERVER
void	KPlayer::s2cLevelUp(BYTE* pMsg)
{
	PLAYER_LEVEL_UP_SYNC	*pLevel = (PLAYER_LEVEL_UP_SYNC*)pMsg;

	if ((DWORD)Npc[m_nIndex].m_Level < (DWORD)pLevel->m_btLevel && m_nExp < PlayerSet.m_cLevelAdd.GetLevelExp(Npc[m_nIndex].m_Level))
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_GET_EXP, PlayerSet.m_cLevelAdd.GetLevelExp(Npc[m_nIndex].m_Level) - m_nExp);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	
	m_nExp = 0;
	Npc[m_nIndex].m_Level = (DWORD)pLevel->m_btLevel;
	m_nNextLevelExp = PlayerSet.m_cLevelAdd.GetLevelExp(Npc[m_nIndex].m_Level);
	
	Npc[m_nIndex].UpdateGameTitle();
	
	// -------------------------- µÈ¼¶Ôö¼ÓÊ±¸÷ÏîÊôÐÔÍ¬²½ ----------------------
	// Í¬²½Î´·ÖÅäÊôÐÔµã
	if (pLevel->m_nAttributePoint > m_nAttributePoint)
	{
		KSystemMessage Msg;
		sprintf(Msg.szMessage, MSG_GET_ATTRIBUTE_POINT, pLevel->m_nAttributePoint - m_nAttributePoint);
		Msg.eType = SMT_PLAYER;
		Msg.byConfirmType = SMCT_UI_ATTRIBUTE;
		Msg.byPriority = 3;
		Msg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, NULL);
	}
	m_nAttributePoint = pLevel->m_nAttributePoint;
	
	// Í¬²½Î´·ÖÅä¼¼ÄÜµã
	if (m_nSkillPoint < pLevel->m_nSkillPoint)
	{
		KSystemMessage Msg;
		sprintf(Msg.szMessage, MSG_GET_SKILL_POINT, pLevel->m_nSkillPoint - m_nSkillPoint);
		Msg.eType = SMT_PLAYER;
		Msg.byConfirmType = SMCT_UI_SKILLS;
		Msg.byPriority = 3;
		Msg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, NULL);
	}
	m_nSkillPoint = pLevel->m_nSkillPoint;
	
	// ´¦ÀíÉúÃü¡¢ÌåÁ¦¡¢ÄÚÁ¦Öµ£¨Ó°Ïì»ù±¾ÉúÃü¡¢ÌåÁ¦¡¢ÄÚÁ¦×î´óÖµµÄÒòËØ£ºµÈ¼¶¡¢ÊôÐÔµã£¬Ó°Ïìµ±Ç°ÉúÃü×î´óÖµµÄÒòËØ£º×°±¸¡¢¼¼ÄÜ¡¢Ò©Îï£¨ÁÙÊ±£©£©
	Npc[m_nIndex].m_LifeMax = pLevel->m_nBaseLifeMax;
	Npc[m_nIndex].m_StaminaMax = pLevel->m_nBaseStaminaMax;
	Npc[m_nIndex].m_ManaMax = pLevel->m_nBaseManaMax;
	//	Npc[m_nIndex].ResetLifeReplenish();
	Npc[m_nIndex].m_CurrentLifeMax = Npc[m_nIndex].m_LifeMax;
	Npc[m_nIndex].m_CurrentStaminaMax = Npc[m_nIndex].m_StaminaMax;
	Npc[m_nIndex].m_CurrentManaMax = Npc[m_nIndex].m_ManaMax;
	
	// ´¦Àí¸÷ÖÖ¿¹ÐÔµÄ±ä»¯ »ð¡¢±ù¡¢¶¾¡¢µç¡¢ÎïÀí
	Npc[m_nIndex].m_FireResist				= PlayerSet.m_cLevelAdd.GetFireResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentFireResist		= Npc[m_nIndex].m_FireResist;
	Npc[m_nIndex].m_ColdResist				= PlayerSet.m_cLevelAdd.GetColdResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentColdResist		= Npc[m_nIndex].m_ColdResist;
	Npc[m_nIndex].m_PoisonResist			= PlayerSet.m_cLevelAdd.GetPoisonResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentPoisonResist		= Npc[m_nIndex].m_PoisonResist;
	Npc[m_nIndex].m_LightResist				= PlayerSet.m_cLevelAdd.GetLightResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentLightResist		= Npc[m_nIndex].m_LightResist;
	Npc[m_nIndex].m_PhysicsResist			= PlayerSet.m_cLevelAdd.GetPhysicsResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentPhysicsResist	= Npc[m_nIndex].m_PhysicsResist;
	Npc[m_nIndex].m_FireResistMax			= BASE_FIRE_RESIST_MAX;
	Npc[m_nIndex].m_ColdResistMax			= BASE_COLD_RESIST_MAX;
	Npc[m_nIndex].m_PoisonResistMax			= BASE_POISON_RESIST_MAX;
	Npc[m_nIndex].m_LightResistMax			= BASE_LIGHT_RESIST_MAX;
	Npc[m_nIndex].m_PhysicsResistMax		= BASE_PHYSICS_RESIST_MAX;
	Npc[m_nIndex].m_CurrentFireResistMax	= Npc[m_nIndex].m_FireResistMax;
	Npc[m_nIndex].m_CurrentColdResistMax	= Npc[m_nIndex].m_ColdResistMax;
	Npc[m_nIndex].m_CurrentPoisonResistMax	= Npc[m_nIndex].m_PoisonResistMax;
	Npc[m_nIndex].m_CurrentLightResistMax	= Npc[m_nIndex].m_LightResistMax;
	Npc[m_nIndex].m_CurrentPhysicsResistMax	= Npc[m_nIndex].m_PhysicsResistMax;
	
	int nOldCurCamp = Npc[m_nIndex].m_CurrentCamp;
	// ¸ù¾Ý×°±¸¡¢¼¼ÄÜÐÅÏ¢¸üÐÂµ±Ç°Êý¾Ý
	this->UpdataCurData();
	SetNpcPhysicsDamage();
	Npc[m_nIndex].m_CurrentCamp = nOldCurCamp;
	
	// ÉúÃü¡¢ÌåÁ¦¡¢ÄÚÁ¦Öµ¼ÓÂú
	Npc[m_nIndex].m_CurrentLife = Npc[m_nIndex].m_CurrentLifeMax;
	Npc[m_nIndex].m_CurrentStamina = Npc[m_nIndex].m_CurrentStaminaMax;
	Npc[m_nIndex].m_CurrentMana = Npc[m_nIndex].m_CurrentManaMax;
	
	// ¿Í»§¶Ë¶ÓÎéÖÐ×Ô¼ºµÄµÈ¼¶¼ÓÒ»
	if (this->m_cTeam.m_nFlag)
	{
		if (m_cTeam.m_nFigure == TEAM_CAPTAIN)
		{
			g_Team[0].m_nMemLevel[0] = Npc[m_nIndex].m_Level;
		}
		else
		{
			for (int i = 0; i < MAX_TEAM_MEMBER; i++)
			{
				if ((DWORD)g_Team[0].m_nMember[i] == Npc[m_nIndex].m_dwID)
				{
					g_Team[0].m_nMemLevel[i] = Npc[m_nIndex].m_Level;
					break;
				}
			}
		}
	}
	
	KSystemMessage Msg;
				
	Msg.byConfirmType = SMCT_CLICK;
	Msg.byParamSize = 0;
	Msg.byPriority = 1;
	Msg.eType = SMT_PLAYER;
	sprintf(Msg.szMessage, MSG_LEVEL_UP, Npc[m_nIndex].m_Level);
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, NULL);
	
	Npc[this->m_nIndex].SetInstantSpr(enumINSTANT_STATE_LEVELUP);
	CoreDataChanged(GDCNI_PLAYER_RT_ATTRIBUTE, 0, 0);
	CoreDataChanged(GDCNI_FIGHT_SKILL_POINT, 0, Player[CLIENT_PLAYER_INDEX].m_nSkillPoint);
}
#endif

#ifdef _SERVER
int	KPlayer::ResetProp()
{
		char szBaseInfo[80];
		sprintf(szBaseInfo,"\\settings\\player\\newplayerini%02d.ini",Npc[m_nIndex].m_Series*2 + Npc[m_nIndex].m_nSex);
		KIniFile BaseAttrib;
		if(!BaseAttrib.Load(szBaseInfo)) return 0;

		int nBaseStr, nBaseDex, nBaseVit, nBaseEn;
		BaseAttrib.GetInteger("ROLE", "ipower", 0, &nBaseStr);
		BaseAttrib.GetInteger("ROLE", "iagility", 0, &nBaseDex);
		BaseAttrib.GetInteger("ROLE", "iouter", 0, &nBaseVit);
		BaseAttrib.GetInteger("ROLE", "iinside", 0, &nBaseEn);
		
		int nStr = m_nStrength - nBaseStr;
		if(nStr < 0) nStr = 0;
		int nDex = m_nDexterity - nBaseDex;
		if(nDex < 0) nDex = 0;
		int nVit = m_nVitality - nBaseVit;
		if(nVit < 0) nVit = 0;
		int nEn = m_nEngergy - nBaseEn;
		if(nEn < 0) nEn = 0;
		
		m_nStrength = nBaseStr;
		m_nDexterity = nBaseDex;
		m_nVitality = nBaseVit;
		m_nEngergy = nBaseEn;
		m_nCurStrength = m_nStrength;
		m_nCurDexterity = m_nDexterity;
		m_nCurVitality = m_nVitality;
		m_nCurEngergy = m_nEngergy;
	
	//thiet lap hp,mp,tl max
	Npc[m_nIndex].m_LifeMax =(Npc[m_nIndex].m_Level*PlayerSet.m_cLevelAdd.GetLifePerLevel(Npc[m_nIndex].m_Series));
	Npc[m_nIndex].m_CurrentLifeMax = Npc[m_nIndex].m_LifeMax;
	
	
	Npc[m_nIndex].m_ManaMax =(Npc[m_nIndex].m_Level*PlayerSet.m_cLevelAdd.GetManaPerLevel(Npc[m_nIndex].m_Series));
	Npc[m_nIndex].m_CurrentManaMax = Npc[m_nIndex].m_ManaMax;
	
	Npc[m_nIndex].m_StaminaMax =(Npc[m_nIndex].m_Level*PlayerSet.m_cLevelAdd.GetStaminaPerLevel(Npc[m_nIndex].m_Series));
	Npc[m_nIndex].m_CurrentStaminaMax = Npc[m_nIndex].m_StaminaMax;
		
		SetNpcAttackRating();
		SetNpcDefence();

		UpdataCurData();
		SetNpcPhysicsDamage();
		
		PLAYER_ATTRIBUTE_SYNC	sSync;
		sSync.ProtocolType = s2c_playersyncattribute;
		sSync.m_btAttribute = ATTRIBUTE_RESET_ALL;
		sSync.m_nBasePoint = 0;
		sSync.m_nCurPoint = 0;
		sSync.m_nLeavePoint = m_nAttributePoint;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));

		return nStr + nDex + nVit + nEn;
}
#endif

#ifndef _SERVER
void	KPlayer::s2cGetCurAttribute(BYTE* pMsg)
{
	PLAYER_ATTRIBUTE_SYNC	*pAttribute = (PLAYER_ATTRIBUTE_SYNC*)pMsg;
	m_nAttributePoint = pAttribute->m_nLeavePoint;
	int nData;
	switch (pAttribute->m_btAttribute)
	{
	case ATTRIBUTE_STRENGTH:
		nData = pAttribute->m_nBasePoint - m_nStrength;
		m_nStrength = pAttribute->m_nBasePoint;
		m_nCurStrength = pAttribute->m_nCurPoint;
		UpdataCurData();
		SetNpcPhysicsDamage();
		break;
	case ATTRIBUTE_DEXTERITY:
		nData = pAttribute->m_nBasePoint - m_nDexterity;
		m_nDexterity = pAttribute->m_nBasePoint;
		SetNpcAttackRating();
		SetNpcDefence();
		UpdataCurData();
		SetNpcPhysicsDamage();
		break;
	case ATTRIBUTE_VITALITY:
		nData = pAttribute->m_nBasePoint - m_nVitality;
		m_nVitality = pAttribute->m_nBasePoint;
		Npc[m_nIndex].AddBaseLifeMax(PlayerSet.m_cLevelAdd.GetLifePerVitality(Npc[m_nIndex].m_Series) * nData);
		Npc[m_nIndex].AddBaseStaminaMax(PlayerSet.m_cLevelAdd.GetStaminaPerVitality(Npc[m_nIndex].m_Series) * nData);
		UpdataCurData();
		break;
	case ATTRIBUTE_ENGERGY:
		nData = pAttribute->m_nBasePoint - m_nEngergy;
		m_nEngergy = pAttribute->m_nBasePoint;
		Npc[m_nIndex].AddBaseManaMax(PlayerSet.m_cLevelAdd.GetManaPerEnergy(Npc[m_nIndex].m_Series) * nData);
		UpdataCurData();
		break;
	case ATTRIBUTE_RESET_ALL:
		{
		char szBaseInfo[80];
		sprintf(szBaseInfo,"\\settings\\player\\newplayerini%02d.ini",Npc[m_nIndex].m_Series*2);
		KIniFile BaseAttrib;
		if(!BaseAttrib.Load(szBaseInfo)) return;
		int nBaseStr, nBaseDex, nBaseVit, nBaseEn;
		BaseAttrib.GetInteger("ROLE", "ipower", 0, &nBaseStr);
		BaseAttrib.GetInteger("ROLE", "iagility", 0, &nBaseDex);
		BaseAttrib.GetInteger("ROLE", "iouter", 0, &nBaseVit);
		BaseAttrib.GetInteger("ROLE", "iinside", 0, &nBaseEn);
		
		int nStr = m_nStrength - nBaseStr;
		if(nStr < 0) nStr = 0;
		int nDex = m_nDexterity - nBaseDex;
		if(nDex < 0) nDex = 0;
		int nVit = m_nVitality - nBaseVit;
		if(nVit < 0) nVit = 0;
		int nEn = m_nEngergy - nBaseEn;
		if(nEn < 0) nEn = 0;
		
		m_nStrength = nBaseStr;
		m_nDexterity = nBaseDex;
		m_nVitality = nBaseVit;
		m_nEngergy = nBaseEn;
		m_nCurStrength = m_nStrength;
		m_nCurDexterity = m_nDexterity;
		m_nCurVitality = m_nVitality;
		m_nCurEngergy = m_nEngergy;
		
		SetNpcAttackRating();
		SetNpcDefence();
		Npc[m_nIndex].AddBaseLifeMax(PlayerSet.m_cLevelAdd.GetLifePerVitality(Npc[m_nIndex].m_Series) * (-nVit));
		Npc[m_nIndex].AddBaseStaminaMax(PlayerSet.m_cLevelAdd.GetStaminaPerVitality(Npc[m_nIndex].m_Series) * (-nVit));
		Npc[m_nIndex].AddBaseManaMax(PlayerSet.m_cLevelAdd.GetManaPerEnergy(Npc[m_nIndex].m_Series) * (-nEn));
		UpdataCurData();
		SetNpcPhysicsDamage();
		}break;
		// end code
	}
	CoreDataChanged(GDCNI_PLAYER_RT_ATTRIBUTE, 0, 0);
}
#endif

#ifndef _SERVER
void	KPlayer::s2cSetExp(double nExp)
{
	if (nExp > m_nExp)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_GET_EXP, nExp - m_nExp);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	else if (nExp < m_nExp)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_DEC_EXP, m_nExp - nExp);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	
	this->m_nExp = nExp;
}
#endif

#ifndef _SERVER
void	KPlayer::s2cSyncMoney(BYTE* pMsg)
{
	PLAYER_MONEY_SYNC	*pMoney = (PLAYER_MONEY_SYNC*)pMsg;
	
	if ( CheckTrading() )
	{
		if (pMoney->m_nMoney1 >= pMoney->m_nMoney3)
		{
			m_ItemList.SetMoney(pMoney->m_nMoney1 - pMoney->m_nMoney3, pMoney->m_nMoney2, pMoney->m_nMoney3);
		}
		else
		{
			m_ItemList.SetMoney(0, pMoney->m_nMoney2 + pMoney->m_nMoney1 - pMoney->m_nMoney3, pMoney->m_nMoney3);
		}
		//m_cTrade.m_nTradeState = 0;
		//m_cTrade.m_nTradeDestState = 0;
		if(pMoney->m_bGamble)
			CoreDataChanged(GDCNI_GAMBLE_RESULT, 0, 0);
		m_cTrade.m_nBackEquipMoney = pMoney->m_nMoney1;
		m_cTrade.m_nBackRepositoryMoney = pMoney->m_nMoney2;
	}
	else
	{
		// ¼ñÇ®Ò»¶¨ÊÇµ½room_equipment
		int nMoney1 = m_ItemList.GetMoney(room_equipment);
		if (pMoney->m_nMoney1 - nMoney1 > 0)
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_EARN_MONEY, pMoney->m_nMoney1 - nMoney1);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		m_ItemList.SetMoney(pMoney->m_nMoney1, pMoney->m_nMoney2, pMoney->m_nMoney3);
	}
}
#endif

#ifndef _SERVER
void	KPlayer::s2cTradeApplyStart(BYTE* pMsg)
{
	if (!pMsg)
		return;
	TRADE_APPLY_START_SYNC	*pApply = (TRADE_APPLY_START_SYNC*)pMsg;
	int		nNpcIdx;
	nNpcIdx = NpcSet.SearchID(pApply->m_dwNpcId);
	if (nNpcIdx == 0)
		return;
	
	KSystemMessage	sMsg;
	KUiPlayerItem	sPlayer;
	
	strcpy(sPlayer.Name, Npc[nNpcIdx].Name);
	sPlayer.nIndex = pApply->m_nDestIdx;
	sPlayer.uId = pApply->m_dwNpcId;
	sPlayer.nData = 0;
	
	sprintf(sMsg.szMessage, MSG_TRADE_GET_APPLY, Npc[nNpcIdx].Name);
	sMsg.eType = SMT_SYSTEM;
	sMsg.byConfirmType = SMCT_UI_TRADE;
	sMsg.byPriority = 3;
	sMsg.byParamSize = sizeof(KUiPlayerItem);
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&sPlayer);
}
#endif

#ifndef _SERVER
void	KPlayer::s2cGambleApplyStart(BYTE* pMsg)
{
	if (!pMsg)
		return;
	GAMBLE_APPLY_START_SYNC* pApply = (GAMBLE_APPLY_START_SYNC*)pMsg;
	int		nNpcIdx;
	nNpcIdx = NpcSet.SearchID(pApply->m_dwNpcId);
	if (nNpcIdx == 0)
		return;

	KSystemMessage	sMsg;
	KUiPlayerItem	sPlayer;

	strcpy(sPlayer.Name, Npc[nNpcIdx].Name);
	sPlayer.nIndex = pApply->m_nDestIdx;
	sPlayer.uId = pApply->m_dwNpcId;
	sPlayer.nData = 0;

	sprintf(sMsg.szMessage, MSG_GAMBLE_GET_APPLY, Npc[nNpcIdx].Name);
	sMsg.eType = SMT_SYSTEM;
	sMsg.byConfirmType = SMCT_UI_GAMBLE;
	sMsg.byPriority = 3;
	sMsg.byParamSize = sizeof(KUiPlayerItem);
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&sPlayer);
}
#endif

#ifndef _SERVER
void	KPlayer::s2cTradeMoneySync(BYTE* pMsg)
{
	TRADE_MONEY_SYNC	*pMoney = (TRADE_MONEY_SYNC*)pMsg;
	
	m_ItemList.SetRoomMoney(room_trade1, pMoney->m_nMoney);
	
	KUiObjAtRegion	sMoney;
	sMoney.Obj.uGenre = CGOG_MONEY;
	sMoney.Obj.uId = pMoney->m_nMoney;
	CoreDataChanged(GDCNI_TRADE_DESIRE_ITEM, (unsigned int)&sMoney, 0);
}
#endif

#ifndef _SERVER
void	KPlayer::s2cGambleMoneySync(BYTE* pMsg)
{
	GAMBLE_MONEY_SYNC* pMoney = (GAMBLE_MONEY_SYNC*)pMsg;

	m_ItemList.SetRoomMoney(room_trade1, pMoney->m_nMoney);

	KUiObjAtRegion	sMoney;
	sMoney.Obj.uGenre = CGOG_MONEY;
	sMoney.Obj.uId = pMoney->m_nMoney;
	CoreDataChanged(GDCNI_GAMBLE_DESIRE_ITEM, (unsigned int)&sMoney, 0);
}
#endif

#ifndef _SERVER

void	KPlayer::s2cTradeDecision(BYTE* pMsg)
{
	TRADE_DECISION_SYNC	*pSync = (TRADE_DECISION_SYNC*)pMsg;
	if (pSync->m_btDecision == 1)	
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_TRADE_SUCCESS, m_cTrade.m_szDestName);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		
		//m_ItemList.ClearRoom(room_trade);
		m_ItemList.RemoveAllInOneRoom(room_trade1);
		//m_ItemList.ClearRoom(room_trade1);
		m_ItemList.ClearRoom(room_tradeback);
		this->m_cTrade.Release();
		m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);
		
		CoreDataChanged(GDCNI_TRADE_END, 0, 0);
		
		m_ItemList.MenuSetMouseItem();
	}
	else if (pSync->m_btDecision == 0)	
	{
		if ( !CheckTrading() )
			return;
		
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_TRADE_FAIL, m_cTrade.m_szDestName);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		
		m_ItemList.RecoverTrade();
		m_ItemList.SetMoney(m_cTrade.m_nBackEquipMoney, m_cTrade.m_nBackRepositoryMoney, 0);
		m_ItemList.ClearRoom(room_trade);
		m_ItemList.RemoveAllInOneRoom(room_trade1);
		//		m_ItemList.ClearRoom(room_trade1);
		m_ItemList.ClearRoom(room_tradeback);
		m_cTrade.Release();
		m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);
		
		CoreDataChanged(GDCNI_TRADE_END, 0, 0);

		CoreDataChanged(GDCNI_CONTAINER_OBJECT_CHANGED, UOC_ITEM_TAKE_WITH, 0);
		
		m_ItemList.MenuSetMouseItem();
	}
}
#endif

#ifndef _SERVER

void	KPlayer::s2cGambleDecision(BYTE* pMsg)
{
	GAMBLE_DECISION_SYNC* pSync = (GAMBLE_DECISION_SYNC*)pMsg;
	if (pSync->m_btDecision == 1 || pSync->m_btDecision == 5 || pSync->m_btDecision == 6 || pSync->m_btDecision == 7)
	{
		KSystemMessage	sMsg;
		if (pSync->m_btDecision == 5) { //win
			m_ItemList.RecoverTrade();
			//m_ItemList.RemoveAllInOneRoom(room_trade);
			m_ItemList.ClearRoom(room_trade);
			sprintf(sMsg.szMessage, MSG_GAMBLE_WIN, m_cTrade.m_szDestName);
			m_ItemList.ClearRoom(room_trade1);
		}
		else if (pSync->m_btDecision == 6) { //lose
			m_ItemList.RemoveAllInOneRoom(room_trade1);
			sprintf(sMsg.szMessage, MSG_GAMBLE_LOSE, m_cTrade.m_szDestName);
			m_ItemList.ClearRoom(room_trade1);
		}
		else if (pSync->m_btDecision == 7) { //draw
			m_ItemList.RecoverTrade();
			sprintf(sMsg.szMessage, MSG_GAMBLE_DRAW, m_cTrade.m_szDestName);
			m_ItemList.ClearRoom(room_trade);
			m_ItemList.ClearRoom(room_trade1);
		}
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		CoreDataChanged(GDCNI_GAMBLE_RESULT, 0, pSync->m_btOtherChoice+1 );

		//m_ItemList.RemoveAllInOneRoom(room_trade);
		//m_ItemList.RemoveAllInOneRoom(room_tradeback);
		//m_ItemList.ClearRoom(room_trade);
		m_ItemList.ClearRoom(room_tradeback);
		this->m_cTrade.Reset();
		m_ItemList.BackupTrade(); //restart gamble
		//m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);

		CoreDataChanged(GDCNI_GAMBLE_RESET, 0, 0);
		//CoreDataChanged(GDCNI_CONTAINER_OBJECT_CHANGED, UOC_ITEM_TAKE_WITH,  0);

		m_ItemList.MenuSetMouseItem();
	}
	else if (pSync->m_btDecision == 0)
	{
		if (!CheckTrading())
			return;

		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_GAMBLE_FAIL, m_cTrade.m_szDestName);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

		m_ItemList.RecoverTrade();
		m_ItemList.SetMoney(m_cTrade.m_nBackEquipMoney, m_cTrade.m_nBackRepositoryMoney, 0);
		m_ItemList.ClearRoom(room_trade);
		m_ItemList.RemoveAllInOneRoom(room_trade1);
		//		m_ItemList.ClearRoom(room_trade1);
		m_ItemList.ClearRoom(room_tradeback);
		m_cTrade.Release();
		m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);

		CoreDataChanged(GDCNI_GAMBLE_END, 0, 0);

		CoreDataChanged(GDCNI_CONTAINER_OBJECT_CHANGED, UOC_ITEM_TAKE_WITH, 0);

		m_ItemList.MenuSetMouseItem();
		Player[CLIENT_PLAYER_INDEX].TradeApplyClose();
	}
}
#endif

#ifndef _SERVER
void	KPlayer::SyncCurPlayer(BYTE* pMsg) //Sync Player 1 lÇn chÝnh m×nh trong tõ db server xuèng ®©y
{
	m_ItemList.RemoveAll();
	
	CURPLAYER_SYNC* PlaySync = (CURPLAYER_SYNC *)pMsg;
	this->m_nIndex = NpcSet.SearchID(PlaySync->m_dwID);
	this->m_dwID = g_FileName2Id(Npc[m_nIndex].Name); 
	
	Npc[m_nIndex].m_Kind = kind_player;
	Npc[m_nIndex].m_Level = (DWORD)PlaySync->m_btLevel;
	Npc[m_nIndex].m_nSex = PlaySync->m_btSex;
	Npc[m_nIndex].m_Series  = PlaySync->m_btSeries;
	Npc[m_nIndex].SetPlayerIdx(CLIENT_PLAYER_INDEX);
	m_nAttributePoint = PlaySync->m_wAttributePoint;
	m_nSkillPoint = PlaySync->m_wSkillPoint;
	m_nStrength = PlaySync->m_wStrength;
	m_nDexterity = PlaySync->m_wDexterity;
	m_nVitality = PlaySync->m_wVitality;
	m_nEngergy = PlaySync->m_wEngergy;
	m_nLucky = PlaySync->m_wLucky;
	m_nActiveEquipNum = PlaySync->m_nActiveEquipSetNum;

	m_nCurStrength = m_nStrength;
	m_nCurDexterity = m_nDexterity;
	m_nCurVitality = m_nVitality;
	m_nCurEngergy = m_nEngergy;
	m_nCurLucky = m_nLucky;
	SetFirstDamage();
	SetBaseAttackRating();
	SetBaseDefence();
	Npc[m_nIndex].m_ActionScriptID = 0;
	Npc[m_nIndex].m_TrapScriptID = 0;
	m_nExp = PlaySync->m_nExp;
	m_ImagePlayer = PlaySync->m_btImagePlayer;
	m_nNextLevelExp = PlayerSet.m_cLevelAdd.GetLevelExp(PlaySync->m_btLevel);
	m_dwLeadExp = PlaySync->m_dwLeadExp;
	m_CUnlocked = PlaySync->m_btCUnlocked;
	m_dwLeadLevel = PlayerSet.m_cLeadExp.GetLevel(m_dwLeadExp);
	m_dwNextLevelLeadExp = PlayerSet.m_cLeadExp.GetLevelExp(m_dwLeadLevel);
	
	m_cFaction.m_nCurFaction = (char)PlaySync->m_btCurFaction;
	m_cFaction.m_nFirstAddFaction = (char)PlaySync->m_btFirstFaction;
	m_cFaction.m_nAddTimes = PlaySync->m_nFactionAddTimes;
	
	m_nWorldStat = (int)PlaySync->m_wWorldStat; //xÕp h¹ng thÕ giíi
	m_nSectStat = (int)PlaySync->m_wSectStat;

	m_ItemList.Init(CLIENT_PLAYER_INDEX);
	m_ItemList.SetMoney(PlaySync->m_nMoney1, PlaySync->m_nMoney2, 0);

	Npc[m_nIndex].m_nMissionGroup = PlaySync->m_nMissionGroup;//#PlayerMissionGroup
	Npc[m_nIndex].m_LifeMax = PlaySync->m_wLifeMax;
	Npc[m_nIndex].m_ManaMax = PlaySync->m_wManaMax;
	Npc[m_nIndex].m_StaminaMax = PlaySync->m_wStaminaMax;
	Npc[m_nIndex].m_LifeReplenish = PLAYER_LIFE_REPLENISH;
	Npc[m_nIndex].m_ManaReplenish = PLAYER_MANA_REPLENISH;
	Npc[m_nIndex].m_StaminaGain = PLAYER_STAMINA_GAIN;
	Npc[m_nIndex].m_StaminaLoss = PLAYER_STAMINA_LOSS;
	
	SetBaseResistData();
	SetBaseSpeedAndRadius();
	
	Npc[m_nIndex].RestoreNpcBaseInfo();
	
	m_BuyInfo.Clear();
	m_cMenuState.Release();
	m_cChat.Release();
	m_cTong.Clear();
	memset(m_szTaskAnswerFun, 0, sizeof(m_szTaskAnswerFun));
	m_nAvailableAnswerNum = 0;
	
	Npc[m_nIndex].m_Experience = 0;
	memset(Npc[m_nIndex].m_szChatBuffer, 0, sizeof(Npc[m_nIndex].m_szChatBuffer));
	Npc[m_nIndex].m_nCurChatTime = 0;
	Npc[m_nIndex].m_nNpcTimeout = 0;
	ZeroMemory(Npc[m_nIndex].m_nNpcParam, sizeof(Npc[m_nIndex].m_nNpcParam));
	Npc[m_nIndex].m_bNpcFollowFindPath = FALSE;
	Npc[m_nIndex].m_uFindPathTime = 0;
	Npc[m_nIndex].m_uLastFindPathTime = 0;

	m_RunStatus = 1;
	m_nLeftSkillID = 0;
	m_nLeftSkillLevel = 0;
	m_nRightSkillID = 0;
	m_nRightSkillLevel = 0;
	m_nPeapleIdx = 0;
	m_nObjectIdx = 0;
	m_MouseDown[0] = 0;
	m_MouseDown[1] = 0;
	
	Npc[m_nIndex].m_SyncSignal = 0;
	//Npc[m_nIndex].RestoreLiveData();
	//	SubWorld[0].AddPlayer(Npc[m_nIndex].m_RegionIndex, 0);//m_Region[Npc[m_nIndex].m_RegionIndex].AddPlayer(0);// m_WorldMessage.Send(GWM_PLAYER_ADD, Npc[m_nIndex].m_RegionIndex, 0);
}
#endif

BOOL	KPlayer::CheckTrading()
{
	return (m_cMenuState.m_nState == PLAYER_MENU_STATE_TRADING) || (m_cMenuState.m_nState == PLAYER_MENU_STATE_GAMBLING);
}

void	KPlayer::SetFirstDamage()
{
	int nDamageBase, nMagicBase;
	
	nDamageBase = m_nCurStrength / STRENGTH_SET_DAMAGE_VALUE + 1;
	nMagicBase = 0; //m_nCurDexterity (hoac) m_nCurEngergy / DEXTERITY_SET_DAMAGE_VALUE + 1; 
	//#muon su dung ban noi cong hoac than phap thi dua va day de 0 la khong su dung
	
	Npc[m_nIndex].m_PhysicsDamage.nValue[0] = nDamageBase;
	Npc[m_nIndex].m_PhysicsDamage.nValue[2] = nDamageBase;
	Npc[m_nIndex].m_PhysicsDamage.nValue[1] = 0;
	
	Npc[m_nIndex].m_CurrentFireDamage.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentFireDamage.nValue[1] = 0;
	Npc[m_nIndex].m_CurrentFireDamage.nValue[2] = 0;
	
	Npc[m_nIndex].m_CurrentColdDamage.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentColdDamage.nValue[1] = 0;
	Npc[m_nIndex].m_CurrentColdDamage.nValue[2] = 0;
	
	Npc[m_nIndex].m_CurrentLightDamage.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentLightDamage.nValue[1] = 0;
	Npc[m_nIndex].m_CurrentLightDamage.nValue[2] = 0;
	
	Npc[m_nIndex].m_CurrentPoisonDamage.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentPoisonDamage.nValue[1] = 0;
	Npc[m_nIndex].m_CurrentPoisonDamage.nValue[2] = 0;

	Npc[m_nIndex].m_PhysicsMagic.nValue[0] = nMagicBase;
	Npc[m_nIndex].m_PhysicsMagic.nValue[2] = nMagicBase;
	Npc[m_nIndex].m_PhysicsMagic.nValue[1] = 0;
	
	Npc[m_nIndex].m_CurrentFireMagic.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentFireMagic.nValue[1] = 0;
	Npc[m_nIndex].m_CurrentFireMagic.nValue[2] = 0;
	
	Npc[m_nIndex].m_CurrentColdMagic.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentColdMagic.nValue[1] = 0;
	Npc[m_nIndex].m_CurrentColdMagic.nValue[2] = 0;
	
	Npc[m_nIndex].m_CurrentLightMagic.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentLightMagic.nValue[1] = 0;
	Npc[m_nIndex].m_CurrentLightMagic.nValue[2] = 0;
	
	Npc[m_nIndex].m_CurrentPoisonMagic.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentPoisonMagic.nValue[1] = 0;
	Npc[m_nIndex].m_CurrentPoisonMagic.nValue[2] = 0;	
}

void	KPlayer::SetBaseAttackRating()
{
	Npc[m_nIndex].m_AttackRating = m_nDexterity * 4 - 28;
}

void	KPlayer::SetBaseDefence()
{
	Npc[m_nIndex].m_Defend = m_nDexterity >> 2;
}

void	KPlayer::SetBaseResistData()
{
	Npc[m_nIndex].m_FireResist			= PlayerSet.m_cLevelAdd.GetFireResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_ColdResist			= PlayerSet.m_cLevelAdd.GetColdResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_PoisonResist		= PlayerSet.m_cLevelAdd.GetPoisonResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_LightResist			= PlayerSet.m_cLevelAdd.GetLightResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_PhysicsResist		= PlayerSet.m_cLevelAdd.GetPhysicsResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_FireResistMax		= BASE_FIRE_RESIST_MAX;
	Npc[m_nIndex].m_ColdResistMax		= BASE_COLD_RESIST_MAX;
	Npc[m_nIndex].m_PoisonResistMax		= BASE_POISON_RESIST_MAX;
	Npc[m_nIndex].m_LightResistMax		= BASE_LIGHT_RESIST_MAX;
	Npc[m_nIndex].m_PhysicsResistMax	= BASE_PHYSICS_RESIST_MAX;
}

void	KPlayer::SetBaseSpeedAndRadius()
{
	Npc[m_nIndex].m_WalkSpeed = BASE_WALK_SPEED;
	Npc[m_nIndex].m_RunSpeed = BASE_RUN_SPEED;
	Npc[m_nIndex].m_AttackSpeed = BASE_ATTACK_SPEED;
	Npc[m_nIndex].m_CastSpeed = BASE_CAST_SPEED;
	Npc[m_nIndex].m_VisionRadius = BASE_VISION_RADIUS;
	Npc[m_nIndex].m_HitRecover = BASE_HIT_RECOVER;
}

#ifndef _SERVER
void KPlayer::DialogNpc(int nIndex)
{
	if (nIndex > 0 && Npc[nIndex].m_Index > 0)
	{
		if(nIndex)
			m_nLastNpcIndex = nIndex;

		if (Npc[nIndex].ActionScript[0])
		{
			ExecuteScript(Npc[nIndex].m_ActionScriptID,"main", nIndex); //#can kiem tra
		}
		else
		{
			PLAYER_DIALOG_NPC_COMMAND DialogNpcCmd;
			DialogNpcCmd.nNpcId = Npc[nIndex].m_dwID;
			DialogNpcCmd.ProtocolType = c2s_dialognpc;		
			if (g_pClient)
				g_pClient->SendPackToServer(&DialogNpcCmd, sizeof(PLAYER_DIALOG_NPC_COMMAND));
			
		}
	}
}
#endif

#ifndef _SERVER
void KPlayer::CheckObject(int nIdx)
{
	AUTOLOG("CHECKOBJ-ENTRY idx=%d id=%d kind=%d itemid=%d omx=%d omy=%d pmx=%d pmy=%d preg=%d oreg=%d t=%u", nIdx, Object[nIdx].m_nID, Object[nIdx].m_nKind, Object[nIdx].m_nItemDataID, Object[nIdx].m_nMapX, Object[nIdx].m_nMapY, Npc[m_nIndex].m_MapX, Npc[m_nIndex].m_MapY, Npc[m_nIndex].m_RegionIndex, Object[nIdx].m_nRegionIdx, GetTickCount());
	switch(Object[nIdx].m_nKind)
	{
	case Obj_Kind_Item:
	case Obj_Kind_Money:
		PickUpObj(nIdx);
		Npc[m_nIndex].m_nObjectIdx = 0;
		break;
	case Obj_Kind_Box:
	case Obj_Kind_Door:
	case Obj_Kind_Trap:
	case Obj_Kind_Prop:
		this->ObjMouseClick(nIdx);
		Npc[m_nIndex].m_nObjectIdx = 0;
		break;
	default:
		break;
	}
	m_nObjectIdx = 0;
	m_nPickObjectIdx = 0;
}

int KPlayer::FindTargetNpc(int nVision, BOOL bFightBack, int nFBVision, int nSelBoss,
	BOOL bTGNpc, const short* pSerOrder, BOOL bFoll, int nPointX, int nPointY)
{
	AUTOLOG_EVERY(1000, "[FT-IN] me=%d vision=%d fb=%d fbvis=%d selboss=%d tgnpc=%d foll=%d pt=(%d,%d) excl=%d", m_nIndex, nVision, bFightBack, nFBVision, nSelBoss, bTGNpc, bFoll, nPointX, nPointY, (int)m_mAutoExcludeNpcID.size());
	if(!bTGNpc && !bFightBack)
		return 0;
	if(nVision < 100)
		nVision = 100;
	else if(nVision > 1200)
		nVision = 1200;
	if(nFBVision < 100)
		nFBVision = 100;
	else if(nFBVision > 1200)
		nFBVision = 1200;
	int nX,nY;
	Npc[m_nIndex].GetMpsPos(&nX, &nY);
	if(nPointX > 0)
	{
		nX = nPointX;
		nY = nPointY;
	}
	int aryNpc[MAX_NPC][2];
	int aryPNpc[MAX_NPC][2];
	int	nIdx = 0, nNpcCount = 0, nPCount = 0, nNpcNearest = 0,
		nDistNear = 5000, nPDistNear = 5000, nNearestPlayer = 0;
	while(nIdx = NpcSet.GetNextIdx(nIdx))
	{
		if(nIdx == m_nIndex || !Npc[nIdx].m_dwID || Npc[nIdx].m_RegionIndex < 0
		|| Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive)
			continue;
		if(m_mAutoExcludeNpcID.find(Npc[nIdx].m_dwID) != m_mAutoExcludeNpcID.end())
		{
			AUTOLOG_EVERY(2000, "[FT-SKIP-EXCL] idx=%d id=%u kind=%d type=%d exclSize=%d", nIdx, Npc[nIdx].m_dwID, (int)Npc[nIdx].m_Kind, (int)Npc[nIdx].m_Type, (int)m_mAutoExcludeNpcID.size());
			continue;
		}
		if(!(NpcSet.GetRelation(m_nIndex, nIdx) == relation_enemy))
		{
			AUTOLOG_EVERY(3000, "[FT-SKIP-REL] idx=%d id=%u kind=%d rel=%d", nIdx, Npc[nIdx].m_dwID, (int)Npc[nIdx].m_Kind, (int)NpcSet.GetRelation(m_nIndex, nIdx));
			continue;
		}
		int x,y;
		Npc[nIdx].GetMpsPos(&x, &y);
		int nDist = g_GetDistance(nX, nY, x, y);
		if(Npc[nIdx].m_Kind == kind_player)
		{
			if(bFoll)
				continue;
			if(nDist >= nFBVision)
				continue;
			if(nDist < nPDistNear)
			{
				nPDistNear = nDist;
				nNearestPlayer = nIdx;
			}
			if(nPCount < MAX_NPC)
			{
				aryPNpc[nPCount][0] = nIdx;
				aryPNpc[nPCount++][1] = nDist;
			}
		}
		else
		{
			if(nDist >= nVision && nDist < nVision + 300)
				AUTOLOG_EVERY(2000, "[FT-SKIP-VIS] idx=%d id=%u dist=%d vision=%d org=(%d,%d) tg=(%d,%d)", nIdx, Npc[nIdx].m_dwID, nDist, nVision, nX, nY, x, y);
			if(nDist >= nVision)
				continue;
			if(nSelBoss == 1)	//ne boss
			{
				if(Npc[nIdx].m_Type != boss_none)
					continue;
			}
			else if(nSelBoss == 3) //chi danh boss
			{
				if(Npc[nIdx].m_Type == boss_none)
					continue;
			}
			if(nDist < nDistNear)
			{
				nDistNear = nDist;
				nNpcNearest = nIdx;
			}
			if(nNpcCount < MAX_NPC)
			{
				aryNpc[nNpcCount][0] = nIdx;
				aryNpc[nNpcCount++][1] = nDist;
			}
		}
	}
	if(bFightBack && nNearestPlayer)
	{
		if(pSerOrder)	//up
		{
			int aryPSerNpc[MAX_NPC][2];
			int sercount = 0;
			for(int s=0;s<series_num;++s)
			{
				sercount = 0;
				int ser = pSerOrder[s];
				for(int i=0;i<nPCount;++i)
				{
					nIdx = aryPNpc[i][0];
					if(Npc[nIdx].m_Series == ser)
					{
						aryPSerNpc[sercount][0] = nIdx;
						aryPSerNpc[sercount++][1] = aryPNpc[i][1];
					}
				}
				if(sercount)
					break;
			}
			int nPDistSer = 5000;
			for(int i = 0; i < sercount; ++i)
			{
				if(aryPSerNpc[i][1] < nPDistSer)
				{
					nPDistSer = aryPSerNpc[i][1];
					nNearestPlayer = aryPSerNpc[i][0];
				}
			}
			if(nPDistSer != 5000)
				return nNearestPlayer;
		}
		if(nPDistNear < nFBVision)
			return nNearestPlayer;
	}
	if(bTGNpc)
	{
		if(nSelBoss == 2) //uu tien boss
		{
			for(int i=0;i<nNpcCount;++i)
			{
				if(Npc[aryNpc[i][0]].m_Type  != boss_none)
				{
					return aryNpc[i][0];
				}
			}
		}
		if(pSerOrder)
		{
			int arySerNpc[MAX_NPC][2];
			int sercount = 0;
			for(int s=0;s<series_num;++s)
			{
				sercount = 0;
				int ser = pSerOrder[s];
				for(int i=0;i<nNpcCount;++i)
				{
					nIdx = aryNpc[i][0];
					if(Npc[nIdx].m_Series == ser)
					{
						arySerNpc[sercount][0] = nIdx;
						arySerNpc[sercount++][1] = aryNpc[i][1];
					}
				}
				if(sercount)
					break;
			}
			int nDistSer = 5000;
			for(int i = 0; i < sercount; ++i)
			{
				if(arySerNpc[i][1] < nDistSer)
				{
					nDistSer = arySerNpc[i][1];
					nNpcNearest = arySerNpc[i][0];
				}
			}
			if(nDistSer != 5000)
				return nNpcNearest;
		}
		AUTOLOG_EVERY(1000, "[FT-SUM] cand=%d pcand=%d near=%d nearIdx=%d vision=%d pnear=%d pnearIdx=%d fbvis=%d selboss=%d fb=%d", nNpcCount, nPCount, nDistNear, nNpcNearest, nVision, nPDistNear, nNearestPlayer, nFBVision, nSelBoss, bFightBack);
		if(nDistNear < nVision)
			return nNpcNearest;
	}
	return 0;
}
#endif

#ifndef _SERVER
BOOL KPlayer::AutoUseItem(int nGenre, int nDetailType, int nParticular)
{
	return m_ItemList.AutoUseItem(nGenre, nDetailType, nParticular, m_nPlayerIndex);
}

BOOL KPlayer::AutoCheckItem(int nGenre, int nDetailType, int nParticular)
{
	return m_ItemList.AutoCheckItem(nGenre, nDetailType, nParticular);
}
#endif

#ifndef _SERVER
void KPlayer::DrawSelectInfo()
{
}
void KPlayer::DrawSelectInfo_Backup()
{
	if (m_nIndex <= 0)
		return;
	
	if (m_nPeapleIdx)
	{
		//		Npc[m_nPeapleIdx].DrawBorder();
		if (Npc[m_nPeapleIdx].m_Kind == kind_player)
		{
			if (!NpcSet.CheckShowName())
			{
				Npc[m_nPeapleIdx].PaintInfo(Npc[m_nPeapleIdx].GetNpcPate(), true);
			}
		}
		else if (Npc[m_nPeapleIdx].m_Kind == kind_dialoger)
		{
			if (!NpcSet.CheckShowName())
			{
				Npc[m_nPeapleIdx].PaintInfo(Npc[m_nPeapleIdx].GetNpcPate(), true);
			}
		}
		else
		{
			Npc[m_nPeapleIdx].DrawBlood();
		}
		
		return;
	}
	if (m_nObjectIdx)
	{
		if (!ObjSet.CheckShowName())
			Object[m_nObjectIdx].DrawInfo();
		//		Object[m_nObjectIdx].DrawBorder();
		return;
	}
}
#endif

#ifndef _SERVER
void KPlayer::GetEchoDamage(int* nMin, int* nMax, int nType)
{
	int nSkillId;
	*nMin = 0;
	*nMax = 0;
	
	if (m_nIndex <= 0 || m_nIndex >= MAX_NPC || nType < 0 || nType > 1 || NULL == nMin || NULL == nMax)
	{
		g_DebugLog("[CLIENT] [KPlayer::GetEchoDamage] Error 1");
		return;
	}
	
	nSkillId = (0 == nType)?m_nLeftSkillID:m_nRightSkillID;
	int nLevel = Npc[m_nIndex].m_SkillList.GetCurrentLevel(nSkillId);
	
	if (nSkillId <= 0 ||nLevel <= 0 || nLevel >= MAX_SKILLLEVEL)
	{
		g_DebugLog("[CLIENT] [KPlayer::GetEchoDamage] Error 2");
		return;
	}
	
	KMagicAttrib*	pMagicData = NULL;
	
	KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nSkillId, nLevel);
	if (!pOrdinSkill) 
	{
		g_DebugLog("[CLIENT] [KPlayer::GetEchoDamage] Error 3");
		return;
	}

	switch(pOrdinSkill->GetSkillStyle())
	{
	case SKILL_SS_Missles:
	case SKILL_SS_Melee:
	case SKILL_SS_InitiativeNpcState:
	case SKILL_SS_PassivityNpcState:
		{
		}
		break;
	default:
		return;
	}

	pMagicData = pOrdinSkill->GetDamageAttribs();
	if (!pMagicData)
	{
		g_DebugLog("[CLIENT] [KPlayer::GetEchoDamage] Error 4");
		return;
	}
	
	int DamePecentToLevel = 0;


	if (Npc[m_nIndex].IsPlayer())
	{
		if (Npc[m_nIndex].m_Level > 100 && Npc[m_nIndex].m_Level <= 200)
		{
			DamePecentToLevel = (Npc[m_nIndex].m_Level - 100) / 5;
		}
	}

	int nMinNpcDamage = Npc[m_nIndex].m_PhysicsDamage.nValue[0] + Npc[m_nIndex].m_CurrentAddPhysicsDamage;
	int nMaxNpcDamage = Npc[m_nIndex].m_PhysicsDamage.nValue[2] + Npc[m_nIndex].m_CurrentAddPhysicsDamage;
	int nAddDamageP = Npc[m_nIndex].m_SkillList.GetAddSkillDamage(nSkillId) + Npc[m_nIndex].m_CurrentSkillEnhancePercent;
	if(Npc[m_nIndex].m_CurrentMana == Npc[m_nIndex].m_CurrentManaMax)//#khi noi cong day tang ky nang cong kich
	{
		nAddDamageP += Npc[m_nIndex].m_CurrentManaToSkillEnhanceP;
	}
	// Skip attackrating[0]
	pMagicData++;

	// Skip ignoredefense[1]
	pMagicData++;

	// Calc magic damage[2]
	if (magic_magicdamage_v == pMagicData->nAttribType)
	{
		*nMin += pMagicData->nValue[0] * (100 + nAddDamageP) / 100;
		*nMax += pMagicData->nValue[2] * (100 + nAddDamageP) / 100;
	}
	pMagicData++;

	// Skip seriesdamage[3]
	pMagicData++;

	// Skip deadlystrike[4]
	pMagicData++;

	// Skip fatallystrike[5]
	pMagicData++;

	// Skip steallife[6]
	pMagicData++;

	// Skip stealmana[7]
	pMagicData++;

	// Skip stealstamina[8]
	pMagicData++;

	// Calc physics damage[9]
	BOOL bIsPhysical = pOrdinSkill->IsPhysical();
	//
	if (magic_physicsenhance_p == pMagicData->nAttribType)
	{
		*nMin += nMinNpcDamage * (100 + pMagicData->nValue[0] * (100 + nAddDamageP) / 100) / 100;
		*nMax += nMaxNpcDamage * (100 + pMagicData->nValue[0] * (100 + nAddDamageP) / 100) / 100;
		
		int nEnhance;
		if (equip_meleeweapon == m_ItemList.GetWeaponType()) //trang bÞ vò khÝ cËn chiÕn
		{
			nEnhance = Npc[m_nIndex].m_CurrentMeleeEnhance[m_ItemList.GetWeaponParticular()];
		}
		else if (equip_rangeweapon == m_ItemList.GetWeaponType()) //trang bÞ vò khÝ tÇm xa
		{
			nEnhance = Npc[m_nIndex].m_CurrentRangeEnhance;
		}
		else 
		{
			nEnhance = Npc[m_nIndex].m_CurrentHandEnhance;
		}
		*nMin += nMinNpcDamage * nEnhance / 100;
		*nMax += nMaxNpcDamage * nEnhance / 100;
		//g_DebugLog("[CLIENT] [magic_physicsenhance_p] nMin: %d, nMax: %d", *nMin, *nMax);
	}
	else if (magic_physicsdamage_v == pMagicData->nAttribType)
	{
		*nMin += pMagicData->nValue[0] * (100 + nAddDamageP) / 100;
		*nMax += pMagicData->nValue[2] * (100 + nAddDamageP) / 100;


		if (!bIsPhysical)
	    {
			*nMin += Npc[m_nIndex].m_PhysicsMagic.nValue[0] + Npc[m_nIndex].m_CurrentAddPhysicsMagic;
			*nMax += Npc[m_nIndex].m_PhysicsMagic.nValue[2] + Npc[m_nIndex].m_CurrentAddPhysicsMagic;
	    }
		//g_DebugLog("[CLIENT] [magic_physicsdamage_v] nMin: %d, nMax: %d", *nMin, *nMax);
	}
	pMagicData++;

	// Calc cold damage[10]
	if (magic_colddamage_v == pMagicData->nAttribType)
	{
		*nMin += pMagicData->nValue[0] * (100 + nAddDamageP) / 100;
		*nMax += pMagicData->nValue[2] * (100 + nAddDamageP) / 100;

		if (!bIsPhysical)
	    {
			*nMin += Npc[m_nIndex].m_PhysicsMagic.nValue[0] + Npc[m_nIndex].m_CurrentColdMagic.nValue[0];
			*nMax += Npc[m_nIndex].m_PhysicsMagic.nValue[2] + Npc[m_nIndex].m_CurrentColdMagic.nValue[2];
	    }
	}
	if (bIsPhysical)
	{
		*nMin += Npc[m_nIndex].m_CurrentColdDamage.nValue[0];
		*nMax += Npc[m_nIndex].m_CurrentColdDamage.nValue[2];
	}
	pMagicData++;

	// Calc fire damage[11]
	if (magic_firedamage_v == pMagicData->nAttribType)
	{
		*nMin += pMagicData->nValue[0] * (100 + nAddDamageP) / 100 + pMagicData->nValue[0] * (100 + nAddDamageP) / 100 * Npc[m_nIndex].m_CurrentFireEnhance / 100;
		*nMax += pMagicData->nValue[2] * (100 + nAddDamageP) / 100 + pMagicData->nValue[2] * (100 + nAddDamageP) / 100 * Npc[m_nIndex].m_CurrentFireEnhance / 100;

		if (!bIsPhysical)
		{
			*nMin += Npc[m_nIndex].m_PhysicsMagic.nValue[0] + Npc[m_nIndex].m_CurrentFireMagic.nValue[0];
			*nMax += Npc[m_nIndex].m_PhysicsMagic.nValue[2] + Npc[m_nIndex].m_CurrentFireMagic.nValue[2];
		}
	}
	if (bIsPhysical)
	{
		*nMin += Npc[m_nIndex].m_CurrentFireDamage.nValue[0] + Npc[m_nIndex].m_CurrentFireDamage.nValue[0] * Npc[m_nIndex].m_CurrentFireEnhance / 100;
		*nMax += Npc[m_nIndex].m_CurrentFireDamage.nValue[2] + Npc[m_nIndex].m_CurrentFireDamage.nValue[2] * Npc[m_nIndex].m_CurrentFireEnhance / 100;
	}
	pMagicData++;

	// Calc lighting damage[12]
	if (magic_lightingdamage_v == pMagicData->nAttribType)
	{
		*nMin += pMagicData->nValue[0] * (100 + nAddDamageP) / 100 + (pMagicData->nValue[2] * (100 + nAddDamageP) / 100 - pMagicData->nValue[0] * (100 + nAddDamageP) / 100) * Npc[m_nIndex].m_CurrentLightEnhance / 100;
		*nMax += pMagicData->nValue[2] * (100 + nAddDamageP) / 100;


		if (!bIsPhysical)
		{
			*nMin += Npc[m_nIndex].m_PhysicsMagic.nValue[0] + Npc[m_nIndex].m_CurrentLightMagic.nValue[0];
			*nMax += Npc[m_nIndex].m_PhysicsMagic.nValue[2] + Npc[m_nIndex].m_CurrentLightMagic.nValue[2];
		}
	}
	if (bIsPhysical)
	{	
		*nMin += Npc[m_nIndex].m_CurrentLightDamage.nValue[0];
		*nMax += Npc[m_nIndex].m_CurrentLightDamage.nValue[2];
	}
	pMagicData++;

	// Calc poison damage[13]
	if (magic_poisondamage_v == pMagicData->nAttribType)
	{
		if (pMagicData->nValue[2] > 0)
		{
			int nPoisonDamage = 0;
			if (pMagicData->nValue[2] * (100 - Npc[m_nIndex].m_CurrentPoisonEnhance) / 100 > 0)
			{
				nPoisonDamage = (pMagicData->nValue[0] * pMagicData->nValue[1] / (pMagicData->nValue[2] * (100 - Npc[m_nIndex].m_CurrentPoisonEnhance) / 100)) * (100 + nAddDamageP) / 100;
			}
			else
			{
				nPoisonDamage = (pMagicData->nValue[0] * pMagicData->nValue[1]) * (100 + nAddDamageP) / 100;
			}
			*nMin += nPoisonDamage;
			*nMax += nPoisonDamage;
		}
		if (!bIsPhysical)
		{
			int nPoisonDamageAdd2 = 0;
			if (Npc[m_nIndex].m_CurrentPoisonMagic.nValue[2] > 0)
			{
				nPoisonDamageAdd2 = (Npc[m_nIndex].m_CurrentPoisonMagic.nValue[0]
					* Npc[m_nIndex].m_CurrentPoisonMagic.nValue[1] 
					/ Npc[m_nIndex].m_CurrentPoisonMagic.nValue[2]);
			}
			else
			{
				nPoisonDamageAdd2 = Npc[m_nIndex].m_CurrentPoisonMagic.nValue[0] * Npc[m_nIndex].m_CurrentPoisonMagic.nValue[1];
			}
			*nMin += nPoisonDamageAdd2;//Npc[m_nIndex].m_PhysicsMagic.nValue[0] + nPoisonDamage;
			*nMax += nPoisonDamageAdd2;//Npc[m_nIndex].m_PhysicsMagic.nValue[2] + nPoisonDamage;
		}
	}
	if (bIsPhysical)
	{
		int nPoisonDamageAdd = 0;
		if (Npc[m_nIndex].m_CurrentPoisonDamage.nValue[2] > 0)
		{
			nPoisonDamageAdd = (Npc[m_nIndex].m_CurrentPoisonDamage.nValue[0]
				* Npc[m_nIndex].m_CurrentPoisonDamage.nValue[1] 
				/ Npc[m_nIndex].m_CurrentPoisonDamage.nValue[2]);
		}
		else
		{
			nPoisonDamageAdd = Npc[m_nIndex].m_CurrentPoisonDamage.nValue[0] * Npc[m_nIndex].m_CurrentPoisonDamage.nValue[1];
		}
		*nMin += nPoisonDamageAdd;
		*nMax += nPoisonDamageAdd;
	}
	pMagicData++;
	// Skip stun[14]

	//pMagicData++;
	//ignorenegativestate_p[15]

	//pMagicData++;
	// Skip randmove[16]

	if (!bIsPhysical)
	{
		*nMin += ((*nMin * DamePecentToLevel) / 100);
		*nMax += (*nMax * DamePecentToLevel) / 100;
	}
}
#endif

void KPlayer::SetNpcDamageAttrib()
{
	SetNpcPhysicsDamage();
}

#ifdef _SERVER
void KPlayer::TobeExchangeServer(DWORD dwMapID, int nX, int nY)
{
	if (!m_dwID || m_nNetConnectIdx == -1)
		return;
	
	m_sExchangePos.m_dwMapID = dwMapID;
	m_sExchangePos.m_nX = nX;
	m_sExchangePos.m_nY = nY;
	m_bExchangeServer = TRUE;
}
#endif

/*
#ifdef _SERVER
void KPlayer::UpdateEnterGamePos(DWORD dwSubWorldID, int nX, int nY, int nFightMode)
{
	TRoleData* pData = (TRoleData *)&m_SaveBuffer;
	pData->BaseInfo.cFightMode = nFightMode;
	pData->BaseInfo.cUseRevive = 0;
	pData->BaseInfo.ientergameid = dwSubWorldID;
	pData->BaseInfo.ientergamex = nX;
	pData->BaseInfo.ientergamey = nY;
}
#endif
*/

#ifdef _SERVER
int		KPlayer::FindNearNpc(int nNpcSettingIdx, int nDistance/* = 0*/)
{
	if (nNpcSettingIdx == 0)
		return 0;
	
	int		nNpc, nRegionNo, i;
	nNpc = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].SearchNpcSettingIdx(nNpcSettingIdx);
	if (nNpc > 0)
	{
		if (nDistance <= 0 || (nDistance >= NpcSet.GetDistance(m_nIndex, nNpc)))
			return nNpc;
	}
	for (i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nNpc = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nRegionNo].SearchNpcSettingIdx(nNpcSettingIdx);
		if (nNpc > 0)
		{
			if (nDistance <= 0 || (nDistance >= NpcSet.GetDistance(m_nIndex, nNpc)))
				return nNpc;
		}
	}
	
	return 0;
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
int		KPlayer::FindNearNpc(const char* szName, int nDistance/* = 0*/)
{
	if (!szName || !szName[0])
		return 0;
	
	int		nNpc, nRegionNo, i;
	nNpc = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].SearchNpcName(szName);
	if (nNpc > 0)
	{
		if (nDistance <= 0 || (nDistance >= NpcSet.GetDistance(m_nIndex, nNpc)))
			return nNpc;
	}
	for (i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nNpc = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nRegionNo].SearchNpcName(szName);
		if (nNpc > 0)
		{
			if (nDistance <= 0 || (nDistance >= NpcSet.GetDistance(m_nIndex, nNpc)))
				return nNpc;
		}
	}
	
	return 0;
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
int		KPlayer::FindNearNpc(DWORD dwID, int nDistance/* = 0*/)
{
	if (dwID == 0)
		return 0;
	
	int		nNpc, nRegionNo, i;
	nNpc = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].SearchNpcID(dwID);
	if (nNpc > 0)
	{
		if (nDistance <= 0 || (nDistance >= NpcSet.GetDistance(m_nIndex, nNpc)))
			return nNpc;
	}
	for (i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nNpc = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nRegionNo].SearchNpcID(dwID);
		if (nNpc > 0)
		{
			if (nDistance <= 0 || (nDistance >= NpcSet.GetDistance(m_nIndex, nNpc)))
				return nNpc;
		}
	}
	
	return 0;
}

BOOL KPlayer::IsExchangingServer()
{
	return m_bExchangeServer;
}
#endif

#ifdef _SERVER
void KPlayer::SetLastNetOperationTime(int nTime)
{
	if (m_nNetConnectIdx == -1 || m_dwID == 0 || m_nIndex <= 0)
		return;
	
	m_nLastNetOperationTime = nTime;
	if (m_bSleepMode)
	{
		m_bSleepMode = FALSE;
		NPC_SLEEP_SYNC	SleepSync;
		SleepSync.ProtocolType = s2c_npcsleepmode;
		SleepSync.bSleep = m_bSleepMode;
		SleepSync.NpcID = Npc[m_nIndex].m_dwID;
		Npc[m_nIndex].SendDataToNearRegion(&SleepSync, sizeof(NPC_SLEEP_SYNC));
	}
}
#endif

#ifdef _SERVER
void 	KPlayer::SetNumImg(int ID)
{
	//g_DebugLog("Kplayer gia tri %d",ID);
	if(ID>0 && ID < 28)
		m_ImagePlayer=ID;
}
BOOL	KPlayer::CreateTong(int nCamp, char *lpszTongName)
{
	return this->m_cTong.Create(nCamp, lpszTongName);
}
#endif

#ifdef _SERVER
void KPlayer::RepairItem(DWORD dwItemID)
{
	int nIdx = m_ItemList.SearchID(dwItemID);
	int nCost = Item[nIdx].GetRepairPrice();
	if (!nCost)
		return;
	int nMaxDur = Item[nIdx].GetMaxDurability();
	int nDur = Item[nIdx].GetDurability();
	if(nDur == 0)//#do ben trang bi hong khong sua duoc bang tien van
		return;
	if (Pay(nCost) && m_nNetConnectIdx >= 0)
	{
		Item[nIdx].SetDurability(nMaxDur);
		ITEM_DURABILITY_CHANGE	IDC;
		IDC.ProtocolType = s2c_itemdurabilitychange;
		IDC.dwItemID = dwItemID;
		IDC.nChange = nMaxDur - nDur;
		if (g_pServer)
			g_pServer->PackDataToClient(m_nNetConnectIdx, &IDC, sizeof(ITEM_DURABILITY_CHANGE));
	}
}
#endif

#ifdef _SERVER
void KPlayer::ServerBreakItem(DWORD dwItemID, int nNum, bool isbreakall)
{
	int nIdx = m_ItemList.SearchID(dwItemID);
	if (nIdx <= 0 || nIdx >= MAX_ITEM)//add by phong kiÒu antihack
		return;
	//
	if(!Item[nIdx].IsStack())
		return;
	//
	if (!m_ItemList.FindSame(nIdx))
		return;
	//
	if (!m_ItemList.CheckItemInAll(nIdx))
	{
		printf("Hack BreakItem [%s] [%s]\n", m_AccoutName, m_PlayerName);
		return;
	}
	//
	if (Item[nIdx].GetStackNum() <= nNum)
		return;
	//
	if (isbreakall)
	{
		while (Item[nIdx].GetStackNum() > nNum)
		{
			int x, y;
			if (m_ItemList.CheckCanPlaceInEquipment(Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &x, &y) == FALSE)
				break;
			//
			Item[nIdx].SetStackNum(Item[nIdx].GetStackNum() - nNum);
			m_ItemList.SyncItem(nIdx);
			//
			int nIndex = ItemSet.AddI(&Item[nIdx]);
			if (nIndex <= 0)
				break;
			//
			Item[nIndex].SetStackNum(nNum);
			m_ItemList.AddKIL(nIndex, pos_equiproom, x, y);
		}
	}
	//
	else
	{
		Item[nIdx].SetStackNum(Item[nIdx].GetStackNum() - nNum);
		m_ItemList.SyncItem(nIdx);
		//
		int nIndex = ItemSet.AddI(&Item[nIdx]);

		Item[nIndex].SetStackNum(nNum);
		if (nIndex <= 0) return;
		//
		int	nIdx = m_ItemList.Hand();
		if (nIdx)
		{
			m_ItemList.Remove(nIdx);

			KMapPos sMapPos;
			KObjItemInfo	sInfo;
				
			GetAboutPos(&sMapPos);
				
			sInfo.m_nItemID = nIdx;
			sInfo.m_nItemWidth = Item[nIdx].GetWidth();
			sInfo.m_nItemHeight = Item[nIdx].GetHeight();
			sInfo.m_nMoneyNum = 0;
			char szNameTemp[OBJ_NAME_LENGHT];
			if (Item[nIdx].GetGenre() != item_equip && Item[nIdx].GetStackNum() > 1)
			{
				sprintf(szNameTemp, "%s x %d", Item[nIdx].GetName(), Item[nIdx].GetStackNum());
				strcpy(sInfo.m_szName, szNameTemp);
			}
			else
				strcpy(sInfo.m_szName, Item[nIdx].GetName());
			sInfo.m_nColorID = Item[nIdx].GetColorItem();
			sInfo.m_nGenre = Item[nIdx].GetGenre();
			sInfo.m_nDetailType = Item[nIdx].GetDetailType();
			sInfo.m_nParticularType = Item[nIdx].GetParticular();
			sInfo.m_nMovieFlag = 1;
			sInfo.m_nSoundFlag = 1;
			sInfo.m_dwNpcId1 = 0;
				
			int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), sMapPos, sInfo);
			if (nObj >= 0)
			{
				if (Item[nIdx].GetGenre() == item_task)
				{
					Object[nObj].SetEntireBelong(m_nPlayerIndex);
				}
				else
				{
					Object[nObj].SetItemBelong(m_nPlayerIndex);
				}
			}
		}
		m_ItemList.AddKIL(nIndex, pos_hand, 0, 0);
	}
}
#endif

int KPlayer::GetExtPoint()
{
	return m_nExtPoint;
}

void KPlayer::CheckRideHorse(BOOL nFlagRide)
{
	int ItemActiveIdx = m_ItemList.GetEquipment(itempart_horse);
	if (ItemActiveIdx <= 0)
		return;

	int nActive = m_ItemList.GetEquipEnhance(itempart_horse);
	int nGoldAct = m_ItemList.GetGoldEquipEnhance(itempart_horse);

	Item[ItemActiveIdx].m_bHorseScaleOnly = true;
	if (nFlagRide)
	{
		//
		Item[ItemActiveIdx].m_fHorseScale = 1.0f;
		Item[ItemActiveIdx].RemoveMagicAttribFromNPC(&Npc[m_nIndex], nActive, nGoldAct);

		
		Item[ItemActiveIdx].m_fHorseScale = 0.5f;
		Item[ItemActiveIdx].ApplyMagicAttribToNPC(&Npc[m_nIndex], nActive, nGoldAct);

		Npc[m_nIndex].m_bRideHorse = FALSE;
	}
	else 
	{
		
		Item[ItemActiveIdx].m_fHorseScale = 0.5f;
		Item[ItemActiveIdx].RemoveMagicAttribFromNPC(&Npc[m_nIndex], nActive, nGoldAct);

		
		Item[ItemActiveIdx].m_fHorseScale = 1.0f;
		Item[ItemActiveIdx].ApplyMagicAttribToNPC(&Npc[m_nIndex], nActive, nGoldAct);

		Npc[m_nIndex].m_bRideHorse = TRUE;
	}
	Item[ItemActiveIdx].m_bHorseScaleOnly = false;
#ifdef _SERVER
	NPC_RIDE_SYNC sValue;
	sValue.ProtocolType = s2c_ridesync;
	sValue.m_btRideFlag = nFlagRide;
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx,
		(BYTE*)&sValue, sizeof(NPC_RIDE_SYNC));
#endif
}




void	KPlayer::SetImageNpcId(int nNumber)
{	
	m_ImageId = nNumber;
#ifdef _SERVER
	IMAGENPC_VALUE_SYNC	sValue;
	sValue.ProtocolType = s2c_imagenpc;
	sValue.m_nType = 0;
	sValue.m_nValue = nNumber;
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(IMAGENPC_VALUE_SYNC));
#endif
}

#ifdef _SERVER
void KPlayer::AddExtPoint(int nPoint, int nChangePoint)
{
	m_nExtPoint += nPoint;
	m_nChangeExtPoint = nChangePoint;

	EXTPOINT_VALUE_SYNC	sValue;
	sValue.ProtocolType = s2c_extpointsync;
	sValue.m_nExtPointValue = m_nExtPoint;
	sValue.m_nChangeExtPoint = m_nChangeExtPoint;
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(EXTPOINT_VALUE_SYNC));
}

void KPlayer::SetExtPoint(int nPoint, int nChangePoint)
{
	m_nExtPoint = nPoint;
	m_nChangeExtPoint = nChangePoint;
	EXTPOINT_VALUE_SYNC	sValue;
	sValue.ProtocolType = s2c_extpointsync;
	sValue.m_nExtPointValue = m_nExtPoint;
	sValue.m_nChangeExtPoint = m_nChangeExtPoint;
//	g_DebugLog("s2c_extpointsync %d", s2c_extpointsync);
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(EXTPOINT_VALUE_SYNC));
}

BOOL KPlayer::PayExtPoint(int nPoint)
{
	if (m_nExtPoint < nPoint)
		return FALSE;

	m_nExtPoint -= nPoint;
	m_nChangeExtPoint += nPoint;
	// new add by lzlsky301
	EXTPOINT_VALUE_SYNC	sValue;
	sValue.ProtocolType = s2c_extpointsync;
	sValue.m_nExtPointValue = m_nExtPoint;
	sValue.m_nChangeExtPoint = m_nChangeExtPoint;
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(EXTPOINT_VALUE_SYNC));
	return TRUE;
}
int KPlayer::GetExtPointChanged()
{
	return m_nChangeExtPoint;
}
#endif

void KPlayer::ProcessDouble() //#time x2 Exp chÕt kh«ng mÊt //#can kiem tra
{
	if (m_cTask.GetSaveVal(TASKVALUE_X2_EXP) > 0)
	{
#ifdef _SERVER
		m_cTask.SetSaveVal(TASKVALUE_X2_EXP,m_cTask.GetSaveVal(TASKVALUE_X2_EXP) - 1);
#endif
	}
}

#ifdef _SERVER
void KPlayer::SetChestLock(BOOL unlockFlag)
{
	if (unlockFlag)
	{
		// Ruong duoc mo
		m_CUnlocked = 1;
		char szMsg[100];
		sprintf(szMsg, "R­¬ng ®· më.");
		KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, "HÖ thèng", (char *) szMsg, strlen(szMsg) );
	}
	else
	{
		// Ruong duoc khoa
		m_CUnlocked = 0;
		char szMsg[100];
		sprintf(szMsg, "R­¬ng ®· ®ãng.");
		KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, "HÖ thèng", (char *) szMsg, strlen(szMsg) );
	}
}

void KPlayer::SetChestPW(int newPW)
{
	m_nChestPW = newPW;
	if(m_nChestPW == 123456)//mat khau mac dinh 123456 khong can khoa ruong
	{
		m_CUnlocked = 1;
	}
	// Thong bao cho Client
	//char szMsg[100];
	//sprintf(szMsg, "MËt khÈu ®· ®­îc thay ®æi thµnh c«ng !");
	//KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, "Th«ng b¸o", (char *) szMsg, strlen(szMsg) );
	
}

BOOL KPlayer::CheckChestPW(int iPassword)
{	
	if(iPassword < 100000 || iPassword > 999999)//anti by phong kiÒu 27/08/2020
		return FALSE;
	if (iPassword == m_nChestPW)
	{
		// Mat khau ruong chinh xac
		//char szMsg[100];
		//sprintf(szMsg, "MËt khÈu ®· ®­îc nhËp chÝnh x¸c !");
		//KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, "HÖ thèng", (char *) szMsg, strlen(szMsg) );
		return TRUE;
	}
	else
	{
		// Mat khau ruong SAI
		char szMsg[100];
		sprintf(szMsg, "M· r­¬ng kh«ng ®óng !");
		KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, "HÖ thèng", (char *) szMsg, strlen(szMsg) );
		return FALSE;
	}
}

void	KPlayer::SetChestPW2(int newPW)
{
	m_nChestPW = newPW;
	
	// Thong bao cho Client
	char szMsg[100];
	sprintf(szMsg, "MËt khÈu ®· ®­îc thay ®æi: %d", m_nChestPW);
	KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, "Th«ng b¸o", (char *) szMsg, strlen(szMsg) );
	
}

void	KPlayer::SetNewPW(int resetPW)
{
	m_nChestPW = resetPW;
	
	// Thong bao cho Client
	char szMsg[100];
	sprintf(szMsg, "MËt khÈu mÆc ®Þnh: %d", m_nChestPW);
	KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, "Th«ng b¸o", (char *) szMsg, strlen(szMsg) );
}
#endif

void	KPlayer::SwitchEquipSet(int setnum)
{
	if (m_nActiveEquipNum == setnum) {
#ifdef _SERVER
		BYTE	byFinished = s2c_itemexchangefinish;
		if (g_pServer)
			g_pServer->PackDataToClient(m_nNetConnectIdx, &byFinished, sizeof(BYTE));
#endif
		return;
	}
	m_nActiveEquipNum = setnum;

#ifdef _SERVER
	// Don't allow switching during certain states
	if (CheckTrading() || m_bExchangeServer) {
		BYTE	byFinished = s2c_itemexchangefinish;
		if (g_pServer)
			g_pServer->PackDataToClient(m_nNetConnectIdx, &byFinished, sizeof(BYTE));
		return;
	}
#else
	if (CheckTrading())
		return;
#endif

	// Switch equipment positions for all items
	for (int i = 0; i < MAX_PLAYER_ITEM; i++)
	{
		if (m_ItemList.m_Items[i].nIdx)
		{
			// Update items in equipment room
			if (m_ItemList.m_Items[i].nPlace == pos_equip)
			{
				m_ItemList.m_Items[i].nPlace = pos_equipback;
			}
			// Update items in equipment backup room
			else if (m_ItemList.m_Items[i].nPlace == pos_equipback)
			{
				m_ItemList.m_Items[i].nPlace = pos_equip;
			}
		}
	}

	KItemList tempList;

	ZeroMemory(tempList.m_EquipItem, sizeof(tempList.m_EquipItem));
	// Copy current equipment to temp
	for (int i = 0; i < itempart_num; i++)
	{
		int nIdx = m_ItemList.m_EquipItem[i];
		if (nIdx >= 0 && nIdx < MAX_ITEM)
			tempList.m_EquipItem[i] = nIdx;
		//m_ItemList.m_EquipItem[i] = 0;
	}

	// Equip new switched items
	for (int i = 0; i < itempart_num; i++)
	{
		m_ItemList.UnEquip(m_ItemList.m_EquipItem[i], i);
	}

	// Copy alternate set to current
	for (int i = 0; i < itempart_num; i++)
	{
		int nIdx = m_ItemList.m_AltEquipmentItem[i];
		if (nIdx >= 0 && nIdx < MAX_ITEM)
			m_ItemList.m_EquipItem[i] = nIdx;
		m_ItemList.m_AltEquipmentItem[i] = 0;
	}

	// Move temp (original equipment) to alternate
	for (int i = 0; i < itempart_num; i++)
	{
		int nIdx = tempList.m_EquipItem[i];
		if (nIdx >= 0 && nIdx < MAX_ITEM)
			m_ItemList.m_AltEquipmentItem[i] = nIdx;
	}
	for (int i = 0; i < itempart_num; i++)
	{
		m_ItemList.Equip(m_ItemList.m_EquipItem[i], i);
	}
#ifndef _SERVER
	KUiObjAtContRegion pInfo;
	for (int i = 0; i < itempart_num; i++)
	{
		pInfo.Region.h = 0;
		pInfo.Region.v = i;
		pInfo.eContainer = UOC_EQUIPTMENT;

		CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo, 0);
		CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo, 1);
	}
	CoreDataChanged(GDCNI_PLAYER_RT_ATTRIBUTE, 0, 0);

#endif
	// Recalculate stats with new equipment
	UpdataCurData();
	SetNpcPhysicsDamage();
#ifdef _SERVER
	PLAYER_ATTRIBUTE_SYNC	sSync;
	sSync.ProtocolType = s2c_playersyncattribute;
	sSync.m_btAttribute = 1;
	sSync.m_nBasePoint = m_nDexterity;
	sSync.m_nCurPoint = m_nCurDexterity;
	sSync.m_nLeavePoint = m_nAttributePoint;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));

	// Send server switch equip done
	PLAYER_SWITCH_EQUIP_SYNC sSwitch;
	sSwitch.ProtocolType = s2c_playerswitchequip;
	sSwitch.m_nSetNum = m_nActiveEquipNum;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSwitch, sizeof(PLAYER_SWITCH_EQUIP_SYNC));
	if (Npc[m_nIndex].m_Doing == do_sit)
	{
		Npc[m_nIndex].SendCommand(do_stand);
	}

	BYTE	byFinished = s2c_itemexchangefinish;
	if (g_pServer)
		g_pServer->PackDataToClient(m_nNetConnectIdx, &byFinished, sizeof(BYTE));
#endif
}

#ifdef _SERVER
int KPlayer::GetItemIdxBymItemListIdx(int midx)
{
	if(midx > MAX_PLAYER_ITEM)
		return 0;
	int nIdx = m_ItemList.m_Items[midx].nIdx;
	if(nIdx<0)
		return 0;
	return nIdx;
}

void KPlayer::SendSellItemInfo( int nTargetPlayer, int nPrcess, BOOL bUpdate)
{
	if (nTargetPlayer <= 0 || nTargetPlayer >= MAX_PLAYER || Player[nTargetPlayer].m_nIndex <= 0)
		return;
	if (!m_CUnlocked) // khoa ko xem duoc bay ban
		return;

	int				nIdx, i, j, k = 0;
	SViewSellItemInfo	*pInfo;
	VIEW_ITEM_SYNC	sView;
	ZeroMemory(sView.m_sInfo,sizeof(sView.m_sInfo));
	
	if (!bUpdate)
		sView.ProtocolType = s2c_viewsellitem;
	else
		sView.ProtocolType =s2c_viewupdateitem;
	sView.m_dwNpcID = Npc[m_nIndex].m_dwID;

	for (i = 0; i < MAX_PLAYER_ITEM; i++)
	{
		nIdx = m_ItemList.m_Items[i].nIdx;
		if (nIdx <= 0 || m_ItemList.m_Items[i].nPrice == 0 || m_ItemList.m_Items[i].nPlace != pos_equiproom)
		{
			continue;
		}
		if (nPrcess)
		{
			nPrcess--;
			continue;
		}

		pInfo = &sView.m_sInfo[k];
		pInfo->m_uPrice = m_ItemList.m_Items[i].nPrice;
		pInfo->m_bX = m_ItemList.m_Items[i].nX;
		pInfo->m_bY = m_ItemList.m_Items[i].nY;
		pInfo->m_nIdx = m_ItemList.m_Items[i].nIdx;

		pInfo->m_nID			= Item[nIdx].GetID();
		pInfo->m_btGenre		= Item[nIdx].GetGenre();
		pInfo->m_btDetail = Item[nIdx].GetDetailType();
		if(Item[nIdx].GetNature() >= NATURE_GOLD)
			pInfo->m_btDetail = Item[nIdx].GetRow();
		pInfo->m_btParticur		= Item[nIdx].GetParticular();
		pInfo->m_btSeries		= Item[nIdx].GetSeries();
		pInfo->m_btLevel		= Item[nIdx].GetLevel();
		pInfo->m_bStack			= Item[nIdx].GetStackNum();
		pInfo->m_bEnChance		= Item[nIdx].GetEnChance();
		pInfo->m_nGoldId		= Item[nIdx].GetGoldId();
		pInfo->m_btLuck			= Item[nIdx].m_GeneratorParam.nLuck;
		pInfo->m_dwRandomSeed	= Item[nIdx].m_GeneratorParam.uRandomSeed;
		pInfo->m_wVersion		= Item[nIdx].m_GeneratorParam.nVersion;
		pInfo->m_YearExp			= Item[nIdx].GetTime()->bYear;
		pInfo->m_curDurability		= Item[nIdx].GetDurability();
		pInfo->m_nParam = Item[nIdx].GetParam();
		pInfo->m_nNature = Item[nIdx].GetNature();

		for (int l = 0;l < MAX_ITEM_MAGICLEVEL;l++)
		{
			pInfo->m_btMagicLevel[l] = Item[nIdx].m_GeneratorParam.nGeneratorLevel[l];
		}

		pInfo->m_bPoint = Item[nIdx].IsPurple();
		k++;

		if (k == 60)
		{
			break;
		}
	}

	g_pServer->PackDataToClient(Player[nTargetPlayer].m_nNetConnectIdx, (BYTE*)&sView, sizeof(sView));
}

void KPlayer::SendSellItemCount( int nIndex )
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER || Player[nIndex].m_nIndex <= 0)
		return;

	PLAYER_GET_COUNT pGetCount;
	pGetCount.ProtocolType = s2c_playergetcount;
	pGetCount.dwId = Npc[m_nIndex].m_dwID;

	int nCount = 0;
	for (int i = 0;i < MAX_PLAYER_ITEM;i++)
	{
		int nIdx = m_ItemList.m_Items[i].nIdx;
		if (nIdx <= 0 || m_ItemList.m_Items[i].nPrice == 0 || m_ItemList.m_Items[i].nPlace != pos_equiproom)
		{
			//			memset(pInfo, 0, sizeof(SViewItemInfo));
			continue;
		}
		nCount++;
	}

	pGetCount.nCount = nCount;

	if (nCount == 0)
	{
		Npc[m_nIndex].m_BaiTan = 0;
	}

	g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&pGetCount, sizeof(pGetCount));
}

int KPlayer::GetTradeCount()
{
		
	int nCount = 0;
	for (int i = 0;i < MAX_PLAYER_ITEM;i++)
	{
		int nIdx = m_ItemList.m_Items[i].nIdx;
		if (nIdx <= 0 || m_ItemList.m_Items[i].nPrice == 0 || m_ItemList.m_Items[i].nPlace != pos_equiproom)
		{
			//			memset(pInfo, 0, sizeof(SViewItemInfo));
			continue;
		}
		nCount++;
	}

	return nCount;
}

void KPlayer::SendMSGroup()
{
	PLAYER_MISSION_DATA	sData;
	sData.ProtocolType = s2c_playermissiondata;
	sData.m_nMissionGroup = Npc[m_nIndex].m_nMissionGroup;
	if (g_pServer && m_nNetConnectIdx != -1)
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sData, sizeof(PLAYER_MISSION_DATA));
}

void KPlayer::SendMSRank(TMissionLadderSelfInfo* SelfData, TMissionLadderInfo* RankData)
{
	PLAYER_MISSION_RANKDATA sData;//sync info player
	sData.ProtocolType = s2c_syncrankdata;
	sData.SelfData = *SelfData;
	if (g_pServer && m_nNetConnectIdx != -1 && sData.SelfData.nParam[0] == 1) //sData.SelfData[0] == 1 ®ang online == 0 ®· rêi khái mission
	{
		g_pServer->PackDataToClient(m_nNetConnectIdx, &sData, sizeof(PLAYER_MISSION_RANKDATA));
	}

	PLAYER_MISSION_RANKDATA2 sData2;//sync top 10
	sData2.ProtocolType = s2c_syncrankdata2;
	for(int i =0;i<MISSION_STATNUM;i++)
	{
		sData2.nRankIndex = i;
		memcpy(&sData2.MissionRank, &RankData[i], sizeof(RankData[i]));
		g_pServer->PackDataToClient(m_nNetConnectIdx, &sData2, sizeof(PLAYER_MISSION_RANKDATA2));
	}
}

void KPlayer::Change_PK_Status(int nId) 
{ 
	if (nId == 1)
	{
		char scriptName[255]; 
		sprintf(scriptName,"\\script\\player\\trangthaipk\\luyencong.lua"); 
		ExecuteScript(scriptName,"main",0); 	
	}
	else if (nId == 2)
	{
		char scriptName[255]; 
		sprintf(scriptName,"\\script\\player\\trangthaipk\\chiendau.lua");
		ExecuteScript(scriptName,"main",0); 		
	}
	
	else if (nId == 3)
	{
		char scriptName[255]; 
		sprintf(scriptName,"\\script\\player\\trangthaipk\\dosat.lua");
		ExecuteScript(scriptName,"main",0); 		
	}	
	
}

struct CollectedItem
{
	int nIdx;
	int w,h;
	int x,y;
};

struct TypePack
{
	int w, h;
	int count;
};

struct Placement
{
	int typeIndex;
	int x, y;
};

static void DFS_Pack(
	int idxCell,
	int usedArea,
	int totalArea,
	int nW,
	int nH,
	int nCells,
	int* pGrid,                       
	std::vector<TypePack>& types,     
	std::vector<Placement>& curPlace, 
	std::vector<Placement>& bestPlace,
	bool& bFoundFull                  
)
{
	if (bFoundFull)
		return;

	if (usedArea == totalArea)
	{
		bestPlace = curPlace;
		bFoundFull = true;
		return;
	}

	if (idxCell == nCells)
		return;

	if (usedArea + (nCells - idxCell) < totalArea)
		return;

	if (pGrid[idxCell] != -1)
	{
		DFS_Pack(idxCell + 1, usedArea, totalArea,
			nW, nH, nCells, pGrid, types, curPlace, bestPlace, bFoundFull);
		return;
	}

	int x = idxCell % nW;
	int y = idxCell / nW;

	int typeCount = (int)types.size();
	int i;

	for (i = 0; i < typeCount; ++i)
	{
		TypePack& tp = types[i];
		if (tp.count <= 0)
			continue;

		if (x + tp.w > nW || y + tp.h > nH)
			continue;

		bool canPlace = true;
		int dx, dy;
		for (dy = 0; dy < tp.h && canPlace; ++dy)
		{
			for (dx = 0; dx < tp.w; ++dx)
			{
				int idx2 = (y + dy) * nW + (x + dx);
				if (pGrid[idx2] != -1)
				{
					canPlace = false;
					break;
				}
			}
		}
		if (!canPlace)
			continue;

		for (dy = 0; dy < tp.h; ++dy)
		{
			for (dx = 0; dx < tp.w; ++dx)
			{
				int idx2 = (y + dy) * nW + (x + dx);
				pGrid[idx2] = i;
			}
		}

		tp.count--;

		Placement pl;
		pl.typeIndex = i;
		pl.x = x;
		pl.y = y;
		curPlace.push_back(pl);

		DFS_Pack(idxCell + 1, usedArea + tp.w * tp.h, totalArea,
			nW, nH, nCells, pGrid, types, curPlace, bestPlace, bFoundFull);

		// backtrack
		curPlace.pop_back();
		tp.count++;
		for (dy = 0; dy < tp.h; ++dy)
		{
			for (dx = 0; dx < tp.w; ++dx)
			{
				int idx2 = (y + dy) * nW + (x + dx);
				pGrid[idx2] = -1;
			}
		}

		if (bFoundFull)
			return;
	}

	DFS_Pack(idxCell + 1, usedArea, totalArea,
		nW, nH, nCells, pGrid, types, curPlace, bestPlace, bFoundFull);
}

int KPlayer::AutoArrangeItem(int nType)
{
	int* pnCmpArray = NULL;
	int nRoom, nPos;
	if(!nType)
	{
		if(m_nArrangeTimeItem > g_SubWorldSet.m_nLoopRate)
			return 0;
		m_nArrangeTimeItem = g_SubWorldSet.m_nLoopRate + 16*18;
		pnCmpArray = &m_nCmpArrayItem[0];
		nRoom = room_equipment;
		nPos = pos_equiproom;
	}
	else if(nType == 1)
	{
		if(m_nArrangeTimeBox > g_SubWorldSet.m_nLoopRate)
			return 0;
		m_nArrangeTimeBox = g_SubWorldSet.m_nLoopRate + 56*18;
		pnCmpArray = &m_nCmpArrayBox[0];
		nRoom = room_repository;
		nPos = pos_repositoryroom;
	}
	else if(nType == 2)
	{
		pnCmpArray = &m_nCmpArrayExBox[0];
		nRoom = room_exbox1;
		nPos = pos_exbox1room;
	}
	else if(nType == 3)
	{
		pnCmpArray = &m_nCmpArrayExBox[0];
		nRoom = room_exbox2;
		nPos = pos_exbox2room;
	}
	else //if(nType == 4)
	{
		pnCmpArray = &m_nCmpArrayExBox[0];
		nRoom = room_exbox3;
		nPos = pos_exbox3room;
	}
	if(nType < 3)
	{
		int* pArr = m_ItemList.m_Room[nRoom].GetArray();
		if(!memcmp(pnCmpArray, pArr, sizeof(int)*EQUIPMENT_ROOM_WIDTH*EQUIPMENT_ROOM_HEIGHT))
			return 1;
	}
	int nW = EQUIPMENT_ROOM_WIDTH;
	int nH = EQUIPMENT_ROOM_HEIGHT;
	int w,h,nIdx;
	std::vector<CollectedItem> vItems;
	std::vector<CollectedItem> vSpecIt;
	for(h=0;h<nH;++h)
	for(w=0;w<nW;++w)
	{
		nIdx = m_ItemList.m_Room[nRoom].FindItem(w, h);
		if(nIdx > 0)
		{
			if(Item[nIdx].GetGenre() == item_equip
			|| Item[nIdx].GetWidth() > 1 || Item[nIdx].GetHeight() > 1)
			{
				CollectedItem insitem;
				insitem.nIdx = nIdx;
				insitem.w = Item[nIdx].GetWidth();
				insitem.h = Item[nIdx].GetHeight();
				insitem.x = 0;
				insitem.y = 0;
				vItems.push_back(insitem);
			}
			else
			{
				CollectedItem specitem;
				specitem.nIdx = nIdx;
				specitem.w = 1;
				specitem.h = 1;
				specitem.x = 0;
				specitem.y = 0;
				vSpecIt.push_back(specitem);
			}
		}
	}
	memset(pnCmpArray, 0, sizeof(int)*EQUIPMENT_ROOM_WIDTH*EQUIPMENT_ROOM_HEIGHT);
	int i;
	int itemCount = (int)vItems.size();
	if(itemCount > 0)
	{
		int nCells = nW * nH;
		std::vector<TypePack> types;
		std::vector< std::vector<int> > itemsOfType;
	
		int j;
		for (i = 0; i < itemCount; ++i)
		{
			CollectedItem &ci = vItems[i];
	
			int typeIndex = -1;
			int typeCount = (int)types.size();
			for (j = 0; j < typeCount; ++j)
			{
				if (types[j].w == ci.w && types[j].h == ci.h)
				{
					typeIndex = j;
					break;
				}
			}
			if (typeIndex == -1)
			{
				TypePack tp;
				tp.w = ci.w;
				tp.h = ci.h;
				tp.count = 1;
				types.push_back(tp);
	
				std::vector<int> lst;
				lst.push_back(i);
				itemsOfType.push_back(lst);
			}
			else
			{
				types[typeIndex].count++;
				itemsOfType[typeIndex].push_back(i);
			}
		}
	
		int totalArea = 0;
		for (i = 0; i < (int)types.size(); ++i)
		{
			totalArea += types[i].w * types[i].h * types[i].count;
		}
	
		std::vector<int> grid(nCells);
		for (i = 0; i < nCells; ++i)
			grid[i] = -1; // -1 = empty
	
		std::vector<Placement> curPlace;
		std::vector<Placement> bestPlace;
		bool bFoundFull = false;
	
		DFS_Pack(0, 0, totalArea,
			nW, nH, nCells,
			&grid[0],
			types,
			curPlace,
			bestPlace,
			bFoundFull);
		if (!bFoundFull)
			return 1;
	
		int typeCount = (int)types.size();
		std::vector<int> nextIndexOfType(typeCount);
		for (i = 0; i < typeCount; ++i)
			nextIndexOfType[i] = 0;
	
		int placeCount = (int)bestPlace.size();
		for (i = 0; i < placeCount; ++i)
		{
			const Placement &pl = bestPlace[i];
			int tIndex = pl.typeIndex;
	
			int k = nextIndexOfType[tIndex];
			if (k >= (int)itemsOfType[tIndex].size())
				continue; // safety
	
			int vIdx = itemsOfType[tIndex][k];
			nextIndexOfType[tIndex]++;
	
			CollectedItem &ci = vItems[vIdx];
			ci.x = pl.x;
			ci.y = pl.y;
	
			int dx, dy;
			for (dy = 0; dy < ci.h; ++dy)
			{
				for (dx = 0; dx < ci.w; ++dx)
				{
					int idxCell = (ci.y + dy) * nW + (ci.x + dx);
					pnCmpArray[idxCell] = ci.nIdx;
				}
			}
		}
	}
	for (i = 0; i < itemCount; ++i)
	{
		m_ItemList.Remove(vItems[i].nIdx);
	}
	for (i = 0; i < (int)vSpecIt.size(); ++i)
	{
		m_ItemList.Remove(vSpecIt[i].nIdx);
	}
	for (i = 0; i < itemCount; ++i)
	{
		m_ItemList.AddKIL(vItems[i].nIdx, nPos, vItems[i].x, vItems[i].y);
	}
	for (i = 0; i < (int)vSpecIt.size(); ++i)
	{
		if(Item[vSpecIt[i].nIdx].GetGenre() != item_medicine)
		{
			bool bAdded = false;
			for(h=0;h<nH;++h)
			{
				if(bAdded)
					break;
				for(w=0;w<nW;++w)
				{
					if(!pnCmpArray[h*nW+w])
					{
						bAdded = true;
						pnCmpArray[h*nW+w] = vSpecIt[i].nIdx;
						m_ItemList.AddKIL(vSpecIt[i].nIdx, nPos, w, h);
						break;
					}
				}
			}
		}
	}
	for (i = 0; i < (int)vSpecIt.size(); ++i)
	{
		if(Item[vSpecIt[i].nIdx].GetGenre() == item_medicine)
		{
			bool bAdded = false;
			for(h=0;h<nH;++h)
			{
				if(bAdded)
					break;
				for(w=0;w<nW;++w)
				{
					if(!pnCmpArray[h*nW+w])
					{
						bAdded = true;
						pnCmpArray[h*nW+w] = vSpecIt[i].nIdx;
						m_ItemList.AddKIL(vSpecIt[i].nIdx, nPos, w, h);
						break;
					}
				}
			}
		}
	}
	return 1;
}
#endif

// auto playgame
void KPlayer::SetAutoFlag(BOOL nbAuto)
{/*		
	m_bActiveAuto = nbAuto;
#ifdef _SERVER
	PLAYER_REQUEST_AUTO Cmd;
	Cmd.ProtocolType = s2c_autoplay;
	Cmd.m_bAuto = 1;
	Cmd.m_bActive = nbAuto;
	if(g_pServer && this->m_nNetConnectIdx != -1)
		g_pServer->PackDataToClient(this->m_nNetConnectIdx, &Cmd, sizeof(PLAYER_REQUEST_AUTO));
#endif*/
}

#ifndef _SERVER
void KPlayer::SendInfoAuto()
{
	int nDesX = 0;
	int nDesY = 0;
	Npc[m_nIndex].GetMpsPos(&nDesX, &nDesY);
	int nDesTempX = nDesX/8/32;
	int nDesTempY = nDesY/16/32;
	int nDesTempXAuto = m_PosXAuto/8/32;
	int nDesTempYAuto = m_PosYAuto/16/32;
	if (nDesTempX != nDesTempXAuto || nDesTempY != nDesTempYAuto)
	{
		m_PosXAuto = nDesX;
		m_PosYAuto = nDesY;
	}
	m_Actacker = 0 ;
	m_bActacker = FALSE;
	m_Count_Acttack_Lag = 0;
	m_nLifeLag = 0;
	m_nTimeRunLag = 0;
	
	ClearArrayNpcNeast();
	ClearArrayObjectNeast();
	ClearArrayNpcLag();
	ClearArrayObjectLag();
	ClearArrayTimeNpcLag();
	ClearArrayTimeObjectLag();
}

void KPlayer::PlayerAuto()
{
	int index = 0;
	int	iObject = 0;

	if (Npc[m_nIndex].m_Doing == do_death || Npc[m_nIndex].m_Doing == do_revive || !Npc[m_nIndex].IsCanInput())
    return;


	if (IR_GetCurrentTime() - m_SortEQCountDown > 30000)//sau 30 giay tu dong xep hanh trang
	{
		m_SortEQCountDown = IR_GetCurrentTime();
		SetSortEquipment(m_bSortEquipment_Active);
	}

	if (IR_GetCurrentTime() - m_SpaceActionTime > AUTO_TIME_SORT_ITEM)
	{
		m_SpaceActionTime = IR_GetCurrentTime();
		if (m_bFilterEquipment)
			PlayerFilterEquip();
		
		//SortEquipment(); //Tu dong sap xep do
	}

	PlayerEatAItem();
	PlayerSwitchAura();

	if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode)//mÆc ®Þnh src nµy kh«ng lµm g× khi ë trong thµnh
	{
		if(ReturnFromPortal() && m_nReturnPortalStep < Step_Return)
			return;
	}

	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode)
	{
		if (m_nReturnPortalStep == Step_Training)
		{
		}
		else
		{
			m_nReturnPortalStep = Step_Training;
		}

		if (TRUE == AutoBuffSkillState()) return;

		int i = 0;
		int nCountNpcLag = 0;
		int nCountObjectLag = 0;
		for (i = 0; i < MAX_ARRAY_NPC_AUTO; i++)
		{
			if (m_ArrayNpcLag[i] > 0)
			{
				nCountNpcLag++;
				if (GetTickCount() - m_ArrayTimeNpcLag[i] > AUTO_TIME_RESET_LAG)
				{
					m_ArrayNpcLag[i] = 0;
					m_ArrayTimeNpcLag[i] = 0;
				}
			}
			
			if (m_ArrayObjectLag[i] > 0)
			{
				nCountObjectLag++;
				if (GetTickCount() - m_ArrayTimeObjectLag[i] > AUTO_TIME_RESET_LAG)
				{
					m_ArrayObjectLag[i] = 0;
					m_ArrayTimeObjectLag[i] = 0;
				}
			}			
		}

		if (nCountNpcLag >= MAX_ARRAY_NPC_AUTO)
		{
			ClearArrayNpcLag();
			ClearArrayTimeNpcLag();
		}

		if (nCountObjectLag >= MAX_ARRAY_NPC_AUTO)
		{
			ClearArrayObjectLag();
			ClearArrayTimeObjectLag();
		}

		//////////////////////////////////////////nhat do///////////////////////////////////////		
		//if (((m_bActacker == FALSE || m_Actacker == 0) && (m_bObject == FALSE || m_nObject == 0)) && m_bPickItem) //nhat do khi khong danh quai
		if ((m_bObject == FALSE || m_nObject == 0) && m_bPickItem)//Nhat do khi dang danh quai
		{
			iObject = FindNearObject2Array();
			if (iObject > 0)
			{
				BOOL _flagLag = FALSE;
				for (i = 0; i < MAX_ARRAY_NPC_AUTO; i++)
				{
					if (m_ArrayObjectLag[i] == iObject)
					{
						_flagLag = TRUE;
						break;
					}
				}
				
				if (_flagLag)
				{
					iObject = 0;
					m_nObject = 0;
					m_bObject = FALSE;
				}
				else
				{
					ItemPos	sItemPos;
					if (Object[iObject].m_nKind == Obj_Kind_Item)
					{
						if ( FALSE == m_ItemList.SearchPosition(Object[iObject].m_nItemWidth, 
							Object[iObject].m_nItemHeight, &sItemPos) || sItemPos.nPlace != pos_equiproom)
						{
							m_bObject = FALSE;
							m_nObject = 0;
						}
						else
						{
							m_bObject = TRUE;
							m_nObject = iObject;
						}
					}
					else if(Object[iObject].m_nKind == Obj_Kind_Money)
					{
						m_bObject = TRUE;
						m_nObject = iObject;
					}	
				}
			}
		}
		if (m_nObject > 0)
		{
			PlayerFollowObject(m_nObject);
			return;
		}
		m_bObject = FALSE;
		m_nObject = 0;
		//////////////////////////////////////////end nhat do///////////////////////////////////////		
		
		if (m_bFollowPeople == TRUE && m_FollowPeopleName[0]) //theo sau nhan vat, kiem tra nhan vat dang trang thai phi chien dau khong cho theo sau tranh keo xe
		{
			if (GetTickCount() - m_nTimeCheckFollowPeopleIdx >= 1000)
			{
				m_FollowPeopleIdx = NpcSet.SearchName(Player[CLIENT_PLAYER_INDEX].m_FollowPeopleName);//Check lai Id NPC theo sau 1s
				m_nTimeCheckFollowPeopleIdx = GetTickCount();
			}
			if (m_FollowPeopleIdx)
			{
				int nX, nY;
				Npc[m_FollowPeopleIdx].GetMpsPos(&nX,&nY);
				if(Npc[m_FollowPeopleIdx].m_nPKFlag != enumPKNormal)
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, "Kh«ng thÓ theo sau nh©n vËt tr¹ng th¸i PK phi chiÕn ®Êu");
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);					
				}
				else
				{
					int dZ = NpcSet.GetDistance(m_FollowPeopleIdx , Player[CLIENT_PLAYER_INDEX].m_nIndex);
					if(dZ >= 16) //khoang cach
					{
						MoveTo(nX,nY);
					}
				}
			}
		}

		if (m_bAutoParty && m_bActiveAutoParty)
		{
			if (m_cTeam.m_nFlag)
				AutoParty();
			else
				ApplyCreateTeam();
		}

		if (m_bActacker == FALSE || m_Actacker == 0)
		{
			SortEquipment(); //Tu dong sap xep do khi khong danh quai

			index = FindNearNpc2Array(relation_enemy);
			if (index > 0)
			{
				BOOL _flagLag = FALSE;
				for (i = 0; i < MAX_ARRAY_NPC_AUTO; i++)
				{
					if (m_ArrayNpcLag[i] == index)
					{
						_flagLag = TRUE;
						break;
					}
				}
				if (_flagLag)
				{
					m_Actacker = 0;
					m_bActacker = FALSE;
					index = 0;
				}
				else
				{
					m_Actacker = index;
					m_bActacker = TRUE;
				}
			}
		}
		else
		{
			index = m_Actacker;
		}
		if (index > 0 && m_AutoAttack == TRUE /*&& m_bFollowPeople == FALSE */)
		{
			PlayerFollowActack(index);
		}
		else
		{
			AutoReturn(); //moi viet them
			m_nLifeLag = 0;
			m_Actacker = 0;
			m_nTimeRunLag = 0;
			m_Count_Acttack_Lag = 0;
			m_bActacker = FALSE;
		}
	}
}

void KPlayer::PlayerSwitchAura()
{
	if (m_bActiveSwitchAura && m_bActiveAuto)
	{
		if (m_AuraSkill[0] && m_AuraSkill[1])
		{
			SYSTEMTIME systm;
			GetLocalTime(&systm);
			Player[CLIENT_PLAYER_INDEX].SetRightSkill((systm.wSecond % 2) ? m_AuraSkill[0] : m_AuraSkill[1]);
		}
	}
}


BOOL KPlayer::PlayerMoveMps()
{
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode == fight_active)
	{
		int nMoveCount = GetMoveMpsCount();
		if (m_MoveStep >= nMoveCount)
			m_MoveStep = 0;

		if (!m_MoveMps[m_MoveStep][0] || 
			(m_MoveMps[m_MoveStep][0] != SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID))
		{
			m_MoveStep++;
			return FALSE;
		}
		int nMapX, nMapY, dX, dY, dZ;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nMapX, &nMapY);
		dX = nMapX-m_MoveMps[m_MoveStep][1];
		dY = nMapY-m_MoveMps[m_MoveStep][2];
		dZ = (int)sqrt((float)dX*dX+(float)dY*dY);
		if (dZ>=64)
			MoveTo(m_MoveMps[m_MoveStep][1], m_MoveMps[m_MoveStep][2]);
		else
		{
			//viet them
			m_Actacker = 0;
			m_bActacker = FALSE;
			m_nTimeRunLag = 0;
			m_Count_Acttack_Lag = 0;
			m_MoveStep++;
		}
	}
	return FALSE;
}

int KPlayer::GetMoveMpsCount()
{
	int nCount = 0;
	for (int i = 0; i < defMAX_AUTO_MOVEMPSL; i ++)
	{
		if (m_MoveMps[i][0])
			nCount++;
	}
	return nCount;
}

BOOL KPlayer::IsNotValidNpc(int nIndex)
{
	if (nIndex <= 0 || Npc[nIndex].m_SubWorldIndex != Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex || Npc[nIndex].m_RegionIndex < 0 || 
	   (Npc[nIndex].m_Kind == kind_player && (FALSE == m_bAttackPeople || Npc[nIndex].m_FightMode == fight_none 
	   //|| m_bFollowPeople
	   )) || 
	   (Npc[nIndex].m_Kind == kind_normal && (FALSE == m_bAttackNpc 
	   || Npc[nIndex].m_Type >= boss_num  //#can kiem tra
	   )) 
	   || Npc[nIndex].m_CurrentLife <= 0 || Npc[nIndex].m_CurrentLifeMax <= 0 || 
		Npc[nIndex].m_HideState.nTime > 0 || 
		Npc[nIndex].m_Doing == do_death || 
		Npc[nIndex].m_Doing == do_revive)

	{
		m_Actacker = 0;
		m_bActacker = FALSE;
		m_nLifeLag = 0;
		m_nTimeRunLag = 0;
		m_Count_Acttack_Lag = 0;
		//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = 0;
		return TRUE;
	}
	return FALSE;
}

BOOL KPlayer::IsRAPEquip(BYTE btDetail)
{
	if (btDetail == equip_ring || 
		btDetail == equip_amulet || 
		btDetail == equip_pendant)
		return TRUE;
	return FALSE;
}

BOOL KPlayer::IsEquipSatisfyCondition(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_ITEM)
		return FALSE;
		
	int i, j;
	for (i = 0; i < defMAX_AUTO_FILTERL; i++)
	{
		if (m_FilterMagic[i][0])
		{
			for (j = 0; j < MAX_ITEM_MAGICLEVEL; j++)
			{
				if (Item[nIdx].m_aryMagicAttrib[j].nAttribType == m_FilterMagic[i][0] && 
					Item[nIdx].m_aryMagicAttrib[j].nValue[0] >= m_FilterMagic[i][1])
					return TRUE;
			}
		}
	}
	return FALSE;
}

void KPlayer::PlayerFilterEquip()
{
	int nHand = m_ItemList.Hand();
	if (nHand)
	{
		if (Item[nHand].GetGenre() == item_equip && Item[nHand].GetDetailType() < equip_horse) 
		{
			if(IsEquipSatisfyCondition(nHand))	//xu ly khi dang xep do khong loc do
				return;
			if(m_SaveRAP && IsRAPEquip(Item[nHand].GetDetailType())) //xu ly khi dang xep do khong loc do
				return;

			PLAYER_THROW_AWAY_ITEM_COMMAND	sThrow;
			sThrow.ProtocolType = c2s_playerthrowawayitem;
			if (g_pClient)
				g_pClient->SendPackToServer(&sThrow, sizeof(PLAYER_THROW_AWAY_ITEM_COMMAND));
		} 			
	}
	else
	{
		ItemPos	P;
		PlayerItem* pItem = m_ItemList.GetFirstItem();
		if (pItem)
		{
			if (pItem->nPlace == pos_equiproom && 
				Item[pItem->nIdx].GetGenre() == item_equip && 
				Item[pItem->nIdx].GetDetailType() < equip_horse)
			{
				BOOL bFilter = TRUE;
				if (Item[pItem->nIdx].GetKind() == gold_item)
					bFilter = FALSE;
				else if (Item[pItem->nIdx].GetPlayerItemLock() > 0)
					bFilter = FALSE;
				else if (Item[pItem->nIdx].GetPlayerItemLock()  == -2)
					bFilter = FALSE;
				else if (Item[pItem->nIdx].m_nCurrentDur == 0)	//#trang bi hong khong vut ra
					bFilter = FALSE;
				else 
					if (m_SaveRAP && IsRAPEquip(Item[pItem->nIdx].GetDetailType()))
					bFilter = FALSE;
				else
				{
					if (IsEquipSatisfyCondition(pItem->nIdx))
						bFilter = FALSE;
				}				
				
				if (bFilter)
				{					
					P.nPlace = pItem->nPlace;
					P.nX = pItem->nX;
					P.nY = pItem->nY;
					MoveItem(P, P);
					return;
				}
			}
		}
		int n = 0;
		while(pItem) 
		{ 
			pItem = m_ItemList.GetNextItem();
			n++;
			if (pItem)
			{
				if (pItem->nPlace == pos_equiproom && 
					Item[pItem->nIdx].GetGenre() == item_equip && 
					Item[pItem->nIdx].GetDetailType() < equip_horse)
				{
					BOOL bFilter = TRUE;
					if (Item[pItem->nIdx].GetKind() == gold_item)
						bFilter = FALSE;
					else if (Item[pItem->nIdx].GetPlayerItemLock() > 0)
						bFilter = FALSE;
					else if (Item[pItem->nIdx].GetPlayerItemLock()  == -2)
						bFilter = FALSE;
					else if (Item[pItem->nIdx].m_nCurrentDur == 0)	//#trang bi hong khong vut ra
						bFilter = FALSE;
					else 
						if (m_SaveRAP && IsRAPEquip(Item[pItem->nIdx].GetDetailType()))
						bFilter = FALSE;							
					else
					{
						if (IsEquipSatisfyCondition(pItem->nIdx))
							bFilter = FALSE;
					}		
							
					if (bFilter)
					{												
						P.nPlace = pItem->nPlace;
						P.nX = pItem->nX;
						P.nY = pItem->nY;
						MoveItem(P, P);
						return;
					}
				}
			}
		}
	}
}

void KPlayer::AutoAddNpc2Array(int nRelation)
{
	int nRangeX = 0;
	int	nRangeY = 0;
	int	nSubWorldIdx = 0;
	int	nRegion = 0;
	int	nMapX = 0;
	int	nMapY = 0;
	if (m_bAttackAround) // Che do danh quanh diem
	{
		nRangeX = m_RadiusAuto;
		nRangeY = m_RadiusAuto;
	}
	else						// che do danh tum lum
	{
		nRangeX = 1024;
		nRangeY = 1024;
	}
	nSubWorldIdx = Npc[m_nIndex].m_SubWorldIndex;
	nRegion = Npc[m_nIndex].m_RegionIndex;
	nMapX = Npc[m_nIndex].m_MapX;
	nMapY = Npc[m_nIndex].m_MapY;
	int	nRet;
	int	nRMx, nRMy, nSearchRegion;

	nRangeX = nRangeX / SubWorld[nSubWorldIdx].m_nCellWidth;
	nRangeY = nRangeY / SubWorld[nSubWorldIdx].m_nCellHeight;	

	for (int j = 0; j < MAX_ARRAY_NPC_AUTO; j++)
	{
		m_ArrayNpcNeast[j] = 0;
	}

	for (int i = 0; i < nRangeX; i++)
	{
		for (int j = 0; j < nRangeY; j++)
		{
			if ((i * i + j * j) > nRangeX * nRangeX)
				continue;

			nRMx = nMapX + i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorldIdx].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorldIdx].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorldIdx].m_nRegionHeight;
			}

			if (nSearchRegion == -1)
				continue;
			nRet = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, nRelation);

			if (Npc[nRet].m_Doing == do_death || Npc[nRet].m_Doing == do_revive)
				nRet = 0;
			
			if (nRet > 0)
			{
				for (int i=0; i < MAX_ARRAY_NPC_AUTO; i++)
				{
					if (m_ArrayNpcNeast[i] == 0)
					{
						m_ArrayNpcNeast[i] = nRet;
						break;
					}
				}
			}
			
			nRMx = nMapX - i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorldIdx].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorldIdx].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorldIdx].m_nRegionHeight;
			}

			if (nSearchRegion == -1)
				continue;
			nRet = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, nRelation);

			if (Npc[nRet].m_Doing == do_death || Npc[nRet].m_Doing == do_revive)
				nRet = 0;
			
			if (nRet > 0)
			{
				for (int i=0; i < MAX_ARRAY_NPC_AUTO; i++)
				{
					if (m_ArrayNpcNeast[i] == 0)
					{
						m_ArrayNpcNeast[i] = nRet;
						break;
					}
				}
			}
			
			nRMx = nMapX - i;
			nRMy = nMapY - j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorldIdx].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorldIdx].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorldIdx].m_nRegionHeight;
			}

			if (nSearchRegion == -1)
				continue;
			nRet = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, nRelation);
			
			if (Npc[nRet].m_Doing == do_death || Npc[nRet].m_Doing == do_revive)
				nRet = 0;

			if (nRet > 0)
			{
				for (int i=0; i < MAX_ARRAY_NPC_AUTO; i++)
				{
					if (m_ArrayNpcNeast[i] == 0)
					{
						m_ArrayNpcNeast[i] = nRet;
						break;
					}
				}
			}
			
			nRMx = nMapX + i;
			nRMy = nMapY - j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorldIdx].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorldIdx].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorldIdx].m_nRegionHeight;
			}

			if (nSearchRegion == -1)
				continue;
			nRet = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, nRelation);
			
			if (Npc[nRet].m_Doing == do_death || Npc[nRet].m_Doing == do_revive)
				nRet = 0;
			
			if (nRet > 0)
			{
				for (int i=0; i < MAX_ARRAY_NPC_AUTO; i++)
				{
					if (m_ArrayNpcNeast[i] == 0)
					{
						m_ArrayNpcNeast[i] = nRet;
						break;
					}
				}
			}
		}
	}
}

void KPlayer::FindObjectNearPlayer()
{
	int nRangeX = 0;
	int	nRangeY = 0;
	int	nSubWorldIdx = 0;
	int	nRegion = 0;
	int	nMapX = 0;
	int	nMapY = 0;

	if (m_bAttackAround) // Che do danh quanh diem
	{
		nRangeX = m_RadiusAuto;
		nRangeY = m_RadiusAuto;
	}
	else						// che do danh tum lum
	{
		nRangeX = 1024;
		nRangeY = 1024;
	}
	nSubWorldIdx = Npc[m_nIndex].m_SubWorldIndex;
	nRegion = Npc[m_nIndex].m_RegionIndex;
	nMapX = Npc[m_nIndex].m_MapX;
	nMapY = Npc[m_nIndex].m_MapY;

	int	nRet;
	int	nRMx, nRMy, nSearchRegion;

	nRangeX = nRangeX / SubWorld[nSubWorldIdx].m_nCellWidth;
	nRangeY = nRangeY / SubWorld[nSubWorldIdx].m_nCellHeight;	

	ClearArrayObjectNeast();

	for (int i = 0; i < nRangeX; i++)
	{
		for (int j = 0; j < nRangeY; j++)
		{
			if ((i * i + j * j) > nRangeX * nRangeX)
				continue;

			nRMx = nMapX + i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorldIdx].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorldIdx].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
				
			nRet = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].FindObject(nRMx, nRMy, true);
			if (nRet > 0)
			{	
				for (int i=0; i < MAX_ARRAY_NPC_AUTO; i++)
				{
					if (m_ArrayObjectNeast[i] == 0)
					{
						m_ArrayObjectNeast[i] = nRet;
						break;
					}
				}
			}
			nRMx = nMapX - i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorldIdx].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorldIdx].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;		
			
			nRet = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].FindObject(nRMx, nRMy, true);
			if (nRet > 0)
			{	
				for (int i=0; i < MAX_ARRAY_NPC_AUTO; i++)
				{
					if (m_ArrayObjectNeast[i] == 0)
					{
						m_ArrayObjectNeast[i] = nRet;
						break;
					}
				}
			}
			
			nRMx = nMapX - i;
			nRMy = nMapY - j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorldIdx].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorldIdx].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			
			nRet = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].FindObject(nRMx, nRMy, true);
			if (nRet > 0)
			{	
				for (int i=0; i < MAX_ARRAY_NPC_AUTO; i++)
				{
					if (m_ArrayObjectNeast[i] == 0)
					{
						m_ArrayObjectNeast[i] = nRet;
						break;
					}
				}
			}

			nRMx = nMapX + i;
			nRMy = nMapY - j;
			nSearchRegion = nRegion;			
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorldIdx].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorldIdx].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			
			nRet = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].FindObject(nRMx, nRMy, true);
			if (nRet > 0)
			{	
				for (int i=0; i < MAX_ARRAY_NPC_AUTO; i++)
				{
					if (m_ArrayObjectNeast[i] == 0)
					{
						m_ArrayObjectNeast[i] = nRet;
						break;
					}
				}
			}
		}
	}
}

int KPlayer::FindNearNpc2Array(int nRelation)
{
	int nRet = 0;
	AutoAddNpc2Array(nRelation);
	int distance = 0 ;
	int distanceMin = 0 ;
	BOOL _fg = FALSE;
	int i = 0;
	int j = 0;
	for (i=0; i < MAX_ARRAY_NPC_AUTO; i++)
	{
		if (m_ArrayNpcNeast[i] != 0)
		{
			BOOL _findLag = FALSE;
			for (j = 0; j < MAX_ARRAY_NPC_AUTO; j++)
			{
				if (m_ArrayNpcNeast[i] == m_ArrayNpcLag[j])
				{
					_findLag = TRUE;
					break;
				}
			}
			if (_findLag == TRUE)
				continue;
			
			if (IsNotValidNpc(m_ArrayNpcNeast[i]))		
				continue;
			distance = NpcSet.GetDistance(m_nIndex, m_ArrayNpcNeast[i]);
			if (FALSE == _fg)
			{
				distanceMin = distance;
				_fg = TRUE;
				nRet = m_ArrayNpcNeast[i];
			}
			if (distance <= distanceMin)
			{
				distanceMin = distance;
				nRet = m_ArrayNpcNeast[i];
			}
		}
	}

	if (nRet == 0)
	{
		if (m_AutoAttack == TRUE)
			AutoReturn();
		m_Actacker = 0;
		m_bActacker = FALSE;
		m_nTimeRunLag = 0;
		m_Count_Acttack_Lag = 0;
	}
	return nRet;
}

int KPlayer::FindNearObject2Array()
{
	int nRet = 0 ;
	FindObjectNearPlayer();
	int distance = 0 ;
	int distanceMin = 0 ;
	BOOL _fg = FALSE;
	for (int i=0; i < MAX_ARRAY_NPC_AUTO; i++)
	{
		if (m_ArrayObjectNeast[i] != 0)
		{
			BOOL _findLag = FALSE;
			for (int j=0; j < MAX_ARRAY_NPC_AUTO; j++)
			{		
				if (Object[m_ArrayObjectNeast[i]].m_nID == m_ArrayObjectLag[j])
				{
					_findLag = TRUE;
					break;
				}
			}
			if (_findLag == TRUE)
				continue;
			if (IsNotValidObject(m_ArrayObjectNeast[i]))
				continue;				
			distance = Object[m_ArrayObjectNeast[i]].GetDistanceSquare(m_nIndex);
			if (FALSE == _fg)
			{
				distanceMin = distance ;
				_fg = TRUE;
				nRet = m_ArrayObjectNeast[i];
			}
			if (distance <= distanceMin)
			{
				distanceMin = distance ;
				nRet = m_ArrayObjectNeast[i];
			}
		}
	}

	if (nRet == 0)
	{
		m_nObject = 0;
		m_bObject = FALSE;
		m_nTimeRunLag = 0;
	}
	return nRet;
}

BOOL KPlayer::IsNotValidObject(int nObject)
{		
	if (!m_bActiveAuto && Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode == fight_none)
		return TRUE;

	if (nObject > 0 && nObject < MAX_OBJECT)
	{
		if (m_bPickItem)
		{
			if (Object[nObject].m_nKind == Obj_Kind_Money)
			{
				if (m_btPickUpKind >= enumPickUpEarn)
					return FALSE;
			}
			else if (Object[nObject].m_nKind == Obj_Kind_Item)
			{
			
				if (m_btPickUpKind == enumPickEventItem)
				{
					if (Object[nObject].m_nGenre != item_equip)
						return FALSE;			
				}
				else if (m_btPickUpKind == enumPickItemColor)
				{
					if (Object[nObject].m_nGenre >= equip_horse && Object[nObject].m_nGenre < equip_detailnum)
					{
						if (Object[nObject].m_dwNpcId2 == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID || Object[nObject].m_dwNpcId2 > 0) //vat pham nem ra dat roi ko nhat nua
						{
							return m_bFilterEquipment;
						}
						return FALSE;
					}

					if (Object[nObject].m_nColorID > 0)
					{
						if (Object[nObject].m_dwNpcId2 == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID || Object[nObject].m_dwNpcId2 > 0) //vat pham nem ra dat roi ko nhat nua
						{
							return m_bFilterEquipment;
						}
						return FALSE;
					}

				}
				else if (m_btPickUpKind == enumPickUpAll)
				{
					if (Object[nObject].m_dwNpcId2 == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID || Object[nObject].m_dwNpcId2 > 0) //vat pham nem ra dat roi ko nhat nua
					{
						return m_bFilterEquipment;
					}
					return FALSE;				
				}
			}
		}
	}
	return TRUE;
}

int KEquipmentArray::FindSame(int i)
{
	int nIdx = 0;
	while(1)
	{
		nIdx = m_Link.GetNext(nIdx);
		if (!nIdx)
			break;

		if (nIdx == i)
			return nIdx;
	}
	return 0;
}

int KEquipmentArray::FindSameGerne(int i)
{
	int nIdx = 0;
	while (1)
	{
		nIdx = m_Link.GetNext(nIdx);
		if (!nIdx)
			break;

		if (Item[nIdx].GetGenre() == Item[i].GetGenre())
			return nIdx;
	}
	return 0;
}

void KPlayer::PlayerFollowActack(int i)
{
	AUTOLOG_EVERY(1000, "AUTO-TGT-CHK i=%d sw=%d/%d rgn=%d kind=%u type=%d life=%d/%d hide=%d doing=%d fm=%d", i, Npc[i].m_SubWorldIndex, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex, Npc[i].m_RegionIndex, Npc[i].m_Kind, Npc[i].m_Type, Npc[i].m_CurrentLife, Npc[i].m_CurrentLifeMax, Npc[i].m_HideState.nTime, (int)Npc[i].m_Doing, Npc[i].m_FightMode);
	if (IsNotValidNpc(i))
		return;

	int _i = 0;
	int nMapX, nMapY, dX, dY, dZ, k, h;
	Npc[m_nIndex].GetMpsPos(&nMapX, &nMapY);
	Npc[i].GetMpsPos(&k, &h);
	if (m_bAttackAround)
	{
		if ((k - nMapX) * (k - nMapX) + (h - nMapY) * (h - nMapY) > m_RadiusAuto * 1024)
		{
			AUTOLOG_EVERY(1000, "[AUTO-LEASH] npc=%d tg=(%d,%d) me=(%d,%d) radius=%d around=%d", i, k, h, nMapX, nMapY, m_RadiusAuto, (int)m_bAttackAround);
			dX = nMapX-m_PosXAuto;
			dY = nMapY-m_PosYAuto;
			dZ = (int)sqrt((float)dX*dX+(float)dY*dY);
			if (dZ>=64)
			{
				AutoReturn();
				m_nLifeLag = 0;
				m_Actacker = 0;
				m_nTimeRunLag = 0;
				m_Count_Acttack_Lag = 0;
				m_bActacker = FALSE;
				return;
			}
		}
	}
	AUTOLOG("AUTO-TGT-REGION npc=%d rgn=%d sw=%d id=%u t=%u", i, Npc[i].m_RegionIndex, Npc[i].m_SubWorldIndex, Npc[i].m_dwID, GetTickCount());
	if ( Npc[i].m_RegionIndex < 0 )
	{
		m_nLifeLag = 0;
		m_Actacker = 0;
		m_nTimeRunLag = 0;
		m_Count_Acttack_Lag = 0;
	if (g_AutoLogOn())
	{
		// FIX 21/08: dong log nay dung TRUOC cua kiem IsNotValidNpc(i) (ngat mach o nIndex <= 0).
		int nLogI = (i > 0 && i < MAX_NPC) ? i : -1;
		int nLogMe = (Player[CLIENT_PLAYER_INDEX].m_nIndex > 0 && Player[CLIENT_PLAYER_INDEX].m_nIndex < MAX_NPC) ? Player[CLIENT_PLAYER_INDEX].m_nIndex : -1;
		AUTOLOG_EVERY(1000, "AUTO-TGT-CHK i=%d sw=%d/%d rgn=%d kind=%u type=%d life=%d/%d hide=%d doing=%d fm=%d", i, (nLogI >= 0) ? Npc[nLogI].m_SubWorldIndex : -1, (nLogMe >= 0) ? Npc[nLogMe].m_SubWorldIndex : -1, (nLogI >= 0) ? Npc[nLogI].m_RegionIndex : -1, (nLogI >= 0) ? Npc[nLogI].m_Kind : 0, (nLogI >= 0) ? Npc[nLogI].m_Type : -1, (nLogI >= 0) ? Npc[nLogI].m_CurrentLife : -1, (nLogI >= 0) ? Npc[nLogI].m_CurrentLifeMax : -1, (nLogI >= 0) ? Npc[nLogI].m_HideState.nTime : 0, (nLogI >= 0) ? (int)Npc[nLogI].m_Doing : -1, (nLogI >= 0) ? Npc[nLogI].m_FightMode : -1);
	}
		return;
	}
	
	if (m_nLifeLag == Npc[i].m_CurrentLife)
	{
		m_nTimeRunLag++;
		m_Count_Acttack_Lag++;
	}
	else
	{
		m_nTimeRunLag = 0;
		m_Count_Acttack_Lag = 0;
	}

	if (m_Count_Acttack_Lag >= AUTO_COUNT_LAG || m_nTimeRunLag >= AUTO_TIME_LAG)
	{
		for (int j=0; j < MAX_ARRAY_NPC_AUTO; j++)
		{
			if (m_ArrayNpcLag[j] == 0)
			{
				//KSystemMessage	sMsg;
				//sprintf(sMsg.szMessage, "%s bÞ lag täa ®é!", Npc[i].Name);
				//sMsg.eType = SMT_NORMAL;
				//sMsg.byConfirmType = SMCT_NONE;
				//sMsg.byPriority = 0;
				//sMsg.byParamSize = 0;
				//CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				AUTOLOG("AUTO-BLACKLIST npc=%d slot=%d cntlag=%d timelag=%d lifelag=%d life=%d dist=%d t=%u", i, j, m_Count_Acttack_Lag, m_nTimeRunLag, m_nLifeLag, Npc[i].m_CurrentLife, NpcSet.GetDistance(m_nIndex, i), GetTickCount());
				m_ArrayNpcLag[j] = i;
				m_ArrayTimeNpcLag[j] = GetTickCount();
				m_Actacker = 0;
				m_bActacker = FALSE;
				m_nLifeLag = 0;
				m_nTimeRunLag = 0;
				m_Count_Acttack_Lag = 0;
				return;
			}
		}
	}
	
	AUTOLOG("AUTO-TGT-DEAD-INC i=%d doing=%d life=%d id=%u nexti=%d t=%u", i, (int)Npc[i].m_Doing, Npc[i].m_CurrentLife, Npc[i].m_dwID, i + 1, GetTickCount());
	if(Npc[i].m_Doing == do_death)
		i++;

	int distanceattack = 0;
	if (m_bFightDistance)
	{
		distanceattack = m_DistanceAuto;	//danh tiep can 75
	}
	else
	{
		distanceattack = Npc[m_nIndex].m_CurrentAttackRadius;	//danh tu xa dua theo pham vi skill
		AUTOLOG_EVERY(1000, "AUTO-RANGE-CFG fightdist=%d distauto=%d curradius=%d active=%d skL=%d", (int)m_bFightDistance, m_DistanceAuto, Npc[m_nIndex].m_CurrentAttackRadius, Npc[m_nIndex].m_ActiveSkillID, m_nLeftSkillID);
		if(distanceattack < 75)
		{
			distanceattack = m_DistanceAuto;	//fix truêng hîp nh©n vËt kh«ng tù ®¸nh m_CurrentAttackRadius = 0
		}
	}

	int distance = NpcSet.GetDistance(m_nIndex, i);
	AUTOLOG_EVERY(500, "AUTO-DIST npc=%d dist=%d need=%d inrange=%d px=%d py=%d nx=%d ny=%d", i, distance, distanceattack, (int)(distance <= distanceattack), nMapX, nMapY, k, h);

	if (distance <= distanceattack)
	{
		int nIdx = 0;
		nIdx = Npc[m_nIndex].m_SkillList.FindSame(m_nLeftSkillID);
		Npc[m_nIndex].SetActiveSkill(nIdx);
		// Kiem tra skill su dung khi tren ngua hay la gi
		KSkill * pSkill = (KSkill *) g_SkillManager.GetSkill(Npc[m_nIndex].m_ActiveSkillID, 1);
		if (pSkill)
		{
			AUTOLOG("AUTO-HORSELIMIT skill=%d limit=%d ride=%d dt=%u", Npc[m_nIndex].m_ActiveSkillID, pSkill->GetHorseLimit(), (int)Npc[m_nIndex].m_bRideHorse, GetTickCount() - (DWORD)Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TimeHorse);
			if (pSkill->GetHorseLimit())
			{
				switch(pSkill->GetHorseLimit())
				{
				case 1:
					{
						if (Npc[m_nIndex].m_bRideHorse)
						{
							if(GetTickCount() - Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TimeHorse >= TIME_RIDE)
							{
								SendClientCmdRide(TRUE);// len xuong ngua
							}
							return;
						}
					}
					break;
				case 2:
					{
						if (!Npc[m_nIndex].m_bRideHorse)
						{
							if(GetTickCount() - Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TimeHorse >= TIME_RIDE)
							{
								SendClientCmdRide(FALSE);// len xuong ngua
							}
							return;
						}
					}
					break;
				default:
					return;
				}
			}
			
		}
		
		if (m_bFollowPeople == TRUE && m_FollowPeopleName[0]) //#Che do theo sau danh quai
		{
			m_FollowPeopleIdx = NpcSet.SearchName(Player[CLIENT_PLAYER_INDEX].m_FollowPeopleName);
			if(m_FollowPeopleIdx)
			{
				if(Npc[m_FollowPeopleIdx].m_nPKFlag != enumPKNormal)
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, "Kh«ng thÓ theo sau nh©n vËt tr¹ng th¸i PK phi chiÕn ®Êu");
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);					
				}
				else
				{
					int nX, nY, nX1, nY1;
					Npc[m_FollowPeopleIdx].GetMpsPos(&nX,&nY);
					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nX1,&nY1);
					dX = nX-nX1;
					dY = nY-nY1;				
					dZ = (int)sqrt((float)dX*dX+(float)dY*dY);
					//dZ = NpcSet.GetDistance(m_FollowPeopleIdx , i);
					if (dZ >= 200)//khoang cach 200
					{
						//viet them
						m_Actacker = 0;
						m_bActacker = FALSE;
						m_nTimeRunLag = 0;
						m_Count_Acttack_Lag = 0;
						MoveTo(nX, nY);
						return;
					}
				}
			}
		}
		
		if(m_Auto_SkillRight && m_nRightSkillID) //add by phong kiÒu ®¸nh chiªu bªn ph¶i
		{
			int nIdx = Npc[m_nIndex].m_SkillList.FindSame(m_nRightSkillID);
			AUTOLOG("AUTO-CAST-R skR=%d nIdx=%d activeOld=%d npc=%d id=%u dist=%d mana=%d t=%u", m_nRightSkillID, nIdx, Npc[m_nIndex].m_ActiveSkillID, i, Npc[i].m_dwID, distance, Npc[m_nIndex].m_CurrentMana, GetTickCount());
			Npc[m_nIndex].SetActiveSkill(nIdx);
			Npc[m_nIndex].SendCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, i);//auto skill ë ®©y
			SendClientCmdSkill(Npc[m_nIndex].m_ActiveSkillID, -1, Npc[i].m_dwID);
			m_nLifeLag = Npc[i].m_CurrentLife ;
			return;
		}
		else
		{
			int nIdx = Npc[m_nIndex].m_SkillList.FindSame(m_nLeftSkillID);
			AUTOLOG("AUTO-CAST-L skL=%d nIdx=%d activeOld=%d npc=%d id=%u dist=%d mana=%d t=%u", m_nLeftSkillID, nIdx, Npc[m_nIndex].m_ActiveSkillID, i, Npc[i].m_dwID, distance, Npc[m_nIndex].m_CurrentMana, GetTickCount());
			Npc[m_nIndex].SetActiveSkill(nIdx);
			Npc[m_nIndex].SendCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, i);//auto skill ë ®©y
			SendClientCmdSkill(Npc[m_nIndex].m_ActiveSkillID, -1, Npc[i].m_dwID);
			m_nLifeLag = Npc[i].m_CurrentLife ;
			return;
		}
	}
	
	m_nTimeRunLag++;
	//GIOI HAN PHAM VI TIM KIEM
	/*if (m_bFollowPeople == TRUE && m_FollowPeopleName[0]) //#Che do theo sau danh quai
	{
		m_FollowPeopleIdx = NpcSet.SearchName(Player[CLIENT_PLAYER_INDEX].m_FollowPeopleName);
		if(m_FollowPeopleIdx)
		{
			if(Npc[m_FollowPeopleIdx].m_nPKFlag != enumPKNormal)
			{
				KSystemMessage	sMsg;
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				sprintf(sMsg.szMessage, "Kh«ng thÓ theo sau nh©n vËt tr¹ng th¸i PK phi chiÕn ®Êu");
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);					
			}
			else
			{
				int nX, nY, nX1, nY1;
				Npc[m_FollowPeopleIdx].GetMpsPos(&nX,&nY);
				//dZ = NpcSet.GetDistance(m_FollowPeopleIdx , i);
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nX1,&nY1);
				dX = nX-nX1;
				dY = nY-nY1;
				dZ = (int)sqrt((float)dX*dX+(float)dY*dY);
				if (dZ >= 300)//khoang cach 200
				{
					//viet them
					m_Actacker = 0;
					m_bActacker = FALSE;
					m_nTimeRunLag = 0;
					m_Count_Acttack_Lag = 0;
					MoveTo(nX, nY);
					return;
				}
			}
		}
	}*/
	if(m_bAttackAround)
	{
		dX = k-m_PosXAuto;
		dY = h-m_PosYAuto;
		dZ = (int)sqrt((float)dX*dX+(float)dY*dY);
		if (dZ>=m_RadiusAuto)
		{
			AutoReturn();
			//viet them
			m_Actacker = 0;
			m_bActacker = FALSE;
			m_nTimeRunLag = 0;
			m_Count_Acttack_Lag = 0;
			return;
		}
	}
	else if(m_AutoMove)
	{
		dX = k-m_MoveMps[m_MoveStep][1];
		dY = h-m_MoveMps[m_MoveStep][2];
		dZ = (int)sqrt((float)dX*dX+(float)dY*dY);
		if (dZ>=m_RadiusAuto)
		{
			PlayerMoveMps();
			return;
		}
	}
	//else //#di chuyen den toa do npc quai vat
	{
		MoveTo(k, h);
		m_Actacker = 0;
		m_bActacker = FALSE;
	}
}

void KPlayer::SetSortEquipment(BOOL bFlag)
{
	if (m_bActiveAuto)
	{
		for (int i = MAX_ITEM - 1; i > 0 ; i--)
		{
			m_sListEquipment.m_Link.Remove(i);
		}
		m_bSortEquipment = bFlag;
	}
}

void KPlayer::SortEquipment()
{
	if (m_bSortEquipment)
	{
		int nHand = Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();//Item dang o tren tay
		if (nHand)
		{
			ItemPos	P;
			if ( FALSE == Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(Item[nHand].GetWidth(), Item[nHand].GetHeight(), &P) )		
			{
				m_bSortEquipment = FALSE;
				return;
			}		
			Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
			if (!m_sListEquipment.FindSame(nHand))
			{
				m_sListEquipment.m_Link.Insert(nHand);
			}				
		}
		else//item trong hanh trang
		{
			ItemPos	P;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			
			if (pItem)
			{
				if (pItem->nPlace == pos_equiproom)
				{
					if (!m_sListEquipment.FindSame(pItem->nIdx))
					{
						P.nPlace = pItem->nPlace;
						P.nX = pItem->nX;
						P.nY = pItem->nY;
						Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
						return;
					}
				}
			}

			while(pItem) 
			{ 
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem)
				{
					if (pItem->nPlace == pos_equiproom)
					{
						if (!m_sListEquipment.FindSame(pItem->nIdx))
						{
							P.nPlace = pItem->nPlace;
							P.nX = pItem->nX;
							P.nY = pItem->nY;
							Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
							return;
						}
					}
				}
			}
			m_bSortEquipment = FALSE;
		}
	}
}

BOOL KPlayer::CheckEquip(BYTE btDetail)
{
	if (btDetail == equip_ring || 
		btDetail == equip_amulet || 
		btDetail == equip_pendant)
		return TRUE;
	return FALSE;
}

BOOL KPlayer::CheckEquipMagic(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_ITEM)
		return FALSE;

	int i, j;
	for (i = 0; i < defMAX_AUTO_FILTERL; i++)
	{
		if (m_FilterMagic[i][0])
		{
			for (j = 0; j < MAX_ITEM_MAGICATTRIB; j++)
			{
				if (Item[nIdx].m_aryMagicAttrib[j].nAttribType == m_FilterMagic[i][0] && 
					Item[nIdx].m_aryMagicAttrib[j].nValue[0] >= m_FilterMagic[i][1])
					return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL KPlayer::InventoryItem()
{
	int nHand = Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
	BOOL m_bSaveTrash = FALSE;	//gi÷ l¹i trang bÞ gi¸ >= m_nSaveTrashPrice
	int m_nSaveTrashPrice = 10000;
	if (nHand)
	{
		POINT pPos;
		for(int nRoom = room_repository; nRoom <= room_repository+Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExItemId && nRoom <= room_exbox3; nRoom++)
		{
			if(Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Room[nRoom].FindRoom(Item[nHand].GetWidth(), Item[nHand].GetHeight(), &pPos))
			{
				ItemPos	P;
				P.nPlace = pos_repositoryroom+nRoom-room_repository;
				P.nX = pPos.x;
				P.nY = pPos.y;
				Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
				return FALSE;
			}
		}
		return TRUE;
	}
	else
	{
		PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
		
		if (pItem)
		{
			if (pItem->nPlace == pos_equiproom || pItem->nPlace == pos_equiproomex)
			{
				BOOL bMatch = FALSE;
				if (Item[pItem->nIdx].m_CommonAttrib.nItemNature)
					bMatch = TRUE;

				//else if(Item[pItem->nIdx].m_CommonAttrib.nItemGenre == item_task)
				//	bMatch = TRUE;

				else if(Item[pItem->nIdx].GetGoldId())
					bMatch = TRUE;

				else if (m_bSaveTrash && Item[pItem->nIdx].GetSalePrice() >= m_nSaveTrashPrice)
					bMatch = TRUE;
				else if (m_bSaveJewelry && CheckEquip(Item[pItem->nIdx].GetDetailType()))//gi÷ trang søc
					bMatch = TRUE;
				else
				{
					if (CheckEquipMagic(pItem->nIdx))
						bMatch = TRUE;
				}
				if (bMatch)
				{
					POINT pPos;
					for(int nRoom = room_repository; nRoom <= room_repository+Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExItemId && nRoom <= room_exbox3; nRoom++)
					{
						if(Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Room[nRoom].FindRoom(Item[pItem->nIdx].GetWidth(), Item[pItem->nIdx].GetHeight(), &pPos))
						{
							ItemPos	P;
							P.nPlace = pItem->nPlace;
							P.nX = pItem->nX;
							P.nY = pItem->nY;
							Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
							return FALSE;
						}
					}
					return TRUE;
				}
			}
		}

		while(pItem) 
		{ 
			pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
			if (pItem)
			{
				if (pItem->nPlace == pos_equiproom)
				{
					BOOL bMatch = FALSE;
					if (Item[pItem->nIdx].m_CommonAttrib.nItemNature)
						bMatch = TRUE;
					
					else if(Item[pItem->nIdx].GetGoldId())
							bMatch = TRUE;

					//else if(Item[pItem->nIdx].m_CommonAttrib.nItemGenre == item_task)
					//	bMatch = TRUE;

					else if (m_bSaveTrash && Item[pItem->nIdx].GetSalePrice() >= m_nSaveTrashPrice)
						bMatch = TRUE;
					else if (m_bSaveJewelry && CheckEquip(Item[pItem->nIdx].GetDetailType()))//gi÷ trang søc
						bMatch = TRUE;
					else
					{
						if (CheckEquipMagic(pItem->nIdx))
							bMatch = TRUE;
					}
					if (bMatch)
					{
						POINT pPos;
						for(int nRoom = room_repository; nRoom <= room_repository+Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExItemId && nRoom <= room_exbox3; nRoom++)
						{
							if(Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Room[nRoom].FindRoom(Item[pItem->nIdx].GetWidth(), Item[pItem->nIdx].GetHeight(), &pPos))
							{
								ItemPos	P;
								P.nPlace = pItem->nPlace;
								P.nX = pItem->nX;
								P.nY = pItem->nY;
								Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
								return FALSE;
							}
						}
						return TRUE;
					}
				}
			}
		}
	}
	return TRUE;
}

BOOL KPlayer::AutoSellItem()
{
	PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
	if (pItem)
	{
		int nIdx = pItem->nIdx;
		if (pItem->nPlace == pos_equiproom && nIdx > 0 && nIdx < MAX_ITEM)
		{
			if ( //Vat pham Item khong giao dich
				CheckEquipMagic(nIdx) ||
				Item[pItem->nIdx].GetGenre() == item_task ||
				Item[pItem->nIdx].GetGenre() == item_medicine ||
				Item[pItem->nIdx].GetGenre() == item_mine ||
				Item[pItem->nIdx].GetGenre() == item_materials ||
				Item[pItem->nIdx].GetGenre() == item_townportal ||
				Item[pItem->nIdx].GetGenre() == item_magicscript ||
				Item[pItem->nIdx].GetGoldId() 
				|| Item[nIdx].GetPlayerItemLock() > 0 
				|| Item[nIdx].GetPlayerItemHLock() > 0
				|| Item[nIdx].GetPlayerItemLock() == -2
				)
				{

				}
			else
			{
				SendClientCmdAutoSell(Item[nIdx].GetID());
			}
		}
	}
	while(pItem) 
	{ 
		pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
		if (pItem)
		{
			int nIdx = pItem->nIdx;
			if (pItem->nPlace == pos_equiproom && nIdx > 0 && nIdx < MAX_ITEM)
			{
				if (
					//Vat pham Item khong giao dich
					CheckEquipMagic(nIdx) ||
					Item[pItem->nIdx].GetGenre() == item_task ||
					Item[pItem->nIdx].GetGenre() == item_medicine ||
					Item[pItem->nIdx].GetGenre() == item_mine ||
					Item[pItem->nIdx].GetGenre() == item_materials ||
					Item[pItem->nIdx].GetGenre() == item_townportal ||
					Item[pItem->nIdx].GetGenre() == item_magicscript ||
					Item[pItem->nIdx].GetGoldId() 
					|| Item[nIdx].GetPlayerItemLock() > 0 
					|| Item[nIdx].GetPlayerItemHLock() > 0
					|| Item[nIdx].GetPlayerItemLock() == -2
					)
					{

					}
				else
				{
					SendClientCmdAutoSell(Item[nIdx].GetID());
				}
			}
		}
	}
	return TRUE;
}

BOOL KPlayer::ReturnFromPortal()
{
		KSystemMessage	sMsg;
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;

		BOOL nRet = TRUE;
		switch(m_nReturnPortalStep)
		{
		case Step_Training:
			{
				if (IR_GetCurrentTime() - m_ReturnSetpCountDown > def_RETURN_FROM_PORTAL_STEP)
				{
					m_ReturnSetpCountDown = IR_GetCurrentTime();
					sprintf(sMsg.szMessage, "->Thao t¸c khi trong thµnh vui lßng ®îi");
					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetChatInfo("Auto", sMsg.szMessage, strlen(sMsg.szMessage));
					m_nReturnPortalStep = Step_Repair;
					break;
				}
			}
		case Step_Repair:
			{
				if (IR_GetCurrentTime() - m_ReturnSetpCountDown > def_RETURN_FROM_PORTAL_STEP)
				{
					m_ReturnSetpCountDown = IR_GetCurrentTime();
					sprintf(sMsg.szMessage, "->Mua m¸u mana tdp, söa, b¸n vËt phÈm");
					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetChatInfo("Auto", sMsg.szMessage, strlen(sMsg.szMessage));
					if (m_bRepairEquip)
					{
						strcpy(sMsg.szMessage, "->.Rót tiÒn, söa trang bÞ háng");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

						if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < 200000)
							Player[CLIENT_PLAYER_INDEX].m_ItemList.ExchangeMoney(room_repository,room_equipment, 200000);
						for(int i = itempart_head; i < itempart_horse; i++)
						{
							int nIdx = Player[CLIENT_PLAYER_INDEX].m_ItemList.m_EquipItem[i];
							if(nIdx && Item[nIdx].GetRepairPrice())
							{
								if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < Item[nIdx].GetRepairPrice())
									continue;
								SendClientCmdRepair(Item[nIdx].GetID());
							}
						}
					}
					if(m_bBuyEquip)
					{
						strcpy(sMsg.szMessage, "->.Rót tiÒn, mua m¸u, mana, thæ ®Þa phï.");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

						if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < 200000)
							Player[CLIENT_PLAYER_INDEX].m_ItemList.ExchangeMoney(room_repository,room_equipment, 200000);
						BYTE itemmauid = 0;
						BYTE itemmanaid = 1;
						BYTE	nLevel = 4;
						BYTE num = 10;
						if(m_ItemList.CountCommonItem(0, item_medicine, itemmauid) <= num)
							SendClientCmdOpenShop(item_medicine, itemmauid, nLevel, num - m_ItemList.CountCommonItem(0, item_medicine, itemmauid));
						if(m_ItemList.CountCommonItem(0, item_medicine, itemmanaid) <= num)
							SendClientCmdOpenShop(item_medicine, itemmanaid, nLevel, num - m_ItemList.CountCommonItem(0, item_medicine, itemmanaid));
						if(m_ItemList.CountCommonItem(0, item_townportal, 0) <= 2)
							SendClientCmdOpenShop(item_townportal, 0, 0, 2 - m_ItemList.CountCommonItem(0, item_townportal, 0)); //tho dia phu
					}

					if(m_Auto_BanItem)//tu dong ban vat pham
					{
						strcpy(sMsg.szMessage, "->.B¸n vËt phÈm kh«ng cã trong läc thuéc tÝnh");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						AutoSellItem();
					}
					
					m_nReturnPortalStep = Step_InventoryMoney;
				}
				break;
			}
		case Step_InventoryMoney:
			{
				if (IR_GetCurrentTime() - m_ReturnSetpCountDown > def_RETURN_FROM_PORTAL_STEP)
				{
					m_ReturnSetpCountDown = IR_GetCurrentTime();
					if(m_bInventoryMoney)
					{
						sprintf(sMsg.szMessage, "->CÊt tiÒn vµo r­¬ng");
						Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetChatInfo("Auto", sMsg.szMessage, strlen(sMsg.szMessage));
						Player[CLIENT_PLAYER_INDEX].m_ItemList.ExchangeMoney(room_equipment, room_repository, Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney());
					}
					m_nReturnPortalStep = Step_InventoryItem;
				}
			}
			break;
		case Step_InventoryItem:
			{
				int m_nReturnPortalSec=15;
				sprintf(sMsg.szMessage, "->CÊt ®å vµ trë l¹i b·i sau %d gi©y", m_nReturnPortalSec);
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetChatInfo("Auto", sMsg.szMessage, strlen(sMsg.szMessage));
				if(m_bInventoryItem)
				{
					if(InventoryItem())//cat do vao ruong
					{
						m_nCurReturnPortalSec = KSG_GetCurSec() + m_nReturnPortalSec;
						m_nReturnPortalStep = Step_Return;
					}
				}
				else
				{
					m_nCurReturnPortalSec = KSG_GetCurSec() + m_nReturnPortalSec;
					m_nReturnPortalStep = Step_Return;
				}
			}
			break;
		case Step_Return:
			if(m_bReturnPortal)
			{
				if(KSG_GetCurSec() >= m_nCurReturnPortalSec)
				{
					sprintf(sMsg.szMessage, "->Quay l¹i ®Þa ®iÓm cò");
					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetChatInfo("Auto", sMsg.szMessage, strlen(sMsg.szMessage));

					PLAYER_COMMAND	sRP;
					sRP.ProtocolType = c2s_playercommand;
					sRP.m_wMsgID = enumC2S_PLAYERCOMMAND_ID_RETURN;
					sRP.m_lpBuf = 0;
					sRP.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
					g_pClient->SendPackToServer((BYTE*)&sRP, sRP.m_wLength + 1);
					nRet = FALSE;
					m_nReturnPortalStep = Step_Completed;
					m_ReturnSetpCountDown = IR_GetCurrentTime();
				}
			}
			break;
		case Step_Completed:
			{
				if (IR_GetCurrentTime() - m_ReturnSetpCountDown > def_RETURN_FROM_PORTAL_STEP)
				{
					m_ReturnSetpCountDown = IR_GetCurrentTime();
					if(!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode)//sau khi hoµn thµnh mµ kh«ng chuyÓn tr¹ng th¸i chiÕn ®Êu th× ko quay l¹i dc ®iÓm cò
					{
						sprintf(sMsg.szMessage, "->Kh«ng x¸c ®Þnh ®iÓm cò");
						Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetChatInfo("Auto", sMsg.szMessage, strlen(sMsg.szMessage));
					}
				}
				break;
			}
		}
		return nRet;
}

void KPlayer::AutoReturn()
{
	if (m_bAttackAround)
	{
		int nMapX, nMapY, dX, dY, dZ;
		Npc[m_nIndex].GetMpsPos(&nMapX, &nMapY);
		dX = nMapX-m_PosXAuto;
		dY = nMapY-m_PosYAuto;
		dZ = (int)sqrt((float)dX*dX+(float)dY*dY);
		if (dZ>=64)
		{
			MoveTo(m_PosXAuto, m_PosYAuto);
		}
		else
		{
			if (Npc[m_nIndex].m_Doing != do_sit)
			{
				if(!Npc[m_nIndex].m_bRideHorse)
				{
					Npc[m_nIndex].SendCommand(do_sit);
					SendClientCmdSit(TRUE);
				}
				else
					return;
			}
		}
	}
	else if(m_AutoMove) //#Fix loi Auto chay mot luc dung yen
	{
		PlayerMoveMps();
		return;
	}
	else
	{
		/*if (m_bFollowPeople == FALSE || m_FollowPeopleName[0] == 0) 
		{
			if (Npc[m_nIndex].m_Doing != do_sit)
			{
				if(!Npc[m_nIndex].m_bRideHorse)
				{
					Npc[m_nIndex].SendCommand(do_sit);
					SendClientCmdSit(TRUE);
				}
			}
		}*/
	}

}

void KPlayer::MoveTo(int nX, int nY)
{
	int nPlayerX, nPlayerY;
	int nDestX, nDestY, dX, dY;
	AUTOLOG_EVERY(500, "MOVE-GATE tox=%d toy=%d frames=%d need=%d run=%d sent=%d", nX, nY, m_nSendMoveFrames, defMAX_PLAYER_SEND_MOVE_FRAME, m_RunStatus, (int)(m_nSendMoveFrames >= defMAX_PLAYER_SEND_MOVE_FRAME));
	if (m_nSendMoveFrames >= defMAX_PLAYER_SEND_MOVE_FRAME)
	{
		Npc[m_nIndex].GetMpsPos(&nPlayerX, &nPlayerY);
		dX = nPlayerX & 0x1F;
		dY = nPlayerY & 0x1F;
		nDestX = nX + 0x10 - dX; // Anti-LAG
		nDestY = nY + 0x10 - dY; // Anti-LAG
		nDestX = nX;
		nDestY = nY;
		if (!m_RunStatus)
		{
			Npc[m_nIndex].SendCommand(do_walk, nDestX, nDestY);
			SendClientCmdWalk(nX, nY);
		}
		else
		{
			Npc[m_nIndex].SendCommand(do_run, nDestX, nDestY);
			SendClientCmdRun(nX, nY);
		}
		m_nSendMoveFrames = 0;
	}
}

BOOL KPlayer::AutoBuffSkillState()
{
	BOOL _break = FALSE;

	for (int i = 0; i < MAX_ARRAY_STATESKILL; i ++)
	{
		if (m_ArrayStateSkill[i] && m_bBuffSkill[i])
		{
			ISkill * pSkill = g_SkillManager.GetSkill(m_ArrayStateSkill[i], 1);

			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.CanCast(m_ArrayStateSkill[i], 
				SubWorld[0].m_dwCurrentTime) && 
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Cost(pSkill->GetSkillCostType(), pSkill->GetSkillCost(this), TRUE) && 
				!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].FindStateSkill(m_ArrayStateSkill[i]))
			{
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_skill, m_ArrayStateSkill[i], -1, Player[CLIENT_PLAYER_INDEX].m_nIndex);
				SendClientCmdSkill(m_ArrayStateSkill[i], -1, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID);
				AUTOLOG("BUFF-CAST slot=%d skill=%d self=%u mana=%d t=%u", i, m_ArrayStateSkill[i], Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana, GetTickCount());
				return TRUE;
			}
		}
	}

	if (m_AutoLifeReplenish)//nga my auto buff m¸u
	{
		int nSkillLifeReplenishID = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSkillLifeReplenish();
		ISkill * pSkill = g_SkillManager.GetSkill(nSkillLifeReplenishID, 1);
		if (nSkillLifeReplenishID && Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Cost(pSkill->GetSkillCostType(), pSkill->GetSkillCost(this), TRUE))
		{
			int pLifePlayer = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife * MAX_PERCENT / Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLifeMax;
			if (pLifePlayer < m_AutoLifeReplenishP)
			{
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_skill, nSkillLifeReplenishID, -1, m_nPlayerIndex);
				AUTOLOG("BUFF-LIFE skill=%d plife=%d thres=%d t=%u", nSkillLifeReplenishID, pLifePlayer, m_AutoLifeReplenishP, GetTickCount());
				SendClientCmdSkill(nSkillLifeReplenishID, -1, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID);
				_break = TRUE;
			}
			if(m_AutoBuffTeam == TRUE)//nga my buff mau cho to doi
			{
				int nghkTeamCount = 8;
				KUiPlayerItem*	m_pPlayersList = (KUiPlayerItem*)malloc(sizeof(KUiPlayerItem) * nghkTeamCount);
				int nghkRet = g_Team[0].GetMemberInfo(m_pPlayersList, nghkTeamCount);
				for(int m = 0; m < nghkRet; m++)//add by phong kiÒu nga my buff m¸u cho tæ ®éi
				{
					if(m_pPlayersList[m].nPercenLife < m_AutoLifeReplenishP)
					{
						Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_skill, nSkillLifeReplenishID, -1, m_nPlayerIndex);
						SendClientCmdSkill(nSkillLifeReplenishID, -1, m_pPlayersList[m].uId);							
					}
				}
			}
		}
	}
	return _break;
}

void KPlayer::PlayerFollowObject(int nObject)
{
	if (IsNotValidObject(nObject))
	{
		m_nObject = 0;
		return;
	}
		
	int nX1, nY1, nX2, nY2;
	Npc[m_nIndex].GetMpsPos(&nX1, &nY1);
	Object[nObject].GetMpsPos(&nX2, &nY2);
	AUTOLOG_EVERY(500, "PICK-DIST obj=%d oid=%d kind=%d px=%d py=%d ox=%d oy=%d need=%d timelag=%d", nObject, Object[nObject].m_nID, Object[nObject].m_nKind, nX1, nY1, nX2, nY2, PLAYER_PICKUP_CLIENT_DISTANCE, m_nTimeRunLag);

	if ((nX1 - nX2) * (nX1 - nX2) + (nY1 - nY2) * (nY1 - nY2) < PLAYER_PICKUP_CLIENT_DISTANCE * PLAYER_PICKUP_CLIENT_DISTANCE)
	{
		CheckObject(nObject);
		m_bObject = FALSE;
		m_nObject = 0;
		ClearArrayObjectLag();
	}
	else
	{
		MoveTo(nX2, nY2);
		m_nTimeRunLag++;
		if (m_nTimeRunLag >= AUTO_TIME_LAG)
		{
			BOOL _flag = FALSE;
			int i = 0;
			for (i=0; i < MAX_ARRAY_NPC_AUTO; i++)
			{
				if (Object[nObject].m_nID == m_ArrayObjectLag[i])
				{
					nObject = 0;
					m_bObject = FALSE;
					m_nObject = 0;
					_flag = TRUE;
					break;
				}
			}
			if (_flag)
				return;
			for (i = 0; i < MAX_ARRAY_NPC_AUTO; i++)
			{
				if (m_ArrayObjectLag[i] == 0)
				{
					AUTOLOG("PICK-GIVEUP obj=%d oid=%d slot=%d timelag=%d limit=%d t=%u", nObject, Object[nObject].m_nID, i, m_nTimeRunLag, AUTO_TIME_LAG, GetTickCount());
					m_ArrayObjectLag[i] = Object[nObject].m_nID;
					m_ArrayTimeObjectLag[i] = GetTickCount();
					m_bObject = FALSE;
					m_nObject = 0;
					nObject = 0;
					break;
				}
			}
		}
	}
}

void KPlayer::AutoParty()
{
	int nCount = NpcSet.GetAroundPlayerForTeamInvite(NULL, 0);
	KUiPlayerItem*	m_pNearbyPlayersList = (KUiPlayerItem*)malloc(sizeof(KUiPlayerItem) * nCount);
	NpcSet.GetAroundPlayerForTeamInvite(m_pNearbyPlayersList, nCount);
	for (int i = 0; i < nCount; i++)
	{
		if(m_bAutoAccecptAll)//tù ®éng mêi tÊt c¶
			TeamInviteAdd(m_pNearbyPlayersList[i].uId);
		else
		{
			for(int j = 0; j < defMAX_AUTO_MOVEMPSL; j++) //add by phong kiÒu qu¶n lý tæ ®éi
			{
				if(m_AutoPT_PlayerList[j][0] && strcmp(m_pNearbyPlayersList[i].Name,m_AutoPT_PlayerList[j]) ==0)//chØ mêi nh÷ng thµnh viªn cã tªn trong ds
					TeamInviteAdd(m_pNearbyPlayersList[i].uId);
			}
		}
	}
}

void KPlayer::PlayerEatAItem()
{
	if(IR_GetCurrentTime() - m_CheckTSCountDown > defMAX_CHECK_TSNAMETIME)
	{
		m_CheckTSCountDown = IR_GetCurrentTime();
	}
	if (m_EatLife && Npc[m_nIndex].m_CurrentLife <= m_LifeAutoV)
	{
		if (IR_GetCurrentTime() - m_LifeCountDown > m_LifeTimeUse)
		{
			PlayerUseItem(ntypemau);//su dung mau
			m_LifeCountDown = IR_GetCurrentTime();
		}
	}
	if (m_EatMana && Npc[m_nIndex].m_CurrentMana <= m_ManaAutoV)
	{
		if (IR_GetCurrentTime() - m_ManaCountDown > m_ManaTimeUse)
		{
			PlayerUseItem(ntypemana);//su dung mana
			m_ManaCountDown = IR_GetCurrentTime();
		}
	}

	if (Npc[m_nIndex].m_FightMode)
	{
		if( m_Auto_TuiDuocPham && m_ItemList.CountCommonItem(0, item_medicine, 2) <= 5 )//2 vua mau va mana
		{
			if (IR_GetCurrentTime() - m_TuiDPhamCountDown > defMAX_EAT_SPACETIME)
			{
				PlayerUseItem(ntypetuiduocpham);
				m_TuiDPhamCountDown = IR_GetCurrentTime();
			}
		}
		bool bTPFlag = false;
		if ((m_TPLife && Npc[m_nIndex].m_CurrentLife <= m_TPLifeV) || 
			(m_TPMana && Npc[m_nIndex].m_CurrentMana <= m_TPManaV) ||
			(m_TPNotMedicineBlood && m_ItemList.CountCommonItem(0, item_medicine, 0) <= 0 && m_ItemList.CountCommonItem(0, item_medicine, 2) <= 0) || // 0 mau
			(m_TPNotMedicineMana && m_ItemList.CountCommonItem(0, item_medicine, 1) <= 0 && m_ItemList.CountCommonItem(0, item_medicine, 2) <= 0) ||  // 1 mana
			(m_TPHightMoney && m_ItemList.GetEquipmentMoney() > 10000 && m_ItemList.GetEquipmentMoney() / 10000 > m_TPHightMoneyV) || 
			(m_btDurabilityItem && m_ItemList.HaveDamageItem(m_btDurabilityValue))
			)
		{
			bTPFlag = true;
		}
		if (bTPFlag && IR_GetCurrentTime() - m_PortalCountDown > defMAX_EAT_SPACETIME)
		{
			PlayerUseItem(ntypethodiaphu);//tho dia phu
			m_PortalCountDown = IR_GetCurrentTime();
		}
	}
	if (m_AutoAntiPoison && Npc[m_nIndex].m_PoisonState.nTime)
	{
		if (IR_GetCurrentTime() - m_AntiPoisonCountDown > defMAX_EAT_SPACETIME)
		{
			PlayerUseItem(ntypegiaidoc);//su dung giai doc
			m_AntiPoisonCountDown = IR_GetCurrentTime();
		}
	}
	if (m_AutoTTL && m_cTask.GetSaveVal(TASKVALUE_X2_EXP) <= 0)		// can than bug khi xai bo hong anh max //#time x2 Exp chÕt kh«ng mÊt
	{
		if (IR_GetCurrentTime() - m_TTLCountDown > defMAX_EAT_SPACETIME)
		{
			PlayerUseItem(ntypettl);//su dung tien thao lo
			m_TTLCountDown = IR_GetCurrentTime();
		}
	}
}

int KPlayer::SearchItemPosByItemIdx(int sIdx)
{
	PlayerItem* pItem = m_ItemList.GetFirstItem();
	if (pItem)
	{
		if(pItem->nIdx == sIdx)
			return pItem->nPlace;
	}
	while(pItem) 
	{ 
		pItem = m_ItemList.GetNextItem();
		if (pItem)
		{
			if(pItem->nIdx == sIdx)
				return pItem->nPlace;
		}
	}
	return 0;
}

void KPlayer::PlayerUseItem(int type)
{
	ItemPos    Pos;
	PlayerItem* pItem = m_ItemList.GetFirstItem();
	if (pItem)
	{
		if (pItem->nPlace == pos_equiproom || pItem->nPlace == pos_immediacy)
		{
			switch(type)
			{
			case ntypemau:
				if (Item[pItem->nIdx].GetGenre() == item_medicine)
				{
					for (int i = 0; i < 6; i++)
					{
						if (Item[pItem->nIdx].GetBaseMagicAttribType(i) == magic_lifepotion_v)
						{
							Pos.nPlace = pItem->nPlace;
							Pos.nX = pItem->nX;
							Pos.nY = pItem->nY;
							ApplyUseItem(pItem->nIdx, Pos);
							return;
						}
					}
				}
				break;
			case ntypemana:
				if (Item[pItem->nIdx].GetGenre() == item_medicine)
				{
					for (int i = 0; i < 6; i++)
					{
						if (Item[pItem->nIdx].GetBaseMagicAttribType(i) == magic_manapotion_v)
						{
							Pos.nPlace = pItem->nPlace;
							Pos.nX = pItem->nX;
							Pos.nY = pItem->nY;
							ApplyUseItem(pItem->nIdx, Pos);
							return;
						}
					}
				}
				break;
			case ntypethodiaphu:
				if (Item[pItem->nIdx].GetGenre() == item_townportal)
				{
					if (Item[pItem->nIdx].GetDetailType() == 0 || Item[pItem->nIdx].GetDetailType() == 1)
					{
						//viet them
						m_Actacker = 0;
						m_bActacker = FALSE;
						m_nTimeRunLag = 0;
						m_Count_Acttack_Lag = 0;

						Pos.nPlace = pItem->nPlace;
						Pos.nX = pItem->nX;
						Pos.nY = pItem->nY;
						ApplyUseItem(pItem->nIdx, Pos);
						return;
					}
				}
				break;
			case ntypegiaidoc:
				if (Item[pItem->nIdx].GetGenre() == item_medicine)
				{
					for (int i = 0; i < 6; i++)
					{
						if (Item[pItem->nIdx].GetBaseMagicAttribType(i) == magic_poisondamagereduce_v)
						{
							Pos.nPlace = pItem->nPlace;
							Pos.nX = pItem->nX;
							Pos.nY = pItem->nY;
							ApplyUseItem(pItem->nIdx, Pos);
							return;
						}
					}
				}
				break;
			case ntypettl:
				if (Item[pItem->nIdx].GetGenre() == item_magicscript)
				{
					for(int i=0; i < sizeof(szMagicScriptName)/sizeof(szMagicScriptName[0]);i++)
					{
						if (strcmp(Item[pItem->nIdx].GetName(), szMagicScriptName[i]) == 0)
						{
							Pos.nPlace = pItem->nPlace;
							Pos.nX = pItem->nX;
							Pos.nY = pItem->nY;
							ApplyUseItem(pItem->nIdx, Pos);
							return;
						}
					}
				}
				break;
			case ntypetuiduocpham:
				if (Item[pItem->nIdx].GetGenre() == item_task)
				{
					if (strcmp(Item[pItem->nIdx].GetName(), szTuiDPhamName) == 0)
					{
						Pos.nPlace = pItem->nPlace;
						Pos.nX = pItem->nX;
						Pos.nY = pItem->nY;
						ApplyUseItem(pItem->nIdx, Pos);
						return;
					}
				}
				break;
			default:
				break;
			}
		}
	}
	
	while(pItem) 
	{ 
		pItem = m_ItemList.GetNextItem();
		if (pItem)
		{
			if (pItem->nPlace == pos_equiproom || pItem->nPlace == pos_immediacy)
			{
				
			switch(type)
			{
			case ntypemau:
				if (Item[pItem->nIdx].GetGenre() == item_medicine)
				{
					for (int i = 0; i < 6; i++)
					{
						if (Item[pItem->nIdx].GetBaseMagicAttribType(i) == magic_lifepotion_v)
						{
							Pos.nPlace = pItem->nPlace;
							Pos.nX = pItem->nX;
							Pos.nY = pItem->nY;
							ApplyUseItem(pItem->nIdx, Pos);
							return;
						}
					}
				}
				break;
			case ntypemana:
				if (Item[pItem->nIdx].GetGenre() == item_medicine)
				{
					for (int i = 0; i < 6; i++)
					{
						if (Item[pItem->nIdx].GetBaseMagicAttribType(i) == magic_manapotion_v)
						{
							Pos.nPlace = pItem->nPlace;
							Pos.nX = pItem->nX;
							Pos.nY = pItem->nY;
							ApplyUseItem(pItem->nIdx, Pos);
							return;
						}
					}
				}
				break;
			case ntypethodiaphu:
				if (Item[pItem->nIdx].GetGenre() == item_townportal)
				{
					if (Item[pItem->nIdx].GetDetailType() == 0 || Item[pItem->nIdx].GetDetailType() == 1)
					{
						//viet them
						m_Actacker = 0;
						m_bActacker = FALSE;
						m_nTimeRunLag = 0;
						m_Count_Acttack_Lag = 0;

						Pos.nPlace = pItem->nPlace;
						Pos.nX = pItem->nX;
						Pos.nY = pItem->nY;
						ApplyUseItem(pItem->nIdx, Pos);
						return;
					}
				}
				break;
			case ntypegiaidoc:
				if (Item[pItem->nIdx].GetGenre() == item_medicine)
				{
					for (int i = 0; i < 6; i++)
					{
						if (Item[pItem->nIdx].GetBaseMagicAttribType(i) == magic_poisondamagereduce_v)
						{
							Pos.nPlace = pItem->nPlace;
							Pos.nX = pItem->nX;
							Pos.nY = pItem->nY;
							ApplyUseItem(pItem->nIdx, Pos);
							return;
						}
					}
				}
				break;
			case ntypettl:
				if (Item[pItem->nIdx].GetGenre() == item_task)
				{
					for(int i=0; i < sizeof(szMagicScriptName)/sizeof(szMagicScriptName[0]);i++)
					{
						if (strcmp(Item[pItem->nIdx].GetName(), szMagicScriptName[i]) == 0)
						{
							Pos.nPlace = pItem->nPlace;
							Pos.nX = pItem->nX;
							Pos.nY = pItem->nY;
							ApplyUseItem(pItem->nIdx, Pos);
							return;
						}
					}
				}
				break;
			case ntypetuiduocpham:
				if (Item[pItem->nIdx].GetGenre() == item_magicscript)
				{
					if (strcmp(Item[pItem->nIdx].GetName(), szTuiDPhamName) == 0)
					{
						Pos.nPlace = pItem->nPlace;
						Pos.nX = pItem->nX;
						Pos.nY = pItem->nY;
						ApplyUseItem(pItem->nIdx, Pos);
						return;
					}
				}
				break;
			default:
				break;
			}
			}
		}
	}
}
#endif

#ifdef _SERVER
void	KPlayer::SetBaseStrength(int nData)
{
	m_nStrength += nData;
	//m_nAttributePoint -= nData;
	m_nCurStrength += nData;
	
	UpdataCurData();
	SetNpcPhysicsDamage();
	// °Ñµ±Ç°Á¦Á¿´«¸ø¿Í»§¶Ë
	PLAYER_ATTRIBUTE_SYNC	sSync;
	sSync.ProtocolType = s2c_playersyncattribute;
	sSync.m_btAttribute = 0;
	sSync.m_nBasePoint = m_nStrength;
	sSync.m_nCurPoint = m_nCurStrength;
	sSync.m_nLeavePoint = m_nAttributePoint;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));
}
#endif

#ifdef _SERVER
void	KPlayer::SetBaseDexterity(int nData)
{
	m_nDexterity += nData;
	//m_nAttributePoint -= nData;
	m_nCurDexterity += nData;
	
	SetNpcAttackRating();
	SetNpcDefence();
	UpdataCurData();
	SetNpcPhysicsDamage();
	
	PLAYER_ATTRIBUTE_SYNC	sSync;
	sSync.ProtocolType = s2c_playersyncattribute;
	sSync.m_btAttribute = 1;
	sSync.m_nBasePoint = m_nDexterity;
	sSync.m_nCurPoint = m_nCurDexterity;
	sSync.m_nLeavePoint = m_nAttributePoint;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));
}
#endif

#ifdef _SERVER
void	KPlayer::SetBaseVitality(int nData)
{
	m_nVitality += nData;
	//m_nAttributePoint -= nData;
	m_nCurVitality += nData;
	
	Npc[m_nIndex].AddBaseLifeMax(PlayerSet.m_cLevelAdd.GetLifePerVitality(Npc[m_nIndex].m_Series) * nData);
	Npc[m_nIndex].AddBaseStaminaMax(PlayerSet.m_cLevelAdd.GetStaminaPerVitality(Npc[m_nIndex].m_Series) * nData);
	UpdataCurData();

	PLAYER_ATTRIBUTE_SYNC	sSync;
	sSync.ProtocolType = s2c_playersyncattribute;
	sSync.m_btAttribute = 2;
	sSync.m_nBasePoint = m_nVitality;
	sSync.m_nCurPoint = m_nCurVitality;
	sSync.m_nLeavePoint = m_nAttributePoint;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));
}
#endif

#ifdef _SERVER
void	KPlayer::SetBaseEngergy(int nData)
{
	m_nEngergy += nData;
	//m_nAttributePoint -= nData;
	m_nCurEngergy += nData;
	
	Npc[m_nIndex].AddBaseManaMax(PlayerSet.m_cLevelAdd.GetManaPerEnergy(Npc[m_nIndex].m_Series) * nData);
	UpdataCurData();
	
	PLAYER_ATTRIBUTE_SYNC	sSync;
	sSync.ProtocolType = s2c_playersyncattribute;
	sSync.m_btAttribute = 3;
	sSync.m_nBasePoint = m_nEngergy;
	sSync.m_nCurPoint = m_nCurEngergy;
	sSync.m_nLeavePoint = m_nAttributePoint;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));
}
#endif