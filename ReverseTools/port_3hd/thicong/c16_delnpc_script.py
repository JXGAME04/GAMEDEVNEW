# -*- coding: ascii -*-
"""C16 - chu chup anh: VAN CON 2 NPC Nhiep Thi Tran (cu + moi, lech 1-3 o).
Xoa theo TEN luc boot khong du (NPC cu co the sinh SAU DriverInit / hoi sinh
theo region). Giai phap chac: xoa theo SCRIPT DOI THOAI - NPC cu bind
nhieptran.lua / thuyenphu(.bac).lua, NPC moi bind nieshichen/hd3_thuyenphu
=> khong bao gio dung nham. Them:
  1) C++ HD3_DelNpcByScript(szSub): match strstr tren Npc[].ActionScript
     (chuoi thuong hoa engine luu san, KNpcSet.cpp:429) - khong can tinh id.
  2) hd3_driver: goi o BOOT va MOI PHUT trong HD3_Tick (tu lanh; chi print
     khi thuc su xoa duoc de khong xa rac console).
"""
import io, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

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
    if "HD3_DelNpcByScript" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    a = "int LuaHD3_AddNpc(Lua_State* L)"
    assert d.count(a) == 1
    body = NL.join([
        "// [3HD 25/08 C16] HD3_DelNpcByScript(szSub) - xoa NPC co ActionScript CHUA",
        "// chuoi szSub (khong phan biet hoa thuong - engine luu ActionScript da",
        "// g_StrLower, KNpcSet.cpp:429). Bam theo SCRIPT nen phan biet duoc NPC cu",
        "// (nhieptran.lua / thuyenphu.lua) voi NPC moi (nieshichen/hd3_thuyenphu)",
        "// du TRUNG TEN. Goi duoc moi phut (tu lanh khi NPC cu sinh muon/hoi sinh).",
        "int LuaHD3_DelNpcByScript(Lua_State* L)",
        "{",
        T + "if (Lua_GetTopIndex(L) < 1 || !Lua_IsString(L, 1))",
        T + T + "return 0;",
        T + "const char* pSub = Lua_ValueToString(L, 1);",
        T + "if (!pSub || !pSub[0])",
        T + T + "return 0;",
        T + "char szSub[80];",
        T + "g_StrCpyLen(szSub, (char*)pSub, sizeof(szSub));",
        T + "g_StrLower(szSub);",
        T + "int nXoa = 0;",
        T + "int nGom = 0;",
        T + "static int s_anGomS[512];",
        T + "for (int nIdx = 1; nIdx < MAX_NPC; nIdx++)",
        T + "{",
        T + T + "if (Npc[nIdx].m_dwID == 0)",
        T + T + T + "continue;",
        T + T + "if (Npc[nIdx].IsPlayer())",
        T + T + T + "continue;",
        T + T + "if (Npc[nIdx].m_SubWorldIndex < 0 || Npc[nIdx].m_RegionIndex < 0)",
        T + T + T + "continue;",
        T + T + "if (Npc[nIdx].ActionScript[0] == 0 || strstr(Npc[nIdx].ActionScript, szSub) == NULL)",
        T + T + T + "continue;",
        T + T + "if (nGom < 512)",
        T + T + T + "s_anGomS[nGom++] = nIdx;",
        T + "}",
        T + "for (int i = 0; i < nGom; i++)",
        T + "{",
        T + T + "int n = s_anGomS[i];",
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
    if "HD3_DelNpcByScript" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    a1 = "extern int LuaHD3_DelNpcByName(Lua_State* L);"
    assert d.count(a1) == 1
    d = d.replace(a1, a1 + NL + "extern int LuaHD3_DelNpcByScript(Lua_State* L);")
    a2 = '{"HD3_DelNpcByNameEx",\tLuaHD3_DelNpcByName},'
    i = d.index(a2)
    eol = d.index("\n", i) + 1
    d = d[:eol] + T + T + '{"HD3_DelNpcByScript",\tLuaHD3_DelNpcByScript},\t// [C16] xoa theo ActionScript (nhieptran/thuyenphu cu)' + NL + d[eol:]
    return d
rw(SRC + r"\ScriptFuns.cpp", f_sf)


# ---------- 3) hd3_driver.lua ----------
def f_drv(d):
    if "HD3_DonNpcCu" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    # ham chung
    helper = NL.join([
        "-- [3HD 25/08 C16] don NPC CU cua ban Viet theo SCRIPT DOI THOAI (chac chan",
        "-- khong dung NPC moi du trung ten). Goi luc boot VA moi phut (tu lanh: NPC",
        "-- cu co the sinh SAU DriverInit boi du lieu region / hoi sinh).",
        "HD3_SCRIPT_CU = {",
        '\t"nhieptran.lua",\t\t-- Nhiep Thi Tran ban Viet (boss satthu + vuot ai cu)',
        '\t"tinhnang\\\\phonglangdo\\\\thuyenphu.lua",\t-- thuyen phu Nam cu',
        '\t"tinhnang\\\\phonglangdo\\\\thuyenphubac.lua",\t-- thuyen phu Bac cu',
        "}",
        "function HD3_DonNpcCu(bNoiLuc)",
        "\tif (HD3_DelNpcByScript == nil) then return end",
        "\tlocal nTong = 0",
        "\tfor i = 1, getn(HD3_SCRIPT_CU) do",
        "\t\tnTong = nTong + HD3_DelNpcByScript(HD3_SCRIPT_CU[i])",
        "\tend",
        "\tif (nTong > 0 or bNoiLuc == 1) then",
        '\t\tprint("[3HD] Don NPC cu (theo script): xoa "..nTong..".")',
        "\tend",
        "end",
        "",
    ])
    a = "function HD3_DriverInit()"
    assert d.count(a) == 1
    d = d.replace(a, helper + a)
    # goi trong boot: sau 2 lenh xoa theo ten (truoc khi sinh NPC moi)
    a2 = "\t-- nap lazy (chi luc boot): 384 KB killbosshead + bang NPC 769"
    assert d.count(a2) == 1
    d = d.replace(a2, "\tHD3_DonNpcCu(1)\t-- [C16] xoa theo script - lop chac chan\n" + a2)
    # goi trong tick (moi phut)
    a3 = "function HD3_Tick(nHr, nMi)"
    assert d.count(a3) == 1
    d = d.replace(a3, a3 + NL + "\tHD3_DonNpcCu(0)\t-- [C16] tu lanh: NPC cu sinh muon/hoi sinh la bi don ngay trong 1 phut")
    return d
rw(SRV + r"\script\tinhnang\3hoatdong\hd3_driver.lua", f_drv)
import shutil
shutil.copyfile(SRV + r"\script\tinhnang\3hoatdong\hd3_driver.lua", MIRROR + r"\script\tinhnang\3hoatdong\hd3_driver.lua")
print("da sync guong")
