# -*- coding: utf-8 -*-
r"""[SUAGD 13/09] Them nut ANH "Chinh giao dien" vao ui\ui3\uioptions.ini cua ban Android (lop ghi de): muc [ChinhGiaoDien]
dung anh nut trong cua VNKU (spr\ui3\uisuagd\nut_chinh_gd.spr, 97x29, 3 khung Up/Down/Over - sinh tu btn_f00.png), nhan
ve de bang KWndLabeledButton; dat ben phai nut Dong, nut Dong [CloseBtn] doi tu Left=53 sang Left=4 de co cho.
Chu 13/09: "them nut nhu cac nut khac trong cai dat de dong bo".
Chay SAU android\sinh_uioptions_fps.py (kich ban do sinh lai ini tu ban PC + [Fps], se mat muc nay). Chep BYTE (latin-1),
chi noi them ASCII + nhan TCVN3; chay lai nhieu lan khong sao (muc cu duoc go ra roi ghi lai). Ghi vao: lop ghi de (git),
D:\jx1_android_data (may ao) va tuy chon goi may chu tai (tham so 1).

Dung:  python android\sinh_uioptions_chinhgd.py [D:\jx1_android_data_dt_v4]
"""
import io
import os
import sys

GOC = os.path.dirname(os.path.abspath(__file__))
NGUON = os.path.join(GOC, "du_lieu_ghi_de", "ui", "ui3", "uioptions.ini")
DICH = [NGUON, os.path.join(r"D:\jx1_android_data", "ui", "ui3", "uioptions.ini")]
if len(sys.argv) > 1:
    DICH.append(os.path.join(sys.argv[1], "ui", "ui3", "uioptions.ini"))

# "Chinh giao dien" ma TCVN3 (khong go tay byte: bang cua skill swordonline-dev, vn_edit.vn)
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402
NHAN = vn("Chỉnh giao diện")
DAU = "; [SUAGD 13/09]"

s = io.open(NGUON, encoding="latin-1", newline="").read()
nl = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
cao = lambda t: sum(1 for c in t if ord(c) >= 0x80)

# go muc cu (tu dong chu thich [SUAGD 13/09] toi truoc muc ke tiep hoac het tep)
if "[ChinhGiaoDien]" in s:
    i = s.index(DAU) if DAU in s else s.index("[ChinhGiaoDien]")
    j = s.find(nl + "[", s.index("[ChinhGiaoDien]") + 1)
    s = s[:i].rstrip("\r\n") + nl + (s[j + len(nl):] if j >= 0 else "")
c0 = cao(s)

# nut Dong sang trai
i = s.index("[CloseBtn]")
j = s.index("[", i + 1)
khoi = s[i:j]
if nl + "Left=53" + nl in khoi:
    khoi = khoi.replace(nl + "Left=53" + nl, nl + "Left=4" + nl, 1)
    s = s[:i] + khoi + s[j:]

them = nl.join([
    DAU + " nut ANH \"Chinh giao dien\": nut trong VNKU 97x29, 3 khung (spr\\ui3\\uisuagd\\nut_chinh_gd.spr, sinh tu btn_f00.png),",
    ";   nhan ve de; ben phai nut Dong (nut Dong doi Left 53 -> 4). Chu: \"them nut nhu cac nut khac trong cai dat de dong bo\".",
    "[ChinhGiaoDien]", "Left=104", "Top=218", "Width=97", "Height=29", "Up=0", "Down=1", "Over=2", "OverFrame=2", "Trans=0",
    "Image=\\spr\\ui3\\uisuagd\\nut_chinh_gd.spr", "Font=13", "LabelYOffset=8",
    "Color=218,255,165", "BorderColor=23,68,0", "OverColor=255,253,122", "OverBorderColor=90,30,0",
    "Label=" + NHAN, "", ""])
if not s.endswith(nl):
    s += nl
s += nl + them
if cao(s) - c0 != cao(NHAN):
    raise SystemExit("so byte cao lech")
for p in DICH:
    os.makedirs(os.path.dirname(p), exist_ok=True)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("da ghi:", p)
