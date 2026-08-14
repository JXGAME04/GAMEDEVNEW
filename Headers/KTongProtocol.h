// -------------------------------------------------------------------------
//	文件名		：	KTongProtocol.h
//	创建者		：	谢茂培 (Hsie)
//	创建时间	：	2003-08-13 15:12:19
//	功能描述	：	
//
// -------------------------------------------------------------------------
#ifndef __KTONGPROTOCOL_H__
#define __KTONGPROTOCOL_H__

#pragma pack(push, 1)

#define		defTONG_PROTOCOL_SERVER_NUM		255
#define		defTONG_PROTOCOL_CLIENT_NUM		255



//---------------------------- tong protocol ----------------------------
// relay server 收到的 game server 的协议
enum 
{
	enumC2S_TONG_CREATE = 0,			// 创建帮会
	enumC2S_TONG_ADD_MEMBER,			// 添加成员
	enumC2S_TONG_GET_HEAD_INFO,			// 获得帮会信息
	enumC2S_TONG_GET_MANAGER_INFO,		// 获得帮会队长信息
	enumC2S_TONG_GET_MEMBER_INFO,		// 获得帮会帮众信息
	enumC2S_TONG_INSTATE,				// 任命
	enumC2S_TONG_KICK,					// 踢人
	enumC2S_TONG_LEAVE,					// 离帮
	enumC2S_TONG_CHANGE_MASTER,			// 传位
	enumC2S_TONG_ACCEPT_MASTER,			// 是否接受传位
	enumC2S_TONG_GET_LOGIN_DATA,		// 玩家登陆时申请帮会数据
	enumC2S_TONG_CHANGE_TITLE,			// changetitle
	enumC2S_TONG_ACCEPT_TITLE,			// changetitle
	enumC2S_TONG_ACCEPT_SEX_TITLE,		// changetitle	
	enumC2S_TONG_MONEY_SAVE,
	enumC2S_TONG_MONEY_GET,
	enumC2S_TONG_MONEY_SND,
	enumC2S_TONG_CHANGE_CAMP,
	enumC2S_TONG_CHANGE_RECRUIT,
	enumC2S_GET_EXTPOINT,
	enumC2S_UPDATE_EXTPOINT,
	enumC2S_TONG_GET_LOGIN_LIMIT,		//#limit account 
	enumC2S_TONG_CHANGE_LEVEL,
	enumC2S_TONG_CHANGE_EXP,
	enumC2S_TONG_CHANGE_WAYEDIT,
	enumC2S_TONG_CHANGE_NEXTTARGER,
	// ==== JX2 port: cac lenh du lieu bang hoi kieu JX2 (chi them o CUOI, truoc _NUM) ====
	enumC2S_TONG_JX2_FIELD,		// dat/cong field bang (KV)
	enumC2S_TONG_JX2_MONEY,		// dat/cong ngan quy 64-bit (field 3/4)
	enumC2S_TONG_JX2_MEMBER_FIELD,	// dat/cong field thanh vien
	enumC2S_TONG_JX2_RIGHT,		// them/xoa quyen thanh vien
	enumC2S_TONG_JX2_GET_FULL,	// GS xin dump toan bo du lieu JX2
	enumC2S_TONG_JX2_STRING,	// ghi chuoi: thong bao bang / so su kien / so lich su
	enumC2S_TONG_JX2_TONG_OP,	// thao tac tong hop (init/upgrade/kick/stunt/map/phan phoi...)
	enumC2S_TONG_NUM,					// 数量
};

// relay server 发给 game server 的协议
enum
{
	enumS2C_TONG_CREATE_SUCCESS = 0,		// 帮会创建成功
	enumS2C_TONG_CREATE_FAIL,				// 帮会创建失败
	enumS2C_TONG_ADD_MEMBER_SUCCESS,		// 帮会添加成员成功
	enumS2C_TONG_ADD_MEMBER_FAIL,			// 帮会添加成员失败
	enumS2C_TONG_HEAD_INFO,					// 帮会信息
	enumS2C_TONG_MANAGER_INFO,				// 帮会队长信息
	enumS2C_TONG_MEMBER_INFO,				// 帮会队长信息
	enumS2C_TONG_BE_INSTATED,				// 被任命
	enumS2C_TONG_INSTATE,					// 任命成功或失败
	enumS2C_TONG_KICK,						// 踢人成功或失败
	enumS2C_TONG_BE_KICKED,					// 被踢出帮会
	enumS2C_TONG_LEAVE,						// 离开帮会成功或失败
	enumS2C_TONG_CHECK_CHANGE_MASTER_POWER,	// 判断是否有当帮主的能力
	enumS2C_TONG_CHANGE_MASTER_FAIL,		// 传位失败
	enumS2C_TONG_CHANGE_AS,					// 传位成功，身份改变
	enumS2C_TONG_CHANGE_MASTER,				// 广播，更换帮主
	enumS2C_TONG_LOGIN_DATA,				// 玩家登陆时候获得帮会数据
	enumS2C_TONG_CHANGE_TITLE_FAIL,			// changetitle
	enumS2C_TONG_CHECK_CHANGE_TITLE_POWER,	// changetitle
	enumS2C_TONG_BE_CHANGED_TITLE,			// changetitle
	enumS2C_TONG_MONEY_SAVE,
	enumS2C_TONG_MONEY_GET,
	enumS2C_TONG_MONEY_SND,
	enumS2C_TONG_BE_CHANGED_CAMP,
	enumS2C_TONG_CHANGE_CAMP_FAIL,
	enumS2C_SET_EXTPOINT,
	enumS2C_TONG_BE_CHANGED_RECRUIT,
	enumS2C_TONG_LOGIN_LIMIT,				 //#limit account 
	enumS2C_TONG_BE_CHANGED_LEVEL,
	enumS2C_TONG_BE_CHANGED_EXP,
	enumS2C_TONG_BE_CHANGED_WAYEDIT,
	enumS2C_TONG_BE_CHANGED_NEXTTARGET,
	enumS2C_TONG_FULL,
	// ==== JX2 port: dong bo du lieu bang hoi kieu JX2 ====
	enumS2C_TONG_JX2_FIELD_SYNC,	// echo field bang (op=SET, gia tri sau khi ghi)
	enumS2C_TONG_JX2_MONEY_SYNC,	// echo ngan quy 64-bit (gia tri tuyet doi)
	enumS2C_TONG_JX2_MEMBER_FIELD_SYNC,	// echo field thanh vien
	enumS2C_TONG_JX2_RIGHT_SYNC,	// echo quyen thanh vien
	enumS2C_TONG_JX2_TONG_SYNC,	// dump 1 bang: thong tin + toan bo field
	enumS2C_TONG_JX2_MEMBER_SYNC,	// dump <=4 thanh vien / goi
	enumS2C_TONG_JX2_TONG_REMOVE_SYNC,	// bang bien mat khoi relay
	enumS2C_TONG_JX2_SYNC_DONE,	// het dump toan cuc
	enumS2C_TONG_JX2_STRING_SYNC,	// echo thong bao bang toi moi GS
	enumS2C_TONG_NUM,					// 数量
};
//-------------------------- tong protocol end --------------------------

//friend protocol
enum 
{
	friend_c2c_askaddfriend,	//请求加为好友
	friend_c2c_repaddfriend,	//同意/拒绝加为好友
	friend_c2s_groupfriend,		//将好友分组
	friend_c2s_erasefriend,		//删除好友

	friend_c2s_asksyncfriendlist,	//请求同步好友列表
	friend_s2c_repsyncfriendlist,	//同步好友列表

	friend_s2c_friendstate,		//好友状态通知

	friend_c2s_associate,		//GS到Relay，自动组合2个人（有方向）
	friend_c2s_associatebevy,	//GS到Relay，自动组合n个人
	friend_s2c_syncassociate,	//Relay到Client，通知组合
};

//extend protocol
enum
{
	extend_s2c_passtosomeone,
	extend_s2c_passtobevy,
};



/////////////////////////////////////////////////////////////////
//friend struct

const int _GROUP_NAME_LEN = _NAME_LEN * 2;


struct ASK_ADDFRIEND_CMD : EXTEND_HEADER
{
	BYTE pckgid;
	char dstrole[_NAME_LEN];
};
struct ASK_ADDFRIEND_SYNC : EXTEND_HEADER
{
	BYTE pckgid;
	char srcrole[_NAME_LEN];
};

enum {answerAgree, answerDisagree, answerUnable};
struct REP_ADDFRIEND_CMD : EXTEND_HEADER
{
	BYTE pckgid;
	char dstrole[_NAME_LEN];
	BYTE answer;	//agree/disagree/unable
};
struct REP_ADDFRIEND_SYNC : EXTEND_HEADER
{
	BYTE pckgid;
	char srcrole[_NAME_LEN];
	BYTE answer;	//agree/disagree/unable
};



//used by GROUP_FRIEND & REP_SYNCFRIENDLIST
enum {specOver = 0x00, specGroup = 0x01, specRole = 0x02};

struct GROUP_FRIEND : EXTEND_HEADER
{
	//format: char seq
	//specGroup标记组，其后接该组好友列表，以\0间隔，specRole标记角色名
	//最后以双\0结束
};


struct ERASE_FRIEND : EXTEND_HEADER
{
	char friendrole[_NAME_LEN];
};


struct ASK_SYNCFRIENDLIST : EXTEND_HEADER
{
	BYTE pckgid;
	BYTE full;
};


struct REP_SYNCFRIENDLIST : EXTEND_HEADER
{
	BYTE pckgid;
	//format: char seq (same as GROUP_FRIEND)
	//specGroup标记组，其后接该组好友列表，以\0间隔，specRole标记角色名
	//最后以双\0结束
};


enum {stateOffline, stateOnline};

struct FRIEND_STATE : EXTEND_HEADER
{
	BYTE state;
	//format: char seq, \0间隔，双\0结束
};

struct FRIEND_ASSOCIATE : EXTEND_HEADER
{
	BYTE bidir;
	//format: string * 3
	//组名
	//角色名 * 2
};

struct FRIEND_ASSOCIATEBEVY : EXTEND_HEADER
{
	//format: char seq, \0间隔，双\0结束
	//组名
	//角色名列表
};

struct FRIEND_SYNCASSOCIATE : EXTEND_HEADER
{
	//format: char seq, \0间隔，双\0结束
	//组名
	//角色名列表
};


/////////////////////////////////////////////////////////////////
//extend struct

struct EXTEND_PASSTOSOMEONE : EXTEND_HEADER
{
	DWORD			nameid;
	unsigned long	lnID;
	WORD			datasize;
};

struct EXTEND_PASSTOBEVY : EXTEND_HEADER
{
	WORD	datasize;
	WORD	playercount;
	//data
	//tagPlusSrcInfo vector
};

//----------------------------- tong struct -----------------------------

struct STONG_CREATE_COMMAND : EXTEND_HEADER
{
	WORD	m_wLength;
	DWORD	m_dwPlayerNameID;
	DWORD	m_dwParam;
	BYTE	m_btCamp;
	BYTE	m_btTongNameLength;
	BYTE	m_btPlayerNameLength;
	char	m_szBuffer[64];
	BYTE	m_btSex;
};

struct STONG_CREATE_SUCCESS_SYNC : EXTEND_HEADER
{
	WORD	m_wLength;
	DWORD	m_dwPlayerNameID;
	DWORD	m_dwParam;
	BYTE	m_btCamp;
	BYTE	m_btTongNameLength;
	char	m_szTongName[32];
};

struct STONG_CREATE_FAIL_SYNC : EXTEND_HEADER
{
	DWORD	m_dwPlayerNameID;
	DWORD	m_dwParam;
	BYTE	m_btFailID;
};

struct STONG_ADD_MEMBER_COMMAND : EXTEND_HEADER
{
	WORD	m_wLength;
	DWORD	m_dwPlayerNameID;
	DWORD	m_dwParam;
	BYTE	m_btCamp;
	BYTE	m_btSex;			// fix doi ten nhan vat nu
	BYTE	m_btTongNameLength;
	BYTE	m_btPlayerNameLength;
	char	m_szBuffer[64];
};

struct STONG_ADD_MEMBER_SUCCESS_SYNC : EXTEND_HEADER
{
	DWORD	m_dwPlayerNameID;
	DWORD	m_dwParam;
	BYTE	m_btCamp;
	char	m_szTongName[32];
	char	m_szMasterName[32];
	char	m_szTitleName[32];
	int		m_nTongLevel;
	int		m_nTongExp;
	BOOL	m_nIsFull;
};

struct STONG_ADD_MEMBER_FAIL_SYNC : EXTEND_HEADER
{
	DWORD	m_dwPlayerNameID;
	DWORD	m_dwParam;
	BYTE	m_btFailID;
};

struct STONG_GET_TONG_HEAD_INFO_COMMAND : EXTEND_HEADER
{
	DWORD	m_dwParam;
	DWORD	m_dwNpcID;
	DWORD	m_dwTongNameID;
};

struct STONG_GET_MANAGER_INFO_COMMAND : EXTEND_HEADER
{
	DWORD	m_dwParam;
	int		m_nParam1;
	int		m_nParam2;
	int		m_nParam3;
};

struct STONG_GET_MEMBER_INFO_COMMAND : EXTEND_HEADER
{
	DWORD	m_dwParam;
	int		m_nParam1;
	int		m_nParam2;
	int		m_nParam3;
};

struct STONG_ONE_LEADER_INFO
{
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szTitle[32];
	char	m_szName[32];
	BYTE	m_btOnline;		// changetitle
};

struct STONG_ONE_MEMBER_INFO
{
	char	m_szName[32];
	BYTE	m_btSex;		// changetitle
	BYTE	m_btOnline;
};

struct STONG_HEAD_INFO_SYNC : EXTEND_HEADER
{
	WORD	m_wLength;
	DWORD	m_dwParam;
	DWORD	m_dwNpcID;
	DWORD	m_dwMoney;
	int		m_nCredit;
	BYTE	m_btCamp;
	BYTE	m_btLevel;
	BYTE	m_btDirectorNum;
	BYTE	m_btManagerNum;
	DWORD	m_dwMemberNum;
	char	m_szTongName[32];
	//add by Fong Ki襲 use
	int		 ms_nStatusGuide;
	char	ms_szWayEdit[32];
	char	ms_szNextTargetEdit[32];
	int		 ms_nExpGuide;
	int		ms_nCityGuide;
	int		ms_nTongLevel;
	char	ms_szLeagueTName[32];
	STONG_ONE_LEADER_INFO	m_sMember[1 + defTONG_MAX_DIRECTOR];
};

struct STONG_MANAGER_INFO_SYNC : EXTEND_HEADER
{
	WORD	m_wLength;
	DWORD	m_dwParam;
	DWORD	m_dwMoney;
	int		m_nCredit;
	BYTE	m_btCamp;
	BYTE	m_btLevel;
	BYTE	m_btDirectorNum;
	BYTE	m_btManagerNum;
	DWORD	m_dwMemberNum;
	BYTE	m_btStartNo;
	BYTE	m_btCurNum;
	char	m_szTongName[32];
	STONG_ONE_LEADER_INFO	m_sMember[defTONG_ONE_PAGE_MAX_NUM];
};

struct STONG_MEMBER_INFO_SYNC : EXTEND_HEADER
{
	WORD	m_wLength;
	DWORD	m_dwParam;
	DWORD	m_dwMoney;
	int		m_nCredit;
	BYTE	m_btCamp;
	BYTE	m_btLevel;
	BYTE	m_btDirectorNum;
	BYTE	m_btManagerNum;
	DWORD	m_dwMemberNum;
	BYTE	m_btStartNo;
	BYTE	m_btCurNum;
	char	m_szTitle[32];
	char	m_szTongName[32];
	char	m_szTitleBoy[32];		// changetitle
	char	m_szTitleGirl[32];		// changetitle
	STONG_ONE_MEMBER_INFO	m_sMember[defTONG_ONE_PAGE_MAX_NUM];
};

struct STONG_INSTATE_COMMAND : EXTEND_HEADER
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btCurFigure;
	BYTE	m_btCurPos;
	BYTE	m_btNewFigure;
	BYTE	m_btNewPos;
	char	m_szName[32];
};

struct STONG_BE_INSTATED_SYNC : EXTEND_HEADER
{
	DWORD	m_dwParam;			// 网络连接号
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szTitle[32];
	char	m_szName[32];
};

struct STONG_BE_KICKED_SYNC : EXTEND_HEADER
{
	DWORD	m_dwParam;			// 网络连接号
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
};

struct STONG_BE_CHANGED_TITLE_SYNC : EXTEND_HEADER		// changetitle
{
	DWORD	m_dwParam;			//	changetitle
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
	char	m_szTitle[32];
};

struct STONG_BE_CHANGED_CAMP_SYNC : EXTEND_HEADER		// changetitle
{
	DWORD	m_dwParam;			//	changetitle
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
	char	m_szTitle[32];
	BYTE	m_btCamp;
};

struct STONG_BE_CHANGED_RECRUIT_SYNC : EXTEND_HEADER		// changetitle
{
	DWORD	m_dwParam;			//	changetitle
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
	char	m_szTitle[32];
	BYTE	m_btRecruit;
};

struct STONG_BE_CHANGED_LEVEL_SYNC : EXTEND_HEADER		// changetitle
{
	DWORD	m_dwParam;			//	changetitle
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
	char	m_szTitle[32];
	int		m_nTongLevel;
};

struct STONG_BE_CHANGED_EXP_SYNC : EXTEND_HEADER		// changetitle
{
	DWORD	m_dwParam;			//	changetitle
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
	char	m_szTitle[32];
	int		m_nTongExp;
};

struct STONG_BE_CHANGED_WAYEDIT_SYNC : EXTEND_HEADER		// changetitle
{
	DWORD	m_dwParam;			//	changetitle
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
	char	m_szTitle[32];
	char	m_szWayEdit[32];
};

struct STONG_BE_CHANGED_NEXTTARGET_SYNC : EXTEND_HEADER		// changetitle
{
	DWORD	m_dwParam;			//	changetitle
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
	char	m_szTitle[32];
	char	m_szNextTarget[32];
};

struct STONG_INSTATE_SYNC : EXTEND_HEADER
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btSuccessFlag;
	BYTE	m_btOldFigure;
	BYTE	m_btOldPos;
	BYTE	m_btNewFigure;
	BYTE	m_btNewPos;
	char	m_szTitle[32];
	char	m_szName[32];
};

struct STONG_KICK_COMMAND : EXTEND_HEADER
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
};

struct STONG_KICK_SYNC : EXTEND_HEADER
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btSuccessFlag;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
};

struct STONG_LEAVE_COMMAND : EXTEND_HEADER
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
};

struct STONG_LEAVE_SYNC : EXTEND_HEADER
{
	DWORD	m_dwParam;
	BYTE	m_btSuccessFlag;
	char	m_szName[32];
};

struct STONG_CHANGE_MASTER_COMMAND : EXTEND_HEADER
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
};

struct STONG_CHANGE_TITLE_COMMAND : EXTEND_HEADER		// changetitle
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
	char	m_szTitle[32];
};

struct STONG_CHANGE_CAMP_COMMAND : EXTEND_HEADER		// changetitle
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
	char	m_szTitle[32];
	BYTE	m_btCamp;
	DWORD	m_nMoney;
};

struct STONG_CHANGE_RECRUIT_COMMAND : EXTEND_HEADER		// changetitle
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
	char	m_szTitle[32];
	BYTE	m_btRecruit;
};

struct STONG_CHANGE_LEVEL_COMMAND : EXTEND_HEADER		// changeLevel
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
	char	m_szTitle[32];
	int		m_nTongLevel;
};

struct STONG_CHANGE_EXP_COMMAND : EXTEND_HEADER		// changeExp
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
	char	m_szTitle[32];
	int		m_nTongExp;
};

struct STONG_CHANGE_WAYEDIT_COMMAND : EXTEND_HEADER		// changetitle
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
	char	m_szTitle[32];
	char	m_szWayEdit[32];
};

struct STONG_CHANGE_NEXTTARGET_COMMAND : EXTEND_HEADER		// changetitle
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
	char	m_szTitle[32];
	char	m_szNextTarget[32];
};

struct STONG_CHANGE_CAMPE_COMMAND : EXTEND_HEADER		// changetitle
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
	char	m_szTitle[32];
};

struct STONG_CHECK_GET_MASTER_POWER_SYNC : EXTEND_HEADER
{
	DWORD	m_dwParam;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	DWORD	m_dwTongNameID;
	char	m_szName[32];
};

struct STONG_CHANGE_MASTER_FAIL_SYNC : EXTEND_HEADER
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btFailID;
	char	m_szName[32];
};

struct STONG_CHECK_GET_TITLE_POWER_SYNC : EXTEND_HEADER			// changetitle
{
	DWORD	m_dwParam;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	DWORD	m_dwTongNameID;
	char	m_szName[32];
	char	m_szTitle[32];
};

struct STONG_CHANGE_TITLE_FAIL_SYNC : EXTEND_HEADER			// changetitle
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btFailID;
	char	m_szName[32];
	char	m_szTitle[32];
};

struct STONG_CHANGE_CAMP_FAIL_SYNC : EXTEND_HEADER			// changecamp
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btFailID;
	char	m_szName[32];
	char	m_szTitle[32];
};

struct STONG_CHANGE_AS_SYNC : EXTEND_HEADER
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szTitle[32];		// 自己的新头衔
	char	m_szName[32];		// 新帮主的名字
};

struct STONG_ACCEPT_MASTER_COMMAND : EXTEND_HEADER
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	BYTE	m_btAcceptFalg;
	char	m_szName[32];
};

struct STONG_ACCEPT_TITLE_COMMAND : EXTEND_HEADER			// changetitle
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	BYTE	m_btAcceptFalg;
	char	m_szName[32];
	char	m_szTitle[32];
};

struct STONG_ACCEPT_SEX_TITLE_COMMAND : EXTEND_HEADER			// changetitle
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	BYTE	m_btSex;
	char	m_szTitle[32];
};

struct STONG_CHANGE_MASTER_SYNC : EXTEND_HEADER
{
	DWORD	m_dwTongNameID;
	char	m_szName[32];
};

struct STONG_GET_LOGIN_DATA_COMMAND : EXTEND_HEADER
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	char	m_szName[32];
	BYTE	m_nSex;	
	int	m_Recruit;
};

struct STONG_GET_LOGIN_LIMIT_COMMAND : EXTEND_HEADER
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID; //#mapping nIdx
	char	m_szName[64];				//#mapping sHWID
}; //#limit account 

struct STONG_RETURN_LOGIN_LIMIT_COMMAND : EXTEND_HEADER
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID; //#mapping nIdx
	char	m_szName[64];				//#mapping sHWID
	BYTE	num_login;
}; //#limit account 

struct STONG_LOGIN_DATA_SYNC : EXTEND_HEADER
{
	DWORD	m_dwParam;
	BYTE	m_btFlag;
	BYTE	m_btCamp;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szTongName[32];
	char	m_szTitle[32];
	char	m_szMaster[32];
	char	m_szName[32];
	int		m_btRecruit;
	DWORD	m_nMoney;
	int		m_nLevel;
	int		m_nExp;
	bool	m_bIsFull;
};

struct STONG_MONEY_COMMAND : EXTEND_HEADER
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	DWORD	m_dwMoney;
	char	m_szName[32];
};

struct STONG_MONEY_SYNC : EXTEND_HEADER
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	char	m_szName[32];
	DWORD	m_nMoney;
	DWORD	m_dwMoney;
};

struct STONG_FULL_SYNC : EXTEND_HEADER
{
	DWORD	m_dwParam;
	DWORD	m_dwTongNameID;
	char	m_szName[32];
	BOOL	m_bIsFull;
};

struct STONG_UPDATE_EXTPOINT_COMMAND : EXTEND_HEADER
{
	char 	m_szAccountName[32];
	DWORD	m_dwParam;
	int 	m_nExtPoint;
};

struct STONG_GET_EXTPOINT_COMMAND : EXTEND_HEADER
{
	char 	m_szAccountName[32];
	DWORD	m_dwParam;
};

struct STONG_GET_EXTPOINT_SYNC : EXTEND_HEADER
{
	int 	m_nExtPoint;
	DWORD	m_dwParam;
};

// ==== JX2 port: cau truc goi du lieu bang hoi kieu JX2 ====

#define defTONG_JX2_OP_SET		0
#define defTONG_JX2_OP_ADD		1	// cong CO dau
#define defTONG_JX2_OP_ADDU	2	// cong KHONG dau

// Suc chua luu tru phia relay/DB (mang them o CUOI TTongStruct/TMemberStruct)
#define defTONG_JX2_MAX_FIELDS		192	// KV cap bang (field 1..48 + bien nhiem vu >=1002 + tac phuong 20000+/30000+)
#define defTONG_JX2_MEMBER_FIELDS	32	// KV cap thanh vien (khoa 1..16 + >=1001)
#define defTONG_JX2_MEMBER_RIGHTS	32	// so quyen toi da moi thanh vien

// Loai chuoi cua goi JX2_STRING
#define defTONG_JX2_STR_ANNOUNCE	0	// thong bao bang (dong bo toi GS)
#define defTONG_JX2_STR_EVENT		1	// so su kien (chi luu tren relay)
#define defTONG_JX2_STR_HISTORY	2	// so lich su (chi luu tren relay)
#define defTONG_JX2_STR_RECRUIT	3	// van an chieu mo (dong bo toi GS)
#define defTONG_JX2_STR_UNION		4	// ten lien minh (dong bo toi GS, luu m_szLeagueTName)
#define defTONG_JX2_ANNOUNCE_LEN	128
#define defTONG_JX2_RECORD_LEN		96
#define defTONG_JX2_RECORD_NUM		16

struct STONG_JX2_FIELD_COMMAND : EXTEND_HEADER
{
	DWORD	m_dwTongNameID;
	WORD	m_wKey;
	DWORD	m_dwValue;		// SET: gia tri moi; ADD/ADDU: so gia (ep kieu)
	BYTE	m_btOp;			// defTONG_JX2_OP_*
	DWORD	m_dwParam;		// player idx phia GS (0 = engine/script)
};

struct STONG_JX2_MONEY_COMMAND : EXTEND_HEADER
{
	DWORD	m_dwTongNameID;
	__int64	m_nValue;		// SET: gia tri; ADD: so gia (am duoc)
	BYTE	m_btOp;
	DWORD	m_dwParam;
};

struct STONG_JX2_MEMBER_FIELD_COMMAND : EXTEND_HEADER
{
	DWORD	m_dwTongNameID;
	DWORD	m_dwMemberNameID;
	WORD	m_wKey;
	DWORD	m_dwValue;
	BYTE	m_btOp;
	DWORD	m_dwParam;
};

struct STONG_JX2_RIGHT_COMMAND : EXTEND_HEADER
{
	DWORD	m_dwTongNameID;
	DWORD	m_dwMemberNameID;
	DWORD	m_dwRightID;
	BYTE	m_btAdd;		// 1 = them, 0 = xoa
	DWORD	m_dwParam;
};

struct STONG_JX2_GET_FULL_COMMAND : EXTEND_HEADER
{
	DWORD	m_dwParam;
};

// Dump 1 bang (goi bien dai, m_wLength = tong kich thuoc goi).
// Ngay sau struct: m_wFieldCount x { WORD wKey; DWORD dwValue; }
struct STONG_JX2_TONG_SYNC : EXTEND_HEADER
{
	WORD	m_wLength;
	DWORD	m_dwTongNameID;
	char	m_szTongName[32];
	BYTE	m_btCamp;
	WORD	m_wMemberTotal;	// tong so thanh vien se toi qua MEMBER_SYNC
	WORD	m_wFieldCount;
	char	m_szAnnounce[defTONG_JX2_ANNOUNCE_LEN];	// thong bao bang
};

// 1 muc thanh vien trong goi MEMBER_SYNC; sau phan co dinh la
// m_btFieldCount x {WORD,DWORD} roi m_btRightCount x DWORD
struct STONG_JX2_ONE_MEMBER
{
	DWORD	m_dwMemberNameID;
	char	m_szName[32];
	BYTE	m_btFigure;	// 0 bang chu/1 truong lao/2 doi truong/3 bang chung/4 an si
	BYTE	m_btSex;
	BYTE	m_btFieldCount;
	BYTE	m_btRightCount;
};

// Bo cuc: header + m_btCount x [STONG_JX2_ONE_MEMBER + fields + rights]
struct STONG_JX2_MEMBER_SYNC : EXTEND_HEADER
{
	WORD	m_wLength;
	DWORD	m_dwTongNameID;
	BYTE	m_btCount;
};

struct STONG_JX2_TONG_REMOVE_SYNC : EXTEND_HEADER
{
	DWORD	m_dwTongNameID;
};

struct STONG_JX2_SYNC_DONE : EXTEND_HEADER
{
	DWORD	m_dwTongCount;
};

// (defTONG_JX2_STR_* / ANNOUNCE_LEN / RECORD_* da don len truoc khoi struct)

// Bien nhiem vu bang do engine JX1 tu quan ly (>48, khong dung voi 1002..1047 cua script JX2)
#define defTONGTSK_STUNT_ID		1101	// tuyet ky dang dat
#define defTONGTSK_STUNT_ENABLED	1102	// tuyet ky con hieu luc (bao tri du tien)
#define defTONGTSK_WEEK_WFCONSUME	1103	// thong ke chien bi tieu trong tuan
#define defTONGTSK_LAST_WM_DAY		1104	// so-ngay (epoch) chay bao tri TUAN gan nhat
#define defTONGTSK_LAST_M_DAY		1105	// so-ngay (epoch) chay bao tri NGAY gan nhat

// Tac phuong (workshop) ma hoa vao dai field:
//   thuoc tinh:  defTONG_JX2_WS_ATTR_BASE + nType*10 + attr (0=ton tai 1=mo 2=cap
//                3=san luong ngay 4=cap su dung 5=bo cap su dung)
//   bien nhiem vu: defTONG_JX2_WS_TASK_BASE + nType*1000 + key (key < 1000)
#define defTONG_JX2_WS_ATTR_BASE	20000
#define defTONG_JX2_WS_TASK_BASE	30000
#define defTONG_JX2_WS_MAX_TYPE		7

struct STONG_JX2_STRING_COMMAND : EXTEND_HEADER
{
	DWORD	m_dwTongNameID;
	BYTE	m_btKind;		// defTONG_JX2_STR_*
	char	m_szText[defTONG_JX2_ANNOUNCE_LEN];
	DWORD	m_dwParam;
};

// Ma thao tac cua goi JX2_TONG_OP
#define defTONG_JX2_TOP_INIT			0	// xoa trang du lieu JX2 cua bang
#define defTONG_JX2_TOP_UPGRADE		1	// nang cap bang (field 13) theo tong_level_data
#define defTONG_JX2_TOP_DEGRADE		2	// ha cap bang
#define defTONG_JX2_TOP_MAINTAIN		3	// bao tri ngay (thuong do relay tu chay)
#define defTONG_JX2_TOP_WEEKLY			4	// bao tri tuan (don WeekGoal -> LWeekGoal)
#define defTONG_JX2_TOP_KICK			5	// duoi thanh vien (m_dwMemberNameID)
#define defTONG_JX2_TOP_SET_STUNT		6	// dat tuyet ky (nParam1 = StuntID)
#define defTONG_JX2_TOP_SET_MAP		7	// dat ban do bang (field 45; nParam1 = map)
#define defTONG_JX2_TOP_CREATE_MAP		8	// tao ban sao ban do (field 45/46)
#define defTONG_JX2_TOP_DELETE_MAP		9	// xoa ban do bang (xoa field 45/46)
#define defTONG_JX2_TOP_CONTRIBUTE		10	// cong nParam1 vao quy du tru (field 18)
#define defTONG_JX2_TOP_DIST_GROUP		11	// phat nParam1 cong hien cho ca nhom chuc vu nParam2
#define defTONG_JX2_TOP_DIST_MEMBER	12	// phat nParam1 cong hien cho m_dwMemberNameID
#define defTONG_JX2_TOP_ADD_OFFER_FIG	13	// cong nParam1 cong hien cho tung nguoi nhom nParam2 (AddOfferEx)
#define defTONG_JX2_TOP_FEATURE		14	// doi ngoai hinh toan bang (nParam1 = feature, nParam2 = giay)
#define defTONG_JX2_TOP_SET_FIGURE	15	// doi chuc vu thanh vien (nParam1 = figure moi 1..3)
#define defTONG_JX2_TOP_SET_CAMP	16	// doi phe bang hoi (nParam1 = 1 chinh / 2 ta / 3 trung lap)
#define defTONG_JX2_TOP_UNION_CREATE	17	// lap lien minh (m_szName = ten lien minh)
#define defTONG_JX2_TOP_UNION_JOIN	18	// minh chu duyet bang m_szName vao lien minh (tru 100 van MOI ben)
#define defTONG_JX2_TOP_UNION_LEAVE	19	// roi lien minh; minh chu roi = giai tan (field 49 = now)
#define defTONG_JX2_TOP_UNION_KICK	20	// minh chu truc xuat bang m_szName
#define defTONG_JX2_TOP_MINISTER	21	// dai than: nParam1 = 1 ThuaTuong/2 NguyenSoai/3 TienPhong -> field 50+slot; nParam2 = 1 phong / 0 cach

struct STONG_JX2_TONG_OP_COMMAND : EXTEND_HEADER
{
	DWORD	m_dwTongNameID;
	DWORD	m_dwMemberNameID;	// 0 neu khong dung
	BYTE	m_btOpCode;		// defTONG_JX2_TOP_*
	int	m_nParam1;
	int	m_nParam2;
	DWORD	m_dwParam;
	char	m_szName[32];	// ten lien minh / ten bang dich (them CUOI struct)
};

//--------------------------- tong struct end ---------------------------


#pragma pack(pop)

#endif // __KTONGPROTOCOL_H__
