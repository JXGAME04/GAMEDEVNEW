# -*- coding: utf-8 -*-
"""t54_va_thutu_longhuyethoan.py - sua THU TU trong longxuewan.lua toi vua port.

LOI (bo phan bien bat duoc): tep toi port cong 3 task RUOI moi goi
RemoveItemByIndex, va KHONG kiem gia tri tra ve.
    RemoveItemByIndex tra 1 khi thanh cong, 0 khi that bai
    (KJx2WarInfra.cpp:896-916, nOk = 1 chi khi RemoveItemIdx thanh cong).
Neu tru that bai thi nguoi choi DA duoc cong luot ma vien thuoc VAN CON - nhan
duoc luot mien phi khong gioi han.

SUA: tru vat pham TRUOC; tru khong duoc thi thoi, khong cong gi.

Chi DOI CHO cac dong san co, KHONG go lai cau tieng Viet nao (chep nguyen byte).

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lua_ham as lh  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

P = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
     r"\item\longxuewan.lua")
DUOI = ".truoc_thutu"
MOC = "[THUTU 30/08]"


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t54 sua thu tu tru vat pham - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()
    raw = doc(P)
    if MOC in raw:
        print("  da sua - bo qua")
        return 0
    dong = raw.split("\n")
    cb0, hi0 = lh.can_bang(raw), hi(raw)

    i_rm = [i for i, l in enumerate(dong) if "RemoveItemByIndex" in l]
    if len(i_rm) != 1:
        print("!!! LOI TO: tim thay %d dong RemoveItemByIndex" % len(i_rm))
        return 1
    i_set = [i for i, l in enumerate(dong)
             if l.lstrip().startswith("SetTask(LXW_TSK_REMAIN_COUNT,"
                                      " GetTask(")]
    if len(i_set) != 1:
        print("!!! LOI TO: tim thay %d dong SetTask dau" % len(i_set))
        return 1
    a, b = i_set[0], i_rm[0]
    if not (a < b):
        print("  thu tu da dung roi")
        return 0
    print("  hien tai: cong luot o dong %d..%d, tru vat pham o dong %d"
          % (a + 1, b - 1, b + 1))

    cr = "\r" if dong[a].endswith("\r") else ""
    thut = dong[a][:len(dong[a]) - len(dong[a].lstrip())]
    khoi = dong[a:b]                       # 3 SetTask + Say (giu nguyen byte)
    guard = [
        thut + "-- " + MOC + " TRU VAT PHAM TRUOC roi moi cong luot."
        " RemoveItemByIndex tra 1/0" + cr,
        thut + "-- (KJx2WarInfra.cpp:896-916); tru khong duoc ma van cong la"
        " cho luot mien phi." + cr,
        thut + "if (RemoveItemByIndex(nItemIndex) ~= 1) then" + cr,
        thut + "\treturn 1" + cr,
        thut + "end" + cr,
    ]
    dong[a:b + 1] = guard + khoi
    nd = "\n".join(dong)

    if lh.can_bang(nd) != cb0:
        print("!!! LOI TO: can bang tu khoa doi (%d -> %d)"
              % (cb0, lh.can_bang(nd)))
        return 1
    if hi(nd) != hi0:
        print("!!! LOI TO: byte tieng Viet doi (%d -> %d)" % (hi0, hi(nd)))
        return 1
    print("  sau khi sua: tru vat pham + kiem ket qua TRUOC, roi moi cong luot")
    print("  can bang %d giu nguyen; byte tieng Viet %d giu nguyen"
          % (cb0, hi0))

    if not ghi:
        print()
        print("DIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    sao = P + DUOI
    if not os.path.isfile(sao):
        shutil.copy2(P, sao)
    with io.open(P, "wb") as f:
        f.write(nd.encode("latin-1"))
    if doc(P) != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI longxuewan.lua")
    return 0


if __name__ == "__main__":
    sys.exit(main())
