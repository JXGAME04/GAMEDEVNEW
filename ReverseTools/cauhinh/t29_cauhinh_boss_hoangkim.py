# -*- coding: utf-8 -*-
"""t29_cauhinh_boss_hoangkim.py - noi day BOSS HOANG KIM.

TRANG THAI: lich tu dong dang TAT (BAT_BOSS_HOANGKIM = 0), nhung boss VAN ra
duoc bang tay qua menu "Test Boss" cua lenh bai admin
(item\\lenhbaiadmin.lua:511-514 goi thang addnpcbosstieuhk / addnpcbossdaihk /
addnpcbossserver / addnpcbosspld). Luc do toan bo hang so duoi day AN THAT.

⚠️ SO DANG NGO NHAT: LIFE_BOSS_DAIH dang la 1 (mot mau) trong khi ghi chu ngay
   canh no noi mac dinh la 18.000.000. So nay dung cho CA Boss Dai Hoang Kim,
   Boss may chu VA Boss Phong Lang Do - tuc ca ba loai boss lon deu chet trong
   MOT NHAT. Rat giong dau vet mot lan thu roi bo quen.
   TOI KHONG TU DOI - chu game chot. Sau ban va chinh o ch_chung.lua.

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
P_CHUNG = os.path.join(S, "cauhinh", "ch_chung.lua")
P_THUONG = os.path.join(S, "cauhinh", "ch_thuong.lua")
NHAN = "[CFGBHK 30/08]"

DOT = [
    dict(
        tep=os.path.join(S, "tinhnang", "boss_hoangkim", "lib_bosshk.lua"),
        cfg=P_CHUNG, bang="tbCFG_CHUNG", ten_ham="BHK_CFG",
        tieu_de=[
            NHAN + " BOSS HOANG KIM - suc manh va thoi gian ton tai",
            "(tinhnang\\boss_hoangkim\\lib_bosshk.lua)",
            "",
            "!! Lich tu dong dang TAT (BAT_BOSS_HOANGKIM = 0), nhung boss van ra",
            "!! duoc bang tay qua menu 'Test Boss' cua lenh bai admin - luc do",
            "!! cac so nay an that.",
            "",
            "!! BHK_MAU_BOSS_DAI dang la 1 (MOT mau) trong khi ghi chu trong ma",
            "!! noi mac dinh la 18.000.000. So nay dung cho CA boss dai, boss may",
            "!! chu VA boss Phong Lang Do - ca ba deu chet trong mot nhat.",
        ],
        muc=[
            ("TIME_LIFE_BOSS", "BHK_PHUT_BOSS_TON_TAI",
             "boss song bao lau (phut) roi tu bien mat neu khong ai giet"),
            ("LIFE_BOSS_TIEU", "BHK_MAU_BOSS_TIEU",
             "mau cua Boss Tieu Hoang Kim (ghi chu trong ma: mac dinh cu"
             " 8.000.000)"),
            ("LIFE_BOSS_DAIH", "BHK_MAU_BOSS_DAI",
             "mau cua boss dai + boss may chu + boss PLD."
             " !! dang la 1, ghi chu noi mac dinh 18.000.000"),
            ("NETRANHBOSS", "BHK_NETRANH_BOSS",
             "chi so ne tranh cua boss - cang cao nguoi choi cang hay danh truot"),
            ("PHSLBOSS", "BHK_HOIMAU_BOSS",
             "luong mau boss tu hoi moi nhip"),
            ("BOSSDAI_EXP", "BHK_EXP_NPC_BOSS_DAI",
             "exp gan thang vao con NPC boss dai (dang 0 - exp thuong phat o"
             " tep death*)"),
            ("BOSSTIEU_EXP", "BHK_EXP_NPC_BOSS_TIEU",
             "exp gan thang vao con NPC boss tieu (dang 0)"),
        ],
    ),
    dict(
        tep=os.path.join(S, "tinhnang", "boss_hoangkim", "deathhktieu.lua"),
        cfg=P_THUONG, bang="tbCFG_THUONG", ten_ham="BHK_CFG",
        tieu_de=[NHAN + " BOSS HOANG KIM - thuong khi ha boss"],
        muc=[
            ("KILLBOSSEXPAWARD", "BHK_EXP_GIET_BOSS_TIEU",
             "exp cho nguoi ha Boss Tieu Hoang Kim va to doi cua ho"),
            ("KILLBOSSNEAREXPAWARD", "BHK_EXP_LANCAN_BOSS_TIEU",
             "exp cho nguoi dung gan cho danh boss tieu nhung khac to doi"),
            ("PHAMVI_HUONGEXP", "BHK_PHAMVI_HUONG_EXP_TIEU",
             "ban kinh quanh xac boss ma nguoi choi phai dung trong do moi duoc"
             " chia exp (boss tieu)"),
        ],
    ),
    dict(
        tep=os.path.join(S, "tinhnang", "boss_hoangkim", "deathhkdai.lua"),
        cfg=P_THUONG, bang="tbCFG_THUONG", ten_ham="BHK_CFG",
        tieu_de=None,
        muc=[
            ("KILLBOSSEXPAWARD", "BHK_EXP_GIET_BOSS_DAI",
             "exp cho nguoi ha Boss Dai Hoang Kim va to doi cua ho"),
            ("KILLBOSSNEAREXPAWARD", "BHK_EXP_LANCAN_BOSS_DAI",
             "exp cho nguoi dung gan cho ha boss dai nhung khac to doi"),
            ("PHAMVI_HUONGEXP", "BHK_PHAMVI_HUONG_EXP_DAI",
             "ban kinh chia exp quanh xac boss dai"),
        ],
    ),
    dict(
        tep=os.path.join(S, "tinhnang", "boss_hoangkim", "deathbosspld.lua"),
        cfg=P_THUONG, bang="tbCFG_THUONG", ten_ham="BHK_CFG",
        tieu_de=None,
        muc=[
            ("KILLBOSSEXPAWARD", "BHK_EXP_GIET_BOSS_PLD",
             "exp cho nguoi ha Boss Hoang Kim ban do Phong Lang Do"),
            ("KILLBOSSNEAREXPAWARD", "BHK_EXP_LANCAN_BOSS_PLD",
             "exp cho nguoi dung gan cho ha boss Phong Lang Do"),
            ("PHAMVI_HUONGEXP", "BHK_PHAMVI_HUONG_EXP_PLD",
             "ban kinh chia exp quanh xac boss Phong Lang Do"),
        ],
    ),
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t29_cauhinh_boss_hoangkim - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    for d in DOT:
        print("--- %s ---" % os.path.basename(d["tep"]))
        kq = nc.noi(tep=d["tep"], tep_cfg=d["cfg"], bang_cfg=d["bang"],
                    ten_ham=d["ten_ham"], nhan=NHAN, muc=d["muc"],
                    tieu_de=d.get("tieu_de"))
        if not nc.in_ket(kq):
            return 1
        if ghi and not nc.ghi(kq, hau_to=".truoc_cfgbhk"):
            return 1
    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
