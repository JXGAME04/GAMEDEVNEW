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

// ==== Luoi tim duong (A* theo BLOCK) - DUNG CHUNG cho ca client lan server ====
// Truoc day ca khoi nay nam trong #ifndef _SERVER nen server khong he co A*.
// Cau truc phai o NGOAI moi cong bien dich vi hai phia deu dung. Rieng MAX_CELL thi
// CHI client dung (mang tinh); server cap phat heap dung co luoi that.
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

// (het khoi cau truc luoi - KHONG dong guard nua, hai phia deu can)

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
	// (ProcLoadPathGrid da khai o khoi dung chung ben duoi - hai phia deu can)
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
#endif

	// ==== A* theo block: cac ham duoi la THUAT TOAN THUAN, dung chung hai phia ====
public:
	// PHAI public: ham luong LoadPathGrid (ngoai lop, KSubWorld.cpp:134) goi vao day.
	void		ProcLoadPathGrid();	// dung luoi tu du lieu vat can (dung chung)
private:
	// Chung von bi nhot trong #ifndef _SERVER nen server khong co duong tim duong nao
	// ngoai bo men-tuong cua KNpcFindPath - ma bo do chi ne duoc DUNG MOT vat can moi
	// lenh (KNpcFindPath.cpp:106-111). Mo cong cho server dung lai nguyen van.
	int			BlockHeuristic(int aId, int bId);
	int			FindPath_Block(int startParentId, int goalParentId);
	int			FindPath_NpcObs(int startParentId, int goalParentId);
	int			FindFreeBlockAround(int nMainId, int nNearX, int nNearY);

#ifdef _SERVER
	// ---- Luoi tim duong phia SERVER ----
	// Client dung MANG TINH VGridNode[MAX_CELL] = 2.400.000 x 20 byte = 48 MB; chap nhan
	// duoc vi client chi co MAX_SUBWORLD = 1. Server co MAX_SUBWORLD = 1000 nen be nguyen
	// se ton hang chuc GB. => cap phat HEAP dung co luoi that: ban do 12x12 region
	// (192 x 384 = 73.728 o) chi ton 1,47 MB, va chi nap cho ban do nao thuc su can.
	// m_GridNode == NULL = ban do chua nap luoi -> ben goi phai tu lui ve cach khac.
	// GIU NGUYEN TEN voi ban client de than ham ProcLoadPathGrid dung chung duoc.
	VGridNode*	m_GridNode;
	int*		m_pTempCover;
	int			m_nGridW;
	int			m_nGridH;
	int			m_nGridTotal;
	BOOL		m_bHavePath;
	char		m_szPathName[FILE_NAME_LENGTH];
	std::vector<VGridNeighbour> m_vNeighbour;
	std::vector<int> m_vRetPath;
	// Bo nho nhap A* GIU LAI giua cac lan goi (assign() giu nguyen capacity): bot tim duong
	// lien tuc, cap/giai phong ~1,3 MB moi lan x nhieu bot lam phan manh heap khong tra lai.
	// HE QUA: FindPathServer chi an toan tren MOT luong (vong lap game).
	std::vector<int>			m_aGCost;
	std::vector<int>			m_aCameFrom;
	std::vector<unsigned char>	m_aClosed;
	// The-he cham tung o (18/08): o mang the he khac lan goi nay = CHUA DUNG,
	// khoi tao luoi khi cham toi - khong phai do day ca 3 mang (~650KB) moi lan.
	std::vector<unsigned int>	m_aTheHe;
	unsigned int				m_nTheHe;
	// Chi de than ham dung chung bien dich duoc; server khong dung.
	int			m_nTargetX;
	int			m_nTargetY;
	int			m_nCurStep;
	BOOL		m_bStopThread;
public:
	// Nap luoi cho ban do nay (cap phat heap dung co luoi). Khong lam gi neu da nap.
	void		LoadPathGridSrv(const char* szPathName, int nGridW, int nGridH);
	// Tra: 1 = duong day du, 2 = duong mot phan, 0 = khong co duong,
	// -1 = dau vao khong hop le. vOutPath = danh sach id BLOCK theo thu tu di.
	int			FindPathServer(int nStartMpsX, int nStartMpsY, int nDestMpsX, int nDestMpsY,
						  std::vector<int>& vOutPath, bool bCheckNpc);
	// Doi id block -> toa do MPS tam block (de phat lenh di).
	bool		BlockCenterMps(int nBlockId, int& nMpsX, int& nMpsY);
	// Diem GAN NHAT ben trong hinh chu nhat cua block (thut vao nua o) so voi (nFromX,Y).
	// Dung lam waypoint thay cho TAM block: block to toi 16x32 o nen tam co the nam
	// NGUOC HUONG di - nham tam la bot chay qua lai nhu con lac.
	bool		BlockNearestMps(int nBlockId, int nFromX, int nFromY, int& nMpsX, int& nMpsY);
	// (19/08 chieu) O luoi tai MPS nay co la vat can khong? 1 = vat can, 0 = di duoc,
	// -1 = chua biet (luoi chua nap / toa do ngoai luoi). Cho he bot tu phat hien
	// minh bi NAP LAI vao vung rong (nay da doi thanh vat can) ma tu cuu [BotCuu].
	int			CellObsSrv(int nMpsX, int nMpsY);
private:
#endif

#ifndef _SERVER
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
	// Vung nhap A* dung lai - PHAI co o ca nhanh CLIENT vi FindPath_Block/
	// FindPath_NpcObs (KSubWorld.cpp) khong nam trong #ifdef _SERVER.
	std::vector<int>			m_aGCost;
	std::vector<int>			m_aCameFrom;
	std::vector<unsigned char>	m_aClosed;
	std::vector<unsigned int>	m_aTheHe;
	unsigned int				m_nTheHe;
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
