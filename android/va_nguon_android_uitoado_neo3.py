# -*- coding: utf-8 -*-
#
# [UITOADO 12/09 NEO] Bo cuc theo NEO (anchor) nhu engine mobile (Cocos/Unity): moi o co neo ngang L/C/R (0/1/2) va neo doc T/C/B
# (0/1/2) - chu 02:20: "dien thoai nhieu kich co, phai di chinh tung kich co khong hop ly; co cong nghe nao can doi khung hinh?".
#   * Dong bo cuc: <lop>|<muc> = Left,Top,TiLe,Co[,NeoX,NeoY]  (thieu neo -> tu suy: < 35 % rong = trai, >= 65 % = phai, giua = giua).
#   * Nap tep: toa do trong tep la cua khung ve 'ManHinh=W,H'; doi sang khung ve that: Left += (SW - W) * (neo - neo cha) / 2
#     (con cua cua so cha toan man hinh dich theo neo cua chinh no TRU neo cua cha, vi toa do con la tuong doi cha).
#   * Ghi tep: 6 truong + ManHinh = khung ve that (tep nguoi choi tren dien thoai tu nhat quan). Ban PC doc 4 truong dau nhu cu.
# Thay khoi [UITOADO 12/09 MANHINH] (chi neo phai/duoi theo nguong 70 %). Chi JX_ANDROID.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[UITOADO 12/09 NEO]"
TEP = "Sources/S3Client/Ui/Elem/UiToaDo.cpp"


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


s = doc(TEP)
if DAU in s:
    raise SystemExit("da va roi: " + TEP)
if "[UITOADO 12/09 MANHINH]" not in s:
    raise SystemExit("can va MANHINH (va_nguon_android_uitoado_manhinh1.py) truoc")
s0 = s

# 1. struct: neo
s = thay(s, ["\tint\t\tnTiLe;\t\t\t// phan nghin, 1000 = 100%", "\tint\t\tnCo;\t\t\t// bit UITOADO_CO_AN", "};"],
            ["\tint\t\tnTiLe;\t\t\t// phan nghin, 1000 = 100%", "\tint\t\tnCo;\t\t\t// bit UITOADO_CO_AN",
             "#ifdef JX_ANDROID",
             "\tint\t\tnNeoX;\t\t\t// %s 0 trai, 1 giua, 2 phai; -1 = chua biet (tu suy)" % DAU,
             "\tint\t\tnNeoY;\t\t\t// 0 tren, 1 giua, 2 duoi; -1",
             "#endif",
             "};"], "struct")

# 2. khoi MANHINH -> khoi NEO
s = thay(s, ["static int\ts_nManHinhW = 0, s_nManHinhH = 0;\t// doc tu tep dang nap",
             "static int\ts_nNeoTheoMep = -1;",
             "static void DoiTheoManHinh(int* pnX, int* pnY)",
             "{",
             "\tint nW0 = (s_nManHinhW > 0) ? s_nManHinhW : 1040;",
             "\tint nH0 = (s_nManHinhH > 0) ? s_nManHinhH : 604;",
             "\tint nDX = SCREEN_WIDTH - nW0, nDY = SCREEN_HEIGHT - nH0;",
             "\tif (s_nNeoTheoMep < 0)",
             "\t\ts_nNeoTheoMep = GetPrivateProfileInt(\"Ui\", \"NeoTheoMep\", 1, \".\\\\config.ini\") ? 1 : 0;",
             "\tif (!s_nNeoTheoMep || SCREEN_WIDTH <= 0 || SCREEN_HEIGHT <= 0 || (nDX == 0 && nDY == 0))",
             "\t\treturn;",
             "\tif (*pnX * 10 >= nW0 * 7)",
             "\t\t*pnX += nDX;",
             "\tif (*pnY * 10 >= nH0 * 7)",
             "\t\t*pnY += nDY;",
             "}"],
            ["static int\ts_nManHinhW = 0, s_nManHinhH = 0;\t// doc tu tep dang nap",
             "static int\ts_nNeoTheoMep = -1;",
             "// %s moi o co neo ngang (0 trai, 1 giua, 2 phai) va neo doc (0 tren, 1 giua, 2 duoi) - nhu anchor cua engine mobile." % DAU,
             "// Toa do trong tep la cua khung ve ManHinh=W,H; sang khung ve that: Left += (SW - W) * (neo - neo cha) / 2 (con cua cua so",
             "// cha toan man hinh dich theo neo cua no TRU neo cua cha vi toa do con tuong doi cha). Thieu neo -> tu suy theo vi tri.",
             "static int\ts_nTepNap = 0;\t\t\t\t// so thu tu tep dang nap",
             "static int\ts_aTepNap[UITOADO_MAX];\t\t// muc i duoc ghi boi tep nao",
             "static int NeoTuDong(int nV, int nToan)",
             "{",
             "\tif (nToan <= 0)",
             "\t\treturn 0;",
             "\tif (nV * 100 < nToan * 35)",
             "\t\treturn 0;",
             "\treturn (nV * 100 >= nToan * 65) ? 2 : 1;",
             "}",
             "static int TimKhoa(const char* pszKhoa);",
             "static void DoiCaTepTheoNeo()",
             "{",
             "\tint nW0 = (s_nManHinhW > 0) ? s_nManHinhW : 1040;",
             "\tint nH0 = (s_nManHinhH > 0) ? s_nManHinhH : 604;",
             "\tint nDX = SCREEN_WIDTH - nW0, nDY = SCREEN_HEIGHT - nH0;",
             "\tint i;",
             "\tif (s_nNeoTheoMep < 0)",
             "\t\ts_nNeoTheoMep = GetPrivateProfileInt(\"Ui\", \"NeoTheoMep\", 1, \".\\\\config.ini\") ? 1 : 0;",
             "\t// buoc 1: suy neo cho o chua co (theo toa do thiet ke)",
             "\tfor (i = 0; i < s_nSo; i++)",
             "\t{",
             "\t\tif (s_aTepNap[i] != s_nTepNap)",
             "\t\t\tcontinue;",
             "\t\tif (s_Bang[i].nNeoX < 0 || s_Bang[i].nNeoX > 2)",
             "\t\t\ts_Bang[i].nNeoX = NeoTuDong(s_Bang[i].nLeft, nW0);",
             "\t\tif (s_Bang[i].nNeoY < 0 || s_Bang[i].nNeoY > 2)",
             "\t\t\ts_Bang[i].nNeoY = NeoTuDong(s_Bang[i].nTop, nH0);",
             "\t}",
             "\tif (!s_nNeoTheoMep || SCREEN_WIDTH <= 0 || SCREEN_HEIGHT <= 0 || (nDX == 0 && nDY == 0))",
             "\t\treturn;",
             "\t// buoc 2: dich theo neo (tru neo cua cha neu cha co trong bang: '<lop>|Main')",
             "\tfor (i = 0; i < s_nSo; i++)",
             "\t{",
             "\t\tint nNeoXCha = 0, nNeoYCha = 0;",
             "\t\tconst char* pGach;",
             "\t\tif (s_aTepNap[i] != s_nTepNap)",
             "\t\t\tcontinue;",
             "\t\tpGach = strchr(s_Bang[i].szKhoa, '|');",
             "\t\tif (pGach && strcmp(pGach + 1, \"Main\") != 0)",
             "\t\t{",
             "\t\t\tchar szCha[UITOADO_CO_KHOA];",
             "\t\t\tint nCha;",
             "\t\t\t_snprintf(szCha, sizeof(szCha), \"%.*s|Main\", (int)(pGach - s_Bang[i].szKhoa), s_Bang[i].szKhoa);",
             "\t\t\tszCha[sizeof(szCha) - 1] = 0;",
             "\t\t\tnCha = TimKhoa(szCha);",
             "\t\t\tif (nCha >= 0 && nCha != i)",
             "\t\t\t{",
             "\t\t\t\tnNeoXCha = (s_Bang[nCha].nNeoX >= 0) ? s_Bang[nCha].nNeoX : NeoTuDong(s_Bang[nCha].nLeft, nW0);",
             "\t\t\t\tnNeoYCha = (s_Bang[nCha].nNeoY >= 0) ? s_Bang[nCha].nNeoY : NeoTuDong(s_Bang[nCha].nTop, nH0);",
             "\t\t\t}",
             "\t\t}",
             "\t\ts_Bang[i].nLeft += nDX * (s_Bang[i].nNeoX - nNeoXCha) / 2;",
             "\t\ts_Bang[i].nTop  += nDY * (s_Bang[i].nNeoY - nNeoYCha) / 2;",
             "\t}",
             "}"], "khoi NEO")

# 3. NapTep: 6 truong (Android), luu neo, doi ca tep sau khi doc
s = thay(s, ["\t\tchar*\tpSo[4];", "\t\tint\t\tnSoTruong = 0;", "\t\tint\t\tnGiaTri[4];"],
            ["#ifdef JX_ANDROID",
             "\t\tchar*\tpSo[6];\t\t\t\t// %s them NeoX, NeoY" % DAU,
             "\t\tint\t\tnSoTruong = 0;",
             "\t\tint\t\tnGiaTri[6];",
             "#else",
             "\t\tchar*\tpSo[4];", "\t\tint\t\tnSoTruong = 0;", "\t\tint\t\tnGiaTri[4];",
             "#endif"], "NapTep khai bao")
s = thay(s, ["\t\tfor (char* q = pBang + 1; *q && nSoTruong < 4; q++)"],
            ["#ifdef JX_ANDROID",
             "\t\tfor (char* q = pBang + 1; *q && nSoTruong < 6; q++)\t// %s" % DAU,
             "#else",
             "\t\tfor (char* q = pBang + 1; *q && nSoTruong < 4; q++)",
             "#endif"], "NapTep vong tach")
s = thay(s, ["\t\tfor (i = 0; i < 4; i++)", "\t\t\tnGiaTri[i] = (i < nSoTruong) ? atoi(pSo[i]) : 0;"],
            ["#ifdef JX_ANDROID",
             "\t\tfor (i = 0; i < 6; i++)\t// %s" % DAU,
             "#else",
             "\t\tfor (i = 0; i < 4; i++)",
             "#endif",
             "\t\t\tnGiaTri[i] = (i < nSoTruong) ? atoi(pSo[i]) : 0;"], "NapTep doc so")
s = thay(s, ["\t\tDoiTheoManHinh(&nGiaTri[0], &nGiaTri[1]);",
             "#endif",
             "\t\tDatKhoa(p, nGiaTri[0], nGiaTri[1], nGiaTri[2], nGiaTri[3]);"],
            ["\t\t{",
             "\t\t\tint nMuc = DatKhoa(p, nGiaTri[0], nGiaTri[1], nGiaTri[2], nGiaTri[3]);\t// %s" % DAU,
             "\t\t\tif (nMuc >= 0)",
             "\t\t\t{",
             "\t\t\t\ts_Bang[nMuc].nNeoX = (nSoTruong >= 5) ? nGiaTri[4] : -1;",
             "\t\t\t\ts_Bang[nMuc].nNeoY = (nSoTruong >= 6) ? nGiaTri[5] : -1;",
             "\t\t\t\ts_aTepNap[nMuc] = s_nTepNap;",
             "\t\t\t}",
             "\t\t}",
             "#else",
             "\t\tDatKhoa(p, nGiaTri[0], nGiaTri[1], nGiaTri[2], nGiaTri[3]);",
             "#endif"], "NapTep DatKhoa")
# dau NapTep: tang so thu tu tep (sau khi mo duoc tep)
s = thay(s, ["\tg_GetFullPath(szDuongDan, (char*)pszTep);", "\tpTep = fopen(szDuongDan, \"rt\");", "\tif (pTep == NULL)", "\t\treturn;"],
            ["\tg_GetFullPath(szDuongDan, (char*)pszTep);", "\tpTep = fopen(szDuongDan, \"rt\");", "\tif (pTep == NULL)", "\t\treturn;",
             "#ifdef JX_ANDROID",
             "\ts_nTepNap++;\t// %s" % DAU,
             "#endif"], "NapTep dau")
s = thay(s, ["\tg_DebugLog(\"[UITOADO] %s: tep thiet ke %dx%d, khung ve %dx%d\", \"[UITOADO 12/09 MANHINH]\", s_nManHinhW, s_nManHinhH, SCREEN_WIDTH, SCREEN_HEIGHT);",
             "\ts_nManHinhW = s_nManHinhH = 0;"],
            ["\tDoiCaTepTheoNeo();\t// %s" % DAU,
             "\tg_DebugLog(\"[UITOADO] %s: tep thiet ke %dx%d, khung ve %dx%d\", \"" + DAU + "\", s_nManHinhW, s_nManHinhH, SCREEN_WIDTH, SCREEN_HEIGHT);",
             "\ts_nManHinhW = s_nManHinhH = 0;"], "NapTep cuoi")

# 4. DatKhoa: muc moi -> neo chua biet
s = thay(s, ["\t\ti = s_nSo++;", "\t\tstrncpy(s_Bang[i].szKhoa, pszKhoa, UITOADO_CO_KHOA - 1);"],
            ["\t\ti = s_nSo++;",
             "#ifdef JX_ANDROID",
             "\t\ts_Bang[i].nNeoX = s_Bang[i].nNeoY = -1;\t// %s" % DAU,
             "\t\ts_aTepNap[i] = 0;",
             "#endif",
             "\t\tstrncpy(s_Bang[i].szKhoa, pszKhoa, UITOADO_CO_KHOA - 1);"], "DatKhoa")

# 5. GhiTepVao: 6 truong
s = thay(s, ["\t\tfprintf(pTep, \"%s=%d,%d,%d,%d\\n\", s_Bang[i].szKhoa,", "\t\t\ts_Bang[i].nLeft, s_Bang[i].nTop, s_Bang[i].nTiLe, s_Bang[i].nCo);"],
            ["#ifdef JX_ANDROID",
             "\t\tfprintf(pTep, \"%s=%d,%d,%d,%d,%d,%d\\n\", s_Bang[i].szKhoa,\t// " + DAU + " neo",
             "\t\t\ts_Bang[i].nLeft, s_Bang[i].nTop, s_Bang[i].nTiLe, s_Bang[i].nCo,",
             "\t\t\t(s_Bang[i].nNeoX >= 0) ? s_Bang[i].nNeoX : NeoTuDong(s_Bang[i].nLeft, SCREEN_WIDTH),",
             "\t\t\t(s_Bang[i].nNeoY >= 0) ? s_Bang[i].nNeoY : NeoTuDong(s_Bang[i].nTop, SCREEN_HEIGHT));",
             "#else",
             "\t\tfprintf(pTep, \"%s=%d,%d,%d,%d\\n\", s_Bang[i].szKhoa,", "\t\t\ts_Bang[i].nLeft, s_Bang[i].nTop, s_Bang[i].nTiLe, s_Bang[i].nCo);",
             "#endif"], "GhiTepVao")
s = thay(s, ["\tfprintf(pTep, \"; Moi dong:  <ten lop cua so>|<ten muc ini> = Left,Top,TiLe,Co\\n\");"],
            ["#ifdef JX_ANDROID",
             "\tfprintf(pTep, \"; Moi dong:  <ten lop cua so>|<ten muc ini> = Left,Top,TiLe,Co,NeoX,NeoY  (neo: 0 trai/tren, 1 giua, 2 phai/duoi)\\n\");",
             "#else",
             "\tfprintf(pTep, \"; Moi dong:  <ten lop cua so>|<ten muc ini> = Left,Top,TiLe,Co\\n\");",
             "#endif"], "GhiTepVao dau")

if cao(s) != cao(s0):
    raise SystemExit("so byte cao doi")
ghi(TEP, s)
print("da va:", TEP)
