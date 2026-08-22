import io, os, re, sys, filecmp, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BS = chr(92)
LNX = r"D:\ServerLinux\server1\script"
RELAY = r"D:\ServerLinux\gateway\s3relay\relaysetting\task"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
SRV2 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\scriptjx2\tong_vn"
SRV3 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\scriptjx2\lib"
ROOTS = ["missions/tongwar", "event/tongwar", "tongpkmessage"]
ROOTFILES = []
RELAYFILES = ["tongwar.lua"]
SKIP = set()

def norm(s):
    s = s.replace(BS, "/")
    while "//" in s: s = s.replace("//", "/")
    s = s.lstrip("/")
    if s.lower().startswith("script/"): s = s[7:]
    return s

# IL/IncludeLib("NAME") -> ban do thu vien cua Linux: script/lib/?  doc tu settings? -> lay tu ScriptFuns? Linux: IncludeLib tra cuu bang trong engine.
def includes_of(path):
    if not os.path.isfile(path): return [], []
    t = io.open(path, encoding="latin-1").read()
    out, libs = [], []
    for line in t.split("\n"):
        s = line.split("--")[0]
        for m in re.finditer(r'Include\s*\(\s*(?:"([^"]+)"|\[\[([^\]]+)\]\])', s):
            out.append(norm(m.group(1) or m.group(2)))
        for m in re.finditer(r'(?:IncludeLib|IL)\s*\(\s*"([^"]+)"', s):
            libs.append(m.group(1))
    return out, libs

seeds = list(ROOTFILES)
for r in ROOTS:
    for dp, _, fs in os.walk(os.path.join(LNX, r)):
        for f in fs:
            if f.lower().endswith(".lua"):
                seeds.append(os.path.relpath(os.path.join(dp, f), LNX).replace(BS, "/"))
seen = {}
libs_all = collections.OrderedDict()
stack = list(seeds)
origin = {s: "ROOT" for s in seeds}
# relay
for rf in RELAYFILES:
    incs, libs = includes_of(os.path.join(RELAY, rf))
    for inc in incs:
        origin.setdefault(inc, "RELAY:" + rf)
        stack.append(inc)
    for l in libs: libs_all.setdefault(l, []).append("RELAY:" + rf)
while stack:
    cur = stack.pop()
    key = cur.lower()
    if key in seen: continue
    seen[key] = cur
    if key in {x.lower() for x in SKIP}: continue
    incs, libs = includes_of(os.path.join(LNX, cur))
    for l in libs: libs_all.setdefault(l, []).append(cur)
    if os.path.isfile(os.path.join(SRV, cur)) or (cur.lower().startswith("tong/") and os.path.isfile(os.path.join(SRV2, cur[5:]))) or (cur.lower().startswith("lib/") and os.path.isfile(os.path.join(SRV3, cur[4:]))):
        continue
    for inc in incs:
        if inc.lower() not in seen:
            origin.setdefault(inc, cur)
            stack.append(inc)

rows = []
for key, rel in sorted(seen.items()):
    lp = os.path.join(LNX, rel)
    sp = os.path.join(SRV, rel)
    sp2 = os.path.join(SRV2, rel[5:]) if rel.lower().startswith("tong/") else None
    sp3 = os.path.join(SRV3, rel[4:]) if rel.lower().startswith("lib/") else None
    if not os.path.isfile(lp):
        st = "LINUX-KHONG"
    elif os.path.isfile(sp):
        st = "IDENT" if filecmp.cmp(lp, sp, shallow=False) else "DIFF"
    elif sp2 and os.path.isfile(sp2):
        st = "IDENT(tong_vn)" if filecmp.cmp(lp, sp2, shallow=False) else "DIFF(tong_vn)"
    elif sp3 and os.path.isfile(sp3):
        st = "IDENT(jx2lib)" if filecmp.cmp(lp, sp3, shallow=False) else "DIFF(jx2lib)"
    else:
        st = "THIEU"
    isroot = origin.get(rel) == "ROOT"
    rows.append((st, rel, "" if isroot else "<- " + origin.get(rel, "?")))
cnt = collections.Counter(r[0] for r in rows)
print("TONG", len(rows), dict(cnt))
for st, rel, o in rows:
    print("%-14s %-60s %s" % (st, rel, o))
print()
print("IncludeLib/IL dung:")
for l, who in libs_all.items():
    print("  %-12s <- %s" % (l, ", ".join(sorted(set(who))[:3])))
