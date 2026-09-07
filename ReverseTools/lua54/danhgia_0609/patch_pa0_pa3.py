r"""patch_pa0_pa3.py - va Engine/Core (TCVN3/GBK -> doc/ghi latin-1) + boot_gia.py cho PA-0/PA-3:
  KLuaScript.cpp : C1 chunkname = "@" + ten script; C2 ma loi ExecuteCode; C3 CallFunction 'b' 'i' 't', GetValuesFromStack 'b'
  KLuaScript.h   : typedef KLuaTableFill
  KPerfTick.cpp  : moi 10 ky perf -> lua4_prof_write("jx_lua_prof.log", 40)
  KSortScript.cpp: g_GetScriptNameByState tra chu coroutine/thread qua lua4_owner; in so lan IncludeOnce bo qua
  boot_gia.py    : LUA54_MOT_STATE=1 (STUB tren master), BOOT_DUMP_GLOBALS=<tep>
"""
import io, re, sys, os
ROOT = r"D:\GAMEDEVNEW"
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hb(s): return sum(1 for c in s if ord(c) > 127)
def patch(p, pairs, regex=()):
    s = rd(p); h0 = hb(s); n = 0
    for old, new in pairs:
        if old not in s:
            print("KHONG THAY anchor trong %s: %r" % (os.path.basename(p), old[:70])); sys.exit(1)
        if s.count(old) != 1:
            print("anchor KHONG duy nhat (%d) trong %s: %r" % (s.count(old), os.path.basename(p), old[:70])); sys.exit(1)
        s = s.replace(old, new); n += 1
    for pat, rep, cnt in regex:
        s2, k = re.subn(pat, rep, s, count=cnt, flags=re.S)
        if k != cnt:
            print("regex %r khop %d (mong %d) trong %s" % (pat[:50], k, cnt, os.path.basename(p))); sys.exit(1)
        s = s2; n += 1
    wr(p, s)
    print("%-18s: %d cho, byte cao %d -> %d" % (os.path.basename(p), n, h0, hb(s)))

NL = "\r\n"
# ---------------- KLuaScript.cpp ----------------
P = ROOT + r"\Sources\Engine\Src\KLuaScript.cpp"
s = rd(P)
nl = NL if "\r\n" in s else "\n"
patch(P, [
 ("\tif (Lua_CompileBuffer(m_LuaState, (char *) pBuffer, dwLen, NULL) != 0)",
  "\t// [LUA54 06/09 toi] C1: ten chunk = \"@\" + ten script -> loi runtime ghi \"\\script\\x.lua:12:\" thay vi [string \"?\"]" + nl +
  "\tchar szChunk[160];" + nl +
  "\tszChunk[0] = '@';" + nl +
  "\tstrncpy(szChunk + 1, m_szScriptName[0] ? m_szScriptName : \"?\", sizeof(szChunk) - 2);" + nl +
  "\tszChunk[sizeof(szChunk) - 1] = 0;" + nl +
  "\tif (Lua_CompileBuffer(m_LuaState, (char *) pBuffer, dwLen, szChunk) != 0)"),
 ("\tint state;" + nl + "\tif (state = Lua_Execute(m_LuaState) != 0)",
  "\tint state;" + nl + "\tstate = Lua_Execute(m_LuaState);\t// [LUA54 06/09 toi] C2: truoc day 'state = (... != 0)' -> ma loi luon [1]" + nl + "\tif (state != 0)"),
 ("\t\t\tcase 'p':" + nl + "\t\t\t\t{" + nl + "\t\t\t\t\tpPoint = va_arg(vlist, void *);",
  "\t\t\tcase 'b'://[LUA54 06/09 toi] C3: boolean that (int 0/1)" + nl +
  "\t\t\t\t{" + nl +
  "\t\t\t\t\tint nB = va_arg(vlist, int);" + nl +
  "\t\t\t\t\tLua_PushBoolean(m_LuaState, nB ? 1 : 0);" + nl +
  "\t\t\t\t\tnArgnum ++;" + nl +
  "\t\t\t\t}" + nl +
  "\t\t\t\tbreak;" + nl +
  "\t\t\tcase 'i'://[LUA54 06/09 toi] C3: so nguyen 64 bit that (long long)" + nl +
  "\t\t\t\t{" + nl +
  "\t\t\t\t\tlong long llV = va_arg(vlist, long long);" + nl +
  "\t\t\t\t\tLua_PushInteger(m_LuaState, llV);" + nl +
  "\t\t\t\t\tnArgnum ++;" + nl +
  "\t\t\t\t}" + nl +
  "\t\t\t\tbreak;" + nl +
  "\t\t\tcase 'p':" + nl + "\t\t\t\t{" + nl + "\t\t\t\t\tpPoint = va_arg(vlist, void *);"),
], regex=[
 # 't' rong -> bang dien bang callback (2 va_arg: KLuaTableFill, void*)
 (r"\t\t\tcase 't':[^\n]*\n\t\t\t\t\{\s*\}\r?\n\t\t\t\tbreak;",
  "\t\t\tcase 't'://[LUA54 06/09 toi] C3: bang: (KLuaTableFill pfn, void* ctx) -> tao bang moi o dinh, pfn dien bang Lua_SetTable_*" + nl +
  "\t\t\t\t{" + nl +
  "\t\t\t\t\tKLuaTableFill pfnFill = va_arg(vlist, KLuaTableFill);" + nl +
  "\t\t\t\t\tvoid* pCtx = va_arg(vlist, void *);" + nl +
  "\t\t\t\t\tLua_NewTable(m_LuaState);" + nl +
  "\t\t\t\t\tif (pfnFill) pfnFill(m_LuaState, Lua_GetTopIndex(m_LuaState), pCtx);" + nl +
  "\t\t\t\t\tnArgnum ++;" + nl +
  "\t\t\t\t}" + nl +
  "\t\t\t\tbreak;", 1),
 # GetValuesFromStack: them 'b' truoc case 's' (nhan dien qua pString = va_arg)
 (r"(\t\t\tcase 's':[^\n]*\n\t\t\t\t\{\r?\n\t\t\t\t\tpString = va_arg\(vlist, const char \*\*\);)",
  "\t\t\tcase 'b'://[LUA54 06/09 toi] C3: boolean hoac so -> int 0/1" + nl +
  "\t\t\t\t{" + nl +
  "\t\t\t\t\tpInt = va_arg(vlist, int *);" + nl +
  "\t\t\t\t\tif (pInt == NULL)" + nl +
  "\t\t\t\t\t\treturn FALSE;" + nl +
  "\t\t\t\t\tif (Lua_IsNumber(m_LuaState, nIndex))" + nl +
  "\t\t\t\t\t\t* pInt = (Lua_ValueToNumber(m_LuaState, nIndex ++) != 0.0) ? 1 : 0;" + nl +
  "\t\t\t\t\telse" + nl +
  "\t\t\t\t\t\t* pInt = Lua_ToBoolean(m_LuaState, nIndex ++);" + nl +
  "\t\t\t\t}" + nl +
  "\t\t\t\tbreak;" + nl + r"\1", 1),
])

# ---------------- KLuaScript.h ----------------
P = ROOT + r"\Sources\Engine\Src\KLuaScript.h"
s = rd(P); nl = NL if "\r\n" in s else "\n"
patch(P, [
 ("typedef struct  {" + nl + "\tconst char *name;" + nl + "\tlua_CFunction func;",
  "// [LUA54 06/09 toi] C3: CallFunction dinh dang 't' = (KLuaTableFill, void* ctx): tao bang moi (chi so nTableIndex) roi goi pfn dien" + nl +
  "typedef void (*KLuaTableFill)(Lua_State* L, int nTableIndex, void* pCtx);" + nl +
  "typedef struct  {" + nl + "\tconst char *name;" + nl + "\tlua_CFunction func;"),
])

# ---------------- KPerfTick.cpp ----------------
P = ROOT + r"\Sources\Core\Src\KPerfTick.cpp"
s = rd(P); nl = NL if "\r\n" in s else "\n"
patch(P, [
 (nl + "\ts_dWindowStart = dNow;" + nl + "}",
  nl + "\t// [LUA54 06/09 toi] C11: profiler lay mau trong Lua54Dll (LUA54_PROF, mac dinh moi 2000 lenh) -> moi 10 ky" + nl +
  "\t// (10 phut) ghi bang xep hang tep:dong vao jx_lua_prof.log roi xoa. Lay ham qua GetProcAddress (DLL cu -> bo qua)." + nl +
  "\t{" + nl +
  "\t\ttypedef long long (*PFN_L4PROFW)(const char*, int);" + nl +
  "\t\tstatic PFN_L4PROFW s_pfnProfW = NULL;" + nl +
  "\t\tstatic int s_nProfThu = 0, s_nProfKy = 0;" + nl +
  "\t\tif (!s_nProfThu)" + nl +
  "\t\t{" + nl +
  "\t\t\ts_nProfThu = 1;" + nl +
  "\t\t\tHMODULE hLua = GetModuleHandleA(\"Lua54Dll.dll\");" + nl +
  "\t\t\tif (hLua) s_pfnProfW = (PFN_L4PROFW)GetProcAddress(hLua, \"lua4_prof_write\");" + nl +
  "\t\t}" + nl +
  "\t\tif (s_pfnProfW && ++s_nProfKy >= 10)" + nl +
  "\t\t{" + nl +
  "\t\t\ts_nProfKy = 0;" + nl +
  "\t\t\ts_pfnProfW(\"jx_lua_prof.log\", 40);" + nl +
  "\t\t}" + nl +
  "\t}" + nl +
  "\ts_dWindowStart = dNow;" + nl + "}"),
])

# ---------------- KSortScript.cpp ----------------
P = ROOT + r"\Sources\Core\Src\KSortScript.cpp"
s = rd(P); nl = NL if "\r\n" in s else "\n"
old_fn = ("const char * g_GetScriptNameByState(Lua_State* L)" + nl + "{" + nl +
 "\tstatic std::map<Lua_State*, int> s_mapState;" + nl +
 "\tif (!L)" + nl + "\t\treturn NULL;" + nl +
 "\tstd::map<Lua_State*, int>::iterator it = s_mapState.find(L);" + nl +
 "\tif (it != s_mapState.end() && it->second >= 0 && it->second < (int)nCurrentScriptNum" + nl +
 "\t\t&& g_ScriptSet[it->second].m_LuaState == L)" + nl +
 "\t\treturn g_ScriptSet[it->second].m_szScriptName;" + nl +
 "\tfor (unsigned int i = 0; i < nCurrentScriptNum && i < MAX_SCRIPT_IN_SET; i++)" + nl +
 "\t{" + nl +
 "\t\tif (g_ScriptSet[i].m_LuaState == L)" + nl +
 "\t\t{" + nl +
 "\t\t\ts_mapState[L] = (int)i;" + nl +
 "\t\t\treturn g_ScriptSet[i].m_szScriptName;" + nl +
 "\t\t}" + nl +
 "\t}" + nl +
 "\treturn NULL;" + nl + "}")
new_fn = ("// [LUA54 06/09 toi] L co the la COROUTINE (SayWait) hay thread cua che do mot state: hoi Lua54Dll thread script so huu" + nl +
 "static Lua_State* sLuaOwner(Lua_State* L)" + nl + "{" + nl +
 "\ttypedef Lua_State* (*PFN_L4OWNER)(Lua_State*);" + nl +
 "\tstatic PFN_L4OWNER s_pfnOwner = NULL;" + nl +
 "\tstatic int s_nThu = 0;" + nl +
 "\tif (!s_nThu)" + nl + "\t{" + nl +
 "\t\ts_nThu = 1;" + nl +
 "\t\tHMODULE h = GetModuleHandleA(\"Lua54Dll.dll\");" + nl +
 "\t\tif (h) s_pfnOwner = (PFN_L4OWNER)GetProcAddress(h, \"lua4_owner\");" + nl +
 "\t}" + nl +
 "\treturn s_pfnOwner ? s_pfnOwner(L) : NULL;" + nl + "}" + nl + nl +
 "const char * g_GetScriptNameByState(Lua_State* L)" + nl + "{" + nl +
 "\tstatic std::map<Lua_State*, int> s_mapState;" + nl +
 "\tif (!L)" + nl + "\t\treturn NULL;" + nl +
 "\tstd::map<Lua_State*, int>::iterator it = s_mapState.find(L);" + nl +
 "\tif (it != s_mapState.end() && it->second >= 0 && it->second < (int)nCurrentScriptNum)" + nl +
 "\t{" + nl +
 "\t\tif (g_ScriptSet[it->second].m_LuaState == L)" + nl +
 "\t\t\treturn g_ScriptSet[it->second].m_szScriptName;" + nl +
 "\t\tLua_State* L2 = sLuaOwner(L);" + nl +
 "\t\tif (L2 && L2 != L && g_ScriptSet[it->second].m_LuaState == L2)" + nl +
 "\t\t\treturn g_ScriptSet[it->second].m_szScriptName;" + nl +
 "\t}" + nl +
 "\tfor (unsigned int i = 0; i < nCurrentScriptNum && i < MAX_SCRIPT_IN_SET; i++)" + nl +
 "\t{" + nl +
 "\t\tif (g_ScriptSet[i].m_LuaState == L)" + nl +
 "\t\t{" + nl +
 "\t\t\ts_mapState[L] = (int)i;" + nl +
 "\t\t\treturn g_ScriptSet[i].m_szScriptName;" + nl +
 "\t\t}" + nl +
 "\t}" + nl +
 "\t{" + nl +
 "\t\tLua_State* L2 = sLuaOwner(L);" + nl +
 "\t\tif (L2 && L2 != L)" + nl +
 "\t\t{" + nl +
 "\t\t\tfor (unsigned int i = 0; i < nCurrentScriptNum && i < MAX_SCRIPT_IN_SET; i++)" + nl +
 "\t\t\t{" + nl +
 "\t\t\t\tif (g_ScriptSet[i].m_LuaState == L2)" + nl +
 "\t\t\t\t{" + nl +
 "\t\t\t\t\ts_mapState[L] = (int)i;" + nl +
 "\t\t\t\t\treturn g_ScriptSet[i].m_szScriptName;" + nl +
 "\t\t\t\t}" + nl +
 "\t\t\t}" + nl +
 "\t\t}" + nl +
 "\t}" + nl +
 "\treturn NULL;" + nl + "}")
patch(P, [(old_fn, new_fn)], regex=[
 # sau dong log LoadAllScript: in so lan IncludeOnce bo qua + che do mot state
 (r'(\tprintf\("%s\\n", szMsg\);\r?\n\tg_DebugLog\(\(LPSTR\)"%s", szMsg\);\r?\n\})',
  "\tprintf(\"%s\\n\", szMsg);" + nl + "\tg_DebugLog((LPSTR)\"%s\", szMsg);" + nl +
  "\t// [LUA54 06/09 toi] IncludeOnce (@IncludeOnce) + che do mot state (LUA54_MOT_STATE)" + nl +
  "\t{" + nl +
  "\t\ttypedef long long (*PFN_ONCE)(void);" + nl +
  "\t\ttypedef int (*PFN_MOT)(void);" + nl +
  "\t\tPFN_ONCE pfnOnce = hLua ? (PFN_ONCE)GetProcAddress(hLua, \"lua4_inc_once_skip\") : NULL;" + nl +
  "\t\tPFN_MOT pfnMot = hLua ? (PFN_MOT)GetProcAddress(hLua, \"lua4_mot_state\") : NULL;" + nl +
  "\t\tif (pfnOnce || pfnMot)" + nl +
  "\t\t{" + nl +
  "\t\t\tsprintf_s(szMsg, sizeof(szMsg), \"[script] IncludeOnce bo qua %lld lan; che do mot state: %s\"," + nl +
  "\t\t\t\tpfnOnce ? pfnOnce() : 0LL, (pfnMot && pfnMot()) ? \"BAT (LUA54_MOT_STATE=1)\" : \"tat\");" + nl +
  "\t\t\tprintf(\"%s\\n\", szMsg);" + nl +
  "\t\t\tg_DebugLog((LPSTR)\"%s\", szMsg);" + nl +
  "\t\t}" + nl +
  "\t}" + nl + "}", 1),
])

# ---------------- boot_gia.py ----------------
P = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\tools\sapxep\boot_gia.py"
s = io.open(P, "r", encoding="utf-8").read()
old1 = 'D.lua4_pushcclosure.argtypes = [vp, CFN, ci]\n'
new1 = old1 + '''try:
    D.lua4_master.restype = vp; D.lua4_master.argtypes = []
    HAS_MASTER = True
except Exception:
    HAS_MASTER = False
MOT_STATE = os.environ.get("LUA54_MOT_STATE") == "1"
DUMP_G = os.environ.get("BOOT_DUMP_GLOBALS")      # tep ghi danh sach bien toan cuc moi state (kiem PA-2 local hoa)
'''
assert old1 in s; s = s.replace(old1, new1)
old2 = '''    L = D.lua4_open(100)
    D.lua4_baselibopen(L)
    D.lua4_dostring(L, STUB); D.lua4_settop(L, 0)
'''
new2 = '''    L = D.lua4_open(100)
    D.lua4_baselibopen(L)
    if MOT_STATE and HAS_MASTER:
        M = D.lua4_master()
        if i == 0:
            D.lua4_dostring(M, STUB); D.lua4_settop(M, 0)       # STUB (_ALERT, metatable rong, sandbox) tren bang chu
            G_BASE = set()
        D.lua4_dostring(M, b"L4_LOI = {}"); D.lua4_settop(M, 0)
        LX = M                                                  # doc L4_LOI tu bang chu
    else:
        D.lua4_dostring(L, STUB); D.lua4_settop(L, 0)
        LX = L
        if DUMP_G and i == 0:
            D.lua4_dostring(L, b"L4_G = {} for k in next, _G do L4_G[#L4_G + 1] = tostring(k) end L4_GS = table.concat(L4_G, ',')"); D.lua4_settop(L, 0)
            D.lua4_getglobal(L, b"L4_GS"); G_BASE = set((D.lua4_tostring(L, -1) or b"").decode("latin-1").split(",")); D.lua4_settop(L, 0)
'''
assert old2 in s; s = s.replace(old2, new2)
old3 = '''    # thu loi
    D.lua4_dostring(L, b"L4_N = #L4_LOI  L4_S = table.concat(L4_LOI, ' || ')"); D.lua4_settop(L, 0)
    D.lua4_getglobal(L, b"L4_N"); n = int(D.lua4_tonumber(L, -1)); D.lua4_settop(L, 0)
    if n > 0:
        n_err_states += 1
        D.lua4_getglobal(L, b"L4_S"); s = D.lua4_tostring(L, -1) or b""; D.lua4_settop(L, 0)
        loi_all.append("%s\\t%d\\t%s" % (rel, n, s[:300].decode("latin-1", "replace")))
'''
new3 = '''    # thu loi
    D.lua4_dostring(LX, b"L4_N = #L4_LOI  L4_S = table.concat(L4_LOI, ' || ')"); D.lua4_settop(LX, 0)
    D.lua4_getglobal(LX, b"L4_N"); n = int(D.lua4_tonumber(LX, -1)); D.lua4_settop(LX, 0)
    if n > 0:
        n_err_states += 1
        D.lua4_getglobal(LX, b"L4_S"); s = D.lua4_tostring(LX, -1) or b""; D.lua4_settop(LX, 0)
        loi_all.append("%s\\t%d\\t%s" % (rel, n, s[:300].decode("latin-1", "replace")))
    if DUMP_G:
        D.lua4_dostring(L, b"L4_G = {} for k in next, _G do L4_G[#L4_G + 1] = tostring(k) end L4_GS = table.concat(L4_G, ',')"); D.lua4_settop(L, 0)
        D.lua4_getglobal(L, b"L4_GS"); gs = (D.lua4_tostring(L, -1) or b"").decode("latin-1", "replace"); D.lua4_settop(L, 0)
        names = sorted(set(gs.split(",")) - G_BASE - {"L4_G", "L4_GS", "L4_N", "L4_S", "L4_LOI", "_G", "MODEL_GAMESERVER", "Include"})
        dump_g.append("%s\\t%s" % (rel, ",".join(names)))
'''
assert old3 in s; s = s.replace(old3, new3)
old4 = 'n_err_states = 0\nloi_all = []\n'
new4 = 'n_err_states = 0\nloi_all = []\ndump_g = []\nG_BASE = set()\n'
assert old4 in s; s = s.replace(old4, new4)
old5 = 'if LOG:\n    open(LOG, "w", encoding="utf-8").write("\\n".join(sorted(loi_all)) + "\\n")\n    print("ghi loi ->", LOG)\n'
new5 = old5 + 'if DUMP_G:\n    open(DUMP_G, "w", encoding="utf-8").write("\\n".join(dump_g) + "\\n")\n    print("ghi bien toan cuc ->", DUMP_G)\n'
assert old5 in s; s = s.replace(old5, new5)
io.open(P, "w", encoding="utf-8", newline="\n").write(s)
print("boot_gia.py: ok")
