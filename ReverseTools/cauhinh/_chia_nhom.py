# -*- coding: utf-8 -*-
"""Chia danh sach hang so con lai thanh N nhom can bang, in ra dang JSON de
dua vao prompt cua Workflow."""
import io
import json
import os
import re
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
P = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                 "hangso_conlai.txt")
RA = os.path.join(os.path.dirname(os.path.abspath(__file__)), "nhom_hangso")

N = int(sys.argv[1]) if len(sys.argv) > 1 else 10

# loc them bang may: bo cac ten ro rang la ID bien nhiem vu / bien toan cuc
BO = re.compile(r"^(T_|TSK_|TASK_|GLB_WLLS|WLLS_LG|MS_|SC_|ST_STATE)", re.I)

theo_tep = {}
tep = None
for l in io.open(P, encoding="utf-8"):
    if l.startswith("## "):
        tep = l[3:].split("  (")[0].strip()
        continue
    m = re.match(r"\s*:(\d+)\s+(\S+)\s+=\s+(-?\d+)\s*(.*)$", l)
    if m and tep:
        ten = m.group(2)
        if BO.match(ten):
            continue
        theo_tep.setdefault(tep, []).append(
            {"dong": int(m.group(1)), "ten": ten, "gt": int(m.group(3)),
             "cm": m.group(4).strip()[:60]})

tong = sum(len(v) for v in theo_tep.values())
print("sau khi loc them bang may: %d hang so o %d tep"
      % (tong, len(theo_tep)))

# chia thanh N nhom can bang theo so hang so
nhom = [[] for _ in range(N)]
dem = [0] * N
for t, hs in sorted(theo_tep.items(), key=lambda x: -len(x[1])):
    k = dem.index(min(dem))
    nhom[k].append({"tep": t, "hangso": hs})
    dem[k] += len(hs)

if not os.path.isdir(RA):
    os.makedirs(RA)
for i, n in enumerate(nhom):
    p = os.path.join(RA, "nhom%02d.json" % (i + 1))
    with io.open(p, "w", encoding="utf-8", newline="") as f:
        json.dump(n, f, ensure_ascii=False, indent=1)
    print("  nhom%02d: %2d tep, %3d hang so" % (i + 1, len(n), dem[i]))
print("=> da ghi %s" % RA)
