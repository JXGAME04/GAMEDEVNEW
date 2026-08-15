// KJx2Title.cpp - xem KJx2Title.h. DOT E cong thanh JX2 (E4).

// LUU Y: Core build voi PCH "Use" qua KCore.h - moi thu TRUOC dong include nay
// deu bi compiler bo qua, nen KCore.h PHAI dung dau tien.
#include "KCore.h"
#include "KWin32.h"

#ifdef _SERVER

#include "KEngine.h"
#include "KDebug.h"
#include "LuaLib.h"
#include "KPlayerSet.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KSkills.h"
#include "KSkillManager.h"
#include "KJx2Title.h"
#include <map>
#include <string>
#include <string.h>
#include <stdio.h>
#include <time.h>

// dinh nghia trong ScriptFuns.cpp (khong co header - idiom KTongJX2.cpp:32)
extern int GetPlayerIndex(Lua_State* L);

#define JX2TITLE_FILE		"\\settings\\playertitle_jx2.txt"
#define JX2TITLE_EXT_MAX	5

//////////////////////////////////////////////////////////////////////
// Bang cau hinh (playertitle_jx2.txt - tab-separated, ten TCVN3 giu byte)
// Cot: TitleName  TitleId  SpeicalGraphic  FaceId  AuraSkill  AuraSkillLevel
//      ExtSkill1..5 + Level  (Memo/HidSkill khong dung)
//////////////////////////////////////////////////////////////////////
struct KJx2TitleRow
{
	char	szName[64];
	int		nAura;
	int		nAuraLv;
	int		nExt[JX2TITLE_EXT_MAX];
	int		nExtLv[JX2TITLE_EXT_MAX];
};

struct KJx2TitleHold
{
	int		nTimeType;		// 0 vinh vien / 1 frame tuong doi / 2 moc MMDDHHMM
	int		nTime;			// tham so goc luc Add
	int		nExpireAt;		// type 1: epoch giay het han; khac: 0
};

struct KJx2PlayerTitles
{
	std::map<int, KJx2TitleHold>	mapOwned;
	int								nActive;
};

static std::map<int, KJx2TitleRow>				s_TitleRows;
static std::map<std::string, KJx2PlayerTitles>	s_PlayerTitles;	// khoa = ten nhan vat
static bool										s_bTitleLoaded = false;

static void sTStrCpy(char* szDst, const char* szSrc, int nDstSize)
{
	strncpy(szDst, szSrc ? szSrc : "", nDstSize - 1);
	szDst[nDstSize - 1] = 0;
}

static void sTitleLoadRows()
{
	if (s_bTitleLoaded)
		return;
	s_bTitleLoaded = true;
	char szRoot[MAX_PATH], szPath[MAX_PATH];
	g_GetRootPath(szRoot);
	sprintf(szPath, "%s%s", szRoot, JX2TITLE_FILE);
	FILE* f = fopen(szPath, "rb");
	if (!f)
	{
		g_DebugLog((LPSTR)"KJx2Title: khong doc duoc %s", szPath);
		return;
	}
	char szLine[1024];
	int nLineNo = 0;
	while (fgets(szLine, sizeof(szLine), f))
	{
		nLineNo++;
		if (nLineNo == 1)
			continue;	// header
		// tach tab tai cho
		char* apCol[20];
		int nCol = 0;
		char* p = szLine;
		apCol[nCol++] = p;
		while (*p && nCol < 20)
		{
			if (*p == '\t')
			{
				*p = 0;
				apCol[nCol++] = p + 1;
			}
			else if (*p == '\r' || *p == '\n')
			{
				*p = 0;
				break;
			}
			p++;
		}
		if (nCol < 2)
			continue;
		int nId = atoi(apCol[1]);
		if (nId <= 0)
			continue;
		KJx2TitleRow row;
		memset(&row, 0, sizeof(row));
		sTStrCpy(row.szName, apCol[0], sizeof(row.szName));
		// cat khoang trang duoi ten (bang goc co space thua cuoi o nhieu dong)
		int nLen = (int)strlen(row.szName);
		while (nLen > 0 && row.szName[nLen - 1] == ' ')
			row.szName[--nLen] = 0;
		row.nAura = (nCol > 4) ? atoi(apCol[4]) : 0;
		row.nAuraLv = (nCol > 5) ? atoi(apCol[5]) : 0;
		for (int k = 0; k < JX2TITLE_EXT_MAX; k++)
		{
			if (nCol > 6 + k * 2)
				row.nExt[k] = atoi(apCol[6 + k * 2]);
			if (nCol > 7 + k * 2)
				row.nExtLv[k] = atoi(apCol[7 + k * 2]);
		}
		s_TitleRows[nId] = row;
	}
	fclose(f);
	g_DebugLog((LPSTR)"KJx2Title: nap %d danh hieu", (int)s_TitleRows.size());
}

//////////////////////////////////////////////////////////////////////
// Helper
//////////////////////////////////////////////////////////////////////
static int sTitleExpired(KJx2TitleHold* pHold)
{
	if (pHold->nTimeType == 1)
		return (pHold->nExpireAt > 0 && (int)time(NULL) >= pHold->nExpireAt) ? 1 : 0;
	if (pHold->nTimeType == 2)
	{
		// moc tuyet doi dang so MMDDHHMM (nhu AddSkillState type 2 cua goc)
		time_t t = time(NULL);
		struct tm* pTm = localtime(&t);
		if (!pTm)
			return 0;
		int nNow = (pTm->tm_mon + 1) * 1000000 + pTm->tm_mday * 10000 +
			pTm->tm_hour * 100 + pTm->tm_min;
		return (nNow >= pHold->nTime) ? 1 : 0;
	}
	return 0;
}

static KJx2PlayerTitles* sTitleOfPlayer(const char* szName, bool bCreate)
{
	if (!szName || !szName[0])
		return NULL;
	std::string sKey(szName);
	std::map<std::string, KJx2PlayerTitles>::iterator it = s_PlayerTitles.find(sKey);
	if (it != s_PlayerTitles.end())
		return &it->second;
	if (!bCreate)
		return NULL;
	KJx2PlayerTitles& r = s_PlayerTitles[sKey];
	r.nActive = 0;
	return &r;
}

static const char* sPlayerNameOf(Lua_State* L, int* pnPlayerIdx)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (pnPlayerIdx)
		*pnPlayerIdx = nPlayerIndex;
	if (nPlayerIndex <= 0)
		return NULL;
	return Player[nPlayerIndex].m_PlayerName;
}

// go / gan skill-state cua title (bang hien tai toan 0 -> thuong la no-op).
// bApply = 1 gan, 0 go. Dung idiom LuaAddSkillState (ScriptFuns.cpp:11400):
// CastStateSkill ap len Npc cua nguoi choi; go = cast lai voi thoi gian 1 frame
// khong co API go truc tiep -> go bang ForceClearStateSkillEffect neu co aura.
static void sTitleApplySkills(int nPlayerIdx, KJx2TitleRow* pRow, int bApply)
{
	if (!pRow || nPlayerIdx <= 0 || Player[nPlayerIdx].m_nIndex <= 0)
		return;
	for (int k = -1; k < JX2TITLE_EXT_MAX; k++)
	{
		int nSkill = (k < 0) ? pRow->nAura : pRow->nExt[k];
		int nLv = (k < 0) ? pRow->nAuraLv : pRow->nExtLv[k];
		if (nSkill <= 0)
			continue;
		if (!bApply)
		{
			// go skill-state khi tat/doi title (phan bien E4 VUA-1: 69/348 dong
			// bang co ExtSkill != 0 - khong go la buff chong vinh vien);
			// idiom LuaRemoveSkillState (ScriptFuns.cpp:11449)
			Npc[Player[nPlayerIdx].m_nIndex].ForceClearStateSkillEffect(nSkill);
			continue;
		}
		if (nLv <= 0)
			nLv = 1;
		KSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(nSkill, nLv);
		if (pSkill)
			pSkill->CastStateSkill(Player[nPlayerIdx].m_nIndex, 0, 0, -1, FALSE);
	}
}

//////////////////////////////////////////////////////////////////////
// Ham Lua
//////////////////////////////////////////////////////////////////////

// (nId, nTimeType, nTime) >= 3 doi, nId > 0
int LuaTitle_AddTitle(Lua_State* L)
{
	sTitleLoadRows();
	if (Lua_GetTopIndex(L) >= 3 && Lua_IsNumber(L, 1) && Lua_IsNumber(L, 2) && Lua_IsNumber(L, 3))
	{
		int nId = (int)Lua_ValueToNumber(L, 1);
		int nTimeType = (int)Lua_ValueToNumber(L, 2);
		int nTime = (int)Lua_ValueToNumber(L, 3);
		const char* szName = sPlayerNameOf(L, NULL);
		if (nId > 0 && szName)
		{
			KJx2PlayerTitles* p = sTitleOfPlayer(szName, true);
			KJx2TitleHold h;
			h.nTimeType = nTimeType;
			h.nTime = nTime;
			h.nExpireAt = 0;
			if (nTimeType == 1 && nTime > 0)
				h.nExpireAt = (int)time(NULL) + nTime / 18;	// frame -> giay
			p->mapOwned[nId] = h;
		}
	}
	return 0;
}

// (nId; 0 = tat het) -> 1 so
int LuaTitle_ActiveTitle(Lua_State* L)
{
	sTitleLoadRows();
	int nOk = 0;
	int nPlayerIdx = 0;
	const char* szName = sPlayerNameOf(L, &nPlayerIdx);
	if (szName && Lua_IsNumber(L, 1))
	{
		int nId = (int)Lua_ValueToNumber(L, 1);
		KJx2PlayerTitles* p = sTitleOfPlayer(szName, true);
		// tat title cu (go skill phu neu co)
		if (p->nActive > 0)
		{
			std::map<int, KJx2TitleRow>::iterator itOld = s_TitleRows.find(p->nActive);
			if (itOld != s_TitleRows.end())
				sTitleApplySkills(nPlayerIdx, &itOld->second, 0);
			p->nActive = 0;
		}
		if (nId > 0)
		{
			std::map<int, KJx2TitleHold>::iterator itH = p->mapOwned.find(nId);
			if (itH != p->mapOwned.end() && !sTitleExpired(&itH->second))
			{
				p->nActive = nId;
				std::map<int, KJx2TitleRow>::iterator itRow = s_TitleRows.find(nId);
				if (itRow != s_TitleRows.end())
					sTitleApplySkills(nPlayerIdx, &itRow->second, 1);
				nOk = 1;
			}
		}
		else
			nOk = 1;	// tat het = thanh cong
	}
	Lua_PushNumber(L, nOk);
	return 1;
}

int LuaTitle_RemoveTitle(Lua_State* L)
{
	sTitleLoadRows();
	const char* szName = sPlayerNameOf(L, NULL);
	if (szName && Lua_IsNumber(L, 1))
	{
		int nId = (int)Lua_ValueToNumber(L, 1);
		KJx2PlayerTitles* p = sTitleOfPlayer(szName, false);
		if (p)
		{
			p->mapOwned.erase(nId);
			if (p->nActive == nId)
				p->nActive = 0;
		}
	}
	return 0;
}

// (nId) -> nTimeType, nTime ; khong so huu -> (0,0)
int LuaTitle_GetTitleInfo(Lua_State* L)
{
	sTitleLoadRows();
	int nTimeType = 0, nTime = 0;
	const char* szName = sPlayerNameOf(L, NULL);
	if (szName && Lua_IsNumber(L, 1))
	{
		KJx2PlayerTitles* p = sTitleOfPlayer(szName, false);
		if (p)
		{
			std::map<int, KJx2TitleHold>::iterator it = p->mapOwned.find((int)Lua_ValueToNumber(L, 1));
			if (it != p->mapOwned.end() && !sTitleExpired(&it->second))
			{
				nTimeType = it->second.nTimeType;
				if (it->second.nTimeType == 1 && it->second.nExpireAt > 0)
				{
					nTime = (it->second.nExpireAt - (int)time(NULL)) * 18;	// con lai (frame)
					if (nTime < 0)
						nTime = 0;
				}
				else
					nTime = it->second.nTime;
			}
		}
	}
	Lua_PushNumber(L, nTimeType);
	Lua_PushNumber(L, nTime);
	return 2;
}

int LuaTitle_GetTitleName(Lua_State* L)
{
	sTitleLoadRows();
	if (Lua_IsNumber(L, 1))
	{
		std::map<int, KJx2TitleRow>::iterator it = s_TitleRows.find((int)Lua_ValueToNumber(L, 1));
		if (it != s_TitleRows.end())
		{
			Lua_PushString(L, it->second.szName);
			return 1;
		}
	}
	Lua_PushString(L, (char*)"");
	return 1;
}

int LuaTitle_GetActiveTitle(Lua_State* L)
{
	sTitleLoadRows();
	int nActive = 0;
	const char* szName = sPlayerNameOf(L, NULL);
	if (szName)
	{
		KJx2PlayerTitles* p = sTitleOfPlayer(szName, false);
		if (p)
			nActive = p->nActive;
	}
	Lua_PushNumber(L, nActive);
	return 1;
}

// () -> BANG cac id dang so huu (chua het han) / nil (0 gia tri) khi khong co gi
// Lua 4.0: lua_newtable + Lua_SetTable (tien le LuaGetSubWorldPlayerIndexes)
int LuaTitle_GetTitleTab(Lua_State* L)
{
	sTitleLoadRows();
	const char* szName = sPlayerNameOf(L, NULL);
	KJx2PlayerTitles* p = szName ? sTitleOfPlayer(szName, false) : NULL;
	if (!p || p->mapOwned.empty())
		return 0;	// nil
	int nCount = 0;
	lua_newtable(L);
	std::map<int, KJx2TitleHold>::iterator it;
	for (it = p->mapOwned.begin(); it != p->mapOwned.end(); ++it)
	{
		if (sTitleExpired(&it->second))
			continue;
		Lua_PushNumber(L, ++nCount);	// key 1..n
		Lua_PushNumber(L, it->first);	// value = title id
		Lua_SetTable(L, -3);
	}
	if (nCount == 0)
	{
		// bang rong -> goc tra nil (titlefuncs.lua:12 kiem getn == 0 truoc,
		// nhung nil cung duoc kiem) - giu bang rong cho don gian, getn = 0
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////
// Cho E6 (doi chu thanh cap/thu Thai Thu theo TEN, nguoi choi co the offline)
//////////////////////////////////////////////////////////////////////
void KJx2Title_GrantByName(const char* szPlayerName, int nTitleId)
{
	sTitleLoadRows();
	if (!szPlayerName || !szPlayerName[0] || nTitleId <= 0)
		return;
	KJx2PlayerTitles* p = sTitleOfPlayer(szPlayerName, true);
	KJx2TitleHold h;
	h.nTimeType = 0;
	h.nTime = 0;
	h.nExpireAt = 0;
	p->mapOwned[nTitleId] = h;
}

void KJx2Title_RevokeByName(const char* szPlayerName, int nTitleId)
{
	if (!szPlayerName || !szPlayerName[0])
		return;
	KJx2PlayerTitles* p = sTitleOfPlayer(szPlayerName, false);
	if (p)
	{
		p->mapOwned.erase(nTitleId);
		if (p->nActive == nTitleId)
			p->nActive = 0;
	}
}

#endif // _SERVER
