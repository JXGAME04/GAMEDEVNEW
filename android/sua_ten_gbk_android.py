# -*- coding: utf-8 -*-
r"""[ANDROID 11/09 TENGBK] Sua ten tep/thu muc GBK bi doi khi ha chu thuong trong D:\jx1_android_data.

Loi: chuan_bi_du_lieu.ps1 ha chu thuong bang .ToLowerInvariant() tren ten Unicode. Ten GBK (byte 0x81..0xFE) khi Windows doc bang
cp1252 thanh cac chu Latin-1 'Ö' 'Ï' 'É' 'Í' 'Ø'... -> bi ha thanh 'ö' 'ï' 'é' 'í' 'ø' -> byte tren dia doi (0xD6 -> 0xF6...). Game
xin tep bang byte GBK goc (JxPathPosix doi cp1252 -> UTF-8 dung ten goc) -> khong thay tep: anh vat pham, hieu ung chieu, ban do...
(do 11/09: spr 394/989, settings 109/145, maps 1136/2384 ten GBK bi doi).

Cach sua: duyet cay PC (chi doc), voi moi ten co byte > 127 tinh ten mong doi ben Android = ha chu thuong CHI A-Z, giu nguyen
byte > 127; tim muc ben Android khop khong phan biet hoa/thuong (casefold ca Latin-1) roi doi ten ve dung. Doi ten tu SAU ra TRUOC
(tep truoc, thu muc sau, sau nhat truoc). Chay lai vo hai. Dung:  python android\sua_ten_gbk_android.py [--thu]
"""
import os
import sys

PC = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
AN = r"D:\jx1_android_data"
THU_MUC = ("spr", "data", "maps", "maps2", "script", "settings", "ui", "userdata", "sound", "music")


def ha_ascii(s):
    return "".join(c.lower() if ord(c) < 128 else c for c in s)


def sua(thu, thu_nghiem):
    goc_pc = os.path.join(PC, thu)
    if not os.path.isdir(goc_pc):
        return 0, 0
    doi = 0; xet = 0
    # duyet tu sau ra truoc de doi ten con truoc, cha sau
    for root, ds, fs in os.walk(goc_pc, topdown=False):
        rel = os.path.relpath(root, PC)
        goc_an = os.path.join(AN, ha_ascii(rel))
        # thu muc cha ben Android co the dang mang ten sai -> tim theo casefold
        if not os.path.isdir(goc_an):
            cha, ten = os.path.split(goc_an)
            if os.path.isdir(cha):
                for x in os.listdir(cha):
                    if x.lower() == ten.lower() and os.path.isdir(os.path.join(cha, x)):
                        goc_an = os.path.join(cha, x); break
        if not os.path.isdir(goc_an):
            continue
        try:
            co = os.listdir(goc_an)
        except Exception:
            continue
        co_low = {x.lower(): x for x in co}
        for ten in list(fs) + list(ds):
            if not any(ord(c) > 127 for c in ten):
                continue
            xet += 1
            mong = ha_ascii(ten)
            if mong in co:
                continue
            x = co_low.get(mong.lower())
            if x is None or x == mong:
                continue
            a, b = os.path.join(goc_an, x), os.path.join(goc_an, mong)
            doi += 1
            if thu_nghiem:
                if doi <= 5:
                    print("  se doi:", a.encode("latin-1", "replace"), "->", mong.encode("latin-1", "replace"))
                continue
            tmp = a + ".__tmp__"
            os.rename(a, tmp)
            os.rename(tmp, b)
            co.remove(x); co.append(mong); co_low[mong.lower()] = mong
    return xet, doi


if __name__ == "__main__":
    thu_nghiem = "--thu" in sys.argv
    tong = 0
    for t in THU_MUC:
        xet, doi = sua(t, thu_nghiem)
        if xet:
            print("%-9s xet %6d ten GBK, %s %5d" % (t, xet, "se doi" if thu_nghiem else "da doi", doi))
        tong += doi
    print("tong", "se doi" if thu_nghiem else "da doi", tong)
