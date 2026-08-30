# -*- coding: utf-8 -*-
"""t21_cauhinh_vantieu.py - noi day cau hinh THUONG VAN TIEU.

⚠️ DAY LA LO KINH TE LON NHAT PHAT HIEN DUOC TRONG DOT NAY. Toi KHONG tu doi
   con so nao (do la doi can bang - chu game phai chot), chi dua chung ra tep
   cau hinh de chu doi MOT DONG la xong, khong can toi sua ma.

SU THAT DA TU DOC TAN DONG (script\\event\\event_vantieu\\tieudau.lua:246-280):

    :249  for k=1,1000 do AddOwnExp(50000) end     =    50.000.000
    :250  AddSumExp(500000000)                     =   500.000.000
    :251  Msg2Player("... nhan duoc 50.000.000 kinh nghiem")   <- BAO 50 TRIEU

  Tuc phan NEN da la 550 trieu trong khi bao 50 trieu. Cong them theo loai tieu:
    tieu dong :253 +100.000.000  (bao "10.000.000")  => tong   650.000.000
    tieu bac  :262 +300.000.000  (bao "30.000.000")  => tong   850.000.000
    tieu vang :271 +500.000.000  (bao "50.000.000")  => tong 1.050.000.000

  Kem vat pham MOI CHUYEN: 300-500 Ho Mach Don, 50-100 Chan Nguyen Don,
  2-10 ruong trang bi xanh, 10 Lenh Bai Boss.

  VA NO DANG MO THAT: startgame.lua:220 goi `addnpcvantieu()` KHONG bi comment,
  nen NPC tieu dau duoc tao moi lan khoi dong may chu. (Chi phan LOA o
  timerserver.lua:128 bi comment - do la ly do de nham tuong hoat dong da tat.)

  AddSumExp di qua DirectAddExp nen KHONG chiu bat ky he so nao - chu chinh
  ExpRate hay nhom [Exp] bao nhieu cung khong cham duoc cuc exp nay.

SAU BAN VA: moi con so o script\\cauhinh\\ch_thuong.lua, khoa tien to VT_.
Mac dinh = DUNG gia tri dang chay, nen ban va nay tu no KHONG doi gi.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import re
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
P = os.path.join(S, "event", "event_vantieu", "tieudau.lua")
P_TH = os.path.join(S, "cauhinh", "ch_thuong.lua")
NHAN = "[CFGVT 29/08]"

VA = [
    ("\tfor k=1,1000 do AddOwnExp(50000) end",
     '\tfor k=1,VT_CFG("VT_EXP_NEN_LAN", 1000) do'
     ' AddOwnExp(VT_CFG("VT_EXP_NEN_MOI", 50000)) end',
     [("VT_EXP_NEN_LAN", 1000, "so lan cong exp nen"),
      ("VT_EXP_NEN_MOI", 50000, "moi lan cong bao nhieu"
                                " (1000 x 50.000 = 50 trieu)")]),
    ("\tAddSumExp(500000000)",
     '\tAddSumExp(VT_CFG("VT_EXP_NEN_SUM", 500000000))',
     [("VT_EXP_NEN_SUM", 500000000,
       "exp nen cong THEM (thong bao chi ghi 50.000.000)")]),
    ("\t\tAddSumExp(100000000)",
     '\t\tAddSumExp(VT_CFG("VT_EXP_DONG", 100000000))',
     [("VT_EXP_DONG", 100000000, "tieu dong: exp cong them"
                                 " (thong bao ghi 10.000.000)")]),
    ("\t\tAddSumExp(300000000)",
     '\t\tAddSumExp(VT_CFG("VT_EXP_BAC", 300000000))',
     [("VT_EXP_BAC", 300000000, "tieu bac: exp cong them"
                                " (thong bao ghi 30.000.000)")]),
    ("\t\tAddSumExp(500000000)",
     '\t\tAddSumExp(VT_CFG("VT_EXP_VANG", 500000000))',
     [("VT_EXP_VANG", 500000000, "tieu vang: exp cong them"
                                 " (thong bao ghi 50.000.000)")]),

    ("\t\tAddItemSL(4844,300,-2) -- ho mach don",
     '\t\tAddItemSL(4844,VT_CFG("VT_HOMACH_DONG", 300),-2) -- ho mach don',
     [("VT_HOMACH_DONG", 300, "tieu dong: so Ho Mach Don")]),
    ("\t\tAddItemSL(4847,50,-2) -- chan nguyen don",
     '\t\tAddItemSL(4847,VT_CFG("VT_CHANNGUYEN_DONG", 50),-2) -- chan nguyen don',
     [("VT_CHANNGUYEN_DONG", 50, "tieu dong: so Chan Nguyen Don")]),
    ("\t\tAddItemSL(4850,2,0) -- ruong trang bi xanh",
     '\t\tAddItemSL(4850,VT_CFG("VT_RUONG_DONG", 2),0) -- ruong trang bi xanh',
     [("VT_RUONG_DONG", 2, "tieu dong: so ruong trang bi xanh")]),

    ("\t\tAddItemSL(4844,400,-2) -- ho mach don",
     '\t\tAddItemSL(4844,VT_CFG("VT_HOMACH_BAC", 400),-2) -- ho mach don',
     [("VT_HOMACH_BAC", 400, "tieu bac: so Ho Mach Don")]),
    ("\t\tAddItemSL(4847,70,-2) -- chan nguyen don",
     '\t\tAddItemSL(4847,VT_CFG("VT_CHANNGUYEN_BAC", 70),-2) -- chan nguyen don',
     [("VT_CHANNGUYEN_BAC", 70, "tieu bac: so Chan Nguyen Don")]),
    ("\t\tAddItemSL(4850,5,0) -- ruong trang bi xanh",
     '\t\tAddItemSL(4850,VT_CFG("VT_RUONG_BAC", 5),0) -- ruong trang bi xanh',
     [("VT_RUONG_BAC", 5, "tieu bac: so ruong trang bi xanh")]),

    ("\t\tAddItemSL(4844,500,-2) -- ho mach don",
     '\t\tAddItemSL(4844,VT_CFG("VT_HOMACH_VANG", 500),-2) -- ho mach don',
     [("VT_HOMACH_VANG", 500, "tieu vang: so Ho Mach Don")]),
    ("\t\tAddItemSL(4847,100,-2) -- chan nguyen don",
     '\t\tAddItemSL(4847,VT_CFG("VT_CHANNGUYEN_VANG", 100),-2) -- chan nguyen don',
     [("VT_CHANNGUYEN_VANG", 100, "tieu vang: so Chan Nguyen Don")]),
    ("\t\tAddItemSL(4850,10,0) -- ruong trang bi xanh",
     '\t\tAddItemSL(4850,VT_CFG("VT_RUONG_VANG", 10),0) -- ruong trang bi xanh',
     [("VT_RUONG_VANG", 10, "tieu vang: so ruong trang bi xanh")]),

    ("\tAddItemSL(1023,10,-2) -- LB bosss",
     '\tAddItemSL(1023,VT_CFG("VT_LENHBAI_BOSS", 10),-2) -- LB bosss',
     [("VT_LENHBAI_BOSS", 10, "so Lenh Bai Boss moi chuyen (moi loai tieu)")]),

    ("\t\tSetTongExp(nExp + 100)",
     '\t\tSetTongExp(nExp + VT_CFG("VT_DIEM_TONG_DONG", 100))',
     [("VT_DIEM_TONG_DONG", 100, "tieu dong: diem cong cho bang hoi")]),
    ("\t\tSetTongExp(nExp + 200)",
     '\t\tSetTongExp(nExp + VT_CFG("VT_DIEM_TONG_BAC", 200))',
     [("VT_DIEM_TONG_BAC", 200, "tieu bac: diem cong cho bang hoi")]),
    ("\t\tSetTongExp(nExp + 300)",
     '\t\tSetTongExp(nExp + VT_CFG("VT_DIEM_TONG_VANG", 300))',
     [("VT_DIEM_TONG_VANG", 300, "tieu vang: diem cong cho bang hoi")]),
]

HAM = """
-- %(nhan)s Bo doc cau hinh cho tep nay. Tra ve MAC DINH (= so cu) khi bo cau
-- hinh chua nap, nen kem nhat cung khong the doi hanh vi.
function VT_CFG(szKhoa, macdinh)
\tif (G_CFG ~= nil) then
\t\treturn G_CFG(szKhoa, macdinh)
\tend
\treturn macdinh
end
""" % dict(nhan=NHAN)


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def can_bang(s):
    t = re.sub(r"--[^\n]*", "", s)
    t = re.sub(r'"[^"]*"', '""', t)
    t = re.sub(r"'[^']*'", "''", t)
    d = lambda w: len(re.findall(r"\b%s\b" % w, t))
    return (d("function") + d("then") + d("do") - d("elseif")) - d("end")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t21_cauhinh_vantieu - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    raw = doc(P)
    if NHAN in raw:
        print("  tieudau.lua DA VA - bo qua")
        return 0
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    cb0 = can_bang(raw)

    nd = raw
    khoa = []
    for cu, moi, ds in VA:
        # So khop TRON DONG (boc hai dau bang ky tu xuong dong). Neu chi so khop
        # chuoi con thi `\tAddSumExp(500000000)` se dinh ca dong `\t\tAddSumExp(
        # 500000000)` cua nhanh tieu vang - hai cho khac han nhau.
        cu_f = eol + cu + eol
        moi_f = eol + moi + eol
        n = nd.count(cu_f)
        if n != 1:
            print("!!! LOI TO: moc khop %d lan (can 1): %r" % (n, cu[:56]))
            return 1
        nd = nd.replace(cu_f, moi_f)
        for k in ds:
            khoa.append(k)
        print("  %-22s %s" % (ds[0][0], cu.strip()[:46]))

    # chen ham VT_CFG + Include ngay sau dong Include dau tien
    m = re.search(r'^Include\("[^"]+"\)', nd, re.M)
    if not m:
        print("!!! LOI TO: khong thay dong Include nao trong tieudau.lua")
        return 1
    moc_inc = m.group(0)
    nd = nd.replace(moc_inc, eol.join([
        moc_inc,
        "-- " + NHAN + " hai tep duoi day la LA (khong Include gi).",
        'Include("\\\\script\\\\cauhinh\\\\ch_lib.lua")',
        'Include("\\\\script\\\\cauhinh\\\\ch_thuong.lua")',
    ]) + HAM.replace("\n", eol), 1)

    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    cb1 = can_bang(nd)
    if cb1 != cb0:
        print("!!! LOI TO: can bang tu khoa Lua doi (%d -> %d)" % (cb0, cb1))
        return 1
    print("  => %d khoa; can bang tu khoa giu nguyen (%d)" % (len(khoa), cb1))

    raw_t = doc(P_TH)
    if NHAN in raw_t:
        print("  ch_thuong.lua DA VA - bo qua")
        nd_t = raw_t
    else:
        eol_t = "\r\n" if raw_t.count("\r\n") >= (raw_t.count("\n")
                                                 - raw_t.count("\r\n")) else "\n"
        dong = [
            "-- " + NHAN + " THUONG VAN TIEU"
            " (script\\event\\event_vantieu\\tieudau.lua)",
            "--",
            "-- !! DOC KY TRUOC KHI DOI: hien MOI CHUYEN cong that la",
            "--      tieu dong    650.000.000 exp  (thong bao ghi  60.000.000)",
            "--      tieu bac     850.000.000 exp  (thong bao ghi  80.000.000)",
            "--      tieu vang  1.050.000.000 exp  (thong bao ghi 100.000.000)",
            "--    Tran 20 luot/ngay => toi da 21 TY exp mot nguoi mot ngay.",
            "--    Exp nay di qua AddSumExp nen KHONG chiu he so nao - chinh",
            "--    ExpRate hay nhom [Exp] deu khong cham toi.",
            "--",
            "-- Cac so duoi day DUNG BANG gia tri dang chay. Doi la an ngay sau",
            "-- khi khoi dong lai GameServer.",
            "",
        ]
        for k, v, mota in khoa:
            dong.append("%-22s= %-12d,\t-- %s" % (k, v, mota))
        moc = "tbCFG_THUONG = {"
        if raw_t.count(moc) != 1:
            print("!!! LOI TO: ch_thuong.lua khong co dung 1 moc tbCFG_THUONG")
            return 1
        nd_t = raw_t.replace(moc, moc + eol_t + eol_t.join(dong))
        print("  ch_thuong.lua: do %d khoa" % len(khoa))

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    for p, moi, cu0 in ((P, nd, raw), (P_TH, nd_t, raw_t)):
        if moi == cu0:
            continue
        sao = p + ".truoc_cfgvt"
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
        with io.open(p, "wb") as f:
            f.write(moi.encode("latin-1"))
        if doc(p) != moi:
            print("!!! LOI TO: doc lai KHONG khop: %s" % p)
            return 1
        print("  DA GHI %s" % os.path.basename(p))
    return 0


if __name__ == "__main__":
    sys.exit(main())
