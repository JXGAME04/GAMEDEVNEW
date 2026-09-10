# -*- coding: utf-8 -*-
#
# [TASKTRACE 12/09] Khung "Theo doi nhiem vu" tren mobile nam sat mep phai o 40 % chieu cao (de len cot icon phai + o phim 1-4):
# KUiTaskTrace::SnapToButton() chay luc KUiPlayerBar chua co (tu dang nhap vao thang game) -> nhanh else (mep phai), va
# m_oFixPos giu luon vi tri do. Sua (chi Android): khi thanh PlayerBar dang ky xong thi neo lai khung theo nut theo doi.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[TASKTRACE 12/09]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (s.count(c), ten))
    return s.replace(c, nl.join(moi))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


H = "Sources/S3Client/Ui/UiCase/UiTaskTrace.h"
s = doc(H)
if DAU not in s:
    s = thay(s, ["\tstatic void\t\t\t\tOnTaskValueChanged(int nTaskId);"],
                ["\tstatic void\t\t\t\tOnTaskValueChanged(int nTaskId);",
                 "#ifdef JX_ANDROID",
                 "\tstatic void\t\t\t\tNeoLaiKhiCoThanh();\t// %s PlayerBar dang ky xong -> neo lai canh nut theo doi" % DAU,
                 "#endif"], "khai bao")
    ghi(H, s); print("da va:", H)

C = "Sources/S3Client/Ui/UiCase/UiTaskTrace.cpp"
s = doc(C)
if DAU not in s:
    s = thay(s, ["void KUiTaskTrace::CloseWindow(bool bDestroy)"],
                ["#ifdef JX_ANDROID",
                 "// %s tu dang nhap vao thang game: khung mo TRUOC khi KUiPlayerBar co -> SnapToButton roi vao nhanh" % DAU,
                 "// 'mep phai, 40 % chieu cao' va m_oFixPos giu luon cho do (de len cot icon phai). PlayerBar goi ham nay sau Wnd_AddWindow.",
                 "void KUiTaskTrace::NeoLaiKhiCoThanh()",
                 "{",
                 "\tif (m_pSelf)",
                 "\t\tm_pSelf->SnapToButton();",
                 "}",
                 "#endif",
                 "",
                 "void KUiTaskTrace::CloseWindow(bool bDestroy)"], "ham")
    ghi(C, s); print("da va:", C)

P = "Sources/S3Client/Ui/UiCase/UiPlayerBar.cpp"
s = doc(P)
if DAU not in s:
    s0 = s
    s = thay(s, ["\tm_pSelf->LoadScheme(Scheme);",
                 "",
                 "\tWnd_AddWindow(this);",
                 "}"],
                ["\tm_pSelf->LoadScheme(Scheme);",
                 "",
                 "\tWnd_AddWindow(this);",
                 "#ifdef JX_ANDROID",
                 "\tKUiTaskTrace::NeoLaiKhiCoThanh();\t// %s khung theo doi nhiem vu mo truoc thanh nay -> neo lai canh nut" % DAU,
                 "#endif",
                 "}"], "goi")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi")
    ghi(P, s); print("da va:", P)
print("xong")
