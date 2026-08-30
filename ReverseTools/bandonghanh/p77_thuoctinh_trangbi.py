# -*- coding: utf-8 -*-
r"""[PETSYS 30/08] TRANG BI DONG HANH - port DUNG bang goc:
Doc lai bang goc (24 cot) thi moi mon co 3 THUOC TINH:
  c15/c16/c17 = loai1 / min1 / max1
  c18/c19/c20 = loai2 / min2 / max2
  c21/c22/c23 = loai3 / min3 / max3
Vi du "Bich Huyet Nhan": 268 (8..10), 276 (3..4), 280 (3..4)
  268 = magic_anti_block_rate, 276/280 = magic_addskilldamage2/6
=> Day chinh la thu ma cua so "Duc lai" roll (ListOldSkill/ListNewSkill).

Sinh petequip_def.lua co du 3 thuoc tinh moi mon; o task luu gia tri da
roll: 5170..5199 (10 mon x 3 gia tri) - MAX_TASK 5200 vua du.
"""
import io
import os

CR = chr(13)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
RA = r"D:\GAMEDEVNEW\ReverseTools\bandonghanh\ra_pet"

goc = [d.split("\t") for d in
       io.open(os.path.join(RA, "vltk_eq_item.txt"), "r", encoding="latin-1").read()
       .replace(CR, "").split("\n") if d.strip()][1:]

BO_TEN = {0: "Bich Huyet", 1: "Kim Lan", 2: "Dan Tam"}
VITRI = ["Vu khi", "Y phuc", "Nhan", "Ho uyen", "Ho than phu",
         "Thuc yeu", "Day chuyen", "Chien ngoa", "Yeu truy", "Non"]

dong = []
for k, c in enumerate(goc):
    idj = 4907 + k
    at = []
    for j in (15, 18, 21):
        loai = c[j].strip() if len(c) > j else "-1"
        vmin = c[j + 1].strip() if len(c) > j + 1 else "0"
        vmax = c[j + 2].strip() if len(c) > j + 2 else "0"
        if loai.lstrip("-").isdigit() and int(loai) > 0:
            at.append("{%s, %s, %s}" % (loai, vmin, vmax))
    dong.append("\t[%d] = {nSlot = %d, nSuit = %d, tbAttrib = {%s}},\t-- %s %s"
                % (idj, int(c[2]) + 1, int(c[3]), ", ".join(at),
                   BO_TEN.get(int(c[3]), "?"), VITRI[int(c[2])]))

noi = "\n".join([
    "-- [PETSYS 30/08] SINH TU DONG tu bang goc VLTK (slistcache 0xdf37e2dc).",
    "-- Moi mon co 3 THUOC TINH: {ma_attrib, gia_tri_min, gia_tri_max}",
    "-- (cot 16..24 cua bang goc). Duc lai = roll lai 3 gia tri trong khoang.",
    "-- Ma attrib theo enum KMagicAttrib.h cua engine (vd 268 =",
    "-- anti_block_rate, 276/280 = addskilldamage2/6).",
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
    "PETEQUIP_VITRI = {",
    "\n".join('\t[%d] = "%s",' % (i + 1, VITRI[i]) for i in range(10)),
    "}",
    "",
    "-- o task: 5143..5152 = id mon dang deo; 5170..5199 = 3 gia tri da roll",
    "-- cua tung mon (mon i -> 5170 + (i-1)*3 .. +2)",
    "PETEQUIP_O_DAU = 5143",
    "PETEQUIP_O_SO = 10",
    "PETEQUIP_O_BOCOUNT = 5163",
    "PETEQUIP_O_ATTRIB = 5170",
    "",
    "-- nguyen lieu / nguon do (bang goc VLTK)",
    "PETEQUIP_KETTINH = 4927",
    "PETEQUIP_KETTINH_CAO = 4928",
    "PETEQUIP_RUONG = {4929, 4930, 4931}",
    "PETEQUIP_CHIAKHOA = 4932",
    "",
])
io.open(SV + r"\script\petsys\petequip_def.lua", "w", encoding="latin-1",
        newline="").write(noi.replace("\n", CR + "\n"))
print("da sinh petequip_def.lua voi 3 thuoc tinh moi mon")
print("vi du dong dau:", dong[0][:110])
