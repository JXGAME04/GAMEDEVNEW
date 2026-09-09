# -*- coding: utf-8 -*-
r"""[ANDROID 08/09] Sinh danh sach nguon / dinh nghia / include cho CMake Android tu chinh cac vcxproj "SDL flavor"
(cung nguon voi GameSDL.exe tren Windows) -> android/lists/<proj>.cmake. Loai bo cac TU chi-Windows (DirectX, IME,
video, AntiHack, CrashLog, khay, PerfHud, JxReplay - xem TONGHOP_NDK.md muc 4) + D3D9on11* + KJpegLib_wic.
Chay lai moi khi vcxproj doi:  python android/gen_lists.py
"""
import io, os, re, collections
import xml.etree.ElementTree as ET
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "Sources")
OUT = os.path.join(ROOT, "android", "lists")
NS = {"m": "http://schemas.microsoft.com/developer/msbuild/2003"}
DROP_DEFS = {"WIN32", "_WINDOWS", "_USRDLL", "_LIB", "WINVER", "_WIN32_WINNT", "SDL_MAIN_HANDLED", "_CRT_SECURE_NO_WARNINGS"}

# TU chi Windows theo ban chat (TONGHOP_NDK.md muc 4) + lop D3D11 + JPEG WIC
WINONLY_RE = re.compile(r"(engine/src/(kddraw|kdinput|kdsound|kderror|kavifile|kmp3music|kmpgmusic|kmp4audio|kmp4movie|kmp4video|"
                        r"kvideo|kime|kfiledialog|kkeyboard|kmouse|knetclient|knetserver|knetservernode|knetthread)\.cpp$"
                        r"|s3client/(crashlog\.cpp|antihack/|ui/traymode\.cpp|ui/uicase/uiplayvideo\.cpp|ui/perfhud\.cpp|jxreplay\.cpp)"
                        r"|represent3/d3d9on11|jpglib/src/kjpeglib_wic\.cpp|jpglib/src/kjpeg\w+\.c$)", re.I)
COMMON_ONLY = ["SocketClient.cpp", "Socket.cpp", "EventSelect.cpp", "Event.cpp", "ManualResetEvent.cpp", "Thread.cpp", "CriticalSection.cpp",
               "IOBuffer.cpp", "NodeList.cpp", "Utils.cpp", "UsesWinsock.cpp", "Exception.cpp", "Win32Exception.cpp", "KSG_EncodeDecode.cpp"]
PROJ = collections.OrderedDict([
    ("engine",     dict(vcx="Engine/Engine.vcxproj",                      cfg="ReleaseSDL|x64",        only=None)),
    ("core",       dict(vcx="Core/Core.vcxproj",                          cfg="Client ReleaseSDL|x64", only=None)),
    ("s3client",   dict(vcx="S3Client/S3Client.vcxproj",                  cfg="ReleaseSDL|x64",        only=None)),
    ("represent3", dict(vcx="Represent/Represent3/Represent3.vcxproj",    cfg="ReleaseSDL|x64",        only=None)),
    ("commonlib",  dict(vcx="MultiServer/Common/Common.vcxproj",          cfg="ReleaseSDL|x64",        only=COMMON_ONLY)),
    ("rainbow",    dict(vcx="MultiServer/Rainbow/Rainbow.vcxproj",        cfg="ReleaseSDL|x64",        only=None)),
    ("lua54",      dict(vcx="Library/Lua54/Lua54Dll.vcxproj",             cfg="Release|x64",           only=None)),
    ("jpglib",     dict(vcx="JpgLib/JpgLib.vcxproj",                      cfg="Release|x64",           only=None)),
    ("filtertext", dict(vcx="FilterText/FilterText.vcxproj",              cfg="Release|x64",           only=None)),
])
# loai tru them (duong dan tuong doi ROOT, / , khong phan biet hoa thuong) - them vao android/loai_tru.txt, moi dong 1 tep
EXTRA_EXCL = set()
p_excl = os.path.join(ROOT, "android", "loai_tru.txt")
if os.path.isfile(p_excl):
    for l in io.open(p_excl, encoding="utf-8"):
        l = l.split("#")[0].strip().replace("\\", "/").lower()
        if l: EXTRA_EXCL.add(l)

def cond_match(cond, key):
    if not cond: return True
    c = cond.replace(" ", ""); k = ("'%s'" % key).replace(" ", "")
    return (k not in c) if "!=" in c else (k in c)

def expand(s, projdir, cfg, plat):
    s = s.replace("$(SolutionDir)", SRC + "\\").replace("$(ProjectDir)", projdir + "\\").replace("$(Configuration)", cfg).replace("$(Platform)", plat)
    s = s.replace("$(JX_SDL3_DIR)", os.path.join(ROOT, "ThirdParty", "SDL3"))
    return re.sub(r"%\([A-Za-z]+\)", "", s)

def parse(vcx, key):
    cfg, plat = key.split("|")
    projdir = os.path.dirname(vcx)
    root = ET.parse(vcx).getroot()
    defs, incs = [], []
    for idg in root.findall("m:ItemDefinitionGroup", NS):
        if not cond_match(idg.get("Condition", ""), key): continue
        cl = idg.find("m:ClCompile", NS)
        if cl is None: continue
        for tag, dst in (("PreprocessorDefinitions", defs), ("AdditionalIncludeDirectories", incs)):
            el = cl.find("m:" + tag, NS)
            if el is not None and el.text:
                dst.extend([x.strip() for x in expand(el.text, projdir, cfg, plat).split(";") if x.strip() and not x.startswith("%(")])
    files = []
    for ig in root.findall("m:ItemGroup", NS):
        for cl in ig.findall("m:ClCompile", NS):
            f = cl.get("Include")
            if not f: continue
            if any(cond_match(ex.get("Condition", ""), key) and (ex.text or "").strip().lower() == "true" for ex in cl.findall("m:ExcludedFromBuild", NS)):
                continue
            files.append(os.path.normpath(os.path.join(projdir, f)))
    return projdir, defs, incs, files

def rel(p):
    return os.path.relpath(p, ROOT).replace("\\", "/")

os.makedirs(OUT, exist_ok=True)
tong = 0
for key, P in PROJ.items():
    vcx = os.path.join(SRC, P["vcx"])
    projdir, defs, incs, files = parse(vcx, P["cfg"])
    if P["only"]:
        files = [f for f in files if os.path.basename(f) in P["only"]]
    keep, loai = [], []
    for f in files:
        r = rel(f)
        if WINONLY_RE.search(r.lower()) or r.lower() in EXTRA_EXCL: loai.append(r)
        elif not os.path.isfile(f): loai.append(r + " (KHONG TON TAI)")
        else: keep.append(r)
    defs2 = [d for d in defs if d.split("=")[0] not in DROP_DEFS]
    incs2 = []
    for i in incs:
        i = os.path.normpath(i if os.path.isabs(i) else os.path.join(projdir, i))
        if os.path.isdir(i) and "SDL3" not in i and "vcpkg" not in i.lower():
            r = rel(i)
            if r not in incs2: incs2.append(r)
    U = key.upper()
    L = ["# Sinh boi android/gen_lists.py tu %s [%s] - KHONG sua tay" % (P["vcx"], P["cfg"]),
         "set(JX_%s_SOURCES" % U] + ["    ${JX_ROOT}/" + f for f in keep] + [")",
         "set(JX_%s_DEFS %s)" % (U, " ".join(defs2)),
         "set(JX_%s_INCS" % U] + ["    ${JX_ROOT}/" + i for i in incs2] + [")"]
    cfiles = [f for f in keep if f.lower().endswith(".c")]
    if cfiles:
        L += ["set_source_files_properties(" + " ".join("${JX_ROOT}/" + f for f in cfiles) + " PROPERTIES LANGUAGE C)"]
    L += ["# loai (%d): %s" % (len(loai), "; ".join(loai))]
    io.open(os.path.join(OUT, key + ".cmake"), "w", encoding="utf-8", newline="\n").write("\n".join(L) + "\n")
    tong += len(keep)
    print("%-11s %3d tep (loai %2d) defs: %s" % (key, len(keep), len(loai), " ".join(defs2)))
print("tong", tong, "TU ->", OUT)
