# -*- coding: utf-8 -*-
"""map53_npcobj.py - doc CHINH XAC phan 2 (NPC client) va phan 3 (OBJ) cua Region_C.dat map 53.
Bo cuc lay tu Sources/Core/Src/Scene/SceneDataDef.h:
  KNpcFileHead = 12 byte (uNumNpc + 2 reserved)
  KSPNpc = 60 byte truoc szScript: tid4 x4 y4 special1 res3 name32 level2 curframe2 head2 kind2 camp1 series1 scriptlen2
  KObjFileHead = 12 byte; KSPObj = 24 byte truoc szScript: tid4 state2 bio2 pos12 dir1 skip1 scriptlen2
Chi doc.
"""
import re
import struct
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk")
sys.path.insert(0, r"D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk\vltk2")
from doc_vatcanh import Kho, DUAN, V20, BS  # noqa: E402

NPC_REC, OBJ_REC, HEAD = 60, 24, 12


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
    uMax = struct.unpack("<I", d[:4])[0]
    if uMax == 0 or uMax > 64 or len(d) < 4 + 8 * uMax:
        return None, 0
    return [struct.unpack("<II", d[4 + i * 8: 12 + i * 8]) for i in range(uMax)], 4 + 8 * uMax


def quet(kho, idx, nhan):
    mp = duong_map(kho, idx)
    if not mp:
        print("%s: khong tim thay map %d trong maplist" % (nhan, idx)); return
    w = kho.doc(mp.encode("gbk", "replace") + b".wor")
    if not w:
        print("%s: khong co .wor" % nhan); return
    m = re.search(r"rect=(\d+),(\d+),(\d+),(\d+)", w.decode("gbk", "replace"))
    x0, y0, x1, y1 = (int(m.group(i)) for i in (1, 2, 3, 4))
    npcs, objs = {}, {}
    n_npc = n_obj = 0
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            d = kho.doc(("%s%sv_%03d%s%03d_Region_C.dat" % (mp, BS, y, BS, x)).encode("gbk", "replace"))
            if not d:
                continue
            secs, head = cat(d)
            if not secs or len(secs) < 6:
                continue
            off, ln = secs[2]
            if ln > HEAD:
                b = d[head + off: head + off + ln]
                n = struct.unpack("<I", b[:4])[0]
                p = HEAD
                for i in range(n):
                    if p + NPC_REC > len(b):
                        break
                    r = b[p:p + NPC_REC]
                    tid, px, py = struct.unpack("<iii", r[0:12])
                    ten = r[16:48].split(b"\x00")[0]
                    kind, = struct.unpack("<h", r[54:56])
                    slen, = struct.unpack("<H", r[58:60])
                    p += NPC_REC + slen
                    n_npc += 1
                    k = npcs.setdefault((tid, ten, kind), [0, []])
                    k[0] += 1
                    if len(k[1]) < 4:
                        k[1].append((px // 256, py // 512))
            off, ln = secs[3]
            if ln > HEAD:
                b = d[head + off: head + off + ln]
                n = struct.unpack("<I", b[:4])[0]
                p = HEAD
                for i in range(n):
                    if p + OBJ_REC > len(b):
                        break
                    r = b[p:p + OBJ_REC]
                    tid, = struct.unpack("<i", r[0:4])
                    ox, oy, oz = struct.unpack("<iii", r[8:20])
                    slen, = struct.unpack("<H", r[22:24])
                    p += OBJ_REC + slen
                    n_obj += 1
                    k = objs.setdefault(tid, [0, []])
                    k[0] += 1
                    if len(k[1]) < 3:
                        k[1].append((ox // 256, oy // 512))
    print("\n===== %s : map %d = %s =====" % (nhan, idx, mp))
    print("-- PHAN 2 NPC: %d ban ghi, %d loai --" % (n_npc, len(npcs)))
    for (tid, ten, kind), (c, vt) in sorted(npcs.items(), key=lambda kv: -kv[1][0]):
        print("  %3d x id=%-5d kind=%-3d %-16s o: %s" % (c, tid, kind, ten.decode("gbk", "replace")[:16], vt))
    print("-- PHAN 3 OBJ: %d ban ghi, %d loai --" % (n_obj, len(objs)))
    for tid, (c, vt) in sorted(objs.items(), key=lambda kv: -kv[1][0]):
        print("  %3d x id=%-5d o: %s" % (c, tid, vt))


if __name__ == "__main__":
    idx = int(sys.argv[1]) if len(sys.argv) > 1 else 53
    quet(Kho(DUAN), idx, "DU AN")
    quet(Kho(V20), idx, "2.0")
