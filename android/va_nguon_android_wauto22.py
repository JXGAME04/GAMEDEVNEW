# -*- coding: utf-8 -*-
#
# [WAUTO 12/09] Hoan thien WAuto mobile - NOI TRANG VAO CAC VIEC.
#
#  1. VA UiWAutoDsach.h/.cpp : them KUiWAutoDsach::TomTat() - dong chu tom tat cua mot danh sach (de trang hien)
#  2. VA UiWAutoTrang.h/.cpp :
#     a) LOC DANH SACH CHIEU theo bay bang con cua WAuto.exe (WA_LC_*). Truoc day ca 19 hop chon chieu deu mo CUNG
#        MOT danh sach phang -> chon duoc chieu sai khe (nhet chieu danh vao o "Vong sang"), may auto nhan ma chieu
#        khong hop le cho khe do ma khong bao gi.
#     b) DANH SACH DAI (> 10 dong) mo BANG PHU thay cho KPopupMenu: menu khong cuon nen tu dong 24 tro di cham khong toi
#        (danh sach chieu toi 72 dong, hop "Di Xa Phu" 31 dong).
#     c) NUT co viec: "Lay" ban do / "Lay" toa do dang dung; bon nut mo bang danh sach.
#     d) HOP CHON co viec: Theo sau (ten nguoi quanh day -> szFollName), Ruong cua (danh sach doi theo thanh se ve,
#        gia tri = MA HUONG chu khong phai chi so dong), Boss co dinh (gia tri = 141 + dong).
#     e) CAP RADIO "Uu tien": Khoang cach (nPriority = 0) / Ngu hanh (nPriority = 1) - bam cai nay thi cai kia tu tat.
#     f) O SO LIEU dien luc chay moi 500 ms: sinh luc / noi luc / the luc / ban do / toa do / dang cap / kinh nghiem
#        (tab Co ban) va ban do + diem da dat (tab Di chuyen) - truoc day la chu "-" chet.
#     g) DONG TOM TAT cua moi o danh sach + nut "Sua" mo bang phu.
#
# Chi mobile (JX_MOBILE); ban PC khong doi mot hanh vi nao. Chay lai vo hai.

import io
import os
import re
import sys

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

DAU = "[WAUTO 12/09]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def V(s):
    return re.sub(r"@@(.*?)@@", lambda m: vn(m.group(1)), s, flags=re.S)


def thay(s, cu, moi, ten, so=1):
    nl = nl_cua(s)
    c = nl.join(V(x) for x in cu)
    if s.count(c) == so:
        return s.replace(c, nl.join(V(x) for x in moi))
    raise SystemExit("khong tim thay dung %d cho (%d): %s" % (so, s.count(c), ten))


def thay_dong(s, dau_nhan, dong_moi, ten):
    """thay CA DONG chua dau_nhan bang dong_moi - dung khi dong goc co chuoi TCVN3 (khong go lai duoc trong nguon UTF-8)"""
    nl = nl_cua(s)
    dong = s.split(nl)
    k = [i for i, d in enumerate(dong) if dau_nhan in d]
    if len(k) != 1:
        raise SystemExit("khong tim thay dung 1 dong (%d): %s" % (len(k), ten))
    dong[k[0]] = V(dong_moi)
    return nl.join(dong)


def va(p, dau, ham):
    s = doc(p)
    if dau in s:
        print("   bo qua (da va):", p)
        return
    ghi(p, ham(s))
    print("   da va:", p)


# ================================================================ 1. UiWAutoDsach: TomTat
def va_dsach_h(s):
    return thay(s, [
        "\tstatic int\tDangMo();",
    ], [
        "\t// %s Dong chu tom tat cua mot danh sach (trang hien ngay canh nut \"Sua\"): \"3 muc: 1/2, 3/4, ...\"" % DAU,
        "\tstatic void\tTomTat(int nViec, char* sz, int nMax);",
        "\tstatic int\tDangMo();",
    ], "UiWAutoDsach.h: TomTat")


def va_dsach_cpp(s):
    return thay(s, [
        "int KUiWAutoDsach::DangMo()",
    ], [
        "// %s Dong tom tat cho trang: so muc + vai muc dau. Khong can bang phu dang mo." % DAU,
        "void KUiWAutoDsach::TomTat(int nViec, char* sz, int nMax)",
        "{",
        "\tchar szD[WA_DS_CHU];",
        "\tautoData* p = JxWAuto_CauHinh();",
        "\tint n, i, nDai;",
        "\tif (!sz || nMax <= 0)",
        "\t\treturn;",
        "\tsz[0] = 0;",
        "\tif (!p || sizeof(autoData) != WA_SIZEOF_AUTODATA)",
        "\t\treturn;",
        "\tn = WA_DsSo(nViec, p);",
        "\tif (n <= 0)",
        "\t{",
        "\t\tsnprintf(sz, nMax, \"@@(chưa có mục nào)@@\");",
        "\t\treturn;",
        "\t}",
        "\tsnprintf(sz, nMax, \"@@%d mục: @@\", n);",
        "\tfor (i = 0; i < n; i++)",
        "\t{",
        "\t\tnDai = (int)strlen(sz);",
        "\t\tif (nDai + 8 >= nMax)",
        "\t\t\tbreak;",
        "\t\tWA_DsDong(nViec, p, i, szD, sizeof(szD));",
        "\t\tif (i)",
        "\t\t\tstrncat(sz, \", \", nMax - nDai - 1);",
        "\t\tstrncat(sz, szD, nMax - (int)strlen(sz) - 1);",
        "\t}",
        "}",
        "",
        "int KUiWAutoDsach::DangMo()",
    ], "UiWAutoDsach.cpp: TomTat")


# ================================================================ 2. UiWAutoTrang.h
def va_trang_h(s):
    s = thay(s, [
        "\tvoid\tDienChon(const WAUiMuc* p);",
        "\tvoid\tNapChieu();",
        "\tvoid\tMoMenuChon(int nKhe);",
    ], [
        "\tvoid\tDienChon(const WAUiMuc* p);",
        "\tvoid\tNapChieu();",
        "\t// %s cac viec them cua ban 12/09 - xem android/va_nguon_android_wauto22.py" % DAU,
        "\tvoid\tDienTick();\t\t\t\t\t// doc lai MOI o tick cua tab (cap radio \"Uu tien\" phai doi theo nhau)",
        "\tvoid\tCapNhatSong();\t\t\t\t// dien o so lieu nhan vat + dong tom tat danh sach (moi 500 ms)",
        "\tvoid\tLamViec(const WAUiMuc* p);\t// nut co WA_V_*",
        "\tint\t\tDungMenu(const WAUiMuc* p);\t// dung danh sach dong cho hop chon -> m_apDong / m_anGT; tra so dong",
        "\tconst WAUiMuc*\tMucTheoIdc(const char* szIdc);",
        "\tvoid\tMoMenuChon(int nKhe);",
    ], "UiWAutoTrang.h: khai bao ham moi")
    s = thay(s, [
        "\tKWndWindow*\t\t\tm_apAnTam[WA_TR_AN_TOI_DA];\t// widget tam an khi menu chon dang mo (trinh chieu Android ve anh + chu SAU bong menu)",
        "\tint\t\t\t\t\tm_nAnTam;",
    ], [
        "\tKWndWindow*\t\t\tm_apAnTam[WA_TR_AN_TOI_DA];\t// widget tam an khi menu chon dang mo (trinh chieu Android ve anh + chu SAU bong menu)",
        "\tint\t\t\t\t\tm_nAnTam;",
        "\tconst char*\t\t\tm_apDong[WA_MENU_TOI_DA];\t// %s cac dong cua menu / bang chon dang dung" % DAU,
        "\tint\t\t\t\t\tm_anGT[WA_MENU_TOI_DA];\t\t// gia tri ung voi tung dong (ma chieu / ma huong ruong / 141+i...)",
        "\tint\t\t\t\t\tm_nDongMenu;",
        "\tunsigned int\t\tm_uSongKe;\t\t\t\t\t// nhip dien lai o so lieu",
    ], "UiWAutoTrang.h: thanh vien moi")
    return s


# ================================================================ 3. UiWAutoTrang.cpp
def va_trang_cpp(s):
    # --- include bang phu + ham loc chieu
    s = thay(s, [
        '#include "UiWAutoBang.h"',
        '#include "../../Platform/JxWAutoNoiBo.h"',
    ], [
        '#include "UiWAutoBang.h"',
        '#include "UiWAutoDsach.h"\t// %s bang phu: danh sach + bo chon dai' % DAU,
        '#include "../../Platform/JxWAutoNoiBo.h"',
    ], "UiWAutoTrang.cpp: include bang phu")

    s = thay(s, [
        'extern "C" int JxCore_WAutoDanhSachChieu(IPCSkillInfo* pOut, int nMax);\t// CoreShell.cpp (chi Android)',
    ], [
        'extern "C" int JxCore_WAutoDanhSachChieu(IPCSkillInfo* pOut, int nMax);\t// CoreShell.cpp (chi Android)',
        'extern "C" int JxCore_WAutoSoLieu(int* pnSo, int nSoMax, char* szMap, int nMapMax, char* szKN, int nKNMax);',
        'extern "C" int JxCore_WAutoViTri(int* pnMapId, char* szMap, int nMapMax, int* pnX, int* pnY);',
        'extern "C" int JxCore_WAutoTenQuanhDay(char* pOut, int nMax);',
    ], "UiWAutoTrang.cpp: khai bao ham CoreShell")

    s = thay(s, [
        "#define WA_CHON_CAO\t\t24",
        "#define WA_MENU_TOI_DA\t40",
    ], [
        "#define WA_CHON_CAO\t\t24",
        "#define WA_MENU_TOI_DA\t100\t// %s du cho danh sach chieu (72) va ten nguoi quanh day (100)" % DAU,
        "#define WA_MENU_DAI\t\t10\t// tu so dong nay tro len thi mo BANG PHU (cuon duoc) thay cho KPopupMenu (khong cuon)",
        "",
        "static char s_szTenQuanh[100][32];\t// %s ten nguoi choi quanh day cho hop \"Theo sau\"" % DAU,
        "",
        "// %s Mot chieu co thuoc danh sach con nao khong - dung dieu kien cua WAuto.exe (WAuto.cpp:3057-3097)." % DAU,
        "// Game chi tra MOT danh sach phang (GetAllSkillByType) nen phai loc lai o day; khong loc thi chon duoc",
        "// chieu sai khe (vi du nhet chieu danh vao o \"Vong sang\").",
        "static int WA_ChieuHop(const IPCSkillInfo& s, int nLoc)",
        "{",
        "\tswitch (nLoc)",
        "\t{",
        "\tcase WA_LC_K:\treturn s.bAura ? 0 : 1;",
        "\tcase WA_LC_S:\treturn (!s.bAura && (s.bState || s.nStyle == 2) && s.bAlly) ? 1 : 0;",
        "\tcase WA_LC_SE:\treturn (s.bState && !s.bAlly) ? 1 : 0;",
        "\tcase WA_LC_BP:\treturn (s.nStyle <= 1 && !s.bAlly) ? 1 : 0;",
        "\tcase WA_LC_A:\treturn s.bAura ? 1 : 0;",
        "\t}",
        "\treturn 1;",
        "}",
    ], "UiWAutoTrang.cpp: WA_ChieuHop")

    # --- khoi tao thanh vien moi
    s = thay(s, [
        "\tm_bDangDien = 0;",
        "\tm_nAnTam = 0;",
        "}",
    ], [
        "\tm_bDangDien = 0;",
        "\tm_nAnTam = 0;",
        "\tm_nDongMenu = 0;",
        "\tm_uSongKe = 0;",
        "}",
    ], "UiWAutoTrang.cpp: khoi tao")

    # --- NapTab: nut co viec thi bat, them DSACH
    s = thay(s, [
        "\t\tcase WA_MUC_NUT:",
        "\t\t\tsprintf(szMuc, \"Nut%d\", m.nKhe);",
        "\t\t\tif (m.nKhe < WA_TR_NUT && Ini.IsSectionExist(szMuc))",
        "\t\t\t{",
        "\t\t\t\tm_Nut[m.nKhe].Init(&Ini, szMuc);",
        "\t\t\t\tm_Nut[m.nKhe].Show();",
        "\t\t\t\tm_Nut[m.nKhe].Enable(0);\t\t// (B2) nut hanh dong: lam o buoc cua tung tab",
        "\t\t\t}",
        "\t\t\tbreak;",
    ], [
        "\t\tcase WA_MUC_NUT:",
        "\t\tcase WA_MUC_DSACH:\t\t// %s o danh sach = nut \"Sua\" + dong tom tat (noi dung sua trong bang phu)" % DAU,
        "\t\t\tsprintf(szMuc, \"Nut%d\", m.nKhe);",
        "\t\t\tif (m.nKhe < WA_TR_NUT && Ini.IsSectionExist(szMuc))",
        "\t\t\t{",
        "\t\t\t\tm_Nut[m.nKhe].Init(&Ini, szMuc);",
        "\t\t\t\tm_Nut[m.nKhe].Show();",
        "\t\t\t\tm_Nut[m.nKhe].Enable(m.nViec != WA_V_KHONG);\t// %s nut nao co viec thi bat" % DAU,
        "\t\t\t}",
        "\t\t\tbreak;",
    ], "UiWAutoTrang.cpp: NapTab nut + dsach")

    # --- NapTab: hop chon co viec thi bat du nguon dong
    s = thay(s, [
        "\t\t\t\tm_Chon[m.nKhe].Enable(m.nOff >= 0 && m.nNguon != WA_NGUON_KHAC);",
    ], [
        "\t\t\t\t// %s hop chon nguon DONG (lay luc chay) van bat neu co viec: Theo sau lay ten nguoi quanh day" % DAU,
        "\t\t\t\tm_Chon[m.nKhe].Enable(m.nOff >= 0 && (m.nNguon != WA_NGUON_KHAC || m.nViec != WA_V_KHONG));",
    ], "UiWAutoTrang.cpp: NapTab hop chon")

    # --- DienGiaTri: tick cap radio + goi CapNhatSong
    s = thay(s, [
        "\t\tcase WA_MUC_TICK:",
        "\t\t\tif (m.nKhe < WA_TR_TICK)",
        "\t\t\t\tm_Tick[m.nKhe].CheckButton(LayInt(&m) ? 1 : 0);",
        "\t\t\tbreak;",
    ], [
        "\t\tcase WA_MUC_TICK:",
        "\t\t\tif (m.nKhe < WA_TR_TICK)",
        "\t\t\t{",
        "\t\t\t\tint v = LayInt(&m) ? 1 : 0;",
        "\t\t\t\tif (m.nViec == WA_V_UUTIEN_GAN)\t// %s cap radio: \"Khoang cach\" = nPriority BANG 0" % DAU,
        "\t\t\t\t\tv = !v;",
        "\t\t\t\tm_Tick[m.nKhe].CheckButton(v);",
        "\t\t\t}",
        "\t\t\tbreak;",
    ], "UiWAutoTrang.cpp: DienGiaTri tick")

    s = thay(s, [
        "\t\t}",
        "\t}",
        "\tm_bDangDien = 0;",
        "}",
        "",
        "void KUiWAutoTrang::NapChieu()",
    ], [
        "\t\t}",
        "\t}",
        "\tm_bDangDien = 0;",
        "\tCapNhatSong();\t\t// %s o so lieu + dong tom tat danh sach" % DAU,
        "}",
        "",
        "// %s doc lai MOI o tick cua tab: cap radio \"Uu tien\" cua the PK phai doi theo nhau" % DAU,
        "void KUiWAutoTrang::DienTick()",
        "{",
        "\tif (!m_pTab)",
        "\t\treturn;",
        "\tfor (int i = 0; i < m_pTab->nMuc; i++)",
        "\t{",
        "\t\tconst WAUiMuc& m = m_pTab->pMuc[i];",
        "\t\tif (m.nLoai != WA_MUC_TICK || m.nKhe >= WA_TR_TICK)",
        "\t\t\tcontinue;",
        "\t\tint v = LayInt(&m) ? 1 : 0;",
        "\t\tif (m.nViec == WA_V_UUTIEN_GAN)",
        "\t\t\tv = !v;",
        "\t\tm_Tick[m.nKhe].CheckButton(v);",
        "\t}",
        "}",
        "",
        "// %s Dien cac o CHU chay theo trang thai: so lieu nhan vat (the Co ban) va ban do / diem da dat (the Di chuyen)," % DAU,
        "// cung dong tom tat cua moi o danh sach. Ban PC dien may o nay trong ProcIpcCommand / UpdateUI.",
        "void KUiWAutoTrang::CapNhatSong()",
        "{",
        "\tchar sz[128];",
        "\tchar szMap[40] = { 0 };",
        "\tchar szKN[48] = { 0 };",
        "\tint aSo[12] = { 0 };",
        "\tint bCo, i;",
        "\tif (!m_pTab)",
        "\t\treturn;",
        "\tbCo = JxCore_WAutoSoLieu(aSo, 12, szMap, sizeof(szMap), szKN, sizeof(szKN));",
        "\tfor (i = 0; i < m_pTab->nMuc; i++)",
        "\t{",
        "\t\tconst WAUiMuc& m = m_pTab->pMuc[i];",
        "\t\tif (m.nLoai == WA_MUC_SOLIEU)",
        "\t\t{",
        "\t\t\tif (m.nKhe >= WA_TR_NHAN)",
        "\t\t\t\tcontinue;",
        "\t\t\tsz[0] = 0;",
        "\t\t\tswitch (m.nChiSo)",
        "\t\t\t{",
        "\t\t\tcase WA_SL_SINHLUC:\t\tif (bCo) snprintf(sz, sizeof(sz), \"%d/%d\", aSo[0], aSo[1]); break;",
        "\t\t\tcase WA_SL_NOILUC:\t\tif (bCo) snprintf(sz, sizeof(sz), \"%d/%d\", aSo[2], aSo[3]); break;",
        "\t\t\tcase WA_SL_THELUC:\t\tif (bCo) snprintf(sz, sizeof(sz), \"%d/%d\", aSo[4], aSo[5]); break;",
        "\t\t\tcase WA_SL_DANGCAP:\t\tif (bCo) snprintf(sz, sizeof(sz), \"%d\", aSo[6]); break;",
        "\t\t\tcase WA_SL_BANDO:\t\tif (bCo) snprintf(sz, sizeof(sz), \"%s\", szMap); break;",
        "\t\t\tcase WA_SL_TOADO:\t\tif (bCo) snprintf(sz, sizeof(sz), \"%d / %d\", aSo[8], aSo[9]); break;",
        "\t\t\tcase WA_SL_KINHNGHIEM:\tif (bCo) snprintf(sz, sizeof(sz), \"%s\", szKN); break;",
        "\t\t\tcase WA_SL_MAP_AP:",
        "\t\t\t{",
        "\t\t\t\tconst char* a = (const char*)DiaChi(&m);",
        "\t\t\t\tif (a && a[0])",
        "\t\t\t\t\tsnprintf(sz, sizeof(sz), \"%s\", a);",
        "\t\t\t\tbreak;",
        "\t\t\t}",
        "\t\t\tcase WA_SL_DIEM_X:",
        "\t\t\tcase WA_SL_DIEM_Y:",
        "\t\t\t\tif (m.nOff >= 0)",
        "\t\t\t\t\tsnprintf(sz, sizeof(sz), \"%d\", LayInt(&m));",
        "\t\t\t\tbreak;",
        "\t\t\t}",
        "\t\t\tm_Nhan[m.nKhe].SetText(sz[0] ? sz : \"-\");",
        "\t\t}",
        "\t\telse if (m.nLoai == WA_MUC_DSACH && m.nKheNhan < WA_TR_NHAN)",
        "\t\t{",
        "\t\t\tKUiWAutoDsach::TomTat(m.nViec, sz, sizeof(sz));",
        "\t\t\tm_Nhan[m.nKheNhan].SetText(sz);",
        "\t\t}",
        "\t}",
        "}",
        "",
        "const WAUiMuc* KUiWAutoTrang::MucTheoIdc(const char* szIdc)",
        "{",
        "\tif (!m_pTab || !szIdc)",
        "\t\treturn NULL;",
        "\tfor (int i = 0; i < m_pTab->nMuc; i++)",
        "\t\tif (m_pTab->pMuc[i].szIdc && !strcmp(m_pTab->pMuc[i].szIdc, szIdc))",
        "\t\t\treturn &m_pTab->pMuc[i];",
        "\treturn NULL;",
        "}",
        "",
        "// %s Nut co viec: hai nut \"Lay\" cua the Di chuyen va bon nut mo bang danh sach." % DAU,
        "void KUiWAutoTrang::LamViec(const WAUiMuc* p)",
        "{",
        "\tautoData* pCH = JxWAuto_CauHinh();",
        "\tchar szMap[32] = { 0 };",
        "\tint nMapId = 0, x = 0, y = 0;",
        "\tif (!p || !pCH || sizeof(autoData) != WA_SIZEOF_AUTODATA)",
        "\t\treturn;",
        "\tswitch (p->nViec)",
        "\t{",
        "\tcase WA_V_LAY_BANDO:",
        "\tcase WA_V_LAY_DIEM:",
        "\t\tif (!JxCore_WAutoViTri(&nMapId, szMap, sizeof(szMap), &x, &y))",
        "\t\t\treturn;",
        "\t\tif (p->nViec == WA_V_LAY_DIEM)",
        "\t\t{",
        "\t\t\tpCH->nPointX = x;",
        "\t\t\tpCH->nPointY = y;",
        "\t\t}",
        "\t\tpCH->nMoveMapId = nMapId;\t\t// nhu ban PC: hai nut nay deu dat lai ban do ap dung",
        "\t\tstrncpy(pCH->szMoveMap, szMap, sizeof(pCH->szMoveMap) - 1);",
        "\t\tpCH->szMoveMap[sizeof(pCH->szMoveMap) - 1] = 0;",
        "\t\tDaDoi();",
        "\t\tCapNhatSong();",
        "\t\tg_DebugLog(\"[WAUTO-UI] %s: map=%d %s (%d,%d)\", p->szIdc, nMapId, szMap, x, y);",
        "\t\tbreak;",
        "\tcase WA_V_DS_TOADO:",
        "\tcase WA_V_DS_LOC:",
        "\tcase WA_V_DS_KHONGNHAT:",
        "\tcase WA_V_DS_TODOI:",
        "\tcase WA_V_DS_NGUHANH:",
        "\tcase WA_V_DS_LIENDAU:",
        "\t\tKUiWAutoDsach::MoSua(p->nViec);",
        "\t\tbreak;",
        "\t}",
        "}",
        "",
        "void KUiWAutoTrang::NapChieu()",
    ], "UiWAutoTrang.cpp: DienTick / CapNhatSong / LamViec")

    # --- DienChon: theo viec
    s = thay(s, [
        "\tv = LayInt(p);",
        "\tsz[0] = 0;",
        "\tif (p->nNguon == WA_NGUON_CHIEU)",
    ], [
        "\tsz[0] = 0;",
        "\tif (p->nKieu == WA_KIEU_CHUOI)",
        "\t{\t// %s hop \"Theo sau\": gia tri la TEN nhan vat (szFollName), khong phai so" % DAU,
        "\t\tconst char* a = (const char*)DiaChi(p);",
        "\t\tif (a && a[0])",
        "\t\t\tstrncpy(sz, a, 31);",
        "\t\telse",
        "\t\t\tstrcpy(sz, \"@@(không theo ai)@@\");",
        "\t\tsz[31] = 0;",
        "\t\tv = 0;",
        "\t}",
        "\telse if (p->nViec == WA_V_ST_BOSS)",
        "\t{\t// %s gia tri = 141 + chi so dong (WAuto.cpp:1328)" % DAU,
        "\t\tv = LayInt(p) - 141;",
        "\t\tif (v < 0 || v >= (int)p->nLuaChon)",
        "\t\t\tv = 0;",
        "\t\tstrncpy(sz, p->pLuaChon[v], 31);",
        "\t\tsz[31] = 0;",
        "\t}",
        "\telse if (p->nViec == WA_V_TK_RUONG)",
        "\t{\t// %s gia tri = MA HUONG 0..4, 5 = gan nhat; danh sach tinh xep dung theo ma nen chi so = ma" % DAU,
        "\t\tv = LayInt(p);",
        "\t\tif (v < 0 || v >= (int)p->nLuaChon)",
        "\t\t\tv = (int)p->nLuaChon - 1;",
        "\t\tstrncpy(sz, p->pLuaChon[v], 31);",
        "\t\tsz[31] = 0;",
        "\t}",
        "\telse if (p->nNguon == WA_NGUON_CHIEU)",
    ], "UiWAutoTrang.cpp: DienChon theo viec")

    s = thay(s, [
        "\tint i, v;",
        "\tif (!p || p->nKhe >= WA_TR_CHON)",
        "\t\treturn;",
    ], [
        "\tint i, v = 0;",
        "\tif (!p || p->nKhe >= WA_TR_CHON)",
        "\t\treturn;",
    ], "UiWAutoTrang.cpp: DienChon khoi tao v")

    # --- MoMenuChon: dung danh sach theo viec + mo bang phu khi dai
    s = thay(s, [
        "void KUiWAutoTrang::MoMenuChon(int nKhe)",
        "{",
        "\tconst WAUiMuc* p = Muc(WA_MUC_CHON, nKhe);",
        "\tint i, n = 0, x = 0, y = 0;",
        "\tif (!p || nKhe >= WA_TR_CHON)",
        "\t\treturn;",
        "\tif (p->nNguon == WA_NGUON_CHIEU)",
        "\t{",
        "\t\tNapChieu();",
        "\t\tn = m_nChieu + 1;",
        "\t}",
        "\telse if (p->pLuaChon)",
        "\t\tn = p->nLuaChon;",
        "\tif (n <= 0)",
        "\t\treturn;",
        "\tif (n > WA_MENU_TOI_DA)",
        "\t\tn = WA_MENU_TOI_DA;",
    ], [
        "// %s Dung danh sach dong cho mot hop chon: m_apDong[i] = chu, m_anGT[i] = gia tri se ghi vao autoData." % DAU,
        "// Tra so dong. Cac hop co viec rieng: Theo sau (ten quanh day), Ruong cua (loc theo thanh), Boss (141 + dong).",
        "int KUiWAutoTrang::DungMenu(const WAUiMuc* p)",
        "{",
        "\tint i, n = 0;",
        "\tm_nDongMenu = 0;",
        "\tif (!p)",
        "\t\treturn 0;",
        "\tif (p->nViec == WA_V_THEO_SAU)",
        "\t{",
        "\t\tint nT = JxCore_WAutoTenQuanhDay((char*)s_szTenQuanh, 100);",
        "\t\tm_apDong[0] = \"@@(không theo ai)@@\";",
        "\t\tm_anGT[0] = 0;",
        "\t\tn = 1;",
        "\t\tfor (i = 0; i < nT && n < WA_MENU_TOI_DA; i++)",
        "\t\t{",
        "\t\t\tm_apDong[n] = s_szTenQuanh[i];",
        "\t\t\tm_anGT[n] = i + 1;",
        "\t\t\tn++;",
        "\t\t}",
        "\t}",
        "\telse if (p->nViec == WA_V_TK_RUONG)",
        "\t{\t// chi liet ke huong CO THAT cua thanh dang chon o hop \"Het tran ve\"; gia tri = MA HUONG",
        "\t\tconst WAUiMuc* pVe = MucTheoIdc(\"IDC_COMBO_9_VE\");",
        "\t\tint nThanh = pVe ? LayInt(pVe) : 0;",
        "\t\tif (nThanh < 0 || nThanh >= WA_TKR_THANH)",
        "\t\t\tnThanh = 0;",
        "\t\tfor (i = 0; i < WA_TKR_HUONG && i < (int)p->nLuaChon; i++)",
        "\t\t\tif (s_WATKRuongCo[nThanh][i])",
        "\t\t\t{",
        "\t\t\t\tm_apDong[n] = p->pLuaChon[i];",
        "\t\t\t\tm_anGT[n] = i;",
        "\t\t\t\tn++;",
        "\t\t\t}",
        "\t\tif ((int)p->nLuaChon > WA_TKR_HUONG)",
        "\t\t{\t// dong cuoi: \"Gan nhat (tu chon)\" = ma 5",
        "\t\t\tm_apDong[n] = p->pLuaChon[WA_TKR_HUONG];",
        "\t\t\tm_anGT[n] = WA_TKR_HUONG;",
        "\t\t\tn++;",
        "\t\t}",
        "\t}",
        "\telse if (p->nNguon == WA_NGUON_CHIEU)",
        "\t{",
        "\t\tNapChieu();",
        "\t\tm_apDong[0] = \"@@Không thiết lập@@\";",
        "\t\tm_anGT[0] = 0;",
        "\t\tn = 1;",
        "\t\tfor (i = 0; i < m_nChieu && n < WA_MENU_TOI_DA; i++)",
        "\t\t\tif (WA_ChieuHop(m_aChieu[i], p->nLocChieu))",
        "\t\t\t{",
        "\t\t\t\tm_apDong[n] = m_aChieu[i].szName;",
        "\t\t\t\tm_anGT[n] = m_aChieu[i].nId;",
        "\t\t\t\tn++;",
        "\t\t\t}",
        "\t}",
        "\telse if (p->pLuaChon)",
        "\t{",
        "\t\tfor (i = 0; i < (int)p->nLuaChon && n < WA_MENU_TOI_DA; i++)",
        "\t\t{",
        "\t\t\tm_apDong[n] = p->pLuaChon[i];",
        "\t\t\tm_anGT[n] = (p->nViec == WA_V_ST_BOSS) ? (141 + i) : i;",
        "\t\t\tn++;",
        "\t\t}",
        "\t}",
        "\tm_nDongMenu = n;",
        "\treturn n;",
        "}",
        "",
        "void KUiWAutoTrang::MoMenuChon(int nKhe)",
        "{",
        "\tconst WAUiMuc* p = Muc(WA_MUC_CHON, nKhe);",
        "\tint i, n = 0, x = 0, y = 0;",
        "\tif (!p || nKhe >= WA_TR_CHON)",
        "\t\treturn;",
        "\tn = DungMenu(p);",
        "\tif (n <= 0)",
        "\t\treturn;",
        "\tif (n >= WA_MENU_DAI)",
        "\t{\t// %s danh sach dai: KPopupMenu khong cuon (cao = so dong x 26 px) nen dong thu 24 tro di cham khong toi" % DAU,
        "\t\tchar szTieuDe[80];",
        "\t\tint nCu = -1;",
        "\t\tint vGio = (p->nKieu == WA_KIEU_CHUOI) ? -1 : LayInt(p);",
        "\t\tfor (i = 0; i < n; i++)",
        "\t\t\tif (vGio >= 0 && m_anGT[i] == vGio)",
        "\t\t\t{",
        "\t\t\t\tnCu = i;",
        "\t\t\t\tbreak;",
        "\t\t\t}",
        "\t\tm_Chon[nKhe].GetLabel(szTieuDe, sizeof(szTieuDe));",
        "\t\tKUiWAutoDsach::MoChonMuc(this, nKhe, szTieuDe, m_apDong, n, nCu);",
        "\t\treturn;",
        "\t}",
    ], "UiWAutoTrang.cpp: DungMenu + MoMenuChon")

    # --- MoMenuChon: dien dong tu m_apDong (dong nay co chuoi TCVN3 nen thay CA DONG theo dau nhan dang)
    s = thay_dong(s, "const char* s = (p->nNguon == WA_NGUON_CHIEU)",
                  "\t\tconst char* s = m_apDong[i];\t// %s danh sach da dung san (loc chieu / loc huong ruong / 141+i)" % DAU,
                  "UiWAutoTrang.cpp: dong menu tu m_apDong")

    # --- ChonMenu: theo m_anGT / chuoi
    s = thay(s, [
        "void KUiWAutoTrang::ChonMenu(int nKhe, int nMuc)",
        "{",
        "\tconst WAUiMuc* p = Muc(WA_MUC_CHON, nKhe);",
        "\tint v;",
        "\tif (!p || nMuc < 0)",
        "\t\treturn;",
        "\tif (p->nNguon == WA_NGUON_CHIEU)",
        "\t\tv = (nMuc == 0 || nMuc - 1 >= m_nChieu) ? 0 : m_aChieu[nMuc - 1].nId;",
        "\telse",
        "\t\tv = nMuc;",
        "\tDatInt(p, v);",
        "\tDienChon(p);",
        "\tDaDoi();",
        "\tg_DebugLog(\"[WAUTO-UI] %s = %d\", p->szIdc, v);",
        "}",
    ], [
        "void KUiWAutoTrang::ChonMenu(int nKhe, int nMuc)",
        "{",
        "\tconst WAUiMuc* p = Muc(WA_MUC_CHON, nKhe);",
        "\tint v;",
        "\tif (!p || nMuc < 0 || nMuc >= m_nDongMenu)",
        "\t\treturn;",
        "\tif (p->nKieu == WA_KIEU_CHUOI)",
        "\t{\t// %s hop \"Theo sau\": ghi TEN vao szFollName (dong 0 = khong theo ai)" % DAU,
        "\t\tchar* a = (char*)DiaChi(p);",
        "\t\tif (!a || p->nCo <= 0)",
        "\t\t\treturn;",
        "\t\tif (nMuc == 0)",
        "\t\t\ta[0] = 0;",
        "\t\telse",
        "\t\t{",
        "\t\t\tstrncpy(a, m_apDong[nMuc] ? m_apDong[nMuc] : \"\", p->nCo - 1);",
        "\t\t\ta[p->nCo - 1] = 0;",
        "\t\t}",
        "\t\tDienChon(p);",
        "\t\tDaDoi();",
        "\t\tg_DebugLog(\"[WAUTO-UI] %s = %s\", p->szIdc, a);",
        "\t\treturn;",
        "\t}",
        "\tv = m_anGT[nMuc];",
        "\tDatInt(p, v);",
        "\tDienChon(p);",
        "\tif (p->nViec == WA_V_TK_RUONG || MucTheoIdc(\"IDC_COMBO_9_RH\"))",
        "\t{\t// %s doi THANH se ve -> danh sach \"Ruong cua\" khac di, doc lai cho khoi lech (nhu WA_NapRuongHuong ben PC)" % DAU,
        "\t\tconst WAUiMuc* pRH = MucTheoIdc(\"IDC_COMBO_9_RH\");",
        "\t\tif (pRH && pRH != p)",
        "\t\t\tDienChon(pRH);",
        "\t}",
        "\tDaDoi();",
        "\tg_DebugLog(\"[WAUTO-UI] %s = %d\", p->szIdc, v);",
        "}",
    ], "UiWAutoTrang.cpp: ChonMenu theo m_anGT")

    # --- WndProc: tick cap radio, nut co viec
    s = thay(s, [
        "\t\t\t\tconst WAUiMuc* p = Muc(WA_MUC_TICK, i);",
        "\t\t\t\tif (p)",
        "\t\t\t\t{",
        "\t\t\t\t\tint v = LayInt(p) ? 0 : 1;",
        "\t\t\t\t\tDatInt(p, v);",
        "\t\t\t\t\tm_Tick[i].CheckButton(v);",
        "\t\t\t\t\tDaDoi();",
        "\t\t\t\t\tg_DebugLog(\"[WAUTO-UI] %s = %d\", p->szIdc, v);",
        "\t\t\t\t}",
    ], [
        "\t\t\t\tconst WAUiMuc* p = Muc(WA_MUC_TICK, i);",
        "\t\t\t\tif (p)",
        "\t\t\t\t{",
        "\t\t\t\t\tint v;",
        "\t\t\t\t\tif (p->nViec == WA_V_UUTIEN_GAN)\t\t// %s cap radio cua the PK: chon la DAT han, khong dao" % DAU,
        "\t\t\t\t\t\tv = 0;",
        "\t\t\t\t\telse if (p->nViec == WA_V_UUTIEN_NGU)",
        "\t\t\t\t\t\tv = 1;",
        "\t\t\t\t\telse",
        "\t\t\t\t\t\tv = LayInt(p) ? 0 : 1;",
        "\t\t\t\t\tDatInt(p, v);",
        "\t\t\t\t\tDienTick();\t\t\t\t\t\t\t\t// doc lai ca tab: o cung cap tu tat",
        "\t\t\t\t\tDaDoi();",
        "\t\t\t\t\tg_DebugLog(\"[WAUTO-UI] %s = %d\", p->szIdc, v);",
        "\t\t\t\t}",
    ], "UiWAutoTrang.cpp: WndProc tick")

    s = thay(s, [
        "\t\tfor (i = 0; i < WA_TR_NUT; i++)",
        "\t\t\tif (uParam == (KUPARAM)(KWndWindow*)&m_Nut[i])",
        "\t\t\t\treturn 1;\t\t// (B2) chua co hanh dong",
    ], [
        "\t\tfor (i = 0; i < WA_TR_NUT; i++)",
        "\t\t\tif (uParam == (KUPARAM)(KWndWindow*)&m_Nut[i])",
        "\t\t\t{",
        "\t\t\t\tconst WAUiMuc* p = Muc(WA_MUC_NUT, i);\t// %s nut thuong" % DAU,
        "\t\t\t\tif (!p)",
        "\t\t\t\t\tp = Muc(WA_MUC_DSACH, i);\t\t\t// hoac nut \"Sua\" cua o danh sach",
        "\t\t\t\tLamViec(p);",
        "\t\t\t\treturn 1;",
        "\t\t\t}",
    ], "UiWAutoTrang.cpp: WndProc nut")

    # --- Breathe: dien lai o so lieu moi 500 ms
    s = thay(s, [
        "void KUiWAutoTrang::Breathe()",
        "{",
        "\tif (m_nAnTam > 0 && KPopupMenu::GetMenuData() == NULL)\t// menu bi huy ma khong bao (Cancel tu noi khac)",
        "\t\tAnDuoiMenu(0);",
        "}",
    ], [
        "void KUiWAutoTrang::Breathe()",
        "{",
        "\tif (m_nAnTam > 0 && KPopupMenu::GetMenuData() == NULL)\t// menu bi huy ma khong bao (Cancel tu noi khac)",
        "\t\tAnDuoiMenu(0);",
        "\t{\t// %s o so lieu nhan vat chay theo game -> dien lai moi 500 ms (nhu WAuto.exe nhan PRG_MAINSYNC)" % DAU,
        "\t\tunsigned int uNow = (unsigned int)timeGetTime();",
        "\t\tif (uNow >= m_uSongKe)",
        "\t\t{",
        "\t\t\tm_uSongKe = uNow + 500;",
        "\t\t\tCapNhatSong();",
        "\t\t}",
        "\t}",
        "}",
    ], "UiWAutoTrang.cpp: Breathe cap nhat song")
    return s


def main():
    va("Sources/S3Client/Ui/UiCase/UiWAutoDsach.h", "TomTat", va_dsach_h)
    va("Sources/S3Client/Ui/UiCase/UiWAutoDsach.cpp", "KUiWAutoDsach::TomTat", va_dsach_cpp)
    va("Sources/S3Client/Ui/UiCase/UiWAutoTrang.h", "CapNhatSong", va_trang_h)
    va("Sources/S3Client/Ui/UiCase/UiWAutoTrang.cpp", "WA_ChieuHop", va_trang_cpp)
    print("xong wauto10")


if __name__ == "__main__":
    main()
