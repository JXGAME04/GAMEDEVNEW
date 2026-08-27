# -*- coding: utf-8 -*-
"""v26 - sua THU TU khai bao s2cDiceItem trong KProtocolProcess.h.

Chu game bat loi: ".h dang sai thu tu so voi .cpp".

Dung vay. v24 neo nham vao 's2cSyncBauCuaResult' nen dat s2cDiceItem o giua:

    .h  (SAI)                        .cpp (dang ky - chuan)
    166 s2cSyncMagic                 228 s2c_syncmagic
    167 s2cSyncMeridian              229 s2c_syncmeridian
    168 s2cSyncBauCuaResult          230 s2c_syncbaucuaresult
    169 s2cDiceItem   <-- SAI        231 s2c_syncbaucuainfo  (dung lai s2cSyncBauCuaResult)
    170 s2cPlayerSync                232 s2c_playersync
    171 s2cRemoveAllItem             233 s2c_removeallitem
                                     234 s2c_diceitem        <-- phai o CUOI

Ve mat bien dich thi vi tri khai bao khong anh huong, nhung day la quy uoc cua
cay nay (xem memory 'protocol-add-order': chu game doi chieu tung tep, moi tep
deu phai cung thu tu) - va de nguoi doc sau khong hieu nham thu tu giao thuc.

Phia c2s v24 lam DUNG roi (c2sSetMeridian -> c2sBauCua -> c2sDiceItem khop
.cpp), ban va nay khong dung toi.
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

P = r"D:\GAMEDEVNEW\Sources\Core\Src\KProtocolProcess.h"
HAU_TO = ".truoc_xucxac_2608"

DONG = "\tvoid\ts2cDiceItem(BYTE* pMsg);\r\n"
NEO_CU = "\tvoid\ts2cSyncBauCuaResult(BYTE* pMsg);\r\n" + DONG
NEO_CU_SACH = "\tvoid\ts2cSyncBauCuaResult(BYTE* pMsg);\r\n"
NEO_MOI = "\tvoid\ts2cRemoveAllItem(BYTE* pMsg);\r\n"


def main():
    d = io.open(P, "rb").read().decode("latin-1")

    if d.count(DONG) != 1:
        print("!! tim thay %d dong s2cDiceItem, can dung 1" % d.count(DONG))
        return 2
    if d.count(NEO_MOI) != 1:
        print("!! neo s2cRemoveAllItem khong duy nhat (%d)" % d.count(NEO_MOI))
        return 2

    # da dung cho chua?
    if NEO_MOI + DONG in d:
        print("   = da dung thu tu, khong phai sua")
        return 0
    if NEO_CU not in d:
        print("!! khong thay s2cDiceItem ngay sau s2cSyncBauCuaResult - dung lai de nguoi kiem")
        return 2

    # 1. go dong dat sai cho
    d2 = d.replace(NEO_CU, NEO_CU_SACH, 1)
    # 2. dat lai ngay sau s2cRemoveAllItem (khop thu tu dang ky trong .cpp)
    d2 = d2.replace(NEO_MOI, NEO_MOI + DONG, 1)

    if d2.count(DONG) != 1:
        print("!! sau khi sua co %d dong s2cDiceItem" % d2.count(DONG))
        return 2
    b = d2.encode("latin-1")
    goc = io.open(P, "rb").read()
    if b.count(b"\n") != goc.count(b"\n"):
        print("!! so dong thay doi: %d -> %d" % (goc.count(b"\n"), b.count(b"\n")))
        return 2
    if b.count(b"\n") - b.count(b"\r\n"):
        print("!! sinh ra dong LF le")
        return 2

    bak = P + HAU_TO
    if not os.path.isfile(bak):
        shutil.copy2(P, bak)
    tmp = P + ".tmp"
    io.open(tmp, "wb").write(b)
    os.replace(tmp, P)
    print("   > da chuyen s2cDiceItem xuong sau s2cRemoveAllItem")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
