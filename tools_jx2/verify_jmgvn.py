# -*- coding: ascii -*-
# Xac minh E1b: dem file loose + doi chieu BYTE voi pak TA (dir 'ib') tren toan bo cell trung
import struct, os

BS = bytes([92])
PAK_TA = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\Pak\maps.pak"
ROOT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\Maps"
JMG = bytes([0xCC,0xD8,0xCA,0xE2,0xD3,0xC3,0xB5,0xD8]) + BS + bytes([0xBD,0xA3,0xC3,0xC5,0xB9,0xD8])
VN_DIR = os.path.join(ROOT, JMG.decode('mbcs') + 'vn')

def fid(name):
    i = 0; idx = 0
    for b in name:
        c = b + 0x61 - 0x41 if 0x41 <= b <= 0x5A else (b - 0x100 if b >= 0x80 else b)
        idx += 1; i = ((i + idx * c) % 0x8000000b * 0xffffffef) & 0xFFFFFFFF
    return (i ^ 0x12345678) & 0xFFFFFFFF

def nrv2b(src, dst_size):
    dst = bytearray(); bb = 0; ip = 0; last = 1
    def gb():
        nonlocal bb, ip
        if bb & 0x7f: bb = (bb * 2) & 0xFFFFFFFF
        else: bb = src[ip] * 2 + 1; ip += 1
        return (bb >> 8) & 1
    while True:
        while gb():
            dst.append(src[ip]); ip += 1
        off = 1
        while True:
            off = off * 2 + gb()
            if gb(): break
        if off == 2: off = last
        else:
            off = (off - 3) * 256 + src[ip]; ip += 1
            if off == 0xFFFFFFFF: break
            off += 1; last = off
        ln = gb(); ln = ln * 2 + gb()
        if ln == 0:
            ln = 1
            while True:
                ln = ln * 2 + gb()
                if gb(): break
            ln += 2
        if off > 0xd00: ln += 1
        pos = len(dst) - off
        for _ in range(ln + 1):
            dst.append(dst[pos]); pos += 1
    assert len(dst) == dst_size
    return bytes(dst)

f = open(PAK_TA, 'rb')
sig, cnt, io_, do_, crc = struct.unpack('<4sLLLL', f.read(32)[:20])
f.seek(io_); raw = f.read(16 * cnt)
tbl = {}
for k in range(cnt):
    uid, off, size, flag = struct.unpack_from('<LLlL', raw, k * 16)
    tbl[uid] = (off, size, flag)

n = 0; match = 0; miss_in_ta = 0; diff = 0
for r, ds, fs in os.walk(VN_DIR):
    for f0 in fs:
        n += 1
        loose = open(os.path.join(r, f0), 'rb').read()
        vdir = os.path.basename(r)                      # v_YYY
        rel = (BS + b'maps' + BS + JMG + b'ib' + BS +
               vdir.encode('mbcs') + BS + f0.encode('mbcs'))
        hit = tbl.get(fid(rel))
        if hit is None:
            miss_in_ta += 1
            continue
        off, size, flag = hit
        method = flag & 0xFF000000; cs = flag & 0xFFFFFF
        f.seek(off)
        data = f.read(size if method == 0 else cs)
        if method != 0:
            data = nrv2b(data, size)
        if data == loose: match += 1
        else: diff += 1
print('loose cells:', n, '| trung khop byte voi pak TA (ib):', match,
      '| ta khong co cell nay:', miss_in_ta, '| KHAC BYTE:', diff)
assert diff == 0, 'CO CELL KHAC BYTE - DUNG LAI KIEM TRA'
print('VERIFY OK')
