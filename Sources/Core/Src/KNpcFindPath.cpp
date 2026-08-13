//---------------------------------------------------------------------------
// Sword3 Engine (c) 2019-2021 by GHK
//
// File:	KNpcFindPath.cpp
// Date:	2002.01.06
// Code:	Fong Ki襲
// Desc:	Obj Class
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	<math.h>
#include	"KMath.h"
#include	"KNpcFindPath.h"
#include	"KSubWorld.h"
#include	"KNpc.h"

#define	MAX_FIND_TIMER	30

//-------------------------------------------------------------------------
//	功能：构造函数
//-------------------------------------------------------------------------
KNpcFindPath::KNpcFindPath()
{
	m_nDestX = 0;
	m_nDestY = 0;
	m_nFindTimer = 0;
	m_nMaxTimeLong = MAX_FIND_TIMER;
	m_nFindState = 0;
	m_nPathSide = 0;
	m_nFindTimes = 0;
	m_NpcIdx = 0;
};

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KNpcFindPath::Init(int nNpc)
{
	m_NpcIdx = nNpc;
	m_nDestX = 0;
	m_nDestY = 0;
	m_nFindTimer = 0;
	m_nMaxTimeLong = MAX_FIND_TIMER;
	m_nFindState = 0;
	m_nPathSide = 0;
	m_nFindTimes = 0;
}

#define		defFIND_PATH_STOP_DISTANCE		64
int	KNpcFindPath::GetDir(int nXpos,int nYpos, int nDir, int nDestX, int nDestY, int nMoveSpeed, int *pnGetDir)
{	
	if ( !CheckDistance(nXpos >> 10, nYpos >> 10, nDestX, nDestY, nMoveSpeed))
	{
		m_nFindTimer = 0;
		m_nFindState = 0;
		m_nFindTimes = 0;
		return 0;
	}

	if (m_nDestX != nDestX || m_nDestY != nDestY)
	{
		m_nFindTimer = 0;
		m_nFindState = 0;
		m_nFindTimes = 0;
		m_nDestX = nDestX;
		m_nDestY = nDestY;
	}

	int		x, y, nWantDir;
	nWantDir = g_GetDirIndex(nXpos >> 10, nYpos >> 10, nDestX, nDestY);
	x = g_DirCos(nWantDir, 64) * nMoveSpeed;
	y = g_DirSin(nWantDir, 64) * nMoveSpeed;

	int nCheckBarrier = CheckBarrier(x, y);
	if ( nCheckBarrier == 0 )
	{
		m_nFindState = 0;
		*pnGetDir = nWantDir;
		return 1;
	}
	
	else if (nCheckBarrier == 0xff)
	{
		return -1;
	}

	int		i;
	
	if (m_nFindState == 0)
	{
		
#ifdef _SERVER
		nCheckBarrier = SubWorld[Npc[m_NpcIdx].m_SubWorldIndex].TestBarrier(nDestX, nDestY);
#else
		if (Npc[m_NpcIdx].m_RegionIndex >= 0)
			nCheckBarrier = SubWorld[0].TestBarrier(nDestX, nDestY);
		else
			nCheckBarrier = 0xff;
#endif
		if (nCheckBarrier != 0 && !CheckDistance(nXpos >> 10, nYpos >> 10, nDestX, nDestY, defFIND_PATH_STOP_DISTANCE))
		{
			m_nFindTimes = 0;
			return 0;
		}
		
		m_nFindTimes++;
		if (m_nFindTimes > 1)
		{
			m_nFindTimes = 0;
			return 0;
		}
		int		nTempDir8, nTempDir64;
		nTempDir8 = Dir64To8(nWantDir) + 8;
		
		nTempDir64 = Dir8To64(nTempDir8 & 0x07);
		x = g_DirCos( nTempDir64, 64 ) * nMoveSpeed;
		y = g_DirSin( nTempDir64, 64 ) * nMoveSpeed;
		if ( CheckBarrier(x, y) == 0 )
		{
			m_nFindState = 1;
			m_nFindTimer = 0;
			if ((nTempDir64 < nWantDir && nWantDir - nTempDir64 <= 4) || (nTempDir64 > nWantDir && nTempDir64 - nWantDir >= 60))
				m_nPathSide = 0;
			else
				m_nPathSide = 1;
			*pnGetDir = nTempDir64;
			return 1;
		}

		for (i = 1; i < 4; i++)
		{
			nTempDir64 = Dir8To64((nTempDir8 + i) & 0x07);
			x = g_DirCos( nTempDir64, 64 ) * nMoveSpeed;
			y = g_DirSin( nTempDir64, 64 ) * nMoveSpeed;
			if ( CheckBarrier(x, y) == 0 )
			{
				m_nFindState = 1;
				m_nFindTimer = 0;
				m_nPathSide = 1;
				*pnGetDir = nTempDir64;
				return 1;
			}

			nTempDir64 = Dir8To64((nTempDir8 - i) & 0x07);
			x = g_DirCos( nTempDir64, 64 ) * nMoveSpeed;
			y = g_DirSin( nTempDir64, 64 ) * nMoveSpeed;
			if ( CheckBarrier(x, y) == 0 )
			{
				m_nFindState = 1;
				m_nFindTimer = 0;
				m_nPathSide = 0;
				*pnGetDir = nTempDir64;
				return 1;
			}
		}
		//m_nFindTimes = 0;
		return 0;
	}
	
	else
	{
		
		if (m_nFindTimer >= m_nMaxTimeLong)
		{
			m_nFindState = 0;
			//m_nFindTimes = 0;
			//m_nFindTimer = 0;
			return 0;
		}
		m_nFindTimer++;
		int		nWantDir8, nTempDir64;
		nWantDir8 = Dir64To8(nWantDir) + 8;
		
		if (m_nPathSide == 1)
		{
			nTempDir64 = Dir8To64(nWantDir8 & 0x07);
			if ((nTempDir64 < nWantDir && nWantDir - nTempDir64 <= 4) || (nTempDir64 > nWantDir && nTempDir64 - nWantDir >= 60))
				i = 1;
			else
				i = 0;

			for (; i < 4; i++)
			{
				nTempDir64 = Dir8To64((nWantDir8 + i) & 0x07);
				x = g_DirCos( nTempDir64, 64 ) * nMoveSpeed;
				y = g_DirSin( nTempDir64, 64 ) * nMoveSpeed;
				if ( CheckBarrier(x, y) == 0 )
				{
					*pnGetDir = nTempDir64;
					return 1;
				}
			}
			m_nFindState = 0;
			//m_nFindTimes = 0;
			m_nFindTimer = 0;
			return 0;
		}

		else
		{
			nTempDir64 = Dir8To64(nWantDir8 & 0x07);
			if ((nTempDir64 < nWantDir && nWantDir - nTempDir64 <= 4) || (nTempDir64 > nWantDir && nTempDir64 - nWantDir >= 60))
				i = 0;
			else
				i = 1;

			for (; i < 4; i++)
			{
				nTempDir64 = Dir8To64((nWantDir8 - i) & 0x07);
				x = g_DirCos( nTempDir64, 64 ) * nMoveSpeed;
				y = g_DirSin( nTempDir64, 64 ) * nMoveSpeed;
				if ( CheckBarrier(x, y) == 0 )
				{
					*pnGetDir = nTempDir64;
					return 1;
				}
			}
			m_nFindState = 0;
			//m_nFindTimes = 0;
			m_nFindTimer = 0;
			return 0;
		}
	}

	m_nFindState = 0;
	//m_nFindTimes = 0;
	m_nFindTimer = 0;
	return 0;
}

//-------------------------------------------------------------------------
//	功能：	64 方向转换为 8 方向
//-------------------------------------------------------------------------
int		KNpcFindPath::Dir64To8(int nDir)
{
	return ((nDir + 4) >> 3) & 0x07;
}

//-------------------------------------------------------------------------
//	功能：	8 方向转换为 64 方向
//-------------------------------------------------------------------------
int		KNpcFindPath::Dir8To64(int nDir)
{
	return nDir << 3;
}

//-------------------------------------------------------------------------
//	返回：	距离小于 nDistance 返回 FALSE ，否则返回 TRUE
//-------------------------------------------------------------------------
BOOL	KNpcFindPath::CheckDistance(int x1, int y1, int x2, int y2, int nDistance)
{
	return ( (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) >= nDistance * nDistance );
}

//-------------------------------------------------------------------------
//	功能：	判断某个点是否是障碍
//-------------------------------------------------------------------------
int	KNpcFindPath::CheckBarrier(int nChangeX, int nChangeY)
{
#ifdef _SERVER
	return SubWorld[Npc[m_NpcIdx].m_SubWorldIndex].TestBarrierMin(Npc[m_NpcIdx].m_RegionIndex, Npc[m_NpcIdx].m_MapX, Npc[m_NpcIdx].m_MapY, Npc[m_NpcIdx].m_OffX, Npc[m_NpcIdx].m_OffY, nChangeX, nChangeY);
#else
	if (Npc[m_NpcIdx].m_RegionIndex >= 0)
		return SubWorld[0].TestBarrierMin(Npc[m_NpcIdx].m_RegionIndex, Npc[m_NpcIdx].m_MapX, Npc[m_NpcIdx].m_MapY, Npc[m_NpcIdx].m_OffX, Npc[m_NpcIdx].m_OffY, nChangeX, nChangeY);
	else
		return 0xff;
#endif
}

