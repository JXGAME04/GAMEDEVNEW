# -*- coding: utf-8 -*-
r"""[WAUTO 12/09] BO SINH GHI CHU: dua 283 ghi chu re chuot + 17 trang huong dan theo the cua WAuto.exe sang ban mobile.

Ban PC co s_aTips (283 dong, WAuto.cpp:3767-4051) hien khi re chuot vao tung o, va s_aNote (17 the) mo bang nut
[H] / [Tro giup] / [?]. Ban mobile khong co "re chuot" nen gop lai: nut [?] tren khung Auto mo BANG PHU hien huong dan
cua THE DANG MO - trang note cua the do roi tung o mot ("nhan: ghi chu"). Day la cho chu bao "nhieu tinh nang khong
biet de lam gi".

Vao (CHI DOC): WAutoUI/WAuto.cpp (UTF-16), android/wauto_bang.json (the nao co o nao, nhan cua o).
Ra: android/du_lieu_ghi_de/ui/ui3/uiwauto_ghichu.ini  (TCVN3, latin-1)
      [The]   0..14    = trang huong dan cua the (dau || la xuong doan)
      [Nhan]  IDC_xxx  = nhan chu cua o (o tick lay nhan cua no; o nhap / hop chon lay dong chu cung hang)
      [O]     IDC_xxx  = ghi chu cua o do
Chu HOA co dau khong co trong TCVN3 nen tu ha thanh chu thuong (nhu sinh_bocuc_wauto.py); dau cau la (em dash, ngoac
nhon...) doi sang dau ASCII tuong duong.
Dung: python android/sinh_ghichu_wauto.py
"""
import io
import json
import os
import re
import sys
import unicodedata

GOC = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

CPP = os.path.join(GOC, "WAutoUI", "WAuto.cpp")
RA = os.path.join(GOC, "android", "du_lieu_ghi_de", "ui", "ui3", "uiwauto_ghichu.ini")

DOI_DAU = (("—", "-"), ("–", "-"), ("’", "'"), ("‘", "'"), ("“", '"'), ("”", '"'),
           ("…", "..."), ("≥", ">="), ("≤", "<="), ("×", "x"), ("→", "->"), ("·", "-"))


def tcvn(s):
    """tieng Viet -> TCVN3; chu HOA co dau (TCVN3 khong co) thi ha thanh chu thuong tung chu"""
    for a, b in DOI_DAU:
        s = s.replace(a, b)
    s = s.replace("&", "").strip()
    try:
        return vn(s)
    except Exception:
        pass
    out = []
    for ch in s:
        try:
            vn(ch)
            out.append(ch)
        except Exception:
            try:
                vn(ch.lower())
                out.append(ch.lower())
            except Exception:
                out.append("".join(c for c in unicodedata.normalize("NFD", ch) if unicodedata.category(c) != "Mn"))
    return vn("".join(out))


def doc_u16(p):
    return io.open(p, "rb").read().decode("utf-16-le").replace("\r\n", "\n")


def main():
    cpp = doc_u16(CPP)
    d = json.load(io.open(os.path.join(GOC, "android", "wauto_bang.json"), encoding="utf-8"))

    # --- 17 trang huong dan theo the (s_aNote); chi so = thu tu the cua ShowTab = thu tu TEN_TAB
    a = cpp.index("static const wchar_t* const s_aNote[WA_SO_TAB]")
    b = cpp.index("};", a)
    note = re.findall(r'L"([^"]*)"', cpp[a:b])

    # --- 283 ghi chu tung o (s_aTips): { IDC_xxx, L"..." }
    a = cpp.index("static const WATipEntry s_aTips[]")
    b = cpp.index("};", a)
    tip = {}
    for m in re.finditer(r'\{\s*(IDC_\w+),\s*L"([^"]*)"', cpp[a:b]):
        tip.setdefault(m.group(1), m.group(2))

    # --- nhan chu cua tung o (bang phu hien "nhan: ghi chu" nen khong phai hoi trang)
    co = set()
    nhan = {}
    for t, rows in d["tabs"].items():
        for r in rows:
            co.add(r["idc"])
            if r.get("label"):
                nhan[r["idc"]] = r["label"]
        for r in rows:
            # o nhap / hop chon / danh sach khong co nhan cua rieng no -> lay dong chu GAN NHAT BEN TRAI cung hang
            if r["idc"] in nhan or r["kind"] not in ("onhap", "chon", "dsach"):
                continue
            cung = [q for q in rows if abs(q["y"] - r["y"]) <= 3 and q.get("label") and q["x"] <= r["x"]]
            if cung:
                nhan[r["idc"]] = sorted(cung, key=lambda q: -q["x"])[0]["label"]

    out = ["; [WAUTO 12/09] Ghi chu cua bang WAuto - SINH BANG MAY tu WAutoUI/WAuto.cpp (s_aNote + s_aTips).",
           "; KHONG SUA TAY: chay lai android/sinh_ghichu_wauto.py. Chu: TCVN3. Dau || = xuong doan.",
           "; Doc boi UiWAutoDsach.cpp (nut [?] tren khung Auto).", "", "[The]"]
    for n in range(15):
        out.append("%d=%s" % (n, tcvn(note[n]) if n < len(note) else ""))
    out += ["", "[Nhan]"]
    for idc in sorted(nhan):
        if idc in tip and idc in co:
            out.append("%s=%s" % (idc, tcvn(nhan[idc])[:80]))
    out += ["", "[O]"]
    nGiu = 0
    for idc in sorted(tip):
        if idc not in co:
            continue
        out.append("%s=%s" % (idc, tcvn(tip[idc])))
        nGiu += 1
    io.open(RA, "w", encoding="latin-1", newline="\r\n").write("\n".join(out) + "\n")
    print("da ghi %s: 15 trang the, %d ghi chu o, %d nhan (bo %d o khong co tren mobile)"
          % (RA, nGiu, sum(1 for k in nhan if k in tip and k in co), len(tip) - nGiu))


if __name__ == "__main__":
    main()
