# -*- coding: utf-8 -*-
#
# [LUACLIENT 09/09 B] lua4_outerrmsg chi viet ra stderr -> tren Android MAT TRANG.
#
# Chuoi lap luan (da do tung buoc, xem BANGIAO_ANDROID_PHA4_0809.md muc 9):
#   1. Tren may ao MOI chunk Lua phia client deu hong (jx_mail.log: 0 "ok" / 85 "LOI
#      than chunk") -> Thu, Dau gia, Chien Lenh deu chet vi ca ba deu chay bang Lua.
#   2. ScriptError.log chi co "ScriptError 4:[1] (<tep>)" - ma so, khong co cau loi.
#   3. Loi Lua di qua lua4_call -> ham nhan loi l4_msgh -> l4_report. l4_report thu
#      _ERRORMESSAGE, roi _ALERT, cuoi cung moi rot xuong lua4_outerrmsg.
#   4. Khong dong nao cua _ALERT hien ra -> tuc la da rot xuong nhanh cuoi, ma nhanh
#      cuoi chi fputs(stderr). Windows con thay o console; Android thi khong dau ca.
#
# Sua: cho lua4_outerrmsg ghi ra CA stderr LAN ScriptError.log - dung tep ma
# KLuaScript::ScriptError va LuaGameAlert dang ghi, nen doc mot cho la du.
# Duong nay CHI chay khi da co loi, khong dung vao mach chay binh thuong.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")

P = "Sources/Library/Lua54/lua4compat.c"
DAU = "[LUACLIENT 09/09 B]"
NL = "\r\n"

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = ("LUA_API void lua4_outerrmsg(const char* szerrmsg)" + NL
          + "{" + NL
          + "\tif (szerrmsg) { fputs(szerrmsg, stderr); fflush(stderr); }" + NL
          + "}")
    assert s.count(CU) == 1, "khong tim thay lua4_outerrmsg (thay %d)" % s.count(CU)
    MOI = NL.join([
        "/* [LUACLIENT 09/09 B] Truoc day chi fputs(stderr): Android khong co stderr nen",
        "   cau loi Lua bay hoi, chi con ma so trong ScriptError.log. Ghi ra ca tep de",
        "   doc duoc o moi nen. Cung ten tep ma KLuaScript::ScriptError dang ghi. */",
        "LUA_API void lua4_outerrmsg(const char* szerrmsg)",
        "{",
        "\tFILE* f;",
        "\tif (!szerrmsg) return;",
        "\tfputs(szerrmsg, stderr); fflush(stderr);",
        "\tf = fopen(\"ScriptError.log\", \"a\");",
        "\tif (!f) return;",
        "\tfputs(szerrmsg, f);",
        "\tfclose(f);",
        "}",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
