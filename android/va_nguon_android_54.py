# -*- coding: utf-8 -*-
#
# [ANDROID 09/09 AURA] Ky nang TRO (noi cong / trang thai) bam la BAT duoc.
#
# Ban truoc toi ghi "chua noi duoc tu day" va de lai o muc con lai. Doc ky hon thi thay
# duong da co san:
#     KPlayer::SetRightSkill (KPlayer.cpp:4436) - neu ky nang la AURA thi goi
#     Npc[].SetAuraSkill(nSkillID), khong phai aura thi SetAuraSkill(0).
# Va GOI_SET_IMMDIA_SKILL voi nParam == 1 chinh la goi SetRightSkill (CoreShell.cpp:18200).
#
# Tuc chi can dat ky nang trO lam ky nang danh PHAI la no bat. Ban truoc toi dat nham vao
# o danh TRAI (nParam 0 -> SetLeftSkill) nen khong bat gi ca.
#
# Doi mot tham so. Ban tham khao cung xu ly aura rieng o cho nay
# (KgameWorld.cpp:4354 SetAuraSkill), nen huong lam la giong nhau.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
DAU = "[ANDROID 09/09 AURA]"

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = NL.join([
        "\tif (nAura)",
        "\t{",
        "\t\t// Ky nang TRO (noi cong / trang thai): khong phai danh. Chi dat lam ky nang",
        "\t\t// dang dung; bat/tat aura chua noi duoc tu day - xem muc \"con lai\" ban giao.",
        "\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&o, 0);",
        "\t\treturn;",
        "\t}",
    ])
    assert s.count(CU) == 1, "khong tim thay nhanh aura (%d)" % s.count(CU)
    MOI = NL.join([
        "\tif (nAura)",
        "\t{",
        "\t\t// [ANDROID 09/09 AURA] Ky nang TRO (noi cong / trang thai) khong phai danh, ma",
        "\t\t// la BAT len. Duong co san: dat lam ky nang danh PHAI (nParam = 1) ->",
        "\t\t// KPlayer::SetRightSkill (KPlayer.cpp:4444) tu goi SetAuraSkill cho ky nang aura.",
        "\t\t// (Ban truoc dat nham vao o danh TRAI nen khong bat gi ca.)",
        "\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&o, 1);",
        "\t\treturn;",
        "\t}",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
