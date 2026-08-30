# -*- coding: utf-8 -*-
r"""[PETSYS 30/08] Va tiep 3 muc phan bien:
A. 18 ky nang bi dong dang cast len NPC PET - ma pet KHONG danh (Linux/VLTK
   deu chi di theo) => buff vo nghia. Chuyen cast len CHU (giong 4 vong
   sang) de nguoi choi thuc su huong.
B. Nguong bo: bo hardcode "du 10 mon", chuyen sang doc bang
   settings\petsys\suitattrib.txt (bac bo) + chuoi goc
   "Bo <ten> %d mon": ap theo SO MON dang mac theo ti le mon/10.
   -> Khong che them so: dung dung 3 gia tri bang goc, chia theo so mon.
C. dataload: nap LoadSkillData that (szSkillDefFile rong -> tro dung
   \settings\petsys\pet_skill_def.txt) + goi trong Init.
"""
import io

CR = chr(13)
BS = chr(92)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"


def doc_lf(p):
    return io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")


def ghi_crlf(p, lf):
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))


# ---------- A. ext skill cast len CHU ----------
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPet.cpp"
lf = doc_lf(p)
cu = """	int nPetNpc = s_nPetNpcIdx[nPlayerIdx];
	if (nPetNpc > 0 && nPetNpc < MAX_NPC)
	{
		for (int k = 0; k < 4; k++)
		{"""
moi = """	// [30/08 phan bien] pet KHONG danh (ca Linux lan VLTK) nen cast ky nang
	// bi dong len PET la vo nghia -> ap len CHU nhu 4 vong sang.
	{
		for (int k = 0; k < 4; k++)
		{"""
if "ap len CHU nhu 4 vong sang" in lf:
    print("A. da co")
else:
    assert lf.count(cu) == 1, lf.count(cu)
    lf = lf.replace(cu, moi, 1)
    lf = lf.replace("\t\t\t\tpExt->CastStateSkill(nPetNpc, 0, 0, PET_AURA_TIME, TRUE);",
                    "\t\t\t\tpExt->CastStateSkill(nOwnerNpc, 0, 0, PET_AURA_TIME, TRUE);", 1)
    print("A. ext skill cast len CHU")

# ---------- B. nguong bo theo so mon ----------
cu2 = """	int nBo = nV / 100;
	int nSo = nV % 100;
	if (nBo < 0 || nBo > 2) return 0;
	if (nSo < 10) return 0;             // du 10 mon moi kich bo
	return s_nSuitHp[nBo];"""
moi2 = """	int nBo = nV / 100;
	int nSo = nV % 100;
	if (nBo < 0 || nBo > 2) return 0;
	if (nSo <= 0) return 0;
	if (nSo > 10) nSo = 10;
	// [30/08 phan bien] bang goc chi cho 3 gia tri theo BAC BO, khong ghi
	// nguong so mon; chuoi goc la "Bo <ten> %d mon" -> cong theo ti le
	// so mon dang mac (du 10 mon = tron gia tri bang goc).
	return s_nSuitHp[nBo] * nSo / 10;"""
if "cong theo ti le" in lf:
    print("B. da co")
else:
    assert lf.count(cu2) == 1
    lf = lf.replace(cu2, moi2, 1)
    print("B. bonus bo theo so mon")
ghi_crlf(p, lf)

# ---------- C. dataload nap pet_skill_def ----------
p = SV + r"\script\petsys\dataload.lua"
lf = doc_lf(p)
if 'szSkillDefFile = ""' in lf:
    lf = lf.replace('szSkillDefFile = ""',
                    'szSkillDefFile = "' + BS * 2 + 'settings' + BS * 2 + 'petsys' + BS * 2 + 'pet_skill_def.txt"')
    print("C1. tro dung duong dan pet_skill_def")
if "LoadSkillData" in lf and "self:LoadSkillData()" not in lf:
    i = lf.find("LoadFeatureData()")
    if i > 0:
        j = lf.find("\n", i)
        lf = lf[:j + 1] + "PetSys:LoadSkillData()\n" + lf[j + 1:]
        print("C2. goi LoadSkillData")
ghi_crlf(p, lf)
print("XONG p68")
