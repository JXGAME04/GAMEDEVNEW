# -*- coding: utf-8 -*-
r"""v15 - GAN BO TEST LO REN VAO LENH BAI ADMIN.

Hai cho trong  E:\...\bin\server\script\item\lenhbaiadmin.lua :
  1. mot dong Include tep bo test
  2. mot nut trong bang chon cua admin

Tep la TCVN3 nen doc/ghi bang latin-1 (byte-nguyen). Sao luu truoc khi ghi.
"""
import io
import os
import shutil
import sys
import importlib.util

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

HERE = os.path.dirname(os.path.abspath(__file__))
_spec = importlib.util.spec_from_file_location(
    "bangtxt", os.path.join(HERE, "..", "viemde", "bangtxt.py"))
bangtxt = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(bangtxt)

CR = chr(13)
NL = chr(10)
TAB = chr(9)
P = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\lenhbaiadmin.lua"


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


def main():
    if not os.path.isfile(P):
        print("KHONG CO %s" % P)
        return 1
    goc = doc(P)
    t = goc
    print("GAN BO TEST LO REN VAO LENH BAI ADMIN")
    print("=" * 76)
    loi = 0

    # 1. Include
    tim = 'Include("\\\\script\\\\item\\\\test_hoatdong_admin.lua")'
    if "test_loren_admin" in t:
        print("  BO QUA  Include: da co san")
    else:
        i = t.find(tim)
        if i < 0:
            print("  *** LOI Include: khong thay moc neo")
            loi += 1
        else:
            j = t.find(NL, i)
            if j > 0 and t[j - 1] == CR:
                j -= 1
            them = theo_eol(
                t,
                NL + 'Include("\\\\script\\\\item\\\\test_loren_admin.lua")' + TAB +
                "-- [26/08] BO TEST HE LO REN")
            t = t[:j] + them + t[j:]
            print("  OK      Include")

    # 2. nut trong bang chon - chen ngay truoc nut ket thuc
    nhan = bangtxt.uni2tcvn("Bộ test hệ lò rèn") + "/LR_Root"
    if nhan in t:
        print("  BO QUA  nut menu: da co san")
    else:
        moc = TAB + TAB + '"' + bangtxt.uni2tcvn("Kết thúc đối thoại") + '/no"})'
        moc = theo_eol(t, moc)
        n = t.count(moc)
        if n != 1:
            print("  *** LOI nut menu: moc neo xuat hien %d lan" % n)
            loi += 1
        else:
            them = TAB + TAB + '"' + nhan + '",' + NL
            them = theo_eol(t, them) + moc
            t = t.replace(moc, them, 1)
            print("  OK      nut menu")

    if loi == 0 and t != goc:
        c0, l0 = dem_dong(goc)
        c1, l1 = dem_dong(t)
        if min(c1, l1) > min(c0, l0):
            print("  *** LOI: tang loai xuong dong thieu so")
            return 1
        sao = P + ".truoc_loren"
        if not os.path.isfile(sao):
            shutil.copy2(P, sao)
        io.open(P, "wb").write(t.encode("latin-1"))
        print("  >> da ghi (CRLF %d->%d, LF %d->%d)" % (c0, c1, l0, l1))

    print("=" * 76)
    print("  %s" % ("XONG" if loi == 0 else "CO %d MUC LOI" % loi))
    return 1 if loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
