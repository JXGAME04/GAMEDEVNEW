# -*- coding: utf-8 -*-
"""[MAIL 03/09] Dot 3 - va C++ trong worktree: dang ky MailDB_* (server) + Core.vcxproj.
Doc/ghi latin-1, neo duy nhat, idempotent theo dau [MAIL 03/09 D3]. Chay: python p3_cpp_patch.py [--check]
"""
import io, os, sys

ROOT = r"D:\GAMEDEVNEW_wt_mail"
MARK = "[MAIL 03/09 D3]"
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


def f_scriptfuns(s):
    s = insert_after(s, '#include "KMailClient.h"\t// [MAIL 03/09 D2] cua so thu (client)',
        '#include "KMailServer.h"\t// [MAIL 03/09 D3] kho thu MySQL (may chu)', "include")
    s = insert_after(s, '\t{"SendScriptDataToPlayer",\tLuaSendScriptDataToPlayer},',
        "\t// [MAIL 03/09 D3] kho THU tren MySQL (KMailServer.cpp) - script\\mail\\mailmanager.lua dung\n"
        "\t{\"MailDB_Ready\",\tLuaMailDB_Ready},\n"
        "\t{\"MailDB_Send\",\tLuaMailDB_Send},\n"
        "\t{\"MailDB_Headers\",\tLuaMailDB_Headers},\n"
        "\t{\"MailDB_Get\",\tLuaMailDB_Get},\n"
        "\t{\"MailDB_SetState\",\tLuaMailDB_SetState},\n"
        "\t{\"MailDB_Delete\",\tLuaMailDB_Delete},\n"
        "\t{\"MailDB_Count\",\tLuaMailDB_Count},\n"
        "\t{\"MailDB_PollNew\",\tLuaMailDB_PollNew},\n"
        "\t{\"MailDB_MaxId\",\tLuaMailDB_MaxId},\n"
        "\t{\"MailDB_Sweep\",\tLuaMailDB_Sweep},",
        "dang ky server")
    return s


def f_core_vcx(s):
    e = eol_of(s)
    return replace_once(s, '<ClCompile Include="Src\\KMailClient.cpp" />',
        '<ClCompile Include="Src\\KMailClient.cpp" />' + e +
        '    <ClCompile Include="Src\\KMailServer.cpp" />', "Core.vcxproj")


if __name__ == "__main__":
    patch(r"Sources\Core\Src\ScriptFuns.cpp", f_scriptfuns)
    p = os.path.join(ROOT, r"Sources\Core\Core.vcxproj")
    s = rd(p)
    if "KMailServer.cpp" in s:
        print("  da va, bo qua: Core.vcxproj")
    else:
        wr(p, f_core_vcx(s))
        print("  OK: Core.vcxproj")
    print("XONG" + (" (chi kiem tra)" if CHECK else ""))
