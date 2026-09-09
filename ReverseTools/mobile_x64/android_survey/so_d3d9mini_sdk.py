# -*- coding: utf-8 -*-
"""[GPU 08/09] So d3d9mini.h voi SDK that: (1) thu tu vtable moi giao dien, (2) gia tri moi hang enum/#define, (3) sizeof struct.
Cach: sinh 1 chuong trinh C++ in ten=gia tri, bien dich 2 lan (SDK that / mini) bang cl, diff."""
import io, os, re, subprocess, sys
REP3 = r"D:\GAMEDEVNEW_wt_mobile\Sources\Represent\Represent3"
MINI = os.path.join(REP3, "d3d9mini.h")
SDK = r"C:\Program Files (x86)\Windows Kits\10\include\10.0.26100.0\shared"
DXI = r"D:\GAMEDEVNEW_wt_mobile\Sources\packages\Microsoft.DXSDK.D3DX.9.29.952.8\build\native\include"
VC = r"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "so_mini")
os.makedirs(OUT, exist_ok=True)
mini = io.open(MINI, encoding="latin-1").read()

# ---- (1) vtable: mini
def vt_mini(src):
    r = {}
    for m in re.finditer(r"struct (I\w+) : public (I\w+)\s*\{(.*?)\n\};", src, re.S):
        names = re.findall(r"virtual\s+[\w\s\*]+?__stdcall\s+(\w+)\s*\(", m.group(3))
        r[m.group(1)] = (m.group(2), names)
    return r
def vt_sdk(src):
    r = {}
    for m in re.finditer(r"DECLARE_INTERFACE_\((\w+),\s*(\w+)\)\s*\{(.*?)\n\};", src, re.S):
        body = m.group(3)
        names = re.findall(r"STDMETHOD(?:_\(\s*[\w\s\*]+?,\s*(\w+)\s*\)|\(\s*(\w+)\s*\))", body)
        names = [a or b for a, b in names]
        r[m.group(1)] = (m.group(2), names)
    return r
vm = vt_mini(mini); vs = vt_sdk(io.open(os.path.join(SDK, "d3d9.h"), encoding="latin-1").read())
bad = 0
for name, (base, names) in vm.items():
    if name not in vs: print("VTABLE: SDK khong co", name); continue
    sb, sn = vs[name]
    if sb != base: print("VTABLE base khac", name, base, "!=", sb); bad += 1
    # SDK block bat dau bang QueryInterface/AddRef/Release (IUnknown) -> bo 3 dau
    sn2 = sn[3:] if sn[:3] == ["QueryInterface", "AddRef", "Release"] else sn
    # bo cac phuong thuc cua base (SDK liet ke lai toan bo ke thua) : tim doan cuoi khop
    if base != "IUnknown" and base in vs:
        nb = len(vt_sdk_names := (vs[base][1][3:] if vs[base][1][:3] == ["QueryInterface", "AddRef", "Release"] else vs[base][1]))
        sn2 = sn2[nb:]
    if sn2 != names:
        bad += 1
        print("VTABLE KHAC", name, ": SDK", len(sn2), "mini", len(names))
        for i in range(max(len(sn2), len(names))):
            a = sn2[i] if i < len(sn2) else "-"; b = names[i] if i < len(names) else "-"
            if a != b: print("   #%d SDK %s | mini %s" % (i, a, b))
    else:
        print("VTABLE OK", name, len(names), "phuong thuc")
print("--- vtable loi:", bad)

# ---- (2) hang so + (3) sizeof
consts = []
for m in re.finditer(r"typedef enum \w+\s*\{(.*?)\}", mini, re.S):
    consts += re.findall(r"\b(D3D[A-Z0-9_]+)\s*=", m.group(1))
for m in re.finditer(r"^#define\s+(D3D[A-Z0-9_]+)\s+\(?\s*(?:0x[0-9A-Fa-f]+|\d+|\(?\s*1\s*<<|\(\s*D3D)", mini, re.M):
    consts.append(m.group(1))
consts = sorted(set(consts) - {"D3DFVF_TEXCOORDSIZE1", "D3DFVF_TEXCOORDSIZE2", "D3DFVF_TEXCOORDSIZE3", "D3DFVF_TEXCOORDSIZE4"})
structs = [s for s in re.findall(r"\}\s*(D3D[A-Z0-9_]+);", mini)] + ["D3DXVECTOR3", "D3DXVECTOR4", "D3DXMATRIX", "D3DCOLOR", "D3DMATRIX"]
structs = sorted(set(structs))
prog = ["#include <windows.h>", "#include <stdio.h>", "#ifdef JX_MINI", '#include "d3d9mini.h"', "#else", "#include <d3d9types.h>", "#include <d3d9.h>", "#include <d3d9caps.h>", "#include <d3dx9.h>", "#endif",
        "int main(){"]
for c in consts: prog.append('#ifdef %s\n printf("%s=%%lld\\n", (long long)(%s));\n#else\n printf("%s=%%lld\\n", (long long)(%s));\n#endif' % (c, c, c, c, c))
for st in structs: prog.append(' printf("sizeof(%s)=%%u\\n", (unsigned)sizeof(%s));' % (st, st))
prog.append(' printf("D3DCOLOR_ARGB(1,2,3,4)=%08X\\n", (unsigned)D3DCOLOR_ARGB(1,2,3,4));')
prog.append(' printf("D3DCOLOR_XRGB(2,3,4)=%08X\\n", (unsigned)D3DCOLOR_XRGB(2,3,4));')
prog.append(' printf("D3DFVF_TEXCOORDSIZE2(1)=%08X\\n", (unsigned)D3DFVF_TEXCOORDSIZE2(1));')
prog.append(' printf("D3DTS_TEXTURE0=%d\\n", (int)D3DTS_TEXTURE0);')
prog.append(' printf("D3DTS_WORLDMATRIX(0)=%d\\n", (int)D3DTS_WORLDMATRIX(0));')
prog.append(' printf("D3DXToRadian(180)=%.5f\\n", (double)D3DXToRadian(180));')
prog.append(" return 0; }")
src = os.path.join(OUT, "so.cpp")
io.open(src, "w", encoding="ascii", newline="\n").write("\n".join(prog) + "\n")
print("hang so:", len(consts), "struct:", len(structs))
cmd = ('call "%s" >nul && cd /d "%s" && cl /nologo /EHsc /W0 /I"%s" /I"%s" so.cpp /Fe:so_that.exe >so_that_build.txt 2>&1'
       ' && cl /nologo /EHsc /W0 /DJX_MINI /DJX_D3D9MINI /I"%s" so.cpp /Fe:so_mini.exe >so_mini_build.txt 2>&1') % (VC, OUT, SDK, DXI, REP3)
io.open(os.path.join(OUT, "so.cmd"), "w", encoding="ascii", newline="\r\n").write(cmd + "\n")
r = subprocess.run(["cmd", "/c", os.path.join(OUT, "so.cmd")], capture_output=True, text=True, errors="replace")
print("rc", r.returncode)
for f in ("so_that_build.txt", "so_mini_build.txt"):
    p = os.path.join(OUT, f)
    if os.path.exists(p):
        t = io.open(p, encoding="utf-8", errors="replace").read()
        errs = [l for l in t.splitlines() if "error" in l]
        if errs: print(f, ":", "\n".join(errs[:15]))
import time
def chay(exe):
    for i in range(5):
        try:
            q = subprocess.run([os.path.join(OUT, exe)], capture_output=True, text=True)
            if q.returncode == 0 and q.stdout: return q.stdout
        except OSError: pass
        time.sleep(1)
    raise SystemExit("khong chay duoc " + exe)
if r.returncode == 0:
    a = dict(l.split("=", 1) for l in chay("so_that.exe").splitlines() if "=" in l)
    b = dict(l.split("=", 1) for l in chay("so_mini.exe").splitlines() if "=" in l)
    khac = [(k, a.get(k), b.get(k)) for k in sorted(set(a) | set(b)) if a.get(k) != b.get(k)]
    print("so sanh:", len(a), "gia tri; KHAC:", len(khac))
    for k, x, y in khac: print("  ", k, "SDK", x, "| mini", y)
