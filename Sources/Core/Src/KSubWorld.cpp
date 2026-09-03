#include "KCore.h"
#include <math.h>
#include "KRegion.h"
#include "KMath.h"
#include "KNpc.h"
#include "KNpcTemplate.h"
#include "KPlayer.h"
#include "KNpcSet.h"
#include "KObjSet.h"
#include "KPlayerSet.h"
#include "KMissleSet.h"
#ifndef _STANDALONE
#include "crtdbg.h"
#endif
#include "Scene/ObstacleDef.h"
// KCombinFileSection / REGION_ELEM_FILE_COUNT dung trong ProcLoadPathGrid. Truoc day
// ham do chi bien dich cho client nen keo duoc gian tiep; nay dung chung hai phia thi
// phai include tuong minh.
#include "Scene/SceneDataDef.h"
#ifdef _SERVER
//#include "KNetServer.h"
//#include "../MultiServer/Heaven/Interface/iServer.h"
#endif
#ifndef _SERVER
#include "scene/KScenePlaceC.h"
#include <queue>
#include <algorithm>
extern BOOL			g_bPaintInterpFocus;	// CoreShell.cpp: PaintFps interpolation drives the camera
extern void			S13_ClearCmd(int nIdx);	// [S13] KNpc.cpp: xoa khe lenh dang giu cua chinh minh
#endif
#include "KSubWorld.h"

#ifndef TOOLVERSION
	#ifdef _SERVER
	KSubWorld* SubWorld = new KSubWorld[MAX_SUBWORLD];

	#else
	KSubWorld	SubWorld[MAX_SUBWORLD];
	#endif
#else 
	CORE_API KSubWorld	SubWorld[MAX_SUBWORLD];
#endif

#define		defLOGIC_CELL_WIDTH			32
#define		defLOGIC_CELL_HEIGHT		32

#ifdef _SERVER
KGlobalMissionArray g_GlobalMissionArray;
#endif

KSubWorld::KSubWorld()
{
	m_Region = NULL;
	m_nRegionWidth	= 512 / 32;
	m_nRegionHeight	= 1024 / 32;
	m_nCellWidth = defLOGIC_CELL_WIDTH;
	m_nCellHeight = defLOGIC_CELL_HEIGHT;
	m_dwCurrentTime = 0;
	m_SubWorldID = -1;
	m_nWeather = WEATHERID_NOTHING;
#ifndef _SERVER
	m_nWorldRegionWidth = 3;
	m_nWorldRegionHeight = 3;
	memset(m_ClientRegionIdx, 0, sizeof(this->m_ClientRegionIdx));
	memset(this->m_szMapPath, 0, sizeof(this->m_szMapPath));
#endif
#ifdef _SERVER
	m_nWorldRegionWidth = 0;
	m_nWorldRegionHeight = 0;
	m_pWeatherMgr = NULL;

	for (int i = 0; i < m_MissionArray.GetTotalCount(); i++)
	{
	    m_MissionArray.m_Data[i].SetOwner(this);
	}
#endif
	m_nTotalRegion = m_nWorldRegionWidth * m_nWorldRegionHeight;
	m_CityTax = 0;
	ZeroMemory(m_CityOwnTong, sizeof(m_CityOwnTong));

	memset(szMapName, 0, sizeof(szMapName));
	memset(szMapType, 0, sizeof(szMapType));
	memset(szGoldenDropRate, 0, sizeof(szGoldenDropRate));
	memset(szNormalDropRate, 0, sizeof(szNormalDropRate));

	m_NpcSeriesAuto = 0;
	m_NpcSeriesMetal = 0; // Default to 0 for metal series rate
	m_NpcSeriesWood = 0;  // Default to 0 for wood series rate
	m_NpcSeriesWater = 0; // Default to 0 for water series rate
	m_NpcSeriesFire = 0;  // Default to 0 for fire series rate
	m_NpcSeriesEarth = 0; // Default to 0 for earth series rate

	nzAutoGoldenNpc = 0;
	nzGoldenType = 0;
	m_nRegionBeginX = 0;
	m_nRegionBeginY = 0;

	m_nIndex = -1;
#ifndef _SERVER
	m_hLoadPathGrid = NULL;
    m_bStopThread = FALSE;
	m_nTargetX = 0;
	m_nTargetY = 0;
	m_bHavePath = FALSE;
	m_uPaintTime = 0;
    m_dwLastNpcCheck = 0;
#endif
}

KSubWorld::~KSubWorld()
{
	if (m_Region)
	{
		delete [] m_Region;
		m_Region = NULL;
	}

#ifdef _SERVER
	if(m_pWeatherMgr)
	{
		delete m_pWeatherMgr;
		m_pWeatherMgr = NULL;
	}
#endif
#ifndef _SERVER
	if(m_hLoadPathGrid)
	{
		m_bStopThread = TRUE;
		WaitForSingleObject(m_hLoadPathGrid, INFINITE);
        CloseHandle(m_hLoadPathGrid);
        m_hLoadPathGrid = NULL;
	}
	m_bHavePath = FALSE;
#endif
}

#ifndef _SERVER

DWORD WINAPI LoadPathGrid(void* pParam)
{
	KSubWorld* pSW = (KSubWorld*)pParam;
	pSW->ProcLoadPathGrid();
	return 0;
}

#endif	// het phan CHI CLIENT (luong nap luoi chay nen)

// ==========================================================================
// Tu day toi truoc KSubWorld::FindPath la THUAT TOAN THUAN, DUNG CHUNG hai phia.
// Truoc day ca khoi nam trong #ifndef _SERVER nen server khong he co A*, phai
// dua vao bo men-tuong cua KNpcFindPath - ma bo do chi ne duoc DUNG MOT vat can
// moi lenh SendCommand (KNpcFindPath.cpp:106-111), nen bot di xa la ket.
// Server nap luoi qua LoadPathGridSrv (goi ProcLoadPathGrid dong bo, khong dung luong).
// ==========================================================================

struct AStarNode
{
    int id;
    int f; // g + h
};

struct AStarCompare
{
    bool operator()(const AStarNode& a, const AStarNode& b) const
    {
        return a.f > b.f;
    }
};

struct BlockSize { int w, h; };

BlockSize gbsizes[] = {
    {16, 32},
    //{16, 16},
    {8, 16},
    //{8, 8},
    {4, 8},
    //{4, 4},
    {2, 4},
    //{2, 2},
    {1, 2},
    {1, 1}
};
const int NUM_SIZES = sizeof(gbsizes)/sizeof(BlockSize);

inline void AddNeighbourUnique(int* arr, int& count, int maxCount, int value)
{
    for (int i = 0; i < count; ++i)
    {
        if (arr[i] == value)
            return;
    }
    if (count < maxCount)
    {
        arr[count++] = value;
    }
}

// (19/08) g_DebugLog CHI gui WM_COPYDATA toi cua so DebugWin.exe (Engine KDebug.cpp:44)
// - KHONG ghi file. Cac dong [PathSrv] chan doan luoi phai vao bot.log moi doc duoc,
// nen dung PB_LogNgoai cua he bot (KPlayerBot.cpp). Khai extern tai cho: KSubWorld.cpp
// khong include KPlayerBot.h (khuon giong ScriptFuns.cpp dang lam voi cac ham PB_).
#ifdef _SERVER
extern void PB_LogNgoai(const char* szFmt, ...);
#endif
extern int g_nPbNpcChan;	// KRegion.cpp - cong tac 'NPC la tuong' (0 = nguoi/bot/quai khong chan nhau)

void KSubWorld::ProcLoadPathGrid()
{
	if(m_bStopThread)
		return;
	int nAllCellW = m_nGridW*m_nRegionWidth;
	int nAllCellH = m_nGridH*m_nRegionHeight;
	int nAllCell  = nAllCellW * nAllCellH;
	char	File[MAX_PATH];
	int		ObstacleInfo[REGION_GRID_WIDTH][REGION_GRID_HEIGHT];
	int		nRegThieu = 0, nRegKhongObs = 0;	// (20/08) nRegThieu = region KHONG mo duoc TEP nao (ngoai map that -> vat can); nRegKhongObs = CO tep ma thieu doan vat can (dat trong co that -> DI DUOC)
	// (20/08 dem - chu game duyet "Lam Siet luat region") dem truoc so region mo
	// duoc _Region_S.dat trong cua so: nSTong > 0 nghia la ban do nay DUOC server
	// dinh nghia bang bo tep _S, khi do region khong co _S la NGOAI MAP THAT va
	// pak client KHONG duoc khai sinh no (xem nhanh lui _Region_C ben duoi). Do
	// that 20/08 dem: 65 ban do / 1.276.115 o bi _C mo trang ngoai ria map that
	// (map 224 nguyen vanh dai 54k o quanh map) - bot di vao do la "ra khoi map"
	// ma moi bo dem deu bao on vi luoi ghi DI DUOC.
#ifdef _SERVER
	int nSTong = 0;
	for (int h0 = 0; h0 < m_nGridH && !m_bStopThread; ++h0)
		for (int w0 = 0; w0 < m_nGridW; ++w0)
		{
			sprintf(File, "%s\\v_%03d\\%03d_Region_S.dat", m_szPathName,
						m_nRegionBeginY+h0, m_nRegionBeginX+w0);
			KPakFile DataPre;
			if (DataPre.Open(File))
				nSTong++;
		}
#endif
	for(int h=0; h < m_nGridH; ++h)
	{
		if(m_bStopThread)
			return;
		for(int w=0; w < m_nGridW; ++w)
		{
			if(m_bStopThread)
				return;
			// BAY DA LAM SAI SUOT: khoi vat can nam trong maps.pak. Ban CLIENT dong goi
			// "%03d_Region_C.dat" cho moi region, ban SERVER dong "%03d_Region_S.dat"
			// (SceneDataDef.h: REGION_COMBIN_FILE_NAME_SERVER). Hoi sai ten thi Data.Open
			// tra false IM LANG, ObstacleInfo o duoi da bi memset ve 0 va giu nguyen 0,
			// nen ca luoi thanh "khong co vat can" -> A* ve duong XUYEN TUONG.
			// Do that tren cay chay: 292 tep _Region_S.dat va 480 tep _Region_C.dat, tuc
			// CO ban do chi co ban client. Nen server thu S truoc roi LUI VE C, va neu ca
			// hai deu khong mo duoc thi BAO RO chu khong im lang.
			// (19/08 dinh chinh) Con so "292 tep _S va 480 tep _C" o tren la SAI khi doc
			// nhu ty le: hai phep dem thuoc HAI THU MUC ROI NHAU (292 = rieng map
			// Kiem Mon Quan, map do co 0 tep _C; 479/480 tep _C nam o map fongkieu co 0 tep _S).
			// Tong that tren cay chay: 2638 tep _S vs 480 tep _C. Nhung KET LUAN van
			// dung: CO map chi dong goi ban _C -> phai co fallback doc _Region_C.
#ifdef _SERVER
			sprintf(File, "%s\\v_%03d\\%03d_Region_S.dat", m_szPathName,
						m_nRegionBeginY+h, m_nRegionBeginX+w);
#else
			sprintf(File, "%s\\v_%03d\\%03d_Region_C.dat", m_szPathName,
						m_nRegionBeginY+h, m_nRegionBeginX+w);
#endif
			memset(ObstacleInfo, 0, sizeof(ObstacleInfo));
			int bCoObs = 0, bCoTep = 0;	// bCoObs = doc duoc doan vat can; bCoTep = mo duoc tep region (20/08)
			KPakFile	Data;
			if (Data.Open(File))
			{
				unsigned int uMaxElemFile = 0; bCoTep = 1;
				Data.Read(&uMaxElemFile, sizeof(unsigned int));
				KCombinFileSection	ElemFile[REGION_ELEM_FILE_COUNT] = { 0 };
				if (uMaxElemFile > REGION_ELEM_FILE_COUNT)
				{
					Data.Read(&ElemFile[0], sizeof(KCombinFileSection) * REGION_ELEM_FILE_COUNT);
					Data.Seek(sizeof(KCombinFileSection) * (uMaxElemFile - REGION_ELEM_FILE_COUNT), FILE_CURRENT);
				}
				else
				{
					Data.Read(&ElemFile[0], sizeof(KCombinFileSection) * uMaxElemFile);
				}
				unsigned int uOffsetAhead = sizeof(unsigned int) + sizeof(KCombinFileSection) * uMaxElemFile;
				if (ElemFile[REGION_OBSTACLE_FILE_INDEX].uLength >= sizeof(ObstacleInfo))
				{
					Data.Seek(uOffsetAhead + ElemFile[REGION_OBSTACLE_FILE_INDEX].uOffset, FILE_BEGIN);
					Data.Read((LPVOID)ObstacleInfo, sizeof(ObstacleInfo));
					bCoObs = 1;
				}
				else
				{
					memset(ObstacleInfo, 0, sizeof(int)*REGION_GRID_WIDTH*REGION_GRID_HEIGHT);
				}
			}
			// (20/08 phan bien) LUI VE _Region_C / _OBSTACLE.DAT PHAI chay CA KHI
			// _Region_S.dat MO DUOC ma ben trong KHONG co doan vat can (tep stub
			// 149-308 byte): truoc day nhanh nay nam trong "else" cua Data.Open(_S)
			// nen region stub khong bao gio duoc thu ban _C - ma tu 20/08 region stub
			// duoc coi la DAT TRONG (di duoc), nen neu ban _C co tuong that thi ta se
			// son trang mat buc tuong do. Do tren maps.pak hien tai: 513 region stub,
			// KHONG con nao co _C/_OBSTACLE - nhung luat phai dung, khong dua vao du lieu.
			// (20/08 dem) LUAT REGION - chu game duyet "Lam Siet luat region: chi
			// tep _Region_S.dat cua server moi duoc dinh nghia": _Region_C cua
			// client chi duoc CAP DU LIEU vat can cho region DA ton tai (_S mo
			// duoc), KHONG duoc khai sinh region moi ngoai ria map. Chi ap luat
			// khi cua so co it nhat mot tep _S (nSTong > 0): ban do dong goi TOAN
			// _C (fongkieu) van dung duong lui cu nhu truoc.
			int bChoLui = 1;
#ifdef _SERVER
			if (nSTong > 0 && !bCoTep)
				bChoLui = 0;
#endif
			if (!bCoObs && bChoLui)
			{
				// (19/08 phan bien) LUI VE ban CLIENT _Region_C.dat - chu thich 18/08 o tren
				// HUA nhung code chua he lam: map chi dong goi ban _C ma coi la "thieu toan
				// bo" thi luoi 100% vat can, bot te liet ca map. Cung dinh dang combin.
				sprintf(File, "%s\\v_%03d\\%03d_Region_C.dat", m_szPathName,
						m_nRegionBeginY+h, m_nRegionBeginX+w);
				if (Data.Open(File))
				{
					unsigned int uMaxElemFile2 = 0; bCoTep = 1;
					Data.Read(&uMaxElemFile2, sizeof(unsigned int));
					KCombinFileSection	ElemFile2[REGION_ELEM_FILE_COUNT] = { 0 };
					if (uMaxElemFile2 > REGION_ELEM_FILE_COUNT)
					{
						Data.Read(&ElemFile2[0], sizeof(KCombinFileSection) * REGION_ELEM_FILE_COUNT);
						Data.Seek(sizeof(KCombinFileSection) * (uMaxElemFile2 - REGION_ELEM_FILE_COUNT), FILE_CURRENT);
					}
					else
					{
						Data.Read(&ElemFile2[0], sizeof(KCombinFileSection) * uMaxElemFile2);
					}
					unsigned int uOffsetAhead2 = sizeof(unsigned int) + sizeof(KCombinFileSection) * uMaxElemFile2;
					if (ElemFile2[REGION_OBSTACLE_FILE_INDEX].uLength >= sizeof(ObstacleInfo))
					{
						Data.Seek(uOffsetAhead2 + ElemFile2[REGION_OBSTACLE_FILE_INDEX].uOffset, FILE_BEGIN);
						Data.Read((LPVOID)ObstacleInfo, sizeof(ObstacleInfo));
						bCoObs = 1;
					}
				}
				if (!bCoObs)
				{
					sprintf(File, "%s\\v_%03d\\%03d_OBSTACLE.DAT", m_szPathName,
							m_nRegionBeginY+h, m_nRegionBeginX+w);
					KPakFile Data2;
					if (Data2.Open(File))
					{
						unsigned int uSize = Data2.Size(); bCoTep = 1;
						if(uSize >= sizeof(ObstacleInfo))
						{
							Data2.Read((LPVOID)ObstacleInfo, sizeof(ObstacleInfo));
							bCoObs = 1;
						}
					}
				}
			}
			if (!bCoObs)
				{ if (bCoTep) nRegKhongObs++; else nRegThieu++; }
			int regx = w*REGION_GRID_WIDTH;
			int regy = h*REGION_GRID_HEIGHT;
			for(int y=0;y<REGION_GRID_HEIGHT;++y)
			{
				for(int x=0;x<REGION_GRID_WIDTH;++x)
				{
					int id = (regy+y)*nAllCellW + regx+x;
					m_GridNode[id].x = m_nRegionBeginX*REGION_GRID_WIDTH + regx+x;
					m_GridNode[id].y = m_nRegionBeginY*REGION_GRID_HEIGHT + regy+y;
					int lInfo = ObstacleInfo[x][y];
					lInfo &= 0x0000000f;
					// 18/08: O GOC (Obstacle_LT..RB) truoc day coi la DI DUOC ca o, nhung
					// engine that chi cho di NUA O trong (GetBarrier kiem cheo nDx+nDy,
					// KRegion.cpp:927-941). Vien vach nui toan o goc -> chuoi nua-o thanh
					// "cau thang" cho bot leo xuyen vach ("bot tu chay vao" - chu game).
					// Coi o goc la VAT CAN: bot di vong nhu nguoi that. Doi predicate =
					// doi luoi -> da nang kMagic cache de moi ban do tu tinh lai.
					// (20/08 SUA GOC) TACH HAI CA: region mo duoc TEP ma trong tep KHONG co
					// doan vat can = DAT TRONG CO THAT (dong bang, quang truong) -> DI DUOC.
					// Chi khi KHONG mo duoc tep nao moi coi la ngoai map -> VAT CAN. Luat 19/08
					// gop hai ca lam mot nen da SON DAC ca vung dat that: diem dap CHINH THONG
					// cua Lam Du Quan (319:1630,3592) + Chan nui TB (320:1146,3130) nam GON trong
#ifdef _SERVER
					// (19/08) chi SERVER ap luat "thieu du lieu = vat can": cache client %d.fp
					// khoa bang FINDPATH_VERSION=0 khong doi, ap chung se lech ngu nghia.
					if(lInfo == 0 && bCoTep)   // bCoObs => bCoTep, xem chu thich tren
#else
					if(lInfo == 0)
#endif
					m_GridNode[id].obs = 0;
					else
					m_GridNode[id].obs = 1;
					m_GridNode[id].parentId = id;
					m_pTempCover[id] = FALSE;
				}
			}
		}
	}
#ifdef _SERVER	// PB_LogNgoai chi co o build server (bot); client khong link duoc
	if (nRegThieu >= m_nGridW * m_nGridH)
		PB_LogNgoai("[PathSrv] CANH BAO map %d: TOAN BO %d region khong co du lieu vat can"
		            " - luoi 100%% vat can, BOT KHONG DI DUOC tren map nay (bo sung"
		            " _Region_S/_Region_C hoac rut map khoi bai luyen)\n",
		            m_SubWorldID, nRegThieu);
	else if (nRegThieu > 0 || nRegKhongObs > 0)
		PB_LogNgoai("[PathSrv] map %d: %d/%d region KHONG co TEP du lieu (%d region co tep ma thieu doan vat can -> DI DUOC)"
		            " -> chi so DAU coi la VAT CAN (ngoai map that); so trong ngoac DI DUOC [luat-S]\n",
		            m_SubWorldID, nRegThieu, m_nGridW * m_nGridH, nRegKhongObs);
#endif
	for (int si = 0; si < NUM_SIZES; ++si)
	{
		if(m_bStopThread)
			return;
		int bw = gbsizes[si].w;
		int bh = gbsizes[si].h;
	
		for (int y = 0; y + bh <= nAllCellH; ++y)
		{
			if(m_bStopThread)
				return;
			for (int x = 0; x + bw <= nAllCellW; ++x)
			{
				if(m_bStopThread)
					return;
				int id0 = y * nAllCellW + x;
	
				if (m_pTempCover[id0])
					continue;
	
				int type = m_GridNode[id0].obs;
				bool ok = true;
				int xx,yy;
				for ( yy = y; yy < y + bh && ok; ++yy)
				{
					for ( xx = x; xx < x + bw; ++xx)
					{
						int id = yy * nAllCellW + xx;
						if (m_pTempCover[id] || m_GridNode[id].obs != type)
						{
							ok = false;
							break;
						}
					}
				}
	
				if (!ok)
					continue;
	
				int parentId = id0;
				m_GridNode[parentId].parentId = parentId;
				m_GridNode[parentId].w = bw;
				m_GridNode[parentId].h = bh;
				for ( yy = y; yy < y + bh; ++yy)
				{
					for ( xx = x; xx < x + bw; ++xx)
					{
						int id = yy * nAllCellW + xx;
						m_pTempCover[id] = TRUE;
						m_GridNode[id].parentId = parentId;
					}
				}
			}
		}
	}
	//connecting neighbours
	const int DIRS[8][2] =
    {
        {-1, -1}, {0, -1}, {1, -1},
        {-1,  0},          {1,  0},
        {-1,  1}, {0,  1}, {1,  1}
    };
	for (int parentId = 0; parentId < nAllCell; ++parentId)
	{
		if(m_bStopThread)
			return;
		VGridNode &node = m_GridNode[parentId];
	
		if (node.parentId != parentId)	//kh«ng ph¶i cell tæng
			continue;
	
		if (node.obs)
			continue;        // block nµy cã obstacle
	
		int px = parentId % nAllCellW;
		int py = parentId / nAllCellW;
		int bw = node.w;
		int bh = node.h;
	
		// m¶ng id connect tèi ®a 100
		int tmpNeighbours[100];
		int tmpCount = 0;
	
		// DuyÖt ngoµi biªn cña block
		for (int cy = py; cy < py + bh; ++cy)
		{
			if(m_bStopThread)
				return;
			for (int cx = px; cx < px + bw; ++cx)
			{
				if(m_bStopThread)
				return;
				if (!(cx == px || cx == px + bw - 1 ||
					cy == py || cy == py + bh - 1))
					continue; // bá qua ruét cña block, chØ xö lý viÒn
				// check 8 h­íng
				for (int d = 0; d < 8; ++d)
				{
					int dx = DIRS[d][0];
					int dy = DIRS[d][1];
	
					int nx = cx + dx;
					int ny = cy + dy;
					//ngoµi map th× bá qua
					if (nx < 0 || nx >= nAllCellW || ny < 0 || ny >= nAllCellH)
						continue;
	
					int nid = ny * nAllCellW + nx;
					int nParent = m_GridNode[nid].parentId;
	
					if (nParent == parentId)	//cïng block
						continue;
	
					// neighbour cã obs
					if (m_GridNode[nParent].obs)
						continue;
	
					// check 4 gãc, v­íng 1 trong 2 « th× bá qua
					if (dx != 0 && dy != 0)
					{
						int sx1 = cx + dx;	//b­íc ngang
						int sy1 = cy;
						int sx2 = cx;
						int sy2 = cy + dy;	//b­íc däc
						//nÕu 1 trong 2 « ra ngoµi map th× xem nh­ v­íng
						if (sx1 < 0 || sx1 >= nAllCellW || sy1 < 0 || sy1 >= nAllCellH)
							continue;
						if (sx2 < 0 || sx2 >= nAllCellW || sy2 < 0 || sy2 >= nAllCellH)
							continue;
	
						int sid1 = sy1 * nAllCellW + sx1;
						int sid2 = sy2 * nAllCellW + sx2;
						//nÕu 1 trong 2 « cã obs th× bÞ v­íng
						if (m_GridNode[sid1].obs || m_GridNode[sid2].obs)
							continue;
					}
					// t×m ®­îc neighbour hîp lÖ
					AddNeighbourUnique(tmpNeighbours, tmpCount, 100, nParent);
				}
			}
		}
	
		if (tmpCount == 0)
			continue;
	
		node.connStart = (int)m_vNeighbour.size();
		node.connCount = tmpCount;
	
		for (int i = 0; i < tmpCount; ++i)
		{
			VGridNeighbour nb;
			nb.toParentId = tmpNeighbours[i];
			nb.cost = g_GetDistance(node.x*32 + node.w*32/2, node.y*32 + node.h*32/2,
						m_GridNode[nb.toParentId].x*32 + m_GridNode[nb.toParentId].w*32/2,
						m_GridNode[nb.toParentId].y*32 + m_GridNode[nb.toParentId].h*32/2);
			m_vNeighbour.push_back(nb);
		}
	}
}

int KSubWorld::BlockHeuristic(int aId, int bId)
{
    const VGridNode& a = m_GridNode[aId];
    const VGridNode& b = m_GridNode[bId];

    int ax = a.x * 32 + a.w * 32 / 2;
    int ay = a.y * 32 + a.h * 32 / 2;
    int bx = b.x * 32 + b.w * 32 / 2;
    int by = b.y * 32 + b.h * 32 / 2;

    return g_GetDistance(ax, ay, bx, by);
}

// Dem so lan A* chay - KPlayerBot.cpp doc de in [BotPerf] (dieu tra lag 18/08).
int g_nPbAstarDem = 0;

// Cham mot o cua vung nhap A* dung lai: o mang the-he khac lan goi nay = CHUA
// DUNG trong lan nay -> khoi tao ngay tai cho. Nho vay moi lan tim duong khong
// phai cap phat + do day 3 mang ~nodeCount phan tu (~650KB) nhu truoc - nguon
// "lag hon nhieu" 18/08 khi 20 bot tim duong lien tuc.
#define PB_ASTAR_CHAM(x) do { if (m_aTheHe[(x)] != m_nTheHe) { \
        m_aTheHe[(x)] = m_nTheHe; m_aGCost[(x)] = 0x3f3f3f3f; \
        m_aCameFrom[(x)] = -1; m_aClosed[(x)] = 0; } } while (0)

int KSubWorld::FindPath_Block(int startParentId, int goalParentId)
{
    m_vRetPath.clear();

    if (startParentId == goalParentId)
    {
        m_vRetPath.push_back(startParentId);
        return 1;
    }

    int nAllCellW = m_nGridW * m_nRegionWidth;
    int nAllCellH = m_nGridH * m_nRegionHeight;
    int nodeCount = nAllCellW * nAllCellH;

    const int INF = 0x3f3f3f3f;

    // vung nhap dung lai (m_aGCost... khai o KSubWorld.h tu truoc nhung chua noi)
    g_nPbAstarDem++;
    if ((int)m_aGCost.size() < nodeCount)
    {
        m_aGCost.resize(nodeCount);
        m_aCameFrom.resize(nodeCount);
        m_aClosed.resize(nodeCount);
        m_aTheHe.assign(nodeCount, 0);
        m_nTheHe = 0;
    }
    if (++m_nTheHe == 0)
    {
        // quay vong 2^32: xoa dau the-he cu de khong trung ngau nhien
        m_aTheHe.assign(m_aTheHe.size(), 0);
        m_nTheHe = 1;
    }

    std::priority_queue<
        AStarNode,
        std::vector<AStarNode>,
        AStarCompare> open;

    PB_ASTAR_CHAM(startParentId);
    m_aGCost[startParentId] = 0;

    AStarNode s;
    s.id = startParentId;
    s.f  = BlockHeuristic(startParentId, goalParentId);
    open.push(s);

    bool found = false;
	int bestId   = -1;
    int bestH    = INF;
    while (!open.empty())
    {
        AStarNode cur = open.top();
        open.pop();
        int id = cur.id;
        PB_ASTAR_CHAM(id);
        if (m_aClosed[id])
            continue;
		
		int hToGoal = BlockHeuristic(id, goalParentId);
        if (hToGoal < bestH)
        {
            bestH  = hToGoal;
            bestId = id;
        }
		
        if (id == goalParentId)
        {
            found = true;
            break;
        }
        m_aClosed[id] = 1;
        VGridNode& node = m_GridNode[id];

        if (node.parentId != id || node.obs != 0)
            continue;

        if (node.connStart < 0 || node.connCount == 0)
            continue;

        int startIdx = node.connStart;
        int cnt      = node.connCount;

        for (int i = 0; i < cnt; ++i)
        {
            int nb = m_vNeighbour[startIdx + i].toParentId;

            VGridNode& nbNode = m_GridNode[nb];
            if (nbNode.parentId != nb || nbNode.obs != 0)
                continue;

            PB_ASTAR_CHAM(nb);
            if (m_aClosed[nb])
                continue;

            int newG = m_aGCost[id] + m_vNeighbour[startIdx + i].cost;
            if (newG < m_aGCost[nb])
            {
                m_aGCost[nb]     = newG;
                m_aCameFrom[nb]  = id;

                int h = BlockHeuristic(nb, goalParentId);

                AStarNode nn;
                nn.id = nb;
                nn.f  = newG + h;
                open.push(nn);
            }
        }
    }

    if (!found)
    {
        // goal unreachable
        if (bestId == -1)
            return 0;

        // reconstruct path
        int curId = bestId;
        while (curId != -1)
        {
            m_vRetPath.push_back(curId);
            curId = m_aCameFrom[curId];
        }
        std::reverse(m_vRetPath.begin(), m_vRetPath.end());
        return 2;
    }
	
    int curId = goalParentId;
    while (curId != -1)
    {
        m_vRetPath.push_back(curId);
        curId = m_aCameFrom[curId];
    }

    std::reverse(m_vRetPath.begin(), m_vRetPath.end());
    return 1;
}

int KSubWorld::FindPath_NpcObs(int startParentId, int goalParentId)
{
    m_vRetPath.clear();

    if (startParentId == goalParentId)
    {
        m_vRetPath.push_back(startParentId);
        return 1;
    }

    int nAllCellW = m_nGridW * m_nRegionWidth;
    int nAllCellH = m_nGridH * m_nRegionHeight;
    int nodeCount = nAllCellW * nAllCellH;

    const int INF = 0x3f3f3f3f;

    // vung nhap dung lai (m_aGCost... khai o KSubWorld.h tu truoc nhung chua noi)
    g_nPbAstarDem++;
    if ((int)m_aGCost.size() < nodeCount)
    {
        m_aGCost.resize(nodeCount);
        m_aCameFrom.resize(nodeCount);
        m_aClosed.resize(nodeCount);
        m_aTheHe.assign(nodeCount, 0);
        m_nTheHe = 0;
    }
    if (++m_nTheHe == 0)
    {
        // quay vong 2^32: xoa dau the-he cu de khong trung ngau nhien
        m_aTheHe.assign(m_aTheHe.size(), 0);
        m_nTheHe = 1;
    }

    std::priority_queue<
        AStarNode,
        std::vector<AStarNode>,
        AStarCompare> open;

    PB_ASTAR_CHAM(startParentId);
    m_aGCost[startParentId] = 0;

    AStarNode s;
    s.id = startParentId;
    s.f  = BlockHeuristic(startParentId, goalParentId);
    open.push(s);

    bool found = false;
	int bestId   = -1;
    int bestH    = INF;
    while (!open.empty())
    {
        AStarNode cur = open.top();
        open.pop();
        int id = cur.id;
        PB_ASTAR_CHAM(id);
        if (m_aClosed[id])
            continue;
		
		int hToGoal = BlockHeuristic(id, goalParentId);
        if (hToGoal < bestH)
        {
            bestH  = hToGoal;
            bestId = id;
        }
		
        if (id == goalParentId)
        {
            found = true;
            break;
        }

        m_aClosed[id] = 1;
        VGridNode& node = m_GridNode[id];

        if (node.parentId != id || node.obs != 0)
            continue;

        if (node.connStart < 0 || node.connCount == 0)
            continue;

        int startIdx = node.connStart;
        int cnt      = node.connCount;

        for (int i = 0; i < cnt; ++i)
        {
            int nb = m_vNeighbour[startIdx + i].toParentId;

            VGridNode& nbNode = m_GridNode[nb];
            if (nbNode.parentId != nb || nbNode.obs != 0)
                continue;

            PB_ASTAR_CHAM(nb);
            if (m_aClosed[nb])
                continue;
			//reject cells with npc obstacles
			if(nbNode.w < 4 && nbNode.h < 8)
			{
				int nRegion, nMapX, nMapY, nOffX, nOffY;
				bool bObsNpc = true;
				for(int yy = 0;yy < (int)nbNode.h; ++yy)
				{
					if(!bObsNpc)
						break;
					for(int xx = 0;xx < (int)nbNode.w; ++xx)
					{
						Mps2Map((nbNode.x + xx)*32, (nbNode.y + yy)*32, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
						// (20/08 dem) cong tac NPC-la-tuong TAT -> khong o nao bi coi la
						// "co nguoi chan" nua (xem g_nPbNpcChan o KRegion.cpp)
						if(nRegion >= 0 && (!g_nPbNpcChan || m_Region[nRegion].GetRef(nMapX, nMapY, obj_npc) <= 0))
						{
							bObsNpc = false;
							break;
						}
					}
				}
				if(bObsNpc)
					continue;
			}
            int newG = m_aGCost[id] + m_vNeighbour[startIdx + i].cost;
            if (newG < m_aGCost[nb])
            {
                m_aGCost[nb]     = newG;
                m_aCameFrom[nb]  = id;

                int h = BlockHeuristic(nb, goalParentId);

                AStarNode nn;
                nn.id = nb;
                nn.f  = newG + h;
                open.push(nn);
            }
        }
    }

    if (!found)
    {
        // goal unreachable
        if (bestId == -1)
            return 0;

        // reconstruct path
        int curId = bestId;
        while (curId != -1)
        {
            m_vRetPath.push_back(curId);
            curId = m_aCameFrom[curId];
        }
        std::reverse(m_vRetPath.begin(), m_vRetPath.end());
        return 2;
    }

    int curId = goalParentId;
    while (curId != -1)
    {
        m_vRetPath.push_back(curId);
        curId = m_aCameFrom[curId];
    }

    std::reverse(m_vRetPath.begin(), m_vRetPath.end());
    return 1;
}

int KSubWorld::FindFreeBlockAround(int nMainId, int nNearX, int nNearY)
{
    int nAllCellW = m_nGridW * m_nRegionWidth;
    int nAllCellH = m_nGridH * m_nRegionHeight;

    VGridNode& b = m_GridNode[nMainId];
	//täa ®é cña block gèc
    int bx0 = nMainId % nAllCellW;
    int by0 = nMainId / nAllCellW;
    int bx1 = bx0 + b.w;  // exclusive
    int by1 = by0 + b.h;  // exclusive
	//ngoµi map -> thôt vµo trong
    int sx0 = bx0 - 1; if (sx0 < 0)           sx0 = 0;
    int sy0 = by0 - 1; if (sy0 < 0)           sy0 = 0;
    int sx1 = bx1;     if (sx1 >= nAllCellW) sx1 = nAllCellW - 1;
    int sy1 = by1;     if (sy1 >= nAllCellH) sy1 = nAllCellH - 1;

    int bestId   = -1;
    int bestDist = 0x7fffffff;
	//vÞ trÝ thùc cña block gèc
    //int cx = b.x * 32 + b.w * 32 / 2;
    //int cy = b.y * 32 + b.h * 32 / 2;

    for (int y = sy0; y <= sy1; ++y)
    {
        for (int x = sx0; x <= sx1; ++x)
        {
            if (x >= bx0 && x < bx1 &&
                y >= by0 && y < by1)	//bªn trong block gèc th× bá qua
            {
                continue;
            }

            int pid = y * nAllCellW + x;
            if (m_GridNode[pid].obs)
                continue;
            pid = m_GridNode[pid].parentId;
            if (m_GridNode[pid].obs)
                continue;

            VGridNode& nb = m_GridNode[pid];
            int px = nb.x * 32 + nb.w * 32 / 2;
            int py = nb.y * 32 + nb.h * 32 / 2;

            int d = g_GetDistance(px, py, nNearX, nNearY);

            if (d < bestDist)
            {
                bestDist = d;
                bestId   = pid;
            }
        }
    }

    return bestId;
}

#ifndef _SERVER	// FindPath co TRANG THAI (StopPath/m_nCurStep) - chi client dung

int KSubWorld::FindPath(int nX, int nY, bool bCheckNpc)
{
	StopPath();	
	if(!m_bHavePath || !m_nGridTotal || nX <= 0 || nY <= 0)
		return -1;
	if(m_hLoadPathGrid && WaitForSingleObject(m_hLoadPathGrid, 0) == WAIT_TIMEOUT)
		return -2;
	if(!Player[CLIENT_PLAYER_INDEX].m_nIndex)
		return -1;
		
	
	
	int nWidth = m_nGridW*REGION_GRID_WIDTH;
	int nHeight = m_nGridH*REGION_GRID_HEIGHT;
	int cx = nX/32-m_nRegionBeginX*REGION_GRID_WIDTH;
	int cy = nY/32-m_nRegionBeginY*REGION_GRID_HEIGHT;
	if(cx < 0)
		cx = 0;
	else if(cx >= nWidth)
		cx = nWidth - 1;
	if(cy < 0)
		cy = 0;
	else if(cy >= nHeight)
		cy = nHeight - 1;
	
	int tid = cy*nWidth + cx;
	int nCellsTotal = nWidth * nHeight;
	if (tid < 0 || tid >= nCellsTotal)
	    return -1;
	tid = m_GridNode[tid].parentId;
	int nx, ny;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nx, &ny);
	int cnx = nx/32-m_nRegionBeginX*REGION_GRID_WIDTH;
	int cny = ny/32-m_nRegionBeginY*REGION_GRID_HEIGHT;
	if(cnx < 0)
		cnx = 0;
	else if(cnx >= nWidth)
		cnx = nWidth - 1;
	if(cny < 0)
		cny = 0;
	else if(cny >= nHeight)
		cny = nHeight - 1;
	
	int sid = cny*nWidth + cnx;
	if (sid < 0 || sid >= nCellsTotal)
    	return -1;
	sid = m_GridNode[sid].parentId;
	if(m_GridNode[sid].obs)
	{
		sid = FindFreeBlockAround(sid, nx, ny);
		if(sid < 0)
			return -1;
	}
	int nRet;

	bool bNear = g_GetDistance(nX, nY, m_nTargetX, m_nTargetY) < 512;

	if (bCheckNpc && bNear && timeGetTime() - m_dwLastNpcCheck > 3000)
	{
	    m_dwLastNpcCheck = timeGetTime();
	    nRet = FindPath_NpcObs(sid, tid);
	}
	else
	{
	    nRet = FindPath_Block(sid, tid);
	}
	if(nRet <= 0)
	{
		StopPath();
	}
	else
	{
		if(nRet == 2)
		{
			tid = m_vRetPath[m_vRetPath.size()-1];
			m_nTargetX = m_GridNode[tid].x*32 + m_GridNode[tid].w*32/2;
			m_nTargetY = m_GridNode[tid].y*32 + m_GridNode[tid].h*32/2;
		}
		else
		{
			m_nTargetX = nX;
			m_nTargetY = nY;
		}
	}
	return nRet;
}

#endif


int KSubWorld::FindRegion(int nRegion)
{
	for (int i = 0; i < m_nTotalRegion; i++)
	{
		if (m_Region[i].m_RegionID == nRegion)
			return i;
	}
	return -1;
}

int KSubWorld::FindFreeRegion(int nX, int nY)
{
	if (nX == 0 && nY == 0)
	{
		for (int i = 0; i < m_nTotalRegion; i++)
		{
			if (m_Region[i].m_RegionID == -1)
				return i;
		}
	}
	else
	{
		for (int i = 0; i < m_nTotalRegion; i++)
		{
			if (m_Region[i].m_RegionID == -1)
				return i;
			int nRegoinX = LOWORD(m_Region[i].m_RegionID);
			int nRegoinY = HIWORD(m_Region[i].m_RegionID);

			if ((nX - nRegoinX) * (nX - nRegoinX) + (nY - nRegoinY) * (nY - nRegoinY) > 2)	// 
				return i;
		}
	}
	return -1;
}

extern int nActiveRegionCount;

#ifdef _SERVER
// [24/08] Bo vong quet region cua nhung ban do dang TRONG (xem chu thich trong
// KSubWorld::Activate). Dung mang static o pham vi tep - KHONG them truong vao
// lop KSubWorld, vi doi bo cuc lop la doi ABI qua ranh gioi CoreClient.dll <->
// Game.exe (Gate: cam dung cham bo cuc struct).
static int s_nRegionNgu[MAX_SUBWORLD] = { 0 };
#define SUBWORLD_KHUNG_NGU	9	// 9 khung = 0,5 giay o 18 khung/giay

static void SubWorld_DanhThucQuetRegion(const KSubWorld* pSubWorld)
{
	const int nIdx = (int)(pSubWorld - SubWorld);
	if (nIdx >= 0 && nIdx < MAX_SUBWORLD)
		s_nRegionNgu[nIdx] = 0;
}
#endif

#ifdef _SERVER
// [S13i 03/09] Chi so vung DANG duoc duyet trong KSubWorld::Activate (-1 = ngoai vong duyet; 0x7FFFFFFF = dang duyet
// danh sach VOID sau cac vung). Dung trong NpcChangeRegion de chi dat co 'da kich hoat' khi NPC sang vung con duoc
// duyet trong CUNG khung (chong kich hoat 2 lan); truoc day dat co cho MOI lan doi vung, sang vung chi so NHO hon (da
// duyet) thi co con nguyen sang khung sau -> KNpc::Activate tra ve ngay = NPC MAT MOT KHUNG (do: 39/44 khung mat cua
// nguoi choi ngay luc doi vung, lech troi ~14 mps/s so voi client; bot/quai cung bi). Client xoa co dau moi khung.
static int s_nS13iVungDangDuyet = -1;
#endif
void KSubWorld::Activate()
{
	if (m_SubWorldID < 0)
		return;
	m_dwCurrentTime ++;

#ifndef _SERVER
	g_ScenePlace.SetCurrentTime(m_dwCurrentTime);
	NpcSet.ClearActivateFlagOfAllNpc();
#endif

#ifdef _SERVER
	// [24/08] ~909 ban do duoc nap san, moi ban do ~370-500 region => quet het
	// la ~334.000 lan doc m_nActive MOI KHUNG (~6 trieu lan/giay), moi lan nhay
	// qua mot KRegion > 6 KB nen truot cache. Ban do dang TRONG thi toan bo
	// region deu IsActive()==0, quet chi de nhan ve 0 - bo di khong doi hanh vi.
	// Ngu toi da 0,5 giay roi TU quet lai (luoi an toan), va duoc danh thuc ngay
	// khi co nguoi vao map qua KSubWorld::AddPlayer - diem DUY NHAT lam m_nActive
	// tang (doan tang trong PlayerChangeRegion nam trong khoi chu thich /* */).
	const int nIdxSubWorld = (int)(this - SubWorld);
	int* pNgu = (nIdxSubWorld >= 0 && nIdxSubWorld < MAX_SUBWORLD)
		? &s_nRegionNgu[nIdxSubWorld] : NULL;
	if (pNgu && *pNgu > 0)
	{
		(*pNgu)--;	// ban do trong: bo qua vong quet khung nay
	}
	else
	{
		int nCoActive = 0;
		for (int i = 0; i < m_nTotalRegion; i++)
		{
			if (m_Region[i].IsActive())
			{
				s_nS13iVungDangDuyet = i;	// [S13i]
				m_Region[i].Activate();
				nActiveRegionCount++;
				nCoActive++;
			}
		}
		s_nS13iVungDangDuyet = -1;	// [S13i]
		if (pNgu && nCoActive == 0)
			*pNgu = SUBWORLD_KHUNG_NGU;
	}
#else
	for (int i = 0; i < m_nTotalRegion; i++)
	{
		if (m_Region[i].IsActive())
		{
//			g_DebugLog("[Region]%d Activating", i);
			m_Region[i].Activate();
			nActiveRegionCount++;
		}
	}
#endif

#ifdef _SERVER
	s_nS13iVungDangDuyet = 0x7FFFFFFF;	// [S13i] danh sach VOID: NPC hoi sinh ve vung (da duyet) -> khong dat co
	KIndexNode* pNode = (KIndexNode *)m_NoneRegionNpcList.GetHead();
	while(pNode)
	{
		Npc[pNode->m_nIndex].Activate();
		pNode = (KIndexNode *)pNode->GetNext();
	}
	s_nS13iVungDangDuyet = -1;	// [S13i]

	if(m_pWeatherMgr)
	{
		int nWeather = m_pWeatherMgr->Activate();
		if(m_nWeather != nWeather)
		{
			m_nWeather = nWeather;
			SYNC_WEATHER weatherMsg;
			weatherMsg.ProtocolType = s2c_changeweather;
			weatherMsg.WeatherID = (BYTE)m_nWeather;
			BroadCast((const char*)&weatherMsg, sizeof(SYNC_WEATHER));
		}
	}
	
		m_MissionArray.Activate();

#endif
#ifndef _SERVER
	if(m_nTargetX && m_nTargetY && m_vRetPath.size() > 0)
	{
		int nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		if(!nNpcIdx)
		{
			StopPath();
			return;
		}
		int nX, nY;
		Npc[nNpcIdx].GetMpsPos(&nX, &nY);
		int nStep = (int)m_vRetPath.size();
		if(m_nCurStep == 0)
		{
			++m_nCurStep;
			if (!Player[CLIENT_PLAYER_INDEX].m_RunStatus)
			{
				if(Npc[nNpcIdx].m_CurrentWalkSpeed <= 6)
				m_uStepDelayTime = timeGetTime() + 13000;
				else if(Npc[nNpcIdx].m_CurrentWalkSpeed <= 10)
				m_uStepDelayTime = timeGetTime() + 8000;
				else if(Npc[nNpcIdx].m_CurrentWalkSpeed <= 20)
				m_uStepDelayTime = timeGetTime() + 5000;
				else
				m_uStepDelayTime = timeGetTime() + 3500;
			}
			else
			{
				if(Npc[nNpcIdx].m_CurrentRunSpeed <= 6)
				m_uStepDelayTime = timeGetTime() + 13000;
				else if(Npc[nNpcIdx].m_CurrentRunSpeed <= 10)
				m_uStepDelayTime = timeGetTime() + 8000;
				else if(Npc[nNpcIdx].m_CurrentRunSpeed <= 20)
				m_uStepDelayTime = timeGetTime() + 5000;
				else
				m_uStepDelayTime = timeGetTime() + 3500;
			}
		}
		if(m_nCurStep >= nStep-1)
		{
			if(g_GetDistance(nX, nY, m_nTargetX, m_nTargetY) < 64)
				return;
			if(m_nCurStep > nStep-1)
			{
				if (!Player[CLIENT_PLAYER_INDEX].m_RunStatus)
				{
					Npc[nNpcIdx].SendCommand(do_walk, m_nTargetX, m_nTargetY);
					SendClientCmdWalk(m_nTargetX, m_nTargetY);
				}
				else
				{
					Npc[nNpcIdx].SendCommand(do_run, m_nTargetX, m_nTargetY);
					SendClientCmdRun(m_nTargetX, m_nTargetY);
				}
			}
			else
			{
				int nBlockid = m_vRetPath[m_nCurStep];
				if(m_GridNode[nBlockid].w < 4 || m_GridNode[nBlockid].h < 4)
				{
					++m_nCurStep;
					if (!Player[CLIENT_PLAYER_INDEX].m_RunStatus)
					{
						if(Npc[nNpcIdx].m_CurrentWalkSpeed <= 6)
						m_uStepDelayTime = timeGetTime() + 13000;
						else if(Npc[nNpcIdx].m_CurrentWalkSpeed <= 10)
						m_uStepDelayTime = timeGetTime() + 8000;
						else if(Npc[nNpcIdx].m_CurrentWalkSpeed <= 20)
						m_uStepDelayTime = timeGetTime() + 5000;
						else
						m_uStepDelayTime = timeGetTime() + 3500;
					}
					else
					{
						if(Npc[nNpcIdx].m_CurrentRunSpeed <= 6)
						m_uStepDelayTime = timeGetTime() + 13000;
						else if(Npc[nNpcIdx].m_CurrentRunSpeed <= 10)
						m_uStepDelayTime = timeGetTime() + 8000;
						else if(Npc[nNpcIdx].m_CurrentRunSpeed <= 20)
						m_uStepDelayTime = timeGetTime() + 5000;
						else
						m_uStepDelayTime = timeGetTime() + 3500;
					}
					if (!Player[CLIENT_PLAYER_INDEX].m_RunStatus)
					{
						Npc[nNpcIdx].SendCommand(do_walk, m_nTargetX, m_nTargetY);
						SendClientCmdWalk(m_nTargetX, m_nTargetY);
					}
					else
					{
						Npc[nNpcIdx].SendCommand(do_run, m_nTargetX, m_nTargetY);
						SendClientCmdRun(m_nTargetX, m_nTargetY);
					}
					return;
				}
				int bX = m_GridNode[nBlockid].x*32;
				int bY = m_GridNode[nBlockid].y*32;
				int eX = m_GridNode[nBlockid].x*32 + m_GridNode[nBlockid].w*32;
				int eY = m_GridNode[nBlockid].y*32 + m_GridNode[nBlockid].h*32;
				if(nX >= bX && nX <= eX && nY >= bY && nY <= eY)
				{
					++m_nCurStep;
					if (!Player[CLIENT_PLAYER_INDEX].m_RunStatus)
					{
						if(Npc[nNpcIdx].m_CurrentWalkSpeed <= 6)
						m_uStepDelayTime = timeGetTime() + 13000;
						else if(Npc[nNpcIdx].m_CurrentWalkSpeed <= 10)
						m_uStepDelayTime = timeGetTime() + 8000;
						else if(Npc[nNpcIdx].m_CurrentWalkSpeed <= 20)
						m_uStepDelayTime = timeGetTime() + 5000;
						else
						m_uStepDelayTime = timeGetTime() + 3500;
					}
					else
					{
						if(Npc[nNpcIdx].m_CurrentRunSpeed <= 6)
						m_uStepDelayTime = timeGetTime() + 13000;
						else if(Npc[nNpcIdx].m_CurrentRunSpeed <= 10)
						m_uStepDelayTime = timeGetTime() + 8000;
						else if(Npc[nNpcIdx].m_CurrentRunSpeed <= 20)
						m_uStepDelayTime = timeGetTime() + 5000;
						else
						m_uStepDelayTime = timeGetTime() + 3500;
					}
				}
				int dX = m_GridNode[nBlockid].x*32 + m_GridNode[nBlockid].w*32/2;
				int dY = m_GridNode[nBlockid].y*32 + m_GridNode[nBlockid].h*32/2;
				if (!Player[CLIENT_PLAYER_INDEX].m_RunStatus)
				{
					Npc[nNpcIdx].SendCommand(do_walk, dX, dY);
					SendClientCmdWalk(dX, dY);
				}
				else
				{
					Npc[nNpcIdx].SendCommand(do_run, dX, dY);
					SendClientCmdRun(dX, dY);
				}
			}
		}
		else
		{
			int nBlockid = m_vRetPath[m_nCurStep];
			int dX = m_GridNode[nBlockid].x*32 + m_GridNode[nBlockid].w*32/2;
			int dY = m_GridNode[nBlockid].y*32 + m_GridNode[nBlockid].h*32/2;
			if(g_GetDistance(nX, nY, dX, dY) < 64)
			{
				++m_nCurStep;
				if (!Player[CLIENT_PLAYER_INDEX].m_RunStatus)
				{
					if(Npc[nNpcIdx].m_CurrentWalkSpeed <= 6)
					m_uStepDelayTime = timeGetTime() + 13000;
					else if(Npc[nNpcIdx].m_CurrentWalkSpeed <= 10)
					m_uStepDelayTime = timeGetTime() + 8000;
					else if(Npc[nNpcIdx].m_CurrentWalkSpeed <= 20)
					m_uStepDelayTime = timeGetTime() + 5000;
					else
					m_uStepDelayTime = timeGetTime() + 3500;
				}
				else
				{
					if(Npc[nNpcIdx].m_CurrentRunSpeed <= 6)
					m_uStepDelayTime = timeGetTime() + 13000;
					else if(Npc[nNpcIdx].m_CurrentRunSpeed <= 10)
					m_uStepDelayTime = timeGetTime() + 8000;
					else if(Npc[nNpcIdx].m_CurrentRunSpeed <= 20)
					m_uStepDelayTime = timeGetTime() + 5000;
					else
					m_uStepDelayTime = timeGetTime() + 3500;
				}
				if(m_nCurStep < nStep)
				{
					nBlockid = m_vRetPath[m_nCurStep];
					dX = m_GridNode[nBlockid].x*32 + m_GridNode[nBlockid].w*32/2;
					dY = m_GridNode[nBlockid].y*32 + m_GridNode[nBlockid].h*32/2;
				}
				else
				{
					dX = m_nTargetX;
					dY = m_nTargetY;
				}
			}
			if (!Player[CLIENT_PLAYER_INDEX].m_RunStatus)
			{
				Npc[nNpcIdx].SendCommand(do_walk, dX, dY);
				SendClientCmdWalk(dX, dY);
			}
			else
			{
				Npc[nNpcIdx].SendCommand(do_run, dX, dY);
				SendClientCmdRun(dX, dY);
			}
		}
		if(m_uStepDelayTime < timeGetTime())
		{
		    static DWORD s_dwLastRePath = 0;
		    if (timeGetTime() - s_dwLastRePath > 1000)
		    {
		        s_dwLastRePath = timeGetTime();
		        m_nCurStep = 0;
    			m_uStepDelayTime = timeGetTime();
		        if (g_GetDistance(nX, nY, m_nTargetX, m_nTargetY) < 512)
				    FindPath(m_nTargetX, m_nTargetY, true);
				else
				    FindPath(m_nTargetX, m_nTargetY, false);
		    }
		}
	}
#endif
}

int KSubWorld::GetDistance(int nRx1, int nRy1, int nRx2, int nRy2)
{
	return (int)sqrt((nRx1 - nRx2) * (nRx1 - nRx2) + (nRy1 - nRy2) * (nRy1 - nRy2));
}
void KSubWorld::NewMap2Mps(int nR, int nX, int nY, int nDx, int nDy, int* nRx, int* nRy)
{
	/*
	#ifdef TOOLVERION
		*nRx = nX;
		*nRy = nY;
		return;
	#endif

	#ifndef _SERVER
	//	_ASSERT(nR >= 0 && nR < 9);
	#endif
		//_ASSERT(nR >= 0);
	*/
	if (nR < 0 || nR >= m_nTotalRegion)
	{
		*nRx = 0;
		*nRy = 0;
		return;
	}

	int x, y;

	x = m_Region[nR].m_nRegionX;
	y = m_Region[nR].m_nRegionY;

	x += nX * m_nCellWidth;
	y += nY * m_nCellHeight;

	x += (nDx >> 10);
	y += (nDy >> 10);

	*nRx = x;
	*nRy = y;

}
void KSubWorld::Map2Mps(int nR, int nX, int nY, int nDx, int nDy, int *nRx, int *nRy)
{
/*
#ifdef TOOLVERION
	*nRx = nX;
	*nRy = nY;
	return;
#endif
*/
#ifndef _SERVER
//	_ASSERT(nR >= 0 && nR < 9);
#endif
	//_ASSERT(nR >= 0);

	if (nR < 0 || nR >= m_nTotalRegion)
	{
		*nRx = 0;
		*nRy = 0;
		return;
	}

	int x, y;
	
	x = m_Region[nR].m_nRegionX;
	y = m_Region[nR].m_nRegionY;
	
	x += nX * m_nCellWidth;
	y += nY * m_nCellHeight;
	
	x += (nDx >> 10);
	y += (nDy >> 10);
	
	*nRx = x;
	*nRy = y;
}

void KSubWorld::Map2Mps(int nRx, int nRy, int nX, int nY, int nDx, int nDy, int *pnX, int *pnY)
{
	*pnX = (nRx * REGION_GRID_WIDTH + nX) * defLOGIC_CELL_WIDTH + (nDx >> 10);
	*pnY = (nRy * REGION_GRID_HEIGHT + nY) * defLOGIC_CELL_HEIGHT + (nDy >> 10);
}

void KSubWorld::Mps2Map(int Rx, int Ry, int * nR, int * nX, int * nY, int *nDx, int * nDy)
{
	if (m_nCellWidth == 0 || m_nCellHeight == 0 || m_nRegionWidth == 0 || m_nRegionHeight == 0)
		return;
	int x = Rx / (m_nRegionWidth * m_nCellWidth);
	int	y = Ry / (m_nRegionHeight * m_nCellHeight);

	*nX = 0;
	*nY = 0;
	*nDx = 0;
	*nDy = 0;
#ifdef _SERVER
	// 
	if (x >= m_nWorldRegionWidth + m_nRegionBeginX || y >= m_nWorldRegionHeight + m_nRegionBeginY || x < m_nRegionBeginX || y < m_nRegionBeginY)
	{
		*nR = -1;
		return;
	}
#endif
	// 
#ifdef _SERVER
	*nR = GetRegionIndex(MAKELONG(x, y));
#else
	int nRegionID = MAKELONG(x, y);
	*nR = FindRegion(nRegionID);
	if (*nR == -1)
		return;
#endif
	if (*nR >= m_nTotalRegion)
	{
		*nR = -1;
		return;
	}

	x = Rx - m_Region[*nR].m_nRegionX;
	y = Ry - m_Region[*nR].m_nRegionY;

	*nX = x / m_nCellWidth;
	*nY = y / m_nCellHeight;

	*nDx = (x - *nX * m_nCellWidth) << 10;
	*nDy = (y - *nY * m_nCellHeight) << 10;
}

BYTE	KSubWorld::TestBarrier(int nMpsX, int nMpsY)
{
	if (m_nCellWidth == 0 || m_nCellHeight == 0 || m_nRegionWidth == 0 || m_nRegionHeight == 0)
		return 0xff;

	int x = nMpsX / (m_nRegionWidth * m_nCellWidth);
	int	y = nMpsY / (m_nRegionHeight * m_nCellHeight);
#ifdef _SERVER
	// 
	if (x >= m_nWorldRegionWidth + m_nRegionBeginX || y >= m_nWorldRegionHeight + m_nRegionBeginY || x < m_nRegionBeginX || y < m_nRegionBeginY)
		return 0xff;
	int nRegion = GetRegionIndex(MAKELONG(x, y));
#else
	int nRegion = FindRegion(MAKELONG(x, y));
	if (nRegion == -1)
		return 0xff;
#endif
	if (nRegion >= m_nTotalRegion)
		return 0xff;

	x = nMpsX - m_Region[nRegion].m_nRegionX;
	y = nMpsY - m_Region[nRegion].m_nRegionY;

	int nCellX = x / m_nCellWidth;
	int nCellY = y / m_nCellHeight;

	int nOffX = x - nCellX * m_nCellWidth;
	int nOffY = y - nCellY * m_nCellHeight;

#ifndef _SERVER
	BYTE bRet = (BYTE)g_ScenePlace.GetObstacleInfo(nMpsX, nMpsY);
	if (bRet != Obstacle_NULL)
		return bRet;
	return m_Region[nRegion].GetBarrier(nCellX, nCellY, nOffX, nOffY);
#endif
#ifdef _SERVER
	return m_Region[nRegion].GetBarrier(nCellX, nCellY, nOffX, nOffY);
#endif
}

BYTE KSubWorld::TestBarrier(int nRegion, int nMapX, int nMapY, int nDx, int nDy, int nChangeX, int nChangeY)
{
	int nOldMapX = nMapX;
	int nOldMapY = nMapY;
	int nOldRegion = nRegion;

	nDx += nChangeX;
	nDy += nChangeY;

	if (nDx < 0)
	{
		nDx += (m_nCellWidth << 10);
		nMapX--;
	}
	else if (nDx >= (m_nCellWidth << 10))
	{
		nDx -= (m_nCellWidth << 10);
		nMapX++;
	}
	
	if (nDy < 0)
	{
		nDy += (m_nCellHeight << 10);
		nMapY--;
	}
	else if (nDy >= (m_nCellHeight << 10))
	{
		nDy -= (m_nCellHeight << 10);
		nMapY++;
	}
	
	if (nMapX < 0)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_LEFT] == -1)
			return 0xff;

		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_LEFT];
		nMapX += m_nRegionWidth;
	}
	else if (nMapX >= m_nRegionWidth)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_RIGHT] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_RIGHT];
		nMapX -= m_nRegionWidth;
	}
	
	if (nMapY < 0)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_UP] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_UP];;
		nMapY += m_nRegionHeight;
	}
	else if (nMapY >= m_nRegionHeight)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_DOWN] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_DOWN];
		nMapY -= m_nRegionHeight;
	}

	int nXf, nYf;
	nXf = (nDx >> 10);
	nYf = (nDy >> 10);

#ifdef TOOLVERSION
	return Obstacle_NULL;
#endif
#ifndef _SERVER

	int nMpsX, nMpsY;
	Map2Mps(nRegion, nMapX, nMapY, nDx, nDy, &nMpsX, &nMpsY);
	BYTE bRet = (BYTE)g_ScenePlace.GetObstacleInfo(nMpsX, nMpsY);
	if (bRet != Obstacle_NULL)
		return bRet;
//	if (nMapX == nOldMapX && nMapY == nOldMapY && nRegion == nOldRegion)
//		return Obstacle_NULL;

	return m_Region[nRegion].GetBarrier(nMapX, nMapY, nXf, nYf);
#else
//	if (nMapX == nOldMapX && nMapY == nOldMapY && nRegion == nOldRegion)
//		return 0;
	return m_Region[nRegion].GetBarrier(nMapX, nMapY, nXf, nYf);
#endif
}

BYTE KSubWorld::TestBarrierMin(int nRegion, int nMapX, int nMapY, int nDx, int nDy, int nChangeX, int nChangeY)
{
	int nOldMapX = nMapX;
	int nOldMapY = nMapY;
	int nOldRegion = nRegion;

	nDx += nChangeX;
	nDy += nChangeY;

	if (nDx < 0)
	{
		nDx += (m_nCellWidth << 10);
		nMapX--;
	}
	else if (nDx >= (m_nCellWidth << 10))
	{
		nDx -= (m_nCellWidth << 10);
		nMapX++;
	}
	
	if (nDy < 0)
	{
		nDy += (m_nCellHeight << 10);
		nMapY--;
	}
	else if (nDy >= (m_nCellHeight << 10))
	{
		nDy -= (m_nCellHeight << 10);
		nMapY++;
	}
	
	if (nMapX < 0)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_LEFT] == -1)
			return 0xff;

		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_LEFT];
		nMapX += m_nRegionWidth;
	}
	else if (nMapX >= m_nRegionWidth)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_RIGHT] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_RIGHT];
		nMapX -= m_nRegionWidth;
	}
	
	if (nMapY < 0)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_UP] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_UP];;
		nMapY += m_nRegionHeight;
	}
	else if (nMapY >= m_nRegionHeight)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_DOWN] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_DOWN];
		nMapY -= m_nRegionHeight;
	}

#ifndef _SERVER

	int nMpsX, nMpsY;
	Map2Mps(nRegion, nMapX, nMapY, nDx, nDy, &nMpsX, &nMpsY);
	BYTE bRet = (BYTE)g_ScenePlace.GetObstacleInfoMin(nMpsX, nMpsY, nDx & 0x000003ff, nDy & 0x000003ff);
	if (bRet != Obstacle_NULL)
		return bRet;

	if (nMapX == nOldMapX && nMapY == nOldMapY && nRegion == nOldRegion)
		return Obstacle_NULL;
	return m_Region[nRegion].GetBarrierMin(nMapX, nMapY, nDx, nDy, TRUE);

#else

	if (nMapX == nOldMapX && nMapY == nOldMapY && nRegion == nOldRegion)
	{
		return m_Region[nRegion].GetBarrierMin(nMapX, nMapY, nDx, nDy, FALSE);
	}

	return m_Region[nRegion].GetBarrierMin(nMapX, nMapY, nDx, nDy, TRUE);
#endif
}

DWORD KSubWorld::GetTrap(int nMpsX, int nMpsY)
{
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion == -1)
		return 0;
	return m_Region[nRegion].GetTrap(nMapX, nMapY);
}

BYTE KSubWorld::GetBarrier(int nMpsX, int nMpsY)
{
#ifdef _SERVER
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	
	if (nRegion == -1)
		return 0xff;
	return m_Region[nRegion].GetBarrier(nMapX, nMapY, (nOffX >> 10), (nOffY) >> 10);
#else
	return (BYTE)g_ScenePlace.GetObstacleInfo(nMpsX, nMpsY);
#endif
}

#ifdef _SERVER

// Ban do NAO duoc nap luoi A* phia server.
//
// Vi sao phai loc: MAX_SUBWORLD = 1000 ma moi luoi ton ~1,5 MB, nap het la vut di hang GB
// cho nhung noi khong bot nao dat chan toi. Bang duoi bung tu cay tham khao
// (USVOLAM KSubWorld.cpp:2356-2361) - la cac thi tran + bai luyen cap bot hay lui toi.
// Map 53 (Ba Lang Huyen) la noi bot sinh ra VA la noi dung ca 10 NPC mon phai.
//
// Them ban do moi chi can them so vao day, khong phai dong toi cho nao khac.
static bool IsBotPathMap(int nMapId)
{
	static const int s_map[] = {
		1,2,4,7,8,9,10,11,12,14,21,26,37,38,41,42,50,53,56,69,70,71,75,78,80,83,90,92,93,94,
		120,122,123,125,140,162,163,164,167,176,203,205,206,207,224,225,226,227,
		319,320,321,322,336,340,341,380,829,
		// bai luyen cap theo Than Hanh Phu con thieu o danh sach goc:
		19,79,124,144,152,166,170,181,182,193,198,875,
		969,970,971,972,973,974,975,976,
		// (19/08 toi #4) 5 thon/tran cua che do VE THANH (pb_VeThanh) - khong co
		// luoi thi PB_WalkTo di dao tra -1, bot ve thon dung im nhu tuong
		99,100,101,153,174,
		// (phan bien dot 2) 20 + 121 la 2/15 map dich VE THANH *va* 2/10 thanh nha
		// Da Tau (s_dtNpc) ma xua nay KHONG co luoi -> DTB_TOI_NPC di bo toi NPC
		// tren 2 thanh nay von da -2, bot thanh nha 20/121 xoay vong nghi 5 phut
		20,121,
		// (21/08 - bot tu tham gia Tong Kim) 324 = map bao danh, 379 = chien
		// truong. Thieu hai so nay thi PB_WalkTo tra -2 va bot dung im: khong
		// di toi duoc NPC bao danh, khong chay sang doanh trai dich duoc.
		// 379 dung CHUNG y het thu muc du lieu ban do voi 380 (MapList.ini,
		// byte trung tuyet doi) nen luoi chac chan dung duoc.
		324,379
	};
	for (int i = 0; i < (int)(sizeof(s_map) / sizeof(s_map[0])); i++)
		if (s_map[i] == nMapId)
			return true;
	return false;
}

BOOL KSubWorld::LoadMap(int nId)
{
	KIniFile	IniFile;
	char		szPathName[FILE_NAME_LENGTH];
	char		szFileName[FILE_NAME_LENGTH];
	char		szKeyName[32];

	m_SubWorldID = nId;
	g_SetFilePath("\\settings");
	IniFile.Load("MapList.ini");
	sprintf(szKeyName, "%d", nId);
	IniFile.GetString("List", szKeyName, "", szPathName, sizeof(szPathName));
	sprintf(szKeyName, "%d_name", nId);
	IniFile.GetString("List", szKeyName, "", szMapName, sizeof(szMapName));
	sprintf(szKeyName, "%d_MapType", nId);
	IniFile.GetString("List", szKeyName, "", szMapType, sizeof(szMapType));
	sprintf(szKeyName, "%d_GoldenDropRate", nId);
	IniFile.GetString("List", szKeyName, "", szGoldenDropRate, sizeof(szGoldenDropRate));
	sprintf(szKeyName, "%d_NormalDropRate", nId);
	IniFile.GetString("List", szKeyName, "", szNormalDropRate, sizeof(szNormalDropRate));
	sprintf(szKeyName, "%d_AutoGoldenNpc", nId);
	IniFile.GetInteger("List", szKeyName, 0, &nzAutoGoldenNpc);
	sprintf(szKeyName, "%d_GoldenType", nId);
	IniFile.GetInteger("List", szKeyName, 0, &nzGoldenType);

	sprintf(szKeyName, "%d_NpcSeriesAuto", nId);
	IniFile.GetInteger("List", szKeyName, 0, &m_NpcSeriesAuto);
	sprintf(szKeyName, "%d_NpcSeriesMetal", nId);
	IniFile.GetInteger("List", szKeyName, 0, &m_NpcSeriesMetal);
	sprintf(szKeyName, "%d_NpcSeriesWood", nId);
	IniFile.GetInteger("List", szKeyName, 0, &m_NpcSeriesWood);
	sprintf(szKeyName, "%d_NpcSeriesWater", nId);
	IniFile.GetInteger("List", szKeyName, 0, &m_NpcSeriesWater);
	sprintf(szKeyName, "%d_NpcSeriesFire", nId);
	IniFile.GetInteger("List", szKeyName, 0, &m_NpcSeriesFire);
	sprintf(szKeyName, "%d_NpcSeriesEarth", nId);
	IniFile.GetInteger("List", szKeyName, 0, &m_NpcSeriesEarth);

	// [WLLS 20/08] khoi phuc 2 khoa NewWorldScript/NewWorldParam (xem KSubWorld.h)
	m_szNewWorldScript[0] = 0;
	m_szNewWorldParam[0] = 0;
	sprintf(szKeyName, "%d_NewWorldScript", nId);
	IniFile.GetString("List", szKeyName, "", m_szNewWorldScript, sizeof(m_szNewWorldScript));
	sprintf(szKeyName, "%d_NewWorldParam", nId);
	IniFile.GetString("List", szKeyName, "", m_szNewWorldParam, sizeof(m_szNewWorldParam));

	// [PORT5 23/08] <id>_MapInfo (tongwar GetMapInfoFile); rut '\\' kep ve '\' don cho
	// KIniFile/TabFile phia Windows khoi phai doan (Linux fopen tu chiu '//')
	m_szMapInfoFile[0] = 0;
	sprintf(szKeyName, "%d_MapInfo", nId);
	IniFile.GetString("List", szKeyName, "", m_szMapInfoFile, sizeof(m_szMapInfoFile));
	{
		char* pR = m_szMapInfoFile;
		char* pW = m_szMapInfoFile;
		while (*pR)
		{
			*pW++ = *pR;
			if (pR[0] == '\\' && pR[1] == '\\')
				pR += 2;
			else
				pR++;
		}
		*pW = 0;
	}

	g_SetFilePath("\\maps");
	sprintf(szFileName, "%s.wor", szPathName);//edit by phong kieu load file wor
	if (!IniFile.Load(szFileName))
	{
		printf("===[szFileName is not exists: %s]",szFileName);
		return FALSE;
	}
	
	int nIsInDoor;
	IniFile.GetInteger("MAIN", "IsInDoor", 0, &nIsInDoor);
	if(!nIsInDoor)
	{
		m_pWeatherMgr = new KWeatherMgr;
		m_pWeatherMgr->InitFromIni(IniFile);
	}

	m_nRegionWidth = 512 / 32;
	m_nRegionHeight = 1024 / 32;

	RECT	sRect;
	IniFile.GetRect("MAIN", "rect", &sRect);
	m_nRegionBeginX = sRect.left;
	m_nRegionBeginY = sRect.top;
	m_nWorldRegionWidth = sRect.right - sRect.left + 1;
	m_nWorldRegionHeight = sRect.bottom - sRect.top + 1;
	m_nTotalRegion = m_nWorldRegionWidth * m_nWorldRegionHeight;
	
	m_nCellWidth = defLOGIC_CELL_WIDTH;
	m_nCellHeight = defLOGIC_CELL_HEIGHT;

	if (m_nTotalRegion <= 0 || m_nWorldRegionWidth <= 0 || m_nWorldRegionHeight <= 0)
	{
		printf("===[m_nTotalRegion <= 0 || m_nWorldRegionWidth <= 0 || m_nWorldRegionHeight <= 0]===");
		return FALSE;
	}

	m_Region = new KRegion[m_nTotalRegion];
	char	szPath[MAX_PATH];
	sprintf(szPath, "\\maps\\%s", szPathName);
	int		nX, nY, nIdx;
	// Region
	for (nY = 0; nY < m_nWorldRegionHeight; nY++)
	{
		for (nX = 0; nX < m_nWorldRegionWidth; nX++)
		{
			char	szRegionPath[128];
			g_SetFilePath(szPath);
			nIdx = nY * m_nWorldRegionWidth + nX;
			if (m_Region[nIdx].Load(nX + m_nRegionBeginX, nY + m_nRegionBeginY))
			{
				m_Region[nIdx].Init(m_nRegionWidth, m_nRegionHeight);
				m_Region[nIdx].m_nIndex = nIdx;
			}
			for (int i = 0; i < 8; i++)
			{
				short nTmpX, nTmpY;
				nTmpX = (short)LOWORD(m_Region[nIdx].m_nConRegionID[i]) - m_nRegionBeginX;
				nTmpY = (short)HIWORD(m_Region[nIdx].m_nConRegionID[i]) - m_nRegionBeginY;
				if (nTmpX < 0 || nTmpY < 0 || nTmpX >= m_nWorldRegionWidth || nTmpY >= m_nWorldRegionHeight)
				{
					m_Region[nIdx].m_nConnectRegion[i] = -1;
					continue;
				}
				int nConIdx = nTmpY * m_nWorldRegionWidth + nTmpX;
				m_Region[nIdx].m_nConnectRegion[i] = nConIdx;
			}
		}
	}

	sprintf(szPath, "\\maps\\%s", szPathName);
	for (nY = 0; nY < m_nWorldRegionHeight; nY++)
	{
		for (nX = 0; nX < m_nWorldRegionWidth; nX++)
		{
			g_SetFilePath(szPath);
			m_Region[nY * m_nWorldRegionWidth + nX].LoadObject(m_nIndex,nX + m_nRegionBeginX, nY + m_nRegionBeginY);
		}
	}

	// ---- Nap luoi A* cho ban do bot can di lai ----
	// Du lieu vat can lay tu chinh cay \maps\<ten>\v_NNN ma vong LoadObject vua doc xong,
	// nen goi o day la dung cho: moi thu can thiet da san sang.
	// Lan dau tinh xong se ghi cache \maps\<id>_srv.fp, boot sau nap thang tu cache.
	if (!m_bHavePath && IsBotPathMap(m_SubWorldID))
	{
		LoadPathGridSrv(szPath, m_nWorldRegionWidth, m_nWorldRegionHeight);
		printf("[BotA*] luoi map=%d gridW=%d gridH=%d o=%d cothanhcong=%d\n",
			   m_SubWorldID, m_nWorldRegionWidth, m_nWorldRegionHeight,
			   m_nGridTotal, (int)m_bHavePath);
	}

	return TRUE;
}
#endif

#ifndef _SERVER
BOOL KSubWorld::LoadMap(int nId, int nRegion)
{
	//g_DebugLog("LoadMap begin");
	static int	nXOff[8] = {0, -1, -1, -1, 0,  1, 1, 1};
	static int	nYOff[8] = {1, 1,  0,  -1, -1, -1, 0, 1};
	//KIniFile	IniFile;

	if (!m_Region)
	{
		m_Region = new KRegion[MAX_REGION];
	}
	bool bLoadNew = false;
	if (nId != m_SubWorldID)
	{
		StopPath();
		S13_ClearCmd(Player[CLIENT_PLAYER_INDEX].m_nIndex);	// [S13] doi map: lenh dang giu la cua map cu
		m_uPaintTime = timeGetTime() + 2000;
		bLoadNew = true;
		SubWorld[0].Close();
		g_ScenePlace.ClosePlace();
		// NpcSet.RemoveAll(Player[CLIENT_PLAYER_INDEX].m_nIndex); -- later finish it. spe
#ifndef _SERVER
		// [FIX-A 26/08] Hoan thien dung cho bo do o tren ("later finish it"): DOI MAP THAT
		// thi tra het khe NPC map cu - tru CHINH MINH va BAN DONG HANH (kind_partner).
		// SubWorld[0].Close() vua go MOI NPC khoi region (RegionIndex=-1) nen Remove o day
		// la an toan. Truoc day toan bo NPC map cu chiem khe toi 55 s dung luc map moi can
		// khe nhat: vao Tong Kim 10-24 s la bang day 255/256 (mo coi 254), 1.198 lan NPC
		// moi KHONG them duoc => dich vo hinh. Nguoi choi/bot map cu van nguyen tren server;
		// gap lai thi client REQNPC + sync full nhu moi (co che san, chay nghin lan/gio).
		{
			extern int S6_UsedSlots();
			int nS6Me = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			int nS6Xoa = 0;
			for (int i6 = 1; i6 < MAX_NPC; i6++)
			{
				if (Npc[i6].m_dwID == 0 || i6 == nS6Me)
					continue;
				if (Npc[i6].m_Kind == kind_partner)
					continue;
				NpcSet.Remove(i6);
				nS6Xoa++;
			}
			AUTOLOG("[S6-DONMAP] xoa=%d conlai=%d/%d t=%u", nS6Xoa, S6_UsedSlots(), (int)MAX_NPC, timeGetTime());
		}
#endif
/*
		char	szKeyName[32], szPathName[FILE_NAME_LENGTH];
		BOOL	m_bBgPic;

		g_SetFilePath("\\settings");
		IniFile.Load("MapList.ini");
		sprintf(szKeyName, "%d", nId);//edit by phong kieu load id maps
		IniFile.GetString("List", szKeyName, "", szPathName, sizeof(szPathName));

		sprintf(m_szMapPath, "\\maps\\%s", szPathName);//edit by phong kieu load path name maps trong folder maps
		g_ScenePlace.OpenPlace(nId);

		sprintf(szKeyName, "%d_Bg", nId);
		IniFile.GetInteger("List", szKeyName, 0, &m_bBgPic);
		g_DebugLog("bgpic %d", m_bBgPic);
		if (m_bBgPic)
		{
			sprintf(szPathName, "\\settings\\maps\\bg\\%d.ini", nId);
			IniFile.Load(szPathName);
			g_ScenePlace.LoadGround(&IniFile); //add by phong kiÒu h×nh nÒn hoa s¬n
		}
*/
		g_ScenePlace.OpenPlace(nId);
		m_SubWorldID = nId;
		g_ScenePlace.LoadSymbol(nId);//#maptraffic
		m_nRegionWidth = KScenePlaceRegionC::RWPP_AREGION_WIDTH / 32;
		m_nRegionHeight = KScenePlaceRegionC::RWPP_AREGION_HEIGHT / 32;
		m_nCellWidth = 32;
		m_nCellHeight = 32;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RegionIndex = -1;
		//pathgrid
		m_nGridTotal = 0;
		KIniFile	IniFile;
		char		szFileName[FILE_NAME_LENGTH];
		char		szKeyName[FILE_NAME_LENGTH];
		
		g_SetFilePath("\\settings");
		IniFile.Load("MapList.ini");
		sprintf(szKeyName, "%d", nId);
		IniFile.GetString("List", szKeyName, "", m_szPathName, sizeof(m_szPathName));
				
		g_SetFilePath("\\maps");
		sprintf(szFileName, "%s.wor", m_szPathName);
		if (IniFile.Load(szFileName))
		{
			RECT	sRect;
			IniFile.GetRect("MAIN", "rect", &sRect);
			m_nRegionBeginX = sRect.left;
			m_nRegionBeginY = sRect.top;
			m_nGridW = sRect.right - sRect.left + 1;
			m_nGridH = sRect.bottom - sRect.top + 1;
			//g_DebugLog("mapinf [%d][%d][%d][%d]", m_nRegionBeginX,m_nRegionBeginY,m_nGridW,m_nGridH);
			m_nGridTotal = m_nGridW * m_nGridH;
		}
	}
	int nX = LOWORD(nRegion);
	int nY = HIWORD(nRegion);

	int nIdx = FindRegion(nRegion);
#ifndef _SERVER
	// [S6 26/08] Moi cu cuon/nap vung phia client. daco=0 = region tam chua nap (se tai
	// dung slot xa -> KRegion::Close -> mot loat NPC MO COI). loadnew=1 = doi han map.
	AUTOLOG("[S6-LOADMAP] sw=%d tam=(%d,%d) daco=%d loadnew=%d t=%u", nId, nX, nY, (int)(nIdx >= 0), (int)(bLoadNew ? 1 : 0), timeGetTime());
#endif

	if (nIdx < 0)
	{
		nIdx = m_ClientRegionIdx[0];

		if (m_Region[nIdx].Load(nX, nY))
		{
			m_Region[nIdx].m_nIndex = nIdx;
			m_Region[nIdx].Init(m_nRegionWidth, m_nRegionHeight);
			//m_Region[nIdx].LoadObject(0, nX, nY, m_szMapPath);
		}
	}
	
	// While PaintFps interpolation drives the camera, a plain region change inside the
	// same map must NOT touch the focus (it points at the region corner and would ping-pong
	// with the interpolated focus, tearing the draw tree down 3x per border). Opening a
	// new map still needs it: OpenPlace has just reset the whole scene.
	if (bLoadNew || !g_bPaintInterpFocus)
		g_ScenePlace.SetFocusPosition(m_Region[nIdx].m_nRegionX, m_Region[nIdx].m_nRegionY, 0);
	m_ClientRegionIdx[0] = nIdx;

	for (int i = 0; i < 8; i++)
	{
		int nConIdx;
		nConIdx = FindRegion(m_Region[nIdx].m_nConRegionID[i]);

		if (nConIdx < 0)
		{
			nConIdx = FindFreeRegion(nX, nY);
			_ASSERT(nConIdx >= 0);

			if (m_Region[nConIdx].Load(nX + nXOff[i], nY + nYOff[i]))
			{
				m_Region[nConIdx].m_nIndex = nConIdx;
				m_Region[nConIdx].Init(m_nRegionWidth, m_nRegionHeight);
				//m_Region[nConIdx].LoadObject(0, nX + nXOff[i], nY + nYOff[i], m_szMapPath);
			}
			else
			{
				m_Region[nConIdx].m_nIndex = -1;
				m_Region[nConIdx].m_RegionID = -1;
				nConIdx = -1;
			}
		}
		m_ClientRegionIdx[i + 1] = nConIdx;
		m_Region[nIdx].m_nConnectRegion[i] = nConIdx;
	}

	if (m_Region[nIdx].m_nConnectRegion[0] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[0] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[1] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[7] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[4] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[2] = m_Region[nIdx].m_nConnectRegion[1];
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[3] = m_Region[nIdx].m_nConnectRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[5] = m_Region[nIdx].m_nConnectRegion[6];
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[6] = m_Region[nIdx].m_nConnectRegion[7];
	}

	if (m_Region[nIdx].m_nConnectRegion[1] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[0] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[1] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[7] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[4] = m_Region[nIdx].m_nConnectRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[2] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[3] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[5] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[6] = m_Region[nIdx].m_nConnectRegion[0];
	}

	if (m_Region[nIdx].m_nConnectRegion[2] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[0] = m_Region[nIdx].m_nConnectRegion[1];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[1] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[7] = m_Region[nIdx].m_nConnectRegion[0];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[4] = m_Region[nIdx].m_nConnectRegion[3];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[2] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[3] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[5] = m_Region[nIdx].m_nConnectRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[6] = nIdx;
	}

	if (m_Region[nIdx].m_nConnectRegion[3] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[0] = m_Region[nIdx].m_nConnectRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[1] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[2] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[3] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[4] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[5] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[6] = m_Region[nIdx].m_nConnectRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[7] = nIdx;
	}

	if (m_Region[nIdx].m_nConnectRegion[4] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[0] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[1] = m_Region[nIdx].m_nConnectRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[2] = m_Region[nIdx].m_nConnectRegion[3];
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[3] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[4] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[5] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[6] = m_Region[nIdx].m_nConnectRegion[5];
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[7] = m_Region[nIdx].m_nConnectRegion[6];
	}

	if (m_Region[nIdx].m_nConnectRegion[5] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[0] = m_Region[nIdx].m_nConnectRegion[6];
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[1] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[2] = m_Region[nIdx].m_nConnectRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[3] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[4] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[5] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[6] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[7] = -1;
	}

	if (m_Region[nIdx].m_nConnectRegion[6] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[0] = m_Region[nIdx].m_nConnectRegion[7];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[1] = m_Region[nIdx].m_nConnectRegion[0];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[2] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[3] = m_Region[nIdx].m_nConnectRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[4] = m_Region[nIdx].m_nConnectRegion[5];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[5] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[6] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[7] = -1;
	}
	
	if (m_Region[nIdx].m_nConnectRegion[7] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[0] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[1] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[2] = m_Region[nIdx].m_nConnectRegion[0];
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[3] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[4] = m_Region[nIdx].m_nConnectRegion[6];
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[5] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[6] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[7] = -1;
	}
	if(bLoadNew && m_nGridTotal)
	{
		bool bLoadData = false;
		char szFile[MAX_PATH];
		sprintf(szFile, "%d.fp", m_SubWorldID);
		KFile File;
		if(File.Open(szFile)) //co san~
		{
			//g_DebugLog("Filesan 1");
			UINT uVersion;
			File.Read(&uVersion, sizeof(UINT));
			if(uVersion == FINDPATH_VERSION)
			{
				File.Read(&uVersion, sizeof(UINT));
				File.Read(m_GridNode, uVersion);
				File.Read(&uVersion, sizeof(UINT));
				if (uVersion > 0)
				{
					m_vNeighbour.resize(uVersion);
					File.Read(&m_vNeighbour[0], sizeof(VGridNeighbour)*uVersion);
					m_bHavePath = TRUE;
				}
				bLoadData = true;
			}
			//g_DebugLog("Filesan 2");
		}
		if(!bLoadData)	//tao moi
		{
			//g_DebugLog("Taomoi 1");
			//m_hLoadPathGrid = CreateThread(NULL, 0,
			//	LoadPathGrid, this, 0, NULL);
			//WaitForSingleObject(m_hLoadPathGrid, INFINITE);
			ProcLoadPathGrid();
			g_CreatePath("\\maps");
			g_SetFilePath("\\maps");
			sprintf(szFile, "%d.fp", m_SubWorldID);
			int nAllCell  = m_nGridW*m_nRegionWidth * m_nGridH*m_nRegionHeight;
			KFile WFile;
			WFile.Create(szFile);
			UINT uVersion = FINDPATH_VERSION;
			WFile.Write(&uVersion, sizeof(UINT));
			uVersion = sizeof(VGridNode)*nAllCell;
			WFile.Write(&uVersion, sizeof(UINT));
			WFile.Write(m_GridNode, sizeof(VGridNode)*nAllCell);
			uVersion = m_vNeighbour.size();
			WFile.Write(&uVersion, sizeof(UINT));
			if (uVersion > 0)
				WFile.Write(&m_vNeighbour[0], sizeof(VGridNeighbour)*uVersion);
			WFile.Close();
			m_bHavePath = TRUE;
			//g_DebugLog("Taomoi 2");
		}
	}
	//g_DebugLog("LoadMap end");
	return TRUE;
}
#endif

void KSubWorld::LoadTrap()
{

}

void KSubWorld::MessageLoop()
{
	KWorldMsgNode	Msg;
	while(m_WorldMessage.Get(&Msg))
	{
		ProcessMsg(&Msg);
	}
}

void KSubWorld::ProcessMsg(KWorldMsgNode *pMsg)
{
	switch(pMsg->m_dwMsgType)
	{
	case GWM_NPC_DEL:
#ifndef _SERVER
		if (Npc[pMsg->m_nParam[0]].m_RegionIndex >= 0)
		{
			int nIdx = pMsg->m_nParam[0];
			int nSubWorld = Npc[nIdx].m_SubWorldIndex;
			int nRegion = Npc[nIdx].m_RegionIndex;
			SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(nIdx);
			SubWorld[nSubWorld].m_Region[nRegion].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
		}
		NpcSet.Remove(pMsg->m_nParam[0]);
#else
		{
			int nIdx = pMsg->m_nParam[0];
			int nSubWorld = Npc[nIdx].m_SubWorldIndex;
			int nRegion = Npc[nIdx].m_RegionIndex;
			
			SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(nIdx);
			SubWorld[nSubWorld].m_Region[nRegion].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
			NpcSet.Remove(nIdx);
		}
#endif
		break;
/*	case GWM_NPC_CHANGE_REGION:
		NpcChangeRegion(pMsg->m_nParam[0], pMsg->m_nParam[1], pMsg->m_nParam[2]);
		break;*/
	case GWM_OBJ_DEL:
#ifndef _SERVER
		if (Object[pMsg->m_nParam[0]].m_nRegionIdx >= 0)
			SubWorld[Object[pMsg->m_nParam[0]].m_nSubWorldID].m_Region[Object[pMsg->m_nParam[0]].m_nRegionIdx].RemoveObj(pMsg->m_nParam[0]);
		ObjSet.Remove(pMsg->m_nParam[0]);
#else
		SubWorld[Object[pMsg->m_nParam[0]].m_nSubWorldID].m_Region[Object[pMsg->m_nParam[0]].m_nRegionIdx].RemoveObj(pMsg->m_nParam[0]);
		ObjSet.Remove(pMsg->m_nParam[0]);
#endif
		break;
	case GWM_OBJ_CHANGE_REGION:
		ObjChangeRegion(pMsg->m_nParam[0], pMsg->m_nParam[1], pMsg->m_nParam[2]);
		break;
	case GWM_MISSLE_CHANGE_REGION:
		MissleChangeRegion(pMsg->m_nParam[0], pMsg->m_nParam[1], pMsg->m_nParam[2]);
		break;
	case GWM_MISSLE_DEL:
#ifndef _SERVER
		if (Missle[pMsg->m_nParam[0]].m_nRegionId >= 0)
		{
			SubWorld[Missle[pMsg->m_nParam[0]].m_nSubWorldId].m_Region[Missle[pMsg->m_nParam[0]].m_nRegionId].RemoveMissle(pMsg->m_nParam[0]);
			//g_DebugLog("[Missle]Missle%dDEL", pMsg->m_nParam[0]);
		   #ifdef _DEBUG
			char szDebugString[128];
			sprintf(szDebugString, "[Missle]Missle%dDEL \n", pMsg->m_nParam[0]);
			//OutputDebugString(szDebugString);
			#endif
			MissleSet.Remove(pMsg->m_nParam[0]);
		}
#else
		// FIX 24/08: nhanh SERVER thieu chan m_nRegionId >= 0 ma nhanh CLIENT ngay tren da co.
		// Tu 24/08 KMissleSet::Remove dat m_nRegionId = -1 sau khi tra bo dem, nen lan GWM_MISSLE_DEL
		// THU HAI cho cung mot khe se roi vao day. Bo qua la dung: lan dau da go nut khoi danh sach roi.
		// (KRegion::RemoveMissle khong dung 'this' nen m_Region[-1] chi la phep cong dia chi, khong
		// doc/ghi ngoai mang - nhung van nen chan cho ro y.)
		if (Missle[pMsg->m_nParam[0]].m_nRegionId >= 0)
			SubWorld[Missle[pMsg->m_nParam[0]].m_nSubWorldId].m_Region[Missle[pMsg->m_nParam[0]].m_nRegionId].RemoveMissle(pMsg->m_nParam[0]);
		MissleSet.Remove(pMsg->m_nParam[0]);
#endif
		break;
/*	case GWM_PLAYER_CHANGE_REGION:
		PlayerChangeRegion(pMsg->m_nParam[0], pMsg->m_nParam[1], pMsg->m_nParam[2]);
		break;*/
	default:
		break;
	}	
}

#ifdef _SERVER
void KSubWorld::NpcChangeRegion(int nSrcRnidx, int nDesRnIdx, int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_NPC)
		return;

	// [S13i 03/09] Chi dat co khi NPC se con duoc duyet lai trong CUNG khung: dang trong vong duyet vung va (dich = VOID
	// - danh sach VOID duyet sau cac vung; hoac dich >= vung dang duyet - AddNpc noi DUOI). Sang vung da duyet, hoac doi
	// vung ngoai vong duyet (MessageLoop/script/SetPos/hoi sinh trong danh sach VOID): khong dat -> khung sau kich hoat
	// binh thuong. Truoc day dat cho MOI truong hop -> mat mot khung (xem chu thich tren s_nS13iVungDangDuyet).
	if (s_nS13iVungDangDuyet >= 0 && s_nS13iVungDangDuyet != 0x7FFFFFFF
		&& (nDesRnIdx == VOID_REGION || nDesRnIdx >= s_nS13iVungDangDuyet))
		Npc[nIdx].SetActiveFlag(TRUE);
	if (nSrcRnidx == -1)
	{
		_ASSERT(nDesRnIdx >= 0);
		if (nDesRnIdx >= 0)
		{
			m_Region[nDesRnIdx].AddNpc(nIdx);
			Npc[nIdx].m_RegionIndex = nDesRnIdx;
		}
		return;
	}
	else if (nSrcRnidx == VOID_REGION)
	{
		Npc[nIdx].m_Node.Remove();
		Npc[nIdx].m_Node.Release();
	}
	else if (nSrcRnidx >= 0)
	{
		SubWorld[Npc[nIdx].m_SubWorldIndex].m_Region[nSrcRnidx].RemoveNpc(nIdx);
		if (nDesRnIdx != VOID_REGION)	// 
			Npc[nIdx].m_RegionIndex = -1;
	}

	if (nDesRnIdx >= 0)
	{
		m_Region[nDesRnIdx].AddNpc(nIdx);
		Npc[nIdx].m_RegionIndex = nDesRnIdx;
	}
	else if (nDesRnIdx == VOID_REGION)
	{
		m_NoneRegionNpcList.AddTail(&Npc[nIdx].m_Node);		//hoi sinh quai sau khi chet
		Npc[nIdx].m_Node.AddRef();
	}
}
#endif

#ifndef _SERVER
void KSubWorld::NpcChangeRegion(int nSrcRnidx, int nDesRnIdx, int nIdx)
{
	int		nSrc, nDest;

	if (nSrcRnidx == -1)
	{
		nDest = SubWorld[0].FindRegion(nDesRnIdx);
		if (nDest < 0)
			return;
		m_Region[nDest].AddNpc(nIdx);
		Npc[nIdx].m_dwRegionID = m_Region[nDest].m_RegionID;
		Npc[nIdx].m_RegionIndex = nDest;
		return;
	}

	nSrc = SubWorld[0].FindRegion(nSrcRnidx);
	if (nSrc >= 0)
		SubWorld[0].m_Region[nSrc].RemoveNpc(nIdx);

	nDest = SubWorld[0].FindRegion(nDesRnIdx);

	KIndexNode *pNode = &Npc[nIdx].m_Node;
	if (nDest >= 0)
	{
		m_Region[nDest].AddNpc(nIdx);
		
		if (Player[CLIENT_PLAYER_INDEX].m_nIndex == nIdx)
			LoadMap(m_SubWorldID, m_Region[nDest].m_RegionID);
		Npc[nIdx].m_dwRegionID = m_Region[nDest].m_RegionID;
		Npc[nIdx].m_RegionIndex = nDest;
	}
	else if (Player[CLIENT_PLAYER_INDEX].m_nIndex == nIdx)
	{
		LoadMap(m_SubWorldID, nDesRnIdx);
		nDest = SubWorld[0].FindRegion(nDesRnIdx);
		if (nDest >= 0)
		{
			m_Region[nDest].AddNpc(nIdx);
			Npc[nIdx].m_dwRegionID = m_Region[nDest].m_RegionID;
			Npc[nIdx].m_RegionIndex = nDest;
		}
	}
}
#endif

void KSubWorld::ObjChangeRegion(int nSrcRnidx, int nDesRnIdx, int nIdx)
{
#ifdef _SERVER
	_ASSERT(nSrcRnidx >= 0);
#endif
	if (nDesRnIdx == -1)
	{
		SubWorld[Object[nIdx].m_nSubWorldID].m_Region[nSrcRnidx].RemoveObj(nIdx);
//		ObjSet.Remove(nIdx);
		return;
	}
	if (nSrcRnidx == -1)
	{
		m_Region[nDesRnIdx].AddObj(nIdx);
		return;
	}
	KIndexNode* pNode = (KIndexNode *)m_Region[nSrcRnidx].GetObjNode(nIdx);
	if (pNode)
	{
		pNode->Remove();
		m_Region[nDesRnIdx].m_ObjList.AddTail(pNode);
	}
}

void KSubWorld::MissleChangeRegion(int nSrcRnidx, int nDesRnIdx, int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_MISSLE)
		return;

	if (nSrcRnidx == -1)
	{
		m_Region[nDesRnIdx].AddMissle(nIdx);
		return;
	}

	SubWorld[Missle[nIdx].m_nSubWorldId].m_Region[nSrcRnidx].RemoveMissle(nIdx);

	if (nDesRnIdx == -1)
	{
		MissleSet.Remove(nIdx);
		return;
	}
	
	m_Region[nDesRnIdx].AddMissle(nIdx);
}

#ifdef _SERVER
void KSubWorld::PlayerChangeRegion(int nSrcRnidx, int nDesRnIdx, int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_PLAYER)
		return;

	if (nSrcRnidx < 0)
		return;

	RemovePlayer(nSrcRnidx, nIdx);
	
	if (nDesRnIdx == -1)
	{
		//PlayerSet.PrepareRemove(Player[nIdx].m_nNetConnectIdx);
		printf("Player change region to not exist..., kill it\n");
		g_pServer->ShutdownClient(Player[nIdx].m_nNetConnectIdx);
		return;
	}

	if (nDesRnIdx >= 0)
	{
		AddPlayer(nDesRnIdx, nIdx);
	}
/* ÒòÎªRemovePlayerºÍAddPlayerÒÑ¾­×öÁË´ÓÁ´±íÖÐÇå³ý½Úµã£¬ÐÞ¸ÄÖÜ±ß¾Å¸öREGIONµÄÒýÓÃ¼ÆÊýµÄ²Ù×÷ÁË
	KIndexNode* pNode = &Player[nIdx].m_Node;
	if (pNode->m_Ref > 0)
	{
		pNode->Remove();
		pNode->Release();
	}
	m_Region[nSrcRnidx].m_nActive--;
	if (m_Region[nSrcRnidx].m_nActive < 0)
		m_Region[nSrcRnidx].m_nActive = 0;
	
	for (int i = 0; i < 8; i++)
	{
		if (m_Region[nSrcRnidx].m_nConnectRegion[i] < 0)
			continue;
		m_Region[m_Region[nSrcRnidx].m_nConnectRegion[i]].m_nActive--;
		if (m_Region[m_Region[nSrcRnidx].m_nConnectRegion[i]].m_nActive < 0)
			m_Region[m_Region[nSrcRnidx].m_nConnectRegion[i]].m_nActive = 0;
	}

	if (pNode->m_Ref == 0)
	{
		m_Region[nDesRnIdx].m_PlayerList.AddTail(pNode);
		pNode->AddRef();
	}

	m_Region[nDesRnIdx].m_nActive++;
	for (i = 0; i < 8; i++)
	{
		if (m_Region[nDesRnIdx].m_nConnectRegion[i] < 0)
			continue;
		m_Region[m_Region[nDesRnIdx].m_nConnectRegion[i]].m_nActive++;
		if (m_Region[m_Region[nDesRnIdx].m_nConnectRegion[i]].m_nActive < 0)
			m_Region[m_Region[nDesRnIdx].m_nConnectRegion[i]].m_nActive = 0;
	}
*/
}
#endif

void KSubWorld::GetMps(int *nX, int *nY, int nSpeed, int nDir, int nMaxDir /* = 64 */)
{
	*nX += (g_DirCos(nDir, nMaxDir) * nSpeed) >> 10;
	*nY += (g_DirSin(nDir, nMaxDir) * nSpeed) >> 10;
}

#ifdef _SERVER
BOOL KSubWorld::SendSyncData(int nIdx, int nClient)
{
	// (19/08 dem) BOT / ket noi -1: dung dong goi + DUNG printf. Xua nay van
	// fail (PackDataToClient truot cong ulnClientID) va tra FALSE - cac duong
	// goi (nap nhan vat theo nStep, KNpc::ChangeWorld vut gia tri) khong doc
	// ket qua nay, nen hanh vi KHONG doi. Cai doi la: 1000 bot doi map trong
	// cua so 60 giay cua che do VE THANH lam console in hang nghin dong
	// 'Packing world sync data failed' - console I/O dong bo tren main thread
	// (~ms/dong) lam giat khung server va che sach log that.
	if (nClient < 0)
		return FALSE;
	WORLD_SYNC	WorldSync;
	WorldSync.ProtocolType = (BYTE)s2c_syncworld;
	WorldSync.Region = m_Region[Npc[nIdx].m_RegionIndex].m_RegionID;
	WorldSync.Weather = m_nWeather;
	WorldSync.Frame = m_dwCurrentTime;
	WorldSync.SubWorld = m_SubWorldID;

	if (SUCCEEDED(g_pServer->PackDataToClient(nClient, (BYTE*)&WorldSync, sizeof(WORLD_SYNC))))
	{
		return TRUE;
	}
	else
	{
		printf("player Packing world sync data failed...\n");
		return FALSE;
	}
}
#endif

#ifndef _SERVER
void KSubWorld::LoadCell()
{
}
#endif

#ifdef TOOLVERSION
int CORE_API g_ScreenX  = 0;
int CORE_API g_ScreenY  = 0;
#endif

#ifndef _SERVER
#include "../../Represent/iRepresent/iRepresentshell.h"
extern struct iRepresentShell*	g_pRepresent;
void KSubWorld::Paint()
{
	if(m_uPaintTime > timeGetTime())
		return;
	int nIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	int nX, nY;

	Map2Mps(Npc[nIdx].m_RegionIndex, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, &nX, &nY);
	
//	m_nScreenX = nX - 400;
//	m_nScreenY = nY - 300;

#ifdef TOOLVERSION
	g_ScreenX = nX;
	g_ScreenY = nY;
#endif

	/*for (int i = 0; i < m_nTotalRegion; i++)
	{
		m_Region[i].Paint();
	}*/
	if(g_pRepresent && m_bHavePath && m_nGridTotal && (!m_hLoadPathGrid || WaitForSingleObject(m_hLoadPathGrid, 0) == WAIT_OBJECT_0))
	{
		KRURect	Rect;
		Rect.Color.Color_dw = 0xff00ff00;
		Rect.oPosition.nZ = Rect.oEndPos.nZ = 0;
		int regx = (nX/512-m_nRegionBeginX)*REGION_GRID_WIDTH;
		int regy = (nY/1024-m_nRegionBeginY)*REGION_GRID_HEIGHT;
		for(int y=0;y<REGION_GRID_HEIGHT;++y)
		{
			for(int x=0;x<REGION_GRID_WIDTH;++x)
			{
				int id=(regy+y)*m_nGridW*REGION_GRID_WIDTH + regx+x;
				if(m_GridNode[id].obs)
				{
					id = m_GridNode[id].parentId;
					Rect.oPosition.nX = m_GridNode[id].x*32;
					Rect.oPosition.nY = m_GridNode[id].y*32;
					Rect.oEndPos.nX = Rect.oPosition.nX + m_GridNode[id].w*32;
					Rect.oEndPos.nY = Rect.oPosition.nY + m_GridNode[id].h*32;
					g_pRepresent->DrawPrimitives(1, &Rect, RU_T_RECT, false);
				}
			}
		}
		regx = nX/32-m_nRegionBeginX*REGION_GRID_WIDTH;
		regy = nY/32-m_nRegionBeginY*REGION_GRID_HEIGHT;
		int nid = regy*m_nGridW*REGION_GRID_WIDTH + regx;
		nid = m_GridNode[nid].parentId;
		Rect.Color.Color_dw = 0xffff0000;
		Rect.oPosition.nX = m_GridNode[nid].x*32;
		Rect.oPosition.nY = m_GridNode[nid].y*32;
		Rect.oEndPos.nX = Rect.oPosition.nX + m_GridNode[nid].w*32;
		Rect.oEndPos.nY = Rect.oPosition.nY + m_GridNode[nid].h*32;
		g_pRepresent->DrawPrimitives(1, &Rect, RU_T_RECT, false);
	}
}

void KSubWorld::Mps2Screen(int *Rx, int *Ry)
{
}

void KSubWorld::Screen2Mps(int *Rx, int *Ry)
{
}
#endif

#ifdef _SERVER
int KSubWorld::GetRegionIndex(int nRegionID)
{
	int nRet;
	short nX = (short)LOWORD(nRegionID) - m_nRegionBeginX;
	short nY = (short)HIWORD(nRegionID) - m_nRegionBeginY;

	if (nX < 0 || nY < 0 || nX >= m_nWorldRegionWidth || nY >= m_nWorldRegionHeight)
		return -1;

	nRet = nY * m_nWorldRegionWidth + nX;
	return nRet;
}
#endif

void KSubWorld::Close()
{
	if (m_SubWorldID == -1)
		return;

	for (int i = 0; i < m_nTotalRegion; i++)
	{
		m_Region[i].Close();
	}
	m_WorldMessage.Clear();
	m_nIndex = -1;
	m_SubWorldID = -1;
#ifndef _SERVER
	if(m_hLoadPathGrid)
	{
		m_bStopThread = TRUE;
		WaitForSingleObject(m_hLoadPathGrid, INFINITE);
        CloseHandle(m_hLoadPathGrid);
        m_hLoadPathGrid = NULL;
		m_bStopThread = FALSE;
		m_vRetPath.clear();
		m_nCurStep = 0;
		m_nTargetX = m_nTargetY = 0;
		m_uStepDelayTime = 0;

	}
	m_vNeighbour.clear();
	m_bHavePath = FALSE;
#endif
}

#ifdef _SERVER
void KSubWorld::LoadObject(char* szPath, char* szFileName)
{
	if (!szFileName || !szFileName[0])
		return;

	KIniFile IniFile;
	// Npc Object Trap
	g_SetFilePath(szPath);
	int nObjNumber = 0;
	if (IniFile.Load(szFileName))
	{
		IniFile.GetInteger("MAIN", "elementnum", 0, &nObjNumber);
	}

	int nLength = strlen(szFileName);
	szFileName[nLength - 4] = 0;
	strcat(szPath, "\\");
	strcat(szPath, szFileName);

	char	szSection[32];
	char	szDataFile[32];

	for (int i = 0; i < nObjNumber; i++)
	{
		float	fPos[3];
		int		nPos[3];

		g_SetFilePath(szPath);
		sprintf(szSection, "%d", i);
		IniFile.GetFloat3(szSection, "groundoffset", fPos);
		nPos[0] = (int)(fPos[0] * 32);
		nPos[1] = (int)(fPos[1] * 32);
		nPos[2] = (int)fPos[2];

		szDataFile[0] = 0;
		IniFile.GetString(szSection, "event", "", szDataFile, sizeof(szDataFile));
		strcat(szDataFile, "_data.ini");
		
		KIniFile IniChange;
		int nType = 0, nLevel = 0;
		char	szName[32];
		
		if (!IniChange.Load(szDataFile))
			continue;
		
		IniChange.GetInteger("MAIN", "Type", 0, &nType);
		IniChange.GetInteger("MAIN", "Level", 4, &nLevel);
		IniChange.GetString("MAIN", "mapedit_templatesection", "", szName, sizeof(szName));
		
		switch(nType)
		{
		case kind_normal:		// ÆÕÍ¨Õ½¶·npc
			{
				int nFindNo = g_NpcSetting.FindRow(szName);
				if (nFindNo == -1)
					continue;
				else
					nFindNo = nFindNo - 2;
				
				int nSettingInfo = MAKELONG(nLevel, nFindNo);
				int nSeries = 0;
				NpcSet.AddNpcSet2(nSettingInfo, nSeries, m_nIndex, nPos[0], nPos[1]);
			}
			break;
		case kind_player:
			break;
		case kind_partner:
			break;
		case kind_dialoger:		// ÆÕÍ¨·ÇÕ½¶·npc
			break;
		case kind_bird:			// ¿Í»§¶Ëonly
			break;
		case kind_mouse:		// ¿Í»§¶Ëonly
			break;
		default:
			break;
		}
	}
}
#endif

void KSubWorld::AddPlayer(int nRegion, int nIdx)
{
	if (nRegion < 0 || nRegion >= m_nTotalRegion)
	{
		printf("Region:(%d) Player(%d)\n", nRegion, nIdx);
		return;
	}

	if (m_Region[nRegion].AddPlayer(nIdx))
	{
		m_Region[nRegion].m_nActive++;
#ifdef _SERVER
		SubWorld_DanhThucQuetRegion(this);	// [24/08] co nguoi vao: quet lai ngay
#endif

		for (int i = 0; i < 8; i++)
		{
			int nConRegion = m_Region[nRegion].m_nConnectRegion[i];
			if (nConRegion == -1)
				continue;
			
			m_Region[nConRegion].m_nActive++;
		}
	}
	else
	{
		printf("Region:(%d) Player(%d)\n", nRegion, nIdx);
	}
}

void KSubWorld::RemovePlayer(int nRegion, int nIdx)
{
	if (nRegion < 0 || nRegion >= m_nTotalRegion)
		return;

	if (m_Region[nRegion].RemovePlayer(nIdx))
	{	
		m_Region[nRegion].m_nActive--;		

		if (m_Region[nRegion].m_nActive < 0)
		{
			_ASSERT(0);
			m_Region[nRegion].m_nActive = 0;
		}
		
		for (int i = 0; i < 8; i++)
		{
			int nConRegion = m_Region[nRegion].m_nConnectRegion[i];
			if (nConRegion == -1)
				continue;
			
			m_Region[nConRegion].m_nActive--;

			if (m_Region[nConRegion].m_nActive < 0)
			{
				_ASSERT(0);
				m_Region[nConRegion].m_nActive = 0;
			}
		}
	}
}

void KSubWorld::GetRandomObjPos(POINT& pos)// add by phong kiÒu r¬i item v?tr?ngÉu nhiªn quanh to?®é
{
	int i = g_Random(3);
	POINT	posLocal = pos;
	POINT	posTemp;

	int myrandom = g_Random(48);
	if (myrandom <= 1)
	{
		posTemp.y = posLocal.y + i*32;
		posTemp.x = posLocal.x + i*32;	
		pos = posTemp;		
	}
	else if(myrandom > 1 && myrandom <= 24)
	{
		posTemp.y = posLocal.y + i*32;
		posTemp.x = posLocal.x - i*32;	
		pos = posTemp;
	}	
	else if(myrandom > 24 && myrandom <= 36)
	{
		posTemp.y = posLocal.y - i*32;
		posTemp.x = posLocal.x + i*32;	
		pos = posTemp;
	}
	else if(myrandom > 36 && myrandom <= 48)
	{
		posTemp.y = posLocal.y - i*32;
		posTemp.x = posLocal.x - i*32;	
		pos = posTemp;
	}
	return;
}

void KSubWorld::GetFreeObjPos(POINT& pos)
{
	POINT	posLocal = pos;
	POINT	posTemp;
	int nLayer = 1;
	bool checkRightUp = true, checkLeftUp = true, checkRightDown = true, checkLeftDown = true;

	if (CanPutObj(posLocal))
		return;

	while(1)
	{
		for (int i = 0; i <= nLayer; i++)
		{
			posTemp.y = posLocal.y + i * 32;
			posTemp.x = posLocal.x + (nLayer - i) * 32;
			if (!CanPutObjBarrier(posTemp))
			{
				checkRightUp = false;
			}
			if (CanPutObj(posTemp) && checkRightUp)
			{
				pos = posTemp;
				return;
			}

			posTemp.y = posLocal.y + i * 32;
			posTemp.x = posLocal.x - (nLayer - i) * 32;
			if (!CanPutObjBarrier(posTemp))
			{
				checkLeftUp = false;
			}
			if (CanPutObj(posTemp) && checkLeftUp)
			{
				pos = posTemp;
				return;
			}

			posTemp.y = posLocal.y - i * 32;
			posTemp.x = posLocal.x + (nLayer - i) * 32;
			if (!CanPutObjBarrier(posTemp))
			{
				checkRightDown = false;
			}
			if (CanPutObj(posTemp) && checkRightDown)
			{
				pos = posTemp;
				return;
			}

			posTemp.y = posLocal.y - i * 32;
			posTemp.x = posLocal.x - (nLayer - i) * 32;
			if (!CanPutObjBarrier(posTemp))
			{
				checkLeftDown = false;
			}
			if (CanPutObj(posTemp) && checkLeftDown)
			{
				pos = posTemp;
				return;
			}
		}
		nLayer++;
		if (nLayer >= 10)
			break;
	}
	return;
}

BOOL KSubWorld::CanPutObj(POINT pos)
{
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	Mps2Map(pos.x, pos.y, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

	if (nRegion >= 0
		&& !m_Region[nRegion].GetBarrier(nMapX, nMapY, nOffX, nOffY) 
		&& !m_Region[nRegion].GetRef(nMapX, nMapY, obj_object))
		return TRUE;
	return FALSE;
}

BOOL KSubWorld::CanPutObjBarrier(POINT pos)  // fix item rot ra ngoai map
{
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	Mps2Map(pos.x, pos.y, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

	if (nRegion >= 0) {
		if (!m_Region[nRegion].GetBarrier(nMapX, nMapY, nOffX, nOffY))
			return TRUE;
		else {
			// FIX 24/08: BO ve "|| GetRef(obj_missle)". Truoc khi va KRegion::AddRef (24/08), bo dem
			// dan luon = 0 nen ve nay CHUA BAO GIO chay; nay no co gia tri that. Dan BAY QUA duoc
			// tuong, no KHONG phai bang chung o do di duoc - de nguyen thi tia quet cua GetFreeObjPos
			// xuyen qua tuong va do roi ra sau tuong (dung trieu chung ham nay sinh ra de chua).
			// NPC thi khac: NPC dung duoc tren o nghia la bang vat can sai o do.
			if (m_Region[nRegion].GetRef(nMapX, nMapY, obj_npc))
				return TRUE;
		}
	}
	return FALSE;
}



#ifdef _SERVER
int	KSubWorld::DelAllNpcInWro()
{
	int nCount=0;
	for (int i = 0; i < m_nTotalRegion; ++i)  
	{	
		nCount +=m_Region[i].DelAllNpc();   
	}

	KIndexNode* pNode = (KIndexNode *)m_NoneRegionNpcList.GetHead();
	KIndexNode *pTmpNode = NULL;
	while(pNode)
	{//ÖØÉúÁÐ±íµÄ	npc
		pTmpNode = (KIndexNode *)pNode->GetNext();
		int  nNpcIdx =	pNode->m_nIndex;
		if  (nNpcIdx>0 && nNpcIdx < MAX_NPC && Npc[nNpcIdx].m_Kind!=kind_player)
		{
			if (Npc[nNpcIdx].m_RegionIndex>=0)
			{
			    //m_Region[Npc[nNpcIdx].m_RegionIndex].RemoveNpc(nNpcIdx);
				Npc[nNpcIdx].m_Node.Remove();
		        Npc[nNpcIdx].m_Node.Release();
			}
		    NpcSet.Remove(nNpcIdx);
			nCount++;
		}

		pNode=pTmpNode;
	}

    return nCount;
	
}

int	KSubWorld::DelAllNpcInWro(char* szName)
{
	int nCount = 0;
	for (int i = 0; i < m_nTotalRegion; ++i)
	{
		nCount += m_Region[i].DelAllNpc(m_SubWorldID, szName);
	}

	KIndexNode* pNode = (KIndexNode*)m_NoneRegionNpcList.GetHead();
	KIndexNode* pTmpNode = NULL;
	while (pNode)
	{//ÖØÉúÁÐ±íµÄ	npc
		pTmpNode = (KIndexNode*)pNode->GetNext();
		int  nNpcIdx = pNode->m_nIndex;
		if (nNpcIdx > 0 && nNpcIdx < MAX_NPC && Npc[nNpcIdx].m_Kind != kind_player)
		{
			if (strcmp(szName, Npc[nNpcIdx].Name) == 0) {
				if (Npc[nNpcIdx].m_RegionIndex >= 0)
				{
					//m_Region[Npc[nNpcIdx].m_RegionIndex].RemoveNpc(nNpcIdx);
					Npc[nNpcIdx].m_Node.Remove();
					Npc[nNpcIdx].m_Node.Release();
				}
				NpcSet.Remove(nNpcIdx);
				nCount++;
			}
		}

		pNode = pTmpNode;
	}

	return nCount;

}
#endif

#ifdef _SERVER
BOOL KSubWorld::ExecuteScript(char * ScriptFileName, char * szFunName, int nParam)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	return ExecuteScript(g_FileName2Id(ScriptFileName), szFunName, nParam);	
}

BOOL KSubWorld::ExecuteScript(DWORD dwScriptId,  char * szFunName, int nParam)
{
	try
	{
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		if (pScript)
		{	
			int nTopIndex = 0;
			
			pScript->SafeCallBegin(&nTopIndex);
			if (pScript->CallFunction(szFunName,0, "d", nParam)) 
			{
				bExecuteScriptMistake = false;
			}
			pScript->SafeCallEnd(nTopIndex);
		}
		
		if (bExecuteScriptMistake)
		{
			return FALSE;
		}
		
		return TRUE;
	}
	catch(...)
	{
		printf("Exception Have Caught When Execute Script[%d]!!!!!", dwScriptId);
		return FALSE;
	}
	return TRUE;
}

void KSubWorld::BroadCast(const char* pBuffer, size_t uSize)
{
	int nIdx = PlayerSet.GetFirstPlayer();
	while(nIdx)
	{
		if (Player[nIdx].m_nNetConnectIdx >= 0 
			&& Player[nIdx].m_nIndex > 0 
			&& Npc[Player[nIdx].m_nIndex].m_SubWorldIndex == m_nIndex)
		{
			g_pServer->PackDataToClient(Player[nIdx].m_nNetConnectIdx, pBuffer, uSize);
		}
		nIdx = PlayerSet.GetNextPlayer();
	}
}

int	KSubWorld::RevivalAllNpc()
{
	KIndexNode * pNode		= NULL;
	size_t	ulCount = 0;
	for (int i = 0; i < m_nTotalRegion; i++)
	{
		KRegion * pCurRegion = &m_Region[i];
		pNode = (KIndexNode *)pCurRegion->m_NpcList.GetHead();
		
		while(pNode)
		{
			int nNpcIdx = pNode->m_nIndex;
			if (!Npc[nNpcIdx].IsPlayer() && Npc[nNpcIdx].m_Kind!=kind_dialoger)
			{
              if (Npc[nNpcIdx].m_Doing == do_revive)
                {
                Npc[nNpcIdx].m_Doing = do_none;
                }
				Npc[nNpcIdx].ExecuteRevive();
			}
			pNode = (KIndexNode *)pNode->GetNext();	
		}
	}
	
	pNode = (KIndexNode*)m_NoneRegionNpcList.GetHead();
	
	while(pNode)
	{
		int nNpcIdx = pNode->m_nIndex;
		if (!Npc[nNpcIdx].IsPlayer() && Npc[nNpcIdx].m_Kind!=kind_dialoger)
		{
			Npc[nNpcIdx].m_Frames.nTotalFrame = 1;
			Npc[nNpcIdx].m_Frames.nCurrentFrame = 0;
			ulCount ++;
		}
		pNode = (KIndexNode *)pNode->GetNext();	
	}
	
	return ulCount;
}

int KSubWorld::FindNpcFromName(const char * szName)
{
	if (!szName || !szName[0])
		return 0;
	
	KIndexNode * pNode		= NULL;

	int nResult = 0;
	
	for (int i = 0; i < m_nTotalRegion; i++)
	{
		KRegion * pCurRegion = &m_Region[i];
		pNode = (KIndexNode *)pCurRegion->m_NpcList.GetHead();
		
		while(pNode)
		{
			int nNpcIdx = pNode->m_nIndex;
			if (!strcmp(Npc[nNpcIdx].Name, szName))
			{
				nResult = nNpcIdx;
				return nResult;
			}
			pNode = (KIndexNode *)pNode->GetNext();	
		}
	}	
	
	pNode = (KIndexNode*)m_NoneRegionNpcList.GetHead();
	
	while(pNode)
	{
		int nNpcIdx = pNode->m_nIndex;
		if (!strcmp(Npc[nNpcIdx].Name, szName))
		{
			return nNpcIdx;
		}
		pNode = (KIndexNode *)pNode->GetNext();	
	}
	
	return nResult;
}

long	KSubWorld::CountAllNpc()
{
	KIndexNode * pNode		= NULL;
	long	ulCount = 0;
	for (int i = 0; i < m_nTotalRegion; i++)
	{
		KRegion * pCurRegion = &m_Region[i];
		pNode = (KIndexNode *)pCurRegion->m_NpcList.GetHead();
		
		while(pNode)
		{
			ulCount++;
			pNode = (KIndexNode *)pNode->GetNext();	
		}
	}
	
	pNode = (KIndexNode*)m_NoneRegionNpcList.GetHead();
	
	while(pNode)
	{
		ulCount ++;
		pNode = (KIndexNode *)pNode->GetNext();	
	}
	
	return ulCount;
}
//GetAllPlayerIndexes

std::vector<int> KSubWorld::GetAllPlayerIndexes() {
	KIndexNode* pNode = NULL;
	std::vector<int> result;
	for (int i = 0; i < m_nTotalRegion; i++)
	{
		KRegion* pCurRegion = &m_Region[i];
		pNode = (KIndexNode*)pCurRegion->m_PlayerList.GetHead();

		if (pNode)
		{
			result.push_back(pNode->m_nIndex);

		}
	}
	return result;
}
long	KSubWorld::CountAllPlayer()
{
	KIndexNode* pNode = NULL;
	long	ulCount = 0;
	for (int i = 0; i < m_nTotalRegion; i++)
	{
		KRegion* pCurRegion = &m_Region[i];
		pNode = (KIndexNode*)pCurRegion->m_PlayerList.GetHead();

		while (pNode)
		{
			ulCount++;
			pNode = (KIndexNode*)pNode->GetNext();
		}
	}
	return ulCount;
}

long	KSubWorld::GetLastPlayerIndex(int nCurrentPlayerIndex)
{
	KIndexNode* pNode = NULL;
	int nLastPlayerIndex = 0;
	for (int i = 0; i < m_nTotalRegion; i++)
	{
		KRegion* pCurRegion = &m_Region[i];
		pNode = (KIndexNode*)pCurRegion->m_PlayerList.GetHead();

		if (pNode)
		{
			if (pNode->m_nIndex != nCurrentPlayerIndex) 
			{
				nLastPlayerIndex = pNode->m_nIndex;
				break;
			}
			
		}
	}
	return nLastPlayerIndex;
}

// [PORT5 23/08] tham so o trap JX2 (AddMapTrap tham so 5) - chi o chinh (nRange 0)
void KSubWorld::SetTrapParam(int nMpsX, int nMpsY, int nParam)
{
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion == -1)
		return;
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_GRID_WIDTH || nMapY >= REGION_GRID_HEIGHT)
		return;
	m_Region[nRegion].SetTrapParam(nMapX, nMapY, nParam);
}

void KSubWorld::SetTrap(DWORD dwTrapId, int nMpsX, int nMpsY, int nRange)
{
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion == -1)
		return;
	
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_GRID_WIDTH || nMapY >= REGION_GRID_HEIGHT)
		return;
	
	m_Region[nRegion].SetTrap(dwTrapId, nMapX, nMapY);
	for (int i = 1; i <= nRange; i++)
	{
		if (nMapX - i >= 0)
			m_Region[nRegion].SetTrap(dwTrapId, nMapX - i, nMapY);
		else if (m_Region[nRegion].m_nConnectRegion[DIR_LEFT] >= 0)	// FIX 24/08: chan m_Region[-1] o ria ban do
			m_Region[m_Region[nRegion].m_nConnectRegion[DIR_LEFT]].SetTrap(dwTrapId, REGION_GRID_WIDTH - i, nMapY);
		
		if (nMapX + i < REGION_GRID_WIDTH)
			m_Region[nRegion].SetTrap(dwTrapId, nMapX + i, nMapY);
		else if (m_Region[nRegion].m_nConnectRegion[DIR_RIGHT] >= 0)	// FIX 24/08: chan m_Region[-1] o ria ban do
			m_Region[m_Region[nRegion].m_nConnectRegion[DIR_RIGHT]].SetTrap(dwTrapId, i-1, nMapY);
		
		if (nMapY - i >= 0)
			m_Region[nRegion].SetTrap(dwTrapId, nMapX, nMapY - i);
		else if (m_Region[nRegion].m_nConnectRegion[DIR_UP] >= 0)	// FIX 24/08: chan m_Region[-1] o ria ban do
			m_Region[m_Region[nRegion].m_nConnectRegion[DIR_UP]].SetTrap(dwTrapId, nMapX, REGION_GRID_HEIGHT - i);
		
		if (nMapY + i < REGION_GRID_HEIGHT)
			m_Region[nRegion].SetTrap(dwTrapId, nMapX, nMapY + i);
		else if (m_Region[nRegion].m_nConnectRegion[DIR_DOWN] >= 0)	// FIX 24/08: chan m_Region[-1] o ria ban do
			m_Region[m_Region[nRegion].m_nConnectRegion[DIR_DOWN]].SetTrap(dwTrapId, nMapX, i-1);
	}
}

void KSubWorld::SetObstacle(long value, int nMpsX, int nMpsY, int nRange) //#Set VËt C¶n
{
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion == -1)
		return;
	
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_GRID_WIDTH || nMapY >= REGION_GRID_HEIGHT)
		return;
	
	// FIX 24/08: o TAM truoc goi SetTrap => ghi vao bang BAY thay vi bang VAT CAN.
	m_Region[nRegion].SetObstacle(value, m_SubWorldID, nMapX, nMapY);
	for (int i = 1; i <= nRange; i++)
	{
		if (nMapX - i >= 0)
			m_Region[nRegion].SetObstacle(value, m_SubWorldID, nMapX - i, nMapY);
		else if (m_Region[nRegion].m_nConnectRegion[DIR_LEFT] >= 0)	// FIX 24/08: chan m_Region[-1] o ria ban do
			m_Region[m_Region[nRegion].m_nConnectRegion[DIR_LEFT]].SetObstacle(value, m_SubWorldID, REGION_GRID_WIDTH - i, nMapY);
		
		if (nMapX + i < REGION_GRID_WIDTH)
			m_Region[nRegion].SetObstacle(value, m_SubWorldID, nMapX + i, nMapY);
		else if (m_Region[nRegion].m_nConnectRegion[DIR_RIGHT] >= 0)	// FIX 24/08: chan m_Region[-1] o ria ban do
			m_Region[m_Region[nRegion].m_nConnectRegion[DIR_RIGHT]].SetObstacle(value, m_SubWorldID, i-1, nMapY);
		
		if (nMapY - i >= 0)
			m_Region[nRegion].SetObstacle(value, m_SubWorldID, nMapX, nMapY - i);
		else if (m_Region[nRegion].m_nConnectRegion[DIR_UP] >= 0)	// FIX 24/08: chan m_Region[-1] o ria ban do
			m_Region[m_Region[nRegion].m_nConnectRegion[DIR_UP]].SetObstacle(value, m_SubWorldID, nMapX, REGION_GRID_HEIGHT - i);
		
		if (nMapY + i < REGION_GRID_HEIGHT)
			m_Region[nRegion].SetObstacle(value, m_SubWorldID, nMapX, nMapY + i);
		else if (m_Region[nRegion].m_nConnectRegion[DIR_DOWN] >= 0)	// FIX 24/08: chan m_Region[-1] o ria ban do
			m_Region[m_Region[nRegion].m_nConnectRegion[DIR_DOWN]].SetObstacle(value, m_SubWorldID, nMapX, i-1);
	}
}

#endif

#ifdef _SERVER
//===========================================================================
// A* PHIA SERVER cho bot. Ba ham duoi day la phan DUY NHAT phai viet moi:
// thuat toan (ProcLoadPathGrid / BlockHeuristic / FindPath_Block / FindPath_NpcObs /
// FindFreeBlockAround) da co san trong tep nay, truoc chi bi nhot sau #ifndef _SERVER.
//
// Vi sao can A*: KNpcFindPath chi cho ne DUNG MOT vat can moi lenh SendCommand
// (KNpcFindPath.cpp:106-111) roi DoStand vinh vien; cong them JX1 tinh NPC LA TUONG
// (KSubWorld.cpp TestBarrierMin -> GetBarrierMin(bCheckNpc=TRUE)). Nen di xa bang
// men-tuong la ket. A* tra ve lo trinh da ne san, moi chang giua hai diem la duong
// thong -> gioi han mot-lan-ne khong con can.
//===========================================================================

// Doi id BLOCK -> toa do MPS tam block. 1 o luoi = 32 MPS.
bool KSubWorld::BlockCenterMps(int nBlockId, int& nMpsX, int& nMpsY)
{
	if (!m_GridNode || nBlockId < 0 || nBlockId >= m_nGridTotal)
		return false;
	const VGridNode& nd = m_GridNode[nBlockId];
	nMpsX = (int)nd.x * 32 + (int)nd.w * 32 / 2;
	nMpsY = (int)nd.y * 32 + (int)nd.h * 32 / 2;
	return true;
}

bool KSubWorld::BlockNearestMps(int nBlockId, int nFromX, int nFromY, int& nMpsX, int& nMpsY)
{
	if (!m_GridNode || nBlockId < 0 || nBlockId >= m_nGridTotal)
		return false;
	const VGridNode& nd = m_GridNode[nBlockId];
	// bien hinh chu nhat cua block, thut vao nua o de khong bam sat mep vat can
	const int x0 = (int)nd.x * 32 + 16;
	const int y0 = (int)nd.y * 32 + 16;
	const int x1 = ((int)nd.x + (int)nd.w) * 32 - 16;
	const int y1 = ((int)nd.y + (int)nd.h) * 32 - 16;
	nMpsX = (nFromX < x0) ? x0 : ((nFromX > x1) ? x1 : nFromX);
	nMpsY = (nFromY < y0) ? y0 : ((nFromY > y1) ? y1 : nFromY);
	return true;
}

// Nap luoi tim duong cho ban do nay. Goi mot lan cho moi ban do CAN bot di lai.
//
// BO NHO: client dung mang tinh VGridNode[MAX_CELL] (2.400.000 x 20B = 48 MB) vi no chi
// co MAX_SUBWORLD = 1. Server co MAX_SUBWORLD = 1000 nen phai cap phat dung co luoi that.
// Vi du ban do 12x12 region = 192 x 384 = 73.728 o x 20B = 1,47 MB.
void KSubWorld::LoadPathGridSrv(const char* szPathName, int nGridW, int nGridH)
{
	if (m_bHavePath && m_GridNode)
		return;		// da nap roi
	if (!szPathName || !szPathName[0] || nGridW <= 0 || nGridH <= 0)
		return;

	strncpy(m_szPathName, szPathName, sizeof(m_szPathName) - 1);
	m_szPathName[sizeof(m_szPathName) - 1] = 0;
	m_nGridW = nGridW;
	m_nGridH = nGridH;
	m_bStopThread = FALSE;

	const int nAllCell = m_nGridW * m_nRegionWidth * m_nGridH * m_nRegionHeight;
	if (nAllCell <= 0)
		return;

	// Ten tep RIENG cho server ("_srv"). KHONG duoc dung chung "%d.fp" voi client:
	// client dong goi "%d.fp" trong maps.pak, KFile.Open co co che lui ve pak, nen server
	// hoi cung ten se AM THAM nap du lieu vat can CUA CLIENT.
	char szFile[MAX_PATH];
	sprintf(szFile, "\\maps\\%d_srv.fp", m_SubWorldID);

	// Cache ghi TOA DO THE GIOI TUYET DOI trong tung VGridNode. Nap cache cua mot ban do
	// khac goc toa do / khac co luoi thi A* VAN ra duong, nhung moi waypoint lech dung bang
	// do lech goc -> bot di "dau do khac". Nen nhet goc + kich thuoc vao dau tep va VUT BO
	// cache nao khong khop.
	const UINT kMagic  = 0x53465006;	// S F P 06 - 20/08 dem: siet luat region (_S dinh nghia region ton tai, _C chi cap du lieu) -> luoi doi, moi ban do tu tinh lai cache
	const UINT kHdrVer = 2;

	bool bLoaded = false;
	{
		KFile File;
		if (File.Open(szFile))
		{
			UINT uMagic = 0, uHdr = 0;
			File.Read(&uMagic, sizeof(UINT));
			File.Read(&uHdr,   sizeof(UINT));
			if (uMagic == kMagic && uHdr == kHdrVer)
			{
				int h[7] = {0};
				File.Read(h, sizeof(h));
				UINT uVer = 0;
				File.Read(&uVer, sizeof(UINT));
				const bool bKhop = (h[0] == m_nRegionBeginX && h[1] == m_nRegionBeginY
								  && h[2] == m_nGridW && h[3] == m_nGridH
								  && h[4] == m_nRegionWidth && h[5] == m_nRegionHeight
								  && h[6] == nAllCell && uVer == FINDPATH_VERSION);
				if (bKhop)
				{
					UINT uGridSize = 0;
					File.Read(&uGridSize, sizeof(UINT));
					if (uGridSize == (UINT)(sizeof(VGridNode) * nAllCell))
					{
						if (m_GridNode == NULL) m_GridNode = new VGridNode[nAllCell];
						m_nGridTotal = nAllCell;
						File.Read(m_GridNode, uGridSize);
						UINT uNb = 0;
						File.Read(&uNb, sizeof(UINT));
						// (19/08 phan bien) tep cache CUT GIUA CHUNG (ghi do dang/dia day) van
						// lot qua magic/header -> phan duoi m_GridNode la RAC (ctor VGridNode
						// khong gan parentId/obs). Doi chieu kich thuoc tep TONG truoc khi tin.
						const UINT uKyVong = 2 * sizeof(UINT) + sizeof(int) * 7 + sizeof(UINT)
						                   + sizeof(UINT) + uGridSize + sizeof(UINT)
						                   + uNb * (UINT)sizeof(VGridNeighbour);
						// uNb rac co the lam phep nhan UINT wrap trung Size() -> resize no bad_alloc
						if (uNb <= (UINT)nAllCell * 8u && File.Size() == uKyVong)
						{
							if (uNb > 0)
							{
								m_vNeighbour.resize(uNb);
								File.Read(&m_vNeighbour[0], sizeof(VGridNeighbour) * uNb);
							}
							m_bHavePath = TRUE;
							bLoaded = true;
						}
					}
				}
			}
		}
	}

	if (bLoaded)
	{
		PB_LogNgoai("[PathSrv] map %d: nap cache %s (%d o)\n", m_SubWorldID, szFile, m_nGridTotal);
		return;
	}

	// Chua co cache -> dung luoi tu du lieu vat can.
	if (m_GridNode == NULL)   m_GridNode   = new VGridNode[nAllCell];
	if (m_pTempCover == NULL) m_pTempCover = new int[nAllCell];
	m_nGridTotal = nAllCell;
	m_vNeighbour.clear();

	ProcLoadPathGrid();	// dong bo, KHONG dung luong nhu client

	// Kiem tinh tao: luoi ma KHONG co o nao bi chan la dau hieu du lieu vat can khong doc
	// duoc (sai ten tep / thieu tep). Bao ro thay vi de A* ve duong xuyen tuong.
	{
		int nObs = 0;
		for (int i = 0; i < m_nGridTotal; i++)
			if (m_GridNode[i].obs) nObs++;
		if (nObs == 0)
		{
			PB_LogNgoai("[PathSrv] CANH BAO map %d: luoi KHONG co vat can nao (%d o). "
					   "Kiem tep %%03d_Region_S.dat trong %s - thieu thi A* se ve duong xuyen tuong.\n",
					   m_SubWorldID, m_nGridTotal, m_szPathName);
		}
		else
		{
			PB_LogNgoai("[PathSrv] map %d: dung luoi %d o, %d o bi chan, %d canh\n",
					   m_SubWorldID, m_nGridTotal, nObs, (int)m_vNeighbour.size());
		}
	}
	m_bHavePath = TRUE;

	// Ghi cache cho lan khoi dong sau. (19/08) GHI DE luon: chi toi duoc day khi
	// cache VANG hoac DA BI VUT (lech magic/header). Guard "khong ghi de" cu lam
	// moi lan nang kMagic la cac map giu cache cu tu dung lai luoi MOI LAN BOOT
	// ma cache moi khong bao gio duoc ghi (da xay ra voi 77 map tu 18/08).
	{
		{
			g_CreatePath("\\maps");
			g_SetFilePath("\\maps");
			KFile WFile;
			sprintf(szFile, "%d_srv.fp", m_SubWorldID);
			if (WFile.Create(szFile))
			{
				UINT uMagic2 = kMagic, uHdr2 = kHdrVer;
				WFile.Write(&uMagic2, sizeof(UINT));
				WFile.Write(&uHdr2,   sizeof(UINT));
				int h[7] = { m_nRegionBeginX, m_nRegionBeginY, m_nGridW, m_nGridH,
							 m_nRegionWidth, m_nRegionHeight, nAllCell };
				WFile.Write(h, sizeof(h));
				UINT uVer2 = FINDPATH_VERSION;
				WFile.Write(&uVer2, sizeof(UINT));
				UINT uGridSize = (UINT)(sizeof(VGridNode) * nAllCell);
				WFile.Write(&uGridSize, sizeof(UINT));
				WFile.Write(m_GridNode, uGridSize);
				UINT uNb = (UINT)m_vNeighbour.size();
				WFile.Write(&uNb, sizeof(UINT));
				if (uNb > 0)
					WFile.Write(&m_vNeighbour[0], sizeof(VGridNeighbour) * uNb);
			}
		}
	}
}

// Tim duong cho bot. KHONG giu trang thai: ket qua chep ra vOutPath roi xoa sach,
// nen nhieu bot goi lien tiep khong dam nhau. CHI an toan tren MOT luong (vong lap game)
// vi dung chung bo nho nhap m_aGCost/m_aCameFrom/m_aClosed.
// Tra: 1 = duong day du, 2 = duong mot phan, 0 = khong co duong, -1 = dau vao khong hop le.
int KSubWorld::FindPathServer(int nStartMpsX, int nStartMpsY, int nDestMpsX, int nDestMpsY,
						   std::vector<int>& vOutPath, bool bCheckNpc)
{
	vOutPath.clear();
	// Ma loi RIENG cho tung duong som - de bot.log ke duoc vi sao khong co duong:
	//   -2 luoi chua nap  .  -3 diem DICH ket trong vat can  .  -4 diem XUAT PHAT ket
	//   -1 dau vao sai    .   0 A* chiu thua that su
	if (!m_bHavePath || m_GridNode == NULL || m_nGridTotal <= 0)	return -2;
	if (nStartMpsX <= 0 || nStartMpsY <= 0 || nDestMpsX <= 0 || nDestMpsY <= 0) return -1;

	const int nCellW = m_nGridW * m_nRegionWidth;
	const int nCellH = m_nGridH * m_nRegionHeight;
	const int nTot   = nCellW * nCellH;

	// dich
	int tcx = nDestMpsX / 32 - m_nRegionBeginX * REGION_GRID_WIDTH;
	int tcy = nDestMpsY / 32 - m_nRegionBeginY * REGION_GRID_HEIGHT;
	if (tcx < 0) tcx = 0; else if (tcx >= nCellW) tcx = nCellW - 1;
	if (tcy < 0) tcy = 0; else if (tcy >= nCellH) tcy = nCellH - 1;
	int tid = tcy * nCellW + tcx;
	if (tid < 0 || tid >= nTot) return -1;
	tid = m_GridNode[tid].parentId;
	if (tid < 0 || tid >= nTot) return -1;	// (19/08) parentId rac tu cache hong
	if (m_GridNode[tid].obs)
	{
		// Dich roi dung vao vat can (vd sat hang rao): khong bam lai thi A* chi tra duong
		// MOT PHAN dung o o gan nhat, roi chang cuoi ban thang vao goc -> ket goc.
		tid = FindFreeBlockAround(tid, nDestMpsX, nDestMpsY);
		if (tid < 0) return -3;
	}

	// xuat phat (doi xung voi dich)
	int scx = nStartMpsX / 32 - m_nRegionBeginX * REGION_GRID_WIDTH;
	int scy = nStartMpsY / 32 - m_nRegionBeginY * REGION_GRID_HEIGHT;
	if (scx < 0) scx = 0; else if (scx >= nCellW) scx = nCellW - 1;
	if (scy < 0) scy = 0; else if (scy >= nCellH) scy = nCellH - 1;
	int sid = scy * nCellW + scx;
	if (sid < 0 || sid >= nTot) return -1;
	sid = m_GridNode[sid].parentId;
	if (sid < 0 || sid >= nTot) return -1;	// (19/08) parentId rac tu cache hong
	if (m_GridNode[sid].obs)
	{
		sid = FindFreeBlockAround(sid, nStartMpsX, nStartMpsY);
		if (sid < 0) return -4;
	}

	if (sid == tid)			// cung mot block, khong can tim duong
	{
		vOutPath.push_back(tid);
		return 1;
	}

	// Gan thi tinh ca NPC lam vat can (JX1 tinh NPC LA TUONG khi buoc sang o moi);
	// xa thi bo qua NPC cho re, vi NPC se tu tan di truoc khi bot toi noi.
	int nRet;
	const bool bNear = (g_GetDistance(nStartMpsX, nStartMpsY, nDestMpsX, nDestMpsY) < 512);
	if (bCheckNpc && bNear)
		nRet = FindPath_NpcObs(sid, tid);
	else
		nRet = FindPath_Block(sid, tid);

	if (nRet > 0 && !m_vRetPath.empty())
		vOutPath = m_vRetPath;
	m_vRetPath.clear();
	return nRet;
}

// (19/08 chieu) Tra loi "o luoi tai MPS nay co phai vat can khong" cho he bot.
// KHONG kep toa do nhu FindPathServer (kep se doi cau hoi thanh o KHAC): ngoai
// luoi -> -1 "chua biet". Doc obs cua CHINH O, khong qua parentId (block gop chi
// gop o cung tinh trang nhung o goc thi cu doc thang cho chac - FindFreeBlockAround
// cung doc kieu nay truoc khi lay parent).
int KSubWorld::CellObsSrv(int nMpsX, int nMpsY)
{
	if (!m_bHavePath || m_GridNode == NULL || m_nGridTotal <= 0)
		return -1;
	if (nMpsX <= 0 || nMpsY <= 0)
		return -1;
	const int nCellW = m_nGridW * m_nRegionWidth;
	const int nCellH = m_nGridH * m_nRegionHeight;
	const int cx = nMpsX / 32 - m_nRegionBeginX * REGION_GRID_WIDTH;
	const int cy = nMpsY / 32 - m_nRegionBeginY * REGION_GRID_HEIGHT;
	if (cx < 0 || cx >= nCellW || cy < 0 || cy >= nCellH)
		return -1;
	const int id = cy * nCellW + cx;
	if (id < 0 || id >= m_nGridTotal || id >= nCellW * nCellH)
		return -1;
	return m_GridNode[id].obs ? 1 : 0;
}

#endif	// _SERVER (A* phia server)
