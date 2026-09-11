# -*- coding: utf-8 -*-
r"""[TKXH 14/09] + [KINHMACH 14/09] Hai icon thanh cong cu cho dien thoai, lay tu kho VNKU (CHI DOC), thu nho cho khung ve.
Sinh ra (ghi vao lop ghi de android\du_lieu_ghi_de\ va D:\jx1_android_data neu co, qua ghi_moi_noi cua bo_cuc_vnku_mobile.py):
  spr\uinew\uitoolscontrolbar\bxh_tk_m.spr     2 khung 54x48 tu UiPlayerBar\bxh_tk.spr (187x165: thuong / sang) - "Xep hang Tong Kim"
  spr\uinew\uitoolscontrolbar\kinh_mach_m.spr  2 khung 48x48 tu kho jx-thiendieu (CHI DOC) spr\Ui3\UiBatMachChanKinh\level:
                                               tangthu2.spr (vang) = thuong, tangthu1.spr (do) = dang mo (35x35 -> 48x48). Chu chi kho
                                               nay 14/09 sau khi kho VNKU khong co icon kinh mach; muon mau khac thi doi ten tep o KHO_TD.
Dung: python android\anh_tkxh_kinhmach_vnku.py
"""
import os
import sys

from PIL import Image, ImageEnhance

GOC = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, GOC)
from bo_cuc_vnku_mobile import doc_spr, ghi_spr_nhieu_khung, ghi_moi_noi, VNKU  # noqa: E402

TMP = os.path.join(GOC, "_tmp_tkxh.spr")


def ghi(rel, cac_khung):
    ghi_spr_nhieu_khung(cac_khung, TMP)
    du_lieu = open(TMP, "rb").read()
    ghi_moi_noi(rel, du_lieu)
    os.remove(TMP)
    print("  %s: %d khung %dx%d, %d byte" % (rel, len(cac_khung), cac_khung[0].width, cac_khung[0].height, len(du_lieu)))


def rgba(k):
    return k if k.mode == "RGBA" else k.convert("RGBA")


def tong_kim():
    W, H, ks, _ = doc_spr(os.path.join(VNKU, "UiPlayerBar", "bxh_tk.spr"))
    assert len(ks) >= 2, len(ks)
    co = (54, 48)                                       # 187x165 -> giu ti le ~1.13, cao 48 nhu nut nhat_m.spr
    ghi(os.path.join("spr", "uinew", "uitoolscontrolbar", "bxh_tk_m.spr"),
        [rgba(k).resize(co, Image.LANCZOS) for k in ks[:2]])


# [KINHMACH 14/09 b] Chu chi kho jx-thiendieu (CHI DOC): spr\Ui3\UiBatMachChanKinh\level\tangthu1..9.spr la 9 huy hieu
# phuong hoang tron 35x35 theo canh gioi kinh mach. Lay tangthu2 (vang) = khung thuong, tangthu1 (do) = khung "dang mo".
KHO_TD = r"E:\SourceTuanLe\SourceVs22\jx-thiendieu\SwordOnline\bin\client\spr\Ui3\UiBatMachChanKinh"


def kinh_mach():
    ks = []
    for ten in ("tangthu2.spr", "tangthu1.spr"):
        W, H, kh, _ = doc_spr(os.path.join(KHO_TD, "level", ten))
        ks.append(rgba(kh[0]).resize((48, 48), Image.LANCZOS))
    ghi(os.path.join("spr", "uinew", "uitoolscontrolbar", "kinh_mach_m.spr"), ks)


if __name__ == "__main__":
    tong_kim()
    kinh_mach()
    print("xong")
