# -*- coding: utf-8 -*-
"""p05_chuyen_tcvn3.py  [RELAYHT 06/09]

Doi cac tep dung tam "*.utf8.lua" (do buoc port viet ra, tieng Viet ma UTF-8) thanh
tep that "*.lua" voi CHU VIET MA TCVN3 -- dung bang ma ma game dang dung.

Vi sao phai qua mot buoc: cong cu Write ghi UTF-8, ma engine doc TCVN3; go thang
tieng Viet co dau vao tep .lua bang Write se ra chu rac trong game.

Chay:
    python p05_chuyen_tcvn3.py <thu muc goc>      # mac dinh: serverscript_relay
In ra tung tep, so ky tu Viet da doi, va canh bao neu con ky tu khong doi duoc.
Chay lai duoc nhieu lan (tep .utf8.lua giu nguyen lam ban goc).
"""
import io
import os
import sys
import unicodedata

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

SKILL = r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts"
if SKILL not in sys.path:
    sys.path.insert(0, SKILL)
from vn_to_octal import unicode_to_tcvn3_bytes  # noqa: E402

GOC_MAC_DINH = r"D:\GAMEDEVNEW_wt_relay\serverscript_relay"

# TCVN3 KHONG co nguyen am HOA co dau (chi co A~ Â Ð Ê Ô Ơ U+01AF...).
# Gap chu hoa co dau thi doi thanh chu thuong tuong ung roi bao de nguoi viet biet.
HOA_KHONG_CO = (
    "\u00c1\u00c0\u1ea2\u00c3\u1ea0"          # A co dau
    "\u1ea4\u1ea6\u1ea8\u1eaa\u1eac"
    "\u1eae\u1eb0\u1eb2\u1eb4\u1eb6"
    "\u00c9\u00c8\u1eba\u1ebc\u1eb8"          # E
    "\u1ebe\u1ec0\u1ec2\u1ec4\u1ec6"
    "\u00cd\u00cc\u1ec8\u0128\u1eca"          # I
    "\u00d3\u00d2\u1ece\u00d5\u1ecc"          # O
    "\u1ed0\u1ed2\u1ed4\u1ed6\u1ed8"
    "\u1eda\u1edc\u1ede\u1ee0\u1ee2"
    "\u00da\u00d9\u1ee6\u0168\u1ee4"          # U
    "\u1ee8\u1eea\u1eec\u1eee\u1ef0"
    "\u00dd\u1ef2\u1ef6\u1ef8\u1ef4"          # Y
)


# Dau cau kieu "chu dep" (em dash, nhay cong, ba cham...) khong co trong TCVN3.
# Doi ve ASCII truoc, KHONG de no lam hong ca tep.
THAY_DAU = {
    "—": "-", "–": "-", "−": "-",
    "“": '"', "”": '"', "„": '"',
    "‘": "'", "’": "'", "‚": "'",
    "…": "...", " ": " ", "→": "->", "⇒": "=>",
    "≤": "<=", "≥": ">=", "×": "x", "•": "*",
    "✓": "v", "✔": "v", "⚠": "!", "✅": "OK",
}


def bo_dau(s):
    """Bo dau tieng Viet -> ASCII (chi dung cho DONG CHU THICH)."""
    s = s.replace("\u0111", "d").replace("\u0110", "D")
    return "".join(c for c in unicodedata.normalize("NFD", s)
                   if unicodedata.category(c) != "Mn")


def bo_dau_chu_thich(d):
    """Chu thich -> ASCII khong dau; chuoi va ma giu nguyen de ma TCVN3.

    Ly do: chu thich chi de nguoi doc, ma TCVN3 lam no thanh chu rac khi mo bang
    trinh soan thao thuong, con chu HOA co dau thi TCVN3 khong co -> phai ha chu
    thuong, ra chu nua hoa nua thuong rat xau. Chuoi hien cho NGUOI CHOI thi bat
    buoc phai TCVN3 nen giu nguyen.
    """
    ra, trong_khoi = [], False
    for ln in d.split("\n"):
        s = ln.lstrip()
        if trong_khoi:
            ra.append(bo_dau(ln))
            if "]]" in ln:
                trong_khoi = False
            continue
        if s.startswith("--[["):
            trong_khoi = "]]" not in s
            ra.append(bo_dau(ln))
            continue
        if s.startswith("--"):
            ra.append(bo_dau(ln))
            continue
        ra.append(ln)
    return "\n".join(ra)


def chuyen(p_in, p_out):
    d = io.open(p_in, encoding="utf-8").read()
    for a, b in THAY_DAU.items():
        d = d.replace(a, b)
    d = bo_dau_chu_thich(d)

    canh_bao = []
    for c in set(d):
        if c in HOA_KHONG_CO:
            canh_bao.append(c)
    if canh_bao:
        d = d.replace("\u00c1", "\u00e1")  # cho chac chan khong con sot
        for c in canh_bao:
            d = d.replace(c, c.lower())

    raw = unicode_to_tcvn3_bytes(d)
    io.open(p_out, "wb").write(raw)

    so_viet = sum(1 for b in raw if b > 127)
    return so_viet, canh_bao


def main():
    goc = sys.argv[1] if len(sys.argv) > 1 else GOC_MAC_DINH
    n = 0
    for d, _, fs in os.walk(goc):
        for f in fs:
            if not f.lower().endswith(".utf8.lua"):
                continue
            p_in = os.path.join(d, f)
            p_out = os.path.join(d, f[: -len(".utf8.lua")] + ".lua")
            so_viet, canh_bao = chuyen(p_in, p_out)
            rel = os.path.relpath(p_out, goc)
            note = ""
            if canh_bao:
                note = "  [chu HOA co dau -> da ha thanh chu thuong: %s]" % "".join(sorted(set(canh_bao)))
            print("  %-52s %4d byte chu Viet%s" % (rel, so_viet, note))
            n += 1
    if n == 0:
        print("  (khong thay tep *.utf8.lua nao trong %s)" % goc)
    else:
        print("XONG %d tep." % n)


if __name__ == "__main__":
    main()
