#!/usr/bin/env python
# -*- coding: utf-8 -*-
r"""Trich TOAN BO muc cua MOT map tu mot .pak (ban Linux) sang .pak moi cho may chu du an.

  pak_extract_map.py <pak nguon> <pak dich> <duong dan map trong MapList (GBK, vd "特殊用地\banghuichengbao")> [--maxv 1000]

Bo cuc pak (Sources\Engine\Src\XPackFile.cpp:13-34): header 32 byte {'PACK', uCount, uIndexTableOffset,
uDataOffset, uCrc32, reserved[12]}, bang index 16 byte/muc {uId, uOffset, lSize, lCompressSizeFlag
(byte cao = XPACK_METHOD, 3 byte thap = kich thuoc nen)}; index PHAI sap tang theo uId (tim nhi phan).
Muc duoc chep NGUYEN BYTE (giu nen/co). uCrc32 khong duoc kiem khi Open -> ghi 0.

Ten muc duoc suy bang cach THU cac mau ten da biet cua mot thu muc map JX1 (KSubWorld.cpp):
  \maps\<map>.wor ; \maps\<map>\v_%03d\%03d_Region_S.dat (chi so TUYET DOI 0..maxv) ;
  \maps\<map>\v_%03d\%03d_Region.dat ; \maps\<map>\Trap.ini/.txt ; \maps\<map>\Obstacle*.dat ; \maps\<map>\<map>.ini ...
Ham bam = KPakList::FileNameToId (& 0xFFFFFFFF truoc %, KHONG lower toan bo: tu ha A-Z, '/'->'\\').
"""
import io, os, struct, sys, glob
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BS = chr(92)

def name2id(s):
    """KPakList::FileNameToId (KPakList.cpp:72) - ban sao DUNG tu pakcheck.py; s = chuoi latin-1 (byte tho)."""
    uid = 0
    idx = 0
    for ch in s:
        c = ord(ch)
        if 65 <= c <= 90:          # 'A'..'Z' -> thuong
            c = c + 32
        elif c > 127:              # char CO DAU tren MSVC/GCC x86 => am
            c = c - 256
        idx += 1
        uid = (((uid + idx * c) & 0xFFFFFFFF) % 0x8000000B) * 0xFFFFFFEF
        uid &= 0xFFFFFFFF
    return uid ^ 0x12345678

def gbk(s):
    return s.encode("gbk").decode("latin-1")

def load_pak(p):
    d = io.open(p, "rb").read()
    sig, count, ioff, doff = struct.unpack("<4sIII", d[:16])
    assert sig == b"PACK", sig
    idx = {}
    for i in range(count):
        uid, off, size, flag = struct.unpack("<IIiI", d[ioff + i * 16: ioff + i * 16 + 16])
        idx[uid] = (off, size, flag)
    return d, idx

def candidates(mapname, maxv):
    base = BS + "maps" + BS + mapname
    out = {base + ".wor": "wor"}
    for v in range(0, maxv):
        for i in range(0, maxv):
            out[base + BS + ("v_%03d" % v) + BS + ("%03d_Region_S.dat" % i)] = "region"
            out[base + BS + ("v_%03d" % v) + BS + ("%03d_Region_C.dat" % i)] = "region_c"
            out[base + BS + ("v_%03d" % v) + BS + ("%03d_OBSTACLE.DAT" % i)] = "obstacle"
    extra = ["Trap.ini", "Trap.txt", "trap.txt", "Obstacle.dat", "ObstacleTable.txt", "Npc.ini", "Npc_S.dat", "Obj_S.dat",
             "Region_S.dat", "Setting.ini", "setting.ini", "map.ini", "mapsetting.ini", "MapSetting.ini", "Trap_S.dat",
             "NpcRevive.ini", "obstacle.ini", "Obstacle.ini"]
    leaf = mapname.split(BS)[-1]
    for e in extra + [leaf + ".ini", leaf + ".txt", leaf + ".mps"]:
        out[base + BS + e] = "extra"
    return out

def main():
    if len(sys.argv) < 4:
        print(__doc__); return 1
    src, dst, mapname = sys.argv[1], sys.argv[2], sys.argv[3]
    maxv = 1000
    if "--maxv" in sys.argv:
        maxv = int(sys.argv[sys.argv.index("--maxv") + 1])
    d, idx = load_pak(src)
    print("nguon %s: %d muc" % (os.path.basename(src), len(idx)))
    mapname = gbk(mapname) if any(ord(c) > 255 for c in mapname) else mapname
    # doc .wor truoc de lay rect=left,top,right,bottom (chi so region TUYET DOI) -> chi thu trong pham vi,
    # tranh va cham bam ngau nhien (3 trieu ten x 95k muc / 2^32 ~ 66 va cham gia)
    wor_uid = name2id(BS + "maps" + BS + mapname + ".wor")
    if wor_uid not in idx:
        print("KHONG thay .wor -> ten map sai?"); return 2
    off, size, flag = idx[wor_uid]
    wor = d[off: off + size].decode("latin-1")
    rect = None
    for ln in wor.splitlines():
        if ln.strip().lower().startswith("rect="):
            rect = [int(x) for x in ln.strip()[5:].split(",")]
    print("wor rect =", rect)
    found = []
    for name, kind in candidates(mapname, maxv).items():
        if rect and (kind in ("region", "region_c", "obstacle")):
            parts = name.split(BS)
            v = int(parts[-2][2:]); i = int(parts[-1][:3])
            # do that map 984: v_%03d = HANG (top..bottom), %03d = COT (left..right)
            if not (rect[1] <= v <= rect[3] and rect[0] <= i <= rect[2]):
                continue
        uid = name2id(name)
        if uid in idx:
            found.append((uid, name, kind))
    kinds = {}
    for _, _, k in found:
        kinds[k] = kinds.get(k, 0) + 1
    print("thay %d muc: %s" % (len(found), kinds))
    if not any(k == "wor" for _, _, k in found):
        print("KHONG thay .wor -> ten map sai?"); return 2
    # ghi pak moi
    found.sort(key=lambda t: t[0])
    blobs = []
    for uid, name, kind in found:
        off, size, flag = idx[uid]
        csize = flag & 0x00FFFFFF
        method = flag & 0xFF000000
        n = csize if method else size
        blobs.append((uid, size, flag, d[off: off + n]))
    header_size = 32
    data_off = header_size
    out = bytearray()
    out += b"PACK" + struct.pack("<III", len(blobs), 0, data_off) + struct.pack("<I", 0) + b"\0" * 12
    index = bytearray()
    cur = data_off
    for uid, size, flag, blob in blobs:
        index += struct.pack("<IIiI", uid, cur, size, flag)
        cur += len(blob)
    ioff = cur
    struct.pack_into("<I", out, 8, ioff)
    for _, _, _, blob in blobs:
        out += blob
    out += index
    io.open(dst, "wb").write(bytes(out))
    print("ghi %s: %d muc, %d byte, index @%d" % (dst, len(blobs), len(out), ioff))
    # tu kiem: doc lai
    d2, idx2 = load_pak(dst)
    for uid, size, flag, blob in blobs:
        off2, size2, flag2 = idx2[uid]
        assert size2 == size and flag2 == flag and d2[off2: off2 + len(blob)] == blob
    print("tu kiem OK: %d muc khop nguyen byte" % len(blobs))
    for uid, name, kind in found:
        if kind != "region":
            print("  %08X %s" % (uid, name.encode("latin-1").decode("gbk", "replace")))
    return 0

if __name__ == "__main__":
    sys.exit(main())
