# -*- coding: utf-8 -*-
r"""[DONHIP 12/09 d] Ap ket qua do SDL 3.2.30 tren Fold 7 (BANGIAO_DONHIP_MOBILE_1209.md muc 9) - CHI Android:
  1. android/du_lieu_ghi_de/config.ini [Client]: PaintVsync=1, PaintSmooth=2 (nhip PC: p99 16-19 ms so voi 26-28, khong thua o buoc nao)
  2. Sources/Represent/Represent3/D3D9onGPUDev.cpp: Android mac dinh KHONG chep swapchain sang m_pLastFrame moi khung (fps bang, dien thap hon
     0,1-0,8 W); GetFrontBufferData (chup man hinh) chua co khung thi tu bat chep tu khung sau. Windows: #else giu nguyen = 1.
  3. Sources/S3Client/Platform/JxPerfHudAndroid.cpp: DnKetThuc tra ve "khong chep khung" (mac dinh Android moi).
Doc/ghi latin-1 (TCVN3, CRLF), moi moc khop dung 1 cho, so byte cao khong doi; chay lai nhieu lan khong sao.
Dung:  python android\va_nguon_android_donhip4.py
"""
import io
import os

GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[DONHIP 12/09 d]"


def doc(p):
    s = io.open(p, encoding="latin-1", newline="").read()
    nl = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
    return s, nl


def ghi(p, s, cao):
    if sum(1 for c in s if ord(c) >= 0x80) != cao:
        raise SystemExit("so byte cao doi - khong ghi: " + p)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("da va:", os.path.relpath(p, GOC))


def thay(s, nl, cu, moi, ten):
    c = nl.join(cu)
    n = s.count(c)
    if n != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (n, ten))
    return s.replace(c, nl.join(moi))


# 1. config.ini lop ghi de
p = os.path.join(GOC, "android", "du_lieu_ghi_de", "config.ini")
s, nl = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    cao = sum(1 for c in s if ord(c) >= 0x80)
    s = thay(s, nl,
        ["PaintFps=-1",
         "PaintInterp=1"],
        ["PaintFps=-1",
         "PaintInterp=1",
         "; " + DAU + " nhip PC cho mobile (do Fold 7 SDL 3.2.30, BANGIAO_DONHIP_MOBILE_1209.md muc 9): PaintVsync=1 ve theo vblank,",
         ";   PaintSmooth=2 lap lich nhu [NHIP a-e] cua PC -> p99 16-19 ms thay vi 26-28, it khung tre hon; khong thua o buoc do nao. 0/1 = nhip cu.",
         "PaintVsync=1",
         "PaintSmooth=2"], "PaintVsync/PaintSmooth")
    ghi(p, s, cao)

# 2. D3D9onGPUDev.cpp
p = os.path.join(GOC, "Sources", "Represent", "Represent3", "D3D9onGPUDev.cpp")
s, nl = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    cao = sum(1 for c in s if ord(c) >= 0x80)
    s = thay(s, nl,
        ["static int      s_nJxChepKhung = 1;\t\t// 0 = bo chep swapchain moi khung (ban sao chi de chup man hinh)"],
        ["#ifdef JX_ANDROID",
         "static int      s_nJxChepKhung = 0;\t\t// " + DAU + " Android: KHONG chep swapchain moi khung (Fold 7: fps bang, dien -0,1..-0,8 W); GetFrontBufferData tu bat",
         "#else",
         "static int      s_nJxChepKhung = 1;\t\t// 0 = bo chep swapchain moi khung (ban sao chi de chup man hinh)",
         "#endif"], "mac dinh chep khung")
    s = thay(s, nl,
        ["\tif (!m_pLastFrame) { RgLog(\"GetFrontBufferData: chua co khung nao (chup lai o khung sau)\"); return D3DERR_INVALIDCALL; }"],
        ["\tif (!m_pLastFrame)",
         "\t{",
         "#ifdef JX_ANDROID",
         "\t\tif (!s_nJxChepKhung) { s_nJxChepKhung = 1; RgLog(\"GetFrontBufferData: bat chep khung tu khung sau (Android mac dinh khong chep)\"); }\t// " + DAU,
         "#endif",
         "\t\tRgLog(\"GetFrontBufferData: chua co khung nao (chup lai o khung sau)\"); return D3DERR_INVALIDCALL;",
         "\t}"], "GetFrontBufferData")
    ghi(p, s, cao)

# 3. JxPerfHudAndroid.cpp: DnKetThuc
p = os.path.join(GOC, "Sources", "S3Client", "Platform", "JxPerfHudAndroid.cpp")
s, nl = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    cao = sum(1 for c in s if ord(c) >= 0x80)
    s = thay(s, nl,
        ["\tif (s_pfnDnDat) s_pfnDnDat(1, 2);",
         "\tif (s_nDnXin) DnXinHz(0);",
         "\ts_nDnXong = 1;",
         "\tDnGhi(\"[NHIP-XONG]\", \"het %d buoc - tra ve cau hinh luc mo app (PaintFps %d vsync %d smooth %d, chep khung, 2 khung bay, khong xin tan so)\","],
        ["\tif (s_pfnDnDat) s_pfnDnDat(0, 2);\t// " + DAU + " mac dinh Android: khong chep khung, 2 khung bay",
         "\tif (s_nDnXin) DnXinHz(0);",
         "\ts_nDnXong = 1;",
         "\tDnGhi(\"[NHIP-XONG]\", \"het %d buoc - tra ve cau hinh luc mo app (PaintFps %d vsync %d smooth %d, khong chep khung, 2 khung bay, khong xin tan so)\","],
        "DnKetThuc")
    ghi(p, s, cao)

print("xong")
