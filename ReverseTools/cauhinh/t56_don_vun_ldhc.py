# -*- coding: utf-8 -*-
"""t56_don_vun_ldhc.py - don hai thu vun con lai sau khi bo Loi Dai Hon Chien.

1. script\\cauhinh\\ch_thuong.lua:25 - dong chu thich TIEU DE cua nhom van ghi
   "DANG TAT" va van tro toi script\\timerserver.lua, trong khi hoat dong da bo
   HAN va timerserver.lua khong con ma LDHC nao. De nguyen la nguoi doc sau nay
   tuong he thong con do.
2. dulieu\\LoiDaiHonChien_MangTam.txt (75 byte) - nhat ky ket qua tran, do
   timerserver.lua ghi ra. Khong con tep .lua nao doc/ghi no.

Mac dinh DIEN TAP; --ghi moi lam that.
"""
import io
import os
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lua_ham as lh  # noqa: E402
import doi_tep as dt  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

SV = dt.SV
P = os.path.join(dt.S, "cauhinh", "ch_thuong.lua")
NEO = "[CFGLDHC 30/08] LOI DAI HON CHIEN"
MOI = [
    "-- [BOLDHC 30/08] LOI DAI HON CHIEN - HOAT DONG DA BO HAN 30/08 theo",
    "-- quyet dinh chu game. Bon khoa LDHC_* ben duoi da bo; khong con ma nao",
    "-- doc chung. Giu lai may dong nay lam vet, xoa duoc bat cu luc nao.",
]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t56 don vun sau khi bo Loi Dai Hon Chien - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()

    # --- 1. chu thich tieu de lac hau ---
    print("## 1. ch_thuong.lua - chu thich tieu de lac hau")
    raw = doc(P)
    if "[BOLDHC 30/08] LOI DAI HON CHIEN - HOAT DONG DA BO HAN" in raw:
        print("  da sua - bo qua")
    else:
        dong = raw.split("\n")
        ci = [i for i, l in enumerate(dong) if NEO in l]
        if len(ci) != 1:
            print("!!! LOI TO: khop %d dong (can 1)" % len(ci))
            return 1
        i = ci[0]
        cr = "\r" if dong[i].endswith("\r") else ""
        print("  dong %d cu: %s" % (i + 1, dong[i].strip()[:96]))
        hi0, cb0 = hi(raw), lh.can_bang(raw)
        dong[i:i + 1] = [x + cr for x in MOI]
        nd = "\n".join(dong)
        if lh.can_bang(nd) != cb0 or hi(nd) != hi0:
            print("!!! LOI TO: can bang / byte tieng Viet doi")
            return 1
        if ghi:
            sao = P + ".truoc_donvun"
            if not os.path.isfile(sao):
                shutil.copy2(P, sao)
            with io.open(P, "wb") as f:
                f.write(nd.encode("latin-1"))
            if doc(P) != nd:
                print("!!! LOI TO: doc lai KHONG khop")
                return 1
            print("  DA GHI (thay bang %d dong noi ro da bo han)" % len(MOI))
        else:
            print("  se thay bang %d dong noi ro da bo han" % len(MOI))
    print()

    # --- 2. tep du lieu con sot ---
    print("## 2. dulieu/LoiDaiHonChien_MangTam.txt")
    src = os.path.join(SV, "dulieu", "LoiDaiHonChien_MangTam.txt")
    if not os.path.isfile(src):
        print("  khong con - bo qua")
    else:
        dst = os.path.join(dt.DARA, "dulieu", "LoiDaiHonChien_MangTam.txt")
        print("  %d byte -> _dara\\dulieu\\" % os.path.getsize(src))
        if ghi:
            tm = os.path.dirname(dst)
            if not os.path.isdir(tm):
                os.makedirs(tm)
            if os.path.exists(dst):
                print("  dich da ton tai - bo qua")
            else:
                shutil.move(src, dst)
                dt.ghi_nhat_ky(
                    "[2026-08-30] dulieu/LoiDaiHonChien_MangTam.txt  <- nhat ky"
                    " tran Loi Dai Hon Chien; hoat dong da bo han")
                print("  DA DOI")
    print()
    if not ghi:
        print("DIEN TAP - chua lam gi. Chay lai voi --ghi de lam that.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
