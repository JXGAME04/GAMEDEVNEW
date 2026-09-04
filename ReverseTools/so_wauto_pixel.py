# -*- coding: utf-8 -*-
r"""so_wauto_pixel.py - CHUNG MINH viec nan toa do la DUNG: so tung o ra PIXEL.

Y tuong: mot o o WAuto.rc chi la con so dlu; cai NGUOI DUNG NHIN THAY la pixel.
    px = dlu * base / 4 (ngang) hoac dlu * base / 8 (doc)
Sau khi doi font + nan toa do, PIXEL cua tung o phai GIU NGUYEN (lech toi da 1-2 px
do lam tron). Script nay doc hai ban .rc, quy ca hai ra pixel bang dung font cua chinh
no, roi so tung dieu khien theo ID.

Chay: python so_wauto_pixel.py <rc_cu> <rc_moi>
"""
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools")
from do_wauto_bo_cuc import Do, doc_rc


def bang(p):
    ten, pt, ds = doc_rc(p)
    d = Do(ten, pt)
    out = {}
    for hop, kind, nhan, idc, x, y, w, h in ds:
        out[(hop, idc)] = (d.px_x(x), d.px_y(y), d.px_x(w), d.px_y(h), kind, nhan)
    return ten, pt, d, out


def main():
    if len(sys.argv) < 3:
        print("dung: so_wauto_pixel.py <rc_cu> <rc_moi>")
        sys.exit(1)
    ten1, pt1, d1, a = bang(sys.argv[1])
    ten2, pt2, d2, b = bang(sys.argv[2])
    print('CU : FONT %d "%s"  base %d x %d' % (pt1, ten1, d1.baseX, d1.baseY))
    print('MOI: FONT %d "%s"  base %d x %d' % (pt2, ten2, d2.baseX, d2.baseY))
    print("so dieu khien: cu %d, moi %d\n" % (len(a), len(b)))

    thieu = [k for k in a if k not in b]
    thua = [k for k in b if k not in a]
    if thieu:
        print("!! CO trong ban cu ma MAT o ban moi: %s" % [k[1] for k in thieu][:10])
    if thua:
        print("!! CHI co o ban moi: %s" % [k[1] for k in thua][:10])

    hist = {}
    xau = []
    for k in a:
        if k not in b:
            continue
        x1, y1, w1, h1, kind, nhan = a[k]
        x2, y2, w2, h2, _, _ = b[k]
        for ten, v1, v2 in (("x", x1, x2), ("y", y1, y2), ("w", w1, w2), ("h", h1, h2)):
            dd = abs(v1 - v2)
            hist[dd] = hist.get(dd, 0) + 1
            if dd > 2:
                xau.append((dd, k[1], ten, v1, v2, nhan))

    print("=== LECH PIXEL (gop ca x, y, w, h cua moi dieu khien) ===")
    for dd in sorted(hist):
        print("   lech %d px : %5d gia tri" % (dd, hist[dd]))
    tong = sum(hist.values())
    ok = sum(v for k, v in hist.items() if k <= 2)
    print("   -> %d/%d gia tri (%.2f %%) lech <= 2 px" % (ok, tong, 100.0 * ok / tong))

    if xau:
        print("\n=== CHO LECH > 2 px (phai xem lai) ===")
        for dd, idc, ten, v1, v2, nhan in sorted(xau, reverse=True)[:30]:
            print("   %-22s %s: %4d -> %4d px (lech %d)  %s" % (idc, ten, v1, v2, dd, nhan[:26]))
    else:
        print("\nKHONG co cho nao lech qua 2 px - viec nan DUNG.")


if __name__ == "__main__":
    main()
