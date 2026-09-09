# -*- coding: utf-8 -*-
#
# [ANDROID 09/09 NGUA] Core: hoi truoc khi danh - ky nang nay dung duoc luc dang cuoi ngua khong.
#
# Chu: "bam danh phim chinh hay phu se qua buoc kiem tra skill tren ngua hay duoi ngua -
# neu duoi ngua thi se tu xuong ngua".
#
# Luat nay DA CO SAN trong ban nay, o he tu danh: KPlayer.cpp:12610-12645.
#   ISkill::GetHorseLimit() == 1 -> ky nang CHI dung duoc khi DUOI ngua
#                                   -> dang cuoi thi SendClientCmdRide(TRUE) roi thoi lan nay
#   ISkill::GetHorseLimit() == 2 -> ky nang CHI dung duoc khi TREN ngua
#                                   -> dang di bo thi SendClientCmdRide(FALSE) roi thoi lan nay
#   khac 0 ma khong phai 1/2     -> khong dung duoc
# Co chan thoi gian TIME_RIDE (5 giay) giua hai lan len/xuong, y nhu he tu danh.
#
# Lam lai dung luat do cho duong mobile, tra ve mot luot ca ba thu client can:
#   - phan xu ngua,
#   - TAM DANH cua ky nang (ban tham khao dung getBackAttackRadius de biet keo bao xa
#     thi diem ngam nam o dau - KuiMyMenu.cpp:310 _Beilv = _BackAttackRadius / radius),
#   - co phai ky nang TRO (aura) khong: aura thi bat/tat chu khong phai danh.
#
# Ma GDI moi duoc NOI VAO CUOI bang (khong chen giua - chen giua se day moi ma so phia
# sau lech di, da ghi o muc 8.11 ban giao). Cuoi bang von da co nhieu muc them sau nay
# (GDI_HS_SP, GDI_FUSION_INFO...) nen day la cach lam san co cua du an.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
DAU = "[ANDROID 09/09 NGUA]"

# ---------------------------------------------------------------- 1. GameDataDef.h
P = "Sources/Core/Src/GameDataDef.h"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = NL.join([
        "\t// [ANDROID 09/09 DANH] chi so NPC - can de goi LockSomeoneUseSkill(nChiSoNpc, ...)",
        "\tint\t\t\t\tnChiSoNpc;",
        "#endif",
        "};",
    ])
    assert s.count(CU) == 1, "khong tim thay cuoi KUiTargetDetailInfo (%d)" % s.count(CU)
    MOI = NL.join([
        "\t// [ANDROID 09/09 DANH] chi so NPC - can de goi LockSomeoneUseSkill(nChiSoNpc, ...)",
        "\tint\t\t\t\tnChiSoNpc;",
        "#endif",
        "};",
        "",
        "//==================================",
        "//\t[ANDROID 09/09 NGUA] Hoi Core truoc khi bam nut ky nang tren dien thoai.",
        "//\tGDI_KYNANG_MOBILE: uParam = KJxKyNangHoi*, nParam = 0.",
        "//==================================",
        "struct KJxKyNangHoi",
        "{",
        "\tint\tnSkillId;\t// VAO : ma ky nang",
        "\tint\tnNgua;\t\t// RA  : 0 = danh duoc luon",
        "\t\t\t\t\t\t//       1 = ky nang chi dung duoc DUOI ngua, da gui lenh xuong -> lan nay thoi",
        "\t\t\t\t\t\t//       2 = ky nang chi dung duoc TREN ngua, da gui lenh len   -> lan nay thoi",
        "\t\t\t\t\t\t//       3 = khong dung duoc (con han len/xuong ngua, hoac gioi han khac)",
        "\tint\tnTamDanh;\t// RA  : tam danh cua ky nang (dung de biet diem ngam nam dau khi keo)",
        "\tint\tnLaAura;\t// RA  : 1 = ky nang TRO (bat/tat) chu khong phai danh",
        "};",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ---------------------------------------------------------------- 2. CoreShell.h (noi ma vao CUOI bang)
P = "Sources/Core/Src/CoreShell.h"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = "\tGDI_HS_SP,"
    assert s.count(CU) == 1, "khong tim thay GDI_HS_SP (%d)" % s.count(CU)
    MOI = NL.join([
        CU,
        "",
        "\t// [ANDROID 09/09 NGUA] NOI VAO CUOI BANG - khong chen giua (chen giua se day moi",
        "\t// ma so phia sau lech di). uParam = KJxKyNangHoi* -> tra 1 neu doc duoc.",
        "\t// Tra ve phan xu ngua + tam danh + co phai ky nang tro, cho nut ky nang tren dien thoai.",
        "\tGDI_KYNANG_MOBILE,",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ---------------------------------------------------------------- 3. CoreShell.cpp
P = "Sources/Core/Src/CoreShell.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = "\tcase NPC_OI_TARGET_INFO:"
    assert s.count(CU) == 1, "khong tim thay case NPC_OI_TARGET_INFO (%d)" % s.count(CU)
    MOI = NL.join([
        "\t// [ANDROID 09/09 NGUA] Nut ky nang tren dien thoai hoi truoc khi danh.",
        "\t// Luat ngua lay DUNG cua he tu danh (KPlayer.cpp:12610) de hai duong xu su giong nhau.",
        "\tcase GDI_KYNANG_MOBILE:",
        "\t{",
        "\t\tKJxKyNangHoi* pHoi = (KJxKyNangHoi*)uParam;",
        "\t\tif (pHoi == NULL || pHoi->nSkillId <= 0)",
        "\t\t\tbreak;",
        "\t\tpHoi->nNgua = 0;",
        "\t\tpHoi->nTamDanh = 0;",
        "\t\tpHoi->nLaAura = 0;",
        "",
        "\t\tint nToi = Player[CLIENT_PLAYER_INDEX].m_nIndex;",
        "\t\tif (nToi <= 0 || nToi >= MAX_NPC)",
        "\t\t\tbreak;",
        "\t\tKSkill* pKN = (KSkill*)g_SkillManager.GetSkill(pHoi->nSkillId, 1);",
        "\t\tif (pKN == NULL)",
        "\t\t\tbreak;",
        "",
        "\t\tpHoi->nTamDanh = pKN->GetAttackRadius();",
        "\t\tpHoi->nLaAura  = pKN->IsAura() ? 1 : 0;",
        "",
        "\t\tint nHan = pKN->GetHorseLimit();",
        "\t\tif (nHan)",
        "\t\t{",
        "\t\t\tDWORD dwTuLuc = GetTickCount() - (DWORD)Npc[nToi].m_TimeHorse;",
        "\t\t\tif (nHan == 1 && Npc[nToi].m_bRideHorse)",
        "\t\t\t{\t// ky nang chi dung duoc DUOI ngua",
        "\t\t\t\tif (dwTuLuc >= TIME_RIDE)",
        "\t\t\t\t{",
        "\t\t\t\t\tSendClientCmdRide(TRUE);\t// xuong ngua",
        "\t\t\t\t\tpHoi->nNgua = 1;",
        "\t\t\t\t}",
        "\t\t\t\telse",
        "\t\t\t\t\tpHoi->nNgua = 3;\t// con han 5 giay, lan nay chiu",
        "\t\t\t}",
        "\t\t\telse if (nHan == 2 && !Npc[nToi].m_bRideHorse)",
        "\t\t\t{\t// ky nang chi dung duoc TREN ngua",
        "\t\t\t\tif (dwTuLuc >= TIME_RIDE)",
        "\t\t\t\t{",
        "\t\t\t\t\tSendClientCmdRide(FALSE);\t// len ngua",
        "\t\t\t\t\tpHoi->nNgua = 2;",
        "\t\t\t\t}",
        "\t\t\t\telse",
        "\t\t\t\t\tpHoi->nNgua = 3;",
        "\t\t\t}",
        "\t\t\telse if (nHan != 1 && nHan != 2)",
        "\t\t\t\tpHoi->nNgua = 3;",
        "\t\t}",
        "\t\tnRet = 1;",
        "\t\tbreak;",
        "\t}",
        "",
        CU,
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
