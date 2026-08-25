# -*- coding: utf-8 -*-
"""NANG-3: soat 8 tep bosstask_lev*.ini — so ten Linux vs JX1 cho MOI bo ba."""
import sys, os, re
sys.path.insert(0, r'D:\GAMEDEVNEW\ReverseTools\port_3hd')
sys.stdout.reconfigure(encoding='utf-8', errors='replace')
from dec2 import decline2

JX1 = r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server'
LNXI = r'D:\ServerLinux\Patch\settings\item\004\magicscript.txt'


def loadtbl(path, iScript, ncol):
    d = open(path, 'rb').read().split(b'\n')
    rows = {}
    for ln in d[1:]:
        c = decline2(ln.rstrip(b'\r')).split('\t')
        if len(c) < ncol:
            continue
        try:
            k = (int(c[1]), int(c[2]), int(c[3]))
        except ValueError:
            continue
        rows[k] = (c[0].strip(), c[iScript].strip())
    return rows


jx1 = loadtbl(os.path.join(JX1, 'settings', 'item', 'magicscript.txt'), 9, 13)
lnx = loadtbl(LNXI, 13, 14)


def norm(s):
    return re.sub(r'\s+', ' ', s.strip().lower())


allkeys = {}
for lv in (20, 30, 40, 50, 60, 70, 80, 90):
    p = os.path.join(JX1, 'settings', 'droprate', 'boss', 'bosstask_lev%d.ini' % lv)
    txt = open(p, 'rb').read().decode('latin-1')
    secs = re.findall(r'\[(\d+)\]\s*\r?\nGenre=(-?\d+)\s*\r?\nDetail=(-?\d+)\s*\r?\nParticular=(-?\d+)\s*\r?\nRandRate=(\d+)', txt)
    for sec, g, d, pt, rate in secs:
        k = (int(g), int(d), int(pt))
        allkeys.setdefault(k, []).append((lv, int(sec), int(rate)))
    print('lev%d: %d muc' % (lv, len(secs)))

print()
print('Tong bo ba khac nhau trong 8 tep ini:', len(allkeys))
print()
bad = same = miss_lnx = 0
for k in sorted(allkeys):
    ln = lnx.get(k)
    jn = jx1.get(k)
    lname = ln[0] if ln else '<khong co o LINUX>'
    jname = jn[0] if jn else '<KHONG CO O JX1>'
    if ln is None:
        miss_lnx += 1
    if jn is None or (ln and norm(lname) != norm(jname)):
        bad += 1
        print('  LECH %-11s Linux=%-36s JX1=%-36s (%d cho)' % ('%d,%d,%d' % k, lname[:36], jname[:36], len(allkeys[k])))
    else:
        same += 1
print()
print('trung ten:', same, ' lech/thieu:', bad, ' khong co o Linux:', miss_lnx)

# Tim so JX1 dung ten Linux (offset)
print()
print('== De xuat: tim so JX1 co CUNG TEN Linux ==')
for k in sorted(allkeys):
    ln = lnx.get(k)
    jn = jx1.get(k)
    if ln is None:
        continue
    if jn is not None and norm(ln[0]) == norm(jn[0]):
        continue
    cands = [kk for kk, vv in jx1.items() if kk[0] == k[0] and norm(vv[0]) == norm(ln[0])]
    print('  %-11s Linux "%s" -> ung vien JX1: %s' % ('%d,%d,%d' % k, ln[0][:34], cands))
