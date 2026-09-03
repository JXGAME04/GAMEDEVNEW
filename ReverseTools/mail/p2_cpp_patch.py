# -*- coding: utf-8 -*-
"""[MAIL 03/09] Dot 2 - va C++ trong worktree D:\\GAMEDEVNEW_wt_mail: cua so thu.
Doc/ghi latin-1, neo duy nhat, idempotent theo dau [MAIL 03/09 D2]. Chay: python p2_cpp_patch.py [--check]
"""
import io, os, re, sys

ROOT = r"D:\GAMEDEVNEW_wt_mail"
MARK = "[MAIL 03/09 D2]"
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
    n = s.count(anchor)
    assert n == 1, "%s: neo %r khop %d lan" % (label, anchor, n)
    i = s.index(anchor)
    j = s.index(eol_of(s), i) + len(eol_of(s))
    return s[:j] + block.replace("\n", eol_of(s)) + eol_of(s) + s[j:]


def insert_before(s, anchor, block, label):
    n = s.count(anchor)
    assert n == 1, "%s: neo %r khop %d lan" % (label, anchor, n)
    i = s.index(anchor)
    # dau dong chua anchor
    k = s.rfind(eol_of(s), 0, i)
    k = 0 if k < 0 else k + len(eol_of(s))
    return s[:k] + block.replace("\n", eol_of(s)) + eol_of(s) + s[k:]


def replace_once(s, old, new, label):
    n = s.count(old)
    assert n == 1, "%s: neo %r khop %d lan" % (label, old[:50], n)
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


# 1) CoreShell.h - 2 enum (CUOI)
def f_coreshell_h(s):
    s = insert_after(s, "\tGDCNI_OPEN_SMELT_BOX,",
        "\t// [MAIL 03/09 D2] cua so THU: uParam = MAILUI_CMD_* (KMailUiDef.h), nParam = con tro / so. Dat CUOI enum.\n"
        "\tGDCNI_MAIL_UI,", "GDCNI")
    s = insert_after(s, "\tGDI_HS_SP,",
        "\n\t// [MAIL 03/09 D2] cua so THU -> Lua uimail.lua (KMailClient.cpp): uParam = MAILUI_OP_MAKE(op, extra), nParam. Dat CUOI enum.\n"
        "\tGOI_MAIL_UI,", "GOI")
    return s


# 2) CoreShell.cpp - include + case
def f_coreshell_cpp(s):
    e = eol_of(s)
    lines = s.split(e)
    last_inc = -1
    for i in range(0, min(len(lines), 260)):
        if lines[i].startswith('#include "'):
            last_inc = i
    assert last_inc >= 0, "CoreShell.cpp: khong thay #include"
    lines.insert(last_inc + 1, '#include "KMailClient.h"\t// [MAIL 03/09 D2] cua so thu')
    s = e.join(lines)
    s = insert_before(s, "\tcase GOI_DICE_CHOICE:\t// DICEITEM 26/08",
        "\tcase GOI_MAIL_UI:\t// [MAIL 03/09 D2] cua so thu -> Lua uimail.lua (KMailClient.cpp)\n"
        "\t{\n"
        "\t\tMailUi_OnRequest(uParam, nParam);\n"
        "\t}\n"
        "\tbreak;", "case GOI_MAIL_UI")
    return s


# 3) GameSpaceChangedNotify.cpp - include + case
def f_gscn(s):
    s = insert_after(s, '#include "UiCase/UiDiceItem.h"\t// DICEITEM 26/08',
        '#include "UiCase/UiMail.h"\t// [MAIL 03/09 D2] cua so thu', "include UiMail")
    s = insert_before(s, "\tcase GDCNI_OPEN_TREMBLE_ITEM:",
        "\tcase GDCNI_MAIL_UI:\t// [MAIL 03/09 D2] uParam = MAILUI_CMD_*, nParam = con tro / so (song trong loi goi)\n"
        "\t\tKUiMail_OnCoreCmd(uParam, nParam);\n"
        "\t\tbreak;", "case GDCNI_MAIL_UI")
    return s


# 4) ScriptFuns.cpp - include + dang ky 18 ham client
def f_scriptfuns(s):
    s = insert_after(s, '#include "KScriptProtocol.h"\t// [MAIL 03/09] kenh ScriptProtocol',
        '#include "KMailClient.h"\t// [MAIL 03/09 D2] cua so thu (client)', "include")
    s = insert_after(s, '\t{"SendScriptDataToServer",\tLuaSendScriptDataToServer},',
        "\t// [MAIL 03/09 D2] 15 ham 2.0 ma \\script\\ui\\uimail.lua goi + 3 ham phu (KMailClient.cpp)\n"
        "\t{\"AddMailHeader\",\tLuaMail_AddMailHeader},\n"
        "\t{\"SetMailHeader\",\tLuaMail_SetMailHeader},\n"
        "\t{\"DeleteOneMail\",\tLuaMail_DeleteOneMail},\n"
        "\t{\"CleanMailAll\",\tLuaMail_CleanMailAll},\n"
        "\t{\"CleanMailList\",\tLuaMail_CleanMailList},\n"
        "\t{\"CleanMailDetail\",\tLuaMail_CleanMailDetail},\n"
        "\t{\"UpdateMailCount\",\tLuaMail_UpdateMailCount},\n"
        "\t{\"NewMailUIEventArrival\",\tLuaMail_NewMailUIEventArrival},\n"
        "\t{\"SwitchMailManager\",\tLuaMail_SwitchMailManager},\n"
        "\t{\"OpenMailWindow\",\tLuaMail_OpenMailWindow},\n"
        "\t{\"SetMailIconVisible\",\tLuaMail_SetMailIconVisible},\n"
        "\t{\"SelectMail\",\tLuaMail_SelectMail},\n"
        "\t{\"SetFilterText\",\tLuaMail_SetFilterText},\n"
        "\t{\"SetMailBntStatus\",\tLuaMail_SetMailBntStatus},\n"
        "\t{\"UpdateMailDetail\",\tLuaMail_UpdateMailDetail},\n"
        "\t{\"FormatTime2String\",\tLuaMail_FormatTime2String},\n"
        "\t{\"MailConfirm\",\tLuaMail_MailConfirm},\n"
        "\t{\"Msg2Player\",\tLuaMail_Msg2Player},\t// client: thong bao he thong (may chu co ban rieng trong khoi _SERVER)",
        "dang ky")
    return s


# 5) KScriptProtocol.h/.cpp - SP_RunClientLua + PlayerIndex cho client
def f_sp_h(s):
    return insert_after(s, "int  LuaSendScriptDataToServer(Lua_State* L);",
        "int  SP_RunClientLua(const char* szScript, const char* szCall);\t// [MAIL 03/09 D2] chay 1 cau Lua trong state cua script (nap neu chua)",
        "SP_RunClientLua decl")


def f_sp_cpp(s):
    e = eol_of(s)
    old = ("#else" + e +
           "\tLua_PushNumber(L, 1);" + e +
           "\tpScript->SetGlobalName((LPSTR)\"MODEL_GAMECLIENT\");" + e +
           "#endif" + e +
           "\tchar szCall[128];")
    new = ("#else" + e +
           "\tLua_PushNumber(L, 1);" + e +
           "\tpScript->SetGlobalName((LPSTR)\"MODEL_GAMECLIENT\");" + e +
           "\t// [MAIL 03/09 D2] client: PlayerIndex = 1 de Talk/Say trong handler chay duoc" + e +
           "\tLua_PushNumber(L, CLIENT_PLAYER_INDEX);" + e +
           "\tpScript->SetGlobalName((LPSTR)SCRIPT_PLAYERINDEX);" + e +
           "#endif" + e +
           "\tchar szCall[128];")
    s = replace_once(s, old, new, "PlayerIndex client")
    s = insert_before(s, "// SendScriptDataToServer(nProtocolId, hOB) -> 1/0",
        "// [MAIL 03/09 D2] chay mot cau Lua (vi du \"UIMail:OnSelect(5)\") trong state cua szScript; nap neu chua.\n"
        "// Dat MODEL_GAMECLIENT + PlayerIndex nhu SP_Dispatch. Tra 1 neu chay duoc.\n"
        "int SP_RunClientLua(const char* szScript, const char* szCall)\n"
        "{\n"
        "\tif (!szCall || !szCall[0])\n"
        "\t\treturn 0;\n"
        "\tKLuaScript* pScript = SP_GetScript(szScript, 1);\n"
        "\tif (!pScript)\n"
        "\t{\n"
        "\t\tg_DebugLog((LPSTR)\"[SP] RunClientLua: khong nap duoc %.128s\", szScript);\n"
        "\t\treturn 0;\n"
        "\t}\n"
        "\tLua_State* L = pScript->m_LuaState;\n"
        "\tLua_PushNumber(L, 1);\n"
        "\tpScript->SetGlobalName((LPSTR)\"MODEL_GAMECLIENT\");\n"
        "\tLua_PushNumber(L, CLIENT_PLAYER_INDEX);\n"
        "\tpScript->SetGlobalName((LPSTR)SCRIPT_PLAYERINDEX);\n"
        "\tint nTop = 0;\n"
        "\tpScript->SafeCallBegin(&nTop);\n"
        "\tint nRet = 1;\n"
        "\tif (lua_dostring(L, szCall) != 0)\n"
        "\t{\n"
        "\t\tg_DebugLog((LPSTR)\"[SP] RunClientLua loi %.128s: %.200s\", szScript, szCall);\n"
        "\t\tnRet = 0;\n"
        "\t}\n"
        "\tpScript->SafeCallEnd(nTop);\n"
        "\treturn nRet;\n"
        "}\n", "SP_RunClientLua def")
    return s


# 6) vcxproj
def f_core_vcx(s):
    e = eol_of(s)
    return replace_once(s, '<ClCompile Include="Src\\KScriptProtocol.cpp" />',
        '<ClCompile Include="Src\\KScriptProtocol.cpp" />' + e +
        '    <ClCompile Include="Src\\KMailClient.cpp" />', "Core.vcxproj")


def f_s3_vcx(s):
    e = eol_of(s)
    s = replace_once(s, '<ClCompile Include="ui\\uicase\\UiDiceItem.cpp" />',
        '<ClCompile Include="ui\\uicase\\UiDiceItem.cpp" />' + e +
        '    <ClCompile Include="ui\\uicase\\UiMail.cpp" />', "S3Client.vcxproj cpp")
    s = replace_once(s, '<ClInclude Include="ui\\uicase\\UiDiceItem.h" />',
        '<ClInclude Include="ui\\uicase\\UiDiceItem.h" />' + e +
        '    <ClInclude Include="ui\\uicase\\UiMail.h" />', "S3Client.vcxproj h")
    return s


if __name__ == "__main__":
    patch(r"Sources\Core\Src\CoreShell.h", f_coreshell_h)
    patch(r"Sources\Core\Src\CoreShell.cpp", f_coreshell_cpp)
    patch(r"Sources\S3Client\Ui\GameSpaceChangedNotify.cpp", f_gscn)
    patch(r"Sources\Core\Src\ScriptFuns.cpp", f_scriptfuns)
    patch(r"Sources\Core\Src\KScriptProtocol.h", f_sp_h)
    patch(r"Sources\Core\Src\KScriptProtocol.cpp", f_sp_cpp)
    for rel, fn, key in [(r"Sources\Core\Core.vcxproj", f_core_vcx, "KMailClient.cpp"),
                         (r"Sources\S3Client\S3Client.vcxproj", f_s3_vcx, "UiMail.cpp")]:
        p = os.path.join(ROOT, rel)
        s = rd(p)
        if key in s:
            print("  da va, bo qua:", rel)
            continue
        wr(p, fn(s))
        print("  OK:", rel)
    print("XONG" + (" (chi kiem tra)" if CHECK else ""))
