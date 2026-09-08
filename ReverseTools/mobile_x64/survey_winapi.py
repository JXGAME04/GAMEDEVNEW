# -*- coding: utf-8 -*-
"""[PHA 1 08/09] Khao sat TINH phu thuoc Windows / MSVC trong ma client (Engine, Core, S3Client, Represent/iRepresent, Represent3):
dem theo nhom API (cua so/thong diep, tep, luong/dong bo, thoi gian, mang Winsock, DirectX cu, hop thoai, registry/ini, __asm/__declspec/
pragma comment, CRT MSVC) -> theo phan he va theo tep. Doc byte (latin-1), khong doi tep. Ket qua: <scratch>/clang_survey/WINAPI_TONGHOP.md
Cach dung: python survey_winapi.py <worktree_root>"""
import io, os, re, sys, collections
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
SRC = os.path.join(ROOT, "Sources")
OUT = os.path.join(r"C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\f7fe3690-d1f7-481d-b4f6-845330c7c14f\scratchpad", "clang_survey")
os.makedirs(OUT, exist_ok=True)
DIRS = [("Engine", os.path.join(SRC, "Engine", "Src")), ("Core", os.path.join(SRC, "Core", "Src")), ("S3Client", os.path.join(SRC, "S3Client")),
        ("iRepresent", os.path.join(SRC, "Represent", "iRepresent")), ("Represent3", os.path.join(SRC, "Represent", "Represent3")),
        ("JpgLib", os.path.join(SRC, "JpgLib")), ("FilterText", os.path.join(SRC, "FilterText"))]
GROUPS = [
    ("cua so / thong diep Win32", r"\b(HWND|CreateWindow\w*|DefWindowProc|RegisterClass\w*|PeekMessage|GetMessage|DispatchMessage|SendMessage|PostMessage|WM_[A-Z_]+|SetCursor|ShowCursor|GetClientRect|ClientToScreen|GetSystemMetrics|WinMain)\b"),
    ("tep / thu muc Win32", r"\b(CreateFile[AW]?|ReadFile|WriteFile|SetFilePointer|GetFileSize|CloseHandle|FindFirstFile\w*|FindNextFile\w*|CreateDirectory\w*|GetCurrentDirectory\w*|SetCurrentDirectory\w*|GetModuleFileName\w*|DeleteFile\w*|MoveFile\w*|CopyFile\w*|GetFileAttributes\w*|CreateFileMapping\w*|MapViewOfFile)\b"),
    ("luong / dong bo", r"\b(CreateThread|_beginthread\w*|CRITICAL_SECTION|EnterCriticalSection|LeaveCriticalSection|InitializeCriticalSection\w*|CreateEvent\w*|WaitForSingleObject|WaitForMultipleObjects|CreateMutex\w*|CreateSemaphore\w*|Interlocked\w+|Sleep|TlsAlloc|SetThreadPriority)\b"),
    ("thoi gian", r"\b(timeGetTime|GetTickCount\w*|QueryPerformanceCounter|QueryPerformanceFrequency|timeBeginPeriod|timeSetEvent|SetTimer|KillTimer|GetLocalTime|GetSystemTime\w*|SystemTimeToFileTime|FileTimeToSystemTime)\b"),
    ("mang Winsock", r"\b(WSAStartup|WSACleanup|WSAGetLastError|WSAEventSelect|WSAAsyncSelect|WSASend|WSARecv|WSAEnumNetworkEvents|WSACreateEvent|SOCKET|closesocket|ioctlsocket|WSAEWOULDBLOCK|WSABUF|CreateIoCompletionPort|GetQueuedCompletionStatus|OVERLAPPED)\b"),
    ("DirectX cu (DDraw/DInput/DSound/D3D9/D3DX)", r"\b(IDirectDraw\w*|LPDIRECTDRAW\w*|DirectDrawCreate\w*|IDirectInput\w*|LPDIRECTINPUT\w*|DirectInput8Create|IDirectSound\w*|LPDIRECTSOUND\w*|DirectSoundCreate\w*|IDirect3D\w*|LPDIRECT3D\w*|D3DX\w+|Direct3DCreate9\w*|DSBUFFERDESC|DIDEVICEOBJECTDATA)\b"),
    ("hop thoai / IME / clipboard", r"\b(MessageBox\w*|ImmGetContext|ImmSetCompositionWindow|ImmGetCompositionString\w*|ImmReleaseContext|ImmAssociateContext|HIMC|OpenClipboard|GetClipboardData|SetClipboardData|ShellExecute\w*|GetOpenFileName\w*)\b"),
    ("registry / ini / he thong", r"\b(RegOpenKey\w*|RegQueryValue\w*|RegSetValue\w*|RegCloseKey|GetPrivateProfile\w+|WritePrivateProfile\w+|GetVersionEx\w*|GlobalMemoryStatus\w*|GetComputerName\w*|GetUserName\w*|GetEnvironmentVariable\w*|LoadLibrary\w*|GetProcAddress|FreeLibrary|OutputDebugString\w*|IsDebuggerPresent|SetUnhandledExceptionFilter|MiniDumpWriteDump)\b"),
    ("__asm / MSVC-ism", r"(\b__asm\b|\b_asm\b|\b__declspec\b|#pragma\s+comment|\b__int64\b|\b__stdcall\b|\bWINAPI\b|\bCALLBACK\b|\b__cdecl\b|\b__forceinline\b|\b__try\b|\b__except\b|#pragma\s+pack)"),
    ("CRT chi co o MSVC", r"\b(_stricmp|_strnicmp|_strlwr|_strupr|_itoa|_ltoa|_ultoa|_snprintf|_vsnprintf|_splitpath|_makepath|_fullpath|_access|_mkdir|_rmdir|_chdir|_getcwd|_findfirst|_findnext|_findclose|_finddata_t|_ftime|_timeb|strcpy_s|strcat_s|sprintf_s|_strdup|_atoi64|_i64toa|stricmp|strnicmp|strlwr|strupr|itoa|ltoa)\b"),
]
GROUP_RE = [(g, re.compile(rx)) for g, rx in GROUPS]
HDR_RE = re.compile(r'#\s*include\s*[<"]([^>"]+)[>"]')
WIN_HDRS = {"windows.h", "winsock2.h", "winsock.h", "mswsock.h", "ws2tcpip.h", "ddraw.h", "dinput.h", "dsound.h", "d3d9.h", "d3dx9.h", "d3d11.h", "dxgi.h", "mmsystem.h", "imm.h", "shellapi.h", "shlwapi.h", "dbghelp.h", "tlhelp32.h", "psapi.h", "commdlg.h", "process.h", "io.h", "direct.h", "conio.h", "tchar.h", "winnt.h", "winbase.h", "wincrypt.h", "wininet.h", "objbase.h", "ole2.h", "wincodec.h", "intrin.h", "crtdbg.h"}

per_sub = collections.OrderedDict(); per_file = {}; hdr_count = collections.Counter(); hdr_files = collections.defaultdict(set)
n_tcvn3 = collections.Counter(); n_files = collections.Counter()
for sub, d in DIRS:
    if not os.path.isdir(d): continue
    per_sub[sub] = collections.Counter()
    for dp, dn, fn in os.walk(d):
        if "vcpkg_installed" in dp or os.sep + "x64" in dp or os.sep + "Release" in dp or os.sep + "Debug" in dp or "ClientRelease" in dp: continue
        for f in fn:
            if not f.lower().endswith((".cpp", ".c", ".h", ".inc", ".hpp")): continue
            p = os.path.join(dp, f)
            try: b = io.open(p, "rb").read()
            except Exception: continue
            n_files[sub] += 1
            if any(c > 127 for c in b): n_tcvn3[sub] += 1
            t = re.sub(rb"//[^\n]*", b"", b); t = re.sub(rb"/\*.*?\*/", b"", t, flags=re.S)
            s = t.decode("latin-1")
            rel = os.path.relpath(p, SRC)
            cnt = {}
            for g, rx in GROUP_RE:
                n = len(rx.findall(s))
                if n: cnt[g] = n; per_sub[sub][g] += n
            for h in HDR_RE.findall(s):
                hl = os.path.basename(h).lower()
                if hl in WIN_HDRS: hdr_count[hl] += 1; hdr_files[hl].add(rel)
            if cnt: per_file[rel] = (sub, cnt)

with io.open(os.path.join(OUT, "WINAPI_TONGHOP.md"), "w", encoding="utf-8", newline="\n") as w:
    w.write("# Khao sat tinh phu thuoc Windows/MSVC trong ma client (dem lan xuat hien, khong ke chu thich)\n\n")
    w.write("## Theo phan he\n\n| phan he | tep | tep co byte >127 (TCVN3/GBK) | " + " | ".join(g for g, _ in GROUPS) + " |\n|---|---|---|" + "---|" * len(GROUPS) + "\n")
    for sub, c in per_sub.items():
        w.write("| %s | %d | %d | %s |\n" % (sub, n_files[sub], n_tcvn3[sub], " | ".join(str(c.get(g, 0)) for g, _ in GROUPS)))
    w.write("\n## Header Windows duoc include (so tep)\n\n| header | tep |\n|---|---|\n")
    for h, n in hdr_count.most_common(): w.write("| %s | %d |\n" % (h, n))
    w.write("\n## 60 tep phu thuoc nhieu nhat (tong lan)\n\n| tep | phan he | tong | chi tiet |\n|---|---|---|---|\n")
    for rel, (sub, cnt) in sorted(per_file.items(), key=lambda kv: -sum(kv[1][1].values()))[:60]:
        w.write("| %s | %s | %d | %s |\n" % (rel.replace("\\", "/"), sub, sum(cnt.values()), "; ".join("%s=%d" % (g.split(" ")[0] + ("…" if " " in g else ""), n) for g, n in sorted(cnt.items(), key=lambda kv: -kv[1]))))
    w.write("\n## Tep co __asm (phai viet lai C)\n\n")
    asm = re.compile(r"\b_?_asm\b")
    for sub, d in DIRS:
        if not os.path.isdir(d): continue
        for dp, dn, fn in os.walk(d):
            if "vcpkg_installed" in dp: continue
            for f in fn:
                if not f.lower().endswith((".cpp", ".c", ".h", ".inc")): continue
                p = os.path.join(dp, f); b = io.open(p, "rb").read()
                t = re.sub(rb"//[^\n]*", b"", b); t = re.sub(rb"/\*.*?\*/", b"", t, flags=re.S); s = t.decode("latin-1")
                n = len(asm.findall(s))
                if n:
                    guarded = len(re.findall(r"#\s*if(?:n?def)?\s*[^\n]*(?:_WIN64|_M_X64|_M_IX86|WIN32)", s))
                    w.write("- `%s`: %d khoi __asm, %d dong #if kiem tra kien truc\n" % (os.path.relpath(p, SRC).replace("\\", "/"), n, guarded))
print("xong:", os.path.join(OUT, "WINAPI_TONGHOP.md"))
for sub, c in per_sub.items(): print("%-11s tep=%3d tcvn3=%3d | %s" % (sub, n_files[sub], n_tcvn3[sub], " ".join("%s=%d" % (g.split(" ")[0], c.get(g, 0)) for g, _ in GROUPS)))
