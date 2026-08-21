#include "KCore.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KMath.h"
#include "KObj.h"
#include "KPlayer.h"
#include "KNpcAI.h"
#include "coreshell.h"
#include "KTaskFuns.h"
// flying add here, to use math lib
#include <math.h>
extern int GetRandomNumber(int nMin, int nMax);

#define		MAX_FOLLOW_DISTANCE		48
#define		FOLLOW_WALK_DISTANCE	100

KNpcAI NpcAI;

KNpcAI::KNpcAI()
{
	m_nIndex = 0;
	m_bActivate = TRUE;
}


void KNpcAI::NotActivate(int nIndex)
{
	m_nIndex = nIndex;	
	if (Npc[m_nIndex].IsPlayer())
	{
	#ifdef _SERVER
	TriggerObjectTrap();
	TriggerMapTrap();
	#endif
	}
}

void KNpcAI::Activate(int nIndex)// flying modified this function. // Jun.4.2003
{
	m_nIndex = nIndex;	
	if (Npc[m_nIndex].IsPlayer())
	{
		ProcessPlayer();
		return;
	}
#ifdef _SERVER
	if (Npc[m_nIndex].m_CurrentLifeMax == 0)
		return;

	if (Npc[m_nIndex].Owner[0] && Npc[m_nIndex].m_bNpcFollowFindPath)//add by phong ki襲 19/08/2021
	{
		ProcessAIFollow();
		return;
	}

	int nCurTime = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_dwCurrentTime;
	if (/*Npc[m_nIndex].m_nPeopleIdx ||*/Npc[m_nIndex].m_NextAITime <= nCurTime)
	{
		Npc[m_nIndex].m_NextAITime = nCurTime + Npc[m_nIndex].m_AIMAXTime;
		switch(Npc[m_nIndex].m_AiMode)
		{
		case 1:
			ProcessAIType01();
			break;
		case 2:
			ProcessAIType02();
			break;
		case 3:
			ProcessAIType03();
			break;
		case 4:
			ProcessAIType04();
			break;
		case 5:
			ProcessAIType05();
			break;
		case 6:
			ProcessAIType06();
			break;
/*		case 7:
			ProcessAIType7();
			break;
		case 8:
			ProcessAIType8();
			break;
		case 9:
			ProcessAIType9();
			break;
		case 10:
			ProcessAIType10();
			break;*/
		default:
			break;
		}
	}
// flying add the code for the macro such as "_CLIENT".
// because this code only run at client.
#else
	if (Npc[m_nIndex].m_Kind > 3 && Npc[m_nIndex].m_AiMode > 10)
	{
		if (CanShowNpc())
		{

			if (GetRandomNumber(0, 1))	
			{
				Npc[m_nIndex].m_AiParam[5] = 0;
				Npc[m_nIndex].m_AiParam[4] = 5;
				return;
			}
			if (!KeepActiveRange())
				ProcessShowNpc();
		}
	}
#endif
}

#ifdef _SERVER
#define	MAX_FIND_PATH_NPC_DISTANCE	750
#define	MIN_FIND_PATH_NPC_DISTANCE	32
#define	MAX_WAIT_PATH_NPC_TIME		6000
#define	MAX_FIND_PATH_NPC_TIME		36000

void	KNpcAI::ProcessAIFollow()										//add by phong ki襲 using v薾 ti猽
{
	int nIdx = Npc[m_nIndex].FindAroundPlayer(Npc[m_nIndex].Owner);

	if (nIdx <= 0 || Npc[nIdx].m_dwID <= 0)
		return;

	if (Npc[m_nIndex].m_SubWorldIndex != Npc[nIdx].m_SubWorldIndex)
		return;

	if (Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive)
		return;

	if (Npc[m_nIndex].m_uFindPathTime)
	{
		if (Npc[m_nIndex].m_uFindPathMaxTime != -1)
		{
			DWORD dwTime = MAX_FIND_PATH_NPC_TIME;

			if (Npc[m_nIndex].m_uFindPathMaxTime > 0)
				dwTime = Npc[m_nIndex].m_uFindPathMaxTime;

			if (g_SubWorldSet.GetGameTime() - Npc[m_nIndex].m_uFindPathTime > dwTime)
			{
				if (Npc[m_nIndex].m_RegionIndex >= 0)
				{
					SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].RemoveNpc(m_nIndex);
					SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].DecRef(Npc[m_nIndex].m_MapX, Npc[m_nIndex].m_MapY, obj_npc);
				}
				NpcSet.Remove(m_nIndex);
				return;
			}
		}
	}

	int distance = NpcSet.GetDistance(nIdx, m_nIndex);

	if (distance <= MAX_FIND_PATH_NPC_DISTANCE)
	{
		if (Npc[m_nIndex].m_CurrentCamp != camp_animal)
			Npc[m_nIndex].SetCurrentCamp(camp_animal);

		if (distance > MIN_FIND_PATH_NPC_DISTANCE)
		{
			if (Npc[m_nIndex].m_CurrentCamp != Npc[nIdx].m_CurrentCamp)
				Npc[m_nIndex].SetCurrentCamp(Npc[nIdx].m_CurrentCamp);

			if (Npc[m_nIndex].m_FightMode != Npc[nIdx].m_FightMode)
				Npc[m_nIndex].m_FightMode = Npc[nIdx].m_FightMode;

			if (Npc[m_nIndex].m_uLastFindPathTime)
				Npc[m_nIndex].m_uLastFindPathTime = 0;

			int nOwnerX, nOwnerY, nMpsX, nMpsY, nXGo, nYGo;

			Npc[nIdx].GetMpsPos(&nOwnerX, &nOwnerY);
			Npc[m_nIndex].GetMpsPos(&nMpsX, &nMpsY);

			nXGo = nOwnerX + ((nOwnerX - nMpsX) > 0 ? -1 : 1) * 50;
			nYGo = nOwnerY + ((nOwnerY - nMpsY) > 0 ? -1 : 1) * 50;

			Npc[m_nIndex].SendCommand(do_walk, nXGo, nYGo);	
		}
	}
	else
	{
		if (Npc[m_nIndex].m_CurrentCamp != camp_animal)
			Npc[m_nIndex].SetCurrentCamp(camp_animal);

		if (Npc[m_nIndex].m_uLastFindPathTime <= 0)
			Npc[m_nIndex].m_uLastFindPathTime = g_SubWorldSet.GetGameTime();
		else
		{
			if (g_SubWorldSet.GetGameTime() - Npc[m_nIndex].m_uLastFindPathTime > MAX_WAIT_PATH_NPC_TIME)
			{
				if (Npc[m_nIndex].m_RegionIndex >= 0)
				{
					SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].RemoveNpc(m_nIndex);
					SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].DecRef(Npc[m_nIndex].m_MapX, Npc[m_nIndex].m_MapY, obj_npc);
				}
				NpcSet.Remove(m_nIndex);
			}
		}
	}
}
#endif

// flying add these functions // Run at client.
#ifndef _SERVER

int KNpcAI::ProcessShowNpc()
{
    int nResult  = false;
    int nRetCode = false;

	switch (Npc[m_nIndex].m_AiMode)
	{

	case 11:
		nRetCode = ShowNpcType11();
        if (!nRetCode)
            goto Exit0;
		break;

	case 12:
		nRetCode = ShowNpcType12();
        if (!nRetCode)
            goto Exit0;
		break;

	case 13:
		nRetCode = ShowNpcType13();
        if (!nRetCode)
            goto Exit0;
		break;

	case 14:
		nRetCode = ShowNpcType14();
        if (!nRetCode)
            goto Exit0;
		break;

	case 15:
		nRetCode = ShowNpcType15();
        if (!nRetCode)
            goto Exit0;
		break;

	case 16:
		nRetCode = ShowNpcType16();
        if (!nRetCode)
            goto Exit0;
		break;

	case 17:
		nRetCode = ShowNpcType17();
        if (!nRetCode)
            goto Exit0;
		break;
	default:
		break;
	}

    nResult = true;
Exit0:
	return nResult;
}

int KNpcAI::ShowNpcType11()
{
    int nResult = false;
    int nRetCode = false;

	KNpc& aNpc = Npc[m_nIndex];
	// Go the distance between P1 to P2	
	int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
	int nCurY = 0;
	int nOffX = 0;
	int nOffY = 0;
	int nOffsetDir = 0;
	
	aNpc.m_Height = GetRandomNumber(aNpc.m_AiParam[6] - 4, aNpc.m_AiParam[6]);

	aNpc.GetMpsPos(&nCurX, &nCurY);

	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	
    if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	
    nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);

	if (aNpc.m_CurrentWalkSpeed > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / (int) aNpc.m_CurrentWalkSpeed;
		aNpc.m_AiParam[5] = 0;
	}
	//if (KeepActiveShowRange())
	//	aNpc.m_Dir += 32;
	aNpc.m_Dir += nOffsetDir;
	if (aNpc.m_Dir < 0)
		aNpc.m_Dir += 64;
	else
		aNpc.m_Dir %= 64;
	
	nRetCode = GetNpcMoveOffset(aNpc.m_Dir, nDistance, &nOffX, &nOffY);
    if (!nRetCode)
        goto Exit0;

	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	aNpc.SendCommand(do_walk, nDesX, nDesY);	

    nResult = true;
Exit0:
	return nResult;
}

int KNpcAI::ShowNpcType12()// 蜻蜓型// done
{
    int nResult = false;
    int nRetCode = false;

	// Go the distance between P1 to P2	
	int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
	int nCurY = 0;
	int nOffX = 0;
	int nOffY = 0;
	int nOffsetDir = 0;
	KNpc& aNpc = Npc[m_nIndex];

	aNpc.m_Height = GetRandomNumber(aNpc.m_AiParam[6] - 4, aNpc.m_AiParam[6]);

	aNpc.GetMpsPos(&nCurX, &nCurY);
		
	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);

	// 取得运动所需的时间，保存在参数表中
	if (aNpc.m_CurrentWalkSpeed > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / (int) aNpc.m_CurrentWalkSpeed;
		aNpc.m_AiParam[5] = 0;
	}
	else
	{
		aNpc.m_AiParam[4] = 0;
		aNpc.m_AiParam[5] = 0;
	}
	aNpc.m_Dir += nOffsetDir;
	if (aNpc.m_Dir < 0)
		aNpc.m_Dir += 64;
	else
		aNpc.m_Dir %= 64;

	// 根据三角函数计算偏移的X、Y数值
	nRetCode = GetNpcMoveOffset(aNpc.m_Dir, nDistance, &nOffX, &nOffY);
	if (!nRetCode)
		goto Exit0;
	// 获取目标坐标
	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	aNpc.SendCommand(do_walk, nDesX, nDesY);

	nResult = true;
Exit0:
	return nResult;
}

// 鱼类型
// done
int KNpcAI::ShowNpcType13()
{
	int nResult  = false;
	int nRetCode = false;
	// Go the distance between P1 to P2	
	int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
	int nCurY = 0;
	int nOffX = 0;
	int nOffY = 0;
	int nOffsetDir = 0;
	int nIndex = 0;
	KNpc& aNpc = Npc[m_nIndex];

	aNpc.GetMpsPos(&nCurX, &nCurY);

	// 计算新角度 和 距离
	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);

	// 取得运动所需的时间，保存在参数表中
	if (aNpc.m_CurrentWalkSpeed > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / (int) aNpc.m_CurrentWalkSpeed;
		aNpc.m_AiParam[5] = 0;
	}
	else
	{
		aNpc.m_AiParam[4] = 0;
		aNpc.m_AiParam[5] = 0;
	}
	//if (KeepActiveShowRange())
	//	aNpc.m_Dir += 32;
	// 附近有玩家
	nIndex = IsPlayerCome();
	if (nIndex > 0)
	{
		// do flee
		DoShowFlee(nIndex);
		goto Exit0;
	}
	// 根据三角函数计算偏移的X、Y数值
	nRetCode = GetNpcMoveOffset(aNpc.m_Dir, nDistance, &nOffX, &nOffY);
	// 获取目标坐标
	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	aNpc.SendCommand(do_walk, nDesX, nDesY);

	nResult = true;
Exit0:
	return nResult;
}

// 老鼠型
// done
int KNpcAI::ShowNpcType14()
{
	int nResult  = false;
	int nRetCode = false;

	int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
	int nCurY = 0;
	int nOffX = 0;
	int nOffY = 0;
	int nRandom = 0;
	int nOffsetDir = 0;
	KNpc& aNpc = Npc[m_nIndex];

	nRandom = GetRandomNumber(1, 10);
	// 掉头就跑
	if (nRandom < 4)
		nDistance = -nDistance;
	// 嗷嗷发呆
	else if (nRandom < 7)
	{
		aNpc.SendCommand(do_stand);
		goto Exit0;
	}
	aNpc.GetMpsPos(&nCurX, &nCurY);
	// 计算新角度 和 距离
	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);
	// 取得运动所需的时间，保存在参数表中
	if (aNpc.m_CurrentWalkSpeed > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / (int) aNpc.m_CurrentWalkSpeed;
		aNpc.m_AiParam[5] = 0;
	}
	else
	{
		aNpc.m_AiParam[4] = 0;
		aNpc.m_AiParam[5] = 0;
	}

	aNpc.m_Dir += nOffsetDir;
	if (aNpc.m_Dir < 0)
		aNpc.m_Dir += 64;
	else
		aNpc.m_Dir %= 64;
	// 根据三角函数计算偏移的X、Y数值
	nRetCode = GetNpcMoveOffset(aNpc.m_Dir, nDistance, &nOffX, &nOffY);
	if (!nRetCode)
		goto Exit0;
	// 获取目标坐标
	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	aNpc.SendCommand(do_walk, nDesX, nDesY);

	nResult = true;
Exit0:
	return nResult;
}

// 鸡犬型
int KNpcAI::ShowNpcType15()
{
	int nResult  = false;
	int nRetCode = false;
	// Go the distance between P1 to P2	
	int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
	int nCurY = 0;
	int nOffX = 0;
	int nOffY = 0;
	int nOffsetDir = 0;
	int nIndex = 0;
	KNpc& aNpc = Npc[m_nIndex];

	aNpc.GetMpsPos(&nCurX, &nCurY);

	// 计算新角度 和 距离
	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);

	// 取得运动所需的时间，保存在参数表中
	if (aNpc.m_CurrentWalkSpeed > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / (int) aNpc.m_CurrentWalkSpeed;
		aNpc.m_AiParam[5] = 0;
	}
	else
	{
		aNpc.m_AiParam[4] = 0;
		aNpc.m_AiParam[5] = 0;
	}
	//if (KeepActiveShowRange())
	//	aNpc.m_Dir += 32;
	// 附近有玩家
	nIndex = IsPlayerCome();
	if (nIndex > 0)
	{
		// do flee
		DoShowFlee(nIndex);
		goto Exit0;
	}
	// 根据三角函数计算偏移的X、Y数值
	nRetCode = GetNpcMoveOffset(aNpc.m_Dir, nDistance, &nOffX, &nOffY);
	// 获取目标坐标
	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	aNpc.SendCommand(do_walk, nDesX, nDesY);

	nResult = true;
Exit0:
	return nResult;
}

// 兔子型
int KNpcAI::ShowNpcType16()
{
	int nResult  = false;
	int nRetCode = false;

	// Go the distance between P1 to P2
	register int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
	int nCurY = 0;
	int nOffX = 0;
	int nOffY = 0;
	int nOffsetDir = 0;
	int nIndex = 0;
	KNpc& aNpc = Npc[m_nIndex];

	aNpc.GetMpsPos(&nCurX, &nCurY);

	// 计算新角度 和 距离
	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);
	// 附近有玩家
	nIndex = IsPlayerCome();
	if (nIndex > 0)
	{
		// do flee
		nRetCode = DoShowFlee(nIndex);
		if (!nRetCode)
			goto Exit0;		
		goto Exit1;
	}

	// 计算距离
	if (aNpc.m_CurrentWalkSpeed > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / (int) aNpc.m_CurrentWalkSpeed;
		aNpc.m_AiParam[5] = 0;
	}
	else
	{
		aNpc.m_AiParam[4] = 0;
		aNpc.m_AiParam[5] = 0;
	}

	// 计算新角度
	//if (KeepActiveShowRange())
	//	aNpc.m_Dir += 32;
	aNpc.m_Dir += GetRandomNumber(0, 6);
	aNpc.m_Dir %= 64;
	// 根据三角函数计算偏移的X、Y数值
	nRetCode = GetNpcMoveOffset(aNpc.m_Dir, nDistance, &nOffX, &nOffY);
	if (!nRetCode)
		goto Exit0;
	// 获取目标坐标
	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	aNpc.SendCommand(do_walk, nDesX, nDesY);

Exit1:	
	nResult = true;
Exit0:
	return nResult;
}

// 蝴蝶型
int KNpcAI::ShowNpcType17()
{
	int nResult  = false;
	int nRetCode = false;

	// Go the distance between P1 to P2
	int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
	int nCurY = 0;
	int nOffX = 0;
	int nOffY = 0;
	int nOffsetDir = 0;
	KNpc& aNpc = Npc[m_nIndex];

	// 效果加强 随机调整高度
	aNpc.m_Height = GetRandomNumber(aNpc.m_AiParam[6] - 4, aNpc.m_AiParam[6]);

	aNpc.GetMpsPos(&nCurX, &nCurY);
		
	// 计算新角度 和 距离
	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);	

	// 取得运动所需的时间，保存在参数表中
	if (aNpc.m_CurrentWalkSpeed > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / (int) aNpc.m_CurrentWalkSpeed;
		aNpc.m_AiParam[5] = 0;
	}
	else
	{
		aNpc.m_AiParam[4] = 0;
		aNpc.m_AiParam[5] = 0;
	}
	if (KeepActiveRange())
	{
		//aNpc.SendCommand(do_walk, aNpc.m_OriginX, aNpc.m_OriginY);
		goto Exit0;
		//aNpc.m_Dir += 32;
	}
	aNpc.m_Dir += nOffsetDir;
	//aNpc.m_Dir += GetRandomNumber(32, 64);
	if (aNpc.m_Dir < 0)
		aNpc.m_Dir += 64;
	else
		aNpc.m_Dir %= 64;
	// 根据三角函数计算偏移的X、Y数值
	nRetCode = GetNpcMoveOffset(aNpc.m_Dir, nDistance, &nOffX, &nOffY);
	if (!nRetCode)
		goto Exit0;
	// 获取目标坐标
	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	aNpc.SendCommand(do_walk, nDesX, nDesY);
	
	nResult = true;
Exit0:
	return nResult;
}
#endif
//---------------------------------------------------------------------
// Player AI add here.
// flying comment
void KNpcAI::ProcessPlayer()
{
#ifdef _SERVER
	TriggerObjectTrap();
	TriggerMapTrap();
#else
	int i = Npc[m_nIndex].m_nPeopleIdx;
	if (i > 0)
	{
		FollowPeople(i);
	}
	i = Npc[m_nIndex].m_nObjectIdx;
	if (i > 0)
	{
		FollowObject(i);
	}
#endif
}

BOOL KNpcAI::CheckNpc(int nIndex)
{
	if (nIndex < 0 || /*Npc[nIndex].m_SubWorldIndex != Npc[m_nIndex].m_SubWorldIndex || */
		Npc[nIndex].m_RegionIndex < 0 || 
		Npc[nIndex].m_CurrentLifeMax <= 0 || 
		Npc[nIndex].m_HideState.nTime > 0 || 
		!Npc[nIndex].IsAlive())
	{
		return TRUE;
	}
	return FALSE;
}

#ifndef _SERVER
void KNpcAI::FollowObject(int nIdx)
{
	if (Npc[m_nIndex].m_Doing == do_death || Npc[m_nIndex].m_Doing == do_revive)
	{
		Npc[m_nIndex].m_nObjectIdx = 0;
		return;
	}

    int nX1, nY1, nX2, nY2;
    Npc[m_nIndex].GetMpsPos(&nX1, &nY1);
    Object[nIdx].GetMpsPos(&nX2, &nY2);

    
    int deltaX = nX1 - nX2;
    int deltaY = nY1 - nY2;
    int distanceSquared = deltaX * deltaX + deltaY * deltaY;

    
    int maxDistanceSquared = PLAYER_PICKUP_CLIENT_DISTANCE * PLAYER_PICKUP_CLIENT_DISTANCE;
    if (distanceSquared < maxDistanceSquared)
    {
        Player[CLIENT_PLAYER_INDEX].CheckObject(nIdx);
    }
}
#endif

#ifndef _SERVER
void KNpcAI::FollowPeople(int nIdx)
{
	if (CheckNpc(nIdx))
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		return;
	}

	if (Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		return;
	}

	if (Npc[m_nIndex].m_Doing == do_death || Npc[m_nIndex].m_Doing == do_revive)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		return;
	}

	int distance = NpcSet.GetDistance(nIdx, m_nIndex);
	int	nRelation = NpcSet.GetRelation(m_nIndex, nIdx);

	if ((Npc[nIdx].m_Kind == kind_dialoger))
	{
		if (distance <= Npc[nIdx].m_DialogRadius)
		{
			int x, y;
			SubWorld[Npc[m_nIndex].m_SubWorldIndex].Map2Mps(Npc[m_nIndex].m_RegionIndex, Npc[m_nIndex].m_MapX, Npc[m_nIndex].m_MapY, Npc[m_nIndex].m_OffX, Npc[m_nIndex].m_OffY, &x, &y);
			Npc[m_nIndex].SendCommand(do_walk, x,y);
			
			// Send Command to Server
			SendClientCmdWalk(x, y);
			Player[CLIENT_PLAYER_INDEX].DialogNpc(nIdx);
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = 0;
			Npc[nIdx].TurnTo(Player[CLIENT_PLAYER_INDEX].m_nIndex);
			//
			return;
		}
	}

	// 距离小于攻击范围就开始攻击
	if (nRelation == relation_enemy)
	{
		ISkill * pSkillCheck = Npc[m_nIndex].GetActiveSkill();
		if(pSkillCheck)
		{
			if (pSkillCheck->GetAttackRadius() >= Npc[m_nIndex].m_CurrentAttackRadius)
			{
				if (distance <= Npc[m_nIndex].m_CurrentAttackRadius)
				{
					Npc[m_nIndex].SendCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, nIdx);

					// Send to Server
					SendClientCmdSkill(Npc[m_nIndex].m_ActiveSkillID, -1, Npc[nIdx].m_dwID);
				}
				// 嗷嗷追
				else
				{
					int nDesX, nDesY;
					Npc[nIdx].GetMpsPos(&nDesX, &nDesY);
					// modify by spe 2003/06/13
					if (Player[CLIENT_PLAYER_INDEX].m_RunStatus)
					{
						if (Npc[m_nIndex].m_CurrentStamina > 0 || Player[CLIENT_PLAYER_INDEX].m_cPK.GetNormalPKState() != enumPKMurder)
						{
							Npc[m_nIndex].SendCommand(do_run, nDesX, nDesY);			
							SendClientCmdRun(nDesX, nDesY);
						}
						else
						{
							Npc[m_nIndex].SendCommand(do_walk, nDesX, nDesY);
							SendClientCmdWalk(nDesX, nDesY);
						}
					}
					else
					{
						Npc[m_nIndex].SendCommand(do_walk, nDesX, nDesY);
						SendClientCmdWalk(nDesX, nDesY);
					}
				}
			}
		}
		return;
	}

	if (Npc[nIdx].m_Kind == kind_player)
	{
		// flow
		int nDesX, nDesY;
		if (distance < MAX_FOLLOW_DISTANCE)
		{
			Npc[this->m_nIndex].GetMpsPos(&nDesX, &nDesY);
			Npc[m_nIndex].SendCommand(do_walk, nDesX, nDesY);
			SendClientCmdWalk(nDesX, nDesY);
		}
		else
		{
			Npc[nIdx].GetMpsPos(&nDesX, &nDesY);
			if (distance < FOLLOW_WALK_DISTANCE ||
				!Player[CLIENT_PLAYER_INDEX].m_RunStatus)
			{
				Npc[m_nIndex].SendCommand(do_walk, nDesX, nDesY);
				SendClientCmdWalk(nDesX, nDesY);
			}
			else
			{
				Npc[m_nIndex].SendCommand(do_run, nDesX, nDesY);			
				SendClientCmdRun(nDesX, nDesY);
			}
		}
	}
	return;
}
#endif

void KNpcAI::TriggerMapTrap()
{
	Npc[m_nIndex].CheckTrap();
}

void KNpcAI::TriggerObjectTrap()
{
	return;
}

int KNpcAI::GetNearestNpc(int nRelation)
{
    int nRangeX = Npc[m_nIndex].m_VisionRadius / SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_nCellWidth;
    int nRangeY = nRangeX;
    int nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
    int nRegion = Npc[m_nIndex].m_RegionIndex;
    int nMapX = Npc[m_nIndex].m_MapX;
    int nMapY = Npc[m_nIndex].m_MapY;

    for (int i = 0; i < nRangeX; i++)
    {
        for (int j = 0; j < nRangeY; j++)
        {
            if ((i * i + j * j) > nRangeX * nRangeX)
                continue;

            for (int dx = -1; dx <= 1; dx += 2)
            {
                for (int dy = -1; dy <= 1; dy += 2)
                {
                    int nRMx = nMapX + dx * i;
                    int nRMy = nMapY + dy * j;
                    int nSearchRegion = nRegion;
					if (nSearchRegion == -1)
						continue;

                    if (nRMx < 0)
                    {
                        nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
                        nRMx += SubWorld[nSubWorld].m_nRegionWidth;
                    }
                    else if (nRMx >= SubWorld[nSubWorld].m_nRegionWidth)
                    {
                        nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
                        nRMx -= SubWorld[nSubWorld].m_nRegionWidth;
                    }
					if (nSearchRegion == -1)
						continue;

                    if (nRMy < 0)
                    {
                        nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
                        nRMy += SubWorld[nSubWorld].m_nRegionHeight;
                    }
                    else if (nRMy >= SubWorld[nSubWorld].m_nRegionHeight)
                    {
                        nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
                        nRMy -= SubWorld[nSubWorld].m_nRegionHeight;
                    }

                    if (nSearchRegion == -1)
                        continue;

                    int nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, nRelation);
                    if (nRet > 0 && Npc[nRet].m_HideState.nTime == 0)
                        return nRet;
                }
            }
        }
    }
    return 0;
}

#ifndef _SERVER
// flying add this
// 查找离某个NPC最近的玩家
int KNpcAI::IsPlayerCome()
{
	int nResult = 0;
	int nPlayer = 0;
	int X1 = 0;
	int Y1 = 0;
	int X2 = 0;
	int Y2 = 0;
	int nDistance = 0;

	nPlayer = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	nDistance = NpcSet.GetDistance(nPlayer, m_nIndex);

	if ( nDistance == 0) nDistance = 1;
	// 鸡看的到的玩家
	if (nDistance < Npc[m_nIndex].m_VisionRadius)
	{
		// 分别处理走和跑
		if (Player[CLIENT_PLAYER_INDEX].m_RunStatus ||
			Npc[m_nIndex].m_CurrentVisionRadius > nDistance * 4)
		{
			nResult = nPlayer;
		}
	}
	return nResult;
}
#endif

int KNpcAI::GetNpcNumber(int nRelation)
{
	int nRangeX = Npc[m_nIndex].m_VisionRadius;
	int	nRangeY = nRangeX;
	int	nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	int	nRegion = Npc[m_nIndex].m_RegionIndex;
	int	nMapX = Npc[m_nIndex].m_MapX;
	int	nMapY = Npc[m_nIndex].m_MapY;
	int	nRet = 0;
	int	nRMx, nRMy, nSearchRegion;

	if  (nSubWorld<0 || nRegion<0)
		return 0;

	nRangeX = nRangeX / SubWorld[nSubWorld].m_nCellWidth;
	nRangeY = nRangeY / SubWorld[nSubWorld].m_nCellHeight;

	// 检查视野范围内的格子里的NPC
	for (int i = -nRangeX; i < nRangeX; i++)
	{
		for (int j = -nRangeY; j < nRangeY; j++)
		{
			// 去掉边角几个格子，保证视野是椭圆形
			if ((i * i + j * j) > nRangeX * nRangeX)
				continue;

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX + i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;

			if (nSearchRegion == -1)
				continue;

			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorld].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorld].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorld].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorld].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorld].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorld].m_nRegionHeight;
			}

			if (nSearchRegion == -1)
				continue;
			int nNpcIdx = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, nRelation);
			if (nNpcIdx > 0)
				nRet++;
		}
	}
	return nRet;
}

void KNpcAI::KeepAttackRange(int nEnemy, int nRange)
{
	int nX1, nY1, nX2, nY2, nDir, nWantX, nWantY;

	Npc[m_nIndex].GetMpsPos(&nX1, &nY1);
	Npc[nEnemy].GetMpsPos(&nX2, &nY2);
	nDir = g_GetDirIndex(nX1, nY1, nX2, nY2);

	nWantX = nX2 - ((nRange * g_DirCos(nDir, 64)) >> 10);
	nWantY = nY2 - ((nRange * g_DirSin(nDir, 64)) >> 10);

	Npc[m_nIndex].SendCommand(do_walk, nWantX, nWantY);
}

void KNpcAI::FollowAttack(int i)
{
	AUTOLOG_EVERY(500, "[E4_AI_TARGET_CHECK] g_DebugLog(\"[E4_AI_TARGET_CHECK] me=%d tgt=%d rgn=%d life=%d/%d hide=%d doing=%d\", m_nIndex, i, Npc[i].m_RegionIndex, Npc[i].m_CurrentLife, Npc[i].m_CurrentLifeMax, Npc[i].m_HideState.nTime, (int)Npc[i].m_Doing);", m_nIndex, i, Npc[i].m_RegionIndex, Npc[i].m_CurrentLife, Npc[i].m_CurrentLifeMax, Npc[i].m_HideState.nTime, (int)Npc[i].m_Doing);
	if (CheckNpc(i))
		return;
	//
	if ( Npc[i].m_RegionIndex < 0 )
		return;
	//
	int distance = NpcSet.GetDistance(m_nIndex, i);

	if ( distance == 0) 
		distance = 1;

//#define	MINI_ATTACK_RANGE	32
//
//	if (distance <= MINI_ATTACK_RANGE)
//	{
//		KeepAttackRange(i, MINI_ATTACK_RANGE);
//		return;
//	}
	// Attack Enemy
	AUTOLOG_EVERY(500, "[E4_AI_RANGE] g_DebugLog(\"[E4_AI_RANGE] me=%d tgt=%d dist=%d atkradius=%d vision=%d skill=%d doing=%d\", m_nIndex, i, distance, Npc[m_nIndex].m_CurrentAttackRadius, Npc[m_nIndex].m_VisionRadius, Npc[m_nIndex].m_ActiveSkillID, (int)Npc[m_nIndex].m_Doing);", m_nIndex, i, distance, Npc[m_nIndex].m_CurrentAttackRadius, Npc[m_nIndex].m_VisionRadius, Npc[m_nIndex].m_ActiveSkillID, (int)Npc[m_nIndex].m_Doing);
	if (distance <= Npc[m_nIndex].m_CurrentAttackRadius && InEyeshot(i))
	{
		ISkill * pISkill =  g_SkillManager.GetSkill(Npc[m_nIndex].m_ActiveSkillID, 1);
		if (!pISkill) 
            return;
		if (pISkill->IsAura())
			return;
		if (pISkill->GetSkillStyle() == SKILL_SS_Missles && (pISkill->IsTargetAlly() /*|| pISkill->IsTargetSelf()*/))
		{
			int nX;
			int nY;
			Npc[m_nIndex].GetMpsPos(&nX, &nY);
			Npc[m_nIndex].SendCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, nX, nY);
			return;
		}
		AUTOLOG_EVERY(300, "[E4_AI_ATTACK_CMD] g_DebugLog(\"[E4_AI_ATTACK_CMD] me=%d tgt=%d skill=%d dist=%d atkradius=%d\", m_nIndex, i, Npc[m_nIndex].m_ActiveSkillID, distance, Npc[m_nIndex].m_CurrentAttackRadius);", m_nIndex, i, Npc[m_nIndex].m_ActiveSkillID, distance, Npc[m_nIndex].m_CurrentAttackRadius);
		Npc[m_nIndex].SendCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, i);
		return;
	}

	// Move to Enemy
	int x, y;
	Npc[i].GetMpsPos(&x, &y);

	AUTOLOG_EVERY(1000, "[E4_AI_OUTOFRANGE] g_DebugLog(\"[E4_AI_OUTOFRANGE] me=%d tgt=%d dist=%d atkradius=%d vision=%d to=(%d,%d)\", m_nIndex, i, distance, Npc[m_nIndex].m_CurrentAttackRadius, Npc[m_nIndex].m_VisionRadius, x, y);", m_nIndex, i, distance, Npc[m_nIndex].m_CurrentAttackRadius, Npc[m_nIndex].m_VisionRadius, x, y);
	Npc[m_nIndex].SendCommand(do_walk, x, y);
}

BOOL KNpcAI::InEyeshot(int nIdx)//trong t莔 quan s竧
{
	int distance = NpcSet.GetDistance(nIdx, m_nIndex);

	return (Npc[m_nIndex].m_VisionRadius > distance);
}

void KNpcAI::CommonAction()
{
	// If it is a dialogue NPC, just stay where you are.
	if (Npc[m_nIndex].m_Kind == kind_dialoger)
	{
		Npc[m_nIndex].SendCommand(do_stand);
		return;
	}
	int	nOffX, nOffY;
	if (g_RandPercent(80))
	{
		nOffX = 0;
		nOffY = 0;
	}
	else
	{
		
		nOffX = g_Random(Npc[m_nIndex].m_CurrentActiveRadius / 2);	//Active radius
		nOffY = g_Random(Npc[m_nIndex].m_CurrentActiveRadius / 2);	//Active radius
		if (nOffX & 1)
		{
			nOffX = - nOffX;
		}
		if (nOffY & 1)
		{
			nOffY = - nOffY;
		}
	}
	if (Npc[m_nIndex].IsCanInput()) { //Process AI command if AI type
		int x, y;

		Npc[m_nIndex].GetMpsPos(&x, &y);
		int	nRange = g_GetDistance(Npc[m_nIndex].GetCommand().Param_X, Npc[m_nIndex].GetCommand().Param_Y, x, y);
		if(nRange > 3)
			Npc[m_nIndex].ProcCommand(1);
		else
			Npc[m_nIndex].SendCommand(do_stand);
	}
	else
		Npc[m_nIndex].SendCommand(do_walk, Npc[m_nIndex].m_OriginX + nOffX, Npc[m_nIndex].m_OriginY + nOffY);
}

BOOL KNpcAI::KeepActiveRange()
{
	int x, y;
	
	Npc[m_nIndex].GetMpsPos(&x, &y);
	int	nRange = g_GetDistance(Npc[m_nIndex].m_OriginX, Npc[m_nIndex].m_OriginY, x, y);

	// If you find that you are beyond the activity range, narrow the current activity range
	// to avoid swinging back and forth at the edge of the activity range.
	if (Npc[m_nIndex].m_ActiveRadius < nRange)	//Active radius
	{
		Npc[m_nIndex].m_CurrentActiveRadius = Npc[m_nIndex].m_ActiveRadius / 2;
	}

	// If you find that you are out of the current activity range, go back
	AUTOLOG_EVERY(1000, "[E4_AI_LEASH] g_DebugLog(\"[E4_AI_LEASH] me=%d cur=(%d,%d) origin=(%d,%d) range=%d curactive=%d active=%d\", m_nIndex, x, y, Npc[m_nIndex].m_OriginX, Npc[m_nIndex].m_OriginY, nRange, Npc[m_nIndex].m_CurrentActiveRadius, Npc[m_nIndex].m_ActiveRadius);", m_nIndex, x, y, Npc[m_nIndex].m_OriginX, Npc[m_nIndex].m_OriginY, nRange, Npc[m_nIndex].m_CurrentActiveRadius, Npc[m_nIndex].m_ActiveRadius);
	if (Npc[m_nIndex].m_CurrentActiveRadius < nRange)	//Active radius
	{
		Npc[m_nIndex].SendCommand(do_walk, Npc[m_nIndex].m_OriginX, Npc[m_nIndex].m_OriginY);
		return TRUE;
	}
	else	// In the current active range, restore the current active range size.
	{
		Npc[m_nIndex].m_CurrentActiveRadius = Npc[m_nIndex].m_ActiveRadius;	//Active radius
		return FALSE;
	}
}

#ifndef _SERVER
// 15/16 AiMode NPC的逃逸动作
int KNpcAI::DoShowFlee(int nIdx)
{
	int nResult  = false;
	int nRetCode = false;
	
	int x1, y1, x2, y2;
	int nDistance = Npc[m_nIndex].m_AiParam[6];

	Npc[m_nIndex].GetMpsPos(&x1, &y1);
	//Npc[nIdx].GetMpsPos(&x2, &y2);
	Npc[m_nIndex].m_Dir = Npc[nIdx].m_Dir;
	nRetCode = GetNpcMoveOffset(Npc[m_nIndex].m_Dir, nDistance, &x2, &y2);
	if (!nRetCode)
		goto Exit0;
	Npc[m_nIndex].m_AiParam[4] = (int) nDistance / Npc[m_nIndex].m_WalkSpeed;
	Npc[m_nIndex].m_AiParam[5] = 0;
	Npc[m_nIndex].SendCommand(do_walk, x1 + x2, y1 + y2);

	nResult = true;
Exit0:
	return nResult;
}

#endif

// 逃离Npc[nIdx]
void KNpcAI::Flee(int nIdx)
{
	int x1, y1, x2, y2;

	Npc[m_nIndex].GetMpsPos(&x1, &y1);
	Npc[nIdx].GetMpsPos(&x2, &y2);

	x1 = x1 * 2 - x2;
	y1 = y1 * 2 - y2;

	Npc[m_nIndex].SendCommand(do_walk, x1, y1);
}

//------------------------------------------------------------------------------
// Function: Normal active class 1
// m_AiParam[0] Patrol probability when there are no enemies
// m_AiParam[1, 2, 3, 4] Probability of using four skills, corresponding to skills 1 2 3 4 in the SkillList
// m_AiParam[5, 6] Probability of Standby and Patrol when the enemy is seen but far away
//------------------------------------------------------------------------------
void	KNpcAI::ProcessAIType01()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;
	// Has the activity radius been exceeded?
	if (KeepActiveRange())
		return;

	int nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;
	// If the enemy is not locked or runs too far away, re-lock the enemy
	if (nEnemyIdx <= 0 || Npc[nEnemyIdx].m_dwID <= 0 || !InEyeshot(nEnemyIdx) )
	{
		nEnemyIdx = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = nEnemyIdx;
	}

	//There are no enemies around, with a certain probability of Standby/Patrol
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]: Patrol probability
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// Patrol
			CommonAction();
		}
		return;
	}

	// If the enemy is out of the attack range of all skills, there is a 
	// certain probability to choose Standby/Patrol/Approach the enemy
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1])
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[5])	// Standby
			return;
		if (nRand < pAIParam[5] + pAIParam[6])	// Patrol
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// Approach the enemy
		return;
	}

	// When the enemy is within the maximum skill attack range, select a skill to attack
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[1])
	{
		if (!Npc[m_nIndex].SetActiveSkill(1))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3] + pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(4))
		{
			CommonAction();
			return;
		}
	}
	else	// Standby
	{
		return;
	}

	FollowAttack(nEnemyIdx);
}


//------------------------------------------------------------------------------
// Function: Normal active class 2
// m_AiParam[0] Patrol probability when there are no enemies
// m_AiParam[1] Execute corresponding processing when the remaining health is lower than this percentage
// m_AiParam[2] Probability of executing corresponding processing when the situation in m_AiParam[1] occurs
// m_AiParam[3] Probability of using recovery skills when the situation in m_AiParam[1] occurs and decides to execute corresponding processing Corresponding to the skills in SkillList 1
// m_AiParam[4, 5, 6] Probability of using three attack skills, corresponding to the skills in SkillList 2 3 4
// m_AiParam[7, 8] Probability of Standby and Patrol when the enemy is seen but far away
//------------------------------------------------------------------------------
void	KNpcAI::ProcessAIType02()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;
	// Has the activity radius been exceeded?
	if (KeepActiveRange())
		return;

	int nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;
	// If the enemy is not locked or runs too far away, re-lock the enemy
	if (nEnemyIdx <= 0 || Npc[nEnemyIdx].m_dwID <= 0 || !InEyeshot(nEnemyIdx) )
	{
		nEnemyIdx = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = nEnemyIdx;
	}

	// There are no enemies around, with a certain probability of Standby/Patrol
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:Patrol Probability
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// Patrol
			CommonAction();
		}
		return;
	}

	// Check whether the remaining life meets the conditions. If the life is too low,
	// there is a certain probability of using blood replenishment skills or running away.
	if (Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax < pAIParam[1])
	{
		if (g_RandPercent(pAIParam[2]))	// Whether to use blood-replenishing skills or escape
		{
			if (Npc[m_nIndex].m_AiAddLifeTime < pAIParam[9] && g_RandPercent(pAIParam[3]))	// Use blood replenishment skills
			{
				Npc[m_nIndex].SetActiveSkill(1);
				Npc[m_nIndex].SendCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, m_nIndex);
				Npc[m_nIndex].m_AiAddLifeTime++;
				return;
			}
			else	// Escape
			{
				Flee(nEnemyIdx);
				return;
			}
		}
	}

	// If the enemy is out of the attack range of all skills, there is a certain probability to choose Standby/Patrol/Approach the enemy
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1])
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[7])	// Standby
			return;
		if (nRand < pAIParam[7] + pAIParam[8])	// Patrol
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// Approach the enemy
		return;
	}

	// When the enemy is within the maximum skill attack range, select a skill to attack
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5] + pAIParam[6])
	{
		if (!Npc[m_nIndex].SetActiveSkill(4))
		{
			CommonAction();
			return;
		}
	}
	else	// Standby
	{
		return;
	}
	FollowAttack(nEnemyIdx);
}

//------------------------------------------------------------------------------
// Function: Normal active class 3
// m_AiParam[0] Patrol probability when there are no enemies
// m_AiParam[1] Execute corresponding processing when the remaining health is lower than this percentage
// m_AiParam[2] Probability of executing corresponding processing when the situation in m_AiParam[1] occurs
// m_AiParam[3] Probability of using attack skills when the situation in m_AiParam[1] occurs and decides to execute corresponding processing Corresponding to the skills in SkillList 1
// m_AiParam[4, 5, 6] Probability of using three attack skills, corresponding to the skills in SkillList 2 3 4
// m_AiParam[7, 8] Probability of Standby and Patrol when the enemy is seen but far away
//------------------------------------------------------------------------------
void	KNpcAI::ProcessAIType03()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;
	// Has the activity radius been exceeded?
	if (KeepActiveRange())
		return;

	int nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;
	// If the enemy is not locked or runs too far away, re-lock the enemy
	if (nEnemyIdx <= 0 || Npc[nEnemyIdx].m_dwID <= 0 || !InEyeshot(nEnemyIdx) )
	{
		nEnemyIdx = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = nEnemyIdx;
	}

	// There are no enemies around, with a certain probability of Standby/Patrol
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:Patrol probability
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// Patrol
			CommonAction();
		}
		return;
	}

	// Check whether the remaining life meets the conditions. If the life is too low, 
	// there is a certain probability of using attack skills or running away.
	if (Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax < pAIParam[1])
	{
		if (g_RandPercent(pAIParam[2]))	// Whether to use attack skills or escape
		{
			if (g_RandPercent(pAIParam[3]))	// Use attack skills
			{
				Npc[m_nIndex].SetActiveSkill(1);
				FollowAttack(nEnemyIdx);
				return;
			}
			else	// Escape
			{
				Flee(nEnemyIdx);
				return;
			}
		}
	}

	// If the enemy is out of the attack range of all skills, there is a certain probability to choose Standby/Patrol/Approach the enemy
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1])
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[7])	// Standby
			return;
		if (nRand < pAIParam[7] + pAIParam[8])	// Patrol
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// Approach the enemy
		return;
	}

	// When the enemy is within the maximum skill attack range, select a skill to attack
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5] + pAIParam[6])
	{
		if (!Npc[m_nIndex].SetActiveSkill(4))
		{
			CommonAction();
			return;
		}
	}
	else	// Standby
	{
		return;
	}
	FollowAttack(nEnemyIdx);
}

//------------------------------------------------------------------------------
// Function: Common passive class 1
// m_AiParam[0] Patrol probability when there is no enemy
// m_AiParam[1, 2, 3, 4] Probability of using four attack skills, corresponding to the skills in SkillList 1 2 3 4
// m_AiParam[5, 6] Probability of Standby and Patrol when the enemy is seen but far away
//------------------------------------------------------------------------------
void	KNpcAI::ProcessAIType04()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;

	int nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;
	// Is it under attack? If not, choose Standby/Patrol with a certain probability
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:Patrol probability
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// Patrol
			CommonAction();
		}
		return;
	}

	// Has the activity radius been exceeded?
	if (KeepActiveRange())
		return;

	// If the enemy is out of the attack range of all skills, there is a certain probability to choose Standby/Patrol/Approach the enemy
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1])
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[5])	// Standby
			return;
		if (nRand < pAIParam[5] + pAIParam[6])	// Patrol
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// Approach the enemy
		return;
	}

	// When the enemy is within the maximum skill attack range, select a skill to attack
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[1])
	{
		if (!Npc[m_nIndex].SetActiveSkill(1))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3] + pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(4))
		{
			CommonAction();
			return;
		}
	}
	else	// Standby
	{
		return;
	}
	FollowAttack(nEnemyIdx);
}

//------------------------------------------------------------------------------
// Function: Common passive class 2
// m_AiParam[0] Probability of Patrol when there are no enemies
// m_AiParam[1] Execute the corresponding action when the remaining health is lower than this percentage
// m_AiParam[2] Probability of whether to execute the corresponding action when the situation in m_AiParam[1] occurs
// m_AiParam[3] Probability of using the recovery skill when the situation in m_AiParam[1] occurs and the corresponding action is decided to be executed Corresponding to the skills in the SkillList 1
// m_AiParam[4, 5, 6] Probability of using the three attack skills, corresponding to the skills in the SkillList 2 3 4
// m_AiParam[7, 8] Probability of Standby and Patrol when the enemy is seen but far away
//------------------------------------------------------------------------------
void	KNpcAI::ProcessAIType05()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;

	int nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;
	// 是否受到攻击，否，一定概率选择Standby/Patrol
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:Patrol probability
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// Patrol
			CommonAction();
		}
		return;
	}

	// Has the activity radius been exceeded?
	if (KeepActiveRange())
		return;

	// Check whether the remaining life meets the conditions. If the life is too low, there is a certain probability of using blood replenishment skills or running away.
	if (Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax < pAIParam[1])
	{
		if (g_RandPercent(pAIParam[2]))	// Whether to use blood-replenishing skills or escape
		{
			if (Npc[m_nIndex].m_AiAddLifeTime < pAIParam[9] && g_RandPercent(pAIParam[3]))	// Use blood replenishment skills
			{
				Npc[m_nIndex].m_AiAddLifeTime++;
				Npc[m_nIndex].SetActiveSkill(1);
				Npc[m_nIndex].SendCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, m_nIndex);
				return;
			}
			else	// Escape
			{
				Flee(nEnemyIdx);
				return;
			}
		}
	}

	// If the enemy is out of the attack range of all skills, there is a certain probability to choose Standby/Patrol/Approach the enemy
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1])
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[7])	// Standby
			return;
		if (nRand < pAIParam[7] + pAIParam[8])	// Patrol
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// Approach the enemy
		return;
	}

	// When the enemy is within the maximum skill attack range, select a skill to attack
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5] + pAIParam[6])
	{
		if (!Npc[m_nIndex].SetActiveSkill(4))
		{
			CommonAction();
			return;
		}
	}
	else	// Standby
	{
		return;
	}
	FollowAttack(nEnemyIdx);
}

//------------------------------------------------------------------------------
// Function: Common passive class 3
// m_AiParam[0] Probability of patrolling when there are no enemies
// m_AiParam[1] Execute corresponding processing when the remaining health is lower than this percentage
// m_AiParam[2] Probability of executing corresponding processing when the situation in m_AiParam[1] occurs
// m_AiParam[3] Probability of using attack skills when the situation in m_AiParam[1] occurs and decides to execute corresponding processing Corresponding to the skills in SkillList 1
// m_AiParam[4, 5, 6] Probability of using three attack skills, corresponding to the skills in SkillList 2 3 4
// m_AiParam[7, 8] Probability of waiting and patrolling when the enemy is seen but far away
//------------------------------------------------------------------------------
void	KNpcAI::ProcessAIType06()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;

	int nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;
    if (Npc[nEnemyIdx].m_Doing == do_death || Npc[nEnemyIdx].m_Doing == do_revive || Npc[nEnemyIdx].m_HideState.nTime >0) 
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		 nEnemyIdx=0;
	}
	//Is it attacked? If not, it will choose standby/patrol with a certain probability.
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:Patrol probability
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// patrol
			CommonAction();
		}
		return;
	}

	if (Npc[nEnemyIdx].m_Kind==kind_player && Npc[nEnemyIdx].m_FightMode==0)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		nEnemyIdx=0;
	}

	// Has the activity radius been exceeded?
	if (KeepActiveRange())
		return;

	// Check whether the remaining life meets the conditions. 
	// If the life is too low, there is a certain probability of using attack skills or running away.
	if (Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax < pAIParam[1])
	{
		if (g_RandPercent(pAIParam[2]))	// Whether to use attack skills or escape
		{
			if (g_RandPercent(pAIParam[3]))	// Use attack skills
			{
				Npc[m_nIndex].SetActiveSkill(1);
				FollowAttack(nEnemyIdx);	// Approach the enemy
				return;
			}
			else	// Escape
			{
				Flee(nEnemyIdx);
				return;
			}
		}
	}

	// If the enemy is out of the attack range of all skills, there is 
	// a certain probability to choose to wait/patrol/approach the enemy
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1])
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[7])	// Standby
			return;
		if (nRand < pAIParam[7] + pAIParam[8])	// Patrol
		{
			CommonAction();
			return;
		}
		int   nSkillID;
		nSkillID=GetRandomNumber(1,4);
		if (!Npc[m_nIndex].SetActiveSkill(nSkillID))
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// Approach the enemy
		return;
	}

	// When the enemy is within the maximum skill attack range, select a skill to attack
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5] + pAIParam[6])
	{
		if (!Npc[m_nIndex].SetActiveSkill(4))
		{
			CommonAction();
			return;
		}
	}
	else	// Standby
	{
		return;
	}
	FollowAttack(nEnemyIdx);
}


//------------------------------------------------------------------------------
// Function: Normal active class 1 variant (ProcessAIType1)
// m_AiParam[0] - Health percentage threshold to consider using skill 1 (e.g. heal or special skill when low HP)
// m_AiParam[1] - Probability (percent) to use skill 1 when HP is below threshold
// m_AiParam[2], m_AiParam[3], m_AiParam[4] - Probabilities of using skills 2, 3, and 4 respectively during normal attack phase
//
// Description:
// This AI process handles NPC actions including:
// - Checking if NPC is outside its active radius (then skip further processing)
// - Using skill 1 when health is below a threshold, based on a random chance
// - Identifying and locking onto the nearest enemy if no current valid enemy
// - Selecting one of skills 2, 3, or 4 to use based on weighted probabilities
// - Falling back to a common default action if skill use fails or no skills are triggered
// - Following and attacking the locked enemy if an attack skill was successfully activated
//------------------------------------------------------------------------------
void KNpcAI::ProcessAIType1()
{
	int* pAIParam = Npc[m_nIndex].m_AiParam;

	// Check if NPC has moved outside its active radius; if so, skip action processing
	if (KeepActiveRange())
		return;

	// If NPC's current life is below the threshold (pAIParam[0]), attempt to use skill 1 with chance pAIParam[1]
	if (Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax < pAIParam[0])
	{
		if (g_RandPercent(pAIParam[1]))
		{
			// Set skill 1 active (usually healing or defensive skill) and send the command
			Npc[m_nIndex].SetActiveSkill(1);
			Npc[m_nIndex].SendCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, m_nIndex);
			return;
		}
	}

	// Get currently locked enemy index
	int nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;

	// If no valid enemy locked or enemy is no longer visible, find nearest enemy and lock it
	AUTOLOG_EVERY(1000, "[E4_AI_TARGET_PICK] g_DebugLog(\"[E4_AI_TARGET_PICK] me=%d locked=%d people=%d vision=%d skill=%d\", m_nIndex, nEnemyIdx, Npc[m_nIndex].m_nPeopleIdx, Npc[m_nIndex].m_VisionRadius, Npc[m_nIndex].m_ActiveSkillID);", m_nIndex, nEnemyIdx, Npc[m_nIndex].m_nPeopleIdx, Npc[m_nIndex].m_VisionRadius, Npc[m_nIndex].m_ActiveSkillID);
	if (nEnemyIdx <= 0 || Npc[nEnemyIdx].m_dwID <= 0 || !InEyeshot(nEnemyIdx))
	{
		nEnemyIdx = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = nEnemyIdx;
	}

	if (nEnemyIdx > 0)  // If an enemy is found
	{
		int nRand = g_Random(100);

		// Attempt to select skill 2, 3 or 4 based on weighted probabilities pAIParam[2], pAIParam[3], pAIParam[4]
		if (nRand < pAIParam[2])
		{
			if (!Npc[m_nIndex].SetActiveSkill(2))
			{
				CommonAction();  // fallback action if skill activation fails
				return;
			}
		}
		else if (nRand < pAIParam[2] + pAIParam[3])
		{
			if (!Npc[m_nIndex].SetActiveSkill(3))
			{
				CommonAction();
				return;
			}
		}
		else if (nRand < pAIParam[2] + pAIParam[3] + pAIParam[4])
		{
			if (!Npc[m_nIndex].SetActiveSkill(4))
			{
				CommonAction();
				return;
			}
		}

		// If none of the above triggered, check randomly for skill use again (probabilities pAIParam[2], [3], [4])
		if (g_RandPercent(pAIParam[2]))
		{
			Npc[m_nIndex].SetActiveSkill(2);
		}
		else if (g_RandPercent(pAIParam[3]))
		{
			Npc[m_nIndex].SetActiveSkill(3);
		}
		else if (g_RandPercent(pAIParam[4]))
		{
			Npc[m_nIndex].SetActiveSkill(4);
		}
		else
		{
			CommonAction();  // No skills used; fallback action
			return;
		}

		// Follow and attack the locked enemy
		FollowAttack(nEnemyIdx);
		return;
	}

	// If no enemy found, do a default action (likely idle or patrol)
	CommonAction();
}




//------------------------------------------------------------------------------
// Function: General passive AI (AI Type 2)
// m_AiParam[0] Health percentage threshold to trigger special skill usage
// m_AiParam[1] Probability to use skill 1 (likely a healing or defensive skill) when below health threshold
// m_AiParam[2] Probability to use skill 2 (attack skill) when enemy is in range
// m_AiParam[3] Probability to use skill 3 (attack skill) when enemy is in range
// m_AiParam[4] Probability to use skill 4 (attack skill) when enemy is in range
// Note: This AI type does not attempt to patrol or standby explicitly; it reacts mostly passively,
//       only acting when health is low or enemy is detected within eyeshot.
//------------------------------------------------------------------------------
void KNpcAI::ProcessAIType2()
{
	int* pAIParam = Npc[m_nIndex].m_AiParam;

	// Check if NPC is outside its active range; if so, stop processing
	if (KeepActiveRange())
		return;

	// If NPC's current health percentage is below threshold (pAIParam[0])...
	if (Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax < pAIParam[0])
	{
		// ... with probability pAIParam[1], use skill 1 (likely a healing or defensive skill)
		if (g_RandPercent(pAIParam[1]))
		{
			Npc[m_nIndex].SetActiveSkill(1);
			Npc[m_nIndex].SendCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, m_nIndex);
			return; // Exit after skill usage
		}
	}

	// Get the current enemy target index
	int nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;

	// If no valid enemy target or enemy is not within eyeshot, do nothing
	if (nEnemyIdx <= 0 || !InEyeshot(nEnemyIdx))
		return;

	// Generate random number 0-99 to decide which attack skill to use
	int nRand = g_Random(100);

	// Try skill 2 with probability pAIParam[2]
	if (nRand < pAIParam[2])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction(); // fallback if skill not usable
			return;
		}
	}
	// Else try skill 3 with probability pAIParam[3]
	else if (nRand < pAIParam[2] + pAIParam[3])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	// Else try skill 4 with probability pAIParam[4]
	else if (nRand < pAIParam[2] + pAIParam[3] + pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(4))
		{
			CommonAction();
			return;
		}
	}

	// If no skill chosen by above, try each skill with independent probabilities
	if (g_RandPercent(pAIParam[2]))
	{
		Npc[m_nIndex].SetActiveSkill(2);
	}
	else if (g_RandPercent(pAIParam[3]))
	{
		Npc[m_nIndex].SetActiveSkill(3);
	}
	else if (g_RandPercent(pAIParam[4]))
	{
		Npc[m_nIndex].SetActiveSkill(4);
	}
	else
	{
		CommonAction(); // fallback if no skill selected
		return;
	}

	// If a skill was selected successfully, attack the enemy
	FollowAttack(nEnemyIdx);

	return;
}




//------------------------------------------------------------------------------
// Function: General Escape AI (Type 3)
// m_AiParam[0] Health percentage threshold to trigger escape check
// m_AiParam[1] Probability to trigger escape when health is below threshold
// m_AiParam[2, 3, 4] Probability of using three different skills (Skill 1, Skill 2, Skill 3)
// Note: Unlike AI Types 1 and 2, this AI has no explicit patrol or standby probabilities when no enemy is found.
//       It focuses on escaping when health is low and skill usage probabilities when engaging.
//------------------------------------------------------------------------------
void KNpcAI::ProcessAIType3()
{
	int* pAIParam = Npc[m_nIndex].m_AiParam;

	// Check if NPC has exceeded its active range, stop processing if true
	if (KeepActiveRange())
		return;

	int nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;

	// If no enemy locked or enemy is out of sight, try to find nearest enemy and lock it
	if (nEnemyIdx <= 0 || !InEyeshot(nEnemyIdx))
	{
		nEnemyIdx = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = nEnemyIdx;
	}

	// If no enemy found after search, perform common default action (like idle or patrol)
	if (nEnemyIdx <= 0)
	{
		CommonAction();
		return;
	}

	// Check if NPC health is below threshold to consider escape behavior
	if (Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax < pAIParam[0])
	{
		// With given probability, execute fleeing from enemy instead of fighting
		if (g_RandPercent(pAIParam[1]))
		{
			Flee(nEnemyIdx);
			return;
		}
	}

	// Generate a random value for deciding skill usage
	int nRand = g_Random(100);

	// Attempt to use Skill 1, 2, or 3 based on cumulative probabilities in pAIParam[2..4]
	if (nRand < pAIParam[2])
	{
		if (!Npc[m_nIndex].SetActiveSkill(1))  // If skill setting fails, do default action
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[2] + pAIParam[3])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[2] + pAIParam[3] + pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}

	// If no skill was selected by the random value above, try individually by probability checks
	if (g_RandPercent(pAIParam[2]))
	{
		Npc[m_nIndex].SetActiveSkill(1);
	}
	else if (g_RandPercent(pAIParam[3]))
	{
		Npc[m_nIndex].SetActiveSkill(2);
	}
	else if (g_RandPercent(pAIParam[4]))
	{
		Npc[m_nIndex].SetActiveSkill(3);
	}
	else  // If none chosen, perform default action
	{
		CommonAction();
		return;
	}

	// After skill set, proceed to follow and attack the locked enemy
	FollowAttack(nEnemyIdx);
	return;
}




//------------------------------------------------------------------------------
// Function: Escape Plus (AI Type 4)
// Description:
//   This AI type adds an enhanced escape logic depending on the NPC's remaining
//   health. It performs the following behaviors:
//     - Reacquires the nearest enemy if the current one is invalid or out of range.
//     - If no enemy is found, the NPC performs a common action (e.g., idle/patrol).
//     - If the NPC's HP drops below a threshold (m_AiParam[0]), it may flee
//       (based on probability m_AiParam[1]).
//     - If HP is below another threshold (m_AiParam[2]), it may cast a healing or
//       defensive skill (based on probability m_AiParam[3]).
//     - If the above don't trigger, it will choose an offensive skill:
//         + m_AiParam[4]: probability to use Skill 2
//         + m_AiParam[5]: probability to use Skill 3
//       If no skill is chosen, it performs a default action.
//     - Attacks the current target if a skill is selected.
//
// Parameters:
//   m_AiParam[0] - HP percent threshold to consider fleeing
//   m_AiParam[1] - Probability of fleeing when HP is below threshold
//   m_AiParam[2] - HP percent threshold to consider using skill 1 (e.g., heal/defense)
//   m_AiParam[3] - Probability of using skill 1 when HP is below threshold
//   m_AiParam[4] - Probability of using offensive skill 2
//   m_AiParam[5] - Probability of using offensive skill 3
//------------------------------------------------------------------------------
void KNpcAI::ProcessAIType4()
{
	int* pAIParam = Npc[m_nIndex].m_AiParam;

	// Check if NPC has exceeded its activity range
	if (KeepActiveRange())
		return;

	int nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;

	// Reacquire nearest enemy if current target is invalid or out of view
	if (nEnemyIdx <= 0 || !InEyeshot(nEnemyIdx))
	{
		nEnemyIdx = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = nEnemyIdx;
	}

	// If no enemy is found, perform a default action
	if (nEnemyIdx <= 0)
	{
		CommonAction();
		return;
	}

	// Calculate current HP percentage
	int nLifePercent = Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax;

	// Case 1: HP is very low, consider fleeing
	if (nLifePercent < pAIParam[0])
	{
		if (g_RandPercent(pAIParam[1]))
		{
			Flee(nEnemyIdx);
			return;
		}
	}

	// Case 2: HP is moderately low, consider using skill 1 (e.g., healing/defensive skill)
	if (nLifePercent < pAIParam[2])
	{
		if (g_RandPercent(pAIParam[3]))
		{
			Npc[m_nIndex].SetActiveSkill(1);
			Npc[m_nIndex].SendCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, m_nIndex);
			return;
		}
	}

	// Case 3: Choose an offensive skill to attack
	if (g_RandPercent(pAIParam[4]))
	{
		Npc[m_nIndex].SetActiveSkill(2);
	}
	else if (g_RandPercent(pAIParam[5]))
	{
		Npc[m_nIndex].SetActiveSkill(3);
	}
	else
	{
		CommonAction();
		return;
	}

	// Attack the target
	FollowAttack(nEnemyIdx);
}




//------------------------------------------------------------------------------
// Function: Normal active class 5 (AI for reacting to many enemies nearby)
// m_AiParam[0] Threshold for number of enemies — if exceeded, may choose to flee
// m_AiParam[1] Probability of fleeing when the number of enemies > m_AiParam[0]
// m_AiParam[2] Probability of using skill 1
// m_AiParam[3] Threshold for number of enemies — if fewer than or equal to this, may use skill 2
// m_AiParam[4] Probability of using skill 2 when the number of enemies <= m_AiParam[3]
//------------------------------------------------------------------------------
void KNpcAI::ProcessAIType5()
{
	int* pAIParam = Npc[m_nIndex].m_AiParam;

	// Exit if the NPC is outside its allowed activity range
	if (KeepActiveRange())
		return;

	int i = Npc[m_nIndex].m_nPeopleIdx;

	// If no current target or target is out of sight, find the nearest enemy
	if (!i || !InEyeshot(i))
	{
		i = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = i;
	}

	// If no enemies are found, perform a default action (standby or patrol)
	if (!i)
	{
		CommonAction();
		return;
	}

	// Count nearby enemies
	int nEnemyNumber = GetNpcNumber(relation_enemy);

	// If enemy count exceeds threshold and probability met, flee
	if (nEnemyNumber > pAIParam[0])
	{
		if (g_RandPercent(pAIParam[1]))
		{
			Flee(i);
			return;
		}
	}

	// Attempt to use skill 1 with defined probability
	if (g_RandPercent(pAIParam[2]))
	{
		Npc[m_nIndex].SetActiveSkill(1);
	}
	// If enemy count is low and probability met, use skill 2
	else if (nEnemyNumber <= pAIParam[3] && g_RandPercent(pAIParam[4]))
	{
		Npc[m_nIndex].SetActiveSkill(2);
	}
	// Otherwise perform default action (e.g., patrol)
	else
	{
		CommonAction();
		return;
	}

	// Perform attack movement toward the target
	FollowAttack(i);
}




//------------------------------------------------------------------------------
// Function: Group-type AI behavior
// Description:
//   This AI type handles NPCs that consider nearby allies before deciding on a behavior.
//   It dynamically switches between attacking, using skills, or fleeing based on 
//   the number of nearby allies and random probabilities.
// m_AiParam[0] - Minimum number of allies required to avoid fleeing
// m_AiParam[1] - Probability to flee when allies are too few
// m_AiParam[2] - Probability to use skill 1 regardless of ally number
// m_AiParam[3] - Threshold of allies for possibly using skill 2
// m_AiParam[4] - Probability to use skill 2 when ally threshold is met
//------------------------------------------------------------------------------
void KNpcAI::ProcessAIType6()
{
	int* pAIParam = Npc[m_nIndex].m_AiParam;

	// Check if NPC is still within the allowed active area
	if (KeepActiveRange())
		return;

	// Try to maintain a target; reacquire if needed
	int i = Npc[m_nIndex].m_nPeopleIdx;
	if (!i || !InEyeshot(i))
	{
		i = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = i;
	}

	// If still no target, perform default action (e.g., idle or patrol)
	if (!i)
	{
		CommonAction();
		return;
	}

	// Count the number of nearby allies (neutral or friendly)
	int nAllyNumber = GetNpcNumber(relation_none);

	// If allies are fewer than required threshold, maybe flee
	if (nAllyNumber <= pAIParam[0])
	{
		if (g_RandPercent(pAIParam[1]))
		{
			Flee(i);
			return;
		}
	}

	// Try using skill 1 with a certain probability
	if (g_RandPercent(pAIParam[2]))
	{
		Npc[m_nIndex].SetActiveSkill(1);
	}
	// If enough allies are around, maybe use skill 2
	else if (nAllyNumber > pAIParam[3] && g_RandPercent(pAIParam[4]))
	{
		Npc[m_nIndex].SetActiveSkill(2);
	}
	else
	{
		// Otherwise, default action (e.g., patrol or idle)
		CommonAction();
		return;
	}

	// Proceed to follow and attack the target
	FollowAttack(i);
}




//------------------------------------------------------------------------------
// Function: Beaten group type (AI Type 7)
// m_AiParam[0] If current HP% is below this value, consider running to an ally
// m_AiParam[1] Probability of running to the nearest ally when the condition above is met
// m_AiParam[2, 3, 4] Probability of using skills 1, 2, 3 respectively from the SkillList
//------------------------------------------------------------------------------  
void KNpcAI::ProcessAIType7()
{
	int* pAIParam = Npc[m_nIndex].m_AiParam;

	// Check if NPC should return due to moving out of the active area
	if (KeepActiveRange())
		return;

	int i = Npc[m_nIndex].m_nPeopleIdx;

	// If no enemy is currently targeted or enemy is out of sight, 
	// search for nearest enemy
	if (!i || !InEyeshot(i))
	{
		i = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = i;
	}

	// If still no enemy found, perform a default idle/patrol action
	if (!i)
	{
		CommonAction();
		return;
	}

	// Check if current HP is lower than threshold
	int j = GetNearestNpc(relation_ally);
	if (j && Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax < pAIParam[0])
	{
		// With given probability, move towards the nearest ally
		if (g_RandPercent(pAIParam[1]))
		{
			int x, y;
			Npc[j].GetMpsPos(&x, &y);
			Npc[m_nIndex].SendCommand(do_walk, x, y);
			return;
		}
	}

	// Try to use one of the active skills based on given probabilities
	if (g_RandPercent(pAIParam[2]))
	{
		Npc[m_nIndex].SetActiveSkill(1);
	}
	else if (g_RandPercent(pAIParam[3]))
	{
		Npc[m_nIndex].SetActiveSkill(2);
	}
	else if (g_RandPercent(pAIParam[4]))
	{
		Npc[m_nIndex].SetActiveSkill(3);
	}
	else
	{
		// If no skill was used, perform a default idle/patrol action
		CommonAction();
		return;
	}

	// If skill is used, attack the current enemy
	FollowAttack(i);
	return;
}




// Active suicide type
void KNpcAI::ProcessAIType8()
{
	int* pAIParam = Npc[m_nIndex].m_AiParam;

	// If the NPC is outside its allowed active range, stop further action
	if (KeepActiveRange())
		return;

	int i = Npc[m_nIndex].m_nPeopleIdx;

	// If no enemy is currently targeted or not in visible range, find the nearest enemy
	if (!i || !InEyeshot(i))
	{
		i = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = i;
	}

	// If still no enemy is found, perform idle behavior (e.g., patrol/standby)
	if (!i)
	{
		CommonAction();
		return;
	}

	// Decide action based on configured probabilities:
	// m_AiParam[0]: walk towards enemy
	// m_AiParam[1-3]: try to use skills 1-3
	if (g_RandPercent(pAIParam[0]))
	{
		int x, y;
		Npc[i].GetMpsPos(&x, &y);
		Npc[m_nIndex].SendCommand(do_walk, x, y);
	}
	else if (g_RandPercent(pAIParam[1]))
	{
		Npc[m_nIndex].SetActiveSkill(1);
	}
	else if (g_RandPercent(pAIParam[2]))
	{
		Npc[m_nIndex].SetActiveSkill(2);
	}
	else if (g_RandPercent(pAIParam[3]))
	{
		Npc[m_nIndex].SetActiveSkill(3);
	}
	else
	{
		// If no condition is met, fallback to default idle behavior
		CommonAction();
		return;
	}

	// Always follow and try to attack the current enemy
	FollowAttack(i);
}




//------------------------------------------------------------------------------
// Function: Normal active class 9 - "The more you fight, the braver you become."
// Description: This AI becomes more aggressive as its HP decreases. It uses 
// stronger skills with increasing probability when HP drops below certain thresholds.
//
// m_AiParam[0] Probability to use skill 1 (at any HP)
// m_AiParam[1] HP threshold (%) to consider using skill 2
// m_AiParam[2] Probability to use skill 2 if HP < m_AiParam[1]
// m_AiParam[3] HP threshold (%) to consider using skill 3
// m_AiParam[4] Probability to use skill 3 if HP < m_AiParam[3]
//------------------------------------------------------------------------------
void KNpcAI::ProcessAIType9()
{
	int* pAIParam = Npc[m_nIndex].m_AiParam;

	// Has the activity radius been exceeded?
	if (KeepActiveRange())
		return;

	int i = Npc[m_nIndex].m_nPeopleIdx;

	// If the enemy is not locked or runs too far away, re-lock the enemy
	if (!i || !InEyeshot(i))
	{
		i = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = i;
	}

	// There are no enemies around, do nothing or fallback to idle behavior
	if (!i)
	{
		CommonAction();
		return;
	}

	// Get current HP percentage
	int nLifePercent = Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax;

	// Skill 1: Attempt based on fixed probability, regardless of HP
	if (g_RandPercent(pAIParam[0]))
	{
		Npc[m_nIndex].SetActiveSkill(1);
	}
	// Skill 2: If HP below threshold 1 and passes probability check
	else if (nLifePercent < pAIParam[1] && g_RandPercent(pAIParam[2]))
	{
		Npc[m_nIndex].SetActiveSkill(2);
	}
	// Skill 3: If HP below threshold 2 and passes probability check
	else if (nLifePercent < pAIParam[3] && g_RandPercent(pAIParam[4]))
	{
		Npc[m_nIndex].SetActiveSkill(3);
	}
	// No skill selected, fallback to normal action
	else
	{
		CommonAction();
		return;
	}

	// Attack the locked enemy
	FollowAttack(i);
	return;
}




//------------------------------------------------------------------------------
// Function: Normal active class 10 (Escape behavior logic)
// This AI prioritizes survival based on health thresholds, using skills or fleeing
//
// m_AiParam[0] Life percentage threshold to use skill 1
// m_AiParam[1] Probability of using skill 1 when health is below m_AiParam[0]
// m_AiParam[2] Life percentage threshold to use skill 2
// m_AiParam[3] Probability of using skill 2 when health is below m_AiParam[2]
// m_AiParam[4] Life percentage threshold to trigger escape behavior
// m_AiParam[5] Probability of escaping when health is below m_AiParam[4]
//------------------------------------------------------------------------------
void KNpcAI::ProcessAIType10()
{
	int* pAIParam = Npc[m_nIndex].m_AiParam;

	// Has the activity radius been exceeded? If so, return
	if (KeepActiveRange())
		return;

	// Get current enemy index
	int i = Npc[m_nIndex].m_nPeopleIdx;

	// If there's no current target or it's out of sight, acquire the nearest enemy
	if (!i || !InEyeshot(i))
	{
		i = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = i;
	}

	// If still no target is found, perform idle behavior and return
	if (!i)
	{
		CommonAction();
		return;
	}

	// Calculate current health percentage
	int nLifePercent = Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax;

	// Evaluate conditions to either use skill 1, skill 2, or flee
	if (nLifePercent < pAIParam[0] && g_RandPercent(pAIParam[1]))
	{
		Npc[m_nIndex].SetActiveSkill(1);
	}
	else if (nLifePercent < pAIParam[2] && g_RandPercent(pAIParam[3]))
	{
		Npc[m_nIndex].SetActiveSkill(2);
	}
	else if (nLifePercent < pAIParam[4] && g_RandPercent(pAIParam[5]))
	{
		// If all conditions met for escape, run away
		Flee(i);
		return;
	}
	else
	{
		// Default fallback behavior
		CommonAction();
		return;
	}

	// Engage with the current target using the selected skill
	FollowAttack(i);
	return;
}


