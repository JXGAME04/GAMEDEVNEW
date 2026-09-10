# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 WAUTO B1] Buoc B1 cua LOTRINH_WAUTO_MOBILE_1109.md: KHUNG AUTO + ICON AUTO (chua co noi dung tab).
#
#  1. TAO  Sources/S3Client/Ui/UiCase/UiWAuto.h + .cpp  - lop KUiWAuto : KWndShowAnimate (khung anh nen kho VNKU), 4 nut NHOM +
#          8 nut TAB CON (KWndPureTextBtn, ten lay dung s_aNhomTab cua WAuto.exe bo Ac chinh / D.nhap), nut BAT/TAT (KWndButton
#          CheckBox, anh bat_tat_auto.spr) noi vao JxWAuto_Bat cua B0, dong trang thai (KWndText80 <- WA_HoatDong cua CoreShell).
#          Chi Android, KHONG nam trong vcxproj (them thang vao target main cua android/CMakeLists.txt).
#  2. TAO  android/du_lieu_ghi_de/ui/ui3/uiwauto.ini - bo cuc 720x432 (chu: "qua to" -> thu tu 980x588), 2 hang nut, BAT/TAT + trang thai o day.
#  3. VA   Sources/S3Client/Ui/UiShell.h / .cpp: lop nut Player_WAuto (ClassType cho thanh cong cu), dang ky class; rao JX_ANDROID.
#          Dong `// Player_AutoPlay::RegisterSelfClass();` GIU NGUYEN dang ghi chu (fkauto khong dung, khong xoa).
#  4. VA   Sources/Core/Src/CoreShell.cpp: JxCore_WAutoHoatDong() boc WA_HoatDong() (trong khoi #ifdef JX_ANDROID co san).
#  5. VA   android/CMakeLists.txt: them UiWAuto.cpp vao target main.
#  6. VA   android/du_lieu_ghi_de/ui/ui3/uitoolscontrolbar.ini: Button6=WAuto + muc [WAuto] (icon auto_m.spr o cot phai, duoi nut Chay).
#  Anh: android/anh_wauto_vnku.py (chay truoc). Chu Viet trong nguon: TCVN3 qua vn_edit.vn(); khong dung chu HOA co dau.
#
# LUAT (LOTRINH §0): ban PC khong doi - moi dong them vao tep dung chung deu trong #ifdef JX_ANDROID; nguon TCVN3 doc/ghi latin-1,
# giu CRLF, so byte cao truoc/sau bang nhau. Chay lai vo hai.

import io
import os
import re
import sys

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

DAU = "[ANDROID 11/09 WAUTO B1]"


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


def va(p, dau, ham, cho_phep_byte_cao=False):
    s = doc(p)
    if dau in s:
        print("da va roi, bo qua:", p)
        return
    s2 = ham(s)
    if not cho_phep_byte_cao and cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


def tao(p, dong, dau):
    if os.path.isfile(p) and dau in doc(p):
        print("da co roi, bo qua:", p)
        return
    os.makedirs(os.path.dirname(p), exist_ok=True)
    ghi(p, "\r\n".join(dong) + "\r\n")
    print("da tao:", p, "(%d dong)" % len(dong))


def C(s):
    """chuoi tieng Viet -> chuoi C (TCVN3) trong dau nhay kep"""
    return '"' + vn(s) + '"'


# ---------------------------------------------------------------- 1a. UiWAuto.h
H = [
    "//---------------------------------------------------------------------------",
    "// %s KHUNG AUTO trong game cho ban Android (buoc B1 cua LOTRINH_WAUTO_MOBILE_1109.md)." % DAU,
    "// Khung anh nen kho VNKU + dai nut NHOM / TAB CON (dung s_aNhomTab cua WAuto.exe, bo Ac chinh va D.nhap) + nut BAT/TAT",
    "// (JxWAuto_Bat cua B0) + dong trang thai (WA_HoatDong cua CoreShell qua JxCore_WAutoHoatDong). Noi dung tab: B2+.",
    "// Anh: android/anh_wauto_vnku.py (thu nho tu kho VNKU); bo cuc: ui\\ui3\\uiwauto.ini. Mo/dong bang icon Auto tren thanh",
    "// cong cu (Player_WAuto trong UiShell.cpp). Chi bien dich khi JX_ANDROID; khong nam trong vcxproj nao.",
    "//---------------------------------------------------------------------------",
    "#ifndef UiWAuto_H",
    "#define UiWAuto_H",
    "#ifdef JX_ANDROID",
    "#include \"../Elem/WndShowAnimate.h\"",
    "#include \"../Elem/WndButton.h\"",
    "#include \"../Elem/WndPureTextBtn.h\"",
    "#include \"../Elem/WndText.h\"",
    "",
    "#define WA_UI_SO_NHOM\t\t4",
    "#define WA_UI_TAB_MOI_NHOM\t8",
    "",
    "class KUiWAuto : protected KWndShowAnimate",
    "{",
    "public:",
    "\tstatic KUiWAuto*\tOpenWindow();",
    "\tstatic KUiWAuto*\tGetIfVisible();",
    "\tstatic void\t\t\tCloseWindow();\t\t// an di (giu doi tuong de mo lai nhanh)",
    "\tstatic void\t\t\tBatTatCuaSo();\t\t// icon Auto: dang hien thi an, dang an thi mo",
    "\tstatic void\t\t\tHuyCuaSo();\t\t\t// luc thoat game",
    "private:",
    "\tstatic KUiWAuto*\tm_pSelf;",
    "\tKUiWAuto();",
    "\tvoid\tInitialize();",
    "\tvoid\tLoadScheme(const char* pScheme);",
    "\tint\t\tWndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam);",
    "\tvoid\tBreathe();",
    "\tvoid\tChonNhom(int nNhom);",
    "\tvoid\tChonTab(int nTab);",
    "\tvoid\tCapNhatBatTat();",
    "\tvoid\tCapNhatTrangThai(int bEp);",
    "",
    "\tKWndPureTextBtn\tm_Nhom[WA_UI_SO_NHOM];",
    "\tKWndPureTextBtn\tm_Tab[WA_UI_TAB_MOI_NHOM];",
    "\tKWndPureTextBtn\tm_Dong;",
    "\tKWndButton\t\tm_BatTat;",
    "\tKWndText80\t\tm_TrangThai;",
    "\tKWndText80\t\tm_TenTab;\t\t// (B1) ten tab dang chon, giua vung noi dung con trong",
    "\tint\t\t\t\tm_nNhom;",
    "\tint\t\t\t\tm_nTab;",
    "\tint\t\t\t\tm_nBatCu;",
    "\tunsigned int\tm_uTrangThaiKe;",
    "};",
    "#endif // JX_ANDROID",
    "#endif",
]

# ---------------------------------------------------------------- 1b. UiWAuto.cpp
NHOM = [
    (["Chiến đấu", "Chiêu KH", "PK", "Di chuyển", "Nhặt đồ"]),
    (["Hậu cần", "Phục hồi", "Tổ đội"]),
    (["Cơ bản"]),
    (["Dã Tẩu", "Tống Kim", "CTC", "Liên đấu", "Sát thủ", "H.động"]),
]
TEN_NHOM = ["Điều khiển", "Hậu cần", "Cài đặt", "Hoạt động"]


def dong_nhom(ds):
    ten = [C(x) for x in ds] + ["NULL"] * (8 - len(ds))
    return "\t{ { %s }, %d }," % (", ".join(ten), len(ds))


CPP = [
    "//---------------------------------------------------------------------------",
    "// %s Khung Auto trong game - xem UiWAuto.h. Chu Viet trong chuoi: TCVN3 (sinh bang vn_edit.py" % DAU,
    "// trong android/va_nguon_android_wauto4.py), khong dung Edit/Write thuong.",
    "//---------------------------------------------------------------------------",
    "#include \"KWin32.h\"",
    "#ifdef JX_ANDROID",
    "#include \"KIniFile.h\"",
    "#include \"KDebug.h\"",
    "#include \"../elem/wnds.h\"",
    "#include \"../Elem/WndMessage.h\"",
    "#include \"../UiBase.h\"",
    "#include \"UiWAuto.h\"",
    "#include \"../../Platform/JxWAutoNoiBo.h\"",
    "#include <stdio.h>",
    "#include <string.h>",
    "",
    "extern int SCREEN_WIDTH;",
    "extern int SCREEN_HEIGHT;",
    "extern \"C\" int JxCore_WAutoHoatDong(char* szOut, int nMax);\t// CoreShell.cpp (chi Android): dong \"auto dang lam gi\" (TCVN3)",
    "",
    "#define SCHEME_INI\t\"UiWAuto.ini\"",
    "",
    "KUiWAuto* KUiWAuto::m_pSelf = NULL;",
    "",
    "// Ten nhom + tab con: dung thu tu s_aNhomTab cua WAutoUI/WAuto.cpp (04/09), bo Ac chinh va D.nhap (mobile mot may mot nhan vat).",
    "static const char* const s_aTenNhom[WA_UI_SO_NHOM] = { %s };" % ", ".join(C(x) for x in TEN_NHOM),
    "struct WAUiNhom { const char* aTen[WA_UI_TAB_MOI_NHOM]; int nSo; };",
    "static const WAUiNhom s_aNhom[WA_UI_SO_NHOM] = {",
] + [dong_nhom(x) for x in NHOM] + [
    "};",
    "",
    "KUiWAuto::KUiWAuto()",
    "{",
    "\tm_nNhom = 0;",
    "\tm_nTab = 0;",
    "\tm_nBatCu = -1;",
    "\tm_uTrangThaiKe = 0;",
    "}",
    "",
    "KUiWAuto* KUiWAuto::OpenWindow()",
    "{",
    "\tif (m_pSelf == NULL)",
    "\t{",
    "\t\tm_pSelf = new KUiWAuto;",
    "\t\tif (m_pSelf)",
    "\t\t\tm_pSelf->Initialize();",
    "\t}",
    "\tif (m_pSelf)",
    "\t{",
    "\t\tint nW = 0, nH = 0;",
    "\t\tm_pSelf->GetSize(&nW, &nH);",
    "\t\tm_pSelf->SetPosition((SCREEN_WIDTH - nW) / 2, (SCREEN_HEIGHT - nH) / 2);\t// giua khung ve (1040x604 hay 1188x616 deu vua)",
    "\t\tm_pSelf->m_nBatCu = -1;",
    "\t\tm_pSelf->CapNhatBatTat();",
    "\t\tm_pSelf->CapNhatTrangThai(1);",
    "\t\tm_pSelf->Show();",
    "\t}",
    "\treturn m_pSelf;",
    "}",
    "",
    "KUiWAuto* KUiWAuto::GetIfVisible()",
    "{",
    "\treturn (m_pSelf && m_pSelf->IsVisible()) ? m_pSelf : NULL;",
    "}",
    "",
    "void KUiWAuto::CloseWindow()",
    "{",
    "\tif (m_pSelf)",
    "\t\tm_pSelf->Hide();",
    "}",
    "",
    "void KUiWAuto::BatTatCuaSo()",
    "{",
    "\tif (GetIfVisible())",
    "\t\tCloseWindow();",
    "\telse",
    "\t\tOpenWindow();",
    "}",
    "",
    "void KUiWAuto::HuyCuaSo()",
    "{",
    "\tif (m_pSelf)",
    "\t{",
    "\t\tm_pSelf->Destroy();",
    "\t\tm_pSelf = NULL;",
    "\t}",
    "}",
    "",
    "void KUiWAuto::Initialize()",
    "{",
    "\tint i;",
    "\tfor (i = 0; i < WA_UI_SO_NHOM; i++)",
    "\t\tAddChild(&m_Nhom[i]);",
    "\tfor (i = 0; i < WA_UI_TAB_MOI_NHOM; i++)",
    "\t\tAddChild(&m_Tab[i]);",
    "\tAddChild(&m_BatTat);",
    "\tAddChild(&m_TrangThai);",
    "\tAddChild(&m_TenTab);",
    "\tAddChild(&m_Dong);",
    "\tchar Scheme[128];",
    "\tg_UiBase.GetCurSchemePath(Scheme, 128);",
    "\tLoadScheme(Scheme);",
    "\tm_Style &= ~WND_S_VISIBLE;",
    "\tWnd_AddWindow(this, WL_NORMAL);",
    "\tfor (i = 0; i < WA_UI_SO_NHOM; i++)",
    "\t\tm_Nhom[i].SetText(s_aTenNhom[i]);",
    "\tm_Dong.SetText(%s);" % C("Đóng"),
    "\tChonNhom(0);",
    "}",
    "",
    "void KUiWAuto::LoadScheme(const char* pScheme)",
    "{",
    "\tchar Buff[256];",
    "\tchar szMuc[32];",
    "\tKIniFile Ini;",
    "\tint i;",
    "\tsprintf(Buff, \"%s\\\\%s\", pScheme, SCHEME_INI);",
    "\tif (!Ini.Load(Buff))",
    "\t{",
    "\t\tg_DebugLog(\"[WAUTO-UI] khong doc duoc bo cuc %s\", Buff);",
    "\t\treturn;",
    "\t}",
    "\tInit(&Ini, \"Main\");",
    "\tfor (i = 0; i < WA_UI_SO_NHOM; i++)",
    "\t{",
    "\t\tsprintf(szMuc, \"Nhom%d\", i);",
    "\t\tm_Nhom[i].Init(&Ini, szMuc);",
    "\t}",
    "\tfor (i = 0; i < WA_UI_TAB_MOI_NHOM; i++)",
    "\t{",
    "\t\tsprintf(szMuc, \"Tab%d\", i);",
    "\t\tm_Tab[i].Init(&Ini, szMuc);",
    "\t}",
    "\tm_BatTat.Init(&Ini, \"BatTat\");",
    "\tm_TrangThai.Init(&Ini, \"TrangThai\");",
    "\tm_TenTab.Init(&Ini, \"TenTab\");",
    "\tm_Dong.Init(&Ini, \"Dong\");",
    "}",
    "",
    "void KUiWAuto::ChonNhom(int nNhom)",
    "{",
    "\tint i;",
    "\tif (nNhom < 0 || nNhom >= WA_UI_SO_NHOM)",
    "\t\treturn;",
    "\tm_nNhom = nNhom;",
    "\tfor (i = 0; i < WA_UI_SO_NHOM; i++)",
    "\t\tm_Nhom[i].CheckButton(i == nNhom);",
    "\tfor (i = 0; i < WA_UI_TAB_MOI_NHOM; i++)",
    "\t{",
    "\t\tif (i < s_aNhom[nNhom].nSo && s_aNhom[nNhom].aTen[i])",
    "\t\t{",
    "\t\t\tm_Tab[i].SetText(s_aNhom[nNhom].aTen[i]);",
    "\t\t\tm_Tab[i].Show();",
    "\t\t}",
    "\t\telse",
    "\t\t{",
    "\t\t\tm_Tab[i].SetText(\"\");",
    "\t\t\tm_Tab[i].Hide();",
    "\t\t}",
    "\t}",
    "\tChonTab(0);",
    "}",
    "",
    "void KUiWAuto::ChonTab(int nTab)",
    "{",
    "\tchar sz[80];",
    "\tif (nTab < 0 || nTab >= s_aNhom[m_nNhom].nSo)",
    "\t\treturn;",
    "\tm_nTab = nTab;",
    "\tfor (int i = 0; i < WA_UI_TAB_MOI_NHOM; i++)",
    "\t\tm_Tab[i].CheckButton(i == nTab);",
    "\t// (B1) chua co noi dung: ghi ten tab + loi nhac; B2 thay bang trang chay theo bang",
    "\tsnprintf(sz, sizeof(sz), \"%s  -  %s\", s_aNhom[m_nNhom].aTen[nTab], " + C("nội dung tab này có ở bước B2") + ");",
    "\tm_TenTab.SetText(sz);",
    "}",
    "",
    "void KUiWAuto::CapNhatBatTat()",
    "{",
    "\tint nBat = JxWAuto_DangBat() ? 1 : 0;",
    "\tif (nBat != m_nBatCu)",
    "\t{",
    "\t\tm_nBatCu = nBat;",
    "\t\tm_BatTat.CheckButton(nBat);\t\t// CheckBox: Up = \"Bat Auto\" (dang tat), Down = \"Tat Auto\" (dang bat)",
    "\t}",
    "}",
    "",
    "void KUiWAuto::CapNhatTrangThai(int bEp)",
    "{",
    "\tchar sz[80];",
    "\tchar szHD[64];",
    "\tunsigned int uNow = timeGetTime();",
    "\tif (!bEp && uNow < m_uTrangThaiKe)",
    "\t\treturn;",
    "\tm_uTrangThaiKe = uNow + 500;",
    "\tsz[0] = 0;",
    "\tszHD[0] = 0;",
    "\tif (JxWAuto_DangBat())",
    "\t{",
    "\t\tJxCore_WAutoHoatDong(szHD, sizeof(szHD));",
    "\t\tif (szHD[0])",
    "\t\t\tsnprintf(sz, sizeof(sz), \"%s: %s\", " + C("Đang bật") + ", szHD);",
    "\t\telse",
    "\t\t\tstrcpy(sz, " + C("Đang bật: đánh quái trong tầm, nhặt đồ, uống thuốc") + ");",
    "\t}",
    "\telse",
    "\t\tstrcpy(sz, " + C("Auto đang tắt. Chạm nút Bật Auto để tự đánh, nhặt đồ, uống thuốc.") + ");",
    "\tm_TrangThai.SetText(sz);",
    "}",
    "",
    "void KUiWAuto::Breathe()",
    "{",
    "\tCapNhatBatTat();",
    "\tCapNhatTrangThai(0);",
    "}",
    "",
    "int KUiWAuto::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)",
    "{",
    "\tif (uMsg == WND_N_BUTTON_CLICK)",
    "\t{",
    "\t\tint i;",
    "\t\tif (uParam == (KUPARAM)(KWndWindow*)&m_BatTat)",
    "\t\t{",
    "\t\t\tJxWAuto_Bat(!JxWAuto_DangBat());",
    "\t\t\tm_nBatCu = -1;",
    "\t\t\tCapNhatBatTat();",
    "\t\t\tCapNhatTrangThai(1);",
    "\t\t\tg_DebugLog(\"[WAUTO-UI] nut BAT/TAT -> %d\", JxWAuto_DangBat());",
    "\t\t\treturn 1;",
    "\t\t}",
    "\t\tif (uParam == (KUPARAM)(KWndWindow*)&m_Dong)",
    "\t\t{",
    "\t\t\tHide();",
    "\t\t\treturn 1;",
    "\t\t}",
    "\t\tfor (i = 0; i < WA_UI_SO_NHOM; i++)",
    "\t\t\tif (uParam == (KUPARAM)(KWndWindow*)&m_Nhom[i])",
    "\t\t\t{",
    "\t\t\t\tChonNhom(i);",
    "\t\t\t\treturn 1;",
    "\t\t\t}",
    "\t\tfor (i = 0; i < WA_UI_TAB_MOI_NHOM; i++)",
    "\t\t\tif (uParam == (KUPARAM)(KWndWindow*)&m_Tab[i])",
    "\t\t\t{",
    "\t\t\t\tChonTab(i);",
    "\t\t\t\treturn 1;",
    "\t\t\t}",
    "\t}",
    "\treturn KWndShowAnimate::WndProc(uMsg, uParam, nParam);",
    "}",
    "#endif // JX_ANDROID",
]

# ---------------------------------------------------------------- 2. uiwauto.ini
# Khung 720x432 = khung.spr 1313x788 x 0,548 (chu 11/09: "qua to, phai nho gon lai" - ban dau 980x588). Doi co: sua KW/KH o day
# + KHUNG_W/KHUNG_H trong anh_wauto_vnku.py, xoa ui/ui3/uiwauto.ini roi chay lai hai kich ban. Cua so tu can giua luc mo.
KW, KH = 720, 432
RAIL = 19                    # thanh xanh hai ben (35 px goc x 0,548)
INI = [
    "; %s Bo cuc KHUNG AUTO trong game (KUiWAuto, UiWAuto.cpp). Khung %dx%d (~70%% be ngang khung ve 1040x604), tu can giua." % (DAU, KW, KH),
    "; Chu cua nut nhom / tab / Dong dat trong ma (KWndPureTextBtn, TCVN3). Anh: android/anh_wauto_vnku.py. Chu thich chi o dong rieng.",
    "[Main]",
    "Left=160",
    "Top=86",
    "Width=%d" % KW,
    "Height=%d" % KH,
    "Moveable=1",
    "Trans=0",
    r"Image=\spr\uinew\uiautonew\khung_wauto.spr",
    "",
]
NHOM_W = (KW - 2 * RAIL - 3 * 6) // 4        # 4 nut nhom, cach nhau 6
for i in range(4):
    INI += ["[Nhom%d]" % i, "Left=%d" % (RAIL + i * (NHOM_W + 6)), "Top=36", "Width=%d" % NHOM_W, "Height=26", "Font=14",
            "Color=255,255,255", "OverColor=255,255,160", "SelColor=255,255,0", "BorderColor=0,0,0", "SelBorderColor=60,40,0",
            "CentreAlign=1", ""]
TAB_W = (KW - 2 * RAIL - 7 * 2) // 8         # 8 tab con, cach nhau 2
for i in range(8):
    INI += ["[Tab%d]" % i, "Left=%d" % (RAIL + i * (TAB_W + 2)), "Top=70", "Width=%d" % TAB_W, "Height=22", "Font=12",
            "Color=230,230,230", "OverColor=255,255,160", "SelColor=255,255,0", "BorderColor=0,0,0", "SelBorderColor=60,40,0",
            "CentreAlign=1", ""]
INI += [
    "[TenTab]",
    "Left=%d" % (RAIL + 10), "Top=200", "Width=%d" % (KW - 2 * RAIL - 20), "Height=24", "Font=14", "Color=200,200,200", "BorderColor=0,0,0", "HAlign=1", "",
    "[BatTat]",
    "Left=%d" % (RAIL + 5), "Top=%d" % (KH - 48), "Width=102", "Height=40", "Trans=0",
    r"Image=\spr\uinew\uiautonew\bat_tat_auto.spr",
    "Up=0", "Down=2", "CheckBox=1", "",
    "[TrangThai]",
    "Left=%d" % (RAIL + 115), "Top=%d" % (KH - 38), "Width=%d" % (KW - 2 * RAIL - 125), "Height=20", "Font=12", "Color=255,252,178", "BorderColor=0,0,0", "HAlign=0", "",
    "[Dong]",
    "Left=%d" % (KW - RAIL - 66), "Top=5", "Width=64", "Height=24", "Font=12", "Color=255,255,255", "OverColor=255,255,160", "SelColor=255,255,0",
    "BorderColor=0,0,0", "CentreAlign=1", "",
]


# ---------------------------------------------------------------- 3. UiShell.h / .cpp
def va_uishell_h(s):
    return thay(s, ["class Player_ItemEx : public KWndButton"],
                   ["#ifdef JX_ANDROID",
                    "// %s icon Auto tren thanh cong cu (uitoolscontrolbar.ini [WAuto] ClassType=Player_WAuto): mo / dong khung WAuto trong game;" % DAU,
                    "// CheckBox: sang khi may auto dang bat (JxWAuto_DangBat). fkauto (Player_AutoPlay) van de nguyen, khong dung.",
                    "class Player_WAuto : public KWndButton",
                    "{",
                    "public:",
                    "\tDECLARE_COMCLASS(Player_WAuto)",
                    "\tvoid OnButtonClick();",
                    "\tvoid UpdateData();",
                    "\tconst char*\tGetShortKey();",
                    "};",
                    "#endif",
                    "",
                    "class Player_ItemEx : public KWndButton"], "UiShell.h: Player_WAuto")


def va_uishell_cpp(s):
    s = thay(s, ["//\tPlayer_AutoPlay::RegisterSelfClass();",
                 "\tPlayer_ItemEx::RegisterSelfClass();"],
                ["//\tPlayer_AutoPlay::RegisterSelfClass();",
                 "#ifdef JX_ANDROID",
                 "\tPlayer_WAuto::RegisterSelfClass();\t// %s icon Auto (khung WAuto trong game)" % DAU,
                 "#endif",
                 "\tPlayer_ItemEx::RegisterSelfClass();"], "UiShell.cpp: RegisterSelfClass")
    s = thay(s, ["IMPLEMENT_COMCLASS(Player_ItemEx)"],
                ["#ifdef JX_ANDROID",
                 "// %s icon Auto tren thanh cong cu -> khung WAuto trong game (UiCase/UiWAuto.cpp)" % DAU,
                 "#include \"UiCase/UiWAuto.h\"",
                 "#include \"../Platform/JxWAutoNoiBo.h\"",
                 "IMPLEMENT_COMCLASS(Player_WAuto)",
                 "void Player_WAuto::OnButtonClick()",
                 "{",
                 "\tKUiWAuto::BatTatCuaSo();",
                 "}",
                 "",
                 "void Player_WAuto::UpdateData()",
                 "{",
                 "\tCheckButton(JxWAuto_DangBat() ? 1 : 0);",
                 "}",
                 "",
                 "const char* Player_WAuto::GetShortKey()",
                 "{",
                 "\treturn NULL;",
                 "}",
                 "#endif",
                 "",
                 "IMPLEMENT_COMCLASS(Player_ItemEx)"], "UiShell.cpp: Player_WAuto")
    return s


# ---------------------------------------------------------------- 4. CoreShell.cpp
def va_coreshell(s):
    return thay(s, ["\t\t*pnManaMax = Npc[nIdx].m_CurrentManaMax;",
                    "\treturn 1;",
                    "}",
                    "#endif\t// JX_ANDROID"],
                   ["\t\t*pnManaMax = Npc[nIdx].m_CurrentManaMax;",
                    "\treturn 1;",
                    "}",
                    "",
                    "// %s Dong \"auto dang lam gi\" (TCVN3, WA_HoatDong o tren - cung dong WAuto.exe hien o chan cua so) cho khung" % DAU,
                    "// WAuto trong game (S3Client/Ui/UiCase/UiWAuto.cpp). Tra 1 khi co chu. Chi Android.",
                    "extern \"C\" int JxCore_WAutoHoatDong(char* szOut, int nMax)",
                    "{",
                    "\tif (!szOut || nMax <= 0)",
                    "\t\treturn 0;",
                    "\tszOut[0] = 0;",
                    "\tif (Player[CLIENT_PLAYER_INDEX].m_nIndex <= 0)",
                    "\t\treturn 0;",
                    "\tWA_HoatDong(CLIENT_PLAYER_INDEX, szOut, nMax);",
                    "\treturn szOut[0] ? 1 : 0;",
                    "}",
                    "#endif\t// JX_ANDROID"], "CoreShell.cpp: JxCore_WAutoHoatDong")


# ---------------------------------------------------------------- 5. CMakeLists.txt
def va_cmake(s):
    nl = nl_cua(s)
    dong = [d for d in s.split(nl) if "JxWAutoNoiBo.cpp)" in d]
    if len(dong) != 1:
        raise SystemExit("CMakeLists.txt: khong thay dong JxWAutoNoiBo.cpp) (%d)" % len(dong))
    cu = dong[0]
    moi = cu.replace("JxWAutoNoiBo.cpp)", "JxWAutoNoiBo.cpp ") + nl + \
        "    ${JX_SRC}/S3Client/Ui/UiCase/UiWAuto.cpp)          # %s khung Auto trong game (icon + BAT/TAT + dai tab)" % DAU
    return s.replace(cu, moi)


# ---------------------------------------------------------------- 6. uitoolscontrolbar.ini (lop ghi de)
def va_toolbar(s):
    s = thay(s, ["Button5=Rec"], ["Button5=Rec", "Button6=WAuto\t; %s icon Auto (cot phai, duoi nut Chay) -> khung WAuto trong game" % DAU],
             "uitoolscontrolbar.ini: Button6")
    nl = nl_cua(s)
    if not s.endswith(nl):
        s += nl
    s += nl.join(["", "; %s icon Auto: 47x47 cat tu hinh kiem cheo cua nut \"Bat Auto\" kho VNKU (android/anh_wauto_vnku.py);" % DAU,
                  ";   khung 0 = xam (auto tat), khung 1 = vang (dang bat). Cham = mo / dong khung WAuto. Ben TRAI giua man hinh (cot phai duoi nut Chay bi bang Theo doi nhiem vu de len).",
                  "[WAuto]", "Left=52", "Top=330", "Width=47", "Height=47", "Trans=0",
                  "Image=\\Spr\\UiNew\\UiToolsControlBar\\auto_m.spr", "Up=0", "Down=1", "CheckBox=1", "ClassType=Player_WAuto", ""])
    return s


tao("Sources/S3Client/Ui/UiCase/UiWAuto.h", H, DAU)
tao("Sources/S3Client/Ui/UiCase/UiWAuto.cpp", CPP, DAU)
tao("android/du_lieu_ghi_de/ui/ui3/uiwauto.ini", INI, DAU)
va("Sources/S3Client/Ui/UiShell.h", DAU, va_uishell_h)
va("Sources/S3Client/Ui/UiShell.cpp", DAU, va_uishell_cpp)
va("Sources/Core/Src/CoreShell.cpp", DAU, va_coreshell)
va("android/CMakeLists.txt", DAU, va_cmake)
va("android/du_lieu_ghi_de/ui/ui3/uitoolscontrolbar.ini", DAU, va_toolbar)
print("xong")
