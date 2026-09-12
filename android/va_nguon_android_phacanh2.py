# -*- coding: utf-8 -*-
import io, os
GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[PHACANH2 11/09]"

def doc(p):
    s = io.open(p, encoding="latin-1", newline="").read()
    crlf = s.count("\r\n")
    if crlf and crlf != s.count("\n"):
        raise SystemExit("xuong dong lan lon: " + p)
    return s.replace("\r\n", "\n"), ("\r\n" if crlf else "\n"), sum(1 for c in s if ord(c) >= 0x80)

def ghi(p, s, nl, cao):
    if sum(1 for c in s if ord(c) >= 0x80) != cao:
        raise SystemExit("byte cao doi: " + p)
    io.open(p, "w", encoding="latin-1", newline="").write(s.replace("\n", nl))
    print("da va:", os.path.relpath(p, GOC))

def thay1(s, cu, moi, ten):
    n = s.count(cu)
    if n != 1:
        raise SystemExit("moc '%s' khop %d cho (can 1)" % (ten, n))
    return s.replace(cu, moi)

# ---------------------------------------------------------------- KScenePlaceC.cpp
p = os.path.join(GOC, "Sources", "Core", "Src", "Scene", "KScenePlaceC.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi")
else:
    s = thay1(s, "double g_dJxPhaCanh[7] = { 0, 0, 0, 0, 0, 0, 0 };",
        "// " + DAU + " do phien 233219 cho thay khung 149 ms ma BAY PHA cong lai chi 0,4 ms -> thoi gian nam NGOAI bay pha do.\n"
        "// Ba cho con sot trong KScenePlaceC::Paint: (7) dau ham = IR_UpdateTime + PrerenderGround, (8) CHO KHOA\n"
        "// EnterCriticalSection (luong nap sprite nen giu khoa thi luong ve dung o day), (9) tong ca ham Paint,\n"
        "// (10) DrawSelectInfo. Phan duoi khoa = (9) - (7) - (8) - tong bay pha.\n"
        "double g_dJxPhaCanh[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };",
        "mang pha canh")
    s = thay1(s, "\tLARGE_INTEGER jxC[8];\n#endif\n\tIR_UpdateTime();\n",
        "\tLARGE_INTEGER jxC[8], jxD[2];\n"
        "\tif (bJxDo) QueryPerformanceCounter(&jxD[0]);\t// " + DAU + " vao ham\n"
        "#endif\n"
        "\tIR_UpdateTime();\n",
        "dau ham")
    s = thay1(s, "\tEnterCriticalSection(&m_ProcessCritical);\n\n#if defined(JX_MOBILE) && !defined(_SERVER)\n\tif (bJxDo) QueryPerformanceCounter(&jxC[0]);",
        "#if defined(JX_MOBILE) && !defined(_SERVER)\n"
        "\tif (bJxDo) QueryPerformanceCounter(&jxD[1]);\t// " + DAU + " truoc khi xin khoa\n"
        "#endif\n"
        "\tEnterCriticalSection(&m_ProcessCritical);\n\n#if defined(JX_MOBILE) && !defined(_SERVER)\n\tif (bJxDo) QueryPerformanceCounter(&jxC[0]);",
        "truoc khoa")
    s = thay1(s, "\t\tfor (int q = 0; q < 7; q++) g_dJxPhaCanh[q] = JxCanhMs(jxC[q], jxC[q + 1]);\n",
        "\t\tfor (int q = 0; q < 7; q++) g_dJxPhaCanh[q] = JxCanhMs(jxC[q], jxC[q + 1]);\n"
        "\t\tg_dJxPhaCanh[7] = JxCanhMs(jxD[0], jxD[1]);\t// " + DAU + " dau ham (IR_UpdateTime + PrerenderGround)\n"
        "\t\tg_dJxPhaCanh[8] = JxCanhMs(jxD[1], jxC[0]);\t// " + DAU + " CHO KHOA\n",
        "chot them")
    ghi(p, s, nl, cao)

# ---------------------------------------------------------------- CoreShell.cpp
p = os.path.join(GOC, "Sources", "Core", "Src", "CoreShell.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi")
else:
    s = thay1(s, "\t\tif (g_pRepresent)\n\t\t{\n\t\t\tg_ScenePlace.Paint();\n\t\t\tPlayer[CLIENT_PLAYER_INDEX].DrawSelectInfo();\n\t\t}\n",
        "\t\tif (g_pRepresent)\n"
        "\t\t{\n"
        "#if defined(JX_MOBILE) && !defined(_SERVER)\n"
        "\t\t\t// " + DAU + " tach tong Paint() va DrawSelectInfo de biet phan nao con sot ngoai bay pha\n"
        "\t\t\textern double g_dJxPhaCanh[11];\n"
        "\t\t\tLARGE_INTEGER jxS[3], jxF; QueryPerformanceFrequency(&jxF);\n"
        "\t\t\tQueryPerformanceCounter(&jxS[0]);\n"
        "\t\t\tg_ScenePlace.Paint();\n"
        "\t\t\tQueryPerformanceCounter(&jxS[1]);\n"
        "\t\t\tPlayer[CLIENT_PLAYER_INDEX].DrawSelectInfo();\n"
        "\t\t\tQueryPerformanceCounter(&jxS[2]);\n"
        "\t\t\tif (jxF.QuadPart)\n"
        "\t\t\t{\n"
        "\t\t\t\tg_dJxPhaCanh[9]  = (double)(jxS[1].QuadPart - jxS[0].QuadPart) * 1000.0 / (double)jxF.QuadPart;\n"
        "\t\t\t\tg_dJxPhaCanh[10] = (double)(jxS[2].QuadPart - jxS[1].QuadPart) * 1000.0 / (double)jxF.QuadPart;\n"
        "\t\t\t}\n"
        "#else\n"
        "\t\t\tg_ScenePlace.Paint();\n"
        "\t\t\tPlayer[CLIENT_PLAYER_INDEX].DrawSelectInfo();\n"
        "#endif\n"
        "\t\t}\n",
        "DrawGameSpace nhanh PaintLog")
    ghi(p, s, nl, cao)

# ---------------------------------------------------------------- UiShell.cpp
p = os.path.join(GOC, "Sources", "S3Client", "Ui", "UiShell.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi")
else:
    s = thay1(s, "\t\t\t\textern double g_dJxPhaCanh[7];\t// [PHACANH 11/09] bay pha ben trong 'the gioi'\n",
        "\t\t\t\textern double g_dJxPhaCanh[11];\t// [PHACANH 11/09] bay pha + " + DAU + " ba cho con sot\n",
        "extern mang")
    NL = chr(92) + "n"   # hai ky tu \ va n nhu trong chuoi C++
    s = thay1(s, "\t\t\t\t\t\" | canh: nen %.1f, nen dat %.1f, phu nen %.1f, VAT THE %.1f, tren dau %.1f, truoc het %.1f, thoi tiet %.1f" + NL + "\",\n",
        "\t\t\t\t\t\" | canh: nen %.1f, nen dat %.1f, phu nen %.1f, VAT THE %.1f, tren dau %.1f, truoc het %.1f, thoi tiet %.1f\"\n"
        "\t\t\t\t\t\" | dau ham %.1f, CHO KHOA %.1f, tong Paint %.1f, chon muc tieu %.1f" + NL + "\",\n",
        "dinh dang")
    s = thay1(s, "\t\t\t\t\tg_dJxPhaCanh[0], g_dJxPhaCanh[1], g_dJxPhaCanh[2], g_dJxPhaCanh[3], g_dJxPhaCanh[4], g_dJxPhaCanh[5], g_dJxPhaCanh[6]);\n",
        "\t\t\t\t\tg_dJxPhaCanh[0], g_dJxPhaCanh[1], g_dJxPhaCanh[2], g_dJxPhaCanh[3], g_dJxPhaCanh[4], g_dJxPhaCanh[5], g_dJxPhaCanh[6],\n"
        "\t\t\t\t\tg_dJxPhaCanh[7], g_dJxPhaCanh[8], g_dJxPhaCanh[9], g_dJxPhaCanh[10]);\n",
        "tham so")
    ghi(p, s, nl, cao)
print("xong")
