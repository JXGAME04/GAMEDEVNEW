# -*- coding: utf-8 -*-
r"""[JX2COMPAT 22/08] Lop tuong thich JX2 cua engine sai ngu nghia (SPEC_PORT_BW muc C, SPEC_PORT_TONGWAR muc C.2):
- g_GetScriptNameByState(L) / g_IsJx2Script(L) (KSortScript.cpp) - nhan biet script goc JX2 theo duong dan
- SetPunish: JX2 -> dao nghia (Linux 0x0810F470: 0 = khong phat)
- GetTeamMember: JX2 -> 1 = doi truong, n>=2 = thanh vien hop le thu n-1 (Linux 0x08115530)
- AddSkillState: JX2 -> luon ap skill that (p3 Linux = kieu thoi gian); guard NULL chung
- GetGameTime: JX2 -> giay (frame/18) (Linux 0x0810F3A0 tra giay)
- SetPKFlag: that = m_cPK.SetNormalPKState(n, giu khoa) (Linux 0x0810F610); ForbidChangePK = khoa PK (0x0810F590)
- AskClientForNumber(cb,min,max,prompt): that, dung hop so KUiGetNumber (S2C_INPUT_BOX type 2), cb(so)
- Death script: khong co OnPlayerDeath -> goi OnDeath(Launcher) (Linux)
"""
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
    assert False, old[:90]

# ---------- KSortScript.h/.cpp ----------
p = SRC + r"\KSortScript.h"; s = rd(p); h = hi(s)
s = rep1(s, "extern const KScript * g_GetScript(const char * szRelativeScriptFile);\n",
            "extern const KScript * g_GetScript(const char * szRelativeScriptFile);\n// [JX2COMPAT 22/08] ten script (duong dan tuong doi, chu thuong) theo lua_State; NULL neu khong thay\nextern const char * g_GetScriptNameByState(Lua_State* L);\n// [JX2COMPAT 22/08] 1 khi state thuoc script port nguyen ban tu Linux/JX2 (ngu nghia ham khac JX1)\nextern int g_IsJx2Script(Lua_State* L);\n")
assert hi(s) == h; wr(p, s); print("KSortScript.h ok")

p = SRC + r"\KSortScript.cpp"; s = rd(p); h = hi(s)
s = rep1(s, """const KScript * g_GetScript(const char * szRelativeScriptFile)
{
	DWORD dwScriptId = g_FileName2Id((LPSTR)szRelativeScriptFile);
	return g_GetScript(dwScriptId);
}
""", """const KScript * g_GetScript(const char * szRelativeScriptFile)
{
	DWORD dwScriptId = g_FileName2Id((LPSTR)szRelativeScriptFile);
	return g_GetScript(dwScriptId);
}

// [JX2COMPAT 22/08] g_ScriptSet[i].m_szScriptName = duong dan tuong doi chu thuong
// ("\\script\\missions\\bw\\bwhead.lua", LoadScriptToSortList). Quet tuyen tinh lan dau,
// nho lai theo state (script chi nap luc boot; ReLoadScript giu nguyen state).
#include <map>
const char * g_GetScriptNameByState(Lua_State* L)
{
	static std::map<Lua_State*, int> s_mapState;
	if (!L)
		return NULL;
	std::map<Lua_State*, int>::iterator it = s_mapState.find(L);
	if (it != s_mapState.end() && it->second >= 0 && it->second < (int)nCurrentScriptNum
		&& g_ScriptSet[it->second].m_LuaState == L)
		return g_ScriptSet[it->second].m_szScriptName;
	for (unsigned int i = 0; i < nCurrentScriptNum && i < MAX_SCRIPT_IN_SET; i++)
	{
		if (g_ScriptSet[i].m_LuaState == L)
		{
			s_mapState[L] = (int)i;
			return g_ScriptSet[i].m_szScriptName;
		}
	}
	return NULL;
}

// Cac cay script chep nguyen ban tu Linux (JX2): SetPunish/GetTeamMember/AddSkillState/
// GetGameTime co ngu nghia KHAC JX1 - ham Lua re nhanh theo co nay. Include() dung
// state cua tep goi nen NPC JX1 goi luong JX2 (dichquan.lua -> posthouse.lua) phai liet ke.
int g_IsJx2Script(Lua_State* L)
{
	static const char* szJx2[] = {
		"\\\\script\\\\missions\\\\citywar_", "\\\\script\\\\missions\\\\leaguematch\\\\", "\\\\script\\\\leaguematch\\\\",
		"\\\\script\\\\missions\\\\tong\\\\", "\\\\script\\\\task\\\\tollgate\\\\", "\\\\script\\\\item\\\\messenger\\\\",
		"\\\\script\\\\item\\\\xinshirenwu\\\\", "\\\\script\\\\missions\\\\tongwar\\\\", "\\\\script\\\\event\\\\tongwar\\\\",
		"\\\\script\\\\missions\\\\bw\\\\", "\\\\script\\\\missions\\\\bairenleitai\\\\", "\\\\script\\\\missions\\\\tongcastle\\\\",
		"\\\\script\\\\missions\\\\arena\\\\", "\\\\script\\\\activitysys\\\\", "\\\\script\\\\tong\\\\", "\\\\scriptjx2\\\\",
		"\\\\script\\\\global\\\\npcchucnang\\\\dichquan.lua",	// Dich Quan 7 thanh: Include posthouse.lua (Tin Su)
	};
	const char* szName = g_GetScriptNameByState(L);
	if (!szName || !szName[0])
		return 0;
	for (int i = 0; i < (int)(sizeof(szJx2) / sizeof(szJx2[0])); i++)
	{
		if (strstr(szName, szJx2[i]) != NULL)
			return 1;
	}
	return 0;
}
""")
assert hi(s) == h; wr(p, s); print("KSortScript.cpp ok")

# ---------- ScriptFuns.cpp ----------
p = SRC + r"\ScriptFuns.cpp"; s = rd(p); h = hi(s)
# SetPunish
s = rep1(s, """	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nState = Lua_ValueToNumber(L, 1);
		if (nState == 0)
			Npc[Player[nPlayerIndex].m_nIndex].m_nCurPKPunishState = 0;
		else
			Npc[Player[nPlayerIndex].m_nIndex].m_nCurPKPunishState = enumDEATH_MODE_PKBATTLE_PUNISH;
	}
	return 0;
}""", """	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nState = Lua_ValueToNumber(L, 1);
		// [JX2COMPAT 22/08] Linux 0x0810F470: SetPunish(0) = KHONG phat, (1) = phat - NGUOC voi JX1.
		// Script JX2 (citywar/tong/messenger/tongwar/bw) goi SetPunish(0) luc vao tran, (1) luc ra.
		if (g_IsJx2Script(L))
			nState = (nState == 0) ? 1 : 0;
		if (nState == 0)
			Npc[Player[nPlayerIndex].m_nIndex].m_nCurPKPunishState = 0;
		else
			Npc[Player[nPlayerIndex].m_nIndex].m_nCurPKPunishState = enumDEATH_MODE_PKBATTLE_PUNISH;
	}
	return 0;
}""")
# GetTeamMember
s = rep1(s, """int LuaGetTeamMem(Lua_State* L)
{
	int nTeamId = -1;
	int nPos = -1;
	int nMemberId = 0;

	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum >= 2)
	{
		nTeamId = Lua_ValueToNumber(L, 1);
		nPos = (int)Lua_ValueToNumber(L, 2);
		if (nPos > 0 && nPos <= MAX_TEAM_MEMBER)
			nMemberId = g_Team[nTeamId].m_nMember[nPos - 1];
		else if (nPos == 0)
			nMemberId = g_Team[nTeamId].m_nCaptain;
	}
	else
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (Player[nPlayerIndex].m_cTeam.m_nFlag)
		{
			nTeamId = Player[nPlayerIndex].m_cTeam.m_nID;
			nPos = (int)Lua_ValueToNumber(L, 1);
			if (nPos > 0 && nPos <= MAX_TEAM_MEMBER)
				nMemberId = g_Team[nTeamId].m_nMember[nPos - 1];
			else if (nPos == 0)
				nMemberId = g_Team[nTeamId].m_nCaptain;
		}
	}
""", """// [JX2COMPAT 22/08] Linux 0x08115530: n == 1 -> doi truong; n >= 2 -> thanh vien HOP LE thu n-1
// (bo o -1). Script JX2 lap "for i = 1, GetTeamSize() do GetTeamMember(i)" (lib_messenger,
// tongwar head, bw) - theo quy uoc JX1 se bo sot doi truong va doc o trong.
static int sJx2TeamMember(int nTeamId, int nPos)
{
	if (nTeamId < 0 || nPos < 1)
		return 0;
	if (nPos == 1)
		return g_Team[nTeamId].m_nCaptain;
	int nSeen = 1;
	for (int i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		int nM = g_Team[nTeamId].m_nMember[i];
		if (nM > 0)
		{
			nSeen++;
			if (nSeen == nPos)
				return nM;
		}
	}
	return 0;
}

int LuaGetTeamMem(Lua_State* L)
{
	int nTeamId = -1;
	int nPos = -1;
	int nMemberId = 0;
	int bJx2 = g_IsJx2Script(L);

	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum >= 2)
	{
		nTeamId = Lua_ValueToNumber(L, 1);
		nPos = (int)Lua_ValueToNumber(L, 2);
		if (bJx2)
			nMemberId = sJx2TeamMember(nTeamId, nPos);
		else if (nPos > 0 && nPos <= MAX_TEAM_MEMBER)
			nMemberId = g_Team[nTeamId].m_nMember[nPos - 1];
		else if (nPos == 0)
			nMemberId = g_Team[nTeamId].m_nCaptain;
	}
	else
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (Player[nPlayerIndex].m_cTeam.m_nFlag)
		{
			nTeamId = Player[nPlayerIndex].m_cTeam.m_nID;
			nPos = (int)Lua_ValueToNumber(L, 1);
			if (bJx2)
				nMemberId = sJx2TeamMember(nTeamId, nPos);
			else if (nPos > 0 && nPos <= MAX_TEAM_MEMBER)
				nMemberId = g_Team[nTeamId].m_nMember[nPos - 1];
			else if (nPos == 0)
				nMemberId = g_Team[nTeamId].m_nCaptain;
		}
	}
""")
# AddSkillState
s = rep1(s, """	if (nTime <= 0)
		nTime = -1;
	//
	if (nIfMagic)//kh""", """	if (nTime <= 0)
		nTime = -1;
	// [JX2COMPAT 22/08] Linux 0x08125D70: tham so 3 = kieu thoi gian (0/1/2), LUON ap skill that
	// tu g_SkillManager -> script JX2 goi AddSkillState(id, lv, 0, t) van phai co thuoc tinh.
	if (g_IsJx2Script(L))
		nIfMagic = 1;
	if (nIfMagic && g_SkillManager.GetSkill(nSkillId, nSkillLevel) == NULL)
	{
		g_DebugLog((LPSTR)"AddSkillState: skill %d level %d khong ton tai - bo qua", nSkillId, nSkillLevel);
		return 0;
	}
	//
	if (nIfMagic)//kh""")
# GetGameTime
s = rep1(s, """int LuaGetGameTime(Lua_State* L)
{
	Lua_PushNumber(L, g_SubWorldSet.GetGameTime());
	return 1;
}""", """int LuaGetGameTime(Lua_State* L)
{
	// [JX2COMPAT 22/08] Linux 0x0810F3A0 tra GIAY; JX1 tra frame (18/s). Script JX2 so hieu
	// voi hang so giay (TIME_PLAYER_STAY 120 tongwar, bw) -> doi ra giay cho script JX2.
	if (g_IsJx2Script(L))
		Lua_PushNumber(L, (double)(g_SubWorldSet.GetGameTime() / 18));
	else
		Lua_PushNumber(L, g_SubWorldSet.GetGameTime());
	return 1;
}""")
assert hi(s) == h; wr(p, s); print("ScriptFuns.cpp ok")

# ---------- KJx2WarInfra.cpp: SetPKFlag / ForbidChangePK ----------
p = SRC + r"\KJx2WarInfra.cpp"; s = rd(p); h = hi(s)
s = rep1(s, """int LuaSetPKFlag(Lua_State* L)
{
	return LuaSetFightState(L);
}
""", """int LuaSetPKFlag(Lua_State* L)
{
	// [JX2COMPAT 22/08] Linux 0x0810F610 -> KPlayerPK::SetNormalPKState(n, giu khoa): doi co PK
	// thuong (0/1), KHONG dung fight-state (ban cu map sang SetFightState -> PK mode khong doi,
	// quan he none, danh khong len damage trong loi dai). Script JX2 van goi SetFightState rieng.
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Lua_IsNumber(L, 1))
	{
		BYTE btFlag = ((int)Lua_ValueToNumber(L, 1) != 0) ? 1 : 0;
		Player[nPlayerIndex].m_cPK.SetNormalPKState(btFlag, Player[nPlayerIndex].m_cPK.GetLockPKState());
	}
	return 0;
}
""")
s = rep1(s, """int LuaForbidChangePK(Lua_State* L)
{
	return 0;
}
""", """int LuaForbidChangePK(Lua_State* L)
{
	// [JX2COMPAT 22/08] Linux 0x0810F590: m_bLockPK = (n == 1) - cam nguoi choi tu doi co PK
	// (client ton trong khoa; server chan them o c2sPKApplyNormalFlag).
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Lua_IsNumber(L, 1))
	{
		BOOL bLock = ((int)Lua_ValueToNumber(L, 1) == 1) ? TRUE : FALSE;
		Player[nPlayerIndex].m_cPK.SetNormalPKState((BYTE)Player[nPlayerIndex].m_cPK.GetNormalPKState(), bLock);
	}
	return 0;
}
""")
assert hi(s) == h; wr(p, s); print("KJx2WarInfra.cpp ok")

# ---------- KProtocolProcess.cpp: khoa PK + AskClientForNumber callback ----------
p = SRC + r"\KProtocolProcess.cpp"; s = rd(p); h = hi(s)
s = rep1(s, """	PK_APPLY_NORMAL_FLAG_COMMAND	*pApply = (PK_APPLY_NORMAL_FLAG_COMMAND*)pProtocol;
	Player[nIndex].m_cPK.SetNormalPKState(pApply->m_btFlag, pApply->m_bLockPK);
}""", """	PK_APPLY_NORMAL_FLAG_COMMAND	*pApply = (PK_APPLY_NORMAL_FLAG_COMMAND*)pProtocol;
	// [JX2COMPAT 22/08] ForbidChangePK(1) dang khoa -> khong cho client tu doi (Linux cung chan)
	if (Player[nIndex].m_cPK.GetLockPKState() && !pApply->m_bLockPK)
		return;
	Player[nIndex].m_cPK.SetNormalPKState(pApply->m_btFlag, pApply->m_bLockPK);
}""")
s = rep1(s, """	case 2:
		{
			Player[nIndex].m_nStringNum = pInput->nNum;
			Player[nIndex].ExecuteScript(Player[nIndex].m_dwNumberBoxId, Player[nIndex].m_szTaskExcuteFun,"");
		}
		break;""", """	case 2:
		{
			Player[nIndex].m_nStringNum = pInput->nNum;
			// [JX2COMPAT 22/08] AskClientForNumber(cb, min, max, prompt): cb nhan SO lam doi so 1,
			// kep vao [min, max] (client chi kiem >= 0)
			if (Player[nIndex].m_bWllsAskStrArg == 2)
			{
				Player[nIndex].m_bWllsAskStrArg = 0;
				int nNum = pInput->nNum;
				if (nNum < Player[nIndex].m_nJx2AskMin) nNum = Player[nIndex].m_nJx2AskMin;
				if (nNum > Player[nIndex].m_nJx2AskMax) nNum = Player[nIndex].m_nJx2AskMax;
				Player[nIndex].ExecuteScript(Player[nIndex].m_dwNumberBoxId, Player[nIndex].m_szTaskExcuteFun, nNum);
			}
			else
			Player[nIndex].ExecuteScript(Player[nIndex].m_dwNumberBoxId, Player[nIndex].m_szTaskExcuteFun,"");
		}
		break;""")
assert hi(s) == h; wr(p, s); print("KProtocolProcess.cpp ok")

# ---------- KPlayer.h: 2 truong min/max ----------
p = SRC + r"\KPlayer.h"; s = rd(p); h = hi(s)
s = rep1(s, "\tBYTE\t\t\tm_bWllsAskStrArg;\t\t// AskClientForString: cb nhan chuoi nhap lam doi so 1\n",
            "\tBYTE\t\t\tm_bWllsAskStrArg;\t\t// AskClientForString: cb nhan chuoi nhap lam doi so 1 (2 = AskClientForNumber)\n\tint\t\t\t\tm_nJx2AskMin;\t\t\t// [JX2COMPAT 22/08] AskClientForNumber: khoang hop le\n\tint\t\t\t\tm_nJx2AskMax;\n")
assert hi(s) == h; wr(p, s); print("KPlayer.h ok")

# ---------- KTongJX2.cpp: AskClientForNumber that ----------
p = SRC + r"\KTongJX2.cpp"; s = rd(p); h = hi(s)
s = rep1(s, """// AskClientForNumber(...) - can hop nhap so phia client (lam o giai doan cua so)
int LuaJX2_AskClientForNumber(Lua_State* L)
{
	Lua_PushNumber(L, -1);
	return 1;
}""", """// AskClientForNumber(szCbFun, nMin, nMax, szPrompt) - [JX2COMPAT 22/08] viet that (truoc la stub -1:
// SignUpTheOne loi dai CN + doi 2v2 bw chet). Linux 0x08115CA0: luu script id + ten cb + cho so, goi
// 0xA3 loai 2 (min/max/prompt); tra loi -> cb(so). Ta dung hop so san co cua client
// (S2C_INPUT_BOX nType 2 -> KUiGetNumber, khuon LuaOpenGetNumber) + co m_bWllsAskStrArg = 2
// de c2sInputCommand case 2 goi cb(so) (KProtocolProcess.cpp).
int LuaJX2_AskClientForNumber(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 4 || !Lua_IsString(L, 1))
		return 0;
	const char* szAction = Lua_ValueToString(L, 1);
	int nMin = (int)Lua_ValueToNumber(L, 2);
	int nMax = (int)Lua_ValueToNumber(L, 3);
	const char* szPrompt = Lua_IsString(L, 4) ? Lua_ValueToString(L, 4) : "";
	if (!szAction || !szAction[0])
		return 0;
	if (nMax < nMin)
		nMax = nMin;
	Player[nPlayerIndex].m_dwNumberBoxId = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;
	strncpy(Player[nPlayerIndex].m_szTaskExcuteFun, szAction, sizeof(Player[nPlayerIndex].m_szTaskExcuteFun) - 1);
	Player[nPlayerIndex].m_szTaskExcuteFun[sizeof(Player[nPlayerIndex].m_szTaskExcuteFun) - 1] = 0;
	Player[nPlayerIndex].m_bWllsAskStrArg = 2;
	Player[nPlayerIndex].m_nJx2AskMin = nMin;
	Player[nPlayerIndex].m_nJx2AskMax = nMax;
	S2C_INPUT_BOX NetCommand;
	NetCommand.ProtocolType = s2c_inputbox;
	NetCommand.nType = 2;
	strncpy(NetCommand.Value, szPrompt ? szPrompt : "", sizeof(NetCommand.Value) - 1);
	NetCommand.Value[sizeof(NetCommand.Value) - 1] = 0;
	strncpy(NetCommand.Value1, szAction, sizeof(NetCommand.Value1) - 1);
	NetCommand.Value1[sizeof(NetCommand.Value1) - 1] = 0;
	if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx != -1)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(S2C_INPUT_BOX));
	return 0;
}""")
assert hi(s) == h; wr(p, s); print("KTongJX2.cpp ok")

# ---------- KNpc.cpp: OnDeath fallback ----------
p = SRC + r"\KNpc.cpp"; s = rd(p); h = hi(s)
s = rep1(s, """			if (Player[m_nPlayerIdx].m_dwDeathScriptId)
			{
				Player[m_nPlayerIdx].ExecuteScript2(Player[m_nPlayerIdx].m_dwDeathScriptId, "OnPlayerDeath", m_nPlayerIdx, m_nLastDamageIdx); //#khi player bi chet
			}""", """			if (Player[m_nPlayerIdx].m_dwDeathScriptId)
			{
				// [JX2COMPAT 22/08] script chet cua Linux dat OnDeath(Launcher) (citywar_arena, tongwar,
				// bw, messenger playerdead_tollgate); JX1 dat OnPlayerDeath(pidx, killer). Do ham co that.
				char szDeathFun[16];
				strcpy(szDeathFun, "OnPlayerDeath");
				{
					KLuaScript* pDs = (KLuaScript*)g_GetScript(Player[m_nPlayerIdx].m_dwDeathScriptId);
					if (pDs && pDs->m_LuaState)
					{
						int nTopD = Lua_GetTopIndex(pDs->m_LuaState);
						Lua_GetGlobal(pDs->m_LuaState, "OnPlayerDeath");
						if (!lua_isfunction(pDs->m_LuaState, Lua_GetTopIndex(pDs->m_LuaState)))
							strcpy(szDeathFun, "OnDeath");
						lua_settop(pDs->m_LuaState, nTopD);
					}
				}
				if (strcmp(szDeathFun, "OnDeath") == 0)
					Player[m_nPlayerIdx].ExecuteScript(Player[m_nPlayerIdx].m_dwDeathScriptId, "OnDeath", m_nLastDamageIdx);
				else
				Player[m_nPlayerIdx].ExecuteScript2(Player[m_nPlayerIdx].m_dwDeathScriptId, "OnPlayerDeath", m_nPlayerIdx, m_nLastDamageIdx); //#khi player bi chet
			}""")
assert hi(s) == h; wr(p, s); print("KNpc.cpp ok")
print("ALL OK")
