# -*- coding: utf-8 -*-
r"""[PETSYS 30/08] Chu chon: "Bat danh + chia thuoc tinh".
 - Pet TU DANH khi chu bat che do chien dau (phan THEM ngoai ban goc -
   ban goc Linux/VLTK pet chi di theo; ghi ro tai day).
 - Thuoc tinh TRANG BI + thuoc tinh BO -> cong cho PET (pet manh len).
 - 4 vong sang + ky nang bi kiep van buff CHU (dung ban goc).
"""
import io

CR = chr(13)
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPet.cpp"
lf = io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")

# ---- 1. thuoc tinh trang bi ve lai PET ----
cu = "\tnNpcIdx = Player[nPlayerIdx].m_nIndex;   // ap len CHU\n"
if cu in lf:
    lf = lf.replace(cu, "\t// [30/08 chu chon] ap len PET (pet danh duoc nen chi so co tac dung)\n", 1)
    print("1. thuoc tinh trang bi -> ap len PET")

# ---- 2. bonus bo: bo khoi chu, cong lai vao mau pet ----
cu2 = """	// [30/08] thuoc tinh BO trang bi: cong sinh luc toi da cho CHU
	{
		int nSuitHp = sPetSuitAttrib(nPlayerIdx);
		if (nSuitHp > 0)
		{
			KMagicAttrib sSuit;
			memset(&sSuit, 0, sizeof(sSuit));
			sSuit.nAttribType = magic_lifemax_yan_v;
			sSuit.nValue[0] = nSuitHp;
			Npc[nOwnerNpc].ModifyAttrib(0, &sSuit);
		}
	}
"""
if cu2 in lf:
    lf = lf.replace(cu2, "", 1)
    print("2. bo bonus bo khoi CHU")
cu3 = "\t\tint nHp = sPetG(nPlayerIdx, PET_TV_ATTRIB0 + 4);\n"
moi3 = "\t\tint nHp = sPetG(nPlayerIdx, PET_TV_ATTRIB0 + 4) + sPetSuitAttrib(nPlayerIdx);\n"
if cu3 in lf and "sPetSuitAttrib(nPlayerIdx);\n" not in cu3:
    lf = lf.replace(cu3, moi3, 1)
    print("3. bonus bo -> cong mau PET")

# ---- 3. bat lai pet tu danh ----
if "sPetFight" not in lf:
    neo = "//---------------------------------------------------------------------------\n// FOLLOW dung 100%% co che + hang so Linux"
    if neo not in lf:
        neo = "static void sPetFollowLinux(int nPlayerIdx, int nNpcIdx)"
    them = """//---------------------------------------------------------------------------
// [30/08 - CHU CHON "bat danh"] PHAN THEM NGOAI BAN GOC: ca Linux lan VLTK
// pet CHI DI THEO. Khuon lay tu he partner: chu bat FightMode -> moi ~1s
// chon dich mode 22 (ke vua danh chu / gan nhat, tam 480) -> do_skill bang
// bo skill cua ngoai quan (bang npcs.txt).
//---------------------------------------------------------------------------
extern int sPartnerPickTarget(int nNpcIdx, int nOwnerNpcIdx, int nMode, int nVision);
static DWORD s_dwFightTick[MAX_PLAYER];

static void sPetFight(int nPlayerIdx, int nNpcIdx)
{
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC) return;
	KNpc* pNpc = &Npc[nNpcIdx];
	int nOwnerNpc = Player[nPlayerIdx].m_nIndex;
	if (nOwnerNpc <= 0 || nOwnerNpc >= MAX_NPC) return;
	KNpc* pOwnerNpc = &Npc[nOwnerNpc];
	if (pNpc->m_CurrentCamp != pOwnerNpc->m_CurrentCamp)
		pNpc->SetCurrentCamp(pOwnerNpc->m_CurrentCamp);
	if (pNpc->m_FightMode != pOwnerNpc->m_FightMode)
		pNpc->m_FightMode = pOwnerNpc->m_FightMode;
	if (!pOwnerNpc->m_FightMode)
		return;
	if (++s_dwFightTick[nPlayerIdx] % 18 != 0)
		return;
	int nTarget = sPartnerPickTarget(nNpcIdx, nOwnerNpc, 22, 480);
	if (nTarget <= 0)
		return;
	int nSkillId = 0;
	for (int nSlot = 1; nSlot <= 4; nSlot++)
		if (pNpc->m_SkillList.m_Skills[nSlot].SkillId > 0)
		{
			nSkillId = pNpc->m_SkillList.m_Skills[nSlot].SkillId;
			if (rand() % 2) break;
		}
	if (nSkillId > 0)
		pNpc->SendCommand(do_skill, nSkillId, -1, nTarget);
}

"""
    assert lf.count(neo) == 1, "anchor follow"
    lf = lf.replace(neo, them + neo, 1)
    # goi trong Pet_Breathe ngay sau follow
    cu4 = "\t\tsPetFollowLinux(i, nNpc);\n"
    assert lf.count(cu4) == 1
    lf = lf.replace(cu4, cu4 + "\t\tsPetFight(i, nNpc);\t// [30/08] pet tu danh (phan them)\n", 1)
    print("4. bat lai pet tu danh")
else:
    print("4. da co sPetFight")

io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
print("XONG p85")
