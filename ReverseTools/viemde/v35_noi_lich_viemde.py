# -*- coding: utf-8 -*-
r"""v35 - noi LICH CHAY Viem De vao dong ho GameServer (buoc 8 con dang do).

Tep ydbz_driver.lua da nam san o cay may chu nhung KHONG AI GOI, nen Viem De
khong bao gio khoi dong. Ban va nay nhan ban dung khuon ma dot 3HD da dung:

    startgame.lua   : Include(...ydbz_driver.lua)  +  YDBZ_DriverInit()
    timerserver.lua : Include(...ydbz_driver.lua)  +  YDBZ_Tick(nHr, nMi)

Giu nguyen loi phong thu cua khuon cu: goi qua chot `if (X ~= nil)` de neu tep
driver chua nap thi may chu KHONG no.

LUU Y: startgame.lua von co san 5 dong LF le - phep kiem chi doi hoi KHONG TANG.

Chay:  python v35_noi_lich_viemde.py          -> chi xem
       python v35_noi_lich_viemde.py --that   -> lam that
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

LIVE = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
KHO = r"D:\GAMEDEVNEW\serverscript_jx2\viemde\script"
HAU_TO = ".truoc_lich_viemde_2608"

F_SG = os.path.join(LIVE, "startgame.lua")
F_TS = os.path.join(LIVE, "timerserver.lua")

INC = 'Include("\\\\script\\\\tinhnang\\\\viemde\\\\ydbz_driver.lua")'

VA = [
    (F_SG,
     'Include("\\\\script\\\\tinhnang\\\\3hoatdong\\\\hd3_driver.lua")',
     None,   # dien sau: giu nguyen ca dong roi them dong moi phia duoi
     "startgame.lua: Include ydbz_driver.lua"),
    (F_SG,
     "\tHD3_DriverInit()",
     None,
     "startgame.lua: goi YDBZ_DriverInit()"),
    (F_TS,
     'Include("\\\\script\\\\tinhnang\\\\3hoatdong\\\\hd3_driver.lua")',
     None,
     "timerserver.lua: Include ydbz_driver.lua"),
    (F_TS,
     "\tif (HD3_Tick ~= nil) then HD3_Tick(nHr, nMi) end",
     None,
     "timerserver.lua: goi YDBZ_Tick(nHr, nMi)"),
]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def ghi(p, s):
    bak = p + HAU_TO
    if not os.path.isfile(bak):
        shutil.copy2(p, bak)
    b = s.encode("latin-1")
    goc = io.open(bak, "rb").read()
    if (b.count(b"\n") - b.count(b"\r\n")) > (goc.count(b"\n") - goc.count(b"\r\n")):
        raise SystemExit("!! %s: sinh them dong LF le" % p)
    tmp = p + ".tmp"
    io.open(tmp, "wb").write(b)
    os.replace(tmp, p)


def dong_chua(d, moc):
    """tra ve NGUYEN dong (khong ke ket dong) chua moc, hoac None"""
    i = d.find(moc)
    if i < 0:
        return None
    dau = d.rfind("\r\n", 0, i)
    dau = 0 if dau < 0 else dau + 2
    cuoi = d.find("\r\n", i)
    if cuoi < 0:
        return None
    return d[dau:cuoi]


def main():
    that = "--that" in sys.argv
    print("=" * 72)
    print("  %s" % ("NOI THAT" if that else "CHI XEM TRUOC (them --that de lam that)"))
    print("=" * 72)

    ke = []
    for p, moc, _, nhan in VA:
        if not os.path.isfile(p):
            print("!! khong co tep:", p)
            return 2
        d = doc(p)
        if "ydbz_driver.lua" in d and "Include" in moc:
            print("   = da co san:", nhan)
            continue
        if ("YDBZ_DriverInit()" in d and "DriverInit" in nhan) or \
           ("YDBZ_Tick(nHr" in d and "YDBZ_Tick" in nhan):
            print("   = da co san:", nhan)
            continue
        dong = dong_chua(d, moc)
        if dong is None:
            print("!! khong tim thay dong chua moc: %s" % nhan)
            return 2
        if d.count(dong + "\r\n") != 1:
            print("!! dong moc khong duy nhat: %s" % nhan)
            return 2

        if "Include" in moc:
            moi = (dong + "\r\n" + INC
                   + "\t-- [VIEMDE 26/08] boot + lich Viem De Bao Tang (thay S3Relay)\r\n")
        elif "DriverInit" in moc:
            moi = (dong + "\r\n"
                   + "\tif (YDBZ_DriverInit ~= nil) then YDBZ_DriverInit() end"
                   + "\t-- [VIEMDE 26/08] dat NPC bao danh\r\n")
        else:
            moi = (dong + "\r\n"
                   + "\tif (YDBZ_Tick ~= nil) then YDBZ_Tick(nHr, nMi) end"
                   + "\t-- [VIEMDE 26/08] lich 8h25/10h25/14h25/16h25/18h25/20h25/22h25\r\n")
        ke.append((p, dong + "\r\n", moi, nhan))
        print("   > se them: %s" % nhan)

    if not ke:
        print("\n(khong con gi de lam)")
        return 0
    if not that:
        print("\n(chua lam gi ca)")
        return 0

    print()
    theo = {}
    for p, cu, moi, nhan in ke:
        theo.setdefault(p, []).append((cu, moi, nhan))
    for p, ds in theo.items():
        d = doc(p)
        for cu, moi, nhan in ds:
            d = d.replace(cu, moi, 1)
            print("   > " + nhan)
        ghi(p, d)

    # luu ban sao vao kho de con dau vet trong repo (giong cac dot port truoc)
    os.makedirs(KHO, exist_ok=True)
    for p in (F_SG, F_TS):
        shutil.copy2(p, os.path.join(KHO, os.path.basename(p)))
    print("\n   da luu ban sao vao %s" % KHO)

    print()
    for p, ten in ((F_SG, "YDBZ_DriverInit"), (F_TS, "YDBZ_Tick")):
        d = doc(p)
        print("   %-18s Include: %-4s  goi %s: %s"
              % (os.path.basename(p),
                 "CO" if "ydbz_driver.lua" in d else "KHONG",
                 ten, "CO" if ten in d else "KHONG"))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
