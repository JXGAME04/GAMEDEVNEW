# -*- coding: latin-1 -*-
r"""[UITEN][VAOMAP 14/09] Hai muc DO cho vong sau, theo phan tich log Fold 7 phien 15:40 (60 phut):

  1. [UITEN] [PDET-UI] moi in duoc toa do + con tro cua so nang nhat ("tai (-152,44) 298x632 lop phu -"),
     khong biet la BANG NAO. KWndWindow da nho ten muc ini trong m_szMucIni ([UITOADO], WndWindow.cpp:545)
     -> chep ten do vao bien toan cuc de in kem. Sau ban nay, mot dong [PDET-UI] du de truy thang ra cua so.

  2. [VAOMAP] doi map dong bang 340 ms trong MOT goi s2c_syncworld (15 lan/gio, tong 2,8 giay; [MANG-CHAM]
     + [LOGIC-PHA] do duoc, nhung khong biet 340 ms nam o dau trong KSubWorld::LoadMap). Cam dong ho vao
     TAM pha cua LoadMap(nId, nRegion) -> mot dong [VAOMAP] moi lan doi map (>= 30 ms) trong jx_paint.log:
       dong (Close + tra khe NPC) | mo map (OpenPlace + LoadSymbol) | ini (.wor) | vung giua | 8 vung ke
       | trang tri (LoadObject) | noi vung | luoi duong (khoi .fp: doc tep hoac dung lai + ghi)
     Khong doi hanh vi: chi QueryPerformanceCounter + mot dong log khi g_nCorePaintLog > 0.

Rao: ca hai deu trong #ifdef JX_MOBILE don (ca ham LoadMap da nam trong #ifndef _SERVER cua tep; bo kiem
tuong duong chi hieu #ifdef/#ifndef DON nen khong duoc viet #if defined(A) && !defined(B)).
Chay:  PYTHONIOENCODING=latin-1 python android\va_nguon_mobile_1409_o.py
"""
import io
import os
import sys

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def ghi(p, s, cao0, lf0):
    cao1 = sum(1 for c in s if ord(c) >= 128)
    lf1 = s.count("\n") - s.count("\r\n")
    if cao1 != cao0:
        raise SystemExit("HONG: %s byte cao %d -> %d" % (p, cao0, cao1))
    if lf1 != lf0:
        raise SystemExit("HONG: %s co %d dong LF don (truoc %d) - phai giu CRLF" % (p, lf1, lf0))
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def va(duong, cap, tag):
    """cap = danh sach (cu, moi). Bo qua neu tag da co trong tep."""
    p = os.path.join(GOC, duong)
    s = doc(p)
    cao0 = sum(1 for c in s if ord(c) >= 128)
    lf0 = s.count("\n") - s.count("\r\n")
    if tag in s:
        print("  bo qua (da co %s): %s" % (tag, duong))
        return
    for cu, moi in cap:
        if cu not in s:
            raise SystemExit("HONG: khong thay neo trong %s:\n%s" % (duong, cu[:160]))
        if s.count(cu) != 1:
            raise SystemExit("HONG: neo xuat hien %d lan trong %s" % (s.count(cu), duong))
        s = s.replace(cu, moi, 1)
    ghi(p, s, cao0, lf0)
    print("  da va: %s" % duong)


# ---------------------------------------------------------------- 1. [UITEN]
print("[UITEN] ten muc ini cua cua so nang nhat vao dong [PDET-UI]")

va(r"Sources\S3Client\Ui\Elem\Wnds.cpp", [
    ("const char* g_pszJxUiMaxLop = NULL; const void* g_pJxUiMax = NULL;",
     "const char* g_pszJxUiMaxLop = NULL; const void* g_pJxUiMax = NULL; char g_szJxUiMaxTen[64] = { 0 };\t// [UITEN 14/09] ten muc ini (m_szMucIni) de biet BANG nao"),
    ("\tg_dJxUiMaxMs = 0.0; g_pJxUiMax = NULL; g_pszJxUiMaxLop = NULL;\t// [PDET-UI 14/09]",
     "\tg_dJxUiMaxMs = 0.0; g_pJxUiMax = NULL; g_pszJxUiMaxLop = NULL; g_szJxUiMaxTen[0] = 0;\t// [PDET-UI 14/09] [UITEN 14/09]"),
], "[UITEN 14/09]")

va(r"Sources\S3Client\Ui\Elem\WndWindow.cpp", [
    ("extern const char* g_pszJxUiMaxLop; extern const void* g_pJxUiMax;",
     "extern const char* g_pszJxUiMaxLop; extern const void* g_pJxUiMax; extern char g_szJxUiMaxTen[64];\t// [UITEN 14/09]"),
    ("g_nJxUiMaxW = m_Width; g_nJxUiMaxH = m_Height; g_pszJxUiMaxLop = UiTenLopPhu(); }",
     "g_nJxUiMaxW = m_Width; g_nJxUiMaxH = m_Height; g_pszJxUiMaxLop = UiTenLopPhu();\r\n"
     "\t\t\t\t// [UITEN 14/09] chep (khong giu con tro) vi cua so co the bi huy truoc luc UiShell in dong [PDET-UI]\r\n"
     "\t\t\t\tstrncpy(g_szJxUiMaxTen, GetMucIni(), sizeof(g_szJxUiMaxTen) - 1); g_szJxUiMaxTen[sizeof(g_szJxUiMaxTen) - 1] = 0; }"),
], "[UITEN 14/09]")

va(r"Sources\S3Client\Ui\UiShell.cpp", [
    ("extern const char* g_pszJxUiMaxLop; extern const void* g_pJxUiMax;",
     "extern const char* g_pszJxUiMaxLop; extern const void* g_pJxUiMax; extern char g_szJxUiMaxTen[64];\t// [UITEN 14/09]"),
    ('fprintf(pLog, "[PDET-UI] cua so goc nang nhat %.1f ms: tai (%d,%d) %dx%d lop phu %s con tro %p\\n", g_dJxUiMaxMs, g_nJxUiMaxX, g_nJxUiMaxY, g_nJxUiMaxW, g_nJxUiMaxH, g_pszJxUiMaxLop ? g_pszJxUiMaxLop : "-", g_pJxUiMax);',
     'fprintf(pLog, "[PDET-UI] cua so goc nang nhat %.1f ms: muc ini [%s] tai (%d,%d) %dx%d lop phu %s con tro %p\\n", g_dJxUiMaxMs, g_szJxUiMaxTen[0] ? g_szJxUiMaxTen : "?", g_nJxUiMaxX, g_nJxUiMaxY, g_nJxUiMaxW, g_nJxUiMaxH, g_pszJxUiMaxLop ? g_pszJxUiMaxLop : "-", g_pJxUiMax);\t// [UITEN 14/09] ten muc ini'),
], "[UITEN 14/09]")

# ---------------------------------------------------------------- 2. [VAOMAP]
print("[VAOMAP] tam pha cua KSubWorld::LoadMap(nId, nRegion)")

MOC = ("#ifdef JX_MOBILE\r\n"
       "\tJX_VAOMAP_MOC(%d);\t// [VAOMAP 14/09]\r\n"
       "#endif\r\n")

va(r"Sources\Core\Src\KSubWorld.cpp", [
    # a. khai bao macro + bo dem, dat ngay truoc ham
    ("BOOL KSubWorld::LoadMap(int nId, int nRegion)\r\n{\r\n\t//g_DebugLog(\"LoadMap begin\");",
     "#ifdef JX_MOBILE\r\n"
     "// [VAOMAP 14/09] Doi map dong bang ~340 ms trong MOT goi s2c_syncworld (log Fold 7 15:40: 15 lan/gio, tong 2,8 s;\r\n"
     "// ngan sach mang khong cat duoc BEN TRONG mot goi). Cam dong ho vao tung pha de vong sau biet cat cho nao.\r\n"
     "static LARGE_INTEGER s_liJxVaoMap[9];\r\n"
     "static int s_nJxVaoMapSo = 0;\r\n"
     "#define JX_VAOMAP_MOC(n) do { if (s_nJxVaoMapSo > 0 && (n) < 9) { QueryPerformanceCounter(&s_liJxVaoMap[(n)]); if ((n) + 1 > s_nJxVaoMapSo) s_nJxVaoMapSo = (n) + 1; } } while (0)\r\n"
     "static double JxVaoMapMs(int a, int b)\r\n"
     "{\r\n"
     "\tstatic double s_dF = 0.0;\r\n"
     "\tif (s_dF == 0.0) { LARGE_INTEGER f; QueryPerformanceFrequency(&f); s_dF = (double)f.QuadPart; }\r\n"
     "\tif (s_dF <= 0.0 || a >= s_nJxVaoMapSo || b >= s_nJxVaoMapSo) return 0.0;\r\n"
     "\treturn (double)(s_liJxVaoMap[b].QuadPart - s_liJxVaoMap[a].QuadPart) * 1000.0 / s_dF;\r\n"
     "}\r\n"
     "#endif\r\n"
     "BOOL KSubWorld::LoadMap(int nId, int nRegion)\r\n{\r\n\t//g_DebugLog(\"LoadMap begin\");\r\n"
     "#ifdef JX_MOBILE\r\n"
     "\t{ extern int g_nCorePaintLog; s_nJxVaoMapSo = (g_nCorePaintLog > 0) ? 1 : 0; }\t// [VAOMAP 14/09] chi do khi bat nhat ky ve\r\n"
     "\tJX_VAOMAP_MOC(0);\r\n"
     "#endif"),
    # b. sau khi dong the gioi cu + tra khe NPC (truoc OpenPlace)
    ("\t\tg_ScenePlace.OpenPlace(nId);\r\n\t\tm_SubWorldID = nId;\r\n\t\tg_ScenePlace.LoadSymbol(nId);//#maptraffic",
     (MOC % 1) +
     "\t\tg_ScenePlace.OpenPlace(nId);\r\n\t\tm_SubWorldID = nId;\r\n\t\tg_ScenePlace.LoadSymbol(nId);//#maptraffic\r\n"
     "#ifdef JX_MOBILE\r\n\t\tJX_VAOMAP_MOC(2);\t// [VAOMAP 14/09]\r\n#endif"),
    # c. sau khi doc MapList.ini + .wor  (het khoi bLoadNew)
    ("\t\t\tm_nGridTotal = m_nGridW * m_nGridH;\r\n\t\t}\r\n\t}\r\n\tint nX = LOWORD(nRegion);",
     "\t\t\tm_nGridTotal = m_nGridW * m_nGridH;\r\n\t\t}\r\n\t}\r\n"
     "#ifdef JX_MOBILE\r\n\tJX_VAOMAP_MOC(3);\t// [VAOMAP 14/09] xong ini + .wor\r\n#endif\r\n"
     "\tint nX = LOWORD(nRegion);"),
    # d. sau vung giua
    ("\tif (bLoadNew || !g_bPaintInterpFocus)\r\n\t\tg_ScenePlace.SetFocusPosition(",
     "#ifdef JX_MOBILE\r\n\tJX_VAOMAP_MOC(4);\t// [VAOMAP 14/09] xong vung giua\r\n#endif\r\n"
     "\tif (bLoadNew || !g_bPaintInterpFocus)\r\n\t\tg_ScenePlace.SetFocusPosition("),
    # e. sau 8 vung ke + sau trang tri
    ("\t// [TRANGTRI 11/09 e] gio moi nap vat trang tri: ca 9 vung deu co mat nen Mps2Map khong con tra -1.\r\n"
     "\tfor (int nT11 = 0; nT11 < nTTSo; nT11++)\r\n"
     "\t\tm_Region[nTTO[nT11]].LoadObject(0, nTTX[nT11], nTTY[nT11], m_szMapPath);",
     "#ifdef JX_MOBILE\r\n\tJX_VAOMAP_MOC(5);\t// [VAOMAP 14/09] xong 8 vung ke\r\n#endif\r\n"
     "\t// [TRANGTRI 11/09 e] gio moi nap vat trang tri: ca 9 vung deu co mat nen Mps2Map khong con tra -1.\r\n"
     "\tfor (int nT11 = 0; nT11 < nTTSo; nT11++)\r\n"
     "\t\tm_Region[nTTO[nT11]].LoadObject(0, nTTX[nT11], nTTY[nT11], m_szMapPath);\r\n"
     "#ifdef JX_MOBILE\r\n\tJX_VAOMAP_MOC(6);\t// [VAOMAP 14/09] xong trang tri\r\n#endif"),
    # f. truoc khoi luoi duong (.fp)
    ("\tif(bLoadNew && m_nGridTotal)\r\n\t{\r\n\t\tbool bLoadData = false;",
     "#ifdef JX_MOBILE\r\n\tJX_VAOMAP_MOC(7);\t// [VAOMAP 14/09] xong noi vung\r\n#endif\r\n"
     "\tif(bLoadNew && m_nGridTotal)\r\n\t{\r\n\t\tbool bLoadData = false;"),
    # g. cuoi ham: in mot dong
    ("\t//g_DebugLog(\"LoadMap end\");\r\n\treturn TRUE;\r\n}",
     "#ifdef JX_MOBILE\r\n"
     "\tJX_VAOMAP_MOC(8);\r\n"
     "\tif (s_nJxVaoMapSo >= 9)\r\n"
     "\t{\t// [VAOMAP 14/09] mot dong moi lan doi map / cuon vung, chi khi >= 30 ms\r\n"
     "\t\tconst double dTong = JxVaoMapMs(0, 8);\r\n"
     "\t\tif (dTong >= 30.0)\r\n"
     "\t\t{\r\n"
     "\t\t\tFILE* pVm = fopen(\"jx_paint.log\", \"a\");\r\n"
     "\t\t\tif (pVm)\r\n"
     "\t\t\t{\r\n"
     "\t\t\t\tfprintf(pVm, \"[VAOMAP] map %d vung (%d,%d) map_moi=%d: tong %.0f ms = dong %.0f + mo map %.0f + ini %.0f + vung giua %.0f + 8 vung ke %.0f + trang tri %.0f + noi vung %.0f + luoi duong %.0f\\n\",\r\n"
     "\t\t\t\t\tnId, LOWORD(nRegion), HIWORD(nRegion), (int)(bLoadNew ? 1 : 0), dTong,\r\n"
     "\t\t\t\t\tJxVaoMapMs(0, 1), JxVaoMapMs(1, 2), JxVaoMapMs(2, 3), JxVaoMapMs(3, 4),\r\n"
     "\t\t\t\t\tJxVaoMapMs(4, 5), JxVaoMapMs(5, 6), JxVaoMapMs(6, 7), JxVaoMapMs(7, 8));\r\n"
     "\t\t\t\tfclose(pVm);\r\n"
     "\t\t\t}\r\n"
     "\t\t}\r\n"
     "\t}\r\n"
     "#endif\r\n"
     "\t//g_DebugLog(\"LoadMap end\");\r\n\treturn TRUE;\r\n}"),
], "[VAOMAP 14/09]")

print("xong.")
