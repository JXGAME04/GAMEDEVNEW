# -*- coding: utf-8 -*-
"""t34_cauhinh_loidai_honchien.py - dua cac con so LOI DAI HON CHIEN ra cau hinh.

Hoat dong nay DANG TAT (BAT_LOIDAI_HONCHIEN = 0). Dua so ra cau hinh de chu game
nhin thay va chinh duoc TRUOC khi bat.

DA TU DOC VA XAC NHAN (script\\timerserver.lua):
  :469  moc khai chien: gio 16 hoac 22, phut 52
  :481  MOI NGUOI dang dung o ban do 210 luc do duoc AddSumExp(50000000)
  :482  ... cong them AddItemSL(4844,100,0) = 100 Ho Mach Don
        -> chi can CO MAT, khong can danh ai
  :485  cong 20 diem vao TASK_DSK
  :497  cua so chot quan quan: phut 12..54  (mo TRUOC moc khai chien 40 phut)
  :511  dieu kien thang: `count == 1` - mot nguoi bao danh don la thang
  :535  quan quan duoc AddSumExp(500000000)
  :536  ... cong 5 manh hoang kim random(753,770)

TOI CHI DUA SO RA CAU HINH, KHONG SUA LOGIC. Nhung diem duoi day la loi THIET KE
chu khong phai con so, nen phai chu game quyet truoc khi bat:
  (a) cua so chot mo truoc khai chien 40 phut
  (b) `count == 1` khong doi hoi danh nhau
  (c) tran 4 mang bi xoa boi SetTaskTemp(1,0) o dong 479 moi lan khai chien
  (d) khi chet KHONG do nguoi choi thi exp roi vao chinh nguoi vua chet
      (bigiet.lua:6-8)
  (e) "tong dame" dung de xep hang la so gia - khong noi nao ghi SetTaskTemp(50)

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
P = os.path.join(S, "timerserver.lua")
P_THUONG = os.path.join(S, "cauhinh", "ch_thuong.lua")
NHAN = "[CFGLDHC 30/08]"

# (dong_cu_nguyen_van, dong_moi, [(khoa, gia_tri, mo_ta)])
VA = [
    ("\t\t\t\t\tAddSumExp(50000000)",
     '\t\t\t\t\tAddSumExp(LDHC_CFG("LDHC_EXP_COMAT", 50000000))',
     [("LDHC_EXP_COMAT", 50000000,
       "exp cho MOI NGUOI dang dung o ban do 210 luc khai chien."
       " !! chi can co mat, khong can danh ai")]),
    ("\t\t\t\t\tAddItemSL(4844,100,0)",
     '\t\t\t\t\tAddItemSL(4844,LDHC_CFG("LDHC_SL_HOMACH_COMAT", 100),0)',
     [("LDHC_SL_HOMACH_COMAT", 100,
       "so Ho Mach Don cho moi nguoi co mat luc khai chien")]),
    ("\t\t\t\tAddSumExp(500000000)",
     '\t\t\t\tAddSumExp(LDHC_CFG("LDHC_EXP_QUANQUAN", 500000000))',
     [("LDHC_EXP_QUANQUAN", 500000000,
       "exp cho quan quan. !! dieu kien thang hien la `count == 1`,"
       " tuc mot nguoi bao danh don cung duoc")]),
    ("\t\t\t\tfor i=1,5 do",
     '\t\t\t\tfor i=1,LDHC_CFG("LDHC_SL_MANH_HOANGKIM", 5) do',
     [("LDHC_SL_MANH_HOANGKIM", 5,
       "so manh hoang kim cho quan quan")]),
]

HAM = """
-- %(nhan)s Bo doc cau hinh cho tep nay (rieng phan Loi Dai Hon Chien).
-- Tra ve MAC DINH (= so cu) khi bo cau hinh chua nap.
function LDHC_CFG(szKhoa, macdinh)
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
    print("=== t34_cauhinh_loidai_honchien - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    raw = doc(P)
    if NHAN in raw:
        print("  timerserver.lua DA VA - bo qua")
        return 0
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    cb0 = can_bang(raw)

    nd = raw
    khoa = []
    for cu, moi, ds in VA:
        cu_f = eol + cu + eol
        n = nd.count(cu_f)
        if n != 1:
            print("!!! LOI TO: moc khop %d lan (can 1): %r" % (n, cu.strip()[:56]))
            return 1
        nd = nd.replace(cu_f, eol + moi + eol)
        khoa.extend(ds)
        print("  %-26s %s" % (ds[0][0], cu.strip()[:48]))

    # chen ham LDHC_CFG sau dong Include dau tien
    m = re.search(r'^Include\("[^"]+"\)', nd, re.M)
    if not m:
        print("!!! LOI TO: khong thay dong Include nao")
        return 1
    nd = nd.replace(m.group(0), m.group(0) + HAM.replace("\n", eol), 1)

    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte tieng Viet doi")
        return 1
    cb1 = can_bang(nd)
    if cb1 != cb0:
        print("!!! LOI TO: can bang tu khoa Lua doi (%d -> %d)" % (cb0, cb1))
        return 1
    print("  => %d khoa; can bang tu khoa giu nguyen (%d)" % (len(khoa), cb1))

    raw_t = doc(P_THUONG)
    eol_t = "\r\n" if raw_t.count("\r\n") >= (raw_t.count("\n")
                                             - raw_t.count("\r\n")) else "\n"
    con_thieu = [x for x in khoa
                 if not re.search(r"^\s*%s\s*=" % re.escape(x[0]), raw_t, re.M)]
    if not con_thieu:
        print("  ch_thuong.lua: cac khoa deu da co")
        nd_t = raw_t
    else:
        dong = [
            "",
            "-- " + NHAN + " LOI DAI HON CHIEN (script\\timerserver.lua)"
            " - DANG TAT",
            "--",
            "-- !! Doc ky truoc khi bat. Ngoai cac so duoi day, hoat dong nay con",
            "-- !! nam diem ho ve THIET KE (khong phai con so) da ghi trong",
            "-- !! BAOCAO_LOHONG_2908.md - phai xu ly truoc khi bat.",
            "",
        ]
        for k, v, mo in con_thieu:
            dong.append("%-26s= %-12d,\t-- %s" % (k, v, mo))
        moc = "tbCFG_THUONG = {"
        if raw_t.count(moc) != 1:
            print("!!! LOI TO: ch_thuong.lua khong co dung mot moc")
            return 1
        nd_t = raw_t.replace(moc, moc + eol_t + eol_t.join(dong))
        print("  ch_thuong.lua: do %d khoa" % len(con_thieu))

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    for p, moi, cu0 in ((P, nd, raw), (P_THUONG, nd_t, raw_t)):
        if moi == cu0:
            continue
        sao = p + ".truoc_cfgldhc"
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
