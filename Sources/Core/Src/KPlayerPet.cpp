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
#define PET_AURA_TIME    (40 * 18)   // 40 giay x 18 khung
#define PET_AURA_RECAST  60          // re-cast moi 60 luot breathe (~30s)

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
// AURA: ap skill state 1600..1603 (cap = cap pet) len CHU trong 40s;
// Pet_Breathe re-cast moi ~30s khi con goi ra -> thu ve tu rung.
//---------------------------------------------------------------------------
static void sPetApplyAura(int nPlayerIdx)
{
	int nKind = sPetG(nPlayerIdx, PET_TV_SKILL0);       // loai 1..4
	if (nKind < 1 || nKind > PET_SKILL_COUNT) return;
	int nLevel = sPetG(nPlayerIdx, PET_TV_LEVEL);
	if (nLevel < 1) return;
	if (nLevel > PET_MAX_LEVEL) nLevel = PET_MAX_LEVEL;
	int nOwnerNpc = Player[nPlayerIdx].m_nIndex;
	if (nOwnerNpc <= 0 || nOwnerNpc >= MAX_NPC) return;
	KSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(PET_AURA_SKILL0 + nKind - 1, nLevel);
	if (pSkill)
		pSkill->CastStateSkill(nOwnerNpc, 0, 0, PET_AURA_TIME, TRUE);
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
	memset(pNpc->Owner, 0, sizeof(pNpc->Owner));
	strncpy(pNpc->Owner, pPlayer->m_PlayerName, sizeof(pNpc->Owner) - 1);
	char szName[20];
	sPetGetName(nPlayerIdx, szName);
	if (szName[0])
	{
		memset(pNpc->Name, 0, sizeof(pNpc->Name));
		strncpy(pNpc->Name, szName, sizeof(pNpc->Name) - 1);
	}
	pNpc->m_CurrentLife = pNpc->m_LifeMax;
	s_nPetNpcIdx[nPlayerIdx] = nNpcIdx;
	s_dwPetNpcID[nPlayerIdx] = pNpc->m_dwID;
	s_dwAuraTick[nPlayerIdx] = 0;
	sPetS(nPlayerIdx, PET_TV_SUMMON, 1);
	sPetApplyAura(nPlayerIdx);
	return 1;
}

static void sPetUnSummon(int nPlayerIdx)
{
	sPetRemoveNpc(nPlayerIdx);
	sPetS(nPlayerIdx, PET_TV_SUMMON, 0);
}

//---------------------------------------------------------------------------
// AI: pet PC CHI di theo chu - khong danh, khong bi danh (bang quan he da chan)
//---------------------------------------------------------------------------
void Pet_ProcessAI(int nNpcIdx)
{
	KNpc* pNpc = &Npc[nNpcIdx];
	int nOwner = pNpc->m_nPartnerOwner;
	if (nOwner <= 0 || nOwner >= MAX_PLAYER || Player[nOwner].m_nIndex <= 0 ||
		s_nPetNpcIdx[nOwner] != nNpcIdx)
	{
		// chu bien mat / khe tai dung -> tu go
		if (pNpc->m_RegionIndex >= 0)
		{
			int sw = pNpc->m_SubWorldIndex, rg = pNpc->m_RegionIndex;
			SubWorld[sw].m_Region[rg].RemoveNpc(nNpcIdx);
			SubWorld[sw].m_Region[rg].DecRef(pNpc->m_MapX, pNpc->m_MapY, obj_npc);
		}
		NpcSet.Remove(nNpcIdx);
		return;
	}
	KNpc* pOwnerNpc = &Npc[Player[nOwner].m_nIndex];
	if (pNpc->m_SubWorldIndex != pOwnerNpc->m_SubWorldIndex)
		return;                                  // Pet_Breathe xu ly chuyen map
	int nPX = 0, nPY = 0, nOX = 0, nOY = 0;
	pNpc->GetMpsPos(&nPX, &nPY);
	pOwnerNpc->GetMpsPos(&nOX, &nOY);
	int nDis = abs(nPX - nOX) + abs(nPY - nOY);
	if (nDis > PET_FORCE_SYNC * 2)
	{
		pNpc->SetPos(nOX + 48, nOY + 48);
		return;
	}
	if (nDis > PET_FOLLOW_DIS)
	{
		int nXGo = nOX + 50 - rand() % 100;
		int nYGo = nOY + 50 - rand() % 100;
		pNpc->SendCommand(do_walk, nXGo, nYGo);
	}
}

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
		if (++s_dwAuraTick[i] >= PET_AURA_RECAST)
		{
			s_dwAuraTick[i] = 0;
			sPetApplyAura(i);
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
			sPetApplyAura(nIdx);
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
