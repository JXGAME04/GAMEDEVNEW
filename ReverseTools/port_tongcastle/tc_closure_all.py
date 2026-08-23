import io, os, re, sys, filecmp
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BS = chr(92)
LNX = r"D:\ServerLinux\server1\script"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
SRV2 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\scriptjx2\tong_vn"
ROOTS = ["missions/tongcastle"]
ROOTFILES = ["global/achievementsys/type/tongcastle.lua"]
SKIP = set()

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
    # chi de quy vao tep DU AN CHUA CO (tep da co -> luc chay Include lay ban du an)
    if os.path.isfile(os.path.join(SRV, cur)) or (cur.lower().startswith("tong/") and os.path.isfile(os.path.join(SRV2, cur[5:]))): continue
    for inc in includes_of(cur):
        if inc.lower() not in seen:
            origin.setdefault(inc, cur)
            stack.append(inc)

rows = []
for key, rel in sorted(seen.items()):
    lp = os.path.join(LNX, rel)
    sp = os.path.join(SRV, rel)
    sp2 = os.path.join(SRV2, rel[5:]) if rel.lower().startswith("tong/") else None
    if not os.path.isfile(lp):
        st = "LINUX-KHONG"
    elif os.path.isfile(sp):
        st = "IDENT" if filecmp.cmp(lp, sp, shallow=False) else "DIFF"
    elif sp2 and os.path.isfile(sp2):
        st = "IDENT(tong_vn)" if filecmp.cmp(lp, sp2, shallow=False) else "DIFF(tong_vn)"
    else:
        st = "THIEU"
    isroot = origin.get(rel) == "ROOT"
    rows.append((st, rel, "" if isroot else "<- " + origin.get(rel, "?")))
import collections
cnt = collections.Counter(r[0] for r in rows)
print("TONG", len(rows), dict(cnt))
for st, rel, o in rows:
    if True:
        print("%-14s %-70s %s" % (st, rel, o))
if len(sys.argv) > 1 and sys.argv[1] == "--list-missing":
    io.open(os.path.join(os.path.dirname(__file__), "tinsu_missing.txt"), "w").write(
        "\n".join(rel for st, rel, o in rows if st == "THIEU"))
