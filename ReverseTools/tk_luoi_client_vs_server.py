# -*- coding: ascii -*-
# tk_luoi_client_vs_server.py - so lu?i vat can A* phia CLIENT (bin\client\maps\<id>.fp)
# voi phia SERVER (bin\server\maps\<id>_srv.fp) tren cung mot ban do.
#
# Vi sao can: KSubWorld::ProcLoadPathGrid doc HAI TEP KHAC NHAU cho hai phia
#   server: %03d_Region_S.dat   (#ifdef _SERVER, KSubWorld.cpp:258)
#   client: %03d_Region_C.dat   (#else,          KSubWorld.cpp:261)
# va luat "thieu du lieu = vat can" CHI ap cho server (KSubWorld.cpp:380-386).
# => hai ben CO THE khac nhau; moi phep do bang <id>_srv.fp KHONG dai dien cho
# cai ma CLIENT (va auto WAuto chay trong client) nhin thay.
#
# Dinh dang cache (KSubWorld.cpp:2193-2240):
#   [FINDPATH_VERSION:u32][gsz:u32][VGridNode * nAllCell][nNb:u32][VGridNeighbour * nNb]
#   VGridNode = 20 byte, truong obs la u16 tai offset 16 (giong ban server - xem fp_view.py).
# Kich thuoc luoi (rbx/rby/gw/gh/rw/rh) KHONG nam trong cache client => lay tu ban _srv.fp
# cua cung ban do (hai ben dung chung cua so region).
import struct, sys, os

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"

def read_srv(mapid):
    p = os.path.join(SRV, "maps", "%d_srv.fp" % mapid)
    d = open(p, "rb").read()
    magic, hdr = struct.unpack_from("<II", d, 0)
    assert magic in (0x53465005, 0x53465006) and hdr == 2, hex(magic)
    rbx, rby, gw, gh, rw, rh, nall = struct.unpack_from("<7i", d, 8)
    ver, gsz = struct.unpack_from("<II", d, 36)
    assert gsz == 20 * nall, (gsz, nall)
    off = 44
    obs = bytearray(nall)
    for i in range(nall):
        obs[i] = 1 if struct.unpack_from("<H", d, off + i * 20 + 16)[0] else 0
    return dict(rbx=rbx, rby=rby, gw=gw, gh=gh, rw=rw, rh=rh,
                W=gw * rw, H=gh * rh, obs=obs, nall=nall)

def read_cli(mapid, geom):
    p = os.path.join(CLI, "maps", "%d.fp" % mapid)
    if not os.path.isfile(p):
        return None
    d = open(p, "rb").read()
    ver, gsz = struct.unpack_from("<II", d, 0)
    nall = gsz // 20
    if nall != geom["nall"]:
        print("  ! cache client co %d o, server %d o - luoi lech, khong so duoc"
              % (nall, geom["nall"]))
        return None
    off = 8
    obs = bytearray(nall)
    for i in range(nall):
        obs[i] = 1 if struct.unpack_from("<H", d, off + i * 20 + 16)[0] else 0
    return obs

def mk(geom, obs):
    OX, OY = geom["rbx"] * geom["rw"], geom["rby"] * geom["rh"]
    W, H = geom["W"], geom["H"]
    def blocked(x, y):
        cx, cy = x - OX, y - OY
        if cx < 0 or cy < 0 or cx >= W or cy >= H:
            return 1
        return obs[cy * W + cx]
    return blocked

def line_cells(a, b):
    x0, y0 = a; x1, y1 = b
    dx = abs(x1 - x0); dy = abs(y1 - y0)
    sx = 1 if x0 < x1 else -1
    sy = 1 if y0 < y1 else -1
    err = dx - dy; x, y = x0, y0
    out = []
    while True:
        out.append((x, y))
        if x == x1 and y == y1:
            return out
        e2 = 2 * err
        if e2 > -dy: err -= dy; x += sx
        if e2 < dx:  err += dx; y += sy

PTS = {
    "DapKim":   (1592, 3075),
    "SwapKim":  (1570, 3085),
    "NpcBdKim": (1555, 3082),
    "XaPhuKim": (1568, 3075),
    "ShopKim":  (1580, 3074),
    "VongKim":  (1566, 3084),
    "DapTong":  (1529, 3196),
    "NpcBdTong":(1550, 3179),
    "XaPhuTong":(1535, 3153),
}
LEGS = [
    ("DapKim -> NpcBdKim",  "DapKim",   "NpcBdKim"),
    ("DapKim -> VongKim",   "DapKim",   "VongKim"),
    ("VongKim -> NpcBdKim", "VongKim",  "NpcBdKim"),
    ("SwapKim -> NpcBdKim", "SwapKim",  "NpcBdKim"),
    ("NpcBdKim -> XaPhuKim","NpcBdKim", "XaPhuKim"),
    ("DapKim -> XaPhuKim",  "DapKim",   "XaPhuKim"),
    ("DapTong -> NpcBdTong","DapTong",  "NpcBdTong"),
]

def main(mapid=324):
    geom = read_srv(mapid)
    print("map %d: cua so region (%d..%d, %d..%d) luoi %dx%d o"
          % (mapid, geom["rbx"], geom["rbx"] + geom["gw"] - 1,
             geom["rby"], geom["rby"] + geom["gh"] - 1, geom["W"], geom["H"]))
    cobs = read_cli(mapid, geom)
    bs = mk(geom, geom["obs"])
    print("\n== o dang dung cua tung diem (1 = CHAN) ==")
    print("   %-12s %-8s %-8s" % ("diem", "SERVER", "CLIENT"))
    bc = mk(geom, cobs) if cobs else None
    for n, p in PTS.items():
        print("   %-12s %-8d %-8s" % (n, bs(*p), (bc(*p) if bc else "?")))
    print("\n== so o CHAN tren duong THANG tung chang ==")
    print("   %-24s %-8s %-8s" % ("chang", "SERVER", "CLIENT"))
    for name, a, b in LEGS:
        cells = line_cells(PTS[a], PTS[b])
        ns = sum(1 for c in cells if bs(*c))
        nc = sum(1 for c in cells if bc(*c)) if bc else "?"
        print("   %-24s %-8d %-8s" % (name, ns, nc))
    if cobs:
        diff = sum(1 for i in range(geom["nall"]) if cobs[i] != geom["obs"][i])
        print("\n== tong the ==")
        print("   o chan SERVER: %d / %d" % (sum(geom["obs"]), geom["nall"]))
        print("   o chan CLIENT: %d / %d" % (sum(cobs), geom["nall"]))
        print("   o KHAC NHAU  : %d (%.2f%%)" % (diff, 100.0 * diff / geom["nall"]))
        # in ASCII khu Kim ca hai ben de nhin tan mat
        for tag, f in (("SERVER", bs), ("CLIENT", bc)):
            print("\n== ASCII khu Kim (%s)  x 1545..1600, y 3070..3092 ==" % tag)
            marks = {v: k[0] for k, v in PTS.items() if 1545 <= v[0] <= 1600 and 3070 <= v[1] <= 3092}
            for y in range(3070, 3093):
                row = "".join(marks.get((x, y), "#" if f(x, y) else ".") for x in range(1545, 1601))
                print("%5d %s" % (y, row))

if __name__ == "__main__":
    main(int(sys.argv[1]) if len(sys.argv) > 1 else 324)
