#include "KCore.h"

#include "KEngine.h"
#include "KProtocol.h"
#include "KPlayer.h"
#include "KItemList.h"
#include "KLadder.h"

int	g_nProtocolSize[MAX_PROTOCOL_NUM] = 
{
#ifndef _SERVER				// 客户端接收到的服务器到客户端的协议长度
	-1,							// s2c_login,
	-1,							// s2c_logout,
	sizeof(BYTE),				// s2c_syncend,
	sizeof(CURPLAYER_SYNC),		// s2c_synccurplayer,
	-1,							// s2c_synccurplayerskill
	sizeof(CURPLAYER_NORMAL_SYNC),// s2c_synccurplayernormal
	-1,							// s2c_newplayer,
	-1,							// s2c_removeplayer,
	sizeof(WORLD_SYNC),			// s2c_syncworld,
	sizeof(PLAYER_SYNC),		// s2c_syncplayer,
	sizeof(PLAYER_NORMAL_SYNC),	// s2c_syncplayermin,
	-1,	//sizeof(NPC_SYNC),			// s2c_syncnpc,
	sizeof(NPC_NORMAL_SYNC),	// s2c_syncnpcmin,
	sizeof(NPC_PLAYER_TYPE_NORMAL_SYNC),	// s2c_syncnpcminplayer,

	-1,//sizeof(OBJ_ADD_SYNC),	// s2c_objadd,
	sizeof(OBJ_SYNC_STATE),		// s2c_syncobjstate,
	sizeof(OBJ_SYNC_DIR),		// s2c_syncobjdir,
	sizeof(OBJ_SYNC_REMOVE),	// s2c_objremove,
	sizeof(OBJ_SYNC_TRAP_ACT),	// s2c_objTrapAct,

	sizeof(NPC_REMOVE_SYNC),	// s2c_npcremove,
	sizeof(NPC_WALK_SYNC),		// s2c_npcwalk,
	sizeof(NPC_RUN_SYNC),		// s2c_npcrun,
	-1,							// s2c_npcattack,
	-1,							// s2c_npcmagic,
	sizeof(NPC_JUMP_SYNC),		// s2c_npcjump,
	-1,							// s2c_npctalk,
	sizeof(NPC_HURT_SYNC),		// s2c_npchurt,
	sizeof(NPC_DEATH_SYNC),		// s2c_npcdeath,
	sizeof(NPC_CHGCURCAMP_SYNC),// s2c_npcchgcurcamp,
	sizeof(NPC_CHGCAMP_SYNC),	// s2c_npcchgcamp,
	sizeof(NPC_SKILL_SYNC),		// s2c_skillcast,
	-1,							// s2c_playertalk,
	sizeof(PLAYER_EXP_SYNC),	// s2c_playerexp,

	sizeof(PLAYER_SEND_TEAM_INFO),			// s2c_teaminfo,
	sizeof(PLAYER_SEND_SELF_TEAM_INFO),		// s2c_teamselfinfo,
	sizeof(PLAYER_APPLY_TEAM_INFO_FALSE),	// s2c_teamapplyinfofalse,
	sizeof(PLAYER_SEND_CREATE_TEAM_SUCCESS),// s2c_teamcreatesuccess,
	sizeof(PLAYER_SEND_CREATE_TEAM_FALSE),	// s2c_teamcreatefalse,
	sizeof(PLAYER_TEAM_OPEN_CLOSE),			// s2c_teamopenclose,
	sizeof(PLAYER_APPLY_ADD_TEAM),			// s2c_teamgetapply,
	sizeof(PLAYER_TEAM_ADD_MEMBER),			// s2c_teamaddmember,
	sizeof(PLAYER_LEAVE_TEAM),				// s2c_teamleave,
	sizeof(PLAYER_TEAM_CHANGE_CAPTAIN),		// s2c_teamchangecaptain,
	sizeof(PLAYER_FACTION_DATA),			// s2c_playerfactiondata,
	sizeof(PLAYER_LEAVE_FACTION),			// s2c_playerleavefaction,
	sizeof(PLAYER_FACTION_SKILL_LEVEL),		// s2c_playerfactionskilllevel,
	-1,//sizeof(PLAYER_SEND_CHAT_SYNC),			// s2c_playersendchat,
	sizeof(PLAYER_LEAD_EXP_SYNC),			// s2c_playersyncleadexp
	sizeof(PLAYER_LEVEL_UP_SYNC),			// s2c_playerlevelup
	sizeof(PLAYER_TEAMMATE_LEVEL_SYNC),		// s2c_teammatelevel
	sizeof(PLAYER_ATTRIBUTE_SYNC),			// s2c_playersyncattribute
	sizeof(PLAYER_SKILL_LEVEL_SYNC),		// s2c_playerskilllevel
	sizeof(ITEM_SYNC),						// s2c_syncitem
	sizeof(ITEM_REMOVE_SYNC),				// s2c_removeitem
	sizeof(PLAYER_MONEY_SYNC),				// s2c_syncmoney
	sizeof(PLAYER_MOVE_ITEM_SYNC),			// s2c_playermoveitem
	sizeof(PLAYER_SWITCH_EQUIP_SYNC),		// s2c_playerswitchequip
	-1,										// s2c_playershowui
	sizeof(CHAT_APPLY_ADD_FRIEND_SYNC),		// s2c_chatapplyaddfriend
	sizeof(CHAT_ADD_FRIEND_SYNC),			// s2c_chataddfriend
	-1,//sizeof(CHAT_REFUSE_FRIEND_SYNC),		// s2c_chatrefusefriend
	sizeof(CHAT_ADD_FRIEND_FAIL_SYNC),		// s2c_chataddfriendfail
	sizeof(CHAT_LOGIN_FRIEND_NONAME_SYNC),	// s2c_chatloginfriendnoname
	-1,//sizeof(CHAT_LOGIN_FRIEND_NAME_SYNC),	// s2c_chatloginfriendname
	sizeof(CHAT_ONE_FRIEND_DATA_SYNC),		// s2c_chatonefrienddata
	sizeof(CHAT_FRIEND_ONLINE_SYNC),		// s2c_chatfriendinline
	sizeof(CHAT_DELETE_FRIEND_SYNC),		// s2c_chatdeletefriend
	sizeof(CHAT_FRIEND_OFFLINE_SYNC),		// s2c_chatfriendoffline
	sizeof(ROLE_LIST_SYNC),					// s2c_syncrolelist
	sizeof(TRADE_CHANGE_STATE_SYNC),		// s2c_tradechangestate
	sizeof(GAMBLE_CHANGE_STATE_SYNC),		// s2c_gameblechangestate
	-1, // NPC_SET_MENU_STATE_SYNC			   s2c_npcsetmenustate
	sizeof(TRADE_MONEY_SYNC),				// s2c_trademoneysync
	sizeof(TRADE_DECISION_SYNC),			// s2c_tradedecision
	sizeof(GAMBLE_MONEY_SYNC),				// s2c_gamblemoneysync
	sizeof(GAMBLE_DECISION_SYNC),			// s2c_gambledecision
	-1, // sizeof(CHAT_SCREENSINGLE_ERROR_SYNC)s2c_chatscreensingleerror
	-1,	// sizeof(TEAM_INVITE_ADD_SYNC)		   s2c_teaminviteadd
	sizeof(TRADE_STATE_SYNC),				// s2c_tradepressoksync
	sizeof(GAMBLE_STATE_SYNC),				// s2c_gamblepressoksync
	sizeof(PING_COMMAND),					// s2c_ping
	sizeof(NPC_SIT_SYNC),					// s2c_npcsit
	sizeof(SALE_BOX_SYNC),					// s2c_opensalebox
	sizeof(NPC_SKILL_SYNC),					// s2c_castskilldirectly
	-1,										// s2c_msgshow
	-1,										// s2c_syncstateeffect
	sizeof(BYTE),							// s2c_openstorebox
	sizeof(NPC_REVIVE_SYNC),				// s2c_playerrevive
	sizeof(NPC_REQUEST_FAIL),				// s2c_requestnpcfail
	sizeof(TRADE_APPLY_START_SYNC),			// s2c_tradeapplystart
	sizeof(GAMBLE_APPLY_START_SYNC),		// s2c_gambleapplystart
	sizeof(tagNewDelRoleResponse),			// s2c_rolenewdelresponse
	sizeof(ITEM_AUTO_MOVE_SYNC),			// s2c_ItemAutoMove
	sizeof(BYTE),							// s2c_itemexchangefinish
	sizeof(SYNC_WEATHER),					// s2c_changeweather
	sizeof(PK_NORMAL_FLAG_SYNC),			// s2c_pksyncnormalflag
	-1,//sizeof(PK_ENMITY_STATE_SYNC),		// s2c_pksyncenmitystate
	-1,//sizeof(PK_EXERCISE_STATE_SYNC),	// s2c_pksyncexercisestate
	sizeof(PK_VALUE_SYNC),					// s2c_pksyncpkvalue
	sizeof(NPC_SLEEP_SYNC),					// s2c_npcsleepmode
	sizeof(VIEW_EQUIP_SYNC),				// s2c_viewequip
	sizeof(LADDER_DATA),					// s2c_ladderresult
	-1,										// s2c_ladderlist
	sizeof(TONG_CREATE_SYNC),				// s2c_tongcreate
	sizeof(PING_COMMAND),					// s2c_replyclientping
	sizeof(NPC_GOLD_CHANGE_SYNC),			// s2c_npcgoldchange
	sizeof(ITEM_DURABILITY_CHANGE),			// s2c_itemdurabilitychange
	sizeof(RESET_PASS),						// s2c_openresetpass
	sizeof(REPUTE_VALUE_SYNC),				// s2c_reputesyncreputevalue
	sizeof(FUYUAN_VALUE_SYNC),				// s2c_fuyuansyncfuyuanvalue
	sizeof(REBORN_VALUE_SYNC),				// s2c_rebornsyncrebornvalue
	sizeof(VIEW_ITEM_SYNC),					// s2c_viewsellitem
	sizeof(VIEW_ITEM_SYNC),					// s2c_viewupdateitem
	sizeof(PLAYER_GET_COUNT),				// s2c_playergetcount
	sizeof(PLAYER_PLAYER_SHOPNAME),			// s2c_shopname
	sizeof(S2CEXIT_GAME),					// s2c_exitgame
	sizeof(PLAYER_ITEM_LOCK_SYNC),	//s2c_playeritemlocksync
	sizeof(DATAU_BOX),						// s2c_opendataubox
	sizeof(DATAU1_BOX),						// s2c_opendataubox1
	sizeof(S2C_BATTLE_BOX),					// s2c_battlebox 
	sizeof(S2C_SUPERSHOP),					//s2c_syncsupershop
	sizeof(EXTPOINT_VALUE_SYNC),			// s2c_extpointsync
	//sizeof(PLAYER_REQUEST_AUTO),			// s2c_autoplay
	//sizeof(S2C_PLAYER_STOP),				// s2c_player_stop
	//sizeof(S2C_POS_EDITION),				// s2c_pos_edition
	//sizeof(S2C_FINDPATHSYNC),				// s2c_findpathsync
	sizeof(NPC_RIDE_SYNC),					// s2c_ridesync
	sizeof(IGNORE_STATE_SYNC),		//s2c_ignorestate
	sizeof(S2C_GIVE_BOX),					// s2c_openaffairbox
	sizeof(S2C_PLAYER_SYNC_M_A),					// s2c_playersync_magic_attr,
	sizeof(PLAYER_ITEM_TIME_SYNC),					// s2c_playeritemtimesync,
	sizeof(S2C_TIME_BOX),					// s2c_timebox
	sizeof(S2C_TALK_EX),					// s2c_talkex
	sizeof(NPC_SIT_SYNC),					// s2c_syncmasklock
	sizeof(TASK_VALUE_SYNC),				// s2c_taskvalue
	sizeof(PLAYER_MISSION_DATA),			// s2c_playermissiondata,
	sizeof(S2C_INPUT_BOX),					// s2c_inputbox 
	sizeof(S2C_SET_OBSTACLE),					// s2c_setobstacle  //#Set V藅 C秐
	sizeof(RETURN_CITY_OWN_TONG),					// s2c_returncityowntong 
	sizeof(PLAYER_LOGIN_REPLAY),					// s2c_playerloginreplay  //fix by phong ki襲 chuy觧 gs b?m蕋 skill
	sizeof(QUEST_FINISH_DLG_SYNC),			// s2c_openquestfinishdlg
	sizeof(IMAGENPC_VALUE_SYNC),			// s2c_imagenpc
	sizeof(OPEN_TREMBLEITEM),				// s2c_opentrembleitem
	sizeof(BYTE),							// s2c_opencompounditem
	sizeof(PLAYER_MISSION_RANKDATA),		// s2c_syncrankdata
	sizeof(PLAYER_MISSION_RANKDATA2),		// s2c_syncrankdata2
	sizeof(DAMAGESHOW),						// s2c_show_damage
	sizeof(ITEM_SYNC_MAGIC),				// s2c_syncmagic
	sizeof(MERIDIAN_SYNC),				// s2c_syncmeridian
	sizeof(BAUCUA_RESULT_SYNC),				// s2c_syncbaucuaresult
	sizeof(BAUCUA_INFO_SYNC),				// s2c_syncbaucuainfo
	sizeof(S2C_PLAYER_SYNC),				// s2c_playersync
	sizeof(ITEM_REMOVE_SYNC),				// s2c_removeallitem
	sizeof(DICE_ITEM_SYNC),				// s2c_diceitem
	sizeof(ITEM_SYNC_PFPACK),			// s2c_syncpfpack [PFSYNC 31/08]
	sizeof(ITEM_SYNC_FUSION),			// s2c_syncfusion [DUNGLUYEN 01/09]
	sizeof(S2C_REDUCE_SKILL_CD),		// s2c_reduceskillcd [HOASON 01/09b]
	sizeof(S2C_SYNC_VHTD),			// s2c_syncvhtd [VHTD 02/09g]
	sizeof(S2C_DETONATE),			// s2c_detonate [VHTD 02/09w]
	-1,							// s2c_scriptdata [MAIL 03/09] do dai dong: WORD sau ProtocolType
	
#else
	sizeof(LOGIN_COMMAND),		//	c2s_login,
	sizeof(tagLogicLogin),		//	c2s_logicLogin,
	sizeof(BYTE),				//	c2s_syncend,
	-1,							//	c2s_loadplayer,
	-1,	// sizeof(NEW_PLAYER_COMMAND)//	c2s_newplayer,
	-1,							//	c2s_removeplayer,
	-1,							//	c2s_requestworld,
	-1,							//	c2s_requestplayer,
	sizeof(NPC_REQUEST_COMMAND),//	c2s_requestnpc,
	sizeof(OBJ_CLIENT_SYNC_ADD),//	c2s_requestobj,
	sizeof(NPC_WALK_COMMAND),	//	c2s_npcwalk,
	sizeof(NPC_RUN_COMMAND),	//	c2s_npcrun,
	sizeof(NPC_SKILL_COMMAND),	//	c2s_npcskill,
	sizeof(NPC_JUMP_COMMAND),	//	c2s_npcjump,
	-1,							//	c2s_npctalk,
	-1,							//	c2s_dynamic_structure//c2s_npchurt,
	-1,							//	c2s_npcdeath,
	-1,							//	c2s_playertalk,
	sizeof(PLAYER_APPLY_TEAM_INFO),				// c2s_teamapplyinfo,
	sizeof(PLAYER_APPLY_CREATE_TEAM),			// c2s_teamapplycreate,
	sizeof(PLAYER_TEAM_OPEN_CLOSE),				// c2s_teamapplyopenclose,
	sizeof(PLAYER_APPLY_ADD_TEAM),				// c2s_teamapplyadd,
	sizeof(PLAYER_ACCEPT_TEAM_MEMBER),			// c2s_teamacceptmember,
	sizeof(PLAYER_APPLY_LEAVE_TEAM),			// c2s_teamapplyleave,
	sizeof(PLAYER_TEAM_KICK_MEMBER),			// c2s_teamapplykickmember,
	sizeof(PLAYER_APPLY_TEAM_CHANGE_CAPTAIN),	// c2s_teamapplychangecaptain,
	sizeof(PLAYER_APPLY_TEAM_DISMISS),			// c2s_teamapplydismiss,
	sizeof(PLAYER_SET_PK),						// c2s_playerapplysetpk,
	sizeof(PLAYER_APPLY_FACTION_DATA),			// c2s_playerapplyfactiondata,
	-1,//sizeof(PLAYER_SEND_CHAT_COMMAND),		// c2s_playersendchat,
	sizeof(PLAYER_ADD_BASE_ATTRIBUTE_COMMAND),	// c2s_playeraddbaseattribute
	sizeof(PLAYER_ADD_SKILL_POINT_COMMAND),		// c2s_playerapplyaddskillpoint
	sizeof(PLAYER_EAT_ITEM_COMMAND),			// c2s_playereatitem
	sizeof(PLAYER_PICKUP_ITEM_COMMAND),			// c2s_playerpickupitem
	sizeof(PLAYER_MOVE_ITEM_COMMAND),			// c2s_playermoveitem
	sizeof(PLAYER_SELL_ITEM_COMMAND),			// c2s_sellitem
	sizeof(PLAYER_BUY_ITEM_COMMAND),			// c2s_buyitem
	sizeof(PLAYER_THROW_AWAY_ITEM_COMMAND),		// c2s_playerthrowawayitem
	sizeof(PLAYER_SELECTUI_COMMAND),			// c2s_playerselui,
	sizeof(CHAT_SET_CHANNEL_COMMAND),			// c2s_chatsetchannel
	-1,//sizeof(CHAT_APPLY_ADD_FRIEND_COMMAND),		// c2s_chatapplyaddfriend
	sizeof(CHAT_ADD_FRIEND_COMMAND),			// c2s_chataddfriend
	sizeof(CHAT_REFUSE_FRIEND_COMMAND),			// c2s_chatrefusefriend
	sizeof(tagDBSelPlayer),					// c2s_dbplayerselect
	sizeof(CHAT_APPLY_RESEND_ALL_FRIEND_NAME_COMMAND),// c2s_chatapplyresendallfriendname
	sizeof(CHAT_APPLY_SEND_ONE_FRIEND_NAME_COMMAND),// c2s_chatapplysendonefriendname
	sizeof(CHAT_DELETE_FRIEND_COMMAND),			// c2s_chatdeletefriend
	sizeof(CHAT_REDELETE_FRIEND_COMMAND),		// c2s_chatredeletefriend
	-1, // TRADE_APPLY_OPEN_COMMAND				// c2s_tradeapplystateopen
	sizeof(TRADE_APPLY_CLOSE_COMMAND),			// c2s_tradeapplystateclose
	sizeof(TRADE_APPLY_START_COMMAND),			// c2s_tradeapplystart
	sizeof(TRADE_MOVE_MONEY_COMMAND),			// c2s_trademovemoney
	sizeof(TRADE_DECISION_COMMAND),				// c2s_tradedecision
	-1, // GAMBLE_APPLY_OPEN_COMMAND				// c2s_gambleapplystateopen
	sizeof(GAMBLE_APPLY_CLOSE_COMMAND),			// c2s_gambleapplystateclose
	sizeof(GAMBLE_APPLY_START_COMMAND),			// c2s_gambleapplystart
	sizeof(GAMBLE_MOVE_MONEY_COMMAND),			// c2s_gamblemovemoney
	sizeof(GAMBLE_DECISION_COMMAND),				// c2s_gambledecision
	sizeof(PLAYER_DIALOG_NPC_COMMAND),			// c2s_dialognpc
	sizeof(TEAM_INVITE_ADD_COMMAND),			// c2s_teaminviteadd
	sizeof(SKILL_CHANGEAURASKILL_COMMAND),		// c2s_changeauraskill
	sizeof(TEAM_REPLY_INVITE_COMMAND),			// c2s_teamreplyinvite
	sizeof(PING_CLIENTREPLY_COMMAND),			// c2s_ping
	sizeof(NPC_SIT_COMMAND),					// c2s_npcsit
	sizeof(OBJ_MOUSE_CLICK_SYNC),				// c2s_objmouseclick
	sizeof(STORE_MONEY_COMMAND),				// c2s_storemoney
	sizeof(NPC_REVIVE_COMMAND),					// c2s_playerrevive
	sizeof(TRADE_REPLY_START_COMMAND),			// c2s_tradereplystart
	sizeof(GAMBLE_REPLY_START_COMMAND),			// c2s_gamblereplystart
	sizeof(PK_APPLY_NORMAL_FLAG_COMMAND),		// c2s_pkapplychangenormalflag
	sizeof(PK_APPLY_ENMITY_COMMAND),			// c2s_pkapplyenmity
	sizeof(VIEW_EQUIP_COMMAND),					// c2s_viewequip
	sizeof(LADDER_QUERY),						// c2s_ladderquery
	sizeof(ITEM_REPAIR),						// c2s_repairitem
	sizeof(NPC_RIDE_COMMAND),					// edit by phong kieu len xuong ngua
	sizeof(PLAYER_REQUEST_CP_UNLOCK),			// c2s_cpunlock
	sizeof(PLAYER_REQUEST_CP_LOCK),				// c2s_cplock
	sizeof(PLAYER_REQUEST_SWITCH_EQUIP_SET),	// c2s_cpswitchequipset
	sizeof(PLAYER_REQUEST_CP_CHANGE),			// c2s_cpchange
	sizeof(PLAYER_REQUEST_CP_RESET),			// c2s_cpreset
	sizeof(PLAYER_SET_PRICE),
	sizeof(PLAYER_START_TRADE),
	sizeof(VIEW_EQUIP_COMMAND),					// c2s_viewitem
	sizeof(PLAYER_TRADE_BUY_ITEM_COMMAND),	// c2s_playertradebuyitem
	sizeof(VIEW_EQUIP_COMMAND),				// c2s_updateviewitem
	sizeof(PLAYER_NEED_COUNT),				// c2s_playerneedcount
	sizeof(PLAYER_NEED_COUNT),				// c2s_playershopname
	sizeof(TONG_DISMISS_SYNC),				// c2s_playersysshop
	sizeof(TONG_DISMISS_SYNC),				// c2s_playerlixian
	sizeof(CP_PKVALUE),									// c2s_pkvalue
	sizeof(PLAYER_LOCK_UNLOCK_ITEM),		// c2s_plockitem
	sizeof(REMOVE_ITEM_YEAR_EXP),		// c2s_itemyearexp
	sizeof(ITEM_BREAK),							// c2s_breakitem
	//sizeof(CP_DATAU),							// c2s_datau
	//sizeof(CP_DATAU1),							// c2s_datau1
	sizeof(PLAYER_COMMAND),							//c2s_playercommand
	//sizeof(PLAYER_REQUEST_AUTO),							//c2s_autoplay
	sizeof(GET_STRING),							// c2s_playeractionchat
	sizeof(CLIENT_OPEN_SHOP),							// c2s_openshop
	sizeof(CP_DATAU),					// c2s_cpsetimage
	sizeof(PLAYER_SELL_ITEM_COMMAND),					// c2s_autoplayersellitem
	//sizeof(C2S_POS_SYNC),						// c2s_player_pos_sync
	//sizeof(C2S_PLAYER_STOP_NOTIFY),				// c2s_playerstop
	sizeof(PLAYER_UI_CMD_SCRIPT),				// c2s_uicmdscript
	sizeof(RECOVERY_BOX_CMD),					// c2s_recoverybox
	sizeof(C2S_PLAYER_INPUT_INFO),				// c2s_inputinfo
	sizeof(GET_CITY_OWN_TONG),				// c2s_getcityowntong
	sizeof(RECOVER_ITEM_COMMAND),				// c2s_recoveritem
	sizeof(PLAYER_THROW_ALL_ITEM_COMMAND),		// c2s_playerthrowallitem
	sizeof(C2SPLAYER_AI_BACKTOTOWN),			// c2s_aibacktotown
	sizeof(SETMERIDIAN_DATA),			// c2s_setmeridian
	sizeof(BAUCUA_DATA),					// c2s_baucua
	sizeof(DICE_CHOICE_DATA),				// c2s_diceitem
	sizeof(PARTNER_OP_DATA),				// c2s_partnerop [BDH-G4]
	-1,							// c2s_scriptdata [MAIL 03/09] do dai dong: WORD sau ProtocolType

#endif
};

void g_InitProtocol()
{
#ifdef _SERVER
	g_nProtocolSize[c2s_extend - c2s_gameserverbegin - 1] = -1;
	g_nProtocolSize[c2s_extendchat - c2s_gameserverbegin - 1] = -1;
	g_nProtocolSize[c2s_extendfriend - c2s_gameserverbegin - 1] = -1;
	g_nProtocolSize[c2s_extendtong - c2s_gameserverbegin - 1] = -1;
#else
	g_nProtocolSize[s2c_extend - s2c_clientbegin - 1] = -1;
	g_nProtocolSize[s2c_extendchat - s2c_clientbegin - 1] = -1;
	g_nProtocolSize[s2c_extendfriend - s2c_clientbegin - 1] = -1;
	g_nProtocolSize[s2c_extendtong - s2c_clientbegin - 1] = -1;
#endif
}

#ifndef _SERVER
//#include "KNetClient.h"
#include "../../Headers/IClient.h"
#include "KCore.h"

// [S12b 28/08] xem KProtocolProcess.cpp: phan biet echo voi lenh dat-di cua server.
extern int g_nS12TuGuiX, g_nS12TuGuiY;
extern DWORD g_uS12TuGuiTick;   // [S12c] moc lan tu-gui gan nhat
void SendClientCmdRun(int nX, int nY)
{
	NPC_RUN_COMMAND	NetCommand;
	g_nS12TuGuiX = nX; g_nS12TuGuiY = nY; g_uS12TuGuiTick = timeGetTime();
	NetCommand.ProtocolType = (BYTE)c2s_npcrun;
	NetCommand.nMpsX = nX;
	NetCommand.nMpsY = nY;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&NetCommand, sizeof(NetCommand));
}

void SendClientCmdWalk(int nX, int nY)
{
	NPC_WALK_COMMAND	NetCommand;
	g_nS12TuGuiX = nX; g_nS12TuGuiY = nY; g_uS12TuGuiTick = timeGetTime();
	NetCommand.ProtocolType = (BYTE)c2s_npcwalk;
	NetCommand.nMpsX = nX;
	NetCommand.nMpsY = nY;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&NetCommand, sizeof(NetCommand));
}

void SendClientCmdSkill(int nSkillID, int nX, int nY)
{
	NPC_SKILL_COMMAND	NetCommand;
	
	NetCommand.ProtocolType = (BYTE)c2s_npcskill;
	NetCommand.nSkillID = nSkillID;
	AUTOLOG("NET-SKILL-PKT skill=%d x=%d y=%d bytgtid=%d cli=%d t=%u", nSkillID, nX, nY, (int)(nX == -1), (int)(g_pClient != NULL), GetTickCount());
#ifndef _SERVER
	// [S6 25/08] nX == -1 nghia la danh THEO ID MUC TIEU (nY = id). Neu client KHONG tim
	// duoc id do trong bang NPC cua minh (thay=0) thi nguoi choi khong nhin thay ai ca
	// nhung nhan vat van vung -> dung trieu chung "Tong Kim dung danh vao khong khi".
	if (nX == -1)
	{
		// [S6 26/08] thay>0 ma tgreg=-1 = muc tieu MO COI (con trong bang, KHONG duoc ve)
		// -> "dung danh vao khong khi" ma phien ban thay=0 cu khong bat duoc.
		int nS6TgIdx = NpcSet.SearchID((DWORD)nY);
		AUTOLOG("[S6-ATK] skill=%d tgtid=%d thay=%d tgreg=%d tgdoing=%d tgcell=(%d,%d) t=%u", nSkillID, nY, nS6TgIdx, (nS6TgIdx > 0) ? Npc[nS6TgIdx].m_RegionIndex : -99, (nS6TgIdx > 0) ? (int)Npc[nS6TgIdx].m_Doing : -1, (nS6TgIdx > 0) ? Npc[nS6TgIdx].m_MapX : -1, (nS6TgIdx > 0) ? Npc[nS6TgIdx].m_MapY : -1, GetTickCount());
	}
#endif
	NetCommand.nMpsX = nX;
	NetCommand.nMpsY = nY;

	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&NetCommand, sizeof(NPC_SKILL_COMMAND));	
}

void SendClientCmdRequestNpc(int nID)
{
	NPC_REQUEST_COMMAND NpcRequest;
	
	NpcRequest.ProtocolType = c2s_requestnpc;
	NpcRequest.ID = nID;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&NpcRequest, sizeof(NPC_REQUEST_COMMAND));

}

void SendClientCmdSell(int nId)
{
	//if (Player[CLIENT_PLAYER_INDEX].m_ItemList.IsLockOperation())
	//	return;
	PLAYER_SELL_ITEM_COMMAND PlayerSell;
	PlayerSell.ProtocolType = c2s_playersellitem;
	PlayerSell.m_ID = nId;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&PlayerSell, sizeof(PLAYER_SELL_ITEM_COMMAND));
//	Player[CLIENT_PLAYER_INDEX].m_ItemList.LockOperation();
}

void SendClientCmdAutoSell(int nId)
{
	//if (Player[CLIENT_PLAYER_INDEX].m_ItemList.IsLockOperation())
	//	return;
	PLAYER_SELL_ITEM_COMMAND PlayerSell;
	PlayerSell.ProtocolType = c2s_autoplayersellitem;
	PlayerSell.m_ID = nId;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&PlayerSell, sizeof(PLAYER_SELL_ITEM_COMMAND));
	//Player[CLIENT_PLAYER_INDEX].m_ItemList.LockOperation();
}

void SendClientCmdBuy(int nShop, int nBuyIdx, int  nNumber, int bSupperS)
{
	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.IsLockOperation() && !bSupperS)
	{
		return;
	}
	//
	PLAYER_BUY_ITEM_COMMAND PlayerBuy;
	PlayerBuy.ProtocolType = c2s_playerbuyitem;
	PlayerBuy.m_Shop = nShop;
	PlayerBuy.m_BuyIdx = (BYTE)nBuyIdx;
	PlayerBuy.m_Number = nNumber;
	//
	if (g_pClient)
	{
		g_pClient->SendPackToServer((BYTE*)&PlayerBuy, sizeof(PLAYER_BUY_ITEM_COMMAND));
	}
	//
	if(!bSupperS) // ch?th鵦 hi謓 v韎 SHOP thng
	{
		Player[CLIENT_PLAYER_INDEX].m_ItemList.LockOperation();
	}
}

//void SendClientCmdPing()
//{
//	PING_COMMAND PingCmd;
//
//	PingCmd.ProtocolType = c2s_ping;
//	PingCmd.m_dwTime = GetTickCount();
//	if (g_pClient && g_bPingReply)
//	{
//		g_pClient->SendPackToServer((BYTE*)&PingCmd, sizeof(PING_COMMAND));
//		g_bPingReply = FALSE;
//	}
//}

void SendClientCmdSit(int nSitFlag)
{
	NPC_SIT_COMMAND SitCmd;

	SitCmd.ProtocolType = c2s_npcsit;
	SitCmd.m_btSitFlag = (nSitFlag != 0);
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&SitCmd, sizeof(NPC_SIT_COMMAND));
}

void SendClientCmdJump(int nMpsX, int nMpsY)
{
	NPC_JUMP_COMMAND JumpCmd;

	JumpCmd.nMpsX = nMpsX;
	JumpCmd.nMpsY = nMpsY;
	JumpCmd.ProtocolType = c2s_npcjump;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&JumpCmd, sizeof(NPC_JUMP_COMMAND));
}

void SendObjMouseClick(int nObjID, DWORD dwRegionID)
{
	OBJ_MOUSE_CLICK_SYNC	sObj;
	sObj.ProtocolType = c2s_objmouseclick;
	sObj.m_dwRegionID = dwRegionID;
	sObj.m_nObjID = nObjID;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&sObj, sizeof(OBJ_MOUSE_CLICK_SYNC));
}

void SendClientCmdStoreMoney(int nDir, int nMoney)
{
	STORE_MONEY_COMMAND	StoreMoneyCmd;

	StoreMoneyCmd.ProtocolType = c2s_storemoney;
	StoreMoneyCmd.m_byDir = (BYTE)nDir;
	StoreMoneyCmd.m_dwMoney = nMoney;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&StoreMoneyCmd, sizeof(STORE_MONEY_COMMAND));
}

void SendClientCmdRevive()
{
	NPC_REVIVE_COMMAND	ReviveCmd;

	ReviveCmd.ProtocolType = c2s_playerrevive;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE *)&ReviveCmd, sizeof(NPC_REVIVE_COMMAND));
}

void SendClientCmdMoveItem(void* pDownPos, void* pUpPos)
{
	if (!pDownPos || !pUpPos)
		return;

	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.IsLockOperation())
		return;

	ItemPos* pos1 = (ItemPos *)pDownPos;
	ItemPos* pos2 = (ItemPos *)pUpPos;

	PLAYER_MOVE_ITEM_COMMAND	sMove;
	sMove.ProtocolType = c2s_playermoveitem;
	sMove.m_btDownPos = pos1->nPlace;
	sMove.m_btDownX = pos1->nX;
	sMove.m_btDownY = pos1->nY;
	sMove.m_btUpPos = pos2->nPlace;
	sMove.m_btUpX = pos2->nX;
	sMove.m_btUpY = pos2->nY;

	if (g_pClient)
		g_pClient->SendPackToServer(&sMove, sizeof(PLAYER_MOVE_ITEM_COMMAND));

	Player[CLIENT_PLAYER_INDEX].m_ItemList.LockOperation();
}

void SendClientCmdQueryLadder(DWORD	dwLadderID)
{
	if (dwLadderID <= enumLadderBegin || dwLadderID >= enumLadderEnd)
		return;

	if (g_pClient)
	{
		LADDER_QUERY	LadderQuery;
		LadderQuery.ProtocolType = c2s_ladderquery;
		LadderQuery.dwLadderID = dwLadderID;
		g_pClient->SendPackToServer(&LadderQuery, sizeof(LADDER_QUERY));
	}
}

void SendClientCmdRepair(DWORD dwID)
{
	ITEM_REPAIR ItemRepair;
	ItemRepair.ProtocolType = c2s_repairitem;
	ItemRepair.dwItemID = dwID;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&ItemRepair, sizeof(ITEM_REPAIR));
}

void SendClientCmdRide(BOOL nFlagRide) // len xuong ngua
{
	NPC_RIDE_COMMAND RideCmd;
	RideCmd.ProtocolType = c2s_npcride;
	RideCmd.m_btRideFlag = nFlagRide;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&RideCmd, sizeof(NPC_RIDE_COMMAND));
}

void SendClientCPUnlockCmd(int CP_IntPW)
{
	PLAYER_REQUEST_CP_UNLOCK UlockCmd;

	UlockCmd.ProtocolType = c2s_cpunlock;
	UlockCmd.int_PW = CP_IntPW;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&UlockCmd, sizeof(PLAYER_REQUEST_CP_UNLOCK));
}

void SendClientCPLockCmd()
{
	PLAYER_REQUEST_CP_LOCK LockCmd;

	LockCmd.ProtocolType = c2s_cplock;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&LockCmd, sizeof(PLAYER_REQUEST_CP_LOCK));
}

void SendClientSwitchEquipSetCmd(int setnum)
{
	PLAYER_REQUEST_SWITCH_EQUIP_SET SwitchEquipSetCmd;

	SwitchEquipSetCmd.ProtocolType = c2s_cpswitchequipset;
	SwitchEquipSetCmd.byte_setnum = setnum;// 1 or 2
	Player[CLIENT_PLAYER_INDEX].m_ItemList.LockOperation();
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&SwitchEquipSetCmd, sizeof(PLAYER_REQUEST_SWITCH_EQUIP_SET));

}
void SendClientCPChangeCmd(int oldPW, int newPW)
{
	PLAYER_REQUEST_CP_CHANGE ChangePWCmd;

	ChangePWCmd.ProtocolType = c2s_cpchange;
	ChangePWCmd.int_OldPW = oldPW;
	ChangePWCmd.int_NewPW = newPW;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&ChangePWCmd, sizeof(PLAYER_REQUEST_CP_CHANGE));
}

void SendClientLockPlayerItemCmd(int ItemIdx, int lock)
{
	PLAYER_LOCK_UNLOCK_ITEM lockCmd;

	lockCmd.ProtocolType = c2s_plockitem;
	lockCmd.itemIdx = Item[ItemIdx].GetID();
	lockCmd.islock = lock;

	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&lockCmd, sizeof(PLAYER_LOCK_UNLOCK_ITEM));
}

void SendClientCPResetCmd(int resetPW)
{
	PLAYER_REQUEST_CP_RESET ResetPWCmd;

	ResetPWCmd.ProtocolType = c2s_cpreset;
	ResetPWCmd.int_ResetPW = resetPW;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&ResetPWCmd, sizeof(PLAYER_REQUEST_CP_RESET));
}

void SendClientCmdStartTrade(int nStart, char* sName)
{
	PLAYER_START_TRADE PlayerTrade;
	PlayerTrade.ProtocolType =c2s_playerstarttrade;
	PlayerTrade.m_bSet = nStart;
	strcpy(PlayerTrade.m_Name,sName);
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&PlayerTrade, sizeof(PLAYER_START_TRADE));
}
void SendClientCmdGetCount( DWORD dwId )
{
	PLAYER_NEED_COUNT	PlayerCount;
	PlayerCount.ProtocolType = c2s_playerneedcount;
	PlayerCount.dwId = dwId;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&PlayerCount, sizeof(PLAYER_NEED_COUNT));
}
void SendClientCmdPlayerBuy(int nIdx, DWORD nPlayerId, int nPlace, int nX, int nY)
{
	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.IsLockOperation())
		return;
	PLAYER_TRADE_BUY_ITEM_COMMAND PlayerBuy;
	PlayerBuy.ProtocolType = c2s_playertradebuyitem;
	PlayerBuy.m_Idx = nIdx;  // s鱝 ch?n祔 
	PlayerBuy.m_PlayerId = nPlayerId;
	PlayerBuy.m_Place = (BYTE)nPlace;
	PlayerBuy.m_X = (BYTE)nX;
	PlayerBuy.m_Y = (BYTE)nY;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&PlayerBuy, sizeof(PLAYER_TRADE_BUY_ITEM_COMMAND));
	Player[CLIENT_PLAYER_INDEX].m_ItemList.LockOperation();  // l鏸 b祔 b竛 mua k輈h nh譽 l莕 l?ko mua 甤 n鱝 ph秈 tho竧 game 
}

void SendClientCmdSetPrice(int nId, int nPrice)
{
	PLAYER_SET_PRICE PlayerPrice;
	PlayerPrice.ProtocolType = c2s_playersetprice;
	PlayerPrice.m_ID = nId;
	PlayerPrice.m_Price = nPrice;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&PlayerPrice, sizeof(PLAYER_SET_PRICE));
}

void SendClientCmdShopName(DWORD dwId)
{
	PLAYER_NEED_COUNT ShopName;
	ShopName.ProtocolType = c2s_playershopname;
	ShopName.dwId = dwId;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&ShopName, sizeof(PLAYER_NEED_COUNT));
}

void SendClientCmdSysShop()
{
	TONG_DISMISS_SYNC SysShop;
	SysShop.ProtocolType = c2s_playersysshop;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&SysShop, sizeof(TONG_DISMISS_SYNC));
}

void SendClientCmdLiXian()
{
	TONG_DISMISS_SYNC SysShop;
	SysShop.ProtocolType = c2s_playerlixian;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&SysShop, sizeof(TONG_DISMISS_SYNC));
}

void SendClientPKValue(int nstt) //pkvalue
{
	CP_PKVALUE PKValueCmd;

	PKValueCmd.ProtocolType = c2s_pkvalue;
	PKValueCmd.nbutton = nstt;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&PKValueCmd, sizeof(CP_PKVALUE));
}

void SendClientRecoveryBox(DWORD dwID, int nX, int nY)
{
	RECOVERY_BOX_CMD Cmd;
	Cmd.ProtocolType = c2s_recoverybox;
	Cmd.dwID   = dwID;
	Cmd.nX	   = nX;
	Cmd.nY     = nY;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&Cmd, sizeof(RECOVERY_BOX_CMD));
}

void SendClientCmdGetCityOwnTong()
{
	GET_CITY_OWN_TONG Cmd;
	Cmd.ProtocolType = c2s_getcityowntong;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&Cmd, sizeof(GET_CITY_OWN_TONG));
}

void SendClientCmdRecoverItem(int nPos)
{	
	RECOVER_ITEM_COMMAND RecoverItem;
	RecoverItem.ProtocolType = c2s_recoveritem;
	RecoverItem.dwID = Player[CLIENT_PLAYER_INDEX].GetPlayerID();						
	RecoverItem.dwTimePacker = GetTickCount();	
	RecoverItem.m_nPos = nPos;
		
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&RecoverItem, sizeof(RECOVER_ITEM_COMMAND));
}

void SendUiCmdScript(int nType, char*szFunc)
{
	PLAYER_UI_CMD_SCRIPT Cmd;
	Cmd.ProtocolType = c2s_uicmdscript;
	Cmd.nType = nType;
	strcpy(Cmd.szFunc, szFunc);
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&Cmd, sizeof(PLAYER_UI_CMD_SCRIPT));
}

void SendClientCmdInputBox(BYTE Value1,int nNum,char* szAction,char*szFunc)
{
	C2S_PLAYER_INPUT_INFO pInput;
	pInput.ProtocolType = c2s_inputinfo;
	pInput.nType = Value1;
	strncpy(pInput.nValue, szFunc, sizeof(pInput.nValue));
	strncpy(pInput.nAction, szAction, sizeof(pInput.nAction));
	pInput.nNum = nNum;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&pInput, sizeof(C2S_PLAYER_INPUT_INFO));
}

void SendClientCmdBreak(DWORD dwID, int nNum, bool isbreakall)
{
	ITEM_BREAK ItemBreak;
	ItemBreak.ProtocolType = c2s_breakitem;
	ItemBreak.dwItemID = dwID;
	ItemBreak.nNum = nNum;
	ItemBreak.isbreakall = isbreakall;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&ItemBreak, sizeof(ITEM_BREAK));
}

/*void SendClientDaTau(int nstt)
{
	CP_DATAU ButtonCmd;

	ButtonCmd.ProtocolType = c2s_datau;
	ButtonCmd.nbutton = nstt;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&ButtonCmd, sizeof(CP_DATAU));
}

void SendClientDaTau1(int nstt)
{
	CP_DATAU1 ButtonCmd;

	ButtonCmd.ProtocolType = c2s_datau1;
	ButtonCmd.nbutton = nstt;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&ButtonCmd, sizeof(CP_DATAU1));
}*/

void SendClientActionChatCmd(char* zString)
{
	GET_STRING GetStringCmd;

	GetStringCmd.ProtocolType = c2s_playeractionchat;
	strcpy(GetStringCmd.szString,zString);
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&GetStringCmd, sizeof(GET_STRING));
}

void SendClientSetMeridian(char* Data) {
	SETMERIDIAN_DATA SetMeridianData;

	SetMeridianData.ProtocolType = c2s_setmeridian;
	memcpy((void *)&SetMeridianData.Data, Data, sizeof(SetMeridianData.Data));
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&SetMeridianData, sizeof(SETMERIDIAN_DATA));
}

void SendClientBaucua(char* Data)
{
	BAUCUA_DATA BauCuaData;

	BauCuaData.ProtocolType = c2s_baucua;
	memcpy((void*)&BauCuaData.Data, Data, sizeof(BauCuaData.Data));
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&BauCuaData, sizeof(BAUCUA_DATA));
}

// He XUC XAC chia do (DICEITEM): nguoi choi chon "Tham du nhan" / "Huy bo nhan".
// Truyen thang hai so thay vi con tro - khoi lo vong doi bo nho ben UI.
void SendClientDiceItem(int nDiceId, int nChoice)
{
	DICE_CHOICE_DATA	Data;

	Data.ProtocolType = (BYTE)c2s_diceitem;
	Data.m_nDiceId    = nDiceId;
	Data.m_btChoice   = (BYTE)((nChoice == DICE_CHOICE_NEED)
		? DICE_CHOICE_NEED : DICE_CHOICE_GIVEUP);
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&Data, sizeof(DICE_CHOICE_DATA));
}

// [BDH-G4] Cua so / thanh nhanh Ban Dong Hanh gui lenh len may chu.
void SendClientPartnerOp(int nOp, int nParam, const char* szName)
{
	PARTNER_OP_DATA	Data;

	memset(&Data, 0, sizeof(Data));
	Data.ProtocolType = (BYTE)c2s_partnerop;
	Data.btOp = (BYTE)nOp;
	Data.nParam = nParam;
	if (szName)
		strncpy(Data.szName, szName, PARTNER_OP_NAME_LEN - 1);
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&Data, sizeof(PARTNER_OP_DATA));
}

void SendClientCPSetImageCmd(int ID)
{
	CP_DATAU SetImageCmd;

	SetImageCmd.ProtocolType = c2s_cpsetimage;
	SetImageCmd.nbutton = ID;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&SetImageCmd, sizeof(CP_DATAU));
}

void SendClientCmdOpenShop(BYTE nItemGenre, BYTE nDetailType, BYTE	nLevel, BYTE num)
{
	for(int i=0; i < num; i++)
	{
		CLIENT_OPEN_SHOP GetStringCmd;
		GetStringCmd.ProtocolType = c2s_openshop;
		GetStringCmd.nItemGenre = nItemGenre;
		GetStringCmd.nDetailType = nDetailType;
		GetStringCmd.nLevel = nLevel;
		GetStringCmd.num = 1;
		if (g_pClient)
		{
			g_pClient->SendPackToServer((BYTE*)&GetStringCmd, sizeof(CLIENT_OPEN_SHOP));
		}
	}
}

/*void SendClientCmdAutoPlay(BOOL nbAuto, BOOL nbActive)
{
	PLAYER_REQUEST_AUTO AutoCmd;

	AutoCmd.ProtocolType = c2s_autoplay;
	AutoCmd.m_bAuto = nbAuto;
	AutoCmd.m_bActive = nbActive;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&AutoCmd, sizeof(PLAYER_REQUEST_AUTO));
}*/

#endif

#ifdef _SERVER
//#include "KNetServer.h"
//#include "NetWork\JXServer.h"
void SendServerCmdWalk(int nX, int nY)
{
}

void SendServerCmdRun(int nX, int nY)
{
}
#endif

