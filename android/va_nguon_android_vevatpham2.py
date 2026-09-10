# -*- coding: utf-8 -*-
#
# [VEVATPHAM 12/09 c] Anh vat pham van tran ra ngoai o du khung dich = dung o (do duoc: o 1081,206 36x36 ma anh ve
# 1075,190 ~41x50). Ly do: khung dich chi la mien nguon [0, nWidth] cua KHUNG ANH; anh that con co OFFSET rieng
# (nOffX/nOffY) va co the tran ra ngoai mien do. Nay hoi Represent kich co + offset khung anh
# (GetImageFrameParam) roi tinh khung dich sao cho HOP ANH THAT nam gon trong o, giu ti le, can giua.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[VEVATPHAM 12/09 c]"


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


K = "Sources/Core/Src/KItem.cpp"
s = doc(K)
if DAU not in s:
    s0 = s
    cu = ["\t\t// [VEVATPHAM 12/09] ve GON TRONG o: giu ti le anh goc (mon 1x2, 2x2... khong bi ep vuong), can giua o.",
          "\t\t// Lay thang khung o do CoreDrawGameObj truyen sang - KHONG dung oPosition vi no da bi cong",
          "\t\t// (+5,+5) cho o dung nhanh (pos_immediacy) va co the da can giua theo co goc.",
          "\t\tint nGocW = m_CommonAttrib.nWidth;",
          "\t\tint nGocH = m_CommonAttrib.nHeight;",
          "\t\tint nVeW, nVeH, nX0, nY0;",
          "\t\tif (nGocW < 1) nGocW = 1;",
          "\t\tif (nGocH < 1) nGocH = 1;",
          "\t\tnVeW = g_nJxVeVatPhamW;",
          "\t\tnVeH = g_nJxVeVatPhamH;",
          "\t\tif (nGocW * nVeH > nGocH * nVeW)",
          "\t\t\tnVeH = nVeW * nGocH / nGocW;",
          "\t\telse",
          "\t\t\tnVeW = nVeH * nGocW / nGocH;",
          "\t\tif (nVeW < 1) nVeW = 1;",
          "\t\tif (nVeH < 1) nVeH = 1;",
          "\t\tnX0 = g_nJxVeVatPhamX + (g_nJxVeVatPhamW - nVeW) / 2;",
          "\t\tnY0 = g_nJxVeVatPhamY + (g_nJxVeVatPhamH - nVeH) / 2;"]
    moi = ["\t\t// %s ve GON TRONG o, giu ti le, can giua." % DAU,
           "\t\t// Khung dich cua RU_T_IMAGE_STRETCH la mien nguon [0, nWidth] x [0, nHeight] cua KHUNG ANH, nhung",
           "\t\t// anh that con lech theo offset rieng cua khung (nOffX/nOffY) nen de tran ra ngoai o (do duoc: o",
           "\t\t// 36x36 ma binh thuoc ve ra 41x50, lech len 16 px). Hoi Represent co + offset khung anh roi tinh",
           "\t\t// khung dich sao cho HOP ANH (hop cua [0,co] va [offset, offset+co]) nam gon trong o.",
           "\t\tint nGocW = m_CommonAttrib.nWidth;",
           "\t\tint nGocH = m_CommonAttrib.nHeight;",
           "\t\tint nVeW, nVeH, nX0, nY0;",
           "\t\tKRPosition2 oLech, oCo;",
           "\t\tint nX1 = 0, nY1 = 0, nX2 = 0, nY2 = 0, nTiLe = 0;\t// hop anh trong he toa do nguon (phan nghin)",
           "\t\tif (nGocW < 1) nGocW = 1;",
           "\t\tif (nGocH < 1) nGocH = 1;",
           "\t\toLech.nX = oLech.nY = 0;",
           "\t\toCo.nX = oCo.nY = 0;",
           "\t\tif (g_pRepresent && g_pRepresent->GetImageFrameParam(m_Image.szImage, m_Image.nFrame, &oLech, &oCo, m_Image.nType)",
           "\t\t\t&& oCo.nX > 0 && oCo.nY > 0)",
           "\t\t{",
           "\t\t\tint nRong, nCao, nA, nB;",
           "\t\t\tnX1 = (oLech.nX < 0) ? oLech.nX : 0;\t// bao ca hai cach ve (co / khong ap offset)",
           "\t\t\tnY1 = (oLech.nY < 0) ? oLech.nY : 0;",
           "\t\t\tnX2 = (oLech.nX + oCo.nX > oCo.nX) ? (oLech.nX + oCo.nX) : oCo.nX;",
           "\t\t\tnY2 = (oLech.nY + oCo.nY > oCo.nY) ? (oLech.nY + oCo.nY) : oCo.nY;",
           "\t\t\tnRong = nX2 - nX1;",
           "\t\t\tnCao  = nY2 - nY1;",
           "\t\t\tif (nRong > 0 && nCao > 0)",
           "\t\t\t{",
           "\t\t\t\tnA = g_nJxVeVatPhamW * 1000 / nRong;",
           "\t\t\t\tnB = g_nJxVeVatPhamH * 1000 / nCao;",
           "\t\t\t\tnTiLe = (nA < nB) ? nA : nB;",
           "\t\t\t}",
           "\t\t}",
           "\t\tif (nTiLe > 0)",
           "\t\t{",
           "\t\t\t// khung dich = mien [0, co] sau khi phong; dat sao cho hop anh can giua trong o",
           "\t\t\tnVeW = oCo.nX * nTiLe / 1000;",
           "\t\t\tnVeH = oCo.nY * nTiLe / 1000;",
           "\t\t\tif (nVeW < 1) nVeW = 1;",
           "\t\t\tif (nVeH < 1) nVeH = 1;",
           "\t\t\tnX0 = g_nJxVeVatPhamX + (g_nJxVeVatPhamW - (nX2 - nX1) * nTiLe / 1000) / 2 - nX1 * nTiLe / 1000;",
           "\t\t\tnY0 = g_nJxVeVatPhamY + (g_nJxVeVatPhamH - (nY2 - nY1) * nTiLe / 1000) / 2 - nY1 * nTiLe / 1000;",
           "\t\t}",
           "\t\telse",
           "\t\t{",
           "\t\t\t// khong hoi duoc khung anh: giu cach cu (theo so O cua mon)",
           "\t\t\tnVeW = g_nJxVeVatPhamW;",
           "\t\t\tnVeH = g_nJxVeVatPhamH;",
           "\t\t\tif (nGocW * nVeH > nGocH * nVeW)",
           "\t\t\t\tnVeH = nVeW * nGocH / nGocW;",
           "\t\t\telse",
           "\t\t\t\tnVeW = nVeH * nGocW / nGocH;",
           "\t\t\tif (nVeW < 1) nVeW = 1;",
           "\t\t\tif (nVeH < 1) nVeH = 1;",
           "\t\t\tnX0 = g_nJxVeVatPhamX + (g_nJxVeVatPhamW - nVeW) / 2;",
           "\t\t\tnY0 = g_nJxVeVatPhamY + (g_nJxVeVatPhamH - nVeH) / 2;",
           "\t\t}"]
    s = thay(s, cu, moi, "tinh khung dich")
    s = thay(s, ["\t\t\tg_DebugLog(\"[VATPHAM-KEO] o %d,%d %dx%d | mon %dx%d o | ve %d,%d %dx%d | anh %s\", g_nJxVeVatPhamX, g_nJxVeVatPhamY,",
                 "\t\t\t\tg_nJxVeVatPhamW, g_nJxVeVatPhamH, nGocW, nGocH, nX0, nY0, nVeW, nVeH, m_Image.szImage);"],
                ["\t\t\tg_DebugLog(\"[VATPHAM-KEO] o %d,%d %dx%d | khung anh %dx%d lech %d,%d | hop %d..%d,%d..%d ti le %d | ve %d,%d %dx%d | %s\",",
                 "\t\t\t\tg_nJxVeVatPhamX, g_nJxVeVatPhamY, g_nJxVeVatPhamW, g_nJxVeVatPhamH, oCo.nX, oCo.nY, oLech.nX, oLech.nY,",
                 "\t\t\t\tnX1, nX2, nY1, nY2, nTiLe, nX0, nY0, nVeW, nVeH, m_Image.szImage);"], "nhat ky")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi")
    ghi(K, s); print("da va:", K)
print("xong")
