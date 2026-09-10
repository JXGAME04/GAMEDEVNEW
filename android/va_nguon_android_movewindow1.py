# -*- coding: utf-8 -*-
#
# [MOVEWINDOW 12/09] Ban do nho (va statustool/normaltool/chatroom) tren man rong khong nam sat mep phai du bang toa do
# da ap dung (BOCUC-AP rel 1004 -> thuc te 856): sau khi vao game, UserData\<tk>\uiconfig.ini [ScriptAuto] chay lai
# MoveWindow("map", 856, 12) (vi tri luu tu phien truoc, LuaMoveWindow -> SetPosition) DE LEN bang toa do.
# Sua (chi Android): sau SetPosition cua MoveWindow, ap lai muc trong bang neu cua so co muc (bang toa do thang).
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[MOVEWINDOW 12/09]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (s.count(c), ten))
    return s.replace(c, nl.join(moi))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


P = "Sources/S3Client/Ui/ShortcutKey.cpp"
s = doc(P)
if DAU not in s:
    s0 = s
    s = thay(s, ["\tKWndWindow* pWin = FindWndWindow(strWindow);",
                 "\tif (pWin)",
                 "\t{",
                 "\t\tpWin->SetPosition(nX, nY);",
                 "\t}",
                 "\treturn 0;",
                 "}",
                 "",
                 "int LuaSizeWindow(Lua_State * L)"],
                ["\tKWndWindow* pWin = FindWndWindow(strWindow);",
                 "\tif (pWin)",
                 "\t{",
                 "\t\tpWin->SetPosition(nX, nY);",
                 "#ifdef JX_ANDROID",
                 "\t\t// %s vi tri luu tu phien truoc (uiconfig.ini [ScriptAuto]) khong duoc de len bang toa do" % DAU,
                 "\t\t// theo neo cua man hinh nay (ban do nho ke mep phai...): co muc trong bang thi ap lai muc do.",
                 "\t\tUiToaDo_ApChoO(pWin);",
                 "#endif",
                 "\t}",
                 "\treturn 0;",
                 "}",
                 "",
                 "int LuaSizeWindow(Lua_State * L)"], "LuaMoveWindow")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi")
    ghi(P, s); print("da va:", P)
print("xong")
