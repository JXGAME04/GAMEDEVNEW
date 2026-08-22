#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Bao dong Include de quy cho cay Bach Nhan Loi Dai (ban sua cua tinsu_closure.py).
Khac ban goc: (1) tep `lib/*.lua` thieu o script\\lib duoc engine tu anh xa sang
scriptjx2\\lib (ScriptFuns.cpp sJX2RemapScriptPath) -> tinh la DA CO neu ton tai o do;
(2) ROOTS/ROOTFILES cua tinh nang nay; (3) --list-missing ghi ra port_bairenleitai/.
"""
import io, os, re, sys, filecmp, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BS = chr(92)
LNX = r"D:\ServerLinux\server1\script"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
SRV2 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\scriptjx2\tong_vn"
SRV3 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\scriptjx2\lib"
ROOTS = ["missions/bairenleitai"]
ROOTFILES = ["event/msg2allworld.lua", "missions/basemission/lib.lua",
             "activitysys/config/9/config.lua", "activitysys/config/9/head.lua", "activitysys/config/9/registe.lua"]
# npcdailog.lua bi thay bang Include dailogsay (keo bonusvlmc/vng_toolaward - khong port)
SKIP = {"activitysys/npcdailog.lua"}


def norm(s):
    s = s.replace(BS, "/")
    while "//" in s:
        s = s.replace("//", "/")
    s = s.lstrip("/")
    if s.lower().startswith("script/"):
        s = s[7:]
    return s


def includes_of(relpath):
    p = os.path.join(LNX, relpath)
    if not os.path.isfile(p):
        return []
    t = io.open(p, encoding="latin-1").read()
    out = []
    for line in t.split("\n"):
        s = line.split("--")[0]
        for m in re.finditer(r'Include\s*\(\s*(?:"([^"]+)"|\[\[([^\]]+)\]\])', s):
            out.append(norm(m.group(1) or m.group(2)))
    return out


def proj_path(rel):
    """duong dan ban du an tuong ung (uu tien script\\, roi remap tong/ va lib/)."""
    p = os.path.join(SRV, rel)
    if os.path.isfile(p):
        return p
    low = rel.lower()
    if low.startswith("tong/"):
        q = os.path.join(SRV2, rel[5:])
        if os.path.isfile(q):
            return q
    if low.startswith("lib/"):
        q = os.path.join(SRV3, rel[4:])
        if os.path.isfile(q):
            return q
    return None


seeds = list(ROOTFILES)
for r in ROOTS:
    for dp, _, fs in os.walk(os.path.join(LNX, r)):
        for f in fs:
            if f.lower().endswith(".lua"):
                seeds.append(os.path.relpath(os.path.join(dp, f), LNX).replace(BS, "/"))
seen, origin, stack = {}, {s: "ROOT" for s in seeds}, list(seeds)
while stack:
    cur = stack.pop()
    key = cur.lower()
    if key in seen:
        continue
    seen[key] = cur
    if key in {x.lower() for x in SKIP}:
        continue
    if proj_path(cur):      # tep du an DA CO -> khong de quy (luc chay lay ban du an)
        continue
    for inc in includes_of(cur):
        if inc.lower() not in seen:
            origin.setdefault(inc, cur)
            stack.append(inc)

rows = []
for key, rel in sorted(seen.items()):
    lp = os.path.join(LNX, rel)
    pp = proj_path(rel)
    if not os.path.isfile(lp):
        st = "LINUX-KHONG"
    elif pp:
        tag = "" if pp.startswith(SRV + BS) else ("(tong_vn)" if pp.startswith(SRV2) else "(scriptjx2/lib)")
        st = ("IDENT" if filecmp.cmp(lp, pp, shallow=False) else "DIFF") + tag
    else:
        st = "THIEU"
    rows.append((st, rel, "" if origin.get(rel) == "ROOT" else "<- " + origin.get(rel, "?")))
cnt = collections.Counter(r[0] for r in rows)
print("TONG", len(rows), dict(cnt))
for st, rel, o in rows:
    print("%-22s %-62s %s" % (st, rel, o))
if "--list-missing" in sys.argv:
    io.open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "closure_missing.txt"), "w").write(
        "\n".join(rel for st, rel, o in rows if st == "THIEU"))
