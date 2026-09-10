# -*- coding: utf-8 -*-
r"""[ANDROID 11/09 WAUTO B1] Anh cho KHUNG WAuto trong game, lay tu kho VNKU (CHI DOC) va thu nho cho khung ve dien thoai.

Sinh ra (ghi vao lop ghi de android\du_lieu_ghi_de\ va D:\jx1_android_data neu co, qua ghi_moi_noi cua bo_cuc_vnku_mobile.py):
  spr\uinew\uiautonew\khung_wauto.spr           khung "TU DONG" 1313x788 -> 720x432 (~70% be ngang khung ve 1040x604, de lo nhan vat xung quanh)
  spr\uinew\uiautonew\bat_tat_auto.spr          4 khung, cao 40: [0] "Bat Auto" vang  [1] "Bat Auto" xam  [2] "Tat Auto" vang  [3] "Tat Auto" xam
                                                 (KWndButton CheckBox: Up=0 khi auto dang tat, Down=2 khi dang bat)
  spr\uinew\uitoolscontrolbar\auto_m.spr        icon Auto tren thanh cong cu 47x47, 2 khung: [0] kiem cheo XAM (tat) [1] VANG (bat)
                                                 - cat tu hinh tron ben trai cua bat_auto.spr (nut "nut_de_auto" cua VNKU la chu "Giu", khong hop)
Nguon: Spr\UiNew\UiAutoNew\khung.spr, Spr\UiNew\MinMapSmall\bat_auto.spr + tat_auto.spr.
Dung: python android\anh_wauto_vnku.py
"""
import io
import os
import sys

from PIL import Image

GOC = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, GOC)
sys.path.insert(0, os.path.join(GOC, "..", "ReverseTools", "tongkim_chat"))
from bo_cuc_vnku_mobile import doc_spr, ghi_spr_nhieu_khung, ghi_moi_noi, VNKU  # noqa: E402

KHUNG_W, KHUNG_H = 720, 432		# (chu 11/09: "qua to, phai nho gon lai" - truoc 980x588 che gan het man 1040x604)
ICON = 47
BATTAT_CAO = 40


def ghi(rel, cac_khung):
    tmp = os.path.join(os.environ.get("TEMP", GOC), "wauto_tmp.spr")
    msg = ghi_spr_nhieu_khung(cac_khung, tmp)
    du_lieu = io.open(tmp, "rb").read()
    print("%-45s %dx%d x%d  %s" % (rel, cac_khung[0].size[0], cac_khung[0].size[1], len(cac_khung), msg))
    ghi_moi_noi(rel, du_lieu)


def khung():
    W, H, ks, _ = doc_spr(os.path.join(VNKU, "UiAutoNew", "khung.spr"))
    ghi(os.path.join("spr", "uinew", "uiautonew", "khung_wauto.spr"), [ks[0].resize((KHUNG_W, KHUNG_H), Image.LANCZOS)])


def bat_tat():
    _, _, bat, _ = doc_spr(os.path.join(VNKU, "MinMapSmall", "bat_auto.spr"))
    _, _, tat, _ = doc_spr(os.path.join(VNKU, "MinMapSmall", "tat_auto.spr"))
    w, h = bat[0].size
    k = BATTAT_CAO / float(h)
    co = (int(round(w * k)), BATTAT_CAO)
    ks = [bat[0], bat[1], tat[0], tat[1]]
    ghi(os.path.join("spr", "uinew", "uiautonew", "bat_tat_auto.spr"), [x.resize(co, Image.LANCZOS) for x in ks])
    return bat


def icon(bat):
    # hinh tron kiem cheo nam ben trai nut "Bat Auto": lay hop bao cua phan co alpha trong 115 px dau
    out = []
    for f in (bat[1], bat[0]):          # [0] xam = tat, [1] vang = bat
        trai = f.crop((0, 0, 115, f.size[1]))
        bb = trai.getchannel("A").point(lambda a: 255 if a > 40 else 0).getbbox()
        # hinh tron: lay canh = chieu CAO cua hop bao (be ngang hop bao con dinh mot dai nen cua than nut ben phai)
        canh = bb[3] - bb[1]
        vung = trai.crop((bb[0], bb[1], bb[0] + canh, bb[3]))
        # ep vuong (lay canh lon), can giua tren nen trong suot
        c = max(vung.size)
        vuong = Image.new("RGBA", (c, c), (0, 0, 0, 0))
        vuong.alpha_composite(vung, ((c - vung.size[0]) // 2, (c - vung.size[1]) // 2))
        out.append(vuong.resize((ICON, ICON), Image.LANCZOS))
        print("  icon: hop bao", bb, "->", c, "x", c)
    ghi(os.path.join("spr", "uinew", "uitoolscontrolbar", "auto_m.spr"), out)


if __name__ == "__main__":
    khung()
    b = bat_tat()
    icon(b)
    print("xong")
