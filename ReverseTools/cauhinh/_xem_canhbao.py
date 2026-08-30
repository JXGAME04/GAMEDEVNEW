# -*- coding: utf-8 -*-
import io
import json
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
P = (r"C:\Users\nguye\.claude\projects\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto"
     r"\61d924bb-ce66-46e1-a22b-820305b4bde7\subagents\workflows"
     r"\wf_47b93acf-281\journal.jsonl")

loc = sys.argv[1].upper() if len(sys.argv) > 1 else None
for l in io.open(P, encoding="utf-8"):
    try:
        o = json.loads(l)
    except Exception:
        continue
    if o.get("type") != "result":
        continue
    r = o.get("result")
    if not isinstance(r, dict) or "bang_chung" not in r:
        continue
    if loc and loc not in (r.get("ma") or "").upper():
        continue
    print("=" * 78)
    print("MA       :", r.get("ma"), "|", r.get("ket_luan"), "| muc do:", r.get("muc_do"))
    print("TOM TAT  :", r.get("tom_tat"))
    print("HAU QUA  :", (r.get("hau_qua") or "")[:700])
    if r.get("do_luong"):
        print("DO LUONG :", r["do_luong"][:600])
    if r.get("cho_sai_cua_canh_bao"):
        print("CANH BAO SAI O:", r["cho_sai_cua_canh_bao"][:500])
    print("DOI CAN BANG:", r.get("doi_can_bang"))
    if r.get("de_xuat_va"):
        print("DE XUAT  :", r["de_xuat_va"][:600])
    print("BANG CHUNG:")
    for b in (r.get("bang_chung") or [])[:12]:
        print("   %-52s %s" % (b.get("vitri", "")[:52], (b.get("noi_dung") or "")[:110]))
    print()
