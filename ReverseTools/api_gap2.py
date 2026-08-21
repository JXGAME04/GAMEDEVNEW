#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Doi chieu API cho MOT cay tinh nang ban Linux voi du an JX1.

Phan loai moi ten duoc goi trong cay:
  [OK ]   du an DA dang ky (ScriptFuns.cpp / Lua*)
  [TREE]  do chinh cay tinh nang dinh nghia (.lua trong cay)
  [LIB ]  do .lua KHAC cua ban Linux dinh nghia -> phai port kem tep do
  [ENG]   khong ai dinh nghia + CO trong bang dang ky ELF Linux -> HAM ENGINE CON THIEU
  [???]   khong ai dinh nghia + KHONG co trong bang ELF -> can soi tay

Dung: python api_gap2.py <ten-nhom> <thu-muc1> [thu-muc2 ...]
"""
import os, re, sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

PROJ = r"D:\GAMEDEVNEW\Sources\Core\Src"
SRC = os.path.join(PROJ, "ScriptFuns.cpp")
LNX = r"D:\ServerLinux\server1\script"
ELF = r"D:\GAMEDEVNEW\ReverseTools\jx_linux_y.luamap.full.txt"
ELF_RELAY = r"D:\GAMEDEVNEW\ReverseTools\s3relay_y.luamap.full.txt"

LUA_KW = set('''and break do else elseif end false for function if in local nil not or repeat return then true until while
print type tostring tonumber format strfind strsub strlen strlower strupper strrep gsub sort getn tinsert tremove
table math string io os random randomseed floor ceil abs min max mod sqrt date time clock dofile assert error
next pairs ipairs setmetatable getmetatable rawget rawset unpack call foreach foreachi tag settag
openfile closefile read write writeto readfrom appendto remove rename tmpname seek gcinfo collectgarbage
strbyte strchar globals rawgetglobal rawsetglobal setglobal getglobal newtag copytagmethods settagmethod gettagmethod
dostring require mod abs'''.split())


def read(p):
    return open(p, "rb").read().decode("latin-1")


def project_registered():
    reg = set(re.findall(r'\{\s*"([A-Za-z_][A-Za-z0-9_]*)"\s*,\s*[A-Za-z_]', read(SRC)))
    for root, _, files in os.walk(PROJ):
        for f in files:
            if f.endswith((".cpp", ".h")):
                p = os.path.join(root, f)
                if p == SRC:
                    continue
                reg |= set(re.findall(r'\{\s*"([A-Za-z_][A-Za-z0-9_]*)"\s*,\s*Lua[A-Za-z_]', read(p)))
    return reg


def lua_defs(root):
    """{ten ham: [tep...]} do cac .lua duoi root dinh nghia."""
    out = {}
    for dp, _, fs in os.walk(root):
        for f in fs:
            if not f.lower().endswith(".lua"):
                continue
            p = os.path.join(dp, f)
            t = re.sub(r'--[^\n]*', '', read(p))
            for m in re.finditer(r'function\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(', t):
                out.setdefault(m.group(1), []).append(p)
            # ham gan kieu  tbX.Foo = function(...)
            for m in re.finditer(r'([A-Za-z_][A-Za-z0-9_]*)\s*=\s*function\s*\(', t):
                out.setdefault(m.group(1), []).append(p)
    return out


def calls_in(roots):
    calls = {}
    nfile = 0
    for R in roots:
        for dp, _, fs in os.walk(R):
            for f in fs:
                if not f.lower().endswith(".lua"):
                    continue
                nfile += 1
                p = os.path.join(dp, f)
                t = re.sub(r'--[^\n]*', '', read(p))
                for m in re.finditer(r'(?<![.:\w])([A-Za-z_][A-Za-z0-9_]*)\s*\(', t):
                    n = m.group(1)
                    if n in LUA_KW:
                        continue
                    calls.setdefault(n, []).append((p, t[:m.start()].count("\n") + 1))
    return calls, nfile


def main():
    name = sys.argv[1]
    roots = [os.path.join(LNX, r.replace("/", "\\")) for r in sys.argv[2:]]
    reg = project_registered()
    elf = set(re.findall(r'0x[0-9A-Fa-f]+\s+(\S+)', read(ELF)))
    elf |= set(re.findall(r'0x[0-9A-Fa-f]+\s+(\S+)', read(ELF_RELAY)))
    tree_defs = {}
    for R in roots:
        for k, v in lua_defs(R).items():
            tree_defs.setdefault(k, []).extend(v)
    lib_defs = lua_defs(LNX)
    calls, nfile = calls_in(roots)

    buckets = {"OK": [], "TREE": [], "LIB": [], "ENG": [], "???": []}
    for n, sites in calls.items():
        if n in tree_defs:
            b = "TREE"
        elif n in reg:
            b = "OK"
        elif n in lib_defs:
            b = "LIB"
        elif n in elf:
            b = "ENG"
        else:
            b = "???"
        buckets[b].append((n, sites))

    print("=" * 78)
    print("NHOM: %s   | %d tep .lua | %d ten duoc goi" % (name, nfile, len(calls)))
    print("cay:", ", ".join(os.path.relpath(r, LNX) for r in roots))
    print("=" * 78)
    for b in ("ENG", "???", "LIB"):
        lst = sorted(buckets[b], key=lambda x: -len(x[1]))
        print("\n--- [%s] %d ten ---" % (b, len(lst)))
        for n, sites in lst:
            p, l = sites[0]
            extra = ""
            if b == "LIB":
                fs = sorted(set(os.path.relpath(x, LNX) for x in lib_defs[n]))
                extra = " <= " + ("; ".join(fs[:2]) + ("..." if len(fs) > 2 else ""))
            print("  %-32s x%-4d %s:%d%s" % (n, len(sites),
                  os.path.relpath(p, LNX).replace("\\", "/"), l, extra))
    print("\n[OK ] da co: %d | [TREE] tu dinh nghia: %d" % (len(buckets["OK"]), len(buckets["TREE"])))


main()
