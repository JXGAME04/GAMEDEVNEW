#include "ServerStage.h" 
#include <time.h>
#include <process.h>		//Thread define
#include <stdio.h>		// [GUI 07/09] jx_gui_server.log
#include <stdarg.h>
#include <string.h>
#include <vector>

#include "KSG_EncodeDecode.h"

#include "Cipher.h"
#include "Macro.h"
#include "Exception.h"
#include "SocketAddress.h"
#include "Utils.h"
#include "Win32Exception.h"

using OnlineGameLib::Win32::CIOBuffer;
using OnlineGameLib::Win32::CWin32Exception;

using OnlineGameLib::Win32::_tstring;
using OnlineGameLib::Win32::CCriticalSection;
using OnlineGameLib::Win32::CException;
using OnlineGameLib::Win32::Output;
using OnlineGameLib::Win32::DumpData;
using OnlineGameLib::Win32::GetTimeStamp;
using OnlineGameLib::Win32::ToString;
using OnlineGameLib::Win32::Trace;
using OnlineGameLib::Win32::CSocketAddress;

/*
 * [GUI 07/09] Do duong gui + phep thu nhan ban.
 * Moi bo dem duoi day chi duoc cham tu LUONG CHINH (Core tick -> PackDataToClient / SendPackToClient(-1)),
 * nen khong can interlocked. Bao cao moi 10 s vao jx_gui_server.log (mo-ghi-dong, khong giu handle de khong
 * can tro viec xoay tep cua Core).
 */
static LONGLONG gs_qpcTanSo = 0;
static inline LONGLONG GuiDoQpc() { LARGE_INTEGER li; ::QueryPerformanceCounter( &li ); return li.QuadPart; }
static inline double   GuiDoMs( LONGLONG t ) { return gs_qpcTanSo ? ( 1000.0 * (double)t / (double)gs_qpcTanSo ) : 0.0; }

static int      gs_nGoi = 0;			// PackDataToClient toi client that
static int      gs_nGoiTran = 0;		// ... trong do dem day, phai gui ngay
static LONGLONG gs_nByteGoi = 0;
static LONGLONG gs_tGoi = 0;			// QPC cong don trong PackDataToClient (cua so 10 s)
static LONGLONG gs_tGoiNhip = 0;		// ... trong nhip hien tai
static LONGLONG gs_tGoiNhipMax = 0;
static int      gs_nXa = 0;			// so lan SendPackToClient = so nhip
static int      gs_nXaClient = 0;		// so client co du lieu duoc xa
static LONGLONG gs_nByteXa = 0;
static LONGLONG gs_tXa = 0;
static LONGLONG gs_tXaMax = 0;
static int      gs_nWrite = 0;
static LONGLONG gs_tWrite = 0;			// QPC trong pSocket->Write (dat vao hang doi IOCP, chua WSASend)
static int      gs_nNhanBan = 0;		// N cua MoPhongNhanBan: 0/1 = tat
static int      gs_nNhanBanCap = 0;	// so node gia da cap
static LONGLONG gs_tNB = 0, gs_tNBNhip = 0, gs_tNBNhipMax = 0, gs_tNBXa = 0, gs_tNBXaMax = 0;
static LONGLONG gs_nByteNB = 0;
static int      gs_nGoiNB = 0, gs_nXaNB = 0;
static DWORD    gs_uBaoCao = 0;
static size_t   gs_nNBBufMax = 0;

struct NB_NODE
{
	CCriticalSection	cs;
	BYTE				*pBuf;
	size_t				used;
	NB_NODE() : pBuf( NULL ), used( 0 ) {}
	~NB_NODE() { delete [] pBuf; pBuf = NULL; }
};
static NB_NODE *gs_pNB = NULL;

static void GuiDoGhi( const char *fmt, ... )
{
	char sz[1400];
	va_list ap;
	va_start( ap, fmt );
	_vsnprintf( sz, sizeof( sz ) - 1, fmt, ap );
	va_end( ap );
	sz[sizeof( sz ) - 1] = 0;
	FILE *f = fopen( "jx_gui_server.log", "a" );
	if ( !f )
		return;
	fprintf( f, "t=%u pid=%u %s\n", (unsigned)::GetTickCount(), (unsigned)::GetCurrentProcessId(), sz );
	fclose( f );
}

/*
 * Khoa tuy chon: chi lay khi bOn. Dung cho cong lui GuiKhoaRieng=0 (lay lai m_csCM tren duong nong nhu cu).
 */
class CKhoaTuyChon
{
public:
	CKhoaTuyChon( CCriticalSection &cs, bool bOn ) : m_p( bOn ? &cs : NULL ) { if ( m_p ) m_p->Enter(); }
	~CKhoaTuyChon() { if ( m_p ) m_p->Leave(); }
private:
	CCriticalSection *m_p;
	CKhoaTuyChon( const CKhoaTuyChon & );
	CKhoaTuyChon &operator=( const CKhoaTuyChon & );
};

/*
 * Get server factory interface
 */
STDAPI CreateInterface( REFIID	riid, void	**ppv )
{
	HRESULT hr = E_NOINTERFACE;

	if ( IID_IServerFactory == riid )
	{
		CServerFactory *pObject = new CServerFactory;

		*ppv = reinterpret_cast< void * > ( dynamic_cast< IServerFactory * >( pObject ) );

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

/*
 * Local per connection data
 */
class CPerConnectionData
{
public:
      
	explicit CPerConnectionData( const _tstring &address, WPARAM wParam, const LPARAM &lParam )
         : m_address( address ), m_wParam( wParam ), m_lParam( lParam )
	{
	}

	void SetConnectionFirstParam( WPARAM wParam )
	{
		m_wParam = wParam;
	}

	WPARAM GetConnectionFirstParam() const
	{
		return m_wParam;
	}

	LPARAM GetConnectionSecondParam() const
	{
		return m_lParam;
	}

	char* GetConnectionDetails() const
	{
		const int length = m_address.length();
		char* char_array = new char[length + 1];
		strcpy(char_array, m_address.c_str());
		//char* ip_address = strtok(char_array, " :");
		//_tstring str = ip_address;
		return char_array;
	}

private:

	const _tstring	m_address;	
	const LPARAM	m_lParam;

	WPARAM			m_wParam;

	/*
	 * No copies, do not implement
	 */
	CPerConnectionData( const CPerConnectionData &rhs );
	CPerConnectionData &operator=( const CPerConnectionData &rhs );

};

CIOCPServer::CTaskQueue::~CTaskQueue()
{
	CCriticalSection::Owner locker( m_cs );

	while ( !m_theTaskStack.empty() )
	{
		m_theTaskStack.pop();
	}		
}

void CIOCPServer::CTaskQueue::Push( void *pItem )
{
	CCriticalSection::Owner locker( m_cs );

	m_theTaskStack.push( pItem );
}

void *CIOCPServer::CTaskQueue::Pop()
{
	CCriticalSection::Owner locker( m_cs );

	void *pItem = NULL;
	
	if ( !m_theTaskStack.empty() )
	{
		pItem = m_theTaskStack.top();
		m_theTaskStack.pop();
	}

	return pItem;
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

CIOCPServer::CIOCPServer( size_t nPlayerMaxCount,
				size_t nPrecision,
				size_t maxFreeSockets,
				size_t maxFreeBuffers,
				size_t maxFreeBuffers_Cache,
				size_t bufferSize_Cache /*= 8192*/,
				size_t bufferSize/* = 1024 * 5*/,
				size_t numThreads /*= 0*/ )
		: OnlineGameLib::Win32::CSocketServer( maxFreeSockets, maxFreeBuffers, bufferSize, numThreads )
		, m_theCacheAllocator( bufferSize_Cache, maxFreeBuffers_Cache )
		, m_lRefCount( 0 )
		, m_nPlayerMaxCount( nPlayerMaxCount )
		, m_nPrecision( nPrecision )
		, m_nNetworkBufferMaxLen( bufferSize - 32 /* sizeof( protocol of network ) */ )
		, m_pfnCallBackServerEvent( NULL )
		, m_hQuitHelper( NULL, true, false, NULL /*"EVENT_IOCPSVR_QUIT"*/ )
		, m_hHelperThread( NULL )
{
	/*
	 * m_nNetworkBufferMaxLen > 0
	 */
	ASSERT( !( m_nNetworkBufferMaxLen & 0x80000000 ) );

	size_t index = 0;

	// [GUI 07/09] bang tra nhanh node: cap mot lan, khong bao gio doi sau ctor (node chi bi huy o dtor).
	// Doc cong lui GuiKhoaRieng, tan so dong ho, va co bo dem gia = co bo dem ghi that (m_nNetworkBufferMaxLen).
	{
		const size_t nCap = ( nPlayerMaxCount > 0 ) ? nPlayerMaxCount : 1;
		m_ppNode = new LPCLIENT_NODE[ nCap ];
		memset( m_ppNode, 0, sizeof( LPCLIENT_NODE ) * nCap );
		m_nGuiKhoaRieng = (int)::GetPrivateProfileIntA( "Server", "GuiKhoaRieng", 1, ".\\config.ini" );
		LARGE_INTEGER liTanSo;
		if ( ::QueryPerformanceFrequency( &liTanSo ) )
			gs_qpcTanSo = liTanSo.QuadPart;
		gs_nNBBufMax = m_nNetworkBufferMaxLen;
		GuiDoGhi( "[GUI-DO] khoi dong: client toi da %u, dem ghi %u byte, GuiKhoaRieng=%d", (unsigned)nPlayerMaxCount, (unsigned)m_nNetworkBufferMaxLen, m_nGuiKhoaRieng );
	}

	for ( index = 0; index < nPlayerMaxCount; index ++ )
	{
		{
			CCriticalSection::Owner locker( m_csFCN );
			
			//m_freeClientNode.push( index );
			m_freeClientNode.push( index );
		}

		/*
		 * Create a client node and add it into list
		 */
		LPCLIENT_NODE pCN = new CLIENT_NODE;

		pCN->pSocket = NULL;
		
		pCN->pRecvBuffer	= m_theCacheAllocator.Allocate();
		pCN->pReadBuffer	= m_theCacheAllocator.Allocate();

		pCN->pWriteBuffer	= m_theCacheAllocator.Allocate();
		
		{
			CCriticalSection::Owner	locker( m_csCM );
			
			m_theClientManager[index] = pCN;

			m_ppNode[index] = pCN;	// [GUI 07/09]
		}
	}

	unsigned int threadID = 0;
	
	m_hHelperThread = (HANDLE)::_beginthreadex( 0, 
								0, 
								HelperThreadFunction,
								( void * )this, 
								0, 
								&threadID );
	
	if ( m_hHelperThread == INVALID_HANDLE_VALUE )
	{
		throw CWin32Exception( _T("CIOCPServer::CIOCPServer() - _beginthreadex"), GetLastError() );
	}
}

CIOCPServer::~CIOCPServer()
{
	
	/*
     * Repeat until stack is empty
	 */
	{
		CCriticalSection::Owner locker( m_csFCN );
		
		while ( !m_freeClientNode.empty() )
		{
			m_freeClientNode.pop();
		}

/*		while ( !m_freeClientNode.empty() )
		{
			m_freeClientNode.pop();
		}
*/
	}

	{
		CCriticalSection::Owner	locker( m_csCM );

		m_usedClientNode.erase( m_usedClientNode.begin(), m_usedClientNode.end() );
		
		CLIENT_MANAGER::iterator it;
		for ( it = m_theClientManager.begin(); it != m_theClientManager.end(); it ++ )
		{
			LPCLIENT_NODE pCN = ( LPCLIENT_NODE )( ( *it ).second );
			
			if ( pCN )
			{
				pCN->pRecvBuffer->Release();
				pCN->pReadBuffer->Release();
				
				pCN->pWriteBuffer->Release();

			}
			
			SAFE_DELETE( pCN );
		}
		
		m_theClientManager.erase( m_theClientManager.begin(), m_theClientManager.end() );
	}

	m_hQuitHelper.Set();

	if ( WAIT_TIMEOUT == ::WaitForSingleObject( m_hHelperThread, 5000 ) )
	{
		::TerminateThread( m_hHelperThread, 0L );
	}

	// [GUI 07/09] tha sau cung, khi khong con luong nao dung bang node
	delete [] m_ppNode;
	m_ppNode = NULL;
	delete [] gs_pNB;
	gs_pNB = NULL;
	gs_nNhanBanCap = 0;
	gs_nNhanBan = 0;
}

STDMETHODIMP CIOCPServer::Startup( )
{
	HRESULT hr = E_FAIL;

	try
	{
		Start();

		hr = S_OK;
	}
	catch( const CException &e )
	{
		Output( _T("CIOCPServer::Startup Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage() );
	}
	catch(...)
	{
		Output( _T("CIOCPServer::Startup - Unexpected exception") );
	}
	
	return hr;	
}

STDMETHODIMP CIOCPServer::Cleanup( )
{
	HRESULT hr = E_FAIL;

	try
	{		
		StopAcceptingConnections();
		
		WaitForShutdownToComplete();

		hr = S_OK;
	}
	catch( const CException &e )
	{
		Output( _T("CIOCPServer::Cleanup Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage() );
	}
	catch(...)
	{
		Output( _T("CIOCPServer::Cleanup - Unexpected exception") );
	}

	return hr;
}

STDMETHODIMP CIOCPServer::OpenService( const unsigned long &ulnAddressToListenOn,
			const unsigned short &usnPortToListenOn )
{
	HRESULT hr = E_FAIL;

	try
	{
		Open( ulnAddressToListenOn, usnPortToListenOn );
		
		StartAcceptingConnections();

		hr = S_OK;
	}
	catch( const CException &e )
	{
		Output( _T("CIOCPServer::OpenService Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage() );
	}
	catch(...)
	{
		Output( _T("CIOCPServer::OpenService - Unexpected exception") );
	}	

	return hr;
}

STDMETHODIMP CIOCPServer::CloseService()
{
	HRESULT hr = E_FAIL;

	try
	{
		StopAcceptingConnections();

		hr = S_OK;
	}
	catch( const CException &e )
	{
		Output( _T("CIOCPServer::CloseService Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage() );
	}
	catch(...)
	{
		Output( _T("CIOCPServer::CloseService - Unexpected exception") );
	}	

	return hr;
}

STDMETHODIMP CIOCPServer::RegisterMsgFilter( LPVOID lpParam, 
		CALLBACK_SERVER_EVENT pfnEventNotify )
{
	m_lpCallBackParam = lpParam;
	m_pfnCallBackServerEvent = pfnEventNotify;

	return S_OK;
}

STDMETHODIMP CIOCPServer::PreparePackSink( )
{

	return S_OK;
}

/*
 * [GUI 07/09] Bao cao [GUI-DO] moi 10 s, doc lai [Server] MoPhongNhanBan (bat/tat khong can khoi dong lai).
 * Goi tu SendPackToClient (luong chinh).
 */
void CIOCPServer::_GuiDoBaoCao()
{
	const DWORD uNay = ::GetTickCount();

	if ( gs_uBaoCao == 0 )
	{
		gs_uBaoCao = uNay;
		return;
	}

	if ( (DWORD)( uNay - gs_uBaoCao ) < 10000 )
		return;

	const double giay = (double)( uNay - gs_uBaoCao ) / 1000.0;
	gs_uBaoCao = uNay;

	const int nNhip = ( gs_nXa > 0 ) ? gs_nXa : 1;

	GuiDoGhi( "[GUI-DO] %.0fs: goi=%d (dem day gui ngay=%d) byte=%I64d t_goi=%.2f ms | xa=%d nhip, client_xa=%d byte_xa=%I64d (%.1f KB/s) t_xa=%.2f ms (Write %d lan, %.1f us/lan) | moi nhip: goi %.3f ms (max %.3f) + xa %.3f ms (max %.3f) | khoa_rieng=%d client=%u",
		giay, gs_nGoi, gs_nGoiTran, gs_nByteGoi, GuiDoMs( gs_tGoi ),
		gs_nXa, gs_nXaClient, gs_nByteXa, (double)gs_nByteXa / 1024.0 / giay, GuiDoMs( gs_tXa ),
		gs_nWrite, gs_nWrite ? ( GuiDoMs( gs_tWrite ) * 1000.0 / (double)gs_nWrite ) : 0.0,
		GuiDoMs( gs_tGoi ) / nNhip, GuiDoMs( gs_tGoiNhipMax ), GuiDoMs( gs_tXa ) / nNhip, GuiDoMs( gs_tXaMax ),
		m_nGuiKhoaRieng, (unsigned)m_usedClientNode.size() );

	if ( gs_nNhanBan > 1 )
	{
		const double msThat = GuiDoMs( gs_tGoi ) + GuiDoMs( gs_tXa );
		const double msGia  = GuiDoMs( gs_tNB ) + GuiDoMs( gs_tNBXa );
		GuiDoGhi( "[GUI-NB] %.0fs nhan ban x%d (%d client gia): goi=%d t=%.2f ms | xa=%d byte=%I64d (%.1f KB/s) t=%.2f ms | moi nhip: goi %.3f ms (max %.3f) + xa %.3f ms (max %.3f) => duong gui uoc tinh moi nhip %.3f ms (that %.3f + gia %.3f)",
			giay, gs_nNhanBan, gs_nNhanBan - 1, gs_nGoiNB, GuiDoMs( gs_tNB ),
			gs_nXaNB, gs_nByteNB, (double)gs_nByteNB / 1024.0 / giay, GuiDoMs( gs_tNBXa ),
			GuiDoMs( gs_tNB ) / nNhip, GuiDoMs( gs_tNBNhipMax ), GuiDoMs( gs_tNBXa ) / nNhip, GuiDoMs( gs_tNBXaMax ),
			( msThat + msGia ) / nNhip, msThat / nNhip, msGia / nNhip );
	}

	gs_nGoi = 0; gs_nGoiTran = 0; gs_nByteGoi = 0; gs_tGoi = 0; gs_tGoiNhipMax = 0;
	gs_nXa = 0; gs_nXaClient = 0; gs_nByteXa = 0; gs_tXa = 0; gs_tXaMax = 0; gs_nWrite = 0; gs_tWrite = 0;
	gs_tNB = 0; gs_tNBNhipMax = 0; gs_tNBXa = 0; gs_tNBXaMax = 0; gs_nByteNB = 0; gs_nGoiNB = 0; gs_nXaNB = 0;

	/*
	 * Doc lai config moi 10 s: MoPhongNhanBan bat/tat khong can khoi dong lai. Node gia chi luong chinh cham
	 * (PackDataToClient / SendPackToClient / day) nen cap lai ngay tai day la an toan.
	 */
	int nMoi = (int)::GetPrivateProfileIntA( "Server", "MoPhongNhanBan", 0, ".\\config.ini" );
	if ( nMoi < 0 ) nMoi = 0;
	if ( nMoi > 2000 ) nMoi = 2000;

	if ( nMoi > 1 && gs_nNhanBanCap < nMoi - 1 )
	{
		NB_NODE *pMoi = new NB_NODE[ nMoi - 1 ];
		for ( int k = 0; k < nMoi - 1; k++ )
		{
			pMoi[k].pBuf = new BYTE[ gs_nNBBufMax + 64 ];
			pMoi[k].used = 0;
		}
		delete [] gs_pNB;
		gs_pNB = pMoi;
		gs_nNhanBanCap = nMoi - 1;
	}

	if ( nMoi != gs_nNhanBan )
	{
		GuiDoGhi( "[GUI-NB] doi MoPhongNhanBan %d -> %d (node gia da cap %d, dem %u byte/node)", gs_nNhanBan, nMoi, gs_nNhanBanCap, (unsigned)gs_nNBBufMax );
		gs_nNhanBan = nMoi;
	}
}

/*
 * [GUI 07/09] Mo phong dung viec ban that lam khi xa mot client: cap bo dem IOCP, ghi dau goi, chep, ma hoa
 * KSG, tha bo dem. Chi KHONG WSASend (khong co socket). Goi duoi khoa nb.cs.
 */
void CIOCPServer::_NhanBanXaMot( NB_NODE &nb )
{
	static const size_t s_len_protocol = sizeof( WORD );

	if ( nb.used == 0 )
		return;

	try
	{
		CIOBuffer *pBuffer = Allocate();

		if ( pBuffer )
		{
			const size_t headlength = s_len_protocol + nb.used;
			pBuffer->AddData( reinterpret_cast< const char * >( &headlength ), s_len_protocol );
			pBuffer->AddData( reinterpret_cast< const char * >( nb.pBuf ), nb.used );

			unsigned uKey = 0x5A17C3E9u;
			KSG_EncodeBuf( nb.used, (unsigned char *)( pBuffer->GetBuffer() + s_len_protocol ), &uKey );

			pBuffer->Release();
		}
	}
	catch( ... )
	{
	}

	gs_nByteNB += (LONGLONG)( nb.used + s_len_protocol );
	gs_nXaNB++;
	nb.used = 0;
}

/*
 * [GUI 07/09] Phep thu nhan ban, phan PackDataToClient: lam them N-1 lan dung trinh tu that
 * (khoa chung neu che do cu -> tra node -> khoa rieng -> kiem dem day -> chep).
 */
void CIOCPServer::_NhanBanGoi( const void * const pData, const size_t &datalength, const unsigned long &ulnClientID )
{
	const int nSo = gs_nNhanBan - 1;

	if ( nSo <= 0 || !gs_pNB || gs_nNhanBanCap < nSo || datalength == 0 || datalength >= gs_nNBBufMax )
		return;

	const LONGLONG tBatDau = GuiDoQpc();

	for ( int k = 0; k < nSo; k++ )
	{
		CKhoaTuyChon khoaChung( m_csCM, m_nGuiKhoaRieng == 0 );

		LPCLIENT_NODE pCN = _Node( ulnClientID );

		if ( !pCN )
			break;

		NB_NODE &nb = gs_pNB[k];

		CCriticalSection::Owner lock( nb.cs );

		if ( ( nb.used >= gs_nNBBufMax ) || ( nb.used + datalength >= gs_nNBBufMax ) )
		{
			_NhanBanXaMot( nb );
		}

		memcpy( nb.pBuf + nb.used, pData, datalength );
		nb.used += datalength;
	}

	gs_nGoiNB += nSo;

	const LONGLONG tHet = GuiDoQpc() - tBatDau;
	gs_tNB += tHet;
	gs_tNBNhip += tHet;
}

/*
 * [GUI 07/09] Phep thu nhan ban, phan SendPackToClient: xa N-1 node gia moi nhip.
 */
void CIOCPServer::_NhanBanXa()
{
	const int nSo = gs_nNhanBan - 1;

	if ( nSo <= 0 || !gs_pNB || gs_nNhanBanCap < nSo )
		return;

	const LONGLONG tBatDau = GuiDoQpc();

	for ( int k = 0; k < nSo; k++ )
	{
		NB_NODE &nb = gs_pNB[k];

		CCriticalSection::Owner lock( nb.cs );

		if ( nb.used > 0 )
			_NhanBanXaMot( nb );
	}

	const LONGLONG tHet = GuiDoQpc() - tBatDau;
	gs_tNBXa += tHet;
	if ( tHet > gs_tNBXaMax )
		gs_tNBXaMax = tHet;
}

STDMETHODIMP CIOCPServer::PackDataToClient( const unsigned long &ulnClientID,
			const void * const	pData,
			const size_t		&datalength )
{
	HRESULT hr = E_FAIL;

	if ( ulnClientID < m_nPlayerMaxCount && datalength > 0 )
	{
		/*
		 * [GUI 07/09] Khong lay m_csCM nua (truoc: 2 khoa moi goi, 83.000 goi/nhip khi 500 nguoi). Bang node
		 * co dinh sau ctor; pSocket duoc doc va dung duoi khoa RIENG csWriteAction, va noi dat pSocket = NULL
		 * (_HelperDelClient / ShutdownClient) cung lay khoa do => socket khong bao gio bi tha khi dang dung.
		 * GuiKhoaRieng=0 lay lai m_csCM truoc csWriteAction, dung thu tu cu.
		 */
		const LONGLONG tBatDau = GuiDoQpc();

		CKhoaTuyChon khoaChung( m_csCM, m_nGuiKhoaRieng == 0 );

		LPCLIENT_NODE pCN = _Node( ulnClientID );

		if ( pCN )
		{
			CCriticalSection::Owner	lock( pCN->csWriteAction );

			Socket *pSock = pCN->pSocket;

			if ( pSock )
			{
				try
				{
					CIOBuffer *pWriteBuffer = pCN->pWriteBuffer;

					const size_t nNetworkBufferLen = pWriteBuffer->GetUsed();

					if ( ( nNetworkBufferLen >= m_nNetworkBufferMaxLen ) || 
						( nNetworkBufferLen + datalength >= m_nNetworkBufferMaxLen ) )
					{
						/*
						 * It need send to client at once, because of buffer is full
						 */
						const BYTE *pPackData = pWriteBuffer->GetBuffer();
						const size_t used = pWriteBuffer->GetUsed();

						_SendDataEx( pCN, pPackData, used );

						pWriteBuffer->Empty();

						gs_nGoiTran++;
					}

					pWriteBuffer->AddData( static_cast< const char * const >( pData ), datalength );

					gs_nGoi++;
					gs_nByteGoi += (LONGLONG)datalength;

					hr = S_OK;
				}
				catch( const CException &e )
				{
					Output( _T("CIOCPServer::PackDataToClient Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage() );

					pSock->Shutdown();
				}
				catch(...)
				{
					Output( _T("CIOCPServer::PackDataToClient - Unexpected exception") );

					pSock->Shutdown();
				}
			}
		}

		const LONGLONG tHet = GuiDoQpc() - tBatDau;
		gs_tGoi += tHet;
		gs_tGoiNhip += tHet;

		if ( hr == S_OK && gs_nNhanBan > 1 )
		{
			_NhanBanGoi( pData, datalength, ulnClientID );
		}
	}

	return hr;
}

STDMETHODIMP CIOCPServer::SendPackToClient( const unsigned long &ulnClientID /* -1 */ )
{
	HRESULT hr = E_FAIL;

	static const size_t s_len_protocol = sizeof( WORD );

	/*
	 * [GUI 07/09] Truoc: giu m_csCM suot vong duyet moi client (500 client = vai ms moi nhip, luong IOCP nhan
	 * goi phai cho). Nay: chup danh sach client dang dung duoi m_csCM (vai micro giay), roi xa tung client chi
	 * duoi khoa rieng csWriteAction cua no. Chi cap bo dem khi that su co du lieu (truoc: cap+tha moi client
	 * moi nhip du rong).
	 */
	const LONGLONG tBatDau = GuiDoQpc();

	std::vector< size_t > vDangDung;

	{
		CCriticalSection::Owner locker( m_csCM );

		vDangDung.assign( m_usedClientNode.begin(), m_usedClientNode.end() );
	}

	CKhoaTuyChon khoaChung( m_csCM, m_nGuiKhoaRieng == 0 );

	for ( size_t i = 0; i < vDangDung.size(); i++ )
	{
		LPCLIENT_NODE pCN = _Node( vDangDung[i] );

		if ( !pCN )
			continue;

		CCriticalSection::Owner lock( pCN->csWriteAction );

		Socket *pSock = pCN->pSocket;

		if ( !pSock )
			continue;

		CIOBuffer *pBuffer = NULL;
		try
		{
			const BYTE * pPackData = pCN->pWriteBuffer->GetBuffer();
			const size_t used = pCN->pWriteBuffer->GetUsed();

			if ( used > 0 )
			{
				pBuffer = Allocate();

				/*
				 * Add package header
				 */
				const size_t headlength = s_len_protocol + used;
				pBuffer->AddData( reinterpret_cast< const char * >( &headlength ), s_len_protocol );

				pBuffer->AddData( reinterpret_cast< const char * >( pPackData ), used );

				_ASSERT(pCN->uKeyMode == 0);
				KSG_EncodeBuf(
					used,
					(unsigned char *)(pBuffer->GetBuffer() + s_len_protocol),
					&pCN->uServerKey
				);

				pCN->pWriteBuffer->Empty();

				const LONGLONG tW = GuiDoQpc();
				pSock->Write( pBuffer, true );
				gs_tWrite += GuiDoQpc() - tW;
				gs_nWrite++;

				gs_nXaClient++;
				gs_nByteXa += (LONGLONG)( used + s_len_protocol );

				pBuffer->Release();
				pBuffer = NULL;
			}

			hr = S_OK;
		}
		catch( const CException &e )
		{
			if (pBuffer)
			{
				pBuffer->Release();	
				pBuffer = NULL;
			}

			Output( _T("CIOCPServer::SendPackToClient Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage() );

			pSock->Shutdown();
		}
		catch(...)
		{
			if (pBuffer)
			{
				pBuffer->Release();	
				pBuffer = NULL;
			}

			Output( _T("CIOCPServer::SendPackToClient - Unexpected exception") );

			pSock->Shutdown();
		}
	}

	const LONGLONG tHet = GuiDoQpc() - tBatDau;
	gs_tXa += tHet;
	if ( tHet > gs_tXaMax )
		gs_tXaMax = tHet;
	if ( gs_tGoiNhip > gs_tGoiNhipMax )
		gs_tGoiNhipMax = gs_tGoiNhip;
	gs_tGoiNhip = 0;
	if ( gs_tNBNhip > gs_tNBNhipMax )
		gs_tNBNhipMax = gs_tNBNhip;
	gs_tNBNhip = 0;
	gs_nXa++;

	if ( gs_nNhanBan > 1 )
		_NhanBanXa();

	_GuiDoBaoCao();

	return hr;
}

STDMETHODIMP CIOCPServer::SendData( const unsigned long &ulnClientID,
					 const void * const	pData,
					 const size_t		&datalength )
{
	HRESULT hr = E_FAIL;

	static const size_t s_len_protocol = sizeof( WORD );

	if ( NULL == pData || 0 == datalength )
	{
		return hr;
	}

	if ( ulnClientID < m_nPlayerMaxCount )
	{
		// [GUI 07/09] khoa rieng csWriteAction thay m_csCM (xem PackDataToClient)
		CKhoaTuyChon khoaChung( m_csCM, m_nGuiKhoaRieng == 0 );

		LPCLIENT_NODE pCN = _Node( ulnClientID );

		if ( pCN )
		{
			CCriticalSection::Owner lock( pCN->csWriteAction );

			Socket *pSock = pCN->pSocket;

			if ( pSock )
			{
				CIOBuffer *pBuffer = NULL;
				try
				{
					pBuffer = Allocate();

					/*
					 * Add package header
					 */
					const size_t headlength = s_len_protocol + datalength;
					pBuffer->AddData( reinterpret_cast< const char * >( &headlength ), s_len_protocol );

					/*
					 * Add data into buffer that it is will be send
					 */
					pBuffer->AddData( reinterpret_cast< const char * >( pData ), datalength );

					_ASSERT(pCN->uKeyMode == 0);
					KSG_EncodeBuf(
						datalength,
						(unsigned char *)(pBuffer->GetBuffer() + s_len_protocol),
						&pCN->uServerKey
					);

					pSock->Write( pBuffer, true );

					pBuffer->Release();
					pBuffer = NULL;

					hr = S_OK;
				}
				catch( const CException &e )
				{
					if (pBuffer)
					{
						pBuffer->Release();
						pBuffer = NULL;
					}

					Output( _T("CIOCPServer::SendData Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage() );

					pSock->Shutdown();
				}
				catch(...)
				{
					if (pBuffer)
					{
						pBuffer->Release();
						pBuffer = NULL;
					}

					Output( _T("CIOCPServer::SendData - Unexpected exception") );

					pSock->Shutdown();
				}
			}
		}
	}

	return hr;
}

bool CIOCPServer::_SendDataEx( LPCLIENT_NODE pNode,
				 const void * const	pData,
				 const size_t		&datalength )
{
	static const size_t s_len_protocol = sizeof( WORD );

	bool ok = false;

	if ( !pNode || !pData || !datalength )
	{
		return false;
	}
	
	if ( pNode->pSocket )
	{
		CIOBuffer *pBuffer = NULL;
		try
		{
			pBuffer = Allocate();
			
			/*
			* Add package header
			*/
			const size_t headlength = s_len_protocol + datalength;
			pBuffer->AddData( reinterpret_cast< const char * >( &headlength ), s_len_protocol );
			
			/*
			* Add data into buffer that it is will be send
			*/
			pBuffer->AddData( reinterpret_cast< const char * >( pData ), datalength );
			
			_ASSERT(pNode->uKeyMode == 0);
			KSG_EncodeBuf(
				datalength,
				(unsigned char *)(pBuffer->GetBuffer() + s_len_protocol),
				&pNode->uServerKey
				);
			
			pNode->pSocket->Write( pBuffer, true );
			
			pBuffer->Release();			
			
			ok = true;
		}
		catch( const CException &e )
		{
			// add by spe 2003/07/06
			if (pBuffer)
			{
				pBuffer->Release();	
				pBuffer = NULL;
			}

			Output( _T("CIOCPServer::_SendDataEx Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage() );
			
			pNode->pSocket->Shutdown();
		}
		catch(...)
		{
			// add by spe 2003/07/06
			if (pBuffer)
			{
				pBuffer->Release();	
				pBuffer = NULL;
			}

			Output( _T("CIOCPServer::_SendDataEx - Unexpected exception") );
			
			pNode->pSocket->Shutdown();
		}
	}

	return ok;
}

STDMETHODIMP_( const void * ) CIOCPServer::GetPackFromClient( const unsigned long &ulnClientID,
			size_t &datalength )
{
	if ( ulnClientID < m_nPlayerMaxCount )
	{
		// [GUI 07/09] khoa rieng csReadAction thay m_csCM; pSocket doc va dung duoi khoa do (_HelperDelClient
		// va ShutdownClient dat NULL duoi cung khoa). ProcessCommand ben trong lay lai csReadAction (de quy, OK).
		CKhoaTuyChon khoaChung( m_csCM, m_nGuiKhoaRieng == 0 );

		LPCLIENT_NODE pCN = _Node( ulnClientID );

		if ( pCN )
		{
			CCriticalSection::Owner lock( pCN->csReadAction );

			Socket *pSock = pCN->pSocket;

			if ( pSock )
			{
				pCN->pReadBuffer->Empty();

				pCN->pRecvBuffer = ProcessDataStream( pSock, pCN->pRecvBuffer );

				const char *pData = reinterpret_cast< const char * >( pCN->pReadBuffer->GetBuffer() );
				datalength = pCN->pReadBuffer->GetUsed();

				return pData;
			}
		}
	}

	datalength = 0;
	
	return NULL;
}

STDMETHODIMP CIOCPServer::ShutdownClient( const unsigned long &ulnClientID )
{
	if ( ulnClientID < m_nPlayerMaxCount )
	{
		LPCLIENT_NODE pCN = NULL;

		CCriticalSection::Owner	locker( m_csCM );
			
		pCN = _Node( ulnClientID );

		if ( pCN )
		{
			// [GUI 07/09] duong nong chi giu khoa rieng -> phai lay ca hai khoa rieng truoc khi tha socket
			CCriticalSection::Owner lockW( pCN->csWriteAction );
			CCriticalSection::Owner lockR( pCN->csReadAction );

			if ( pCN->pSocket )
			{
				// Fix by BladeKnight109 21042020
				//pCN->pSocket->AbortiveClose();
				pCN->pSocket->Shutdown();
				pCN->pSocket = NULL;
			}

			pCN->pReadBuffer->Empty();
			pCN->pRecvBuffer->Empty();

			pCN->pWriteBuffer->Empty();

		}
	}

	return S_OK;
}

STDMETHODIMP_( size_t ) CIOCPServer::GetClientCount( )
{
//	CCriticalSection::Owner locker( m_csCM );
	
	return m_usedClientNode.size();
}

STDMETHODIMP_( const char * ) CIOCPServer::GetClientInfo(
					const unsigned long &ulnClientID )
{

	if ( ulnClientID < m_nPlayerMaxCount )
	{
		LPCLIENT_NODE pCN = NULL;

		CCriticalSection::Owner	locker( m_csCM );

		pCN = ( LPCLIENT_NODE )( m_theClientManager[ulnClientID] );

		if ( pCN && pCN->pSocket )
		{
			const CPerConnectionData *pData = reinterpret_cast< const CPerConnectionData * >( pCN->pSocket->GetUserPtr() );
			
			if ( pData )
			{
				return pData->GetConnectionDetails();
			}
		}
	}

	return NULL;
}

STDMETHODIMP CIOCPServer::QueryInterface( REFIID riid, void** ppv )
{
	/*
	 * By definition all COM objects support the IUnknown interface
	 */
	if( riid == IID_IUnknown )
	{
		AddRef();

		*ppv = dynamic_cast< IUnknown * >( this );
	}
	else if ( riid == IID_IIOCPServer )
	{
		AddRef();

		*ppv = dynamic_cast< IServer * >( this );	
	}
	else
	{
		*ppv = NULL;

		return E_NOINTERFACE;
	}

	return S_OK;
}
    
STDMETHODIMP_(ULONG) CIOCPServer::AddRef()
{
	return ::InterlockedIncrement( &m_lRefCount );
}

STDMETHODIMP_(ULONG) CIOCPServer::Release()
{
	if ( ::InterlockedDecrement( &m_lRefCount ) > 0 )
	{
		return m_lRefCount;
	}

	delete this;
	return 0L;
}

void CIOCPServer::OnConnectionEstablished( Socket *pSocket,
			 OnlineGameLib::Win32::CIOBuffer *pAddress )
{
	if ( pSocket )
	{
		pSocket->AddRef();

		/*
		 * Allocate per connection data
		 */
		CSocketAddress address( reinterpret_cast< const sockaddr * >( pAddress->GetBuffer() ) );
		
		CPerConnectionData *pData = new CPerConnectionData( address.GetAsString(), ( WPARAM )( -1 ), ( LPARAM )( pSocket ) );
		
		ASSERT( pData );
		
		m_theAddClientQueue.Push( ( void * )pData );
	}
}

bool CIOCPServer::OnConnectionClosing( Socket *pSocket )
{
	if ( pSocket )
	{
		pSocket->Close();
	}

	return true;
}

void CIOCPServer::OnConnectionClosed( Socket *pSocket )
{	
	if ( pSocket )
	{
		pSocket->AddRef();

		/*
		 * Release per connection data
		 */
		CPerConnectionData *pData = reinterpret_cast< CPerConnectionData * >( pSocket->GetUserPtr() );

		m_theDelClientQueue.Push( pData );
	}
}

void CIOCPServer::ReadCompleted( Socket *pSocket, OnlineGameLib::Win32::CIOBuffer *pBuffer )
{
	if (pSocket->GetMessageCallback())
	{
		ProcessMessage(pSocket, pBuffer);
		return;
	}

	try
	{
		const CPerConnectionData *pData = reinterpret_cast< const CPerConnectionData * >( pSocket->GetUserPtr() );

		if ( pData )
		{
			DWORD dwIndex = pData->GetConnectionFirstParam();
			
			const BYTE *pPackData = pBuffer->GetBuffer();
			const size_t used = pBuffer->GetUsed();
			
			LPCLIENT_NODE pCN = NULL;
			{
				// [GUI 07/09] chi khoa rieng csReadAction (bang node co dinh, dwIndex duoc kiem bien trong _Node;
				// truoc day dwIndex = -1 cua ket noi bi tu choi con chen mot muc NULL vao map). GuiKhoaRieng=0 nhu cu.
				CKhoaTuyChon khoaChung( m_csCM, m_nGuiKhoaRieng == 0 );

				pCN = _Node( dwIndex );

				if ( pCN )
				{
					CCriticalSection::Owner lock( pCN->csReadAction );

					pCN->pRecvBuffer->AddData( pPackData, used );
				}
			}
		}
		
		pBuffer->Empty();
		
		pSocket->Read( pBuffer );
	}
	catch( const CException &e )
	{
		Output( _T("ReadCompleted - Exception - ") + e.GetWhere() + _T(" - ") + e.GetMessage() );

		pSocket->Shutdown();
	}
	catch(...)
	{
		Output( _T("ReadCompleted - Unexpected exception") );

		pSocket->Shutdown();
	}
}

CIOBuffer *CIOCPServer::ProcessDataStream( Socket *pSocket, CIOBuffer *pBuffer)
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
				Output( _T("Too much data! found error and close this socket!") );
				
				/*
				 * Shutdown the sending side of the socket.
				 */
				pSocket->Shutdown();
				
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
			ProcessCommand( pSocket, pBuffer );
			
			pBuffer->Empty();
		}
		else if ( used > messageSize )
		{
			Output( _T("Got message plus extra data") );
			
			/*
			* we have a message, plus some more data
			* allocate a new buffer, copy the extra data into it and try again
			*/
			
			CIOBuffer *pMessage = pBuffer->SplitBuffer( messageSize );
			
			ProcessCommand( pSocket, pMessage );
			
			pMessage->Release();			
		}
	}
	
	/*
	 * not enough data in the buffer, reissue a read into the same buffer to collect more data
	 */
	return pBuffer;
}

size_t CIOCPServer::GetMinimumMessageSize()
{
	static size_t length = sizeof( WORD ) + sizeof( BYTE );

	/*
     * The smallest possible command we accept is a byte onlye package
	 */
	return length;
}

size_t CIOCPServer::GetMessageSize( const CIOBuffer *pBuffer )
{
	const BYTE *pData = pBuffer->GetBuffer();   
	const size_t used = pBuffer->GetUsed();

	WORD wHeadLen = ( WORD )( *( WORD * )( pData ) );

	return ( size_t )( wHeadLen );
}

void CIOCPServer::ProcessCommand( Socket *pSocket, const CIOBuffer *pBuffer )
{
	static const size_t s_len_protocol = sizeof( WORD );

	const BYTE *pData = pBuffer->GetBuffer();   
	const size_t used = pBuffer->GetUsed();

	//ASSERT( used > s_len_protocol );

	if ( used <= s_len_protocol )
	{
        return;
	}

	const CPerConnectionData *pPCD = reinterpret_cast< const CPerConnectionData * >( pSocket->GetUserPtr() );

	if ( !pPCD )
	{
		return;
	}

	DWORD dwIndex = pPCD->GetConnectionFirstParam();
	LPCLIENT_NODE pCN = NULL;
	{
		// [GUI 07/09] khoa rieng csReadAction thay m_csCM (goi tu GetPackFromClient dang giu csReadAction: de quy)
		CKhoaTuyChon khoaChung( m_csCM, m_nGuiKhoaRieng == 0 );

		if ( NULL == ( pCN = _Node( dwIndex ) ) )
		{
			return;
		}

		CCriticalSection::Owner lockR( pCN->csReadAction );
		
        // Add by Freeway Chen
        _ASSERT(pCN->uKeyMode == 0);

        KSG_DecodeBuf(
            used - s_len_protocol,
            const_cast<unsigned char *>(pData + s_len_protocol),
            &(pCN->uClientKey)
        );

		pCN->pReadBuffer->AddData( ( const BYTE * )( pData + s_len_protocol ), used - s_len_protocol );
	}
}

CServerFactory::CServerFactory()
		: m_lRefCount( 0 ),
		m_nPlayerMaxCount( 0 ),
		m_nPrecision( 0 ),
		m_maxFreeBuffers_Cache( 0 ),
		m_bufferSize_Cache( 0 )
{

}

CServerFactory::~CServerFactory()
{

}

STDMETHODIMP CServerFactory::SetEnvironment( const size_t &nPlayerMaxCount,
			const size_t &nPrecision,
			const size_t &maxFreeBuffers_Cache,
			const size_t &bufferSize_Cache )
{
	m_nPlayerMaxCount = nPlayerMaxCount;
	m_nPrecision = nPrecision;

	m_maxFreeBuffers_Cache = maxFreeBuffers_Cache;
	m_bufferSize_Cache = bufferSize_Cache;

	return S_OK;
}

STDMETHODIMP CServerFactory::CreateServerInterface( REFIID riid, void** ppv )
{
	HRESULT hr = E_NOINTERFACE;

	if ( IID_IIOCPServer == riid )
	{		
		const size_t maxPlayerCount = ( m_nPlayerMaxCount > 0 ) ? m_nPlayerMaxCount : 100;
		const size_t precision = ( m_nPrecision > 0 ) ? m_nPrecision : 10;

		const size_t maxFreeBuffers_Cache = ( m_maxFreeBuffers_Cache > 0 ) ? m_maxFreeBuffers_Cache : 10;
		const size_t bufferSize_Cache = ( m_bufferSize_Cache > 0 ) ? m_bufferSize_Cache : 8192;

		const size_t maxFreeBuffers = 10240;

		try
		{
			CIOCPServer *pObject = new CIOCPServer( maxPlayerCount, 
											precision, 
											maxPlayerCount,
											maxFreeBuffers,
											maxFreeBuffers_Cache,
											bufferSize_Cache );
			
			*ppv = reinterpret_cast< void * > ( dynamic_cast< IServer * >( pObject ) );
			
			if ( *ppv )
			{
				reinterpret_cast< IUnknown * >( *ppv )->AddRef();
				
				hr = S_OK;
			}
		}
		catch( ... )
		{
			TRACE( "CServerFactory::CreateServerInterface exception!" );
		}
	}

	if ( FAILED( hr ) )
	{
		*ppv = NULL;
	}

	return ( HRESULT )( hr );
}

STDMETHODIMP CServerFactory::QueryInterface( REFIID riid, void** ppv )
{
	/*
	 * By definition all COM objects support the IUnknown interface
	 */
	if( riid == IID_IUnknown )
	{
		AddRef();

		*ppv = dynamic_cast< IUnknown * >( this );
	}
	else if ( riid == IID_IServerFactory )
	{
		AddRef();

		*ppv = dynamic_cast< IServerFactory * >( this );	
	}
	else
	{
		*ppv = NULL;

		return E_NOINTERFACE;
	}

	return S_OK;
}
    
STDMETHODIMP_(ULONG) CServerFactory::AddRef()
{
	return ::InterlockedIncrement( &m_lRefCount );
}

STDMETHODIMP_(ULONG) CServerFactory::Release()
{
	if ( ::InterlockedDecrement( &m_lRefCount ) > 0 )
	{
		return m_lRefCount;
	}

	delete this;
	return 0L;
}

unsigned int __stdcall CIOCPServer::HelperThreadFunction( void *pParam )
{
	CIOCPServer *pThis = ( CIOCPServer * )( pParam );

	ASSERT( pThis );

	try
	{
		pThis->_HelperThreadFunction();
	}
	catch( ... )
	{
		TRACE( "CIOCPServer::HelperThreadFunction exception!" );
	}

	return 0L;
}

unsigned int CIOCPServer::_HelperThreadFunction()
{
	bool bAdd, bDel;

	while ( !m_hQuitHelper.Wait( 0 ) )
	{
		bAdd = _HelperAddClient();

		bDel = _HelperDelClient();

		if ( !bAdd && !bDel )
		{
			::Sleep( 1 );
		}
	}

	return 0L;
}

bool CIOCPServer::_HelperAddClient()
{
   	static const size_t s_len_protocol = sizeof( WORD );

	CPerConnectionData *pData = ( CPerConnectionData * )( m_theAddClientQueue.Pop() );

	if ( NULL == pData )
	{
		return false;
	}

	Socket *pSocket = ( Socket * )( pData->GetConnectionSecondParam() );

	ASSERT( pSocket );
	
	USES_CONVERSION;

	DWORD dwIndex = (DWORD)( -1 );

	if ( pSocket )
	{
		/*
		 * Get ID from the free table
		 */
		{
			CCriticalSection::Owner locker( m_csFCN );

			if ( !m_freeClientNode.empty() )
			{
				dwIndex = m_freeClientNode.front();
				m_freeClientNode.pop();
				//dwIndex = m_freeClientNode.top();
				//m_freeClientNode.pop();
			}
		}

		pData->SetConnectionFirstParam( dwIndex );

		pSocket->SetUserPtr( pData );

		/*
		 * Store this socket if we can use it
		 */
		if ( dwIndex != (DWORD)( -1 ) )
		{
			/*
			 * Store socket into array
			 */
			{
				CCriticalSection::Owner	locker( m_csCM );

				// [GUI 07/09] duong gui/nhan chi giu khoa rieng: lay ca hai khoa rieng, dat khoa ma truoc,
				// pSocket SAU CUNG, de khong luong nao thay pSocket khac NULL ma khoa/dem chua san sang.
				LPCLIENT_NODE pCNMoi = _Node( dwIndex );

				if ( pCNMoi )
				{
					CCriticalSection::Owner lockW( pCNMoi->csWriteAction );
					CCriticalSection::Owner lockR( pCNMoi->csReadAction );

					pCNMoi->uServerKey = _Rand();
					pCNMoi->uClientKey = _Rand();
					pCNMoi->uKeyMode   = 0;

					pCNMoi->pWriteBuffer->Empty();
					pCNMoi->pReadBuffer->Empty();
					pCNMoi->pRecvBuffer->Empty();

					pCNMoi->pSocket    = pSocket;
				}

				/*
				 * Set ID into the used table
				 */
				m_usedClientNode.push_back( dwIndex );
			}

            #pragma pack(1)
            struct  {
                WORD wLen;
                ACCOUNT_BEGIN AccountBegin;
            } SendAccountBegin;
            #pragma pack()

            RandMemSet(sizeof(SendAccountBegin), (unsigned char *)&SendAccountBegin);

			{
				CCriticalSection::Owner	locker( m_csCM );

                SendAccountBegin.wLen = s_len_protocol + sizeof(SendAccountBegin.AccountBegin);

                SendAccountBegin.AccountBegin.ProtocolType = CIPHER_PROTOCOL_TYPE;
                SendAccountBegin.AccountBegin.Mode         = m_theClientManager[dwIndex]->uKeyMode;
                SendAccountBegin.AccountBegin.ServerKey    = ~m_theClientManager[dwIndex]->uServerKey;
                SendAccountBegin.AccountBegin.ClientKey    = ~m_theClientManager[dwIndex]->uClientKey;
				
			}

            pSocket->Write((BYTE *)&SendAccountBegin, sizeof(SendAccountBegin));

			/*
			 Output( GetTimeStamp() + _T("Add a client [ID:") + 
			 ToString( dwIndex ) + _T( "] - Current total is " ) +
			 ToString( m_usedClientNode.size() ) );
			 */
			/*
			 * Post a read command to IOCP
			 * Enter into a process that it can get data from network
			 */
			
			if ( m_pfnCallBackServerEvent )
			{
				m_pfnCallBackServerEvent( m_lpCallBackParam, dwIndex, enumClientConnectCreate );
			}
			
//edit by phong kieu thong bao login tai consol GS
//			Trace( pData->GetConnectionDetails().c_str(), GetTimeStamp() + _T(" Add a client [ID:") + 
//				ToString( dwIndex ) + _T( "] - Current total is " ) +
//				ToString( m_usedClientNode.size() ) );

			pSocket->Read();			
		}
		else
		{
			pSocket->AbortiveClose();
		}

		SAFE_RELEASE( pSocket );

		return true;
	}
	else
	{
		SAFE_DELETE( pData );
	}

	return false;
}

bool CIOCPServer::_HelperDelClient()
{
	CPerConnectionData *pData = ( CPerConnectionData * )( m_theDelClientQueue.Pop() );

	if ( NULL == pData )
	{
		return false;
	}
	
	Socket *pSocket = ( Socket * )( pData->GetConnectionSecondParam() );
	
	if ( pSocket )
	{
		pSocket->SetUserData( NULL );
	}
	
	DWORD dwIndex = pData->GetConnectionFirstParam();
	
	if ( ( DWORD )( -1 ) != dwIndex )
	{
		if ( m_pfnCallBackServerEvent )
		{
			m_pfnCallBackServerEvent( m_lpCallBackParam, dwIndex, enumClientConnectClose );
		}

		LPCLIENT_NODE pCN = NULL;
		{
			CCriticalSection::Owner	locker( m_csCM );

			pCN = _Node( dwIndex );

			if ( pCN )
			{
				// [GUI 07/09] duong nong chi giu khoa rieng -> lay ca hai khoa rieng roi moi dat pSocket = NULL;
				// SAFE_RELEASE( pSocket ) o duoi chi chay sau khi ra khoi day => khong luong nao con dang dung socket.
				CCriticalSection::Owner lockW( pCN->csWriteAction );
				CCriticalSection::Owner lockR( pCN->csReadAction );

				pCN->pSocket = NULL;

				pCN->pReadBuffer->Empty();
				pCN->pRecvBuffer->Empty();
				pCN->pWriteBuffer->Empty();
			}
			
			/*
			* Erase ID from the used table
			*/
			
			m_usedClientNode.remove( dwIndex );
		}
		
		/*
		* A ID of call back is entered into the free table
		*/
		{
			CCriticalSection::Owner locker( m_csFCN );
			
			//m_freeClientNode.push( dwIndex );
			m_freeClientNode.push( dwIndex );
		}

//edit by phong kieu thong bao logout tai consol GS
//		Trace( pData->GetConnectionDetails().c_str(), GetTimeStamp() + _T(" Del a client [ID:") + 
//			ToString( dwIndex ) + _T( "] - Current total is " ) +
//			ToString( m_usedClientNode.size() ) );
		
		/*
		Output( GetTimeStamp() + _T("Del a client [ID:") + 
		ToString( dwIndex ) + _T( "] - Current total is " ) + 
		ToString( m_usedClientNode.size() ) );
		*/		
	}
	
	SAFE_RELEASE( pSocket );
	SAFE_DELETE( pData );

	return true;
}

STDMETHODIMP CIOCPServer::RegisterMsgFilter( const unsigned long ulnClientID, 
		IMessageProcess* pfnMsgNotify )
{
	HRESULT hr = E_FAIL;

	if ( ulnClientID < m_nPlayerMaxCount )
	{
		LPCLIENT_NODE pCN = NULL;

		CCriticalSection::Owner	locker( m_csCM );

		pCN = ( LPCLIENT_NODE )( m_theClientManager[ulnClientID] );

		if ( pCN && pCN->pSocket )
		{
			pCN->pSocket->SetMessageCallback(pfnMsgNotify);
			
			hr = S_OK;
		}
	}

	return hr;
}

void CIOCPServer::ProcessMessage( Socket *pSocket, CIOBuffer *pBuffer )
{
	const size_t used = pBuffer->GetUsed();
	static size_t minMessageSize = sizeof( WORD ) + sizeof( BYTE );
	
	if ( used >= minMessageSize )
	{
		const CPerConnectionData *pPCD = reinterpret_cast< const CPerConnectionData * >( pSocket->GetUserPtr() );

		if ( !pPCD )
		{
			return;
		}

		DWORD dwIndex = pPCD->GetConnectionFirstParam();
		LPCLIENT_NODE pCN;
		{
			// [GUI 07/09] khoa rieng csReadAction thay m_csCM (cung khoa voi GetPackFromClient/ProcessCommand)
			CKhoaTuyChon khoaChung( m_csCM, m_nGuiKhoaRieng == 0 );

			if ( NULL == ( pCN = _Node( dwIndex ) ) )
			{
				return;
			}

			CCriticalSection::Owner lockR( pCN->csReadAction );
			
			// Add by Freeway Chen
			_ASSERT(pCN->uKeyMode == 0);

			const BYTE* pData = pBuffer->GetBuffer();
			WORD processedSize = 0;
			WORD messageSize = ( WORD )( *( WORD * )( pData ) );

			while ( used >= processedSize + messageSize )
			{
				KSG_DecodeBuf(
					messageSize - sizeof(WORD),
					const_cast<unsigned char *>(pData + sizeof( WORD )),
					&(pCN->uClientKey)
					);

				pSocket->GetMessageCallback()->OnMessage((VOID*)(pData+sizeof( WORD )), 
						messageSize - sizeof(WORD));

				pData += messageSize;

				processedSize += messageSize;

				if ( used < processedSize + sizeof(WORD))
				{
					// no more complete message
					break;
				}
				
				messageSize = ( WORD )( *( WORD * )( pData ) );
			}

			if ( used > processedSize )
			{
				pBuffer->RemoveBuffer( processedSize );
			}
			else
			{
				pBuffer->Empty();
			}
		}
	}

	pSocket->Read( pBuffer );
}
