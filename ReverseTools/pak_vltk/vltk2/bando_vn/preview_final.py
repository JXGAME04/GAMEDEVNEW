# -*- coding: utf-8 -*-
"""Anh xem truoc CUOI cho chu: ban do VN 2.0 + 7 nhan bang hoi ve dung hinh hoc da hieu chinh
theo anh chup that (nhan can giua nut 102 px, chu ve thap hon BtnStartPos.y ~5 px, rong ~255 px)."""
import os, re, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
from PIL import Image, ImageDraw, ImageFont

HERE = os.path.dirname(os.path.abspath(__file__))
INI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\Ui\Ui3\UiWorldMap.ini"
ini = open(INI, "rb").read().decode("latin-1")
cities = ["fengxiang", "chengdu", "dali", "bianjing", "xiangyang", "yangzhou", "linan"]
btn = {s: tuple(map(int, re.search(r"\[%s\][^\[]*?BtnStartPos=(\d+),(\d+)" % s, ini, re.S).groups())) for s in cities}
try:
    font = ImageFont.truetype("arialbd.ttf", 12)
except Exception:
    font = ImageFont.load_default()
TXT = "Bang h\u1ed9i chi\u1ebfm l\u0129nh: V\u00f4 ch\u1ee7 - Thu\u1ebf 0%"

for tag, src in (("world_VN", "update_43841ADC.png"), ("world_CN_cu", "update_2C3D0BA1.png")):
    img = Image.open(os.path.join(HERE, "vnmap", src)).convert("RGB")
    dr = ImageDraw.Draw(img)
    for s in cities:
        x, y = btn[s]
        cx = x + 51
        w = dr.textlength(TXT, font=font)
        x0 = int(cx - w / 2)
        # bong toi cho de doc nhu game (TextShadow)
        dr.text((x0 + 1, y + 6), TXT, fill=(0, 0, 0), font=font)
        dr.text((x0, y + 5), TXT, fill=(0, 255, 0), font=font)
    out = os.path.join(HERE, "giao_vn", "XEMTRUOC_%s.png" % tag)
    img.save(out)
    print("->", out)
