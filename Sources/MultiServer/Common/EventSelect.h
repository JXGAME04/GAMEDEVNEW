/********************************************************************
	created:	2003/02/19
	file base:	EventSelect
	file ext:	h
	author:		liupeng
	
	purpose:	
*********************************************************************/
#ifndef __INCLUDE_EVENTSELECT_H__
#define __INCLUDE_EVENTSELECT_H__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
	#pragma once
#endif

#ifndef _WINDOWS_
	#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#undef WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>

#include "Win32Exception.h"
#ifdef JX_PLATFORM_SDL
#include "JxNetShim.h"
#endif

/*
 * namespace OnlineGameLib::Win32
 */

namespace OnlineGameLib {
namespace Win32 {

#ifdef JX_PLATFORM_SDL
/*
 * [SDL 08/09 2b-2] CEventSelect ban SDL: khong co WSAEventSelect; dung select() tren socket
 * non-blocking, recv(MSG_PEEK) de phan biet FD_READ (co du lieu) / FD_CLOSE (ben kia dong).
 * Giu nguyen API (AssociateEvent/DissociateEvent/WaitForEnumEvent/Read/Write/Connect/Close/IsError)
 * va ma tra ve enumSuccess/enumFail/enumException nhu ban Win32.
 */
class CEventSelect
{
public:

	CEventSelect() : m_lMask( 0 ), m_lEvents( 0 ), m_nErrorCode( 0 ), m_bAssociated( false ), m_bPendingConnect( false ) { memset( m_nErr, 0, sizeof( m_nErr ) ); }
	~CEventSelect() {}

	void AssociateEvent( SOCKET s, long lNetworkEvents )
	{
		JX_NET_TRACE( "[evsel] AssociateEvent s=%d mask=%lx", ( int )s, lNetworkEvents );	// [SDL 08/09 2b-2c]
		m_lMask = lNetworkEvents; m_lEvents = 0; m_nErrorCode = 0; m_bAssociated = true;
		m_bPendingConnect = ( lNetworkEvents & FD_CONNECT ) != 0;	// [SDL 08/09 2b-2b] WSAEventSelect ghi nhan FD_CONNECT ca khi socket da noi -> bao 1 lan o WaitForEnumEvent dau

		if ( !JxNetSetNonBlocking( s, true ) )
		{
			throw CWin32Exception( _T("CEventSelect::AssociateEvent() - non-blocking"), ( DWORD )JxNetLastError() );
		}
	}

	void DissociateEvent() { JX_NET_TRACE( "[evsel] DissociateEvent" ); m_bAssociated = false; m_lEvents = 0; m_bPendingConnect = false; }

	bool WaitForEnumEvent( SOCKET s, DWORD dwTimeout );

	bool IsError() { return ( bool )( ( m_nErrorCode >> 1 ) & 0x1 ); };

	enum enumExitValue
	{
		enumSuccess		= 0x0,
		enumFail		= 0x1,
		enumException	= 0x3
	};

	int Read()    { return Check( FD_READ, 0 ); }
	int Write()   { return Check( FD_WRITE, 1 ); }
	int Connect() { return Check( FD_CONNECT, 2 ); }
	int Close()   { return Check( FD_CLOSE, 3 ); }

private:

	int Check( long lBit, int nIdx )
	{
		if ( m_lEvents & lBit )
		{
			if ( m_nErr[nIdx] != 0 )
			{
				m_nErrorCode |= enumException;
				return enumException;
			}
			return enumSuccess;
		}
		return enumFail;
	}

	long	m_lMask;
	long	m_lEvents;
	int		m_nErrorCode;
	bool	m_bAssociated;
	bool	m_bPendingConnect;
	int		m_nErr[4];		// 0 read, 1 write, 2 connect, 3 close (nhu iErrorCode[FD_*_BIT])

	CEventSelect( const CEventSelect &rhs );
	CEventSelect &operator=( const CEventSelect &rhs );
};

inline bool CEventSelect::WaitForEnumEvent( SOCKET s, DWORD dwTimeout )
{
	m_nErrorCode = 0; m_lEvents = 0; memset( m_nErr, 0, sizeof( m_nErr ) );

	if ( m_bPendingConnect && m_bAssociated )
	{	// [SDL 08/09 2b-2b] nhu Win32: FD_CONNECT dau tien -> CSocketClient::Run goi OnStartConnections (callback enumServerConnectCreate)
		m_bPendingConnect = false;
		m_lEvents |= FD_CONNECT;
		dwTimeout = 0;
	}

	if ( !m_bAssociated || INVALID_SOCKET == s )
	{
		SDL_Delay( dwTimeout > 50 ? 50 : ( Uint32 )dwTimeout );	// khong quay tron CPU khi chua ket noi
		return false;
	}

	fd_set rd, wr, ex;
	FD_ZERO( &rd ); FD_ZERO( &wr ); FD_ZERO( &ex );
	FD_SET( s, &rd ); FD_SET( s, &ex );
	const bool bWantWrite = ( m_lMask & FD_WRITE ) != 0;
	if ( bWantWrite ) FD_SET( s, &wr );

	struct timeval tv;
	tv.tv_sec = ( long )( dwTimeout / 1000 ); tv.tv_usec = ( long )( ( dwTimeout % 1000 ) * 1000 );

	int n = ::select( JxSelectNfds( s ), &rd, bWantWrite ? &wr : NULL, &ex, &tv );
	if ( 0 != n ) JX_NET_TRACE( "[sel] s=%d cho<=%u ms -> n=%d rd=%d ex=%d", ( int )s, ( unsigned )dwTimeout, n, ( int )( n > 0 && FD_ISSET( s, &rd ) ), ( int )( n > 0 && FD_ISSET( s, &ex ) ) );	// [SDL 08/09 2b-2c]
	if ( 0 == n ) return 0 != m_lEvents;
	if ( n < 0 )
	{
		int e = JxNetLastError();
		if ( JX_NET_EINTR == e ) return false;
		m_lEvents |= FD_CLOSE; m_nErr[3] = e; return true;
	}
	if ( FD_ISSET( s, &ex ) )
	{
		int e = JxNetSockError( s );
		m_lEvents |= FD_CLOSE; m_nErr[3] = ( 0 != e ) ? e : -1; return true;
	}
	if ( FD_ISSET( s, &rd ) )
	{
		char c;
		int r = ::recv( s, &c, 1, MSG_PEEK );
		JX_NET_TRACE( "[peek] r=%d err=%d", r, r < 0 ? JxNetLastError() : 0 );	// [SDL 08/09 2b-2c]
		if ( r > 0 )       m_lEvents |= FD_READ;
		else if ( 0 == r ) m_lEvents |= FD_CLOSE;				// FIN: dong binh thuong, khong loi
		else
		{
			int e = JxNetLastError();
			if ( e != JX_NET_WOULDBLOCK && e != JX_NET_EINTR ) { m_lEvents |= FD_CLOSE; m_nErr[3] = e; }
		}
	}
	if ( bWantWrite && FD_ISSET( s, &wr ) ) m_lEvents |= FD_WRITE;

	return 0 != m_lEvents;
}
#else	// JX_PLATFORM_SDL
class CEventSelect
{
public:
	
	CEventSelect();
	~CEventSelect();

	void AssociateEvent( SOCKET s, long lNetworkEvents );
	void DissociateEvent();

	bool WaitForEnumEvent( SOCKET s, DWORD dwTimeout );

	bool IsError() { return ( bool )( ( m_nErrorCode >> 1 ) & 0x1 ); };

	/*
	 * return value
	 *
	 * 0 : success
	 * 1 : fail
	 * 3 : exception
	 */
	enum enumExitValue
	{
		enumSuccess		= 0x0,
		enumFail		= 0x1,
		enumException	= 0x3
	};

	int Read();
	int Write();
	int Connect();
	int Close();

private:
	
	WSAEVENT	m_event;

	WSANETWORKEVENTS m_networkEvents;

	int			m_nErrorCode;

	/*
	 * No copies do not implement
	 */
	CEventSelect( const CEventSelect &rhs );
	CEventSelect &operator=( const CEventSelect &rhs );

};

inline bool CEventSelect::WaitForEnumEvent( SOCKET s, DWORD dwTimeout )
{
	/*
	 * Clear the older network events and wait to update
	 */
	m_nErrorCode = 0;

	memset( &m_networkEvents, 0, sizeof( WSANETWORKEVENTS ) );

	DWORD dwRet = ::WSAWaitForMultipleEvents(
						1,
						&m_event,
						FALSE,
						dwTimeout,
						FALSE );
	
	if ( WSA_WAIT_TIMEOUT == dwRet || WSA_WAIT_FAILED == dwRet )
	{
		return false;
	}

	::WSAEnumNetworkEvents(
					s,
					m_event,
					&m_networkEvents );

	return true;
}

inline int CEventSelect::Read()
{
	if ( m_networkEvents.lNetworkEvents & FD_READ )
	{
		if ( m_networkEvents.iErrorCode[FD_READ_BIT] != 0 )
		{
			/*
			 * throw CWin32Exception( _T("CEventSelect : FD_READ failed with error "), 
			 *	m_networkEvents.iErrorCode[FD_READ_BIT] );
			 */
			m_nErrorCode |= enumException;

			return enumException;
		}
		
		return enumSuccess;
	}

	return enumFail;
}

inline int CEventSelect::Write()
{
	if ( m_networkEvents.lNetworkEvents & FD_WRITE )
	{
		if ( m_networkEvents.iErrorCode[FD_WRITE_BIT] != 0 )
		{
			/*
			 * throw CWin32Exception( _T("CEventSelect : FD_WRITE failed with error "), 
			 *	m_networkEvents.iErrorCode[FD_WRITE_BIT] );
			 */

			m_nErrorCode |= enumException;

			return enumException;
		}
		
		return enumSuccess;
	}

	return enumFail;
}

inline int CEventSelect::Connect()
{
	if ( m_networkEvents.lNetworkEvents & FD_CONNECT )
	{
		if ( m_networkEvents.iErrorCode[FD_CONNECT_BIT] != 0 )
		{
			/*
			 * throw CWin32Exception( _T("CEventSelect : FD_CONNECT failed with error "), 
			 *	m_networkEvents.iErrorCode[FD_CONNECT_BIT] );
			 */
			m_nErrorCode |= enumException;

			return enumException;
		}
		
		return enumSuccess;
	}

	return enumFail;
}

inline int CEventSelect::Close()
{
	if ( m_networkEvents.lNetworkEvents & FD_CLOSE )
	{
		if ( m_networkEvents.iErrorCode[FD_CLOSE_BIT] != 0 )
		{
			/*
			 * throw CWin32Exception( _T("CEventSelect : FD_CLOSE failed with error "), 
			 *	m_networkEvents.iErrorCode[FD_CLOSE_BIT] );
			 */
			m_nErrorCode |= enumException;

			return enumException;
		}
		
		return enumSuccess;
	}

	return enumFail;
}
	
#endif	// JX_PLATFORM_SDL [SDL 08/09 2b-2]

} // End of namespace OnlineGameLib
} // End of namespace Win32

#endif //__INCLUDE_EVENTSELECT_H__