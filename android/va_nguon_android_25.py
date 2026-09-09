# -*- coding: utf-8 -*-
#
# [LUACLIENT 09/09 C] lua4_execute thoat IM LANG khi dinh stack khong phai ham.
#
# Ban cu:
#     if (!lua_isfunction(L, -1)) return L4_ERRRUN;
# Tra ve dung ma loi 1 - GIONG HET mot loi chay that - nhung khong he goi pcall,
# nen khong co ham nhan loi, khong co thong diep, khong co traceback. Ben goi
# (KLuaScript::ExecuteCode) chi ghi duoc "ScriptError 4:[1] (<tep>)".
#
# Hai canh nay KHONG the phan biet tu ben ngoai, va do la ly do mat hang gio khi
# truy vi sao Thu / Dau gia / Chien Lenh chet tren Android:
#     (a) than chunk chay va nem loi that
#     (b) chunk chua bao gio duoc chay vi dinh stack khong phai ham
#
# Sua: noi ro canh (b) ra - ghi so o tren stack va kieu cua o tren cung. Chi them
# mot cau bao truoc khi tra ve dung ma loi cu, khong doi hanh vi.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")

P = "Sources/Library/Lua54/lua4compat.c"
DAU = "[LUACLIENT 09/09 C]"
NL = "\r\n"

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = ("LUA_API int lua4_execute(lua_State* L)" + NL
          + "{" + NL
          + "\tif (!lua_isfunction(L, -1)) return L4_ERRRUN;" + NL
          + "\treturn lua4_call(L, 0, LUA_MULTRET);" + NL
          + "}")
    assert s.count(CU) == 1, "khong tim thay lua4_execute (thay %d)" % s.count(CU)
    MOI = NL.join([
        "LUA_API int lua4_execute(lua_State* L)",
        "{",
        "\tif (!lua_isfunction(L, -1))",
        "\t{",
        "\t\t/* [LUACLIENT 09/09 C] truoc day return im lang: ben goi chi thay ma loi 1,",
        "\t\t   khong phan biet duoc voi loi chay that trong than chunk. Noi ro ra. */",
        "\t\tchar szBao[160];",
        "\t\tint nTop = lua_gettop(L);",
        "\t\tsprintf(szBao, \"[lua4] execute: dinh stack khong phai ham \"",
        "\t\t\t\"(so o tren stack = %d, kieu o tren cung = %s)\\n\",",
        "\t\t\tnTop, nTop > 0 ? luaL_typename(L, -1) : \"stack trong\");",
        "\t\tlua4_outerrmsg(szBao);",
        "\t\treturn L4_ERRRUN;",
        "\t}",
        "\treturn lua4_call(L, 0, LUA_MULTRET);",
        "}",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
