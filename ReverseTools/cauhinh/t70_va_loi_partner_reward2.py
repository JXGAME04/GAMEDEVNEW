# -*- coding: utf-8 -*-
"""t70_va_loi_partner_reward2.py - VA LOI DO CHINH TOI GAY RA.

TRIEU CHUNG (chu game bao, thay tren console may chu):
    ScriptError 4:[1] (\\script\\task\\partner\\reward\\partner_reward2.lua)
    ====== [Total ScriptLoaded : 2948] ======

GOC: t65 doi partner_reward2.lua:15 tu
        TITLETIME = 30 * 24 * 60 * 60 * 18 * 36
     thanh
        TITLETIME = BDH_CFG("BDH_THOIHAN_DANHHIEU_NGAY", 30) * ...
     de cong tac cau hinh co tac dung. NHUNG:

     BDH_CFG duoc khai BEN TRONG partner_reward.lua:16 - la ham CUA RIENG TEP DO.
     partner_reward2.lua KHONG khai va KHONG Include partner_reward.lua.

     Khi hai tep cung duoc Include vao MOT trang thai (reward_partner.lua:7-8,
     changge_people.lua:9-10) thi thu tu reward -> reward2 lam BDH_CFG ton tai,
     nen toi tuong la du. SAI: engine con NAP SAN TUNG TEP mot luc khoi dong
     (dong "Total ScriptLoaded : 2948"). Nap doc lap thi BDH_CFG = nil ->
     goi ham nil ngay o than chunk -> ScriptError 4.

VA: cho partner_reward2.lua TU DU - chep nguyen khuon partner_reward.lua:11-21
    (Include ch_lib + ch_thuong roi khai BDH_CFG co guard).
    Khong doi hanh vi: khi bo cau hinh chua nap thi BDH_CFG tra ve mac dinh 30,
    dung bang so cung cu.

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

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
P1 = os.path.join(S, "task", "partner", "reward", "partner_reward.lua")
P2 = os.path.join(S, "task", "partner", "reward", "partner_reward2.lua")
MOC = "[VALOI 30/08]"


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t70 va loi ScriptError partner_reward2.lua - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()

    raw2 = doc(P2)
    if MOC in raw2:
        print("  da va - bo qua")
        return 0
    if "BDH_CFG" not in raw2:
        print("  tep khong dung BDH_CFG - khong can va")
        return 0

    # --- lay NGUYEN VAN khoi khai BDH_CFG tu partner_reward.lua ---
    d1 = doc(P1).split("\n")
    i_ham = [i for i, l in enumerate(d1)
             if l.strip().startswith("function BDH_CFG(")]
    if len(i_ham) != 1:
        print("!!! LOI TO: partner_reward.lua co %d ham BDH_CFG" % len(i_ham))
        return 1
    a = i_ham[0]
    r = lh.tim_ham([x.rstrip("\r") for x in d1], "BDH_CFG")
    if r is None:
        print("!!! LOI TO: khong do duoc ranh gioi BDH_CFG")
        return 1
    _, b = r
    # lui len lay ca hai dong Include ch_lib / ch_thuong
    i0 = a
    while i0 > 0 and ("Include" in d1[i0 - 1] and "cauhinh" in d1[i0 - 1]
                      or d1[i0 - 1].lstrip().startswith("--")):
        i0 -= 1
    khoi = [x.rstrip("\r") for x in d1[i0:b + 1]]
    print("  chep khoi tu partner_reward.lua dong %d..%d (%d dong):"
          % (i0 + 1, b + 1, len(khoi)))
    for x in khoi:
        print("     %s" % x.strip()[:96])
    if not any("function BDH_CFG(" in x for x in khoi):
        print("!!! LOI TO: khoi chep khong chua ham BDH_CFG")
        return 1
    if lh.can_bang("\n".join(khoi)) != 0:
        print("!!! LOI TO: khoi chep khong can bang")
        return 1

    # --- chen vao truoc dong dung BDH_CFG dau tien cua reward2 ---
    d2 = raw2.split("\n")
    ci = [i for i, l in enumerate(d2)
          if "BDH_CFG(" in l and not l.lstrip().startswith("--")]
    if not ci:
        print("!!! LOI TO: khong tim thay dong dung BDH_CFG")
        return 1
    i = ci[0]
    cr = "\r" if d2[i].endswith("\r") else ""
    hi0, cb0 = hi(raw2), lh.can_bang(raw2)
    chen = ["-- " + MOC + " partner_reward2.lua bi engine NAP DOC LAP luc khoi"
            " dong" + cr,
            "-- (dong 'Total ScriptLoaded'), khong chi qua reward_partner.lua."
            " Luc do" + cr,
            "-- BDH_CFG chua ton tai (no khai trong partner_reward.lua) ->"
            " ScriptError 4." + cr,
            "-- Chep nguyen khuon cua partner_reward.lua de tep nay TU DU." + cr]
    d2[i:i] = chen + [x + cr for x in khoi]
    nd = "\n".join(d2)

    cb1 = lh.can_bang(nd)
    if cb1 != cb0:
        print("!!! LOI TO: can bang tu khoa doi (%d -> %d)" % (cb0, cb1))
        return 1
    them = hi("\n".join(khoi))
    if hi(nd) != hi0 + them:
        print("!!! LOI TO: byte tieng Viet lech (%d -> %d, khoi chep %d)"
              % (hi0, hi(nd), them))
        return 1
    print()
    print("  chen vao truoc dong %d; can bang %d giu nguyen" % (i + 1, cb1))

    if not ghi:
        print()
        print("DIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    sao = P2 + ".truoc_valoi"
    if not os.path.isfile(sao):
        shutil.copy2(P2, sao)
    with io.open(P2, "wb") as f:
        f.write(nd.encode("latin-1"))
    if doc(P2) != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI partner_reward2.lua")
    return 0


if __name__ == "__main__":
    sys.exit(main())
