# -*- coding: utf-8 -*-
"""Quet ca cay: AUTOLOG_EVERY / AUTOLOG_IDX_EVERY nam TRONG than vong lap.

Ly do: AUTOLOG_EVERY / AUTOLOG_IDX_EVERY o Core/Src/KCore.h co HAI nhanh.
  - Nhanh PC / may chu (#else): goi timeGetTime() NGAY SAU phep thu log-bat, truoc phep so ms
    -> khi log dang bat thi MOI LAN danh gia deu ton mot loi goi dong ho, ke ca khi khong ghi gi.
    Dat trong than vong lap (quet NPC / quet obj / quet o tui) la dat mot phep goi dong ho vao
    duong nong.
  - Nhanh JX_MOBILE: KHONG goi dong ho, so voi g_uAutoLogNow (cap nhat mot lan moi tick). Da sua
    tu 11/09 ([DAN 11/09 b]) sau vu tut fps Fold 7. Nen ket qua bo quet nay la viec cua PC / may
    chu, KHONG phai lo cho dien thoai nua. Dung ket luan nguoc.
Xem bo nho autolog-every-khong-trong-vong-lap.

BAN DAU TOI VIET SAI: chi coi mot dong la "mo vong lap" khi dau { nam CUNG DONG voi for/while.
Trong ma nay dau { hay xuong dong rieng ("while (nObj)" roi "{"), nen bo quet bao 0 ngay tren doan
DA BIET la sai (CoreShell.cpp:7169 truoc ban r7). Ban nay tach han hai buoc: bo chu thich + chuoi,
roi bam theo dau ngoac that.

Bat duoc ca hai kieu than vong: khoi { } va than MOT LENH khong ngoac.
Dung: python quet_autolog_vong.py <goc>        (goc la thu muc chua Sources/)
"""
import io
import os
import re
import sys

GOC = sys.argv[1] if len(sys.argv) > 1 else '.'
PAT = re.compile(r'\bAUTOLOG(?:_IDX)?_EVERY\s*\(')
TU_VONG = re.compile(r'\b(for|while|do)\b')


def bo_chu_thich(s):
    """Thay chu thich va noi dung chuoi/ky tu bang dau cach, GIU nguyen so ky tu va so dong."""
    ra = list(s)
    i, n = 0, len(s)
    while i < n:
        c = s[i]
        if c == '/' and i + 1 < n and s[i + 1] == '/':
            while i < n and s[i] != '\n':
                ra[i] = ' '; i += 1
        elif c == '/' and i + 1 < n and s[i + 1] == '*':
            ra[i] = ra[i + 1] = ' '; i += 2
            while i < n and not (s[i] == '*' and i + 1 < n and s[i + 1] == '/'):
                if s[i] != '\n':
                    ra[i] = ' '
                i += 1
            if i < n:
                ra[i] = ra[i + 1] = ' '; i += 2
        elif c in '"\'':
            dau = c; ra[i] = ' '; i += 1
            while i < n and s[i] != dau:
                if s[i] == '\\':
                    ra[i] = ' '
                    if i + 1 < n:
                        ra[i + 1] = ' '
                    i += 2
                    continue
                if s[i] != '\n':
                    ra[i] = ' '
                i += 1
            if i < n:
                ra[i] = ' '; i += 1
        else:
            i += 1
    return ''.join(ra)


def cho_trong_vong(s):
    """Tra ve tap chi so ky tu nam trong than mot vong lap."""
    sach = bo_chu_thich(s)
    n = len(sach)
    mo_vong = set()                       # chi so cua tu khoa for/while/do dang cho than
    for m in TU_VONG.finditer(sach):
        mo_vong.add(m.start())
    trong = [False] * n
    ngoac = 0                             # do sau ( )
    nhon = 0                              # do sau { }
    khoi_vong = []                        # do sau { } ma mot vong lap chiem
    than_mot_lenh = []                    # (do sau nhon) cua than mot lenh dang mo
    cho_than = False                      # vua doc xong dau vong, dang cho than
    trong_dau = False                     # dang trong phan ( ) cua for/while
    i = 0
    while i < n:
        c = sach[i]
        if i in mo_vong and ngoac == 0 and not trong_dau:
            tu = sach[i:i + 5]
            if tu.startswith('do'):
                cho_than = True
            else:
                trong_dau = True          # phai doc het ( ... ) da
        if c == '(':
            ngoac += 1
        elif c == ')':
            ngoac -= 1
            if trong_dau and ngoac == 0:
                trong_dau = False
                cho_than = True
        elif c == '{':
            nhon += 1
            if cho_than:
                khoi_vong.append(nhon); cho_than = False
        elif c == '}':
            while khoi_vong and khoi_vong[-1] > nhon - 1:
                khoi_vong.pop()
            while than_mot_lenh and than_mot_lenh[-1] > nhon - 1:
                than_mot_lenh.pop()
            nhon -= 1
        elif c == ';':
            if ngoac == 0:
                if cho_than:
                    cho_than = False      # "while (x);" hay than rong
                elif than_mot_lenh:
                    than_mot_lenh.pop()
        elif not c.isspace() and cho_than and ngoac == 0:
            than_mot_lenh.append(nhon); cho_than = False   # than MOT LENH khong ngoac
        trong[i] = bool(khoi_vong or than_mot_lenh)
        i += 1
    return sach, trong


def main():
    goc_src = os.path.join(GOC, 'Sources')
    if not os.path.isdir(goc_src):
        raise SystemExit('khong thay %s' % goc_src)
    dinh = []
    tep_co = 0
    for root, dirs, files in os.walk(goc_src):
        dirs[:] = [d for d in dirs if d.lower() not in ('.git', 'lib', 'bin', 'obj')]
        for f in files:
            if not f.endswith(('.cpp', '.h', '.inc')):
                continue
            p = os.path.join(root, f)
            try:
                s = io.open(p, encoding='latin-1', newline='').read()
            except Exception:
                continue
            if not PAT.search(s):
                continue
            tep_co += 1
            sach, trong = cho_trong_vong(s)
            for m in PAT.finditer(sach):
                dau_dong = sach.rfind(chr(10), 0, m.start()) + 1
                if sach[dau_dong:m.start()].lstrip().startswith('#'):
                    continue          # chinh dong #define macro: than no la do{}while(0), khong tinh
                if trong[m.start()]:
                    dong = s.count('\n', 0, m.start()) + 1
                    dau = s.rfind('\n', 0, m.start()) + 1
                    cuoi = s.find('\n', m.start())
                    dinh.append((p.replace(os.sep, '/'), dong, s[dau:cuoi].strip()[:100]))
    print('tep co macro: %d | so cho nam TRONG than vong lap: %d' % (tep_co, len(dinh)))
    for p, l, t in dinh:
        print('  %s:%d' % (p, l))
        print('      %s' % t)
    return len(dinh)


if __name__ == '__main__':
    main()
