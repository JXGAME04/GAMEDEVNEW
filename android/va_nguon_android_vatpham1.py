# -*- coding: utf-8 -*-
#
# [VATPHAM 12/09] Moc mo BANG THONG TIN VAT PHAM khi cham vao mot o vat pham (chi Android):
# - KWndObjectMatrix::WndProc / KWndObjectBox::WndProc, ngay dau WM_LBUTTONDOWN: thu mo KUiVatPham::Mo(...);
#   mo duoc thi THOI, khong nhac vat pham len tay nua. Bang tu tra ve false o cac ngan khong dung (o phim
#   tat, ky nang...) va khi dang cam vat pham tren tay -> luc do thao tac cu chay y nguyen.
# - Ban PC khong bien dich doan nay (#ifdef JX_ANDROID) nen khong doi gi.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[VATPHAM 12/09]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def thay(s, cu, moi, ten):
    c = "\r\n".join(cu)
    if s.count(c) != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (s.count(c), ten))
    return s.replace(c, "\r\n".join(moi))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


P = "Sources/S3Client/Ui/Elem/WndObjContainer.cpp"
s = doc(P)
if DAU not in s:
    s0 = s
    # 1) them include
    s = thay(s, ['#include "WndObjContainer.h"'],
                ['#include "WndObjContainer.h"',
                 '#ifdef JX_ANDROID',
                 '#include "../UiCase/UiVatPham.h"\t// %s bang thong tin vat pham + nut thao tac (chi mobile)' % DAU,
                 '#endif'], "include")

    # 2) KWndObjectMatrix: luoi o (hanh trang, ruong, cua hang, giao dich)
    s = thay(s, ["int KWndObjectMatrix::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)",
                 "{",
                 "\tswitch(uMsg)",
                 "\t{",
                 "\tcase WM_LBUTTONDOWN:"],
                ["int KWndObjectMatrix::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)",
                 "{",
                 "\tswitch(uMsg)",
                 "\t{",
                 "\tcase WM_LBUTTONDOWN:",
                 "#ifdef JX_ANDROID",
                 "\t\t{\t// %s cham vao o co vat pham -> mo bang thong tin + nut, khong nhac len tay" % DAU,
                 "\t\t\tint nO = GetObjectAt(LOWORD(nParam), HIWORD(nParam));",
                 "\t\t\tif (nO >= 0)",
                 "\t\t\t{",
                 "\t\t\t\tint nAbsX = 0, nAbsY = 0;",
                 "\t\t\t\tGetAbsolutePos(&nAbsX, &nAbsY);",
                 "\t\t\t\tif (KUiVatPham::Mo(&m_pObjects[nO], (UIOBJECT_CONTAINER)m_nContainerId,",
                 "\t\t\t\t\t\tnAbsX + LOWORD(nParam), nAbsY + HIWORD(nParam), m_pParentWnd))",
                 "\t\t\t\t\tbreak;",
                 "\t\t\t}",
                 "\t\t}",
                 "#endif"], "matrix")

    # 3) KWndObjectBox: mot o (o trang bi tren nguoi)
    s = thay(s, ["int KWndObjectBox::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)",
                 "{",
                 "\tswitch(uMsg)",
                 "\t{",
                 "\tcase WM_LBUTTONDOWN:"],
                ["int KWndObjectBox::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)",
                 "{",
                 "\tswitch(uMsg)",
                 "\t{",
                 "\tcase WM_LBUTTONDOWN:",
                 "#ifdef JX_ANDROID",
                 "\t\tif (m_Object.uGenre != CGOG_NOTHING)\t// %s" % DAU,
                 "\t\t{",
                 "\t\t\tKUiDraggedObject oVP = m_Object;",
                 "\t\t\tint nAbsX = 0, nAbsY = 0;",
                 "\t\t\tGetAbsolutePos(&nAbsX, &nAbsY);",
                 "\t\t\tif (KUiVatPham::Mo(&oVP, (UIOBJECT_CONTAINER)m_nContainerId,",
                 "\t\t\t\t\tnAbsX + m_Width / 2, nAbsY + m_Height / 2, m_pParentWnd))",
                 "\t\t\t\tbreak;",
                 "\t\t}",
                 "#endif"], "box")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi")
    ghi(P, s)
    print("da va:", P)

# 4) them tep vao ban dung Android
C = "android/CMakeLists.txt"
s = doc(C)
if "UiVatPham.cpp" not in s:
    s = thay(s, ["    ${JX_SRC}/S3Client/Ui/UiCase/UiWAutoTrang.cpp)     # [ANDROID 11/09 WAUTO B2] trang noi dung chay theo bang (UiWAutoBang.h sinh bang may)"],
                ["    ${JX_SRC}/S3Client/Ui/UiCase/UiWAutoTrang.cpp      # [ANDROID 11/09 WAUTO B2] trang noi dung chay theo bang (UiWAutoBang.h sinh bang may)",
                 "    ${JX_SRC}/S3Client/Ui/UiCase/UiVatPham.cpp)        # %s bang thong tin vat pham + nut (cham vao o vat pham)" % DAU], "cmake")
    ghi(C, s)
    print("da va:", C)
print("xong")
