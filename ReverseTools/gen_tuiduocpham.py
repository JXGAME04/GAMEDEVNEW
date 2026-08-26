# -*- coding: utf-8 -*-
"""SINH KTuiDuocPham.h - danh sach BAN DO KHONG MO DUOC "Tui duoc pham" (G6/D1/P4813).

Chu game 25/08: "dang o trong map tong kim auto tu mo tui mau (map tong kim khong
cho mo tui mau)". Auto (ATYPE_OPENBAG) cu 3 giay lai thu mot lan, server tra
Talk("Ban do hien tai nguoi dang dung khong the mo!") -> spam vo ich.

Nguon SU THAT: chinh script cua vat pham -
  bin\\server\\script\\item\\tuiduocpham.lua  (ham main, cac dieu kien nSubWorldID)
Script nay KHONG goi checkSJMaps ma liet ke tay, nen phai doc thang tu no.

Chay:  python ReverseTools\\gen_tuiduocpham.py
"""
import io, os, re, sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

LUA = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\tuiduocpham.lua"
OUT = r"D:\GAMEDEVNEW\Sources\Core\Src\KTuiDuocPham.h"

src = io.open(LUA, encoding="latin-1", newline="").read()
i = src.find("function main(")
assert i > 0, "khong thay function main() trong tuiduocpham.lua"
j = src.find("if (GetLevel()", i)
assert j > i, "khong thay moc ket thuc phan kiem ban do"
than = src[i:j]

# bo dong da bi chu thich (-- o dau dong)
sach = "\n".join(l for l in than.split("\n") if not l.strip().startswith("--"))

khoang = []
for a, b in re.findall(r"nSubWorldID\s*>=\s*(\d+)\s*and\s*nSubWorldID\s*<=\s*(\d+)", sach):
    khoang.append((int(a), int(b)))
for a in re.findall(r"nSubWorldID\s*==\s*(\d+)", sach):
    khoang.append((int(a), int(a)))
assert khoang, "khong doc duoc so map nao - script doi dang, DUNG LAI"

# gop khoang
khoang.sort()
gop = []
for a, b in khoang:
    if gop and a <= gop[-1][1] + 1:
        gop[-1][1] = max(gop[-1][1], b)
    else:
        gop.append([a, b])

L = []
w = L.append
w("// ============================================================================")
w("// KTuiDuocPham.h - SINH TU DONG boi ReverseTools/gen_tuiduocpham.py - DUNG SUA TAY.")
w("// Danh sach BAN DO KHONG MO DUOC \"Tui duoc pham\" (genre 6 / detail 1 /")
w("// particular 4813) - doc thang tu ham main() cua")
w("//   bin\\server\\script\\item\\tuiduocpham.lua")
w("// (script do liet ke tay chu KHONG goi checkSJMaps, nen phai lay dung tu no).")
w("//")
w("// Dung o ATYPE_OPENBAG: dang dung tren map trong danh sach nay thi ImKHONG gui")
w("// lenh mo - khong thi cu 3 giay lai an mot cau \"Ban do hien tai... khong the mo!\".")
w("// ============================================================================")
w("#ifndef KTUIDUOCPHAM_H")
w("#define KTUIDUOCPHAM_H")
w("")
w("#define TUIDP_ITEM_G\t6")
w("#define TUIDP_ITEM_D\t1")
w("#define TUIDP_ITEM_P\t4813")
w("")
w("#define TUIDP_CAM_COUNT\t%d" % len(gop))
w("static const short g_TuiDPCam[TUIDP_CAM_COUNT][2] = {")
for a, b in gop:
    w("\t{ %d, %d }," % (a, b))
w("};")
w("")
w("// 1 = ban do nay KHONG mo duoc tui duoc pham")
w("static inline int TuiDP_CamMap(int nMapId)")
w("{")
w("\tfor (int i = 0; i < TUIDP_CAM_COUNT; ++i)")
w("\t\tif (nMapId >= (int)g_TuiDPCam[i][0] && nMapId <= (int)g_TuiDPCam[i][1])")
w("\t\t\treturn 1;")
w("\treturn 0;")
w("}")
w("")
w("#endif // KTUIDUOCPHAM_H")

io.open(OUT, "w", encoding="latin-1", newline="\r\n").write("\n".join(L) + "\n")
print("Da sinh %s" % OUT)
print("So khoang map cam: %d" % len(gop))
print("  " + ", ".join("%d..%d" % (a, b) if a != b else "%d" % a for a, b in gop))
