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

# (26/08 v2) PHAI mo phong DUNG bo lay mau cua TK_ThayDuoc (CoreShell.cpp:6861),
# KHONG duoc dung Bresenham: hai cach di HAI DUONG KHAC NHAU. Ban dau tool nay dung
# Bresenham nen cham diem (1566,3084) la 'sach', trong khi engine lay mau deu thi
# duong do cham 2 o tuong (1560,3082)+(1559,3082) => auto van huc tuong. Ngoai ra
# toa do phai tinh bang O*32 (dung nhu macro TK_O), KHONG cong nua o.
def line_ok(a, b):
    ax, ay = a[0] * 32, a[1] * 32
    bx, by = b[0] * 32, b[1] * 32
    dx, dy = bx - ax, by - ay
    nB = max(abs(dx), abs(dy)) // 32
    if nB < 2:
        return True
    if nB > 48:
        nB = 48
    for i in range(1, nB):
        if blocked((ax + dx * i // nB) // 32, (ay + dy * i // nB) // 32):
            return False
    return True

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
