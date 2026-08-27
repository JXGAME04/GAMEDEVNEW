# -*- coding: utf-8 -*-
"""v31 - PORT KICH BAN LO REN TU CLIENT VLTK SANG JX1.

NGUON: 14 kich ban Lua + 4 bang, rut tu pak cua client VLTK
       (D:/GAMEDEVNEW/ReverseTools/pak_vltk/ra_vltk/, giai nen UCL, doc duoc 100%).
       Day la ban VIET HOA CHAY THAT, chinh xac hon ban dich nguoc tay cua toi.

DO DUOC: client VLTK va may chu Linux danh so vat pham Y HET NHAU; rieng JX1
       lech -1 o cac dai nay. Doi chieu ten:
           ptc 147 : VLTK/Linux = Huyen Tinh Khoang Thach -> JX1 146
           ptc 398 : VLTK/Linux = Than bi khoang thach    -> JX1 397
                     (JX1 398 = "Sat Thu lenh" - dung nham la hong tinh nang khac)
           ptc 1019: VLTK/Linux = Tran Bang Thach         -> JX1 1020
                     (JX1 1019 = "Hop hoa")

CACH NAN: KHONG thay so mu. Chi thay o cho DA XAC DINH NGU CANH:
       - bo ba { 6, 1, N } trong bang
       - dong co ca "nGenre == 6" lan "nDetailType == 1" roi moi den so
       Moi lan thay deu in ra ten vat pham hai ben de kiem mat.

*** Ma nao khong tra duoc ten thi BAO LOI TO va DUNG LAI - khong doan. ***
"""
import io
import os
import re
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, "D:/GAMEDEVNEW/ReverseTools/viemde")
import bangtxt

TAB = chr(9)
CR = chr(13)
NL = chr(10)

VLTK = "D:/GAMEDEVNEW/ReverseTools/pak_vltk/ra_vltk"
LNX_MS = "D:/ServerLinux/server1/settings/item/004/magicscript.txt"
JX1_MS = "D:/GAMEDEVNEW/ReverseTools/loren/ra/settings/item/magicscript.txt"
RA_LUA = "D:/GAMEDEVNEW/ReverseTools/loren/ra/lua/item"

# ten tep VLTK -> duong dan dich trong goi JX1
DICH = {
    "item_header.lua":            "item_header.lua",
    "compound_header.lua":        "compound/compound_header.lua",
    "xuanjing_compound.lua":      "compound/xuanjing_compound.lua",
    "ore_upgrade.lua":            "compound/ore_upgrade.lua",
    "magic_distill.lua":          "compound/magic_distill.lua",
    "equip_compound.lua":         "compound/equip_compound.lua",
    "equip_enchase.lua":          "compound/equip_enchase.lua",
    "atlas.lua":                  "compound/atlas.lua",
    "itemvalue_header.lua":       "itemvalue/itemvalue_header.lua",
    "magicattriblevel.lua":       "itemvalue/magicattriblevel.lua",
    "ore.lua":                    "itemvalue/ore.lua",
    "equip_normal.lua":           "itemvalue/equip_normal.lua",
    "equip_enchasable.lua":       "itemvalue/equip_enchasable.lua",
    "equip_gold.lua":             "itemvalue/equip_gold.lua",
}


def doc_bang(p):
    t = io.open(p, "rb").read().decode("latin-1")
    return [l.split(TAB) for l in t.replace(CR + NL, NL).split(NL) if l.strip()]


def chuan(s):
    return " ".join(bangtxt.tcvn2uni(s).strip().lower().split())


def moi_nhat(ten):
    """Trong ra_vltk moi duong dan co nhieu ban theo pak; lay ban pak MOI NHAT."""
    ung = [f for f in os.listdir(VLTK) if f.endswith("__" + ten)]
    if not ung:
        return None
    return os.path.join(VLTK, sorted(ung)[-1])


class Nan:
    """Nan ma particular cua genre 6 / detail 1 theo TEN vat pham."""

    def __init__(self):
        self.L = doc_bang(LNX_MS)
        self.J = doc_bang(JX1_MS)
        self.kho = {}
        for i in range(1, len(self.J)):
            r = self.J[i]
            if len(r) > 3:
                self.kho.setdefault(chuan(r[0]), []).append((r[3], i))
        self.nho = {}
        self.loi = []

    def __call__(self, p):
        p = int(p)
        if p in self.nho:
            return self.nho[p]
        dong = None
        for i in range(1, len(self.L)):
            r = self.L[i]
            if len(r) > 3 and r[1] == "6" and r[2] == "1" and r[3] == str(p):
                dong = i
                break
        if dong is None:
            self.loi.append("ptc %d khong co trong bang Linux (6/1)" % p)
            self.nho[p] = (p, "<khong tra duoc>")
            return self.nho[p]
        ten = bangtxt.tcvn2uni(self.L[dong][0]).strip()
        ung = sorted(self.kho.get(chuan(self.L[dong][0]), []),
                     key=lambda u: abs(u[1] - dong))
        if not ung:
            self.loi.append("ptc %d [%s]: JX1 khong co vat pham ten do" % (p, ten))
            self.nho[p] = (p, "<khong tra duoc>")
            return self.nho[p]
        if len(ung) > 1 and abs(ung[0][1] - dong) == abs(ung[1][1] - dong):
            self.loi.append("ptc %d [%s]: trung ten, hai ung vien cach deu" % (p, ten))
            self.nho[p] = (p, "<khong tra duoc>")
            return self.nho[p]
        self.nho[p] = (int(ung[0][0]), ten)
        return self.nho[p]


RX_BO3 = re.compile(r"\{(\s*)6(\s*),(\s*)1(\s*),(\s*)(\d+)(\s*)([,}])")
RX_SO = re.compile(r"(?<![\w.])(\d{2,4})(?![\w.])")


def nan_dong(s, nan, ghi):
    """Chi nan khi dong DA XAC DINH la noi ve genre 6 / detail 1."""
    goc = s

    def f3(m):
        p = int(m.group(6))
        moi, ten = nan(p)
        if moi != p:
            ghi.append((p, moi, ten, goc.strip()[:70]))
        return "{%s6%s,%s1%s,%s%d%s%s" % (m.group(1), m.group(2), m.group(3),
                                          m.group(4), m.group(5), moi,
                                          m.group(7), m.group(8))
    s = RX_BO3.sub(f3, s)

    co_ngucanh = ("nGenre == 6" in s or "nGenre==6" in s) and \
                 ("nDetailType == 1" in s or "nDetailType==1" in s)
    if co_ngucanh:
        def fs(m):
            p = int(m.group(1))
            if p < 2 or p > 4000:
                return m.group(0)
            moi, ten = nan(p)
            if moi != p:
                ghi.append((p, moi, ten, goc.strip()[:70]))
            return str(moi)
        # chi nan phan SAU cum dieu kien genre/detail de khong dung vao so 6 va 1
        vt = max(s.find("nDetailType == 1"), s.find("nDetailType==1"))
        dau, duoi = s[:vt + 16], s[vt + 16:]
        s = dau + RX_SO.sub(fs, duoi)
    return s


def main():
    ghi_that = "--ghi" in sys.argv
    nan = Nan()
    print("PORT KICH BAN LO REN: client VLTK -> JX1  %s"
          % ("[GHI THAT]" if ghi_that else "[DIEN TAP]"))
    print("=" * 92)
    tong = 0
    for ten_v, rel in sorted(DICH.items()):
        src = moi_nhat(ten_v)
        if src is None:
            print("  *** THIEU NGUON: %s" % ten_v)
            nan.loi.append("thieu nguon %s" % ten_v)
            continue
        t = io.open(src, "rb").read().decode("latin-1")
        eol_crlf = t.count(CR + NL) > t.count(NL) - t.count(CR + NL)
        dong = t.replace(CR + NL, NL).split(NL)
        ghi = []
        ra = [nan_dong(d, nan, ghi) for d in dong]
        print("  %-26s -> %-34s %d thay" % (ten_v, rel, len(ghi)))
        for p, moi, tn, ctx in ghi:
            print("       %4d -> %-4d  %-28s | %s" % (p, moi, tn[:28], ctx))
        tong += len(ghi)
        if ghi_that:
            p = os.path.join(RA_LUA, rel)
            d = os.path.dirname(p)
            if not os.path.isdir(d):
                os.makedirs(d)
            out = (CR + NL if eol_crlf else NL).join(ra)
            io.open(p, "wb").write(out.encode("latin-1"))

    print()
    print("  tong so lan nan ma: %d" % tong)
    if nan.loi:
        print("  *** CO %d LOI TRA MA - KHONG GHI GI CA:" % len(nan.loi))
        for x in nan.loi:
            print("      %s" % x)
        return 1
    print("  khong loi tra ma.")
    if ghi_that:
        print("  DA GHI vao %s" % RA_LUA)
    else:
        print("  (chay lai voi --ghi de ghi that)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
