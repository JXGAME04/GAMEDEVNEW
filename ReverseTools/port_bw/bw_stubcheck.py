#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Liet ke moi ham engine ma cay missions/bw goi, tim ham C dang ky trong du an,
in kich thuoc than ham + danh dau STUB (than ham <= 3 dong hoac chi return 0).
Dung: python bw_stubcheck.py
"""
import os, re, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
PROJ = r"D:\GAMEDEVNEW\Sources\Core\Src"
LNX = r"D:\ServerLinux\server1\script\missions\bw"
EXTRA = [r"D:\ServerLinux\server1\script\task\newtask\branch\branch_bwsj.lua"]

def read(p):
    return open(p, "rb").read().decode("latin-1")

# 1. ten goi trong cay
called = {}
files = [os.path.join(LNX, f) for f in os.listdir(LNX) if f.lower().endswith(".lua")] + EXTRA
defs = set()
for p in files:
    t = read(p)
    for i, line in enumerate(t.split("\n"), 1):
        s = line.split("--")[0]
        for m in re.finditer(r'function\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(', s):
            defs.add(m.group(1))
        for m in re.finditer(r'\b([A-Za-z_][A-Za-z0-9_]*)\s*\(', s):
            n = m.group(1)
            if n in ("function", "if", "while", "for", "return", "and", "or", "not", "elseif", "local"):
                continue
            called.setdefault(n, []).append("%s:%d" % (os.path.basename(p), i))
LUA_KW = set("format floor mod random getn tinsert tremove strfind strsub strlen gsub tostring tonumber".split())

# 2. bang dang ky du an: ten -> ham C
reg = {}
for root, _, fs in os.walk(PROJ):
    for f in fs:
        if f.endswith((".cpp", ".h")):
            p = os.path.join(root, f)
            for m in re.finditer(r'\{\s*"([A-Za-z_][A-Za-z0-9_]*)"\s*,\s*([A-Za-z_][A-Za-z0-9_]*)\s*\}', read(p)):
                reg.setdefault(m.group(1), (m.group(2), p))

# 3. tim than ham C
src_cache = {}
def find_body(cname):
    for root, _, fs in os.walk(PROJ):
        for f in fs:
            if not f.endswith(".cpp"):
                continue
            p = os.path.join(root, f)
            if p not in src_cache:
                src_cache[p] = read(p)
            t = src_cache[p]
            m = re.search(r'\n(?:int|void)\s+' + re.escape(cname) + r'\s*\(\s*Lua_State\s*\*\s*\w+\s*\)\s*\n?\s*\{', t)
            if not m:
                continue
            start = m.end()
            depth = 1
            i = start
            while i < len(t) and depth > 0:
                if t[i] == "{": depth += 1
                elif t[i] == "}": depth -= 1
                i += 1
            body = t[start:i-1]
            line = t[:m.start()].count("\n") + 2
            return p, line, body
    return None, None, None

print("%-24s %-28s %-50s %5s %s" % ("TEN", "HAM C", "TEP:DONG", "DONG", "GHI CHU"))
for n in sorted(called):
    if n in defs or n in LUA_KW:
        continue
    if n not in reg:
        print("%-24s %-28s %-50s %5s %s" % (n, "-", "-", "-", "KHONG DANG KY  <- " + ",".join(called[n][:2])))
        continue
    cname, regp = reg[n]
    p, line, body = find_body(cname)
    if body is None:
        print("%-24s %-28s %-50s %5s %s" % (n, cname, os.path.basename(regp), "?", "khong tim thay than ham"))
        continue
    nl = len([l for l in body.split("\n") if l.strip() and not l.strip().startswith("//")])
    note = ""
    if nl <= 2:
        note = "STUB? " + " | ".join(l.strip() for l in body.split("\n") if l.strip())[:80]
    print("%-24s %-28s %-50s %5d %s" % (n, cname, os.path.relpath(p, PROJ) + ":" + str(line), nl, note))
