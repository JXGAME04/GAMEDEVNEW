# -*- coding: utf-8 -*-
"""C14 - chu bao: "con cac NPC Nhiep Thi Tran CU cua ban du an" => chua 100% Linux.
NPC cu (script global\\npcchucnang\\nhieptran.lua, Fong Kieu 2021) KHONG do script
nao spawn (grep het cay: khong ai goi nhieptran/addnpcbosssatthu da comment) =>
nam trong DU LIEU MAP TINH (Npc_S.dat trong .fp - nen). Cach tat sach khong dung
map: xoa theo TEN luc boot TRUOC khi sinh NPC 769 ban Linux.
  1) KJx2WarInfra.cpp: LuaHD3_DelNpcByName(szName) - duyet m_UseIdx (chi NPC song,
     khong dinh free-slot), bo qua player, match strstr, go dung khuon LuaDelNpc
     (Region RemoveNpc + DecRef + NpcSet.Remove), tra so luong xoa.
  2) ScriptFuns.cpp: extern + dang ky.
  3) hd3_driver.lua: goi dau HD3_DriverInit (co guard nil cho DLL cu).
"""
import io, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
T = "\t"


def rw(p, fn):
    d = io.open(p, encoding="latin-1", newline="").read()
    d2 = fn(d)
    if d2 != d:
        io.open(p, "w", encoding="latin-1", newline="").write(d2)
        print("  OK", p.rsplit("\\", 1)[-1])
    else:
        print("  (khong doi)", p.rsplit("\\", 1)[-1])


# ---------- 1) KJx2WarInfra.cpp ----------
def f_infra(d):
    if "HD3_DelNpcByName" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    a = "int LuaHD3_AddNpc(Lua_State* L)"
    assert d.count(a) == 1
    body = NL.join([
        "// [3HD 25/08 C14] HD3_DelNpcByName(szTen) - xoa NPC (khong phai player) co ten",
        "// CHUA chuoi szTen. Dung don NPC \"Nhiep Thi Tran\" CU cua ban Viet (nam trong",
        "// du lieu map tinh, khong script nao tat duoc) TRUOC khi sinh NPC 769 Linux.",
        "// Duyet m_UseIdx nhu KNpcSet::GetAroundGoldMonster => chi cham NPC dang song,",
        "// khong dinh free-slot; go theo dung khuon LuaDelNpc (ScriptFuns.cpp:7078).",
        "int LuaHD3_DelNpcByName(Lua_State* L)",
        "{",
        T + "if (Lua_GetTopIndex(L) < 1 || !Lua_IsString(L, 1))",
        T + T + "return 0;",
        T + "const char* pTen = Lua_ValueToString(L, 1);",
        T + "if (!pTen || !pTen[0])",
        T + T + "return 0;",
        T + "int nXoa = 0;",
        T + "int nIdx = 0;",
        T + "int nGom = 0;",
        T + "static int s_anGom[512];",
        T + "for (;;)",
        T + "{",
        T + T + "nIdx = NpcSet.m_UseIdx.GetNext(nIdx);",
        T + T + "if (nIdx == 0)",
        T + T + T + "break;",
        T + T + "if (Npc[nIdx].IsPlayer())",
        T + T + T + "continue;",
        T + T + "if (Npc[nIdx].m_SubWorldIndex < 0 || Npc[nIdx].m_RegionIndex < 0)",
        T + T + T + "continue;",
        T + T + "if (strstr(Npc[nIdx].Name, pTen) == NULL)",
        T + T + T + "continue;",
        T + T + "if (nGom < 512)",
        T + T + T + "s_anGom[nGom++] = nIdx;",
        T + "}",
        T + "for (int i = 0; i < nGom; i++)",
        T + "{",
        T + T + "int n = s_anGom[i];",
        T + T + "SubWorld[Npc[n].m_SubWorldIndex].m_Region[Npc[n].m_RegionIndex].RemoveNpc(n);",
        T + T + "SubWorld[Npc[n].m_SubWorldIndex].m_Region[Npc[n].m_RegionIndex].DecRef(Npc[n].m_MapX, Npc[n].m_MapY, obj_npc);",
        T + T + "NpcSet.Remove(n);",
        T + T + "nXoa++;",
        T + "}",
        T + "Lua_PushNumber(L, nXoa);",
        T + "return 1;",
        "}",
        "",
    ])
    return d.replace(a, body + a)
rw(SRC + r"\KJx2WarInfra.cpp", f_infra)


# ---------- 2) ScriptFuns.cpp ----------
def f_sf(d):
    if "HD3_DelNpcByName" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    a1 = "extern int LuaHD3_AddNpc(Lua_State* L);"
    assert d.count(a1) == 1
    d = d.replace(a1, a1 + NL + "extern int LuaHD3_DelNpcByName(Lua_State* L);")
    a2 = T + T + '{"HD3_AddNpc",\tLuaHD3_AddNpc},'
    assert d.count(a2) == 1, "anchor dang ky"
    d = d.replace(a2, a2 + NL + T + T + '{"HD3_DelNpcByName",\tLuaHD3_DelNpcByName},\t// [3HD C14] xoa NPC cu theo ten (don Nhiep Thi Tran ban Viet)')
    return d
rw(SRC + r"\ScriptFuns.cpp", f_sf)


# ---------- 3) hd3_driver.lua (song + guong) ----------
TEN = unicode_to_tcvn3_bytes("Nhiếp Thí Trần").decode("latin-1")

def f_drv(d):
    if "HD3_DelNpcByName" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    a = T + '-- nap lazy (chi luc boot): 384 KB killbosshead + bang NPC 769'
    assert d.count(a) == 1
    ins = NL.join([
        T + "-- [3HD 25/08 C14] don NPC \"Nhiep Thi Tran\" CU cua ban Viet (nam trong du",
        T + "-- lieu map tinh) TRUOC khi sinh NPC 769 Linux - de dung 100% ban Linux.",
        T + "-- Guard nil: DLL cu chua co ham thi bo qua (khong lam chet boot).",
        T + "if (HD3_DelNpcByName ~= nil) then",
        T + T + 'local nXoaCu = HD3_DelNpcByName("' + TEN + '")',
        T + T + 'print("[3HD] Da xoa "..nXoaCu.." NPC Nhiep Thi Tran cu (ban Viet).")',
        T + "end",
    ])
    return d.replace(a, ins + NL + a)

p_live = SRV + r"\script\tinhnang\3hoatdong\hd3_driver.lua"
rw(p_live, f_drv)
import shutil
shutil.copyfile(p_live, MIRROR + r"\script\tinhnang\3hoatdong\hd3_driver.lua")
print("da sync guong hd3_driver.lua")
