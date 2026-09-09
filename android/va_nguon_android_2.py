# -*- coding: utf-8 -*-
r"""[ANDROID 08/09] Dot 2 va nguon co guard (byte-an-toan, idempotent):
  1. KDDraw.h : 5 ham inline dung DDSURFACEDESC/Lock/Blt -> #ifndef JX_NO_DIRECTX ... #else stub #endif
  2. KEngine.cpp : 2 khoi #ifndef _SERVER (Init/Exit DirectX) -> && !defined(JX_NO_DIRECTX)
  3. KWin32App.cpp : IsWindows8OrGreater() than ham duoi #ifndef JX_POSIX (RtlGetVersion/ntdll)
  4. KSpriteCodec.cpp : 'long KSpriteCodec::ConvertLine' -> 'LONG' (khai bao la LONG; LP64 long = 64 bit)
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

def kddraw(s):
    E = eol(s)
    if "JX_NO_DIRECTX" in s: return s, ""
    a = "inline LPVOID KDirectDraw::LockPrimaryBuffer()" + E
    b = "extern ENGINE_API KDirectDraw* g_pDirectDraw;" + E
    i = s.find(a); j = s.find(b)
    assert i >= 0 and j > i, "neo KDDraw.h"
    stub = (E.join([
        "#else\t// [ANDROID 08/09] JX_NO_DIRECTX: khong co DirectDraw, than ham trong (g_pDirectDraw luon NULL - xem Platform/KDDrawStub.cpp)",
        "inline LPVOID KDirectDraw::LockPrimaryBuffer() { return NULL; }",
        "inline void KDirectDraw::UnLockPrimaryBuffer() {}",
        "inline LPVOID KDirectDraw::LockBackBuffer() { return NULL; }",
        "inline void KDirectDraw::UnLockBackBuffer() {}",
        "inline void KDirectDraw::UpdateScreen(LPRECT lpRect) { (void)lpRect; }",
        "#endif // JX_NO_DIRECTX"]) + E)
    s = s[:i] + "#ifndef JX_NO_DIRECTX" + E + s[i:j] + stub + s[j:]
    return s, "boc 5 inline"
sua("Engine/Src/KDDraw.h", kddraw)

def kengine(s):
    n = 0
    def r(m):
        nonlocal n; n += 1; return "#if !defined(_SERVER) && !defined(JX_NO_DIRECTX)" + m.group(1)
    s2 = re.sub(r"^#ifndef _SERVER([ \t]*)(?=\r?\n[ \t]*//[ \t]*init ddraw|\r?\n[ \t]*if \(g_pDirectDraw\))", r, s, flags=re.M)
    return s2, "%d khoi" % n
sua("Engine/Src/KEngine.cpp", kengine)

def kwin32app(s):
    E = eol(s)
    if "JX_POSIX" in s: return s, ""
    a = "bool IsWindows8OrGreater()" + E + "{" + E
    b = "\treturn false; // default to \"not Windows 8 or greater\"" + E
    i = s.find(a); j = s.find(b, i)
    assert i >= 0 and j > i, "neo KWin32App.cpp"
    s = s[:i + len(a)] + "#ifndef JX_POSIX\t// [ANDROID 08/09] ntdll/RtlGetVersion chi co tren Windows" + E + s[i + len(a):j] + "#endif" + E + s[j:]
    return s, "IsWindows8OrGreater guard"
sua("Engine/Src/KWin32App.cpp", kwin32app)

def codec(s):
    a = "long KSpriteCodec::ConvertLine("
    if a in s: return s.replace(a, "LONG KSpriteCodec::ConvertLine("), "long -> LONG"
    return s, ""
sua("Engine/Src/KSpriteCodec.cpp", codec)
print("xong dot 2")
