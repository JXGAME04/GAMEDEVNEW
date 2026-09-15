# -*- coding: latin-1 -*-
r"""[TGPHA 14/09] Tach pha "the gioi" cua vong ve: ban than / con / anh em.

So sanh log 14/09 toi (iPhone 18,2 ban da bat KHOI vs Fold 7 ban 109141951), trung binh moi khung:

    pha "the gioi" (pGameSpaceWnd->Paint)   iOS 12,92 ms   Android  5,51 ms
      trong do KScenePlaceC::Paint (canh)   iOS  4,04 ms   Android  3,73 ms   <- NHU NHAU
      => phan NGOAI canh                    iOS  8,88 ms   Android  1,78 ms   <- lech 5 lan

Phan "ngoai canh" chua duoc do bao gio. KWndWindow::Paint cua cua so the gioi goi ba thu:
PaintWindow() cua chinh no (DrawGameSpace + tim muc tieu duoi con tro), CAC O CON, va CAC O ANH EM
(Paint() de quy sang m_pNextWnd). Ban nay cam ba dong ho rieng cho ba phan do, in vao dong [PDET]:

    | the gioi: ban than %.1f, con %.1f, anh em %.1f

Chi chay khi g_nPaintLog > 0, va chi cho DUNG cua so the gioi (g_pJxWndTheGioi) nen khong ton gi
o cac cua so khac. Khong doi hanh vi ve.

Chay:  PYTHONIOENCODING=latin-1 python android\va_nguon_mobile_1409_s.py
"""
import io
import os

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TAG = "[TGPHA 14/09]"


def va(duong, cap, tag):
    p = os.path.join(GOC, duong)
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    cao0 = sum(1 for c in s if ord(c) >= 128)
    lf0 = s.count("\n") - s.count("\r\n")
    if tag in s:
        print("  bo qua (da co %s): %s" % (tag, duong))
        return
    for cu, moi in cap:
        if cu not in s:
            raise SystemExit("HONG: khong thay neo trong %s:\n%s" % (duong, cu[:200]))
        if s.count(cu) != 1:
            raise SystemExit("HONG: neo xuat hien %d lan trong %s" % (s.count(cu), duong))
        s = s.replace(cu, moi, 1)
    cao1 = sum(1 for c in s if ord(c) >= 128)
    lf1 = s.count("\n") - s.count("\r\n")
    if cao1 != cao0:
        raise SystemExit("HONG: %s byte cao %d -> %d" % (duong, cao0, cao1))
    if lf1 != lf0:
        raise SystemExit("HONG: %s co %d dong LF don (truoc %d)" % (duong, lf1, lf0))
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("  da va: %s" % duong)


print("[TGPHA] ba dong ho cho cua so the gioi")

# --- 1. Wnds.cpp: bien toan cuc + tro toi cua so the gioi
va(r"Sources\S3Client\Ui\Elem\Wnds.cpp", [
    ("const char* g_pszJxUiMaxLop = NULL; const void* g_pJxUiMax = NULL; char g_szJxUiMaxTen[64] = { 0 };",
     "const char* g_pszJxUiMaxLop = NULL; const void* g_pJxUiMax = NULL; char g_szJxUiMaxTen[64] = { 0 };\r\n"
     "// [TGPHA 14/09] tach pha cua so THE GIOI: 0 = ban than (DrawGameSpace + tim muc tieu), 1 = cac o con, 2 = cac o anh em.\r\n"
     "// Do log 14/09: pha the gioi iOS 12,92 ms / Android 5,51 ms trong khi KScenePlaceC::Paint hai ben nhu nhau (4,04 / 3,73)\r\n"
     "// -> phan lech nam NGOAI canh va chua duoc do bao gio.\r\n"
     "double g_dJxTgPha[3] = { 0.0, 0.0, 0.0 };\r\n"
     "void*  g_pJxWndTheGioi = NULL;"),
    ("\ts_WndStation.pGameSpaceWnd = NULL;",
     "\ts_WndStation.pGameSpaceWnd = NULL;\r\n"
     "\t{ extern void* g_pJxWndTheGioi; g_pJxWndTheGioi = NULL; }\t// [TGPHA 14/09]"),
    ("\ts_WndStation.pGameSpaceWnd = pWnd;",
     "\ts_WndStation.pGameSpaceWnd = pWnd;\r\n"
     "\t{ extern void* g_pJxWndTheGioi; g_pJxWndTheGioi = (void*)pWnd; }\t// [TGPHA 14/09]"),
    ("\t\tg_dJxPhaVe[0] = JxPhaMs(jxT[0], jxT[1]);",
     "\t\tg_dJxPhaVe[0] = JxPhaMs(jxT[0], jxT[1]);"),
], TAG)

# --- 2. WndWindow.cpp: do ba phan khi this == cua so the gioi
va(r"Sources\S3Client\Ui\Elem\WndWindow.cpp", [
    ("\t\tLARGE_INTEGER jxU0; if (bJxDoUi) QueryPerformanceCounter(&jxU0);\r\n"
     "#endif\r\n"
     "\t\tPaintWindow();\r\n"
     "\t\tif (m_pFirstChild)\r\n"
     "\t\t\tm_pFirstChild->Paint();",
     "\t\tLARGE_INTEGER jxU0; if (bJxDoUi) QueryPerformanceCounter(&jxU0);\r\n"
     "\t\t// [TGPHA 14/09] rieng cua so THE GIOI: tach ban than / con / anh em (phan lech iOS-Android nam o day)\r\n"
     "\t\textern void* g_pJxWndTheGioi; extern double g_dJxTgPha[3];\r\n"
     "\t\tconst bool bJxTg = (g_nPaintLog > 0 && g_pJxWndTheGioi == (void*)this);\r\n"
     "\t\tLARGE_INTEGER jxG[4]; if (bJxTg) QueryPerformanceCounter(&jxG[0]);\r\n"
     "#endif\r\n"
     "\t\tPaintWindow();\r\n"
     "#ifdef JX_MOBILE\r\n"
     "\t\tif (bJxTg) QueryPerformanceCounter(&jxG[1]);\t// [TGPHA 14/09] xong ban than\r\n"
     "#endif\r\n"
     "\t\tif (m_pFirstChild)\r\n"
     "\t\t\tm_pFirstChild->Paint();\r\n"
     "#ifdef JX_MOBILE\r\n"
     "\t\tif (bJxTg)\r\n"
     "\t\t{\t// [TGPHA 14/09] xong cac o con; anh em do o duoi (sau khoi #endif cua PDET-UI)\r\n"
     "\t\t\tLARGE_INTEGER jxGF; QueryPerformanceFrequency(&jxGF);\r\n"
     "\t\t\tQueryPerformanceCounter(&jxG[2]);\r\n"
     "\t\t\tconst double dK = jxGF.QuadPart ? 1000.0 / (double)jxGF.QuadPart : 0.0;\r\n"
     "\t\t\tg_dJxTgPha[0] = (double)(jxG[1].QuadPart - jxG[0].QuadPart) * dK;\r\n"
     "\t\t\tg_dJxTgPha[1] = (double)(jxG[2].QuadPart - jxG[1].QuadPart) * dK;\r\n"
     "\t\t\tg_dJxTgPha[2] = 0.0;\r\n"
     "\t\t}\r\n"
     "#endif"),
], TAG)

# --- 3. UiShell.cpp: in ba so vao dong [PDET]
va(r"Sources\S3Client\Ui\UiShell.cpp", [
    ('\t\t\t\t\t" | dau ham %.1f, CHO KHOA %.1f, tong Paint %.1f, chon muc tieu %.1f\\n",',
     '\t\t\t\t\t" | dau ham %.1f, CHO KHOA %.1f, tong Paint %.1f, chon muc tieu %.1f"\r\n'
     '\t\t\t\t\t" | the gioi: ban than %.1f, con %.1f\\n",\t// [TGPHA 14/09]'),
    ("\t\t\t\t\tg_dJxPhaCanh[7], g_dJxPhaCanh[8], g_dJxPhaCanh[9], g_dJxPhaCanh[10]);",
     "\t\t\t\t\tg_dJxPhaCanh[7], g_dJxPhaCanh[8], g_dJxPhaCanh[9], g_dJxPhaCanh[10],\r\n"
     "\t\t\t\t\tg_dJxTgPha[0], g_dJxTgPha[1]);\t// [TGPHA 14/09]"),
    ("\t\t\t\textern double g_dJxPhaCanh[11];",
     "\t\t\t\textern double g_dJxPhaCanh[11]; extern double g_dJxTgPha[3];\t// [TGPHA 14/09]"),
], TAG)

print("xong.")
