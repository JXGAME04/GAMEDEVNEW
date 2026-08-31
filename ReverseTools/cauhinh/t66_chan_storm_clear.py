# -*- coding: utf-8 -*-
"""t66_chan_storm_clear.py - chan qua min storm_clear().

script\\event\\storm\\function.lua:392-400
    function storm_clear(gameid)
        if (gameid == nil) then
            ...
            for i = 1661, 16710 do SetTask(i,0); end

Goi storm_clear() KHONG THAM SO la xoa trang MOI task tu 1661 toi tran
(MAX_TASK) cua nhan vat do - nuot luon:
    2641/2642/4018  Long Huyet Hoan (vua noi hom nay)
    1825/2419/2420/2690/2574  tasklink Da Tau
    ca dai chuyen sinh
Hien KHONG noi nao goi (grep toan cay chi ra dung dong dinh nghia), nhung ham
duoc NAP SAN vao 4 trang thai Lua dang chay:
    battles\\battlehead.lua:4 · global\\seasonnpc.lua:9
    challengeoftime\\mission_match.lua:6 · challengeoftime\\npc_death.lua:10

VA: nhanh "xoa tat" khong con chay khi goi thieu tham so. Muon xoa tat thi phai
goi ro rang storm_clear(-1). Nhanh theo gameid (goi dung) GIU NGUYEN.

KHONG doi hanh vi cua bat ky duong goi nao dang chay - vi khong co duong nao.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
import lua_ham as lh  # noqa: E402
import vn_edit as ve  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

P = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
     r"\event\storm\function.lua")
MOC = "[CHANMIN 30/08]"
CAU = ve.vn("Lệnh này xoá sạch dữ liệu nhiệm vụ của nhân vật."
            " Phải gọi rõ storm_clear(-1) mới chạy.")


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t66 chan min storm_clear - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()
    print("  Cau canh bao (sinh bang bo ma hoa TCVN3, khong go tay byte):")
    print("    %s" % ve.doc_vn(CAU))
    print()
    raw = doc(P)
    if MOC in raw:
        print("  da chan - bo qua")
        return 0
    dong = raw.split("\n")
    ci = [i for i, l in enumerate(dong)
          if l.strip() == "if (gameid == nil) then"]
    if len(ci) != 1:
        print("!!! LOI TO: khop %d dong 'if (gameid == nil) then'" % len(ci))
        return 1
    i = ci[0]
    cr = "\r" if dong[i].endswith("\r") else ""
    thut = dong[i][:len(dong[i]) - len(dong[i].lstrip())]
    hi0, cb0 = hi(raw), lh.can_bang(raw)

    moi = [
        thut + "-- " + MOC + " CHOT AN TOAN. Nhanh duoi day xoa trang MOI task"
        " tu 1661" + cr,
        thut + "-- toi tran cua nhan vat (nuot ca Long Huyet Hoan 2641/2642/4018"
        " va" + cr,
        thut + "-- tasklink Da Tau). Truoc day chi can goi storm_clear() thieu"
        " tham so" + cr,
        thut + "-- la no chay. Nay phai goi ro storm_clear(-1)." + cr,
        thut + "if (gameid == nil) then" + cr,
        thut + "\tMsg2Player(\"" + CAU + "\")" + cr,
        thut + "\treturn nil" + cr,
        thut + "end" + cr,
        thut + "if (gameid == -1) then" + cr,
    ]
    dong[i:i + 1] = moi
    nd = "\n".join(dong)

    # Khoi chen vao la TRON VEN: `if (gameid == nil) then ... end` (mo 1, dong 1),
    # roi `if (gameid == -1) then` THAY CHO dong `if (gameid == nil) then` cu -
    # van do `end` cu cua ham dong lai. Nen can bang phai GIU NGUYEN.
    cb1 = lh.can_bang(nd)
    if cb1 != cb0:
        print("!!! LOI TO: can bang tu khoa doi (%d -> %d)" % (cb0, cb1))
        return 1
    if hi(nd) != hi0 + hi(CAU):
        print("!!! LOI TO: byte tieng Viet tang khong dung bang cau them"
              " (%d -> %d, cau %d byte)" % (hi0, hi(nd), hi(CAU)))
        return 1
    print("  can bang tu khoa %d giu nguyen" % cb0)
    print("  byte tieng Viet %d -> %d (+%d = dung cau canh bao)"
          % (hi0, hi(nd), hi(CAU)))
    print("  dong %d: nhanh 'xoa tat' nay doi hoi storm_clear(-1)" % (i + 1))

    if not ghi:
        print()
        print("DIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    sao = P + ".truoc_chanmin"
    if not os.path.isfile(sao):
        shutil.copy2(P, sao)
    with io.open(P, "wb") as f:
        f.write(nd.encode("latin-1"))
    if doc(P) != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI function.lua")
    return 0


if __name__ == "__main__":
    sys.exit(main())
