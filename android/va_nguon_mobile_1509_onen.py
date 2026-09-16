# -*- coding: latin-1 -*-
r"""[ONEN 15/09] Ghi log NGAY TAI VONG VE TUNG O NEN - moi o mot dong.

VI SAO CAN: bo dem "bo" cua dong [PGND-V] chi dem MOT trong BA duong bo ve, nen suot dem qua toi
suy sai tu no. Ba duong:
  1. GetImage tra NULL                      -> dem vao "bo"          (KRepresentShell3.cpp:3035)
  2. nFrame >= so khung cua sprite           -> cung dem vao "bo"      (cung dong)
  3. sprite co, nhung TEXTURE cua khung rong -> KHONG dem vao "bo"    (RIO_CopySprToBufferAlpha,
     KRepresentShell3.cpp:5252-5257, chi tang g_uRep3FxTexNull roi return)
Dong [PGND-V] gop het lai thanh mot so, va no cung KHONG noi o NAO bi bo.

BAN NAY: khoa [Client] Rep3ONenLog=N -> ghi chi tiet N lan ghep nen dau tien, moi o mot dong:
  [ONEN] <anh dich> o <i>/<tong> tai <x>,<y> khung <k> : <ten anh> -> VE | BO GetImage NULL |
         BO khung <k> >= <so khung> | BO texture rong
Sau N lan thi tu tat, khong ton gi. Doi chieu voi dong [PGND-R] (co toa do vung) la biet o nao,
vung nao, roi so thang voi cho chu nhin thay den.

Rao #ifdef JX_MOBILE, ban PC bien dich y het.
Chay:  PYTHONIOENCODING=latin-1 python android\va_nguon_mobile_1509_onen.py
"""
import io
import os

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TAG = "[ONEN 15/09]"


def va(duong, cap):
    p = os.path.join(GOC, duong)
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    cao0 = sum(1 for c in s if ord(c) >= 128)
    lf0 = s.count("\n") - s.count("\r\n")
    if TAG in s:
        print("  bo qua (da co %s): %s" % (TAG, duong))
        return
    for cu, moi in cap:
        if s.count(cu) != 1:
            raise SystemExit("HONG: neo xuat hien %d lan trong %s:\n%s" % (s.count(cu), duong, cu[:180]))
        s = s.replace(cu, moi, 1)
    cao1 = sum(1 for c in s if ord(c) >= 128)
    lf1 = s.count("\n") - s.count("\r\n")
    if cao1 != cao0:
        raise SystemExit("HONG: %s byte cao %d -> %d" % (duong, cao0, cao1))
    if lf1 != lf0:
        raise SystemExit("HONG: %s co %d dong LF don (truoc %d)" % (duong, lf1, lf0))
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("  da va: %s" % duong)


print("[ONEN 15/09] log tung o nen")

va(r"Sources\Represent\Represent3\KRepresentShell3.cpp", [
    # --- 1. bien dem + khoa ini
    ("unsigned g_uRep3FxAnhNull = 0;",
     "#ifdef JX_MOBILE\r\n"
     "int g_nJxONenLog = 0;\t// [ONEN 15/09] [Client] Rep3ONenLog = so lan ghep nen con phai ghi chi tiet (tu giam ve 0)\r\n"
     "#endif\r\n"
     "unsigned g_uRep3FxAnhNull = 0;"),
    # --- 2. doc khoa ini
    ("\tg_nJxPsBuffer       = Rep3Ini(\"Rep3PsBuffer\", 1) ? 1 : 0;",
     "#ifdef JX_MOBILE\r\n"
     "\t{ extern int g_nJxONenLog; g_nJxONenLog = Rep3Ini(\"Rep3ONenLog\", 0); if (g_nJxONenLog < 0) g_nJxONenLog = 0; if (g_nJxONenLog > 64) g_nJxONenLog = 64; }\t// [ONEN 15/09]\r\n"
     "#endif\r\n"
     "\tg_nJxPsBuffer       = Rep3Ini(\"Rep3PsBuffer\", 1) ? 1 : 0;"),
    # --- 3. dau ham: quyet dinh lan ghep nay co ghi khong
    ("\tconst unsigned uJxNdNap0 = (unsigned)m_TextureResMgr.m_nLoadCount;\r\n",
     "\tconst unsigned uJxNdNap0 = (unsigned)m_TextureResMgr.m_nLoadCount;\r\n"
     "\textern int g_nJxONenLog;\t// [ONEN 15/09] ghi chi tiet tung o cho N lan ghep nen dau tien\r\n"
     "\tconst bool bJxONen = (g_nJxONenLog > 0 && uGenre == RU_T_IMAGE && pszImage && pszImage[0]);\r\n"
     "\tif (bJxONen) { g_nJxONenLog--; Rep3Log(\"[ONEN] === bat dau ghep %s: %d anh ===\", pszImage, nPrimitiveCount); }\r\n"),
    # --- 4. trong vong: ghi ket qua tung o
    ("\t\t\t\t\tif (pSprite == NULL || pTemp->nFrame >= pSprite->m_nFrameNum) { s_uJxNenBoLan++; break; }\r\n",
     "\t\t\t\t\tif (bJxONen)\r\n"
     "\t\t\t\t\t{\t// [ONEN 15/09] ghi TRUOC khi bo, de biet DUNG o nao va vi sao\r\n"
     "\t\t\t\t\t\tif (pSprite == NULL)\r\n"
     "\t\t\t\t\t\t\tRep3Log(\"[ONEN] o %d/%d tai %d,%d khung %d : %s -> BO GetImage NULL\", i, nPrimitiveCount, pTemp->oPosition.nX, pTemp->oPosition.nY, pTemp->nFrame, pTemp->szImage);\r\n"
     "\t\t\t\t\t\telse if (pTemp->nFrame >= pSprite->m_nFrameNum)\r\n"
     "\t\t\t\t\t\t\tRep3Log(\"[ONEN] o %d/%d tai %d,%d khung %d : %s -> BO khung %d >= so khung %d\", i, nPrimitiveCount, pTemp->oPosition.nX, pTemp->oPosition.nY, pTemp->nFrame, pTemp->szImage, pTemp->nFrame, pSprite->m_nFrameNum);\r\n"
     "\t\t\t\t\t}\r\n"
     "\t\t\t\t\tif (pSprite == NULL || pTemp->nFrame >= pSprite->m_nFrameNum) { s_uJxNenBoLan++; break; }\r\n"),
    # --- 5. quanh loi goi ve: bat truong hop texture rong (khong duoc dem vao "bo")
    ("\t\t\t\t\t\tQueryPerformanceCounter(&jxNd0); RIO_CopySprToBufferAlpha(pSprite, pTemp->nFrame, pDestBitmap, nX, nY); QueryPerformanceCounter(&jxNd1); s_dJxNenRioMs += (double)(jxNd1.QuadPart - jxNd0.QuadPart) * jxNdK;\t// [NENDO 13/09]\r\n",
     "\t\t\t\t\t\tQueryPerformanceCounter(&jxNd0);\r\n"
     "\t\t\t\t\t\t{\t// [ONEN 15/09] g_uRep3FxTexNull tang = RIO thoat vi texture rong -> o do DEN ma khong bi dem vao \"bo\"\r\n"
     "\t\t\t\t\t\t\tconst unsigned uJxTN0 = g_uRep3FxTexNull;\r\n"
     "\t\t\t\t\t\t\tRIO_CopySprToBufferAlpha(pSprite, pTemp->nFrame, pDestBitmap, nX, nY);\r\n"
     "\t\t\t\t\t\t\tif (bJxONen)\r\n"
     "\t\t\t\t\t\t\t\tRep3Log(\"[ONEN] o %d/%d tai %d,%d khung %d : %s -> %s\", i, nPrimitiveCount, pTemp->oPosition.nX, pTemp->oPosition.nY, pTemp->nFrame, pTemp->szImage,\r\n"
     "\t\t\t\t\t\t\t\t\t(g_uRep3FxTexNull != uJxTN0) ? \"BO texture rong\" : \"VE\");\r\n"
     "\t\t\t\t\t\t}\r\n"
     "\t\t\t\t\t\tQueryPerformanceCounter(&jxNd1); s_dJxNenRioMs += (double)(jxNd1.QuadPart - jxNd0.QuadPart) * jxNdK;\t// [NENDO 13/09]\r\n"),
])

print("xong.")
