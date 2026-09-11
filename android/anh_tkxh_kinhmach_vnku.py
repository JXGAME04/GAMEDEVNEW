# -*- coding: utf-8 -*-
r"""[TKXH 14/09] + [KINHMACH 14/09] Hai icon thanh cong cu cho dien thoai, lay tu kho VNKU (CHI DOC), thu nho cho khung ve.
Sinh ra (ghi vao lop ghi de android\du_lieu_ghi_de\ va D:\jx1_android_data neu co, qua ghi_moi_noi cua bo_cuc_vnku_mobile.py):
  spr\uinew\uitoolscontrolbar\bxh_tk_m.spr     2 khung 54x48 tu UiPlayerBar\bxh_tk.spr (187x165: thuong / sang) - "Xep hang Tong Kim"
  spr\uinew\uitoolscontrolbar\kinh_mach_m.spr  2 khung 48x48: huy hieu tron ben trai nut UiPlayerBar\luyencong.spr (210x51, 1 khung),
                                               khung 1 = ban lam sang (dang mo). Anh TAM cho "Kinh mach" - kho VNKU khong co icon kinh
                                               mach rieng; chu chi tep khac thi doi o day.
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


def kinh_mach():
    W, H, ks, _ = doc_spr(os.path.join(VNKU, "UiPlayerBar", "luyencong.spr"))
    k = rgba(ks[0])
    trai = k.crop((0, 0, H, H))                         # huy hieu tron nam trong o vuong ben trai (H = 51)
    bb = trai.getchannel("A").getbbox() or (0, 0, H, H)
    trai = trai.crop(bb)
    canh = max(trai.width, trai.height)
    vuong = Image.new("RGBA", (canh, canh), (0, 0, 0, 0))
    vuong.paste(trai, ((canh - trai.width) // 2, (canh - trai.height) // 2), trai)
    k0 = vuong.resize((48, 48), Image.LANCZOS)
    k1 = ImageEnhance.Brightness(k0).enhance(1.35)      # khung "dang mo": sang hon, cung kieu khung 2 cua bxh_tk
    ghi(os.path.join("spr", "uinew", "uitoolscontrolbar", "kinh_mach_m.spr"), [k0, k1])


if __name__ == "__main__":
    tong_kim()
    kinh_mach()
    print("xong")
