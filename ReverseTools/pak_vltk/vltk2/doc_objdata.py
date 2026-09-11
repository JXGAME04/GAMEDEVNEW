# -*- coding: utf-8 -*-
"""doc_objdata.py - in cac dong ObjData.txt ung voi id OBJ tim duoc o map 53, va tra anh hoat anh cua map 53."""
import struct
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk")
sys.path.insert(0, r"D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk\vltk2")
from doc_vatcanh import Kho, DUAN, BS  # noqa: E402

IDS = [313, 314, 319, 320, 321, 322, 323]
k = Kho(DUAN)
for ten in [BS + "settings" + BS + "obj" + BS + "ObjData.txt", BS + "settings" + BS + "ObjData.txt"]:
    d = k.doc(ten.encode("ascii"))
    if not d:
        print("%s: KHONG CO" % ten)
        continue
    ls = d.replace(b"\r\n", b"\n").split(b"\n")
    print("=== %s : %d dong ===" % (ten, len(ls)))
    for l in ls:
        c = l.split(b"\t")
        if len(c) > 6 and c[1].strip().isdigit() and int(c[1]) in IDS:
            print("  id %-4s %-22s kind=%-14s anh=%-26s tieng=%s" % (
                c[1].decode(), c[0].decode("gbk", "replace")[:22], c[2].decode("gbk", "replace"),
                c[4].decode("gbk", "replace")[:26], c[5].decode("gbk", "replace")[:34]))
    print("")

ANH = [BS + "\u6e38\u620f\u8d44\u6e90" + BS + "\u5730\u56fe\u52a8\u753b" + BS + "\u7194\u5ca9" + BS + "10160.spr",
       BS + "\u6e38\u620f\u8d44\u6e90" + BS + "\u5730\u56fe\u52a8\u753b" + BS + "\u7194\u5ca9" + BS + "\u77ed\u70df.spr"]
for anh in ANH:
    d = k.doc(anh.encode("gbk", "replace"))
    if d and len(d) >= 24:
        sig, ver, w, h, cx, cy, nf, ncol, ndir, interval = struct.unpack("<4sHhhhhHHHH", d[:22])
        print("%-16s %8d byte, %dx%d, %d khung, %d mau, %d huong, %d ms/khung" % (
            anh.split(BS)[-1], len(d), w, h, nf, ncol, ndir, interval))
    else:
        print("%-16s KHONG CO trong pak" % anh.split(BS)[-1])
