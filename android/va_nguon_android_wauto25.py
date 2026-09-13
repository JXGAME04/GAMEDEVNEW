# -*- coding: utf-8 -*-
#
# [WAUTO 12/09] Hoan thien WAuto mobile - GHI CHU / HUONG DAN.
#
# Ban PC co 283 ghi chu re chuot (s_aTips) + 17 trang huong dan theo the (s_aNote) mo bang nut [H] / [Tro giup] / [?].
# Dien thoai khong co "re chuot" nen gop lai: nut [?] tren dai tieu de khung Auto mo BANG PHU hien huong dan cua THE
# DANG MO - trang note cua the roi tung o mot ("nhan: ghi chu"). Day la cho chu bao "nhieu tinh nang khong biet de lam gi".
#
#  1. VA android/sinh_ghichu_wauto.py : them muc [Nhan] (nhan chu cua tung o) de bang phu khong phai hoi trang
#  2. VA UiWAutoDsach.h/.cpp : che do 3 = hien CHU (cuon duoc); MoGhiChu(nTab, ten the)
#  3. VA UiWAuto.h/.cpp + uiwauto.ini : nut [?] canh nut Dong
#
# Chi mobile (JX_MOBILE). Chay lai vo hai.

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


def va(p, dau, ham):
    s = doc(p)
    if dau in s:
        print("   bo qua (da va):", p)
        return
    ghi(p, ham(s))
    print("   da va:", p)


# ---------------------------------------------------------------- 2. UiWAutoDsach.h
def va_dsach_h(s):
    s = thay(s, [
        "#define WA_DS_TOI_DA\t100\t\t// so dong toi da giu trong o (nguoi quanh day toi 100; szNOPName 60; chieu 72)",
        "#define WA_DS_CHU\t\t64\t\t// do dai moi dong",
    ], [
        "#define WA_DS_TOI_DA\t240\t\t// so dong toi da (trang huong dan cua mot the dai nhat khoang 200 dong)",
        "#define WA_DS_CHU\t\t96\t\t// do dai moi dong (o rong 560 px, font 12 = 6 px/byte -> 93 byte)",
    ], "UiWAutoDsach.h: noi o danh sach")
    s = thay(s, [
        "\tstatic int\tDangMo();",
    ], [
        "\t// %s che do 3: hien HUONG DAN cua the dang mo (trang note + ghi chu tung o), doc uiwauto_ghichu.ini" % DAU,
        "\tstatic int\tMoGhiChu(int nTab, const char* szTenThe);",
        "\tstatic int\tDangMo();",
    ], "UiWAutoDsach.h: MoGhiChu")
    s = thay(s, [
        "\tvoid\tDoi(int nBuoc);\t\t\t\t\t\t// doi cho dong dang chon len / xuong",
    ], [
        "\tvoid\tDoi(int nBuoc);\t\t\t\t\t\t// doi cho dong dang chon len / xuong",
        "\tvoid\tNapGhiChu(int nTab);\t\t\t\t\t// %s che do 3" % DAU,
        "\tvoid\tXuongDong(const char* szChu, const char* szDau);\t// cat chu dai thanh nhieu dong vua be ngang o",
    ], "UiWAutoDsach.h: ham che do 3")
    return s


# ---------------------------------------------------------------- 2b. UiWAutoDsach.cpp
def va_dsach_cpp(s):
    s = thay(s, [
        "#define WA_DS_SCHEME\t\"UiWAuto_Dsach.ini\"",
    ], [
        "#define WA_DS_SCHEME\t\"UiWAuto_Dsach.ini\"",
        "#define WA_DS_GHICHU\t\"UiWAuto_GhiChu.ini\"\t// %s sinh boi android/sinh_ghichu_wauto.py tu s_aTips + s_aNote cua WAuto.cpp" % DAU,
    ], "UiWAutoDsach.cpp: ten tep ghi chu")

    s = thay(s, [
        "int KUiWAutoDsach::MoSua(int nViec)",
    ], [
        "// %s Cat mot doan chu dai thanh nhieu dong vua be ngang o danh sach (cat o dau cach, khong cat giua tu)." % DAU,
        "void KUiWAutoDsach::XuongDong(const char* szChu, const char* szDau)",
        "{",
        "\tchar sz[WA_DS_CHU];",
        "\tint nRong, nMax, nDauDai, i, k, nCat;",
        "\tif (!szChu || !szChu[0])",
        "\t\treturn;",
        "\tm_Dsach.GetSize(&nRong, &i);",
        "\tnMax = (nRong - 12) / 6;\t\t\t\t// font 12: 6 px moi byte TCVN3",
        "\tif (nMax > WA_DS_CHU - 1)",
        "\t\tnMax = WA_DS_CHU - 1;",
        "\tif (nMax < 8)",
        "\t\tnMax = 8;",
        "\tnDauDai = szDau ? (int)strlen(szDau) : 0;",
        "\ti = 0;",
        "\twhile (szChu[i])",
        "\t{",
        "\t\tint nCho = nMax - (i == 0 ? nDauDai : 2);\t// dong sau lui vao 2 ky tu",
        "\t\tif (nCho < 8)",
        "\t\t\tnCho = 8;",
        "\t\tnCat = 0;",
        "\t\tfor (k = 0; k < nCho && szChu[i + k]; k++)",
        "\t\t\tif (szChu[i + k] == ' ')",
        "\t\t\t\tnCat = k;",
        "\t\tif (!szChu[i + k])",
        "\t\t\tnCat = k;\t\t\t\t\t\t// het chu",
        "\t\telse if (nCat <= 0)",
        "\t\t\tnCat = nCho;\t\t\t\t\t// mot tu dai hon ca dong: cat cung",
        "\t\tsz[0] = 0;",
        "\t\tif (i == 0 && nDauDai)",
        "\t\t\tstrncpy(sz, szDau, WA_DS_CHU - 1);",
        "\t\telse if (i)",
        "\t\t\tstrcpy(sz, \"  \");",
        "\t\tk = (int)strlen(sz);",
        "\t\tif (nCat > WA_DS_CHU - 1 - k)",
        "\t\t\tnCat = WA_DS_CHU - 1 - k;",
        "\t\tmemcpy(sz + k, szChu + i, nCat);",
        "\t\tsz[k + nCat] = 0;",
        "\t\tif (m_Dsach.Them(sz) < 0)",
        "\t\t\treturn;",
        "\t\ti += nCat;",
        "\t\twhile (szChu[i] == ' ')",
        "\t\t\ti++;",
        "\t}",
        "}",
        "",
        "// %s Trang huong dan cua mot the: trang note cua the (dau || la xuong doan) roi ghi chu tung o." % DAU,
        "void KUiWAutoDsach::NapGhiChu(int nTab)",
        "{",
        "\tchar Scheme[128];",
        "\tchar Buff[256];",
        "\tchar szKhoa[8];",
        "\tstatic char s_szChu[4096];",
        "\tstatic char s_szNhan[128];",
        "\tKIniFile Ini;",
        "\tint i;",
        "\tm_Dsach.XoaHet();",
        "\tg_UiBase.GetCurSchemePath(Scheme, 128);",
        "\tsprintf(Buff, \"%s\\\\%s\", Scheme, WA_DS_GHICHU);",
        "\tif (!Ini.Load(Buff))",
        "\t{",
        "\t\tm_Dsach.Them(\"@@Chưa có tệp ghi chú (uiwauto_ghichu.ini).@@\");",
        "\t\treturn;",
        "\t}",
        "\tsnprintf(szKhoa, sizeof(szKhoa), \"%d\", nTab);",
        "\ts_szChu[0] = 0;",
        "\tif (Ini.GetString(\"The\", szKhoa, \"\", s_szChu, sizeof(s_szChu)) && s_szChu[0])",
        "\t{\t// dau || = xuong doan",
        "\t\tchar* p = s_szChu;",
        "\t\twhile (p && *p)",
        "\t\t{",
        "\t\t\tchar* q = strstr(p, \"||\");",
        "\t\t\tif (q)",
        "\t\t\t\t*q = 0;",
        "\t\t\tXuongDong(p, \"\");",
        "\t\t\tif (!q)",
        "\t\t\t\tbreak;",
        "\t\t\tp = q + 2;",
        "\t\t}",
        "\t\tm_Dsach.Them(\"\");",
        "\t}",
        "\tif (nTab < 0 || nTab >= WA_TR_SO_TAB)",
        "\t\treturn;",
        "\tm_Dsach.Them(\"@@--- Từng ô một ---@@\");",
        "\t{",
        "\t\tconst WAUiTab* pT = &s_WAUiTab[nTab];",
        "\t\tfor (i = 0; i < pT->nMuc; i++)",
        "\t\t{",
        "\t\t\tconst WAUiMuc& m = pT->pMuc[i];",
        "\t\t\tif (!m.szIdc)",
        "\t\t\t\tcontinue;",
        "\t\t\ts_szChu[0] = 0;",
        "\t\t\tif (!Ini.GetString(\"O\", m.szIdc, \"\", s_szChu, sizeof(s_szChu)) || !s_szChu[0])",
        "\t\t\t\tcontinue;",
        "\t\t\ts_szNhan[0] = 0;",
        "\t\t\tIni.GetString(\"Nhan\", m.szIdc, \"\", s_szNhan, sizeof(s_szNhan));",
        "\t\t\tif (s_szNhan[0])",
        "\t\t\t{",
        "\t\t\t\tchar szDau[144];",
        "\t\t\t\tsnprintf(szDau, sizeof(szDau), \"%s: \", s_szNhan);",
        "\t\t\t\tXuongDong(s_szChu, szDau);",
        "\t\t\t}",
        "\t\t\telse",
        "\t\t\t\tXuongDong(s_szChu, \"- \");",
        "\t\t}",
        "\t}",
        "}",
        "",
        "int KUiWAutoDsach::MoGhiChu(int nTab, const char* szTenThe)",
        "{",
        "\tchar sz[96];",
        "\tif (!s_pSelf)",
        "\t\treturn 0;",
        "\tsnprintf(sz, sizeof(sz), \"@@Hướng dẫn: @@%s\", szTenThe ? szTenThe : \"\");",
        "\ts_pSelf->m_TieuDe.SetText(sz);",
        "\ts_pSelf->NapGhiChu(nTab);",
        "\ts_pSelf->Mo(3);",
        "\treturn 1;",
        "}",
        "",
        "int KUiWAutoDsach::MoSua(int nViec)",
    ], "UiWAutoDsach.cpp: che do ghi chu")

    # Mo(): them nhanh che do 3
    s = thay(s, [
        "\telse",
        "\t{",
        "\t\tm_Huong.SetText(\"@@Chạm một dòng để chọn.@@\");",
        "\t}",
    ], [
        "\telse if (nCheDo == 2)",
        "\t{",
        "\t\tm_Huong.SetText(\"@@Chạm một dòng để chọn.@@\");",
        "\t}",
        "\telse",
        "\t{\t// %s che do 3: chi doc, cuon bang hai nut ben phai" % DAU,
        "\t\tm_Huong.SetText(\"@@Cuộn bằng hai nút bên phải. Bấm Đóng để quay lại.@@\");",
        "\t}",
    ], "UiWAutoDsach.cpp: Mo che do 3")
    return s


# ---------------------------------------------------------------- 3. UiWAuto
def va_uiwauto_h(s):
    return thay(s, [
        "\tKWndPureTextBtn\tm_Dong;",
    ], [
        "\tKWndPureTextBtn\tm_Dong;",
        "\tKWndPureTextBtn\tm_TroGiup;\t\t// %s nut [?]: mo huong dan cua the dang xem (uiwauto_ghichu.ini)" % DAU,
    ], "UiWAuto.h: nut tro giup")


def va_uiwauto_cpp(s):
    s = thay(s, [
        "\tAddChild(&m_Dong);",
        "\tm_BangPhu.KhoiTao(&m_Trang);",
    ], [
        "\tAddChild(&m_Dong);",
        "\tAddChild(&m_TroGiup);\t\t\t// %s nut [?]" % DAU,
        "\tm_BangPhu.KhoiTao(&m_Trang);",
    ], "UiWAuto.cpp: AddChild tro giup")
    s = thay(s, [
        "\tm_Dong.SetText(\"@@Đóng@@\");",
    ], [
        "\tm_Dong.SetText(\"@@Đóng@@\");",
        "\tm_TroGiup.SetText(\"?\");",
    ], "UiWAuto.cpp: chu nut tro giup")
    s = thay(s, [
        "\tm_Dong.Init(&Ini, \"Dong\");",
    ], [
        "\tm_Dong.Init(&Ini, \"Dong\");",
        "\tm_TroGiup.Init(&Ini, \"TroGiup\");",
    ], "UiWAuto.cpp: Init nut tro giup")
    s = thay(s, [
        "\t\tif (uParam == (KUPARAM)(KWndWindow*)&m_Dong)",
    ], [
        "\t\tif (uParam == (KUPARAM)(KWndWindow*)&m_TroGiup)",
        "\t\t{\t// %s huong dan cua the dang xem (trang note + ghi chu tung o cua WAuto.exe)" % DAU,
        "\t\t\tint nId = s_aNhomTabId[m_nNhom][m_nTab];",
        "\t\t\tif (nId >= 0)",
        "\t\t\t\tKUiWAutoDsach::MoGhiChu(nId, s_aNhom[m_nNhom].aTen[m_nTab]);",
        "\t\t\treturn 1;",
        "\t\t}",
        "\t\tif (uParam == (KUPARAM)(KWndWindow*)&m_Dong)",
    ], "UiWAuto.cpp: bam nut tro giup")
    # nut [?] cung phai tam an khi menu chon dang mo
    s = thay(s, [
        "\tap[n++] = &m_Dong;",
    ], [
        "\tap[n++] = &m_Dong;",
        "\tap[n++] = &m_TroGiup;",
    ], "UiWAuto.cpp: an nut tro giup duoi menu")
    return s


def va_ini(s):
    return thay(s, [
        "[Dong]",
    ], [
        "[TroGiup]",
        "Left=597",
        "Top=5",
        "Width=32",
        "Height=24",
        "Font=12",
        "Color=255,236,170",
        "OverColor=255,255,160",
        "SelColor=255,255,0",
        "BorderColor=0,0,0",
        "CentreAlign=1",
        "",
        "[Dong]",
    ], "uiwauto.ini: muc TroGiup")


def main():
    va("Sources/S3Client/Ui/UiCase/UiWAutoDsach.h", "MoGhiChu", va_dsach_h)
    va("Sources/S3Client/Ui/UiCase/UiWAutoDsach.cpp", "NapGhiChu", va_dsach_cpp)
    va("Sources/S3Client/Ui/UiCase/UiWAuto.h", "m_TroGiup", va_uiwauto_h)
    va("Sources/S3Client/Ui/UiCase/UiWAuto.cpp", "m_TroGiup", va_uiwauto_cpp)
    va("android/du_lieu_ghi_de/ui/ui3/uiwauto.ini", "[TroGiup]", va_ini)
    print("xong wauto13")


if __name__ == "__main__":
    main()
