# -*- coding: utf-8 -*-
"""v30 - chuyen GDCNI_DICE_ITEM xuong CUOI enum.

Ly do: v29 chen no ngay sau GDCNI_PLAYER_BAUCUA_RESULT_SYNC, tuc lam DICH gia tri
cua MOI thanh vien phia sau (GDCNI_EXIT_GAME, GDCNI_UI_ACT, GDCNI_TONG_JX2,
GDCNI_TASK_VALUE_UPDATE). Enum nay duoc CA HAI ben bien dich - Core (CoreClient.dll)
va S3Client (Game.exe) - nen neu chi build lai mot ben thi hai ben goi nham callback.

Dat o CUOI thi khong gia tri cu nao bi doi, va neu lo thieu mot ben thi cung chi
la "khong biet gia tri moi" chu khong phai "hieu nham gia tri cu". Day dung tinh
than ky luat da ap dung cho enum giao thuc (memory 'protocol-add-order').
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

P = r"D:\GAMEDEVNEW\Sources\Core\Src\CoreShell.h"
HAU_TO = ".truoc_xucxac_2608"

KHOI = (
    "\r\n"
    "\t// He XUC XAC chia do (DICEITEM) 26/08: may chu bao mo/dong/cap nhat mot o.\r\n"
    "\t// uParam = con tro DICE_ITEM_SYNC, CHI SONG trong loi goi - phai chep ra ngay.\r\n"
    "\tGDCNI_DICE_ITEM,\r\n")

NEO_CUOI = "\tGDCNI_TASK_VALUE_UPDATE,\t// [TaskGuide] uParam = task id, nParam = gia tri\r\n"


def main():
    d = io.open(P, "rb").read().decode("latin-1")

    if d.count(KHOI) != 1:
        print("!! khong tim thay dung 1 khoi GDCNI_DICE_ITEM da chen (%d)" % d.count(KHOI))
        return 2
    if d.count(NEO_CUOI) != 1:
        print("!! mo neo cuoi enum khong duy nhat (%d)" % d.count(NEO_CUOI))
        return 2

    # go khoi o cho cu roi dat xuong cuoi
    d2 = d.replace(KHOI, "", 1)
    d2 = d2.replace(NEO_CUOI, NEO_CUOI + KHOI, 1)

    if d2.count("GDCNI_DICE_ITEM") != 1:
        print("!! sau khi chuyen co %d lan GDCNI_DICE_ITEM" % d2.count("GDCNI_DICE_ITEM"))
        return 2

    b = d2.encode("latin-1")
    goc = io.open(P, "rb").read()
    if (b.count(b"\n") - b.count(b"\r\n")) > (goc.count(b"\n") - goc.count(b"\r\n")):
        print("!! sinh them dong LF le")
        return 2
    if b.count(b"\n") != goc.count(b"\n"):
        print("!! so dong thay doi %d -> %d" % (goc.count(b"\n"), b.count(b"\n")))
        return 2

    bak = P + HAU_TO
    if not os.path.isfile(bak):
        shutil.copy2(P, bak)
    tmp = P + ".tmp"
    io.open(tmp, "wb").write(b)
    os.replace(tmp, P)
    print("   > da chuyen GDCNI_DICE_ITEM xuong cuoi enum")

    # in lai de mat thuong soi
    L = d2.split("\r\n")
    i = [k for k, s in enumerate(L) if "GDCNI_DICE_ITEM" in s][0]
    for j in range(i - 4, i + 3):
        if 0 <= j < len(L):
            print("   %d: %s" % (j + 1, L[j]))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
