# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] C: 10 o trang bi + thuoc tinh BO theo suitattrib.txt goc.
- INI: 7 o hien co -> dat lai 10 o PetEquip_1..10 (hang tren 7 o + 3 o
  con lai dat tiep hang duoi cua khu trang bi theo do anh nen).
- UiPet: PET_UI_EQUIP_NUM 7 -> 10, anh theo bang phu (id 4907..4926 ->
  ten file goc trong bang item, doc bang KTabFile mot lan).
- Server: doc task 5163 (bo*100 + so mon) -> tra bang suitattrib
  (settings\petsys\suitattrib.txt) -> ap len PET khi summon/breathe:
  233 = sinh luc toi da (JX1 magic_lifemax_yan_v), 308/311 NGOAI DAI JX1
  -> chi ap 233; ghi log 2 ma con lai de bao chu.
"""
import io
import os
import re

CR = chr(13)
BS = chr(92)
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"


def doc_lf(p):
    return io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")


def ghi_crlf(p, lf):
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))


# ---------- 1. INI: 10 o ----------
p = CL + r"\Ui\Ui3\pet_main.ini"
s = io.open(p, "r", encoding="latin-1", newline="").read()
# 7 o hang tren (y=81) + 3 o tiep theo dat duoi cung hang (y=113) cho du 10
TOA = [(340, 81), (376, 81), (412, 81), (448, 81), (484, 81), (520, 81), (556, 81),
       (340, 113), (376, 113), (412, 113)]
for k, (x, y) in enumerate(TOA):
    sec = "[PetEquip_%d]" % (k + 1)
    if sec in s:
        i = s.find(sec)
        j = s.find("[", i + 1)
        kh = s[i:j] if j > 0 else s[i:]
        m = re.sub(r"Left=\d+", "Left=%d" % x, kh, 1)
        m = re.sub(r"Top=\d+", "Top=%d" % y, m, 1)
        s = s.replace(kh, m, 1)
    else:
        s = s.rstrip() + CR + "\n" + ("[PetEquip_%d]%sLeft=%d%sTop=%d%sWidth=26%sHeight=26%s"
                                      % (k + 1, CR + "\n", x, CR + "\n", y, CR + "\n", CR + "\n", CR + "\n"))
io.open(p, "w", encoding="latin-1", newline="").write(s)
print("1. INI 10 o PetEquip")

# ---------- 2. UiPet 10 o ----------
ph = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPet.h"
lf = doc_lf(ph)
lf = re.sub(r"#define\s+PET_UI_EQUIP_NUM\s+\d+", "#define PET_UI_EQUIP_NUM   10", lf, 1)
ghi_crlf(ph, lf)
pc = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPet.cpp"
lf = doc_lf(pc)
i = lf.find("    // [29/08] 7 o trang bi: he tu che DA GO")
if i > 0:
    j = lf.find("\n", i) + 1
    moi = """    // [29/08] 10 o trang bi Dong Hanh (task 5143..5152 = ParticularType).
    // Anh lay tu bang item (cot ImageName) - bang goc VLTK da port sang JX1.
    for (i = 0; i < PET_UI_EQUIP_NUM && i < 10; i++)
    {
        int nP = sPetTV(5143 + i);
        char szImg[128];
        szImg[0] = 0;
        if (nP > 0)
            sPetItemImg(nP, szImg, sizeof(szImg));
        if (szImg[0])
        {
            m_Equip[i].SetImage(ISI_T_SPR, szImg);
            m_Equip[i].Show();
        }
        else
        {
            m_Equip[i].SetImage(ISI_T_SPR, (char*)"");
            m_Equip[i].Hide();
        }
    }
"""
    lf = lf[:i] + moi + lf[j:]
    print("2. UiPet 10 o (anh tu bang)")
# nut Duc lai -> mo menu trang bi (op 10)
lf = lf.replace('        // m_CompanionBtn ("Duc lai"): he trang bi chua port tu nguon that\n',
                '        if (uParam == (unsigned int)(KWndWindow*)&m_CompanionBtn)\n'
                '            SendOp(10);\t// menu trang bi Dong Hanh (server)\n')
ghi_crlf(pc, lf)

# ---------- 3. server: thuoc tinh bo ----------
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPet.cpp"
lf = doc_lf(p)
if "sPetSuitAttrib" not in lf:
    neo = "static void sPetApplyAura(int nPlayerIdx)"
    assert lf.count(neo) == 1
    them = """// [29/08] THUOC TINH BO trang bi Dong Hanh - bang goc
// settings\\petsys\\suitattrib.txt (EquipParticular = bac bo):
//   bac0: 233=5000 308=30 311=1 | bac1: 7500/40/2 | bac2: 10000/50/3
// Ma 233 = sinh luc toi da; 308/311 NGOAI DAI 305 attrib cua JX1 (ban
// private mo rong) -> hien chi ap 233.
#define PET_TV_SUITCOUNT   5163      // lua ghi: bo*100 + so mon dang mac
static int s_nSuitHp[3] = { 5000, 7500, 10000 };

static int sPetSuitAttrib(int nPlayerIdx)
{
	int nV = sPetG(nPlayerIdx, PET_TV_SUITCOUNT);
	if (nV <= 0) return 0;
	int nBo = nV / 100;
	int nSo = nV % 100;
	if (nBo < 0 || nBo > 2) return 0;
	if (nSo < 10) return 0;             // du 10 mon moi kich bo
	return s_nSuitHp[nBo];
}

static void sPetApplyAura(int nPlayerIdx)"""
    lf = lf.replace(neo, them, 1)
    # cong vao mau khi summon
    cu = "\t\tint nHp = sPetG(nPlayerIdx, PET_TV_ATTRIB0 + 4) + sPetG(nPlayerIdx, 5157);\n"
    if cu in lf:
        lf = lf.replace(cu, "\t\tint nHp = sPetG(nPlayerIdx, PET_TV_ATTRIB0 + 4) + sPetSuitAttrib(nPlayerIdx);\n", 1)
    cu2 = "\t\tint nMp = sPetG(nPlayerIdx, PET_TV_ATTRIB0 + 5) + sPetG(nPlayerIdx, 5158);\n"
    if cu2 in lf:
        lf = lf.replace(cu2, "\t\tint nMp = sPetG(nPlayerIdx, PET_TV_ATTRIB0 + 5);\n", 1)
    ghi_crlf(p, lf)
    print("3. server thuoc tinh bo (233 = sinh luc)")
else:
    print("3. da co")
print("XONG p65")
