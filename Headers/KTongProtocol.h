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

//--------------------------- tong struct end ---------------------------


#pragma pack(pop)

#endif // __KTONGPROTOCOL_H__
