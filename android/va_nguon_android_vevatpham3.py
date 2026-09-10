# -*- coding: utf-8 -*-
#
# [VEVATPHAM 12/09 e] Chu 10:20: "toa do dat vat pham dung roi, chi la HINH ANH no sai vi tri".
# Do duoc: o 36x36 tai 1081,206 nhung anh binh thuoc nam lech sang phai - duoi. Vi:
#   (1) ban goc can giua theo O GOC 26 px:  x += (36 - 26)/2 = +5;
#   (2) khi ve, Represent con cong OFFSET RIENG cua khung anh (obj-potion13: khung 16x28, lech 7,1) -> lech tiep +7,+1;
#   (3) hinh ve that con to hon khung anh khai bao nen phep KEO cho vua o lam anh tran ra ngoai (do duoc 41x50 trong o 36).
# Sua (chi Android): CoreDrawGameObj truyen ca KHUNG O cho PaintItem; PaintItem hoi Represent kich co + offset khung anh
# roi dat anh sao cho KHUNG ANH nam CHINH GIUA o - ve nguyen co (khong keo, khong meo). Muon keo cho day o thi bat
# config.ini [Ui] KeoAnhVatPham=1 (van giu ti le, va thu nho theo hop anh de khong tran).
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[VEVATPHAM 12/09 e]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def thay(s, cu, moi, ten):
    c = "\r\n".join(cu)
    if s.count(c) != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (s.count(c), ten))
    return s.replace(c, "\r\n".join(moi))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


# ---- CoreDrawGameObj: luon truyen khung o (khong con phu thuoc cong tac keo) ----
C = "Sources/Core/Src/CoreDrawGameObj.cpp"
s = doc(C)
if DAU not in s:
    s0 = s
    s = thay(s, ["\t\telse if (g_nJxKeoAnhVatPham && uObjGenre != CGOG_IME_ITEM && (nParam & 0x40000000) == 0",
                 "\t\t\t&& Width > Item[uId].GetWidth() * ITEM_CELL_WIDTH && Height > Item[uId].GetHeight() * ITEM_CELL_HEIGHT)"],
                ["\t\telse if (uObjGenre != CGOG_IME_ITEM && (nParam & 0x40000000) == 0",
                 "\t\t\t&& Width > Item[uId].GetWidth() * ITEM_CELL_WIDTH && Height > Item[uId].GetHeight() * ITEM_CELL_HEIGHT)"], "bo cong tac o dieu kien")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (CoreDrawGameObj)")
    ghi(C, s); print("da va:", C)

# ---- KItem::PaintItem: can giua theo KHUNG ANH THAT; keo chi khi bat cong tac ----
K = "Sources/Core/Src/KItem.cpp"
s = doc(K)
if DAU not in s:
    s0 = s
    s = thay(s, ["\t\tif (nTiLe > 0)",
                 "\t\t{",
                 "\t\t\t// khung dich = mien [0, co] sau khi phong; dat sao cho hop anh can giua trong o",
                 "\t\t\tnVeW = oCo.nX * nTiLe / 1000;",
                 "\t\t\tnVeH = oCo.nY * nTiLe / 1000;",
                 "\t\t\tif (nVeW < 1) nVeW = 1;",
                 "\t\t\tif (nVeH < 1) nVeH = 1;",
                 "\t\t\tnX0 = g_nJxVeVatPhamX + (g_nJxVeVatPhamW - (nX2 - nX1) * nTiLe / 1000) / 2 - nX1 * nTiLe / 1000;",
                 "\t\t\tnY0 = g_nJxVeVatPhamY + (g_nJxVeVatPhamH - (nY2 - nY1) * nTiLe / 1000) / 2 - nY1 * nTiLe / 1000;",
                 "\t\t}"],
                ["\t\tif (!g_nJxKeoAnhVatPham && oCo.nX > 0 && oCo.nY > 0)",
                 "\t\t{",
                 "\t\t\t// %s MAC DINH: ve NGUYEN CO, dat sao cho KHUNG ANH nam chinh giua o." % DAU,
                 "\t\t\t// Represent se cong offset khung anh khi ve (nOffX/nOffY) nen tru truoc o day.",
                 "\t\t\tm_Image.oPosition.nX = g_nJxVeVatPhamX + (g_nJxVeVatPhamW - oCo.nX) / 2 - oLech.nX;",
                 "\t\t\tm_Image.oPosition.nY = g_nJxVeVatPhamY + (g_nJxVeVatPhamH - oCo.nY) / 2 - oLech.nY;",
                 "\t\t\tif (g_nJxNhatKyVatPham > 0)",
                 "\t\t\t{",
                 "\t\t\t\tg_nJxNhatKyVatPham--;",
                 "\t\t\t\tg_DebugLog(\"[VATPHAM-GIUA] o %d,%d %dx%d | khung anh %dx%d lech %d,%d | ve tai %d,%d | %s\",",
                 "\t\t\t\t\tg_nJxVeVatPhamX, g_nJxVeVatPhamY, g_nJxVeVatPhamW, g_nJxVeVatPhamH, oCo.nX, oCo.nY,",
                 "\t\t\t\t\toLech.nX, oLech.nY, m_Image.oPosition.nX, m_Image.oPosition.nY, m_Image.szImage);",
                 "\t\t\t}",
                 "\t\t\tg_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, TRUE);",
                 "\t\t\treturn;\t// da ve xong (so luong mon chong o phim tat khong ve, giong ban goc)",
                 "\t\t}",
                 "\t\tif (nTiLe > 0 && g_nJxKeoAnhVatPham)",
                 "\t\t{",
                 "\t\t\t// khung dich = mien [0, co] sau khi phong; dat sao cho hop anh can giua trong o",
                 "\t\t\tnVeW = oCo.nX * nTiLe / 1000;",
                 "\t\t\tnVeH = oCo.nY * nTiLe / 1000;",
                 "\t\t\tif (nVeW < 1) nVeW = 1;",
                 "\t\t\tif (nVeH < 1) nVeH = 1;",
                 "\t\t\tnX0 = g_nJxVeVatPhamX + (g_nJxVeVatPhamW - (nX2 - nX1) * nTiLe / 1000) / 2 - nX1 * nTiLe / 1000;",
                 "\t\t\tnY0 = g_nJxVeVatPhamY + (g_nJxVeVatPhamH - (nY2 - nY1) * nTiLe / 1000) / 2 - nY1 * nTiLe / 1000;",
                 "\t\t}"], "nhanh can giua")
    s = thay(s, ["\t\tint nX1 = 0, nY1 = 0, nX2 = 0, nY2 = 0, nTiLe = 0;\t// hop anh trong he toa do nguon (phan nghin)"],
                ["\t\tint nX1 = 0, nY1 = 0, nX2 = 0, nY2 = 0, nTiLe = 0;\t// hop anh trong he toa do nguon (phan nghin)",
                 "\t\textern int g_nJxKeoAnhVatPham;\t// %s" % DAU], "extern cong tac")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (KItem)")
    ghi(K, s); print("da va:", K)
print("xong")
