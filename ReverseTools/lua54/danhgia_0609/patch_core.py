import sys
WT = sys.argv[1].rstrip("/").rstrip(chr(92))
H = WT + "/Sources/Core/Src/KPerfTick.h"
CPP = WT + "/Sources/Core/Src/KPerfTick.cpp"
KS = WT + "/Sources/Core/Src/KSortScript.cpp"

def rd(p):
    return open(p, "rb").read().decode("latin-1")
def wr(p, s):
    open(p, "wb").write(s.encode("latin-1"))
def eol(t):
    return "\r\n" if t.count("\r\n") > t.count("\n") // 2 else "\n"
def fix(s, e):
    return s.replace("\r\n", "\n").replace("\n", e)
def once(txt, anchor, what):
    n = txt.count(anchor)
    if n != 1:
        raise SystemExit("ANCHOR %s: found %d times" % (what, n))
def hi(t):
    return sum(1 for ch in t if ord(ch) > 127)

# ---------------------------------------------------------------- KPerfTick.h
t = rd(H); e = eol(t)
a = fix("\tPERF_GS_MAINLOOP,		// (GameServer.exe) KSwordOnLineSever::MainLoop\n\tPERF_COUNT\n", e)
once(t, a, "enum")
t = t.replace(a, fix("\tPERF_GS_MAINLOOP,		// (GameServer.exe) KSwordOnLineSever::MainLoop\n"
                     "\tPERF_LUA_CALL,			// [LUA54 06/09] tong thoi gian chay Lua trong tick (lua4_call do sau 0, doc tu Lua54Dll)\n"
                     "\tPERF_COUNT\n", e))
wr(H, t); print("KPerfTick.h OK")

# ---------------------------------------------------------------- KPerfTick.cpp
t = rd(CPP); e = eol(t)
a = fix('\t"GS_MSGLOOP",\n\t"GS_MAINLOOP",\n};\n', e)
once(t, a, "stage names")
t = t.replace(a, fix('\t"GS_MSGLOOP",\n\t"GS_MAINLOOP",\n\t"LUA_CALL",\n};\n', e))
a = fix("//---------------------------------------------------------------------------\nvoid g_PerfFrame(int nOnlinePlayer)\n{\n", e)
once(t, a, "g_PerfFrame head")
FN = fix('''//---------------------------------------------------------------------------
// [LUA54 06/09] Thoi gian chay Lua cua tick vua xong: Lua54Dll cong don trong lua4_call
// (do sau 0, ke ca Include/dofile), Core doc + xoa moi tick -> giai doan LUA_CALL.
// Lay ham qua GetProcAddress de CoreServer van chay voi Lua54Dll cu (khong co ham -> bo qua).
//---------------------------------------------------------------------------
static void PerfLuaDoc()
{
	typedef int (*PFN_L4READ)(double*, double*, int*);
	typedef void (*PFN_L4SET)(int);
	static PFN_L4READ s_pfnRead = NULL;
	static int s_nDaThu = 0;
	if (!s_nDaThu)
	{
		s_nDaThu = 1;
		HMODULE hLua = GetModuleHandleA("Lua54Dll.dll");
		if (hLua)
		{
			s_pfnRead = (PFN_L4READ)GetProcAddress(hLua, "lua4_perf_read");
			PFN_L4SET pfnSet = (PFN_L4SET)GetProcAddress(hLua, "lua4_perf_set");
			if (s_pfnRead && pfnSet)
				pfnSet(1);
			else
				s_pfnRead = NULL;
		}
	}
	if (s_pfnRead)
	{
		double dTong = 0.0, dMax = 0.0;
		int n = 0;
		if (s_pfnRead(&dTong, &dMax, &n))
			g_PerfAdd(PERF_LUA_CALL, dTong);
	}
}

''', e)
t = t.replace(a, FN + a)
a = fix("\ts_nFrame++;\n\tif (s_dLastTick > s_dTreMs)\n", e)
once(t, a, "s_nFrame++")
t = t.replace(a, fix("\ts_nFrame++;\n\tPerfLuaDoc();\n\tif (s_dLastTick > s_dTreMs)\n", e))
wr(CPP, t); print("KPerfTick.cpp OK")

# ---------------------------------------------------------------- KSortScript.cpp (TCVN3 - byte-safe latin-1, chi chen ASCII)
t = rd(KS); e = eol(t); h0 = hi(t)
a = fix("unsigned long g_IniScriptEngine()\n{\n\tg_szCurScriptDir[0] = 0;\n\tnCurrentScriptNum = 0;\n\tg_ScriptBinTree.ClearList();\n", e)
once(t, a, "g_IniScriptEngine head")
HELPER = fix('''// [LUA54 06/09] Thong ke nap script + cache Include cua Lua54Dll (PHANTICH_NANG_LUA54_0509.md muc 13).
// Lay ham qua GetProcAddress de Core van chay voi Lua54Dll cu (chua co ham) - khi do chi in thoi gian.
static void sGhiThongKeNapScript(unsigned long nLoaded, DWORD dwMs)
{
	typedef void (*PFN_INCSTATS)(long long*, long long*, long long*, long long*, long long*);
	long long nStateHit = 0, nGlobalHit = 0, nCompile = 0, nBytesSaved = 0, nBytesCode = 0;
	HMODULE hLua = GetModuleHandleA("Lua54Dll.dll");
	PFN_INCSTATS pfn = hLua ? (PFN_INCSTATS)GetProcAddress(hLua, "lua4_inc_stats") : NULL;
	if (pfn)
		pfn(&nStateHit, &nGlobalHit, &nCompile, &nBytesSaved, &nBytesCode);
	char szMsg[512];
	sprintf_s(szMsg, sizeof(szMsg),
		"[script] LoadAllScript: %lu tep, %lu ms; cache Include: bien dich %lld, dung lai cung state %lld, bytecode chung %lld, bo qua phan tich %lld KB, bytecode giu %lld KB",
		nLoaded, dwMs, nCompile, nStateHit, nGlobalHit, nBytesSaved / 1024, nBytesCode / 1024);
	printf("%s\\n", szMsg);
	g_DebugLog((LPSTR)"%s", szMsg);
}

''', e)
t = t.replace(a, HELPER + a + fix("\tDWORD dwT0 = GetTickCount();		// [LUA54 06/09] do thoi gian nap toan bo script\n", e))
a = fix('\tnLoaded = LoadAllScript("\\\\scriptjx2\\\\tong_vn");\n#endif\n\treturn nLoaded;\n}\n', e)
once(t, a, "g_IniScriptEngine tail")
t = t.replace(a, fix('\tnLoaded = LoadAllScript("\\\\scriptjx2\\\\tong_vn");\n#endif\n\tsGhiThongKeNapScript(nLoaded, GetTickCount() - dwT0);\n\treturn nLoaded;\n}\n', e))
assert hi(t) == h0, "high-byte count changed!"
wr(KS, t); print("KSortScript.cpp OK, high bytes unchanged:", h0)
