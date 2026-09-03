# -*- coding: utf-8 -*-
"""Sua lai dong chi muc dau MEMORY.md (lan truoc bi bash nuot dau backtick)."""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
M = (r"C:\Users\nguye\.claude\projects"
     r"\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\memory")
p = os.path.join(M, "MEMORY.md")
d = io.open(p, encoding="utf-8").read()

DONG = (
    "- [\u2620\ufe0f\U0001f9ee\U0001f534 31/08 **\u00d4 CH\u1ebeT \u2014 g\u1ed1c"
    " th\u1eadt c\u1ee7a \u201c\u0111\u00e1nh kh\u00f4ng tr\u00fang con qu\u00e1i"
    " cu\u1ed1i trong Vi\u00eam \u0110\u1ebf\u201d.** `KNpc.cpp:2344` (m\u00e3"
    " m\u1edbi 23/08) **tr\u1eeb `m_pNpcRef` L\u1ea6N HAI** \u21d2 \u00f4 v\u1ec1"
    " 0 \u21d2 `FindNpc` tho\u00e1t ngay \u1edf `KRegion.h:191` \u21d2 **m\u1ecdi"
    " th\u1ee9 \u0111\u1ee9ng tr\u00ean \u00f4 \u0111\u00f3 t\u00e0ng h\u00ecnh"
    " tr\u01b0\u1edbc va ch\u1ea1m** d\u00f9 v\u1eabn s\u1ed1ng v\u00e0 v\u1eabn"
    " \u0111\u00e1nh tr\u1ea3. \u0110o: c\u00f9ng con qu\u00e1i \u2014 \u00f4"
    " (37;7,2) **440 qu\u00e9t 0 ch\u1ea1m**, ba \u00f4 kh\u00e1c 25-50%%. \u0110\u00e3"
    " v\u00e1 1 d\u00f2ng + nh\u00e3n `[REFOAN]`, d\u1ef1ng s\u1ea1ch 2 c\u1ea5u"
    " h\u00ecnh, **`CoreServer.dll.moi` `9ed3cb2a907f` CH\u1edc SWAP**]"
    "(jx1-npcref-o-chet-3108.md) \u2014 commit `aaf5bb24`.\n"
).replace("%%", "%")

dong = d.split("\n")
# dong dau hien tai la dong hong do bash nuot backtick -> thay the
if dong and "jx1-npcref-o-chet-3108" in dong[0]:
    print("thay dong dau (dai %d ky tu) bang ban dung" % len(dong[0]))
    d = DONG + "\n".join(dong[1:])
else:
    print("dong dau khong phai dong can sua - CHEN them")
    d = DONG + d

io.open(p, "w", encoding="utf-8", newline="").write(d)
print("xong, MEMORY.md = %d byte" % os.path.getsize(p))
print()
print("dong dau moi:")
print(io.open(p, encoding="utf-8").read().split("\n")[0])
