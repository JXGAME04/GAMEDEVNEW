#include "stdafx.h"
#include "PlayerCreator.h"
#include "inifile.h"
#include "utils.h"
#include "tstring.h"
#include "Macro.h"
#include "GameDatadef.h"

using OnlineGameLib::Win32::CIniFile;
using OnlineGameLib::Win32::GetAppFullPath;
using OnlineGameLib::Win32::_tstring;
using OnlineGameLib::Win32::ToBool;


CPlayerCreator::CPlayerCreator()
{
	for (int i = 0; i < MAX_PLAYERTYPE_VALUE; i++)
	{
		m_pRoleData[i] = NULL;
	}
	
	Init();
}

CPlayerCreator::~CPlayerCreator()
{
	for (int i = 0; i < MAX_PLAYERTYPE_VALUE; i++)
	{
		if (!m_pRoleData[i])
		{
			delete [] m_pRoleData[i];
			m_pRoleData[i] = NULL;
		}
	}

	stdMapID2RID::iterator it;	
	for ( it = m_theMapID2RID.begin(); it != m_theMapID2RID.end(); it ++ )
	{
		stdRevivalID &SL = ( *it ).second;
		
		SL.clear();
	}
	
	m_theMapID2RID.erase( m_theMapID2RID.begin(), m_theMapID2RID.end() );
}

bool CPlayerCreator::Init()
{
	CIniFile theIniFile;

	_tstring sIniFilePathName;

	sIniFilePathName = GetAppFullPath( NULL );

	char	szFileName[MAX_PATH];

	for (int i = 0; i < MAX_PLAYERTYPE_VALUE; i++)
	{
		sprintf(szFileName, PLAYERCREATOR_FILE, i);
		_tstring sIniFileName = sIniFilePathName + szFileName;

		if (!m_pRoleData[i])
			m_pRoleData[i] = new BYTE[MAX_NEWPLAYER_BUFFER];
		ZeroMemory(m_pRoleData[i], MAX_NEWPLAYER_BUFFER * sizeof(BYTE));
		GetRoleDataFromIni(m_pRoleData[i], sIniFileName.c_str());
	}
	
	_tstring sRevivalFileName = sIniFilePathName + REVIVALID_FILENAME;

	CIniFile	cFile;
	cFile.SetFile( sRevivalFileName.c_str() );

	CIniFile::_VETSTR theIniVetstr;
	cFile.ReadSections( theIniVetstr );

	char szBuffer[64];

	CIniFile::_VETSTR::iterator it;
	for ( it = theIniVetstr.begin(); it != theIniVetstr.end(); it ++ )
	{
		_tstring info = ( *it );

		if ( info.empty() )
		{
			continue;
		}

		int nSection = atoi( info.c_str() );
		int nCount = cFile.ReadInteger( info.c_str(), "Count", 0 );

		if ( nCount > 0 )
		{
			stdRevivalID rid;
			
			for ( int i=0; i<nCount; i++ )
			{
				sprintf( szBuffer, "RevivalId%2.2d", i );
				
				int nID = cFile.ReadInteger( info.c_str(), szBuffer, 0 );
				
				rid.push_back( nID );
			}
			
			m_theMapID2RID.insert( stdMapID2RID::value_type( nSection, rid ) );
		}
	}

	return false;
}

unsigned CPlayerCreator::GetRevivalID( size_t nMapID, UINT nType /*= enumRandom*/ )
{
	stdMapID2RID::iterator it;
	
	if ( m_theMapID2RID.end() != ( it = m_theMapID2RID.find( nMapID ) ) )
	{
		stdRevivalID& sl = ( *it ).second;

		/*
		 * TODO : Don't get the server when it can't carry anyone
		 */
		if ( !sl.empty() )
		{
			int nCount = sl.size();

			ASSERT( nCount > 0 );

			int nIndex = rand() % nCount;
			nIndex = ( nIndex >= 0 && nIndex < nCount ) ? nIndex : 0;

			return sl[nIndex];
		}
	}

	return 0;
}

const TRoleData* CPlayerCreator::GetRoleData(unsigned int &uDataLength, LPROLEPARAM lpParam)
{
	int nIndex = lpParam->nSeries * ROLE_NO + lpParam->nSex;
	if (nIndex < 0 || nIndex >= MAX_PLAYERTYPE_VALUE)
	{
		uDataLength = 0;
		return NULL;
	}

	if (m_pRoleData[nIndex])
	{
		TRoleData *pData = ( TRoleData * )m_pRoleData[nIndex];

		ASSERT( pData );

		strncpy(pData->BaseInfo.szName, lpParam->szName, sizeof(pData->BaseInfo.szName));
        pData->BaseInfo.szName[sizeof(pData->BaseInfo.szName) - 1] = '\0';

		strncpy(pData->BaseInfo.caccname, lpParam->szAccName,  sizeof(pData->BaseInfo.caccname));
        pData->BaseInfo.caccname[sizeof(pData->BaseInfo.caccname) - 1] = '\0';

		pData->BaseInfo.bSex = ( bool )( 0 != lpParam->nSex );
		pData->BaseInfo.ifiveprop = lpParam->nSeries;

		unsigned nNativeID = GetRevivalID( lpParam->nMapID );

		pData->BaseInfo.irevivalid = lpParam->nMapID;
		pData->BaseInfo.irevivalx = nNativeID;
		pData->BaseInfo.irevivaly = 0;
		
		uDataLength = pData->dwDataLen;
		return ( const TRoleData * )pData;
	}

	uDataLength = 0;
	return NULL;
}

bool CPlayerCreator::GetRoleDataFromIni(BYTE* pData, const char* szFileName)
{
	return true;
}