# -*- coding: utf-8 -*-
r"""[ANDROID 08/09] Dot 4 va nguon co guard (byte-an-toan, idempotent):
  1. S3Client.cpp : GenerateMiniDump + ExeptionFillert (SEH/minidump) duoi #ifndef JX_POSIX
  2. enum khai bao truoc (MS extension) -> C++11 'enum X : int;' : UiAddPoint.h, UiStatus.h (UI_PLAYER_ATTRIBUTE), UiSkillTree.h (SHORTCUT_KEY_INDEX);
     dinh nghia GameDataDef.h 'enum UI_PLAYER_ATTRIBUTE' -> ': int' (MSVC van dung int -> nhi phan khong doi)
  3. UiInit.cpp : video mo dau (KLVideo, chi Windows) duoi #ifndef JX_POSIX
  4. KCanvas.cpp : BltSurface duoi #ifndef JX_NO_DIRECTX
  5. d3d9mini.h : bo IID_IUnknown (KPosixCompat.h da co); them D3DPCMPCAPS_* (KFont3.cpp dung)
"""
import io, os, re
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
S = os.path.join(ROOT, "Sources")
def doc(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def ghi(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def cao(s): return sum(1 for c in s if ord(c) >= 128)
def eol(s): return "\r\n" if "\r\n" in s else "\n"
def sua(rel, fn):
    p = os.path.join(S, rel); s = doc(p); n0 = cao(s)
    s2, msg = fn(s)
    if s2 == s: print("  =", rel, "(khong doi)", msg); return
    assert cao(s2) == n0, "byte cao doi o " + rel
    ghi(p, s2); print("  +", rel, msg)

def s3client(s):
    E = eol(s)
    a = "int GenerateMiniDump(HANDLE hFile, LPEXCEPTION_POINTERS lpExceptionPointer, PWCHAR pwAppName)" + E
    if a not in s or "#ifndef JX_POSIX\t// [ANDROID 08/09] minidump" in s: return s, ""
    i = s.find(a)
    b = "LONG WINAPI ExeptionFillert(LPEXCEPTION_POINTERS lpExceptionInfo)" + E
    j = s.find(b, i); assert j > i
    m = re.compile(r"^\}[ \t]*\r?\n", re.M).search(s, j); assert m
    return s[:i] + "#ifndef JX_POSIX\t// [ANDROID 08/09] minidump/SEH chi Windows" + E + s[i:m.end()] + "#endif // JX_POSIX" + E + s[m.end():], "minidump guard"
sua("S3Client/S3Client.cpp", s3client)

def enum_fwd(name):
    def fn(s):
        a = "enum %s;" % name
        if a in s: return s.replace(a, "enum %s : int;\t// [ANDROID 08/09] C++11 (clang): khai bao truoc phai co kieu nen" % name), name
        return s, ""
    return fn
sua("S3Client/Ui/UiCase/UiAddPoint.h", enum_fwd("UI_PLAYER_ATTRIBUTE"))
sua("S3Client/Ui/UiCase/UiStatus.h", enum_fwd("UI_PLAYER_ATTRIBUTE"))
sua("S3Client/Ui/UiCase/UiSkillTree.h", enum_fwd("SHORTCUT_KEY_INDEX"))

def gamedatadef(s):
    E = eol(s)
    a = "enum UI_PLAYER_ATTRIBUTE" + E + "{" + E
    if a in s: return s.replace(a, "enum UI_PLAYER_ATTRIBUTE : int\t// [ANDROID 08/09] khop khai bao truoc 'enum UI_PLAYER_ATTRIBUTE : int' (UiAddPoint.h/UiStatus.h)" + E + "{" + E), "UI_PLAYER_ATTRIBUTE : int"
    return s, ""
sua("Core/Src/GameDataDef.h", gamedatadef)

def uiinit(s):
    E = eol(s)
    if "JX_POSIX" in s: return s, ""
    a = '#include "UiPlayVideo.h"' + E
    b = "\t\tif (szFile[0])" + E
    c = "\tKUiPlayVideo::CloseWindow(true);" + E
    assert a in s and b in s and c in s, "neo UiInit.cpp"
    s = s.replace(a, "#ifndef JX_POSIX\t// [ANDROID 08/09] video mo dau (KLVideo) chi Windows" + E + a + "#endif" + E)
    i = s.find(b); j = s.find(c, i) + len(c)
    s = s[:i] + "#ifndef JX_POSIX" + E + s[i:j] + "#endif" + E + s[j:]
    return s, "video guard"
sua("S3Client/Ui/UiCase/UiInit.cpp", uiinit)

def kcanvas(s):
    E = eol(s)
    a = "void KCanvas::BltSurface(LPDIRECTDRAWSURFACE pSurface, RECT* pDestRect)" + E + "{" + E
    if a not in s: return s, ""
    i = s.find(a) + len(a)
    if s[i:i + 20].startswith("#ifndef JX_NO_DIRECTX"): return s, ""
    m = re.compile(r"^\}[ \t]*\r?\n", re.M).search(s, i)
    return s[:i] + "#ifndef JX_NO_DIRECTX" + E + s[i:m.start()] + "#endif" + E + s[m.start():], "BltSurface guard"
sua("Engine/Src/KCanvas.cpp", kcanvas)

def mini(s):
    E = eol(s); n = 0
    s2, k = re.subn(r"^#ifndef _WIN32\r?\nJX_DEFINE_GUID\(IID_IUnknown,[^\r\n]*\r?\n#endif\r?\n", "", s, flags=re.M); n += k; s = s2
    if "D3DPCMPCAPS_NEVER" not in s:
        a = "#define D3DX_PI"
        i = s.find(a); assert i >= 0
        add = E.join(["#define D3DPCMPCAPS_NEVER        0x00000001", "#define D3DPCMPCAPS_LESS         0x00000002", "#define D3DPCMPCAPS_EQUAL        0x00000004",
                      "#define D3DPCMPCAPS_LESSEQUAL    0x00000008", "#define D3DPCMPCAPS_GREATER      0x00000010", "#define D3DPCMPCAPS_NOTEQUAL     0x00000020",
                      "#define D3DPCMPCAPS_GREATEREQUAL 0x00000040", "#define D3DPCMPCAPS_ALWAYS       0x00000080", ""])
        # dat truoc phan d3dx (sau cac #define co)
        j = s.rfind(E, 0, i) + len(E)
        s = s[:j] + add + s[j:]; n += 1
    return s, "%d cho" % n
sua("Represent/Represent3/d3d9mini.h", mini)
print("xong dot 4")
