# -*- coding: utf-8 -*-
"""Kiem nguoc: t71 co that su bat duoc loi khong (chay tren ban TRUOC khi va)."""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import t71_quet_goi_nil_thanchunk as t  # noqa: E402
import lua_ham as lh  # noqa: E402

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
S = t.S
he = t.ham_engine()


def quet(p):
    raw = t.doc(p)
    dong = [x.rstrip("\r") for x in raw.replace("\r\n", "\n").split("\n")]
    trong = [False] * len(dong)
    cb, dang = 0, False
    for i, l in enumerate(dong):
        s = lh.sach(l)
        if re.search(r"\bfunction\b", s) and not dang:
            dang, cb = True, 0
        if dang:
            trong[i] = True
            cb += lh.can_bang(l)
            if cb == 0:
                dang = False
    hop_le = set(he) | t.khai_trong(p) | t.BO
    for m in t.RE_INC.finditer(raw):
        q = m.group(1).replace("/", "\\").lstrip("\\")
        if q.lower().startswith("script\\"):
            q = q[len("script\\"):]
        q = os.path.join(S, q)
        if os.path.isfile(q):
            hop_le |= t.khai_trong(q)
    loi = []
    for i, l in enumerate(dong):
        if trong[i] or l.lstrip().startswith("--") or not l.strip():
            continue
        for m in t.RE_GOI.finditer(lh.sach(l)):
            if m.group(1) not in hop_le:
                loi.append((i + 1, m.group(1)))
    return loi


B = os.path.join(S, "task", "partner", "reward")
for nhan, p in (("BAN LOI (truoc khi va)",
                 os.path.join(B, "partner_reward2.lua.truoc_valoi")),
                ("BAN DA VA            ",
                 os.path.join(B, "partner_reward2.lua"))):
    if not os.path.isfile(p):
        print("%s -> khong co tep" % nhan)
        continue
    loi = quet(p)
    print("%s -> %d loi  %s" % (nhan, len(loi), loi[:3]))
