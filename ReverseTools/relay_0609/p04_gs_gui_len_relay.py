# -*- coding: latin-1 -*-
"""p04_gs_gui_len_relay.py  [RELAYHT 06/09]

NUA BEN GameServer cua RemoteExecute: kich ban KHONG co trong cay GameServer thi
GUI LEN S3RELAY chay, roi nhan ket qua ve chay callback. Kich ban CO SAN van chay
TAI CHO nhu cu -> 14 cho goi dang chay (weeklyrank, tongcastle, bigboss,
msg2allworld...) khong doi hanh vi.

  Headers/KProtocolDef.h              + relay_c2s_script / relay_s2c_script
  Sources/MultiServer/S3Relay/RelayRpc.cpp   goi ve GS dat ProtocolID = relay_s2c_script
  Sources/Core/Src/KJx2SharedStore.cpp       + Jx2Relay_SendRemote + KJx2_OnRelayScriptPacket
  Sources/Core/Src/KNewProtocolProcess.cpp   + dang ky bang giao thuc
  Sources/MultiServer/GameServer/KSOServer.cpp + case s2s_script

Doc/ghi latin-1, giu CRLF. Chay lai duoc nhieu lan.
"""
import io
import os

ROOT = r"D:\GAMEDEVNEW_wt_relay"
MARK = "[RELAYHT 06/09]"
TAB = chr(9)
NL = chr(13) + chr(10)
Q = chr(34)


def read(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def write(p, d):
    io.open(p, "w", encoding="latin-1", newline="").write(d)


def after_line(d, anchor, addition):
    assert d.count(anchor) == 1, "neo khong duy nhat: " + anchor[:70]
    i = d.find(NL, d.find(anchor))
    assert i > 0
    return d[:i] + NL + addition + d[i:]


# ---------------------------------------------------------------- 1) KProtocolDef.h
p = os.path.join(ROOT, "Headers", "KProtocolDef.h")
d = read(p)
if "relay_c2s_script" not in d:
    d = after_line(d, "relay_s2c_updatemaps,",
                   TAB + "relay_c2s_script," + TAB + TAB + "// " + MARK + " GameServer -> relay: goi kich ban" + NL
                   + TAB + "relay_s2c_script,")
    write(p, d)
    print("  + KProtocolDef.h: relay_c2s_script / relay_s2c_script")
else:
    print("  = KProtocolDef.h: da co")

# ---------------------------------------------------------------- 2) RelayRpc.cpp
p = os.path.join(ROOT, "Sources", "MultiServer", "S3Relay", "RelayRpc.cpp")
d = read(p)
old = TAB + "pSC->ProtocolID = relay_c2c_data;"
if d.count(old) == 1:
    d = d.replace(old, TAB + "pSC->ProtocolID = relay_s2c_script;" + TAB + "// " + MARK
                  + " de GameServer dinh tuyen duoc", 1)
    write(p, d)
    print("  + RelayRpc.cpp: ProtocolID = relay_s2c_script")
else:
    print("  = RelayRpc.cpp: da sua roi")

# ---------------------------------------------------------------- 3) KJx2SharedStore.cpp
p = os.path.join(ROOT, "Sources", "Core", "Src", "KJx2SharedStore.cpp")
d = read(p)
n = 0
if '#include "KNewProtocolProcess.h"' not in d:
    d = after_line(d, '#include "KJx2SharedStore.h"',
                   '#include "KNewProtocolProcess.h"' + TAB + '// ' + MARK + ' gui goi len relay' + NL
                   + '#include <string>')
    n += 1

if "Jx2Relay_SendRemote" not in d:
    code = (
        "//////////////////////////////////////////////////////////////////////////////" + NL
        + "// " + MARK + " REMOTEEXECUTE THAT: GameServer -> S3Relay" + NL
        + "//" + NL
        + "// Goi nam trong RELAY_DATA (nToIP = 0 nghia la 'da toi noi' -> relay tu xu ly)," + NL
        + "// byte dau than goi la s2s_script, roi RELAY_SCRIPT_CALL, roi 3 khoi:" + NL
        + "//     szScript / szFunc / du lieu ObjBuffer." + NL
        + "// Ham callback duoc tim trong state cua CHINH tep kich ban da goi -- dung theo" + NL
        + "// loi remoteexc.lua ban Linux (callback dinh nghia cung tep voi ham duoc goi)." + NL
        + "//////////////////////////////////////////////////////////////////////////////" + NL
        + "#define JX2RPC_MAX_PENDING\t512" + NL
        + "#define JX2RPC_MAX_PACKET\t8192" + NL
        + NL
        + "struct KJx2RpcPending" + NL
        + "{" + NL
        + TAB + "std::string\tstrScript;" + NL
        + TAB + "std::string\tstrCbFunc;" + NL
        + TAB + "int\t\t\tnCbParam;" + NL
        + "};" + NL
        + NL
        + "static std::map<DWORD, KJx2RpcPending>\ts_mapRpcPending;" + NL
        + "static DWORD\t\t\t\t\t\t\ts_dwRpcNextId = 0;" + NL
        + NL
        + "// Tra ve 1 = da xep vao hang gui len relay." + NL
        + "static int Jx2Relay_SendRemote(const char* szScript, const char* szFunc," + NL
        + TAB + TAB + TAB + TAB + TAB + TAB + TAB + "   const BYTE* pData, int nDataLen," + NL
        + TAB + TAB + TAB + TAB + TAB + TAB + TAB + "   const char* szCbFunc, int nCbParam)" + NL
        + "{" + NL
        + TAB + "if (!szScript || !szScript[0] || !szFunc || !szFunc[0])" + NL
        + TAB + TAB + "return 0;" + NL
        + TAB + "if (nDataLen < 0)" + NL
        + TAB + TAB + "nDataLen = 0;" + NL
        + NL
        + TAB + "size_t nScriptLen = strlen(szScript) + 1;" + NL
        + TAB + "size_t nFuncLen = strlen(szFunc) + 1;" + NL
        + TAB + "size_t nBody = 1 + sizeof(RELAY_SCRIPT_CALL) + nScriptLen + nFuncLen + nDataLen;" + NL
        + TAB + "size_t nTotal = sizeof(RELAY_DATA) + nBody;" + NL
        + TAB + "if (nTotal > JX2RPC_MAX_PACKET)" + NL
        + TAB + "{" + NL
        + TAB + TAB + "g_DebugLog((LPSTR)" + Q + "[RELAYHT] RemoteExecute: goi qua lon (%d byte) %.128s" + Q + ", (int)nTotal, szScript);" + NL
        + TAB + TAB + "return 0;" + NL
        + TAB + "}" + NL
        + NL
        + TAB + "DWORD dwCallId = 0;" + NL
        + TAB + "if (szCbFunc && szCbFunc[0])" + NL
        + TAB + "{" + NL
        + TAB + TAB + "if (s_mapRpcPending.size() >= JX2RPC_MAX_PENDING)" + NL
        + TAB + TAB + "{" + NL
        + TAB + TAB + TAB + "g_DebugLog((LPSTR)" + Q + "[RELAYHT] RemoteExecute: tran so lenh cho ket qua" + Q + ");" + NL
        + TAB + TAB + TAB + "return 0;" + NL
        + TAB + TAB + "}" + NL
        + TAB + TAB + "dwCallId = ++s_dwRpcNextId;" + NL
        + TAB + TAB + "if (dwCallId == 0)" + NL
        + TAB + TAB + TAB + "dwCallId = ++s_dwRpcNextId;" + NL
        + TAB + TAB + "KJx2RpcPending pend;" + NL
        + TAB + TAB + "pend.strScript = szScript;" + NL
        + TAB + TAB + "pend.strCbFunc = szCbFunc;" + NL
        + TAB + TAB + "pend.nCbParam = nCbParam;" + NL
        + TAB + TAB + "s_mapRpcPending[dwCallId] = pend;" + NL
        + TAB + "}" + NL
        + NL
        + TAB + "char szBuf[JX2RPC_MAX_PACKET];" + NL
        + TAB + "RELAY_DATA* pRD = (RELAY_DATA*)szBuf;" + NL
        + TAB + "pRD->ProtocolFamily = pf_relay;" + NL
        + TAB + "pRD->ProtocolID = relay_c2c_data;" + NL
        + TAB + "pRD->nToIP = 0;" + TAB + TAB + TAB + TAB + "// 0 = da toi noi: relay tu xu ly than goi" + NL
        + TAB + "pRD->nToRelayID = 0;" + NL
        + TAB + "pRD->nFromIP = 0;" + NL
        + TAB + "pRD->nFromRelayID = 0;" + NL
        + TAB + "pRD->routeDateLength = (WORD)nBody;" + NL
        + NL
        + TAB + "BYTE* p = (BYTE*)szBuf + sizeof(RELAY_DATA);" + NL
        + TAB + "*p++ = (BYTE)s2s_script;" + NL
        + TAB + "RELAY_SCRIPT_CALL* pSC = (RELAY_SCRIPT_CALL*)p;" + NL
        + TAB + "pSC->ProtocolFamily = pf_relay;" + NL
        + TAB + "pSC->ProtocolID = relay_c2s_script;" + NL
        + TAB + "pSC->dwCallId = dwCallId;" + NL
        + TAB + "pSC->dwGameSvrId = 0;" + NL
        + TAB + "pSC->byIsResult = 0;" + NL
        + TAB + "pSC->wScriptLen = (WORD)nScriptLen;" + NL
        + TAB + "pSC->wFuncLen = (WORD)nFuncLen;" + NL
        + TAB + "pSC->wDataLen = (WORD)nDataLen;" + NL
        + TAB + "p += sizeof(RELAY_SCRIPT_CALL);" + NL
        + NL
        + TAB + "memcpy(p, szScript, nScriptLen);" + TAB + "p += nScriptLen;" + NL
        + TAB + "memcpy(p, szFunc, nFuncLen);" + TAB + TAB + "p += nFuncLen;" + NL
        + TAB + "if (nDataLen > 0)" + NL
        + TAB + TAB + "memcpy(p, pData, nDataLen);" + NL
        + NL
        + TAB + "g_NewProtocolProcess.PushMsgInTransfer(szBuf, nTotal);" + NL
        + TAB + "return 1;" + NL
        + "}" + NL
        + NL
        + "// Relay tra ket qua ve. Goi tu KNewProtocolProcess (bang giao thuc)." + NL
        + "void KJx2_OnRelayScriptPacket(const void* pData, int nSize)" + NL
        + "{" + NL
        + TAB + "if (!pData || nSize < (int)sizeof(RELAY_SCRIPT_CALL))" + NL
        + TAB + TAB + "return;" + NL
        + TAB + "RELAY_SCRIPT_CALL sc;" + NL
        + TAB + "memcpy(&sc, pData, sizeof(sc));" + NL
        + TAB + "const BYTE* q = (const BYTE*)pData + sizeof(RELAY_SCRIPT_CALL);" + NL
        + TAB + "int nLeft = nSize - (int)sizeof(RELAY_SCRIPT_CALL);" + NL
        + TAB + "if ((int)sc.wScriptLen + sc.wFuncLen + sc.wDataLen > nLeft)" + NL
        + TAB + TAB + "return;" + NL
        + TAB + "const BYTE* pRes = q + sc.wScriptLen + sc.wFuncLen;" + NL
        + NL
        + TAB + "if (!sc.byIsResult)" + NL
        + TAB + TAB + "return;" + TAB + TAB + TAB + "// relay chu dong goi xuong: chua dung toi" + NL
        + NL
        + TAB + "std::map<DWORD, KJx2RpcPending>::iterator it = s_mapRpcPending.find(sc.dwCallId);" + NL
        + TAB + "if (it == s_mapRpcPending.end())" + NL
        + TAB + TAB + "return;" + NL
        + TAB + "KJx2RpcPending pend = it->second;" + NL
        + TAB + "s_mapRpcPending.erase(it);" + NL
        + TAB + "if (pend.strCbFunc.empty())" + NL
        + TAB + TAB + "return;" + NL
        + NL
        + TAB + "KLuaScript* pScript = (KLuaScript*)g_GetScript((char*)pend.strScript.c_str());" + NL
        + TAB + "if (!pScript || !pScript->m_LuaState)" + NL
        + TAB + "{" + NL
        + TAB + TAB + "g_DebugLog((LPSTR)" + Q + "[RELAYHT] callback: khong thay kich ban %.128s" + Q + ", pend.strScript.c_str());" + NL
        + TAB + TAB + "return;" + NL
        + TAB + "}" + NL
        + NL
        + TAB + "KJx2ObjBuffer* pOb = new KJx2ObjBuffer;" + NL
        + TAB + "if (!pOb)" + NL
        + TAB + TAB + "return;" + NL
        + TAB + "pOb->nRead = 0;" + NL
        + TAB + "pOb->nWrite = (sc.wDataLen > JX2OB_BUF_SIZE) ? JX2OB_BUF_SIZE : sc.wDataLen;" + NL
        + TAB + "if (pOb->nWrite > 0)" + NL
        + TAB + TAB + "memcpy(pOb->Buf, pRes, pOb->nWrite);" + NL
        + TAB + "int hRes = ++s_nOBNextHandle;" + NL
        + TAB + "s_OBMap[hRes] = pOb;" + NL
        + NL
        + TAB + "char szCall[600];" + NL
        + TAB + "sprintf(szCall, " + Q + "%s(%d,%d)" + Q + ", pend.strCbFunc.c_str(), pend.nCbParam, hRes);" + NL
        + TAB + "int nTop = lua_gettop(pScript->m_LuaState);" + NL
        + TAB + "if (lua_dostring(pScript->m_LuaState, szCall) != 0)" + NL
        + TAB + TAB + "g_DebugLog((LPSTR)" + Q + "[RELAYHT] callback LOI: %.200s" + Q + ", szCall);" + NL
        + TAB + "lua_settop(pScript->m_LuaState, nTop);" + NL
        + NL
        + TAB + "{" + NL
        + TAB + TAB + "std::map<int, KJx2ObjBuffer*>::iterator itR = s_OBMap.find(hRes);" + NL
        + TAB + TAB + "if (itR != s_OBMap.end())" + NL
        + TAB + TAB + "{" + NL
        + TAB + TAB + TAB + "delete itR->second;" + NL
        + TAB + TAB + TAB + "s_OBMap.erase(itR);" + NL
        + TAB + TAB + "}" + NL
        + TAB + "}" + NL
        + "}" + NL
        + NL
    )
    anchor = "int LuaJX2_RemoteExecute(Lua_State* L)"
    assert d.count(anchor) == 1
    d = d.replace(anchor, code + anchor, 1)
    n += 1

old = (TAB + "if (!pScript || !pScript->m_LuaState)" + NL
       + TAB + "{" + NL
       + TAB + TAB + "g_DebugLog((LPSTR)" + Q + "[PORT5] RemoteExecute: script chua nap %.128s" + Q + ", szLow);" + NL
       + TAB + TAB + "return 0;" + NL
       + TAB + "}")
if d.count(old) == 1:
    new = (TAB + "if (!pScript || !pScript->m_LuaState)" + NL
           + TAB + "{" + NL
           + TAB + TAB + "// " + MARK + " khong co kich ban nay trong cay GameServer -> GUI LEN S3RELAY" + NL
           + TAB + TAB + "// chay (truoc day chi ghi log roi bo qua). Kich ban CO SAN van chay tai cho" + NL
           + TAB + TAB + "// nhu cu nen cac cho goi dang chay khong doi hanh vi." + NL
           + TAB + TAB + "const BYTE* pOut = NULL;" + NL
           + TAB + TAB + "int nOutLen = 0;" + NL
           + TAB + TAB + "{" + NL
           + TAB + TAB + TAB + "std::map<int, KJx2ObjBuffer*>::iterator itP = s_OBMap.find(nHandle);" + NL
           + TAB + TAB + TAB + "if (itP != s_OBMap.end())" + NL
           + TAB + TAB + TAB + "{" + NL
           + TAB + TAB + TAB + TAB + "pOut = itP->second->Buf;" + NL
           + TAB + TAB + TAB + TAB + "nOutLen = itP->second->nWrite;" + NL
           + TAB + TAB + TAB + "}" + NL
           + TAB + TAB + "}" + NL
           + TAB + TAB + "const char* szCbR = (Lua_GetTopIndex(L) >= 4 && Lua_IsString(L, 4)) ? Lua_ValueToString(L, 4) : NULL;" + NL
           + TAB + TAB + "int nCbIdR = (Lua_GetTopIndex(L) >= 5 && Lua_IsNumber(L, 5)) ? (int)Lua_ValueToNumber(L, 5) : 0;" + NL
           + TAB + TAB + "int nSent = Jx2Relay_SendRemote(szLow, szFunc, pOut, nOutLen, szCbR, nCbIdR);" + NL
           + TAB + TAB + "if (!nSent)" + NL
           + TAB + TAB + TAB + "g_DebugLog((LPSTR)" + Q + "[RELAYHT] RemoteExecute: khong nap duoc, gui len relay cung hong: %.128s" + Q + ", szLow);" + NL
           + TAB + TAB + "Lua_PushNumber(L, nSent);" + NL
           + TAB + TAB + "return 1;" + NL
           + TAB + "}")
    d = d.replace(old, new, 1)
    n += 1
if n:
    write(p, d)
print("  KJx2SharedStore.cpp: %d cho" % n)

# ---------------------------------------------------------------- 4) KNewProtocolProcess.cpp
p = os.path.join(ROOT, "Sources", "Core", "Src", "KNewProtocolProcess.cpp")
d = read(p)
if "sProcessRelayScript" not in d:
    anchor = TAB + "m_ProtocolTable[MAKEWORD(pf_gamemaster, gm_c2s_execute)] = P_ProcessGMExecute;"
    assert d.count(anchor) == 1, "KNewProtocolProcess.cpp: khong thay neo dang ky"
    helper = (
        "// " + MARK + " relay tra ket qua RemoteExecute ve (than goi = RELAY_SCRIPT_CALL)" + NL
        + "void KJx2_OnRelayScriptPacket(const void* pData, int nSize);" + NL
        + "static void sProcessRelayScript(IClient* pTransfer," + NL
        + TAB + TAB + TAB + TAB + TAB + TAB + TAB + "DWORD dwFromIP, DWORD dwFromRelayID," + NL
        + TAB + TAB + TAB + TAB + TAB + TAB + TAB + "int nPlayerIndex, BYTE* pData, int nDataSize)" + NL
        + "{" + NL
        + TAB + "KJx2_OnRelayScriptPacket(pData, nDataSize);" + NL
        + "}" + NL
        + NL
    )
    # dat ham phu TRUOC ham chua dong dang ky
    i = d.rfind("KNewProtocolProcess::", 0, d.find(anchor))
    j = d.rfind(NL, 0, i)
    # lui ve dau dong khai bao kieu tra ve
    k = d.rfind(NL, 0, j)
    d = d[:k + len(NL)] + helper + d[k + len(NL):]
    d = after_line(d, anchor,
                   TAB + "m_ProtocolTable[MAKEWORD(pf_relay, relay_s2c_script)] = sProcessRelayScript;"
                   + TAB + "// " + MARK)
    write(p, d)
    print("  + KNewProtocolProcess.cpp: dang ky relay_s2c_script")
else:
    print("  = KNewProtocolProcess.cpp: da co")

# ---------------------------------------------------------------- 5) KSOServer.cpp
p = os.path.join(ROOT, "Sources", "MultiServer", "GameServer", "KSOServer.cpp")
d = read(p)
if "s2s_script" not in d:
    old = (TAB + "case s2s_execute:" + NL
           + TAB + TAB + "{" + NL
           + TAB + TAB + TAB + "BYTE* pS = (BYTE *)pData;" + NL
           + TAB + TAB + TAB + "m_pCoreServerShell->ProcessExecuteMessage((const char *)(pS + 1), dataLength - 1);" + NL
           + TAB + TAB + "}" + NL
           + TAB + TAB + "break;")
    assert d.count(old) == 1, "KSOServer.cpp: khong thay case s2s_execute"
    new = (old + NL
           + TAB + "case s2s_script:" + TAB + TAB + "// " + MARK + " relay tra ket qua RemoteExecute ve" + NL
           + TAB + TAB + "{" + NL
           + TAB + TAB + TAB + "BYTE* pS = (BYTE *)pData;" + NL
           + TAB + TAB + TAB + "m_pCoreServerShell->ProcessNewClientMessage(m_pTransferClient, 0, 0, -1," + NL
           + TAB + TAB + TAB + TAB + "(const char *)(pS + 1), dataLength - 1);" + NL
           + TAB + TAB + "}" + NL
           + TAB + TAB + "break;")
    d = d.replace(old, new, 1)
    write(p, d)
    print("  + KSOServer.cpp: case s2s_script")
else:
    print("  = KSOServer.cpp: da co")

print("XONG p04.")
