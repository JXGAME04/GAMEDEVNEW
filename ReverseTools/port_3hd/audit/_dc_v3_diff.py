# -*- coding: utf-8 -*-
"""Doi chat doc lap: diff 101 tep Linux goc <-> cay song JX1."""
import sys, os, io, re, json
sys.path.insert(0, r'D:\GAMEDEVNEW\ReverseTools\port_3hd')
sys.stdout.reconfigure(encoding='utf-8', errors='replace')
from dec2 import decline2

JX1 = r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server'
LNX = r'D:\ServerLinux\server1'
HERE = r'D:\GAMEDEVNEW\ReverseTools\port_3hd\thicong'
man = [x.strip() for x in io.open(os.path.join(HERE, 'b1_manifest.txt'), encoding='utf-8').read().split('\n') if x.strip()]
print('manifest:', len(man))

missA = missB = 0
difflines = 0
perfile = {}
for rel in man:
    pa = os.path.join(LNX, rel)
    pb = os.path.join(JX1, rel)
    if not os.path.isfile(pa):
        missA += 1
        print('  THIEU O LINUX:', rel)
        continue
    if not os.path.isfile(pb):
        missB += 1
        print('  THIEU O JX1  :', rel)
        continue
    a = open(pa, 'rb').read().split(b'\n')
    b = open(pb, 'rb').read().split(b'\n')
    if len(a) != len(b):
        print('  !! SO DONG KHAC %s  lnx=%d jx1=%d' % (rel, len(a), len(b)))
    n = 0
    for i, (x, y) in enumerate(zip(a, b), 1):
        if x != y:
            n += 1
            perfile.setdefault(rel, []).append((i, x, y))
    difflines += n
print()
print('thieu Linux', missA, 'thieu JX1', missB, 'tong dong khac', difflines, 'so tep khac', len(perfile))
print()
# phan loai dong khac
CAT = {}
for rel, rows in perfile.items():
    for i, x, y in rows:
        xs = x.decode('latin-1'); ys = y.decode('latin-1')
        if 'AddNpcEx(' in ys and 'AddNpc(' in xs:
            c = 'AddNpc->AddNpcEx'
        elif 'bo Include' in ys:
            c = 'bo Include'
        elif re.sub(r'\d', '#', xs) == re.sub(r'\d', '#', ys):
            c = 'chi doi SO'
        else:
            c = 'KHAC'
        CAT.setdefault(c, []).append((rel, i, xs, ys))
for c in CAT:
    print('== %s : %d dong' % (c, len(CAT[c])))
for c in CAT:
    if c in ('KHAC',):
        for rel, i, xs, ys in CAT[c]:
            print('  %s:%d' % (rel, i))
            print('    LNX', xs[:150])
            print('    JX1', ys[:150])
print()
# voi nhom 'chi doi SO': liet ke tung cap so doi
print('== chi tiet cac cap so bi doi ==')
pairs = {}
for rel, i, xs, ys in CAT.get('chi doi SO', []):
    nx = re.findall(r'\d+', xs); ny = re.findall(r'\d+', ys)
    if len(nx) != len(ny):
        print('  ?? so luong so khac %s:%d' % (rel, i)); continue
    ch = [(a, b) for a, b in zip(nx, ny) if a != b]
    for a, b in ch:
        pairs.setdefault((a, b), []).append('%s:%d' % (rel, i))
for k in sorted(pairs, key=lambda t: -len(pairs[t])):
    print('  %-8s -> %-8s  x%d' % (k[0], k[1], len(pairs[k])))
