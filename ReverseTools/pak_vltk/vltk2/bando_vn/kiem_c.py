# -*- coding: utf-8 -*-
"""Kiem 2 khang dinh ve loi (C) tren DU LIEU THAT:
 1) MapList.ini co bao nhieu _MapPos TRUNG NHAU (mot toa do dung cho nhieu map)?
 2) Bao nhieu dia diem nam DUOI 7 nhan bang hoi (KWndPureTextBtn) -> bi nuot chuot?
Chi doc, khong sua gi."""
import re, sys, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

LIVE = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
ml = open(LIVE + r"\settings\MapList.ini", "rb").read().decode("latin-1")
ui = open(LIVE + r"\Ui\Ui3\UiWorldMap.ini", "rb").read().decode("latin-1")

# ---- doc dia diem giong het KWorldMapLocs::Load ----
pos, name, mtype = {}, {}, {}
for m in re.finditer(r"^(\d+)_MapPos=(-?\d+)\s*,\s*(-?\d+)", ml, re.M):
    pos[int(m.group(1))] = (int(m.group(2)), int(m.group(3)))
for m in re.finditer(r"^(\d+)_name=([^\r\n]*)", ml, re.M):
    name[int(m.group(1))] = m.group(2).strip()
for m in re.finditer(r"^(\d+)_MapType=([^\r\n]*)", ml, re.M):
    mtype[int(m.group(1))] = m.group(2).strip()

locs = [(i, pos[i], name.get(i, ""), mtype.get(i, "")) for i in sorted(pos) if 1 <= i <= 1200 and name.get(i, "")]
print("Dia diem nap duoc (co _MapPos va _name): %d" % len(locs))
print("So map co _MapPos nhung THIEU _name (bi bo): %d" % len([i for i in pos if not name.get(i, "")]))

# ---- 1) trung toa do ----
by = collections.defaultdict(list)
for i, p, n, t in locs:
    by[p].append((i, n, t))
dup = {p: v for p, v in by.items() if len(v) > 1}
n_in_dup = sum(len(v) for v in dup.values())
print("\n1) TRUNG TOA DO")
print("   toa do KHAC NHAU: %d cho %d dia diem" % (len(by), len(locs)))
print("   toa do bi dung >1 lan: %d  -> %d dia diem (%.1f%%) khong bao gio bam trung dung map"
      % (len(dup), n_in_dup, 100.0 * n_in_dup / len(locs)))
print("   5 cho dong nhat:")
for p, v in sorted(dup.items(), key=lambda kv: -len(kv[1]))[:5]:
    print("     %s : %d map  vd: %s" % (p, len(v), ", ".join("%d %s" % (i, n) for i, n, t in v[:4])))

# ---- 2) nam duoi nhan bang hoi ----
CITY = ["fengxiang", "chengdu", "dali", "bianjing", "xiangyang", "yangzhou", "linan"]
btn = {}
for s in CITY:
    m = re.search(r"\[%s\][^\[]*?BtnStartPos=(\d+),(\d+)" % s, ui, re.S)
    btn[s] = (int(m.group(1)), int(m.group(2)))
W = int(re.search(r"\[PureTextBtn\][^\[]*?Width=(\d+)", ui, re.S).group(1))
H = int(re.search(r"\[PureTextBtn\][^\[]*?Height=(\d+)", ui, re.S).group(1))
print("\n2) NHAN BANG HOI NUOT CHUOT   (KWndPureTextBtn %dx%d)" % (W, H))
under = []
for i, (px, py) in ((i, p) for i, p, n, t in locs):
    cx, cy = px + 30, py + 17          # tam dia diem theo ma hien tai
    for s, (bx, byy) in btn.items():
        if bx <= cx <= bx + W and byy <= cy <= byy + H:
            under.append((i, name.get(i, ""), mtype.get(i, ""), s))
            break
print("   dia diem nam TRONG khung nhan: %d" % len(under))
for i, n, t, s in under[:24]:
    print("     map %-5d %-22s %-12s duoi nhan %s" % (i, n, t, s))

# ---- 3) 7 thanh lon co bam duoc khong ----
print("\n3) 7 THANH LON")
CITYMAP = {1: "fengxiang", 11: "chengdu", 162: "dali", 37: "bianjing", 78: "xiangyang", 80: "yangzhou", 176: "linan"}
for mid, s in CITYMAP.items():
    p = pos.get(mid)
    if not p:
        print("   map %-4d %-14s KHONG co _MapPos" % (mid, s)); continue
    cx, cy = p[0] + 30, p[1] + 17
    bx, byy = btn[s]
    hid = bx <= cx <= bx + W and byy <= cy <= byy + H
    shared = len(by[p])
    print("   map %-4d %-12s %-16s MapPos%s tam(%d,%d) %s | dung chung toa do voi %d map"
          % (mid, s, name.get(mid, "")[:16], p, cx, cy, "BI NHAN CHE" if hid else "khong bi che", shared))
