# -*- coding: utf-8 -*-
"""t30_cauhinh_bdh_datau.py - noi day BAN DONG HANH + DA TAU + hang ngay.

⚠️ GIOI HAN PHAI NOI RO VOI CHU GAME: rat nhieu con so quan trong cua hai he
   nay KHONG nam trong Lua ma nam trong bang settings, nen ban va nay khong
   cham toi duoc:
     Da Tau  : settings\\task\\award_basic.txt, award_link.txt, award_loop.txt,
               tasklink_*.txt, levellink.txt  (moc thuong theo so nhiem vu,
               gia tri goc de tinh tien/exp)
     Ban Dong Hanh: settings\\petsys\\levelup.txt (cot nItemCostCount,
               nSuccessRate = ti le thanh cong nang cap va so thuoc can),
               settings\\petsys\\feature.txt
   Muon chinh tron ven thi phai dua ca cac tep .txt do vao he cau hinh - do la
   viec rieng, khong lam trong dot nay.

KHONG NOI (deu la BANG, khong phai mot so):
   petsys\\levelup.lua  tbLevelUpAttr, attrInitValue
   task\\partner\\task_award.lua  aryMasterAward

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
NHAN = "[CFGBDH 30/08]"

DOT = [
    dict(
        tep=os.path.join(S, "petsys", "common.lua"),
        cfg=P_CHUNG, bang="tbCFG_CHUNG", ten_ham="BDH_CFG",
        tieu_de=[
            NHAN + " BAN DONG HANH - gioi han va gia ca (petsys\\common.lua)",
            "",
            "!! Ti le thanh cong nang cap va so thuoc can KHONG o day - chung",
            "!! nam trong settings\\petsys\\levelup.txt (cot nSuccessRate,",
            "!! nItemCostCount). Sua o day khong cham toi chung.",
        ],
        muc=[
            ("MAX_FRUIT_COUNT_DAILY", "BDH_SO_LAN_CHO_AN_MOI_NGAY",
             "so lan moi ngay duoc cho an MOI LOAI qua (dem rieng tung loai)"),
            ("CHANGE_FEATURE_COIN", "BDH_GIA_DOI_NGOAI_QUAN",
             "so Xu phai tra de doi ngoai quan"),
            ("CHANGE_NAME_COIN", "BDH_GIA_DOI_TEN",
             "so Xu phai tra de doi ten"),
            ("MAX_LEVEL", "BDH_CAP_TOI_DA",
             "cap toi da cua Ban Dong Hanh."
             " !! ten bien nay rat chung - chi doi trong tep petsys\\common.lua"),
            ("PET_LEVEL_STEP", "BDH_BUOC_CAP_NGOAI_QUAN",
             "cu moi bao nhieu cap thi mo them mot nhom ngoai quan"),
            ("MIN_LEVEL", "BDH_CAP_NHANVAT_TOI_THIEU",
             "cap nhan vat toi thieu de tao duoc Ban Dong Hanh"),
            ("EXT_SKILL_OPEN_PET_LEVEL", "BDH_CAP_MO_KYNANG_BIKIP",
             "cap Ban Dong Hanh toi thieu de bat dau hoc ky nang tu Bi kip"),
            ("EXT_SKILL_MAX_COUNT", "BDH_SO_O_KYNANG_TOI_DA",
             "so o ky nang bi dong toi da"),
            ("EXT_SKILL_GET_NEW_LEVEL", "BDH_BUOC_CAP_MO_O_KYNANG",
             "cu moi bao nhieu cap thi mo them mot o ky nang"),
        ],
    ),
    dict(
        tep=os.path.join(S, "petsys", "xiuzhen.lua"),
        cfg=P_CHUNG, bang="tbCFG_CHUNG", ten_ham="BDH_CFG",
        tieu_de=None,
        muc=[
            ("ZHENYUAN_RATE", "BDH_CHANNGUYEN_DOI_1_TUCHAN",
             "so diem chan nguyen bo ra de doi lay 1 diem Tu Chan"),
            ("XIUZHEN_MAX", "BDH_TUCHAN_TOI_DA",
             "tran diem Tu Chan cua Ban Dong Hanh"),
        ],
    ),
    dict(
        tep=os.path.join(S, "petsys", "petequip.lua"),
        cfg=P_CHUNG, bang="tbCFG_CHUNG", ten_ham="BDH_CFG",
        tieu_de=None,
        muc=[
            ("PETEQUIP_O_SO", "BDH_SO_O_TRANGBI",
             "so o trang bi cua Ban Dong Hanh"),
        ],
    ),
    dict(
        tep=os.path.join(S, "task", "partner", "reward", "partner_reward.lua"),
        cfg=P_THUONG, bang="tbCFG_THUONG", ten_ham="BDH_CFG",
        tieu_de=[NHAN + " BAN DONG HANH - danh hieu"],
        muc=[
            ("TITLETIME", "BDH_THOIHAN_DANHHIEU_NGAY",
             "so NGAY danh hieu Ban Dong Hanh ton tai (ma viet dang"
             " 30*24*60*60*18*36 nen day la con so dau tien)."
             " !! khai lai y het o partner_reward2.lua"),
        ],
    ),
    dict(
        tep=os.path.join(S, "task", "newtask", "tasklink", "tasklink_head.lua"),
        cfg=P_CHUNG, bang="tbCFG_CHUNG", ten_ham="DT_CFG",
        tieu_de=[
            NHAN + " DA TAU - so vong va chu ky",
            "",
            "!! Moc thuong theo so nhiem vu va gia tri goc de tinh tien/exp",
            "!! KHONG o day - chung nam trong settings\\task\\award_basic.txt,",
            "!! award_link.txt, award_loop.txt, tasklink_*.txt.",
        ],
        muc=[
            ("TL_MAXTIMES", "DT_SO_NV_MOI_VONG",
             "so nhiem vu phai lam xong de qua mot vong moi"),
            ("TL_MAXLINKS", "DT_SO_VONG_MOI_CHU_KY",
             "so vong nhiem vu trong mot chu ky"),
            ("TL_MAXLOOPS", "DT_SO_CHU_KY_TOI_DA",
             "so chu ky toi da truoc khi dem quay ve 0"),
        ],
    ),
    # KHONG NOI huoyuedu\huoyuedu.lua:18 `STATE = OPENED`: gia tri la mot HANG
    # KHAC (OPENED) chu khong phai so, nen bo khung tu loai - dung. Muon dua ra
    # cau hinh thi phai nan chinh hang OPENED, la viec khac.
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t30_cauhinh_bdh_datau - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    for d in DOT:
        print("--- %s ---" % os.path.basename(d["tep"]))
        kq = nc.noi(tep=d["tep"], tep_cfg=d["cfg"], bang_cfg=d["bang"],
                    ten_ham=d["ten_ham"], nhan=NHAN, muc=d["muc"],
                    tieu_de=d.get("tieu_de"))
        if not nc.in_ket(kq):
            return 1
        if ghi and not nc.ghi(kq, hau_to=".truoc_cfgbdh"):
            return 1
    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
