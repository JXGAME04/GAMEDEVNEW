# -*- coding: utf-8 -*-
"""DOI CHIEU NHAN: moi dong trong 101 tep DA TRIEN KHAI co ca (a) mot chuoi ten
va (b) mot bo 6,d,P  ->  so ten do voi ten thuc te cua JX1 tai DONG P cua
magicscript.txt (engine tra theo CHI SO DONG = ParticularType).
Bat duoc ca nhung dong ma _a4_cls.py bo qua (khong co tu khoa tbProp/AddItem...)."""
import sys, os, io, re, unicodedata
sys.path.insert(0, r'D:\GAMEDEVNEW\ReverseTools\port_3hd')
sys.stdout.reconfigure(encoding='utf-8', errors='replace')
from dec2 import decline2

JX1 = r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server'
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


jx = rows(os.path.join(JX1, 'settings', 'item', 'magicscript.txt'))


def strip_accent(s):
    s = unicodedata.normalize('NFD', s)
    s = ''.join(c for c in s if unicodedata.category(c) != 'Mn')
    s = s.replace('đ', 'd').replace('Đ', 'D')
    return re.sub(r'[^a-z0-9]', '', s.lower())


man = [x.strip() for x in io.open(os.path.join(HERE, 'b1_manifest.txt'), encoding='utf-8').read().split('\n') if x.strip()]
TRIP = re.compile(r'(?<![0-9])6(\s*,\s*)(\d{1,2})(\s*,\s*)(\d{1,5})(?![0-9])')
STR = re.compile(r'"([^"]{3,60})"')
res = []
for rel in man:
    if not rel.lower().endswith('.lua'):
        continue
    p = os.path.join(JX1, rel)
    if not os.path.isfile(p):
        continue
    for i, raw in enumerate(open(p, 'rb').read().split(b'\n'), 1):
        s = decline2(raw.rstrip(b'\r'))
        if s.strip().startswith('--'):
            continue
        trips = list(TRIP.finditer(s))
        strs = [m.group(1) for m in STR.finditer(s)]
        if len(trips) != 1 or not strs:
            continue
        part = int(trips[0].group(4))
        jn = jx[part][0].strip() if part < len(jx) else '<NGOAI BANG JX1>'
        a = strip_accent(jn)
        ok = False
        for t in strs:
            b = strip_accent(t)
            if not b:
                continue
            if a and (a == b or a in b or b in a):
                ok = True
                break
        if not ok:
            res.append((rel, i, part, strs[0][:38], jn[:38], s.strip()[:95]))

print('So dong NHAN != TEN THUC TE cua JX1:', len(res))
print()
for rel, i, part, lab, jn, t in res:
    print('  6,%s,%-5d  nhan="%-34s"  JX1 thuc te="%-34s"  %s:%d' % ('?', part, lab, jn, rel, i))
