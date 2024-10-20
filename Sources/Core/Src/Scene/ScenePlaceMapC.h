// ***************************************************************************************
// 场景地图的小地图
// Copyright : Kingsoft 2003
// Author    : wooy(wu yue)
// CreateTime: 2003-7-8
// ---------------------------------------------------------------------------------------
// 考虑效率因素，决定障碍小地图显示的范围要包含在场景地图架载的范围之中。场景模块架载地图的范围是
// 7*7=49个区域，障碍小地图的最大显示范围对应为6*6=36个区域。当小地图的缩小比率为16分之一的情况
// 下，则对应的小地图显示图形的大小就限定在不大于192*192像素。
// ***************************************************************************************
#pragma once

#include <KLittleMap.h>


enum SYMBOL_KIND
{
	MapDirect,
	SymName,
	SymPoint,
};

#define	defMAX_NUM_SYMBOL				58

class KScenePlaceMapC
{
public:

	enum SCENE_PLACE_MAP_WORK_PARAM_PUB
	{
        MAP_SCALE_H = 16,				//地图在横向上的比例
		MAP_SCALE_V = 32,				//地图在纵向上的比例
	};

#ifndef _SERVER
	KLittleMap	m_cLittleMap;
	
#endif

public:
	KScenePlaceMapC();
	~KScenePlaceMapC();

	void	Terminate();	

	BOOL Load(KIniFile* pSetting, const char* pszScenePlaceRootPath);

	void Free();

	void SetFocusPosition(int nX, int nY, bool bChangedRegion);

	void SetShowElemsFlag(unsigned int uShowElemsFlag);

	void SetSize(int cx, int cy);

	void PaintMap(int nx, int ny);
	void PaintAutoPath(int nX, int nY);

	int	GetMapRect(RECT* pRect);

	void PaintSymbol(int nX);
	void LoadSymbol(int nSubWorldID);
	void PaintFindPos(int nX, int nY, int nMpsX, int nMpsY);
	void DirectFindPos(int nX, int nY, BOOL bSync, BOOL bPaintLine);
	BOOL AutoRunTo(int nX, int nY);
	BOOL AutoRunToB(int nX, int nY);
	BOOL OnDirectMap(int nX, int nY);
	void DoDirectMap(int nX, int nY);
	POINT m_DirectPos; 

	BYTE** GetbtBarrier();
	BYTE* GetbtBarrier(int X, int Y);
	int			m_nMapWidth;			// 地图长(region, m_sMapRect.right - m_sMapRect.left + 1)
	int			m_nMapHeight;			// 地图宽(region, m_sMapRect.bottom - m_sMapRect.top + 1)

private:
	bool	Initialize();	//初始化
	void	FillCellsPicInfo();
	void	PaintMapPic(int nX, int nY);
	void	PaintCharacters(int nX, int nY);
	void	CalcPicLayout();
private:

	//对象运行时的一些参数(仅内部使用)
	enum SCENE_PLACE_MAP_WORK_PARAM
	{
		MAP_CELL_MAP_WIDTH = 128,		//地图块在地图中横向的宽度（单位：像素点）
		MAP_CELL_MAP_HEIGHT = 128,		//地图块在地图中纵向的宽度（单位：像素点）
		MAP_CELL_SCENE_WIDTH = 2048,	//地图块在实际场景中横向的宽度（单位：像素点）= MAP_SCALE_H * MAP_CELL_MAP_WIDTH
		MAP_CELL_SCENE_HEIGHT = 4096,	//地图块在实际场景中纵向的宽度（单位：像素点）= MAP_SCALE_V * MAP_CELL_MAP_HEIGHT
		MAP_CELL_NUM_REGION_H = 4,		//每个地图块在横向包含的场景区域的数目
		MAP_CELL_NUM_REGION_V = 4,		//每个地图块在纵向包含的场景区域的数目

 		MAP_CELL_MAX_RANGE = 5,			//模块中同时绘制的地图块的横/纵向最大跨度（单位：块）
		MAP_CELL_CENTRE_INDEX = 2,		//焦点所在的地图格的横或纵向索引

//		MAP_CELL_FOCUS_INDEX_MIN = 2,	//焦点所在的地图格的横或纵向索引许可最小值
//		MAP_CELL_FOCUS_INDEX_MAX = 2,	//焦点所在的地图格的横或纵向索引许可最大值

		MAP_MAX_SUPPORT_WIDTH  = (MAP_CELL_MAX_RANGE - 1) * MAP_CELL_MAP_WIDTH,		//小地图的最大支持显示宽（单位：像素点）
		MAP_MAX_SUPPORT_HEIGHT = (MAP_CELL_MAX_RANGE - 1) * MAP_CELL_MAP_HEIGHT,	//小地图的最大支持显示高（单位：像素点）

		MAP_A_REGION_NUM_MAP_PIXEL_H = 32,	//场景一个区域在地图上的横向跨度（单位：像素点）
		MAP_A_REGION_NUM_MAP_PIXEL_V = 32,	//场景一个区域在地图上的纵向跨度（单位：像素点）
	};

	struct	MAP_CELL
	{
		 char			szImageName[32];
		 unsigned int	uImageId;
		 short			sISPosition;
		 short			sReserved;
	};

private:
	int		m_bHavePicMap;			
	bool	m_bInited;				
	int bFlag; // toa do
	BOOL bSearch;
	RECT m_MapPos;
	POINT m_GreenLineDes; // ket thuc toa do
	char	m_szEntireMapFile[128];	
	KSGImageContent*	m_pEntireMap;

	POINT	m_EntireMapLTPosition;	

	POINT	m_EndMapLTPosition;	    //右下角场景坐标

	POINT	m_FocusPosition;		
	POINT	m_PicLoadedLTPosition;	
	RECT	m_FocusLimit;			

	SIZE			m_Size;			
	unsigned int	m_uMapShowElems;
	MAP_CELL		m_ElemsList[MAP_CELL_MAX_RANGE][MAP_CELL_MAX_RANGE];

	//--绘制显示时参数预先计算与保留的--
	RECT		m_MapCoverArea;		//地图绘制部分覆盖的范围（以地图像素点为单位，场景坐标点0,0对应点为原点）
	RECT		m_PaintCell;			//要绘制的地图块索引范围
	SIZE		m_FirstCellSkipWidth;	//绘制的横/纵头一块要跳过的宽/高度
	SIZE		m_LastCellSkipHeight;	//绘制的横/纵最末那块的结束处的宽/高度

	KMapTraffic	m_SymbolMap[defMAX_NUM_SYMBOL];

	unsigned int	m_uSelfColor;			// 主角颜色
	unsigned int	m_uTeammateColor;		// 队友颜色
	unsigned int	m_uPlayerColor;			// 其他玩家颜色
	unsigned int	m_uFightNpcColor;		// 战斗npc颜色
	unsigned int	m_uNormalNpcColor;		// 普通npc颜色
	unsigned int	m_uSelfPartnerColor;			// 其他玩家颜色
	unsigned int	m_uOtherPartnerColor;			// 其他玩家颜色
	unsigned int	m_uOtherPlayerColor;		// 队友颜色
	unsigned int	m_uOtherNpcColor;			// 其他玩家颜色
	unsigned int	m_uSelfNpcColor;		// 战斗npc颜色
	unsigned int	m_uSelfPlayerColor;		// 普通npc颜色

	BOOL m_bPaintLine;
};

