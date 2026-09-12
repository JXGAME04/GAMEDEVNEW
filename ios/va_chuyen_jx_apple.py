# -*- coding: utf-8 -*-
r"""[JXAPPLE 11/09] Chuyen rao "cua Apple noi chung" tu JX_IOS sang JX_APPLE.

macOS cung la Apple: cung thieu <sys/sysinfo.h>, cung dung Metal (khong nhan SPIR-V), cung co
/private/var va /Users, cung link tinh mot nhi phan. Nhung cho do dung chung duoc cho ca hai nen.

VI SAO AN TOAN: sau dot nay ios/CMakeLists.txt dinh nghia CA HAI (JX_APPLE va JX_IOS) nen ban iOS
khong the doi hanh vi. Android, Windows va may chu khong dinh nghia macro nao trong hai.
Kiem bang may: doi NGUOC JX_APPLE -> JX_IOS roi so tung byte voi ban trong git.

GIU NGUYEN JX_IOS o:
  KSdlApp.cpp  co SDL_WINDOW_HIGH_PIXEL_DENSITY - tren macOS cua so la cua so may tinh, giao dien
               ban PC 1024x768, bat mat do cao se doi khung ve; de rieng cho iOS.

Chay lai vo hai.  python3 ios/va_chuyen_jx_apple.py
"""
import io, os, re, sys
GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
bc = lambda t: sum(1 for c in t if ord(c) >= 0x80)
GIU = ("KSdlApp.cpp",)

TEP = []
for goc in ("Sources",):
    for r, _, tep in os.walk(os.path.join(GOC, goc)):
        for t in tep:
            if not t.lower().endswith((".cpp", ".h", ".c", ".inc")): continue
            if t in GIU: continue
            p = os.path.join(r, t)
            if "JX_IOS" in io.open(p, encoding="latin-1", newline="").read():
                TEP.append(p)
tong = 0
for p in sorted(TEP):
    s = io.open(p, encoding="latin-1", newline="").read(); n0 = bc(s)
    s2, n = re.subn(r"\bJX_IOS\b", "JX_APPLE", s)
    if bc(s2) != n0: sys.exit("LOI: %s byte cao doi" % p)
    io.open(p, "w", encoding="latin-1", newline="").write(s2)
    tong += n
    print("   %-52s %2d cho" % (os.path.relpath(p, GOC), n))

# ios/CMakeLists.txt phai dinh nghia THEM JX_APPLE
P = os.path.join(GOC, "ios", "CMakeLists.txt")
s = io.open(P, encoding="latin-1", newline="").read()
if "JX_APPLE" in s:
    print("   ios/CMakeLists.txt: da co JX_APPLE")
else:
    cu = "set(JX_COMMON_DEFS JX_PLATFORM_SDL JX_POSIX JX_MOBILE JX_IOS"
    moi = "set(JX_COMMON_DEFS JX_PLATFORM_SDL JX_POSIX JX_MOBILE JX_APPLE JX_IOS"
    assert s.count(cu) == 1
    io.open(P, "w", encoding="latin-1", newline="").write(s.replace(cu, moi))
    print("   ios/CMakeLists.txt: da them JX_APPLE (iOS dinh nghia CA HAI)")
print("=" * 66)
print("xong va_chuyen_jx_apple.py - %d cho / %d tep" % (tong, len(TEP)))
