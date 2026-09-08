# -*- coding: utf-8 -*-
"""Dot S3Client x64 (b): rad.h khoi __RADNT__ LockedIncrement/Decrement (hop ngu lock inc/dec) -> Interlocked* tren _M_X64."""
import io, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
p = ROOT + r"\Sources\KLVideo\rad.h"
b = io.open(p, "rb").read()
if b"[X64 08/09] Locked" in b: print("rad.h (Locked): da va"); sys.exit(0)
nl = b"\r\n" if b"\r\n" in b else b"\n"
old = b"#ifdef __RADNT__" + nl + b"  #define LockedIncrement(var) __asm { lock inc [var] }"
assert b.count(old) == 1, b.count(old)
new = nl.join([
    b"#if defined(__RADNT__) && defined(_M_X64)   /* [X64 08/09] Locked*: dung Interlocked* thay hop ngu lock inc/dec */",
    b"  #define LockedIncrement(var) InterlockedIncrement((volatile long*)&(var))",
    b"  #define LockedDecrement(var) InterlockedDecrement((volatile long*)&(var))",
    b"  void __inline LockedIncrementFunc(void PTR4* var) { InterlockedIncrement((volatile long*)var); }",
    b"  void __inline LockedDecrementFunc(void PTR4* var) { InterlockedDecrement((volatile long*)var); }",
    b"#elif defined(__RADNT__)",
    b"  #define LockedIncrement(var) __asm { lock inc [var] }"])
nb = b.replace(old, new, 1)
assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127)
io.open(p, "wb").write(nb); print("rad.h (Locked): OK")
