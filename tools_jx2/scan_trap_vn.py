# -*- coding: ascii -*-
# Quet 292 Region_S.dat cua map vn: tim DWORD ScriptID (g_FileName2Id, duong lowercase)
# cua cac trap ung vien -> biet map vn goi bo trap NAO (goc / tuyen* / khac)
import os, struct

BS = chr(92)
ROOT = r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\Maps'
JMG = (bytes([0xCC,0xD8,0xCA,0xE2,0xD3,0xC3,0xB5,0xD8]) + bytes([92]) +
       bytes([0xBD,0xA3,0xC3,0xC5,0xB9,0xD8])).decode('mbcs')
VN = os.path.join(ROOT, JMG + 'vn')

def fid(name_bytes):
    i = 0
    for k, b in enumerate(name_bytes):
        c = b - 0x100 if b >= 0x80 else b
        i = ((i + (k + 1) * c) % 0x8000000b * 0xffffffef) & 0xFFFFFFFF
    return (i ^ 0x12345678) & 0xFFFFFFFF

base = BS + 'script' + BS + 'missions' + BS + 'citywar_city' + BS
names = ['chengzhan_map' + BS + n for n in (
    'ctrap1.lua','ctrap1b.lua','ctrap1c.lua','ctrap2.lua','ctrap2b.lua','ctrap2c.lua',
    'trap1.lua','trap2.lua','andaoshouwei.lua','chuwuxiang.lua','yaoshang.lua',
    'tuyenduoicong.lua','tuyenduoithu.lua','tuyentrencong.lua','tuyentrenthu.lua')]
names += ['zhongzhuan_map' + BS + 'trap.lua']
cand = {}
for n in names:
    p = (base + n).lower()
    cand[fid(p.encode('latin-1'))] = n
# them vai duong khac de doi chieu (he cu, maps\congthanhchien)
extra = [BS + 'script' + BS + 'maps' + BS + 'congthanhchien' + BS + 'trap' + BS + x
         for x in ('cong_congphu_1.lua','thu_congchinh.lua')]
for p in extra:
    cand[fid(p.lower().encode('latin-1'))] = '[HE CU] ' + p

hits = {}
for r, ds, fs in os.walk(VN):
    for f0 in fs:
        d = open(os.path.join(r, f0), 'rb').read()
        for uid, nm in cand.items():
            pat = struct.pack('<L', uid)
            if pat in d:
                hits.setdefault(nm, []).append(os.path.basename(r) + BS + f0)
if not hits:
    print('KHONG tim thay ScriptID trap ung vien nao trong 292 cell!')
for nm in sorted(hits):
    print('%-40s %d cell: %s' % (nm, len(hits[nm]), ', '.join(hits[nm][:4])))
