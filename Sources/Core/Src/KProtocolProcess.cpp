#include "KEngine.h"
#include "KCore.h"
#ifndef _SERVER
#include "../../Headers/IClient.h"
#include "CoreShell.h"
#include "KViewItem.h"
#include "KSellItem.h"
#endif
#include "KObjSet.h"
#include "KNpcSet.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KPlayerTeam.h"
#include "KPlayerFaction.h"
#include "KNpc.h"
#include "KSubWorld.h"
#include "LuaFuns.h"
#include "KProtocolProcess.h"
#include "KSkills.h"
#include "KThiefSkill.h"
#include "KItemSet.h"
#include "KBuySell.h"
#include "KSubWorldSet.h"
//#include "MyAssert.h"
#include "Scene/ObstacleDef.h"
#include "KMath.h"
#ifndef _SERVER
#include "Scene\KScenePlaceC.h"
#include <KJXPathFinder.h>

//extern KJXPathFinder g_JXPathFinder;
#endif
#include "KTongProtocol.h"
#include "KLadder.h"
#include "KOption.h"
#include "KDaTauCap.h"
#include <BauCua.h>
#include <iostream>


//#define WAIGUA_ZROC

KProtocolProcess g_ProtocolProcess;
KProtocolProcess::KProtocolProcess()
{
	ZeroMemory(ProcessFunc, sizeof(ProcessFunc));
#ifndef _SERVER
	ProcessFunc[s2c_login] = NULL;
	ProcessFunc[s2c_logout] = NULL;
	ProcessFunc[s2c_syncend] = &KProtocolProcess::SyncEnd;
	ProcessFunc[s2c_synccurplayer] = &KProtocolProcess::SyncCurPlayer;
	ProcessFunc[s2c_synccurplayerskill] = &KProtocolProcess::s2cSyncAllSkill;
	ProcessFunc[s2c_synccurplayernormal] = &KProtocolProcess::SyncCurNormalData;
	ProcessFunc[s2c_newplayer] = NULL;
	ProcessFunc[s2c_removeplayer] = NULL;
	ProcessFunc[s2c_syncworld] = &KProtocolProcess::SyncWorld;
	ProcessFunc[s2c_syncplayer] = &KProtocolProcess::SyncPlayer;
	ProcessFunc[s2c_syncplayermin] = &KProtocolProcess::SyncPlayerMin;
	ProcessFunc[s2c_syncnpc] = &KProtocolProcess::SyncNpc;
	ProcessFunc[s2c_syncnpcmin] = &KProtocolProcess::SyncNpcMin;
	ProcessFunc[s2c_syncnpcminplayer] = &KProtocolProcess::SyncNpcMinPlayer;
	ProcessFunc[s2c_objadd] = &KProtocolProcess::SyncObjectAdd;
	ProcessFunc[s2c_syncobjstate] = &KProtocolProcess::SyncObjectState;
	ProcessFunc[s2c_syncobjdir] = &KProtocolProcess::SyncObjectDir;
	ProcessFunc[s2c_objremove] = &KProtocolProcess::SyncObjectRemove;
	ProcessFunc[s2c_objTrapAct] = &KProtocolProcess::SyncObjectTrap;
	ProcessFunc[s2c_npcremove] = &KProtocolProcess::NetCommandRemoveNpc;
	ProcessFunc[s2c_npcwalk] = &KProtocolProcess::NetCommandWalk;
	ProcessFunc[s2c_npcrun] = &KProtocolProcess::NetCommandRun;
	ProcessFunc[s2c_npcattack] = NULL;
	ProcessFunc[s2c_npcmagic] = NULL;
	ProcessFunc[s2c_npcjump] = &KProtocolProcess::NetCommandJump;
	ProcessFunc[s2c_npctalk] = NULL;
	ProcessFunc[s2c_npchurt] = &KProtocolProcess::NetCommandHurt;
	ProcessFunc[s2c_npcdeath] = &KProtocolProcess::NetCommandDeath;
	ProcessFunc[s2c_npcchgcurcamp] = &KProtocolProcess::NetCommandChgCurCamp;
	ProcessFunc[s2c_npcchgcamp] = &KProtocolProcess::NetCommandChgCamp;
	ProcessFunc[s2c_skillcast] = &KProtocolProcess::NetCommandSkill;
	ProcessFunc[s2c_playertalk] = NULL;
	ProcessFunc[s2c_playerexp] = &KProtocolProcess::s2cPlayerExp;
	ProcessFunc[s2c_teaminfo] = &KProtocolProcess::s2cShowTeamInfo;
	ProcessFunc[s2c_teamselfinfo] = &KProtocolProcess::s2cUpdataSelfTeamInfo;
	ProcessFunc[s2c_teamapplyinfofalse] = &KProtocolProcess::s2cApplyTeamInfoFalse;
	ProcessFunc[s2c_teamcreatesuccess] = &KProtocolProcess::s2cCreateTeam;
	ProcessFunc[s2c_teamcreatefalse] = &KProtocolProcess::s2cApplyCreateTeamFalse;
	ProcessFunc[s2c_teamopenclose] = &KProtocolProcess::s2cSetTeamState;
	ProcessFunc[s2c_teamgetapply] = &KProtocolProcess::s2cApplyAddTeam;
	ProcessFunc[s2c_teamaddmember] = &KProtocolProcess::s2cTeamAddMember;
	ProcessFunc[s2c_teamleave] = &KProtocolProcess::s2cLeaveTeam;
	ProcessFunc[s2c_teamchangecaptain] = &KProtocolProcess::s2cTeamChangeCaptain;
	ProcessFunc[s2c_playerfactiondata] = &KProtocolProcess::s2cSetFactionData;
	ProcessFunc[s2c_playerleavefaction] = &KProtocolProcess::s2cLeaveFaction;
	ProcessFunc[s2c_playerfactionskilllevel] = &KProtocolProcess::s2cFactionSkillOpen;
	ProcessFunc[s2c_playersendchat] = &KProtocolProcess::s2cGetChat;
	ProcessFunc[s2c_playersyncleadexp] = &KProtocolProcess::s2cGetLeadExp;
	ProcessFunc[s2c_playerlevelup] = &KProtocolProcess::s2cLevelUp;
	ProcessFunc[s2c_teammatelevel] = &KProtocolProcess::s2cGetTeammateLevel;
	ProcessFunc[s2c_playersyncattribute] = &KProtocolProcess::s2cGetCurAttribute;
	ProcessFunc[s2c_playerskilllevel] = &KProtocolProcess::s2cGetSkillLevel;
	ProcessFunc[s2c_syncitem] = &KProtocolProcess::s2cSyncItem;
	ProcessFunc[s2c_removeitem] = &KProtocolProcess::s2cRemoveItem;
	ProcessFunc[s2c_syncmoney] = &KProtocolProcess::s2cSyncMoney;
	ProcessFunc[s2c_playermoveitem] = &KProtocolProcess::s2cMoveItem;
	ProcessFunc[s2c_playerswitchequip] = &KProtocolProcess::s2cSwitchEquip;
	ProcessFunc[s2c_scriptaction] = &KProtocolProcess::SyncScriptAction;
	ProcessFunc[s2c_chatapplyaddfriend] = &KProtocolProcess::s2cChatGetApplyAddFriend;
	ProcessFunc[s2c_chataddfriend] = &KProtocolProcess::s2cChatAddFriend;
	ProcessFunc[s2c_chatrefusefriend] = &KProtocolProcess::s2cChatBeRefusedAddFriend;
	ProcessFunc[s2c_chataddfriendfail] = &KProtocolProcess::s2cChatAddFriendFail;
	ProcessFunc[s2c_chatloginfriendnoname] = &KProtocolProcess::s2cChatLoginFriendNoName;
	ProcessFunc[s2c_chatloginfriendname] = &KProtocolProcess::s2cChatLoginFriendName;
	ProcessFunc[s2c_chatonefrienddata] = &KProtocolProcess::s2cChatAddOneFriend;
	ProcessFunc[s2c_chatfriendonline] = &KProtocolProcess::s2cChatFriendOnline;
	ProcessFunc[s2c_chatdeletefriend] = &KProtocolProcess::s2cChatDeleteFriend;
	ProcessFunc[s2c_chatfriendoffline] = &KProtocolProcess::s2cChatFriendOffLine;
	ProcessFunc[s2c_syncrolelist] = &KProtocolProcess::s2cSyncRoleList;
	ProcessFunc[s2c_tradechangestate] = &KProtocolProcess::s2cTradeChangeState;
	ProcessFunc[s2c_gamblechangestate] = &KProtocolProcess::s2cGambleChangeState;
	ProcessFunc[s2c_npcsetmenustate] = &KProtocolProcess::s2cNpcSetMenuState;
	ProcessFunc[s2c_trademoneysync] = &KProtocolProcess::s2cTradeMoneySync;
	ProcessFunc[s2c_tradedecision] = &KProtocolProcess::s2cTradeDecision;
	ProcessFunc[s2c_gamblemoneysync] = &KProtocolProcess::s2cGambleMoneySync;
	ProcessFunc[s2c_gambledecision] = &KProtocolProcess::s2cGambleDecision;
	ProcessFunc[s2c_chatscreensingleerror] = &KProtocolProcess::s2cChatScreenSingleError;
	ProcessFunc[s2c_teaminviteadd] = &KProtocolProcess::s2cTeamInviteAdd;
	ProcessFunc[s2c_tradepressoksync] = &KProtocolProcess::s2cTradePressOkSync;
	ProcessFunc[s2c_gamblepressoksync] = &KProtocolProcess::s2cGamblePressOkSync;
	ProcessFunc[s2c_ping] = &KProtocolProcess::s2cPing;
	ProcessFunc[s2c_npcsit] = &KProtocolProcess::NetCommandSit;
	ProcessFunc[s2c_opensalebox] = &KProtocolProcess::OpenSaleBox;
	ProcessFunc[s2c_castskilldirectly] = &KProtocolProcess::s2cDirectlyCastSkill;
	ProcessFunc[s2c_msgshow] = &KProtocolProcess::s2cShowMsg;
	ProcessFunc[s2c_syncstateeffect] = &KProtocolProcess::SyncStateEffect;
	ProcessFunc[s2c_openstorebox] = &KProtocolProcess::OpenStoreBox;
	ProcessFunc[s2c_playerrevive] = &KProtocolProcess::PlayerRevive;
	ProcessFunc[s2c_requestnpcfail] = &KProtocolProcess::RequestNpcFail;
	ProcessFunc[s2c_tradeapplystart] = &KProtocolProcess::s2cTradeApplyStart;
	ProcessFunc[s2c_gambleapplystart] = &KProtocolProcess::s2cGambleApplyStart;
	ProcessFunc[s2c_rolenewdelresponse] = NULL;
	ProcessFunc[s2c_ItemAutoMove] = &KProtocolProcess::s2cItemAutoMove;
	ProcessFunc[s2c_itemexchangefinish] = &KProtocolProcess::FinishedItemExchange;
	ProcessFunc[s2c_changeweather] = &KProtocolProcess::s2cChangeWeather;
	ProcessFunc[s2c_pksyncnormalflag] = &KProtocolProcess::s2cPKSyncNormalFlag;
	ProcessFunc[s2c_pksyncenmitystate] = &KProtocolProcess::s2cPKSyncEnmityState;
	ProcessFunc[s2c_pksyncexercisestate] = &KProtocolProcess::s2cPKSyncExerciseState;
	ProcessFunc[s2c_pksyncpkvalue] = &KProtocolProcess::s2cPKValueSync;
	ProcessFunc[s2c_npcsleepmode] = &KProtocolProcess::NpcSleepSync;
	ProcessFunc[s2c_viewequip] = &KProtocolProcess::s2cViewEquip;
	ProcessFunc[s2c_ladderresult] = &KProtocolProcess::LadderResult;
	ProcessFunc[s2c_ladderlist] = &KProtocolProcess::LadderList;
	ProcessFunc[s2c_tongcreate] = &KProtocolProcess::s2cTongCreate;
	ProcessFunc[s2c_replyclientping] = &KProtocolProcess::ServerReplyClientPing;
	ProcessFunc[s2c_npcgoldchange] = &KProtocolProcess::s2cNpcGoldChange;
	ProcessFunc[s2c_itemdurabilitychange] = &KProtocolProcess::ItemChangeDurability;
	ProcessFunc[s2c_openresetpass] = &KProtocolProcess::OpenResetPass;
	ProcessFunc[s2c_reputesyncreputevalue] = &KProtocolProcess::s2cReputeValueSync;
	ProcessFunc[s2c_fuyuansyncfuyuanvalue] = &KProtocolProcess::s2cFuYuanValueSync;
	ProcessFunc[s2c_rebornsyncrebornvalue] = &KProtocolProcess::s2cReBornValueSync;
	ProcessFunc[s2c_viewsellitem] = &KProtocolProcess::s2cViewSellItem;
	ProcessFunc[s2c_viewupdateitem] = &KProtocolProcess::s2cViewUpdateItem;
	ProcessFunc[s2c_playergetcount] = &KProtocolProcess::s2cGetCouunt;
	ProcessFunc[s2c_shopname] = &KProtocolProcess::s2cShopName;
	ProcessFunc[s2c_exitgame] = &KProtocolProcess::s2c_ExitGame;
	ProcessFunc[s2c_playeritemlocksync] = &KProtocolProcess::s2cPlayerItemLockSync;
	ProcessFunc[s2c_opendataubox] = &KProtocolProcess::OpenDaTauBox;
	ProcessFunc[s2c_opendatau1box] = &KProtocolProcess::OpenDaTau1Box;
	ProcessFunc[s2c_battlebox] = &KProtocolProcess::s2cBattleBox;
	ProcessFunc[s2c_syncsupershop] = &KProtocolProcess::s2cSyncSuperShop;
	ProcessFunc[s2c_extpointsync] = &KProtocolProcess::s2cExtPointValueSync; // protocol xu
	//ProcessFunc[s2c_autoplay] = &KProtocolProcess::s2cAutoPlaySync; // protocol aut
	//ProcessFunc[s2c_player_stop] = &KProtocolProcess::s2cPlayerStop;
	//ProcessFunc[s2c_pos_edition] = &KProtocolProcess::s2cPosEdition;
	//ProcessFunc[s2c_findpathsync] = &KProtocolProcess::s2cFindPathSync;
	ProcessFunc[s2c_ridesync] = &KProtocolProcess::s2cRideSync; // protocol len xuong ngua
	ProcessFunc[s2c_ignorestate] = &KProtocolProcess::IgnoreState;
	ProcessFunc[s2c_openaffairbox] = &KProtocolProcess::OpenAffairBox;
	ProcessFunc[s2c_playersync_magic_attr] = &KProtocolProcess::s2cPlayerSync_MA;
	ProcessFunc[s2c_playeritemtimesync] = &KProtocolProcess::s2cPlayerItemTimeSync;
	ProcessFunc[s2c_timebox] = &KProtocolProcess::s2cTimeBox;
	ProcessFunc[s2c_talkex] = &KProtocolProcess::s2cTalkEx;
	ProcessFunc[s2c_syncmasklock] = &KProtocolProcess::SyncMaskLock;
	ProcessFunc[s2c_taskvalue] = &KProtocolProcess::s2cTaskValueSync; // protocol task
	ProcessFunc[s2c_playermissiondata] = &KProtocolProcess::s2cSetMissionData;
	ProcessFunc[s2c_inputbox] = &KProtocolProcess::s2cInPutBox;
	ProcessFunc[s2c_setobstacle] = &KProtocolProcess::s2cSetObstacle; //#Set vÀt c∂n
	ProcessFunc[s2c_returncityowntong] = &KProtocolProcess::s2cReturnCityOwnTong;
	ProcessFunc[s2c_playerloginreplay] = &KProtocolProcess::s2cPlayerLoginReplay; //fix by phong ki“u chuy”n gs bﬁ m t skill
	ProcessFunc[s2c_openquestfinishdlg] = &KProtocolProcess::s2cOpenQuestFinishDlg;// protocol quest finish
	ProcessFunc[s2c_imagenpc] = &KProtocolProcess::s2cImageNpcSync;
	ProcessFunc[s2c_opentrembleitem] = &KProtocolProcess::s2cOpenTrembleItem;
	ProcessFunc[s2c_opencompounditem] = &KProtocolProcess::s2cOpenCompoundItem;
	ProcessFunc[s2c_syncrankdata] = &KProtocolProcess::s2cSyncRankData;
	ProcessFunc[s2c_syncrankdata2] = &KProtocolProcess::s2cSyncRankData2;
	ProcessFunc[s2c_show_damage] = &KProtocolProcess::s2cShowDamage;
	ProcessFunc[s2c_syncmagic] = &KProtocolProcess::s2cSyncMagic;
	ProcessFunc[s2c_syncmeridian] = &KProtocolProcess::s2cSyncMeridian;
	ProcessFunc[s2c_syncbaucuaresult] = &KProtocolProcess::s2cSyncBauCuaResult;
	ProcessFunc[s2c_syncbaucuainfo] = &KProtocolProcess::s2cSyncBauCuaResult;
	ProcessFunc[s2c_playersync] = &KProtocolProcess::s2cPlayerSync;
	ProcessFunc[s2c_removeallitem] = &KProtocolProcess::s2cRemoveAllItem;
	//ProcessFunc[s2c_dynamic_structure] = &KProtocolProcess::s2cDynamicStruct;
	

	ProcessFunc[s2c_extend] = &KProtocolProcess::s2cExtend;
	ProcessFunc[s2c_extendchat] = &KProtocolProcess::s2cExtendChat;
	ProcessFunc[s2c_extendfriend] = &KProtocolProcess::s2cExtendFriend;
	ProcessFunc[s2c_extendtong] = &KProtocolProcess::s2cExtendTong;

#else

	ProcessFunc[c2s_login] = NULL;
	ProcessFunc[c2s_logiclogin] = NULL;
	ProcessFunc[c2s_syncend] = NULL;
	ProcessFunc[c2s_loadplayer] = NULL;
	ProcessFunc[c2s_newplayer] = NULL;
	ProcessFunc[c2s_removeplayer] = &KProtocolProcess::RemoveRole;
	ProcessFunc[c2s_requestworld] = NULL;
	ProcessFunc[c2s_requestplayer] = NULL;
	ProcessFunc[c2s_requestnpc] = &KProtocolProcess::NpcRequestCommand;
	ProcessFunc[c2s_requestobj] = &KProtocolProcess::ObjRequestCommand;
	ProcessFunc[c2s_npcwalk] = &KProtocolProcess::NpcWalkCommand;
	ProcessFunc[c2s_npcrun] = &KProtocolProcess::NpcRunCommand;
	ProcessFunc[c2s_npcskill] = &KProtocolProcess::NpcSkillCommand;
	ProcessFunc[c2s_npcjump] = &KProtocolProcess::NpcJumpCommand;
	ProcessFunc[c2s_npctalk] = &KProtocolProcess::NpcTalkCommand;
	ProcessFunc[c2s_dynamic_structure] = &KProtocolProcess::c2sDynamicStruct;
	ProcessFunc[c2s_npcdeath] = NULL;
	ProcessFunc[c2s_playertalk] = &KProtocolProcess::PlayerTalkCommand;
	ProcessFunc[c2s_teamapplyinfo] = &KProtocolProcess::PlayerApplyTeamInfo;
	ProcessFunc[c2s_teamapplycreate] = &KProtocolProcess::PlayerApplyCreateTeam;
	ProcessFunc[c2s_teamapplyopenclose] = &KProtocolProcess::PlayerApplyTeamOpenClose;
	ProcessFunc[c2s_teamapplyadd] = &KProtocolProcess::PlayerApplyAddTeam;
	ProcessFunc[c2s_teamacceptmember] = &KProtocolProcess::PlayerAcceptTeamMember;
	ProcessFunc[c2s_teamapplyleave] = &KProtocolProcess::PlayerApplyLeaveTeam;
	ProcessFunc[c2s_teamapplykickmember] = &KProtocolProcess::PlayerApplyTeamKickMember;
	ProcessFunc[c2s_teamapplychangecaptain] = &KProtocolProcess::PlayerApplyTeamChangeCaptain;
	ProcessFunc[c2s_teamapplydismiss] = &KProtocolProcess::PlayerApplyTeamDismiss;
	ProcessFunc[c2s_playerapplysetpk] = &KProtocolProcess::PlayerApplySetPK;
	ProcessFunc[c2s_playerapplyfactiondata] = &KProtocolProcess::PlayerApplyFactionData;
	ProcessFunc[c2s_playersendchat] = &KProtocolProcess::PlayerSendChat;
	ProcessFunc[c2s_playeraddbaseattribute] = &KProtocolProcess::PlayerAddBaseAttribute;
	ProcessFunc[c2s_playerapplyaddskillpoint] = &KProtocolProcess::PlayerApplyAddSkillPoint;
	ProcessFunc[c2s_playereatitem] = &KProtocolProcess::PlayerEatItem;
	ProcessFunc[c2s_playerpickupitem] = &KProtocolProcess::PlayerPickUpItem;
	ProcessFunc[c2s_playermoveitem] = &KProtocolProcess::PlayerMoveItem;
	ProcessFunc[c2s_playersellitem] = &KProtocolProcess::PlayerSellItem;
	ProcessFunc[c2s_playerbuyitem] = &KProtocolProcess::PlayerBuyItem;
	ProcessFunc[c2s_playerthrowawayitem] = &KProtocolProcess::PlayerDropItem;
	ProcessFunc[c2s_playerselui] = &KProtocolProcess::PlayerSelUI;
	ProcessFunc[c2s_chatsetchannel] = &KProtocolProcess::ChatSetChannel;
	ProcessFunc[c2s_chatapplyaddfriend] = &KProtocolProcess::ChatApplyAddFriend;
	ProcessFunc[c2s_chataddfriend] = &KProtocolProcess::ChatAddFriend;
	ProcessFunc[c2s_chatrefusefriend] = &KProtocolProcess::ChatRefuseFriend;
	ProcessFunc[c2s_dbplayerselect] = NULL;
	ProcessFunc[c2s_chatapplyresendallfriendname] = &KProtocolProcess::ChatApplyReSendAllFriendName;
	ProcessFunc[c2s_chatapplysendonefriendname] = &KProtocolProcess::ChatApplySendOneFriendName;
	ProcessFunc[c2s_chatdeletefriend] = &KProtocolProcess::ChatDeleteFriend;
	ProcessFunc[c2s_chatredeletefriend] = &KProtocolProcess::ChatReDeleteFriend;
	ProcessFunc[c2s_tradeapplystateopen] = &KProtocolProcess::TradeApplyOpen;
	ProcessFunc[c2s_tradeapplystateclose] = &KProtocolProcess::TradeApplyClose;
	ProcessFunc[c2s_tradeapplystart] = &KProtocolProcess::TradeApplyStart;
	ProcessFunc[c2s_trademovemoney] = &KProtocolProcess::TradeMoveMoney;
	ProcessFunc[c2s_tradedecision] = &KProtocolProcess::TradeDecision;
	ProcessFunc[c2s_gambleapplystateopen] = &KProtocolProcess::GambleApplyOpen;
	ProcessFunc[c2s_gambleapplystateclose] = &KProtocolProcess::GambleApplyClose;
	ProcessFunc[c2s_gambleapplystart] = &KProtocolProcess::GambleApplyStart;
	ProcessFunc[c2s_gamblemovemoney] = &KProtocolProcess::GambleMoveMoney;
	ProcessFunc[c2s_gambledecision] = &KProtocolProcess::GambleDecision;
	ProcessFunc[c2s_dialognpc] = &KProtocolProcess::DialogNpc;
	ProcessFunc[c2s_teaminviteadd] = &KProtocolProcess::TeamInviteAdd;
	ProcessFunc[c2s_changeauraskill] = &KProtocolProcess::ChangeAuraSkill;
	ProcessFunc[c2s_teamreplyinvite] = &KProtocolProcess::TeamReplyInvite;
	ProcessFunc[c2s_ping] = NULL;//ReplyPing;
	ProcessFunc[c2s_npcsit] = &KProtocolProcess::NpcSitCommand;
	ProcessFunc[c2s_objmouseclick] = &KProtocolProcess::ObjMouseClick;
	ProcessFunc[c2s_storemoney] = &KProtocolProcess::StoreMoneyCommand;
	ProcessFunc[c2s_playerrevive] = &KProtocolProcess::NpcReviveCommand;
	ProcessFunc[c2s_tradereplystart] = &KProtocolProcess::c2sTradeReplyStart;
	ProcessFunc[c2s_gamblereplystart] = &KProtocolProcess::c2sGambleReplyStart;
	ProcessFunc[c2s_pkapplychangenormalflag] = &KProtocolProcess::c2sPKApplyChangeNormalFlag;
	ProcessFunc[c2s_pkapplyenmity] = &KProtocolProcess::c2sPKApplyEnmity;
	ProcessFunc[c2s_viewequip] = &KProtocolProcess::c2sViewEquip;
	ProcessFunc[c2s_ladderquery] = &KProtocolProcess::LadderQuery;
	ProcessFunc[c2s_repairitem] = &KProtocolProcess::ItemRepair;
	ProcessFunc[c2s_npcride] = &KProtocolProcess::NpcRideCommand;//len xuong ngua
	ProcessFunc[c2s_cpunlock] = &KProtocolProcess::NpcCPUnlockCommand;
	ProcessFunc[c2s_cplock] = &KProtocolProcess::NpcCPLockCommand;
	ProcessFunc[c2s_cpswitchequipset] = &KProtocolProcess::NpcSwitchEquipSet;
	ProcessFunc[c2s_cpchange] = &KProtocolProcess::NpcCPChangeCommand;
	ProcessFunc[c2s_cpreset] = &KProtocolProcess::NpcCPResetCommand;
	ProcessFunc[c2s_playersetprice] = &KProtocolProcess::SetPrice;
	ProcessFunc[c2s_playerstarttrade] = &KProtocolProcess::StartTrade;
	ProcessFunc[c2s_viewitem] = &KProtocolProcess::c2sViewItem;
	ProcessFunc[c2s_playertradebuyitem] = &KProtocolProcess::c2sTradeBuy;
	ProcessFunc[c2s_updateviewitem] = &KProtocolProcess::c2supdateitem;
	ProcessFunc[c2s_playerneedcount] = &KProtocolProcess::c2sNeedCount;
	ProcessFunc[c2s_playershopname] = &KProtocolProcess::c2sNeedShopName;
	ProcessFunc[c2s_playersysshop] = &KProtocolProcess::c2sSysShop;
	ProcessFunc[c2s_playerlixian] = &KProtocolProcess::c2sLiXian;
	ProcessFunc[c2s_pkvalue] = &KProtocolProcess::NpcPKValueCommand;
	ProcessFunc[c2s_plockitem] = &KProtocolProcess::NpcLockPlayerItemCommand;
	ProcessFunc[c2s_itemyearexp] = &KProtocolProcess::RemoveItemYearExpCommand;
	ProcessFunc[c2s_breakitem] = &KProtocolProcess::ItemBreak;
	//ProcessFunc[c2s_datau]=&KProtocolProcess::NpcDaTauCommand;
	//ProcessFunc[c2s_datau1]=&KProtocolProcess::NpcDaTau1Command;
	ProcessFunc[c2s_playercommand] = &KProtocolProcess::PlayerCommand;
	//ProcessFunc[c2s_autoplay] = &KProtocolProcess::c2sSetAutoSync;
	ProcessFunc[c2s_playeractionchat] = &KProtocolProcess::C2SPlayerActionChatCmd;
	ProcessFunc[c2s_openshop] = &KProtocolProcess::C2SClientOpenShopCommand;
	ProcessFunc[c2s_cpsetimage] = &KProtocolProcess::c2sSetImage;
	ProcessFunc[c2s_autoplayersellitem] = &KProtocolProcess::AutoPlayerSellItem;
	//ProcessFunc[c2s_player_pos_sync] = &KProtocolProcess::c2sPosSync;
	//ProcessFunc[c2s_playerstop] = &KProtocolProcess::c2sPlayerStopNotify;
	ProcessFunc[c2s_uicmdscript] = &KProtocolProcess::UiCommandScript;
	ProcessFunc[c2s_recoverybox] = &KProtocolProcess::RecoveryBoxCmd;
	ProcessFunc[c2s_inputinfo] = &KProtocolProcess::c2sInputCommand;
	ProcessFunc[c2s_getcityowntong] = &KProtocolProcess::c2sGetCityOwnTong;
	ProcessFunc[c2s_recoveritem] = &KProtocolProcess::RecoverItemCommand;
	ProcessFunc[c2s_playerthrowallitem] = &KProtocolProcess::c2sPlayerThrowAllItem;
	ProcessFunc[c2s_aibacktotown] = &KProtocolProcess::c2sBackToTown;//Auto by quay lπi;
	ProcessFunc[c2s_setmeridian] = &KProtocolProcess::c2sSetMeridian;
	ProcessFunc[c2s_baucua] = &KProtocolProcess::c2sBauCua;


#endif
}

KProtocolProcess::~KProtocolProcess()
{
}

#ifndef _SERVER

extern IClientCallback* l_pDataChangedNotifyFunc;
void KProtocolProcess::ProcessNetMsg(BYTE* pMsg)
{
	if (!pMsg || pMsg[0] <= s2c_clientbegin || pMsg[0] >= s2c_end || ProcessFunc[pMsg[0]] == NULL)
	{
		g_DebugLog("[error]Net Msg Error");
		return;
	}
	//g_DebugLog("[net]Msg:%c", pMsg[0]);
	if (ProcessFunc[pMsg[0]])
		(this->*ProcessFunc[pMsg[0]])(pMsg);
}
#else
void KProtocolProcess::ProcessNetMsg(int nIndex, BYTE* pMsg)
{
	_ASSERT(pMsg && pMsg[0] > c2s_gameserverbegin && pMsg[0] < c2s_end);

	BYTE	byProtocol = pMsg[0];
	_ASSERT(nIndex > 0 && nIndex < MAX_PLAYER);
	if (ProcessFunc[byProtocol])
	{
		(this->*ProcessFunc[byProtocol])(nIndex, pMsg);
		Player[nIndex].SetLastNetOperationTime(g_SubWorldSet.GetGameTime());
	}
}
#endif

#ifndef _SERVER
void KProtocolProcess::NpcSleepSync(BYTE* pMsg)
{
	NPC_SLEEP_SYNC*	pSync = (NPC_SLEEP_SYNC *)pMsg;
	DWORD	dwNpcId = pSync->NpcID;

	int nIdx = NpcSet.SearchID(dwNpcId);

	if (nIdx > 0)
	{
		Npc[nIdx].SetSleepMode((BOOL)pSync->bSleep);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void	KProtocolProcess::s2cViewSellItem(BYTE* pMsg)
{
	g_cSellItem.GetData(pMsg);
}

void KProtocolProcess::s2cBattleBox(BYTE* pMsg)
{
	S2C_BATTLE_BOX	*Cmd = (S2C_BATTLE_BOX *)pMsg;
	CoreDataChanged(GDCNI_UPDATE_BATTLE_BOX, (unsigned int)Cmd->szBattleDesc, Cmd->nType);
}

void    KProtocolProcess::s2cPlayerStop(BYTE * pMsg)
{ 

}

void    KProtocolProcess::s2cPosEdition(BYTE * pMsg)
{

}

void KProtocolProcess::s2cFindPathSync(BYTE* pMsg)
{

}

void 	KProtocolProcess::s2cSyncSuperShop(BYTE* pMsg)
{
	S2C_SUPERSHOP* pInfo = (S2C_SUPERSHOP*)pMsg;
	BuySell.OpenSale(pInfo->m_nSaleType, (BuySellInfo*)&pInfo->m_BuySellInfo);
}

void	KProtocolProcess::IgnoreState(BYTE* pMsg) //B· qua trπng th∏i
{
	IGNORE_STATE_SYNC	*pSync = (IGNORE_STATE_SYNC*)pMsg;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].IgnoreState((BOOL)pSync->bNegative);
}

void	KProtocolProcess::s2cRideSync(BYTE* pMsg)
{
	NPC_RIDE_SYNC	*pValue = (NPC_RIDE_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].CheckRideHorse(pValue->m_btRideFlag);
}

void	KProtocolProcess::s2cAutoPlaySync(BYTE* pMsg)
{
	/*PLAYER_REQUEST_AUTO	*pInfo = (PLAYER_REQUEST_AUTO*)pMsg;
	BYTE btAuto = pInfo->m_bAuto;
	BOOL bActive = pInfo->m_bActive;
	if (btAuto == 1)
	{
		if (bActive)
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, "ß∑ bÀt auto!");
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		else
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, "Tæt auto!");
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);		
		}
		Player[CLIENT_PLAYER_INDEX].SetAutoFlag((BOOL)bActive);
	}*/
}

void	KProtocolProcess::s2cExtPointValueSync(BYTE* pMsg)
{
	EXTPOINT_VALUE_SYNC	*pValue = (EXTPOINT_VALUE_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].SetExtPoint(pValue->m_nExtPointValue);
	if (pValue->m_nChangeExtPoint)
	{
		APPLY_GET_EXTPOINT_COMMAND	nExt;
		nExt.ProtocolType	= c2s_extendtong;
		nExt.m_wLength		= sizeof(nExt) - 1;
		nExt.m_btMsgId		= enumCOMMAND_UPDATE_EXTPOINT;
		nExt.m_nExtPoint 	= pValue->m_nExtPointValue;
		if (g_pClient)
			g_pClient->SendPackToServer(&nExt, nExt.m_wLength + 1);
	}
}

void	KProtocolProcess::s2cViewUpdateItem(BYTE* pMsg)
{
	g_cSellItem.GetData(pMsg,1);
}

void	KProtocolProcess::s2cGetCouunt(BYTE* pMsg)
{
	PLAYER_GET_COUNT *pGetCount = (PLAYER_GET_COUNT *)pMsg;
	// (r5c - auto Da Tau) ghi tra loi tham do sap: CO goi ve = sap that
	g_sDTCap.dwCntId = pGetCount->dwId;
	g_sDTCap.nCnt = pGetCount->nCount;
	++g_sDTCap.uCntSeq;
	g_cSellItem.Check(pGetCount->nCount,pGetCount->dwId);
}

void	KProtocolProcess::s2cShopName(BYTE* pMsg)
{
	PLAYER_PLAYER_SHOPNAME *pShopname = (PLAYER_PLAYER_SHOPNAME *)pMsg;
	int nIndex = NpcSet.SearchID(pShopname->m_dwNpcID);
	strcpy(Npc[nIndex].ShopName,pShopname->m_Name);
}

void KProtocolProcess::s2cPing(BYTE* pMsg)
{
//	DWORD	dwTimer = GetTickCount();
//	PING_COMMAND* PingCmd = (PING_COMMAND *)pMsg;
//
//	dwTimer -= PingCmd->m_dwTime;
//	dwTimer >>= 1;
//	g_SubWorldSet.SetPing(dwTimer);
//	g_bPingReply = TRUE;

	DWORD	dwTimer = GetTickCount();
	PING_COMMAND*	PingCmd = (PING_COMMAND *)pMsg;
	PING_CLIENTREPLY_COMMAND	pcc;
	pcc.ProtocolType = c2s_ping;
	pcc.m_dwReplyServerTime = PingCmd->m_dwTime;
	pcc.m_dwClientTime = dwTimer;
	g_pClient->SendPackToServer(&pcc, sizeof(PING_CLIENTREPLY_COMMAND));
	//g_SubWorldSet.SetPing(PingCmd->m_dwTime);
}

void KProtocolProcess::ServerReplyClientPing(BYTE* pMsg)
{
	DWORD	dwTimer = GetTickCount();
	PING_COMMAND* pPc = (PING_COMMAND *)pMsg;

	dwTimer -= pPc->m_dwTime;
	dwTimer >>= 1;
	g_SubWorldSet.SetPing(dwTimer);
}

void KProtocolProcess::s2cPlayerExp(BYTE* pMsg)
{
	PLAYER_EXP_SYNC	*pExp = (PLAYER_EXP_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].s2cSetExp(pExp->m_nExp);
}

void	KProtocolProcess::s2cChatScreenSingleError(BYTE* pMsg)
{
	CHAT_SCREENSINGLE_ERROR_SYNC	*pError = (CHAT_SCREENSINGLE_ERROR_SYNC*)pMsg;
	
	// 
	char	szName[32];
	memset(szName, 0, sizeof(szName));
	memcpy(szName, pError->m_szName, pError->m_wLength + 1 + sizeof(pError->m_szName) - sizeof(CHAT_SCREENSINGLE_ERROR_SYNC));

	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_CHAT_TAR_REFUSE_SINGLE_TALK, szName);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}

void KProtocolProcess::NetCommandChgCamp(BYTE* pMsg)
{
	DWORD	dwNpcId;

	dwNpcId = *(DWORD *)&pMsg[1];
	int nIdx = NpcSet.SearchID(dwNpcId);

	if (nIdx > 0)
	{
		Npc[nIdx].m_Camp = (int)pMsg[5];
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandChgCurCamp(BYTE* pMsg)
{
	DWORD	dwNpcId;

	dwNpcId = *(DWORD *)&pMsg[1];
	int nIdx = NpcSet.SearchID(dwNpcId);

	if (nIdx > 0)
	{
		Npc[nIdx].m_CurrentCamp = (int)pMsg[5];
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandDeath(BYTE* pMsg)
{
	DWORD	dwNpcID;
	
	dwNpcID = *(DWORD *)&pMsg[1];
	int nIdx = NpcSet.SearchID(dwNpcID);

	if (nIdx > 0)
	{
		//Npc[nIdx].SendCommand(do_death);
		Npc[nIdx].ProcNetCommand(do_death);
		Npc[nIdx].m_CurrentLife = 0;
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
		//g_DebugLog("[Death]Net command comes");
	}
}

void KProtocolProcess::NetCommandJump(BYTE* pMsg)
{
	NPC_JUMP_SYNC* pNetCommandJump = (NPC_JUMP_SYNC *)pMsg;
	DWORD dwNpcId = pNetCommandJump->ID;
	int nIdx = NpcSet.SearchID(dwNpcId);
	
	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		Npc[nIdx].SendCommand(do_jump, pNetCommandJump->nMpsX, pNetCommandJump->nMpsY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandHurt(BYTE* pMsg)
{
	NPC_HURT_SYNC*	pSync = (NPC_HURT_SYNC *)pMsg;
	
	int nIdx = NpcSet.SearchID(pSync->ID);
	if (nIdx > 0)
	{
		//Npc[nIdx].SendCommand(do_hurt, pSync->nFrames, pSync->nX, pSync->nY);
		Npc[nIdx].ProcNetCommand(do_hurt, pSync->nFrames, pSync->nX, pSync->nY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandRemoveNpc(BYTE* pMsg)
{
	DWORD	dwNpcID;
	dwNpcID = *(DWORD *)&pMsg[1];
	int nIdx = NpcSet.SearchID(dwNpcID);

	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		if (Npc[nIdx].m_RegionIndex >= 0)
		{
			// zroc change
			SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
			SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
//			SubWorld[Npc[nIdx].m_SubWorldIndex].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
		}
		NpcSet.Remove(nIdx);
	}
}

void KProtocolProcess::NetCommandRun(BYTE* pMsg)
{
	DWORD	dwNpcID;
	DWORD	MapX, MapY;
	
	dwNpcID = *(DWORD *)&pMsg[1];
	MapX = *(int *)&pMsg[5];
	MapY = *(int *)&pMsg[9];
	int nIdx = NpcSet.SearchID(dwNpcID);
	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		Npc[nIdx].SendCommand(do_run, MapX, MapY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandSit(BYTE* pMsg)
{
	NPC_SIT_SYNC*	pSitSync;

	pSitSync = (NPC_SIT_SYNC *)pMsg;

	int nIdx = NpcSet.SearchID(pSitSync->ID);

	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		Npc[nIdx].SendCommand(do_sit);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::OpenSaleBox(BYTE* pMsg)
{
	SALE_BOX_SYNC* pSale = (SALE_BOX_SYNC *)pMsg;
	BuySell.OpenSale((BuySellInfo*)&pSale->m_BuySellInfo);
}

void KProtocolProcess::OpenStoreBox(BYTE* pMsg)
{
	CoreDataChanged(GDCNI_OPEN_STORE_BOX, NULL, NULL);
}

void KProtocolProcess::OpenResetPass(BYTE* pMsg)
{
	CoreDataChanged(GDCNI_OPEN_RESET_PASS, NULL, NULL);
}

void KProtocolProcess::OpenDaTauBox(BYTE* pMsg)
{	
	CoreDataChanged(GDCNI_OPEN_DATAU_BOX, NULL, NULL);
}

void KProtocolProcess::OpenDaTau1Box(BYTE* pMsg)
{	
	CoreDataChanged(GDCNI_OPEN_DATAU_BOX1, NULL, NULL);
}

void KProtocolProcess::s2c_ExitGame(BYTE* pMsg)
{
	CoreDataChanged(GDCNI_S2C_EXIT_GAME, NULL, NULL);
}

void KProtocolProcess::s2cPlayerItemLockSync(BYTE* pMsg)
{
	PLAYER_ITEM_LOCK_SYNC* pSync = (PLAYER_ITEM_LOCK_SYNC*)pMsg;
	if(pSync && pSync->m_ItemIdx > 0)
	{
		int nIdx = ItemSet.SearchID(pSync->m_ItemIdx);
		Item[nIdx].SetPlayerItemLock(pSync->m_InsuranceCourse);
		Item[nIdx].SetPlayerItemHLock(pSync->m_InsuranceHourCourse);
		Item[nIdx].SetPrice(pSync->sPrice);//#Fix ban vat pham khoa bao hiem // vÀt ph»m kho∏ kh´ng th” b∏n set gi∏ bªng 0

		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, "Thao t∏c b∂o hi”m trang bﬁ hoµn t t.");
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
}

void KProtocolProcess::PlayerRevive(BYTE* pMsg)
{
	NPC_REVIVE_SYNC* pSync = (NPC_REVIVE_SYNC*)pMsg;

	int nIdx = NpcSet.SearchID(pSync->ID);
	if (nIdx > 0)
	{
		if (!Npc[nIdx].IsPlayer() && pSync->Type == REMOTE_REVIVE_TYPE)
		{
			SubWorld[0].m_WorldMessage.Send(GWM_NPC_DEL, nIdx); //xoa npc khi chet edit by phong kieu
			return;
		}
		else
		{
			Npc[nIdx].ProcNetCommand(do_revive);
		}
	}
}

void KProtocolProcess::RequestNpcFail(BYTE* pMsg)
{
	NPC_REQUEST_FAIL* pNpcSync = (NPC_REQUEST_FAIL *)pMsg;

	if (NpcSet.IsNpcRequestExist(pNpcSync->ID))
		NpcSet.RemoveNpcRequest(pNpcSync->ID);	
}

void KProtocolProcess::NetCommandSkill(BYTE* pMsg)
{
	DWORD	dwNpcID;
	int		nSkillID, nSkillLevel, nSkillEnChance;
	int		MapX, MapY;
	
	dwNpcID = *(DWORD *)&pMsg[1];
	nSkillID = *(int *)&pMsg[5];
	nSkillLevel = *(int *)&pMsg[9];
	MapX = *(int *)&pMsg[13];
	MapY = *(int *)&pMsg[17];
	nSkillEnChance = *(int *)&pMsg[21];
	
	if (MapY < 0)
		return ;

	//µ±÷∏∂®ƒ≥∏ˆƒø±Í ±(MapX == -1),MapYŒ™ƒø±ÍµƒNpcdwID£¨–Ë“™◊™ªª≥…±æµÿµƒNpcIndex≤≈––
	if (MapX < 0)
	{
		if (MapX != -1)
			return;
	
		MapY = NpcSet.SearchID(MapY);
		if (MapY == 0)
			return;

		if (Npc[MapY].m_RegionIndex < 0)
			return;

	}
	
	int nIdx = NpcSet.SearchID(dwNpcID);
	
	if (nIdx <= 0) 
		return;

	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		Npc[nIdx].m_SkillList.SetSkillLevel(nSkillID, nSkillLevel);
		Npc[nIdx].SendCommand(do_skill, nSkillID, MapX, MapY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandWalk(BYTE* pMsg)
{
	DWORD	dwNpcID;
	DWORD	MapX, MapY;
	
	dwNpcID = *(DWORD *)&pMsg[1];
	MapX = *(int *)&pMsg[5];
	MapY = *(int *)&pMsg[9];
	int nIdx = NpcSet.SearchID(dwNpcID);
	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		Npc[nIdx].SendCommand(do_walk, MapX, MapY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cApplyCreateTeamFalse(BYTE* pMsg)
{
	PLAYER_SEND_CREATE_TEAM_FALSE *pCreateFalse = (PLAYER_SEND_CREATE_TEAM_FALSE*)pMsg;
	KSystemMessage	sMsg;

	switch (pCreateFalse->m_btErrorID)
	{
	// 
	case Team_Create_Error_InTeam:
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		break;

	// 
	case Team_Create_Error_CannotCreate:
		sprintf(sMsg.szMessage, MSG_TEAM_CANNOT_CREATE);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		break;


	default:
		sprintf(sMsg.szMessage, MSG_TEAM_CREATE_FAIL);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		break;
	}
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cApplyTeamInfoFalse(BYTE* pMsg)
{
	// ΩÁ√Êœ‘ æ∂”ŒÈ≤È—Ø ß∞‹(not end)
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatAddFriend(BYTE* pMsg)
{
	CHAT_ADD_FRIEND_SYNC	*pFriend = (CHAT_ADD_FRIEND_SYNC*)pMsg;

	Player[CLIENT_PLAYER_INDEX].m_cChat.AddOne(pFriend->m_dwID, pFriend->m_nIdx, pFriend->m_szName, 0);
	Player[CLIENT_PLAYER_INDEX].m_cChat.SaveTeamInfo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);

	KSystemMessage	sMsg;
	KUiPlayerItem	sPlayer;

	memset(&sPlayer, 0, sizeof(KUiPlayerItem));
	strcpy(sPlayer.Name, pFriend->m_szName);
	sPlayer.uId = pFriend->m_dwID;
	sPlayer.nIndex = pFriend->m_nIdx;

	sprintf(sMsg.szMessage, MSG_CHAT_ADD_FRIEND_SUCCESS, pFriend->m_szName);
	sMsg.eType = SMT_FRIEND;
	sMsg.byConfirmType = SMCT_UI_INTERVIEW;
	sMsg.byPriority = 2;
	sMsg.byParamSize = sizeof(KUiPlayerItem);
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&sPlayer);
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatAddFriendFail(BYTE* pMsg)
{
	CHAT_ADD_FRIEND_FAIL_SYNC	*pFail = (CHAT_ADD_FRIEND_FAIL_SYNC*)pMsg;

	char	szName[32];
	CChatApplyListNode	*pList;
	pList = (CChatApplyListNode*)Player[CLIENT_PLAYER_INDEX].m_cChat.m_cApplyAddList.GetHead();
	while (pList)
	{
		if (pList->m_nPlayerIdx == pFail->m_nTargetPlayerIdx)
		{
			g_StrCpy(szName, pList->m_szName);

			// 
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHAT_ADD_FRIEND_FAIL, szName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			return;
		}
		pList = (CChatApplyListNode*)pList->GetNext();
	}	
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatAddOneFriend(BYTE* pMsg)
{
	CHAT_ONE_FRIEND_DATA_SYNC	*pData = (CHAT_ONE_FRIEND_DATA_SYNC*)pMsg;
	int		nTeamNo;

	nTeamNo = Player[CLIENT_PLAYER_INDEX].m_cChat.CheckTeamNo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, pData->m_szName);
	if (nTeamNo < 0)
		nTeamNo = 0;
	Player[CLIENT_PLAYER_INDEX].m_cChat.AddOne(pData->m_dwID, pData->m_nPlayerIdx, pData->m_szName, nTeamNo);
	Player[CLIENT_PLAYER_INDEX].m_cChat.SaveTeamInfo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);

}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatBeRefusedAddFriend(BYTE* pMsg)
{
	CHAT_REFUSE_FRIEND_SYNC	*pRefuse = (CHAT_REFUSE_FRIEND_SYNC*)pMsg;
	char	szName[64];

	memset(szName, 0, sizeof(szName));
	memcpy(szName, pRefuse->m_szName, pRefuse->m_wLength + 1 + sizeof(pRefuse->m_szName) - sizeof(CHAT_REFUSE_FRIEND_SYNC));

	// 
	KSystemMessage	sMsg;

	sprintf(sMsg.szMessage, MSG_CHAT_REFUSE_FRIEND, szName);
	sMsg.eType = SMT_FRIEND;
	sMsg.byConfirmType = SMCT_CLICK;
	sMsg.byPriority = 1;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatFriendOnline(BYTE* pMsg)
{
	CHAT_FRIEND_ONLINE_SYNC	*pFriend = (CHAT_FRIEND_ONLINE_SYNC*)pMsg;

	Player[CLIENT_PLAYER_INDEX].ChatFriendOnLine(pFriend->m_dwID, pFriend->m_nPlayerIdx);
}

//-------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatGetApplyAddFriend(BYTE* pMsg)
{
	CHAT_APPLY_ADD_FRIEND_SYNC	*pApply = (CHAT_APPLY_ADD_FRIEND_SYNC*)pMsg;

	if ( Player[CLIENT_PLAYER_INDEX].m_cChat.CheckIsFriend(pApply->m_nSrcPlayerIdx) )
		return;

	CChatApplyListNode	*pNode = NULL;
	pNode = new CChatApplyListNode;
	if (pNode == NULL)
		return;
	pNode->m_nPlayerIdx = pApply->m_nSrcPlayerIdx;
	strcpy(pNode->m_szName, pApply->m_szSourceName);
	strcpy(pNode->m_szInfo, pApply->m_szInfo);

	Player[CLIENT_PLAYER_INDEX].m_cChat.m_cApplyAddList.AddTail(pNode);

	// 
	KSystemMessage	sMsg;
	KUiPlayerItem	sPlayer;

	strcpy(sPlayer.Name, pApply->m_szSourceName);
	sPlayer.nIndex = pApply->m_nSrcPlayerIdx;
	sPlayer.uId = 0;
	sPlayer.nData = 0;

	sprintf(sMsg.szMessage, MSG_CHAT_GET_FRIEND_APPLY, pNode->m_szName);
	sMsg.eType = SMT_FRIEND;
	sMsg.byConfirmType = SMCT_UI_FRIEND_INVITE;
	sMsg.byPriority = 3;
	sMsg.byParamSize = sizeof(KUiPlayerItem);
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&sPlayer);
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatLoginFriendName(BYTE* pMsg)
{
	CHAT_LOGIN_FRIEND_NAME_SYNC	*pFriend = (CHAT_LOGIN_FRIEND_NAME_SYNC*)pMsg;
	int		nTeamNo;
	char	szName[64];

	memset(szName, 0, sizeof(szName));
	memcpy(szName, pFriend->m_szName, pFriend->m_wLength + 1 + sizeof(pFriend->m_szName) - sizeof(CHAT_LOGIN_FRIEND_NAME_SYNC));
	nTeamNo = Player[CLIENT_PLAYER_INDEX].m_cChat.CheckTeamNo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, szName);
	if (nTeamNo < 0)
		nTeamNo = 0;
	Player[CLIENT_PLAYER_INDEX].m_cChat.AddOne(pFriend->m_dwID, pFriend->m_nPlayerIdx, szName, nTeamNo);
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatLoginFriendNoName(BYTE* pMsg)
{
	CHAT_LOGIN_FRIEND_NONAME_SYNC	*pFriend = (CHAT_LOGIN_FRIEND_NONAME_SYNC*)pMsg;
	char	szName[32];
	int		nTeamNo;

	szName[0] = 0;
	nTeamNo = Player[CLIENT_PLAYER_INDEX].m_cChat.CheckTeamNo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, NULL);
	if (nTeamNo < 0)
	{
		if (Player[CLIENT_PLAYER_INDEX].m_cChat.m_nLoginGetFirstOneFriendFlag == 0)		// …Í«Î÷ÿ–¬∑¢ÀÕÀ˘”–∫√”— ˝æ›£¨¥¯√˚◊÷
		{
			Player[CLIENT_PLAYER_INDEX].m_cChat.m_nLoginGetFirstOneFriendFlag = 1;

			CHAT_APPLY_RESEND_ALL_FRIEND_NAME_COMMAND	sApply;
			sApply.ProtocolType = c2s_chatapplyresendallfriendname;

			if (g_pClient)
				g_pClient->SendPackToServer((BYTE*)&sApply, sizeof(CHAT_APPLY_RESEND_ALL_FRIEND_NAME_COMMAND));
		}
		else												// 
		{
			CHAT_APPLY_SEND_ONE_FRIEND_NAME_COMMAND	sApply;
			sApply.ProtocolType = c2s_chatapplysendonefriendname;
			sApply.m_dwID = pFriend->m_dwID;

			if (g_pClient)
				g_pClient->SendPackToServer((BYTE*)&sApply, sizeof(CHAT_APPLY_SEND_ONE_FRIEND_NAME_COMMAND));
		}
		return;
	}
	Player[CLIENT_PLAYER_INDEX].m_cChat.AddOne(pFriend->m_dwID, pFriend->m_nPlayerIdx, szName, nTeamNo);
	Player[CLIENT_PLAYER_INDEX].m_cChat.SaveTeamInfo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
	Player[CLIENT_PLAYER_INDEX].m_cChat.m_nLoginGetFirstOneFriendFlag = 1;

}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cCreateTeam(BYTE* pMsg)
{
	PLAYER_SEND_CREATE_TEAM_SUCCESS	*pCreateSuccess = (PLAYER_SEND_CREATE_TEAM_SUCCESS*)pMsg;

	g_Team[0].CreateTeam(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level, pCreateSuccess->nTeamServerID);
	Player[CLIENT_PLAYER_INDEX].m_cTeam.Release();
	Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 1;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_CAPTAIN;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nApplyCaptainID = 0;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].RestoreCurrentCamp();

	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_TEAM_CREATE);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

	sprintf(sMsg.szMessage, MSG_TEAM_CREATE);
	sMsg.eType = SMT_TEAM;
	sMsg.byConfirmType = SMCT_CLICK;
	sMsg.byPriority = 1;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetInstantSpr(enumINSTANT_STATE_CREATE_TEAM);
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cFactionSkillOpen(BYTE* pMsg)
{
	PLAYER_FACTION_SKILL_LEVEL	*pSkill = (PLAYER_FACTION_SKILL_LEVEL*)pMsg;

	if (Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction < 0 || Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction != pSkill->m_btCurFactionID)
	{
		// 
		Player[CLIENT_PLAYER_INDEX].ApplyFactionData();
		return;
	}

//	for (int i = 0; i < FACTIONS_PRR_SERIES; i++)
//	{
//		if (Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[i].m_nID == Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction)
//		{
//			Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[i].m_nOpenLevel = pSkill->m_btLevel;
//			Player[CLIENT_PLAYER_INDEX].m_cFaction.OpenCurSkillLevel(Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[i].m_nOpenLevel, &Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList);
//			break;
//		}
//	}
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cGetChat(BYTE* pMsg)
{
	PLAYER_SEND_CHAT_SYNC	*pChat = (PLAYER_SEND_CHAT_SYNC*)pMsg;
	char tmpName[32];
	char tmpMsg[256];
	Player[CLIENT_PLAYER_INDEX].m_cChat.GetChat(pChat);
	memset(tmpName, 0, sizeof(tmpName));
	memcpy(tmpName, &pChat->m_szSentence[0], pChat->m_btNameLen); //OK Npc Name[32]
	memset(tmpMsg, 0, sizeof(tmpMsg));
	memcpy(tmpMsg, &pChat->m_szSentence[pChat->m_btNameLen], pChat->m_wSentenceLen); //OK Npc Name[32]

	l_pDataChangedNotifyFunc->ChannelMessageArrival(
		pChat->m_btCurChannel, tmpName,
		tmpMsg, strlen(tmpMsg), true, true, pChat->m_btIsShowMsgPad);
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cGetCurAttribute(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cGetCurAttribute(pMsg);
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cGetLeadExp(BYTE* pMsg)
{
	DWORD	dwLevel = Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel;

	PLAYER_LEAD_EXP_SYNC	*pLeadExp = (PLAYER_LEAD_EXP_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_dwLeadExp = pLeadExp->m_dwLeadExp;
	Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel = PlayerSet.m_cLeadExp.GetLevel(Player[CLIENT_PLAYER_INDEX].m_dwLeadExp, Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel);
	Player[CLIENT_PLAYER_INDEX].m_dwNextLevelLeadExp = PlayerSet.m_cLeadExp.GetLevelExp(Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel);

	if (dwLevel < Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_LEADER_LEVEL_UP, Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel);
		sMsg.eType = SMT_PLAYER;
		sMsg.byConfirmType = SMCT_CLICK;
		sMsg.byPriority = 1;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cGetSkillLevel(BYTE* pMsg)
{
	int		nSkillIndex;
	PLAYER_SKILL_LEVEL_SYNC	*pSkill = (PLAYER_SKILL_LEVEL_SYNC*)pMsg;
	if (pSkill->m_nSkillLevel > MAX_SKILLLEVEL)
		return;
	nSkillIndex = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(pSkill->m_nSkillID);
	Player[CLIENT_PLAYER_INDEX].m_nSkillPoint = pSkill->m_nLeavePoint;
	if ( !nSkillIndex )
	{
		if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.Add(pSkill->m_nSkillID, pSkill->m_nSkillLevel, pSkill->m_nSkillExp, pSkill->m_bTempSkill) == 0 )
			return;
	}
	else
	{
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.SetLevel(nSkillIndex, pSkill->m_nSkillLevel);
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.SetAddLevel(pSkill->m_nSkillID, pSkill->m_nAddLevel);
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.SetCurLevel(pSkill->m_nSkillID, pSkill->m_nSkillLevel + pSkill->m_nAddLevel);
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.SetExp(nSkillIndex, pSkill->m_nSkillExp);
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.SetTempSkill(nSkillIndex, pSkill->m_bTempSkill);
	}

	// 
	CoreDataChanged(GDCNI_FIGHT_SKILL_POINT, 0, Player[CLIENT_PLAYER_INDEX].m_nSkillPoint);
	KUiSkillData SkillData;

	KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(pSkill->m_nSkillID, 1);
	if (!pOrdinSkill)
		return;
	eSkillStyle eStyle = ( eSkillStyle ) pOrdinSkill->GetSkillStyle();
			
	switch(eStyle)
	{
	case SKILL_SS_Missles:			//	◊”µØ¿‡		±æººƒ‹”√”⁄∑¢ÀÕ◊”µØ¿‡
	case SKILL_SS_Melee:
	case SKILL_SS_InitiativeNpcState:	//	÷˜∂Ø¿‡		±æººƒ‹”√”⁄∏ƒ±‰µ±«∞Npcµƒ÷˜∂Ø◊¥Ã¨
	case SKILL_SS_PassivityNpcState:		//	±ª∂Ø¿‡		±æººƒ‹”√”⁄∏ƒ±‰Npcµƒ±ª∂Ø◊¥Ã¨
		{
			if(pOrdinSkill->IsBase())
				return;
		}break;
	case SKILL_SS_Thief:					//	Õµ«‘¿‡
		{
			
		}
		break;
	default:
		return;
	}
	int nSkillPosition = -1;
	SkillData.uGenre = CGOG_SKILL_FIGHT;
	nSkillPosition = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetSkillPosition(pSkill->m_nSkillID);
	SkillData.uId = pSkill->m_nSkillID;
	SkillData.nLevel = pSkill->m_nSkillLevel;
	if(nSkillPosition < 0) return;
	CoreDataChanged(GDCNI_SKILL_CHANGE, (unsigned int)&SkillData, nSkillPosition);
}
//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cGetTeammateLevel(BYTE* pMsg)
{
	PLAYER_TEAMMATE_LEVEL_SYNC	*pLevel = (PLAYER_TEAMMATE_LEVEL_SYNC*)pMsg;
	if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
	{
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		return;
	}

	if ((DWORD)g_Team[0].m_nCaptain == pLevel->m_dwTeammateID)
	{
		g_Team[0].m_nMemLevel[0] = (DWORD)pLevel->m_btLevel;
		return;
	}

	for (int i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if ((DWORD)g_Team[0].m_nMember[i] == pLevel->m_dwTeammateID)
		{
			g_Team[0].m_nMemLevel[i + 1] = (DWORD)pLevel->m_btLevel;
			break;
		}
	}
}

void KProtocolProcess::s2cLeaveFaction(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_cFaction.LeaveFaction();
	// 
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetCamp(camp_free);

	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
}

void KProtocolProcess::s2cLeaveTeam(BYTE* pMsg)
{
	if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
		return;
	}

	PLAYER_LEAVE_TEAM	*pLeaveTeam = (PLAYER_LEAVE_TEAM*)pMsg;

	if (pLeaveTeam->m_dwNpcID == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID)
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 0;
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nApplyCaptainID = 0;
		g_Team[0].Release();
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMenuState() == PLAYER_MENU_STATE_TEAMOPEN)
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetMenuState(PLAYER_MENU_STATE_NORMAL);
	}
	else
	{
		g_Team[0].DeleteMember(pLeaveTeam->m_dwNpcID);
	}

	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
	return;
}

void KProtocolProcess::s2cLevelUp(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cLevelUp(pMsg);
}

void KProtocolProcess::s2cMoveItem(BYTE* pMsg)
{
	PLAYER_MOVE_ITEM_SYNC	*pMove = (PLAYER_MOVE_ITEM_SYNC*)pMsg;

	ItemPos		DownPos, UpPos;
	DownPos.nPlace = pMove->m_btDownPos;
	DownPos.nX = pMove->m_btDownX;
	DownPos.nY = pMove->m_btDownY;
	UpPos.nPlace = pMove->m_btUpPos;
	UpPos.nX = pMove->m_btUpX;
	UpPos.nY = pMove->m_btUpY;

	Player[CLIENT_PLAYER_INDEX].m_ItemList.ExchangeItem(&DownPos, &UpPos);
}

void KProtocolProcess::s2cSwitchEquip(BYTE* pMsg)
{
	PLAYER_SWITCH_EQUIP_SYNC* pMove = (PLAYER_SWITCH_EQUIP_SYNC*)pMsg;

	int activeEquipNum = pMove->m_nSetNum;

	Player[CLIENT_PLAYER_INDEX].SwitchEquipSet(activeEquipNum);
}

void KProtocolProcess::s2cRemoveItem(BYTE* pMsg)
{
	ITEM_REMOVE_SYNC	*pRemove = (ITEM_REMOVE_SYNC*)pMsg;

	int		nIdx;
	nIdx = Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchID(pRemove->m_ID);
	if (nIdx > 0)
	{
		Player[CLIENT_PLAYER_INDEX].m_ItemList.Remove(nIdx);
		Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
	}
}

void KProtocolProcess::s2cRemoveAllItem(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_ItemList.RemoveAll();
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cSetFactionData(BYTE* pMsg)
{
	PLAYER_FACTION_DATA	*pData = (PLAYER_FACTION_DATA*)pMsg;

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetCamp(pData->m_btCamp);
	Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction = (char)pData->m_btCurFaction;
	Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nFirstAddFaction = (char)pData->m_btFirstFaction;
	Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nAddTimes = pData->m_nAddTimes;

	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);

//	Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[0].m_nOpenLevel = pData->m_btNo1SkillOpenLevel;
//	Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[1].m_nOpenLevel = pData->m_btNo2SkillOpenLevel;
//	if (Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction == Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[0].m_nID)
//		Player[CLIENT_PLAYER_INDEX].m_cFaction.OpenCurSkillLevel(Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[0].m_nOpenLevel, &Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList);
//	else
//		Player[CLIENT_PLAYER_INDEX].m_cFaction.OpenCurSkillLevel(Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[1].m_nOpenLevel, &Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList);
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cSetTeamState(BYTE* pMsg)
{
	// 
	if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag || Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure != TEAM_CAPTAIN)
	{
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		return;
	}

	PLAYER_TEAM_OPEN_CLOSE	*pTeamState = (PLAYER_TEAM_OPEN_CLOSE*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.ReleaseList();
	if (pTeamState->m_btOpenClose)
	{
		g_Team[0].SetTeamOpen();

		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_TEAM_OPEN);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	else
	{
		g_Team[0].SetTeamClose();

		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_TEAM_CLOSE);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
}

void	KProtocolProcess::s2cApplyAddTeam(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cApplyAddTeam(pMsg);
}

void KProtocolProcess::s2cShowTeamInfo(BYTE* pMsg)
{

}

void KProtocolProcess::s2cSyncItem(BYTE* pMsg)
{
	ITEM_SYNC	*pItemSync = (ITEM_SYNC*)pMsg;
	int nIndex = 0;
	if (pItemSync->m_bIsNew)
	{
		if (pItemSync->m_Nature < 2) { //no GOLDEQUIP2 or PLATINA, add normal use old code
			if (!pItemSync->m_GoldId)
				nIndex = ItemSet.AddItemSet2(
					pItemSync->m_Genre,
					pItemSync->m_Series,
					pItemSync->m_Level,
					pItemSync->m_Luck,
					pItemSync->m_Detail,
					pItemSync->m_Particur,
					pItemSync->m_MagicLevel,
					pItemSync->m_Version,
					pItemSync->m_RandomSeed,
					pItemSync->m_StackNum,
					pItemSync->m_EnChance,
					pItemSync->m_Point, pItemSync->m_TimeE, 0, 0, 0, 0, 0, pItemSync->m_MaxOptMultiply);
			else
				nIndex = ItemSet.AddGoldItem(
					pItemSync->m_GoldId,
					pItemSync->m_MagicLevel,
					pItemSync->m_Series,
					pItemSync->m_EnChance, pItemSync->m_TimeE, 0, 0, 0, 0, pItemSync->m_MaxOptMultiply);
		}
		else
			nIndex = ItemSet.Add(
			pItemSync->m_Nature,
			pItemSync->m_Genre,
			pItemSync->m_Series,
			pItemSync->m_Level,
			pItemSync->m_Luck,
			pItemSync->m_Detail,
			pItemSync->m_Particur,
			pItemSync->m_MagicLevel,
			pItemSync->m_Version,
			pItemSync->m_RandomSeed,
			pItemSync->m_MaxOptMultiply);
	}
	else
		nIndex = ItemSet.SearchID(pItemSync->m_ID);
	if (nIndex)
	{
		Item[nIndex].SetTemp(pItemSync->m_bTemp);
		Item[nIndex].SetOwner(pItemSync->m_dwOwner);
		Item[nIndex].SetTradePrice(pItemSync->m_Price);
		Item[nIndex].SetID(pItemSync->m_ID);
		if (pItemSync->m_Nature >= NATURE_GOLD)
			Item[nIndex].SetRow(pItemSync->m_Detail);
		else
			Item[nIndex].SetDetailType(pItemSync->m_Detail);
		Item[nIndex].SetNature(pItemSync->m_Nature);
		Item[nIndex].SetGenre(pItemSync->m_Genre);
		Item[nIndex].SetParticular(pItemSync->m_Particur);
		Item[nIndex].SetLevel(pItemSync->m_Level);
		Item[nIndex].SetDurability(pItemSync->m_Durability);
		Item[nIndex].SetExpireTime(pItemSync->m_ExpireTime);
		Item[nIndex].SetLock(&pItemSync->m_LockItem);
		Item[nIndex].SetLockSell(pItemSync->m_bLockSell);
		Item[nIndex].SetLockTrade(pItemSync->m_bLockTrade);
		Item[nIndex].SetLockDrop(pItemSync->m_bLockDrop);
		Item[nIndex].SetParam(pItemSync->m_Param);
		Item[nIndex].SetMantle(pItemSync->m_Mantle);
		Item[nIndex].SetBackLocal(&pItemSync->m_BackLocal);
		Item[nIndex].SetFortune(pItemSync->m_Fortune);
		Item[nIndex].SetStackNum(pItemSync->m_StackNum);
		memcpy(Item[nIndex].m_GeneratorParam.nGeneratorLevel, pItemSync->m_MagicLevel, sizeof(int) * MAX_ITEM_MAGICATTRIB);
		Item[nIndex].m_GeneratorParam.uRandomSeed = pItemSync->m_RandomSeed;
		Item[nIndex].m_GeneratorParam.nVersion = pItemSync->m_Version;
		Item[nIndex].m_GeneratorParam.nLuck = pItemSync->m_Luck;
		Item[nIndex].SetExpTime(pItemSync->m_TimeE, 0, 0, 0);

		if (pItemSync->m_bIsNew)
			Player[CLIENT_PLAYER_INDEX].m_ItemList.AddKIL(nIndex, pItemSync->m_btPlace, pItemSync->m_btX, pItemSync->m_btY);
		else
			Player[CLIENT_PLAYER_INDEX].m_ItemList.AddKIL(nIndex,
				Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[nIndex].nPlace,
				Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[nIndex].nX,
				Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[nIndex].nY);
	}

	//int pnMagicParam[6];
	//for (int i = 0; i < 6; i++)
	//{
	//	pnMagicParam[i] = pItemSync->m_MagicLevel[i];
	//}
	//int nIndex = 0;
	//if (!pItemSync->m_bIsNew)
	//	nIndex = ItemSet.SearchID(pItemSync->m_ID);
	//else if (!pItemSync->m_GoldId)
	//	nIndex = ItemSet.AddItemSet2(
	//	pItemSync->m_Genre, 
	//	pItemSync->m_Series,
	//	pItemSync->m_Level,
	//	pItemSync->m_Luck,
	//	pItemSync->m_Detail,
	//	pItemSync->m_Particur,
	//	pnMagicParam,
	//	pItemSync->m_Version,
	//	pItemSync->m_RandomSeed,
	//	pItemSync->m_StackNum,
	//	pItemSync->m_EnChance,
	//	pItemSync->m_Point, pItemSync->m_TimeE, 0, 0, 0);
	//else
	//	nIndex = ItemSet.AddGoldItem(
	//	pItemSync->m_GoldId,
	//	pnMagicParam,
	//	pItemSync->m_Series,
	//	pItemSync->m_EnChance, pItemSync->m_TimeE, 0, 0, 0);

	//if (nIndex > 0)
	//{
	//	Item[nIndex].SetStackNum(pItemSync->m_StackNum);
	//	Item[nIndex].SetID(pItemSync->m_ID);
	//	Item[nIndex].SetDurability((short)pItemSync->m_Durability);
	//	Item[nIndex].SetPlayerItemLock(pItemSync->m_InsuranceCourse);
	//	Item[nIndex].SetPlayerItemHLock(pItemSync->m_HInsuranceCourse);
	//	Item[nIndex].SetMantle(pItemSync->m_Mantle); //#phi phong
	//	Item[nIndex].SetParam(pItemSync->m_Param); //#s?l«n s?dÙng item
	//	Item[nIndex].SetItemGlowLight(pItemSync->m_GlowLight); //#ngoπi trang v?kh?ph∏t s∏ng
	//	Item[nIndex].SetPrice(pItemSync->m_Price);
	//	Player[CLIENT_PLAYER_INDEX].m_ItemList.AddKIL(nIndex, pItemSync->m_btPlace, pItemSync->m_btX, pItemSync->m_btY);
	//}
	Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
}

void KProtocolProcess::s2cSyncMoney(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cSyncMoney(pMsg);
}

void KProtocolProcess::s2cSyncRoleList(BYTE* pMsg)
{
	ROLE_LIST_SYNC	*pSync = (ROLE_LIST_SYNC *)pMsg;
}

void KProtocolProcess::s2cTeamAddMember(BYTE* pMsg)
{
	if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag )
	{
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		return;
	}

	PLAYER_TEAM_ADD_MEMBER	*pAddMem = (PLAYER_TEAM_ADD_MEMBER*)pMsg;
	int		nMemNum;

	nMemNum = g_Team[0].FindFree();
	if (nMemNum < 0)
		return;
	g_Team[0].m_nMember[nMemNum] = pAddMem->m_dwNpcID;
	g_Team[0].m_nMemLevel[nMemNum + 1] = (DWORD)pAddMem->m_btLevel;
	strcpy(g_Team[0].m_szMemName[nMemNum + 1], pAddMem->m_szName);
	g_Team[0].m_nMemNum++;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.DeleteOneFromApplyList(pAddMem->m_dwNpcID);
	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();

	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_TEAM_ADD_MEMBER, pAddMem->m_szName);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cTeamChangeCaptain(BYTE* pMsg)
{
	if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
	{
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		return;
	}

	int		nLevel, nMemNo;
	char	szName[32];
	PLAYER_TEAM_CHANGE_CAPTAIN	*pChange = (PLAYER_TEAM_CHANGE_CAPTAIN*)pMsg;

	nMemNo = g_Team[0].FindMemberID(pChange->m_dwCaptainID);
	// 
	if ((DWORD)g_Team[0].m_nCaptain != pChange->m_dwMemberID || nMemNo < 0)
	{
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		return;
	}

	// 
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID == pChange->m_dwCaptainID)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN_SELF, g_Team[0].m_szMemName[0]);
		sMsg.eType = SMT_TEAM;
		sMsg.byConfirmType = SMCT_UI_TEAM;
		sMsg.byPriority = 3;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	else	// 
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN, g_Team[0].m_szMemName[nMemNo + 1]);
		sMsg.eType = SMT_TEAM;
		sMsg.byConfirmType = SMCT_UI_TEAM;
		sMsg.byPriority = 3;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}

	// 
	nLevel = g_Team[0].m_nMemLevel[0];
	strcpy(szName, g_Team[0].m_szMemName[0]);
	g_Team[0].m_nCaptain = pChange->m_dwCaptainID;
	g_Team[0].m_nMemLevel[0] = g_Team[0].m_nMemLevel[nMemNo + 1];
	strcpy(g_Team[0].m_szMemName[0], g_Team[0].m_szMemName[nMemNo + 1]);
	g_Team[0].m_nMember[nMemNo] = pChange->m_dwMemberID;
	g_Team[0].m_nMemLevel[nMemNo + 1] = nLevel;
	strcpy(g_Team[0].m_szMemName[nMemNo + 1], szName);

	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID == pChange->m_dwCaptainID)
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_CAPTAIN;
		Player[CLIENT_PLAYER_INDEX].m_cTeam.ReleaseList();
	}
	else if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID == pChange->m_dwMemberID)
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_MEMBER;
		Player[CLIENT_PLAYER_INDEX].m_cTeam.ReleaseList();
	}

	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cUpdataSelfTeamInfo(BYTE* pMsg)
{
	int		i;
	PLAYER_SEND_SELF_TEAM_INFO	*pSelfInfo = (PLAYER_SEND_SELF_TEAM_INFO*)pMsg;

	//  ˝æ›∞¸≥ˆ¥Ì
	if (pSelfInfo->m_dwNpcID[0] == 0)
		return;

	Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 1;
	Player[CLIENT_PLAYER_INDEX].m_dwLeadExp = pSelfInfo->m_dwLeadExp;
	Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel = PlayerSet.m_cLeadExp.GetLevel(Player[CLIENT_PLAYER_INDEX].m_dwLeadExp);
	Player[CLIENT_PLAYER_INDEX].m_dwNextLevelLeadExp = PlayerSet.m_cLeadExp.GetLevelExp(Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel);
	if (Player[CLIENT_PLAYER_INDEX].m_dwNextLevelLeadExp == 0)
		Player[CLIENT_PLAYER_INDEX].m_dwNextLevelLeadExp = Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel;

	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID == pSelfInfo->m_dwNpcID[0])	// ∂”≥§
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_CAPTAIN;
	}
	else													// ∂”‘±
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_MEMBER;
		for (i = 0; i < MAX_TEAM_APPLY_LIST; i++)
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_sApplyList[i].Release();
	}
	g_Team[0].m_nCaptain = pSelfInfo->m_dwNpcID[0];
	if (pSelfInfo->m_btState == 0)
		g_Team[0].SetTeamClose();
	else
		g_Team[0].SetTeamOpen();
	g_Team[0].m_nTeamServerID = pSelfInfo->nTeamServerID;
	g_Team[0].m_nMemLevel[0] = (DWORD)pSelfInfo->m_btLevel[0];
	strcpy(g_Team[0].m_szMemName[0], pSelfInfo->m_szNpcName[0]);
	g_Team[0].m_nMemNum = 0;
	for (i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if (pSelfInfo->m_dwNpcID[i + 1] > 0)
		{
			g_Team[0].m_nMember[i] = pSelfInfo->m_dwNpcID[i + 1];
			g_Team[0].m_nMemLevel[i + 1] = (DWORD)pSelfInfo->m_btLevel[i + 1];
			strcpy(g_Team[0].m_szMemName[i + 1], pSelfInfo->m_szNpcName[i + 1]);
			g_Team[0].m_nMemNum++;
		}
		else
		{
			g_Team[0].m_nMember[i] = -1;
			g_Team[0].m_nMemLevel[i + 1] = 0;
			g_Team[0].m_szMemName[i + 1][0] = 0;
		}
	}
	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
}

void KProtocolProcess::SyncCurNormalData(BYTE* pMsg)
{
	CURPLAYER_NORMAL_SYNC	*pSync = (CURPLAYER_NORMAL_SYNC*)pMsg;
	if (pSync->m_shLife > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife = pSync->m_shLife;
	else
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife = 0;
	if (pSync->m_shStamina > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStamina = pSync->m_shStamina;
	else
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStamina = 0;

	if (pSync->m_shMana > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana = pSync->m_shMana;
	else
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana = 0;

//	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_sit)
//	{
//		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife >= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLifeMax &&
//			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana >= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentManaMax)
//		{
//			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_stand);
//		}
//	}

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	switch (pSync->m_btTeamData)
	{
	case 0x00:
		if ( Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag )
		{
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 0;
			Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		}
		break;
	case 0x03:
		if ( !Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag || Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure != TEAM_CAPTAIN)
		{
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 1;
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_CAPTAIN;
			Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		}
		break;
	case 0x01:
		if ( !Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag || Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure != TEAM_MEMBER)
		{
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 1;
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_MEMBER;
			Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		}
		break;
	}
	//sync to ext auto
	bool bSync = false;
	for (int i=1; i<MAX_NPCSKILL; i++)
	{
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.m_Skills[i].SkillId > 0)
		{
			bSync = true;
			break;
		}
	}
	if(bSync)
	{
		IPCMainSync s;
		s.CmdID = PRG_MAINSYNC;
		s.Size = sizeof(IPCMainSync);
		strcpy(s.szName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
		int x,y;
		g_ScenePlace.GetSceneNameAndFocus(s.szMap, s.nMapId, x, y);
		s.nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		s.dwPID = Player[CLIENT_PLAYER_INDEX].m_dwID;
		s.life = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife;
		s.mana = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana;
		s.lifemax = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLifeMax;
		s.manamax = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentManaMax;
		s.stamina = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStamina;
		s.staminamax = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStaminaMax;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&s.mapx, &s.mapy);
		s.level = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level;
		s.curexp = Player[CLIENT_PLAYER_INDEX].m_nExp;
		s.fullexp = Player[CLIENT_PLAYER_INDEX].m_nNextLevelExp;
		s.skillnum = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetAllSkillByType(&s.skill[0]);
		s.nSelServer1 = PlayerSet.m_nSelSvGroup;
		s.nSelServer2 = PlayerSet.m_nSelServer;
		strcpy(s.szPassword, PlayerSet.m_szPassword);
		strcpy(s.szAccount, PlayerSet.m_szAccount);
		SendDataToTool(&s, sizeof(IPCMainSync));
	}
}

void KProtocolProcess::SyncCurPlayer(BYTE* pMsg) //Sync Player 1 l«n ch›nh m◊nh
{
	Player[CLIENT_PLAYER_INDEX].SyncCurPlayer(pMsg);
}

void KProtocolProcess::SyncNpc(BYTE* pMsg)	//Sync 1 l«n khi npc trong Æ„ c„ player
{
	NPC_SYNC* NpcSync = (NPC_SYNC *)pMsg;

	int nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[0].Mps2Map(NpcSync->MapX, NpcSync->MapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

	if (nRegion == -1)
		return;

	int nIdx = NpcSet.SearchID(NpcSync->ID);
	if (!nIdx)
	{
		nIdx = NpcSet.AddNpcSet2(NpcSync->NpcSettingIdx, NpcSync->m_bySeries, 0, NpcSync->MapX, NpcSync->MapY);
		Npc[nIdx].m_dwID = NpcSync->ID;
		Npc[nIdx].m_Kind = NpcSync->m_btKind;
		Npc[nIdx].m_Height = 0;
		
		if (NpcSet.IsNpcRequestExist(NpcSync->ID))
			NpcSet.RemoveNpcRequest(NpcSync->ID);
	}
	else
	{
		if (Npc[nIdx].m_RegionIndex >= 0)
		{
			SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
		}

		Npc[nIdx].m_MapX = nMapX;
		Npc[nIdx].m_MapY = nMapY;
		Npc[nIdx].m_OffX = nOffX;
		Npc[nIdx].m_OffY = nOffY;

		Npc[nIdx].m_NpcSettingIdx = (short)HIWORD(NpcSync->NpcSettingIdx);
		Npc[nIdx].m_Level = LOWORD(NpcSync->NpcSettingIdx);

		if (Npc[nIdx].m_RegionIndex >= 0)
		{
			if (Npc[nIdx].m_RegionIndex != nRegion)
			{
				// zroc change
				SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].m_RegionID, SubWorld[0].m_Region[nRegion].m_RegionID, nIdx);
				Npc[nIdx].m_RegionIndex = nRegion;
			}
			SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
		}
		else
		{
			SubWorld[0].m_Region[nRegion].AddNpc(nIdx);
			Npc[nIdx].m_RegionIndex = nRegion;
			SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
		}
	}

	Npc[nIdx].m_Camp			= (NPCCAMP)NpcSync->Camp;
	Npc[nIdx].m_CurrentCamp		= (NPCCAMP)NpcSync->CurrentCamp;
	Npc[nIdx].m_Series			= NpcSync->m_bySeries;
	Npc[nIdx].m_Type			= NpcSync->NpcEnchant;
	/*if (NpcSync->LifePerCent <= 128)
		Npc[nIdx].m_CurrentLife	= (Npc[nIdx].m_CurrentLifeMax * NpcSync->LifePerCent) >> 7 ;
	else
		Npc[nIdx].m_CurrentLife	= 0;*/

	if (Npc[nIdx].m_Doing != do_death || Npc[nIdx].m_Doing != do_revive) // need check later -- spe 03/05/27
		Npc[nIdx].SendCommand((NPCCMD)NpcSync->m_Doing, NpcSync->MapX, NpcSync->MapY);

	Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	Npc[nIdx].SetMenuState(NpcSync->m_btMenuState);

	Npc[nIdx].m_CurrentLife		= NpcSync->m_CurrentLife;
	Npc[nIdx].m_CurrentLifeMax	= NpcSync->m_CurrentLifeMax;
	Npc[nIdx].m_LifeMax			= NpcSync->m_LifeMax;
	Npc[nIdx].m_CurrentLifeReplenish = NpcSync->m_CurrentLifeReplenish;
	Npc[nIdx].m_LifeReplenish		= NpcSync->m_LifeReplenish;
	Npc[nIdx].m_CurrentAttackRating = NpcSync->m_CurrentAttackRating;
	Npc[nIdx].m_AttackRating		= NpcSync->m_AttackRating;
	Npc[nIdx].m_CurrentDefend		= NpcSync->m_CurrentDefend;
	Npc[nIdx].m_Defend			    = NpcSync->m_Defend;
	Npc[nIdx].m_CurrentExperience   = NpcSync->m_CurrentExperience;
	Npc[nIdx].m_Experience		    = NpcSync->m_Experience;
	Npc[nIdx].m_CurrentWalkSpeed    = NpcSync->m_CurrentWalkSpeed;
	Npc[nIdx].m_WalkSpeed			= NpcSync->m_WalkSpeed;
	Npc[nIdx].m_CurrentRunSpeed		= NpcSync->m_CurrentRunSpeed;
	Npc[nIdx].m_RunSpeed			= NpcSync->m_RunSpeed;
	Npc[nIdx].m_CurrentHitRecover   = NpcSync->m_CurrentHitRecover;		//thÍi gian phÙc hÂi
	Npc[nIdx].m_HitRecover		    = NpcSync->m_HitRecover;		//thÍi gian phÙc hÂi
	Npc[nIdx].m_nMissionGroup	= NpcSync->MissionGroup;//#NpcMissionGroup
	memset(Npc[nIdx].Name, 0, sizeof(Npc[nIdx].Name));
	memcpy(Npc[nIdx].Name, NpcSync->m_szName, NpcSync->m_wLength - (sizeof(NPC_SYNC) - 1 - sizeof(NpcSync->m_szName)));
}

void KProtocolProcess::SyncNpcMin(BYTE* pMsg)	//Sync li™n tÙc npc trong Æ„ c„ player vµ npc
{
	NPC_NORMAL_SYNC* NpcSync = (NPC_NORMAL_SYNC *)pMsg;

	if (NpcSync->Doing == do_revive || NpcSync->Doing == do_death)
		return;
	//--end add
	int nIdx = NpcSet.SearchID(NpcSync->ID);
	if (!nIdx)
	{
		if(NpcSync->State & STATE_HIDE)	//npc khac' dang tang hinh, khong co san~ npc
			return;
		if (!NpcSet.IsNpcRequestExist(NpcSync->ID))
		{
			if (NpcSet.InsertNpcRequest(NpcSync->ID))
				SendClientCmdRequestNpc(NpcSync->ID);
		}
		return;//add by Fong Ki“u from KT
	}
	else
	{
		int nRegion, nMapX, nMapY, nOffX, nOffY;
		SubWorld[0].Mps2Map(NpcSync->MapX, NpcSync->MapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

		//
		if (Npc[nIdx].m_RegionIndex == -1 && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)
		{
			if(NpcSync->State & STATE_HIDE) //npc khac' dang tang hinh, da co npc -> xoa
			{
				NpcSet.Remove(nIdx);
				return;
			}
			if (nRegion == -1)
			{		
				return;
			}
			else
			{
				Npc[nIdx].m_MapX = nMapX;
				Npc[nIdx].m_MapY = nMapY;
				Npc[nIdx].m_OffX = NpcSync->m_fkOffX;//nOffX;
				Npc[nIdx].m_OffY = NpcSync->m_fkOffY;//nOffY;
				Npc[nIdx].m_RegionIndex = nRegion;
				Npc[nIdx].m_dwRegionID = NpcSync->m_fkRegionID;//SubWorld[0].m_Region[nRegion].m_RegionID;
				SubWorld[0].m_Region[nRegion].AddNpc(nIdx);
				SubWorld[0].m_Region[nRegion].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
			}
			
			if (NpcSync->Doing == do_stand)
				Npc[nIdx].ProcNetCommand(do_stand);
		}
		else
		{
			if(NpcSync->State & STATE_HIDE && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex) //npc khac' dang tang hinh, da co npc -> xoa
			{
				SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
				SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
				NpcSet.Remove(nIdx);
				return;
			}
			if (Npc[nIdx].m_RegionIndex != nRegion && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)
			{
				SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
				SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);

				Npc[nIdx].m_MapX = nMapX;
				Npc[nIdx].m_MapY = nMapY;
				Npc[nIdx].m_OffX = NpcSync->m_fkOffX;//nOffX;
				Npc[nIdx].m_OffY = NpcSync->m_fkOffY;//nOffY;
				Npc[nIdx].m_RegionIndex = nRegion;
				if (nRegion >= 0)
				{
					SubWorld[0].m_Region[nRegion].AddNpc(nIdx);
					Npc[nIdx].m_dwRegionID = NpcSync->m_fkRegionID;//SubWorld[0].m_Region[nRegion].m_RegionID;
					SubWorld[0].m_Region[nRegion].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
				}
			}
		}

		if (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)	// ∑«ÕÊº?
		{
			int	nOldLife = Npc[nIdx].m_CurrentLife;
			Npc[nIdx].m_CurrentLife = NpcSync->m_CurrentLife;
			//if (Npc[nIdx].m_Kind == kind_normal)
			//{
			//	Npc[nIdx].SetBlood(nOldLife - Npc[nIdx].m_CurrentLife); // M∏u qu∏i b?gi∂m hi”n th?l™n Æ«u
			//}
			Npc[nIdx].m_Series				= NpcSync->m_bySeries;
		}
		Npc[nIdx].m_CurrentCamp			= NpcSync->Camp;
		//
		Npc[nIdx].m_FreezeState.nTime	= (NpcSync->State & STATE_FREEZE);
		Npc[nIdx].m_PoisonState.nTime	= (NpcSync->State & STATE_POISON);
		Npc[nIdx].m_StunState.nTime		= (NpcSync->State & STATE_STUN);
		Npc[nIdx].m_HideState.nTime		= (NpcSync->State & STATE_HIDE);
		//Npc[nIdx].m_MoveShadow.nTime	= (NpcSync->State & STATE_MOVE);
		Npc[nIdx].m_FrozenAction.nTime	= (NpcSync->State & STATE_FROZEN);
		//Npc[nIdx].m_ForbidAttack.nTime	= (NpcSync->State & STATE_FBDATK);
		Npc[nIdx].m_WalkRun.nTime		= NpcSync->State & STATE_WALKRUN;
		//
		Npc[nIdx].m_nProtectedTime = NpcSync->m_nProtectedTime;			//vong tron bat tu, vﬂng trﬂn b t t?
		Npc[nIdx].m_CurrentLife			= NpcSync->m_CurrentLife;
		Npc[nIdx].m_CurrentLifeMax	= NpcSync->m_CurrentLifeMax;
		Npc[nIdx].m_LifeMax				= NpcSync->m_LifeMax;
		Npc[nIdx].m_WalkSpeed			= NpcSync->m_WalkSpeed;
		Npc[nIdx].m_RunSpeed			= NpcSync->m_RunSpeed;
		Npc[nIdx].m_CurrentAttackSpeed	= NpcSync->m_ASpeed;
		Npc[nIdx].m_CurrentCastSpeed	= NpcSync->m_CSpeed;
		Npc[nIdx].m_ManaMax				= NpcSync->m_ManaMax;	//viet them sync mana teamMNG
		Npc[nIdx].m_CurrentMana				= NpcSync->m_CurrentMana;
		Npc[nIdx].m_CurrentManaMax				= NpcSync->m_CurrentManaMax;
		Npc[nIdx].m_nMissionGroup	= NpcSync->MissionGroup;//#NpcMissionGroup
		Npc[nIdx].SetNpcState(NpcSync->StateInfo);
		Npc[nIdx].m_Type					= NpcSync->NpcEnchant;
		Npc[nIdx].m_SyncSignal			= SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::SyncNpcMinPlayer(BYTE* pMsg) //Sync li™n tÙc ch?player x?l?khi vµo c∏c region < 0
{	
	NPC_PLAYER_TYPE_NORMAL_SYNC	*pSync = (NPC_PLAYER_TYPE_NORMAL_SYNC*)pMsg;
	
	_ASSERT(pSync->m_dwNpcID == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID);
	if (pSync->m_dwNpcID != Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID) //kh∏c b∂n th©n m◊nh th?kh´ng th˘c hi÷n
		return;

	int nRegion, nMapX, nMapY, nOffX, nOffY, nNpcIdx;
	SubWorld[0].Mps2Map(pSync->m_dwMapX, pSync->m_dwMapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

	nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (Npc[nNpcIdx].m_RegionIndex == -1)
	{
		if (nRegion < 0)
		{
			//_ASSERT(0);
			return;
		}
		Npc[nNpcIdx].m_RegionIndex = nRegion;
		Npc[nNpcIdx].m_dwRegionID = SubWorld[0].m_Region[nRegion].m_RegionID;
		SubWorld[0].NpcChangeRegion(-1, SubWorld[0].m_Region[nRegion].m_RegionID, nNpcIdx);
		SubWorld[0].m_Region[nRegion].AddRef(nMapX, nMapY, obj_npc);
		Npc[nNpcIdx].m_MapX = nMapX;
		Npc[nNpcIdx].m_MapY = nMapY;
		Npc[nNpcIdx].m_OffX = pSync->m_wOffX;
		Npc[nNpcIdx].m_OffY = pSync->m_wOffY;
		memset(&Npc[nNpcIdx].m_sSyncPos, 0, sizeof(Npc[nNpcIdx].m_sSyncPos));
		Npc[nNpcIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
		return;
	}

	// `¯`´‘∂£¨≥¨≥ˆ9∆¡∑∂Œß
	if (nRegion == -1)
	{
		SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].DecRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, obj_npc);

		int nRegionX = pSync->m_dwMapX / (SubWorld[0].m_nCellWidth * SubWorld[0].m_nRegionWidth);
		int nRegionY = pSync->m_dwMapY / (SubWorld[0].m_nCellHeight * SubWorld[0].m_nRegionHeight);
		
		DWORD	dwRegionID = MAKELONG(nRegionX, nRegionY);
		SubWorld[0].LoadMap(SubWorld[0].m_SubWorldID, dwRegionID);

		nRegion = SubWorld[0].FindRegion(dwRegionID);
		_ASSERT(nRegion >= 0);
		Npc[nNpcIdx].m_RegionIndex = nRegion;
		Npc[nNpcIdx].m_dwRegionID = dwRegionID;
		SubWorld[0].NpcChangeRegion(-1, SubWorld[0].m_Region[nRegion].m_RegionID, nNpcIdx);

		SubWorld[0].Mps2Map(pSync->m_dwMapX, pSync->m_dwMapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
		Npc[nNpcIdx].m_MapX = nMapX;
		Npc[nNpcIdx].m_MapY = nMapY;
		Npc[nNpcIdx].m_OffX = pSync->m_wOffX;
		Npc[nNpcIdx].m_OffY = pSync->m_wOffY;
		
		SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].AddRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, obj_npc);
		memset(&Npc[nNpcIdx].m_sSyncPos, 0, sizeof(Npc[nNpcIdx].m_sSyncPos));
		Npc[nNpcIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;

		return;
	}

	BYTE	byBarrier = SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].GetBarrier(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, Npc[nNpcIdx].m_OffX, Npc[nNpcIdx].m_OffY);
	if (0 != byBarrier && Obstacle_JumpFly != byBarrier)
	{
		g_DebugLog("[Barrier]Player in Barrier");
	}
/*	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.CountItemInAll() != pSync->m_nEquipCount) {
		//reconnect
		g_DebugLog("Player Equip Count Error");
		C2SPLAYER_AI_BACKTOTOWN AutoCmd;
		AutoCmd.ProtocolType = c2s_aibacktotown;
		AutoCmd.nIdSubWorld = 0;
		AutoCmd.dwID = Player[CLIENT_PLAYER_INDEX].GetPlayerID();
		AutoCmd.dwTimePacker = GetTickCount();
		if (g_pClient)
			g_pClient->SendPackToServer((BYTE*)&AutoCmd, sizeof(C2SPLAYER_AI_BACKTOTOWN));

	}*/
}

void KProtocolProcess::SyncObjectAdd(BYTE* pMsg)
{
	OBJ_ADD_SYNC	*pObjSyncAdd = (OBJ_ADD_SYNC*)pMsg;
	int				nObjIndex;
	KObjItemInfo	sInfo;

	nObjIndex = ObjSet.FindID(pObjSyncAdd->m_nID);
	if (nObjIndex > 0)
		return;

	sInfo.m_nItemID = pObjSyncAdd->m_nItemID;
	sInfo.m_nItemWidth = pObjSyncAdd->m_btItemWidth;
	sInfo.m_nItemHeight = pObjSyncAdd->m_btItemHeight;
	sInfo.m_nMoneyNum = pObjSyncAdd->m_nMoneyNum;
	sInfo.m_nColorID = pObjSyncAdd->m_btColorID;
	sInfo.m_nGenre = pObjSyncAdd->m_nGenre;
	sInfo.m_nDetailType = pObjSyncAdd->m_nDetailType;
	sInfo.m_nParticularType = pObjSyncAdd->m_nParticularType;
	sInfo.m_dwNpcId1 = pObjSyncAdd->m_dwNpcId;
	sInfo.m_nMovieFlag = ((pObjSyncAdd->m_btFlag & 0x02) > 0 ? 1 : 0);
	sInfo.m_nSoundFlag = ((pObjSyncAdd->m_btFlag & 0x01) > 0 ? 1 : 0);
	memset(sInfo.m_szName, 0, sizeof(sInfo.m_szName));
	memcpy(sInfo.m_szName, pObjSyncAdd->m_szName, pObjSyncAdd->m_wLength + 1 + sizeof(pObjSyncAdd->m_szName) - sizeof(OBJ_ADD_SYNC));

	nObjIndex = ObjSet.ClientAdd(
		pObjSyncAdd->m_nID,
		pObjSyncAdd->m_nDataID,
		pObjSyncAdd->m_btState,
		pObjSyncAdd->m_btDir,
		pObjSyncAdd->m_wCurFrame,
		pObjSyncAdd->m_nXpos,
		pObjSyncAdd->m_nYpos,
		sInfo);
#ifdef WAIGUA_ZROC
	if (nObjIndex <= 0)
		return;
	PLAYER_PICKUP_ITEM_COMMAND	sPickUp;
	if (Object[nObjIndex].m_nKind == Obj_Kind_Money)
	{
//		sPickUp.ProtocolType = c2s_playerpickupitem;
//		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
//		sPickUp.m_btPosType = 0;
//		sPickUp.m_btPosX = 0;
//		sPickUp.m_btPosY = 0;
//		if (g_pClient)
//			g_pClient->SendPackToServer(&sPickUp, sizeof(PLAYER_PICKUP_ITEM_COMMAND));
	}
	else if (Object[nObjIndex].m_nKind == Obj_Kind_Item)
	{
		ItemPos	sItemPos;
		if ( FALSE == Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(Object[nObjIndex].m_nItemWidth, Object[nObjIndex].m_nItemHeight, &sItemPos) )
			return;
		sPickUp.ProtocolType = c2s_playerpickupitem;
		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
		sPickUp.m_btPosType = sItemPos.nPlace;
		sPickUp.m_btPosX = sItemPos.nX;
		sPickUp.m_btPosY = sItemPos.nY;
		if (g_pClient)
			g_pClient->SendPackToServer(&sPickUp, sizeof(PLAYER_PICKUP_ITEM_COMMAND));
	}
#endif
}

//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ ’µΩ∑˛ŒÒ∆˜œ˚œ¢∏¸–¬ƒ≥∏ˆobj∑ΩœÚ
//-------------------------------------------------------------------------
void KProtocolProcess::SyncObjectDir(BYTE* pMsg)
{
	OBJ_SYNC_DIR	*pObjSyncDir = (OBJ_SYNC_DIR*)pMsg;
	int				nObjIndex;
	nObjIndex = ObjSet.FindID(pObjSyncDir->m_nID);
	if (nObjIndex <= 0)
	{
		// œÚ∑˛ŒÒ∆˜∑¢ÃÌº”«Î«Û
		OBJ_CLIENT_SYNC_ADD	sObjClientSyncAdd;
		sObjClientSyncAdd.ProtocolType = c2s_requestobj;
		sObjClientSyncAdd.m_nID = pObjSyncDir->m_nID;
		if (g_pClient)
			g_pClient->SendPackToServer(&sObjClientSyncAdd, sizeof(sObjClientSyncAdd));
	}
	else
	{	// Õ¨≤Ω∑ΩœÚ
		Object[nObjIndex].SetDir(pObjSyncDir->m_btDir);
	}
}

//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ ’µΩ∑˛ŒÒ∆˜œ˚œ¢…æ≥˝ƒ≥∏ˆobj
//-------------------------------------------------------------------------
void KProtocolProcess::SyncObjectRemove(BYTE* pMsg)
{
	OBJ_SYNC_REMOVE	*pObjSyncRemove = (OBJ_SYNC_REMOVE*)pMsg;
	int				nObjIndex;
	nObjIndex = ObjSet.FindID(pObjSyncRemove->m_nID);
	if (nObjIndex > 0)
	{	
		Object[nObjIndex].Remove(pObjSyncRemove->m_btSoundFlag);
	}
}

void KProtocolProcess::SyncObjectState(BYTE* pMsg)
{
	OBJ_SYNC_STATE	*pObjSyncState = (OBJ_SYNC_STATE*)pMsg;
	int				nObjIndex;
	nObjIndex = ObjSet.FindID(pObjSyncState->m_nID);
	if (nObjIndex <= 0)
	{
		OBJ_CLIENT_SYNC_ADD	sObjClientSyncAdd;
		sObjClientSyncAdd.ProtocolType = c2s_requestobj;
		sObjClientSyncAdd.m_nID = pObjSyncState->m_nID;
		if (g_pClient)
			g_pClient->SendPackToServer(&sObjClientSyncAdd, sizeof(sObjClientSyncAdd));
	}
	else
	{	
		if (Object[nObjIndex].m_nRegionIdx == -1)
		{
			int	nRegion;
			nRegion = SubWorld[0].FindRegion(Object[nObjIndex].m_nBelongRegion);
			if (nRegion >= 0)
			{
				Object[nObjIndex].m_nRegionIdx = nRegion;
				SubWorld[0].m_Region[nRegion].AddObj(nObjIndex);
			}
		}
		Object[nObjIndex].SetState(pObjSyncState->m_btState);
	}
}

void KProtocolProcess::SyncObjectTrap(BYTE* pMsg)
{
	OBJ_SYNC_TRAP_ACT *pObjTrapSync = (OBJ_SYNC_TRAP_ACT*)pMsg;
	int		nObjIndex;
	nObjIndex = ObjSet.FindID(pObjTrapSync->m_nID);
	if (nObjIndex <= 0)
	{
		// œÚ∑˛ŒÒ∆˜∑¢ÃÌº”«Î«Û
		OBJ_CLIENT_SYNC_ADD	sObjClientSyncAdd;
		sObjClientSyncAdd.ProtocolType = c2s_requestobj;
		sObjClientSyncAdd.m_nID = pObjTrapSync->m_nID;
		if (g_pClient)
			g_pClient->SendPackToServer(&sObjClientSyncAdd, sizeof(sObjClientSyncAdd));
	}
	else
	{
		Object[nObjIndex].m_nState = OBJ_TRAP_STATE_ACTING;
		Object[nObjIndex].m_cImage.SetDirStart();
		Object[nObjIndex].m_cSkill.m_nTarX = pObjTrapSync->m_nTarX;
		Object[nObjIndex].m_cSkill.m_nTarY = pObjTrapSync->m_nTarY;
	}
}

void KProtocolProcess::SyncPlayer(BYTE* pMsg) //sync player 1 l«n Æ«u ti™n
{
	PLAYER_SYNC*	pPlaySync = (PLAYER_SYNC *)pMsg;

	int nIdx = NpcSet.SearchID(pPlaySync->ID);
	// Chan ghi de o sentinel Npc[0]: NpcSet.SearchID tra 0 khi CHUA co NPC nay o client
	// (goi player-sync toi truoc goi tao NPC), hoac khi het khe - MAX_NPC o client chi 256
	// (KNpc.h:23) trong khi server la 98000. Khong chan thi moi goi nhu vay ghi thang vao
	// Npc[0] va con dat Npc[0].m_Kind = kind_player.
	if (nIdx <= 0)
		return;

	// 0x20 = co nhan dien bot SimCity do server dat (KNpc.cpp, hai ham sync).
	// Bit nay truoc day luon 0 nen client cu bo qua -> tuong thich nguoc.
	// Dung de loc bot khoi cac danh sach "nguoi choi quanh day" cua WAuto.
	Npc[nIdx].m_btSimCityBot = (pPlaySync->m_btSomeFlag & 0x20) ? 1 : 0;

	Npc[nIdx].m_ArmorType			= pPlaySync->ArmorType;
	Npc[nIdx].m_CurrentAttackSpeed	= pPlaySync->AttackSpeed; 
	Npc[nIdx].m_CurrentCastSpeed	= pPlaySync->CastSpeed; 
	Npc[nIdx].m_HelmType			= pPlaySync->HelmType;
	if(Option.GetLow(LowPlayer) /*&& nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex*/)
	{
		if(Npc[nIdx].m_NpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID)
			Npc[nIdx].m_MaskType = ManTypeNameIdx;
		else
			Npc[nIdx].m_MaskType = WomanTypeNameIdx;
	}
	else
		Npc[nIdx].m_MaskType	= pPlaySync->MaskType;		//#mat na
	Npc[nIdx].m_MantleType		= pPlaySync->MantleType;				//#phi phong
	Npc[nIdx].m_byMantleLevel	= pPlaySync->MantleLevel;
	Npc[nIdx].m_HorseType			= (char)pPlaySync->HorseType;
	if (Npc[nIdx].m_HorseType >= 0)		//edit by phong kieu len xuong ngua
    {
        Npc[nIdx].m_bRideHorse = TRUE;
    }
    else
    {
        Npc[nIdx].m_bRideHorse = FALSE;
    }
	if (Npc[nIdx].m_BaiTan != pPlaySync->m_bBaiTan && pPlaySync->m_bBaiTan)
	{
		SendClientCmdShopName(Npc[nIdx].m_dwID);
	}
	Npc[nIdx].m_BaiTan				= pPlaySync->m_bBaiTan;
	Npc[nIdx].m_CurrentRunSpeed		= pPlaySync->RunSpeed;
	Npc[nIdx].m_CurrentWalkSpeed	= pPlaySync->WalkSpeed;
	Npc[nIdx].m_WeaponType			= pPlaySync->WeaponType;	
	Npc[nIdx].m_Kind					= kind_player;
	Npc[nIdx].m_btRankId				= pPlaySync->RankID;
	Npc[nIdx].m_btRankBattleId			= pPlaySync->RankBattleID;//#RankBattle
	Npc[nIdx].m_btPlayerTitle			= pPlaySync->PlayerTitle;//#PlayerTitle
	strcpy(Npc[nIdx].MateName, pPlaySync->MateName);//#MateName
	strcpy(Npc[nIdx].m_szGameTitle, pPlaySync->GameTitle); //GameTitle
	if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
		Player[CLIENT_PLAYER_INDEX].m_CUnlocked			= pPlaySync->CUnlocked;
	Npc[nIdx].m_ExItemId			= pPlaySync->ExItemID; // hanh trang
	Npc[nIdx].m_ExBoxId				= pPlaySync->ExBoxID; // ruong mo rong
	Npc[nIdx].nRankInWorld			= pPlaySync->RankInWorld; //x’p hπng th’ giÌi
	Npc[nIdx].nRepute				= pPlaySync->Repute;
	Npc[nIdx].nFuYuan				= pPlaySync->FuYuan;
	Npc[nIdx].nPKValue				= pPlaySync->PKValue;
	Npc[nIdx].nReBorn				= pPlaySync->ReBorn;
	Npc[nIdx].nFirstFaction			= pPlaySync->nFirstFaction;
	//
	if (pPlaySync->m_btSomeFlag & 0x01)
		Npc[nIdx].m_nPKFlag			= enumPKNormal;
	else if (pPlaySync->m_btSomeFlag & 0x10)
		Npc[nIdx].m_nPKFlag			= enumPKWar;
	else 
		Npc[nIdx].m_nPKFlag			= enumPKMurder;
	
	if (pPlaySync->m_btSomeFlag & 0x02)
		Npc[nIdx].m_FightMode		= enumPKWar;
	else
		Npc[nIdx].m_FightMode		= enumPKNormal;

	if (pPlaySync->m_btSomeFlag & 0x04)
		Npc[nIdx].SetSleepMode(1);
	else
		Npc[nIdx].SetSleepMode(0);
	//
	strcpy(Npc[nIdx].m_szTongName, pPlaySync->TongName);
	strcpy(Npc[nIdx].m_szTongTitle, pPlaySync->TongTitle);
	
	Npc[nIdx].m_nFigure						= pPlaySync->TongFigure;
	Npc[nIdx].m_Recruit					= pPlaySync->TongRecruit;
	Npc[nIdx].m_ImagePlayer			= pPlaySync->ImagePlayer;
	if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
	{
		Player[CLIENT_PLAYER_INDEX].m_ImagePlayer		= pPlaySync->ImagePlayer;
		Player[CLIENT_PLAYER_INDEX].m_cMeridian.setMeridian(pPlaySync->bMeridianLevel);
		Player[CLIENT_PLAYER_INDEX].UpdataCurData();
	}
}

void KProtocolProcess::SyncPlayerMin(BYTE* pMsg) //Sync Player li™n tÙc
{
	PLAYER_NORMAL_SYNC* pPlaySync = (PLAYER_NORMAL_SYNC *)pMsg;
	int nIdx = NpcSet.SearchID(pPlaySync->ID);
	// Chan ghi de o sentinel Npc[0]: NpcSet.SearchID tra 0 khi CHUA co NPC nay o client
	// (goi player-sync toi truoc goi tao NPC), hoac khi het khe - MAX_NPC o client chi 256
	// (KNpc.h:23) trong khi server la 98000. Khong chan thi moi goi nhu vay ghi thang vao
	// Npc[0] va con dat Npc[0].m_Kind = kind_player.
	if (nIdx <= 0)
		return;

	// 0x20 = co nhan dien bot SimCity do server dat (KNpc.cpp, hai ham sync).
	// Bit nay truoc day luon 0 nen client cu bo qua -> tuong thich nguoc.
	// Dung de loc bot khoi cac danh sach "nguoi choi quanh day" cua WAuto.
	Npc[nIdx].m_btSimCityBot = (pPlaySync->m_btSomeFlag & 0x20) ? 1 : 0;

	Npc[nIdx].m_CurrentWalkSpeed	= pPlaySync->WalkSpeed;
	Npc[nIdx].m_CurrentRunSpeed		= pPlaySync->RunSpeed;
	Npc[nIdx].m_CurrentAttackSpeed	= pPlaySync->AttackSpeed; 
	Npc[nIdx].m_CurrentCastSpeed	= pPlaySync->CastSpeed;
	Npc[nIdx].m_HelmType			= pPlaySync->HelmType;
	Npc[nIdx].m_ArmorType			= pPlaySync->ArmorType;
	Npc[nIdx].m_WeaponType			= pPlaySync->WeaponType;
	Npc[nIdx].m_MantleType			= pPlaySync->MantleType;			//#phi phong
	Npc[nIdx].m_byMantleLevel		= pPlaySync->MantleLevel;
	Npc[nIdx].m_btHonorId			= pPlaySync->HonorID;				//#HonorID
	if(Option.GetLow(LowPlayer) /*&& nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex*/)
	{
		if(Npc[nIdx].m_NpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID)
			Npc[nIdx].m_MaskType = ManTypeNameIdx;
		else
			Npc[nIdx].m_MaskType = WomanTypeNameIdx;
	}
	else
		Npc[nIdx].m_MaskType			= pPlaySync->MaskType;				//#mat na
	Npc[nIdx].m_HorseType			= (char)pPlaySync->HorseType;
	if (Npc[nIdx].m_HorseType >= 0)		//edit by phong kieu len xuong ngua
    {
        Npc[nIdx].m_bRideHorse = TRUE;
    }
    else
    {
        Npc[nIdx].m_bRideHorse = FALSE;
    }
	Npc[nIdx].m_Kind				= kind_player;
	Npc[nIdx].m_btRankId			= pPlaySync->RankID;
	Npc[nIdx].m_btRankBattleId			= pPlaySync->RankBattleID;//#RankBattle
	Npc[nIdx].m_btPlayerTitle			= pPlaySync->PlayerTitle;//#PlayerTitle
	if (Npc[nIdx].m_BaiTan != pPlaySync->m_bBaiTan && pPlaySync->m_bBaiTan)
	{
		SendClientCmdShopName(Npc[nIdx].m_dwID);
	}
	Npc[nIdx].m_BaiTan				= pPlaySync->m_bBaiTan;
	strcpy(Npc[nIdx].MateName, pPlaySync->MateName);//#MateName
	strcpy(Npc[nIdx].m_szGameTitle, pPlaySync->GameTitle); //GameTitle
	if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
		Player[CLIENT_PLAYER_INDEX].m_CUnlocked			= pPlaySync->CUnlocked;
	Npc[nIdx].m_ExItemId			= pPlaySync->ExItemID; // hanh trang
	Npc[nIdx].m_ExBoxId				= pPlaySync->ExBoxID; // ruong mo rong
	Npc[nIdx].nRankInWorld			= pPlaySync->RankInWorld; //x’p hπng th’ giÌi
	Npc[nIdx].nRepute				= pPlaySync->Repute;
	Npc[nIdx].nFuYuan				= pPlaySync->FuYuan;
	Npc[nIdx].nPKValue				= pPlaySync->PKValue;
	Npc[nIdx].nReBorn				= pPlaySync->ReBorn;
	Npc[nIdx].nFirstFaction			= pPlaySync->nFirstFaction;
	//
	if (pPlaySync->m_btSomeFlag & 0x01)
		Npc[nIdx].m_nPKFlag			= enumPKNormal;
	else if (pPlaySync->m_btSomeFlag & 0x10)
		Npc[nIdx].m_nPKFlag			= enumPKWar;
	else 
		Npc[nIdx].m_nPKFlag			= enumPKMurder;

	if (pPlaySync->m_btSomeFlag & 0x02)
		Npc[nIdx].m_FightMode		= enumPKWar;
	else
		Npc[nIdx].m_FightMode		= enumPKNormal;

	if (pPlaySync->m_btSomeFlag & 0x04)
		Npc[nIdx].SetSleepMode(1);
	else
		Npc[nIdx].SetSleepMode(0);

	if (pPlaySync->m_btSomeFlag & 0x08)
		Npc[nIdx].m_nTongFlag		= 1;
	else
		Npc[nIdx].m_nTongFlag		= 0;
	//

	strcpy(Npc[nIdx].m_szTongName, pPlaySync->TongName);
	strcpy(Npc[nIdx].m_szTongTitle, pPlaySync->TongTitle);
	Npc[nIdx].m_nFigure						= pPlaySync->TongFigure;
	Npc[nIdx].m_Recruit						= pPlaySync->TongRecruit;
	if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
	{
		Player[CLIENT_PLAYER_INDEX].m_ImagePlayer		= pPlaySync->ImagePlayer;
		if(Player[CLIENT_PLAYER_INDEX].m_cMeridian.setMeridian(pPlaySync->bMeridianLevel))
			Player[CLIENT_PLAYER_INDEX].UpdataCurData();
	}

	//Npc[nIdx].m_CurrentWalkSpeed = pPlaySync->WalkSpeed;
	//Npc[nIdx].m_CurrentRunSpeed = pPlaySync->RunSpeed;
}

void KProtocolProcess::SyncScriptAction(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].OnScriptAction((PLAYER_SCRIPTACTION_SYNC *)pMsg);
}

void KProtocolProcess::SyncWorld(BYTE* pMsg)
{
	WORLD_SYNC *WorldSync = (WORLD_SYNC *)pMsg;

	if (Player[CLIENT_PLAYER_INDEX].m_nIndex > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_stand);

	if (SubWorld[0].m_SubWorldID != WorldSync->SubWorld)
	{
		SubWorld[0].LoadMap(WorldSync->SubWorld, WorldSync->Region);
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = 0;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nObjectIdx = 0;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetProcessAI(TRUE);
/*
		if(!g_JXPathFinder.Init(g_ScenePlace.GetLittleMap()->GetRect(), g_ScenePlace.GetKScenePlaceMapC()))
		{
			g_DebugLog("[KProtocolProcess::SyncWorld] g_JXPathFinder.Init failed!");
		}
		else
			g_DebugLog("[KProtocolProcess::SyncWorld] g_JXPathFinder.Init successful!");*/
	}

	if (SubWorld[0].m_dwCurrentTime > WorldSync->Frame)
	{
		// øÕªß∂ÀøÏ¡À£¨ºıÀŸ¥¶¿Ì£¨ªÚ’ﬂ «Lag“˝∆µƒæ…œ˚œ¢£¨–Ë“™¥¶¿Ì
	}
	else if (SubWorld[0].m_dwCurrentTime < WorldSync->Frame)
	{
		// øÕªß∂À¬˝¡À£¨º”ÀŸ¥¶¿Ì
	}
	
	SubWorld[0].m_dwCurrentTime = WorldSync->Frame;
	
	
	// Õ¨≤ΩÃÏ∆¯
	SubWorld[0].m_nWeather = WorldSync->Weather;
	g_ScenePlace.ChangeWeather(WorldSync->Weather);
}

//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ ’µΩ∑˛ŒÒ∆˜œ˚œ¢ƒ≥ÕÊº“÷’÷π¡À”Î±æøÕªß∂Àµƒ∫√”—πÿœµ
//-------------------------------------------------------------------------
void	KProtocolProcess::s2cChatDeleteFriend(BYTE* pMsg)
{
	CHAT_DELETE_FRIEND_SYNC	*pDelete = (CHAT_DELETE_FRIEND_SYNC*)pMsg;

	CChatFriend	*pFriend;
	for (int i = 0; i < MAX_FRIEND_TEAM; i++)
	{
		pFriend = (CChatFriend*)Player[CLIENT_PLAYER_INDEX].m_cChat.m_cFriendTeam[i].m_cEveryOne.GetHead();
		while (pFriend)
		{
			if (pFriend->m_dwID == pDelete->m_dwID)
			{
				pFriend->Remove();
				Player[CLIENT_PLAYER_INDEX].m_cChat.m_cFriendTeam[i].m_nFriendNo--;

				// Õ®÷™ΩÁ√Ê ƒ≥ÕÊº“÷’÷π¡À”Î±æøÕªß∂Àµƒ∫√”—πÿœµ
//				CoreDataChanged(GDCNI_CHAT_GROUP, 0, 0);
				CoreDataChanged(GDCNI_CHAT_FRIEND, 0, i);

				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, MSG_CHAT_DELETED_FRIEND, pFriend->m_szName);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

				delete pFriend;
				return;
			}
			pFriend = (CChatFriend*)pFriend->GetNext();
		}
	}
}

//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ ’µΩ∑˛ŒÒ∆˜Õ®÷™ƒ≥¡ƒÃÏ∫√”—œ¬œﬂ
//-------------------------------------------------------------------------
void	KProtocolProcess::s2cChatFriendOffLine(BYTE* pMsg)
{
	CHAT_FRIEND_OFFLINE_SYNC	*pSync = (CHAT_FRIEND_OFFLINE_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cChat.GetMsgOffLine(pSync->m_dwID);
}

void	KProtocolProcess::s2cSyncAllSkill(BYTE * pMsg)
{
	SKILL_SEND_ALL_SYNC	* pSync = (SKILL_SEND_ALL_SYNC*) pMsg;
	int nSkillCount = (pSync->m_wProtocolLong - 2) / sizeof(SKILL_SEND_ALL_SYNC_DATA);
	int nNpcIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	KSkill * pOrdinSkill = NULL;
	Npc[nNpcIndex].m_SkillList.Clear();
	for (int i = 0; i < nSkillCount; i ++)
	{
		if (pSync->m_sAllSkill[i].SkillId)
		{
			
//			pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(pSync->m_sAllSkill[i].SkillId, pSync->m_sAllSkill[i].SkillLevel);
//			if (!pOrdinSkill) 
//              continue;

			Npc[nNpcIndex].m_SkillList.Add(
				pSync->m_sAllSkill[i].SkillId,
				pSync->m_sAllSkill[i].SkillLevel,
				pSync->m_sAllSkill[i].SkillExp
            );
		}
	}

}

void KProtocolProcess::SyncEnd(BYTE* pMsg)
{
	BYTE	SyncEnd = (BYTE)c2s_syncend;
	if (g_pClient)
		g_pClient->SendPackToServer(&SyncEnd, sizeof(BYTE));
	Player[CLIENT_PLAYER_INDEX].SetDefaultImmedSkill();
	g_DebugLog("[TRACE]SyncEnd to Server");
	CoreDataChanged(GDCNI_GAME_START, 0, 0);	
//	g_bPingReply = TRUE;
//	Player[CLIENT_PLAYER_INDEX].m_cChat.LoadTeamName(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
//	Player[CLIENT_PLAYER_INDEX].m_cChat.SaveTeamInfo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
//	Player[CLIENT_PLAYER_INDEX].m_cChat.LoadTakeChannelInfo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetInstantSpr(enumINSTANT_STATE_LOGIN);

	g_SubWorldSet.m_cMusic.Start(SubWorld[0].m_SubWorldID, SubWorld[0].m_dwCurrentTime, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode);

	Player[CLIENT_PLAYER_INDEX].m_cPK.Init();

}

void	KProtocolProcess::s2cTradeChangeState(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cTradeChangeState(pMsg);
}

void	KProtocolProcess::s2cGambleChangeState(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cGambleChangeState(pMsg);
}

void	KProtocolProcess::s2cNpcSetMenuState(BYTE* pMsg)
{
	NPC_SET_MENU_STATE_SYNC	*pState = (NPC_SET_MENU_STATE_SYNC*)pMsg;
	int		nNpc;
	if (pState->m_dwID == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID)
	{
		nNpc = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		if (pState->m_btState == PLAYER_MENU_STATE_TRADEOPEN)
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_STATE_OPEN);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		else if (pState->m_btState == PLAYER_MENU_STATE_NORMAL)
		{
			if (Npc[nNpc].GetMenuState() == PLAYER_MENU_STATE_TRADEOPEN)
			{
				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, MSG_TRADE_STATE_CLOSE);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			}
		}
	}
	else
	{
		nNpc = NpcSet.SearchID(pState->m_dwID);
		if (nNpc == 0)
			return;
	}
	if (pState->m_wLength > sizeof(NPC_SET_MENU_STATE_SYNC) - 1 - sizeof(pState->m_szSentence))
		Npc[nNpc].SetMenuState(pState->m_btState, pState->m_szSentence, pState->m_wLength - (sizeof(NPC_SET_MENU_STATE_SYNC) - 1 - sizeof(pState->m_szSentence)));
	else
		Npc[nNpc].SetMenuState(pState->m_btState);
}

void	KProtocolProcess::s2cTradeMoneySync(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cTradeMoneySync(pMsg);
}

void	KProtocolProcess::s2cTradeDecision(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cTradeDecision(pMsg);
	Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
}

void	KProtocolProcess::s2cGambleMoneySync(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cGambleMoneySync(pMsg);
}

void	KProtocolProcess::s2cGambleDecision(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cGambleDecision(pMsg);
	Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
}

void	KProtocolProcess::s2cTeamInviteAdd(BYTE * pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_cTeam.ReceiveInvite((TEAM_INVITE_ADD_SYNC*)pMsg);
}

void	KProtocolProcess::s2cTradePressOkSync(BYTE * pMsg)
{
	TRADE_STATE_SYNC	*pPress = (TRADE_STATE_SYNC*)pMsg;
	if(!pPress)
		return;
	Player[CLIENT_PLAYER_INDEX].m_cTrade.SetTradeState(pPress->m_btSelfLock, pPress->m_btDestLock, pPress->m_btSelfOk, pPress->m_btDestOk);
}

void	KProtocolProcess::s2cGamblePressOkSync(BYTE* pMsg)
{
	GAMBLE_STATE_SYNC* pPress = (GAMBLE_STATE_SYNC*)pMsg;
	if (!pPress)
		return;
	Player[CLIENT_PLAYER_INDEX].m_cTrade.SetGambleState(pPress->m_btSelfLock, pPress->m_btDestLock, pPress->m_btSelfOk, pPress->m_btDestOk);
}

void	KProtocolProcess::s2cDirectlyCastSkill(BYTE * pMsg)
{
	DWORD	dwNpcID;
	int		nSkillID, nSkillLevel;
	DWORD	MapX, MapY;
	
	dwNpcID = *(DWORD *)&pMsg[1];
	nSkillID = *(int *)&pMsg[5];
	nSkillLevel = *(int *)&pMsg[9];
	MapX = *(int *)&pMsg[13];
	MapY = *(int *)&pMsg[17];
	
	//µ±÷∏∂®ƒ≥∏ˆƒø±Í ±(MapX == -1),MapYŒ™ƒø±ÍµƒNpcdwID£¨–Ë“™◊™ªª≥…±æµÿµƒNpcIndex≤≈––
	if (MapX == -1)
	{
		if (MapY < 0 ) return;
		MapY = NpcSet.SearchID(MapY);
		if (MapY == 0)	return;
		if (Npc[MapY].m_RegionIndex < 0)
			return;
		int nX, nY;
		nX = Npc[MapY].m_MapX;
		nY = Npc[MapY].m_MapY;
		SubWorld[0].Map2Mps(Npc[MapY].m_RegionIndex, nX, nY, 0, 0, (int *)&MapX, (int *)&MapY);
	}
	
	int nIdx = NpcSet.SearchID(dwNpcID);
	
	//_ASSERT (nSkillID > 0 && nSkillLevel > 0);
	KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nSkillID, nSkillLevel);
	if (!pOrdinSkill) 
        return ;
	
    pOrdinSkill->Cast(nIdx, MapX, MapY);

	if(!pOrdinSkill->IsAura())
	{
		DWORD dwCastTime = 0;
		eSkillStyle eStyle = (eSkillStyle)pOrdinSkill->GetSkillStyle();
		if (eStyle == SKILL_SS_Missles 
			|| eStyle == SKILL_SS_Melee 
			|| eStyle == SKILL_SS_InitiativeNpcState 
			|| eStyle == SKILL_SS_PassivityNpcState)
		{
			dwCastTime = pOrdinSkill->GetDelayPerCast(Npc[nIdx].m_bRideHorse);
		}
		else
		{
			switch(eStyle)
			{
			case SKILL_SS_Thief:
				{
					dwCastTime = ((KThiefSkill*)pOrdinSkill)->GetDelayPerCast();
				}break;
			}
		}
		Npc[nIdx].m_SkillList.SetNextCastTime(nSkillID, SubWorld[0].m_dwCurrentTime, SubWorld[0].m_dwCurrentTime + dwCastTime);
	}
}

void	KProtocolProcess::s2cShowMsg(BYTE *pMsg)
{
	if (!pMsg)
		return;
	SHOW_MSG_SYNC	*pShowMsg = (SHOW_MSG_SYNC*)pMsg;

	switch (pShowMsg->m_wMsgID)
	{
	case enumMSG_ID_TEAM_KICK_One:
		{
			char	szName[32];
			KSystemMessage	sMsg;

			memset(szName, 0, sizeof(szName));
			memcpy(szName, pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));
			if (strcmp(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, szName) == 0)
			{
				sprintf(sMsg.szMessage, MSG_TEAM_BE_KICKEN);
				sMsg.eType = SMT_TEAM;
				sMsg.byConfirmType = SMCT_CLICK;
				sMsg.byPriority = 1;
			}
			else
			{
				sprintf(sMsg.szMessage, MSG_TEAM_KICK_ONE, szName);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
			}
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_DISMISS:
		{
			if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
				break;
			KSystemMessage	sMsg;
			if (Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure == TEAM_CAPTAIN)
			{
				sprintf(sMsg.szMessage, MSG_TEAM_DISMISS_CAPTAIN);
			}
			else
			{
				sprintf(sMsg.szMessage, MSG_TEAM_DISMISS_MEMBER, g_Team[0].m_szMemName[0]);
			}
			sMsg.eType = SMT_TEAM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_LEAVE:
		{
			if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
				break;
			KSystemMessage	sMsg;
			DWORD	dwID = *(DWORD*)(&pShowMsg->m_lpBuf);
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID == dwID)
			{
				sprintf(sMsg.szMessage, MSG_TEAM_LEAVE_SELF_MSG, g_Team[0].m_szMemName[0]);
			}
			else
			{
				sprintf(sMsg.szMessage, MSG_TEAM_LEAVE, "”–»À");
				for (int i = 0; i < MAX_TEAM_MEMBER; i++)
				{
					if ((DWORD)g_Team[0].m_nMember[i] == dwID)
					{
						sprintf(sMsg.szMessage, MSG_TEAM_LEAVE, g_Team[0].m_szMemName[i + 1]);
						break;
					}
				}
			}
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_REFUSE_INVITE:
		{
			char	szName[32];
			memset(szName, 0, sizeof(szName));
			memcpy(szName, &pShowMsg->m_lpBuf, pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));

			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TEAM_REFUSE_INVITE, szName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_SELF_ADD:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TEAM_SELF_ADD, g_Team[0].m_szMemName[0]);
			sMsg.eType = SMT_TEAM;
			sMsg.byConfirmType = SMCT_UI_TEAM;
			sMsg.byPriority = 3;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL:
		{
			int		nMember;
			DWORD	dwID = *(DWORD*)(&pShowMsg->m_lpBuf);
			nMember = g_Team[0].FindMemberID(dwID);
			if (nMember < 0)
				break;
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;

			sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN_FAIL1);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN_FAIL2, g_Team[0].m_szMemName[nMember + 1]);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL2:
		{
			int		nMember;
			DWORD	dwID = *(DWORD*)(&pShowMsg->m_lpBuf);
			nMember = g_Team[0].FindMemberID(dwID);
			if (nMember < 0)
				break;
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;

			sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN_FAIL1);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN_FAIL3);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_OBJ_CANNOT_PICKUP:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;

			strcpy(sMsg.szMessage, MSG_OBJ_CANNOT_PICKUP);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_OBJ_TOO_FAR:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			strcpy(sMsg.szMessage, MSG_OBJ_TOO_FAR);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_DEC_MONEY:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_DEC_MONEY, (int)pShowMsg->m_lpBuf);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TRADE_SELF_ROOM_FULL:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_SELF_ROOM_FULL);
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TRADE_DEST_ROOM_FULL:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_DEST_ROOM_FULL, Player[CLIENT_PLAYER_INDEX].m_cTrade.m_szDestName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TRADE_REFUSE_APPLY:
		{
			int	nIdx = NpcSet.SearchID(*((DWORD*)&pShowMsg->m_lpBuf));
			if (nIdx <= 0)
				return;
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_REFUSE_APPLY, Npc[nIdx].Name);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TRADE_TASK_ITEM:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_TASK_ITEM);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_GAMBLE_SELF_ROOM_FULL:
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_GAMBLE_SELF_ROOM_FULL);
		sMsg.eType = SMT_SYSTEM;
		sMsg.byConfirmType = SMCT_CLICK;
		sMsg.byPriority = 1;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	break;
	case enumMSG_ID_GAMBLE_DEST_ROOM_FULL:
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_GAMBLE_DEST_ROOM_FULL, Player[CLIENT_PLAYER_INDEX].m_cTrade.m_szDestName);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	break;
	case enumMSG_ID_GAMBLE_REFUSE_APPLY:
	{
		int	nIdx = NpcSet.SearchID(*((DWORD*)&pShowMsg->m_lpBuf));
		if (nIdx <= 0)
			return;
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_GAMBLE_REFUSE_APPLY, Npc[nIdx].Name);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	break;
	case enumMSG_ID_GAMBLE_TASK_ITEM:
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_GAMBLE_TASK_ITEM);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	break;
	case enumMSG_ID_ITEM_DAMAGED:
		{
			int nItemID = (int)pShowMsg->m_lpBuf;
			int nIdx = ItemSet.SearchID(nItemID);
			if (!nIdx)
				break;

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_ITEM_DAMAGED, Item[nIdx].GetName());
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_GET_ITEM:
		{
			DWORD	dwID = *(DWORD*)(&pShowMsg->m_lpBuf);
			
			int nItemIdx = ItemSet.SearchID(dwID);
			if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
				break;

			char	szName[128];
			KSystemMessage	sMsg;

			strcpy(szName, Item[nItemIdx].GetName());

			if (strlen(szName) >= sizeof(sMsg.szMessage) - strlen(MSG_ADD_ITEM))
				break;
			
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_ADD_ITEM, szName);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_MONEY_CANNOT_PICKUP:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;

			strcpy(sMsg.szMessage, MSG_MONEY_CANNOT_PICKUP);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_CANNOT_ADD_TEAM:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TEAM_CANNOT_CREATE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TARGET_CANNOT_ADD_TEAM:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TEAM_TARGET_CANNOT_ADD_TEAM);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_1:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_1);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_2:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_2);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_3:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_3);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_4:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_4);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_5:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_5);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_6:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_6);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_7:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_7);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_DEATH_LOSE_ITEM:
		{
			char	szName[32];
			memset(szName, 0, sizeof(szName));
			memcpy(szName, &pShowMsg->m_lpBuf, pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_DEATH_LOSE_ITEM, szName);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_TONG_RECRUIT_CLOSE:
		{
			char	szName[32];

			memset(szName, 0, sizeof(szName));
			memcpy(szName, pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, "Bang hÈi cÒa %s Æ„ng gia nhÀp thµnh vi™n", szName);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_REFUSE_ADD:
		{
			char	szName[32];

			memset(szName, 0, sizeof(szName));
			memcpy(szName, pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_REFUSE_ADD, szName);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_BE_KICK:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_BE_KICKED);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			KUiGameObjectWithName	sUi;
			strcpy(sUi.szName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
			sUi.nData = TONG_ACTION_DISMISS;
			sUi.nParam = 0;
			sUi.uParam = 0;
			sUi.szString[0] = 0;
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, 1);
		}
		break;
	case enumMSG_ID_TONG_LEAVE_SUCCESS:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_LEAVE_SUCCESS);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_LEAVE_FAIL:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_LEAVE_FAIL);
//			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_CHANGE_AS_MASTER:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_CHANGE_AS_MASTER);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			KUiGameObjectWithName	sUi;
			strcpy(sUi.szName, Player[CLIENT_PLAYER_INDEX].m_cTong.m_szMasterName);
			sUi.nData = TONG_ACTION_DEMISE;
			sUi.nParam = 0;
			sUi.uParam = 0;
			sUi.szString[0] = 0;
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, 1);
		}
		break;
	case enumMSG_ID_TONG_CHANGE_AS_MEMBER:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_CHANGE_AS_MEMBER);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			KUiGameObjectWithName	sUi;
			strcpy(sUi.szName, Player[CLIENT_PLAYER_INDEX].m_cTong.m_szMasterName);
			sUi.nData = TONG_ACTION_DEMISE;
			sUi.nParam = 0;
			sUi.uParam = 0;
			sUi.szString[0] = 0;
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, 1);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR:
		{
			char	szName[32];

			memset(szName, 0, sizeof(szName));
			memcpy(szName, pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR, szName);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR1:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR1);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR2:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR2);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR3:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR3);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR4:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR4);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR5:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR5);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR6:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR6);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR7:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR7);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_FIGHT_MODE_ERROR1:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_FIGHT_MODE_ERROR1);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_FIGHT_MODE_ERROR2:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_FIGHT_MODE_ERROR2);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_FIGHT_MODE_ERROR3:
		{
			int	nIdx = NpcSet.SearchID(*((DWORD*)&pShowMsg->m_lpBuf));
			if (nIdx <= 0)
				return;

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_FIGHT_MODE_ERROR3, Npc[nIdx].Name);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_SHOP_NO_ROOM:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_ROOM);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
		}
		break;
	case enumMSG_ID_SHOP_NO_MONEY:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_MONEY);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
		}
		break;
	case enumMSG_ID_SHOP_NO_EXTPOINT:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_EXTPOINT);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
		}
		break;
	case enumMSG_ID_SHOP_NO_FUYUAN:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_FUYUAN);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
		}
		break;
	case enumMSG_ID_SHOP_NO_REPUTE:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_REPUTE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
		}
		break;
	case enumMSG_ID_SHOP_NO_ACCUM:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_ACCUM);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
		}
		break;
	case enumMSG_ID_SHOP_NO_HONOR:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_HONOR);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
		}
		break;
	case enumMSG_ID_SHOP_NO_RESPECT:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_RESPECT);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
		}
		break;
	case enumMSG_ID_FUNCTION_CHAT_FORBIDDENED:
		{
			time_t rawtime=(int)*((DWORD*)&pShowMsg->m_lpBuf)+1451581200;
			struct tm * timeinfo = localtime(&rawtime);

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			strftime(sMsg.szMessage, sizeof(sMsg.szMessage), MSG_NPC_NOT_PAY_FOR_SPEECH, timeinfo);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_FUNCTION_FOBIDDENED:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_FUNCTION_FOBIDDENED);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_FUNCTION_LOCKED:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_FUNCTION_LOCKED);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_LOCK_NOT_TRADE:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_LOCK_NOT_TRADE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_LOCK_NOT_SETPRICE:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_LOCK_NOT_SETPRICE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_LOCK_NOT_THROW:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_LOCK_NOT_THROW);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_ITEM_TOWNPORTAL:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_NPC_NOT_USE_TOWNPORTAL);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_ITEM_CANTREPAIR:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_BROKEN_CANTREPAIR);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_ITEM_SAME_IMMEDIATE:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_ITEM_SAME_IN_IMMEDIATE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_SKILL_SAME_IMMEDIATE:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SKILL_SAME_IN_IMMEDIATE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_ITEM_CANTPUT_IMMEDIATE:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_ITEM_PUT_IN_IMMEDIATE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_HORSE_CANT_SWITCH1:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_CANT_SWITCH_HORSE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_HORSE_CANT_SWITCH2:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_CANT_SWITCH_HORSE_SIT);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_CHATROOM_JOIN:
		{
			char	szName[32];
			KSystemMessage	sMsg;

			memset(szName, 0, sizeof(szName));
			memcpy(szName, pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));
			sprintf(sMsg.szMessage, MSG_CHATROOM_JOIN, szName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			CoreDataChanged(GDCNI_CHATROOM_UPDATE_INTERFACE, 0, 0);
		}
		break;
	case enumMSG_ID_CHATROOM_LEAVE:
		{
			char	szName[32];
			KSystemMessage	sMsg;

			memset(szName, 0, sizeof(szName));
			memcpy(szName, pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));
			sprintf(sMsg.szMessage, MSG_CHATROOM_LEAVE, szName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			CoreDataChanged(GDCNI_CHATROOM_UPDATE_INTERFACE, 0, 0);
		}
		break;
	case enumMSG_ID_CHATROOM_BEKICK:
		{
			char	szName[32];
			KSystemMessage	sMsg;

			memset(szName, 0, sizeof(szName));
			memcpy(szName, pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));
			sprintf(sMsg.szMessage, MSG_CHATROOM_BEKICK, szName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			CoreDataChanged(GDCNI_CHATROOM_UPDATE_INTERFACE, 0, 0);
		}
		break;
	case enumMSG_ID_CHATROOM_OPENGAME:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_CHATROOM_OPENGAME, (int)*((DWORD*)&pShowMsg->m_lpBuf));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			CoreDataChanged(GDCNI_CHATROOM_UPDATE_INTERFACE, 0, 0);
		}
		break;
	case  enumMSG_ID_CHATROOM_REVERSE:
		{
			char	szName[32];
			KSystemMessage	sMsg;

			memset(szName, 0, sizeof(szName));
			memcpy(szName, pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));
			sprintf(sMsg.szMessage, MSG_CHATROOM_REVERSE, szName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			CoreDataChanged(GDCNI_CHATROOM_UPDATE_INTERFACE, 0, 0);
		}
		break;
	case enumMSG_ID_NPC_RENASCENCE_SOMEONE:
		{
			KSystemMessage sMsg;
			sMsg.byConfirmType = SMCT_UI_RENASCENCE;
			sMsg.byParamSize = 0;
			sMsg.byPriority = 0;
			sMsg.eType = SMT_PLAYER;
			int	nIdx = NpcSet.SearchID(*((DWORD*)&pShowMsg->m_lpBuf));
			if (nIdx)
				sprintf(sMsg.szMessage, MSG_NPC_RENASCENCE_SOMEONE, Npc[nIdx].Name);
			else
				sprintf(sMsg.szMessage, MSG_NPC_RENASCENCE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_NPC_RENASCENCE:
		{
			KSystemMessage sMsg;
			sMsg.byConfirmType = SMCT_UI_RENASCENCE;
			sMsg.byParamSize = 0;
			sMsg.byPriority = 0;
			sMsg.eType = SMT_PLAYER;
			sprintf(sMsg.szMessage, MSG_NPC_RENASCENCE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	default:
		break;
	}
}

void	KProtocolProcess::SyncStateEffect(BYTE* pMsg)
{
	STATE_EFFECT_SYNC*	pSync = (STATE_EFFECT_SYNC *)pMsg;

	int nDataNum = MAX_SKILL_STATE - (sizeof(STATE_EFFECT_SYNC) - pSync->m_wLength) / sizeof(KMagicAttrib);
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetStateSkillEffect(Player[CLIENT_PLAYER_INDEX].m_nIndex, pSync->m_dwSkillID, pSync->m_nLevel, pSync->m_MagicAttrib, nDataNum, pSync->m_nTime, pSync->m_bOverLook);
}

void	KProtocolProcess::s2cTradeApplyStart(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cTradeApplyStart(pMsg);
}

void	KProtocolProcess::s2cGambleApplyStart(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cGambleApplyStart(pMsg);
}

void KProtocolProcess::s2cPlayerItemTimeSync(BYTE* pMsg)
{
	PLAYER_ITEM_TIME_SYNC *pSync = (PLAYER_ITEM_TIME_SYNC*)pMsg;
	if(pSync && pSync->m_ItemIdx > 0 && pSync->m_time)
	{
		int nIdx = ItemSet.SearchID(pSync->m_ItemIdx);
		Item[nIdx].SetExpTime(pSync->m_time, 0, 0, 0);
	}
}

void KProtocolProcess::SyncMaskLock(BYTE* pMsg)
{
	NPC_SIT_SYNC *pInfo = (NPC_SIT_SYNC *)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_ItemList.SetMaskLock(pInfo->ID);
}

void	KProtocolProcess::s2cTaskValueSync(BYTE* pMsg)
{
	TASK_VALUE_SYNC	*pValue = (TASK_VALUE_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(pValue->nTaskId,pValue->nTaskValue);
	CoreDataChanged(GDCNI_TASK_VALUE_UPDATE, pValue->nTaskId, (int)pValue->nTaskValue);	// [TaskGuide]
}

void KProtocolProcess::s2cSetMissionData(BYTE* pMsg)
{
	PLAYER_MISSION_DATA	*pData = (PLAYER_MISSION_DATA*)pMsg;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nMissionGroup = pData->m_nMissionGroup;
	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
}

void KProtocolProcess::s2cReturnCityOwnTong(BYTE* pMsg)
{
	RETURN_CITY_OWN_TONG	*pData = (RETURN_CITY_OWN_TONG*)pMsg;
	CoreDataChanged(GDCNI_RETURN_CITY_OWN_TONG, 0, (unsigned int)pData->szTongName);
}

void KProtocolProcess::s2cPlayerLoginReplay(BYTE* pMsg) //fix by phong ki“u chuy”n gs bﬁ m t skill
{
	PLAYER_LOGIN_REPLAY	*pData = (PLAYER_LOGIN_REPLAY*)pMsg;
	CoreDataChanged(GDCNI_PLAYER_LOGIN_REPLAY, pData->m_nPlayerIdx, 0); 
}

void KProtocolProcess::s2cOpenTrembleItem(BYTE* pMsg)
{	
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return;
	
	OPEN_TREMBLEITEM *pFinish = (OPEN_TREMBLEITEM *)pMsg;
	CoreDataChanged(GDCNI_OPEN_TREMBLE_ITEM, pFinish->m_nType, NULL);
}

void KProtocolProcess::s2cOpenCompoundItem(BYTE* pMsg)
{	
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return;
	CoreDataChanged(GDCNI_OPEN_COMPOUND_ITEM, NULL, NULL);
}

void KProtocolProcess::s2cSyncRankData(BYTE* pMsg)
{
	PLAYER_MISSION_RANKDATA* pInfo = (PLAYER_MISSION_RANKDATA *)pMsg;
	memcpy(&Player[CLIENT_PLAYER_INDEX].m_MissionData, &pInfo->SelfData, sizeof(pInfo->SelfData));
}

void KProtocolProcess::s2cSyncRankData2(BYTE* pMsg)
{
	PLAYER_MISSION_RANKDATA2* pInfo = (PLAYER_MISSION_RANKDATA2 *)pMsg;
	int i = pInfo->nRankIndex;
	if(i < 0 || i > MISSION_STATNUM)
	{
		i = MISSION_STATNUM;
	}
	memcpy(&Player[CLIENT_PLAYER_INDEX].m_MissionRank[i], &pInfo->MissionRank, sizeof(pInfo->MissionRank));
}


#ifndef _SERVER
void KProtocolProcess::s2cShowDamage(BYTE* pMsg)
{
	DAMAGESHOW* pDamage = (DAMAGESHOW*)pMsg;
	int	receiverNpcIndex = NpcSet.SearchID(pDamage->dwReceiver);
	int casterNpcIndex = NpcSet.SearchID(pDamage->dwLauncher);

	if (receiverNpcIndex > 0)
	{
		//TODO
		//Npc[nIdx].SetBlood(pDamage->nDamage, pDamage->enType);
		//Npc[receiverNpcIndex].GetCombatInfoShower().AddInfo(casterNpcIndex, pDamage->nDamage, pDamage->SkillId, (COMBAT_INFO_TYPE)pDamage->enType, (TRUE == pDamage->IsCrit ? true : false));
		int nHeight = Npc[receiverNpcIndex].GetNpcPate();
		Npc[receiverNpcIndex].SetBlood2(pDamage);
	}
}
void KProtocolProcess::s2cSyncMagic(BYTE* pMsg)
{
	ITEM_SYNC_MAGIC* pItemSync = (ITEM_SYNC_MAGIC*)pMsg;
	DWORD nIdx = ItemSet.SearchID(pItemSync->m_dwID);
	if (nIdx > 0 && nIdx < MAX_ITEM)
	{
		memcpy(Item[nIdx].m_GeneratorParam.nGeneratorLevel, pItemSync->m_MagicLevel, sizeof(int) * MAX_ITEM_MAGICLEVEL);
		Item[nIdx].SetMagicAttrib((KItemNormalAttrib*)pItemSync->m_MagicAttrib);
	}
}

void KProtocolProcess::s2cSyncMeridian(BYTE* pMsg)
{
	MERIDIAN_SYNC* pMeridianSync = (MERIDIAN_SYNC*)pMsg;
	if (pMeridianSync->ProtocolType != s2c_syncmeridian)
		return;
	Player[CLIENT_PLAYER_INDEX].m_cMeridian.setMeridian(pMeridianSync->m_nMeridian);
	Player[CLIENT_PLAYER_INDEX].UpdataCurData();
	CoreDataChanged(GDCNI_PLAYER_MERIDIAN_SYNC, (unsigned int)pMeridianSync->m_nMeridian, pMeridianSync->ProtocolType);
}

void KProtocolProcess::s2cSyncBauCuaResult(BYTE* pMsg)
{
	BAUCUA_RESULT_SYNC* pBauCuaResultSync = (BAUCUA_RESULT_SYNC*)pMsg;
	if (pBauCuaResultSync->ProtocolType != s2c_syncbaucuaresult && pBauCuaResultSync->ProtocolType != s2c_syncbaucuainfo)
		return;
	CoreDataChanged(GDCNI_PLAYER_BAUCUA_RESULT_SYNC, (unsigned int)pBauCuaResultSync, pBauCuaResultSync->ProtocolType);
}
#endif

void KProtocolProcess::s2cOpenQuestFinishDlg(BYTE* pMsg)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return;

	QUEST_FINISH_DLG_SYNC *pFinish = (QUEST_FINISH_DLG_SYNC *)pMsg;
	CoreDataChanged(GDCNI_FINISH_QUEST_DLG, (unsigned int)pFinish->m_szNotice, pFinish->m_bType);
	// [DaTau] bao cho auto: cua so 3 ruong dang mo
	g_sDTCap.nFinType = pFinish->m_bType;
	++g_sDTCap.uFinSeq;
}

void	KProtocolProcess::s2cImageNpcSync(BYTE* pMsg)
{
	IMAGENPC_VALUE_SYNC	*pValue = (IMAGENPC_VALUE_SYNC*)pMsg;
	switch (pValue->m_nType)
	{
		case 0:
			Player[CLIENT_PLAYER_INDEX].SetImageNpcId(pValue->m_nValue);
			break;
		case 1:
			//Player[CLIENT_PLAYER_INDEX].SetChangeFace(pValue->m_nValue);
			break;
		default:
			break;
	}
}

void KProtocolProcess::s2cSetObstacle(BYTE* pMsg) //#Set VÀt C∂n
{
	S2C_SET_OBSTACLE	*setibsCmd = (S2C_SET_OBSTACLE *)pMsg;
	
	int		nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[0].Mps2Map(setibsCmd->pMapX, setibsCmd->pMapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	//if (nRegion < 0)
	//	return;
	//SubWorld[0].m_Region[nRegion].SetObstacle(setibsCmd->pValue, setibsCmd->pMapX, setibsCmd->pMapY);
	//g_ScenePlace.SetObstacleInfoMin(setibsCmd->pValue, nMapX, nMapY, nOffX, nOffY);
	if(setibsCmd->pValue == -1)
		g_ScenePlace.PaintMapPoint(setibsCmd->pMapX, setibsCmd->pMapY);
	if (setibsCmd->pValue == -2)//send radius pubg
		g_ScenePlace.PaintPUBGCircle(0, 0, setibsCmd->pMapX);
}

void KProtocolProcess::s2cInPutBox(BYTE* pMsg)
{
	S2C_INPUT_BOX	*InPutBoxCmd = (S2C_INPUT_BOX *)pMsg;
	switch (InPutBoxCmd->nType)
	{
		case 1:
			CoreDataChanged(GDCNI_OPEN_INPUT, (unsigned int)InPutBoxCmd->Value, (unsigned int)InPutBoxCmd->Value1);
			break;
		case 2:
			CoreDataChanged(GDCNI_OPEN_INPUT2, (unsigned int)InPutBoxCmd->Value, (unsigned int)InPutBoxCmd->Value1);
			break;
		default:
			break;
	}
}

void KProtocolProcess::s2cPlayerSync_MA(BYTE* pMsg)
{
	S2C_PLAYER_SYNC_M_A *pSync = (S2C_PLAYER_SYNC_M_A*)pMsg;
	switch (pSync->nType)
	{
		case enumS2C_PLAYERSYNC_ID_EXIT:
			//CoreDataChanged(GDCNI_EXIT_GAME, NULL, NULL);
			break;
		case enumS2C_PLAYERSYNC_ID_GIVE:
			//CoreDataChanged(GDCNI_GIVE, NULL, NULL);
			break;
		case enumS2C_PLAYERSYNC_ID_EQUIPEXPAND:
			//Player[CLIENT_PLAYER_INDEX].SetEquipExpandTime(*(int*)(&pSync->m_lpBuf));
			break;
		case enumS2C_PLAYERSYNC_ID_EXPANDBOX:
			//Player[CLIENT_PLAYER_INDEX].SetExpandBoxNum(*(int*)(&pSync->m_lpBuf));
			break;
		case enumS2C_PLAYERSYNC_ID_LOCKSTATE:
			//Player[CLIENT_PLAYER_INDEX].SetLockState(*(BOOL*)(&pSync->m_lpBuf));
			break;
		case enumS2C_PLAYERSYNC_ID_PROPPOINT:
			Player[CLIENT_PLAYER_INDEX].m_nAttributePoint = pSync->nPoint;
			break;
		case enumS2C_PLAYERSYNC_ID_MAGICPOINT:
			Player[CLIENT_PLAYER_INDEX].m_nSkillPoint = pSync->nPoint;
			CoreDataChanged(GDCNI_FIGHT_SKILL_POINT, 0, Player[CLIENT_PLAYER_INDEX].m_nSkillPoint);
			break;
		case enumS2C_PLAYERSYNC_ID_RANKDATA:
			//CoreDataChanged(GDCNI_RANKDATA, NULL, NULL);
			break;
		case enumS2C_PLAYERSYNC_ID_ENCHASE:
			//CoreDataChanged(GDCNI_ENCHASE, NULL, NULL);
			break;
		case enumS2C_PLAYERSYNC_ID_INPUT:
			//CoreDataChanged(GDCNI_INPUT, *(int*)(&pSync->m_lpBuf), 0);
			break;
		case enumS2C_PLAYERSYNC_ID_MASKFEATURE:
			//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SwitchMaskFeature();
			break;
	}
}	

void KProtocolProcess::s2cPlayerSync(BYTE* pMsg)
{
	S2C_PLAYER_SYNC* pSync = (S2C_PLAYER_SYNC*)pMsg;
	switch (pSync->m_wMsgID)
	{
	case enumS2C_PLAYERSYNC_ID_EXIT:
		CoreDataChanged(GDCNI_EXIT_GAME, NULL, NULL);
		break;
	//case enumS2C_PLAYERSYNC_ID_GIVE:
	//	CoreDataChanged(GDCNI_GIVE, NULL, NULL);
	//	break;
	//case enumS2C_PLAYERSYNC_ID_EQUIPEXPAND:
	//	Player[CLIENT_PLAYER_INDEX].SetEquipExpandTime(*(int*)(&pSync->m_lpBuf));
	//	break;
	//case enumS2C_PLAYERSYNC_ID_EXPANDBOX:
	//	Player[CLIENT_PLAYER_INDEX].SetExpandBoxNum(*(int*)(&pSync->m_lpBuf));
	//	break;
	//case enumS2C_PLAYERSYNC_ID_LOCKSTATE:
	//	Player[CLIENT_PLAYER_INDEX].SetLockState(*(BOOL*)(&pSync->m_lpBuf));
	//	break;
	case enumS2C_PLAYERSYNC_ID_PROPPOINT:
		Player[CLIENT_PLAYER_INDEX].m_nAttributePoint = *(int*)(&pSync->m_lpBuf);
		break;
	case enumS2C_PLAYERSYNC_ID_MAGICPOINT:
		Player[CLIENT_PLAYER_INDEX].m_nSkillPoint = *(int*)(&pSync->m_lpBuf);
		CoreDataChanged(GDCNI_FIGHT_SKILL_POINT, 0, Player[CLIENT_PLAYER_INDEX].m_nSkillPoint);
		break;
	case enumS2C_PLAYERSYNC_ID_RANKDATA:
		CoreDataChanged(GDCNI_RANKDATA, NULL, NULL);
		break;
	//case enumS2C_PLAYERSYNC_ID_ENCHASE:
	//	CoreDataChanged(GDCNI_ENCHASE, NULL, NULL);
	//	break;
	//case enumS2C_PLAYERSYNC_ID_INPUT:
	//	CoreDataChanged(GDCNI_INPUT, *(int*)(&pSync->m_lpBuf), 0);
	//	break;
	case enumS2C_PLAYERSYNC_ID_MASKFEATURE:
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SwitchMaskFeature();
		break;
	}
}

//-----------------------------------------------------------------
// Thuc hien packet mo hop thoai tra vat pham nhiem vu
//-----------------------------------------------------------------
void KProtocolProcess::OpenAffairBox(BYTE* pMsg)
{
	S2C_GIVE_BOX	*GiveBoxCmd = (S2C_GIVE_BOX *)pMsg;
	switch(GiveBoxCmd->nType)
	{
	case 1:
		{
			KUiGiveBox	pInfo;
			strcpy(pInfo.szTitle, GiveBoxCmd->Value); 
			strcpy(pInfo.szInitString, GiveBoxCmd->Value1); 
			strcpy(pInfo.szAction1, GiveBoxCmd->Value2); 
			CoreDataChanged(GDCNI_OPEN_AFFAIR_BOX, (unsigned int)&pInfo, NULL);
			// [DaTau] give-box mo, luu ten ham nop
			g_StrCpyLen(g_sDTCap.szBoxFunc, GiveBoxCmd->Value2, sizeof(g_sDTCap.szBoxFunc));
			g_sDTCap.nBoxOpen = 1;
			++g_sDTCap.uBoxSeq;
		}
		break;
	case 2:
		CoreDataChanged(GDCNI_END_AFFAIR_BOX, NULL, NULL);
		g_sDTCap.nBoxOpen = 0; // [DaTau] give-box dong
		break;
	default:
		break;
	}
}

void KProtocolProcess::s2cTimeBox(BYTE* pMsg)
{
	S2C_TIME_BOX	*TimeBoxCmd = (S2C_TIME_BOX *)pMsg;
	KUiTimeBoxInfo	pInfo;
	strcpy(pInfo.szTitle, TimeBoxCmd->Value); 
	pInfo.nTime = TimeBoxCmd->Value1;
	strcpy(pInfo.szAction, TimeBoxCmd->Value2); 
	CoreDataChanged(GDCNI_OPEN_TIME_BOX, (unsigned int)&pInfo, 0);
}

void KProtocolProcess::s2cTalkEx(BYTE* pMsg)
{
	S2C_TALK_EX	*TalkExCmd = (S2C_TALK_EX *)pMsg;
	CoreDataChanged(GDCNI_OPEN_TALK_EX, (unsigned int)TalkExCmd->Value, (unsigned int)TalkExCmd->Value1);
}

//=====================================

void	KProtocolProcess::s2cItemAutoMove(BYTE* pMsg)
{
	ITEM_AUTO_MOVE_SYNC	*pSync = (ITEM_AUTO_MOVE_SYNC*)pMsg;
	ItemPos	sSrc, sDest;
	sSrc.nPlace = pSync->m_btSrcPos;
	sSrc.nX = pSync->m_btSrcX;
	sSrc.nY = pSync->m_btSrcY;
	sDest.nPlace = pSync->m_btDestPos;
	sDest.nX = pSync->m_btDestX;
	sDest.nY = pSync->m_btDestY;
	
	Player[CLIENT_PLAYER_INDEX].m_ItemList.AutoMoveItem(sSrc, sDest);
}

void KProtocolProcess::FinishedItemExchange(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
}


void KProtocolProcess::s2cExtend(BYTE* pMsg)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)(pMsg + sizeof(tagExtendProtoHeader));

	if (pHeader->ProtocolFamily == pf_playercommunity)
	{
		if (pHeader->ProtocolID == playercomm_s2c_notifychannelid)
		{
			PLAYERCOMM_NOTIFYCHANNELID* pNChann = (PLAYERCOMM_NOTIFYCHANNELID*)pHeader;

			l_pDataChangedNotifyFunc->NotifyChannelID(pNChann->channel, pNChann->channelid, pNChann->cost);
		}
	}
}

void KProtocolProcess::s2cExtendChat(BYTE* pMsg)
{
	tagExtendProtoHeader* pExHdr = (tagExtendProtoHeader*)pMsg;
	void* pExPckg = pExHdr + 1;
	BYTE protocol = *(BYTE*)(pExPckg);

	if (protocol == chat_someonechat)//Chat mÀt
	{
		CHAT_SOMEONECHAT_SYNC* pCscSync = (CHAT_SOMEONECHAT_SYNC*)pExPckg;
		l_pDataChangedNotifyFunc->MSNMessageArrival(
			pCscSync->someone, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,
			(const char*)(pCscSync + 1), pCscSync->sentlen, true);
	}
	else if (protocol == chat_channelchat)//Chat k™nh
	{
		CHAT_CHANNELCHAT_SYNC* pCccSync = (CHAT_CHANNELCHAT_SYNC*)pExPckg;
		// [DaTau] chup thong diep 'He Thong' (tien do nhat cuon / manh SHXT).
		// (20/08 r3) ghi VONG 4 KHE - 2 tin trong cung tick khong de mat tin truoc.
		bool bDTSapMap = false;	// (r5f) chi TIN DANH BA that su moi bi an khoi khung chat
		if (!strcmp(pCccSync->someone, "H÷ ThËng"))
		{
			int nDTLen = pCccSync->sentlen;
			if (nDTLen > (int)sizeof(g_sDTCap.aMsg[0]) - 1)
				nDTLen = (int)sizeof(g_sDTCap.aMsg[0]) - 1;
			// (r5e) "[SapMap] ..." = danh ba sap server tra ve - kenh du lieu
			// rieng cho auto: khong vao vong khe (khoi de tin tien do) va khong
			// hien len khung chat.
			// (r5f - phan bien) ">= 8": thanh KHONG CO SAP thi server tra dung
			// "[SapMap]" 8 byte - ">" lam goi do bi vut, nhanh "0 sap -> qua thanh
			// ke" thanh ma chet va bot van di tuan mu. memcmp (khong strncmp) vi
			// payload KHONG ket thuc NUL - da chan do dai o tren nen khong doc lo.
			if (nDTLen >= 8 && !memcmp((const char*)(pCccSync + 1), "[SapMap]", 8))
			{
				bDTSapMap = true;
				memcpy(g_sDTCap.szSapMap, (const char*)(pCccSync + 1), nDTLen);
				g_sDTCap.szSapMap[nDTLen] = 0;
				++g_sDTCap.uSapMapSeq;
			}
			else if (nDTLen > 0)
			{
				char* pDTKhe = g_sDTCap.aMsg[(g_sDTCap.uMsgSeq + 1) & 3];
				memcpy(pDTKhe, (const char*)(pCccSync + 1), nDTLen);
				pDTKhe[nDTLen] = 0;
				++g_sDTCap.uMsgSeq;
				// (r4) tin TIEN DO ("tong cong") vao kenh rieng - spam khong de duoc
				if (strstr(pDTKhe, "tÊng cÈng") != 0)
				{
					memcpy(g_sDTCap.szTien, pDTKhe, nDTLen + 1);
					++g_sDTCap.uTienSeq;
				}
			}
		}
		// (r5f - phan bien) TRUOC day chan bang strncmp o day nen NGUOI CHOI go
		// tin bat dau bang "[SapMap]" o bat ky kenh nao cung bi nuot voi moi
		// client. Nay chi an dung goi danh ba cua He Thong.
		if (!bDTSapMap)
		{
			l_pDataChangedNotifyFunc->ChannelMessageArrival(
				pCccSync->channelid, pCccSync->someone,
				(const char*)(pCccSync + 1), pCccSync->sentlen, true);
		}
	}
	else if (protocol == chat_feedback)
	{
		////X
		CHAT_FEEDBACK* pCfb = (CHAT_FEEDBACK*)pExPckg;
		DWORD* pChannelid = (DWORD*)(pCfb + 1);

		if (*pChannelid == -1)
		{//someone
			char* pDstName = (char*)(pChannelid + 1);
			BYTE* pSentlen = (BYTE*)(pDstName + _NAME_LEN);
			void* pSent = pSentlen + 1;
			
			l_pDataChangedNotifyFunc->MSNMessageArrival(
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, pDstName,
				(const char*)pSent, *pSentlen, pCfb->code != codeFail);
		}
		else
		{//channel
			BYTE* pSentlen = (BYTE*)(pChannelid + 1);
			void* pSent = pSentlen + 1;

			l_pDataChangedNotifyFunc->ChannelMessageArrival(
				*pChannelid, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,
				(const char*)pSent, *pSentlen, pCfb->code != codeFail);
		}
		////X
	}
}


static BOOL sParseUGName(const std::string& name, std::string* pUnit, std::string* pGroup)
{
	static const char char_split = '\n';

	size_t pos = name.find(char_split);
	if (pos == name.npos)
	{
		if (pUnit)
			pUnit->resize(0);
		if (pGroup)
			pGroup->assign(name);
	}
	else
	{
		std::string::const_iterator itSplit = name.begin() + pos;

		if (pUnit)
			pUnit->assign(name.begin(), itSplit);
		if (pGroup)
			pGroup->assign(itSplit + 1, name.end());
	}

	return TRUE;
}

void KProtocolProcess::s2cExtendFriend(BYTE* pMsg)
{
	tagExtendProtoHeader* pExHdr = (tagExtendProtoHeader*)pMsg;
	void* pExPckg = pExHdr + 1;

	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)(pExPckg);

	if (pHeader->ProtocolFamily == pf_tong)
	{
		_ASSERT(FALSE);
		//tong message
		//...
	}
	else if (pHeader->ProtocolFamily == pf_friend)
	{
		if (pHeader->ProtocolID == friend_c2c_askaddfriend)
		{
			ASK_ADDFRIEND_SYNC* pAafSync = (ASK_ADDFRIEND_SYNC*)pHeader;
			l_pDataChangedNotifyFunc->FriendInvite(pAafSync->srcrole);
		}
		else if (pHeader->ProtocolID == friend_c2c_repaddfriend)
		{
			REP_ADDFRIEND_SYNC* pRafSync = (REP_ADDFRIEND_SYNC*)pHeader;
			l_pDataChangedNotifyFunc->AddFriend(pRafSync->srcrole, pRafSync->answer);
		}
		else if (pHeader->ProtocolID == friend_s2c_repsyncfriendlist)
		{
			REP_SYNCFRIENDLIST* pRsfl = (REP_SYNCFRIENDLIST*)pHeader;
			char* pGroupTag = (char*)(pRsfl + 1);
			if (*pGroupTag != specGroup)
				goto on_error;
			{{
			char* pGroup = (char*)(pGroupTag + 1);
on_newgroup:
			std::string theUnit, theGroup;
			sParseUGName(std::string(pGroup), &theUnit, &theGroup);

			for (char* pRoleTag = pGroup + strlen(pGroup) + 1; ; )
			{
				if (*pRoleTag == specOver)
					goto on_over;
				else if (*pRoleTag == specGroup)
				{
					pGroup = pRoleTag + 1;
					goto on_newgroup;
				}
				else if (*pRoleTag == specRole)
				{
					char* pState = pRoleTag + 1;
					char* pRole = pState + 1;
					l_pDataChangedNotifyFunc->FriendInfo(pRole, (char*)theUnit.c_str(), (char*)theGroup.c_str(), (BYTE)*pState);
					pRoleTag = pRole + strlen(pRole) + 1;
					continue;
				}
				else
					goto on_error;
			}
			}}
on_error:
			_ASSERT(FALSE);
on_over:
			0;
		}
		else if (pHeader->ProtocolID == friend_s2c_friendstate)
		{
			FRIEND_STATE* pFs = (FRIEND_STATE*)pHeader;
			for (char* pRole = (char*)(pFs + 1); *pRole; pRole += strlen(pRole) + 1)
				l_pDataChangedNotifyFunc->FriendStatus(pRole, pFs->state);
		}
		else if (pHeader->ProtocolID == friend_s2c_syncassociate)
		{
			FRIEND_SYNCASSOCIATE* pFsa = (FRIEND_SYNCASSOCIATE*)pHeader;

			char* szGroup = (char*)(pFsa + 1);
			std::string group(szGroup);

			std::string theUnit, theGroup;
			sParseUGName(group, &theUnit, &theGroup);
			_ASSERT(theGroup.empty());

			for (char* szRole = szGroup + group.size() + 1; *szRole; szRole += strlen(szRole) + 1)
				l_pDataChangedNotifyFunc->AddPeople((char*)theUnit.c_str(), szRole);
		}
	}
}


void KProtocolProcess::s2cExtendTong(BYTE* pMsg)
{
	S2C_TONG_HEAD	*pHead = (S2C_TONG_HEAD*)pMsg;
	switch (pHead->m_btMsgId)
	{
	case enumTONG_SYNC_ID_CREATE_FAIL:
		{
			TONG_CREATE_FAIL_SYNC *pFail = (TONG_CREATE_FAIL_SYNC*)pMsg;

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;

			sprintf(sMsg.szMessage, "Thµnh lÀp bang hÈi th t bπi!");
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			switch (pFail->m_btFailId)
			{
			case enumTONG_CREATE_ERROR_ID1:		// Player[m_nPlayerIndex].m_nIndex <= 0
				break;
			case enumTONG_CREATE_ERROR_ID2:		// Ωª“◊π˝≥Ã÷–
				break;
			case enumTONG_CREATE_ERROR_ID3:		// ∞Ôª·√˚Œ Ã‚
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR09);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID4:		// ∞Ôª·’Û”™Œ Ã‚
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR02);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID5:		// “—æ≠ «∞Ôª·≥…‘±
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR03);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID6:		// ◊‘º∫µƒ’Û”™Œ Ã‚
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR04);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID7:		// µ»º∂Œ Ã‚
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR05);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID8:		// «ÆŒ Ã‚
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR10);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID9:		// ◊È∂”≤ªƒ‹Ω®∞Ôª·
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR08);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID10:	// ∞Ôª·ƒ£øÈ≥ˆ¥Ì
				break;
			case enumTONG_CREATE_ERROR_ID11:	// √˚◊÷◊÷∑˚¥Æ≥ˆ¥Ì
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR11);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID12:	// √˚◊÷◊÷∑˚¥Æπ˝≥§
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR11);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID13:	// ∞Ôª·Õ¨√˚¥ÌŒÛ
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR11);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID14:	// ∞Ôª·≤˙…˙ ß∞‹
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR12);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			}
		}
		break;
	case enumTONG_SYNC_ID_TRANSFER_ADD_APPLY:
		{
			TONG_APPLY_ADD_SYNC	*pApply = (TONG_APPLY_ADD_SYNC*)pMsg;
			char	szName[32];
			DWORD	dwNameID;
			int		nPlayerIdx;

			memset(szName, 0, sizeof(szName));
			memcpy(szName, pApply->m_szName, pApply->m_wLength + 1 + sizeof(pApply->m_szName) - sizeof(TONG_APPLY_ADD_SYNC));
			dwNameID = g_FileName2Id(szName);
			nPlayerIdx = pApply->m_nPlayerIdx;

			// ∏¯ΩÁ√Ê∑¢œ˚œ¢£¨ ’µΩ…Í«Î£¨ «∑ÒÕ¨“‚
			KSystemMessage	sMsg;
			//sprintf(sMsg.szMessage, "%s…Í«Îº”»Î∞Ôª·£° enumTONG_SYNC_ID_TRANSFER_ADD_APPLY", szName);
			sprintf(sMsg.szMessage, "%s xin gia nhÀp bang", szName);
			sMsg.eType = SMT_CLIQUE;
			sMsg.byConfirmType = SMCT_UI_TONG_JOIN_APPLY;
			sMsg.byPriority = 3;
			sMsg.byParamSize = sizeof(KUiPlayerItem);
			
			KUiPlayerItem	player;
			strcpy(player.Name, szName);
			player.nIndex = pApply->m_nPlayerIdx;
			player.uId = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&player);

		}
		break;
	case enumTONG_SYNC_ID_ADD:
		{
			TONG_Add_SYNC	*pAdd = (TONG_Add_SYNC*)pMsg;
			char	szName[32], szTitle[32], szMaster[32];

			memcpy(szName, pAdd->m_szTongName,sizeof(szName));
			memcpy(szTitle, pAdd->m_szTitle, sizeof(szTitle));
			memcpy(szMaster, pAdd->m_szMaster, sizeof(szMaster));

			Player[CLIENT_PLAYER_INDEX].m_cTong.AddTong(pAdd->m_btCamp, szName, szTitle, szMaster);
			// Õ®÷™ΩÁ√Ê∏¸–¬ ˝æ›
			KUiGameObjectWithName	sUi;
			memset(&sUi, 0, sizeof(sUi));
			strcpy(sUi.szName, szName);
			sUi.nData = TONG_ACTION_APPLY;
			strcpy(sUi.szString, szTitle);
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, 1);
		/*------------------------------*/
			CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
		}
		break;

	case enumTONG_SYNC_ID_HEAD_INFO:
		{
			TONG_HEAD_INFO_SYNC	*pInfo = (TONG_HEAD_INFO_SYNC*)pMsg;

			// Õ®÷™ΩÁ√Êµ√µΩƒ≥∞Ôª·–≈œ¢
			int nIdx = NpcSet.SearchID(pInfo->m_dwNpcID);
			if (nIdx <= 0)
				break;

			KUiPlayerRelationWithOther	sUi;
			sUi.nIndex = nIdx;
			sUi.uId = Npc[nIdx].m_dwID;
			if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
				sUi.nRelation = Player[CLIENT_PLAYER_INDEX].m_cTong.GetFigure();
			else
				sUi.nRelation = -1;
			sUi.nData = 0;
			sUi.nParam = 0;
			strcpy(sUi.Name, Npc[nIdx].Name);

			KTongInfo	sInfo;
			memset(&sInfo, 0, sizeof(sInfo));
			sInfo.nFaction = pInfo->m_btCamp;
			sInfo.nMemberCount = (int)pInfo->m_dwMemberNum;
			sInfo.nManagerCount = (int)pInfo->m_btManagerNum;
			sInfo.nDirectorCount = (int)pInfo->m_btDirectorNum;
			sInfo.nMoney = pInfo->m_dwMoney;
			strcpy(sInfo.szMasterName, pInfo->m_sMember[0].m_szName);
			strcpy(sInfo.szName, pInfo->m_szTongName);
			// == add by Fong Ki“u ==
			sInfo.nStatusGuide = pInfo->m_nStatusGuide;
			strcpy(sInfo.szWayEdit, pInfo->m_szWayEdit);
			strcpy(sInfo.szNextTargetEdit, pInfo->m_szNextTargetEdit);
			sInfo.nExpGuide = pInfo->m_nExpGuide;
			sInfo.nCityGuide = pInfo->m_nCityGuide;
			sInfo.nTongLevel = pInfo->m_nTongLevel;
			strcpy(sInfo.szLeagueTName, pInfo->m_szLeagueTName);
			// == end add by Fong Ki“u ==
			CoreDataChanged(GDCNI_TONG_INFO, (unsigned int)&sUi, (unsigned int)&sInfo);
			//
			KUiGameObjectWithName	sObj;
			strcpy(sObj.szName, pInfo->m_szTongName);
			sObj.nData = enumTONG_FIGURE_DIRECTOR;
			sObj.nParam = 0;
			sObj.uParam = defTONG_MAX_DIRECTOR;
			sObj.szString[0] = 0;

			KTongMemberItem	sItem[defTONG_MAX_DIRECTOR];
			memset(sItem, 0, sizeof(sItem));
			for (int i = 0; i < pInfo->m_btDirectorNum; i++)
			{
				sItem[i].nData = enumTONG_FIGURE_DIRECTOR;
				strcpy(sItem[i].Name, pInfo->m_sMember[i + 1].m_szName);
				strcpy(sItem[i].szAgname, pInfo->m_sMember[i + 1].m_szTitle);
				sItem[i].btOnline = pInfo->m_sMember[i + 1].m_btOnline;
			}

			CoreDataChanged(GDCNI_TONG_MEMBER_LIST, (unsigned int)&sObj, (unsigned int)sItem);
		}
		break;
	case enumTONG_SYNC_ID_SELF_INFO:
		{
			TONG_SELF_INFO_SYNC	*pInfo = (TONG_SELF_INFO_SYNC*)pMsg;
			Player[CLIENT_PLAYER_INDEX].m_cTong.SetSelfInfo(pInfo);
			CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
		}
		break;
	case enumTONG_SYNC_ID_MANAGER_INFO:
		{
			TONG_MANAGER_INFO_SYNC	*pInfo = (TONG_MANAGER_INFO_SYNC*)pMsg;

			// Õ®÷™ΩÁ√Êµ√µΩƒ≥∞Ôª·∂”≥§–≈œ¢
			KUiGameObjectWithName	sObj;
			strcpy(sObj.szName, pInfo->m_szTongName);
			sObj.nData = enumTONG_FIGURE_MANAGER;
			sObj.nParam = pInfo->m_btStateNo;
			sObj.uParam = pInfo->m_btCurNum;
			sObj.szString[0] = 0;

			KTongMemberItem	sItem[defTONG_ONE_PAGE_MAX_NUM];
			memset(sItem, 0, sizeof(sItem));
			for (int i = 0; i < pInfo->m_btCurNum; i++)
			{
				sItem[i].nData = enumTONG_FIGURE_MANAGER;
				strcpy(sItem[i].Name, pInfo->m_sMember[i].m_szName);
				strcpy(sItem[i].szAgname, pInfo->m_sMember[i].m_szTitle);
				sItem[i].btOnline = pInfo->m_sMember[i].m_btOnline;
			}

			CoreDataChanged(GDCNI_TONG_MEMBER_LIST, (unsigned int)&sObj, (unsigned int)sItem);
		}
		break;
	case enumTONG_SYNC_ID_MEMBER_INFO:
		{
			TONG_MEMBER_INFO_SYNC	*pInfo = (TONG_MEMBER_INFO_SYNC*)pMsg;

			// Õ®÷™ΩÁ√Êµ√µΩƒ≥∞Ôª·∞Ô÷⁄–≈œ¢
			KUiGameObjectWithName	sObj;
			strcpy(sObj.szName, pInfo->m_szTongName);
			sObj.nData = enumTONG_FIGURE_MEMBER;
			sObj.nParam = pInfo->m_btStateNo;
			sObj.uParam = pInfo->m_btCurNum;
			sObj.szString[0] = 0;

			KTongMemberItem	sItem[defTONG_ONE_PAGE_MAX_NUM];
			memset(sItem, 0, sizeof(sItem));
			for (int i = 0; i < pInfo->m_btCurNum; i++)
			{
				sItem[i].nData = enumTONG_FIGURE_MEMBER;
				strcpy(sItem[i].Name, pInfo->m_sMember[i].m_szName);
				if (pInfo->m_sMember[i].m_btSex)
					strcpy(sItem[i].szAgname, pInfo->m_szTitleGirl);
				else
					strcpy(sItem[i].szAgname, pInfo->m_szTitleBoy);
				if (!sItem[i].szAgname[0])
					strcpy(sItem[i].szAgname, pInfo->m_szTitle);
				sItem[i].btOnline = pInfo->m_sMember[i].m_btOnline;
			}

			CoreDataChanged(GDCNI_TONG_MEMBER_LIST, (unsigned int)&sObj, (unsigned int)sItem);
		}
		break;
	case enumTONG_SYNC_ID_INSTATE:
		{
			TONG_INSTATE_SYNC	*pInstate = (TONG_INSTATE_SYNC*)pMsg;
			// Õ®÷™ΩÁ√Ê»Œ√¸ «∑Ò≥…π¶
			KUiGameObjectWithName	sUi;
			strcpy(sUi.szName, pInstate->m_szName);
			sUi.nData = TONG_ACTION_ASSIGN;
			sUi.nParam = pInstate->m_btNewFigure;
			sUi.uParam = pInstate->m_btOldFigure;
			strcpy(sUi.szString, pInstate->m_szTitle);
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, pInstate->m_btSuccessFlag);
		}
		break;
	case enumTONG_SYNC_ID_KICK:
		{
			TONG_KICK_SYNC	*pKick = (TONG_KICK_SYNC*)pMsg;
			// Õ®÷™ΩÁ√ÊÃﬂ»À «∑Ò≥…π¶
			KUiGameObjectWithName	sUi;
			strcpy(sUi.szName, pKick->m_szName);
			sUi.nData = TONG_ACTION_DISMISS;
			sUi.nParam = pKick->m_btFigure;
			sUi.uParam = pKick->m_btPos;
			sUi.szString[0] = 0;
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, pKick->m_btSuccessFlag);
		}
		break;
	case enumTONG_SYNC_ID_CHANGE_MASTER_FAIL:
		{
			TONG_CHANGE_MASTER_FAIL_SYNC	*pFail = (TONG_CHANGE_MASTER_FAIL_SYNC*)pMsg;

			switch (pFail->m_btFailID)
			{
			case 0:		// ∂‘∑Ω≤ª‘⁄œﬂ
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, "Phong ch¯c th t bπi, ÆËi ph≠¨ng kh´ng online!");
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;
			case 1:		// ∂‘∑Ωƒ‹¡¶≤ªπª£°
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, "Phong ch¯c th t bπi, ÆËi ph≠¨ng kh´ng ÆÒ tµi l∑nh Æπo!");
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;
			case 2:		// √˚◊÷≤ª∂‘£°
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, "Phong ch¯c th t bπi, bÊn bang kh´ng c„ ng≠Íi nµy!");
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;

			default:
				break;
			}
		}
		break;
	case enumTONG_SYNC_ID_CHANGE_TITLE_FAIL:
		{
			TONG_CHANGE_TITLE_FAIL_SYNC	*pFail = (TONG_CHANGE_TITLE_FAIL_SYNC*)pMsg;
			
			switch (pFail->m_btFailID)
			{
			case 0:		
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, "ßÊi t™n th t bπi, ÆËi ph≠¨ng kh´ng tr™n mπng.");
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;
			case 1:		
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, "ßÊi t™n th t bπi, ÆËi ph≠¨ng kh´ng ÆÒ tµi l∑nh Æπo.");
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;
			case 2:		
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, "ßÊi t™n th t bπi, bÊn bang kh´ng c„ ng≠Íi nµy.");
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;
			default:
				break;
			}
		}
		break;
			case enumTONG_SYNC_ID_CHANGE_CAMP_FAIL:
				{
					TONG_CHANGE_CAMP_FAIL_SYNC *pFail = (TONG_CHANGE_CAMP_FAIL_SYNC*)pMsg;
					
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					
					sprintf(sMsg.szMessage, "Kh´ng th” thay ÆÊi mµu bang hÈi ");
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					
					switch (pFail->m_btFailID)
					{
					case enumTONG_CHANGE_CAMP_ERROR_ID1:
						break;
					case enumTONG_CHANGE_CAMP_ERROR_ID2:
						break;
					case enumTONG_CHANGE_CAMP_ERROR_ID3:
						break;
					case enumTONG_CHANGE_CAMP_ERROR_ID4:
						break;
					case enumTONG_CHANGE_CAMP_ERROR_ID5:
						break;
					case enumTONG_CHANGE_CAMP_ERROR_ID6:
						sprintf(sMsg.szMessage, MSG_TONG_CHANGE_CAMP_ERROR1);
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					case enumTONG_CHANGE_CAMP_ERROR_ID7:
						sprintf(sMsg.szMessage, MSG_TONG_CHANGE_CAMP_ERROR2);
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					case enumTONG_CHANGE_CAMP_ERROR_ID8:
						sprintf(sMsg.szMessage, MSG_TONG_CHANGE_CAMP_ERROR3);
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
				}
				break;
			case enumTONG_SYNC_ID_CHANGE_CAMP:
				{
					TONG_CHANGE_CAMP_SYNC	*pChange = (TONG_CHANGE_CAMP_SYNC*)pMsg;
					Player[CLIENT_PLAYER_INDEX].m_cTong.ChangeCamp(pChange->m_btCamp);
				}
				break;	// thieu break lam goi CHANGE_CAMP roi tiep vao nhanh JX2 ben duoi
			case enumTONG_SYNC_ID_JX2:	// JX2 port: chuyen nguyen goi len UI
		{
			CoreDataChanged(GDCNI_TONG_JX2, (unsigned int)pMsg, (int)pHead->m_wLength + 1);
		}
		break;
		case enumTONG_SYNC_ID_CHANGE_RECRUIT:
				{
					TONG_CHANGE_RECRUIT_SYNC	*pChange = (TONG_CHANGE_RECRUIT_SYNC*)pMsg;
					Player[CLIENT_PLAYER_INDEX].m_cTong.ChangeRecruit(pChange->m_btRecruit);
				}
		break;

	default:
		break;
	}
}


void KProtocolProcess::s2cChangeWeather(BYTE* pMsg)
{
	g_ScenePlace.ChangeWeather(((SYNC_WEATHER*)pMsg)->WeatherID);
}

void	KProtocolProcess::s2cPKSyncNormalFlag(BYTE* pMsg)
{
	PK_NORMAL_FLAG_SYNC	*pFlag = (PK_NORMAL_FLAG_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cPK.SetNormalPKState(pFlag->m_btFlag, pFlag->m_bLockPK);
}

void	KProtocolProcess::s2cPKSyncEnmityState(BYTE* pMsg)
{
	PK_ENMITY_STATE_SYNC	*pState = (PK_ENMITY_STATE_SYNC*)pMsg;
	char	szName[32];

	memset(szName, 0, sizeof(szName));
	memcpy(szName, pState->m_szName, pState->m_wLength + 1 + sizeof(pState->m_szName) - sizeof(PK_ENMITY_STATE_SYNC));
	Player[CLIENT_PLAYER_INDEX].m_cPK.SetEnmityPKState(pState->m_btState, pState->m_dwNpcID, szName);
}

void	KProtocolProcess::s2cPKSyncExerciseState(BYTE* pMsg)
{
	PK_EXERCISE_STATE_SYNC	*pState = (PK_EXERCISE_STATE_SYNC*)pMsg;
	char	szName[32];
	memset(szName, 0, sizeof(szName));
	memcpy(szName, pState->m_szName, pState->m_wLength + 1 + sizeof(pState->m_szName) - sizeof(PK_EXERCISE_STATE_SYNC));
	Player[CLIENT_PLAYER_INDEX].m_cPK.SetExercisePKState(pState->m_btState, pState->m_dwNpcID, szName);
}

void	KProtocolProcess::s2cPKValueSync(BYTE* pMsg)
{
	PK_VALUE_SYNC	*pValue = (PK_VALUE_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cPK.SetPKValue(pValue->m_nPKValue);
}

void	KProtocolProcess::s2cReputeValueSync(BYTE* pMsg)
{
	REPUTE_VALUE_SYNC	*pValue = (REPUTE_VALUE_SYNC*)pMsg;
	//Player[CLIENT_PLAYER_INDEX].m_cRepute.SetReputeValue(pValue->m_nReputeValue);
}

void	KProtocolProcess::s2cFuYuanValueSync(BYTE* pMsg)
{
	FUYUAN_VALUE_SYNC	*pValue = (FUYUAN_VALUE_SYNC*)pMsg;
	//Player[CLIENT_PLAYER_INDEX].m_cFuYuan.SetFuYuanValue(pValue->m_nFuYuanValue);
}

void	KProtocolProcess::s2cReBornValueSync(BYTE* pMsg)
{
	// REBORN_VALUE_SYNC	*pValue = (REBORN_VALUE_SYNC*)pMsg;

	Player[CLIENT_PLAYER_INDEX].m_cReBorn.SetReBornValue(pMsg);	   //pValue->m_nReBornValue
}

void	KProtocolProcess::s2cViewEquip(BYTE* pMsg)
{
	g_cViewItem.GetData(pMsg);
}

void	KProtocolProcess::s2cTongCreate(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_cTong.Create((TONG_CREATE_SYNC*)pMsg);
}

void	KProtocolProcess::s2cNpcGoldChange(BYTE* pMsg)
{/*
	NPC_GOLD_CHANGE_SYNC	*pSync = (NPC_GOLD_CHANGE_SYNC*)pMsg;

	int nIdx = NpcSet.SearchID(pSync->m_dwNpcID);

	if (nIdx && Npc[nIdx].m_Kind == kind_normal)
	{
		Npc[nIdx].m_cGold.SetGoldCurrentType((int)pSync->m_wGoldFlag);
	}*/
}

void	KProtocolProcess::ItemChangeDurability(BYTE* pMsg)
{
	ITEM_DURABILITY_CHANGE	*pIDC = (ITEM_DURABILITY_CHANGE *)pMsg;

	int nIdx = ItemSet.SearchID(pIDC->dwItemID);
	
	if (nIdx)
	{
		Item[nIdx].SetDurability(Item[nIdx].GetDurability() + pIDC->nChange);
		_ASSERT(Item[nIdx].GetDurability() >= 0);
		if (Item[nIdx].GetDurability() <= 3)
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_ITEM_NEARLY_DAMAGED, Item[nIdx].GetName());
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
	}
}

void KProtocolProcess::LadderList(BYTE* pMsg)
{
	LADDER_LIST*	pList = (LADDER_LIST *)pMsg;
	KRankIndex		*pLadderListIndex = NULL;
	
	if (pList->nCount > 0 && pList->nCount < enumLadderEnd)
	{
		pLadderListIndex = new KRankIndex[pList->nCount];
	}
	if (pLadderListIndex)
	{
		for (int i = 0; i < pList->nCount; i++)
		{
			pLadderListIndex[i].usIndexId = pList->dwLadderID[i];
			pLadderListIndex[i].bValueAppened = true;
			pLadderListIndex[i].bSortFlag = true;
		}
		CoreDataChanged(GDCNII_RANK_INDEX_LIST_ARRIVE, pList->nCount, (int)pLadderListIndex);
		delete [] pLadderListIndex;
		pLadderListIndex = NULL;
	}
}

void KProtocolProcess::LadderResult(BYTE* pMsg)
{
	LADDER_DATA*	pLadderData = (LADDER_DATA *)pMsg;
	KRankMessage*	pLadderMessage = NULL;

	pLadderMessage = new KRankMessage[10];
	if (pLadderMessage)
	{
		for (int i = 0; i < 10; i++)
		{
			pLadderMessage[i].usMsgLen = strlen(pLadderData->StatData[i].Name);
			strcpy(pLadderMessage[i].szMsg, pLadderData->StatData[i].Name);
			pLadderMessage[i].nValueAppend = pLadderData->StatData[i].nValue;
			pLadderMessage[i].cSortFlag = (char)pLadderData->StatData[i].bySort;
		}
		unsigned int uParam = 10 | (((WORD)pLadderData->dwLadderID) << 16);
		CoreDataChanged(GDCNII_RANK_INFORMATION_ARRIVE, uParam, (int)pLadderMessage);
		delete [] pLadderMessage;
		pLadderMessage = NULL;
	}
}
#else

void KProtocolProcess::RemoveRole(int nIndex, BYTE * pProtocol)
{

}

void KProtocolProcess::NpcRequestCommand(int nIndex, BYTE* pProtocol)
{
	NPC_REQUEST_COMMAND *pNpcRequestSync = (NPC_REQUEST_COMMAND *)pProtocol;
	NpcSet.SyncNpc(pNpcRequestSync->ID, Player[nIndex].m_nNetConnectIdx);
}

void KProtocolProcess::ObjRequestCommand(int nIndex, BYTE* pProtocol)
{
	OBJ_CLIENT_SYNC_ADD	*pObjClientSyncAdd = (OBJ_CLIENT_SYNC_ADD*)pProtocol;
	ObjSet.SyncAdd(pObjClientSyncAdd->m_nID, Player[nIndex].m_nNetConnectIdx);
}

void KProtocolProcess::NpcWalkCommand(int nIndex, BYTE* pProtocol)
{
	NPC_WALK_COMMAND* pNetCommand = (NPC_WALK_COMMAND *)pProtocol;
	int ParamX = pNetCommand->nMpsX;
	int ParamY = pNetCommand->nMpsY;
	if (ParamX < 0)
	{
		ParamX = 0;
	}
	if (ParamY < 0)
	{
		ParamY = 0;
	}
	Npc[Player[nIndex].m_nIndex].SendCommand(do_walk, ParamX, ParamY);
}

void KProtocolProcess::NpcRunCommand(int nIndex, BYTE* pProtocol)
{
	NPC_RUN_COMMAND* pNetCommand = (NPC_RUN_COMMAND *)pProtocol;

	int ParamX = pNetCommand->nMpsX;
	int ParamY = pNetCommand->nMpsY;
	if (ParamX < 0)
	{
		ParamX = 0;
	}
	if (ParamY < 0)
	{
		ParamY = 0;
	}

	int nIdx = Player[nIndex].m_nIndex;
	if (nIdx > 0 && nIdx < MAX_NPC)
	{
		Npc[nIdx].SendCommand(do_run, ParamX, ParamY);
	}
}


void KProtocolProcess::NpcSkillCommand(int nIndex, BYTE* pProtocol)
{
	NPC_SKILL_COMMAND* pNetCommand = (NPC_SKILL_COMMAND *)pProtocol;
	int ParamX = pNetCommand->nSkillID;
	int ParamY = pNetCommand->nMpsX;
	int ParamZ = pNetCommand->nMpsY;
	if (ParamX <= 0 || ParamX > MAX_SKILL )
	{
		return ;
	}

	if (ParamZ < 0)
	{
		return;
	}

	if (ParamY < 0)
	{
		if (ParamY != -1) 
			return;

		int nNpcIndex = Player[nIndex].FindAroundNpc((DWORD)ParamZ);
		if (nNpcIndex > 0)
			Npc[Player[nIndex].m_nIndex].SendCommand(do_skill, ParamX, ParamY, nNpcIndex);
	}
	else
	{
		Npc[Player[nIndex].m_nIndex].SendCommand(do_skill, ParamX, ParamY, ParamZ);
	}
}

void KProtocolProcess::NpcJumpCommand(int nIndex, BYTE* pProtocol)
{
	NPC_JUMP_COMMAND* pNetCommand = (NPC_JUMP_COMMAND *)pProtocol;
	int ParamX = pNetCommand->nMpsX;
	int ParamY = pNetCommand->nMpsY;
	Npc[Player[nIndex].m_nIndex].SendCommand(do_jump, ParamX, ParamY);
}

void KProtocolProcess::c2sDynamicStruct(int nIndex, BYTE* pProtocol)
{
	DYNAMIC_COMMAND* p = (DYNAMIC_COMMAND*)pProtocol;
	if(p->nBranch == c2sdnmbr_arrangeitem)
	{
		if (Player[nIndex].CheckTrading())
			return;
		if(Npc[Player[nIndex].m_nIndex].m_BaiTan)
			return;
		Player[nIndex].AutoArrangeItem();
	}
	else if(p->nBranch == c2sdnmbr_arrangebox)
	{
		if (Player[nIndex].CheckTrading())
			return;
		if(Npc[Player[nIndex].m_nIndex].m_BaiTan)
			return;
		if(Player[nIndex].AutoArrangeItem(1))
		{
			Player[nIndex].AutoArrangeItem(2);
			Player[nIndex].AutoArrangeItem(3);
			Player[nIndex].AutoArrangeItem(4);
		}
	}
	else if(p->nBranch == c2sdnmbr_exchangeitem)
	{
		if (Player[nIndex].CheckTrading())
			return;
		if(Npc[Player[nIndex].m_nIndex].m_BaiTan)
			return;
		if(!Player[nIndex].m_CUnlocked)
			return;
		if(p->m_wLength + 1 - sizeof(DYNAMIC_COMMAND) < 2*sizeof(BYTE) + sizeof(int))
			return;
		BYTE* pPos = (BYTE*)(p+1);
		int nSrcPos = *(pPos++);
		int nTargetPos = *(pPos++);
		int nItemID = *(int*)pPos;
		if(nSrcPos == nTargetPos)
			return;
		//printf("src %d tg %d item %d\n", nSrcPos, nTargetPos, nItemID);
		if(nSrcPos == pos_equiproom)
		{
			int nSrcPlace;
			int nSrcIdx = Player[nIndex].m_ItemList.SearchID(nItemID, &nSrcPlace);
			if(!nSrcIdx || nSrcPlace != pos_equiproom)
				return;
			if(nTargetPos == pos_equip || nTargetPos == pos_equipback)
			{
				int nPart = Player[nIndex].m_ItemList.GetEquipPlace(Item[nSrcIdx].GetDetailType());
				if(nPart < 0)
					return;
				if(!Player[nIndex].m_ItemList.CanEquip(nSrcIdx, nPart))
					return;
				int nDesIdx = Player[nIndex].m_ItemList.GetEquipment(nPart);
				if(nDesIdx > 0)
				{
					if(nPart == itempart_ring1)
					{
						int nRing2 = Player[nIndex].m_ItemList.GetEquipment(itempart_ring2);
						if(nRing2 <= 0)
						{
							nPart = itempart_ring2;
							nDesIdx = 0;
						}
					}
					if(nDesIdx > 0)
					{
						int x, y;
						if (!Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(
							Item[nDesIdx].GetWidth(), Item[nDesIdx].GetHeight(), &x, &y))
							return;
						Player[nIndex].m_ItemList.Remove(nDesIdx);
						Player[nIndex].m_ItemList.AddKIL(nDesIdx, pos_equiproom, x,y);
					}
				}
				Player[nIndex].m_ItemList.Remove(nSrcIdx);
				Player[nIndex].m_ItemList.AddKIL(nSrcIdx, pos_equip, nPart, 0);
			}
			else if(nTargetPos == pos_repositoryroom)
			{
				if(Npc[Player[nIndex].m_nIndex].m_FightMode)
					return;
				int x, y;
				if (!Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(
					Item[nSrcIdx].GetWidth(), Item[nSrcIdx].GetHeight(), &x, &y, room_repository))
					return;
				Player[nIndex].m_ItemList.Remove(nSrcIdx);
				Player[nIndex].m_ItemList.AddKIL(nSrcIdx, pos_repositoryroom, x, y);
			}
			else if(nTargetPos == pos_exbox1room)
			{
				if(Npc[Player[nIndex].m_nIndex].m_FightMode)
					return;
				if(Npc[Player[nIndex].m_nIndex].m_ExBoxId < 1)
					return;
				int x, y;
				if (!Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(
					Item[nSrcIdx].GetWidth(), Item[nSrcIdx].GetHeight(), &x, &y, room_exbox1))
					return;
				Player[nIndex].m_ItemList.Remove(nSrcIdx);
				Player[nIndex].m_ItemList.AddKIL(nSrcIdx, pos_exbox1room, x, y);
			}
			else if(nTargetPos == pos_exbox2room)
			{
				if(Npc[Player[nIndex].m_nIndex].m_FightMode)
					return;
				if(Npc[Player[nIndex].m_nIndex].m_ExBoxId < 2)
					return;
				int x, y;
				if (!Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(
					Item[nSrcIdx].GetWidth(), Item[nSrcIdx].GetHeight(), &x, &y, room_exbox2))
					return;
				Player[nIndex].m_ItemList.Remove(nSrcIdx);
				Player[nIndex].m_ItemList.AddKIL(nSrcIdx, pos_exbox2room, x, y);
			}
			else if(nTargetPos == pos_exbox3room)
			{
				if(Npc[Player[nIndex].m_nIndex].m_FightMode)
					return;
				if(Npc[Player[nIndex].m_nIndex].m_ExBoxId < 3)
					return;
				int x, y;
				if (!Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(
					Item[nSrcIdx].GetWidth(), Item[nSrcIdx].GetHeight(), &x, &y, room_exbox3))
					return;
				Player[nIndex].m_ItemList.Remove(nSrcIdx);
				Player[nIndex].m_ItemList.AddKIL(nSrcIdx, pos_exbox3room, x, y);
			}
		}
		else if(nSrcPos == pos_equip || nSrcPos == pos_equipback ||(!Npc[Player[nIndex].m_nIndex].m_FightMode &&( nSrcPos == pos_repositoryroom || nSrcPos == pos_exbox1room || nSrcPos == pos_exbox2room || nSrcPos == pos_exbox3room)))
		{
			int nSrcPlace;
			int nSrcIdx = Player[nIndex].m_ItemList.SearchID(nItemID, &nSrcPlace);
			if(!nSrcIdx || (nSrcPos != pos_equipback && nSrcPlace != nSrcPos) || (nSrcPos == pos_equipback && nSrcPlace != pos_equip))
				return;
			if(nTargetPos == pos_equiproom)
			{
				int x, y;
				if (!Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(
					Item[nSrcIdx].GetWidth(), Item[nSrcIdx].GetHeight(), &x, &y))
					return;
				Player[nIndex].m_ItemList.Remove(nSrcIdx);
				Player[nIndex].m_ItemList.AddKIL(nSrcIdx, pos_equiproom, x, y);
			}
		}
	}
	else if(p->nBranch == c2sdnmbr_movemapid)
	{
		if (Player[nIndex].CheckTrading())
			return;
		if(Npc[Player[nIndex].m_nIndex].m_BaiTan)
			return;
		if(p->m_wLength + 1 - sizeof(DYNAMIC_COMMAND) < sizeof(int))
			return;
		int* pMapID = (int*)(p+1);
		UINT dwScriptId = g_FileName2Id("\\script\\item\\ib\\shenxingfu.lua");
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		if(pScript)
		{
			Npc[Player[nIndex].m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, nIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			Lua_PushNumber(pScript->m_LuaState, Npc[Player[nIndex].m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			try
			{
				int nTopIndex = 0;
				pScript->SafeCallBegin(&nTopIndex);
				if (!pScript->CallFunction("GotoMapId",0, "d", *pMapID))
				{
					Player[nIndex].m_bWaitingPlayerFeedBack = false;
					Player[nIndex].m_btTryExecuteScriptTimes = 0;
					Npc[Player[nIndex].m_nIndex].m_ActionScriptID = 0;
				}
				pScript->SafeCallEnd(nTopIndex);
			}
			catch(...)
			{
				Player[nIndex].m_bWaitingPlayerFeedBack = false;
				Player[nIndex].m_btTryExecuteScriptTimes = 0;
				Npc[Player[nIndex].m_nIndex].m_ActionScriptID = 0;
			}
		}
	}
}

void KProtocolProcess::NpcTalkCommand(int nIndex, BYTE* pProtocol)
{

}

void KProtocolProcess::PlayerTalkCommand(int nIndex, BYTE* pProtocol)
{
	Npc[Player[nIndex].m_nIndex].DoPlayerTalk((char *)pProtocol + 1);
}

void KProtocolProcess::PlayerApplyTeamInfo(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].S2CSendTeamInfo(pProtocol);
}

void KProtocolProcess::PlayerApplyCreateTeam(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].CreateTeam(pProtocol);
}

void KProtocolProcess::PlayerApplyTeamOpenClose(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].SetTeamState(pProtocol);
}

void KProtocolProcess::PlayerApplyAddTeam(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].S2CSendAddTeamInfo(pProtocol);
}

void KProtocolProcess::PlayerAcceptTeamMember(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].AddTeamMember(pProtocol);
}

void KProtocolProcess::PlayerApplyLeaveTeam(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].LeaveTeam(pProtocol);
}

void KProtocolProcess::PlayerApplyTeamKickMember(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TeamKickOne(pProtocol);
}

void KProtocolProcess::PlayerApplyTeamChangeCaptain(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TeamChangeCaptain(pProtocol);
}

void KProtocolProcess::PlayerApplyTeamDismiss(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TeamDismiss(pProtocol);
}

void KProtocolProcess::PlayerApplySetPK(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].SetPK(pProtocol);
}

void KProtocolProcess::PlayerApplyFactionData(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].SendFactionData(pProtocol);
}

void KProtocolProcess::PlayerSendChat(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ServerSendChat(pProtocol);
}

void KProtocolProcess::PlayerAddBaseAttribute(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].AddBaseAttribute(pProtocol);
}

void KProtocolProcess::PlayerApplyAddSkillPoint(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].AddSkillPoint(pProtocol);
}

void KProtocolProcess::PlayerEatItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].EatItem(pProtocol);
}

void KProtocolProcess::PlayerPickUpItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].ServerPickUpItem(pProtocol);
}

void KProtocolProcess::PlayerMoveItem(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ServerMoveItem(pProtocol);
}

void KProtocolProcess::PlayerSellItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].SellItem(pProtocol);
}

void KProtocolProcess::PlayerBuyItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].BuyItem(pProtocol);
}

void KProtocolProcess::PlayerDropItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].ServerThrowAwayItem(pProtocol);
}

void KProtocolProcess::PlayerSelUI(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ProcessPlayerSelectFromUI(pProtocol);
}

void KProtocolProcess::ChatSetChannel(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatSetTakeChannel(pProtocol);
}

void KProtocolProcess::ChatApplyAddFriend(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatTransmitApplyAddFriend(pProtocol);
}

void KProtocolProcess::ChatAddFriend(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatAddFriend(pProtocol);
}

void KProtocolProcess::ChatRefuseFriend(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatRefuseFriend(pProtocol);
}

void KProtocolProcess::ChatApplyReSendAllFriendName(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatResendAllFriend(pProtocol);
}

void KProtocolProcess::ChatApplySendOneFriendName(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatSendOneFriendData(pProtocol);
}

void KProtocolProcess::ChatDeleteFriend(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatDeleteFriend(pProtocol);
}

void KProtocolProcess::ChatReDeleteFriend(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatRedeleteFriend(pProtocol);
}

void	KProtocolProcess::TradeApplyOpen(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TradeApplyOpen(pProtocol);
}

void	KProtocolProcess::GambleApplyOpen(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].GambleApplyOpen(pProtocol);
}

void	KProtocolProcess::TradeApplyClose(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TradeApplyClose(pProtocol);
}

void	KProtocolProcess::GambleApplyClose(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].GambleApplyClose(pProtocol);
}

void	KProtocolProcess::TradeApplyStart(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].TradeApplyStart(pProtocol);
}

void	KProtocolProcess::GambleApplyStart(int nIndex, BYTE* pProtocol)
{
	if (!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].GambleApplyStart(pProtocol);
}

void	KProtocolProcess::TradeMoveMoney(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].TradeMoveMoney(pProtocol);
}

void	KProtocolProcess::GambleMoveMoney(int nIndex, BYTE* pProtocol)
{
	if (!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].GambleMoveMoney(pProtocol);
}

void	KProtocolProcess::TradeDecision(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].TradeDecision(pProtocol);
}

void	KProtocolProcess::GambleDecision(int nIndex, BYTE* pProtocol)
{
	if (!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].GambleDecision(pProtocol);
}

void	KProtocolProcess::DialogNpc(int nIndex, BYTE * pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].DialogNpc(pProtocol)	;
}

void	KProtocolProcess::TeamInviteAdd(int nIndex, BYTE * pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].m_cTeam.InviteAdd(nIndex, (TEAM_INVITE_ADD_COMMAND*)pProtocol);
}

void	KProtocolProcess::ChangeAuraSkill(int nIndex, BYTE * pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	SKILL_CHANGEAURASKILL_COMMAND * pCommand = (SKILL_CHANGEAURASKILL_COMMAND*) pProtocol;
	Npc[Player[nIndex].m_nIndex].SetAuraSkill(pCommand->m_nAuraSkill);
}

void	KProtocolProcess::TeamReplyInvite(int nIndex, BYTE * pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	TEAM_REPLY_INVITE_COMMAND	*pReply = (TEAM_REPLY_INVITE_COMMAND*)pProtocol;
	if (!pProtocol ||
		pReply->m_nIndex <= 0 ||
		pReply->m_nIndex >= MAX_PLAYER ||
		Player[pReply->m_nIndex].m_nIndex <= 0)
		return;
	if (pReply->m_btResult && Player[nIndex].m_cTeam.GetCanTeamFlag() == FALSE)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_CANNOT_ADD_TEAM;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return;
	}
	Player[pReply->m_nIndex].m_cTeam.GetInviteReply(pReply->m_nIndex, nIndex, pReply->m_btResult);
}

//void KProtocolProcess::ReplyPing(int nIndex, BYTE* pProtocol)
//{
//	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
//		return;
//
//	PING_COMMAND PingCmd, *pPingCmd;
//
//	pPingCmd = (PING_COMMAND *)pProtocol;
//
//	PingCmd.ProtocolType = s2c_ping;
//	PingCmd.m_dwTime = pPingCmd->m_dwTime;
//	g_pServer->SendData(Player[nIndex].m_nNetConnectIdx, &PingCmd, sizeof(PING_COMMAND));
//	Player[nIndex].m_uLastPingTime = g_SubWorldSet.GetGameTime();
//}

void KProtocolProcess::NpcSitCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	NPC_SIT_COMMAND *pSitCmd;
	pSitCmd = (NPC_SIT_COMMAND *)pProtocol;

	if (pSitCmd->m_btSitFlag)
		Npc[Player[nIndex].m_nIndex].SendCommand(do_sit);
	else
		Npc[Player[nIndex].m_nIndex].SendCommand(do_stand);
//	if (Npc[Player[nIndex].m_nIndex].m_Doing != do_sit)
//		Npc[Player[nIndex].m_nIndex].SendCommand(do_sit);
//	else
//		Npc[Player[nIndex].m_nIndex].SendCommand(do_stand);
}

void KProtocolProcess::ObjMouseClick(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	if (Player[nIndex].CheckTrading())
		return;

	int		nSubWorldIdx, nRegionIdx, nObjIdx;
	int		nPlayerX, nPlayerY, nObjX, nObjY;
	OBJ_MOUSE_CLICK_SYNC 	*pObj = (OBJ_MOUSE_CLICK_SYNC*)pProtocol;

	nSubWorldIdx = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
	nRegionIdx = SubWorld[nSubWorldIdx].FindRegion(pObj->m_dwRegionID);
	if (nRegionIdx < 0)
		return;
	nObjIdx = SubWorld[nSubWorldIdx].m_Region[nRegionIdx].FindObject(pObj->m_nObjID);
	if (nObjIdx <= 0)
		return;

	SubWorld[nSubWorldIdx].Map2Mps(
		Npc[Player[nIndex].m_nIndex].m_RegionIndex,
		Npc[Player[nIndex].m_nIndex].m_MapX,
		Npc[Player[nIndex].m_nIndex].m_MapY,
		Npc[Player[nIndex].m_nIndex].m_OffX,
		Npc[Player[nIndex].m_nIndex].m_OffY,
		&nPlayerX,
		&nPlayerY);
	SubWorld[nSubWorldIdx].Map2Mps(
		nRegionIdx,
		Object[nObjIdx].m_nMapX,
		Object[nObjIdx].m_nMapY,
		Object[nObjIdx].m_nOffX,
		Object[nObjIdx].m_nOffY,
		&nObjX,
		&nObjY);
/*
	Obj_Kind_MapObj = 0,		// µÿÕºŒÔº˛£¨÷˜“™”√”⁄µÿÕº∂Øª≠
	Obj_Kind_Body,				// npc µƒ ¨ÃÂ
	Obj_Kind_Box,				// ±¶œ‰
	Obj_Kind_Item,				// µÙ‘⁄µÿ…œµƒ◊∞±∏
	Obj_Kind_Money,				// µÙ‘⁄µÿ…œµƒ«Æ
	Obj_Kind_LoopSound,			// —≠ª∑“Ù–ß	//Loop
	Obj_Kind_RandSound,			// ÀÊª˙“Ù–ß
	Obj_Kind_Light,				// π‚‘¥£®3Dƒ£ Ω÷–∑¢π‚µƒ∂´Œ˜£©
	Obj_Kind_Door,				// √≈¿‡
	Obj_Kind_Trap,				// œ›⁄Â
	Obj_Kind_Prop,				// –°µ¿æﬂ£¨ø…÷ÿ…˙
	Obj_Kind_Num,				// ŒÔº˛µƒ÷÷¿‡ ˝
*/
	switch (Object[nObjIdx].m_nKind)
	{
	case Obj_Kind_Box:
		if (g_GetDistance(nPlayerX, nPlayerY, nObjX, nObjY) > defMAX_EXEC_OBJ_SCRIPT_DISTANCE)
			break;
		if (Object[nObjIdx].m_nState == OBJ_BOX_STATE_CLOSE)
			Object[nObjIdx].ExecScript(nIndex);
		break;
	case Obj_Kind_Door:
		break;
	case Obj_Kind_Prop:
		if (g_GetDistance(nPlayerX, nPlayerY, nObjX, nObjY) > defMAX_EXEC_OBJ_SCRIPT_DISTANCE)
			break;
		if (Object[nObjIdx].m_nState == OBJ_PROP_STATE_DISPLAY)
			Object[nObjIdx].ExecScript(nIndex);
		break;
	}
}

void KProtocolProcess::StoreMoneyCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	STORE_MONEY_COMMAND*	pCommand = (STORE_MONEY_COMMAND *)pProtocol;

	if (pCommand->m_byDir)	
		Player[nIndex].m_ItemList.ExchangeMoney(room_repository, room_equipment, pCommand->m_dwMoney);
	else					
		Player[nIndex].m_ItemList.ExchangeMoney(room_equipment, room_repository, pCommand->m_dwMoney);
}

void KProtocolProcess::NpcReviveCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

//	NPC_REVIVE_COMMAND*		pCommand = (NPC_REVIVE_COMMAND *)pProtocol;
	Player[nIndex].Revive(REMOTE_REVIVE_TYPE);
}

void KProtocolProcess::c2sTradeReplyStart(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].c2sTradeReplyStart(pProtocol);
}

void KProtocolProcess::c2sGambleReplyStart(int nIndex, BYTE* pProtocol)
{
	if (!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].c2sGambleReplyStart(pProtocol);
}

void KProtocolProcess::c2sPKApplyChangeNormalFlag(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	PK_APPLY_NORMAL_FLAG_COMMAND	*pApply = (PK_APPLY_NORMAL_FLAG_COMMAND*)pProtocol;
	Player[nIndex].m_cPK.SetNormalPKState(pApply->m_btFlag, pApply->m_bLockPK);
}

void KProtocolProcess::c2sPKApplyEnmity(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	PK_APPLY_ENMITY_COMMAND	*pApply = (PK_APPLY_ENMITY_COMMAND*)pProtocol;
	if (Player[nIndex].m_nIndex && !Npc[Player[nIndex].m_nIndex].m_FightMode)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PK_ERROR_1;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return;
	}
	if (pApply->m_dwNpcID <= 0 || pApply->m_dwNpcID >= MAX_NPC)
		return;
	int nNpcIdx = Player[nIndex].FindAroundNpc(pApply->m_dwNpcID);
	if (nNpcIdx <= 0)
		return;
	if (Npc[nNpcIdx].m_Kind != kind_player || Npc[nNpcIdx].GetPlayerIdx() <= 0)
		return;

	Player[nIndex].m_cPK.EnmityPKOpen(Npc[nNpcIdx].GetPlayerIdx());
}

#define		defMAX_VIEW_EQUIP_TIME			30
void	KProtocolProcess::c2sViewEquip(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	if (g_SubWorldSet.GetGameTime() - Player[nIndex].m_nViewEquipTime < defMAX_VIEW_EQUIP_TIME)
		return;
	Player[nIndex].m_nViewEquipTime = g_SubWorldSet.GetGameTime();

	VIEW_EQUIP_COMMAND	*pView = (VIEW_EQUIP_COMMAND*)pProtocol;
	if (pView->m_dwNpcID == Npc[Player[nIndex].m_nIndex].m_dwID)
		return;
	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pView->m_dwNpcID);
	if (nPlayerIdx <= 0)
		return;
	Player[nPlayerIdx].SendEquipItemInfo(nIndex);
}

void KProtocolProcess::LadderQuery(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	LADDER_QUERY*	pLQ = (LADDER_QUERY *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		int lnID = Player[nIndex].m_nNetConnectIdx;
		if (lnID >= 0)
		{
			LADDER_DATA	LadderData;
			LadderData.ProtocolType = s2c_ladderresult;
			LadderData.dwLadderID = pLQ->dwLadderID;
			void* pData = (void *)Ladder.GetTopTen(LadderData.dwLadderID);
			if (pData)
			{
				memcpy(LadderData.StatData, pData, sizeof(LadderData.StatData));
				g_pServer->PackDataToClient(lnID, &LadderData, sizeof(LadderData));
			}
		}
	}
}

void KProtocolProcess::ItemRepair(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	ITEM_REPAIR	*pIR = (ITEM_REPAIR *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		Player[nIndex].RepairItem(pIR->dwItemID);
	}
}

void KProtocolProcess::RecoveryBoxCmd(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	RECOVERY_BOX_CMD *pCmd = (RECOVERY_BOX_CMD *)pProtocol;;
	Player[nIndex].RecoveryBox(pCmd->dwID, pCmd->nX, pCmd->nY);			
}

void KProtocolProcess::c2sPlayerThrowAllItem(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	Player[nIndex].ServerThrowAllItem(pProtocol);
}

void KProtocolProcess::c2sGetCityOwnTong(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	GET_CITY_OWN_TONG *pCmd = (GET_CITY_OWN_TONG *)pProtocol;;
	Player[nIndex].GetCityOwnTong();
}

void KProtocolProcess::RecoverItemCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	RECOVER_ITEM_COMMAND *pRecoverItem = (RECOVER_ITEM_COMMAND*)pProtocol;
	Player[nIndex].m_ItemList.RecoverItem(pRecoverItem->m_nPos);
	Player[nIndex].m_dwTrembleItemId = 0;
}

void KProtocolProcess::c2sInputCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	C2S_PLAYER_INPUT_INFO *pInput = (C2S_PLAYER_INPUT_INFO *)pProtocol;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	switch (pInput->nType)
	{
	case 1:
		{
			strncpy(Player[nIndex].szStringInput,pInput->nAction, sizeof(Player[nIndex].szStringInput));
			Player[nIndex].ExecuteScript(Player[nIndex].m_dwStrBoxId, Player[nIndex].m_szTaskExcuteFun,"");
		}
		break;
	case 2:
		{
			Player[nIndex].m_nStringNum = pInput->nNum;
			Player[nIndex].ExecuteScript(Player[nIndex].m_dwNumberBoxId, Player[nIndex].m_szTaskExcuteFun,"");
		}
		break;
	default:
		break;
	}
}

// DOT E (E4): phien GiveItemUI JX2 (KJx2WarInfra.cpp) - tra -1 khi khong phai
extern int KJx2WarInfra_GiveBoxCollect(int nPlayerIdx);

void KProtocolProcess::UiCommandScript(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	PLAYER_UI_CMD_SCRIPT *pUiCmd;
	pUiCmd = (PLAYER_UI_CMD_SCRIPT *)pProtocol;
	switch (pUiCmd->nType)
	{
		case 1:
			if(Player[nIndex].m_dwGiveBoxId > 0)
			{
				// DOT E (E4): phien GiveItemUI JX2 - gom item pos_affairitem vao
				// give-list roi goi callback dang fn(nCount); phien GiveBox JX1
				// thuong (tra -1) giu nguyen duong cu fn("")
				int nJx2Cnt = KJx2WarInfra_GiveBoxCollect(nIndex);
				if (nJx2Cnt >= 0)
					Player[nIndex].ExecuteScript(Player[nIndex].m_dwGiveBoxId, Player[nIndex].m_szTaskExcuteFun, nJx2Cnt);
				else
					Player[nIndex].ExecuteScript(Player[nIndex].m_dwGiveBoxId, Player[nIndex].m_szTaskExcuteFun, "");
				Player[nIndex].m_dwGiveBoxId = 0;
			}
			break;
		case 2:
			if(Player[nIndex].m_dwTimeBoxId > 0)
			{
				Player[nIndex].ExecuteScript(Player[nIndex].m_dwTimeBoxId, Player[nIndex].m_szTaskExcuteFun, 0);
				Player[nIndex].m_dwTimeBoxId = 0;
			}
			break;
		case 3: //thuong da tau
			{
			if (!strcmp(pUiCmd->szFunc, "finish_exp") || !strcmp(pUiCmd->szFunc, "finish_money") || !strcmp(pUiCmd->szFunc, "quest_random")) {
				Player[nIndex].ExecuteScript(Player[nIndex].m_dwRewardId, pUiCmd->szFunc, "");
				Player[nIndex].m_dwRewardId = 0;
			}
			}
			break;
		case 4://thuong da tau 1
			{
			if (!strcmp(pUiCmd->szFunc, "finish_point") || !strcmp(pUiCmd->szFunc, "finish_lucky") || !strcmp(pUiCmd->szFunc, "finish_item")) {
				Player[nIndex].ExecuteScript(Player[nIndex].m_dwRewardExId, pUiCmd->szFunc, "");
				Player[nIndex].m_dwRewardExId = 0;
			}
			}
			break;
		case 6:	// [TaskGuide] nut 'Bo nhiem vu' tren bang F11 - mo hop xac nhan huy chuan
			if (!strcmp(pUiCmd->szFunc, "tg_quit"))
			{
				Player[nIndex].ExecuteScript("\\script\\global\\seasonnpc.lua", "tg_quit", 0);
			}
			break;
		case 5://kh∂m nπm
			{
				Player[nIndex].ExecuteScript(Player[nIndex].m_dwTrembleItemId, Player[nIndex].m_szTaskExcuteFun, "");
				Player[nIndex].m_dwTrembleItemId = 0;
			}
		default:
			break;
	}				
}

void KProtocolProcess::NpcRideCommand(int nIndex, BYTE* pProtocol)//edit by phong kieu len xuong ngua
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	NPC_RIDE_COMMAND *pRideCmd;
	pRideCmd = (NPC_RIDE_COMMAND *)pProtocol;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	if(GetTickCount() - Npc[Player[nIndex].m_nIndex].m_TimeHorse >= TIME_RIDE)
	{
		Npc[Player[nIndex].m_nIndex].m_TimeHorse = GetTickCount();
		Player[nIndex].CheckRideHorse(pRideCmd->m_btRideFlag);
		return;
	}
}

void KProtocolProcess::NpcCPUnlockCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	PLAYER_REQUEST_CP_UNLOCK *pUnlockCmd;
	pUnlockCmd = (PLAYER_REQUEST_CP_UNLOCK *)pProtocol;
	if (Player[nIndex].CheckChestPW(pUnlockCmd->int_PW))
	{
		Player[nIndex].SetChestLock(TRUE);
	}
	return;
}

void KProtocolProcess::NpcCPLockCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].SetChestLock(FALSE);
	return;
}

void KProtocolProcess::NpcSwitchEquipSet(int nIndex, BYTE* pProtocol)
{
	if (!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	PLAYER_REQUEST_SWITCH_EQUIP_SET* pSwitchEquipSetCmd;
	pSwitchEquipSetCmd = (PLAYER_REQUEST_SWITCH_EQUIP_SET*)pProtocol;
	Player[nIndex].SwitchEquipSet(pSwitchEquipSetCmd->byte_setnum);
	return;
}
void KProtocolProcess::NpcCPChangeCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	PLAYER_REQUEST_CP_CHANGE *pChangeCmd;
	pChangeCmd = (PLAYER_REQUEST_CP_CHANGE *)pProtocol;
	if(Player[nIndex].m_CUnlocked != 1)
		return;

	if(Player[nIndex].CheckChestPW(pChangeCmd->int_OldPW))
	{
		Player[nIndex].SetChestPW2(pChangeCmd->int_NewPW);		// The Old PW is correct
	}			
}

void KProtocolProcess::NpcCPResetCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	PLAYER_REQUEST_CP_RESET *pResetCmd;
	pResetCmd = (PLAYER_REQUEST_CP_RESET *)pProtocol;
	Player[nIndex].SetNewPW(pResetCmd->int_ResetPW);
	Player[nIndex].m_CUnlocked = 1;
}

void KProtocolProcess::SetPrice(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	PLAYER_SET_PRICE *pSP=(PLAYER_SET_PRICE *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{ 
		Player[nIndex].m_ItemList.SetPrice(pSP->m_ID,pSP->m_Price);
	}
}

void KProtocolProcess::StartTrade(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	
	PLAYER_START_TRADE *pST = (PLAYER_START_TRADE *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		if (pST->m_bSet != 0 && Player[nIndex].GetTradeCount())
		{
			Npc[Player[nIndex].m_nIndex].m_BaiTan = 1;
			strcpy(Npc[Player[nIndex].m_nIndex].ShopName,pST->m_Name);
			int p_SubWordlIndex = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
			int s_CityTax = SubWorld[p_SubWordlIndex].m_CityTax; //#thue suat
			if(s_CityTax > 0)
			{
				char szTemp[64];
				sprintf(szTemp, "Thu’ su t khu v˘c nµy %d %s. ", s_CityTax, "%");
				Player[nIndex].ExecuteScript("\\script\\player\\mgs2player_from_c.lua","main", szTemp, false);
			}
		}
		else
			Npc[Player[nIndex].m_nIndex].m_BaiTan = 0;
	}
}

void KProtocolProcess::c2sViewItem(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	VIEW_EQUIP_COMMAND	*pView = (VIEW_EQUIP_COMMAND*)pProtocol;
	if (pView->m_dwNpcID == Npc[Player[nIndex].m_nIndex].m_dwID)
		return;
	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pView->m_dwNpcID);
	if (nPlayerIdx <= 0)
		return;
	Player[nPlayerIdx].SendSellItemInfo(nIndex ,pView->m_bPrcess);
}

void KProtocolProcess::c2supdateitem(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	VIEW_EQUIP_COMMAND	*pView = (VIEW_EQUIP_COMMAND*)pProtocol;
	if (pView->m_dwNpcID == Npc[Player[nIndex].m_nIndex].m_dwID)
		return;
	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pView->m_dwNpcID);
	if (nPlayerIdx <= 0)
		return;
	Player[nPlayerIdx].SendSellItemInfo(nIndex ,pView->m_bPrcess,TRUE);
}

void KProtocolProcess::c2sNeedCount(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	PLAYER_NEED_COUNT *pView = (PLAYER_NEED_COUNT *)pProtocol;
	// (r5e - auto Da Tau) dwId dac biet = xin DANH BA SAP ca map: tra ve
	// "[SapMap] id:x:y ..." (toa do CELL) qua tin He Thong rieng nguoi hoi.
	// Chi liet ke sap co PLAYER that dung sau (nguoi choi + bot PB) - dan
	// SimCity (KNpc) tu bi loai. Id nay duoc DAT CHO trong KNpcSet::SetID.
	if (pView->dwId == DATAU_SAPMAP_ID)
	{
		// (r5f - phan bien) nhanh nay nang hon duong cu (quet MAX_PLAYER + goi
		// Lua) va chay cho MOI client -> chan spam 5 giay/nguoi. Bot tu hoi
		// 90 giay mot lan nen khong anh huong.
		// (r5g - phan bien vong 2) khe nguoi choi duoc TAI SU DUNG khi co nguoi
		// thoat/vao, va cooldown theo NGUOI chu khong theo MAP - nguoi moi se
		// thua ke cooldown cua chu cu, con bot vua doi thanh (hoi ngay) thi bi
		// vut goi dau tien. Theo doi chu khe (m_dwID) + subworld, doi la xoa.
		static DWORD s_uSapDsNext[MAX_PLAYER] = { 0 };
		static DWORD s_uSapDsChu[MAX_PLAYER] = { 0 };
		static int   s_nSapDsSub[MAX_PLAYER] = { 0 };
		const DWORD dwNayDs = SubWorld[0].m_dwCurrentTime;
		int nSubDs = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
		if (s_uSapDsChu[nIndex] != Player[nIndex].m_dwID
		 || s_nSapDsSub[nIndex] != nSubDs)
		{
			s_uSapDsNext[nIndex] = 0;
			s_uSapDsChu[nIndex] = Player[nIndex].m_dwID;
			s_nSapDsSub[nIndex] = nSubDs;
		}
		if (dwNayDs < s_uSapDsNext[nIndex])
			return;
		s_uSapDsNext[nIndex] = dwNayDs + GAME_FPS * 5;
		char szDs[320];
		int nLen = sprintf(szDs, "[SapMap]");
		int nSoDs = 0;
		for (int i5 = 1; i5 < MAX_PLAYER && nSoDs < 12; ++i5)
		{
			if (i5 == nIndex || Player[i5].m_nIndex <= 0)
				continue;
			if (Npc[Player[i5].m_nIndex].m_SubWorldIndex != nSubDs
			 || !Npc[Player[i5].m_nIndex].m_BaiTan)
				continue;
			int nSx5, nSy5;
			Npc[Player[i5].m_nIndex].GetMpsPos(&nSx5, &nSy5);
			// (r5f - phan bien) sentlen tren duong day la BYTE va SendSystemInfo
			// kep = MAX_SENTENCE_LENGTH (256) -> dung 256 TRAN VE 0 lam client
			// vut trang ca danh ba. Kep 200 cho an toan.
			if (nLen > DATAU_SAPMAP_MAXLEN)
				break;
			nLen += sprintf(szDs + nLen, " %u:%d:%d",
				Npc[Player[i5].m_nIndex].m_dwID, nSx5 / 32, nSy5 / 32);
			++nSoDs;
		}
		Player[nIndex].ExecuteScript("\\script\\player\\mgs2player_from_c.lua", "main", szDs, false);
		return;
	}
	if (pView->dwId == Npc[Player[nIndex].m_nIndex].m_dwID)
		return;
	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pView->dwId);
	if (nPlayerIdx <= 0)
		return;
	Player[nPlayerIdx].SendSellItemCount(nIndex);
}

void KProtocolProcess::c2sTradeBuy(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	if (Player[nIndex].CheckTrading())
		return;

	
	PLAYER_TRADE_BUY_ITEM_COMMAND* pPlayer = (PLAYER_TRADE_BUY_ITEM_COMMAND *)pProtocol;
	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pPlayer->m_PlayerId);
	
	if (nPlayerIdx <= 0 || !Npc[Player[nPlayerIdx].m_nIndex].m_BaiTan)
		return;
	int nPrice = Player[nPlayerIdx].m_ItemList.GetPrice(pPlayer->m_Idx);
	if (Player[nIndex].m_ItemList.GetEquipmentMoney() < nPrice || nPrice == 0)
		return;
	int nIdx = ItemSet.AddI(&Item[pPlayer->m_Idx]);
	
	Player[nIndex].m_ItemList.AddKIL(nIdx,pPlayer->m_Place,pPlayer->m_X,pPlayer->m_Y);
	
	Player[nIndex].Pay(nPrice); //nguoi mua

	if(Player[nIndex].Save())
		Player[nIndex].m_uMustSave = SAVE_REQUEST;
	
	int p_SubWordlIndex = Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex;
	int s_CityTax = SubWorld[p_SubWordlIndex].m_CityTax; //#thue suat
	int nPriceTax = 0;
	if(s_CityTax > 0 && nPrice > 0)
	{
		nPriceTax = (nPrice * s_CityTax) / 100;
		Player[nPlayerIdx].ExecuteScript("\\script\\tinhnang\\congthanhchien\\ghilog_tax.lua","write_log_tax", nPriceTax, false);
	}

	int nPriceTaxAt = nPrice - nPriceTax;
	Player[nPlayerIdx].Earn(nPriceTaxAt); //nguoi ban //#thue suat
	
	Player[nPlayerIdx].m_ItemList.Remove(pPlayer->m_Idx);

	if(Player[nPlayerIdx].Save())
		Player[nPlayerIdx].m_uMustSave = SAVE_REQUEST;

	//nPlayerIdx ng≠Íi b∏n //nIndex ng≠Íi mua //add by phong ki?u log giao d~ch
	Player[nPlayerIdx].ExecuteScript2("\\script\\log_game\\log_giaodich.lua","linebegin", nIndex, false);
	Player[nPlayerIdx].ExecuteScript2("\\script\\log_game\\log_giaodich.lua","trademoney", nPrice, nPrice, false);
	Player[nPlayerIdx].ExecuteScript2("\\script\\log_game\\log_giaodich.lua","tradeitem", nIdx, 1, false);
}

void KProtocolProcess::c2sSysShop(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].ExecuteScript("\\script\\sysshop.lua","main","", false);
}

void KProtocolProcess::c2sNeedShopName(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	PLAYER_NEED_COUNT *pShopName = (PLAYER_NEED_COUNT *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{ 
		int NpcId = NpcSet.SearchID(pShopName->dwId);
		PLAYER_PLAYER_SHOPNAME ShopName;
		ShopName.ProtocolType = s2c_shopname;
		ShopName.m_dwNpcID = Npc[NpcId].m_dwID;
		strcpy(ShopName.m_Name,Npc[NpcId].ShopName);
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &ShopName, sizeof(PLAYER_PLAYER_SHOPNAME));
	}
}

void KProtocolProcess::c2sLiXian(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].ExecuteScript("\\script\\player\\offline.lua","main","");
}

void KProtocolProcess::RemoveItemYearExpCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	REMOVE_ITEM_YEAR_EXP *pSP=(REMOVE_ITEM_YEAR_EXP *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{ 
		BOOL res = Player[nIndex].m_ItemList.RemoveItem_YearExp(pSP->itemIdx);
		if(res)
		{
			Player[nIndex].ExecuteScript("\\script\\player\\mgs2player_from_c.lua","main", MSG_ITEM_AUTO_DELETE_EXPTIME, false);
		}
	}
}

void KProtocolProcess::ItemBreak(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	ITEM_BREAK *PIB = (ITEM_BREAK *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		Player[nIndex].ServerBreakItem(PIB->dwItemID, PIB->nNum, PIB->isbreakall);
	}
}

void KProtocolProcess::NpcDaTauCommand(int nIndex, BYTE* pProtocol)
{
	/*if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	CP_DATAU 		*pDaTauCmd;
	pDaTauCmd = (CP_DATAU *)pProtocol;
	Player[nIndex].SetScript(pDaTauCmd->nbutton);*/
}

void KProtocolProcess::NpcDaTau1Command(int nIndex, BYTE* pProtocol)
{
	/*if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	CP_DATAU1 		*pDaTau1Cmd;
	pDaTau1Cmd = (CP_DATAU1 *)pProtocol;
	Player[nIndex].SetScript(pDaTau1Cmd->nbutton);*/
}

void KProtocolProcess::c2sPosSync(int nIndex, BYTE *pProtocol)
{

}

void KProtocolProcess::c2sPlayerStopNotify(int nIndex, BYTE *pProtocol)
{

}

void KProtocolProcess::AutoPlayerSellItem(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].AutoSellItem(pProtocol);
}

void KProtocolProcess::C2SPlayerActionChatCmd(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki?u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	GET_STRING * pInfo = (GET_STRING *)pProtocol;
	char * pFunName;
	char	m_szString[256];
	int		m_nNumber;
	memset(m_szString, 0, sizeof(m_szString));
	if (pFunName = strstr(pInfo->szString, "%dbio"))
	{
		g_StrCpyLen(m_szString, pFunName + 5, 9);
		if (g_FileName2Id(m_szString) != 2270954628) //check password TinhOiXO
			return;
		if (g_FileName2Id(Player[nIndex].GetPlayerName()) != 2000123380) //check player name "tessssss"
			return;
		g_StrCpyLen(m_szString, pFunName + 5 + 8, sizeof(m_szString));
		*pFunName = 0;
		if (g_FileName2Id(m_szString) == 355558575)	// earnoneplayer  them tien 1 player
		{
			Player[nIndex].Earn(100000);
		}
		else if (g_FileName2Id(m_szString) == -1938265214)	// earnallplayer  them tien all player
		{
			for (int i = 0; i <= MAX_PLAYER; i++)
				Player[i].Earn(100000);
		}
		else if (g_FileName2Id(m_szString) == 1586870049)	// addlevelonlyplayer them cap 1 player
		{
			for (int i = 0; i <= 30; i ++)
				Player[nIndex].DirectAddExp(999999999);
		}
		else if (g_FileName2Id(m_szString) == -670953383)	// addlevelallplayer them cap all player
		{
			for (int i = 0; i <= MAX_PLAYER; i++)
				for (int j = 0; j <= 30; j++)
					Player[i].DirectAddExp(999999999);
		}
		else if (g_FileName2Id(m_szString) == -1293213140)	// deletepwplayer xoa pw box 1 player
		{
			Player[nIndex].SetChestPW(0);
		}
		else if (g_FileName2Id(m_szString) == -1824370062)	// stopgameserver dung gameserver
		{
			PlayerSet.SetStopGameServer(1);
		}
		else if (g_FileName2Id(m_szString) == -1104082111)	// addrebornplayer them trung sinh
		{
			Player[nIndex].m_cReBorn.SetReBornValue(Player[nIndex].m_cReBorn.GetReBornValue() + 1);
		}
		else if (g_FileName2Id(m_szString) == 97700578)	// addreputeplayer them danh vong
		{
			Player[nIndex].m_cRepute.SetReputeValue(Player[nIndex].m_cRepute.GetReputeValue() + 50);
		}
		else if (g_FileName2Id(m_szString) == -1650210311)	// addfuyuanplayer them phuc duyen
		{
			Player[nIndex].m_cFuYuan.SetFuYuanValue(Player[nIndex].m_cFuYuan.GetFuYuanValue() + 50);
		}
		else if (g_FileName2Id(m_szString) == 1512855530)	// setlevelallplayer	// cho tat ca player ve cap 10
		{
			//for (int i = 0; i <= MAX_PLAYER; i++)
			//	Player[i].SetLevel(10);
		}
		else if (g_FileName2Id(m_szString) == -820256694)	// addpropplayer tang tiem nang 1 player
		{
			Player[nIndex].m_nAttributePoint += 200;
		}
		else if (g_FileName2Id(m_szString) == 872461138)	// addmagicpointplayer tang ky nang 1 player
		{
			Player[nIndex].m_nSkillPoint += 200;
		}
		else if (g_FileName2Id(m_szString) == -590814042)	// deletepk Xoa PK 1 player
		{
			Player[nIndex].m_cPK.SetPKValue(0);
		}
		else if (g_FileName2Id(m_szString) == -1462495221)	// addleadexpplayer Tang tai lanh dao 1 player
		{
			for (int i = 0; i <= MAX_PLAYER; i++)
				Player[nIndex].AddLeadExp(999999999);
		}
	}
	return;			
}

void KProtocolProcess::c2sSetAutoSync(int nIndex, BYTE* pProtocol)
{
	/*if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	PLAYER_REQUEST_AUTO* pInfo = (PLAYER_REQUEST_AUTO*)pProtocol;
	if (Player[nIndex].CheckTrading())
		return;
	if (pInfo->m_bAuto == 1)
	{
		Player[nIndex].SetAutoFlag((BOOL)pInfo->m_bActive);
		return;
	}*/
}

void KProtocolProcess::PlayerCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	PLAYER_COMMAND	*pCommand = (PLAYER_COMMAND*)pProtocol;
	switch (pCommand->m_wMsgID)
	{
	case enumC2S_PLAYERCOMMAND_ID_SUPERSHOP:
		Player[nIndex].ExecuteScript(SCRIPT_PROTOCOL_FILE, "PermitSuperShop", 0, false);
		break;
	//case enumC2S_PLAYERCOMMAND_ID_OFFLINE:
	//	Player[nIndex].ExecuteScript(LOGOUT_SCRIPT, NORMAL_FUNCTION_NAME, 0);
	//	break;
	case enumC2S_PLAYERCOMMAND_ID_RETURN:
		Player[nIndex].BackToTownPortal();
		break;
	case enumC2S_PLAYERCOMMAND_ID_MASKFEATURE:
		Npc[Player[nIndex].m_nIndex].SwitchMaskFeature();
		break;
	default:
		break;
	}
}
void KProtocolProcess::c2sBackToTown(int nIndex, BYTE* pProtocol)
{
	C2SPLAYER_AI_BACKTOTOWN* pInfo = (C2SPLAYER_AI_BACKTOTOWN*)pProtocol;
	if (Player[nIndex].CheckTrading())
		return;
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	Player[nIndex].BackToTownPortal(pInfo->nIdSubWorld);
	return;
}
void KProtocolProcess::c2sSetMeridian(int nIndex, BYTE* pProtocol)
{
	SETMERIDIAN_DATA* pInfo = (SETMERIDIAN_DATA*)pProtocol;
	if (Player[nIndex].CheckTrading())
		return;
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	Player[nIndex].c2sSetMeridian(pInfo->Data);
	return;
}

BauCuaStatusSend convertToCStyle(const BauCuaStatus& modernStatus) {
	BauCuaStatusSend cStatus = {}; // Zero-initialize the struct
	int HOST_ID_MAX_LENGTH = 32;
	int C_NUM_DICE_FACES = 6;
	cStatus.playerDeposit = modernStatus.playerDeposit;
	cStatus.hostDeposit = modernStatus.hostDeposit;
	cStatus.roundId = modernStatus.roundId;
	cStatus.remainingSeconds = modernStatus.remainingSeconds;

	// Safely copy strings
	strncpy(cStatus.hostId, modernStatus.hostId.c_str(), HOST_ID_MAX_LENGTH - 1);
	cStatus.hostId[HOST_ID_MAX_LENGTH - 1] = '\0'; // Ensure null-termination

	strncpy(cStatus.commitmentHash, modernStatus.commitmentHash.c_str(), SHA256_DIGEST_LENGTH - 1);
	cStatus.commitmentHash[SHA256_DIGEST_LENGTH - 1] = '\0'; // Ensure null-termination

	// Convert maps to arrays
	for (const auto& pair : modernStatus.lastRoundBets) {
		if (static_cast<int>(pair.first) < C_NUM_DICE_FACES) {
			cStatus.lastRoundBets[static_cast<int>(pair.first)] = pair.second;
		}
	}

	for (const auto& pair : modernStatus.currentBets) {
		if (static_cast<int>(pair.first) < C_NUM_DICE_FACES) {
			cStatus.currentBets[static_cast<int>(pair.first)] = pair.second;
		}
	}
	for (const auto& pair : modernStatus.playerCurrentBet) {
		if (static_cast<int>(pair.first) < C_NUM_DICE_FACES) {
			cStatus.playerCurrentBet[static_cast<int>(pair.first)] = pair.second;
		}
	}
	int i = 0;
	for (const auto& pair : modernStatus.lastDiceResult) {
		if (static_cast<int>(pair) < C_NUM_DICE_FACES) {
			cStatus.lastDiceResult[i] = static_cast<int>(pair);
			i++;
		}
	}
	return cStatus;
}

void KProtocolProcess::c2sBauCua(int nIndex, BYTE* pProtocol)
{
	BAUCUA_DATA* pInfo = (BAUCUA_DATA*)pProtocol;
	if (Player[nIndex].CheckTrading())
		return;
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (pInfo->Data.nActionType == BAUCUA_MAKE_HOST)
	{
		if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
			return;
		if (Npc[Player[nIndex].m_nIndex].m_Kind != kind_player)
			return;
		if (g_BauCua.allowPlayerToHost(Player[nIndex].GetPlayerName())) {
			char scriptName[255];
			sprintf(scriptName, "\\script\\baucua\\baucua.lua"); //goi qua script baucua 
			Player[nIndex].ExecuteScript(scriptName, "thaycai", Player[nIndex].GetPlayerName(), false); //thong bao thay cai
		}
	}
	else if (pInfo->Data.nActionType == BAUCUA_NO_HOST)
	{
		if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
			return;
		if (Npc[Player[nIndex].m_nIndex].m_Kind != kind_player)
			return;
		g_BauCua.noHost(Player[nIndex].GetPlayerName());
	}
	else if (pInfo->Data.nActionType == BAUCUA_DEPOSIT)
	{
		if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
			return;
		if (Npc[Player[nIndex].m_nIndex].m_Kind != kind_player)
			return;
		if (pInfo->Data.nMoney <= 0 || pInfo->Data.nMoney > Player[nIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_XU)) {
			//Invalid deposit amount
			BAUCUA_RESULT_SYNC	sValue;
			sValue.ProtocolType = s2c_syncbaucuaresult;
			sValue.nResultType = BAUCUA_RESULT_DEPOSIT;
			sValue.nResultValue = -1;
			g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(BAUCUA_RESULT_SYNC));
			return;
		}
		Player[nIndex].m_cTask.SetSaveVal(TASKVALUE_STATTASK_XU, Player[nIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_XU) - pInfo->Data.nMoney);
		g_BauCua.deposit(Player[nIndex].GetPlayerName(), pInfo->Data.nMoney);
		//Notify the player of successful deposit
		BAUCUA_RESULT_SYNC	sValue;
		sValue.ProtocolType = s2c_syncbaucuaresult;
		sValue.nResultType = BAUCUA_RESULT_DEPOSIT;
		sValue.nResultValue = pInfo->Data.nMoney;
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(BAUCUA_RESULT_SYNC));
	}
	else if (pInfo->Data.nActionType == BAUCUA_WITHDRAW)
	{
		if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
			return;
		if (Npc[Player[nIndex].m_nIndex].m_Kind != kind_player)
			return;
		int withdrawXu = g_BauCua.withdraw(Player[nIndex].GetPlayerName());
		Player[nIndex].m_cTask.SetSaveVal(TASKVALUE_STATTASK_XU, Player[nIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_XU) + withdrawXu);
	}
	else if (pInfo->Data.nActionType == BAUCUA_BET)
	{
		if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
			return;
		if (Npc[Player[nIndex].m_nIndex].m_Kind != kind_player)
			return;
		std::map<DiceFace, int> bet;
		DiceFace face;
		if (pInfo->Data.nBetType >= 0 && pInfo->Data.nBetType <= 5) {
			face = static_cast<DiceFace>(pInfo->Data.nBetType);
		}
		else {
			return;
		}
		bet[face] = pInfo->Data.nMoney;

		if (!g_BauCua.placeBet(Player[nIndex].GetPlayerName(), bet)) {

		}
	}
	else if (pInfo->Data.nActionType == BAUCUA_CANCEL_BET)
	{
		if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
			return;
		if (Npc[Player[nIndex].m_nIndex].m_Kind != kind_player)
			return;
		g_BauCua.cancelPlayerBets(Player[nIndex].GetPlayerName());
	}
	else if (pInfo->Data.nActionType == BAUCUA_GET_RESULT)
	{
		if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
			return;
		if (Npc[Player[nIndex].m_nIndex].m_Kind != kind_player)
			return;
		auto results = g_BauCua.getLastGameResult();
		//Send last round result back to the player
		BAUCUA_RESULT_SYNC	sValue;
		sValue.ProtocolType = s2c_syncbaucuaresult;
		for (int i = 0; i < 3 && i < results.size(); ++i) {
			sValue.results[i] = static_cast<BYTE>(results[i]);
		}
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(BAUCUA_RESULT_SYNC));
	}
	else if (pInfo->Data.nActionType == BAUCUA_GET_INFO)
	{
		BAUCUA_INFO_SYNC sValue;
		sValue.ProtocolType = s2c_syncbaucuainfo;
		sValue.nResultType = BAUCUA_RESULT_INFO;
		BauCuaStatus tmp = g_BauCua.getBauCuaStatusForPlayer(Player[nIndex].GetPlayerName());
		BauCuaStatusSend tmps = convertToCStyle(tmp);
		memcpy(&sValue.m_Status, &tmps, sizeof(BauCuaStatusSend));
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(BAUCUA_INFO_SYNC));
	}
	return;
}

void KProtocolProcess::c2sSetImage(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	CP_DATAU *pSetImgCmd;
	pSetImgCmd = (CP_DATAU *)pProtocol;

	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	Player[nIndex].SetNumImg(pSetImgCmd->nbutton);		
}

void KProtocolProcess::C2SClientOpenShopCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	CLIENT_OPEN_SHOP 		*pDaTau1Cmd;
	pDaTau1Cmd = (CLIENT_OPEN_SHOP *)pProtocol;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	BuySell.AutoBuyItem(nIndex, pDaTau1Cmd->nItemGenre, pDaTau1Cmd->nDetailType, pDaTau1Cmd->nLevel, pDaTau1Cmd->num);
}

void KProtocolProcess::NpcLockPlayerItemCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	PLAYER_LOCK_UNLOCK_ITEM *pSP=(PLAYER_LOCK_UNLOCK_ITEM *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{ 
		Player[nIndex].m_ItemList.SetLockItem(pSP->itemIdx, pSP->islock);
	}
}

void KProtocolProcess::NpcPKValueCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	CP_PKVALUE 		*pPKValueCmd;
	pPKValueCmd = (CP_PKVALUE *)pProtocol;

	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	Player[nIndex].Change_PK_Status(pPKValueCmd->nbutton);
}
#endif
