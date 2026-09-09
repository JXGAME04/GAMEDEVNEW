# -*- coding: utf-8 -*-
#
# [ANDROID 10/09 CUON] "lam kenh chat co the vuot len xuong xem tin tuc" (chu).
#
# Vuot doc tren giao dien da thanh CHAM_CUON (09/09) va gui WM_MOUSEWHEEL. Nhung WM_MOUSEWHEEL
# KHONG mang toa do: Wnd_ProcessInput (Wnds.cpp) lay vi tri CON TRO de tim cua so nhan. Con tro
# con nam o cham TRUOC (cho khac) -> lan chuot roi vao cua so khac -> khung chat khong cuon
# (thoai NPC cuon duoc chi vi cham truoc do la cham mo thoai, con tro da o do).
# Sua: vao CHAM_CUON thi dua con tro ve cho DAT NGON truoc (WM_MOUSEMOVE), them dong nhat ky.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
DAU = "[ANDROID 10/09 CUON]"
P = "Sources/S3Client/Platform/KSdlApp.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit
CU = NL.join([
    "\t\t\t\tm_nCham = CHAM_CUON;",
    "\t\t\t\tm_nCuonDon = 0;",
    "\t\t\t\treturn true;",
])
assert s.count(CU) == 1, "khong tim thay nhanh CHAM_CUON (%d)" % s.count(CU)
MOI = NL.join([
    "\t\t\t\tm_nCham = CHAM_CUON;",
    "\t\t\t\tm_nCuonDon = 0;",
    "\t\t\t\t// %s WM_MOUSEWHEEL khong mang toa do - Wnd_ProcessInput lay cho CON TRO," % DAU,
    "\t\t\t\t// ma con tro con o cham truoc (cho khac) -> lan chuot roi vao cua so khac, khung chat",
    "\t\t\t\t// khong cuon. Dua con tro ve cho dat ngon truoc.",
    "\t\t\t\tMsgProc(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(m_nChamX0, m_nChamY0));",
    "\t\t\t\tg_DebugLog(\"[CHAM] cuon tai %d,%d\", m_nChamX0, m_nChamY0);",
    "\t\t\t\treturn true;",
])
s = s.replace(CU, MOI)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("da va:", P)
