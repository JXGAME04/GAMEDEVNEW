# -*- coding: utf-8 -*-
"""v30 - NAN foundryresdemand.ini TU HE LINUX SANG HE JX1.

Tep goc: D:/ServerLinux/server1/settings/item/foundryresdemand.ini (7796 byte)
  53 muc dinh nghia loai nguyen lieu + 4 so do [ResScheme_1..4].
  Moi muc co the co: Quality, Genre, DetailType, PtcType, Stackable.

NGU NGHIA TRUONG VANG MAT = BO QUA (khong kiem), do duoc tu chinh du lieu:
  [Cryolite_1] khong co Stackable, ma Huyen Tinh LA vat pham xep chong
  (max 50). Neu vang mat nghia la "phai bang 0" thi nguyen lieu chinh cua ca
  he lo ren se bi chinh bo loc tu choi - vo ly. Vay vang mat = ky tu dai dien.
  Tuong tu [Equip(normal)_*] khong co PtcType = nhan moi PtcType.

NAN MA:
  Genre 6 (item_magicscript): tra PtcType theo TEN qua magicscript.txt.
  Genre 4 (item_task)       : tra DetailType theo TEN qua questkey.txt.
  Genre 0 (item_equip)      : DetailType la khe trang bi, JX1 dung y het Linux
                              (EQUIPDETAILTYPE 0..9), KHONG can nan.
  Quality                   : Linux 0/1/2/4 -> JX1 EQUIPNATURE 0/2/1/3.

*** Ma nao khong tra duoc thi BAO LOI TO va LOAI BO khoi tep ra. Giu nguyen ma
    Linux la tu tao khoa trung: do duoc 6/1/398 ben JX1 la "Sat Thu lenh",
    6/1/1019 la "Hop hoa" - hoan toan khac y nghia ben Linux. ***
"""
import io
import os
import re
import sys
import unicodedata

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, "D:/GAMEDEVNEW/ReverseTools/viemde")
import bangtxt

TAB = chr(9)
CR = chr(13)
NL = chr(10)

INI = "D:/ServerLinux/server1/settings/item/foundryresdemand.ini"
LNX_MS = "D:/ServerLinux/server1/settings/item/004/magicscript.txt"
LNX_QK = "D:/ServerLinux/server1/settings/item/004/questkey.txt"
JX1_MS = "D:/GAMEDEVNEW/ReverseTools/loren/ra/settings/item/magicscript.txt"
JX1_QK = "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/settings/item/questkey.txt"
RA = "D:/GAMEDEVNEW/ReverseTools/loren/ra/settings/item/foundryresdemand.ini"

QUALITY = {0: 0, 1: 2, 2: 1, 4: 3}      # Linux -> JX1 EQUIPNATURE
THU_TU = ("Quality", "Genre", "DetailType", "PtcType", "Stackable")


def doc(p):
    t = io.open(p, "rb").read().decode("latin-1")
    return [l.split(TAB) for l in t.replace(CR + NL, NL).split(NL) if l.strip()]


def chuan(s):
    return " ".join(bangtxt.tcvn2uni(s).strip().lower().split())


def bo_dau(s):
    s = unicodedata.normalize("NFD", s)
    s = "".join(c for c in s if unicodedata.category(c) != "Mn")
    return s.replace(chr(273), "d").replace(chr(272), "D")


def doc_ini(p):
    t = io.open(p, "rb").read().decode("latin-1")
    muc = []
    sec = None
    kv = None
    for line in t.replace(CR + NL, NL).split(NL):
        s = line.strip()
        if not s or s.startswith(";"):
            continue
        m = re.match(r"^\[(.+)\]$", s)
        if m:
            if sec:
                muc.append((sec, kv))
            sec = m.group(1)
            kv = {}
            continue
        if "=" in s and sec:
            k, v = s.split("=", 1)
            kv[k.strip()] = v.strip()
    if sec:
        muc.append((sec, kv))
    return muc


def lam_kho(rows, cot_ma):
    kho = {}
    for i in range(1, len(rows)):
        r = rows[i]
        if len(r) <= cot_ma:
            continue
        try:
            ma = int(r[cot_ma])
        except ValueError:
            continue
        kho.setdefault(chuan(r[0]), []).append((ma, i))
    return kho


def tra(kho_jx1, ma_lnx, cot_ma, rows_lnx):
    """Tim ten ben Linux theo ma, roi tra ma tuong ung ben JX1."""
    dong = None
    for i in range(1, len(rows_lnx)):
        r = rows_lnx[i]
        if len(r) <= cot_ma:
            continue
        try:
            if int(r[cot_ma]) == ma_lnx:
                dong = i
                break
        except ValueError:
            continue
    if dong is None:
        return None, "ma %d khong co trong bang Linux" % ma_lnx
    ten_goc = bangtxt.tcvn2uni(rows_lnx[dong][0]).strip()
    ung = kho_jx1.get(chuan(rows_lnx[dong][0]), [])
    if not ung:
        return None, "JX1 khong co vat pham ten [%s]" % ten_goc
    ung = sorted(ung, key=lambda u: abs(u[1] - dong))
    if len(ung) > 1 and abs(ung[0][1] - dong) == abs(ung[1][1] - dong):
        return None, "trung ten [%s], hai ung vien cach deu" % ten_goc
    return ung[0][0], ten_goc


def main():
    ghi = "--ghi" in sys.argv
    muc = doc_ini(INI)
    LMS = doc(LNX_MS)
    JMS = doc(JX1_MS)
    LQK = doc(LNX_QK)
    JQK = doc(JX1_QK)
    kho_ms = lam_kho(JMS, 3)        # cot 4 = ParticularType
    kho_qk = lam_kho(JQK, 2)        # cot 3 = DetailType

    print("NAN foundryresdemand.ini  Linux -> JX1  %s"
          % ("[GHI THAT]" if ghi else "[DIEN TAP]"))
    print("=" * 92)

    ra = []
    hong = []
    for ten, kv in muc:
        if ten.startswith("ResScheme"):
            ra.append((ten, kv, None))
            continue
        g = int(kv.get("Genre", "-1"))
        moi = dict(kv)
        ghichu = ""
        if "Quality" in kv:
            q = int(kv["Quality"])
            if q not in QUALITY:
                hong.append((ten, "Quality=%d khong co trong bang doi" % q))
                continue
            moi["Quality"] = str(QUALITY[q])
        if g == 6 and "PtcType" in kv:
            p = int(kv["PtcType"])
            m, gc = tra(kho_ms, p, 3, LMS)
            if m is None:
                hong.append((ten, "Genre6 PtcType=%d: %s" % (p, gc)))
                continue
            moi["PtcType"] = str(m)
            ghichu = "%s  (Linux %d -> JX1 %d)" % (bo_dau(gc), p, m)
        elif g == 4 and "DetailType" in kv:
            d = int(kv["DetailType"])
            m, gc = tra(kho_qk, d, 2, LQK)
            if m is None:
                hong.append((ten, "Genre4 DetailType=%d: %s" % (d, gc)))
                continue
            moi["DetailType"] = str(m)
            ghichu = "%s  (Linux %d -> JX1 %d)" % (bo_dau(gc), d, m)
        elif g == 0:
            ghichu = "khe trang bi, JX1 dung so giong Linux"
        ra.append((ten, moi, ghichu))

    n_muc = len([x for x in ra if not x[0].startswith("ResScheme")])
    print("  muc dinh nghia tra duoc : %d" % n_muc)
    print("  muc KHONG tra duoc      : %d" % len(hong))
    if hong:
        print()
        print("  --- KHONG TRA DUOC (LOAI BO, khong giu ma Linux) ---")
        for a, b in hong:
            print("    %-24s %s" % (a, b))
    print()
    print("  --- MUC DA DOI MA ---")
    for ten, kv, gc in ra:
        if gc and "->" in gc:
            print("    %-22s %-42s %s" % (ten,
                  " ".join("%s=%s" % (k, kv[k]) for k in THU_TU if k in kv),
                  gc[:44]))

    if ghi:
        out = []
        out.append("; foundryresdemand.ini - dieu kien nguyen lieu cua he lo ren")
        out.append("; NAN MA tu ban Linux sang he JX1 bang v30_nan_ini_nguyenlieu.py")
        out.append("; Truong VANG MAT = khong kiem (ky tu dai dien).")
        out.append("; Quality theo EQUIPNATURE cua JX1:")
        out.append(";   0 = thuong   1 = tim (kham nam duoc)   2 = hoang kim   3 = bach kim")
        out.append("")
        for ten, kv, gc in ra:
            if gc:
                out.append("; " + gc)
            out.append("[%s]" % ten)
            for k in THU_TU:
                if k in kv:
                    out.append("%s=%s" % (k, kv[k]))
            for k in kv:
                if k not in THU_TU:
                    out.append("%s=%s" % (k, kv[k]))
            out.append("")
        d = os.path.dirname(RA)
        if not os.path.isdir(d):
            os.makedirs(d)
        io.open(RA, "wb").write((CR + NL).join(out).encode("latin-1"))
        print()
        print("  DA GHI %s (%d dong)" % (RA, len(out)))
    else:
        print()
        print("  (chay lai voi --ghi de ghi that)")
    return 1 if hong else 0


if __name__ == "__main__":
    raise SystemExit(main())
