# -*- coding: utf-8 -*-
"""t26_cauhinh_congthanh.py - noi day CONG THANH CHIEN + LOI DAI JX2.

Hai hoat dong nay DANG CHAY THAT (cong tac BAT_CTC_JX2 = 1).

⚠️ LOI THAT PHAT HIEN KHI DOC (da tu kiem chung tan noi goi):
   MAX_CAMP1COUNT..4 duoc khai o HAI NOI voi HAI GIA TRI KHAC NHAU:
       missions\\citywar_city\\head.lua:65-68  = 200 / 200 / 50 / 50
       tinhnang\\congthanhchien\\lib_ctc.lua:90-93 =  50 /  50 /  5 /  5
   Va HAI NOI KIEM cung mot gioi han lai nap hai ban khac nhau:
       missions\\citywar_city\\camper.lua:102-119  KHONG Include gi
           => chay trong vung cua mission head.lua => dung ban 200/200/50/50
       tinhnang\\congthanhchien\\vebinhquandoanh.lua:35-54 Include lib_ctc
           => dung ban 50/50/5/5
   Nghia la NPC ve binh quan doanh chan o 50 nguoi moi phe, trong khi cua tran
   cho toi 200. Nguoi thu 51 bi NPC tu choi du tran con cho.

   TOI KHONG TU GOP HAI SO LAM MOT - gop la doi can bang. Ban va nay tach
   chung thanh HAI KHOA rieng, giu nguyen ca hai gia tri, va ghi ro trong chu
   thich de chu game quyet.

Tuong tu CITYWAR_TOP10_EXP cung co hai ban (cung gia tri 3.000.000) - tach hai
khoa de sua mot ban khong lam nghi la da sua ca hai.

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
P_CHUNG = os.path.join(S, "cauhinh", "ch_chung.lua")
P_THUONG = os.path.join(S, "cauhinh", "ch_thuong.lua")
NHAN = "[CFGCTC 30/08]"

DOT = [
    dict(
        tep=os.path.join(S, "missions", "citywar_city", "head.lua"),
        cfg=P_CHUNG, bang="tbCFG_CHUNG", ten_ham="CTC_CFG",
        tieu_de=[
            NHAN + " CONG THANH CHIEN - tran dia"
            " (missions\\citywar_city\\head.lua)",
            "",
            "!! MAX_CAMP* o day la ban ma CUA TRAN dung (camper.lua).",
            "!! Con NPC ve binh quan doanh dung ban KHAC o lib_ctc.lua -"
            " xem cac khoa",
            "!! CTC_MAX_*_VEBINH ben duoi. Hai ban dang LECH NHAU.",
        ],
        muc=[
            ("REPORTTIME", "CTC_CHUKY_LOA_GIAY",
             "chu ky loa trong tran: bao nhieu giay bao mot lan tinh hinh"),
            ("GAMETIME", "CTC_DODAI_TRAN_PHUT",
             "do dai mot tran Cong Thanh (phut)"),
            ("CITYWAR_TOP10_EXP", "CTC_EXP_TOP10",
             "kinh nghiem thuong cho 10 nguoi dan dau bang cong trang."
             " !! co mot ban nua o lib_ctc.lua"),
            ("MS_SYMBOLCOUNT", "CTC_SO_LONGTRU",
             "so Long Tru trong tran. Pha du ca ba la thang ngay"),
            ("g_nDoorCount", "CTC_SO_CONG_THANH",
             "so cong thanh phai pha. !! phai khop so dong trong bang DoorPos"),
            ("MAX_CAMP1COUNT", "CTC_MAX_NGUOI_PHE_THU",
             "so nguoi toi da phe THU vao tran (ban CUA TRAN dung)."
             " !! NPC ve binh dung so khac"),
            ("MAX_CAMP2COUNT", "CTC_MAX_NGUOI_PHE_CONG",
             "so nguoi toi da phe CONG vao tran (ban CUA TRAN dung)"),
            ("MAX_CAMP3COUNT", "CTC_MAX_LIENMINH_THU",
             "so nguoi ngoai bang tro giup phe THU (ban CUA TRAN dung)"),
            ("MAX_CAMP4COUNT", "CTC_MAX_LIENMINH_CONG",
             "so nguoi ngoai bang tro giup phe CONG (ban CUA TRAN dung)"),
            ("STONELEVEL1", "CTC_CAP_LONGTRU",
             "cap do NPC Long Tru khi sinh ra (anh huong mau)"),
            ("DOORLEVEL", "CTC_CAP_CONG_THANH",
             "cap do NPC cong thanh - quyet dinh cong kho pha den dau"),
            ("BONUS_KILLPLAYER", "CTC_DIEM_GIET_NGUOI",
             "diem cong trang co ban moi lan ha guc doi phuong"),
            ("BONUS_MAXSERIESKILL", "CTC_DIEM_LIEN_TRAM",
             "diem thuong moi lan dat moc lien tram"),
        ],
    ),
    dict(
        tep=os.path.join(S, "tinhnang", "congthanhchien", "lib_ctc.lua"),
        cfg=P_CHUNG, bang="tbCFG_CHUNG", ten_ham="CTC_CFG",
        tieu_de=[
            NHAN + " CONG THANH - ban NPC ve binh quan doanh dung"
            " (lib_ctc.lua)",
            "",
            "!! Bon so nay DANG LECH voi ban cua tran o tren:"
            " 50/50/5/5 so voi 200/200/50/50.",
            "!! Nguoi thu 51 bi NPC ve binh tu choi du tran con cho.",
            "!! Muon thong nhat thi dat bang nhau - do la quyet dinh can bang.",
        ],
        muc=[
            ("MAX_CAMP1COUNT", "CTC_MAX_NGUOI_PHE_THU_VEBINH",
             "so nguoi toi da phe THU - ban NPC VE BINH dung"),
            ("MAX_CAMP2COUNT", "CTC_MAX_NGUOI_PHE_CONG_VEBINH",
             "so nguoi toi da phe CONG - ban NPC VE BINH dung"),
            ("MAX_CAMP3COUNT", "CTC_MAX_LIENMINH_THU_VEBINH",
             "lien minh phe THU - ban NPC VE BINH dung"),
            ("MAX_CAMP4COUNT", "CTC_MAX_LIENMINH_CONG_VEBINH",
             "lien minh phe CONG - ban NPC VE BINH dung"),
            ("CITYWAR_TOP10_EXP", "CTC_EXP_TOP10_BANSAO",
             "ban sao thu hai cua CTC_EXP_TOP10 - sua mot ban khong doi ban kia"),
            ("KILL_TRU_POINT", "CTC_DIEM_PHA_LONGTRU",
             "diem tich luy khi pha duoc mot Long Tru"),
        ],
    ),
    # LUU Y: MOT TEP chi duoc xu ly MOT LAN trong ca dot. Ban dau toi tach
    # arena/head.lua thanh hai muc (thoi gian -> ch_lich, thuong -> ch_thuong),
    # nhung lan thu hai se thay nhan da co trong tep va BO QUA, lam mat khoa
    # thuong. Gop lai mot muc, do het vao ch_lich va ghi chu ro.
    dict(
        tep=os.path.join(S, "missions", "citywar_arena", "head.lua"),
        cfg=P_LICH, bang="tbCFG_LICH", ten_ham="CTLD_CFG",
        tieu_de=[
            NHAN + " LOI DAI DAU TRUONG (missions\\citywar_arena\\head.lua)",
            "(khoa CTLD_EXP_BANG_THANG la phan THUONG, de chung o day cho",
            " cung mot cho voi cac khoa khac cua Loi Dai)",
        ],
        muc=[
            ("MAX_MEMBER_COUNT", "CTLD_MAX_NGUOI_MOI_PHE",
             "so nguoi toi da moi bang vao dau truong Loi Dai"),
            ("TIMER_1", "CTLD_CHUKY_LOA_GIAY",
             "chu ky loa trong Loi Dai (giay)"),
            ("TIMER_2", "CTLD_DODAI_TRAN_PHUT",
             "do dai mot tran Loi Dai (phut). !! chu thich trong ma ghi nham"
             " la mot tieng"),
            ("GO_TIME", "CTLD_GIAY_CHO_VAO_DAUTRUONG",
             "thoi gian cho hai ben vao dau truong (giay). !! chu thich ghi"
             " nham la 10 phut"),
            ("WIN_TONGEXP", "CTLD_EXP_BANG_THANG",
             "kinh nghiem bang hoi cong cho ben thang mot tran Loi Dai"),
        ],
    ),
    dict(
        tep=os.path.join(S, "missions", "citywar_global", "head.lua"),
        cfg=P_CHUNG, bang="tbCFG_CHUNG", ten_ham="CTC_CFG",
        tieu_de=[NHAN + " CONG THANH - lenh bai tro chien"],
        muc=[
            ("CardPrice", "CTC_GIA_LENHBAI",
             "gia mot Thanh Chien Lenh Bai (luong) - chi bang chu mua duoc"),
            ("ReturnCardPrice", "CTC_GIA_TRA_LAI_LENHBAI",
             "so luong hoan lai khi tra lai lenh bai chua dung"),
        ],
    ),
    dict(
        tep=os.path.join(S, "missions", "citywar_global", "mission.lua"),
        cfg=P_LICH, bang="tbCFG_LICH", ten_ham="CTC_CFG",
        tieu_de=[NHAN + " CONG THANH - nhip quet mo tran"],
        muc=[
            ("INTERVAL", "CTC_CHUKY_QUET_MO_TRAN_PHUT",
             "cu bao nhieu phut di kiem xem co thanh nao dang khai chien."
             " !! do tre toi da bang chinh so nay"),
        ],
    ),
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t26_cauhinh_congthanh - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    for d in DOT:
        print("--- %s -> %s ---" % (os.path.basename(d["tep"]),
                                    os.path.basename(d["cfg"])))
        kq = nc.noi(tep=d["tep"], tep_cfg=d["cfg"], bang_cfg=d["bang"],
                    ten_ham=d["ten_ham"], nhan=NHAN, muc=d["muc"],
                    tieu_de=d.get("tieu_de"))
        if not nc.in_ket(kq):
            return 1
        if ghi and not nc.ghi(kq, hau_to=".truoc_cfgctc"):
            return 1
    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
