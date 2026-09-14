# -*- coding: latin-1 -*-
r"""[ZOOMTHANH 14/09 e] Don cua so "Toi uu" (ui\ui3\uioptions2.ini) con 11 cong tac theo chu ("chum zoom la gi? nhung tinh nang nao khong dung don
sach phan cai dat"): bo 9 Zoom nhanh, 10 Zoom cham, 12 Chum zoom; 9 = Lac camera, 10 = Thanh zoom; [ToggleBtn]/[ToggleStatus] Top 58 -> 60,
Interval 100,23 -> 100,27 (6 hang x 2 cot). Viet lai ca khoi [ToggleOptionsName] (0..8 giu nguyen byte). Chep BYTE (latin-1).
Ghi vao lop ghi de (git), D:\jx1_android_data (may ao) va tuy chon goi may chu tai (tham so 1; nho --chi-manifest sau do). Chay lai khong sao.

Dung:  python android\sinh_uioptions2_don.py [D:\jx1_android_data_dt_v4]
"""
import io
import os
import sys

sys.path.insert(0, os.path.join(os.path.expanduser('~'), '.claude', 'skills', 'swordonline-dev', 'scripts'))
from vn_edit import vn

GOC = os.path.dirname(os.path.abspath(__file__))
DICH = [os.path.join(GOC, "du_lieu_ghi_de", "ui", "ui3", "uioptions2.ini"),
        os.path.join(r"D:\jx1_android_data", "ui", "ui3", "uioptions2.ini")]
if len(sys.argv) > 1:
    DICH.append(os.path.join(sys.argv[1], "ui", "ui3", "uioptions2.ini"))
DAU_HIEU = "[ZOOMTHANH 14/09 e]"


def cao(t):
    return sum(1 for c in t if ord(c) >= 0x80)


def doi_top(s, muc, nl):
    i = s.index("[" + muc + "]")
    j = s.find(nl + "[", i + 1)
    if j < 0:
        j = len(s)
    khoi = s[i:j]
    if nl + "Top=58" + nl not in khoi:
        raise SystemExit("khong thay Top=58 trong [%s]" % muc)
    return s[:i] + khoi.replace(nl + "Top=58" + nl, nl + "Top=60" + nl, 1) + s[j:]


for p in DICH:
    if not os.path.isfile(p):
        print("khong thay:", p)
        continue
    s = io.open(p, encoding="latin-1", newline="").read()
    if DAU_HIEU in s:
        print("bo qua (da co):", p)
        continue
    nl = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
    s2 = doi_top(s, "ToggleBtn", nl)
    s2 = doi_top(s2, "ToggleStatus", nl)
    if nl + "Interval=100,23" + nl not in s2:
        raise SystemExit("khong thay Interval=100,23 o " + p)
    s2 = s2.replace(nl + "Interval=100,23" + nl, nl + "Interval=100,27" + nl, 1)
    i = s2.index("[ToggleOptionsName]")
    j = s2.find(nl + "8=", i)
    if j < 0:
        raise SystemExit("khong thay dong 8= trong [ToggleOptionsName]")
    k = s2.find(nl, j + 1)
    if k < 0:
        k = len(s2)
    giu = s2[:k]          # toi het dong 8=
    them = (nl + "; " + DAU_HIEU + " con 11 cong tac: bo Zoom nhanh / Zoom cham / Chum zoom (chum hai ngon + do nhay chi con qua config [Cham] ZoomChum / ZoomNhay);"
            + nl + ";   9 camera lac nhe khi lia, 10 hien thanh keo zoom; Top 58 -> 60, cach 23 -> 27 (6 hang)"
            + nl + "9=" + vn(u"L\u1eafc camera")
            + nl + "10=Thanh zoom" + nl)
    s3 = giu + them
    if cao(s3) - cao(s2[:k]) != cao(vn(u"L\u1eafc camera")):
        raise SystemExit("so byte cao lech")
    io.open(p, "w", encoding="latin-1", newline="").write(s3)
    print("da ghi:", p)
