# -*- coding: utf-8 -*-
"""t72_truy_chuoi_include.py - truy chuoi Include cua mot tep, tim mat xich gay.

TRIEU CHUNG: ScriptError 4 (\\script\\item\\lenhbaiadmin.lua) cFuncName TTHD_MM_Vao853
    Ham do CO trong item\\test_hoatdong_admin.lua:398, va lenhbaiadmin.lua:43 CO
    Include tep do. Ca hai dung cu phap. Vay ham nil vi mot mat xich TRONG CHUOI
    Include bi gay: neu mot tep duoc Include nem loi o than chunk thi phan con
    lai cua tep goi (ke ca cac 'function ... end' phia sau) KHONG duoc khai.

Cong cu nay duyet ca cay Include (de quy) va bao:
  - tep trong chuoi TRO TOI mot tep DA DOI sang _dara  -> mat xich gay
  - tep trong chuoi KHONG TON TAI                       -> mat xich gay
  - tep trong chuoi sai cu phap                         -> mat xich gay

Dung: t72_truy_chuoi_include.py <duong/dan/tuong/doi.lua>
CHI DOC.
"""
import io
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import doi_tep as dt  # noqa: E402
import lua_ham as lh  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

RE_INC = re.compile(r'Include\s*\(\s*[\["]+([^\]"]+)', re.I)


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def chuan_rel(x):
    x = x.replace("/", "\\").lstrip("\\")
    x = re.sub(r"\\+", "\\\\", x)
    if x.lower().startswith("script\\"):
        x = x[len("script\\"):]
    return x


def main():
    goc = sys.argv[1] if len(sys.argv) > 1 else "item/lenhbaiadmin.lua"
    goc = chuan_rel(goc)

    # ten tep dang nam trong _dara
    mat = {}
    for g, _, ts in os.walk(os.path.join(dt.DARA, "script")):
        for t in ts:
            if t.lower().endswith(".lua"):
                mat[t.lower()] = os.path.relpath(os.path.join(g, t),
                                                 os.path.join(dt.DARA,
                                                              "script"))
    print("=== t72 truy chuoi Include: %s ===" % goc)
    print("    tep dang nam trong _dara: %d" % len(mat))
    print()

    seen = set()
    hong = []
    q = [(goc, "(goc)")]
    while q:
        rel, cha = q.pop()
        k = rel.lower()
        if k in seen:
            continue
        seen.add(k)
        p = os.path.join(dt.S, rel.replace("/", os.sep))
        ten = os.path.basename(rel).lower()
        if not os.path.isfile(p):
            vi = mat.get(ten)
            hong.append((cha, rel,
                         "DA DOI sang _dara (%s)" % vi if vi
                         else "KHONG TON TAI"))
            continue
        try:
            d = doc(p)
        except OSError as e:
            hong.append((cha, rel, "khong doc duoc: %s" % e))
            continue
        if lh.can_bang(d) != 0:
            hong.append((cha, rel,
                         "can bang tu khoa LECH %d" % lh.can_bang(d)))
        # CHI lay dong Include DANG SONG. Bo qua dong da comment - khong thi
        # bao nham cac tep da co y go (t38 comment Include chu khong xoa dong).
        for l in d.replace("\r\n", "\n").split("\n"):
            if l.lstrip().startswith("--"):
                continue
            for m in RE_INC.finditer(l):
                x = chuan_rel(m.group(1))
                q.append((x.replace("\\", "/"), rel))

    print("  tep trong chuoi: %d" % len(seen))
    print()
    if not hong:
        print("  => chuoi Include SACH - khong mat xich nao gay")
    else:
        print("  !! %d MAT XICH GAY:" % len(hong))
        for cha, rel, ly in hong:
            print("     %s" % rel)
            print("        %s   (duoc %s Include)" % (ly, cha))
    return 0


if __name__ == "__main__":
    sys.exit(main())
