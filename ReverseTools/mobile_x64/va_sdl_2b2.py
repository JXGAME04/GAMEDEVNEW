# -*- coding: utf-8 -*-
"""[SDL 08/09 2b-2] Mang (Rainbow.dll <- MultiServer\Common) qua BSD socket + SDL3 khi JX_PLATFORM_SDL.
Byte-safe (latin-1), moi vet va co the chay lai (kiem TAG). Win32/x64 thuong: khong doi (moi thay doi trong #ifdef JX_PLATFORM_SDL).
 - EventSelect.h : lop CEventSelect ban SDL (select + recv MSG_PEEK) ; EventSelect.cpp chan #ifndef
 - Thread.h/.cpp : SDL_CreateThread/SDL_WaitThread, co m_nDone + friend JxThreadProcSdl
 - Event.cpp     : su kien manual/auto-reset bang SDL_Mutex + SDL_Condition (+ generation cho Pulse)
 - CriticalSection.h : SDL_Mutex (de quy) trong void* m_crit
 - SocketClient.cpp  : Run() tham dinh ky 2 su kien, OnRead recv(), Write send() + select, WaitAndVerifyCipher nfds, tao socket
 - Rainbow/ClientStage.cpp : Sleep(1) -> SDL_Delay(1) ; Rainbow.cpp : DllMain chi _WIN32
Cach dung: python va_sdl_2b2.py [worktree_root]"""
import io, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
C = ROOT + "\\Sources\\MultiServer\\Common\\"
R = ROOT + "\\Sources\\MultiServer\\Rainbow\\"
TAG = "[SDL 08/09 2b-2]"

def rd(p):
    b = io.open(p, "rb").read()
    return b.decode("latin-1")
def wr(p, t, old):
    assert sum(1 for c in old if ord(c) > 127) == sum(1 for c in t if ord(c) > 127), ("so byte cao doi!", p)
    io.open(p, "wb").write(t.encode("latin-1"))
def nl_of(t): return "\r\n" if "\r\n" in t else "\n"
def N(block, nl):
    """chuan hoa xuong dong cua khoi chen theo tep"""
    return block.replace("\r\n", "\n").replace("\n", nl)
def sub1(t, pat, repl_fn, name, flags=re.S):
    m = re.search(pat, t, flags); assert m, "khong thay neo: " + name
    return t[:m.start()] + repl_fn(m) + t[m.end():]

# ---------------------------------------------------------------- EventSelect.h
p = C + "EventSelect.h"; t = rd(p); nl = nl_of(t)
if TAG not in t:
    o = t
    t = t.replace('#include "Win32Exception.h"' + nl, '#include "Win32Exception.h"' + nl + '#ifdef JX_PLATFORM_SDL' + nl + '#include "JxNetShim.h"' + nl + '#endif' + nl, 1)
    SDL_CLASS = r'''#ifdef JX_PLATFORM_SDL
/*
 * [SDL 08/09 2b-2] CEventSelect ban SDL: khong co WSAEventSelect; dung select() tren socket
 * non-blocking, recv(MSG_PEEK) de phan biet FD_READ (co du lieu) / FD_CLOSE (ben kia dong).
 * Giu nguyen API (AssociateEvent/DissociateEvent/WaitForEnumEvent/Read/Write/Connect/Close/IsError)
 * va ma tra ve enumSuccess/enumFail/enumException nhu ban Win32.
 */
class CEventSelect
{
public:

	CEventSelect() : m_lMask( 0 ), m_lEvents( 0 ), m_nErrorCode( 0 ), m_bAssociated( false ) { memset( m_nErr, 0, sizeof( m_nErr ) ); }
	~CEventSelect() {}

	void AssociateEvent( SOCKET s, long lNetworkEvents )
	{
		m_lMask = lNetworkEvents; m_lEvents = 0; m_nErrorCode = 0; m_bAssociated = true;

		if ( !JxNetSetNonBlocking( s, true ) )
		{
			throw CWin32Exception( _T("CEventSelect::AssociateEvent() - non-blocking"), ( DWORD )JxNetLastError() );
		}
	}

	void DissociateEvent() { m_bAssociated = false; m_lEvents = 0; }

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
	int		m_nErr[4];		// 0 read, 1 write, 2 connect, 3 close (nhu iErrorCode[FD_*_BIT])

	CEventSelect( const CEventSelect &rhs );
	CEventSelect &operator=( const CEventSelect &rhs );
};

inline bool CEventSelect::WaitForEnumEvent( SOCKET s, DWORD dwTimeout )
{
	m_nErrorCode = 0; m_lEvents = 0; memset( m_nErr, 0, sizeof( m_nErr ) );

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
	if ( 0 == n ) return false;
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
'''
    t = sub1(t, r"class CEventSelect\r?\n\{\r?\npublic:", lambda m: N(SDL_CLASS, nl) + m.group(0), "class CEventSelect")
    t = sub1(t, r"\} // End of namespace OnlineGameLib", lambda m: "#endif	// JX_PLATFORM_SDL " + TAG + nl + nl + m.group(0), "End of namespace")
    wr(p, t, o); print("EventSelect.h: OK")
else: print("EventSelect.h: da va")

# ---------------------------------------------------------------- EventSelect.cpp
p = C + "EventSelect.cpp"; t = rd(p); nl = nl_of(t)
if TAG not in t:
    o = t
    t = t.replace('#include "EventSelect.h"' + nl, '#include "EventSelect.h"' + nl + '#ifndef JX_PLATFORM_SDL	// ' + TAG + ' ban SDL: lop inline trong EventSelect.h' + nl, 1)
    if not t.endswith(nl): t += nl
    t += "#endif	// JX_PLATFORM_SDL " + TAG + nl
    wr(p, t, o); print("EventSelect.cpp: OK")
else: print("EventSelect.cpp: da va")

# ---------------------------------------------------------------- Thread.h
p = C + "Thread.h"; t = rd(p); nl = nl_of(t)
if TAG not in t:
    o = t
    t = sub1(t, r"([ \t]*)HANDLE m_hThread;\r?\n", lambda m: N("#ifdef JX_PLATFORM_SDL\n" + m.group(1) + "mutable HANDLE m_hThread;	// " + TAG + " SDL_Thread*; mutable de Wait() const thu hoi luong\n" + m.group(1) + "volatile int m_nDone;\n" + m.group(1) + "friend int JxThreadProcSdl( void *pV );\n#else\n" + m.group(1) + "HANDLE m_hThread;\n#endif\n", nl), "HANDLE m_hThread")
    wr(p, t, o); print("Thread.h: OK")
else: print("Thread.h: da va")

# ---------------------------------------------------------------- Thread.cpp
p = C + "Thread.cpp"; t = rd(p); nl = nl_of(t)
if TAG not in t:
    o = t
    SDL_THREAD = r'''#ifdef JX_PLATFORM_SDL
#include "JxNetShim.h"
/*
 * [SDL 08/09 2b-2] CThread qua SDL_CreateThread / SDL_WaitThread. Wait(timeout) tham co m_nDone
 * (SDL khong co join co han). Terminate: khong ho tro (chi TRACE).
 */
namespace OnlineGameLib {
namespace Win32 {

int JxThreadProcSdl( void *pV )
{
	int result = 0;
	CThread *pThis = ( CThread * )pV;

	if ( pThis )
	{
		try
		{
			result = pThis->Run();
		}
		catch( ... )
		{
			TRACE( "CThread::ThreadFunction exception!" );
		}
		pThis->m_nDone = 1;
	}

	return result;
}

CThread::CThread()
	: m_hThread( NULL )
{
	m_nDone = 0;
}

CThread::~CThread()
{
	if ( m_hThread )
	{
		if ( m_nDone ) SDL_WaitThread( ( SDL_Thread * )m_hThread, NULL );
		else           SDL_DetachThread( ( SDL_Thread * )m_hThread );
		m_hThread = NULL;
	}
}

HANDLE CThread::GetHandle() const
{
	return m_hThread;
}

void CThread::Start()
{
	if ( m_hThread == NULL )
	{
		m_nDone = 0;
		m_hThread = ( HANDLE )SDL_CreateThread( JxThreadProcSdl, "JxNetIO", ( void * )this );

		if ( m_hThread == NULL )
		{
			throw CWin32Exception( _T("CThread::Start() - SDL_CreateThread"), 0 );
		}
	}
	else
	{
		throw CException( _T("CThread::Start()"), _T("Thread already running - you can only call Start() once!") );
	}
}

void CThread::Wait() const
{
	if ( !Wait( INFINITE ) )
	{
		throw CException( _T("CThread::Wait()"), _T("Unexpected timeout on infinite wait") );
	}
}

bool CThread::Wait( DWORD timeoutMillis ) const
{
	if ( !m_hThread )
	{
		return true;
	}

	if ( INFINITE != timeoutMillis )
	{
		Uint64 t0 = SDL_GetTicks();
		while ( !m_nDone )
		{
			if ( SDL_GetTicks() - t0 >= ( Uint64 )timeoutMillis ) return false;
			SDL_Delay( 1 );
		}
	}

	SDL_WaitThread( ( SDL_Thread * )m_hThread, NULL );
	m_hThread = NULL;
	return true;
}

unsigned int __stdcall CThread::ThreadFunction( void *pV )
{
	return ( unsigned int )JxThreadProcSdl( pV );
}

void CThread::Terminate( DWORD /* exitCode */ )
{
	TRACE( "CThread::Terminate: khong ho tro tren SDL" );
}

} // End of namespace Win32
} // End of namespace OnlineGameLib
#else	// JX_PLATFORM_SDL
'''
    t = t.replace('#include "Win32Exception.h"' + nl, '#include "Win32Exception.h"' + nl + nl + N(SDL_THREAD, nl), 1)
    assert "JxThreadProcSdl" in t
    if not t.endswith(nl): t += nl
    t += "#endif	// JX_PLATFORM_SDL " + TAG + nl
    wr(p, t, o); print("Thread.cpp: OK")
else: print("Thread.cpp: da va")

# ---------------------------------------------------------------- Event.cpp
p = C + "Event.cpp"; t = rd(p); nl = nl_of(t)
if TAG not in t:
    o = t
    SDL_EVENT = r'''#ifdef JX_PLATFORM_SDL
#include "JxNetShim.h"
/*
 * [SDL 08/09 2b-2] CEvent (manual/auto-reset, cho co han) bang SDL_Mutex + SDL_Condition.
 * m_hEvent giu con tro JxSdlEvent. Pulse dung bo dem the he de danh thuc moi luong dang cho.
 */
namespace OnlineGameLib {
namespace Win32 {

struct JxSdlEvent
{
	SDL_Mutex		*mu;
	SDL_Condition	*cv;
	bool			manual;
	bool			state;
	unsigned		gen;
};

static HANDLE JxCreateEvent( bool bManualReset, bool bInitialState )
{
	JxSdlEvent *e = new JxSdlEvent;
	e->mu = SDL_CreateMutex(); e->cv = SDL_CreateCondition();
	e->manual = bManualReset; e->state = bInitialState; e->gen = 0;

	if ( !e->mu || !e->cv )
	{
		throw CWin32Exception( _T("CEvent::Create() - SDL"), 0 );
	}

	return ( HANDLE )e;
}

CEvent::CEvent(
		   LPSECURITY_ATTRIBUTES /* lpEventAttributes */,
		   bool bManualReset,
		   bool bInitialState)
		   : m_hEvent( JxCreateEvent( bManualReset, bInitialState ) )
{
}

CEvent::CEvent(
		   LPSECURITY_ATTRIBUTES /* lpEventAttributes */,
		   bool bManualReset,
		   bool bInitialState,
		   const char * /* pEventName */ )
		   : m_hEvent( JxCreateEvent( bManualReset, bInitialState ) )
{
}

CEvent::~CEvent()
{
	JxSdlEvent *e = ( JxSdlEvent * )m_hEvent;
	if ( e )
	{
		SDL_DestroyCondition( e->cv );
		SDL_DestroyMutex( e->mu );
		delete e;
	}
}

void CEvent::Wait() const
{
	if ( !Wait( INFINITE ) )
	{
		throw CException( _T("CEvent::Wait()"), _T("Unexpected timeout on infinite wait") );
	}
}

bool CEvent::Wait( DWORD timeoutMillis ) const
{
	JxSdlEvent *e = ( JxSdlEvent * )m_hEvent;
	bool ok = false;

	SDL_LockMutex( e->mu );

	const unsigned gen0 = e->gen;

	if ( !e->state && 0 != timeoutMillis )
	{
		if ( INFINITE == timeoutMillis )
		{
			while ( !e->state && e->gen == gen0 ) SDL_WaitCondition( e->cv, e->mu );
		}
		else
		{
			Uint64 t0 = SDL_GetTicks();
			while ( !e->state && e->gen == gen0 )
			{
				Uint64 dt = SDL_GetTicks() - t0;
				if ( dt >= ( Uint64 )timeoutMillis ) break;
				SDL_WaitConditionTimeout( e->cv, e->mu, ( Sint32 )( timeoutMillis - dt ) );
			}
		}
	}

	ok = e->state || e->gen != gen0;
	if ( e->state && !e->manual ) e->state = false;

	SDL_UnlockMutex( e->mu );

	return ok;
}

void CEvent::Reset()
{
	JxSdlEvent *e = ( JxSdlEvent * )m_hEvent;
	SDL_LockMutex( e->mu );
	e->state = false;
	SDL_UnlockMutex( e->mu );
}

void CEvent::Set()
{
	JxSdlEvent *e = ( JxSdlEvent * )m_hEvent;
	SDL_LockMutex( e->mu );
	e->state = true;
	SDL_BroadcastCondition( e->cv );
	SDL_UnlockMutex( e->mu );
}

void CEvent::Pulse()
{
	JxSdlEvent *e = ( JxSdlEvent * )m_hEvent;
	SDL_LockMutex( e->mu );
	e->gen++;
	SDL_BroadcastCondition( e->cv );
	e->state = false;
	SDL_UnlockMutex( e->mu );
}

} // End of namespace Win32
} // End of namespace OnlineGameLib
#else	// JX_PLATFORM_SDL
'''
    t = t.replace('#include "Win32Exception.h"' + nl, '#include "Win32Exception.h"' + nl + nl + N(SDL_EVENT, nl), 1)
    assert "JxCreateEvent" in t
    if not t.endswith(nl): t += nl
    t += "#endif	// JX_PLATFORM_SDL " + TAG + nl
    wr(p, t, o); print("Event.cpp: OK")
else: print("Event.cpp: da va")

# ---------------------------------------------------------------- CriticalSection.h
p = C + "CriticalSection.h"; t = rd(p); nl = nl_of(t)
if TAG not in t:
    o = t
    t = t.replace('#include "Utils.h"' + nl, '#include "Utils.h"' + nl + '#ifdef JX_PLATFORM_SDL' + nl + '#include "JxNetShim.h"' + nl + '#endif' + nl, 1)
    t = sub1(t, r"([ \t]*)CRITICAL_SECTION[ \t]+m_crit;\r?\n", lambda m: N("#ifdef JX_PLATFORM_SDL\n" + m.group(1) + "void *m_crit;	// " + TAG + " SDL_Mutex* (de quy nhu CRITICAL_SECTION)\n#else\n" + m.group(0).replace("\r\n", "\n") + "#endif\n", nl), "m_crit")
    SDL_CS = r'''#ifdef JX_PLATFORM_SDL
inline void CCriticalSection::Enter() { SDL_LockMutex( ( SDL_Mutex * )m_crit ); }
inline void CCriticalSection::Leave() { SDL_UnlockMutex( ( SDL_Mutex * )m_crit ); }
inline CCriticalSection::CCriticalSection() { m_crit = SDL_CreateMutex(); }
inline CCriticalSection::~CCriticalSection() { if ( m_crit ) SDL_DestroyMutex( ( SDL_Mutex * )m_crit ); m_crit = NULL; }
#if ( _WIN32_WINNT >= 0x0400 )
inline bool CCriticalSection::TryEnter() { return SDL_TryLockMutex( ( SDL_Mutex * )m_crit ); }
#endif
#else	// JX_PLATFORM_SDL
'''
    t = sub1(t, r"inline void CCriticalSection::Enter\(\)", lambda m: N(SDL_CS, nl) + m.group(0), "inline Enter")
    t = sub1(t, r"(return BOOL_to_bool\( ::TryEnterCriticalSection\( &m_crit \) \);\r?\n\}\r?\n#endif\r?\n)", lambda m: m.group(1) + "#endif	// JX_PLATFORM_SDL " + TAG + nl, "TryEnter #endif")
    wr(p, t, o); print("CriticalSection.h: OK")
else: print("CriticalSection.h: da va")

# ---------------------------------------------------------------- SocketClient.cpp
p = C + "SocketClient.cpp"; t = rd(p); nl = nl_of(t)
if TAG not in t:
    o = t
    t = t.replace('#include "Macro.h"' + nl, '#include "Macro.h"' + nl + '#ifdef JX_PLATFORM_SDL' + nl + '#include "JxNetShim.h"	// ' + TAG + nl + '#endif' + nl, 1)
    # (c) tao socket
    t = sub1(t, r"([ \t]*)SOCKET s = ::WSASocket\( AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, 0 \);\r?\n",
             lambda m: N("#ifdef JX_PLATFORM_SDL\n" + m.group(1) + "SOCKET s = JxNetCreateTcpSocket();	// " + TAG + "\n#else\n" + m.group(0).replace("\r\n", "\n") + "#endif\n", nl), "WSASocket")
    # (d) Run(): thay WaitForMultipleObjects
    RUN_SDL = '''#ifdef JX_PLATFORM_SDL
		// {TAG} khong co WaitForMultipleObjects: tham dinh ky 2 su kien (shutdown / da ket noi), 50 ms moi nhip
		while ( !m_shutdownEvent.Wait( 0 ) )
		{
			if ( !m_successConnectionsEvent.Wait( 50 ) )
			{
				continue;
			}

			{
#else
'''.replace("{TAG}", TAG)
    t = sub1(t, r"[ \t]*HANDLE handlesToWaitFor\[2\];.*?else if \( waitResult == WAIT_OBJECT_0 \+ 1 \)\r?\n[ \t]*\{\r?\n",
             lambda m: N(RUN_SDL, nl) + m.group(0) + "#endif" + nl, "Run WaitForMultipleObjects")
    t = sub1(t, r"([ \t]*else\r?\n[ \t]*\{\r?\n(?:[^\n]*\n){3}[ \t]*OnError\( _T\(\"CSocketClient::Run\(\) - WaitForMultipleObjects: \"\)[^\n]*\n[ \t]*\}\r?\n)",
             lambda m: "#ifndef JX_PLATFORM_SDL" + nl + m.group(1) + "#endif" + nl, "Run else OnError")
    # (e) OnRead
    READ_SDL = '''#ifdef JX_PLATFORM_SDL
	// {TAG} recv() tren socket non-blocking (thay WSARecv); 0 = ben kia dong; WOULDBLOCK = chua co du lieu
	{
		WSABUF *pWsa = pBuffer->GetWSABUF();
		int nRecv = ::recv( m_connectSocket, pWsa->buf, ( int )pWsa->len, 0 );

		if ( nRecv > 0 )
		{
			pBuffer->Use( ( size_t )nRecv );

			ReadCompleted( pBuffer );
		}
		else if ( 0 == nRecv )
		{
			StopConnections();
		}
		else
		{
			int lastError = JxNetLastError();

			if ( lastError != JX_NET_WOULDBLOCK && lastError != JX_NET_EINTR )
			{
				Output( _T("CSocketClient::OnRead() - recv: ") + GetLastErrorMessage( ( DWORD )lastError ) );

				if ( lastError == JX_NET_CONNABORTED ||
					lastError == JX_NET_CONNRESET ||
					lastError == JX_NET_DISCON ||
					lastError == JX_NET_NOTCONN )
				{
					StopConnections();
				}
			}
		}
	}
#else
'''.replace("{TAG}", TAG)
    t = sub1(t, r"[ \t]*if \( SOCKET_ERROR == ::WSARecv\(.*?ReadCompleted\( pBuffer \);\r?\n[ \t]*\}\r?\n",
             lambda m: N(READ_SDL, nl) + m.group(0) + "#endif" + nl, "OnRead WSARecv")
    # (f) Write: WSASend -> send
    SEND_SDL = '''#ifdef JX_PLATFORM_SDL
			{	// {TAG} send() thay WSASend; loi -> dwSendNumBytes = 0 de vong lap khong tru lai so byte cu
				int nSent = ::send( m_connectSocket, wsa.buf, ( int )wsa.len, JX_SEND_FLAGS );
				if ( nSent < 0 ) { nError = SOCKET_ERROR; dwSendNumBytes = 0; }
				else             { nError = 0; dwSendNumBytes = ( DWORD )nSent; }
			}
#else
'''.replace("{TAG}", TAG)
    t = sub1(t, r"[ \t]*nError = ::WSASend\(.*?\);\r?\n", lambda m: N(SEND_SDL, nl) + m.group(0) + "#endif" + nl, "WSASend")
    # fd_set kieu Windows -> FD_SET
    t = sub1(t, r"([ \t]*)writefds\.fd_count = 1;\r?\n[ \t]*writefds\.fd_array\[0\] = m_connectSocket;\r?\n",
             lambda m: N("#ifdef JX_PLATFORM_SDL\n" + m.group(1) + "FD_ZERO( &writefds ); FD_SET( m_connectSocket, &writefds );	// " + TAG + "\n#else\n" + m.group(0).replace("\r\n", "\n") + "#endif\n", nl), "fd_count")
    t = sub1(t, r"([ \t]*)nError = select\( 1, NULL, &writefds, NULL, &gs_CheckRW_timeout \);\r?\n",
             lambda m: N("#ifdef JX_PLATFORM_SDL\n" + m.group(1) + "{	// " + TAG + " select sua fd_set/timeout -> nap lai moi vong; nfds = s + 1 cho POSIX\n" + m.group(1) + "\tstruct timeval tvCho = gs_CheckRW_timeout;\n" + m.group(1) + "\tFD_ZERO( &writefds ); FD_SET( m_connectSocket, &writefds );\n" + m.group(1) + "\tnError = ::select( JxSelectNfds( m_connectSocket ), NULL, &writefds, NULL, &tvCho );\n" + m.group(1) + "}\n#else\n" + m.group(0).replace("\r\n", "\n") + "#endif\n", nl), "select writefds")
    # timeout 5 s khi gui -> dong ket noi (nhu hieu ung thuc te cua ban Win32: fd_set rong -> loi -> StopConnections)
    t = sub1(t, r"([ \t]*)if \( nError > 0 \)\r?\n[ \t]*\{\r?\n[ \t]*break;\r?\n[ \t]*\}\r?\n",
             lambda m: m.group(0) + N("#ifdef JX_PLATFORM_SDL\n" + m.group(1) + "if ( 0 == nError )\n" + m.group(1) + "{	// " + TAG + " qua 5 s khong gui duoc: dong ket noi (ban Win32 cung ket thuc o day vi fd_set da bi select xoa)\n" + m.group(1) + "\tOutput( _T(\"CSocketClient::Write() - send cho qua han -> dong ket noi\") );\n" + m.group(1) + "\tStopConnections();\n" + m.group(1) + "\treturn;\n" + m.group(1) + "}\n#endif\n", nl), "nError > 0 break")
    # (g) WaitAndVerifyCipher nfds
    t = sub1(t, r"([ \t]*)int res = select\( 0, &fdRead, NULL, NULL, pstTime \);\r?\n",
             lambda m: N("#ifdef JX_PLATFORM_SDL\n" + m.group(1) + "int res = ::select( JxSelectNfds( s ), &fdRead, NULL, NULL, pstTime );	// " + TAG + "\n#else\n" + m.group(0).replace("\r\n", "\n") + "#endif\n", nl), "select cipher")
    wr(p, t, o); print("SocketClient.cpp: OK")
else: print("SocketClient.cpp: da va")

# ---------------------------------------------------------------- Rainbow/ClientStage.cpp
p = R + "ClientStage.cpp"; t = rd(p); nl = nl_of(t)
if TAG not in t:
    o = t
    t = t.replace('#include "Exception.h"' + nl, '#include "Exception.h"' + nl + '#ifdef JX_PLATFORM_SDL' + nl + '#include "JxNetShim.h"	// ' + TAG + nl + '#endif' + nl, 1)
    t = sub1(t, r"([ \t]*)::Sleep\( 1 \);\r?\n", lambda m: N("#ifdef JX_PLATFORM_SDL\n" + m.group(1) + "SDL_Delay( 1 );\n#else\n" + m.group(0).replace("\r\n", "\n") + "#endif\n", nl), "Sleep(1)")
    wr(p, t, o); print("ClientStage.cpp: OK")
else: print("ClientStage.cpp: da va")

# ---------------------------------------------------------------- Rainbow/Rainbow.cpp
p = R + "Rainbow.cpp"; t = rd(p); nl = nl_of(t)
if TAG not in t:
    o = t
    t = sub1(t, r"#define WIN32_LEAN_AND_MEAN", lambda m: "#ifdef _WIN32	// " + TAG + " DllMain chi tren Windows" + nl + m.group(0), "WIN32_LEAN_AND_MEAN")
    if not t.endswith(nl): t += nl
    t += "#endif	// _WIN32 " + TAG + nl
    wr(p, t, o); print("Rainbow.cpp: OK")
else: print("Rainbow.cpp: da va")
print("XONG 2b-2")
