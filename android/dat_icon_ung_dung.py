# -*- coding: utf-8 -*-
r"""[ICON 14/09] Dat anh icon cua chu lam icon ung dung Android.

Chu 19:40 dua anh 1254x1254 (JX MOBILE) vao D:\GAMEDEVNEW_wt_mobile. Kich ban nay:
  1. Thu nho ve nam co mipmap chuan (mdpi 48, hdpi 72, xhdpi 96, xxhdpi 144, xxxhdpi 192) bang bo loc
     LANCZOS, ghi de app/src/main/res/mipmap-*/ic_launcher.png.
  2. Sinh THEM icon thich ung (adaptive, Android 8 tro len): lop truoc 432x432 dat anh trong vung an toan
     (66 % giua - Android cat tron / vuong / giot nuoc tuy may), lop sau la mau nen lay tu goc anh.
     -> mipmap-anydpi-v26/ic_launcher.xml + drawable/ic_launcher_nen.xml + mipmap-*/ic_launcher_truoc.png
  3. Giu nguyen ic_launcher.png cho may Android 7 tro xuong.

Chay:  python android\dat_icon_ung_dung.py [<duong dan anh>]
Kiem:  anh ra o cac thu muc mipmap-*, va AndroidManifest van tro @mipmap/ic_launcher (khong phai sua).
"""
import io
import os
import sys

try:
    from PIL import Image
except ImportError:
    raise SystemExit("can Pillow: python -m pip install pillow")

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RES = os.path.join(GOC, "android", "gradle-project", "app", "src", "main", "res")
ANH_MAC_DINH = r"D:\GAMEDEVNEW_wt_mobile\d3dece91-e9c8-42ed-9cfe-37662d2361f3.png"
CO = {"mdpi": 48, "hdpi": 72, "xhdpi": 96, "xxhdpi": 144, "xxxhdpi": 192}
# lop truoc cua icon thich ung: 108dp, trong do 72dp o giua la vung an toan -> ti le anh = 72/108
CO_TRUOC = {"mdpi": 108, "hdpi": 162, "xhdpi": 216, "xxhdpi": 324, "xxxhdpi": 432}
AN_TOAN = 72.0 / 108.0


def main():
    nguon = sys.argv[1] if len(sys.argv) > 1 else ANH_MAC_DINH
    if not os.path.exists(nguon):
        raise SystemExit("khong thay anh: %s" % nguon)
    goc = Image.open(nguon).convert("RGBA")
    print("anh nguon: %s (%dx%d)" % (nguon, goc.size[0], goc.size[1]))

    # 1. icon co dien (may cu + noi nao khong dung adaptive)
    for ten, px in CO.items():
        d = os.path.join(RES, "mipmap-%s" % ten)
        if not os.path.isdir(d):
            os.makedirs(d)
        goc.resize((px, px), Image.LANCZOS).save(os.path.join(d, "ic_launcher.png"))
        print("  ic_launcher.png %-8s %dx%d" % (ten, px, px))

    # 2. lop truoc cho icon thich ung: anh nam trong vung an toan, quanh la trong suot
    for ten, px in CO_TRUOC.items():
        d = os.path.join(RES, "mipmap-%s" % ten)
        trong = int(round(px * AN_TOAN))
        nen = Image.new("RGBA", (px, px), (0, 0, 0, 0))
        nen.paste(goc.resize((trong, trong), Image.LANCZOS), ((px - trong) // 2, (px - trong) // 2))
        nen.save(os.path.join(d, "ic_launcher_truoc.png"))
        print("  ic_launcher_truoc.png %-3s %dx%d (anh %dx%d o giua)" % (ten, px, px, trong, trong))

    # 3. mau nen = mau trung binh cua vien anh (goc anh la khung vang toi)
    nho = goc.convert("RGB").resize((32, 32), Image.LANCZOS)
    px32 = nho.load()
    vien = [px32[x, y] for x in range(32) for y in range(32) if x < 2 or y < 2 or x > 29 or y > 29]
    r = sum(c[0] for c in vien) // len(vien)
    g = sum(c[1] for c in vien) // len(vien)
    b = sum(c[2] for c in vien) // len(vien)
    mau = "#%02X%02X%02X" % (r, g, b)
    print("  mau nen lop sau: %s" % mau)

    d = os.path.join(RES, "values")
    if not os.path.isdir(d):
        os.makedirs(d)
    io.open(os.path.join(d, "ic_launcher_mau.xml"), "w", encoding="utf-8", newline="\n").write(
        '<?xml version="1.0" encoding="utf-8"?>\n'
        '<resources>\n'
        '    <!-- [ICON 14/09] mau nen lop sau cua icon thich ung, lay tu vien anh icon -->\n'
        '    <color name="ic_launcher_nen">%s</color>\n'
        '</resources>\n' % mau)

    d = os.path.join(RES, "mipmap-anydpi-v26")
    if not os.path.isdir(d):
        os.makedirs(d)
    xml = ('<?xml version="1.0" encoding="utf-8"?>\n'
           '<!-- [ICON 14/09] icon thich ung: Android 8+ tu cat theo hinh cua tung may -->\n'
           '<adaptive-icon xmlns:android="http://schemas.android.com/apk/res/android">\n'
           '    <background android:drawable="@color/ic_launcher_nen" />\n'
           '    <foreground android:drawable="@mipmap/ic_launcher_truoc" />\n'
           '</adaptive-icon>\n')
    io.open(os.path.join(d, "ic_launcher.xml"), "w", encoding="utf-8", newline="\n").write(xml)
    io.open(os.path.join(d, "ic_launcher_round.xml"), "w", encoding="utf-8", newline="\n").write(xml)
    print("  mipmap-anydpi-v26/ic_launcher.xml + ic_launcher_round.xml")
    print("xong. AndroidManifest da tro @mipmap/ic_launcher nen khong phai sua.")


if __name__ == "__main__":
    main()
