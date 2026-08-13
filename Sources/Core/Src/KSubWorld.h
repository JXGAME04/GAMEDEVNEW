#ifndef	KWorldH
#define	KWorldH

#ifdef _SERVER
#define	MAX_SUBWORLD	1000//edit by phong kieu max maps mac dinh 80
#else
#define	MAX_SUBWORLD	1
#endif
#define	VOID_REGION		-2
//-------------------------------------------------------------
#include "KEngine.h"
#include "KRegion.h"
#include "KWeatherMgr.h"

#ifndef _SERVER
#include <KLittleMap.h>
#endif

#ifdef _SERVER
#include "KMission.h"
#include "KMissionArray.h"
#define MAX_SUBWORLD_MISSIONCOUNT 10
#define MAX_GLOBAL_MISSIONCOUNT 50
typedef KMissionArray <KMission , MAX_TIMER_PERMISSION> KSubWorldMissionArray;
typedef KMissionArray <KMission , MAX_GLOBAL_MISSIONCOUNT> KGlobalMissionArray;
extern KGlobalMissionArray g_GlobalMissionArray;
#endif

#ifndef _SERVER
#define FINDPATH_VERSION	0	//t¨ng lan ?ó c?p nh?t d÷ li?u map míi
#define MAX_CELL		2400000
struct VGridNode
{
	int parentId;
	int connStart;
    int connCount;
	WORD x;
	WORD y;
	WORD obs;
	BYTE w;
	BYTE h;
	VGridNode()
	{
		w = 1;
		h = 1;
		connStart = -1;
		connCount = 0;
	}
};

struct VGridNeighbour
{
    int toParentId;  // id to m_vNeighbour
	int cost;
};

#endif

#ifndef TOOLVERSION
class KSubWorld
#else

class CORE_API KSubWorld
#endif
{
public:	
	int			m_nIndex;
	int			m_SubWorldID;
#ifdef _SERVER
	KSubWorldMissionArray m_MissionArray;
#endif
	KRegion*	m_Region;
#ifndef _SERVER
	int			m_ClientRegionIdx[MAX_REGION];
	char		m_szMapPath[FILE_NAME_LENGTH];
	//KLittleMap	m_cLittleMap;
#endif
#ifndef _SERVER
    DWORD m_dwLastNpcCheck;
#endif
	char		szMapName[FILE_NAME_LENGTH];
	char		szMapType[FILE_NAME_LENGTH];
	char		szGoldenDropRate[FILE_NAME_LENGTH];
	char		szNormalDropRate[FILE_NAME_LENGTH];
	int			 m_NpcSeriesAuto;
	int			 m_NpcSeriesMetal; //ti le quai he kim
	int			 m_NpcSeriesWood; //ti le quai he moc
	int			 m_NpcSeriesWater; //ti le quai he thuy
	int			 m_NpcSeriesFire; //ti le quai he hoa
	int			 m_NpcSeriesEarth; //ti le quai he tho
	int			nzAutoGoldenNpc;
	int			nzGoldenType;
	int			m_nWorldRegionWidth;			//	
	int			m_nWorldRegionHeight;			//	
	int			m_nTotalRegion;					//	SubWorldRegion
	int			m_nRegionWidth;					//	
	int			m_nRegionHeight;				//	
	int			m_nCellWidth;					//	
	int			m_nCellHeight;					//	
	int			m_nRegionBeginX;				
	int			m_nRegionBeginY;
	int			m_nWeather;						//	天气变化
	DWORD		m_dwCurrentTime;				//	当前帧
	KWorldMsg	m_WorldMessage;					//	消息
	KList		m_NoneRegionNpcList;			//	不在地图上的NPC
	char		m_CityOwnTong[32];
	BYTE		m_CityTax;

#ifdef _SERVER
	KWeatherMgr *m_pWeatherMgr;
#endif
#ifndef _SERVER
	HANDLE  m_hLoadPathGrid;
    volatile BOOL m_bStopThread;
#endif
public:
	KSubWorld();
	~KSubWorld();
	void		Activate();
	void		GetFreeObjPos(POINT& pos);
	void		GetRandomObjPos(POINT& pos);	//add by phong ki襲 x?l?t筸 thay cho h祄 GetFreeObjPos sau n祔 fix 頲 s?d鬾g l筰
	BOOL		CanPutObj(POINT pos);
	BOOL		CanPutObjBarrier(POINT pos);
	void		ObjChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nObjIdx);
	void		MissleChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nObjIdx);
	void		AddPlayer(int nRegion, int nIdx);
	void		RemovePlayer(int nRegion, int nIdx);
	void		Close();
	int			GetDistance(int nRx1, int nRy1, int nRx2, int nRy2);						//
	void		NewMap2Mps(int nR, int nX, int nY, int nDx, int nDy, int* nRx, int* nRy);
	void		Map2Mps(int nR, int nX, int nY, int nDx, int nDy, int *nRx, int *nRy);		// 
	static void Map2Mps(int nRx, int nRy, int nX, int nY, int nDx, int nDy, int *pnX, int *pnY);		// 
	void		Mps2Map(int Rx, int Ry, int * nR, int * nX, int * nY, int *nDx, int * nDy);	// 
	void		GetMps(int *nX, int *nY, int nSpeed, int nDir, int nMaxDir = 64);			// 
	BYTE		TestBarrier(int nMpsX, int nMpsY);
	BYTE		TestBarrier(int nRegion, int nMapX, int nMapY, int nDx, int nDy, int nChangeX, int nChangeY);	// 
	BYTE		TestBarrierMin(int nRegion, int nMapX, int nMapY, int nDx, int nDy, int nChangeX, int nChangeY);	// 
	BYTE		GetBarrier(int nMpsX, int nMpsY);											// 
	DWORD		GetTrap(int nMpsX, int nMpsY);
	void		MessageLoop();
	int			FindRegion(int RegionID);													// 
	int			FindFreeRegion(int nX = 0, int nY = 0);
#ifdef _SERVER
	int			RevivalAllNpc();//
	void		BroadCast(const char* pBuffer, size_t uSize);
	BOOL		LoadMap(int nIdx);
	void		LoadObject(char* szPath, char* szFile);
	void		NpcChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nNpcIdx);
	void		PlayerChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nObjIdx);
	BOOL		SendSyncData(int nIdx, int nClient);
	int			GetRegionIndex(int nRegionID);
	int			FindNpcFromName(const char * szName);
	int	        DelAllNpcInWro(); //删除该地图上的所有NPC
	int DelAllNpcInWro(char* szName);
	long	 CountAllNpc();
	long	CountAllPlayer();
	long	GetLastPlayerIndex(int nCurrentPlayerIndex);
	std::vector<int> GetAllPlayerIndexes();
	void		SetTrap(DWORD dwTrapId, int nMpsX, int nMpsY, int nRange); // AddTrap Fong Ki襲
	void		SetObstacle(long value, int nMpsX, int nMpsY, int nRange); //#Set V藅 C秐
	BOOL ExecuteScript(char * ScriptFileName, char * szFunName, int nParam);
	BOOL ExecuteScript(DWORD dwScriptId,  char * szFunName, int nParam);
#endif
#ifndef _SERVER
	BOOL		LoadMap(int nIdx, int nRegion);
	void		NpcChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nNpcIdx);
	void		Paint();
	void		Mps2Screen(int *Rx, int *Ry);
	void		Screen2Mps(int *Rx, int *Ry);
	void		ProcLoadPathGrid();
	int			FindPath(int nX, int nY, bool bCheckNpc = false);
	void		StopPath() {
		m_nTargetX = 0;
		m_nTargetY = 0;
		m_nCurStep = 0;
		m_vRetPath.clear();
	};
	bool HaveTarget(int& x, int& y)
	{
		x = m_nTargetX;
		y = m_nTargetY;
		if(m_vRetPath.size() > 0 && m_nTargetX > 0 && m_nTargetY > 0)
			return true;
		return false;
	}
#endif
private:
	void		LoadTrap();
	void		ProcessMsg(KWorldMsgNode *pMsg);
#ifndef _SERVER
	void		LoadCell();
	int			BlockHeuristic(int aId, int bId);
	int			FindPath_Block(int startParentId, int goalParentId);
	int			FindPath_NpcObs(int startParentId, int goalParentId);
	int			FindFreeBlockAround(int nMainId, int nNearX, int nNearY);
	int			m_nGridW;
	int			m_nGridH;
	int			m_nGridTotal;
	int			m_nTargetX;
	int			m_nTargetY;
	int			m_nCurStep;
	UINT		m_uStepDelayTime;
	char		m_szPathName[FILE_NAME_LENGTH];
	std::vector<VGridNeighbour> m_vNeighbour;
	std::vector<int> m_vRetPath;
	VGridNode	m_GridNode[MAX_CELL];
	int		m_pTempCover[MAX_CELL];
	BOOL	m_bHavePath;
	BOOL	m_uPaintTime;
#endif
};

#ifndef TOOLVERSION
	#ifdef _SERVER
	extern KSubWorld* SubWorld;
	#else
	extern KSubWorld	SubWorld[MAX_SUBWORLD];
	#endif
#else 
	extern CORE_API KSubWorld	SubWorld[MAX_SUBWORLD];
#endif
#endif
