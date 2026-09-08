# -*- coding: ascii -*-
"""goi_va_d3d11h_anhnull_top_0809.py - [REP3 08/09 h] anh_null 0,6-1,6 trieu / 30 s (toi 900 luot/khung) chua biet la anh nao:
ghi top 6 ten anh (kem khung xin) bi bo qua trong ky thong ke, in dong '[REP3] anh_null top:' sau dong RAM."""
import io, re, sys
p = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3\KRepresentShell3.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read(); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:70], c, n)); sys.exit(1)
    return s.replace(old, new)
if "Rep3AnhNullGhi" not in s:
    helper = (
        "// [REP3 08/09 h] top ten anh bi bo qua (GetImage NULL hoac khung ngoai tam) trong ky thong ke\r\n"
        "struct Rep3AnhNullMuc { char szTen[64]; int nKhung; unsigned uDem; };\r\n"
        "static Rep3AnhNullMuc s_Rep3AnhNull[8];\r\n"
        "static int s_nRep3AnhNull = 0;\r\n"
        "static void Rep3AnhNullGhi(const char* szTen, int nKhung)\r\n"
        "{\r\n"
        "\tif (!szTen) szTen = \"?\";\r\n"
        "\tint i;\r\n"
        "\tfor (i = 0; i < s_nRep3AnhNull; i++)\r\n"
        "\t\tif (strncmp(s_Rep3AnhNull[i].szTen, szTen, 63) == 0 && s_Rep3AnhNull[i].nKhung == nKhung) { s_Rep3AnhNull[i].uDem++; return; }\r\n"
        "\tif (s_nRep3AnhNull < 8)\r\n"
        "\t{\r\n"
        "\t\tstrncpy(s_Rep3AnhNull[s_nRep3AnhNull].szTen, szTen, 63); s_Rep3AnhNull[s_nRep3AnhNull].szTen[63] = 0;\r\n"
        "\t\ts_Rep3AnhNull[s_nRep3AnhNull].nKhung = nKhung; s_Rep3AnhNull[s_nRep3AnhNull].uDem = 1; s_nRep3AnhNull++;\r\n"
        "\t}\r\n"
        "}\r\n"
        "static void Rep3AnhNullIn()\r\n"
        "{\r\n"
        "\tif (s_nRep3AnhNull <= 0) return;\r\n"
        "\tchar sz[640]; int n = 0;\r\n"
        "\tfor (int i = 0; i < s_nRep3AnhNull && n < 560; i++)\r\n"
        "\t\tn += sprintf(sz + n, \"%s%s(k%d) x%u\", i ? \" ; \" : \"\", s_Rep3AnhNull[i].szTen, s_Rep3AnhNull[i].nKhung, s_Rep3AnhNull[i].uDem);\r\n"
        "\tRep3Log(\"[REP3] anh_null top: %s\", sz);\r\n"
        "\ts_nRep3AnhNull = 0;\r\n"
        "}\r\n"
        "void Rep3Log(const char* fmt, ...)\r\n")
    s = rep(s, "void Rep3Log(const char* fmt, ...)\r\n", helper)
    # 7 cho co pTemp->szImage; cho thu 8 (dong ~1669) khong co -> ghi "?"
    lines = s.split("\r\n")
    n7 = 0
    for i, l in enumerate(lines):
        if "g_uRep3FxAnhNull++;" in l:
            ctx = "\r\n".join(lines[max(0, i - 4):i])
            if "pTemp->szImage" in ctx:
                lines[i] = l.replace("g_uRep3FxAnhNull++;", "g_uRep3FxAnhNull++; Rep3AnhNullGhi(pTemp->szImage, pTemp->nFrame);"); n7 += 1
            else:
                lines[i] = l.replace("g_uRep3FxAnhNull++;", "g_uRep3FxAnhNull++; Rep3AnhNullGhi(\"?\", pTemp->nFrame);")
    s = "\r\n".join(lines)
    print("cho co ten:", n7)
    # in sau dong thong ke (sau khi reset cac bo dem fx)
    s = rep(s, "g_uRep3FxTexNull = 0; g_uRep3FxAnhNull = 0; g_uRep3FxTaoHong = 0; g_uRep3FxKhungKhongTex = 0; g_uRep3FxGiaiMa = 0; g_dRep3FxGiaiMaMs = 0.0;",
        "g_uRep3FxTexNull = 0; g_uRep3FxAnhNull = 0; g_uRep3FxTaoHong = 0; g_uRep3FxKhungKhongTex = 0; g_uRep3FxGiaiMa = 0; g_dRep3FxGiaiMaMs = 0.0;\r\n\t\t\tRep3AnhNullIn();\t// [REP3 08/09 h]")
if sum(1 for ch in s if ord(ch) >= 0x80) != h0 or re.search(r"[^\r]\n", s): print("FAIL byte cao / LF"); sys.exit(1)
io.open(p, "w", encoding="latin-1", newline="").write(s)
print("OK anh_null top")
