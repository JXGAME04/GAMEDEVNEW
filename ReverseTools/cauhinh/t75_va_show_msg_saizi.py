# -*- coding: utf-8 -*-
"""t75_va_show_msg_saizi.py - va loi "attempt to call a string value" o saizi.lua.

TRIEU CHUNG (chu game bao):
    ScriptError 4:[1] (\\script\\missions\\yandibaozang\\saizi.lua)
                      cFuncName:(YDBZ_OnTimeOver)
Vet ngan xep trong ScriptError.log (30/08 16:36 va 16:37):
    error: attempt to call a string value
       1: function `CallPlayerFunction'   [C]
       2: function `YDBZ_show_msg'        line 50
       3: function `YDBZ_show_roll_info'  line 92
       4: function `YDBZ_OnTimeOver'      line 45

GOC: saizi.lua:48-52
    function YDBZ_show_msg(list, msg)
        for i=1, getn(list) do
            CallPlayerFunction(list[i], msg)     <-- msg la CHUOI
        end
    end
Goi tu :92  YDBZ_show_msg(tbPlayerList, str)   voi str la chuoi thong bao.

CallPlayerFunction cua JX1 doi tham so 2 la MOT HAM, khong phai chuoi:
    ScriptFuns.cpp:2232-2258  lua_pushvalue(L, 2) roi lua_rawcall(...)
Moi cho dung dung trong cay deu truyen HAM:
    battles\\weeklyrank.lua:9   CallPlayerFunction(idx, GetTask, 751)
    battles\\weeklyrank.lua:11  CallPlayerFunction(idx, GetName)
    battles\\weeklyrank.lua:69  CallPlayerFunction(idx, CreateNewSayEx, ...)

Y DINH cua doan nay ro rang: gui chuoi msg cho tung nguoi trong danh sach.
VA: truyen HAM Msg2Player, con chuoi thanh tham so thu ba - dung co che ma
CallPlayerFunction sinh ra de lam (no day tham so 3..n vao ham).

    CallPlayerFunction(list[i], Msg2Player, msg)

Them mot chot: bo qua phan tu khong hop le (nil hoac <= 0) - danh sach nguoi
choi co the thua khi co nguoi thoat giua chung (chinh C++ ghi ro dieu do o
KItemDice.cpp:714-717).

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
     r"\missions\yandibaozang\saizi.lua")
MOC = "[VASAIZI 30/08]"
CU = "CallPlayerFunction(list[i], msg)"


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t75 va loi CallPlayerFunction(chuoi) - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()
    raw = doc(P)
    if MOC in raw:
        print("  da va - bo qua")
        return 0
    dong = raw.split("\n")
    ci = [i for i, l in enumerate(dong)
          if CU in l and not l.lstrip().startswith("--")]
    if len(ci) != 1:
        print("!!! LOI TO: khop %d dong (can 1): %s" % (len(ci), CU))
        return 1
    i = ci[0]
    cr = "\r" if dong[i].endswith("\r") else ""
    thut = dong[i][:len(dong[i]) - len(dong[i].lstrip())]
    hi0, cb0 = hi(raw), lh.can_bang(raw)
    print("  dong %d cu : %s" % (i + 1, dong[i].strip()))

    dong[i:i + 1] = [
        thut + "-- " + MOC + " CallPlayerFunction doi tham so 2 la MOT HAM" + cr,
        thut + "-- (ScriptFuns.cpp:2232-2258 lua_pushvalue(L,2) roi rawcall)."
        + cr,
        thut + "-- Truoc day truyen thang chuoi msg -> 'attempt to call a"
        " string value'." + cr,
        thut + "-- Dung co che san co: ham o vi tri 2, tham so tu vi tri 3."
        + cr,
        thut + "if (list[i] ~= nil and list[i] > 0) then" + cr,
        thut + "\tCallPlayerFunction(list[i], Msg2Player, msg)" + cr,
        thut + "end" + cr,
    ]
    nd = "\n".join(dong)

    cb1 = lh.can_bang(nd)
    if cb1 != cb0:
        print("!!! LOI TO: can bang tu khoa doi (%d -> %d)" % (cb0, cb1))
        return 1
    if hi(nd) != hi0:
        print("!!! LOI TO: byte tieng Viet doi (%d -> %d)" % (hi0, hi(nd)))
        return 1
    print("  dong moi  : CallPlayerFunction(list[i], Msg2Player, msg)")
    print("  can bang %d giu nguyen; byte tieng Viet giu nguyen" % cb1)

    if not ghi:
        print()
        print("DIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    sao = P + ".truoc_vasaizi"
    if not os.path.isfile(sao):
        shutil.copy2(P, sao)
    with io.open(P, "wb") as f:
        f.write(nd.encode("latin-1"))
    if doc(P) != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI saizi.lua")
    return 0


if __name__ == "__main__":
    sys.exit(main())
