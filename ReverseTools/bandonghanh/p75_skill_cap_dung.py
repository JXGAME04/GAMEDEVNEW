# -*- coding: utf-8 -*-
r"""[PETSYS 30/08] Chu: "len cap 26 roi ky nang van chua tang len diem".
Nguyen nhan do duoc: KWndObjectBox::HoldObject(uGenre, uId, nDataW, nDataH)
- tham so 3/4 la KICH THUOC, KHONG phai cap. Client tu tra "dang cap hien
thoi" tu KSkillList cua NGUOI CHOI -> chua hoc nen = 0 -> moi param tinh
theo cap 0 = 0%.
VA: dat CAP SKILL that cho CHU (SetSkillLevelDirectlyUsingId - dung ham
engine da co, xem LuaSetSkillLevel ScriptFuns.cpp) moi lan ap aura:
  - 4 vong sang 1600..1603: cap = cap pet (toi da 130)
  - 4 ky nang bi kiep 1670..1687: cap 1
  - khi thu pet / xoa pet: dat 0 de go
Nho vay tooltip hien dung cap va hieu ung bi dong (Attrib 1008) an that.
"""
import io

CR = chr(13)
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPet.cpp"
lf = io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")

if "SetSkillLevelDirectlyUsingId" in lf:
    print("da co")
else:
    # 1. trong sPetApplyAura: sau khi CastStateSkill cho aura -> set cap that
    cu = """	KSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(PET_AURA_SKILL0 + nKind - 1, nLevel);
	if (pSkill)
		pSkill->CastStateSkill(nOwnerNpc, 0, 0, PET_AURA_TIME, TRUE);"""
    moi = """	KSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(PET_AURA_SKILL0 + nKind - 1, nLevel);
	if (pSkill)
		pSkill->CastStateSkill(nOwnerNpc, 0, 0, PET_AURA_TIME, TRUE);
	// [30/08] dat CAP SKILL that cho chu: client tra "dang cap hien thoi"
	// tu KSkillList cua nguoi choi (HoldObject khong truyen cap duoc) ->
	// thieu buoc nay thi tooltip hien cap 0 va tac dung 0%.
	for (int nK = 0; nK < PET_SKILL_COUNT; nK++)
	{
		int nSkId = PET_AURA_SKILL0 + nK;
		Npc[nOwnerNpc].m_SkillList.SetSkillLevelDirectlyUsingId(
			nSkId, (nK == nKind - 1) ? nLevel : 0);
	}"""
    assert lf.count(cu) == 1, lf.count(cu)
    lf = lf.replace(cu, moi, 1)

    # 2. ext skill: set cap 1 cho chu
    cu2 = """			KSkill* pExt = (KSkill*)g_SkillManager.GetSkill(nSk, 1);
			if (pExt)
				pExt->CastStateSkill(nOwnerNpc, 0, 0, PET_AURA_TIME, TRUE);"""
    moi2 = """			KSkill* pExt = (KSkill*)g_SkillManager.GetSkill(nSk, 1);
			if (pExt)
				pExt->CastStateSkill(nOwnerNpc, 0, 0, PET_AURA_TIME, TRUE);
			Npc[nOwnerNpc].m_SkillList.SetSkillLevelDirectlyUsingId(nSk, 1);"""
    if cu2 in lf:
        lf = lf.replace(cu2, moi2, 1)

    # 3. khi thu pet: go cap skill
    cu3 = """static void sPetUnSummon(int nPlayerIdx)
{
	sPetRemoveNpc(nPlayerIdx);
	sPetS(nPlayerIdx, PET_TV_SUMMON, 0);
}"""
    moi3 = """static void sPetUnSummon(int nPlayerIdx)
{
	sPetRemoveNpc(nPlayerIdx);
	sPetS(nPlayerIdx, PET_TV_SUMMON, 0);
	// [30/08] thu pet -> go cap 4 vong sang khoi chu (khong go ky nang bi
	// kiep vi do la cua pet da hoc, se ap lai khi goi ra)
	int nOwnerNpc = Player[nPlayerIdx].m_nIndex;
	if (nOwnerNpc > 0 && nOwnerNpc < MAX_NPC)
	{
		for (int nK = 0; nK < PET_SKILL_COUNT; nK++)
			Npc[nOwnerNpc].m_SkillList.SetSkillLevelDirectlyUsingId(
				PET_AURA_SKILL0 + nK, 0);
	}
}"""
    assert lf.count(cu3) == 1, "unsummon"
    lf = lf.replace(cu3, moi3, 1)
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
    print("VA: dat cap skill that cho chu (aura + ext) + go khi thu pet")
