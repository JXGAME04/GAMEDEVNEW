/*****************************************************************************************
//	Copyright : Kingsoft 2020
//	Author	:   Fong Ki“u
//	CreateTime:	2020-8-13
*****************************************************************************************/
#pragma once
#include "LoginDef.h"
#include "../NetConnect/NetMsgTargetObject.h"

struct TProcessData;
struct tagNewDelRoleResponse;
struct tagNotifyPlayerLogin;

enum LOGIN_LOGIC_STATUS
{
	LL_S_IDLE = 0,					
	LL_S_WAIT_INPUT_ACCOUNT,		
	LL_S_ACCOUNT_CONFIRMING,		
	LL_S_WAIT_ROLE_LIST,			
	LL_S_ROLE_LIST_READY,			
	LL_S_CREATING_ROLE,				
	LL_S_DELETING_ROLE,				
	LL_S_WAIT_TO_LOGIN_GAMESERVER,	
	LL_S_ENTERING_GAME,				
	LL_S_IN_GAME,					
};

enum LOGIN_LOGIC_RESULT_INFO
{
	LL_R_NOTHING,					
	LL_R_CONNECT_FAILED,			
	LL_R_CONNECT_SERV_BUSY,			
	LL_R_CONNECT_TIMEOUT,			
	LL_R_ACCOUNT_PWD_ERROR,			
	LL_R_ACCOUNT_FREEZE,			
	LL_R_ACCOUNT_LOCKED,			
	LL_R_INVALID_ROLENAME,			
	LL_R_SERVER_SHUTDOWN,			
	LL_R_INVALID_PROTOCOLVERSION,	
	LL_R_INVALID_PASSWORD,			
	LL_R_ACCOUNT_CONFIRM_SUCCESS,	
	LL_R_CREATE_ROLE_SUCCESS,		
	LL_R_LOGIN_TO_GAMESERVER,		
	LL_R_ACCOUNT_NOT_ENOUGH_POINT,	
	LL_LOGIN_R_LIMIT_GAME,	//LOGIN_R_LIMIT_GAME
};

#ifndef MAX_PLAYER_PER_ACCOUNT
#define	MAX_PLAYER_PER_ACCOUNT	3
#endif

struct KRoleChiefInfo
{
	char				Name[32];		
	unsigned	char	Gender;			//	be one of the SSC_CHARACTER_GENDER value
	unsigned	char	Attribute;		
	union
	{
		unsigned short	NativePlaceId;	
		short			nLevel;			
	};
};

struct KLoginServer
{
	char			Title[32];		
	unsigned char	Address[4];		
};

class KLogin : public iKNetMsgTargetObject
{
public:
	KLogin();								
	~KLogin();								

	void	AcceptNetMsg(void* pMsgData);	
	int	CreateConnection(const unsigned char* pAddress);
	int	AccountLogin(const char* pAccount, const KSG_PASSWORD& crPassword, bool bOrignPassword = true);
	int	SelectRole(int nIndex);
	int	CreateRole(KRoleChiefInfo* pCreateInfo);
	int	DeleteRole(int nIndex, const KSG_PASSWORD &crSupperPassword);
	void NotifyTimeout();
	void NotifyToStartGame();
	void NotifyDisconnect();
	void ReturnToIdle();
	void AutoLogin();
	int	IsAutoLoginEnable();
	void SetRememberAccountFlag(bool bEnable);
	void SetRememberAllFlag(bool bEnable);
	LOGIN_LOGIC_STATUS GetStatus() { return m_Status; }
	LOGIN_LOGIC_RESULT_INFO GetResult();
	int		GetRoleCount(int& nAdviceChoice);
	int		GetRoleInfo(int nIndex, KRoleChiefInfo* pInfo);
	void	SaveLoginChoice();
	void	LoadLoginChoice();
	KLoginServer*	GetServerRegionList(int& nCount, int& nAdviceChoice);
	KLoginServer*	GetServerList(int nRegion, int& nCount, int& nAdviceChoice);
	bool	GetLoginAccount(char* pszAccount);
	DWORD	GetAccountLifeTime() { return m_LeftTime; }
	int		IsRoleNewCreated() { return m_Choices.bIsRoleNewCreated; }
    int     SetAccountServer(const KLoginServer &rcSelectServer);

private:

	void	ProcessAccountLoginResponse(KLoginStructHead* pResponse);		
	void	ProcessRoleListResponse(TProcessData* pResponse);				
	void	ProcessDeleteRoleResponse(tagNewDelRoleResponse* pResponse);	
	void	ProcessCreateRoleResponse(tagNewDelRoleResponse* pResponse);	
	void	ProcessToLoginGameServResponse(tagNotifyPlayerLogin* pResponse);
	int		Request(const char* pszAccount, const KSG_PASSWORD* pcPassword, int nAction);
	int		ConnectAccountServer(const unsigned char* pIpAddress);
	void	RegistNetAgent();
	void	UnRegistNetAgent();
	void	GetAccountPassword(char* pszAccount, KSG_PASSWORD* pPassword);
	void	SetAccountPassword(const char* pszAccount, const KSG_PASSWORD* pcPassword);
	void	ClearAccountPassword(bool bAccount, bool bPassword);

private:
	LOGIN_LOGIC_STATUS			m_Status;
	LOGIN_LOGIC_RESULT_INFO		m_Result;
	
	bool						m_bInAutoProgress;				
	bool						m_bReserved;
	short						m_nNumRole;						
	KRoleChiefInfo				m_RoleList[MAX_PLAYER_PER_ACCOUNT];
	DWORD						m_LeftTime;

	struct	LOGIN_CHOICE
	{
		int						nServerRegionIndex;			
		KLoginServer			AccountServer;				
		char					Account[32];				
		KSG_PASSWORD    		Password;				
		char					szProcessingRoleName[32];	
		bool					bRememberAccount;			
		bool					bRememberAll;				
		bool					bAutoLoginEnable;			
		bool					bIsRoleNewCreated;			
		bool					bLoaded;					
	}							m_Choices;
};

extern	KLogin		g_LoginLogic;
