#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Doc / tim kiem trong cay script Linux (GBK + TCVN3 tron lan).

Nhan dang TUNG DONG:
  - khong co byte >127                      -> ascii
  - MOI byte cao nam trong bang 74 byte TCVN3 va run lien tiep <= 3 -> tcvn3
  - con lai                                 -> gbk

  gbktool.py read  <file> [start] [count]
  gbktool.py grep  <root> <text> [ext] [-l]
  gbktool.py greps <root> <ascii-regex> [ext]
  gbktool.py enc   <file>
"""
import io, os, re, sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes, _UNICODE_TO_TCVN3

_T2U = {ord(v): k for k, v in _UNICODE_TO_TCVN3.items()}
_TSET = set(_T2U)


def tcvn3_dec(b):
    return "".join(_T2U.get(c, chr(c)) for c in b)


def runs(data):
    out, c = [], 0
    for x in data:
        if x > 127:
            c += 1
        else:
            if c:
                out.append(c)
            c = 0
    if c:
        out.append(c)
    return out


def detect(data):
    hi = [b for b in data if b > 127]
    if not hi:
        return "ascii"
    if all(b in _TSET for b in hi) and max(runs(data)) <= 3:
        return "tcvn3"
    return "gbk"


def dec(b, enc):
    if enc == "gbk":
        return b.decode("gbk", errors="replace")
    if enc == "tcvn3":
        return tcvn3_dec(b)
    return b.decode("latin-1")


def decline(b):
    return dec(b, detect(b))


def cmd_read(path, start=1, count=100000):
    data = open(path, "rb").read()
    print("### %s  [%s]" % (path, detect(data)))
    for i, ln in enumerate(data.split(b"\n")[int(start) - 1:int(start) - 1 + int(count)], int(start)):
        print("%5d\t%s" % (i, decline(ln.rstrip(b"\r"))))


def walk(root, ext):
    if os.path.isfile(root):
        yield root
        return
    for dp, dn, fn in os.walk(root):
        for f in fn:
            if not ext or f.lower().endswith(tuple(ext.split(","))):
                yield os.path.join(dp, f)


def cmd_grep(root, pattern, ext=".lua", only_files=False):
    pats = []
    try:
        pats.append(pattern.encode("gbk"))
    except Exception:
        pass
    try:
        pats.append(unicode_to_tcvn3_bytes(pattern))
    except Exception:
        pass
    pats = [p for p in set(pats) if p]
    hits = 0
    for p in walk(root, ext):
        try:
            data = open(p, "rb").read()
        except Exception:
            continue
        if not any(q in data for q in pats):
            continue
        hits += 1
        if only_files:
            print(p)
            continue
        for i, ln in enumerate(data.split(b"\n"), 1):
            ln = ln.rstrip(b"\r")
            if any(q in ln for q in pats):
                print("%s:%d: %s" % (p, i, decline(ln)))
    print("--- files matched: %d" % hits)


def cmd_greps(root, regex, ext=".lua"):
    rx = re.compile(regex.encode("latin-1"), re.I)
    for p in walk(root, ext):
        try:
            data = open(p, "rb").read()
        except Exception:
            continue
        for i, ln in enumerate(data.split(b"\n"), 1):
            ln = ln.rstrip(b"\r")
            if rx.search(ln):
                print("%s:%d: %s" % (p, i, decline(ln)))


if __name__ == "__main__":
    c, a = sys.argv[1], sys.argv[2:]
    if c == "read":
        cmd_read(*a)
    elif c == "grep":
        only = "-l" in a
        a = [x for x in a if x != "-l"]
        cmd_grep(a[0], a[1], a[2] if len(a) > 2 else ".lua", only)
    elif c == "greps":
        cmd_greps(a[0], a[1], a[2] if len(a) > 2 else ".lua")
    elif c == "enc":
        d = open(a[0], "rb").read()
        print(a[0], detect(d))
