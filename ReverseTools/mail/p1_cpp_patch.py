# -*- coding: utf-8 -*-
"""[MAIL 03/09] Dot 1 - va C++ trong worktree D:\\GAMEDEVNEW_wt_mail cho kenh ScriptProtocol.
Doc/ghi latin-1 (giu nguyen byte TCVN3/GBK), moi neo phai KHOP DUY NHAT, idempotent theo dau [MAIL 03/09].
Chay: python p1_cpp_patch.py [--check]
"""
import io, os, re, sys

ROOT = r"D:\GAMEDEVNEW_wt_mail"
MARK = "[MAIL 03/09]"
CHECK = "--check" in sys.argv


def rd(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def wr(p, s):
    if CHECK:
        return
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def eol_of(s):
    return "\r\n" if "\r\n" in s else "\n"


def insert_after(s, anchor, block, label):
    """Chen block (cac dong, khong co EOL cuoi) ngay SAU dong chua anchor (neo phai duy nhat)."""
    n = s.count(anchor)
    assert n == 1, "%s: neo %r khop %d lan" % (label, anchor, n)
    i = s.index(anchor)
    j = s.index(eol_of(s), i) + len(eol_of(s))
    return s[:j] + block.replace("\n", eol_of(s)) + eol_of(s) + s[j:]


def replace_once(s, old, new, label):
    n = s.count(old)
    assert n == 1, "%s: neo %r khop %d lan" % (label, old, n)
    return s.replace(old, new)


def patch(rel, fn):
    p = os.path.join(ROOT, rel)
    s = rd(p)
    if MARK in s:
        print("  da va, bo qua:", rel)
        return
    hb = sum(1 for c in s if ord(c) >= 0x80)
    s2 = fn(s)
    hb2 = sum(1 for c in s2 if ord(c) >= 0x80)
    assert hb == hb2, "%s: so byte cao doi %d -> %d" % (rel, hb, hb2)
    wr(p, s2)
    print("  OK:", rel)


# 1) Headers\KProtocolDef.h - enum (GIA TRI = hop dong client/server, noi CUOI moi day)
def f_protocoldef(s):
    s = insert_after(s, "\ts2c_detonate,",
        "\ts2c_scriptdata,\t\t\t// [MAIL 03/09] goi ScriptProtocol (ObjBuffer) may chu -> client, do dai dong (-1)",
        "s2c enum")
    s = insert_after(s, "\tc2s_partnerop,",
        "\tc2s_scriptdata,\t\t// [MAIL 03/09] goi ScriptProtocol (ObjBuffer) client -> may chu, do dai dong (-1)",
        "c2s enum")
    return s


# 2) KProtocol.cpp - bang kich thuoc (cung vi tri voi enum)
def f_protocolcpp(s):
    s = insert_after(s, "sizeof(S2C_DETONATE),",
        "\t-1,\t\t\t\t\t\t\t// s2c_scriptdata [MAIL 03/09] do dai dong: WORD sau ProtocolType",
        "s2c size")
    s = insert_after(s, "sizeof(PARTNER_OP_DATA),",
        "\t-1,\t\t\t\t\t\t\t// c2s_scriptdata [MAIL 03/09] do dai dong: WORD sau ProtocolType",
        "c2s size")
    return s


# 3) KProtocolProcess.h - khai bao handler
def f_proch(s):
    s = insert_after(s, "void\ts2cDetonate(BYTE* pMsg);",
        "\tvoid\ts2cScriptData(BYTE* pMsg);\t// [MAIL 03/09] kenh ScriptProtocol (KScriptProtocol.cpp)",
        "s2c decl")
    s = insert_after(s, "void \tc2sPartnerOp(int nIndex, BYTE* pProtocol);",
        "\tvoid \tc2sScriptData(int nIndex, BYTE* pProtocol);\t// [MAIL 03/09] kenh ScriptProtocol",
        "c2s decl")
    return s


# 4) KProtocolProcess.cpp - include, dang ky, than ham (cuoi tep, ngoai moi #ifdef)
def f_proccpp(s):
    s = insert_after(s, '#include "KSubWorldSet.h"',
        '#include "KScriptProtocol.h"\t// [MAIL 03/09] kenh ScriptProtocol (ObjBuffer)',
        "include")
    s = insert_after(s, "ProcessFunc[s2c_detonate] = &KProtocolProcess::s2cDetonate;",
        "\tProcessFunc[s2c_scriptdata] = &KProtocolProcess::s2cScriptData;\t// [MAIL 03/09]",
        "s2c reg")
    s = insert_after(s, "ProcessFunc[c2s_partnerop] = &KProtocolProcess::c2sPartnerOp;",
        "\tProcessFunc[c2s_scriptdata] = &KProtocolProcess::c2sScriptData;\t// [MAIL 03/09]",
        "c2s reg")
    e = eol_of(s)
    tail = e.join([
        "",
        "// [MAIL 03/09] kenh ScriptProtocol (ObjBuffer): chi chuyen tiep sang KScriptProtocol.cpp.",
        "// Goi do dai dong (-1): may chu da qua CheckProtocolSize, client tu doc wLength trong goi.",
        "#ifndef _SERVER",
        "void KProtocolProcess::s2cScriptData(BYTE* pMsg)",
        "{",
        "\tSP_OnClientRecv(pMsg);",
        "}",
        "#else",
        "void KProtocolProcess::c2sScriptData(int nIndex, BYTE* pProtocol)",
        "{",
        "\tSP_OnServerRecv(nIndex, pProtocol);",
        "}",
        "#endif",
        "",
    ])
    if not s.endswith(e):
        s += e
    return s + tail


# 5) KJx2SharedStore.h - ObjBuffer dung ca client; Ladder/GlbValue van _SERVER
def f_storeh(s):
    s = replace_once(s,
        "#ifdef _SERVER\r\n\r\ntypedef struct lua_State Lua_State;" if "\r\n" in s else "#ifdef _SERVER\n\ntypedef struct lua_State Lua_State;",
        ("// [MAIL 03/09] ObjBuffer dung cho CA client (kenh ScriptProtocol, KScriptProtocol.cpp);\n"
         "// Ladder / GlbValue van chi may chu (#ifdef _SERVER phia duoi).\n\ntypedef struct lua_State Lua_State;").replace("\n", eol_of(s)),
        "header guard dau")
    s = insert_after(s, "int LuaOB_PopString(Lua_State* L);",
        ("\n// [MAIL 03/09] truy cap byte tho cho kenh ScriptProtocol (KScriptProtocol.cpp)\n"
         "int  KJx2OB_CreateFromBytes(const void* pData, int nLen);     // -> handle (>0), 0 = fail\n"
         "int  KJx2OB_GetBytes(int h, const unsigned char** ppData);   // -> so byte da ghi, -1 neu handle xau\n"
         "int  KJx2OB_Release(int h);                                  // -> 1/0\n"
         "\n#ifdef _SERVER"),
        "header bytes api")
    return s


# 6) KJx2SharedStore.cpp - bo #ifdef bao toan tep, bao rieng cac khoi may chu
def f_storecpp(s):
    e = eol_of(s)
    s = replace_once(s, '#include "KWin32.h"' + e + e + "#ifdef _SERVER" + e,
        '#include "KWin32.h"' + e + e +
        "// [MAIL 03/09] ObjBuffer bien dich cho CA client (kenh ScriptProtocol); RemoteExecute, ShareData," + e +
        "// Ladder, GlbValue van nam trong #ifdef _SERVER phia duoi." + e,
        "cpp guard dau")
    s = replace_once(s, "// [PORT5 23/08] RemoteExecute (Linux GS 0x08100740",
        "#ifdef _SERVER\t// [MAIL 03/09] RemoteExecute + ShareData: chi may chu" + e +
        "// [PORT5 23/08] RemoteExecute (Linux GS 0x08100740",
        "cpp RemoteExecute")
    s = replace_once(s,
        "\tLua_PushNumber(L, 1);" + e + "\treturn 1;" + e + "}" + e + e + "int LuaOB_IsEmpty(Lua_State* L)",
        "\tLua_PushNumber(L, 1);" + e + "\treturn 1;" + e + "}" + e + "#endif // _SERVER (RemoteExecute + ShareData) [MAIL 03/09]" + e + e +
        "int LuaOB_IsEmpty(Lua_State* L)",
        "cpp end ShareData")
    s = replace_once(s,
        "//////////////////////////////////////////////////////////////////////" + e +
        "// 2) Ladder - top 10 moi id, id > 10000, persist \\settings\\jx2ladder.txt",
        "// [MAIL 03/09] truy cap byte tho cho kenh ScriptProtocol (KScriptProtocol.cpp)" + e +
        "int KJx2OB_CreateFromBytes(const void* pData, int nLen)" + e +
        "{" + e +
        "\tif (nLen < 0 || nLen > JX2OB_BUF_SIZE || (nLen > 0 && !pData))" + e +
        "\t\treturn 0;" + e +
        "\tKJx2ObjBuffer* p = new KJx2ObjBuffer;" + e +
        "\tif (!p)" + e +
        "\t\treturn 0;" + e +
        "\tif (nLen > 0)" + e +
        "\t\tmemcpy(p->Buf, pData, nLen);" + e +
        "\tp->nWrite = nLen;" + e +
        "\tp->nRead = 0;" + e +
        "\tint h = ++s_nOBNextHandle;" + e +
        "\ts_OBMap[h] = p;" + e +
        "\treturn h;" + e +
        "}" + e + e +
        "int KJx2OB_GetBytes(int h, const unsigned char** ppData)" + e +
        "{" + e +
        "\tif (h <= 0 || !ppData)" + e +
        "\t\treturn -1;" + e +
        "\tstd::map<int, KJx2ObjBuffer*>::iterator it = s_OBMap.find(h);" + e +
        "\tif (it == s_OBMap.end())" + e +
        "\t\treturn -1;" + e +
        "\t*ppData = it->second->Buf;" + e +
        "\treturn it->second->nWrite;" + e +
        "}" + e + e +
        "int KJx2OB_Release(int h)" + e +
        "{" + e +
        "\tstd::map<int, KJx2ObjBuffer*>::iterator it = s_OBMap.find(h);" + e +
        "\tif (it == s_OBMap.end())" + e +
        "\t\treturn 0;" + e +
        "\tdelete it->second;" + e +
        "\ts_OBMap.erase(it);" + e +
        "\treturn 1;" + e +
        "}" + e + e +
        "#ifdef _SERVER\t// [MAIL 03/09] Ladder + GlbValue: chi may chu (dong bang #endif cuoi tep)" + e +
        "//////////////////////////////////////////////////////////////////////" + e +
        "// 2) Ladder - top 10 moi id, id > 10000, persist \\settings\\jx2ladder.txt",
        "cpp Ladder")
    return s


# 7) ScriptFuns.cpp - include, dang ky, tro SendScriptData sang ban that
def f_scriptfuns(s):
    s = insert_after(s, '#include "KGameKV.h"',
        '#include "KJx2SharedStore.h"\t// [MAIL 03/09] OB_* dung ca client\n#include "KScriptProtocol.h"\t// [MAIL 03/09] kenh ScriptProtocol',
        "include")
    s = insert_after(s, '{"DynamicExecuteByPlayer",\tLuaDynamicExecuteByPlayer},',
        "\t// [MAIL 03/09] kenh ScriptProtocol that (KScriptProtocol.cpp) - dung chung hai phia\n"
        "\t{\"EnsureScript\",\t\tLuaEnsureScript},\n"
        "#ifndef _SERVER\n"
        "\t// client: may chu da dang ky DynamicExecute + OB_* trong khoi _SERVER phia duoi\n"
        "\t{\"DynamicExecute\",\tLuaDynamicExecute},\n"
        "\t{\"SendScriptDataToServer\",\tLuaSendScriptDataToServer},\n"
        "\t{ \"OB_Create\",\tLuaOB_Create },\n"
        "\t{ \"OB_Release\",\tLuaOB_Release },\n"
        "\t{ \"OB_IsEmpty\",\tLuaOB_IsEmpty },\n"
        "\t{ \"OB_Clear\",\tLuaOB_Clear },\n"
        "\t{ \"OB_Append\",\tLuaOB_Append },\n"
        "\t{ \"OB_Copy\",\tLuaOB_Copy },\n"
        "\t{ \"OB_PushByte\",\tLuaOB_PushByte },\n"
        "\t{ \"OB_PopByte\",\tLuaOB_PopByte },\n"
        "\t{ \"OB_PushInt\",\tLuaOB_PushInt },\n"
        "\t{ \"OB_PopInt\",\tLuaOB_PopInt },\n"
        "\t{ \"OB_PushDouble\",\tLuaOB_PushDouble },\n"
        "\t{ \"OB_PopDouble\",\tLuaOB_PopDouble },\n"
        "\t{ \"OB_PushString\",\tLuaOB_PushString },\n"
        "\t{ \"OB_PopString\",\tLuaOB_PopString },\n"
        "#else\n"
        "\t{\"SendScriptDataToPlayer\",\tLuaSendScriptDataToPlayer},\n"
        "#endif",
        "dang ky")
    s = replace_once(s, '{"SendScriptData",\tLuaHD3_SendScriptData},',
        '{"SendScriptData",\tLuaSendScriptData},\t// [MAIL 03/09] ban that (KScriptProtocol.cpp), thay stub LuaHD3_SendScriptData',
        "SendScriptData")
    return s


# 8) KSortScript.cpp - dat co MODEL_* cho thu muc script_protocol (bo dieu phoi mo dau bang guard)
def f_sortscript(s):
    e = eol_of(s)
    old = ('\t\t\tlua_setglobal(g_ScriptSet[nCurrentScriptNum].m_LuaState, "MODEL_GAMESERVER");' + e +
           "\t\t}" + e)
    new = old + (
        "\t\t// [MAIL 03/09] bo dieu phoi ScriptProtocol (script_protocol\\protocol_def_gs.lua /" + e +
        "\t\t// protocol_def_c.lua) mo dau bang 'if MODEL_GAME* ~= 1 then return end' - dat co theo phia." + e +
        "\t\tif (strstr(szRelativeFile, \"script_protocol\") != NULL)" + e +
        "\t\t{" + e +
        "\t\t\tLua_PushNumber(g_ScriptSet[nCurrentScriptNum].m_LuaState, 1);" + e +
        "#ifdef _SERVER" + e +
        "\t\t\tlua_setglobal(g_ScriptSet[nCurrentScriptNum].m_LuaState, \"MODEL_GAMESERVER\");" + e +
        "#else" + e +
        "\t\t\tlua_setglobal(g_ScriptSet[nCurrentScriptNum].m_LuaState, \"MODEL_GAMECLIENT\");" + e +
        "#endif" + e +
        "\t\t}" + e)
    return replace_once(s, old, new, "MODEL flag")


# 9) Core.vcxproj - them KScriptProtocol.cpp, bo loai tru client cua KJx2SharedStore.cpp
def f_vcxproj(s):
    e = eol_of(s)
    s = replace_once(s, '<ClCompile Include="Src\\BauCua.cpp" />',
        '<ClCompile Include="Src\\BauCua.cpp" />' + e +
        '    <ClCompile Include="Src\\KScriptProtocol.cpp" />',
        "vcxproj them")
    old = ('    <ClCompile Include="Src\\KJx2SharedStore.cpp">' + e +
           "      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='Client Debug|Win32'\">true</ExcludedFromBuild>" + e +
           "      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='Client Debug|x64'\">true</ExcludedFromBuild>" + e +
           "      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='Client Release|Win32'\">true</ExcludedFromBuild>" + e +
           "      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='Client Release|x64'\">true</ExcludedFromBuild>" + e +
           "    </ClCompile>")
    new = '    <ClCompile Include="Src\\KJx2SharedStore.cpp" />'
    return replace_once(s, old, new, "vcxproj KJx2SharedStore")


def f_vcxproj_marked(s):
    # vcxproj khong the chua chuoi MARK trong XML thoai mai -> dung dau rieng
    if 'Src\\KScriptProtocol.cpp' in s:
        return None
    return f_vcxproj(s)


if __name__ == "__main__":
    patch(r"Headers\KProtocolDef.h", f_protocoldef)
    patch(r"Sources\Core\Src\KProtocol.cpp", f_protocolcpp)
    patch(r"Sources\Core\Src\KProtocolProcess.h", f_proch)
    patch(r"Sources\Core\Src\KProtocolProcess.cpp", f_proccpp)
    patch(r"Sources\Core\Src\KJx2SharedStore.h", f_storeh)
    patch(r"Sources\Core\Src\KJx2SharedStore.cpp", f_storecpp)
    patch(r"Sources\Core\Src\ScriptFuns.cpp", f_scriptfuns)
    patch(r"Sources\Core\Src\KSortScript.cpp", f_sortscript)
    p = os.path.join(ROOT, r"Sources\Core\Core.vcxproj")
    s = rd(p)
    s2 = f_vcxproj_marked(s)
    if s2 is None:
        print("  da va, bo qua: Core.vcxproj")
    else:
        wr(p, s2)
        print("  OK: Core.vcxproj")
    print("XONG" + (" (chi kiem tra)" if CHECK else ""))
