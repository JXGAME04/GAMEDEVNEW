# -*- coding: utf-8 -*-
r"""[ANDROID 08/09] Dot 3 va nguon co guard (byte-an-toan, idempotent):
  1. d3d9mini.h : bo HMONITOR / PALETTEENTRY / IsEqualGUID trong khoi POSIX (KPosixCompat.h da co)
  2. KEngine.h : JX_NO_DIRECTX chi loai KDInput.h + KFileDialog.h (KDDraw.h/KDSound.h nay bien dich duoc nho typedef + stub)
  3. KRepresentShell3.cpp : Test3D() (DirectDraw7 caps) tra true khi JX_NO_DIRECTX; AdviseRepresent/UnAdviseRepresent HRESULT -> long (khop iRepresentShell.h)
  4. TextureResMgr.cpp : Rep3DemClient() (Toolhelp) tra 1 khi JX_POSIX
  5. KCanvas.cpp : 5 khoi dung DirectDrawSurface (Terminate/FillCanvas/Clear/LockCanvas/UnlockCanvas) duoi #ifndef JX_NO_DIRECTX
  6. KRegion.cpp : __try/__except -> doc thang khi JX_POSIX
  7. ScriptFuns.cpp : typedef _SYSTEMTIME rieng chi khi !WIN32 && !JX_POSIX
  8. MapHandler.cpp : bien cuc bo ten M_PI dung macro M_PI cua math.h -> #undef truoc ham
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

def mini(s):
    n = 0
    for pat in [r"^\ttypedef void\* HMONITOR;[ \t]*\r?\n", r"^\ttypedef struct tagPALETTEENTRY \{[^\n]*\} PALETTEENTRY;[ \t]*\r?\n",
                r"^inline bool IsEqualGUID\(const GUID& a, const GUID& b\) \{ return memcmp\(&a, &b, sizeof\(GUID\)\) == 0; \}[ \t]*\r?\n"]:
        s, k = re.subn(pat, "", s, flags=re.M); n += k
    return s, "%d dong bo (KPosixCompat.h da co)" % n
sua("Represent/Represent3/d3d9mini.h", mini)

def kengine(s):
    E = eol(s)
    a = "#ifndef JX_NO_DIRECTX" + E + '#include "KDDraw.h"' + E + '#include "KDSound.h"' + E + '#include "KDInput.h"' + E + "#endif" + E
    b = '#include "KDDraw.h"' + E + '#include "KDSound.h"' + E + "#ifndef JX_NO_DIRECTX\t// [ANDROID 08/09] KDDraw.h/KDSound.h bien dich duoc (typedef + stub); DirectInput thi khong" + E + '#include "KDInput.h"' + E + "#endif" + E
    if a in s: return s.replace(a, b), "chi loai KDInput.h"
    return s, ""
sua("Engine/Src/KEngine.h", kengine)

def shell3(s):
    E = eol(s); n = 0
    a = "bool Test3D()" + E + "{" + E
    if a in s and "JX_NO_DIRECTX" not in s:
        i = s.find(a) + len(a)
        m = re.compile(r"^\}[ \t]*\r?\n", re.M).search(s, i)
        s = s[:i] + "#ifdef JX_NO_DIRECTX" + E + "\treturn true;\t// [ANDROID 08/09] khong co DirectDraw7 de hoi VRAM" + E + "#else" + E + s[i:m.start()] + "#endif" + E + s[m.start():]
        n += 1
    for f in ("AdviseRepresent", "UnAdviseRepresent"):
        a2 = "HRESULT KRepresentShell3::%s(IInlinePicEngineSink* pSink)" % f
        if a2 in s: s = s.replace(a2, "long KRepresentShell3::%s(IInlinePicEngineSink* pSink)\t// [ANDROID 08/09] khop 'long' cua iRepresentShell.h (LP64)" % f); n += 1
    return s, "%d cho" % n
sua("Represent/Represent3/KRepresentShell3.cpp", shell3)

def texmgr(s):
    E = eol(s)
    a = "static int Rep3DemClient()" + E + "{" + E
    if a in s and "JX_POSIX" not in s:
        i = s.find(a) + len(a)
        m = re.compile(r"^\}[ \t]*\r?\n", re.M).search(s, i)
        s = s[:i] + "#ifdef JX_POSIX" + E + "\treturn 1;\t// [ANDROID 08/09] mot tien trinh" + E + "#else" + E + s[i:m.start()] + "#endif" + E + s[m.start():]
        return s, "Rep3DemClient guard"
    return s, ""
sua("Represent/Represent3/TextureResMgr.cpp", texmgr)

def kcanvas(s):
    E = eol(s); n = 0
    if "JX_NO_DIRECTX" in s: return s, ""
    def boc_khoi(s, fn_header, start_pat):
        """trong ham fn_header, boc tu dong khop start_pat toi dong '\t}' dau tien sau do (khoi 1 tab)"""
        i = s.find(fn_header); assert i >= 0, fn_header
        m = re.compile(start_pat, re.M).search(s, i); assert m, start_pat + " trong " + fn_header
        m2 = re.compile(r"^\t\}[ \t]*\r?\n", re.M).search(s, m.end()); assert m2
        return s[:m.start()] + "#ifndef JX_NO_DIRECTX\t// [ANDROID 08/09] khong co DirectDrawSurface (m_pSurface luon NULL)" + E + s[m.start():m2.end()] + "#endif" + E + s[m2.end():]
    s = boc_khoi(s, "void KCanvas::Terminate()", r"^\tif \(m_pSurface\)[ \t]*\r?\n"); n += 1
    s = boc_khoi(s, "void KCanvas::FillCanvas(WORD wColor)", r"^\tif \(m_pSurface\)[ \t]*\r?\n"); n += 1
    s = boc_khoi(s, "void KCanvas::Clear(int nX, int nY, int nWidth, int nHeight, int nColor)", r"^\tDDBLTFX ddbltfx;[ \t]*\r?\n"); n += 1
    s = boc_khoi(s, "void* KCanvas::LockCanvas(int& nPitch)", r"^\tif \(m_pSurface\)[ \t]*\r?\n"); n += 1
    s = boc_khoi(s, "void KCanvas::UnlockCanvas()", r"^\tif \(m_pSurface\)[ \t]*\r?\n"); n += 1
    return s, "%d khoi" % n
sua("Engine/Src/KCanvas.cpp", kcanvas)

def kregion(s):
    E = eol(s)
    a = "\t\t__try {" + E + "\t\t\tnRef = (int)pBuffer[index];" + E + "\t\t} __except (EXCEPTION_EXECUTE_HANDLER) {" + E + "\t\t\treturn FALSE;" + E + "\t\t}" + E
    if a in s and "JX_POSIX" not in s:
        return s.replace(a, "#ifdef JX_POSIX\t// [ANDROID 08/09] khong co SEH" + E + "\t\tnRef = (int)pBuffer[index];" + E + "#else" + E + a + "#endif" + E), "SEH"
    return s, ""
sua("Core/Src/KRegion.cpp", kregion)

def scriptfuns(s):
    E = eol(s)
    a = "#ifndef WIN32" + E + "typedef struct  _SYSTEMTIME" + E
    if a in s: return s.replace(a, "#if !defined(WIN32) && !defined(JX_POSIX)" + E + "typedef struct  _SYSTEMTIME" + E), "_SYSTEMTIME guard"
    return s, ""
sua("Core/Src/ScriptFuns.cpp", scriptfuns)

def maphandler(s):
    E = eol(s)
    a = "void MapHandler::generateTrapLayers(Point center) {" + E
    if a in s and "#undef M_PI" not in s:
        return s.replace(a, "#ifdef M_PI" + E + "#undef M_PI\t// [ANDROID 08/09] math.h co macro M_PI, ham nay dung bien cuc bo cung ten" + E + "#endif" + E + a), "undef M_PI"
    return s, ""
sua("Core/Src/MapHandler.cpp", maphandler)
print("xong dot 3")
