# -*- coding: utf-8 -*-
"""t04_nhip_nap_lai.py - THI CONG DOT 5: cho chinh NHIP tu nap lai timerserver.

HIEN TRANG (da doc tan ma):
  CoreServerShell.cpp:1140  pTimeScript = g_GetScript("\\script\\timerserver.lua")
  CoreServerShell.cpp:1165-1171  goi RunTime() MOI PHUT
  script\\timerserver.lua:40      dofile("script/timerserver.lua")   <- TU NAP LAI

  Tuc la moi phut may chu nap lai chinh tep do, keo theo CA 33 dong Include o
  dau tep (~103 tep, ~0,97 MB doc dia + bien dich Lua). Doi lai duoc mot thu
  that su co ich: sua script la an ngay, khong phai restart.

VA: giu nguyen kha nang do, nhung cho CHINH NHIP bang cau hinh.
    ch_chung.lua khoa CH_NAPLAI_PHUT:
       1 = y het hien nay (MAC DINH - khong doi gi ca)
       5 = nap lai 5 phut/lan  (nhe hon 5 lan)
       0 = tat han (nhe nhat; sua script phai restart moi an)

    Cach nhan biet moc: RunTime() chay moi phut, nen lay (gio*60 + phut) chia
    du cho nhip. Khong can bien dem - bien dem se mat sau moi lan dofile.

⚠️ KHONG dung toi bat ky dong nao khac cua timerserver.lua.

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_nhipnap lan dau).
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

P = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
     r"\script\timerserver.lua")
HAU_TO = ".truoc_nhipnap"
NHAN = "[NHIPNAP 29/08]"
T = "\t"

# 1) them 2 Include (LA, khong keo theo gi) ngay truoc `function RunTime()`
MOC_INC = "function RunTime()"
THEM_INC = [
    "-- " + NHAN + " bo doc cau hinh (hai tep nay deu la LA, khong Include gi)",
    'Include("\\\\script\\\\cauhinh\\\\ch_lib.lua")',
    'Include("\\\\script\\\\cauhinh\\\\ch_chung.lua")',
    "",
    "function RunTime()",
]

# 2) boc dong dofile
CU_DOFILE = [
    T + " dofile(\"script/timerserver.lua\")",
]
MOI_DOFILE = [
    T + "-- " + NHAN + " NHIP TU NAP LAI. Dong dofile duoi day nap lai CHINH tep",
    T + "-- nay moi phut, keo theo ca 33 Include o dau tep (~103 tep, ~0,97 MB).",
    T + "-- Doi lai: sua script an ngay, khong can restart.",
    T + "-- CH_NAPLAI_PHUT o script\\cauhinh\\ch_chung.lua:",
    T + "--    1 = y het truoc day (mac dinh) | 5 = 5 phut/lan | 0 = tat han",
    T + "local nNhipNap = 1",
    T + "if (G_CFG ~= nil) then",
    T*2 + "nNhipNap = G_CFG(\"CH_NAPLAI_PHUT\", 1)",
    T + "end",
    T + "if (nNhipNap ~= nil and nNhipNap > 0) then",
    T*2 + "local nYrN, nMoN, nDyN, nHrN, nMiN = GetTimeNow()",
    T*2 + "-- RunTime chay moi phut nen chi can chia du theo phut trong ngay;",
    T*2 + "-- KHONG dung bien dem vi bien se mat sau moi lan dofile.",
    T*2 + "if (nNhipNap <= 1 or mod(nHrN * 60 + nMiN, nNhipNap) == 0) then",
    T*3 + "dofile(\"script/timerserver.lua\")",
    T*2 + "end",
    T + "end",
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
    print("=== t04_nhip_nap_lai - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    if not os.path.isfile(P):
        print("!!! LOI TO: thieu %s" % P)
        return 1
    raw = doc(P)
    if NHAN in raw:
        print("  DA VA - bo qua")
        return 0
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    cb0 = can_bang(raw)

    nd = raw
    n = nd.count(MOC_INC)
    if n != 1:
        print("!!! LOI TO: `%s` khop %d lan (can 1)" % (MOC_INC, n))
        return 1
    nd = nd.replace(MOC_INC, eol.join(THEM_INC))
    print("  them 2 Include bo doc cau hinh")

    kcu = eol.join(CU_DOFILE)
    n = nd.count(kcu)
    if n != 1:
        print("!!! LOI TO: dong dofile khop %d lan (can 1)" % n)
        print("    moc: %r" % kcu)
        return 1
    nd = nd.replace(kcu, eol.join(MOI_DOFILE))
    print("  boc dong dofile bang nhip CH_NAPLAI_PHUT")

    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    cb1 = can_bang(nd)
    if cb1 != cb0:
        print("!!! LOI TO: can bang tu khoa Lua doi (%d -> %d)" % (cb0, cb1))
        return 1
    if nd.count('dofile("script/timerserver.lua")') != 1:
        print("!!! LOI TO: so dong dofile khong con la 1")
        return 1
    print("  can bang tu khoa Lua: %d (khong doi)" % cb1)

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    sao = P + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(P, sao)
    with io.open(P, "wb") as f:
        f.write(nd.encode("latin-1"))
    if doc(P) != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI timerserver.lua")
    print("\nCan KHOI DONG LAI GameServer (bao chu).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
