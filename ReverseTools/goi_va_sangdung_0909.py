# -*- coding: utf-8 -*-
"""goi_va_sangdung_0909.py - [SANGDUNG 09/09] Do DUT DIEM hai cau hoi, khong suy doan nua:

  1. He chieu sang co THUC SU duoc dung khi ve SPR khong?
     Doc ma thi SetOption(DYNAMICLIGHT) dat m_bDoLighting = false o CA hai nhanh (KRepresentShell3.cpp
     dong 551-554), va CoreShell.cpp:23014 co goi no => ve ly thuyet la TAT HAN. Nhung lan truoc toi
     ket luan tu suy doan roi sai, nen lan nay IN THANG gia tri m_bDoLighting va DEM so lan that su
     di vao nhanh ve-co-chieu-sang (DrawSpriteAlpha3DLighting).

  2. Dung bao nhieu CPU cho ban do sang bi VUT DI?
     KIpoTree::RenderLightMap() chay MOI KHUNG VE (luoi 48x96 o, ~40 nguon, moi nguon ~400 o kem sqrt).
     Neu (1) cho thay khong ai dung ket qua thi day la CPU lang phi thuan tuy - nghi lien quan
     'dang nhap cham' / lag chu bao. Do ms/khung roi in kem dong [SANGDO] da co.

CHI GHI SO, khong doi cach ve.
"""
import io
import sys

NL = "\r\n"
R3 = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3\KRepresentShell3.cpp"
IPO = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\Scene\KIpoTree.cpp"


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def rep(s, old, new, ten, cho=1):
    n = s.count(old)
    if n != cho:
        print("FAIL neo %s: thay %d lan, can %d" % (ten, n, cho)); sys.exit(1)
    return s.replace(old, new)


# ---------------------------------------------------------------- Represent3
s = doc(R3)
h0 = sum(1 for c in s if ord(c) >= 0x80)
if "[SANGDUNG 09/09]" in s:
    print("Represent3 da va roi")
else:
    old = "int  g_nRep3Pal       = 1;"
    new = NL.join([
        "unsigned g_uRep3VeCoSang = 0;\t// [SANGDUNG 09/09] so lan that su ve qua nhanh CO chieu sang",
        old,
    ])
    s = rep(s, old, new, "R1 bien dem")

    # dem o CA BA cho dung nhanh co chieu sang
    old = "\t\t\t\t\tif(m_bDoLighting && pTemp->bRenderStyle != IMAGE_RENDER_STYLE_ALPHA_NOT_BE_LIT)"
    new = "\t\t\t\t\tif(m_bDoLighting && pTemp->bRenderStyle != IMAGE_RENDER_STYLE_ALPHA_NOT_BE_LIT ? (++g_uRep3VeCoSang, true) : false)\t// [SANGDUNG 09/09] dem"
    s = rep(s, old, new, "R2 hai cho ve sprite", 2)

    old = "\t\t\t\tif(m_bDoLighting)"
    new = "\t\t\t\tif(m_bDoLighting ? (++g_uRep3VeCoSang, true) : false)\t// [SANGDUNG 09/09] dem"
    s = rep(s, old, new, "R3 cho ve bitmap")

    # in ra
    old = "\t\t\tg_dRep3PresentMs = 0.0; g_uRep3Presents = 0; g_uRep3PresentSkip = 0;"
    new = NL.join([
        "\t\t\tRep3Log(\"[SANGDUNG] m_bDoLighting=%d | so lan ve QUA NHANH CO CHIEU SANG: %u\", (int)m_bDoLighting, g_uRep3VeCoSang);",
        "\t\t\tg_uRep3VeCoSang = 0;",
        old,
    ])
    s = rep(s, old, new, "R4 dong log")

    if sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa Represent3"); sys.exit(1)
    io.open(R3, "w", encoding="latin-1", newline="").write(s)
    print("OK KRepresentShell3.cpp")

# ---------------------------------------------------------------- Core / KIpoTree
s = doc(IPO)
h0 = sum(1 for c in s if ord(c) >= 0x80)
if "[SANGCPU 09/09]" in s:
    print("KIpoTree da va roi")
else:
    old = "\t\tRenderLightMap();"
    new = NL.join([
        "\t\tLARGE_INTEGER liSang0, liSang1, liSangF;\t// [SANGCPU 09/09] do CPU dung ban do sang moi khung",
        "\t\tQueryPerformanceCounter(&liSang0);",
        "\t\tRenderLightMap();",
        "\t\tQueryPerformanceCounter(&liSang1);",
        "\t\tQueryPerformanceFrequency(&liSangF);",
        "\t\textern double g_dSangMs, g_dSangMax; extern unsigned g_uSangLan;",
        "\t\t{",
        "\t\t\tconst double dMs = (double)(liSang1.QuadPart - liSang0.QuadPart) * 1000.0 / (double)liSangF.QuadPart;",
        "\t\t\tg_dSangMs += dMs; g_uSangLan++;",
        "\t\t\tif (dMs > g_dSangMax) g_dSangMax = dMs;",
        "\t\t}",
    ])
    s = rep(s, old, new, "I1 do thoi gian")

    old = "\t\t\t\t\t\tfprintf(pLog, \"[SANGDO] t=%u khung=%u | o giua luoi DOI %u lan | sang min %08X max %08X\""
    new = "\t\t\t\t\t\tfprintf(pLog, \"[SANGDO] t=%u khung=%u | CPU dung ban do sang %.2f ms/khung (max %.2f) | o giua luoi DOI %u lan | sang min %08X max %08X\""
    s = rep(s, old, new, "I2 dinh dang log")

    old = "\t\t\t\t\t\t\tdwNow, s_uKhung, s_uDoi, s_dwMin, s_dwMax, s_uGocDoi,"
    new = "\t\t\t\t\t\t\tdwNow, s_uKhung, g_uSangLan ? g_dSangMs / g_uSangLan : 0.0, g_dSangMax, s_uDoi, s_dwMin, s_dwMax, s_uGocDoi,"
    s = rep(s, old, new, "I3 tham so log")

    old = "\t\t\t\t\ts_uKhung = s_uDoi = s_uGocDoi = s_uDenTong = s_uDenMax = 0;"
    new = old + NL + "\t\t\t\t\tg_dSangMs = 0.0; g_dSangMax = 0.0; g_uSangLan = 0;"
    s = rep(s, old, new, "I4 dat lai")

    old = "//##ModelId=3DD9ECFD00E6"
    new = NL.join([
        "double   g_dSangMs = 0.0, g_dSangMax = 0.0;\t// [SANGCPU 09/09]",
        "unsigned g_uSangLan = 0;",
        old,
    ])
    s = rep(s, old, new, "I5 bien toan cuc")

    if sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa KIpoTree"); sys.exit(1)
    io.open(IPO, "w", encoding="latin-1", newline="").write(s)
    print("OK KIpoTree.cpp")

for f in (R3, IPO):
    t = doc(f)
    print("  %-24s CRLF %5d | LF don %d | byte cao %d"
          % (f.rsplit("\\", 1)[-1], t.count("\r\n"), t.count("\n") - t.count("\r\n"), sum(1 for c in t if ord(c) >= 0x80)))
print("XONG SANGDUNG")
