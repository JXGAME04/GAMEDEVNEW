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

KRegion::KRegion()
{
	m_nIndex		= -1;
	m_RegionID		= -1;
	m_nActive		= 0;
	m_nNpcSyncCounter = 0;
	m_nObjSyncCounter = 0;
	m_nWidth		= 0;
	m_nHeight		= 0;
	ZeroMemory(m_nConnectRegion, 8 * sizeof(int));
	memset(m_nConRegionID, -1, sizeof(m_nConRegionID));
	m_pNpcRef		= NULL;
	m_pObjRef		= NULL;
	m_pMslRef		= NULL;
#ifdef _SERVER
	memset(m_dwTrap, 0, sizeof(m_dwTrap));
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

	if (!m_pMslRef)
		m_pMslRef = new BYTE[nWidth * nHeight];
	if (!m_pMslRef)
		return FALSE;
	ZeroMemory(m_pMslRef, nWidth * nHeight);
	
	return TRUE;
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
		cData.Seek(dwHeadSize + sElemFile[REGION_OBJ_FILE_INDEX].uOffset, FILE_BEGIN);
		LoadServerObj(nSubWorld, &cData, sElemFile[REGION_OBJ_FILE_INDEX].uLength);

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
				if(sNpcCell.shKind == kind_dialoger)	//ChØ load c¸c npc ®èi tho¹i
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
			strcpy(sNpcCell.szName, gRenameNpc);
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
			if(sNpcCell.shKind == kind_dialoger)	//ChØ load c¸c npc ®èi tho¹i
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

void KRegion::Activate()
{
    KIndexNode *pNode = NULL;
    KIndexNode *pTmpNode = NULL;

    int nCounter = 0;

    pNode = (KIndexNode *)m_NpcList.GetHead();

    while (pNode)
    {
        pTmpNode = (KIndexNode *)pNode->GetNext();
        int nNpcIdx = pNode->m_nIndex;
        if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
        {
#ifdef _SERVER
            if ((nCounter == m_nNpcSyncCounter / 2) && (m_nNpcSyncCounter & 1))
            {
                Npc[nNpcIdx].NormalSync();
            }
            nCounter++;
#endif
            Npc[nNpcIdx].Activate();
        }
        pNode = pTmpNode;
    }
    m_nNpcSyncCounter++;
    if (m_nNpcSyncCounter > m_NpcList.GetNodeCount() * 2)
    {
        m_nNpcSyncCounter = 0;
    }

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
	}
}

void KRegion::RemoveObj(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_OBJECT)
		return;

	KIndexNode *pNode = NULL;
	
	pNode = (KIndexNode *)m_ObjList.GetHead();
	
	while(pNode)
	{
		if (pNode->m_nIndex == nIdx)
		{
			pNode->Remove();
			delete pNode;
			break;
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}

	if (Object[nIdx].m_nMapX > 0 && Object[nIdx].m_nMapY > 0)
	{
		DecRef(Object[nIdx].m_nMapX, Object[nIdx].m_nMapY, obj_object);
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
	if (m_pNpcRef)
	{
		if (m_pNpcRef[nMapY * m_nWidth + nMapX] > 0)
			return Obstacle_JumpFly;
	}
	return Obstacle_NULL;
#else
	if (m_pNpcRef)
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
		if (bCheckNpc && m_pNpcRef)
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
	if (bCheckNpc && m_pNpcRef)
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
	//
	if (nMapX >= m_nWidth || nMapY >= m_nHeight || nMapX < 0 || nMapY < 0) 
		return 0;
	//
	//if(m_nWidth > REGION_GRID_WIDTH) m_nWidth = REGION_GRID_WIDTH;
	//
	//if(m_nHeight > REGION_GRID_HEIGHT) m_nHeight = REGION_GRID_HEIGHT;
	//
	switch(nType)
	{
	case obj_npc:
		nRet = (int)m_pNpcRef[nMapY * m_nWidth + nMapX];
		break;
	case obj_object:
		nRet = (int)m_pObjRef[nMapY * m_nWidth + nMapX];
		break;
	case obj_missle:
		nRet = (int)m_pMslRef[nMapY * m_nWidth + nMapX];
		break;
	default:
		break;
	}
	return nRet;
}

BOOL KRegion::AddRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType)
{
	BYTE* pBuffer = NULL;
	int nRef = 0;
	//
	if (nMapX >= m_nWidth || nMapY >= m_nHeight || nMapX < 0 || nMapY < 0) 
		return FALSE;
	//
	//if(m_nWidth > REGION_GRID_WIDTH) m_nWidth = REGION_GRID_WIDTH;
	//
	//if(m_nHeight > REGION_GRID_HEIGHT) m_nHeight = REGION_GRID_HEIGHT;
	//
	switch(nType)
	{
	case obj_npc:
		pBuffer = m_pNpcRef;
		break;
	case obj_object:
		pBuffer = m_pObjRef;
		break;
	case obj_missle:
		pBuffer = m_pObjRef;
		break;
	default:
		break;
	}
	if (pBuffer)
	{
		nRef = (int)pBuffer[nMapY * m_nWidth + nMapX];
		if (nRef == 255)
			return FALSE;
		else
		{
			pBuffer[nMapY * m_nWidth + nMapX]++;
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}
}

BOOL KRegion::DecRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType)
{
	BYTE* pBuffer = NULL;
	int nRef = 0;
	//
	if (nMapX >= m_nWidth || nMapY >= m_nHeight || nMapX < 0 || nMapY < 0) 
		return FALSE;
	//
	//if(m_nWidth > REGION_GRID_WIDTH) m_nWidth = REGION_GRID_WIDTH;
	//
	//if(m_nHeight > REGION_GRID_HEIGHT) m_nHeight = REGION_GRID_HEIGHT;
	//	
	switch(nType)
	{
	case obj_npc:
		pBuffer = m_pNpcRef;
		break;
	case obj_object:
		pBuffer = m_pObjRef;
		break;
	case obj_missle:
		pBuffer = m_pObjRef;
		break;
	default:
		break;
	}
	if (pBuffer)
	{
		nRef = (int)pBuffer[nMapY * m_nWidth + nMapX];
		if (nRef == 0)
		{
//			_ASSERT(0);
			return FALSE;
		}
		else
		{
			pBuffer[nMapY * m_nWidth + nMapX]--;
			return TRUE;
		}
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
		_ASSERT(Player[nIdx].m_Node.m_Ref == 0);
		if (Player[nIdx].m_Node.m_Ref == 0)
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

void KRegion::BroadCast(const void* pBuffer, DWORD dwSize, int &nMaxCount, int nOX, int nOY)
{
	#define	MAX_SYNC_RANGE	32//25
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode && nMaxCount > 0)
	{
	 if (pNode->m_nIndex > 0 && pNode->m_nIndex < MAX_PLAYER)
	  { 
		int nPlayerIndex = pNode->m_nIndex;
		int nNpcIndex = Player[nPlayerIndex].m_nIndex;
		int nDX = Npc[nNpcIndex].m_MapX - nOX;
		int nDY = Npc[nNpcIndex].m_MapY - nOY;
		if (Player[pNode->m_nIndex].m_nNetConnectIdx >= 0 
			&& nDX <= MAX_SYNC_RANGE && nDY <= MAX_SYNC_RANGE
			&& Player[pNode->m_nIndex].m_bSleepMode == FALSE)
			g_pServer->PackDataToClient(Player[pNode->m_nIndex].m_nNetConnectIdx, (BYTE*)pBuffer, dwSize);
		nMaxCount--;
		pNode = (KIndexNode *)pNode->GetNext();
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
	if (m_pMslRef)
	{
		ZeroMemory(m_pMslRef, m_nWidth * m_nHeight);
	}
	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KIndexNode *)pNode->GetNext();
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
