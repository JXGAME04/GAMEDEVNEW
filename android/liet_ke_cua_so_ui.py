# -*- coding: utf-8 -*-
r"""[UITOADO 12/09 TOANBO] Liet ke MOI cua so giao dien trong game (lop KUi* + ini + muc chinh + Left/Top/Width/Height) de neo toan bo UI
(chu 07:55: "ban can neo toan bo UI trong game de cho can doi", "vao game lech rat nhieu icon, thong bao").
  * Quet Sources\S3Client\Ui\UiCase\*.cpp: ten tep ini ("Ui*.ini"), lop (class KUi* trong .h cung ten), muc chinh = Init(&Ini, "X")
    dau tien cua cua so (bo nhanh SCREEN_WIDTH == 1024: "X1024").
  * Doc ui\ui3\<ini> trong D:\jx1_android_data -> Left/Top/Width/Height cua muc chinh. Ini goc thiet ke cho 800x600.
  * Bo: cua so toan man (>= 790x590) tru danh sach truoc game (da co), cua so khong co ini / khong co muc chinh, lop da co trong
    bo cuc cua chu (giu cua chu).
Ghi: android\du_lieu_ghi_de\ui\cua_so_ui.json  { "KUiXxx|Main": [Left, Top, Width, Height] } -> sinh_bocuc_rong.py doc de them
muc voi neo tu suy (theo tam o trong 800x600) va dich sang khung thiet ke 1040x604 / 1371x617.
"""
import glob
import io
import json
import os
import re
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
UICASE = os.path.join("Sources", "S3Client", "Ui", "UiCase")
INI_DIR = r"D:\jx1_android_data\ui\ui3"
RA = os.path.join("android", "du_lieu_ghi_de", "ui", "cua_so_ui.json")


def doc(p):
    return io.open(p, encoding="latin-1", errors="replace").read()


def muc_ini(p_ini, muc):
    """Left/Top/Width/Height cua [muc] trong ini (khong phan biet hoa thuong)"""
    s = doc(p_ini)
    m = re.search(r"^\[" + re.escape(muc) + r"\]\s*$(.*?)(?=^\[|\Z)", s, re.M | re.S | re.I)
    if not m:
        return None
    gt = {}
    for k in ("Left", "Top", "Width", "Height"):
        mm = re.search(r"^" + k + r"\s*=\s*(-?\d+)", m.group(1), re.M | re.I)
        gt[k] = int(mm.group(1)) if mm else None
    return gt


def main():
    ket = {}
    bo = []
    for cpp in sorted(glob.glob(os.path.join(UICASE, "Ui*.cpp"))):
        s = doc(cpp)
        h = cpp[:-4] + ".h"
        lop = None
        if os.path.isfile(h):
            m = re.search(r"class\s+(KUi\w+)\s*[:{]", doc(h))
            lop = m.group(1) if m else None
        if not lop:
            continue
        inis = re.findall(r'"((?:\\\\Ui\\\\)?[Uu]i\w+\.ini)"', s)
        if not inis:
            continue
        ini = inis[0].split("\\")[-1]
        p_ini = os.path.join(INI_DIR, ini.lower())
        if not os.path.isfile(p_ini):
            bo.append((lop, ini, "khong co ini")); continue
        mucs = re.findall(r'Init\(&Ini,\s*"(\w+)"\)', s) + re.findall(r'Init\(pIni,\s*"(\w+)"\)', s)
        mucs = [x for x in mucs if not x.endswith("1024")]
        if not mucs:
            bo.append((lop, ini, "khong thay Init")); continue
        muc = mucs[0]
        gt = muc_ini(p_ini, muc)
        if not gt or gt["Width"] is None or gt["Height"] is None:
            bo.append((lop, ini, "muc %s khong co kich thuoc" % muc)); continue
        L, T, W, H = gt["Left"] or 0, gt["Top"] or 0, gt["Width"], gt["Height"]
        if W >= 790 and H >= 590:
            bo.append((lop, ini, "toan man %dx%d" % (W, H))); continue
        ket["%s|%s" % (lop, muc)] = [L, T, W, H]
    os.makedirs(os.path.dirname(RA), exist_ok=True)
    io.open(RA, "w", encoding="utf-8", newline="\n").write(json.dumps(ket, indent=1, ensure_ascii=True, sort_keys=True))
    print("da ghi %d cua so -> %s; bo %d" % (len(ket), RA, len(bo)))
    for b in bo[:40]:
        print("   bo:", b)


if __name__ == "__main__":
    main()
