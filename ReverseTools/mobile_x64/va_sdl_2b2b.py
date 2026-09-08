# -*- coding: utf-8 -*-
"""[SDL 08/09 2b-2b] CEventSelect ban SDL bao FD_CONNECT mot lan sau AssociateEvent (nhu WSAEventSelect ghi nhan FD_CONNECT
ca khi socket da noi) -> CSocketClient::Run goi OnStartConnections -> callback enumServerConnectCreate nhu ban Win32."""
import io, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
p = ROOT + "\\Sources\\MultiServer\\Common\\EventSelect.h"
b = io.open(p, "rb").read(); t = b.decode("latin-1"); o = t
TAG2 = "[SDL 08/09 2b-2b]"
if TAG2 in t:
    print("EventSelect.h: da va 2b-2b"); sys.exit(0)
def rep(a, c):
    global t
    assert a in t, "khong thay: " + a[:60]
    t = t.replace(a, c, 1)
rep("CEventSelect() : m_lMask( 0 ), m_lEvents( 0 ), m_nErrorCode( 0 ), m_bAssociated( false ) { memset( m_nErr, 0, sizeof( m_nErr ) ); }",
    "CEventSelect() : m_lMask( 0 ), m_lEvents( 0 ), m_nErrorCode( 0 ), m_bAssociated( false ), m_bPendingConnect( false ) { memset( m_nErr, 0, sizeof( m_nErr ) ); }")
rep("\t\tm_lMask = lNetworkEvents; m_lEvents = 0; m_nErrorCode = 0; m_bAssociated = true;\n",
    "\t\tm_lMask = lNetworkEvents; m_lEvents = 0; m_nErrorCode = 0; m_bAssociated = true;\n"
    "\t\tm_bPendingConnect = ( lNetworkEvents & FD_CONNECT ) != 0;\t// " + TAG2 + " WSAEventSelect ghi nhan FD_CONNECT ca khi socket da noi -> bao 1 lan o WaitForEnumEvent dau\n")
rep("\tvoid DissociateEvent() { m_bAssociated = false; m_lEvents = 0; }",
    "\tvoid DissociateEvent() { m_bAssociated = false; m_lEvents = 0; m_bPendingConnect = false; }")
rep("\tbool\tm_bAssociated;\n\tint\t\tm_nErr[4];",
    "\tbool\tm_bAssociated;\n\tbool\tm_bPendingConnect;\n\tint\t\tm_nErr[4];")
rep("\tm_nErrorCode = 0; m_lEvents = 0; memset( m_nErr, 0, sizeof( m_nErr ) );\n\n\tif ( !m_bAssociated || INVALID_SOCKET == s )",
    "\tm_nErrorCode = 0; m_lEvents = 0; memset( m_nErr, 0, sizeof( m_nErr ) );\n\n"
    "\tif ( m_bPendingConnect && m_bAssociated )\n"
    "\t{\t// " + TAG2 + " nhu Win32: FD_CONNECT dau tien -> CSocketClient::Run goi OnStartConnections (callback enumServerConnectCreate)\n"
    "\t\tm_bPendingConnect = false;\n"
    "\t\tm_lEvents |= FD_CONNECT;\n"
    "\t\tdwTimeout = 0;\n"
    "\t}\n\n"
    "\tif ( !m_bAssociated || INVALID_SOCKET == s )")
rep("\tint n = ::select( JxSelectNfds( s ), &rd, bWantWrite ? &wr : NULL, &ex, &tv );\n\tif ( 0 == n ) return false;",
    "\tint n = ::select( JxSelectNfds( s ), &rd, bWantWrite ? &wr : NULL, &ex, &tv );\n\tif ( 0 == n ) return 0 != m_lEvents;")
assert sum(1 for c in o if ord(c) > 127) == sum(1 for c in t if ord(c) > 127)
io.open(p, "wb").write(t.encode("latin-1")); print("EventSelect.h: + pending connect OK")
