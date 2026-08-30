# -*- coding: utf-8 -*-
"""t20_cauhinh_tran_baoruong.py - noi day TRAN EXP/NGAY tu bao ruong.

DAY LA "TRAN KINH TE AN" ma chu game rat de bo sot: mot nguoi choi mo bao nhieu
bao ruong di nua thi moi ngay cung chi nhan duoc toi da 50 / 80 / 100 TRIEU exp,
tuy so lan chuyen sinh. Con so nay nam trong ma nguon, khong tep cau hinh nao
nhac toi, va no ap cho CA CHIN loai ruong (Tong Kim Bi Bao, bao ruong vuot ai,
bao ruong thuy tac, Viem De mat bao, ...) qua bon noi goi:
    item\\xinshirenwu\\xinshibaoxiang.lua:2
    missions\\challengeoftime\\item\\chuangguanbaoxiang.lua:6
    missions\\fengling_ferry\\hd3_baoruongthuytac.lua:10
    missions\\yandibaozang\\item\\yandimibao.lua:5

Tep dich: script\\vng_event\\change_request_baoruong\\exp_award.lua:69-77
Sau ban va chinh o script\\cauhinh\\ch_exp.lua.

⚠️ KHONG dong toi `nMaxValue = 100` trong bang tbBitTask - do la tran cua o BIT
   (7 bit), khong phai tran exp. Doi no la hong cho luu du lieu.

Gia tri mac dinh lay tu chinh dong ma. Diem doc la ham CFG_EXP tu dinh nghia
trong tep, tra ve MAC DINH neu bo cau hinh chua nap.

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
P = os.path.join(S, "vng_event", "change_request_baoruong", "exp_award.lua")
P_EXP = os.path.join(S, "cauhinh", "ch_exp.lua")
NHAN = "[CFGTRAN 29/08]"

CU = """\tlocal nTranslife = ST_GetTransLifeCount()
\tif nTranslife <= 4 then
\t\tMAX_EXP_PER_DAY = 50
\telseif nTranslife == 5 then
\t\tMAX_EXP_PER_DAY = 80
\telse
\t\tMAX_EXP_PER_DAY = 100
\tend"""

MOI = """\tlocal nTranslife = ST_GetTransLifeCount()
\t-- %(nhan)s TRAN EXP/NGAY tu MOI bao ruong, theo so lan chuyen sinh.
\t-- Day la tran kinh te an: mo bao nhieu ruong cung khong vuot duoc so nay.
\tif nTranslife <= CFG_EXP("BRXP_MOC_CS", 4) then
\t\tMAX_EXP_PER_DAY = CFG_EXP("BRXP_TRAN_CS4", 50)
\telseif nTranslife == CFG_EXP("BRXP_MOC_CS", 4) + 1 then
\t\tMAX_EXP_PER_DAY = CFG_EXP("BRXP_TRAN_CS5", 80)
\telse
\t\tMAX_EXP_PER_DAY = CFG_EXP("BRXP_TRAN_CS6", 100)
\tend""" % dict(nhan=NHAN)

HAM = """
-- %(nhan)s Bo doc cau hinh cho tep nay. Tra ve MAC DINH (= so cu) khi bo cau
-- hinh chua nap, nen kem nhat cung khong the doi hanh vi.
function CFG_EXP(szKhoa, macdinh)
\tif (G_CFG ~= nil) then
\t\treturn G_CFG(szKhoa, macdinh)
\tend
\treturn macdinh
end
""" % dict(nhan=NHAN)

KHOA = [
    ("BRXP_MOC_CS", 4, "moc chuyen sinh: <= so nay dung tran CS4, +1 dung CS5"),
    ("BRXP_TRAN_CS4", 50, "tran exp/ngay tu MOI bao ruong (trieu) - chuyen sinh thap"),
    ("BRXP_TRAN_CS5", 80, "tran exp/ngay (trieu) - chuyen sinh dung moc+1"),
    ("BRXP_TRAN_CS6", 100, "tran exp/ngay (trieu) - chuyen sinh cao hon"),
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
    print("=== t20_cauhinh_tran_baoruong - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    raw = doc(P)
    if NHAN in raw:
        print("  exp_award.lua DA VA - bo qua")
        return 0
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    cb0 = can_bang(raw)

    cu = CU.replace("\n", eol)
    if raw.count(cu) != 1:
        print("!!! LOI TO: khoi tran exp khop %d lan (can 1)" % raw.count(cu))
        return 1
    nd = raw.replace(cu, MOI.replace("\n", eol))

    moc_inc = 'Include("\\\\script\\\\vng_lib\\\\bittask_lib.lua")'
    if nd.count(moc_inc) != 1:
        print("!!! LOI TO: khong thay dong Include bittask_lib")
        return 1
    nd = nd.replace(moc_inc, eol.join([
        moc_inc,
        "-- " + NHAN + " hai tep duoi day la LA (khong Include gi).",
        'Include("\\\\script\\\\cauhinh\\\\ch_lib.lua")',
        'Include("\\\\script\\\\cauhinh\\\\ch_exp.lua")',
    ]) + HAM.replace("\n", eol))

    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    # them 1 `elseif` (trong nhanh moc+1) => can bang giam 1? kiem that:
    cb1 = can_bang(nd)
    if cb1 != cb0:
        print("!!! LOI TO: can bang tu khoa Lua doi (%d -> %d)" % (cb0, cb1))
        return 1
    if "nMaxValue = 100" not in nd:
        print("!!! LOI TO: da dong nham vao bang bit")
        return 1
    print("  exp_award.lua: 4 khoa; can bang tu khoa giu nguyen (%d)" % cb1)

    raw_e = doc(P_EXP)
    if NHAN in raw_e:
        print("  ch_exp.lua DA VA - bo qua")
        nd_e = raw_e
    else:
        eol_e = "\r\n" if raw_e.count("\r\n") >= (raw_e.count("\n")
                                                 - raw_e.count("\r\n")) else "\n"
        dong = [
            "-- " + NHAN + " TRAN EXP/NGAY TU BAO RUONG (tran kinh te an)",
            "-- Ap cho CA CHIN loai bao ruong. Don vi: TRIEU exp.",
            "-- Nguon: script\\vng_event\\change_request_baoruong\\exp_award.lua",
            "",
        ]
        for k, v, mota in KHOA:
            dong.append("%-18s= %-5d,\t-- %s" % (k, v, mota))
        moc = "tbCFG_EXP = {"
        if raw_e.count(moc) != 1:
            print("!!! LOI TO: ch_exp.lua khong co dung 1 moc tbCFG_EXP")
            return 1
        nd_e = raw_e.replace(moc, moc + eol_e + eol_e.join(dong))
        print("  ch_exp.lua: do %d khoa" % len(KHOA))

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    for p, moi, cu0 in ((P, nd, raw), (P_EXP, nd_e, raw_e)):
        if moi == cu0:
            continue
        sao = p + ".truoc_cfgtran"
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
