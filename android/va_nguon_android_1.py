# -*- coding: utf-8 -*-
r"""[ANDROID 08/09] Dot 1 va nguon co guard cho ban Android (PC/Win32/SDL-Windows KHONG doi: moi dieu kien moi chi them
'|| defined(JX_PLATFORM_SDL)' o noi WIN32 da dung, hoac khoi #ifdef JX_POSIX / #ifndef JX_NO_DIRECTX).
Sua byte-an-toan (latin-1, giu CRLF/LF cua tung tep, kiem so byte cao truoc/sau). Chay lai vo hai (idempotent).
  1. KThread.h/.cpp, KMutex.h, KTimer.h/.cpp : #ifdef WIN32 -> #if defined(WIN32) || defined(JX_PLATFORM_SDL) (hinh dang lop nhu ban SDL Windows)
  2. KEngine.h : 4 include DirectX (KDDraw/KDSound/KDInput/KFileDialog) duoi #ifndef JX_NO_DIRECTX
  3. KJXPathFinder.h : operator< them const (libc++ can; MSVC trung tinh)
  4. JxNetShim.h : bo 3 typedef WSABUF/OVERLAPPED/WSADATA trong khoi POSIX (KPosixCompat.h da co)
  5. KFile.cpp : #include "zport.h" (tan du Linux server) khong lay khi JX_POSIX
  6. S3Client.cpp : them JxPosixMain() (JX_POSIX) + boc WinMain trong #ifndef JX_POSIX
"""
import io, os, re, sys
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
S = os.path.join(ROOT, "Sources")
def doc(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def ghi(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def cao(s): return sum(1 for c in s if ord(c) >= 128)
def eol(s): return "\r\n" if "\r\n" in s else "\n"
tong = 0

def sua(rel, fn):
    global tong
    p = os.path.join(S, rel); s = doc(p); n0 = cao(s)
    s2, msg = fn(s)
    if s2 == s: print("  =", rel, "(khong doi)", msg); return
    assert cao(s2) == n0, "byte cao doi o " + rel
    ghi(p, s2); tong += 1; print("  +", rel, msg)

# 1. hinh dang lop luong/khoa/dong ho
def win32_to_sdl(s):
    n = 0
    def r1(m):
        nonlocal n; n += 1; return "#if defined(WIN32) || defined(JX_PLATFORM_SDL)" + m.group(1)
    def r2(m):
        nonlocal n; n += 1; return "#if !defined(WIN32) && !defined(JX_PLATFORM_SDL)" + m.group(1)
    s = re.sub(r"^#\s*ifdef\s+WIN32\b(?!_)([ \t]*)", r1, s, flags=re.M)
    s = re.sub(r"^#\s*ifndef\s+WIN32\b(?!_)([ \t]*)", r2, s, flags=re.M)
    return s, "%d guard WIN32 -> WIN32||JX_PLATFORM_SDL" % n
for f in ("Engine/Src/KThread.h", "Engine/Src/KThread.cpp", "Engine/Src/KMutex.h", "Engine/Src/KTimer.h", "Engine/Src/KTimer.cpp"):
    sua(f, win32_to_sdl)

# 2. KEngine.h
def kengine(s):
    E = eol(s); n = 0
    a = '#include "KDDraw.h"' + E + '#include "KDSound.h"' + E + '#include "KDInput.h"' + E
    if a in s and "JX_NO_DIRECTX" not in s:
        s = s.replace(a, "#ifndef JX_NO_DIRECTX" + E + a + "#endif" + E); n += 1
        b = '#include "KFileDialog.h"' + E
        s = s.replace(b, "#ifndef JX_NO_DIRECTX" + E + b + "#endif" + E); n += 1
    return s, "%d khoi JX_NO_DIRECTX" % n
sua("Engine/Src/KEngine.h", kengine)

# 3. KJXPathFinder.h
def pathfinder(s):
    a = "bool operator<(CONST OpenNodePtr& p)" + eol(s)
    if a in s: return s.replace(a, "bool operator<(CONST OpenNodePtr& p) const" + eol(s)), "them const"
    return s, ""
sua("Core/Src/KJXPathFinder.h", pathfinder)

# 4. JxNetShim.h
def netshim(s):
    n = 0
    def r(m):
        nonlocal n; n += 1
        return "\t/* [ANDROID 08/09] %s: dung dinh nghia cua KPosixCompat.h */" % m.group(1)
    s = re.sub(r"^\ttypedef struct \{[^}]*\} (WSABUF|OVERLAPPED|WSADATA);[^\r\n]*", r, s, flags=re.M)
    return s, "%d typedef bo" % n
sua("MultiServer/Common/JxNetShim.h", netshim)

# 5. KFile.cpp
def kfile(s):
    E = eol(s)
    a = "#ifndef WIN32" + E + '#include "zport.h"' + E + "#endif" + E
    if a in s: return s.replace(a, "#if !defined(WIN32) && !defined(JX_POSIX)" + E + '#include "zport.h"' + E + "#endif" + E), "zport.h"
    return s, ""
sua("Engine/Src/KFile.cpp", kfile)

# 6. S3Client.cpp
def s3client(s):
    E = eol(s)
    if "JxPosixMain" in s: return s, ""
    i = s.find("int APIENTRY WinMain(HINSTANCE hInstance,")
    assert i >= 0, "khong thay WinMain"
    # cuoi ham: dong '}' o cot 0 dau tien sau WinMain
    m = re.compile(r"^\}[ \t]*\r?\n", re.M).search(s, i)
    assert m, "khong thay '}' ket WinMain"
    j = m.end()
    them = ("#ifdef JX_POSIX" + E +
            "// [ANDROID 08/09] Diem vao POSIX (Platform/JxAndroidMain.cpp goi tu SDL_main): phan WinMain khong dinh Windows" + E +
            "// (khong CrashLog/SEH/AntiHack/splash - cac tep do khong bien dich tren Android)." + E +
            "int JxPosixMain(int argc, char* argv[])" + E + "{" + E +
            "\t(void)argc; (void)argv;" + E +
            "\thInst = NULL;" + E +
            "\tLoadResolutionFromConfig();" + E +
            "\tSetEngineResolution(SCREEN_WIDTH, SCREEN_HEIGHT);" + E +
            "\tif (MyApp.Init(NULL))" + E + "\t\tMyApp.Run();" + E +
            "\treturn 0;" + E + "}" + E + "#else" + E)
    s = s[:i] + them + s[i:j] + "#endif // JX_POSIX" + E + s[j:]
    return s, "JxPosixMain + boc WinMain"
sua("S3Client/S3Client.cpp", s3client)
print("xong: %d tep doi" % tong)
