#include "stdafx.h"
#include "IDBRoleServer.h"
#include "DBTable.h"
#include "DBBackup.h"
#include "../../../../Headers/KGmProtocol.h"
#include "CRC32.h"

#include <iostream>
#include <strstream>

using namespace std;

#include "Macro.h"

static ZDBTable *db_table = NULL;
static size_t nMaxRoleCount_InAccount = 3;
static CDBBackup::TStatData GameStatData;
static CDBBackup* DBBackup = NULL;

static HANDLE hDeadLockThread = NULL;		
static HANDLE hRemoveLogThread = NULL;	

HWND hListOutput = NULL;	

int get_account(DB *db, const DBT *pkey, const DBT *pdata, DBT *ikey) 
{
	memset( ikey, 0, sizeof( DBT ) );
	TRoleData *pRoleData = (TRoleData *)pdata->data;

	ikey->data = pRoleData->BaseInfo.caccname;
	ikey->size = strlen( pRoleData->BaseInfo.caccname ) + 1;

	return 0;
}

//========== Add By Fong Ki襲 2020.9.10==============
DWORD WINAPI DeadlockProc(LPVOID lpParameter) {
	while(!db_table->bStop) {
		Sleep(5 * 1000);	
		db_table->deadlock();
	}
	return 0;
}

DWORD WINAPI RemoveLogProc(LPVOID lpParameter) {
	while(!db_table->bStop) {
		db_table->removeLog();
		Sleep(60 * 60 * 1000);			
	}
	return 0;
}

BOOL InitDBInterface( size_t nMaxRoleCount )
{
	nMaxRoleCount_InAccount = nMaxRoleCount;

	db_table = new ZDBTable( "database", "roledb" );

	db_table->addIndex( get_account );
	if ( db_table->open() )
	{
		DWORD dwThreadId, dwThrdParam = 1;
/*

		hDeadLockThread = CreateThread(
			NULL,				// no security attributes 
			0,					// use default stack size  
			DeadlockProc,		// thread function 
			&dwThrdParam,		// argument to thread function 
			0,					// use default creation flags 
			&dwThreadId);		// returns the thread identifier 
		if(!hDeadLockThread)
		{
			//创建线程失败,暂时没有处理
		}
*/

		hRemoveLogThread = CreateThread(
			NULL,				// no security attributes 
			0,					// use default stack size  
			RemoveLogProc,		// thread function 
			&dwThrdParam,		// argument to thread function 
			0,					// use default creation flags 
			&dwThreadId);		// returns the thread identifier 
		if(!hRemoveLogThread)
		{
			//创建线程失败,暂时没有处理
		}

		return TRUE;
	}
		
	return FALSE;
}

void ReleaseDBInterface()		//释放数据库引擎
{
	if ( db_table )
	{
		db_table->commit();
/*

		if(!hDeadLockThread)
			TerminateThread(hDeadLockThread, 0);
*/

		if(!hRemoveLogThread)
			TerminateThread(hRemoveLogThread, 0);

		db_table->removeLog();
		
		StopBackupTimer();//停止备份线程
		
		db_table->close();
		delete db_table;
	}
}

//[wxb 2003-7-23]
void SetRoleInfoForGM(int nInfoID, char* pRoleBuffer, char* strUser, int nBufLen)
{
	char* aBuffer = new char[64 * 1024];
	TRoleData* pRoleData = NULL;
	int size;

	GetRoleInfo(aBuffer, strUser, size);
	if(size)
	{
		pRoleData = (TRoleData*)aBuffer;
		switch (nInfoID)
		{
		case gm_role_entergame_position:
			ASSERT(nBufLen == sizeof(GM_ROLE_DATA_SUB_ENTER_POS));
			memcpy(&(pRoleData->BaseInfo.ientergameid), pRoleBuffer, sizeof(GM_ROLE_DATA_SUB_ENTER_POS));
			break;
		default:
			ASSERT(0);
			return;
			break;
		}
		SaveRoleInfo(aBuffer, strUser, false);
	}
	
    delete aBuffer;
}

//[wxb 2003-7-22]
void *GetRoleInfoForGM(int nInfoID, char * pRoleBuffer, char * strUser, int &nBufLen)
{
	int size = 0;
	ZCursor *cursor = db_table->search( strUser, strlen( strUser ) + 1 );
//	char *buffer = db_table->search( strUser, strlen( strUser ) + 1, size );
	if ( cursor )
	{
		TRoleData* pRole = (TRoleData*)cursor->data;
		switch(nInfoID) {
		case gm_role_entergame_position:
			nBufLen = sizeof(pRole->BaseInfo.ientergameid) + sizeof(pRole->BaseInfo.ientergamex) + sizeof(pRole->BaseInfo.ientergamey);
			memcpy( pRoleBuffer, &(pRole->BaseInfo.ientergameid), nBufLen );
			break;
		default:
			ASSERT(0);
			nBufLen = 0;
			break;
		}

		db_table->closeCursor(cursor);
	}
	else
	{
		nBufLen = 0;
	}

	return pRoleBuffer;
}

void *GetRoleInfo( char * pRoleBuffer, char * strUser, int &nBufLen )
{
	//Add by phong ki襲  2021
	char aStr[1024];
	TRoleData *pRoleData = ( TRoleData * )pRoleBuffer;
	sprintf(aStr,"GetRoleInfo:%s",strUser);
	AddOutputString(hListOutput,aStr);
	//===============
	int size = 0;
	ZCursor *cursor = db_table->search( strUser, strlen( strUser ) + 1 );
//	char *buffer = db_table->search( strUser, strlen( strUser ) + 1, size );
	if ( cursor )
	{
		nBufLen = size;
		memcpy( pRoleBuffer, cursor->data, cursor->size );
		nBufLen = cursor->size;
		db_table->closeCursor(cursor);
	}
	else
	{
		nBufLen = 0;
	}

	return pRoleBuffer;
}

int	SaveRoleInfo( char * pRoleBuffer, const char *strUser, BOOL bAutoInsertWhenNoExistUser )
{
	ASSERT( pRoleBuffer );

	TRoleData *pRoleData = ( TRoleData * )pRoleBuffer;
	
	//fix by phong ki襲
	char aStr[1024];
	sprintf(aStr,"SaveRoleInfo:%s dwDataLen=%d",pRoleData->BaseInfo.szName,pRoleData->dwDataLen);
	AddOutputString(hListOutput,aStr);
	//===============

	if(pRoleData->dwDataLen >= 64 * 1024) return 0;//64K

	if(bAutoInsertWhenNoExistUser)
	{
		char *ptr = pRoleData->BaseInfo.caccname;	
		while(*ptr) {
		if(*ptr >= 'A' && *ptr <= 'Z') *ptr += 'a' - 'A';
		ptr++;
		}
	}

	if (!bAutoInsertWhenNoExistUser)
	{
		DWORD	dwCRC = 0;
		dwCRC = CRC32(dwCRC, pRoleData, pRoleData->dwDataLen - 4);
		DWORD	dwOrigCRC = *(DWORD *)(pRoleBuffer + pRoleData->dwDataLen - 4);
		if (dwCRC != dwOrigCRC)
		{
			// TODO:
			FILE *fLog = fopen("crc_error", "a+b");
			if(fLog) {
				char buffer[255];
				sprintf(buffer, "----\r\n%s\r\b%s\r\n", pRoleData->BaseInfo.szName, pRoleData->BaseInfo.caccname);
				fwrite(buffer, 1, strlen(buffer), fLog);
				fwrite(pRoleBuffer, 1, pRoleData->dwDataLen, fLog);
				fclose(fLog);
			}
			return 0;
		}
	}

	char szAccountName[32];

	int nLength = strlen( pRoleData->BaseInfo.caccname );

	ASSERT( nLength > 0 );

	nLength = nLength > 31 ? 31 : nLength;

	memcpy( szAccountName, pRoleData->BaseInfo.caccname, nLength );
	szAccountName[nLength] = '\0';

	char szName[32];
	const char *pName = szName;

	if ( NULL == strUser || strUser[0] == 0 )
	{
		int len = strlen( pRoleData->BaseInfo.szName );

		ASSERT( len > 0 );

		len = len > 31 ? 31 : len;

		memcpy( szName, pRoleData->BaseInfo.szName, len );
		szName[len] = '\0';
	}
	else
	{
		int len = strlen( strUser );

		ASSERT( len > 0 );

		memcpy( szName, strUser, len );
		szName[len] = '\0';
	}
	
	if ( bAutoInsertWhenNoExistUser )
	{
		int nCount = 0;
		int size = 0;

		/*
		 * Role of same name only
		 */
		ZCursor *user_cursor = db_table->search(pName, strlen( pName ) + 1 );
//		char *user_data = db_table->search( pName, strlen( pName ) + 1, size );
		if ( user_cursor )
		{
			db_table->closeCursor(user_cursor);

			return 0;
		}

		/*
		 * Get count of role by the key of account
		 */
		ZCursor *cursor = db_table->search( szAccountName, strlen( szAccountName ) + 1, 0 );
//		char *buffer = db_table->search( szAccountName, strlen( szAccountName ) + 1, size, 0 );

		while ( cursor )
		{
			nCount ++;
			if(!db_table->next( cursor ))break;
		}

		if ( nCount >= nMaxRoleCount_InAccount )
		{
			return 0;
		}
	}

	if ( db_table->add( pName, strlen( pName ) + 1, pRoleBuffer, pRoleData->dwDataLen ) )
	{
		return 1;
	}

	return 0;
}

int GetRoleListOfAccount( char * szAccountName, S3DBI_RoleBaseInfo * RoleBaseList, int nMaxCount )
{
	int size = 0;
	int count = 0;

	char *ptr = szAccountName;	
	while(*ptr) {
		if(*ptr >= 'A' && *ptr <= 'Z') *ptr += 'a' - 'A';
		ptr++;
	}

	//======add by phong ki襲
	char aStr[1024]; int jxcUseRevive = 0; int jxientergameid = 0;
	sprintf(aStr,"GetRoleListOfAccount:%s",szAccountName);
	AddOutputString(hListOutput,aStr);
	//===============

	S3DBI_RoleBaseInfo *base_ptr = RoleBaseList;

	ZCursor *cursor = db_table->search( szAccountName, strlen( szAccountName ) + 1, 0 );
//	char *buffer = db_table->search( szAccountName, strlen( szAccountName ) + 1, size, 0 );
	
	while ( count < nMaxCount && cursor )
	{
		TRoleData *pRoleData = (TRoleData *)cursor->data;
		
		strncpy( base_ptr->szName, pRoleData->BaseInfo.szName, 32 ); 
        base_ptr->szName[31] = '\0';
		
		base_ptr->Sex = pRoleData->BaseInfo.bSex;
		base_ptr->Series = pRoleData->BaseInfo.ifiveprop;
//		base_ptr->HelmType = pRoleData->BaseInfo.ihelmres;
//		base_ptr->ArmorType = pRoleData->BaseInfo.iarmorres;
//		base_ptr->WeaponType = pRoleData->BaseInfo.iweaponres;
		base_ptr->Level = pRoleData->BaseInfo.ifightlevel;

		jxcUseRevive = pRoleData->BaseInfo.cUseRevive;//add by phong ki襲
		jxientergameid = pRoleData->BaseInfo.ientergameid;
		
		sprintf(aStr,"GetRoleListOfAccount:%s cUseRevive=%d-%d-%d-%d ientergameid=%d-%d-%d",base_ptr->szName, jxcUseRevive
			, pRoleData->BaseInfo.irevivalid, pRoleData->BaseInfo.irevivalx, pRoleData->BaseInfo.irevivaly
			, jxientergameid, pRoleData->BaseInfo.ientergamex, pRoleData->BaseInfo.ientergamey);
		
		AddOutputString(hListOutput,aStr);//end add

		base_ptr++;
		
		/*
		 * Get next info from database
		 */
		count++;
		if(!db_table->next( cursor ))break;
	}
	return count;
}

bool DeleteRole( const char * strUser )
{
	if ( db_table && strUser && strUser[0] )
	{
		return db_table->remove( strUser, strlen( strUser ) + 1 );
	}
	return false;
}

// 下面的函数可能存在内存泄漏问题
//char* GetAccountByUser(char * strUser)
//{//通过用户名查找帐户
//	char* aBuffer = new char[64 * 1024];
//	TRoleData* pRoleData;
//	int size;
//
//	GetRoleInfo(aBuffer, strUser, size);
//	if(size)
//	{
//		pRoleData = (TRoleData*)aBuffer;
//		return pRoleData->BaseInfo.caccname;
//	}
//	else
//	{
//		return NULL;
//	}
//}

//------------------------------------------------------------------------
//数据库备份与数据统计 Add By Fellow At 2003.08.14
bool StartBackupTimer(int aTime)
{//开始运行备份线程
	DBBackup = new CDBBackup( "database", "roledb", db_table );
	DBBackup->SaveStatInfo();
	bool aStartResult = DBBackup->Open(aTime);
	return aStartResult;
}

bool StopBackupTimer()
{//结束运行备份线程
	if(!DBBackup)return false;
	while(IsBackupWorking()){}//等待备份线程结束

	bool aResult = DBBackup->Close();
	delete DBBackup;
	DBBackup = NULL;
	return aResult;
}

bool SuspendBackupTimer()
{//挂起线程
	if(!DBBackup)return false;
	return DBBackup->Suspend();
}

bool ResumeBackupTimer()
{//继续运行线程
	if(!DBBackup)return false;
	return DBBackup->Resume();
}

bool IsBackupThreadWorking()
{//线程是否正在运行
	if(!DBBackup)return false;
	return DBBackup->IsWorking();
}

bool IsBackupWorking()
{//是否在备份
	if(!DBBackup)return false;
	return DBBackup->IsBackuping();
}

bool DoManualBackup()
{
	if(!DBBackup)return false;
	return DBBackup->ManualBackup();
}

bool GetGameStat(TGAME_STAT_DATA* aStatData)
{
	if(!DBBackup)return false;
	TGAME_STAT_DATA tmpStatData = DBBackup->GetSendStatData();
	memcpy(aStatData, &tmpStatData, sizeof(TGAME_STAT_DATA));
	return true;
}

void AddOutputString(HWND hListCtrl, char* aStr)
{
	if ( hListCtrl && ::IsWindow( hListCtrl ) )
	{
		int nCount = ::SendMessage( hListCtrl, LB_GETCOUNT, 0, 0 );
		if(nCount >= 100)
		{
			::SendMessage( hListCtrl, LB_DELETESTRING, 100, 0 );
		}
		int nIndex = ::SendMessage( hListCtrl, LB_INSERTSTRING, 0, ( LPARAM )aStr );
	}
}