# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 KHOAMT c] GOC THAT cua "kich vao van khong di theo" - doc duoc bang NHAT KY va 85, khong doan:
#   [TTMT] hoi thong tin: nguoi=0 hover=245 khoa=0 -> chon 245
#   [TTMT] hoi thong tin: nguoi=0 hover=31  khoa=0 -> chon 31
#   [TTMT] hoi thong tin: nguoi=0 hover=101 khoa=0 -> chon 101
# tuc: (1) khoa = 0 SUOT -> KPlayer::OnButtonDown KHONG he chay tren Android (dong "[TTMT] cham" khong bao gio hien),
# nen cho dat khoa cua va 84 la cho CHET; (2) hover nhay lien tuc 245/31/101 vi SinhHover() cua KSdlApp bom
# WM_MOUSEMOVE moi khung tai cho ngon tay dat truoc, ma the gioi thi troi -> NPC duoi diem do doi lien tuc.
# Do la dung cai chu thay: "lo di chuot di cho khac la no doi muc tieu".
#
# SUA: dat khoa NGAY CHO CHAM THAT (KSdlApp, luc nha ngon = luc gui WM_LBUTTONDOWN), qua Core:
#   NPC_OI_TARGET_INFO nParam == 3 = "khoa muc tieu tai diem man hinh (nViTriVeX, nViTriVeY)" (dung lai ma so cu,
#   KHONG them ma GDI moi - them vao giua enum se day moi ma so phia sau lech di).
# Cham trung nguoi/NPC -> khoa vao do; cham dat / cham giao dien -> GIU khoa cu (chu: "kich vao se tu bam theo").
# Nhat ky [TTMT] chi in khi muc tieu DOI (truoc: moi 1 s) cho do ram log.
# Windows: khong doi (moi thu rao JX_ANDROID).

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 KHOAMT c]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten, so=1):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) == so:
        return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho (%d): %s" % (so, s.count(c), ten))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def va(p, ham):
    s = doc(p)
    if DAU in s:
        print("da va roi, bo qua:", p)
        return
    s2 = ham(s)
    if cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


# ---------------------------------------------------------------- Core: nParam == 3 = khoa tai diem
def va_coreshell(s):
    s = thay(s, ["\t\t// [ANDROID 09/09 DANH] nParam == 2: tra ve con DICH hop nhat de danh."],
             ["\t\t// %s nParam == 3: KHOA muc tieu tai diem man hinh (pInfo->nViTriVeX/nViTriVeY = cho ngon tay cham)." % DAU,
              "\t\t// KSdlApp goi luc nha ngon. Cham trung nguoi/NPC -> khoa vao do; cham dat -> GIU khoa cu.",
              "\t\t// Phai lam o day chu khong o KPlayer::OnButtonDown: nhat ky va 85 cho thay OnButtonDown KHONG chay tren Android.",
              "\t\tif (nParam == 3)",
              "\t\t{",
              "\t\t\tKUiTargetDetailInfo* pCham = (KUiTargetDetailInfo*)uParam;",
              "\t\t\textern int g_nJxMucTieuKhoa;",
              "\t\t\tint nCu = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();",
              "\t\t\tint nTrung = 0;",
              "",
              "\t\t\tif (pCham == NULL)",
              "\t\t\t\tbreak;",
              "\t\t\tPlayer[CLIENT_PLAYER_INDEX].FindSelectNpc(pCham->nViTriVeX, pCham->nViTriVeY, relation_all);",
              "\t\t\tnTrung = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();",
              "\t\t\tPlayer[CLIENT_PLAYER_INDEX].SetTargetNpc(nCu);\t// tra lai hover nhu cu, khong dung ham nay de chon muc tieu danh",
              "\t\t\tif (nTrung > 0 && nTrung < MAX_NPC)",
              "\t\t\t{",
              "\t\t\t\tg_nJxMucTieuKhoa = nTrung;",
              "\t\t\t\tg_DebugLog(\"[TTMT] cham (%d,%d) -> khoa muc tieu %d (%s)\", pCham->nViTriVeX, pCham->nViTriVeY, nTrung, Npc[nTrung].Name);",
              "\t\t\t\tnRet = 1;",
              "\t\t\t}",
              "\t\t\tbreak;",
              "\t\t}",
              "",
              "\t\t// [ANDROID 09/09 DANH] nParam == 2: tra ve con DICH hop nhat de danh."], "CoreShell.cpp: nParam 3")
    # nhat ky chi in khi doi
    s = thay(s, ["\t\t\t// [ANDROID 11/09 TTMT b] doc bang so xem khoa co chay khong (moi 1 s, theo nhip UpdateData)",
                 "\t\t\tg_DebugLog(\"[TTMT] hoi thong tin: nguoi=%d hover=%d khoa=%d -> chon %d\",",
                 "\t\t\t\tNpc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx, idx_hover, g_nJxMucTieuKhoa, idx);"],
                ["\t\t\t// [ANDROID 11/09 TTMT b] %s chi in khi muc tieu DOI (truoc: moi 1 s - qua ram)" % DAU,
                 "\t\t\t{",
                 "\t\t\t\tstatic int s_nTtIdxTruoc = -1;",
                 "\t\t\t\tif (idx != s_nTtIdxTruoc)",
                 "\t\t\t\t{",
                 "\t\t\t\t\ts_nTtIdxTruoc = idx;",
                 "\t\t\t\t\tg_DebugLog(\"[TTMT] thong tin doi: nguoi=%d hover=%d khoa=%d -> chon %d\",",
                 "\t\t\t\t\t\tNpc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx, idx_hover, g_nJxMucTieuKhoa, idx);",
                 "\t\t\t\t}",
                 "\t\t\t}"], "CoreShell.cpp: nhat ky chi khi doi")
    return s


# ---------------------------------------------------------------- KPlayer.cpp: bo nhat ky cho chet
def va_kplayer(s):
    return thay(s, ["\tg_DebugLog(\"[TTMT] cham (%d,%d) nut %d -> trung nguoi %d, khoa %d\", x, y, (int)nButton, m_nPeapleIdx, g_nJxMucTieuKhoa);\t// [ANDROID 11/09 TTMT b]"],
                ["\t// %s OnButtonDown KHONG chay tren Android (do bang nhat ky va 85) - khoa dat o CoreShell nParam 3, goi tu KSdlApp." % DAU],
                "KPlayer.cpp: bo nhat ky cho chet")


# ---------------------------------------------------------------- JxCanDieuKhien: ham khoa
def va_can(s):
    return thay(s, ["void JxCan_Ve()", "{"],
                ["//\t%s Khoa muc tieu tai cho ngon tay vua cham (KSdlApp goi luc nha ngon, ngoai giao dien)." % DAU,
                 "//\tCham trung nguoi/NPC -> thanh thong tin bam theo nguoi do; cham dat -> giu nguyen nguoi dang xem.",
                 "void JxMucTieu_Khoa(int x, int y)",
                 "{",
                 "\tKUiTargetDetailInfo oCham;",
                 "",
                 "\tif (g_pCoreShell == NULL)",
                 "\t\treturn;",
                 "\tmemset(&oCham, 0, sizeof(oCham));",
                 "\toCham.nViTriVeX = x;",
                 "\toCham.nViTriVeY = y;",
                 "\tg_pCoreShell->GetGameData(NPC_OI_TARGET_INFO, (KUPARAM)&oCham, 3);",
                 "}",
                 "",
                 "void JxCan_Ve()", "{"], "JxCanDieuKhien.cpp: JxMucTieu_Khoa")


def va_can_h(s):
    return thay(s, ["int  JxIconNpc_Cham(int x, int y, int* pnX, int* pnY);\t// [ANDROID 11/09 ICON b] cham trung icon Giao tiep -> toa do than NPC de bam"],
                ["int  JxIconNpc_Cham(int x, int y, int* pnX, int* pnY);\t// [ANDROID 11/09 ICON b] cham trung icon Giao tiep -> toa do than NPC de bam",
                 "void JxMucTieu_Khoa(int x, int y);\t// %s cham trung ai thi thanh thong tin bam theo nguoi do" % DAU],
                "JxCanDieuKhien.h")


# ---------------------------------------------------------------- KSdlApp: goi luc nha ngon
def va_sdlapp(s):
    return thay(s, ["\t\t\tLPARAM l = MAKELPARAM(m_nChamX0, m_nChamY0);",
                    "\t\t\tGhiChuot(MK_LBUTTON, l);",
                    "\t\t\tMsgProc(hWnd, bDup ? WM_LBUTTONDBLCLK : WM_LBUTTONDOWN, MK_LBUTTON, l);"],
                   ["\t\t\tLPARAM l = MAKELPARAM(m_nChamX0, m_nChamY0);",
                    "\t\t\t// %s cham ngoai giao dien: khoa muc tieu tai dung cho cham -> thanh thong tin bam theo nguoi vua cham," % DAU,
                    "\t\t\t// khong chay theo hover (SinhHover bom WM_MOUSEMOVE moi khung, the gioi troi nen NPC duoi con tro doi lien tuc).",
                    "\t\t\tif (!JxUi_CoGiaoDienTaiDiem(m_nChamX0, m_nChamY0))",
                    "\t\t\t\tJxMucTieu_Khoa(m_nChamX0, m_nChamY0);",
                    "\t\t\tGhiChuot(MK_LBUTTON, l);",
                    "\t\t\tMsgProc(hWnd, bDup ? WM_LBUTTONDBLCLK : WM_LBUTTONDOWN, MK_LBUTTON, l);"],
                   "KSdlApp.cpp: khoa luc nha ngon")


va("Sources/Core/Src/CoreShell.cpp", va_coreshell)
va("Sources/Core/Src/KPlayer.cpp", va_kplayer)
va("Sources/S3Client/Platform/JxCanDieuKhien.cpp", va_can)
va("Sources/S3Client/Platform/JxCanDieuKhien.h", va_can_h)
va("Sources/S3Client/Platform/KSdlApp.cpp", va_sdlapp)
print("xong")
