# -*- coding: utf-8 -*-
"""Trich CHINH XAC bo ba vat pham theo VI TRI THAM SO (khong dung regex tron)."""
import sys, os, io, re
sys.path.insert(0, r'D:\GAMEDEVNEW\ReverseTools\port_3hd')
sys.stdout.reconfigure(encoding='utf-8', errors='replace')
from dec2 import decline2

JX1 = r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server'
HERE = r'D:\GAMEDEVNEW\ReverseTools\port_3hd\thicong'
man = [x.strip() for x in io.open(os.path.join(HERE, 'b1_manifest.txt'), encoding='utf-8').read().split('\n') if x.strip()]

N = r'\s*(\d+)\s*'
PATS = [
    ('tbProp',      re.compile(r'tbProp\s*=\s*\{' + N + ',' + N + ',' + N)),
    ('AddItem',     re.compile(r'\bAddItem\s*\(' + N + ',' + N + ',' + N)),
    ('AddItemLuck', re.compile(r'\bAddItemEx\s*\(' + N + ',' + N + ',' + N)),
    ('DropItem',    re.compile(r'\bDropItem\s*\(\s*[^,]+,' + N + ',' + N + ',' + N)),
    ('ConsumeItem', re.compile(r'\bConsumeItem\s*\(\s*[^,]+,\s*[^,]+,' + N + ',' + N + ',' + N)),
    ('GetItemCount', re.compile(r'\bGetItemCount\s*\(\s*[^,]+,' + N + ',' + N + ',' + N)),
    ('CalcItemCount', re.compile(r'\bCalcItemCount\s*\(' + N + ',' + N + ',' + N)),
    ('key',         re.compile(r'\["' + N + ',' + N + ',' + N + r'"\]')),
]
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
        for nm, pat in PATS:
            for m in pat.finditer(s):
                k = (int(m.group(1)), int(m.group(2)), int(m.group(3)))
                hits.setdefault(k, []).append((rel, i, nm, decline2(ln.strip())[:100]))

print('Tong bo ba (chinh xac vi tri):', len(hits))
print()
for g in range(7):
    ks = [k for k in hits if k[0] == g]
    if not ks:
        continue
    print('== genre %d : %d bo ==' % (g, len(ks)))
    for k in sorted(ks):
        srcs = sorted(set('%s:%d(%s)' % (r, i, n) for r, i, n, _ in hits[k]))
        print('   %-12s x%-3d %s' % ('%d,%d,%d' % k, len(hits[k]), '; '.join(srcs[:4])))
    print()
