# -*- coding: utf-8 -*-
r"""[JXMOBILE 11/09] Chuyen rao "vi la dien thoai" tu JX_ANDROID sang JX_MOBILE.

Chu yeu cau: "lay phan can chinh giao dien cua ban Android ap vao iOS". Day dung la viec ma
GHICHU_PHIEN_1209.md muc 1.1 da de nghi tu 12/09 nhung chua ai lam.

VI SAO AN TOAN TUYET DOI VOI ANDROID: sau dot nay android/CMakeLists.txt dinh nghia CA HAI macro
(JX_ANDROID va JX_MOBILE), nen moi rao doi ten van duoc thoa y het. Android khong the doi hanh vi.
Windows khong dinh nghia macro nao trong hai -> cung khong doi.
Rui ro nam HET ve phia iOS: iOS nay se di vao nhung nhanh truoc kia chi Android. Trinh bien dich
se chi ra cho nao dung API rieng cua Android.

CHI chuyen cac tep GIAO DIEN / NEN TANG cua S3Client. KHONG dong vao:
  - Core/Src/KSubWorldSet.cpp        (logic)
  - Represent/Represent3/*          (thu nghiem atlas / texture mang rieng cua Android)
  - Platform/JxAndroidStubs.cpp     (dung ten Android co chu y)

Chay lai vo hai.  python3 ios/va_chuyen_jx_mobile.py
"""
import io, os, re, sys
GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
bc = lambda t: sum(1 for c in t if ord(c) >= 0x80)

# Chuyen CA cay giao dien cua S3Client + 3 tep Engine lo duong dan du lieu.
# GIU NGUYEN JX_ANDROID o:
#   - JxAndroidStubs.cpp, JxPerfHudAndroid.cpp  (that su chi Android: /proc, stub rieng)
#   - Sources/Core/*                            (tinh chinh loi choi cho mobile - dot sau)
#   - Sources/Represent/Represent3/*            (thu nghiem atlas / texture mang rieng cua Android)
GIU = ("JxAndroidStubs.cpp", "JxPerfHudAndroid.cpp")
TEP = []
for goc in ("Sources/S3Client", "Sources/Engine/Src"):
    for r, _, tep in os.walk(os.path.join(GOC, goc)):
        for t in tep:
            if not t.lower().endswith((".cpp", ".h", ".inc")): continue
            if t in GIU: continue
            p = os.path.join(r, t)
            if goc.endswith("Engine/Src") and t not in ("KFilePath.cpp", "KFile.cpp", "KPakList.cpp"): continue
            if "JX_ANDROID" in io.open(p, encoding="latin-1", newline="").read():
                TEP.append(os.path.relpath(p, GOC))
TEP.sort()
print("so tep se chuyen:", len(TEP))

tong = 0
for t in TEP:
    p = os.path.join(GOC, t)
    if not os.path.isfile(p):
        print("   bo qua (khong co): " + t); continue
    s = io.open(p, encoding="latin-1", newline="").read(); n0 = bc(s)
    s2, n = re.subn(r"\bJX_ANDROID\b", "JX_MOBILE", s)
    if n == 0:
        print("   %-52s  -  khong con JX_ANDROID" % t); continue
    if bc(s2) != n0: sys.exit("LOI: %s byte >= 0x80 doi" % t)
    io.open(p, "w", encoding="latin-1", newline="").write(s2)
    tong += n
    print("   %-52s %3d cho" % (t, n))

# android/CMakeLists.txt phai dinh nghia THEM JX_MOBILE (Android van thoa moi rao nhu cu)
P = os.path.join(GOC, "android", "CMakeLists.txt")
s = io.open(P, encoding="latin-1", newline="").read()
if "JX_MOBILE" in s:
    print("   android/CMakeLists.txt: da co JX_MOBILE")
else:
    cu = "set(JX_COMMON_DEFS JX_PLATFORM_SDL JX_POSIX JX_ANDROID"
    moi = "set(JX_COMMON_DEFS JX_PLATFORM_SDL JX_POSIX JX_ANDROID JX_MOBILE"
    assert s.count(cu) == 1
    io.open(P, "w", encoding="latin-1", newline="").write(s.replace(cu, moi))
    print("   android/CMakeLists.txt: da them JX_MOBILE (Android van dinh nghia CA HAI)")
print("=" * 70)
print("xong va_chuyen_jx_mobile.py - %d cho" % tong)
