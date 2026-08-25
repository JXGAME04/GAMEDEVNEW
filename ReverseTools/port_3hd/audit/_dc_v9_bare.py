# -*- coding: utf-8 -*-
"""Doi chat muc 5: 'chi co 2 cho dang so tran'. Quet doc lap tren 101 tep LINUX GOC:
tim moi so nguyen == mot trong 42 nguon remap, dung O NGOAI dang bo ba."""
import sys, os, io, re, json
sys.path.insert(0, r'D:\GAMEDEVNEW\ReverseTools\port_3hd')
sys.stdout.reconfigure(encoding='utf-8', errors='replace')
from dec2 import decline2

LNX = r'D:\ServerLinux\server1'
PATCH = r'D:\ServerLinux\Patch'
HERE = r'D:\GAMEDEVNEW\ReverseTools\port_3hd\thicong'
remap = json.load(io.open(os.path.join(HERE, 'remap_resolved.json'), encoding='utf-8'))
srcp = set(int(k.split(',')[2]) for k in remap)
man = [x.strip() for x in io.open(os.path.join(HERE, 'b1_manifest.txt'), encoding='utf-8').read().split('\n') if x.strip()]

TRIP = re.compile(r'(?<![0-9])(\d{1,2})(\s*,\s*)(\d{1,3})(\s*,\s*)(\d{1,5})(?![0-9])')
NUM = re.compile(r'(?<![0-9.])(\d{2,5})(?![0-9])')
out = []
for rel in man:
    p = os.path.join(LNX, rel)
    if not os.path.isfile(p):
        p = os.path.join(PATCH, rel)
    if not os.path.isfile(p):
        continue
    for i, ln in enumerate(open(p, 'rb').read().split(b'\n'), 1):
        s = ln.decode('latin-1')
        if s.strip().startswith('--'):
            continue
        # che cac vi tri da nam trong bo ba
        masked = list(s)
        for m in TRIP.finditer(s):
            for j in range(m.start(), m.end()):
                masked[j] = ' '
        ms = ''.join(masked)
        for m in NUM.finditer(ms):
            v = int(m.group(1))
            if v in srcp:
                ctx = s.strip()
                out.append((rel, i, v, decline2(ctx.encode('latin-1'))[:120]))
print('So cho co SO TRAN trung voi 42 nguon remap:', len(out))
print()
for rel, i, v, t in out:
    print('  %-6d %s:%d  %s' % (v, rel, i, t))
