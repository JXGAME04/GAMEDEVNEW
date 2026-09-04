
#include "ClientStage.h"
#include "KSG_EncodeDecode.h"
#include <time.h>
#include <stdio.h>

#include "Utils.h"
#include "Exception.h"

using OnlineGameLib::Win32::CIOBuffer;

using OnlineGameLib::Win32::CException;
using OnlineGameLib::Win32::Output;
using OnlineGameLib::Win32::CCriticalSection;
using OnlineGameLib::Win32::CWin32Exception;

/*
 * Get client factory interface
 */
STDAPI CreateInterface( REFIID	riid, void	**ppv )
{
	HRESULT hr = E_NOINTERFACE;

	if ( IID_IClientFactory == riid )
	{
		CClientFactory *pObject = new CClientFactory;

		*ppv = reinterpret_cast< void * > ( dynamic_cast< IClientFactory * >( pObject ) );

		if ( *ppv )
		{
			reinterpret_cast< IUnknown * >( *ppv )->AddRef();

			hr = S_OK;
		}
	}

	if ( FAILED( hr ) )
	{
		*ppv = NULL;
	}

	return ( HRESULT )( hr );
}

static unsigned gs_holdrand = time(NULL);

static inline unsigned _Rand()
{
    gs_holdrand = gs_holdrand * 214013L + 2531011L;
     
    return gs_holdrand;
}

static inline void RandMemSet(int nSize, unsigned char *pbyBuffer)
{
    _ASSERT(nSize);
    _ASSERT(pbyBuffer);

    while (nSize--)
    {
        *pbyBuffer++ = (unsigned char)_Rand();
    }
}

/*
 * class CGameClient
 */
CGameClient::CGameClient( size_t maxFreeBuffers, 
		size_t maxFreeBuffers_Cache,
		size_t bufferSize_Cache /*= 8192*/,
		size_t bufferSize /*= 1024*/ )
		: OnlineGameLib::Win32::CSocketClient( maxFreeBuffers, bufferSize )
		, m_theCacheAllocator( bufferSize_Cache, maxFreeBuffers_Cache )
		, m_lRefCount( 0 )
		, m_pfnCallbackClientEvent( NULL )
{
	m_pReadBuffer = m_theCacheAllocator.Allocate();
	m_pRecvBuffer = m_theCacheAllocator.Allocate();
}

CGameClient::~CGameClient()
{
	m_pReadBuffer->Release();
	m_pRecvBuffer->Release();
}

STDMETHODIMP CGameClient::Startup()
{
	HRESULT hr = E_FAIL;

	try
	{
		Start();

		hr = S_OK;
	}
	catch( const CException &e )
	{
		Output( _T("CGameClient::Startup Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage() );
	}
	catch(...)
	{
		Output( _T("CGameClient::Startup - Unexpected exception") );
	}
	
	return hr;	
}

STDMETHODIMP CGameClient::Cleanup()
{
	HRESULT hr = E_FAIL;

	try
	{
		StopConnections();
		
		WaitForShutdownToComplete();

		hr = S_OK;
	}
	catch( const CException &e )
	{
		Output( _T("CConnectManager::Cleanup Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage() );
	}
	catch(...)
	{
		Output( _T("CConnectManager::Cleanup Unexpected exception") );
	}

	return hr;
}

STDMETHODIMP CGameClient::ConnectTo(
				const char * const &pAddressToConnectServer, 
				unsigned short usPortToConnectServer )
{
	HRESULT hr = E_FAIL;

	try
	{
		Connect( pAddressToConnectServer, usPortToConnectServer );
		
		if ( StartConnections() )
		{
			hr = S_OK;
		}
	}
	catch( const CException &e )
	{
		Output( _T("Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage() );
	}
	catch(...)
	{
		Output( _T("Unexpected exception") );
	}
	
	return hr;
}

STDMETHODIMP CGameClient::RegisterMsgFilter( 
				LPVOID lpParam, 
				CALLBACK_CLIENT_EVENT pfnEventNotify )
{
	m_lpCallBackParam = lpParam;
	m_pfnCallbackClientEvent = pfnEventNotify;

	return S_OK;
}

STDMETHODIMP CGameClient::SendPackToServer(
				const void * const pData,
				const size_t &datalength )
{
	static const size_t s_len_protocol = sizeof( WORD );
	
	if ( !pData || 0 == datalength )
	{
		return E_FAIL;
	}

	CIOBuffer *pBuffer = Allocate();

	/*
	 * Add package header
	 */
	const size_t headlength = s_len_protocol + datalength;
	pBuffer->AddData( reinterpret_cast< const char * >( &headlength ), s_len_protocol );	

	unsigned uOldKey = m_uClientKey;

	pBuffer->AddData( reinterpret_cast< const char* const >( pData ), datalength );

	_ASSERT(m_uKeyMode == 0);
	KSG_EncodeBuf(
		datalength,
		(unsigned char *)(pBuffer->GetBuffer() + s_len_protocol),
        &m_uClientKey
	);

	Write( pBuffer );

	pBuffer->Release();

	return S_OK;
}

STDMETHODIMP_( const void * ) CGameClient::GetPackFromServer( size_t &datalength )
{
	CCriticalSection::Owner locker( m_csReadAction );

	m_pReadBuffer->Empty();

	m_pRecvBuffer = ProcessDataStream( m_pRecvBuffer );

	const BYTE *pPackData = m_pReadBuffer->GetBuffer();
	datalength = m_pReadBuffer->GetUsed();	
	
	return reinterpret_cast<const char*>( pPackData );
}

STDMETHODIMP CGameClient::Shutdown()
{
	StopConnections();

	return S_OK;
}

STDMETHODIMP CGameClient::QueryInterface( REFIID riid, void** ppv )
{
	/*
	 * By definition all COM objects support the IUnknown interface
	 */
	if( riid == IID_IUnknown )
	{
		AddRef();

		*ppv = dynamic_cast< IUnknown * >( this );
	}
	else if ( riid == IID_IESClient )
	{
		AddRef();

		*ppv = dynamic_cast< IClient * >( this );	
	}
	else
	{
		*ppv = NULL;

		return E_NOINTERFACE;
	}

	return S_OK;
}
    
STDMETHODIMP_(ULONG) CGameClient::AddRef()
{
	return InterlockedIncrement( &m_lRefCount );
}

STDMETHODIMP_(ULONG) CGameClient::Release()
{
	if ( InterlockedDecrement( &m_lRefCount ) > 0 )
	{
		return m_lRefCount;
	}

	delete this;
	return 0L;
}

void CGameClient::ProcessCommand( const OnlineGameLib::Win32::CIOBuffer *pBuffer)
{
	static const size_t s_len_protocol = sizeof( WORD );

	const BYTE *pPackData = pBuffer->GetBuffer();
	const size_t used = pBuffer->GetUsed();

	if ( used <= s_len_protocol )
	{
        return;
	}
	
	_ASSERT(m_uKeyMode == 0);
	KSG_DecodeBuf(
		used - s_len_protocol, 
		(unsigned char *)( pPackData + s_len_protocol ), 
		&m_uServerKey
	);

    m_pReadBuffer->AddData( ( const BYTE * )( pPackData + s_len_protocol ), used - s_len_protocol );
}

size_t CGameClient::GetMinimumMessageSize() const
{
	static size_t length = sizeof( WORD ) + sizeof( BYTE );

	/*
     * The smallest possible command we accept is a byte onlye package
	 */
	return length;
}

size_t CGameClient::GetMessageSize( 
			const OnlineGameLib::Win32::CIOBuffer *pBuffer ) const
{
	const BYTE *pData = pBuffer->GetBuffer();   
	const size_t used = pBuffer->GetUsed();

	WORD wHeadLen = ( WORD )( *( WORD * )( pData ) );

	return ( size_t )( wHeadLen );
}

OnlineGameLib::Win32::CIOBuffer *CGameClient::ProcessDataStream( 
						OnlineGameLib::Win32::CIOBuffer *pBuffer)
{
	const size_t used = pBuffer->GetUsed();
	
	if ( used >= GetMinimumMessageSize() )
	{
		const size_t messageSize = GetMessageSize( pBuffer );
		
		if ( messageSize == 0 )
		{
			/*
			 * havent got a complete message yet.
			
			 * we null terminate our messages in the buffer, so we need to reserve
			 * a byte of the buffer for this purpose...
			 */
			
			if ( used == ( pBuffer->GetSize() - 1 ) )
			{
				Output( _T("Too much data!") );
				
				/*
				 * Write this message and then shutdown the sending side of the socket.
				 */
				Output( "found error and close this connection!" );
	
				Shutdown();
				
				/*
				 * throw the rubbish away
				 */
				pBuffer->Empty();					
			}
		}
		else if ( used == messageSize )
		{
			Output( _T("Got a complete message and begin to process it") );
			/*
			 * we have a whole, distinct, message
			 */
			
			ProcessCommand( pBuffer );
			
			pBuffer->Empty();
		}
		else if (used > messageSize)
		{
			Output(_T("Got message plus extra data"));
			/*
			 * we have a message, plus some more data
			 * 
			 * allocate a new buffer, copy the extra data into it and try again...
			 */
			
			OnlineGameLib::Win32::CIOBuffer *pMessage = pBuffer->SplitBuffer( messageSize );
			
			ProcessCommand( pMessage );
			
			pMessage->Release();
			
		}
	}
	
	/*
	 * Reissue a read into the same buffer to collect more data
	 */
	return pBuffer;
}

void CGameClient::OnStartConnections()
{
	if ( m_pfnCallbackClientEvent )
	{
		m_pfnCallbackClientEvent( m_lpCallBackParam, enumServerConnectCreate );
	}
}

void CGameClient::OnStopConnections()
{
	if ( m_pfnCallbackClientEvent )
	{
		m_pfnCallbackClientEvent( m_lpCallBackParam, enumServerConnectClose );
	}
}

void CGameClient::ReadCompleted( OnlineGameLib::Win32::CIOBuffer *pBuffer )
{
	try
	{	
		const BYTE *pPackData = pBuffer->GetBuffer();
		size_t used = pBuffer->GetUsed();

		if (used > 0)
		{
			// [RECV 04/09] Truoc day: m_pRecvBuffer day -> CIOBuffer::AddData lang le BO CA KHOI vua doc
			// (IOBuffer.cpp:90-97, dong throw da bi chu thich, chi return) -> mat nguyen 1-2 lan doc ~10 KB
			// (19 khung 518 byte) tren luong Goddess->GameServer ma ket noi van song. CPackager::PackUp ghep
			// goi 1 cua nhan vat A voi goi 3.. cua nhan vat B -> TRoleData header rac -> LoadDBPlayerInfo doc
			// ngoai vung -> GameServer sap 15:05:38 04/09 (dump: L1 LyMinh812 + P3.. PhanNghia813).
			// Nay KHONG BAO GIO vut: cho luong chinh (MessageLoop) lay bot - backpressure TCP ve Goddess;
			// ket qua 30 giay thi dong ket noi va bao to, thay vi doc rac.
			int nCho = 0;
			for ( ; ; )
			{
				{
					CCriticalSection::Owner locker( m_csReadAction );

					if ( used <= m_pRecvBuffer->GetSize() - m_pRecvBuffer->GetUsed() )
					{
						m_pRecvBuffer->AddData( reinterpret_cast< const char * >( pPackData ), used );
						break;
					}
				}

				if ( nCho == 0 )
				{
					printf( "--CGameClient::ReadCompleted: bo dem nhan %u KB DAY (dang giu %u byte, can them %u) -> CHO luong chinh lay bot, KHONG vut--\n",
						(unsigned)( m_pRecvBuffer->GetSize() / 1024 ), (unsigned)m_pRecvBuffer->GetUsed(), (unsigned)used );
				}
				if ( ++nCho > 30000 )
				{
					printf( "--CGameClient::ReadCompleted: ket %d ms luong chinh khong lay bot -> DONG ket noi de khong doc rac--\n", nCho );
					StopConnections();
					break;
				}
				::Sleep( 1 );
			}
		}
		
		pBuffer->Empty();
	}
	catch(const CException &e)
	{
		Output( _T("ReadCompleted - Exception - ") + e.GetWhere() + _T(" - ") + e.GetMessage() );

		StopConnections();
	}
	catch(...)
	{
		Output( _T("ReadCompleted - Unexpected exception") );
		
		StopConnections();
	}
}

CClientFactory::CClientFactory()
		: m_lRefCount( 0 ),
		m_bufferSize( 0 )
{

}

CClientFactory::~CClientFactory()
{

}

STDMETHODIMP CClientFactory::SetEnvironment( const size_t &bufferSize )
{
	m_bufferSize = bufferSize;

	return S_OK;
}

STDMETHODIMP CClientFactory::CreateClientInterface( REFIID riid, void** ppv )
{
	HRESULT hr = E_NOINTERFACE;

	if ( IID_IESClient == riid )
	{		
		// [RECV 04/09] San duoi 4 MB cho bo dem nhan cua moi client: GameServer xin 128 KB cho link
		// Goddess (KSOServer.cpp:688) = 0,4 giay o ~300 KB/s khi nap 1000 bot -> tran -> mat khung.
		// 4 MB = ~13 giay dem. (KSOServer.cpp da nang len 8 MB cho lan build GameServer.exe ke tiep.)
		size_t bufferSize = ( m_bufferSize > 0 ) ? m_bufferSize : ( 1024 * 4 );
		if ( bufferSize < 4 * 1024 * 1024 )
			bufferSize = 4 * 1024 * 1024;

		CGameClient *pObject = new CGameClient( 2, 2, bufferSize, 1024*10 );

		*ppv = reinterpret_cast< void * > ( dynamic_cast< IClient * >( pObject ) );

		if ( *ppv )
		{
			reinterpret_cast< IUnknown * >( *ppv )->AddRef();

			hr = S_OK;
		}
	}

	if ( FAILED( hr ) )
	{
		*ppv = NULL;
	}

	return ( HRESULT )( hr );
}

STDMETHODIMP CClientFactory::QueryInterface( REFIID riid, void** ppv )
{
	/*
	 * By definition all COM objects support the IUnknown interface
	 */
	if( riid == IID_IUnknown )
	{
		AddRef();

		*ppv = dynamic_cast< IUnknown * >( this );
	}
	else if ( riid == IID_IClientFactory )
	{
		AddRef();

		*ppv = dynamic_cast< IClientFactory * >( this );	
	}
	else
	{
		*ppv = NULL;

		return E_NOINTERFACE;
	}

	return S_OK;
}
    
STDMETHODIMP_(ULONG) CClientFactory::AddRef()
{
	return ::InterlockedIncrement( &m_lRefCount );
}

STDMETHODIMP_(ULONG) CClientFactory::Release()
{
	if ( ::InterlockedDecrement( &m_lRefCount ) > 0 )
	{
		return m_lRefCount;
	}

	delete this;
	return 0L;
}