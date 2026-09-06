# -*- coding: utf-8 -*-
"""[LMBC 06/09] BO VA - chep 4 tep thu vien Long Mon Tieu Cuc vao CAY CHAY THAT.

KHONG tu chay khi chua duoc chu duyet. Script chi CHEP tep .lua va giu ban cu
thanh <ten>.truoc_lmbc.

CANH BAO [LMBC-KIEM 06/09]: TEP .LUA MOI KHONG NAP NONG. Cay \\script chi duoc
quet mot lan luc boot (LoadAllScript("\\script") trong g_IniScriptEngine,
KSortScript.cpp:56); g_GetScript KHONG tu nap tep chua co trong cay (xem
LuaDynamicExecuteByPlayer ScriptFuns.cpp: in "script chua nap, bo qua").
=> Sau khi --ghi PHAI restart may chu, hoac dung lenh GM ReloadAllScript
(KGMCommand.cpp:156 -> ReLoadAllScript).

CHAY THU (chi in, khong ghi):
    set PYTHONIOENCODING=utf-8 && python ap_dung_4tep.py
GHI THAT:
    set PYTHONIOENCODING=utf-8 && python ap_dung_4tep.py --ghi
GO RA (tra lai ban .truoc_lmbc):
    set PYTHONIOENCODING=utf-8 && python ap_dung_4tep.py --lui
"""
import io
import os
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

NGUON = r"D:\GAMEDEVNEW_wt_vantieu\serverscript_jx2\vantieu\moi"
DICH = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"

TEP = [
    r"script\lib\lib_lmbiaoche.lua",
    r"script\global\maplist.lua",
    r"script\activitysys\npcfunlib.lua",
    r"script\global\yunbiao_system.lua",
]

GHI = "--ghi" in sys.argv
LUI = "--lui" in sys.argv


def main():
    for rel in TEP:
        src = os.path.join(NGUON, rel)
        dst = os.path.join(DICH, rel)
        bak = dst + ".truoc_lmbc"

        if LUI:
            if os.path.exists(bak):
                print("LUI  %s  <- %s" % (rel, os.path.basename(bak)))
                shutil.copyfile(bak, dst)
            else:
                print("LUI  %s  KHONG co ban luu -> XOA thu cong neu muon" % rel)
            continue

        if not os.path.exists(src):
            print("!! THIEU NGUON %s" % src)
            continue
        n = os.path.getsize(src)
        co = os.path.exists(dst)
        print("%s %-46s %6d byte  %s" % ("GHI " if GHI else "THU ", rel, n,
                                         "(de len ban cu)" if co else "(tep moi)"))
        if not GHI:
            continue
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        if co and not os.path.exists(bak):
            shutil.copyfile(dst, bak)
        # chep NGUYEN BYTE - tuyet doi khong doc/ghi qua text mode
        shutil.copyfile(src, dst)

    if not GHI and not LUI:
        print("")
        print("Day moi la CHAY THU. Them --ghi de chep that.")
        print("CANH BAO: tep .lua MOI khong nap nong - sau khi --ghi phai")
        print("RESTART may chu (hoac GM ReloadAllScript) thi engine moi thay.")


if __name__ == "__main__":
    main()
