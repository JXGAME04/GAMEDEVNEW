# -*- coding: utf-8 -*-
r"""dongbo_wauto_co.py - DONG BO CO sau khi nan toa do.

Nan theo MEP giu duoc lien ke tuyet doi, nhung phai tra gia: hai dieu khien CUNG VAI TRO
o hai hang khac nhau co the ra chieu cao lech 1 dlu (vi round(y+h) - round(y) khong deu).
Do lai sau dot 11:
    333 o cao 10 dlu  -> 157 cai ra 18 px, 176 cai ra 20 px
    22 nut cao 12 dlu -> 6 cai 21 px, 16 cai 23 px
    29 vach ke h=1    -> 24 cai 2 px, 5 cai 3 px   (de thay nhat: vach day mong khong deu)
    39 o EDITTEXT     -> 18 px, ma vien 2 px + chu 13 px = 17 px, du 1 px: dau tieng Viet
                        chong tang ("Ộ", "ế") cham mep

Script nay EP chieu cao ve gia tri chuan cua tung nhom (giu nguyen x, y, w da nan):
    h_cu = 1  -> 1     (vach ke)
    h_cu = 10 -> 12    (chu / o tich / o nhap / combo dong)
    h_cu = 11 -> 13
    h_cu = 12 -> 14    (nut)
    con lai   -> round(h_cu * 15/13)
Doc h_cu tu ban sao .truoc_nan de biet dung vai tro goc.

AN TOAN: ep xong thi KIEM lai, cho nao lam CHONG hang duoi thi TRA VE gia tri cu.

Chay: python dongbo_wauto_co.py [--thu]
"""
import io
import os
import re
import shutil
import sys

RC = r"E:\Src_Auto_Ngoai\WAuto\WAuto\WAuto.rc"
GOC = RC + ".truoc_nan"
MIRROR = r"D:\GAMEDEVNEW\WAutoUI\WAuto.rc"
THU = "--thu" in sys.argv

CHUAN = {1: 1, 10: 12, 11: 13, 12: 14}


def r15(v):
    return int(round(v * 15.0 / 13.0))


def quet(p):
    """Tra ve danh sach (chi_so_dong, idc, x, y, w, h) + list dong."""
    s = io.open(p, encoding="utf-16", newline="").read()
    s = s.replace("\r\n", "\n").replace("\n", "\r\n")
    L = s.split("\r\n")
    ra = []
    for i, l in enumerate(L):
        m = re.match(r'^\s*(?:LTEXT|RTEXT|CTEXT|PUSHBUTTON|DEFPUSHBUTTON|GROUPBOX)\s+".*?"\s*,\s*'
                     r'([A-Za-z_0-9]+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)', l)
        if not m:
            m = re.match(r'^\s*(?:EDITTEXT|COMBOBOX|LISTBOX|SCROLLBAR)\s+([A-Za-z_0-9]+)\s*,\s*'
                         r'(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)', l)
        if not m:
            m = re.match(r'^\s*CONTROL\s+".*?"\s*,\s*([A-Za-z_0-9]+)\s*,.*?,\s*'
                         r'(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*$', l)
        if m:
            ra.append((i, m.group(1), int(m.group(2)), int(m.group(3)), int(m.group(4)), int(m.group(5))))
    return L, ra


def main():
    if not os.path.exists(GOC):
        print("!! khong thay %s - phai chay sau nan_wauto_dlu.py" % GOC)
        sys.exit(1)
    _, dsG = quet(GOC)
    hCu = {}
    for _, idc, x, y, w, h in dsG:
        hCu[idc] = h

    L, ds = quet(RC)
    # ban do y-dinh de kiem chong: voi moi dieu khien, hang xom NGAY DUOI cung cot
    def tab(idc):
        m = re.match(r"IDC_[A-Z]+_(\d+)_", idc)
        return m.group(1) if m else None

    def chong(idc_bo_qua, x, y, w, h):
        """CHI xet dieu khien CUNG TAB: 17 tab von xep chong len nhau roi an/hien
        theo dai ID, nen so ca tep se ra hang tram cap chong AO va chan het viec ep."""
        t = tab(idc_bo_qua)
        for _, i2, x2, y2, w2, h2 in ds:
            if i2 == idc_bo_qua or tab(i2) != t:
                continue
            if x < x2 + w2 and x2 < x + w and y < y2 + h2 and y2 < y + h:
                return i2
        return None

    doi = 0
    giu = 0
    for i, idc, x, y, w, h in ds:
        g = hCu.get(idc)
        if g is None:
            continue
        moi = CHUAN.get(g, r15(g))
        if moi == h:
            continue
        if moi > h and chong(idc, x, y, w, moi):
            giu += 1
            continue
        l = L[i]
        # thay so CUOI cung (chieu cao) cua dong
        m = re.match(r'^(.*?)(-?\d+)(\s*(?:,.*)?)$', l[::-1])
        # de an toan: thay theo mau "…, w, h" o cuoi phan toa do
        m2 = re.search(r'(,\s*)(%d)(\s*,\s*)(%d)(\s*)(,|$)' % (w, h), l)
        if not m2:
            continue
        L[i] = l[:m2.start(4)] + str(moi) + l[m2.end(4):]
        doi += 1

    print("Da ep chieu cao cho %d dieu khien; giu nguyen %d cai (ep se lam chong hang duoi)" % (doi, giu))
    if THU:
        print("(--thu: khong ghi)")
        return
    io.open(RC, "w", encoding="utf-16", newline="").write("\r\n".join(L))
    if os.path.isdir(os.path.dirname(MIRROR)):
        shutil.copyfile(RC, MIRROR)
    print("da ghi WAuto.rc (+ mirror)")


if __name__ == "__main__":
    main()
