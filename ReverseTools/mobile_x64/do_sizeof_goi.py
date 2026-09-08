# -*- coding: utf-8 -*-
"""Do sizeof cua moi struct trong Headers/KProtocol.h (va cac header con) o ca x86 lan x64.
Cach dung: python do_sizeof_goi.py <worktree_root>
- Sinh mot chuong trinh C++ nho in "TEN,sizeof" cho tung ten typedef/struct.
- Bien dich bang cl.exe x86 va x64 (goi thang, dat INCLUDE/LIB tay - khong qua vcvarsall), chay, so sanh.
- Ket qua: <scratch>/sizeof_x86.txt, sizeof_x64.txt, sizeof_lech.txt. Nhe (mot don vi bien dich moi kien truc).
"""
import glob, io, os, re, subprocess, sys

ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
OUT = os.path.dirname(os.path.abspath(__file__))
HDR = os.path.join(ROOT, "Headers")
MSVC = sorted(glob.glob(r"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\*"))[-1]
KITS = sorted(glob.glob(r"C:\Program Files (x86)\Windows Kits\10\Include\10.*"))[-1]
KVER = os.path.basename(KITS)
KLIB = r"C:\Program Files (x86)\Windows Kits\10\Lib\%s" % KVER

def env_for(arch):
    e = dict(os.environ)
    e["INCLUDE"] = ";".join([os.path.join(MSVC, "include"), os.path.join(KITS, "ucrt"), os.path.join(KITS, "shared"), os.path.join(KITS, "um")])
    e["LIB"] = ";".join([os.path.join(MSVC, "lib", arch), os.path.join(KLIB, "ucrt", arch), os.path.join(KLIB, "um", arch)])
    e["PATH"] = os.path.join(MSVC, "bin", "Hostx64", "x64") + ";" + e["PATH"]  # de cl x86 (Hostx64\x86) tim duoc dll dung chung
    return e

def strip_comments(t):
    t = re.sub(r'//[^\n]*', '', t)
    t = re.sub(r'/\*.*?\*/', '', t, flags=re.S)
    return t

def names_from(path):
    t = strip_comments(io.open(path, encoding='latin-1').read())
    n = set(re.findall(r'\}\s*([A-Za-z_]\w*)\s*(?:,\s*\*?\s*\w+\s*)*;', t))
    n |= set(re.findall(r'\b(?:struct|union)\s+([A-Za-z_]\w*)\s*\{', t))
    return n

files = ["KProtocol.h", "KProtocolDef.h", "KRelayProtocol.h", "KDiceProtocol.h", "KPartnerProtocol.h", "KMantleProtocol.h"]
names = set()
for f in files:
    p = os.path.join(HDR, f)
    if os.path.exists(p):
        names |= names_from(p)
names = sorted(names)
print("so ten:", len(names))

def gen(names, skip):
    lines = ['#include "KWin32.h"', '#include <stdio.h>', '#include "KProtocol.h"', 'int main(){']
    for n in names:
        if n in skip: continue
        lines.append('    printf("%s,%%u\\n", (unsigned)sizeof(%s));' % (n, n))
    lines.append('    return 0; }')
    return "\n".join(lines)

def build_run(arch, skip):
    src = "sizeof_goi_%s.cpp" % arch
    exe = "sizeof_goi_%s.exe" % arch
    io.open(os.path.join(OUT, src), "w", encoding="latin-1", newline="\n").write(gen(names, skip))
    inc = [os.path.join(ROOT, "Sources", "Engine", "Src"), os.path.join(ROOT, "Sources", "Engine", "include"),
           os.path.join(ROOT, "Sources", "Core", "Src"), HDR]
    cl = os.path.join(MSVC, "bin", "Hostx64", arch, "cl.exe")
    cmd = [cl, "/nologo", "/EHsc", "/W0", "/source-charset:windows-1258", "/execution-charset:windows-1258", "/DWIN32", "/D_WINDOWS", "/DS3_CLIENT"]
    cmd += ["/I" + i for i in inc] + ["/Fo" + src.replace(".cpp", ".obj"), "/Fe" + exe, src]
    r = subprocess.run(cmd, cwd=OUT, env=env_for(arch), capture_output=True, text=True, errors='replace')
    return r, os.path.join(OUT, exe)

skip = set()
for arch in ("x86", "x64"):
    ok = False
    for attempt in range(8):
        r, exe = build_run(arch, skip)
        txt = r.stdout + r.stderr
        if r.returncode == 0 and os.path.exists(exe):
            ok = True; break
        bad = set()
        for line in txt.splitlines():
            m = re.search(r"sizeof\((\w+)\)", line)
            if "error" in line and m: bad.add(m.group(1))
            m2 = re.search(r"error C2065: '(\w+)'", line)
            if m2: bad.add(m2.group(1))
        if not bad:
            print(arch, "bien dich loi khong nhan ra ten:\n", txt[-3000:]); sys.exit(1)
        skip |= bad
        print(arch, "bo qua ten khong do duoc:", sorted(bad)[:30])
    if not ok:
        print(arch, "that bai"); sys.exit(1)
    out = subprocess.run([exe], capture_output=True, text=True).stdout
    io.open(os.path.join(OUT, "sizeof_%s.txt" % arch), "w", encoding="utf-8", newline="\n").write(out)
    print(arch, "do duoc", len(out.splitlines()), "struct")

a = dict(l.split(",") for l in io.open(os.path.join(OUT, "sizeof_x86.txt"), encoding="utf-8").read().splitlines() if "," in l)
b = dict(l.split(",") for l in io.open(os.path.join(OUT, "sizeof_x64.txt"), encoding="utf-8").read().splitlines() if "," in l)
diff = [(k, a[k], b.get(k)) for k in a if b.get(k) != a[k]]
print("LECH x86/x64:", len(diff), "/", len(a))
for k, va, vb in diff[:80]:
    print("  ", k, va, "->", vb)
io.open(os.path.join(OUT, "sizeof_lech.txt"), "w", encoding="utf-8", newline="\n").write("\n".join("%s %s %s" % d for d in diff))
