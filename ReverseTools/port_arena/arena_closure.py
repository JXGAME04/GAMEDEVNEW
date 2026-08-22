import io, os, re, sys, filecmp, collections
# Bao dong Include de quy cua cay missions/arena (ban sao cua tinsu_closure.py, doi ROOTS)
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BS = chr(92)
LNX = r"D:\ServerLinux\server1\script"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
SRV2 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\scriptjx2\tong_vn"
ROOTS = ["missions/arena"]
ROOTFILES = []
SKIP = set()
FULL = "--full" in sys.argv   # de quy ca vao tep du an DA CO (de biet cay that cua Linux)

def norm(s):
    s = s.replace(BS, "/")
    while "//" in s: s = s.replace("//", "/")
    s = s.lstrip("/")
    if s.lower().startswith("script/"): s = s[7:]
    return s

def includes_of(relpath):
    p = os.path.join(LNX, relpath)
    if not os.path.isfile(p): return []
    t = io.open(p, encoding="latin-1").read()
    out = []
    for line in t.split("\n"):
        s = line.split("--")[0]
        for m in re.finditer(r'Include\s*\(\s*(?:"([^"]+)"|\[\[([^\]]+)\]\])', s):
            out.append(norm(m.group(1) or m.group(2)))
    return out

seeds = list(ROOTFILES)
for r in ROOTS:
    for dp, _, fs in os.walk(os.path.join(LNX, r)):
        for f in fs:
            if f.lower().endswith(".lua"):
                seeds.append(os.path.relpath(os.path.join(dp, f), LNX).replace(BS, "/"))
seen = {}
stack = list(seeds)
origin = {s: "ROOT" for s in seeds}
while stack:
    cur = stack.pop()
    key = cur.lower()
    if key in seen: continue
    seen[key] = cur
    if key in {x.lower() for x in SKIP}: continue
    if not FULL and (os.path.isfile(os.path.join(SRV, cur)) or (cur.lower().startswith("tong/") and os.path.isfile(os.path.join(SRV2, cur[5:])))): continue
    for inc in includes_of(cur):
        if inc.lower() not in seen:
            origin.setdefault(inc, cur)
            stack.append(inc)

rows = []
for key, rel in sorted(seen.items()):
    lp = os.path.join(LNX, rel)
    sp = os.path.join(SRV, rel)
    if not os.path.isfile(lp):
        st = "LINUX-KHONG"
    elif os.path.isfile(sp):
        st = "IDENT" if filecmp.cmp(lp, sp, shallow=False) else "DIFF"
    else:
        st = "THIEU"
    isroot = origin.get(rel) == "ROOT"
    n = sum(1 for _ in open(lp, "rb")) if os.path.isfile(lp) else 0
    rows.append((st, rel, n, "" if isroot else "<- " + origin.get(rel, "?")))
cnt = collections.Counter(r[0] for r in rows)
print("TONG", len(rows), dict(cnt))
for st, rel, n, o in rows:
    print("%-12s %-58s %5d  %s" % (st, rel, n, o))
