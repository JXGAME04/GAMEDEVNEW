# -*- coding: utf-8 -*-
r"""[PETSYS 30/08] VA theo bao cao PHAN BIEN (agent doc lap):
1. [CHI TU] AddItem CAN 7 THAM SO (ScriptFuns.cpp:4932 - duoi 7 la
   khong them item nao). Moi cho petsys/bdh_admin dang goi 6 ->
   "thao trang bi la MAT TRANG", "mo ruong tru chia ma khong ra do",
   "menu admin cap do khong duoc gi". Them tham so thu 7 = 0.
2. [NANG] UiPet::OnTaskValueChanged loc 5110..5149 -> bo 3 o trang bi
   cuoi (5150..5152) va o bo (5163). Mo rong 5110..5169.
3. [NANG] Resync client chi ban 5110..5138 -> mo rong 5110..5169
   (protocol_process_gs.lua + bdh_admin.lua).
4. [NANG] Bonus BO chi tinh luc goi pet -> Pet_Breathe cap nhat lai
   LifeMax moi khi so mon bo doi.
5. [VUA] Chia Khoa Ruong dang tro Script=petbox.lua (khong co trong bang
   PETBOX_RUONG_BO) -> de trong.
6. [VUA] dataload.lua doc lo 1 dong (for i=2, 1+nRowCount) -> sua.
7. [VUA] feed.lua hoan vi nTaskId giua Mia/Khoai lang -> sua theo ban goc.
8. [VUA] bdh_admin BDH_P_TrangBi la ma chet + cap nham id 4881..4886 -> go.
"""
import io
import re

CR = chr(13)
BS = chr(92)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"


def doc_lf(p):
    return io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")


def ghi_crlf(p, lf):
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))


# ---------- 1. AddItem 6 -> 7 tham so ----------
sua = 0
for f in (r"\script\petsys\petequip.lua", r"\script\petsys\petbox.lua",
          r"\script\item\bdh_admin.lua", r"\script\petsys\petcard.lua",
          r"\script\petsys\bikip.lua"):
    p = SV + f
    try:
        lf = doc_lf(p)
    except Exception:
        continue
    moi = re.sub(r"AddItem\((\d+),\s*(\d+),\s*([\w\d]+),\s*(\d+),\s*(\d+),\s*(\d+)\)",
                 r"AddItem(\1, \2, \3, \4, \5, \6, 0)", lf)
    if moi != lf:
        n = len(re.findall(r"AddItem\([^)]*, 0\)", moi)) - len(re.findall(r"AddItem\([^)]*, 0\)", lf))
        ghi_crlf(p, moi)
        sua += 1
        print("1. +tham so 7 cho AddItem:", f)
print("   tong file sua:", sua)

# ---------- 2. UiPet dai task ----------
p = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPet.cpp"
lf = doc_lf(p)
cu = "    if (nTaskId < PET_TV_BASE || nTaskId > PET_TV_BASE + 39)"
moi = ("    // [30/08 phan bien] dai cu +39 cat mat o trang bi 8/9/10 (5150..5152)\n"
       "    // va o bo (5163) -> mo rong +59\n"
       "    if (nTaskId < PET_TV_BASE || nTaskId > PET_TV_BASE + 59)")
if "+ 59)" in lf:
    print("2. da co")
else:
    assert lf.count(cu) == 1
    ghi_crlf(p, lf.replace(cu, moi, 1))
    print("2. UiPet dai 5110..5169")

# ---------- 3. resync 5110..5169 ----------
for f in (r"\script\petsys\protocol_process_gs.lua", r"\script\item\bdh_admin.lua"):
    p = SV + f
    lf = doc_lf(p)
    if "5110, 5169" in lf:
        continue
    moi = lf.replace("for i = 5110, 5138 do", "for i = 5110, 5169 do")
    if moi != lf:
        ghi_crlf(p, moi)
        print("3. resync mo rong:", f)

# ---------- 4. bonus bo cap nhat trong Pet_Breathe ----------
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPet.cpp"
lf = doc_lf(p)
if "s_nSuitLast" not in lf:
    neo = "static int sPetSuitAttrib(int nPlayerIdx)"
    them = ("static int s_nSuitLast[MAX_PLAYER];   // [30/08] theo doi doi bo de cap nhat mau ngay\n\n"
            + neo)
    lf = lf.replace(neo, them, 1)
    # trong Pet_Breathe: sau khi co nNpc hop le
    neo2 = "\t\t// [29/08 - theo Linux] follow chay tu PLAYER TICK moi frame\n"
    assert lf.count(neo2) == 1
    them2 = ("\t\t// [30/08 phan bien] deo/thao trang bi doi bo -> cap nhat mau NGAY,\n"
             "\t\t// khong doi den lan goi pet ke tiep\n"
             "\t\t{\n"
             "\t\t\tint nSuit = sPetG(i, PET_TV_SUITCOUNT);\n"
             "\t\t\tif (nSuit != s_nSuitLast[i])\n"
             "\t\t\t{\n"
             "\t\t\t\ts_nSuitLast[i] = nSuit;\n"
             "\t\t\t\tint nHp = sPetG(i, PET_TV_ATTRIB0 + 4) + sPetSuitAttrib(i);\n"
             "\t\t\t\tif (nHp > 0)\n"
             "\t\t\t\t{\n"
             "\t\t\t\t\tNpc[nNpc].m_LifeMax = nHp;\n"
             "\t\t\t\t\tNpc[nNpc].m_CurrentLifeMax = nHp;\n"
             "\t\t\t\t\tif (Npc[nNpc].m_CurrentLife > nHp)\n"
             "\t\t\t\t\t\tNpc[nNpc].m_CurrentLife = nHp;\n"
             "\t\t\t\t}\n"
             "\t\t\t}\n"
             "\t\t}\n")
    lf = lf.replace(neo2, them2 + neo2, 1)
    ghi_crlf(p, lf)
    print("4. bonus bo cap nhat trong Pet_Breathe")
else:
    print("4. da co")

# ---------- 5. chia khoa: bo Script ----------
for pth in (SV + r"\settings\item\magicscript.txt",
            r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\settings\item\magicscript.txt"):
    rows = [d.split("\t") for d in doc_lf(pth).split("\n") if d.strip()]
    doi = 0
    for c in rows:
        if len(c) > 9 and c[3] == "4932" and "petbox" in c[9]:
            c[9] = "0"
            doi += 1
    if doi:
        ghi_crlf(pth, "\n".join("\t".join(c) for c in rows) + "\n")
        print("5. chia khoa bo Script:", pth[:2])

# ---------- 6. dataload doc lo 1 dong ----------
p = SV + r"\script\petsys\dataload.lua"
lf = doc_lf(p)
if "for i=2, 1+nRowCount" in lf:
    lf = lf.replace("for i=2, 1+nRowCount", "for i=2, nRowCount")
    ghi_crlf(p, lf)
    print("6. dataload bo dong rac")

# ---------- 7. feed.lua hoan vi task ----------
p = SV + r"\script\petsys\feed.lua"
lf = doc_lf(p)
if "nTaskId=TSK_SUGARCANE_DAILY}" in lf and "SWEET_POTATO" in lf:
    lf = lf.replace('ITEM_SWEET_POTATO = {szName="Khoai lang", tbProp={6,1,4879,1,0,0}, nTaskId=TSK_SUGARCANE_DAILY}',
                    'ITEM_SWEET_POTATO = {szName="Khoai lang", tbProp={6,1,4879,1,0,0}, nTaskId=TSK_SWEET_POTATO_DAILY}')
    lf = lf.replace('ITEM_SUGARCANE = {szName="M�a", tbProp={6,1,4877,1,0,0}, nTaskId=TSK_SWEET_POTATO_DAILY}',
                    'ITEM_SUGARCANE = {szName="M�a", tbProp={6,1,4877,1,0,0}, nTaskId=TSK_SUGARCANE_DAILY}')
    ghi_crlf(p, lf)
    print("7. feed.lua sua hoan vi task id")

# ---------- 8. go ham chet BDH_P_TrangBi ----------
p = SV + r"\script\item\bdh_admin.lua"
lf = doc_lf(p)
i = lf.find("function BDH_P_TrangBi()")
if i > 0 and "/BDH_P_TrangBi" not in lf:
    j = lf.find("\nend", i) + len("\nend\n")
    lf = lf[:i] + lf[j:]
    ghi_crlf(p, lf)
    print("8. go ham chet BDH_P_TrangBi")
print("XONG p67")
