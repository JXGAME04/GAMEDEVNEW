# -*- coding: utf-8 -*-
#
# [LUACLIENT 09/09 D] l4_report: ghi THANG cau loi ra nhat ky, khong phu thuoc moc script.
#
# Tu khi chuyen sang loi Lua 5.4 (05/09), moi cau loi Lua deu bien mat. Da do:
#   - lua4_execute CO goi pcall (ban va 25 khong nổ) -> co loi chay that;
#   - nhung ScriptError.log chi co dong "ScriptError 4:[1] (<tep>)" cua ben goi,
#     khong co cau loi nao -> ca ba nhanh cua l4_report deu khong ra chu.
#
# l4_report cu di vong: tim _ERRORMESSAGE, khong co thi _ALERT, khong co nua moi
# lua4_outerrmsg. Duong vong nay phu thuoc vao trang thai dang ky ham cua tung
# state - dung mot mat xich la mat sach thong diep, dung nhu dang xay ra.
#
# Sua: ghi thang bang lua4_outerrmsg NGAY (ban va 24 da cho no ra ca tep), roi moi
# goi moc cua script. Chi goi moc khi do la ham LUA (moc rieng cua script); moc
# ham C trong du an nay la LuaGameAlert - cung ghi vao dung tep ay - nen bo qua
# de khoi ra hai dong trung nhau.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")

P = "Sources/Library/Lua54/lua4compat.c"
DAU = "[LUACLIENT 09/09 D]"
NL = "\r\n"

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = NL.join([
        "static void l4_report(lua_State* L)",
        "{",
        "\tint top = lua_gettop(L);",
        "\tif (top < 1) return;",
        "\tlua4_getglobal(L, \"_ERRORMESSAGE\");\t\t/* [LUA54 06/09 toi] theo E cua script (mot state) roi bang chu */",
        "\tif (!lua_isfunction(L, -1))",
        "\t{",
        "\t\tlua_pop(L, 1);",
        "\t\tlua4_getglobal(L, \"_ALERT\");",
        "\t}",
        "\tif (lua_isfunction(L, -1))",
        "\t{",
        "\t\tlua_pushvalue(L, top);",
        "\t\tif (lua_pcall(L, 1, 0, 0) != LUA_OK)",
        "\t\t\tlua_pop(L, 1);",
        "\t}",
        "\telse",
        "\t{",
        "\t\tlua_pop(L, 1);",
        "\t\tlua4_outerrmsg(lua_tostring(L, top) ? lua_tostring(L, top) : \"(loi khong phai chuoi)\");",
        "\t\tlua4_outerrmsg(\"\\n\");",
        "\t}",
        "\tlua_settop(L, top);",
        "}",
    ])
    assert s.count(CU) == 1, "khong tim thay l4_report (thay %d)" % s.count(CU)
    MOI = NL.join([
        "static void l4_report(lua_State* L)",
        "{",
        "\tint top = lua_gettop(L);",
        "\tif (top < 1) return;",
        "\t/* [LUACLIENT 09/09 D] Ghi THANG ra nhat ky truoc tien. Duong vong qua",
        "\t   _ERRORMESSAGE / _ALERT phu thuoc trang thai dang ky cua tung state;",
        "\t   dut mot mat xich la mat sach cau loi (do duoc tren Android 09/09).",
        "\t   lua4_outerrmsg ghi ra ca stderr lan ScriptError.log. */",
        "\tlua4_outerrmsg(lua_tostring(L, top) ? lua_tostring(L, top) : \"(loi khong phai chuoi)\");",
        "\tlua4_outerrmsg(\"\\n\");",
        "\tlua4_getglobal(L, \"_ERRORMESSAGE\");\t\t/* [LUA54 06/09 toi] theo E cua script (mot state) roi bang chu */",
        "\tif (!lua_isfunction(L, -1))",
        "\t{",
        "\t\tlua_pop(L, 1);",
        "\t\tlua4_getglobal(L, \"_ALERT\");",
        "\t}",
        "\t/* Chi goi moc rieng cua script (ham LUA). Moc ham C trong du an nay la",
        "\t   LuaGameAlert - cung ghi vao ScriptError.log - goi nua thi ra hai dong",
        "\t   trung nhau, nen bo qua. */",
        "\tif (lua_isfunction(L, -1) && !lua_iscfunction(L, -1))",
        "\t{",
        "\t\tlua_pushvalue(L, top);",
        "\t\tif (lua_pcall(L, 1, 0, 0) != LUA_OK)",
        "\t\t\tlua_pop(L, 1);",
        "\t}",
        "\telse",
        "\t\tlua_pop(L, 1);",
        "\tlua_settop(L, top);",
        "}",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
