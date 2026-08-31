# -*- coding: utf-8 -*-
"""t53_cuu_nguoi_ket_210.py - loi ra cho nguoi con ket o ban do 210.

RUI RO PHAT HIEN SAU KHI BO LOI DAI HON CHIEN:
  NPC bao danh cu (tinhnang\\loidaihonchien\\mainloidai.lua:42) chi kiem GIO
  (16h/22h) chu KHONG kiem cong tac BAT_LOIDAI_HONCHIEN. Cong tac de 0 da lau
  nhung NPC van cho vao, va khi vao thi bi dat:
      SetMask(2019) / SetPunish(1) / SetPKMode(2,1) / SetFightState(0)
      SetDeathScript(bigiet.lua) / NewWorld(210, ...)
  Doan ma dua nguoi choi RA nam trong ham LoiDaiHonChien() cua timerserver.lua
  - ham do vua bi go. Vay ai dang o 210 luc nay thi khong con ai dua ra.

Da kiem:
  - m_dwDeathScriptId chi nam trong bo nho (KPlayer.cpp:296 dat = 0 luc khoi
    tao; khong co ma luu/nap) -> thoat game la sach, khong ket vinh vien vi no.
  - Ban do 210 co trong BW_COMPETEMAP = {209,210,211} (missions\\bw\\bwhead.lua:9)
    nhung he Ty Vo chi dung BW_COMPETEMAP[1] = 209 (9/9 cho tham chieu), nen
    ham nay khong dam vao he Ty Vo.

CACH LAM: them mot ham vao script\\player\\playerlogin.lua, chay luc dang nhap:
neu nhan vat dang o ban do 210 thi tra ve Ba Lang Huyen va don sach trang thai
- dung DUNG chuoi lenh ma ban cu dung de dua nguoi ra (chep tu
timerserver.lua.truoc_boldhc).

Khi nao chac khong con ai ket thi go ham nay di duoc.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import re
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

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
P = os.path.join(S, "player", "playerlogin.lua")
MOC = "[BOLDHC 30/08]"
DUOI = ".truoc_cuu210"

CAU = ve.vn("Lôi Đài Hỗn Chiến đã ngừng tổ chức."
            " Ngươi được đưa về Ba Lăng Huyện.")


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t53 loi ra cho nguoi ket o ban do 210 - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()
    print("  Cau thong bao (sinh bang bo ma hoa TCVN3 cua bo ky nang,")
    print("  KHONG tu go byte): %d byte cao" % hi(CAU))
    print("    %s" % ve.doc_vn(CAU))
    print()

    raw = doc(P)
    if MOC in raw:
        print("  playerlogin.lua da co - bo qua")
        return 0
    dong = raw.split("\n")
    cb0, hi0 = lh.can_bang(raw), hi(raw)

    ham = [
        "",
        "-- %s LOI RA cho nguoi con ket o ban do 210." % MOC,
        "-- Hoat dong Loi Dai Hon Chien da bo han. NPC bao danh cu chi kiem GIO",
        "-- chu khong kiem cong tac, nen van co the co nguoi dang o trong do; ma",
        "-- dua ho ra nam trong ham LoiDaiHonChien() cua timerserver.lua - da go.",
        "-- Chuoi lenh don trang thai CHEP tu chinh ban cu (nhanh ket thuc tran).",
        "-- Khi chac khong con ai ket thi go ham nay di duoc.",
        "function CuuNguoiKetLDHC()",
        "\tlocal nW = GetWorldPos()",
        "\tif (nW ~= 210) then",
        "\t\treturn",
        "\tend",
        "\tSetDeathScript(\"\")",
        "\tSetCurCamp(GetCamp())",
        "\tReSetMask()",
        "\tSetFightState(0)",
        "\tSetPKMode(0,0)",
        "\tSetPunish(0)",
        "\tSetCreateTeam(1)",
        "\tSetLogoutRV(0)",
        "\tNewWorld(53, 1619, 3185)",
        "\tMsg2Player(\"%s\")" % CAU,
        "end",
    ]
    if lh.can_bang("\n".join(ham)) != 0:
        print("!!! LOI TO: ham moi khong can bang")
        return 1

    # chen loi goi trong main(), ngay sau CheckPK()
    ci = [i for i, l in enumerate(dong)
          if re.match(r"\s*CheckPK\(\)\s*\r?$", l)]
    if len(ci) != 1:
        print("!!! LOI TO: tim thay %d dong CheckPK() (can 1)" % len(ci))
        return 1
    i = ci[0]
    cr = "\r" if dong[i].endswith("\r") else ""
    thut = dong[i][:len(dong[i]) - len(dong[i].lstrip())]
    dong[i:i + 1] = [dong[i],
                     thut + "CuuNguoiKetLDHC()\t-- " + MOC + " loi ra ban do 210"
                     + cr]
    print("  chen loi goi CuuNguoiKetLDHC() sau CheckPK() (dong %d)" % (i + 2))

    # them than ham o cuoi tep
    dong = dong + [x + cr for x in ham]
    nd = "\n".join(dong)
    if lh.can_bang(nd) != cb0:
        print("!!! LOI TO: can bang tu khoa doi (%d -> %d)"
              % (cb0, lh.can_bang(nd)))
        return 1
    print("  can bang tu khoa %d giu nguyen" % cb0)
    print("  byte tieng Viet: %d -> %d (+%d = dung cau vua them)"
          % (hi0, hi(nd), hi(nd) - hi0))
    if hi(nd) - hi0 != hi(CAU):
        print("!!! LOI TO: byte tieng Viet tang khong dung bang cau them vao")
        return 1

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
    print("  DA GHI playerlogin.lua")
    return 0


if __name__ == "__main__":
    sys.exit(main())
