# -*- coding: utf-8 -*-
"""t25_cauhinh_tongkim_thuong.py - noi day DIEM + THUONG + DIEU KIEN Tong Kim.

Bo sung cho t22 (t22 da lam phan THOI GIAN va LICH). Dot nay lam phan diem,
thuong va dieu kien tham gia - 17 hang so o ba tep.

DANG CHU Y (bo khao sat neu, deu ghi vao chu thich cua tung khoa):
 - NEEDMONEY = 20000 (phi bao danh) HIEN CHI HIEN CHU, khong he tru tien: moi
   lenh Pay deu da bi comment. Van dua ra cau hinh de sau nay co dung lai thi
   chinh mot cho.
 - LIFE_NGUYEN_SOAI duoc nhan them voi STRONGBOSS_NSTK (da noi day o dot truoc
   voi ten GLB_MANH_NGUYENSOAI_TK), nen doi mot trong hai la doi ca hai phe.
 - KILL_PLAYER_POINT = 30 la HANG CHET (khong noi nao doc) - so 30 that su duoc
   go cung trong kimtu.lua va tongtu.lua. KHONG dua vao cau hinh de khoi tao
   cam giac sai la chinh no se an.

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
P_CFG = os.path.join(S, "cauhinh", "ch_thuong.lua")
NHAN = "[CFGTKT 30/08]"

DOT = [
    dict(
        tep=os.path.join(S, "tinhnang", "tong_kim_tcap", "lib_tktc.lua"),
        ten_ham="TK_CFG",          # tep nay DA CO TK_CFG tu dot t22
        muc=[
            ("LEVEL_ENOUGH_TK", "TKT_CAP_TOI_THIEU",
             "cap toi thieu moi duoc bao danh"),
            ("MAX_PLAYER_CL", "TKT_LECH_QUANSO_TOIDA",
             "chenh lech quan so toi da giua hai phe; qua muc nay phe dong hon"
             " bi chan bao danh"),
            ("NEEDMONEY", "TKT_PHI_BAO_DANH",
             "phi bao danh. !! hien CHI HIEN CHU, khong tru tien - moi lenh"
             " Pay da bi comment"),
            ("TICH_LUY_THUONG_TK_W", "TKT_THUONG_TICHLUY_THANG",
             "diem tich luy cong them cho phe THANG, nhan voi cap quan ham 1-6"),
            ("TICH_LUY_THUONG_TK_L", "TKT_THUONG_TICHLUY_THUA",
             "diem tich luy cong them cho phe THUA, nhan voi cap quan ham"),
            ("TICH_LUY_THUONG_TK_H", "TKT_THUONG_TICHLUY_HOA",
             "diem tich luy cong them khi tran HOA, nhan voi cap quan ham"),
            ("MIN_POINT_AWARD", "TKT_DIEM_TOITHIEU_NHAN_THUONG",
             "duoi nguong diem nay thi het tran KHONG duoc goi qua"),
            ("MAX_CAMCO", "TKT_SO_CO_TOIDA",
             "moi phe chi duoc cam toi da bay nhieu la co trong mot tran"),
            ("FLAG_POINT", "TKT_DIEM_CAM_CO",
             "diem moi lan mang co ve cam thanh cong (cong ca ca nhan lan phe)"),
            ("RUN_SPEDD_DOW_ONFLAG", "TKT_MUC_GIAM_TOCDO_GIU_CO",
             "muc giam toc do chay cua nguoi dang vac co"),
            ("TIME_IN_TRAI", "TKT_GIAY_TRONG_DOANHTRAI",
             "so GIAY toi da duoc dung trong doanh trai truoc khi bi day ra"),
            ("TIME_DELAY_RA_TRAI", "TKT_GIAY_DELAY_RA_TRAI",
             "so GIAY con lai duoc giu khi buoc ra khoi trai"),
            ("LIFE_NGUYEN_SOAI", "TKT_MAU_NGUYENSOAI",
             "mau cua Nguyen Soai hai phe. !! con duoc nhan them voi"
             " GLB_MANH_NGUYENSOAI_TK"),
            ("TICHLUY_GIETSOAI", "TKT_DIEM_GIET_NGUYENSOAI",
             "diem cho nguoi ha guc Nguyen Soai (nguoi danh don cuoi an tron)"),
        ],
    ),
    dict(
        tep=os.path.join(S, "tinhnang", "tong_kim_tcap", "quanquan.lua"),
        ten_ham="TKT_CFG",
        muc=[
            ("NEED_MONEY", "TKT_GIA_DOI_NHACVUONGKIEM",
             "so tien can de doi Nhac Vuong Kiem o NPC quan quan"),
            ("NVHT_TO_NVK", "TKT_SL_HONTHACH_DOI_KIEM",
             "so Nhac Vuong Hon Thach can de doi mot Nhac Vuong Kiem"),
        ],
    ),
    dict(
        tep=os.path.join(S, "tinhnang", "tong_kim_tcap", "trinhsat.lua"),
        ten_ham="TKT_CFG",
        muc=[
            ("MONEY_TS", "TKT_PHI_TRINHSAT",
             "phi moi lan dung chuc nang trinh sat"),
        ],
    ),
]

TIEU_DE = [
    NHAN + " TONG KIM - DIEM, THUONG va DIEU KIEN THAM GIA",
    "(phan THOI GIAN va LICH nam o ch_lich.lua, tien to TK_)",
    "",
    "Moi so DUNG BANG gia tri dang chay. Sua xong KHOI DONG LAI GameServer.",
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t25_cauhinh_tongkim_thuong - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    ds_kq = []
    for i, d in enumerate(DOT):
        print("--- %s ---" % os.path.basename(d["tep"]))
        kq = nc.noi(tep=d["tep"], tep_cfg=P_CFG, bang_cfg="tbCFG_THUONG",
                    ten_ham=d["ten_ham"], nhan=NHAN, muc=d["muc"],
                    tieu_de=TIEU_DE if i == 0 else None)
        if not nc.in_ket(kq):
            return 1
        ds_kq.append(kq)
        # cac dot sau doc lai tep cau hinh vua duoc sua trong bo nho:
        # ghi tung dot mot de tranh ghi de lan nhau
        if ghi and not nc.ghi(kq, hau_to=".truoc_cfgtkt"):
            return 1
    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
