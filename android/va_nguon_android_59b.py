# -*- coding: utf-8 -*-
# [UITOADO 10/09 G] sua nho sau 59: chuoi bao loi co "Ui\UiToaDo" -> clang doc "\U" la ma
# Unicode -> loi bien dich. Trong ma C phai la "Ui\\UiToaDo". (Bash tool nuot dau \ nen
# khong sua bang sed duoc - viet script rieng.)
import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")

P = "Sources/S3Client/Ui/Elem/UiToaDo.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
CU = 'c Ui\\UiToaDo_MacDinh.ini");'          # trong tep: ...c Ui\UiToaDo_MacDinh.ini");
MOI = 'c Ui\\\\UiToaDo_MacDinh.ini");'       # trong tep: ...c Ui\\UiToaDo_MacDinh.ini");
if s.count(CU) == 1:
    io.open(P, "w", encoding="latin-1", newline="").write(s.replace(CU, MOI))
    print("da sua:", P)
else:
    print("bo qua (%d):" % s.count(CU), P)

P = "android/va_nguon_android_59.py"
s = io.open(P, encoding="utf-8", newline="").read()
CU = 'Ui\\\\UiToaDo_MacDinh.ini"),'          # trong script: Ui\\UiToaDo_MacDinh.ini"),
MOI = 'Ui\\\\\\\\UiToaDo_MacDinh.ini"),'     # trong script: Ui\\\\UiToaDo_MacDinh.ini"),
if s.count(CU) == 1:
    io.open(P, "w", encoding="utf-8", newline="").write(s.replace(CU, MOI))
    print("da sua:", P)
else:
    print("bo qua (%d):" % s.count(CU), P)
