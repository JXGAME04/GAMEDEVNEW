# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] Bo sung 15 ham PET_* ban Linux co ma JX1 thieu (audit p54):
PET_SetStr/Dex/Vit/Eng/Life/Mana, PET_GetStr/Dex/Vit/Eng/Life/Mana,
PET_SetAttrib, PET_ClearSkill, PET_AddUpgradePoint.
Trong do PET_AddUpgradePoint la co che CONG DIEM THANG CAP chuan cua Linux
(activitysys\config\44\extend.lua: moi hoat dong xong trong ngay -> +1 diem).
Chi so attrib: 0=Str 1=Dex 2=Vit 3=Eng 4=Life 5=Mana -> o 5118..5123.
"""
import io

CR = chr(13)
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPet.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read()
lf = s.replace(CR + "\n", "\n")

if "LuaPET_AddUpgradePoint" in lf:
    print("1. da co")
else:
    neo = "int LuaPET_IsCreate(Lua_State* L)"
    assert lf.count(neo) == 1
    them = """// [29/08 - bu theo audit] 15 ham ban Linux co ma JX1 thieu.
// attrib: 0=Str 1=Dex 2=Vit 3=Eng 4=Life 5=Mana (o PET_TV_ATTRIB0 + i)
static int sPetGetAttrAt(Lua_State* L, int nIdx)
{ Lua_PushNumber(L, sPetG(sPetCtx(L), PET_TV_ATTRIB0 + nIdx)); return 1; }
static int sPetSetAttrAt(Lua_State* L, int nIdx)
{ sPetS(sPetCtx(L), PET_TV_ATTRIB0 + nIdx, (int)Lua_ValueToNumber(L, 1)); return 0; }

int LuaPET_GetStr(Lua_State* L)  { return sPetGetAttrAt(L, 0); }
int LuaPET_GetDex(Lua_State* L)  { return sPetGetAttrAt(L, 1); }
int LuaPET_GetVit(Lua_State* L)  { return sPetGetAttrAt(L, 2); }
int LuaPET_GetEng(Lua_State* L)  { return sPetGetAttrAt(L, 3); }
int LuaPET_GetLife(Lua_State* L) { return sPetGetAttrAt(L, 4); }
int LuaPET_GetMana(Lua_State* L) { return sPetGetAttrAt(L, 5); }
int LuaPET_SetStr(Lua_State* L)  { return sPetSetAttrAt(L, 0); }
int LuaPET_SetDex(Lua_State* L)  { return sPetSetAttrAt(L, 1); }
int LuaPET_SetVit(Lua_State* L)  { return sPetSetAttrAt(L, 2); }
int LuaPET_SetEng(Lua_State* L)  { return sPetSetAttrAt(L, 3); }
int LuaPET_SetLife(Lua_State* L) { return sPetSetAttrAt(L, 4); }
int LuaPET_SetMana(Lua_State* L) { return sPetSetAttrAt(L, 5); }

// PET_SetAttrib(nIndex, nValue) - nIndex 0..5
int LuaPET_SetAttrib(Lua_State* L)
{
	int nIdx = (int)Lua_ValueToNumber(L, 1);
	if (nIdx < 0 || nIdx >= PET_ATTRIB_COUNT) return 0;
	sPetS(sPetCtx(L), PET_TV_ATTRIB0 + nIdx, (int)Lua_ValueToNumber(L, 2));
	return 0;
}

// PET_ClearSkill() - xoa 4 o ky nang
int LuaPET_ClearSkill(Lua_State* L)
{
	int nIdx = sPetCtx(L);
	for (int i = 0; i < PET_SKILL_COUNT; i++)
		sPetS(nIdx, PET_TV_SKILL0 + i, 0);
	return 0;
}

// PET_AddUpgradePoint(n) - CONG diem thang cap (Linux: moi hoat dong/ngay +1)
int LuaPET_AddUpgradePoint(Lua_State* L)
{
	int nIdx = sPetCtx(L);
	int nAdd = (int)Lua_ValueToNumber(L, 1);
	if (nIdx <= 0 || nAdd == 0) return 0;
	int nMoi = sPetG(nIdx, PET_TV_UPGRADE) + nAdd;
	if (nMoi < 0) nMoi = 0;
	sPetS(nIdx, PET_TV_UPGRADE, nMoi);
	return 0;
}

int LuaPET_IsCreate(Lua_State* L)"""
    lf = lf.replace(neo, them, 1)
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
    print("1. them 15 ham vao KPlayerPet.cpp")

# dang ky
p = r"D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read()
lf = s.replace(CR + "\n", "\n")
TEN = ["PET_GetStr", "PET_GetDex", "PET_GetVit", "PET_GetEng", "PET_GetLife", "PET_GetMana",
       "PET_SetStr", "PET_SetDex", "PET_SetVit", "PET_SetEng", "PET_SetLife", "PET_SetMana",
       "PET_SetAttrib", "PET_ClearSkill", "PET_AddUpgradePoint"]
if '"PET_AddUpgradePoint"' in lf:
    print("2. da dang ky")
else:
    neo = "extern int LuaPET_GetName(Lua_State* L);"
    assert lf.count(neo) == 1
    lf = lf.replace(neo, neo + "\n" + "\n".join("extern int Lua%s(Lua_State* L);" % t for t in TEN), 1)
    neo2 = '\t\t{ "PET_GetName",\tLuaPET_GetName },'
    assert lf.count(neo2) == 1
    lf = lf.replace(neo2, neo2 + "\n\t\t// [29/08] 15 ham ban Linux con thieu (audit p54)\n" +
                    "\n".join('\t\t{ "%s",\tLua%s },' % (t, t) for t in TEN), 1)
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
    print("2. dang ky 15 ham")
print("XONG p55")
