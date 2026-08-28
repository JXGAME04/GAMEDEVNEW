# -*- coding: utf-8 -*-
"""x4_soi_them_phong_dopho.py - Them phong DO PHO (10) vao cong cu "Soi o lo ren".

CHU GAME: "bo do pho vo la mat, khong hien hinh" + "do pho van khong hien thong tin".

Truoc khi doan tiep, can biet MOT dieu duy nhat: MAY CHU CO NHAN mon do vao
phong 10 hay khong.
  - Neu SOI THAY mon trong phong 10  => may chu nhan tot, loi nam o phia CLIENT
    (khong ve o / anh xa vi tri sai).
  - Neu SOI KHONG THAY               => mon khong vao duoc phong, loi nam o
    duong dat vat pham phia may chu.
Hai huong sua hoan toan khac nhau, nen phai do truoc.

Cong cu `LR_UI_Soi` da co san (bang LR_UI_PHONG) nhung chi liet ke phong 4..9,
chua co phong 10 cua the Do pho. Mieng va nay chi them mot dong vao bang do.

CHI LA KICH BAN LUA - khong phai build, nap lai script la dung duoc ngay.
Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import shutil
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\viemde")

DICH = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
        r"\script\item\compound\compound_ui.lua")
HAU_TO = ".truoc_soidopho"
T = "\t"

CU = T + '{ 9, 11,  3, "Kham nam" },'
MOI = (T + '{ 9, 11,  3, "Kham nam" },' + "\n"
       + T + '{ 10, 9,  8, "Do pho Hoang Kim" },')


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== x4_soi_them_phong_dopho - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"

    if '{ 10, 9,  8, "Do pho Hoang Kim" }' in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    cu = CU
    if raw.count(cu) != 1:
        print("!!! LOI TO: moc neo khop %d lan (can 1)" % raw.count(cu))
        return 1
    nd = raw.replace(cu, MOI.replace("\n", eol), 1)
    try:
        nd.encode("latin-1")
    except UnicodeEncodeError as e:
        print("!!! LOI TO: ngoai latin-1: %s" % e)
        return 1
    print("  ok  them dong phong 10 (9 o, o thu 8 tro di la PHU)")

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
    with io.open(DICH, "wb") as f:
        f.write(nd.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI. Nap lai script la dung duoc (KHONG phai build).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
