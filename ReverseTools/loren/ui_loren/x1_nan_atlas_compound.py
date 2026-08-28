# -*- coding: utf-8 -*-
"""x1_nan_atlas_compound.py - NAN MA VAT PHAM cho `atlas_compound.txt` (Do pho HK).

VAN DE: bang cong thuc Do pho duoc lay tu ban goc (VLTK/Linux) nhung CHUA DOI
sang ma vat pham cua JX1. O dai lo ren, JX1 lech DUNG 1 don vi so voi ban goc,
nen cong thuc ghi "Huyen Thiet khoang" kem ma 200, trong khi ma 200 ben JX1 la
"Khong Tuoc Thach" (Huyen Thiet khoang that la 199) => ep se doi NHAM khoang va
ra NHAM mon.

DO THAT (tra theo TEN trong magicscript.txt):
    lech +1 (ma ban goc, chua nan) : 4.163 o
    da dung san                    :   856 o
    ten tra khong ra               :   238 o  (21 ten do pho)

=> TUYET DOI KHONG duoc tru 1 hang loat: 856 o dang dung se hong theo.
Cach dung, va la LUAT da chot cua du an: NAN THEO TEN - tra ten trong bang vat
pham that roi lay ma dung; TRA TEN THAT BAI thi PHAI BAO LOI, khong duoc im
lang giu ma cu (giu im lang chinh la tu tao khoa trung).

PHAM VI NAN (chi cac cot ma cua vat pham lo ren, genre 6 / detail 1):
    ATLAS_PARTICULAR                 (cot 3,  ten o cot 0)
    1_..6_PARTICULAR                 (cot 7,14,21,28,35,42; ten o cot 4,11,...)
KHONG dong toi `DES_*`: `atlas.lua:176` da tu tru 1 cho DES_DETAILTYPE, tuc mon
RA duoc xu ly trong ma - sua them o bang la tru hai lan.

VOI 21 TEN DO PHO KHONG TRA RA (JX1 chua co vat pham do):
Cac dong ay se duoc TACH RA tep `atlas_compound_thieu.txt` va BO KHOI bang chinh.
Ly do: neu de nguyen, ma cu (cua ban goc) se tro NHAM sang mot mon khac cua JX1
=> nguoi choi bo do pho A vao lai ra mon B. Bo ra la an toan; khi nao co vat pham
thi chep nguoc lai tu tep do.

KHONG PHAI BUILD: chi la tep du lieu.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_nan lan dau).
"""
import io
import os
import re
import shutil
import sys

B = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item"
DICH = os.path.join(B, "atlas_compound.txt")
THIEU = os.path.join(B, "atlas_compound_thieu.txt")
HAU_TO = ".truoc_nan"

# (cot TEN, cot GENRE, cot DETAIL, cot PARTICULAR, nhan)
COT = [(0, 1, 2, 3, "ATLAS")] + [(4 + i * 7, 5 + i * 7, 6 + i * 7, 7 + i * 7, "NL%d" % (i + 1))
                                 for i in range(6)]


def chuan(s):
    return re.sub(r"\s+", "", s).lower()


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== x1_nan_atlas_compound - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    for p in (DICH, os.path.join(B, "magicscript.txt")):
        if not os.path.isfile(p):
            print("!!! LOI TO: khong thay %s" % p)
            return 1

    # ---- bang tra: ten -> danh sach (genre, detail, ptc) ----
    raw_ms = io.open(os.path.join(B, "magicscript.txt"), "rb").read().decode("latin-1")
    eol_ms = "\r\n" if raw_ms.count("\r\n") >= (raw_ms.count("\n") - raw_ms.count("\r\n")) else "\n"
    tra = {}
    trac = {}
    for l in raw_ms.split(eol_ms)[1:]:
        c = [x.strip() for x in l.split("\t")]
        if len(c) > 3 and c[0] and c[3]:
            tra.setdefault(c[0], []).append((c[1], c[2], c[3]))
            trac.setdefault(chuan(c[0]), []).append((c[1], c[2], c[3]))
    print("  bang tra: %d ten tu magicscript.txt" % len(tra))

    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    dong = raw.split(eol)
    if "NAN MA" in dong[0]:
        print("  DA NAN - bo qua (idempotent)")
        return 0

    giu = [dong[0]]
    bo = []
    n_sua = n_giu = 0
    ten_thieu = {}

    for l in dong[1:]:
        if not l.strip():
            continue
        c = l.split("\t")
        if len(c) < 50:
            giu.append(l)
            continue
        # kiem TEN do pho truoc: khong tra ra thi bo ca dong
        ten_atlas = c[0].strip()
        if ten_atlas and ten_atlas not in tra:
            g = trac.get(chuan(ten_atlas))
            if not g:
                ten_thieu[ten_atlas] = ten_thieu.get(ten_atlas, 0) + 1
                bo.append(l)
                continue
        # nan tung cot
        for iten, ig, idt, ip, nhan in COT:
            if iten >= len(c) or ip >= len(c):
                continue
            ten = c[iten].strip()
            cu = c[ip].strip()
            if not ten or not cu:
                continue
            ds = tra.get(ten) or trac.get(chuan(ten))
            if not ds:
                print("  !!! TRA TEN THAT BAI (%s): %r - DUNG LAI, khong nan lieu"
                      % (nhan, ten[:40]))
                return 1
            # loc theo genre/detail neu co
            loc = [x for x in ds
                   if (not c[ig].strip() or x[0] == c[ig].strip())
                   and (not c[idt].strip() or x[1] == c[idt].strip())]
            if not loc:
                loc = ds
            dung = loc[0][2]
            if dung != cu:
                c[ip] = dung
                n_sua += 1
            else:
                n_giu += 1
        giu.append("\t".join(c))

    print("  sua %d o | giu nguyen %d o (da dung san)" % (n_sua, n_giu))
    print("  bo %d dong cua %d ten do pho JX1 CHUA CO vat pham:" % (len(bo), len(ten_thieu)))
    for k in sorted(ten_thieu)[:6]:
        print("     %-46s (%d dong)" % (k[:46], ten_thieu[k]))
    if len(ten_thieu) > 6:
        print("     ... con %d ten nua" % (len(ten_thieu) - 6))

    giu[0] = dong[0]
    nd = eol.join(giu) + eol
    hi1 = sum(1 for x in nd if ord(x) > 127)
    print("  byte cao %d -> %d (giam do bo %d dong)" % (hi0, hi1, len(bo)))
    print("  so dong %d -> %d" % (len(dong), len(giu)))

    # chot: khong con o nao lech
    con = 0
    for l in giu[1:]:
        c = [x.strip() for x in l.split("\t")]
        if len(c) < 50:
            continue
        for iten, ig, idt, ip, nhan in COT:
            if iten >= len(c) or ip >= len(c):
                continue
            if not c[iten] or not c[ip]:
                continue
            ds = tra.get(c[iten]) or trac.get(chuan(c[iten])) or []
            if c[ip] not in [x[2] for x in ds]:
                con += 1
    print("  chot: con %d o KHONG khop ten (mong 0)" % con)
    if con:
        print("!!! LOI TO: van con o lech - khong ghi")
        return 1

    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
        print("  sao luu -> %s" % os.path.basename(sao))
    with io.open(DICH, "wb") as f:
        f.write(nd.encode("latin-1"))
    if bo:
        with io.open(THIEU, "wb") as f:
            f.write((dong[0] + eol + eol.join(bo) + eol).encode("latin-1"))
        print("  ghi %d dong bi bo -> %s" % (len(bo), os.path.basename(THIEU)))
    print("  DA GHI + doc lai: %s" % ("khop" if io.open(DICH, "rb").read().decode("latin-1") == nd else "!!! LECH"))
    return 0


if __name__ == "__main__":
    sys.exit(main())
