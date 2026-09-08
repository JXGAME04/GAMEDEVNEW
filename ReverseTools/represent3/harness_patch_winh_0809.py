# -*- coding: utf-8 -*-
"""harness_patch_winh_0809.py - REP_WINH=808: vung client cua so cao hon swapchain (nhu Game.exe cu +40 px) de tai hien
'bo 31 khung/s' cua game (harness cua so 1024x768 chuan thi 0)."""
import io, sys
p = r"D:\GAMEDEVNEW_wt_rep3\ReverseTools\represent3\rep_harness.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read()
if "REP_WINH" in s:
    print("da va"); sys.exit(0)
old = "\tRECT rc = {0, 0, 1024, 768};"
if s.count(old) != 1: print("FAIL neo RECT"); sys.exit(1)
new = "\tint nWinH = (getenv(\"REP_WINH\") && atoi(getenv(\"REP_WINH\")) > 0) ? atoi(getenv(\"REP_WINH\")) : 768;\t// [08/09] REP_WINH=808\n\tRECT rc = {0, 0, 1024, nWinH};"
s = s.replace(old, new)
io.open(p, "w", encoding="latin-1", newline="").write(s)
print("OK harness REP_WINH")
