
/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong KiÒu
//	CreateTime:	2020-9-12
*****************************************************************************************/

#pragma once

enum GAMEDATA_INDEX
{
	GDI_GAME_OBJ_DESC = 1,		//ÓÎÏ·¶ÔÏóÃèÊöËµÃ÷ÎÄ±¾´®
	//uParam = (KUiObjAtContRegion*) ÃèÊöÓÎÏ·¶ÔÏóµÄ½á¹¹Êý¾ÝµÄÖ¸Õë£¬ÆäÖÐKUiObjAtContRegion::RegionµÄÊý¾Ý¹Ì¶¨Îª0£¬ÎÞÒâÒå¡£
	//nParam = (KGameObjDesc*) ËùÖ¸µÄ½á¹¹¿Õ¼äÓÃÓÚ±£´æÈ¡µÃµÄÃèÊöËµÃ÷¡£
	
	GDI_GAME_OBJ_DESC_INCLUDE_TRADEINFO,	//ÓÎÏ·¶ÔÏóÃèÊöËµÃ÷ÎÄ±¾´®(°üº¬½»Ò×Ïà¹ØÐÅÏ¢)
	//²ÎÊýº¬ÒåÍ¬GDI_GAME_OBJ_DESC

	GDI_GAME_OBJ_DESC_INCLUDE_REPAIRINFO,	//ÓÎÏ·¶ÔÏóÃèÊöËµÃ÷ÎÄ±¾´®(°üº¬ÐÞÀíÏà¹ØÐÅÏ¢)
	//²ÎÊýº¬ÒåÍ¬GDI_GAME_OBJ_DESC
	
	GDI_GAME_OBJ_LIGHT_PROP,	//¶ÔÏóµÄ¹âÔ´ÊôÐÔÊý¾Ý
	//uParam = (KUiGameObject*) ÃèÊöÓÎÏ·¶ÔÏóµÄ½á¹¹Êý¾ÝµÄÖ¸Õë
	//nParam = to be def

	GDI_PLAYER_BASE_INFO,
	//uParam = (KUiPlayerBaseInfo*)pInfo

	GDI_PLAYER_RT_INFO,			//Ö÷½ÇµÄÒ»Ð©Ò×±äµÄÊý¾Ý
	//uParam = (KUiPlayerRuntimeInfo*)pInfo

	GDI_PLAYER_RT_ATTRIBUTE,	//Ö÷½ÇµÄÒ»Ð©Ò×±äµÄÊôÐÔÊý¾Ý
	//uParam = (KUiPlayerAttribute*)pInfo

	GDI_PLAYER_IMMED_ITEMSKILL,
	//uParam = (KUiPlayerImmedItemSkill*)pInfo

	GDI_PLAYER_HOLD_MONEY,
	//nRet = Ö÷½ÇËæÉíÐ¯´øµÄÇ®

	GDI_PLAYER_IS_MALE,			//Ö÷½ÇÊÇ·ñÄÐÐÔ
	//nRet = (int)(bool)bMale	ÊÇ·ñÄÐÐÔ

	// from now on, flying add this item, get the information whether
	// a player can ride a horse.
	GDI_PLAYER_CAN_RIDE,		//Ö÷½ÇÊÇ·ñ¿ÉÒÔÆïÂíÂí
	//nRet = (int)bCanRide		ÊÇ·ñ¿ÉÒÔ
	//0 - ²»¿ÉÒÔÆïÂíÅ¶
	//1 - ¿ÉÒÔÅ¶

	GDI_ITEM_TAKEN_WITH,		
	//uParam = (KUiObjAtRegion*) pInfo -> KUiObjAtRegion½á¹¹Êý×éµÄÖ¸Õë£¬KUiObjAtRegion
	//				½á¹¹ÓÃÓÚ´æ´¢ÎïÆ·µÄÊý¾Ý¼°Æä·ÅÖÃÇøÓòÎ»ÖÃÐÅÏ¢¡£
	//nParam = pInfoÊý×éÖÐ°üº¬KUiObjAtRegion½á¹¹µÄÊýÄ¿
	//Return = Èç¹û·µ»ØÖµÐ¡ÓÚµÈÓÚ´«Èë²ÎÊýnParam£¬ÆäÖµ±íÊ¾pInfoÊý×éÖÐµÄÇ°¶àÉÙ¸öKUiObjAtRegion
	//			½á¹¹±»Ìî³äÁËÓÐÐ§µÄÊý¾Ý£»·ñÔò±íÊ¾ÐèÒª´«Èë°üº¬¶àÉÙ¸öKUiObjAtRegion½á¹¹µÄÊý×é
	//			²Å¹»´æ´¢È«²¿µÄËæÉíÐ¯´øµÄÎïÆ·ÐÅÏ¢¡£

	GDI_ITEM_IN_STORE_BOX,		//´¢ÎïÏäÀïµÄÎïÆ·
	//²ÎÊý¼°·µ»ØÖµº¬ÒåÍ¬GDI_ITEM_TAKEN_WITHµÄ

	GDI_EQUIPMENT,				//Ö÷½Ç×°±¸ÎïÆ·
	//uParam = (KUiObjAtRegion*)pInfo -> °üº¬11¸öÔªËØµÄKUiObjAtRegion½á¹¹Êý×éÖ¸Õë£¬
	//				KUiObjAtRegion½á¹¹ÓÃÓÚ´æ´¢×°±¸µÄÊý¾ÝºÍ·ÅÖÃÎ»ÖÃÐÅÏ¢¡£
	//			KUiObjAtRegion::Region::h = 0
	//			KUiObjAtRegion::Region::v ±íÊ¾ÊôÓÚÄÄ¸öÎ»ÖÃµÄ×°±¸,ÆäÖµÎªÃ·¾ÙÀàÐÍ
	//			UI_EQUIPMENT_POSITIONµÄÈ¡ÖµÖ®Ò»¡£Çë²Î¿´UI_EQUIPMENT_POSITIONµÄ×¢ÊÍ¡£
	//Return =  ÆäÖµ±íÊ¾pInfoÊý×éÖÐµÄÇ°¶àÉÙ¸öKUiObjAtRegion½á¹¹±»Ìî³äÁËÓÐÐ§µÄÊý¾Ý¡£

	GDI_TRADE_NPC_ITEM,			//npcÁÐ³öÀ´½»Ò×µÄÎïÆ·
	//uParam = (KUiObjAtContRegion*) pInfo -> KUiObjAtContRegion½á¹¹Êý×éµÄÖ¸Õë£¬KUiObjAtContRegion
	//				½á¹¹ÓÃÓÚ´æ´¢ÎïÆ·µÄÊý¾Ý¼°Æä·ÅÖÃÇøÓòÎ»ÖÃÐÅÏ¢¡£
	//				ÆäÖÐKUiObjAtContRegion::nContainerÖµ±íÊ¾µÚ¼¸Ò³µÄÎïÆ·
	//nParam = pInfoÊý×éÖÐ°üº¬KUiObjAtContRegion½á¹¹µÄÊýÄ¿
	//Return = Èç¹û·µ»ØÖµÐ¡ÓÚµÈÓÚ´«Èë²ÎÊýnParam£¬ÆäÖµ±íÊ¾pInfoÊý×éÖÐµÄÇ°¶àÉÙ¸öKUiObjAtContRegion
	//			½á¹¹±»Ìî³äÁËÓÐÐ§µÄÊý¾Ý£»·ñÔò±íÊ¾ÐèÒª´«Èë°üº¬¶àÉÙ¸öKUiObjAtContRegion½á¹¹µÄÊý×é
	//			²Å¹»´æ´¢È«²¿µÄnpcÁÐ³öÀ´½»Ò×µÄÎïÆ·ÐÅÏ¢¡£

	GDI_TRADE_ITEM_PRICE,		//½»Ò×ÎïÆ·µÄ¼Û¸ñ
	//uParam = (KUiObjAtContRegion*) pItemInfo -> ÓÃÓÚÖ¸³öÊÇÄÄ´¦µÄÄÄ¸öÎïÆ·
	//nParam = (KUiItemBuySelInfo*) pPriceInfo -> ÓÃÓÚ½ÓÊÕÎïÆ·Ãû³Æ½»Ò×¼Û¸ñµÈÐÅÏ¢
	//Return = (int)(bool) ¿É·ñ½»Ò×

	GDI_REPAIR_ITEM_PRICE,		//ÐÞÀíÎïÆ·µÄ¼Û¸ñ
	//uParam = (KUiObjAtContRegion*) pItemInfo -> ÓÃÓÚÖ¸³öÊÇÄÄ´¦µÄÄÄ¸öÎïÆ·
	//nParam = (KUiItemBuySelInfo*) pPriceInfo -> ÓÃÓÚ½ÓÊÕÎïÆ·Ãû³ÆÐÞÀí·ÑÓÃµÈÐÅÏ¢
	//Return = (int)(bool) ¿É·ñÐÞÀí

	GDI_TRADE_OPER_DATA,		//½»Ò×²Ù×÷Ïà¹ØµÄÊý¾Ý
	//uParam = (UI_TRADE_OPER_DATA)eOper ¾ßÌåº¬Òå¼ûUI_TRADE_OPER_DATA
	//nParam ¾ßÌåÓ¦ÓÃÓëº¬ÒåÓÉuParamµÄÈ¡Öµ×´¿ö¾ö¶¨,¼ûUI_TRADE_OPER_DATAµÄËµÃ÷
	//Return ¾ßÌåº¬ÒåÓÉuParamµÄÈ¡Öµ×´¿ö¾ö¶¨,¼ûUI_TRADE_OPER_DATAµÄËµÃ÷

	GDI_LIVE_SKILL_BASE,		
	//uParam = (KUiPlayerLiveSkillBase*) pInfo -> Ö÷½ÇµÄÉú»î¼¼ÄÜÊý¾Ý

	GDI_LIVE_SKILLS,			
	//uParam = (KUiSkillData*) pSkills -> °üº¬10¸öKUiSkillDataµÄÊý×éÓÃÓÚ´æ´¢¸÷ÏîÉú»î¼¼ÄÜµÄµÄÊý¾Ý¡£

	GDI_FIGHT_SKILL_POINT,		
	//Return = Ê£ÓàÕ½¶·¼¼ÄÜµãÊý

	GDI_FIGHT_SKILLS,			
	//uParam = (KUiSkillData*) pSkills -> °üº¬50¸öKUiSkillDataµÄÊý×éÓÃÓÚ´æ´¢¸÷ÏîÕ½¶·µÄÊý¾Ý¡£

	GDI_CUSTOM_SKILLS,			
	//uParam = (KUiSkillData*) pSkills -> °üº¬5¸öKUiSkillDataµÄÊý×éÓÃÓÚ´æ´¢¸÷Ïî×Ô´´Îä¹¦µÄÊý¾Ý¡£

	GDI_LEFT_ENABLE_SKILLS,		
	//uParam = (KUiSkillData*) pSkills -> °üº¬65¸öKUiSkillDataµÄÊý×éÓÃÓÚ´æ´¢¸÷¼¼ÄÜµÄÊý¾Ý¡£
	//								KUiSkillData::nLevelÓÃÀ´±íÊö¼¼ÄÜÏÔÊ¾ÔÚµÚ¼¸ÐÐ
	//Return = ·µ»ØÓÐÐ§Êý¾ÝµÄSkillsµÄÊýÄ¿

	GDI_RIGHT_ENABLE_SKILLS,	
	//uParam = (KUiSkillData*) pSkills -> °üº¬65¸öKUiSkillDataµÄÊý×éÓÃÓÚ´æ´¢¸÷¼¼ÄÜµÄÊý¾Ý¡£
	//								KUiSkillData::nLevelÓÃÀ´±íÊö¼¼ÄÜÏÔÊ¾ÔÚµÚ¼¸ÐÐ
	//Return = ·µ»ØÓÐÐ§Êý¾ÝµÄSkillsµÄÊýÄ¿

	GDI_NEARBY_PLAYER_LIST,
	//uParam = (KUiPlayerItem*)pList -> ÈËÔ±ÐÅÏ¢ÁÐ±í
	//			KUiPlayerItem::nData = 0
	//nParam = pListÊý×éÖÐ°üº¬KUiPlayerItem½á¹¹µÄÊýÄ¿
	//Return = Èç¹û·µ»ØÖµÐ¡ÓÚµÈÓÚ´«Èë²ÎÊýnParam£¬ÆäÖµ±íÊ¾pListÊý×éÖÐµÄÇ°¶àÉÙ¸öKUiPlayerItem
	//			½á¹¹±»Ìî³äÁËÓÐÐ§µÄÊý¾Ý£»·ñÔò±íÊ¾ÐèÒª´«Èë°üº¬¶àÉÙ¸öKUiPlayerItem½á¹¹µÄÊý×é
	//			²Å¹»´æ´¢È«²¿ÈËÔ±ÐÅÏ¢¡£

	GDI_NEARBY_IDLE_PLAYER_LIST,//»ñÈ¡ÖÜÎ§¹Âµ¥¿ÉÊÜÑûÇëµÄÍæ¼ÒµÄÁÐ±í

	GDI_NEARBY_NOT_FRIEND_LIST,//»ñÈ¡ÖÜÎ§·ÇºÃÓÑµÄÍæ¼ÒÁÐ±í

	GDI_PLAYER_LEADERSHIP,		//Ö÷½ÇÍ³Ë§ÄÜÁ¦Ïà¹ØµÄÊý¾Ý
	//uParam = (KUiPlayerLeaderShip*) -> Ö÷½ÇÍ³Ë§ÄÜÁ¦Ïà¹ØµÄÊý¾Ý½á¹¹Ö¸Õë

	GDI_ITEM_IN_ENVIRO_PROP,	//»ñµÃÎïÆ·ÔÚÄ³¸ö»·¾³Î»ÖÃµÄÊôÐÔ×´Ì¬
	//uParam = (KUiGameObject*)pObj£¨µ±nParam==0Ê±£©ÎïÆ·µÄÐÅÏ¢
	//uParam = (KUiObjAtContRegion*)pObj£¨µ±nParam!=0Ê±£©ÎïÆ·µÄÐÅÏ¢
	//			´ËÊ±KUiObjAtContRegion::RegionµÄÊý¾Ý¹Ì¶¨Îª0£¬ÎÞÒâÒå¡£
	//nParam = (int)(bool)bJustTry  ÊÇ·ñÖ»ÊÇ³¢ÊÔ·ÅÖÃ
	//Return = (ITEM_IN_ENVIRO_PROP)eProp ÎïÆ·µÄÊôÐÔ×´Ì¬
	
	GDI_CHAT_SEND_CHANNEL_LIST,		//Íæ¼Ò¿ÉÒÔ·¢ËÍÏûÏ¢µÄÁÄÌìÆµµÀµÄÁÐ±í
	//uParam = (KUiChatChannel*) pList -> KUiChatChannel½á¹¹Êý×éµÄÖ¸Õë
	//nParam = pListÊý×éÖÐ°üº¬KUiChatChannel½á¹¹µÄÊýÄ¿
	//Return = Èç¹û·µ»ØÖµÐ¡ÓÚµÈÓÚ´«Èë²ÎÊýnParam£¬ÆäÖµ±íÊ¾pInfoÊý×éÖÐµÄÇ°¶àÉÙ¸öKUiChatChannel
	//			½á¹¹±»Ìî³äÁËÓÐÐ§µÄÊý¾Ý£»·ñÔò±íÊ¾ÐèÒª´«Èë°üº¬¶àÉÙ¸öKUiChatChannel½á¹¹µÄÊý×é
	//			²Å¹»´æ´¢È«²¿µÄÖ¸¶¨ÆµµÀÐÅÏ¢¡£

	GDI_CHAT_RECEIVE_CHANNEL_LIST,	//Íæ¼Ò¿ÉÒÔ¶©ÔÄµÄÏûÏ¢ÆµµÀµÄÁÐ±í
	//²ÎÊý¼°·µ»ØÖµº¬ÒåÍ¬GDI_CHAT_SEND_CHANNEL_LIST

	GDI_CHAT_CURRENT_SEND_CHANNEL,	//»ñÈ¡µ±Ç°·¢ËÍÏûÏ¢µÄÆµµÀµÄÐÅÏ¢
	//uParam = (KUiChatChannel*) pChannelInfo Ö¸ÏòÓÃÓÚÌî³äÆµµÀµÄÐÅÏ¢µÄ½á¹¹¿Õ¼ä

	GDI_CHAT_GROUP_INFO,			//ÁÄÌìµÄºÃÓÑ·Ö×éÐÅÏ¢
	//´ÓnParam¸ø¶¨µÄË÷Òý¿ªÊ¼²éÕÒµÚÒ»¸öÓÐÐ§µÄ·Ö×é£¬·µ»Ø¸Ã·Ö×éµÄÐÅÏ¢Óë·Ö×éË÷Òý¡£
	//uParam = (KUiChatGroupInfo*) pGroupInfo ·Ö×éÐÅÏ¢
	//nParam = nIndex Óû»ñÈ¡µÄ·Ö×éµÄË÷Òý
	//Return = Êµ¼Ê·µ»ØÊý¾ÝµÄ·Ö×éµÄË÷Òý£¬Èç¹ûÎ´»ñµÃÔò·µ»Ø-1

	GDI_CHAT_FRIENDS_IN_AGROUP,		//ÁÄÌìÒ»¸öºÃÓÑ·Ö×éÖÐºÃÓÑµÄÐÅÏ¢
	//uParam = (KUiPlayerItem*)pList -> ÈËÔ±ÐÅÏ¢ÁÐ±í
	//			KUiPlayerItem::nData = (CHAT_STATUS)eFriendStatus ºÃÓÑµÄµ±Ç°×´Ì¬
	//nParam = Òª»ñÈ¡ÁÐ±íµÄºÃÓÑ·Ö×éµÄË÷Òý
	//Return = ÆäÖµ±íÊ¾pListÊý×éÖÐµÄÇ°¶àÉÙ¸öKUiPlayerItem½á¹¹±»Ìî³äÁËÓÐÐ§µÄÊý¾Ý.

	GDI_PK_SETTING,					//»ñÈ¡pkÉèÖÃ
	//Return = (int)(bool)bEnable	ÊÇ·ñÔÊÐípk

	GDI_SHOW_PLAYERS_NAME,			//»ñÈ¡ÏÔÊ¾¸÷Íæ¼ÒÈËÃû
	//Return = (int)(bool)bShow	ÊÇ·ñÏÔÊ¾
	GDI_SHOW_PLAYERS_LIFE,			//»ñÈ¡ÏÔÊ¾¸÷Íæ¼ÒÉúÃü
	//Return = (int)(bool)bShow	ÊÇ·ñÏÔÊ¾
	GDI_SHOW_PLAYERS_MANA,			//»ñÈ¡ÏÔÊ¾¸÷Íæ¼ÒÄÚÁ¦
	//Return = (int)(bool)bShow	ÊÇ·ñÏÔÊ¾
	
	GDI_PARADE_EQUIPMENT,				//¿´Íæ¼Ò×°±¸ÎïÆ·,ÏûÏ¢º¬ÒåÍ¬GDI_EQUIPMENT
	
	GDI_IMMEDIATEITEM_NUM,				
	
	GDI_SHOW_OBJ_NAME, // HIEN TEN OBJ DUOI DAT edit by phong kieu

	GDI_IS_CHEST_UNLOCKED,	// khoa ruong

	GDI_ITEM_IN_EX_BOX1, // ruong mo rong 1

	GDI_ITEM_IN_EX_BOX2,	// ruong mo rong 2
	
	GDI_ITEM_IN_EX_BOX3,	// ruong mo rong 3

	GDI_ITEM_EX,			// hanh trang mo rong
	
	GDI_EXBOX_ID,		// truyen task ruong mo rong

	GDI_TRADE_PLAYER_ITEM,
	
	GDI_TRADE_PLAYER_ITEM_COUNT,
	
	GDI_TRADE_PLAYER_UPDATE,
	
	GDI_PLAYER_IS_BAITAN,

	GDI_IS_CHECK_IMAGE,

	GDI_ITEM_CHAT,

	GDI_ITEM_NAME,

	GDI_GET_ITEM_PARAM,

	GDI_CHAT_ITEM_DESC,

	GDI_CHAT_ITEM_IMAGE,

	GDI_NPC_STATE_SKILL,

	GDI_AFFAIR_ITEM,

	GDI_IS_TONG_MASTER,

	GDI_TREMBLE_ITEM,

	GDI_MISSION_SELFDATA,

	GDI_MISSION_RANKDATA,

	GDI_ITEM_EQUIP_ROOM_LIST,

	GDI_PLAYER_HOLD_FKCOIN,		//add by phong kiÒu sè xu ng­êi trªn player
};

enum GAMEDATA_CHANGED_NOTIFY_INDEX
{
	GDCNI_HOLD_OBJECT = 1,
	//uParam = (KUiGameObject*)pObject -> 
	GDCNI_PLAYER_BASE_INFO,	//see also GDI_PLAYER_BASE_INFO
	GDCNI_PLAYER_RT_ATTRIBUTE,	// see also GDI_PLAYER_RT_ATTRIBUTE
	GDCNI_PLAYER_IMMED_ITEMSKILL,
	//uParam = (KUiGameObject*)pInfo ÎïÆ·»òÕßÎä¹¦µÄÊý¾Ý
	//nParam = ÄÄ¸öÎ»ÖÃµÄÎïÆ·»òÕßÎä¹¦ µ±nParam >=0 Ê±ºò nParam±íÊ¾µÚnParam¸öÁ¢¼´ÎïÆ·
	//			µ±nParam = -1 Ê±£¬±íÊ¾Îª×óÊÖÎä¹¦£¬-2Ê±ÎªÓÒÊÖÎä¹¦
	GDCNI_PLAYER_BRIEF_PROP,//
	//uParam = (PLAYER_BRIEF_PROP)eProp ±ä»¯ÄÚÈÝÏî£¬È¡ÖµÎªÃ¶¾ÙPLAYER_BRIEF_PROPµÄÖµÖ®Ò»¡£
	//nParam = º¬ÒåÒÀ¾ÝuParamµÄ¾ßÌåº¬Òå¶ø¶¨
	GDCNI_OBJECT_CHANGED,		//Ä³´¦ÎïÆ·µÄÐÂÔö»òÕß¼õÉÙ
	//uParam = (KUiObjAtContRegion*)pInfo -> ÎïÆ·Êý¾Ý¼°Æä·ÅÖÃÇøÓòÎ»ÖÃÐÅÏ¢
	//          µ±pInfo->eContainer == UOC_EQUIPTMENTÊ±
	//				KUiObjAtRegion::Region::h ±íÊ¾ÊôÓÚµÚ¼¸Ì××°±¸
	//				KUiObjAtRegion::Region::v ±íÊ¾ÊôÓÚÄÄ¸öÎ»ÖÃµÄ×°±¸,ÆäÖµÎªÃ¶¾ÙÀàÐÍ
	//				UI_EQUIPMENT_POSITIONµÄÈ¡ÖµÖ®Ò»¡£Çë²Î¿´UI_EQUIPMENT_POSITIONµÄ×¢ÊÍ¡£
	//nParam = bAdd -> 0Öµ±íÊ¾¼õÉÙÕâ¸öÎïÆ·£¬·Ç0Öµ±íÊ¾Ôö¼ÓÕâ¸öÎïÆ·
	GDCNI_CONTAINER_OBJECT_CHANGED,	//Õû¸öÃæ°åÀïµÄ¶«Î÷È«²¿¸üÐÂ
	//uParam = (UIOBJECT_CONTAINER)eContainer;	//Ãæ°åÎ»ÖÃ
	GDCNI_LIVE_SKILL_BASE,		//Éú»î¼¼ÄÜÊýÖµ±ä»¯

	GDCNI_FIGHT_SKILL_POINT,	//Ê£ÓàÕ½¶·¼¼ÄÜµãÊý±ä»¯
	//nParam = ÐÂµÄÊ£ÓàÕ½¶·¼¼ÄÜµãÊý
	GDCNI_SKILL_CHANGE,			//ÐÂÕÆÎÕÁËÒ»¸ö¼¼ÄÜ/¼¼ÄÜÉý¼‰
	//uParam = (KUiSkillData*)pSkill -> ÐÂÕÆÎÕµÄ¼¼ÄÜ
	//nParam = ÐÂ¼¼ÄÜÔÚÍ¬îÖÐµÄÅÅÐòÎ»ÖÃ
	GDCNI_PLAYER_LEADERSHIP,	//Ö÷½ÇÍ³Ë§ÄÜÁ¦Ïà¹ØµÄÊý¾Ý·¢Éú±ä»¯
	//
	GDCNI_TEAM,	
	//uParam = (KUiPlayerTeam*)pTeam -> ¶ÓÎéÐÅÏ¢,¿ÉÒÔÎªNULL,±íÊ¾Íæ¼ÒÍÑÀëÁË¶ÓÎé
	GDCNI_TEAM_NEARBY_LIST,		
	//uParam = (KUiTeamItem*)pList ´æ¶ÓÎéÐÅÏ¢µÄÊý×é
	//nParam = nCount ¶ÓÎéµÄÊýÄ¿
	GDCNI_TRADE_START,			//ÓëÍæ¼Ò½»Ò×¿ªÊ¼
	//uParam = (KUiPlayerItem*) pPlayer¶Ô·½µÄÐÅÏ¢
	GDCNI_TRADE_DESIRE_ITEM,	//¶Ô·½Ôö¼õÏë½»Ò×µÄÎïÆ·
	//uParam = (KUiObjAtRegion*) pObject -> ÎïÆ·ÐÅÏ¢£¬ÆäÖÐ×ø±êÐÅÏ¢ÎªÔÚ½»Ò×½çÃæÖÐµÄ×ø±ê
	//nParam = bAdd -> 0Öµ±íÊ¾¼õÉÙ£¬1Öµ±íÊ¾Ôö¼Ó
	//Remark : Èç¹ûÎïÆ·ÊÇ½ðÇ®µÄ»°£¬ÔòKUiObjAtRegion::Obj::uId±íÊ¾°Ñ½ðÇ®¶îµ÷ÕûÎªÕâ¸öÖµ£¬ÇÒnParamÎÞÒâÒå¡£
	GDCNI_TRADE_OPER_DATA,		//½»Ò×²Ù×÷Ïà¹ØµÄ(×´Ì¬)Êý¾Ý·¢Éú±ä»¯
	//uParam = (const char*) pInfoText ÌáÊ¾µÄÎÄ×Ö£¬Èç"¶Ô·½½â³ýËø¶¨"µÈ
	GDCNI_TRADE_END,			//½»Ò×½áÊø
	//nParam = (int)(bool)bTraded	ÊÇ·ñ½øÐÐÁË½»Ò×
	GDCNI_NPC_TRADE,			
	//nParam = (bool)bStart	ÊÇ·ñ½»Ò×Îª½»Ò×¿ªÊ¼£¬Èç¹ûÈ¡Öµ·ÇÕæ±íÊ¾½»Ò×£¨¹Ø±Õ£©½áÊø
	GDCNI_NPC_TRADE_ITEM,		
	//Í¨¹ýµ÷ÓÃiCoreShell::GetGameData,Ê¹²ÎÊýuDataIdÎªGDI_TRADE_NPC_ITEM£¬À´»ñÈ¡ÐÂµÄÎïÆ·ÐÅÏ¢¡£
	GDCNI_GAME_START,			//½øÈëÓÎÏ·ÊÀ½ç

	GDCNI_GAME_QUIT,			//Àë¿ªÁËÓÎÏ·ÊÀ½ç

	GDCNI_QUESTION_CHOOSE,		//ÎÊÌâÑ¡Ôñ
	//uParam = (KUiQuestionAndAnswer*)pQuestionAndAnswer
	GDCNI_SPEAK_WORDS,			//npcËµ»°ÄÚÈÝ
	//uParam = (KUiInformationParam*) pWordDataList Ö¸ÏòKUiInformationParamÊý×é
	//nParam = pWordDataList°üº¬KUiInformationParamÔªËØµÄÊýÄ¿
	GDCNI_INFORMATION,			//ÐÂµÄÐÅÏ¢À´µ½£¨µ¯³ö×¨ÃÅµÄÏûÏ¢¿ò£©
	//uParam = (KUiInformationParam*)pInformation	//ÏûÏ¢ÄÚÈÝ
	GDCNI_MSG_ARRIVAL,			//ÐÂ£¨ÁÄÌìÄÚÈÝ´°¿Ú£©ÏûÏ¢À´µ½
	//uParam = (cons char*)pMsgBuff ÏûÏ¢ÄÚÈÝ»º³åÇø
	//nParam = (KUiMsgParam*)pMsgParam ÏûÏ¢²ÎÊý
	GDCNI_SEND_CHAT_CHANNEL,	//µ±Ç°·¢ËÍÏûÏ¢µÄÆµµÀ¸Ä±äÁË

	GDCNI_CHAT_GROUP,			//ÁÄÌìºÃÓÑ·Ö×é·¢Éú±ä»¯

	GDCNI_CHAT_FRIEND,			//ÁÄÌìºÃÓÑ·¢Éú±ä»¯
	//nParam = nGroupIndex ·¢ÉúºÃÓÑ±ä»¯µÄ·Ö×éµÄË÷Òý
	GDCNI_CHAT_FRIEND_STATUS,	//ÁÄÌìºÃÓÑ×´Ì¬·¢Éú±ä»¯
	//uParam = (KUiPlayerItem*)pFriend ·¢Éú×´Ì¬±ä»¯µÄºÃÓÑ
	//			KUiPlayerItem::nData = (CHAT_STATUS)eFriendStatus ºÃÓÑµÄµ±Ç°×´Ì¬
	//nParam = nGroupIndex ºÃÓÑËùÊôÓÚµÄ×éµÄË÷Òý
	GDCNI_CHAT_MESSAGE,			//ºÃÓÐ·¢À´µÄÑ¶Ï¢
	//uParam = (KUiChatMessage*)pMessage ÏûÏ¢Êý¾Ý
	//nParam = (KUiPlayerItem*)pFriend   ·¢À´Ñ¶Ï¢µÄºÃÓÑ
	GDCNI_SYSTEM_MESSAGE,		//ÏµÍ³ÏûÏ¢
	//uParam = (KSystemMessage*)pMsg ÏµÍ³ÏûÏ¢µÄÄÚÈÝ
	//nParam = (void*)pParamBuf Ö¸ÏòÒ»¸ö²ÎÊý»º³åÇø£¬»º³åÄÚÊý¾Ýº¬ÒåÒÀ¾ÝpMsgµÄÄÚÈÝ¶ø¶¨£¬
	//			²Î¿´KSystemMessageµÄ×¢ÊÍËµÃ÷£¬»º³åÇøµÄ´óÐ¡ÓÉpMsg->byParamSize¸ø³ö¡£
	GDCNI_NEWS_MESSAGE,			//ÐÂÎÅÏûÏ¢
	//uParam = (KNewsMessage*)pMsg ÐÂÎÅÏûÏ¢ÄÚÈÝ
	//nParam = (SYSTEMTIME*)pTime  ÐÂÎÅÏûÏ¢µÄÊ±¼ä²ÎÊý£¬¾ßÌåº¬ÒåÒÀÀµÓÚÐÂÎÅÏûÏ¢µÄÀàÐÍ¶ø¶¨
	GDCNI_SWITCH_CURSOR,		//ÇÐ»»Êó±êÖ¸ÕëÍ¼ÐÎ
	
	GDCNI_OPEN_STORE_BOX,	
	
	GDCNI_SWITCHING_SCENEPLACE,	//µØÍ¼ÇÐ»»
	//nParam = (int)(bool)bStart Îª·Ç0Öµ±íÊ¾¿ªÊ¼µØÍ¼ÇÐ»»£¬Îª0Öµ±íÊ¾½áÊøµØÍ¼ÇÐ»»
	GDCNI_MISSION_RECORD,		//ÒªÇó¼ÍÂ¼ÏÂ´ËÈÎÎñÌáÊ¾ÐÅÏ¢
	//uParam = (KMissionRecord*) pRecord ¼ÍÂ¼ÄÚÈÝ
	GDCNI_PK_SETTING,			//pkÔÊÐí×´¿ö·¢ÉúÁË±ä»¯
	//nParam = (int)(bool)bEnable ÊÇ·ñÔÊÐípk
	GDCNI_VIEW_PLAYERITEM,			//±»ÉêÇëÍæ¼Ò×°±¸µÄÊý¾ÝÒÑµ½
	//uParam = (KUiPlayerItem*)		//Íæ¼ÒµÄ player data
	GDCNII_RANK_INDEX_LIST_ARRIVE,	//ÅÅÃûÏîidÁÐ±íµ½ÁË
	//uParam = uCount ÅÅÃûÏîidµÄÊýÄ¿
	//nParam = (KRankIndex*)pIndexList ÅÅÃûÏîidÃèÊö½á¹¹µÄÊý×é
	GDCNII_RANK_INFORMATION_ARRIVE,	//ÅÅÃûÏîµÄÄÚÈÝµ½ÁË
	//uParam = ((unsigned short)usCount) | ((unsigned short)usIndexId << 16)
	//			usCount    ´Ë´Î´«À´µÄÄÚÈÝÏîµÄÊýÄ¿
	//			usIndexId  ÅÅÃûÏîid£¬±íÊ¾´«»ØµÄÊÇÕâ¸öidµÄÅÅÃûÏîµÄÄÚÈÝ
	//nParam = (KRankMessage *) ÁÐ±íÄÚÈÝ
	//====°ïÅÉÏà¹Ø====
	GDCNI_TONG_INFO,				//Ä³ÈËËùÔÚµÄ°ïÅÉµÄÐÅÏ¢
									//¶ÔÓ¦ÓÚGTOI_REQUEST_PLAYER_TONG£¨²éÑ¯Ä³ÈËÊÇÄÄ°ï»á£©
	//uParam = (KUiPlayerRelationWithOther*) ÄÇ¸ö£¨Ä³£©ÈË,ÒÔ¼°´ËÈËÓë´Ë°ïÅÉµÄ¹ØÏµ
					//KUiPlayerRelationWithOther::nParam ÊÇ·ñ¿ª×ÅµÄÕÐÈË¿ª¹Ø
	//nParam = (KTongInfo*) pToneInfo	//°ïÅÉµÄÐÅÏ¢
	GDCNI_TONG_MEMBER_LIST,			//Ä³¸ö°ïÅÉÖÐµÄ³ÉÔ±µÄÁÐ±í
	//uParam = (KUiGameObjectWithName*) pTong ÓÃÓÚÃèÊöÊÇÄÄ¸ö°ïÅÉ
							//KUiGameObjectWithName::szName °ïÅÉÃû³Æ
							//KUiGameObjectWithName::nData  pMemberListËù¸ø°ïÅÉ³ÉÔ±µÄÀàÐÍ£¬²Î¿´TONG_MEMBER_FIGURE
							//KUiGameObjectWithName::nParam pMemberListËù¸ø°ïÅÉ³ÉÔ±ÁÐ±í°üº¬³ÉÔ±µÄÆðÊ¼µÄË÷Òý
							//KUiGameObjectWithName::uParam pMemberListËù¸ø°ïÅÉ³ÉÔ±ÁÐ±í°üº¬³ÉÔ±µÄÊýÄ¿
	//nParam = (KTongMemberItem*) pMemberList
	GDCNI_TONG_ACTION_RESULT,       //°ï»á²Ù×÷·µ»ØµÄ½á¹û
	//uParam = (KUiGameObjectWithName*) pInfo ·µ»ØµÄÊý¾Ý
	                        //KUiGameObjectWithName::szName ¶ÔÏóÍæ¼ÒµÄÃû×Ö
							//KUiGameObjectWithName::nData  ²Ù×÷µÄÖÖÀà£¬²Î¿¼TONG_ACTION_TYPE
							//KUiGameObjectWithName::nParam   V
							//KUiGameObjectWithName::uParam   V
							//KUiGameObjectWithName::szString Õâ¼¸¸öº­ÒåËæ²»Í¬²Ù×÷²»Í¬
	// nParam : ³É¹¦ 1 Ê§°Ü 0
	GDCNI_OPEN_TONG_CREATE_SHEET,   

	GDCNI_OPEN_RESET_PASS,				//cho phep reset pass tu sv

	GDCNI_OPEN_EX_BOX, // mo rong ruong 1
	
	GDCNI_OPEN_EX_BOX2, // mo rong ruong 2
	
	GDCNI_OPEN_EX_BOX3, // mo rong ruong 3
	
	GDCNI_OPEN_ITEMEX, // mo rong hanh trang

	GDCNI_VIEW_PLAYERSELLITEM,
	
	GDCNI_VIEW_PLAYERUPDATEITEM,
	
	GDCNI_CLOSE_BAITAN,

	GDCNI_S2C_EXIT_GAME,

	GDCNI_OPEN_DATAU_BOX,
	
	GDCNI_OPEN_DATAU_BOX1,

	GDCNI_UPDATE_BATTLE_BOX,

	GDCNI_NEWS_MESSAGE_1,

	GDCNI_SUPERSHOP,

	GDCNI_CHATROOM_UPDATE_INTERFACE,

	GDCNI_OPEN_AFFAIR_BOX,

	GDCNI_END_AFFAIR_BOX,

	GDCNI_OPEN_TIME_BOX,

	GDCNI_OPEN_TALK_EX,

	GDCNI_OPEN_INPUT,

	GDCNI_OPEN_INPUT2,

	GDCNI_RETURN_CITY_OWN_TONG,

	GDCNI_SWITCHING_MAPMODE,

	GDCNI_PLAYER_LOGIN_REPLAY, //fix by phong kiÒu chuyÓn gs bÞ mÊt skill

	GDCNI_FINISH_QUEST_DLG,

	GDCNI_QUESTION_CHOOSE_3,

	GDCNI_QUESTION_CHOOSE_4,

	GDCNI_OPEN_TREMBLE_ITEM,

	GDCNI_OPEN_COMPOUND_ITEM,

	GDCNI_PLAY_SOUND,

	GDCNI_AUTO_SET_HOTKEY,

	GDCNI_AUTO_SET_HOTKEY_DR,

	GDCNI_FK_AUTO_TALK,

	GDCNI_FK_AUTO_SELECTUI,

	GDCNI_FK_AUTO_ITEM,

	GDCNI_USE_SHORCUT_SKILL,

	GDCNI_AUTO_HOTKEY_CAST_B,
};

enum GAMEDEBUGCONTROL
{
	DEBUG_SHOWINFO = 1,					//show info debug edit by phong kieu mac dinh la 1
	DEBUG_SHOWOBSTACLE,
};

enum GAMEOPERATION_INDEX
{
	GOI_EXIT_GAME = 1,		

	GOI_SWITCH_OBJECT_QUERY,		

	GOI_SWITCH_OBJECT,	

	GOI_REJECT_OBJECT,	

	GOI_MONEY_INOUT_STORE_BOX,	

	GOI_PLAYER_ACTION,

	GOI_PLAYER_RENASCENCE,		

	GOI_INFORMATION_CONFIRM_NOTIFY,

	GOI_QUESTION_CHOOSE,

	GOI_USE_ITEM,	
	//uParam = (KUiObjAtRegion*)pInfo ->

	GOI_WEAR_EQUIP,			
	//uParam = (KUiObjAtRegion*)pInfo -> 
	//			KUiObjAtRegion::Region::h
	//			KUiObjAtRegion::Region::v

	GOI_USE_SKILL,		
	//uParam = (KUiGameObject*)pInfo -> ¼¼ÄÜÊý¾Ý
	//nParam = (Ä¿Ç°Ö»´«0Öµ¡£)Êó±êÖ¸Õëµ±Ç°×ø±ê£¨¾ø¶Ô×ø±ê£©£¬ºá×ø±êÔÚµÍ16Î»£¬×Ý×ø±êÔÚ¸ß16Î»¡£(ÏñËØµã×ø±ê)

	GOI_SET_IMMDIA_SKILL,	//ÉèÖÃÁ¢¼´¼¼ÄÜ
	//uParam = (KUiGameObject*)pSKill, ¼¼ÄÜÐÅÏ¢
	//nParam = Á¢¼´Î»ÖÃ£¬0±íÊ¾Îª×ó¼ü¼¼ÄÜ£¬1±íÊ¾ÎªÓÒ¼ü¼¼ÄÜ

	GOI_TONE_UP_SKILL,		//ÔöÇ¿Ò»ÖÖ¼¼ÄÜ£¬£¬Ò»´Î¼ÓÒ»µã
	//uParam = ¼¼ÄÜÀàÊô
	//nParam = (uint)¼¼ÄÜid

	GOI_TONE_UP_ATTRIBUTE,	//ÔöÇ¿Ò»Ð©ÊôÐÔµÄÖµ£¬Ò»´Î¼ÓÒ»µã
	//uParam = ±íÊ¾ÒªÔöÇ¿µÄÊÇÄÄ¸öÊôÐÔ£¬È¡ÖµÎªUI_PLAYER_ATTRIBUTEµÄÃ·¾ÙÖµÖ®Ò»

	//============£¨ÓëÆäËüÍæ¼Ò£©½»Ò×Ïà¹Ø================
	GOI_TRADE_INVITE_RESPONSE,	//´ðÓ¦/¾Ü¾ø½»Ò×ÇëÇó
	//uParam = (KUiPlayerItem*)pRequestPlayer ·¢³öÇëÇóµÄÍæ¼Ò
	//nParam = (int)(bool)bAccept ÊÇ·ñ½ÓÊÜÇëÇó

	GOI_TRADE_DESIRE_ITEM,		//Ôö¼õÒ»¸öÓûÂô³öµÄÎïÆ·
	//uParam = (KUiObjAtRegion*) pObject -> ÎïÆ·ÐÅÏ¢£¬ÆäÖÐ×ø±êÐÅÏ¢ÎªÔÚ½»Ò×½çÃæÖÐµÄ×ø±ê
	//nParam = bAdd -> 0Öµ±íÊ¾¼õÉÙ£¬1Öµ±íÊ¾Ôö¼Ó
	//Remark : Èç¹ûÎïÆ·ÊÇ½ðÇ®µÄ»°£¬ÔòKUiObjAtRegion::Obj::uId±íÊ¾°Ñ½ðÇ®¶îµ÷ÕûÎªÕâ¸öÖµ£¬ÇÒnParamÎÞÒâÒå¡£

	GOI_TRADE_WILLING,			//giao dich giao dÞch
	//uParam = (const char*)pszTradMsg ¹ØÓÚ½»Ò×ÏûÏ¢Ò»¾ä»°£¬µ±bWillingÎªtrueÊ±ÓÐÐ§
	//nParam = (int)(bool)bWilling ÊÇ·ñÆÚ´ý½»Ò×(½ÐÂô)

	GOI_TRADE_LOCK,				//Ëø¶¨½»Ò×
	//nParam = (int)(bool)bLock ÊÇ·ñËø¶¨

	GOI_TRADE,					//½»Ò×
	//nParam = (int)(bool)bTrading
	
	GOI_TRADE_CANCEL,			//½»Ò×È¡Ïû

	//============================
	GOI_TRADE_NPC_BUY,
	//uParam = (KUiGameObject*)pObj -> ÎïÆ·ÐÅÏ¢

	GOI_TRADE_NPC_SELL,			
	//uParam = (KUiObjAtContRegion*)pObj -> ÎïÆ·ÐÅÏ¢

	GOI_TRADE_NPC_REPAIR,		//ÐÞÀíÎïÆ·
	//uParam = (KUiObjAtContRegion*) pObj -> ÏëÒªµÄÎïÆ·µÄÐÅÏ¢

	GOI_TRADE_NPC_CLOSE,		//½áÊø½»Ò×

	GOI_DROP_ITEM_QUERY,		//²éÑ¯ÊÇ·ñ¿ÉÒÔ¶ªÄ³¸ö¶«Î÷µ½ÓÎÏ·´°¿Ú
	//uParam = (KUiGameObject*)pObject -> ÎïÆ·ÐÅÏ¢
	//nParam = ±»ÍÏ¶¯¶«Î÷µÄµ±Ç°×ø±ê£¨¾ø¶Ô×ø±ê£©£¬ºá×ø±êÔÚµÍ16Î»£¬×Ý×ø±êÔÚ¸ß16Î»¡£(ÏñËØµã×ø±ê)
	//Return = ÊÇ·ñ¿ÉÒÔ·ÅÏÂ
	
//	GOI_DROP_ITEM,				//·ÅÖÃÎïÆ·µ½ÓÎÏ·´°¿Ú
	//²ÎÊýº¬ÒåÍ¬GOI_DROP_ITEM_QUERY²ÎÊýº¬ÒåÏàÍ¬
	//Return = ÊÇ·ñ¶«Î÷±»·ÅÏÂÁË

	GOI_SEND_MSG,				//·¢ËÍÏûÏ¢
	//uParam = (const char*)pMsgBuff ÏûÏ¢ÄÚÈÝ»º³åÇø
	//nParam = (KUiMsgParam*)pMsgParam ÏûÏ¢²ÎÊý
	
	//============ÁÄÌìÏà¹Ø================
	GOI_SET_SEND_CHAT_CHANNEL,	//ÉèÖÃµ±Ç°·¢ËÍÏûÏ¢µÄÆµµÀ
	//uParam = (KUiChatChannel*) pChannelInfo ÒªÉèÖÃµÄÆµµÀµÄÐÅÏ¢
	
	GOI_SET_SEND_WHISPER_CHANNEL,//ÉèÖÃµ±Ç°·¢ËÍÏûÏ¢µÄÆµµÀÎªÓëÄ³ÈËµ¥ÁÄ
	//uParam = (KUiPlayerItem*) pFriend

	GOI_SET_RECEIVE_CHAT_CHANNEL,//ÉèÖÃ¶©ÔÄ/È¡ÏûÏûÏ¢ÆµµÀ
	//uParam = (KUiChatChannel*) pChannelInfo ÒªÉèÖÃµÄÆµµÀµÄÐÅÏ¢
	//nParam = (int)(bool) bEnable ÊÇ·ñ¶©ÔÄ

	GOI_CHAT_GROUP_NEW,			//ÐÂ½¨ÁÄÌìºÃÓÑ×é
	//uParam = (const char*) pGroupName
	//Return = (int)(bool) bSuccessed

	GOI_CHAT_GROUP_RENAME,		//ÖØÃüÃûÁÄÌìºÃÓÑ×é
	//uParam = (const char*) pGroupName
	//nParam = nIndex ×éË÷Òý
	//Return = (int)(bool) bSuccessed

	GOI_CHAT_GROUP_DELETE,		//É¾³ýÁÄÌìºÃÓÑ×é
	//nParam = nIndex ×éË÷Òý
	//Return = (int)(bool) bSuccessed

	GOI_CHAT_FRIEND_ADD,		//ÐÂÌí¼ÓÁÄÌìºÃÓÑ
	//uParam = (KUiPlayerItem*)pFriend
	//			KUiPlayerItem::nData = 0

	GOI_CHAT_FRIEND_DELETE,		//É¾³ýÁÄÌìºÃÓÑ
	//uParam = (KUiPlayerItem*)pFriend
	//			KUiPlayerItem::nData = 0
	//nParam = nGroupIndex ºÃÓÑËùÔÚµÄ×éµÄË÷Òý
	//Return = (int)(bool) bSuccessed

	GOI_CHAT_FRIEND_MOVE,		//ºÃÓÑÒÆµ½ÐÂ×é
	//uParam = (KUiPlayerItem*)pFriend
	//			KUiPlayerItem::nData = 0
	//nParam = nGroupIndex ºÃÓÑËùµ½µÄÐÂ×éµÄË÷Òý
	//Return = (int)(bool) bSuccessed

	GOI_CHAT_SET_STATUS,		//ÇÐ»»ÁÄÌì×´Ì¬
	//uParam = (CHAT_STATUS)eStatus ÐÂµÄÁÄÌì×´Ì¬

	GOI_CHAT_WORDS_TO_FRIEND,	//·¢¸øºÃÓÑÒ»ÌõÑ¶Ï¢
	//uParam = (KUiChatMessage*)pMessage ÏûÏ¢Êý¾Ý
	//nParam = (KUiPlayerItem*)pFriend   ·¢À´Ñ¶Ï¢µÄºÃÓÑ

	GOI_CHAT_FRIEND_INVITE,		//¶Ô±ðÈËÒª¼Ó×Ô¼ºÎªºÃÓÑµÄ»Ø¸´
	//uParam = (KUiPlayerItem*)pRequestPlayer ·¢³öÇëÇóµÄÍæ¼Ò
	//nParam = (int)(bool)bAccept ÊÇ·ñ½ÓÊÜÇëÇó

	GOI_OPTION_SETTING,			
	//uParam = (OPTIONS_LIST)eOptionItem ÒªÉèÖÃµÄÑ¡Ïî
	//nParam = (int)nValue ÉèÖÃµÄÖµ£¬Æäº¬ÒåÒÀÀµÓÚeOptionItemµÄº¬Òå
	//					²Î¿´OPTIONS_LIST¸÷ÖµµÄ×¢ÊÍ

	GOI_PLAY_SOUND,				//²¥·ÅÉùÒô
	//uParam = (const char*)pszFileName

	GOI_PK_SETTING,				//ÉèÖÃPK
	//nParam = (int)(bool)bEnable	ÊÇ·ñÔÊÐípk

	GOI_REVENGE_SOMEONE,		//³ðÉ±Ä³ÈË
	//uParam = (KUiPlayerItem*) pTarget	³ðÉ±Ä¿±ê

	GOI_SHOW_PLAYERS_NAME,		//ÏÔÊ¾¸÷Íæ¼ÒÈËÃû
	//nParam = (int)(bool)bShow	ÊÇ·ñÏÔÊ¾
	GOI_SHOW_PLAYERS_LIFE,		//ÏÔÊ¾¸÷Íæ¼ÒÉúÃü
	//nParam = (int)(bool)bShow	ÊÇ·ñÏÔÊ¾
	GOI_SHOW_PLAYERS_MANA,		
	//nParam = (int)(bool)bShow	ÊÇ·ñÏÔÊ¾

	GOI_GAMESPACE_DISCONNECTED,	
	
	GOI_VIEW_PLAYERITEM,		
	//uParam = dwNpcID	Íæ¼ÒµÄm_dwID
	GOI_VIEW_PLAYERITEM_END,	

	GOI_FOLLOW_SOMEONE,			
	//uParam = (KUiPlayerItem*) pTarget	¸úËæÄ¿±ê

	GOI_QUERY_RANK_INFORMATION,  
	//uParam = usIndexId ÅÅÃûÏîµÄid

	GOI_SHOW_OBJ_NAME, // hien ten obj duoi dat edit by phong kieu

	GOI_CP_UNLOCK,		//open ruong
	
	GOI_CP_LOCK,		//close ruong

	GOI_CP_CHANGE,		//doi mk ruong
	
	GOI_CP_RESET,		//reset mk ruong

	GDI_SET_TRADE_ITEM,			// ban hang

	GDI_PLAYER_TRADE,			//°ÚÌ¯
	
	GOI_VIEW_PLAYERSELLITEM,
	
	GOI_VIEW_PLAYERSELLITEM_END,
	
	GOI_TRADE_PLAYER_BUY,

	GOI_PLAYER_SYSSHOP,

	GOI_LIXIAN,

	GOI_SAY_NEW,

	GOI_LOCK_PLAYER_ITEM,

	GOI_NPC_ITEM_BREAK,

	GOI_DATAU,

	GOI_DATAU1,

	GOI_SUPERSHOP,

	GOI_AUTO_PLAY,

	GAUTO_AUTO_MOVEMPSID,
	
	GAUTO_AUTO_MOVEMPSX,
	
	GAUTO_AUTO_MOVEMPSY,

	GOI_PLAYER_ACTIONCHAT,

	GOI_CP_SET_IMAGE_PLAYER,

	GOI_ADD_UI_CMD_SCRIPT,

	GOI_RECOVERY_BOX_COMMAND,

	GOI_INPUT_INFO,

	GOI_GET_CITY_OWN_TONG,

	GOI_RECOVER_ITEM,

	GAUTO_AUTO_PT_PLAYERTEAM,//qu¶n lý tæ ®éi

	GAUTO_AUTO_MOVETPSID, //ch¹y to¹ ®é ra b·i train
	
	GAUTO_AUTO_MOVETPSX,
	
	GAUTO_AUTO_MOVETPSY,

	GAUTO_AUTO_BLACK_ITEM, //qu¶n lý vËt phÈm ®en

	GDI_THROW_ALL_ITEM,

	GOI_RCLICK_MOVE_ITEM,

	GOI_SUPPERSHOP_TRADE_NPC_BUY,
};

//=========================================================
// CoreÍâ²¿¿Í»§¶ÔcoreµÄ³¡¾°µØÍ¼Ïà¹ØµÄ²Ù×÷ÇëÇóµÄË÷Òý¶¨Òå
//=========================================================
//¸÷Êý¾ÝÏîË÷ÒýµÄÏà¹Ø²ÎÊýuParamÓënParamÈç¹ûÔÚ×¢ÊÍÖÐÎ´Ìá¼°£¬Ôò´«µÝ¶¨Öµ0¡£
//Èç¹ûÌØ±ðÖ¸Ã÷·µ»ØÖµº¬Òå£¬Ôò³É¹¦»ñÈ¡Êý¾Ý·µ»Ø1£¬Î´³É¹¦·µ»Ø0¡£
enum GAME_SCENE_MAP_OPERATION_INDEX
{
	GSMOI_SCENE_TIME_INFO,			//µ±Ç°Ö÷½ÇËù´¦µÄµØÓòÊ±¼ä»·¾³
	GSMOI_SCENE_TIME_INFO_OFTEN,
	//uParam = (KUiSceneTimeInfo*)pInfo

	GSMOI_SCENE_MAP_INFO,				//µ±Ç°Ö÷½ÇËù´¦µÄ³¡¾°µÄµØÍ¼ÐÅÏ¢
	//uParam = (KSceneMapInfo*) pInfo ÓÃÓÚ»ñÈ¡ÐÅÏ¢µÄ½á¹¹»º³åÇøµÄÖ¸Õë
	//Return = (int)(bool)bHaveMap ·µ»ØÖµ±íÊ¾µ±Ç°³¡¾°ÊÇ·ñÓÐÐ¡µØÍ¼¡£Èç¹û·µ»Ø0ÖµÊ±£¬ pInfoÄÚ·µ»ØµÄÖµÎÞÒâÒå

	GSMOI_IS_SCENE_MAP_SHOWING,	//ÉèÖÃ³¡¾°µÄÐ¡µØÍ¼ÊÇ·ñÏÔÊ¾µÄ×´Ì¬
	//uParam = uShowElem,		//ÏÔÊ¾ÄÄÐ©ÄÚÈÝ£¬È¡ÖµÎªSCENE_PLACE_MAP_ELEMÃ¶¾ÙµÄÒ»¸ö»ò¶à¸öµÄ×éºÏ¡£
				//SCENE_PLACE_MAP_ELEMÔÚGameDataDef.hÖÐ¶¨Òå
				//ä¯ÀÀÐ¡µØÍ¼ÓëÆäËüÒ»Ð©ÏîÊÇ»¥³âµÄ
	//nParam = µÍ16Î»±íÊ¾ÏÔÊ¾µÄ¿í¶È£¬¸ß16Î»±íÊ¾ÏÔÊ¾µÄ¸ß¶È£¨µ¥Î»£ºÏñËØµã£©

	GSMOI_PAINT_SCENE_MAP,		//»æÖÆ³¡¾°µÄÐ¡µØÍ¼
	//uParam = (int)h ±íÊ¾»æÖÆÆðÊ¼µãÔÚÆÁÄ»ÉÏºá×ø±ê×ø±ê£¨µ¥Î»£ºÏñËØµã£©
	//nParam = (int)v ±íÊ¾»æÖÆÆðÊ¼µãÔÚÆÁÄ»ÉÏ×Ý×ø±ê×ø±ê£¨µ¥Î»£ºÏñËØµã£©

	GSMOI_SCENE_MAP_FOCUS_OFFSET,//ÉèÖÃÐ¡µØÍ¼µÄ½¹µã£¨/ÖÐÐÄ£©
	//uParam = (int)nOffsetH	ÉèÖÃÐ¡µØÍ¼½¹µãµÄË®Æ½×ø±ê£¨µ¥Î»£º³¡¾°×ø±ê£©
	//nParam = (int)nOffsetV	ÉèÖÃÐ¡µØÍ¼½¹µãµÄ´¹Öµ×ø±ê£¨µ¥Î»£º³¡¾°×ø±ê£©

	GSMOI_SCENE_FOLLOW_WITH_MAP,	//ÉèÖÃ³¡¾°ÊÇ·ñËæ×ÅµØÍ¼µÄÒÆ¶¯¶øÒÆ¶¯
	//nParam = (int)nbEnable ³¡¾°ÊÇ·ñËæ×ÅµØÍ¼µÄÒÆ¶¯¶øÒÆ¶¯

	GSMOI_IS_SCENE_DIRECT_MAP,

	GSMOI_IS_SCENE_DO_DIRECT_MAP,
};

//=========================================================
// CoreÍâ²¿¿Í»§¶ÔcoreµÄ°ï»áÏà¹ØµÄ²Ù×÷ÇëÇóµÄË÷Òý¶¨Òå
//=========================================================
//¸÷Êý¾ÝÏîË÷ÒýµÄÏà¹Ø²ÎÊýuParamÓënParamÈç¹ûÔÚ×¢ÊÍÖÐÎ´Ìá¼°£¬Ôò´«µÝ¶¨Öµ0¡£
//Èç¹ûÌØ±ðÖ¸Ã÷·µ»ØÖµº¬Òå£¬Ôò³É¹¦»ñÈ¡Êý¾Ý·µ»Ø1£¬Î´³É¹¦·µ»Ø0¡£
enum GAME_TONG_OPERATION_INDEX
{
	GTOI_TONG_CREATE,			//´´½¨°ï»á
	//uParam = (const char*) pszTongName °ï»áµÄÃû×Ö
	//nParam = (NPCCAMP)enFaction °ï»áÕóÓª

	GTOI_TONG_IS_RECRUIT,		//²éÑ¯Ä³ÈËµÄÕÐÈË¿ª¹Ø
	//uParam = (KUiPlayerItme*) Òª²éË­
	//Return = (int)(bool)		ÊÇ·ñ¿ª×ÅµÄÕÐÈË¿ª¹Ø

	GTOI_TONG_RECRUIT,          //ÕÐÈË¿ª¹Ø
	//uParam = (int)(bool)bRecruit ÊÇ·ñÔ¸ÒâÕÐÈË

	GTOI_TONG_ACTION,           //¶Ô°ïÄÚ³ÉÔ±×öµÄ¶¯×÷£¬»ò×Ô¼ºÓë°ï»áµÄ¹ØÏµµÄ¸Ä±ä
	//uParam = (KTongOperationParam*) pOperParam ¶¯×÷Ê±µÄ²ÎÊý
	//nParam = (KTongMemberItem*) pMember Ö¸³öÁË²Ù×÷£¨°ï»á³ÉÔ±£©¶ÔÏó£¬

	GTOI_TONG_JOIN_REPLY,       //¶ÔÉêÇë¼ÓÈëµÄ´ð¸´
	//uPAram = (KUiPlayerItem *) pTarget   ÉêÇë·½
	//nParam : !=0Í¬Òâ     ==0¾Ü¾ø

	GTOI_REQUEST_PLAYER_TONG,	//²éÑ¯Ä³ÈËÊÇÄÄ°ï»á
	//uParam = (KUiPlayerItem*) Òª²éË­
	//nParam = (int)(bool)bReturnTongDetail ÊÇ·ñÒª·µ»ØÄÇ¸ö°ï»áµÄÐÅÏ¢

	GTOI_REQUEST_TONG_DATA,     //ÒªÇóÄ³¸ö°ï»áµÄ¸÷ÖÖ×ÊÁÏ
	//uParam = (KUiGameObjectWithName*)pTong Òª²éÑ¯µÄ°ï»á
			//KUiGameObjectWithName::szName °ï»áµÄÃû×Ö
			//KUiGameObjectWithName::nData ×ÊÁÏµÄÖÖÀà£¬ÖµÈ¡×ÔÃ¶¾ÙTONG_MEMBER_FIGURE
			//			ÁÐ±íµÄÖÖÀàÊÇenumTONG_FIGURE_MASTERµÄ»°´ú±íÒªÇóµÄÊÇ°ï»áµÄ×ÊÑ¶¡£
			//KUiGameObjectWithName::nParam ¿ªÊ¼µÄË÷Òý
	GTOI_TONG_MONEY_ACTION,

	GTOI_TONG_GET_RECRUIT,
};

//=========================================================
// CoreÍâ²¿¿Í»§¶ÔcoreµÄ×é¶ÓÏà¹ØµÄ²Ù×÷ÇëÇóµÄË÷Òý¶¨Òå
//=========================================================
//¸÷Êý¾ÝÏîË÷ÒýµÄÏà¹Ø²ÎÊýuParamÓënParamÈç¹ûÔÚ×¢ÊÍÖÐÎ´Ìá¼°£¬Ôò´«µÝ¶¨Öµ0¡£
//Èç¹ûÌØ±ðÖ¸Ã÷·µ»ØÖµº¬Òå£¬Ôò³É¹¦»ñÈ¡Êý¾Ý·µ»Ø1£¬Î´³É¹¦·µ»Ø0¡£
enum GAME_TEAM_OPERATION_INDEX
{
	TEAM_OI_GD_INFO,				//Ö÷½ÇËùÔÚµÄ¶ÓÎéÐÅÏ¢
	//uParam = (KUiPlayerTeam*)pTeam -> ¶ÓÎéÐÅÏ¢
	//Return = bInTeam, Èç¹ûÎª·Ç0Öµ±íÊ¾Ö÷½ÇÔÚ¶ÓÎéÖÐ£¬pTeam½á¹¹ÊÇ·ñ±»Ìî³äÐÅÏ¢¡£
	//					Èç¹ûÎª0Öµ±íÊ¾Ö÷½Ç²»ÔÚ¶ÓÎéÖÐ£¬pTeam½á¹¹Î´±»Ìî³äÓÐÐ§ÐÅÏ¢¡£

	TEAM_OI_GD_MEMBER_LIST,		
	//uParam = (KUiPlayerItem*)pList -> ÈËÔ±ÐÅÏ¢ÁÐ±í
	//			KUiPlayerItem::nData = (int)(bool)bCaptain ÊÇ·ñÊÇ¶Ó³¤
	//nParam = pListÊý×éÖÐ°üº¬KUiPlayerItem½á¹¹µÄÊýÄ¿
	//Return = Èç¹û·µ»ØÖµÐ¡ÓÚµÈÓÚ´«Èë²ÎÊýnParam£¬ÆäÖµ±íÊ¾pListÊý×éÖÐµÄÇ°¶àÉÙ¸öKUiPlayerItem
	//			½á¹¹±»Ìî³äÁËÓÐÐ§µÄÊý¾Ý£»·ñÔò±íÊ¾ÐèÒª´«Èë°üº¬¶àÉÙ¸öKUiPlayerItem½á¹¹µÄÊý×é
	//			²Å¹»´æ´¢È«²¿µÄ³ÉÔ±ÐÅÏ¢¡£

	TEAM_OI_GD_REFUSE_INVITE_STATUS,//»ñÈ¡¾Ü¾øÑûÇëµÄ×´Ì¬
	//Return = (int)(bool)bEnableRefuse ÎªÕæÖµ±íÊ¾¾Ü¾ø×´Ì¬ÉúÐ§£¬·ñÔò±íÊ¾²»¾Ü¾ø¡£

	TEAM_OI_COLLECT_NEARBY_LIST,//»ñÈ¡ÖÜÎ§¶ÓÎéµÄÁÐ±í

	TEAM_OI_APPLY,				//ÉêÇë¼ÓÈëËýÈË¶ÓÎé
	//uParam = (KUiTeamItem*)	ÒªÉêÇë¼ÓÈëµÄ¶ÓÎéµÄÐÅÏ¢

	TEAM_OI_CREATE,				//ÐÂ×é¶ÓÎé

	TEAM_OI_APPOINT,			//ÈÎÃü¶Ó³¤£¬Ö»ÓÐ¶Ó³¤µ÷ÓÃ²ÅÓÐÐ§¹û
	//uParam = (KUiPlayerItem*)pPlayer -> ÐÂ¶Ó³¤µÄÐÅÏ¢
	//			KUiPlayerItem::nData = 0

	TEAM_OI_INVITE,			//ÑûÇë±ðÈË¼ÓÈë¶ÓÎé£¬Ö»ÓÐ¶Ó³¤µ÷ÓÃ²ÅÓÐÐ§¹û
	//uParam = (KUiPlayerItem*)pPlayer -> ÒªÑûÇëµÄÈËµÄÐÅÏ¢
	//			KUiPlayerItem::nData = 0

	TEAM_OI_KICK,				//Ìß³ý¶ÓÀïµÄÒ»¸ö¶ÓÔ±£¬Ö»ÓÐ¶Ó³¤µ÷ÓÃ²ÅÓÐÐ§¹û
	//uParam = (KUiPlayerItem*)pPlayer -> ÒªÌß³ýµÄ¶ÓÔ±µÄÐÅÏ¢
	//			KUiPlayerItem::nData = 0

	TEAM_OI_LEAVE,				//Àë¿ª¶ÓÎé

	TEAM_OI_CLOSE,				//¹Ø±Õ×é¶Ó£¬Ö»ÓÐ¶Ó³¤µ÷ÓÃ²ÅÓÐÐ§¹û
	//nParam = (int)(bool)bClose ÎªÕæÖµ±íÊ¾´¦ÓÚ¹Ø±Õ×´Ì¬£¬·ñÔò±íÊ¾²»´¦ÓÚ¹Ø±Õ×´Ì¬

	TEAM_OI_REFUSE_INVITE,		
	//nParam = (int)(bool)bEnableRefuse ÎªÕæÖµ±íÊ¾¾Ü¾ø×´Ì¬ÉúÐ§£¬·ñÔò±íÊ¾²»¾Ü¾ø¡£

	TEAM_OI_APPLY_RESPONSE,			//ÊÇ·ñÅú×¼ËûÈË¼ÓÈë¶ÓÎé£¬Ö»ÓÐ¶Ó³¤µ÷ÓÃ²ÅÓÐÐ§¹û
	//uParam = (KUiPlayerItem*)pPlayer -> ÒªÓûÅú×¼µÄÈËÐÅÏ¢
	//			KUiPlayerItem::nData = 0
	//nParam = (int)(bool)bApprove -> ÊÇ·ñÅú×¼ÁË

	TEAM_OI_INVITE_RESPONSE,	//¶Ô×é¶ÓÑûÇëµÄ»Ø¸´
	//uParam = (KUiPlayerItem*)pTeamLeader ·¢³ö×é¶ÓÑûÇëµÄ¶Ó³¤
	//nParam = (int)(bool)bAccept ÊÇ·ñ½ÓÊÜÑûÇë

	TEAM_OI_GET_NPC_MAP_POS,

};

enum GAME_AUTOPLAY_OPERATION_INDEX //add by phong kiÒu using fkauto
{	
	AUTOPLAY_OI_ACTIVE, //--chiÕn ®Êu--
	AUTOPLAY_OI_PAUSE,
	AUTOPLAY_OI_FIGHT_B,
	AUTOPLAY_OI_FIGHT_V,
	AUTOPLAY_OI_FIGHT_S,
	AUTOPLAY_OI_DISTANCE_B,
	AUTOPLAY_OI_DISTANCE_V,
	AUTOPLAY_OI_DISTANCE_S,
	AUTOPLAY_OI_SELFDEF_B,
	AUTOPLAY_OI_SELFDEF_V,
	AUTOPLAY_OI_SELFDEF_S,
	AUTOPLAY_OI_SUPPORT_SKILL,
	AUTOPLAY_OI_SHORTKEY,
	AUTOPLAY_OI_SHORTCUT_EDIT,
	AUTOPLAY_OI_SHORTCUT,
	AUTOPLAY_OI_USKILL_RIGHT_B,
	AUTOPLAY_OI_NR_DO_SKILL_B,
	AUTOPLAY_OI_HP_B,	//b¬m sl --phôc håi--
	AUTOPLAY_OI_HP_1,
	AUTOPLAY_OI_HP_2,
	AUTOPLAY_OI_HP_3,
	AUTOPLAY_OI_MP_B,	//b¬m nl
	AUTOPLAY_OI_MP_1,
	AUTOPLAY_OI_MP_2,
	AUTOPLAY_OI_MP_3,
	AUTOPLAY_OI_TP_1,	//tho dia phu
	AUTOPLAY_OI_TP_2,
	AUTOPLAY_OI_TP_3,
	AUTOPLAY_OI_TP_4,
	AUTOPLAY_OI_TP_5,	//het khoang trong
	AUTOPLAY_OI_TP_6,	//tien nhieu
	AUTOPLAY_OI_TP_7,	//do hu < 5
	AUTOPLAY_OI_USEM_FCELL,
	AUTOPLAY_OI_ANTI_TOXIC,
	AUTOPLAY_OI_UX2ITEM,
	AUTOPLAY_OI_UX2SKILL,
	AUTOPLAY_OI_NMBUFF,
	AUTOPLAY_OI_BUFF_TEAM,
	AUTOPLAY_OI_OPENBAGHP,
	AUTOPLAY_OI_RING_TDP,
	AUTOPLAY_OI_OBJ_1,//--nhÆt ®å ---
	AUTOPLAY_OI_OBJ_2,
	AUTOPLAY_OI_OBJ_3,
	AUTOPLAY_OI_OBJ_4,
	AUTOPLAY_OI_OBJ_5,
	AUTOPLAY_OI_OBJ_6, 
	AUTOPLAY_OI_OBJ_7,//gi÷ trang søc
	AUTOPLAY_OI_OBJ_8,
	AUTOPLAY_OI_OBJ_9,
	AUTOPLAY_OI_OBJ_10,
	AUTOPLAY_OI_OBJ_11,
	AUTOPLAY_OI_OBJ_12,
	AUTOPLAY_OI_OBJ_13,
	AUTOPLAY_OI_MOVE_1,//--di chuyÓn--
	AUTOPLAY_OI_MOVE_2,
	AUTOPLAY_OI_MOVE_3,
	AUTOPLAY_OI_MOVE_4,
	AUTOPLAY_OI_MOVE_5,
	AUTOPLAY_OI_MOVE_6,
	AUTOPLAY_OI_MOVE_7,
	AUTOPLAY_OI_MOVE_8,
	AUTOPLAY_OI_MOVE_9,
	AUTOPLAY_OI_MOVE_10,
	AUTOPLAY_OI_MOVE_11,
	AUTOPLAY_OI_MOVE_12,
	AUTOPLAY_OI_MAP_1,//--b¶n ®å--
	AUTOPLAY_OI_MAP_2,
	AUTOPLAY_OI_MAP_3,
	AUTOPLAY_OI_MAP_4,
	AUTOPLAY_OI_MAP_5,
	AUTOPLAY_OI_MAP_6,
	AUTOPLAY_OI_MAP_7,
	AUTOPLAY_OI_MAP_8,
	AUTOPLAY_OI_MAP_9,
	AUTOPLAY_OI_MAP_10,
	AUTOPLAY_OI_MAP_11,
	AUTOPLAY_OI_MAP_12,
	AUTOPLAY_OI_MAP_13,
	AUTOPLAY_OI_MAP_14,
	AUTOPLAY_OI_MAP_15,
	AUTOPLAY_OI_MAP_16,
	AUTOPLAY_OI_MAP_17,
	AUTOPLAY_OI_TEAM_1,//--tæ ®éi--
	AUTOPLAY_OI_TEAM_2,
	AUTOPLAY_OI_TEAM_3,
	AUTOPLAY_OI_TEAM_4,
	AUTOPLAY_OI_TEAM_5,
	AUTOPLAY_OI_TEAM_6,
	AUTOPLAY_OI_TEAM_7,
	AUTOPLAY_OI_TEAM_8,
	AUTOPLAY_OI_TEAM_9,
};

struct IClientCallback
{
	virtual void CoreDataChanged(unsigned int uDataId, unsigned int uParam, int nParam) = 0;
	virtual void ChannelMessageArrival(DWORD nChannelID, char* szSendName, const char* pMsgBuff, unsigned short nMsgLength, bool bSucc) = 0;
	virtual void MSNMessageArrival(char* szSourceName, char* szSendName, const char* pMsgBuff, unsigned short nMsgLength, bool bSucc) = 0;
	virtual void NotifyChannelID(char* ChannelName, DWORD channelid, BYTE cost) = 0;
	virtual void FriendInvite(char* roleName) = 0;
	virtual void AddFriend(char* roleName, BYTE answer) = 0;
	virtual void FriendStatus(char* roleName, BYTE state) = 0;
	virtual void FriendInfo(char* roleName, char* unitName, char* groupname, BYTE state) = 0;
	virtual void AddPeople(char* unitName, char* roleName) = 0;
};

struct _declspec (novtable) iCoreShell
{
	virtual	int	 GetProtocolSize(BYTE byProtocol) = 0;
	virtual int	 Debug(unsigned int uDataId, unsigned int uParam, int nParam) = 0;
	virtual int	 OperationRequest(unsigned int uOper, unsigned int uParam, int nParam) = 0;
	virtual void ProcessInput(unsigned int uMsg, unsigned int uParam, int nParam) = 0;
	virtual int FindSelectNPC(int x, int y, int nRelation, bool bSelect, void* pReturn, int& nKind) = 0;
	virtual int FindSpecialNPC(char* Name, void* pReturn, int& nKind) = 0;
	virtual int FindSelectObject(int x, int y, bool bSelect, int& nObjectIdx, int& nKind) = 0;
	virtual int ChatSpecialPlayer(void* pPlayer, const char* pMsgBuff, unsigned short nMsgLength) = 0;
	virtual void ApplyAddTeam(void* pPlayer) = 0;
	virtual void TradeApplyStart(void* pPlayer) = 0;
	virtual int UseSkill(int x, int y, int nSkillID) = 0;
	virtual int UseSkillCastB(int x, int y, int nSkillID, int nNpcIdx) = 0;
	virtual int LockSomeoneUseSkill(int nTargetIndex, int nSkillID) = 0;
	virtual int LockSomeoneAction(int nTargetIndex) = 0;
	virtual int LockObjectAction(int nTargetIndex) = 0;
	virtual void GotoWhere(int x, int y, int mode) = 0;	//mode 0 is auto, 1 is walk, 2 is run
	virtual void Goto(int nDir, int mode) = 0;	//nDir 0~63, mode 0 is auto, 1 is walk, 2 is run
	virtual void Turn(int nDir) = 0;	//nDir 0 is left, 1 is right, 2 is back
	virtual int ThrowAwayItem() = 0;
	virtual int GetNPCRelation(int nIndex) = 0;
	virtual	int GetNPCBAITAN(int nIndex) = 0;
	virtual	int GetNPCBAITAN2() = 0;
	virtual int GetGenreItem2(unsigned int UId) = 0;
	virtual int GetPriceSell2(unsigned int UId) = 0;
	virtual int GetTypeItem(unsigned int uItemId) = 0;//edit by phong kieu load them cot va phan loai item
	virtual	int GetStallState() = 0;
	virtual int GetPriceSell(unsigned int uId ) = 0;
	virtual int GetNatureItem(unsigned int uItemId, unsigned int uGenre = 0) = 0;
	virtual int GetGenreItem(unsigned int uItemId, unsigned int uGenre = 0) = 0;
	//=====================
	virtual int	SceneMapOperation(unsigned int uOper, unsigned int uParam, int nParam) = 0;
	virtual int	TongOperation(unsigned int uOper, unsigned int uParam, int nParam) = 0;
	virtual int TeamOperation(unsigned int uOper, unsigned int uParam, int nParam) = 0;
	virtual int	 GetGameData(unsigned int uDataId, unsigned int uParam, int nParam) = 0;
	virtual void DrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam) = 0;
	virtual void DrawGameSpace() = 0;
	virtual DWORD GetPing() = 0;
	//virtual void SendPing() = 0;
	virtual int	 SetCallDataChangedNofify(IClientCallback* pNotifyFunc) = 0;
	virtual void NetMsgCallbackFunc(void* pMsgData) = 0;
	virtual void SetRepresentShell(struct iRepresentShell* pRepresent) = 0;
	virtual void SetMusicInterface(void* pMusicInterface) = 0;
	virtual void SetRepresentAreaSize(int nWidth, int nHeight) = 0;
	virtual int  Breathe() = 0;
	virtual void Release() = 0;
	virtual void SetClient(LPVOID pClient) = 0;
	virtual void SendNewDataToServer(void* pData, int nLength) = 0;	
	virtual int GetOwnValue(int nMoneyUnit) = 0;
	virtual int GetDataSuperShop(int nSaleId, unsigned int uParam, int nParam) = 0;
	virtual int GetObjAtCountRegionInSuperShop(int nSaleId, unsigned int uParam, int nParam) = 0;
	virtual int GetDataDynamicShop(int nSaleId, unsigned int uParam, int nParam) = 0;
	virtual	int GetItemIdxNpcShop(char* szItemName) = 0; //fkauto
	virtual BOOL GetAutoFlag() = 0;
	virtual BOOL GetFightFlag() = 0;
	virtual void SetActiveAutoPlay(BOOL nActive = FALSE) = 0;
	virtual	void SetMoveMap(int nType, int nPos, int nValue) = 0;
	virtual	void SetSortItem(int nType, int nPos, int nValue) = 0;
	virtual	void FkAutoSetFillterMagic(int nType, int nPos, int nValue) = 0;
	virtual int FindSkillInfo(int nType, int nIndex) = 0;
	virtual	void GetSkillName(int nSkillId, char* szSkillName) = 0;
	virtual	BOOL GetSkillData(int nSkillId, int *nLevel) = 0;
	virtual	BOOL GetFlagMode() = 0;
	virtual	void SetFlagMode(bool nIndex) = 0;
	virtual void DirectFindPos(unsigned int uParam, int nParam, BOOL bSync, BOOL bPaintLine) = 0;
	virtual	BYTE GetPaintMode() = 0;
	virtual	void SetPaintMode(BYTE nIndex) = 0;
	virtual int AutoPlayOperation(unsigned int uOper, unsigned int uParam, int nParam) = 0;//fkauto
	virtual BOOL AutoMove() = 0;
	virtual void ClearPathFinder() = 0;
};

#ifndef CORE_EXPORTS

	extern "C" iCoreShell* CoreGetShell();

#else

	void	CoreDataChanged(unsigned int uDataId, unsigned int uParam, int nParam);

#endif

