# -*- coding: utf-8 -*-
import io, os, re, sys, json
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\port_3hd")
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
from dec2 import decline2
LNXI = r"D:\ServerLinux\Patch\settings\item\004\magicscript.txt"
JX1I = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item\magicscript.txt"
LNX = r"D:\ServerLinux\server1"
HERE = r"D:\GAMEDEVNEW\ReverseTools\port_3hd\thicong"
def load(path):
    d = open(path, "rb").read().split(b"\n")
    hdr = decline2(d[0].rstrip(b"\r")).split("\t")
    iG = hdr.index("Genre") if "Genre" in hdr else hdr.index("ItemGenre")
    iD = hdr.index("DetailType"); iP = hdr.index("ParticularType")
    out = {}
    for ln in d[1:]:
        c = decline2(ln.rstrip(b"\r")).split("\t")
        if len(c) <= max(iG, iD, iP): continue
        try: k = (int(c[iG]), int(c[iD]), int(c[iP]))
        except ValueError: continue
        out[k] = c[0].strip()
    return out
lnx = load(LNXI); jx1 = load(JX1I)
print("JX1 co bao nhieu muc genre6 detail0:", sum(1 for k in jx1 if k[0] == 6 and k[1] == 0))
print("Linux co bao nhieu muc genre6 detail0:", sum(1 for k in lnx if k[0] == 6 and k[1] == 0))
remap = json.load(io.open(os.path.join(HERE, "remap_resolved.json"), encoding="utf-8"))
man = [x for x in io.open(os.path.join(HERE, "b1_manifest.txt"), encoding="utf-8").read().split("\n") if x.strip()]
TRIP = re.compile(r"(?<![0-9])(\d{1,2})(\s*,\s*)(\d{1,2})(\s*,\s*)(\d{1,5})(?![0-9])")
PROD = re.compile(r"AddItem|DropItem|tbProp|GiveAward|DropRateItem|AddEquip", re.I)
CONS = re.compile(r"ConsumeItem|CalcItemCount|GetItemCount|CalcEquiproomItemCount|ConsumeEquiproomItem|CheckItemInBag|GetItemProp", re.I)
FORB = re.compile(r"ForbidItem|tbForbidInMap", re.I)
def norm(s): return re.sub(r"\s+", " ", s.strip().lower())
cats = {"A_ton_tai_ten_khac": {}, "B_khong_ton_tai": {}}
for rel in man:
    if not rel.lower().endswith(".lua"): continue
    p = os.path.join(LNX, rel.replace("/", os.sep))
    if not os.path.isfile(p): continue
    d = open(p, "rb").read().decode("latin-1")
    for i, ln in enumerate(d.split("\n"), 1):
        if ln.strip().startswith("--"): continue
        if not (PROD.search(ln) or CONS.search(ln) or FORB.search(ln) or re.search(r'\["\d+,\d+,\d+"\]', ln)): continue
        role = "SX" if PROD.search(ln) else ("TT" if CONS.search(ln) else "CAM")
        for m in TRIP.finditer(ln):
            k = (int(m.group(1)), int(m.group(3)), int(m.group(5)))
            if k[0] != 6 or k not in lnx: continue
            s = "%d,%d,%d" % k
            if s in remap: continue
            jn = jx1.get(k)
            if jn is None:
                cats["B_khong_ton_tai"].setdefault(s, []).append((rel, i, role, lnx[k], decline2(ln.strip().encode("latin-1"))[:88]))
            elif norm(jn) != norm(lnx[k]):
                cats["A_ton_tai_ten_khac"].setdefault(s, []).append((rel, i, role, lnx[k] + " -> JX1:" + jn, decline2(ln.strip().encode("latin-1"))[:88]))
for cat in cats:
    print("\n########", cat, len(cats[cat]), "bo")
    for s in sorted(cats[cat], key=lambda x: tuple(map(int, x.split(",")))):
        roles = set(r for _, _, r, _, _ in cats[cat][s])
        print("  %-12s [%s] %s" % (s, ",".join(sorted(roles)), cats[cat][s][0][3][:70]))
        for rel, i, r, _, t in cats[cat][s]:
            print("        %s:%d (%s) %s" % (rel, i, r, t))
