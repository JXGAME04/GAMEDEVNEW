//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMemBase.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Memory base functions
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KDebug.h"
#include "KNode.h"
#include "KList.h"
#include "KMemBase.h"
#include <string.h>
//---------------------------------------------------------------------------
class KMemNode : public KNode
{
public:
	DWORD	m_dwMemSize;//内存大小
	DWORD	m_dwMemSign;//内存标志
};
//---------------------------------------------------------------------------
class KMemList : public KList
{
public:
	~KMemList()
	{
		KMemNode* pNode = (KMemNode*)GetHead();
		while (pNode)
		{
			g_DebugLog("KMemList::Leak Detected, Size = %d", pNode->m_dwMemSize);
			pNode = (KMemNode*)pNode->GetNext();
		}
	};
	void ShowUsage()
	{
		KMemNode* pNode = (KMemNode*)GetHead();
		DWORD dwMemSize = 0;
		while (pNode)
		{
			dwMemSize += pNode->m_dwMemSize;
			pNode = (KMemNode*)pNode->GetNext();
		}
		g_DebugLog("Memory Usage Size = %d KB", dwMemSize / 1024);
	}
};
static KMemList m_MemList;
//---------------------------------------------------------------------------
#define MEMSIGN 1234567890
//---------------------------------------------------------------------------
// 函数:	g_MemInfo
// 功能:	Memory Infomation
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
ENGINE_API void g_MemInfo()
{
//	MEMORYSTATUS stat;
	
//	GlobalMemoryStatus(&stat);
	
//	g_DebugLog("Total Physical Memory = %d MB", stat.dwTotalPhys >> 20);
//	g_DebugLog("Total Virtual Memory = %d MB", stat.dwTotalVirtual >> 20);
//	g_DebugLog("%d percent of memory is in use.", stat.dwMemoryLoad);
}
//---------------------------------------------------------------------------
// 函数:	g_MemAlloc
// 功能:	分配内存
// 参数:	dwSize		内存块大小
// 返回:	lpMem (lpMem = NULL 表示分配失败)
//---------------------------------------------------------------------------
ENGINE_API LPVOID g_MemAlloc(DWORD dwSize)
{
//	HANDLE hHeap = GetProcessHeap();
	PBYTE  lpMem = NULL;
	DWORD  dwHeapSize = dwSize + sizeof(KMemNode);

//	lpMem = (PBYTE)HeapAlloc(hHeap, 0, dwHeapSize);
	lpMem = (PBYTE)new char[dwHeapSize];
	if (NULL == lpMem)
	{
		g_MessageBox("g_MemAlloc() Failed, Size = %d", dwSize);
		return NULL;
	}

	KMemNode* pNode = (KMemNode*)lpMem;
	pNode->m_pPrev = NULL;
	pNode->m_pNext = NULL;
	pNode->m_dwMemSize = dwSize;
	pNode->m_dwMemSign = MEMSIGN;
	m_MemList.AddHead(pNode);
	
	return (lpMem + sizeof(KMemNode));
//	return 0;
}
//---------------------------------------------------------------------------
// 函数:	g_MemFree
// 功能:	释放内存
// 参数:	lpMem		要释放的内存指针
// 返回:	void
//---------------------------------------------------------------------------
ENGINE_API void g_MemFree(LPVOID lpMem)
{
//	HANDLE hHeap = GetProcessHeap();
	if (lpMem == NULL)
		return;
	lpMem = (PBYTE)lpMem - sizeof(KMemNode);
	KMemNode* pNode = (KMemNode *)lpMem;
	if (pNode->m_dwMemSign != MEMSIGN)
	{
		g_MessageBox("g_MemFree() Failed, Size = %d", pNode->m_dwMemSize);
		return;
	}
	pNode->Remove();
//	HeapFree(hHeap, 0, lpMem);
	delete[] lpMem;
}
//---------------------------------------------------------------------------
// 函数:	MemoryCopy
// 功能:	内存拷贝
// 参数:	lpDest	:	目标内存块
//			lpSrc	:	源内存块
//			dwLen	:	拷贝长度
// 返回:	void
//---------------------------------------------------------------------------
ENGINE_API void g_MemCopy(PVOID lpDest, PVOID lpSrc, DWORD dwLen)
{
	memcpy(lpDest, lpSrc, dwLen);
}
//---------------------------------------------------------------------------
// 函数:	MemoryCopyMmx
// 功能:	内存拷贝（MMX版本）
// 参数:	lpDest	:	目标内存块
//			lpSrc	:	源内存块
//			dwLen	:	拷贝长度
// 返回:	void
//---------------------------------------------------------------------------
ENGINE_API void g_MemCopyMmx(PVOID lpDest, PVOID lpSrc, DWORD dwLen)
{
	memcpy(lpDest, lpSrc, dwLen);
}
//---------------------------------------------------------------------------
// 函数:	MemoryComp
// 功能:	内存比较
// 参数:	lpDest	:	内存块1
//			lpSrc	:	内存块2
//			dwLen	:	比较长度
// 返回:	TRUE	:	相同
//			FALSE	:	不同	
//---------------------------------------------------------------------------
ENGINE_API BOOL g_MemComp(PVOID lpDest, PVOID lpSrc, DWORD dwLen)
{
	return (0 == memcmp(lpDest, lpSrc, dwLen));
}
//---------------------------------------------------------------------------
// 函数:	MemoryFill
// 功能:	内存填充
// 参数:	lpDest	:	内存地址
//			dwLen	:	内存长度
//			byFill	:	填充字节
// 返回:	void
//---------------------------------------------------------------------------
ENGINE_API void g_MemFill(PVOID lpDest, DWORD dwLen, BYTE byFill)
{
	memset(lpDest, byFill, dwLen);
}
//---------------------------------------------------------------------------
// 函数:	MemoryFill
// 功能:	内存填充
// 参数:	lpDest	:	内存地址
//			dwLen	:	内存长度
//			wFill	:	填充字
// 返回:	void
//---------------------------------------------------------------------------
ENGINE_API void g_MemFill(PVOID lpDest, DWORD dwLen, WORD wFill)
{
     memset(lpDest, wFill & 0xff, dwLen);
}
//---------------------------------------------------------------------------
// 函数:	MemoryFill
// 功能:	内存填充
// 参数:	lpDest	:	内存地址
//			dwLen	:	内存长度
//			dwFill	:	填充字
// 返回:	void
//---------------------------------------------------------------------------
ENGINE_API void g_MemFill(PVOID lpDest, DWORD dwLen, DWORD dwFill)
{
     memset(lpDest, dwFill & 0xff, dwLen);
}
//---------------------------------------------------------------------------
// 函数:	MemoryZero
// 功能:	内存清零
// 参数:	lpDest	:	内存地址
//			dwLen	:	内存长度
// 返回:	void
//---------------------------------------------------------------------------
ENGINE_API void g_MemZero(PVOID lpDest, DWORD dwLen)
{
     memset(lpDest, 0, dwLen);
}
//---------------------------------------------------------------------------
// 函数:	MemoryXore
// 功能:	内存异或
// 参数:	lpDest	:	内存地址
//			dwLen	:	内存长度
//			dwXor	:	异或字节
// 返回:	void
//---------------------------------------------------------------------------
ENGINE_API void g_MemXore(PVOID lpDest, DWORD dwLen, DWORD dwXor)
{
     unsigned long *ptr = (unsigned long *)lpDest;
     while((long)dwLen > 0) {
       *ptr++ ^= dwXor;
       dwLen -= sizeof(unsigned long);
     }
}
