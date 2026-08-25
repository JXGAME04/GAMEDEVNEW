# -*- coding: utf-8 -*-
r"""[25/08] Rut gon 2 goi thoai SONG vuot tran 511B cua posthouse (menu se DUT nhu
NPC 769 neu mo): messenger_what 704B + messenger_storehouse 700B. 3 goi con lai
(flyhorse/mountaindeity/orderlevel/limittotask) la MA CHET (muc menu goi bi comment
tu goc) - giu nguyen. Nhan the: thoai noi "cap 120" sai thuc te server -> noi
HD_CFG("TS_CAP_TOITHIEU") cho dung config (posthouse da Include cauhinh tu r8).
Van ban gon giu du y goc, tieng Viet co dau (TCVN3)."""
import io, os, sys, shutil
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from tcvn3 import tcvn3 as T

P = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\task\tollgate\messenger\posthouse.lua"
s = io.open(P, "rb").read()
NL = b"\r\n" if b"\r\n" in s else b"\n"

def dong_bat_dau(marker):
    i = s.find(marker)
    assert i > 0, marker
    j = s.rfind(NL, 0, i)
    k = s.find(NL, i)
    return s[j+len(NL):k]

def dong_sau_ham(ten_ham):
    """dong Describe dau tien SAU 'function <ten_ham>'"""
    i = s.find(b"function " + ten_ham)
    assert i > 0, ten_ham
    d = s.find(b"Describe(", i)
    assert d > 0
    j = s.rfind(NL, 0, d)
    k = s.find(NL, d)
    return s[j+len(NL):k]

# ---- 1) messenger_what ----
cu = dong_sau_ham(b"messenger_what")
van = ("Nhi\u1ec7m v\u1ee5 T\u00edn S\u1ee9: \u0111\u01b0a th\u01b0 gi\u00fap Tri\u1ec1u \u0111\u00ecnh, c\u1ea7n \u0111\u1ea1t ")
van2 = (" tr\u1edf l\u00ean. V\u01b0\u1ee3t \u1ea3i <color=red>Thi\u00ean B\u1ea3o Kh\u1ed1 <color>: gi\u1ebft boss gi\u1eef r\u01b0\u01a1ng r\u1ed3i m\u1edf 5 B\u1ea3o R\u01b0\u01a1ng \u0111\u00fang th\u1ee9 t\u1ef1 l\u00e0 qua \u1ea3i. Ho\u00e0n th\u00e0nh nh\u1eadn \u0111i\u1ec3m t\u00edch l\u0169y \u0111\u1ed5i th\u01b0\u1edfng; m\u1ed7i ng\u00e0y 2 l\u01b0\u1ee3t, th\u00eam 1 l\u01b0\u1ee3t b\u1eb1ng Thi\u00ean Kh\u1ed1 B\u1ea3o L\u1ec7nh. C\u1ed1 l\u00ean chi\u1ebfn h\u1eefu!")
moi = (b'\tDescribe(DescLink_YiGuan..": ' + T(van) +
       b'<color=red> c\xca'  # "cấp" ghep tay? dung T() cho chac:
       )
# lam sach: dung T() tron
moi = (b'\tDescribe(DescLink_YiGuan..": ' + T(van) +
       b'<color=red> ' + T("c\u1ea5p ") + b'"..HD_CFG("TS_CAP_TOITHIEU", 90).."' + T(" tr\u1edf l\u00ean <color>") + b'.' +
       T(" V\u01b0\u1ee3t \u1ea3i <color=red>Thi\u00ean B\u1ea3o Kh\u1ed1 <color>: gi\u1ebft boss gi\u1eef r\u01b0\u01a1ng r\u1ed3i m\u1edf 5 B\u1ea3o R\u01b0\u01a1ng \u0111\u00fang th\u1ee9 t\u1ef1 l\u00e0 qua \u1ea3i. Ho\u00e0n th\u00e0nh nh\u1eadn \u0111i\u1ec3m t\u00edch l\u0169y \u0111\u1ed5i th\u01b0\u1edfng; m\u1ed7i ng\u00e0y 2 l\u01b0\u1ee3t, th\u00eam 1 l\u01b0\u1ee3t b\u1eb1ng Thi\u00ean Kh\u1ed1 B\u1ea3o L\u1ec7nh. C\u1ed1 l\u00ean chi\u1ebfn h\u1eefu!") +
       b'",4,')
assert cu in s
s = s.replace(cu, moi, 1)

# ---- 2) messenger_storehouse ----
cu = dong_sau_ham(b"messenger_storehouse")
moi = (b'\tDescribe(DescLink_YiGuan..": ' +
       T("Khi nh\u1eadn nhi\u1ec7m v\u1ee5, h\u1ec7 th\u1ed1ng ph\u00e1t ng\u1eabu nhi\u00ean 5 ch\u1eef s\u1ed1 - m\u1edf <color=red>B\u1ea3o R\u01b0\u01a1ng <color>theo \u0111\u00fang th\u1ee9 t\u1ef1 \u0111\u00f3 l\u00e0 qua \u1ea3i. Mu\u1ed1n m\u1edf r\u01b0\u01a1ng ph\u1ea3i gi\u1ebft <color=red>B\u1ea3o Kh\u1ed1 Th\u1ee7 H\u1ed9 Gi\u1ea3 <color>\u0111\u1ee9ng c\u1ea1nh. <color=red>Ng\u0169 H\u00e0nh Ph\u00f9 <color>(b\u00e1n \u1edf K\u1ef3 Tr\u00e2n C\u00e1c) m\u1edf ra Tri\u1ec7t Kim/M\u1ed9c/Th\u1ee7y/H\u1ecfa/Th\u1ed5 Ph\u00f9, d\u00f9ng \u0111\u1ec3 \u0111\u00e1nh boss d\u1ec5 h\u01a1n h\u1eb3n.") +
       b'",2,"' + T("Quay l\u1ea1i") + b'/messenger_what"," ' + T("K\u1ebft th\u00fac \u0111\u1ed1i tho\u1ea1i!") + b'/no")')
assert cu in s
s = s.replace(cu, moi, 1)

# ---- 3) messenger_levelmap: sua so 120 -> HD_CFG ----
cu = dong_sau_ham(b"messenger_levelmap")
moi = (b'\tDescribe(DescLink_YiGuan..": ' +
       T("Tr\u01b0\u1edbc m\u1eaft ch\u1ec9 c\u00f3 ") + b'<color=red> ' + T("c\u1ea5p ") + b'"..HD_CFG("TS_CAP_TOITHIEU", 90).."' +
       T(" tr\u1edf l\u00ean <color>") + T(" m\u1ed9t m\u1ee9c \u0111\u1ed9 nhi\u1ec7m v\u1ee5") +
       b'",2,"' + T("Quay l\u1ea1i") + b'/messenger_what"," ' + T("K\u1ebft th\u00fac \u0111\u1ed1i tho\u1ea1i!") + b'/no")')
assert cu in s
s = s.replace(cu, moi, 1)

q = P + ".truoc_rutgon_2508"
if not os.path.isfile(q): shutil.copyfile(P, q)
io.open(P, "wb").write(s)
print("DA RUT GON what/storehouse + sua cap levelmap theo config")
