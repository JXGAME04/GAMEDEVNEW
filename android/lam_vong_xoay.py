# -*- coding: utf-8 -*-
r"""[ANDROID 10/09 LUAN] Vong xoay tren o phu dang giu VONG SANG (aura) - ban tham khao xoay sprite 360 do/giay
(KgameWorld.cpp: CCRotateBy(1.0f, 360) tren "skill_selected.png"). Bo ve cua ta khong xoay duoc anh, nen
lam san 16 khung (moi khung quay 22,5 do) tu effect_skill.spr cua VNKU -> spr\ui3\uiskillcontrol\vong_xoay.spr.
Client ve khung (GetTickCount()/62) % 16 = ~1 vong/giay.
Dung: python android\lam_vong_xoay.py
"""
import os
import sys

from PIL import Image

GOC = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, GOC)
from bo_cuc_vnku_mobile import doc_spr, ghi_spr_nhieu_khung  # noqa: E402

NGUON = os.path.join(GOC, "du_lieu_ghi_de", "spr", "ui3", "uiskillcontrol", "effect_skill.spr")
SO_KHUNG = 16
W, H, ks, _ = doc_spr(NGUON)
goc = ks[0].convert("RGBA")
khung = [goc.rotate(-360.0 * k / SO_KHUNG, resample=Image.BICUBIC) for k in range(SO_KHUNG)]   # am = xoay thuan chieu kim
tmp = os.path.join(GOC, "_vong_xoay_tmp.spr")
print(ghi_spr_nhieu_khung(khung, tmp))
du_lieu = open(tmp, "rb").read()
os.remove(tmp)
for goc_ghi in [os.path.join(GOC, "du_lieu_ghi_de"), r"D:\jx1_android_data"]:
    p = os.path.join(goc_ghi, "spr", "ui3", "uiskillcontrol", "vong_xoay.spr")
    if os.path.isdir(os.path.dirname(p)):
        open(p, "wb").write(du_lieu)
        print("  ghi", p)
