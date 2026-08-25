# -*- coding: utf-8 -*-
"""BO SOT cua A4: cac bo ba KHONG PHAI genre 6 trong 101 tep port."""
import sys, os, io, re, json
sys.path.insert(0, r'D:\GAMEDEVNEW\ReverseTools\port_3hd')
sys.stdout.reconfigure(encoding='utf-8', errors='replace')
from dec2 import decline2

JX1 = r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server'
LNX = r'D:\ServerLinux\server1'
LNXS = r'D:\ServerLinux\Patch\settings\item\004'
HERE = r'D:\GAMEDEVNEW\ReverseTools\port_3hd\thicong'
man = [x.strip() for x in io.open(os.path.join(HERE, 'b1_manifest.txt'), encoding='utf-8').read().split('\n') if x.strip()]

TRIP = re.compile(r'(?<![0-9])(\d{1,2})(\s*,\s*)(\d{1,3})(\s*,\s*)(\d{1,5})(?![0-9])')
SIG = re.compile(r'tbProp|AddItem|DropItem|ConsumeItem|CalcItemCount|GetItemCount|GiveAward|ForbidItem|ItemProp|DropRateItem', re.I)

hits = {}
for rel in man:
    if not rel.lower().endswith('.lua'):
        continue
    p = os.path.join(JX1, rel)
    if not os.path.isfile(p):
        continue
    for i, ln in enumerate(open(p, 'rb').read().split(b'\n'), 1):
        s = ln.decode('latin-1')
        if s.strip().startswith('--'):
            continue
        if not SIG.search(s):
            continue
        for m in TRIP.finditer(s):
            k = (int(m.group(1)), int(m.group(3)), int(m.group(5)))
            if k[0] in (0, 1, 2, 3, 4, 5) and k[0] < 6:
                hits.setdefault(k, []).append((rel, i, decline2(ln.strip())[:110]))

print('== bo ba genre != 6 trong 101 tep (bi _a4_cls.py loai bo) ==')
for k in sorted(hits):
    print('  %-12s x%d   vd: %s:%d' % ('%d,%d,%d' % k, len(hits[k]), hits[k][0][0], hits[k][0][1]))
print('tong:', len(hits), 'bo')
print()


def load2(path, iName, iGenre, iDetail, iPart):
    d = open(path, 'rb').read().split(b'\n')
    rows = {}
    for ln in d[1:]:
        c = decline2(ln.rstrip(b'\r')).split('\t')
        if len(c) <= max(iGenre, iDetail, iPart if iPart is not None else 0):
            continue
        try:
            g = int(c[iGenre]); dt = int(c[iDetail])
        except ValueError:
            continue
        rows[(g, dt)] = c[iName].strip()
    return rows


# questkey: Name Genre DetailType ...
qj = load2(os.path.join(JX1, 'settings', 'item', 'questkey.txt'), 0, 1, 2, None)
ql = load2(os.path.join(LNXS, 'questkey.txt'), 0, 1, 2, None)
print('questkey JX1 rows', len(qj), ' Linux rows', len(ql))
print()
print('== so ten questkey (genre 4) cho cac detail dung trong 101 tep ==')
for k in sorted(hits):
    if k[0] != 4:
        continue
    a = ql.get((4, k[1]), '<khong co Linux>')
    b = qj.get((4, k[1]), '<KHONG CO JX1>')
    flag = 'OK ' if re.sub(r'\s+', ' ', a.lower()) == re.sub(r'\s+', ' ', b.lower()) else 'LECH'
    print('  %s 4,%-4d Linux=%-40s JX1=%-40s' % (flag, k[1], a[:40], b[:40]))
print()
# mask (genre 0 detail 11)
print('== mask.txt (genre 0 detail 11) ==')
def loadmask(path):
    d = open(path, 'rb').read().split(b'\n')
    hdr = decline2(d[0].rstrip(b'\r')).split('\t')
    return hdr, d
for nm, p in (('JX1', os.path.join(JX1, 'settings', 'item', 'mask.txt')), ('LNX', os.path.join(LNXS, 'mask.txt'))):
    hdr, d = loadmask(p)
    print(' ', nm, 'cols=', hdr[:8], 'rows=', len(d))
