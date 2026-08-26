# -*- coding: ascii -*-
# tk_pick_vongkim.py - chon DIEM VONG khu Kim cua map 324 (diem bao danh Tong Kim)
# cho g_TKVongKim trong KTongKimTables.h (gen_tongkim_tables.py emit hang so nay).
#
# Boi canh (26/08/2026, chu game bao): "qua map phe kim thi chay dam thang vao
# tuong roi moi toi npc bao danh". Do tren luoi vat can server maps/324_srv.fp:
#   - duong thang DapKim (1592,3075) -> NpcBdKim (1555,3082) cham 5 o chan
#   - duong thang NpcBdKim -> XaPhuKim (1568,3075) cung cham 5 o
# (mang tuong x~1556..1568, y~3076..3082; NPC bao danh nam trong hoc phia tay,
# phai vong xuong san trong phia nam ~y 3084 roi re vao).
#
# Tieu chi chon diem vong: (1) o di duoc, 5x5 quanh diem deu di duoc;
# (2) line-of-sight (Bresenham tren luoi fp) SACH toi ca 3 dau: DapKim, NpcBdKim,
# XaPhuKim; (3) tong duong DapKim->wp->Npc ngan nhat. Ket qua la mot DAI diem
# (1564..1571, 3084) tuong duong nhau - lay diem giua (1566,3084).
#
# Chay lai khi DOI MAP 324: python tk_pick_vongkim.py
# roi cap nhat hang so trong gen_tongkim_tables.py neu diem doi.
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from fp_view import read_fp

fp = read_fp(324)
W, H = fp["W"], fp["H"]
OX, OY = fp["rbx"] * fp["rw"], fp["rby"] * fp["rh"]
obs = fp["obs"]

def blocked(x, y):
    cx, cy = x - OX, y - OY
    if cx < 0 or cy < 0 or cx >= W or cy >= H:
        return 1
    return obs[cy * W + cx]

def line_ok(a, b):
    x0, y0 = a; x1, y1 = b
    dx = abs(x1 - x0); dy = abs(y1 - y0)
    sx = 1 if x0 < x1 else -1
    sy = 1 if y0 < y1 else -1
    err = dx - dy; x, y = x0, y0
    while True:
        if blocked(x, y):
            return False
        if x == x1 and y == y1:
            return True
        e2 = 2 * err
        if e2 > -dy: err -= dy; x += sx
        if e2 < dx:  err += dx; y += sy

def clear5(x, y):
    for dx in range(-2, 3):
        for dy in range(-2, 3):
            if blocked(x + dx, y + dy):
                return False
    return True

DAP  = (1592, 3075)   # diem dap Chieu Thu phe Kim (item/battles/rescript.lua)
NPC  = (1555, 3082)   # Kim Binh Bao Danh (startgame.lua)
XAFU = (1568, 3075)   # Xa Phu phe Kim
SWAP = (1570, 3085)   # diem dap khi qua phe bang Xa Phu (xaphu.lua battle_transprot)

best = []
for x in range(1550, 1585):
    for y in range(3080, 3095):
        if blocked(x, y) or not clear5(x, y):
            continue
        if not (line_ok((x, y), DAP) and line_ok((x, y), NPC) and line_ok((x, y), XAFU)):
            continue
        d = abs(x - DAP[0]) + abs(y - DAP[1]) + abs(x - NPC[0]) + abs(y - NPC[1])
        best.append((d, x, y))
best.sort()
if not best:
    print("KHONG tim duoc diem vong dat chuan - map 324 doi nhieu, xem lai tay!")
    sys.exit(1)
print("top diem vong (LOS sach Dap+Npc+XaPhu, 5x5 thoang, duong ngan truoc):")
for d, x, y in best[:12]:
    print("  (%d,%d) tongduong=%d LOS_Swap=%d" % (x, y, d, line_ok((x, y), SWAP)))
print("de nghi: lay diem GIUA cua dai ngan nhat (hien tai: 1566,3084)")
