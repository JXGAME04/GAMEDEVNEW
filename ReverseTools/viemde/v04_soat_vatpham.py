# -*- coding: utf-8 -*-
"""VIEM DE - buoc 4a: SOAT toan bo ma vat pham ma tinh nang dung, tra sang JX1 THEO TEN.

Luat (HUONGDAN_DICHNGUOC_TINHNANG_LINUX.md, bay #7): TUYET DOI khong dung lai id
cua ban Linux. Phai tra theo TEN. Dot 3 hoat dong da dinh 42 ma trung id khac nghia.

Ra 3 nhom:
   A. CO SAN o JX1 cung ten  -> chi can doi so
   B. JX1 KHONG CO           -> phai them vat pham moi
   C. KHONG CHAC             -> ten gan giong, can chu game duyet

Ghi ket qua ra  ReverseTools\\viemde\\_vatpham.csv  va in bang ra man hinh.
"""
import io
import os
import re
import sys
import importlib.util
import unicodedata

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

spec = importlib.util.spec_from_file_location("dec2", r"D:\GAMEDEVNEW\ReverseTools\port_3hd\dec2.py")
dec2 = importlib.util.module_from_spec(spec)
spec.loader.exec_module(dec2)

LNX = r"D:\ServerLinux\server1"
JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
FEAT = LNX + r"\script\missions\yandibaozang"

REV = {}
for cp in range(0x20, 0x2000):
    ch = chr(cp)
    try:
        b = unicode_to_tcvn3_bytes(ch)
    except Exception:
        continue
    if len(b) == 1 and b[0] >= 0x80:
        REV.setdefault(b[0], ch)


def tv(s):
    return "".join(REV.get(ord(c), c) if ord(c) >= 0x80 else c for c in s)


def chuan(s):
    s = unicodedata.normalize("NFD", s.lower())
    s = "".join(c for c in s if unicodedata.category(c) != "Mn")
    s = s.replace("\u0111", "d")
    return re.sub(r"[^a-z0-9]+", " ", s).strip()


# ---------- bang item hai ben ----------
def bang_jx1():
    """(genre,detail,particular) -> ten ; va ten_chuan -> [(g,d,p)]"""
    theo_id, theo_ten = {}, {}
    p = JX1 + r"\settings\item\magicscript.txt"
    rows = [r.rstrip("\r").split("\t") for r in io.open(p, encoding="latin-1").read().split("\n")]
    ix = {n: i for i, n in enumerate(rows[0])}
    for c in rows[1:]:
        if len(c) < 4:
            continue
        nm = tv(c[ix["Name"]])
        k = (c[ix["Genre"]], c[ix["DetailType"]], c[ix["ParticularType"]])
        theo_id[k] = nm
        theo_ten.setdefault(chuan(nm), []).append(k)
    # genre 4 nam o questkey.txt (khong co cot ParticularType)
    p = JX1 + r"\settings\item\questkey.txt"
    rows = [r.rstrip("\r").split("\t") for r in io.open(p, encoding="latin-1").read().split("\n")]
    ix = {n: i for i, n in enumerate(rows[0])}
    for c in rows[1:]:
        if len(c) < 3:
            continue
        nm = tv(c[ix["Name"]])
        k = (c[ix["Genre"]], c[ix["DetailType"]], "*")
        theo_id[k] = nm
        theo_ten.setdefault(chuan(nm), []).append(k)
    return theo_id, theo_ten


def bang_lnx():
    theo_id = {}
    p = LNX + r"\settings\item\004\magicscript.txt"
    raw = open(p, "rb").read().split(b"\n")
    for r in raw[1:]:
        c = dec2.decline2(r.rstrip(b"\r")).split("\t")
        if len(c) < 4:
            continue
        theo_id[(c[1], c[2], c[3])] = c[0].strip()
    return theo_id


# ---------- quet ma item trong kich ban ----------
RE_ITEM = re.compile(r"\{\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)")


def quet_ma():
    ra = {}
    for dp, dn, fn in os.walk(FEAT):
        for f in fn:
            if not f.endswith(".lua"):
                continue
            p = os.path.join(dp, f)
            for i, l in enumerate(open(p, "rb").read().split(b"\n"), 1):
                s = dec2.decline2(l.rstrip(b"\r"))
                if s.lstrip().startswith("--"):
                    continue
                for m in RE_ITEM.finditer(s):
                    g, d, pp = m.group(1), m.group(2), m.group(3)
                    if g not in ("0", "4", "6"):
                        continue
                    if g == "6" and d not in ("0", "1"):
                        continue
                    ra.setdefault((g, d, pp), []).append("%s:%d" % (f, i))
    return ra


def main():
    jx1_id, jx1_ten = bang_jx1()
    lnx_id = bang_lnx()
    ma = quet_ma()
    A, B, C = [], [], []
    for k in sorted(ma, key=lambda x: (int(x[0]), int(x[1]), int(x[2]))):
        g, d, pp = k
        ten_lnx = lnx_id.get(k, "")
        if not ten_lnx:
            # genre 4 / genre 0 khong nam o magicscript
            ten_lnx = ""
        cho = ", ".join(sorted(set(ma[k]))[:3])
        if not ten_lnx:
            C.append((k, "<khong tra duoc ten o ban Linux>", "", cho))
            continue
        hit = jx1_ten.get(chuan(ten_lnx))
        if hit and len(hit) == 1:
            A.append((k, ten_lnx, hit[0], cho))
        elif hit:
            # TRUNG TEN o JX1 -> KHONG duoc tu chon, day sang nhom C cho chu game duyet.
            # (bay #7: dot 3 hoat dong da trao nham vat pham vi lay bua ket qua dau tien)
            C.append((k, ten_lnx, "TRUNG %d ma: %s" % (len(hit), " | ".join(",".join(h) for h in hit)), cho))
        else:
            # thu tim ten gan giong
            gan = [t for t in jx1_ten if chuan(ten_lnx)[:14] and chuan(ten_lnx)[:14] in t]
            if gan:
                C.append((k, ten_lnx, "gan giong: " + "; ".join(
                    "%s=%s" % (",".join(jx1_ten[t][0]), t) for t in gan[:2]), cho))
            else:
                B.append((k, ten_lnx, "", cho))

    def inbang(tieu, ds):
        print()
        print("=== %s (%d) ===" % (tieu, len(ds)))
        for k, ten, jx, cho in ds:
            print("   Linux %-11s %-34s %-26s  %s" % (",".join(k), ten[:33], (",".join(jx) if isinstance(jx, tuple) else jx)[:25], cho[:44]))

    inbang("A. JX1 CO SAN cung ten - chi doi so", A)
    inbang("B. JX1 KHONG CO - phai them vat pham moi", B)
    inbang("C. CAN CHU GAME DUYET", C)

    out = r"D:\GAMEDEVNEW\ReverseTools\viemde\_vatpham.csv"
    with io.open(out, "w", encoding="utf-8", newline="") as f:
        f.write("nhom,linux_g,linux_d,linux_p,ten_linux,jx1,cho_dung\n")
        for nhom, ds in (("A", A), ("B", B), ("C", C)):
            for k, ten, jx, cho in ds:
                jxs = ",".join(jx) if isinstance(jx, tuple) else jx
                f.write('%s,%s,%s,%s,"%s","%s","%s"\n' % (nhom, k[0], k[1], k[2], ten, jxs, cho))
    print()
    print("da ghi", out)


main()
