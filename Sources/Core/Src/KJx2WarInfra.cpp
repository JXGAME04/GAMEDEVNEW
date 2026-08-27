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
#include "KObjSet.h"	// [TONG 21/08] ObjSet cho LuaClearMapObj
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
extern int GetPlayerIndex(Lua_State* L);
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
	// [WLLS 21/08] engine ta KHONG bom global NpcIndex vao state thoai NPC
	// (KPlayer::ExecuteScript chi bom PlayerIndex/PlayerID/SubWorld) nen
	// truoc day luon tra 0 -> GetNpcName(0)=nil -> concat gay (officer.lua
	// wlls_npcname - trace ScriptError 20/08 23:33). Fallback: idx NPC vua
	// bam, luu tai KPlayer::DialogNpc.
	if (nNpcIdx == 0)
	{
		int nPlayerIdx = GetPlayerIndex(L);
		if (nPlayerIdx > 0)
			nNpcIdx = Player[nPlayerIdx].m_nWllsLastDiagNpc;
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
	// [JX2COMPAT 22/08] Linux 0x0810F610 -> KPlayerPK::SetNormalPKState(n, giu khoa): doi co PK
	// thuong (0/1), KHONG dung fight-state (ban cu map sang SetFightState -> PK mode khong doi,
	// quan he none, danh khong len damage trong loi dai). Script JX2 van goi SetFightState rieng.
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Lua_IsNumber(L, 1))
	{
		BYTE btFlag = ((int)Lua_ValueToNumber(L, 1) != 0) ? 1 : 0;
		Player[nPlayerIndex].m_cPK.SetNormalPKState(btFlag, Player[nPlayerIndex].m_cPK.GetLockPKState());
	}
	return 0;
}

// LO GOC (binary khong dang ky): nhan doi de duoi ham chay tron; JX1 chua co
// co "cam nguoi choi tu doi trang thai PK" -> chua cuong che (ghi PHULUC muc 4)
int LuaForbidChangePK(Lua_State* L)
{
	// [JX2COMPAT 22/08] Linux 0x0810F590: m_bLockPK = (n == 1) - cam nguoi choi tu doi co PK
	// (client ton trong khoa; server chan them o c2sPKApplyNormalFlag).
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Lua_IsNumber(L, 1))
	{
		BOOL bLock = ((int)Lua_ValueToNumber(L, 1) == 1) ? TRUE : FALSE;
		Player[nPlayerIndex].m_cPK.SetNormalPKState((BYTE)Player[nPlayerIndex].m_cPK.GetNormalPKState(), bLock);
	}
	return 0;
}

// [3HD 25/08] het la stub: chuyen sang ban that o cuoi file (co theo
// PlayerIndex + nho subworld luc bat - LuaHD3_DisabledUseTownP_Real).
// Truoc day stub return 0 -> 7 loi goi cua PLD / Vuot Ai vo tac dung.
int LuaHD3_DisabledUseTownP_Real(Lua_State* L);
int LuaDisabledUseTownP(Lua_State* L)
{
	return LuaHD3_DisabledUseTownP_Real(L);
}

extern int LuaReSetMask(Lua_State* L);		// ScriptFuns.cpp - khoi phuc mat na theo do dang deo
int LuaRestoreOwnFeature(Lua_State* L)
{
	// [TONG 21/08] Linux KNpc::RestoreFeature (0x0807ACB0) tra ngoai hinh ve binh thuong;
	// JX1 ChangeOwnFeature dung co che mat na (m_MaskType) -> khoi phuc = ReSetMask.
	return LuaReSetMask(L);
}

// ---------------------------------------------------------------------------
// [TONG 21/08] 3 ham theo MAP ID cua ban Linux (missions\tong). Goc:
//  ClearMapObj        0x08102B40 -> KSubWorld::ClearObj: xoa MOI KObj moi region
//  ClearMapNpcWithName 0x08102DF0 -> KSubWorld::ClearNpcWithName (strcmp ten)
//  GetMapNpcWithName  0x08102BC0 -> KSubWorld::GetNpcListByName (so bam ten)
// Ca 3 deu SearchWorld(map id) -> INSTANCE DAU TIEN (giong goc).
// ---------------------------------------------------------------------------
// [PORT5 23/08] ClearMapTrap(nMapId) - Linux 0x08102AC0 -> KSubWorld::ClearTrap 0x080EFDF0:
// xoa SACH trap MOI region cua map (ke ca trap map-data - Linux cung vay). tongwar
// InitMission/EndMission goi cho map chien truong rieng 605-607, KHONG dung cho map thanh.
int LuaClearMapTrap(Lua_State* L)
{
	if (Lua_GetTopIndex(L) != 1 || !Lua_IsNumber(L, 1))
		return 0;
	int w = g_SubWorldSet.SearchWorld((DWORD)Lua_ValueToNumber(L, 1));
	if (w < 0 || w >= MAX_SUBWORLD)
		return 0;
	KSubWorld* pWorld = &SubWorld[w];
	for (int r = 0; r < pWorld->m_nTotalRegion; r++)
		pWorld->m_Region[r].ClearAllTraps();
	return 0;
}

// [PORT5 23/08] GetItemStackCount (Linux 0x080FD250) / SetItemStackCount (0x0810D9A0) -
// tongcastle guard.lua tru dan bua trieu hoi trong chong (item 3822 stack).
int LuaGetItemStackCount(Lua_State* L)
{
	if (Lua_GetTopIndex(L) != 1 || !Lua_IsNumber(L, 1))
	{
		Lua_PushNumber(L, -1);
		return 1;
	}
	int n = (int)Lua_ValueToNumber(L, 1);
	if (n <= 0 || n >= MAX_ITEM)
	{
		Lua_PushNumber(L, -1);
		return 1;
	}
	int c = Item[n].GetStackNum();
	if (c < 1)
		c = 1;
	int nMax = Item[n].GetMaxStackNum();
	if (nMax >= 1 && c > nMax)
		c = nMax;
	Lua_PushNumber(L, c);
	return 1;
}

int LuaSetItemStackCount(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (Lua_GetTopIndex(L) != 2 || nPlayerIndex <= 0 || !Lua_IsNumber(L, 1) || !Lua_IsNumber(L, 2))
	{
		Lua_PushNumber(L, -1);
		return 1;
	}
	int n = (int)Lua_ValueToNumber(L, 1);
	int c = (int)Lua_ValueToNumber(L, 2);
	if (n <= 0 || n >= MAX_ITEM || c < 1)
	{
		Lua_PushNumber(L, -1);
		return 1;
	}
	int nMax = Item[n].GetMaxStackNum();
	if (nMax >= 1 && c > nMax)
	{
		Lua_PushNumber(L, -1);
		return 1;
	}
	Item[n].SetStackNum(c);
	Player[nPlayerIndex].m_ItemList.SyncItem(n);
	Lua_PushNumber(L, 1);
	return 1;
}

// [PORT5 23/08] GetNpcAroundNpcList(nNpcIndex, nDist[, nMask]) - Linux 0x08104A20: nhu
// GetAroundNpcList nhung tam la NPC bat ky (tongcastle treedeath quet quanh cay). Mask bo
// qua (khong loc quan he - script tu loc theo NpcParam). Tra (tbList, nCount).
int LuaGetNpcAroundNpcList(Lua_State* L)
{
	int nCount = 0;
	int nTop = Lua_GetTopIndex(L);
	Lua_NewTable(L);
	if (nTop < 2 || !Lua_IsNumber(L, 1) || !Lua_IsNumber(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 2;
	}
	int nMe = (int)Lua_ValueToNumber(L, 1);
	int nDist = (int)Lua_ValueToNumber(L, 2);
	if (nMe <= 0 || nMe >= MAX_NPC || Npc[nMe].m_Index <= 0 || nDist <= 0 || Npc[nMe].m_SubWorldIndex < 0)
	{
		Lua_PushNumber(L, 0);
		return 2;
	}
	KSubWorld* pWorld = &SubWorld[Npc[nMe].m_SubWorldIndex];
	// [PORT5 23/08 phan bien F6] toa do toan cuc (xem LuaGetAroundNpcList)
	if (Npc[nMe].m_RegionIndex < 0)
	{
		Lua_PushNumber(L, 0);
		return 2;
	}
	int nMX = LOWORD(pWorld->m_Region[Npc[nMe].m_RegionIndex].m_RegionID) * pWorld->m_nRegionWidth + Npc[nMe].m_MapX;
	int nMY = HIWORD(pWorld->m_Region[Npc[nMe].m_RegionIndex].m_RegionID) * pWorld->m_nRegionHeight + Npc[nMe].m_MapY;
	for (int r = 0; r < pWorld->m_nTotalRegion; r++)
	{
		KRegion* pRegion = &pWorld->m_Region[r];
		KIndexNode* pNode = (KIndexNode*)pRegion->m_NpcList.GetHead();
		while (pNode)
		{
			int i = pNode->m_nIndex;
			pNode = (KIndexNode*)pNode->GetNext();
			if (i <= 0 || i >= MAX_NPC || Npc[i].m_Index <= 0 || i == nMe)
				continue;
			if (Npc[i].m_Doing == do_death || Npc[i].m_Doing == do_revive)
				continue;
			int dx = LOWORD(pRegion->m_RegionID) * pWorld->m_nRegionWidth + Npc[i].m_MapX - nMX;
			int dy = HIWORD(pRegion->m_RegionID) * pWorld->m_nRegionHeight + Npc[i].m_MapY - nMY;
			if (dx * dx + dy * dy > nDist * nDist)
				continue;
			Lua_PushNumber(L, i);
			Lua_RawSetI(L, -2, ++nCount);
		}
	}
	Lua_PushNumber(L, nCount);
	return 2;
}

int LuaClearMapObj(Lua_State* L)
{
	if (Lua_GetTopIndex(L) != 1 || !Lua_IsNumber(L, 1))
		return 0;
	int nMapId = (int)Lua_ValueToNumber(L, 1);
	int w = g_SubWorldSet.SearchWorld((DWORD)nMapId);
	if (w < 0 || w >= MAX_SUBWORLD)
		return 0;
	KSubWorld* pWorld = &SubWorld[w];
	for (int r = 0; r < pWorld->m_nTotalRegion; r++)
	{
		KRegion* pRegion = &pWorld->m_Region[r];
		KIndexNode* pNode = (KIndexNode*)pRegion->m_ObjList.GetHead();
		while (pNode)
		{
			KIndexNode* pNext = (KIndexNode*)pNode->GetNext();	// lay next TRUOC khi xoa
			int nIdx = pNode->m_nIndex;
			if (nIdx > 0 && nIdx < MAX_OBJECT)
			{
				// KHONG dung KObj::Remove(): phia server no Send(GWM_OBJ_DEL) roi KSubWorld
				// xoa TRE lan nua (KSubWorld.cpp GWM_OBJ_DEL) -> double free. Khuon KPlayer.cpp.
				Object[nIdx].SyncRemove(FALSE);
				pRegion->RemoveObj(nIdx);
				Object[nIdx].m_nRegionIdx = -1;
				ObjSet.Remove(nIdx);
			}
			pNode = pNext;
		}
	}
	return 0;
}

int LuaClearMapNpcWithName(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2 || !Lua_IsNumber(L, 1) || !Lua_IsString(L, 2))
		return 0;
	int nMapId = (int)Lua_ValueToNumber(L, 1);
	const char* szName = (const char*)Lua_ValueToString(L, 2);
	if (!szName)
		return 0;
	int w = g_SubWorldSet.SearchWorld((DWORD)nMapId);
	if (w < 0 || w >= MAX_SUBWORLD)
		return 0;
	KSubWorld* pWorld = &SubWorld[w];
	// (a) NPC dang o trong region - khuon LuaClearMapNpc o tren
	for (int r = 0; r < pWorld->m_nTotalRegion; r++)
	{
		KRegion* pRegion = &pWorld->m_Region[r];
		KIndexNode* pNode = (KIndexNode*)pRegion->m_NpcList.GetHead();
		while (pNode)
		{
			KIndexNode* pNext = (KIndexNode*)pNode->GetNext();
			int i = pNode->m_nIndex;
			if (i > 0 && i < MAX_NPC && Npc[i].m_Index > 0 && !Npc[i].IsPlayer() && Npc[i].GetPlayerIdx() <= 0
				&& strcmp(Npc[i].Name, szName) == 0)
			{
				pRegion->RemoveNpc(i);
				pRegion->DecRef(Npc[i].m_MapX, Npc[i].m_MapY, obj_npc);
				NpcSet.Remove(i);
			}
			pNode = pNext;
		}
	}
	// (b) quai dang chet cho hoi sinh (m_NoneRegionNpcList) - goc cung xoa de khong hoi sinh lai
	{
		KIndexNode* pNode = (KIndexNode*)pWorld->m_NoneRegionNpcList.GetHead();
		while (pNode)
		{
			KIndexNode* pNext = (KIndexNode*)pNode->GetNext();
			int i = pNode->m_nIndex;
			if (i > 0 && i < MAX_NPC && Npc[i].m_Index > 0 && !Npc[i].IsPlayer() && Npc[i].GetPlayerIdx() <= 0
				&& strcmp(Npc[i].Name, szName) == 0)
			{
				Npc[i].m_Node.Remove();		// KSubWorld.cpp: cap Remove() + Release() khi go khoi list nay
				Npc[i].m_Node.Release();
				NpcSet.Remove(i);
			}
			pNode = pNext;
		}
	}
	return 0;
}

int LuaGetMapNpcWithName(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;
	int nMapId = (int)Lua_ValueToNumber(L, 1);
	const char* szName = (const char*)Lua_ValueToString(L, 2);
	if (!szName || !szName[0])
		return 0;
	int w = g_SubWorldSet.SearchWorld((DWORD)nMapId);
	if (w < 0 || w >= MAX_SUBWORLD)
	{
		Lua_PushNil(L);	// goc tra rac (chuoi tren dinh stack); ta tra nil cho sach
		return 1;
	}
	DWORD dwNameId = g_FileName2Id((char*)szName);	// goc so bam ten (KRegion::SearchNpcID cung vay)
	KSubWorld* pWorld = &SubWorld[w];
	int nCount = 0;
	for (int r = 0; r < pWorld->m_nTotalRegion; r++)
	{
		KRegion* pRegion = &pWorld->m_Region[r];
		KIndexNode* pNode = (KIndexNode*)pRegion->m_NpcList.GetHead();
		while (pNode)
		{
			int i = pNode->m_nIndex;
			pNode = (KIndexNode*)pNode->GetNext();
			if (i <= 0 || i >= MAX_NPC || Npc[i].m_Index <= 0)
				continue;
			if (Npc[i].IsPlayer() || Npc[i].GetPlayerIdx() > 0)
				continue;
			if (Npc[i].m_Doing == do_death || Npc[i].m_Doing == do_revive)
				continue;
			if (g_FileName2Id(Npc[i].Name) != dwNameId)
				continue;
			if (nCount == 0)
				Lua_NewTable(L);
			Lua_PushNumber(L, i);
			Lua_RawSetI(L, -2, ++nCount);
		}
	}
	if (nCount == 0)
		return 0;	// goc: danh sach rong -> return 0 (nil), KHONG tao bang
	return 1;
}

// GetAroundNpcList(nDist[, nKind]) -> tbList, nCount : port Tin Su (21/08).
// Linux (item\xinshirenwu\che*fu.lua): "20 = khoang 1 man hinh", tra bang chi so NPC
// quanh nguoi choi de script tu loc GetNpcSettingIdx. nDist tinh bang O (32px, cung
// don vi GetWorldPos/m_MapX). Tham so 2 (Linux luon 8) KHONG loc gi o ta - quai
// muc tieu (849 Bao Kho Thu Ho Gia) co Kind = 0 trong npcs.txt nen 8 khong the la
// bo loc Kind; bo qua cho an toan. Bo qua nguoi choi + NPC dang chet.
int LuaGetAroundNpcList(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nCount = 0;
	Lua_NewTable(L);
	if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 2 || !Lua_IsNumber(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 2;
	}
	int nDist = (int)Lua_ValueToNumber(L, 1);
	int nMe = Player[nPlayerIndex].m_nIndex;
	if (nDist <= 0 || nMe <= 0 || nMe >= MAX_NPC || Npc[nMe].m_SubWorldIndex < 0)
	{
		Lua_PushNumber(L, 0);
		return 2;
	}
	KSubWorld* pWorld = &SubWorld[Npc[nMe].m_SubWorldIndex];
	// [PORT5 23/08 phan bien F6] m_MapX/m_MapY la toa do CUC BO region (0..15/0..31) - so giua
	// cac region phai quy ve toa do toan cuc (khuon KNpc::GetMapDisX/Y KNpc.cpp:5353):
	// G = LOWORD/HIWORD(m_RegionID) * m_nRegionWidth/Height + m_MapX/Y.
	if (Npc[nMe].m_RegionIndex < 0)
	{
		Lua_PushNumber(L, 0);
		return 2;
	}
	int nMX = LOWORD(pWorld->m_Region[Npc[nMe].m_RegionIndex].m_RegionID) * pWorld->m_nRegionWidth + Npc[nMe].m_MapX;
	int nMY = HIWORD(pWorld->m_Region[Npc[nMe].m_RegionIndex].m_RegionID) * pWorld->m_nRegionHeight + Npc[nMe].m_MapY;
	for (int r = 0; r < pWorld->m_nTotalRegion; r++)
	{
		KRegion* pRegion = &pWorld->m_Region[r];
		KIndexNode* pNode = (KIndexNode*)pRegion->m_NpcList.GetHead();
		while (pNode)
		{
			int i = pNode->m_nIndex;
			pNode = (KIndexNode*)pNode->GetNext();
			if (i <= 0 || i >= MAX_NPC || Npc[i].m_Index <= 0 || i == nMe)
				continue;
			if (Npc[i].IsPlayer() || Npc[i].GetPlayerIdx() > 0)
				continue;
			if (Npc[i].m_Doing == do_death || Npc[i].m_Doing == do_revive)
				continue;
			int dx = LOWORD(pRegion->m_RegionID) * pWorld->m_nRegionWidth + Npc[i].m_MapX - nMX;
			int dy = HIWORD(pRegion->m_RegionID) * pWorld->m_nRegionHeight + Npc[i].m_MapY - nMY;
			if (dx * dx + dy * dy > nDist * nDist)
				continue;
			Lua_PushNumber(L, i);
			Lua_RawSetI(L, -2, ++nCount);
		}
	}
	Lua_PushNumber(L, nCount);
	return 2;
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
// [LOREN 26/08] Them doi THU HAI = so luong can tru (mac dinh 1).
// He lo ren tinh gia tri nguyen lieu theo CA CHONG nen khi an nguyen lieu
// cung phai an ca chong; compound_header.lua goi
//     RemoveItemByIndex( idx, GetItemStackCount(idx) )
// Goi mot doi van giu nguyen hanh vi cu (tru 1 don vi).
int LuaRemoveItemByIndex(Lua_State* L)
{
	int nOk = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Lua_IsNumber(L, 1))
	{
		int nIdx = (int)Lua_ValueToNumber(L, 1);
		int nNum = 1;
		if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2))
		{
			nNum = (int)Lua_ValueToNumber(L, 2);
			if (nNum < 1)
				nNum = 1;
		}
		if (nIdx > 0 && nIdx < MAX_ITEM &&
			Player[nPlayerIndex].m_ItemList.RemoveItemIdx(nIdx, nNum))
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

// ============================================================================
// == [3HD 25/08/2026] PORT 3 HOAT DONG BAN LINUX =============================
// ==   (1) San boss Sat Thu  - script\task\tollgate\killer\
// ==   (2) Phong Lang Do     - script\missions\fengling_ferry\
// ==   (3) Vuot Ai           - script\missions\challengeoftime\
// == Cac ham engine Linux (jx_linux_y) chua co tren JX1. Chu ky lay tu
// == dich nguoc ELF + chinh script goc; doi chieu day du:
// == D:\GAMEDEVNEW\ReverseTools\port_3hd\15_bosung_soat_api.md (muc 6).
// == Dang ky o ScriptFuns.cpp (khoi "[3HD 25/08]"). Toan bo server-only.
// ============================================================================

#include "KItemSet.h"
#include "KSubWorldSet.h"
#include "KSG_StringProcess.h"	// KSG_GetCurSec
#include "KSortScript.h"		// g_FileName2Id / g_GetScriptNameByState
#include <time.h>

extern KItemSet ItemSet;
extern KObjSet ObjSet;

// log 1 lan cho moi ham stub de biet duong nao dang duoc goi that
static void sHD3_LogOnce(const char* szTag)
{
	static std::map<std::string, int> s_mapOnce;
	if (s_mapOnce.find(szTag) == s_mapOnce.end())
	{
		s_mapOnce[szTag] = 1;
		g_DebugLog((LPSTR)"[3HD] ham stub duoc goi lan dau: %.60s", szTag);
	}
}

// ---------------------------------------------------------------------------
// Tm2Time(nYear, nMonth, nDay, nHour, nMin, nSec) -> so giay Unix.
// Linux 0x08103AC0 - ham nguoc cua Time2Tm (JX1 da co :14891). Thieu tham so
// -> dung 0 (mktime tu chinh). functionlib.lua:364,380 dung de tinh lich.
int LuaHD3_Tm2Time(Lua_State* L)
{
	int nTop = Lua_GetTopIndex(L);
	struct tm t;
	memset(&t, 0, sizeof(t));
	int nVal[6] = { 1970, 1, 1, 0, 0, 0 };
	for (int i = 0; i < 6 && i < nTop; i++)
		nVal[i] = (int)Lua_ValueToNumber(L, i + 1);
	t.tm_year = nVal[0] - 1900;
	t.tm_mon  = nVal[1] - 1;
	t.tm_mday = nVal[2];
	t.tm_hour = nVal[3];
	t.tm_min  = nVal[4];
	t.tm_sec  = nVal[5];
	t.tm_isdst = -1;
	time_t nTime = mktime(&t);
	if (nTime == (time_t)-1)
		nTime = 0;
	Lua_PushNumber(L, (double)nTime);
	return 1;
}

// FormatTime2Date(nUnix) -> so YYYYMMDD. Linux 0x081022B0 ("%04d%02d%02d").
int LuaHD3_FormatTime2Date(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1 || !Lua_IsNumber(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	time_t nTime = (time_t)(double)Lua_ValueToNumber(L, 1);
	struct tm* pTm = localtime(&nTime);
	if (!pTm)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, (double)((pTm->tm_year + 1900) * 10000 + (pTm->tm_mon + 1) * 100 + pTm->tm_mday));
	return 1;
}

// AddStatData(szTen [, nSoLuong]) -> 0 gia tri. Linux 0x080FF550 cong vao bo
// dem thong ke phia relay (StatGameData). JX1 khong co kenh do -> ghi dong
// "thoi_gian \t ten \t so_luong" vao bin\server\log_game\hd3_statdata.log de
// van dem duoc (nguoi van hanh gop bang tay/script). Mo file giu san, flush
// moi dong (tan suat thap: mo ruong/chet/bao danh - khong phai moi tick).
int LuaHD3_AddStatData(Lua_State* L)
{
	int nTop = Lua_GetTopIndex(L);
	if (nTop < 1 || !Lua_IsString(L, 1))
		return 0;
	const char* szName = Lua_ValueToString(L, 1);
	int nCount = 1;
	if (nTop >= 2 && Lua_IsNumber(L, 2))
		nCount = (int)Lua_ValueToNumber(L, 2);
	if (!szName || !szName[0])
		return 0;
	static FILE* s_pStat = NULL;
	if (!s_pStat)
	{
		char szPath[MAX_PATH * 2];
		g_GetRootPath(szPath);
		// [VA 25/08 - N4] bin\server\log_game KHONG TON TAI (log_game la thu muc SCRIPT);
		// thu muc log that cua may chu la logs\.
		strcat(szPath, "\\logs\\hd3_statdata.log");
		s_pStat = fopen(szPath, "a");
	}
	if (s_pStat)
	{
		time_t nNow = time(NULL);
		struct tm* pTm = localtime(&nNow);
		fprintf(s_pStat, "%04d-%02d-%02d %02d:%02d:%02d\t%s\t%d\n",
			pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday,
			pTm->tm_hour, pTm->tm_min, pTm->tm_sec, szName, nCount);
		fflush(s_pStat);
	}
	return 0;
}

// ---------------------------------------------------------------------------
// GetItemAllParams(nItemIdx) -> bang {1..6} = nGeneratorLevel[0..5].
// Linux 0x08102D20 doc 6 so lien tiep o +0x1E0. JX1: m_GeneratorParam (KItem.h:143).
int LuaHD3_GetItemAllParams(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1 || !Lua_IsNumber(L, 1))
		return 0;
	int nItemIdx = (int)Lua_ValueToNumber(L, 1);
	if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
		return 0;
	KItemGeneratorParam* pParam = Item[nItemIdx].GetGeneratorParam();
	Lua_NewTable(L);
	for (int i = 0; i < MAX_ITEM_MAGICLEVEL && i < 6; i++)
	{
		Lua_PushNumber(L, pParam->nGeneratorLevel[i]);
		Lua_RawSetI(L, -2, i + 1);
	}
	return 1;
}

// ITEM_GetItemRandSeed(nItemIdx) -> uRandomSeed (khong dau, script in %0.0f).
int LuaHD3_GetItemRandSeed(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1 || !Lua_IsNumber(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nItemIdx = (int)Lua_ValueToNumber(L, 1);
	if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, (double)Item[nItemIdx].GetGeneratorParam()->uRandomSeed);
	return 1;
}

// GetItemGenTime(nItemIdx) -> 0. JX1 KItem KHONG luu moc thoi gian tao vat pham
// (da kiem KItemCommonAttrib). Chi dung de GHI LOG roi do (lib\log.lua:51) nen
// tra 0 an toan; ghi chu sai lech trong BANGIAO.
int LuaHD3_GetItemGenTime(Lua_State* L)
{
	Lua_PushNumber(L, 0);
	return 1;
}

// GetItemQuality / GetGlodEqIndex / GetPlatinaEquipIndex / GetPlatinaLevel:
// he "pham chat vang/bach kim" cua JX2 (truong +4 bang item). JX1 quan ly
// trang bi Hoang Kim bang bang rieng phia script, KItem khong co truong nay.
// Cac cho goi trong 3 hoat dong CHI de ghi log + ghep do (composeex - ngoai
// luong choi). Tra "khong phai do vang" la an toan va trung thuc nhat.
int LuaHD3_GetItemQuality(Lua_State* L)
{
	Lua_PushNumber(L, 0);
	return 1;
}
int LuaHD3_GetGlodEqIndex(Lua_State* L)
{
	Lua_PushNumber(L, 0);
	return 1;
}
int LuaHD3_GetPlatinaEquipIndex(Lua_State* L)
{
	Lua_PushNumber(L, 0);
	return 1;
}
int LuaHD3_GetPlatinaLevel(Lua_State* L)
{
	// Linux: khong phai bach kim -> push nil (van 1 gia tri, script tostring())
	Lua_PushNil(L);
	return 1;
}

// GetRoomItems(nRoomType) -> bang chi so vat pham cua nguoi choi hien tai.
// Linux 0x0810D170. Cho goi duy nhat trong bao dong: lib\composeex.lua:191
// (quet TUI khi ghep do). JX1: quet tung o cua khay hanh trang
// (room_equipment + room_equipmentex - pos_equiproom map vao room_equipment,
//  KItemList.cpp:251) qua KInventory::FindItem(x,y); kich thuoc dung hang
// EQUIPMENT_ROOM_WIDTH/HEIGHT (GameDataDef.h:371-372, ca room ex cung co 6x10;
// FindItem tu kiem bien nen thua o cung vo hai);
// vat pham chiem nhieu o -> khu trung bang chi so lien truoc.
int LuaHD3_GetRoomItems(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	Lua_NewTable(L);
	if (nPlayerIndex <= 0)
		return 1;
	int nCount = 0;
	// [VA 25/08 - N7] ton trong tham so nRoomType (composeex.lua co nhieu khoang).
	// Mac dinh (thieu tham so hoac <=0) = khay hanh trang + phan mo rong, y nhu truoc.
	int nRoomArg = (Lua_GetTopIndex(L) >= 1 && Lua_IsNumber(L, 1)) ? (int)Lua_ValueToNumber(L, 1) : -1;
	int nRoomsOne[1] = { nRoomArg };
	static const int nRoomsDef[2] = { room_equipment, room_equipmentex };
	const int* pRooms = nRoomsDef;
	int nRoomN = 2;
	if (nRoomArg >= 0 && nRoomArg < room_num)
	{
		pRooms = nRoomsOne;
		nRoomN = 1;
	}
	static const int nRooms[2] = { room_equipment, room_equipmentex };
	std::map<int, int> mapSeen;
	for (int r = 0; r < nRoomN; r++)
	{
		KInventory* pRoom = &Player[nPlayerIndex].m_ItemList.m_Room[pRooms[r]];
		for (int y = 0; y < EQUIPMENT_ROOM_HEIGHT; y++)
		{
			for (int x = 0; x < EQUIPMENT_ROOM_WIDTH; x++)
			{
				int nItemIdx = pRoom->FindItem(x, y);
				if (nItemIdx > 0 && nItemIdx < MAX_ITEM && mapSeen.find(nItemIdx) == mapSeen.end())
				{
					mapSeen[nItemIdx] = 1;
					Lua_PushNumber(L, nItemIdx);
					Lua_RawSetI(L, -2, ++nCount);
				}
			}
		}
	}
	return 1;
}

// ---------------------------------------------------------------------------
// GetFirstPlayerAtServer() / GetNextPlayerAtServer() -> PlayerIndex (0 = het).
// Linux 0x08101CF0/0x08101D20 - con tro duyet noi bo (server don luong).
// missions\boss\bigboss.lua:289-294 duyet toan bo nguoi choi online.
static int s_nHD3PlayerCursor = 0;
static int sHD3_NextOnlinePlayer(int nFrom)
{
	for (int i = nFrom; i < MAX_PLAYER; i++)
	{
		if (Player[i].m_nIndex > 0)
			return i;
	}
	return 0;
}
int LuaHD3_GetFirstPlayerAtServer(Lua_State* L)
{
	s_nHD3PlayerCursor = sHD3_NextOnlinePlayer(1);
	Lua_PushNumber(L, s_nHD3PlayerCursor);
	return 1;
}
int LuaHD3_GetNextPlayerAtServer(Lua_State* L)
{
	if (s_nHD3PlayerCursor <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	s_nHD3PlayerCursor = sHD3_NextOnlinePlayer(s_nHD3PlayerCursor + 1);
	Lua_PushNumber(L, s_nHD3PlayerCursor);
	return 1;
}

// GetNpcAroundPlayerList(nNpcIdx, nDist) -> tbPlayerIndex, nCount.
// Linux 0x08104870 (2 gia tri). Khuon quet region + quy toa do cuc bo ve toan
// cuc lay tu LuaGetAroundNpcList o tren (PORT5 - da xu ly dung bay region).
int LuaHD3_GetNpcAroundPlayerList(Lua_State* L)
{
	int nCount = 0;
	int nTop = Lua_GetTopIndex(L);
	Lua_NewTable(L);
	if (nTop < 2 || !Lua_IsNumber(L, 1) || !Lua_IsNumber(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 2;
	}
	int nMe = (int)Lua_ValueToNumber(L, 1);
	int nDist = (int)Lua_ValueToNumber(L, 2);
	if (nMe <= 0 || nMe >= MAX_NPC || nDist <= 0 || Npc[nMe].m_SubWorldIndex < 0
		|| Npc[nMe].m_RegionIndex < 0)
	{
		Lua_PushNumber(L, 0);
		return 2;
	}
	KSubWorld* pWorld = &SubWorld[Npc[nMe].m_SubWorldIndex];
	int nMX = LOWORD(pWorld->m_Region[Npc[nMe].m_RegionIndex].m_RegionID) * pWorld->m_nRegionWidth + Npc[nMe].m_MapX;
	int nMY = HIWORD(pWorld->m_Region[Npc[nMe].m_RegionIndex].m_RegionID) * pWorld->m_nRegionHeight + Npc[nMe].m_MapY;
	for (int r = 0; r < pWorld->m_nTotalRegion; r++)
	{
		KRegion* pRegion = &pWorld->m_Region[r];
		KIndexNode* pNode = (KIndexNode*)pRegion->m_NpcList.GetHead();
		while (pNode)
		{
			int i = pNode->m_nIndex;
			pNode = (KIndexNode*)pNode->GetNext();
			if (i <= 0 || i >= MAX_NPC || Npc[i].m_Index <= 0 || i == nMe)
				continue;
			if (Npc[i].GetPlayerIdx() <= 0)		// chi lay NGUOI CHOI
				continue;
			if (Npc[i].m_Doing == do_death || Npc[i].m_Doing == do_revive)
				continue;
			int dx = LOWORD(pRegion->m_RegionID) * pWorld->m_nRegionWidth + Npc[i].m_MapX - nMX;
			int dy = HIWORD(pRegion->m_RegionID) * pWorld->m_nRegionHeight + Npc[i].m_MapY - nMY;
			if (dx * dx + dy * dy > nDist * nDist)
				continue;
			Lua_PushNumber(L, Npc[i].GetPlayerIdx());
			Lua_RawSetI(L, -2, ++nCount);
		}
	}
	Lua_PushNumber(L, nCount);
	return 2;
}

// ---------------------------------------------------------------------------
// ITEM_SetExpiredTime(nItemIdx, nGiaTri) -> 1. HAI DON VI (dung nhu ban goc):
//   >= 20000000 : ngay tuyet doi YYYYMMDD (het han 0h ngay do)
//   <  20000000 : SO PHUT ke tu bay gio (rank_perday.lua: 24*60)
// KItem.nExpireTime = giay theo goc 1451581200 (01/01/2016 - KItem.cpp:2629),
// so sanh voi KSG_GetCurSec() (KItem.h:346).
int LuaHD3_ITEM_SetExpiredTime(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2 || !Lua_IsNumber(L, 1) || !Lua_IsNumber(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nItemIdx = (int)Lua_ValueToNumber(L, 1);
	double dVal = (double)Lua_ValueToNumber(L, 2);
	if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	// [VA 25/08 - N9] ban goc: tham so 2 == 0 nghia la KHONG DAT HAN (test esi,esi/je),
	// KHONG phai 'het han ngay'. droptemplet.lua:129 chi kiem 'if .. then' ma Lua coi
	// 0 la true nen gia tri 0 CO THE toi day.
	if (dVal == 0.0)
	{
		Item[nItemIdx].SetExpireTime(0);
		Lua_PushNumber(L, 1);
		return 1;
	}
	int nExpireSec;		// giay theo goc 1451581200
	if (dVal >= 20000000.0)
	{
		int nDate = (int)dVal;
		struct tm t;
		memset(&t, 0, sizeof(t));
		t.tm_year = nDate / 10000 - 1900;
		t.tm_mon  = (nDate / 100) % 100 - 1;
		t.tm_mday = nDate % 100;
		t.tm_isdst = -1;
		time_t nAbs = mktime(&t);
		if (nAbs == (time_t)-1)
		{
			Lua_PushNumber(L, 0);
			return 1;
		}
		nExpireSec = (int)(nAbs - 1451581200);
	}
	else
	{
		nExpireSec = KSG_GetCurSec() + (int)dVal * 60;
	}
	Item[nItemIdx].SetExpireTime(nExpireSec);
	Lua_PushNumber(L, 1);
	return 1;
}

// ITEM_GetExpiredTime(nItemIdx) -> so PHUT con lai (<= 0: khong co han).
// (doi ung don vi phut cua duong Set; cho goi la activitysys\activity.lua:314
// chi so sanh <= 0)
int LuaHD3_ITEM_GetExpiredTime(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1 || !Lua_IsNumber(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nItemIdx = (int)Lua_ValueToNumber(L, 1);
	if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	// [VA 25/08 - N1] ban goc 0x08154540 tra GIA TRI THO (khong phep tinh nao) va
	// cho goi (activitysys\activity.lua:314-322) dua thang vao Time2Tm = epoch Unix
	// that => phai cong goc 1451581200 cua JX1 (KItem.cpp:2629). Doc TRUC TIEP truong,
	// KHONG qua GetExpireTime() vi getter tra 0 khi vat pham DA het han.
	int nExpire = Item[nItemIdx].m_CommonAttrib.nExpireTime;
	if (nExpire <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, (double)nExpire + 1451581200.0);
	return 1;
}

// ITEM_SetLeftUsageTime(nItemIdx, n) -> 1. KItem.nParam = "so lan su dung item"
// (KItem.h:87, SetParam/GetParam :413-414) - dung truong co san.
int LuaHD3_ITEM_SetLeftUsageTime(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2 || !Lua_IsNumber(L, 1) || !Lua_IsNumber(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nItemIdx = (int)Lua_ValueToNumber(L, 1);
	if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Item[nItemIdx].SetParam((int)Lua_ValueToNumber(L, 2));
	Lua_PushNumber(L, 1);
	return 1;
}

// SetItemBindState(nItemIdx, nState) -> 1. Linux 0x08127630, -2 = khoa vinh vien
// (battlehead.lua:1317, droptemplet.lua:144). JX1 DA CO he khoa vat pham:
// SetPlayerItemLock -> InsuranceCourse + LockItem.nState (KItem.h:313-316);
// duong cam ban/giao dich kiem "InsuranceCourse > 0 || InsuranceCourse == -2"
// (KPlayer.cpp:3755, :5155, :5237) => anh xa 1-1, ben vung qua save.
int LuaHD3_SetItemBindState(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2 || !Lua_IsNumber(L, 1) || !Lua_IsNumber(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nItemIdx = (int)Lua_ValueToNumber(L, 1);
	if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Item[nItemIdx].SetPlayerItemLock((int)Lua_ValueToNumber(L, 2));
	Lua_PushNumber(L, 1);
	return 1;
}

// ---------------------------------------------------------------------------
// DropItemEx(nSubWorldIdx, nMpsX, nMpsY, nBelonger, nVersion, szRandSeed,
//            nQuality, nGenre, nDetail, nParticular, nLevel, nSeries, nLuck,
//            nMagic1..nMagic6) -> nItemIdx (0 = hong).
// Linux 0x0811FD70 - tha xuong dat vat pham DA xac dinh day du thuoc tinh.
// Chu ky ghi ngay trong lib\droptemplet.lua:59-93 (19 tham so, szRandSeed la
// CHUOI). JX1: dung dung khuon KNpc::DropItemFromLuaScript (KNpc.cpp:8344)
// nhung theo TOA DO thay vi vi tri NPC. nQuality bo qua (JX1 khong co truong
// pham chat - xem ghi chu GetItemQuality); nVersion bo qua (AddItemSet2 tu lay
// GetGameVersion) - ca hai chi anh huong hien thi log.
int LuaHD3_DropItemEx(Lua_State* L)
{
	int nTop = Lua_GetTopIndex(L);
	if (nTop < 13)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nSubWorldIdx = (int)Lua_ValueToNumber(L, 1);
	int nMpsX      = (int)Lua_ValueToNumber(L, 2);
	int nMpsY      = (int)Lua_ValueToNumber(L, 3);
	int nBelonger  = (int)Lua_ValueToNumber(L, 4);
	double dSeed   = 0;
	if (Lua_IsString(L, 6))
		dSeed = atof(Lua_ValueToString(L, 6));
	else if (Lua_IsNumber(L, 6))
		dSeed = (double)Lua_ValueToNumber(L, 6);
	int nGenre     = (int)Lua_ValueToNumber(L, 8);
	int nDetail    = (int)Lua_ValueToNumber(L, 9);
	int nParticular= (int)Lua_ValueToNumber(L, 10);
	int nLevel     = (int)Lua_ValueToNumber(L, 11);
	int nSeries    = (int)Lua_ValueToNumber(L, 12);
	int nLuck      = (int)Lua_ValueToNumber(L, 13);
	int nMagic[MAX_ITEM_MAGICLEVEL];
	memset(nMagic, 0, sizeof(nMagic));
	for (int i = 0; i < 6 && i < MAX_ITEM_MAGICLEVEL; i++)
	{
		if (nTop >= 14 + i)
			nMagic[i] = (int)Lua_ValueToNumber(L, 14 + i);
	}
	if (nSubWorldIdx < 0 || nSubWorldIdx >= MAX_SUBWORLD)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nXOpt = ItemSet.genXOpt(nLuck);
	int nIdx = ItemSet.AddItemSet2(nGenre, nSeries, nLevel, nLuck, nDetail, nParticular,
		nMagic, g_SubWorldSet.GetGameVersion(), 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, nXOpt);
	if (nIdx <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	if (dSeed > 0)
		Item[nIdx].GetGeneratorParam()->uRandomSeed = (UINT)dSeed;

	POINT	ptLocal;
	KMapPos	Pos;
	ptLocal.x = nMpsX;
	ptLocal.y = nMpsY;
	SubWorld[nSubWorldIdx].GetFreeObjPos(ptLocal);
	Pos.nSubWorld = nSubWorldIdx;
	SubWorld[nSubWorldIdx].Mps2Map(ptLocal.x, ptLocal.y,
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, &Pos.nOffX, &Pos.nOffY);

	KObjItemInfo sInfo;
	sInfo.m_nItemID = nIdx;
	sInfo.m_nItemWidth = Item[nIdx].GetWidth();
	sInfo.m_nItemHeight = Item[nIdx].GetHeight();
	sInfo.m_nMoneyNum = 0;
	if (Item[nIdx].GetGenre() != item_equip && Item[nIdx].GetStackNum() > 1)
		sprintf(sInfo.m_szName, "%s x %d", Item[nIdx].GetName(), Item[nIdx].GetStackNum());
	else
		strcpy(sInfo.m_szName, Item[nIdx].GetName());
	sInfo.m_nColorID = Item[nIdx].GetColorItem();
	sInfo.m_nGenre = Item[nIdx].GetGenre();
	sInfo.m_nDetailType = Item[nIdx].GetDetailType();
	sInfo.m_nParticularType = Item[nIdx].GetParticular();
	sInfo.m_nMovieFlag = 1;
	sInfo.m_nSoundFlag = 1;
	sInfo.m_dwNpcId1 = 0;
	int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), Pos, sInfo);
	if (nObj == -1)
	{
		ItemSet.Remove(nIdx);
		Lua_PushNumber(L, 0);
		return 1;
	}
	if (nBelonger > 0 && nBelonger < MAX_PLAYER)
		Object[nObj].SetItemBelong(nBelonger);
	Lua_PushNumber(L, nIdx);
	return 1;
}

// NpcDropMoney(nNpcIdx, nSoTien, nBelonger) -> 0 gia tri. Linux 0x0811D9C0.
// JX1 da co DropNpcMoney(nNpcIdx, nSoTien) voi belong = nguoi choi hien tai
// (ScriptFuns.cpp:4267) - day la ban them tham so 3.
int LuaHD3_NpcDropMoney(Lua_State* L)
{
	int nTop = Lua_GetTopIndex(L);
	if (nTop < 2)
		return 0;
	int nIndex = (int)Lua_ValueToNumber(L, 1);
	int nMoneyNum = (int)Lua_ValueToNumber(L, 2);
	int nBelonger = (nTop >= 3) ? (int)Lua_ValueToNumber(L, 3) : GetPlayerIndex(L);
	if (nIndex <= 0 || nIndex >= MAX_NPC || nMoneyNum <= 0 || Npc[nIndex].m_SubWorldIndex < 0)
		return 0;
	int nX, nY;
	POINT ptLocal;
	KMapPos Pos;
	Npc[nIndex].GetMpsPos(&nX, &nY);
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[Npc[nIndex].m_SubWorldIndex].GetFreeObjPos(ptLocal);
	Pos.nSubWorld = Npc[nIndex].m_SubWorldIndex;
	SubWorld[Npc[nIndex].m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y,
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, &Pos.nOffX, &Pos.nOffY);
	int nObjIdx = ObjSet.AddMoneyObj(Pos, nMoneyNum);
	if (nObjIdx > 0 && nObjIdx < MAX_OBJECT && nBelonger > 0)
		Object[nObjIdx].SetItemBelong(nBelonger);
	return 0;
}

// ---------------------------------------------------------------------------
// JoinMission(nMissionId, nCamp) -> 0 gia tri. Linux 0x08137E40: them NGUOI
// CHOI HIEN TAI vao mission tren SUBWORLD HIEN TAI voi nhom nCamp (dich nguoc
// 15_bosung_soat_api.md muc 3.2). Cho goi: challengeoftime\npc\
// dragonboat_main.lua:163. Khuon = LuaAddMissionPlayer (ScriptFuns.cpp:11435).
// LUU Y trung ten: mission_match.lua tu dinh nghia "function JoinMission" -
// JX1 moi tep .lua mot Lua_State rieng nen ban script CHI che trong tep do,
// khong anh huong dang ky engine nay (va nguoc lai).
int LuaHD3_JoinMission(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2 || !Lua_IsNumber(L, 1))
		return 0;
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	int nCamp = (int)Lua_ValueToNumber(L, 2);
	if (nMissionId < 0 || nCamp < 0)
		return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nPlayerIndex <= 0 || nSubWorldIndex < 0)
		return 0;
	KMission Mission;
	Mission.SetMissionId(nMissionId);
	KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
	if (pMission)
		pMission->AddPlayer(nPlayerIndex, Player[nPlayerIndex].m_dwID, nCamp);
	return 0;
}

// ---------------------------------------------------------------------------
// [VA 25/08 - A7-C1] HD3_AddNpc / HD3_AddNpcEx - dung ngu nghia ban Linux.
// LuaAddNpcEx cua JX1 lay tham so 7 lam CAMP va SetCurrentCamp GHI DE camp tu
// npcs.txt; GenOneRelation (KNpcSet.cpp:143) coi camp 0 / camp trung la ALLY
// => moi NPC 3 hoat dong thanh dong minh, khong danh duoc. Hai ham nay:
//   - KHONG dung den camp (giu nguyen camp tu npcs.txt - dung nhu Linux)
//   - HD3_AddNpc: series TU RANDOM 0..4 (Linux AddNpc goi rand()%5 noi bo,
//     0x0811BB10 tai 0x0811BC8C), tham so 6 = bNoRevive
//   - HD3_AddNpcEx: tham so 3 = series (script tu tinh), tham so 7 = bNoRevive
// Khong sua LuaAddNpc/LuaAddNpcEx goc de khoi dung cham cac he JX1/PORT5 dang chay.
static int sHD3_AddNpcCommon(Lua_State* L, int bHasSeries)
{
	int nTop = Lua_GetTopIndex(L);
	int nMin = bHasSeries ? 6 : 5;
	if (nTop < nMin)
		return 0;
	int nId = 0;
	if (Lua_IsNumber(L, 1))
		nId = (int)Lua_ValueToNumber(L, 1);
	else if (Lua_IsString(L, 1))
	{
		const char* pName = Lua_ValueToString(L, 1);
		if (!pName || !pName[0])
			return 0;
		nId = g_NpcSetting.FindRow((char*)pName) - 2;
	}
	else
		return 0;
	if (nId < 0)
		nId = 0;
	int nLevel = (int)Lua_ValueToNumber(L, 2);
	if (nLevel >= 128) nLevel = 127;
	if (nLevel < 0)    nLevel = 1;
	int nArg = 3;
	int nSeries;
	if (bHasSeries)
		nSeries = (int)Lua_ValueToNumber(L, nArg++);
	else
		nSeries = g_Random(5);	// Linux: rand() % 5
	if (nSeries < 0 || nSeries > 4)
		nSeries = g_Random(5);
	int nSubWorldIdx = (int)Lua_ValueToNumber(L, nArg++);
	int nX = (int)Lua_ValueToNumber(L, nArg++);
	int nY = (int)Lua_ValueToNumber(L, nArg++);
	int nNoRevive = (nTop >= nArg && Lua_IsNumber(L, nArg)) ? (int)Lua_ValueToNumber(L, nArg) : 0;
	nArg++;
	if (nSubWorldIdx < 0 || nSubWorldIdx >= MAX_SUBWORLD)
		return 0;
	int nNpcIdxInfo = MAKELONG(nLevel, nId);
	int nNpcIdx = NpcSet.AddNpcSet2(nNpcIdxInfo, nSeries, nSubWorldIdx, nX, nY);
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
	{
		Lua_PushNumber(L, nNpcIdx);
		return 1;
	}
	if (nNoRevive != 0)
		Npc[nNpcIdx].m_bNoRevive = 1;
	// tham so ke tiep: ten hien thi (chuoi khac rong)
	if (nTop >= nArg && Lua_IsString(L, nArg))
	{
		const char* pDispName = Lua_ValueToString(L, nArg);
		if (pDispName && pDispName[0])
			g_StrCpy(Npc[nNpcIdx].Name, (char*)pDispName);
	}
	// tham so cuoi (flag/isboss ban Linux - cot [8] bang killbosshead): dich
	// nguoc ban Linux ghi +0x181C=3 khi flag==1 => ten boss mau VANG tren
	// client (chu game chot 25/08). m_Type duoc day sang client qua
	// NpcEnchant (KNpc.cpp SendSyncData/SendNormalSyncData - va kem 25/08).
	nArg++;
	if (nTop >= nArg && Lua_IsNumber(L, nArg) && (int)Lua_ValueToNumber(L, nArg) != 0)
		Npc[nNpcIdx].m_Type = boss_gold;
	Lua_PushNumber(L, nNpcIdx);
	return 1;
}
// [3HD 25/08 C14] HD3_DelNpcByName(szTen) - xoa NPC (khong phai player) co ten
// CHUA chuoi szTen. Dung don NPC "Nhiep Thi Tran" CU cua ban Viet (nam trong
// du lieu map tinh, khong script nao tat duoc) TRUOC khi sinh NPC 769 Linux.
// Duyet m_UseIdx nhu KNpcSet::GetAroundGoldMonster => chi cham NPC dang song,
// khong dinh free-slot; go theo dung khuon LuaDelNpc (ScriptFuns.cpp:7078).
int LuaHD3_DelNpcByName(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1 || !Lua_IsString(L, 1))
		return 0;
	const char* pTen = Lua_ValueToString(L, 1);
	// tham so 2 (tuy chon): CHI xoa NPC dang o map co SubWorldID nay (vd 336
	// ben Phong Lang Do) - tranh xoa nham NPC trung ten o he khac (ben do thon).
	int nLocMapID = 0;
	if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2))
		nLocMapID = (int)Lua_ValueToNumber(L, 2);
	if (!pTen || !pTen[0])
		return 0;
	int nXoa = 0;
	int nGom = 0;
	static int s_anGom[512];
	// m_UseIdx la private => quet thang mang Npc[]; slot trong co m_dwID == 0
	// (quy uoc engine, vd KNpc.cpp:2621 kiem "khong ton tai" bang m_dwID == 0).
	for (int nIdx = 1; nIdx < MAX_NPC; nIdx++)
	{
		if (Npc[nIdx].m_dwID == 0)
			continue;
		if (Npc[nIdx].IsPlayer())
			continue;
		if (Npc[nIdx].m_SubWorldIndex < 0 || Npc[nIdx].m_RegionIndex < 0)
			continue;
		if (nLocMapID != 0 && SubWorld[Npc[nIdx].m_SubWorldIndex].m_SubWorldID != nLocMapID)
			continue;
		if (strstr(Npc[nIdx].Name, pTen) == NULL)
			continue;
		if (nGom < 512)
			s_anGom[nGom++] = nIdx;
	}
	for (int i = 0; i < nGom; i++)
	{
		int n = s_anGom[i];
		SubWorld[Npc[n].m_SubWorldIndex].m_Region[Npc[n].m_RegionIndex].RemoveNpc(n);
		SubWorld[Npc[n].m_SubWorldIndex].m_Region[Npc[n].m_RegionIndex].DecRef(Npc[n].m_MapX, Npc[n].m_MapY, obj_npc);
		NpcSet.Remove(n);
		nXoa++;
	}
	Lua_PushNumber(L, nXoa);
	return 1;
}
// [3HD 25/08 C16] HD3_DelNpcByScript(szSub) - xoa NPC co ActionScript CHUA
// chuoi szSub (khong phan biet hoa thuong - engine luu ActionScript da
// g_StrLower, KNpcSet.cpp:429). Bam theo SCRIPT nen phan biet duoc NPC cu
// (nhieptran.lua / thuyenphu.lua) voi NPC moi (nieshichen/hd3_thuyenphu)
// du TRUNG TEN. Goi duoc moi phut (tu lanh khi NPC cu sinh muon/hoi sinh).
// [3HD 25/08 C19] HD3_DelNpcByNameEx(szTen, nMapID|0, szExcludeScript) - xoa NPC
// trung TEN nhung ActionScript KHONG chua szExcludeScript (phan biet NPC cu voi
// NPC moi cung ten bat ke NPC cu bind script gi hay khong co script). Log tung
// nan nhan vao DebugLog de truy nguon (template + script).
int LuaHD3_DelNpcByNameEx(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1 || !Lua_IsString(L, 1))
		return 0;
	const char* pTen = Lua_ValueToString(L, 1);
	if (!pTen || !pTen[0])
		return 0;
	int nLocMap = 0;
	if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2))
		nLocMap = (int)Lua_ValueToNumber(L, 2);
	char szExcl[80];
	szExcl[0] = 0;
	if (Lua_GetTopIndex(L) >= 3 && Lua_IsString(L, 3))
	{
		g_StrCpyLen(szExcl, (char*)Lua_ValueToString(L, 3), sizeof(szExcl));
		g_StrLower(szExcl);
	}
	int nXoa = 0;
	int nGom = 0;
	static int s_anGomN[512];
	for (int nIdx = 1; nIdx < MAX_NPC; nIdx++)
	{
		if (Npc[nIdx].m_dwID == 0)
			continue;
		if (Npc[nIdx].IsPlayer())
			continue;
		if (Npc[nIdx].m_SubWorldIndex < 0 || Npc[nIdx].m_RegionIndex < 0)
			continue;
		if (nLocMap != 0 && SubWorld[Npc[nIdx].m_SubWorldIndex].m_SubWorldID != nLocMap)
			continue;
		if (strstr(Npc[nIdx].Name, pTen) == NULL)
			continue;
		if (szExcl[0] && Npc[nIdx].ActionScript[0] && strstr(Npc[nIdx].ActionScript, szExcl) != NULL)
			continue;	// NPC cua minh - giu
		if (nGom < 512)
			s_anGomN[nGom++] = nIdx;
	}
	for (int i = 0; i < nGom; i++)
	{
		int n = s_anGomN[i];
		g_DebugLog("[3HD C19] xoa NPC cu idx=%d setting=%d map=%d script=%s",
			n, Npc[n].m_NpcSettingIdx, SubWorld[Npc[n].m_SubWorldIndex].m_SubWorldID, Npc[n].ActionScript);
		SubWorld[Npc[n].m_SubWorldIndex].m_Region[Npc[n].m_RegionIndex].RemoveNpc(n);
		SubWorld[Npc[n].m_SubWorldIndex].m_Region[Npc[n].m_RegionIndex].DecRef(Npc[n].m_MapX, Npc[n].m_MapY, obj_npc);
		NpcSet.Remove(n);
		nXoa++;
	}
	Lua_PushNumber(L, nXoa);
	return 1;
}
int LuaHD3_DelNpcByScript(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1 || !Lua_IsString(L, 1))
		return 0;
	const char* pSub = Lua_ValueToString(L, 1);
	if (!pSub || !pSub[0])
		return 0;
	char szSub[80];
	g_StrCpyLen(szSub, (char*)pSub, sizeof(szSub));
	g_StrLower(szSub);
	int nXoa = 0;
	int nGom = 0;
	static int s_anGomS[512];
	for (int nIdx = 1; nIdx < MAX_NPC; nIdx++)
	{
		if (Npc[nIdx].m_dwID == 0)
			continue;
		if (Npc[nIdx].IsPlayer())
			continue;
		if (Npc[nIdx].m_SubWorldIndex < 0 || Npc[nIdx].m_RegionIndex < 0)
			continue;
		if (Npc[nIdx].ActionScript[0] == 0 || strstr(Npc[nIdx].ActionScript, szSub) == NULL)
			continue;
		if (nGom < 512)
			s_anGomS[nGom++] = nIdx;
	}
	for (int i = 0; i < nGom; i++)
	{
		int n = s_anGomS[i];
		SubWorld[Npc[n].m_SubWorldIndex].m_Region[Npc[n].m_RegionIndex].RemoveNpc(n);
		SubWorld[Npc[n].m_SubWorldIndex].m_Region[Npc[n].m_RegionIndex].DecRef(Npc[n].m_MapX, Npc[n].m_MapY, obj_npc);
		NpcSet.Remove(n);
		nXoa++;
	}
	Lua_PushNumber(L, nXoa);
	return 1;
}
int LuaHD3_AddNpc(Lua_State* L)
{
	return sHD3_AddNpcCommon(L, 0);
}
int LuaHD3_AddNpcEx(Lua_State* L)
{
	return sHD3_AddNpcCommon(L, 1);
}

// ---------------------------------------------------------------------------
// DisabledUseTownP(n) - THAY THE stub cu (return 0). Linux: cam nguoi choi
// hien tai dung Hoi thanh phu khi o trong hoat dong (PLD gọi (1) luc len
// thuyen fld_head.lua:142; Vuot Ai goi (1) khi vao ai). JX1 khong co co nay
// trong KPlayer (CAM them truong - ABI), dung mang static theo PlayerIndex +
// NHO SUBWORLD luc bat: co chi con hieu luc khi nguoi choi VAN o dung
// subworld do => roi map / relog sang map khac la tu het, khong bi ket co.
// Script kiem bang GetDisabledUseTownP() (dang ky moi); item hoi thanh cua
// JX1 chan theo map o script\header\forbidmap.lua (da them map 3 hoat dong -
// cung khuon Thanh Bao 984), co nay la lop dung theo dung nguyen ban Linux.
static BYTE s_byHD3NoTownP[MAX_PLAYER];
static int  s_nHD3NoTownPWorld[MAX_PLAYER];
int LuaHD3_DisabledUseTownP_Real(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
		return 0;
	int nOn = 1;
	if (Lua_GetTopIndex(L) >= 1 && Lua_IsNumber(L, 1))
		nOn = (int)Lua_ValueToNumber(L, 1);
	if (nOn == 1)
	{
		s_byHD3NoTownP[nPlayerIndex] = 1;
		s_nHD3NoTownPWorld[nPlayerIndex] = (Player[nPlayerIndex].m_nIndex > 0)
			? Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex : -1;
	}
	else
	{
		s_byHD3NoTownP[nPlayerIndex] = 0;
	}
	return 0;
}
int LuaHD3_GetDisabledUseTownP(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nRet = 0;
	if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && s_byHD3NoTownP[nPlayerIndex]
		&& Player[nPlayerIndex].m_nIndex > 0
		&& Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex == s_nHD3NoTownPWorld[nPlayerIndex])
		nRet = 1;
	Lua_PushNumber(L, nRet);
	return 1;
}

// IsDisabledUseHeart() -> 0/1. Linux 0x0812ED70 doc co cam cua nguoi choi
// hien tai (bit 0x200000 - cung ho co voi DisabledUseTownP). Dung chung co
// tren: item\heart_head.lua:116 chi can chan Tam Tam Tuong Anh trong map
// hoat dong - hanh vi trung voi co TownP.
int LuaHD3_IsDisabledUseHeart(Lua_State* L)
{
	return LuaHD3_GetDisabledUseTownP(L);
}

// ---------------------------------------------------------------------------
// OpenProgressBar(szTitle, nFrame, nEventFlag, bDesc, szOnTime, szOnBreak)
// Linux 0x081082D0 - thanh tien trinh tren client, xong goi szOnTime trong
// TEP GOI. JX1 co san co che TimeBox (S2C_TIME_BOX + m_dwTimeBoxId +
// m_szTaskExcuteFun - LuaOpenTimeBox ScriptFuns.cpp:9675) lam dung viec do.
// Chuyen doi: nFrame (18 khung/giay - Linux FRAME2TIME) -> giay; szOnBreak
// khong co doi ung tren TimeBox JX1 (ngat = khong goi gi) - ghi chu sai lech.
int LuaHD3_OpenProgressBar(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 5)
		return 0;
	const char* szTitle = Lua_IsString(L, 1) ? Lua_ValueToString(L, 1) : "";
	int nFrame = (int)Lua_ValueToNumber(L, 2);
	const char* szOnTime = Lua_IsString(L, 5) ? Lua_ValueToString(L, 5) : "";
	if (!szOnTime || !szOnTime[0])
		return 0;
	int nSec = nFrame / 18;
	if (nSec < 1)
		nSec = 1;
	// callback chay trong chinh tep dang goi
	const char* szSelf = g_GetScriptNameByState(L);
	if (szSelf && szSelf[0])
		Player[nPlayerIndex].m_dwTimeBoxId = g_FileName2Id((LPSTR)szSelf);
	S2C_TIME_BOX NetCommand;
	NetCommand.ProtocolType = s2c_timebox;
	strncpy(NetCommand.Value, szTitle, sizeof(NetCommand.Value) - 1);
	NetCommand.Value[sizeof(NetCommand.Value) - 1] = 0;
	NetCommand.Value1 = nSec;
	strncpy(NetCommand.Value2, szOnTime, sizeof(NetCommand.Value2) - 1);
	NetCommand.Value2[sizeof(NetCommand.Value2) - 1] = 0;
	strncpy(Player[nPlayerIndex].m_szTaskExcuteFun, szOnTime, sizeof(Player[nPlayerIndex].m_szTaskExcuteFun) - 1);
	if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx != -1)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(S2C_TIME_BOX));
	return 0;
}

// ---------------------------------------------------------------------------
// Add120SkillExp(nExp) -> so exp da cong. JX1 DA CO he luyen ky nang 120:
// KPlayer::AddSkillExp120 (duong danh quai KNpc.cpp:3843-3859 dang dung).
// Cho goi: task\task_award_extend.lua:6 (thuong sat thu cap 90).
int LuaHD3_Add120SkillExp(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1 || !Lua_IsNumber(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nExp = (int)Lua_ValueToNumber(L, 1);
	if (nExp <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Player[nPlayerIndex].AddSkillExp120(nExp);
	Lua_PushNumber(L, nExp);
	return 1;
}

// ST_IsTransLife() -> 0/1. Suy truc tiep tu ST_GetTransLifeCount (JX1 da co,
// = LuaGetPlayerReBornValue - ScriptFuns.cpp:14336): so lan chuyen sinh > 0.
extern int LuaGetPlayerReBornValue(Lua_State* L);
int LuaHD3_ST_IsTransLife(Lua_State* L)
{
	int nRet = LuaGetPlayerReBornValue(L);
	if (nRet >= 1 && Lua_IsNumber(L, -1))
	{
		int nCount = (int)Lua_ValueToNumber(L, -1);
		Lua_PushNumber(L, (nCount > 0) ? 1 : 0);
		return 1;
	}
	Lua_PushNumber(L, 0);
	return 1;
}

// ---------------------------------------------------------------------------
// Nhom stub CO CHU DICH - cac he JX2 chua ton tai tren JX1, MOI cho goi deu
// nam NGOAI luong choi loi cua 3 hoat dong (bang doi chieu: 15_bosung muc 6).
// Tra gia tri an toan de khong "attempt to call nil" lam dut ham dang chay.
// SendScriptData(nProtocolId, nObHandle): giao thuc script->client cua JX2 -
// client JX1 khong hieu goi nay; cho goi (protocol_def_gs.lua:193) chi chay
// khi UI JX2 mo. QueryWiseManForSB: hoi vi tri qua may chu khac - khong co
// cum lien server. BT_GetBattleParam: kho du lieu battle relay. ST_DoTransLife/
// ST_LevelUp/PET_*: chuyen sinh cuong buc / thu cung JX2 (thapnienlenhbai,
// translife_6 - ngoai 3 hoat dong).
int LuaHD3_SendScriptData(Lua_State* L)
{
	sHD3_LogOnce("SendScriptData");
	Lua_PushNumber(L, 0);
	return 1;
}
int LuaHD3_QueryWiseManForSB(Lua_State* L)
{
	sHD3_LogOnce("QueryWiseManForSB");
	return 0;
}
int LuaHD3_BT_GetBattleParam(Lua_State* L)
{
	// [VA 25/08 - N8] ban goc 0x081C69B0 tra CHUOI (lua_pushstring), khong phai so.
	// battlehead.lua:631/639 goi getNpcInfo(str) - day so se tach sai.
	sHD3_LogOnce("BT_GetBattleParam");
	Lua_PushString(L, "");
	return 1;
}
int LuaHD3_ST_DoTransLife(Lua_State* L)
{
	sHD3_LogOnce("ST_DoTransLife");
	Lua_PushNumber(L, 0);
	return 1;
}
int LuaHD3_ST_LevelUp(Lua_State* L)
{
	sHD3_LogOnce("ST_LevelUp");
	Lua_PushNumber(L, 0);
	return 1;
}
int LuaHD3_PET_Stub(Lua_State* L)
{
	sHD3_LogOnce("PET_*");
	Lua_PushNumber(L, 0);
	return 1;
}

// TrimString() - cat trang (space/TAB/CR/LF) o HAI DAU chuoi trong bo dem
// dung chung s_szStrBuf cua nhom PushString/AppendString/PopString (dinh
// nghia phia tren trong file nay). Linux 0x080FF630. lib\string.lua:163.
int LuaHD3_TrimString(Lua_State* L)
{
	int nLen = (int)strlen(s_szStrBuf);
	int nFrom = 0;
	while (nFrom < nLen && (s_szStrBuf[nFrom] == ' ' || s_szStrBuf[nFrom] == '\t'
		|| s_szStrBuf[nFrom] == '\r' || s_szStrBuf[nFrom] == '\n'))
		nFrom++;
	int nTo = nLen - 1;
	while (nTo >= nFrom && (s_szStrBuf[nTo] == ' ' || s_szStrBuf[nTo] == '\t'
		|| s_szStrBuf[nTo] == '\r' || s_szStrBuf[nTo] == '\n'))
		nTo--;
	int nNew = nTo - nFrom + 1;
	if (nNew < 0)
		nNew = 0;
	if (nFrom > 0 && nNew > 0)
		memmove(s_szStrBuf, s_szStrBuf + nFrom, nNew);
	s_szStrBuf[nNew] = 0;
	return 0;
}

#endif // _SERVER
