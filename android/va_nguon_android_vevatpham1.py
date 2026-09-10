# -*- coding: utf-8 -*-
#
# [VEVATPHAM 12/09] Chu 10:20: "toa do dat vat pham dung roi, chi la HINH ANH no sai vi tri". Do tren may:
# o phim 1 (Item_0) 36x36 tai 1081,206 nhung anh binh thuoc ve tu ~1081,198 co ~41x48 - tran ra ngoai o.
# Goc: [HANHTRANG 12/09] chi truyen CO (g_nJxVeVatPhamW/H) roi PaintItem keo anh tu m_Image.oPosition - ma
# oPosition da bi doi truoc do: (a) PaintItem cong (+5,+5) cho vat pham nam o o dung nhanh (pos_immediacy),
# (b) CoreDrawGameObj co the da can giua theo co GOC. Va anh khong giu ti le khi o khong vuong.
# Sua: truyen CA KHUNG O (X, Y, W, H) roi ve gon TRONG o - giu ti le anh goc, can giua - khong phu thuoc
# cac phep dich phia truoc. Chi trong #ifdef JX_ANDROID.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[VEVATPHAM 12/09]"


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


# ---------------- KItem.cpp: bien khung o + ve gon trong o ----------------
K = "Sources/Core/Src/KItem.cpp"
s = doc(K)
if DAU not in s:
    s0 = s
    s = thay(s, ["int g_nJxVeVatPhamW = 0, g_nJxVeVatPhamH = 0;"],
                ["int g_nJxVeVatPhamW = 0, g_nJxVeVatPhamH = 0;",
                 "// %s them goc o (tuyet doi) de ve gon TRONG o, khong an theo m_Image.oPosition da bi dich truoc do" % DAU,
                 "int g_nJxVeVatPhamX = 0, g_nJxVeVatPhamY = 0;",
                 "int g_nJxNhatKyVatPham = 0;\t// so lan con ghi nhat ky ve vat pham (dat > 0 de chan doan)"], "bien")
    s = thay(s, ["\telse if (g_nJxVeVatPhamW > 0 && g_nJxVeVatPhamH > 0)\t// [HANHTRANG 12/09]",
                 "\t{",
                 "\t\tm_Image.oEndPos.nX = m_Image.oPosition.nX + g_nJxVeVatPhamW;",
                 "\t\tm_Image.oEndPos.nY = m_Image.oPosition.nY + g_nJxVeVatPhamH;",
                 "\t\tm_Image.oEndPos.nZ = m_Image.oPosition.nZ;",
                 "\t\tg_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE_STRETCH, TRUE);",
                 "\t}"],
                ["\telse if (g_nJxVeVatPhamW > 0 && g_nJxVeVatPhamH > 0)\t// [HANHTRANG 12/09] + %s" % DAU,
                 "\t{",
                 "\t\t// %s ve GON TRONG o: giu ti le anh goc (mon 1x2, 2x2... khong bi ep vuong), can giua o." % DAU,
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
                 "\t\tnY0 = g_nJxVeVatPhamY + (g_nJxVeVatPhamH - nVeH) / 2;",
                 "\t\tm_Image.oPosition.nX = nX0;",
                 "\t\tm_Image.oPosition.nY = nY0;",
                 "\t\tm_Image.oEndPos.nX = nX0 + nVeW;",
                 "\t\tm_Image.oEndPos.nY = nY0 + nVeH;",
                 "\t\tm_Image.oEndPos.nZ = m_Image.oPosition.nZ;",
                 "\t\tif (g_nJxNhatKyVatPham > 0)",
                 "\t\t{",
                 "\t\t\tg_nJxNhatKyVatPham--;",
                 "\t\t\tg_DebugLog(\"[VATPHAM] o %d,%d %dx%d | mon %dx%d o | ve %d,%d %dx%d\", g_nJxVeVatPhamX, g_nJxVeVatPhamY,",
                 "\t\t\t\tg_nJxVeVatPhamW, g_nJxVeVatPhamH, nGocW, nGocH, nX0, nY0, nVeW, nVeH);",
                 "\t\t}",
                 "\t\tg_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE_STRETCH, TRUE);",
                 "\t}"], "ve gon")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (KItem)")
    ghi(K, s); print("da va:", K)

# ---------------- CoreDrawGameObj.cpp: truyen khung o + nhat ky khi KHONG keo ----------------
C = "Sources/Core/Src/CoreDrawGameObj.cpp"
s = doc(C)
if DAU not in s:
    s0 = s
    s = thay(s, ["\t\t\textern int g_nJxVeVatPhamW, g_nJxVeVatPhamH;",
                 "\t\t\tg_nJxVeVatPhamW = Width;",
                 "\t\t\tg_nJxVeVatPhamH = Height;"],
                ["\t\t\textern int g_nJxVeVatPhamW, g_nJxVeVatPhamH;",
                 "\t\t\textern int g_nJxVeVatPhamX, g_nJxVeVatPhamY;\t// %s goc o" % DAU,
                 "\t\t\tg_nJxVeVatPhamW = Width;",
                 "\t\t\tg_nJxVeVatPhamH = Height;",
                 "\t\t\tg_nJxVeVatPhamX = x;",
                 "\t\t\tg_nJxVeVatPhamY = y;"], "truyen goc o")
    s = thay(s, ["\t\t\t\textern int g_nJxVeVatPhamW, g_nJxVeVatPhamH;\t// [HANHTRANG 12/09] xoa co sau khi ve",
                 "\t\t\t\tg_nJxVeVatPhamW = g_nJxVeVatPhamH = 0;"],
                ["\t\t\t\textern int g_nJxVeVatPhamW, g_nJxVeVatPhamH;\t// [HANHTRANG 12/09] xoa co sau khi ve",
                 "\t\t\t\textern int g_nJxVeVatPhamX, g_nJxVeVatPhamY, g_nJxNhatKyVatPham;\t// %s" % DAU,
                 "\t\t\t\tif (g_nJxNhatKyVatPham > 0 && g_nJxVeVatPhamW == 0)",
                 "\t\t\t\t{",
                 "\t\t\t\t\tg_nJxNhatKyVatPham--;",
                 "\t\t\t\t\tg_DebugLog(\"[VATPHAM] KHONG keo: the loai %d mon %d tai %d,%d khung %dx%d (co goc %dx%d) tham so %08x\",",
                 "\t\t\t\t\t\t(int)uObjGenre, (int)uId, x, y, Width, Height,",
                 "\t\t\t\t\t\tItem[uId].GetWidth() * ITEM_CELL_WIDTH, Item[uId].GetHeight() * ITEM_CELL_HEIGHT, (unsigned int)nParam);",
                 "\t\t\t\t}",
                 "\t\t\t\tg_nJxVeVatPhamW = g_nJxVeVatPhamH = 0;",
                 "\t\t\t\tg_nJxVeVatPhamX = g_nJxVeVatPhamY = 0;"], "nhat ky khong keo")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (CoreDrawGameObj)")
    ghi(C, s); print("da va:", C)

# ---------------- bat nhat ky theo config: [Ui] NhatKyVatPham=<so lan> ----------------
J = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
s = doc(J)
if DAU not in s:
    s0 = s
    s = thay(s, ["\t{\t// [UITOADO 12/09 NEO d] 900 khung (~15 s) sau khi vao the gioi: ghi vi tri thuc moi cua so neu [Ui] NhatKyBoCuc>=2"],
                ["\t{\t// %s [Ui] NhatKyVatPham=<so dong> -> ghi vet phep ve vat pham trong o (mot lan sau khi vao game)" % DAU,
                 "\t\tstatic int s_nDaBatVatPham = 0;",
                 "\t\tif (!s_nDaBatVatPham && KyNang_TrongGame())",
                 "\t\t{",
                 "\t\t\textern int g_nJxNhatKyVatPham;",
                 "\t\t\ts_nDaBatVatPham = 1;",
                 "\t\t\tg_nJxNhatKyVatPham = GetPrivateProfileInt(\"Ui\", \"NhatKyVatPham\", 0, \".\\\\config.ini\");",
                 "\t\t}",
                 "\t}",
                 "\t{\t// [UITOADO 12/09 NEO d] 900 khung (~15 s) sau khi vao the gioi: ghi vi tri thuc moi cua so neu [Ui] NhatKyBoCuc>=2"], "bat nhat ky")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (JxCanDieuKhien)")
    ghi(J, s); print("da va:", J)
print("xong")
