# -*- coding: utf-8 -*-
"""So ten mask.txt / horse.txt (genre 0 detail 11 / 10) giua Linux va JX1
cho cac bo dung trong 101 tep port."""
import sys, os, re
sys.path.insert(0, r'D:\GAMEDEVNEW\ReverseTools\port_3hd')
sys.stdout.reconfigure(encoding='utf-8', errors='replace')
from dec2 import decline2

JX1 = r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item'
LNXS = r'D:\ServerLinux\Patch\settings\item\004'


def load(path):
    d = open(path, 'rb').read().split(b'\n')
    rows = {}
    for ln in d[1:]:
        c = decline2(ln.rstrip(b'\r')).split('\t')
        if len(c) < 4:
            continue
        try:
            k = (int(c[1]), int(c[2]), int(c[3]))
        except ValueError:
            continue
        if k not in rows:
            rows[k] = c[0].strip()
    return rows


def norm(s):
    return re.sub(r'\s+', ' ', s.strip().lower())


for fn, keys in (('mask.txt', [(0, 11, x) for x in (446, 447, 450, 482, 647, 828)]),
                 ('horse.txt', [(0, 10, x) for x in (5, 6, 7, 8, 19, 20, 21)])):
    j = load(os.path.join(JX1, fn))
    l = load(os.path.join(LNXS, fn))
    print('== %s   JX1=%d rows  Linux=%d rows ==' % (fn, len(j), len(l)))
    for k in keys:
        a = l.get(k, '<khong co Linux>')
        b = j.get(k, '<KHONG CO O JX1>')
        f = 'OK  ' if norm(a) == norm(b) else 'LECH'
        print('  %s %-11s Linux=%-38s JX1=%-38s' % (f, '%d,%d,%d' % k, a[:38], b[:38]))
        if norm(a) != norm(b):
            cands = [kk for kk, vv in j.items() if kk[:2] == k[:2] and norm(vv) == norm(a)]
            print('        -> ung vien JX1 cung ten: %s' % (cands,))
    print()
