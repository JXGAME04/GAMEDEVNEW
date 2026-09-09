# -*- coding: utf-8 -*-
#
# [LUACLIENT 09/09] Loi Lua phia client BIEN MAT -> khong sua duoc Thu / Dau gia / Chien Lenh.
#
# Do duoc: tren may ao, MOI chunk Lua phia client deu hong (jx_mail.log: 85 lan
# "LOI than chunk", 0 lan "ok"), nen ca ba he Thu / Dau gia / Chien Lenh deu chet
# theo. Nhung ScriptError.log chi ghi duoc "ScriptError 4:[1] (<ten tep>)" -
# tuc "than chunk chay loi, ma loi 1" - KHONG co cau loi that.
#
# Vi sao mat cau loi: Lua 4 khi gap loi chay goi ham toan cuc _ERRORMESSAGE.
# Ban dung o day la ban NOI SINH cua thu vien (lstate.c:33 errormessage) chi lam
# dung mot viec: fprintf(stderr, ...). Tren Windows stderr ra cua so console,
# tren Android stderr KHONG DI DAU CA -> cau loi bay hoi.
#
# Ghi chu 21/08 trong ScriptFuns.cpp da dinh vay: dang ky {"_ALERT", LuaGameAlert}
# de loi ghi vao ScriptError.log. Nhung ban _ERRORMESSAGE noi sinh KHONG he goi
# _ALERT (ban chuan cua Lua moi goi), nen dong dang ky _ALERT do tu 21/08 den nay
# la MA CHET - chua bao gio bat duoc cau loi nao.
#
# Sua: dang ky luon _ERRORMESSAGE ve cung ham C. Chi them mot dong bang ham.
# An toan cho ca PC lan may chu: duong nay CHI chay khi da co loi, va viec no lam
# la ghi them chu vao dung tep ScriptError.log ma ScriptError() dang ghi.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")

P = "Sources/Core/Src/ScriptFuns.cpp"
DAU = "[LUACLIENT 09/09]"
NL = "\r\n"

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = ('\t{"_ALERT",LuaGameAlert},\t'
          "// [WLLS 21/08] loi runtime Lua ghi nguyen van vao ScriptError.log")
    assert s.count(CU) == 1, "khong tim thay dong _ALERT (thay %d)" % s.count(CU)
    MOI = CU + NL + NL.join([
        "\t// [LUACLIENT 09/09] _ERRORMESSAGE: ban noi sinh cua thu vien (lstate.c:33)",
        "\t// chi fprintf ra stderr - Windows con thay o console, Android thi MAT HAN,",
        "\t// va no KHONG goi _ALERT nen dong tren mot minh khong bat duoc gi. Tro ve",
        '\t// cung ham C thi cau loi that ("attempt to index global ...", so dong) vao',
        '\t// thang ScriptError.log, canh dong "ScriptError 4:[1]" von chi co ma so.',
        '\t{"_ERRORMESSAGE",LuaGameAlert},',
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
