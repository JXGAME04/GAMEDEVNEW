//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMemBase.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KMemBase_H
#define KMemBase_H
//---------------------------------------------------------------------------
ENGINE_API	void	g_MemInfo(void);
ENGINE_API	void*	g_MemAlloc(DWORD dwSize);
ENGINE_API	void	g_MemFree(LPVOID lpMem);
ENGINE_API	void	g_MemCopy(LPVOID lpDest, LPVOID lpSrc, DWORD dwLen);
inline void g_MemCopy(LPVOID lpDest, LPCVOID lpSrc, DWORD dwLen) { g_MemCopy(lpDest, (LPVOID)lpSrc, dwLen); }	// [CLANG 08/09] nguon const (clang); overload inline, khong doi ham xuat
ENGINE_API	void	g_MemCopyMmx(LPVOID lpDest, LPVOID lpSrc, DWORD dwLen);
ENGINE_API	BOOL	g_MemComp(LPVOID lpDest, LPVOID lpSrc, DWORD dwLen);
inline BOOL g_MemComp(LPCVOID lpDest, LPCVOID lpSrc, DWORD dwLen) { return g_MemComp((LPVOID)lpDest, (LPVOID)lpSrc, dwLen); }	// [CLANG 08/09] goi voi chuoi hang (const) - clang khong cho const->non-const; overload inline, khong doi ham xuat
ENGINE_API	void	g_MemFill(LPVOID lpDest, DWORD dwLen, BYTE byFill);
ENGINE_API	void	g_MemFill(LPVOID lpDest, DWORD dwLen, WORD wFill);
ENGINE_API	void	g_MemFill(LPVOID lpDest, DWORD dwLen, DWORD dwFill);
ENGINE_API	void	g_MemZero(LPVOID lpDest, DWORD dwLen);
ENGINE_API	void	g_MemXore(LPVOID lpDest, DWORD dwLen, DWORD dwXor);
//---------------------------------------------------------------------------
#endif
