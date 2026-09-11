# -*- coding: utf-8 -*-
"""canh_v20.py - MO NHI PHAN (chi doc) client VLTK 2.0: xem canh (scene) cua cac map duoc dung
bang nhung tep gi, so voi dinh dang JX1 cua du an.

Dung: python canh_v20.py <lenh>
  wor          - rut .wor cua 3 map chu hoi + dem .wor cua moi map trong MapList.ini
  vung <map>   - do xem map do co bao nhieu o vung (v_yyy\\xxx_*.dat), tep nao co that
  kieu         - phan loai 64 byte dau cua mot so muc trong map.pak (xem co dinh dang la khong)
"""
import io
import os
import re
import struct
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk\vltk2")
import ucl  # noqa: E402

CLIENT = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky 2.0\data"
SP = r"C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\4001d2df-3b1a-4386-9730-72e4d6ac7cf7\scratchpad\v20canh"
PAKS = ["common.pak", "slistcl.pak", "update.pak", "minimap.pak", "freeresource.pak",
        "skills.pak", "spr.pak", "resource.pak", "map.pak", "font.pak", "sound.pak"]
BS = chr(92)  # dau \


def name2id(b):
    uid = 0
    idx = 0
    for c in b:
        if 65 <= c <= 90:
            c += 32
        elif c > 127:
            c -= 256
        idx += 1
        uid = (((uid + idx * c) & 0xFFFFFFFF) % 0x8000000B) * 0xFFFFFFEF
        uid &= 0xFFFFFFFF
    return uid ^ 0x12345678


_PAK = {}


def mo():
    if _PAK:
        return _PAK
    for name in PAKS:
        p = os.path.join(CLIENT, name)
        if not os.path.exists(p):
            continue
        f = open(p, "rb")
        sig, count, ioff, doff = struct.unpack("<IIII", f.read(16))
        f.seek(ioff)
        raw = f.read(count * 16)
        d = {}
        for i in range(count):
            uid, off, size, cf = struct.unpack("<IIiI", raw[i * 16:i * 16 + 16])
            d[uid] = (off, size, cf)
        _PAK[name] = (f, d)
    return _PAK


def tim1(b):
    """tra (pak, off, size, cf) hoac None"""
    uid = name2id(b)
    for name, (f, d) in mo().items():
        if uid in d:
            return (name,) + d[uid]
    return None


def doc(b):
    r = tim1(b)
    if not r:
        return None
    name, off, size, cf = r
    f = mo()[name][0]
    csize = cf & 0xFFFFFF
    f.seek(off)
    if csize == 0 or csize == size:
        return f.read(size)
    raw = f.read(csize)
    try:
        return bytes(ucl.nrv2b_decompress_8(raw, size))
    except Exception as e:
        return b"(giai nen hong: %s)" % str(e).encode()


def maplist():
    d = doc((BS + "settings" + BS + "maplist.ini").encode("ascii"))
    s = d.decode("gbk", "replace").replace("\r\n", "\n")
    out = []
    for l in s.split("\n"):
        m = re.match(r"^\s*(\d+)\s*=\s*(" + re.escape(BS) + r"maps" + re.escape(BS) + r".+?)\s*$", l)
        if m:
            out.append((int(m.group(1)), m.group(2)))
    return out


def lenh_wor():
    os.makedirs(SP, exist_ok=True)
    ml = maplist()
    print("MapList.ini: %d muc map" % len(ml))
    duong = {}
    for idx, p in ml:
        duong.setdefault(p, []).append(idx)
    print("duong dan map khac nhau: %d" % len(duong))
    co = khong = 0
    thieu = []
    for p in duong:
        b = p.encode("gbk", "replace") + b".wor"
        if tim1(b):
            co += 1
        else:
            khong += 1
            thieu.append(p)
    print("  co .wor trong pak: %d | khong thay: %d" % (co, khong))
    for p in thieu[:10]:
        print("    thieu: %s" % p)
    for ten, p in [("HoaSon", BS + "maps" + BS + "西北南区" + BS + "华山"),
                   ("HoaSonTuyetDinh", BS + "maps" + BS + "西北南区" + BS + "华山绝顶"),
                   ("MacCaoQuat", BS + "maps" + BS + "西北北区" + BS + "莫高窟"),
                   ("MinhNguyetTran", BS + "maps" + BS + "特殊用地" + BS + "明月镇")]:
        b = p.encode("gbk", "replace") + b".wor"
        d = doc(b)
        print("=== %s (%s) ===" % (ten, p))
        if d is None:
            print("   KHONG THAY .wor")
            continue
        io.open(os.path.join(SP, ten + ".wor"), "wb").write(d)
        print(d.decode("gbk", "replace").replace("\r\n", "\n").strip()[:900])


def lenh_vung(ten=None):
    """do so o vung co that cua mot map: <map>\\v_yyy\\xxx_<tep>"""
    ds = {"hoason": BS + "maps" + BS + "西北南区" + BS + "华山",
          "hoasontuyetdinh": BS + "maps" + BS + "西北南区" + BS + "华山绝顶",
          "maccaoquat": BS + "maps" + BS + "西北北区" + BS + "莫高窟",
          "minhnguyettran": BS + "maps" + BS + "特殊用地" + BS + "明月镇",
          "phuongtuong": BS + "maps" + BS + "西北南区" + BS + "凤翔"}
    keys = [ten] if ten else list(ds)
    TEP = ["Region_C.dat", "Ground.dat", "BuildinObj.Dat", "OBSTACLE.DAT", "Trap.dat", "Npc_C.dat", "Obj_C.dat"]
    for k in keys:
        p = ds[k]
        print("=== %s : %s ===" % (k, p))
        # rect trong .wor = pham vi CHI SO O VUNG (x0,y0,x1,y1), khong phai 0..31
        w = doc(p.encode("gbk", "replace") + b".wor")
        x0, y0, x1, y1 = 0, 0, 31, 31
        if w:
            m = re.search(r"rect=(\d+),(\d+),(\d+),(\d+)", w.decode("gbk", "replace"))
            if m:
                x0, y0, x1, y1 = (int(m.group(i)) for i in (1, 2, 3, 4))
        print("   rect = %d,%d .. %d,%d (%d x %d o)" % (x0, y0, x1, y1, x1 - x0 + 1, y1 - y0 + 1))
        dem = {t: 0 for t in TEP}
        cell = 0
        for y in range(max(0, y0 - 2), y1 + 3):
            for x in range(max(0, x0 - 2), x1 + 3):
                pre = "%s%sv_%03d%s%03d_" % (p, BS, y, BS, x)
                co_o = False
                for t in TEP:
                    b = (pre + t).encode("gbk", "replace")
                    r = tim1(b)
                    if r:
                        dem[t] += 1
                        co_o = True
                if co_o:
                    cell += 1
        print("   o vung co tep: %d | %s" % (cell, ", ".join("%s %d" % (t, dem[t]) for t in TEP)))


if __name__ == "__main__":
    l = sys.argv[1] if len(sys.argv) > 1 else "wor"
    if l == "wor":
        lenh_wor()
    elif l == "vung":
        lenh_vung(sys.argv[2] if len(sys.argv) > 2 else None)
    else:
        print(__doc__)
