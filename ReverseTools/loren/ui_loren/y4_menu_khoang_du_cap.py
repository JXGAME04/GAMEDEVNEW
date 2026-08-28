# -*- coding: utf-8 -*-
"""y4_menu_khoang_du_cap.py - Them muc "Lay khoang thuoc tinh du cap" vao lenh bai.

CHU GAME: "lay nguyen lieu o lenh bai admin cac vien khoang thieu pham chat yeu
cau trong do".

DUNG. `LR_LayChung` CO Y khong phat dai khoang THUOC TINH 199..204 - chinh chu
thich trong ham ghi: "KHONG phat day 199..204 (khoang DA mang phep): ban goc
khong bao gio phat truc tiep, chung chi sinh ra qua the Trich lay / gop 3 vien".
No chi phat NGUYEN KHOANG rong (148..153) o CAP 1. Ma cong thuc Do pho lai doi
khoang thuoc tinh o cap 5..8 (vd Phuc Ma Tu Kim Con: Huyen Thiet c8, Mat Ngan
c7, Chu Sa c6, Huyen Thiet c7, Phu Dung c7).

Muc "Lay bo do pho" (LR_LayBoDoPho) DA phat dung cap - no doc cot `k.."_LEVEL"`
tu bang roi truyen vao AddItemEx. Nhung do la mot BO CO DINH theo cong thuc dau
tien con song; muon test tu do thi van thieu.

MIENG VA: them muc menu "Lay khoang thuoc tinh du cap" -> phat 6 loai khoang
thuoc tinh (199..204) o CAP 5, 6, 7, 8 (bon cap cac cong thuc dung nhieu nhat),
moi vien mot ma phep de con ep duoc. Khong dong toi LR_LayChung (giu dung hanh
vi ban goc).

CHI LA KICH BAN LUA - khong phai build.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_khoangcap lan dau).
"""
import io
import os
import shutil
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\viemde")
from bangtxt import uni2tcvn, tcvn2uni  # noqa: E402

DICH = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
        r"\script\item\test_loren_admin.lua")
HAU_TO = ".truoc_khoangcap"
T = "\t"
CHO_PHEP_HOA = set("\u0102\u00c2\u0110\u00ca\u00d4\u01a0\u01af")


def tcvn(u):
    for ch in u:
        if ord(ch) > 127 and ch.isupper() and ch not in CHO_PHEP_HOA:
            raise SystemExit("!!! chu HOA co dau: %r" % ch)
    t = uni2tcvn(u)
    if tcvn2uni(t) != u:
        raise SystemExit("!!! cham vong tron that bai: %r" % u)
    return t


TIEU_DE = tcvn(u"L\u1ea5y kho\u00e1ng thu\u1ed9c t\u00ednh \u0111\u1ee7 c\u1ea5p (5-8)")
MSG = tcvn(u"\u0110\u00e3 ph\u00e1t <color=yellow>%d<color> vi\u00ean kho\u00e1ng thu\u1ed9c t\u00ednh.\\n"
           u"S\u00e1u lo\u1ea1i 199-204, m\u1ed7i lo\u1ea1i c\u1ea5p 5, 6, 7, 8.")
NUT1 = tcvn(u"L\u1ea5y ti\u1ebfp")
NUT2 = tcvn(u"Quay l\u1ea1i")
NUT3 = tcvn(u"K\u1ebft th\u00fac")

HAM = [
    "",
    "--------------------------------------------------------------------------------",
    "-- [LOREN 28/08] Lay khoang THUOC TINH du cap.",
    "-- LR_LayChung CO Y khong phat dai 199..204 (ban goc chi sinh chung qua the",
    "-- Trich lay), nen khi test Do pho se thieu khoang dung cap. Muc nay phat",
    "-- rieng de test: 6 loai x 4 cap (5,6,7,8).",
    "--------------------------------------------------------------------------------",
    "function LR_LayKhoangCap()",
    T + "local nVer = ITEM_GetLatestItemVersion();",
    T + "local nDem = 0;",
    T + "local p;",
    T + "for p = 199, 204 do",
    T*2 + "local lv;",
    T*2 + "for lv = 5, 8 do",
    T*3 + "if( AddItemEx( nVer, 0, 0, 6, 1, p, lv, 0, 0, 0 ) > 0 ) then",
    T*4 + "nDem = nDem + 1;",
    T*3 + "end",
    T*2 + "end",
    T + "end",
    T + 'SayEx({ format( "%s", nDem ),' % MSG,
    T*2 + '"%s/LR_LayKhoangCap", "%s/LR_Root", "%s/no" })' % (NUT1, NUT2, NUT3),
    "end",
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== y4_menu_khoang_du_cap - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    if "LR_LayKhoangCap" in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    # 1) them ham: chen truoc LR_LayBoDoPho
    neo = "function LR_LayBoDoPho()"
    if raw.count(neo) != 1:
        print("!!! LOI TO: moc neo LR_LayBoDoPho khop %d lan" % raw.count(neo))
        return 1
    nd = raw.replace(neo, eol.join(HAM).replace("\n", eol) + eol + eol + neo, 1)
    print("  ok  them ham LR_LayKhoangCap (%d dong)" % len(HAM))

    # 2) them muc menu: chen sau muc goi LR_LayChung trong LR_Root
    # "/LR_LayChung" xuat hien 3 lan (menu goc + hai nut "Lay tiep"/"Lay moi").
    # Chi lay dong cua MENU GOC trong LR_Root - nhan dien bang nhan day du.
    NHAN_GOC = tcvn(u"Lấy nguyên liệu chung") + "/LR_LayChung"
    m = [l for l in nd.split(eol) if NHAN_GOC in l]
    if len(m) != 1:
        print("!!! LOI TO: dong menu goc khop %d lan" % len(m))
        return 1
    cu = m[0]
    them = cu.replace("/LR_LayChung", "/LR_LayKhoangCap")
    # doi nhan hien thi
    i1 = them.find('"')
    i2 = them.find("/", i1)
    them = them[:i1 + 1] + TIEU_DE + them[i2:]
    nd = nd.replace(cu, cu + eol + them, 1)
    print("  ok  them muc menu vao LR_Root")

    hi1 = sum(1 for c in nd if ord(c) > 127)
    print("  byte cao %d -> %d (chuoi tieng Viet moi)" % (hi0, hi1))
    try:
        nd.encode("latin-1")
    except UnicodeEncodeError as e:
        print("!!! LOI TO: ngoai latin-1: %s" % e)
        return 1
    n_fn = nd.count("function ")
    n_end = len([l for l in nd.split(eol) if l.strip() == "end"])
    print("  function=%d | dong 'end'=%d" % (n_fn, n_end))

    if not ghi:
        print("\n  --- ham se them ---")
        for l in HAM:
            try:
                print("   |%s" % tcvn2uni(l)[:96])
            except Exception:
                print("   |%s" % l[:96])
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
    with io.open(DICH, "wb") as f:
        f.write(nd.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI. Nap lai script la dung duoc (KHONG phai build).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
