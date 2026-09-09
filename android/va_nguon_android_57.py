# -*- coding: utf-8 -*-
#
# [ANDROID 10/09 LOCTG] Sau khi rebase len main moi, ban Android KHONG LINK duoc:
#     ld: error: undefined symbol: g_uRep3LocKhung
#
# Nguon: commit "[LOCTG 09/09] Bo loc thoi gian luc trinh khung" tren main dinh nghia bo dem
#     unsigned g_uRep3LocKhung = 0;          (D3D9on11Dev.cpp:364)
# nhung dung no o KRepresentShell3.cpp:2742 (dong nhat ky [LOCTG] moi 10 giay) KHONG guard.
# D3D9on11Dev.cpp la lop ve D3D11 - chi co tren Windows; Android dung D3D9onGPUDev.cpp
# (android/lists/represent3.cmake loai han tep D3D9on11*). Nen tren Android ky hieu thieu.
#
# Sua theo dung nep san co cua chinh tep KRepresentShell3.cpp: cac bien [LOCTG]/[SANGDUNG]
# nhu g_nRep3LocMs, g_uRep3VeCoSang deu DINH NGHIA o KRepresentShell3.cpp (dong 55-56, tep
# nay bien dich o moi nen), lop ve chi extern. Doi cho dinh nghia va extern cua
# g_uRep3LocKhung cho giong vay. Ban PC khong doi hanh vi (cung mot bien, chi doi tep chua).

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
DAU = "[ANDROID 10/09 LOCTG]"

P = "Sources/Represent/Represent3/KRepresentShell3.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = "extern unsigned g_uRep3LocKhung;"
    assert s.count(CU) == 1, "khong tim thay extern g_uRep3LocKhung (%d)" % s.count(CU)
    MOI = ("unsigned g_uRep3LocKhung = 0;\t// [LOCTG 09/09] so khung da tron. "
           "[ANDROID 10/09 LOCTG] dinh nghia o DAY (nhu g_nRep3LocMs ben tren) chu khong o "
           "D3D9on11Dev.cpp - tep do chi co tren Windows, Android link thieu ky hieu.")
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

P = "Sources/Represent/Represent3/D3D9on11Dev.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = "unsigned g_uRep3LocKhung = 0;"
    assert s.count(CU) == 1, "khong tim thay dinh nghia g_uRep3LocKhung (%d)" % s.count(CU)
    MOI = ("extern unsigned g_uRep3LocKhung;\t// [ANDROID 10/09 LOCTG] dinh nghia da doi sang "
           "KRepresentShell3.cpp (bien dich o moi nen); tep nay chi co tren Windows")
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
