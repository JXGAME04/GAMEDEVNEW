# -*- coding: utf-8 -*-
#
# [ANDROID 10/09 OTRONG] Chu: "mat mot so nut ky nang phu".
#
# Anh chup 13:13: cum chi con nut chinh + 2 o phu. Ly do: vong ve (JxKyNang_Ve) BO QUA o phu chua gan
# ky nang khi khong o che do sua ("o phu trong thi khong ve") - luc chu xem 11:50 la dang o che do
# sua nen thay du 8 o. Ban tham khao VNKU (anh mau HUD) luon ve o trong (vong "+" quanh nut chinh).
#
# Sua: (1) o phu trong van ve khung tron; (2) cham o phu TRONG (khong o che do gan) = vao che do gan
# voi dung o do + mo bang ky nang luon (SCK_SHORTCUT_SKILLSNEW, nhu nut "Vo cong") - dung mot cham
# thay vi ba; (3) hit-test cho o trong bat duoc cham ca khi khong o che do gan.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
DAU = "[ANDROID 10/09 OTRONG]"
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit

# 0. include ShortcutKey (mo bang ky nang)
CU = '#include "KDebug.h"' + NL
assert s.count(CU) == 1, "khong tim thay include KDebug (%d)" % s.count(CU)
s = s.replace(CU, CU + '#include "../Ui/ShortcutKey.h"\t// %s mo bang ky nang khi cham o trong' % DAU + NL)

# 1. vong ve: o trong van ve
CU = NL.join([
    "\t\tif (!bCo && i > 0 && !UiToaDo_DangSua())",
    "\t\t\tcontinue;\t\t// o phu trong thi khong ve; tru luc dang sua (de con keo duoc)",
])
assert s.count(CU) == 1, "khong tim thay dong bo qua o trong trong vong ve (%d)" % s.count(CU)
MOI = NL.join([
    "\t\t// %s o phu TRONG van ve khung tron (VNKU ve vong \"+\"), chu: \"mat mot so nut ky nang phu\"." % DAU,
    "\t\t// (Truoc: khong o che do sua thi bo qua -> tuong mat nut.)",
])
s = s.replace(CU, MOI)

# 2. hit-test: o trong bat duoc cham ca khi khong o che do gan
CU = NL.join([
    "\t\tif (!KyNang_CuaNut(i, &o) && !s_nKNCheDoGan)",
    "\t\t\tcontinue;",
])
assert s.count(CU) == 1, "khong tim thay dong bo qua o trong trong hit-test (%d)" % s.count(CU)
MOI = NL.join([
    "\t\t// %s o trong cung bat duoc cham (cham = vao che do gan cho dung o do)" % DAU,
    "\t\tKyNang_CuaNut(i, &o);",
])
s = s.replace(CU, MOI)

# 3. cham o trong (khong o che do gan) -> vao che do gan + mo bang ky nang
CU = NL.join([
    "\ts_nKNDangCam = nNut - 1;\t// 0 = nut chinh, 1..8 = o phu",
    "\ts_nKNNgonX = x;",
])
assert s.count(CU) == 1, "khong tim thay dau phan giu nut (%d)" % s.count(CU)
MOI = NL.join([
    "\t// %s cham o phu TRONG = chon o do de gan va mo bang ky nang luon (mot cham thay vi ba)." % DAU,
    "\tif (nNut > 1)",
    "\t{",
    "\t\tKUiGameObject oCo;",
    "",
    "\t\tif (!KyNang_CuaNut(nNut - 1, &oCo))",
    "\t\t{",
    "\t\t\ts_nKNCheDoGan = 1;",
    "\t\t\ts_nKNOChon = nNut - 2;",
    "\t\t\ts_nKNDangCam = -1;",
    "\t\t\tg_DebugLog(\"[KYNANG] cham o trong %d -> che do gan, mo bang ky nang\", nNut - 1);",
    "\t\t\tKShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_SKILLSNEW);",
    "\t\t\treturn;",
    "\t\t}",
    "\t}",
    "\ts_nKNDangCam = nNut - 1;\t// 0 = nut chinh, 1..8 = o phu",
    "\ts_nKNNgonX = x;",
])
s = s.replace(CU, MOI)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("da va:", P)
