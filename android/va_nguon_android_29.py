# -*- coding: utf-8 -*-
#
# [LUACLIENT 09/09 G] Ba du kien de chot: ban thu 80 ham dang ky di dau.
#
# Ban va 28 cho: g_GetGameScriptFunNum() = 80, va sau khi dang ky thi Include /
# OB_Create van nil, trong khi type / table / string / _ALERT co (nhung cai nay
# do thu vien chuan tao, khong phai ta dang ky).
#
# GameScriptFuns[0] chinh la {"Include", LuaIncludeFile} nen le ra phai co. Do them:
#   - ten mucImpl dau bang doc tu chinh Core (bang co dung khong, con tro co thang khong)
#   - lua4_mot_state(): che do MOT STATE doi han cho dat/lay bien toan cuc
#   - con tro state co that su khac NULL khong (RegisterFunctions im lang tra FALSE neu NULL)

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")

P = "Sources/Core/Src/KScriptProtocol.cpp"
DAU = "[LUACLIENT 09/09 G]"
NL = "\r\n"

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = "\tSP_ClientLog(\"[SP] dang ky %d ham C cho %s\", g_GetGameScriptFunNum(), szLow);"
    assert s.count(CU) == 1, "khong tim thay dong log cua ban va 28 (thay %d)" % s.count(CU)
    MOI = NL.join([
        "\t// [LUACLIENT 09/09 G] ba du kien de chot xem 80 ham dang ky di dau",
        "\tSP_ClientLog(\"[SP] dang ky %d ham C cho %s | mucImpl[0]=%s | mot_state=%d | state=%p\",",
        "\t\tg_GetGameScriptFunNum(), szLow,",
        "\t\tGameScriptFuns[0].name ? GameScriptFuns[0].name : \"(rong)\",",
        "\t\tlua4_mot_state(), (void*)p->m_LuaState);",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
