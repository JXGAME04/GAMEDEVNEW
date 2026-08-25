# -*- coding: utf-8 -*-
"""Engine JX1 tra bang theo CHI SO DONG, khong theo (genre,detail,particular).
  - genre 6 : GetMagicScript(nParticularType)  -> dong thu nParticularType
  - genre 0 detail<=10 : i = particular*10 + level - 1
  - genre 0 detail >10 : i = particular
Kiem lai cac ket luan cua A4 tren co so DUNG."""
import sys, os, re
sys.path.insert(0, r'D:\GAMEDEVNEW\ReverseTools\port_3hd')
sys.stdout.reconfigure(encoding='utf-8', errors='replace')
from dec2 import decline2

JX1 = r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item'
LNXS = r'D:\ServerLinux\Patch\settings\item\004'


def rows(path):
    d = open(path, 'rb').read().split(b'\n')
    out = []
    for ln in d[1:]:
        s = ln.rstrip(b'\r')
        if not s.strip():
            continue
        c = decline2(s).split('\t')
        out.append(c)
    return out


def norm(s):
    return re.sub(r'\s+', ' ', s.strip().lower())


# --- 1. magicscript: kiem dong thu i co ParticularType == i khong ---
for nm, p in (('JX1', os.path.join(JX1, 'magicscript.txt')), ('LNX', os.path.join(LNXS, 'magicscript.txt'))):
    r = rows(p)
    bad = [(i, c[1], c[2], c[3], c[0]) for i, c in enumerate(r) if len(c) > 3 and c[3].strip().lstrip('-').isdigit() and int(c[3]) != i]
    print('%s magicscript: %d dong, so dong co Particular != chi so dong: %d' % (nm, len(r), len(bad)))
    for b in bad[:6]:
        print('    dong %d: g=%s d=%s p=%s  %s' % (b[0], b[1], b[2], b[3], b[4][:40]))
print()

# --- 2. mask: so ten theo CHI SO DONG ---
mj = rows(os.path.join(JX1, 'mask.txt'))
ml = rows(os.path.join(LNXS, 'mask.txt'))
print('mask.txt  JX1 %d dong, Linux %d dong' % (len(mj), len(ml)))
for p in (446, 447, 450, 482, 647, 828):
    a = ml[p][0].strip() if p < len(ml) else '<ngoai bang Linux>'
    b = mj[p][0].strip() if p < len(mj) else '<NGOAI BANG JX1>'
    f = 'OK  ' if norm(a) == norm(b) else 'LECH'
    print('  %s p=%-4d Linux=%-38s JX1=%-38s' % (f, p, a[:38], b[:38]))
    if norm(a) != norm(b):
        cands = [i for i, c in enumerate(mj) if norm(c[0]) == norm(a)]
        print('        -> dong JX1 cung ten: %s' % (cands[:6],))
print()

# --- 3. horse: i = p*10 + lv - 1 ---
hj = rows(os.path.join(JX1, 'horse.txt'))
hl = rows(os.path.join(LNXS, 'horse.txt'))
print('horse.txt JX1 %d dong, Linux %d dong' % (len(hj), len(hl)))
for p, lv in ((5, 5), (5, 2), (5, 3), (5, 4), (5, 1), (8, 1), (6, 1), (7, 1), (19, 1), (20, 1), (21, 1)):
    i = p * 10 + lv - 1
    a = hl[i][0].strip() if i < len(hl) else '<ngoai bang Linux>'
    b = hj[i][0].strip() if i < len(hj) else '<NGOAI BANG JX1>'
    f = 'OK  ' if norm(a) == norm(b) else 'LECH'
    print('  %s p=%-3d lv=%-2d i=%-4d Linux=%-30s JX1=%-30s' % (f, p, lv, i, a[:30], b[:30]))
