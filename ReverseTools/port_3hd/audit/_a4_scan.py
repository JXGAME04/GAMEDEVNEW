# -*- coding: utf-8 -*-
"""Quet TOAN CAY JX1 (script+settings) tim:
  A) con sot bo so NGUON (Linux) chua duoc anh xa  -> DUONG SOT
  B) bo so DICH da co he KHAC cua JX1 dung  -> DUNG CHUNG
  C) dang so tran (== N / = N) cua 42 particular trong cac tep manifest
"""
import io, os, re, sys, json
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\port_3hd")
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
from dec2 import decline2

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
HERE = r"D:\GAMEDEVNEW\ReverseTools\port_3hd\thicong"
remap = json.load(io.open(os.path.join(HERE, "remap_resolved.json"), encoding="utf-8"))
manifest = set(x.lower().replace("/", "\\") for x in
               io.open(os.path.join(HERE, "b1_manifest.txt"), encoding="utf-8").read().split("\n"))

pats_src = {}
pats_dst = {}
for s, d in remap.items():
    g, dd, pt = s.split(",")
    pats_src[s] = re.compile(r"(?<![0-9])%s(\s*,\s*)%s(\s*,\s*)%s(?![0-9])" % (g, dd, pt))
    g2, dd2, pt2 = d.split(",")
    pats_dst[d] = re.compile(r"(?<![0-9])%s(\s*,\s*)%s(\s*,\s*)%s(?![0-9])" % (g2, dd2, pt2))

roots = [os.path.join(JX1, "script"), os.path.join(JX1, "scriptjx2"), os.path.join(JX1, "settings")]
files = []
for r in roots:
    for dp, dn, fs in os.walk(r):
        for f in fs:
            if f.lower().endswith((".lua", ".txt", ".ini", ".tab")):
                files.append(os.path.join(dp, f))
print("so tep quet:", len(files))

A = {}
B = {}
for p in files:
    try:
        raw = open(p, "rb").read()
    except Exception:
        continue
    d = raw.decode("latin-1")
    rel = os.path.relpath(p, JX1).lower()
    inman = rel in manifest
    for s, pat in pats_src.items():
        for m in pat.finditer(d):
            ln = d.count("\n", 0, m.start()) + 1
            A.setdefault(s, []).append((rel, ln, inman, decline2(d.split("\n")[ln-1].strip().encode("latin-1"))[:110]))
    for dst, pat in pats_dst.items():
        for m in pat.finditer(d):
            ln = d.count("\n", 0, m.start()) + 1
            B.setdefault(dst, []).append((rel, ln, inman, decline2(d.split("\n")[ln-1].strip().encode("latin-1"))[:110]))

print("\n===== A) CON SOT bo so NGUON (Linux) trong cay JX1 =====")
for s in sorted(A):
    print("-- %s (JX1 so nay = ?)" % s)
    for rel, ln, inman, txt in A[s]:
        print("   %s%s:%d  %s" % ("[MANIFEST] " if inman else "", rel, ln, txt))

print("\n===== B) bo so DICH xuat hien o dau (ke ca he JX1 cu) =====")
for d in sorted(B):
    out = B[d]
    nonman = [x for x in out if not x[2]]
    print("-- %s : tong %d cho, NGOAI manifest %d" % (d, len(out), len(nonman)))
    for rel, ln, inman, txt in nonman:
        print("   !! %s:%d  %s" % (rel, ln, txt))
