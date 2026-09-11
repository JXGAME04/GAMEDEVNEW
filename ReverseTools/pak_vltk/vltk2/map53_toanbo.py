# -*- coding: utf-8 -*-
"""map53_toanbo.py - do TOAN BO du lieu canh cua map 53 (Ba Lang huyen) trong pak client du an:
  phan 5 BUILDIN (vat canh + hoat anh), phan 2 NPC (client npc = dong vat), phan 3 OBJ (am thanh/vat the).
Chi doc, khong sua gi.
"""
import re
import struct
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk")
sys.path.insert(0, r"D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk\vltk2")
from doc_vatcanh import Kho, DUAN, V20, BS, BIO  # noqa: E402

NPC_REC = 52      # KSPNpc khong ke szScript
OBJ_REC = 24      # KSPObj khong ke szScript


def duong_map(kho, idx):
    d = kho.doc((BS + "settings" + BS + "maplist.ini").encode("ascii"))
    for l in d.decode("gbk", "replace").replace("\r\n", "\n").split("\n"):
        m = re.match(r"^\s*(\d+)\s*=\s*(.+?)\s*$", l)
        if m and int(m.group(1)) == idx:
            mp = m.group(2)
            if not mp.lower().startswith(BS + "maps"):
                mp = BS + "maps" + (mp if mp.startswith(BS) else BS + mp)
            return mp
    return None


def cat(d):
    """tra list (offset, length) cac phan + head size"""
    uMax = struct.unpack("<I", d[:4])[0]
    if uMax == 0 or uMax > 64 or len(d) < 4 + 8 * uMax:
        return None, 0
    secs = [struct.unpack("<II", d[4 + i * 8: 12 + i * 8]) for i in range(uMax)]
    return secs, 4 + 8 * uMax


def doc_npc(b):
    ra = []
    if len(b) < 8:
        return ra
    n = struct.unpack("<I", b[:4])[0]
    if n > 5000:
        return ra
    p = 8
    for i in range(n):
        if p + NPC_REC > len(b):
            break
        r = b[p:p + NPC_REC]
        p += NPC_REC
        tid, = struct.unpack("<i", r[0:4])
        px, py = struct.unpack("<ii", r[4:12])
        kind, = struct.unpack("<h", r[12:14])
        slen = r[NPC_REC - 1]
        ten = r[16:16 + 32].split(b"\x00")[0]
        ra.append((tid, px, py, kind, ten, r))
        # script noi tiep
    return ra


def main():
    kho = Kho(DUAN)
    mp = duong_map(kho, 53)
    print("map 53 = %s" % mp)
    w = kho.doc(mp.encode("gbk", "replace") + b".wor")
    m = re.search(r"rect=(\d+),(\d+),(\d+),(\d+)", w.decode("gbk", "replace"))
    x0, y0, x1, y1 = (int(m.group(i)) for i in (1, 2, 3, 4))
    print("rect=%d,%d,%d,%d" % (x0, y0, x1, y1))
    ani = {}
    tong = 0
    npc_tong = {}
    obj_tong = {}
    cell_npc = []
    dai_npc = dai_obj = 0
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            d = kho.doc(("%s%sv_%03d%s%03d_Region_C.dat" % (mp, BS, y, BS, x)).encode("gbk", "replace"))
            if not d:
                continue
            secs, head = cat(d)
            if not secs or len(secs) < 6:
                continue
            # phan 5: buildin
            off, ln = secs[5]
            if ln:
                b = d[head + off: head + off + ln]
                n = struct.unpack("<I", b[:4])[0]
                p = 16
                for i in range(n):
                    if p + BIO > len(b):
                        break
                    rec = b[p:p + BIO]
                    p += BIO
                    tong += 1
                    a = struct.unpack("<H", rec[192:194])[0]
                    if a:
                        ten = rec[56:184].split(b"\x00")[0]
                        o1 = struct.unpack("<iii", rec[196:208])
                        k = ani.setdefault(ten, [0, []])
                        k[0] += 1
                        if len(k[1]) < 3:
                            k[1].append((o1[0] // 256, o1[1] // 512))
            # phan 2: npc
            off, ln = secs[2]
            dai_npc += ln
            if ln:
                b = d[head + off: head + off + ln]
                for tid, px, py, kind, ten, r in doc_npc(b):
                    k = npc_tong.setdefault((tid, ten), [0, []])
                    k[0] += 1
                    if len(k[1]) < 4:
                        k[1].append((px // 256, py // 512))
            # phan 3: obj
            off, ln = secs[3]
            dai_obj += ln
            if ln:
                b = d[head + off: head + off + ln]
                n = struct.unpack("<I", b[:4])[0]
                p = 8
                for i in range(n):
                    if p + OBJ_REC > len(b):
                        break
                    r = b[p:p + OBJ_REC]
                    p += OBJ_REC
                    did, = struct.unpack("<i", r[0:4])
                    obj_tong[did] = obj_tong.get(did, 0) + 1
    print("\n-- PHAN 5 BUILDIN: %d vat canh, %d loai co hoat anh --" % (tong, len(ani)))
    for t, (c, vt) in sorted(ani.items(), key=lambda kv: -kv[1][0]):
        print("  %5d x %-46s o: %s" % (c, t.decode("gbk", "replace")[:46], vt))
    print("\n-- PHAN 2 NPC (client): tong do dai %d byte, %d loai --" % (dai_npc, len(npc_tong)))
    for (tid, ten), (c, vt) in sorted(npc_tong.items(), key=lambda kv: -kv[1][0]):
        print("  %3d x id=%-5d %-20s o: %s" % (c, tid, ten.decode("gbk", "replace")[:20], vt))
    print("\n-- PHAN 3 OBJ: tong do dai %d byte, %d loai --" % (dai_obj, len(obj_tong)))
    for did, c in sorted(obj_tong.items(), key=lambda kv: -kv[1]):
        print("  %3d x dataid=%d" % (c, did))


if __name__ == "__main__":
    main()
