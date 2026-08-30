# -*- coding: utf-8 -*-
"""t05a2_exp_ra_ngoai_ifdef.py - sua tiep t05a: dua 23 bien he so exp RA NGOAI
`#ifdef _SERVER`.

LY DO (build client bat duoc, 14 loi C2065):
  KCore.cpp:130 mo `#ifdef _SERVER`, t05a chen 23 bien vao TRONG do.
  Nhung noi DUNG chung - KPlayer::AddSelfExp KPlayer.cpp:2516-2540 - lai nam
  NGOAI moi #ifdef (ca client lan server deu bien dich). => "Server Release|x64"
  sach, con "Client Release|Win32" hong 14 loi 'undeclared identifier'.
  Day dung la bay "tep dung chung" ma so tay du an canh bao.

VA: chuyen ca khoi khai bao (KCore.cpp) va khoi extern (KCore.h) len TRUOC
`#ifdef _SERVER`. Client se dung gia tri mac dinh (= so cung cu) vi client
khong doc gamesetting.ini nhom [Exp]; server doc va ghi de - dung nhu g_ExpRate
xua nay. Khong dong toi bien nao khac.

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

CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"
HAU_TO = ".truoc_expngoai"
NHAN_CU = "[CFGEXP 29/08]"
NHAN = "[CFGEXP2 29/08]"


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def chuyen(tep, mo_dau_khoi, ket_khoi):
    """Cat khoi (tu dong chua mo_dau_khoi den dong cuoi khop ket_khoi) roi
    dat lai NGAY TRUOC dong `#ifdef _SERVER` bao quanh no."""
    raw = doc(tep)
    ten = os.path.basename(tep)
    if NHAN in raw:
        print("  %-10s DA VA - bo qua" % ten)
        return None
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    d = raw.split(eol)

    # tim khoi da chen boi t05a
    vt_dau = [i for i, l in enumerate(d) if mo_dau_khoi in l]
    if len(vt_dau) != 1:
        print("!!! LOI TO: %s - tim thay %d dong mo dau khoi (can 1)"
              % (ten, len(vt_dau)))
        return None
    i0 = vt_dau[0]
    i1 = i0
    while i1 < len(d) and not re.match(ket_khoi, d[i1]):
        i1 += 1
    if i1 >= len(d):
        print("!!! LOI TO: %s - khong thay dong cuoi khoi" % ten)
        return None

    # lui ve dau khoi: gom ca cac dong chu thich ngay truoc no
    j = i0
    while j > 0 and (d[j - 1].strip().startswith("//") or d[j - 1].strip() == ""):
        j -= 1
    khoi = d[j:i1 + 1]

    # tim `#ifdef _SERVER` gan nhat PHIA TREN khoi
    k = j
    while k > 0 and d[k].strip() != "#ifdef _SERVER":
        k -= 1
    if d[k].strip() != "#ifdef _SERVER":
        print("!!! LOI TO: %s - khong thay #ifdef _SERVER bao quanh" % ten)
        return None

    # LUU Y: chu thich duoi day TUYET DOI khong duoc chua chuoi dang duoc dem
    # ben duoi (#if...def _SERVER) - da mac loi "chot dem ca comment cua chinh
    # ban va" bon lan trong dot nay.
    con = d[:k] + ["// " + NHAN + " 23 bien he so exp phai nam NGOAI vung"] \
        + ["// bien dich chi-may-chu: KPlayer::AddSelfExp dung chung cho"] \
        + ["// ca client lan server."] \
        + khoi + [""] + d[k:j] + d[i1 + 1:]

    nd = eol.join(con)
    if sum(1 for c in nd if ord(c) > 127) != sum(1 for c in raw if ord(c) > 127):
        print("!!! LOI TO: byte cao doi (%s)" % ten)
        return None
    for t in ("#ifdef _SERVER", "#endif", "#ifndef\t_SERVER"):
        if nd.count(t) != raw.count(t):
            print("!!! LOI TO: so dong %r doi (%s)" % (t, ten))
            return None
    print("  %-10s chuyen %d dong ra truoc #ifdef _SERVER (dong %d)"
          % (ten, len(khoi), k + 1))
    return nd


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t05a2_exp_ra_ngoai_ifdef - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))

    viec = []
    a = chuyen(os.path.join(CORE, "KCore.cpp"),
               "int\t\t\t\tg_nExpMocCap1 = 50;", r"^int\s+g_nExpToDoiKhac\s*=")
    if a is None:
        return 1
    viec.append((os.path.join(CORE, "KCore.cpp"), a))

    b = chuyen(os.path.join(CORE, "KCore.h"),
               "extern int\t\t \t\tg_nExpMocCap1;", r"^extern int\s+g_nExpToDoiKhac;")
    if b is None:
        return 1
    viec.append((os.path.join(CORE, "KCore.h"), b))

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    for p, nd in viec:
        sao = p + HAU_TO
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
        with io.open(p, "wb") as f:
            f.write(nd.encode("latin-1"))
        if doc(p) != nd:
            print("!!! LOI TO: doc lai KHONG khop: %s" % p)
            return 1
        print("  DA GHI %s" % os.path.basename(p))
    print("\nBuild lai CA HAI cau hinh.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
