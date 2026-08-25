# -*- coding: utf-8 -*-
"""C3 - MO RONG anh xa vat pham (A7-N1): quet TOAN BO bo ba (g,d,p) trong cac tep
HD3, doi chieu TEN vat pham giua bang Linux va bang JX1:
  - neu ten JX1 tai cung bo so KHAC ten Linux, VA ton tai bo so JX1 CUNG TEN
    (uu tien cung genre+detail) -> them vao bang anh xa dot 2.
  - ap dung ca dang bo ba '6,1,N' lan dang so sanh tran 'bien ==/~= N'.
Chi cham cac tep trong b1_manifest + 4 tep moi (khong dung tep JX1 cu).
Ket qua: remap2_applied.json + bao cao ra man hinh.
"""
import io, os, re, json, sys, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\port_3hd")
from dec2 import decline2

HERE = os.path.dirname(os.path.abspath(__file__))
JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
LNX_ITEM = r"D:\ServerLinux\Patch\settings\item\004\magicscript.txt"
JX1_ITEM = os.path.join(JX1, "settings", "item", "magicscript.txt")


def load_items(path):
    d = open(path, "rb").read().split(b"\n")
    hdr = decline2(d[0].rstrip(b"\r")).split("\t")
    iG = hdr.index("Genre") if "Genre" in hdr else hdr.index("ItemGenre")
    iD = hdr.index("DetailType")
    iP = hdr.index("ParticularType")
    out = {}
    for ln in d[1:]:
        c = decline2(ln.rstrip(b"\r")).split("\t")
        if len(c) <= max(iG, iD, iP):
            continue
        try:
            out[(int(c[iG]), int(c[iD]), int(c[iP]))] = c[0].strip()
        except ValueError:
            continue
    return out


def norm(s):
    return re.sub(r"\s+", " ", s.strip().lower())


lnx = load_items(LNX_ITEM)
jx1 = load_items(JX1_ITEM)
jx1_by_name = collections.defaultdict(list)
for k, v in jx1.items():
    jx1_by_name[norm(v)].append(k)

# --- gom bo ba tu tep HD3 ---
files = [x.strip().replace("/", os.sep) for x in
         io.open(os.path.join(HERE, "b1_manifest.txt"), encoding="utf-8").read().split("\n")
         if x.strip().lower().endswith(".lua")]
files += [r"script\item\hd3_admin.lua", r"script\tinhnang\3hoatdong\hd3_driver.lua"]

RX_TRIPLE = re.compile(r"(?<![0-9])([0-9])\s*,\s*([0-9])\s*,\s*([0-9]{2,5})(?![0-9])")
used = collections.Counter()
for rel in sorted(set(files)):
    p = os.path.join(JX1, rel)
    if not os.path.isfile(p):
        continue
    d = open(p, "rb").read().decode("latin-1")
    for m in RX_TRIPLE.finditer(d):
        g, dd, pt = int(m.group(1)), int(m.group(2)), int(m.group(3))
        if g in (0, 1, 2, 4, 5, 6) and dd <= 30:
            used[(g, dd, pt)] += 1

# --- da anh xa dot 1 ---
prev = json.load(io.open(os.path.join(HERE, "remap_resolved.json"), encoding="utf-8"))
prev_src = set(tuple(map(int, k.split(","))) for k in prev)
prev_dst = set(tuple(map(int, v.split(","))) for v in prev.values())
prev_dst.add((6, 1, 3363)); prev_dst.add((6, 1, 195))

remap2 = {}
report = []
for key, cnt in sorted(used.items()):
    if key in prev_src or key in prev_dst:
        continue                      # da xu ly / la dich dot 1
    ln_name = lnx.get(key)
    if not ln_name:
        continue                      # khong phai vat pham ban Linux
    jx_name = jx1.get(key, "")
    if norm(jx_name) == norm(ln_name):
        continue                      # cung ten -> khong can doi
    cands = jx1_by_name.get(norm(ln_name), [])
    # uu tien cung genre+detail
    same_gd = [c for c in cands if c[0] == key[0] and c[1] == key[1]]
    pick = same_gd[0] if same_gd else None
    if pick:
        remap2["%d,%d,%d" % key] = "%d,%d,%d" % pick
        report.append((key, ln_name, jx_name, pick, cnt))
    else:
        report.append((key, ln_name, jx_name, None, cnt))

print("Bo ba dung trong tep HD3:", len(used))
print("Anh xa DOT 2 tim duoc:", len(remap2))
print()
print("| Nguon | Ten Linux | JX1 cung so dang la | -> Dich | So cho |")
print("|---|---|---|---|---|")
for key, ln_name, jx_name, pick, cnt in report:
    print("| %d,%d,%d | %s | %s | %s | %d |" % (key[0], key[1], key[2], ln_name[:26],
          (jx_name or "(KHONG CO)")[:26], ("%d,%d,%d" % pick) if pick else "**KHONG TIM DUOC**", cnt))

json.dump(remap2, io.open(os.path.join(HERE, "remap2_applied.json"), "w", encoding="utf-8"),
          ensure_ascii=False, indent=1)

if "--apply" not in sys.argv:
    print()
    print("(xem truoc - them --apply de ap)")
    raise SystemExit

# --- ap MOT LUOT (single-pass): chuoi lech-1-bac (116->115, 115->114...) ap tuan
# tu se truot day chuyen (gia tri vua thay bi thay tiep). Mot regex bat MOI bo ba,
# callback tra map dong thoi. So sanh tran cung mot luot theo particular (6,1,*).
map_triple = {}
map_bare = {}
for src, dst in remap2.items():
    sg, sd, sp = src.split(",")
    map_triple[(sg, sd, sp)] = dst.split(",")
    if (sg, sd) == ("6", "1"):
        map_bare[sp] = dst.split(",")[2]

RX_ANY = re.compile(r"(?<![0-9])([0-9])(\s*,\s*)([0-9])(\s*,\s*)([0-9]{1,5})(?![0-9])")
VARS = r"(?:particular|parttype|nParticular|nPart|np|particulartype|nParticularType|detailtype)"
RX_BARE = re.compile(r"(\b%s\s*[~=]=\s*)([0-9]{2,5})(?![0-9])" % VARS)


def sub_triple(m):
    key = (m.group(1), m.group(3), m.group(5))
    if key in map_triple:
        a, b, c = map_triple[key]
        return "%s%s%s%s%s" % (a, m.group(2), b, m.group(4), c)
    return m.group(0)


def sub_bare(m):
    if m.group(2) in map_bare:
        return m.group(1) + map_bare[m.group(2)]
    return m.group(0)


tot_f = 0
for rel in sorted(set(files)):
    p = os.path.join(JX1, rel)
    if not os.path.isfile(p):
        continue
    d = open(p, "rb").read().decode("latin-1")
    orig = d
    d = RX_ANY.sub(sub_triple, d)
    d = RX_BARE.sub(sub_bare, d)
    if d != orig:
        open(p, "wb").write(d.encode("latin-1"))
        m2 = os.path.join(MIRROR, rel)
        os.makedirs(os.path.dirname(m2), exist_ok=True)
        open(m2, "wb").write(d.encode("latin-1"))
        tot_f += 1
        print("  ap:", rel.rsplit(os.sep, 1)[-1])
print()
print("DA AP (mot luot) tren %d tep." % tot_f)
