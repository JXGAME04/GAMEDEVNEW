# -*- coding: utf-8 -*-
import io, os
GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[NENDAT 11/09]"

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

p = os.path.join(GOC, "Sources", "Core", "Src", "Scene", "KScenePlaceC.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi")
else:
    s = thay1(s, "double g_dJxPhaCanh[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };",
        "double g_dJxPhaCanh[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };\n"
        "// " + DAU + " ba nhanh cua PrerenderGround: 0 vung CHUA nguoi choi (khong chiu ngan sach),\n"
        "// 1 tam vung ke ben (chiu ngan sach 8 ms), 2 vung XA ngoai man hinh (1 vung/khung, KHONG chiu ngan sach).\n"
        "// Do phien 234254: 'dau ham' = PrerenderGround chiem 61,7 %% thoi gian ve the gioi, dan dau 204/311 khung,\n"
        "// dinh 142,6 ms; 115 dong [PGND] >= 15 ms trong mot phien. Chia ba nhanh de biet cat cho nao cho dung.\n"
        "double g_dJxNenNhanh[3] = { 0, 0, 0 };\n"
        "int    g_nJxNenSo[3] = { 0, 0, 0 };",
        "mang nhanh nen")
    s = thay1(s, "\tDWORD\tdwPgT0 = timeGetTime();\n\tconst DWORD\tdwPgBudgetMs = 8;\n",
        "\tDWORD\tdwPgT0 = timeGetTime();\n"
        "\tconst DWORD\tdwPgBudgetMs = 8;\n"
        "#if defined(JX_MOBILE) && !defined(_SERVER)\n"
        "\tLARGE_INTEGER jxN0, jxN1, jxNF; QueryPerformanceFrequency(&jxNF);\t// " + DAU + "\n"
        "\tfor (int q = 0; q < 3; q++) { g_dJxNenNhanh[q] = 0.0; g_nJxNenSo[q] = 0; }\n"
        "#define JX_NEN_DO(nhanh, lenh) \\n"
        "\tdo { QueryPerformanceCounter(&jxN0); lenh; QueryPerformanceCounter(&jxN1); \\n"
        "\t     if (jxNF.QuadPart) g_dJxNenNhanh[nhanh] += (double)(jxN1.QuadPart - jxN0.QuadPart) * 1000.0 / (double)jxNF.QuadPart; \\n"
        "\t     g_nJxNenSo[nhanh]++; } while (0)\n"
        "#else\n"
        "#define JX_NEN_DO(nhanh, lenh) do { lenh; } while (0)\n"
        "#endif\n",
        "ngan sach")
    s = thay1(s, "\t\t\tm_pInProcessAreaRegions[i]->PrerenderGround(false);\n\t\t}\n\t\telse if (nDx <= 1 && nDy <= 1)\n",
        "\t\t\tJX_NEN_DO(0, m_pInProcessAreaRegions[i]->PrerenderGround(false));\t// " + DAU + "\n"
        "\t\t}\n\t\telse if (nDx <= 1 && nDy <= 1)\n",
        "nhanh tieu diem")
    s = thay1(s, "\t\t\tif (timeGetTime() - dwPgT0 < dwPgBudgetMs)\n\t\t\t\tm_pInProcessAreaRegions[i]->PrerenderGround(false);\n",
        "\t\t\tif (timeGetTime() - dwPgT0 < dwPgBudgetMs)\n"
        "\t\t\t\tJX_NEN_DO(1, m_pInProcessAreaRegions[i]->PrerenderGround(false));\t// " + DAU + "\n",
        "nhanh ke ben")
    s = thay1(s, "\t\t\tif (m_pInProcessAreaRegions[i]->PrerenderGround(false))\n\t\t\t\tnFarBudget--;\n",
        "\t\t\t{\n"
        "\t\t\t\tbool bJxOk = false;\t// " + DAU + "\n"
        "\t\t\t\tJX_NEN_DO(2, bJxOk = (m_pInProcessAreaRegions[i]->PrerenderGround(false) != 0));\n"
        "\t\t\t\tif (bJxOk) nFarBudget--;\n"
        "\t\t\t}\n",
        "nhanh xa")
    s = thay1(s, "\tif (nDeferred && bForce == false)\n",
        "#undef JX_NEN_DO\n"
        "\tif (nDeferred && bForce == false)\n",
        "cuoi vong")
    NL = chr(92) + "n"
    s = thay1(s, "\t\t\t\tfprintf(pPgLog, \"[PGND] ms=%u" + NL + "\", dwPgMs);\n",
        "#if defined(JX_MOBILE) && !defined(_SERVER)\n"
        "\t\t\t\t// " + DAU + " tach ba nhanh de biet cat cho nao\n"
        "\t\t\t\tfprintf(pPgLog, \"[PGND] ms=%u | tieu diem %.1f/%d, ke ben %.1f/%d, XA %.1f/%d" + NL + "\", dwPgMs,\n"
        "\t\t\t\t\tg_dJxNenNhanh[0], g_nJxNenSo[0], g_dJxNenNhanh[1], g_nJxNenSo[1], g_dJxNenNhanh[2], g_nJxNenSo[2]);\n"
        "#else\n"
        "\t\t\t\tfprintf(pPgLog, \"[PGND] ms=%u" + NL + "\", dwPgMs);\n"
        "#endif\n",
        "dong PGND")
    ghi(p, s, nl, cao)
print("xong")
