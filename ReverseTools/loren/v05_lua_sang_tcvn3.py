# -*- coding: utf-8 -*-
r"""v05 - CHUYEN KICH BAN LUA TU UTF-8 SANG TCVN3 va dat vao may chu.

Vi sao can: kich ban cua JX1 doc bang mot byte, chu Viet phai la TCVN3. Ta
soan tep bang UTF-8 cho de doc/sua, roi chuyen o buoc nay.

BA CHOT AN TOAN (deu la bai hoc da tra gia):
  1. VONG TRON uni -> tcvn -> uni. Ky tu nao khong ve dung chinh no thi BAO
     LOI VA DUNG. TCVN3 chi ma hoa duoc 7/67 nguyen am HOA co dau (A E O A D O U)
     nen viet HOA la roi dau IM LANG: "BO TEST TOAN BO" -> "B TEST TON B".
  2. CAM dau '/' trong NHAN cua menu SayEx (ky tu tach nhan/ham).
  3. Ghi ra tep tam roi moi thay the, khong ghi de truc tiep.

Chay:  python v05_lua_sang_tcvn3.py            -> kiem tra, khong ghi
       python v05_lua_sang_tcvn3.py --ghi      -> ghi vao thu muc ra\lua
       python v05_lua_sang_tcvn3.py --dat      -> chep tiep vao may chu
"""
import io
import os
import shutil
import sys
import importlib.util

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

HERE = os.path.dirname(os.path.abspath(__file__))
_spec = importlib.util.spec_from_file_location(
    "bangtxt", os.path.join(HERE, "..", "viemde", "bangtxt.py"))
bangtxt = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(bangtxt)

NGUON = os.path.join(HERE, "lua_utf8")
RA = os.path.join(HERE, "ra", "lua")
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"

CR = chr(13)
NL = chr(10)


def quet(goc):
    for thu_muc, _, ten_tep in os.walk(goc):
        for t in sorted(ten_tep):
            if t.lower().endswith(".lua"):
                p = os.path.join(thu_muc, t)
                yield p, os.path.relpath(p, goc)


def kiem_vong_tron(uni, ten, rel):
    """tra ve danh sach (dong, cot, ky tu) khong song sot vong tron"""
    hong = []
    for i, dong in enumerate(uni.split(NL), 1):
        tc = bangtxt.uni2tcvn(dong)
        ve = bangtxt.tcvn2uni(tc)
        if ve == dong:
            continue
        n = min(len(ve), len(dong))
        j = 0
        while j < n and ve[j] == dong[j]:
            j += 1
        hong.append((i, j + 1, dong[j] if j < len(dong) else "?", dong.strip()[:70]))
    return hong


def main():
    ghi = "--ghi" in sys.argv or "--dat" in sys.argv
    dat = "--dat" in sys.argv

    if not os.path.isdir(NGUON):
        print("KHONG CO thu muc nguon: %s" % NGUON)
        return 1

    tep = list(quet(NGUON))
    print("CHUYEN KICH BAN LUA UTF-8 -> TCVN3")
    print("=" * 88)
    print("nguon: %s  (%d tep)" % (NGUON, len(tep)))
    print()

    tong_hong = 0
    ket = []
    for p, rel in tep:
        uni = io.open(p, "rb").read().decode("utf-8")
        uni = uni.replace(CR + NL, NL)          # chuan hoa truoc khi do
        hong = kiem_vong_tron(uni, os.path.basename(p), rel)
        tcvn = bangtxt.uni2tcvn(uni)
        # kich ban cua JX1 trong cay nay dung LF (giong cac tep .lua san co)
        b = tcvn.encode("latin-1")
        ket.append((rel, b, hong))
        tong_hong += len(hong)
        trang = "OK" if not hong else "*** %d DONG HONG ***" % len(hong)
        print("  %-46s %7d byte  %s" % (rel, len(b), trang))
        for d, c, ky, noi_dung in hong[:5]:
            print("       dong %-4d cot %-3d ky tu %r  |  %s" % (d, c, ky, noi_dung))

    print()
    print("=" * 88)
    if tong_hong:
        print("  DUNG LAI: %d dong khong song sot vong tron uni->tcvn->uni." % tong_hong)
        print("  Thuong la do viet HOA co dau. Doi sang chu thuong / Title case.")
        return 1
    print("  Tat ca %d tep qua vong tron sach." % len(tep))

    if not ghi:
        print("  [chua ghi tep nao - them --ghi hoac --dat]")
        return 0

    for rel, b, _ in ket:
        p = os.path.join(RA, rel)
        d = os.path.dirname(p)
        if not os.path.isdir(d):
            os.makedirs(d)
        tam = p + ".tam"
        io.open(tam, "wb").write(b)
        if os.path.isfile(p):
            os.remove(p)
        os.rename(tam, p)
    print("  >> da ghi %d tep vao %s" % (len(ket), RA))

    if dat:
        for rel, b, _ in ket:
            p = os.path.join(SRV, rel)
            d = os.path.dirname(p)
            if not os.path.isdir(d):
                os.makedirs(d)
            if os.path.isfile(p) and not os.path.isfile(p + ".truoc_loren"):
                shutil.copy2(p, p + ".truoc_loren")
            tam = p + ".tam"
            io.open(tam, "wb").write(b)
            if os.path.isfile(p):
                os.remove(p)
            os.rename(tam, p)
        print("  >> da dat %d tep vao %s" % (len(ket), SRV))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
