# -*- coding: utf-8 -*-
# [NENDO 13/09] DO CHI TIET ghep nen vung (chi log, chi JX_MOBILE): 18 ms/vung trong [PGND] nam o dau?
#   DrawPrimitivesOnImage: GetImage (tim / nap dong bo) + RIO_CopySprToBufferAlpha (ghi lenh ve) + doi dich ve.
#   Ghi [PGND-V] (>= 4 ms) vao jx_rep3.log ngay trong Represent3 - KHONG dung bien toan cuc cheo .so
#   (libCoreClient.so khong lien ket libRepresent3.so: nap bang dlopen -> undefined symbol, dinh 18:54).
# Chay lai vo hai. Ban PC khong doi (moi dong trong #ifdef JX_MOBILE).
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[NENDO 13/09]"
NL = "\r\n"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def thay(s, cu, moi, ten, n=1):
    if s.count(cu) != n:
        raise SystemExit("%s: neo '%s...' thay %d lan, can %d" % (ten, cu.strip()[:60], s.count(cu), n))
    return s.replace(cu, moi)


def cao(s):
    return sum(1 for c in s if ord(c) >= 128)


R = "Sources/Represent/Represent3/KRepresentShell3.cpp"
s = doc(R)
if DAU in s:
    print("da va roi:", R)
else:
    assert NL in s
    c0 = cao(s)
    # 1. bien tinh trong tep (khong xuat khoi .so)
    a = "unsigned g_uJxKhoiSo = 0, g_uJxKhoiMB = 0, g_uJxKhoiHet = 0;"
    i = s.find(a)
    assert i >= 0 and s.count(a) == 1, "khong thay g_uJxKhoiSo"
    j = s.find(NL, i) + len(NL)
    s = (s[:j] + "#ifdef JX_MOBILE" + NL
         + "// " + DAU + " do chi tiet DrawPrimitivesOnImage (ghep nen vung): GetImage (tim/nap) vs RIO (ghi lenh ve) vs doi dich ve; nap = so anh nap dong bo. Chi trong .so nay." + NL
         + "static double s_dJxNenGetMs = 0.0, s_dJxNenRioMs = 0.0, s_dJxNenRtMs = 0.0; static unsigned s_uJxNenGetLan = 0, s_uJxNenNapLan = 0, s_uJxNenBoLan = 0;" + NL
         + "#endif" + NL + s[j:])
    # 2. dau ham: bien do + reset
    cu = "\tRep3NapDongBo napDongBo(m_TextureResMgr);\t// [NAP 08/09 d] ghep/ghi anh mot lan -> nap dong bo" + NL + "\tif(!pPrimitives)" + NL
    moi = ("\tRep3NapDongBo napDongBo(m_TextureResMgr);\t// [NAP 08/09 d] ghep/ghi anh mot lan -> nap dong bo" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\t// " + DAU + " do chi tiet: GetImage / RIO / doi dich ve -> [PGND-V] trong jx_rep3.log" + NL
           + "\tLARGE_INTEGER jxNd0, jxNd1, jxNdT0, jxNdF; QueryPerformanceFrequency(&jxNdF); QueryPerformanceCounter(&jxNdT0); const double jxNdK = jxNdF.QuadPart ? 1000.0 / (double)jxNdF.QuadPart : 0.0;" + NL
           + "\ts_dJxNenGetMs = 0.0; s_dJxNenRioMs = 0.0; s_dJxNenRtMs = 0.0; s_uJxNenGetLan = 0; s_uJxNenBoLan = 0; s_uJxNenNapLan = 0;" + NL
           + "\tconst unsigned uJxNdNap0 = (unsigned)m_TextureResMgr.m_nLoadCount;" + NL
           + "#endif" + NL
           + "\tif(!pPrimitives)" + NL)
    s = thay(s, cu, moi, "dau ham")
    # 3. doi dich ve: tu GetImage(dich) toi truoc vong lap
    cu = "\tTextureResBmp* pDestBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(" + NL + "\t\tpszImage, uImage, nImagePosition, 0, ISI_T_BITMAP16);" + NL
    moi = "#ifdef JX_MOBILE" + NL + "\tQueryPerformanceCounter(&jxNd0);\t// " + DAU + NL + "#endif" + NL + cu
    s = thay(s, cu, moi, "GetImage dich")
    cu = "\tint i;" + NL + "\tswitch(uGenre)" + NL + "\t{" + NL + "\tcase RU_T_IMAGE:" + NL
    moi = ("#ifdef JX_MOBILE" + NL
           + "\tQueryPerformanceCounter(&jxNd1); s_dJxNenRtMs += (double)(jxNd1.QuadPart - jxNd0.QuadPart) * jxNdK;\t// " + DAU + " doi dich ve" + NL
           + "#endif" + NL + cu)
    s = thay(s, cu, moi, "switch uGenre")
    # 4. GetImage tung anh
    cu = ("\t\t\t\t\tTextureResSpr* pSprite = (TextureResSpr *)m_TextureResMgr.GetImage(" + NL
          + "\t\t\t\t\t\tpTemp->szImage, pTemp->uImage," + NL
          + "\t\t\t\t\t\tpTemp->nISPosition, pTemp->nFrame, pTemp->nType);" + NL
          + "\t\t\t\t\tif (pSprite == NULL || pTemp->nFrame >= pSprite->m_nFrameNum)" + NL
          + "\t\t\t\t\t\tbreak;" + NL)
    moi = ("#ifdef JX_MOBILE" + NL + "\t\t\t\t\tQueryPerformanceCounter(&jxNd0);\t// " + DAU + NL + "#endif" + NL
           + "\t\t\t\t\tTextureResSpr* pSprite = (TextureResSpr *)m_TextureResMgr.GetImage(" + NL
           + "\t\t\t\t\t\tpTemp->szImage, pTemp->uImage," + NL
           + "\t\t\t\t\t\tpTemp->nISPosition, pTemp->nFrame, pTemp->nType);" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\t\t\t\t\tQueryPerformanceCounter(&jxNd1); s_dJxNenGetMs += (double)(jxNd1.QuadPart - jxNd0.QuadPart) * jxNdK; s_uJxNenGetLan++;\t// " + DAU + NL
           + "\t\t\t\t\tif (pSprite == NULL || pTemp->nFrame >= pSprite->m_nFrameNum) { s_uJxNenBoLan++; break; }" + NL
           + "#else" + NL
           + "\t\t\t\t\tif (pSprite == NULL || pTemp->nFrame >= pSprite->m_nFrameNum)" + NL
           + "\t\t\t\t\t\tbreak;" + NL
           + "#endif" + NL)
    s = thay(s, cu, moi, "GetImage anh")
    # 5. RIO
    cu = "\t\t\t\t\t\tRIO_CopySprToBufferAlpha(pSprite, pTemp->nFrame, pDestBitmap, nX, nY);" + NL
    moi = ("#ifdef JX_MOBILE" + NL
           + "\t\t\t\t\t\tQueryPerformanceCounter(&jxNd0); RIO_CopySprToBufferAlpha(pSprite, pTemp->nFrame, pDestBitmap, nX, nY); QueryPerformanceCounter(&jxNd1); s_dJxNenRioMs += (double)(jxNd1.QuadPart - jxNd0.QuadPart) * jxNdK;\t// " + DAU + NL
           + "#else" + NL + cu + "#endif" + NL)
    s = thay(s, cu, moi, "RIO")
    # 6. cuoi ham: tra dich ve + dem nap + ghi log
    cu = "\tPD3DDEVICE->SetRenderTarget( 0, pOldSurface );" + NL + "\tpDesSurface->Release();" + NL + "\tpOldSurface->Release();" + NL + "}" + NL
    moi = ("#ifdef JX_MOBILE" + NL + "\tQueryPerformanceCounter(&jxNd0);\t// " + DAU + NL + "#endif" + NL
           + "\tPD3DDEVICE->SetRenderTarget( 0, pOldSurface );" + NL + "\tpDesSurface->Release();" + NL + "\tpOldSurface->Release();" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\tQueryPerformanceCounter(&jxNd1); s_dJxNenRtMs += (double)(jxNd1.QuadPart - jxNd0.QuadPart) * jxNdK; s_uJxNenNapLan = (unsigned)m_TextureResMgr.m_nLoadCount - uJxNdNap0;\t// " + DAU + NL
           + "\t{\t// " + DAU + " ghi khi >= 4 ms: 've len anh' cua mot vung = GetImage (tim/nap) + RIO (ghi lenh ve) + doi dich ve" + NL
           + "\t\tconst double dNdTong = (double)(jxNd1.QuadPart - jxNdT0.QuadPart) * jxNdK;" + NL
           + "\t\tif (dNdTong >= 4.0)" + NL
           + "\t\t\tRep3Log(\"[PGND-V] %s: %d anh, %.1f ms = GetImage %.1f (%u lan, nap dong bo %u, bo %u) + RIO %.1f + doi dich %.1f\", pszImage, nPrimitiveCount, dNdTong," + NL
           + "\t\t\t\ts_dJxNenGetMs, s_uJxNenGetLan, s_uJxNenNapLan, s_uJxNenBoLan, s_dJxNenRioMs, s_dJxNenRtMs);" + NL
           + "\t}" + NL
           + "#endif" + NL + "}" + NL)
    s = thay(s, cu, moi, "tra dich ve")
    assert cao(s) == c0
    ghi(R, s)
    print("da va:", R)
