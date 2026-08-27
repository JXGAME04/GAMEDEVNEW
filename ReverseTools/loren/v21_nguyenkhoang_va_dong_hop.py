# -*- coding: utf-8 -*-
r"""v21 - HAI SUA TU PHAN BIEN VONG 4.

(1) sLaNguyenLieuLoRen THIEU sau NGUYEN KHOANG 6/1/148..153.
    Do bang magicscript cua may chu: ca sau deu nMaxStack = 50 nen van dinh
    SetSeries(-1) cua Gen_MagicScript. Ma magic_distill.lua doc ngu hanh cua
    chinh chung (g_nOreSeries) -> luon -1 -> vi tri chan tra SERIES_ERROR,
    con isMagicMatchSeries(..., -1) lam MOI vi tri tra MAGIC_ERROR.
    magic_distill la nguon DUY NHAT sinh ra sau khoang thuoc tinh 199..204,
    nen hong no la kham nam va Do pho cung khong co nguyen lieu.
    Doi chung ban goc: settings\item\004\magicscript.txt ghi
    "yeu cau ngu hanh giong nhau = 1" cho 6/1/150,152,154 -> ban goc GIU ngu
    hanh that.

(2) KICH BAN KHONG BAO GIO DONG HOP GIAO VAT PHAM.
    JX1 co san ham Lua EndGiveBox (ScriptFuns.cpp:3610, dang ky :14563) va
    cac kich ban khac deu goi (vd global\npcchucnang\lequan.lua:289).
    Ban port khong goi o loi ra nao -> hop van mo, va mon nao con lai trong
    o pos_affairitem thi nam ke't o do; cho do co luu xuong CSDL nen dang
    nhap lai van khong thay do dau.
    Them EndGiveBox() vao moi loi ra cua ham nhan nguyen lieu.
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CR = chr(13)
NL = chr(10)
TAB = chr(9)
SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"
HERE = os.path.dirname(os.path.abspath(__file__))


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def dem_dong(t):
    crlf = t.count(CR + NL)
    return crlf, t.count(NL) - crlf


def theo_eol(t, s):
    crlf, lf = dem_dong(t)
    s = s.replace(CR + NL, NL)
    if crlf > lf:
        s = s.replace(NL, CR + NL)
    return s


def khoi(*d):
    return "".join(x + NL for x in d)


# --- (1) C++: them sau nguyen khoang -----------------------------------------
CPP_TIM = khoi(
    TAB + "if (nParticularType == 146)",
    TAB * 2 + "return TRUE;",
)
CPP_THAY = khoi(
    TAB + "if (nParticularType == 146)",
    TAB * 2 + "return TRUE;",
    TAB + "// sau NGUYEN KHOANG - nguyen lieu cua thao tac tinh luyen thuoc tinh.",
    TAB + "// Chung cung nMaxStack = 50 nen cung dinh phep ep, va magic_distill",
    TAB + "// doc ngu hanh cua chinh chung; thieu day la ca kham nam lan Do pho",
    TAB + "// deu khong co nguyen lieu.",
    TAB + "if (nParticularType >= 148 && nParticularType <= 153)",
    TAB * 2 + "return TRUE;",
)


def va_cpp():
    p = os.path.join(SRC, "KItemGenerator.cpp")
    goc = doc(p)
    a = theo_eol(goc, CPP_TIM)
    b = theo_eol(goc, CPP_THAY)
    if b in goc:
        print("  BO QUA  KItemGenerator.cpp: da co san")
        return 0
    n = goc.count(a)
    if n != 1:
        print("  *** LOI KItemGenerator.cpp: moc neo %d lan" % n)
        return 1
    t = goc.replace(a, b, 1)
    c0, l0 = dem_dong(goc)
    c1, l1 = dem_dong(t)
    if min(c1, l1) > min(c0, l0):
        print("  *** LOI KItemGenerator.cpp: tang loai xuong dong thieu so")
        return 1
    io.open(p, "wb").write(t.encode("latin-1"))
    print("  OK      KItemGenerator.cpp them 148..153 (CRLF %d->%d)" % (c0, c1))
    return 0


# --- (2) Lua: dong hop giao o moi loi ra --------------------------------------
LUA = [
    (os.path.join(HERE, "lua_utf8", "global", "equip_system.lua"), "LoRenChay"),
    (os.path.join(HERE, "lua_utf8", "item", "test_loren_admin.lua"), "LR_ChayNhan"),
]


def va_lua():
    loi = 0
    for p, ten_ham in LUA:
        if not os.path.isfile(p):
            print("  *** KHONG CO %s" % p)
            loi += 1
            continue
        t = io.open(p, encoding="utf-8").read()
        if "EndGiveBox" in t:
            print("  BO QUA  %-24s da co EndGiveBox" % os.path.basename(p))
            continue
        i = t.index("function " + ten_ham)
        j = t.index("\nend", i) + 4
        than = t[i:j]
        # chen EndGiveBox() ngay truoc moi `return` VA truoc `end` cuoi ham
        ra = []
        so = 0
        for dong in than.split("\n"):
            r = dong.strip()
            if r == "return" or r.startswith("return "):
                dau = dong[:len(dong) - len(dong.lstrip())]
                ra.append(dau + "EndGiveBox();\t-- dong hop giao vat pham")
                so += 1
            ra.append(dong)
        than2 = "\n".join(ra)
        # loi ra cuoi cung (roi thang xuong `end`) cung phai dong
        than2 = than2[:than2.rindex("\nend")] + \
            "\n\tEndGiveBox();\t-- dong hop giao vat pham\nend" + \
            than2[than2.rindex("\nend") + 4:]
        so += 1
        t = t[:i] + than2 + t[j:]
        io.open(p, "w", encoding="utf-8", newline="\n").write(t)
        print("  OK      %-24s them EndGiveBox o %d loi ra" % (os.path.basename(p), so))
    return loi


def main():
    print("THEM NGUYEN KHOANG + DONG HOP GIAO VAT PHAM")
    print("=" * 74)
    loi = va_cpp() + va_lua()
    print("=" * 74)
    print("  %s" % ("XONG" if loi == 0 else "CO %d MUC LOI" % loi))
    return 1 if loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
