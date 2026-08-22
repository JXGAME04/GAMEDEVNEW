# -*- coding: utf-8 -*-
r"""(22/08) S1: timer mission goi "OnMissionTimer" -> fallback "OnTimer" (script Linux: citywar_arena, citywar_city,
missions\tong schedule, tongwar). S3: FormatTime2Number = YYMMDDHHMM nhu Linux 0x081023B0 (dang YYYYMMDD -> danh hieu
30 ngay khong het han, 6 caller Tong Kim/Lien Dau/Thien Cong dang sai)."""
import io, sys
SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hi(s): return sum(1 for c in s if ord(c) > 127)
def rep1(s, old, new):
    o = old.replace("\r\n", "\n"); n = new.replace("\r\n", "\n")
    for cand in ("\r\n", "\n"):
        o2 = o.replace("\n", cand)
        if s.count(o2) == 1:
            return s.replace(o2, n.replace("\n", cand), 1)
    assert False, old[:80]

p = SRC + r"\KMission.cpp"; s = rd(p); h = hi(s)
s = rep1(s, """int g_MissionTimerCallBackFun(void * pOwner, char * szScriptFile)
{
	if (!pOwner) return FALSE;
	KMission *pMission = (KMission*)pOwner;
	pMission->ExecuteScript(szScriptFile, "OnMissionTimer", 0);
""", """int g_MissionTimerCallBackFun(void * pOwner, char * szScriptFile)
{
	if (!pOwner) return FALSE;
	KMission *pMission = (KMission*)pOwner;
	// [22/08 S1] mission cua du an dat "OnMissionTimer"; script Linux/JX2 (citywar_arena,
	// citywar_city, missions\\tong schedule, tongwar) dat "OnTimer". Do ham co that trong
	// state dich roi goi dung ten (khuon LuaInitMission ScriptFuns.cpp "InitMission"/"BeginMission").
	char szFun[20];
	strcpy(szFun, "OnMissionTimer");
	if (szScriptFile && szScriptFile[0])
	{
		char szLow[MAX_PATH];
		g_StrCpyLen(szLow, szScriptFile, MAX_PATH);
		g_StrLower(szLow);
		KLuaScript* pMs = (KLuaScript*)g_GetScript(szLow);
		if (pMs && pMs->m_LuaState)
		{
			int nTop = Lua_GetTopIndex(pMs->m_LuaState);
			Lua_GetGlobal(pMs->m_LuaState, "OnMissionTimer");
			if (!lua_isfunction(pMs->m_LuaState, Lua_GetTopIndex(pMs->m_LuaState)))
				strcpy(szFun, "OnTimer");
			lua_settop(pMs->m_LuaState, nTop);
		}
	}
	pMission->ExecuteScript(szScriptFile, szFun, 0);
""")
assert hi(s) == h; wr(p, s); print("KMission.cpp S1 ok")

p = SRC + r"\KTongJX2.cpp"; s = rd(p); h = hi(s)
s = rep1(s, """// FormatTime2Number(epoch) -> so YYYYMMDD
int LuaJX2_FormatTime2Number(Lua_State* L)
{
	time_t tVal = Lua_IsNumber(L, 1) ? (time_t)Lua_ValueToNumber(L, 1) : time(NULL);
	struct tm* pTm = localtime(&tVal);
	int nNum = 0;
	if (pTm)
		nNum = (pTm->tm_year + 1900) * 10000 + (pTm->tm_mon + 1) * 100 + pTm->tm_mday;
	Lua_PushNumber(L, nNum);
	return 1;
}""", """// FormatTime2Number(epoch) -> so YYMMDDHHMM. [22/08 S3] Linux 0x081023B0 sprintf
// "%02d%02d%02d%02d%02d" (yy,mm,dd,HH,MM); truoc day tra YYYYMMDD -> mod(...,1e8) cua
// head.lua (Title_AddTitle MMDDHHMM) sai, danh hieu 30 ngay khong het han; 6 caller
// songjin_shophead/leaguematch officer/ws_tiangong/seasonnpc deu giai ma theo Linux.
int LuaJX2_FormatTime2Number(Lua_State* L)
{
	time_t tVal = Lua_IsNumber(L, 1) ? (time_t)Lua_ValueToNumber(L, 1) : time(NULL);
	struct tm* pTm = localtime(&tVal);
	double dNum = 0;
	if (pTm)
		dNum = (double)((pTm->tm_year + 1900) % 100) * 100000000.0 + (double)(pTm->tm_mon + 1) * 1000000.0
			+ (double)pTm->tm_mday * 10000.0 + (double)pTm->tm_hour * 100.0 + (double)pTm->tm_min;
	Lua_PushNumber(L, dNum);
	return 1;
}""")
assert hi(s) == h; wr(p, s); print("KTongJX2.cpp S3 ok")
