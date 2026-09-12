# -*- coding: utf-8 -*-
r"""[PHAVE 11/09] Do BON PHA cua Wnd_RenderWindows de biet 20 ms cua khung giat nam o dau.

Duong di da truy duoc den day:
  [VE-GIAT] (lop ve)      -> khung giat 28,6 ms, trong do lop ve chi 8,7 ms, NGOAI 19,9 ms (70 %).
  [PDET]    (UiShell.cpp) -> "begin=0 render=16..22 end=3..6": toan bo 20 ms nam trong MOT lenh goi
                             Wnd_RenderWindows() (271 lan trong 19 phut, ~14 lan/phut).
Va da kiem: moi ban toi uu dung canh cua ban PC ([VUNG][WORLD][CHUGIU][NAPNPC][SANG2][LOCTG][TRANGTRI]...)
DEU DA CO trong nhanh mobile, nen khong con gi de chep - phai tu do.

Wnd_RenderWindows chia lam bon pha ro rang (Wnds.cpp):
  1. pGameSpaceWnd->Paint()      = the gioi (ban do + nhan vat + hieu ung)
  2. vong FitToScreen             = CHI CO TREN MOBILE, duyet moi cua so goc moi khung
  3. LowLayerRoot.Paint()         = giao dien lop duoi
  4. NormalLayerRoot.Paint()      = giao dien lop giua
  5. TopLayerRoot.Paint()         = giao dien lop tren
Bo do cong thoi gian tung pha, UiShell.cpp in kem vao dong [PDET] san co.

Chi bat khi PaintLog=1 (dien thoai dang bat). Chi 6 lan doc dong ho moi khung nen khong dang ke.
Rao JX_MOBILE de ban PC khong doi mot byte.
Doc/ghi latin-1 (Wnds.cpp co 736 byte cao), giu CRLF, moc khop dung 1 cho, byte cao khong doi.
Dung:  python android\va_nguon_android_phave.py
"""
import io
import os

GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[PHAVE 11/09]"


def doc(p):
    s = io.open(p, encoding="latin-1", newline="").read()
    crlf = s.count("\r\n")
    if crlf and crlf != s.count("\n"):
        raise SystemExit("tep co xuong dong lan lon, khong va: " + p)
    return s.replace("\r\n", "\n"), ("\r\n" if crlf else "\n"), sum(1 for c in s if ord(c) >= 0x80)


def ghi(p, s, nl, cao):
    if sum(1 for c in s if ord(c) >= 0x80) != cao:
        raise SystemExit("so byte cao doi - khong ghi: " + p)
    io.open(p, "w", encoding="latin-1", newline="").write(s.replace("\n", nl))
    print("da va:", os.path.relpath(p, GOC))


def thay1(s, cu, moi, ten):
    n = s.count(cu)
    if n != 1:
        raise SystemExit("moc '%s' khop %d cho (can 1)" % (ten, n))
    return s.replace(cu, moi)


# ============================================================ Wnds.cpp
p = os.path.join(GOC, "Sources", "S3Client", "Ui", "Elem", "Wnds.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "void Wnd_RenderWindows()\n{\n\tint\tbShowCursor = true;\n\n"
        "\tif (s_WndStation.pGameSpaceWnd && s_WndStation.bPaintGameSpace)\n"
        "\t\ts_WndStation.pGameSpaceWnd->Paint();\n",
        "#ifdef JX_MOBILE\n"
        "// " + DAU + " do bon pha cua Wnd_RenderWindows. Da truy ra: 70 %% thoi gian cua khung giat nam ngoai lop ve,\n"
        "// va [PDET] cho thay ca 20 ms do nam gon trong MOT lenh goi Wnd_RenderWindows. Chia nho de biet pha nao.\n"
        "// 0 the gioi | 1 neo lai cua so (chi mobile) | 2 lop duoi | 3 lop giua | 4 lop tren.\n"
        "double g_dJxPhaVe[5] = { 0, 0, 0, 0, 0 };\n"
        "extern int g_nPaintLog;\n"
        "static double JxPhaMs(const LARGE_INTEGER& a, const LARGE_INTEGER& b)\n"
        "{\n"
        "\tstatic double s_dF = 0.0;\n"
        "\tif (s_dF == 0.0) { LARGE_INTEGER f; QueryPerformanceFrequency(&f); s_dF = (double)f.QuadPart; }\n"
        "\treturn s_dF > 0.0 ? (double)(b.QuadPart - a.QuadPart) * 1000.0 / s_dF : 0.0;\n"
        "}\n"
        "#endif\n"
        "\n"
        "void Wnd_RenderWindows()\n{\n\tint\tbShowCursor = true;\n"
        "#ifdef JX_MOBILE\n"
        "\tconst bool bJxDo = (g_nPaintLog > 0);\t// " + DAU + "\n"
        "\tLARGE_INTEGER jxT[6];\n"
        "\tif (bJxDo) QueryPerformanceCounter(&jxT[0]);\n"
        "#endif\n"
        "\n"
        "\tif (s_WndStation.pGameSpaceWnd && s_WndStation.bPaintGameSpace)\n"
        "\t\ts_WndStation.pGameSpaceWnd->Paint();\n"
        "#ifdef JX_MOBILE\n"
        "\tif (bJxDo) QueryPerformanceCounter(&jxT[1]);\t// " + DAU + " het pha the gioi\n"
        "#endif\n",
        "dau Wnd_RenderWindows")
    s = thay1(s, "\t}\n#endif\n\ts_WndStation.LowLayerRoot.Paint();\n"
        "\ts_WndStation.NormalLayerRoot.Paint();\n"
        "\ts_WndStation.TopLayerRoot.Paint();\n",
        "\t}\n"
        "\tif (bJxDo) QueryPerformanceCounter(&jxT[2]);\t// " + DAU + " het pha neo lai cua so\n"
        "#endif\n"
        "\ts_WndStation.LowLayerRoot.Paint();\n"
        "#ifdef JX_MOBILE\n"
        "\tif (bJxDo) QueryPerformanceCounter(&jxT[3]);\t// " + DAU + "\n"
        "#endif\n"
        "\ts_WndStation.NormalLayerRoot.Paint();\n"
        "#ifdef JX_MOBILE\n"
        "\tif (bJxDo) QueryPerformanceCounter(&jxT[4]);\t// " + DAU + "\n"
        "#endif\n"
        "\ts_WndStation.TopLayerRoot.Paint();\n"
        "#ifdef JX_MOBILE\n"
        "\tif (bJxDo)\n"
        "\t{\t// " + DAU + " chot nam pha; UiShell.cpp in kem vao dong [PDET] khi khung cham\n"
        "\t\tQueryPerformanceCounter(&jxT[5]);\n"
        "\t\tg_dJxPhaVe[0] = JxPhaMs(jxT[0], jxT[1]);\n"
        "\t\tg_dJxPhaVe[1] = JxPhaMs(jxT[1], jxT[2]);\n"
        "\t\tg_dJxPhaVe[2] = JxPhaMs(jxT[2], jxT[3]);\n"
        "\t\tg_dJxPhaVe[3] = JxPhaMs(jxT[3], jxT[4]);\n"
        "\t\tg_dJxPhaVe[4] = JxPhaMs(jxT[4], jxT[5]);\n"
        "\t}\n"
        "#endif\n",
        "cuoi ba lop")
    ghi(p, s, nl, cao)

# ============================================================ UiShell.cpp
p = os.path.join(GOC, "Sources", "S3Client", "Ui", "UiShell.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "\t\t\t\tfprintf(pLog, \"[PDET] begin=%u render=%u end=%u\\n\",\n"
        "\t\t\t\t\tnPdT1 - nPdT0, nPdT2 - nPdT1, nPdT3 - nPdT2);\n",
        "#ifdef JX_MOBILE\n"
        "\t\t\t\t// " + DAU + " tach 'render' thanh nam pha cua Wnd_RenderWindows\n"
        "\t\t\t\textern double g_dJxPhaVe[5];\n"
        "\t\t\t\tfprintf(pLog, \"[PDET] begin=%u render=%u end=%u | the gioi %.1f, neo cua so %.1f, lop duoi %.1f, lop giua %.1f, lop tren %.1f\\n\",\n"
        "\t\t\t\t\tnPdT1 - nPdT0, nPdT2 - nPdT1, nPdT3 - nPdT2,\n"
        "\t\t\t\t\tg_dJxPhaVe[0], g_dJxPhaVe[1], g_dJxPhaVe[2], g_dJxPhaVe[3], g_dJxPhaVe[4]);\n"
        "#else\n"
        "\t\t\t\tfprintf(pLog, \"[PDET] begin=%u render=%u end=%u\\n\",\n"
        "\t\t\t\t\tnPdT1 - nPdT0, nPdT2 - nPdT1, nPdT3 - nPdT2);\n"
        "#endif\n",
        "dong [PDET]")
    ghi(p, s, nl, cao)

print("xong")
