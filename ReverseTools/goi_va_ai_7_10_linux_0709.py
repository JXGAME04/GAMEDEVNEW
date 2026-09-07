#!/usr/bin/env python3
# -*- coding: ascii -*-
"""
goi_va_ai_7_10_linux_0709.py

[AI710L 07/09] AI quai 7/8/9/10: thay bon ham ProcessAIType7..10 cua JX1 (thuat toan KHAC
ban Linux) bang bon ham dich tu nhi phan D:/ServerLinux/server1/jx_linux_y:

    AI7  0x08094040  Cong thanh Xung xa : tim NPC co m_dwID == p0 (vung hien tai + 8 vung ke) roi pha
    AI8  0x0808F1C0  Dau Thach Xa       : ban skill vao diem ngau nhien trong o vuong canh p2 quanh (p0,p1)
    AI9  0x08092E30  HANH QUAN          : p0% tim dich ngau nhien; p6=1 bam thu linh; p6=0 di toi (p7,p8)
    AI10 0x08091EB0  DUNG YEN BAN       : chi ra chieu khi dich trong m_CurrentAttackRadius, KHONG di

Xem PHANTICH_CAIBANG_DAN_VA_QUAI_DOT2_0609.md Phan F.3. Bon ham cu ProcessAIType7..10 GIU NGUYEN
(khong con duoc goi, giong ProcessAIType1..6). Toan bo ma moi nam trong #ifdef _SERVER nen
CoreClient.dll khong doi.

Doc/ghi latin-1, giu nguyen EOL va byte TCVN3/GBK. Chay lai duoc (idempotent).
"""
import io
import os
import shutil

SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"
NHAN = "[AI710L 07/09]"


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def ghi(p, d):
    io.open(p, "w", encoding="latin-1", newline="").write(d)


def kiem_cao(d0, d, ten):
    hb0 = sum(1 for c in d0 if ord(c) >= 0x80)
    hb1 = sum(1 for c in d if ord(c) >= 0x80)
    assert hb0 == hb1, "%s: so byte cao doi %d -> %d" % (ten, hb0, hb1)
    assert "\xef\xbf\xbd" not in d, "%s: xuat hien FFFD" % ten


def saoluu(p):
    b = p + ".truoc_ai710l_0709"
    if not os.path.exists(b):
        shutil.copyfile(p, b)
        print("  sao luu -> " + os.path.basename(b))


def thay(d, cu, moi, ten):
    n = d.count(cu)
    assert n == 1, "%s: neo khong duy nhat (%d): %r" % (ten, n, cu[:60])
    return d.replace(cu, moi)


# ---------------------------------------------------------------- KNpcAI.h
def va_h():
    p = os.path.join(SRC, "KNpcAI.h")
    d0 = doc(p)
    if NHAN in d0:
        print("KNpcAI.h: da va roi, bo qua")
        return False
    nl = "\r\n" if "\r\n" in d0 else "\n"
    neo = "ProcessAIType06();"
    assert d0.count(neo) == 1, "KNpcAI.h: neo ProcessAIType06 khong duy nhat"
    i = d0.index(neo)
    j = d0.index(nl, i) + len(nl)          # sau dong khai bao ProcessAIType06
    khoi = (
        "#ifdef _SERVER" + nl +
        "\t// " + NHAN + " AI 7/8/9/10 dich tu nhi phan Linux jx_linux_y - xem PHANTICH_CAIBANG_DAN_VA_QUAI_DOT2_0609.md Phan F.3." + nl +
        "\t// Bon ham ProcessAIType7..10 o tren la thuat toan KHAC ban Linux nen KHONG con duoc Activate goi." + nl +
        "\tvoid\t\t\tAI07_XungXa();\t\t\t// Linux 0x08094040: xung xa tim NPC co m_dwID == p0 roi pha" + nl +
        "\tvoid\t\t\tAI08_ThachXa();\t\t\t// Linux 0x0808F1C0: dau thach xa ban vao o vuong canh p2 quanh (p0,p1)" + nl +
        "\tvoid\t\t\tAI09_HanhQuan();\t\t// Linux 0x08092E30: hanh quan toi (p7,p8) / bam thu linh p7 / p0% danh dich" + nl +
        "\tvoid\t\t\tAI10_DungBan();\t\t\t// Linux 0x08091EB0: dung yen, chi ra chieu khi dich trong tam" + nl +
        "\tvoid\t\t\tAI_BoMucTieuLinux();\t// nhip AI Linux 0x0808C640: bo muc tieu chet/hoi sinh/nguoi choi chua bat chien dau" + nl +
        "\tint\t\t\t\tAI_TimDichNgauNhien();\t// Linux 0x0808DBA0: gom toi da 10 dich trong tam nhin, boc 1" + nl +
        "\tint\t\t\t\tAI_TimNpcTheoIdGan(DWORD dwID);\t// Linux 0x0807A1F0: NPC co m_dwID trong vung hien tai + 8 vung ke" + nl +
        "\tvoid\t\t\tAI_BanTaiCho(int nEnemy);\t// Linux 0x0808F360: ra chieu tai cho, khong di" + nl +
        "#endif" + nl
    )
    d = d0[:j] + khoi + d0[j:]
    kiem_cao(d0, d, "KNpcAI.h")
    saoluu(p)
    ghi(p, d)
    print("KNpcAI.h: them 8 khai bao trong #ifdef _SERVER")
    return True


# -------------------------------------------------------------- KNpcAI.cpp
def than_ham(nl):
    L = [
        "#ifdef _SERVER",
        "//------------------------------------------------------------------------------",
        "// " + NHAN + " AI 7/8/9/10 dich tu nhi phan Linux jx_linux_y (bang nhay 0x08255340).",
        "// Xem D:/GAMEDEVNEW/PHANTICH_CAIBANG_DAN_VA_QUAI_DOT2_0609.md Phan F.3.",
        "// Bon ham ProcessAIType7..10 co san cua JX1 (ben duoi) la thuat toan KHAC, khong phai",
        "// ban dich, nen tu 07/09 Activate goi bon ham nay thay the (bon ham cu giu nguyen, khong goi).",
        "// Ben Linux moi ProcessAIType goi 'nhip AI' 0x0808C640 o dong dau: dat m_NextAITime",
        "// (JX1 lam san trong Activate) + bo muc tieu da chet/hoi sinh + bo NGUOI CHOI chua bat",
        "// che do chien dau. AI_BoMucTieuLinux() lam phan sau.",
        "//------------------------------------------------------------------------------",
        "void KNpcAI::AI_BoMucTieuLinux()",
        "{",
        "\tint nTarget = Npc[m_nIndex].m_nPeopleIdx;",
        "\tif (nTarget <= 0 || nTarget >= MAX_NPC)",
        "\t{",
        "\t\tNpc[m_nIndex].m_nPeopleIdx = 0;",
        "\t\treturn;",
        "\t}",
        "\tif (Npc[nTarget].m_Doing == do_death || Npc[nTarget].m_Doing == do_revive)",
        "\t\tNpc[m_nIndex].m_nPeopleIdx = 0;",
        "\telse if (Npc[nTarget].m_Kind == kind_player && Npc[nTarget].m_FightMode == 0)",
        "\t\tNpc[m_nIndex].m_nPeopleIdx = 0;",
        "}",
        "",
        "// Linux 0x0808DBA0: quet o trong tam nhin (nhu GetNpcNumber), gom toi da 10 dich",
        "// (bo NPC dang an) roi boc NGAU NHIEN mot con - khong phai con gan nhat.",
        "int KNpcAI::AI_TimDichNgauNhien()",
        "{",
        "\tint nList[10];",
        "\tint nCount = 0;",
        "\tint nSubWorld = Npc[m_nIndex].m_SubWorldIndex;",
        "\tint nRegion = Npc[m_nIndex].m_RegionIndex;",
        "\tint nMapX = Npc[m_nIndex].m_MapX;",
        "\tint nMapY = Npc[m_nIndex].m_MapY;",
        "\tif (nSubWorld < 0 || nRegion < 0)",
        "\t\treturn 0;",
        "\tint nRangeX = Npc[m_nIndex].m_VisionRadius / SubWorld[nSubWorld].m_nCellWidth;",
        "\tint nRangeY = Npc[m_nIndex].m_VisionRadius / SubWorld[nSubWorld].m_nCellHeight;",
        "\tconst int nR2 = nRangeX * nRangeX;",
        "\tfor (int i = -nRangeX; i < nRangeX && nCount < 10; i++)",
        "\t{",
        "\t\tfor (int j = -nRangeY; j < nRangeY && nCount < 10; j++)",
        "\t\t{",
        "\t\t\tif ((i * i + j * j) > nR2)",
        "\t\t\t\tcontinue;",
        "\t\t\tint nRMx = nMapX + i;",
        "\t\t\tint nRMy = nMapY + j;",
        "\t\t\tint nSearchRegion = nRegion;",
        "\t\t\tif (nRMx < 0)",
        "\t\t\t{",
        "\t\t\t\tnSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];",
        "\t\t\t\tnRMx += SubWorld[nSubWorld].m_nRegionWidth;",
        "\t\t\t}",
        "\t\t\telse if (nRMx >= SubWorld[nSubWorld].m_nRegionWidth)",
        "\t\t\t{",
        "\t\t\t\tnSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];",
        "\t\t\t\tnRMx -= SubWorld[nSubWorld].m_nRegionWidth;",
        "\t\t\t}",
        "\t\t\tif (nSearchRegion == -1)",
        "\t\t\t\tcontinue;",
        "\t\t\tif (nRMy < 0)",
        "\t\t\t{",
        "\t\t\t\tnSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];",
        "\t\t\t\tnRMy += SubWorld[nSubWorld].m_nRegionHeight;",
        "\t\t\t}",
        "\t\t\telse if (nRMy >= SubWorld[nSubWorld].m_nRegionHeight)",
        "\t\t\t{",
        "\t\t\t\tnSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];",
        "\t\t\t\tnRMy -= SubWorld[nSubWorld].m_nRegionHeight;",
        "\t\t\t}",
        "\t\t\tif (nSearchRegion == -1)",
        "\t\t\t\tcontinue;",
        "\t\t\tint nNpcIdx = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, relation_enemy);",
        "\t\t\tif (nNpcIdx > 0 && nNpcIdx < MAX_NPC && Npc[nNpcIdx].m_HideState.nTime == 0)",
        "\t\t\t\tnList[nCount++] = nNpcIdx;",
        "\t\t}",
        "\t}",
        "\tif (nCount <= 0)",
        "\t\treturn 0;",
        "\tint nPick = g_Random(nCount);",
        "\tif (nPick < 0 || nPick >= nCount)",
        "\t\tnPick = 0;",
        "\treturn nList[nPick];",
        "}",
        "",
        "// Linux 0x0807A1F0 -> 0x080E1FD0: NPC co m_dwID == dwID trong vung hien tai va 8 vung ke, bo NPC dang an.",
        "int KNpcAI::AI_TimNpcTheoIdGan(DWORD dwID)",
        "{",
        "\tif (dwID == 0)",
        "\t\treturn 0;",
        "\tint nSubWorld = Npc[m_nIndex].m_SubWorldIndex;",
        "\tint nMyRegion = Npc[m_nIndex].m_RegionIndex;",
        "\tif (nSubWorld < 0 || nMyRegion < 0)",
        "\t\treturn 0;",
        "\tint nIdx = NpcSet.SearchID(dwID);",
        "\tif (nIdx <= 0 || nIdx >= MAX_NPC)",
        "\t\treturn 0;",
        "\tif (Npc[nIdx].m_SubWorldIndex != nSubWorld || Npc[nIdx].m_RegionIndex < 0 || Npc[nIdx].m_HideState.nTime > 0)",
        "\t\treturn 0;",
        "\tif (Npc[nIdx].m_RegionIndex == nMyRegion)",
        "\t\treturn nIdx;",
        "\tfor (int i = 0; i < 8; i++)",
        "\t{",
        "\t\tif (SubWorld[nSubWorld].m_Region[nMyRegion].m_nConnectRegion[i] == Npc[nIdx].m_RegionIndex)",
        "\t\t\treturn nIdx;",
        "\t}",
        "\treturn 0;",
        "}",
        "",
        "// Linux 0x0808F360: ra chieu TAI CHO - chi khi dich con song, trong m_CurrentAttackRadius va trong tam nhin.",
        "// KHONG bao gio di (khac FollowAttack: ngoai tam thi do_walk toi dich).",
        "void KNpcAI::AI_BanTaiCho(int nEnemy)",
        "{",
        "\tif (nEnemy <= 0 || nEnemy >= MAX_NPC)",
        "\t\treturn;",
        "\tif (Npc[nEnemy].m_RegionIndex < 0)",
        "\t\treturn;",
        "\tif (Npc[nEnemy].m_Doing == do_death || Npc[nEnemy].m_Doing == do_revive)",
        "\t\treturn;",
        "\tint nD2 = KNpcSet::GetDistanceSquare(m_nIndex, nEnemy);",
        "\tint nAtk = Npc[m_nIndex].m_CurrentAttackRadius;",
        "\tif (nD2 >= nAtk * nAtk)",
        "\t\treturn;",
        "\tif (!InEyeshot(nEnemy))",
        "\t\treturn;",
        "\tNpc[m_nIndex].SendCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, nEnemy);",
        "}",
        "",
        "// AI 7 - Linux 0x08094040 'Cong thanh Xung xa': p0 = m_dwID cua NPC muc tieu (script dat),",
        "// p1..p4 = ty le skill 1..4. p0 = 0 (mac dinh npcs.txt) -> khong lam gi.",
        "void KNpcAI::AI07_XungXa()",
        "{",
        "\tAI_BoMucTieuLinux();",
        "\tint* pAIParam = Npc[m_nIndex].m_AiParam;",
        "\tif (pAIParam[0] == 0)",
        "\t\treturn;",
        "\tint nTarget = AI_TimNpcTheoIdGan((DWORD)pAIParam[0]);",
        "\tif (nTarget <= 0)",
        "\t\treturn;",
        "\tint nRand = g_Random(100);",
        "\tif (nRand < pAIParam[1])",
        "\t\tNpc[m_nIndex].SetActiveSkill(1);",
        "\telse if (nRand < pAIParam[1] + pAIParam[2])",
        "\t\tNpc[m_nIndex].SetActiveSkill(2);",
        "\telse if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3])",
        "\t\tNpc[m_nIndex].SetActiveSkill(3);",
        "\telse if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3] + pAIParam[4])",
        "\t\tNpc[m_nIndex].SetActiveSkill(4);",
        "\tFollowAttack(nTarget);",
        "}",
        "",
        "// AI 8 - Linux 0x0808F1C0 'Dau Thach Xa': ban skill (p3..p6 -> skill 1..4) vao diem ngau nhien",
        "// trong o vuong canh p2 quanh (p0, p1). p0 hoac p1 = 0 -> khong lam gi. Khong bao gio di.",
        "void KNpcAI::AI08_ThachXa()",
        "{",
        "\tAI_BoMucTieuLinux();",
        "\tint* pAIParam = Npc[m_nIndex].m_AiParam;",
        "\tif (pAIParam[0] == 0 || pAIParam[1] == 0)",
        "\t\treturn;",
        "\tint nSide = pAIParam[2];",
        "\tint nRandX = (nSide > 0) ? g_Random(nSide) : 0;",
        "\tint nRandY = (nSide > 0) ? g_Random(nSide) : 0;",
        "\tint nRand = g_Random(100);",
        "\tif (nRand < pAIParam[3])",
        "\t\tNpc[m_nIndex].SetActiveSkill(1);",
        "\telse if (nRand < pAIParam[3] + pAIParam[4])",
        "\t\tNpc[m_nIndex].SetActiveSkill(2);",
        "\telse if (nRand < pAIParam[3] + pAIParam[4] + pAIParam[5])",
        "\t\tNpc[m_nIndex].SetActiveSkill(3);",
        "\telse if (nRand < pAIParam[3] + pAIParam[4] + pAIParam[5] + pAIParam[6])",
        "\t\tNpc[m_nIndex].SetActiveSkill(4);",
        "\tint nX = pAIParam[0] - nSide / 2 + nRandX;",
        "\tint nY = pAIParam[1] - nSide / 2 + nRandY;",
        "\tNpc[m_nIndex].SendCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, nX, nY);",
        "}",
        "",
        "// AI 9 - Linux 0x08092E30 'HANH QUAN' (newcitydefence: SetNpcAI(idx,9,20,-1,-1,-1,-1,-1,0,x,y)):",
        "//   p0 = % moi nhip tim dich ngau nhien trong tam nhin; p2/p3/p4 = ty le skill 1/2/3 (con lai skill 4);",
        "//   p6 = 1: bam thu linh Npc[p7] (m_dwID = p8); p6 = 0: di toi diem (p7,p8), dung cach dich",
        "//   min(AttackRadius, VisionRadius)/2. Goc m_OriginX/Y DOI THEO vi tri hien tai moi lan di nen",
        "//   day xich khong can. JX1 them MOT cua: p7 = p8 = 0 hoac p6 ngoai {0,1} (50 mau trong npcs.txt:",
        "//   quan quan Tong Kim, Moc nhan, Tru ai... mang 0|0 hay 20|50 tu thoi AI9 cu) thi KHONG hanh quan",
        "//   toi goc ban do - chi CommonAction.",
        "void KNpcAI::AI09_HanhQuan()",
        "{",
        "\tAI_BoMucTieuLinux();",
        "\tint* pAIParam = Npc[m_nIndex].m_AiParam;",
        "\tif (KeepActiveRange())",
        "\t{",
        "\t\tNpc[m_nIndex].m_nPeopleIdx = 0;",
        "\t\treturn;",
        "\t}",
        "\tint nRand = g_Random(100);",
        "\tint nSkillNo = 4;",
        "\tif (nRand < pAIParam[2])",
        "\t\tnSkillNo = 1;",
        "\telse if (nRand < pAIParam[2] + pAIParam[3])",
        "\t\tnSkillNo = 2;",
        "\telse if (nRand < pAIParam[2] + pAIParam[3] + pAIParam[4])",
        "\t\tnSkillNo = 3;",
        "\tif (!Npc[m_nIndex].SetActiveSkill(nSkillNo))",
        "\t{",
        "\t\tCommonAction();",
        "\t\treturn;",
        "\t}",
        "\tint nEnemy = Npc[m_nIndex].m_nPeopleIdx;",
        "\tif (nEnemy > 0 && Npc[m_nIndex].m_CurrentVisionRadius != 0)",
        "\t{",
        "\t\t// Linux: FollowAttack tra 0 (muc tieu mat vung / chet) -> bo muc tieu",
        "\t\tif (CheckNpc(nEnemy))",
        "\t\t{",
        "\t\t\tNpc[m_nIndex].m_nPeopleIdx = 0;",
        "\t\t\treturn;",
        "\t\t}",
        "\t\tFollowAttack(nEnemy);",
        "\t\treturn;",
        "\t}",
        "\tif (pAIParam[0] > g_Random(100))",
        "\t{",
        "\t\tnEnemy = AI_TimDichNgauNhien();",
        "\t\tNpc[m_nIndex].m_nPeopleIdx = nEnemy;",
        "\t\tif (nEnemy > 0)",
        "\t\t{",
        "\t\t\tNpc[m_nIndex].GetMpsPos(&Npc[m_nIndex].m_OriginX, &Npc[m_nIndex].m_OriginY);",
        "\t\t\tFollowAttack(nEnemy);",
        "\t\t\treturn;",
        "\t\t}",
        "\t}",
        "\tif (pAIParam[6] == 1)",
        "\t{",
        "\t\tint nLeader = pAIParam[7];",
        "\t\tif (nLeader > 0 && nLeader < MAX_NPC &&",
        "\t\t\tNpc[nLeader].m_dwID == (DWORD)pAIParam[8] &&",
        "\t\t\tNpc[nLeader].m_Doing != do_death &&",
        "\t\t\tNpc[nLeader].m_SubWorldIndex == Npc[m_nIndex].m_SubWorldIndex)",
        "\t\t{",
        "\t\t\tNpc[nLeader].GetMpsPos(&Npc[m_nIndex].m_OriginX, &Npc[m_nIndex].m_OriginY);",
        "\t\t\t// Linux: FollowAttack(thu linh) roi ROI TIEP vao CommonAction (khong return)",
        "\t\t\t// -> lenh cuoi cung la di ve goc = vi tri thu linh.",
        "\t\t\tFollowAttack(nLeader);",
        "\t\t}",
        "\t\tCommonAction();",
        "\t\treturn;",
        "\t}",
        "\tif (pAIParam[6] != 0 || (pAIParam[7] == 0 && pAIParam[8] == 0))",
        "\t{",
        "\t\tCommonAction();",
        "\t\treturn;",
        "\t}",
        "\tint nMyX, nMyY;",
        "\tNpc[m_nIndex].GetMpsPos(&nMyX, &nMyY);",
        "\tint nDesX = pAIParam[7];",
        "\tint nDesY = pAIParam[8];",
        "\tint nWantX = nDesX;",
        "\tint nWantY = nDesY;",
        "\tif (nDesX != nMyX || nDesY != nMyY)",
        "\t{",
        "\t\tint nR = Npc[m_nIndex].m_CurrentAttackRadius;",
        "\t\tif (Npc[m_nIndex].m_CurrentVisionRadius < nR)",
        "\t\t\tnR = Npc[m_nIndex].m_CurrentVisionRadius;",
        "\t\tnR = nR / 2;",
        "\t\tint nDir = g_GetDirIndex(nMyX, nMyY, nDesX, nDesY);",
        "\t\tnWantX = nDesX - ((nR * g_DirCos(nDir, 64)) >> 10);",
        "\t\tnWantY = nDesY - ((nR * g_DirSin(nDir, 64)) >> 10);",
        "\t}",
        "\tNpc[m_nIndex].GetMpsPos(&Npc[m_nIndex].m_OriginX, &Npc[m_nIndex].m_OriginY);",
        "\tNpc[m_nIndex].SendCommand(do_walk, nWantX, nWantY);",
        "}",
        "",
        "// AI 10 - Linux 0x08091EB0 'DUNG YEN BAN' (Tuyet Sat, Tri Thu To, cung binh, co quan...):",
        "//   khoa/tim dich gan nhat (kiem an + tam nhin), p1..p4 = ty le skill 1..4 (cong don, con lai -> thoi),",
        "//   chi ra chieu khi dich trong m_CurrentAttackRadius. KHONG bao gio do_walk.",
        "void KNpcAI::AI10_DungBan()",
        "{",
        "\tAI_BoMucTieuLinux();",
        "\tint* pAIParam = Npc[m_nIndex].m_AiParam;",
        "\tint nEnemy = Npc[m_nIndex].m_nPeopleIdx;",
        "\tBOOL bValid = FALSE;",
        "\tif (nEnemy > 0 && nEnemy < MAX_NPC && Npc[nEnemy].m_dwID != 0)",
        "\t{",
        "\t\tint nD2 = KNpcSet::GetDistanceSquare(m_nIndex, nEnemy);",
        "\t\tint nVision = Npc[m_nIndex].m_VisionRadius;",
        "\t\tif (nD2 > 0 && nD2 < nVision * nVision && Npc[nEnemy].m_HideState.nTime == 0)",
        "\t\t\tbValid = TRUE;",
        "\t}",
        "\tif (!bValid)",
        "\t{",
        "\t\tnEnemy = GetNearestNpc(relation_enemy);",
        "\t\tNpc[m_nIndex].m_nPeopleIdx = nEnemy;",
        "\t}",
        "\tint nRand = g_Random(100);",
        "\tint nSkillNo = 0;",
        "\tif (nRand < pAIParam[1])",
        "\t\tnSkillNo = 1;",
        "\telse if (nRand < pAIParam[1] + pAIParam[2])",
        "\t\tnSkillNo = 2;",
        "\telse if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3])",
        "\t\tnSkillNo = 3;",
        "\telse if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3] + pAIParam[4])",
        "\t\tnSkillNo = 4;",
        "\tif (nSkillNo == 0)",
        "\t\treturn;",
        "\tif (!Npc[m_nIndex].SetActiveSkill(nSkillNo))",
        "\t\treturn;",
        "\tAI_BanTaiCho(nEnemy);",
        "}",
        "#endif",
        "",
        "",
    ]
    return nl.join(L)


def va_cpp():
    p = os.path.join(SRC, "KNpcAI.cpp")
    d0 = doc(p)
    if NHAN in d0:
        print("KNpcAI.cpp: da va roi, bo qua")
        return False
    nl = "\r\n" if "\r\n" in d0 else "\n"
    d = d0

    # 1) doi duong goi trong switch cua Activate
    for cu, moi in (("\t\t\tProcessAIType7();", "\t\t\tAI07_XungXa();"),
                    ("\t\t\tProcessAIType8();", "\t\t\tAI08_ThachXa();"),
                    ("\t\t\tProcessAIType9();", "\t\t\tAI09_HanhQuan();"),
                    ("\t\t\tProcessAIType10();", "\t\t\tAI10_DungBan();")):
        d = thay(d, cu, moi, "KNpcAI.cpp switch")

    # 2) chu thich cua ban va [AI710 06/09]
    cu = ("\t\t// JX1 chu thich 4 case nay nen 112 con quai khai AIMode 9/10 khong co AI." + nl +
          "\t\t// Bon ham duoi day da co san trong tep nay, chi thieu duong goi." + nl)
    moi = ("\t\t// JX1 chu thich 4 case nay nen 112 con quai khai AIMode 9/10 khong co AI." + nl +
           "\t\t// " + NHAN + " ProcessAIType7..10 cua JX1 la thuat toan KHAC ban Linux (9 'cang danh cang hang'," + nl +
           "\t\t// 10 'bo chay' deu FollowAttack -> 62 bay/tru mode 10 se di theo nguoi choi). Goi bon ham" + nl +
           "\t\t// AI07_XungXa / AI08_ThachXa / AI09_HanhQuan / AI10_DungBan dich tu nhi phan Linux." + nl)
    d = thay(d, cu, moi, "KNpcAI.cpp chu thich AI710")

    # 3) chen than ham truoc khoi chu thich cua ProcessAIType1()
    neo = "void KNpcAI::ProcessAIType1()"
    assert d.count(neo) == 1, "KNpcAI.cpp: neo ProcessAIType1 khong duy nhat"
    pos = d.index(neo)
    trong = d.rfind(nl + nl, 0, pos)
    assert trong > 0
    vitri = trong + len(nl)
    d = d[:vitri] + than_ham(nl) + d[vitri:]

    kiem_cao(d0, d, "KNpcAI.cpp")
    saoluu(p)
    ghi(p, d)
    print("KNpcAI.cpp: switch -> 4 ham moi + them than ham (trong #ifdef _SERVER)")
    return True


if __name__ == "__main__":
    n = 0
    n += 1 if va_h() else 0
    n += 1 if va_cpp() else 0
    print("xong, %d tep da sua" % n)
