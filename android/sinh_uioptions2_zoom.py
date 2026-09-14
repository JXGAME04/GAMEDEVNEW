# -*- coding: latin-1 -*-
r"""[ZOOM3D 14/09] Sua ui\ui3\uioptions2.ini (ban da qua sinh_uioptions2_camera.py + sinh_uioptions2_haoquang.py) cho 12 cong tac o cua so
"Toi uu" (KUiOptions2): [ToggleBtn]/[ToggleStatus] Top 70 -> 60, Interval 100,31 -> 100,27 (6 hang x 2 cot: 60, 87, 114, 141, 168, 195; nut
Dong o Top=218 khong dung) va [ToggleOptionsName] 9..11 = Zoom nhanh / Zoom cham / Lac camera (TCVN3 qua vn_edit.vn, ten <= 12 ky tu).
Chep BYTE, chi doi phan ASCII + noi ten. Ghi vao lop ghi de (git), D:\jx1_android_data (may ao) va tuy chon goi may chu tai (tham so 1;
nho --chi-manifest sau do). Chay lai nhieu lan khong sao (bo qua khi da co dau hieu).

Dung:  python android\sinh_uioptions2_zoom.py [D:\jx1_android_data_dt_v4]
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
DAU_CU = "[HAOQUANG 14/09]"
DAU_HIEU = "[ZOOM3D 14/09]"

TEN = [u"Zoom nhanh", u"Zoom ch\u1eadm", u"L\u1eafc camera"]   # 9, 10, 11


def cao(t):
    return sum(1 for c in t if ord(c) >= 0x80)


def doi_top(s, muc, nl):
    i = s.index("[" + muc + "]")
    j = s.find(nl + "[", i + 1)
    if j < 0:
        j = len(s)
    khoi = s[i:j]
    if nl + "Top=70" + nl not in khoi:
        raise SystemExit("khong thay Top=70 trong [%s]" % muc)
    khoi = khoi.replace(nl + "Top=70" + nl, nl + "Top=60" + nl, 1)
    return s[:i] + khoi + s[j:]


for p in DICH:
    if not os.path.isfile(p):
        print("khong thay:", p)
        continue
    s = io.open(p, encoding="latin-1", newline="").read()
    if DAU_HIEU in s:
        print("bo qua (da co):", p)
        continue
    if DAU_CU not in s:
        raise SystemExit("chua qua sinh_uioptions2_haoquang.py o " + p)
    nl = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
    s2 = doi_top(s, "ToggleBtn", nl)
    s2 = doi_top(s2, "ToggleStatus", nl)
    if nl + "Interval=100,31" + nl not in s2:
        raise SystemExit("khong thay Interval=100,31 o " + p)
    s2 = s2.replace(nl + "Interval=100,31" + nl, nl + "Interval=100,27" + nl, 1)
    i = s2.index("[ToggleOptionsName]")
    j = s2.find(nl + "8=", i)
    if j < 0:
        raise SystemExit("khong thay dong 8= trong [ToggleOptionsName]")
    k = s2.find(nl, j + 1)
    if k < 0:
        k = len(s2)
    them = nl + "; " + DAU_HIEU + " 3 cong tac: 9 chum hai ngon nhay gap doi, 10 nhay mot nua (loai tru nhau), 11 camera lac nhe khi lia; Top 70 -> 60, cach 31 -> 27 (6 hang)"
    for n, t in enumerate(TEN):
        them += nl + "%d=%s" % (9 + n, vn(t))
    s2 = s2[:k] + them + s2[k:]
    if cao(s2) - cao(s) != sum(cao(vn(t)) for t in TEN):
        raise SystemExit("so byte cao lech")
    io.open(p, "w", encoding="latin-1", newline="").write(s2)
    print("da ghi:", p)
