# -*- coding: utf-8 -*-
"""C19 - phan SERVER cua dot "auto lien map toi boss" + don NPC cu triet de:
  1) KProtocolProcess case 6: nhanh "st3_goboss" -> hd3_st_goboss.lua (da sinh).
  2) KJx2WarInfra: HD3_DelNpcByNameEx thanh ham RIENG 3 tham so
     (ten, mapID|0, excludeScriptSub) - xoa NPC trung TEN nhung KHONG thuoc
     script cua minh => bat duoc NPC cu bat ke no bind script gi/khong script.
     Kem g_DebugLog tung nan nhan (idx, setting, script) de con soi nguon goc.
  3) hd3_driver HD3_DonNpcCu: them 2 lenh exclude-name moi phut.
  4) nieshichen [1]: phuc hoi cau 'thach thuc thoi gian' (ban 179B, van < 511B);
     main(): khi event birthday chen [29] thi THAY [11] (flavor trung voi Dong)
     de khong vuot tran goi.
  5) cauhinh: khoa HD3_ST_TIEN_XE (gia xe toi boss, khuon cu MONEY_GO_BOSS=1000).
"""
import io, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")

CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"
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


def sync(rel):
    import shutil, os
    os.makedirs(os.path.dirname(MIRROR + "\\" + rel), exist_ok=True)
    shutil.copyfile(SRV + "\\" + rel, MIRROR + "\\" + rel)


# ---------- 1) KProtocolProcess: st3_goboss ----------
def f_proto(d):
    if "st3_goboss" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    a = T*3 + 'else if (!strcmp(pUiCmd->szFunc, "st3_quit"))'
    assert d.count(a) == 1
    ins = NL.join([
        T*3 + 'else if (!strcmp(pUiCmd->szFunc, "st3_goboss"))',
        T*3 + "{",
        T*4 + "// [3HD C19] F11 dan duong: thue xe toi cho boss (khuon cu denchobossST",
        T*4 + "// cua ban Viet: kiem tien + Pay + NewWorld + SetFightState(1))",
        T*4 + 'Player[nIndex].ExecuteScript("\\\\script\\\\task\\\\tollgate\\\\killer\\\\hd3_st_goboss.lua", "st3_goboss", 0);',
        T*3 + "}",
    ])
    return d.replace(a, ins + NL + a)
rw(CORE + r"\KProtocolProcess.cpp", f_proto)


# ---------- 2) KJx2WarInfra: Ex rieng 3 tham so ----------
def f_infra(d):
    if "LuaHD3_DelNpcByNameEx" in d and "pExcl" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    a = "int LuaHD3_DelNpcByScript(Lua_State* L)"
    assert d.count(a) == 1
    body = NL.join([
        "// [3HD 25/08 C19] HD3_DelNpcByNameEx(szTen, nMapID|0, szExcludeScript) - xoa NPC",
        "// trung TEN nhung ActionScript KHONG chua szExcludeScript (phan biet NPC cu voi",
        "// NPC moi cung ten bat ke NPC cu bind script gi hay khong co script). Log tung",
        "// nan nhan vao DebugLog de truy nguon (template + script).",
        "int LuaHD3_DelNpcByNameEx(Lua_State* L)",
        "{",
        T + "if (Lua_GetTopIndex(L) < 1 || !Lua_IsString(L, 1))",
        T + T + "return 0;",
        T + "const char* pTen = Lua_ValueToString(L, 1);",
        T + "if (!pTen || !pTen[0])",
        T + T + "return 0;",
        T + "int nLocMap = 0;",
        T + "if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2))",
        T + T + "nLocMap = (int)Lua_ValueToNumber(L, 2);",
        T + "char szExcl[80];",
        T + "szExcl[0] = 0;",
        T + "if (Lua_GetTopIndex(L) >= 3 && Lua_IsString(L, 3))",
        T + "{",
        T + T + "g_StrCpyLen(szExcl, (char*)Lua_ValueToString(L, 3), sizeof(szExcl));",
        T + T + "g_StrLower(szExcl);",
        T + "}",
        T + "int nXoa = 0;",
        T + "int nGom = 0;",
        T + "static int s_anGomN[512];",
        T + "for (int nIdx = 1; nIdx < MAX_NPC; nIdx++)",
        T + "{",
        T + T + "if (Npc[nIdx].m_dwID == 0)",
        T + T + T + "continue;",
        T + T + "if (Npc[nIdx].IsPlayer())",
        T + T + T + "continue;",
        T + T + "if (Npc[nIdx].m_SubWorldIndex < 0 || Npc[nIdx].m_RegionIndex < 0)",
        T + T + T + "continue;",
        T + T + "if (nLocMap != 0 && SubWorld[Npc[nIdx].m_SubWorldIndex].m_SubWorldID != nLocMap)",
        T + T + T + "continue;",
        T + T + "if (strstr(Npc[nIdx].Name, pTen) == NULL)",
        T + T + T + "continue;",
        T + T + "if (szExcl[0] && Npc[nIdx].ActionScript[0] && strstr(Npc[nIdx].ActionScript, szExcl) != NULL)",
        T + T + T + "continue;\t// NPC cua minh - giu",
        T + T + "if (nGom < 512)",
        T + T + T + "s_anGomN[nGom++] = nIdx;",
        T + "}",
        T + "for (int i = 0; i < nGom; i++)",
        T + "{",
        T + T + "int n = s_anGomN[i];",
        T + T + "g_DebugLog(\"[3HD C19] xoa NPC cu idx=%d setting=%d map=%d script=%s\",",
        T + T + T + "n, Npc[n].m_NpcSettingIdx, SubWorld[Npc[n].m_SubWorldIndex].m_SubWorldID, Npc[n].ActionScript);",
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
rw(CORE + r"\KJx2WarInfra.cpp", f_infra)


def f_sf(d):
    NL = "\r\n" if "\r\n" in d else "\n"
    # alias cu tro LuaHD3_DelNpcByName -> tro ham moi + extern
    a = "extern int LuaHD3_DelNpcByScript(Lua_State* L);"
    if "extern int LuaHD3_DelNpcByNameEx(Lua_State* L);" not in d:
        assert d.count(a) == 1
        d = d.replace(a, a + NL + "extern int LuaHD3_DelNpcByNameEx(Lua_State* L);")
    old = '{"HD3_DelNpcByNameEx",\tLuaHD3_DelNpcByName},'
    if old in d:
        d = d.replace(old, '{"HD3_DelNpcByNameEx",\tLuaHD3_DelNpcByNameEx},\t// [C19] ten + map + exclude-script')
    return d
rw(CORE + r"\ScriptFuns.cpp", f_sf)


# ---------- 3) hd3_driver: exclude-name moi phut ----------
def f_drv(d):
    if "nieshichen" in d and "HD3_DelNpcByNameEx" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    a = T + "if (nTong > 0 or bNoiLuc == 1) then"
    assert d.count(a) == 1
    ins = NL.join([
        T + "-- [C19] lop CHAC CHAN: xoa NPC trung ten KHONG mang script cua minh",
        T + "-- (bat duoc NPC cu bat ke no bind script gi / khong script)",
        T + "if (HD3_DelNpcByNameEx ~= nil) then",
        T + T + 'nTong = nTong + HD3_DelNpcByNameEx("' + V("Nhiếp Thí Trần") + '", 0, "nieshichen")',
        T + T + 'nTong = nTong + HD3_DelNpcByNameEx("' + V("Thuyền phu") + '", 336, "hd3_thuyenphu")',
        T + "end",
    ])
    return d.replace(a, ins + NL + a)
rw(SRV + r"\script\tinhnang\3hoatdong\hd3_driver.lua", f_drv)
sync(r"script\tinhnang\3hoatdong\hd3_driver.lua")


# ---------- 4) nieshichen: [1] du cau + main() thay [11] bang [29] ----------
def f_nie2(d):
    NL = "\r\n" if "\r\n" in d else "\n"
    # 4a. [1]: noi them cau thach thuc (neu chua co)
    if "enter>" not in d.split("\n")[22]:
        old = V("là được.") + '",'
        assert d.count(old) == 1, ("[1]", d.count(old))
        new = V("là được.") + ' <enter>' + V("Hãy nhận 'thách thức thời gian' để thử sức đồng đội.") + '",'
        d = d.replace(old, new, 1)
    # 4b. main(): tinsert [29] -> thay [11] (vi tri 5)
    old2 = T + T + "tinsert(tbDialog, 12, ContentList[29]);"
    if old2 in d:
        d = d.replace(old2, T + T + "tbDialog[5] = ContentList[29];\t-- [C19] thay muc flavor [11] de khong vuot tran goi 511B")
    return d
rw(SRV + r"\script\task\tollgate\killer\nieshichen.lua", f_nie2)
sync(r"script\task\tollgate\killer\nieshichen.lua")


# ---------- 5) cauhinh: HD3_ST_TIEN_XE ----------
def f_cfg(d):
    if "HD3_ST_TIEN_XE" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    a = "HD3_ST_THUONG = nil,"
    assert d.count(a) == 1
    ins = NL.join([
        "",
        "-- (A) SAT THU - gia thue xe toi cho boss khi bam dan duong tren F11 (khuon",
        "--     cu cua ban Viet MONEY_GO_BOSS = 1000 luong). [LIVE]",
        "HD3_ST_TIEN_XE = 1000,",
    ])
    return d.replace(a, a + ins)
rw(SRV + r"\script\header\cauhinh_hoatdong.lua", f_cfg)
sync(r"script\header\cauhinh_hoatdong.lua")
print("xong C19 (server)")
