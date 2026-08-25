# -*- coding: utf-8 -*-
"""Gom ScriptError.log theo (tep, ham, loi) + dem, tach theo NGAY de biet
loi nao la MOI (25/08 - do dot nay) va loi nao da co tu truoc."""
import io, os, re, sys, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\port_3hd")
from dec2 import decline2

J = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
paths = [os.path.join(J, "ScriptError.log")]
for dp, dn, fs in os.walk(J):
    for f in fs:
        if f.lower() == "scripterror.log" and os.path.join(dp, f) not in paths:
            paths.append(os.path.join(dp, f))

rx_hdr = re.compile(r"^(\d{4}/\d{2}/\d{2}) [\d:.]+\s+ScriptError.*Script Name: \((.*?)\)\s+cFuncName:\((.*?)\)")
for p in paths:
    if not os.path.isfile(p):
        continue
    raw = open(p, "rb").read().split(b"\n")
    cnt = collections.Counter()
    cur = None
    for ln in raw:
        s = decline2(ln.rstrip(b"\r"))
        m = rx_hdr.match(s.strip())
        if m:
            cur = (m.group(1), m.group(2), m.group(3))
            continue
        if cur and s.strip().startswith("error:"):
            cnt[(cur[0], cur[1], cur[2], s.strip()[:70])] += 1
            cur = None
    print("=" * 100)
    print("TEP:", p.replace(J, "<server>"), "  (%d dong)" % len(raw))
    print("=" * 100)
    for (day, script, fn, err), n in sorted(cnt.items(), key=lambda x: (-x[1], x[0])):
        mark = "  <== MOI 25/08" if day == "2026/08/25" else ""
        print("%5dx  %s  %-58s  fn=%-22s %s%s" % (n, day, script[-56:], fn[:22], err[:46], mark))
