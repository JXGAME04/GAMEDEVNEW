# -*- coding: utf-8 -*-
"""[DUNGLUYEN-HKMP 02/09] Dien cot 58 (so o Van Cuong) / 59 (pham chat toi da)
cho hoang kim mon phai + An Bang + Dinh Quoc trong goldequip.txt.

Muc do chu game chot 02/09:
    HKMP           : 4 o / pham chat 10
    An Bang        : 3 o / pham chat 8
    Dinh Quoc      : 2 o / pham chat 7
    [Cuc pham]/[Hoan My] : 3 o / pham chat 9
KHONG dung toi do thue theo gio ([Dinh thoi] / [Han che thoi gian]).

Doc/ghi latin-1 (anh xa 1:1 byte) -> khong the pha byte TCVN3.
Chay lai duoc nhieu lan: chi dien o DANG TRONG.
"""
import io, os, sys, shutil
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from phanloai_hkmp import classify, load

DUOI = ".truoc_dungluyen_hkmp_0209"
TEP = [
    "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/settings/item/goldequip.txt",
    "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/client/settings/item/goldequip.txt",
]

def dem_byte_cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)

def va(p, res, that=True):
    goc = io.open(p, 'r', encoding='latin-1', newline='').read()
    dong = goc.split('\n')
    hi_truoc = dem_byte_cao(goc)
    sua = 0
    for filerow, (cap, qual, _nhan) in res.items():
        i = filerow - 1
        line = dong[i]
        cr = line.endswith('\r')
        f = (line[:-1] if cr else line).split('\t')
        if len(f) != 62:
            raise SystemExit('DUNG LAI: dong %d co %d cot (mong doi 62)' % (filerow, len(f)))
        if f[57].strip() or f[58].strip():
            continue                       # da co gia tri -> khong dung toi
        f[57] = str(cap)
        f[58] = str(qual)
        dong[i] = '\t'.join(f) + ('\r' if cr else '')
        sua += 1
    moi = '\n'.join(dong)
    hi_sau = dem_byte_cao(moi)
    if hi_sau != hi_truoc:
        raise SystemExit('DUNG LAI: byte cao doi %d -> %d' % (hi_truoc, hi_sau))
    if moi.count('\n') != goc.count('\n'):
        raise SystemExit('DUNG LAI: so dong doi')
    if that:
        if not os.path.exists(p + DUOI):
            shutil.copy2(p, p + DUOI)
        io.open(p, 'w', encoding='latin-1', newline='').write(moi)
    print('%s: sua %d dong | byte cao %d (khong doi) | %d dong tep'
          % (os.path.basename(os.path.dirname(os.path.dirname(p))) + '/' + os.path.basename(p),
             sua, hi_sau, len(dong)))
    return sua

if __name__ == '__main__':
    that = '--that' in sys.argv
    res = classify(load(TEP[0]))
    print('So dong se dien: %d' % len(res))
    for p in TEP:
        va(p, res, that)
    if not that:
        print('(CHAY THU - them --that de ghi that)')
