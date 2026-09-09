# -*- coding: utf-8 -*-
#
# [ANDROID 09/09 DANH] Core: tim DICH de danh, co the ngam theo huong.
#
# Chu: "Thieu phan cac nut chon ky nang danh ban mobile - cham la danh, dieu chinh
# huong danh".
#
# Ban PC chon dich bang cach tro chuot vao no. Tren dien thoai ngon tay che mat con
# quai, va cham trung mot con dang chay la rat kho. Ban JX1 Mobile tham khao
# (D:\USVOLAM\Jx1mClientMobile, lop KSkillRocker) dung CAN: dat ngon len nut ky nang
# roi keo ve huong muon danh.
#
# Phia Core can mot viec: tu vi tri nhan vat, tra ve con DICH hop nhat - hoac gan nhat
# (khong keo), hoac gan nhat TRONG HUONG dang keo.
#
# Dung lai ma so NPC_OI_TARGET_INFO voi nParam == 2, KHONG them ma GDI moi: them vao
# giua enum se day moi ma so phia sau lech di (da ghi o muc 8.11 ban giao).
#
#   Vao : pInfo->nViTriVeX / nViTriVeY = vec to huong ngam (0,0 = khong ngam)
#   Ra  : sTargetName, nViTriVeX / nViTriVeY = vi tri VE cua dich,
#         nChiSoNpc = chi so NPC (de goi LockSomeoneUseSkill), nDangKhoa = 1
#         Tra ve 1 neu tim duoc, 0 neu khong.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
DAU = "[ANDROID 09/09 DANH]"

# ---------------------------------------------------------------- 1. GameDataDef.h
P = "Sources/Core/Src/GameDataDef.h"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = "\tint\t\t\t\tnDangKhoa;\t// 1 = muc tieu dang KHOA (m_nPeopleIdx); 0 = chi dang tro chuot vao"
    assert s.count(CU) == 1, "khong tim thay nDangKhoa (thay %d)" % s.count(CU)
    MOI = NL.join([
        CU,
        "\t// [ANDROID 09/09 DANH] chi so NPC - can de goi LockSomeoneUseSkill(nChiSoNpc, ...)",
        "\tint\t\t\t\tnChiSoNpc;",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ---------------------------------------------------------------- 2. CoreShell.cpp
P = "Sources/Core/Src/CoreShell.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = NL.join([
        "\t\t\tif (nChon)",
        "\t\t\t{",
        "\t\t\t\tstrcpy_s(pGan->sTargetName, sizeof(pGan->sTargetName), Npc[nChon].Name);",
        "\t\t\t\tNpc[nChon].GetDrawPos(&pGan->nViTriVeX, &pGan->nViTriVeY);",
        "\t\t\t\tpGan->nDangKhoa = 0;",
        "\t\t\t\tnRet = 1;",
        "\t\t\t}",
        "\t\t\tbreak;",
        "\t\t}",
    ])
    assert s.count(CU) == 1, "khong tim thay duoi nhanh nParam==1 (thay %d)" % s.count(CU)

    MOI = NL.join([
        CU,
        "",
        "\t\t// [ANDROID 09/09 DANH] nParam == 2: tra ve con DICH hop nhat de danh.",
        "\t\t// Vao : pInfo->nViTriVeX / nViTriVeY = vec to huong ngam (0,0 = khong ngam)",
        "\t\t// Ra  : ten, vi tri VE, nChiSoNpc = chi so NPC, nDangKhoa = 1",
        "\t\t// Khong ngam thi lay con GAN NHAT; co ngam thi lay con gan nhat NAM TRONG",
        "\t\t// nón +-60 do quanh huong ngam. Dung lai ma so nay, khong them ma GDI moi.",
        "\t\tif (nParam == 2)",
        "\t\t{",
        "\t\t\tKUiTargetDetailInfo* pDich = (KUiTargetDetailInfo*)uParam;",
        "\t\t\tint nMe = Player[CLIENT_PLAYER_INDEX].m_nIndex;",
        "\t\t\tif (pDich == NULL || nMe <= 0 || nMe >= MAX_NPC)",
        "\t\t\t\tbreak;",
        "",
        "\t\t\tconst __int64 XA_NHAT = 700 * 700;\t// ngoai khung nhin thi khong tinh",
        "\t\t\tint nX0 = 0, nY0 = 0;",
        "\t\t\tNpc[nMe].GetDrawPos(&nX0, &nY0);",
        "\t\t\t__int64 ax = (__int64)pDich->nViTriVeX;",
        "\t\t\t__int64 ay = (__int64)pDich->nViTriVeY;",
        "\t\t\t__int64 nDaiNgam = ax * ax + ay * ay;\t// 0 = khong ngam",
        "",
        "\t\t\tint nChonDich = 0;",
        "\t\t\t__int64 nGanNhat = 0;",
        "\t\t\tint nDuyetD = 0;",
        "\t\t\twhile (nDuyetD = NpcSet.GetNextIdx(nDuyetD))",
        "\t\t\t{",
        "\t\t\t\tif (nDuyetD == nMe || !Npc[nDuyetD].m_dwID || Npc[nDuyetD].m_RegionIndex < 0)",
        "\t\t\t\t\tcontinue;",
        "\t\t\t\tif (Npc[nDuyetD].m_Doing == do_death || Npc[nDuyetD].m_Doing == do_revive",
        "\t\t\t\t\t|| Npc[nDuyetD].m_CurrentLife <= 0)",
        "\t\t\t\t\tcontinue;",
        "\t\t\t\tif (NpcSet.GetRelation(nMe, nDuyetD) != relation_enemy)",
        "\t\t\t\t\tcontinue;",
        "",
        "\t\t\t\tint x = 0, y = 0;",
        "\t\t\t\tNpc[nDuyetD].GetDrawPos(&x, &y);",
        "\t\t\t\t__int64 dx = (__int64)(x - nX0), dy = (__int64)(y - nY0);",
        "\t\t\t\t__int64 d = dx * dx + dy * dy;",
        "\t\t\t\tif (d > XA_NHAT)",
        "\t\t\t\t\tcontinue;",
        "",
        "\t\t\t\tif (nDaiNgam > 0 && d > 0)",
        "\t\t\t\t{",
        "\t\t\t\t\t// giu con nam trong nón +-60 do: cos >= 1/2  <=>  4*tich^2 >= dai*dai",
        "\t\t\t\t\t__int64 tich = ax * dx + ay * dy;",
        "\t\t\t\t\tif (tich <= 0 || 4 * tich * tich < nDaiNgam * d)",
        "\t\t\t\t\t\tcontinue;",
        "\t\t\t\t}",
        "",
        "\t\t\t\tif (!nChonDich || d < nGanNhat)",
        "\t\t\t\t{",
        "\t\t\t\t\tnChonDich = nDuyetD;",
        "\t\t\t\t\tnGanNhat = d;",
        "\t\t\t\t}",
        "\t\t\t}",
        "",
        "\t\t\tpDich->nChiSoNpc = nChonDich;",
        "\t\t\tif (nChonDich)",
        "\t\t\t{",
        "\t\t\t\tstrcpy_s(pDich->sTargetName, sizeof(pDich->sTargetName), Npc[nChonDich].Name);",
        "\t\t\t\tNpc[nChonDich].GetDrawPos(&pDich->nViTriVeX, &pDich->nViTriVeY);",
        "\t\t\t\tpDich->nDangKhoa = 1;",
        "\t\t\t\tnRet = 1;",
        "\t\t\t}",
        "\t\t\tbreak;",
        "\t\t}",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
