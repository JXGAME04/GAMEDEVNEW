# -*- coding: utf-8 -*-
r"""v40 - SOAT bang atlas_compound.txt: ma vat pham co dung cua JX1 khong.

Bang nay la BAN SAO Y SO cua ban Linux (chi dich phan ten), nen moi ma vat pham
trong do dang la MA CUA LINUX. Ma Linux va ma JX1 TRUNG SO nhung KHAC NGHIA -
dung bay da vap voi Hinh nhan {6,1,1605} (o JX1 la "Thiep chuc su de").

Cach soat: voi tung tham chieu vat pham tren mot dong (co 8 cho: ATLAS, 6 o
nguyen lieu, DES), lay TEN o cot ben canh roi TRA NGUOC sang bang vat pham cua
JX1 de tim ma DUNG. So voi ma dang ghi trong bang.

Bang tra cua JX1:
  magicscript.txt : Name | Genre | DetailType | ParticularType | ...
  questkey.txt    : Name | Genre | DetailType | ...      (KHONG co ParticularType)
  goldequip.txt   : Ten  | ItemGenre | DetailType | ParticularType | ...

Bang atlas_compound.txt: 56 cot
   0- 3 ATLAS_NAME/GENRE/DETAILTYPE/PARTICULAR
   4-45 sau o nguyen lieu, moi o 7 cot NAME/GENRE/DETAILTYPE/PARTICULAR/LEVEL/SERIES/MAGIC_ID
  46-55 DES_NAME/QUALITY/GENRE/DETAILTYPE/PARTICULAR/LEVEL/SERIES/PIECE/PIECESUM/VALUES
"""
import io
import os
import sys
import importlib.util

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

HERE = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location("bangtxt", os.path.join(HERE, "bangtxt.py"))
bangtxt = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bangtxt)

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item"
LNX = r"D:\ServerLinux\server1\settings\item\004"

F_ATLAS = os.path.join(SRV, "atlas_compound.txt")
F_ATLAS_LNX = os.path.join(LNX, "atlas_compound.txt")


def doc_bang(p):
    d = io.open(p, "rb").read().decode("latin-1")
    return [ln.split("\t") for ln in d.replace("\r\n", "\n").split("\n") if ln.strip()]


def chuan(s):
    """chuan hoa ten de so: bo khoang trang thua, ha chu thuong"""
    return " ".join(bangtxt.tcvn2uni(s).split()).lower()


def nap_ten_jx1():
    """{ten chuan hoa: (nguon, genre, detail, particular)}"""
    ra = {}

    def them(nguon, ten, g, d, p):
        k = chuan(ten)
        if not k:
            return
        ra.setdefault(k, (nguon, g, d, p))

    b = doc_bang(os.path.join(SRV, "magicscript.txt"))
    for c in b[1:]:
        if len(c) > 3:
            them("magicscript", c[0], c[1], c[2], c[3])

    p = os.path.join(SRV, "questkey.txt")
    if os.path.isfile(p):
        b = doc_bang(p)
        for c in b[1:]:
            if len(c) > 2:
                # questkey KHONG co ParticularType - vat pham genre 4 dinh danh
                # bang (Genre, DetailType)
                them("questkey", c[0], c[1], c[2], "")

    p = os.path.join(SRV, "goldequip.txt")
    if os.path.isfile(p):
        b = doc_bang(p)
        for i, c in enumerate(b[1:]):
            if len(c) > 3:
                them("goldequip", c[0], c[1], c[2], c[3])
    return ra


# (ten cot, chi so cot ten, chi so genre, detail, particular)
CHO = [("ATLAS", 0, 1, 2, 3)]
for k in range(6):
    o = 4 + k * 7
    CHO.append(("NL%d" % (k + 1), o, o + 1, o + 2, o + 3))
CHO.append(("DES", 46, 48, 49, 50))


def main():
    b = doc_bang(F_ATLAS)
    print("bang JX1: %d dong (ke ca tieu de), %d cot" % (len(b), len(b[0])))

    # 1. co that la ban sao y so cua Linux khong
    if os.path.isfile(F_ATLAS_LNX):
        bl = doc_bang(F_ATLAS_LNX)
        khac = 0
        soanh = 0
        n = min(len(b), len(bl))
        for i in range(1, n):
            for _, ci, cg, cd, cp in CHO:
                for c in (cg, cd, cp):
                    if c < len(b[i]) and c < len(bl[i]):
                        soanh += 1
                        if b[i][c].strip() != bl[i][c].strip():
                            khac += 1
        print("so voi ban Linux: %d o SO doi chieu, %d o khac nhau  -> %s"
              % (soanh, khac,
                 "BAN SAO Y SO" if khac == 0 else "DA CO SUA"))
    print()

    ten_jx1 = nap_ten_jx1()
    print("bang tra JX1: %d ten rieng" % len(ten_jx1))
    print()

    khop = 0
    lech = {}
    khongco = {}
    for i in range(1, len(b)):
        c = b[i]
        for nhan, ci, cg, cd, cp in CHO:
            if ci >= len(c):
                continue
            ten = c[ci].strip()
            if not ten:
                continue
            g = c[cg].strip() if cg < len(c) else ""
            d = c[cd].strip() if cd < len(c) else ""
            p = c[cp].strip() if cp < len(c) else ""
            if not g:
                continue
            ma = "%s,%s,%s" % (g, d, p)
            k = chuan(ten)
            if k not in ten_jx1:
                khongco.setdefault((bangtxt.tcvn2uni(ten), ma), 0)
                khongco[(bangtxt.tcvn2uni(ten), ma)] += 1
                continue
            nguon, jg, jd, jp = ten_jx1[k]
            madung = "%s,%s,%s" % (jg, jd, jp)
            if ma == madung or (nguon == "questkey" and "%s,%s" % (g, d) == "%s,%s" % (jg, jd)):
                khop += 1
            else:
                khoa = (bangtxt.tcvn2uni(ten), ma, madung, nguon)
                lech[khoa] = lech.get(khoa, 0) + 1

    print("=" * 100)
    print("  KHOP: %d tham chieu  |  LECH MA: %d loai  |  KHONG TRA DUOC TEN: %d loai"
          % (khop, len(lech), len(khongco)))
    print("=" * 100)

    if lech:
        print()
        print("--- MA LECH (bang ghi ma Linux, JX1 dung ma khac) ---")
        print("%-46s %-12s %-12s %-12s %s" % ("ten", "bang ghi", "JX1 dung", "nguon", "so o"))
        for (ten, ma, madung, nguon), n in sorted(lech.items(), key=lambda x: -x[1]):
            print("%-46s %-12s %-12s %-12s %d" % (ten[:46], ma, madung, nguon, n))

    if khongco:
        print()
        print("--- KHONG TRA DUOC TEN trong bang vat pham JX1 (can tao moi) ---")
        for (ten, ma), n in sorted(khongco.items(), key=lambda x: -x[1])[:40]:
            print("   %-58s ma trong bang: %-12s (%d o)" % (ten[:58], ma, n))
        if len(khongco) > 40:
            print("   ... con %d loai nua" % (len(khongco) - 40))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
