/* -------------------------------------------------------------------------
//	文件名		：	kautoai.cpp
//	创建者		：	luobaohang
//	创建时间	：	2007-11-3 12:25:22
//	功能描述	：	自动挂机AI
//
// -----------------------------------------------------------------------*/

#include "kcore.h"
#include <iostream>
#include <fstream>
#include "knpc.h"
#include "kplayer.h"
#include "knpcfindpath.h"
#include "kautoai.h"
#include <math.h>
#include "KMath.h"

using namespace std;

#ifndef _SERVER

class KSelfNpcFindPath : public KNpcFindPath
{
public:
	KSelfNpcFindPath() { m_NpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex; }
};

//目标循环（用于AI分析）//lbhtest
#define AI_BLACK_COUNT			2				//黑名单的最大个数
#define AI_TIMES_ADDBLACK			15			//多少次攻击无效后加入黑名单
#define AI_TIMES_BLACK_CLOSE		5			//开始攻击后多少次不掉血就加黑名单，需小于AI_TIMES_ADDBLACK
#define AI_TIME_BLACK_LAST			(1000*3)	//黑名单有效时间
typedef struct tagTargetNode 
{
	tagTargetNode()
	{
		m_IDToHit = 0;
	}
	INT	m_IDToHit;
	INT	m_nAddTime; //加入黑名单的时间
}TargetNode;

// ---------KAutoMove----------------------------------------------------------------
INT KAutoMove::AutoMove() 
{
	if (m_CurDir < 0)
		m_CurDir = g_Random(8); //m_CurDir = KSysService::Rand() % 8;
	INT nDis;
	if ((nDis = FindPathAndMove(m_CurDir)) <= 0)
	{	
		//变成反方向
		m_CurDir = (m_CurDir + 4) & 0x7;
		return FindPathAndMove(m_CurDir);
	}
	return nDis;
}

INT KAutoMove::AutoMoveTo( INT nX, INT nY ) 
{
	INT nOrgX, nOrgY;
	INT nPlayer = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	KNpc *pNpc = &Npc[nPlayer];
	pNpc->GetMpsPos(&nOrgX, &nOrgY);
	INT nDis = g_GetDisSquare(nOrgX, nOrgY, nX, nY);
	//距离接近，直接走
	if (nDis <= 200 * 200)
	{
		pNpc->ClientGotoPos(nX, nY);
		return -2;	//-2表示已经到达
	}
	INT nDir = g_GetDirIndex(nOrgX, nOrgY, nX, nY);
	nDir = KSelfNpcFindPath().Dir64To8(nDir);
	//若以前没走过
	if (m_CurDir < 0)
		m_CurDir = nDir;
	//和当前方向合成（否则易出现在两点来回走的情况）
	INT nCDir = (nDir + m_CurDir) / 2;
	//角度大于180度，取反角度合成
	if (abs(nDir - m_CurDir) > 4)
		nCDir = (nCDir + 4) & 0x7;
	m_CurDir = nCDir;
	//如果这个方向不能走，则走随机方向
	if ((nDis = FindPathAndMove(m_CurDir, min((INT)sqrt((DOUBLE)nDis), (INT)DIS_MOVE))) <= 0)
	{
		Reset();
		return AutoMove();
	}
	return nDis;
}

INT KAutoMove::FindPathAndMove( INT &nDir8, INT minStep /*= -1*/ ) 
{
	if (minStep < 0)
		minStep = DIS_MOVE;
	INT nPlayer = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	KNpc *pNpc = &Npc[nPlayer];
	KSelfNpcFindPath path;
	INT nOrgX, nOrgY;
	pNpc->GetMpsPos(&nOrgX, &nOrgY);
	INT nDir64 = path.Dir8To64(nDir8 & 0x7);
	INT i, x, y;
	INT nSpeed = pNpc->m_CurrentRunSpeed;
	INT nLastDis = CheckDirectMoveDis(pNpc, nDir8, nSpeed);
	INT nMaxDis = nLastDis;
	i = 1;
	INT nGoodDir = nDir8;
	while(nMaxDis < minStep && i < 3) 
	{
		INT nDis = CheckDirectMoveDis(pNpc, nDir8 + i, nSpeed);
		if (nDis > nMaxDis)
		{
			nMaxDis = nDis;
			nGoodDir = (nDir8 + i + 8) & 0x7;
		}
		i = i > 0 ? -i : 1-i;
	}
	if (nMaxDis == 0)
	{	
		//各个方向都走不通
		return 0;
	}
	nDir8 = nGoodDir;
	nDir64 = path.Dir8To64(nDir8 & 0x7);
	x = (g_DirCos( nDir64, 64 ) * nMaxDis) >> 10;
	y = (g_DirSin( nDir64, 64 ) * nMaxDis) >> 10;
	pNpc->ClientGotoPos(nOrgX + x, nOrgY + y);
	return nMaxDis;
}

INT KAutoMove::CheckDirectMoveDis( KNpc *pNpc, INT nDir8, INT nSpeed ) 
{
	KSelfNpcFindPath path;
	INT nDir64 = path.Dir8To64((nDir8 + 8) & 0x7);
	INT nDx = g_DirCos( nDir64, 64 ) * nSpeed;
	INT nDy = g_DirSin( nDir64, 64 ) * nSpeed;
	INT nDis = 0;
	INT i;
	for (i = 1; i <= DIS_MOVE / nSpeed; i++)
	{
		if (path.CheckBarrier(nDx * i, nDy * i) != Obstacle_NULL)
			return nDis;
		nDis += nSpeed;
	}
	return DIS_MOVE;
}

#endif // #ifndef _SERVER
