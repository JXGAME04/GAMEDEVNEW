# -*- coding: ascii -*-
# E1b: trich map JianMenGuan-vn (509 region + .wor) tu pak LINUX ra loose bin\server\Maps
# Format: XPackFile.cpp (header PACK, index {uId,uOffset,lSize,lCompressSizeFlag}),
# id = KPakList::FileNameToId (KPakList.cpp:72-85, char CO DAU -> byte>=0x80 la so am),
# nen = ucl_nrv2b_decompress_8.
import struct, os, sys

PAK_LINUX = r"D:\ServerLinux\server1\pak\maps.pak"
PAK_TA    = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\Pak\maps.pak"
OUT_ROOT  = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\Maps"

JMG   = bytes([0xCC,0xD8,0xCA,0xE2,0xD3,0xC3,0xB5,0xD8]) + b'\\' + bytes([0xBD,0xA3,0xC3,0xC5,0xB9,0xD8])  # te-shu-yong-di \ jian-men-guan
DIRVN = b'\\maps\\' + JMG + b'vn'

def file_name_to_id(name):
    # name: bytes; A-Z gap ve thuong; char la SIGNED (byte >=0x80 -> am)
    i = 0
    idx = 0
    for b in name:
        c = b
        if 0x41 <= b <= 0x5A:
            c = b + 0x61 - 0x41
        elif b >= 0x80:
            c = b - 0x100
        idx += 1
        i = ((i + idx * c) % 0x8000000b * 0xffffffef) & 0xFFFFFFFF
    return (i ^ 0x12345678) & 0xFFFFFFFF

def nrv2b_decompress(src, dst_size):
    dst = bytearray()
    bb = 0
    ip = 0
    last_off = 1
    def getbit():
        nonlocal bb, ip
        if bb & 0x7f:
            bb = (bb * 2) & 0xFFFFFFFF
        else:
            bb = src[ip] * 2 + 1
            ip += 1
        return (bb >> 8) & 1
    while True:
        while getbit():
            dst.append(src[ip]); ip += 1
        off = 1
        while True:
            off = off * 2 + getbit()
            if getbit(): break
        if off == 2:
            off = last_off
        else:
            off = (off - 3) * 256 + src[ip]; ip += 1
            if off == 0xFFFFFFFF or off < 0:
                break
            off += 1
            last_off = off
        ln = getbit()
        ln = ln * 2 + getbit()
        if ln == 0:
            ln = 1
            while True:
                ln = ln * 2 + getbit()
                if getbit(): break
            ln += 2
        if off > 0xd00:
            ln += 1
        # copy ln+1 byte tu dst[-off]
        pos = len(dst) - off
        assert pos >= 0, 'offset lui qua dau'
        for _ in range(ln + 1):
            dst.append(dst[pos]); pos += 1
    assert len(dst) == dst_size, (len(dst), dst_size)
    return bytes(dst)

def load_index(pak):
    f = open(pak, 'rb')
    hdr = f.read(32)
    sig, cnt, idx_off, dat_off, crc = struct.unpack('<4sLLLL', hdr[:20])
    assert sig == b'KCAP'[::-1] or sig == b'PACK', sig
    f.seek(idx_off)
    raw = f.read(16 * cnt)
    tbl = {}
    for k in range(cnt):
        uid, off, size, flag = struct.unpack_from('<LLlL', raw, k * 16)
        tbl[uid] = (off, size, flag)
    return f, tbl

def read_elem(f, off, size, flag):
    method = flag & 0xFF000000
    csize = flag & 0x00FFFFFF
    if method == 0:
        f.seek(off)
        return f.read(size)
    assert method in (0x01000000, 0x20000000), hex(method)
    f.seek(off)
    comp = f.read(csize)
    return nrv2b_decompress(comp, size)

def main():
    f, tbl = load_index(PAK_LINUX)
    print('index Linux pak:', len(tbl), 'entries')
    # 1) .wor (nam duoi ten ...vn.wor)
    jobs = [(DIRVN + b'.wor', JMG + b'vn.wor')]
    # 2) region: trong pak Linux du lieu nam duoi thu muc GOC (khong 'vn');
    #    .wor vn co rect=93,98,122,114 -> engine chi nap cell x93..122, y98..114
    #    duoi duong \maps\<dir MapList>=...vn\ -> trich tu dir goc, GHI RA duoi ten vn
    DIRGOC = b'\\maps\\' + JMG
    for vy in range(98, 115):
        for fx in range(93, 123):
            rel = DIRGOC + (b'\\v_%03d\\%03d_Region_S.dat' % (vy, fx))
            outrel = JMG + (b'vn\\v_%03d\\%03d_Region_S.dat' % (vy, fx))
            jobs.append((rel, outrel))
    found = 0
    wor_ok = False
    for rel, outrel in jobs:
        uid = file_name_to_id(rel)
        hit = tbl.get(uid)
        if hit is None:
            continue
        data = read_elem(f, *hit)
        # ten tren dia = byte GBK dien giai qua ANSI codepage (engine mo bang CreateFileA)
        outp = os.path.join(OUT_ROOT, outrel.decode('mbcs'))
        os.makedirs(os.path.dirname(outp), exist_ok=True)
        open(outp, 'wb').write(data)
        found += 1
        if rel.endswith(b'.wor'):
            wor_ok = True
            print('WOR OK:', len(data), 'bytes')
    print('tong file trich:', found, '(cua so 17x30=510 slot; ky vong ~285-330 cell + 1 wor)')
    assert wor_ok, 'THIEU .wor!'
    return found

if __name__ == '__main__':
    n = main()
    sys.exit(0 if n >= 400 else 1)
