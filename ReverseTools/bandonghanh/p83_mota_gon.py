# -*- coding: utf-8 -*-
r"""[PETSYS 30/08] Sua 3 loi hien thi mo ta trang bi (anh chu gui):
1. "<enter>" HIEN THO: tag do ENGINE tu chen khi tu xuong dong
   (KItem.cpp:2662) chu khong phai de script viet tay -> BO het tag,
   de engine tu wrap.
2. Ten thuoc tinh con tieng Anh (returnskill2enemy, addblockrate...)
   -> dich not 15 ma con lai.
3. "+3~4" hien thanh "+3^4" (font khong co dau ~) -> doi sang "3-4".
"""
import io
import re
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

CR = chr(13)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"

# ---------- 1. dich not ten thuoc tinh ----------
DICH = {
    97: vn("Thân pháp"), 98: vn("Sinh lực gốc"), 99: vn("Nội lực gốc"),
    100: vn("Kháng độc"), 190: vn("Sát thương bổ trợ"),
    226: vn("Kháng độc"), 227: vn("Kháng lôi"), 236: vn("Uy lực trừng phạt"),
    246: vn("Xuyên kháng lôi"), 248: vn("Giảm thời gian hồi chiêu"),
    269: vn("Tỷ lệ đỡ đòn"), 270: vn("Tốc độ di chuyển"),
    271: vn("Phản đòn kẻ địch"), 284: vn("Tầm đánh xa"),
    285: vn("Tầm bắn"), 308: vn("Uy lực Đồng Hành"),
}
p = SV + r"\settings\petsys\attribname.txt"
rows = [d.split("\t") for d in io.open(p, "r", encoding="latin-1").read().replace(CR, "").split("\n") if d.strip()]
n = 0
for c in rows[1:]:
    if len(c) >= 2 and c[0].strip().isdigit():
        ma = int(c[0])
        if ma in DICH:
            c[1] = DICH[ma]
            n += 1
noi = ("\n".join("\t".join(c) for c in rows) + "\n").replace("\n", CR + "\n")
io.open(p, "w", encoding="latin-1", newline="").write(noi)
io.open(CL + r"\settings\petsys\attribname.txt", "w", encoding="latin-1", newline="").write(noi)
print("1. dich them", n, "ten thuoc tinh (ca 2 ben)")

# ---------- 2. viet lai mo ta: bo <enter>, doi ~ thanh - ----------
ten_att = {}
for d in io.open(p, "r", encoding="latin-1").read().replace(CR, "").split("\n")[1:]:
    c = d.split("\t")
    if len(c) >= 2 and c[0].strip().isdigit():
        ten_att[int(c[0])] = c[1]

info = {}
lf = io.open(SV + r"\script\petsys\petequip_def.lua", "r", encoding="latin-1").read().replace(CR, "")
for dong in lf.split("\n"):
    m = re.match(r"\s*\[(\d+)\] = \{nSlot = (\d+), nSuit = (\d+), tbAttrib = \{(.*)\}\},", dong)
    if m:
        info[int(m.group(1))] = {
            "slot": int(m.group(2)), "suit": int(m.group(3)),
            "att": re.findall(r"\{(\d+),\s*(\d+),\s*(\d+)\}", m.group(4))}

SUIT = {0: vn("Bích Huyết"), 1: vn("Kim Lân"), 2: vn("Đan Tâm")}
VITRI = [vn("Vũ khí"), vn("Y phục"), vn("Nhẫn"), vn("Hộ uyển"), vn("Hộ thân phù"),
         vn("Thúc yêu"), vn("Dây chuyền"), vn("Chiến ngoa"), vn("Yêu trụy"), vn("Nón")]

for pth in (SV + r"\settings\item\magicscript.txt", CL + r"\settings\item\magicscript.txt"):
    raw = io.open(pth, "r", encoding="latin-1", newline="").read()
    rows2 = [d.split("\t") for d in raw.replace(CR + "\n", "\n").split("\n") if d.strip()]
    k = 0
    for c in rows2:
        if len(c) < 9 or not c[3].isdigit():
            continue
        it = info.get(int(c[3]))
        if not it:
            continue
        phan = [vn("Trang bị Bạn Đồng Hành - bộ ") + (SUIT.get(it["suit"]) or "?")
                + " (" + VITRI[it["slot"] - 1] + ")."]
        for ma, vmin, vmax in it["att"]:
            phan.append("%s +%s-%s." % (ten_att.get(int(ma), "attrib %s" % ma), vmin, vmax))
        phan.append(vn("Đúc lại bằng Kết Tinh Đồng Hành."))
        c[8] = " ".join(phan)     # KHONG dung <enter> - engine tu xuong dong
        k += 1
    io.open(pth, "w", encoding="latin-1", newline="").write(
        ("\n".join("\t".join(x) for x in rows2) + "\n").replace("\n", CR + "\n"))
    print("2. viet lai mo ta", k, "mon:", pth[:2])
