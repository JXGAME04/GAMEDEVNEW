
#ifndef COREUSENAMEDEF_H
#define COREUSENAMEDEF_H

#define		MAX_PLAYER_IN_ACCOUNT			3								//edit by phong kieu max nhan vat trong 1 account
#define		SETTING_PATH					"\\settings"
#define		NPCNAME_KEY						"<npc>"
#define		PLAYERNAME_KEY					"<player>"
#define		SEXNAME_KEY						"<sex>"
//---------------------------- npc res  ------------------------------
#define		RES_INI_FILE_PATH				"\\settings\\npcres"
#define		RES_NPC_STYLE_PATH				"\\spr\\npcres\\style"
#define		RES_SOUND_FILE_PATH				"sound"
#define		NPC_RES_KIND_FILE_NAME			"\\settings\\npcres\\npc_res_kind_file_name.txt"		
#define		NPC_NORMAL_RES_FILE			"\\settings\\npcres\\npc_normal_res_file.txt"
#define		NPC_NORMAL_SPRINFO_FILE			"\\settings\\npcres\\npc_normal_spr_info_file.txt"	
#define		STATE_MAGIC_TABLE_NAME			"\\settings\\npcres\\state_magic_table_name.txt"
#define		PLAYER_RES_SHADOW_FILE			"\\settings\\npcres\\player_res_shadow_file.txt"
#define		PLAYER_SOUND_FILE				"\\settings\\npcres\\player_sound_file.txt"
#define		NPC_SOUND_FILE					"\\settings\\npcres\\npc_sound_file.txt"
#define		NPC_ACTION_NAME					"npc_action_name.txt"
#define		ACTION_FILE_NAME				"action_file_name.txt"
#define		PLAYER_MENU_STATE_RES_FILE		"\\settings\\npcres\\player_menu_state_res_file.txt"
#define		PLAYER_INSTANT_SPECIAL_FILE		"\\settings\\npcres\\player_instant_special_file.txt"
#define		EFFECT_INFO_NAME				"_effect"
#define		SPR_INFO_NAME					"Info"

//#define		NPC_RES_KIND_FILE_NAME			"\\settings\\npcres\\ÈËÎïÀàÐÍ.txt"
//#define		NPC_NORMAL_RES_FILE				"\\settings\\npcres\\ÆÕÍ¨npc×ÊÔ´.txt"
//#define		NPC_NORMAL_SPRINFO_FILE			"\\settings\\npcres\\ÆÕÍ¨npc×ÊÔ´ÐÅÏ¢.txt"
//#define		STATE_MAGIC_TABLE_NAME			"\\settings\\npcres\\×´Ì¬Í¼ÐÎ¶ÔÕÕ±í.txt"		//Hieu ung magic len nhan vat player nhu lai thien diep, da cau tran, l­ìng nghi ch©n khÝ, ph¸o hoa ....
//#define		PLAYER_RES_SHADOW_FILE			"\\settings\\npcres\\Ö÷½Ç¶¯×÷ÒõÓ°¶ÔÓ¦±í.txt"
//#define		PLAYER_SOUND_FILE				"\\settings\\npcres\\Ö÷½Ç¶¯×÷ÉùÒô±í.txt"
//#define		NPC_SOUND_FILE					"\\settings\\npcres\\npc¶¯×÷ÉùÒô±í.txt"
//#define		NPC_ACTION_NAME					"npc¶¯×÷±í.txt"
//#define		ACTION_FILE_NAME				"¶¯×÷±àºÅ±í.txt"
//#define		PLAYER_MENU_STATE_RES_FILE		"\\settings\\npcres\\½çÃæ×´Ì¬ÓëÍ¼ÐÎ¶ÔÕÕ±í.txt"
//#define		PLAYER_INSTANT_SPECIAL_FILE		"\\settings\\npcres\\Ë²¼äÌØÐ§.txt"

#define		NPC_RES_SHADOW_FILE				"ÆÕÍ¨npc¶¯×÷ÒõÓ°¶ÔÓ¦±í.txt"

//#define		KIND_NAME_SECT					"ÈËÎïÀàÐÍ"
//#define		KIND_NAME_SPECIAL				"ÌØÊânpc"
//#define		KIND_NAME_NORMAL				"ÆÕÍ¨npc"
//#define		KIND_FILE_SECT1					"²¿¼þËµÃ÷ÎÄ¼þÃû"
//#define		KIND_FILE_SECT2					"ÎäÆ÷ÐÐÎª¹ØÁª±í1"
//#define		KIND_FILE_SECT3					"ÎäÆ÷ÐÐÎª¹ØÁª±í2"
//#define		KIND_FILE_SECT4					"¶¯×÷ÌùÍ¼Ë³Ðò±í"
//#define		KIND_FILE_SECT5					"×ÊÔ´ÎÄ¼þÂ·¾­"

#define		KIND_NAME_SECT					"CharacterType"
#define		KIND_NAME_SPECIAL				"SpecialNpc"
#define		KIND_NAME_NORMAL				"NormalNpc"
#define		KIND_FILE_SECT1					"PartFileName"		
#define		KIND_FILE_SECT2					"WeaponActionTab1"			// OnHorse
#define		KIND_FILE_SECT3					"WeaponActionTab2"			// OnHorse
#define		KIND_FILE_SECT4					"ActionRenderOrderTab"
#define		KIND_FILE_SECT5					"ResFilePath"

//--------------------------- player  ------------------------------
#define		FACTION_FILE					"\\settings\\faction\\FactionInfo.ini"
//--------------------------- player PK------------------------------
#define		defPK_PUNISH_FILE				"\\settings\\npc\\player\\PKPunish.txt"
//--------------------------- player  ------------------------------
#define		defPLAYER_TONG_PARAM_FILE		"\\settings\\tong\\TongSet.ini"
//-----------------------------  ------------------------------
#define		CHAT_PATH						"\\chat"
#define		CHAT_TEAM_INFO_FILE_NAME		"Team.cht"
#define		CHAT_CHANNEL_INFO_FILE_NAME		"Channel.cht"
#define		CHAT_CHANNEL_NAME_ALL			"ThÕ giíi"
#define		CHAT_CHANNEL_NAME_SCREEN		"L©n cËn"
#define		CHAT_CHANNEL_NAME_SINGLE		"H¶o h÷u"
#define		CHAT_CHANNEL_NAME_TEAM			"§éi ngò"
#define		CHAT_CHANNEL_NAME_FACTION		"M«n ph¸i"
#define		CHAT_CHANNEL_NAME_TONG			"Bang ph¸i"
#define		CHAT_CHANNEL_NAME_SCREENSINGLE	"Kh¸c"
#define		CHAT_CHANNEL_NAME_SYSTEM		"HÖ thèng"
#define		CHAT_TAKE_CHANNEL_NAME_TEAM			"Kªnh ®éi ngò"
#define		CHAT_TAKE_CHANNEL_NAME_FACTION		"Kªnh m«n ph¸i"
#define		CHAT_TAKE_CHANNEL_NAME_TONG			"Kªnh bang ph¸i"
#define		CHAT_TAKE_CHANNEL_NAME_SCREENSINGLE	"Kªnh trß chuyÖn"
//---------------------------- player  ------------------------------
#define		PLAYER_LEVEL_EXP_FILE			"\\settings\\npc\\player\\level_exp.txt"
#define		PLAYER_LEVEL_ADD_FILE			"\\settings\\npc\\player\\level_add.txt"
#define		PLAYER_LEVEL_LEAD_EXP_FILE		"\\settings\\npc\\player\\level_lead_exp.txt"
#define		PLAYER_MAGIC_LEVEL_EXP_FILE		"\\settings\\npc\\player\\magic_level_exp.txt"
#define		BASE_ATTRIBUTE_FILE_NAME		"\\settings\\npc\\player\\NewPlayerBaseAttribute.ini"
#define		PLAYER_PK_RATE_FILE				"\\settings\\npc\\PKRate.ini"
#define		PLAYER_BASE_VALUE				"\\settings\\npc\\player\\BaseValue.ini"
#define		PLAYER_STAMINA_FILE_NAME		"\\settings\\npc\\player\\stamina.ini"
// 00ÄÐ½ð 01Å®½ð 02ÄÐÄ¾ 03Å®Ä¾ 04ÄÐË® 05Å®Ë® 06ÄÐ»ð 07Å®»ð 08ÄÐÍÁ 09Å®ÍÁ
#define		NEW_PLAYER_INI_FILE_NAME		"\\settings\\npc\\player\\NewPlayerIni%02d.ini"
//---------------------------- player  ------------------------------
#define		BUYSELL_FILE					"\\settings\\buysell.txt"
#define		GOODS_FILE						"\\settings\\goods.txt"
//-----------------------------  ------------------------------
#define		CHANGERES_MELEE_FILE			"\\settings\\item\\MeleeRes.txt"
#define		CHANGERES_RANGE_FILE			"\\settings\\item\\RangeRes.txt"
#define		CHANGERES_ARMOR_FILE			"\\settings\\item\\ArmorRes.txt"
#define		CHANGERES_HELM_FILE				"\\settings\\item\\HelmRes.txt"
#define		CHANGERES_HORSE_FILE			"\\settings\\item\\HorseRes.txt"
#define		CHANCERES_GOLD_FILE_O			"\\settings\\item\\GolditemRes.txt"
#define		CHANCERES_GOLD_FILE_N				"\\settings\\item\\goldequipres.txt"
#define		CHANCERES_PLATINA_FILE				"\\settings\\item\\platinaequipres.txt"
#define		CHANGERES_MASK_FILE				"\\settings\\item\\MaskRes.txt"
#define		CHANGERES_HOODS_FILE			"\\settings\\item\\HoodsRes.txt"
#define		CHANGERES_CLOAK_FILE			"\\settings\\item\\CloakRes.txt"
#define		TABFILE_GOLDITEM_FULL_O			"\\Settings\\Item\\GoldItem.txt"
#define		PLATINA_EQUIP_FILE			"\\settings\\item\\platinaequip.txt"
#define		TABFILE_GOLDITEM_FULL_N			"\\Settings\\Item\\goldequip.txt"
#define		GOLD_EQUIP_MAGIC_FILE_N		"\\settings\\item\\magicattrib_ge.txt"
#define		GOLD_EQUIP_MAGIC_FILE_O		"\\Settings\\Item\\GoldMagic.txt"
#define		ITEM_ABRADE_FILE				"\\settings\\item\\AbradeRate.ini"
#define		MAGICATTRIB_LEVEL_FILE		"\\settings\\item\\magicattriblevel.txt"
#define		MAGICATTRIB_LVINDEX_FILE	"\\settings\\item\\magicattriblevel_index.txt"
//--------------- npc skill missles  -------------
#define		SKILL_SETTING_FILE				"\\settings\\Skills.txt"					//edit by phong kieu file lien quan den skill
#define		MISSLES_SETTING_FILE			"\\settings\\Missles.txt"					//edit by phong kieu file lien quan den skill
#define		NPC_SETTING_FILE				"\\settings\\NpcS.txt"
#define		NPC_GOLD_TEMPLATE_FILE			"\\settings\\npc\\NpcGoldTemplate.txt"
#define		MERIDIAN_SETTING_FILE			"\\settings\\meridian_level.txt"
//---------------------------- object  ------------------------------
#define		OBJ_DATA_FILE_NAME		"\\settings\\obj\\ObjData.txt"					//load thoi gian ton tai object khi roi ra dat trong file nay
#define		MONEY_OBJ_FILE_NAME		"\\settings\\obj\\MoneyObj.txt"
#define		OBJ_NAME_COLOR_FILE		"\\settings\\obj\\ObjNameColor.ini"
//----------------------------  -------------------------------
#define		defLITTLE_MAP_SET_FILE	"\\Ui\\Default\\Ð¡µØÍ¼ÑÕÉ«.ini"
//-----------------------------  --------------------------------
#define		defINSTANT_SOUND_FILE	"\\settings\\SoundList.txt"
#define		defMUSIC_SET_FILE		"\\settings\\music\\MusicSet.txt"
#define		defMUSIC_FIGHT_SET_FILE	"\\settings\\music\\MusicFightSet.ini"
//------------------------------------------------------------------------
#define NPC_LEVELSCRIPT_FILENAME		"\\script\\npclevelscript\\npclevelscript.lua"
#define NPC_TEMPLATE_BINFILEPATH		"\\settings"
#define NPC_TEMPLATE_BINFILE			"NpcTemplate.Bin"
#define WEAPON_PHYSICSSKILLFILE			"\\settings\\ClientWeaponSkill.txt"			
#define WEAPON_PARTICULARTYPE			"ParticularType"
#define	WEAPON_DETAILTYPE				"DetailType"
#define	WEAPON_SKILLID					"PhysicsSkillID"
//-----------------------------------------------------------------------
#define	WORLD_WAYPOINT_TABFILE			"\\settings\\WayPoint.txt"
#define WORLD_STATION_TABFILE			"\\settings\\Station.txt"
#define WORLD_STATIONPRICE_TABFILE		"\\settings\\StationPrice.txt"
#define WORLD_WAYPOINTPRICE_TABFILE		"\\settings\\WayPointPrice.txt"
#define WORLD_DOCK_TABFILE				"\\settings\\Wharf.txt"
#define WORLD_DOCKPRICE_TABFILE			"\\settings\\WharfPrice.txt"
#define STRINGRESOURSE_TABFILE			"\\settings\\StringResource.txt"
#define PLAYER_RANK_SETTING_TABFILE		"\\settings\\RankSetting.txt"
#define PLAYER_RANK_BATTLE_SETTING_TABFILE		"\\settings\\PlayerTitle.txt"
#define HONOR_SETTING_FILE				"\\settings\\HonorSetting.txt"
#define QUESTITEM_TABFILE				"\\settings\\item\\questkey.txt"
#define RENAME_NPC_SETTING_TABFILE		"\\settings\\NpcName.txt"		//edit by phong kieu load file rename npc
#define GAME_REBORN_FILE                "\\Settings\\ReBorn\\rebornsetting.txt"
//-----------------------------------------------------------
#define TASK_MISSION_SETTING_TABFILE	"\\settings\\task\\missions.txt"
#define 	GAME_SETTING_FILE_INI		"\\settings\\GameSetting.ini"
#define		UNKNOWNITEM_SPR				"\\spr\\item\\unknownitem.spr"
#define		UNKNOWNITEM_SPR36			"\\spr\\item\\unknownitem36.spr"
#define		BROKEN_ITEM_SPR				"\\spr\\item\\cucsat.spr"
#define		BROKEN_ITEM_SPR36				"\\spr\\item\\cucsat36.spr"
#define		RESIZEITEM_SPR				"\\spr\\item\\twzhuanyun\\zhuanyunbao_big.spr"
#define		SCRIPT_PROTOCOL_FILE		"\\script\\global\\script_protocol.lua"
#define		MINIMAP_SETTING_FILE_INI	"\\Settings\\MapTraffic.ini"
#define		NORMAL_UNCLEAR_WORD			"Ch­a râ"
#define		NORMAL_FUNCTION_NAME		"main"
#define 	NPC_BOBO_FILE					"\\Ui\\npcbobo.ini"
#define		MSG_GET_EXP						"B¹n nhËn ®­îc %2.0f ®iÓm kinh nghiÖm."
#define		MSG_DEC_EXP						"B¹n bÞ mÊt %2.0f ®iÓm kinh nghiÖm."
#define		MSG_LEVEL_UP					"§¼ng cÊp cña b¹n lµ %d cÊp."
#define		MSG_LEADER_LEVEL_UP				"Tµi l·nh ®¹o b¹n t¨ng lªn %d cÊp."
#define		MSG_GET_ATTRIBUTE_POINT			"B¹n nhËn ®­îc %d ®iÓm tiÒm n¨ng."
#define		MSG_GET_SKILL_POINT				"B¹n nhËn ®­îc %d ®iÓm kü n¨ng."
#define		MSG_GET_ATTRIBUTE_SKILL_POINT	"B¹n cã %d ®iÓm tiÒm n¨ng %d ®iÓm kü n¨ng."
#define		MSG_TEAM_AUTO_REFUSE_INVITE		"Tù ®éng tõ chèi lêi mêi tæ ®éi."
#define		MSG_TEAM_NOT_AUTO_REFUSE_INVITE	"T¾t tù ®éng tõ chèi lêi mêi tæ ®éi."
#define		MSG_TEAM_SEND_INVITE			"B¹n mêi %s vµo ®éi."
#define		MSG_TEAM_GET_INVITE				"%s mêi tæ ®éi!"
#define		MSG_TEAM_REFUSE_INVITE			"%s tõ chèi vµo ®éi."
#define		MSG_TEAM_CREATE					"B¹n tiÕn hµnh lËp ®éi ngò."
#define		MSG_TEAM_CREATE_FAIL			"LËp ®éi thÊt b¹i."
#define		MSG_TEAM_CANNOT_CREATE			"B¹n kh«ng thÓ lËp ®éi."
#define		MSG_TEAM_TARGET_CANNOT_ADD_TEAM	"§èi ph­¬ng kh«ng thÓ vµo ®éi."
#define		MSG_TEAM_OPEN					"B¹n cã thÓ tiÕp nhËn thµnh viªn ®éi."
#define		MSG_TEAM_CLOSE					"B¹n kh«ng thÓ tiÕp nhËn thµnh viªn ®éi."
#define		MSG_TEAM_ADD_MEMBER				"%s trë thµnh thµnh viªn cña ®éi."
#define		MSG_TEAM_SELF_ADD				"B¹n thªm %s vµo ®éi"
#define		MSG_TEAM_DISMISS_CAPTAIN		"B¹n gi¶i t¸n ®éi ngò"
#define		MSG_TEAM_DISMISS_MEMBER			"%s gi¶i t¸n ®éi ngò"
#define		MSG_TEAM_KICK_ONE				"%s bÞ khai tõ khái ®éi"
#define		MSG_TEAM_BE_KICKEN				"B¹n bÞ khai tõ khái ®éi"
#define		MSG_TEAM_APPLY_ADD				"%s xin vµo ®éi"
#define		MSG_TEAM_APPLY_ADD_SELF_MSG		"B¹n xin gia nhËp ®éi cña %s"
#define		MSG_TEAM_LEAVE					"%s rêi ®éi."
#define		MSG_TEAM_LEAVE_SELF_MSG			"B¹n rêi khái ®éi"
#define		MSG_TEAM_CHANGE_CAPTAIN_FAIL1	"Nh­êng quyÒn thÊt b¹i"
#define		MSG_TEAM_CHANGE_CAPTAIN_FAIL2	"%s nh­êng quyÒn thÊt b¹i"
#define		MSG_TEAM_CHANGE_CAPTAIN_FAIL3	"Kh«ng thÓ nhêng quyÒn"
#define		MSG_TEAM_CHANGE_CAPTAIN			"%s lµ ®éi trëng"
#define		MSG_TEAM_CHANGE_CAPTAIN_SELF	"B¹n ®­îc %s nhêng quyÒn"

#define		MSG_CHAT_APPLY_ADD_FRIEND		"B¹n mêi %s h¶o h÷u"
#define		MSG_CHAT_FRIEND_HAD_IN			"%s ®ång ý h¶o h÷u"
#define		MSG_CHAT_GET_FRIEND_APPLY		"%s muèn thµnh h¶o h÷u"
#define		MSG_CHAT_REFUSE_FRIEND			"%s tõ chèi h¶o h÷u"
#define		MSG_CHAT_ADD_FRIEND_FAIL		"Thªm %s vµo h¶o h÷u thÊt b¹i"
#define		MSG_CHAT_ADD_FRIEND_SUCCESS		"B¹n vµ %s trë thµnh h¶o h÷u"
#define		MSG_CHAT_CREATE_TEAM_FAIL1		"Yªu cÇu h¶o h÷u thÊt b¹i"
#define		MSG_CHAT_CREATE_TEAM_FAIL2		"Ng­êi nµy ®· lµ h¶o h÷u"
#define		MSG_CHAT_CREATE_TEAM_FAIL3		"Sè l­îng h¶o h÷u ®¹t tèi ®a råi"
#define		MSG_CHAT_RENAME_TEAM_FAIL		"§æi tªn thÊt b¹i"
#define		MSG_CHAT_DELETE_TEAM_FAIL1		"Xãa bá h¶o h÷u thÊt b¹i"
#define		MSG_CHAT_DELETE_TEAM_FAIL2		"Kh«ng cho phÐp xãa bá h¶o h÷u"
#define		MSG_CHAT_FRIEND_ONLINE			"%s lªn m¹ng"
#define		MSG_CHAT_FRIEND_OFFLINE			"%s rêi m¹ng"
#define		MSG_CHAT_DELETE_FRIEND			"B¹n vµ %s c¾t ®øt quan hÖ h¶o h÷u"
#define		MSG_CHAT_DELETED_FRIEND			"%s c¾t ®øt quan hÖ h¶o h÷u víi b¹n"
#define		MSG_CHAT_MSG_FROM_FRIEND		"%s nãi chuyÖn víi b¹n"
#define		MSG_CHAT_FRIEND_NOT_NEAR		"%s kh«ng ë gÇn"
#define		MSG_CHAT_FRIEND_NOT_ONLINE		"%s kh«ng cã trªn m¹ng"
#define		MSG_CHAT_TAR_REFUSE_SINGLE_TALK	"%s kh«ng cã trªn tÇn sè h¶o h÷u"

#define		MSG_SHOP_NO_ROOM				"Kh«ng gian chøa ®å kh«ng ®ñ!"
#define		MSG_SHOP_NO_MONEY				"Kh«ng ®ñ ng©n l­îng!"
#define		MSG_NO_REPAIR_MONEY			   "Trang bÞ háng ®Õn gÆp thî rÌn thÇn bÝ dïng tiÒn xu ®Ó kh«i phôc!"

#define		MSG_NPC_NO_MANA					"Kh«ng ®ñ néi lùc!"
#define		MSG_NPC_NO_STAMINA				"Kh«ng ®ñ thÓ lùc!"
#define		MSG_NPC_NO_LIFE					"Kh«ng ®ñ sinh lùc!"
#define		MSG_NPC_DEATH					"%s ®· vÒ thµnh d­ìng søc!"

#define		MSG_OBJ_CANNOT_PICKUP			"B¹n kh«ng thÓ nhÆt vËt phÈm cña ng­êi kh¸c"
#define		MSG_MONEY_CANNOT_PICKUP			"B¹n kh«ng thÓ nhÆt tiÒn cña ng­êi kh¸c"
#define		MSG_OBJ_TOO_FAR					"VËt phÈm ë qu¸ xa, kh«ng thÓ nhÆt"
#define		MSG_DEC_MONEY					"B¹n bÞ mÊt %d l­îng"
#define		MSG_DEC_MONEY2					"B¹n bÞ mÊt %d v¹n l­îng"
#define		MSG_DEC_MONEY3					"B¹n bÞ mÊt %d v¹n %d l­îng"
#define		MSG_EARN_MONEY					"B¹n nhËn ®­îc %d l­îng."
#define		MSG_EARN_MONEY2					"B¹n nhËn ®­îc %d v¹n l­îng."
#define		MSG_EARN_MONEY3					"B¹n nhËn ®­îc %d v¹n %d l­îng."
#define		MSG_DEATH_LOSE_ITEM				"B¹n ®¸nh r¬i %s."
#define		MSG_ADD_ITEM					"B¹n nhÆt ®­îc %s"
#define		MSG_ITEM_SAME_DETAIL_IN_IMMEDIATE	"§· cã vËt phÈm cïng lo¹i"
#define		MSG_SKILL_SAME_DETAIL_IN_IMMEDIATE	"§· cã kü n¨ng cïng lo¹i"
#define		MSG_ITEM_NOT_STACK_IN_IMMEDIATE	"VËt phÈm kh«ng thÓ bá vµo « phÝm t¾t"
#define		MSG_ITEM_AUTO_DELETE_EXPTIME	"Tù ®éng xo¸ vËt phÈm hÕt h¹n"
#define		MSG_CAN_NOT_VIEW_ITEM			"B¹n kh«ng thÓ xem tin tøc ng­êi kh¸c"
#define		MSG_ITEM_DAMAGED				"%s ®· bÞ h­"
#define		MSG_ITEM_NEARLY_DAMAGED			"%s s¾p h­, h·y mau ®i söa"
#define		MSG_ITEM_REPAIRED				"Tiªu tèn %d l­îng b¹c ®Ó söa chöa %s"
#define		MSG_TRADE_STATE_OPEN			"B¹n hiÖn giê cã thÓ giao dÞch."
#define		MSG_TRADE_STATE_CLOSE			"B¹n hiÖn giê kh«ng thÓ giao dÞch."
#define		MSG_TRADE_SELF_LOCK				"B¹n bÞ kho¸ giao dÞch"
#define		MSG_TRADE_SELF_UNLOCK			"B¹n ®­îc më khãa giao dÞch"
#define		MSG_TRADE_DEST_LOCK				"%s bÞ kho¸ giao dÞch"
#define		MSG_TRADE_DEST_UNLOCK			"%s ®­îc më khãa giao dÞch"
#define		MSG_TRADE_SUCCESS				"B¹n vµ %s giao dÞch thµnh c«ng"
#define		MSG_TRADE_FAIL					"B¹n vµ %s giao dÞch thÊt b¹i"
#define		MSG_TRADE_SELF_ROOM_FULL		"Kh«ng gian chøa ®å cña b¹n ®· ®Çy"
#define		MSG_TRADE_DEST_ROOM_FULL		"Kh«ng gian chøa ®å cña %s ®· ®Çy"
#define		MSG_TRADE_SEND_APPLY			"B¹n mêi %s giao dÞch."
#define		MSG_TRADE_GET_APPLY				"%s xin giao dÞch víi b¹n"
#define		MSG_TRADE_REFUSE_APPLY			"%s tõ chèi giao dÞch"
#define		MSG_TRADE_TASK_ITEM				"VËt phÈm kh«ng thÓ giao dÞch."
#define		MSG_GAMBLE_STATE_OPEN			"B¹n hiÖn giê ca thÓ O¼n tï t×."
#define		MSG_GAMBLE_STATE_CLOSE			"B¹n hiÖn giê kh«ng thÓ O¼n tï t×."
#define		MSG_GAMBLE_SELF_LOCK				"B¹n b~ kho¸ O¼n tï t×"
#define		MSG_GAMBLE_SELF_UNLOCK			"B¹n ®­îc më khaa O¼n tï t×"
#define		MSG_GAMBLE_DEST_LOCK				"%s b~ kho¸ O¼n tï t×"
#define		MSG_GAMBLE_DEST_UNLOCK			"%s ®­îc më khaa O¼n tï t×"
#define		MSG_GAMBLE_SUCCESS				"B¹n vµ %s OTT thµnh c«ng"
#define		MSG_GAMBLE_LOSE				"O¼n tï t× b¹n ®· thua"
#define		MSG_GAMBLE_WIN				"O¼n tï t× b¹n ®· th¾ng"
#define		MSG_GAMBLE_DRAW				"O¼n tï t× b¹n ®· hßa."
#define		MSG_GAMBLE_FAIL					"B¹n vµ %s nghØ ch¬i O¼n tï t×"
#define		MSG_GAMBLE_SELF_ROOM_FULL		"Kh«ng gian chøa ®å cña b¹n ®· ®Çy"
#define		MSG_GAMBLE_DEST_ROOM_FULL		"Kh«ng gian chøa ®å cña %s ®· ®Çy"
#define		MSG_GAMBLE_SEND_APPLY			"B¹n mêi %s O¼n tï t×."
#define		MSG_GAMBLE_GET_APPLY				"%s xin O¼n tï t× víi b¹n"
#define		MSG_GAMBLE_REFUSE_APPLY			"%s to chèi O¼n tï t×"
#define		MSG_GAMBLE_TASK_ITEM				"VËt phÈm kh«ng thÓ O¼n tï t×."
#define		MSG_GAMBLE_SEND_APPLY			"B¹n mêi %s O¼n tï t×."
#define		MSG_PK_NORMAL_FLAG_OPEN			"B¹n ®ang ë tr¹ng th¸i chiÕn ®Êu!"
#define		MSG_PK_NORMAL_FLAG_CLOSE		"B¹n ®ang ë tr¹ng th¸i luyÖn c«ng!"
#define		MSG_PK_NORMAL_FLAG_DS			"B¹n ®ang trong tr¹ng th¸i giÕt ng­êi!"
#define		MSG_PK_VALUE					"TrÞ PK hiÖn t¹i lµ %d"
#define		MSG_PK_ERROR_1					"B¹n hiÖn ë n¬i phi chiÕn ®Êu"
#define		MSG_PK_ERROR_2					"Ch÷ tr¾ng kh«ng thÓ cõu s¸t!"
#define		MSG_PK_ERROR_3					"B¹n ®ang t×m hiÓu víi nh÷ng ng­êi kh¸c!"
#define		MSG_PK_ERROR_4					"B¹n ®ang cõu s¸t víi ng­êi kh¸c!"
#define		MSG_PK_ERROR_5					"B¹n kh«ng thÓ tÊn c«ng ng­êi míi!"
#define		MSG_PK_ERROR_6					"§èi ph¬ng ®ang ë n¬i phi chiÕn ®Êu"
#define		MSG_PK_ERROR_7					"Kh«ng thÕ chiÕn ®Êu"
#define		MSG_PK_ENMITY_SUCCESS_1			"B¹n vµ %s thiÕt lËp quan hÖ cõu s¸t"
#define		MSG_PK_ENMITY_SUCCESS_2			"10 sau b¾t ®Çu cõu s¸t"
#define		MSG_PK_ENMITY_CLOSE				"Cõu s¸t kÕt thóc"
#define		MSG_PK_ENMITY_OPEN				"Cõu s¸t b¾t ®Çu"
//edit by phong kieu xuat thong bao
#define		MSG_TONG_CREATE_ERROR01			"Tªn bang qu¸ dµi"
#define		MSG_TONG_CREATE_ERROR02			"Tªn bang sai"
#define		MSG_TONG_CREATE_ERROR03			"Thµnh viªn bang héi kh«ng thÓ t¹o mét bang héi míi"
#define		MSG_TONG_CREATE_ERROR04			"B¹n ch­a xuÊt s­."
#define		MSG_TONG_CREATE_ERROR05			"B¹n cha ®ñ cÊp ®é thµnh lËp bang"
#define		MSG_TONG_CREATE_ERROR06			"Tµi l·nh ®¹o cña b¹n kh«ng ®ñ"
#define		MSG_TONG_CREATE_ERROR07			"LËp bang cÇn %d l­îng"
#define		MSG_TONG_CREATE_ERROR012		"LËp bang cÇn %d v¹n l­îng"
#define		MSG_TONG_CREATE_ERROR013		"LËp bang cÇn %d v¹n %d l­îng"
#define		MSG_TONG_CREATE_ERROR08			"T©n thñ kh«ng thÓ lËp bang"
#define		MSG_TONG_CREATE_ERROR09			"Lçi bang héi 1101"
#define		MSG_TONG_CREATE_ERROR10			"Sè tiÒn ®Ó lËp bang kh«ng ®ñ!"
#define		MSG_TONG_CREATE_ERROR11			"Tªn bang nµy ®· ®­îc sö dông!"
#define		MSG_TONG_CREATE_ERROR12			"Thµnh lËp bang héi cÇn Nh¹c V­¬ng KiÕm!"
#define		MSG_TONG_APPLY_CREATE			"§ång ý t¹o bang héi!"
#define		MSG_TONG_CREATE_SUCCESS			"T¹o bang héi thµnh c«ng!"
#define		MSG_TONG_APPLY_ADD				"B¹n xin gia nhËp bang héi!"
#define		MSG_TONG_APPLY_ADD_ERROR		"Ng­êi ch¬i %s kh«ng ®ñ ®iÒu kiÖn gia nhËp bang héi"
#define		MSG_TONG_APPLY_ADD_ERROR1		"Thµnh viªn bang héi kh«ng thÓ gia nhËp bang héi kh¸c!"
#define		MSG_TONG_APPLY_ADD_ERROR2		"S¸t thñ míi cã thÓ gia nhËp bang héi!"
#define		MSG_TONG_APPLY_ADD_ERROR3		"Nhãm kh«ng thÓ gia nhËp bang héi!"
#define		MSG_TONG_APPLY_ADD_ERROR4		"B¹n võa chñ ®éng rêi khái bang héi, ®ang trong thêi gian thö th¸ch kh«ng thÓ göi yªu cÇu"
#define		MSG_TONG_APPLY_ADD_ERROR5		"Bang héi mµ b¹n võa xin gia nhËp ®· ®ãng chøc n¨ng chiªu mé ®Ö tö"
#define		MSG_TONG_APPLY_ADD_ERROR6		"Bang héi ®· ®ãng chøc n¨ng chiªu mé ®Ö tö"
#define		MSG_TONG_APPLY_ADD_ERROR7		"Ng­êi nµy võa rêi khái bang héi ch­a thÓ gia nhËp b©y giê"
#define		MSG_TONG_REFUSE_ADD				"%s tõ chèi gia nhËp bang héi!"
#define		MSG_TONG_ADD_SUCCESS			"Gia nhËp bang héi thµnh c«ng"
#define		MSG_TONG_CANNOT_LEAVE1			"Thµnh viªn chÝnh thøc kh«ng thÓ rêi khái bang"
#define		MSG_TONG_CANNOT_LEAVE2			"Tr­ëng l·o kh«ng thÓ rêi khái bang!"
#define		MSG_TONG_BE_KICKED				"B¹n bÞ ®uæi ra khái bang!"
#define		MSG_TONG_LEAVE_SUCCESS			"Rêi khái bang héi thµnh c«ng"
#define		MSG_TONG_LEAVE_FAIL				"Rêi khái bang héi thÊt b¹i!"
#define		MSG_TONG_CHANGE_AS_MASTER		"B¹n ®­îc bæ nhiÖm lµm bang chñ!"
#define		MSG_TONG_CHANGE_AS_MEMBER		"B©y giê b¹n trë thµnh thµnh viªn b×nh th­êng!"
#define		MSG_TONG_CHANGE_CAMP			"TiÕn hµnh thay ®æi mµu bang héi."
#define		MSG_TONG_CHANGE_CAMP_ERROR1		"ChØ cã bang chñ míi sö dông ®­îc chøc n¨ng nµy."
#define		MSG_TONG_CHANGE_CAMP_ERROR2		"Vui lßng chän phe."
#define		MSG_TONG_CHANGE_CAMP_ERROR3		"CÇn %d l­îng ®Ó thay ®æi mµu bang héi."
#define		MSG_EXPAND_STORE_BOX			"B¹n ch­a mua chøc n¨ng nµy. H·y ®Õn Ba L¨ng HuyÖn gÆp ThÈm Cöu (188,198) ®Ó mua chøc n¨ng nµy."

#define		MSG_NPC_CANNOT_RIDE				"B¹n qu¸ mÖt mái ,Kh«ng ThÓ TiÕp Tôc Lªn Xuèng Ngùa!"
#define		MSG_COMP_FAILED					"ChÕ t¹o trang bÞ thÊt b¹i, vui lßng thö l¹i !"
#define		MSG_COMP_SUCCESS				"ChÕ t¹o trang bÞ thµnh c«ng !"
#define		MSG_COMP_ERITEM					"Ng­¬i kh«ng cã mang ®ñ nguyªn liÖu !"
#define		MSG_COMP_NOMONEY				"Ng­¬i kh«ng cã ®ñ 5000 l­îng"
#define		MSG_COMP_CAPTOIDA				"VËt phÈm ®¹t cÊp tèi ®a, kh«ng thÓ n¨ng cÊp ®­îc n÷a"
#define		MSG_NOT_THROW					"VËt phÈm nµy, kh«ng thÓ vøt bá ®­îc. "
#define		MESSAGE_SYSTEM_ANNOUCE_HEAD		"HÖ Thèng"
#define		MESSAGE_SYSTEM_TONG_HEAD		"Tin bang"
#define		MESSAGE_SYSTEM_FACTION_HEAD		"Tin ph¸i"
#define		MSG_FIGHT_MODE_ERROR1			"Tr¹ng th¸i chiÕn ®Êu kh«ng thÓ thùc hiÖn"
#define		MSG_FIGHT_MODE_ERROR2			"B¹n trong tr¹ng th¸i chiÕn ®Êu kh«ng thÓ thùc hiÖn"
#define		MSG_FIGHT_MODE_ERROR3			"%s trong tr¹ng th¸i chiÕn ®Êu kh«ng thÓ thùc hiÖn"
#define		MSG_SHOP_NO_EXTPOINT			"B¹n kh«ng cã ®ñ tiÒn xu"
#define		MSG_SHOP_NO_FUYUAN				"B¹n kh«ng cã ®ñ phóc duyªn"
#define		MSG_SHOP_NO_REPUTE				"B¹n kh«ng cã ®ñ danh väng"
#define		MSG_SHOP_NO_ACCUM				"B¹n kh«ng cã ®ñ tÝch lòy"
#define		MSG_SHOP_NO_HONOR				"B¹n kh«ng cã ®ñ vinh dù"
#define		MSG_SHOP_NO_RESPECT				"B¹n kh«ng cã ®ñ uy danh"
#define		MSG_NPC_NOT_USE_TOWNPORTAL		"Kh«ng thÓ sö dông thæ ®Þa phï ë ®©y"
#define		MSG_NPC_NOT_PAY_FOR_SPEECH		"B¹n bÞ cÊm ph¸t ng«n ®Õn %H:%M - %d/%m/%Y"
#define		MSG_NPC_NOT_USE_SKILL_SILENT	"B¹n kh«ng thÓ thi triÓn vâ c«ng lóc nµy"
#define		MSG_NPC_NOT_USE_SKILL_DISTANCE	"B¹n ®øng c¸ch tiªu ®iÓm qu¸ xa, tr­íc m¾t sö dông vâ c«ng nµy sÏ kh«ng ®ñ hiÖu nghiÖm."
#define		MSG_NPC_NOT_USE_SKILL_WEAPON	"Vò khÝ mµ b¹n ®ang sö dông kh«ng phï hîp víi lo¹i vâ c«ng mµ b¹n hiÖn ®ang sö dông."
#define		MSG_NPC_NOT_USE_SKILL_HORSE1	"Vâ c«ng mµ b¹n sö dông hiÖn giê kh«ng thÓ c­ìi ngùa."
#define		MSG_NPC_NOT_USE_SKILL_HORSE2	"Vâ c«ng mµ b¹n sö dông hiÖn giê cÇn ph¶i c­ìi ngùa."
#define		MSG_NPC_RENASCENCE_SOMEONE		"B¹n bÞ %s h¹ träng th­¬ng."
#define		MSG_NPC_RENASCENCE				"B¹n ®· bÞ träng th­¬ng."
#define		MSG_FUNCTION_FOBIDDENED			"Xin lçi, c«ng n¨ng nµy t¹m thêi ®· bÞ ®ãng!"
#define		MSG_FUNCTION_LOCKED				"Thao t¸c ®· bÞ kho¸! vui lßng më khãa b¶o vÖ tr­íc khi thùc hiÖn."
#define		MSG_LOCK_NOT_THROW				"Nh»m b¶o vÖ tµi s¶n cho b¹n VËt phÈm nµy kh«ng ®­îc tïy ý vøt bá."
#define		MSG_LOCK_NOT_TRADE				"Nh»m b¶o vÖ tµi s¶n cho b¹n VËt phÈm nµy kh«ng thÓ giao dÞch."
#define		MSG_LOCK_NOT_SETPRICE			"Nh»m b¶o vÖ tµi s¶n cho b¹n VËt phÈm nµy kh«ng thÓ ®Þnh gi¸."
#define		MSG_BROKEN_CANTREPAIR			"Trang bÞ ®· h­ háng hoµn toµn kh«ng thÓ söa ch÷a b»ng ph­¬ng ph¸p b×nh th­êng."
#define		MSG_ITEM_SAME_IN_IMMEDIATE		"HiÖn ®· cã vËt phÈm ®ång lo¹i råi."
#define		MSG_SKILL_SAME_IN_IMMEDIATE		"HiÖn ®· cã kü n¨ng ®ång lo¹i råi."
#define		MSG_ITEM_PUT_IN_IMMEDIATE		"Thanh phÝm t¾t kh«ng thÓ ®Æt vµo vËt phÈm nµy!"
#define		MSG_BREAK_ITEM_NOT		"B¹n kh«ng ®ñ vËt phÈm!"
#define		MSG_CANT_SIT_RIDE				"Lóc c­ìi ngùa kh«ng thÓ thiÒn täa"
#define		MSG_CANT_SWITCH_HORSE_SIT		"B¹n ®ang thiÒn täa kh«ng thÓ lªn ngùa"
#define		MSG_CANT_SWITCH_HORSE			"B¹n qu¸ mÖt mái kh«ng thÓ liªn tôc lªn hoÆc xuèng ngùa"
#define		MSG_CANT_TRADE_BUZY				"Trong lóc cìi ngùa hoÆc ®ang giao dÞch víi tæ ®éi kh«ng thÓ bµy b¸n!"
#define		MSG_CHATROOM_CREATE				"B¹n t¹o phßng míi %s"
#define		MSG_CHATROOM_CREATE_ERROR_SAMENAME	"Tªn nµy ®· tån t¹i"
#define		MSG_CHATROOM_CREATE_ERROR_SAMEHOST	"B¹n ®· t¹o mét phßng råi"
#define		MSG_CHATROOM_CREATE_ERROR_NAME	"Tªn kh«ng hîp lÖ"
#define		MSG_CHATROOM_CREATE_ERROR_PASS	"MËt m· phßng kh«ng hîp lÖ"
#define		MSG_CHATROOM_CREATE_ERROR_ROOMFULL	"Sè phßng t¸n gÉu cña m¸y chñ nµy ®· ®ñ, kh«ng thÓ t¹o thªm"
#define		MSG_CHATROOM_SELF_JOIN			"B¹n vµo phßng %s"
#define		MSG_CHATROOM_JOIN				"%s ®· vµo phßng"
#define		MSG_CHATROOM_JOIN_FAIL1			"B¹n nhËp sai mËt m· phßng %s"
#define		MSG_CHATROOM_JOIN_FAIL2			"B¹n bÞ liÖt vµo sæ ®en kh«ng thÓ ®¨ng nhËp vµo phßng %s"
#define		MSG_CHATROOM_JOIN_FAIL3			"Phßng %s ®· ®Çy"
#define		MSG_CHATROOM_SELF_LEAVE			"B¹n rêi khái phßng %s"
#define		MSG_CHATROOM_LEAVE				"%s rêi khái phßng"
#define		MSG_CHATROOM_SELF_DELETE		"B¹n ®· hñy phßng %s"
#define		MSG_CHATROOM_SELF_FORCELEAVE	"B¹n bÞ ®Èy khái phßng %s"
#define		MSG_CHATROOM_SELF_BEKICK		"B¹n bÞ khai trõ khái phßng %s"
#define		MSG_CHATROOM_BEKICK				"%s bÞ khai trõ khái phßng"
#define		MSG_CHATROOM_ADDBLACKLIST		"Thªm %s vµo sæ ®en"
#define		MSG_CHATROOM_ADDBLACKLIST_FAIL	"Thªm %s vµo sæ ®en thÊt b¹i"
#define		MSG_CHATROOM_REMOVEBLACKLIST	"L­îc bá %s khái sæ ®en"
#define		MSG_CHATROOM_SELD_CHANGEPW		"B¹n ®· ®æi mËt m· phßng %s"
#define		MSG_CHATROOM_OPENGAME			"L­ît ch¬i míi ®­îc më, lÇn nµy ®Æt tèi ®a %d ®iÓm"
#define		MSG_CHATROOM_REVERSE			"L­ît ch¬i cña phßng %s ®· kÕt thóc"

enum enumGAMBLE {
	enumGAMBLE_Rock = 0,
	enumGAMBLE_Paper,
	//enumGAMBLE_Sissors,
	enumGAMBLE_Scissors,
};
enum enumMSG_ID
{
	enumMSG_ID_NONE = 0,
	enumMSG_ID_TEAM_KICK_One,
	enumMSG_ID_TEAM_DISMISS,
	enumMSG_ID_TEAM_LEAVE,
	enumMSG_ID_TEAM_REFUSE_INVITE,
	enumMSG_ID_TEAM_SELF_ADD,
	enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL,
	enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL2,
	enumMSG_ID_OBJ_CANNOT_PICKUP,
	enumMSG_ID_OBJ_TOO_FAR,
	enumMSG_ID_DEC_MONEY,
	enumMSG_ID_TRADE_SELF_ROOM_FULL,
	enumMSG_ID_TRADE_DEST_ROOM_FULL,
	enumMSG_ID_TRADE_REFUSE_APPLY,
	enumMSG_ID_TRADE_TASK_ITEM,
	enumMSG_ID_GAMBLE_SELF_ROOM_FULL,
	enumMSG_ID_GAMBLE_DEST_ROOM_FULL,
	enumMSG_ID_GAMBLE_REFUSE_APPLY,
	enumMSG_ID_GAMBLE_TASK_ITEM,
	enumMSG_ID_GET_ITEM,
	enumMSG_ID_ITEM_DAMAGED,
	enumMSG_ID_MONEY_CANNOT_PICKUP,
	enumMSG_ID_CANNOT_ADD_TEAM,
	enumMSG_ID_TARGET_CANNOT_ADD_TEAM,
	enumMSG_ID_PK_ERROR_1,
	enumMSG_ID_PK_ERROR_2,
	enumMSG_ID_PK_ERROR_3,
	enumMSG_ID_PK_ERROR_4,
	enumMSG_ID_PK_ERROR_5,
	enumMSG_ID_PK_ERROR_6,
	enumMSG_ID_PK_ERROR_7,
	enumMSG_ID_DEATH_LOSE_ITEM,
	enumMSG_ID_TONG_REFUSE_ADD,
	enumMSG_ID_TONG_BE_KICK,
	enumMSG_ID_TONG_LEAVE_SUCCESS,
	enumMSG_ID_TONG_LEAVE_FAIL,
	enumMSG_ID_TONG_CHANGE_AS_MASTER,
	enumMSG_ID_TONG_CHANGE_AS_MEMBER,
	enumMSG_TONG_RECRUIT_CLOSE,
	enumMSG_ID_TONG_APPLY_ADD_ERROR,
	enumMSG_ID_TONG_APPLY_ADD_ERROR1,
	enumMSG_ID_TONG_APPLY_ADD_ERROR2,
	enumMSG_ID_TONG_APPLY_ADD_ERROR3,
	enumMSG_ID_TONG_APPLY_ADD_ERROR4,
	enumMSG_ID_TONG_APPLY_ADD_ERROR5,
	enumMSG_ID_TONG_APPLY_ADD_ERROR6,
	enumMSG_ID_TONG_APPLY_ADD_ERROR7,
	enumMSG_ID_FIGHT_MODE_ERROR1,
	enumMSG_ID_FIGHT_MODE_ERROR2,
	enumMSG_ID_FIGHT_MODE_ERROR3,
	enumMSG_ID_SHOP_NO_ROOM,
	enumMSG_ID_SHOP_NO_MONEY,
	enumMSG_ID_SHOP_NO_EXTPOINT,
	enumMSG_ID_SHOP_NO_FUYUAN,
	enumMSG_ID_SHOP_NO_REPUTE,
	enumMSG_ID_SHOP_NO_ACCUM,
	enumMSG_ID_SHOP_NO_HONOR,
	enumMSG_ID_SHOP_NO_RESPECT,
	enumMSG_ID_FUNCTION_CHAT_FORBIDDENED,
	enumMSG_ID_FUNCTION_FOBIDDENED,
	enumMSG_ID_FUNCTION_LOCKED,
	enumMSG_ID_LOCK_NOT_TRADE,
	enumMSG_ID_LOCK_NOT_SETPRICE,
	enumMSG_ID_LOCK_NOT_THROW,
	enumMSG_ID_ITEM_TOWNPORTAL,
	enumMSG_ID_ITEM_CANTREPAIR,
	enumMSG_ID_ITEM_SAME_IMMEDIATE,
	enumMSG_ID_SKILL_SAME_IMMEDIATE,
	enumMSG_ID_ITEM_CANTPUT_IMMEDIATE,
	enumMSG_ID_HORSE_CANT_SWITCH1,
	enumMSG_ID_HORSE_CANT_SWITCH2,
	enumMSG_ID_CHATROOM_JOIN,
	enumMSG_ID_CHATROOM_LEAVE,
	enumMSG_ID_CHATROOM_BEKICK,
	enumMSG_ID_CHATROOM_OPENGAME,
	enumMSG_ID_CHATROOM_REVERSE,
	enumMSG_ID_NPC_RENASCENCE_SOMEONE,
	enumMSG_ID_NPC_RENASCENCE,
	enumMSG_ID_NUM,
};

//----------------------------  ------------------------------
#define		MOUSE_CURSOR_NORMAL				0
#define		MOUSE_CURSOR_FIGHT				1
#define		MOUSE_CURSOR_DIALOG				2
#define		MOUSE_CURSOR_PICK				3
#define		MOUSE_CURSOR_USE				8
#endif
