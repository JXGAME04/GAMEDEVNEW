# -*- coding: utf-8 -*-
"""t19_cauhinh_drop_quai.py - VA 1 LOI THAT + noi day cau hinh ROT DO QUAI THUONG.

Tep: script\\global\\LuaNpcMonsters\\Droprate_normal.lua
Day la duong rot do cua MOI quai thuong tren MOI ban do (gamesetting.ini
NormalDropRate -> KNpcSet.cpp:511-513 -> chay khi quai chet).

=== LOI THAT (va) ===
  :115  DropNpcMoney(NpcIndex, nPriceNormal*nXMoney)
  Bien `NpcIndex` (KHONG co tien to n) khong duoc gan o BAT KY dau trong ca cay
  script - da quet toan bo. Cac dong lan can (:103, :117, :122) deu dung dung
  `nNpcIndex`. Day la loi go thieu mot chu.
  Hau qua: Lua_ValueToNumber(L,1) tren nil tra 0, nen LuaDropNpcMoney
  (ScriptFuns.cpp) chay `Npc[0].GetMpsPos(...)` - lay VI TRI CUA PHAN TU DAU
  MANG NPC thay vi vi tri con quai vua chet. Tien duoc tao ra nhung roi o mot
  cho khac han, nguoi choi khong bao gio nhat duoc; va do la truy cap chi so 0
  khong ai bao dam hop le.
  ⚠️ BAO CHU: va xong thi quai thuong BAT DAU roi tien dung cho (nhanh nay
     chay khi random(0,30) == 2, tuc ~3,2% moi lan quai chet). Do la THEM MOT
     NGUON TIEN cho may chu. Neu chu khong muon, dat DRQ_NGUONG_TIEN = -1
     trong ch_drop.lua la nhanh do khong bao gio chay nua.

=== NOI DAY CAU HINH ===
Cac con so quyet dinh nhip rot do cua toan bo quai thuong deu nam cung trong
tep nay. Sau ban va chinh duoc o script\\cauhinh\\ch_drop.lua.

Gia tri mac dinh LAY TU CHINH dong ma (moc phai khop nguyen van dung mot lan),
va diem doc la ham SKD_CFG da co san trong lib_sukien.lua - tep nay Include
lib_sukien o dong 4 nen dung duoc ngay.

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
P = os.path.join(S, "global", "LuaNpcMonsters", "Droprate_normal.lua")
P_DROP = os.path.join(S, "cauhinh", "ch_drop.lua")
NHAN = "[CFGQUAI 29/08]"

VA = [
    # (dong_cu, dong_moi, [(khoa, giatri, mota)])
    ("\tlocal nXMoney \t\t\t= 1; -- x2 drop tien",
     '\tlocal nXMoney \t\t\t= SKD_CFG("DRQ_HESO_TIEN", 1); -- he so nhan tien roi ra',
     [("DRQ_HESO_TIEN", 1, "nhan vao so tien quai roi ra")]),

    ("\tif (nLevel < 10) then",
     '\tif (nLevel < SKD_CFG("DRQ_CAP_BAC_TOITHIEU", 10)) then',
     [("DRQ_CAP_BAC_TOITHIEU", 10, "duoi cap nay thi xep vao bac 1")]),

    ("\telseif (nLevel > 100) then ",
     '\telseif (nLevel > SKD_CFG("DRQ_CAP_BAC_TOIDA", 100)) then ',
     [("DRQ_CAP_BAC_TOIDA", 100, "tren cap nay thi boc ngau nhien bac 9-10")]),

    # --- boss xanh ---
    ("\t\tDropRateItem(nNpcIndex,8,nDropItemBoss,1,nLevel,nSeries)",
     '\t\tDropRateItem(nNpcIndex,SKD_CFG("DRQ_QUAY_BOSSXANH", 8),'
     "nDropItemBoss,1,nLevel,nSeries)",
     [("DRQ_QUAY_BOSSXANH", 8, "boss xanh: so mon roi ra moi lan chet")]),
    ("\t\tnSel = random(0,10);",
     '\t\tnSel = random(0,SKD_CFG("DRQ_MAU_BOSSXANH", 10));',
     [("DRQ_MAU_BOSSXANH", 10, "boss xanh: mau so cua phep boc tien")]),
    ("\t\tif(nSel > 8) then",
     '\t\tif(nSel > SKD_CFG("DRQ_NGUONG_TIEN_BOSSXANH", 8)) then',
     [("DRQ_NGUONG_TIEN_BOSSXANH", 8,
       "boss xanh: vuot nguong nay moi roi tien (mau 10 => ~18%)")]),

    # --- quai thuong ---
    ("\tlocal nSelT = random(0,30);",
     '\tlocal nSelT = random(0,SKD_CFG("DRQ_MAU_QUAI", 30));',
     [("DRQ_MAU_QUAI", 30, "quai thuong: mau so cua phep boc")]),
    ("\t\tif(nSelT == 2) then",
     '\t\tif(nSelT == SKD_CFG("DRQ_NGUONG_TIEN", 2)) then',
     [("DRQ_NGUONG_TIEN", 2,
       "quai thuong: trung dung so nay thi roi tien (mau 30 => ~3,2%)."
       " Dat -1 de tat han")]),
    ("\t\tDropNpcMoney(NpcIndex,nPriceNormal*nXMoney)",
     "\t\t-- " + NHAN + " VA LOI GO THIEU CHU: bien `NpcIndex` (khong co tien to"
     "\n\t\t-- n) khong duoc gan o BAT KY dau trong ca cay script. Lua tra nil,"
     "\n\t\t-- ham C nhan 0 va lay vi tri cua Npc[0] thay vi con quai vua chet,"
     "\n\t\t-- nen tien roi o cho khac han - nguoi choi khong bao gio nhat duoc."
     "\n\t\tDropNpcMoney(nNpcIndex,nPriceNormal*nXMoney)",
     []),
    ("\t\telseif (nSelT < 4) then ",
     '\t\telseif (nSelT < SKD_CFG("DRQ_NGUONG_ITEM", 4)) then ',
     [("DRQ_NGUONG_ITEM", 4,
       "quai thuong: duoi so nay thi quay bang rot do (mau 30 => 2/31 sau khi"
       " tru nhanh tien)")]),
    # thut dau dong nay la MOT tab + 4 dau cach (khong phai hai tab) - da doc
    # byte that de xac nhan, khong go theo tri nho
    ("\t    DropRateItem(nNpcIndex,1,nDropItem,1,nLevel,nSeries)",
     '\t    DropRateItem(nNpcIndex,SKD_CFG("DRQ_QUAY_QUAI", 1),'
     "nDropItem,1,nLevel,nSeries)",
     [("DRQ_QUAY_QUAI", 1, "quai thuong: so mon roi ra moi lan trung")]),

    # --- nhanh su kien ---
    ("\tlocal nSelTE = random(0,4);",
     '\tlocal nSelTE = random(0,SKD_CFG("DRQ_MAU_SUKIEN", 4));',
     [("DRQ_MAU_SUKIEN", 4, "mau so cua phep boc goi dropeventmap")]),
    ("\tif (nLevel > 7) then",
     '\tif (nLevel > SKD_CFG("DRQ_CAP_SUKIEN", 7)) then',
     [("DRQ_CAP_SUKIEN", 7, "quai tu bac nay tro len moi goi nhanh su kien")]),
    ("\t\tif(nSelTE < 2) then",
     '\t\tif(nSelTE < SKD_CFG("DRQ_NGUONG_SUKIEN", 2)) then',
     [("DRQ_NGUONG_SUKIEN", 2,
       "duoi so nay thi goi dropeventmap (mau 4 => 40%). Dat -1 de tat han")]),
]


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
    print("=== t19_cauhinh_drop_quai - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    raw = doc(P)
    if NHAN in raw:
        print("  Droprate_normal.lua DA VA - bo qua")
        return 0
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    cb0 = can_bang(raw)

    # chot: SKD_CFG phai co san (tep nay Include lib_sukien)
    if 'Include("\\\\script\\\\lib\\\\lib_sukien.lua")' not in raw:
        print("!!! LOI TO: tep khong Include lib_sukien - SKD_CFG se khong co")
        return 1

    nd = raw
    khoa = []
    for cu, moi, ds in VA:
        cu_e = cu.replace("\n", eol)
        moi_e = moi.replace("\n", eol)
        n = nd.count(cu_e)
        if n != 1:
            print("!!! LOI TO: moc khop %d lan (can 1): %r"
                  % (n, cu[:60]))
            return 1
        nd = nd.replace(cu_e, moi_e)
        for k in ds:
            khoa.append(k)
        print("  %-26s %s" % (ds[0][0] if ds else "(VA LOI NpcIndex)", cu.strip()[:44]))

    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    cb1 = can_bang(nd)
    if cb1 != cb0:
        print("!!! LOI TO: can bang tu khoa Lua doi (%d -> %d)" % (cb0, cb1))
        return 1
    if "DropNpcMoney(NpcIndex" in nd:
        print("!!! LOI TO: van con loi go thieu chu")
        return 1
    print("  => %d khoa + 1 loi da va; can bang tu khoa giu nguyen (%d)"
          % (len(khoa), cb1))

    # ---- do khoa vao ch_drop.lua ----
    raw_d = doc(P_DROP)
    if NHAN in raw_d:
        print("  ch_drop.lua DA VA - bo qua")
        nd_d = raw_d
    else:
        eol_d = "\r\n" if raw_d.count("\r\n") >= (raw_d.count("\n")
                                                 - raw_d.count("\r\n")) else "\n"
        dong = [
            "",
            "-- " + NHAN + " ROT DO QUAI THUONG"
            " (script\\global\\LuaNpcMonsters\\Droprate_normal.lua)",
            "-- Nhom nay chi phoi MOI quai thuong tren MOI ban do.",
            "-- Moi so lay tu chinh ma nguon dang chay.",
            "",
        ]
        for k, v, mota in khoa:
            dong.append("%-26s= %-6d,\t-- %s" % (k, v, mota))
        moc = "tbCFG_DROP = {"
        if raw_d.count(moc) != 1:
            print("!!! LOI TO: ch_drop.lua khong co dung 1 moc tbCFG_DROP")
            return 1
        nd_d = raw_d.replace(moc, moc + eol_d + eol_d.join(dong))
        print("  ch_drop.lua: do them %d khoa" % len(khoa))

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    for p, moi, cu in ((P, nd, raw), (P_DROP, nd_d, raw_d)):
        if moi == cu:
            continue
        sao = p + ".truoc_cfgquai"
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
