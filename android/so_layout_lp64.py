# -*- coding: utf-8 -*-
r"""[ANDROID 08/09] Ra bo cuc struct LP64: bien dich moi TU cua ban Android bang clang NDK (aarch64, -fsyntax-only,
-Xclang -fdump-record-layouts-complete) roi liet ke moi struct/class/union co THANH VIEN kieu 'long' / 'unsigned long' /
'wchar_t' / 'long double' (8 byte tren Android, 4 byte tren Windows) -> neu la struct tren dia hay goi mang thi Android doc SAI
(vi du XPackFileHeader lam sap khi mo pak). Bao cao: android/build/lp64/lp64_layout.txt (ten struct | truong | TU).
Chay: python android/so_layout_lp64.py [--proj engine,core,...] [--jobs 6]
"""
import io, os, re, sys, subprocess, glob, collections, argparse
from concurrent.futures import ThreadPoolExecutor
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "Sources")
NDK = sorted(glob.glob(r"C:\Users\nguye\AppData\Local\Android\Sdk\ndk\*"))[-1]
LLVM = os.path.join(NDK, "toolchains", "llvm", "prebuilt", "windows-x86_64")
CLANGXX = os.path.join(LLVM, "bin", "clang++.exe")
SYSROOT = os.path.join(LLVM, "sysroot")
ap = argparse.ArgumentParser()
ap.add_argument("--proj", default="engine,core,commonlib,rainbow,represent3,s3client")
ap.add_argument("--jobs", type=int, default=6)
A = ap.parse_args()
COMMON_DEFS = "JX_PLATFORM_SDL JX_POSIX JX_ANDROID JX_NO_DIRECTX JX_D3D9MINI _WIN64 NDEBUG _MBCS USE_STANDALONE_SPR _LIBCPP_ENABLE_CXX17_REMOVED_FEATURES _LIBCPP_ENABLE_CXX20_REMOVED_FEATURES".split()
COMMON_INCS = [os.path.join(SRC, "Engine", "Src", "Platform", "winshim"), os.path.join(SRC, "Engine", "Src", "Platform"),
               os.path.join(SRC, "Engine", "Src", "Platform", "posixssl"), os.path.join(ROOT, "ThirdParty", "miniaudio"), os.path.join(ROOT, "ThirdParty", "stb"),
               os.path.join(ROOT, "ThirdParty", "SDL3-src", "SDL3-3.2.14", "include")]
EXTRA_INCS = {"represent3": [os.path.join(SRC, "Represent", "Represent3")],
              "core": [os.path.join(SRC, "Core", "vcpkg_installed", "x64-windows-static", "x64-windows-static", "include")]}
OUT = os.path.join(ROOT, "android", "build", "lp64")
os.makedirs(OUT, exist_ok=True)
BAD = re.compile(r"(^|[^\w])(unsigned\s+long|long\s+double|long|wchar_t|WCHAR)(?!\s+long)(?![\w])")
SKIP_REC = re.compile(r"^(std::|__|SDL_|ma_|stbi|lua|Lua|nlohmann|ucl|Vk|_Vk|json|\(unnamed (struct|class|union) at [^)]*(SDL3|miniaudio|stb|sysroot|nlohmann|posixssl|Platform)[^)]*\))")

def doc_list(key):
    txt = io.open(os.path.join(ROOT, "android", "lists", key + ".cmake"), encoding="utf-8").read()
    files = re.findall(r"\$\{JX_ROOT\}/(\S+)", txt.split("set(JX_%s_DEFS" % key.upper())[0])
    defs = re.search(r"set\(JX_%s_DEFS ([^)]*)\)" % key.upper(), txt).group(1).split()
    incs = re.findall(r"\$\{JX_ROOT\}/(\S+)", txt.split("set(JX_%s_INCS" % key.upper())[1].split(")")[0])
    return [os.path.join(ROOT, f) for f in files if f.lower().endswith((".cpp", ".cxx", ".cc"))], defs, [os.path.join(ROOT, i) for i in incs]

def cmd_for(f, defs, incs):
    c = [CLANGXX, "--target=aarch64-linux-android24", "--sysroot=" + SYSROOT, "-fsyntax-only", "-x", "c++", "-std=gnu++17", "-fms-extensions", "-fsigned-char",
         "-U__linux", "-w", "-Wno-register", "-Wno-address-of-temporary", "-ferror-limit=0", "-fno-caret-diagnostics", "-Xclang", "-fdump-record-layouts"]   # -complete lam clang 14 sap
    for d in COMMON_DEFS + defs: c.append("-D" + d)
    for i in COMMON_INCS + incs: c.append("-I" + i)
    c.append(f)
    return c

REC = re.compile(r"^\s*0 \| (struct|class|union) (.+?)\s*$")
FIELD = re.compile(r"^\s*(\d+) \|\s+(.+?)\s+(\w+)\s*$")   # "  4 |   unsigned long uCount"
SZ = re.compile(r"\[sizeof=(\d+)")
def parse_dump(txt):
    recs = {}   # name -> (sizeof, [bad fields])
    cur = None; fields = []
    for l in txt.splitlines():
        if l.startswith("*** Dumping AST Record Layout"): cur = None; fields = []; continue
        if cur is None:
            m = REC.match(l)
            if m: cur = m.group(2); fields = []
            continue
        m2 = SZ.search(l)
        if m2:
            if fields: recs[cur] = (int(m2.group(1)), fields)
            cur = None; continue
        m3 = FIELD.match(l)
        if m3 and BAD.search(m3.group(2)) and not m3.group(2).strip().startswith(("struct", "class", "union")):
            fields.append("%s %s@%s" % (m3.group(2).strip(), m3.group(3), m3.group(1)))
    return recs

def run_one(args):
    f, defs, incs = args
    try:
        r = subprocess.run(cmd_for(f, defs, incs), capture_output=True, text=True, errors="replace", timeout=600)
        out = r.stdout + r.stderr
    except Exception as e:
        out = "ERR " + str(e)
    return f, parse_dump(out), ("fatal error" in out)

jobs = []
for key in A.proj.split(","):
    files, defs, incs = doc_list(key)
    for f in files: jobs.append((f, defs, incs + EXTRA_INCS.get(key, [])))
print("TU:", len(jobs))
found = {}; where = collections.defaultdict(set); fatal = []
with ThreadPoolExecutor(max_workers=A.jobs) as ex:
    for f, recs, bad in ex.map(run_one, jobs):
        if bad: fatal.append(os.path.relpath(f, SRC))
        for n, (sz, fl) in recs.items():
            if SKIP_REC.match(n): continue
            found[n] = (sz, fl); where[n].add(os.path.relpath(f, SRC))
L = ["# Struct co thanh vien long/unsigned long/wchar_t (LP64: 8 byte tren Android, 4 tren Windows): %d struct; TU fatal: %d" % (len(found), len(fatal)), ""]
for n in sorted(found):
    sz, fl = found[n]
    L.append("%-55s sizeof=%-6d %s  | %s" % (n, sz, "; ".join(fl), "; ".join(sorted(where[n])[:2])))
L += ["", "# fatal:"] + ["  " + x for x in fatal[:60]]
io.open(os.path.join(OUT, "lp64_layout.txt"), "w", encoding="utf-8", newline="\n").write("\n".join(L) + "\n")
print("\n".join(L[:150]))
print("-> android/build/lp64/lp64_layout.txt")
