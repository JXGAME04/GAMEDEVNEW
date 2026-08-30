# -*- coding: utf-8 -*-
r"""[PETSYS 30/08] Chu: re chuot vao trang bi TRONG TUI khong thay thuoc
tinh. Do la tooltip do ENGINE ve theo bang item: chi co Ten + cot Intro,
ma Intro cua 20 mon dang de trong (bang ta ghi = ten mon).
Bang goc VLTK cho moi mon 3 thuoc tinh {ma, min, max} -> viet han vao
Intro (dinh dang xuong dong cua JX1 la "<enter>", xem item 2706).
Vi du: "Bo Bich Huyet - Vu khi<enter>Xuyen do don +8~10<enter>..."
"""
import io
import re
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

CR = chr(13)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"

# ten thuoc tinh tieng Viet (bang da sinh o p80)
ten_att = {}
for d in io.open(SV + r"\settings\petsys\attribname.txt", "r", encoding="latin-1").read().replace(CR, "").split("\n")[1:]:
    c = d.split("\t")
    if len(c) >= 2 and c[0].strip().isdigit():
        ten_att[int(c[0])] = c[1]

# bang dinh nghia mon (id -> slot/suit/3 thuoc tinh)
info = {}
lf = io.open(SV + r"\script\petsys\petequip_def.lua", "r", encoding="latin-1").read().replace(CR, "")
for dong in lf.split("\n"):
    m = re.match(r"\s*\[(\d+)\] = \{nSlot = (\d+), nSuit = (\d+), tbAttrib = \{(.*)\}\},\s*--\s*(.*)$", dong)
    if not m:
        continue
    at = re.findall(r"\{(\d+),\s*(\d+),\s*(\d+)\}", m.group(4))
    info[int(m.group(1))] = {"slot": int(m.group(2)), "suit": int(m.group(3)),
                             "att": at, "ghi": m.group(5).strip()}
print("doc duoc", len(info), "mon")

SUIT = {0: vn("Bích Huyết"), 1: vn("Kim Lân"), 2: vn("Đan Tâm")}
VITRI = [vn("Vũ khí"), vn("Y phục"), vn("Nhẫn"), vn("Hộ uyển"), vn("Hộ thân phù"),
         vn("Thúc yêu"), vn("Dây chuyền"), vn("Chiến ngoa"), vn("Yêu trụy"), vn("Nón")]
XUONG = "<enter>"

for pth in (SV + r"\settings\item\magicscript.txt", CL + r"\settings\item\magicscript.txt"):
    raw = io.open(pth, "r", encoding="latin-1", newline="").read()
    rows = [d.split("\t") for d in raw.replace(CR + "\n", "\n").split("\n") if d.strip()]
    n = 0
    for c in rows:
        if len(c) < 9 or not c[3].isdigit():
            continue
        it = info.get(int(c[3]))
        if not it:
            continue
        dong = [vn("Trang bị Bạn Đồng Hành - bộ ") + (SUIT.get(it["suit"]) or "?")
                + " - " + VITRI[it["slot"] - 1]]
        for ma, vmin, vmax in it["att"]:
            ten = ten_att.get(int(ma), "attrib %s" % ma)
            dong.append("%s +%s~%s" % (ten, vmin, vmax))
        dong.append(vn("Dùng để mặc cho bạn đồng hành; có thể Đúc lại bằng Kết Tinh Đồng Hành"))
        c[8] = XUONG.join(dong)
        n += 1
    io.open(pth, "w", encoding="latin-1", newline="").write(
        ("\n".join("\t".join(x) for x in rows) + "\n").replace("\n", CR + "\n"))
    print("da ghi mo ta cho", n, "mon:", pth[:2])
