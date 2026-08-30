# -*- coding: utf-8 -*-
"""t16_noi_chot_randrange.py - noi chot `nMaxRandRate <= 0` thanh `< 0`.

LY DO (bo phan bien neu, toi da tu doc ma xac nhan):
  Engine\\Src\\KRandom.cpp  g_Random(UINT nMax): nMax == 0 thi TRA VE 0, khong
  chia cho 0, khong sap. Nen mot tep .ini co RandRange = 0 ma tong RandRate > 0
  van chay duoc: nRand luon = 0 => luon trung muc dau tien => van rot do.

  Chot cu cua toi (`|| m_pDropRate->nMaxRandRate <= 0` thi return) se lam nhung
  tep nhu vay NGUNG ROT DO - tuc DOI HANH VI, dung cai ma ban va tu hua la khong
  lam. Chot chong treo chi can lo tong RandRate = 0 la du.

  Do that: hien KHONG co tep .ini nao co RandRange <= 0 (quet ca 49 tep), nen
  sua nay khong doi gi hom nay - no chi bo mot cai bay cho mai sau.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

P = r"D:\GAMEDEVNEW\Sources\Core\Src\KNpc.cpp"
NHAN = "[NOICHOT 29/08]"
T = "\t"

CU = [
    T + "if (nTongRate <= 0 || m_pDropRate->nMaxRandRate <= 0)",
    T*2 + "return;",
]
MOI = [
    T + "// " + NHAN + " chi chan dung cai gay TREO. g_Random(0) tra ve 0 chu",
    T + "// khong chia cho 0 (Engine\\Src\\KRandom.cpp), nen tep .ini co",
    T + "// RandRange = 0 ma tong rate > 0 van rot do binh thuong (luon trung",
    T + "// muc dau). Chan ca truong hop do la DOI HANH VI, khong phai chong treo.",
    T + "if (nTongRate <= 0 || m_pDropRate->nMaxRandRate < 0)",
    T*2 + "return;",
]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t16_noi_chot_randrange - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    raw = doc(P)
    if NHAN in raw:
        print("  DA VA - bo qua")
        return 0
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    cu = eol.join(CU)
    if raw.count(cu) != 1:
        print("!!! LOI TO: moc khop %d lan (can 1)" % raw.count(cu))
        return 1
    nd = raw.replace(cu, eol.join(MOI))
    if sum(1 for c in nd if ord(c) > 127) != sum(1 for c in raw if ord(c) > 127):
        print("!!! LOI TO: byte cao doi")
        return 1
    print("  noi chot: nMaxRandRate <= 0  ->  < 0")
    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    sao = P + ".truoc_noichot"
    if not os.path.isfile(sao):
        shutil.copy2(P, sao)
    with io.open(P, "wb") as f:
        f.write(nd.encode("latin-1"))
    if doc(P) != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI KNpc.cpp")
    return 0


if __name__ == "__main__":
    sys.exit(main())
