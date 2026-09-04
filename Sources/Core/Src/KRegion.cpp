#include "KCore.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KMissleSet.h"
#include "KObjSet.h"
#include "KPlayerSet.h"
#include "KPlayer.h"
#include "KMissle.h"
#include "KObj.h"
#include "KWorldMsg.h"
#include "KSubWorld.h"
#ifdef _SERVER
//#include "KNetServer.h"
//#include "../MultiServer/Heaven/Interface/iServer.h"
#endif
#include "Scene/ObstacleDef.h"
#include "Scene/SceneDataDef.h"
#include "KRegion.h"
#include "MyAssert.H"
#include <chrono>

#define BC_MAX_VUNG 8192
int g_anBCVungGoi[BC_MAX_VUNG];		// [BC 04/09 do] so luot BroadCast phat VAO tung vung
int g_anBCVungActive[BC_MAX_VUNG];	// so lan vung do duoc Activate (= so tick trong 10 giay)
int g_nBCNormalSync = 0;			// so lan KNpc::NormalSync (dat trong KNpc.cpp)
int g_nBCTick = 0;					// [BC 04/09 do2] nhip tick that (KSubWorldSet::Activate)
int g_nBCTheoDoi = 0;				// chi so Player dang theo doi (client nhan nhieu nhat 10 giay truoc)
static unsigned char g_abyBCVungGui[BC_MAX_VUNG];	// vung nao da gui cho client do (1 byte/vung)

// (20/08 dem - chu game: "thu lam phan nguoi choi hay bot khong phai vat can
// co the dung chong len nhau xem con bi khong") CONG TAC 'NPC LA TUONG':
//   0 = nguoi choi/bot/quai KHONG chan duong nhau (dung chong len nhau duoc) -
//       moi diem DOC m_pNpcRef phia SERVER deu bo qua; bo dem AddRef/DecRef
//       van chay du nen bat lai la ve nguyen trang, khong dong 20 cap cong/tru.
//   1 = ban goc: NPC dung o nao thi o do la vat can song (Obstacle_JumpFly).
// Doi luc chay bang lenh GM Lua PB_SetNpcChan(1/0/-1) - khong can restart.
int g_nPbNpcChan = 0;
KRegion::KRegion()
{
	m_nIndex		= -1;
	m_RegionID		= -1;
	m_nActive		= 0;
	m_nNpcSyncCounter = 0;
	m_nBroadCastCursor = 0;
	m_nObjSyncCounter = 0;
	m_nNpcSyncCursor  = 0;
	m_nWidth		= 0;
	m_nHeight		= 0;
	ZeroMemory(m_nConnectRegion, 8 * sizeof(int));
	memset(m_nConRegionID, -1, sizeof(m_nConRegionID));
	m_pNpcRef		= NULL;
	m_pObjRef		= NULL;
	m_pObstacleRef	= NULL;
	m_pMslRef		= NULL;
	m_nRegionX		= 0;
	m_nRegionY		= 0;
#ifdef _SERVER
	memset(m_dwTrap, 0, sizeof(m_dwTrap));
	// [PORT5 23/08] tham so trap JX2: mac dinh NONE (trap JX1)
	for (int _tp = 0; _tp < REGION_GRID_WIDTH * REGION_GRID_HEIGHT; _tp++)
		((int*)m_nTrapParam)[_tp] = JX2TRAP_PARAM_NONE;
#endif
}

KRegion::~KRegion()
{
	if (m_pMslRef)
		delete [] m_pMslRef;
	if (m_pNpcRef)
		delete [] m_pNpcRef;
	if (m_pObjRef)
		delete [] m_pObjRef;
	if(m_pObstacleRef)
		delete [] m_pObstacleRef;
}

BOOL KRegion::Init(int nWidth, int nHeight)
{
	m_nWidth	= nWidth;
	m_nHeight	= nHeight;

	if (!m_pNpcRef)
		m_pNpcRef = new BYTE[nWidth * nHeight];
	if (!m_pNpcRef)
		return FALSE;
	ZeroMemory(m_pNpcRef, nWidth * nHeight);

	if (!m_pObjRef)
		m_pObjRef = new BYTE[nWidth * nHeight];
	if (!m_pObjRef)
		return FALSE;
	ZeroMemory(m_pObjRef, nWidth * nHeight);

	if (!m_pObstacleRef)
		m_pObstacleRef = new BYTE[nWidth * nHeight];
	if (!m_pObstacleRef)
		return FALSE;
	ZeroMemory(m_pObstacleRef, nWidth * nHeight);

	if (!m_pMslRef)
		m_pMslRef = new BYTE[nWidth * nHeight];
	if (!m_pMslRef)
		return FALSE;
	ZeroMemory(m_pMslRef, nWidth * nHeight);
	
	return TRUE;
}

void KRegion::ClearRefGrid()
{
    if (m_pNpcRef)
        ZeroMemory(m_pNpcRef, m_nWidth * m_nHeight);

    if (m_pMslRef)
        ZeroMemory(m_pMslRef, m_nWidth * m_nHeight);
}

BOOL KRegion::Load(int nX, int nY)
{
#ifndef _SERVER
	Close();
#endif

	m_RegionID = MAKELONG(nX, nY);

	m_nRegionX = nX * 512;
	m_nRegionY = nY * 1024;

	// ÏÂ·½
	m_nConRegionID[0] = MAKELONG(nX, nY + 1);
	// ×óÏÂ·½
	m_nConRegionID[1] = MAKELONG(nX - 1, nY + 1);
	// ×ó·½
	m_nConRegionID[2] = MAKELONG(nX - 1, nY);
	// ×óÉÏ·½
	m_nConRegionID[3] = MAKELONG(nX - 1, nY - 1);
	// ÉÏ·½
	m_nConRegionID[4] = MAKELONG(nX, nY - 1);
	// ÓÒÉÏ·½
	m_nConRegionID[5] = MAKELONG(nX + 1, nY - 1);
	// ÓÒ·½
	m_nConRegionID[6] = MAKELONG(nX + 1, nY);
	// ÓÒÏÂ·½
	m_nConRegionID[7] = MAKELONG(nX + 1, nY + 1);

	return TRUE;
}

#ifdef _SERVER
//----------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë·þÎñÆ÷¶ËµØÍ¼ÉÏ±¾region µÄ objectÊý¾Ý£¨°üÀ¨npc¡¢trap¡¢boxµÈ£©
//	Load maps VNG loadobject edit by phong kieu
//	×¢Òâ£ºÊ¹ÓÃ´Ëº¯ÊýÖ®Ç°±ØÐë±£Ö¤µ±Ç°Â·¾¶ÊÇ±¾µØÍ¼Â·¾¶
//----------------------------------------------------------------------
BOOL KRegion::LoadObject(int nSubWorld, int nX, int nY)
{
	KPakFile	cData;
	char		szFilePath[1024];
	char		szFile[1024];
	//
	g_GetFilePath(szFile);
	//
	sprintf(szFilePath, "\\%sv_%03d", szFile, nY);
	//g_SetFilePath(szFilePath);
	sprintf(szFile, "%s\\%03d_%s", szFilePath, nX, REGION_COMBIN_FILE_NAME_SERVER);
	if (cData.Open(szFile))
	{
		DWORD	dwHeadSize;
		DWORD	dwMaxElemFile = 0;
		KCombinFileSection	sElemFile[REGION_ELEM_FILE_COUNT];

		if (cData.Size() < sizeof(DWORD) + sizeof(KCombinFileSection) * REGION_ELEM_FILE_COUNT)
		{
			ZeroMemory(m_Obstacle, sizeof(m_Obstacle));
			goto gotoCLOSE;
		}
		cData.Read(&dwMaxElemFile, sizeof(DWORD));
		if (dwMaxElemFile > REGION_ELEM_FILE_COUNT)
		{
			cData.Read(sElemFile, sizeof(sElemFile));
			cData.Seek(sizeof(KCombinFileSection) * (dwMaxElemFile - REGION_ELEM_FILE_COUNT), FILE_CURRENT);
		}
		else
		{
			cData.Read(sElemFile, sizeof(sElemFile));
		}
		dwHeadSize = sizeof(DWORD) + sizeof(KCombinFileSection) * dwMaxElemFile;

		// VËt c¶n
		cData.Seek(dwHeadSize + sElemFile[REGION_OBSTACLE_FILE_INDEX].uOffset, FILE_BEGIN);
		LoadServerObstacle(&cData, sElemFile[REGION_OBSTACLE_FILE_INDEX].uLength);

		// Trap
		cData.Seek(dwHeadSize + sElemFile[REGION_TRAP_FILE_INDEX].uOffset, FILE_BEGIN);
		LoadServerTrap(&cData, sElemFile[REGION_TRAP_FILE_INDEX].uLength);

		// Npc vµ qu¸i
		cData.Seek(dwHeadSize + sElemFile[REGION_NPC_FILE_INDEX].uOffset, FILE_BEGIN);
		LoadServerNpc(nSubWorld, &cData, sElemFile[REGION_NPC_FILE_INDEX].uLength);
		
		// Object
	//	cData.Seek(dwHeadSize + sElemFile[REGION_OBJ_FILE_INDEX].uOffset, FILE_BEGIN);
	//	LoadServerObj(nSubWorld, &cData, sElemFile[REGION_OBJ_FILE_INDEX].uLength);

gotoCLOSE:
		cData.Close();
	}
	else
	{
		KPakFile	cObstacle;
		//g_SetFilePath(szFilePath);
		sprintf(szFile, "%s\\%03d_%s", szFilePath, nX, REGION_OBSTACLE_FILE);
		if (cObstacle.Open(szFile))
		{
			if (cObstacle.Size() != sizeof(m_Obstacle))
				ZeroMemory(m_Obstacle, sizeof(m_Obstacle));
			else
				cObstacle.Read((LPVOID)m_Obstacle, sizeof(m_Obstacle));
			cObstacle.Close();
		}
		else
		{
			ZeroMemory(m_Obstacle, sizeof(m_Obstacle));
		}
		KPakFile	cTrapData, cNpcData;
		// 
		KTrapFileHead	sTrapFileHead;
		KSPTrap			sTrapCell;
		int				i, j;
//		g_SetFilePath(szFilePath);
		sprintf(szFile, "%s\\%03d_%s", szFilePath, nX, REGION_TRAP_FILE);
		memset(this->m_dwTrap, 0, sizeof(m_dwTrap));
		// [PORT5 23/08] tham so trap JX2: mac dinh NONE (trap JX1)
		for (int _tp = 0; _tp < REGION_GRID_WIDTH * REGION_GRID_HEIGHT; _tp++)
			((int*)m_nTrapParam)[_tp] = JX2TRAP_PARAM_NONE;
		if (!cTrapData.Open(szFile))
			goto TRAP_CLOSE;
		if (cTrapData.Size() < sizeof(KTrapFileHead))
			goto TRAP_CLOSE;
		cTrapData.Read(&sTrapFileHead, sizeof(KTrapFileHead));
		if (sTrapFileHead.uNumTrap * sizeof(KSPTrap) + sizeof(KTrapFileHead) != cTrapData.Size())
			goto TRAP_CLOSE;
		for (i = 0; i < sTrapFileHead.uNumTrap; i++)
		{
			cTrapData.Read(&sTrapCell, sizeof(KSPTrap));
			if (sTrapCell.cY >= REGION_GRID_HEIGHT || sTrapCell.cX + sTrapCell.cNumCell - 1 >= REGION_GRID_WIDTH)
				continue;
			for (j = 0; j < sTrapCell.cNumCell; j++)
			{
				m_dwTrap[sTrapCell.cX + j][sTrapCell.cY] = sTrapCell.uTrapId;
			}
		}
TRAP_CLOSE:
		cTrapData.Close();
		//
		KNpcFileHead	sNpcFileHead;
		KSPNpc			sNpcCell;
		//g_SetFilePath(szFilePath);
		sprintf(szFile, "%s\\%03d_%s", szFilePath, nX, REGION_NPC_FILE_SERVER);
		if (!cNpcData.Open(szFile))
			goto NPC_CLOSE;
		if (cNpcData.Size() < sizeof(KNpcFileHead))
			goto NPC_CLOSE;
		cNpcData.Read(&sNpcFileHead, sizeof(KNpcFileHead));
		for (i = 0; i < sNpcFileHead.uNumNpc; i++)
		{
			cNpcData.Read(&sNpcCell, sizeof(KSPNpc) - sizeof(sNpcCell.szScript));
			if (sNpcCell.nScriptNameLen < sizeof(sNpcCell.szScript))
			{
				cNpcData.Read(sNpcCell.szScript, sNpcCell.nScriptNameLen);
				sNpcCell.szScript[sNpcCell.nScriptNameLen] = 0;
			}
			else
			{
				sNpcCell.szScript[0] = 0;
			}
			//
			if(g_NotAddNpcNormal) //Kh«ng add npc kind normal trong folder maps
			{
				if(sNpcCell.shKind != kind_dialoger)	//ChØ load c¸c npc ®èi tho¹i
				{
					NpcSet.AddNpcSet3(nSubWorld, &sNpcCell);	//Hµm add npc ë server load ë trong folder kh«ng pak
				}
			}
			else
			{
				NpcSet.AddNpcSet3(nSubWorld, &sNpcCell);	//Hµm add npc ë server load ë trong folder kh«ng pak
			}
		}
		g_DebugLog("[TEST]Region%x have %d npc", m_RegionID, sNpcFileHead.uNumNpc);
NPC_CLOSE:
		cNpcData.Close();
		// 
		ObjSet.ServerLoadRegionObj(szFilePath, nX, nY, nSubWorld);
	}

	return TRUE;
}
#endif

#ifndef _SERVER
//----------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë¿Í»§¶ËµØÍ¼ÉÏ±¾region µÄ objectÊý¾Ý£¨°üÀ¨npc¡¢boxµÈ£©
//	Èç¹û bLoadNpcFlag == TRUE ÐèÒªÔØÈë clientonly npc else ²»ÔØÈë
//----------------------------------------------------------------------
BOOL KRegion::LoadObject(int nSubWorld, int nX, int nY, char *lpszPath)
{
#ifdef TOOLVERSION
	return TRUE;
#endif
/*	char	szPath[FILE_NAME_LENGTH], szFile[FILE_NAME_LENGTH];

	if (!lpszPath || !lpszPath[0] || strlen(lpszPath) >= FILE_NAME_LENGTH)
		return FALSE;
	sprintf(szPath, "%s\\v_%03d", lpszPath, nY);
//	g_SetFilePath(szPath);

	// 
	NpcSet.InsertNpcToRegion(this->m_nIndex);

	KPakFile	cData;
	sprintf(szFile, "%s\\%03d_%s", szPath, nX, REGION_COMBIN_FILE_NAME_CLIENT);
	if (cData.Open(szFile))
	{
		DWORD	dwHeadSize;
		DWORD	dwMaxElemFile = 0;
		KCombinFileSection	sElemFile[REGION_ELEM_FILE_COUNT];

		if (cData.Size() < sizeof(DWORD) + sizeof(KCombinFileSection) * REGION_ELEM_FILE_COUNT)
			goto gotoCLOSE;
		cData.Read(&dwMaxElemFile, sizeof(DWORD));
		if (dwMaxElemFile > REGION_ELEM_FILE_COUNT)
		{
			cData.Read(sElemFile, sizeof(sElemFile));
			cData.Seek(sizeof(KCombinFileSection) * (dwMaxElemFile - REGION_ELEM_FILE_COUNT), FILE_CURRENT);
		}
		else
		{
			cData.Read(sElemFile, sizeof(sElemFile));
		}
		dwHeadSize = sizeof(DWORD) + sizeof(KCombinFileSection) * dwMaxElemFile;

		// ÔØÈënpcÊý¾Ý
		cData.Seek(dwHeadSize + sElemFile[REGION_NPC_FILE_INDEX].uOffset, FILE_BEGIN);
		LoadClientNpc(&cData, sElemFile[REGION_NPC_FILE_INDEX].uLength);

		// ÔØÈëobjÊý¾Ý
		cData.Seek(dwHeadSize + sElemFile[REGION_OBJ_FILE_INDEX].uOffset, FILE_BEGIN);
		LoadClientObj(&cData, sElemFile[REGION_OBJ_FILE_INDEX].uLength);

gotoCLOSE:
		cData.Close();
	}
	else
	{
		// ÔØÈë Client npc Êý¾Ý
		KPakFile		cNpcData;
		KNpcFileHead	sNpcFileHead;
		KSPNpc			sNpcCell;
		DWORD			i;
		KClientNpcID	sTempID;
		int				nNpcNo;

		// ÔØÈëµØÍ¼ÎÄ¼þÀïµÄ client npc
//		g_SetFilePath(szPath);
		sprintf(szFile, "%s\\%03d_%s", szPath, nX, REGION_NPC_FILE_CLIENT);
		if (!cNpcData.Open(szFile))
			goto NPC_CLOSE;
		if (cNpcData.Size() < sizeof(KNpcFileHead))
			goto NPC_CLOSE;
		cNpcData.Read(&sNpcFileHead, sizeof(KNpcFileHead));
		for (i = 0; i < sNpcFileHead.uNumNpc; i++)
		{
			cNpcData.Read(&sNpcCell, sizeof(KSPNpc) - sizeof(sNpcCell.szScript));
			_ASSERT(sNpcCell.nScriptNameLen < sizeof(sNpcCell.szScript));
			if (sNpcCell.nScriptNameLen > 0)
			{
				cNpcData.Read(sNpcCell.szScript, sNpcCell.nScriptNameLen);
				sNpcCell.szScript[sNpcCell.nScriptNameLen] = 0;
			}
			else
			{
				sNpcCell.szScript[0] = 0;
			}
			sTempID.m_dwRegionID = MAKELONG(nX, nY);
			sTempID.m_nNo = i;
			nNpcNo = NpcSet.SearchClientID(sTempID);
			if (nNpcNo == 0)
			{
				int nIdx = NpcSet.AddClientNpc(sNpcCell.nTemplateID, nX, nY, sNpcCell.nPositionX, sNpcCell.nPositionY, i);
				if (nIdx > 0)
				{
					Npc[nIdx].m_Kind = sNpcCell.shKind;
					Npc[nIdx].SendCommand(do_stand);
					Npc[nIdx].m_Dir = Npc[nIdx].GetNormalNpcStandDir(sNpcCell.nCurFrame);
				}
			}
		}

NPC_CLOSE:
		cNpcData.Close();

		// 
		ObjSet.ClientLoadRegionObj(szPath, nX, nY, nSubWorld, this->m_nIndex);
	}*/

	return TRUE;
}
#endif

#ifdef _SERVER
//----------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë·þÎñÆ÷¶ËµØÍ¼ÉÏ±¾ region µÄÕÏ°­Êý¾Ý
//----------------------------------------------------------------------
BOOL	KRegion::LoadServerObstacle(KPakFile *pFile, DWORD dwDataSize)
{
	if (!pFile || dwDataSize != sizeof(this->m_Obstacle))
	{
		memset(m_Obstacle, 0, sizeof(m_Obstacle));
		return FALSE;
	}
	pFile->Read((LPVOID)m_Obstacle, sizeof(m_Obstacle));

	return TRUE;
}
#endif

#ifdef _SERVER
//----------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë·þÎñÆ÷¶ËµØÍ¼ÉÏ±¾ region µÄ trap Êý¾Ý
//----------------------------------------------------------------------
BOOL	KRegion::LoadServerTrap(KPakFile *pFile, DWORD dwDataSize)
{
	memset(m_dwTrap, 0, sizeof(m_dwTrap));
	// [PORT5 23/08] tham so trap JX2: mac dinh NONE (trap JX1)
	for (int _tp = 0; _tp < REGION_GRID_WIDTH * REGION_GRID_HEIGHT; _tp++)
		((int*)m_nTrapParam)[_tp] = JX2TRAP_PARAM_NONE;
	if (!pFile || dwDataSize < sizeof(KTrapFileHead))
		return FALSE;

	KTrapFileHead	sTrapFileHead;
	KSPTrap			sTrapCell;
	int				i, j;

	pFile->Read(&sTrapFileHead, sizeof(KTrapFileHead));
	if (sTrapFileHead.uNumTrap * sizeof(KSPTrap) + sizeof(KTrapFileHead) != dwDataSize)
		return FALSE;
	for (i = 0; i < sTrapFileHead.uNumTrap; i++)
	{
		pFile->Read(&sTrapCell, sizeof(KSPTrap));
		if (sTrapCell.cY >= REGION_GRID_HEIGHT || sTrapCell.cX + sTrapCell.cNumCell - 1 >= REGION_GRID_WIDTH)
			continue;
		for (j = 0; j < sTrapCell.cNumCell; j++)
		{
			m_dwTrap[sTrapCell.cX + j][sTrapCell.cY] = sTrapCell.uTrapId;
		}
	}

	return TRUE;
}
#endif

#ifdef _SERVER
BOOL	KRegion::LoadServerNpc(int nSubWorld, KPakFile *pFile, DWORD dwDataSize)
{
	if (!pFile || dwDataSize < sizeof(KNpcFileHead))
		return FALSE;

	KNpcFileHead	sNpcFileHead;
	KSPNpc				sNpcCell;
	//
	pFile->Read(&sNpcFileHead, sizeof(KNpcFileHead));
	//
	for (int i = 0; i < sNpcFileHead.uNumNpc; i++)
	{
		pFile->Read(&sNpcCell, sizeof(KSPNpc) - sizeof(sNpcCell.szScript));
		if (sNpcCell.nScriptNameLen < sizeof(sNpcCell.szScript))
		{
			pFile->Read(sNpcCell.szScript, sNpcCell.nScriptNameLen);
			char* gRenameNpc = sNpcCell.szName;
			RenameNpc(gRenameNpc);
			//strcpy(sNpcCell.szName, gRenameNpc);
			sNpcCell.szScript[sNpcCell.nScriptNameLen] = 0;
		}
		else
		{
			sNpcCell.szScript[0] = 0;
			pFile->Seek(sNpcCell.nScriptNameLen, FILE_CURRENT);
		}
		//
		if(g_NotAddNpcNormal) //Kh«ng add npc kind normal trong folder maps
		{
			if(sNpcCell.shKind != kind_dialoger)	//ChØ load c¸c npc ®èi tho¹i
			{
				NpcSet.AddNpcSet3(nSubWorld, &sNpcCell);	//Hµm add npc ë server load ë trong file pak
			}
		}
		else
		{
			NpcSet.AddNpcSet3(nSubWorld, &sNpcCell);	//Hµm add npc ë server load ë trong file pak
		}
	}
	return TRUE;
}

KTabFile g_RenameNpcTabSetting;

void KRegion::RenameNpc(char* &sNpcCell)
{
	int row123 = g_RenameNpcTabSetting.FindRow(sNpcCell);
	g_RenameNpcTabSetting.GetString(row123, "targetname", "Npc=Name", sNpcCell, 32);//get dong dau tien file npcname.txt
	return;
}
#endif

#ifdef _SERVER
BOOL	KRegion::LoadServerObj(int nSubWorld, KPakFile *pFile, DWORD dwDataSize)
{
	return ObjSet.ServerLoadRegionObj(nSubWorld, pFile, dwDataSize);
}
#endif

#ifndef _SERVER
BOOL	KRegion::LoadClientNpc(KPakFile *pFile, DWORD dwDataSize)
{
	if (!pFile || dwDataSize < sizeof(KNpcFileHead))
		return FALSE;

	KNpcFileHead	sNpcFileHead;
	KSPNpc				sNpcCell;
	DWORD			i;
	KClientNpcID	sTempID;
	int						nNpcNo;

	pFile->Read(&sNpcFileHead, sizeof(KNpcFileHead));
	for (i = 0; i < sNpcFileHead.uNumNpc; i++)
	{
		pFile->Read(&sNpcCell, sizeof(KSPNpc) - sizeof(sNpcCell.szScript));
		if (sNpcCell.nScriptNameLen < sizeof(sNpcCell.szScript))
		{
			pFile->Read(sNpcCell.szScript, sNpcCell.nScriptNameLen);
			sNpcCell.szScript[sNpcCell.nScriptNameLen] = 0;
		}
		else
		{
			sNpcCell.szScript[0] = 0;
			pFile->Seek(sNpcCell.nScriptNameLen, FILE_CURRENT);
		}
		sTempID.m_dwRegionID = m_RegionID;
		sTempID.m_nNo = i;
		nNpcNo = NpcSet.SearchClientID(sTempID);
		if (nNpcNo == 0)
		{
			int nIdx = NpcSet.AddClientNpc(sNpcCell.nTemplateID, LOWORD(m_RegionID), HIWORD(m_RegionID), sNpcCell.nPositionX, sNpcCell.nPositionY, i);
			if (nIdx > 0)
			{
				Npc[nIdx].m_Kind = sNpcCell.shKind;
				Npc[nIdx].SendCommand(do_stand);
				Npc[nIdx].m_Dir = Npc[nIdx].GetNormalNpcStandDir(sNpcCell.nCurFrame);
			}
		}
	}

	return TRUE;
}
#endif

#ifndef _SERVER
//----------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë¿Í»§¶ËµØÍ¼ÉÏ±¾ region µÄ clientonlyobj Êý¾Ý
//----------------------------------------------------------------------
BOOL	KRegion::LoadClientObj(KPakFile *pFile, DWORD dwDataSize)
{
	return ObjSet.ClientLoadRegionObj(pFile, dwDataSize);
}
#endif

#ifndef _SERVER
//----------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈëÕÏ°­Êý¾Ý¸øÐ¡µØÍ¼
//----------------------------------------------------------------------
void	KRegion::LoadLittleMapData(int nX, int nY, char *lpszPath, BYTE *lpbtObstacle)
{
	if (!lpbtObstacle)
		return;

	char	szPath[FILE_NAME_LENGTH], szFile[FILE_NAME_LENGTH];
	int		i, j;

	if (!lpszPath || !lpszPath[0] || strlen(lpszPath) >= FILE_NAME_LENGTH)
		return ;
	sprintf(szPath, "%s\\v_%03d", lpszPath, nY);

	KPakFile	cData;
	long		nTempTable[REGION_GRID_WIDTH][REGION_GRID_HEIGHT];	// µØÍ¼ÕÏ°­ÐÅÏ¢±í

//	g_SetFilePath(szPath);
	sprintf(szFile, "%s\\%03d_%s", szPath, nX, REGION_COMBIN_FILE_NAME_CLIENT);
	if (cData.Open(szFile))
	{
		DWORD	dwHeadSize;
		DWORD	dwMaxElemFile = 0;
		KCombinFileSection	sElemFile[REGION_ELEM_FILE_COUNT];

		if (cData.Size() < sizeof(DWORD) + sizeof(KCombinFileSection) * REGION_ELEM_FILE_COUNT)
		{
			ZeroMemory(nTempTable, sizeof(nTempTable));
		}
		else
		{
			cData.Read(&dwMaxElemFile, sizeof(DWORD));
			if (dwMaxElemFile > REGION_ELEM_FILE_COUNT)
			{
				cData.Read(sElemFile, sizeof(sElemFile));
				cData.Seek(sizeof(KCombinFileSection) * (dwMaxElemFile - REGION_ELEM_FILE_COUNT), FILE_CURRENT);
			}
			else
			{
				cData.Read(sElemFile, sizeof(sElemFile));
			}

			if (sElemFile[REGION_OBSTACLE_FILE_INDEX].uLength == sizeof(nTempTable))
			{
				dwHeadSize = sizeof(DWORD) + sizeof(KCombinFileSection) * dwMaxElemFile;
				cData.Seek(dwHeadSize + sElemFile[REGION_OBSTACLE_FILE_INDEX].uOffset, FILE_BEGIN);
				cData.Read(nTempTable, sizeof(nTempTable));
			}
			else
			{
				ZeroMemory(nTempTable, sizeof(nTempTable));
			}
		}

		cData.Close();
	}
	else
	{
		sprintf(szFile, "%03d_%s", nX, REGION_OBSTACLE_FILE);
		if (cData.Open(szFile))
			cData.Read((LPVOID)nTempTable, sizeof(nTempTable));
		else
			ZeroMemory(nTempTable, sizeof(nTempTable));
		cData.Close();
	}
	
	for (i = 0; i < REGION_GRID_HEIGHT; i++)
	{
		for (j = 0; j < REGION_GRID_WIDTH; j++)
		{
			lpbtObstacle[i * REGION_GRID_WIDTH + j] = (BYTE)nTempTable[j][i];
		}
	}
}
#endif
#ifdef _SERVER 
int KRegion::DelAllNpc(int mSubWorldID, char* szName)
{
	KIndexNode *pNode = NULL;
	KIndexNode *pTmpNode = NULL;
	int	nCounter = 0;
	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		pTmpNode = (KIndexNode *)pNode->GetNext();

		int nNpcIdx = pNode->m_nIndex;
		if (nNpcIdx>0 && nNpcIdx<MAX_NPC && Npc[nNpcIdx].m_Kind!=kind_player && Npc[nNpcIdx].m_Doing != do_revive)
		{
			if (szName == NULL || (szName != NULL && strcmp(szName, Npc[nNpcIdx].Name) == 0)) {
				if (Npc[nNpcIdx].m_RegionIndex >= 0 && mSubWorldID > 0 && Npc[nNpcIdx].m_NpcSettingIdx == mSubWorldID)
				{
					SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_Region[Npc[nNpcIdx].m_RegionIndex].RemoveNpc(nNpcIdx);
					SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_Region[Npc[nNpcIdx].m_RegionIndex].DecRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, obj_npc);
				}
				else
				{
					if (Npc[nNpcIdx].m_RegionIndex >= 0)
					{
						SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_Region[Npc[nNpcIdx].m_RegionIndex].RemoveNpc(nNpcIdx);
						SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_Region[Npc[nNpcIdx].m_RegionIndex].DecRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, obj_npc);
					}
				}

				NpcSet.Remove(nNpcIdx);
				nCounter++;
			}
		}

		pNode = pTmpNode;

	}
  return nCounter;
}
#endif
void KRegion::Activate()
{
	if (m_nIndex >= 0 && m_nIndex < BC_MAX_VUNG)	// [BC 04/09 do] dem so lan vung nay chay mot tick
		g_anBCVungActive[m_nIndex]++;
    KIndexNode *pNode = NULL;
    KIndexNode *pTmpNode = NULL;
	const int kNpcSyncChunkSize = 5;  // Number of NPCs to sync per frame
	int npcCount = m_NpcList.GetNodeCount();
#ifdef _SERVER
	extern int nActiveNpcCount;	// [PerfLog 24/08] khoi luong tick (KSubWorldSet.cpp)
	nActiveNpcCount += npcCount;
#endif

    int nCounter = 0;

    pNode = (KIndexNode *)m_NpcList.GetHead();
	int currentIndex = 0;
	int synced = 0;
#ifdef _SERVER
	// [24/08] Tinh san 5 chi so can dong bo. Truoc day phep '%' (idiv, ~20-40
	// chu ky, khong pipeline duoc) chay cho MOI NPC MOI KHUNG, du ca vong chi
	// dung toi da 5 gia tri: m_nNpcSyncCursor khong doi trong vong (chi cap nhat
	// sau vong) va npcCount lay mot lan o tren.
	int aSyncIdx[kNpcSyncChunkSize];
	for (int k = 0; k < kNpcSyncChunkSize; k++)
		aSyncIdx[k] = (npcCount > 0) ? ((m_nNpcSyncCursor + k) % npcCount) : -1;
#endif

	while (pNode)
	{
		// PHAI lay node ke TRUOC khi goi Activate(). Activate() -> ProcStatus() ->
		// ServeMove() co the goi NpcChangeRegion (KNpc.cpp:4638, hoac :2120 khi chet)
		// -> KRegion::RemoveNpc -> KNode::Remove() dat m_pNext = NULL => GetNext() tra
		// NULL => vong lap DUNG GIUA CHUNG, moi NPC nam sau trong danh sach region do
		// khong duoc Activate() VA khong duoc NormalSync() trong tick nay.
		// Ma NormalSync la kenh DUY NHAT de client biet co nguoi la (SyncNpcMin ->
		// InsertNpcRequest); cac handler su kien khac lang le vut goi khi chua biet ID.
		// => cho dong nguoi: nguoi khong hien ra, bot ket o tu the dung, va NPC khong
		// duoc day di trong tick do.
		// Ban goc 43bca2e0 lam dung (pTmpNode lay truoc, dong 619/632); vong OBJECT ngay
		// ben duoi trong CUNG ham cung lay truoc. Chi rieng vong NPC nay bi sai.
		KIndexNode* pNpcTmpNode = (KIndexNode*)pNode->GetNext();
		int nNpcIdx = pNode->m_nIndex;
		if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
		{
#ifdef _SERVER
			// [24/08] DA BO nhanh dong bo cu:
			//     if ((nCounter == m_nNpcSyncCounter / 2) && (m_nNpcSyncCounter & 1))
			// m_nNpcSyncCounter CHI duoc gan 0 trong ctor (KRegion.cpp:35) va KHONG
			// o dau tang no (grep toan cay: dung 3 lan - khai bao, gan 0, doc o day),
			// nen '(0 & 1)' = 0 => dieu kien LUON SAI, khoi NormalSync do chua tung
			// chay. Doi chieu: m_nObjSyncCounter thi CO duoc tang - chung to day dung
			// la ma bo quen chu khong phai co y.
			// Viec dong bo that su van do vong 'chunk' ngay duoi dam nhiem.
			if (synced < kNpcSyncChunkSize && currentIndex == aSyncIdx[synced])
			{
				Npc[nNpcIdx].NormalSync();
				synced++;
			}
#endif
			// Always activate
			Npc[nNpcIdx].Activate();
			currentIndex++;
		}
		pNode = pNpcTmpNode;	// dung ban da lay TRUOC Activate (xem chu thich dau vong)
	}
	// Move sync cursor forward
	m_nNpcSyncCursor = (m_nNpcSyncCursor + kNpcSyncChunkSize) % (npcCount > 0 ? npcCount : 1);

    nCounter = 0;
    KIndexNode *pObjNode = NULL;
    KIndexNode *pObjTmpNode = NULL;
    pObjNode = (KIndexNode *)m_ObjList.GetHead();

    while (pObjNode)
    {
        pObjTmpNode = (KIndexNode *)pObjNode->GetNext();
#ifdef _SERVER
        if (pObjNode->m_nIndex > 0 && pObjNode->m_nIndex < MAX_OBJECT)
        {
            if ((nCounter == m_nObjSyncCounter / 2) && (m_nObjSyncCounter & 1))
            {
                Object[pObjNode->m_nIndex].SyncState();
            }
            nCounter++;
#else
        if (pObjNode->m_nIndex > 0)
        {
#endif
       if (pObjNode->m_nIndex >= 0 && pObjNode->m_nIndex < MAX_OBJECT)
         {
            try
            {
                Object[pObjNode->m_nIndex].Activate();
            }
            catch (...)
            {
#ifdef _SERVER
                if (Object[pObjNode->m_nIndex].m_nSubWorldID >= 0 && Object[pObjNode->m_nIndex].m_nRegionIdx >= 0)
                {
                    SubWorld[Object[pObjNode->m_nIndex].m_nSubWorldID].m_Region[Object[pObjNode->m_nIndex].m_nRegionIdx].RemoveObj(pObjNode->m_nIndex);
                }
#endif
                ObjSet.Remove(pObjNode->m_nIndex);
               }
            }
        }
        pObjNode = pObjTmpNode;
    }
    m_nObjSyncCounter++;
    if (m_nObjSyncCounter > m_ObjList.GetNodeCount() * 2)
    {
        m_nObjSyncCounter = 0;
    }

    pNode = (KIndexNode *)m_MissleList.GetHead();
    while (pNode)
    {
        pTmpNode = (KIndexNode *)pNode->GetNext();
        if (pNode->m_nIndex >= 0 && pNode->m_nIndex < MAX_MISSLE)
        {
            Missle[pNode->m_nIndex].Activate();
        }
        pNode = pTmpNode;
    }

#ifdef _SERVER
    pNode = (KIndexNode *)m_PlayerList.GetHead();
    while (pNode)
    {
        pTmpNode = (KIndexNode *)pNode->GetNext();
        if (pNode->m_nIndex > 0 && pNode->m_nIndex < MAX_PLAYER)
        {
            Player[pNode->m_nIndex].Active();
        }
        pNode = pTmpNode;
    }
#endif

#ifndef _SERVER
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RegionIndex == m_nIndex)	// Player Region
	{
		Player[CLIENT_PLAYER_INDEX].Active();
	}
#endif
}

void KRegion::AddNpc(int nIdx)
{
	if (nIdx > 0 && nIdx < MAX_NPC)
	{
		_ASSERT(Npc[nIdx].m_Node.m_Ref == 0);
		if (Npc[nIdx].m_Node.m_Ref == 0)
		{
			m_NpcList.AddTail(&Npc[nIdx].m_Node);
			Npc[nIdx].m_Node.AddRef();
		}
	}
}

void KRegion::RemoveNpc(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_NPC)
		return;

	//_ASSERT(Npc[nIdx].m_Node.m_Ref > 0);

	if (Npc[nIdx].m_Node.m_Ref > 0)
	{
		Npc[nIdx].m_Node.Remove();
		Npc[nIdx].m_Node.Release();
	}
	else
	{
		char szDebugString[128];
		sprintf(szDebugString, "KRegion::RemoveNpc Npc[%d].m_Node.m_Ref > 0", nIdx);
		OutputDebugString(szDebugString);
	}
#ifndef _SERVER
	Npc[nIdx].RemoveRes();
#endif
}

void KRegion::AddMissle(int nId)
{
	if (nId > 0 && nId < MAX_MISSLE)
	{
		//_ASSERT(Missle[nId].m_Node.m_Ref == 0);//dang loi o day
		if (Missle[nId].m_Node.m_Ref == 0)
		{
			m_MissleList.AddTail(&Missle[nId].m_Node);
			Missle[nId].m_Node.AddRef();
		}
	}
}

void KRegion::RemoveMissle(int nId)
{
	if (nId > 0 && nId < MAX_MISSLE)
	{
		//_ASSERT(Missle[nId].m_Node.m_Ref > 0);//kiem tra loi o day
		if (Missle[nId].m_Node.m_Ref > 0)
		{
			Missle[nId].m_Node.Remove();
			Missle[nId].m_Node.Release();
		}
	}
}

void KRegion::AddObj(int nIdx)
{

	if (nIdx <= 0 || nIdx >= MAX_OBJECT)
		return;

	KIndexNode *pNode = NULL;
	
	pNode = new KIndexNode;
	pNode->m_nIndex = nIdx;
	m_ObjList.AddTail(pNode);

	if (Object[nIdx].m_nMapX >= 0 && Object[nIdx].m_nMapY >= 0)
	{
		AddRef(Object[nIdx].m_nMapX, Object[nIdx].m_nMapY, obj_object);
		if (Object[nIdx].m_nKind == Obj_Kind_Obstacle)
			AddRef(Object[nIdx].m_nMapX, Object[nIdx].m_nMapY, obj_obstacle);
	}
}

void KRegion::RemoveObj(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_OBJECT)
		return;

	KIndexNode *pNode = NULL;
	BOOL bFound = FALSE;
	
	pNode = (KIndexNode *)m_ObjList.GetHead();
	
	while(pNode)
	{
		if (pNode->m_nIndex == nIdx)
		{
			pNode->Remove();
			delete pNode;
			bFound = TRUE;
			break;
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}

	// FIX 24/08 (di kem C3): CHI tra bo dem khi that su GO duoc nut khoi danh sach vung nay.
	// KObj::Release() dat m_nMapX = m_nMapY = m_nRegionIdx = 0, nen lan RemoveObj THU HAI cho
	// cung mot vat pham (GWM_OBJ_DEL gui 2 lan: nhat do o KPlayer.cpp:4963 + het han trong
	// KObj::Activate cung mot khung) se DecRef NHAM vao o (0,0) cua region 0. Dieu kien "> 0"
	// cu tinh co chan duoc chuyen do; sau khi doi thanh ">= 0" thi khong con gi chan nua.
	if (!bFound)
		return;

	// FIX 24/08: AddObj dung ">= 0" (KRegion.cpp:896) con day dung "> 0" => vat pham o COT 0 hoac
	// HANG 0 cua region duoc AddRef ma KHONG BAO GIO DecRef, bo dem ket o do vinh vien.
	if (Object[nIdx].m_nMapX >= 0 && Object[nIdx].m_nMapY >= 0)
	{
		DecRef(Object[nIdx].m_nMapX, Object[nIdx].m_nMapY, obj_object);
		if(Object[nIdx].m_nKind == Obj_Kind_Obstacle)
			DecRef(Object[nIdx].m_nMapX, Object[nIdx].m_nMapY, obj_obstacle);
	}
}

DWORD KRegion::GetTrap(int nMapX, int nMapY)
{
#ifdef _SERVER
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_GRID_WIDTH || nMapY >= REGION_GRID_HEIGHT)
	{
		return 0;
	}
	return m_dwTrap[nMapX][nMapY];
#else
	return 0;
#endif
}

BYTE KRegion::GetBarrier(int nMapX, int nMapY, int nDx, int nDy)
{
#ifdef _SERVER	
	long lType, lInfo;
	long lRet = 0;
	
	lInfo = m_Obstacle[nMapX][nMapY];
	lRet = lInfo & 0x0000000f;
	lType = (lInfo >> 4) & 0x0000000f;

	switch(lType)
	{
	case Obstacle_LT://33
		if (nDx + nDy > 32)
			lRet = Obstacle_NULL;
		break;
	case Obstacle_RT://49
		if (nDx < nDy)
			lRet = Obstacle_NULL;
		break;
	case Obstacle_LB://65
		if (nDx > nDy)
			lRet = Obstacle_NULL;
		break;
	case Obstacle_RB://81
		if (nDx + nDy < 32)
			lRet = Obstacle_NULL;
		break;
	default:
		break;
	}
	if (lRet != Obstacle_NULL)
		return lRet;
	if (g_nPbNpcChan && m_pNpcRef)
	{
		if (m_pNpcRef[nMapY * m_nWidth + nMapX] > 0)
			return Obstacle_JumpFly;
	}
	return Obstacle_NULL;
#else
	// PHAI dung CUNG cong tac voi nhanh server o tren. Truoc day nhanh nay khong
	// duoc gac, nen server cho di xuyen qua nhau con client van chan => client mo
	// phong nguoi khac bi DUNG trong khi server cho CHAY TIEP => lech tang mot chieu
	// ~10px/tick, den luc goi dong bo toi thi dan mot phat = "nguoi xung quanh giat
	// manh khi dong". Hai ben lech luat va cham la lech mo phong, khong the vot lai
	// bang noi suy.
	if (g_nPbNpcChan && m_pNpcRef)
	{
		if (m_pNpcRef[nMapY * m_nWidth + nMapX] > 0)
			return Obstacle_JumpFly;
	}
	return Obstacle_NULL;
#endif
}

//----------------------------------------------------------------------------
//	¹¦ÄÜ£º°´ ÏñËØµã×ø±ê * 1024 µÄ¾«¶ÈÅÐ¶ÏÄ³¸öÎ»ÖÃÊÇ·ñÕÏ°­
//	²ÎÊý£ºnGridX nGirdY £º±¾region¸ñ×Ó×ø±ê
//	²ÎÊý£ºnOffX nOffY £º¸ñ×ÓÄÚµÄÆ«ÒÆÁ¿(ÏñËØµã * 1024 ¾«¶È)
//	²ÎÊý£ºbCheckNpc £ºÊÇ·ñÅÐ¶ÏnpcÐÎ³ÉµÄÕÏ°­
//	·µ»ØÖµ£ºÕÏ°­ÀàÐÍ(if ÀàÐÍ == Obstacle_NULL ÎÞÕÏ°­)
//----------------------------------------------------------------------------
BYTE	KRegion::GetBarrierMin(int nGridX, int nGridY, int nOffX, int nOffY, BOOL bCheckNpc)
{
#ifdef _SERVER
	_ASSERT(0 <= nGridX && nGridX < REGION_GRID_WIDTH && 0 <= nGridY && nGridY < REGION_GRID_HEIGHT);
	long lType;
	long lRet = Obstacle_NULL;
	
	lRet = m_Obstacle[nGridX][nGridY] & 0x0000000f;
	lType = (m_Obstacle[nGridX][nGridY] >> 4) & 0x0000000f;

	if (lRet == Obstacle_NULL)
	{
		if (g_nPbNpcChan && bCheckNpc && m_pNpcRef)
		{
			if (m_pNpcRef[nGridY * m_nWidth + nGridX] > 0)
				return Obstacle_JumpFly;
		}
		return Obstacle_NULL;
	}

	switch(lType)
	{
	case Obstacle_LT:
		if (nOffX + nOffY > 32 * 1024)
			return Obstacle_NULL;
		break;
	case Obstacle_RT:
		if (nOffX < nOffY)
			return Obstacle_NULL;
		break;
	case Obstacle_LB:
		if (nOffX > nOffY)
			return Obstacle_NULL;
		break;
	case Obstacle_RB:
		if (nOffX + nOffY < 32 * 1024)
			return Obstacle_NULL;
		break;
	default:
		break;
	}

	return lRet;

#else
	_ASSERT(0 <= nGridX && nGridX < REGION_GRID_WIDTH && 0 <= nGridY && nGridY < REGION_GRID_HEIGHT);
	// Cung ly do voi GetBarrier o tren: phai chung cong tac voi phia server.
	if (g_nPbNpcChan && bCheckNpc && m_pNpcRef)
	{
		if (m_pNpcRef[nGridY * m_nWidth + nGridX] > 0)
			return Obstacle_JumpFly;
	}
	return Obstacle_NULL;
#endif
}

int KRegion::GetRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType)
{
	int nRet = 0;
	int index = nMapY * m_nWidth + nMapX;
	if (nMapX >= m_nWidth || nMapY >= m_nHeight || index < 0)
		return 0;

	switch(nType)
	{
	case obj_npc:
		nRet = (int)m_pNpcRef[index];
		break;
	case obj_object:
		nRet = (int)m_pObjRef[index];
		break;
	case obj_obstacle:
		nRet = (int)m_pObstacleRef[index];
		break;
	case obj_missle:
		nRet = (int)m_pMslRef[index];
		break;
	default:
		break;
	}
	return nRet;
}

BOOL KRegion::AddRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType)
{
	if (nMapX >= m_nWidth || nMapY >= m_nHeight || nMapX < 0 || nMapY < 0  || m_nWidth < 0 || m_nHeight < 0 )
		return FALSE;

	BYTE* pBuffer = NULL;
	int nRef = 0;
		
	switch(nType)
	{
	case obj_npc:
		pBuffer = m_pNpcRef;
		break;
	case obj_object:
		pBuffer = m_pObjRef;
		break;
	case obj_obstacle:
		pBuffer = m_pObstacleRef;
		break;
	case obj_missle:
		pBuffer = m_pMslRef;	// FIX 24/08: truoc tro nham m_pObjRef (bang dem VAT PHAM) trong khi
		break;			// GetRef(obj_missle) (KRegion.cpp:1082) lai doc m_pMslRef
	default:
		break;
	}
	if (pBuffer)
	{
		int index = nMapY * m_nWidth + nMapX;
		if (index >= m_nWidth * m_nHeight || index < 0)
			return FALSE;
		__try {
			nRef = (int)pBuffer[index];
		} __except (EXCEPTION_EXECUTE_HANDLER) {
			return FALSE;
		}
		if (nRef == 255)
			return FALSE;
		pBuffer[index]++;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL KRegion::DecRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType)
{
	if (nMapX >= m_nWidth || nMapY >= m_nHeight || nMapX < 0 || nMapY < 0  || m_nWidth < 0 || m_nHeight < 0)
		return FALSE;

	BYTE* pBuffer = NULL;
	int nRef = 0;

	switch(nType)
	{
	case obj_npc:
		pBuffer = m_pNpcRef;
		break;
	case obj_object:
		pBuffer = m_pObjRef;
		break;
	case obj_obstacle:
		pBuffer = m_pObstacleRef;
		break;
	case obj_missle:
		pBuffer = m_pMslRef;	// FIX 24/08: truoc tro nham m_pObjRef (bang dem VAT PHAM) trong khi
		break;			// GetRef(obj_missle) (KRegion.cpp:1082) lai doc m_pMslRef
	default:
		break;
	}

	if (pBuffer)
	{
		int index = nMapY * m_nWidth + nMapX;
        if (index >= m_nWidth * m_nHeight || index < 0)
        	return FALSE;
		__try {
			nRef = (int)pBuffer[index];
		} __except (EXCEPTION_EXECUTE_HANDLER) {
			return FALSE;
		}
		if (nRef == 0)
		{
			// [DO 31/08] Ai do vua tru mot o VON DA RONG => ke toan AddRef/DecRef da lech.
			// O nao bi tru oan se doc ra 0 trong khi van con NPC dung do, va KRegion::FindNpc
			// (KRegion.h:191) thoat ngay o cua chan == 0 => moi thu dung tren o ay TANG HINH
			// truoc va cham. Nhan nay de nghiem thu ban va KNpc.cpp:2344 (DecRef thua trong
			// OnRevive nhanh m_bNoRevive). Chi chay trong nhanh VON DA that bai => duong
			// chay binh thuong khong ton them gi. KHONG kep nhip: bo dem bi kep khong con
			// la bo dem (AutoLog da tu chan tran 1200 dong/giay).
			AUTOLOG("[REFOAN] DecRef tru o RONG: o=(%d,%d) type=%d rgn=(%d,%d)",
				nMapX, nMapY, (int)nType, (int)LOWORD(m_RegionID), (int)HIWORD(m_RegionID));
			return FALSE;
		}
		pBuffer[index]--;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


BOOL KRegion::AddPlayer(int nIdx)
{
	if (nIdx > 0 && nIdx < MAX_PLAYER)
	{
		if (Player[nIdx].m_Node.m_Ref)
		{
			printf("[ERROR!] AddPlayer: %d, m_Ref: %d\n", nIdx, Player[nIdx].m_Node.m_Ref);
			_ASSERT(FALSE);
		}
		else
		{
			m_PlayerList.AddTail(&Player[nIdx].m_Node);
			Player[nIdx].m_Node.AddRef();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL KRegion::RemovePlayer(int nIdx)
{
	if (nIdx > 0 && nIdx < MAX_PLAYER)
	{
		if (Player[nIdx].m_Node.m_Ref > 0)
		{
			Player[nIdx].m_Node.Remove();
			Player[nIdx].m_Node.Release();
			return TRUE;
		}
	}
	return FALSE;
}

//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÑ°ÕÒ±¾ÇøÓòÄÚÊÇ·ñÓÐÄ³¸öÖ¸¶¨ id µÄ npc
//-------------------------------------------------------------------------
int		KRegion::SearchNpc(DWORD dwNpcID)
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		if (Npc[pNode->m_nIndex].m_dwID == dwNpcID)
			return pNode->m_nIndex;
		pNode = (KIndexNode *)pNode->GetNext();
	}	

	return 0;
}

int KRegion::FindObject(int nMapX, int nMapY, bool bAutoFind)
{
	KIndexNode *pNode = NULL;
	
	pNode = (KIndexNode *)m_ObjList.GetHead();
	
	while(pNode)
	{
		if (Object[pNode->m_nIndex].m_nMapX == nMapX && Object[pNode->m_nIndex].m_nMapY == nMapY)
		{
			if (bAutoFind)
			{
				if (Object[pNode->m_nIndex].m_nKind == Obj_Kind_Money || 
					Object[pNode->m_nIndex].m_nKind == Obj_Kind_Item)
					return pNode->m_nIndex;
			}
			else
				return pNode->m_nIndex;
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}	
	return 0;
}

int KRegion::FindObject(int nObjID)
{
	KIndexNode *pNode = NULL;
	
	pNode = (KIndexNode *)m_ObjList.GetHead();
	
	while(pNode)
	{
		if (Object[pNode->m_nIndex].m_nID == nObjID)
		{
			return pNode->m_nIndex;
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}	
	return 0;
}

int KRegion::FindEquip(int nMapX, int nMapY)
{
	KIndexNode *pNode = NULL;
	
	pNode = (KIndexNode *)m_ObjList.GetHead();
	
	while(pNode)
	{
		if (Object[pNode->m_nIndex].m_nMapX == nMapX && Object[pNode->m_nIndex].m_nMapY == nMapY)
		{
			if (Object[pNode->m_nIndex].m_nKind == Obj_Kind_Item)
			{
				return pNode->m_nIndex;
			}
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}	
	return 0;
}

int	KRegion::SearchNpcSettingIdx(int  nNpcSettingIdx)
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		if (Npc[pNode->m_nIndex].m_NpcSettingIdx == nNpcSettingIdx)
			return pNode->m_nIndex;
		pNode = (KIndexNode *)pNode->GetNext();
	}	

	return 0;
}

int	KRegion::SearchNpcName(const char*  szName)
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		if (strcmp(Npc[pNode->m_nIndex].Name, szName) == 0)
			return pNode->m_nIndex;
		pNode = (KIndexNode *)pNode->GetNext();
	}	

	return 0;
}

int	KRegion::SearchNpcID(DWORD dwID)
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		if (g_FileName2Id(Npc[pNode->m_nIndex].Name) == dwID)
			return pNode->m_nIndex;
		pNode = (KIndexNode *)pNode->GetNext();
	}	

	return 0;
}

#ifdef	_SERVER
void KRegion::SendSyncData(int nClient)
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		Npc[pNode->m_nIndex].SendSyncData(nClient);
		pNode = (KIndexNode *)pNode->GetNext();
	}
	pNode = (KIndexNode *)m_ObjList.GetHead();
	while(pNode)
	{
		Object[pNode->m_nIndex].SyncAdd(nClient);
		pNode = (KIndexNode *)pNode->GetNext();
	}
}

// [BC 03/09 c] DO CHO RA GOC (khong sua hanh vi): moi 10 giay in ba dong vao jx_auto_server.log
//   [BC-NGUOI] so client thuc nhan goi, client nhan NHIEU NHAT bao nhieu goi/byte, loai goi nhieu nhat cua no.
//   [BC-TOP]   8 loai goi nhieu nhat theo SO LUOT GUI (ban truoc in 8 so hieu nho nhat -> giau mat 87/91/95).
// So hieu: 75 syncplayermin 76 syncnpc 77 syncnpcmin 78 syncnpcminplayer 84 npcremove 85 npcwalk 86 npcrun
//          87 npcattack 88 npcmagic 91 npchurt 92 npcdeath 95 skillcast.
static int   g_anBCNguoiGoi[MAX_PLAYER];	// so goi da gui cho tung nguoi choi (chi so Player[])
static int   g_anBCNguoiByte[MAX_PLAYER];
static short g_anBCNguoiLoai[MAX_PLAYER];	// loai goi gan nhat cua nguoi do (de xem ai doi loai gi)
static int   g_anBCLoaiGui[256];		// so luot GUI theo loai
static int   g_anBCLoaiByte[256];
static void BC_DemNguoiNhan(int nPlayerIndex, int nLoai, int nSize)
{
	if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
	{
		g_anBCNguoiGoi[nPlayerIndex]++;
		g_anBCNguoiByte[nPlayerIndex] += nSize;
		g_anBCNguoiLoai[nPlayerIndex] = (short)nLoai;
	}
	if (nLoai >= 0 && nLoai < 256)
	{
		g_anBCLoaiGui[nLoai]++;
		g_anBCLoaiByte[nLoai] += nSize;
	}
}
static void BC_BaoCao10s()
{
	// nguoi nhan
	int nSo = 0, nMax = 0, nMaxIdx = 0;
	double dTongByte = 0;
	for (int i = 1; i < MAX_PLAYER; i++)
	{
		if (g_anBCNguoiGoi[i] <= 0)
			continue;
		nSo++;
		dTongByte += g_anBCNguoiByte[i];
		if (g_anBCNguoiGoi[i] > nMax)
		{
			nMax = g_anBCNguoiGoi[i];
			nMaxIdx = i;
		}
	}
	AUTOLOG("[BC-NGUOI] 10s: so_client=%d tong_byte=%.0f | nhieu nhat: player=%d ten=%s goi=%d (%d/giay) byte=%d (%d KB/giay) loai_cuoi=%d",
		nSo, dTongByte, nMaxIdx,
		(nMaxIdx > 0 && Npc[Player[nMaxIdx].m_nIndex].Name[0]) ? Npc[Player[nMaxIdx].m_nIndex].Name : "?",
		nMax, nMax / 10, nMaxIdx > 0 ? g_anBCNguoiByte[nMaxIdx] : 0, nMaxIdx > 0 ? g_anBCNguoiByte[nMaxIdx] / 10240 : 0,
		nMaxIdx > 0 ? (int)g_anBCNguoiLoai[nMaxIdx] : -1);
	// top 8 loai theo so luot gui
	char szTop[512]; int nLen = 0;
	int anDung[256];
	memset(anDung, 0, sizeof(anDung));
	for (int nLan = 0; nLan < 8 && nLen < 440; nLan++)
	{
		int nBest = -1, nBestVal = 0;
		for (int k = 0; k < 256; k++)
		{
			if (!anDung[k] && g_anBCLoaiGui[k] > nBestVal)
			{
				nBestVal = g_anBCLoaiGui[k];
				nBest = k;
			}
		}
		if (nBest < 0)
			break;
		anDung[nBest] = 1;
		nLen += _snprintf(szTop + nLen, sizeof(szTop) - 1 - nLen, " %d:gui=%d,%dKB", nBest, g_anBCLoaiGui[nBest], g_anBCLoaiByte[nBest] / 1024);
	}
	szTop[sizeof(szTop) - 1] = 0;
	if (nLen > 0)
		AUTOLOG("[BC-TOP] 10s top loai theo luot GUI:%s", szTop);
	// [BC 04/09 do] vung cua client + 3 vung bi phat nhieu nhat + nhip tick
	{
		int nVungClient = (nMaxIdx > 0) ? Npc[Player[nMaxIdx].m_nIndex].m_RegionIndex : -1;
		int nGoiVung = (nVungClient >= 0 && nVungClient < BC_MAX_VUNG) ? g_anBCVungGoi[nVungClient] : 0;
		int nTick = (nVungClient >= 0 && nVungClient < BC_MAX_VUNG) ? g_anBCVungActive[nVungClient] : 0;
		int anTop[3] = { -1, -1, -1 };
		for (int nL = 0; nL < 3; nL++)
		{
			int nBest = -1, nVal = 0;
			for (int k = 0; k < BC_MAX_VUNG; k++)
			{
				if (k == anTop[0] || k == anTop[1] || k == anTop[2])
					continue;
				if (g_anBCVungGoi[k] > nVal)
				{
					nVal = g_anBCVungGoi[k];
					nBest = k;
				}
			}
			anTop[nL] = nBest;
		}
		AUTOLOG("[BC-VUNG] 10s: vung_client=%d nhan=%d luot (%d/tick, tick=%d ~%d/giay) | top vung: %d:%d %d:%d %d:%d",
			nVungClient, nGoiVung, nTick > 0 ? nGoiVung / nTick : 0, nTick, nTick / 10,
			anTop[0], anTop[0] >= 0 ? g_anBCVungGoi[anTop[0]] : 0,
			anTop[1], anTop[1] >= 0 ? g_anBCVungGoi[anTop[1]] : 0,
			anTop[2], anTop[2] >= 0 ? g_anBCVungGoi[anTop[2]] : 0);
		AUTOLOG("[BC-SYNC] 10s: NormalSync=%d (%d/giay) -> moi lan 9 luot (NPC thuong) hoac 18 luot (NPC nguoi choi: gui ca goi 77 va 75)",
			g_nBCNormalSync, g_nBCNormalSync / 10);
		{	// [BC 04/09 do2] so VUNG KHAC NHAU da gui cho client dang theo doi + nhip tick that
			int nSoVung = 0;
			for (int k = 0; k < BC_MAX_VUNG; k++)
				if (g_abyBCVungGui[k])
					nSoVung++;
			int nVungCuaHo = (g_nBCTheoDoi > 0 && g_nBCTheoDoi < MAX_PLAYER) ? Npc[Player[g_nBCTheoDoi].m_nIndex].m_RegionIndex : -1;
			AUTOLOG("[BC-TUVUNG] 10s: client theo doi=%d dung o vung=%d, NHAN GOI TU %d VUNG khac nhau (dung thiet ke phai <= 9) | tick that=%d (%d/giay)",
				g_nBCTheoDoi, nVungCuaHo, nSoVung, g_nBCTick, g_nBCTick / 10);
			memset(g_abyBCVungGui, 0, sizeof(g_abyBCVungGui));
			g_nBCTheoDoi = nMaxIdx;	// 10 giay toi theo doi client nhan nhieu nhat vua roi
			g_nBCTick = 0;
		}
		g_nBCNormalSync = 0;
		memset(g_anBCVungGoi, 0, sizeof(g_anBCVungGoi));
		memset(g_anBCVungActive, 0, sizeof(g_anBCVungActive));
	}
	memset(g_anBCNguoiGoi, 0, sizeof(g_anBCNguoiGoi));
	memset(g_anBCNguoiByte, 0, sizeof(g_anBCNguoiByte));
	memset(g_anBCLoaiGui, 0, sizeof(g_anBCLoaiGui));
	memset(g_anBCLoaiByte, 0, sizeof(g_anBCLoaiByte));
}
void KRegion::BroadCast(const void* pBuffer, DWORD dwSize, int &nMaxCount, int nOX, int nOY)
{
	#define	MAX_SYNC_RANGE	32//25
	// [BC 03/09 b] tam phat THAT hai chieu (|dx|,|dy| <= BC_TAM): kiem cu chi 'nDX <= 32' (khong tri tuyet doi) nen phia am
	// khong loc -> sau F4 moi goi cua 9 vung (~258 bot ria) doi vao 1 client (~59.000 goi/giay, do 03/09 dem) -> client sap.
	// Client tu bo goi >= 40 o (S6_XaQuaTam) nen gui ngoai 32 o la vo ich. [Server] BroadCastTam trong config.ini de chinh.
	static int s_nBCTam = -1;
	if (s_nBCTam < 0)
	{
		s_nBCTam = (int)GetPrivateProfileIntA("Server", "BroadCastTam", MAX_SYNC_RANGE, ".\\config.ini");
		if (s_nBCTam < 8) s_nBCTam = 8;
		if (s_nBCTam > 64) s_nBCTam = 64;
	}
	KIndexNode *pNode = NULL;

	//if (m_PlayerList.IsEmpty())
	if (m_PlayerList.m_nNodeCount <= 0)
		return;

	// Bat dau duyet tu con tro xoay chu khong phai dau danh sach: neu so nguoi trong
	// vung vuot tran nMaxCount thi moi lan phat se phuc vu mot doan khac nhau, thay vi
	// luon phuc vu dung nhom dau danh sach (nhung nguoi con lai se thanh vo hinh).
	// [F4 04/09] Truoc day 'nMaxCount--' nam NGOAI khoi if gui that (xem duoi) nen MOI node deu tru ngan sach,
	// ke ca BOT (m_nNetConnectIdx = -1, khong the nhan goi) va nguoi ngoai tam. KRegion::AddPlayer them CA bot vao
	// m_PlayerList nen mot vung Tong Kim ~258 bot an sach tran 100 truoc khi toi nguoi that; nMaxCount lai la int&
	// dung chung cho CURREGION + 8 vung lan can nen can o vung dau la 8 vung sau khong ai nhan.
	// Trieu chung (chu bao 04/09): 'bot chay toi dich nhung khong danh, dung yen lau sau moi danh vai cai' - bot VAN
	// danh (585 bot chet/phut o may chu), chi la client khong nhan duoc goi. Do client cua chu (2 phut, ~258 nguoi
	// quanh minh): 4.991 lenh chay tu 405 NPC nhung chi 366 goi chieu tu 95 NPC = 181/phut (~1-2% luong that).
	// Con tro xoay m_nBroadCastCursor la cach cu chia luot khi qua tai - no NHAY QUA ca nguoi that dung truoc no,
	// nen khi ngan sach chi con tinh nguoi that thi phai duyet VONG TRON (toi da nTong node) moi khong bo sot.
	const int nBCLoaiGoi = (pBuffer && dwSize > 0) ? (int)((const BYTE*)pBuffer)[0] : 0;
	if (m_nIndex >= 0 && m_nIndex < BC_MAX_VUNG)	// [BC 04/09 do] dem luot phat vao vung nay
		g_anBCVungGoi[m_nIndex]++;	// [BC 03/09 c] so hieu goi (byte dau)
	const int nF4Tong = m_PlayerList.m_nNodeCount;
	pNode = (KIndexNode *)m_PlayerList.GetHead();
	if (nF4Tong > nMaxCount)
	{
		m_nBroadCastCursor %= nF4Tong;
		for (int nSkip = 0; nSkip < m_nBroadCastCursor && pNode; nSkip++)
			pNode = (KIndexNode *)pNode->GetNext();
		if (pNode == NULL)
			pNode = (KIndexNode *)m_PlayerList.GetHead();
		m_nBroadCastCursor += nMaxCount;
	}
	int nF4Duyet = 0;	// so node da xet (chi de dem)
	int nF4Gui   = 0;	// so nguoi that da gui trong lan phat nay (chi de dem)
	int nBCNgoaiTam = 0;	// nguoi that bi loc vi ngoai tam (chi de dem)
	// [BC 04/09 LUI] TRA VE VONG LAP TRUOC F4: tru ngan sach cho MOI node hop le (ke ca bot), khong duyet vong tron.
	// Day chinh la bo han dong tinh co ma ca he thong dang dua vao: bo no ra thi mot client nhan
	// 21.000-94.844 goi/giay -> nghen socket -> may chu vut byte khi ghi thieu -> luong lech -> client doc rac -> sap.
	// Chi duoc bo F4 tro lai SAU KHI sua loi vut byte trong SocketServer.cpp (heaven.dll).
	while(pNode && nMaxCount > 0)
	{
		KIndexNode* pNext = (KIndexNode *)pNode->GetNext();
		nF4Duyet++;
		if (pNode->m_nIndex > 0 && pNode->m_nIndex < MAX_PLAYER)
		{
			int nPlayerIndex = pNode->m_nIndex;
			int nNpcIndex = Player[nPlayerIndex].m_nIndex;
			int nDX = Npc[nNpcIndex].m_MapX - nOX;
			int nDY = Npc[nNpcIndex].m_MapY - nOY;
			if (nDX < 0) nDX = -nDX;	// [BC 03/09 b] tri tuyet doi hai chieu (chi loc bot, giu lai)
			if (nDY < 0) nDY = -nDY;
			if (Player[pNode->m_nIndex].m_nNetConnectIdx >= 0 && (nDX > s_nBCTam || nDY > s_nBCTam))
				nBCNgoaiTam++;
			if (Player[pNode->m_nIndex].m_nNetConnectIdx >= 0 
				&& nDX <= s_nBCTam && nDY <= s_nBCTam
				&& Player[pNode->m_nIndex].m_bSleepMode == FALSE)
			{
				g_pServer->PackDataToClient(Player[pNode->m_nIndex].m_nNetConnectIdx, (BYTE*)pBuffer, dwSize);
				nF4Gui++;
				BC_DemNguoiNhan(pNode->m_nIndex, nBCLoaiGoi, (int)dwSize);
				if (pNode->m_nIndex == g_nBCTheoDoi && m_nIndex >= 0 && m_nIndex < BC_MAX_VUNG)
					g_abyBCVungGui[m_nIndex] = 1;
			}
			nMaxCount--;	// [BC 04/09 LUI] TRU CHO MOI NODE nhu ban goc (day la cai van)
		}
		pNode = pNext;
	}
	// [F4] bo dem 10 giay/dong (tu quan moc thoi gian de RESET duoc so lieu, khong dung AUTOLOG_EVERY):
	// goi phat | nguoi that da gui | lan bi cat vi het ngan sach (chi con khi > 100 NGUOI THAT cung vung) | node duyet.
	{
		static int   s_nF4Goi = 0, s_nF4Gui = 0, s_nF4Bo = 0, s_nF4Duyet = 0;
		static DWORD s_uF4Moc = 0;
		// [BC 03/09] huong 5: dem THEO LOAI GOI (byte dau cua goi = ProtocolType, xem Headers/KProtocolDef.h:
		// 76 syncnpcmin, 85 npcrun, 84 npcwalk, 90 npchurt, 91 npcdeath, 92 chgcurcamp, 94 skillcast).
		static int   s_anBCGoi[256], s_anBCGui[256], s_anBCBo[256];
		const int nBCLoai = (pBuffer && dwSize > 0) ? (int)((const BYTE*)pBuffer)[0] : 0;
		const int nBCCat  = (nMaxCount <= 0 && nF4Duyet < nF4Tong) ? 1 : 0;
		s_anBCGoi[nBCLoai]++; s_anBCGui[nBCLoai] += nF4Gui; s_anBCBo[nBCLoai] += nBCCat;
		static int s_nBCNgoaiTam = 0;	// [BC 03/09 b]
		s_nBCNgoaiTam += nBCNgoaiTam;
		s_nF4Goi++;
		s_nF4Gui   += nF4Gui;
		s_nF4Duyet += nF4Duyet;
		if (nBCCat)
			s_nF4Bo++;
		const DWORD uF4Now = GetTickCount();
		if (s_uF4Moc == 0)
			s_uF4Moc = uF4Now;
		else if (uF4Now - s_uF4Moc >= 10000)
		{
			s_uF4Moc = uF4Now;
			AUTOLOG("[BC-DEM] 10s: goi=%d gui=%d cat_vi_het_ngan_sach=%d node_duyet=%d ngoai_tam=%d tam=%d", s_nF4Goi, s_nF4Gui, s_nF4Bo, s_nF4Duyet, s_nBCNgoaiTam, s_nBCTam);
			// [BC 03/09] in toi da 8 loai goi nhieu nhat + moi loai co bi cat: 'loai:goi/gui/cat'
			char szBC[512]; int nBCLen = 0; int nBCIn = 0;
			for (int nBCK = 0; nBCK < 256 && nBCLen < 440; nBCK++)
			{
				if (s_anBCGoi[nBCK] <= 0)
					continue;
				if (nBCIn >= 8 && s_anBCBo[nBCK] <= 0)
					continue;
				nBCLen += _snprintf(szBC + nBCLen, sizeof(szBC) - 1 - nBCLen, " %d:%d/%d/%d", nBCK, s_anBCGoi[nBCK], s_anBCGui[nBCK], s_anBCBo[nBCK]);
				nBCIn++;
			}
			szBC[sizeof(szBC) - 1] = 0;
			if (nBCLen > 0)
				AUTOLOG("[BC-LOAI] 10s loai:goi/gui/cat:%s", szBC);
			memset(s_anBCGoi, 0, sizeof(s_anBCGoi)); memset(s_anBCGui, 0, sizeof(s_anBCGui)); memset(s_anBCBo, 0, sizeof(s_anBCBo));
			BC_BaoCao10s();	// [BC 03/09 c]
			s_nF4Goi = 0; s_nF4Gui = 0; s_nF4Bo = 0; s_nF4Duyet = 0; s_nBCNgoaiTam = 0;
		}
	}
}

//---------------------------------------------------------------------
// ²éÕÒ¸ÃRegionÖÐNpcIDÎªdwIdµÄPlayerË÷Òý
//---------------------------------------------------------------------
int KRegion::FindPlayer(DWORD dwId)
{
	KIndexNode *pNode = NULL;
	int	nRet = -1;

	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode)
	{
		if (Npc[Player[pNode->m_nIndex].m_nIndex].m_dwID == dwId)
		{
			nRet = pNode->m_nIndex;
			break;
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}
	return nRet;
}

//---------------------------------------------------------------------
// ²éÕÒ¸ÃRegionÖÐÊÇ·ñÓÐË÷ÒýÎª nPlayerIdx µÄÍæ¼Ò
//---------------------------------------------------------------------
BOOL	KRegion::CheckPlayerIn(int nPlayerIdx)
{
	if (nPlayerIdx <= 0)
		return FALSE;

	KIndexNode *pNode = NULL;
	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode)
	{
		if (pNode->m_nIndex == nPlayerIdx)
			return TRUE;
		pNode = (KIndexNode *)pNode->GetNext();
	}
	return FALSE;
}

void	KRegion::SetTrap(DWORD nTrapId, int nMapX, int nMapY)
{
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_GRID_WIDTH || nMapY >= REGION_GRID_HEIGHT)
		return;
	m_dwTrap[nMapX][nMapY] = nTrapId;
}

// [PORT5 23/08] tham so trap JX2 (AddMapTrap tham so 5). Than #ifdef nhu GetTrap.
int KRegion::GetTrapParam(int nMapX, int nMapY)
{
#ifdef _SERVER
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_GRID_WIDTH || nMapY >= REGION_GRID_HEIGHT)
		return JX2TRAP_PARAM_NONE;
	return m_nTrapParam[nMapX][nMapY];
#else
	return JX2TRAP_PARAM_NONE;
#endif
}

void KRegion::SetTrapParam(int nMapX, int nMapY, int nParam)
{
#ifdef _SERVER
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_GRID_WIDTH || nMapY >= REGION_GRID_HEIGHT)
		return;
	m_nTrapParam[nMapX][nMapY] = nParam;
#endif
}

// [PORT5 23/08] ClearMapTrap (Linux KRegion::ClearTrap 0x080E11A0): xoa SACH trap cua region
// (ke ca trap map-data - Linux cung vay; chi dung cho map chien truong rieng).
void KRegion::ClearAllTraps()
{
#ifdef _SERVER
	memset(m_dwTrap, 0, sizeof(m_dwTrap));
	for (int _tp = 0; _tp < REGION_GRID_WIDTH * REGION_GRID_HEIGHT; _tp++)
		((int*)m_nTrapParam)[_tp] = JX2TRAP_PARAM_NONE;
#endif
}

void	KRegion::SetObstacle(long value, int nSubWorld, int nMapX, int nMapY) //#Set VËt C¶n
{
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_GRID_WIDTH || nMapY >= REGION_GRID_HEIGHT)
		return;
	if(m_Obstacle[nMapX][nMapY] == 0)
		m_Obstacle[nMapX][nMapY] = value;

	S2C_SET_OBSTACLE cObjAdd;
	cObjAdd.ProtocolType = s2c_setobstacle;
	cObjAdd.pValue = value;
	cObjAdd.pMapX = nMapX;
	cObjAdd.pMapY = nMapY;

	//if(g_pServer && Player[nPlayerIndex].m_nNetConnectIdx != -1)
	//	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx,&NetCommand,sizeof(S2C_SET_OBSTACLE));

	/*POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	int nMaxCount = MAX_BROADCAST_COUNT;
	BroadCast((BYTE*)&cObjAdd, sizeof(S2C_SET_OBSTACLE), nMaxCount, nMapX, nMapY);
	int nConRegion;
	for (int i = 0; i < 8; i++)
	{
		nConRegion = m_nConnectRegion[i];
		if (nConRegion == -1)
			continue;
		
		SubWorld[nSubWorld].m_Region[nConRegion].BroadCast((BYTE*)&cObjAdd, sizeof(S2C_SET_OBSTACLE), nMaxCount, nMapX - POff[i].x, nMapY - POff[i].y);
	}*/
}

#endif

#ifndef _SERVER
void KRegion::Paint()
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		Npc[pNode->m_nIndex].Paint();
		pNode = (KIndexNode *)pNode->GetNext();
	}
	pNode = (KIndexNode *)m_MissleList.GetHead();
	while(pNode)
	{
		Missle[pNode->m_nIndex].Paint();
		pNode = (KIndexNode *)pNode->GetNext();
	}
	pNode = (KIndexNode *)m_ObjList.GetHead();
	while(pNode)
	{
//		Object[pNode->m_nIndex].Paint();		need add -spe
		pNode = (KIndexNode *)pNode->GetNext();
	}
}
#endif

void* KRegion::GetObjNode(int nIdx)
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_ObjList.GetHead();

	while(pNode)
	{
		if (pNode->m_nIndex == nIdx)
		{
			break;
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}
	return pNode;
}

void KRegion::Close()		// Çå³ýRegionÖÐµÄ¼¸¸öÁ´±í£¨ËùÖ¸ÏòµÄÄÚÈÝÃ»ÓÐ±»Çå³ý£©
{
	KIndexNode* pNode = NULL;
	KIndexNode* pTempNode = NULL;

	if (!m_nWidth || !m_nHeight)
		return;
	
	if (m_pNpcRef)
	{
		ZeroMemory(m_pNpcRef, m_nWidth * m_nHeight);
	}
	if (m_pObjRef)
	{
		ZeroMemory(m_pObjRef, m_nWidth * m_nHeight);
    }
	if (m_pObstacleRef)
    {
		ZeroMemory(m_pObstacleRef, m_nWidth * m_nHeight);
    }
	if (m_pMslRef)
	{
		ZeroMemory(m_pMslRef, m_nWidth * m_nHeight);
	}
	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KIndexNode *)pNode->GetNext();
#ifndef _SERVER
		// [S6 26/08] Duong MO COI thu 1: region-slot bi tai dung (LoadMap cuon vung) hoac
		// dong map -> MOI NPC cua region nay bi go (RegionIndex=-1, khong xoa khoi NpcSet).
		AUTOLOG("[S6-ORPHAN] npc=%u idx=%d kind=%u doing=%d cell=(%d,%d) regid=(%d,%d) t=%u", Npc[pTempNode->m_nIndex].m_dwID, pTempNode->m_nIndex, Npc[pTempNode->m_nIndex].m_Kind, (int)Npc[pTempNode->m_nIndex].m_Doing, Npc[pTempNode->m_nIndex].m_MapX, Npc[pTempNode->m_nIndex].m_MapY, (int)LOWORD(m_RegionID), (int)HIWORD(m_RegionID), timeGetTime());
#endif
		Npc[pTempNode->m_nIndex].m_RegionIndex = -1;
		RemoveNpc(pTempNode->m_nIndex);
	}

	pNode = (KIndexNode *)m_MissleList.GetHead();
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KIndexNode *)pNode->GetNext();
		MissleSet.Remove(pTempNode->m_nIndex);
		Missle[pTempNode->m_nIndex].m_nRegionId = -1;
		pTempNode->Remove();
		pTempNode->Release();
	}

	pNode = (KIndexNode *)m_ObjList.GetHead();
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KIndexNode *)pNode->GetNext();
#ifndef _SERVER
		Object[pTempNode->m_nIndex].Remove(FALSE);
#endif
		ObjSet.Remove(pTempNode->m_nIndex); // TODO:
		Object[pTempNode->m_nIndex].m_nRegionIdx = -1;
		pTempNode->Remove();
		delete pTempNode;
	}

	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KIndexNode *)pNode->GetNext();
		pTempNode->Remove();
		pTempNode->Release();
	}

	m_RegionID	= -1;
	m_nIndex	= -1;
	m_nActive	= 0;
	memset(m_nConnectRegion, -1, sizeof(m_nConnectRegion));
	memset(m_nConRegionID, -1, sizeof(m_nConRegionID));

}
