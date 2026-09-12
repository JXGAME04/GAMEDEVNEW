# -*- coding: utf-8 -*-
r"""[IOS-MATDO 11/09] Bat mat do diem anh cao khi tao cua so (chi iOS).

Benh: chu bao "da vao duoc game nhung chua can chinh dung kich thuoc man hinh".
Nhat ky may that: "[GPU] man hinh 956x440@120" va "cua so 956x440 px".
Nhung iPhone 17 Pro Max co 2868x1320 diem anh that (956x440 la don vi DIEM, khong phai diem anh).

Goc: SDL_CreateWindow(..., 0) khong co co SDL_WINDOW_HIGH_PIXEL_DENSITY. Tren Android khong sao vi
cua so Android von tinh bang diem anh. Tren iOS/macOS, thieu co nay thi SDL lam viec o 1x:
  - khung ve tinh sai (ra 1124x516 thay vi bam theo ChieuCaoMucTieu=640),
  - anh bi he dieu hanh phong len 3 lan -> mo,
  - bo cuc icon lech vi khung vẽ khong dung ti le that.

Chua (rao JX_IOS): them SDL_WINDOW_HIGH_PIXEL_DENSITY. Android va Windows di nhanh #else,
giu nguyen van dong cu -> ios/kiem_rao.py so tung dong van bang nhau.

Chay lai vo hai.  python3 ios/va_nguon_ios_mat_do.py
"""
import io, os, sys
GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
P = os.path.join(GOC, "Sources", "S3Client", "Platform", "KSdlApp.cpp")
s = io.open(P, encoding="latin-1", newline="").read()
bc = lambda t: sum(1 for c in t if ord(c) >= 0x80)
n0 = bc(s)
CU = "\tm_pWindow = SDL_CreateWindow(m_szTitle, SCREEN_WIDTH, SCREEN_HEIGHT, 0);\n"
MOI = ("#ifdef JX_IOS\t// [IOS-MATDO 11/09] thieu co nay thi SDL chay o 1x (diem thay vi diem anh) -> khung ve sai, hinh mo\n"
       "\tm_pWindow = SDL_CreateWindow(m_szTitle, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_HIGH_PIXEL_DENSITY);\n"
       "#else\n" + CU + "#endif\n")
if "IOS-MATDO" in s:
    print("da va tu truoc")
else:
    if s.count(CU) != 1: sys.exit("LOI: khop %d lan" % s.count(CU))
    s = s.replace(CU, MOI)
    if bc(s) != n0: sys.exit("LOI: byte cao doi")
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va: bat SDL_WINDOW_HIGH_PIXEL_DENSITY cho iOS (byte cao %d khong doi)" % n0)
