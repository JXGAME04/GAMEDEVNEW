# -*- coding: utf-8 -*-
#
# [LUACLIENT 09/09 F] Do ky hon: mat mot minh Include, hay mat ca bang toan cuc?
#
# Ban va 27 cho biet Include = nil ngay sau RegisterFunctions. Con hai kha nang:
#   (a) chi cac ham C cua GameScriptFuns khong vao duoc  -> loi o duong dang ky;
#   (b) ca ham thu vien chuan (type, table...) cung khong thay -> chunk chay
#       trong MOT bang toan cuc khac han voi cho dang ky.
# Phan biet bang cach do them ham thu vien chuan, va in ra so ham C ma
# g_GetGameScriptFunNum() bao (neu la 0 thi vong dang ky chay 0 lan).

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")

P = "Sources/Core/Src/KScriptProtocol.cpp"
DAU = "[LUACLIENT 09/09 F]"
NL = "\r\n"

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = NL.join([
        "\t// [LUACLIENT 09/09 E] do: chunk moi trong state nay co thay Include khong.",
        "\t// Loi (neu co) tu vao ScriptError.log qua l4_report.",
        "\tlua_dostring(p->m_LuaState,",
        "\t\t\"if Include == nil then error('[chan doan] Include = nil trong state vua tao') end\");",
    ])
    assert s.count(CU) == 1, "khong tim thay ban va 27 (thay %d)" % s.count(CU)
    MOI = NL.join([
        "\t// [LUACLIENT 09/09 F] do: sau khi dang ky, chunk moi trong CHINH state nay",
        "\t// thay duoc nhung gi. Loi (neu co) tu vao ScriptError.log qua l4_report.",
        "\tSP_ClientLog(\"[SP] dang ky %d ham C cho %s\", g_GetGameScriptFunNum(), szLow);",
        "\tlua_dostring(p->m_LuaState,",
        "\t\t\"local t = '' \"",
        "\t\t\"if Include == nil then t = t .. 'Include ' end \"",
        "\t\t\"if OB_Create == nil then t = t .. 'OB_Create ' end \"",
        "\t\t\"if _ALERT == nil then t = t .. '_ALERT ' end \"",
        "\t\t\"if type == nil then t = t .. 'type ' end \"",
        "\t\t\"if table == nil then t = t .. 'table ' end \"",
        "\t\t\"if string == nil then t = t .. 'string ' end \"",
        "\t\t\"if t ~= '' then error('[chan doan] thieu trong bang toan cuc: ' .. t) end\");",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
