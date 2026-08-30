# -*- coding: utf-8 -*-
"""Kiem TAT CA tep settings\\droprate\\**\\*.ini: tong RandRate va so lan roll.

Muc dich: tu kiem chung can cu cua ban va chong treo (t03/L1) thay vi tin bao
cao cua nguoi khac. Neu tong RandRate = 0 ma Count > 0 => vong while trong
KNpc::DropRateItem khong bao gio thoat => TREO GAMESERVER.
"""
import io
import os
import re
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
G = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\droprate"

xau = []
ds = []
for goc, _, tep in os.walk(G):
    for t in tep:
        if not t.lower().endswith(".ini"):
            continue
        p = os.path.join(goc, t)
        d = io.open(p, "rb").read().decode("latin-1").replace("\r\n", "\n")
        m = re.search(r"^\s*Count\s*=\s*(\d+)", d, re.M | re.I)
        cnt = int(m.group(1)) if m else 0
        m = re.search(r"^\s*RandRange\s*=\s*(\d+)", d, re.M | re.I)
        rng = int(m.group(1)) if m else 0
        rates = [int(x) for x in re.findall(r"^\s*RandRate\s*=\s*(\d+)", d, re.M | re.I)]
        tong = sum(rates)
        rel = os.path.relpath(p, G)
        ds.append((rel, cnt, rng, tong, len(rates)))
        if cnt > 0 and tong <= 0:
            xau.append((rel, cnt, rng, tong, len(rates)))

print("Tong %d tep .ini" % len(ds))
print()
print("=== TEP NGUY HIEM (Count>0 nhung tong RandRate=0 => TREO) ===")
if not xau:
    print("  (khong co)")
for rel, cnt, rng, tong, n in xau:
    print("  %-42s Count=%-4d RandRange=%-9d tong=%d  so muc=%d"
          % (rel, cnt, rng, tong, n))

print()
print("=== SO LAN ROLL TRUNG BINH cho MOI mon (RandRange / tong RandRate) ===")
xep = []
for rel, cnt, rng, tong, n in ds:
    if tong > 0 and rng > 0:
        xep.append((rng / float(tong), rel, rng, tong))
xep.sort(reverse=True)
for r, rel, rng, tong in xep[:8]:
    print("  %-42s %8.1f lan  (RandRange=%d / tong=%d)" % (rel, r, rng, tong))
if xep:
    print()
    print("  Te nhat: %.1f lan/mon. Voi nCount lon nhat script truyen (10 mon)"
          % xep[0][0])
    print("  => can %.0f lan roll. Tran ban va dat 2.000.000 => du xa %.0f lan."
          % (xep[0][0] * 10, 2000000 / (xep[0][0] * 10)))
