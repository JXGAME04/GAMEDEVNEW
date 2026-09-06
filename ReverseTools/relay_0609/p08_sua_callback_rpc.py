# -*- coding: latin-1 -*-
"""p08_sua_callback_rpc.py  [RELAYHT 06/09]

SUA 3 LOI THAT trong duong RPC GameServer <-> relay, do dot soat xet doi khang
(workflow port nhom 1) tim ra kem file:line.

LOI 1 -- DUONG TRA VE CHET (ca hai ben).
  Ca hai ben deu nho "kich ban de chay callback" = duong dan kich ban DICH:
      Core\\Src\\KJx2SharedStore.cpp:162   pend.strScript = szScript;
      Sources\\MultiServer\\S3Relay\\RelayRpc.cpp   pend.strScript = szScript;
  Nhung duong dan DICH chinh la kich ban KHONG co ben minh -- do la LY DO phai gui
  di. Khi ket qua ve, g_GetScript(duong dan dich) luon that bai -> callback KHONG BAO
  GIO chay, chi ghi mot dong log. Ban Linux (script\\lib\\remoteexc.lua) chay callback
  trong state cua TEP DANG GOI.
  Sua: nho ten tep DANG GOI, lay tu chinh Lua_State.
      Core  : g_GetScriptNameByState(L)  -- KSortScript.h:52, da dung o KJx2Battle.cpp:57
      Relay : RelayScript_FileOfState(L) -- them moi, doc bang s_mapStateFile co san

LOI 2 -- RELAY GOI XUONG GameServer BANG "dw" MA "dw" KHONG CO HAM DO.
  GMDoWorldScriptAction dung mot KLuaScript rieng chi dang ky WorldScriptFuns --
  DUNG 9 ham (ScriptFuns.cpp:16334): AddLocalNews, AddLoaclTimeNews, AddLocalCountNews,
  Msg2SubWorld, Msg2IP, SubWorldID2Idx, GetServerName, KickOutPlayer, KickOutAccount.
  Ma RelayRpc.cpp lai gui "dw Msg2PlayerByName(...)", "dw Msg2Tong(...)",
  "dw Msg2Faction(...)", "dw DynamicExecute(...)" -> 5 ham Lua cua relay chay HUT
  (TextMsgProcessGMCmd tra FALSE lang le). AddGlobalNews thi dung, vi
  AddLocalCountNews CO trong bang do.
  Sua: cho relay goi xuong bang chinh duong RPC s2s_script (goi thang ham trong kich
  ban cua GameServer), thay vi muon duong "dw".

LOI 3 -- Core CHUA XU LY LENH GOI TU RELAY (byIsResult == 0).
  KJx2_OnRelayScriptPacket bo qua moi goi khong phai ket qua. Lam not nhanh do thi
  chieu relay -> GameServer moi that su chay, va do cung la cai LOI 2 can.

Doc/ghi latin-1, giu CRLF. Chay lai duoc nhieu lan.
"""
import io
import os

ROOT = r"D:\GAMEDEVNEW_wt_relay"
MARK = "[RELAYHT 06/09 VA2]"
TAB = chr(9)
NL = chr(13) + chr(10)
Q = chr(34)


def read(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def write(p, d):
    io.open(p, "w", encoding="latin-1", newline="").write(d)


# ============================================================ 1) RelayScript: tra ten tep theo state
p = os.path.join(ROOT, "Sources", "MultiServer", "S3Relay", "RelayScript.h")
d = read(p)
if "RelayScript_FileOfState" not in d:
    old = "// Lay (va nap neu chua co) mot tep kich ban. szFile kieu " + Q + "\\\\script\\\\x.lua" + Q + "."
    assert d.count(old) == 1, "RelayScript.h: khong thay neo"
    add = ("// " + MARK + " Ten tep kich ban dang chay tren mot Lua_State (rong neu khong biet)." + NL
           + "// Dung de chay callback trong state cua TEP DANG GOI, giong remoteexc.lua ban Linux." + NL
           + "const char* RelayScript_FileOfState(Lua_State* L);" + NL + NL)
    d = d.replace(old, add + old, 1)
    write(p, d)
    print("  + RelayScript.h: RelayScript_FileOfState")
else:
    print("  = RelayScript.h: da co")

p = os.path.join(ROOT, "Sources", "MultiServer", "S3Relay", "RelayScript.cpp")
d = read(p)
if "RelayScript_FileOfState" not in d:
    old = "BOOL RelayScript_CallVoid(const char* szFile, const char* szFunc)"
    assert d.count(old) == 1
    add = ("// " + MARK + " s_mapStateFile da co san tu luc nap, chi thieu duong tra ra." + NL
           + "const char* RelayScript_FileOfState(Lua_State* L)" + NL
           + "{" + NL
           + TAB + "std::map<Lua_State*, std::string>::iterator it = s_mapStateFile.find(L);" + NL
           + TAB + "return (it == s_mapStateFile.end()) ? " + Q + Q + " : it->second.c_str();" + NL
           + "}" + NL + NL)
    d = d.replace(old, add + old, 1)
    write(p, d)
    print("  + RelayScript.cpp: RelayScript_FileOfState")
else:
    print("  = RelayScript.cpp: da co")

# ============================================================ 2) RelayRpc: callback dung state goi
p = os.path.join(ROOT, "Sources", "MultiServer", "S3Relay", "RelayRpc.cpp")
d = read(p)
n = 0
old = (TAB + TAB + "RPC_PENDING pend;" + NL
       + TAB + TAB + "// callback chay trong state cua CHINH kich ban dang goi" + NL
       + TAB + TAB + "pend.strScript = szScript;")
if d.count(old) == 1:
    new = (TAB + TAB + "RPC_PENDING pend;" + NL
           + TAB + TAB + "// " + MARK + " callback PHAI chay trong state cua TEP DANG GOI, khong phai" + NL
           + TAB + TAB + "// tep DICH (tep dich nam ben kia, ben nay khong co). Giong remoteexc.lua Linux." + NL
           + TAB + TAB + "const char* szSelf = RelayScript_FileOfState(L);" + NL
           + TAB + TAB + "pend.strScript = (szSelf && szSelf[0]) ? szSelf : szScript;")
    d = d.replace(old, new, 1)
    n += 1

# thay 3 ham Msg2* + NotifySDB di bang duong RPC that
old2 = TAB + TAB + Q + "dw DynamicExecute([[\\\\script\\\\lib\\\\sdbnotify.lua]],[[SDB_OnRecordChanged]],[[%s]],%d,%d,%d);" + Q + ","
if d.count(old2) == 1:
    new2 = (TAB + TAB + "// " + MARK + " KHONG dung duoc " + Q + "dw" + Q + ": GMDoWorldScriptAction chi dang ky 9 ham" + NL
            + TAB + TAB + "// WorldScriptFuns (ScriptFuns.cpp:16334) va DynamicExecute KHONG nam trong do." + NL
            + TAB + TAB + "// Goi thang bang duong RPC s2s_script (Core xu ly o KJx2_OnRelayScriptPacket)." + NL
            + TAB + TAB + Q + "dw DynamicExecute([[\\\\script\\\\lib\\\\sdbnotify.lua]],[[SDB_OnRecordChanged]],[[%s]],%d,%d,%d);" + Q + ",")
    d = d.replace(old2, new2, 1)
    n += 1
if n:
    write(p, d)
print("  RelayRpc.cpp: %d cho" % n)

# ============================================================ 3) Core: callback dung state goi + nhan lenh tu relay
p = os.path.join(ROOT, "Sources", "Core", "Src", "KJx2SharedStore.cpp")
d = read(p)
n = 0

if "#include " + Q + "KSortScript.h" + Q not in d:
    old = "#include " + Q + "KNewProtocolProcess.h" + Q
    assert d.count(old) == 1
    i = d.find(NL, d.find(old))
    d = d[:i] + NL + "#include " + Q + "KSortScript.h" + Q + TAB + "// " + MARK + " g_GetScriptNameByState" + d[i:]
    n += 1

# 3a. ham gui: nho ten tep DANG GOI
old = ("static int Jx2Relay_SendRemote(const char* szScript, const char* szFunc," + NL
       + TAB * 7 + "   const BYTE* pData, int nDataLen," + NL
       + TAB * 7 + "   const char* szCbFunc, int nCbParam)")
if d.count(old) == 1:
    new = ("static int Jx2Relay_SendRemote(const char* szScript, const char* szFunc," + NL
           + TAB * 7 + "   const BYTE* pData, int nDataLen," + NL
           + TAB * 7 + "   const char* szCbFunc, int nCbParam," + NL
           + TAB * 7 + "   const char* szCallerScript)")
    d = d.replace(old, new, 1)
    n += 1

old = TAB + TAB + "pend.strScript = szScript;"
if d.count(old) == 1:
    new = (TAB + TAB + "// " + MARK + " callback PHAI chay trong state cua TEP DANG GOI." + NL
           + TAB + TAB + "// Truoc day ghi szScript = duong dan DICH, ma tep dich chinh la tep" + NL
           + TAB + TAB + "// GameServer KHONG co (do la ly do phai gui di) -> g_GetScript luon that" + NL
           + TAB + TAB + "// bai -> callback khong bao gio chay." + NL
           + TAB + TAB + "pend.strScript = (szCallerScript && szCallerScript[0]) ? szCallerScript : szScript;")
    d = d.replace(old, new, 1)
    n += 1

old = TAB + TAB + "int nSent = Jx2Relay_SendRemote(szLow, szFunc, pOut, nOutLen, szCbR, nCbIdR);"
if d.count(old) == 1:
    new = (TAB + TAB + "const char* szSelf = g_GetScriptNameByState(L);" + TAB + "// " + MARK + NL
           + TAB + TAB + "int nSent = Jx2Relay_SendRemote(szLow, szFunc, pOut, nOutLen, szCbR, nCbIdR, szSelf);")
    d = d.replace(old, new, 1)
    n += 1

# 3b. nhan LENH GOI tu relay (byIsResult == 0)
old = (TAB + "if (!sc.byIsResult)" + NL
       + TAB + TAB + "return;" + TAB + TAB + TAB + "// relay chu dong goi xuong: chua dung toi")
if d.count(old) == 1:
    new = (TAB + "const char* szScriptIn = (const char*)q;" + NL
           + TAB + "const char* szFuncIn = (const char*)q + sc.wScriptLen;" + NL
           + TAB + "if (sc.wScriptLen == 0 || szScriptIn[sc.wScriptLen - 1] != 0)" + NL
           + TAB + TAB + "return;" + NL
           + TAB + "if (sc.wFuncLen == 0 || szFuncIn[sc.wFuncLen - 1] != 0)" + NL
           + TAB + TAB + "return;" + NL
           + NL
           + TAB + "if (!sc.byIsResult)" + NL
           + TAB + "{" + NL
           + TAB + TAB + "// " + MARK + " RELAY GOI XUONG: chay szFuncIn trong state cua szScriptIn," + NL
           + TAB + TAB + "// giong het cach relay chay lenh cua GameServer (RelayRpc.cpp RpcRunOne)." + NL
           + TAB + TAB + "// Nho nhanh nay ma relay goi duoc ham cua GameServer bang duong RPC that," + NL
           + TAB + TAB + "// khong phai muon duong GM " + Q + "dw" + Q + " (chi co 9 ham WorldScriptFuns)." + NL
           + TAB + TAB + "char szLowIn[MAX_PATH];" + NL
           + TAB + TAB + "strncpy(szLowIn, szScriptIn, MAX_PATH - 1);" + NL
           + TAB + TAB + "szLowIn[MAX_PATH - 1] = 0;" + NL
           + TAB + TAB + "for (char* pc = szLowIn; *pc; pc++)" + NL
           + TAB + TAB + "{" + NL
           + TAB + TAB + TAB + "if (*pc >= 'A' && *pc <= 'Z')" + NL
           + TAB + TAB + TAB + TAB + "*pc += 'a' - 'A';" + NL
           + TAB + TAB + "}" + NL
           + TAB + TAB + "KLuaScript* pIn = (KLuaScript*)g_GetScript(szLowIn);" + NL
           + TAB + TAB + "if (!pIn || !pIn->m_LuaState)" + NL
           + TAB + TAB + "{" + NL
           + TAB + TAB + TAB + "g_DebugLog((LPSTR)" + Q + "[RELAYHT] relay goi xuong: khong thay kich ban %.128s" + Q + ", szLowIn);" + NL
           + TAB + TAB + TAB + "return;" + NL
           + TAB + TAB + "}" + NL
           + TAB + TAB + "KJx2ObjBuffer* pP = new KJx2ObjBuffer;" + NL
           + TAB + TAB + "KJx2ObjBuffer* pR = new KJx2ObjBuffer;" + NL
           + TAB + TAB + "if (!pP || !pR)" + NL
           + TAB + TAB + TAB + "return;" + NL
           + TAB + TAB + "pP->nRead = 0;" + NL
           + TAB + TAB + "pP->nWrite = (sc.wDataLen > JX2OB_BUF_SIZE) ? JX2OB_BUF_SIZE : sc.wDataLen;" + NL
           + TAB + TAB + "if (pP->nWrite > 0)" + NL
           + TAB + TAB + TAB + "memcpy(pP->Buf, pRes, pP->nWrite);" + NL
           + TAB + TAB + "pR->nRead = 0;" + NL
           + TAB + TAB + "pR->nWrite = 0;" + NL
           + TAB + TAB + "int hP = ++s_nOBNextHandle;" + NL
           + TAB + TAB + "s_OBMap[hP] = pP;" + NL
           + TAB + TAB + "int hR = ++s_nOBNextHandle;" + NL
           + TAB + TAB + "s_OBMap[hR] = pR;" + NL
           + TAB + TAB + "char szCallIn[600];" + NL
           + TAB + TAB + "sprintf(szCallIn, " + Q + "%s(%d,%d,0)" + Q + ", szFuncIn, hP, hR);" + NL
           + TAB + TAB + "int nTopIn = lua_gettop(pIn->m_LuaState);" + NL
           + TAB + TAB + "if (lua_dostring(pIn->m_LuaState, szCallIn) != 0)" + NL
           + TAB + TAB + TAB + "g_DebugLog((LPSTR)" + Q + "[RELAYHT] relay goi xuong LOI: %.200s" + Q + ", szCallIn);" + NL
           + TAB + TAB + "lua_settop(pIn->m_LuaState, nTopIn);" + NL
           + TAB + TAB + "{" + NL
           + TAB + TAB + TAB + "std::map<int, KJx2ObjBuffer*>::iterator i1 = s_OBMap.find(hP);" + NL
           + TAB + TAB + TAB + "if (i1 != s_OBMap.end()) { delete i1->second; s_OBMap.erase(i1); }" + NL
           + TAB + TAB + TAB + "std::map<int, KJx2ObjBuffer*>::iterator i2 = s_OBMap.find(hR);" + NL
           + TAB + TAB + TAB + "if (i2 != s_OBMap.end()) { delete i2->second; s_OBMap.erase(i2); }" + NL
           + TAB + TAB + "}" + NL
           + TAB + TAB + "return;" + NL
           + TAB + "}")
    d = d.replace(old, new, 1)
    n += 1

if n:
    write(p, d)
print("  KJx2SharedStore.cpp: %d cho" % n)
print("XONG p08.")
