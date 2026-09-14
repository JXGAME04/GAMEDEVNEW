# -*- coding: utf-8 -*-
r"""[DONET 14/09] Do DO NET cua canh tren may ao (LDPlayer emulator-5554) theo phep A/B da chot voi phien do nhip sau vu
"npc voi player bi mo" (TGNAC 14/09): moi thay doi dung toi HINH ANH phai do TRUOC khi len dt_v4 - 2 phut, con chu nhan ra
mo trong vai phut va mat ca buoi choi.

Cach do: RMS cua Laplacian (nang luong tan so cao) tren mot HOP CO DINH cua anh chup, cung canh, cung muc zoom (ZoomThu),
chi doi MOT khoa trong D:\jx1_android_data\config.ini giua hai luot. So lon hon = net hon. Moc 14/09 (hop 0,250-700,560,
ZoomThu=125): TGNAC bat 35,8 | tat 52,9 | ban sach 109141607 53,2.

Dung:
  python android\do_net_ab.py --khoa TheGioiRTNac --gia-tri 0 1100          # do hai luot, in bang so
  python android\do_net_ab.py --khoa TheGioiRTNac --gia-tri 0 --zoom 150    # mot luot o zoom 150 %
  python android\do_net_ab.py --anh a.png b.png                             # chi tinh so tren anh co san (khong dung may ao)
Tham so khac: --hop x0,y0,x1,y1 (mac dinh 0,250,700,560), --zoom (ZoomThu, mac dinh 125), --luu <thu muc>.
Yeu cau: adb thay emulator-5554, app da cai, tai khoan da nho mat ma (chuoi cham dang nhap nhu cac kich ban khac).
"""
import argparse
import io
import os
import re
import subprocess
import sys
import time

DEV = 'emulator-5554'
CFG = r'D:\jx1_android_data\config.ini'
HOP = (0, 250, 700, 560)
# chuoi cham dang nhap (giong cac kich ban thu khac): (cho truoc khi cham, x, y)
DANG_NHAP = [(14, 520, 120), (6, 450, 468), (5, 520, 117), (1, 450, 468), (8, 420, 368), (15, 262, 530)]
CHO_VAO_THE_GIOI = 36


def adb(*a, **kw):
    return subprocess.run(['adb', '-s', DEV] + list(a), capture_output=True, **kw)


def dat_khoa(khoa, gia_tri):
    """Doi mot khoa trong config may ao (byte-safe latin-1). Tra ve gia tri cu."""
    s = io.open(CFG, encoding='latin-1', newline='').read()
    m = re.search(r'(?m)^%s=(\S*)' % re.escape(khoa), s)
    if not m:
        raise SystemExit('khong thay khoa %s trong %s' % (khoa, CFG))
    cu = m.group(1)
    io.open(CFG, 'w', encoding='latin-1', newline='').write(s[:m.start()] + '%s=%s' % (khoa, gia_tri) + s[m.end():])
    return cu


def chay_va_chup(duong_dan):
    adb('shell', 'am force-stop vn.jx1.mobile')
    time.sleep(4)
    adb('logcat', '-c')
    adb('shell', 'am start -n vn.jx1.mobile/.TaiDuLieuActivity')
    for cho, x, y in DANG_NHAP:
        time.sleep(cho)
        adb('shell', 'input tap %d %d' % (x, y))
    time.sleep(CHO_VAO_THE_GIOI)
    r = subprocess.run(['adb', '-s', DEV, 'exec-out', 'screencap', '-p'], capture_output=True)
    open(duong_dan, 'wb').write(r.stdout)
    return duong_dan


def do_net(duong_dan, hop):
    """RMS cua Laplacian 4 lang gieng tren anh xam trong hop."""
    import numpy as np
    from PIL import Image
    a = np.asarray(Image.open(duong_dan).convert('L'), dtype=np.float64)
    x0, y0, x1, y1 = hop
    x1 = min(x1, a.shape[1]); y1 = min(y1, a.shape[0])
    v = a[y0:y1, x0:x1]
    if v.shape[0] < 3 or v.shape[1] < 3:
        raise SystemExit('hop qua nho / ngoai anh')
    lap = (4.0 * v[1:-1, 1:-1] - v[:-2, 1:-1] - v[2:, 1:-1] - v[1:-1, :-2] - v[1:-1, 2:])
    return float((lap ** 2).mean() ** 0.5)


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--khoa')
    p.add_argument('--gia-tri', nargs='+')
    p.add_argument('--zoom', type=int, default=125)
    p.add_argument('--hop', default=','.join(str(n) for n in HOP))
    p.add_argument('--anh', nargs='+')
    p.add_argument('--luu', default=os.environ.get('TEMP', '.'))
    a = p.parse_args()
    hop = tuple(int(n) for n in a.hop.split(','))
    if a.anh:
        for t in a.anh:
            print('%-40s %6.1f' % (os.path.basename(t), do_net(t, hop)))
        return
    if not a.khoa or not a.gia_tri:
        p.error('can --khoa va --gia-tri (hoac --anh)')
    zoom_cu = dat_khoa('ZoomThu', a.zoom)
    khoa_cu = None
    kq = []
    try:
        for gt in a.gia_tri:
            khoa_cu = dat_khoa(a.khoa, gt) if khoa_cu is None else dat_khoa(a.khoa, gt) or khoa_cu
            t = os.path.join(a.luu, 'donet_%s_%s.png' % (a.khoa, gt))
            chay_va_chup(t)
            kq.append((gt, do_net(t, hop), t))
            print('%s=%-6s do net %6.1f   (%s)' % (a.khoa, gt, kq[-1][1], t))
    finally:
        dat_khoa('ZoomThu', zoom_cu)
        if khoa_cu is not None:
            dat_khoa(a.khoa, khoa_cu)
        print('(da tra config: ZoomThu=%s, %s=%s)' % (zoom_cu, a.khoa, khoa_cu))
    if len(kq) > 1:
        goc = max(k[1] for k in kq)
        print('\n%-14s %8s %10s' % (a.khoa, 'do net', 'so voi net nhat'))
        for gt, n, _ in kq:
            print('%-14s %8.1f %9.1f %%' % (gt, n, 100.0 * (n - goc) / goc))


if __name__ == '__main__':
    main()
