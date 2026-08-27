# -*- coding: utf-8 -*-
r"""v11 - VA 8 CHO THIEU `return` trong kich ban tru tran bang dang chay.

Phan bien muc B-2. Ma hien tai:

    local nQuality = GetItemQuality(nItemIdx)

    if nQuality == 4 then
        Talk(1, "", "Trang bi nay khong the cuong hoa.")
    end            <-- THIEU return, chay tiep xuong duoi

Hom nay vo hai vi GetItemQuality luon tra 0 VA GetCurDurability o ngay duoi la
nil (ham chet truoc). Sau khi thay DLL thi ca hai deu that => do Bach Kim hien
thong bao "khong the cuong hoa" roi VAN DUOC cuong hoa.

4 tep x 2 ham = 8 cho. Day la CAY VAN HANH (E:\...), chu du an da dong y.
Sao luu tung tep truoc khi ghi.
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CR = chr(13)
NL = chr(10)
TAB = chr(9)

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
TEP = [
    r"scriptjx2\tong\npc\tong_totempole.lua",
    r"scriptjx2\tong_vn\npc\tong_totempole.lua",
    r"scriptjx2\tong\npc\city_totempole.lua",
    r"scriptjx2\tong_vn\npc\city_totempole.lua",
]


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
    print("VA 8 CHO THIEU `return` (tru tran bang)")
    print("=" * 80)
    loi = 0
    for rel in TEP:
        p = os.path.join(SRV, rel)
        if not os.path.isfile(p):
            print("  *** KHONG CO  %s" % rel)
            loi += 1
            continue
        t = doc(p)
        goc = t

        # Moc neo lay theo BYTE that: dong Talk co chuoi TCVN3, ta khong go lai
        # ma cat tu chinh tep - tim "if nQuality == 4 then" roi ghep den "end".
        dau = TAB + "if nQuality == 4 then" + NL
        dau = theo_eol(t, dau)
        n = t.count(dau)
        if n == 0:
            print("  *** MOC NEO 0 lan  %s" % rel)
            loi += 1
            continue

        so_va = 0
        vi_tri = 0
        while True:
            i = t.find(dau, vi_tri)
            if i < 0:
                break
            # tim dong "\tend" dau tien sau do
            ket = theo_eol(t, TAB + "end" + NL)
            j = t.find(ket, i)
            if j < 0:
                vi_tri = i + len(dau)
                continue
            than = t[i + len(dau):j]
            if "return" in than:
                vi_tri = j + len(ket)
                continue
            them = theo_eol(t, TAB + TAB + "return" + NL)
            t = t[:j] + them + t[j:]
            so_va += 1
            vi_tri = j + len(them) + len(ket)

        if so_va == 0:
            print("  BO QUA        %-46s da co return" % rel)
            continue

        c0, l0 = dem_dong(goc)
        c1, l1 = dem_dong(t)
        if min(c1, l1) > min(c0, l0):
            print("  *** LOI       %-46s tang loai xuong dong thieu so" % rel)
            loi += 1
            continue
        sao = p + ".truoc_loren"
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
        io.open(p, "wb").write(t.encode("latin-1"))
        print("  OK  va %d cho  %-46s (CRLF %d->%d)" % (so_va, rel, c0, c1))

    print("=" * 80)
    print("  %s" % ("XONG" if loi == 0 else "CO %d MUC LOI" % loi))
    return 1 if loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
