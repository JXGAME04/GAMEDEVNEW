# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] Don dep + nan id dung day TRONG:
- Lan chay p62 dau tien them nham vao 4881..4900 nhung JX1 DA CO item khac
  o 4881..4889 (Tinh Ngoc, Thien Tinh Ngoc, Vuong Thiet Tuong Lenh Phu...)
  -> chi 11 mon vao duoc (4890..4900).
- Day THAT SU trong: tu 4907 tro di (id max hien tai 4906).
=> Go het dong trang bi pet da them, nan lai:
   4907..4926 = 20 mon trang bi (Bich Huyet 10 + Kim Lan 10)
   4927..4932 = Ket Tinh / Ket Tinh Cao / Ruong 1-3 / Chia khoa
   (6 item phu truoc lo them o 4901..4906 -> cung go, dat lai cho lien mach)
"""
import io
import os
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn, doc_vn  # noqa: E402

CR = chr(13)
BS = chr(92)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
RA = r"D:\GAMEDEVNEW\ReverseTools\bandonghanh\ra_pet"
K1, K2 = vn("Bích Huyết"), vn("Kim Lân")
PHU_TEN = [vn("Kết Tinh Đồng Hành"), vn("Rương"), vn("Chìa Khóa Rương")]


def doc_lf(p):
    return io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")


def ghi_crlf(p, lf):
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))


goc = [d.split("\t") for d in
       io.open(os.path.join(RA, "vltk_eq_item.txt"), "r", encoding="latin-1").read()
       .replace(CR, "").split("\n") if d.strip()][1:]
vl4 = {c[3]: c for c in [d.split("\t") for d in
       io.open(os.path.join(RA, "vltk_item", "004.txt"), "r", encoding="latin-1").read()
       .replace(CR, "").split("\n")] if len(c) > 8}
PHU_SRC = ["5063", "5257", "5064", "5065", "5066", "5067"]

for pth in (SV + r"\settings\item\magicscript.txt", CL + r"\settings\item\magicscript.txt"):
    rows = [d.split("\t") for d in doc_lf(pth).split("\n") if d.strip()]
    # 1. go moi dong trang bi pet + 6 item phu da them (4890..4906)
    def la_cua_pet(c):
        if len(c) < 4 or not c[3].isdigit():
            return False
        n = int(c[3])
        if 4890 <= n <= 4906:
            return True
        return K1 in c[0] or K2 in c[0]
    giu = [c for c in rows if not la_cua_pet(c)]
    bo = len(rows) - len(giu)
    khuon = None
    for c in giu:
        if len(c) > 12 and c[3] == "4874":
            khuon = c[:]
    assert khuon
    # 2. them lai o day trong 4907+
    them = 0
    for k, g in enumerate(goc):
        c = khuon[:]
        c[0] = g[0]
        c[3] = str(4907 + k)
        c[4] = g[4]
        c[8] = g[8] if len(g) > 8 and g[8].strip() else g[0]
        c[9] = BS + "script" + BS + "petsys" + BS + "petequip.lua"
        c[12] = "1"
        giu.append(c)
        them += 1
    for k, idv in enumerate(PHU_SRC):
        g = vl4.get(idv)
        if not g:
            continue
        c = khuon[:]
        c[0] = g[0]
        c[3] = str(4927 + k)
        c[4] = g[4]
        c[8] = g[8]
        c[9] = "" if idv in ("5063", "5257") else BS + "script" + BS + "petsys" + BS + "petbox.lua"
        c[12] = "200"
        giu.append(c)
        them += 1
    ghi_crlf(pth, "\n".join("\t".join(c) for c in giu) + "\n")
    print("%s: go %d dong cu, them %d dong (4907..4932)" % (pth[:2], bo, them))

# 3. sinh lai bang phu
BO_TEN = {0: "Bich Huyet", 1: "Kim Lan", 2: "Dan Tam"}
VITRI = ["Vu khi", "Y phuc", "Nhan", "Ho uyen", "Ho than phu",
         "Thuc yeu", "Day chuyen", "Chien ngoa", "Yeu truy", "Non"]
dong = []
for k, g in enumerate(goc):
    dong.append("\t[%d] = {nSlot = %d, nSuit = %d},\t-- %s %s"
                % (4907 + k, int(g[2]) + 1, int(g[3]),
                   BO_TEN.get(int(g[3]), "?"), VITRI[int(g[2])]))
noidung = "\n".join([
    "-- [PETSYS 29/08] SINH TU DONG tu bang goc VLTK (slistcache entry",
    "-- 0xdf37e2dc): 20 mon trang bi Dong Hanh, goc Genre 12 / DetailType",
    "-- 0..9 = vi tri / ParticularType 0..1 = bo. JX1 nan sang 4907..4926.",
    "PETEQUIP_DEF = {",
    "\n".join(dong),
    "}",
    "",
    "PETEQUIP_SUIT_TEN = {",
    '\t[0] = "' + vn("Bích Huyết") + '",',
    '\t[1] = "' + vn("Kim Lân") + '",',
    '\t[2] = "' + vn("Đan Tâm") + '",',
    "}",
    "",
    "PETEQUIP_VITRI = {",
    "\n".join('\t[%d] = "%s",' % (i + 1, VITRI[i]) for i in range(10)),
    "}",
    "",
    "-- nguyen lieu duc lai va nguon do (theo bang goc VLTK)",
    "PETEQUIP_KETTINH = 4927\t\t-- Ket Tinh Dong Hanh (goc 5063)",
    "PETEQUIP_KETTINH_CAO = 4928\t-- Ket Tinh Dong Hanh (Cao) (goc 5257)",
    "PETEQUIP_RUONG = {4929, 4930, 4931}\t-- Ruong trang bi 1/2/3 (goc 5064..5066)",
    "PETEQUIP_CHIAKHOA = 4932\t-- Chia khoa ruong (goc 5067)",
    "",
])
ghi_crlf(SV + r"\script\petsys\petequip_def.lua", noidung)
print("3. petequip_def.lua: 20 mon (4907..4926) + phu 4927..4932")
