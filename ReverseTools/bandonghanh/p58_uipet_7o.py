# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] UiPet: 7 o trang bi hang TREN (section PetEquip_1..7),
anh theo duong dan CO DINH \spr\item\petequip\pet_%d.spr (bo tra bang
KTabFile - da chung minh khong an tren may that)."""
import io
import re

CR = chr(13)


def doc_lf(p):
    return io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")


def ghi_crlf(p, lf):
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))


ph = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPet.h"
lf = doc_lf(ph)
lf = re.sub(r"#define\s+PET_UI_EQUIP_NUM\s+\d+", "#define PET_UI_EQUIP_NUM   7", lf, 1)
ghi_crlf(ph, lf)
print("1. UiPet.h EQUIP_NUM = 7")

pc = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPet.cpp"
lf = doc_lf(pc)

# Init: doc section PetEquip_%d
cu = 'sprintf(szSec, "Equip_%d", i + 1);'
if cu in lf:
    lf = lf.replace(cu, 'sprintf(szSec, "PetEquip_%d", i + 1);\t// [29/08] 7 o hang TREN', 1)
    print("2. Init doc PetEquip_%d")

# UpdateData: anh co dinh theo slot
i = lf.find("    // [29/08] 6 o trang bi (task 5143..5148 = particular item)")
if i > 0:
    j = lf.find("\n\n", i)
    moi = """    // [29/08] 7 o trang bi hang tren (task 5143..5149 = particular item);
    // anh CO DINH theo o de khong phu thuoc bang: \\spr\\item\\petequip\\pet_N.spr
    for (i = 0; i < PET_UI_EQUIP_NUM && i < 7; i++)
    {
        int nP = sPetTV(5143 + i);
        if (nP >= 4881 && nP <= 4887)
        {
            char szImg[96];
            _snprintf(szImg, sizeof(szImg) - 1, "\\\\spr\\\\item\\\\petequip\\\\pet_%d.spr", nP - 4880);
            szImg[sizeof(szImg) - 1] = 0;
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
    lf = lf[:i] + moi + lf[j + 1:]
    print("3. UpdateData 7 o anh co dinh")
ghi_crlf(pc, lf)
print("XONG p58")
