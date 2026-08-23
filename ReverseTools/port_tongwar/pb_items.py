import re, os
PR = r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings'
LN = r'D:\ServerLinux\server1\settings'
ms = open(os.path.join(PR, r'item\magicscript.txt'), 'rb').read().split(b'\n')
print('magicscript.txt du an: so dong', len(ms), 'dong cuoi rong?', ms[-1].strip() == b'')
print('header:', ms[0].rstrip(b'\r').split(b'\t')[:12])
def find(name_bytes):
    out = []
    for i, l in enumerate(ms):
        c = l.rstrip(b'\r').split(b'\t')
        if c and c[0].strip() == name_bytes:
            out.append((i + 1, i + 1 - 2, c[:12]))
    return out
for nm in [b'Qu\xb6 \xa7\xb9i Ho\xb5ng Kim', b'Ho\xb5ng Ch\xa9n \xa7\xacn', b'H\xe7n nguy\xaan ch\xa9n \xae\xacn', b'H\xe7n Nguy\xaan Ch\xa9n \xa7\xacn']:
    print(nm, find(nm))
# 4864 = dong 4866 ton tai?
print('dong 4866 (id 4864):', ms[4865][:60] if len(ms) > 4865 else 'KHONG CO')
print('dong 4865 (id 4863):', ms[4864][:60] if len(ms) > 4864 else 'KHONG CO')
print('dong 4859 (id 4857):', ms[4858][:80] if len(ms) > 4858 else 'KHONG CO')
print('dong 2275 (id 2273):', ms[2274][:120])
# Linux magicscript
lms = open(os.path.join(LN, r'item\004\magicscript.txt'), 'rb').read().split(b'\n') if os.path.exists(os.path.join(LN, r'item\004\magicscript.txt')) else None
if lms:
    for i, l in enumerate(lms):
        c = l.rstrip(b'\r').split(b'\t')
        if len(c) > 3 and c[3].strip() in (b'30438', b'2264', b'30301') and c[1].strip() == b'6' and c[2].strip() == b'1':
            print('LINUX', i + 1, c[:4], c[13:15] if len(c) > 14 else '')
# goldequip
ge = open(os.path.join(PR, 'goldequip.txt'), 'rb').read().split(b'\n')
print('goldequip du an dong', len(ge), 'hdr', ge[0].rstrip(b'\r').split(b'\t')[:6])
for i in (4492, 4493, 4494, 3477, 3478):
    c = ge[i].rstrip(b'\r').split(b'\t')
    print(' dong', i + 1, c[:4], c[8:16])
lge = open(os.path.join(LN, 'goldequip.txt'), 'rb').read().split(b'\n')
print('goldequip linux dong', len(lge), 'hdr', lge[0].rstrip(b'\r').split(b'\t')[:6])
for i, l in enumerate(lge):
    c = l.rstrip(b'\r').split(b'\t')
    if c and c[0].strip() in (b'4862', b'3477'):
        print(' LINUX dong', i + 1, c[:4], c[8:16])
