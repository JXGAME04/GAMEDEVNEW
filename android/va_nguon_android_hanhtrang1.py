# -*- coding: utf-8 -*-
#
# [HANHTRANG 12/09] Hanh trang mobile o 44 px (android/anh_hanhtrang_vnku.py): anh vat pham goc 26 px duoc CoreDrawGameObj CAN GIUA
# trong o (khong phong) -> icon nho lot thom trong o to. Sua (chi JX_ANDROID, ban PC khong doi):
#   Sources/Core/src/CoreDrawGameObj.cpp : o to hon anh goc (Width > so o x 26) va khong phai che do thu nho -> dat co ve
#   Sources/Core/src/KItem.cpp            : PaintItem ve keo (RU_T_IMAGE_STRETCH) theo co do; xong xoa co.
# Tui do / cua hang / ruong o 26-28 px khong doi (Width - vien < 26).
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[HANHTRANG 12/09]"


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


def va(p, ham):
    s = doc(p)
    if DAU in s:
        print("da va roi:", p)
        return
    s2 = ham(s)
    if cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


def va_kitem(s):
    s = thay(s, ['#include "KTabFile.h"'],
                ['#include "KTabFile.h"',
                 "#ifdef JX_ANDROID",
                 "// %s o hanh trang mobile (44 px) to hon anh vat pham goc (26 px): CoreDrawGameObj dat co o can ve," % DAU,
                 "// PaintItem keo anh theo co do (RU_T_IMAGE_STRETCH). 0 = ve nhu cu.",
                 "int g_nJxVeVatPhamW = 0, g_nJxVeVatPhamH = 0;",
                 "#endif"], "KItem include")
    s = thay(s, ["\telse", "\t\tg_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, TRUE);"],
                ["#ifdef JX_ANDROID",
                 "\telse if (g_nJxVeVatPhamW > 0 && g_nJxVeVatPhamH > 0)\t// %s" % DAU,
                 "\t{",
                 "\t\tm_Image.oEndPos.nX = m_Image.oPosition.nX + g_nJxVeVatPhamW;",
                 "\t\tm_Image.oEndPos.nY = m_Image.oPosition.nY + g_nJxVeVatPhamH;",
                 "\t\tm_Image.oEndPos.nZ = m_Image.oPosition.nZ;",
                 "\t\tg_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE_STRETCH, TRUE);",
                 "\t}",
                 "#endif",
                 "\telse", "\t\tg_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, TRUE);"], "KItem PaintItem")
    return s


def va_core(s):
    s = thay(s, ["\t\telse if ((nParam & 0x40000000) == 0 || uObjGenre == CGOG_IME_ITEM",
                 "\t\t\t|| Item[uId].GetWidth() * Item[uId].GetHeight() <= 1)",
                 "\t\t{"],
                ["#ifdef JX_ANDROID",
                 "\t\t// %s o to hon anh goc (hanh trang mobile 44 px) -> keo anh theo o, khong can giua" % DAU,
                 "\t\telse if (uObjGenre != CGOG_IME_ITEM && (nParam & 0x40000000) == 0",
                 "\t\t\t&& Width > Item[uId].GetWidth() * ITEM_CELL_WIDTH && Height > Item[uId].GetHeight() * ITEM_CELL_HEIGHT)",
                 "\t\t{",
                 "\t\t\textern int g_nJxVeVatPhamW, g_nJxVeVatPhamH;",
                 "\t\t\tg_nJxVeVatPhamW = Width;",
                 "\t\t\tg_nJxVeVatPhamH = Height;",
                 "\t\t}",
                 "#endif",
                 "\t\telse if ((nParam & 0x40000000) == 0 || uObjGenre == CGOG_IME_ITEM",
                 "\t\t\t|| Item[uId].GetWidth() * Item[uId].GetHeight() <= 1)",
                 "\t\t{"], "CoreDrawGameObj can giua")
    s = thay(s, ["\t\t\tItem[uId].PaintItem(x, y, (nParam & 0x40000000) != 0, true, uId);"],
                ["\t\t\tItem[uId].PaintItem(x, y, (nParam & 0x40000000) != 0, true, uId);",
                 "#ifdef JX_ANDROID",
                 "\t\t\t{",
                 "\t\t\t\textern int g_nJxVeVatPhamW, g_nJxVeVatPhamH;\t// %s xoa co sau khi ve" % DAU,
                 "\t\t\t\tg_nJxVeVatPhamW = g_nJxVeVatPhamH = 0;",
                 "\t\t\t}",
                 "#endif"], "CoreDrawGameObj xoa co")
    return s


va("Sources/Core/src/KItem.cpp", va_kitem)
va("Sources/Core/src/CoreDrawGameObj.cpp", va_core)
print("xong")
