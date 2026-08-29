# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] C:
1. sPetApplyAura ext: o skill luu SkillId*100+Level (tuong thich gia tri cu
   dang id tran) -> GetSkill(id, lv 1..5).
2. UiPet 4 o ext: parse nhu tren -> HoldObject(id, lv).
3. GO he partner mobile khoi UI: 5 cua so UiPartner* OpenWindow -> return NULL
   (giu ma; chu: "xoa he thong pet cu truoc ban lam nham").
"""
import io
import glob

CR = chr(13)


def doc_lf(p):
    return io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")


def ghi_crlf(p, lf):
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))


# ---- 1. server parse ----
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPet.cpp"
lf = doc_lf(p)
cu = """		for (int k = 0; k < 4; k++)
		{
			int nSk = sPetG(nPlayerIdx, 5139 + k);
			if (nSk <= 0) continue;
			KSkill* pExt = (KSkill*)g_SkillManager.GetSkill(nSk, 1);
			if (pExt)
				pExt->CastStateSkill(nPetNpc, 0, 0, PET_AURA_TIME, TRUE);
		}"""
moi = """		for (int k = 0; k < 4; k++)
		{
			// o luu SkillId*100+Level (bikip.lua); gia tri cu = id tran -> lv 1
			int nV = sPetG(nPlayerIdx, 5139 + k);
			if (nV <= 0) continue;
			int nSk = (nV >= 100000) ? nV / 100 : nV;
			int nLv = (nV >= 100000) ? nV % 100 : 1;
			if (nLv < 1) nLv = 1;
			if (nLv > 5) nLv = 5;
			KSkill* pExt = (KSkill*)g_SkillManager.GetSkill(nSk, nLv);
			if (pExt)
				pExt->CastStateSkill(nPetNpc, 0, 0, PET_AURA_TIME, TRUE);
		}"""
if "nV >= 100000" in lf:
    print("1. da co")
else:
    assert lf.count(cu) == 1
    lf = lf.replace(cu, moi, 1)
    ghi_crlf(p, lf)
    print("1. server parse id*100+lv")

# ---- 2. UiPet parse ----
p = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPet.cpp"
lf = doc_lf(p)
cu = """    for (i = 0; i < PET_UI_EXTSKILL_NUM && i < 4; i++)
    {
        int nSk = sPetTV(5139 + i);
        if (nSk > 0)
            m_ExtSkill[i].HoldObject(CGOG_SKILL_FIGHT, nSk, 1, 0);
        else
            m_ExtSkill[i].HoldObject(CGOG_NOTHING, 0, 0, 0);
    }"""
moi = """    for (i = 0; i < PET_UI_EXTSKILL_NUM && i < 4; i++)
    {
        int nV = sPetTV(5139 + i);
        int nSk = (nV >= 100000) ? nV / 100 : nV;
        int nLv = (nV >= 100000) ? nV % 100 : 1;
        if (nSk > 0)
            m_ExtSkill[i].HoldObject(CGOG_SKILL_FIGHT, nSk, nLv, 0);
        else
            m_ExtSkill[i].HoldObject(CGOG_NOTHING, 0, 0, 0);
    }"""
if "nV >= 100000" in lf:
    print("2. da co")
else:
    assert lf.count(cu) == 1
    lf = lf.replace(cu, moi, 1)
    ghi_crlf(p, lf)
    print("2. UiPet parse")

# ---- 3. chan 5 cua so partner cu ----
for f in glob.glob(r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPartner*.cpp"):
    lf = doc_lf(f)
    if "[29/08] he partner mobile DA GO" in lf:
        print("3. da chan:", f[-24:])
        continue
    import re
    m = re.search(r"(KUiPartner\w*\*\s+KUiPartner\w+::OpenWindow\([^)]*\)\s*\n\{\n)", lf)
    if not m:
        print("3. !! khong thay OpenWindow:", f[-24:])
        continue
    lf = lf.replace(m.group(1), m.group(1) +
                    "    // [29/08] he partner mobile DA GO theo yeu cau chu (dung he PET PC)\n"
                    "    return NULL;\n", 1)
    ghi_crlf(f, lf)
    print("3. chan:", f[-24:])
print("XONG p47b")
