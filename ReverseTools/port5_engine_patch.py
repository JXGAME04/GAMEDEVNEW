# -*- coding: utf-8 -*-
r"""[PORT5 23/08] Engine con lai cho 4 tinh nang port (tongwar / bairenleitai / bw / tongcastle).
Nguon dac ta: SPEC_PORT_TONGWAR.md muc C, port_tongwar\dac_ta_ham_tongwar.json,
port_bairenleitai\dac_ta_ham_engine_bairenleitai.json, port_tongcastle\dac_ta_ham_tongcastle.json.

TONGWAR   : SetDeathType (luu), GetMapInfoFile, ClearMapTrap, GetAllEquipment,
            ST_SyncMiniMapObj stub, CreateChannel/Enter/Leave/DeleteChannel stub,
            BT_LeaveBattle -> no-op (Linux chi gui goi client), BT_ClearPlayerData chua type 40-49,
            kho BT tach nhom theo cay script (citywar <-> tongwar dung chung type -> ghi de cheo).
BAIREN    : Msg2Map alias, SetTmpCamp/GetTmpCamp + luat GetRelation, AddTimer/DelTimer/
            SuspendTimer/ResumeTimer + KJx2ScriptTimer_Breathe, RemoteExecute (1 GS tai cho),
            AddMapTrap tham so 5, CallPlayerFunction tra ket qua, DynamicExecute* [[...]] 4096.
TONGCASTLE: GetNpcId/NpcCastSkill alias, GetItemStackCount/SetItemStackCount, MakeDateTime,
            GetNpcAroundNpcList, NPCINFO_Get/SetNpcCurrentLife, OB_SaveShareData/OB_LoadShareData,
            AddNpcEx tham so 7 = bNoRevive (giu nhanh SetCurrentCamp cu).
GHI CHU   : GetAroundNpcList "< 2" KHONG sua - Lua_NewTable push truoc khi dem top nen 1 tham so
            van qua (JSON tongcastle doan sai); kiem lai bang tay 23/08.
"""
import io, re, sys

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
    assert False, "KHONG THAY DUY NHAT: " + old[:120]

def repn(s, old, new, count):
    o = old.replace("\r\n", "\n"); n = new.replace("\r\n", "\n")
    for cand in ("\r\n", "\n"):
        o2 = o.replace("\n", cand)
        if s.count(o2) == count:
            return s.replace(o2, n.replace("\n", cand))
    assert False, "KHONG DUNG %d LAN: %s (thay %d/%d)" % (count, old[:90], s.count(old.replace("\n", "\r\n")), s.count(old))

def rex1(s, pattern, repl):
    m = re.findall(pattern, s)
    assert len(m) == 1, "REGEX %d lan (can 1): %s" % (len(m), pattern[:90])
    return re.sub(pattern, repl, s, count=1)

# ============================================================================
# 1) KRegion.h - mang tham so trap + accessor + ClearAllTraps + define
# ============================================================================
p = SRC + r"\KRegion.h"; s = rd(p); h = hi(s)
s = rex1(s, r"(#define\tREGION_GRID_WIDTH\t16\r?\n#define\tREGION_GRID_HEIGHT\t32\r?\n)",
         "\\1// [PORT5 23/08] o trap khong co tham so JX2 (trap JX1/map-data): main(nPlayerIdx) nhu cu\n#define JX2TRAP_PARAM_NONE\t0x7FFFFFFF\n")
s = rex1(s, r"(\tDWORD\t\tm_dwTrap\[REGION_GRID_WIDTH\]\[REGION_GRID_HEIGHT\];[^\r\n]*\r?\n)",
         "\\1\t// [PORT5 23/08] tham so trap JX2 (AddMapTrap tham so 5 - Linux KRegion trap = {scriptId, nParam})\n\tint\t\t\tm_nTrapParam[REGION_GRID_WIDTH][REGION_GRID_HEIGHT];\n")
s = rex1(s, r"(\tDWORD\t\tGetTrap\(int MapX, int MapY\);[^\r\n]*\r?\n)",
         "\\1\t// [PORT5 23/08] tham so trap JX2 theo o; JX2TRAP_PARAM_NONE khi khong dat (than co #ifdef nhu GetTrap)\n\tint\t\t\tGetTrapParam(int nMapX, int nMapY);\n\tvoid\t\tSetTrapParam(int nMapX, int nMapY, int nParam);\n\tvoid\t\tClearAllTraps();\n")
assert hi(s) == h; wr(p, s); print("KRegion.h ok")

# ============================================================================
# 2) KRegion.cpp - init tham so tai 3 cho memset + 3 ham moi
# ============================================================================
p = SRC + r"\KRegion.cpp"; s = rd(p); h = hi(s)
NL = "\r\n" if "\r\n" in s else "\n"
FILL_LINES = ["// [PORT5 23/08] tham so trap JX2: mac dinh NONE (trap JX1)",
              "for (int _tp = 0; _tp < REGION_GRID_WIDTH * REGION_GRID_HEIGHT; _tp++)",
              "\t((int*)m_nTrapParam)[_tp] = JX2TRAP_PARAM_NONE;"]
FILL1 = "".join("\t" + ln + NL for ln in FILL_LINES)
FILL2 = "".join("\t\t" + ln + NL for ln in FILL_LINES)
old1 = "\tmemset(m_dwTrap, 0, sizeof(m_dwTrap));" + NL
old2 = "\t\tmemset(this->m_dwTrap, 0, sizeof(m_dwTrap));" + NL
assert s.count(old1) == 2, "memset m_dwTrap: %d" % s.count(old1)
assert s.count(old2) == 1, "memset this->m_dwTrap: %d" % s.count(old2)
s = s.replace(old1, old1 + FILL1)
s = s.replace(old2, old2 + FILL2)
s = rep1(s, """void	KRegion::SetTrap(DWORD nTrapId, int nMapX, int nMapY)
{
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_GRID_WIDTH || nMapY >= REGION_GRID_HEIGHT)
		return;
	m_dwTrap[nMapX][nMapY] = nTrapId;
}
""", """void	KRegion::SetTrap(DWORD nTrapId, int nMapX, int nMapY)
{
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_GRID_WIDTH || nMapY >= REGION_GRID_HEIGHT)
		return;
	m_dwTrap[nMapX][nMapY] = nTrapId;
}

// [PORT5 23/08] tham so trap JX2 (AddMapTrap tham so 5). Than #ifdef nhu GetTrap.
int KRegion::GetTrapParam(int nMapX, int nMapY)
{
#ifdef _SERVER
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_GRID_WIDTH || nMapY >= REGION_GRID_HEIGHT)
		return JX2TRAP_PARAM_NONE;
	return m_nTrapParam[nMapX][nMapY];
#else
	return JX2TRAP_PARAM_NONE;
#endif
}

void KRegion::SetTrapParam(int nMapX, int nMapY, int nParam)
{
#ifdef _SERVER
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_GRID_WIDTH || nMapY >= REGION_GRID_HEIGHT)
		return;
	m_nTrapParam[nMapX][nMapY] = nParam;
#endif
}

// [PORT5 23/08] ClearMapTrap (Linux KRegion::ClearTrap 0x080E11A0): xoa SACH trap cua region
// (ke ca trap map-data - Linux cung vay; chi dung cho map chien truong rieng).
void KRegion::ClearAllTraps()
{
#ifdef _SERVER
	memset(m_dwTrap, 0, sizeof(m_dwTrap));
	for (int _tp = 0; _tp < REGION_GRID_WIDTH * REGION_GRID_HEIGHT; _tp++)
		((int*)m_nTrapParam)[_tp] = JX2TRAP_PARAM_NONE;
#endif
}
""")
assert hi(s) == h; wr(p, s); print("KRegion.cpp ok")

# ============================================================================
# 3) KSubWorld.h - m_szMapInfoFile + SetTrapParam decl
# ============================================================================
p = SRC + r"\KSubWorld.h"; s = rd(p); h = hi(s)
s = rep1(s, "\tchar\t\tm_szNewWorldParam[128];\n",
            "\tchar\t\tm_szNewWorldParam[128];\n\t// [PORT5 23/08] khoa <id>_MapInfo cua MapList.ini (tongwar GetMapInfoFile - Linux KSubWorld+0x4EFC4)\n\tchar\t\tm_szMapInfoFile[128];\n")
s = rex1(s, r"(\tvoid\t\tSetTrap\(DWORD dwTrapId, int nMpsX, int nMpsY, int nRange\);[^\r\n]*\r?\n)",
         "\\1\tvoid\t\tSetTrapParam(int nMpsX, int nMpsY, int nParam);\t// [PORT5 23/08] tham so o trap JX2 (o chinh)\n")
assert hi(s) == h; wr(p, s); print("KSubWorld.h ok")

# ============================================================================
# 4) KSubWorld.cpp - doc %d_MapInfo + SetTrapParam body
# ============================================================================
p = SRC + r"\KSubWorld.cpp"; s = rd(p); h = hi(s)
s = rep1(s, """	sprintf(szKeyName, "%d_NewWorldParam", nId);
	IniFile.GetString("List", szKeyName, "", m_szNewWorldParam, sizeof(m_szNewWorldParam));
""", """	sprintf(szKeyName, "%d_NewWorldParam", nId);
	IniFile.GetString("List", szKeyName, "", m_szNewWorldParam, sizeof(m_szNewWorldParam));

	// [PORT5 23/08] <id>_MapInfo (tongwar GetMapInfoFile); rut '\\\\' kep ve '\\' don cho
	// KIniFile/TabFile phia Windows khoi phai doan (Linux fopen tu chiu '//')
	m_szMapInfoFile[0] = 0;
	sprintf(szKeyName, "%d_MapInfo", nId);
	IniFile.GetString("List", szKeyName, "", m_szMapInfoFile, sizeof(m_szMapInfoFile));
	{
		char* pR = m_szMapInfoFile;
		char* pW = m_szMapInfoFile;
		while (*pR)
		{
			*pW++ = *pR;
			if (pR[0] == '\\\\' && pR[1] == '\\\\')
				pR += 2;
			else
				pR++;
		}
		*pW = 0;
	}
""")
s = rep1(s, "void KSubWorld::SetTrap(DWORD dwTrapId, int nMpsX, int nMpsY, int nRange)\n{",
            """// [PORT5 23/08] tham so o trap JX2 (AddMapTrap tham so 5) - chi o chinh (nRange 0)
void KSubWorld::SetTrapParam(int nMpsX, int nMpsY, int nParam)
{
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion == -1)
		return;
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_GRID_WIDTH || nMapY >= REGION_GRID_HEIGHT)
		return;
	m_Region[nRegion].SetTrapParam(nMapX, nMapY, nParam);
}

void KSubWorld::SetTrap(DWORD dwTrapId, int nMpsX, int nMpsY, int nRange)
{""")
assert hi(s) == h; wr(p, s); print("KSubWorld.cpp ok")

# ============================================================================
# 5) KNpc.h - 3 truong moi (server)
# ============================================================================
p = SRC + r"\KNpc.h"; s = rd(p); h = hi(s)
s = rex1(s, r"(\tDWORD\t+m_TrapScriptID;[^\r\n]*\r?\n)",
         "\\1#ifdef _SERVER\n\t// [PORT5 23/08] JX2: tham so o trap dang dung / trai tam thoi / co khong-hoi-sinh\n\tint\t\t\t\tm_nLastTrapParam;\t\t// JX2TRAP_PARAM_NONE = trap JX1\n\tint\t\t\t\tm_nTmpCamp;\t\t\t\t// Linux KNpc+0x1900; 0 = khong dung (SetTmpCamp)\n\tBYTE\t\t\tm_bNoRevive;\t\t\t// Linux +0x1824 (AddNpcEx tham so 7): chet la bien mat\n#endif\n")
assert hi(s) == h; wr(p, s); print("KNpc.h ok")

# ============================================================================
# 6) KNpc.cpp - Init + CheckTrap + OnDeath + OnRevive
# ============================================================================
p = SRC + r"\KNpc.cpp"; s = rd(p); h = hi(s)
s = rep1(s, "\tm_TrapScriptID = 0;\n",
            "\tm_TrapScriptID = 0;\n#ifdef _SERVER\n\tm_nLastTrapParam = JX2TRAP_PARAM_NONE;\t// [PORT5 23/08]\n\tm_nTmpCamp = 0;\n\tm_bNoRevive = 0;\n#endif\n")
s = rep1(s, """	DWORD	dwTrap = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].GetTrap(m_MapX, m_MapY);
	if (m_TrapScriptID == dwTrap)
	{
		return;
	}
	else
	{
		m_TrapScriptID = dwTrap;
	}
""", """	DWORD	dwTrap = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].GetTrap(m_MapX, m_MapY);
	// [PORT5 23/08] trap JX2 (AddMapTrap tham so 5): so sanh CA (scriptId, param) de hai vung
	// trap ke nhau CUNG script khac param van kich (tongcastle); trap JX1 param NONE - nhu cu.
	int nCellTrapParam = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].GetTrapParam(m_MapX, m_MapY);
	if (m_TrapScriptID == dwTrap
#ifdef _SERVER
		&& m_nLastTrapParam == nCellTrapParam
#endif
		)
	{
		return;
	}
	else
	{
		m_TrapScriptID = dwTrap;
#ifdef _SERVER
		m_nLastTrapParam = nCellTrapParam;
#endif
	}
""")
s = rep1(s, """	Player[m_nPlayerIdx].ExecuteScript(m_TrapScriptID, "main", m_nPlayerIdx);
}

void KNpc::SetFightMode(BOOL bFightMode)
""", """#ifdef _SERVER
	if (nCellTrapParam != JX2TRAP_PARAM_NONE)
	{
		// [PORT5 23/08] trap dat qua AddMapTrap: main(nParam) nhu Linux (KNpc::CheckTrap 0x0807DA78;
		// 4 tham so -> param 0 -> main(0) cung nhu Linux)
		Player[m_nPlayerIdx].ExecuteScript(m_TrapScriptID, "main", nCellTrapParam);
		return;
	}
#endif
	Player[m_nPlayerIdx].ExecuteScript(m_TrapScriptID, "main", m_nPlayerIdx);
}

void KNpc::SetFightMode(BOOL bFightMode)
""")
s = rep1(s, """		if (m_Kind != kind_partner)
		{
			DoRevive();
#ifdef _SERVER
			int nPIdx = GetPlayerIdx();
""", """		if (m_Kind != kind_partner)
		{
			DoRevive();
#ifdef _SERVER
			// [PORT5 23/08] AddNpcEx bNoRevive: xac chi nan ~1s roi OnRevive go NPC (Linux xoa
			// ngay sau hoat anh chet qua hang doi 0x3E9 - 0x08083520)
			if (!IsPlayer() && m_bNoRevive && m_Doing == do_revive)
				m_Frames.nTotalFrame = 18;
			int nPIdx = GetPlayerIdx();
""")
s = rep1(s, """#ifdef _SERVER
	if (!IsPlayer() && WaitForFrame())
	{
		Revive();
	}
""", """#ifdef _SERVER
	if (!IsPlayer() && WaitForFrame())
	{
		// [PORT5 23/08] AddNpcEx bNoRevive: go NPC thay vi hoi sinh (khuon LuaDelNpc; cung thoi
		// diem voi DelNpc trong script OnRevive cua global\\drop\\daihoangkim.lua - da chay that)
		if (m_bNoRevive)
		{
			if (m_SubWorldIndex >= 0 && m_RegionIndex >= 0)
			{
				SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].RemoveNpc(m_Index);
				SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);
				NpcSet.Remove(m_Index);
			}
			return;
		}
		Revive();
	}
""")
assert hi(s) == h; wr(p, s); print("KNpc.cpp ok")

# ============================================================================
# 7) KNpcSet.cpp - luat TmpCamp trong GetRelation (sau khoi SimCity)
# ============================================================================
p = SRC + r"\KNpcSet.cpp"; s = rd(p); h = hi(s)
s = rep1(s, """	if (Npc[nId1].m_btSimCityBot || Npc[nId2].m_btSimCityBot)
	{
		return relation_none;
	}

	if (Npc[nId1].m_Kind != kind_player || Npc[nId2].m_Kind != kind_player)
""", """	if (Npc[nId1].m_btSimCityBot || Npc[nId2].m_btSimCityBot)
	{
		return relation_none;
	}

	// [PORT5 23/08] trai tam thoi JX2 (Linux GetRelation 0x0809EF72-0x0809EFB3): ca hai co
	// TmpCamp != 0 va (khong phai nguoi choi HOAC dang bat chien dau) -> DE LEN moi luat
	// phe/PK: khac trai = dich, cung trai = dong minh (bairenleitai / tongcastle).
	if (Npc[nId1].m_nTmpCamp && Npc[nId2].m_nTmpCamp &&
		Npc[nId1].m_Kind != kind_dialoger && Npc[nId2].m_Kind != kind_dialoger &&
		(Npc[nId1].m_Kind != kind_player || Npc[nId1].m_FightMode) &&
		(Npc[nId2].m_Kind != kind_player || Npc[nId2].m_FightMode))
	{
		return (Npc[nId1].m_nTmpCamp != Npc[nId2].m_nTmpCamp) ? relation_enemy : relation_ally;
	}

	if (Npc[nId1].m_Kind != kind_player || Npc[nId2].m_Kind != kind_player)
""")
assert hi(s) == h; wr(p, s); print("KNpcSet.cpp ok")

# ============================================================================
# 8) KPlayer.h / KPlayer.cpp - m_nJX2DeathType
# ============================================================================
p = SRC + r"\KPlayer.h"; s = rd(p); h = hi(s)
s = rep1(s, "\tint\t\t\t\tm_nJx2AskMax;\n",
            "\tint\t\t\t\tm_nJx2AskMax;\n\tint\t\t\t\tm_nJX2DeathType;\t\t// [PORT5 23/08] SetDeathType tongwar (Linux Player+0xE4) - chi luu, cay luon goi kem SetPunish\n")
assert hi(s) == h; wr(p, s); print("KPlayer.h ok")

p = SRC + r"\KPlayer.cpp"; s = rd(p); h = hi(s)
s = rep1(s, "\tm_dwDeathScriptId = 0;\n",
            "\tm_dwDeathScriptId = 0;\n\tm_nJX2DeathType = 0;\t\t// [PORT5 23/08]\n")
assert hi(s) == h; wr(p, s); print("KPlayer.cpp ok")

# ============================================================================
# 9) KJx2WarInfra.cpp - ClearMapTrap + GetItemStackCount/Set + GetNpcAroundNpcList
# ============================================================================
p = SRC + r"\KJx2WarInfra.cpp"; s = rd(p); h = hi(s)
s = rep1(s, """int LuaClearMapObj(Lua_State* L)
{""", """// [PORT5 23/08] ClearMapTrap(nMapId) - Linux 0x08102AC0 -> KSubWorld::ClearTrap 0x080EFDF0:
// xoa SACH trap MOI region cua map (ke ca trap map-data - Linux cung vay). tongwar
// InitMission/EndMission goi cho map chien truong rieng 605-607, KHONG dung cho map thanh.
int LuaClearMapTrap(Lua_State* L)
{
	if (Lua_GetTopIndex(L) != 1 || !Lua_IsNumber(L, 1))
		return 0;
	int w = g_SubWorldSet.SearchWorld((DWORD)Lua_ValueToNumber(L, 1));
	if (w < 0 || w >= MAX_SUBWORLD)
		return 0;
	KSubWorld* pWorld = &SubWorld[w];
	for (int r = 0; r < pWorld->m_nTotalRegion; r++)
		pWorld->m_Region[r].ClearAllTraps();
	return 0;
}

// [PORT5 23/08] GetItemStackCount (Linux 0x080FD250) / SetItemStackCount (0x0810D9A0) -
// tongcastle guard.lua tru dan bua trieu hoi trong chong (item 3822 stack).
int LuaGetItemStackCount(Lua_State* L)
{
	if (Lua_GetTopIndex(L) != 1 || !Lua_IsNumber(L, 1))
	{
		Lua_PushNumber(L, -1);
		return 1;
	}
	int n = (int)Lua_ValueToNumber(L, 1);
	if (n <= 0 || n >= MAX_ITEM)
	{
		Lua_PushNumber(L, -1);
		return 1;
	}
	int c = Item[n].GetStackNum();
	if (c < 1)
		c = 1;
	int nMax = Item[n].GetMaxStackNum();
	if (nMax >= 1 && c > nMax)
		c = nMax;
	Lua_PushNumber(L, c);
	return 1;
}

int LuaSetItemStackCount(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (Lua_GetTopIndex(L) != 2 || nPlayerIndex <= 0 || !Lua_IsNumber(L, 1) || !Lua_IsNumber(L, 2))
	{
		Lua_PushNumber(L, -1);
		return 1;
	}
	int n = (int)Lua_ValueToNumber(L, 1);
	int c = (int)Lua_ValueToNumber(L, 2);
	if (n <= 0 || n >= MAX_ITEM || c < 1)
	{
		Lua_PushNumber(L, -1);
		return 1;
	}
	int nMax = Item[n].GetMaxStackNum();
	if (nMax >= 1 && c > nMax)
	{
		Lua_PushNumber(L, -1);
		return 1;
	}
	Item[n].SetStackNum(c);
	Player[nPlayerIndex].m_ItemList.SyncItem(n);
	Lua_PushNumber(L, 1);
	return 1;
}

// [PORT5 23/08] GetNpcAroundNpcList(nNpcIndex, nDist[, nMask]) - Linux 0x08104A20: nhu
// GetAroundNpcList nhung tam la NPC bat ky (tongcastle treedeath quet quanh cay). Mask bo
// qua (khong loc quan he - script tu loc theo NpcParam). Tra (tbList, nCount).
int LuaGetNpcAroundNpcList(Lua_State* L)
{
	int nCount = 0;
	int nTop = Lua_GetTopIndex(L);
	Lua_NewTable(L);
	if (nTop < 2 || !Lua_IsNumber(L, 1) || !Lua_IsNumber(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 2;
	}
	int nMe = (int)Lua_ValueToNumber(L, 1);
	int nDist = (int)Lua_ValueToNumber(L, 2);
	if (nMe <= 0 || nMe >= MAX_NPC || Npc[nMe].m_Index <= 0 || nDist <= 0 || Npc[nMe].m_SubWorldIndex < 0)
	{
		Lua_PushNumber(L, 0);
		return 2;
	}
	KSubWorld* pWorld = &SubWorld[Npc[nMe].m_SubWorldIndex];
	int nMX = Npc[nMe].m_MapX, nMY = Npc[nMe].m_MapY;
	for (int r = 0; r < pWorld->m_nTotalRegion; r++)
	{
		KRegion* pRegion = &pWorld->m_Region[r];
		KIndexNode* pNode = (KIndexNode*)pRegion->m_NpcList.GetHead();
		while (pNode)
		{
			int i = pNode->m_nIndex;
			pNode = (KIndexNode*)pNode->GetNext();
			if (i <= 0 || i >= MAX_NPC || Npc[i].m_Index <= 0 || i == nMe)
				continue;
			if (Npc[i].m_Doing == do_death || Npc[i].m_Doing == do_revive)
				continue;
			int dx = Npc[i].m_MapX - nMX, dy = Npc[i].m_MapY - nMY;
			if (dx * dx + dy * dy > nDist * nDist)
				continue;
			Lua_PushNumber(L, i);
			Lua_RawSetI(L, -2, ++nCount);
		}
	}
	Lua_PushNumber(L, nCount);
	return 2;
}

int LuaClearMapObj(Lua_State* L)
{""")
assert hi(s) == h; wr(p, s); print("KJx2WarInfra.cpp ok")

# ============================================================================
# 10) KJx2Battle.cpp - kho theo NHOM cay script + LeaveBattle no-op + ClearPlayerData 40-49
# ============================================================================
p = SRC + r"\KJx2Battle.cpp"; s = rd(p); h = hi(s)
s = rep1(s, "#include \"KJx2Battle.h\"\n",
            "#include \"KJx2Battle.h\"\n#include \"KSortScript.h\"\t// [PORT5 23/08] g_GetScriptNameByState - tach kho BT theo cay script\n")
s = rep1(s, """static std::map<int, int>		s_Type2Task;	// type -> player task id
static std::map<int, int>		s_Bonus;		// khoa = nType*16 + nCamp
static std::map<int, int>		s_GameData;
static std::vector<int>			s_ViewTypes;
static char						s_szMissionName[64] = "";
static int						s_nRestTime = 0;
static std::vector<KJx2BtMember> s_Members;
static int						s_nBattleSeq = 1;	// tang moi BT_ClearBattle
""", """// [PORT5 23/08] kho BT tach theo CAY script (Linux: KBattle nam trong KSubWorld). citywar_city
// dang ky type->task luc boot (head.lua:435-437), tongwar dang ky lai luc InitMission 20h
// (bt_setnormaltask2type) - dung chung MOT map la ghi de cheo: diem citywar rot vao task mua
// tongwar (2369-2378) va nguoc lai; BT_ClearPlayerData cua ben nay quet task cua ben kia.
struct KJx2BtStore
{
	std::map<int, int>			mapType2Task;	// type -> player task id
	std::map<int, int>			mapBonus;		// khoa = nType*16 + nCamp
	std::map<int, int>			mapGameData;
	std::vector<int>			vViewTypes;
	char						szMissionName[64];
	int							nRestTime;
	std::vector<KJx2BtMember>	vMembers;
	int							nBattleSeq;		// tang moi BT_ClearBattle
	KJx2BtStore() { szMissionName[0] = 0; nRestTime = 0; nBattleSeq = 1; }
};
static KJx2BtStore	s_BtStore[2];		// [0] citywar/mac dinh; [1] tongwar
static int			sBtCurGroup = 0;	// dat o dau MOI ham Lua theo state goi

static int sBtGroupOfState(Lua_State* L)
{
	const char* szName = g_GetScriptNameByState(L);
	if (szName && (strstr(szName, "\\\\script\\\\missions\\\\tongwar\\\\") != NULL ||
			strstr(szName, "\\\\script\\\\event\\\\tongwar\\\\") != NULL))
		return 1;
	return 0;
}

// giu nguyen moi than ham ben duoi: cac ten cu tro vao kho cua nhom hien hanh
#define s_Type2Task		(s_BtStore[sBtCurGroup].mapType2Task)
#define s_Bonus			(s_BtStore[sBtCurGroup].mapBonus)
#define s_GameData		(s_BtStore[sBtCurGroup].mapGameData)
#define s_ViewTypes		(s_BtStore[sBtCurGroup].vViewTypes)
#define s_szMissionName	(s_BtStore[sBtCurGroup].szMissionName)
#define s_nRestTime		(s_BtStore[sBtCurGroup].nRestTime)
#define s_Members		(s_BtStore[sBtCurGroup].vMembers)
#define s_nBattleSeq	(s_BtStore[sBtCurGroup].nBattleSeq)
""")
# ClearPlayerData: chua type 40-49 (Linux 0x081C6050 "lea eax,[ebx-0x28]; cmp eax,9; jbe")
s = rep1(s, """// (phan bien E4 CHAN-4) trap.lua:52-53 goi TRUOC JoinCamp; chefu.lua:8 khi roi
int LuaBT_ClearPlayerData(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		sBtResetPlayerTasks(nPlayerIndex);
		KJx2BtMember* pMem = sBtMember(Player[nPlayerIndex].m_PlayerName, false);
		if (pMem)
			pMem->mapData.clear();
	}
	return 0;
}
""", """// (phan bien E4 CHAN-4) trap.lua:52-53 goi TRUOC JoinCamp; chefu.lua:8 khi roi
int LuaBT_ClearPlayerData(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		// [PORT5 23/08] Linux 0x081C6050: type 0..49 TRU 40..49 (PL_KEYNUMBER 45 /
		// PL_LASTDEATHTIME 46 / PL_BATTLEPOINT 47 GIU) - tongwar giu so bao danh 2372 +
		// diem ca mua 2378 khi roi tran.
		std::map<int, int>::iterator t;
		for (t = s_Type2Task.begin(); t != s_Type2Task.end(); ++t)
		{
			if (t->first >= 40 && t->first <= 49)
				continue;
			if (t->second > 0)
				Player[nPlayerIndex].m_cTask.SetSaveVal(t->second, 0);
		}
		KJx2BtMember* pMem = sBtMember(Player[nPlayerIndex].m_PlayerName, false);
		if (pMem)
		{
			std::map<int, int>::iterator d = pMem->mapData.begin();
			while (d != pMem->mapData.end())
			{
				if (d->first >= 40 && d->first <= 49)
					++d;
				else
					pMem->mapData.erase(d++);
			}
		}
	}
	return 0;
}
""")
# LeaveBattle: no-op (Linux KBattle::LeaveBattle 0x08148F30 chi gui goi 7 byte 0xB0/5 cho client)
s = rep1(s, """int LuaBT_LeaveBattle(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		sBtResetPlayerTasks(nPlayerIndex);
		for (size_t i = 0; i < s_Members.size(); i++)
		{
			if (strcmp(s_Members[i].szName, Player[nPlayerIndex].m_PlayerName) == 0)
			{
				s_Members.erase(s_Members.begin() + i);
				break;
			}
		}
	}
	return 0;
}
""", """int LuaBT_LeaveBattle(Lua_State* L)
{
	// [PORT5 23/08] Linux KBattle::LeaveBattle 0x08148F30: CHI gui goi 7 byte (0xB0 sub 5)
	// dong bang xep hang phia client - KHONG dung task/diem/member. Ban cu quet 0 moi task
	// da map: tongwar trap:92 goi TRUOC khi vao tran -> NDEATH/MAXDEATH ve 0 -> check_outmatch
	// (0 >= 0) day nguoi choi ra ngay o hometrap dau tien, diem ca mua mat. Client JX1 khong
	// co goi 0xB0 -> no-op. (Xoa member cung KHONG lam: SetData ke tiep se tao lai voi nSeq=0
	// va quet 0 task qua duong CAO-3.)
	return 0;
}
""")
# dat nhom theo state o dau MOI ham LuaBT_* (dang nhieu dong)
pat = re.compile(r"(int LuaBT_\w+\(Lua_State\* L\)\r?\n\{\r?\n)")
found = pat.findall(s)
assert len(found) == 15, "LuaBT_ nhieu dong: %d (can 15)" % len(found)
s = pat.sub(lambda m: m.group(1) + ("\tsBtCurGroup = sBtGroupOfState(L);\r\n" if "\r\n" in m.group(1) else "\tsBtCurGroup = sBtGroupOfState(L);\n"), s)
assert hi(s) == h; wr(p, s); print("KJx2Battle.cpp ok")

# ============================================================================
# 11) KJx2League.h + KJx2League.cpp - he AddTimer/DelTimer JX2
# ============================================================================
p = SRC + r"\KJx2League.h"; s = rd(p); h = hi(s)
s = rex1(s, r"(void KJx2DeferredExec_Breathe\(\);[^\r\n]*\r?\n)",
         "\\1void KJx2ScriptTimer_Breathe();\t\t\t// [PORT5 23/08] AddTimer/DelTimer JX2 (bairenleitai/tongcastle)\n")
assert hi(s) == h; wr(p, s); print("KJx2League.h ok")

p = SRC + r"\KJx2League.cpp"; s = rd(p); h = hi(s)
s = rep1(s, "#include \"KJx2League.h\"\n",
            "#include \"KJx2League.h\"\n#include \"KSortScript.h\"\t// [PORT5 23/08] g_ScriptSet - AddTimer tim script theo lua_State\n")
s = rep1(s, """// ============================================================================
// == WLLS / leaguematch port 20/08/2026 - xem THICONG_LIENDAU_PORT.md ========
// ============================================================================
""", """// ============================================================================
// [PORT5 23/08] He hen gio script JX2: AddTimer(nFrames, "Ham", nParam) -> id (Linux 0x08100D40,
// bo dem KScriptTimer 0x081CC300). Khi den han goi Ham(nParam, nTimerId) trong state cua TEP
// DANG GOI (ho tro dang "Bang:Ham" - timerlist.lua dang ky "TimerList:OnTime"); ket qua:
// 0 gia tri -> huy; 1 gia tri t -> t==0 huy / t!=0 hen lai t frame; >=2 gia tri (t, p) ->
// hen lai t frame voi param moi p. DelTimer/SuspendTimer/ResumeTimer theo id.
// ============================================================================
extern unsigned int nCurrentScriptNum;	// KSortScript.cpp

struct KJx2ScriptTimer
{
	int		nId;
	int		nScriptIdx;		// chi so trong g_ScriptSet (script khong bao gio go luc chay)
	char	szFunc[260];	// Linux ten ham toi da 0x103
	int		nParam;
	DWORD	dwIntervalMs;
	DWORD	dwNextFire;
	int		bSuspend;
	DWORD	dwRemainMs;
};
static std::vector<KJx2ScriptTimer>	s_ScriptTimers;
static int							s_nScriptTimerNextId = 1;

static int sFindScriptIdxByState(Lua_State* L)
{
	if (!L)
		return -1;
	for (unsigned int i = 0; i < nCurrentScriptNum && i < MAX_SCRIPT_IN_SET; i++)
	{
		if (g_ScriptSet[i].m_LuaState == L)
			return (int)i;
	}
	return -1;
}

static DWORD sJx2Frames2Ms(double fFrames)
{
	if (!(fFrames >= 1))
		fFrames = 1;			// bat ca NaN
	if (fFrames > 31104000.0)
		fFrames = 31104000.0;	// tran 20 ngay (nhu GlbMSTimer)
	return (DWORD)(fFrames * 1000.0 / 18.0);
}

int LuaJX2_AddTimer(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 3 || !Lua_IsNumber(L, 1) || !Lua_IsString(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	const char* szFunc = Lua_ValueToString(L, 2);
	if (!szFunc || !szFunc[0])
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nIdx = sFindScriptIdxByState(L);
	if (nIdx < 0)
	{
		// = Linux ActionScriptID == 0 (state khong thuoc script nao) -> push 0
		Lua_PushNumber(L, 0);
		return 1;
	}
	KJx2ScriptTimer t;
	t.nId = s_nScriptTimerNextId++;
	t.nScriptIdx = nIdx;
	strncpy(t.szFunc, szFunc, sizeof(t.szFunc) - 1);
	t.szFunc[sizeof(t.szFunc) - 1] = 0;
	t.nParam = (int)Lua_ValueToNumber(L, 3);
	t.dwIntervalMs = sJx2Frames2Ms(Lua_ValueToNumber(L, 1));
	t.dwNextFire = GetTickCount() + t.dwIntervalMs;
	t.bSuspend = 0;
	t.dwRemainMs = 0;
	s_ScriptTimers.push_back(t);
	Lua_PushNumber(L, t.nId);
	return 1;
}

int LuaJX2_DelTimer(Lua_State* L)
{
	int nOk = 0;
	if (Lua_GetTopIndex(L) >= 1 && Lua_IsNumber(L, 1))
	{
		int nId = (int)Lua_ValueToNumber(L, 1);
		for (size_t i = 0; i < s_ScriptTimers.size(); i++)
		{
			if (s_ScriptTimers[i].nId == nId)
			{
				s_ScriptTimers[i].nId = 0;	// danh dau, Breathe don - an toan khi goi trong callback
				nOk = 1;
				break;
			}
		}
	}
	Lua_PushNumber(L, nOk);
	return 1;
}

int LuaJX2_SuspendTimer(Lua_State* L)
{
	int nOk = 0;
	if (Lua_GetTopIndex(L) >= 1 && Lua_IsNumber(L, 1))
	{
		int nId = (int)Lua_ValueToNumber(L, 1);
		for (size_t i = 0; i < s_ScriptTimers.size(); i++)
		{
			if (s_ScriptTimers[i].nId == nId && !s_ScriptTimers[i].bSuspend)
			{
				int nLeft = (int)(s_ScriptTimers[i].dwNextFire - GetTickCount());
				s_ScriptTimers[i].dwRemainMs = (nLeft > 0) ? (DWORD)nLeft : 0;
				s_ScriptTimers[i].bSuspend = 1;
				nOk = 1;
				break;
			}
		}
	}
	Lua_PushNumber(L, nOk);
	return 1;
}

int LuaJX2_ResumeTimer(Lua_State* L)
{
	int nOk = 0;
	if (Lua_GetTopIndex(L) >= 1 && Lua_IsNumber(L, 1))
	{
		int nId = (int)Lua_ValueToNumber(L, 1);
		for (size_t i = 0; i < s_ScriptTimers.size(); i++)
		{
			if (s_ScriptTimers[i].nId == nId && s_ScriptTimers[i].bSuspend)
			{
				s_ScriptTimers[i].dwNextFire = GetTickCount() + s_ScriptTimers[i].dwRemainMs;
				s_ScriptTimers[i].bSuspend = 0;
				nOk = 1;
				break;
			}
		}
	}
	Lua_PushNumber(L, nOk);
	return 1;
}

// goi moi tick ngay sau KJx2DeferredExec_Breathe (CoreServerShell.cpp)
void KJx2ScriptTimer_Breathe()
{
	if (s_ScriptTimers.empty())
		return;
	DWORD dwNow = GetTickCount();
	// don timer da DelTimer (nId == 0)
	for (int k = (int)s_ScriptTimers.size() - 1; k >= 0; k--)
	{
		if (s_ScriptTimers[k].nId == 0)
			s_ScriptTimers.erase(s_ScriptTimers.begin() + k);
	}
	// thu thap truoc - callback co the Add/Del lam doi vector
	int nFire[64];
	int nFireCount = 0;
	for (size_t i = 0; i < s_ScriptTimers.size() && nFireCount < 64; i++)
	{
		if (!s_ScriptTimers[i].bSuspend && (int)(dwNow - s_ScriptTimers[i].dwNextFire) >= 0)
			nFire[nFireCount++] = s_ScriptTimers[i].nId;
	}
	for (int k = 0; k < nFireCount; k++)
	{
		size_t i;
		for (i = 0; i < s_ScriptTimers.size(); i++)
		{
			if (s_ScriptTimers[i].nId == nFire[k])
				break;
		}
		if (i >= s_ScriptTimers.size())
			continue;
		KJx2ScriptTimer t = s_ScriptTimers[i];	// ban sao - vector co the doi trong callback
		KLuaScript* pScript = NULL;
		if (t.nScriptIdx >= 0 && t.nScriptIdx < MAX_SCRIPT_IN_SET)
			pScript = &g_ScriptSet[t.nScriptIdx];
		int bContinue = 0;
		double fNewFrames = 0;
		int nNewParam = t.nParam;
		int bHasParam = 0;
		if (pScript && pScript->m_LuaState)
		{
			char szCall[400];
			sprintf(szCall, "return %s(%d,%d)", t.szFunc, t.nParam, t.nId);
			int nTop0 = Lua_GetTopIndex(pScript->m_LuaState);
			if (lua_dostring(pScript->m_LuaState, szCall) == 0)
			{
				int nRes = Lua_GetTopIndex(pScript->m_LuaState) - nTop0;
				if (nRes == 1)
				{
					fNewFrames = Lua_ValueToNumber(pScript->m_LuaState, -1);
					bContinue = (fNewFrames != 0);
				}
				else if (nRes >= 2)
				{
					fNewFrames = Lua_ValueToNumber(pScript->m_LuaState, -2);
					bContinue = (fNewFrames != 0);
					nNewParam = (int)Lua_ValueToNumber(pScript->m_LuaState, -1);
					bHasParam = 1;
				}
			}
			else
				g_DebugLog((LPSTR)"[PORT5] ScriptTimer %d LOI: %.200s", t.nId, szCall);
			lua_settop(pScript->m_LuaState, nTop0);
		}
		// tim lai - callback co the da DelTimer / push them
		for (i = 0; i < s_ScriptTimers.size(); i++)
		{
			if (s_ScriptTimers[i].nId == nFire[k])
				break;
		}
		if (i >= s_ScriptTimers.size())
			continue;
		if (!bContinue)
		{
			s_ScriptTimers.erase(s_ScriptTimers.begin() + i);
			continue;
		}
		s_ScriptTimers[i].dwIntervalMs = sJx2Frames2Ms(fNewFrames);
		s_ScriptTimers[i].dwNextFire = GetTickCount() + s_ScriptTimers[i].dwIntervalMs;
		if (bHasParam)
			s_ScriptTimers[i].nParam = nNewParam;
	}
}

// ============================================================================
// == WLLS / leaguematch port 20/08/2026 - xem THICONG_LIENDAU_PORT.md ========
// ============================================================================
""")
assert hi(s) == h; wr(p, s); print("KJx2League.cpp ok")

# ============================================================================
# 12) CoreServerShell.cpp - goi Breathe
# ============================================================================
p = SRC + r"\CoreServerShell.cpp"; s = rd(p); h = hi(s)
s = rep1(s, "\tKJx2DeferredExec_Breathe();\t// WLLS: hang doi \"dw/dwf\" hoan 1 tick (lien dau)\n",
            "\tKJx2DeferredExec_Breathe();\t// WLLS: hang doi \"dw/dwf\" hoan 1 tick (lien dau)\n\tKJx2ScriptTimer_Breathe();\t// [PORT5 23/08] AddTimer/DelTimer JX2 (bairenleitai/tongcastle)\n")
assert hi(s) == h; wr(p, s); print("CoreServerShell.cpp ok")

# ============================================================================
# 13) KJx2SharedStore.cpp - RemoteExecute + OB_SaveShareData/OB_LoadShareData
# ============================================================================
p = SRC + r"\KJx2SharedStore.cpp"; s = rd(p); h = hi(s)
s = rep1(s, "int LuaOB_IsEmpty(Lua_State* L)\n",
            """// [PORT5 23/08] RemoteExecute (Linux GS 0x08100740 / relay 0x0810363A): RPC GS<->relay qua
// ObjBuffer. Du an 1 GS khong relay -> thuc thi TAI CHO dong bo: fn(hParam, hRes, 0) trong
// state cua szScript (co remap \\script\\lib -> \\scriptjx2\\lib nhu Include); co callback ->
// cb(nCbParam, hRes) trong state DANG GOI; dwGameSvrId (tham so 6) bo qua. Caller tu
// OB_Release hParam; hRes do ham nay cap va huy.
int LuaJX2_RemoteExecute(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 3 || !Lua_IsString(L, 1) || !Lua_IsString(L, 2) || !Lua_IsNumber(L, 3))
		return 0;
	const char* szScript = Lua_ValueToString(L, 1);
	const char* szFunc = Lua_ValueToString(L, 2);
	int nHandle = (int)Lua_ValueToNumber(L, 3);
	if (!szScript || !szScript[0] || !szFunc || !szFunc[0] || nHandle < 0)
		return 0;
	char szLow[MAX_PATH];
	strncpy(szLow, szScript, MAX_PATH - 1);
	szLow[MAX_PATH - 1] = 0;
	{
		for (char* pc = szLow; *pc; pc++)
		{
			if (*pc >= 'A' && *pc <= 'Z')
				*pc += 'a' - 'A';
		}
	}
	KLuaScript* pScript = (KLuaScript*)g_GetScript(szLow);
	if (!pScript)
	{
		char szAlt[MAX_PATH + 16];
		const char* pLib = strstr(szLow, "\\\\script\\\\lib\\\\");
		if (pLib)
		{
			int nPre = (int)(pLib - szLow);
			memcpy(szAlt, szLow, nPre);
			szAlt[nPre] = 0;
			strcat(szAlt, "\\\\scriptjx2\\\\lib\\\\");
			strcat(szAlt, pLib + 12);
			pScript = (KLuaScript*)g_GetScript(szAlt);
		}
	}
	if (!pScript || !pScript->m_LuaState)
	{
		g_DebugLog((LPSTR)"[PORT5] RemoteExecute: script chua nap %.128s", szLow);
		return 0;
	}
	const char* szCb = (Lua_GetTopIndex(L) >= 4 && Lua_IsString(L, 4)) ? Lua_ValueToString(L, 4) : NULL;
	int nCbId = (Lua_GetTopIndex(L) >= 5 && Lua_IsNumber(L, 5)) ? (int)Lua_ValueToNumber(L, 5) : 0;
	KJx2ObjBuffer* pRes = new KJx2ObjBuffer;
	pRes->nWrite = 0;
	pRes->nRead = 0;
	int hRes = ++s_nOBNextHandle;
	s_OBMap[hRes] = pRes;
	char szCall[600];
	sprintf(szCall, "%s(%d,%d,0)", szFunc, nHandle, hRes);
	int nTop0 = 0;
	pScript->SafeCallBegin(&nTop0);
	if (lua_dostring(pScript->m_LuaState, szCall) != 0)
		g_DebugLog((LPSTR)"[PORT5] RemoteExecute LOI: %.128s -> %.200s", szLow, szCall);
	pScript->SafeCallEnd(nTop0);
	if (szCb && szCb[0])
	{
		sprintf(szCall, "%s(%d,%d)", szCb, nCbId, hRes);
		int nTopL = lua_gettop(L);
		if (lua_dostring(L, szCall) != 0)
			g_DebugLog((LPSTR)"[PORT5] RemoteExecute cb LOI: %.200s", szCall);
		lua_settop(L, nTopL);
	}
	{
		std::map<int, KJx2ObjBuffer*>::iterator itR = s_OBMap.find(hRes);
		if (itR != s_OBMap.end())
		{
			delete itR->second;
			s_OBMap.erase(itR);
		}
	}
	Lua_PushNumber(L, 1);
	return 1;
}

// [PORT5 23/08] OB_Save/LoadShareData (relay s3relay_y 0x08102F54 / 0x08102D58): Linux luu MySQL
// bang ShareData khoa (ShareKey, Param1, Param2). Du an persist ra tep
// \\settings\\jx2sharedata\\<key>_<p1>_<p2>.bin theo khuon tmp + MoveFileEx cua Ladder.
// tongcastle luu diem bang/nguoi choi + trang thai cay than.
static void sShareDataPath(char* szOut, const char* szKey, int nP1, int nP2, bool bTmp)
{
	char szRoot[MAX_PATH];
	g_GetRootPath(szRoot);
	char szSafe[64];
	int i = 0;
	for (; szKey[i] && i < 60; i++)
	{
		char c = szKey[i];
		if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-' || c == '_')
			szSafe[i] = c;
		else
			szSafe[i] = '_';
	}
	szSafe[i] = 0;
	sprintf(szOut, "%s\\\\settings\\\\jx2sharedata", szRoot);
	CreateDirectory(szOut, NULL);
	sprintf(szOut, "%s\\\\settings\\\\jx2sharedata\\\\%s_%d_%d.bin%s", szRoot, szSafe, nP1, nP2, bTmp ? ".tmp" : "");
}

int LuaOB_SaveShareData(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 4 || !Lua_IsString(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	KJx2ObjBuffer* p = sOBGet(L, 1);
	const char* szKey = Lua_ValueToString(L, 2);
	if (!p || !szKey || !szKey[0])
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nP1 = (int)Lua_ValueToNumber(L, 3);
	int nP2 = (int)Lua_ValueToNumber(L, 4);
	char szTmp[MAX_PATH], szPath[MAX_PATH];
	sShareDataPath(szTmp, szKey, nP1, nP2, true);
	sShareDataPath(szPath, szKey, nP1, nP2, false);
	FILE* f = fopen(szTmp, "wb");
	if (!f)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	if (p->nWrite > 0)
		fwrite(p->Buf, 1, p->nWrite, f);
	fclose(f);
	MoveFileEx(szTmp, szPath, MOVEFILE_REPLACE_EXISTING);
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaOB_LoadShareData(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 4 || !Lua_IsString(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	KJx2ObjBuffer* p = sOBGet(L, 1);
	const char* szKey = Lua_ValueToString(L, 2);
	if (!p || !szKey || !szKey[0])
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nP1 = (int)Lua_ValueToNumber(L, 3);
	int nP2 = (int)Lua_ValueToNumber(L, 4);
	p->nWrite = 0;
	p->nRead = 0;
	char szPath[MAX_PATH];
	sShareDataPath(szPath, szKey, nP1, nP2, false);
	FILE* f = fopen(szPath, "rb");
	if (f)
	{
		p->nWrite = (int)fread(p->Buf, 1, JX2OB_BUF_SIZE, f);
		fclose(f);
	}
	// khong co tep -> buffer rong (OB_IsEmpty == 1) nhu Linux khong co ban ghi
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaOB_IsEmpty(Lua_State* L)
""")
assert hi(s) == h; wr(p, s); print("KJx2SharedStore.cpp ok")

# ============================================================================
# 14) KItemList.cpp - sIsJx2ItemScript + 2 duong dan item tongwar
# ============================================================================
p = SRC + r"\KItemList.cpp"; s = rd(p); h = hi(s)
s = rep1(s, "\t\t\"\\\\script\\\\item\\\\event\\\\kinhmach\\\\honnguyenchandon.lua\",\t// Hon nguyen chan don (tra 0)\n",
            "\t\t\"\\\\script\\\\item\\\\event\\\\kinhmach\\\\honnguyenchandon.lua\",\t// Hon nguyen chan don (tra 0)\n\t\t\"\\\\script\\\\item\\\\huangzhendan.lua\",\t\t\t\t\t\t// [PORT5 23/08] Hoang Chan Don (tongwar) - tu Remove + return\n\t\t\"\\\\script\\\\vng_event\\\\item\\\\biggoldenseed.lua\",\t\t\t// [PORT5 23/08] Qua Dai Hoang Kim (tongwar)\n")
assert hi(s) == h; wr(p, s); print("KItemList.cpp ok")

# ============================================================================
# 15) ScriptFuns.cpp - CallPlayerFunction MULTRET, DynamicExecute [[...]],
#     AddMapTrap tham so 5, AddNpcEx bNoRevive, cac ham moi + extern + dang ky
# ============================================================================
p = SRC + r"\ScriptFuns.cpp"; s = rd(p); h = hi(s)

# 15.1 CallPlayerFunction tra ket qua
s = rep1(s, """	lua_rawcall(L, nArgs, 0);
	// khoi phuc PlayerIndex cu (van nam o nOldTop)
	lua_pushvalue(L, nOldTop);
	lua_setglobal(L, SCRIPT_PLAYERINDEX);
	return 0;
}
""", """	// [PORT5 23/08] Linux tra MOI ket qua cua pFun (bairenleitai: szName = CallPlayerFunction(idx,
	// GetName); so sanh ST_GetDamageCounter). Ket qua nam tren gia tri cu o nOldTop; khi C tra
	// nRes, Lua lay dung nRes gia tri tren cung. Caller cu khong doc ket qua -> vo hai.
	lua_rawcall(L, nArgs, LUA_MULTRET);
	int nRes = lua_gettop(L) - nOldTop;
	if (nRes < 0)
		nRes = 0;
	// khoi phuc PlayerIndex cu (van nam o nOldTop)
	lua_pushvalue(L, nOldTop);
	lua_setglobal(L, SCRIPT_PLAYERINDEX);
	return nRes;
}
""")

# 15.2 DynamicExecute / DynamicExecuteByPlayer: buffer 4096 + tham so chuoi [[...]]
OLD_BUF = """	char szCall[512];
	int nPos = 0;
	nPos += sprintf(szCall + nPos, "%s(", szFun);
"""
NEW_BUF = """	char szCall[4096];	// [PORT5 23/08] 512 -> 4096: RemoteExc chuyen thong bao ~150 byte + du phong
	int nPos = 0;
	nPos += sprintf(szCall + nPos, "%s(", szFun);
"""
s = repn(s, OLD_BUF, NEW_BUF, 2)
OLD_ARG = """		else if (Lua_IsString(L, i))
			nPos += sprintf(szCall + nPos, "\\"%.64s\\"", Lua_ValueToString(L, i));
"""
NEW_ARG = """		else if (Lua_IsString(L, i))
		{
			// [PORT5 23/08] khong cat 64 byte, chiu duoc dau nhay kep: dung [[...]] (long string
			// Lua 4 - llex.c chi ho tro dang nay); "]]" trong chuoi -> "] ]" de khong dong som
			char szArgTmp[2049];
			g_StrCpyLen(szArgTmp, (char*)Lua_ValueToString(L, i), sizeof(szArgTmp));
			for (char* q = strstr(szArgTmp, "]]"); q; q = strstr(q + 2, "]]"))
				q[1] = ' ';
			nPos += sprintf(szCall + nPos, "[[%s]]", szArgTmp);
		}
"""
s = repn(s, OLD_ARG, NEW_ARG, 2)
OLD_GUARD1 = "for (int i = 4; i <= nParamNum && nPos < (int)sizeof(szCall) - 80; i++)"
NEW_GUARD1 = "for (int i = 4; i <= nParamNum && nPos < (int)sizeof(szCall) - 2200; i++)"
assert s.count(OLD_GUARD1) == 1; s = s.replace(OLD_GUARD1, NEW_GUARD1)
OLD_GUARD2 = "for (int i = 3; i <= nParamNum && nPos < (int)sizeof(szCall) - 80; i++)"
NEW_GUARD2 = "for (int i = 3; i <= nParamNum && nPos < (int)sizeof(szCall) - 2200; i++)"
assert s.count(OLD_GUARD2) == 1; s = s.replace(OLD_GUARD2, NEW_GUARD2)

# 15.3 AddMapTrap tham so 5
s = rep1(s, """	SubWorld[nSubWorldIndex].SetTrap(dwTrapID, nMpsX, nMpsY, 0);
	Lua_PushNumber(L, 1);
	return 1;
}
""", """	SubWorld[nSubWorldIndex].SetTrap(dwTrapID, nMpsX, nMpsY, 0);
	// [PORT5 23/08] tham so 5 (Linux 0x08102700 - trap JX2 = {scriptId, nParam}): script trap
	// nhan main(nParam); thieu tham so -> 0 nhu Linux. Trap JX1 (AddTrap/map-data) van
	// main(nPlayerIdx) vi param mac dinh JX2TRAP_PARAM_NONE.
	int nTrapParam = (nTop >= 5 && Lua_IsNumber(L, 5)) ? (int)Lua_ValueToNumber(L, 5) : 0;
	SubWorld[nSubWorldIndex].SetTrapParam(nMpsX, nMpsY, nTrapParam);
	Lua_PushNumber(L, 1);
	return 1;
}
""")

# 15.4 cac ham moi - chen ngay sau LuaAddMapTrap (van trong #ifdef _SERVER)
s = rep1(s, """int LuaAddObj(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 5)
""", """// ============================================================================
// [PORT5 23/08] tongwar / bairenleitai / tongcastle - xem SPEC_PORT_*.md muc C
// ============================================================================

// SetDeathType(n) - Linux 0x08110580 ghi Player+0xE4 (0 = thuong; -1 = che do dac biet khong
// phat; N>0 = N lan mien). Cay tongwar LUON goi kem SetPunish cung nghia -> chi can LUU.
int LuaSetDeathType(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || Player[nPlayerIndex].m_nIndex <= 0)
		return 0;
	Player[nPlayerIndex].m_nJX2DeathType = (int)Lua_ValueToNumber(L, 1);
	return 0;
}

// GetAllEquipment() -> bang 1..itempart_num chi so Item cua o trang bi (ke ca o trong = 0).
// Linux 0x0810D0F0 tra 15 o; JX1 co 17 o - script chi tim mat na nen tra du 17.
int LuaGetAllEquipment(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	Lua_NewTable(L);
	for (int i = 0; i < itempart_num; i++)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.GetEquipment(i));
		Lua_RawSetI(L, -2, i + 1);
	}
	return 1;
}

// GetMapInfoFile(nMapId) -> chuoi khoa <id>_MapInfo cua MapList.ini (Linux 0x081024E0,
// KSubWorld+0x4EFC4). gettop phai == 1 nhu Linux; loi -> chuoi rong.
int LuaGetMapInfoFile(Lua_State* L)
{
	if (Lua_GetTopIndex(L) != 1 || !Lua_IsNumber(L, 1))
	{
		Lua_PushString(L, (char*)"");
		return 1;
	}
	int w = g_SubWorldSet.SearchWorld((DWORD)Lua_ValueToNumber(L, 1));
	if (w < 0 || w >= MAX_SUBWORLD)
	{
		Lua_PushString(L, (char*)"");
		return 1;
	}
	Lua_PushString(L, SubWorld[w].m_szMapInfoFile);
	return 1;
}

// ST_SyncMiniMapObj(x, y) - Linux 0x081C1930 gui goi 0xB4 (ky hieu ban do nho client JX2).
// Client JX1 khong co protocol nay; cay tongwar chi goi (-1,-1) = go ky hieu -> stub.
int LuaST_SyncMiniMapObj(Lua_State* L)
{
	return 0;
}

// CreateChannel/EnterChannel/LeaveChannel/DeleteChannel - he kenh chat tam JX2 (Linux
// 0x081045A0/0x081044C0/0x08104420/0x08104560 -> KChannelMgr). Client JX1 khong co kenh
// dong -> stub co chu dich (chi mat chat rieng theo phe; PHANTICH 5.2 da chot bo).
int LuaJx2ChannelStub(Lua_State* L)
{
	return 0;
}

// SetTmpCamp(nCamp[, nNpcIdx]) -> 1/0 ; GetTmpCamp([nNpcIdx]) - Linux 0x0810BA50/0x0810BB70
// ghi/doc KNpc+0x1900 (trai tam thoi, luat quan he o KNpcSet::GetRelation). Linux con gui goi
// 0xD1 dong bo client - JX1 khong co goi nay; client tu tinh quan he bang CurCamp/FightMode/
// PKFlag ma script da dat du (hundred_arena 485/591-593).
int LuaSetTmpCamp(Lua_State* L)
{
	int nTop = Lua_GetTopIndex(L);
	if (nTop < 1 || !Lua_IsNumber(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nCamp = (int)Lua_ValueToNumber(L, 1);
	if (nCamp < 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nNpcIdx = 0;
	if (nTop >= 2 && Lua_IsNumber(L, 2))
		nNpcIdx = (int)Lua_ValueToNumber(L, 2);
	else
	{
		int nPlayerIdx = GetPlayerIndex(L);
		if (nPlayerIdx <= 0)
		{
			Lua_PushNumber(L, 0);
			return 1;
		}
		nNpcIdx = Player[nPlayerIdx].m_nIndex;
	}
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
#ifdef _SERVER
	Npc[nNpcIdx].m_nTmpCamp = nCamp;
#endif
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaGetTmpCamp(Lua_State* L)
{
	int nNpcIdx = 0;
	if (Lua_GetTopIndex(L) >= 1 && Lua_IsNumber(L, 1))
		nNpcIdx = (int)Lua_ValueToNumber(L, 1);
	else
	{
		int nPlayerIdx = GetPlayerIndex(L);
		if (nPlayerIdx <= 0)
			return 0;
		nNpcIdx = Player[nPlayerIdx].m_nIndex;
	}
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
		return 0;
#ifdef _SERVER
	Lua_PushNumber(L, Npc[nNpcIdx].m_nTmpCamp);
#else
	Lua_PushNumber(L, 0);
#endif
	return 1;
}

// MakeDateTime([nCount[, nTime]]) -> nCount gia tri {nam, thang, ngay, gio, phut, giay,
// thu (0=CN), ngay-trong-nam} - Linux 0x081038A0 (mac dinh 6; epoch tuy chon).
int LuaMakeDateTime(Lua_State* L)
{
	int nTop = Lua_GetTopIndex(L);
	int nCount = 6;
	if (nTop >= 1)
	{
		nCount = (int)Lua_ValueToNumber(L, 1);
		if (nCount < 1 || nCount > 8)
			return 0;
	}
	time_t tVal = time(NULL);
	if (nTop >= 2 && Lua_IsNumber(L, 2))
		tVal = (time_t)Lua_ValueToNumber(L, 2);
	struct tm* pTm = localtime(&tVal);
	if (!pTm)
		return 0;
	int v[8];
	v[0] = pTm->tm_year + 1900;
	v[1] = pTm->tm_mon + 1;
	v[2] = pTm->tm_mday;
	v[3] = pTm->tm_hour;
	v[4] = pTm->tm_min;
	v[5] = pTm->tm_sec;
	v[6] = pTm->tm_wday;
	v[7] = pTm->tm_yday + 1;
	for (int i = 0; i < nCount; i++)
		Lua_PushNumber(L, v[i]);
	return nCount;
}

// NPCINFO_GetNpcCurrentLife / NPCINFO_SetNpcCurrentLife - Linux 0x081C06B0/0x081C0070:
// doc/ghi m_CurrentLife (KHONG dong max nhu SetNpcLife cua JX1 - SetNpcLife ghi ca 2 =
// nang tran mau cay moi lan hoi). Set kep [1, m_CurrentLifeMax].
int LuaNPCINFO_GetNpcCurrentLife(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1 || !Lua_IsNumber(L, 1))
		return 0;
	int n = (int)Lua_ValueToNumber(L, 1);
	if (n <= 0 || n >= MAX_NPC)
		return 0;
	Lua_PushNumber(L, Npc[n].m_CurrentLife);
	return 1;
}

int LuaNPCINFO_SetNpcCurrentLife(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2 || !Lua_IsNumber(L, 1) || !Lua_IsNumber(L, 2))
		return 0;
	int n = (int)Lua_ValueToNumber(L, 1);
	int v = (int)Lua_ValueToNumber(L, 2);
	if (n <= 0 || n >= MAX_NPC)
		return 0;
	if (v > Npc[n].m_CurrentLifeMax)
		v = Npc[n].m_CurrentLifeMax;
	if (v < 0)
		v = 0;
	Npc[n].m_CurrentLife = v;
	return 0;
}

int LuaAddObj(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 5)
""")

# 15.5 AddNpcEx tham so 7: them bNoRevive (giu SetCurrentCamp cu)
s = rep1(s, """	// tham so 7: phe NPC (ban goc la mot co byte rieng - xem chu thich tren)
	if (nTop >= 7 && Lua_IsNumber(L, 7))
	{
		int nCamp = (int)Lua_ValueToNumber(L, 7);
		if (nCamp >= 0 && nCamp < camp_num)
			Npc[nNpcIdx].SetCurrentCamp(nCamp);
	}
""", """	// tham so 7: phe NPC (ban goc la mot co byte rieng - xem chu thich tren)
	if (nTop >= 7 && Lua_IsNumber(L, 7))
	{
		int nCamp = (int)Lua_ValueToNumber(L, 7);
		if (nCamp >= 0 && nCamp < camp_num)
			Npc[nNpcIdx].SetCurrentCamp(nCamp);
#ifdef _SERVER
		// [PORT5 23/08] nghia goc Linux cua tham so 7 = bNoRevive (KNpc+0x1824): chet la bien
		// mat, khong hoi sinh (0x080833E2/0x08083520). Giu nhanh SetCurrentCamp o tren de khong
		// doi hanh vi da chay; moi caller la script JX2 nen != 0 <=> bNoRevive that.
		if ((int)Lua_ValueToNumber(L, 7) != 0)
			Npc[nNpcIdx].m_bNoRevive = 1;
#endif
	}
""")

# 15.6 extern cho cac ham o tep khac
s = rep1(s, "extern int LuaGetAroundNpcList(Lua_State* L);\t// [TIN SU 21/08] KJx2WarInfra.cpp\n",
            """extern int LuaGetAroundNpcList(Lua_State* L);	// [TIN SU 21/08] KJx2WarInfra.cpp
extern int LuaClearMapTrap(Lua_State* L);			// [PORT5 23/08] KJx2WarInfra.cpp
extern int LuaGetItemStackCount(Lua_State* L);		// [PORT5 23/08] KJx2WarInfra.cpp
extern int LuaSetItemStackCount(Lua_State* L);		// [PORT5 23/08] KJx2WarInfra.cpp
extern int LuaGetNpcAroundNpcList(Lua_State* L);	// [PORT5 23/08] KJx2WarInfra.cpp
extern int LuaJX2_AddTimer(Lua_State* L);			// [PORT5 23/08] KJx2League.cpp
extern int LuaJX2_DelTimer(Lua_State* L);			// [PORT5 23/08] KJx2League.cpp
extern int LuaJX2_SuspendTimer(Lua_State* L);		// [PORT5 23/08] KJx2League.cpp
extern int LuaJX2_ResumeTimer(Lua_State* L);		// [PORT5 23/08] KJx2League.cpp
extern int LuaJX2_RemoteExecute(Lua_State* L);		// [PORT5 23/08] KJx2SharedStore.cpp
extern int LuaOB_SaveShareData(Lua_State* L);		// [PORT5 23/08] KJx2SharedStore.cpp
extern int LuaOB_LoadShareData(Lua_State* L);		// [PORT5 23/08] KJx2SharedStore.cpp
""")

# 15.7 dang ky - truoc #endif cua khoi JX2
s = rep1(s, """		{ "CTC_JX2_SetTax",	LuaCTC_JX2_SetTax },
		{ "CTC_JX2_GetTax",	LuaCTC_JX2_GetTax },
#endif
""", """		{ "CTC_JX2_SetTax",	LuaCTC_JX2_SetTax },
		{ "CTC_JX2_GetTax",	LuaCTC_JX2_GetTax },
		// ==== [PORT5 23/08] tongwar / bairenleitai / bw / tongcastle ====
		{ "SetDeathType",	LuaSetDeathType },	// tongwar (Linux 0x08110580) - luu, cay luon kem SetPunish
		{ "GetMapInfoFile",	LuaGetMapInfoFile },	// tongwar (0x081024E0)
		{ "ClearMapTrap",	LuaClearMapTrap },	// tongwar (0x08102AC0) KJx2WarInfra.cpp
		{ "GetAllEquipment",	LuaGetAllEquipment },	// tongwar (0x0810D0F0)
		{ "ST_SyncMiniMapObj",	LuaST_SyncMiniMapObj },	// tongwar: stub (goi 0xB4 JX2; cay chi go ky hieu)
		{ "CreateChannel",	LuaJx2ChannelStub },	// tongwar/bw: kenh chat dong JX2 - stub
		{ "EnterChannel",	LuaJx2ChannelStub },
		{ "LeaveChannel",	LuaJx2ChannelStub },
		{ "DeleteChannel",	LuaJx2ChannelStub },
		{ "Msg2Map",	LuaMsgToAroundRegion },	// bairenleitai/tongcastle (0x08105080) = Msg2Region
		{ "SetTmpCamp",	LuaSetTmpCamp },	// bairenleitai/tongcastle (0x0810BA50)
		{ "GetTmpCamp",	LuaGetTmpCamp },	// (0x0810BB70)
		{ "AddTimer",	LuaJX2_AddTimer },	// bairenleitai/tongcastle (0x08100D40) KJx2League.cpp
		{ "DelTimer",	LuaJX2_DelTimer },	// (0x08100CA0)
		{ "SuspendTimer",	LuaJX2_SuspendTimer },	// (0x08100C00)
		{ "ResumeTimer",	LuaJX2_ResumeTimer },	// (0x08100B60)
		{ "RemoteExecute",	LuaJX2_RemoteExecute },	// (0x08100740) 1 GS thuc thi tai cho - KJx2SharedStore.cpp
		{ "OB_SaveShareData",	LuaOB_SaveShareData },	// tongcastle (relay 0x08102F54) persist settings\\jx2sharedata
		{ "OB_LoadShareData",	LuaOB_LoadShareData },	// (0x08102D58)
		{ "GetNpcId",	LuaGetNpcID },	// tongcastle: alias GetNpcID (0x080FD920)
		{ "NpcCastSkill",	LuaNpcCastSkill },	// tongcastle: alias CastNpcSkill (0x0812B3A0; x,y thua = vi tri NPC)
		{ "GetItemStackCount",	LuaGetItemStackCount },	// tongcastle (0x080FD250) KJx2WarInfra.cpp
		{ "SetItemStackCount",	LuaSetItemStackCount },	// (0x0810D9A0)
		{ "MakeDateTime",	LuaMakeDateTime },	// tongcastle (0x081038A0)
		{ "GetNpcAroundNpcList",	LuaGetNpcAroundNpcList },	// tongcastle (0x08104A20) KJx2WarInfra.cpp
		{ "NPCINFO_GetNpcCurrentLife",	LuaNPCINFO_GetNpcCurrentLife },	// tongcastle (0x081C06B0)
		{ "NPCINFO_SetNpcCurrentLife",	LuaNPCINFO_SetNpcCurrentLife },	// (0x081C0070)
#endif
""")
assert hi(s) == h; wr(p, s); print("ScriptFuns.cpp ok")

print("ALL OK - 15 tep da va")
