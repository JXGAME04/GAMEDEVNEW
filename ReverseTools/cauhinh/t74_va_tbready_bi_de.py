# -*- coding: utf-8 -*-
"""t74_va_tbready_bi_de.py - VA LOI VIEM DE "vao tran mot minh bao loi".

TRIEU CHUNG (chu game bao):
    ScriptError 4:[1] (\\script\\item\\lenhbaiadmin.lua) cFuncName:(TTHD_MM_Vao853)
Vet ngan xep trong ScriptError.log:
    error: attempt to call field `InitMatchMission' (a nil value)
       1: function `TTHD_MM_Vao'    at line 389  test_hoatdong_admin.lua
       2: function `TTHD_MM_Vao853' at line 398

KHONG PHAI LOI CUA DOT SUA 30/08: lan dau xuat hien 26/08/2026 17:30, tong 22
lan trong log (truoc khi toi dung den cay script).

GOC: HAI tep cung khai `tbReady = {}`
    missions\\yandibaozang\\readymap\\ready.lua:39    -> roi gan 18 PHUONG THUC
    missions\\yandibaozang\\readymap\\include.lua:42  -> gan 0 phuong thuc,
                                                       chi 6 truong so
Chuoi nap cua bo test:
    test_hoatdong_admin.lua:17 -> ydbz_driver.lua:16 -> yandibaozang_trigger.lua:10
                               -> ready.lua        (tbReady + 18 phuong thuc)  OK
    test_hoatdong_admin.lua:21 -> npc.lua:10 -> readymap\\include.lua:42
                               -> tbReady = {}     XOA SACH 18 phuong thuc
Sau do `tbReady` van KHAC nil (no la bang rong) nen chot o TTHD_MM_Vao:377
"if tbReady == nil" VAN QUA, roi :389 goi tbReady:InitMatchMission -> nil.

VA: guard dong 42 - chi tao bang khi CHUA co. Dung khuon da dung trong du an cho
`PetSys` (cung loi: Include khong guard lam head reset PetSys={} 8 lan).
Sau khi guard, 6 dong gan truong ben duoi VAN CHAY - da kiem: khong truong nao
trong 6 truong do bi ready.lua dat, nen khong the de nham gia tri.

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
     r"\missions\yandibaozang\readymap\include.lua")
MOC = "[VATBREADY 30/08]"


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t74 va loi tbReady bi de - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()
    raw = doc(P)
    if MOC in raw:
        print("  da va - bo qua")
        return 0
    dong = raw.split("\n")
    ci = [i for i, l in enumerate(dong) if l.rstrip("\r").strip() == "tbReady = {}"]
    if len(ci) != 1:
        print("!!! LOI TO: khop %d dong 'tbReady = {}' (can 1)" % len(ci))
        return 1
    i = ci[0]
    cr = "\r" if dong[i].endswith("\r") else ""
    hi0, cb0 = hi(raw), lh.can_bang(raw)
    print("  dong %d: %s" % (i + 1, dong[i].strip()))

    dong[i:i + 1] = [
        "-- " + MOC + " GUARD. Truoc day dong nay ghi thang tbReady = {}, nen khi" + cr,
        "-- tep duoc nap SAU readymap\\ready.lua thi no XOA SACH 18 phuong thuc ma" + cr,
        "-- ready.lua vua gan (InitMatchMission, ToMatch...). Bang van khac nil nen" + cr,
        "-- moi chot 'if tbReady == nil' deu qua, roi goi phuong thuc moi bao nil." + cr,
        "-- Trieu chung: bo test Viem De bam 'vao tran mot minh' bao ScriptError 4" + cr,
        "-- (co tu 26/08, 22 lan trong ScriptError.log). Cung loi da tung gap voi" + cr,
        "-- bang PetSys - va bang dung cach guard nay." + cr,
        "if (tbReady == nil) then" + cr,
        "\ttbReady = {}" + cr,
        "end" + cr,
    ]
    nd = "\n".join(dong)

    cb1 = lh.can_bang(nd)
    if cb1 != cb0:
        print("!!! LOI TO: can bang tu khoa doi (%d -> %d)" % (cb0, cb1))
        return 1
    if hi(nd) != hi0:
        print("!!! LOI TO: byte tieng Viet doi (%d -> %d)" % (hi0, hi(nd)))
        return 1
    print("  can bang tu khoa %d giu nguyen; byte tieng Viet giu nguyen" % cb1)

    if not ghi:
        print()
        print("DIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    sao = P + ".truoc_vatbready"
    if not os.path.isfile(sao):
        shutil.copy2(P, sao)
    with io.open(P, "wb") as f:
        f.write(nd.encode("latin-1"))
    if doc(P) != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI readymap\\include.lua")
    return 0


if __name__ == "__main__":
    sys.exit(main())
