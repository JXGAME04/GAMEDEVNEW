# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] PORT DU LIEU he TRANG BI DONG HANH tu nguon VLTK:
Nguon da moi ra:
 - bang item trang bi: entry 0xdf37e2dc trong slistcache.pak -> 20 mon
   (Genre 12, DetailType 0..9 = vi tri, ParticularType 0/1 = bo)
   Bo 0 "Bich Huyet", bo 1 "Kim Lan" (chuoi G_STR_COMPANION_SUIT_3 con
   bo 3 "Dan Tam" - ban private cap nhat sau, chua co item).
 - \settings\companionequip\suitattrib.txt: thuoc tinh BO theo bac
   (233=5000/7500/10000, 308=30/40/50, 311=1/2/3 = cap Van Khoi Long Tuong)
 - item lien quan: 5063 Ket Tinh Dong Hanh, 5257 Ket Tinh (Cao),
   5064/65/66 Ruong trang bi 1/2/3, 5067 Chia khoa ruong
 - cua so duc lai: ini 720a151f (2 danh sach cu/moi + 3 nut + o objEquip)

JX1 chi co 7 ItemGenre (khong co 12) -> trang bi pet dua vao bang
magicscript (genre 6) nhung GIU nguyen ten/anh goc; vi tri + bo luu o
bang phu script\petsys\petequip_def.lua (sinh tu dong tu bang goc).
Id JX1: 4881..4900 = 20 mon; 4901..4906 = ket tinh/ruong/chia khoa.
"""
import io
import os
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
import pakdump as P  # noqa: E402
import ucl  # noqa: E402
from vn_edit import doc_vn  # noqa: E402

CR = chr(13)
BS = chr(92)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
VL = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"
RA = r"D:\GAMEDEVNEW\ReverseTools\bandonghanh\ra_pet"


def doc_lf(p):
    return io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")


def ghi_crlf(p, lf):
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))


def blob(f, e):
    u, off, size, cf = e
    cs = cf & 0xFFFFFF
    fl = cf >> 24
    if fl == 0x20:
        fl = 1
    fl &= 0x0F
    f.seek(off)
    raw = f.read(cs if cs else size)
    try:
        if fl == 0 or cs in (0, size):
            return raw[:size] if size > 0 else raw
        if fl == 1:
            return ucl.nrv2b_decompress_8(raw, size)
        import bz2
        import zlib
        return bz2.decompress(raw) if fl == 2 else zlib.decompress(raw)
    except Exception:
        return b""


# ---------- 1. doc bang goc 20 mon ----------
goc = [d.split("\t") for d in
       io.open(os.path.join(RA, "vltk_eq_item.txt"), "r", encoding="latin-1").read()
       .replace(CR, "").split("\n") if d.strip()][1:]
print("1. bang goc:", len(goc), "mon")

# ---------- 2. lay 5 item phu tu bang VLTK 004 ----------
vl4 = {c[3]: c for c in [d.split("\t") for d in
       io.open(os.path.join(RA, "vltk_item", "004.txt"), "r", encoding="latin-1").read()
       .replace(CR, "").split("\n")] if len(c) > 8}
PHU = [("5063", 4901), ("5257", 4902), ("5064", 4903), ("5065", 4904),
       ("5066", 4905), ("5067", 4906)]

# ---------- 3. rut anh (ten GBK) ----------
anh_can = set()
for c in goc:
    anh_can.add(c[4])
for idv, _ in PHU:
    if idv in vl4:
        anh_can.add(vl4[idv][4])
thieu = [a for a in anh_can if not os.path.exists(CL + a)]
ids = {P.name2id(a): a for a in thieu}
print("2. anh can:", len(anh_can), "| thieu loose:", len(thieu))
THU_TU = ["1024.pak", "serverlist.pak", "slistcache.pak"] + \
    ["updatejx%02d.pak" % i for i in range(17, 0, -1)] + \
    ["updatejx07_jxf06.pak", "update02.pak", "update.pak", "update04.pak",
     "update03.pak", "update01.pak", "spr.pak", "resource.pak"]
rut = 0
for pak in THU_TU:
    p = os.path.join(VL, pak)
    if not os.path.exists(p) or not ids:
        continue
    f, es = P.entries(p)
    for e in es:
        if e[0] in ids:
            d = blob(f, e)
            duong = ids.pop(e[0])
            ra = CL + duong
            os.makedirs(os.path.dirname(ra), exist_ok=True)
            open(ra, "wb").write(d)
            rut += 1
    f.close()
print("3. rut anh tu VLTK:", rut, "| con thieu:", len(ids))

# ---------- 4. them vao 2 bang JX1 ----------
BO_TEN = {0: "Bich Huyet", 1: "Kim Lan", 2: "Dan Tam"}
VITRI = ["Vu khi", "Y phuc", "Nhan", "Ho uyen", "Ho than phu",
         "Thuc yeu", "Day chuyen", "Chien ngoa", "Yeu truy", "Non"]
ban = []
for k, c in enumerate(goc):
    ban.append({"id": 4881 + k, "ten": c[0], "anh": c[4], "mota": c[8] if len(c) > 8 else "",
                "vitri": int(c[2]), "bo": int(c[3])})
for pth in (SV + r"\settings\item\magicscript.txt", CL + r"\settings\item\magicscript.txt"):
    rows = [d.split("\t") for d in doc_lf(pth).split("\n") if d.strip()]
    dang_co = {c[3] for c in rows if len(c) > 3}
    khuon = None
    for c in rows:
        if len(c) > 12 and c[3] == "4874":
            khuon = c[:]
    assert khuon
    them = 0
    for it in ban:
        if str(it["id"]) in dang_co:
            continue
        c = khuon[:]
        c[0] = it["ten"]
        c[3] = str(it["id"])
        c[4] = it["anh"]
        c[8] = it["mota"] or it["ten"]
        c[9] = BS + "script" + BS + "petsys" + BS + "petequip.lua"
        c[12] = "1"
        rows.append(c)
        them += 1
    for idv, idj in PHU:
        if str(idj) in dang_co or idv not in vl4:
            continue
        g = vl4[idv]
        c = khuon[:]
        c[0] = g[0]
        c[3] = str(idj)
        c[4] = g[4]
        c[8] = g[8]
        c[9] = "" if idv in ("5063", "5257") else BS + "script" + BS + "petsys" + BS + "petbox.lua"
        c[12] = "200"
        rows.append(c)
        them += 1
    ghi_crlf(pth, "\n".join("\t".join(c) for c in rows) + "\n")
    print("4. them", them, "item:", pth[:2])

# ---------- 5. bang phu petequip_def.lua ----------
dong = []
for it in ban:
    dong.append("\t[%d] = {nSlot = %d, nSuit = %d},\t-- %s %s"
                % (it["id"], it["vitri"] + 1, it["bo"],
                   BO_TEN.get(it["bo"], "?"), VITRI[it["vitri"]]))
noidung = "\n".join([
    "-- [PETSYS 29/08] SINH TU DONG tu bang goc VLTK (entry 0xdf37e2dc):",
    "-- 20 mon trang bi Dong Hanh - Genre 12 goc, JX1 nan sang 4881..4900.",
    "-- nSlot 1..10 = vi tri (Vu khi/Y phuc/Nhan/Ho uyen/Ho than phu/Thuc yeu/",
    "-- Day chuyen/Chien ngoa/Yeu truy/Non); nSuit 0=Bich Huyet 1=Kim Lan.",
    "PETEQUIP_DEF = {",
    "\n".join(dong),
    "}",
    "",
    "PETEQUIP_SUIT_TEN = {",
    '\t[0] = "Bich Huyet",',
    '\t[1] = "Kim Lan",',
    '\t[2] = "Dan Tam",',
    "}",
    "",
    "-- nguyen lieu duc lai (goc: Dong Hanh Ket Tinh)",
    "PETEQUIP_KETTINH = 4901",
    "PETEQUIP_KETTINH_CAO = 4902",
    "",
])
ghi_crlf(SV + r"\script\petsys\petequip_def.lua", noidung)
print("5. petequip_def.lua:", len(ban), "mon")

# ---------- 6. suitattrib ----------
src = os.path.join(RA, "companionequip", "suitattrib.txt")
dst = os.path.join(SV, "settings", "petsys", "suitattrib.txt")
if os.path.exists(src):
    io.open(dst, "w", encoding="latin-1", newline="").write(
        io.open(src, "r", encoding="latin-1", newline="").read())
    print("6. chep suitattrib.txt")
print("XONG p62")
