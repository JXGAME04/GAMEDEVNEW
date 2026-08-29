//---------------------------------------------------------------------------
// KPlayerPartner.cpp - He BAN DONG HANH (port 100% ngu nghia ban Linux VNG)
//
// Xem KPlayerPartner.h ve so do luu task-value + PHANTICH_BANDONGHANH_2708.md.
// Toan bo phan than nam trong #ifdef _SERVER (file nay bien dich vao ca
// CoreClient lan CoreServer - phia client chi la don vi rong).
//---------------------------------------------------------------------------
#include "KCore.h"
#include "KPlayerPartner.h"
#include "KPlayerPet.h"	// [PETSYS]

#ifdef _SERVER

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KSubWorldSet.h"
#include "KPlayerChat.h"
#include "KIniFile.h"
#include "KTabFile.h"
#include "KLuaScript.h"
#include "KSortScript.h"

// Npc/Player/SubWorld/NpcSet/PlayerSet da extern trong header tuong ung;
// KPlayerChat::SendSystemInfo / NpcChat la ham static.
extern int GetPlayerIndex(Lua_State* L);        // ScriptFuns.cpp:10039

KPartnerTables g_PartnerTables;

//---------------------------------------------------------------------------
// tien ich chung
//---------------------------------------------------------------------------
static DWORD sPartnerRand(DWORD* pSeed)         // LCG kieu MSVC - tai lap duoc
{
	*pSeed = (*pSeed) * 214013u + 2531011u;
	return ((*pSeed) >> 16) & 0x7fff;
}

static float sRandRangeF(DWORD* pSeed, float fMin, float fMax)
{
	if (fMax <= fMin) return fMin;
	return fMin + (fMax - fMin) * ((float)sPartnerRand(pSeed) / 32767.0f);
}

void Partner_WriteLog(const char* fmt, ...)
{
	// khuon LuaCmp_WriteCompoundLog (KItemCompound.cpp:1587) - ghi Logs\KSG_PartnerLog_*.txt
	char szMsg[1024];
	va_list ap;
	va_start(ap, fmt);
	_vsnprintf(szMsg, sizeof(szMsg) - 4, fmt, ap);
	szMsg[sizeof(szMsg) - 4] = 0;
	va_end(ap);

	time_t tNow = time(NULL);
	struct tm* pTm = localtime(&tNow);
	if (!pTm) return;
	char szFile[128];
	_snprintf(szFile, sizeof(szFile) - 1, "Logs\\KSG_PartnerLog_%04d%02d%02d.txt",
		pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday);
	FILE* fp = fopen(szFile, "a+");
	if (!fp) return;
	fprintf(fp, "[%02d:%02d:%02d] %s\r\n", pTm->tm_hour, pTm->tm_min, pTm->tm_sec, szMsg);
	fclose(fp);
}

//---------------------------------------------------------------------------
// KPartnerTables::Load - nap 11 bang \settings\partner\
//---------------------------------------------------------------------------
static void sLoadIniInt(KIniFile* pIni, const char* szSect, const char* szKey, int nDef, int* pnOut)
{
	pIni->GetInteger((LPSTR)szSect, (LPSTR)szKey, nDef, pnOut);
}

BOOL KPartnerTables::Load()
{
	if (bLoaded) return TRUE;
	memset(this, 0, sizeof(*this));

	// ---- partner_setting.ini ----
	{
		KIniFile Ini;
		if (!Ini.Load((LPSTR)"\\settings\\partner\\partner_setting.ini"))
		{
			printf("[Partner] LOI: thieu \\settings\\partner\\partner_setting.ini\n");
			return FALSE;
		}
		sLoadIniInt(&Ini, "EMOTION_DEGREE", "EMOTION_MIN", 0, &Setting.nEmotionMin);
		sLoadIniInt(&Ini, "EMOTION_DEGREE", "EMOTION_MAX", 100, &Setting.nEmotionMax);
		sLoadIniInt(&Ini, "EMOTION_DEGREE", "EMOTION_INITIALIZE", 50, &Setting.nEmotionInit);
		sLoadIniInt(&Ini, "EMOTION_DEGREE", "EMOTION_DEG_BASE", 50, &Setting.nEmotionDegBase);
		sLoadIniInt(&Ini, "EMOTION_DEGREE", "EMOTION_DEG_STEP", 7200, &Setting.nEmotionDegStep);
		sLoadIniInt(&Ini, "DEATH_PUNISH", "PUNISH_TIME", 300, &Setting.nPunishTime);
		sLoadIniInt(&Ini, "CALLOUT", "CALLOUT_INTERVAL", 30, &Setting.nCalloutInterval);
		sLoadIniInt(&Ini, "REVIVE", "LIFE_RESTORE", 200, &Setting.nLifeRestore);
	}

	// ---- feature.txt ----
	{
		KTabFile Tab;
		if (!Tab.Load((LPSTR)"\\settings\\partner\\feature.txt")) return FALSE;
		for (int r = 2; r <= Tab.GetHeight(); r++)
		{
			int nId = 0;
			Tab.GetInteger(r, 1, 0, &nId);
			if (nId < 1 || nId > PARTNER_FEATURE_NUM) continue;
			Tab.GetInteger(r, 2, 0, &Feature[nId].nNpcIdxPeriod[0]);
			Tab.GetInteger(r, 3, 0, &Feature[nId].nNpcIdxPeriod[1]);
			Tab.GetInteger(r, 4, 0, &Feature[nId].nNpcIdxPeriod[2]);
			Tab.GetInteger(r, 5, 0, &Feature[nId].nSex);
		}
	}

	// ---- character.txt (header 17 cot nhung data 15 so + 2 chuoi - doc theo chi so cot) ----
	{
		KTabFile Tab;
		if (!Tab.Load((LPSTR)"\\settings\\partner\\character.txt")) return FALSE;
		for (int r = 2; r <= Tab.GetHeight(); r++)
		{
			int nId = 0;
			Tab.GetInteger(r, 1, 0, &nId);
			if (nId < 1 || nId > PARTNER_CHARACTER_NUM) continue;
			KPartnerCharacterRow* p = &Character[nId];
			Tab.GetInteger(r, 2, 500, &p->nVisionRadius);
			Tab.GetInteger(r, 3, 500, &p->nActiveRadius);
			Tab.GetInteger(r, 4, 800, &p->nForceSync);
			Tab.GetInteger(r, 5, 6, &p->nAIMaxTime);
			Tab.GetInteger(r, 6, 21, &p->nAIMode);
			for (int k = 0; k < 9; k++)
				Tab.GetInteger(r, 7 + k, 0, &p->nAIParam[k]);
		}
	}

	// ---- resist.txt ----
	{
		KTabFile Tab;
		if (!Tab.Load((LPSTR)"\\settings\\partner\\resist.txt")) return FALSE;
		for (int r = 2; r <= Tab.GetHeight(); r++)
		{
			int nSeries = -1;
			Tab.GetInteger(r, 1, -1, &nSeries);
			if (nSeries < 0 || nSeries >= PARTNER_SERIES_NUM) continue;
			for (int k = 0; k < PARTNER_RESIST_NUM; k++)
			{
				Tab.GetFloat(r, 2 + k * 2, 0.0f, &Resist[nSeries].fInit[k]);
				Tab.GetFloat(r, 3 + k * 2, 0.0f, &Resist[nSeries].fInc[k]);
			}
		}
	}

	// ---- aptitude_range.txt: APTITUDE + 6 cap (MIN,MAX) ----
	{
		KTabFile Tab;
		if (!Tab.Load((LPSTR)"\\settings\\partner\\aptitude_range.txt")) return FALSE;
		for (int r = 2; r <= Tab.GetHeight(); r++)
		{
			int nApt = 0;
			Tab.GetInteger(r, 1, 0, &nApt);
			if (nApt < 1 || nApt > 10) continue;
			for (int k = 0; k < PARTNER_APT_NUM; k++)
			{
				Tab.GetFloat(r, 2 + k * 2, 0.0f, &AptRange[nApt].fMin[k]);
				Tab.GetFloat(r, 3 + k * 2, 0.0f, &AptRange[nApt].fMax[k]);
			}
		}
	}

	// ---- attrib_range.txt: SERIES + 6 nhom x 4 cot (INIT_MIN INIT_MAX INC_MIN INC_MAX) ----
	{
		KTabFile Tab;
		if (!Tab.Load((LPSTR)"\\settings\\partner\\attrib_range.txt")) return FALSE;
		for (int r = 2; r <= Tab.GetHeight(); r++)
		{
			int nSeries = -1;
			Tab.GetInteger(r, 1, -1, &nSeries);
			if (nSeries < 0 || nSeries >= PARTNER_SERIES_NUM) continue;
			KPartnerAttribRangeRow* p = &AttribRange[nSeries];
			for (int k = 0; k < PARTNER_APT_NUM; k++)
			{
				Tab.GetFloat(r, 2 + k * 4, 0.0f, &p->fInitMin[k]);
				Tab.GetFloat(r, 3 + k * 4, 0.0f, &p->fInitMax[k]);
				Tab.GetFloat(r, 4 + k * 4, 0.0f, &p->fIncMin[k]);
				Tab.GetFloat(r, 5 + k * 4, 0.0f, &p->fIncMax[k]);
			}
		}
	}

	// ---- aptitude_mode.txt: MODE_ID, DESC, roi 6 nhom x 10 cot trong so ----
	{
		KTabFile Tab;
		if (!Tab.Load((LPSTR)"\\settings\\partner\\aptitude_mode.txt")) return FALSE;
		for (int r = 2; r <= Tab.GetHeight(); r++)
		{
			int nMode = 0;
			Tab.GetInteger(r, 1, 0, &nMode);
			if (nMode < 1 || nMode > 2) continue;
			for (int a = 0; a < PARTNER_APT_NUM; a++)
				for (int t = 0; t < 10; t++)
					Tab.GetInteger(r, 3 + a * 10 + t, 0, &AptMode[nMode].nWeight[a][t]);
		}
	}

	// ---- level_exp.txt ----
	{
		KTabFile Tab;
		if (!Tab.Load((LPSTR)"\\settings\\partner\\level_exp.txt")) return FALSE;
		for (int r = 2; r <= Tab.GetHeight(); r++)
		{
			int nLv = 0, nExp = 0;
			Tab.GetInteger(r, 1, 0, &nLv);
			Tab.GetInteger(r, 2, 0, &nExp);
			if (nLv >= 1 && nLv <= PARTNER_MAX_LEVEL)
				nLevelExp[nLv] = nExp;
		}
	}

	// ---- init_skill.ini ----
	{
		KIniFile Ini;
		if (!Ini.Load((LPSTR)"\\settings\\partner\\init_skill.ini")) return FALSE;
		for (int s = 0; s < PARTNER_SERIES_NUM; s++)
		{
			char szSect[64];
			sprintf(szSect, "%d_INIT_SKILL", s);
			sLoadIniInt(&Ini, szSect, "COUNT", 0, &InitSkill[s].nCount);
			if (InitSkill[s].nCount > 4) InitSkill[s].nCount = 4;
			for (int k = 0; k < InitSkill[s].nCount; k++)
			{
				sprintf(szSect, "%d_INIT_SKILL_%d", s, k + 1);
				sLoadIniInt(&Ini, szSect, "SKILL_TYPE", 1, &InitSkill[s].nType[k]);
				sLoadIniInt(&Ini, szSect, "SKILL_ID", 0, &InitSkill[s].nSkillId[k]);
				sLoadIniInt(&Ini, szSect, "SKILL_LEVEL", 1, &InitSkill[s].nLevel[k]);
				sLoadIniInt(&Ini, szSect, "SKILL_EXP", 0, &InitSkill[s].nExp[k]);
			}
		}
	}

	// ---- partner_bag.ini ----
	{
		KIniFile Ini;
		if (!Ini.Load((LPSTR)"\\settings\\partner\\partner_bag.ini")) return FALSE;
		int nColor = 419430560;
		Ini.GetInteger((LPSTR)"Main", (LPSTR)"UnActiveColor", 419430560, &nColor);
		uUnActiveColor = (unsigned int)nColor;
		for (int lv = 0; lv <= 10; lv++)
		{
			char szKey[32], szVal[64];
			sprintf(szKey, "Level_%02d", lv);
			szVal[0] = 0;
			Ini.GetString((LPSTR)"LEVEL_GRID", szKey, (LPSTR)"0,0", szVal, sizeof(szVal));
			int w = 0, h = 0;
			sscanf(szVal, "%d,%d", &w, &h);
			nBagGrid[lv][0] = w;
			nBagGrid[lv][1] = h;
		}
	}

	// ---- partner_event.ini ----
	{
		KIniFile Ini;
		if (Ini.Load((LPSTR)"\\settings\\partner\\partner_event.ini"))
		{
			Ini.GetString((LPSTR)"Main", (LPSTR)"HostName_Man", (LPSTR)"", szHostNameMan, sizeof(szHostNameMan));
			Ini.GetString((LPSTR)"Main", (LPSTR)"HostName_Woman", (LPSTR)"", szHostNameWoman, sizeof(szHostNameWoman));
			Ini.GetString((LPSTR)"Main", (LPSTR)"Format", (LPSTR)"", szFormat, sizeof(szFormat));

			static const char* s_szEvents[] = {
				"PartnerCallout", "PartnerBloodLess", "PartnerBloodFallQuickly",
				"PartnerKillAnimal", "PartnerKillBoss", "PartnerKillPlayer", "PartnerKillPartner",
				"PartnerLevelUp", "PartnerBeDrawBack", "PartnerCastToHost",
				"HostBloodLess", "HostBloodFallQuickly", "HostKillAnimal", "HostKillBoss",
				"HostKillPlayer", "HostKillPartner", "HostLevelUp", "HostEnmity", "HostBeEnmityed",
				"HostTeamInvite", "HostTradeInvite", "WorldKillerAppear", "HostFriendInvite",
				"HostEquipBreaking", "WorldBossAppear", "WorldEnemyAppear", "TownPortalUseOut",
				"GoldItemDrop", "HostPkTooHigh", "TakeTantoGolds", "GameNewSupply",
			};
			nEventCount = 0;
			for (int e = 0; e < (int)(sizeof(s_szEvents) / sizeof(s_szEvents[0])) && nEventCount < PARTNER_EVENT_MAX; e++)
			{
				KPartnerEventRow* p = &Event[nEventCount];
				int nOdds = -1;
				Ini.GetInteger((LPSTR)s_szEvents[e], (LPSTR)"Odds", -1, &nOdds);
				if (nOdds < 0) continue;                 // section khong ton tai
				strncpy(p->szName, s_szEvents[e], sizeof(p->szName) - 1);
				p->nOdds = nOdds;
				Ini.GetInteger((LPSTR)s_szEvents[e], (LPSTR)"Param1", 0, &p->nParam1);
				p->nCountM = p->nCountF = 0;
				for (int d = 0; d < PARTNER_EVENT_DIALOG; d++)
				{
					char szKey[16];
					sprintf(szKey, "DialogM%d", d + 1);
					p->szDialogM[d][0] = 0;
					Ini.GetString((LPSTR)s_szEvents[e], szKey, (LPSTR)"", p->szDialogM[d], sizeof(p->szDialogM[d]));
					if (p->szDialogM[d][0]) p->nCountM = d + 1;
					sprintf(szKey, "DialogF%d", d + 1);
					p->szDialogF[d][0] = 0;
					Ini.GetString((LPSTR)s_szEvents[e], szKey, (LPSTR)"", p->szDialogF[d], sizeof(p->szDialogF[d]));
					if (p->szDialogF[d][0]) p->nCountF = d + 1;
				}
				nEventCount++;
			}
		}
	}

	bLoaded = TRUE;
	printf("[Partner] Da nap bang cau hinh dong hanh (event=%d)\n", nEventCount);
	return TRUE;
}

//---------------------------------------------------------------------------
// KPartnerSys - kho task value
//---------------------------------------------------------------------------
void KPartnerSys::Init(int nPlayerIdx)
{
	m_nPlayerIdx = nPlayerIdx;
	m_nNpcIdx = 0;
	m_dwNpcID = 0;
	m_nNpcOfPartner = 0;
	m_dwLastRegenTick = 0;
	m_dwLastEmoTick = 0;
}

int  KPartnerSys::GetG(int nId) const           { return Player[m_nPlayerIdx].m_cTask.GetSaveVal(nId); }
void KPartnerSys::SetG(int nId, int nVal)       { Player[m_nPlayerIdx].m_cTask.SetSaveVal(nId, nVal); }
int  KPartnerSys::GetP(int nP, int nOff) const  { return GetG(PARTNER_TASK_P(nP - 1) + nOff); }
void KPartnerSys::SetP(int nP, int nOff, int nVal) { SetG(PARTNER_TASK_P(nP - 1) + nOff, nVal); }

BOOL KPartnerSys::IsUsed(int nP) const
{
	if (nP < 1 || nP > PARTNER_MAX_COUNT) return FALSE;
	return GetP(nP, PTP_USED) != 0;
}

int KPartnerSys::Count() const
{
	if (m_nPlayerIdx <= 0) return -1;
	int n = 0;
	for (int i = 1; i <= PARTNER_MAX_COUNT; i++)
		if (IsUsed(i)) n++;
	return n;
}

int KPartnerSys::GetCur() const
{
	int nCur = GetG(PTG_CURPARTNER);
	if (nCur < 1 || nCur > PARTNER_MAX_COUNT || !IsUsed(nCur)) return 0;
	return nCur;
}

BOOL KPartnerSys::IsCallOut() const
{
	return (GetG(PTG_CALLOUT) != 0) && m_nNpcIdx > 0;
}

int KPartnerSys::PeriodOfLevel(int nLevel) const
{
	// FEATURE_PERIOD = {1, 60, 100} (partner_levelup_server.lua:3)
	if (nLevel >= 100) return 3;
	if (nLevel >= 60) return 2;
	return 1;
}

int KPartnerSys::NpcTemplateOf(int nP) const
{
	int nFeature = GetP(nP, PTP_FEATURE);
	if (nFeature < 1 || nFeature > PARTNER_FEATURE_NUM) return 0;
	int nPeriod = PeriodOfLevel(GetP(nP, PTP_LEVEL));
	return g_PartnerTables.Feature[nFeature].nNpcIdxPeriod[nPeriod - 1];
}

int KPartnerSys::GetName(int nP, char* szOut) const
{
	int i;
	for (i = 0; i < 4; i++)
	{
		int v = GetP(nP, PTP_NAME0 + i);
		memcpy(szOut + i * 4, &v, 4);
	}
	szOut[PARTNER_NAME_LEN] = 0;
	return (int)strlen(szOut);
}

void KPartnerSys::SetName(int nP, const char* szName)
{
	char szBuf[PARTNER_NAME_LEN + 4];
	memset(szBuf, 0, sizeof(szBuf));
	strncpy(szBuf, szName, PARTNER_NAME_LEN - 1);
	for (int i = 0; i < 4; i++)
	{
		int v;
		memcpy(&v, szBuf + i * 4, 4);
		SetP(nP, PTP_NAME0 + i, v);
	}
	if (nP == GetCur() && m_nNpcIdx > 0 && Npc[m_nNpcIdx].m_Index > 0)
	{
		memset(Npc[m_nNpcIdx].Name, 0, sizeof(Npc[m_nNpcIdx].Name));
		strncpy(Npc[m_nNpcIdx].Name, szBuf, sizeof(Npc[m_nNpcIdx].Name) - 1);
	}
}

//---------------------------------------------------------------------------
// sinh chi so
//---------------------------------------------------------------------------
void KPartnerSys::RollAttribs(int nP)
{
	int nSeries = GetP(nP, PTP_SERIES);
	if (nSeries < 0 || nSeries >= PARTNER_SERIES_NUM) nSeries = 0;
	DWORD dwSeed = (DWORD)GetP(nP, PTP_RANDSEED);
	KPartnerAttribRangeRow* pAR = &g_PartnerTables.AttribRange[nSeries];
	for (int a = 0; a < PARTNER_APT_NUM; a++)
	{
		int nApt = GetP(nP, PTP_APT0 + a);
		if (nApt < 1) nApt = 1;
		if (nApt > 10) nApt = 10;
		// he so noi suy tu tu chat (aptitude_range), roi ap vao dai init/inc (attrib_range)
		float fCoef = sRandRangeF(&dwSeed, g_PartnerTables.AptRange[nApt].fMin[a],
			g_PartnerTables.AptRange[nApt].fMax[a]);
		float fInit = pAR->fInitMin[a] + fCoef * (pAR->fInitMax[a] - pAR->fInitMin[a]);
		float fCoef2 = sRandRangeF(&dwSeed, g_PartnerTables.AptRange[nApt].fMin[a],
			g_PartnerTables.AptRange[nApt].fMax[a]);
		float fInc = pAR->fIncMin[a] + fCoef2 * (pAR->fIncMax[a] - pAR->fIncMin[a]);
		SetP(nP, PTP_ATTRINIT0 + a, (int)(fInit * 10000.0f));
		SetP(nP, PTP_ATTRINC0 + a, (int)(fInc * 10000.0f));
	}
}

void KPartnerSys::ReGenAttribsInc(int nP)
{
	// roll lai TANG TRUONG (goi o lv 10/30/50/90 - partner_levelup_server.lua:55)
	int nSeries = GetP(nP, PTP_SERIES);
	if (nSeries < 0 || nSeries >= PARTNER_SERIES_NUM) nSeries = 0;
	DWORD dwSeed = (DWORD)time(NULL) ^ (DWORD)(m_nPlayerIdx * 2654435761u) ^ (DWORD)GetP(nP, PTP_RANDSEED);
	KPartnerAttribRangeRow* pAR = &g_PartnerTables.AttribRange[nSeries];
	for (int a = 0; a < PARTNER_APT_NUM; a++)
	{
		int nApt = GetP(nP, PTP_APT0 + a);
		if (nApt < 1) nApt = 1;
		if (nApt > 10) nApt = 10;
		float fCoef = sRandRangeF(&dwSeed, g_PartnerTables.AptRange[nApt].fMin[a],
			g_PartnerTables.AptRange[nApt].fMax[a]);
		float fInc = pAR->fIncMin[a] + fCoef * (pAR->fIncMax[a] - pAR->fIncMin[a]);
		SetP(nP, PTP_ATTRINC0 + a, (int)(fInc * 10000.0f));
	}
	ApplyToNpc(nP);
}

//---------------------------------------------------------------------------
// ky nang - kho pack id*1000+level
//---------------------------------------------------------------------------
static void sSkillSlotRange(int nType, int* pnOff, int* pnMax)
{
	switch (nType)
	{
	case PARTNER_SKTYPE_RESIST:   *pnOff = PTP_SK_RESIST0; *pnMax = PARTNER_SK_RESIST_MAX; break;
	case PARTNER_SKTYPE_GIVEN:    *pnOff = PTP_SK_GIVEN0;  *pnMax = PARTNER_SK_GIVEN_MAX; break;
	case PARTNER_SKTYPE_LEARNT:   *pnOff = PTP_SK_LEARNT0; *pnMax = PARTNER_SK_LEARNT_MAX; break;
	case PARTNER_SKTYPE_ULTIMATE: *pnOff = PTP_SK_ULT;     *pnMax = PARTNER_SK_ULT_MAX; break;
	default:                      *pnOff = 0;              *pnMax = 0; break;
	}
}

int KPartnerSys::GetSkillSlot(int nP, int nType, int nSlot) const
{
	int nOff, nMax;
	sSkillSlotRange(nType, &nOff, &nMax);
	if (nSlot < 0 || nSlot >= nMax) return 0;
	return GetP(nP, nOff + nSlot);
}

int KPartnerSys::AddSkill(int nP, int nType, int nSkillId, int nLevel, int nExp)
{
	if (!IsUsed(nP) || nSkillId <= 0) return 0;
	if (nLevel < 1) nLevel = 1;
	int nOff, nMax;
	sSkillSlotRange(nType, &nOff, &nMax);
	if (nMax == 0) return 0;
	int nFree = -1;
	for (int i = 0; i < nMax; i++)
	{
		int nPack = GetP(nP, nOff + i);
		if (nPack / 1000 == nSkillId && nPack != 0)
		{
			// da co: nang cap
			SetP(nP, nOff + i, nSkillId * 1000 + nLevel);
			if (nType == PARTNER_SKTYPE_GIVEN)
				SetP(nP, PTP_SK_GIVENEXP0 + i, nExp);
			ApplyToNpc(nP);
			return 1;
		}
		if (nPack == 0 && nFree < 0) nFree = i;
	}
	if (nFree < 0) return 0;
	SetP(nP, nOff + nFree, nSkillId * 1000 + nLevel);
	if (nType == PARTNER_SKTYPE_GIVEN)
		SetP(nP, PTP_SK_GIVENEXP0 + nFree, nExp);
	ApplyToNpc(nP);
	return 1;
}

int KPartnerSys::RemoveSkill(int nP, int nType, int nSkillId)
{
	int nOff, nMax;
	sSkillSlotRange(nType, &nOff, &nMax);
	for (int i = 0; i < nMax; i++)
	{
		if (GetP(nP, nOff + i) / 1000 == nSkillId && GetP(nP, nOff + i) != 0)
		{
			SetP(nP, nOff + i, 0);
			if (nType == PARTNER_SKTYPE_GIVEN)
				SetP(nP, PTP_SK_GIVENEXP0 + i, 0);
			ApplyToNpc(nP);
			return 1;
		}
	}
	return 0;
}

void KPartnerSys::RemoveAllSkill(int nP, int nType)
{
	int nOff, nMax;
	sSkillSlotRange(nType, &nOff, &nMax);
	for (int i = 0; i < nMax; i++)
	{
		SetP(nP, nOff + i, 0);
		if (nType == PARTNER_SKTYPE_GIVEN)
			SetP(nP, PTP_SK_GIVENEXP0 + i, 0);
	}
	ApplyToNpc(nP);
}

int KPartnerSys::GetSkillInfo(int nP, int nSkillId, int* pnLevel, int* pnExp) const
{
	*pnLevel = 0;
	*pnExp = 0;
	static const int s_nTypes[4] = { PARTNER_SKTYPE_RESIST, PARTNER_SKTYPE_GIVEN,
		PARTNER_SKTYPE_LEARNT, PARTNER_SKTYPE_ULTIMATE };
	for (int t = 0; t < 4; t++)
	{
		int nOff, nMax;
		sSkillSlotRange(s_nTypes[t], &nOff, &nMax);
		for (int i = 0; i < nMax; i++)
		{
			int nPack = GetP(nP, nOff + i);
			if (nPack / 1000 == nSkillId && nPack != 0)
			{
				*pnLevel = nPack % 1000;
				if (s_nTypes[t] == PARTNER_SKTYPE_GIVEN)
					*pnExp = GetP(nP, PTP_SK_GIVENEXP0 + i);
				return 1;
			}
		}
	}
	return 0;
}

//---------------------------------------------------------------------------
// tao / xoa / chon / trieu hoi
//---------------------------------------------------------------------------
int KPartnerSys::AddFightPartner(int nFeature, int nSeries, int nCharacter,
	const int* pnApt, int nAptMode)
{
	if (m_nPlayerIdx <= 0) return 0;
	if (!g_PartnerTables.bLoaded && !g_PartnerTables.Load()) return 0;
	if (nFeature < 1 || nFeature > PARTNER_FEATURE_NUM) return 0;
	if (nSeries < 0 || nSeries >= PARTNER_SERIES_NUM) return 0;
	if (nCharacter < 1 || nCharacter > PARTNER_CHARACTER_NUM) nCharacter = 1;

	int nP = 0;
	for (int i = 1; i <= PARTNER_MAX_COUNT; i++)
		if (!IsUsed(i)) { nP = i; break; }
	if (nP == 0) return 0;                       // day (tran 3 - ban goc)

	SetG(PTG_VERSION, 1);
	DWORD dwSeed = (DWORD)time(NULL) ^ ((DWORD)m_nPlayerIdx << 16) ^ (DWORD)rand();

	// tu chat: 6 gia tri tuong minh HOAC boc theo mode (aptitude_mode.txt)
	int nApt[PARTNER_APT_NUM];
	if (pnApt)
	{
		for (int a = 0; a < PARTNER_APT_NUM; a++)
		{
			nApt[a] = pnApt[a];
			if (nApt[a] < 1) nApt[a] = 1;
			if (nApt[a] > 10) nApt[a] = 10;
		}
	}
	else
	{
		if (nAptMode < 1 || nAptMode > 2) nAptMode = 1;
		for (int a = 0; a < PARTNER_APT_NUM; a++)
		{
			int nTotal = 0, t;
			for (t = 0; t < 10; t++) nTotal += g_PartnerTables.AptMode[nAptMode].nWeight[a][t];
			int nRoll = (nTotal > 0) ? (int)(sPartnerRand(&dwSeed) % (DWORD)nTotal) : 0;
			nApt[a] = 1;
			for (t = 0; t < 10; t++)
			{
				nRoll -= g_PartnerTables.AptMode[nAptMode].nWeight[a][t];
				if (nRoll < 0) { nApt[a] = t + 1; break; }
			}
		}
	}

	SetP(nP, PTP_USED, 1);
	SetP(nP, PTP_FEATURE, nFeature);
	SetP(nP, PTP_SERIES, nSeries);
	SetP(nP, PTP_CHARACTER, nCharacter);
	for (int a = 0; a < PARTNER_APT_NUM; a++)
		SetP(nP, PTP_APT0 + a, nApt[a]);
	SetP(nP, PTP_GENTIME, (int)time(NULL));
	SetP(nP, PTP_RANDSEED, (int)dwSeed);
	SetP(nP, PTP_LEVEL, 1);
	SetP(nP, PTP_EXP, 0);
	SetP(nP, PTP_EMOTION, g_PartnerTables.Setting.nEmotionInit);
	SetP(nP, PTP_PUNISHUNTIL, 0);
	SetP(nP, PTP_STANDBYSKILL, 0);
	SetP(nP, PTP_EMO_LASTDEC, (int)time(NULL));

	char szName[PARTNER_NAME_LEN + 4];
	memset(szName, 0, sizeof(szName));
	sprintf(szName, "DongHanh%d", nP);
	SetName(nP, szName);

	RollAttribs(nP);

	// ky nang khoi tao theo he (init_skill.ini)
	KPartnerInitSkillRow* pInit = &g_PartnerTables.InitSkill[nSeries];
	for (int k = 0; k < pInit->nCount; k++)
		AddSkill(nP, pInit->nType[k], pInit->nSkillId[k], pInit->nLevel[k], pInit->nExp[k]);

	if (GetCur() == 0) SetG(PTG_CURPARTNER, nP);

	Partner_WriteLog("(%s:%s) AddFightPartner idx=%d Gen(%d,%d,%d,%d,%d,%d,%d,%d,%d) Seed=%u",
		Player[m_nPlayerIdx].m_AccoutName, Player[m_nPlayerIdx].m_PlayerName, nP,
		nFeature, nSeries, nCharacter, nApt[0], nApt[1], nApt[2], nApt[3], nApt[4], nApt[5],
		dwSeed);
	return nP;
}

int KPartnerSys::RemovePartner(int nP)
{
	if (!IsUsed(nP)) return 0;
	Partner_WriteLog("(%s:%s) Request Remove Partner(Level: %d; Gen Info: %d, %d, %d, %d, %d, %d, %d, %d, %d), GenTime: %u, RandSeed: %u",
		Player[m_nPlayerIdx].m_AccoutName, Player[m_nPlayerIdx].m_PlayerName,
		GetP(nP, PTP_LEVEL),
		GetP(nP, PTP_FEATURE), GetP(nP, PTP_SERIES), GetP(nP, PTP_CHARACTER),
		GetP(nP, PTP_APT0), GetP(nP, PTP_APT0 + 1), GetP(nP, PTP_APT0 + 2),
		GetP(nP, PTP_APT0 + 3), GetP(nP, PTP_APT0 + 4), GetP(nP, PTP_APT0 + 5),
		(DWORD)GetP(nP, PTP_GENTIME), (DWORD)GetP(nP, PTP_RANDSEED));

	if (nP == GetCur() && m_nNpcIdx > 0)
		Recall(TRUE);
	for (int off = 0; off < PARTNER_TASK_BLOCK; off++)
		SetP(nP, off, 0);
	if (GetG(PTG_CURPARTNER) == nP)
	{
		SetG(PTG_CURPARTNER, 0);
		for (int i = 1; i <= PARTNER_MAX_COUNT; i++)
			if (IsUsed(i)) { SetG(PTG_CURPARTNER, i); break; }
		SetG(PTG_CALLOUT, 0);
	}
	return 1;
}

int KPartnerSys::SetCurPartner(int nP)
{
	if (!IsUsed(nP)) return 0;
	if (GetCur() == nP) return 1;
	if (IsCallOut())
		Recall(TRUE);
	SetG(PTG_CURPARTNER, nP);
	return 1;
}

void KPartnerSys::Recall(BOOL bCorpseToo)
{
	if (m_nNpcIdx > 0 && Npc[m_nNpcIdx].m_Index > 0 && NpcSet.IsNpcExist(m_nNpcIdx, m_dwNpcID))
	{
		int n = m_nNpcIdx;
		if (Npc[n].m_RegionIndex >= 0)
		{
			int sw = Npc[n].m_SubWorldIndex;
			int rg = Npc[n].m_RegionIndex;
			SubWorld[sw].m_Region[rg].RemoveNpc(n);
			SubWorld[sw].m_Region[rg].DecRef(Npc[n].m_MapX, Npc[n].m_MapY, obj_npc);
		}
		NpcSet.Remove(n);
	}
	m_nNpcIdx = 0;
	m_dwNpcID = 0;
	m_nNpcOfPartner = 0;
	SetG(PTG_CALLOUT, 0);
	(void)bCorpseToo;
}

int KPartnerSys::CallOut(int nFlag)
{
	if (m_nPlayerIdx <= 0) return 0;
	if (!g_PartnerTables.bLoaded && !g_PartnerTables.Load()) return 0;
	int nP = GetCur();
	if (nFlag == 0)
	{
		Recall(TRUE);
		return 1;
	}
	if (nP == 0) return 0;
	if (GetG(PTG_CALLOUT_SWITCH) != 0) return 0;         // map cam goi (PARTNER_OFF)

	time_t tNow = time(NULL);
	if ((int)tNow < GetP(nP, PTP_PUNISHUNTIL)) return 0; // dang hon me
	int nLast = GetG(PTG_LASTCALLTIME);
	if (nLast > 0 && (int)tNow - nLast < g_PartnerTables.Setting.nCalloutInterval &&
		!IsCallOut())
		return 0;                                        // cooldown 30s

	if (IsCallOut())
		Recall(TRUE);                                    // goi lai = thay the

	KPlayer* pPlayer = &Player[m_nPlayerIdx];
	if (pPlayer->m_nIndex <= 0) return 0;
	KNpc* pOwner = &Npc[pPlayer->m_nIndex];

	int nTpl = NpcTemplateOf(nP);
	if (nTpl <= 0) return 0;
	int nLevel = GetP(nP, PTP_LEVEL);
	if (nLevel < 1) nLevel = 1;
	if (nLevel > 100) nLevel = 100;
	int nSeries = GetP(nP, PTP_SERIES);

	int nOwnerX = 0, nOwnerY = 0;
	pOwner->GetMpsPos(&nOwnerX, &nOwnerY);
	int nNpcIdx = NpcSet.AddNpcSet2(MAKELONG(nLevel, nTpl), nSeries,
		pOwner->m_SubWorldIndex, nOwnerX + 48, nOwnerY + 48);
	if (nNpcIdx <= 0)
	{
		nNpcIdx = NpcSet.AddNpcSet2(MAKELONG(nLevel, nTpl), nSeries,
			pOwner->m_SubWorldIndex, nOwnerX, nOwnerY);
	}
	if (nNpcIdx <= 0) return 0;

	KNpc* pNpc = &Npc[nNpcIdx];
	pNpc->m_Kind = kind_partner;
	pNpc->m_nPartnerOwner = m_nPlayerIdx;
	pNpc->m_nPartnerNo = nP;
	pNpc->SetCamp(pOwner->m_CurrentCamp);
	pNpc->SetCurrentCamp(pOwner->m_CurrentCamp);
	memset(pNpc->Owner, 0, sizeof(pNpc->Owner));
	strncpy(pNpc->Owner, pPlayer->m_PlayerName, sizeof(pNpc->Owner) - 1);
	pNpc->m_bNpcFollowFindPath = FALSE;          // KHONG dung nhanh van tieu; AI rieng
	char szName[PARTNER_NAME_LEN + 4];
	GetName(nP, szName);
	if (szName[0])
	{
		memset(pNpc->Name, 0, sizeof(pNpc->Name));
		strncpy(pNpc->Name, szName, sizeof(pNpc->Name) - 1);
	}
	// AI theo tinh cach (character.txt)
	int nChar = GetP(nP, PTP_CHARACTER);
	if (nChar < 1 || nChar > PARTNER_CHARACTER_NUM) nChar = 1;
	pNpc->m_AIMAXTime = (BYTE)g_PartnerTables.Character[nChar].nAIMaxTime;

	m_nNpcIdx = nNpcIdx;
	m_dwNpcID = pNpc->m_dwID;
	m_nNpcOfPartner = nP;
	SetG(PTG_CALLOUT, 1);
	SetG(PTG_LASTCALLTIME, (int)tNow);

	ApplyToNpc(nP);
	pNpc->m_CurrentLife = pNpc->m_LifeMax;	// trieu hoi = day mau (khong dung mau template)
	FireEvent("PartnerCallout");
	return 1;
}

//---------------------------------------------------------------------------
// ap chi so + ky nang vao NPC dang goi ra
//---------------------------------------------------------------------------
void KPartnerSys::ApplyToNpc(int nP)
{
	if (m_nNpcIdx <= 0 || m_nNpcOfPartner != nP) return;
	if (!NpcSet.IsNpcExist(m_nNpcIdx, m_dwNpcID)) return;
	KNpc* pNpc = &Npc[m_nNpcIdx];
	int nLevel = GetP(nP, PTP_LEVEL);
	if (nLevel < 1) nLevel = 1;

	// gia tri hien tai = init + inc*(level-1)  (fixed point x10000)
	double dVal[PARTNER_APT_NUM];
	for (int a = 0; a < PARTNER_APT_NUM; a++)
	{
		double dInit = (double)GetP(nP, PTP_ATTRINIT0 + a) / 10000.0;
		double dInc = (double)GetP(nP, PTP_ATTRINC0 + a) / 10000.0;
		dVal[a] = dInit + dInc * (double)(nLevel - 1);
		if (dVal[a] < 0) dVal[a] = 0;
	}

	int nOldMax = pNpc->m_LifeMax;
	pNpc->m_LifeMax = (int)dVal[pattr_life];
	if (pNpc->m_LifeMax < 1) pNpc->m_LifeMax = 1;
	if (nOldMax <= 0 || pNpc->m_CurrentLife > pNpc->m_LifeMax || pNpc->m_CurrentLife <= 0)
		pNpc->m_CurrentLife = pNpc->m_LifeMax;

	int nStrength = (int)dVal[pattr_strength];
	pNpc->m_PhysicsDamage.nValue[0] = nStrength;
	pNpc->m_PhysicsDamage.nValue[1] = nStrength;
	pNpc->m_AttackRating = (int)dVal[pattr_hitrate];
	pNpc->m_Defend = (int)dVal[pattr_defence];
	int nSpeed = (int)dVal[pattr_speed];
	if (nSpeed > 0)
	{
		pNpc->m_RunSpeed = nSpeed;
		pNpc->m_WalkSpeed = nSpeed;
	}
	// luck (pattr_luck): chua co truong NPC tuong ung - giu trong kho, dung sau

	// khang theo he: resist.txt (init + inc*(level-1)); thu tu physics cold light fire poison
	int nSeries = GetP(nP, PTP_SERIES);
	if (nSeries >= 0 && nSeries < PARTNER_SERIES_NUM)
	{
		KPartnerResistRow* pR = &g_PartnerTables.Resist[nSeries];
		int nRes[PARTNER_RESIST_NUM];
		for (int k = 0; k < PARTNER_RESIST_NUM; k++)
			nRes[k] = (int)(pR->fInit[k] + pR->fInc[k] * (float)(nLevel - 1));
		pNpc->m_PhysicsResist = pNpc->m_CurrentPhysicsResist = nRes[0];
		pNpc->m_ColdResist = pNpc->m_CurrentColdResist = nRes[1];
		pNpc->m_LightResist = pNpc->m_CurrentLightResist = nRes[2];
		pNpc->m_FireResist = pNpc->m_CurrentFireResist = nRes[3];
		pNpc->m_PoisonResist = pNpc->m_CurrentPoisonResist = nRes[4];
	}

	// vo cong chu dong (given + ultimate) vao SkillList o 1..4 de AI dung
	int nSlot = 1;
	for (int i = 0; i < PARTNER_SK_GIVEN_MAX && nSlot <= 4; i++)
	{
		int nPack = GetP(nP, PTP_SK_GIVEN0 + i);
		if (nPack > 0)
			pNpc->m_SkillList.SetNpcSkill(nSlot++, nPack / 1000, nPack % 1000);
	}
	{
		int nPack = GetP(nP, PTP_SK_ULT);
		if (nPack > 0 && nSlot <= 4)
			pNpc->m_SkillList.SetNpcSkill(nSlot++, nPack / 1000, nPack % 1000);
	}

	// ky nang bi dong (khang + tu hoc) ap thanh trang thai skill vinh vien
	// (khuon LuaAddNpcSkillState ScriptFuns.cpp:13066: CastStateSkill)
	for (int t = 0; t < 2; t++)
	{
		int nOff = (t == 0) ? PTP_SK_RESIST0 : PTP_SK_LEARNT0;
		int nMax = (t == 0) ? PARTNER_SK_RESIST_MAX : PARTNER_SK_LEARNT_MAX;
		for (int i = 0; i < nMax; i++)
		{
			int nPack = GetP(nP, nOff + i);
			if (nPack <= 0) continue;
			KSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(nPack / 1000, nPack % 1000);
			if (pSkill)
				pSkill->CastStateSkill(m_nNpcIdx, 0, 0, -1, TRUE);
		}
	}
}

//---------------------------------------------------------------------------
// exp / len cap
//---------------------------------------------------------------------------
int KPartnerSys::AddExp(int nP, int nExp, int nShowMsg)
{
	if (!IsUsed(nP) || nExp == 0) return 0;
	int nLevel = GetP(nP, PTP_LEVEL);
	int nCur = GetP(nP, PTP_EXP);
	nCur += nExp;
	if (nCur < 0) nCur = 0;
	SetP(nP, PTP_EXP, nCur);
	if (nShowMsg)
	{
		char szName[PARTNER_NAME_LEN + 4];
		GetName(nP, szName);
		char szMsg[128];
		if (nExp > 0)
			sprintf(szMsg, "%s nhan duoc %d diem kinh nghiem.", szName, nExp);
		else
			sprintf(szMsg, "%s ton that %d diem kinh nghiem.", szName, -nExp);
		KPlayerChat::SendSystemInfo(1, m_nPlayerIdx, (char*)MESSAGE_SYSTEM_ANNOUCE_HEAD,
			szMsg, (int)strlen(szMsg));
	}
	// bang level_exp.txt: NGUONG TONG de len cap ke tiep [GIA DINH - xem PHANTICH muc 4]
	while (nLevel < PARTNER_MAX_LEVEL &&
		g_PartnerTables.nLevelExp[nLevel] > 0 && nCur >= g_PartnerTables.nLevelExp[nLevel])
	{
		nLevel++;
		LevelUpTo(nP, nLevel);
	}
	return 1;
}

void KPartnerSys::LevelUpTo(int nP, int nNewLevel)
{
	SetP(nP, PTP_LEVEL, nNewLevel);

	char szName[PARTNER_NAME_LEN + 4];
	GetName(nP, szName);
	char szMsg[128];
	sprintf(szMsg, "%s da thang den %d cap.", szName, nNewLevel);
	KPlayerChat::SendSystemInfo(1, m_nPlayerIdx, (char*)MESSAGE_SYSTEM_ANNOUCE_HEAD,
		szMsg, (int)strlen(szMsg));

	// goi script (partner_levelup_server.lua lo AddSkill vo cong 30/60/80 + ReGen 10/30/50/90)
	// Goi truc tiep CallFunction de KHONG pha m_ActionScriptID (bay ExecuteScript2)
	KLuaScript* pScript = (KLuaScript*)g_GetScript("\\script\\partner\\partner_levelup_server.lua");
	if (pScript)
	{
		int nTop = 0;
		pScript->SafeCallBegin(&nTop);
		Lua_PushNumber(pScript->m_LuaState, m_nPlayerIdx);
		pScript->SetGlobalName((LPSTR)SCRIPT_PLAYERINDEX);
		pScript->CallFunction((LPSTR)"main", 0, (LPSTR)"dd", nP, nNewLevel);
		pScript->SafeCallEnd(nTop);
	}
	// dong bo hinh dang neu vuot moc thoi ky (script cung goi ChangeFeatureOfPeriod,
	// nhung neu script thieu thi van dung)
	if (nP == m_nNpcOfPartner && m_nNpcIdx > 0 && NpcSet.IsNpcExist(m_nNpcIdx, m_dwNpcID))
	{
		int nTpl = NpcTemplateOf(nP);
		if (nTpl > 0)
		{
			Npc[m_nNpcIdx].GetNpcCopyFromTemplate(nTpl, nNewLevel, GetP(nP, PTP_SERIES));
			Npc[m_nNpcIdx].m_Kind = kind_partner;
		}
		ApplyToNpc(nP);
	}
	FireEvent("PartnerLevelUp");
}

//---------------------------------------------------------------------------
// than mat / su kien thoai
//---------------------------------------------------------------------------
void KPartnerSys::AddEmotion(int nP, int nDelta)
{
	if (!IsUsed(nP)) return;
	int nEmo = GetP(nP, PTP_EMOTION) + nDelta;
	if (nEmo < g_PartnerTables.Setting.nEmotionMin) nEmo = g_PartnerTables.Setting.nEmotionMin;
	if (nEmo > g_PartnerTables.Setting.nEmotionMax) nEmo = g_PartnerTables.Setting.nEmotionMax;
	SetP(nP, PTP_EMOTION, nEmo);
}

void KPartnerSys::FireEvent(const char* szEvent)
{
	if (!IsCallOut()) return;
	int nP = m_nNpcOfPartner;
	KPartnerEventRow* pRow = NULL;
	for (int e = 0; e < g_PartnerTables.nEventCount; e++)
		if (strcmp(g_PartnerTables.Event[e].szName, szEvent) == 0)
		{
			pRow = &g_PartnerTables.Event[e];
			break;
		}
	if (!pRow || pRow->nOdds <= 0) return;
	if (rand() % 100 >= pRow->nOdds) return;

	int nFeature = GetP(nP, PTP_FEATURE);
	int nSex = (nFeature >= 1 && nFeature <= PARTNER_FEATURE_NUM)
		? g_PartnerTables.Feature[nFeature].nSex : 0;
	int nCount = nSex ? pRow->nCountF : pRow->nCountM;
	if (nCount <= 0) return;
	const char* szLine = nSex ? pRow->szDialogF[rand() % nCount]
	                          : pRow->szDialogM[rand() % nCount];
	if (!szLine[0]) return;

	// thay %HostName theo gioi tinh CHU (goc: HostName_Man/HostName_Woman)
	int nOwnerSex = 0;
	if (Player[m_nPlayerIdx].m_nIndex > 0)
		nOwnerSex = Npc[Player[m_nPlayerIdx].m_nIndex].m_nSex;
	const char* szHost = nOwnerSex ? g_PartnerTables.szHostNameWoman
	                               : g_PartnerTables.szHostNameMan;
	char szMsg[192];
	const char* pFind = strstr(szLine, "%HostName");
	if (pFind)
	{
		int nPre = (int)(pFind - szLine);
		if (nPre > 100) nPre = 100;
		memcpy(szMsg, szLine, nPre);
		szMsg[nPre] = 0;
		strncat(szMsg, szHost, 40);
		strncat(szMsg, pFind + 9, 100);
	}
	else
	{
		strncpy(szMsg, szLine, sizeof(szMsg) - 1);
		szMsg[sizeof(szMsg) - 1] = 0;
	}
	int nLen = (int)strlen(szMsg);
	if (nLen > 120) { szMsg[120] = 0; nLen = 120; }     // SetChatInfo tran o 128
	KPlayerChat::NpcChat(m_nNpcIdx, szMsg, nLen, false);
}

//---------------------------------------------------------------------------
// vong doi moi tick / logout / doi map
//---------------------------------------------------------------------------
void KPartnerSys::OnPlayerLogout()
{
	Recall(TRUE);
}

void KPartnerSys::OnPlayerChangeWorld()
{
	// duoc Breathe xu ly (khong can goi truc tiep)
}

void KPartnerSys::Breathe()
{
	if (m_nPlayerIdx <= 0) return;
	KPlayer* pPlayer = &Player[m_nPlayerIdx];
	if (pPlayer->m_nIndex <= 0) return;

	// npc chet/ao => don co runtime
	if (m_nNpcIdx > 0 && !NpcSet.IsNpcExist(m_nNpcIdx, m_dwNpcID))
	{
		m_nNpcIdx = 0;
		m_dwNpcID = 0;
		m_nNpcOfPartner = 0;
		SetG(PTG_CALLOUT, 0);
	}

	time_t tNow = time(NULL);

	// giam than mat: emotion > DEG_BASE thi cu DEG_STEP giay giam 1 (moi con)
	DWORD dwTick = GetTickCount();
	if (dwTick - m_dwLastEmoTick >= 60000)               // kiem moi 60s la du
	{
		m_dwLastEmoTick = dwTick;
		for (int i = 1; i <= PARTNER_MAX_COUNT; i++)
		{
			if (!IsUsed(i)) continue;
			int nEmo = GetP(i, PTP_EMOTION);
			if (nEmo > g_PartnerTables.Setting.nEmotionDegBase)
			{
				int nLast = GetP(i, PTP_EMO_LASTDEC);
				if (nLast <= 0) { SetP(i, PTP_EMO_LASTDEC, (int)tNow); continue; }
				if ((int)tNow - nLast >= g_PartnerTables.Setting.nEmotionDegStep)
				{
					SetP(i, PTP_EMOTION, nEmo - 1);
					SetP(i, PTP_EMO_LASTDEC, (int)tNow);
				}
			}
		}
	}

	if (m_nNpcIdx <= 0) return;
	KNpc* pNpc = &Npc[m_nNpcIdx];
	KNpc* pOwner = &Npc[pPlayer->m_nIndex];

	// theo chu qua map
	if (pNpc->m_SubWorldIndex != pOwner->m_SubWorldIndex)
	{
		if (pNpc->m_Doing == do_death)
		{
			Recall(TRUE);
			return;
		}
		DWORD dwMapId = (DWORD)SubWorld[pOwner->m_SubWorldIndex].m_SubWorldID;
		int nOwnerX = 0, nOwnerY = 0;
		pOwner->GetMpsPos(&nOwnerX, &nOwnerY);
		if (pNpc->ChangeWorld(dwMapId, nOwnerX + 32, nOwnerY + 32) <= 0)
			Recall(TRUE);
		return;
	}

	// hoi mau LIFE_RESTORE/10000 moi 5 giay (partner_setting.ini [REVIVE])
	if (pNpc->m_Doing != do_death && dwTick - m_dwLastRegenTick >= 5000)
	{
		m_dwLastRegenTick = dwTick;
		if (pNpc->m_CurrentLife > 0 && pNpc->m_CurrentLife < pNpc->m_LifeMax)
		{
			int nAdd = pNpc->m_LifeMax * g_PartnerTables.Setting.nLifeRestore / 10000;
			if (nAdd < 1) nAdd = 1;
			pNpc->m_CurrentLife += nAdd;
			if (pNpc->m_CurrentLife > pNpc->m_LifeMax)
				pNpc->m_CurrentLife = pNpc->m_LifeMax;
		}
		// mau it => thoai (PartnerBloodLess, Param1 = nguong %)
		if (pNpc->m_LifeMax > 0)
		{
			int nPct = pNpc->m_CurrentLife * 100 / pNpc->m_LifeMax;
			for (int e = 0; e < g_PartnerTables.nEventCount; e++)
			{
				if (strcmp(g_PartnerTables.Event[e].szName, "PartnerBloodLess") == 0)
				{
					if (g_PartnerTables.Event[e].nParam1 > 0 &&
						nPct < g_PartnerTables.Event[e].nParam1)
						FireEvent("PartnerBloodLess");
					break;
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
// AI cua partner (goi tu KNpcAI::Activate khi m_Kind == kind_partner)
//---------------------------------------------------------------------------
static int sPartnerPickTarget(int nNpcIdx, int nOwnerNpcIdx, int nMode, int nVision)
{
	KNpc* pNpc = &Npc[nNpcIdx];
	int nSubWorld = pNpc->m_SubWorldIndex;
	int nBest = 0;
	int nBestKey = 0x7fffffff;
	int nCount = 0;
	int aryRand[8] = { 0 };
	int nMyX = 0, nMyY = 0;
	pNpc->GetMpsPos(&nMyX, &nMyY);

	// ve chu (mode 22): uu tien ke vua danh chu
	if (nMode == 22)
	{
		int nAtk = Npc[nOwnerNpcIdx].m_nLastDamageIdx;
		if (nAtk > 0 && nAtk < MAX_NPC && Npc[nAtk].m_Index > 0 &&
			Npc[nAtk].m_Doing != do_death &&
			Npc[nAtk].m_SubWorldIndex == nSubWorld &&
			NpcSet.GetRelation(nNpcIdx, nAtk) == relation_enemy)
		{
			int nTX = 0, nTY = 0;
			Npc[nAtk].GetMpsPos(&nTX, &nTY);
			if (abs(nTX - nMyX) + abs(nTY - nMyY) <= nVision * 2)
				return nAtk;
		}
	}

	int nRegion = pNpc->m_RegionIndex;
	if (nRegion < 0) return 0;
	for (int r = 0; r < 9; r++)
	{
		int nCheckRegion = (r == 0) ? nRegion
			: SubWorld[nSubWorld].m_Region[nRegion].m_nConnectRegion[r - 1];
		if (nCheckRegion < 0) continue;
		KIndexNode* pNode = (KIndexNode*)SubWorld[nSubWorld].m_Region[nCheckRegion].m_NpcList.GetHead();
		int nGuard = 0;
		while (pNode && nGuard++ < 512)
		{
			int nIdx = pNode->m_nIndex;
			pNode = (KIndexNode*)pNode->GetNext();
			if (nIdx <= 0 || nIdx == nNpcIdx) continue;
			KNpc* pT = &Npc[nIdx];
			if (pT->m_Index > 0 && pT->m_Doing != do_death &&
				pT->m_Doing != do_revive && pT->m_CurrentLife > 0 &&
				NpcSet.GetRelation(nNpcIdx, nIdx) == relation_enemy)
			{
				int nTX = 0, nTY = 0;
				pT->GetMpsPos(&nTX, &nTY);
				int nDis = abs(nTX - nMyX) + abs(nTY - nMyY);
				if (nDis <= nVision)
				{
					int nKey;
					switch (nMode)
					{
					case 23: nKey = pT->m_CurrentLife; break;    // luu manh: it mau nhat
					case 21:
					case 22:
					default: nKey = nDis; break;                 // gan nhat
					}
					if (nMode == 24)
					{
						if (nCount < 8) aryRand[nCount] = nIdx;  // nhu nhuoc: ngau nhien
						nCount++;
					}
					else if (nKey < nBestKey)
					{
						nBestKey = nKey;
						nBest = nIdx;
					}
				}
			}
		}
	}
	if (nMode == 24 && nCount > 0)
		return aryRand[rand() % ((nCount < 8) ? nCount : 8)];
	return nBest;
}

//---------------------------------------------------------------------------
// [BDH-G4] mo doi thoai chinh (partner_talk.lua main) trong ngu canh player.
// Dung CallFunction truc tiep de KHONG pha m_ActionScriptID (bay ExecuteScript2).
//---------------------------------------------------------------------------
void Partner_RunTalkScript(int nPlayerIdx)
{
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER) return;
	KLuaScript* pScript = (KLuaScript*)g_GetScript("\\script\\partner\\partner_talk.lua");
	if (!pScript || !pScript->m_LuaState) return;
	Lua_PushNumber(pScript->m_LuaState, nPlayerIdx);
	pScript->SetGlobalName((LPSTR)SCRIPT_PLAYERINDEX);
	pScript->CallFunction((LPSTR)"main", 0, (LPSTR)"");
}

void Partner_ProcessAI(int nNpcIdx)
{
	// [PETSYS 28/08] pet PC (petsys) dung chung kind_partner, nNo=100
	if (Npc[nNpcIdx].m_nPartnerNo == PET_PARTNER_NO)
	{
		Pet_ProcessAI(nNpcIdx);
		return;
	}
	KNpc* pNpc = &Npc[nNpcIdx];
	int nOwner = pNpc->m_nPartnerOwner;
	if (nOwner <= 0 || nOwner >= MAX_PLAYER || Player[nOwner].m_nIndex <= 0)
	{
		// chu bien mat -> tu go
		if (pNpc->m_RegionIndex >= 0)
		{
			int sw = pNpc->m_SubWorldIndex, rg = pNpc->m_RegionIndex;
			SubWorld[sw].m_Region[rg].RemoveNpc(nNpcIdx);
			SubWorld[sw].m_Region[rg].DecRef(pNpc->m_MapX, pNpc->m_MapY, obj_npc);
		}
		NpcSet.Remove(nNpcIdx);
		return;
	}
	KPartnerSys* pSys = &Player[nOwner].m_cPartner;
	if (pSys->m_nNpcIdx != nNpcIdx)
	{
		// khe cu bi tai dung / mo coi -> go
		if (pNpc->m_RegionIndex >= 0)
		{
			int sw = pNpc->m_SubWorldIndex, rg = pNpc->m_RegionIndex;
			SubWorld[sw].m_Region[rg].RemoveNpc(nNpcIdx);
			SubWorld[sw].m_Region[rg].DecRef(pNpc->m_MapX, pNpc->m_MapY, obj_npc);
		}
		NpcSet.Remove(nNpcIdx);
		return;
	}
	if (pNpc->m_Doing == do_death) return;

	KNpc* pOwnerNpc = &Npc[Player[nOwner].m_nIndex];
	if (pNpc->m_SubWorldIndex != pOwnerNpc->m_SubWorldIndex)
		return;                                  // Breathe xu ly chuyen map

	int nP = pSys->m_nNpcOfPartner;
	int nChar = pSys->GetP(nP, PTP_CHARACTER);
	if (nChar < 1 || nChar > PARTNER_CHARACTER_NUM) nChar = 1;
	KPartnerCharacterRow* pChar = &g_PartnerTables.Character[nChar];

	int nPX = 0, nPY = 0, nOX = 0, nOY = 0;
	pNpc->GetMpsPos(&nPX, &nPY);
	pOwnerNpc->GetMpsPos(&nOX, &nOY);
	int nDisOwner = abs(nPX - nOX) + abs(nPY - nOY);

	// bi bo qua xa -> nhay ve canh chu (goc: PartnerBeDrawBack)
	if (nDisOwner > pChar->nForceSync * 2)
	{
		pNpc->SetPos(nOX + 48, nOY + 48);
		pSys->FireEvent("PartnerBeDrawBack");
		return;
	}

	// dong bo trang thai chien dau + phe voi chu (khuon ProcessAIFollow KNpcAI.cpp:168-172)
	if (pNpc->m_CurrentCamp != pOwnerNpc->m_CurrentCamp)
		pNpc->SetCurrentCamp(pOwnerNpc->m_CurrentCamp);
	if (pNpc->m_FightMode != pOwnerNpc->m_FightMode)
		pNpc->m_FightMode = pOwnerNpc->m_FightMode;

	// chi danh nhau khi chu dang bat che do chien dau
	int nTarget = 0;
	if (pOwnerNpc->m_FightMode && pSys->GetG(PTG_FIGHTMODE) != 1)	// [BDH-G4] 1 = chi theo
	{
		if (pChar->nAIParam[0] <= 0 || rand() % 100 < pChar->nAIParam[0])
			nTarget = sPartnerPickTarget(nNpcIdx, Player[nOwner].m_nIndex,
				pChar->nAIMode, pChar->nVisionRadius);
	}
	if (nTarget > 0)
	{
		// chon 1 trong cac o skill 1..4 co skill
		int nSkillId = 0;
		int nTry = 4;
		while (nTry-- > 0)
		{
			int nSlot = 1 + rand() % 4;
			if (pNpc->m_SkillList.m_Skills[nSlot].SkillId > 0)
			{
				nSkillId = pNpc->m_SkillList.m_Skills[nSlot].SkillId;
				break;
			}
		}
		if (nSkillId == 0)
		{
			for (int s = 1; s <= 4; s++)
				if (pNpc->m_SkillList.m_Skills[s].SkillId > 0)
				{
					nSkillId = pNpc->m_SkillList.m_Skills[s].SkillId;
					break;
				}
		}
		if (nSkillId > 0)
			pNpc->SendCommand(do_skill, nSkillId, -1, nTarget);
		return;
	}

	// khong co dich: bam theo chu (lech ngau nhien +-50 nhu ProcessAIFollow)
	if (nDisOwner > 128)
	{
		int nXGo = nOX + 50 - rand() % 100;
		int nYGo = nOY + 50 - rand() % 100;
		pNpc->SendCommand(do_walk, nXGo, nYGo);
	}
}

//---------------------------------------------------------------------------
// hon me (goi tu KNpc::OnDeath nhanh kind_partner)
//---------------------------------------------------------------------------
void Partner_OnNpcDeath(int nNpcIdx)
{
	KNpc* pNpc = &Npc[nNpcIdx];
	int nOwner = pNpc->m_nPartnerOwner;
	if (nOwner <= 0 || nOwner >= MAX_PLAYER || Player[nOwner].m_nIndex <= 0)
	{
		NpcSet.Remove(nNpcIdx);
		return;
	}
	KPartnerSys* pSys = &Player[nOwner].m_cPartner;
	int nP = pSys->m_nNpcOfPartner;
	if (nP >= 1 && nP <= PARTNER_MAX_COUNT)
	{
		// hon me PUNISH_TIME giay: khong duoc goi ra (partner_setting.ini [DEATH_PUNISH])
		pSys->SetP(nP, PTP_PUNISHUNTIL, (int)time(NULL) + g_PartnerTables.Setting.nPunishTime);
	}
	// script thong bao (partner_action.lua OnDeath) - khong pha ActionScriptID
	KLuaScript* pScript = (KLuaScript*)g_GetScript("\\script\\partner\\partner_action.lua");
	if (pScript)
	{
		int nTop = 0;
		pScript->SafeCallBegin(&nTop);
		Lua_PushNumber(pScript->m_LuaState, nOwner);
		pScript->SetGlobalName((LPSTR)SCRIPT_PLAYERINDEX);
		pScript->CallFunction((LPSTR)"OnDeath", 0, (LPSTR)"d", pNpc->m_nLastDamageIdx);
		pScript->SafeCallEnd(nTop);
	}
	Partner_WriteLog("(%s:%s) Partner %d hon me (level %d)",
		Player[nOwner].m_AccoutName, Player[nOwner].m_PlayerName, nP,
		(nP >= 1) ? pSys->GetP(nP, PTP_LEVEL) : 0);
	// go npc (xac da phat hoat anh chet; client giu xac theo co che kind_partner)
	pSys->Recall(TRUE);
}

//---------------------------------------------------------------------------
// nhip toan cuc (goi tu CoreServerShell::Breathe canh PB_Breathe)
//---------------------------------------------------------------------------
void KPartner_Breathe()
{
	Pet_Breathe();	// [PETSYS 28/08]
	static DWORD s_dwLast = 0;
	DWORD dwNow = GetTickCount();
	if (dwNow - s_dwLast < 500) return;          // 2 lan/giay la du cho vong doi
	s_dwLast = dwNow;
	if (!g_PartnerTables.bLoaded)
	{
		static BOOL s_bTried = FALSE;
		if (!s_bTried)
		{
			s_bTried = TRUE;
			g_PartnerTables.Load();
		}
		if (!g_PartnerTables.bLoaded) return;
	}
	for (int i = 1; i < MAX_PLAYER; i++)
	{
		if (Player[i].m_nIndex <= 0) continue;
		Player[i].m_cPartner.Breathe();
	}
}

//---------------------------------------------------------------------------
// gate o tui dong hanh theo cap (partner_bag.ini LEVEL_GRID) - KItemList goi
//---------------------------------------------------------------------------
int Partner_BagCellActive(int nPlayerIdx, int nX, int nY)
{
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER) return 0;
	if (!g_PartnerTables.bLoaded && !g_PartnerTables.Load()) return 0;
	int nLevel = Player[nPlayerIdx].m_cPartner.GetG(PTG_BAGLEVEL);
	if (nLevel < 0) nLevel = 0;
	if (nLevel > 10) nLevel = 10;
	int nCols = g_PartnerTables.nBagGrid[nLevel][0];
	int nRows = g_PartnerTables.nBagGrid[nLevel][1];
	return (nX >= 0 && nY >= 0 && nX < nCols && nY < nRows) ? 1 : 0;
}

//===========================================================================
// HAM LUA PARTNER_* (+ 2 ham tui)
//===========================================================================
static KPartnerSys* sGetSys(Lua_State* L, int* pnPlayerIdx)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return NULL;
	if (Player[nPlayerIndex].m_nIndex <= 0) return NULL;
	if (!g_PartnerTables.bLoaded && !g_PartnerTables.Load()) return NULL;
	*pnPlayerIdx = nPlayerIndex;
	return &Player[nPlayerIndex].m_cPartner;
}

static int sArgInt(Lua_State* L, int n, int nDef)
{
	if (Lua_GetTopIndex(L) < n || !Lua_IsNumber(L, n)) return nDef;
	return (int)Lua_ValueToNumber(L, n);
}

// --- truy van ---
int LuaPARTNER_GetCurPartner2(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	if (!pSys) { Lua_PushNumber(L, 0); Lua_PushNumber(L, 0); return 2; }
	Lua_PushNumber(L, pSys->GetCur());
	Lua_PushNumber(L, pSys->IsCallOut() ? 1 : 0);
	return 2;
}

int LuaPARTNER_Count(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	Lua_PushNumber(L, pSys ? pSys->Count() : -1);
	return 1;
}

int LuaPARTNER_GetName(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	char szName[PARTNER_NAME_LEN + 4] = "";
	if (pSys && pSys->IsUsed(nP)) pSys->GetName(nP, szName);
	Lua_PushString(L, szName);
	return 1;
}

int LuaPARTNER_SetName(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	if (pSys && pSys->IsUsed(nP) && Lua_GetTopIndex(L) >= 2 && Lua_IsString(L, 2))
		pSys->SetName(nP, Lua_ValueToString(L, 2));
	return 0;
}

int LuaPARTNER_GetLevel(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	Lua_PushNumber(L, (pSys && pSys->IsUsed(nP)) ? pSys->GetP(nP, PTP_LEVEL) : 0);
	return 1;
}

int LuaPARTNER_GetExp(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	Lua_PushNumber(L, (pSys && pSys->IsUsed(nP)) ? pSys->GetP(nP, PTP_EXP) : 0);
	return 1;
}

int LuaPARTNER_GetSeries(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	Lua_PushNumber(L, (pSys && pSys->IsUsed(nP)) ? pSys->GetP(nP, PTP_SERIES) : 0);
	return 1;
}

int LuaPARTNER_GetCharacter(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	Lua_PushNumber(L, (pSys && pSys->IsUsed(nP)) ? pSys->GetP(nP, PTP_CHARACTER) : 0);
	return 1;
}

int LuaPARTNER_GetEssentialFeatureID(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	Lua_PushNumber(L, (pSys && pSys->IsUsed(nP)) ? pSys->GetP(nP, PTP_FEATURE) : 0);
	return 1;
}

int LuaPARTNER_GetSettingIdx2(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	if (pSys && nP == 0) nP = pSys->GetCur();
	Lua_PushNumber(L, (pSys && pSys->IsUsed(nP)) ? pSys->NpcTemplateOf(nP) : 0);
	return 1;
}

int LuaPARTNER_GetEmotionDegree(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	Lua_PushNumber(L, (pSys && pSys->IsUsed(nP)) ? pSys->GetP(nP, PTP_EMOTION) : 0);
	return 1;
}

int LuaPARTNER_SetEmotionDegree(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	if (pSys && pSys->IsUsed(nP))
	{
		pSys->SetP(nP, PTP_EMOTION, sArgInt(L, 2, 0));
		pSys->AddEmotion(nP, 0);                 // kep vao [min,max]
	}
	return 0;
}

int LuaPARTNER_AddEmotionDegree(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	if (pSys && pSys->IsUsed(nP))
		pSys->AddEmotion(nP, sArgInt(L, 2, 0));
	return 0;
}

int LuaPARTNER_GetBirthday(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	Lua_PushNumber(L, (pSys && pSys->IsUsed(nP)) ? pSys->GetP(nP, PTP_GENTIME) : 0);
	return 1;
}

int LuaPARTNER_GetGenData(Lua_State* L)
{
	// tra 9 so GenInfo + GenTime + RandSeed = 11 gia tri
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	if (!pSys || !pSys->IsUsed(nP)) { Lua_PushNumber(L, 0); return 1; }
	Lua_PushNumber(L, pSys->GetP(nP, PTP_FEATURE));
	Lua_PushNumber(L, pSys->GetP(nP, PTP_SERIES));
	Lua_PushNumber(L, pSys->GetP(nP, PTP_CHARACTER));
	for (int a = 0; a < PARTNER_APT_NUM; a++)
		Lua_PushNumber(L, pSys->GetP(nP, PTP_APT0 + a));
	Lua_PushNumber(L, pSys->GetP(nP, PTP_GENTIME));
	Lua_PushNumber(L, pSys->GetP(nP, PTP_RANDSEED));
	return 11;
}

int LuaPARTNER_GetAptitudes(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	if (!pSys || !pSys->IsUsed(nP)) return 0;
	for (int a = 0; a < PARTNER_APT_NUM; a++)
		Lua_PushNumber(L, pSys->GetP(nP, PTP_APT0 + a));
	return PARTNER_APT_NUM;
}

int LuaPARTNER_GetAttribs(Lua_State* L)
{
	// tra 6 gia tri HIEN TAI (init + inc*(lv-1))
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	if (!pSys || !pSys->IsUsed(nP)) return 0;
	int nLevel = pSys->GetP(nP, PTP_LEVEL);
	for (int a = 0; a < PARTNER_APT_NUM; a++)
	{
		double d = ((double)pSys->GetP(nP, PTP_ATTRINIT0 + a)
			+ (double)pSys->GetP(nP, PTP_ATTRINC0 + a) * (nLevel - 1)) / 10000.0;
		Lua_PushNumber(L, d);
	}
	return PARTNER_APT_NUM;
}

int LuaPARTNER_GetAttribsInc(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	if (!pSys || !pSys->IsUsed(nP)) return 0;
	for (int a = 0; a < PARTNER_APT_NUM; a++)
		Lua_PushNumber(L, (double)pSys->GetP(nP, PTP_ATTRINC0 + a) / 10000.0);
	return PARTNER_APT_NUM;
}

int LuaPARTNER_SetAttribs(Lua_State* L)
{
	// SetAttribs(idx, a1..a6) - dat GIA TRI INIT truc tiep
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	if (!pSys || !pSys->IsUsed(nP)) { Lua_PushNumber(L, 0); return 1; }
	for (int a = 0; a < PARTNER_APT_NUM; a++)
	{
		if (Lua_GetTopIndex(L) >= 2 + a && Lua_IsNumber(L, 2 + a))
			pSys->SetP(nP, PTP_ATTRINIT0 + a, (int)(Lua_ValueToNumber(L, 2 + a) * 10000.0));
	}
	pSys->ApplyToNpc(nP);
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaPARTNER_GetResists(Lua_State* L)
{
	// 5 khang hien tai theo he (thu tu resist.txt: physics cold light fire poison)
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	if (!pSys || !pSys->IsUsed(nP)) return 0;
	int nSeries = pSys->GetP(nP, PTP_SERIES);
	int nLevel = pSys->GetP(nP, PTP_LEVEL);
	if (nSeries < 0 || nSeries >= PARTNER_SERIES_NUM) nSeries = 0;
	for (int k = 0; k < PARTNER_RESIST_NUM; k++)
	{
		double d = g_PartnerTables.Resist[nSeries].fInit[k]
			+ g_PartnerTables.Resist[nSeries].fInc[k] * (nLevel - 1);
		Lua_PushNumber(L, d);
	}
	return PARTNER_RESIST_NUM;
}

int LuaPARTNER_SetResists(Lua_State* L)
{
	// dat khang truc tiep len npc dang goi ra
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	if (!pSys || !pSys->IsUsed(nP)) { Lua_PushNumber(L, 0); return 1; }
	if (pSys->m_nNpcOfPartner == nP && pSys->m_nNpcIdx > 0 &&
		NpcSet.IsNpcExist(pSys->m_nNpcIdx, pSys->m_dwNpcID))
	{
		KNpc* pNpc = &Npc[pSys->m_nNpcIdx];
		int v[5];
		for (int k = 0; k < 5; k++) v[k] = sArgInt(L, 2 + k, 0);
		pNpc->m_PhysicsResist = pNpc->m_CurrentPhysicsResist = v[0];
		pNpc->m_ColdResist = pNpc->m_CurrentColdResist = v[1];
		pNpc->m_LightResist = pNpc->m_CurrentLightResist = v[2];
		pNpc->m_FireResist = pNpc->m_CurrentFireResist = v[3];
		pNpc->m_PoisonResist = pNpc->m_CurrentPoisonResist = v[4];
	}
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaPARTNER_GetTaskValue(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	int nVar = sArgInt(L, 2, 0);
	if (!pSys || !pSys->IsUsed(nP) || nVar < 1 || nVar > PARTNER_TASKVALUE_NUM)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, pSys->GetP(nP, PTP_TASKVALUE0 + nVar - 1));
	return 1;
}

int LuaPARTNER_SetTaskValue(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	int nVar = sArgInt(L, 2, 0);
	int nVal = sArgInt(L, 3, 0);
	if (pSys && pSys->IsUsed(nP) && nVar >= 1 && nVar <= PARTNER_TASKVALUE_NUM)
		pSys->SetP(nP, PTP_TASKVALUE0 + nVar - 1, nVal);
	else if (pSys)
	{
		char szName[PARTNER_NAME_LEN + 4] = "";
		if (pSys->IsUsed(nP)) pSys->GetName(nP, szName);
		Partner_WriteLog("[PartnerDataError:TaskValue] [%s:%s] [PartnerName:%s] [TaskIndex:%d]",
			Player[nIdx].m_AccoutName, Player[nIdx].m_PlayerName, szName, nVar);
	}
	return 0;
}

int LuaPARTNER_GetSkillInfo(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	int nSkillId = sArgInt(L, 2, 0);
	int nLevel = 0, nExp = 0;
	if (pSys && pSys->IsUsed(nP))
		pSys->GetSkillInfo(nP, nSkillId, &nLevel, &nExp);
	Lua_PushNumber(L, nLevel);
	Lua_PushNumber(L, nExp);
	return 2;
}

int LuaPARTNER_GetAllSkill(Lua_State* L)
{
	// tra BANG {skillId, ...} theo loai; khong co gi -> nil (script goc kiem nil)
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	int nType = sArgInt(L, 2, -1);
	if (!pSys || !pSys->IsUsed(nP) || nType < 0)
	{
		Lua_PushNil(L);
		return 1;
	}
	int nOff, nMax;
	sSkillSlotRange(nType, &nOff, &nMax);
	int nCount = 0;
	int i;
	for (i = 0; i < nMax; i++)
		if (pSys->GetP(nP, nOff + i) > 0) nCount++;
	if (nCount == 0)
	{
		Lua_PushNil(L);
		return 1;
	}
	Lua_NewTable(L);
	nCount = 0;
	for (i = 0; i < nMax; i++)
	{
		int nPack = pSys->GetP(nP, nOff + i);
		if (nPack > 0)
		{
			Lua_PushNumber(L, nPack / 1000);
			Lua_RawSetI(L, -2, ++nCount);
		}
	}
	return 1;
}

// --- thao tac ---
int LuaPARTNER_AddFightPartner(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	if (!pSys) { Lua_PushNumber(L, 0); return 1; }
	int nFeature = sArgInt(L, 1, 0);
	int nSeries = sArgInt(L, 2, -1);
	int nCharacter = sArgInt(L, 3, 1);
	int nTop = Lua_GetTopIndex(L);
	int nRet;
	if (nTop >= 9)
	{
		int nApt[PARTNER_APT_NUM];
		for (int a = 0; a < PARTNER_APT_NUM; a++)
			nApt[a] = sArgInt(L, 4 + a, 1);
		nRet = pSys->AddFightPartner(nFeature, nSeries, nCharacter, nApt, 0);
	}
	else
	{
		int nMode = sArgInt(L, 4, 1);            // dang 4 tham so: aptitude_mode
		nRet = pSys->AddFightPartner(nFeature, nSeries, nCharacter, NULL, nMode);
	}
	Lua_PushNumber(L, nRet);
	return 1;
}

int LuaPARTNER_RemovePartner(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	Lua_PushNumber(L, pSys ? pSys->RemovePartner(nP) : 0);
	return 1;
}

int LuaPARTNER_SetCurPartner(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	Lua_PushNumber(L, pSys ? pSys->SetCurPartner(nP) : 0);
	return 1;
}

int LuaPARTNER_CallOutCurPartner(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nFlag = sArgInt(L, 1, 1);                // 0 tham so = goi ra
	Lua_PushNumber(L, pSys ? pSys->CallOut(nFlag) : 0);
	return 1;
}

int LuaPARTNER_AddExp(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	int nExp = sArgInt(L, 2, 0);
	int nFlag = sArgInt(L, 3, 1);                // tham so 3 tuy chon
	Lua_PushNumber(L, pSys ? pSys->AddExp(nP, nExp, nFlag) : 0);
	return 1;
}

int LuaPARTNER_LevelUp(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	if (pSys && pSys->IsUsed(nP))
	{
		int nLevel = pSys->GetP(nP, PTP_LEVEL);
		if (nLevel < PARTNER_MAX_LEVEL)
			pSys->LevelUpTo(nP, nLevel + 1);
	}
	return 0;
}

int LuaPARTNER_AddSkill(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	int nType = sArgInt(L, 2, 0);
	int nSkillId = sArgInt(L, 3, 0);
	int nLevel = sArgInt(L, 4, 1);
	int nExp = sArgInt(L, 5, 0);
	Lua_PushNumber(L, pSys ? pSys->AddSkill(nP, nType, nSkillId, nLevel, nExp) : 0);
	return 1;
}

int LuaPARTNER_RemoveSkill(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	int nType = sArgInt(L, 2, -1);
	int nSkillId = sArgInt(L, 3, 0);
	if (pSys && Lua_GetTopIndex(L) < 3)
	{
		// dang 2 tham so (idx, skillId): tim o moi loai
		nSkillId = sArgInt(L, 2, 0);
		int nDone = 0;
		for (int t = 0; t < 4 && !nDone; t++)
			nDone = pSys->RemoveSkill(nP, t, nSkillId);
		Lua_PushNumber(L, nDone);
		return 1;
	}
	Lua_PushNumber(L, pSys ? pSys->RemoveSkill(nP, nType, nSkillId) : 0);
	return 1;
}

int LuaPARTNER_RemoveAllSkill(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	int nType = sArgInt(L, 2, -1);
	if (pSys && pSys->IsUsed(nP))
	{
		if (nType < 0)
			for (int t = 0; t < 4; t++) pSys->RemoveAllSkill(nP, t);
		else
			pSys->RemoveAllSkill(nP, nType);
	}
	return 0;
}

int LuaPARTNER_SetStandbySkill(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	int nSkillId = sArgInt(L, 2, 0);
	if (pSys && pSys->IsUsed(nP))
		pSys->SetP(nP, PTP_STANDBYSKILL, nSkillId);
	return 0;
}

int LuaPARTNER_ReGenAttribsInc(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	if (pSys && pSys->IsUsed(nP))
		pSys->ReGenAttribsInc(nP);
	return 0;
}

int LuaPARTNER_ChangeCharacter(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	int nChar = sArgInt(L, 2, 1);
	if (pSys && pSys->IsUsed(nP) && nChar >= 1 && nChar <= PARTNER_CHARACTER_NUM)
	{
		pSys->SetP(nP, PTP_CHARACTER, nChar);
		Lua_PushNumber(L, 1);
	}
	else
		Lua_PushNumber(L, 0);
	return 1;
}

int LuaPARTNER_ChangeFeature(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	int nFeature = sArgInt(L, 2, 0);
	if (pSys && pSys->IsUsed(nP) && nFeature >= 1 && nFeature <= PARTNER_FEATURE_NUM)
	{
		pSys->SetP(nP, PTP_FEATURE, nFeature);
		if (pSys->m_nNpcOfPartner == nP && pSys->m_nNpcIdx > 0 &&
			NpcSet.IsNpcExist(pSys->m_nNpcIdx, pSys->m_dwNpcID))
		{
			// dung khuon he MAT NA de doi hinh instance (KNpc::ReSetRes)
			int nTpl = pSys->NpcTemplateOf(nP);
			if (nTpl > 0)
			{
				Npc[pSys->m_nNpcIdx].GetNpcCopyFromTemplate(nTpl,
					pSys->GetP(nP, PTP_LEVEL), pSys->GetP(nP, PTP_SERIES));
				Npc[pSys->m_nNpcIdx].m_Kind = kind_partner;
				pSys->ApplyToNpc(nP);
			}
		}
		Lua_PushNumber(L, 1);
	}
	else
		Lua_PushNumber(L, 0);
	return 1;
}

int LuaPARTNER_ChangeFeatureOfPeriod(Lua_State* L)
{
	// doi hinh theo THOI KY (1..3) giu nguyen feature goc
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	int nPeriod = sArgInt(L, 2, 1);
	if (!pSys || !pSys->IsUsed(nP) || nPeriod < 1 || nPeriod > PARTNER_PERIOD_NUM)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nFeature = pSys->GetP(nP, PTP_FEATURE);
	int nTpl = g_PartnerTables.Feature[nFeature].nNpcIdxPeriod[nPeriod - 1];
	if (nTpl > 0 && pSys->m_nNpcOfPartner == nP && pSys->m_nNpcIdx > 0 &&
		NpcSet.IsNpcExist(pSys->m_nNpcIdx, pSys->m_dwNpcID))
	{
		Npc[pSys->m_nNpcIdx].GetNpcCopyFromTemplate(nTpl,
			pSys->GetP(nP, PTP_LEVEL), pSys->GetP(nP, PTP_SERIES));
		Npc[pSys->m_nNpcIdx].m_Kind = kind_partner;
		pSys->ApplyToNpc(nP);
	}
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaPARTNER_GetEndure(Lua_State* L)
{
	// the luc: ban goc chua thay call-site; tra % mau hien tai cua npc dang goi ra
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nVal = 0;
	if (pSys && pSys->m_nNpcIdx > 0 && NpcSet.IsNpcExist(pSys->m_nNpcIdx, pSys->m_dwNpcID))
	{
		KNpc* pNpc = &Npc[pSys->m_nNpcIdx];
		if (pNpc->m_LifeMax > 0)
			nVal = pNpc->m_CurrentLife * 100 / pNpc->m_LifeMax;
	}
	Lua_PushNumber(L, nVal);
	return 1;
}

int LuaPARTNER_SetCallOutSwitch(Lua_State* L)
{
	// 1 = CAM goi dong hanh (map PARTNER_OFF); 0 = cho phep
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	if (pSys)
	{
		int nSwitch = sArgInt(L, 1, 0);
		pSys->SetG(PTG_CALLOUT_SWITCH, nSwitch);
		if (nSwitch && pSys->IsCallOut())
			pSys->Recall(TRUE);
	}
	return 0;
}

int LuaPARTNER_AddState(Lua_State* L)
{
	// AddState(skillId, level) len con dang goi ra
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nSkillId = sArgInt(L, 1, 0);
	int nLevel = sArgInt(L, 2, 1);
	if (pSys && nSkillId > 0 && pSys->m_nNpcIdx > 0 &&
		NpcSet.IsNpcExist(pSys->m_nNpcIdx, pSys->m_dwNpcID))
	{
		KSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(nSkillId, nLevel);
		if (pSkill)
		{
			pSkill->CastStateSkill(pSys->m_nNpcIdx, 0, 0, -1, TRUE);
			Lua_PushNumber(L, 1);
			return 1;
		}
	}
	Lua_PushNumber(L, 0);
	return 1;
}

// --- tu chat le ---
static int sAddAptitude(Lua_State* L, int nAttr)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nP = sArgInt(L, 1, 0);
	int nAdd = sArgInt(L, 2, 1);
	if (!pSys || !pSys->IsUsed(nP)) { Lua_PushNumber(L, 0); return 1; }
	int nApt = pSys->GetP(nP, PTP_APT0 + nAttr) + nAdd;
	if (nApt < 1) nApt = 1;
	if (nApt > 10) nApt = 10;
	pSys->SetP(nP, PTP_APT0 + nAttr, nApt);
	Lua_PushNumber(L, nApt);
	return 1;
}

int LuaPARTNER_AddLifeAptitude(Lua_State* L)          { return sAddAptitude(L, pattr_life); }
int LuaPARTNER_AddStrengthAptitude(Lua_State* L)      { return sAddAptitude(L, pattr_strength); }
int LuaPARTNER_AddHitTargetRateAptitude(Lua_State* L) { return sAddAptitude(L, pattr_hitrate); }
int LuaPARTNER_AddDefenceAptitude(Lua_State* L)       { return sAddAptitude(L, pattr_defence); }
int LuaPARTNER_AddSpeedAptitude(Lua_State* L)         { return sAddAptitude(L, pattr_speed); }
int LuaPARTNER_AddLuckAptitude(Lua_State* L)          { return sAddAptitude(L, pattr_luck); }

// --- tui dong hanh ---
int LuaGetPartnerBagLevel2(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	Lua_PushNumber(L, pSys ? pSys->GetG(PTG_BAGLEVEL) : 0);
	return 1;
}

int LuaSetPartnerBagLevel2(Lua_State* L)
{
	int nIdx;
	KPartnerSys* pSys = sGetSys(L, &nIdx);
	int nLevel = sArgInt(L, 1, 0);
	if (pSys)
	{
		if (nLevel < 0) nLevel = 0;
		if (nLevel > 10) nLevel = 10;
		pSys->SetG(PTG_BAGLEVEL, nLevel);
		Lua_PushNumber(L, 1);
	}
	else
		Lua_PushNumber(L, 0);
	return 1;
}

#endif // _SERVER
