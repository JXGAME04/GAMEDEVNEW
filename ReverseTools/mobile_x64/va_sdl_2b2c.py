# -*- coding: utf-8 -*-
"""[SDL 08/09 2b-2c] Luong I/O ban SDL khong thuc khi socket bi dong tu luong khac (select cho het 1 s; Win32 thuc ngay nho WSACloseEvent)
-> Cleanup mat ~1,75 s moi lan ngat -> dang nhap (ngat/noi 2-3 lan) dung ~5 s, doi may chu khi qua map/phu ve khung.
Sua (chi JX_PLATFORM_SDL):
 - StopConnections: ::shutdown(SD_BOTH) truoc closesocket -> select thuc ngay (readable, recv = 0)
 - WaitForEnumEvent(m_connectSocket, 1000) -> 200 ms (luoi an toan)
 - Dau vet mang: JxNetTraceOn()/JxNetTrace() (JxNetShim.h khai bao, SocketClient.cpp dinh nghia); bat khi co tep jx_net_trace.on
   trong thu muc lam viec hoac bien moi truong JX_NET_TRACE=1; ghi jx_net_sdl.log [ms][luong] ...
 - Harness: tach thoi gian Shutdown / Cleanup / Release
Byte-safe, chay lai an toan."""
import io, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
C = ROOT + "\\Sources\\MultiServer\\Common\\"
R = ROOT + "\\Sources\\MultiServer\\Rainbow\\"
TAG = "[SDL 08/09 2b-2c]"
def rd(p): return io.open(p, "rb").read().decode("latin-1")
def wr(p, t, old):
    assert sum(1 for c in old if ord(c) > 127) == sum(1 for c in t if ord(c) > 127), ("so byte cao doi!", p)
    io.open(p, "wb").write(t.encode("latin-1"))
def nl_of(t): return "\r\n" if "\r\n" in t else "\n"
def N(b, nl): return b.replace("\r\n", "\n").replace("\n", nl)
def sub1(t, pat, repl, name, flags=re.S):
    t2, n = re.subn(pat, repl, t, count=1, flags=flags); assert n == 1, "khong thay neo: " + name; return t2

# ---------------------------------------------------------------- JxNetShim.h: khai bao dau vet
p = C + "JxNetShim.h"; t = rd(p); nl = nl_of(t)
if TAG not in t:
    o = t
    DECL = '''/* {TAG} dau vet mang (bat khi co tep "jx_net_trace.on" trong thu muc lam viec hoac JX_NET_TRACE=1): ghi jx_net_sdl.log */
int  JxNetTraceOn();
void JxNetTrace( const char *fmt, ... );
#define JX_NET_TRACE( ... ) do { if ( JxNetTraceOn() ) JxNetTrace( __VA_ARGS__ ); } while ( 0 )

'''.replace("{TAG}", TAG)
    t = sub1(t, r"/\* nfds cho select\(\)", lambda m: N(DECL, nl) + m.group(0), "nfds comment")
    wr(p, t, o); print("JxNetShim.h: + khai bao dau vet")
else: print("JxNetShim.h: da va")

# ---------------------------------------------------------------- EventSelect.h: dau vet +
p = C + "EventSelect.h"; t = rd(p); nl = nl_of(t)
if TAG not in t:
    o = t
    t = sub1(t, r"(\tvoid AssociateEvent\( SOCKET s, long lNetworkEvents \)\r?\n\t\{\r?\n)",
             lambda m: m.group(1) + "\t\tJX_NET_TRACE( \"[evsel] AssociateEvent s=%d mask=%lx\", ( int )s, lNetworkEvents );\t// " + TAG + nl, "AssociateEvent")
    t = sub1(t, r"\tvoid DissociateEvent\(\) \{ m_bAssociated = false; m_lEvents = 0; m_bPendingConnect = false; \}",
             lambda m: "\tvoid DissociateEvent() { JX_NET_TRACE( \"[evsel] DissociateEvent\" ); m_bAssociated = false; m_lEvents = 0; m_bPendingConnect = false; }", "DissociateEvent")
    # sau select: ghi vet khi co su kien / loi
    t = sub1(t, r"(\tint n = ::select\( JxSelectNfds\( s \), &rd, bWantWrite \? &wr : NULL, &ex, &tv \);\r?\n)",
             lambda m: m.group(1) + "\tif ( 0 != n ) JX_NET_TRACE( \"[sel] s=%d cho<=%u ms -> n=%d rd=%d ex=%d\", ( int )s, ( unsigned )dwTimeout, n, ( int )( n > 0 && FD_ISSET( s, &rd ) ), ( int )( n > 0 && FD_ISSET( s, &ex ) ) );\t// " + TAG + nl, "sau select")
    t = sub1(t, r"(\tif \( FD_ISSET\( s, &rd \) \)\r?\n\t\{\r?\n\t\tchar c;\r?\n\t\tint r = ::recv\( s, &c, 1, MSG_PEEK \);\r?\n)",
             lambda m: m.group(1) + "\t\tJX_NET_TRACE( \"[peek] r=%d err=%d\", r, r < 0 ? JxNetLastError() : 0 );\t// " + TAG + nl, "peek")
    wr(p, t, o); print("EventSelect.h: + dau vet")
else: print("EventSelect.h: da va")

# ---------------------------------------------------------------- SocketClient.cpp
p = C + "SocketClient.cpp"; t = rd(p); nl = nl_of(t)
if TAG not in t:
    o = t
    # dinh nghia dau vet ngay sau include JxNetShim.h
    IMPL = '''#ifdef JX_PLATFORM_SDL
#include <stdarg.h>
#include <stdio.h>
/* {TAG} dau vet mang ban SDL: jx_net_sdl.log trong thu muc lam viec; bat bang tep jx_net_trace.on hoac JX_NET_TRACE=1 */
static int s_nJxNetTrace = -1;
int JxNetTraceOn()
{
	if ( s_nJxNetTrace < 0 )
	{
		const char *e = SDL_getenv( "JX_NET_TRACE" );
		FILE *f = fopen( "jx_net_trace.on", "rb" );
		s_nJxNetTrace = ( ( e && *e == '1' ) || f ) ? 1 : 0;
		if ( f ) fclose( f );
	}
	return s_nJxNetTrace;
}
void JxNetTrace( const char *fmt, ... )
{
	static SDL_Mutex *s_mu = SDL_CreateMutex();
	static FILE *s_f = NULL;
	SDL_LockMutex( s_mu );
	if ( !s_f ) s_f = fopen( "jx_net_sdl.log", "a" );
	if ( s_f )
	{
		fprintf( s_f, "[%8llu][%lu] ", ( unsigned long long )SDL_GetTicks(), ( unsigned long )SDL_GetCurrentThreadID() );
		va_list ap; va_start( ap, fmt ); vfprintf( s_f, fmt, ap ); va_end( ap );
		fputc( '\\n', s_f ); fflush( s_f );
	}
	SDL_UnlockMutex( s_mu );
}
#endif
'''.replace("{TAG}", TAG)
    t = sub1(t, r"(#ifdef JX_PLATFORM_SDL\r?\n#include \"JxNetShim.h\"[^\n]*\n#endif\r?\n)", lambda m: m.group(1) + N(IMPL, nl), "include shim")
    # StartConnections: dau vet thoi gian connect + cipher
    t = sub1(t, r"(\t\tm_connectSocket = CreateConnectionSocket\( m_address, m_port \);\r?\n)",
             lambda m: N("#ifdef JX_PLATFORM_SDL\n\t\tUint64 uT0 = SDL_GetTicks();\t// " + TAG + "\n#endif\n", nl) + m.group(1) + N("#ifdef JX_PLATFORM_SDL\n\t\tJX_NET_TRACE( \"[conn] %s:%u connect -> s=%d (%llu ms)\", m_address.c_str(), ( unsigned )m_port, ( int )m_connectSocket, ( unsigned long long )( SDL_GetTicks() - uT0 ) );\n\t\tuT0 = SDL_GetTicks();\n#endif\n", nl), "CreateConnectionSocket")
    t = sub1(t, r"(\t\tif \( !WaitAndVerifyCipher\(\) \)\r?\n\t\t\{\r?\n)",
             lambda m: m.group(1) + N("#ifdef JX_PLATFORM_SDL\n\t\t\tJX_NET_TRACE( \"[conn] cipher THAT BAI (%llu ms)\", ( unsigned long long )( SDL_GetTicks() - uT0 ) );\n#endif\n", nl), "cipher fail")
    t = sub1(t, r"(\t\tm_eventSelect\.AssociateEvent\( m_connectSocket, FD_CONNECT \| FD_CLOSE \| FD_READ \);\r?\n)",
             lambda m: N("#ifdef JX_PLATFORM_SDL\n\t\tJX_NET_TRACE( \"[conn] cipher OK (%llu ms) mode=%u\", ( unsigned long long )( SDL_GetTicks() - uT0 ), m_uKeyMode );\n#endif\n", nl) + m.group(1), "AssociateEvent call")
    # StopConnections: shutdown truoc closesocket (SDL) + dau vet
    t = sub1(t, r"(\t\tm_successConnectionsEvent\.Reset\(\);\r?\n\r?\n\t\tm_eventSelect\.DissociateEvent\(\);\r?\n)",
             lambda m: N("#ifdef JX_PLATFORM_SDL\n\t\tJX_NET_TRACE( \"[conn] StopConnections s=%d\", ( int )m_connectSocket );\n\t\t::shutdown( m_connectSocket, 2 /* SD_BOTH */ );\t// " + TAG + " danh thuc select cua luong I/O (Win32: WSACloseEvent lam viec nay)\n#endif\n", nl) + m.group(1), "StopConnections")
    # Run: timeout select 1000 -> 200 (SDL) + dau vet vao/ra vong trong
    t = sub1(t, r"\t\t\t\tCIOBuffer \*pReadContext = Allocate\(\);\r?\n",
             lambda m: m.group(0) + N("#ifdef JX_PLATFORM_SDL\n\t\t\t\tJX_NET_TRACE( \"[run] vao vong doc s=%d\", ( int )m_connectSocket );\n#endif\n", nl), "pReadContext Allocate")
    t = sub1(t, r"(\t\t\t\t\tif \( m_eventSelect\.WaitForEnumEvent\( m_connectSocket, )1000( \) \)\r?\n)",
             lambda m: N("#ifdef JX_PLATFORM_SDL\n", nl) + m.group(1) + "200" + m.group(2) + N("#else\n", nl) + m.group(1) + "1000" + m.group(2) + N("#endif\n", nl), "WaitForEnumEvent 1000")
    t = sub1(t, r"(\t\t\t\t\t\tif \( m_eventSelect\.IsError\(\) \)\r?\n\t\t\t\t\t\t\{\r?\n)",
             lambda m: m.group(1) + N("#ifdef JX_PLATFORM_SDL\n\t\t\t\t\t\t\tJX_NET_TRACE( \"[run] IsError -> thoat luong\" );\n#endif\n", nl), "IsError")
    t = sub1(t, r"(\t\t\t\tpReadContext->Release\(\);\r?\n)",
             lambda m: N("#ifdef JX_PLATFORM_SDL\n\t\t\t\tJX_NET_TRACE( \"[run] ra vong doc\" );\n#endif\n", nl) + m.group(1), "pReadContext Release")
    # OnRead SDL: dau vet
    t = sub1(t, r"(\t\tint nRecv = ::recv\( m_connectSocket, pWsa->buf, \( int \)pWsa->len, 0 \);\r?\n)",
             lambda m: m.group(1) + "\t\tJX_NET_TRACE( \"[recv] %d B (bo dem %u)\", nRecv, ( unsigned )pWsa->len );" + nl, "recv trace")
    # Write SDL: dau vet send + WOULDBLOCK + timeout
    t = sub1(t, r"(\t\t\t\tint nSent = ::send\( m_connectSocket, wsa\.buf, \( int \)wsa\.len, JX_SEND_FLAGS \);\r?\n)",
             lambda m: m.group(1) + "\t\t\t\tJX_NET_TRACE( \"[send] %u B -> %d%s\", ( unsigned )wsa.len, nSent, nSent < 0 ? \" LOI\" : \"\" );" + nl, "send trace")
    t = sub1(t, r"(\t\t\t\tnError = ::select\( JxSelectNfds\( m_connectSocket \), NULL, &writefds, NULL, &tvCho \);\r?\n)",
             lambda m: m.group(1) + "\t\t\t\tJX_NET_TRACE( \"[send] WOULDBLOCK -> cho ghi duoc: select=%d\", nError );" + nl, "select write trace")
    wr(p, t, o); print("SocketClient.cpp: + shutdown truoc dong, select 200 ms, dau vet")
else: print("SocketClient.cpp: da va")

# ---------------------------------------------------------------- ClientStage.cpp: dau vet backpressure + Cleanup
p = R + "ClientStage.cpp"; t = rd(p); nl = nl_of(t)
if TAG not in t:
    o = t
    t = sub1(t, r"(\t\t\t\tif \( nCho == 0 \)\r?\n\t\t\t\t\{\r?\n)",
             lambda m: m.group(1) + N("#ifdef JX_PLATFORM_SDL\n\t\t\t\t\tJX_NET_TRACE( \"[rc] bo dem nhan DAY: giu %u, can %u\", ( unsigned )m_pRecvBuffer->GetUsed(), ( unsigned )used );\t// " + TAG + "\n#endif\n", nl), "backpressure")
    t = sub1(t, r"(STDMETHODIMP CGameClient::Cleanup\(\)\r?\n\{\r?\n\tHRESULT hr = E_FAIL;\r?\n)",
             lambda m: m.group(1) + N("#ifdef JX_PLATFORM_SDL\n\tJX_NET_TRACE( \"[api] Cleanup bat dau\" );\n#endif\n", nl), "Cleanup")
    t = sub1(t, r"(\t\tWaitForShutdownToComplete\(\);\r?\n\r?\n\t\thr = S_OK;\r?\n)",
             lambda m: m.group(1) + N("#ifdef JX_PLATFORM_SDL\n\t\tJX_NET_TRACE( \"[api] Cleanup xong\" );\n#endif\n", nl), "Cleanup xong")
    t = sub1(t, r"(STDMETHODIMP CGameClient::Shutdown\(\)\r?\n\{\r?\n)",
             lambda m: m.group(1) + N("#ifdef JX_PLATFORM_SDL\n\tJX_NET_TRACE( \"[api] Shutdown\" );\n#endif\n", nl), "Shutdown")
    wr(p, t, o); print("ClientStage.cpp: + dau vet")
else: print("ClientStage.cpp: da va")

# ---------------------------------------------------------------- harness: tach Shutdown / Cleanup / Release
p = ROOT + "\\ReverseTools\\mobile_x64\\test_rainbow\\test_rainbow.cpp"; t = rd(p); nl = nl_of(t)
old = '\t\t\tdouble t3 = Now(); c->Shutdown(); c->Cleanup(); c->Release(); f->Release();\n\t\t\tprintf( "  lan %d: Shutdown+Cleanup %.1f ms\\n", lan, Now() - t3 );\n'
if old.replace("\n", nl) in t:
    new = ('\t\t\tdouble t3 = Now(); c->Shutdown(); double t4 = Now(); c->Cleanup(); double t5 = Now(); c->Release(); f->Release(); double t6 = Now();\n'
           '\t\t\tprintf( "  lan %d: Shutdown %.1f ms, Cleanup %.1f ms, Release %.1f ms\\n", lan, t4 - t3, t5 - t4, t6 - t5 );\n')
    t = t.replace(old.replace("\n", nl), new.replace("\n", nl), 1); io.open(p, "wb").write(t.encode("latin-1")); print("test_rainbow.cpp: tach thoi gian")
else: print("test_rainbow.cpp: da tach / khong thay neo")
print("XONG 2b-2c")
