import os
lin = r'D:\ServerLinux\server1\settings\item\004\mask.txt'
if not os.path.exists(lin):
    # tim file mask.txt trong settings\item cua Linux
    for root, ds, fs in os.walk(r'D:\ServerLinux\server1\settings\item'):
        for f in fs:
            if f.lower() == 'mask.txt':
                print('found', os.path.join(root, f))
prj = r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item\mask.txt'
def rows(p):
    d = open(p, 'rb').read().split(b'\n')
    hdr = d[0].rstrip(b'\r').split(b'\t')
    out = []
    for l in d[1:]:
        l = l.rstrip(b'\r')
        if not l.strip():
            continue
        out.append(l.split(b'\t'))
    return hdr, out
h1, r1 = rows(lin)
h2, r2 = rows(prj)
print('LINUX hdr[:8]', h1[:8])
print('PRJ hdr[:8]', h2[:8])
def col(h, names):
    for n in names:
        for i, x in enumerate(h):
            if x.strip().lower() == n.lower():
                return i
    return None
pc1 = col(h1, [b'Particular', b'particular']); lc1 = col(h1, [b'Level', b'level']); nc1 = col(h1, [b'Name', b'name'])
pc2 = col(h2, [b'Particular', b'particular']); lc2 = col(h2, [b'Level', b'level']); nc2 = col(h2, [b'Name', b'name'])
print('cols linux p/l/n', pc1, lc1, nc1, 'prj', pc2, lc2, nc2)
want = [482, 447, 450, 446, 647, 806]
for w in want:
    for r in r1:
        try:
            if int(r[pc1]) == w:
                name = r[nc1]
                print('LINUX p=%d lv=%s name=%r' % (w, r[lc1], name))
                hits = [(x[pc2], x[lc2], x[nc2]) for x in r2 if x[nc2].strip() == name.strip()]
                print('   PRJ by name:', hits)
        except Exception as e:
            pass
