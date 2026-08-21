#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Trich moi item id mot cay script ban Linux dung, roi anh xa sang id cua du an THEO TEN.

  item_remap.py <thu-muc-tuong-doi-trong-script-linux> [thu-muc2 ...]
"""
import io, os, re, sys, collections

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

LNX_SCRIPT = r"D:\ServerLinux\server1\script"
LNX_TBL = r"D:\ServerLinux\server1\settings\item\004\magicscript.txt"
PRJ_TBL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item\magicscript.txt"


def load(p):
    """{particular: [ten, anh, script]} - doc byte tho (latin-1), KHONG giai ma."""
    d = {}
    for ln in io.open(p, encoding="latin-1"):
        c = ln.rstrip("\n").split("\t")
        if len(c) > 4 and c[3].strip().isdigit() and c[1].strip() == "6" and c[2].strip() == "1":
            d[int(c[3])] = c
    return d


def vn(s):
    try:
        return s.encode("latin-1").decode("gbk", "replace")
    except Exception:
        return s


def scan_ids(roots):
    """Tim moi (6,1,N) trong script."""
    pat = re.compile(r"6\s*,\s*1\s*,\s*(\d+)")
    hits = collections.defaultdict(list)
    for R in roots:
        base = os.path.join(LNX_SCRIPT, R.replace("/", "\\"))
        for dp, _, fs in os.walk(base):
            for f in fs:
                if not f.lower().endswith(".lua"):
                    continue
                p = os.path.join(dp, f)
                t = io.open(p, encoding="latin-1").read()
                for i, line in enumerate(t.split("\n"), 1):
                    if line.lstrip().startswith("--"):
                        continue
                    for m in pat.finditer(line):
                        hits[int(m.group(1))].append("%s:%d" % (os.path.relpath(p, LNX_SCRIPT).replace("\\", "/"), i))
    return hits


def main():
    roots = sys.argv[1:]
    ld, pd = load(LNX_TBL), load(PRJ_TBL)
    byname = collections.defaultdict(list)
    for i, c in pd.items():
        byname[c[0].strip()].append(i)
    hits = scan_ids(roots)

    print("Cay: %s" % ", ".join(roots))
    print("Bang Linux %d muc (6/1/*) | bang du an %d muc" % (len(ld), len(pd)))
    print()
    print("%-8s %-36s %-12s %s" % ("LinuxID", "TEN", "DU AN", "DUNG O"))
    print("-" * 110)
    thieu = []
    for i in sorted(hits):
        c = ld.get(i)
        if not c:
            print("%-8d %-36s %-12s %s" % (i, "(khong co trong bang Linux)", "-", hits[i][0]))
            continue
        name = c[0].strip()
        cand = byname.get(name, [])
        if not cand:
            st = ">>> THIEU <<<"
            thieu.append((i, name, c))
        elif cand == [i]:
            st = "TRUNG ID"
        else:
            st = "-> %s" % ",".join(str(x) for x in cand)
        print("%-8d %-36s %-12s %s (x%d)" % (i, vn(name)[:36], st, hits[i][0], len(hits[i])))

    print()
    print("=== ITEM PHAI LAM THEM: %d ===" % len(thieu))
    for i, name, c in thieu:
        print("  Linux id %d | %s | anh %s" % (i, vn(name), vn(c[4] if len(c) > 4 else "")))
        print("     dong goc: %s" % vn("\t".join(c))[:200])


main()
