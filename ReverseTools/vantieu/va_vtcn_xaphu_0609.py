# -*- coding: utf-8 -*-
"""patch_xaphu.py [VTCN 06/09 chu] - Xa Phu du thanh/thon:
 1. script/global/station.lua: AddStation 1..15 -> 1..getn(STATION_ARRAY) (16, them Tran Thach Co).
 2. settings/StationPrice.txt: ma tran gia thua (-1 = khong tuyen) -> DAY DU moi cap ben, gia = duong ngan nhat
    (Floyd-Warshall) tren cac tuyen co san; cheo van -1. Sao luu .truoc_vtcn_xaphu_0609. Byte-safe (latin-1).
Chay: python patch_xaphu.py [--dry]
"""
import io, os, shutil, sys
sys.stdout.reconfigure(encoding="utf-8")
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
DRY = "--dry" in sys.argv
BAK = ".truoc_vtcn_xaphu_0609"

def save(p, s):
    if DRY:
        return
    if not os.path.isfile(p + BAK):
        shutil.copyfile(p, p + BAK)
    io.open(p, "w", encoding="latin-1", newline="").write(s)

# 1. station.lua
p = os.path.join(SRV, r"script\global\station.lua")
s = io.open(p, "r", encoding="latin-1", newline="").read()
old = "\tfor j = 1, 15 do\r\n\t\tAddStation(j)\r\n"
if s.count(old) != 1:
    old = old.replace("\r\n", "\n")
    if s.count(old) != 1:
        sys.exit("station.lua: khong thay neo 'for j = 1, 15'")
eol = "\r\n" if "\r\n" in old else "\n"
new = ("\tfor j = 1, getn(STATION_ARRAY) do\t-- [VTCN 06/09] chu: du 16 ben Station.txt (truoc 1..15, sot Tran Thach Co)" + eol +
       "\t\tAddStation(j)" + eol)
hb = sum(1 for c in s if ord(c) >= 0x80)
s = s.replace(old, new)
assert sum(1 for c in s if ord(c) >= 0x80) == hb
save(p, s)
print("station.lua OK")

# 2. StationPrice.txt
p = os.path.join(SRV, r"settings\StationPrice.txt")
raw = io.open(p, "r", encoding="latin-1", newline="").read()
eol = "\r\n" if "\r\n" in raw else "\n"
lines = raw.split(eol)
hdr = lines[0].split("\t")
rows = [l for l in lines[1:] if l.strip() != ""]
n = len(rows)
names = [r.split("\t")[0] for r in rows]
if len(hdr) - 1 != n:
    sys.exit("StationPrice: header %d cot ten, %d hang" % (len(hdr) - 1, n))
INF = 10 ** 6
d = [[INF] * n for _ in range(n)]
for i, r in enumerate(rows):
    c = r.split("\t")
    if len(c) - 1 < n:
        sys.exit("hang %d thieu cot: %d" % (i + 2, len(c) - 1))
    for j in range(n):
        v = int(c[1 + j].strip() or "-1")
        if i == j:
            d[i][j] = 0
        elif v > 0:
            d[i][j] = v
# Floyd-Warshall
for k in range(n):
    for i in range(n):
        if d[i][k] >= INF:
            continue
        for j in range(n):
            if d[i][k] + d[k][j] < d[i][j]:
                d[i][j] = d[i][k] + d[k][j]
them = 0
out = [lines[0]]
for i, r in enumerate(rows):
    c = r.split("\t")
    for j in range(n):
        if i == j:
            c[1 + j] = "-1"
            continue
        v = int(c[1 + j].strip() or "-1")
        if v <= 0:
            if d[i][j] >= INF:
                sys.exit("khong noi duoc %d -> %d" % (i + 1, j + 1))
            c[1 + j] = str(d[i][j]); them += 1
    out.append("\t".join(c[:1 + n]) + ("\t" + "\t".join(c[1 + n:]) if len(c) > 1 + n and any(x.strip() for x in c[1 + n:]) else ""))
out.append("")
s2 = eol.join(out)
assert sum(1 for ch in s2 if ord(ch) >= 0x80) == sum(1 for ch in raw if ord(ch) >= 0x80)
save(p, s2)
print("StationPrice.txt: %d ben, dien them %d o gia; gia lon nhat %d" % (n, them, max(max(x for x in row if x < INF) for row in d)))
for i in range(n):
    print("  %-18s" % names[i].strip()[:18], " ".join("%3d" % (d[i][j] if i != j else -1) for j in range(n)))
