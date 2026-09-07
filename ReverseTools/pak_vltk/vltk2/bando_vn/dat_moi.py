# -*- coding: utf-8 -*-
"""Dat cap .moi (Game.exe + CoreClient.dll) vao bin\\client, sao luu .moi cu cua phien khac (neu co).
Dung: python dat_moi.py <Game.exe moi> <CoreClient.dll moi> [nhan_sao_luu]"""
import os, sys, shutil, hashlib, time
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
CLIENT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
game, core = sys.argv[1], sys.argv[2]
tag = sys.argv[3] if len(sys.argv) > 3 else "phienkhac"
def md5(p): return hashlib.md5(open(p, "rb").read()).hexdigest()[:8]
for src, name in ((game, "Game.exe"), (core, "CoreClient.dll")):
    dst = os.path.join(CLIENT, name + ".moi")
    if os.path.exists(dst):
        bak = dst + ".%s_%s_%s" % (tag, time.strftime("%H%M", time.localtime(os.path.getmtime(dst))), md5(dst))
        shutil.move(dst, bak)
        print("  sao luu .moi cu -> %s" % os.path.basename(bak))
    shutil.copy2(src, dst)
    print("  DAT %-18s %9d B  md5 %s  (dang chay: %s %d B)" % (
        os.path.basename(dst), os.path.getsize(dst), md5(dst), md5(os.path.join(CLIENT, name)), os.path.getsize(os.path.join(CLIENT, name))))
print("XONG - chu chay ChoiGame.bat de swap CA HAI (cung commit).")
