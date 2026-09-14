# -*- coding: latin-1 -*-
r"""[HAOQUANG 14/09] Sua ui\ui3\uioptions2.ini (ban da sinh boi sinh_uioptions2_camera.py) cho 10 cong tac o cua so "Toi uu" (KUiOptions2):
[ToggleBtn]/[ToggleStatus] Top 101 -> 70 (5 hang x 2 cot, cach 31 nhu cu: 70, 101, 132, 163, 194; nut Dong o Top=218 khong dung) va
[ToggleOptionsName] 7..9 = Vong quai / Vong do mac / Sang vat roi (TCVN3 qua vn_edit.vn, ten <= 12 ky tu de khong bi cat o nut rong 72 px).
Chep BYTE, chi doi phan ASCII + noi ten. Ghi vao lop ghi de (git), D:\jx1_android_data (may ao) va tuy chon goi may chu tai (tham so 1;
nho --chi-manifest sau do). Chay lai nhieu lan khong sao (bo qua khi da co dau hieu).

Dung:  python android\sinh_uioptions2_haoquang.py [D:\jx1_android_data_dt_v4]
"""
import io
import os
import sys

sys.path.insert(0, os.path.join(os.path.expanduser('~'), '.claude', 'skills', 'swordonline-dev', 'scripts'))
from vn_edit import vn  # tieng Viet -> chuoi latin-1 mang byte TCVN3

GOC = os.path.dirname(os.path.abspath(__file__))
DICH = [os.path.join(GOC, "du_lieu_ghi_de", "ui", "ui3", "uioptions2.ini"),
        os.path.join(r"D:\jx1_android_data", "ui", "ui3", "uioptions2.ini")]
if len(sys.argv) > 1:
    DICH.append(os.path.join(sys.argv[1], "ui", "ui3", "uioptions2.ini"))
DAU_CU = "[CAMERA 13/09 TUYCHON]"
DAU_HIEU = "[HAOQUANG 14/09]"

TEN = [u"V\u00f2ng qu\u00e1i", u"V\u00f2ng \u0111\u1ed3 m\u1eb7c", u"S\u00e1ng v\u1eadt r\u01a1i"]   # 7, 8, 9


def cao(t):
    return sum(1 for c in t if ord(c) >= 0x80)


def doi_top(s, muc, nl):
    i = s.index("[" + muc + "]")
    j = s.find(nl + "[", i + 1)
    if j < 0:
        j = len(s)
    khoi = s[i:j]
    if nl + "Top=101" + nl not in khoi:
        raise SystemExit("khong thay Top=101 trong [%s]" % muc)
    khoi = khoi.replace(nl + "Top=101" + nl, nl + "Top=70" + nl, 1)
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
        raise SystemExit("chua sinh ban camera (sinh_uioptions2_camera.py) o " + p)
    nl = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
    s2 = doi_top(s, "ToggleBtn", nl)
    s2 = doi_top(s2, "ToggleStatus", nl)
    i = s2.index("[ToggleOptionsName]")
    j = s2.find(nl + "6=", i)
    if j < 0:
        raise SystemExit("khong thay dong 6= trong [ToggleOptionsName]")
    k = s2.find(nl, j + 1)
    if k < 0:
        k = len(s2)
    them = nl + "; " + DAU_HIEU + " 3 cong tac hieu ung: 7 vong hao quang quai tinh anh/boss (Core KNpc.cpp), 8 vong hao quang trang bi cua minh, 9 cot sang + loe vat pham roi (Core KObj.cpp); Top 101 -> 70 (5 hang)"
    for n, t in enumerate(TEN):
        them += nl + "%d=%s" % (7 + n, vn(t))
    s2 = s2[:k] + them + s2[k:]
    if cao(s2) - cao(s) != sum(cao(vn(t)) for t in TEN):
        raise SystemExit("so byte cao lech")
    io.open(p, "w", encoding="latin-1", newline="").write(s2)
    print("da ghi:", p)
