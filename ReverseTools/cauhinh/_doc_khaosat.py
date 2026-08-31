# -*- coding: utf-8 -*-
"""Doc ket qua workflow khao sat 4 viec (wf_ff69e431-72a)."""
import io
import json
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
D = (r"C:\Users\nguye\.claude\projects"
     r"\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto"
     r"\61d924bb-ce66-46e1-a22b-820305b4bde7\subagents\workflows"
     r"\wf_ff69e431-72a")
J = os.path.join(D, "journal.jsonl")

loc = sys.argv[1].lower() if len(sys.argv) > 1 else None

for l in io.open(J, encoding="utf-8"):
    try:
        o = json.loads(l)
    except Exception:
        continue
    if o.get("type") != "result":
        continue
    r = o.get("result")
    if not isinstance(r, dict):
        continue
    kq = r.get("kq") if "kq" in r else r
    ten = r.get("ten") or r.get("nhan") or r.get("len") or "?"
    if loc and loc not in str(ten).lower():
        continue
    if not isinstance(kq, dict):
        continue
    print("=" * 78)
    print("### %s" % ten)
    print("=" * 78)
    print(kq.get("tom_tat", ""))
    print()
    print("-- PHAT HIEN --")
    for x in kq.get("phat_hien", []):
        print("  * %s" % x.get("muc", "")[:150])
        print("    bang chung: %s" % str(x.get("bang_chung", ""))[:220])
        print("    y nghia   : %s" % str(x.get("y_nghia", ""))[:220])
    print()
    print("-- DE XUAT --")
    for x in kq.get("de_xuat", []):
        print("  * %s" % x.get("viec", "")[:150])
        print("    %s | %s" % (str(x.get("tep_dong", ""))[:90],
                               str(x.get("ly_do", ""))[:150]))
    print()
    print("-- RUI RO --")
    for x in kq.get("rui_ro", []):
        print("  ! %s" % str(x)[:220])
    print()
