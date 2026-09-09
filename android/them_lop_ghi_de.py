# -*- coding: utf-8 -*-
# [ANDROID 10/09 GHIDE] chuan_bi_du_lieu.ps1 sinh lai D:\jx1_android_data tu cay client PC va KHONG co
# buoc nao chep cac tep chi-Android (anh VNKU cho nut ky nang, khung thanh duoi, bo cuc mac dinh
# UiToaDo, config.ini co [Resolution]/[Login] cua Android). Chay lai kich ban = mat het.
# Them lop ghi de android\du_lieu_ghi_de\ chep DE LEN sau cung (truoc buoc ha chu thuong).
import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
P = "android/chuan_bi_du_lieu.ps1"
DAU = "[ANDROID 10/09 GHIDE]"
s = io.open(P, encoding="utf-8", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit
NL = "\r\n" if "\r\n" in s else "\n"
CU = '"ha chu thuong ten tep/thu muc ..."'
assert s.count(CU) == 1, "khong tim thay dong ha chu thuong (%d)" % s.count(CU)
MOI = NL.join([
    "# %s Lop ghi de rieng cho Android (anh VNKU cho nut ky nang, khung thanh duoi, bo cuc mac dinh" % DAU,
    "# UiToaDo, config.ini co [Resolution]/[Login]): chep DE LEN sau cung. Them tep chi-Android thi bo vao day.",
    '$GhiDe = Join-Path $PSScriptRoot "du_lieu_ghi_de"',
    "if (Test-Path $GhiDe) {",
    '  "chep lop ghi de Android tu $GhiDe ..."',
    "  robocopy $GhiDe $Dich /E /R:1 /W:1 /NFL /NDL /NJH /NJS | Out-Null",
    '} else { "  (khong co $GhiDe - bo qua lop ghi de)" }',
    "",
    CU,
])
s = s.replace(CU, MOI)
io.open(P, "w", encoding="utf-8", newline="").write(s)
print("da va:", P)
