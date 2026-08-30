# -*- coding: utf-8 -*-
"""t06d_extern_log_ra_ngoai.py - dua 6 dong extern he log RA NGOAI vung chi-may-chu.

LOI (build client bat duoc):
  ScriptFuns.cpp(2508): error C3861: 'g_GhiLogHeThong': identifier not found
  => t06b chen khoi extern ngay TRUOC `extern int g_ExpRate;` (KCore.h:140),
  ma dong do nam TRONG `#ifdef _SERVER` (mo tai :132). Ham LuaGhiLog trong
  ScriptFuns.cpp lai bien dich cho CA client.
  Dinh nghia ham thi da nam dung cho (KCore.cpp, ngoai moi #ifdef, sau khoi
  bien exp do t05a2 chuyen ra) - chi thieu KHAI BAO ben client.

VA: chuyen khoi 6 dong len ngay TRUOC dong `#ifdef _SERVER` bao quanh no,
tuc dung canh nhom bien exp (cung da o ngoai).

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

P = r"D:\GAMEDEVNEW\Sources\Core\Src\KCore.h"
NHAN = "[HELOG3 29/08]"
MO_KHOI = "extern int  g_nGhiLogHeThong;"
KET_KHOI = r"^void g_GhiLogHeThong\("


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t06d_extern_log_ra_ngoai - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    raw = doc(P)
    if NHAN in raw:
        print("  DA VA - bo qua")
        return 0
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    d = raw.split(eol)
    hi0 = sum(1 for c in raw if ord(c) > 127)

    vt = [i for i, l in enumerate(d) if l.strip() == MO_KHOI]
    if len(vt) != 1:
        print("!!! LOI TO: tim thay %d dong mo khoi (can 1)" % len(vt))
        return 1
    i0 = vt[0]
    while i0 > 0 and d[i0 - 1].strip().startswith("//"):
        i0 -= 1
    i1 = vt[0]
    while i1 < len(d) and not re.match(KET_KHOI, d[i1]):
        i1 += 1
    if i1 >= len(d):
        print("!!! LOI TO: khong thay dong ket khoi")
        return 1
    khoi = d[i0:i1 + 1]

    k = i0
    while k > 0 and d[k].strip() != "#ifdef _SERVER":
        k -= 1
    if d[k].strip() != "#ifdef _SERVER":
        print("!!! LOI TO: khong thay vung chi-may-chu bao quanh")
        return 1

    # bo dong trong dinh kem sau khoi (neu co)
    j = i1 + 1
    if j < len(d) and d[j].strip() == "":
        j += 1

    con = d[:k] + ["// " + NHAN + " khoi nay phai nam NGOAI vung bien dich"] \
        + ["// chi-may-chu: LuaGhiLog trong ScriptFuns.cpp bien dich ca ben client."] \
        + khoi + [""] + d[k:i0] + d[j:]
    nd = eol.join(con)

    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    for t in ("#ifdef _SERVER", "#endif"):
        if nd.count(t) != raw.count(t):
            print("!!! LOI TO: so dong %r doi" % t)
            return 1
    if nd.count("void g_GhiLogHeThong(") != 1:
        print("!!! LOI TO: khai bao ham khong con duy nhat")
        return 1
    print("  chuyen %d dong ra truoc vung chi-may-chu (dong %d)" % (len(khoi), k + 1))

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    sao = P + ".truoc_externlog"
    if not os.path.isfile(sao):
        shutil.copy2(P, sao)
    with io.open(P, "wb") as f:
        f.write(nd.encode("latin-1"))
    if doc(P) != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI KCore.h")
    return 0


if __name__ == "__main__":
    sys.exit(main())
