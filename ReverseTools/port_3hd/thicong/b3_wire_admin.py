# -*- coding: utf-8 -*-
"""Noi hd3_admin vao lenhbaiadmin.lua: 1 dong Include + 1 muc menu.
Byte-an toan (latin-1). Menu entry co tieng Viet -> ma hoa TCVN3."""
import io, os, sys
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
rel = "script/item/lenhbaiadmin.lua"


def V(s):
    return unicode_to_tcvn3_bytes(s).decode("latin-1")


p = os.path.join(JX1, rel.replace("/", os.sep))
d = io.open(p, encoding="latin-1", newline="").read()
if "hd3_admin.lua" in d:
    print("da noi roi")
    raise SystemExit

# 1) Include: chen NGAY SAU dong hoatdong_admin (match dau dong, giu duoi nguyen)
anchor = 'Include("\\\\script\\\\item\\\\hoatdong_admin.lua")'
assert d.count(anchor) == 1, "anchor Include=%d" % d.count(anchor)
i = d.index(anchor)
eol = d.index("\n", i)
new_inc = '\r\nInclude("\\\\script\\\\item\\\\hd3_admin.lua")\t-- [3HD 25/08] menu test 3 hoat dong Linux'
if d[eol - 1] == "\r":
    d = d[:eol - 1] + new_inc + d[eol - 1:]
else:
    d = d[:eol] + new_inc + d[eol:]

# 2) muc menu: chen truoc dong "Ket thuc doi thoai/no" trong chucnangadmin.
#    Dong do dang: "<V ket thuc>/no"})   -- tim theo duoi ASCII '/no"})'
menu_item = '\t"' + V("Hoạt động Linux (Săn Boss Sát Thủ / Phong Lăng Độ / Vượt ải): test") + '/HD3_AdminMenu",\r\n'
end_anchor = '/no"})'
assert d.count(end_anchor) >= 1, "khong thay /no\"})"
# lay lan xuat hien dau tien SAU "Hoat dong 23-24.08" (menu chinh)
mark = "HD_AdminMenu"
pos_mark = d.index(mark)
pos_end = d.index(end_anchor, pos_mark)
line_start = d.rfind("\n", 0, pos_end) + 1
d = d[:line_start] + menu_item + d[line_start:]

io.open(p, "w", encoding="latin-1", newline="").write(d)
m = os.path.join(MIRROR, rel.replace("/", os.sep))
os.makedirs(os.path.dirname(m), exist_ok=True)
io.open(m, "w", encoding="latin-1", newline="").write(d)
print("da noi Include + muc menu HD3_AdminMenu")
