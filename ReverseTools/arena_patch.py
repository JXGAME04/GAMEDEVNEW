# -*- coding: utf-8 -*-
"""LOI DAI BANG HOI CN (21/08): va KJx2CityWar.h/.cpp + KTongJX2.cpp + ScriptFuns.cpp.
Chuoi TCVN3 lay nguyen byte tu stringtable_relay.txt (latin-1)."""
import io, os, re, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
SP = r"C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\51bb04de-d5c8-4a11-ab83-a6aaceecb2ed\scratchpad"
SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"
H = os.path.join(SRC, "KJx2CityWar.h"); C = os.path.join(SRC, "KJx2CityWar.cpp")
TJ = os.path.join(SRC, "KTongJX2.cpp"); SF = os.path.join(SRC, "ScriptFuns.cpp")
STR = r"D:\ServerLinux\gateway\s3relay\lang\vn\stringtable_relay.txt"
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hi(s): return sum(1 for c in s if ord(c) > 127)
def nlof(s): return "\r\n" if "\r\n" in s else "\n"
def rep1(s, old, new, nl="\n"):
    old = old.replace("\r\n", "\n").replace("\n", nl); new = new.replace("\r\n", "\n").replace("\n", nl)
    assert s.count(old) == 1, (s.count(old), old[:80])
    return s.replace(old, new, 1)

# ---------- 0. chuoi ----------
tbl = {}
for ln in rd(STR).split("\n"):
    ln = ln.rstrip("\r")
    if "\t" not in ln: continue
    k, v = ln.split("\t", 1)
    v = v.strip()
    if len(v) >= 2 and v[0] == '"' and v[-1] == '"': v = v[1:-1]
    tbl[k.strip()] = v
block = rd(os.path.join(SP, "arena_block.cpp"))
def fill(m):
    k = m.group(1)
    assert k in tbl, k
    v = tbl[k].replace('"', '\\"')
    return v
block, nsub = re.subn(r"@@([A-Z_0-9]+)@@", fill, block)
assert "@@" not in block
block = block.replace("for (int i = 1; i < MAX_PLAYER; i++)", "for (int i = 1; i <= PlayerSet.GetPlayerMaxNumber(); i++)")
print("chuoi dien:", nsub, "| byte cao trong block:", hi(block))

# ---------- 1. KJx2CityWar.h ----------
s = rd(H); nl = nlof(s); h0 = hi(s)
s = rep1(s, """// trang thai thanh (nhanh VN dung 3)
#define JX2CW_STATE_NORMAL		0	// binh thuong (co the vo chu / co chu)
#define JX2CW_STATE_WARDECIDED	1	// 19h da chot khieu chien gia - mai danh
#define JX2CW_STATE_ATWAR		2	// 20h dang danh (HaveBeginWar = 1)
""", """// trang thai thanh - [LOI DAI CN 21/08] dung DU 6 trang thai cua ban goc
// (R +0x05 / G +0x05; mirror jx2citywar.txt 'V 2'; mirror cu 1->4, 2->5)
#define JX2CW_STATE_NORMAL		0	// roi (co the vo chu / co chu)
#define JX2CW_STATE_SIGNUP		1	// 18h-19h dang mo bao danh loi dai
#define JX2CW_STATE_BRACKET		2	// 19h da chot >= 2 bang, co bang dau, cho StartArena
#define JX2CW_STATE_ARENA		3	// 20h loi dai dang dien ra
#define JX2CW_STATE_WARDECIDED	4	// da co khieu chien gia - mai 20h cong thanh
#define JX2CW_STATE_ATWAR		5	// dang cong thanh (HaveBeginWar = 1)
""", nl)
s = rep1(s, """// nhom ARENA (E4) - nhanh VN loi dai CHET (binary goc khong dang ky
// SignUpCityWarArena): dang ky DU de script nap/thoai khong loi, tra "chua co"
int LuaIsArenaBegin(Lua_State* L);        // (0..7) -> 0
int LuaGetArenaBothSides(Lua_State* L);   // (0..7) -> "",""
int LuaGetArenaCityArea(Lua_State* L);    // (nArenaID) -> 0
int LuaGetArenaLevel(Lua_State* L);       // () -> 0 (chua/xong)
int LuaGetArenaTargetCity(Lua_State* L);  // () -> 0
int LuaGetArenaTotalLevel(Lua_State* L);  // () -> 0
int LuaGetArenaTotalLevelByCity(Lua_State* L); // (1..7) -> 0
int LuaGetArenaSchedule(Lua_State* L);    // (1..7) -> ""
int LuaGetArenaInfoByCity(Lua_State* L);  // (1..7[,nLevel]) -> ""
int LuaNotifyArenaResult(Lua_State* L);   // (nArenaID, bSide1Win) -> 0 gia tri (nuot)
""", """// nhom ARENA - [LOI DAI CN 21/08] dung lai ban CN goc (dich nguoc relay + GS,
// DACTA_LOIDAI_BANGHOI_CN_WORKFLOW.md): bao danh dau thau -> bang dau loai truc
// tiep <= 16 bang -> vo dich = khieu chien gia. 4 pha = 4 API Lua trung ten relay.
int LuaStartSignUp(Lua_State* L);         // (1..7) -> 1/0 : 18h xoa du lieu cu, state 1
int LuaEndSignUp(Lua_State* L);           // (1..7) -> 1/0 : 19h 0 bang->0, 1->challenger, >=2->bracket
int LuaStartArena(Lua_State* L);          // (1..7) -> 1/0 : 20h state 3, timer 18 mo mission 9
int LuaStartCityWar(Lua_State* L);        // (1..7) -> 1/0 : 20h hom sau; vo chu -> chiem luon; co chu -> state 5
int LuaSignUpCityWarArena(Lua_State* L);  // (1..7, nFee) - bang chu bao danh (10 dieu kien), tru quy bang
int LuaIsArenaBegin(Lua_State* L);        // (0..7) -> 1 khi cap nArenaID cua thanh dang dau chua co ket qua
int LuaGetArenaBothSides(Lua_State* L);   // (0..7) -> ten A, ten B / "",""
int LuaGetArenaCityArea(Lua_State* L);    // (nArenaID) -> thanh dang dau loi dai / 0
int LuaGetArenaLevel(Lua_State* L);       // () -> vong dang cho (1..) ; 0 gia tri khi khong co
int LuaGetArenaTargetCity(Lua_State* L);  // () -> thanh dang dau / 0
int LuaGetArenaTotalLevel(Lua_State* L);  // () -> log2(size bracket) cua thanh dang dau
int LuaGetArenaTotalLevelByCity(Lua_State* L); // (1..7) -> log2(size bracket)
int LuaGetArenaSchedule(Lua_State* L);    // (1..7) -> lich vong hien tai (chuoi)
int LuaGetArenaInfoByCity(Lua_State* L);  // (1..7[,nLevel]) -> moi ket qua da co
int LuaNotifyArenaResult(Lua_State* L);   // (nArenaID, bSide1Win) -> 0 gia tri; het cay -> challenger
""", nl)
s = rep1(s, "int LuaIsSigningUp(Lua_State* L);         // (1..7) -> 1 khi league 508 task 1 == 1\n",
            "int LuaIsSigningUp(Lua_State* L);         // (1..7) -> 1 khi state == SIGNUP (truoc: league 508)\n", nl)
s = rep1(s, "int LuaNumOfSignUpTongs(Lua_State* L);    // (1..7) -> so bang da nop lenh (member league 508)\n",
            "int LuaNumOfSignUpTongs(Lua_State* L);    // (1..7) -> so bang bao danh loi dai (state 1..3)\n", nl)
s = rep1(s, "int LuaGetSignUpTongName(Lua_State* L);   // (1..7, nIndex 0-based) -> ten bang ; fail -> \"\"\n",
            "int LuaGetSignUpTongName(Lua_State* L);   // (1..7, nIndex 0-based) -> ten bang bao danh ; fail -> \"\"\n", nl)
s = rep1(s, "int LuaCTC_JX2_SetCityState(Lua_State* L);// (1..7, nState 0..2) - ha tang cho tick 5 pha (E6) -> 1/0\n",
            "int LuaCTC_JX2_SetCityState(Lua_State* L);// (1..7, nState 0..5) - admin/ep trang thai -> 1/0\n", nl)
assert hi(s) == h0
wr(H, s); print("KJx2CityWar.h ok")

# ---------- 2. KJx2CityWar.cpp ----------
s = rd(C); nl = nlof(s); h0 = hi(s)
# includes
s = rep1(s, '#include "KJx2CityWar.h"\n#include <string.h>\n',
            '#include "KJx2CityWar.h"\n#include "KProtocol.h"\n#include "KPlayerDef.h"\n#include "CoreUseNameDef.h"\n#include <string.h>\n', nl)
# forward decl cho persist
s = rep1(s, "static KJx2City\ts_Cities[JX2CW_CITY_MAX];\n",
            "static KJx2City\ts_Cities[JX2CW_CITY_MAX];\n// [LOI DAI CN 21/08] persist loi dai - than o khoi ARENA phia duoi\nstatic void sArenaLoadBegin();\nstatic void sArenaLoadLine(const char* szLine);\nstatic void sArenaLoadFinish();\nstatic void sArenaSaveLines(FILE* f);\n", nl)
# sLoadMirror: V, state 0..5 + doi mirror cu, dong arena
s = rep1(s, """	char szLine[512];
	while (fgets(szLine, sizeof(szLine), f))
	{
		sChopEol(szLine);
		if (szLine[0] && szLine[1] != ' ')
			continue;	// dong hong/cut - bo qua, khong parse "ma" byte cu trong dem
		if (szLine[0] == 'C')
		{
			int nId = 0, nState = 0, nTax = 0, nPP = 0, nDate = 0;
			if (sscanf(szLine + 2, "%d %d %d %d %d", &nId, &nState, &nTax, &nPP, &nDate) == 5 &&
				nId >= 1 && nId <= 7)
			{
				s_Cities[nId].nState = (nState >= 0 && nState <= 2) ? nState : JX2CW_STATE_NORMAL;
""", """	char szLine[512];
	int nVer = 1;		// [LOI DAI CN 21/08] 'V 2' = state 0..5; mirror cu (khong co V) 1->4, 2->5
	sArenaLoadBegin();
	while (fgets(szLine, sizeof(szLine), f))
	{
		sChopEol(szLine);
		if (szLine[0] && szLine[1] != ' ')
			continue;	// dong hong/cut - bo qua, khong parse "ma" byte cu trong dem
		if (szLine[0] == 'V')
		{
			sscanf(szLine + 2, "%d", &nVer);
		}
		else if (szLine[0] == 'G' || szLine[0] == 'A' || szLine[0] == 'F' || szLine[0] == 'B' || szLine[0] == 'R')
		{
			sArenaLoadLine(szLine);
		}
		else if (szLine[0] == 'C')
		{
			int nId = 0, nState = 0, nTax = 0, nPP = 0, nDate = 0;
			if (sscanf(szLine + 2, "%d %d %d %d %d", &nId, &nState, &nTax, &nPP, &nDate) == 5 &&
				nId >= 1 && nId <= 7)
			{
				if (nVer < 2)
					nState = (nState == 1) ? JX2CW_STATE_WARDECIDED : (nState == 2) ? JX2CW_STATE_ATWAR : JX2CW_STATE_NORMAL;
				s_Cities[nId].nState = (nState >= JX2CW_STATE_NORMAL && nState <= JX2CW_STATE_ATWAR) ? nState : JX2CW_STATE_NORMAL;
""", nl)
s = rep1(s, """				sStrCpy(szDst, szLine + 2 + nPos, sizeof(s_Cities[nId].szOwnerTong));
			}
		}
	}
	fclose(f);
}
""", """				sStrCpy(szDst, szLine + 2 + nPos, sizeof(s_Cities[nId].szOwnerTong));
			}
		}
	}
	fclose(f);
	sArenaLoadFinish();
}
""", nl)
# sSaveMirror: V 2 + arena
s = rep1(s, """	FILE* f = fopen(szTmp, "wb");
	if (!f)
		return;
	for (int i = 1; i <= 7; i++)
	{
		KJx2City* p = &s_Cities[i];
		fprintf(f, "C %d %d %d %d %d\\n", i, p->nState, p->nTax, p->nPriceParam, p->nOccupyDate);
""", """	FILE* f = fopen(szTmp, "wb");
	if (!f)
		return;
	fprintf(f, "V 2\\n");	// [LOI DAI CN 21/08] state 0..5 + cac dong G/A/F/B/R loi dai
	for (int i = 1; i <= 7; i++)
	{
		KJx2City* p = &s_Cities[i];
		fprintf(f, "C %d %d %d %d %d\\n", i, p->nState, p->nTax, p->nPriceParam, p->nOccupyDate);
""", nl)
s = rep1(s, """		if (p->szChallenger[0])
			fprintf(f, "H %d %s\\n", i, p->szChallenger);
	}
	// dia day/loi ghi""", """		if (p->szChallenger[0])
			fprintf(f, "H %d %s\\n", i, p->szChallenger);
	}
	sArenaSaveLines(f);
	// dia day/loi ghi""", nl)
# AppointViceroy / AppointChallenger: xoa du lieu loi dai cua thanh (R 0x080988C4)
s = rep1(s, """			p->szChallenger[0] = 0;
			p->nState = JX2CW_STATE_NORMAL;
			sOwnerChanged(n, szOldOwner, szOldMaster);
			sSaveMirror();
			sApplyCityToSubWorld(n);
			g_DebugLog((LPSTR)"KJx2CityWar: AppointViceroy thanh %d chu=[%s] thaithu=[%s]",""",
"""			p->szChallenger[0] = 0;
			p->nState = JX2CW_STATE_NORMAL;
			sArenaClear(n);		// [LOI DAI CN] bo nhiem tay = xoa bang dau/ket qua cua thanh
			sOwnerChanged(n, szOldOwner, szOldMaster);
			sSaveMirror();
			sApplyCityToSubWorld(n);
			g_DebugLog((LPSTR)"KJx2CityWar: AppointViceroy thanh %d chu=[%s] thaithu=[%s]",""", nl)
s = rep1(s, """			sStrCpy(p->szChallenger, szTong, sizeof(p->szChallenger));
			p->nState = JX2CW_STATE_WARDECIDED;
			sSaveMirror();
			g_DebugLog((LPSTR)"KJx2CityWar: AppointChallenger thanh %d khieuchien=[%s]",""",
"""			sStrCpy(p->szChallenger, szTong, sizeof(p->szChallenger));
			p->nState = JX2CW_STATE_WARDECIDED;
			sArenaClear(n);		// [LOI DAI CN] goc R 0x080988C4: xoa toan bo bao danh/bracket/ket qua thanh
			if (s_nArenaCity == n) { s_bArenaRunning = 0; s_nArenaCity = -1; }
			sSaveMirror();
			g_DebugLog((LPSTR)"KJx2CityWar: AppointChallenger thanh %d khieuchien=[%s]",""", nl)
# IsSigningUp / NumOfSignUpTongs / GetSignUpTongName -> nguon moi
s = rep1(s, """	int n = sArgCity(L, 1);
	int nRet = 0;
	if (n && KJx2League_GetLeagueTaskC(508, s_Cities[n].szAreaName, 1) == 1)
		nRet = 1;
	Lua_PushNumber(L, nRet);
	return 1;
}""", """	int n = sArgCity(L, 1);
	// [LOI DAI CN 21/08] nguon = state thanh (goc G 0x08115930), khong con league 508
	Lua_PushNumber(L, (n && s_Cities[n].nState == JX2CW_STATE_SIGNUP) ? 1 : 0);
	return 1;
}""", nl)
s = rep1(s, """	int n = sArgCity(L, 1);
	Lua_PushNumber(L, n ? KJx2League_GetMemberCountC(508, s_Cities[n].szAreaName) : 0);
	return 1;
}""", """	int n = sArgCity(L, 1);
	// [LOI DAI CN 21/08] goc G 0x080FDEF0: chi dem khi state 1..3
	int nRet = 0;
	if (n && s_Cities[n].nState >= JX2CW_STATE_SIGNUP && s_Cities[n].nState <= JX2CW_STATE_ARENA)
		nRet = (int)s_Arena[n].vecTong.size();
	Lua_PushNumber(L, nRet);
	return 1;
}""", nl)
s = rep1(s, """	const char* szName = "";
	if (n && Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2))
		szName = KJx2League_GetMemberNameC(508, s_Cities[n].szAreaName,
			(int)Lua_ValueToNumber(L, 2));
	Lua_PushString(L, (char*)szName);
	return 1;
}""", """	const char* szName = "";
	if (n && Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2))
		szName = sTongName(n, (int)Lua_ValueToNumber(L, 2));	// [LOI DAI CN 21/08] goc G 0x0812C690
	Lua_PushString(L, (char*)szName);
	return 1;
}""", nl)
# CTC_JX2_SetCityState 0..5
s = rep1(s, """// (1..7, nState 0..2) - ha tang cho tick 5 pha (E6: 20h flip ATWAR de timer 18
// poll HaveBeginWar mo mission); KHONG co trong ban goc (goc: relay StartCityWar
// gui goi rieng) - ten co tien to CTC_JX2_ de khong dung ten goc""",
"""// (1..7, nState 0..5) - [LOI DAI CN 21/08] chi con cho admin ep trang thai; lich
// chinh thuc dung StartSignUp/EndSignUp/StartArena/StartCityWar (ten goc relay)""", nl)
# xoa khoi stub ARENA cu (tu header "Nhom ARENA (E4)" den truoc LuaGetCityWarTongCamp)
i0 = s.find("//////////////////////////////////////////////////////////////////////" + nl + "// Nhom ARENA (E4) - idle nhu nhanh VN goc")
i1 = s.find("// (szTongName) -> 1 = dang THU thanh co tran, 2 = dang CONG, nil = khong thuoc")
assert i0 > 0 and i1 > i0, (i0, i1)
s = s[:i0] + s[i1:]
# chen khoi arena truoc "Nhom ham Lua"
anchor = "//////////////////////////////////////////////////////////////////////" + nl + "// Nhom ham Lua" + nl
assert s.count(anchor) == 1
blk = block.replace("\r\n", "\n").replace("\n", nl)
s = s.replace(anchor, blk + nl + anchor, 1)
# sArenaLoadBegin than (chen vao block: truoc sArenaLoadLine) - dung truc tiep
s = rep1(s, "static KJx2ArenaLoadTmp* s_pArenaLoad = NULL;\n",
            "static KJx2ArenaLoadTmp* s_pArenaLoad = NULL;\nstatic void sArenaLoadBegin()\n{\n\tif (s_pArenaLoad)\n\t\tdelete s_pArenaLoad;\n\ts_pArenaLoad = new KJx2ArenaLoadTmp;\n\ts_pArenaLoad->bRunning = 0; s_pArenaLoad->nCity = -1; s_pArenaLoad->bHasG = false;\n\tfor (int c = 0; c < JX2CW_CITY_MAX; c++)\n\t\tsArenaClear(c);\n}\n", nl)
assert hi(s) == h0 + hi(block), (hi(s), h0, hi(block))
wr(C, s); print("KJx2CityWar.cpp ok")

# ---------- 3. KTongJX2.cpp: 3 helper C ----------
s = rd(TJ); nl = nlof(s); h0 = hi(s)
s = rep1(s, """DWORD KTongJX2_GetFieldC(DWORD dwTongID, WORD wKey)
{
	return g_TongJX2.GetField(dwTongID, wKey);
}
""", """DWORD KTongJX2_GetFieldC(DWORD dwTongID, WORD wKey)
{
	return g_TongJX2.GetField(dwTongID, wKey);
}

// [LOI DAI CN 21/08] cho KJx2CityWar: field 11 = WarState (Tong::CityWarState goc:
// 0 thuong / 2 dang dau loi dai / 3 khieu chien gia / 4 dang cong thanh)
void KTongJX2_SetFieldC(DWORD dwTongID, WORD wKey, DWORD dwVal)
{
	if (!dwTongID || !g_TongJX2.FindTong(dwTongID))
		return;
	sSendFieldCmd(dwTongID, wKey, dwVal, defTONG_JX2_OP_SET, 0);
}

// quy bang (field 4 cao + 3 thap) - cung cong thuc TONG_GetMoney
__int64 KTongJX2_GetMoneyC(DWORD dwTongID)
{
	if (!dwTongID || !g_TongJX2.FindTong(dwTongID))
		return 0;
	return ((__int64)g_TongJX2.GetField(dwTongID, 4) << 32) | (__int64)g_TongJX2.GetField(dwTongID, 3);
}

// cong/tru quy bang qua relay (am duoc; relay chan duoi 0) - phi dau thau loi dai
void KTongJX2_AddMoneyC(DWORD dwTongID, __int64 nDelta)
{
	if (!dwTongID || !g_TongJX2.FindTong(dwTongID) || nDelta == 0)
		return;
	sSendMoneyCmd(dwTongID, nDelta, defTONG_JX2_OP_ADD, 0);
}
""", nl)
assert hi(s) == h0
wr(TJ, s); print("KTongJX2.cpp ok")

# ---------- 4. ScriptFuns.cpp: extern + dang ky 5 ham moi ----------
s = rd(SF); nl = nlof(s); h0 = hi(s)
s = rep1(s, "extern int LuaNotifyArenaResult(Lua_State* L);\n",
            "extern int LuaNotifyArenaResult(Lua_State* L);\n// [LOI DAI CN 21/08] 4 pha + bao danh (ten trung API relay goc)\nextern int LuaStartSignUp(Lua_State* L);\nextern int LuaEndSignUp(Lua_State* L);\nextern int LuaStartArena(Lua_State* L);\nextern int LuaStartCityWar(Lua_State* L);\nextern int LuaSignUpCityWarArena(Lua_State* L);\n", nl)
s = rep1(s, '\t\t{ "NotifyArenaResult",\tLuaNotifyArenaResult },\n',
            '\t\t{ "NotifyArenaResult",\tLuaNotifyArenaResult },\n\t\t// [LOI DAI CN 21/08]\n\t\t{ "StartSignUp",\tLuaStartSignUp },\n\t\t{ "EndSignUp",\tLuaEndSignUp },\n\t\t{ "StartArena",\tLuaStartArena },\n\t\t{ "StartCityWar",\tLuaStartCityWar },\n\t\t{ "SignUpCityWarArena",\tLuaSignUpCityWarArena },\n', nl)
assert hi(s) == h0
wr(SF, s); print("ScriptFuns.cpp ok")
