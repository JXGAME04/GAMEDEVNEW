# -*- coding: utf-8 -*-
r"""v27 - GO CAC PHEP SO XEP CHONG DO TOI TU NGHI RA.

DO TU NHI PHAN LINUX (jx_linux_y):
  Ban goc co DUNG MOT ham quyet dinh xep chong: 0x08065A70 (5 noi goi:
  081FC352, 082029F9, 08206C3E, 08207A9D, 08207B68) - dung chung cho ca
  tim-chong lan nhap-chong. No so:
     2 cong bStack (+0x14 cua ca hai mon)
     6 so VO DIEU KIEN : genre +0x00, detail +0x08, particular +0x0C,
                         bind +0x350, expired +0x34C, left-usage +0x348
     9 so CO CO       : cap, ngu hanh, may man, MagicLevel1..6
                        (co lay tu KItem +0x310..+0x330)
  Chin co do la COT 22..30 cua magicscript.txt, nap tai 0x08067C10.

DO TREN DU LIEU GOC (settings\item\004\magicscript.txt, 4.995 dong du lieu):
     yeu cau cung cap     : 51 dong
     yeu cau cung ngu hanh:  3 dong
     yeu cau cung may man :  0 dong   <-- KHONG MOT DONG NAO
     yeu cau cung MagicLevel1..6: 27, 4, 4, 4, 3, 3 dong
  Rieng 15 ma nguyen lieu lo ren: KHONG ma nao co co MagicLevel, va khong ma
  nao co co may man. Huyen Tinh + Huyen Kim co co CAP; ba Nguyen Thach co co
  NGU HANH; sau khoang thuoc tinh thi KHONG XEP CHONG (bStack = 0).

=> Bai toan "hai vien khoang khac ma phep nhap lam mot" duoc ban goc giai bang
   KHONG CHO XEP CHONG, chu khong bang phep so. Ma viec do thi ban va
   nMaxStack 50->0 (v19) da lam dung roi.

=> Hai phep so toi tu them la KHONG CO CAN CU O BAN GOC, va chung dang ap cho
   TOAN BO 4.995 vat pham chu khong rieng gi lo ren. Go bo.

Cu the go:
  KItem.cpp   CanStack(nOldIdx, Dest)  - go ca ba dong toi them
              (nLevel, nGeneratorLevel[0], nLuck). Ban goc cua JX1 khong so
              cap o ham nay; them vao la chat hon ban goc lan JX1.
  KInventory.cpp FindSameItemToSort    - go hai dong toi them
              (nGeneratorLevel[0], nLuck). Phep so CAP o day la CO SAN cua
              JX1 tu truoc, giu nguyen.

CON LAI DE LAM CHO DUNG 100%: nap settings\item\magicscript_stack.txt (JX1 DA
CO SAN tep nay - 544 dong, 11 cot, dung khuon tep phu cua ban goc - nhung
KHONG mot dong ma nao doc no) roi dung 9 co do dieu khien phep so, y het
0x08065A70. Chi so trong tep la chi so dong cua bang LINUX nen phai nan sang
JX1 truoc, giong cach da lam voi atlas_compound.
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CR = chr(13)
NL = chr(10)
TAB = chr(9)
SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def dem_dong(t):
    crlf = t.count(CR + NL)
    return crlf, t.count(NL) - crlf


def theo_eol(t, s):
    crlf, lf = dem_dong(t)
    s = s.replace(CR + NL, NL)
    if crlf > lf:
        s = s.replace(NL, CR + NL)
    return s


def khoi(*d):
    return "".join(x + NL for x in d)


CAP = [
    ("KItem.cpp", "go ba phep so tu che khoi CanStack",
     khoi(TAB * 3 + "// [LOREN] So them CAP va MA PHEP. Nguyen lieu lo ren co cap that",
          TAB * 3 + "// va mang ma phep rieng; thieu hai phep so nay thi hai vien khac",
          TAB * 3 + "// nhau se nhap lam mot chong va MAT du lieu. Them dieu kien chi",
          TAB * 3 + "// lam viec nhap chong kho hon, khong dung toi do da nhap tu truoc.",
          TAB * 3 + "&& m_CommonAttrib.nLevel == Item[nOldIdx].GetLevel()",
          TAB * 3 + "&& m_GeneratorParam.nGeneratorLevel[0] == Item[nOldIdx].m_GeneratorParam.nGeneratorLevel[0]",
          TAB * 3 + "// nLuck la cua ma JX1 dung cho ma phep cua khoang (KItem.cpp:2372)",
          TAB * 3 + "&& m_GeneratorParam.nLuck == Item[nOldIdx].m_GeneratorParam.nLuck"),
     khoi(TAB * 3 + "// [LOREN] Da GO ba phep so tung them o day. Do nhi phan ban goc:",
          TAB * 3 + "// ham xep chong 0x08065A70 so may man va MagicLevel CO DIEU KIEN,",
          TAB * 3 + "// theo co o cot 22..30 cua magicscript.txt. Do tren du lieu goc:",
          TAB * 3 + "// co may man = 0/4995 dong, va KHONG mot nguyen lieu lo ren nao co",
          TAB * 3 + "// co MagicLevel. Ban goc chan viec tron ma phep bang cach KHONG CHO",
          TAB * 3 + "// XEP CHONG sau khoang thuoc tinh (nMaxStack = 0), khong bang phep so.")),

    ("KInventory.cpp", "go hai phep so tu che khoi FindSameItemToSort",
     khoi(TAB * 3 + "// [LOREN] So them MA PHEP. Khoang thuoc tinh cua he lo ren mang",
          TAB * 3 + "// ma phep rieng o nGeneratorLevel[0]; thieu phep so nay thi hai",
          TAB * 3 + "// vien khac ma phep se nhap lam mot chong va MAT mot ma.",
          TAB * 3 + "&& Item[*pArray].m_GeneratorParam.nGeneratorLevel[0] == Item[nIdx].m_GeneratorParam.nGeneratorLevel[0]",
          TAB * 3 + "// nLuck la cua ma JX1 dung cho ma phep cua khoang (KItem.cpp:2372)",
          TAB * 3 + "&& Item[*pArray].m_GeneratorParam.nLuck == Item[nIdx].m_GeneratorParam.nLuck"),
     khoi(TAB * 3 + "// [LOREN] Da GO hai phep so tung them o day - xem giai thich trong",
          TAB * 3 + "// KItem::CanStack. Phep so CAP ngay tren la cua JX1 co san, giu nguyen.")),
]


def main():
    print("GO CAC PHEP SO XEP CHONG DO TOI TU NGHI RA")
    print("=" * 78)
    loi = 0
    for ten, nhan, tim, thay in CAP:
        p = os.path.join(SRC, ten)
        goc = doc(p)
        a = theo_eol(goc, tim)
        b = theo_eol(goc, thay)
        if b in goc:
            print("  BO QUA  %-16s %s" % (ten, nhan))
            continue
        n = goc.count(a)
        if n != 1:
            print("  *** LOI %-16s %s: moc neo %d lan" % (ten, nhan, n))
            loi += 1
            continue
        t = goc.replace(a, b, 1)
        c0, l0 = dem_dong(goc)
        c1, l1 = dem_dong(t)
        if min(c1, l1) > min(c0, l0):
            print("  *** LOI %-16s tang loai xuong dong thieu so" % ten)
            loi += 1
            continue
        io.open(p, "wb").write(t.encode("latin-1"))
        print("  OK      %-16s %s (CRLF %d->%d)" % (ten, nhan, c0, c1))
    print("=" * 78)
    print("  %s" % ("XONG" if loi == 0 else "CO %d MUC LOI" % loi))
    return 1 if loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
