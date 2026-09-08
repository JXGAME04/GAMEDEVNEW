# -*- coding: utf-8 -*-
"""Vong 2 Engine x64 (byte-safe): KCodecLzo.cpp so sanh/tru con tro qua (unsigned) -> (KUPARAM);
KMemManager.cpp luu chi so trong con tro next: (void*)i -> (void*)(KNPARAM)i, (int)bh->next -> (KNPARAM)bh->next.
Win32: KUPARAM==unsigned int, KNPARAM==int -> ma may khong doi."""
import io, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
E = ROOT + r"\Sources\Engine\Src"

def wr(p, b, nb):
    assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127), p
    io.open(p, "wb").write(nb)

# KCodecLzo.cpp
p = E + r"\KCodecLzo.cpp"; b = io.open(p, "rb").read()
if b"(KUPARAM)m_pos" in b:
    print("KCodecLzo.cpp: da va")
else:
    rx = re.compile(rb"\(unsigned\)(\s*\(?\s*(?:m_pos|pIn|ip)\b(?!\s*\[))")   # bo qua (unsigned)(ip[3]) = gia tri byte
    nb, n = rx.subn(lambda m: b"(KUPARAM)" + m.group(1), b)
    assert n == 8, n
    # danh dau
    nb = nb.replace(b"\t\tif\t(((KUPARAM)m_pos < (KUPARAM)pIn)  ||", b"\t\tif\t(((KUPARAM)m_pos < (KUPARAM)pIn)  ||	// [X64 08/09] so sanh/tru con tro bang KUPARAM (uintptr_t)", 1)
    wr(p, b, nb); print("KCodecLzo.cpp: OK", n)

# KMemManager.cpp
p = E + r"\KMemManager.cpp"; b = io.open(p, "rb").read()
if b"(KNPARAM)i;" in b:
    print("KMemManager.cpp: da va")
else:
    pairs = [
        (b"\t\tbh->next = (void *)i;", b"\t\tbh->next = (void *)(KNPARAM)i;	// [X64 08/09] chi so nam trong con tro next"),
        (b"    if (((int)bh->next) == -1L)", b"    if (((KNPARAM)bh->next) == -1L)"),
        (b"        int i = (int)bh->next;", b"        int i = (int)(KNPARAM)bh->next;"),
    ]
    nb = b
    for o, n_ in pairs:
        assert nb.count(o) == 1, o
        nb = nb.replace(o, n_, 1)
    wr(p, b, nb); print("KMemManager.cpp: OK")
