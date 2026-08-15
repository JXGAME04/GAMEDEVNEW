// KJx2Battle.cpp - xem KJx2Battle.h. DOT E cong thanh JX2 (E4).

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
#include "KJx2Battle.h"
#include <map>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>

// dinh nghia trong ScriptFuns.cpp (idiom KTongJX2.cpp:32)
extern int GetPlayerIndex(Lua_State* L);

//////////////////////////////////////////////////////////////////////
// Store MOT tran (citywar 1 map; arena nhanh VN idle)
//////////////////////////////////////////////////////////////////////
struct KJx2BtMember
{
	char				szName[32];
	int					nSeq;		// so tran (chong ro diem tran truoc - phan bien E4 CAO-3)
	std::map<int, int>	mapData;	// type -> gia tri cuoi (cache cho ladder/offline)
};

static std::map<int, int>		s_Type2Task;	// type -> player task id
static std::map<int, int>		s_Bonus;		// khoa = nType*16 + nCamp
static std::map<int, int>		s_GameData;
static std::vector<int>			s_ViewTypes;
static char						s_szMissionName[64] = "";
static int						s_nRestTime = 0;
static std::vector<KJx2BtMember> s_Members;
static int						s_nBattleSeq = 1;	// tang moi BT_ClearBattle

// dua MOI task da map ve 0 cho mot nguoi choi dang online
static void sBtResetPlayerTasks(int nPlayerIdx)
{
	if (nPlayerIdx <= 0)
		return;
	std::map<int, int>::iterator t;
	for (t = s_Type2Task.begin(); t != s_Type2Task.end(); ++t)
	{
		if (t->second > 0)
			Player[nPlayerIdx].m_cTask.SetSaveVal(t->second, 0);
	}
}

static KJx2BtMember* sBtMember(const char* szName, bool bCreate)
{
	if (!szName || !szName[0])
		return NULL;
	for (size_t i = 0; i < s_Members.size(); i++)
	{
		if (strcmp(s_Members[i].szName, szName) == 0)
			return &s_Members[i];
	}
	if (!bCreate)
		return NULL;
	KJx2BtMember m;
	strncpy(m.szName, szName, sizeof(m.szName) - 1);
	m.szName[sizeof(m.szName) - 1] = 0;
	m.nSeq = 0;	// chua thuoc tran nao - lan SetData dau se reset task + gan seq
	s_Members.push_back(m);
	return &s_Members.back();
}

static int sBtTaskOfType(int nType)
{
	std::map<int, int>::iterator it = s_Type2Task.find(nType);
	return (it == s_Type2Task.end()) ? 0 : it->second;
}

//////////////////////////////////////////////////////////////////////
// Ham Lua
//////////////////////////////////////////////////////////////////////
int LuaBT_SetType2Task(Lua_State* L)
{
	if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 1) && Lua_IsNumber(L, 2))
		s_Type2Task[(int)Lua_ValueToNumber(L, 1)] = (int)Lua_ValueToNumber(L, 2);
	return 0;
}

int LuaBT_GetData(Lua_State* L)
{
	int nVal = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Lua_IsNumber(L, 1))
	{
		int nTask = sBtTaskOfType((int)Lua_ValueToNumber(L, 1));
		if (nTask > 0)
			nVal = Player[nPlayerIndex].m_cTask.GetSaveVal(nTask);
	}
	Lua_PushNumber(L, nVal);
	return 1;
}

int LuaBT_SetData(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 1) && Lua_IsNumber(L, 2))
	{
		int nType = (int)Lua_ValueToNumber(L, 1);
		int nVal = (int)Lua_ValueToNumber(L, 2);
		KJx2BtMember* pMem = sBtMember(Player[nPlayerIndex].m_PlayerName, true);
		// lan cham dau cua nguoi nay trong TRAN nay: quet 0 het task da map
		// (nguoi offline luc ClearBattle mang diem cu quay lai - CAO-3)
		if (pMem && pMem->nSeq != s_nBattleSeq)
		{
			sBtResetPlayerTasks(nPlayerIndex);
			pMem->mapData.clear();
			pMem->nSeq = s_nBattleSeq;
		}
		int nTask = sBtTaskOfType(nType);
		if (nTask > 0)
			Player[nPlayerIndex].m_cTask.SetSaveVal(nTask, nVal);
		if (pMem)
			pMem->mapData[nType] = nVal;
	}
	return 0;
}

// (phan bien E4 CHAN-4) trap.lua:52-53 goi TRUOC JoinCamp; chefu.lua:8 khi roi
int LuaBT_ClearPlayerData(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		sBtResetPlayerTasks(nPlayerIndex);
		KJx2BtMember* pMem = sBtMember(Player[nPlayerIndex].m_PlayerName, false);
		if (pMem)
			pMem->mapData.clear();
	}
	return 0;
}

int LuaBT_LeaveBattle(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		sBtResetPlayerTasks(nPlayerIndex);
		for (size_t i = 0; i < s_Members.size(); i++)
		{
			if (strcmp(s_Members[i].szName, Player[nPlayerIndex].m_PlayerName) == 0)
			{
				s_Members.erase(s_Members.begin() + i);
				break;
			}
		}
	}
	return 0;
}

int LuaBT_SetTypeBonus(Lua_State* L)
{
	if (Lua_GetTopIndex(L) >= 3 && Lua_IsNumber(L, 1) && Lua_IsNumber(L, 2) && Lua_IsNumber(L, 3))
	{
		int nType = (int)Lua_ValueToNumber(L, 1);
		int nCamp = (int)Lua_ValueToNumber(L, 2) & 15;
		s_Bonus[nType * 16 + nCamp] = (int)Lua_ValueToNumber(L, 3);
	}
	return 0;
}

int LuaBT_GetTypeBonus(Lua_State* L)
{
	int nVal = 0;
	if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 1) && Lua_IsNumber(L, 2))
	{
		int nType = (int)Lua_ValueToNumber(L, 1);
		int nCamp = (int)Lua_ValueToNumber(L, 2) & 15;
		std::map<int, int>::iterator it = s_Bonus.find(nType * 16 + nCamp);
		if (it != s_Bonus.end())
			nVal = it->second;
	}
	Lua_PushNumber(L, nVal);
	return 1;
}

int LuaBT_SetView(Lua_State* L)
{
	if (Lua_IsNumber(L, 1) && s_ViewTypes.size() < 32)
		s_ViewTypes.push_back((int)Lua_ValueToNumber(L, 1));
	return 0;
}

int LuaBT_SetMissionName(Lua_State* L)
{
	if (Lua_IsString(L, 1))
	{
		strncpy(s_szMissionName, Lua_ValueToString(L, 1), sizeof(s_szMissionName) - 1);
		s_szMissionName[sizeof(s_szMissionName) - 1] = 0;
	}
	return 0;
}

int LuaBT_SetGameData(Lua_State* L)
{
	if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 1) && Lua_IsNumber(L, 2))
		s_GameData[(int)Lua_ValueToNumber(L, 1)] = (int)Lua_ValueToNumber(L, 2);
	return 0;
}

int LuaBT_SetRestTime(Lua_State* L)
{
	if (Lua_IsNumber(L, 1))
		s_nRestTime = (int)Lua_ValueToNumber(L, 1);
	return 0;
}

// ta sap khi doc (GetTopTenInfo) - giu ham cho script goi
int LuaBT_SortLadder(Lua_State* L)
{
	return 0;
}

// (nRank 1..10, nType) -> szName, nValue ; ngoai dai -> ("", 0) KHONG nil
// (camper.lua:51: szName,nZhanGong = BT_GetTopTenInfo(i, PL_TOTALPOINT))
int LuaBT_GetTopTenInfo(Lua_State* L)
{
	const char* szName = "";
	int nValue = 0;
	if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 1) && Lua_IsNumber(L, 2))
	{
		int nRank = (int)Lua_ValueToNumber(L, 1);
		int nType = (int)Lua_ValueToNumber(L, 2);
		if (nRank >= 1 && (size_t)nRank <= s_Members.size())
		{
			// chon lan thu nRank lon nhat (selection tren ban sao chi so - store nho)
			std::vector<int> vIdx;
			for (size_t i = 0; i < s_Members.size(); i++)
				vIdx.push_back((int)i);
			for (int r = 0; r < nRank; r++)
			{
				int nBest = r;
				for (size_t j = r; j < vIdx.size(); j++)
				{
					std::map<int, int>::iterator a = s_Members[vIdx[j]].mapData.find(nType);
					std::map<int, int>::iterator b = s_Members[vIdx[nBest]].mapData.find(nType);
					int va = (a == s_Members[vIdx[j]].mapData.end()) ? 0 : a->second;
					int vb = (b == s_Members[vIdx[nBest]].mapData.end()) ? 0 : b->second;
					if (va > vb)
						nBest = (int)j;
				}
				int t = vIdx[r]; vIdx[r] = vIdx[nBest]; vIdx[nBest] = t;
			}
			KJx2BtMember* pMem = &s_Members[vIdx[nRank - 1]];
			szName = pMem->szName;
			std::map<int, int>::iterator it = pMem->mapData.find(nType);
			if (it != pMem->mapData.end())
				nValue = it->second;
		}
	}
	Lua_PushString(L, (char*)szName);
	Lua_PushNumber(L, nValue);
	return 2;
}

int LuaBT_UpdateMemberCount(Lua_State* L)
{
	return 0;
}

// xoa state tran + dua task da map ve 0 cho nguoi choi ONLINE (chong diem cu
// tran truoc ro ri sang tran sau; nguoi offline se duoc chinh script JoinCamp/
// InitMission chu ky sau ghi de)
int LuaBT_ClearBattle(Lua_State* L)
{
	for (size_t i = 0; i < s_Members.size(); i++)
	{
		for (int p = 1; p <= PlayerSet.GetPlayerMaxNumber(); p++)
		{
			if (Player[p].m_nIndex > 0 &&
				strcmp(Player[p].m_PlayerName, s_Members[i].szName) == 0)
			{
				std::map<int, int>::iterator t;
				for (t = s_Type2Task.begin(); t != s_Type2Task.end(); ++t)
				{
					if (t->second > 0)
						Player[p].m_cTask.SetSaveVal(t->second, 0);
				}
				break;
			}
		}
	}
	s_Members.clear();
	s_Bonus.clear();
	s_GameData.clear();
	s_ViewTypes.clear();
	s_szMissionName[0] = 0;
	s_nRestTime = 0;
	s_nBattleSeq++;	// nguoi offline quay lai se bi reset o lan SetData dau (CAO-3)
	// GIU s_Type2Task: InitMission tran ke tiep goi bt_setnormaltask2type ghi lai
	return 0;
}

int LuaBT_BroadView(Lua_State* L)		{ return 0; }
int LuaBT_BroadGameData(Lua_State* L)	{ return 0; }
int LuaBT_BroadAllLadder(Lua_State* L)	{ return 0; }
int LuaBT_BroadSelf(Lua_State* L)		{ return 0; }

#endif // _SERVER
