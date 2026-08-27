#ifndef	KProtocolDefH
#define	KProtocolDefH

// Add by Fong Ki“u in 2020.7.1 PHIEN BAN GAME 127 Phi™n b∂n game
#define USE_KPROTOCOL_VERSION   1
//#undef USE_KPROTOCOL_VERSION
#define KPROTOCOL_EXPIRATION_DATE   20261230 // y/m/d
#define KPROTOCOL_VERSION   20240430

/*
 * It was to judge a package type that 
 * it is a larger package or it is a small package
 */
const UINT g_nGlobalProtocolType = 31;

enum s2c_PROTOCOL
{
	s2c_roleserver_getroleinfo_result = 10,
	s2c_syncgamesvr_roleinfo_cipher,
	s2c_gamestatistic_bigpackage,
	s2c_syncgamesvr_roleinfo_lixian,//#uy thac //kick out account lixian
	/*
	 * This value must be equal to c2s_micropackbegin
	 */
	s2c_micropackbegin = g_nGlobalProtocolType,

	s2c_accountbegin = 32,
	s2c_accountlogin,
	s2c_gamelogin,
	s2c_accountlogout,
	s2c_gatewayverify,		
	s2c_gatewayverifyagain,		
	s2c_gatewayinfo,

	s2c_multiserverbegin = 48,
	s2c_querymapinfo,
	s2c_querygameserverinfo,
	s2c_identitymapping,
	s2c_notifyplayerlogin,
	s2c_notifyplayerexchange,
	s2c_notifysvrip,
	
	s2c_roleserver_getrolelist_result,
	s2c_roleserver_saverole_result,
	s2c_roleserver_createrole_result,
	s2c_roleserver_deleterole_result,
	s2c_logiclogout,
	s2c_gateway_broadcast,
	s2c_gamestatistic,
	
	s2c_clientbegin = 64,
	s2c_login,		//ref: ../../S3Client/Login/LoginDef.h
	s2c_logout,
	s2c_syncend,
	s2c_synccurplayer,
	s2c_synccurplayerskill,
	s2c_synccurplayernormal,
	s2c_newplayer,
	s2c_removeplayer,
	s2c_syncworld,
	s2c_syncplayer,
	s2c_syncplayermin,
	s2c_syncnpc,
	s2c_syncnpcmin,
	s2c_syncnpcminplayer,
	s2c_objadd,
	s2c_syncobjstate,
	s2c_syncobjdir,
	s2c_objremove,
	s2c_objTrapAct,
	s2c_npcremove,
	s2c_npcwalk,
	s2c_npcrun,
	s2c_npcattack,
	s2c_npcmagic,
	s2c_npcjump,
	s2c_npctalk,
	s2c_npchurt,
	s2c_npcdeath,
	s2c_npcchgcurcamp,
	s2c_npcchgcamp,
	s2c_skillcast,
	s2c_playertalk,
	s2c_playerexp,
	s2c_teaminfo,
	s2c_teamselfinfo,
	s2c_teamapplyinfofalse,
	s2c_teamcreatesuccess,
	s2c_teamcreatefalse,
	s2c_teamopenclose,
	s2c_teamgetapply,
	s2c_teamaddmember,
	s2c_teamleave,
	s2c_teamchangecaptain,
	s2c_playerfactiondata,
	s2c_playerleavefaction,
	s2c_playerfactionskilllevel,
	s2c_playersendchat,
	s2c_playersyncleadexp,
	s2c_playerlevelup,
	s2c_teammatelevel,
	s2c_playersyncattribute,
	s2c_playerskilllevel,
	s2c_syncitem,
	s2c_removeitem,
	s2c_syncmoney,
	s2c_playermoveitem,
	s2c_playerswitchequip,
	s2c_scriptaction,
	s2c_chatapplyaddfriend,
	s2c_chataddfriend,
	s2c_chatrefusefriend,
	s2c_chataddfriendfail,
	s2c_chatloginfriendnoname,
	s2c_chatloginfriendname,
	s2c_chatonefrienddata,
	s2c_chatfriendonline,
	s2c_chatdeletefriend,
	s2c_chatfriendoffline,
	s2c_syncrolelist,
	s2c_tradechangestate,
	s2c_gamblechangestate,
	s2c_npcsetmenustate,
	s2c_trademoneysync,
	s2c_tradedecision,
	s2c_gamblemoneysync,
	s2c_gambledecision,
	s2c_chatscreensingleerror,
	s2c_teaminviteadd,
	s2c_tradepressoksync,
	s2c_gamblepressoksync,
	s2c_ping,
	s2c_npcsit,
	s2c_opensalebox,
	s2c_castskilldirectly,
	s2c_msgshow,
	s2c_syncstateeffect,
	s2c_openstorebox,
	s2c_playerrevive,
	s2c_requestnpcfail,
	s2c_tradeapplystart,
	s2c_gambleapplystart,
	s2c_rolenewdelresponse,	//tagNewDelRoleResponse
	s2c_ItemAutoMove,
	s2c_itemexchangefinish,
	s2c_changeweather,
	s2c_pksyncnormalflag,
	s2c_pksyncenmitystate,
	s2c_pksyncexercisestate,
	s2c_pksyncpkvalue,
	s2c_npcsleepmode,
	s2c_viewequip,
	s2c_ladderresult,
	s2c_ladderlist,
	s2c_tongcreate,
	s2c_replyclientping,
	s2c_npcgoldchange,
	s2c_itemdurabilitychange,
	s2c_openresetpass,					//cho phep reset mk ruong tu server
	s2c_reputesyncreputevalue,
	s2c_fuyuansyncfuyuanvalue,
	s2c_rebornsyncrebornvalue,
	s2c_viewsellitem,
	s2c_viewupdateitem,
	s2c_playergetcount,
	s2c_shopname,
	s2c_exitgame,
	s2c_playeritemlocksync,
	s2c_opendataubox,
	s2c_opendatau1box,
	s2c_battlebox,
	s2c_syncsupershop,
	s2c_extpointsync,  // protocol xu
	//s2c_autoplay,
	//s2c_player_stop,
	//s2c_pos_edition,
	//s2c_findpathsync,
	s2c_ridesync,
	s2c_ignorestate,
	s2c_openaffairbox,
	s2c_playersync_magic_attr,
	s2c_playeritemtimesync,
	s2c_timebox,
	s2c_talkex,
	s2c_syncmasklock,
	s2c_taskvalue, // protocol task
	s2c_playermissiondata,
	s2c_inputbox,
	s2c_setobstacle,	//#Set VÀt C∂n
	s2c_returncityowntong,
	s2c_playerloginreplay, //fix by phong ki“u chuy”n gs b?m t skill
	s2c_openquestfinishdlg,
	s2c_imagenpc,
	s2c_opentrembleitem,
	s2c_opencompounditem,
	s2c_syncrankdata,	//add by phong ki“u using tËng kim
	s2c_syncrankdata2,
	s2c_show_damage,
	s2c_syncmagic,
	s2c_syncmeridian,
	s2c_syncbaucuaresult,
	s2c_syncbaucuainfo,
	s2c_playersync,
	s2c_removeallitem,
	s2c_diceitem,			// mo/cap nhat/dong o xuc xac chia do
//	s2c_dynamic_structure,	//dynamic structure

//	s2c_gmgateway2relaysvr,		//GM
	s2c_extend = 250,
	s2c_extendchat = 251,
	s2c_extendfriend = 252,
	s2c_extendtong = 253,
	s2c_end,
};

enum c2s_PROTOCOL
{
	c2s_roleserver_saveroleinfo = 10,
	c2s_roleserver_createroleinfo,
	
	c2s_gmsvr2gateway_saverole,
	/*
	 * This value must be equal to s2c_micropackbegin
	 */
	c2s_micropackbegin = g_nGlobalProtocolType,

	c2s_accountbegin = 32,
	c2s_accountlogin,
	c2s_gamelogin,
	c2s_accountlogout,
	c2s_gatewayverify,
	c2s_gatewayverifyagain,
	c2s_gatewayinfo,

	c2s_multiserverbegin = 48,
	c2s_permitplayerlogin,
	c2s_updatemapinfo,
	c2s_updategameserverinfo,
	c2s_entergame,
	c2s_leavegame,
	c2s_registeraccount,

	c2s_requestsvrip,

	c2s_roleserver_getrolelist,
	c2s_roleserver_getroleinfo,
	c2s_roleserver_deleteplayer,
	c2s_gamestatistic,
	c2s_roleserver_lock,

	c2s_gameserverbegin = 64,
	c2s_login,
	c2s_logiclogin,
	c2s_syncend,
	c2s_loadplayer,
	c2s_newplayer,
	c2s_removeplayer,
	c2s_requestworld,
	c2s_requestplayer,
	c2s_requestnpc,
	c2s_requestobj,
	c2s_npcwalk,
	c2s_npcrun,
	c2s_npcskill,
	c2s_npcjump,
	c2s_npctalk,
	c2s_dynamic_structure,
	c2s_npcdeath,
	c2s_playertalk,
	c2s_teamapplyinfo,
	c2s_teamapplycreate,
	c2s_teamapplyopenclose,
	c2s_teamapplyadd,
	c2s_teamacceptmember,
	c2s_teamapplyleave,
	c2s_teamapplykickmember,
	c2s_teamapplychangecaptain,
	c2s_teamapplydismiss,
	c2s_playerapplysetpk,
	c2s_playerapplyfactiondata,
	c2s_playersendchat,
	c2s_playeraddbaseattribute,
	c2s_playerapplyaddskillpoint,
	c2s_playereatitem,
	c2s_playerpickupitem,
	c2s_playermoveitem,
	c2s_playersellitem,
	c2s_playerbuyitem,
	c2s_playerthrowawayitem,
	c2s_playerselui,
	c2s_chatsetchannel,
	c2s_chatapplyaddfriend,
	c2s_chataddfriend,
	c2s_chatrefusefriend,
	c2s_dbplayerselect,
	c2s_chatapplyresendallfriendname,
	c2s_chatapplysendonefriendname,
	c2s_chatdeletefriend,
	c2s_chatredeletefriend,
	c2s_tradeapplystateopen,
	c2s_tradeapplystateclose,
	c2s_tradeapplystart,
	c2s_trademovemoney,
	c2s_tradedecision,
	c2s_gambleapplystateopen,
	c2s_gambleapplystateclose,
	c2s_gambleapplystart,
	c2s_gamblemovemoney,
	c2s_gambledecision,
	c2s_dialognpc,
	c2s_teaminviteadd,
	c2s_changeauraskill,
	c2s_teamreplyinvite,
	c2s_ping,
	c2s_npcsit,
	c2s_objmouseclick,
	c2s_storemoney,
	c2s_playerrevive,
	c2s_tradereplystart,
	c2s_gamblereplystart,
	c2s_pkapplychangenormalflag,
	c2s_pkapplyenmity,
	c2s_viewequip,
//	c2s_gmlogin,		//c2s_login
	c2s_ladderquery,
	c2s_repairitem,
	c2s_npcride,		//edit by phong kieu khai bao len xuong ngua
	c2s_cpunlock,		//mo ruong
	c2s_cplock,			//khoa ruong
	c2s_cpswitchequipset,
	c2s_cpchange,		//doi mk ruong
	c2s_cpreset,		//reset mk ruong
	c2s_playersetprice,	//ban hang
	c2s_playerstarttrade,
	c2s_viewitem,
	c2s_playertradebuyitem,
	c2s_updateviewitem,
	c2s_playerneedcount,
	c2s_playershopname,
	c2s_playersysshop,
	c2s_playerlixian,		//uy thac call script gs
	c2s_pkvalue,
	c2s_plockitem,
	c2s_itemyearexp,
	c2s_breakitem,
	//c2s_datau,
	//c2s_datau1,
	c2s_playercommand,
	//c2s_autoplay,
	c2s_playeractionchat,
	c2s_openshop,
	c2s_cpsetimage,
	c2s_autoplayersellitem,
	//c2s_player_pos_sync,
	//c2s_playerstop,
	c2s_uicmdscript,
	c2s_recoverybox,
	c2s_inputinfo,
	c2s_getcityowntong,
	c2s_recoveritem,
	c2s_playerthrowallitem,
	c2s_aibacktotown,//AutoAI by quay lπi ;
	c2s_setmeridian,
	c2s_baucua,
	c2s_diceitem,			// nguoi choi chon: tham du nhan / huy bo



	_c2s_begin_relay = 250,
	c2s_extend = _c2s_begin_relay,
	c2s_extendchat,
	c2s_extendfriend,
	_c2s_end_relay = c2s_extendfriend,
	c2s_extendtong,
	c2s_end,
};

enum c2c_PROTOCOL		
{
	c2c_transferroleinfo = 14,
	c2c_micropackbegin = g_nGlobalProtocolType,
	c2c_protocolbegin = 32,
	c2c_permitplayerexchangeout = 64,
	c2c_permitplayerexchangein,
	c2c_notifyexchange,

	s2s_broadcast = 96,		//Relay
	s2s_execute = 97,		//Relay

	c2c_end,
};

//c2s_extend KRelayProtocol.h
//EXTEND_HEADER

enum gm_PROTOCOL	
{
	gm_begin = 32,
	gm_c2s_execute,				//GM relay_c2c_askwaydata
	gm_c2s_disable,
	gm_c2s_enable,
	gm_c2s_tracking,
	gm_c2s_setrole,
	gm_c2s_getrole,	
	gm_c2s_findplayer,
	gm_c2s_unlock,
	gm_c2s_getrolelist,
	gm_c2s_broadcast_chat,

	gm_s2c_tracking,			//relay_c2c_data GM
	gm_s2c_getrole,
	gm_s2c_findplayer,
	gm_s2c_getrolelist,

	gm_end,
};


enum relay_PROTOCOL		//server relay
{
	relay_begin = 32,
	relay_c2c_data,
	relay_c2c_askwaydata,
	relay_s2c_loseway,
	relay_s2c_updateserver,
	relay_s2c_updatemaps,
	relay_end,
};

enum chat_PROTOCOL
{
	chat_micropackbegin = g_nGlobalProtocolType,

	chat_someonechat,
	chat_channelchat,
	chat_feedback,

	chat_everyone,

	chat_groupman,
	chat_specman,

	chat_relegate,
	chat_filterplayer,
};

enum playercomm_PROTOCOL		//server client server relay relay server
{
	playercomm_begin = 32,

	playercomm_c2s_querychannelid,
	playercomm_s2c_notifychannelid,
	playercomm_c2s_freechannelid,

	playercomm_c2s_subscribe,
	
	playercomm_s2c_relegate,
	playercomm_c2s_rollback,

	playercomm_s2c_gmquerychannelid,
	playercomm_s2c_gmfreechannelid,
	playercomm_s2c_gmsubscribe,

	playercomm_c2s_someoneact,
	playercomm_c2s_channelact,

	playercomm_someonechat,
	playercomm_channelchat,
};

// game server s3client id
enum
{
	enumTONG_SYNC_ID_CREATE_FAIL = 0,
	enumTONG_SYNC_ID_TRANSFER_ADD_APPLY,
	enumTONG_SYNC_ID_ADD,
	enumTONG_SYNC_ID_HEAD_INFO,
	enumTONG_SYNC_ID_SELF_INFO,
	enumTONG_SYNC_ID_MANAGER_INFO,
	enumTONG_SYNC_ID_MEMBER_INFO,
	enumTONG_SYNC_ID_INSTATE,
	enumTONG_SYNC_ID_KICK,
	enumTONG_SYNC_ID_CHANGE_MASTER_FAIL,
	enumTONG_SYNC_ID_CHANGE_TITLE_FAIL,
	enumTONG_SYNC_ID_CHANGE_CAMP_FAIL,
	enumTONG_SYNC_ID_CHANGE_CAMP,
	enumTONG_SYNC_ID_CHANGE_RECRUIT,
	enumTONG_SYNC_ID_JX2,	// JX2 port: du lieu trang bang hoi (bien dai theo trang)

	enumTONG_SYNC_ID_NUM,
};

// s3client game server id
enum
{
	enumTONG_COMMAND_ID_START = 0,
	enumTONG_COMMAND_ID_APPLY_CREATE,
	enumTONG_COMMAND_ID_APPLY_ADD,
	enumTONG_COMMAND_ID_ACCEPT_ADD,
	enumTONG_COMMAND_ID_APPLY_INFO,
	enumTONG_COMMAND_ID_APPLY_INSTATE,
	enumTONG_COMMAND_ID_APPLY_KICK,
	enumTONG_COMMAND_ID_APPLY_LEAVE,
	enumTONG_COMMAND_ID_APPLY_CHANGE_MASTER,
	enumTONG_COMMAND_ID_APPLY_CHANGE_TITLE,
	enumTONG_COMMAND_ID_APPLY_CHANGE_SEX_TITLE,
	enumTONG_COMMAND_ID_APPLY_MONEY_SAVE,
	enumTONG_COMMAND_ID_APPLY_MONEY_GET,
	enumTONG_COMMAND_ID_APPLY_MONEY_SND,
	enumTONG_COMMAND_ID_APPLY_CHANGE_CAMP,
	enumCOMMAND_UPDATE_EXTPOINT,
	enumTONG_COMMAND_ID_APPLY_CHANGE_RECRUIT,
	enumTONG_COMMAND_ID_APPLY_CHANGE_LEVEL,
	enumTONG_COMMAND_ID_APPLY_CHANGE_WAYEDIT,
	enumTONG_COMMAND_ID_APPLY_CHANGE_NEXTTARGET,
	enumTONG_COMMAND_ID_JX2VIEW,	// JX2 port: xin du lieu trang bang hoi
	enumTONG_COMMAND_ID_JX2OP,		// JX2 port: thao tac bang hoi tu cua so
	enumTONG_COMMAND_ID_NUM,
};

enum
{
	enumC2S_PLAYERCOMMAND_ID_LOCKSTATE,
	enumC2S_PLAYERCOMMAND_ID_GIVE,
	enumC2S_PLAYERCOMMAND_ID_LOCKITEM,
	enumC2S_PLAYERCOMMAND_ID_UNLOCKITEM,
	enumC2S_PLAYERCOMMAND_ID_SUPERSHOP,
	enumC2S_PLAYERCOMMAND_ID_OFFLINE,
	enumC2S_PLAYERCOMMAND_ID_MASKFEATURE,
	enumC2S_PLAYERCOMMAND_ID_RETURN,
};

enum
{
	enumS2C_PLAYERSYNC_ID_EXIT,
	enumS2C_PLAYERSYNC_ID_GIVE,
	enumS2C_PLAYERSYNC_ID_EQUIPEXPAND,
	enumS2C_PLAYERSYNC_ID_EXPANDBOX,
	enumS2C_PLAYERSYNC_ID_LOCKSTATE,
	enumS2C_PLAYERSYNC_ID_LOCKITEMBOX,
	enumS2C_PLAYERSYNC_ID_PROPPOINT,
	enumS2C_PLAYERSYNC_ID_MAGICPOINT,
	enumS2C_PLAYERSYNC_ID_RANKDATA,
	enumS2C_PLAYERSYNC_ID_ENCHASE,
	enumS2C_PLAYERSYNC_ID_INPUT,
	enumS2C_PLAYERSYNC_ID_MASKFEATURE,
	enumS2C_PLAYERSYNC_ID_IMMEDSKILL,	// [TONG 21/08] SetImmedSkill - di qua S2C_PLAYER_SYNC_M_A.nPoint = (slot<<24)|skillId
};

#endif
