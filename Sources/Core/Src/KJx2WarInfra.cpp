// KJx2WarInfra.cpp - xem KJx2WarInfra.h. DOT E cong thanh JX2 (E4).

// LUU Y: Core build voi PCH "Use" qua KCore.h - moi thu TRUOC dong include nay
// deu bi compiler bo qua, nen KCore.h PHAI dung dau tien.
#include "KCore.h"
#include "KWin32.h"

#ifdef _SERVER

#include "KEngine.h"
#include "KDebug.h"
#include "LuaLib.h"
#include "KSubWorld.h"
#include "KSubWorldSet.h"
#include "KNpc.h"
#include "KNpcSet.h"
// KPlayer.h tu keo du chuoi KItem/KInventory/KItemList theo DUNG thu tu
// (include KItemList.h truc tiep se thieu KInventory - loi C3646 m_Room)
#include "KPlayerSet.h"
#include "KPlayer.h"
#include "KItem.h"
#include "KProtocol.h"
#include "KTongJX2.h"
#include "KJx2WarInfra.h"
#include <map>
#include <vector>
#include <string.h>
#include <stdio.h>

// dinh nghia trong ScriptFuns.cpp (idiom KTongJX2.cpp:32)
extern int GetPlayerIndex(Lua_State* L);
extern int GetSubWorldIndex(Lua_State* L);
extern int LuaSetFightState(Lua_State* L);

static void sWStrCpy(char* szDst, const char* szSrc, int nDstSize)
{
	strncpy(szDst, szSrc ? szSrc : "", nDstSize - 1);
	szDst[nDstSize - 1] = 0;
}

//////////////////////////////////////////////////////////////////////
// npc / map
//////////////////////////////////////////////////////////////////////

// (nNpcIdx, szScript) - ghi ActionScript[80]; KNpc::OnDeath (KNpc.cpp:1517-1519)
// tu chay ExecuteScript2(ActionScript, "OnDeath", m_Index, m_nLastDamageIdx).
// GIOI HAN ghi nhan: JX1 chi kich khi ke giet LA NGUOI (KNpc.cpp:1512).
int LuaSetNpcDeathScript(Lua_State* L)
{
	if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 1) && Lua_IsString(L, 2))
	{
		int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
		if (nNpcIdx > 0 && nNpcIdx < MAX_NPC && Npc[nNpcIdx].m_Index > 0)
		{
			const char* szScript = Lua_ValueToString(L, 2);
			sWStrCpy(Npc[nNpcIdx].ActionScript, szScript, sizeof(Npc[nNpcIdx].ActionScript));
			Npc[nNpcIdx].m_ActionScriptID = szScript[0] ? g_FileName2Id((LPSTR)szScript) : 0;
		}
	}
	return 0;
}

// (nMapId) - xoa moi NPC thuong (khong phai nguoi / khong thuoc nguoi choi)
// tren MOI instance cua map; khuon xoa = LuaDelNpc (ScriptFuns.cpp:5847-5858)
int LuaClearMapNpc(Lua_State* L)
{
	int nCount = 0;
	if (Lua_IsNumber(L, 1))
	{
		int nMapId = (int)Lua_ValueToNumber(L, 1);
		for (int i = 1; i < MAX_NPC; i++)
		{
			if (Npc[i].m_Index <= 0 || Npc[i].IsPlayer() || Npc[i].GetPlayerIdx() > 0)
				continue;
			int w = Npc[i].m_SubWorldIndex;
			if (w < 0 || w >= MAX_SUBWORLD || SubWorld[w].m_SubWorldID != nMapId)
				continue;
			if (Npc[i].m_RegionIndex >= 0)
			{
				SubWorld[w].m_Region[Npc[i].m_RegionIndex].RemoveNpc(i);
				SubWorld[w].m_Region[Npc[i].m_RegionIndex].DecRef(Npc[i].m_MapX, Npc[i].m_MapY, obj_npc);
				NpcSet.Remove(i);
				nCount++;
			}
		}
	}
	Lua_PushNumber(L, nCount);
	return 1;
}

// (nObjId, nMapId, nMpsX, nMpsY) - goc them OBJ vat can co hinh; JX1: chan O THAT
// tren luoi m_Obstacle (ghi truc tiep qua accessor moi KRegion.h - phan bien E4
// CHAN-1: KRegion::SetObstacle chi ghi khi o == 0 nen Clear(0) KHONG BAO GIO go
// duoc; KSubWorld::SetObstacle lai ghi TRAP o tam - CAO-1). Ta tu luu gia tri
// CU cua o de Clear khoi phuc dung (khong pha tuong that cua map).
// DEVIATION ghi nhan: khong ve hinh + client khong duoc bao (S2C_SET_OBSTACLE
// goc bi comment) -> co the giat keo-ve o ria rao (VUA-3, ghi ban giao test).
struct KJx2Obst
{
	int		nSubWorld;
	int		nRegion;
	int		nMapX;
	int		nMapY;
	int		nMapId;
	int		nMpsX;
	int		nMpsY;
	long	nOld;
};
static std::vector<KJx2Obst> s_Obstacles;
#define JX2OBST_MAX 2048

int LuaAddObstacleObj(Lua_State* L)
{
	if (Lua_GetTopIndex(L) >= 4 && Lua_IsNumber(L, 2) && Lua_IsNumber(L, 3) && Lua_IsNumber(L, 4))
	{
		int nMapId = (int)Lua_ValueToNumber(L, 2);
		int nMpsX = (int)Lua_ValueToNumber(L, 3);
		int nMpsY = (int)Lua_ValueToNumber(L, 4);
		for (int w = 0; w < MAX_SUBWORLD; w++)
		{
			if (SubWorld[w].m_SubWorldID != nMapId)
				continue;
			int r = -1, x = -1, y = -1, dx = 0, dy = 0;
			SubWorld[w].Mps2Map(nMpsX, nMpsY, &r, &x, &y, &dx, &dy);
			if (r < 0)
				continue;
			int bHave = 0;
			for (size_t i = 0; i < s_Obstacles.size(); i++)
			{
				if (s_Obstacles[i].nSubWorld == w && s_Obstacles[i].nRegion == r &&
					s_Obstacles[i].nMapX == x && s_Obstacles[i].nMapY == y)
				{
					bHave = 1;
					break;
				}
			}
			if (bHave || s_Obstacles.size() >= JX2OBST_MAX)
				continue;
			KJx2Obst o;
			o.nSubWorld = w;
			o.nRegion = r;
			o.nMapX = x;
			o.nMapY = y;
			o.nMapId = nMapId;
			o.nMpsX = nMpsX;
			o.nMpsY = nMpsY;
			o.nOld = SubWorld[w].m_Region[r].GetObstacleCell(x, y);
			SubWorld[w].m_Region[r].SetObstacleCell(x, y, 1);
			s_Obstacles.push_back(o);
		}
	}
	return 0;
}

int LuaClearObstacleObj(Lua_State* L)
{
	if (Lua_GetTopIndex(L) >= 3 && Lua_IsNumber(L, 1) && Lua_IsNumber(L, 2) && Lua_IsNumber(L, 3))
	{
		int nMapId = (int)Lua_ValueToNumber(L, 1);
		int nMpsX = (int)Lua_ValueToNumber(L, 2);
		int nMpsY = (int)Lua_ValueToNumber(L, 3);
		for (int i = (int)s_Obstacles.size() - 1; i >= 0; i--)
		{
			if (s_Obstacles[i].nMapId != nMapId || s_Obstacles[i].nMpsX != nMpsX ||
				s_Obstacles[i].nMpsY != nMpsY)
				continue;
			KJx2Obst& o = s_Obstacles[i];
			SubWorld[o.nSubWorld].m_Region[o.nRegion].SetObstacleCell(o.nMapX, o.nMapY, o.nOld);
			s_Obstacles.erase(s_Obstacles.begin() + i);
		}
	}
	return 0;
}

int LuaGetLoop(Lua_State* L)
{
	Lua_PushNumber(L, (double)g_SubWorldSet.GetGameTime());
	return 1;
}

int LuaGetNpcSettingIdx(Lua_State* L)
{
	int nIdx = Lua_IsNumber(L, 1) ? (int)Lua_ValueToNumber(L, 1) : 0;
	int nSetting = 0;
	if (nIdx > 0 && nIdx < MAX_NPC && Npc[nIdx].m_Index > 0)
		nSetting = Npc[nIdx].m_NpcSettingIdx;
	Lua_PushNumber(L, nSetting);
	return 1;
}

// npc dang thoai = global Lua "NpcIndex" cua state goi (engine gan khi chay
// script dialog - SCRIPT_NPCINDEX KPlayerDef.h:15; m_nLastNpcIndex chi co
// phia CLIENT nen khong dung duoc o day)
int LuaGetLastDiagNpc(Lua_State* L)
{
	int nNpcIdx = 0;
	Lua_GetGlobal(L, (char*)"NpcIndex");
	if (!lua_isnil(L, Lua_GetTopIndex(L)))
	{
		nNpcIdx = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
		if (nNpcIdx < 0 || nNpcIdx >= MAX_NPC)
			nNpcIdx = 0;
	}
	Lua_PushNumber(L, nNpcIdx);
	return 1;
}

//////////////////////////////////////////////////////////////////////
// nguoi choi
//////////////////////////////////////////////////////////////////////

// JX2 goi SetPKFlag + SetFightState canh nhau cung gia tri (camper LeaveGame,
// OnLeave) -> map ve cung mot duong fight-state cua JX1
int LuaSetPKFlag(Lua_State* L)
{
	return LuaSetFightState(L);
}

// LO GOC (binary khong dang ky): nhan doi de duoi ham chay tron; JX1 chua co
// co "cam nguoi choi tu doi trang thai PK" -> chua cuong che (ghi PHULUC muc 4)
int LuaForbidChangePK(Lua_State* L)
{
	return 0;
}

// LO GOC nhu tren; map 221 la map mission - viec chan Tho Dia Phu de E5 xu
// bang thuoc tinh map neu can
int LuaDisabledUseTownP(Lua_State* L)
{
	return 0;
}

int LuaRestoreOwnFeature(Lua_State* L)
{
	return 0;	// JX1 khong doi feature nguoi choi trong duong citywar
}

// () -> nSubWorldId, nRevId cua diem hoi sinh BEN VUNG (m_sLoginRevivalPos -
// KPlayer.cpp:1329-1330, doc qua 2 getter inline them o KPlayer.h vi member
// private); camper LeaveGame: SetRevPos(GetPlayerRev())
int LuaGetPlayerRev(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].GetLoginRevivalWorld());
		Lua_PushNumber(L, Player[nPlayerIndex].GetLoginRevivalID());
	}
	else
	{
		Lua_PushNumber(L, 0);
		Lua_PushNumber(L, 0);
	}
	return 2;
}

int LuaSearchPlayer(Lua_State* L)
{
	int nFound = 0;
	if (Lua_IsString(L, 1))
	{
		const char* szName = Lua_ValueToString(L, 1);
		if (szName && szName[0])
		{
			for (int i = 1; i <= PlayerSet.GetPlayerMaxNumber(); i++)
			{
				if (Player[i].m_nIndex > 0 && strcmp(Player[i].m_PlayerName, szName) == 0)
				{
					nFound = i;
					break;
				}
			}
		}
	}
	Lua_PushNumber(L, nFound);
	return 1;
}

// () -> SO PHUT tu luc nguoi goi vao bang (phan bien E4 CHAN-3: cong vao chinh
// cua tran la trap.lua:44 `GetJoinTongTime() >= 7200` phut = 5 ngay - stub 0
// chan sach quan chinh quy). Nguon = member KV field 2 (epoch giay vao bang,
// KProtocol.h:2474; ban sao g_TongJX2 dong bo tu relay).
int LuaGetJoinTongTime(Lua_State* L)
{
	int nMinutes = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Player[nPlayerIndex].m_cTong.m_dwTongNameID)
	{
		KTongJX2Tong* pTong = g_TongJX2.FindTong(Player[nPlayerIndex].m_cTong.m_dwTongNameID);
		if (pTong)
		{
			DWORD dwNameID = g_FileName2Id(Player[nPlayerIndex].m_PlayerName);
			std::map<DWORD, KTongJX2Member>::iterator it = pTong->mapMember.find(dwNameID);
			if (it != pTong->mapMember.end())
			{
				DWORD dwJoin = 0;
				std::map<WORD, DWORD>::iterator itF = it->second.mapField.find(2);
				if (itF != it->second.mapField.end())
					dwJoin = itF->second;
				// nhu so hoc goc: (nay - epoch)/60; field thieu (=0) -> so rat lon
				// = coi nhu thanh vien lau nam (giong (now-0)/60 cua goc)
				nMinutes = (int)((time(NULL) - (time_t)dwJoin) / 60);
			}
		}
	}
	Lua_PushNumber(L, nMinutes);
	return 1;
}

// () -> ten bang chu cua bang NGUOI GOI (citywar_function.lua:177 so voi GetName())
int LuaGetTongMaster(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Player[nPlayerIndex].m_cTong.m_dwTongNameID)
	{
		KTongJX2Tong* pTong = g_TongJX2.FindTong(Player[nPlayerIndex].m_cTong.m_dwTongNameID);
		if (pTong)
		{
			std::map<DWORD, KTongJX2Member>::iterator it;
			for (it = pTong->mapMember.begin(); it != pTong->mapMember.end(); ++it)
			{
				if (it->second.btFigure == 0)
				{
					Lua_PushString(L, it->second.szName);
					return 1;
				}
			}
		}
	}
	Lua_PushString(L, (char*)"");
	return 1;
}

int LuaAddTongExp(Lua_State* L)
{
	Lua_PushNumber(L, 0);	// chi citywar_arena (idle) goi - stub du ten
	return 1;
}

//////////////////////////////////////////////////////////////////////
// item theo EVENT id (genre item_task, detail = id - khuon AddEventItem
// ScriptFuns.cpp:3553-3593; bang \settings\item\questkey.txt)
//////////////////////////////////////////////////////////////////////
// dem bang vong lap (phan bien E4 CHAN-2: CountCommonItem so nature TRUC TIEP
// khong co wildcard -1 -> luon 0), cong don StackNum
int LuaGetItemCountEx(Lua_State* L)
{
	int nCount = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Lua_IsNumber(L, 1))
	{
		int nEventId = (int)Lua_ValueToNumber(L, 1);
		KItemList* pList = &Player[nPlayerIndex].m_ItemList;
		for (PlayerItem* p = pList->GetFirstItem(); p; p = pList->GetNextItem())
		{
			if (p->nIdx <= 0 || p->bIsSkill)
				continue;
			if (p->nPlace != pos_equiproom)
				continue;
			if (Item[p->nIdx].GetGenre() == item_task && Item[p->nIdx].GetDetailType() == nEventId)
			{
				int nStack = Item[p->nIdx].GetStackNum();
				nCount = nCount + ((nStack > 0) ? nStack : 1);
			}
		}
	}
	Lua_PushNumber(L, nCount);
	return 1;
}

static int sFindEventItemInBag(int nPlayerIndex, int nEventId)
{
	KItemList* pList = &Player[nPlayerIndex].m_ItemList;
	for (PlayerItem* p = pList->GetFirstItem(); p; p = pList->GetNextItem())
	{
		if (p->nIdx <= 0 || p->bIsSkill)
			continue;
		if (p->nPlace != pos_equiproom)
			continue;
		if (Item[p->nIdx].GetGenre() == item_task && Item[p->nIdx].GetDetailType() == nEventId)
			return p->nIdx;
	}
	return 0;
}

int LuaDelItemEx(Lua_State* L)
{
	int nOk = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Lua_IsNumber(L, 1))
	{
		int nIdx = sFindEventItemInBag(nPlayerIndex, (int)Lua_ValueToNumber(L, 1));
		if (nIdx > 0 && Player[nPlayerIndex].m_ItemList.RemoveItemIdx(nIdx, 1))
			nOk = 1;
	}
	Lua_PushNumber(L, nOk);
	return 1;
}

// DEVIATION ghi nhan: item cua ta khong luu tuoi (phut tu luc phat) -> co item
// tra 0 ("phat truoc 0 ngay, con hieu luc"), khong co tra -1. Lenh bai vi the
// khong tu het han - ghi ban giao.
int LuaGetItemLife(Lua_State* L)
{
	int nLife = -1;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Lua_IsNumber(L, 1))
	{
		if (sFindEventItemInBag(nPlayerIndex, (int)Lua_ValueToNumber(L, 1)) > 0)
			nLife = 0;
	}
	Lua_PushNumber(L, nLife);
	return 1;
}

// (nItemIdx) -> genre, detail, particular (3 gia tri - sure_GiveTiaoZhanLing:227)
// [DA TAU 16/08/2026] mo rong tra 6 gia tri (them level, series, luck):
// seasonnpc.lua (Task_Accept_01/02) nhan 6 nhu binary JX2 goc; truoc day
// chi tra 3 -> Level/nSeries = nil -> tl_checktask LUON truot (nil == so).
// Caller cu nhan 3 gia tri khong anh huong (Lua bo phan thua).
int LuaGetItemProp(Lua_State* L)
{
	int g = 0, d = 0, p = 0, lv = 0, se = 0, lk = 0;
	if (Lua_IsNumber(L, 1))
	{
		int nIdx = (int)Lua_ValueToNumber(L, 1);
		if (nIdx > 0 && nIdx < MAX_ITEM)
		{
			g = Item[nIdx].GetGenre();
			d = Item[nIdx].GetDetailType();
			p = Item[nIdx].GetParticular();
			lv = Item[nIdx].GetLevel();
			se = Item[nIdx].GetSeries();
			lk = Item[nIdx].m_GeneratorParam.nLuck;
		}
	}
	Lua_PushNumber(L, g);
	Lua_PushNumber(L, d);
	Lua_PushNumber(L, p);
	Lua_PushNumber(L, lv);
	Lua_PushNumber(L, se);
	Lua_PushNumber(L, lk);
	return 6;
}

//////////////////////////////////////////////////////////////////////
// Giao vat pham - map len GiveBox (pos_affairitem) co san cua JX1.
// LuaGiveItemUI mo hop nhu LuaOpenGiveBox 4 doi (ScriptFuns.cpp:2418-2457);
// khi client xac nhan, KProtocolProcess::UiCommandScript case 1 goi
// KJx2WarInfra_GiveBoxCollect: gom item o pos_affairitem vao give-list
// (stack N don vi = N muc) roi callback duoc goi dang fn(nCount).
//////////////////////////////////////////////////////////////////////
#define JX2GIVE_MAX 512
struct KJx2GiveSession
{
	std::vector<int> vItems;
};
static std::map<int, KJx2GiveSession>	s_GiveSessions;	// playerIdx -> phien
static std::map<int, DWORD>				s_GivePending;	// playerIdx -> script id da mo
                                                        // (phan bien E4 CAO-2: phai KHOP
                                                        // m_dwGiveBoxId luc xac nhan, khong
                                                        // thi phien JX1 cu bi nuot)

int LuaGiveItemUI(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || Player[nPlayerIndex].m_nIndex <= 0)
		return 0;
	if (Lua_GetTopIndex(L) < 3 || !Lua_IsString(L, 1) || !Lua_IsString(L, 2) || !Lua_IsString(L, 3))
		return 0;
	// callback = ham trong CHINH script dang thoai (ActionScript cua npc dialog
	// da duoc engine gan vao Npc nguoi choi - khuon OpenGiveBox 4 doi :2445)
	Player[nPlayerIndex].m_dwGiveBoxId = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;
	S2C_GIVE_BOX NetCommand;
	NetCommand.ProtocolType = s2c_openaffairbox;
	NetCommand.nType = 1;
	sWStrCpy(NetCommand.Value, Lua_ValueToString(L, 1), sizeof(NetCommand.Value));
	sWStrCpy(NetCommand.Value1, Lua_ValueToString(L, 2), sizeof(NetCommand.Value1));
	sWStrCpy(NetCommand.Value2, Lua_ValueToString(L, 3), sizeof(NetCommand.Value2));
	strncpy(Player[nPlayerIndex].m_szTaskExcuteFun, Lua_ValueToString(L, 3),
		sizeof(Player[nPlayerIndex].m_szTaskExcuteFun) - 1);
	Player[nPlayerIndex].m_szTaskExcuteFun[sizeof(Player[nPlayerIndex].m_szTaskExcuteFun) - 1] = 0;
	s_GivePending[nPlayerIndex] = Player[nPlayerIndex].m_dwGiveBoxId;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(S2C_GIVE_BOX));
	return 0;
}

// goi tu KProtocolProcess::UiCommandScript case 1 TRUOC khi chay callback.
// -1 = khong phai phien JX2 (giu nguyen duong OpenGiveBox cu, callback "" 0 doi)
int KJx2WarInfra_GiveBoxCollect(int nPlayerIdx)
{
	std::map<int, DWORD>::iterator itP = s_GivePending.find(nPlayerIdx);
	if (itP == s_GivePending.end())
		return -1;
	// phien treo tu truoc (nguoi choi dong hop khong xac nhan roi mo hop
	// GiveBox JX1 khac): id script khong khop -> tra phien ve duong JX1 cu
	if (nPlayerIdx > 0 && itP->second != Player[nPlayerIdx].m_dwGiveBoxId)
	{
		s_GivePending.erase(itP);
		return -1;
	}
	s_GivePending.erase(itP);
	KJx2GiveSession& s = s_GiveSessions[nPlayerIdx];
	s.vItems.clear();
	if (nPlayerIdx <= 0)
		return 0;
	KItemList* pList = &Player[nPlayerIdx].m_ItemList;
	for (PlayerItem* p = pList->GetFirstItem(); p; p = pList->GetNextItem())
	{
		if (p->nIdx <= 0 || p->bIsSkill)
			continue;
		if (p->nPlace != pos_affairitem)
			continue;
		int nUnits = Item[p->nIdx].GetStackNum();
		if (nUnits < 1)
			nUnits = 1;
		for (int u = 0; u < nUnits && (int)s.vItems.size() < JX2GIVE_MAX; u++)
			s.vItems.push_back(p->nIdx);
	}
	return (int)s.vItems.size();
}

// (i 1-based) -> item idx / 0
int LuaGetGiveItemUnit(Lua_State* L)
{
	int nIdx = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Lua_IsNumber(L, 1))
	{
		int i = (int)Lua_ValueToNumber(L, 1);
		std::map<int, KJx2GiveSession>::iterator it = s_GiveSessions.find(nPlayerIndex);
		if (it != s_GiveSessions.end() && i >= 1 && (size_t)i <= it->second.vItems.size())
			nIdx = it->second.vItems[i - 1];
	}
	Lua_PushNumber(L, nIdx);
	return 1;
}

// (nItemIdx) - tru 1 DON VI (stack-- / xoa khi het) - khop khai trien stack
// cua give-list nen vong for cua sure_GiveTiaoZhanLing tru dung tung don vi
int LuaRemoveItemByIndex(Lua_State* L)
{
	int nOk = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Lua_IsNumber(L, 1))
	{
		int nIdx = (int)Lua_ValueToNumber(L, 1);
		if (nIdx > 0 && nIdx < MAX_ITEM &&
			Player[nPlayerIndex].m_ItemList.RemoveItemIdx(nIdx, 1))
			nOk = 1;
	}
	Lua_PushNumber(L, nOk);
	return 1;
}

//////////////////////////////////////////////////////////////////////
// Bo dung chuoi engine. LO GOC: binary chi co AppendString, khong co
// PushString/PopString - script van goi ca ba (infocenter:133-137) -> lam THAT
// ca ba + ReplaceString (thao tac tren dem: task_string.lua:70-85).
// Don luong game thread; moi Push...Pop khep kin trong mot ham Lua.
//////////////////////////////////////////////////////////////////////
#define JX2SB_MAX 16384
static char s_szStrBuf[JX2SB_MAX] = "";

int LuaPushString(Lua_State* L)
{
	if (Lua_IsString(L, 1))
		sWStrCpy(s_szStrBuf, Lua_ValueToString(L, 1), sizeof(s_szStrBuf));
	else
		s_szStrBuf[0] = 0;
	return 0;
}

int LuaAppendString(Lua_State* L)
{
	if (Lua_IsString(L, 1))
	{
		int nCur = (int)strlen(s_szStrBuf);
		int nRoom = JX2SB_MAX - 1 - nCur;
		if (nRoom > 0)
		{
			strncpy(s_szStrBuf + nCur, Lua_ValueToString(L, 1), nRoom);
			s_szStrBuf[nCur + nRoom] = 0;
			s_szStrBuf[JX2SB_MAX - 1] = 0;
		}
	}
	return 0;
}

// (szFrom, szTo) - thay MOI lan xuat hien; bao ve tran dem + szFrom rong
int LuaReplaceString(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2 || !Lua_IsString(L, 1) || !Lua_IsString(L, 2))
		return 0;
	const char* szFrom = Lua_ValueToString(L, 1);
	const char* szTo = Lua_ValueToString(L, 2);
	int nFromLen = (int)strlen(szFrom);
	if (nFromLen <= 0)
		return 0;
	int nToLen = (int)strlen(szTo);
	static char szTmp[JX2SB_MAX];
	int nOut = 0;
	const char* p = s_szStrBuf;
	while (*p && nOut < JX2SB_MAX - 1)
	{
		if (strncmp(p, szFrom, nFromLen) == 0)
		{
			if (nOut + nToLen >= JX2SB_MAX - 1)
				break;
			memcpy(szTmp + nOut, szTo, nToLen);
			nOut += nToLen;
			p += nFromLen;
		}
		else
			szTmp[nOut++] = *p++;
	}
	szTmp[nOut] = 0;
	memcpy(s_szStrBuf, szTmp, nOut + 1);
	return 0;
}

int LuaPopString(Lua_State* L)
{
	Lua_PushString(L, s_szStrBuf);
	s_szStrBuf[0] = 0;
	return 1;
}

//////////////////////////////////////////////////////////////////////
// Khac
//////////////////////////////////////////////////////////////////////
static std::map<int, int>	s_SiegeTarget;		// toolNpcIdx -> doorNpcIdx
struct KJx2Mangonel { int nX, nY, nParam; };
static std::map<int, KJx2Mangonel> s_Mangonel;	// toolNpcIdx -> tham so

// (nToolNpcIdx, nDoorNpcIdx) - gongchengche.lua:42. Luu de dung khi lam AI xe;
// hien xe la NPC thuong (nguoi choi tu danh cong/tru) - ghi nhan trong PHULUC.
int LuaSetSiegeVoitureParam(Lua_State* L)
{
	if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 1) && Lua_IsNumber(L, 2))
		s_SiegeTarget[(int)Lua_ValueToNumber(L, 1)] = (int)Lua_ValueToNumber(L, 2);
	return 0;
}

// (nToolNpcIdx, nX, nY, nParam) - toushiche.lua:45
int LuaSetMangonelParam(Lua_State* L)
{
	if (Lua_GetTopIndex(L) >= 4 && Lua_IsNumber(L, 1) && Lua_IsNumber(L, 2) &&
		Lua_IsNumber(L, 3) && Lua_IsNumber(L, 4))
	{
		KJx2Mangonel m;
		m.nX = (int)Lua_ValueToNumber(L, 2);
		m.nY = (int)Lua_ValueToNumber(L, 3);
		m.nParam = (int)Lua_ValueToNumber(L, 4);
		s_Mangonel[(int)Lua_ValueToNumber(L, 1)] = m;
	}
	return 0;
}

int LuaNW_GetSealInfo(Lua_State* L)
{
	Lua_PushString(L, (char*)"");	// quoc chien chua port (citybulletin noi chuoi)
	return 1;
}

int LuaPARTNER_GetCurPartner(Lua_State* L)
{
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaPARTNER_GetSettingIdx(Lua_State* L)
{
	Lua_PushNumber(L, 0);
	return 1;
}

//////////////////////////////////////////////////////////////////////
// Kho CHUOI mission rieng (phan bien E4 CHAN-5): JX1 chi co MOT mang
// m_MissionValue dung chung cho ca so lan chuoi -> SetMissionV(1,..) de mat
// SetMissionS(1, ten bang thu) cua mission.lua:87-88, trap.lua:43 nhan nham.
// Ta tach: SetMissionS/GetMissionS di kho nay (dang ky DE LEN ban cu - muc
// sau trong GameScriptFuns thang); SetMissionV/GetMissionV giu m_MissionArray.
// RAM thuan - mission khong song qua restart (InitMission tu xoa 1..5).
//////////////////////////////////////////////////////////////////////
static std::map<long, std::string> s_MissionStr;	// khoa = subworldIdx*4096 + id

int LuaJx2SetMissionString(Lua_State* L)
{
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0 || Lua_GetTopIndex(L) < 2 || !Lua_IsNumber(L, 1))
		return 0;
	int nId = (int)Lua_ValueToNumber(L, 1);
	if (nId < 0 || nId >= 4096)
		return 0;
	const char* szVal = Lua_IsString(L, 2) ? Lua_ValueToString(L, 2) : "";
	long nKey = (long)nSubWorldIndex * 4096 + nId;
	if (szVal && szVal[0])
		s_MissionStr[nKey] = szVal;
	else
		s_MissionStr.erase(nKey);
	return 0;
}

int LuaJx2GetMissionString(Lua_State* L)
{
	const char* szVal = "";
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0 && Lua_IsNumber(L, 1))
	{
		int nId = (int)Lua_ValueToNumber(L, 1);
		if (nId >= 0 && nId < 4096)
		{
			std::map<long, std::string>::iterator it =
				s_MissionStr.find((long)nSubWorldIndex * 4096 + nId);
			if (it != s_MissionStr.end())
				szVal = it->second.c_str();
		}
	}
	Lua_PushString(L, (char*)szVal);
	return 1;
}

#endif // _SERVER
