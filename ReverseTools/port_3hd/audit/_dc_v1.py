# -*- coding: utf-8 -*-
import sys, os
sys.path.insert(0, r'D:\GAMEDEVNEW\ReverseTools\port_3hd')
sys.stdout.reconfigure(encoding='utf-8', errors='replace')
from dec2 import decline2

JX1 = r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server'
LNX = r'D:\ServerLinux\server1'
LNXI = r'D:\ServerLinux\Patch\settings\item\004\magicscript.txt'


def loadtbl(path, iScript, iStack, ncol):
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
        rows[k] = (c[0].strip(), c[iScript].strip(), c[iStack].strip(), c)
    return rows


jx1 = loadtbl(os.path.join(JX1, 'settings', 'item', 'magicscript.txt'), 9, 12, 13)
lnx = loadtbl(LNXI, 13, 12, 14)
print('JX1 rows', len(jx1), ' LNX rows', len(lnx))


def sfile(root, scr):
    if not scr or scr == '0':
        return None
    p = os.path.join(root, scr.lstrip('\\').replace('\\', os.sep))
    return os.path.isfile(p)


KEYS = [(6, 1, 3360), (6, 1, 2356), (6, 1, 3430), (6, 1, 3362), (6, 1, 398), (6, 1, 399),
        (6, 1, 2015), (6, 1, 2024), (6, 1, 2126), (6, 1, 2135), (6, 1, 2117),
        (6, 1, 121), (6, 1, 122), (6, 1, 123), (6, 1, 124), (6, 1, 129), (6, 1, 130),
        (6, 1, 71), (6, 1, 3), (6, 1, 6), (6, 1, 1401), (6, 1, 3361), (6, 1, 4752),
        (6, 1, 1076), (6, 1, 1095), (6, 1, 1790), (6, 1, 2536), (6, 1, 2124), (6, 1, 2125),
        (6, 1, 3570), (6, 1, 3571), (6, 1, 3640), (6, 1, 4755), (6, 1, 3440), (6, 1, 2357),
        (6, 1, 4428), (6, 1, 4429), (6, 1, 3821), (6, 1, 26), (6, 1, 22)]
print()
print('%-13s %-34s %-46s %-6s %-6s' % ('KEY', 'TEN JX1', 'SCRIPT JX1', 'TEP?', 'STACK'))
for k in KEYS:
    r = jx1.get(k)
    if not r:
        print('%-13s KHONG CO O JX1' % (str(k),))
        continue
    print('%-13s %-34s %-46s %-6s %-6s' % (str(k), r[0][:34], r[1][:46], sfile(JX1, r[1]), r[2]))
