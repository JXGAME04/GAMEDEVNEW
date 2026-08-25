# -*- coding: utf-8 -*-
"""Liet ke MOI cho da bi doi (so sanh ban Linux goc <-> ban da trien khai o JX1),
va phat hien va CHUOI (chain) khi dich cua luat nay lai la nguon cua luat khac."""
import io, os, re, sys, json
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\port_3hd")
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
from dec2 import decline2

LNX_A = r"D:\ServerLinux\server1"
JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
HERE = r"D:\GAMEDEVNEW\ReverseTools\port_3hd\thicong"
remap = json.load(io.open(os.path.join(HERE, "remap_resolved.json"), encoding="utf-8"))
manifest = io.open(os.path.join(HERE, "b1_manifest.txt"), encoding="utf-8").read().split("\n")

srcs = set(remap.keys())
dsts = set(remap.values())
print("== CHUOI (dich cua luat A == nguon cua luat B) ==")
keys = list(remap.keys())
for i, a in enumerate(keys):
    if remap[a] in srcs:
        b = remap[a]
        j = keys.index(b)
        print("  %s -> %s  ; luat %s -> %s  (thu tu ap: A=%d B=%d) => %s" %
              (a, remap[a], b, remap[b], i, j, "VA CHONG (A truoc B)" if i < j else "an toan (B truoc A)"))
print()

def apply_seq(d):
    for s, t in remap.items():
        g, dd, pt = s.split(",")
        pat = re.compile(r"(?<![0-9])%s(\s*,\s*)%s(\s*,\s*)%s(?![0-9])" % (g, dd, pt))
        nv = t.split(",")
        d = pat.sub(lambda m, nv=nv: "%s%s%s%s%s" % (nv[0], m.group(1), nv[1], m.group(2), nv[2]), d)
    return d

TOK = re.compile(r"(?<![0-9])(\d+)(\s*,\s*)(\d+)(\s*,\s*)(\d+)(?![0-9])")
def apply_sim(d):
    def rep(m):
        k = "%s,%s,%s" % (m.group(1), m.group(3), m.group(5))
        if k in remap:
            nv = remap[k].split(",")
            return "%s%s%s%s%s" % (nv[0], m.group(2), nv[1], m.group(4), nv[2])
        return m.group(0)
    return TOK.sub(rep, d)

print("== DIFF: tuan tu (da trien khai) vs dong thoi (dung) ==")
bad = 0
for rel in manifest:
    if not rel.lower().endswith(".lua"):
        continue
    src = os.path.join(LNX_A, rel.replace("/", os.sep))
    if not os.path.isfile(src):
        continue
    d0 = open(src, "rb").read().decode("latin-1")
    a = apply_seq(d0); b = apply_sim(d0)
    if a != b:
        bad += 1
        for i, (l0, la, lb) in enumerate(zip(d0.split("\n"), a.split("\n"), b.split("\n")), 1):
            if la != lb:
                print("  !! %s:%d" % (rel, i))
                print("     GOC : %s" % decline2(l0.strip().encode("latin-1"))[:110])
                print("     DA VA: %s" % decline2(la.strip().encode("latin-1"))[:110])
                print("     DUNG : %s" % decline2(lb.strip().encode("latin-1"))[:110])
print("  so tep hong do va chuoi:", bad)
print()

print("== KIEM: ban trien khai co dung ban tuan tu khong (xac nhan da chay) ==")
for rel in manifest:
    if not rel.lower().endswith(".lua"):
        continue
    src = os.path.join(LNX_A, rel.replace("/", os.sep))
    dst = os.path.join(JX1, rel.replace("/", os.sep))
    if not (os.path.isfile(src) and os.path.isfile(dst)):
        print("  MAT:", rel, os.path.isfile(src), os.path.isfile(dst))
