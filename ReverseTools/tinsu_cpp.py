# -*- coding: utf-8 -*-
"""Engine cho Tin Su (21/08): GetAroundNpcList (KJx2WarInfra.cpp), ConsumeEquiproomItem + dang JX2 cua
ConsumeItem (ScriptFuns.cpp). latin-1, giu byte cao."""
import io, sys
T = "\t"
SF = r"D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp"
WI = r"D:\GAMEDEVNEW\Sources\Core\Src\KJx2WarInfra.cpp"
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hi(s): return sum(1 for c in s if ord(c) > 127)
def rep1(s, old, new):
    assert s.count(old) == 1, (s.count(old), old[:60])
    return s.replace(old, new, 1)

# ================= KJx2WarInfra.cpp: GetAroundNpcList =================
s = rd(WI); h = hi(s)
anchor = "// () -> nSubWorldId, nRevId cua diem hoi sinh BEN VUNG (m_sLoginRevivalPos -"
new = r"""// GetAroundNpcList(nDist[, nKind]) -> tbList, nCount : port Tin Su (21/08).
// Linux (item\xinshirenwu\che*fu.lua): "20 = khoang 1 man hinh", tra bang chi so NPC
// quanh nguoi choi de script tu loc GetNpcSettingIdx. nDist tinh bang O (32px, cung
// don vi GetWorldPos/m_MapX). Tham so 2 (Linux luon 8) KHONG loc gi o ta - quai
// muc tieu (849 Bao Kho Thu Ho Gia) co Kind = 0 trong npcs.txt nen 8 khong the la
// bo loc Kind; bo qua cho an toan. Bo qua nguoi choi + NPC dang chet.
int LuaGetAroundNpcList(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nCount = 0;
	Lua_NewTable(L);
	if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 2 || !Lua_IsNumber(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 2;
	}
	int nDist = (int)Lua_ValueToNumber(L, 1);
	int nMe = Player[nPlayerIndex].m_nIndex;
	if (nDist <= 0 || nMe <= 0 || nMe >= MAX_NPC || Npc[nMe].m_SubWorldIndex < 0)
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
			if (Npc[i].IsPlayer() || Npc[i].GetPlayerIdx() > 0)
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

"""
s = rep1(s, anchor, new + anchor)
assert hi(s) == h
wr(WI, s); print("KJx2WarInfra.cpp: + LuaGetAroundNpcList")

# ================= ScriptFuns.cpp =================
s = rd(SF); h = hi(s)
# (1) LuaConsumeItem: nhan them DANG JX2 ConsumeItem(nPos, nCount, g, d, p[, lvl])
old = (T*3 + "int nDelNum = (int)Lua_ValueToNumber(L, 1);\n\n"
       + T*3 + "if (nDelNum)\n")
new = (T*3 + "int nDelNum = (int)Lua_ValueToNumber(L, 1);\n\n"
       + T*3 + "// [TIN SU 21/08] DANG JX2: ConsumeItem(nPos, nCount, g, d, p[, lvl]) - nPos = 3 (pos_equiproom)\n"
       + T*3 + "// hoac -1 (tay + hanh trang + tui mo rong), nCount >= 1. Phan biet voi dang JX1\n"
       + T*3 + "// (nCount, nNature, g, d, p, ...) bang tham so 2: moi caller JX1 trong cay du an deu\n"
       + T*3 + "// truyen nNature = 0 (NATURE_NORMAL); tham so 2 != 0 chi co o script port JX2\n"
       + T*3 + "// (songjin_shophead.lua:139, xinshibaoxiang.lua:146/151, tong_springfestival\\head.lua:251\n"
       + T*3 + "// - truoc day 3 noi nay la NO-OP vi FindSameToRemove doi nature == 1/1000).\n"
       + T*3 + "if (nParamNum >= 5 && Lua_IsNumber(L, 2) && (int)Lua_ValueToNumber(L, 2) != 0\n"
       + T*4 + "&& (nDelNum == -1 || (nDelNum >= pos_hand && nDelNum < pos_num)))\n"
       + T*3 + "{\n"
       + T*4 + "int nPos = nDelNum;\n"
       + T*4 + "int nCnt = (int)Lua_ValueToNumber(L, 2);\n"
       + T*4 + "int g = (int)Lua_ValueToNumber(L, 3);\n"
       + T*4 + "int d = (int)Lua_ValueToNumber(L, 4);\n"
       + T*4 + "int p = (int)Lua_ValueToNumber(L, 5);\n"
       + T*4 + "int lv = nParamNum > 5 ? (int)Lua_ValueToNumber(L, 6) : -1;\n"
       + T*4 + "int nDone = 0;\n"
       + T*4 + "if (nCnt > 0)\n"
       + T*4 + "{\n"
       + T*5 + "if (nPos == -1 || nPos == pos_equiproom)\n"
       + T*5 + "{\n"
       + T*6 + "nDone += Player[nPlayerIndex].m_ItemList.RemoveCommonItem(nCnt - nDone, 0, g, d, p, lv, -1, pos_equiproom);\n"
       + T*6 + "if (nDone < nCnt)\n"
       + T*7 + "nDone += Player[nPlayerIndex].m_ItemList.RemoveCommonItem(nCnt - nDone, 0, g, d, p, lv, -1, pos_equiproomex);\n"
       + T*5 + "}\n"
       + T*5 + "else\n"
       + T*6 + "nDone += Player[nPlayerIndex].m_ItemList.RemoveCommonItem(nCnt - nDone, 0, g, d, p, lv, -1, nPos);\n"
       + T*5 + "if (nPos == -1 && nDone < nCnt)\n"
       + T*6 + "nDone += Player[nPlayerIndex].m_ItemList.RemoveCommonItem(nCnt - nDone, 0, g, d, p, lv, -1, pos_hand);\n"
       + T*4 + "}\n"
       + T*4 + "Lua_PushNumber(L, nDone);\n"
       + T*4 + "return 1;\n"
       + T*3 + "}\n\n"
       + T*3 + "if (nDelNum)\n")
s = rep1(s, old, new)

# (2) ConsumeEquiproomItem(nCount, g, d, p[, lvl]) -> so da tru : truoc LuaRemoveItemIdx
old = "int LuaRemoveItemIdx(Lua_State* L)\n{\n"
new = r"""// ConsumeEquiproomItem(nCount, g, d, p[, lvl]) -> so item da tru : port Tin Su (21/08).
// Linux (posthouse.lua:503, wuxingfu.lua:25) kiem "== 1". Chi hanh trang (+ tui mo rong);
// bang voi CalcEquiproomItemCount cung cap (WLLS). Khong dung tren quay (pos_hand).
int LuaConsumeEquiproomItem(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nDone = 0;
	if (nPlayerIndex > 0 && Lua_GetTopIndex(L) >= 4)
	{
		int nCnt = (int)Lua_ValueToNumber(L, 1);
		int g = (int)Lua_ValueToNumber(L, 2);
		int d = (int)Lua_ValueToNumber(L, 3);
		int p = (int)Lua_ValueToNumber(L, 4);
		int lv = Lua_GetTopIndex(L) >= 5 ? (int)Lua_ValueToNumber(L, 5) : -1;
		if (nCnt > 0)
		{
			nDone = Player[nPlayerIndex].m_ItemList.RemoveCommonItem(nCnt, 0, g, d, p, lv, -1, pos_equiproom);
			if (nDone < nCnt)
				nDone += Player[nPlayerIndex].m_ItemList.RemoveCommonItem(nCnt - nDone, 0, g, d, p, lv, -1, pos_equiproomex);
		}
	}
	Lua_PushNumber(L, nDone);
	return 1;
}

""" + old
s = rep1(s, old, new)

# (3) dang ky
old = T + '{"ConsumeItem",' + T*3 + 'LuaConsumeItem},\n'
new = old + T + '{"ConsumeEquiproomItem",' + T + 'LuaConsumeEquiproomItem},' + T + '// [TIN SU 21/08] chi hanh trang\n'
s = rep1(s, old, new)
old = "extern int LuaGetMapNpcWithName(Lua_State* L);\n"
new = old + "extern int LuaGetAroundNpcList(Lua_State* L);\t// [TIN SU 21/08] KJx2WarInfra.cpp\n"
s = rep1(s, old, new)
old = T*2 + '{ "GetMapNpcWithName",' + T + 'LuaGetMapNpcWithName },\n'
new = old + T*2 + '{ "GetAroundNpcList",' + T + 'LuaGetAroundNpcList },' + T + '// [TIN SU 21/08] tbList, nCount quanh nguoi choi\n'
s = rep1(s, old, new)
assert hi(s) == h and "\ufffd" not in s
wr(SF, s); print("ScriptFuns.cpp: ConsumeItem dang JX2 + ConsumeEquiproomItem + dang ky GetAroundNpcList")
