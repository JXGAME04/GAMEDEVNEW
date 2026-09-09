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

	/* [SDL 08/09 2b-2d] PHAI la socket OVERLAPPED (nhu socket() mac dinh): handle khong overlapped bi Windows xep hang MOI I/O
	   (send/setsockopt/shutdown/closesocket tu luong chinh phai doi select() cua luong I/O het han -> tre toi 1 s moi goi gui).
	   Ban Win32 khong dinh vi WSAEventSelect cho tren EVENT, khong giu I/O tren socket. */
	inline SOCKET JxNetCreateTcpSocket() { return ::WSASocket( AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED ); }

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
	/* [ANDROID 08/09] WSABUF: dung dinh nghia cua KPosixCompat.h */
	/* [ANDROID 08/09] OVERLAPPED: dung dinh nghia cua KPosixCompat.h */
	/* [ANDROID 08/09] WSADATA: dung dinh nghia cua KPosixCompat.h */
	/* [ANDROID 08/09] closesocket / WSAGetLastError / Interlocked*: KPosixCompat.h + KPosixWin32.h da co */


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


#endif	/* _WIN32 */

/* [SDL 08/09 2b-2c] dau vet mang (bat khi co tep "jx_net_trace.on" trong thu muc lam viec hoac JX_NET_TRACE=1): ghi jx_net_sdl.log */
int  JxNetTraceOn();
void JxNetTrace( const char *fmt, ... );
#define JX_NET_TRACE( ... ) do { if ( JxNetTraceOn() ) JxNetTrace( __VA_ARGS__ ); } while ( 0 )

/* nfds cho select(): Windows bo qua, POSIX can s + 1 */
inline int JxSelectNfds( SOCKET s ) { return ( int )s + 1; }

#endif	/* JX_PLATFORM_SDL */

#endif	/* __INCLUDE_JX_NET_SHIM_H__ */
