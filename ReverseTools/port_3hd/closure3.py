# -*- coding: utf-8 -*-
"""Tinh BAO DONG (Include closure) cho 3 tinh nang tu ban Linux."""
import io, os, re, sys, json
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BS = chr(92)
LNX = r"D:\ServerLinux\server1\script"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"

FEATS = {
  "satthu":   {"dirs": ["task/tollgate/killer"],
               "files": ["task/tollgate/killbosshead.lua","task/tollgate/addtollgatenpc.lua"]},
  "phonglangdo": {"dirs": ["missions/fengling_ferry"], "files": []},
  "vuotai":   {"dirs": ["missions/challengeoftime"], "files": []},
}

def norm(s):
    s = s.replace(BS, "/")
    while "//" in s: s = s.replace("//", "/")
    s = s.lstrip("/")
    if s.lower().startswith("script/"): s = s[7:]
    return s

def includes_of(rel):
    p = os.path.join(LNX, rel.replace("/", os.sep))
    if not os.path.isfile(p): return []
    t = io.open(p, encoding="latin-1").read()
    out = []
    for line in t.split("\n"):
        s = line.split("--")[0]
        for m in re.finditer(r'(?:Include|IncludeLib|DynamicExecute[A-Za-z]*)\s*\(\s*(?:"([^"]+)"|\[\[([^\]]+)\]\])', s):
            v = m.group(1) or m.group(2)
            if v.lower().endswith(".lua"): out.append(norm(v))
    return out

res = {}
for feat, cfg in FEATS.items():
    seeds = list(cfg["files"])
    for d in cfg["dirs"]:
        for dp,_,fs in os.walk(os.path.join(LNX, d.replace("/", os.sep))):
            for f in fs:
                if f.lower().endswith(".lua"):
                    seeds.append(os.path.relpath(os.path.join(dp,f), LNX).replace(BS,"/"))
    seen, stack, depth = {}, [(s,0) for s in seeds], {}
    while stack:
        cur, dep = stack.pop()
        k = cur.lower()
        if k in seen: continue
        seen[k] = cur; depth[k] = dep
        for inc in includes_of(cur):
            if inc.lower() not in seen: stack.append((inc, dep+1))
    rows = []
    for k in sorted(seen):
        rel = seen[k]
        pl = os.path.join(LNX, rel.replace("/", os.sep))
        ps = os.path.join(SRV, rel.replace("/", os.sep))
        rows.append({"rel": rel, "seed": rel in seeds,
                     "linux": os.path.isfile(pl), "size": os.path.getsize(pl) if os.path.isfile(pl) else 0,
                     "in_jx1": os.path.isfile(ps)})
    res[feat] = rows
    miss = [r for r in rows if r["linux"] and not r["in_jx1"]]
    print("=== %s: %d tep trong bao dong, %d CHUA CO trong JX1, %d thieu ca ban Linux"
          % (feat, len(rows), len(miss), len([r for r in rows if not r["linux"]])))
    for r in rows:
        mark = "OK " if r["in_jx1"] else ("NEW" if r["linux"] else "!!!")
        print("   %s  %-70s %8d %s" % (mark, r["rel"], r["size"], "" if r["linux"] else "(KHONG CO trong ban Linux)"))
json.dump(res, io.open(os.path.join(os.path.dirname(__file__),"closure3.json"),"w",encoding="utf-8"), ensure_ascii=False, indent=1)
