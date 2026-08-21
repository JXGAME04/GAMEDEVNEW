# -*- coding: ascii -*-
# fp_view.py - ve luoi A* server tu cache <id>_srv.fp + doi chieu tep _Region_S.dat tren dia
# Phan loai o:
#   DEN      = chan, region KHONG co _S tren dia  (ngoai map that)
#   NAU      = chan, region CO _S                 (tuong that)
#   XANH LA  = di duoc, region CO _S              (dat server: gom ca "vung trong gia")
#   VANG     = di duoc, region KHONG co _S        (mo nho pak client / khai sinh)
# Overlay: neo bai (xanh duong + vong xich), diem BotCuu (do), BotNgoai VD (cam), BotLach (tim nho)
import struct, sys, os, io, re
from PIL import Image, ImageDraw

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
OUT = os.path.dirname(os.path.abspath(__file__))

def read_fp(mapid):
    p = os.path.join(SRV, "maps", "%d_srv.fp" % mapid)
    d = open(p, "rb").read()
    magic, hdr = struct.unpack_from("<II", d, 0)
    assert magic == 0x53465005 and hdr == 2, hex(magic)
    h = struct.unpack_from("<7i", d, 8)
    rbx, rby, gw, gh, rw, rh, nall = h
    ver, gsz = struct.unpack_from("<II", d, 8+28)
    off = 8+28+8
    assert gsz == 20*nall, (gsz, nall)
    W = gw*rw; H = gh*rh
    obs = bytearray(W*H)
    for i in range(nall):
        o = struct.unpack_from("<H", d, off + i*20 + 16)[0]
        obs[i] = 1 if o else 0
    return dict(rbx=rbx, rby=rby, gw=gw, gh=gh, rw=rw, rh=rh, W=W, H=H, obs=obs)

def fname_to_id(name_bytes):
    # KPakList::FileNameToId (KPakList.cpp:72) - byte GBK la SIGNED char
    idv = 0
    idx = 0
    for b in name_bytes:
        c = b - 256 if b >= 0x80 else b
        if 0x41 <= b <= 0x5A:
            c = b + 0x20
        idx += 1
        idv = ((idv + idx * c) & 0xFFFFFFFF) % 0x8000000B * 0xFFFFFFEF & 0xFFFFFFFF
    return idv ^ 0x12345678

_pak_cache = {}
def pak_ids(pak):
    if pak in _pak_cache:
        return _pak_cache[pak]
    p = os.path.join(SRV, "Pak", pak)
    ids = set()
    if os.path.isfile(p):
        with open(p, "rb") as f:
            hdr = f.read(32)
            sig, cnt, idxoff, dataoff = struct.unpack_from("<4sIII", hdr, 0)
            assert sig == b"PACK", (pak, sig)
            f.seek(idxoff)
            d = f.read(16*cnt)
            for i in range(cnt):
                ids.add(struct.unpack_from("<I", d, i*16)[0])
    _pak_cache[pak] = ids
    return ids

def map_path_bytes(mapid):
    raw = open(os.path.join(SRV, "settings", "MapList.ini"), "rb").read()
    m = re.search((r"^%d=(.+?)\r?$" % mapid).encode(), raw, re.M)
    return m.group(1) if m else None

def srv_regions(fp, mapid):
    # set (col,row) co _Region_S.dat trong maps.pak (server);
    # kem set co _Region_C.dat trong maps_client.pak (pak client them 20/08)
    val = map_path_bytes(mapid)
    ids_srv = pak_ids("maps.pak")
    ids_cli = pak_ids("maps_client.pak")
    have_s, have_c = set(), set()
    for i in range(fp["gh"]):
        row = fp["rby"]+i
        for j in range(fp["gw"]):
            col = fp["rbx"]+j
            base = b"\\maps\\" + val + (b"\\v_%03d\\%03d" % (row, col))
            if fname_to_id(base + b"_region_s.dat") in ids_srv:
                have_s.add((col, row))
            if fname_to_id(base + b"_region_c.dat") in ids_cli:
                have_c.add((col, row))
    return have_s, have_c

def render(mapid, anchor, cuu, vd, lach, xich_cells):
    fp = read_fp(mapid)
    have, have_c = srv_regions(fp, mapid)
    W, H = fp["W"], fp["H"]
    img = Image.new("RGB", (W, H), (0, 0, 0))
    px = img.load()
    n_open_srv = n_open_cli = n_blk = 0
    for cy in range(H):
        row = fp["rby"] + cy//fp["rh"]
        for cx in range(W):
            col = fp["rbx"] + cx//fp["rw"]
            o = fp["obs"][cy*W + cx]
            has = (col, row) in have
            if o:
                px[cx, cy] = (70, 45, 25) if has else (18, 18, 18)
                n_blk += 1
            else:
                if has:
                    px[cx, cy] = (60, 130, 60); n_open_srv += 1
                else:
                    px[cx, cy] = (235, 210, 60); n_open_cli += 1
    dr = ImageDraw.Draw(img)
    def cell2px(c):
        return (c[0]-fp["rbx"]*fp["rw"], c[1]-fp["rby"]*fp["rh"])
    if anchor:
        ax, ay = cell2px(anchor)
        dr.ellipse([ax-4, ay-4, ax+4, ay+4], fill=(40, 90, 255))
        dr.ellipse([ax-xich_cells, ay-xich_cells, ax+xich_cells, ay+xich_cells],
                   outline=(40, 90, 255), width=2)
    for c in lach:
        x, y = cell2px(c)
        if 0 <= x < W and 0 <= y < H:
            dr.ellipse([x-1, y-1, x+1, y+1], fill=(180, 60, 200))
    for c in vd:
        x, y = cell2px(c)
        if 0 <= x < W and 0 <= y < H:
            dr.ellipse([x-3, y-3, x+3, y+3], outline=(255, 140, 0), width=2)
    for c in cuu:
        x, y = cell2px(c)
        if 0 <= x < W and 0 <= y < H:
            dr.line([x-4, y-4, x+4, y+4], fill=(255, 40, 40), width=2)
            dr.line([x-4, y+4, x+4, y-4], fill=(255, 40, 40), width=2)
    out = os.path.join(OUT, "map%d_grid.png" % mapid)
    img.save(out)
    tot = W*H
    print("map %d: cua so region (%d..%d, %d..%d) luoi %dx%d o" % (
        mapid, fp["rbx"], fp["rbx"]+fp["gw"]-1, fp["rby"], fp["rby"]+fp["gh"]-1, W, H))
    print("  o di duoc DAT SERVER (_S): %d | o di duoc KHONG _S (pak client khai sinh): %d | o chan: %d | tong %d" % (
        n_open_srv, n_open_cli, n_blk, tot))
    print("  region trong cua so: co _S(maps.pak)=%d, co _C(maps_client.pak)=%d, _C ma KHONG _S=%d / tong %d" % (
        len(have), len(have_c), len(have_c - have), fp["gw"]*fp["gh"]))
    print("  -> %s" % out)

def tail_log(nbytes=80*1024*1024):
    p = os.path.join(SRV, "bot.log")
    sz = os.path.getsize(p)
    f = open(p, "rb")
    f.seek(max(0, sz-nbytes))
    return f.read().decode("latin-1", "replace")

BAI = {"Sa Mac Dia Bieu": 224, "Thanh Khe Dong": 198, "Chan nui Truong Bach": 320,
       "Luong Thuy Dong": 181, "Hac Sa Dong": 875, "Truong Bach Son Bac": 322,
       "Truong Bach Son Nam": 321, "Khoa Lang Dong": 75, "Sa Mac Me Cung 1": 225,
       "Sa Mac Me Cung 2": 226, "Sa Mac Me Cung 3": 227, "Phong Lang Do": 336,
       "Mac Cao Quat": 340, "Duoc Vuong Dong tang 4": 144, "Tien Cuc Dong": 93,
       "Can Vien Dong": 124, "Tuyet Bao Dong tang 8": 152, "Lam Du Quan": 319,
       "Lao Ho Dong": 123, "Tan Lang tang 2": 206, "Tuong Duong Mat Dao": 79,
       "Hoanh Son Phai": 56, "Thien Tam Thap tang 3": 166, "Nghiet Long Dong": 182,
       "Thien Tam Thap tang 1": 164, "Luu Tien Dong": 125, "Oc Ba Dia Dao": 163}

def collect(mapid):
    txt = tail_log()
    i = txt.find("[20/08 20:26")   # chi lay du lieu SAU restart 20:25
    if i > 0:
        txt = txt[i:]
    cuu, vd, lach = [], [], []
    for m in re.finditer(r"\[BotCuu\] \S+ ket .+? tai o\((\d+),(\d+)\) -> ve bai ([A-Za-z0-9 ]+?) o\(", txt):
        if BAI.get(m.group(3).strip()) == mapid:
            cuu.append((int(m.group(1)), int(m.group(2))))
    for m in re.finditer(r"\[BotNgoai\] VD\d+ \S+ map=(\d+) o\((\d+),(\d+)\)", txt):
        if int(m.group(1)) == mapid:
            vd.append((int(m.group(2)), int(m.group(3))))
    for m in re.finditer(r"\[BotLach\] \S+ map=(\d+) ket dam dong o\((\d+),(\d+)\)", txt):
        if int(m.group(1)) == mapid:
            lach.append((int(m.group(2)), int(m.group(3))))
    return cuu, vd, lach

if __name__ == "__main__":
    XICH = 9600 // 32  # 300 o
    for mapid, anchor in [(224, (1622, 3118)), (181, (1425, 2999)), (320, (1147, 3123))]:
        cuu, vd, lach = collect(mapid)
        print("map %d: %d diem BotCuu, %d diem VD o-chan, %d diem BotLach (80MB log cuoi)" % (
            mapid, len(cuu), len(vd), len(lach)))
        render(mapid, anchor, cuu, vd, lach, XICH)
        print()
