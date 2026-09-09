# -*- coding: utf-8 -*-
r"""[ANDROID 08/09] Dot 6: mo tep bang SDL_IOFromFile (pak / ZSPR) phai qua JxPathPosix tren POSIX ("\" -> "/", ha chu thuong).
  XPackFile.cpp XP_Open, ZSPRPackFile.h : #ifdef JX_POSIX -> JxPathPosix(name) truoc SDL_IOFromFile. Byte-an-toan, idempotent.
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

def xpack(s):
    E = eol(s)
    a = "static inline HANDLE XP_Open(const char* pszName) { SDL_IOStream* p = SDL_IOFromFile(pszName, \"rb\"); return p ? (HANDLE)p : INVALID_HANDLE_VALUE; }"
    if a not in s: return s, "" if "JX_POSIX" in s else "khong thay neo XP_Open"
    b = ("static inline HANDLE XP_Open(const char* pszName)" + E + "{" + E +
         "#ifdef JX_POSIX" + E +
         "\tchar szPosix[1024]; pszName = JxPathPosix(pszName, szPosix, sizeof(szPosix));\t// [ANDROID 08/09] \\ -> /, ha chu thuong" + E +
         "#endif" + E +
         "\tSDL_IOStream* p = SDL_IOFromFile(pszName, \"rb\"); return p ? (HANDLE)p : INVALID_HANDLE_VALUE;" + E + "}")
    return s.replace(a, b), "XP_Open qua JxPathPosix"
sua("Engine/Src/XPackFile.cpp", xpack)

def zspr(s):
    E = eol(s)
    a = "{ SDL_IOStream* pIO = SDL_IOFromFile(name, \"rb\"); m_hFile = pIO ? (HANDLE)pIO : INVALID_HANDLE_VALUE; }"
    if a not in s: return s, "" if "JX_POSIX" in s else "khong thay neo ZSPR"
    b = ("{" + E + "#ifdef JX_POSIX" + E +
         "\t\t\tchar szPosix[1024]; name = JxPathPosix(name, szPosix, sizeof(szPosix));\t// [ANDROID 08/09]" + E +
         "#endif" + E +
         "\t\t\tSDL_IOStream* pIO = SDL_IOFromFile(name, \"rb\"); m_hFile = pIO ? (HANDLE)pIO : INVALID_HANDLE_VALUE; }")
    return s.replace(a, b), "ZSPR qua JxPathPosix"
sua("Engine/Src/ZSPRPackFile.h", zspr)
print("xong dot 6")
