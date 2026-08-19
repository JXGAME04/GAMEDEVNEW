#!/usr/bin/env python3
# -*- coding: ascii -*-
"""
re_pe_crt.py - KIEM THU BAT BUOC TRUOC KHI TRIEN KHAI .dll/.exe CLIENT.

Doc bang nhap PE de biet module dung thu vien CRT nao. Tron Debug voi Release
giua cac module = SAP GAME (xem BANGIAO_PHIEN_1808.md muc "Hai lan mac cung mot loi").

Bang dung cho client JX1 nay:
    engine.dll        UCRT-DEBUG      (Debug|Win32)
    Represent2.dll    UCRT-DEBUG      (Debug|Win32)
    LuaLibDll.dll     UCRT-DEBUG
    Game.exe          UCRT-RELEASE    (Release|Win32)
    CoreClient.dll    CRT-TINH        (ClientRelease)

DUNG:
    python re_pe_crt.py E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/client
    python re_pe_crt.py <tep.dll>
"""
import struct, sys, os, datetime

DUNG = {
    'engine.dll':     'UCRT-DEBUG',
    'represent2.dll': 'UCRT-DEBUG',
    'lualibdll.dll':  'UCRT-DEBUG',
    'game.exe':       'UCRT-RELEASE',
    'coreclient.dll': 'CRT-TINH',
}

def imports(path):
    d = open(path, 'rb').read()
    pe = struct.unpack_from('<I', d, 0x3c)[0]
    nsec = struct.unpack_from('<H', d, pe + 6)[0]
    optsz = struct.unpack_from('<H', d, pe + 20)[0]
    magic = struct.unpack_from('<H', d, pe + 24)[0]
    base = pe + 24 + (96 if magic == 0x10b else 112)
    imp = struct.unpack_from('<I', d, base + 8)[0]
    secs = []
    for i in range(nsec):
        o = pe + 24 + optsz + i * 40
        vs, va, rs, pr = struct.unpack_from('<IIII', d, o + 8)
        secs.append((va, vs, pr, rs))
    def r2o(r):
        for va, vs, pr, rs in secs:
            if va <= r < va + max(vs, rs):
                return pr + (r - va)
        return None
    out = []
    o = r2o(imp)
    while o:
        ent = d[o:o + 20]
        if len(ent) < 20 or ent == b'\0' * 20:
            break
        nr = struct.unpack_from('<I', ent, 12)[0]
        if nr == 0:
            break
        out.append(d[r2o(nr):].split(b'\0')[0].decode('latin-1'))
        o += 20
    return out

def crt_of(names):
    low = [x.lower() for x in names]
    if any('ucrtbased' in x for x in low):
        return 'UCRT-DEBUG'
    if any(x.startswith('ucrtbase.') or x.startswith('api-ms-win-crt') for x in low):
        return 'UCRT-RELEASE'
    return 'CRT-TINH'

def check(path):
    n = os.path.basename(path).lower()
    got = crt_of(imports(path))
    want = DUNG.get(n)
    ts = datetime.datetime.fromtimestamp(os.path.getmtime(path)).strftime('%Y-%m-%d %H:%M')
    if want is None:
        flag = '(khong co trong bang)'
    elif want == got:
        flag = 'DUNG'
    else:
        flag = '>>> SAI! phai la %s <<<' % want
    print("%-18s %-17s %10d  %-14s %s" % (os.path.basename(path), ts, os.path.getsize(path), got, flag))
    return want is None or want == got

if __name__ == '__main__':
    t = sys.argv[1] if len(sys.argv) > 1 else \
        'E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/client'
    print("%-18s %-17s %10s  %-14s %s" % ('module', 'ngay', 'co', 'CRT', 'ket luan'))
    print('-' * 88)
    ok = True
    if os.path.isdir(t):
        for f in ('Game.exe', 'CoreClient.dll', 'Represent2.dll', 'engine.dll', 'LuaLibDll.dll'):
            p = os.path.join(t, f)
            if os.path.exists(p):
                ok &= check(p)
    else:
        ok &= check(t)
    print('-' * 88)
    print('KET QUA: ' + ('tat ca khop bang dung' if ok else 'CO MODULE SAI CAU HINH - DUNG TRIEN KHAI'))
    sys.exit(0 if ok else 1)
