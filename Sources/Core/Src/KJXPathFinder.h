/* -------------------------------------------------------------------------
//	文件名		：	jx_path_finder.h
//	创建者		：	liupeng
//	创建时间	：	2007-5-17
//	功能描述	：	封装自动寻路类
//
// -----------------------------------------------------------------------*/
#pragma once
#ifndef __JX_PATH_FINDER_H__
#define __JX_PATH_FINDER_H__

#ifndef _SERVER
#include <vector>
#include <algorithm>
#include "MyAssert.H"
#include <KWin32.h>
#include <ctime>

/*
enum Obstacle_Type			// 物件地面格子障碍类型
{
	Obstacle_Empty = 0,		// 空，无障碍
	Obstacle_Full,			// 完整障碍
	Obstacle_LT,			// 左上区域为障碍
	Obstacle_RT,			// 右上区域为障碍
	Obstacle_LB,			// 左下区域为障碍
	Obstacle_RB,			// 右下区域为障碍
	Obstacle_Type_Num,		// 障碍类型数
};

enum Obstacle_Kind
{
	Obstacle_NULL = 0,		// 没有障碍
	Obstacle_Normal,		// 一般障碍
	Obstacle_Fly,			// 障碍但是可以飞过
	Obstacle_Jump,			// 障碍但是可以跳过
	Obstacle_JumpFly,		// 障碍但是可以跳过并且飞过
	Obstacle_Kind_Num,		// 障碍种类数
};
*/
struct FindPathNode
{
	INT		x;
	INT		y;
};

//-------------------------------------------------------------------------------

CONST INT		notfinished = 0;
CONST INT		NearObstacle = 10;
CONST INT		MaxListNum = 0XFFFF;		// 最大的
CONST INT		RegionWidth = 16;
CONST INT		RegionHeight = 32;
CONST INT		notStarted = 0, found = 1, nonexistent = 2;
CONST INT		unwalkable = 0, walkable = 1;

#pragma pack(push, 1)
class OpenNode
{
public:
	WORD		x;						// open 节点 x 位置
	WORD		y;						// open 节点 y 位置
	WORD		fcost;					// open 节点到起始点的评估
	WORD		hcost;					// open 节点到目标点的评估值
};

struct PathNode
{
	BYTE		canwoalk;				// 是否是障碍
	BYTE		pathflag;				// 路径标志
	INT			gconst;					// 到起始点的评估值
	WORD		parent_x;				// 从当前点出发到起始点最短路径中的下一个点 x 位置
	WORD		parent_y;				// 从当前点出发到起始点最短路径中的下一个点 y 位置
	FLOAT		factor;					// 选择当前点时的权值(靠近障碍的点的权值较大,路径通过该点所用花费就比较大)
	WORD		pad_0;					// 4字节对齐边界填充
};
#pragma pack(pop)

class OpenNodePtr
{
public:
	OpenNodePtr() : m_pNode(NULL) {}
	OpenNodePtr(OpenNode* pNode) : m_pNode(pNode) {}
	VOID operator=(OpenNode* pNode)
	{
		m_pNode = pNode;
	}
	bool operator<(CONST OpenNodePtr& p)
	{
		return m_pNode->fcost > p.m_pNode->fcost;
	}
	OpenNode* operator->()
	{
		return m_pNode;
	}
	OpenNode* GetPtr()
	{
		return m_pNode;
	}
	OpenNode* detch()
	{
		OpenNode* res = m_pNode;
		m_pNode = NULL;
		return res;
	}
private:
	OpenNode* m_pNode;
};

class OpenTable
{
public:
	OpenTable() : m_pOpenTable(NULL), m_nSize(0), m_nCurSize(0)
	{
	}
	~OpenTable()
	{
		if (m_pOpenTable)
			delete[] m_pOpenTable;
		m_pOpenTable = NULL;
		m_nSize = 0;
		m_nCurSize = 0;
	}

	bool			empty() { return m_nCurSize == 0; }
	INT				GetSize() { return m_nCurSize; }
	OpenNode* GetNode(INT i) { return m_pOpenTable[i].GetPtr(); }

	VOID			Reset(INT nSize)
	{
		if (m_pOpenTable)
		{
			delete[] m_pOpenTable;
			m_pOpenTable = NULL;
		}
		if (nSize)
		{
			m_nSize = nSize;
			m_pOpenTable = new OpenNodePtr[m_nSize];
		}
		m_nCurSize = 0;
	}

	OpenNode* PopNode()
	{
		if (empty())
			return NULL;
		std::pop_heap(&m_pOpenTable[0], &m_pOpenTable[m_nCurSize]);
		m_nCurSize--;
		return m_pOpenTable[m_nCurSize].detch();
	}
	VOID			PushNode(OpenNode* pNode)
	{
		_ASSERT(m_nCurSize < m_nSize);
		if (!m_nCurSize)
		{
			m_pOpenTable[0] = pNode;
			m_nCurSize++;
			return;
		}
		else
		{
			m_pOpenTable[m_nCurSize] = pNode;
			m_nCurSize++;
			std::push_heap(&m_pOpenTable[0], &m_pOpenTable[m_nCurSize]);
		}
	}
	VOID			MakeHeap()
	{
		if (empty()) return;
		std::make_heap(&m_pOpenTable[0], &m_pOpenTable[m_nCurSize]);
	}

private:
	// 所有的open节点,按照fcos大小排列,使用堆算法排序
	OpenNodePtr* m_pOpenTable;
	INT				m_nSize;
	INT				m_nCurSize;
};

//------------------------------------------------------------------------
CONST WORD			cs_CloseFlagStart = 10;
CONST WORD			cs_OpenFlagStart = cs_CloseFlagStart - 1;

//////////////////////////////////////////////////////////////////////////
// 障碍表
class KBusPointList
{
public:
	KBusPointList() {}
	~KBusPointList() {}

	INT AddVector(INT x)
	{
		m_vecX.push_back(x);
		//m_vecY.push_back(y);
		return m_vecX.size();
	}

	INT	GetSize() { return m_vecX.size(); }
	VOID GetPoint(INT i, INT& x)
	{
		x = m_vecX[i];
	}
	VOID Reset()
	{
		//m_nCurPos = 0;
		m_vecX.erase(m_vecX.begin(), m_vecX.end());
		//m_vecY.erase(m_vecY.begin(), m_vecY.end());
	}
	INT	GetIndex(INT x)
	{
		for (INT i = 0; i < (INT)m_vecX.size(); i++)
		{
			if (x == m_vecX[i])
				return i + 1;
		}
		return -1;
	}
private:
	std::vector<SHORT>		m_vecX;
	//std::vector<SHORT>		m_vecY;
};
//----------------------------------------------------------

enum KE_NEXTSTEP_RESULT
{
	emKNEXTSTEP_RESULT_SUCCESS,//Indicates that the next point was successfully found
	emKNEXTSTEP_RESULT_NOANYWAY,//Indicates that the next point cannot be found
	emKNEXTSTEP_RESULT_ARRIVAL,//Indicates that the destination has been reached
};

class KScenePlaceMapC;
class KJXPathFinder
{
public:

	KJXPathFinder(BOOL bFactor = TRUE);
	~KJXPathFinder();

	void	FillNote(int, int, int);
	BOOL	FindPath(int, int, int, int);
	void	GetPath(int, int, std::vector<FindPathNode>&, time_t);

	//Every time a new map arrives, it will be reinitialized and the obstacle.bin obstacle information file of the map will be loaded
	BOOL				Init(RECT*, KScenePlaceMapC*);

	//Calculate the trajectory to the target point (return value: TRUE means you can go, FALSE means you can't go)
	BOOL				FindPath(FindPathNode& start, FindPathNode& target, BOOL bFactor = TRUE);

	//According to the current position now, determine where to go next and store the return value in nextstep
	//Note: The now position is the player's current position synchronized by the server at regular intervals.
	//Due to the deviation between the server algorithm and the client, or the player may move the position midway, the now position may not necessarily be in the trajectory set during the last pathfinding
	//Need to decide the next move position based on now. And there may be a situation where it is difficult to go from the now position to the final target point. At this time, it needs to be handled well
	KE_NEXTSTEP_RESULT	GetNextStep(CONST FindPathNode& now, FindPathNode& nextstep, INT steplen);

	// 获取地图的宽度和高度
	INT					GetMapWidth() { return m_nMapWidth; }
	INT					GetMapHeight() { return m_nMapHeight; }

	INT					CanWalk(INT x, INT y) { return m_map[x][y].canwoalk == walkable; }
	FLOAT				GetFactor(INT x, INT y) { return m_map[x][y].factor; }

	INT					GetPath(FindPathNode& now, FindPathNode*& pPath);
	BOOL				IsReady()
	{
		return m_bReady;
	};
	RECT* GetLeftTop()
	{
		return &m_LeftTop;
	}
	void setStand(int x, int y) {
		m_Stand.x = x;
		m_Stand.y = y;
	}

	FindPathNode getStand() {
		return m_Stand;
	}

private:
	KE_NEXTSTEP_RESULT	GetNextStep(CONST FindPathNode& now, FindPathNode& nextstep);
	BOOL CheckIfStraightPath(DOUBLE ex, DOUBLE ey, DOUBLE tx, DOUBLE ty);
	BOOL GetNextStraightPoint(DOUBLE dCurX, DOUBLE dCurY, INT& x, INT& y);
	BOOL				CreateMap(INT w, INT h);
	VOID				ClearMap();
	VOID				ClearOpenTable();
	BOOL				LoadMap(RECT*, KScenePlaceMapC*);
	VOID				ResetMap();
	VOID				AddOpenNode(INT x, INT y, WORD fcost, WORD hcost, INT px, INT py);
	VOID				FixOpenTable(CONST FindPathNode& now);
	VOID				AddCloseNode(OpenNode* pNode, INT tx, INT ty);
	VOID				CalcFactors();
private:
	// destination coordinates
	FindPathNode		m_target;
	// map size
	INT					m_nMapWidth;
	INT					m_nMapHeight;

	BYTE** m_ppbtBarrier;//障碍数据
	// A* 算法的辅助数据结构

	// 障碍表
	//KBusPointList	    m_vec;	//存

	// 当前到目标路径的close点标志
	WORD				m_sCloseFlag;
	// 当前到目标路径的open点标志
	WORD				m_sOpenFlag;
	// 当前节点的open表
	OpenTable			m_OpenTable;
	// 地图信息
	PathNode** m_map;
	// 是否使用权值进行寻路
	BOOL				m_bHasFactor;

	BOOL				m_bReady;
	RECT				m_LeftTop;
	FindPathNode	m_Stand;
};

//extern KJXPathFinder g_JXPathFinder;
#endif
#endif // __JX_PATH_FINDER_H__
