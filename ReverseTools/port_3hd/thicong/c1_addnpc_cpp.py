# -*- coding: ascii -*-
"""C1 - VA A7-C1 (CHAN): them 2 ham C++ mang DUNG ngu nghia AddNpc/AddNpcEx ban Linux.

VAN DE: LuaAddNpcEx cua JX1 lay tham so 7 lam CAMP va SetCurrentCamp ghi de camp
tu npcs.txt. GenOneRelation (KNpcSet.cpp:143): camp 0 (hoac camp trung voi nguoi
choi) => relation_ally => NPC thanh DONG MINH, khong danh duoc. 14 cho da doi
AddNpc->AddNpcEx deu truyen 0/1 o vi tri do => CA 3 hoat dong khong choi duoc,
va keo hong nguoc 9 NPC Ho Thu cua Tin Su (di qua killbosshead.lua:189).

Doi chat A7 #5: KHONG the va bang g_IsJx2Script vi HD3_DriverInit chay trong
STATE cua startgame.lua (Include = cung state), khong nam trong danh sach JX2.

CACH VA DUNG: 2 ham moi, KHONG dung den camp (giu nguyen tu npcs.txt nhu Linux):
  HD3_AddNpc(id, level, subworldIdx, x, y, bNoRevive[, name[, flag]])
      - dung thu tu Linux AddNpc; series TU RANDOM 0..4 trong C (nhu Linux rand()%5)
  HD3_AddNpcEx(id, level, series, subworldIdx, x, y, bNoRevive[, name[, isboss]])
      - dung thu tu Linux AddNpcEx; p7 = bNoRevive VO DIEU KIEN, khong camp
Dang ky ca hai trong khoi [3HD].
"""
import io, os, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

P = r"D:\GAMEDEVNEW\Sources\Core\Src\KJx2WarInfra.cpp"
d = io.open(P, encoding="latin-1", newline="").read()
if "LuaHD3_AddNpc" in d:
    print("da co - bo qua")
    raise SystemExit

anchor = "// ---------------------------------------------------------------------------\r\n// DisabledUseTownP(n) - THAY THE stub cu"
assert d.count(anchor) == 1, d.count(anchor)

lines = [
"// ---------------------------------------------------------------------------",
"// [VA 25/08 - A7-C1] HD3_AddNpc / HD3_AddNpcEx - dung ngu nghia ban Linux.",
"// LuaAddNpcEx cua JX1 lay tham so 7 lam CAMP va SetCurrentCamp GHI DE camp tu",
"// npcs.txt; GenOneRelation (KNpcSet.cpp:143) coi camp 0 / camp trung la ALLY",
"// => moi NPC 3 hoat dong thanh dong minh, khong danh duoc. Hai ham nay:",
"//   - KHONG dung den camp (giu nguyen camp tu npcs.txt - dung nhu Linux)",
"//   - HD3_AddNpc: series TU RANDOM 0..4 (Linux AddNpc goi rand()%5 noi bo,",
"//     0x0811BB10 tai 0x0811BC8C), tham so 6 = bNoRevive",
"//   - HD3_AddNpcEx: tham so 3 = series (script tu tinh), tham so 7 = bNoRevive",
"// Khong sua LuaAddNpc/LuaAddNpcEx goc de khoi dung cham cac he JX1/PORT5 dang chay.",
"static int sHD3_AddNpcCommon(Lua_State* L, int bHasSeries)",
"{",
"\tint nTop = Lua_GetTopIndex(L);",
"\tint nMin = bHasSeries ? 6 : 5;",
"\tif (nTop < nMin)",
"\t\treturn 0;",
"\tint nId = 0;",
"\tif (Lua_IsNumber(L, 1))",
"\t\tnId = (int)Lua_ValueToNumber(L, 1);",
"\telse if (Lua_IsString(L, 1))",
"\t{",
"\t\tconst char* pName = Lua_ValueToString(L, 1);",
"\t\tif (!pName || !pName[0])",
"\t\t\treturn 0;",
"\t\tnId = g_NpcSetting.FindRow((char*)pName) - 2;",
"\t}",
"\telse",
"\t\treturn 0;",
"\tif (nId < 0)",
"\t\tnId = 0;",
"\tint nLevel = (int)Lua_ValueToNumber(L, 2);",
"\tif (nLevel >= 128) nLevel = 127;",
"\tif (nLevel < 0)    nLevel = 1;",
"\tint nArg = 3;",
"\tint nSeries;",
"\tif (bHasSeries)",
"\t\tnSeries = (int)Lua_ValueToNumber(L, nArg++);",
"\telse",
"\t\tnSeries = g_Random(5);\t// Linux: rand() % 5",
"\tif (nSeries < 0 || nSeries > 4)",
"\t\tnSeries = g_Random(5);",
"\tint nSubWorldIdx = (int)Lua_ValueToNumber(L, nArg++);",
"\tint nX = (int)Lua_ValueToNumber(L, nArg++);",
"\tint nY = (int)Lua_ValueToNumber(L, nArg++);",
"\tint nNoRevive = (nTop >= nArg && Lua_IsNumber(L, nArg)) ? (int)Lua_ValueToNumber(L, nArg) : 0;",
"\tnArg++;",
"\tif (nSubWorldIdx < 0 || nSubWorldIdx >= MAX_SUBWORLD)",
"\t\treturn 0;",
"\tint nNpcIdxInfo = MAKELONG(nLevel, nId);",
"\tint nNpcIdx = NpcSet.AddNpcSet2(nNpcIdxInfo, nSeries, nSubWorldIdx, nX, nY);",
"\tif (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)",
"\t{",
"\t\tLua_PushNumber(L, nNpcIdx);",
"\t\treturn 1;",
"\t}",
"\tif (nNoRevive != 0)",
"\t\tNpc[nNpcIdx].m_bNoRevive = 1;",
"\t// tham so ke tiep: ten hien thi (chuoi khac rong)",
"\tif (nTop >= nArg && Lua_IsString(L, nArg))",
"\t{",
"\t\tconst char* pDispName = Lua_ValueToString(L, nArg);",
"\t\tif (pDispName && pDispName[0])",
"\t\t\tg_StrCpy(Npc[nNpcIdx].Name, (char*)pDispName);",
"\t}",
"\t// tham so cuoi (flag/isboss cua ban Linux): AddNpcSet2 da tu nap thuoc tinh, bo qua",
"\tLua_PushNumber(L, nNpcIdx);",
"\treturn 1;",
"}",
"int LuaHD3_AddNpc(Lua_State* L)",
"{",
"\treturn sHD3_AddNpcCommon(L, 0);",
"}",
"int LuaHD3_AddNpcEx(Lua_State* L)",
"{",
"\treturn sHD3_AddNpcCommon(L, 1);",
"}",
"",
]
block = "\r\n".join(lines) + "\r\n"
d = d.replace(anchor, block + anchor)
io.open(P, "w", encoding="latin-1", newline="").write(d)
print("da them LuaHD3_AddNpc + LuaHD3_AddNpcEx")

# dang ky + extern trong ScriptFuns.cpp
P2 = r"D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp"
d2 = io.open(P2, encoding="latin-1", newline="").read()
ex_anchor = "extern int LuaHD3_TrimString(Lua_State* L);\n"
assert d2.count(ex_anchor) == 1
d2 = d2.replace(ex_anchor, ex_anchor +
    "extern int LuaHD3_AddNpc(Lua_State* L);\n"
    "extern int LuaHD3_AddNpcEx(Lua_State* L);\n")
reg_anchor = '\t\t{"TrimString",\tLuaHD3_TrimString},\n'
assert d2.count(reg_anchor) == 1
d2 = d2.replace(reg_anchor, reg_anchor +
    '\t\t// [VA A7-C1] AddNpc/AddNpcEx ngu nghia Linux: KHONG dung camp (giu npcs.txt),\n'
    '\t\t// HD3_AddNpc tu random ngu hanh, tham so 6/7 = bNoRevive.\n'
    '\t\t{"HD3_AddNpc",\tLuaHD3_AddNpc},\n'
    '\t\t{"HD3_AddNpcEx",\tLuaHD3_AddNpcEx},\n')
io.open(P2, "w", encoding="latin-1", newline="").write(d2)
print("da dang ky HD3_AddNpc + HD3_AddNpcEx")
