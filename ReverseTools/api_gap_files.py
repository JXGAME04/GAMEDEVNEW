#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Doi chieu API cho MOT DANH SACH TEP da chep sang du an (khong phai ca thu muc Linux).

Khac api_gap2.py: (1) quet dung cac tep trong danh sach; (2) [LIB] doi chieu voi .lua
CUA DU AN (cay dang chay) chu khong phai cay Linux -> biet ngay ham Lua nao chua co.

Dung: python api_gap_files.py <danh-sach.txt (duong dan tuong doi script/, moi dong 1 tep)>
"""
import os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import api_gap2 as G

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
SRV2 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\scriptjx2"


def main():
    lst = [l.strip() for l in open(sys.argv[1]).read().split("\n") if l.strip()]
    files = [os.path.join(SRV, l.replace("/", "\\")) for l in lst]
    files = [f for f in files if os.path.isfile(f)]
    reg = G.project_registered()
    elf = set(re.findall(r'0x[0-9A-Fa-f]+\s+(\S+)', G.read(G.ELF)))
    elf |= set(re.findall(r'0x[0-9A-Fa-f]+\s+(\S+)', G.read(G.ELF_RELAY)))
    proj_defs = G.lua_defs(SRV)
    for k, v in G.lua_defs(SRV2).items():
        proj_defs.setdefault(k, []).extend(v)
    lnx_defs = G.lua_defs(G.LNX)

    calls = {}
    tree_defs = {}
    for p in files:
        t = re.sub(r'--[^\n]*', '', G.read(p))
        for m in re.finditer(r'function\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(', t):
            tree_defs.setdefault(m.group(1), []).append(p)
        for m in re.finditer(r'([A-Za-z_][A-Za-z0-9_]*)\s*=\s*function\s*\(', t):
            tree_defs.setdefault(m.group(1), []).append(p)
        for m in re.finditer(r'(?<![.:\w])([A-Za-z_][A-Za-z0-9_]*)\s*\(', t):
            n = m.group(1)
            if n in G.LUA_KW:
                continue
            calls.setdefault(n, []).append((p, t[:m.start()].count("\n") + 1))

    buckets = {"OK": [], "TREE": [], "PROJLUA": [], "LNXLUA": [], "ENG": [], "???": []}
    for n, sites in calls.items():
        if n in tree_defs:
            b = "TREE"
        elif n in reg:
            b = "OK"
        elif n in proj_defs:
            b = "PROJLUA"
        elif n in lnx_defs:
            b = "LNXLUA"
        elif n in elf:
            b = "ENG"
        else:
            b = "???"
        buckets[b].append((n, sites))
    print("%d tep | %d ten duoc goi" % (len(files), len(calls)))
    for b in ("ENG", "???", "LNXLUA", "PROJLUA"):
        lst2 = sorted(buckets[b], key=lambda x: -len(x[1]))
        print("\n--- [%s] %d ten ---" % (b, len(lst2)))
        for n, sites in lst2:
            p, l = sites[0]
            extra = ""
            if b == "LNXLUA":
                fs = sorted(set(os.path.relpath(x, G.LNX) for x in lnx_defs[n]))
                extra = " <= LINUX " + ("; ".join(fs[:2]) + ("..." if len(fs) > 2 else ""))
            elif b == "PROJLUA":
                fs = sorted(set(os.path.relpath(x, SRV) if x.startswith(SRV) else os.path.relpath(x, SRV2) for x in proj_defs[n]))
                extra = " <= " + ("; ".join(fs[:2]) + ("..." if len(fs) > 2 else ""))
            print("  %-28s x%-4d %s:%d%s" % (n, len(sites), os.path.relpath(p, SRV).replace("\\", "/"), l, extra))
    print("\n[OK] engine da co: %d | [TREE] tu dinh nghia: %d" % (len(buckets["OK"]), len(buckets["TREE"])))


main()
