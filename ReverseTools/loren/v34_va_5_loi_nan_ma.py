# -*- coding: utf-8 -*-
"""v34 - VA NAM LOI NAN MA SO HOC TRONG KICH BAN LO REN.

Cong cu v31 chi nan ma trong BO BA {6, 1, N} va trong dong co san ngu canh
"nGenre == 6 and nDetailType == 1". No KHONG bat duoc:
  * hang so nam giua mot mang phang:  { ver, seed, 0, 6, 1, 147, i, ... }
  * hang so lam GOC TRU/CONG:          nParticular - 148   /   199 + pos
  * phep kiem CHAN LE dao nghia khi dai ma dich di mot bac

Ca nam deu cung mot goc: DA DOI DAI MA trong dieu kien nhung QUEN doi hang so
o cho DUNG. Day dung la loai loi da ghi thanh luat ngay 26/08.

DOI CHIEU DAI MA (do bang ten vat pham, khong doan):
  Huyen Tinh Khoang Thach : Linux 147        -> JX1 146
  Nguyen Khoang/Thach     : Linux 149..154   -> JX1 148..153
  Khoang thuoc tinh       : Linux 200..205   -> JX1 199..204
  Trong do "Am" (co ngu hanh) ben Linux la 201/203/205 (LE),
  sau khi dich mot bac thanh 200/202/204 (CHAN) => phep chan le PHAI DAO.

NAM MIENG VA:
  1. xuanjing_compound.lua : ma san pham 147 -> 146
  2. magic_distill.lua     : pos = ptc - 148 -> - 147   (de pos ra 1..6)
  3. magic_distill.lua     : san pham 199+pos -> 198+pos (de ra 199..204)
  4. ore_upgrade.lua       : mod(ptc,2) ~= 0 -> == 0
  5. equip_enchase.lua     : pos = ptc - 199 -> - 198

Sau khi va, cong cu KIEM CHUNG DUONG TINH: cho sau ma quang di qua cong thuc
va doi chieu pos phai ra dung 1..6, va phan loai Minh/Am phai dung ten vat pham.
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

CAY = [
    ("goi nguon", r"D:\GAMEDEVNEW\ReverseTools\loren\ra\lua\item\compound"),
    ("may chu  ", r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\compound"),
]

VA = [
    ("xuanjing_compound.lua",
     "0, 6, 1, 147, i,",
     "0, 6, 1, 146, i,",
     "ma san pham Huyen Tinh 147 -> 146"),
    ("magic_distill.lua",
     "g_nDistillMagicPos = nParticular - 148;",
     "g_nDistillMagicPos = nParticular - 147;",
     "goc tru cho pos: -148 -> -147 (pos ra 1..6)"),
    ("magic_distill.lua",
     "6, 1, 199 + g_nDistillMagicPos,",
     "6, 1, 198 + g_nDistillMagicPos,",
     "ma san pham: 199+pos -> 198+pos (ra 199..204)"),
    ("ore_upgrade.lua",
     "if( mod( nParticular, 2 ) ~= 0 and g_nOreSeries ~= nSeries ) then",
     "if( mod( nParticular, 2 ) == 0 and g_nOreSeries ~= nSeries ) then",
     "chan le DAO: Am ben JX1 la so CHAN"),
    ("equip_enchase.lua",
     "nOreMagLvlPos = nParticular - 199;",
     "nOreMagLvlPos = nParticular - 198;",
     "goc tru cho pos: -199 -> -198 (pos ra 1..6)"),
]


def kiem_chung_duong_tinh():
    """Chay THAT cong thuc sau khi va, doi chieu voi ten vat pham trong bang."""
    bang = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item\magicscript.txt"
    t = io.open(bang, "rb").read().decode("latin-1")
    rows = [l.split(TAB) for l in t.replace(CR + NL, NL).split(NL) if l.strip()]
    ten = {}
    for r in rows[1:]:
        if len(r) > 3 and r[1] == "6" and r[2] == "1":
            try:
                ten[int(r[3])] = bangtxt.tcvn2uni(r[0]).strip()
            except ValueError:
                pass

    print()
    print("  KIEM CHUNG DUONG TINH (chay that cong thuc, doi chieu ten):")
    ok = True

    print("    -- magic_distill: pos = ptc - 147, san pham = 198 + pos --")
    for p in range(148, 154):
        pos = p - 147
        sp = 198 + pos
        loai = "Am " if pos % 2 == 0 else "Minh"
        dung = (1 <= pos <= 6) and (199 <= sp <= 204)
        # "Nguyen Thach" la mon co ngu hanh (Am); "Nguyen Khoang" la Minh
        tn = ten.get(p, "?")
        khop_ten = ("thach" in bangtxt.tcvn2uni(tn).lower() or "thạch" in tn.lower()) == (pos % 2 == 0)
        if not (dung and khop_ten):
            ok = False
        print("      ptc %d -> pos %d -> san pham %d  [%s]  %-28s %s"
              % (p, pos, sp, loai, tn[:28], "OK" if (dung and khop_ten) else "*** SAI"))

    print("    -- equip_enchase: pos = ptc - 198 --")
    for p in range(199, 205):
        pos = p - 198
        loai = "Am " if pos % 2 == 0 else "Minh"
        tn = ten.get(p, "?")
        khop_ten = ("thach" in bangtxt.tcvn2uni(tn).lower() or "thạch" in tn.lower()) == (pos % 2 == 0)
        if not (1 <= pos <= 6) or not khop_ten:
            ok = False
        print("      ptc %d -> pos %d  [%s]  %-30s %s"
              % (p, pos, loai, tn[:30], "OK" if (1 <= pos <= 6 and khop_ten) else "*** SAI"))

    print("    -- ore_upgrade: mod(ptc,2) == 0 phai trung voi mon Am --")
    for p in range(199, 205):
        la_am = (p % 2 == 0)
        tn = ten.get(p, "?")
        that_la_am = ("thach" in bangtxt.tcvn2uni(tn).lower() or "thạch" in tn.lower())
        if la_am != that_la_am:
            ok = False
        print("      ptc %d  mod==0: %-5s  ten: %-30s %s"
              % (p, str(la_am), tn[:30], "OK" if la_am == that_la_am else "*** SAI"))

    return ok


def main():
    ghi = "--ghi" in sys.argv
    print("VA NAM LOI NAN MA SO HOC  %s" % ("[GHI THAT]" if ghi else "[DIEN TAP]"))
    print("=" * 92)
    loi = 0
    for nhan, thumuc in CAY:
        print()
        print("--- %s : %s" % (nhan, thumuc))
        if not os.path.isdir(thumuc):
            print("    *** KHONG CO THU MUC")
            loi += 1
            continue
        for ten_tep, tim, thay, mota in VA:
            p = os.path.join(thumuc, ten_tep)
            if not os.path.isfile(p):
                print("    *** THIEU %s" % ten_tep)
                loi += 1
                continue
            b = io.open(p, "rb").read()
            t = b.decode("latin-1")
            if thay in t:
                print("    BO QUA  %-24s %s" % (ten_tep, mota))
                continue
            n = t.count(tim)
            if n != 1:
                print("    *** LOI %-24s moc neo %d lan: %s" % (ten_tep, n, tim[:44]))
                loi += 1
                continue
            t2 = t.replace(tim, thay, 1)
            if len(t2) != len(t) and abs(len(t2) - len(t)) > 2:
                print("    *** LOI %-24s doi do dai bat thuong" % ten_tep)
                loi += 1
                continue
            print("    OK      %-24s %s" % (ten_tep, mota))
            if ghi:
                sao = p + ".truoc_va5"
                if not os.path.isfile(sao):
                    io.open(sao, "wb").write(b)
                io.open(p, "wb").write(t2.encode("latin-1"))

    print()
    print("=" * 92)
    if loi:
        print("  CO %d MUC LOI" % loi)
        return 1
    if ghi:
        if not kiem_chung_duong_tinh():
            print()
            print("  *** KIEM CHUNG THAT BAI - xem lai")
            return 1
        print()
        print("  DA VA XONG va kiem chung duong tinh dat.")
    else:
        print("  (chay lai voi --ghi de ghi that)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
