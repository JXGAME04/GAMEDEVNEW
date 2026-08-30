# -*- coding: utf-8 -*-
"""t24_cauhinh_heso_nen.py - noi day HE SO NEN TOAN CUC (script\\lib\\lib_server.lua).

Tep nay chi 14 dong nhung la mot trong nhung tep quan trong nhat: no khai cac
he so nhan vao rat nhieu noi khac.

DANG CHAY THAT (5):
  SERVER_OPEN      moc gio mo cua, 7 cho trong game doc de chan nguoi choi
  SERVER_TEST      co thu nghiem - DANG MO, xem BAOCAO_LOHONG_2908.md muc 1
  MONEY_RATE       he so tien thuong (hien con an vao thuong nhiem vu Da Tau)
  EXP_RATE         he so kinh nghiem toan server, dang x20 - day la NUT CHINH
                   de dieu chinh toc do len cap
  STRONGBOSS_NSTK  he so mau + sat thuong Nguyen Soai Tong Kim

DANG TAT nhung MA VAN CON (6): DAMAGE_UPPER_BOSS, DAMAGE_UP_TTPLD,
  DAMAGE_UPPER_TTDL, STRONGBOSS_ST, STRONGBOSS_VA, MAX_MAGIC_LEVEL.
  Van dua ra cau hinh (ghi ro "dang tat") vi chung song lai ngay khi chu bat
  cong tac BAT_* tuong ung.

⚠️ SERVER_OPEN BI TRUNG TEN: script\\lib\\worldlibrary.lua:4 cung khai
   SERVER_OPEN = 2104011900 (19h00 ngay 01/04/2021). Ban do van chay nhung
   khong bao gio thang vi moi cho doc deu nap lib_server truoc. Chi ghi chu,
   KHONG dong toi - de tranh gay tep khac.

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
P = os.path.join(S, "lib", "lib_server.lua")
P_CFG = os.path.join(S, "cauhinh", "ch_chung.lua")
NHAN = "[CFGNEN 30/08]"

MUC = [
    ("SERVER_OPEN", "GLB_GIO_MO_SERVER",
     "moc gio mo cua, dang yymmddHHMM. 7 cho trong game doc de chan nguoi choi"),
    ("SERVER_TEST", "GLB_CHE_DO_TEST",
     "1 = MO che do thu nghiem. !! Dang mo: hai NPC 'Ho Tro Test' o lang tan"
     " thu phat do GM cho MOI nguoi choi. Dat 0 de tat"),
    ("MONEY_RATE", "GLB_TILE_TIEN",
     "he so nhan tien thuong toan server (1 = giu nguyen)"),
    ("EXP_RATE", "GLB_TILE_EXP",
     "he so nhan kinh nghiem toan server. NUT CHINH de dieu chinh toc do len cap"),
    ("DAMAGE_UPPER_BOSS", "GLB_SATTHUONG_BOSS_HK",
     "he so sat thuong Boss Hoang Kim tieu. (dang tat cung hoat dong)"),
    ("DAMAGE_UP_TTPLD", "GLB_SATTHUONG_QUAI_PLD",
     "he so sat thuong quai Thuy Tac tren thuyen Phong Lang Do ban Viet cu."
     " (dang tat)"),
    ("DAMAGE_UPPER_TTDL", "GLB_SATTHUONG_BOSS_TTDL",
     "he so sat thuong boss Thuy Tac Dau Linh ban Viet cu. (dang tat)"),
    ("STRONGBOSS_ST", "GLB_MANH_BOSS_SATTHU",
     "he so mau + sat thuong 20 boss Sat Thu ban Viet cu. (dang tat)"),
    ("STRONGBOSS_VA", "GLB_MANH_BOSS_VUOTAI",
     "he so mau + sat thuong quai va boss Vuot Ai ban Viet cu. (dang tat)"),
    ("STRONGBOSS_NSTK", "GLB_MANH_NGUYENSOAI_TK",
     "he so mau + sat thuong Nguyen Soai hai phe trong Tong Kim - DANG CHAY"),
    ("MAX_MAGIC_LEVEL", "GLB_MAX_DONG_THUOCTINH",
     "tran so dong thuoc tinh do roi tu quai. !! khong tep script nao doc so"
     " nay - so that dang chan nam trong ma C++"),
]

TIEU_DE = [
    NHAN + " HE SO NEN TOAN CUC (script\\lib\\lib_server.lua)",
    "",
    "Day la tep 14 dong khai cac he so nhan vao rat nhieu noi khac.",
    "Moi so DUNG BANG gia tri dang chay. Sua xong KHOI DONG LAI GameServer.",
    "",
    "!! GLB_CHE_DO_TEST dang la 1: hai NPC 'Ho Tro Test' o lang tan thu"
    " (ban do 53)",
    "!! mo menu GM day du cho MOI nguoi choi - 1 ty luong + 100.000 Xu + len",
    "!! thang cap 200, khong kiem tai khoan GM, khong gioi han so lan.",
    "!! Dat 0 de doi ve NPC tan thu that. Xem BAOCAO_LOHONG_2908.md muc 1.",
    "",
    "!! GLB_GIO_MO_SERVER bi TRUNG TEN voi mot ban trong"
    " script\\lib\\worldlibrary.lua:4",
    "!! (gia tri cu 2104011900). Ban do van chay nhung khong bao gio thang.",
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t24_cauhinh_heso_nen - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    kq = nc.noi(tep=P, tep_cfg=P_CFG, bang_cfg="tbCFG_CHUNG",
                ten_ham="GLB_CFG", nhan=NHAN, muc=MUC, tieu_de=TIEU_DE)
    if not nc.in_ket(kq):
        return 1
    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    return 0 if nc.ghi(kq, hau_to=".truoc_cfgnen") else 1


if __name__ == "__main__":
    sys.exit(main())
