# -*- coding: utf-8 -*-
#
# [WAUTO 13/09] Nut "Dong" va "?" tren thanh tieu de khung WAuto cho dep (chu: "sua nut dong lai o Auto cho dep").
#
# Truoc: m_Dong / m_TroGiup la KWndPureTextBtn = CHU TRAN mau trang nam tro troi tren thanh da, nhin nhu chua lam xong.
# Sau:   KWndLabeledButton co anh -
#          nut_dong.spr 64x24 = nut Dong CO SAN cua bo UiAutoNew (CloseSeriesBtn.spr: ngoc + hoa van vang, chu ve san)
#          nut_hoi.spr  28x24 = cung chat lieu, khuc giua sach chu; chu "?" ve bang Label
#        khung 0 thuong / 1 dang bam (chu vang) / 2 tat (xam). Anh: android/anh_wauto_vnku.py nut_dong_hoi().
#
# UiWAuto.cpp/.h CHI nam trong android/CMakeLists.txt (khong vcxproj nao) -> ban PC khong doi.
# Chay lai vo hai.

import io
import os
import re
import sys

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))

DAU = "[WAUTO 13/09]"
H = "Sources/S3Client/Ui/UiCase/UiWAuto.h"
C = "Sources/S3Client/Ui/UiCase/UiWAuto.cpp"
INI = "android/du_lieu_ghi_de/ui/ui3/uiwauto.ini"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def va_h():
    s = doc(H)
    if "KWndLabeledButton\tm_Dong;" in s:
        print("   bo qua (da va):", H)
        return
    for cu, moi in (("KWndPureTextBtn\tm_Dong;",
                     "KWndLabeledButton\tm_Dong;\t\t// %s anh nut_dong.spr (nut Dong co san cua bo UiAutoNew)" % DAU),
                    ("KWndPureTextBtn\tm_TroGiup;",
                     "KWndLabeledButton\tm_TroGiup;")):
        if s.count(cu) != 1:
            raise SystemExit("UiWAuto.h: khong thay dung 1 cho (%d): %s" % (s.count(cu), cu))
        s = s.replace(cu, moi)
    ghi(H, s)
    print("   da va:", H)


def va_cpp():
    s = doc(C)
    if "m_TroGiup.SetLabel(\"?\");" in s:
        print("   bo qua (da va):", C)
        return
    m = re.findall(r'\tm_Dong\.SetText\("[^"]*"\);', s)
    if len(m) != 1:
        raise SystemExit("UiWAuto.cpp: m_Dong.SetText thay %d cho" % len(m))
    s = s.replace(m[0], "\t// %s m_Dong: chu \"Dong\" da ve san trong anh nut_dong.spr -> khong dat Label" % DAU)
    cu = '\tm_TroGiup.SetText("?");'
    if s.count(cu) != 1:
        raise SystemExit("UiWAuto.cpp: m_TroGiup.SetText thay %d cho" % s.count(cu))
    s = s.replace(cu, '\tm_TroGiup.SetLabel("?");')
    ghi(C, s)
    print("   da va:", C)


def muc_ini(ten, dong):
    return "[%s]\n%s\n\n" % (ten, "\n".join(dong))


def va_ini():
    s = doc(INI)
    nl = "\r\n" if "\r\n" in s else "\n"
    if "nut_dong.spr" in s:
        print("   bo qua (da va):", INI)
        return
    dong = muc_ini("Dong", [
        "; %s nut Dong co san cua bo UiAutoNew (CloseSeriesBtn.spr): 0 thuong / 1 bam / 2 tat; chu ve san trong anh" % DAU,
        "Left=636", "Top=5", "Width=64", "Height=24", "Trans=0",
        r"Image=\spr\uinew\uiautonew\nut_dong.spr", "Up=0", "Down=1", "DisableFrame=2",
    ])
    hoi = muc_ini("TroGiup", [
        "; %s cung chat lieu nut Dong, khuc giua sach chu; chu ? ve bang Label" % DAU,
        "Left=602", "Top=5", "Width=28", "Height=24", "Trans=0",
        r"Image=\spr\uinew\uiautonew\nut_hoi.spr", "Up=0", "Down=1", "DisableFrame=2",
        "Font=14", "LabelYOffset=5", "Color=255,236,170", "OverColor=255,255,160",
        "SelectColor=255,255,0", "DisableColor=140,140,140",
    ])
    for ten, moi in (("Dong", dong), ("TroGiup", hoi)):
        m = re.search(r"\[%s\][^\[]*" % ten, s)
        if not m:
            raise SystemExit("uiwauto.ini: khong co [%s]" % ten)
        s = s[:m.start()] + moi.replace("\n", nl) + s[m.end():]
    ghi(INI, s)
    print("   da va:", INI)


if __name__ == "__main__":
    va_h()
    va_cpp()
    va_ini()
    print("xong wauto33")
