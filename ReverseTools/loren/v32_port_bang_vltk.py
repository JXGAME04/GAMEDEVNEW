# -*- coding: utf-8 -*-
"""v32 - PORT HAI BANG CUA HE LO REN TU CLIENT VLTK SANG JX1.

1) itemvaluescript.txt
   Ban VLTK (update01, 46 dong du lieu) la ban VIET HOA CHAY THAT - lay lam goc.
   Nan ma particular cua genre 6 / detail 1 theo TEN vat pham.
   Giu them cac dong Huyen Kim (FantasyGold) do ban Linux co ma VLTK khong co,
   vi JX1 CO hai mon do (3769 / 3770).

2) magicscript_stack.txt
   *** DAY LA CHO CLIENT VLTK KHAC HAN BAN LINUX ***
   May chu Linux : 543 dong, 32 dong bat co
   Client VLTK   : 247 dong, CHI 4 dong bat co
       chi so 148 -> yeu cau cung cap      (Huyen Tinh Khoang Thach)
       chi so 151 -> yeu cau cung ngu hanh (Khong Tuoc Nguyen Thach)
       chi so 153 -> yeu cau cung ngu hanh (Phu Dung Nguyen Thach)
       chi so 155 -> yeu cau cung ngu hanh (Chung Nhu Nguyen Thach)
   28 dong con lai cua ban Linux (banh kem, nen, thiep hoa hong, hop trang suc...)
   KHONG co trong ban Viet hoa. Lay ban VLTK: dung 4 co, dung cac mon lo ren.

   Chi so cot 1 la SO DONG cua bang magicscript (header ghi "so dong tru 1"),
   khong phai ma particular - phai nan theo dong, khong nan theo ma.
   Chi ghi 4 dong co co; cot "max xep chong" KHONG dung vi JX1 da co co che
   rieng o cot 13 (nMaxStack), dung cot cua Linux se doi cach xep chong cua
   hang tram vat pham dang song, ngoai pham vi viec lo ren.
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

VLTK = "D:/GAMEDEVNEW/ReverseTools/pak_vltk/ra_vltk"
LNX_MS = "D:/ServerLinux/server1/settings/item/004/magicscript.txt"
RA = "D:/GAMEDEVNEW/ReverseTools/loren/ra/settings/item"
JX1_MS = os.path.join(RA, "magicscript.txt")

CO_TEN = ["cung cap", "cung ngu hanh", "cung may man",
          "ML1", "ML2", "ML3", "ML4", "ML5", "ML6"]


def doc(p):
    t = io.open(p, "rb").read().decode("latin-1")
    return [l.split(TAB) for l in t.replace(CR + NL, NL).split(NL) if l.strip()]


def chuan(s):
    return " ".join(bangtxt.tcvn2uni(s).strip().lower().split())


def _hang(fn):
    """Xep hang pak theo thu tu NAP, khong phai theo bang chu.
    Bay: "update__x" sap SAU "update04__x" theo bang chu (dau _ lon hon chu so)
    nen sorted()[-1] tra ve ban CU nhat. Phai boc so duoi ten pak."""
    pak = fn.split("__")[0]
    so = ""
    while pak and pak[-1].isdigit():
        so = pak[-1] + so
        pak = pak[:-1]
    return (0 if pak.startswith("slist") else 1, pak, int(so) if so else 0)


def moi_nhat(ten):
    ung = [f for f in os.listdir(VLTK) if f.endswith("__" + ten)]
    if not ung:
        return None
    ung.sort(key=_hang)
    return os.path.join(VLTK, ung[-1])


def main():
    ghi = "--ghi" in sys.argv
    print("PORT BANG LO REN: client VLTK -> JX1  %s"
          % ("[GHI THAT]" if ghi else "[DIEN TAP]"))
    print("=" * 92)
    L = doc(LNX_MS)
    J = doc(JX1_MS)
    loi = []

    # kho tra ten -> (particular, so dong) ben JX1
    kho_ten = {}
    for i in range(1, len(J)):
        r = J[i]
        if len(r) > 3:
            kho_ten.setdefault(chuan(r[0]), []).append((r[3], i))

    def tra_ptc(p):
        """ma particular Linux (genre6/detail1) -> ma JX1, theo ten."""
        dong = None
        for i in range(1, len(L)):
            r = L[i]
            if len(r) > 3 and r[1] == "6" and r[2] == "1" and r[3] == str(p):
                dong = i
                break
        if dong is None:
            loi.append("ptc %s: khong co trong bang Linux (6/1)" % p)
            return None, None
        ten = bangtxt.tcvn2uni(L[dong][0]).strip()
        ung = sorted(kho_ten.get(chuan(L[dong][0]), []),
                     key=lambda u: abs(u[1] - dong))
        if not ung:
            loi.append("ptc %s [%s]: JX1 khong co ten do" % (p, ten))
            return None, ten
        if len(ung) > 1 and abs(ung[0][1] - dong) == abs(ung[1][1] - dong):
            loi.append("ptc %s [%s]: trung ten, hai ung vien cach deu" % (p, ten))
            return None, ten
        return int(ung[0][0]), ten

    def tra_dong(idx):
        """chi so dong bang Linux -> chi so dong bang JX1, theo ten."""
        idx = int(idx)
        if idx < 1 or idx >= len(L):
            loi.append("chi so dong %d ngoai bang Linux" % idx)
            return None, None
        ten = bangtxt.tcvn2uni(L[idx][0]).strip()
        ung = sorted(kho_ten.get(chuan(L[idx][0]), []),
                     key=lambda u: abs(u[1] - idx))
        if not ung:
            loi.append("chi so dong %d [%s]: JX1 khong co ten do" % (idx, ten))
            return None, ten
        if len(ung) > 1 and abs(ung[0][1] - idx) == abs(ung[1][1] - idx):
            loi.append("chi so dong %d [%s]: trung ten, cach deu" % (idx, ten))
            return None, ten
        return ung[0][1], ten

    # ---------------- 1. itemvaluescript.txt ----------------
    print()
    print("--- itemvaluescript.txt ---")
    V = doc(moi_nhat("itemvaluescript.txt"))
    cu = doc(os.path.join(RA, "itemvaluescript.txt"))
    ra_dong = [TAB.join(V[0])]
    nan_dem = 0
    for r in V[1:]:
        r = list(r)
        # cot: 0 ten 1 quality 2 genre 3 detail 4 particular 5 level 6 series ...
        if len(r) > 4 and r[2] == "6" and r[3] == "1" and r[4].strip().isdigit():
            p = int(r[4])
            moi, ten = tra_ptc(p)
            if moi is None:
                continue
            if moi != p:
                print("      %-30s %4d -> %-4d" % (ten[:30], p, moi))
                nan_dem += 1
            r[4] = str(moi)
        ra_dong.append(TAB.join(r))
    # giu cac dong Huyen Kim cua ban cu (VLTK khong co)
    them = 0
    for r in cu[1:]:
        if len(r) > 8 and ("fantasygold" in r[8].lower()):
            ra_dong.append(TAB.join(r))
            them += 1
    print("  VLTK %d dong, nan %d ma, giu them %d dong Huyen Kim -> tong %d dong"
          % (len(V) - 1, nan_dem, them, len(ra_dong) - 1))

    # ---------------- 2. magicscript_stack.txt ----------------
    print()
    print("--- magicscript_stack.txt (chi lay 4 dong co co cua ban VLTK) ---")
    S = doc(moi_nhat("magicscript_stack.txt"))
    ra_stack = [TAB.join(S[0])]
    for r in S[1:]:
        co = [r[i].strip() if i < len(r) else "0" for i in range(2, 11)]
        if not any(c == "1" for c in co):
            continue
        moi, ten = tra_dong(r[0])
        if moi is None:
            continue
        ptc = J[moi][3] if len(J[moi]) > 3 else "?"
        print("      chi so %-5s -> %-5d  %-30s JX1 6/1/%-5s [%s]"
              % (r[0], moi, ten[:30], ptc,
                 ", ".join(CO_TEN[k] for k in range(9) if co[k] == "1")))
        r2 = list(r)
        r2[0] = str(moi)
        ra_stack.append(TAB.join(r2))
    print("  tong %d dong co" % (len(ra_stack) - 1))

    print()
    if loi:
        print("  *** CO %d LOI TRA MA - KHONG GHI GI CA:" % len(loi))
        for x in loi:
            print("      %s" % x)
        return 1
    print("  khong loi tra ma.")
    if ghi:
        for ten, dong in (("itemvaluescript.txt", ra_dong),
                          ("magicscript_stack.txt", ra_stack)):
            p = os.path.join(RA, ten)
            if os.path.isfile(p) and not os.path.isfile(p + ".truoc_vltk"):
                io.open(p + ".truoc_vltk", "wb").write(io.open(p, "rb").read())
            io.open(p, "wb").write(((CR + NL).join(dong) + CR + NL).encode("latin-1"))
            print("  DA GHI %s (%d dong du lieu)" % (ten, len(dong) - 1))
    else:
        print("  (chay lai voi --ghi de ghi that)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
