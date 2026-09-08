# -*- coding: utf-8 -*-
"""Vong 5 Engine x64: KPakFile.h macro KSG_IMAGE_CONTENT_SIZE ep con tro (offsetof) qua (unsigned) -> (unsigned)(KUPARAM)."""
import io, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
p = ROOT + r"\Sources\Engine\Src\KPakFile.h"
b = io.open(p, "rb").read()
old = b"#define\tKSG_IMAGE_CONTENT_SIZE(w, h)    ((unsigned)((&((KSGImageContent *)0)->Data[0])) + w * h * 2)"
new = b"#define\tKSG_IMAGE_CONTENT_SIZE(w, h)    ((unsigned)(KUPARAM)((&((KSGImageContent *)0)->Data[0])) + w * h * 2)	// [X64 08/09] offsetof qua KUPARAM"
if new in b: print("KPakFile.h: da va"); sys.exit(0)
assert b.count(old) == 1, b.count(old)
nb = b.replace(old, new, 1)
assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127)
io.open(p, "wb").write(nb); print("KPakFile.h: OK")
