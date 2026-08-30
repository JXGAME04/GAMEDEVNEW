# -*- coding: utf-8 -*-
"""t18_cauhinh_drop_sukien.py - NOI DAY cau hinh ROT DO SU KIEN (lib_sukien.lua).

VI SAO NHOM NAY TRUOC: `script\\lib\\lib_sukien.lua` chua cac ti le rot do dung
chung, va vai cai trong do RAT CAO ma khong ai nhin thay:
    dropeventHMD    : `if nRand > 1` tren mau 100  => 99% rot, moi lan 10 mon
    dropmanhhkmp    : `if nRand < 80` tren mau 100 => 79% rot manh do pho HKMP
    dropeventboss   : `if nRand > 1` tren mau 10   => 90% rot trang bi
    dropntiendong   : `if nRand > 1` tren mau 10   => 90% rot Tien Dong
Muon chinh, hom nay phai sua ma nguon. Sau ban va thi sua so trong
`script\\cauhinh\\ch_drop.lua`.

CACH LAM DAM BAO KHONG DOI CAN BANG:
 - Moi moc la DONG NGUYEN VAN cua tep, phai khop DUNG MOT LAN (trong pham vi
   ham neu con so do lap lai o ham khac).
 - Gia tri mac dinh dua vao ch_drop.lua duoc LAY TU CHINH DONG DO, khong go tay.
 - Diem doc la ham `SKD_CFG(khoa, macdinh)` tu dinh nghia trong lib_sukien.lua:
   neu vi ly do gi ma bo cau hinh chua nap (G_CFG == nil) thi no tra ve MAC DINH
   = so cu. Tuc kem nhat cung khong the sai.

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
P = os.path.join(S, "lib", "lib_sukien.lua")
P_DROP = os.path.join(S, "cauhinh", "ch_drop.lua")
NHAN = "[CFGDROP 29/08]"
T = "\t"

# (ten_ham hoac None = ca tep, dong_cu, dong_moi, [(khoa, giatri, mota)])
VA = [
    (None,
     "RANDMAP \t= 100\t-- Cang cao ti le cang thap",
     'RANDMAP \t= SKD_CFG("SKD_RANDMAP", 100)\t-- mau so chung; cang cao ti le cang thap',
     [("SKD_RANDMAP", 100, "mau so cho dropeventmap/HCNT/HMD/manhhkmp")]),
    (None, "RANDPLD \t= 100", 'RANDPLD \t= SKD_CFG("SKD_RANDPLD", 100)',
     [("SKD_RANDPLD", 100, "mau so nhanh Phong Lang Do (ham than rong)")]),
    (None, "RANDOTHER\t= 10", 'RANDOTHER\t= SKD_CFG("SKD_RANDOTHER", 10)',
     [("SKD_RANDOTHER", 10, "mau so cho dropeventboss / dropntiendong")]),
    (None, "RANDTK\t \t= 100", 'RANDTK\t \t= SKD_CFG("SKD_RANDTK", 100)',
     [("SKD_RANDTK", 100, "mau so nhanh Tong Kim (ham than rong)")]),
    (None, "RANDVA\t \t= 100", 'RANDVA\t \t= SKD_CFG("SKD_RANDVA", 100)',
     [("SKD_RANDVA", 100, "mau so nhanh Vuot Ai (ham than rong)")]),
    (None, "DROPMDTB\t= 95", 'DROPMDTB\t= SKD_CFG("SKD_DROPMDTB", 95)',
     [("SKD_DROPMDTB", 95, "mau so cho dropnvdt / dropnvdt01")]),

    ("dropeventmap",
     "\tif nRand > 80 then",
     '\tif nRand > SKD_CFG("SKD_MAP_NGUONG", 80) then',
     [("SKD_MAP_NGUONG", 80, "vuot nguong nay moi rot (mau 100 => con 20%)")]),
    ("dropeventmap",
     "\t DropItem(nNpcIndex,6,1,4854,0,0,0,0);\t-- Event",
     '\t DropItem(nNpcIndex,6,1,SKD_CFG("SKD_MAP_ITEM", 4854),0,0,0,0);\t-- Event',
     [("SKD_MAP_ITEM", 4854, "ma vat pham rot theo su kien tren ban do")]),

    ("droptrangbihiepcotnhutinh",
     "\tif nRand > 50 then",
     '\tif nRand > SKD_CFG("SKD_HCNT_NGUONG", 50) then',
     [("SKD_HCNT_NGUONG", 50, "Hiep Cot Nhu Tinh: vuot nguong moi rot (=> 50%)")]),
    ("droptrangbihiepcotnhutinh",
     "\tDropItemPUBG(nNpcIndex, 0, random(185,192), 0, 10, 0, 0, 2)",
     '\tDropItemPUBG(nNpcIndex, 0, random(SKD_CFG("SKD_HCNT_MA_MIN", 185),'
     ' SKD_CFG("SKD_HCNT_MA_MAX", 192)), 0, 10, 0, 0, 2)',
     [("SKD_HCNT_MA_MIN", 185, "dai ma trang bi Hiep Cot Nhu Tinh - dau"),
      ("SKD_HCNT_MA_MAX", 192, "dai ma trang bi Hiep Cot Nhu Tinh - cuoi")]),

    ("dropeventPUBG",
     "local nRand = random(4851, 4853);",
     'local nRand = random(SKD_CFG("SKD_PUBG_MA_MIN", 4851),'
     ' SKD_CFG("SKD_PUBG_MA_MAX", 4853));',
     [("SKD_PUBG_MA_MIN", 4851, "dai ma vat pham Sinh Ton - dau"),
      ("SKD_PUBG_MA_MAX", 4853, "dai ma vat pham Sinh Ton - cuoi")]),

    ("droptrangbihkmp",
     "local itemIdx =  DropItemPUBG(nNpcIndex, 0, random(0,139), 0, 10, 0, 0, 2)",
     'local itemIdx =  DropItemPUBG(nNpcIndex, 0,'
     ' random(SKD_CFG("SKD_HKMP_MA_MIN", 0), SKD_CFG("SKD_HKMP_MA_MAX", 139)),'
     " 0, 10, 0, 0, 2)",
     [("SKD_HKMP_MA_MIN", 0, "dai ma trang bi Hoang Kim Mon Phai - dau"),
      ("SKD_HKMP_MA_MAX", 139, "dai ma trang bi Hoang Kim Mon Phai - cuoi")]),
    ("droptrangbihkmp",
     "AddTimeItem(itemIdx,60*60*24*7)",
     'AddTimeItem(itemIdx,60*60*24*SKD_CFG("SKD_HKMP_HAN_NGAY", 7))',
     [("SKD_HKMP_HAN_NGAY", 7, "so ngay ton tai cua trang bi HKMP rot ra")]),

    ("dropeventHMD",
     "\tif nRand > 1 then",
     '\tif nRand > SKD_CFG("SKD_HMD_NGUONG", 1) then',
     [("SKD_HMD_NGUONG", 1, "Huy Hoang Don: vuot nguong moi rot"
                            " (mau 100 => dang la 99%)")]),
    ("dropeventHMD",
     "\tfor i=1,10 do\t",
     '\tfor i=1,SKD_CFG("SKD_HMD_SOLUONG", 10) do\t',
     [("SKD_HMD_SOLUONG", 10, "moi lan rot bao nhieu Huy Hoang Don")]),
    ("dropeventHMD",
     "\t DropItem(nNpcIndex,6,1,4844,0,0,0,0);\t-- Event",
     '\t DropItem(nNpcIndex,6,1,SKD_CFG("SKD_HMD_ITEM", 4844),0,0,0,0);\t-- Event',
     [("SKD_HMD_ITEM", 4844, "ma Huy Hoang Don")]),

    ("dropmanhhkmp",
     "\tif nRand < 80 then",
     '\tif nRand < SKD_CFG("SKD_MANHHKMP_NGUONG", 80) then',
     [("SKD_MANHHKMP_NGUONG", 80, "manh do pho HKMP: duoi nguong nay thi rot"
                                  " (mau 100 => dang la 79%)")]),

    ("dropeventboss",
     "\t\tif nRand > 1 then",
     '\t\tif nRand > SKD_CFG("SKD_BOSS_NGUONG", 1) then',
     [("SKD_BOSS_NGUONG", 1, "boss su kien: vuot nguong moi rot"
                             " (mau 10 => dang la 90%)")]),
    ("dropeventboss",
     "\t\t\tDropItem(nNpcIndex,4,random(753,770),0,0,0,0,0);\t-- ",
     '\t\t\tDropItem(nNpcIndex,4,random(SKD_CFG("SKD_BOSS_MA_MIN", 753),'
     ' SKD_CFG("SKD_BOSS_MA_MAX", 770)),0,0,0,0,0);\t-- ',
     [("SKD_BOSS_MA_MIN", 753, "dai ma trang bi boss su kien - dau"),
      ("SKD_BOSS_MA_MAX", 770, "dai ma trang bi boss su kien - cuoi")]),

    ("dropntiendong",
     "\tif nRand > 1 then",
     '\tif nRand > SKD_CFG("SKD_TIENDONG_NGUONG", 1) then',
     [("SKD_TIENDONG_NGUONG", 1, "Tien Dong: vuot nguong moi rot"
                                 " (mau 10 => dang la 90%)")]),
    ("dropntiendong",
     "local nbRand = random(1, 3);",
     'local nbRand = random(SKD_CFG("SKD_TIENDONG_SL_MIN", 1),'
     ' SKD_CFG("SKD_TIENDONG_SL_MAX", 3));',
     [("SKD_TIENDONG_SL_MIN", 1, "so Tien Dong moi lan - it nhat"),
      ("SKD_TIENDONG_SL_MAX", 3, "so Tien Dong moi lan - nhieu nhat")]),
    ("dropntiendong",
     "\tDropItem(nNpcIndex,6,1,4835,0,0,0,0);\t-- tien dong",
     '\tDropItem(nNpcIndex,6,1,SKD_CFG("SKD_TIENDONG_ITEM", 4835),0,0,0,0);'
     "\t-- tien dong",
     [("SKD_TIENDONG_ITEM", 4835, "ma Tien Dong")]),
]

HAM_CFG = """
-- %(nhan)s Bo doc cau hinh cho tep nay.
-- Vi sao co lop nay ma khong goi thang G_CFG: lib_sukien.lua duoc Include vao
-- rat nhieu state khac nhau; state nao chua nap bo cau hinh thi G_CFG la nil.
-- Ham nay tra ve MAC DINH (= dung so cu) trong truong hop do, nen kem nhat
-- cung khong the doi hanh vi.
function SKD_CFG(szKhoa, macdinh)
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


def pham_vi_ham(nd, ten_ham, eol):
    """Tra ve (i_dau, i_cuoi) cua than ham trong chuoi nd."""
    moc = "function %s(" % ten_ham
    i = nd.find(moc)
    if i < 0:
        return None
    j = nd.find(eol + "end", i)
    if j < 0:
        return None
    return i, j + len(eol) + 3


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t18_cauhinh_drop_sukien - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    raw = doc(P)
    if NHAN in raw:
        print("  lib_sukien.lua DA VA - bo qua")
        return 0
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    cb0 = can_bang(raw)

    nd = raw
    khoa = []
    for ten_ham, cu, moi, ds in VA:
        if ten_ham is None:
            n = nd.count(cu)
            if n != 1:
                print("!!! LOI TO: %-24s moc khop %d lan (can 1): %r"
                      % (ds[0][0], n, cu))
                return 1
            nd = nd.replace(cu, moi)
        else:
            pv = pham_vi_ham(nd, ten_ham, eol)
            if pv is None:
                print("!!! LOI TO: khong thay ham %s" % ten_ham)
                return 1
            a, b = pv
            than = nd[a:b]
            n = than.count(cu)
            if n != 1:
                print("!!! LOI TO: %-24s trong ham %s khop %d lan (can 1): %r"
                      % (ds[0][0], ten_ham, n, cu))
                return 1
            nd = nd[:a] + than.replace(cu, moi) + nd[b:]
        for k in ds:
            khoa.append(k)
        print("  %-24s %s" % (ds[0][0], ten_ham or "(khai bao)"))

    # chen ham SKD_CFG + Include bo cau hinh
    moc_dau = 'Include("\\\\script\\\\lib\\\\lib_vatpham.lua")'
    if nd.count(moc_dau) != 1:
        print("!!! LOI TO: khong thay dong Include lib_vatpham")
        return 1
    nd = nd.replace(moc_dau, eol.join([
        moc_dau,
        '-- ' + NHAN + ' hai tep duoi day deu la LA (khong Include gi) nen',
        '-- khong tao vong Include.',
        'Include("\\\\script\\\\cauhinh\\\\ch_lib.lua")',
        'Include("\\\\script\\\\cauhinh\\\\ch_drop.lua")',
    ]) + HAM_CFG.replace("\n", eol))

    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    cb1 = can_bang(nd)
    # Ham SKD_CFG them vao TU CAN BANG (function + then = 2, hai `end` = -2),
    # nen tong phai GIU NGUYEN. Chot nay vua bat duoc mot gia dinh sai cua toi
    # (tuong la +1) - de nguyen de lan sau khong doan bua.
    if cb1 != cb0:
        print("!!! LOI TO: can bang tu khoa Lua doi (%d -> %d)" % (cb0, cb1))
        return 1
    print("  => %d khoa; can bang tu khoa Lua giu nguyen (%d)" % (len(khoa), cb1))

    # ---- do khoa vao ch_drop.lua ----
    raw_d = doc(P_DROP)
    if NHAN in raw_d:
        print("  ch_drop.lua DA VA - bo qua")
        nd_d = raw_d
    else:
        eol_d = "\r\n" if raw_d.count("\r\n") >= (raw_d.count("\n")
                                                 - raw_d.count("\r\n")) else "\n"
        dong = [
            "-- " + NHAN + " ROT DO SU KIEN (script\\lib\\lib_sukien.lua)",
            "-- Moi so duoi day LAY TU CHINH ma nguon dang chay, nen tep nay khong",
            "-- lam doi gi cho toi khi ban sua. Sua xong KHOI DONG LAI GameServer.",
            "--",
            "-- Cach doc ti le: <nguong> so voi <mau so>. Vi du SKD_HMD_NGUONG = 1",
            "-- voi mau SKD_RANDMAP = 100 nghia la 'random(1,100) > 1' => 99%.",
            "",
        ]
        for k, v, mota in khoa:
            dong.append("%-22s= %-6d-- %s," % (k, v, mota))
        # sua lai dau phay cho dung cu phap bang Lua
        dong = [re.sub(r"^(\S+\s*=\s*\d+)(\s*)-- (.*),$", r"\1,\2-- \3", x)
                if x.startswith("SKD_") else x for x in dong]
        moc = "tbCFG_DROP = {"
        if raw_d.count(moc) != 1:
            print("!!! LOI TO: ch_drop.lua khong co dung 1 moc tbCFG_DROP")
            return 1
        nd_d = raw_d.replace(moc, moc + eol_d + eol_d.join(dong))
        print("  ch_drop.lua: do %d khoa" % len(khoa))

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    for p, moi, cu in ((P, nd, raw), (P_DROP, nd_d, raw_d)):
        if moi == cu:
            continue
        sao = p + ".truoc_cfgdrop"
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
