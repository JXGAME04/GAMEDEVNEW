# -*- coding: utf-8 -*-
r"""[PETSYS 28/08 chieu] Va cac ham Linux thieu tren JX1:

A. script\petsys\jx1_compat.lua (MOI, an ngay qua "Nap lai script"):
   GetCashCoin=GetExtPoint (xu JX1 = ExtPoint - admin point.lua:59 tienxu),
   PayCoin qua SetExtPoint, GetServerDate=GetLocalDate, TabFile_GetColCount=0,
   ReduceOwnExp compat-bao-cho (tu nhuong khi ban C co).
   Include tu head.lua DAU TIEN + ReLoadScript trong lenh bai.
B. C: LuaReduceOwnExp = DirectAddExp(-n) (DirectAddExp xu am + sync san,
   KPlayer.cpp) -> KPlayerPet.cpp + dang ky; build server DAT CANH cho chu swap.
"""
import io
import os

BS = chr(92)
CR = chr(13)

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"

# ---------- A1. jx1_compat.lua ----------
p = os.path.join(SV, "script", "petsys", "jx1_compat.lua")
noidung = "\r\n".join([
    "-- [PETSYS 28/08] cac ham Linux chua co tren JX1 - compat TU NHUONG:",
    "-- moi khoi chi dinh nghia khi ham C chua ton tai (ban C moi se de len).",
    "",
    "if (GetCashCoin == nil) then",
    "\t-- xu cua JX1 = ExtPoint (admin point.lua: tienxu -> EarnExtPoint)",
    "\tfunction GetCashCoin()",
    "\t\treturn GetExtPoint()",
    "\tend",
    "end",
    "",
    "if (PayCoin == nil) then",
    "\tfunction PayCoin(nCoin)",
    "\t\tif (GetExtPoint() < nCoin) then",
    "\t\t\treturn 0",
    "\t\tend",
    "\t\tSetExtPoint(GetExtPoint() - nCoin)",
    "\t\treturn 1",
    "\tend",
    "end",
    "",
    "if (GetServerDate == nil) then",
    "\tfunction GetServerDate(szFmt)",
    "\t\treturn GetLocalDate(szFmt)",
    "\tend",
    "end",
    "",
    "if (TabFile_GetColCount == nil) then",
    "\tfunction TabFile_GetColCount(szAlias)",
    "\t\treturn 0",
    "\tend",
    "end",
    "",
    "if (ReduceOwnExp == nil) then",
    "\t-- ban C that (DirectAddExp am) nam trong CoreServer.dll.moi_petsys_rb2;",
    "\t-- khi chua swap thi bao ro thay vi chet script.",
    "\tfunction ReduceOwnExp(nExp)",
    '\t\tTalk(1, "", "Chuc nang nay can ban CoreServer moi - admin swap CoreServer.dll.moi_petsys_rb2 roi thu lai")',
    "\t\treturn 0",
    "\tend",
    "end",
    "",
])
io.open(p, "w", encoding="latin-1", newline="").write(noidung)
print("A1. ghi jx1_compat.lua")

# ---------- A2. head.lua Include compat DAU TIEN ----------
p = os.path.join(SV, "script", "petsys", "head.lua")
s = io.open(p, "r", encoding="latin-1", newline="").read()
if "jx1_compat" not in s:
    dong = ('Include("' + BS*2 + 'script' + BS*2 + 'petsys' + BS*2 +
            'jx1_compat.lua")\r\n')
    s = dong + s
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("A2. head.lua Include compat dau tien")
else:
    print("A2. da co")

# ---------- A3. ReLoadScript compat trong lenh bai ----------
p = os.path.join(SV, "script", "item", "lenhbaiadmin.lua")
s = io.open(p, "r", encoding="latin-1", newline="").read()
if "jx1_compat" not in s:
    neo = ('ReLoadScript("' + BS*2 + 'script' + BS*2 + 'petsys' + BS*2 +
           'protocol_process_gs.lua")')
    assert s.count(neo) == 1
    s = s.replace(neo, ('ReLoadScript("' + BS*2 + 'script' + BS*2 + 'petsys' +
                        BS*2 + 'jx1_compat.lua")\r\n' + neo), 1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("A3. reload + compat")
else:
    print("A3. da co")

# ---------- B. C: LuaReduceOwnExp ----------
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPet.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read()
if "LuaReduceOwnExp" not in s:
    lf = s.replace(CR, "")
    neo = "int LuaPET_GetName(Lua_State* L)"
    them = '''// [PETSYS] tru exp truc tiep (Linux ReduceOwnExp) - DirectAddExp xu am + sync
int LuaReduceOwnExp(Lua_State* L)
{
	int nIdx = sPetCtx(L);
	if (nIdx <= 0) return 0;
	double nExp = (double)Lua_ValueToNumber(L, 1);
	if (nExp > 0)
		Player[nIdx].DirectAddExp(-nExp);
	return 0;
}

int LuaPET_GetName(Lua_State* L)'''
    assert lf.count(neo) == 1
    lf = lf.replace(neo, them, 1)
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
    print("B1. + LuaReduceOwnExp")

p = r"D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read()
if '"ReduceOwnExp"' not in s:
    neo = 'extern int LuaPET_GetName(Lua_State* L);'
    assert s.count(neo) == 1
    s = s.replace(neo, neo + "\r\nextern int LuaReduceOwnExp(Lua_State* L);", 1)
    neo2 = '\t\t{ "PET_GetName",\tLuaPET_GetName },'
    assert s.count(neo2) == 1
    s = s.replace(neo2, neo2 + '\r\n\t\t{ "ReduceOwnExp",\tLuaReduceOwnExp },\t// [PETSYS] Linux API', 1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("B2. dang ky ReduceOwnExp")
else:
    print("B2. da co")
print("XONG p24")
