# -*- coding: utf-8 -*-
#
# [LUACLIENT 09/09 E] Do xem "Include" mat tu luc nao.
#
# Ban va 26 da lay duoc cau loi that:
#     \script\script_protocol\protocol_def_c.lua:10:
#     attempt to call a nil value (global 'Include')
# Tuc than chunk chet ngay dong Include dau tien -> moi script client deu chet
# -> Thu / Dau gia / Chien Lenh deu khong chay.
#
# sClientLoad CO goi RegisterFunctions(GameScriptFuns, ...) va bang do CO muc
# {"Include", LuaIncludeFile} (ScriptFuns.cpp:14955, ngoai moi #ifdef _SERVER).
# Vay phai do: sau khi dang ky xong, mot chunk moi bien dich trong CHINH state do
# co nhin thay Include khong.
#
# Phep thu chay bang lua_dostring - dung duong ma than chunk di (chunk moi, cung
# bang toan cuc), nen phan biet duoc hai kha nang:
#   - bao loi "[chan doan] Include = nil ..."  -> dang ky KHONG vao bang toan cuc;
#   - im lang                                  -> dang ky OK, chunk cua Load()
#                                                 chay trong moi truong KHAC.
# Cau loi tu di vao ScriptError.log qua l4_report (ban va 26).

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")

P = "Sources/Core/Src/KScriptProtocol.cpp"
DAU = "[LUACLIENT 09/09 E]"
NL = "\r\n"

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = "\tint nOk = p->Load((char*)szLow);"
    assert s.count(CU) == 1, "khong tim thay dong Load (thay %d)" % s.count(CU)
    MOI = NL.join([
        "\t// [LUACLIENT 09/09 E] do: chunk moi trong state nay co thay Include khong.",
        "\t// Loi (neu co) tu vao ScriptError.log qua l4_report.",
        "\tlua_dostring(p->m_LuaState,",
        "\t\t\"if Include == nil then error('[chan doan] Include = nil trong state vua tao') end\");",
        CU,
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
