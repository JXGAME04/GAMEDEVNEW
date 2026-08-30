# -*- coding: utf-8 -*-
"""t06c_dat_dung_pham_vi.py - sua tiep t06b: dat ham g_GhiLogHeThong DUNG PHAM VI TEP.

LOI (build bat duoc):
  KCore.cpp(324): error C2601: 'g_GhiLogHeThong': local function definitions
  are illegal
  => t06b tim chuoi "InitGameSetting(" va gap LOI GOI nam BEN TRONG mot ham
  khac truoc khi gap DINH NGHIA, nen chen ca khoi ham vao giua than ham do.

VA: cat khoi da chen sai ra, dat lai ngay SAU khoi khai bao bien toan cuc
(sau dong `int g_nExpToDoiKhac = ...` do t05a/t05a2 tao), la cho chac chan
thuoc pham vi tep.

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

P = r"D:\GAMEDEVNEW\Sources\Core\Src\KCore.cpp"
NHAN = "[HELOG2 29/08]"
DAU = "//---------------------------------------------------------------------------"


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t06c_dat_dung_pham_vi - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    raw = doc(P)
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    d = raw.split(eol)
    hi0 = sum(1 for c in raw if ord(c) > 127)

    # tim khoi da chen: tu dong DAU dung ngay truoc dong chua NHAN, den dong
    # '}' dong ham g_GhiLogHeThong
    vt_nhan = [i for i, l in enumerate(d) if NHAN in l and "GHI LOG HE THONG" in l]
    if len(vt_nhan) != 1:
        print("!!! LOI TO: tim thay %d dong tieu de khoi log (can 1)" % len(vt_nhan))
        return 1
    i0 = vt_nhan[0]
    while i0 > 0 and d[i0 - 1].strip() == DAU:
        i0 -= 1
    # dong cuoi: '}' dau tien o cot 0 sau `void g_GhiLogHeThong`
    iv = [i for i, l in enumerate(d) if l.startswith("void g_GhiLogHeThong(")]
    if len(iv) != 1:
        print("!!! LOI TO: tim thay %d dinh nghia g_GhiLogHeThong (can 1)" % len(iv))
        return 1
    i1 = iv[0]
    while i1 < len(d) and d[i1] != "}":
        i1 += 1
    if i1 >= len(d):
        print("!!! LOI TO: khong thay dau dong ham")
        return 1
    khoi = d[i0:i1 + 1]
    print("  khoi log dang o dong %d..%d (%d dong) - TRONG long ham khac"
          % (i0 + 1, i1 + 1, len(khoi)))

    con = d[:i0] + d[i1 + 1:]
    # bo dong trong thua tai cho cat
    while i0 < len(con) and con[i0].strip() == "" and i0 > 0 and con[i0 - 1].strip() == "":
        del con[i0]

    # dat lai sau dong khai bao bien toan cuc cuoi cung cua nhom exp
    vt_neo = [i for i, l in enumerate(con) if l.startswith("int\t\t\t\tg_nExpToDoiKhac")]
    if len(vt_neo) != 1:
        print("!!! LOI TO: tim thay %d dong neo g_nExpToDoiKhac (can 1)"
              % len(vt_neo))
        return 1
    j = vt_neo[0] + 1
    con = con[:j] + [""] + khoi + [""] + con[j:]

    nd = eol.join(con)
    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    if nd.count("void g_GhiLogHeThong(") != 1:
        print("!!! LOI TO: ham khong con duy nhat")
        return 1
    if len(nd) != len(raw) + len(eol) * 2:
        # chi la canh bao mem: do them 2 dong trong
        pass
    print("  dat lai sau dong %d (pham vi TEP)" % (vt_neo[0] + 1))

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    sao = P + ".truoc_phamvi"
    if not os.path.isfile(sao):
        shutil.copy2(P, sao)
    with io.open(P, "wb") as f:
        f.write(nd.encode("latin-1"))
    if doc(P) != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI KCore.cpp")
    return 0


if __name__ == "__main__":
    sys.exit(main())
