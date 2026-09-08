# -*- coding: utf-8 -*-
"""Vong 3 Engine x64: ucl/ucl_ptr.h - tren Win64 'unsigned long' chi 4 byte nen ucl_ptr_t (phai rong bang con tro) bi cat cut.
Chen nhanh _WIN64 dung unsigned __int64 truoc chuoi chon kieu. Tep ASCII, CRLF; chi chen dong ASCII."""
import io, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
p = ROOT + r"\Sources\Engine\Src\ucl\ucl_ptr.h"
b = io.open(p, "rb").read()
if b"[X64 08/09]" in b:
    print("ucl_ptr.h: da va"); sys.exit(0)
anchor = b"/* Unsigned type that has *exactly* the same number of bits as a ucl_voidp */"
assert b.count(anchor) == 1
nl = b"\r\n" if b"\r\n" in b else b"\n"
block = nl.join([
    anchor,
    b"/* [X64 08/09] Win64: unsigned long chi 4 byte -> dung kieu 8 byte cho ucl_ptr_t/ucl_sptr_t */",
    b"#if !defined(__UCL_HAVE_PTR_T) && defined(_WIN64)",
    b"     typedef unsigned __int64   ucl_ptr_t;",
    b"     typedef __int64            ucl_sptr_t;",
    b"#    define __UCL_HAVE_PTR_T",
    b"#endif",
])
nb = b.replace(anchor, block, 1)
assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127)
io.open(p, "wb").write(nb); print("ucl_ptr.h: OK")
