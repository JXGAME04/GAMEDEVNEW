# -*- coding: utf-8 -*-
r"""[PETSYS 30/08] C: ap 3 thuoc tinh cua 10 mon trang bi len NPC pet.
Lua da roll gia tri va luu:
  5143..5152 = id mon dang deo (0 = trong)
  5170..5199 = 3 gia tri da roll cua tung mon (mon i -> 5170+(i-1)*3)
C khong doc duoc bang Lua nen doc THANG bang goc da port sang
settings\petsys\equipattrib.txt (sinh tu petequip_def) de biet MA attrib
cua tung mon; roi cong vao npc pet qua KNpc::m_MagicAttrib.
"""
import io
import os
import re

CR = chr(13)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"

# ---- 1. sinh bang tra ma attrib cho C: id \t a1 \t a2 \t a3 ----
lf = io.open(SV + r"\script\petsys\petequip_def.lua", "r", encoding="latin-1").read().replace(CR, "")
ra = ["ItemId\tAttrib1\tAttrib2\tAttrib3"]
for m in re.finditer(r"\[(\d+)\] = \{nSlot = \d+, nSuit = \d+, tbAttrib = \{([^}]*)\}\}", lf.replace("}}", "}}\n")):
    idj = m.group(1)
    inner = m.group(2)
    mas = re.findall(r"\{(\d+),\s*\d+,\s*\d+\}", inner)
    while len(mas) < 3:
        mas.append("0")
    ra.append("%s\t%s\t%s\t%s" % (idj, mas[0], mas[1], mas[2]))
io.open(SV + r"\settings\petsys\equipattrib.txt", "w", encoding="latin-1",
        newline="").write(("\n".join(ra) + "\n").replace("\n", CR + "\n"))
print("1. sinh equipattrib.txt:", len(ra) - 1, "mon")

# ---- 2. C doc bang + ap len pet ----
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPet.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read()
lf = s.replace(CR + "\n", "\n")
if "sPetApplyEquip" in lf:
    print("2. da co")
else:
    neo = "static int sPetSuitAttrib(int nPlayerIdx)"
    them = """// [30/08] AP THUOC TINH TRANG BI len NPC pet.
// Bang goc VLTK cho moi mon 3 thuoc tinh {ma, min, max}; Lua da roll gia
// tri va luu o 5170..5199, con MA attrib tra o bang
// settings\\petsys\\equipattrib.txt (sinh tu petequip_def.lua).
#define PET_TV_EQUIP0      5143
#define PET_TV_EQUIPATT0   5170

static KTabFile s_EquipAttTab;
static int      s_bEquipAttLoaded = 0;

static void sPetApplyEquip(int nPlayerIdx, int nNpcIdx)
{
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC) return;
	if (!s_bEquipAttLoaded)
	{
		s_bEquipAttLoaded = 1;
		s_EquipAttTab.Load((LPSTR)"\\\\settings\\\\petsys\\\\equipattrib.txt");
	}
	int nRow = s_EquipAttTab.GetHeight();
	for (int nSlot = 0; nSlot < 10; nSlot++)
	{
		int nId = sPetG(nPlayerIdx, PET_TV_EQUIP0 + nSlot);
		if (nId <= 0) continue;
		int nMa[3] = { 0, 0, 0 };
		char szNum[16];
		for (int r = 2; r <= nRow; r++)
		{
			s_EquipAttTab.GetString(r, 1, (LPSTR)"", szNum, sizeof(szNum));
			if (atoi(szNum) != nId) continue;
			for (int c = 0; c < 3; c++)
			{
				s_EquipAttTab.GetString(r, 2 + c, (LPSTR)"", szNum, sizeof(szNum));
				nMa[c] = atoi(szNum);
			}
			break;
		}
		for (int c = 0; c < 3; c++)
		{
			int nVal = sPetG(nPlayerIdx, PET_TV_EQUIPATT0 + nSlot * 3 + c);
			if (nMa[c] > 0 && nVal > 0)
				Npc[nNpcIdx].ModifyMagicAttrib(nMa[c], nVal, 0, 0);
		}
	}
}

static int sPetSuitAttrib(int nPlayerIdx)"""
    lf = lf.replace(neo, them, 1)
    # goi khi summon (sau khi set mau)
    cu = "\tpNpc->m_CurrentLife = pNpc->m_LifeMax;\n\ts_nPetNpcIdx[nPlayerIdx] = nNpcIdx;"
    moi = ("\tpNpc->m_CurrentLife = pNpc->m_LifeMax;\n"
           "\tsPetApplyEquip(nPlayerIdx, nNpcIdx);\t// [30/08] thuoc tinh trang bi\n"
           "\ts_nPetNpcIdx[nPlayerIdx] = nNpcIdx;")
    assert lf.count(cu) == 1, "anchor summon"
    lf = lf.replace(cu, moi, 1)
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
    print("2. C: sPetApplyEquip + goi khi summon")
print("XONG p79")
