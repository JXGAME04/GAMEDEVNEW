# -*- coding: utf-8 -*-
# So bo cuc do duoc tren may voi ban PC (khung 1040x604): moi cua so phai giu DUNG khoang cach toi mep
# (trai / phai / tren / duoi) hoac dung ti le tam - tuy cach nao khop; bao cac cua so lech > 2 px.
import io
import os
import re
import sys

T = os.environ["TEMP"]
MAU = re.compile(r"(\S+) = abs (-?\d+),(-?\d+) rel (-?\d+),(-?\d+) (\d+)x(\d+)(.*)")
BO_QUA = ("|ShorcutKeyBar", "|OpenChannelBtn", "|SwitchSizeBtn", "|InputBack", "|ChatRoom")


def doc(f):
    d = {}
    W = H = 0
    for ln in io.open(os.path.join(T, f), encoding="latin-1"):
        ln = ln.strip()
        m = re.match(r"khung ve (\d+)x(\d+)", ln)
        if m:
            W, H = int(m.group(1)), int(m.group(2))
            continue
        m = MAU.match(ln)
        if not m:
            continue
        k = m.group(1)
        ax, ay, rx, ry, w, h = [int(v) for v in m.groups()[1:7]]
        if k in d or w <= 0 or h <= 0 or ax < -100:
            continue
        if any(b in k for b in BO_QUA):
            continue
        d[k] = (ax, ay, w, h, m.group(8).strip() == "an")
    return W, H, d


def lech(pc_pos, pc_co, pc_khung, pos, co, khung):
    """khoang lech nho nhat trong 3 cach neo: mep dau, giua, mep cuoi"""
    return min(abs(pos - pc_pos),
               abs((pos + co / 2.0 - khung / 2.0) - (pc_pos + pc_co / 2.0 - pc_khung / 2.0)),
               abs((khung - pos - co) - (pc_khung - pc_pos - pc_co)))


PW, PH, pc = doc(sys.argv[1] if len(sys.argv) > 1 else "bocuc_r_1040x604.txt")
print("ban PC (khung %dx%d): %d cua so" % (PW, PH, len(pc)))
for ten, f in [(a.split("=")[0], a.split("=")[1]) for a in sys.argv[2:]]:
    W, H, d = doc(f)
    xau = []
    for k, (ax, ay, w, h, an) in sorted(d.items()):
        if k not in pc or an or pc[k][4]:
            continue
        px, py, pw, ph, _ = pc[k]
        lx = lech(px, pw, PW, ax, w, W)
        ly = lech(py, ph, PH, ay, h, H)
        if lx > 2 or ly > 2:
            xau.append((round(max(lx, ly)), round(lx), round(ly), k, (px, py), (ax, ay)))
    xau.sort(reverse=True)
    print("\n=== %s (%dx%d): %d/%d cua so lech > 2 px ===" % (ten, W, H, len(xau), len(d)))
    for _, lx, ly, k, p, a in xau[:30]:
        print("   %-40s PC %4d,%-4d -> %4d,%-4d  lech %3d,%3d" % (k, p[0], p[1], a[0], a[1], lx, ly))
