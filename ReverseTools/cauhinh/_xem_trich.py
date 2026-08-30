# -*- coding: utf-8 -*-
"""Xem ket qua trich hang so tu workflow (chi doc journal)."""
import io
import json
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
P = (r"C:\Users\nguye\.claude\projects\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto"
     r"\61d924bb-ce66-46e1-a22b-820305b4bde7\subagents\workflows"
     r"\wf_76762b95-5ed\journal.jsonl")

loc = sys.argv[1] if len(sys.argv) > 1 else None
for l in io.open(P, encoding="utf-8"):
    try:
        o = json.loads(l)
    except Exception:
        continue
    if o.get("type") != "result":
        continue
    r = o.get("result")
    if not isinstance(r, dict) or "hangso" not in r:
        continue
    nhom = r.get("nhom") or "?"
    if loc and loc.lower() not in nhom.lower():
        continue
    ds = r.get("hangso") or []
    print("=" * 78)
    print("NHOM: %s   (%d hang so)" % (nhom, len(ds)))
    if r.get("tom_tat"):
        print("TOM TAT:", r["tom_tat"][:400])
    print()
    for h in ds:
        chay = "" if h.get("dang_chay", True) else "  [KHONG CHAY]"
        ten = h.get("ten_bien") or ("ham " + (h.get("ham_bao_quanh") or "?"))
        gt = h.get("gia_tri")
        if gt is None:
            gt = h.get("gia_tri_chuoi", "?")
        print("  %-44s:%-5s %-28s = %-12s -> %-24s%s"
              % (h.get("tep", "")[:44], h.get("so_dong", "?"), ten[:28],
                 str(gt)[:12], h.get("ten_khoa_de_xuat", "")[:24], chay))
        if h.get("mo_ta"):
            print("        %s" % h["mo_ta"][:150])
    if r.get("canh_bao"):
        print()
        print("CANH BAO:", r["canh_bao"][:1500])
    print()
