# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 WAUTO B1 c] Chu (11/09): "cac tab - ban phai nut cho dep, o kho co the hinh anh roi".
# Nut NHOM va nut TAB CON doi tu chu thuan (KWndPureTextBtn) sang KWndLabeledButton co ANH NEN lay tu kho VNKU:
# UiTong_Sheet0\btn_noname.spr (nut tron, dai xanh ngoc hop voi thanh vien khung) -> nut_nhom.spr 166x30 / nut_tab.spr 84x22
# (android/anh_wauto_vnku.py, khung 0 = toi = chua chon, khung 1 = sang = dang chon; CheckBox Up=0 Down=1). Chu van ve bang
# phong (SetLabel), mau trang / vang khi chon. Nut Dong giu chu thuan.
#  1. VA  Sources/S3Client/Ui/UiCase/UiWAuto.h  : KWndPureTextBtn m_Nhom[] / m_Tab[] -> KWndLabeledButton + include.
#  2. VA  Sources/S3Client/Ui/UiCase/UiWAuto.cpp: SetText -> SetLabel cho nhom / tab.
#  3. TAO LAI android/du_lieu_ghi_de/ui/ui3/uiwauto.ini (khoi [NhomN] / [TabN] co Image/Up/Down/CheckBox; phan con lai nhu wauto4).
# Chi tep Android-only; ban PC khong lien quan. Chay lai vo hai.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 WAUTO B1 c]"
DAU4 = "[ANDROID 11/09 WAUTO B1]"


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


def va(p, dau, ham):
    s = doc(p)
    if dau in s:
        print("da va roi, bo qua:", p)
        return
    s2 = ham(s)
    if cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


# ---------------------------------------------------------------- 1. UiWAuto.h
def va_h(s):
    s = thay(s, ["#include \"../Elem/WndPureTextBtn.h\""],
                ["#include \"../Elem/WndPureTextBtn.h\"",
                 "#include \"../Elem/WndLabeledButton.h\"\t// %s nut nhom / tab co anh nen kho VNKU" % DAU], "UiWAuto.h: include")
    s = thay(s, ["\tKWndPureTextBtn\tm_Nhom[WA_UI_SO_NHOM];",
                 "\tKWndPureTextBtn\tm_Tab[WA_UI_TAB_MOI_NHOM];"],
                ["\tKWndLabeledButton\tm_Nhom[WA_UI_SO_NHOM];\t\t// %s anh nut_nhom.spr, chu SetLabel" % DAU,
                 "\tKWndLabeledButton\tm_Tab[WA_UI_TAB_MOI_NHOM];\t// anh nut_tab.spr"], "UiWAuto.h: kieu nut")
    return s


# ---------------------------------------------------------------- 2. UiWAuto.cpp
def va_cpp(s):
    s = thay(s, ["\tfor (i = 0; i < WA_UI_SO_NHOM; i++)",
                 "\t\tm_Nhom[i].SetText(s_aTenNhom[i]);"],
                ["\tfor (i = 0; i < WA_UI_SO_NHOM; i++)",
                 "\t\tm_Nhom[i].SetLabel(s_aTenNhom[i]);\t// %s KWndLabeledButton" % DAU], "UiWAuto.cpp: nhan nhom")
    s = thay(s, ["\t\t\tm_Tab[i].SetText(s_aNhom[nNhom].aTen[i]);"],
                ["\t\t\tm_Tab[i].SetLabel(s_aNhom[nNhom].aTen[i]);"], "UiWAuto.cpp: nhan tab")
    s = thay(s, ["\t\t\tm_Tab[i].SetText(\"\");"],
                ["\t\t\tm_Tab[i].SetLabel(\"\");"], "UiWAuto.cpp: nhan tab rong")
    return s


# ---------------------------------------------------------------- 3. uiwauto.ini (tao lai)
KW, KH = 720, 432
RAIL = 19
INI = [
    "; %s Bo cuc KHUNG AUTO trong game (KUiWAuto, UiWAuto.cpp). Khung %dx%d (~70%% be ngang khung ve 1040x604), tu can giua." % (DAU4, KW, KH),
    "; %s nut nhom / tab con co anh nen (nut_nhom.spr 166x30, nut_tab.spr 84x22 tu btn_noname.spr VNKU), chu ve bang Label." % DAU,
    "; Chu cua nut dat trong ma (SetLabel, TCVN3). Anh: android/anh_wauto_vnku.py. Chu thich chi o dong rieng.",
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
NHOM_W = 166
NHOM_BUOC = (KW - 2 * RAIL - 4 * NHOM_W) // 3 + NHOM_W       # 4 nut cach deu tren be rong trong
for i in range(4):
    INI += ["[Nhom%d]" % i, "Left=%d" % (RAIL + i * NHOM_BUOC), "Top=34", "Width=%d" % NHOM_W, "Height=30", "Trans=0",
            r"Image=\spr\uinew\uiautonew\nut_nhom.spr", "Up=0", "Down=1", "CheckBox=1",
            "Font=14", "LabelYOffset=7", "Color=230,230,230", "OverColor=255,255,160", "SelectColor=255,255,0", "DisableColor=120,120,120", ""]
TAB_W = 84
TAB_BUOC = (KW - 2 * RAIL - 8 * TAB_W) // 7 + TAB_W          # 8 nut cach deu
for i in range(8):
    INI += ["[Tab%d]" % i, "Left=%d" % (RAIL + i * TAB_BUOC), "Top=70", "Width=%d" % TAB_W, "Height=22", "Trans=0",
            r"Image=\spr\uinew\uiautonew\nut_tab.spr", "Up=0", "Down=1", "CheckBox=1",
            "Font=12", "LabelYOffset=4", "Color=230,230,230", "OverColor=255,255,160", "SelectColor=255,255,0", "DisableColor=120,120,120", ""]
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


def tao_lai_ini(p):
    if os.path.isfile(p) and DAU in doc(p):
        print("da co roi, bo qua:", p)
        return
    ghi(p, "\r\n".join(INI) + "\r\n")
    print("da tao lai:", p, "(%d dong)" % len(INI))


va("Sources/S3Client/Ui/UiCase/UiWAuto.h", DAU, va_h)
va("Sources/S3Client/Ui/UiCase/UiWAuto.cpp", DAU, va_cpp)
tao_lai_ini("android/du_lieu_ghi_de/ui/ui3/uiwauto.ini")
print("xong")
