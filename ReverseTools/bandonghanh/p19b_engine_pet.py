# -*- coding: utf-8 -*-
r"""[PETSYS 28/08] Engine C cho he Ban Dong Hanh PC (petsys):

1. KPlayerPet.h (MOI)  : hang PET_TV_* (ngoai _SERVER de client doc) + extern
2. KPlayerPet.cpp (MOI): luu task value 5110.. + 22 ham Lua PET_* + summon/
   unsummon (NPC kind_partner, m_nPartnerNo=100, AI follow-only) + AURA
   (CastStateSkill 1600..1603 len CHU, 40s, re-cast 30s) + Pet_Breathe +
   Pet_RunProtocol (goi PetSys_Protocol trong protocol_process_gs.lua)
3. ScriptFuns.cpp      : extern + dang ky 22 ham; IncludeLib +"PET"->noop
4. KPartnerProtocol.h  : PARTNER_OP_PETSYS = 30
5. KProtocolProcess.cpp: nhanh op PETSYS (TRUOC check Count partner)
6. KPlayerPartner.cpp  : Partner_ProcessAI re nhanh pet; KPartner_Breathe goi
   Pet_Breathe
7. Core.vcxproj        : dang ky 2 tep
Marker [PETSYS], sao luu .truoc_petsys. Idempotent.
"""
import io
import os
import shutil

BS = chr(92)
CR = chr(13)
LF = chr(10)
T = chr(9)
E = CR + LF

SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def ghi(p, s, bak=".truoc_petsys"):
    if os.path.exists(p) and not os.path.exists(p + bak):
        shutil.copyfile(p, p + bak)
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def patch(p, neo, moi, marker):
    s = doc(p)
    if marker in s:
        print("  da co:", os.path.basename(p), marker[:38])
        return
    n = s.count(neo)
    assert n == 1, "anchor %d lan trong %s: %r" % (n, p, neo[:70])
    ghi(p, s.replace(neo, moi, 1))
    print("  VA:", os.path.basename(p), marker[:38])


# ==================================================================== 1. .h
h = r'''//---------------------------------------------------------------------------
// KPlayerPet.h - He BAN DONG HANH ban PC (petsys), port 100% tu Linux
// D:\ServerLinux\server1\script\petsys + client VLTK pet.ini (UiPet).
// KHAC he partner mobile (KPlayerPartner) - hai he song song nhu ban goc.
// Luu bang TASK VALUE 5110..5139 (tu sync client qua UI_TASKVALUE).
//---------------------------------------------------------------------------
#ifndef KPLAYERPET_H
#define KPLAYERPET_H

#include "KWin32.h"

// so do task value (client doc qua GDI_TASK_SAVE_VALUE)
#define PET_TV_BASE        5110
#define PET_TV_CREATE      (PET_TV_BASE + 0)    // =1 da co pet
#define PET_TV_LEVEL       (PET_TV_BASE + 1)    // 1..20
#define PET_TV_UPGRADE     (PET_TV_BASE + 2)    // diem thang cap
#define PET_TV_GROWN       (PET_TV_BASE + 3)    // diem tang truong
#define PET_TV_TAME        (PET_TV_BASE + 4)    // diem tu luyen
#define PET_TV_XIUZHEN     (PET_TV_BASE + 5)    // diem Tu Chan
#define PET_TV_FEATURE     (PET_TV_BASE + 6)    // npc template id (ngoai quan)
#define PET_TV_SUMMON      (PET_TV_BASE + 7)    // 1 = dang goi ra (persist)
#define PET_TV_ATTRIB0     (PET_TV_BASE + 8)    // 6 o: STR DEX VIT ENG HP MP
#define PET_TV_SKILL0      (PET_TV_BASE + 14)   // 4 o; o 1 = loai aura 1..4
#define PET_TV_NAME0       (PET_TV_BASE + 18)   // 4 int = 16 byte ten

#define PET_ATTRIB_COUNT   6
#define PET_SKILL_COUNT    4
#define PET_MAX_LEVEL      20
#define PET_PARTNER_NO     100                  // m_nPartnerNo danh dau pet petsys
#define PET_AURA_SKILL0    1600                 // 4 skill aura 1600..1603 (p19a)

#ifdef _SERVER
void Pet_ProcessAI(int nNpcIdx);        // KPlayerPartner.cpp re sang khi nNo==100
void Pet_Breathe();                     // KPartner_Breathe goi (moi ~0.5s)
void Pet_RunProtocol(int nPlayerIdx, int nOp);  // c2s_partnerop op PETSYS
void Pet_OnLogout(int nPlayerIdx);      // go npc khi thoat
#endif

#endif // KPLAYERPET_H
'''

# ==================================================================== 2. .cpp
cpp = r'''//---------------------------------------------------------------------------
// KPlayerPet.cpp - He BAN DONG HANH ban PC (petsys). Xem KPlayerPet.h.
// Nguon ngu nghia 100%: D:\ServerLinux\server1\script\petsys\*.lua
// (C-API "PET" cua Linux la lib strip - hanh vi suy tu cach lua goi).
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KCore.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KSkills.h"
#include "KLuaScript.h"
#include "KPlayerPet.h"
#include "KPlayerPartner.h"

#ifdef _SERVER

extern KPlayer* Player;
extern KNpc* Npc;
extern KNpcSet NpcSet;
extern KSubWorldSet SubWorld;
extern KSkills g_SkillManager;
extern void* g_GetScript(const char* szFile);

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
	KLuaScript* pScript = (KLuaScript*)g_GetScript(
		"\\script\\petsys\\protocol_process_gs.lua");
	if (!pScript || !pScript->m_LuaState) return;
	Lua_PushNumber(pScript->m_LuaState, nPlayerIdx);
	pScript->SetGlobalName((LPSTR)SCRIPT_PLAYERINDEX);
	pScript->CallFunction((LPSTR)"PetSys_Protocol", 0, (LPSTR)"d", nOp);
}

//---------------------------------------------------------------------------
// 22 ham Lua PET_* (dang ky o ScriptFuns.cpp)
//---------------------------------------------------------------------------
static int sPetCtx(Lua_State* L)
{
	extern int GetPlayerIndex(Lua_State * L);
	return GetPlayerIndex(L);
}

#define PET_GETSET(TEN, OFF) \
int LuaPET_Get##TEN(Lua_State* L) \
{ Lua_PushNumber(L, sPetG(sPetCtx(L), OFF)); return 1; } \
int LuaPET_Set##TEN(Lua_State* L) \
{ sPetS(sPetCtx(L), OFF, (int)Lua_ValueToNumber(L, 1)); return 0; }

PET_GETSET(Level, PET_TV_LEVEL)
PET_GETSET(UpgradePoint, PET_TV_UPGRADE)
PET_GETSET(GrownPoint, PET_TV_GROWN)
PET_GETSET(TamePoint, PET_TV_TAME)
PET_GETSET(XiuzhenPoint, PET_TV_XIUZHEN)
PET_GETSET(FeatureId, PET_TV_FEATURE)

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

int LuaPET_GetName(Lua_State* L)
{
	char szName[20];
	sPetGetName(sPetCtx(L), szName);
	Lua_PushString(L, szName);
	return 1;
}

#endif // _SERVER
'''

ghi(os.path.join(SRC, "KPlayerPet.h"), h.replace(LF, E) if E not in h else h, ".khongco")
ghi(os.path.join(SRC, "KPlayerPet.cpp"), cpp.replace(LF, E) if E not in cpp else cpp, ".khongco")
print("ghi KPlayerPet.h/.cpp")

# ==================================================================== 3. ScriptFuns
p = os.path.join(SRC, "ScriptFuns.cpp")
s = doc(p)
if "LuaPET_IsCreate" not in s:
    neo = "extern int LuaGetNpcSettingIdx(Lua_State* L);"
    assert s.count(neo) == 1
    ext = neo + E + E + "// [PETSYS 28/08] he Ban Dong Hanh PC (KPlayerPet.cpp)" + E
    for fn in ["IsCreate", "Create", "Delete", "IsSummon", "Summon", "UnSummon",
               "GetLevel", "SetLevel", "GetUpgradePoint", "SetUpgradePoint",
               "GetGrownPoint", "SetGrownPoint", "GetTamePoint", "SetTamePoint",
               "GetXiuzhenPoint", "SetXiuzhenPoint", "GetFeatureId",
               "SetFeatureId2", "ClearAttrib", "AddAttrib", "GetAttrib",
               "SetSkill", "GetSkill", "SetName", "GetName"]:
        ext += "extern int LuaPET_%s(Lua_State* L);" % fn + E
    s = s.replace(neo, ext, 1)

    neo2 = T + T + '{ "GetNpcSettingIdx",' + T + "LuaGetNpcSettingIdx },"
    assert s.count(neo2) == 1, "khong thay dang ky GetNpcSettingIdx"
    dk = neo2 + E + T + T + "// [PETSYS 28/08]" + E
    for lua_ten, c_ten in [
            ("PET_IsCreate", "IsCreate"), ("PET_Create", "Create"),
            ("PET_Delete", "Delete"), ("PET_IsSummon", "IsSummon"),
            ("PET_Summon", "Summon"), ("PET_UnSummon", "UnSummon"),
            ("PET_GetLevel", "GetLevel"), ("PET_SetLevel", "SetLevel"),
            ("PET_GetUpgradePoint", "GetUpgradePoint"),
            ("PET_SetUpgradePoint", "SetUpgradePoint"),
            ("PET_GetGrownPoint", "GetGrownPoint"),
            ("PET_SetGrownPoint", "SetGrownPoint"),
            ("PET_GetTamePoint", "GetTamePoint"),
            ("PET_SetTamePoint", "SetTamePoint"),
            ("PET_GetXiuzhenPoint", "GetXiuzhenPoint"),
            ("PET_SetXiuzhenPoint", "SetXiuzhenPoint"),
            ("PET_GetFeatureId", "GetFeatureId"),
            ("PET_SetFeatureId", "SetFeatureId2"),
            ("PET_ClearAttrib", "ClearAttrib"), ("PET_AddAttrib", "AddAttrib"),
            ("PET_GetAttrib", "GetAttrib"), ("PET_SetSkill", "SetSkill"),
            ("PET_GetSkill", "GetSkill"), ("PET_SetName", "SetName"),
            ("PET_GetName", "GetName")]:
        dk += T + T + '{ "%s",' % lua_ten + T + "LuaPET_%s }," % c_ten + E
    s = s.replace(neo2, dk, 1)
    ghi(p, s)
    print("  VA: ScriptFuns extern + 25 dang ky PET_*")
else:
    print("  da co: ScriptFuns PET_*")

# IncludeLib +PET (vung nay LF thuan)
s = doc(p)
if '"PET",' not in s:
    neo = 'static const char* szMod[21] = {'
    assert s.count(neo) == 1
    s = s.replace(neo, 'static const char* szMod[22] = {', 1)
    neo = T + T + '"ITEM",' + LF
    assert s.count(neo) == 1, "anchor ITEM"
    s = s.replace(neo, T + T + '"ITEM", "PET",' + T +
                  "// [PETSYS 28/08] PET: ham that o C (KPlayerPet.cpp)" + LF, 1)
    neo = 'static const char* szFile[21] = {'
    assert s.count(neo) == 1
    s = s.replace(neo, 'static const char* szFile[22] = {', 1)
    i = s.find('szFile[22]')
    j = s.find("};", i)
    chot = s.rfind('noop.lua",', i, j)
    assert chot > 0
    chot_end = chot + len('noop.lua",')
    s = s[:chot_end] + ' "scriptjx2' + BS*2 + 'lib' + BS*2 + 'noop.lua",' + s[chot_end:]
    ghi(p, s)
    print("  VA: IncludeLib +PET")
else:
    print("  da co: IncludeLib PET")

# ==================================================================== 4. protocol op
patch(os.path.join(r"D:\GAMEDEVNEW\Headers", "KPartnerProtocol.h"),
      "    PARTNER_OP_DELETE      = 8,     // (mo qua doi thoai - nhu TALK)" + E,
      "    PARTNER_OP_DELETE      = 8,     // (mo qua doi thoai - nhu TALK)" + E +
      "    PARTNER_OP_PETSYS      = 30,    // [PETSYS] nParam = PET_OPERATION_* (1..7)" + E,
      "PARTNER_OP_PETSYS")

# ==================================================================== 5. handler
p = os.path.join(SRC, "KProtocolProcess.cpp")
patch(p,
      T + "KPartnerSys* pSys = &Player[nIndex].m_cPartner;" + E,
      T + "// [PETSYS 28/08] he Ban Dong Hanh PC: khong doi hoi co partner mobile" + E +
      T + "if (pInfo->btOp == PARTNER_OP_PETSYS)" + E +
      T + "{" + E +
      T + T + "Pet_RunProtocol(nIndex, pInfo->nParam);" + E +
      T + T + "return;" + E +
      T + "}" + E + E +
      T + "KPartnerSys* pSys = &Player[nIndex].m_cPartner;" + E,
      "PARTNER_OP_PETSYS)")
s = doc(p)
if '#include "KPlayerPet.h"' not in s:
    neo = '#include "KPlayerPartner.h"'
    assert s.count(neo) == 1
    ghi(p, s.replace(neo, neo + E + '#include "KPlayerPet.h"' + T + "// [PETSYS]", 1))
    print("  VA include KPlayerPet.h vao KProtocolProcess.cpp")

# ==================================================================== 6. AI + Breathe hook
p = os.path.join(SRC, "KPlayerPartner.cpp")
s = doc(p)
LE = LF  # file nay LF thuan
if "Pet_ProcessAI" not in s:
    neo = "void Partner_ProcessAI(int nNpcIdx)" + LE + "{" + LE
    assert s.count(neo) == 1, s.count(neo)
    moi = (neo +
           T + "// [PETSYS 28/08] pet PC (petsys) dung chung kind_partner, nNo=100" + LE +
           T + "if (Npc[nNpcIdx].m_nPartnerNo == PET_PARTNER_NO)" + LE +
           T + "{" + LE +
           T + T + "Pet_ProcessAI(nNpcIdx);" + LE +
           T + T + "return;" + LE +
           T + "}" + LE)
    s = s.replace(neo, moi, 1)
    neo2 = '#include "KPlayerPartner.h"'
    assert s.count(neo2) == 1
    s = s.replace(neo2, neo2 + LE + '#include "KPlayerPet.h"' + T + "// [PETSYS]", 1)
    ghi(p, s)
    print("  VA: Partner_ProcessAI re nhanh pet")
else:
    print("  da co: nhanh pet trong AI")

# KPartner_Breathe -> Pet_Breathe: tim than ham KPartner_Breathe
s = doc(p)
if "Pet_Breathe();" not in s:
    neo = "void KPartner_Breathe()" + LE + "{" + LE
    assert s.count(neo) == 1, "khong thay KPartner_Breathe"
    s = s.replace(neo, neo + T + "Pet_Breathe();" + T + "// [PETSYS 28/08]" + LE, 1)
    ghi(p, s)
    print("  VA: KPartner_Breathe goi Pet_Breathe")
else:
    print("  da co: Pet_Breathe hook")

# ==================================================================== 7. vcxproj
p = r"D:\GAMEDEVNEW\Sources\Core\Core.vcxproj"
patch(p,
      '    <ClCompile Include="Src\\KPlayerPartner.cpp">' + E + "    </ClCompile>" + E,
      '    <ClCompile Include="Src\\KPlayerPartner.cpp">' + E + "    </ClCompile>" + E +
      '    <ClCompile Include="Src\\KPlayerPet.cpp">' + E + "    </ClCompile>" + E,
      "KPlayerPet.cpp")
patch(p,
      '    <ClInclude Include="Src\\KPlayerPartner.h" />' + E,
      '    <ClInclude Include="Src\\KPlayerPartner.h" />' + E +
      '    <ClInclude Include="Src\\KPlayerPet.h" />' + E,
      "KPlayerPet.h")

print("XONG p19b")
