# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 HUD b] Chu xem HUD va 77: "khung ve con rat cao" (16,5 ms) - do la NHIP giua hai lan ve (60 Hz = 16,7 ms),
# khong phai chi phi ve. Va 78: S3Client.cpp do that chi phi Breathe+UiHeartBeat (logic) va UiPaint (ve) moi vong, chi
# JX_ANDROID, luon bat (2 QueryPerformanceCounter/vong, khong dang ke) -> g_uJxHudLogicUs / g_uJxHudVeUs cho
# JxPerfHudAndroid.cpp hien "ve x,x ms | logic x,x ms | nhip 16,6 ms (te nhat N)". Windows khong doi.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 HUD b]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten, so=1):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) == so:
        return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho (%d): %s" % (so, s.count(c), ten))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def va(p, ham):
    s = doc(p)
    if DAU in s:
        print("da va roi, bo qua:", p)
        return
    s2 = ham(s)
    if cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


def va_s3client(s):
    # bien toan cuc, dat sau dong khai bao g_nPaintFps
    s = thay(s, ["static int\tg_nPaintFps = 30;\t\t// paint frames per second, config.ini [Client] PaintFps; 0 = paint locked to logic tick (legacy)"],
             ["static int\tg_nPaintFps = 30;\t\t// paint frames per second, config.ini [Client] PaintFps; 0 = paint locked to logic tick (legacy)",
              "#ifdef JX_ANDROID",
              "unsigned g_uJxHudLogicUs = 0, g_uJxHudVeUs = 0;\t// %s chi phi Breathe+UiHeartBeat / UiPaint vong gan nhat (us) cho bang do (JxPerfHudAndroid.cpp)" % DAU,
              "static inline unsigned JxHudUs(const LARGE_INTEGER& a) { LARGE_INTEGER b, f; QueryPerformanceCounter(&b); QueryPerformanceFrequency(&f); return f.QuadPart ? (unsigned)((b.QuadPart - a.QuadPart) * 1000000 / f.QuadPart) : 0; }",
              "#endif"], "S3Client.cpp: khai bao g_nPaintFps")
    # logic: Breathe + UiHeartBeat
    s = thay(s, ["\t\tDWORD\tdwLgT0 = g_nPaintLog > 0 ? timeGetTime() : 0;",
                 "\t\tBOOL\tbLgBre = g_pCoreShell->Breathe();"],
             ["\t\tDWORD\tdwLgT0 = g_nPaintLog > 0 ? timeGetTime() : 0;",
              "#ifdef JX_ANDROID",
              "\t\tLARGE_INTEGER liHudLg; QueryPerformanceCounter(&liHudLg);\t// %s" % DAU,
              "#endif",
              "\t\tBOOL\tbLgBre = g_pCoreShell->Breathe();"], "S3Client.cpp: truoc Breathe")
    s = thay(s, ["\t\tBOOL\tbLgUi  = bLgBre ? UiHeartBeat() : FALSE;",
                 "\t\tDWORD\tdwLgT2 = g_nPaintLog > 0 ? timeGetTime() : 0;"],
             ["\t\tBOOL\tbLgUi  = bLgBre ? UiHeartBeat() : FALSE;",
              "\t\tDWORD\tdwLgT2 = g_nPaintLog > 0 ? timeGetTime() : 0;",
              "#ifdef JX_ANDROID",
              "\t\tg_uJxHudLogicUs = JxHudUs(liHudLg);\t// %s" % DAU,
              "#endif"], "S3Client.cpp: sau UiHeartBeat")
    # ve: hai cho goi UiPaint
    s = thay(s, ["\t\t\t\tUiPaint(nGameFps);//nhe hon"],
             ["#ifdef JX_ANDROID",
              "\t\t\t\t{ LARGE_INTEGER liHudVe; QueryPerformanceCounter(&liHudVe); UiPaint(nGameFps); g_uJxHudVeUs = JxHudUs(liHudVe); }\t// %s" % DAU,
              "#else",
              "\t\t\t\tUiPaint(nGameFps);//nhe hon",
              "#endif"], "S3Client.cpp: UiPaint (PaintFps=0)")
    s = thay(s, ["\t\t\tUiPaint(nGameFps);", "\t\t\tbPainted = TRUE;"],
             ["#ifdef JX_ANDROID",
              "\t\t\t{ LARGE_INTEGER liHudVe; QueryPerformanceCounter(&liHudVe); UiPaint(nGameFps); g_uJxHudVeUs = JxHudUs(liHudVe); }\t// %s" % DAU,
              "#else",
              "\t\t\tUiPaint(nGameFps);",
              "#endif",
              "\t\t\tbPainted = TRUE;"], "S3Client.cpp: UiPaint (PaintFps>0)")
    return s


va("Sources/S3Client/S3Client.cpp", va_s3client)
print("xong")
