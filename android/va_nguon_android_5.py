# -*- coding: utf-8 -*-
r"""[ANDROID 08/09] Dot 5 va nguon co guard (byte-an-toan, idempotent):
  1. UiInit.cpp : sua guard dot 4 (nuot mat '}' cua if (Ini.Load)) -> chi boc khoi if (szFile[0]) va dong CloseWindow
  2. S3Client.cpp : KMyApp::GameInit kiem tra ten tien trinh (Toolhelp) chi Windows
  3. UiParadeItem.h : enum UI_PLAYER_ATTRIBUTE : int (khai bao truoc thu 3)
  4. d3d9mini.h : bo struct IUnknown rieng (KPosixCompat.h da co)
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

def uiinit(s):
    E = eol(s)
    a = "\t\t}" + E + "\t}" + E + "\tKUiPlayVideo::CloseWindow(true);" + E + "#endif" + E
    if a in s:
        return s.replace(a, "\t\t}" + E + "#endif" + E + "\t}" + E + "#ifndef JX_POSIX" + E + "\tKUiPlayVideo::CloseWindow(true);" + E + "#endif" + E), "sua guard video"
    return s, ""
sua("S3Client/Ui/UiCase/UiInit.cpp", uiinit)

def s3client(s):
    E = eol(s)
    a = "\tDWORD aPid = GetCurrentProcessId();" + E + "\tPROCESSENTRY32 processInfo;" + E
    b = "\tError_SetErrorString(\"KMyApp::GameInit\");" + E
    if a not in s: return s, ""
    i = s.find(a); j = s.find(b, i); assert j > i, "neo GameInit"
    if s[i - 30:i].find("JX_POSIX") >= 0: return s, ""
    return s[:i] + "#ifndef JX_POSIX\t// [ANDROID 08/09] kiem ten tien trinh (Toolhelp) chi Windows" + E + s[i:j] + "#endif // JX_POSIX" + E + s[j:], "GameInit Toolhelp guard"
sua("S3Client/S3Client.cpp", s3client)

def parade(s):
    a = "enum UI_PLAYER_ATTRIBUTE;"
    if a in s: return s.replace(a, "enum UI_PLAYER_ATTRIBUTE : int;\t// [ANDROID 08/09] C++11 (clang): khai bao truoc phai co kieu nen"), "enum : int"
    return s, ""
sua("S3Client/Ui/UiCase/UiParadeItem.h", parade)

def mini(s):
    m = re.search(r"^#ifndef _WIN32\r?\nstruct IUnknown\r?\n\{\r?\n(?:.*\r?\n)*?\};\r?\n#endif\r?\n", s, re.M)
    if m: return s[:m.start()] + "/* IUnknown: KPosixCompat.h (POSIX) / unknwn.h (Windows) */" + eol(s) + s[m.end():], "bo IUnknown rieng"
    return s, ""
sua("Represent/Represent3/d3d9mini.h", mini)
print("xong dot 5")
