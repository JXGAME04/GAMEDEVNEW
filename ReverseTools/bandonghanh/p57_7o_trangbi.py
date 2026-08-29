# -*- coding: utf-8 -*-
r"""[PETSYS 29/08 chieu] Sua theo do THAT tren anh nen + INI goc VLTK:
1. Do anh nen pet_main_1.spr: 7 O TRANG BI hang tren = x 340,376,412,448,
   484,520,556 | y=81 | 26x26 (chu: "trang bi la 7 o phia tren").
   Hang ky nang = x 376..520 buoc 36 | y=167 | 26x26.
2. INI: sua Skill_1 + Ext_Skill_1..4 ve 26x26 dung toa do; THEM 7 section
   PetEquip_1..7; CompanionBtn ("Duc lai") THIEU Width/Height -> vung bam
   = 0 => "nut duc lai khong bam duoc" -> them 62x24.
3. UiPet: 6 -> 7 o, doc PetEquip_%d, anh theo duong dan CO DINH
   \spr\item\petequip\pet_%d.spr (bo tra bang KTabFile - khong an).
4. Item: them 4887 Nhan Dong Hanh (7 loai); petequip.lua 7 slot
   (task 5143..5149, pct 5150..5156).
"""
import io
import os
import re
import shutil
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

CR = chr(13)
BS = chr(92)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"


def doc_lf(p):
    return io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")


def ghi_crlf(p, lf):
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))


# ---------- 1+2. INI ----------
p = CL + r"\Ui\Ui3\pet_main.ini"
s = io.open(p, "r", encoding="latin-1", newline="").read()


def doi(s, sec, l, t, w, h):
    i = s.find("[" + sec + "]")
    if i < 0:
        return s
    j = s.find("[", i + 1)
    kh = s[i:j] if j > 0 else s[i:]
    m = kh
    m = re.sub(r"Left=\d+", "Left=%d" % l, m, 1)
    m = re.sub(r"Top=\d+", "Top=%d" % t, m, 1)
    if "Width=" in m:
        m = re.sub(r"Width=\d+", "Width=%d" % w, m, 1)
    else:
        m = m.replace("Top=%d" % t, "Top=%d%sWidth=%d" % (t, CR + "\n", w), 1)
    if "Height=" in m:
        m = re.sub(r"Height=\d+", "Height=%d" % h, m, 1)
    else:
        m = m.replace("Width=%d" % w, "Width=%d%sHeight=%d" % (w, CR + "\n", h), 1)
    return s.replace(kh, m, 1)


s = doi(s, "Skill_1", 376, 167, 26, 26)
for k in range(4):
    s = doi(s, "Ext_Skill_%d" % (k + 1), 412 + 36 * k, 167, 26, 26)
s = doi(s, "CompanionBtn", 235, 390, 62, 24)
if "[PetEquip_1]" not in s:
    them = []
    for k in range(7):
        them.append("[PetEquip_%d]%sLeft=%d%sTop=81%sWidth=26%sHeight=26%s" %
                    (k + 1, CR + "\n", 340 + 36 * k, CR + "\n", CR + "\n", CR + "\n", CR + "\n"))
    s = s.rstrip() + CR + "\n" + (CR + "\n").join(them) + CR + "\n"
io.open(p, "w", encoding="latin-1", newline="").write(s)
print("1+2. INI: 5 o ky nang 26x26 (y=167) + 7 o PetEquip (y=81) + nut Duc lai 62x24")

# ---------- 3. anh 7 slot ----------
ra = os.path.join(CL, "spr", "item", "petequip")
CU = ["pet_weapon.spr", "pet_cap.spr", "pet_armor.spr", "pet_sash.spr",
      "pet_boots.spr", "pet_neck.spr"]
for i, f in enumerate(CU):
    a = os.path.join(ra, f)
    b = os.path.join(ra, "pet_%d.spr" % (i + 1))
    if os.path.exists(a) and not os.path.exists(b):
        shutil.copyfile(a, b)
# o 7 (nhan): tam dung anh day chuyen
b7 = os.path.join(ra, "pet_7.spr")
if not os.path.exists(b7) and os.path.exists(os.path.join(ra, "pet_neck.spr")):
    shutil.copyfile(os.path.join(ra, "pet_neck.spr"), b7)
print("3. anh pet_1..7.spr:", len([f for f in os.listdir(ra) if re.match(r"pet_\d\.spr", f)]))

# ---------- 4. item 4887 + bang ----------
TEN7 = [vn("Vũ khí Đồng Hành"), vn("Nón Đồng Hành"), vn("Y phục Đồng Hành"),
        vn("Đai Đồng Hành"), vn("Giày Đồng Hành"), vn("Hộ Phù Đồng Hành"),
        vn("Nhẫn Đồng Hành")]
for pth in (SV + r"\settings\item\magicscript.txt", CL + r"\settings\item\magicscript.txt"):
    rows = [d.split("\t") for d in doc_lf(pth).split("\n") if d.strip()]
    co4887 = any(len(c) > 3 and c[3] == "4887" for c in rows)
    khuon = None
    for c in rows:
        if len(c) > 9 and c[3] == "4881":
            khuon = c[:]
    for c in rows:
        if len(c) > 4 and c[3] in [str(x) for x in range(4881, 4888)]:
            k = int(c[3]) - 4881
            c[0] = TEN7[k]
            c[4] = BS + "spr" + BS + "item" + BS + "petequip" + BS + "pet_%d.spr" % (k + 1)
    if not co4887 and khuon:
        c = khuon[:]
        c[0] = TEN7[6]
        c[3] = "4887"
        c[4] = BS + "spr" + BS + "item" + BS + "petequip" + BS + "pet_7.spr"
        rows.append(c)
    ghi_crlf(pth, "\n".join("\t".join(x) for x in rows) + "\n")
    print("4. item 4881..4887:", pth[:2])

# ---------- 5. petequip.lua 7 slot ----------
p = SV + r"\script\petsys\petequip.lua"
lf = doc_lf(p)
if "4887" not in lf:
    lf = lf.replace("\t[4886] = {nSlot = 6, nHp = 0, nMp = 300},",
                    "\t[4886] = {nSlot = 6, nHp = 0, nMp = 300},\n"
                    "\t[4887] = {nSlot = 7, nHp = 150, nMp = 150},")
    lf = lf.replace('PETEQUIP_TEN = {"', 'PETEQUIP_TEN = {"')
    lf = re.sub(r'(PETEQUIP_TEN = \{[^}]*)\}', r'\1, "' + vn("Nhẫn") + '"}', lf, 1)
    lf = lf.replace("for i = 1, 6 do", "for i = 1, 7 do")
    ghi_crlf(p, lf)
    print("5. petequip.lua 7 slot")
else:
    print("5. da co")
print("XONG p57")
