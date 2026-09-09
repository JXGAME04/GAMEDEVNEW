# -*- coding: utf-8 -*-
r"""[ANDROID 08/09] Khao sat bien dich CU PHAP (chi -fsyntax-only, khong sinh obj, khong link) ma client bang
clang++ THAT cua Android NDK (--target=aarch64-linux-android24), de uoc luong cong port sang POSIX/Android.

Du an + cau hinh (deu la "SDL flavor" JX_PLATFORM_SDL):
  engine   Sources/Engine/Engine.vcxproj                 ReleaseSDL|x64          (138 tep, 20 tep .c cua ucl)
  core     Sources/Core/Core.vcxproj                     Client ReleaseSDL|x64
  s3client Sources/S3Client/S3Client.vcxproj             ReleaseSDL|x64
  common   Sources/MultiServer/Common/Common.vcxproj     ReleaseSDL|x64  CHI 14 tep (SocketClient, Socket, EventSelect, ...)
  rainbow  Sources/MultiServer/Rainbow/Rainbow.vcxproj   ReleaseSDL|x64  (ClientStage, IClient, Rainbow)
  (Sources/Represent bo qua hoan toan: DirectX, se thay bang Represent4/SDL_GPU)

Cach dung:
  python ndk_survey.py --iter 0                 # vong 0: "thuan" - chi nhanh #ifndef __linux co san, KHONG compat header, KHONG stub
  python ndk_survey.py --iter 1                 # vong >=1: force-include Sources/Engine/Src/Platform/KPosixCompat.h
                                                #   + thu muc stub header Windows (sinh vao scratch, chi de khao sat)
                                                #   + KWin32.h "bong" (shadow, sinh tu tep that: nhanh #else Linux -> #include KPosixCompat.h)
                                                #   + JxNetShim.h "bong" cho common/rainbow (bo 3 typedef trung WSABUF/OVERLAPPED/WSADATA)
  python ndk_survey.py --iter 2 --proj engine,core   # chi mot vai du an
  python ndk_survey.py --iter 2 --no-run        # chi phan tich lai log co san cua vong 2
  python ndk_survey.py --compare                # gom it*/tonghop.json -> SOSANH.md (truoc/sau)
  Tuy chon: --root D:\GAMEDEVNEW_wt_mobile  --jobs 4  --errlimit 200  --scratch <thu muc tam>  --tag <ten vong>

Ket qua (trong scratch/android_survey/it<N>/):
  <proj>_raw.log      nguyen van stderr tung TU (dong "=== <tep> exit=.. errors=.. giay")
  <proj>_tomtat.txt   loai loi + so loi tung tep (cot 'rieng' = loi nam trong chinh tep .cpp, con lai la tu header)
  tonghop.json        so lieu may doc duoc (TU, pass, loi, ky hieu thieu, header thieu, MSVC-ism, include sai chu hoa)
  TONGHOP_it<N>.md    bang tu dong (tieng Viet) de dan vao TONGHOP_NDK.md
KHONG sua bat ky tep nao trong Sources/ (chi doc). Chay uu tien BELOW_NORMAL (may thu co the dang chay tran)."""
import argparse, collections, glob, io, json, os, re, subprocess, sys, time
import xml.etree.ElementTree as ET
from concurrent.futures import ThreadPoolExecutor

# ----------------------------------------------------------------------------------------------------------------
# Tham so
# ----------------------------------------------------------------------------------------------------------------
ap = argparse.ArgumentParser()
ap.add_argument("--root", default=r"D:\GAMEDEVNEW_wt_mobile")
ap.add_argument("--iter", type=int, default=0)
ap.add_argument("--proj", default="engine,core,s3client,common,rainbow")
ap.add_argument("--jobs", type=int, default=4)
ap.add_argument("--errlimit", type=int, default=200)
ap.add_argument("--scratch", default=r"C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\f7fe3690-d1f7-481d-b4f6-845330c7c14f\scratchpad\android_survey")
ap.add_argument("--tag", default="")
ap.add_argument("--compare", action="store_true")
ap.add_argument("--no-run", action="store_true", help="chi phan tich lai log co san cua vong --iter")
A = ap.parse_args()

ROOT = A.root
SRC = os.path.join(ROOT, "Sources")
SRC_N = os.path.normcase(SRC)
NDK = sorted(glob.glob(r"C:\Users\nguye\AppData\Local\Android\Sdk\ndk\*"))[-1]
LLVM = os.path.join(NDK, "toolchains", "llvm", "prebuilt", "windows-x86_64")
CLANGXX = os.path.join(LLVM, "bin", "clang++.exe")
CLANG = os.path.join(LLVM, "bin", "clang.exe")
SYSROOT = os.path.join(LLVM, "sysroot")
TARGET = "aarch64-linux-android24"
SDL3_DIR = os.path.join(ROOT, "ThirdParty", "SDL3")
MINIAUDIO_INC = os.path.join(ROOT, "ThirdParty", "miniaudio")
PLATFORM_DIR = os.path.join(SRC, "Engine", "Src", "Platform")
COMPAT_H = os.path.join(PLATFORM_DIR, "KPosixCompat.h")
KWIN32_REAL = os.path.join(SRC, "Engine", "Src", "KWin32.h")
JXNETSHIM_REAL = os.path.join(SRC, "MultiServer", "Common", "JxNetShim.h")
OUT = os.path.join(A.scratch, "it%d%s" % (A.iter, ("_" + A.tag) if A.tag else ""))
STUB_DIR = os.path.join(A.scratch, "stub_include")
SHADOW_DIR = os.path.join(A.scratch, "shadow")
SHADOW_KWIN32 = os.path.join(SHADOW_DIR, "KWin32.h")
SHADOW_JXNETSHIM = os.path.join(SHADOW_DIR, "JxNetShim.h")
STUB_EXTRA_JSON = os.path.join(A.scratch, "stub_extra.json")
NS = {"m": "http://schemas.microsoft.com/developer/msbuild/2003"}
BELOW_NORMAL = 0x00004000
DROP_DEFS = {"WIN32", "_WINDOWS", "_USRDLL", "_LIB", "WINVER", "_WIN32_WINNT"}     # bo: khong con la Windows

# Tep "chi Windows theo ban chat" (DirectX, cua so Win32, IME, video AVI/MP3/MP4, AntiHack, CrashLog, khay he thong...):
# loi nam trong cac tep nay (ke ca header) duoc tach rieng -> phan con lai moi la backlog port that.
WINONLY_RE = re.compile(r"(engine/src/(kddraw|kdinput|kdsound|kderror|kwin32app|kwin32wnd|kavifile|kmp3music|kmpgmusic|kmp4audio|kmp4movie|kmp4video|"
                        r"kvideo|kime|kfiledialog|kkeyboard|kmouse|knetclient|knetserver|knetservernode|knetthread)\.(cpp|h)$"
                        r"|s3client/(crashlog\.|antihack/|ui/traymode\.|ui/uicase/uiplayvideo\.|ui/perfhud\.|jxreplay\.)|klvideo/|engine/src/kmusic\.h$|engine/src/kwavsound\.h$"
                        r"|headers/dbghelp\.h$)", re.I)

COMMON_ONLY = ["SocketClient.cpp", "Socket.cpp", "EventSelect.cpp", "Event.cpp", "ManualResetEvent.cpp", "Thread.cpp", "CriticalSection.cpp",
               "IOBuffer.cpp", "NodeList.cpp", "Utils.cpp", "UsesWinsock.cpp", "Exception.cpp", "Win32Exception.cpp", "KSG_EncodeDecode.cpp"]
PROJ = collections.OrderedDict([
    ("engine",   dict(vcx="Engine/Engine.vcxproj", cfg="ReleaseSDL", plat="x64", only=None, kwin32=True, netshim=False,
                      vcpkg="Engine/vcpkg_installed/x64-windows-static/x64-windows-static/include", scan=["Engine/Src", "Engine/include"])),
    ("core",     dict(vcx="Core/Core.vcxproj", cfg="Client ReleaseSDL", plat="x64", only=None, kwin32=True, netshim=False,
                      vcpkg="Core/vcpkg_installed/x64-windows-static/x64-windows-static/include", scan=["Core/Src"])),
    ("s3client", dict(vcx="S3Client/S3Client.vcxproj", cfg="ReleaseSDL", plat="x64", only=None, kwin32=True, netshim=False,
                      vcpkg="S3Client/vcpkg_installed/x86-windows-static/x86-windows-static/include", scan=["S3Client"])),
    ("common",   dict(vcx="MultiServer/Common/Common.vcxproj", cfg="ReleaseSDL", plat="x64", only=COMMON_ONLY, kwin32=False, netshim=True,
                      vcpkg=None, scan=["MultiServer/Common"])),
    ("rainbow",  dict(vcx="MultiServer/Rainbow/Rainbow.vcxproj", cfg="ReleaseSDL", plat="x64", only=["ClientStage.cpp", "IClient.cpp", "Rainbow.cpp"],
                      kwin32=False, netshim=True, vcpkg=None, scan=["MultiServer/Rainbow"])),
])

# Header Windows SDK / CRT-MSVC duoc "stub" (tep rong -> #include KPosixCompat.h) CHI DE KHAO SAT, de TU di qua duoc
# dong #include va lo ra loi ky hieu that.  Ban port that phai bo cac #include nay (hoac de KWin32.h bao).
WIN_HEADERS = """windows.h winsock2.h winsock.h ws2tcpip.h mswsock.h wsipx.h windowsx.h winbase.h winuser.h wingdi.h winnt.h windef.h
winerror.h winreg.h winnls.h wincon.h winver.h shellapi.h shlwapi.h shlobj.h commdlg.h commctrl.h richedit.h mmsystem.h mmreg.h msacm.h
vfw.h dsound.h dinput.h ddraw.h d3d9.h d3dx9.h d3d9types.h d3d9caps.h d3d8.h d3dx8.h dxerr.h dxerr8.h dxerr9.h dxdiag.h dxsdkver.h imm.h
objbase.h ole2.h oleauto.h unknwn.h guiddef.h initguid.h basetsd.h atlbase.h atlstr.h atlconv.h atlcomcli.h crtdbg.h io.h direct.h
process.h tchar.h conio.h excpt.h eh.h intrin.h xmmintrin.h emmintrin.h mmintrin.h pmmintrin.h dbghelp.h imagehlp.h tlhelp32.h psapi.h
wininet.h winhttp.h wincrypt.h wintrust.h softpub.h iphlpapi.h ipexport.h iptypes.h icmpapi.h nb30.h lm.h lmcons.h sddl.h aclapi.h
userenv.h shfolder.h shobjidl.h comdef.h comutil.h mlang.h urlmon.h ocidl.h oaidl.h propidl.h powrprof.h setupapi.h winioctl.h winsvc.h
winspool.h dde.h ddeml.h winternl.h ntstatus.h mmdeviceapi.h audioclient.h dwmapi.h uxtheme.h olectl.h zmouse.h versionhelpers.h
fpieee.h share.h sys/timeb.h sys/utime.h new.h iostream.h fstream.h strstrea.h minmax.h wtypes.h rpc.h rpcndr.h propkey.h
strsafe.h dshow.h vmr9.h evr.h mfapi.h wmsdk.h nspapi.h ws2spi.h af_irda.h ipmib.h netioapi.h gdiplus.h winscard.h
lmerr.h ntsecapi.h wbemidl.h activscp.h mshtml.h exdisp.h ntddscsi.h ntddndis.h
pshpack1.h pshpack2.h pshpack4.h pshpack8.h poppack.h""".split()
SPECIAL_STUBS = {"pshpack1.h": "#pragma pack(push, 1)\n", "pshpack2.h": "#pragma pack(push, 2)\n", "pshpack4.h": "#pragma pack(push, 4)\n",
                 "pshpack8.h": "#pragma pack(push, 8)\n", "poppack.h": "#pragma pack(pop)\n",
                 "initguid.h": "#define INITGUID\n#include \"KPosixCompat.h\"\n"}
WIN_HEADER_RE = re.compile(r"^(win|wsock|ws2|mmsystem|mmreg|msacm|dd|di|ds|d3d|dx|atl|shl|shell|ole|com|ntdd|imm|crtdbg|conio|io\.h|direct\.h|"
                           r"process\.h|tchar|excpt|eh\.h|intrin|xmmintrin|emmintrin|mmintrin|pmmintrin|dbghelp|imagehlp|tlhelp32|psapi|wininet|"
                           r"winhttp|iphlp|nb30|lm|sddl|aclapi|userenv|uxtheme|dwmapi|vfw|richedit|commctrl|commdlg|objbase|unknwn|guiddef|"
                           r"basetsd|initguid|new\.h|fstream\.h|iostream\.h|strstrea\.h|minmax|wtypes|rpc|strsafe|gdiplus|share\.h|sys/timeb|sys/utime|pshpack|poppack)", re.I)

# ----------------------------------------------------------------------------------------------------------------
# Doc vcxproj (chep tu clang_survey.py, them: dieu kien '!=' va CompileAs tung tep)
# ----------------------------------------------------------------------------------------------------------------
def cond_match(cond, cfg, plat):
    if not cond: return True
    c = cond.replace(" ", "")
    key = ("'%s|%s'" % (cfg, plat)).replace(" ", "")
    if "!=" in c: return key not in c           # ExcludedFromBuild Condition="...!='ReleaseSDL|x64'" (KSdlApp.cpp)
    return key in c

def expand(s, projdir, cfg, plat):
    s = s.replace("$(SolutionDir)", SRC + "\\").replace("$(ProjectDir)", projdir + "\\").replace("$(Configuration)", cfg).replace("$(Platform)", plat)
    s = s.replace("$(JX_SDL3_DIR)", SDL3_DIR)
    s = re.sub(r"%\([A-Za-z]+\)", "", s)
    return s

def parse(vcx, cfg, plat):
    projdir = os.path.dirname(vcx)
    root = ET.parse(vcx).getroot()
    defs, incs, forced, undef = [], [], [], []
    for idg in root.findall("m:ItemDefinitionGroup", NS):
        if not cond_match(idg.get("Condition", ""), cfg, plat): continue
        cl = idg.find("m:ClCompile", NS)
        if cl is None: continue
        for tag, dst in (("PreprocessorDefinitions", defs), ("AdditionalIncludeDirectories", incs), ("ForcedIncludeFiles", forced), ("UndefinePreprocessorDefinitions", undef)):
            el = cl.find("m:" + tag, NS)
            if el is not None and el.text:
                dst.extend([x.strip() for x in expand(el.text, projdir, cfg, plat).split(";") if x.strip()])
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
            excl = False; compile_as = None
            for ex in cl.findall("m:ExcludedFromBuild", NS):
                if cond_match(ex.get("Condition", ""), cfg, plat) and (ex.text or "").strip().lower() == "true": excl = True
            for ca in cl.findall("m:CompileAs", NS):
                if cond_match(ca.get("Condition", ""), cfg, plat): compile_as = (ca.text or "").strip()
            if excl: continue
            files.append((os.path.normpath(os.path.join(projdir, f)), compile_as))
    return projdir, defs, incs, forced, charset, files, undef

# ----------------------------------------------------------------------------------------------------------------
# Stub header + KWin32.h / JxNetShim.h bong (chi vong >= 1)
# ----------------------------------------------------------------------------------------------------------------
def make_stubs():
    extra = []
    if os.path.isfile(STUB_EXTRA_JSON):
        extra = json.load(io.open(STUB_EXTRA_JSON, encoding="utf-8"))
    names = sorted(set(h.lower().replace("\\", "/") for h in WIN_HEADERS + extra))
    for n in names:
        p = os.path.join(STUB_DIR, n.replace("/", os.sep))
        os.makedirs(os.path.dirname(p), exist_ok=True)
        body = SPECIAL_STUBS.get(n, "#include \"KPosixCompat.h\"\n")
        with io.open(p, "w", encoding="ascii", newline="\n") as w:
            w.write("/* [ANDROID SURVEY] stub thay <%s> cua Windows SDK / CRT MSVC - CHI DE KHAO SAT.\n   Ban port that khong dung tep nay: cac #include Windows phai bo hoac do KWin32.h bao. */\n%s" % (n, body))
    return names

def make_shadow_kwin32():
    """KWin32.h bong = tep that, nhung nhanh '#else' (khong Windows: #define LONG long, HWND unsigned long, RECT/POINT/GUID rieng...)
    thay bang #include KPosixCompat.h + nhac lai anh xa SDL (timeGetTime/GetTickCount/Sleep) von chi nam trong nhanh Windows.
    Day chinh la 1 hunk ma ban port that phai sua trong KWin32.h (o day khong duoc sua tep goc)."""
    src = io.open(KWIN32_REAL, "r", encoding="latin-1", newline="").read().splitlines(True)
    start = next(i for i, l in enumerate(src) if re.match(r"\s*#\s*ifndef\s+__linux\b", l))
    depth = 0; else_i = None; end_i = None
    for i in range(start, len(src)):
        l = src[i].strip()
        if re.match(r"#\s*if", l): depth += 1
        elif re.match(r"#\s*endif", l):
            depth -= 1
            if depth == 0: end_i = i; break
        elif depth == 1 and re.match(r"#\s*else\b", l): else_i = i
    assert else_i and end_i, "khong tim thay khoi #ifndef __linux / #else / #endif trong KWin32.h"
    nl = "\r\n" if src[0].endswith("\r\n") else "\n"
    repl = ["// [ANDROID SURVEY] KWin32.h BONG (sinh tu dong tu tep that boi ndk_survey.py; tep that KHONG bi sua).",
            "// Nhanh khong-Windows cu (#define LONG long, HWND unsigned long, RECT/POINT/GUID rieng, MAX_PATH 1200...) thay bang:",
            "#include \"%s\"" % COMPAT_H.replace("\\", "/"),
            "#ifdef JX_PLATFORM_SDL",
            "\t#include <SDL3/SDL.h>",
            "\t#define timeGetTime()   ((DWORD)SDL_GetTicks())",
            "\t#define GetTickCount()  ((DWORD)SDL_GetTicks())",
            "\t#define Sleep(ms)       SDL_Delay((Uint32)(ms))",
            "#endif"]
    out = src[:else_i + 1] + [r + nl for r in repl] + src[end_i:]
    os.makedirs(SHADOW_DIR, exist_ok=True)
    with io.open(SHADOW_KWIN32, "w", encoding="latin-1", newline="") as w: w.write("".join(out))
    return else_i + 1, end_i + 1

def make_shadow_jxnetshim():
    """JxNetShim.h bong: khoi POSIX ('#else') cua no dinh nghia lai WSABUF / OVERLAPPED / WSADATA bang struct vo danh
    -> trung (khac kieu) voi KPosixCompat.h.  Bo 3 dong typedef do = 1 hunk cua ban port that."""
    src = io.open(JXNETSHIM_REAL, "r", encoding="latin-1", newline="").read().splitlines(True)
    out = []; dropped = 0
    for l in src:
        if re.match(r"\s*typedef\s+struct\s*\{[^}]*\}\s*(WSABUF|OVERLAPPED|WSADATA)\s*;", l):
            out.append("/* [ANDROID SURVEY] bo dong nay: KPosixCompat.h da dinh nghia */" + ("\r\n" if l.endswith("\r\n") else "\n")); dropped += 1
        else: out.append(l)
    os.makedirs(SHADOW_DIR, exist_ok=True)
    with io.open(SHADOW_JXNETSHIM, "w", encoding="latin-1", newline="") as w: w.write("".join(out))
    return dropped

# ----------------------------------------------------------------------------------------------------------------
# Chay clang
# ----------------------------------------------------------------------------------------------------------------
def build_cmd(p, defs, incs, is_c):
    cmd = [CLANG if is_c else CLANGXX, "--target=" + TARGET, "--sysroot=" + SYSROOT, "-fsyntax-only", "-x", "c" if is_c else "c++",
           "-std=gnu11" if is_c else "-std=gnu++17", "-fms-extensions", "-ferror-limit=%d" % A.errlimit, "-w",
           "-fno-caret-diagnostics", "-fno-color-diagnostics", "-fdiagnostics-absolute-paths",
           "-D_MBCS", "-DJX_PLATFORM_SDL", "-DNDEBUG", "-DUSE_STANDALONE_SPR"]
    if is_c: cmd.append("-Werror=implicit-function-declaration")     # C: ham Win32/CRT thieu chi la warning -> ep thanh loi de dem
    else: cmd.append("-D_LIBCPP_ENABLE_CXX17_REMOVED_FEATURES")       # auto_ptr/bind1st... (MSVC van giu; libc++ can co nay)
    for d in defs:
        # *_EXPORTS GIU LAI: ma nguon dung chung nhu co "dang o trong module" (CoreShell.h:1269 khai bao CoreDataChanged
        # duoi #ifdef CORE_EXPORTS; KWin32.h:148 include winsock2.h khi !ENGINE_EXPORTS), khong chi de chon __declspec
        # (__declspec da rong trong KPosixCompat.h).  Bo *_EXPORTS lam 415 loi gia 'CoreDataChanged' o vong 1.
        if d.split("=")[0] in DROP_DEFS: continue
        if d in ("JX_PLATFORM_SDL", "NDEBUG", "USE_STANDALONE_SPR"): continue
        cmd.append("-D" + d)
    seen = set()
    for i in incs + [os.path.join(SDL3_DIR, "include"), MINIAUDIO_INC] + ([os.path.join(SRC, p["vcpkg"])] if p["vcpkg"] else []):
        i = os.path.normpath(i)
        if i.lower() in seen or not os.path.isdir(i): continue
        seen.add(i.lower()); cmd.append("-I" + i)
    if A.iter >= 1:
        cmd += ["-I" + STUB_DIR, "-I" + PLATFORM_DIR, "-include", COMPAT_H]
        if p["kwin32"] and not is_c: cmd += ["-include", SHADOW_KWIN32]
        if p["netshim"] and not is_c: cmd += ["-include", SHADOW_JXNETSHIM]
    return cmd

def run_one(cmd, f, projdir):
    t0 = time.time()
    try:
        r = subprocess.run(cmd + [f], cwd=projdir, capture_output=True, text=True, errors="replace", timeout=900, creationflags=BELOW_NORMAL)
        out = r.stdout + r.stderr; code = r.returncode
    except subprocess.TimeoutExpired:
        out = "TIMEOUT"; code = -9
    return f, code, out, time.time() - t0

# ----------------------------------------------------------------------------------------------------------------
# Phan tich loi
# ----------------------------------------------------------------------------------------------------------------
ERR_RE = re.compile(r"^((?:[A-Za-z]:)?[^:\r\n]+?):(\d+):(\d+): (fatal error|error): (.*)$")
SYM_RES = [
    ("kieu", re.compile(r"^unknown type name '([^']+)'")),
    ("kieu", re.compile(r"^unknown class name '([^']+)'")),
    ("kieu", re.compile(r"^no type named '([^']+)' in the global namespace")),
    ("dinhdanh", re.compile(r"^use of undeclared identifier '([^']+)'")),
    ("dinhdanh", re.compile(r"^no member named '([^']+)' in the global namespace")),
    ("dinhdanh", re.compile(r"^call to undeclared function '([^']+)'")),
    ("dinhdanh", re.compile(r"^implicit declaration of function '([^']+)'")),
    ("std", re.compile(r"^no (?:member|type|template) named '([^']+)' in namespace '(?:std|stdext)'")),
    ("header", re.compile(r"^'([^']+)' file not found")),
    ("macro", re.compile(r"^function-like macro '([^']+)' is not defined")),
]
def normalize(msg):
    m = re.sub(r"'[^']*'", "'X'", msg)
    m = re.sub(r"\b\d+\b", "N", m)
    m = re.sub(r"; did you mean.*$", "", m)
    return m

CRT_SET = set("""itoa ltoa ultoa strupr strlwr strrev stricmp strnicmp strcmpi memicmp strset strnset
_finddata_t _findfirst _findnext _findclose _splitpath _makepath _fullpath _getch _kbhit _ftime _timeb _beginthread _beginthreadex _endthread
_endthreadex _swab _gcvt _ecvt _fcvt _chsize _utime _utimbuf _wfopen _setmode _isatty _getdrive _chdrive _getdcwd _heapmin _msize _expand
_CrtCheckMemory _CrtSetDbgFlag _CrtDumpMemoryLeaks _ASSERT _ASSERTE _RPT0 _RPT1 strcpy_s strcat_s sprintf_s _snprintf_s strncpy_s
_vsnprintf_s vsprintf_s sscanf_s fopen_s _strtime _strdate localtime_s gmtime_s _localtime64 _time64 __time64_t _stat64 _fstat64 _filelengthi64""".split())
DX_RE = re.compile(r"^(IDirect|LPDIRECT|DD[A-Z]|DI[A-Z0-9]|DS[A-Z]|D3D|LPDD|LPDI|LPDS|DirectDraw|DirectInput|DirectSound|Direct3D|DX|LPD3D|"
                   r"DIK_|DIMOFS_|DSBUFFERDESC|DSBCAPS|DSSCL|DSBPLAY|DSBSTATUS|DSBVOLUME|DDSURFACEDESC|DDSCAPS|DDBLTFX|DDCOLORKEY|DDPIXELFORMAT|"
                   r"IID_IDirect|CLSID_Direct|GUID_SysKeyboard|GUID_SysMouse|GUID_XAxis|GUID_YAxis|c_dfDI|DIJOYSTATE|DIMOUSESTATE|DIDATAFORMAT)")
WSOCK_RE = re.compile(r"^(WSA|SOCKET$|closesocket|ioctlsocket|FD_|SD_|SOCKADDR|LPHOSTENT|HOSTENT|CreateIoCompletionPort|GetQueuedCompletionStatus|"
                      r"PostQueuedCompletionStatus|AcceptEx|GetAcceptExSockaddrs|TransmitFile|LPFN_|WSAID_|LPWSAOVERLAPPED|LPWSABUF|INVALID_SOCKET|SOCKET_ERROR)")
COM_RE = re.compile(r"^(IUnknown|REFIID|REFGUID|IID_IUnknown|CoInitialize|CoUninitialize|CoCreateInstance|CoTaskMem|IID_|CLSID_|HRESULT_FROM_WIN32|"
                    r"IStream|IMalloc|VARIANT|BSTR|SysAllocString|SysFreeString|OLECHAR|LPOLESTR|SafeArray|IDispatch|IClassFactory)")
IME_RE = re.compile(r"^(Imm[A-Z]|HIMC|IMC_|IME_|GCS_|CFS_|COMPOSITIONFORM|CANDIDATEFORM|CANDIDATELIST|NI_|ISC_|IMN_|IGP_|IMR_)")
WND_RE = re.compile(r"^(WM_|VK_|MB_|MK_|SW_|WS_|SM_|PM_|CS_|IDC_|IDI_|IDOK|IDCANCEL|IDYES|IDNO|HWND_|SWP_|GWL_|GCL_|GWLP_|WNDCLASS|WNDPROC|MSG$|LPMSG|"
                    r"PAINTSTRUCT|CreateWindow|DestroyWindow|DefWindowProc|RegisterClass|UnregisterClass|PeekMessage|GetMessage|DispatchMessage|TranslateMessage|"
                    r"SendMessage|PostMessage|PostQuitMessage|SetWindow|GetWindow|ShowWindow|UpdateWindow|MoveWindow|FindWindow|SetCursor|ShowCursor|GetCursor|SetCapture|"
                    r"ReleaseCapture|GetClientRect|GetWindowRect|ClientToScreen|ScreenToClient|GetSystemMetrics|AdjustWindowRect|SetForegroundWindow|GetForegroundWindow|"
                    r"GetActiveWindow|SetActiveWindow|IsWindow|IsIconic|GetFocus|SetFocus|MessageBox|LoadCursor|LoadIcon|LoadImage|GetDC|ReleaseDC|BeginPaint|EndPaint|"
                    r"InvalidateRect|ValidateRect|GetKeyState|GetAsyncKeyState|GetKeyboardState|MapVirtualKey|keybd_event|mouse_event|SetTimer|KillTimer|"
                    r"GetDlgItem|DialogBox|EndDialog|SendDlgItemMessage|SetDlgItemText|GetDlgItemText|EnableWindow|GetParent|SetParent|GetDesktopWindow|WinMain|"
                    r"HINSTANCE|HWND|HMENU|HICON|HCURSOR|HDC|HBITMAP|HFONT|HBRUSH|HPEN|HRGN|HPALETTE|HGDIOBJ|COLORREF|RGB$|LOGFONT|TEXTMETRIC|CreateFont|SelectObject|"
                    r"DeleteObject|GetStockObject|CreateCompatibleDC|CreateDIBSection|BitBlt|StretchBlt|SetBkMode|SetTextColor|TextOut|DrawText|GetTextExtent|CreateSolidBrush|"
                    r"FillRect|SetPixel|GetPixel|CreatePalette|SetPaletteEntries|GetDeviceCaps|OpenClipboard|CloseClipboard|GetClipboardData|SetClipboardData|EmptyClipboard|"
                    r"GlobalAlloc|GlobalLock|GlobalUnlock|GlobalFree|GlobalSize|ShellExecute|SystemParametersInfo|GetSystemMenu|SetWindowLong|GetWindowLong|SetWindowPos|"
                    r"SetLayeredWindowAttributes|GetMonitorInfo|MonitorFromWindow|EnumDisplaySettings|ChangeDisplaySettings|DEVMODE|SetWindowText|GetWindowText|SetClassLong|"
                    r"SetCaretPos|CreateCaret|DestroyCaret|ShowCaret|HideCaret|GetCaretPos|HOTKEYF_|Shell_NotifyIcon|NOTIFYICONDATA|NIM_|NIF_)")
CRT_RE = re.compile(r"^(_[a-z]|__[a-z]|str[a-z]+$|mem[a-z]+$|itoa|ltoa|ultoa)")
CAMEL_RE = re.compile(r"^[A-Z][A-Za-z0-9]*$")
CAPS_RE = re.compile(r"^[A-Z][A-Z0-9_]*$")
PROJ_RE = re.compile(r"^(K[A-Z]|g_|m_|c_|s_|SDL_|lua_|Lua|T[A-Z][A-Z_]*_|I[A-Z][a-z]|Jx|JX|CRep|MP|SPR|Rep|CoreDataChanged|HKLVIDEO|KL[A-Z])")

def classify(sym, kind):
    if DX_RE.match(sym): return "DirectX (DDraw/DInput/DSound/D3D)"
    if IME_RE.match(sym): return "IME (imm32)"
    if WSOCK_RE.match(sym): return "Winsock / IOCP"
    if COM_RE.match(sym): return "COM / OLE"
    if kind == "std": return "STL MSVC (stdext/hash_map...)"
    if kind == "header": return "header thieu"
    if kind == "msvc": return "MSVC-only (__asm/SEH)"
    if sym in CRT_SET or CRT_RE.match(sym): return "CRT MSVC"
    if PROJ_RE.match(sym): return "Du an / thu vien (khong phai Win32)"
    if WND_RE.match(sym): return "Cua so / GDI / thong diep (user32/gdi32)"
    if sym in ("min", "max"): return "min/max (macro bi libc++ go)"
    if CAMEL_RE.match(sym) and re.search(r"[a-z]", sym): return "Win32 API (kernel32/advapi32/...)" if kind != "kieu" else "Kieu Win32 khac"
    if CAPS_RE.match(sym): return "Hang so / kieu Win32 (ALLCAPS)"
    return "Khac"

def analyze_output(out):
    """-> list (file, line, col, fatal, msg)  (file: duong dan nhu clang in, chua normcase)"""
    res = []
    for l in out.splitlines():
        m = ERR_RE.match(l.strip())
        if m: res.append((m.group(1), int(m.group(2)), int(m.group(3)), m.group(4) == "fatal error", m.group(5)))
    return res

def sym_of(msg):
    for kind, rx in SYM_RES:
        m = rx.match(msg)
        if m: return kind, m.group(1)
    if "MS-style inline assembly" in msg: return "msvc", "__asm (MS-style inline asm)"
    m = re.match(r"^SEH '([^']+)' is not supported", msg)
    if m: return "msvc", "SEH " + m.group(1)
    return None, None

def rel_src(f):
    """duong dan tuong doi Sources/ (giu chu hoa nhu clang in), hoac nguyen ven neu ngoai Sources"""
    fn = f.replace("/", "\\")
    if os.path.normcase(fn).startswith(SRC_N + "\\"): return fn[len(SRC) + 1:]
    return f

# ----------------------------------------------------------------------------------------------------------------
# Quet tinh: MSVC-ism trong nguon, include sai chu hoa / dau '\\'
# ----------------------------------------------------------------------------------------------------------------
MSVC_PATTERNS = [
    ("__asm / _asm (x86 inline asm)", r"\b_{1,2}asm\b"),
    ("__try / __except / __finally / __leave (SEH)", r"\b__(try|except|finally|leave)\b"),
    ("_alloca", r"\b_alloca\b"),
    ("__super", r"\b__super\b"),
    ("__declspec(dllexport/dllimport)", r"__declspec\s*\(\s*dll(ex|im)port"),
    ("__declspec(naked/align/noinline/thread/novtable/selectany/property/uuid/nothrow)", r"__declspec\s*\(\s*(naked|align|noinline|thread|novtable|selectany|property|uuid|nothrow|noreturn|deprecated|allocate|restrict)"),
    ("__int8/16/32/64", r"\b__int(8|16|32|64)\b"),
    ("__forceinline", r"\b__forceinline\b"),
    ("__uuidof", r"\b__uuidof\b"),
    ("__stdcall/__cdecl/__fastcall/WINAPI/CALLBACK/APIENTRY/PASCAL", r"\b(__stdcall|__cdecl|__fastcall|__thiscall|WINAPI|CALLBACK|APIENTRY|PASCAL)\b"),
    ("__FUNCSIG__", r"\b__FUNCSIG__\b"),
    ("__assume / __debugbreak / __noop", r"\b__(assume|debugbreak|noop)\b"),
    ("__based / __w64 / __unaligned / __ptr32 / __ptr64", r"\b__(based|w64|unaligned|ptr32|ptr64)\b"),
    ("__if_exists / __interface / __event / __identifier", r"\b__(if_exists|if_not_exists|interface|event|raise|hook|identifier)\b"),
    ("#if _MSC_VER", r"\b_MSC_VER\b"),
    ("#pragma comment(lib/linker)", r"#\s*pragma\s+comment"),
    ("#pragma pack", r"#\s*pragma\s+pack"),
    ("#pragma warning", r"#\s*pragma\s+warning"),
    ("#pragma once", r"#\s*pragma\s+once"),
    ("#pragma optimize/intrinsic/function/inline_depth/auto_inline/check_stack", r"#\s*pragma\s+(optimize|intrinsic|function|inline_depth|inline_recursion|auto_inline|check_stack)"),
    ("#pragma data_seg/code_seg/section/init_seg/bss_seg/const_seg", r"#\s*pragma\s+(data_seg|code_seg|section|init_seg|bss_seg|const_seg)"),
    ("#pragma message/region/endregion/hdrstop", r"#\s*pragma\s+(message|region|endregion|hdrstop)"),
    ("#pragma khac cua MSVC", r"#\s*pragma\s+(deprecated|pointers_to_members|vtordisp|managed|unmanaged|runtime_checks|setlocale|strict_gs_check|float_control|fp_contract|fenv_access|loop|detect_mismatch|conform|component|execution_character_set)"),
    ("#ifdef/#ifndef _WIN64 (chon C hay __asm; x64 da co nhanh C)", r"#\s*(ifdef|ifndef|if\s+!?\s*defined\s*\(?)\s*_WIN64\b"),
    ("_M_IX86 / _M_X64 / _M_AMD64 (kiem tra CPU kieu MSVC)", r"\b_M_(IX86|X64|AMD64|ARM64)\b"),
    ("SSE intrinsics (_mm_* / __m128)", r"\b_mm_[a-z0-9_]+\b|\b__m128i?\b|\b__m64\b"),
    ("register (bo o C++17)", r"\bregister\s+(int|unsigned|char|long|short|float|double|const|signed|BYTE|WORD|DWORD|LONG|UINT|INT)\b"),
    ("auto_ptr / hash_map / hash_set / stdext::", r"\b(auto_ptr|hash_map|hash_set|stdext::)"),
    ("#include <windows.h> truc tiep", r"#\s*include\s*[<\"]windows\.h[>\"]"),
    ("#include <winsock2.h> / winsock.h truc tiep", r"#\s*include\s*[<\"]winsock2?\.h[>\"]"),
    ("#include DirectX (ddraw/dinput/dsound/d3d*)", r"#\s*include\s*[<\"](ddraw|dinput|dsound|d3d[0-9a-z]*)\.h[>\"]"),
]
MSVC_RE = [(n, re.compile(rx, re.I if n.startswith("#include") else 0)) for n, rx in MSVC_PATTERNS]
INC_RE = re.compile(r'^\s*#\s*include\s*([<"])([^>"]+)[>"]', re.M)
_listdir_cache = {}
def listdir_cached(d):
    if d not in _listdir_cache:
        try: _listdir_cache[d] = set(os.listdir(d))
        except OSError: _listdir_cache[d] = set()
    return _listdir_cache[d]
def exact_case(path):
    path = os.path.abspath(path)
    drive, rest = os.path.splitdrive(path)
    cur = drive + os.sep
    for comp in [c for c in rest.split(os.sep) if c]:
        if comp not in listdir_cached(cur): return False
        cur = os.path.join(cur, comp)
    return True
_realcase_cache = {}
def real_case_path(path):
    """duong dan voi dung chu hoa/thuong nhu tren dia (vcxproj ghi '../engine/src' nhung dia la 'Engine\\Src');
    None neu khong ton tai.  Dung de chi cham diem phan #include, khong cham diem thu muc include cua vcxproj."""
    path = os.path.abspath(path)
    if path in _realcase_cache: return _realcase_cache[path]
    drive, rest = os.path.splitdrive(path)
    cur = drive + os.sep; ok = True
    for comp in [c for c in rest.split(os.sep) if c]:
        ents = listdir_cached(cur)
        if comp in ents: cur = os.path.join(cur, comp); continue
        hit = next((e for e in ents if e.lower() == comp.lower()), None)
        if hit is None: ok = False; break
        cur = os.path.join(cur, hit)
    _realcase_cache[path] = cur if ok else None
    return _realcase_cache[path]

def strip_comments(txt):
    txt = re.sub(r"/\*.*?\*/", " ", txt, flags=re.S)
    txt = re.sub(r"//[^\n]*", " ", txt)
    return txt

def static_scan(key, p, files, incs):
    dirs = [os.path.join(SRC, d) for d in p["scan"]]
    srcfiles = []
    for d in dirs:
        for dp, dn, fn in os.walk(d):
            if "vcpkg_installed" in dp or os.sep + "Release" in dp or os.sep + "x64" in dp or os.sep + "Debug" in dp: continue
            for f in fn:
                if f.lower().endswith((".cpp", ".c", ".h", ".inc", ".hpp")): srcfiles.append(os.path.join(dp, f))
    if p["only"]:   # common/rainbow: chi cac tep duoc khao sat + header .h cung thu muc
        keep = set(os.path.normcase(f) for f, _ in files)
        srcfiles = [f for f in srcfiles if os.path.normcase(f) in keep or f.lower().endswith(".h")]
    cons = collections.OrderedDict((n, [0, set()]) for n, _ in MSVC_RE)
    case_bad = []; backslash = []; n_inc = 0
    incs_real = [real_case_path(i) or i for i in incs]
    for f in srcfiles:
        raw = io.open(f, "r", encoding="latin-1").read()
        txt = strip_comments(raw)
        for n, rx in MSVC_RE:
            k = len(rx.findall(txt))
            if k: cons[n][0] += k; cons[n][1].add(os.path.relpath(f, SRC))
        fdir_real = real_case_path(os.path.dirname(f)) or os.path.dirname(f)
        for m in INC_RE.finditer(txt):
            inc = m.group(2).strip(); n_inc += 1
            if "\\" in inc: backslash.append((os.path.relpath(f, SRC), inc))
            cand = [os.path.join(fdir_real, inc.replace("\\", os.sep))] + [os.path.join(i, inc.replace("\\", os.sep)) for i in incs_real]
            for c in cand:
                if os.path.isfile(c):
                    if not exact_case(c): case_bad.append((os.path.relpath(f, SRC), inc))
                    break
    return dict(files_scanned=len(srcfiles), includes=n_inc,
                constructs=collections.OrderedDict((n, dict(count=v[0], files=len(v[1]), examples=sorted(v[1])[:6])) for n, v in cons.items() if v[0]),
                include_case_mismatch=case_bad, include_backslash=backslash)

# ----------------------------------------------------------------------------------------------------------------
# Khao sat mot du an
# ----------------------------------------------------------------------------------------------------------------
def survey(key):
    p = PROJ[key]
    vcx = os.path.join(SRC, p["vcx"].replace("/", os.sep))
    projdir, defs, incs, forced, charset, files, undef = parse(vcx, p["cfg"], p["plat"])
    if p["only"]:
        want = set(x.lower() for x in p["only"])
        files = [(f, ca) for f, ca in files if os.path.basename(f).lower() in want]
        missing = want - set(os.path.basename(f).lower() for f, _ in files)
        if missing: print("  [%s] CANH BAO: tep yeu cau khong co trong vcxproj/cau hinh: %s" % (key, sorted(missing)))
    incs_abs = [os.path.normpath(os.path.join(projdir, i)) if not os.path.isabs(i) else i for i in incs]
    rawpath = os.path.join(OUT, key + "_raw.log")
    results = {}
    if not A.no_run:
        cmd_cpp = build_cmd(p, defs, incs_abs, False); cmd_c = build_cmd(p, defs, incs_abs, True)
        print("[%s] %s | %s|%s | %d TU | vong %d" % (key, os.path.basename(vcx), p["cfg"], p["plat"], len(files), A.iter)); sys.stdout.flush()
        raw = io.open(rawpath, "w", encoding="utf-8", newline="\n")
        raw.write("CMD C++: " + " ".join(cmd_cpp) + "\nCMD C  : " + " ".join(cmd_c) + "\n\n")
        t0 = time.time(); done = 0
        def job(fc):
            f, ca = fc
            is_c = f.lower().endswith(".c") and ca != "CompileAsCpp"
            return run_one(cmd_c if is_c else cmd_cpp, f, projdir)
        with ThreadPoolExecutor(max_workers=A.jobs) as ex:
            for f, code, out, dt in ex.map(job, files):
                done += 1
                rel = os.path.relpath(f, SRC)
                errs = analyze_output(out)
                results[rel] = dict(code=code, errors=len(errs), time=round(dt, 1), out=out)
                raw.write("=== %s exit=%d errors=%d %.1fs\n%s\n" % (rel, code, len(errs), dt, out))
                if done % 25 == 0: print("  ...%d/%d (%.0fs)" % (done, len(files), time.time() - t0)); sys.stdout.flush()
        raw.close()
        print("[%s] chay xong %d TU, %.0fs" % (key, len(files), time.time() - t0)); sys.stdout.flush()
    else:
        txt = io.open(rawpath, "r", encoding="utf-8").read()
        for m in re.finditer(r"^=== (.+?) exit=(-?\d+) errors=(\d+) ([\d.]+)s\n(.*?)(?=^=== |\Z)", txt, re.M | re.S):
            results[m.group(1)] = dict(code=int(m.group(2)), errors=int(m.group(3)), time=float(m.group(4)), out=m.group(5))
    # --- phan tich: moi loi duoc gan (a) tep chua loi = chinh TU hay header, (b) co nam trong tep Windows-only khong
    cats = collections.Counter(); cat_files = collections.defaultdict(set); cat_ex = {}
    syms = {}          # sym -> dict(kind, cls, tus:set, sites:set, hsites:set, wsites:set)
    headers = collections.defaultdict(lambda: dict(tus=set(), sites=set()))
    errfiles = collections.defaultdict(lambda: dict(n=0, sites=set(), tus=set(), disp=None, winonly=False))
    per_tu = {}
    n_err_hdr = 0; n_err_winonly = 0
    for rel, r in results.items():
        errs = analyze_output(r["out"])
        tu_norm = os.path.normcase(os.path.join(SRC, rel))
        per_tu[rel] = dict(code=r["code"], errors=len(errs), own=0, hdr=0, winonly=0, time=r["time"], fatal=next((e[4] for e in errs if e[3]), None), top=collections.Counter())
        for (f, ln, col, fatal, msg) in errs:
            frel = rel_src(f); fkey = os.path.normcase(frel).replace("\\", "/")
            own = (os.path.normcase(f) == tu_norm)
            wo = bool(WINONLY_RE.search(fkey))
            if own: per_tu[rel]["own"] += 1
            else: per_tu[rel]["hdr"] += 1; n_err_hdr += 1
            if wo: per_tu[rel]["winonly"] += 1; n_err_winonly += 1
            ef = errfiles[fkey]; ef["n"] += 1; ef["sites"].add((ln, col)); ef["tus"].add(rel); ef["disp"] = ef["disp"] or frel; ef["winonly"] = wo
            c = normalize(msg)
            cats[c] += 1; cat_files[c].add(rel); cat_ex.setdefault(c, "%s:%d: %s" % (frel, ln, msg[:160]))
            kind, s = sym_of(msg)
            if not s: continue
            site = "%s:%d:%d" % (fkey, ln, col)
            if kind == "header":
                headers[s]["tus"].add(rel); headers[s]["sites"].add(site); continue
            d = syms.setdefault(s, dict(kind=kind, cls=classify(s, kind), tus=set(), sites=set(), hsites=set(), wsites=set()))
            if kind == "kieu" and d["kind"] != "kieu": d["kind"] = "kieu"
            d["tus"].add(rel); d["sites"].add(site); per_tu[rel]["top"][s] += 1
            if not own: d["hsites"].add(site)
            if wo: d["wsites"].add(site)
    n_tu = len(per_tu); n_pass = sum(1 for v in per_tu.values() if v["code"] == 0 and v["errors"] == 0)
    n_pass_own = sum(1 for v in per_tu.values() if v["own"] == 0 and not v["fatal"] and v["code"] != -9)
    n_err = sum(v["errors"] for v in per_tu.values())
    n_cap = sum(1 for v in per_tu.values() if v["errors"] >= A.errlimit)
    n_timeout = sum(1 for v in per_tu.values() if v["code"] == -9)
    n_tu_winonly = sum(1 for rel in per_tu if WINONLY_RE.search(os.path.normcase(rel).replace("\\", "/")))
    sym_rows = sorted(((s, d["kind"], d["cls"], len(d["tus"]), len(d["sites"]), len(d["hsites"]), len(d["wsites"])) for s, d in syms.items()), key=lambda x: (-x[3], -x[4], x[0]))
    hdr_rows = sorted(((h, len(d["tus"]), len(d["sites"])) for h, d in headers.items()), key=lambda x: (-x[1], x[0]))
    errfile_rows = sorted(((v["disp"], v["n"], len(v["sites"]), len(v["tus"]), v["winonly"]) for v in errfiles.values()), key=lambda x: -x[1])[:40]
    cls_tot = collections.Counter(); cls_syms = collections.Counter(); cls_tus = collections.defaultdict(set)
    for s, d in syms.items():
        cls_tot[d["cls"]] += len(d["sites"]); cls_syms[d["cls"]] += 1; cls_tus[d["cls"]] |= d["tus"]
    prefix = collections.defaultdict(lambda: [0, 0, set()])
    for s, d in syms.items():
        m = re.match(r"^([A-Z][A-Z0-9]*)_[A-Z0-9_]+$", s)
        if m:
            g = m.group(1) + "_*"; prefix[g][0] += 1; prefix[g][1] += len(d["sites"]); prefix[g][2] |= d["tus"]
    # static scan (khong phu thuoc vong)
    stat = static_scan(key, p, files, incs_abs)
    with io.open(os.path.join(OUT, key + "_tomtat.txt"), "w", encoding="utf-8", newline="\n") as w:
        w.write("%s %s|%s vong %d: %d TU, %d TU sach, %d TU sach neu bo loi tu header, %d loi (%d tu header, %d trong tep Windows-only; %d TU cham tran %d, %d timeout)\n\n"
                % (key, p["cfg"], p["plat"], A.iter, n_tu, n_pass, n_pass_own, n_err, n_err_hdr, n_err_winonly, n_cap, A.errlimit, n_timeout))
        w.write("== LOAI LOI (theo so lan) ==\n")
        for c, n in cats.most_common(80): w.write("%6d  x%-3d TU  %s\n        vd: %s\n" % (n, len(cat_files[c]), c, cat_ex[c]))
        w.write("\n== KY HIEU THIEU (TU, diem goi, diem trong header, diem trong tep Windows-only) ==\n")
        for s, kind, cls, ntu, nsite, nh, nw in sym_rows[:300]: w.write("%4d TU %5d diem %4d hdr %4d winonly  %-10s %-45s %s\n" % (ntu, nsite, nh, nw, kind, cls, s))
        w.write("\n== TEP SINH LOI (loi, diem, TU) ==\n")
        for disp, n, ns, nt, wo in errfile_rows: w.write("%6d %5d %4d %s %s\n" % (n, ns, nt, "[WINONLY]" if wo else "         ", disp))
        w.write("\n== HEADER THIEU ==\n")
        for h, ntu, nsite in hdr_rows: w.write("%4d TU %5d diem  %s\n" % (ntu, nsite, h))
        w.write("\n== TEP (loi tong / rieng trong tep) ==\n")
        for rel, v in sorted(per_tu.items(), key=lambda kv: (-kv[1]["own"], -kv[1]["errors"])):
            w.write("%5d %5d  %-60s %s\n" % (v["errors"], v["own"], rel, ("FATAL: " + v["fatal"][:100]) if v["fatal"] else ", ".join("%s x%d" % kv for kv in v["top"].most_common(4))))
    return dict(key=key, cfg=p["cfg"], plat=p["plat"], iter=A.iter, tus=n_tu, passed=n_pass, passed_own=n_pass_own, errors=n_err, err_hdr=n_err_hdr,
                err_winonly=n_err_winonly, tus_winonly=n_tu_winonly, capped=n_cap, timeout=n_timeout,
                cats=[(c, n, len(cat_files[c]), cat_ex[c]) for c, n in cats.most_common(60)],
                symbols=sym_rows[:400], headers=hdr_rows, errfiles=errfile_rows,
                classes=sorted(((c, cls_syms[c], cls_tot[c], len(cls_tus[c])) for c in cls_tot), key=lambda x: -x[2]),
                prefix=sorted(((g, v[0], v[1], len(v[2])) for g, v in prefix.items()), key=lambda x: -x[2])[:40],
                per_tu={rel: dict(code=v["code"], errors=v["errors"], own=v["own"], hdr=v["hdr"], winonly=v["winonly"], time=v["time"], fatal=v["fatal"], top=v["top"].most_common(6)) for rel, v in per_tu.items()},
                static=stat)

# ----------------------------------------------------------------------------------------------------------------
# Bang markdown
# ----------------------------------------------------------------------------------------------------------------
def md_report(res, cmd_note):
    L = []
    L.append("# Khảo sát NDK clang++ (aarch64-linux-android24, -fsyntax-only) — vòng %d — %s\n" % (A.iter, time.strftime("%d/%m %H:%M")))
    L.append(cmd_note + "\n")
    L.append("| dự án | cấu hình | TU | TU sạch | TU sạch nếu bỏ lỗi từ header | lỗi | trong đó từ header | trong tệp Windows-only | TU Windows-only | TU chạm trần %d | timeout |\n|---|---|---|---|---|---|---|---|---|---|---|" % A.errlimit)
    for r in res: L.append("| %s | %s\\|%s | %d | %d | %d | %d | %d | %d | %d | %d | %d |" % (r["key"], r["cfg"], r["plat"], r["tus"], r["passed"], r["passed_own"], r["errors"], r["err_hdr"], r["err_winonly"], r["tus_winonly"], r["capped"], r["timeout"]))
    L.append("| **tổng** | | %d | %d | %d | %d | %d | %d | %d | %d | %d |" % tuple(sum(r[k] for r in res) for k in ("tus", "passed", "passed_own", "errors", "err_hdr", "err_winonly", "tus_winonly", "capped", "timeout")))
    L.append("")
    # gop ky hieu toan bo
    allsym = {}
    for r in res:
        for s, kind, cls, ntu, nsite, nh, nw in r["symbols"]:
            d = allsym.setdefault(s, dict(kind=kind, cls=cls, tu=0, site=0, hsite=0, wsite=0, proj=set()))
            d["tu"] += ntu; d["site"] += nsite; d["hsite"] += nh; d["wsite"] += nw; d["proj"].add(r["key"])
    rows = sorted(allsym.items(), key=lambda kv: (-kv[1]["site"], -kv[1]["tu"]))
    L.append("## Ký hiệu thiếu (gộp mọi dự án) — 60 đầu theo số điểm gọi\n")
    L.append("| # | ký hiệu | loại | nhóm | TU | điểm gọi | trong header | trong tệp Windows-only | dự án |\n|---|---|---|---|---|---|---|---|---|")
    for i, (s, d) in enumerate(rows[:60], 1):
        L.append("| %d | `%s` | %s | %s | %d | %d | %d | %d | %s |" % (i, s, d["kind"], d["cls"], d["tu"], d["site"], d["hsite"], d["wsite"], ",".join(sorted(d["proj"]))))
    L.append("")
    cls = collections.defaultdict(lambda: [0, 0, 0])
    for s, d in allsym.items():
        cls[d["cls"]][0] += 1; cls[d["cls"]][1] += d["site"]; cls[d["cls"]][2] += d["wsite"]
    L.append("## Nhóm ký hiệu thiếu\n\n| nhóm | số ký hiệu | điểm gọi | trong tệp Windows-only |\n|---|---|---|---|")
    for c, v in sorted(cls.items(), key=lambda kv: -kv[1][1]): L.append("| %s | %d | %d | %d |" % (c, v[0], v[1], v[2]))
    L.append("")
    pre = collections.defaultdict(lambda: [0, 0])
    for r in res:
        for g, nsym, nsite, ntu in r["prefix"]: pre[g][0] += nsym; pre[g][1] += nsite
    L.append("## Nhóm tiền tố hằng số (ALLCAPS_)\n\n| tiền tố | số ký hiệu | điểm gọi |\n|---|---|---|")
    for g, v in sorted(pre.items(), key=lambda kv: -kv[1][1])[:30]: L.append("| `%s` | %d | %d |" % (g, v[0], v[1]))
    L.append("")
    ef = collections.defaultdict(lambda: [0, 0, 0, False, None])
    for r in res:
        for disp, n, ns, nt, wo in r["errfiles"]:
            k = disp.lower().replace("\\", "/"); ef[k][0] += n; ef[k][1] = max(ef[k][1], ns); ef[k][2] += nt; ef[k][3] = wo; ef[k][4] = ef[k][4] or disp
    L.append("## Tệp sinh lỗi nhiều nhất (header \"độc\": 1 điểm lỗi trong header nhân lên mọi TU include nó)\n\n| tệp | lỗi | điểm lỗi (distinct) | TU bị ảnh hưởng | Windows-only |\n|---|---|---|---|---|")
    for k, v in sorted(ef.items(), key=lambda kv: -kv[1][0])[:30]: L.append("| `%s` | %d | %d | %d | %s |" % (v[4], v[0], v[1], v[2], "có" if v[3] else ""))
    L.append("")
    hd = collections.defaultdict(lambda: [0, 0, set()])
    for r in res:
        for h, ntu, nsite in r["headers"]: hd[h][0] += ntu; hd[h][1] += nsite; hd[h][2].add(r["key"])
    L.append("## Header không tìm thấy\n\n| header | TU | điểm | dự án |\n|---|---|---|---|")
    for h, v in sorted(hd.items(), key=lambda kv: -kv[1][0]): L.append("| `%s` | %d | %d | %s |" % (h, v[0], v[1], ",".join(sorted(v[2]))))
    L.append("")
    for r in res:
        L.append("## %s — 30 loại thông báo lỗi đầu\n\n| số lần | TU | loại | ví dụ |\n|---|---|---|---|" % r["key"])
        for c, n, nf, ex in r["cats"][:30]: L.append("| %d | %d | `%s` | `%s` |" % (n, nf, c.replace("|", "\\|"), ex.replace("|", "\\|")))
        L.append("")
        L.append("### %s — 30 TU nhiều lỗi RIÊNG (trong chính tệp .cpp) nhất\n\n| lỗi riêng | lỗi tổng | TU | fatal / ký hiệu chính |\n|---|---|---|---|" % r["key"])
        for rel, v in sorted(r["per_tu"].items(), key=lambda kv: (-kv[1]["own"], -kv[1]["errors"]))[:30]:
            L.append("| %d | %d | `%s` | %s |" % (v["own"], v["errors"], rel, ("FATAL " + v["fatal"][:80]) if v["fatal"] else ", ".join("`%s`×%d" % kv for kv in v["top"][:4])))
        L.append("")
        st = r["static"]
        L.append("### %s — MSVC-ism trong nguồn (quét tĩnh %d tệp, đã bỏ chú thích)\n\n| cấu trúc | số lần | tệp | ví dụ |\n|---|---|---|---|" % (r["key"], st["files_scanned"]))
        for n, v in st["constructs"].items(): L.append("| %s | %d | %d | %s |" % (n, v["count"], v["files"], ", ".join("`%s`" % e for e in v["examples"][:3])))
        L.append("\n- `#include` sai chữ hoa/thường so với tên tệp thật: **%d** (vd %s)" % (len(st["include_case_mismatch"]), "; ".join("`%s` → `%s`" % x for x in st["include_case_mismatch"][:5])))
        L.append("- `#include` dùng dấu `\\`: **%d** (vd %s)\n" % (len(st["include_backslash"]), "; ".join("`%s` → `%s`" % x for x in st["include_backslash"][:5])))
    return "\n".join(L)

def compare():
    its = sorted(glob.glob(os.path.join(A.scratch, "it*", "tonghop.json")))
    data = []
    for f in its:
        j = json.load(io.open(f, encoding="utf-8")); data.append((os.path.basename(os.path.dirname(f)), j))
    L = ["# So sánh các vòng khảo sát NDK\n", "| vòng | dự án | TU | TU sạch | TU sạch nếu bỏ lỗi header | lỗi | từ header | trong tệp Windows-only | TU chạm trần |", "|---|---|---|---|---|---|---|---|---|"]
    for name, j in data:
        for r in j["projects"]: L.append("| %s | %s | %d | %d | %s | %d | %s | %s | %d |" % (name, r["key"], r["tus"], r["passed"], r.get("passed_own", "-"), r["errors"], r.get("err_hdr", "-"), r.get("err_winonly", "-"), r["capped"]))
        L.append("| %s | **tổng** | %d | %d | %s | %d | %s | %s | %d |" % (name, sum(r["tus"] for r in j["projects"]), sum(r["passed"] for r in j["projects"]),
                 sum(r.get("passed_own", 0) for r in j["projects"]), sum(r["errors"] for r in j["projects"]), sum(r.get("err_hdr", 0) for r in j["projects"]),
                 sum(r.get("err_winonly", 0) for r in j["projects"]), sum(r["capped"] for r in j["projects"])))
    io.open(os.path.join(A.scratch, "SOSANH.md"), "w", encoding="utf-8", newline="\n").write("\n".join(L) + "\n")
    print("SOSANH:", os.path.join(A.scratch, "SOSANH.md"))

if __name__ == "__main__":
    if A.compare: compare(); sys.exit(0)
    os.makedirs(OUT, exist_ok=True)
    note = "Trình dịch: `%s` (NDK %s). Cờ chung: `--target=%s --sysroot=<NDK sysroot> -fsyntax-only -std=gnu++17 -D_LIBCPP_ENABLE_CXX17_REMOVED_FEATURES (.c: clang -std=gnu11 -Werror=implicit-function-declaration) -fms-extensions -ferror-limit=%d -w -D_MBCS -DJX_PLATFORM_SDL -DNDEBUG -DUSE_STANDALONE_SPR` + define/include của vcxproj (bỏ WIN32/_WINDOWS/_USRDLL/_LIB/WINVER/_WIN32_WINNT; GIỮ *_EXPORTS vì mã dùng chúng như cờ \"đang ở trong module\", `__declspec` đã rỗng) + `-I ThirdParty/SDL3/include -I ThirdParty/miniaudio` + include vcpkg." % (CLANGXX, os.path.basename(NDK), TARGET, A.errlimit)
    if A.iter >= 1:
        if not os.path.isfile(COMPAT_H): sys.exit("thieu " + COMPAT_H)
        stubs = make_stubs(); a, b = make_shadow_kwin32(); dropped = make_shadow_jxnetshim()
        note += " Vòng ≥1: `-I <stub_include> -I Sources/Engine/Src/Platform -include KPosixCompat.h` (+ `-include <shadow>/KWin32.h` cho engine/core/s3client; shadow = KWin32.h thật với dòng %d–%d (nhánh `#else` không-Windows) thay bằng `#include KPosixCompat.h` + ánh xạ SDL; + `-include <shadow>/JxNetShim.h` cho common/rainbow = JxNetShim.h thật bỏ %d typedef trùng WSABUF/OVERLAPPED/WSADATA). %d stub header Windows." % (a, b, dropped, len(stubs))
    res = []
    for k in [x.strip() for x in A.proj.split(",") if x.strip()]:
        if k in PROJ: res.append(survey(k))
    json.dump(dict(iter=A.iter, note=note, projects=res), io.open(os.path.join(OUT, "tonghop.json"), "w", encoding="utf-8"), ensure_ascii=False, indent=1, default=list)
    md = md_report(res, note)
    io.open(os.path.join(OUT, "TONGHOP_it%d.md" % A.iter), "w", encoding="utf-8", newline="\n").write(md)
    # goi y stub them cho vong sau: header Windows/MSVC chua co stub
    if A.iter >= 1:
        have = set(os.path.relpath(os.path.join(dp, f), STUB_DIR).replace("\\", "/").lower() for dp, dn, fn in os.walk(STUB_DIR) for f in fn)
        extra = json.load(io.open(STUB_EXTRA_JSON, encoding="utf-8")) if os.path.isfile(STUB_EXTRA_JSON) else []
        new = []
        for r in res:
            for h, ntu, nsite in r["headers"]:
                hn = h.replace("\\", "/").lower()
                if hn not in have and WIN_HEADER_RE.match(hn) and hn not in extra: new.append(hn)
        if new:
            extra = sorted(set(extra + new)); json.dump(extra, io.open(STUB_EXTRA_JSON, "w", encoding="utf-8"), indent=1)
            print("stub them cho vong sau:", new)
    for r in res: print("[%s] %d TU, %d sach (%d neu bo loi header), %d loi (%d tu header, %d Windows-only), %d cham tran" % (r["key"], r["tus"], r["passed"], r["passed_own"], r["errors"], r["err_hdr"], r["err_winonly"], r["capped"]))
    print("TONGHOP:", os.path.join(OUT, "TONGHOP_it%d.md" % A.iter))
