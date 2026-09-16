# -*- coding: utf-8 -*-
r"""[TENTEP 16/09] Doi ten tep/thu muc trong mot kho du lieu Android/iOS ve TEN CHUAN cua game.

Ten chuan = cach JxPathPosix (Sources/Engine/Src/Platform/KPosixWin32.cpp) ha chu thuong khi mo tep: A-Z; Latin-1 U+00C0..00DE
(tru U+00D7) +0x20; S/OE/Z hoa (U+0160/0152/017D) +1; Y hoa U+0178 -> y (U+00FF); roi NFC. Ten GBK tren PC doc kieu cp1252 la
chu Latin-1 hoa -> he tep phan biet hoa/thuong (Android <= 10 sdcardfs voi ky tu ngoai ASCII, iOS APFS) khong tim thay.

Dung:  python android\ha_ten_theo_game.py <thu muc kho> [--thu]        (--thu: chi in, khong doi)
  vd:  python android\ha_ten_theo_game.py D:\jx1_android_data_dt_v4
       python android\ha_ten_theo_game.py D:\jx1_android_data
Doi tep truoc, thu muc sau (sau nhat truoc); NTFS khong phan biet hoa/thuong nen doi qua ten tam. Bo qua userdata/ va apdata/.
Sau do: python android\may_chu_tai_du_lieu.py --thu-muc <kho> --chi-manifest
"""
import os
import sys
import unicodedata

BO_QUA = ("userdata", "apdata", "capnhat")


def chuan_hoa(s):
    out = []
    for ch in unicodedata.normalize("NFC", s):
        o = ord(ch)
        if 65 <= o <= 90:
            out.append(chr(o + 32))
        elif 0xC0 <= o <= 0xDE and o != 0xD7:
            out.append(chr(o + 0x20))
        elif o in (0x160, 0x152, 0x17D):
            out.append(chr(o + 1))
        elif o == 0x178:
            out.append(chr(0xFF))
        else:
            out.append(ch)
    return unicodedata.normalize("NFC", "".join(out))


def doi_ten(cha, ten, moi, thu_nghiem):
    cu = os.path.join(cha, ten)
    dich = os.path.join(cha, moi)
    if thu_nghiem:
        print("  se doi: %s -> %s" % (cu, moi))
        return True
    if os.path.exists(dich) and os.path.normcase(cu) != os.path.normcase(dich):
        # da co muc ten chuan that su khac (khong phai cung mot muc tren NTFS): giu ca hai, bao de xu ly tay
        print("  !! TRUNG: %s da co, khong doi %s" % (dich, cu))
        return False
    tam = cu + ".__tentam__"
    os.rename(cu, tam)
    os.rename(tam, dich)
    return True


def main():
    a = [x for x in sys.argv[1:] if not x.startswith("--")]
    thu_nghiem = "--thu" in sys.argv
    if not a:
        raise SystemExit(__doc__)
    goc = a[0]
    if not os.path.isdir(goc):
        raise SystemExit("khong co thu muc: " + goc)
    doi_tep = doi_thu = xet = 0
    thu_muc = []
    for root, ds, fs in os.walk(goc):
        rel = os.path.relpath(root, goc)
        if rel == ".":
            ds[:] = [d for d in ds if d.lower() not in BO_QUA]
        for f in fs:
            xet += 1
            moi = chuan_hoa(f)
            if moi != f and doi_ten(root, f, moi, thu_nghiem):
                doi_tep += 1
        for d in ds:
            thu_muc.append(os.path.join(root, d))
    # thu muc: sau nhat truoc de duong dan cha con nguyen khi doi
    for p in sorted(thu_muc, key=lambda x: -len(x)):
        cha, ten = os.path.split(p)
        moi = chuan_hoa(ten)
        if moi != ten and doi_ten(cha, ten, moi, thu_nghiem):
            doi_thu += 1
    print("%s: xet %d tep; doi ten %d tep, %d thu muc%s" % (goc, xet, doi_tep, doi_thu, " (chi in)" if thu_nghiem else ""))


if __name__ == "__main__":
    main()
