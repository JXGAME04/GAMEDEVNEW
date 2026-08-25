# -*- coding: utf-8 -*-
"""BO SOT: nCount cua bang thuong vs nMaxStack cua JX1 (o tui = ceil(nCount/max(stack,1)))."""
import sys, os, io, re, math
sys.path.insert(0, r'D:\GAMEDEVNEW\ReverseTools\port_3hd')
sys.stdout.reconfigure(encoding='utf-8', errors='replace')
from dec2 import decline2

JX1 = r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server'
LNXS = r'D:\ServerLinux\Patch\settings\item\004\magicscript.txt'
HERE = r'D:\GAMEDEVNEW\ReverseTools\port_3hd\thicong'


def rows(path):
    d = open(path, 'rb').read().split(b'\n')
    out = []
    for ln in d[1:]:
        s = ln.rstrip(b'\r')
        if not s.strip():
            continue
        out.append(decline2(s).split('\t'))
    return out


jx = rows(os.path.join(JX1, 'settings', 'item', 'magicscript.txt'))   # [12]=nMaxStack
lx = rows(LNXS)                                                       # [12]=la co xep chong


def jname(p):
    return jx[p][0].strip() if p < len(jx) else '<NGOAI BANG>'


def jstack(p):
    try:
        return int(jx[p][12])
    except Exception:
        return None


man = [x.strip() for x in io.open(os.path.join(HERE, 'b1_manifest.txt'), encoding='utf-8').read().split('\n') if x.strip()]
PAT = re.compile(r'tbProp\s*=\s*\{\s*6\s*,\s*(\d+)\s*,\s*(\d+)[^}]*\}\s*,\s*nCount\s*=\s*(\d+)')
PAT2 = re.compile(r'nCount\s*=\s*(\d+)[^}]*tbProp\s*=\s*\{\s*6\s*,\s*(\d+)\s*,\s*(\d+)')
bad = []
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
        for m in PAT.finditer(s):
            part = int(m.group(2)); cnt = int(m.group(3))
            st = jstack(part)
            if st is None:
                continue
            cells = cnt if st <= 1 else math.ceil(cnt / st)
            if cells >= 3:
                bad.append((cells, rel, i, part, jname(part), st, cnt))
bad.sort(reverse=True)
print('%-5s %-6s %-9s %-8s %s' % ('O TUI', 'nCount', 'nMaxStack', 'bo', 'ten JX1 / cho'))
for cells, rel, i, part, nm, st, cnt in bad:
    print('%-5d %-6d %-9d 6,1,%-5d %-34s %s:%d' % (cells, cnt, st, part, nm[:34], rel, i))
print()
print('Tong dong thuong ton >= 3 o tui:', len(bad))
