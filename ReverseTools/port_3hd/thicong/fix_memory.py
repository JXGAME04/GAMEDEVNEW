# -*- coding: utf-8 -*-
"""Sua 4 chuoi bi bay 'bash heredoc nuot backslash' lam hong trong tep memory."""
import io

p = r"C:\Users\nguye\.claude\projects\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\memory\jx1-3hoatdong-dichnguoc-2408.md"
d = io.open(p, encoding="utf-8").read()

BS = chr(92)
TAB = chr(9)
FF = chr(12)

fixes = [
    ("scriptjx2" + TAB + "ong_vn", "scriptjx2" + BS + "tong_vn"),
    ("config" + BS + "config!", "config" + BS + "41"),          # phong truong hop
    ("`activitysys" + BS + "config!`", "`activitysys" + BS + "config" + BS + "41`"),
    ("header" + FF + "orbidmap.lua", "header" + BS + "forbidmap.lua"),
    ("settings" + TAB + "ask" + BS + "missions.txt", "settings" + BS + "task" + BS + "missions.txt"),
]
n = 0
for a, b in fixes:
    if a in d:
        d = d.replace(a, b)
        n += 1
        print("  sua:", repr(a[:40]), "->", repr(b[:40]))

io.open(p, "w", encoding="utf-8", newline="\n").write(d)
print("da sua", n, "chuoi")

# kiem con ky tu dieu khien la khong
bad = [(i, repr(c)) for i, c in enumerate(d) if c in (FF, chr(11), chr(7), chr(8))]
print("ky tu dieu khien con lai:", bad[:5] if bad else "khong con")
# kiem cac chuoi quan trong
for s in ["header" + BS + "forbidmap.lua", "scriptjx2" + BS + "tong_vn",
          "settings" + BS + "task" + BS + "missions.txt", "activitysys" + BS + "config" + BS + "41"]:
    print("  co '%s':" % s, s in d)
