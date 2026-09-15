# -*- coding: utf-8 -*-
"""Dong nay co THAT SU duoc bien dich khong, hay dang nam trong khoi chu thich?

Vi sao can: kho nay day khoi /* */ bao ca than ham (vi du KScenePlaceMapC::AutoRunTo mo bang "{/*"),
nen cau hoi "loi goi nay con song khong" xuat hien luon luc go ma chet. Cach hay lam - grep roi tim
dau /* gan nhat phia tren - SAI trong ba ca that co trong kho nay:
  - dau mo nam ngay sau dau ngoac nhon ("{/*") nen khong cung dong voi ten ham;
  - trong chuoi co the co "/*" hay "//" (duong dan, chuoi dinh dang) lam lech phep dem;
  - mot dong co the vua co ma song vua co chu thich duoi.
Bo nay quet TU DAU TEP, co hieu chuoi "..." va ky tu '...' va dau thoat, nen tra loi dung.

Dung:
    python android/kiem_dong_song.py <tep> <dong> [dong ...]
    python android/kiem_dong_song.py <tep> --mau "g_JXPathFinder"     # moi dong chua mau
Vi du (14/09, soi ban va [BANDONHO] cua phien do nhip):
    python android/kiem_dong_song.py Sources/Core/Src/CoreShell.cpp 27302
    -> 27302  CHET (trong /* */)
"""
import io
import os
import sys


def trang_thai_tung_dong(duong_dan):
    """Tra ve dict {so_dong: (co_ma_song, co_nam_trong_khoi_chu_thich)}.

    Mot dong "co ma song" khi co it nhat mot ky tu khong phai khoang trang nam ngoai moi chu thich.
    Tach rieng co thu hai de KHONG gan nhan "chet trong /* */" cho mot dong von chi la chu thich //
    hay dong trong - nhan sai kieu do chinh la thu de danh lua nguoi doc.
    """
    s = io.open(duong_dan, encoding='latin-1', newline='').read()
    n = len(s)
    dong = 1
    co_ma = {1: False}
    trong_kh = {1: False}       # dong nay co phan nao nam trong /* */ khong
    trong_khoi = False          # dang trong /* */
    i = 0
    while i < n:
        c = s[i]
        if c == '\n':
            dong += 1
            co_ma.setdefault(dong, False)
            trong_kh.setdefault(dong, trong_khoi)
            i += 1
            continue
        if trong_khoi:
            trong_kh[dong] = True
            if c == '*' and i + 1 < n and s[i + 1] == '/':
                trong_khoi = False
                i += 2
                continue
            i += 1
            continue
        # ngoai chu thich
        if c == '/' and i + 1 < n and s[i + 1] == '*':
            trong_khoi = True
            trong_kh[dong] = True
            i += 2
            continue
        if c == '/' and i + 1 < n and s[i + 1] == '/':
            while i < n and s[i] != '\n':
                i += 1
            continue
        if c in '"\'':
            co_ma[dong] = True
            dau = c
            i += 1
            while i < n and s[i] != dau:
                if s[i] == '\\':          # dau thoat: bo qua ky tu ke tiep
                    i += 2
                    continue
                if s[i] == '\n':          # chuoi khong dong (hiem): dung lai cho an toan
                    break
                i += 1
            if i < n and s[i] == dau:
                i += 1
            continue
        if not c.isspace():
            co_ma[dong] = True
        i += 1
    return co_ma, trong_kh


def main():
    if len(sys.argv) < 3:
        raise SystemExit(__doc__)
    tep = sys.argv[1]
    if not os.path.isfile(tep):
        raise SystemExit('khong thay tep: %s' % tep)
    co_ma, trong_kh = trang_thai_tung_dong(tep)
    cac_dong = io.open(tep, encoding='latin-1', newline='').read().split('\n')

    if sys.argv[2] == '--mau':
        if len(sys.argv) < 4:
            raise SystemExit('thieu mau sau --mau')
        mau = sys.argv[3]
        can = [i + 1 for i, ln in enumerate(cac_dong) if mau in ln]
        if not can:
            print('khong dong nao chua %r' % mau)
            return
    else:
        can = [int(x) for x in sys.argv[2:]]

    for d in can:
        if d < 1 or d > len(cac_dong):
            print('%-7d ngoai tep (%d dong)' % (d, len(cac_dong)))
            continue
        if co_ma.get(d, False):
            nhan = 'SONG (duoc bien dich)'
        elif trong_kh.get(d, False):
            nhan = 'CHET (trong /* */)'
        else:
            nhan = 'chi chu thich // / dong trong'
        print('%-7d %-30s %s' % (d, nhan, cac_dong[d - 1].strip()[:84]))


if __name__ == '__main__':
    main()
