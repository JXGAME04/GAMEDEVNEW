# -*- coding: utf-8 -*-
r"""[AUTO 12/09 USV b] Chu 17:50: "ban lay icon nay thay vao Auto duoc" + "ban da thay icon Auto thanh hinh
BONG HOA cho toi dau".

Anh chu gui la nut tron DO cua bo auto ban USVOLAM / GHM (kho tham khao C:\Users\nguye\Downloads\NHACTAI\GHM_auto):
dia do vien vang, giua la banh rang vang + chu "CONFIG AUTO" - thu nho bang o icon thi nhin dung nhu mot bong hoa
do nhuy vang. Truoc do toi tu GHEP (vong tron kiem cheo + chu "Auto") nen chu bao "khong phai ban che".

Bo sinh nay lay NGUYEN hai anh co san, chi thu nho cho vua o icon (khong ghep, khong ve them):
  spr\uinew\uitoolscontrolbar\auto_m.spr   48x48, 2 khung:
      [0] auto TAT  = config_auto.png     (do tham)
      [1] auto BAT  = config_auto_2.png   (cung anh, ban sang hon - dang bam / dang bat)
  Kem theo: ui\Ui3\uitoolscontrolbar.ini muc [WAuto] Height 56 -> 48 (anh tron, khong con cho chu ghep ben duoi).

Nguon CHI DOC. Dung: python android\anh_auto_usv.py
"""
import io
import os
import sys

from PIL import Image

GOC = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, GOC)
from bo_cuc_vnku_mobile import ghi_spr_nhieu_khung, ghi_moi_noi  # noqa: E402

#   kho tham khao CHI DOC (xem MEMORY: jx1-mobile-nguon-tham-khao)
KHO = r"C:\Users\nguye\Downloads\NHACTAI\GHM_auto"
E_TAT = "config_auto.png"       # dia do, banh rang vang, chu CONFIG AUTO
E_BAT = "config_auto_2.png"     # cung anh, ban sang hon
ICON = 48


def cat_vien(im):
    """Bo vien trong suot roi tra ve hinh VUONG (thu nho khong meo)."""
    bb = im.getchannel("A").point(lambda a: 255 if a > 20 else 0).getbbox()
    if bb:
        im = im.crop(bb)
    c = max(im.size)
    o = Image.new("RGBA", (c, c), (0, 0, 0, 0))
    o.alpha_composite(im, ((c - im.size[0]) // 2, (c - im.size[1]) // 2))
    return o


def doc(ten):
    p = os.path.join(KHO, ten)
    if not os.path.isfile(p):
        raise SystemExit("khong thay %s" % p)
    return cat_vien(Image.open(p).convert("RGBA")).resize((ICON, ICON), Image.LANCZOS)


def sua_ini():
    """[WAuto] cao 56 (cho vong tron + chu "Auto" ghep) -> 48: anh moi la mot hinh tron da co chu ben trong."""
    p = os.path.join(GOC, "du_lieu_ghi_de", "ui", "Ui3", "uitoolscontrolbar.ini")
    s = io.open(p, encoding="latin-1", newline="").read()
    nl = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
    dau = s.find("[WAuto]")
    if dau < 0:
        print("  (khong thay muc [WAuto] trong ini)")
        return
    cuoi = s.find(nl + "[", dau + 1)
    khoi = s[dau:cuoi if cuoi > 0 else len(s)]
    moi = khoi.replace("Height=56", "Height=%d" % ICON)
    if moi == khoi:
        print("  (ini da dung co roi)")
        return
    s = s[:dau] + moi + s[dau + len(khoi):]
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("  sua ini: [WAuto] Height -> %d" % ICON)
    ghi_moi_noi(os.path.join("ui", "Ui3", "uitoolscontrolbar.ini"), io.open(p, "rb").read())


def main():
    khung = [doc(E_TAT), doc(E_BAT)]
    rel = os.path.join("spr", "uinew", "uitoolscontrolbar", "auto_m.spr")
    tmp = os.path.join(os.environ.get("TEMP", GOC), "auto_usv_tmp.spr")
    msg = ghi_spr_nhieu_khung(khung, tmp)
    print("%-48s %dx%d x%d  %s" % (rel, ICON, ICON, len(khung), msg))
    ghi_moi_noi(rel, io.open(tmp, "rb").read())
    for i, k in enumerate(khung):
        k.save(os.path.join(os.environ.get("TEMP", GOC), "auto_usv_%d.png" % i))
    sua_ini()
    print("xong")


if __name__ == "__main__":
    main()
