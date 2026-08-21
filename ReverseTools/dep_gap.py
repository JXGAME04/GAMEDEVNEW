#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Bao dong phu thuoc + chenh lech API cho MOT tinh nang ban Linux.

- Duyet Include() de dong bao phu thuoc (chi trong cay script ban Linux).
- Danh dau tep nao DU AN DA CO (script\... hoac scriptjx2\tong_vn\...).
- Gom moi ten ham duoc goi trong bao dong roi phan loai.

Dung: python dep_gap.py <ten> <root1> [root2 ...]
"""
import os, re, sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

LNX = r"D:\ServerLinux\server1\script"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
PROJ = r"D:\GAMEDEVNEW\Sources\Core\Src"
SRC = os.path.join(PROJ, "ScriptFuns.cpp")
ELF = [r"D:\GAMEDEVNEW\ReverseTools\jx_linux_y.luamap.full.txt",
       r"D:\GAMEDEVNEW\ReverseTools\s3relay_y.luamap.full.txt"]

LUA_KW = set('''and break do else elseif end false for function if in local nil not or repeat return then true until while
print type tostring tonumber format strfind strsub strlen strlower strupper strrep gsub sort getn tinsert tremove
table math string io os random randomseed floor ceil abs min max mod sqrt date time clock dofile assert error
next pairs ipairs setmetatable getmetatable rawget rawset unpack call foreach foreachi tag settag
openfile closefile read write writeto readfrom appendto remove rename tmpname seek gcinfo collectgarbage
strbyte strchar globals rawgetglobal rawsetglobal setglobal getglobal newtag copytagmethods settagmethod gettagmethod
dostring require curpack usepack'''.split())


def rd(p):
    return open(p, "rb").read().decode("latin-1")


def registered():
    reg = set(re.findall(r'\{\s*"([A-Za-z_]\w*)"\s*,\s*[A-Za-z_]', rd(SRC)))
    for root, _, fs in os.walk(PROJ):
        for f in fs:
            if f.endswith((".cpp", ".h")):
                p = os.path.join(root, f)
                if p != SRC:
                    reg |= set(re.findall(r'\{\s*"([A-Za-z_]\w*)"\s*,\s*Lua[A-Za-z_]', rd(p)))
    return reg


def lua_files(root):
    if os.path.isfile(root):
        return [root]
    out = []
    for dp, _, fs in os.walk(root):
        out += [os.path.join(dp, f) for f in fs if f.lower().endswith(".lua")]
    return out


BS = chr(92)


def resolve(inc):
    inc = inc.replace("/", BS)
    while BS + BS in inc:
        inc = inc.replace(BS + BS, BS)
    inc = inc.lstrip(BS)
    if inc.lower().startswith("script" + BS):
        inc = inc[7:]
    p = os.path.join(LNX, inc)
    return p if os.path.exists(p) else None


def closure(roots):
    seen, stack = set(), []
    for r in roots:
        stack += lua_files(r)
    files = []
    while stack:
        p = stack.pop()
        k = p.lower()
        if k in seen:
            continue
        seen.add(k)
        files.append(p)
        t = rd(p)
        for m in re.finditer(r'Include\s*\(\s*(?:"([^"]+)"|\[\[([^\]]+)\]\])', t):
            q = resolve(m.group(1) or m.group(2))
            if q and q.lower() not in seen:
                stack.append(q)
    return files


def in_project(p):
    rel = os.path.relpath(p, LNX)
    for cand in (os.path.join(SRV, "script", rel),
                 os.path.join(SRV, "scriptjx2", rel),
                 os.path.join(SRV, "scriptjx2", "tong_vn", rel[5:]) if rel.lower().startswith("tong\\") else ""):
        if cand and os.path.exists(cand):
            return cand
    return None


def defs_and_calls(files):
    defs, calls = {}, {}
    for p in files:
        t = re.sub(r'--[^\n]*', '', rd(p))
        for m in re.finditer(r'function\s+([A-Za-z_]\w*)\s*\(', t):
            defs.setdefault(m.group(1), []).append(p)
        for m in re.finditer(r'([A-Za-z_]\w*)\s*=\s*function\s*\(', t):
            defs.setdefault(m.group(1), []).append(p)
        for m in re.finditer(r'(?<![.:\w])([A-Za-z_]\w*)\s*\(', t):
            if m.group(1) not in LUA_KW:
                calls.setdefault(m.group(1), []).append((p, t[:m.start()].count("\n") + 1))
    return defs, calls


def main():
    name, roots = sys.argv[1], [os.path.join(LNX, r.replace("/", "\\")) for r in sys.argv[2:]]
    files = closure(roots)
    reg = registered()
    elf = set()
    for e in ELF:
        elf |= set(re.findall(r'0x[0-9A-Fa-f]+\s+(\S+)', rd(e)))
    defs, calls = defs_and_calls(files)

    have, missing = [], []
    for p in sorted(files):
        (have if in_project(p) else missing).append(os.path.relpath(p, LNX))

    print("=" * 78)
    print("### %s" % name)
    print("bao dong: %d tep .lua  |  du an DA CO %d  |  CHUA CO %d" % (len(files), len(have), len(missing)))
    print("=" * 78)
    print("\n--- TEP CHUA CO TRONG DU AN (%d) ---" % len(missing))
    for f in missing:
        print("   " + f.replace("\\", "/"))
    print("\n--- TEP DA CO (%d, chi liet ke) ---" % len(have))
    print("   " + "  ".join(f.replace("\\", "/") for f in have))

    missset = set(os.path.join(LNX, f) for f in missing)
    eng, unk = [], []
    for n, sites in calls.items():
        if n in defs or n in reg:
            continue
        sites = [s for s in sites if s[0] in missset]   # chi tinh call site o TEP CHUA CO
        if not sites:
            continue
        (eng if n in elf else unk).append((n, sites))
    print("\n--- [ENG] HAM ENGINE BAN LINUX CO / DU AN CHUA DANG KY: %d ---" % len(eng))
    for n, s in sorted(eng, key=lambda x: -len(x[1])):
        p, l = s[0]
        print("  %-30s x%-4d %s:%d" % (n, len(s), os.path.relpath(p, LNX).replace("\\", "/"), l))
    print("\n--- [???] khong ai dinh nghia, khong co trong ELF: %d ---" % len(unk))
    for n, s in sorted(unk, key=lambda x: -len(x[1]))[:40]:
        p, l = s[0]
        print("  %-30s x%-4d %s:%d" % (n, len(s), os.path.relpath(p, LNX).replace("\\", "/"), l))


main()
