/********************************************************************
	created:	2003/06/03
	file base:	IDBRoleServer
	file ext:	h
	author:		liupeng
	
	purpose:	
*********************************************************************/
#ifndef __INCLUDE_IDBROLESERVER_H__
#define __INCLUDE_IDBROLESERVER_H__

#include "S3DBInterface.h"

BOOL InitDBInterface( size_t nMaxRoleCount );

void MergeDB();

void ReleaseDBInterface();

void *GetRoleInfo( char * pRoleBuffer, char * strUser, int &nBufLen );
void *GetRoleInfoForGM(int nInfoID, char * pRoleBuffer, char * strUser, int &nBufLen);
void SetRoleInfoForGM(int nInfoID, char * pRoleBuffer, char * strUser, int nBufLen);

int	SaveRoleInfo( char * pRoleBuffer, const char * strUser, BOOL bAutoInsertWhenNoExistUser );

// (20/08) Giu lai mot goi luu BI TU CHOI de con cuu ho, thay vi vut bo im lang.
// Tra ve TRUE neu da cat giu duoc.
BOOL QuarantineRoleInfo( char * pRoleBuffer, const char * strUser, const char * szLyDo );

int GetRoleListOfAccount( char * szAccountName, S3DBI_RoleBaseInfo * RoleBaseList, int nMaxCount );

bool DeleteRole( const char * strUser );

char* GetAccountByUser(char * strUser);//通过用户名查找帐户

//------------------------------------------------------------------------
//数据库备份与数据统计 Add By Fellow At 2003.08.14
bool StartBackupTimer(int aTime);		//开始运行备份线程
bool StopBackupTimer();							//结束运行备份线程
bool SuspendBackupTimer();						//挂起线程
bool ResumeBackupTimer();						//继续运行线程
bool IsBackupThreadWorking();					//线程是否正在运行
bool IsBackupWorking();							//是否在备份
bool DoManualBackup();							//手工备份

bool GetGameStat(TGAME_STAT_DATA* aStatData);
//------------------------------------------------------------------------
// (20/08) Cua so nhat ky cua giao dien Goddess. Dinh nghia o IDBRoleServer.cpp:23.
// Khai bao o day de ClientNode.cpp ghi duoc nhat ky khoa/tu choi luu.
extern HWND hListOutput;

void AddOutputString(HWND hListCtrl, char* aStr);
#endif // __INCLUDE_IDBROLESERVER_H__