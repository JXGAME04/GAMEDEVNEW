# -*- coding: utf-8 -*-
#
# [ANDROID 09/09 PHIMMA] Che do sua giao dien TU BAT sau khi vao game.
#
# Do duoc: 18 giay sau khi mo app thi chua bat, 58 giay thi da bat. Khong ai cham
# vao dau. Da loai tru:
#   - nut "Sua giao dien" cua minh: day han ra ngoai man hinh (SuaToaDoNutX=5000)
#     ma van bat;
#   - dong AddCommand("Ctrl+U"...) trong ui\autoexec.lua: bo di van bat (ban ghi
#     phim tat con nam trong UserData\<nhan vat>\uiconfig.ini nua).
#
# Chi con duong ShortcutKey: mot su kien phim NAO DO khop voi Ctrl+U. Dat vet de
# biet chinh xac ma phim va co bo trong nao dang bat.
#
# Hai dong log nay GIU LAI duoc: chung chi chay khi mot phim tat that su kich, va
# tren Android "phim ma" kieu nay rat kho doan neu khong co vet.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
DAU = "[ANDROID 09/09 PHIMMA]"

# ---------------------------------------------------------------- 1. ai bat che do sua
P = "Sources/S3Client/Ui/Elem/UiToaDo.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = NL.join([
        "\telse",
        "\t{",
        "\t\ts_bDangSua = true;",
    ])
    assert s.count(CU) == 1, "khong tim thay nhanh bat che do sua (%d)" % s.count(CU)
    MOI = NL.join([
        "\telse",
        "\t{",
        "\t\t// [ANDROID 09/09 PHIMMA] ghi vet: che do nay tung TU BAT tren Android",
        "\t\tg_DebugLog(\"[UITOADO] BAT che do sua giao dien\");",
        "\t\ts_bDangSua = true;",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ---------------------------------------------------------------- 2. phim tat nao kich
P = "Sources/S3Client/Ui/ShortcutKey.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = NL.join([
        "int\tKShortcutKeyCentre::HandleKeyInput(unsigned int uKey, int nModifier)",
        "{",
        "\tint nIndex = FindCommand(MAKELONG(uKey, nModifier));",
        "\tif (nIndex >= 0)",
        "\t{",
        "\t\treturn ExcuteScript(ms_pCommands[nIndex].szDo);",
        "\t}",
    ])
    assert s.count(CU) == 1, "khong tim thay HandleKeyInput (%d)" % s.count(CU)
    MOI = NL.join([
        "int\tKShortcutKeyCentre::HandleKeyInput(unsigned int uKey, int nModifier)",
        "{",
        "\tint nIndex = FindCommand(MAKELONG(uKey, nModifier));",
        "\tif (nIndex >= 0)",
        "\t{",
        "#ifdef JX_ANDROID",
        "\t\t// [ANDROID 09/09 PHIMMA] Tren dien thoai khong co ban phim ma van co phim tat",
        "\t\t// tu kich (che do sua giao dien tu bat 09/09). Ghi lai ma phim + bo trong de",
        "\t\t// lan sau nhin la biet ngay phim nao.",
        "\t\tg_DebugLog(\"[PHIMTAT] ma phim=%u (0x%02X) bo tro=%d -> %.80s\",",
        "\t\t\tuKey, uKey, nModifier, ms_pCommands[nIndex].szDo);",
        "#endif",
        "\t\treturn ExcuteScript(ms_pCommands[nIndex].szDo);",
        "\t}",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
