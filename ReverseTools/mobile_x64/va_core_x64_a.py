# -*- coding: utf-8 -*-
"""Dot Core x64 (a), byte-safe:
 1. KWin32.h: typedef KSCENEID (client = KUPARAM, may chu = unsigned int nhu cu) cho ID vat the canh (thuc chat la con tro KIpotRuntimeObj).
 2. KScenePlaceC.h/.cpp: AddObject/MoveObject tra KSCENEID, uRtoid la KSCENEID&.
 3. KMissle.h / KNpcRes.h / KObj.h: m_SceneID -> KSCENEID.
 4. CoreShell.h/.cpp + CoreDrawGameObj.h/.cpp: DrawGameObj(... KNPARAM nParam); SetMoveMap(..., KNPARAM nValue); SetSortItem(KNPARAM nType, ...).
 5. UiAuto.cpp: (unsigned int)m_cFilterMagic[i] -> (KNPARAM).
 6. MultiServer/Common/OpaqueUserData.h: unsigned long -> ULONG_PTR (Win32 y nguyen 4 byte).
"""
import io, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
S = ROOT + r"\Sources"

def rd(p): return io.open(p, "rb").read()
def wr(p, b, nb):
    assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127), "byte cao doi: " + p
    io.open(p, "wb").write(nb)
def rep(b, pairs, p):
    nb = b
    for old, new, cnt in pairs:
        c = nb.count(old)
        assert c == cnt, (p, old[:60], c, cnt)
        nb = nb.replace(old, new)
    return nb

# 1. KWin32.h
p = S + r"\Engine\Src\KWin32.h"; b = rd(p)
if b"KSCENEID" in b: print("1 KWin32.h: da va")
else:
    nl = b"\r\n" if b"\r\n" in b else b"\n"
    old = b"typedef intptr_t  KNPARAM;"
    new = nl.join([old,
        b"// [X64 08/09] ID vat the tren canh (KScenePlaceC::AddObject) thuc chat la con tro KIpotRuntimeObj:",
        b"// client x64 phai rong 8 byte; may chu (_SERVER) khong dung canh, giu unsigned int de bo cuc khong doi.",
        b"#ifdef _SERVER",
        b"typedef unsigned int KSCENEID;",
        b"#else",
        b"typedef KUPARAM      KSCENEID;",
        b"#endif"])
    wr(p, b, rep(b, [(old, new, 1)], p)); print("1 KWin32.h: OK")

# 2. KScenePlaceC.h / .cpp
p = S + r"\Core\Src\Scene\KScenePlaceC.h"; b = rd(p)
if b"KSCENEID" in b: print("2a KScenePlaceC.h: da va")
else:
    nb = rep(b, [(b"\tunsigned int AddObject(", b"\tKSCENEID AddObject(", 1), (b"\tunsigned int MoveObject(", b"\tKSCENEID MoveObject(", 1), (b"unsigned int& uRtoid", b"KSCENEID& uRtoid", 2)], p)
    wr(p, b, nb); print("2a KScenePlaceC.h: OK")
p = S + r"\Core\Src\Scene\KScenePlaceC.cpp"; b = rd(p)
if b"KSCENEID" in b: print("2b KScenePlaceC.cpp: da va")
else:
    nb = b.replace(b"unsigned int KScenePlaceC::AddObject(", b"KSCENEID KScenePlaceC::AddObject(", 1).replace(b"unsigned int KScenePlaceC::MoveObject(", b"KSCENEID KScenePlaceC::MoveObject(", 1)
    n1 = nb.count(b"unsigned int& uRtoid"); nb = nb.replace(b"unsigned int& uRtoid", b"KSCENEID& uRtoid")
    nb, n2 = re.subn(rb"\(unsigned int\)\s*(pLeaf|pRto|pObj)\b", lambda m: b"(KSCENEID)" + m.group(1), nb)
    wr(p, b, nb); print("2b KScenePlaceC.cpp: OK (uRtoid&", n1, ", cast", n2, ")")

# 3. fields
for f, cnt in ((r"\Core\Src\KMissle.h", 1), (r"\Core\Src\KNpcRes.h", 2), (r"\Core\Src\KObj.h", 1)):
    p = S + f; b = rd(p)
    if b"KSCENEID" in b: print("3", f, "da va"); continue
    nb, n = re.subn(rb"unsigned int(\s+)m_SceneID(?=[\[;\s])", lambda m: b"KSCENEID" + m.group(1) + b"m_SceneID", b)
    assert n == cnt, (f, n)
    wr(p, b, nb); print("3", f, "OK", n)

# 4. CoreShell / CoreDrawGameObj
p = S + r"\Core\Src\CoreShell.h"; b = rd(p)
if b"KNPARAM nParam) = 0;" in b and b"SetMoveMap(int nType, int nPos, KNPARAM nValue)" in b: print("4a CoreShell.h: da va")
else:
    nb = rep(b, [
        (b"virtual void DrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam) = 0;",
         b"virtual void DrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, KNPARAM nParam) = 0;", 1),
        (b"virtual\tvoid SetMoveMap(int nType, int nPos, int nValue) = 0;", b"virtual\tvoid SetMoveMap(int nType, int nPos, KNPARAM nValue) = 0;", 1),
        (b"virtual\tvoid SetSortItem(int nType, int nPos, int nValue) = 0;", b"virtual\tvoid SetSortItem(KNPARAM nType, int nPos, int nValue) = 0;", 1)], p)
    wr(p, b, nb); print("4a CoreShell.h: OK")
p = S + r"\Core\Src\CoreShell.cpp"; b = rd(p)
if b"KCoreShell::SetMoveMap(int nType, int nPos, KNPARAM nValue)" in b: print("4b CoreShell.cpp: da va")
else:
    nb = rep(b, [
        (b"\tvoid DrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam);",
         b"\tvoid DrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, KNPARAM nParam);", 1),
        (b"\tvoid SetMoveMap(int nType, int nPos, int nValue);", b"\tvoid SetMoveMap(int nType, int nPos, KNPARAM nValue);", 1),
        (b"\tvoid SetSortItem(int nType, int nPos, int nValue);", b"\tvoid SetSortItem(KNPARAM nType, int nPos, int nValue);", 1),
        (b"void KCoreShell::SetMoveMap(int nType, int nPos, int nValue)", b"void KCoreShell::SetMoveMap(int nType, int nPos, KNPARAM nValue)", 1),
        (b"void KCoreShell::SetSortItem(int nType, int nValue, int nPos)", b"void KCoreShell::SetSortItem(KNPARAM nType, int nValue, int nPos)", 1)], p)
    nb, n = re.subn(rb"void KCoreShell::DrawGameObj\(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam\)",
                    b"void KCoreShell::DrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, KNPARAM nParam)", nb)
    assert n == 1, n
    wr(p, b, nb); print("4b CoreShell.cpp: OK")
for f in (r"\Core\Src\CoreDrawGameObj.h", r"\Core\Src\CoreDrawGameObj.cpp"):
    p = S + f; b = rd(p)
    if b"KNPARAM nParam" in b: print("4c", f, "da va"); continue
    nb, n = re.subn(rb"(CoreDrawGameObj\(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, )int nParam\)", lambda m: m.group(1) + b"KNPARAM nParam)", b)
    assert n == 1, (f, n)
    wr(p, b, nb); print("4c", f, "OK")

# 5. UiAuto.cpp
p = S + r"\S3Client\Ui\UiCase\UiAuto.cpp"; b = rd(p)
if b"(KNPARAM)m_cFilterMagic[i]" in b: print("5 UiAuto.cpp: da va")
else:
    wr(p, b, rep(b, [(b"SetSortItem((unsigned int)m_cFilterMagic[i]", b"SetSortItem((KNPARAM)m_cFilterMagic[i]", 1)], p)); print("5 UiAuto.cpp: OK")

# 6. OpaqueUserData.h
p = S + r"\MultiServer\Common\OpaqueUserData.h"; b = rd(p)
if b"ULONG_PTR" in b: print("6 OpaqueUserData.h: da va")
else:
    nb = rep(b, [(b"\tunsigned long GetUserData() const", b"\tULONG_PTR GetUserData() const\t// [X64 08/09] rong bang con tro (Win32 van 4 byte)", 1),
                 (b"return reinterpret_cast<unsigned long>( GetUserPtr() );", b"return reinterpret_cast<ULONG_PTR>( GetUserPtr() );", 1),
                 (b"\tvoid SetUserData( unsigned long data )", b"\tvoid SetUserData( ULONG_PTR data )", 1)], p)
    wr(p, b, nb); print("6 OpaqueUserData.h: OK")
