# -*- coding: ascii -*-
import zipfile, os, hashlib
BS = chr(92)
Z = r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\missions\citywar_city.zip'
G = r'D:\ServerLinux\server1\script\missions\citywar_city'
T = r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\missions\citywar_city'
z = zipfile.ZipFile(Z)
zn = {}
for n in z.namelist():
    if n.endswith('/'):
        continue
    key = n.replace('/', BS).lower()
    if key.startswith('citywar_city' + BS):
        key = key[len('citywar_city' + BS):]
    zn[key] = hashlib.md5(z.read(n)).hexdigest()

def scan(root):
    out = {}
    for r, ds, fs in os.walk(root):
        for f in fs:
            p = os.path.join(r, f)
            rel = os.path.relpath(p, root).lower()
            out[rel] = hashlib.md5(open(p, 'rb').read()).hexdigest()
    return out

gn = scan(G); tn = scan(T)
allk = sorted(set(zn) | set(gn) | set(tn))
print('%-44s %-7s %-7s %-7s' % ('file', 'ZIP', 'GOC', 'TA'))
for k in allk:
    a = zn.get(k); b = gn.get(k); c = tn.get(k)
    def m(x):
        return '-' if x is None else x[:6]
    tag = ''
    if a and b:
        tag += ' zg' + ('=' if a == b else '!')
    if a and c:
        tag += ' zt' + ('=' if a == c else '!')
    print('%-44s %-7s %-7s %-7s%s' % (k[:44], m(a), m(b), m(c), tag))
print('zip:', len(zn), 'goc:', len(gn), 'ta:', len(tn))
