# -*- coding: latin-1 -*-
r"""[KIEMGIAO 15/09] Kiem thu muc phat du lieu TRUOC KHI bao chu "ban da len".

Viet sau khi chinh toi lam chu vap loi "Tai tep cap nhat bi loi" luc 00:2x 15/09: toi de mot tep
0 BYTE trong manifest. TaiDuLieuActivity.taiMot() thay daCo == m.co == 0 nen BO QUA buoc tai, roi
part.renameTo(dich) that bai vi .part khong ton tai -> nem IOException. Mot tep 0 byte du de hong
ca dot cap nhat, va khong co bo kiem nao bat duoc.

Kiem 6 thu (moi thu deu la mot lan da that su vap):
  1. Muc 0 BYTE trong manifest            -> bo tai NEM LOI (bay 00:2x 15/09)
  2. Manifest lech tep tren dia (co/md5)  -> "Tai tep cap nhat bi loi" (bay 13/09, MEMORY dt-v4-manifest)
  3. Tep tren dia MOI HON manifest.txt    -> sinh manifest xong moi chep tep = cung loi tren
  4. apk.txt lech tep APK                 -> may tai APK hong
  5. May chu 8765 co tra dung noi dung    -> hai tien trinh cung listen, tra tu thu muc khac
  6. Tep trong manifest ma khong co tren dia

Chay:  python android\kiem_dt_v4.py [--thu-muc D:\jx1_android_data_dt_v4] [--url http://10.0.0.140:8765]
Ma thoat 0 = DAT, 1 = co loi.
"""
import argparse
import hashlib
import io
import os
import sys
import urllib.request


def md5_tep(p):
    h = hashlib.md5()
    with open(p, "rb") as f:
        while True:
            b = f.read(1 << 20)
            if not b:
                break
            h.update(b)
    return h.hexdigest()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--thu-muc", default=r"D:\jx1_android_data_dt_v4")
    ap.add_argument("--url", default="http://10.0.0.140:8765")
    ap.add_argument("--md5-day-du", action="store_true", help="bam md5 MOI tep (cham, ~8 GB)")
    a = ap.parse_args()
    goc = a.thu_muc
    loi = []

    p_man = os.path.join(goc, "manifest.txt")
    if not os.path.isfile(p_man):
        print("HONG: khong co %s" % p_man)
        return 1
    t_man = os.path.getmtime(p_man)
    dong = io.open(p_man, encoding="utf-8", newline="").read().splitlines()
    print("manifest: %d muc" % len(dong))

    moi_hon = []
    for L in dong:
        phan = L.split("\t")
        if len(phan) != 3:
            loi.append("dong sai dinh dang: %s" % L[:60])
            continue
        co, md5, duong = phan
        p = os.path.join(goc, duong.replace("/", os.sep))
        if co == "0":
            loi.append("MUC 0 BYTE (bo tai se nem loi): %s" % duong)
        if not os.path.isfile(p):
            loi.append("thieu tep tren dia: %s" % duong)
            continue
        that = os.path.getsize(p)
        if that != int(co):
            loi.append("co lech %d != %s: %s" % (that, co, duong))
        elif a.md5_day_du and md5_tep(p) != md5:
            loi.append("md5 lech: %s" % duong)
        if os.path.getmtime(p) > t_man + 1.0:
            moi_hon.append(duong)
    if moi_hon:
        loi.append("%d tep MOI HON manifest.txt (phai chay lai --chi-manifest): %s"
                   % (len(moi_hon), ", ".join(moi_hon[:4])))

    p_apk = os.path.join(goc, "apk.txt")
    if os.path.isfile(p_apk):
        s = io.open(p_apk, encoding="latin-1").read().split()
        if len(s) >= 4:
            ma, md5, co, ten = s[0], s[1], s[2], s[3]
            p = os.path.join(goc, ten)
            if not os.path.isfile(p):
                loi.append("apk.txt tro toi tep khong co: %s" % ten)
            else:
                if os.path.getsize(p) != int(co):
                    loi.append("apk.txt co lech %d != %s" % (os.path.getsize(p), co))
                if md5_tep(p) != md5:
                    loi.append("apk.txt md5 lech")
                print("apk.txt: ban %s, %s byte - khop tep" % (ma, co))
        else:
            loi.append("apk.txt sai dinh dang")

    if a.url:
        thu = [("/manifest.txt", os.path.getsize(p_man)), ("/apk.txt", None)]
        if dong:
            co0, _, d0 = dong[0].split("\t")
            thu.append(("/" + urllib.parse.quote(d0), int(co0)))
        for duong, co in thu:
            try:
                r = urllib.request.urlopen(a.url + duong, timeout=8)
                d = r.read()
                if co is not None and len(d) != co:
                    loi.append("may chu tra %s co %d, tren dia %d (may chu khac thu muc?)" % (duong, len(d), co))
                print("may chu %-46s HTTP %d  %d byte" % (duong, r.status, len(d)))
            except Exception as e:
                loi.append("khong lay duoc %s tu may chu: %s" % (duong, e))

    print("")
    if loi:
        print("KIEM GIAO: HONG - %d viec phai sua" % len(loi))
        for x in loi:
            print("   !! %s" % x)
        return 1
    print("KIEM GIAO: DAT - co the bao chu")
    return 0


if __name__ == "__main__":
    import urllib.parse  # noqa: F401  (dung trong main)
    sys.exit(main())
