# -*- coding: utf-8 -*-
#
# [LUACLIENT 09/09 H] SUA GOC: hai mang toan cuc TRUNG TEN "GameScriptFuns".
#
# Chuoi do dan den day (BANGIAO_ANDROID_PHA4_0809.md muc 9):
#     GameScriptFuns[0].name = "Mouse_Action" | mot_state=0 | 80 ham
# Trong khi bang cua Core bat dau bang {"Include", LuaIncludeFile} va co 1351 muc.
#
# Co HAI mang toan cuc cung ten trong cung mot tien trinh:
#   1. Sources/Core/Src/ScriptFuns.cpp:14954   -> libCoreClient.so  (bang lon: Include, OB_Create...)
#   2. Sources/S3Client/Ui/ShortcutKey.cpp:2343 -> libmain.so       (80 muc: Mouse_Action...)
# va ca ham dem g_GetGameScriptFunNum() cung bi nhan doi y het.
#
# Tren Windows moi DLL giu ban rieng nen khong ai va cham -> ban PC chay dung.
# Tren Android moi thu la thu vien chia se ELF: ky hieu de o muc thay duoc mac dinh,
# bo nap dong chi giu MOT dinh nghia cho ca tien trinh, va libmain.so thang.
# Ket qua: Core dang ky nham 80 ham phim tat cho MOI script phia client -> Include = nil
# -> moi chunk chet ngay dong dau -> Thu / Dau gia / Chien Lenh (va moi tinh nang
# chay bang Lua phia client) deu khong hoat dong.
#
# Sua: cho ban cua S3Client lien ket NOI BO (static). Ca hai deu chi duoc dung ngay
# trong ShortcutKey.cpp (dong 2343 dinh nghia, 2431 va 2436 dung) nen khong anh huong
# ai khac. Ban PC khong doi hanh vi: truoc gio no van dung dung bang cua chinh no.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")

P = "Sources/S3Client/Ui/ShortcutKey.cpp"
DAU = "[LUACLIENT 09/09 H]"
NL = "\r\n"

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU1 = "TLua_Funcs GameScriptFuns[] = "
    assert s.count(CU1) == 1, "khong tim thay bang GameScriptFuns (thay %d)" % s.count(CU1)
    MOI1 = NL.join([
        "// [LUACLIENT 09/09 H] PHAI la static: Core/Src/ScriptFuns.cpp cung co mot mang",
        "// toan cuc TEN Y HET (bang script lon, bat dau bang \"Include\"). Windows cho moi",
        "// DLL mot ban rieng nen khong sao; tren Android moi thu la .so chia se, bo nap dong",
        "// chi giu MOT dinh nghia va libmain.so thang -> Core dang ky nham 80 ham phim tat",
        "// nay cho moi script phia client, Include hoa nil, moi chunk chet o dong dau.",
        "static TLua_Funcs GameScriptFuns[] = ",
    ])
    s = s.replace(CU1, MOI1)

    CU2 = "int g_GetGameScriptFunNum()"
    assert s.count(CU2) == 1, "khong tim thay g_GetGameScriptFunNum (thay %d)" % s.count(CU2)
    MOI2 = NL.join([
        "// [LUACLIENT 09/09 H] static vi ly do y het bang o tren - Core cung co ham nay.",
        "static int g_GetGameScriptFunNum()",
    ])
    s = s.replace(CU2, MOI2)

    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
