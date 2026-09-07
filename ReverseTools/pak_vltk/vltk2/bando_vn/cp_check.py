# -*- coding: utf-8 -*-
"""Chung minh: ten thu muc GBK tren dia duoc Windows luu = byte GBK doc theo cp1252.
Neu dung thi client ANSI (CreateFileA) se tim thay thu muc ta tao bang cach nay."""
import os, sys, locale
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
ROOT = "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/client/Spr/Ui3"
print("ANSI CP cua may:", locale.getpreferredencoding())
tests = [("NPC对话条", "NPC" + "对话条".encode("gbk").decode("cp1252")),
         ("帮会界面", "帮会界面".encode("gbk").decode("cp1252")),
         ("登入界面", "登入界面".encode("gbk").decode("cp1252")),
         ("铸造装备", "铸造装备".encode("gbk").decode("cp1252"))]
ok = 0
for cn, disk in tests:
    p = os.path.join(ROOT, disk)
    r = os.path.isdir(p)
    ok += r
    print("  %-10s -> %-14r ton tai=%s" % (cn, disk, r))
print("KET LUAN:", "cp1252 DUNG (%d/%d)" % (ok, len(tests)) if ok == len(tests) else "KHONG KHOP - DUNG LAI")
# ten se tao
for cn in ("小地图", "剑侠大地图2_vn.spr", "打怪向导山洞图_vn.spr"):
    b = cn.encode("gbk")
    bad = [x for x in b if x in (0x81, 0x8D, 0x8F, 0x90, 0x9D)]
    print("  %-22s gbk=%s cp1252=%r  byte-khong-dinh-nghia=%s" % (cn, b.hex(" "), b.decode("cp1252", "replace"), bad))
