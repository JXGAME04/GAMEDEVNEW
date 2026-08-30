# -*- coding: utf-8 -*-
"""Loc ket qua trich: chi giu muc khai TEN = SO o CAP TEP va DANG CHAY.

In ra dang de chep thang vao danh sach `muc` cua bo khung noi_cauhinh.
"""
import io
import json
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
P = (r"C:\Users\nguye\.claude\projects\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto"
     r"\61d924bb-ce66-46e1-a22b-820305b4bde7\subagents\workflows"
     r"\wf_76762b95-5ed\journal.jsonl")

loc = sys.argv[1] if len(sys.argv) > 1 else None
DA_NOI = {"STRONGBOSS_NSTK", "SERVER_OPEN", "SERVER_TEST", "MONEY_RATE",
          "EXP_RATE", "DAMAGE_UPPER_BOSS", "DAMAGE_UP_TTPLD",
          "DAMAGE_UPPER_TTDL", "STRONGBOSS_ST", "STRONGBOSS_VA",
          "MAX_MAGIC_LEVEL", "TIME_BD_TK", "TIME_KT_TK", "TIME_NS_TK",
          "TIME_XOANPC_TK", "PLAYER_MS_LIMIT"}

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
    theo_tep = {}
    for h in ds:
        tb = h.get("ten_bien") or ""
        if not tb or "[" in tb or "." in tb:
            continue
        if not h.get("dang_chay", True):
            continue
        if tb in DA_NOI:
            continue
        if h.get("gia_tri") is None:
            continue
        theo_tep.setdefault(h.get("tep", "?"), []).append(h)

    print("=" * 78)
    print("NHOM: %s" % nhom)
    print("  tong %d muc, loc con %d muc o %d tep"
          % (len(ds), sum(len(v) for v in theo_tep.values()), len(theo_tep)))
    print()
    for tep, hs in sorted(theo_tep.items(), key=lambda x: -len(x[1])):
        print("  --- %s  (%d muc) ---" % (tep, len(hs)))
        seen = set()
        for h in hs:
            tb = h["ten_bien"]
            if tb in seen:
                continue
            seen.add(tb)
            mo = (h.get("mo_ta") or "").replace('"', "'")
            print('        ("%s", "%s",\n         "%s"),'
                  % (tb, h.get("ten_khoa_de_xuat", ""), mo[:150]))
        print()
    if r.get("canh_bao"):
        print("  CANH BAO:", (r["canh_bao"] or "")[:900])
    print()
