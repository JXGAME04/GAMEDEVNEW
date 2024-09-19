/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong Ki“u
//	CreateTime:	2020-10-6
*****************************************************************************************/
#pragma once
//#include "KNetClient.h"
#include "../../../Headers/KProtocol.h"
#include "../../../Headers/iClient.h"

struct iKNetMsgTargetObject;
typedef void (*fnNetMsgCallbackFunc)(void* pMsgData);

typedef HRESULT ( __stdcall * pfnCreateClientInterface )(
			REFIID	riid,
			void	**ppv
		);


#define	DEF_TIMEOUT_LIMIT	60000	//60sec

class KNetConnectAgent
{
public:
	KNetConnectAgent();
	~KNetConnectAgent();

	int		Initialize();

	void	Exit();

	int		ClientConnectByNumericIp(const unsigned char* pIpAddress, unsigned short pszPort);

	void	DisconnectClient();

	int		ConnectToGameSvr(const unsigned char* pIpAddress, unsigned short uPort, GUID* pGuid);

	void	DisconnectGameSvr();

	int		SendMsg(const void *pBuffer, int nSize);

	void	Breathe();

	void	UpdateClientRequestTime(bool bCancel, unsigned int uTimeLimit = DEF_TIMEOUT_LIMIT);

	void	RegisterMsgTargetObject(PROTOCOL_MSG_TYPE Msg, iKNetMsgTargetObject* pObject);

	int		IsConnecting(int bGameServ);

	void	TobeDisconnect();

private:
	bool	ProcessSwitchGameSvrMsg(void* pMsgData);		

private:
	IClient*				m_pClient;
	IClient*				m_pGameSvrClient;

private:
#define	MAX_MSG_COUNT	1 << (PROTOCOL_MSG_SIZE * 8)
	iKNetMsgTargetObject*	m_MsgTargetObjs[MAX_MSG_COUNT];

	HMODULE					    m_hModule;
	pfnCreateClientInterface    m_pFactroyFun;
	IClientFactory             *m_pClientFactory;

	bool					m_bIsClientConnecting;
	bool					m_bIsGameServConnecting;
	bool					m_bTobeDisconnect;
	unsigned int			m_uClientRequestTime;		
	unsigned int			m_uClientTimeoutLimit;
};

extern KNetConnectAgent g_NetConnectAgent;