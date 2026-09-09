# -*- coding: utf-8 -*-
# [ANDROID 10/09 ANICON b] sua sau 61: bien tinh s_bIconTronAn dinh nghia o cuoi tep (truoc PaintWindow)
# nhung WndProc (o tren) da dung -> "undeclared identifier". Doi len dau tep, ngay sau include.
import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
P = "Sources/S3Client/Ui/UiCase/UiPlayerBar.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
NL = "\r\n"
DONG = "//\t[ANDROID 10/09 ANICON] Hang icon tron dang an hay hien (mot thanh, mot nut - de tinh cho gon)" + NL + \
       "static int s_bIconTronAn = 0;" + NL + NL
if s.count(DONG) != 1:
    print("khong thay dong can doi (%d) - co the da doi roi" % s.count(DONG))
    raise SystemExit
s = s.replace(DONG, "")
MOC = '#include "UiToolsControlBar.h"\t// [ANDROID 10/09 ANICON] tim nut Ngoi'
assert s.count(MOC) == 1, "khong tim thay include ANICON (%d)" % s.count(MOC)
s = s.replace(MOC, MOC + NL + "static int s_bIconTronAn = 0;\t// [ANDROID 10/09 ANICON] hang icon tron dang an (1) hay hien (0)")
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("da doi s_bIconTronAn len dau tep:", P)
