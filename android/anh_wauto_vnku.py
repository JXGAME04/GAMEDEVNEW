# -*- coding: utf-8 -*-
r"""[ANDROID 11/09 WAUTO B1] Anh cho KHUNG WAuto trong game, lay tu kho VNKU (CHI DOC) va thu nho cho khung ve dien thoai.

Sinh ra (ghi vao lop ghi de android\du_lieu_ghi_de\ va D:\jx1_android_data neu co, qua ghi_moi_noi cua bo_cuc_vnku_mobile.py):
  spr\uinew\uiautonew\khung_wauto.spr           khung "TU DONG" 1313x788 -> 720x432 (~70% be ngang khung ve 1040x604, de lo nhan vat xung quanh)
  spr\uinew\uiautonew\bat_tat_auto.spr          4 khung, cao 40: [0] "Bat Auto" vang  [1] "Bat Auto" xam  [2] "Tat Auto" vang  [3] "Tat Auto" xam
                                                 (KWndButton CheckBox: Up=0 khi auto dang tat, Down=2 khi dang bat)
  spr\uinew\uitoolscontrolbar\auto_m.spr        icon Auto tren thanh cong cu 47x47, 2 khung: [0] kiem cheo XAM (tat) [1] VANG (bat)
                                                 - cat tu hinh tron ben trai cua bat_auto.spr (nut "nut_de_auto" cua VNKU la chu "Giu", khong hop)
  [B2 b] NEN DO (chu 11/09: "thay may nut nen thanh ve nen do dep hon") tu UiTong_Sheet0\btn_1.spr:
  spr\uinew\uiautonew\nut_nhom.spr 166x30, nut_tab.spr 84x22 (khung 0 thuong, 1 dang chon: do tuoi + vien vang)
  spr\uinew\uiautonew\nut_do_60/84/120/160.spr (W x 22) nut hanh dong; hop_chon_120/160/200.spr (W x 24, mui ten vang) hop chon
  spr\uinew\uiautonew\tick_chon.spr 24x24 tu UiAutoNew\tick_chon.spr (khung 0 trong, 1 da chon)
Nguon: Spr\UiNew\UiAutoNew\khung.spr, Spr\UiNew\MinMapSmall\bat_auto.spr + tat_auto.spr.
Dung: python android\anh_wauto_vnku.py
"""
import io
import os
import sys

from PIL import Image, ImageDraw

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


ICON_W, ICON_H = 48, 56         # [B2 h] icon Auto: hai kiem cheo (tren) + chu "Auto" (duoi) - chu: "icon auto co chu Auto 2 kiem cheo nhau"
NUT_TCB_W, NUT_TCB_H = 102, 40  # [B2 i] nut tren thanh cong cu lay NGUYEN nut kho VNKU (chu: "goc no co icon luon ko can ghep")


def nut_thanh_cong_cu(bat):
    """[B2 i] Ba nut Auto tren thanh cong cu, lay NGUYEN anh kho VNKU (khong ghep), cung co 102x40 nhu nut trong khung:
      spr\\uinew\\uitoolscontrolbar\\bat_auto_m.spr   4 khung tu MinMapSmall\\bat_auto.spr + tat_auto.spr (Bat vang, Bat xam, Tat vang, Tat xam)
                                                    -> KWndButton CheckBox Up=0 Down=2: bam = BAT/TAT auto ngay (Player_WAutoBat)
      spr\\uinew\\uitoolscontrolbar\\thiet_lap_m.spr 2 khung tu MinMapSmall\\thietlapauto.spr (vang / xam) -> mo khung thiet lap (Player_WAuto)
      spr\\uinew\\uitoolscontrolbar\\nhat_m.spr      2 khung 48x48 tu UiToolsControlBar\\nut_nhat.spr (ban tay trang / vang)
                                                    -> nhat ngay, uu tien hon danh (Player_NhatDo)"""
    _, _, tat, _ = doc_spr(os.path.join(VNKU, "MinMapSmall", "tat_auto.spr"))
    co = (NUT_TCB_W, NUT_TCB_H)
    ghi(os.path.join("spr", "uinew", "uitoolscontrolbar", "bat_auto_m.spr"), [x.resize(co, Image.LANCZOS) for x in (bat[0], bat[1], tat[0], tat[1])])
    _, _, tl, _ = doc_spr(os.path.join(VNKU, "MinMapSmall", "thietlapauto.spr"))
    ghi(os.path.join("spr", "uinew", "uitoolscontrolbar", "thiet_lap_m.spr"), [x.resize(co, Image.LANCZOS) for x in tl[:2]])
    _, _, nh, _ = doc_spr(os.path.join(VNKU, "UiToolsControlBar", "nut_nhat.spr"))
    ghi(os.path.join("spr", "uinew", "uitoolscontrolbar", "nhat_m.spr"), [x.resize((48, 48), Image.LANCZOS) for x in nh[:2]])


def icon(bat):
    """Icon Auto tren thanh cong cu ICON_W x ICON_H, 2 khung: [0] tat (xam), [1] bat (vang).
    [B2 h] Chu (11/09): "icon auto co chu Auto 2 kiem cheo nhau" -> ghep tu chinh nut "Bat Auto" cua kho (MinMapSmall\\bat_auto.spr
    285x112, khung 0 vang / khung 1 xam): hinh tron hai kiem cheo (ben trai nut) dat tren, chu "Auto" (cat tu chu "Bat Auto", cot
    185..258, hang 44..81) dat duoi. pk.spr (kiem cheo khong chu) da thu truoc do, chu muon co chu."""
    out = []
    for f in (bat[1], bat[0]):          # [0] xam = tat, [1] vang = bat
        trai = f.crop((0, 0, 115, f.size[1]))
        bb = trai.getchannel("A").point(lambda a: 255 if a > 40 else 0).getbbox()
        canh = bb[3] - bb[1]
        vung = trai.crop((bb[0], bb[1], bb[0] + canh, bb[3]))
        c = max(vung.size)
        vuong = Image.new("RGBA", (c, c), (0, 0, 0, 0))
        vuong.alpha_composite(vung, ((c - vung.size[0]) // 2, (c - vung.size[1]) // 2))
        kiem = vuong.resize((34, 34), Image.LANCZOS)
        chu = f.crop((182, 44, 262, 82)).resize((46, 22), Image.LANCZOS)
        a = Image.new("RGBA", (ICON_W, ICON_H), (0, 0, 0, 0))
        a.alpha_composite(kiem, ((ICON_W - 34) // 2, 0))
        a.alpha_composite(chu, ((ICON_W - 46) // 2, 33))
        out.append(a)
    ghi(os.path.join("spr", "uinew", "uitoolscontrolbar", "auto_m.spr"), out)


TICK = 24                       # [B2 b] o tick 24x24 (hang cao 24..28) - truoc 36 (hang 38: "chu phai hien thi day du")
CHON_RONG = (120, 160, 200)     # hop chon: 3 co, chon theo dong dai nhat
NUT_RONG = (60, 84, 120, 160)   # nut hanh dong: 4 co, chon theo chu
VANG = (214, 176, 96, 255)      # vien / mui ten vang (hop tick_chon.spr cua VNKU)
VANG_TOI = (120, 90, 45, 255)


TONG = {    # (thuong tren, thuong duoi, chon tren, chon duoi, canh tren thuong, canh tren chon, canh trai thuong, canh trai chon)
    "do":   (((92, 22, 26), (34, 6, 9)), ((150, 38, 40), (78, 12, 16)), (150, 70, 66), (200, 110, 100), (100, 44, 42), (140, 60, 58)),
    "xanh": (((26, 78, 96), (10, 36, 46)), ((48, 140, 160), (22, 88, 104)), (80, 150, 165), (130, 210, 220), (50, 110, 125), (90, 170, 185)),
}


def _nen_do(w, h, sang, tong="do"):
    """Nut w x h, ve co KHOI (chu 11/09: "thay may nut nen thanh ve nen do dep hon", "mau hien tai qua dam -> do den",
    "may nut tab chinh phu thi phai co nut", "cac nut tab chinh - phu phai mau xanh"): nen chuyen sac (sang tren, toi duoi) tren van
    cua UiTong_Sheet0\\btn_1.spr, vien ngoai den, canh tren sang (noi khoi), canh duoi toi. sang=1 (dang chon / dang bam): tuoi hon +
    vien vang. tong="do" (o noi dung: hop chon, nut hanh dong) / "xanh" (nut nhom + tab con, cung tong xanh ngoc cua vien khung)."""
    _, _, ks, _ = doc_spr(os.path.join(VNKU, "UiTong_Sheet0", "btn_1.spr"))
    van = ks[0].crop((2, 2, ks[0].size[0] - 2, ks[0].size[1] - 2)).resize((w, h), Image.LANCZOS).convert("L")
    T = TONG[tong]
    tren, duoi = T[1] if sang else T[0]
    a = Image.new("RGBA", (w, h), (0, 0, 0, 255))
    px = a.load()
    vp = van.load()
    for y in range(h):
        t = y / max(1, h - 1)
        for x in range(w):
            k = 0.85 + 0.3 * (vp[x, y] / 255.0)          # van nhe cua btn_1 (+-15 %)
            px[x, y] = (min(255, int((tren[0] + (duoi[0] - tren[0]) * t) * k)),
                        min(255, int((tren[1] + (duoi[1] - tren[1]) * t) * k)),
                        min(255, int((tren[2] + (duoi[2] - tren[2]) * t) * k)), 255)
    d = ImageDraw.Draw(a)
    d.rectangle((0, 0, w - 1, h - 1), outline=(6, 6, 6, 255))                             # vien ngoai den
    d.line((1, 1, w - 2, 1), fill=(T[3] if sang else T[2]) + (255,))                       # canh tren sang: noi khoi
    d.line((1, h - 2, w - 2, h - 2), fill=(duoi[0] // 2, duoi[1] // 2, duoi[2] // 2, 255))  # canh duoi toi
    d.line((1, 1, 1, h - 2), fill=(T[5] if sang else T[4]) + (255,))
    if sang:
        d.rectangle((1, 1, w - 2, h - 2), outline=VANG)                                   # dang chon: vien vang
    return a


def _mui_ten(a, w, h, sang):
    """mui ten xuong vang o dau phai cua hop chon + vach ngan"""
    d = ImageDraw.Draw(a)
    ox = w - 18
    d.line((ox, 2, ox, h - 3), fill=VANG if sang else VANG_TOI)
    cx, cy = ox + 9, h // 2
    d.polygon([(cx - 5, cy - 3), (cx + 5, cy - 3), (cx, cy + 3)], fill=VANG)
    return a


def _kho(rel):
    return doc_spr(os.path.join(VNKU, *rel.split("/")))[2]


def _nut_kho(khung, w, h, sang):
    """nut lay NGUYEN anh kho VNKU (chu 11/09: "cac nut co san o kho vnku ban quen roi a"), chi thu ve w x h; sang=1 them vien vang mong"""
    a = khung.resize((w, h), Image.LANCZOS)
    if sang:
        ImageDraw.Draw(a).rectangle((0, 0, w - 1, h - 1), outline=VANG)
    return a


def nut_do():
    """[B2 c] Nut cua khung WAuto, TAT CA tu anh co san trong kho VNKU (chu 11/09: "cac nut co san o kho vnku ban quen roi a",
    "tab chinh - phu phai mau xanh", "nen do lam mau do den"):
      nut nhom 166x30 + nut tab 84x22  <- UiTong_Sheet0\\btn_noname.spr (nut tron xanh ngoc: khung 1 toi = thuong, khung 0 sang = dang chon)
      nut hanh dong nut_do_W.spr (W x 22) <- UiTong_Sheet0\\btn_1.spr (do den, vien den) - khung 1 (dang bam) them vien vang
      hop chon hop_chon_W.spr (W x 24)   <- btn_1.spr + o mui ten vang cat tu UiAutoNew\\nut_pop.spr (cot 414..476, khung 0)
    Khung 0 = thuong, 1 = dang chon / bam."""
    goc = os.path.join("spr", "uinew", "uiautonew")
    kn = _kho("UiTong_Sheet0/btn_noname.spr")
    toi, sang = kn[1], kn[0]
    ghi(os.path.join(goc, "nut_nhom.spr"), [_nut_kho(toi, 166, 30, 0), _nut_kho(sang, 166, 30, 0)])
    ghi(os.path.join(goc, "nut_tab.spr"), [_nut_kho(toi, 84, 22, 0), _nut_kho(sang, 84, 22, 0)])
    b1 = _kho("UiTong_Sheet0/btn_1.spr")[0]
    for w in NUT_RONG:
        ghi(os.path.join(goc, "nut_do_%d.spr" % w), [_nut_kho(b1, w, 22, 0), _nut_kho(b1, w, 22, 1)])
    pop = _kho("UiAutoNew/nut_pop.spr")[0].crop((414, 0, 476, 62))
    for w in CHON_RONG:
        khung = []
        for s in (0, 1):
            a = _nut_kho(b1, w, 24, s)
            a.alpha_composite(pop.resize((22, 22), Image.LANCZOS), (w - 23, 1))
            khung.append(a)
        ghi(os.path.join(goc, "hop_chon_%d.spr" % w), khung)


def o_tick():
    """[B2] O tick tu tick_chon.spr VNKU (36x36: khung 0 trong, khung 1 da chon) thu ve TICK x TICK."""
    _, _, ks, _ = doc_spr(os.path.join(VNKU, "UiAutoNew", "tick_chon.spr"))
    ghi(os.path.join("spr", "uinew", "uiautonew", "tick_chon.spr"), [k.resize((TICK, TICK), Image.LANCZOS) for k in ks[:2]])


if __name__ == "__main__":
    khung()
    b = bat_tat()
    icon(b)
    nut_thanh_cong_cu(b)
    nut_do()
    o_tick()
    print("xong")
