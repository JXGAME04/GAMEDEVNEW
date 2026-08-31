# -*- coding: utf-8 -*-
"""t67_tat_dong_chet_bu_hoatdong.py - tat 5 dong CHET trong bang "bu hoat dong".

HIEN TRANG (do bang t47 + t48, quet ca theo TEN HANG lan SO HIEU, moi dang ghi):

  #  ten        task dem luot        ai GHI task do
  1  PLD        TSK_NUMPLDNGAY  315  KHONG AI
  2  VA         T_NVVATRONGNGAY  43  KHONG AI
  3  VT         T_NVVTTRONGNGAY  49  event_vantieu\\tieudau.lua:248/319/341  <- SONG
  4  Hang Ngay  TASK_TANTHU3    345  KHONG AI (lenhbaitanthu.lua:110 da comment)
  5  Vip        TASK_NEWTHOREN6 377  KHONG AI (lenhbaitanthu.lua:111 da comment)
  6  DT         T_SoNVTrongNgay  87  chi he Da Tau CU - da go sang _dara hom nay

Hai ham chay MOI NGAY cho MOI nhan vat (lib_ham.lua:232 va :291, trong
updatengaymoi):
  LuuTrangThaiHoatDongHomQua()  ghi cot 6 -> task 390..397
  CapNhatThieuHoatDong()        ghi cot 3 -> task 383..395
Voi 5 dong chet, moi ngay moi nhan vat bi cong them "no ao" (so luot thieu) toi
tran MAX_BU_DAYS * nCan, roi nam do VINH VIEN.

VA KHONG CO CHO NAO PHAT: quet toan cay, khong noi nao DOC TASK_HD_MISS_* de tra
thuong (cho duy nhat nhac toi la lenhbaitanthu.lua:107, DA COMMENT).

=> Tat 5 dong chet la: (a) thoi ghi rac vao 10 task cua nguoi choi moi ngay,
   (b) KHONG doi gi ma nguoi choi thay duoc (vi khong co cho phat).

BAY PHAI TRANH: hai ham duyet bang bang `while TB_BU_HD[i] do ... i = i + 1`.
Chi can [1] la nil thi vong lap DUNG NGAY - cac dong sau khong chay. Nen KHONG
duoc comment [1] va [2] roi de [3] nguyen chi so; phai DANH SO LAI dong con song
thanh [1].

Muon bat lai: bo comment dong tuong ung VA danh so lai cho lien tuc tu [1].
Nhung truoc do phai quyet dinh he MOI dem luot bang task nao - do la viec thiet
ke, khong phai don dep.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import re
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lua_ham as lh  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

P = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
     r"\lib\lib_ham.lua")
MOC = "[BUHD 30/08]"
GIU = "VT"          # dong duy nhat con he ghi that


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t67 tat 5 dong chet trong TB_BU_HD - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()
    raw = doc(P)
    if MOC in raw:
        print("  da tat - bo qua")
        return 0
    dong = raw.split("\n")

    i0 = [i for i, l in enumerate(dong) if l.startswith("TB_BU_HD")]
    if len(i0) != 1:
        print("!!! LOI TO: khop %d dong TB_BU_HD =" % len(i0))
        return 1
    a = i0[0]
    b = None
    for i in range(a, len(dong)):
        if dong[i].rstrip("\r") == "}":
            b = i
            break
    if b is None:
        print("!!! LOI TO: khong tim thay dau dong bang")
        return 1

    muc = []
    for i in range(a + 1, b):
        m = re.match(r'\s*\[(\d+)\]\s*=\s*\{\s*"([^"]*)"', dong[i])
        if m and not dong[i].lstrip().startswith("--"):
            muc.append((i, m.group(1), m.group(2)))
    print("  bang co %d dong dang bat" % len(muc))
    giu = [x for x in muc if x[2] == GIU]
    if len(giu) != 1:
        print("!!! LOI TO: tim thay %d dong '%s'" % (len(giu), GIU))
        return 1
    i_giu = giu[0][0]
    cr = "\r" if dong[i_giu].endswith("\r") else ""
    than_giu = dong[i_giu].rstrip("\r")
    # danh so lai thanh [1]
    than_giu_moi = re.sub(r"\[\d+\]", "[1]", than_giu, count=1)
    print("  giu (danh so lai thanh [1]): %s" % than_giu_moi.strip()[:92])

    moi = [
        "\t-- " + MOC + " CHI CON MOT DONG. Nam dong kia dem bang task ma"
        " KHONG AI GHI" + cr,
        "\t-- (PLD 315 / VA 43 / Hang Ngay 345 / Vip 377 / DT 87) - he cu da go,"
        " he" + cr,
        "\t-- moi dem bang task khac. De nguyen thi moi ngay moi nhan vat bi"
        " cong 'no ao'" + cr,
        "\t-- vao task 383..397 roi nam do vinh vien, trong khi KHONG CO CHO NAO"
        " PHAT" + cr,
        "\t-- (lenhbaitanthu.lua:107 da comment). Tat di khong doi gi nguoi choi"
        " thay." + cr,
        "\t--" + cr,
        "\t-- MUON BAT LAI: bo comment dong tuong ung VA danh so lai lien tuc tu"
        " [1]," + cr,
        "\t-- vi hai ham duyet bang bang `while TB_BU_HD[i]` - ho mot so la dung"
        " ngay." + cr,
        "\t-- Truoc do phai quyet he MOI dem luot bang task nao (viec thiet ke)."
        + cr,
        than_giu_moi + cr,
    ]
    for i, so, ten in muc:
        if i == i_giu:
            continue
        moi.append("\t-- " + dong[i].strip().rstrip("\r") + cr)

    dong[a + 1:b] = moi
    nd = "\n".join(dong)

    cb0, cb1 = lh.can_bang(raw), lh.can_bang(nd)
    if cb1 != cb0:
        print("!!! LOI TO: can bang tu khoa doi (%d -> %d)" % (cb0, cb1))
        return 1
    if hi(nd) != hi(raw):
        print("!!! LOI TO: byte tieng Viet doi (%d -> %d)" % (hi(raw), hi(nd)))
        return 1
    print("  tat %d dong, can bang %d giu nguyen, byte tieng Viet giu nguyen"
          % (len(muc) - 1, cb1))

    if not ghi:
        print()
        print("DIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    sao = P + ".truoc_buhd"
    if not os.path.isfile(sao):
        shutil.copy2(P, sao)
    with io.open(P, "wb") as f:
        f.write(nd.encode("latin-1"))
    if doc(P) != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI lib_ham.lua")
    return 0


if __name__ == "__main__":
    sys.exit(main())
