# -*- coding: utf-8 -*-
"""t51_va_cau_station.py - sua cau tieng Viet o station.lua ma toi go SAI BYTE.

LOI CUA TOI: trong t49 buoc A3, dong Talk(...) cuoi ham godatau() toi TU GO
byte TCVN3 ("Kh\\xf4ng nh\\xe2n...") thay vi chep nguyen van tu ban cu. Ket qua
la byte SAI - khong phai TCVN3, game se hien chu hong.

LUAT: KHONG BAO GIO tu go byte tieng Viet. Luon chep NGUYEN BYTE tu ban goc.

Cong cu nay: doc dong Talk goc trong station.lua.truoc_dondatau, thay vao cho
dong sai. So byte tieng Viet truoc/sau de chac.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import re
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lua_ham as lh  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
P = os.path.join(S, "global", "station.lua")
BAK = P + ".truoc_dondatau"


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t51 va cau tieng Viet o station.lua - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()
    if not os.path.isfile(BAK):
        print("!!! LOI TO: khong thay ban sao %s" % BAK)
        return 1

    cu = doc(BAK).split("\n")
    # dong goc: Talk(...) trong nhanh he cu, ngay sau dong "else"
    goc = [l for l in cu if "Talk(1,\"\"," in l and "l?a ta" in l]
    if len(goc) != 1:
        print("!!! LOI TO: tim thay %d dong goc (can 1)" % len(goc))
        return 1
    dong_goc = goc[0].rstrip("\r")
    print("  Dong goc (nguyen byte, %d byte tieng Viet):" % hi(dong_goc))
    print("    %r" % dong_goc.strip()[:90])

    raw = doc(P)
    moi = raw.split("\n")
    sai = [i for i, l in enumerate(moi)
           if "Talk(1,\"\"," in l and "l?a ta" in l]
    if len(sai) != 1:
        print("!!! LOI TO: tim thay %d dong can sua (can 1)" % len(sai))
        return 1
    i = sai[0]
    cu_dong = moi[i].rstrip("\r")
    if cu_dong == dong_goc:
        print("  Da dung roi - khong can sua")
        return 0
    print("  Dong hien tai SAI (%d byte tieng Viet):" % hi(cu_dong))
    print("    %r" % cu_dong.strip()[:90])

    hi0 = hi(raw)
    cb0 = lh.can_bang(raw)
    co_cr = moi[i].endswith("\r")
    moi[i] = dong_goc + ("\r" if co_cr else "")
    nd = "\n".join(moi)
    if lh.can_bang(nd) != cb0:
        print("!!! LOI TO: can bang tu khoa doi")
        return 1
    print("  byte tieng Viet toan tep: %d -> %d (chenh %+d = dung phan cau)"
          % (hi0, hi(nd), hi(nd) - hi0))

    if not ghi:
        print()
        print("DIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    sao = P + ".truoc_vacau"
    if not os.path.isfile(sao):
        shutil.copy2(P, sao)
    with io.open(P, "wb") as f:
        f.write(nd.encode("latin-1"))
    if doc(P) != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI station.lua")
    return 0


if __name__ == "__main__":
    sys.exit(main())
