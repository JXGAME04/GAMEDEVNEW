# -*- coding: utf-8 -*-
import io
import json
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
P = (r"C:\Users\nguye\.claude\projects\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto"
     r"\61d924bb-ce66-46e1-a22b-820305b4bde7\subagents\workflows"
     r"\wf_68d6ea94-7a4\journal.jsonl")

loc = sys.argv[1] if len(sys.argv) > 1 else None
for l in io.open(P, encoding="utf-8"):
    try:
        o = json.loads(l)
    except Exception:
        continue
    if o.get("type") != "result":
        continue
    r = o.get("result")
    if not isinstance(r, dict) or "phat_hien" not in r:
        continue
    kl = r.get("ket_luan")
    if loc and loc.upper() not in (kl or "").upper():
        continue
    print("=" * 78)
    print("KET LUAN:", kl)
    print("TOM TAT :", (r.get("tom_tat") or "")[:400])
    for f in r.get("phat_hien") or []:
        if f.get("muc_do") == "NHE":
            continue
        print()
        print("  [%s] %s" % (f.get("muc_do"), f.get("vitri")))
        print("    van de   :", (f.get("van_de") or "")[:400])
        print("    kich hoat:", (f.get("kich_hoat") or "")[:250])
        if f.get("de_xuat"):
            print("    de xuat  :", f["de_xuat"][:250])
    print()
