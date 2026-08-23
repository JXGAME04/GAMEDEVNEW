# -*- coding: utf-8 -*-
r"""[PORT5 23/08 - hau phan bien engine] 3 va:
F6 (CAO): GetNpcAroundNpcList + GetAroundNpcList so KHOANG CACH bang toa do CUC BO region
    (m_MapX 0-15 / m_MapY 0-31) khi quet MOI region -> sai hoan toan giua cac region.
    Quy ve toa do toan cuc theo cong thuc KNpc::GetMapDisX/Y (KNpc.cpp:5353):
    G = LOWORD/HIWORD(Region.m_RegionID) * m_nRegionWidth/Height + m_MapX/Y.
F7 (TRUNG): AddNpcEx tham so 7 bat m_bNoRevive VO DIEU KIEN - 3 script JX1 SONG truyen 1
    (bosscharm.lua:99 boss bang hoi, seasonnpc_item.lua:57, spider_web.lua:42) -> boss het
    hoi sinh = doi hanh vi content dang chay. Gate bang g_IsJx2Script(L); them tien to
    basemission vao danh sach JX2 (phong khi lib chay trong state rieng).
F8 (THAP): tham so chuoi [[...]] cua DynamicExecute*: khu them "[[" -> "[ [" va chuoi ket
    thuc ']' -> chen 1 dau cach truoc "]]" (ten nguoi choi "Bao[GM]" khong pha cau goi).
"""
import io, re

SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hi(s): return sum(1 for c in s if ord(c) > 127)
def rep1(s, old, new, tag=""):
    o = old.replace("\r\n", "\n")
    for cand in ("\r\n", "\n"):
        o2 = o.replace("\n", cand)
        if s.count(o2) == 1:
            return s.replace(o2, new.replace("\r\n", "\n").replace("\n", cand), 1)
    assert False, "rep1 %s" % (tag or old[:80])
def repn(s, old, new, n, tag=""):
    o = old.replace("\r\n", "\n")
    for cand in ("\r\n", "\n"):
        o2 = o.replace("\n", cand)
        if s.count(o2) == n:
            return s.replace(o2, new.replace("\r\n", "\n").replace("\n", cand))
    assert False, "repn %s (%d)" % (tag or old[:80], n)

# ============================================================
# F6: KJx2WarInfra.cpp - toa do toan cuc cho 2 ham quet quanh
# ============================================================
p = SRC + r"\KJx2WarInfra.cpp"; s = rd(p); h = hi(s)

# 6a. LuaGetAroundNpcList (tam = nguoi choi)
s = rep1(s, """	KSubWorld* pWorld = &SubWorld[Npc[nMe].m_SubWorldIndex];
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
			if (Npc[i].IsPlayer() || Npc[i].GetPlayerIdx() > 0)
				continue;
			if (Npc[i].m_Doing == do_death || Npc[i].m_Doing == do_revive)
				continue;
			int dx = Npc[i].m_MapX - nMX, dy = Npc[i].m_MapY - nMY;
			if (dx * dx + dy * dy > nDist * nDist)""",
"""	KSubWorld* pWorld = &SubWorld[Npc[nMe].m_SubWorldIndex];
	// [PORT5 23/08 phan bien F6] m_MapX/m_MapY la toa do CUC BO region (0..15/0..31) - so giua
	// cac region phai quy ve toa do toan cuc (khuon KNpc::GetMapDisX/Y KNpc.cpp:5353):
	// G = LOWORD/HIWORD(m_RegionID) * m_nRegionWidth/Height + m_MapX/Y.
	if (Npc[nMe].m_RegionIndex < 0)
	{
		Lua_PushNumber(L, 0);
		return 2;
	}
	int nMX = LOWORD(pWorld->m_Region[Npc[nMe].m_RegionIndex].m_RegionID) * pWorld->m_nRegionWidth + Npc[nMe].m_MapX;
	int nMY = HIWORD(pWorld->m_Region[Npc[nMe].m_RegionIndex].m_RegionID) * pWorld->m_nRegionHeight + Npc[nMe].m_MapY;
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
			if (Npc[i].IsPlayer() || Npc[i].GetPlayerIdx() > 0)
				continue;
			if (Npc[i].m_Doing == do_death || Npc[i].m_Doing == do_revive)
				continue;
			int dx = LOWORD(pRegion->m_RegionID) * pWorld->m_nRegionWidth + Npc[i].m_MapX - nMX;
			int dy = HIWORD(pRegion->m_RegionID) * pWorld->m_nRegionHeight + Npc[i].m_MapY - nMY;
			if (dx * dx + dy * dy > nDist * nDist)""", "F6a")

# 6b. LuaGetNpcAroundNpcList (tam = NPC bat ky)
s = rep1(s, """	KSubWorld* pWorld = &SubWorld[Npc[nMe].m_SubWorldIndex];
	int nMX = Npc[nMe].m_MapX, nMY = Npc[nMe].m_MapY;
	for (int r = 0; r < pWorld->m_nTotalRegion; r++)""",
"""	KSubWorld* pWorld = &SubWorld[Npc[nMe].m_SubWorldIndex];
	// [PORT5 23/08 phan bien F6] toa do toan cuc (xem LuaGetAroundNpcList)
	if (Npc[nMe].m_RegionIndex < 0)
	{
		Lua_PushNumber(L, 0);
		return 2;
	}
	int nMX = LOWORD(pWorld->m_Region[Npc[nMe].m_RegionIndex].m_RegionID) * pWorld->m_nRegionWidth + Npc[nMe].m_MapX;
	int nMY = HIWORD(pWorld->m_Region[Npc[nMe].m_RegionIndex].m_RegionID) * pWorld->m_nRegionHeight + Npc[nMe].m_MapY;
	for (int r = 0; r < pWorld->m_nTotalRegion; r++)""", "F6b")

# 6b-tiep: vong ung vien cua LuaGetNpcAroundNpcList
s = rep1(s, """			if (Npc[i].m_Doing == do_death || Npc[i].m_Doing == do_revive)
				continue;
			int dx = Npc[i].m_MapX - nMX, dy = Npc[i].m_MapY - nMY;
			if (dx * dx + dy * dy > nDist * nDist)
				continue;
			Lua_PushNumber(L, i);
			Lua_RawSetI(L, -2, ++nCount);""",
"""			if (Npc[i].m_Doing == do_death || Npc[i].m_Doing == do_revive)
				continue;
			int dx = LOWORD(pRegion->m_RegionID) * pWorld->m_nRegionWidth + Npc[i].m_MapX - nMX;
			int dy = HIWORD(pRegion->m_RegionID) * pWorld->m_nRegionHeight + Npc[i].m_MapY - nMY;
			if (dx * dx + dy * dy > nDist * nDist)
				continue;
			Lua_PushNumber(L, i);
			Lua_RawSetI(L, -2, ++nCount);""", "F6b2")
assert hi(s) == h; wr(p, s); print("KJx2WarInfra.cpp F6 ok")

# ============================================================
# F7: ScriptFuns.cpp AddNpcEx - gate bNoRevive theo g_IsJx2Script
# ============================================================
p = SRC + r"\ScriptFuns.cpp"; s = rd(p); h = hi(s)
s = rep1(s, """#ifdef _SERVER
		// [PORT5 23/08] nghia goc Linux cua tham so 7 = bNoRevive (KNpc+0x1824): chet la bien
		// mat, khong hoi sinh (0x080833E2/0x08083520). Giu nhanh SetCurrentCamp o tren de khong
		// doi hanh vi da chay; moi caller la script JX2 nen != 0 <=> bNoRevive that.
		if ((int)Lua_ValueToNumber(L, 7) != 0)
			Npc[nNpcIdx].m_bNoRevive = 1;
#endif""",
"""#ifdef _SERVER
		// [PORT5 23/08 phan bien F7] nghia goc Linux cua tham so 7 = bNoRevive (+0x1824): chet la
		// bien mat. CHI ap cho script JX2 (g_IsJx2Script) - 3 script JX1 dang song cung truyen 1
		// o vi tri nay (bosscharm.lua:99 boss bang hoi, seasonnpc_item.lua:57, spider_web.lua:42)
		// va dua vao hoi sinh cu; giu nguyen hanh vi JX1 cho chung.
		if (g_IsJx2Script(L) && (int)Lua_ValueToNumber(L, 7) != 0)
			Npc[nNpcIdx].m_bNoRevive = 1;
#endif""", "F7")

# ============================================================
# F8: 2 khoi boc chuoi [[...]] - khu "[[" + duoi ']'
# ============================================================
OLD = """		else if (Lua_IsString(L, i))
		{
			// [PORT5 23/08] khong cat 64 byte, chiu duoc dau nhay kep: dung [[...]] (long string
			// Lua 4 - llex.c chi ho tro dang nay); "]]" trong chuoi -> "] ]" de khong dong som
			char szArgTmp[2049];
			g_StrCpyLen(szArgTmp, (char*)Lua_ValueToString(L, i), sizeof(szArgTmp));
			for (char* q = strstr(szArgTmp, "]]"); q; q = strstr(q + 2, "]]"))
				q[1] = ' ';
			nPos += sprintf(szCall + nPos, "[[%s]]", szArgTmp);
		}"""
NEW = """		else if (Lua_IsString(L, i))
		{
			// [PORT5 23/08] khong cat 64 byte, chiu duoc dau nhay kep: dung [[...]] (long string
			// Lua 4); "]]" -> "] ]" va "[[" -> "[ [" de khong dong som/mo long; duoi ']' don
			// chen 1 dau cach truoc khi dong (phan bien F8: ten kieu "Bao[GM]").
			char szArgTmp[2049];
			g_StrCpyLen(szArgTmp, (char*)Lua_ValueToString(L, i), sizeof(szArgTmp));
			for (char* q = strstr(szArgTmp, "]]"); q; q = strstr(q + 2, "]]"))
				q[1] = ' ';
			for (char* q2 = strstr(szArgTmp, "[["); q2; q2 = strstr(q2 + 2, "[["))
				q2[1] = ' ';
			int nTailFix = (szArgTmp[0] && szArgTmp[strlen(szArgTmp) - 1] == ']') ? 1 : 0;
			nPos += sprintf(szCall + nPos, nTailFix ? "[[%s ]]" : "[[%s]]", szArgTmp);
		}"""
s = repn(s, OLD, NEW, 2, "F8")
assert hi(s) == h; wr(p, s); print("ScriptFuns.cpp F7+F8 ok")

# ============================================================
# F7b: KSortScript.cpp - them tien to basemission (phong ve)
# ============================================================
p = SRC + r"\KSortScript.cpp"; s = rd(p); h = hi(s)
s = rep1(s, '"\\\\script\\\\global\\\\npcchucnang\\\\dichquan.lua",\t// Dich Quan 7 thanh: Include posthouse.lua (Tin Su)',
            '"\\\\script\\\\global\\\\npcchucnang\\\\dichquan.lua",\t// Dich Quan 7 thanh: Include posthouse.lua (Tin Su)\n\t\t"\\\\script\\\\missions\\\\basemission\\\\",\t// [PORT5 23/08 phan bien F7] lib JX2 (CallNpc bNoRevive)', "F7b")
assert hi(s) == h; wr(p, s); print("KSortScript.cpp F7b ok")
print("ALL OK")
