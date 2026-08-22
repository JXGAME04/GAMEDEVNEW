#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Liet ke moi ten duoc goi trong cay tongwar + noi du an dang ky (tep:dong, ham C) de doi chieu ngu nghia."""
import os, re, sys, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
PROJ = r"D:\GAMEDEVNEW\Sources\Core\Src"
LNX = r"D:\ServerLinux\server1\script"
ROOTS = ["missions/tongwar", "event/tongwar", "tongpkmessage"]
LUA_KW = set('''and break do else elseif end false for function if in local nil not or repeat return then true until while
print type tostring tonumber format strfind strsub strlen strlower strupper strrep gsub sort getn tinsert tremove
table math string io os random randomseed floor ceil abs min max mod sqrt date time clock dofile assert error
next pairs ipairs setmetatable getmetatable rawget rawset unpack call foreach foreachi tag settag
openfile closefile read write writeto readfrom appendto remove rename tmpname seek gcinfo collectgarbage
strbyte strchar globals rawgetglobal rawsetglobal setglobal getglobal newtag copytagmethods settagmethod gettagmethod
dostring require mod abs'''.split())

def read(p):
    return open(p, "rb").read().decode("latin-1")

reg = {}
for root, _, files in os.walk(PROJ):
    for f in files:
        if f.endswith((".cpp", ".h")):
            p = os.path.join(root, f)
            t = read(p)
            for m in re.finditer(r'\{\s*"([A-Za-z_][A-Za-z0-9_]*)"\s*,\s*([A-Za-z_][A-Za-z0-9_]*)', t):
                ln = t[:m.start()].count("\n") + 1
                reg.setdefault(m.group(1), []).append("%s:%d %s" % (os.path.relpath(p, PROJ), ln, m.group(2)))

calls = collections.OrderedDict()
for R in ROOTS:
    for dp, _, fs in os.walk(os.path.join(LNX, R.replace("/", "\\"))):
        for f in fs:
            if not f.lower().endswith(".lua"):
                continue
            p = os.path.join(dp, f)
            t = re.sub(r'--[^\n]*', '', read(p))
            for m in re.finditer(r'(?<![.:\w])([A-Za-z_][A-Za-z0-9_]*)\s*\(', t):
                n = m.group(1)
                if n in LUA_KW:
                    continue
                calls.setdefault(n, []).append("%s:%d" % (os.path.relpath(p, LNX).replace("\\", "/"), t[:m.start()].count("\n") + 1))

for n in sorted(calls):
    if n in reg:
        print("%-28s x%-3d %s   | %s" % (n, len(calls[n]), reg[n][0], calls[n][0]))
