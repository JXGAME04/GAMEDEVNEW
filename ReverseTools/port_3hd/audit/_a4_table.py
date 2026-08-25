# -*- coding: utf-8 -*-
"""Sinh bang markdown 42 bo cho bao cao A4."""
import io, os, re, sys, json
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\port_3hd")
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
from dec2 import decline2

LNX = r"D:\ServerLinux\server1"
JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
HERE = r"D:\GAMEDEVNEW\ReverseTools\port_3hd\thicong"

def load(path, scol):
    d = open(path, "rb").read().split(b"\n")
    hdr = decline2(d[0].rstrip(b"\r")).split("\t")
    iG = hdr.index("Genre") if "Genre" in hdr else hdr.index("ItemGenre")
    iD = hdr.index("DetailType"); iP = hdr.index("ParticularType")
    out = {}
    for ln in d[1:]:
        c = decline2(ln.rstrip(b"\r")).split("\t")
        if len(c) <= max(iG, iD, iP, scol):
            continue
        try:
            k = (int(c[iG]), int(c[iD]), int(c[iP]))
        except ValueError:
            continue
        out[k] = (c[0].strip(), c[scol].strip())
    return out

L = load(r"D:\ServerLinux\Patch\settings\item\004\magicscript.txt", 13)
J = load(os.path.join(JX1, "settings", "item", "magicscript.txt"), 9)
remap = json.load(io.open(os.path.join(HERE, "remap_resolved.json"), encoding="utf-8"))
man = [x for x in io.open(os.path.join(HERE, "b1_manifest.txt"), encoding="utf-8").read().split("\n") if x.strip()]

PROD = re.compile(r"AddItem|DropItem|tbProp|GiveAward", re.I)
CONS = re.compile(r"ConsumeItem|CalcItemCount|GetItemCount|CalcEquiproomItemCount|ConsumeEquiproomItem|CheckItemInBag|GetItemProp", re.I)
FORB = re.compile(r"ForbidIn")
sites = {s: {"SX": [], "TT": [], "CAM": []} for s in remap}
for rel in man:
    if not rel.lower().endswith(".lua"):
        continue
    p = os.path.join(LNX, rel.replace("/", os.sep))
    if not os.path.isfile(p):
        continue
    d = open(p, "rb").read().decode("latin-1")
    for i, ln in enumerate(d.split("\n"), 1):
        for s in remap:
            g, dd, pt = s.split(",")
            if re.search(r"(?<![0-9])%s(\s*,\s*)%s(\s*,\s*)%s(?![0-9])" % (g, dd, pt), ln):
                role = "CAM" if FORB.search(ln) else ("TT" if (CONS.search(ln) and not PROD.search(ln)) else "SX")
                sites[s][role].append("%s:%d" % (os.path.basename(rel), i))
# bare forms da va tay
sites["6,1,399"]["TT"].append("nieshichen.lua:168(so tran)")
sites["6,1,2015"]["TT"].append("shuizei.lua:74(so tran)")

BS = chr(92)
CHAIN = {"6,1,2006": "6,1,2024", "6,1,2117": "6,1,2135"}
rows = []
for s in sorted(remap, key=lambda x: int(x.split(",")[2])):
    dd = remap[s]
    ks = tuple(map(int, s.split(","))); kd = tuple(map(int, dd.split(",")))
    ln, lsc = L.get(ks, ("?", "")); jn, jsc = J.get(kd, ("?", ""))
    sx = ", ".join(sites[s]["SX"]) or "-"
    tt = ", ".join(sites[s]["TT"]) or "-"
    cam = len(sites[s]["CAM"])
    kl = []
    if s in CHAIN:
        kl.append("**SAI - va chuoi -> %s**" % CHAIN[s])
    if not tt or tt == "-":
        kl.append("chi SX")
    if lsc and lsc not in ("0", "") and "noscript" not in lsc.lower():
        if not jsc or jsc == "0":
            kl.append("JX1 KHONG co script dung -> vat pham CHET")
        else:
            relj = jsc.replace("/", BS).lstrip(BS)
            if not os.path.isfile(os.path.join(JX1, relj)):
                kl.append("JX1 tro toi `%s` NHUNG TEP KHONG TON TAI" % jsc)
            elif os.path.basename(relj).lower() != os.path.basename(lsc.replace("/", BS)).lower():
                kl.append("JX1 dung script RIENG `%s` (ban port bi bo qua)" % os.path.basename(relj))
    if not kl:
        kl.append("OK")
    rows.append((s, ln, dd, jn, sx, tt, cam, "; ".join(kl)))

print("| # | Nguon (Linux) | Ten | Dich (JX1) | Ten JX1 | Producer | Consumer | Cam | Ket luan |")
print("|---|---|---|---|---|---|---|---|---|")
for i, r in enumerate(rows, 1):
    print("| %d | `%s` | %s | `%s` | %s | %s | %s | %d | %s |" %
          (i, r[0], r[1], r[2], r[3], r[4][:78], r[5][:60], r[6], r[7]))
