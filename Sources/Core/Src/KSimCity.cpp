// KSimCity.cpp - xem KSimCity.h. Port SimCity (bot nguoi choi gia lap) JX2 -> JX1.
//
// LUU Y: Core build voi PCH "Use" qua KCore.h - moi thu TRUOC dong include nay
// deu bi compiler bo qua, nen KCore.h PHAI dung dau tien (giong KJx2WarInfra.cpp).
#include "KCore.h"
#include "KWin32.h"

#ifdef _SERVER

#include "LuaLib.h"
#include "KNpc.h"           // KNpc, Npc[], MAX_NPC, do_walk
#include "KNpcSet.h"        // NpcSet
#include "KSubWorld.h"      // SubWorld[]
#include "KSubWorldSet.h"
#include "KTabFile.h"       // KTabFile - nap _preset.txt
#include "KSimCity.h"
#include <string.h>         // strncpy, strcmp, memset
#include <stdio.h>          // sscanf

// ============================================================================
// GD1 Fix1 - co danh dau bot giu can bang (xem KSimCity.h)
// ============================================================================

int LuaSC_SetBotFlag(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
	int nVal    = (int)Lua_ValueToNumber(L, 2);
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Npc[nNpcIdx].m_btSimCityBot = (BYTE)(nVal ? 1 : 0);
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaSC_GetBotFlag(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, Npc[nNpcIdx].m_btSimCityBot);
	return 1;
}

// ============================================================================
// GD2 - SO BOT + BANG LO TRINH + DI CHUYEN (driver C++ hook vao CoreServerShell::Breathe)
//
// Co che (da doc mã): SendCommand(do_walk, mpsX, mpsY) PUBLIC (Goto/RunTo private) =
//   ban-roi-quen: phat 1 lan/chang -> ProcCommand(nAI=1) -> Goto; moi tick engine tu buoc
//   (ServeMove), ne vat can cuc bo (KNpcFindPath::GetDir, men-tuong, KHONG A*), tu do_stand khi toi.
//   Bot kind_player DUNG giu m_ProcessAI=1 (OnStand KHONG ha) -> SendCommand tac dung NGAY.
//   DoWalk broadcast s2c_npcwalk TRUOC early-return -> CHI phat khi doi chang (chong spam).
//   Bot chi buoc khi region NONG (co nguoi choi gan) - KRegion nong qua AddPlayer, khong AddNpc.
// ============================================================================

#define SC_MAX_BOTS    4096
#define SC_MAX_ROUTE   64      // so lo trinh (chia se giua nhieu bot)
#define SC_MAX_NODE    320     // so waypoint moi lo trinh (tuyen dai nhat that = 237, du bien)
#define SC_ARRIVE_MPS  48      // toi chang khi trong 48 MPS; phai > buoc/tick (ServeMove kep <=31)
#define SC_STUCK_LIMIT 18      // so tick dung ì (chua toi) truoc khi bo qua chang (ket tuong)
#define SC_BOX_HALF    320     // ban kinh mac dinh patrol box (10 o)

struct SC_Route
{
	int nCount;
	int wpX[SC_MAX_NODE];      // toa do MPS (da ×32)
	int wpY[SC_MAX_NODE];
};

struct SC_Bot
{
	int   nNpcIdx;             // slot Npc[]
	DWORD dwID;                // chot danh tinh (chong reuse slot)
	int   nRouteId;            // chi so vao s_routes[]; -1 = chua co lo trinh
	int   nLeg;                // chang hien tai
	int   nDir;                // +1 / -1 (khi khong khep vong)
	int   bLoop;               // 1 = khep vong (%nCount); 0 = quay dau
	int   nLastX, nLastY;      // vi tri MPS lan truoc (phat hien ket)
	int   nStuck;              // so tick khong nhuc nhich
};

static SC_Route s_routes[SC_MAX_ROUTE];
static int      s_routeCount   = 0;
static SC_Bot   s_bots[SC_MAX_BOTS];
static int      s_botCount     = 0;
static int      g_bSimCityMove = 0;   // cong tac driver di chuyen

// ---- quan ly so bot ----
static int sc_Find(int nNpcIdx)
{
	for (int r = 0; r < s_botCount; r++)
		if (s_bots[r].nNpcIdx == nNpcIdx)
			return r;
	return -1;
}

static void sc_Register(int nNpcIdx)
{
	int r = sc_Find(nNpcIdx);
	if (r < 0)
	{
		if (s_botCount >= SC_MAX_BOTS)
			return;
		r = s_botCount++;
	}
	memset(&s_bots[r], 0, sizeof(SC_Bot));
	s_bots[r].nNpcIdx  = nNpcIdx;
	s_bots[r].dwID     = Npc[nNpcIdx].m_dwID;
	s_bots[r].nRouteId = -1;
	s_bots[r].nDir     = 1;
}

static void sc_Deregister(int nNpcIdx)
{
	int r = sc_Find(nNpcIdx);
	if (r < 0)
		return;
	s_bots[r] = s_bots[--s_botCount];   // nen mang
}

// tien chang (khep vong / quay dau), kep an toan chi so
static void sc_NextLeg(SC_Bot& b, SC_Route& rt)
{
	b.nLeg += b.nDir;
	if (b.bLoop)
	{
		if (b.nLeg >= rt.nCount) b.nLeg = 0;
		if (b.nLeg < 0)          b.nLeg = rt.nCount - 1;
	}
	else
	{
		if (b.nLeg >= rt.nCount) { b.nLeg = rt.nCount - 2; b.nDir = -1; }
		else if (b.nLeg < 0)     { b.nLeg = 1;             b.nDir = 1;  }
	}
	if (b.nLeg < 0)          b.nLeg = 0;
	if (b.nLeg >= rt.nCount) b.nLeg = 0;
}

// phat lenh di toi chang hien tai
static void sc_IssueLeg(SC_Bot& b, SC_Route& rt, int i)
{
	if (b.nLeg < 0)          b.nLeg = 0;
	if (b.nLeg >= rt.nCount) b.nLeg = 0;
	int tx = rt.wpX[b.nLeg];
	int ty = rt.wpY[b.nLeg];
	if (tx > 0 && ty > 0)
		Npc[i].SendCommand(do_walk, tx, ty);
	b.nStuck = 0;
}

// ============================================================================
// GD1 Fix2 - sinh / xoa bot kind_player mang ngoai hinh nguoi choi (sentinel -1/-2)
// ============================================================================

// SC_AddBot(nSex, nLevel, nSubWorldIdx, nMpsX, nMpsY, nSeries [, szName] [, nFaction] [, nLifeMax])
int LuaSC_AddBot(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 6)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nSex         = (int)Lua_ValueToNumber(L, 1);
	int nLevel       = (int)Lua_ValueToNumber(L, 2);
	int nSubWorldIdx = (int)Lua_ValueToNumber(L, 3);
	int nMpsX        = (int)Lua_ValueToNumber(L, 4);
	int nMpsY        = (int)Lua_ValueToNumber(L, 5);
	int nSeries      = (int)Lua_ValueToNumber(L, 6);

	if (nLevel < 1)   nLevel = 1;
	if (nLevel > 127) nLevel = 127;
	if (nSeries < 0 || nSeries > 5) nSeries = 0;

	int nTid = nSex ? PLAYER_FEMALE_NPCTEMPLATEID : PLAYER_MALE_NPCTEMPLATEID;

	int nNpcIdx = NpcSet.AddNpcSet2(MAKELONG(nLevel, nTid), nSeries, nSubWorldIdx, nMpsX, nMpsY);
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	KNpc* p = &Npc[nNpcIdx];

	p->m_btSimCityBot = 1;
	p->m_Level        = nLevel;
	p->m_Series       = nSeries;
	p->m_nSex         = nSex;

	if (Lua_GetTopIndex(L) >= 7 && Lua_IsString(L, 7))
	{
		char* pName = (char*)lua_tostring(L, 7);
		if (pName && pName[0])
		{
			strncpy(p->Name, pName, sizeof(p->Name) - 1);   // Name[32] - tranh tran buffer
			p->Name[sizeof(p->Name) - 1] = 0;
		}
	}
	if (Lua_GetTopIndex(L) >= 8)
	{
		int nFaction = (int)Lua_ValueToNumber(L, 8);
		p->m_Camp        = nFaction;
		p->m_CurrentCamp = nFaction;
	}
	if (Lua_GetTopIndex(L) >= 9)
	{
		int nLifeMax = (int)Lua_ValueToNumber(L, 9);
		if (nLifeMax > 0)
			p->m_LifeMax = nLifeMax;
	}

	p->RestoreNpcBaseInfo();          // fill m_Current* tu base (con kind_normal)
	p->m_Kind = kind_player;          // flip CUOI CUNG
	p->SetProcessAI(1);               // bot phai co AI de di duoc; xac dinh, khong phu thuoc lich su slot

	sc_Register(nNpcIdx);             // vao so de driver di chuyen (chua co lo trinh)

	Lua_PushNumber(L, nNpcIdx);
	return 1;
}

int LuaSC_DelBot(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int i = (int)Lua_ValueToNumber(L, 1);
	if (i <= 0 || i >= MAX_NPC || !Npc[i].m_btSimCityBot)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	if (Npc[i].m_SubWorldIndex >= 0 && Npc[i].m_RegionIndex >= 0)
	{
		SubWorld[Npc[i].m_SubWorldIndex].m_Region[Npc[i].m_RegionIndex].RemoveNpc(i);
		SubWorld[Npc[i].m_SubWorldIndex].m_Region[Npc[i].m_RegionIndex].DecRef(Npc[i].m_MapX, Npc[i].m_MapY, obj_npc);
		NpcSet.Remove(i);
		sc_Deregister(i);
		Lua_PushNumber(L, 1);
		return 1;
	}
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaSC_ClearBots(Lua_State* L)
{
	int nCount = 0;
	for (int i = 1; i < MAX_NPC; i++)
	{
		if (!Npc[i].m_btSimCityBot)
			continue;
		if (Npc[i].m_SubWorldIndex >= 0 && Npc[i].m_RegionIndex >= 0)
		{
			SubWorld[Npc[i].m_SubWorldIndex].m_Region[Npc[i].m_RegionIndex].RemoveNpc(i);
			SubWorld[Npc[i].m_SubWorldIndex].m_Region[Npc[i].m_RegionIndex].DecRef(Npc[i].m_MapX, Npc[i].m_MapY, obj_npc);
			NpcSet.Remove(i);
			nCount++;
		}
	}
	s_botCount   = 0;                 // xoa so bot
	s_routeCount = 0;                 // xoa bang lo trinh (khong con bot nao tham chieu)
	Lua_PushNumber(L, nCount);
	return 1;
}

// ============================================================================
// GD2 - dieu khien di chuyen
// ============================================================================

int LuaSC_MoveOn(Lua_State* L)
{
	g_bSimCityMove = 1;
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaSC_MoveOff(Lua_State* L)
{
	g_bSimCityMove = 0;
	Lua_PushNumber(L, 1);
	return 1;
}

// SC_Goto(nNpcIdx, nMpsX, nMpsY) -> phat 1 lenh di toi (MPS). Test tay 1 chang.
int LuaSC_Goto(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 3)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int i     = (int)Lua_ValueToNumber(L, 1);
	int nMpsX = (int)Lua_ValueToNumber(L, 2);
	int nMpsY = (int)Lua_ValueToNumber(L, 3);
	if (i <= 0 || i >= MAX_NPC || !Npc[i].m_btSimCityBot || nMpsX <= 0 || nMpsY <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Npc[i].SendCommand(do_walk, nMpsX, nMpsY);
	Lua_PushNumber(L, 1);
	return 1;
}

// SC_LoadPreset(szPresetPath, szPathName) -> nRouteId (>=0) hoac -1.
//   Doc file _preset.txt (2 cot: PathName, node_name "X_Y"), gom node cua szPathName theo thu tu -> 1 lo trinh.
//   szPresetPath vd "\\settings\\simcity\\maps\\thanhthi\\1_phuongtuong_preset.txt" (giai theo cwd server).
int LuaSC_LoadPreset(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2 || !Lua_IsString(L, 1) || !Lua_IsString(L, 2))
	{
		Lua_PushNumber(L, -1);
		return 1;
	}
	char* szFile = (char*)lua_tostring(L, 1);
	char* szPath = (char*)lua_tostring(L, 2);
	if (!szFile || !szFile[0] || !szPath || !szPath[0] || s_routeCount >= SC_MAX_ROUTE)
	{
		Lua_PushNumber(L, -1);
		return 1;
	}

	KTabFile tab;
	if (!tab.Load((LPSTR)szFile))
	{
		Lua_PushNumber(L, -1);
		return 1;
	}

	SC_Route& rt = s_routes[s_routeCount];
	rt.nCount = 0;

	int hgt = tab.GetHeight();
	char szPn[64], szNn[64];
	for (int row = 2; row <= hgt && rt.nCount < SC_MAX_NODE; row++)   // row 1 = header
	{
		tab.GetString(row, 1, (LPSTR)"", szPn, sizeof(szPn));
		if (strcmp(szPn, szPath) != 0)
			continue;
		tab.GetString(row, 2, (LPSTR)"", szNn, sizeof(szNn));
		int gx = 0, gy = 0;
		if (sscanf(szNn, "%d_%d", &gx, &gy) == 2 && gx > 0 && gy > 0)
		{
			rt.wpX[rt.nCount] = gx * 32;   // o luoi -> MPS
			rt.wpY[rt.nCount] = gy * 32;
			rt.nCount++;
		}
	}

	if (rt.nCount < 2)
	{
		Lua_PushNumber(L, -1);
		return 1;
	}
	Lua_PushNumber(L, s_routeCount);
	s_routeCount++;
	return 1;
}

// SC_SetBotRoute(nNpcIdx, nRouteId [, bLoop=1]) -> 1/0. Gan lo trinh cho bot + phat chang dau.
int LuaSC_SetBotRoute(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int i     = (int)Lua_ValueToNumber(L, 1);
	int nRid  = (int)Lua_ValueToNumber(L, 2);
	int bLoop = (Lua_GetTopIndex(L) >= 3) ? (int)Lua_ValueToNumber(L, 3) : 1;
	if (i <= 0 || i >= MAX_NPC || !Npc[i].m_btSimCityBot || nRid < 0 || nRid >= s_routeCount)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int r = sc_Find(i);
	if (r < 0)
	{
		sc_Register(i);
		r = sc_Find(i);
		if (r < 0)
		{
			Lua_PushNumber(L, 0);
			return 1;
		}
	}
	SC_Bot& b = s_bots[r];
	b.nRouteId = nRid;
	b.nLeg     = 0;
	b.nDir     = 1;
	b.bLoop    = bLoop ? 1 : 0;
	b.nStuck   = 0;
	Npc[i].GetMpsPos(&b.nLastX, &b.nLastY);
	sc_IssueLeg(b, s_routes[nRid], i);
	Lua_PushNumber(L, 1);
	return 1;
}

// SC_PatrolBox(nNpcIdx [, nHalfMps]) -> gan lo trinh tuan tra hinh vuong quanh vi tri hien tai (khep vong).
int LuaSC_PatrolBox(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int i = (int)Lua_ValueToNumber(L, 1);
	if (i <= 0 || i >= MAX_NPC || !Npc[i].m_btSimCityBot || s_routeCount >= SC_MAX_ROUTE)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nHalf = (Lua_GetTopIndex(L) >= 2) ? (int)Lua_ValueToNumber(L, 2) : SC_BOX_HALF;
	if (nHalf < 32)   nHalf = 32;
	if (nHalf > 2048) nHalf = 2048;   // kep de tranh waypoint am / tran int khi tinh dist2

	int r = sc_Find(i);
	if (r < 0)
	{
		sc_Register(i);
		r = sc_Find(i);
		if (r < 0)
		{
			Lua_PushNumber(L, 0);
			return 1;
		}
	}

	int cx, cy;
	Npc[i].GetMpsPos(&cx, &cy);

	SC_Route& rt = s_routes[s_routeCount];
	rt.nCount = 4;
	rt.wpX[0] = cx - nHalf; rt.wpY[0] = cy - nHalf;
	rt.wpX[1] = cx + nHalf; rt.wpY[1] = cy - nHalf;
	rt.wpX[2] = cx + nHalf; rt.wpY[2] = cy + nHalf;
	rt.wpX[3] = cx - nHalf; rt.wpY[3] = cy + nHalf;
	int nRid = s_routeCount++;

	SC_Bot& b = s_bots[r];
	b.nRouteId = nRid;
	b.nLeg     = 0;
	b.nDir     = 1;
	b.bLoop    = 1;
	b.nStuck   = 0;
	b.nLastX   = cx;
	b.nLastY   = cy;
	sc_IssueLeg(b, rt, i);

	Lua_PushNumber(L, 1);
	return 1;
}

// SC_Breathe() - goi moi tick tu CoreServerShell::Breathe. Duyet so bot, tien chang khi toi.
void SC_Breathe()
{
	if (!g_bSimCityMove)
		return;

	int w = 0;
	for (int r = 0; r < s_botCount; r++)
	{
		SC_Bot b = s_bots[r];
		int i = b.nNpcIdx;

		// chot bot con song + dung danh tinh; entry hong -> khong copy sang w (bi nen)
		if (i <= 0 || i >= MAX_NPC
			|| !Npc[i].m_btSimCityBot
			|| !Npc[i].m_Index
			|| !Npc[i].IsMatch(b.dwID)
			|| Npc[i].m_RegionIndex < 0)
			continue;

		if (b.nRouteId >= 0 && b.nRouteId < s_routeCount)
		{
			SC_Route& rt = s_routes[b.nRouteId];
			if (rt.nCount >= 2)
			{
				if (b.nLeg < 0)          b.nLeg = 0;
				if (b.nLeg >= rt.nCount) b.nLeg = 0;

				int nx, ny;
				Npc[i].GetMpsPos(&nx, &ny);

				int tx = rt.wpX[b.nLeg];
				int ty = rt.wpY[b.nLeg];
				int dx = nx - tx, dy = ny - ty;

				if (dx * dx + dy * dy <= SC_ARRIVE_MPS * SC_ARRIVE_MPS)
				{
					sc_NextLeg(b, rt);
					sc_IssueLeg(b, rt, i);
				}
				else if (nx == b.nLastX && ny == b.nLastY)
				{
					if (++b.nStuck >= SC_STUCK_LIMIT)   // ket -> bo chang
					{
						sc_NextLeg(b, rt);
						sc_IssueLeg(b, rt, i);
					}
				}
				else
				{
					b.nStuck = 0;
				}
				b.nLastX = nx;
				b.nLastY = ny;
			}
		}

		s_bots[w++] = b;
	}
	s_botCount = w;
}

#endif // _SERVER
