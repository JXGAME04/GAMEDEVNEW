# -*- coding: utf-8 -*-
"""Dot Core x64 (b): FkAutoSetFillterMagic(int nType,...) nhan con tro chuoi qua nType -> KNPARAM (decl .h, .cpp, impl) + caller S3Client."""
import io, os, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
S = ROOT + r"\Sources"
def rd(p): return io.open(p, "rb").read()
def wr(p, b, nb):
    assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127), p
    io.open(p, "wb").write(nb)
p = S + r"\Core\Src\CoreShell.h"; b = rd(p)
nb = b.replace(b"virtual\tvoid FkAutoSetFillterMagic(int nType, int nPos, int nValue) = 0;", b"virtual\tvoid FkAutoSetFillterMagic(KNPARAM nType, int nPos, int nValue) = 0;")
print("CoreShell.h:", int(nb != b)); wr(p, b, nb)
p = S + r"\Core\Src\CoreShell.cpp"; b = rd(p)
nb = b.replace(b"\tvoid FkAutoSetFillterMagic(int nType, int nPos, int nValue);", b"\tvoid FkAutoSetFillterMagic(KNPARAM nType, int nPos, int nValue);")
nb = nb.replace(b"void KCoreShell::FkAutoSetFillterMagic(int nType, int nValue, int nPos)", b"void KCoreShell::FkAutoSetFillterMagic(KNPARAM nType, int nValue, int nPos)")
print("CoreShell.cpp:", nb.count(b"FkAutoSetFillterMagic(KNPARAM")); wr(p, b, nb)
n = 0
for dp, dn, fn in os.walk(S + r"\S3Client"):
    if "vcpkg" in dp: continue
    for f in fn:
        if not f.lower().endswith((".cpp", ".h")): continue
        q = os.path.join(dp, f); b = rd(q)
        nb, k = re.subn(rb"FkAutoSetFillterMagic\(\s*\((?:unsigned int|UINT|DWORD|int)\)", b"FkAutoSetFillterMagic((KNPARAM)", b)
        if k: wr(q, b, nb); n += k; print("caller", f, k)
print("callers doi:", n)
