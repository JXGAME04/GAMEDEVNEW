# -*- coding: latin-1 -*-
r"""[CAMERA 13/09 TUYCHON] Sinh ui\ui3\uioptions2.ini cho ban Android = ini goc cua PC (cua so "Toi uu" / KUiOptions2) + 3 cong tac
camera: [ToggleBtn]/[ToggleStatus] Top 163 -> 101 (4 hang: 4 muc cu + lia canh / nhin rong / lia ve nhanh, nam trong vung trong
phia tren cua main2.spr) va [ToggleOptionsName] 4..6 (chu TCVN3 ma hoa bang vn_edit.vn, khong go tay byte). Chep BYTE, chi doi
phan ASCII + noi ten. Ghi vao: lop ghi de (git), D:\jx1_android_data (may ao) va tuy chon goi may chu tai (tham so 1; nho
--chi-manifest sau do). Chay lai nhieu lan khong sao (bo qua khi da co).

Dung:  python android\sinh_uioptions2_camera.py [D:\jx1_android_data_dt_v4]
"""
import io
import os
import sys

sys.path.insert(0, os.path.join(os.path.expanduser('~'), '.claude', 'skills', 'swordonline-dev', 'scripts'))
from vn_edit import vn  # tieng Viet -> chuoi latin-1 mang byte TCVN3

GOC = os.path.dirname(os.path.abspath(__file__))
NGUON = [os.path.join(r"D:\jx1_android_data", "ui", "Ui3", "uioptions2.ini"),
         r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\ui\Ui3\UiOptions2.ini"]
DICH = [os.path.join(GOC, "du_lieu_ghi_de", "ui", "ui3", "uioptions2.ini"),
        os.path.join(r"D:\jx1_android_data", "ui", "ui3", "uioptions2.ini")]
if len(sys.argv) > 1:
    DICH.append(os.path.join(sys.argv[1], "ui", "ui3", "uioptions2.ini"))
DAU_HIEU = "[CAMERA 13/09 TUYCHON]"

TEN = [u"Lia c\u1ea3nh", u"Nh\u00ecn r\u1ed9ng", u"Lia v\u1ec1 nhanh"]   # 4, 5, 6

def cao(t):
    return sum(1 for c in t if ord(c) >= 0x80)

def doi_top(s, muc, nl):
    """Trong muc [muc]: dong Top=163 -> Top=101 (chi mot dong, chi trong muc do)."""
    i = s.index("[" + muc + "]")
    j = s.find(nl + "[", i + 1)
    if j < 0:
        j = len(s)
    khoi = s[i:j]
    if nl + "Top=163" + nl not in khoi:
        raise SystemExit("khong thay Top=163 trong [%s]" % muc)
    khoi = khoi.replace(nl + "Top=163" + nl, nl + "Top=101" + nl, 1)
    return s[:i] + khoi + s[j:]

src = None
for p in NGUON:
    if os.path.isfile(p):
        t = io.open(p, encoding="latin-1", newline="").read()
        if DAU_HIEU not in t:
            src = p
            break
if not src:
    # moi nguon deu da sinh roi -> khong con viec
    print("bo qua: nguon da co dau hieu hoac khong thay: " + " | ".join(NGUON))
    sys.exit(0)
s = io.open(src, encoding="latin-1", newline="").read()
nl = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
s2 = doi_top(s, "ToggleBtn", nl)
s2 = doi_top(s2, "ToggleStatus", nl)
# noi ten muc 4..6 sau dong "3=..." cua [ToggleOptionsName]
i = s2.index("[ToggleOptionsName]")
j = s2.find(nl + "3=", i)
if j < 0:
    raise SystemExit("khong thay dong 3= trong [ToggleOptionsName]")
k = s2.find(nl, j + 1)          # cuoi dong 3=
if k < 0:
    k = len(s2)
them = nl + "; " + DAU_HIEU + " 3 cong tac camera (Platform/JxLiaCanh): 4 lia mot ngon, 5 chum nhin rong, 6 lia ve nhanh; Top 163 -> 101 o [ToggleBtn]/[ToggleStatus]"
for n, t in enumerate(TEN):
    them += nl + "%d=%s" % (4 + n, vn(t))
s2 = s2[:k] + them + s2[k:]
if cao(s2) - cao(s) != sum(cao(vn(t)) for t in TEN):
    raise SystemExit("so byte cao lech")
for p in DICH:
    if os.path.isfile(p) and DAU_HIEU in io.open(p, encoding="latin-1", newline="").read():
        print("bo qua (da co):", p)
        continue
    os.makedirs(os.path.dirname(p), exist_ok=True)
    io.open(p, "w", encoding="latin-1", newline="").write(s2)
    print("da ghi:", p)
