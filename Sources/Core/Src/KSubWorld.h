#ifndef	KWorldH
#define	KWorldH

#ifdef _SERVER
#define	MAX_SUBWORLD	180//edit by phong kieu max maps mac dinh 80
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
	int			m_nWeather;						//	ÌìÆø±ä»¯
	DWORD		m_dwCurrentTime;				//	µ±Ç°Ö¡
	KWorldMsg	m_WorldMessage;					//	ÏûÏ¢
	KList		m_NoneRegionNpcList;			//	²»ÔÚµØÍ¼ÉÏµÄNPC
	char		m_CityOwnTong[32];
	BYTE		m_CityTax;

#ifdef _SERVER
	KWeatherMgr *m_pWeatherMgr;
#endif
private:
public:
	KSubWorld();
	~KSubWorld();
	void		Activate();
	void		GetFreeObjPos(POINT& pos);
	void		GetRandomObjPos(POINT& pos);	//add by phong kiÒu xö lý t¹m thay cho hµm GetFreeObjPos sau nµy fix ®­îc sö dông l¹i
	BOOL		CanPutObj(POINT pos);
	void		ObjChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nObjIdx);
	void		MissleChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nObjIdx);
	void		AddPlayer(int nRegion, int nIdx);
	void		RemovePlayer(int nRegion, int nIdx);
	void		Close();
	int			GetDistance(int nRx1, int nRy1, int nRx2, int nRy2);						//
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
	int	        DelAllNpcInWro(); //É¾³ý¸ÃµØÍ¼ÉÏµÄËùÓÐNPC
	long	 CountAllNpc();
	void		SetTrap(DWORD dwTrapId, int nMpsX, int nMpsY, int nRange); // AddTrap Fong KiÒu
	void		SetObstacle(long value, int nMpsX, int nMpsY, int nRange); //#Set VËt C¶n
	BOOL ExecuteScript(char * ScriptFileName, char * szFunName, int nParam);
	BOOL ExecuteScript(DWORD dwScriptId,  char * szFunName, int nParam);
#endif
#ifndef _SERVER
	BOOL		LoadMap(int nIdx, int nRegion);
	void		NpcChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nNpcIdx);
	void		Paint();
	void		Mps2Screen(int *Rx, int *Ry);
	void		Screen2Mps(int *Rx, int *Ry);
#endif
private:
	void		LoadTrap();
	void		ProcessMsg(KWorldMsgNode *pMsg);
#ifndef _SERVER
	void		LoadCell();
#endif
};

#ifndef TOOLVERSION
extern KSubWorld	SubWorld[MAX_SUBWORLD];
#else 
extern CORE_API KSubWorld	SubWorld[MAX_SUBWORLD];
#endif
#endif
