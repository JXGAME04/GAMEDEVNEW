# -*- coding: utf-8 -*-
"""v25_zuanyong_ma.py - VA bang YDBZ_ZUANYONG_ITEM con mang MA CUA BAN LINUX.

GOC (do bang doi chieu TEN, khong doan):
  Dai ma vat pham Viem De cua JX1 = ma Linux + 9:
      Linux 1604 Anh Hung Thiep      -> JX1 1613
      Linux 1605 Hinh nhan           -> JX1 1614
      Linux 1617 Viem De Lenh        -> JX1 1626   (v.v.)
  head.lua cua JX1 DA duoc nan cho ve vao cua:
      YDBZ_LIMIT_ITEM       = {{6,1,1613}, ... "Anh Hung Thiep"}  OK
      YDBZ_LIMIT_DOUBEL_ITEM= {{6,1,1626}, ... "Viem De Lenh"}    OK
  nhung BO SOT bang YDBZ_ZUANYONG_ITEM (head.lua:116-130) - van la ma Linux
  1605 / 1607..1616, va con ghi sai DetailType 0 cho 10 mon hoan (JX1 deu la 1).

TAC HAI (doc duong ma):
  include.lua:16 YDBZ_clearitem() duyet DUNG bang nay va goi
      ConsumeItem(-1, so_luong, g, d, p, -1)
  no chay trong YDBZ_restore (include.lua:42) = MOI LAN nguoi choi roi tran
  (het gio, chet bi dua ra, bam thoat, bi loai). Voi ma Linux:
      (6,1,1605) ben JX1 = "Thiep chuc su de"
      (6,0,1607) ... cac ma khac = qua/thiep su kien Viet
  => roi tran la XOA NHAM do that cua nguoi choi; dong thoi vat pham Viem De
  that (1614..1625) KHONG bi thu hoi nen mang ra ngoai duoc (sai thiet ke).

MIENG VA: nan lai ca bang theo TEN (tra magicscript.txt cua CHINH du an).
Moi mon khong tra duoc ten -> DUNG LAI, khong ghi gi (luat nan-ma 26/08).
Giu nguyen thu tu / ten hien thi; chi sua 3 so genre/detail/particular.

KHONG build. Can KHOI DONG LAI GameServer de nap (bao chu).
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_zuanyong lan dau).
"""
import io
import os
import re
import shutil
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\viemde")
from bangtxt import tcvn2uni  # noqa: E402

# console Windows la cp1252 - in ten tieng Viet se nem UnicodeEncodeError
try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

DICH = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
        r"\script\missions\yandibaozang\head.lua")
MS = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
      r"\settings\item\magicscript.txt")
HAU_TO = ".truoc_zuanyong"
NHAN = "[MA 29/08]"


def chuan(s):
    return re.sub(r"\s+", " ", s).strip().lower()


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== v25_zuanyong_ma - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    # bang tra cua DU AN: ten -> (genre, detail, particular)
    tra = {}
    for l in io.open(MS, "rb").read().decode("latin-1").replace("\r\n", "\n").split("\n")[1:]:
        c = [x.strip() for x in l.split("\t")]
        if len(c) > 3 and c[0] and c[3].isdigit():
            tra.setdefault(chuan(tcvn2uni(c[0])), (c[1], c[2], c[3]))

    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    if NHAN in raw:
        print("  DA VA - bo qua (idempotent)")
        return 0

    d = raw.split(eol)
    # xac dinh khoi YDBZ_ZUANYONG_ITEM
    bat = [i for i, l in enumerate(d) if l.strip().startswith("YDBZ_ZUANYONG_ITEM")]
    if len(bat) != 1:
        print("!!! LOI TO: thay %d khoi YDBZ_ZUANYONG_ITEM (can 1)" % len(bat))
        return 1
    ket = None
    for i in range(bat[0], min(bat[0] + 40, len(d))):
        if d[i].strip() == "}":
            ket = i
            break
    if ket is None:
        print("!!! LOI TO: khong thay dau dong '}' cua bang")
        return 1

    mau = re.compile(r'^(\s*\[\d+\]\s*=\s*\{\s*")([^"]+)("\s*,\s*)(\d+)(\s*,\s*)(\d+)(\s*,\s*)(\d+)(\s*\}.*)$')
    sua = 0
    loi = []
    for i in range(bat[0], ket + 1):
        m = mau.match(d[i])
        if not m:
            continue
        ten_uni = tcvn2uni(m.group(2))
        hit = tra.get(chuan(ten_uni))
        if hit is None:
            loi.append((i + 1, ten_uni))
            continue
        g, dt, pt = hit
        cu = (m.group(4), m.group(6), m.group(8))
        if cu == (g, dt, pt):
            continue
        d[i] = (m.group(1) + m.group(2) + m.group(3) + g + m.group(5) + dt +
                m.group(7) + pt + m.group(9))
        print("    %-28s (%s,%s,%s) -> (%s,%s,%s)"
              % (ten_uni[:28], cu[0], cu[1], cu[2], g, dt, pt))
        sua += 1

    if loi:
        print("!!! LOI TO: %d mon KHONG tra duoc ten trong magicscript - DUNG LAI:" % len(loi))
        for dong, ten in loi:
            print("    dong %-4d %s" % (dong, ten))
        return 1
    if sua == 0:
        print("  bang da dung ma - khong can sua")
        return 0
    print("  nan %d mon theo TEN (tra magicscript cua du an)" % sua)

    # chu thich dau bang
    d[bat[0]:bat[0]] = [
        "-- " + NHAN + " Ma vat pham Viem De cua JX1 = ma Linux + 9 (Anh Hung Thiep",
        "-- 1604->1613, Hinh nhan 1605->1614, Viem De Lenh 1617->1626...). Bang duoi",
        "-- truoc day con nguyen ma LINUX nen YDBZ_clearitem (include.lua:16, chay moi",
        "-- lan roi tran qua YDBZ_restore) XOA NHAM do that cua nguoi choi (1605 ben",
        "-- JX1 la 'Thiep chuc su de'...) va KHONG thu hoi vat pham Viem De that.",
        "-- Da nan lai theo TEN bang magicscript.txt cua chinh du an.",
    ]

    nd = eol.join(d)
    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    if nd.count("[1] =") != raw.count("[1] ="):
        print("!!! LOI TO: so muc trong bang thay doi")
        return 1
    print("  chot: byte cao %d (khong doi), so muc giu nguyen" % hi0)

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
        print("  sao luu -> %s" % os.path.basename(sao))
    with io.open(DICH, "wb") as f:
        f.write(nd.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI. Can KHOI DONG LAI GameServer (bao chu).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
