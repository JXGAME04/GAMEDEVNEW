/********************************************************************
	[SDL 08/09 2b-2] Lop dem mang cho ban JX_PLATFORM_SDL (Rainbow.dll / common.lib).

	Muc dich: ma OnlineGameLib::Win32 (SocketClient, EventSelect, Thread, Event, CriticalSection)
	di qua BSD socket (select / recv / send) + SDL3 (luong, khoa, dieu kien) thay cho
	WSAEventSelect / WSARecv / WSASend / _beginthreadex / CreateEvent / CRITICAL_SECTION.

	- Tren Windows (bo thu bin\client64sdl): Winsock 2 tuong thich BSD, chi can vai ham inline.
	- Tren POSIX (Android/Linux, pha sau): khoi #else dinh nghia lai cac kieu/hang Winsock ma
	  Common dang dung. Khoi POSIX CHUA duoc bien dich (may thu nghiem la Windows); cac kieu
	  Win32 co ban (DWORD/BYTE/HANDLE/_tstring...) se do lop dem chung cua pha Android cung cap.

	Chi co hieu luc khi JX_PLATFORM_SDL; ban Win32/x64 thuong khong include tep nay.
*********************************************************************/
#ifndef __INCLUDE_JX_NET_SHIM_H__
#define __INCLUDE_JX_NET_SHIM_H__

#ifdef JX_PLATFORM_SDL

#include <SDL3/SDL.h>

#ifdef _WIN32

	#include <winsock2.h>

	#define JX_NET_WOULDBLOCK	WSAEWOULDBLOCK
	#define JX_NET_EINTR		WSAEINTR
	#define JX_NET_CONNRESET	WSAECONNRESET
	#define JX_NET_CONNABORTED	WSAECONNABORTED
	#define JX_NET_DISCON		WSAEDISCON
	#define JX_NET_NOTCONN		WSAENOTCONN
	#define JX_SEND_FLAGS		0

	inline int JxNetLastError() { return ::WSAGetLastError(); }

	inline bool JxNetSetNonBlocking( SOCKET s, bool bOn )
	{
		u_long v = bOn ? 1 : 0;
		return 0 == ::ioctlsocket( s, FIONBIO, &v );
	}

	/* giu nguyen cach tao socket cua ban Win32 (WSASocket khong overlapped) */
	inline SOCKET JxNetCreateTcpSocket() { return ::WSASocket( AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, 0 ); }

	inline int JxNetSockError( SOCKET s )
	{
		int err = 0; int len = sizeof( err );
		if ( 0 != ::getsockopt( s, SOL_SOCKET, SO_ERROR, ( char * )&err, &len ) ) return -1;
		return err;
	}

#else	/* POSIX: CHUA BIEN DICH - se hoan thien o pha Android */

	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/select.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <errno.h>

	typedef int SOCKET;
	#define INVALID_SOCKET	(-1)
	#define SOCKET_ERROR	(-1)
	typedef struct sockaddr_in	SOCKADDR_IN;
	typedef struct linger		LINGER;
	typedef struct timeval		TIMEVAL;
	typedef struct { unsigned long len; char *buf; } WSABUF;	/* CIOBuffer::m_wsabuf */
	typedef struct { void *Internal; void *InternalHigh; unsigned Offset; unsigned OffsetHigh; void *hEvent; } OVERLAPPED;	/* CIOBuffer ke thua, chi giu bo cuc */
	typedef struct { int wVersion; } WSADATA;					/* CUsesWinsock::m_data */

	#define closesocket( s )	::close( s )
	#define WSAGetLastError()	errno

	/* gia tri FD_* nhu Winsock, chi dung noi bo CEventSelect */
	#define FD_READ		0x01
	#define FD_WRITE	0x02
	#define FD_OOB		0x04
	#define FD_ACCEPT	0x08
	#define FD_CONNECT	0x10
	#define FD_CLOSE	0x20

	#define JX_NET_WOULDBLOCK	EWOULDBLOCK
	#define JX_NET_EINTR		EINTR
	#define JX_NET_CONNRESET	ECONNRESET
	#define JX_NET_CONNABORTED	ECONNABORTED
	#define JX_NET_DISCON		EPIPE
	#define JX_NET_NOTCONN		ENOTCONN
	#define JX_SEND_FLAGS		MSG_NOSIGNAL

	inline int JxNetLastError() { return errno; }

	inline bool JxNetSetNonBlocking( SOCKET s, bool bOn )
	{
		int fl = ::fcntl( s, F_GETFL, 0 );
		if ( fl < 0 ) return false;
		fl = bOn ? ( fl | O_NONBLOCK ) : ( fl & ~O_NONBLOCK );
		return 0 == ::fcntl( s, F_SETFL, fl );
	}

	inline SOCKET JxNetCreateTcpSocket() { return ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); }

	inline int JxNetSockError( SOCKET s )
	{
		int err = 0; socklen_t len = sizeof( err );
		if ( 0 != ::getsockopt( s, SOL_SOCKET, SO_ERROR, &err, &len ) ) return -1;
		return err;
	}

	inline long InterlockedIncrement( volatile long *p ) { return __sync_add_and_fetch( p, 1 ); }
	inline long InterlockedDecrement( volatile long *p ) { return __sync_sub_and_fetch( p, 1 ); }

#endif	/* _WIN32 */

/* nfds cho select(): Windows bo qua, POSIX can s + 1 */
inline int JxSelectNfds( SOCKET s ) { return ( int )s + 1; }

#endif	/* JX_PLATFORM_SDL */

#endif	/* __INCLUDE_JX_NET_SHIM_H__ */
