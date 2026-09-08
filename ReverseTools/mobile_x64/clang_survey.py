# -*- coding: utf-8 -*-
"""[PHA 1 08/09] Khao sat 'biên dịch bằng clang' cho client (Engine, Core client, Represent3, S3Client): doc vcxproj lay cờ từng tệp của cấu hình
x64, chạy clang-cl (LLVM của NDK r25, chế độ cl, header MSVC) CHỈ KIỂM CÚ PHÁP (-fsyntax-only), gom lỗi, phân loại theo mẫu thông báo
=> danh sách port thật (MSVC-ism, __asm, kiểu, ...). Không sinh obj, không link.
Cách dùng: python clang_survey.py <worktree_root> [engine,core,rep3,s3client] [so_luong_song_song]
Ket qua: <scratch>/clang_survey/<proj>_raw.log (nguyen van), <proj>_tomtat.txt, TONGHOP.md
Chạy ở ưu tiên thấp (gọi qua PowerShell đã hạ BelowNormal) và ngoài giờ trận."""
import glob, io, os, re, subprocess, sys, time, json, collections
import xml.etree.ElementTree as ET
from concurrent.futures import ThreadPoolExecutor

ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
WHICH = (sys.argv[2] if len(sys.argv) > 2 else "engine,core,rep3,s3client").split(",")
NPAR = int(sys.argv[3]) if len(sys.argv) > 3 else 3
SRC = os.path.join(ROOT, "Sources")
OUT = os.path.join(r"C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\f7fe3690-d1f7-481d-b4f6-845330c7c14f\scratchpad", "clang_survey")
os.makedirs(OUT, exist_ok=True)
NDK = sorted(glob.glob(r"C:\Users\nguye\AppData\Local\Android\Sdk\ndk\*"))[-1]
CLANGCL = os.path.join(NDK, "toolchains", "llvm", "prebuilt", "windows-x86_64", "bin", "clang-cl.exe")
MSVC = sorted(glob.glob(r"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\*"))[-1]
KITS = sorted(glob.glob(r"C:\Program Files (x86)\Windows Kits\10\Include\10.*"))[-1]
NS = {"m": "http://schemas.microsoft.com/developer/msbuild/2003"}
EXTRA_INC = [x for x in os.environ.get("CLANG_SURVEY_EXTRA_INC", "").split(";") if x]   # vd include cua vcpkg (MSBuild tu them, o day phai them tay)

PROJ = {
    "engine":   (os.path.join(SRC, "Engine", "Engine.vcxproj"), "Release", "x64"),
    "core":     (os.path.join(SRC, "Core", "Core.vcxproj"), "Client Release", "x64"),
    "rep3":     (os.path.join(SRC, "Represent", "Represent3", "Represent3.vcxproj"), "Release", "x64"),
    "s3client": (os.path.join(SRC, "S3Client", "S3Client.vcxproj"), "Release", "x64"),
}

def env():
    e = dict(os.environ)
    e["INCLUDE"] = ";".join([os.path.join(MSVC, "include"), os.path.join(KITS, "ucrt"), os.path.join(KITS, "shared"), os.path.join(KITS, "um"), os.path.join(KITS, "winrt")])
    return e

def cond_match(cond, cfg, plat):
    if not cond: return True
    return ("'%s|%s'" % (cfg, plat)).replace(" ", "") in cond.replace(" ", "")   # bo dau cach CA HAI phia ('Client Release|x64')

def expand(s, projdir, cfg, plat):
    s = s.replace("$(SolutionDir)", SRC + "\\").replace("$(ProjectDir)", projdir + "\\").replace("$(Configuration)", cfg).replace("$(Platform)", plat)
    s = re.sub(r"%\([A-Za-z]+\)", "", s)
    return s

def parse(vcx, cfg, plat):
    projdir = os.path.dirname(vcx)
    tree = ET.parse(vcx); root = tree.getroot()
    defs, incs, forced, std, undef = [], [], [], None, []
    for idg in root.findall("m:ItemDefinitionGroup", NS):
        if not cond_match(idg.get("Condition", ""), cfg, plat): continue
        cl = idg.find("m:ClCompile", NS)
        if cl is None: continue
        for tag, dst in (("PreprocessorDefinitions", defs), ("AdditionalIncludeDirectories", incs), ("ForcedIncludeFiles", forced), ("UndefinePreprocessorDefinitions", undef)):
            el = cl.find("m:" + tag, NS)
            if el is not None and el.text:
                dst.extend([x.strip() for x in expand(el.text, projdir, cfg, plat).split(";") if x.strip()])
        el = cl.find("m:LanguageStandard", NS)
        if el is not None and el.text: std = el.text.strip()
    charset = None
    for pg in root.findall("m:PropertyGroup", NS):
        if not cond_match(pg.get("Condition", ""), cfg, plat): continue
        el = pg.find("m:CharacterSet", NS)
        if el is not None and el.text: charset = el.text.strip()
    files = []
    for ig in root.findall("m:ItemGroup", NS):
        for cl in ig.findall("m:ClCompile", NS):
            f = cl.get("Include")
            if not f: continue
            excl = False
            for ex in cl.findall("m:ExcludedFromBuild", NS):
                if cond_match(ex.get("Condition", ""), cfg, plat) and (ex.text or "").strip().lower() == "true": excl = True
            if excl: continue
            files.append(os.path.normpath(os.path.join(projdir, f)))
    return projdir, defs, incs, forced, std, charset, files

def run_one(cmd_base, f, projdir):
    cmd = cmd_base + [f]
    t0 = time.time()
    try:
        r = subprocess.run(cmd, cwd=projdir, env=env(), capture_output=True, text=True, errors="replace", timeout=600)
        out = r.stdout + r.stderr; code = r.returncode
    except subprocess.TimeoutExpired:
        out = "TIMEOUT"; code = -9
    return f, code, out, time.time() - t0

def normalize(msg):
    m = msg
    m = re.sub(r"'[^']*'", "'X'", m)
    m = re.sub(r"\b\d+\b", "N", m)
    return m

def survey(key):
    vcx, cfg, plat = PROJ[key]
    projdir, defs, incs, forced, std, charset, files = parse(vcx, cfg, plat)
    cmd = [CLANGCL, "-fsyntax-only", "/nologo", "-m64", "-fms-compatibility-version=19.44", "/EHsc", "/W0", "-ferror-limit=0",
           "-Wno-everything", "/D_CRT_SECURE_NO_WARNINGS", "/D_CRT_NONSTDC_NO_WARNINGS",
           "/D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH"]   # STL MSVC 14.44 doi clang >= 16, NDK r25 la clang 14
    for extra in EXTRA_INC: cmd.append("/I" + extra)
    # include cua vcpkg: MSBuild tu them (manifest: <proj>cpkg_installed\<triplet>\include; classic: VCPKG_ROOT\installed\<triplet>\include)
    vinc = []
    for pat in (os.path.join(projdir, "vcpkg_installed", "x64-windows-static", "include"), os.path.join(projdir, "vcpkg_installed", "x64-windows", "include"),
                os.path.join(os.environ.get("VCPKG_ROOT", r"E:cpkg-master"), "installed", "x64-windows-static", "include"),
                os.path.join(os.environ.get("VCPKG_ROOT", r"E:cpkg-master"), "installed", "x64-windows", "include"),
                r"C:cpkg\installedd-windows-static\include", r"C:cpkg\installedd-windows\include"):
        if os.path.isdir(pat) and pat not in vinc: vinc.append(pat)
    for v in vinc: cmd.append("/I" + v)
    # DirectX SDK (d3dx9.h cho Represent3): MSBuild lay tu $(DXSDK_DIR) / IncludePath
    dx = glob.glob(os.path.join(SRC, "packages", "Microsoft.DXSDK.D3DX*", "build", "native", "include")) + [os.path.join(os.environ.get("DXSDK_DIR", ""), "Include")] + glob.glob(r"C:\Program Files (x86)\Microsoft DirectX SDK*\Include")   # goi NuGet cua Represent3 truoc
    for d in dx:
        if d and os.path.isdir(d): cmd.append("/I" + d); break
    if std: cmd.append("/std:" + std.replace("stdcpp", "c++"))
    if charset == "MultiByte": cmd.append("/D_MBCS")
    elif charset == "Unicode": cmd += ["/DUNICODE", "/D_UNICODE"]
    for d in defs: cmd.append("/D" + d)
    for u in undef: cmd.append("/U" + u)
    for i in incs: cmd.append("/I" + i)
    for fi in forced: cmd.append("/FI" + fi)
    print("[%s] %s | %s|%s | %d tep | std=%s charset=%s" % (key, os.path.basename(vcx), cfg, plat, len(files), std, charset)); sys.stdout.flush()
    raw = io.open(os.path.join(OUT, key + "_raw.log"), "w", encoding="utf-8", newline="\n")
    raw.write("CMD: " + " ".join(cmd) + "\n\n")
    per_file = {}; cats = collections.Counter(); cat_example = {}; cat_files = collections.defaultdict(set)
    t0 = time.time(); done = 0
    with ThreadPoolExecutor(max_workers=NPAR) as ex:
        for f, code, out, dt in ex.map(lambda f: run_one(cmd, f, projdir), files):
            done += 1
            rel = os.path.relpath(f, SRC)
            errs = [l for l in out.splitlines() if re.search(r": (fatal )?error: ", l)]
            per_file[rel] = (code, len(errs), dt)
            raw.write("=== %s exit=%d errors=%d %.1fs\n%s\n" % (rel, code, len(errs), dt, out))
            for l in errs:
                m = re.search(r": (?:fatal )?error: (.*)$", l)
                if not m: continue
                c = normalize(m.group(1))
                if "Program Files" in l[:80]: c = "[HEADER HE THONG] " + c
                cats[c] += 1; cat_files[c].add(rel)
                cat_example.setdefault(c, l.strip()[:220])
            if done % 25 == 0: print("  ...%d/%d (%.0fs)" % (done, len(files), time.time() - t0)); sys.stdout.flush()
    raw.close()
    nerr_files = sum(1 for v in per_file.values() if v[1] > 0)
    total = sum(v[1] for v in per_file.values())
    with io.open(os.path.join(OUT, key + "_tomtat.txt"), "w", encoding="utf-8", newline="\n") as w:
        w.write("%s %s|%s: %d tep, %d tep co loi, %d loi, %.0fs\n\n" % (key, cfg, plat, len(files), nerr_files, total, time.time() - t0))
        w.write("== LOAI LOI (theo so lan) ==\n")
        for c, n in cats.most_common():
            w.write("%6d  x%-3d tep  %s\n        vd: %s\n" % (n, len(cat_files[c]), c, cat_example[c]))
        w.write("\n== TEP (so loi) ==\n")
        for rel, (code, n, dt) in sorted(per_file.items(), key=lambda kv: -kv[1][1]):
            w.write("%5d  %s\n" % (n, rel))
    print("[%s] xong: %d tep, %d tep co loi, %d loi, %.0fs" % (key, len(files), nerr_files, total, time.time() - t0)); sys.stdout.flush()
    return dict(key=key, files=len(files), err_files=nerr_files, errors=total, cats=cats.most_common(40), cat_files={c: len(cat_files[c]) for c, _ in cats.most_common(40)})

if __name__ == "__main__":
    res = []
    for k in WHICH:
        if k in PROJ: res.append(survey(k))
    with io.open(os.path.join(OUT, "TONGHOP.md"), "w", encoding="utf-8", newline="\n") as w:
        w.write("# Khao sat clang-cl (NDK r25 LLVM 14, -fsyntax-only, header MSVC) - %s\n\n" % time.strftime("%d/%m %H:%M"))
        w.write("| du an | tep | tep co loi | loi |\n|---|---|---|---|\n")
        for r in res: w.write("| %s | %d | %d | %d |\n" % (r["key"], r["files"], r["err_files"], r["errors"]))
        w.write("\n")
        for r in res:
            w.write("## %s - 40 loai loi dau\n\n| so lan | tep | loai |\n|---|---|---|\n" % r["key"])
            for c, n in r["cats"]: w.write("| %d | %d | `%s` |\n" % (n, r["cat_files"][c], c.replace("|", "\\|")))
            w.write("\n")
    print("TONGHOP:", os.path.join(OUT, "TONGHOP.md"))
