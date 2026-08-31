# -*- coding: utf-8 -*-
"""t73_quet_chuoi_thanchunk.py - quet loi goi nil o than chunk TREN CA CHUOI Include.

t71 chi quet cac tep toi sua. Cong cu nay: cho mot tep goc, duyet DE QUY moi tep
trong chuoi Include cua no, roi voi TUNG tep kiem loi goi ham o THAN CHUNK.

VI SAO CAN: neu mot tep trong chuoi nem loi o than chunk thi tep GOI no bi ngat
NGAY TAI DONG Include - moi 'function ... end' phia SAU dong do KHONG duoc khai.
Trieu chung o game la "bam nut bao ScriptError, ham khong ton tai" du ham nam
ro rang trong tep.

Khi kiem tung tep, tap ten hop le = ham engine + ten khai trong chinh tep +
ten khai trong CAC TEP DA Include O TREN dong dang xet (dung thu tu).

Dung: t73_quet_chuoi_thanchunk.py <duong/dan.lua>
CHI DOC.
"""
import io
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import doi_tep as dt  # noqa: E402
import lua_ham as lh  # noqa: E402
import t71_quet_goi_nil_thanchunk as t71  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

RE_INC = re.compile(r'Include\s*\(\s*[\["]+([^\]"]+)', re.I)


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def chuan_rel(x):
    x = re.sub(r"\\+", "\\\\", x.replace("/", "\\")).lstrip("\\")
    if x.lower().startswith("script\\"):
        x = x[len("script\\"):]
    return x


def chuoi(goc):
    """Danh sach tep trong chuoi Include, theo thu tu gap."""
    ra, seen, q = [], set(), [chuan_rel(goc)]
    while q:
        rel = q.pop(0)
        k = rel.lower()
        if k in seen:
            continue
        seen.add(k)
        p = os.path.join(dt.S, rel.replace("/", os.sep))
        if not os.path.isfile(p):
            continue
        ra.append(rel)
        try:
            d = doc(p)
        except OSError:
            continue
        for l in d.replace("\r\n", "\n").split("\n"):
            if l.lstrip().startswith("--"):
                continue
            for m in RE_INC.finditer(l):
                q.append(chuan_rel(m.group(1)))
    return ra


def main():
    goc = sys.argv[1] if len(sys.argv) > 1 else "item/test_hoatdong_admin.lua"
    he = t71.ham_engine()
    ds = chuoi(goc)
    print("=== t73 quet than chunk ca chuoi Include: %s ===" % goc)
    print("    ham engine: %d | tep trong chuoi: %d" % (len(he), len(ds)))
    print()
    n = 0
    for rel in ds:
        p = os.path.join(dt.S, rel.replace("/", os.sep))
        try:
            raw = doc(p)
        except OSError:
            continue
        dong = [x.rstrip("\r") for x in raw.replace("\r\n", "\n").split("\n")]
        trong = [False] * len(dong)
        cb, dang = 0, False
        for i, l in enumerate(dong):
            s = lh.sach(l)
            if re.search(r"\bfunction\b", s) and not dang:
                dang, cb = True, 0
            if dang:
                trong[i] = True
                cb += lh.can_bang(l)
                if cb == 0:
                    dang = False
        hop_le = set(he) | t71.khai_trong(p) | t71.BO
        for i, l in enumerate(dong):
            # cap nhat dan: Include o TREN thi ten cua no da co
            if not trong[i] and not l.lstrip().startswith("--"):
                for m in RE_INC.finditer(l):
                    q = os.path.join(dt.S,
                                     chuan_rel(m.group(1)).replace("/", os.sep))
                    if os.path.isfile(q):
                        hop_le |= t71.khai_trong(q)
            if trong[i] or l.lstrip().startswith("--") or not l.strip():
                continue
            for m in t71.RE_GOI.finditer(lh.sach(l)):
                ten = m.group(1)
                if ten in hop_le:
                    continue
                print("  !! %s:%d  goi %s()" % (rel, i + 1, ten))
                print("       %s" % l.strip()[:100])
                n += 1
    print()
    print("=> %d cho goi ham co the NIL o than chunk trong ca chuoi" % n)
    return 0


if __name__ == "__main__":
    sys.exit(main())
