
/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong Ki襲
//	CreateTime:	2020-9-12
*****************************************************************************************/

#pragma once
enum AUTOOPERATION_INDEX
{
	ATYPE_PUMPLIFE = 0,
	ATYPE_PUMPMANA,
	ATYPE_TP_CHECKLIFE,
	ATYPE_TP_CHECKMANA,
	ATYPE_TP_LIFEGONE,
	ATYPE_TP_MANAGONE,
	ATYPE_TP_FULLITEM,
	ATYPE_TP_FULLMONEY,
	ATYPE_TP_DMGITEM,
	ATYPE_CLEAR,
	ATYPE_CHECKTIME,
	ATYPE_TP_EXIT,
	ATYPE_DISEXIT,
	ATYPE_CANCHAT,
	ATYPE_EATLIFEFULL,
	ATYPE_EATPOISON,
	ATYPE_EATEXPX2,
	ATYPE_EATSKILLX2,
	ATYPE_BASEBUFF,
	ATYPE_OPENBAG,
	ATYPE_CLBUFF,
	ATYPE_SUPPORTBUFF,
	ATYPE_LEFTSKILL,
	ATYPE_RIGHTSKILL,
	ATYPE_CHANGEAURA,
	ATYPE_FIGHT,
	ATYPE_RESETNPCID,
	ATYPE_PKFIGHT,
	ATYPE_ISFIGHTMODE,
	ATYPE_DRAWVISION,
	ATYPE_PICKUPSET,
	ATYPE_GETITEMNAME,
	ATYPE_PICKUP,
	ATYPE_FILTER,
	ATYPE_ARRANGEITEM,
	ATYPE_ARRANGEBOX,
	ATYPE_GETAROUNDNAME,
	ATYPE_PTPROC,
	ATYPE_PTINVITE,
	ATYPE_PTJOIN,
	ATYPE_REPAIRF,
	ATYPE_RETURN,
	ATYPE_RESETMOVE,
	ATYPE_MOVE,
	ATYPE_SETSELSV1,
	ATYPE_SETSELSV2,
	ATYPE_SETACC,
	ATYPE_SETPASS,
	ATYPE_DATAU,
};

enum GAMEDATA_INDEX
{
	GDI_GAME_OBJ_DESC = 1,		//游戏对象描述说明文本串
	//uParam = (KUiObjAtContRegion*) 描述游戏对象的结构数据的指针，其中KUiObjAtContRegion::Region的数据固定为0，无意义。
	//nParam = (KGameObjDesc*) 所指的结构空间用于保存取得的描述说明。
	
	GDI_GAME_OBJ_DESC_INCLUDE_TRADEINFO,	//游戏对象描述说明文本串(包含交易相关信息)
	//参数含义同GDI_GAME_OBJ_DESC

	GDI_GAME_OBJ_DESC_INCLUDE_REPAIRINFO,	//游戏对象描述说明文本串(包含修理相关信息)
	//参数含义同GDI_GAME_OBJ_DESC
	
	GDI_GAME_OBJ_LIGHT_PROP,	//对象的光源属性数据
	//uParam = (KUiGameObject*) 描述游戏对象的结构数据的指针
	//nParam = to be def

	GDI_PLAYER_BASE_INFO,
	//uParam = (KUiPlayerBaseInfo*)pInfo

	GDI_PLAYER_RT_INFO,			//主角的一些易变的数据
	//uParam = (KUiPlayerRuntimeInfo*)pInfo

	GDI_PLAYER_RT_ATTRIBUTE,	//主角的一些易变的属性数据
	//uParam = (KUiPlayerAttribute*)pInfo

	GDI_PLAYER_IMMED_ITEMSKILL,
	//uParam = (KUiPlayerImmedItemSkill*)pInfo

	GDI_PLAYER_HOLD_MONEY,
	//nRet = 主角随身携带的钱

	GDI_PLAYER_IS_MALE,			//主角是否男性
	//nRet = (int)(bool)bMale	是否男性

	// from now on, flying add this item, get the information whether
	// a player can ride a horse.
	GDI_GET_PLAYERNPC_INDEX,		//GDI_PLAYER_CAN_RIDE
	//nRet = (int)bCanRide		是否可以
	//0 - 不可以骑马哦
	//1 - 可以哦

	GDI_ITEM_TAKEN_WITH,		
	//uParam = (KUiObjAtRegion*) pInfo -> KUiObjAtRegion结构数组的指针，KUiObjAtRegion
	//				结构用于存储物品的数据及其放置区域位置信息。
	//nParam = pInfo数组中包含KUiObjAtRegion结构的数目
	//Return = 如果返回值小于等于传入参数nParam，其值表示pInfo数组中的前多少个KUiObjAtRegion
	//			结构被填充了有效的数据；否则表示需要传入包含多少个KUiObjAtRegion结构的数组
	//			才够存储全部的随身携带的物品信息。

	GDI_ITEM_IN_STORE_BOX,		//储物箱里的物品
	//参数及返回值含义同GDI_ITEM_TAKEN_WITH的

	GDI_EQUIPMENT,				//主角装备物品
	//uParam = (KUiObjAtRegion*)pInfo -> 包含11个元素的KUiObjAtRegion结构数组指针，
	//				KUiObjAtRegion结构用于存储装备的数据和放置位置信息。
	//			KUiObjAtRegion::Region::h = 0
	//			KUiObjAtRegion::Region::v 表示属于哪个位置的装备,其值为梅举类型
	//			UI_EQUIPMENT_POSITION的取值之一。请参看UI_EQUIPMENT_POSITION的注释。
	//Return =  其值表示pInfo数组中的前多少个KUiObjAtRegion结构被填充了有效的数据。

	GDI_EQUIPMENT_SETNUM,

	GDI_TRADE_NPC_ITEM,			//npc列出来交易的物品
	//uParam = (KUiObjAtContRegion*) pInfo -> KUiObjAtContRegion结构数组的指针，KUiObjAtContRegion
	//				结构用于存储物品的数据及其放置区域位置信息。
	//				其中KUiObjAtContRegion::nContainer值表示第几页的物品
	//nParam = pInfo数组中包含KUiObjAtContRegion结构的数目
	//Return = 如果返回值小于等于传入参数nParam，其值表示pInfo数组中的前多少个KUiObjAtContRegion
	//			结构被填充了有效的数据；否则表示需要传入包含多少个KUiObjAtContRegion结构的数组
	//			才够存储全部的npc列出来交易的物品信息。

	GDI_TRADE_ITEM_PRICE,		//交易物品的价格
	//uParam = (KUiObjAtContRegion*) pItemInfo -> 用于指出是哪处的哪个物品
	//nParam = (KUiItemBuySelInfo*) pPriceInfo -> 用于接收物品名称交易价格等信息
	//Return = (int)(bool) 可否交易

	GDI_REPAIR_ITEM_PRICE,		//修理物品的价格
	//uParam = (KUiObjAtContRegion*) pItemInfo -> 用于指出是哪处的哪个物品
	//nParam = (KUiItemBuySelInfo*) pPriceInfo -> 用于接收物品名称修理费用等信息
	//Return = (int)(bool) 可否修理

	GDI_TRADE_OPER_DATA,		//交易操作相关的数据
	//uParam = (UI_TRADE_OPER_DATA)eOper 具体含义见UI_TRADE_OPER_DATA
	//nParam 具体应用与含义由uParam的取值状况决定,见UI_TRADE_OPER_DATA的说明
	//Return 具体含义由uParam的取值状况决定,见UI_TRADE_OPER_DATA的说明

	GDI_LIVE_SKILL_BASE,		
	//uParam = (KUiPlayerLiveSkillBase*) pInfo -> 主角的生活技能数据

	GDI_LIVE_SKILLS,			
	//uParam = (KUiSkillData*) pSkills -> 包含10个KUiSkillData的数组用于存储各项生活技能的的数据。

	GDI_FIGHT_SKILL_POINT,		
	//Return = 剩余战斗技能点数

	GDI_FIGHT_SKILLS,			
	//uParam = (KUiSkillData*) pSkills -> 包含50个KUiSkillData的数组用于存储各项战斗的数据。

	GDI_CUSTOM_SKILLS,			
	//uParam = (KUiSkillData*) pSkills -> 包含5个KUiSkillData的数组用于存储各项自创武功的数据。

	GDI_LEFT_ENABLE_SKILLS,		
	//uParam = (KUiSkillData*) pSkills -> 包含65个KUiSkillData的数组用于存储各技能的数据。
	//								KUiSkillData::nLevel用来表述技能显示在第几行
	//Return = 返回有效数据的Skills的数目

	GDI_RIGHT_ENABLE_SKILLS,	
	//uParam = (KUiSkillData*) pSkills -> 包含65个KUiSkillData的数组用于存储各技能的数据。
	//								KUiSkillData::nLevel用来表述技能显示在第几行
	//Return = 返回有效数据的Skills的数目

	GDI_NEARBY_PLAYER_LIST,
	//uParam = (KUiPlayerItem*)pList -> 人员信息列表
	//			KUiPlayerItem::nData = 0
	//nParam = pList数组中包含KUiPlayerItem结构的数目
	//Return = 如果返回值小于等于传入参数nParam，其值表示pList数组中的前多少个KUiPlayerItem
	//			结构被填充了有效的数据；否则表示需要传入包含多少个KUiPlayerItem结构的数组
	//			才够存储全部人员信息。

	GDI_NEARBY_IDLE_PLAYER_LIST,//获取周围孤单可受邀请的玩家的列表

	GDI_NEARBY_NOT_FRIEND_LIST,//获取周围非好友的玩家列表

	GDI_PLAYER_LEADERSHIP,		//主角统帅能力相关的数据
	//uParam = (KUiPlayerLeaderShip*) -> 主角统帅能力相关的数据结构指针

	GDI_ITEM_IN_ENVIRO_PROP,	//获得物品在某个环境位置的属性状态
	//uParam = (KUiGameObject*)pObj（当nParam==0时）物品的信息
	//uParam = (KUiObjAtContRegion*)pObj（当nParam!=0时）物品的信息
	//			此时KUiObjAtContRegion::Region的数据固定为0，无意义。
	//nParam = (int)(bool)bJustTry  是否只是尝试放置
	//Return = (ITEM_IN_ENVIRO_PROP)eProp 物品的属性状态
	
	GDI_CHAT_SEND_CHANNEL_LIST,		//玩家可以发送消息的聊天频道的列表
	//uParam = (KUiChatChannel*) pList -> KUiChatChannel结构数组的指针
	//nParam = pList数组中包含KUiChatChannel结构的数目
	//Return = 如果返回值小于等于传入参数nParam，其值表示pInfo数组中的前多少个KUiChatChannel
	//			结构被填充了有效的数据；否则表示需要传入包含多少个KUiChatChannel结构的数组
	//			才够存储全部的指定频道信息。

	GDI_CHAT_RECEIVE_CHANNEL_LIST,	//玩家可以订阅的消息频道的列表
	//参数及返回值含义同GDI_CHAT_SEND_CHANNEL_LIST

	GDI_CHAT_CURRENT_SEND_CHANNEL,	//获取当前发送消息的频道的信息
	//uParam = (KUiChatChannel*) pChannelInfo 指向用于填充频道的信息的结构空间

	GDI_CHAT_GROUP_INFO,			//聊天的好友分组信息
	//从nParam给定的索引开始查找第一个有效的分组，返回该分组的信息与分组索引。
	//uParam = (KUiChatGroupInfo*) pGroupInfo 分组信息
	//nParam = nIndex 欲获取的分组的索引
	//Return = 实际返回数据的分组的索引，如果未获得则返回-1

	GDI_CHAT_FRIENDS_IN_AGROUP,		//聊天一个好友分组中好友的信息
	//uParam = (KUiPlayerItem*)pList -> 人员信息列表
	//			KUiPlayerItem::nData = (CHAT_STATUS)eFriendStatus 好友的当前状态
	//nParam = 要获取列表的好友分组的索引
	//Return = 其值表示pList数组中的前多少个KUiPlayerItem结构被填充了有效的数据.

	GDI_PK_SETTING,					//获取pk设置
	//Return = (int)(bool)bEnable	是否允许pk

	GDI_SHOW_PLAYERS_NAME,			//获取显示各玩家人名
	//Return = (int)(bool)bShow	是否显示
	GDI_SHOW_PLAYERS_LIFE,			//获取显示各玩家生命
	//Return = (int)(bool)bShow	是否显示
	GDI_SHOW_PLAYERS_MANA,			//获取显示各玩家内力
	//Return = (int)(bool)bShow	是否显示
	
	GDI_PARADE_EQUIPMENT,				//看玩家装备物品,消息含义同GDI_EQUIPMENT
	
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

	GDI_PLAYER_HOLD_FKCOIN,		//add by phong ki襲 s?xu ngi tr猲 player

	NPC_OI_TARGET_INFO, //get target info for showing in client

	GDI_PLAYER_MERIDIAN,			//get Meridian info, return meridian level array

	GDI_ITEM_EQUIP_SAME_GERNE,  //get item with same gerne with id in equip

	GDI_GAMBLE_OPER_DATA, //OTT Data related to trading operations
	//uParam = (UI_GAMBLE_OPER_DATA)eOper For specific meaning, see UI_TRADE_OPER_DATA
	//nParam The specific application and meaning are determined by the value of uParam, see the description of UI_TRADE_OPER_DATA
	//Return The specific meaning is determined by the value of uParam, see the description of UI_TRADE_OPER_DATA
};

enum GAMEDATA_CHANGED_NOTIFY_INDEX
{
	GDCNI_HOLD_OBJECT = 1,
	//uParam = (KUiGameObject*)pObject -> 
	GDCNI_PLAYER_BASE_INFO,	//see also GDI_PLAYER_BASE_INFO
	GDCNI_PLAYER_RT_ATTRIBUTE,	// see also GDI_PLAYER_RT_ATTRIBUTE
	GDCNI_PLAYER_IMMED_ITEMSKILL,
	//uParam = (KUiGameObject*)pInfo 物品或者武功的数据
	//nParam = 哪个位置的物品或者武功 当nParam >=0 时候 nParam表示第nParam个立即物品
	//			当nParam = -1 时，表示为左手武功，-2时为右手武功
	GDCNI_PLAYER_BRIEF_PROP,//
	//uParam = (PLAYER_BRIEF_PROP)eProp 变化内容项，取值为枚举PLAYER_BRIEF_PROP的值之一。
	//nParam = 含义依据uParam的具体含义而定
	GDCNI_OBJECT_CHANGED,		//某处物品的新增或者减少
	//uParam = (KUiObjAtContRegion*)pInfo -> 物品数据及其放置区域位置信息
	//          当pInfo->eContainer == UOC_EQUIPTMENT时
	//				KUiObjAtRegion::Region::h 表示属于第几套装备
	//				KUiObjAtRegion::Region::v 表示属于哪个位置的装备,其值为枚举类型
	//				UI_EQUIPMENT_POSITION的取值之一。请参看UI_EQUIPMENT_POSITION的注释。
	//nParam = bAdd -> 0值表示减少这个物品，非0值表示增加这个物品
	GDCNI_CONTAINER_OBJECT_CHANGED,	//整个面板里的东西全部更新
	//uParam = (UIOBJECT_CONTAINER)eContainer;	//面板位置
	GDCNI_LIVE_SKILL_BASE,		//生活技能数值变化

	GDCNI_FIGHT_SKILL_POINT,	//剩余战斗技能点数变化
	//nParam = 新的剩余战斗技能点数
	GDCNI_SKILL_CHANGE,			//新掌握了一个技能/技能升級
	//uParam = (KUiSkillData*)pSkill -> 新掌握的技能
	//nParam = 新技能在同類中的排序位置
	GDCNI_PLAYER_LEADERSHIP,	//主角统帅能力相关的数据发生变化
	//
	GDCNI_TEAM,	
	//uParam = (KUiPlayerTeam*)pTeam -> 队伍信息,可以为NULL,表示玩家脱离了队伍
	GDCNI_TEAM_NEARBY_LIST,		
	//uParam = (KUiTeamItem*)pList 存队伍信息的数组
	//nParam = nCount 队伍的数目
	GDCNI_TRADE_START,			//与玩家交易开始
	//uParam = (KUiPlayerItem*) pPlayer对方的信息
	GDCNI_TRADE_DESIRE_ITEM,	//对方增减想交易的物品
	//uParam = (KUiObjAtRegion*) pObject -> 物品信息，其中坐标信息为在交易界面中的坐标
	//nParam = bAdd -> 0值表示减少，1值表示增加
	//Remark : 如果物品是金钱的话，则KUiObjAtRegion::Obj::uId表示把金钱额调整为这个值，且nParam无意义。
	GDCNI_TRADE_OPER_DATA,		//交易操作相关的(状态)数据发生变化
	//uParam = (const char*) pInfoText 提示的文字，如"对方解除锁定"等
	GDCNI_TRADE_END,			//交易结束
	GDCNI_GAMBLE_START,			//与玩家交易开始
	GDCNI_GAMBLE_RESET,
	GDCNI_GAMBLE_RESULT,
	//uParam = (KUiPlayerItem*) pPlayer对方的信息
	GDCNI_GAMBLE_DESIRE_ITEM,	//对方增减想交易的物品
	//uParam = (KUiObjAtRegion*) pObject -> 物品信息，其中坐标信息为在交易界面中的坐标
	//nParam = bAdd -> 0值表示减少，1值表示增加
	//Remark : 如果物品是金钱的话，则KUiObjAtRegion::Obj::uId表示把金钱额调整为这个值，且nParam无意义。
	GDCNI_GAMBLE_OPER_DATA,		//交易操作相关的(状态)数据发生变化
	//uParam = (const char*) pInfoText 提示的文字，如"对方解除锁定"等
	GDCNI_GAMBLE_END,			//交易结束
	//nParam = (int)(bool)bTraded	是否进行了交易
	GDCNI_NPC_TRADE,			
	//nParam = (bool)bStart	是否交易为交易开始，如果取值非真表示交易（关闭）结束
	GDCNI_NPC_TRADE_ITEM,		
	//通过调用iCoreShell::GetGameData,使参数uDataId为GDI_TRADE_NPC_ITEM，来获取新的物品信息。
	GDCNI_GAME_START,			//进入游戏世界

	GDCNI_GAME_QUIT,			//离开了游戏世界

	GDCNI_QUESTION_CHOOSE,		//问题选择
	//uParam = (KUiQuestionAndAnswer*)pQuestionAndAnswer
	GDCNI_SPEAK_WORDS,			//npc说话内容
	//uParam = (KUiInformationParam*) pWordDataList 指向KUiInformationParam数组
	//nParam = pWordDataList包含KUiInformationParam元素的数目
	GDCNI_INFORMATION,			//新的信息来到（弹出专门的消息框）
	//uParam = (KUiInformationParam*)pInformation	//消息内容
	GDCNI_MSG_ARRIVAL,			//新（聊天内容窗口）消息来到
	//uParam = (cons char*)pMsgBuff 消息内容缓冲区
	//nParam = (KUiMsgParam*)pMsgParam 消息参数
	GDCNI_SEND_CHAT_CHANNEL,	//当前发送消息的频道改变了

	GDCNI_CHAT_GROUP,			//聊天好友分组发生变化

	GDCNI_CHAT_FRIEND,			//聊天好友发生变化
	//nParam = nGroupIndex 发生好友变化的分组的索引
	GDCNI_CHAT_FRIEND_STATUS,	//聊天好友状态发生变化
	//uParam = (KUiPlayerItem*)pFriend 发生状态变化的好友
	//			KUiPlayerItem::nData = (CHAT_STATUS)eFriendStatus 好友的当前状态
	//nParam = nGroupIndex 好友所属于的组的索引
	GDCNI_CHAT_MESSAGE,			//好有发来的讯息
	//uParam = (KUiChatMessage*)pMessage 消息数据
	//nParam = (KUiPlayerItem*)pFriend   发来讯息的好友
	GDCNI_SYSTEM_MESSAGE,		//系统消息
	//uParam = (KSystemMessage*)pMsg 系统消息的内容
	//nParam = (void*)pParamBuf 指向一个参数缓冲区，缓冲内数据含义依据pMsg的内容而定，
	//			参看KSystemMessage的注释说明，缓冲区的大小由pMsg->byParamSize给出。
	GDCNI_NEWS_MESSAGE,			//新闻消息
	//uParam = (KNewsMessage*)pMsg 新闻消息内容
	//nParam = (SYSTEMTIME*)pTime  新闻消息的时间参数，具体含义依赖于新闻消息的类型而定
	GDCNI_SWITCH_CURSOR,		//切换鼠标指针图形
	
	GDCNI_OPEN_STORE_BOX,	
	
	GDCNI_SWITCHING_SCENEPLACE,	//地图切换
	//nParam = (int)(bool)bStart 为非0值表示开始地图切换，为0值表示结束地图切换
	GDCNI_MISSION_RECORD,		//要求纪录下此任务提示信息
	//uParam = (KMissionRecord*) pRecord 纪录内容
	GDCNI_PK_SETTING,			//pk允许状况发生了变化
	//nParam = (int)(bool)bEnable 是否允许pk
	GDCNI_VIEW_PLAYERITEM,			//被申请玩家装备的数据已到
	//uParam = (KUiPlayerItem*)		//玩家的 player data
	GDCNII_RANK_INDEX_LIST_ARRIVE,	//排名项id列表到了
	//uParam = uCount 排名项id的数目
	//nParam = (KRankIndex*)pIndexList 排名项id描述结构的数组
	GDCNII_RANK_INFORMATION_ARRIVE,	//排名项的内容到了
	//uParam = ((unsigned short)usCount) | ((unsigned short)usIndexId << 16)
	//			usCount    此次传来的内容项的数目
	//			usIndexId  排名项id，表示传回的是这个id的排名项的内容
	//nParam = (KRankMessage *) 列表内容
	//====帮派相关====
	GDCNI_TONG_INFO,				//某人所在的帮派的信息
									//对应于GTOI_REQUEST_PLAYER_TONG（查询某人是哪帮会）
	//uParam = (KUiPlayerRelationWithOther*) 那个（某）人,以及此人与此帮派的关系
					//KUiPlayerRelationWithOther::nParam 是否开着的招人开关
	//nParam = (KTongInfo*) pToneInfo	//帮派的信息
	GDCNI_TONG_MEMBER_LIST,			//某个帮派中的成员的列表
	//uParam = (KUiGameObjectWithName*) pTong 用于描述是哪个帮派
							//KUiGameObjectWithName::szName 帮派名称
							//KUiGameObjectWithName::nData  pMemberList所给帮派成员的类型，参看TONG_MEMBER_FIGURE
							//KUiGameObjectWithName::nParam pMemberList所给帮派成员列表包含成员的起始的索引
							//KUiGameObjectWithName::uParam pMemberList所给帮派成员列表包含成员的数目
	//nParam = (KTongMemberItem*) pMemberList
	GDCNI_TONG_ACTION_RESULT,       //帮会操作返回的结果
	//uParam = (KUiGameObjectWithName*) pInfo 返回的数据
	                        //KUiGameObjectWithName::szName 对象玩家的名字
							//KUiGameObjectWithName::nData  操作的种类，参考TONG_ACTION_TYPE
							//KUiGameObjectWithName::nParam   V
							//KUiGameObjectWithName::uParam   V
							//KUiGameObjectWithName::szString 这几个涵义随不同操作不同
	// nParam : 成功 1 失败 0
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

	GDCNI_PLAYER_LOGIN_REPLAY, //fix by phong ki襲 chuy觧 gs b?m蕋 skill

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

	GDCNI_PLAYER_MERIDIAN_SYNC,

	GDCNI_PLAYER_BAUCUA_RESULT_SYNC,
		
	GDCNI_EXIT_GAME,
	
	GDCNI_UI_ACT, //connect to s3client (ext auto)
	GDCNI_TONG_JX2,	// JX2 port: du lieu trang cua so bang hoi
	//uParam = (BYTE*) goi TONG_JX2_*_SYNC nguyen ven (byte dau ProtocolType)
	//nParam = kich thuoc goi
	GDCNI_TASK_VALUE_UPDATE,	// [TaskGuide] uParam = task id, nParam = gia tri
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
	//uParam = (KUiGameObject*)pInfo -> 技能数据
	//nParam = (目前只传0值。)鼠标指针当前坐标（绝对坐标），横坐标在低16位，纵坐标在高16位。(像素点坐标)

	GOI_SET_IMMDIA_SKILL,	//设置立即技能
	//uParam = (KUiGameObject*)pSKill, 技能信息
	//nParam = 立即位置，0表示为左键技能，1表示为右键技能

	GOI_TONE_UP_SKILL,		//增强一种技能，，一次加一点
	//uParam = 技能类属
	//nParam = (uint)技能id

	GOI_TONE_UP_ATTRIBUTE,	//增强一些属性的值，一次加一点
	//uParam = 表示要增强的是哪个属性，取值为UI_PLAYER_ATTRIBUTE的梅举值之一

	//============（与其它玩家）交易相关================
	GOI_TRADE_INVITE_RESPONSE,	//答应/拒绝交易请求
	//uParam = (KUiPlayerItem*)pRequestPlayer 发出请求的玩家
	//nParam = (int)(bool)bAccept 是否接受请求

	GOI_TRADE_DESIRE_ITEM,		//增减一个欲卖出的物品
	//uParam = (KUiObjAtRegion*) pObject -> 物品信息，其中坐标信息为在交易界面中的坐标
	//nParam = bAdd -> 0值表示减少，1值表示增加
	//Remark : 如果物品是金钱的话，则KUiObjAtRegion::Obj::uId表示把金钱额调整为这个值，且nParam无意义。

	GOI_TRADE_WILLING,			//giao dich giao d辌h
	//uParam = (const char*)pszTradMsg 关于交易消息一句话，当bWilling为true时有效
	//nParam = (int)(bool)bWilling 是否期待交易(叫卖)

	GOI_TRADE_LOCK,				//锁定交易
	//nParam = (int)(bool)bLock 是否锁定

	GOI_TRADE,					//交易
	//nParam = (int)(bool)bTrading
	
	GOI_TRADE_CANCEL,			//交易取消

	GOI_GAMBLE_INVITE_RESPONSE,	//答应/拒绝交易请求
	//uParam = (KUiPlayerItem*)pRequestPlayer 发出请求的玩家
	//nParam = (int)(bool)bAccept 是否接受请求

	GOI_GAMBLE_DESIRE_ITEM,		//增减一个欲卖出的物品
	//uParam = (KUiObjAtRegion*) pObject -> 物品信息，其中坐标信息为在交易界面中的坐标
	//nParam = bAdd -> 0值表示减少，1值表示增加
	//Remark : 如果物品是金钱的话，则KUiObjAtRegion::Obj::uId表示把金钱额调整为这个值，且nParam无意义。

	GOI_GAMBLE_WILLING,			//giao dich giao d辌h
	//uParam = (const char*)pszTradMsg 关于交易消息一句话，当bWilling为true时有效
	//nParam = (int)(bool)bWilling 是否期待交易(叫卖)

	GOI_GAMBLE_LOCK,				//锁定交易
	//nParam = (int)(bool)bLock 是否锁定

	GOI_GAMBLE,					//交易
	//nParam = (int)(bool)bTrading

	GOI_GAMBLE_CANCEL,			//交易取消

	//============================
	GOI_TRADE_NPC_BUY,
	//uParam = (KUiGameObject*)pObj -> 物品信息

	GOI_TRADE_NPC_SELL,			
	//uParam = (KUiObjAtContRegion*)pObj -> 物品信息

	GOI_TRADE_NPC_REPAIR,		//修理物品
	//uParam = (KUiObjAtContRegion*) pObj -> 想要的物品的信息

	GOI_TRADE_NPC_CLOSE,		//结束交易

	GOI_AUTOPLAY_ACTION,		//GOI_DROP_ITEM_QUERY
	//uParam = (KUiGameObject*)pObject -> 物品信息
	//nParam = 被拖动东西的当前坐标（绝对坐标），横坐标在低16位，纵坐标在高16位。(像素点坐标)
	//Return = 是否可以放下
	
//	GOI_DROP_ITEM,				//放置物品到游戏窗口
	//参数含义同GOI_DROP_ITEM_QUERY参数含义相同
	//Return = 是否东西被放下了

	GOI_SEND_MSG,				//发送消息
	//uParam = (const char*)pMsgBuff 消息内容缓冲区
	//nParam = (KUiMsgParam*)pMsgParam 消息参数
	
	//============聊天相关================
	GOI_SET_SEND_CHAT_CHANNEL,	//设置当前发送消息的频道
	//uParam = (KUiChatChannel*) pChannelInfo 要设置的频道的信息
	
	GOI_SET_SEND_WHISPER_CHANNEL,//设置当前发送消息的频道为与某人单聊
	//uParam = (KUiPlayerItem*) pFriend

	GOI_SET_RECEIVE_CHAT_CHANNEL,//设置订阅/取消消息频道
	//uParam = (KUiChatChannel*) pChannelInfo 要设置的频道的信息
	//nParam = (int)(bool) bEnable 是否订阅

	GOI_CHAT_GROUP_NEW,			//新建聊天好友组
	//uParam = (const char*) pGroupName
	//Return = (int)(bool) bSuccessed

	GOI_CHAT_GROUP_RENAME,		//重命名聊天好友组
	//uParam = (const char*) pGroupName
	//nParam = nIndex 组索引
	//Return = (int)(bool) bSuccessed

	GOI_CHAT_GROUP_DELETE,		//删除聊天好友组
	//nParam = nIndex 组索引
	//Return = (int)(bool) bSuccessed

	GOI_CHAT_FRIEND_ADD,		//新添加聊天好友
	//uParam = (KUiPlayerItem*)pFriend
	//			KUiPlayerItem::nData = 0

	GOI_CHAT_FRIEND_DELETE,		//删除聊天好友
	//uParam = (KUiPlayerItem*)pFriend
	//			KUiPlayerItem::nData = 0
	//nParam = nGroupIndex 好友所在的组的索引
	//Return = (int)(bool) bSuccessed

	GOI_CHAT_FRIEND_MOVE,		//好友移到新组
	//uParam = (KUiPlayerItem*)pFriend
	//			KUiPlayerItem::nData = 0
	//nParam = nGroupIndex 好友所到的新组的索引
	//Return = (int)(bool) bSuccessed

	GOI_CHAT_SET_STATUS,		//切换聊天状态
	//uParam = (CHAT_STATUS)eStatus 新的聊天状态

	GOI_CHAT_WORDS_TO_FRIEND,	//发给好友一条讯息
	//uParam = (KUiChatMessage*)pMessage 消息数据
	//nParam = (KUiPlayerItem*)pFriend   发来讯息的好友

	GOI_CHAT_FRIEND_INVITE,		//对别人要加自己为好友的回复
	//uParam = (KUiPlayerItem*)pRequestPlayer 发出请求的玩家
	//nParam = (int)(bool)bAccept 是否接受请求

	GOI_OPTION_SETTING,			
	//uParam = (OPTIONS_LIST)eOptionItem 要设置的选项
	//nParam = (int)nValue 设置的值，其含义依赖于eOptionItem的含义
	//					参看OPTIONS_LIST各值的注释

	GOI_PLAY_SOUND,				//播放声音
	//uParam = (const char*)pszFileName

	GOI_PK_SETTING,				//设置PK
	//nParam = (int)(bool)bEnable	是否允许pk

	GOI_REVENGE_SOMEONE,		//仇杀某人
	//uParam = (KUiPlayerItem*) pTarget	仇杀目标

	GOI_SHOW_PLAYERS_NAME,		//显示各玩家人名
	//nParam = (int)(bool)bShow	是否显示
	GOI_SHOW_PLAYERS_LIFE,		//显示各玩家生命
	//nParam = (int)(bool)bShow	是否显示
	GOI_SHOW_PLAYERS_MANA,		
	//nParam = (int)(bool)bShow	是否显示

	GOI_GAMESPACE_DISCONNECTED,	
	
	GOI_VIEW_PLAYERITEM,		
	//uParam = dwNpcID	玩家的m_dwID
	GOI_VIEW_PLAYERITEM_END,	

	GOI_FOLLOW_SOMEONE,			
	//uParam = (KUiPlayerItem*) pTarget	跟随目标

	GOI_QUERY_RANK_INFORMATION,  
	//uParam = usIndexId 排名项的id

	GOI_SHOW_OBJ_NAME, // hien ten obj duoi dat edit by phong kieu

	GOI_CP_UNLOCK,		//open ruong
	
	GOI_CP_LOCK,		//close ruong

	GOI_CP_SWITCH_EQUIPSET,

	GOI_CP_CHANGE,		//doi mk ruong
	
	GOI_CP_RESET,		//reset mk ruong

	GDI_SET_TRADE_ITEM,			// ban hang

	GDI_PLAYER_TRADE,			//摆摊
	
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

	GAUTO_AUTO_PT_PLAYERTEAM,//qu秐 l?t?i

	GAUTO_AUTO_MOVETPSID, //ch箉 to? ra b穒 train
	
	GAUTO_AUTO_MOVETPSX,
	
	GAUTO_AUTO_MOVETPSY,

	GAUTO_AUTO_BLACK_ITEM, //qu秐 l?v藅 ph萴 甧n

	GDI_THROW_ALL_ITEM,

	GOI_EXCHANGEITEM,

	GOI_SUPPERSHOP_TRADE_NPC_BUY,

	GOI_DRAW_TARGET_INFO,

	GOI_SET_PLAYER_MERIDIAN, //Set meridian

	GOI_BAUCUA,
	
	GOI_MASKFEATURE,

	GOI_PROCFRAME_BREATHE,

	GOI_PROCFRAME_POSSHIFT,

	GDCNI_GIVE,

	GDCNI_RANKDATA,

	GDCNI_ENCHASE,

	GDCNI_INPUT,
	
	GDI_TASK_SAVE_VALUE,		// [TaskGuide] uParam = task id -> tra ve ban sao task cua nhan vat
	GDI_PLAYER_REPUTE_VALUE,	// [TaskGuide] danh vong hien tai cua nhan vat
	GOI_TASKGUIDE_GOTO_XAFU,	// [TaskGuide] bam nhiem vu loai 4 -> tu chay den Xa Phu (bam lai = huy)
	GDI_CITY_INFO,				// [CITYINFO 21/08] uParam = thanh 1..7, nParam = KCityInfoView* -> 1 co du lieu / 0
	GDI_CITY_INFO_CURMAP,		// [CITYINFO 21/08] nParam = KCityInfoView* -> id thanh cua map dang dung / 0
	GDCNI_CITY_INFO_UPDATE,		// [CITYINFO 21/08] uParam = id thanh vua nhan tu server

};

// [CITYINFO 21/08] ban sao 7 thanh (KJx2CityWar server -> UI_CITYINFO -> Core client)
struct KCityInfoView
{
	int		nCityId;		// 0 = chua co du lieu
	int		nState;			// JX2CW_STATE_* (0 roi .. 5 cong thanh)
	int		nTax;			// thue giao dich (%)
	int		nPrice;			// chi so vat gia (-1 = chua dat)
	int		nMapId;			// map chinh cua thanh (AreaIncludes dau tien)
	char	szName[32];		// ten thanh (TCVN3)
	char	szOwner[32];	// bang chiem thanh ("" = vo chu)
	char	szMaster[32];	// Thai Thu
};

//=========================================================
// Core外部客户对core的场景地图相关的操作请求的索引定义
//=========================================================
//各数据项索引的相关参数uParam与nParam如果在注释中未提及，则传递定值0。
//如果特别指明返回值含义，则成功获取数据返回1，未成功返回0。
enum GAME_SCENE_MAP_OPERATION_INDEX
{
	GSMOI_SCENE_TIME_INFO,			//当前主角所处的地域时间环境
	GSMOI_SCENE_TIME_INFO_OFTEN,
	//uParam = (KUiSceneTimeInfo*)pInfo

	GSMOI_SCENE_MAP_INFO,				//当前主角所处的场景的地图信息
	//uParam = (KSceneMapInfo*) pInfo 用于获取信息的结构缓冲区的指针
	//Return = (int)(bool)bHaveMap 返回值表示当前场景是否有小地图。如果返回0值时， pInfo内返回的值无意义

	GSMOI_IS_SCENE_MAP_SHOWING,	//设置场景的小地图是否显示的状态
	//uParam = uShowElem,		//显示哪些内容，取值为SCENE_PLACE_MAP_ELEM枚举的一个或多个的组合。
				//SCENE_PLACE_MAP_ELEM在GameDataDef.h中定义
				//浏览小地图与其它一些项是互斥的
	//nParam = 低16位表示显示的宽度，高16位表示显示的高度（单位：像素点）

	GSMOI_PAINT_SCENE_MAP,		//绘制场景的小地图
	//uParam = (int)h 表示绘制起始点在屏幕上横坐标坐标（单位：像素点）
	//nParam = (int)v 表示绘制起始点在屏幕上纵坐标坐标（单位：像素点）

	GSMOI_SCENE_MAP_FOCUS_OFFSET,//设置小地图的焦点（/中心）
	//uParam = (int)nOffsetH	设置小地图焦点的水平坐标（单位：场景坐标）
	//nParam = (int)nOffsetV	设置小地图焦点的垂值坐标（单位：场景坐标）

	GSMOI_SCENE_FOLLOW_WITH_MAP,	//设置场景是否随着地图的移动而移动
	//nParam = (int)nbEnable 场景是否随着地图的移动而移动

	GSMOI_IS_SCENE_DIRECT_MAP,

	GSMOI_IS_SCENE_DO_DIRECT_MAP,
	GSMOI_SCENE_MAP_FLAG_ON_TARGET,
	GSMOI_IS_SCENE_MAP_FLAGIMG,
	GSMOI_SCENE_MAP_REMOVE_FLAG,
	GSMOI_SCENE_MAP_GET_FLAGPOS,
	GSMOI_SCENE_MAP_TG_COORD,
};

//=========================================================
// Core外部客户对core的帮会相关的操作请求的索引定义
//=========================================================
//各数据项索引的相关参数uParam与nParam如果在注释中未提及，则传递定值0。
//如果特别指明返回值含义，则成功获取数据返回1，未成功返回0。
enum GAME_TONG_OPERATION_INDEX
{
	GTOI_TONG_CREATE,			//创建帮会
	//uParam = (const char*) pszTongName 帮会的名字
	//nParam = (NPCCAMP)enFaction 帮会阵营

	GTOI_TONG_IS_RECRUIT,		//查询某人的招人开关
	//uParam = (KUiPlayerItme*) 要查谁
	//Return = (int)(bool)		是否开着的招人开关

	GTOI_TONG_RECRUIT,          //招人开关
	//uParam = (int)(bool)bRecruit 是否愿意招人

	GTOI_TONG_ACTION,           //对帮内成员做的动作，或自己与帮会的关系的改变
	//uParam = (KTongOperationParam*) pOperParam 动作时的参数
	//nParam = (KTongMemberItem*) pMember 指出了操作（帮会成员）对象，

	GTOI_TONG_JOIN_REPLY,       //对申请加入的答复
	//uPAram = (KUiPlayerItem *) pTarget   申请方
	//nParam : !=0同意     ==0拒绝

	GTOI_REQUEST_PLAYER_TONG,	//查询某人是哪帮会
	//uParam = (KUiPlayerItem*) 要查谁
	//nParam = (int)(bool)bReturnTongDetail 是否要返回那个帮会的信息

	GTOI_REQUEST_TONG_DATA,     //要求某个帮会的各种资料
	//uParam = (KUiGameObjectWithName*)pTong 要查询的帮会
			//KUiGameObjectWithName::szName 帮会的名字
			//KUiGameObjectWithName::nData 资料的种类，值取自枚举TONG_MEMBER_FIGURE
			//			列表的种类是enumTONG_FIGURE_MASTER的话代表要求的是帮会的资讯。
			//KUiGameObjectWithName::nParam 开始的索引
	GTOI_TONG_MONEY_ACTION,

	GTOI_TONG_GET_RECRUIT,
	GTOI_TONG_JX2_VIEW,	// JX2 port: uParam = page (4 = trang thong bao dung INFO), nParam = start
	GTOI_TONG_JX2_OP,		// JX2 port: uParam = (KUiTongJX2Op*)
};

// JX2 port: goi thao tac cua so bang hoi JX2 (UI -> Core -> GS)
typedef struct
{
	int	nOp;	// defTONG_JX2_COP_* (KProtocol.h)
	unsigned long	dwTarget;
	int	nParam1;
	int	nParam2;
	char	szText[128];
} KUiTongJX2Op;

//=========================================================
// Core外部客户对core的组队相关的操作请求的索引定义
//=========================================================
//各数据项索引的相关参数uParam与nParam如果在注释中未提及，则传递定值0。
//如果特别指明返回值含义，则成功获取数据返回1，未成功返回0。
enum GAME_TEAM_OPERATION_INDEX
{
	TEAM_OI_GD_INFO,				//主角所在的队伍信息
	//uParam = (KUiPlayerTeam*)pTeam -> 队伍信息
	//Return = bInTeam, 如果为非0值表示主角在队伍中，pTeam结构是否被填充信息。
	//					如果为0值表示主角不在队伍中，pTeam结构未被填充有效信息。

	TEAM_OI_GD_MEMBER_LIST,		
	//uParam = (KUiPlayerItem*)pList -> 人员信息列表
	//			KUiPlayerItem::nData = (int)(bool)bCaptain 是否是队长
	//nParam = pList数组中包含KUiPlayerItem结构的数目
	//Return = 如果返回值小于等于传入参数nParam，其值表示pList数组中的前多少个KUiPlayerItem
	//			结构被填充了有效的数据；否则表示需要传入包含多少个KUiPlayerItem结构的数组
	//			才够存储全部的成员信息。

	TEAM_OI_GD_REFUSE_INVITE_STATUS,//获取拒绝邀请的状态
	//Return = (int)(bool)bEnableRefuse 为真值表示拒绝状态生效，否则表示不拒绝。

	TEAM_OI_COLLECT_NEARBY_LIST,//获取周围队伍的列表

	TEAM_OI_APPLY,				//申请加入她人队伍
	//uParam = (KUiTeamItem*)	要申请加入的队伍的信息

	TEAM_OI_CREATE,				//新组队伍

	TEAM_OI_APPOINT,			//任命队长，只有队长调用才有效果
	//uParam = (KUiPlayerItem*)pPlayer -> 新队长的信息
	//			KUiPlayerItem::nData = 0

	TEAM_OI_INVITE,			//邀请别人加入队伍，只有队长调用才有效果
	//uParam = (KUiPlayerItem*)pPlayer -> 要邀请的人的信息
	//			KUiPlayerItem::nData = 0

	TEAM_OI_KICK,				//踢除队里的一个队员，只有队长调用才有效果
	//uParam = (KUiPlayerItem*)pPlayer -> 要踢除的队员的信息
	//			KUiPlayerItem::nData = 0

	TEAM_OI_LEAVE,				//离开队伍

	TEAM_OI_CLOSE,				//关闭组队，只有队长调用才有效果
	//nParam = (int)(bool)bClose 为真值表示处于关闭状态，否则表示不处于关闭状态

	TEAM_OI_REFUSE_INVITE,		
	//nParam = (int)(bool)bEnableRefuse 为真值表示拒绝状态生效，否则表示不拒绝。

	TEAM_OI_APPLY_RESPONSE,			//是否批准他人加入队伍，只有队长调用才有效果
	//uParam = (KUiPlayerItem*)pPlayer -> 要欲批准的人信息
	//			KUiPlayerItem::nData = 0
	//nParam = (int)(bool)bApprove -> 是否批准了

	TEAM_OI_INVITE_RESPONSE,	//对组队邀请的回复
	//uParam = (KUiPlayerItem*)pTeamLeader 发出组队邀请的队长
	//nParam = (int)(bool)bAccept 是否接受邀请

	TEAM_OI_GET_NPC_MAP_POS,

};

enum GAME_AUTOPLAY_OPERATION_INDEX //add by phong ki襲 using fkauto
{	
	AUTOPLAY_OI_ACTIVE, //--chi課 u--
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
	AUTOPLAY_OI_HP_B,	//b琺 sl --ph鬰 h錳--
	AUTOPLAY_OI_HP_1,
	AUTOPLAY_OI_HP_2,
	AUTOPLAY_OI_HP_3,
	AUTOPLAY_OI_MP_B,	//b琺 nl
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
	AUTOPLAY_OI_OBJ_1,//--nh苩  ---
	AUTOPLAY_OI_OBJ_2,
	AUTOPLAY_OI_OBJ_3,
	AUTOPLAY_OI_OBJ_4,
	AUTOPLAY_OI_OBJ_5,
	AUTOPLAY_OI_OBJ_6, 
	AUTOPLAY_OI_OBJ_7,//gi?trang s鴆
	AUTOPLAY_OI_OBJ_8,
	AUTOPLAY_OI_OBJ_9,
	AUTOPLAY_OI_OBJ_10,
	AUTOPLAY_OI_OBJ_11,
	AUTOPLAY_OI_OBJ_12,
	AUTOPLAY_OI_OBJ_13,
	AUTOPLAY_OI_MOVE_1,//--di chuy觧--
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
	AUTOPLAY_OI_MAP_1,//--b秐 --
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
	AUTOPLAY_OI_TEAM_1,//--t?i--
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
	virtual int CoreDataChanged(unsigned int uDataId, unsigned int uParam, int nParam) = 0;
	virtual void ChannelMessageArrival(DWORD nChannelID, char* szSendName, const char* pMsgBuff, unsigned short nMsgLength, bool bSucc, bool isNpcChat = false, bool isShowMsgPad = false) = 0;
	virtual void MSNMessageArrival(char* szSourceName, char* szSendName, const char* pMsgBuff, unsigned short nMsgLength, bool bSucc) = 0;
	virtual void NotifyChannelID(char* ChannelName, DWORD channelid, BYTE cost) = 0;
	virtual void FriendInvite(char* roleName) = 0;
	virtual void AddFriend(char* roleName, BYTE answer) = 0;
	virtual void FriendStatus(char* roleName, BYTE state) = 0;
	virtual void FriendInfo(char* roleName, char* unitName, char* groupname, BYTE state) = 0;
	virtual void AddPeople(char* unitName, char* roleName) = 0;
	virtual void SendDataToTool(const void * const pData, const size_t &datalength) = 0;
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
	virtual void GambleApplyStart(void* pPlayer) = 0;
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
	//virtual BOOL AutoMove() = 0;
//	virtual void ClearPathFinder() = 0;
	virtual int CheckMapLoiDai() = 0;
};

#ifndef CORE_EXPORTS

	extern "C" iCoreShell* CoreGetShell();

#else

	int	CoreDataChanged(unsigned int uDataId, unsigned int uParam, int nParam);
	void SendDataToTool(const void * const pData, const size_t &datalength);
#endif

