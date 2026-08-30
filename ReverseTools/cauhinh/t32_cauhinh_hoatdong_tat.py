# -*- coding: utf-8 -*-
"""t32_cauhinh_hoatdong_tat.py - noi day cac hoat dong DANG TAT, de bat lai an toan.

Ca hai hoat dong duoi day dang TAT (BAT_TRONG_BANGHOI = 0,
BAT_LOIDAI_HONCHIEN = 0). Noi truoc de khi chu muon bat thi chinh duoc ngay,
va de thay ro cac con so truoc khi bat.

⚠️ TRONG BANG HOI - vi sao phai xem so truoc khi bat:
   EXP_RAN1 / EXP_RAN2 la exp cho MOI NHIP, va mot lan danh trong co MUOI nhip
   (tungtung1.lua:58 `for i=1,10`), moi nhip 5 giay. Tran ngay EXP_TBH_DAY chi
   duoc kiem TRUOC vong lap nen con vuot them mot nhip nua.

⚠️ LOI DAI HON CHIEN - bo khao sat neu 5 diem ho exp, trong do co:
   - phut 53 chi can `count == 1` (mot nguoi bao danh don) la an 500 trieu exp
     + 5 manh hoang kim, 2 lan/ngay
   - tran 4 mang bi xoa moi lan bao danh lai
   - khi chet KHONG do nguoi choi thi exp roi vao chinh NGUOI VUA CHET
   PHAI VA NHUNG CHO DO TRUOC KHI BAT. Ban va nay chi dua con so ra cau hinh,
   KHONG sua logic.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import noi_cauhinh as nc  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
P_LICH = os.path.join(S, "cauhinh", "ch_lich.lua")
P_THUONG = os.path.join(S, "cauhinh", "ch_thuong.lua")
NHAN = "[CFGTAT 30/08]"

DOT = [
    dict(
        tep=os.path.join(S, "event", "trongbanghoi", "lib.lua"),
        cfg=P_THUONG, bang="tbCFG_THUONG", ten_ham="TBH_CFG",
        tieu_de=[
            NHAN + " TRONG BANG HOI (event\\trongbanghoi\\lib.lua)"
            " - DANG TAT",
            "",
            "!! Doc ky truoc khi bat: TBH_EXP_MIN/MAX la exp cho MOI NHIP, va",
            "!! mot lan danh trong co MUOI nhip (tungtung1.lua:58), moi nhip 5",
            "!! giay. Tran ngay chi duoc kiem TRUOC vong lap nen con vuot them",
            "!! mot nhip.",
        ],
        muc=[
            ("TIME_LIFE_TRONG1", "TBH_PHUT_TRONG_TON_TAI",
             "so PHUT trong do bang chu tu danh ra ton tai"
             " (ma viet dang 30*60*18 nen day la con so dau)"),
            ("nStartTime", "TBH_GIO_MO",
             "gio som nhat trong ngay duoc dung Trong Khai Hoan, dang HHMM."
             " !! ten bien trong ma rat chung"),
            ("nCloseTime", "TBH_GIO_DONG",
             "gio muon nhat trong ngay duoc dung Trong Khai Hoan, dang HHMM"),
            ("EXP_TBH_DAY", "TBH_EXP_TRAN_NGAY",
             "tran exp moi nguoi nhan trong mot ngay tu danh trong bang"),
            ("EXP_RAN1", "TBH_EXP_MIN",
             "exp thap nhat cho MOI NHIP (mot lan danh co 10 nhip)"),
            ("EXP_RAN2", "TBH_EXP_MAX",
             "exp cao nhat cho MOI NHIP (mot lan danh co 10 nhip)"),
        ],
    ),
    dict(
        tep=os.path.join(S, "tinhnang", "loidaihonchien", "mainloidai.lua"),
        cfg=P_LICH, bang="tbCFG_LICH", ten_ham="LDHC_CFG",
        tieu_de=[
            NHAN + " LOI DAI HON CHIEN - DANG TAT",
            "",
            "!! PHAI VA MAY CHO HO EXP TRUOC KHI BAT - xem BAOCAO_LOHONG_2908.md",
        ],
        muc=[
            ("cost", "LDHC_PHI_BAODANH",
             "phi bao danh Loi Dai Hon Chien (luong). Dang de 0 nen vao mien phi"),
        ],
    ),
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t32_cauhinh_hoatdong_tat - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    for d in DOT:
        print("--- %s ---" % os.path.basename(d["tep"]))
        kq = nc.noi(tep=d["tep"], tep_cfg=d["cfg"], bang_cfg=d["bang"],
                    ten_ham=d["ten_ham"], nhan=NHAN, muc=d["muc"],
                    tieu_de=d.get("tieu_de"))
        if not nc.in_ket(kq):
            return 1
        if ghi and not nc.ghi(kq, hau_to=".truoc_cfgtat"):
            return 1
    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
