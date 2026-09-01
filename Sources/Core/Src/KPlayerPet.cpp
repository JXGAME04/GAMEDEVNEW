//---------------------------------------------------------------------------
// KPlayerPet.cpp - He BAN DONG HANH ban PC (petsys). Xem KPlayerPet.h.
// Nguon ngu nghia 100%: D:\ServerLinux\server1\script\petsys\*.lua
// (C-API "PET" cua Linux la lib strip - hanh vi suy tu cach lua goi).
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KCore.h"
#include "KPlayerPet.h"
#include "KPlayerPartner.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KItemSet.h"	// [30/08] extern KItemSet ItemSet (PET_ClearHand)
#include "KSubWorld.h"
#include "KSubWorldSet.h"
#include "KSkills.h"
#include "KLuaScript.h"
#include "KSortScript.h"

#ifdef _SERVER

// Npc/Player/SubWorld/NpcSet/g_SkillManager/g_GetScript da extern trong header

// runtime (khong luu): npc cua pet dang goi ra
static int   s_nPetNpcIdx[MAX_PLAYER] = { 0 };
static DWORD s_dwPetNpcID[MAX_PLAYER] = { 0 };
static DWORD s_dwAuraTick[MAX_PLAYER] = { 0 };

#define PET_FORCE_SYNC   1500     // keo ve canh chu khi cach qua xa (mps)
#define PET_FOLLOW_DIS   128
// [PETKN 31/08] PET_AURA_TIME da bo - bi kip gio la passive -1 tren NPC pet
#define PET_AURA_RECAST  60          // lam moi skill pet moi 60 luot breathe (~30s)

extern int sPartnerPickTarget(int nNpcIdx, int nOwnerNpcIdx, int nMode, int nVision);

//---------------------------------------------------------------------------
static int sPetG(int nPlayerIdx, int nId)
{
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER) return 0;
	return (int)Player[nPlayerIdx].m_cTask.GetSaveVal(nId);
}

static void sPetS(int nPlayerIdx, int nId, int nVal)
{
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER) return;
	Player[nPlayerIdx].m_cTask.SetSaveVal(nId, nVal);   // tu sync client
}

static void sPetGetName(int nPlayerIdx, char* szOut17)
{
	int* p = (int*)szOut17;
	for (int i = 0; i < 4; i++)
		p[i] = sPetG(nPlayerIdx, PET_TV_NAME0 + i);
	szOut17[16] = 0;
}

static void sPetSetName(int nPlayerIdx, const char* szName)
{
	char szBuf[20];
	memset(szBuf, 0, sizeof(szBuf));
	strncpy(szBuf, szName ? szName : "", 16);
	int* p = (int*)szBuf;
	for (int i = 0; i < 4; i++)
		sPetS(nPlayerIdx, PET_TV_NAME0 + i, p[i]);
	// doi ten npc dang goi
	int nNpc = s_nPetNpcIdx[nPlayerIdx];
	if (nNpc > 0 && nNpc < MAX_NPC && Npc[nNpc].m_dwID == s_dwPetNpcID[nPlayerIdx] &&
		szBuf[0])
	{
		memset(Npc[nNpc].Name, 0, sizeof(Npc[nNpc].Name));
		strncpy(Npc[nNpc].Name, szBuf, sizeof(Npc[nNpc].Name) - 1);
	}
}

//---------------------------------------------------------------------------
// [PETKN 31/08 - yeu cau chu] KY NANG PET AP LEN CHINH PET, khong ap len chu:
// - Skill MAC DINH 1600..1603 da doi thanh DON DANH vat ly (skills.txt) -
//   dat vao slot 10 skill-list NPC pet, sPetFight do_skill bang no.
//   Cap skill = (cap pet + 1) / 2, tran 63 (MAX_SKILLLEVEL=64).
// - 4 bi kip (id o task 5139..5142, CAP o 5166..5169 nang bang diem Tu Chan):
//   SetNpcSkill slot 11..14 - style 3 tu CastPassivitySkill ap m_StateAttribs
//   len PET vinh vien (-1); SetStateSkillEffect cung id cung cap chi refresh
//   gio (KHONG cong don - KNpc.cpp:7721), cap cao hon thi go cu ap moi.
// Tooltip client khong doc skill-list nguoi choi nua ma doc thang task value
// (CoreShell.cpp nhanh CGOG_SKILL_FIGHT [PETKN]).
//---------------------------------------------------------------------------
// [29/08] THUOC TINH BO trang bi Dong Hanh - bang goc
// settings\petsys\suitattrib.txt (EquipParticular = bac bo):
//   bac0: 233=5000 308=30 311=1 | bac1: 7500/40/2 | bac2: 10000/50/3
// Ma 233 = sinh luc toi da; 308/311 NGOAI DAI 305 attrib cua JX1 (ban
// private mo rong) -> hien chi ap 233.
#define PET_TV_SUITCOUNT   5163      // lua ghi: bo*100 + so mon dang mac
static int s_nSuitHp[3] = { 5000, 7500, 10000 };

static int s_nSuitLast[MAX_PLAYER];   // [30/08] theo doi doi bo de cap nhat mau ngay

// [30/08] AP THUOC TINH TRANG BI len NPC pet.
// Bang goc VLTK cho moi mon 3 thuoc tinh {ma, min, max}; Lua da roll gia
// tri va luu o 5170..5199, con MA attrib tra o bang
// settings\petsys\equipattrib.txt (sinh tu petequip_def.lua).
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
		s_EquipAttTab.Load((LPSTR)"\\settings\\petsys\\equipattrib.txt");
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
			{
				KMagicAttrib sAtt;
				memset(&sAtt, 0, sizeof(sAtt));
				sAtt.nAttribType = nMa[c];
				sAtt.nValue[0] = nVal;
				Npc[nNpcIdx].ModifyAttrib(0, &sAtt);
			}
		}
	}
}

static int sPetSuitAttrib(int nPlayerIdx)
{
	int nV = sPetG(nPlayerIdx, PET_TV_SUITCOUNT);
	if (nV <= 0) return 0;
	int nBo = nV / 100;
	int nSo = nV % 100;
	if (nBo < 0 || nBo > 2) return 0;
	if (nSo <= 0) return 0;
	if (nSo > 10) nSo = 10;
	// [30/08 phan bien] bang goc chi cho 3 gia tri theo BAC BO, khong ghi
	// nguong so mon; chuoi goc la "Bo <ten> %d mon" -> cong theo ti le
	// so mon dang mac (du 10 mon = tron gia tri bang goc).
	return s_nSuitHp[nBo] * nSo / 10;
}

// [PETKN2 01/09 - chu chot] don danh mac dinh cua pet = chieu NOI CONG 90
// theo HE cua CHU (vd chu he Hoa -> Phi Long Tai Thien cua Cai Bang - rong).
// Chon theo cot Series trong skills.txt, uu tien ban flat-damage/npc:
//   Kim(0)=327 Truy Tinh Truc Nguyet | Moc(1)=437 Truy Tinh Truc Dien npc
//   Thuy(2)=336 Bang Tung Vo Anh     | Hoa(3)=357 Phi Long Tai Thien
//   Tho(4)=438 Thai Cuc Vo Y npc     -- DOI CHIEU THI SUA O DAY
static const int s_nPetAtkBySeries[5] = { 327, 437, 336, 357, 438 };

// [PETKN2 01/09] cap chieu danh CHI theo cap pet (khong nang bang Tu Chan):
// cap = cap_pet * MaxLevel(bang skills.txt) / 130, lam tron len, tran 63.
// CLIENT tooltip dung CUNG cong thuc (CoreShell.cpp nhanh [PETKN]).
static int sPetAtkLevel(int nPetLevel, int nSkillId)
{
	int nMax = (int)g_SkillManager.GetSkillMaxLevel(nSkillId);
	if (nMax < 1 || nMax >= MAX_SKILLLEVEL) nMax = MAX_SKILLLEVEL - 1;
	int nLv = (nPetLevel * nMax + PET_MAX_LEVEL - 1) / PET_MAX_LEVEL;
	if (nLv < 1) nLv = 1;
	if (nLv > nMax) nLv = nMax;
	return nLv;
}

// [PETKN 31/08] cap bi kip o thu k (0..3): o 5166+k, chua nang = 1, tran 63
static int sPetExtLevel(int nPlayerIdx, int k)
{
	int nLv = sPetG(nPlayerIdx, PET_TV_EXTLV0 + k);
	if (nLv < 1) nLv = 1;
	if (nLv >= MAX_SKILLLEVEL) nLv = MAX_SKILLLEVEL - 1;
	return nLv;
}

static void sPetApplyPetSkills(int nPlayerIdx)
{
	int nNpc = s_nPetNpcIdx[nPlayerIdx];
	if (nNpc <= 0 || nNpc >= MAX_NPC) return;
	KNpc* pNpc = &Npc[nNpc];
	if (pNpc->m_dwID != s_dwPetNpcID[nPlayerIdx] ||
		pNpc->m_nPartnerNo != PET_PARTNER_NO) return;
	int nLevel = sPetG(nPlayerIdx, PET_TV_LEVEL);
	if (nLevel < 1) return;
	// [PETKN2 01/09] suc danh co ban theo CAP PET - template goc la NPC
	// thoai Min/MaxDamage=100 AR=100 nen chieu physicsenhance/AR danh nhu
	// gai (goc "danh khong co sat thuong"). Cong thuc o KPlayerPet.h.
	// Dat o day de len cap giua chung an ngay (goi tu summon/levelup/30s).
	{
		int nLvD = nLevel;
		if (nLvD > PET_MAX_LEVEL) nLvD = PET_MAX_LEVEL;
		pNpc->m_PhysicsDamage.nValue[0] = PET_DMGMIN(nLvD);
		pNpc->m_PhysicsDamage.nValue[1] = PET_DMGMAX(nLvD);
		pNpc->m_AttackRating = PET_ATKRATING(nLvD);
	}
	// don danh mac dinh = chieu 90 theo HE cua CHU (style 0)
	// - chi dat vao list + ghi id ra 5156 (client ve icon), sPetFight do_skill
	int nOwnerNpc = Player[nPlayerIdx].m_nIndex;
	if (nOwnerNpc > 0 && nOwnerNpc < MAX_NPC)
	{
		int nSeries = Npc[nOwnerNpc].m_Series;
		if (nSeries >= 0 && nSeries < 5)
		{
			int nAtk = s_nPetAtkBySeries[nSeries];
			pNpc->m_SkillList.SetNpcSkill(PET_SKILLSLOT_ATK, nAtk,
				sPetAtkLevel(nLevel, nAtk));
			if (sPetG(nPlayerIdx, PET_TV_ATKSKILL) != nAtk)
				sPetS(nPlayerIdx, PET_TV_ATKSKILL, nAtk);
		}
	}
	// 4 bi kip (style 3): SetNpcSkill tu CastPassivitySkill len PET (-1)
	for (int k = 0; k < 4; k++)
	{
		int nSk = sPetG(nPlayerIdx, 5139 + k);          // id tran (bikip.lua)
		if (nSk < PET_BIKIP_SKILL0 || nSk > PET_BIKIP_SKILL_MAX) continue;
		pNpc->m_SkillList.SetNpcSkill(PET_SKILLSLOT_EXT0 + k, nSk,
			sPetExtLevel(nPlayerIdx, k));
	}
	// [PETKN2 01/09] sinh luc toi da HIEU LUC (goc + trang bi + buff bi kip
	// vua ap o tren) - ghi ra 5159 cho cua so pet hien dung tong
	if (sPetG(nPlayerIdx, PET_TV_SHOWHP) != pNpc->m_CurrentLifeMax)
		sPetS(nPlayerIdx, PET_TV_SHOWHP, pNpc->m_CurrentLifeMax);
}

//---------------------------------------------------------------------------
static void sPetRemoveNpc(int nPlayerIdx)
{
	int nNpc = s_nPetNpcIdx[nPlayerIdx];
	s_nPetNpcIdx[nPlayerIdx] = 0;
	s_dwPetNpcID[nPlayerIdx] = 0;
	if (nNpc <= 0 || nNpc >= MAX_NPC) return;
	KNpc* pNpc = &Npc[nNpc];
	if (pNpc->m_dwID == 0 || pNpc->m_nPartnerNo != PET_PARTNER_NO) return;
	if (pNpc->m_RegionIndex >= 0)
	{
		int sw = pNpc->m_SubWorldIndex, rg = pNpc->m_RegionIndex;
		SubWorld[sw].m_Region[rg].RemoveNpc(nNpc);
		SubWorld[sw].m_Region[rg].DecRef(pNpc->m_MapX, pNpc->m_MapY, obj_npc);
	}
	NpcSet.Remove(nNpc);
}

static int sPetSummon(int nPlayerIdx)
{
	if (sPetG(nPlayerIdx, PET_TV_CREATE) != 1) return 0;
	if (s_nPetNpcIdx[nPlayerIdx] > 0) return 1;         // da goi
	KPlayer* pPlayer = &Player[nPlayerIdx];
	if (pPlayer->m_nIndex <= 0) return 0;
	KNpc* pOwner = &Npc[pPlayer->m_nIndex];
	int nTpl = sPetG(nPlayerIdx, PET_TV_FEATURE);
	if (nTpl <= 0) return 0;
	int nX = 0, nY = 0;
	pOwner->GetMpsPos(&nX, &nY);
	int nNpcIdx = NpcSet.AddNpcSet2(MAKELONG(1, nTpl), 0,
		pOwner->m_SubWorldIndex, nX + 48, nY + 48);
	if (nNpcIdx <= 0)
		nNpcIdx = NpcSet.AddNpcSet2(MAKELONG(1, nTpl), 0,
			pOwner->m_SubWorldIndex, nX, nY);
	if (nNpcIdx <= 0) return 0;
	KNpc* pNpc = &Npc[nNpcIdx];
	pNpc->m_Kind = kind_partner;
	pNpc->m_nPartnerOwner = nPlayerIdx;
	pNpc->m_nPartnerNo = PET_PARTNER_NO;
	pNpc->SetCamp(pOwner->m_CurrentCamp);
	pNpc->SetCurrentCamp(pOwner->m_CurrentCamp);
	pNpc->m_bNpcFollowFindPath = FALSE;
	// [29/08 - theo Linux] KPet::CreateNpc (0x081D5180) KHONG chinh toc do /
	// AI - moi chi so theo BANG npcs.txt cua template.
	memset(pNpc->Owner, 0, sizeof(pNpc->Owner));
	strncpy(pNpc->Owner, pPlayer->m_PlayerName, sizeof(pNpc->Owner) - 1);
	char szName[20];
	sPetGetName(nPlayerIdx, szName);
	if (szName[0])
	{
		memset(pNpc->Name, 0, sizeof(pNpc->Name));
		strncpy(pNpc->Name, szName, sizeof(pNpc->Name) - 1);
	}
	// [29/08] mau pet = ATTRIB Sinh luc (o 5122) - template 566.. la NPC
	// thoai LifeMax=0 nen thanh mau tren dau pet hien 0
	{
		// [29/08] + bonus trang bi pet (petequip.lua tinh tong vao 5157/5158)
		int nHp = sPetG(nPlayerIdx, PET_TV_ATTRIB0 + 4) + sPetSuitAttrib(nPlayerIdx);
		if (nHp > 0)
		{
			pNpc->m_LifeMax = nHp;
			// thanh mau client tinh theo m_CurrentLifeMax (nhu LuaSetNpcLife)
			pNpc->m_CurrentLifeMax = nHp;
		}
		int nMp = sPetG(nPlayerIdx, PET_TV_ATTRIB0 + 5);
		if (nMp > 0)
			pNpc->m_ManaMax = nMp;
		pNpc->m_CurrentMana = pNpc->m_ManaMax;
	}
	// (suc danh co ban theo cap dat trong sPetApplyPetSkills - de len cap
	// giua chung cung an ngay, khong doi goi lai)
	sPetApplyEquip(nPlayerIdx, nNpcIdx);	// [30/08] thuoc tinh trang bi
	s_nPetNpcIdx[nPlayerIdx] = nNpcIdx;
	s_dwPetNpcID[nPlayerIdx] = pNpc->m_dwID;
	s_dwAuraTick[nPlayerIdx] = 0;
	sPetS(nPlayerIdx, PET_TV_SUMMON, 1);
	sPetApplyPetSkills(nPlayerIdx);
	// [PETKN2 01/09] hoi day SAU khi trang bi + passive bi kip da cong vao
	// m_CurrentLifeMax (truoc day hoi theo m_LifeMax goc -> pet ra tran
	// trong nhu bi thieu mau khi co buff sinh luc)
	pNpc->m_CurrentLife = pNpc->m_CurrentLifeMax;
	return 1;
}

static void sPetUnSummon(int nPlayerIdx)
{
	// [PETKN 31/08] buff bi kip nam tren NPC pet - go npc la mat theo,
	// khong con gi phai go khoi CHU nua (truoc day go 4 vong sang).
	sPetRemoveNpc(nPlayerIdx);
	sPetS(nPlayerIdx, PET_TV_SUMMON, 0);
}

//---------------------------------------------------------------------------
// AI: pet PC CHI di theo chu - khong danh, khong bi danh (bang quan he da chan)
//---------------------------------------------------------------------------
void Pet_ProcessAI(int nNpcIdx)
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
// [30/08 - CHU CHON "bat danh"] PHAN THEM NGOAI BAN GOC: ca Linux lan VLTK
// pet CHI DI THEO. Chu bat FightMode -> moi ~1s chon dich mode 22 (ke vua
// danh chu / gan nhat, tam 480).
// [PETKN 31/08] doi sang danh bang SKILL MAC DINH 1600..1603 (don danh
// vat ly theo cap pet) thay cho bo skill template npcs.txt.
//---------------------------------------------------------------------------
extern int sPartnerPickTarget(int nNpcIdx, int nOwnerNpcIdx, int nMode, int nVision);
static DWORD s_dwFightTick[MAX_PLAYER];
// [PETKN 31/08] so khung con lai dang DUOI DANH muc tieu - sPetFollowLinux
// nhuong quyen di chuyen trong luc nay (chi giu luoi keo ve khi qua xa chu)
static int   s_nChaseTick[MAX_PLAYER];

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
	// [PETKN2 01/09] danh bang chieu 90 theo HE cua CHU (id o 5156, do
	// sPetApplyPetSkills ghi khi goi pet); cap CHI theo cap pet
	int nSkillId = sPetG(nPlayerIdx, PET_TV_ATKSKILL);
	if (nSkillId <= 0)
		return;
	int nWantLv = sPetAtkLevel(sPetG(nPlayerIdx, PET_TV_LEVEL), nSkillId);
	if (pNpc->m_SkillList.m_Skills[PET_SKILLSLOT_ATK].SkillId != nSkillId ||
		(int)pNpc->m_SkillList.m_Skills[PET_SKILLSLOT_ATK].SkillLevel != nWantLv)
		pNpc->m_SkillList.SetNpcSkill(PET_SKILLSLOT_ATK, nSkillId, nWantLv);
	int nTarget = sPartnerPickTarget(nNpcIdx, nOwnerNpc, 22, 480);
	if (nTarget <= 0 || nTarget >= MAX_NPC)
		return;
	// khuon KNpcAI::FollowAttack: trong tam thi danh, ngoai tam thi chay lai
	if (!pNpc->SetActiveSkill(PET_SKILLSLOT_ATK))
		return;
	int nDis = NpcSet.GetDistance(nNpcIdx, nTarget);
	if (nDis <= pNpc->m_CurrentAttackRadius)
	{
		pNpc->SendCommand(do_skill, nSkillId, -1, nTarget);
	}
	else
	{
		int nX = 0, nY = 0;
		Npc[nTarget].GetMpsPos(&nX, &nY);
		pNpc->SendCommand(do_walk, nX, nY);
		s_nChaseTick[nPlayerIdx] = 18;
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
	// [PETKN 31/08] dang duoi danh muc tieu (sPetFight vua ra lenh do_walk):
	// khong keo nguoc ve chu trong ~1s, tranh giang co lenh di chuyen
	if (s_nChaseTick[nPlayerIdx] > 0)
	{
		s_nChaseTick[nPlayerIdx]--;
		return;
	}
	int nGoX = nOX + ((nDX > 0) ? -100 : 100);
	int nGoY = nOY + ((nDY > 0) ? -100 : 100);
	pNpc->SendCommand(do_walk, nGoX, nGoY);
}

//---------------------------------------------------------------------------
// Nhip pet: don npc, theo chu qua map, follow, danh ([PETKN 31/08] bang
// skill mac dinh), lam moi skill/passive dinh ky.
//---------------------------------------------------------------------------
void Pet_Breathe()
{
	for (int i = 1; i < MAX_PLAYER; i++)
	{
		if (Player[i].m_nIndex <= 0)
		{
			if (s_nPetNpcIdx[i] > 0)
				sPetRemoveNpc(i);               // logout
			continue;
		}
		if (sPetG(i, PET_TV_SUMMON) != 1)
		{
			if (s_nPetNpcIdx[i] > 0)
				sPetRemoveNpc(i);
			continue;
		}
		int nNpc = s_nPetNpcIdx[i];
		if (nNpc <= 0 || nNpc >= MAX_NPC || Npc[nNpc].m_dwID != s_dwPetNpcID[i] ||
			Npc[nNpc].m_nPartnerNo != PET_PARTNER_NO)
		{
			// login lai / khe mat -> goi lai
			s_nPetNpcIdx[i] = 0;
			sPetSummon(i);
			continue;
		}
		KNpc* pOwner = &Npc[Player[i].m_nIndex];
		KNpc* pNpc = &Npc[nNpc];
		if (pNpc->m_SubWorldIndex != pOwner->m_SubWorldIndex)
		{
			// theo chu qua map (neu map cam thi script LeaveMap da UnSummon)
			int nX = 0, nY = 0;
			pOwner->GetMpsPos(&nX, &nY);
			pNpc->ChangeWorld(SubWorld[pOwner->m_SubWorldIndex].m_SubWorldID,
				nX + 48, nY + 48);
		}
		// [30/08 phan bien] deo/thao trang bi doi bo -> cap nhat mau NGAY,
		// khong doi den lan goi pet ke tiep
		{
			int nSuit = sPetG(i, PET_TV_SUITCOUNT);
			if (nSuit != s_nSuitLast[i])
			{
				s_nSuitLast[i] = nSuit;
				int nHp = sPetG(i, PET_TV_ATTRIB0 + 4) + sPetSuitAttrib(i);
				if (nHp > 0)
				{
					// [PETKN2 01/09] chi dich phan GOC (m_LifeMax), GIU
					// nguyen phan buff dang cong tren m_CurrentLifeMax
					// (bi kip Thiem Quang + thuoc tinh trang bi) - truoc
					// day ghi de tong lam MAT buff moi lan doi bo.
					int nDelta = nHp - Npc[nNpc].m_LifeMax;
					Npc[nNpc].m_LifeMax = nHp;
					Npc[nNpc].m_CurrentLifeMax += nDelta;
					if (Npc[nNpc].m_CurrentLifeMax < 1)
						Npc[nNpc].m_CurrentLifeMax = 1;
					if (Npc[nNpc].m_CurrentLife > Npc[nNpc].m_CurrentLifeMax)
						Npc[nNpc].m_CurrentLife = Npc[nNpc].m_CurrentLifeMax;
					sPetS(i, PET_TV_SHOWHP, Npc[nNpc].m_CurrentLifeMax);
				}
			}
		}
		// [29/08 - theo Linux] follow chay tu PLAYER TICK moi frame
		// (jx_linux_y goi KPet-follow tu 0x080B7104 trong player tick)
		sPetFollowLinux(i, nNpc);
		sPetFight(i, nNpc);	// [30/08] pet tu danh (phan them)
		if (++s_dwAuraTick[i] >= PET_AURA_RECAST)
		{
			s_dwAuraTick[i] = 0;
			// [PETKN 31/08] lam moi skill/passive tren PET (refresh cung
			// cap = chi refresh gio, khong cong don)
			sPetApplyPetSkills(i);
		}
	}
}

void Pet_OnLogout(int nPlayerIdx)
{
	sPetRemoveNpc(nPlayerIdx);
}

//---------------------------------------------------------------------------
// chay PetSys_Protocol(nOp) trong protocol_process_gs.lua, ngu canh player
//---------------------------------------------------------------------------
void Pet_RunProtocol(int nPlayerIdx, int nOp)
{
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER) return;
	// [28/08] chay qua KPlayer::ExecuteScript de m_ActionScriptID tro dung
	// script petsys (KPlayer.cpp:7092) - Say chon muc / AskClientForString
	// moi callback ve dung state; no cung tu inject PlayerIndex/PlayerID.
	Player[nPlayerIdx].ExecuteScript(
		(char*)"\\script\\petsys\\protocol_process_gs.lua",
		(char*)"PetSys_Protocol", nOp);
}

//---------------------------------------------------------------------------
// 22 ham Lua PET_* (dang ky o ScriptFuns.cpp)
//---------------------------------------------------------------------------
static int sPetCtx(Lua_State* L)
{
	extern int GetPlayerIndex(Lua_State * L);
	return GetPlayerIndex(L);
}

// 12 ham get/set viet TUONG MINH voi so o tran (28/08: ban macro no-op kho hieu
// tren binary truoc - viet thang + Rebuild sach de loai stale obj)
static int sPetGetAt(Lua_State* L, int nId)
{ Lua_PushNumber(L, sPetG(sPetCtx(L), nId)); return 1; }
static int sPetSetAt(Lua_State* L, int nId)
{ sPetS(sPetCtx(L), nId, (int)Lua_ValueToNumber(L, 1)); return 0; }

int LuaPET_GetLevel(Lua_State* L)        { return sPetGetAt(L, 5111); }
int LuaPET_SetLevel(Lua_State* L)        { return sPetSetAt(L, 5111); }
int LuaPET_GetUpgradePoint(Lua_State* L) { return sPetGetAt(L, 5112); }
int LuaPET_SetUpgradePoint(Lua_State* L) { return sPetSetAt(L, 5112); }
int LuaPET_GetGrownPoint(Lua_State* L)   { return sPetGetAt(L, 5113); }
int LuaPET_SetGrownPoint(Lua_State* L)   { return sPetSetAt(L, 5113); }
int LuaPET_GetTamePoint(Lua_State* L)    { return sPetGetAt(L, 5114); }
int LuaPET_SetTamePoint(Lua_State* L)    { return sPetSetAt(L, 5114); }
int LuaPET_GetXiuzhenPoint(Lua_State* L) { return sPetGetAt(L, 5115); }
int LuaPET_SetXiuzhenPoint(Lua_State* L) { return sPetSetAt(L, 5115); }
int LuaPET_GetFeatureId(Lua_State* L)    { return sPetGetAt(L, 5116); }
int LuaPET_SetFeatureId(Lua_State* L)    { return sPetSetAt(L, 5116); }

// [29/08 - bu theo audit] 15 ham ban Linux co ma JX1 thieu.
// attrib: 0=Str 1=Dex 2=Vit 3=Eng 4=Life 5=Mana (o PET_TV_ATTRIB0 + i)
static int sPetGetAttrAt(Lua_State* L, int nIdx)
{ Lua_PushNumber(L, sPetG(sPetCtx(L), PET_TV_ATTRIB0 + nIdx)); return 1; }
static int sPetSetAttrAt(Lua_State* L, int nIdx)
{ sPetS(sPetCtx(L), PET_TV_ATTRIB0 + nIdx, (int)Lua_ValueToNumber(L, 1)); return 0; }

int LuaPET_GetStr(Lua_State* L)  { return sPetGetAttrAt(L, 0); }
int LuaPET_GetDex(Lua_State* L)  { return sPetGetAttrAt(L, 1); }
int LuaPET_GetVit(Lua_State* L)  { return sPetGetAttrAt(L, 2); }
int LuaPET_GetEng(Lua_State* L)  { return sPetGetAttrAt(L, 3); }
int LuaPET_GetLife(Lua_State* L) { return sPetGetAttrAt(L, 4); }
int LuaPET_GetMana(Lua_State* L) { return sPetGetAttrAt(L, 5); }
int LuaPET_SetStr(Lua_State* L)  { return sPetSetAttrAt(L, 0); }
int LuaPET_SetDex(Lua_State* L)  { return sPetSetAttrAt(L, 1); }
int LuaPET_SetVit(Lua_State* L)  { return sPetSetAttrAt(L, 2); }
int LuaPET_SetEng(Lua_State* L)  { return sPetSetAttrAt(L, 3); }
int LuaPET_SetLife(Lua_State* L) { return sPetSetAttrAt(L, 4); }
int LuaPET_SetMana(Lua_State* L) { return sPetSetAttrAt(L, 5); }

// PET_SetAttrib(nIndex, nValue) - nIndex 0..5
int LuaPET_SetAttrib(Lua_State* L)
{
	int nIdx = (int)Lua_ValueToNumber(L, 1);
	if (nIdx < 0 || nIdx >= PET_ATTRIB_COUNT) return 0;
	sPetS(sPetCtx(L), PET_TV_ATTRIB0 + nIdx, (int)Lua_ValueToNumber(L, 2));
	return 0;
}

// PET_ClearSkill() - xoa 4 o ky nang
int LuaPET_ClearSkill(Lua_State* L)
{
	int nIdx = sPetCtx(L);
	for (int i = 0; i < PET_SKILL_COUNT; i++)
		sPetS(nIdx, PET_TV_SKILL0 + i, 0);
	return 0;
}

// PET_AddUpgradePoint(n) - CONG diem thang cap (Linux: moi hoat dong/ngay +1)
int LuaPET_AddUpgradePoint(Lua_State* L)
{
	int nIdx = sPetCtx(L);
	int nAdd = (int)Lua_ValueToNumber(L, 1);
	if (nIdx <= 0 || nAdd == 0) return 0;
	int nMoi = sPetG(nIdx, PET_TV_UPGRADE) + nAdd;
	if (nMoi < 0) nMoi = 0;
	sPetS(nIdx, PET_TV_UPGRADE, nMoi);
	return 0;
}

// [30/08] Go item KET TREN TAY (pos_hand) - con tro dinh mon do lam
// khong bam duoc NPC. Uu tien tra ve hanh trang; het cho thi xoa han.
int LuaPET_ClearHand(Lua_State* L)
{
	int nIdx = sPetCtx(L);
	int nDone = 0;
	if (nIdx <= 0 || nIdx >= MAX_PLAYER)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	for (int nVong = 0; nVong < 32; nVong++)
	{
		PlayerItem* pIt = Player[nIdx].m_ItemList.GetFirstItem();
		int nFound = 0;
		while (pIt)
		{
			if (pIt->nPlace == pos_hand && pIt->nIdx > 0)
			{
				nFound = pIt->nIdx;
				break;
			}
			pIt = Player[nIdx].m_ItemList.GetNextItem();
		}
		if (!nFound)
			break;
		int nX = 0, nY = 0;
		if (Player[nIdx].m_ItemList.CheckCanPlaceInEquipment(
			Item[nFound].GetWidth(), Item[nFound].GetHeight(), &nX, &nY))
			Player[nIdx].m_ItemList.AddKIL(nFound, pos_equiproom, nX, nY);
		else
		{
			Player[nIdx].m_ItemList.Remove(nFound);
			ItemSet.Remove(nFound);
		}
		nDone++;
	}
	Lua_PushNumber(L, nDone);
	return 1;
}

int LuaPET_IsCreate(Lua_State* L)
{
	Lua_PushNumber(L, sPetG(sPetCtx(L), PET_TV_CREATE) == 1 ? 1 : 0);
	return 1;
}

int LuaPET_Create(Lua_State* L)
{
	int nIdx = sPetCtx(L);
	int nTpl = (int)Lua_ValueToNumber(L, 1);
	const char* szName = Lua_ValueToString(L, 2);
	if (nIdx <= 0 || nTpl <= 0) { Lua_PushNumber(L, 0); return 1; }
	sPetS(nIdx, PET_TV_CREATE, 1);
	sPetS(nIdx, PET_TV_FEATURE, nTpl);
	sPetS(nIdx, PET_TV_LEVEL, 1);
	sPetSetName(nIdx, szName ? szName : "");
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaPET_Delete(Lua_State* L)
{
	int nIdx = sPetCtx(L);
	sPetUnSummon(nIdx);
	sPetS(nIdx, PET_TV_CREATE, 0);
	sPetS(nIdx, PET_TV_LEVEL, 0);
	sPetS(nIdx, PET_TV_FEATURE, 0);
	int i;
	for (i = 0; i < PET_ATTRIB_COUNT; i++)
		sPetS(nIdx, PET_TV_ATTRIB0 + i, 0);
	for (i = 0; i < PET_SKILL_COUNT; i++)
		sPetS(nIdx, PET_TV_SKILL0 + i, 0);
	for (i = 0; i < 4; i++)
		sPetS(nIdx, PET_TV_NAME0 + i, 0);
	// GIU 4 diem (thang cap / tang truong / tu luyen / Tu Chan) - dung ban goc
	return 0;
}

int LuaPET_IsSummon(Lua_State* L)
{
	int nIdx = sPetCtx(L);
	Lua_PushNumber(L, (s_nPetNpcIdx[nIdx] > 0 &&
		sPetG(nIdx, PET_TV_SUMMON) == 1) ? 1 : 0);
	return 1;
}

int LuaPET_Summon(Lua_State* L)
{
	Lua_PushNumber(L, sPetSummon(sPetCtx(L)));
	return 1;
}

int LuaPET_UnSummon(Lua_State* L)
{
	sPetUnSummon(sPetCtx(L));
	return 0;
}

int LuaPET_ClearAttrib(Lua_State* L)
{
	int nIdx = sPetCtx(L);
	for (int i = 0; i < PET_ATTRIB_COUNT; i++)
		sPetS(nIdx, PET_TV_ATTRIB0 + i, 0);
	return 0;
}

int LuaPET_AddAttrib(Lua_State* L)
{
	int nIdx = sPetCtx(L);
	int nAttr = (int)Lua_ValueToNumber(L, 1);           // 1..6
	int nVal = (int)Lua_ValueToNumber(L, 2);
	if (nAttr >= 1 && nAttr <= PET_ATTRIB_COUNT)
		sPetS(nIdx, PET_TV_ATTRIB0 + nAttr - 1,
			sPetG(nIdx, PET_TV_ATTRIB0 + nAttr - 1) + nVal);
	return 0;
}

int LuaPET_GetAttrib(Lua_State* L)
{
	int nIdx = sPetCtx(L);
	int nAttr = (int)Lua_ValueToNumber(L, 1);
	int nVal = 0;
	if (nAttr >= 1 && nAttr <= PET_ATTRIB_COUNT)
		nVal = sPetG(nIdx, PET_TV_ATTRIB0 + nAttr - 1);
	Lua_PushNumber(L, nVal);
	return 1;
}

int LuaPET_SetSkill(Lua_State* L)
{
	int nIdx = sPetCtx(L);
	int nSlot = (int)Lua_ValueToNumber(L, 1);           // 1..4
	int nVal = (int)Lua_ValueToNumber(L, 2);
	if (nSlot >= 1 && nSlot <= PET_SKILL_COUNT)
	{
		sPetS(nIdx, PET_TV_SKILL0 + nSlot - 1, nVal);
		if (s_nPetNpcIdx[nIdx] > 0)
			sPetApplyPetSkills(nIdx);	// [PETKN 31/08] Lua dung lam "refresh"
	}
	return 0;
}

int LuaPET_GetSkill(Lua_State* L)
{
	int nIdx = sPetCtx(L);
	int nSlot = (int)Lua_ValueToNumber(L, 1);
	int nVal = 0;
	if (nSlot >= 1 && nSlot <= PET_SKILL_COUNT)
		nVal = sPetG(nIdx, PET_TV_SKILL0 + nSlot - 1);
	Lua_PushNumber(L, nVal);
	return 1;
}

int LuaPET_SetFeatureId2(Lua_State* L)
{
	int nIdx = sPetCtx(L);
	int nTpl = (int)Lua_ValueToNumber(L, 1);
	if (nTpl > 0)
	{
		sPetS(nIdx, PET_TV_FEATURE, nTpl);
		// dang goi ra thi goi lai voi ngoai quan moi (ban goc UnSummon truoc)
		if (s_nPetNpcIdx[nIdx] > 0)
		{
			sPetUnSummon(nIdx);
			sPetSummon(nIdx);
		}
	}
	return 0;
}

int LuaPET_SetName(Lua_State* L)
{
	sPetSetName(sPetCtx(L), Lua_ValueToString(L, 1));
	return 0;
}

// [PETSYS] tru exp truc tiep (Linux ReduceOwnExp) - DirectAddExp xu am + sync
int LuaReduceOwnExp(Lua_State* L)
{
	int nIdx = sPetCtx(L);
	if (nIdx <= 0) return 0;
	double nExp = (double)Lua_ValueToNumber(L, 1);
	if (nExp > 0)
		Player[nIdx].DirectAddExp(-nExp);
	return 0;
}

int LuaPET_GetName(Lua_State* L)
{
	char szName[20];
	sPetGetName(sPetCtx(L), szName);
	Lua_PushString(L, szName);
	return 1;
}

#endif // _SERVER
