# -*- coding: utf-8 -*-
"""goi_va_ve2_thoi_gian_pass_ve_0809.py - [VE 08/09 b] DO TRUOC viec #2 (tiep): thoi gian CPU luong ve: tong ms trong DrawPrimitives
moi khung (TB / max trong ky) va ca khung tu RepresentBegin den truoc Present (TB / max). In vao [REP3-NAP]:
'| cpu ve: DrawPrimitives %.2f ms/khung (max %.1f), khung %.2f ms (max %.1f)'."""
import io, re, sys
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3"
p = ROOT + "\\KRepresentShell3.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read(); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:90], c, n)); sys.exit(1)
    return s.replace(old, new)
if "g_dRep3VeDpKhung" not in s:
    s = rep(s, "unsigned g_uRep3VeLoai[6] = {0, 0, 0, 0, 0, 0}; unsigned g_uRep3VeKhung = 0;\t// [VE 08/09 a]\r\n",
        "unsigned g_uRep3VeLoai[6] = {0, 0, 0, 0, 0, 0}; unsigned g_uRep3VeKhung = 0;\t// [VE 08/09 a]\r\n"
        "// [VE 08/09 b] thoi gian CPU luong ve: DrawPrimitives (tong trong khung) va ca khung Begin->End\r\n"
        "double g_dRep3VeDpKhung = 0.0, g_dRep3VeDpTong = 0.0, g_dRep3VeDpMax = 0.0, g_dRep3VeKhungTong = 0.0, g_dRep3VeKhungMax = 0.0;\r\n"
        "LARGE_INTEGER g_liRep3VeBegin = {0};\r\n"
        "struct Rep3VeDpTimer { LARGE_INTEGER a; Rep3VeDpTimer() { QueryPerformanceCounter(&a); } ~Rep3VeDpTimer() { LARGE_INTEGER b; QueryPerformanceCounter(&b); g_dRep3VeDpKhung += Rep3NapMs(a, b); } };\r\n")
    # Rep3NapMs phai duoc khai bao truoc struct: no nam sau (dinh nghia o duoi) -> them khai bao truoc
    s = rep(s, "unsigned g_uRep3VeLoai[6] = {0, 0, 0, 0, 0, 0}; unsigned g_uRep3VeKhung = 0;\t// [VE 08/09 a]\r\n",
        "double Rep3NapMs(const LARGE_INTEGER& a, const LARGE_INTEGER& b);\r\n"
        "unsigned g_uRep3VeLoai[6] = {0, 0, 0, 0, 0, 0}; unsigned g_uRep3VeKhung = 0;\t// [VE 08/09 a]\r\n")
    s = rep(s, "void KRepresentShell3::DrawPrimitives(int nPrimitiveCount, KRepresentUnit* pPrimitives, unsigned int uGenre, int bSinglePlaneCoord)\r\n{\r\n",
        "void KRepresentShell3::DrawPrimitives(int nPrimitiveCount, KRepresentUnit* pPrimitives, unsigned int uGenre, int bSinglePlaneCoord)\r\n{\r\n\tRep3VeDpTimer veDp;\t// [VE 08/09 b]\r\n")
    s = rep(s, "bool KRepresentShell3::RepresentBegin(int bClear, unsigned int Color)\r\n{\r\n",
        "bool KRepresentShell3::RepresentBegin(int bClear, unsigned int Color)\r\n{\r\n\tQueryPerformanceCounter(&g_liRep3VeBegin);\t// [VE 08/09 b]\r\n")
    s = rep(s, "\tg_uRep3VeKhung++;\t// [VE 08/09 a]\r\n",
        "\tg_uRep3VeKhung++;\t// [VE 08/09 a]\r\n"
        "\t{\t// [VE 08/09 b] CPU pass ve khung nay\r\n"
        "\t\tLARGE_INTEGER liNow; QueryPerformanceCounter(&liNow);\r\n"
        "\t\tconst double dKhung = g_liRep3VeBegin.QuadPart ? Rep3NapMs(g_liRep3VeBegin, liNow) : 0.0;\r\n"
        "\t\tg_dRep3VeKhungTong += dKhung; if (dKhung > g_dRep3VeKhungMax) g_dRep3VeKhungMax = dKhung;\r\n"
        "\t\tg_dRep3VeDpTong += g_dRep3VeDpKhung; if (g_dRep3VeDpKhung > g_dRep3VeDpMax) g_dRep3VeDpMax = g_dRep3VeDpKhung;\r\n"
        "\t\tg_dRep3VeDpKhung = 0.0;\r\n\t}\r\n")
    s = rep(s, "ve/khung: npc %.0f skill %.0f ui %.0f map %.0f tao %.0f khac %.0f (khung %u)\",\t// [NAP 08/09 a/b] [VE 08/09 a]\r\n",
        "ve/khung: npc %.0f skill %.0f ui %.0f map %.0f tao %.0f khac %.0f (khung %u) | cpu ve: DrawPrimitives %.2f ms/khung (max %.1f), khung %.2f ms (max %.1f)\",\t// [NAP 08/09 a/b] [VE 08/09 a/b]\r\n")
    s = rep(s, "g_uRep3VeKhung ? (double)g_uRep3VeLoai[5] / g_uRep3VeKhung : 0.0, g_uRep3VeKhung);\r\n",
        "g_uRep3VeKhung ? (double)g_uRep3VeLoai[5] / g_uRep3VeKhung : 0.0, g_uRep3VeKhung,\r\n"
        "\t\t\t\tg_uRep3VeKhung ? g_dRep3VeDpTong / g_uRep3VeKhung : 0.0, g_dRep3VeDpMax, g_uRep3VeKhung ? g_dRep3VeKhungTong / g_uRep3VeKhung : 0.0, g_dRep3VeKhungMax);\r\n"
        "\t\t\tg_dRep3VeDpTong = 0.0; g_dRep3VeDpMax = 0.0; g_dRep3VeKhungTong = 0.0; g_dRep3VeKhungMax = 0.0;\r\n")
if sum(1 for ch in s if ord(ch) >= 0x80) != h0: print("FAIL byte cao"); sys.exit(1)
io.open(p, "w", encoding="latin-1", newline="").write(s); print("OK KRepresentShell3.cpp"); print("XONG VE b")
