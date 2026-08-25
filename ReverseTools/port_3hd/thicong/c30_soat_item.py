# -*- coding: utf-8 -*-
"""C30 - SOAT ITEM cua 3 hoat dong: dung HINH ANH va dung CHUC NANG chua?
Voi moi bo (6,1,N) xuat hien trong script da port, kiem 4 dieu:
  1) SERVER settings\\item\\magicscript.txt co dong particular = N khong
  2) CLIENT co dong do khong (thieu -> O DO/TRANG trong tui)
  3) tep .spr trong cot ImageName co that trong pak client khong (thieu -> khong co hinh)
  4) cot Script (neu khac "0") tro toi tep .lua CO THAT khong
     -> day la loi chu game gap: "Sat Thu Bi Bao kich khong dung duoc" vi tro
        \\script\\item\\satthubibao.lua (ban Viet) - tep KHONG TON TAI.
"""
import io, os, re, struct, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
OUT = r"D:\GAMEDEVNEW\ReverseTools\port_3hd\audit\C30_soat_item.md"
BS = chr(92)

THUMUC = [
    r"script\task\tollgate\killer",
    r"script\missions\fengling_ferry",
    r"script\missions\challengeoftime",
    r"script\vng_feature\challengeoftime",
]
TEPLE = [r"script\task\tollgate\killbosshead.lua",
         r"script\tinhnang\3hoatdong\hd3_driver.lua"]


def liet_ke():
    ra = []
    for t in THUMUC:
        d = os.path.join(JX1, t)
        if not os.path.isdir(d):
            continue
        for dp, dn, fs in os.walk(d):
            for f in fs:
                if f.endswith(".lua"):
                    ra.append(os.path.relpath(os.path.join(dp, f), JX1))
    ra += [t for t in TEPLE if os.path.exists(os.path.join(JX1, t))]
    return ra


# --- gom moi bo 6,1,N trong script ---
dung = {}
for rel in liet_ke():
    txt = io.open(os.path.join(JX1, rel), encoding="latin-1", newline="").read()
    for line in txt.split("\n"):
        s = line.lstrip()
        if s.startswith("--"):
            continue
        for m in re.finditer(r"\b6\s*,\s*1\s*,\s*(\d{2,5})\b", s):
            dung.setdefault(int(m.group(1)), set()).add(rel)
print("So bo (6,1,N) khac nhau dung trong script 3 hoat dong:", len(dung))


def bang(p):
    d = io.open(p, encoding="latin-1", newline="").read().split("\n")
    ra = {}
    for l in d[1:]:
        c = l.split("\t")
        if len(c) > 9 and c[1] == "6" and c[2] == "1" and c[3].isdigit():
            ra[int(c[3])] = c
    return ra


sv = bang(os.path.join(JX1, "settings", "item", "magicscript.txt"))
cl = bang(os.path.join(CLI, "settings", "item", "magicscript.txt"))
print("magicscript: server %d dong 6/1, client %d dong" % (len(sv), len(cl)))


def name2id(s):
    uid = 0
    idx = 0
    for ch in s:
        c = ord(ch)
        if 65 <= c <= 90:
            c += 32
        elif c > 127:
            c -= 256
        idx += 1
        uid = (((uid + idx * c) & 0xFFFFFFFF) % 0x8000000B) * 0xFFFFFFEF
        uid &= 0xFFFFFFFF
    return uid ^ 0x12345678


paks = {}
for f in os.listdir(os.path.join(CLI, "data")):
    if not f.lower().endswith(".pak"):
        continue
    try:
        b = open(os.path.join(CLI, "data", f), "rb").read()
        sig, cnt, ioff, doff = struct.unpack("<IIII", b[:16])
        s = set()
        for i in range(cnt):
            o = ioff + i * 16
            uid, off, size, cf = struct.unpack("<IIiI", b[o:o + 16])
            s.add(uid)
        paks[f] = s
    except Exception:
        pass

loi = []
for n in sorted(dung):
    r = sv.get(n)
    if not r:
        loi.append((n, "KHONG CO dong tren SERVER", ""))
        continue
    ten = r[0]
    if n not in cl:
        loi.append((n, "THIEU dong tren CLIENT (o do/trang trong tui)", ten))
    spr = r[4]
    if spr and spr not in ("0", ""):
        uid = name2id(spr if spr.startswith(BS) else BS + spr)
        if not any(uid in s for s in paks.values()):
            loi.append((n, "THIEU anh trong pak: %s" % spr, ten))
    sc = r[9] if len(r) > 9 else "0"
    if sc and sc not in ("0", ""):
        pt = os.path.join(JX1, sc.lstrip(BS))
        if not os.path.exists(pt):
            loi.append((n, "SCRIPT KHONG TON TAI: %s" % sc, ten))

print("\n=== LOI TIM DUOC: %d ===" % len(loi))
for (n, v, ten) in loi:
    print("  6,1,%-5d %-46s %s" % (n, v, ten[:26]))

os.makedirs(os.path.dirname(OUT), exist_ok=True)
with io.open(OUT, "w", encoding="utf-8") as f:
    f.write("# C30 - Soat item 3 hoat dong (hinh anh + chuc nang)\n\n")
    f.write("- So bo (6,1,N) dung trong script: **%d**\n- Loi: **%d**\n\n" % (len(dung), len(loi)))
    for (n, v, ten) in loi:
        f.write("- `6,1,%d` %s — %s\n" % (n, v, ten))
print("\nda ghi", OUT)
