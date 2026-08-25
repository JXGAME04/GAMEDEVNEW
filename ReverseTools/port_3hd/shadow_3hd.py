# -*- coding: utf-8 -*-
"""Danh sach ten ham bi LOAI khoi 'khe ho engine' vi CAY SCRIPT LINUX co dinh nghia,
NHUNG tep dinh nghia lai KHONG nam trong bao dong Include cua 3 tinh nang.
=> Khi port, neu khong keo theo tep do thi Lua se bao 'attempt to call global'.
Neu ten do CO trong luamap ELF thi day thuc su la ham engine bi che khuat."""
import io, os, re, sys, json

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
LNX_SCRIPT = r"D:\ServerLinux\server1\script"
LNX_VNG = r"D:\ServerLinux\server1\vng_script"

import api_gap_3hd as G   # chay lai toan bo pipeline, dung lai bien module

closure = G.closure
in_closure = set()
for f in G.FEATS:
    for row in closure[f]:
        in_closure.add(row["rel"].lower())

# ten bi loai vi lib_defs
removed = {n: v for n, v in G.calls.items()
           if n not in G.LUA40_STDLIB and n in G.lib_defs}
rows = []
for n, v in sorted(removed.items()):
    p, ln = G.lib_def_where.get(n, ("", 0))
    rel = ""
    if p.lower().startswith(LNX_SCRIPT.lower()):
        rel = os.path.relpath(p, LNX_SCRIPT).replace("\\", "/")
    elif p.lower().startswith(LNX_VNG.lower()):
        rel = "vng_script/" + os.path.relpath(p, LNX_VNG).replace("\\", "/")
    inside = rel.lower() in in_closure
    rows.append((n, rel, ln, inside, n in G.luamap, n in G.reg))

out = [r for r in rows if not r[3]]
print("Ten bi loai vi script tu dinh nghia: %d ; trong do dinh nghia NGOAI bao dong: %d"
      % (len(rows), len(out)))
print("\n== NGOAI BAO DONG **VA** CO TRONG luamap ELF (nghi la ham engine bi che) ==")
k = 0
for n, rel, ln, inside, inmap, injx1 in out:
    if inmap:
        k += 1
        print("  %-28s luamap=YES jx1=%-5s  def: %s:%d" % (n, injx1, rel, ln))
print("  -> %d ten" % k)

json.dump([{"ten": n, "def_tep": rel, "def_dong": ln, "trong_bao_dong": inside,
            "co_trong_luamap": inmap, "co_trong_jx1": injx1}
           for n, rel, ln, inside, inmap, injx1 in rows],
          io.open(os.path.join(HERE, "shadow.json"), "w", encoding="utf-8"),
          ensure_ascii=False, indent=1)
