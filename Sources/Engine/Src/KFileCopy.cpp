//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KFileCopy.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Win32 File Copy Operation Class
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KDebug.h"
#include "KMemBase.h"
#include "KStrBase.h"
#include "KFilePath.h"
#include "KFileCopy.h"
//---------------------------------------------------------------------------
// ����:	CopyFiles
// ����:	�����ļ�
// ����:	lpSrcDir		ԴĿ¼
//			lpDesDir		Ŀ��Ŀ¼
// ����:	void
//---------------------------------------------------------------------------
ENGINE_API void g_DeleteFiles(LPSTR lpSrcDir, LPSTR lpDesDir)
{
	char			szSrcDir[MAXPATH];
	char			szDesDir[MAXPATH];
	WIN32_FIND_DATA FindData1;
	WIN32_FIND_DATA FindData2;
	HANDLE			hFind1;
	HANDLE			hFind2;
	int				nSrcLen;
	int				nDesLen;
	
	g_StrCpy(szSrcDir, lpSrcDir);
	g_StrCpy(szDesDir, lpDesDir);

	nSrcLen = g_StrLen(szSrcDir);
	nDesLen = g_StrLen(szDesDir);
	
	g_StrCpy(&szSrcDir[nSrcLen], "*.*");
	g_StrCpy(&szDesDir[nDesLen], "*.*");
	
	hFind1 = FindFirstFile(szSrcDir, &FindData1);
	if (hFind1 == INVALID_HANDLE_VALUE)
		return;
	
	while (FindNextFile(hFind1, &FindData1))
	{
		g_StrCpy(&szSrcDir[nSrcLen], FindData1.cFileName);
		g_StrCpy(&szDesDir[nDesLen], FindData1.cFileName);
		hFind2 = FindFirstFile(szDesDir, &FindData2);
		// ���szDesDirû��ͬ���ļ���ɾ��szSrcDir�µ��ļ�
		if (hFind2 == INVALID_HANDLE_VALUE)
		{
			SetFileAttributes(szSrcDir, FILE_ATTRIBUTE_NORMAL);
			DeleteFile(szSrcDir);
		}
		FindClose(hFind2);
	}
	FindClose(hFind1);
}
//---------------------------------------------------------------------------
// ����:	UpdateFiles
// ����:	�����ļ�
// ����:	lpSrcDir		ԴĿ¼
//			lpDesDir		Ŀ��Ŀ¼
// ����:	void
//---------------------------------------------------------------------------
ENGINE_API void g_UpdateFiles(LPSTR lpSrcDir, LPSTR lpDesDir)
{
	char			szSrcDir[MAXPATH];
	char			szDesDir[MAXPATH];
	WIN32_FIND_DATA FindData1;
	WIN32_FIND_DATA FindData2;
	HANDLE			hFind1;
	HANDLE			hFind2;
	LONG			lResult;
	int				nSrcLen;
	int				nDesLen;
	
	g_StrCpy(szSrcDir, lpSrcDir);
	g_StrCpy(szDesDir, lpDesDir);

	nSrcLen = g_StrLen(szSrcDir);
	nDesLen = g_StrLen(szDesDir);
	
	g_StrCpy(&szSrcDir[nSrcLen], "*.*");
	g_StrCpy(&szDesDir[nDesLen], "*.*");
	
	hFind1 = FindFirstFile(szSrcDir, &FindData1);
	if (hFind1 == INVALID_HANDLE_VALUE)
		return;

	while (FindNextFile(hFind1, &FindData1))
	{
		g_StrCpy(&szSrcDir[nSrcLen], FindData1.cFileName);
		g_StrCpy(&szDesDir[nDesLen], FindData1.cFileName);
		hFind2 = FindFirstFile(szDesDir, &FindData2);
		if (hFind2 == INVALID_HANDLE_VALUE)
		{	// û��ͬ���ļ�
			CopyFile(szSrcDir, szDesDir, FALSE);
		}
		else
		{	// ��ͬ���ļ����ͱȽ���ʱ��
			lResult = CompareFileTime(&FindData1.ftLastWriteTime,
				&FindData2.ftLastWriteTime);
			if (lResult != 0)
			{	// ʱ�䲻ͬ�򿽱����ǣ�Ϊȷ�������ɹ���ȥ��Ŀ���ļ���ֻ������
				SetFileAttributes(szDesDir, FILE_ATTRIBUTE_NORMAL);
				CopyFile(szSrcDir, szDesDir, FALSE);
			}
		}
		// ȥ�������������ļ���ֻ������
		SetFileAttributes(szDesDir, FILE_ATTRIBUTE_NORMAL);
		FindClose(hFind2);
	}
	FindClose(hFind1);
}
//---------------------------------------------------------------------------
