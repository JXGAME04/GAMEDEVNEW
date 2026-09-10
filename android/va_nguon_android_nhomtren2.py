# -*- coding: utf-8 -*-
#
# [NHOMTREN 12/09 b] Do cao cua nhom tren phai tinh theo VI TRI GOC TRONG INI cua khung 800x600, khong theo rieng khoang
# ma bo neo dich: tep bo cuc man rong co khung thiet ke 1371x617 nen da cong san 13 px cho khung (neo duoi) - man thap hon
# 617 (vd 1600x720 -> khung ve 1280x576) thi nhom tren lech 13 px. dy = Top hien tai - Top trong ini dung cho MOI tep.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[NHOMTREN 12/09 b]"


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


H = "Sources/S3Client/Ui/UiCase/UiPlayerBar.h"
s = doc(H)
if DAU not in s:
    s0 = s
    s = thay(s, ["\tvoid\t\t\t\t  NeoNhomTren();\t// [NHOMTREN 12/09] keo bang trang thai goc tren-trai len dinh man (man cao)"],
                ["\tvoid\t\t\t\t  NeoNhomTren();\t// [NHOMTREN 12/09] keo bang trang thai goc tren-trai len dinh man (man cao)",
                 "\tstatic int\t\t\t  ms_nTopIni;\t// %s Top cua [Main] trong UiPlayerBar.ini (khung 800x600)" % DAU], "khai bao ms_nTopIni")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (h)")
    ghi(H, s); print("da va:", H)

C = "Sources/S3Client/Ui/UiCase/UiPlayerBar.cpp"
s = doc(C)
if DAU not in s:
    s0 = s
    s = thay(s, ["\t_ASSERT(pIni);",
                 "\tint i = 0;",
                 "\tInit(pIni, $Main);"],
                ["\t_ASSERT(pIni);",
                 "\tint i = 0;",
                 "\tInit(pIni, $Main);",
                 "#ifdef JX_ANDROID",
                 "\tpIni->GetInteger($Main, \"Top\", 0, &ms_nTopIni);\t// %s do cao goc cua khung (truoc khi bo cuc neo lai)" % DAU,
                 "#endif"], "doc Top ini")
    s = thay(s, ["\tint nDichY = 0, nX = 0, nY = 0, i;",
                 "\tUiToaDo_LayDich(\"KUiPlayerBar|Main\", NULL, &nDichY);",
                 "\tif (nDichY == 0)",
                 "\t\treturn;"],
                ["\tint nDichY = 0, nX = 0, nY = 0, i;",
                 "\t// %s khoang khung da roi khoi cho goc trong ini (bo cuc neo + chenh khung thiet ke cua tep bo cuc)" % DAU,
                 "\tGetPosition(&nX, &nY);",
                 "\tnDichY = nY - ms_nTopIni;",
                 "\tif (nDichY == 0)",
                 "\t\treturn;"], "tinh dy")
    s = thay(s, ["void KUiPlayerBar::NeoNhomTren()"],
                ["int KUiPlayerBar::ms_nTopIni = 0;\t// %s" % DAU,
                 "void KUiPlayerBar::NeoNhomTren()"], "bien tinh")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (cpp)")
    ghi(C, s); print("da va:", C)
print("xong")
