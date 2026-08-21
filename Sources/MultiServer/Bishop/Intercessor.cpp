#include "stdafx.h"
#include "Intercessor.h"

#include "AccountLoginDef.h"
#include "LoginDef.h"

#include "KProtocolDef.h"
#include "KProtocol.h"

#include "Macro.h"
#include "Buffer.h"

#include "SmartClient.h"

#include <fstream>
#include <ctime>
#include <sstream>

using OnlineGameLib::Win32::CEvent;
using OnlineGameLib::Win32::CCriticalSection;
using OnlineGameLib::Win32::CPackager;

OnlineGameLib::Win32::CLibrary	CIntercessor::m_theHeavenLib( "heaven.dll" );
OnlineGameLib::Win32::CLibrary	CIntercessor::m_theRainbowLib( "rainbow.dll" );

CIntercessor::CIntercessor( unsigned long lnMaxPlayerCount, CNetwork &theNetworkConfig )
		: m_hwndViewer( NULL )
//		, m_pAccSvrClient( NULL )
		, m_pDBRoleClient( NULL )
		, m_pGameSvrServer( NULL )
		, m_pPlayerServer( NULL )
		, m_hWorkingThread( INVALID_HANDLE_VALUE )
		, m_hHelperThread( INVALID_HANDLE_VALUE )
		, m_hQuitEvent( NULL, true, false, NULL /*"PM_Quit_Event"*/ )
		, m_hStartupWoringThreadEvent( NULL, false, false, NULL /*"PM_WORKING_EVENT"*/ )
		, m_hStartupHelperThreadEvent( NULL, false, false, NULL /*"PM_HELPER_EVENT"*/ )
		, m_theNetworkConfig( theNetworkConfig )
		, m_lnMaxPlayerCount( lnMaxPlayerCount )
	    , m_maxConnectionsPerIP(10)	// Default to 5 connections per IP
{

}

CIntercessor::~CIntercessor()
{

}

bool CIntercessor::Create( HWND hwndViewer /* NULL */ )
{
	m_hwndViewer = hwndViewer;
	
	/*
	 * (20/08) PHAI dat lai cac su kien truoc khi tao luong.
	 *
	 * m_hQuitEvent duoc tao kieu MANUAL-RESET (:34, tham so thu 2 = true) va chi
	 * duoc Set() trong Destroy() (:98) -- KHONG noi nao Reset(). Hai vong lam viec
	 * deu la `while ( !m_hQuitEvent.Wait( 0 ) )` (:625 va :734).
	 *
	 * Khong co doan nay thi: Create() hong mot lan (vi du ConnectTo(RoleSvr) o :389
	 * that bai khien StartupNetwork() tra false o :505) -> Application.cpp:521-526
	 * goi Destroy() tuc Set() co thoat. Nguoi van hanh bam OK de thu lai
	 * (Application.cpp:700 goi lai Create()): cong 5622/5632 mo lai duoc NHUNG hai
	 * luong moi thoat ngay lap tuc vi co thoat van bat. Bishop thanh "xac song" --
	 * cong mo ma khong xu ly gi, khong phat c2s_ping, nen may chu tai khoan cat ket
	 * noi moi 20 giay (S3PDBSocketPool.cpp:21 PING_TIME 20000), nguoi choi khong vao
	 * duoc trong khi nhin ben ngoai van tuong binh thuong.
	 */
	m_hQuitEvent.Reset();
	m_hStartupWoringThreadEvent.Reset();
	m_hStartupHelperThreadEvent.Reset();
	
	/*
	 * Start a working thread
	 */
	DWORD dwThreadID = 0;

	m_hWorkingThread = ::CreateThread( NULL, 
		0, 
		WorkingThreadFunction, 
		( void * )this, 
		0, 
		&dwThreadID );
			
	// CreateThread that bai tra NULL, KHONG phai INVALID_HANDLE_VALUE (do la quy
	// uoc cua CreateFile). So sai thi nhanh nay la ma chet, khong bao gio bat
	// duoc loi tao luong.
	if ( m_hWorkingThread == NULL )
	{
		return false;
	}

	/*
	 * Startup a helper thread
	 */
	dwThreadID = 0;

	m_hHelperThread = ::CreateThread( NULL, 
		0, 
		HelperThreadFunction, 
		( void * )this,
		0, 
		&dwThreadID );
			
	// CreateThread that bai tra NULL, KHONG phai INVALID_HANDLE_VALUE (do la quy
	// uoc cua CreateFile). So sai thi nhanh nay la ma chet, khong bao gio bat
	// duoc loi tao luong.
	if ( m_hHelperThread == NULL )
	{
		return false;
	}

//	m_pAccSvrClient = m_theNetworkConfig.CreateAccSvrClient( m_theNetworkConfig.GetAccSvrIP(), m_theNetworkConfig.GetAccSvrPort() );

//	ASSERT( m_pAccSvrClient );

//	m_pAccSvrClient->RegisterMsgFilter( ( void * )this, AccountEventNotify );

	return StartupNetwork();
}

void CIntercessor::Destroy()
{
	m_hQuitEvent.Set();

	m_hStartupWoringThreadEvent.Set();
	m_hStartupHelperThreadEvent.Set();

	SAFE_CLOSEHANDLE( m_hWorkingThread );
	SAFE_CLOSEHANDLE( m_hHelperThread );

//	SAFE_RELEASE( m_pAccSvrClient );

	CleanNetwork();
}
/*
void __stdcall CIntercessor::AccountEventNotify( LPVOID lpParam, 
						const unsigned long &ulnEventType )
{
	CIntercessor *pPlayerManager = reinterpret_cast< CIntercessor * >( lpParam );

	ASSERT( pPlayerManager );

	try
	{
		pPlayerManager->_AccountEventNotify( ulnEventType );
	}
	catch(...)
	{
		TRACE( "CIntercessor::AccountEventNotify exception!" );
	}
}

void CIntercessor::_AccountEventNotify( const unsigned long &ulnEventType )
{
	switch( ulnEventType )
	{
	case enumServerConnectCreate:

		::PostMessage( m_hwndViewer, WM_SERVER_STATUS, ACCOUNTSERVER_NOTIFY, CONNECTED );

		break;

	case enumServerConnectClose:

		::PostMessage( m_hwndViewer, WM_SERVER_STATUS, ACCOUNTSERVER_NOTIFY, DICONNECTED );

		break;
	}
}	
*/
void __stdcall CIntercessor::DBRoleEventNotify( LPVOID lpParam, 
						const unsigned long &ulnEventType )
{
	CIntercessor *pPlayerManager = reinterpret_cast< CIntercessor * >( lpParam );

	ASSERT( pPlayerManager );

	try
	{
		pPlayerManager->_DBRoleEventNotify(ulnEventType);
	}
	catch (const std::exception& ex)
	{
		DWORD threadId = GetCurrentThreadId();
		std::time_t now = std::time(nullptr);
		char timeStr[64];
		std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

		std::ostringstream oss;
		oss << "[" << timeStr << "] "
			<< "ThreadID: " << threadId
			<< ", this: " << pPlayerManager
			<< ", ulnEventType: " << ulnEventType
			<< ", std::exception: " << ex.what()
			<< std::endl;

		std::ofstream log("CIntercessor_DBRoleEventNotify.log", std::ios::app);
		log << oss.str();
		log.close();

		::MessageBoxA(NULL, oss.str().c_str(), "CIntercessor DBRoleEventNotify std::exception", MB_OK | MB_ICONERROR);
	}
	catch (...)
	{
		DWORD threadId = GetCurrentThreadId();
		std::time_t now = std::time(nullptr);
		char timeStr[64];
		std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

		std::ostringstream oss;
		oss << "[" << timeStr << "] "
			<< "ThreadID: " << threadId
			<< ", this: " << pPlayerManager
			<< ", ulnEventType: " << ulnEventType
			<< ", Unknown exception in DBRoleEventNotify"
			<< std::endl;

		std::ofstream log("CIntercessor_DBRoleEventNotify.log", std::ios::app);
		log << oss.str();
		log.close();

		::MessageBoxA(NULL, oss.str().c_str(), "CIntercessor DBRoleEventNotify exception", MB_OK | MB_ICONERROR);
	}
}

void CIntercessor::_DBRoleEventNotify( const unsigned long &ulnEventType )
{
	switch( ulnEventType )
	{
	case enumServerConnectCreate:

		::PostMessage( m_hwndViewer, WM_SERVER_STATUS, DBROLESERVER_NOTIFY, CONNECTED );

		break;

	case enumServerConnectClose:

		::PostMessage( m_hwndViewer, WM_SERVER_STATUS, DBROLESERVER_NOTIFY, DICONNECTED );

		break;
	}
}

void __stdcall CIntercessor::PlayerEventNotify( LPVOID lpParam,
			const unsigned long &ulnID,
			const unsigned long &ulnEventType )
{
	CIntercessor *pPlayerManager = reinterpret_cast< CIntercessor * >( lpParam );

	ASSERT( pPlayerManager );

	try
	{
		pPlayerManager->_PlayerEventNotify(ulnID, ulnEventType);
	}
	catch (const std::exception& ex)
	{
		DWORD threadId = GetCurrentThreadId();
		std::time_t now = std::time(nullptr);
		char timeStr[64];
		std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

		std::ostringstream oss;
		oss << "[" << timeStr << "] "
			<< "ThreadID: " << threadId
			<< ", this: " << pPlayerManager
			<< ", ulnID: " << ulnID
			<< ", ulnEventType: " << ulnEventType
			<< ", std::exception: " << ex.what()
			<< std::endl;

		std::ofstream log("CIntercessor_PlayerEventNotify.log", std::ios::app);
		log << oss.str();
		log.close();

		::MessageBoxA(NULL, oss.str().c_str(), "CIntercessor PlayerEventNotify std::exception", MB_OK | MB_ICONERROR);
	}
	catch (...)
	{
		DWORD threadId = GetCurrentThreadId();
		std::time_t now = std::time(nullptr);
		char timeStr[64];
		std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

		std::ostringstream oss;
		oss << "[" << timeStr << "] "
			<< "ThreadID: " << threadId
			<< ", this: " << pPlayerManager
			<< ", ulnID: " << ulnID
			<< ", ulnEventType: " << ulnEventType
			<< ", Unknown exception in PlayerEventNotify"
			<< std::endl;

		std::ofstream log("CIntercessor_PlayerEventNotify.log", std::ios::app);
		log << oss.str();
		log.close();

		::MessageBoxA(NULL, oss.str().c_str(), "CIntercessor PlayerEventNotify exception", MB_OK | MB_ICONERROR);
	}
}

void CIntercessor::_PlayerEventNotify( const unsigned long &ulnID,
			const unsigned long &ulnEventType )
{
	//CCriticalSection::Owner locker( m_csPlayerAction );

	switch ( ulnEventType )
	{
	case enumClientConnectCreate:
	{
		// Get the client's IP address
		const char* clientIP = m_pPlayerServer->GetClientInfo(ulnID);
		if (!clientIP)
		{
			TRACE("Failed to retrieve client IP for player ID: %lu", ulnID);
			return;
		}

		// Extract IP address from "IP:Port" format
		std::string clientIPStr(clientIP);
		size_t colonPos = clientIPStr.find(':');
		if (colonPos == std::string::npos)
		{
			TRACE("Invalid client IP format: %s", clientIP);
			return;
		}

		std::string ipAddress = clientIPStr.substr(0, colonPos);

		// Count active connections from the same IP
		int connectionCount = 0;
		for (const auto& pair : m_thePlayers)
		{
			const auto& playerID = pair.first;
			IPlayer* player = pair.second;
			if (player && player->IsActive())
			{
				const char* existingIP = m_pPlayerServer->GetClientInfo(playerID);
				if (existingIP)
				{
					std::string existingIPStr(existingIP);
					size_t pos = existingIPStr.find(':');
					if (pos != std::string::npos && existingIPStr.substr(0, pos) == ipAddress)
					{
						connectionCount++;
					}
				}
			}
		}

		if (connectionCount >= m_maxConnectionsPerIP)
		{
			TRACE("Connection limit reached for IP: %s", ipAddress.c_str());
			m_pPlayerServer->ShutdownClient(ulnID);
			return;
		}

		// Activate the player
		IPlayer* pPlayer = m_thePlayers[ulnID];
		ASSERT(pPlayer);
		pPlayer->Active();
	}
	break;

	case enumClientConnectClose:
		{
			PLAYER_MAP::iterator it;
			
			if ( m_thePlayers.end() != ( it = m_thePlayers.find( ulnID ) ) )
			{
				IPlayer *pPlayer = m_thePlayers[ulnID];
				
				ASSERT( pPlayer );

				pPlayer->Inactive();
			}
		}
		break;
	}
}

bool CIntercessor::StartupNetwork()
{
	bool bPlayerServerSucceed = false;
	bool bDBRoleServerSucceed = false;
	bool bGameSvrServerSucceed = false;
	
	/*
	 * There is connectted the heaven by the rainbow
	 */

	/*
	 * For the db-role server
	 */
	pfnCreateClientInterface pClientFactroyFun = ( pfnCreateClientInterface )( m_theRainbowLib.GetProcAddress( _T( "CreateInterface" ) ) );
	
	IClientFactory *pClientFactory = NULL;

	if ( pClientFactroyFun && SUCCEEDED( pClientFactroyFun( IID_IClientFactory, reinterpret_cast< void ** >( &pClientFactory ) ) ) )
	{
		pClientFactory->SetEnvironment( 1024 * 1024 );
		
		pClientFactory->CreateClientInterface( IID_IESClient, reinterpret_cast< void ** >( &m_pDBRoleClient ) );
		
		SAFE_RELEASE( pClientFactory );
	}

	if ( m_pDBRoleClient )
	{
		m_pDBRoleClient->Startup();

		m_pDBRoleClient->RegisterMsgFilter( reinterpret_cast< void * >( this ), DBRoleEventNotify );

		if ( SUCCEEDED( m_pDBRoleClient->ConnectTo( m_theNetworkConfig.GetRoleSvrIP(), m_theNetworkConfig.GetRoleSvrPort() ) ) )
		{
			bDBRoleServerSucceed = true;
		}
	}

	/*
	 * We open the heaven to wait for the rainbow
	 */
	pfnCreateServerInterface pServerFactroyFun = 
		( pfnCreateServerInterface )( m_theHeavenLib.GetProcAddress( _T( "CreateInterface" ) ) );

	IServerFactory *pServerFactory = NULL;

	if ( pServerFactroyFun && 
		SUCCEEDED( pServerFactroyFun( IID_IServerFactory, reinterpret_cast< void ** >( &pServerFactory ) ) ) )
	{
		pServerFactory->SetEnvironment( m_lnMaxPlayerCount, 10, 1000, 1024 * 8 );

		/*
		 * For player
		 */
		pServerFactory->CreateServerInterface( IID_IIOCPServer, reinterpret_cast< void ** >( &m_pPlayerServer ) );

		/*
		 * For gameserver
		 */
		pServerFactory->CreateServerInterface( IID_IIOCPServer, reinterpret_cast< void ** >( &m_pGameSvrServer ) );
		
		pServerFactory->Release();		
	}

	if ( m_pGameSvrServer )
	{
		m_pGameSvrServer->Startup();
/*
		IServer *pCloneGameServer = NULL;
		m_pGameSvrServer->QueryInterface( IID_IIOCPServer, reinterpret_cast< void ** >( &pCloneGameServer ) );
		
		IClient *pCloneAcc = NULL;
		m_pAccSvrClient->QueryInterface( IID_IESClient, reinterpret_cast< void ** >( &pCloneAcc ) );
*/
		static CGameServer::NI ni;
		ni.pServer = m_pGameSvrServer;
		ni.pClient = NULL; //m_pAccSvrClient;
		ni.hwndContainer = m_hwndViewer;
		
		m_pGameSvrServer->RegisterMsgFilter( reinterpret_cast< void * >( &ni ), CGameServer::GameSvrEventNotify );

		if ( SUCCEEDED( m_pGameSvrServer->OpenService( INADDR_ANY, m_theNetworkConfig.GetGameSvrOpenPort() ) ) )
		{
			bGameSvrServerSucceed = true;
		}
	}

	if ( m_pPlayerServer )
	{
		m_pPlayerServer->Startup();

		m_pPlayerServer->RegisterMsgFilter( reinterpret_cast< void * >( this ), PlayerEventNotify );

		if ( SUCCEEDED( m_pPlayerServer->OpenService( INADDR_ANY, m_theNetworkConfig.GetClientOpenPort() ) ) )
		{
			bPlayerServerSucceed = true;			
		}
	}

	if ( bPlayerServerSucceed && 
			bDBRoleServerSucceed && 
			bGameSvrServerSucceed )
	{
//		IClient *pCloneAcc = NULL;
//		m_pAccSvrClient->QueryInterface( IID_IESClient, reinterpret_cast< void ** >( &pCloneAcc ) );
		
		IServer *pClonePlayer = NULL;
		m_pPlayerServer->QueryInterface( IID_IIOCPServer, reinterpret_cast< void ** >( &pClonePlayer ) );
		
		IClient *pCloneDBRole = NULL;
		m_pDBRoleClient->QueryInterface( IID_IESClient, reinterpret_cast< void ** >( &pCloneDBRole ) );
		
		ASSERT( /* pCloneAcc && */ pClonePlayer && pCloneDBRole );
			
		CGamePlayer::ATTACH_NETWORK( NULL/* pCloneAcc */, pClonePlayer, pCloneDBRole );

		/*
		 * Create new player
		 */
		{
			CCriticalSection::Owner lock( m_csPlayerAction );
			
			/*
			 * Initialize all of player
			 */
			for ( size_t index = 0; index < m_lnMaxPlayerCount; index ++ )
			{
				/*
				* Create a player node and add it into list
				*/
				IPlayer *pPlayer = new CGamePlayer( index );
				
				ASSERT( pPlayer );
				
				m_thePlayers[index] = pPlayer;
			}
		}

		IServer *pCloneGameServer = NULL;
		m_pGameSvrServer->QueryInterface( IID_IIOCPServer, reinterpret_cast< void ** >( &pCloneGameServer ) );

		CGameServer::Begin( pCloneGameServer );

		m_hStartupWoringThreadEvent.Set();
		m_hStartupHelperThreadEvent.Set();

	}
	
	return ( bPlayerServerSucceed && bDBRoleServerSucceed && bGameSvrServerSucceed );
}

void CIntercessor::CleanNetwork()
{
	/*
	 * Clear player info
	 */
	{
		CCriticalSection::Owner locker( m_csPlayerAction );
		
		PLAYER_MAP::iterator it;

		for ( it = m_thePlayers.begin(); it != m_thePlayers.end(); it ++ )
		{
			IPlayer *pPlayer = ( IPlayer * )( ( *it ).second );
			
			ASSERT( pPlayer );
			
			SAFE_DELETE( pPlayer );
		}
		
		m_thePlayers.erase( m_thePlayers.begin(), m_thePlayers.end() );
	}

	CGameServer::End();
	CGamePlayer::DETACH_NETWORK();

	/*
	 * Disconnect network and relase this resource
	 */
	if ( m_pPlayerServer )
	{
		m_pPlayerServer->CloseService();

		m_pPlayerServer->Cleanup();
		
		SAFE_RELEASE( m_pPlayerServer );
	}

	if ( m_pDBRoleClient )
	{
		m_pDBRoleClient->Cleanup();

		SAFE_RELEASE( m_pDBRoleClient );
	}

	if ( m_pGameSvrServer )
	{
		m_pGameSvrServer->CloseService();

		m_pGameSvrServer->Cleanup();

		SAFE_RELEASE( m_pGameSvrServer );
	}
}

DWORD WINAPI CIntercessor::WorkingThreadFunction( void *pV )
{
	CIntercessor* pThis = reinterpret_cast<CIntercessor*>(pV);

	ASSERT(pThis);

	try
	{
		pThis->Working();
	}
	catch (const std::exception& ex)
	{
		// Gather context
		DWORD threadId = GetCurrentThreadId();
		std::time_t now = std::time(nullptr);
		char timeStr[64];
		std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

		std::ostringstream oss;
		oss << "[" << timeStr << "] "
			<< "ThreadID: " << threadId
			<< ", this: " << pThis
			<< ", m_lnMaxPlayerCount: " << pThis->m_lnMaxPlayerCount
			<< ", std::exception: " << ex.what()
			<< std::endl;

		std::ofstream log("CIntercessor_WorkingThread.log", std::ios::app);
		log << oss.str();
		log.close();

		::MessageBoxA(NULL, oss.str().c_str(), "CIntercessor std::exception", MB_OK | MB_ICONERROR);
	}
	catch (...)
	{
		DWORD threadId = GetCurrentThreadId();
		std::time_t now = std::time(nullptr);
		char timeStr[64];
		std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

		std::ostringstream oss;
		oss << "[" << timeStr << "] "
			<< "ThreadID: " << threadId
			<< ", this: " << pThis
			<< ", m_lnMaxPlayerCount: " << pThis->m_lnMaxPlayerCount
			<< ", Unknown exception in WorkingThreadFunction"
			<< std::endl;

		std::ofstream log("CIntercessor_WorkingThread.log", std::ios::app);
		log << oss.str();
		log.close();

		::MessageBoxA(NULL, oss.str().c_str(), "CIntercessor class", MB_OK | MB_ICONERROR);
	}

	return 0;
}

int	CIntercessor::Working()
{
//	::CoInitialize( NULL );
	
	m_hStartupWoringThreadEvent.Wait();
	
	while ( !m_hQuitEvent.Wait( 0 ) )
	{
		/*
		 * For some alive player
		 */
		{
			CCriticalSection::Owner locker( m_csPlayerAction );
			
			for ( size_t index = 0; index < m_lnMaxPlayerCount; index ++ )
			{		
				IPlayer *pPlayer = m_thePlayers[index];
				
				ASSERT( pPlayer );	

				if ( pPlayer && pPlayer->IsActive() )
				{
					/*
					 * Get data from player
					 */
					size_t datalength = 0;
					
					const void *pData = m_pPlayerServer->GetPackFromClient( index, datalength );
					
					if ( pData && datalength )
					{
						AnalyzePlayerRequire( index, pData, datalength );
					}
					
					/*
					* Execute work
					*/
					pPlayer->Run();
				}
			}
		}

		::Sleep( 1 );
	}

//	::CoUninitialize();

	return 0;
}

DWORD WINAPI CIntercessor::HelperThreadFunction( void *pV )
{
	CIntercessor *pThis = reinterpret_cast< CIntercessor * >( pV );

	ASSERT( pThis );

	try
	{
		pThis->Helper();
	}
	catch (const std::exception& ex)
	{
		// Gather context
		DWORD threadId = GetCurrentThreadId();
		std::time_t now = std::time(nullptr);
		char timeStr[64];
		std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

		std::ostringstream oss;
		oss << "[" << timeStr << "] "
			<< "ThreadID: " << threadId
			<< ", this: " << pThis
			<< ", m_lnMaxPlayerCount: " << pThis->m_lnMaxPlayerCount
			<< ", std::exception: " << ex.what()
			<< std::endl;

		std::ofstream log("CIntercessor_HelperThread.log", std::ios::app);
		log << oss.str();
		log.close();

		::MessageBoxA(NULL, oss.str().c_str(), "CIntercessor HelperThread std::exception", MB_OK | MB_ICONERROR);
	}
	catch (...)
	{
		DWORD threadId = GetCurrentThreadId();
		std::time_t now = std::time(nullptr);
		char timeStr[64];
		std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

		std::ostringstream oss;
		oss << "[" << timeStr << "] "
			<< "ThreadID: " << threadId
			<< ", this: " << pThis
			<< ", m_lnMaxPlayerCount: " << pThis->m_lnMaxPlayerCount
			<< ", Unknown exception in HelperThreadFunction"
			<< std::endl;

		std::ofstream log("CIntercessor_HelperThread.log", std::ios::app);
		log << oss.str();
		log.close();

		::MessageBoxA(NULL, oss.str().c_str(), "CIntercessor HelperThread exception", MB_OK | MB_ICONERROR);
	}

	return 0;
}

int	CIntercessor::Helper()
{
	m_hStartupHelperThreadEvent.Wait();
	
	static const DWORD dwTimeLimit = 1000 * 10;
	DWORD dwStart = ::GetTickCount();
	DWORD dwEnd = dwStart;

	while ( !m_hQuitEvent.Wait( 0 ) )
	{
		/*
		 * ping account server
		 */
		if ( ( ( ( dwEnd = ::GetTickCount() ) - dwStart ) > dwTimeLimit ) ||
			( dwStart > dwEnd ) )
		{
			dwStart = dwEnd;

			PING_COMMAND pc;
			pc.ProtocolType = c2s_ping;
			pc.m_dwTime		= dwStart;

			g_theSmartClient.Send( &pc, sizeof( PING_COMMAND ) );
//			m_pAccSvrClient->SendPackToServer( &pc, sizeof( PING_COMMAND ) );
		}

		/*
		 * Get data from account-server
		 */
		size_t dataLength = 0;
		const void *pData = g_theSmartClient.Recv( dataLength );
//		const void *pData = m_pAccSvrClient->GetPackFromServer( dataLength );

		while( pData && dataLength )
		{
			ASSERT( CPackager::Peek( pData ) > g_nGlobalProtocolType );

			if ( s2c_accountlogin == *( const char * )pData )
			{
				const KAccountHead *pAUR = ( KAccountHead * )( ( ( const char * )pData ) + 1 );
				
				UINT uID = pAUR->Operate;
				
				if ( uID < m_lnMaxPlayerCount )
				{
					IPlayer *pPlayer = m_thePlayers[uID];
					
					ASSERT( pPlayer );
					
					pPlayer->AppendData( CGamePlayer::enumOwnerAccSvr, pData, dataLength );
				}
			}
			
			/*
			 * ping account server
			 */
			if ( ( ( ( dwEnd = ::GetTickCount() ) - dwStart ) > dwTimeLimit ) ||
				( dwStart > dwEnd ) )
			{
				dwStart = dwEnd;
				
				PING_COMMAND pc;
				pc.ProtocolType = c2s_ping;
				pc.m_dwTime		= dwStart;
				
				g_theSmartClient.Send( &pc, sizeof( PING_COMMAND ) );
//				m_pAccSvrClient->SendPackToServer( &pc, sizeof( PING_COMMAND ) );
			}

			pData = g_theSmartClient.Recv( dataLength );
//			pData = m_pAccSvrClient->GetPackFromServer( dataLength );
		}
		
		/*
		 * Get data from role-database
		 */
		dataLength = 0;
		pData = m_pDBRoleClient->GetPackFromServer( dataLength );

		while( pData && dataLength )
		{
			BYTE cProtocol = CPackager::Peek( pData );

			UINT uID = ( UINT )( -1 );

			if ( cProtocol < g_nGlobalProtocolType )
			{
				uID = *( const unsigned long * )( ( const char * )pData + 2 );
			}
			else // cProtocol > g_nGlobalProtocolType
			{
				TProcessData *pPD = ( TProcessData * )( pData );

				uID = pPD->ulIdentity;
			}

			if ( uID < m_lnMaxPlayerCount )
			{
				IPlayer *pPlayer = m_thePlayers[uID];

				ASSERT( pPlayer );
				
				pPlayer->AppendData( CGamePlayer::enumOwnerRoleSvr, pData, dataLength );
			}
			
			/*
			 * ping account server
			 */
			if ( ( ( ( dwEnd = ::GetTickCount() ) - dwStart ) > dwTimeLimit ) ||
				( dwStart > dwEnd ) )
			{
				dwStart = dwEnd;
				
				PING_COMMAND pc;
				pc.ProtocolType = c2s_ping;
				pc.m_dwTime		= dwStart;

				g_theSmartClient.Send( &pc, sizeof( PING_COMMAND ) );
//				m_pAccSvrClient->SendPackToServer( &pc, sizeof( PING_COMMAND ) );
			}

			pData = m_pDBRoleClient->GetPackFromServer( dataLength );
		}
		
		::Sleep( 1 );
	}

	return 0;
}

bool CIntercessor::AnalyzePlayerRequire( size_t index, const void *pData, size_t datalength )
{
	ASSERT( pData && datalength );

	BYTE cProtocol = *( const BYTE * )pData;

	switch ( cProtocol )
	{
	case c2s_login:
		{
			const BYTE *pBuffer = ( const BYTE * )pData + 1;

			KLoginAccountInfo *pLAI = ( KLoginAccountInfo * )pBuffer;

			ASSERT( pLAI );

			IPlayer *pPlayer = m_thePlayers[index];

			if ( pLAI && pPlayer )
			{				
				pPlayer->AppendData( CGamePlayer::enumOwnerPlayer, pData, datalength );

				pPlayer->DispatchTask( CGamePlayer::enumLogin );
			}
		}

		break;
		
	case c2s_dbplayerselect:
	case c2s_newplayer:
	case c2s_roleserver_deleteplayer:
		{
			if ( index < m_lnMaxPlayerCount )
			{
				IPlayer *pPlayer = m_thePlayers[index];
				
				ASSERT( pPlayer );
				
				pPlayer->AppendData( CGamePlayer::enumOwnerPlayer, pData, datalength );
			}
		}		
		break;

	default:
		break;
	}
	
	return true;
}

size_t CIntercessor::GetGameServerCount()
{
	if ( m_pGameSvrServer )
	{
		return m_pGameSvrServer->GetClientCount();
	}

	return 0;
}

const char *CIntercessor::GetGameServerInfo( const unsigned long &ulnID )
{
	if ( m_pGameSvrServer )
	{
		return m_pGameSvrServer->GetClientInfo( ulnID );
	}

	return NULL;
}