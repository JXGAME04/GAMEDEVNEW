# -*- coding: utf-8 -*-
r"""nan_wauto_dlu_cpp.py - NAN TOA DO DIALOG-UNIT nam trong WAuto.cpp.

Di kem nan_wauto_dlu.py (nan WAuto.rc). PHAI CHAY CA HAI CUNG MOT LUC: .rc giu toa do
tinh, con .cpp dat lai vi tri luc chay (MoveDlgItem) - nan mot ben thoi la lech ngay.

Ty le bu khi doi font hop thoai Segoe UI 9 -> Microsoft Sans Serif 8:
    ngang x 7/6      doc x 15/13
(vi base unit 7 x 15 px -> 6 x 13 px; nan de so PIXEL giu nguyen)

KHONG can nan:
  - MoveDlgItem() ban than no (dung MapDialogRect - tu dung theo font moi)
  - AdjustComboDropHeight() (dung CB_GETITEMHEIGHT - la PIXEL that, tu theo font)
  - moi con so trong WM_DRAWITEM (dis->rcItem Windows dua sang la PIXEL)

Chay: python nan_wauto_dlu_cpp.py [--thu]
"""
import io
import os
import re
import shutil
import sys

CPP = r"E:\Src_Auto_Ngoai\WAuto\WAuto\WAuto.cpp"
MIRROR = r"D:\GAMEDEVNEW\WAutoUI\WAuto.cpp"
THU = "--thu" in sys.argv

TX = 7.0 / 6.0
TY = 15.0 / 13.0


def r(v, ty):
    return int(round(v * ty))


def nan4(x, y, w, h):
    """Nan BIEN (khong nan rieng x va w) de hai o lien ke van dinh nhau sau lam tron."""
    x2 = r(x, TX)
    w2 = r(x + w, TX) - x2
    y2 = r(y, TY)
    h2 = r(y + h, TY) - y2
    return x2, y2, w2, h2


# MoveDlgItem(hDlg, ID, x, y, w, h) voi CA BON tham so la so tho
MAU_MDI = re.compile(r'^(\s*MoveDlgItem\s*\(\s*hDlg\s*,\s*[A-Za-z_0-9\[\]]+\s*,\s*)'
                     r'(-?\d+)(\s*,\s*)(-?\d+)(\s*,\s*)(-?\d+)(\s*,\s*)(-?\d+)(\s*\)\s*;.*)$')

# cac dong co BIEU THUC (nDay / WA_DICH_Y) - thay tay, chi doi HANG SO
DAC_BIET = [
    ("MoveDlgItem(hDlg, nGrpId[nTabBtn], 2, 112 + WA_DICH_Y, 156, nDay - 111);",
     "MoveDlgItem(hDlg, nGrpId[nTabBtn], 2, 129 + WA_DICH_Y, 182, nDay - 128);"),
    ("MoveDlgItem(hDlg, IDC_STRING_HD_L, 4, nDay + 3 + WA_DICH_Y, 14, 10);",
     "MoveDlgItem(hDlg, IDC_STRING_HD_L, 5, nDay + 3 + WA_DICH_Y, 16, 12);"),
    ("MoveDlgItem(hDlg, IDC_STRING_HD_V, 20, nDay + 3 + WA_DICH_Y, 136, 10);",
     "MoveDlgItem(hDlg, IDC_STRING_HD_V, 23, nDay + 3 + WA_DICH_Y, 159, 12);"),
    ("MoveDlgItem(hDlg, IDC_BTN_BOT_ON,     4, nDay + 16 + WA_DICH_Y, 36, 12);",
     "MoveDlgItem(hDlg, IDC_BTN_BOT_ON,     5, nDay + 18 + WA_DICH_Y, 42, 14);"),
    ("MoveDlgItem(hDlg, IDC_BTN_BOT_OFF,   42, nDay + 16 + WA_DICH_Y, 36, 12);",
     "MoveDlgItem(hDlg, IDC_BTN_BOT_OFF,   49, nDay + 18 + WA_DICH_Y, 42, 14);"),
    ("MoveDlgItem(hDlg, IDC_BTN_BOT_SYNC,  80, nDay + 16 + WA_DICH_Y, 36, 12);",
     "MoveDlgItem(hDlg, IDC_BTN_BOT_SYNC,  93, nDay + 18 + WA_DICH_Y, 42, 14);"),
    ("MoveDlgItem(hDlg, IDC_BTN_BOT_POS,  118, nDay + 16 + WA_DICH_Y, 38, 12);",
     "MoveDlgItem(hDlg, IDC_BTN_BOT_POS,  138, nDay + 18 + WA_DICH_Y, 44, 14);"),
    # be ngang hop thoai dung de tinh lai kich thuoc cua so
    ("RECT rcU = { 0, 0, 160, nDay + 30 + WA_DICH_Y };",
     "RECT rcU = { 0, 0, 187, nDay + 35 + WA_DICH_Y };"),
    # do dich khoi noi dung (dlu DOC)
    ("#define WA_DICH_Y\t34", "#define WA_DICH_Y\t39"),
]


def main():
    s = io.open(CPP, encoding="utf-16", newline="").read()
    nl = "\r\n"
    if "#define WA_DICH_Y\t39" in s:
        print("!! WAuto.cpp DA nan roi (WA_DICH_Y = 39) - dung chay lai.")
        sys.exit(1)

    # ---- 1. cac dong dac biet ----
    nDB = 0
    for cu, moi in DAC_BIET:
        if s.count(cu) == 1:
            s = s.replace(cu, moi, 1)
            nDB += 1
        elif moi in s:
            print("  da co:", moi[:60])
        else:
            print("  !! KHONG THAY (%d lan): %s" % (s.count(cu), cu[:70]))

    # ---- 2. MoveDlgItem so tho ----
    L = s.split(nl)
    nMDI = 0
    for i, l in enumerate(L):
        m = MAU_MDI.match(l)
        if not m:
            continue
        x, y, w, h = (int(m.group(k)) for k in (2, 4, 6, 8))
        x2, y2, w2, h2 = nan4(x, y, w, h)
        L[i] = "%s%d%s%d%s%d%s%d%s" % (m.group(1), x2, m.group(3), y2,
                                       m.group(5), w2, m.group(7), h2, m.group(9))
        nMDI += 1
    s = nl.join(L)

    # ---- 3. bang chieu cao noi dung tung tab (dlu DOC) ----
    m = re.search(r'(static const short s_aTabDay\[WA_SO_TAB\] = \{\s*\r?\n\s*)([0-9,\s]+?)(\s*//)', s)
    if not m:
        m = re.search(r'(static const short s_aTabDay\[WA_SO_TAB\] = \{\s*\r?\n\s*)([0-9,\s]+?)(\s*\r?\n\})', s)
    if m:
        so = [int(t) for t in re.findall(r"\d+", m.group(2))]
        moi = [r(v, TY) for v in so]
        s = s[:m.start(2)] + ", ".join(str(v) for v in moi) + s[m.end(2):]
        print("  s_aTabDay: %s" % " ".join("%d->%d" % (a, b) for a, b in zip(so, moi)))
    else:
        print("  !! KHONG doc duoc s_aTabDay")

    print("Da nan: %d dong dac biet, %d loi goi MoveDlgItem so tho" % (nDB, nMDI))
    if THU:
        print("(--thu: khong ghi)")
        return
    if not os.path.exists(CPP + ".truoc_nan"):
        shutil.copyfile(CPP, CPP + ".truoc_nan")
    io.open(CPP, "w", encoding="utf-16", newline="").write(s)
    if os.path.isdir(os.path.dirname(MIRROR)):
        shutil.copyfile(CPP, MIRROR)
    print("da ghi WAuto.cpp (+ mirror WAutoUI)")


if __name__ == "__main__":
    main()
