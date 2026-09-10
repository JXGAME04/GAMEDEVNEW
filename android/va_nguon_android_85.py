# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 TTMT b] Chu: "kich vao van khong di theo"; "toi moi doi toa do thong tin - o thong tin hien thi thieu mau
# ong mau"; "thu nho lai 30% nua".
#
# GOC LOI ONG MAU (doc ma, khong doan): KWndWindow::UiDatTiLe (WndWindow.cpp:254) CHI doi m_Width/m_Height cua chinh cua so
# (SetSize) - KHONG thu nho cac o CON. Nen va 84 nhan toa do ong mau voi ti le 0,7 trong khi khung (BackGround0), anh dau
# (Head0), ten (Name) va % mau (LifePercent) van ve nguyen co -> ong mau lech ra ngoai khung = "thieu mau ong mau".
# SUA: KUiTargetInfo::LoadScheme (chi JX_ANDROID) TU thu nho 6 o con theo [Cham] ThongTinTiLe (phan tram, mac dinh 50 =
# "70% roi 30% nua"), va PaintWindow gui DUNG ti le do xuong Core -> ong mau va khung luon khop nhau o moi co / vi tri.
# O chu (ten, % mau) giu chieu cao >= 12 cho khoi cat chu. Ong mau giu day >= 6 px.
#
# KHOA MUC TIEU: ma va 84 nam dung nhanh (CoreShell.cpp nParam==0, da soi) nhung chua ro g_nJxMucTieuKhoa co duoc dat khong.
# Them nhat ky [TTMT] o hai dau (KPlayer::OnButtonDown khi cham, CoreShell khi tra thong tin) de doc bang so, khong doan.
# Windows: khong doi (moi thu rao JX_ANDROID).

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 TTMT b]"


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


# ---------------------------------------------------------------- UiTargetInfo.cpp
def va_uitargetinfo(s):
    # 1. helper thu nho + doc ti le, dat ngay truoc LoadScheme
    s = thay(s, ["void KUiTargetInfo::LoadScheme(const char *pScheme)", "{"],
             ["#ifdef JX_ANDROID",
              "// %s Thu nho ca THANH thong tin muc tieu tren dien thoai." % DAU,
              "// KWndWindow::UiDatTiLe chi doi khung cua chinh cua so, KHONG thu nho o con - nen phai tu thu o day,",
              "// va gui DUNG ti le nay xuong Core (PaintWindow) de ong mau khop khung.",
              "// [Cham] ThongTinTiLe = phan tram (20..100), mac dinh 50.",
              "static int s_nTtTiLe = 0;",
              "",
              "int Tt_LayTiLe()",
              "{",
              "\tif (s_nTtTiLe == 0)",
              "\t{",
              "\t\tchar szCfg[MAX_PATH] = { 0 };",
              "",
              "\t\tGetCurrentDirectory(MAX_PATH, szCfg);",
              "\t\tstrcat(szCfg, \"\\\\Config.ini\");",
              "\t\ts_nTtTiLe = GetPrivateProfileInt(\"Cham\", \"ThongTinTiLe\", 50, szCfg);",
              "\t\tif (s_nTtTiLe < 20) s_nTtTiLe = 20;",
              "\t\tif (s_nTtTiLe > 100) s_nTtTiLe = 100;",
              "\t}",
              "\treturn s_nTtTiLe;",
              "}",
              "",
              "//\tnCaoToiThieu > 0: o chu - giu chieu cao do cho khoi cat chu.",
              "static void Tt_ThuNhoO(KWndWindow* pWnd, int nCaoToiThieu)",
              "{",
              "\tint nL = 0, nT = 0, nW = 0, nH = 0;",
              "\tconst int nTL = Tt_LayTiLe();",
              "",
              "\tif (pWnd == NULL || nTL >= 100)",
              "\t\treturn;",
              "\tpWnd->GetPosition(&nL, &nT);",
              "\tpWnd->GetSize(&nW, &nH);",
              "\tpWnd->SetPosition(nL * nTL / 100, nT * nTL / 100);",
              "\tnW = nW * nTL / 100;",
              "\tnH = nH * nTL / 100;",
              "\tif (nCaoToiThieu > 0 && nH < nCaoToiThieu)",
              "\t\tnH = nCaoToiThieu;",
              "\tpWnd->SetSize(nW, nH);",
              "}",
              "#endif",
              "",
              "void KUiTargetInfo::LoadScheme(const char *pScheme)",
              "{"], "UiTargetInfo.cpp: helper truoc LoadScheme")
    # 2. goi thu nho sau khi Init het o con
    s = thay(s, ["\t\tm_pSelf->m_pLifePercent.Init(&Ini, \"LifePercent\");",
                 "\t\tm_pSelf->m_pTargetName.Init(&Ini, \"Name\");",
                 "    }",
                 "}"],
                ["\t\tm_pSelf->m_pLifePercent.Init(&Ini, \"LifePercent\");",
                 "\t\tm_pSelf->m_pTargetName.Init(&Ini, \"Name\");",
                 "#ifdef JX_ANDROID",
                 "\t\t// %s thu nho ca thanh (Init o tren da dat lai tu ini nen chay lai LoadScheme khong bi thu hai lan)" % DAU,
                 "\t\tTt_ThuNhoO(&m_btnSwitch, 0);",
                 "\t\tTt_ThuNhoO(&m_btnCaptainFlag, 0);",
                 "\t\tTt_ThuNhoO(&m_pSelf->a_IconHead, 0);",
                 "\t\tTt_ThuNhoO(&m_pSelf->a_btnBackGround, 0);",
                 "\t\tTt_ThuNhoO(&m_pSelf->m_pLifePercent, 12);",
                 "\t\tTt_ThuNhoO(&m_pSelf->m_pTargetName, 12);",
                 "#endif",
                 "    }",
                 "}"], "UiTargetInfo.cpp: goi thu nho")
    # 2b. khai bao truoc PaintWindow (PaintWindow nam TRUOC LoadScheme trong tep)
    s = thay(s, ["void KUiTargetInfo::PaintWindow()", "{"],
             ["#ifdef JX_ANDROID",
              "int Tt_LayTiLe();	// " + DAU + " dinh nghia o duoi (truoc LoadScheme); PaintWindow dung truoc nen phai khai bao o day",
              "#endif",
              "",
              "void KUiTargetInfo::PaintWindow()", "{"], "UiTargetInfo.cpp: khai bao truoc PaintWindow")
    # 3. PaintWindow gui ti le CON (khong phai ti le khung cua so)
    s = thay(s, ["\t\tnPainTMG->nTiLe = (UiLayTiLe() > 0) ? UiLayTiLe() : 1000;"],
             ["\t\tnPainTMG->nTiLe = Tt_LayTiLe() * 10;\t// %s ti le CUA O CON (UiDatTiLe khong thu o con) - phai khop ong mau" % DAU],
             "UiTargetInfo.cpp: PaintWindow ti le")
    return s


# ---------------------------------------------------------------- KNpc.cpp: ong mau khong mong qua
def va_knpc(s):
    return thay(s, ["\t\tBlood.oEndPos.nY = nTtY0 + (75 + i * verDistance - 10) * nTtTL / 1000;\t// [ANDROID 11/09 TTMT]"],
                ["\t\tBlood.oEndPos.nY = nTtY0 + (75 + i * verDistance - 10) * nTtTL / 1000;\t// [ANDROID 11/09 TTMT]",
                 "\t\tif (Blood.oEndPos.nY - Blood.oPosition.nY < 6)",
                 "\t\t\tBlood.oEndPos.nY = Blood.oPosition.nY + 6;\t// %s thu nho nhieu thi ong mau van day >= 6 px" % DAU],
                "KNpc.cpp: day ong mau")


# ---------------------------------------------------------------- nhat ky khoa muc tieu
def va_coreshell(s):
    return thay(s, ["\t\t\tif (g_nJxMucTieuKhoa > 0 && g_nJxMucTieuKhoa < MAX_NPC && Npc[g_nJxMucTieuKhoa].m_dwID && Npc[g_nJxMucTieuKhoa].m_RegionIndex >= 0)",
                    "\t\t\t\tidx = g_nJxMucTieuKhoa;",
                    "\t\t\telse",
                    "\t\t\t\tg_nJxMucTieuKhoa = 0;",
                    "\t\t}"],
                   ["\t\t\tif (g_nJxMucTieuKhoa > 0 && g_nJxMucTieuKhoa < MAX_NPC && Npc[g_nJxMucTieuKhoa].m_dwID && Npc[g_nJxMucTieuKhoa].m_RegionIndex >= 0)",
                    "\t\t\t\tidx = g_nJxMucTieuKhoa;",
                    "\t\t\telse",
                    "\t\t\t\tg_nJxMucTieuKhoa = 0;",
                    "\t\t\t// %s doc bang so xem khoa co chay khong (moi 1 s, theo nhip UpdateData)" % DAU,
                    "\t\t\tg_DebugLog(\"[TTMT] hoi thong tin: nguoi=%d hover=%d khoa=%d -> chon %d\",",
                    "\t\t\t\tNpc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx, idx_hover, g_nJxMucTieuKhoa, idx);",
                    "\t\t}"], "CoreShell.cpp: nhat ky khoa")


def va_kplayer(s):
    return thay(s, ["\tif (m_nPeapleIdx)",
                    "\t\tg_nJxMucTieuKhoa = m_nPeapleIdx;\t// [ANDROID 11/09 KHOAMT] [ANDROID 11/09 TTMT] cham trung ai thi KHOA; cham dat KHONG bo khoa"],
                   ["\tif (m_nPeapleIdx)",
                    "\t\tg_nJxMucTieuKhoa = m_nPeapleIdx;\t// [ANDROID 11/09 KHOAMT] [ANDROID 11/09 TTMT] cham trung ai thi KHOA; cham dat KHONG bo khoa",
                    "\tg_DebugLog(\"[TTMT] cham (%d,%d) nut %d -> trung nguoi %d, khoa %d\", x, y, (int)nButton, m_nPeapleIdx, g_nJxMucTieuKhoa);\t// " + DAU],
                   "KPlayer.cpp: nhat ky cham")


def va_config(s):
    return thay(s, ["IconNpcGan=130"],
                ["IconNpcGan=130",
                 "; %s co thanh thong tin muc tieu, phan tram (20..100). Nho hon = thanh nho hon; ong mau tu khop theo." % DAU,
                 "ThongTinTiLe=50"], "config: ThongTinTiLe")


va("Sources/S3Client/Ui/UiCase/UiTargetInfo.cpp", va_uitargetinfo)
va("Sources/Core/Src/KNpc.cpp", va_knpc)
va("Sources/Core/Src/CoreShell.cpp", va_coreshell)
va("Sources/Core/Src/KPlayer.cpp", va_kplayer)
for p in ["android/du_lieu_ghi_de/config.ini", "D:/jx1_android_data/config.ini"]:
    if os.path.isfile(p):
        va(p, va_config)
print("xong")
