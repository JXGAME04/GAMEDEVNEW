# -*- coding: utf-8 -*-
r"""[PHACANH 11/09] Chia nho pha "ve the gioi" - buoc cuoi cua chuoi truy khung giat.

Chuoi truy den gio, moi buoc deu co so:
  1. [VE-GIAT] khung giat 28,6 ms = trong lop ve 8,7 + NGOAI 19,9 (70 %); 71/84 khung la >80 % ngoai.
  2. [PDET]    ca 20 ms do nam gon trong MOT lenh goi Wnd_RenderWindows().
  3. [PHAVE]   trong Wnd_RenderWindows: VE THE GIOI chiem 78,6 %, dan dau o 177/202 khung cham
               (TB 15,12 ms, dinh 146,4). Ba lop giao dien chi 0,8-1,9 ms TB. Vong "neo cua so"
               (chi co tren mobile) = 0,00 ms - khong phai thu pham, do xong loai bo nghi ngo.
  4. (buoc nay) trong KScenePlaceC::Paint chia lam bay pha.

Bay pha:
  0 nen        = PaintBackGround + VeLopCanh(1) + VeLopCanh(3)
  1 nen dat    = vong PaintGround cua cac vung
  2 phu nen    = m_ObjectsTree.Paint(IPOT_RL_COVER_GROUND)
  3 VAT THE    = m_ObjectsTree.Paint(IPOT_RL_OBJECT)  <- noi ve NPC / nguoi choi (nghi la pha nang nhat)
  4 tren dau   = vong PaintAboveHeadObj (the ten, thanh mau, bieu tuong)
  5 truoc het  = m_ObjectsTree.Paint(IPOT_RL_INFRONTOF_ALL) + VeLopCanh(2)
  6 thoi tiet  = m_pWeather->Render

Chi chay khi CorePaintLog > 0. Bay lan doc dong ho moi khung, khong dang ke.
Rao JX_MOBILE nen ban PC khong doi mot byte. KScenePlaceC.cpp co 1 631 byte cao -> doc/ghi latin-1.
Dung:  python android\va_nguon_android_phacanh.py
"""
import io
import os

GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[PHACANH 11/09]"


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


# ============================================================ KScenePlaceC.cpp
p = os.path.join(GOC, "Sources", "Core", "Src", "Scene", "KScenePlaceC.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "void KScenePlaceC::Paint()\n{\n\tIR_UpdateTime();\n",
        "#if defined(JX_MOBILE) && !defined(_SERVER)\n"
        "// " + DAU + " bay pha cua viec ve the gioi. Da truy duoc: 78,6 %% thoi gian khung giat nam o day\n"
        "// (dan dau 177/202 khung cham, TB 15,12 ms, dinh 146,4). Chia nho de biet pha nao.\n"
        "double g_dJxPhaCanh[7] = { 0, 0, 0, 0, 0, 0, 0 };\n"
        "extern int g_nCorePaintLog;\n"
        "static double JxCanhMs(const LARGE_INTEGER& a, const LARGE_INTEGER& b)\n"
        "{\n"
        "\tstatic double s_dF = 0.0;\n"
        "\tif (s_dF == 0.0) { LARGE_INTEGER f; QueryPerformanceFrequency(&f); s_dF = (double)f.QuadPart; }\n"
        "\treturn s_dF > 0.0 ? (double)(b.QuadPart - a.QuadPart) * 1000.0 / s_dF : 0.0;\n"
        "}\n"
        "#endif\n"
        "\n"
        "void KScenePlaceC::Paint()\n"
        "{\n"
        "#if defined(JX_MOBILE) && !defined(_SERVER)\n"
        "\tconst bool bJxDo = (g_nCorePaintLog > 0);\t// " + DAU + "\n"
        "\tLARGE_INTEGER jxC[8];\n"
        "#endif\n"
        "\tIR_UpdateTime();\n",
        "dau KScenePlaceC::Paint")
    s = thay1(s, "\tBOOL bPrerenderGroundImg = PaintBackGround();",
        "#if defined(JX_MOBILE) && !defined(_SERVER)\n"
        "\tif (bJxDo) QueryPerformanceCounter(&jxC[0]);\t// " + DAU + "\n"
        "#endif\n"
        "\tBOOL bPrerenderGroundImg = PaintBackGround();",
        "PaintBackGround")
    s = thay1(s, "\tVeLopCanh(3);\t// [ANHNEN 10/09 f] lop phu nen (may xa) ve tren anh nen, DUOI nen dat\n",
        "\tVeLopCanh(3);\t// [ANHNEN 10/09 f] lop phu nen (may xa) ve tren anh nen, DUOI nen dat\n"
        "#if defined(JX_MOBILE) && !defined(_SERVER)\n"
        "\tif (bJxDo) QueryPerformanceCounter(&jxC[1]);\t// " + DAU + " het pha NEN\n"
        "#endif\n",
        "VeLopCanh 3")
    s = thay1(s, "\tm_ObjectsTree.Paint(&m_RepresentArea, IPOT_RL_COVER_GROUND);\n"
        "\tm_ObjectsTree.Paint(&m_RepresentArea, IPOT_RL_OBJECT);\n",
        "#if defined(JX_MOBILE) && !defined(_SERVER)\n"
        "\tif (bJxDo) QueryPerformanceCounter(&jxC[2]);\t// " + DAU + " het pha NEN DAT\n"
        "#endif\n"
        "\tm_ObjectsTree.Paint(&m_RepresentArea, IPOT_RL_COVER_GROUND);\n"
        "#if defined(JX_MOBILE) && !defined(_SERVER)\n"
        "\tif (bJxDo) QueryPerformanceCounter(&jxC[3]);\t// " + DAU + " het pha PHU NEN\n"
        "#endif\n"
        "\tm_ObjectsTree.Paint(&m_RepresentArea, IPOT_RL_OBJECT);\n"
        "#if defined(JX_MOBILE) && !defined(_SERVER)\n"
        "\tif (bJxDo) QueryPerformanceCounter(&jxC[4]);\t// " + DAU + " het pha VAT THE (NPC / nguoi choi)\n"
        "#endif\n",
        "ObjectsTree COVER_GROUND + OBJECT")
    s = thay1(s, "\tm_ObjectsTree.Paint(&m_RepresentArea, IPOT_RL_INFRONTOF_ALL);\n"
        "\tVeLopCanh(2);\t// [ANHNEN 10/09] tien canh ve sau cung\n",
        "#if defined(JX_MOBILE) && !defined(_SERVER)\n"
        "\tif (bJxDo) QueryPerformanceCounter(&jxC[5]);\t// " + DAU + " het pha TREN DAU\n"
        "#endif\n"
        "\tm_ObjectsTree.Paint(&m_RepresentArea, IPOT_RL_INFRONTOF_ALL);\n"
        "\tVeLopCanh(2);\t// [ANHNEN 10/09] tien canh ve sau cung\n"
        "#if defined(JX_MOBILE) && !defined(_SERVER)\n"
        "\tif (bJxDo) QueryPerformanceCounter(&jxC[6]);\t// " + DAU + " het pha TRUOC HET\n"
        "#endif\n",
        "INFRONTOF_ALL")
    s = thay1(s, "\t\tm_pWeather->Render(g_pRepresent);\n\n\tLeaveCriticalSection(&m_ProcessCritical);\n",
        "\t\tm_pWeather->Render(g_pRepresent);\n"
        "\n"
        "#if defined(JX_MOBILE) && !defined(_SERVER)\n"
        "\tif (bJxDo)\n"
        "\t{\t// " + DAU + " chot bay pha; UiShell.cpp in kem vao dong [PDET]\n"
        "\t\tQueryPerformanceCounter(&jxC[7]);\n"
        "\t\tfor (int q = 0; q < 7; q++) g_dJxPhaCanh[q] = JxCanhMs(jxC[q], jxC[q + 1]);\n"
        "\t}\n"
        "#endif\n"
        "\tLeaveCriticalSection(&m_ProcessCritical);\n",
        "LeaveCriticalSection sau thoi tiet")
    ghi(p, s, nl, cao)

# ============================================================ UiShell.cpp
p = os.path.join(GOC, "Sources", "S3Client", "Ui", "UiShell.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "\t\t\t\textern double g_dJxPhaVe[5];\n"
        "\t\t\t\tfprintf(pLog, \"[PDET] begin=%u render=%u end=%u | the gioi %.1f, neo cua so %.1f, lop duoi %.1f, lop giua %.1f, lop tren %.1f\\n\",\n"
        "\t\t\t\t\tnPdT1 - nPdT0, nPdT2 - nPdT1, nPdT3 - nPdT2,\n"
        "\t\t\t\t\tg_dJxPhaVe[0], g_dJxPhaVe[1], g_dJxPhaVe[2], g_dJxPhaVe[3], g_dJxPhaVe[4]);\n",
        "\t\t\t\textern double g_dJxPhaVe[5];\n"
        "\t\t\t\textern double g_dJxPhaCanh[7];\t// " + DAU + " bay pha ben trong 'the gioi'\n"
        "\t\t\t\tfprintf(pLog, \"[PDET] begin=%u render=%u end=%u | the gioi %.1f, neo cua so %.1f, lop duoi %.1f, lop giua %.1f, lop tren %.1f\"\n"
        "\t\t\t\t\t\" | canh: nen %.1f, nen dat %.1f, phu nen %.1f, VAT THE %.1f, tren dau %.1f, truoc het %.1f, thoi tiet %.1f\\n\",\n"
        "\t\t\t\t\tnPdT1 - nPdT0, nPdT2 - nPdT1, nPdT3 - nPdT2,\n"
        "\t\t\t\t\tg_dJxPhaVe[0], g_dJxPhaVe[1], g_dJxPhaVe[2], g_dJxPhaVe[3], g_dJxPhaVe[4],\n"
        "\t\t\t\t\tg_dJxPhaCanh[0], g_dJxPhaCanh[1], g_dJxPhaCanh[2], g_dJxPhaCanh[3], g_dJxPhaCanh[4], g_dJxPhaCanh[5], g_dJxPhaCanh[6]);\n",
        "dong [PDET] mobile")
    ghi(p, s, nl, cao)

print("xong")
