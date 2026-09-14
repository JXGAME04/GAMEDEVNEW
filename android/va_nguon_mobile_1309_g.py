# -*- coding: utf-8 -*-
# [NENDO 13/09 b] Do nốt phần chưa đo của ghép nền (chỉ log, chỉ JX_MOBILE) sau log Fold 7 21:03-21:42 (bản 109132016):
#   [PGND] còn 429 lần x 17,6 ms, 349 lần ở nhánh XA, mà 'vẽ lên ảnh' chỉ 1,5 ms và [PGND-X] (phần GPU của ClearImageData) = 0 dòng
#   -> 16 ms nằm ở chỗ CHƯA đo: GetImage(ảnh đích) trong ClearImageData, hoặc phần còn lại của KScenePlaceRegionC::PrerenderGround.
#   * Core: [PGND-R] từng vùng (>= 3 ms) vào jx_paint.log: tổng = xoá (ClearImageData) + ghép (DrawPrimitivesOnImage), số ảnh, số lượt.
#   * Represent3: [PGND-X] thêm thời gian GetImage(ảnh đích), ngưỡng 3 ms.
# Chạy lại vô hại (chạy sau _f.py). Bản PC không đổi.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[NENDO 13/09 b]"
NL = "\r\n"


def doc(p):
    s = io.open(p, encoding="latin-1", newline="").read()
    assert NL in s, p
    return s


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def cao(s):
    return sum(1 for c in s if ord(c) >= 128)


def thay(s, cu, moi, ten, n=1):
    if s.count(cu) != n:
        raise SystemExit("%s: neo '%s...' thay %d lan, can %d" % (ten, cu.strip()[:60], s.count(cu), n))
    return s.replace(cu, moi)


def va(p, viec):
    s = doc(p)
    if DAU in s:
        print("da va roi:", p)
        return
    c0 = cao(s)
    s = viec(s)
    assert cao(s) == c0, p + ": byte cao doi"
    ghi(p, s)
    print("da va:", p)


def v_region(s):
    cu = ("\tm_pPrerenderGroundImg->GROUND_IMG_OK_FLAG = true;" + NL + NL
          + "\tg_pRepresent->ClearImageData(m_pPrerenderGroundImg->szImage," + NL
          + "\t\tm_pPrerenderGroundImg->uImage, m_pPrerenderGroundImg->nISPosition);" + NL)
    moi = ("\tm_pPrerenderGroundImg->GROUND_IMG_OK_FLAG = true;" + NL + NL
           + "#ifdef JX_MOBILE" + NL
           + "\tLARGE_INTEGER jxR0, jxR1, jxR2, jxRF; QueryPerformanceFrequency(&jxRF); QueryPerformanceCounter(&jxR0);\t// " + DAU + NL
           + "#endif" + NL
           + "\tg_pRepresent->ClearImageData(m_pPrerenderGroundImg->szImage," + NL
           + "\t\tm_pPrerenderGroundImg->uImage, m_pPrerenderGroundImg->nISPosition);" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\tQueryPerformanceCounter(&jxR1);\t// " + DAU + " xong xoa" + NL
           + "#endif" + NL)
    s = thay(s, cu, moi, "PrerenderGround xoa")
    cu = "\t\tJX_NEN_VE(nNum);\t// [GOMNEN 12/09]" + NL + "\t}" + NL + "\treturn true;" + NL + "}" + NL
    moi = ("\t\tJX_NEN_VE(nNum);\t// [GOMNEN 12/09]" + NL + "\t}" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\t{\t// " + DAU + " tung vung: tong = xoa (ClearImageData) + ghep (DrawPrimitivesOnImage); ghi >= 3 ms de tim 17 ms/vung XA tren Fold 7" + NL
           + "\t\textern int g_nCorePaintLog;" + NL
           + "\t\tQueryPerformanceCounter(&jxR2);" + NL
           + "\t\tconst double dR = jxRF.QuadPart ? 1000.0 / (double)jxRF.QuadPart : 0.0;" + NL
           + "\t\tconst double dTong = (double)(jxR2.QuadPart - jxR0.QuadPart) * dR, dXoa = (double)(jxR1.QuadPart - jxR0.QuadPart) * dR;" + NL
           + "\t\tif (g_nCorePaintLog > 0 && dTong >= 3.0)" + NL
           + "\t\t{" + NL
           + "\t\t\tFILE* pR = fopen(\"jx_paint.log\", \"a\");" + NL
           + "\t\t\tif (pR)" + NL
           + "\t\t\t{" + NL
           + "\t\t\t\tPOINT ptR = GetRegionIdx();" + NL
           + "\t\t\t\tfprintf(pR, \"[PGND-R] vung (%d,%d) %s: tong %.1f ms = xoa %.1f + ghep %.1f (%u luot, %u anh)\\n\", ptR.x, ptR.y, m_pPrerenderGroundImg->szImage, dTong, dXoa, g_dJxNenVeMs, g_uJxNenVeLan, uJxTong);" + NL
           + "\t\t\t\tfclose(pR);" + NL
           + "\t\t\t}" + NL
           + "\t\t}" + NL
           + "\t}" + NL
           + "#endif" + NL
           + "\treturn true;" + NL + "}" + NL)
    s = thay(s, cu, moi, "PrerenderGround cuoi")
    return s


def v_shell(s):
    cu = ("\tTextureResBmp* pBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(" + NL
          + "\t\tpszImage, uImage, nImagePosition, 0, ISI_T_BITMAP16);" + NL
          + "\tif (pBitmap)" + NL + "\t{" + NL
          + "#ifdef JX_MOBILE" + NL
          + "\t\t// [XOANEN 13/09] anh nen vung")
    moi = ("#ifdef JX_MOBILE" + NL
           + "\tLARGE_INTEGER jxG0, jxG1; QueryPerformanceCounter(&jxG0);\t// " + DAU + " do GetImage anh dich" + NL
           + "#endif" + NL
           + "\tTextureResBmp* pBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(" + NL
           + "\t\tpszImage, uImage, nImagePosition, 0, ISI_T_BITMAP16);" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\tQueryPerformanceCounter(&jxG1);" + NL
           + "#endif" + NL
           + "\tif (pBitmap)" + NL + "\t{" + NL
           + "#ifdef JX_MOBILE" + NL
           + "\t\t// [XOANEN 13/09] anh nen vung")
    s = thay(s, cu, moi, "ClearImageData GetImage")
    cu = "\t\t\tif (dXMs >= 4.0) Rep3Log(\"[PGND-X] xoa nen %s tren GPU: %.1f ms (%s)\", pszImage, dXMs, bXong ? \"xong\" : \"HONG -> memset\");" + NL
    moi = ("\t\t\tconst double dGMs = jxXF.QuadPart ? (double)(jxG1.QuadPart - jxG0.QuadPart) * 1000.0 / (double)jxXF.QuadPart : 0.0;\t// " + DAU + NL
           + "\t\t\tif (dXMs + dGMs >= 3.0) Rep3Log(\"[PGND-X] xoa nen %s tren GPU: %.1f ms (%s) | GetImage dich %.1f ms\", pszImage, dXMs, bXong ? \"xong\" : \"HONG -> memset\", dGMs);" + NL)
    s = thay(s, cu, moi, "PGND-X log")
    return s


va("Sources/Core/Src/Scene/KScenePlaceRegionC.cpp", v_region)
va("Sources/Represent/Represent3/KRepresentShell3.cpp", v_shell)
