
/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong Ki�u
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
	ATYPE_TONGKIM,
	ATYPE_LIENDAU,
	ATYPE_HOATDONG,
	ATYPE_SATTHU,		// (25/08) may san boss Sat Thu + ghep Sat Thu Gian (ST_Process)
	ATYPE_MAPSUKIEN,	// (25/08) dang o MAP SU KIEN? -> dung cac auto TU DO
};

enum GAMEDATA_INDEX
{
	GDI_GAME_OBJ_DESC = 1,		//��Ϸ��������˵���ı���
	//uParam = (KUiObjAtContRegion*) ������Ϸ����Ľṹ���ݵ�ָ�룬����KUiObjAtContRegion::Region�����ݹ̶�Ϊ0�������塣
	//nParam = (KGameObjDesc*) ��ָ�Ľṹ�ռ����ڱ���ȡ�õ�����˵����
	
	GDI_GAME_OBJ_DESC_INCLUDE_TRADEINFO,	//��Ϸ��������˵���ı���(�������������Ϣ)
	//��������ͬGDI_GAME_OBJ_DESC

	GDI_GAME_OBJ_DESC_INCLUDE_REPAIRINFO,	//��Ϸ��������˵���ı���(�������������Ϣ)
	//��������ͬGDI_GAME_OBJ_DESC
	
	GDI_GAME_OBJ_LIGHT_PROP,	//����Ĺ�Դ��������
	//uParam = (KUiGameObject*) ������Ϸ����Ľṹ���ݵ�ָ��
	//nParam = to be def

	GDI_PLAYER_BASE_INFO,
	//uParam = (KUiPlayerBaseInfo*)pInfo

	GDI_PLAYER_RT_INFO,			//���ǵ�һЩ�ױ������
	//uParam = (KUiPlayerRuntimeInfo*)pInfo

	GDI_PLAYER_RT_ATTRIBUTE,	//���ǵ�һЩ�ױ����������
	//uParam = (KUiPlayerAttribute*)pInfo

	GDI_PLAYER_IMMED_ITEMSKILL,
	//uParam = (KUiPlayerImmedItemSkill*)pInfo

	GDI_PLAYER_HOLD_MONEY,
	//nRet = ��������Я����Ǯ

	GDI_PLAYER_IS_MALE,			//�����Ƿ�����
	//nRet = (int)(bool)bMale	�Ƿ�����

	// from now on, flying add this item, get the information whether
	// a player can ride a horse.
	GDI_GET_PLAYERNPC_INDEX,		//GDI_PLAYER_CAN_RIDE
	//nRet = (int)bCanRide		�Ƿ����
	//0 - ����������Ŷ
	//1 - ����Ŷ

	GDI_ITEM_TAKEN_WITH,		
	//uParam = (KUiObjAtRegion*) pInfo -> KUiObjAtRegion�ṹ�����ָ�룬KUiObjAtRegion
	//				�ṹ���ڴ洢��Ʒ�����ݼ����������λ����Ϣ��
	//nParam = pInfo�����а���KUiObjAtRegion�ṹ����Ŀ
	//Return = �������ֵС�ڵ��ڴ������nParam����ֵ��ʾpInfo�����е�ǰ���ٸ�KUiObjAtRegion
	//			�ṹ���������Ч�����ݣ������ʾ��Ҫ����������ٸ�KUiObjAtRegion�ṹ������
	//			�Ź��洢ȫ��������Я������Ʒ��Ϣ��

	GDI_ITEM_IN_STORE_BOX,		//�����������Ʒ
	//����������ֵ����ͬGDI_ITEM_TAKEN_WITH��

	GDI_EQUIPMENT,				//����װ����Ʒ
	//uParam = (KUiObjAtRegion*)pInfo -> ����11��Ԫ�ص�KUiObjAtRegion�ṹ����ָ�룬
	//				KUiObjAtRegion�ṹ���ڴ洢װ�������ݺͷ���λ����Ϣ��
	//			KUiObjAtRegion::Region::h = 0
	//			KUiObjAtRegion::Region::v ��ʾ�����ĸ�λ�õ�װ��,��ֵΪ÷������
	//			UI_EQUIPMENT_POSITION��ȡֵ֮һ����ο�UI_EQUIPMENT_POSITION��ע�͡�
	//Return =  ��ֵ��ʾpInfo�����е�ǰ���ٸ�KUiObjAtRegion�ṹ���������Ч�����ݡ�

	GDI_EQUIPMENT_SETNUM,

	GDI_TRADE_NPC_ITEM,			//npc�г������׵���Ʒ
	//uParam = (KUiObjAtContRegion*) pInfo -> KUiObjAtContRegion�ṹ�����ָ�룬KUiObjAtContRegion
	//				�ṹ���ڴ洢��Ʒ�����ݼ����������λ����Ϣ��
	//				����KUiObjAtContRegion::nContainerֵ��ʾ�ڼ�ҳ����Ʒ
	//nParam = pInfo�����а���KUiObjAtContRegion�ṹ����Ŀ
	//Return = �������ֵС�ڵ��ڴ������nParam����ֵ��ʾpInfo�����е�ǰ���ٸ�KUiObjAtContRegion
	//			�ṹ���������Ч�����ݣ������ʾ��Ҫ����������ٸ�KUiObjAtContRegion�ṹ������
	//			�Ź��洢ȫ����npc�г������׵���Ʒ��Ϣ��

	GDI_TRADE_ITEM_PRICE,		//������Ʒ�ļ۸�
	//uParam = (KUiObjAtContRegion*) pItemInfo -> ����ָ�����Ĵ����ĸ���Ʒ
	//nParam = (KUiItemBuySelInfo*) pPriceInfo -> ���ڽ�����Ʒ���ƽ��׼۸����Ϣ
	//Return = (int)(bool) �ɷ���

	GDI_REPAIR_ITEM_PRICE,		//������Ʒ�ļ۸�
	//uParam = (KUiObjAtContRegion*) pItemInfo -> ����ָ�����Ĵ����ĸ���Ʒ
	//nParam = (KUiItemBuySelInfo*) pPriceInfo -> ���ڽ�����Ʒ�����������õ���Ϣ
	//Return = (int)(bool) �ɷ�����

	GDI_TRADE_OPER_DATA,		//���ײ�����ص�����
	//uParam = (UI_TRADE_OPER_DATA)eOper ���庬���UI_TRADE_OPER_DATA
	//nParam ����Ӧ���뺬����uParam��ȡֵ״������,��UI_TRADE_OPER_DATA��˵��
	//Return ���庬����uParam��ȡֵ״������,��UI_TRADE_OPER_DATA��˵��

	GDI_LIVE_SKILL_BASE,		
	//uParam = (KUiPlayerLiveSkillBase*) pInfo -> ���ǵ����������

	GDI_LIVE_SKILLS,			
	//uParam = (KUiSkillData*) pSkills -> ����10��KUiSkillData���������ڴ洢��������ܵĵ����ݡ�

	GDI_FIGHT_SKILL_POINT,		
	//Return = ʣ��ս�����ܵ���

	GDI_FIGHT_SKILLS,			
	//uParam = (KUiSkillData*) pSkills -> ����50��KUiSkillData���������ڴ洢����ս�������ݡ�

	GDI_CUSTOM_SKILLS,			
	//uParam = (KUiSkillData*) pSkills -> ����5��KUiSkillData���������ڴ洢�����Դ��书�����ݡ�

	GDI_LEFT_ENABLE_SKILLS,		
	//uParam = (KUiSkillData*) pSkills -> ����65��KUiSkillData���������ڴ洢�����ܵ����ݡ�
	//								KUiSkillData::nLevel��������������ʾ�ڵڼ���
	//Return = ������Ч���ݵ�Skills����Ŀ

	GDI_RIGHT_ENABLE_SKILLS,	
	//uParam = (KUiSkillData*) pSkills -> ����65��KUiSkillData���������ڴ洢�����ܵ����ݡ�
	//								KUiSkillData::nLevel��������������ʾ�ڵڼ���
	//Return = ������Ч���ݵ�Skills����Ŀ

	GDI_NEARBY_PLAYER_LIST,
	//uParam = (KUiPlayerItem*)pList -> ��Ա��Ϣ�б�
	//			KUiPlayerItem::nData = 0
	//nParam = pList�����а���KUiPlayerItem�ṹ����Ŀ
	//Return = �������ֵС�ڵ��ڴ������nParam����ֵ��ʾpList�����е�ǰ���ٸ�KUiPlayerItem
	//			�ṹ���������Ч�����ݣ������ʾ��Ҫ����������ٸ�KUiPlayerItem�ṹ������
	//			�Ź��洢ȫ����Ա��Ϣ��

	GDI_NEARBY_IDLE_PLAYER_LIST,//��ȡ��Χ�µ������������ҵ��б�

	GDI_NEARBY_NOT_FRIEND_LIST,//��ȡ��Χ�Ǻ��ѵ�����б�

	GDI_PLAYER_LEADERSHIP,		//����ͳ˧������ص�����
	//uParam = (KUiPlayerLeaderShip*) -> ����ͳ˧������ص����ݽṹָ��

	GDI_ITEM_IN_ENVIRO_PROP,	//�����Ʒ��ĳ������λ�õ�����״̬
	//uParam = (KUiGameObject*)pObj����nParam==0ʱ����Ʒ����Ϣ
	//uParam = (KUiObjAtContRegion*)pObj����nParam!=0ʱ����Ʒ����Ϣ
	//			��ʱKUiObjAtContRegion::Region�����ݹ̶�Ϊ0�������塣
	//nParam = (int)(bool)bJustTry  �Ƿ�ֻ�ǳ��Է���
	//Return = (ITEM_IN_ENVIRO_PROP)eProp ��Ʒ������״̬
	
	GDI_CHAT_SEND_CHANNEL_LIST,		//��ҿ��Է�����Ϣ������Ƶ�����б�
	//uParam = (KUiChatChannel*) pList -> KUiChatChannel�ṹ�����ָ��
	//nParam = pList�����а���KUiChatChannel�ṹ����Ŀ
	//Return = �������ֵС�ڵ��ڴ������nParam����ֵ��ʾpInfo�����е�ǰ���ٸ�KUiChatChannel
	//			�ṹ���������Ч�����ݣ������ʾ��Ҫ����������ٸ�KUiChatChannel�ṹ������
	//			�Ź��洢ȫ����ָ��Ƶ����Ϣ��

	GDI_CHAT_RECEIVE_CHANNEL_LIST,	//��ҿ��Զ��ĵ���ϢƵ�����б�
	//����������ֵ����ͬGDI_CHAT_SEND_CHANNEL_LIST

	GDI_CHAT_CURRENT_SEND_CHANNEL,	//��ȡ��ǰ������Ϣ��Ƶ������Ϣ
	//uParam = (KUiChatChannel*) pChannelInfo ָ���������Ƶ������Ϣ�Ľṹ�ռ�

	GDI_CHAT_GROUP_INFO,			//����ĺ��ѷ�����Ϣ
	//��nParam������������ʼ���ҵ�һ����Ч�ķ��飬���ظ÷������Ϣ�����������
	//uParam = (KUiChatGroupInfo*) pGroupInfo ������Ϣ
	//nParam = nIndex ����ȡ�ķ��������
	//Return = ʵ�ʷ������ݵķ�������������δ����򷵻�-1

	GDI_CHAT_FRIENDS_IN_AGROUP,		//����һ�����ѷ����к��ѵ���Ϣ
	//uParam = (KUiPlayerItem*)pList -> ��Ա��Ϣ�б�
	//			KUiPlayerItem::nData = (CHAT_STATUS)eFriendStatus ���ѵĵ�ǰ״̬
	//nParam = Ҫ��ȡ�б��ĺ��ѷ��������
	//Return = ��ֵ��ʾpList�����е�ǰ���ٸ�KUiPlayerItem�ṹ���������Ч������.

	GDI_PK_SETTING,					//��ȡpk����
	//Return = (int)(bool)bEnable	�Ƿ�����pk

	GDI_SHOW_PLAYERS_NAME,			//��ȡ��ʾ���������
	//Return = (int)(bool)bShow	�Ƿ���ʾ
	GDI_SHOW_PLAYERS_LIFE,			//��ȡ��ʾ���������
	//Return = (int)(bool)bShow	�Ƿ���ʾ
	GDI_SHOW_PLAYERS_MANA,			//��ȡ��ʾ���������
	//Return = (int)(bool)bShow	�Ƿ���ʾ
	
	GDI_PARADE_EQUIPMENT,				//�����װ����Ʒ,��Ϣ����ͬGDI_EQUIPMENT
	
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

	GDI_PLAYER_HOLD_FKCOIN,		//add by phong ki�u s?xu ng��i tr�n player

	NPC_OI_TARGET_INFO, //get target info for showing in client

	GDI_PLAYER_MERIDIAN,			//get Meridian info, return meridian level array
	GDI_PLAYER_SERIES,				// [KM 27/08] he ngu hanh nhan vat (0..4 theo series_*), -1 neu chua co

	GDI_ITEM_EQUIP_SAME_GERNE,  //get item with same gerne with id in equip

	GDI_GAMBLE_OPER_DATA, //OTT Data related to trading operations
	//uParam = (UI_GAMBLE_OPER_DATA)eOper For specific meaning, see UI_TRADE_OPER_DATA
	//nParam The specific application and meaning are determined by the value of uParam, see the description of UI_TRADE_OPER_DATA
	//Return The specific meaning is determined by the value of uParam, see the description of UI_TRADE_OPER_DATA

	// [UILOREN] 6 khu chua do cua he lo ren - them CUOI enum de khong
	// xe dich gia tri cac muc cu; doi ung UOC_*/pos_* trong GameDataDef.h
	GDI_COMPONE_ITEM,		//Lo ren - luyen huyen tinh khoang thach
	GDI_COMPTWO_ITEM,		//Lo ren - nang cap huyen tinh khoang thach
	GDI_COMPTHREE_ITEM,		//Lo ren - nang cap khoang thach thuoc tinh
	GDI_DISTILL_ITEM,		//Lo ren - rut option trang bi
	GDI_FORGE_ITEM,			//Lo ren - che tao trang bi tim
	GDI_ENCHASE_ITEM,		//Lo ren - kham nam trang bi
	GDI_ITEM_IN_PARTNER_BAG,	// [BDH-G4] tui ban dong hanh (them CUOI)
	GDI_ATLAS_ITEM,		// [LOREN 27/08] THE DO PHO - o cua the Do pho (them CUOI)
	GDI_ITEM_PARTICULAR,	// [LOREN 28/08] Do pho: ve ten nguyen lieu tren trang (them CUOI)
};

enum GAMEDATA_CHANGED_NOTIFY_INDEX
{
	GDCNI_HOLD_OBJECT = 1,
	//uParam = (KUiGameObject*)pObject -> 
	GDCNI_PLAYER_BASE_INFO,	//see also GDI_PLAYER_BASE_INFO
	GDCNI_PLAYER_RT_ATTRIBUTE,	// see also GDI_PLAYER_RT_ATTRIBUTE
	GDCNI_PLAYER_IMMED_ITEMSKILL,
	//uParam = (KUiGameObject*)pInfo ��Ʒ�����书������
	//nParam = �ĸ�λ�õ���Ʒ�����书 ��nParam >=0 ʱ�� nParam��ʾ��nParam��������Ʒ
	//			��nParam = -1 ʱ����ʾΪ�����书��-2ʱΪ�����书
	GDCNI_PLAYER_BRIEF_PROP,//
	//uParam = (PLAYER_BRIEF_PROP)eProp �仯�����ȡֵΪö��PLAYER_BRIEF_PROP��ֵ֮һ��
	//nParam = ��������uParam�ľ��庬�����
	GDCNI_OBJECT_CHANGED,		//ĳ����Ʒ���������߼���
	//uParam = (KUiObjAtContRegion*)pInfo -> ��Ʒ���ݼ����������λ����Ϣ
	//          ��pInfo->eContainer == UOC_EQUIPTMENTʱ
	//				KUiObjAtRegion::Region::h ��ʾ���ڵڼ���װ��
	//				KUiObjAtRegion::Region::v ��ʾ�����ĸ�λ�õ�װ��,��ֵΪö������
	//				UI_EQUIPMENT_POSITION��ȡֵ֮һ����ο�UI_EQUIPMENT_POSITION��ע�͡�
	//nParam = bAdd -> 0ֵ��ʾ���������Ʒ����0ֵ��ʾ���������Ʒ
	GDCNI_CONTAINER_OBJECT_CHANGED,	//���������Ķ���ȫ������
	//uParam = (UIOBJECT_CONTAINER)eContainer;	//���λ��
	GDCNI_LIVE_SKILL_BASE,		//�������ֵ�仯

	GDCNI_FIGHT_SKILL_POINT,	//ʣ��ս�����ܵ����仯
	//nParam = �µ�ʣ��ս�����ܵ���
	GDCNI_SKILL_CHANGE,			//��������һ������/��������
	//uParam = (KUiSkillData*)pSkill -> �����յļ���
	//nParam = �¼�����ͬ��е�����λ��
	GDCNI_PLAYER_LEADERSHIP,	//����ͳ˧������ص����ݷ����仯
	//
	GDCNI_TEAM,	
	//uParam = (KUiPlayerTeam*)pTeam -> ������Ϣ,����ΪNULL,��ʾ��������˶���
	GDCNI_TEAM_NEARBY_LIST,		
	//uParam = (KUiTeamItem*)pList �������Ϣ������
	//nParam = nCount �������Ŀ
	GDCNI_TRADE_START,			//����ҽ��׿�ʼ
	//uParam = (KUiPlayerItem*) pPlayer�Է�����Ϣ
	GDCNI_TRADE_DESIRE_ITEM,	//�Է������뽻�׵���Ʒ
	//uParam = (KUiObjAtRegion*) pObject -> ��Ʒ��Ϣ������������ϢΪ�ڽ��׽����е�����
	//nParam = bAdd -> 0ֵ��ʾ���٣�1ֵ��ʾ����
	//Remark : �����Ʒ�ǽ�Ǯ�Ļ�����KUiObjAtRegion::Obj::uId��ʾ�ѽ�Ǯ�����Ϊ���ֵ����nParam�����塣
	GDCNI_TRADE_OPER_DATA,		//���ײ�����ص�(״̬)���ݷ����仯
	//uParam = (const char*) pInfoText ��ʾ�����֣���"�Է��������"��
	GDCNI_TRADE_END,			//���׽���
	GDCNI_GAMBLE_START,			//����ҽ��׿�ʼ
	GDCNI_GAMBLE_RESET,
	GDCNI_GAMBLE_RESULT,
	//uParam = (KUiPlayerItem*) pPlayer�Է�����Ϣ
	GDCNI_GAMBLE_DESIRE_ITEM,	//�Է������뽻�׵���Ʒ
	//uParam = (KUiObjAtRegion*) pObject -> ��Ʒ��Ϣ������������ϢΪ�ڽ��׽����е�����
	//nParam = bAdd -> 0ֵ��ʾ���٣�1ֵ��ʾ����
	//Remark : �����Ʒ�ǽ�Ǯ�Ļ�����KUiObjAtRegion::Obj::uId��ʾ�ѽ�Ǯ�����Ϊ���ֵ����nParam�����塣
	GDCNI_GAMBLE_OPER_DATA,		//���ײ�����ص�(״̬)���ݷ����仯
	//uParam = (const char*) pInfoText ��ʾ�����֣���"�Է��������"��
	GDCNI_GAMBLE_END,			//���׽���
	//nParam = (int)(bool)bTraded	�Ƿ�����˽���
	GDCNI_NPC_TRADE,			
	//nParam = (bool)bStart	�Ƿ���Ϊ���׿�ʼ�����ȡֵ�����ʾ���ף��رգ�����
	GDCNI_NPC_TRADE_ITEM,		
	//ͨ������iCoreShell::GetGameData,ʹ����uDataIdΪGDI_TRADE_NPC_ITEM������ȡ�µ���Ʒ��Ϣ��
	GDCNI_GAME_START,			//������Ϸ����

	GDCNI_GAME_QUIT,			//�뿪����Ϸ����

	GDCNI_QUESTION_CHOOSE,		//����ѡ��
	//uParam = (KUiQuestionAndAnswer*)pQuestionAndAnswer
	GDCNI_SPEAK_WORDS,			//npc˵������
	//uParam = (KUiInformationParam*) pWordDataList ָ��KUiInformationParam����
	//nParam = pWordDataList����KUiInformationParamԪ�ص���Ŀ
	GDCNI_INFORMATION,			//�µ���Ϣ����������ר�ŵ���Ϣ��
	//uParam = (KUiInformationParam*)pInformation	//��Ϣ����
	GDCNI_MSG_ARRIVAL,			//�£��������ݴ��ڣ���Ϣ����
	//uParam = (cons char*)pMsgBuff ��Ϣ���ݻ�����
	//nParam = (KUiMsgParam*)pMsgParam ��Ϣ����
	GDCNI_SEND_CHAT_CHANNEL,	//��ǰ������Ϣ��Ƶ���ı���

	GDCNI_CHAT_GROUP,			//������ѷ��鷢���仯

	GDCNI_CHAT_FRIEND,			//������ѷ����仯
	//nParam = nGroupIndex �������ѱ仯�ķ��������
	GDCNI_CHAT_FRIEND_STATUS,	//�������״̬�����仯
	//uParam = (KUiPlayerItem*)pFriend ����״̬�仯�ĺ���
	//			KUiPlayerItem::nData = (CHAT_STATUS)eFriendStatus ���ѵĵ�ǰ״̬
	//nParam = nGroupIndex ���������ڵ��������
	GDCNI_CHAT_MESSAGE,			//���з�����ѶϢ
	//uParam = (KUiChatMessage*)pMessage ��Ϣ����
	//nParam = (KUiPlayerItem*)pFriend   ����ѶϢ�ĺ���
	GDCNI_SYSTEM_MESSAGE,		//ϵͳ��Ϣ
	//uParam = (KSystemMessage*)pMsg ϵͳ��Ϣ������
	//nParam = (void*)pParamBuf ָ��һ�����������������������ݺ�������pMsg�����ݶ�����
	//			�ο�KSystemMessage��ע��˵�����������Ĵ�С��pMsg->byParamSize������
	GDCNI_NEWS_MESSAGE,			//������Ϣ
	//uParam = (KNewsMessage*)pMsg ������Ϣ����
	//nParam = (SYSTEMTIME*)pTime  ������Ϣ��ʱ����������庬��������������Ϣ�����Ͷ���
	GDCNI_SWITCH_CURSOR,		//�л����ָ��ͼ��
	
	GDCNI_OPEN_STORE_BOX,	
	
	GDCNI_SWITCHING_SCENEPLACE,	//��ͼ�л�
	//nParam = (int)(bool)bStart Ϊ��0ֵ��ʾ��ʼ��ͼ�л���Ϊ0ֵ��ʾ������ͼ�л�
	GDCNI_MISSION_RECORD,		//Ҫ���¼�´�������ʾ��Ϣ
	//uParam = (KMissionRecord*) pRecord ��¼����
	GDCNI_PK_SETTING,			//pk����״�������˱仯
	//nParam = (int)(bool)bEnable �Ƿ�����pk
	GDCNI_VIEW_PLAYERITEM,			//���������װ���������ѵ�
	//uParam = (KUiPlayerItem*)		//��ҵ� player data
	GDCNII_RANK_INDEX_LIST_ARRIVE,	//������id�б�����
	//uParam = uCount ������id����Ŀ
	//nParam = (KRankIndex*)pIndexList ������id�����ṹ������
	GDCNII_RANK_INFORMATION_ARRIVE,	//����������ݵ���
	//uParam = ((unsigned short)usCount) | ((unsigned short)usIndexId << 16)
	//			usCount    �˴δ��������������Ŀ
	//			usIndexId  ������id����ʾ���ص������id�������������
	//nParam = (KRankMessage *) �б�����
	//====�������====
	GDCNI_TONG_INFO,				//ĳ�����ڵİ��ɵ���Ϣ
									//��Ӧ��GTOI_REQUEST_PLAYER_TONG����ѯĳ�����İ�ᣩ
	//uParam = (KUiPlayerRelationWithOther*) �Ǹ���ĳ����,�Լ�������˰��ɵĹ�ϵ
					//KUiPlayerRelationWithOther::nParam �Ƿ��ŵ����˿���
	//nParam = (KTongInfo*) pToneInfo	//���ɵ���Ϣ
	GDCNI_TONG_MEMBER_LIST,			//ĳ�������еĳ�Ա���б�
	//uParam = (KUiGameObjectWithName*) pTong �����������ĸ�����
							//KUiGameObjectWithName::szName ��������
							//KUiGameObjectWithName::nData  pMemberList�������ɳ�Ա�����ͣ��ο�TONG_MEMBER_FIGURE
							//KUiGameObjectWithName::nParam pMemberList�������ɳ�Ա�б�������Ա����ʼ������
							//KUiGameObjectWithName::uParam pMemberList�������ɳ�Ա�б�������Ա����Ŀ
	//nParam = (KTongMemberItem*) pMemberList
	GDCNI_TONG_ACTION_RESULT,       //���������صĽ��
	//uParam = (KUiGameObjectWithName*) pInfo ���ص�����
	                        //KUiGameObjectWithName::szName ������ҵ�����
							//KUiGameObjectWithName::nData  ���������࣬�ο�TONG_ACTION_TYPE
							//KUiGameObjectWithName::nParam   V
							//KUiGameObjectWithName::uParam   V
							//KUiGameObjectWithName::szString �⼸�������治ͬ������ͬ
	// nParam : �ɹ� 1 ʧ�� 0
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

	GDCNI_PLAYER_LOGIN_REPLAY, //fix by phong ki�u chuy�n gs b?m�t skill

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

	// He XUC XAC chia do (DICEITEM) 26/08: may chu bao mo/dong/cap nhat mot o.
	// uParam = con tro DICE_ITEM_SYNC, CHI SONG trong loi goi - phai chep ra ngay.
	GDCNI_DICE_ITEM,

	// [PHI PHONG 2026-08-29] mo panel kham Tinh Than Thach.
	// PHAI o CUOI enum: gia tri GDCNI di qua ranh gioi CoreClient.dll <-> Game.exe.
	GDCNI_OPEN_MANTLE_INLAY,
	// [PF13 01/09] mo panel TAY LUYEN thuoc tinh an (chen SAU MANTLE_INLAY, van cuoi enum)
	GDCNI_OPEN_MANTLE_WASH,
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
	//uParam = (KUiGameObject*)pInfo -> ��������
	//nParam = (Ŀǰֻ��0ֵ��)���ָ�뵱ǰ���꣨�������꣩���������ڵ�16λ���������ڸ�16λ��(���ص�����)

	GOI_SET_IMMDIA_SKILL,	//������������
	//uParam = (KUiGameObject*)pSKill, ������Ϣ
	//nParam = ����λ�ã�0��ʾΪ������ܣ�1��ʾΪ�Ҽ�����

	GOI_TONE_UP_SKILL,		//��ǿһ�ּ��ܣ���һ�μ�һ��
	//uParam = ��������
	//nParam = (uint)����id

	GOI_TONE_UP_ATTRIBUTE,	//��ǿһЩ���Ե�ֵ��һ�μ�һ��
	//uParam = ��ʾҪ��ǿ�����ĸ����ԣ�ȡֵΪUI_PLAYER_ATTRIBUTE��÷��ֵ֮һ

	//============����������ң��������================
	GOI_TRADE_INVITE_RESPONSE,	//��Ӧ/�ܾ���������
	//uParam = (KUiPlayerItem*)pRequestPlayer ������������
	//nParam = (int)(bool)bAccept �Ƿ��������

	GOI_TRADE_DESIRE_ITEM,		//����һ������������Ʒ
	//uParam = (KUiObjAtRegion*) pObject -> ��Ʒ��Ϣ������������ϢΪ�ڽ��׽����е�����
	//nParam = bAdd -> 0ֵ��ʾ���٣�1ֵ��ʾ����
	//Remark : �����Ʒ�ǽ�Ǯ�Ļ�����KUiObjAtRegion::Obj::uId��ʾ�ѽ�Ǯ�����Ϊ���ֵ����nParam�����塣

	GOI_TRADE_WILLING,			//giao dich giao d�ch
	//uParam = (const char*)pszTradMsg ���ڽ�����Ϣһ�仰����bWillingΪtrueʱ��Ч
	//nParam = (int)(bool)bWilling �Ƿ��ڴ�����(����)

	GOI_TRADE_LOCK,				//��������
	//nParam = (int)(bool)bLock �Ƿ�����

	GOI_TRADE,					//����
	//nParam = (int)(bool)bTrading
	
	GOI_TRADE_CANCEL,			//����ȡ��

	GOI_GAMBLE_INVITE_RESPONSE,	//��Ӧ/�ܾ���������
	//uParam = (KUiPlayerItem*)pRequestPlayer ������������
	//nParam = (int)(bool)bAccept �Ƿ��������

	GOI_GAMBLE_DESIRE_ITEM,		//����һ������������Ʒ
	//uParam = (KUiObjAtRegion*) pObject -> ��Ʒ��Ϣ������������ϢΪ�ڽ��׽����е�����
	//nParam = bAdd -> 0ֵ��ʾ���٣�1ֵ��ʾ����
	//Remark : �����Ʒ�ǽ�Ǯ�Ļ�����KUiObjAtRegion::Obj::uId��ʾ�ѽ�Ǯ�����Ϊ���ֵ����nParam�����塣

	GOI_GAMBLE_WILLING,			//giao dich giao d�ch
	//uParam = (const char*)pszTradMsg ���ڽ�����Ϣһ�仰����bWillingΪtrueʱ��Ч
	//nParam = (int)(bool)bWilling �Ƿ��ڴ�����(����)

	GOI_GAMBLE_LOCK,				//��������
	//nParam = (int)(bool)bLock �Ƿ�����

	GOI_GAMBLE,					//����
	//nParam = (int)(bool)bTrading

	GOI_GAMBLE_CANCEL,			//����ȡ��

	//============================
	GOI_TRADE_NPC_BUY,
	//uParam = (KUiGameObject*)pObj -> ��Ʒ��Ϣ

	GOI_TRADE_NPC_SELL,			
	//uParam = (KUiObjAtContRegion*)pObj -> ��Ʒ��Ϣ

	GOI_TRADE_NPC_REPAIR,		//������Ʒ
	//uParam = (KUiObjAtContRegion*) pObj -> ��Ҫ����Ʒ����Ϣ

	GOI_TRADE_NPC_CLOSE,		//��������

	GOI_AUTOPLAY_ACTION,		//GOI_DROP_ITEM_QUERY
	//uParam = (KUiGameObject*)pObject -> ��Ʒ��Ϣ
	//nParam = ���϶������ĵ�ǰ���꣨�������꣩���������ڵ�16λ���������ڸ�16λ��(���ص�����)
	//Return = �Ƿ���Է���
	
//	GOI_DROP_ITEM,				//������Ʒ����Ϸ����
	//��������ͬGOI_DROP_ITEM_QUERY����������ͬ
	//Return = �Ƿ�����������

	GOI_SEND_MSG,				//������Ϣ
	//uParam = (const char*)pMsgBuff ��Ϣ���ݻ�����
	//nParam = (KUiMsgParam*)pMsgParam ��Ϣ����
	
	//============�������================
	GOI_SET_SEND_CHAT_CHANNEL,	//���õ�ǰ������Ϣ��Ƶ��
	//uParam = (KUiChatChannel*) pChannelInfo Ҫ���õ�Ƶ������Ϣ
	
	GOI_SET_SEND_WHISPER_CHANNEL,//���õ�ǰ������Ϣ��Ƶ��Ϊ��ĳ�˵���
	//uParam = (KUiPlayerItem*) pFriend

	GOI_SET_RECEIVE_CHAT_CHANNEL,//���ö���/ȡ����ϢƵ��
	//uParam = (KUiChatChannel*) pChannelInfo Ҫ���õ�Ƶ������Ϣ
	//nParam = (int)(bool) bEnable �Ƿ���

	GOI_CHAT_GROUP_NEW,			//�½����������
	//uParam = (const char*) pGroupName
	//Return = (int)(bool) bSuccessed

	GOI_CHAT_GROUP_RENAME,		//���������������
	//uParam = (const char*) pGroupName
	//nParam = nIndex ������
	//Return = (int)(bool) bSuccessed

	GOI_CHAT_GROUP_DELETE,		//ɾ�����������
	//nParam = nIndex ������
	//Return = (int)(bool) bSuccessed

	GOI_CHAT_FRIEND_ADD,		//�������������
	//uParam = (KUiPlayerItem*)pFriend
	//			KUiPlayerItem::nData = 0

	GOI_CHAT_FRIEND_DELETE,		//ɾ���������
	//uParam = (KUiPlayerItem*)pFriend
	//			KUiPlayerItem::nData = 0
	//nParam = nGroupIndex �������ڵ��������
	//Return = (int)(bool) bSuccessed

	GOI_CHAT_FRIEND_MOVE,		//�����Ƶ�����
	//uParam = (KUiPlayerItem*)pFriend
	//			KUiPlayerItem::nData = 0
	//nParam = nGroupIndex �������������������
	//Return = (int)(bool) bSuccessed

	GOI_CHAT_SET_STATUS,		//�л�����״̬
	//uParam = (CHAT_STATUS)eStatus �µ�����״̬

	GOI_CHAT_WORDS_TO_FRIEND,	//��������һ��ѶϢ
	//uParam = (KUiChatMessage*)pMessage ��Ϣ����
	//nParam = (KUiPlayerItem*)pFriend   ����ѶϢ�ĺ���

	GOI_CHAT_FRIEND_INVITE,		//�Ա���Ҫ���Լ�Ϊ���ѵĻظ�
	//uParam = (KUiPlayerItem*)pRequestPlayer ������������
	//nParam = (int)(bool)bAccept �Ƿ��������

	GOI_OPTION_SETTING,			
	//uParam = (OPTIONS_LIST)eOptionItem Ҫ���õ�ѡ��
	//nParam = (int)nValue ���õ�ֵ���京��������eOptionItem�ĺ���
	//					�ο�OPTIONS_LIST��ֵ��ע��

	GOI_PLAY_SOUND,				//��������
	//uParam = (const char*)pszFileName

	GOI_PK_SETTING,				//����PK
	//nParam = (int)(bool)bEnable	�Ƿ�����pk

	GOI_REVENGE_SOMEONE,		//��ɱĳ��
	//uParam = (KUiPlayerItem*) pTarget	��ɱĿ��

	GOI_SHOW_PLAYERS_NAME,		//��ʾ���������
	//nParam = (int)(bool)bShow	�Ƿ���ʾ
	GOI_SHOW_PLAYERS_LIFE,		//��ʾ���������
	//nParam = (int)(bool)bShow	�Ƿ���ʾ
	GOI_SHOW_PLAYERS_MANA,		
	//nParam = (int)(bool)bShow	�Ƿ���ʾ

	GOI_GAMESPACE_DISCONNECTED,	
	
	GOI_VIEW_PLAYERITEM,		
	//uParam = dwNpcID	��ҵ�m_dwID
	GOI_VIEW_PLAYERITEM_END,	

	GOI_FOLLOW_SOMEONE,			
	//uParam = (KUiPlayerItem*) pTarget	����Ŀ��

	GOI_QUERY_RANK_INFORMATION,  
	//uParam = usIndexId �������id

	GOI_SHOW_OBJ_NAME, // hien ten obj duoi dat edit by phong kieu

	GOI_CP_UNLOCK,		//open ruong
	
	GOI_CP_LOCK,		//close ruong

	GOI_CP_SWITCH_EQUIPSET,

	GOI_CP_CHANGE,		//doi mk ruong
	
	GOI_CP_RESET,		//reset mk ruong

	GDI_SET_TRADE_ITEM,			// ban hang

	GDI_PLAYER_TRADE,			//��̯
	
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

	GAUTO_AUTO_PT_PLAYERTEAM,//qu�n l?t?��i

	GAUTO_AUTO_MOVETPSID, //ch�y to?�� ra b�i train
	
	GAUTO_AUTO_MOVETPSX,
	
	GAUTO_AUTO_MOVETPSY,

	GAUTO_AUTO_BLACK_ITEM, //qu�n l?v�t ph�m �en

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
	GOI_TASKGUIDE_GOTO_SATTHU,	// [3HD C20] bam nhiem vu Sat Thu tren F11 -> tu chay toi boss (bam lai = huy)

	// He XUC XAC chia do (DICEITEM) 26/08: nguoi choi bam nut trong cua so.
	// uParam = ma phien xuc xac, nParam = DICE_CHOICE_GIVEUP / DICE_CHOICE_NEED.
	// Dat o CUOI enum: enum nay do CA Core lan S3Client bien dich, chen giua
	// se dich gia tri cua moi thanh vien sau no.
	GOI_DICE_CHOICE,

	// [BDH-G4] cua so Ban Dong Hanh: uParam = PARTNER_OP_*, nParam = tham so.
	// Op RENAME: nParam = (int)(const char*) ten moi. Dat CUOI enum.
	GOI_PARTNER_OP,

	// [PF13 01/09] panel Tay Luyen: uParam = dwID cua phi phong, nParam = (char*)
	// dem >=256 byte -> ghi 2 dong an (khe 6-7) da dinh dang, phan cach bang '\n';
	// tra 1 neu co dong an, 0 neu khong. Dat CUOI enum (Core + S3Client cung dich).
	GDI_MANTLE_HIDDEN_DESC,

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
// Core�ⲿ�ͻ���core�ĳ�����ͼ��صĲ����������������
//=========================================================
//����������������ز���uParam��nParam�����ע����δ�ἰ���򴫵ݶ�ֵ0��
//����ر�ָ������ֵ���壬��ɹ���ȡ���ݷ���1��δ�ɹ�����0��
enum GAME_SCENE_MAP_OPERATION_INDEX
{
	GSMOI_SCENE_TIME_INFO,			//��ǰ���������ĵ���ʱ�价��
	GSMOI_SCENE_TIME_INFO_OFTEN,
	//uParam = (KUiSceneTimeInfo*)pInfo

	GSMOI_SCENE_MAP_INFO,				//��ǰ���������ĳ����ĵ�ͼ��Ϣ
	//uParam = (KSceneMapInfo*) pInfo ���ڻ�ȡ��Ϣ�Ľṹ��������ָ��
	//Return = (int)(bool)bHaveMap ����ֵ��ʾ��ǰ�����Ƿ���С��ͼ���������0ֵʱ�� pInfo�ڷ��ص�ֵ������

	GSMOI_IS_SCENE_MAP_SHOWING,	//���ó�����С��ͼ�Ƿ���ʾ��״̬
	//uParam = uShowElem,		//��ʾ��Щ���ݣ�ȡֵΪSCENE_PLACE_MAP_ELEMö�ٵ�һ����������ϡ�
				//SCENE_PLACE_MAP_ELEM��GameDataDef.h�ж���
				//���С��ͼ������һЩ���ǻ����
	//nParam = ��16λ��ʾ��ʾ�Ŀ��ȣ���16λ��ʾ��ʾ�ĸ߶ȣ���λ�����ص㣩

	GSMOI_PAINT_SCENE_MAP,		//���Ƴ�����С��ͼ
	//uParam = (int)h ��ʾ������ʼ������Ļ�Ϻ��������꣨��λ�����ص㣩
	//nParam = (int)v ��ʾ������ʼ������Ļ�����������꣨��λ�����ص㣩

	GSMOI_SCENE_MAP_FOCUS_OFFSET,//����С��ͼ�Ľ��㣨/���ģ�
	//uParam = (int)nOffsetH	����С��ͼ�����ˮƽ���꣨��λ���������꣩
	//nParam = (int)nOffsetV	����С��ͼ����Ĵ�ֵ���꣨��λ���������꣩

	GSMOI_SCENE_FOLLOW_WITH_MAP,	//���ó����Ƿ����ŵ�ͼ���ƶ����ƶ�
	//nParam = (int)nbEnable �����Ƿ����ŵ�ͼ���ƶ����ƶ�

	GSMOI_IS_SCENE_DIRECT_MAP,

	GSMOI_IS_SCENE_DO_DIRECT_MAP,
	GSMOI_SCENE_MAP_FLAG_ON_TARGET,
	GSMOI_IS_SCENE_MAP_FLAGIMG,
	GSMOI_SCENE_MAP_REMOVE_FLAG,
	GSMOI_SCENE_MAP_GET_FLAGPOS,
	GSMOI_SCENE_MAP_TG_COORD,
};

//=========================================================
// Core�ⲿ�ͻ���core�İ����صĲ����������������
//=========================================================
//����������������ز���uParam��nParam�����ע����δ�ἰ���򴫵ݶ�ֵ0��
//����ر�ָ������ֵ���壬��ɹ���ȡ���ݷ���1��δ�ɹ�����0��
enum GAME_TONG_OPERATION_INDEX
{
	GTOI_TONG_CREATE,			//�������
	//uParam = (const char*) pszTongName ��������
	//nParam = (NPCCAMP)enFaction �����Ӫ

	GTOI_TONG_IS_RECRUIT,		//��ѯĳ�˵����˿���
	//uParam = (KUiPlayerItme*) Ҫ��˭
	//Return = (int)(bool)		�Ƿ��ŵ����˿���

	GTOI_TONG_RECRUIT,          //���˿���
	//uParam = (int)(bool)bRecruit �Ƿ�Ը������

	GTOI_TONG_ACTION,           //�԰��ڳ�Ա���Ķ��������Լ�����Ĺ�ϵ�ĸı�
	//uParam = (KTongOperationParam*) pOperParam ����ʱ�Ĳ���
	//nParam = (KTongMemberItem*) pMember ָ���˲���������Ա������

	GTOI_TONG_JOIN_REPLY,       //���������Ĵ�
	//uPAram = (KUiPlayerItem *) pTarget   ���뷽
	//nParam : !=0ͬ��     ==0�ܾ�

	GTOI_REQUEST_PLAYER_TONG,	//��ѯĳ�����İ��
	//uParam = (KUiPlayerItem*) Ҫ��˭
	//nParam = (int)(bool)bReturnTongDetail �Ƿ�Ҫ�����Ǹ�������Ϣ

	GTOI_REQUEST_TONG_DATA,     //Ҫ��ĳ�����ĸ�������
	//uParam = (KUiGameObjectWithName*)pTong Ҫ��ѯ�İ��
			//KUiGameObjectWithName::szName ��������
			//KUiGameObjectWithName::nData ���ϵ����ֵ࣬ȡ��ö��TONG_MEMBER_FIGURE
			//			�б���������enumTONG_FIGURE_MASTER�Ļ�����Ҫ����ǰ�����Ѷ��
			//KUiGameObjectWithName::nParam ��ʼ������
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
// Core�ⲿ�ͻ���core�������صĲ����������������
//=========================================================
//����������������ز���uParam��nParam�����ע����δ�ἰ���򴫵ݶ�ֵ0��
//����ر�ָ������ֵ���壬��ɹ���ȡ���ݷ���1��δ�ɹ�����0��
enum GAME_TEAM_OPERATION_INDEX
{
	TEAM_OI_GD_INFO,				//�������ڵĶ�����Ϣ
	//uParam = (KUiPlayerTeam*)pTeam -> ������Ϣ
	//Return = bInTeam, ���Ϊ��0ֵ��ʾ�����ڶ����У�pTeam�ṹ�Ƿ������Ϣ��
	//					���Ϊ0ֵ��ʾ���ǲ��ڶ����У�pTeam�ṹδ�������Ч��Ϣ��

	TEAM_OI_GD_MEMBER_LIST,		
	//uParam = (KUiPlayerItem*)pList -> ��Ա��Ϣ�б�
	//			KUiPlayerItem::nData = (int)(bool)bCaptain �Ƿ��Ƕӳ�
	//nParam = pList�����а���KUiPlayerItem�ṹ����Ŀ
	//Return = �������ֵС�ڵ��ڴ������nParam����ֵ��ʾpList�����е�ǰ���ٸ�KUiPlayerItem
	//			�ṹ���������Ч�����ݣ������ʾ��Ҫ����������ٸ�KUiPlayerItem�ṹ������
	//			�Ź��洢ȫ���ĳ�Ա��Ϣ��

	TEAM_OI_GD_REFUSE_INVITE_STATUS,//��ȡ�ܾ������״̬
	//Return = (int)(bool)bEnableRefuse Ϊ��ֵ��ʾ�ܾ�״̬��Ч�������ʾ���ܾ���

	TEAM_OI_COLLECT_NEARBY_LIST,//��ȡ��Χ������б�

	TEAM_OI_APPLY,				//����������˶���
	//uParam = (KUiTeamItem*)	Ҫ�������Ķ������Ϣ

	TEAM_OI_CREATE,				//�������

	TEAM_OI_APPOINT,			//�����ӳ���ֻ�жӳ����ò���Ч��
	//uParam = (KUiPlayerItem*)pPlayer -> �¶ӳ�����Ϣ
	//			KUiPlayerItem::nData = 0

	TEAM_OI_INVITE,			//������˼�����飬ֻ�жӳ����ò���Ч��
	//uParam = (KUiPlayerItem*)pPlayer -> Ҫ������˵���Ϣ
	//			KUiPlayerItem::nData = 0

	TEAM_OI_KICK,				//�߳������һ����Ա��ֻ�жӳ����ò���Ч��
	//uParam = (KUiPlayerItem*)pPlayer -> Ҫ�߳��Ķ�Ա����Ϣ
	//			KUiPlayerItem::nData = 0

	TEAM_OI_LEAVE,				//�뿪����

	TEAM_OI_CLOSE,				//�ر���ӣ�ֻ�жӳ����ò���Ч��
	//nParam = (int)(bool)bClose Ϊ��ֵ��ʾ���ڹر�״̬�������ʾ�����ڹر�״̬

	TEAM_OI_REFUSE_INVITE,		
	//nParam = (int)(bool)bEnableRefuse Ϊ��ֵ��ʾ�ܾ�״̬��Ч�������ʾ���ܾ���

	TEAM_OI_APPLY_RESPONSE,			//�Ƿ���׼���˼�����飬ֻ�жӳ����ò���Ч��
	//uParam = (KUiPlayerItem*)pPlayer -> Ҫ����׼������Ϣ
	//			KUiPlayerItem::nData = 0
	//nParam = (int)(bool)bApprove -> �Ƿ���׼��

	TEAM_OI_INVITE_RESPONSE,	//���������Ļظ�
	//uParam = (KUiPlayerItem*)pTeamLeader �����������Ķӳ�
	//nParam = (int)(bool)bAccept �Ƿ��������

	TEAM_OI_GET_NPC_MAP_POS,

};

enum GAME_AUTOPLAY_OPERATION_INDEX //add by phong ki�u using fkauto
{	
	AUTOPLAY_OI_ACTIVE, //--chi�n ��u--
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
	AUTOPLAY_OI_HP_B,	//b�m sl --ph�c h�i--
	AUTOPLAY_OI_HP_1,
	AUTOPLAY_OI_HP_2,
	AUTOPLAY_OI_HP_3,
	AUTOPLAY_OI_MP_B,	//b�m nl
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
	AUTOPLAY_OI_OBJ_1,//--nh�t �� ---
	AUTOPLAY_OI_OBJ_2,
	AUTOPLAY_OI_OBJ_3,
	AUTOPLAY_OI_OBJ_4,
	AUTOPLAY_OI_OBJ_5,
	AUTOPLAY_OI_OBJ_6, 
	AUTOPLAY_OI_OBJ_7,//gi?trang s�c
	AUTOPLAY_OI_OBJ_8,
	AUTOPLAY_OI_OBJ_9,
	AUTOPLAY_OI_OBJ_10,
	AUTOPLAY_OI_OBJ_11,
	AUTOPLAY_OI_OBJ_12,
	AUTOPLAY_OI_OBJ_13,
	AUTOPLAY_OI_MOVE_1,//--di chuy�n--
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
	AUTOPLAY_OI_MAP_1,//--b�n ��--
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
	AUTOPLAY_OI_TEAM_1,//--t?��i--
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

