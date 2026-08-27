# -*- coding: utf-8 -*-
"""v28 - NAN CHI SO magicscript_stack.txt TU HE LINUX SANG HE JX1.

DO DUOC (v28, doi chieu ten vat pham):
  Cot 1 cua bang phu la CHI SO DONG (header ghi ro "so dong tru 1"), KHONG phai
  ma particular. Ban magicscript_stack.txt dang nam trong cay JX1 la BAN SAO
  NGUYEN XI CUA LINUX - chi so cua no tro vao bang Linux, va KHONG dong ma nao
  cua JX1 doc no.

  Chung minh bang 4 dong co co ro nghia nhat:
     chi so 148 (co "cung cap")     -> Linux: Huyen Tinh Khong Thach  (6/1/147)
                                    -> JX1  : Le vat Hoa Hong        (6/1/147)
     chi so 151/153/155 (co "cung ngu hanh")
                                    -> Linux: 3 mon NGUYEN THACH     (150/152/154)
                                    -> JX1  : Mat Ngan Nguyen Khoang, Chu Sa
                                              Nguyen Khoang, TONG KIM CHIEU THU
  Ban Linux dung nghia tuyet doi (Huyen Tinh co cap; Nguyen Thach co ngu hanh),
  ban JX1 doc thang thi ra vo nghia - va se bat Tong Kim Chieu thu phai "cung
  ngu hanh" moi xep chong duoc.

  Luu y: quan he "chi so = particular + 1" CHI dung o dai thap, vi cang xuong
  duoi thi so dong va ma particular cang lech nhau. Phai nan theo TEN.

*** Theo luat da chot: tra ten that bai thi BAO LOI TO, tuyet doi khong giu
    nguyen chi so cu - giu nguyen la tu tao khoa trung voi vat pham khac. ***
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, "D:/GAMEDEVNEW/ReverseTools/viemde")
import bangtxt

TAB = chr(9)
CR = chr(13)
NL = chr(10)

LNX = "D:/ServerLinux/server1/settings/item/004/magicscript.txt"
JX1 = "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/settings/item/magicscript.txt"
PHU = "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/settings/item/magicscript_stack.txt"
RA = "D:/GAMEDEVNEW/ReverseTools/loren/ra/settings/item/magicscript_stack.txt"

CO_TEN = ["cung cap", "cung ngu hanh", "cung may man",
          "ML1", "ML2", "ML3", "ML4", "ML5", "ML6"]


def doc(p):
    t = io.open(p, "rb").read().decode("latin-1")
    return [l.split(TAB) for l in t.replace(CR + NL, NL).split(NL) if l.strip()]


def chuan(s):
    s = bangtxt.tcvn2uni(s).strip().lower()
    return " ".join(s.split())


def main():
    L = doc(LNX)
    J = doc(JX1)
    O = doc(PHU)
    print("NAN CHI SO BANG PHU  Linux -> JX1")
    print("=" * 88)
    print("  magicscript Linux : %d dong du lieu" % (len(L) - 1))
    print("  magicscript JX1   : %d dong du lieu" % (len(J) - 1))
    print("  bang phu          : %d dong du lieu" % (len(O) - 1))

    # kho JX1: ten -> danh sach (chi so dong, genre, detail, particular)
    kho = {}
    for i in range(1, len(J)):
        r = J[i]
        if len(r) < 4:
            continue
        kho.setdefault(chuan(r[0]), []).append((i, r[1], r[2], r[3]))

    ra = []
    hong = []
    nhap_nhang = []
    for r in O[1:]:
        try:
            idx = int(r[0])
        except ValueError:
            hong.append((r[0], "chi so khong phai so"))
            continue
        if idx < 1 or idx >= len(L):
            hong.append((r[0], "chi so ngoai bang Linux (1..%d)" % (len(L) - 1)))
            continue
        nguon = L[idx]
        ten = chuan(nguon[0])
        ung = kho.get(ten, [])
        if not ung:
            hong.append((r[0], "JX1 khong co vat pham ten '%s' (Linux %s/%s/%s)"
                         % (bangtxt.tcvn2uni(nguon[0]).strip(),
                            nguon[1], nguon[2], nguon[3])))
            continue
        # uu tien cung Genre + DetailType
        hop = [u for u in ung if u[1] == nguon[1] and u[2] == nguon[2]]
        if not hop:
            hop = ung
        # KHU NHAP NHANG: JX1 co nhieu vat pham TRUNG TEN (do duoc: "Hoang Kim
        # Bao Hap" co 4 dong, "Thiep Hoa Hong" 2 dong, "Bi Kip Gia Truyen" 2
        # dong). Lay dong dau la SAI. Hai bang gan nhu song song nen dong dung
        # luon nam sat vi tri tuong ung ben Linux -> chon dong GAN NHAT.
        hop = sorted(hop, key=lambda u: abs(u[0] - idx))
        if len(hop) > 1 and abs(hop[0][0] - idx) == abs(hop[1][0] - idx):
            hong.append((r[0], "trung ten '%s', hai ung vien cach deu nhau %s"
                         % (bangtxt.tcvn2uni(nguon[0]).strip(),
                            [u[0] for u in hop[:4]])))
            continue
        if abs(hop[0][0] - idx) > 40:
            nhap_nhang.append((r[0], bangtxt.tcvn2uni(nguon[0]).strip(),
                               hop[0][0], abs(hop[0][0] - idx)))
        ra.append((hop[0][0], r, nguon, hop[0]))

    print()
    print("  tra duoc  : %d dong" % len(ra))
    print("  KHONG tra : %d dong" % len(hong))
    print("  cach xa >40 dong: %d (dang ngo, xem duoi)" % len(nhap_nhang))

    if hong:
        print()
        print("  --- KHONG TRA DUOC (bi loai bo, KHONG giu chi so cu) ---")
        for a, b in hong[:40]:
            print("    chi so %-6s %s" % (a, b))
        if len(hong) > 40:
            print("    ... con %d dong nua" % (len(hong) - 40))

    print()
    print("  --- CAC DONG CO CO (doi chieu Linux -> JX1) ---")
    for moi, r, nguon, dich in ra:
        cs = [CO_TEN[i - 2] for i in range(2, 11)
              if i < len(r) and r[i].strip() == "1"]
        if not cs:
            continue
        print("    %-5s -> %-5s  %-30s LNX %s/%s/%s  JX1 %s/%s/%s  [%s]"
              % (r[0], moi, bangtxt.tcvn2uni(nguon[0]).strip()[:30],
                 nguon[1], nguon[2], nguon[3], dich[1], dich[2], dich[3],
                 ", ".join(cs)))

    # ghi ra ban da nan
    if "--ghi" in sys.argv:
        d = os.path.dirname(RA)
        if not os.path.isdir(d):
            os.makedirs(d)
        dong = [TAB.join(O[0])]
        for moi, r, nguon, dich in ra:
            r2 = list(r)
            r2[0] = str(moi)
            dong.append(TAB.join(r2))
        t = (CR + NL).join(dong) + CR + NL
        io.open(RA, "wb").write(t.encode("latin-1"))
        print()
        print("  DA GHI %s (%d dong du lieu)" % (RA, len(dong) - 1))
    else:
        print()
        print("  (chay lai voi --ghi de xuat ban da nan)")
    return 1 if hong else 0


if __name__ == "__main__":
    raise SystemExit(main())
