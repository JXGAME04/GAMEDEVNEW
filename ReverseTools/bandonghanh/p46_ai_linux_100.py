# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] AI pet = DUNG 100% co che + hang so dich nguoc tu
jx_linux_y (KPet follow 0x081D4F80, goi tu player-tick moi frame):
- dist^2 <= 46224 (215mps ~ 6,7 o): DUNG YEN
- dist^2 >  562499 (750mps ~ 23,4 o): SetPos DUNG toa do chu (keo ve)
- o giua: di toi diem CHEO-SAU chu 100mps cung phia pet dang dung
  (offs = delta>0 ? -100 : +100 tung truc), lenh WALK (type 2 Linux)
- BO het tu che: khong doi toc do (theo bang npcs nhu Linux), khong doi
  m_AIMAXTime, khong random dich.
Tinh nang DANH (Linux goc KHONG co - chu yeu cau giu): chuyen sang nhip
rieng trong Pet_Breathe (moi ~18 frame ~1s) khi chu bat FightMode -
chon dich mode 22/vision 480 (bang partner) -> do_skill.
"""
import io

CR = chr(13)
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPet.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read()
lf = s.replace(CR + "\n", "\n")

# ---- 1. go khoi toc do + AIMAXTime tu che trong sPetSummon ----
cu1 = """	pNpc->m_bNpcFollowFindPath = FALSE;
	// [29/08] chay bang chu de bam kip ("di nhanh qua pet khong theo")
	pNpc->m_WalkSpeed = pOwner->m_RunSpeed;
	pNpc->m_RunSpeed = pOwner->m_RunSpeed * 5 / 4;
	// tick AI ~0.5s (template npc thoai tick cham -> canh 'toan nhay' thay vi chay)
	pNpc->m_AIMAXTime = 9;
"""
moi1 = """	pNpc->m_bNpcFollowFindPath = FALSE;
	// [29/08 - theo Linux] KPet::CreateNpc (0x081D5180) KHONG chinh toc do /
	// AI - moi chi so theo BANG npcs.txt cua template.
"""
if cu1 in lf:
    lf = lf.replace(cu1, moi1, 1)
    print("1. go toc/AI tu che")
else:
    print("1. da go / khac dang")

# ---- 2. Pet_ProcessAI -> chi con vai tro don npc mo coi ----
i = lf.find("void Pet_ProcessAI(int nNpcIdx)")
j = lf.find("\n//---------------------------------------------------------------------------\nvoid Pet_Breathe()", i)
assert i > 0 and j > i, (i, j)
moi2 = """void Pet_ProcessAI(int nNpcIdx)
{
	// [29/08 - theo Linux] follow KHONG nam o AI npc: jx_linux_y goi
	// KPet-follow tu PLAYER TICK moi frame (caller 0x080B7104) -> ta lam
	// trong Pet_Breathe (CoreServerShell goi moi frame). O day chi don
	// npc mo coi (chu bien mat).
	KNpc* pNpc = &Npc[nNpcIdx];
	int nOwner = pNpc->m_nPartnerOwner;
	if (nOwner <= 0 || nOwner >= MAX_PLAYER || Player[nOwner].m_nIndex <= 0 ||
		s_nPetNpcIdx[nOwner] != nNpcIdx)
	{
		if (pNpc->m_RegionIndex >= 0)
		{
			int sw = pNpc->m_SubWorldIndex, rg = pNpc->m_RegionIndex;
			SubWorld[sw].m_Region[rg].RemoveNpc(nNpcIdx);
			SubWorld[sw].m_Region[rg].DecRef(pNpc->m_MapX, pNpc->m_MapY, obj_npc);
		}
		NpcSet.Remove(nNpcIdx);
	}
}

//---------------------------------------------------------------------------
// FOLLOW dung 100%% co che + hang so Linux (KPet 0x081D4F80):
// dist^2<=46224 dung; >562499 SetPos ve toa do chu; giua: WALK toi diem
// cheo-sau chu 100mps cung phia dang dung.
//---------------------------------------------------------------------------
static void sPetFollowLinux(int nPlayerIdx, int nNpcIdx)
{
	KNpc* pNpc = &Npc[nNpcIdx];
	KNpc* pOwnerNpc = &Npc[Player[nPlayerIdx].m_nIndex];
	if (pNpc->m_SubWorldIndex != pOwnerNpc->m_SubWorldIndex)
		return;
	int nPX = 0, nPY = 0, nOX = 0, nOY = 0;
	pNpc->GetMpsPos(&nPX, &nPY);
	pOwnerNpc->GetMpsPos(&nOX, &nOY);
	int nDX = nOX - nPX, nDY = nOY - nPY;
	int nDis2 = nDX * nDX + nDY * nDY;
	if (nDis2 <= 46224)
		return;
	if (nDis2 > 562499)
	{
		pNpc->SetPos(nOX, nOY);
		return;
	}
	int nGoX = nOX + ((nDX > 0) ? -100 : 100);
	int nGoY = nOY + ((nDY > 0) ? -100 : 100);
	pNpc->SendCommand(do_walk, nGoX, nGoY);
}

//---------------------------------------------------------------------------
// DANH (tinh nang them theo yeu cau chu - Linux goc pet KHONG danh):
// moi ~18 frame (~1s) khi chu bat FightMode: chon dich mode 22 (ke vua
// danh chu / gan nhat, vision 480 nhu bang partner) -> do_skill bang bo
// skill BANG npcs cua template.
//---------------------------------------------------------------------------
static DWORD s_dwFightTick[MAX_PLAYER];
static void sPetFight(int nPlayerIdx, int nNpcIdx)
{
	KNpc* pNpc = &Npc[nNpcIdx];
	KNpc* pOwnerNpc = &Npc[Player[nPlayerIdx].m_nIndex];
	if (pNpc->m_CurrentCamp != pOwnerNpc->m_CurrentCamp)
		pNpc->SetCurrentCamp(pOwnerNpc->m_CurrentCamp);
	if (pNpc->m_FightMode != pOwnerNpc->m_FightMode)
		pNpc->m_FightMode = pOwnerNpc->m_FightMode;
	if (!pOwnerNpc->m_FightMode)
		return;
	if (++s_dwFightTick[nPlayerIdx] % 18 != 0)
		return;
	int nTarget = sPartnerPickTarget(nNpcIdx, Player[nPlayerIdx].m_nIndex, 22, 480);
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
lf = lf[:i] + moi2 + lf[j + 1:]
print("2. viet lai AI theo Linux")

# ---- 3. Pet_Breathe: goi follow + fight moi frame (truoc khoi aura) ----
neo3 = "\t\tif (++s_dwAuraTick[i] >= PET_AURA_RECAST)"
if "sPetFollowLinux(i, nNpc);" in lf:
    print("3. da co")
else:
    assert lf.count(neo3) == 1, lf.count(neo3)
    chen = ("\t\t// [29/08 - theo Linux] follow chay tu PLAYER TICK moi frame\n"
            "\t\t// (jx_linux_y goi KPet-follow tu 0x080B7104 trong player tick)\n"
            "\t\tsPetFollowLinux(i, nNpc);\n"
            "\t\tsPetFight(i, nNpc);\n")
    lf = lf.replace(neo3, chen + neo3, 1)
    print("3. Breathe + follow/fight")

io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
print("XONG p46 - kiem tay khoi Breathe truoc khi build")
