# -*- coding: utf-8 -*-
#
# [GOC 12/09] Cua so goc cua mot lop KHONG phai luc nao cung ten muc "Main": ban do nho la KUiMiniMap|MiniMap. Bo neo tim cha
# theo "<lop>|Main" nen 4 nut cua ban do (SwitchBtn, WorldMapBtn, CaveMapBtn, BtnFlag) khong tru neo cua cha -> dich DOI (thua
# 148 px tren dien thoai, 112 px tren may tinh bang: nut roi ra ngoai ban do). Them dong "Goc.<lop>=<muc goc>" trong tep bo cuc
# (bo sinh tu ghi tu vi tri that) va bo neo tra cuu ten muc goc qua bang do.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[GOC 12/09]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (s.count(c), ten))
    return s.replace(c, nl.join(moi))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


T = "Sources/S3Client/Ui/Elem/UiToaDo.cpp"
s = doc(T)
if DAU not in s:
    s0 = s
    # bang do lop -> ten muc goc + ham tao khoa cha
    s = thay(s, ["static int	s_nTepNap = 0;				// so thu tu tep dang nap"],
                ["// %s Ten muc cua CUA SO GOC theo lop, doc tu dong \"Goc.<lop>=<muc>\" trong tep bo cuc (mac dinh \"Main\")." % DAU,
                 "#define\tUITOADO_MAX_GOC\t32",
                 "static char\ts_szGocLop[UITOADO_MAX_GOC][64];",
                 "static char\ts_szGocMuc[UITOADO_MAX_GOC][64];",
                 "static int\ts_nSoGoc = 0;",
                 "//\tKhoa cua cua so goc (cha) cua mot o con \"<lop>|<muc>\"; tra false neu chinh no la goc.",
                 "static bool KhoaCuaCha(const char* pszKhoa, char* pszRa, int nCo)",
                 "{",
                 "\tconst char*\tpGach = strchr(pszKhoa, '|');",
                 "\tconst char*\tpszGoc = \"Main\";",
                 "\tint\t\t\ti;",
                 "\tif (pGach == NULL)",
                 "\t\treturn false;",
                 "\tfor (i = 0; i < s_nSoGoc; i++)",
                 "\t\tif ((int)strlen(s_szGocLop[i]) == (int)(pGach - pszKhoa) && strncmp(s_szGocLop[i], pszKhoa, pGach - pszKhoa) == 0)",
                 "\t\t{",
                 "\t\t\tpszGoc = s_szGocMuc[i];",
                 "\t\t\tbreak;",
                 "\t\t}",
                 "\tif (strcmp(pGach + 1, pszGoc) == 0)",
                 "\t\treturn false;",
                 "\t_snprintf(pszRa, nCo, \"%.*s|%s\", (int)(pGach - pszKhoa), pszKhoa, pszGoc);",
                 "\tpszRa[nCo - 1] = 0;",
                 "\treturn true;",
                 "}",
                 "static int	s_nTepNap = 0;				// so thu tu tep dang nap"], "bang goc")
    # buoc 1
    s = thay(s, ["\t\tpGach = strchr(s_Bang[i].szKhoa, '|');",
                 "\t\tif (pGach && strcmp(pGach + 1, \"Main\") != 0)",
                 "\t\t{",
                 "\t\t\tchar szCha[UITOADO_CO_KHOA];",
                 "\t\t\t_snprintf(szCha, sizeof(szCha), \"%.*s|Main\", (int)(pGach - s_Bang[i].szKhoa), s_Bang[i].szKhoa);",
                 "\t\t\tszCha[sizeof(szCha) - 1] = 0;",
                 "\t\t\tnCha = TimKhoa(szCha);",
                 "\t\t\tif (nCha == i)",
                 "\t\t\t\tnCha = -1;",
                 "\t\t}"],
                ["\t\t{",
                 "\t\t\tchar szCha[UITOADO_CO_KHOA];\t// %s ten muc goc theo lop (ban do nho: MiniMap)" % DAU,
                 "\t\t\tif (KhoaCuaCha(s_Bang[i].szKhoa, szCha, sizeof(szCha)))",
                 "\t\t\t\tnCha = TimKhoa(szCha);",
                 "\t\t\tif (nCha == i)",
                 "\t\t\t\tnCha = -1;",
                 "\t\t}"], "buoc 1")
    # buoc 2
    s = thay(s, ["\t\tpGach = strchr(s_Bang[i].szKhoa, '|');",
                 "\t\tif (pGach && strcmp(pGach + 1, \"Main\") != 0)",
                 "\t\t{",
                 "\t\t\tchar szCha[UITOADO_CO_KHOA];",
                 "\t\t\tint nCha;",
                 "\t\t\t_snprintf(szCha, sizeof(szCha), \"%.*s|Main\", (int)(pGach - s_Bang[i].szKhoa), s_Bang[i].szKhoa);",
                 "\t\t\tszCha[sizeof(szCha) - 1] = 0;",
                 "\t\t\tnCha = TimKhoa(szCha);",
                 "\t\t\tif (nCha >= 0 && nCha != i)"],
                ["\t\t{",
                 "\t\t\tchar szCha[UITOADO_CO_KHOA];\t// %s" % DAU,
                 "\t\t\tint nCha = -1;",
                 "\t\t\tif (KhoaCuaCha(s_Bang[i].szKhoa, szCha, sizeof(szCha)))",
                 "\t\t\t\tnCha = TimKhoa(szCha);",
                 "\t\t\tif (nCha >= 0 && nCha != i)"], "buoc 2")
    # doc dong Goc.<lop>=<muc>
    s = thay(s, ["\t\tif (strcmpi(p, \"ManHinh\") == 0)\t// [UITOADO 12/09 MANHINH] khung ve luc luu tep nay"],
                ["\t\tif (strncmpi(p, \"Goc.\", 4) == 0)\t// %s Goc.<lop>=<ten muc cua so goc>" % DAU,
                 "\t\t{",
                 "\t\t\tif (s_nSoGoc < UITOADO_MAX_GOC && pSo[0] && pSo[0][0])",
                 "\t\t\t{",
                 "\t\t\t\tint n;",
                 "\t\t\t\tstrncpy(s_szGocLop[s_nSoGoc], p + 4, 63);",
                 "\t\t\t\ts_szGocLop[s_nSoGoc][63] = 0;",
                 "\t\t\t\tstrncpy(s_szGocMuc[s_nSoGoc], pSo[0], 63);",
                 "\t\t\t\ts_szGocMuc[s_nSoGoc][63] = 0;",
                 "\t\t\t\tfor (n = (int)strlen(s_szGocMuc[s_nSoGoc]) - 1; n >= 0 && (s_szGocMuc[s_nSoGoc][n] == '\\r'",
                 "\t\t\t\t\t|| s_szGocMuc[s_nSoGoc][n] == '\\n' || s_szGocMuc[s_nSoGoc][n] == ' '); n--)",
                 "\t\t\t\t\ts_szGocMuc[s_nSoGoc][n] = 0;",
                 "\t\t\t\ts_nSoGoc++;",
                 "\t\t\t}",
                 "\t\t\tcontinue;",
                 "\t\t}",
                 "\t\tif (strcmpi(p, \"ManHinh\") == 0)\t// [UITOADO 12/09 MANHINH] khung ve luc luu tep nay"], "doc dong Goc")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi")
    ghi(T, s); print("da va:", T)
print("xong")
