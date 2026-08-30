# -*- coding: utf-8 -*-
"""t10_congtac_hoatdong.py - THI CONG DOT 8: CONG TAC BAT/TAT 18 hoat dong.

PHAT HIEN (khao sat 6 nhom hoat dong, 29/08):
  TOAN BO viec bat/tat hoat dong nam o MOT cho duy nhat - ham RunTime() trong
  script\\timerserver.lua (dong 99-122). Muon bat mot hoat dong, hom nay phai
  SUA MA NGUON (bo dau `--`), va nho khoi dong lai. Khong co cach nao khac.

  Trang thai hien tai:
     DANG CHAY  : sukien_tongkim, HD3_Tick, YDBZ_Tick, CTC_JX2_Tick
     DANG TAT   : 14 hoat dong khac (deu bi comment)

MIENG VA: moi lenh goi duoc boc trong mot cong tac doc tu cau hinh.
  ⚠️ GIA TRI MAC DINH = DUNG TRANG THAI HIEN NAY:
       4 hoat dong dang chay -> mac dinh 1
       14 hoat dong dang tat -> mac dinh 0
  => Neu khong ai dong vao ch_lich.lua thi hanh vi KHONG DOI MOT LI NAO.

  Voi hoat dong dang tat, ham cua no CO THE chua duoc nap (Include tuong ung
  cung da bi go). Nen dieu kien co them `ham ~= nil`, va neu chu BAT ma ham
  khong co thi GHI LOG mot lan cho biet - thay vi im lang khong chay.

CACH DUNG SAU KHI VA: sua so trong script\\cauhinh\\ch_lich.lua roi khoi dong
lai GameServer.

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_congtac lan dau).
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
P = os.path.join(S, "timerserver.lua")
P_LICH = os.path.join(S, "cauhinh", "ch_lich.lua")
HAU_TO = ".truoc_congtac"
NHAN = "[CONGTAC 29/08]"
T = "\t"

# (khoa, mac dinh, dong CU nguyen van, ten ham, loi goi, mo ta)
CT = [
    ("BAT_LIENDAU", 0, T + "-- sukien_liendau(nHr,nDy,nMi,nMo)",
     "sukien_liendau", "sukien_liendau(nHr,nDy,nMi,nMo)", "Lien Dau"),
    ("BAT_TONGKIM", 1, T + "sukien_tongkim(nHr,nMi)",
     "sukien_tongkim", "sukien_tongkim(nHr,nMi)", "Tong Kim"),
    ("BAT_CONGTHANH_VIET", 0, T + "-- sukien_congthanh(nDyfW,nHr,nMi)",
     "sukien_congthanh", "sukien_congthanh(nDyfW,nHr,nMi)", "Cong Thanh ban Viet"),
    ("BAT_HATHUYHOANG", 0, T + "-- sukien_hathuyhoang(nHr,nMi)",
     "sukien_hathuyhoang", "sukien_hathuyhoang(nHr,nMi)", "Ha Thuy Hoang"),
    ("BAT_BOSS_HOANGKIM", 0, T + "-- sukien_bosshk(nHr,nMi)",
     "sukien_bosshk", "sukien_bosshk(nHr,nMi)", "Boss Hoang Kim"),
    ("BAT_VUOTAI_VIET", 0, T + "-- sukien_vuotai(nHr,nMi)",
     "sukien_vuotai", "sukien_vuotai(nHr,nMi)", "Vuot Ai ban Viet"),
    ("BAT_PHONGLANGDO_VIET", 0, T + "-- sukien_phonglangdo(nHr,nMi)",
     "sukien_phonglangdo", "sukien_phonglangdo(nHr,nMi)", "Phong Lang Do ban Viet"),
    ("BAT_HOADANG", 0, T + "-- sukien_trangnguyen(nHr,nMi)",
     "sukien_trangnguyen", "sukien_trangnguyen(nHr,nMi)", "Hoa Dang / Trang Nguyen"),
    ("BAT_LOIDAI_BANGHOI", 0, T + "-- sukien_loidaibanghoi(nDyfW,nHr,nMi)",
     "sukien_loidaibanghoi", "sukien_loidaibanghoi(nDyfW,nHr,nMi)",
     "Loi Dai Bang Hoi"),
    ("BAT_TRONG_BANGHOI", 0, T + "-- sukien_trongbanghoi(nHr,nMi)",
     "sukien_trongbanghoi", "sukien_trongbanghoi(nHr,nMi)", "Trong Bang Hoi"),
    ("BAT_KIEMMONQUAN", 0, T + "-- sukien_kiemmonquan(nHr,nMi)",
     "sukien_kiemmonquan", "sukien_kiemmonquan(nHr,nMi)", "Kiem Mon Quan"),
    ("BAT_VANTIEU_LOA", 0, T + "-- sukien_vantieu(nHr,nMi)",
     "sukien_vantieu", "sukien_vantieu(nHr,nMi)",
     "loa Van Tieu (NPC van chay khong can khoa nay)"),
    ("BAT_DUATOP", 0, T + "-- XepHangDuaTop()",
     "XepHangDuaTop", "XepHangDuaTop()", "Dua Top"),
    ("BAT_SONGBAC", 0, T + "-- songbac(nHr,nMi)",
     "songbac", "songbac(nHr,nMi)", "Song Bac"),
    ("BAT_LOIDAI_HONCHIEN", 0, T + "-- LoiDaiHonChien(nHr,nMi)",
     "LoiDaiHonChien", "LoiDaiHonChien(nHr,nMi)", "Loi Dai Hon Chien"),
    ("BAT_CHECK_KICK", 0,
     T + "-- check_and_kick(nMi,nSe) -- check kich tai khoan",
     "check_and_kick", "check_and_kick(nMi,nSe)", "tu kich tai khoan"),
    ("BAT_PUBG", 0, T + "-- pubg_runner(nHr,nMi)",
     "pubg_runner", "pubg_runner(nHr,nMi)", "Sinh Ton (PUBG)"),
]

# hai cho da co dang `if (X ~= nil) then` - chi them cong tac, giu nguyen kiem nil
CT_SAN = [
    ("BAT_HD3", 1,
     T + "if (HD3_Tick ~= nil) then HD3_Tick(nHr, nMi) end",
     T + "if (G_CFG(\"BAT_HD3\", 1) == 1 and HD3_Tick ~= nil) then"
     + " HD3_Tick(nHr, nMi) end",
     "cum 3 hoat dong Linux: Sat Thu / Phong Lang Do / Vuot Ai"),
    ("BAT_VIEMDE", 1,
     T + "if (YDBZ_Tick ~= nil) then YDBZ_Tick(nHr, nMi) end"
     + "\t-- [VIEMDE 26/08] lich 8h25/10h25/14h25/16h25/18h25/20h25/22h25",
     T + "if (G_CFG(\"BAT_VIEMDE\", 1) == 1 and YDBZ_Tick ~= nil) then"
     + " YDBZ_Tick(nHr, nMi) end"
     + "\t-- [VIEMDE 26/08] lich 8h25/10h25/14h25/16h25/18h25/20h25/22h25",
     "Viem De Bao Tang"),
    ("BAT_CTC_JX2", 1,
     T + "if (CTC_JX2_Tick ~= nil) then\n" + T*2 + "CTC_JX2_Tick(nDyfW, nHr, nMi)\n"
     + T + "end",
     T + "if (G_CFG(\"BAT_CTC_JX2\", 1) == 1 and CTC_JX2_Tick ~= nil) then\n"
     + T*2 + "CTC_JX2_Tick(nDyfW, nHr, nMi)\n" + T + "end",
     "Cong Thanh + Loi Dai bang hoi (ban JX2)"),
]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def can_bang(s):
    t = re.sub(r"--[^\n]*", "", s)
    t = re.sub(r'"[^"]*"', '""', t)
    t = re.sub(r"'[^']*'", "''", t)
    d = lambda w: len(re.findall(r"\b%s\b" % w, t))
    return (d("function") + d("then") + d("do") - d("elseif")) - d("end")


def khoi_moi(khoa, md, ten_ham, loi_goi, eol):
    """Sinh khoi if cho mot cong tac."""
    return eol.join([
        T + "if (G_CFG(\"%s\", %d) == 1) then" % (khoa, md),
        T*2 + "if (%s ~= nil) then" % ten_ham,
        T*3 + loi_goi,
        T*2 + "else",
        T*3 + "if (GhiLog ~= nil) then",
        T*4 + "GhiLog(\"CAUHINH\", \"%s bat nhung ham %s chua nap\")"
              % (khoa, ten_ham),
        T*3 + "end",
        T*2 + "end",
        T + "end",
    ])


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t10_congtac_hoatdong - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    raw = doc(P)
    if NHAN in raw:
        print("  timerserver.lua DA VA - bo qua")
        return 0
    if "G_CFG" not in raw:
        print("!!! LOI TO: timerserver.lua chua Include ch_lib.lua (chay t04 truoc)")
        return 1
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    cb0 = can_bang(raw)

    # CHI thay trong THAN ham RunTime. Ly do: `-- pubg_runner(nHr,nMi)` con
    # xuat hien lan nua trong ham RunTimePUBG() ngay ben duoi (dong 127) -
    # thay ca hai la sai cho.
    moc_dau = "function RunTime()"
    i_dau = raw.find(moc_dau)
    if i_dau < 0:
        print("!!! LOI TO: khong thay function RunTime()")
        return 1
    i_cuoi = raw.find(eol + "end", i_dau)
    if i_cuoi < 0:
        print("!!! LOI TO: khong thay dau dong ham RunTime")
        return 1
    i_cuoi += len(eol) + 3
    dau, than, cuoi = raw[:i_dau], raw[i_dau:i_cuoi], raw[i_cuoi:]
    print("  pham vi va: than ham RunTime (%d byte)" % len(than))

    nd = than
    n_ok = 0
    for khoa, md, dong_cu, ten_ham, loi_goi, mota in CT:
        cu = dong_cu.replace("\n", eol)
        n = nd.count(cu)
        if n != 1:
            print("!!! LOI TO: %-22s moc khop %d lan (can 1)" % (khoa, n))
            print("    moc: %r" % cu)
            return 1
        nd = nd.replace(cu, khoi_moi(khoa, md, ten_ham, loi_goi, eol))
        print("  %-22s mac dinh %d  %s" % (khoa, md, mota))
        n_ok += 1

    for khoa, md, cu0, moi0, mota in CT_SAN:
        cu = cu0.replace("\n", eol)
        moi = moi0.replace("\n", eol)
        n = nd.count(cu)
        if n != 1:
            print("!!! LOI TO: %-22s moc khop %d lan (can 1)" % (khoa, n))
            print("    moc: %r" % cu)
            return 1
        nd = nd.replace(cu, moi)
        print("  %-22s mac dinh %d  %s" % (khoa, md, mota))
        n_ok += 1

    nd = dau + nd + cuoi

    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    cb1 = can_bang(nd)
    if cb1 != cb0:
        print("!!! LOI TO: can bang tu khoa Lua doi (%d -> %d)" % (cb0, cb1))
        return 1
    print("  => %d cong tac; can bang tu khoa Lua: %d (khong doi)" % (n_ok, cb1))

    # ---- do khoa vao ch_lich.lua ----
    raw_l = doc(P_LICH)
    if NHAN in raw_l:
        print("  ch_lich.lua DA VA - bo qua")
        nd_l = raw_l
    else:
        eol_l = "\r\n" if raw_l.count("\r\n") >= (raw_l.count("\n")
                                                 - raw_l.count("\r\n")) else "\n"
        dong = [
            "-- " + NHAN + " CONG TAC BAT/TAT HOAT DONG",
            "-- 1 = bat, 0 = tat. Gia tri duoi day DUNG BANG trang thai dang chay",
            "-- hom nay, nen file nay khong lam doi gi ca cho toi khi ban sua.",
            "-- Sua xong phai KHOI DONG LAI GameServer.",
            "--",
            "-- LUU Y: hoat dong dang tat lau ngay co the chua duoc nap thu vien.",
            "-- Bat len ma khong thay chay thi xem logs\\hethong.log - se co dong",
            "-- \"<khoa> bat nhung ham <ten> chua nap\".",
            "",
        ]
        for khoa, md, _c, _h, _g, mota in CT:
            dong.append("%-22s= %d,\t-- %s" % (khoa, md, mota))
        for khoa, md, _c, _m, mota in CT_SAN:
            dong.append("%-22s= %d,\t-- %s" % (khoa, md, mota))
        moc = "tbCFG_LICH = {"
        if raw_l.count(moc) != 1:
            print("!!! LOI TO: ch_lich.lua khong co dung 1 moc tbCFG_LICH")
            return 1
        nd_l = raw_l.replace(moc, moc + eol_l + eol_l.join(dong))
        print("  ch_lich.lua: do %d khoa cong tac" % (len(CT) + len(CT_SAN)))

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    for p, moi, cu in ((P, nd, raw), (P_LICH, nd_l, raw_l)):
        if moi == cu:
            continue
        sao = p + HAU_TO
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
        with io.open(p, "wb") as f:
            f.write(moi.encode("latin-1"))
        if doc(p) != moi:
            print("!!! LOI TO: doc lai KHONG khop: %s" % p)
            return 1
        print("  DA GHI %s" % os.path.basename(p))
    print("\nCan KHOI DONG LAI GameServer (bao chu).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
