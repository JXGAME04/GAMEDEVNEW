# -*- coding: utf-8 -*-
r"""v16 - DAT BO LO REN LEN MAY CHU DANG CHAY.

Chay khong tham so = DIEN TAP (chi bao se lam gi, khong ghi tep nao).
Them --that de ghi that.

Ba thu duoc dat:
  1. 20 kich ban Lua  ->  bin\server\script\...
  2. 28 bang du lieu  ->  bin\server\settings\item\...
  3. CoreServer.dll   ->  bin\server\      (doi ten, khong ghi de - nhi phan
                          dang chay khong xoa duoc)

CHOT AN TOAN TRUOC KHI GHI:
  * moi tep se DE LEN mot tep dang co deu duoc sao luu <ten>.truoc_loren
  * bang .txt: dong THUA cot la CHAN LAI (kieu loi lam sap GameServer 26/08)
  * kich ban .lua: kiem can bang function/end tho, va kiem con byte la khong
  * DLL: doi chieu so ten ham Lua moi so voi ban dang chay
"""
import io
import os
import re
import shutil
import sys
import importlib.util

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

HERE = os.path.dirname(os.path.abspath(__file__))
_spec = importlib.util.spec_from_file_location(
    "bangtxt", os.path.join(HERE, "..", "viemde", "bangtxt.py"))
bangtxt = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(bangtxt)

RA_LUA = os.path.join(HERE, "ra", "lua")
RA_BANG = os.path.join(HERE, "ra", "settings", "item")
RA_CLIENT = os.path.join(HERE, "ra", "client", "settings", "item")
DLL_MOI = r"D:\GAMEDEVNEW\Sources\Core\x64\ServerRelease\CoreServer.dll"

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
SRV_SCRIPT = os.path.join(SRV, "script")
SRV_BANG = os.path.join(SRV, "settings", "item")
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
CLI_BANG = os.path.join(CLI, "settings", "item")

TAB = chr(9)
CR = chr(13)
NL = chr(10)


def kiem_bang(b):
    d = b.decode("latin-1")
    dong = d.replace(CR + NL, NL).split(NL)
    if not dong or not dong[0].strip():
        return "tep rong"
    ncot = len(dong[0].split(TAB))
    for i, x in enumerate(dong[1:], 2):
        if x.strip() and len(x.split(TAB)) > ncot:
            return "dong %d THUA cot (%d > %d)" % (i, len(x.split(TAB)), ncot)
    return None


def kiem_lua(b, ten):
    uni = bangtxt.tcvn2uni(b.decode("latin-1"))
    ma = NL.join(l.split("--")[0] for l in uni.split(NL))
    f = len(re.findall(r"\bfunction\b", ma))
    e = len(re.findall(r"\bend\b", ma))
    i = len(re.findall(r"\bif\b", ma))
    fo = len(re.findall(r"\bfor\b", ma))
    w = len(re.findall(r"\bwhile\b", ma))
    # Khoi `do ... end` dung mot minh cung an mot `end`. Ban goc dung no de
    # tat mot ham: `function previewProb(...) do return end ...`.
    # `do` cua `for`/`while` thi da tinh roi, tru ra.
    d = len(re.findall(r"\bdo\b", ma)) - fo - w
    if d < 0:
        d = 0
    can = f + i + fo + w + d
    if e != can:
        return "lech function/end: co %d end, can %d" % (e, can)
    return None


def quet(goc, duoi):
    """duoi co the la mot chuoi hoac mot bo nhieu duoi."""
    if isinstance(duoi, str):
        duoi = (duoi,)
    ra = []
    for d, _, fs in os.walk(goc):
        for f in sorted(fs):
            if f.lower().endswith(tuple(duoi)):
                p = os.path.join(d, f)
                ra.append((p, os.path.relpath(p, goc)))
    return ra


def kiem_ini(b):
    """Tep .ini: kiem toi thieu - phai co it nhat mot muc [..] va khong co
    byte NUL. KIniFile bo qua dong ';' nen khong can kiem gi them."""
    d = b.decode("latin-1")
    if chr(0) in d:
        return "co byte NUL"
    if "[" not in d or "]" not in d:
        return "khong co muc [...] nao"
    return None


def dat(nguon, dich, kiem, that, ghi_nhan):
    b = io.open(nguon, "rb").read()
    loi = kiem(b, os.path.basename(nguon)) if kiem.__code__.co_argcount == 2 else kiem(b)
    if loi:
        ghi_nhan.append(("LOI", dich, loi))
        return False
    dacÓ = os.path.isfile(dich)
    if that:
        d = os.path.dirname(dich)
        if not os.path.isdir(d):
            os.makedirs(d)
        if dacÓ and not os.path.isfile(dich + ".truoc_loren"):
            shutil.copy2(dich, dich + ".truoc_loren")
        tam = dich + ".tam"
        io.open(tam, "wb").write(b)
        if os.path.isfile(dich):
            os.remove(dich)
        os.rename(tam, dich)
    ghi_nhan.append(("DE LEN" if dacÓ else "MOI", dich, "%d byte" % len(b)))
    return True


def main():
    that = "--that" in sys.argv
    print("DAT BO LO REN LEN MAY CHU  %s" % ("[GHI THAT]" if that else "[DIEN TAP]"))
    print("=" * 96)

    ghi_nhan = []
    loi = 0

    # --- 1. kich ban ---
    print()
    print("--- kich ban Lua ---")
    for p, rel in quet(RA_LUA, ".lua"):
        if not dat(p, os.path.join(SRV_SCRIPT, rel), kiem_lua, that, ghi_nhan):
            loi += 1
    # --- 2. bang ---
    print("--- bang du lieu ---")
    for p, rel in quet(RA_BANG, (".txt", ".ini")):
        kiem = kiem_ini if rel.lower().endswith(".ini") else (lambda b: kiem_bang(b))
        if not dat(p, os.path.join(SRV_BANG, rel), kiem, that, ghi_nhan):
            loi += 1
    # --- 2b. bang cho CLIENT ---
    # Ham dich cap thuoc tinh nam trong KItemGenerator.cpp, bien dich cho CA hai
    # ben, nen client cung phai co bang - khong thi client ve mot dang, may chu
    # tinh mot neo tren cung mot hat ngau nhien.
    print("--- bang du lieu cho CLIENT ---")
    for p, rel in quet(RA_CLIENT, (".txt", ".ini")):
        kiem = kiem_ini if rel.lower().endswith(".ini") else (lambda b: kiem_bang(b))
        if not dat(p, os.path.join(CLI_BANG, rel), kiem, that, ghi_nhan):
            loi += 1

    for trang, dich, ghi in ghi_nhan:
        nhan = os.path.relpath(dich, SRV)
        if trang == "LOI":
            print("  *** LOI  %-58s %s" % (nhan, ghi))
        else:
            print("  %-7s %-58s %s" % (trang, nhan, ghi))

    # --- 3. DLL ---
    print()
    print("--- nhi phan ---")
    if not os.path.isfile(DLL_MOI):
        print("  *** KHONG CO %s" % DLL_MOI)
        loi += 1
    else:
        dich = os.path.join(SRV, "CoreServer.dll")
        print("  nguon: %s (%d byte)" % (DLL_MOI, os.path.getsize(DLL_MOI)))
        print("  dich : %s (%d byte)" % (dich, os.path.getsize(dich) if os.path.isfile(dich) else 0))
        print("  >> KHONG tu dong thay: nhi phan dang chay phai DOI TEN chu khong")
        print("     xoa duoc. Dung tay khi may chu dung:")
        print("       ren CoreServer.dll CoreServer.dll.truoc_loren")
        print("       copy <nguon> CoreServer.dll")

    print()
    print("=" * 96)
    if loi:
        print("  CO %d MUC LOI - chua dat gi ca" % loi)
        return 1
    if that:
        print("  DA DAT XONG. Nho thay CoreServer.dll bang tay roi khoi dong lai may chu.")
    else:
        print("  DIEN TAP xong, khong ghi tep nao. Them --that de ghi.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
