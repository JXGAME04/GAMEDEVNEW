# -*- coding: utf-8 -*-
"""Tim bo so JX1 cho cac VE VAO Phong Lang Do (bi bo sot khoi bang 42 item vi
chung xuat hien duoi dang SO SANH TRAN 'particular == N', khong phai 6,1,N):
  Linux 6,1,2745 = "Lenh Bai Thuy Tac"   (gio ton phi)
  Linux 6,1,196  = "Mat do than bi"      (200 cuon - gio thuong)
  Linux 4,489    = "Lenh bai Phong Lang Do"
Quet CA magicscript.txt lan cac bang item khac cua JX1 theo TEN.
"""
import io, os, re, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\port_3hd")
from dec2 import decline2

JX1_ITEMDIR = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item"
LNX_ITEM = r"D:\ServerLinux\Patch\settings\item\004\magicscript.txt"

TARGETS = ["Lệnh Bài Thủy Tặc", "Mật đồ thần bí", "Lệnh bài Phong Lăng Độ",
           "Phong Lăng Độ lệnh bài", "Lệnh Bài Phong Lăng"]


def norm(s):
    return re.sub(r"\s+", " ", s.strip().lower())


def scan(path):
    try:
        d = open(path, "rb").read().split(b"\n")
    except Exception:
        return []
    if not d:
        return []
    hdr = decline2(d[0].rstrip(b"\r")).split("\t")
    def col(*names):
        for n in names:
            if n in hdr:
                return hdr.index(n)
        return None
    iG, iD, iP = col("Genre", "ItemGenre"), col("DetailType"), col("ParticularType")
    out = []
    for i, ln in enumerate(d[1:], 2):
        c = decline2(ln.rstrip(b"\r")).split("\t")
        if not c or not c[0].strip():
            continue
        name = c[0].strip()
        g = dd = pt = "?"
        try:
            if iG is not None and iG < len(c): g = c[iG]
            if iD is not None and iD < len(c): dd = c[iD]
            if iP is not None and iP < len(c): pt = c[iP]
        except Exception:
            pass
        out.append((name, g, dd, pt, os.path.basename(path), i))
    return out


rows = []
for dp, dn, fs in os.walk(JX1_ITEMDIR):
    for f in fs:
        if f.lower().endswith(".txt"):
            rows += scan(os.path.join(dp, f))
print("Da quet %d dong item JX1" % len(rows))
print()
for t in TARGETS:
    nt = norm(t)
    hits = [r for r in rows if norm(r[0]) == nt]
    loose = [r for r in rows if nt in norm(r[0]) or norm(r[0]) in nt] if not hits else []
    print("### %s" % t)
    for r in (hits or loose)[:6]:
        print("   %-34s  %s,%s,%s   (%s dong %d)" % (r[0][:34], r[1], r[2], r[3], r[4], r[5]))
    if not hits and not loose:
        print("   (KHONG TIM THAY)")
    print()
