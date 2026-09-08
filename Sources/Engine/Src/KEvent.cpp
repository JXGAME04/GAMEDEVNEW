//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KEvent.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Implements a simple event object for thread synchronization
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KDebug.h"
#include "KEvent.h"
//---------------------------------------------------------------------------
// 函数:	KEvent
// 功能:	购造函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KEvent::KEvent()
{
#ifdef JX_PLATFORM_SDL	// [SDL 08/09 2b-1] event auto-reset ~ semaphore (HANDLE chua SDL_Semaphore*)
    m_hEvent = (HANDLE)SDL_CreateSemaphore(0);
#else
    m_hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
#endif
    if (!m_hEvent)
		g_DebugLog("KEvent::KEvent(): CreateEvent() failed!");
}
//---------------------------------------------------------------------------
// 函数:	~KEvent
// 功能:	析购函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KEvent::~KEvent()
{
#ifdef JX_PLATFORM_SDL
    SDL_DestroySemaphore((SDL_Semaphore*)m_hEvent);
#else
    CloseHandle(m_hEvent);
#endif
}
//---------------------------------------------------------------------------
// 函数:	Signal
// 功能:	
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
inline void KEvent::Signal(void)
{
#ifdef JX_PLATFORM_SDL
    if (SDL_GetSemaphoreValue((SDL_Semaphore*)m_hEvent) == 0) SDL_SignalSemaphore((SDL_Semaphore*)m_hEvent);	// auto-reset: khong don
#else
    SetEvent(m_hEvent);
#endif
}
//---------------------------------------------------------------------------
// 函数:	Wait
// 功能:	
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
inline void KEvent::Wait(void)
{
#ifdef JX_PLATFORM_SDL
    SDL_WaitSemaphore((SDL_Semaphore*)m_hEvent);
#else
    WaitForSingleObject(m_hEvent,INFINITE);
#endif
}
//---------------------------------------------------------------------------
// 函数:	TimedWait
// 功能:	
// 参数:	long ms
// 返回:	void
//---------------------------------------------------------------------------
inline bool KEvent::TimedWait(long ms)
{
#ifdef JX_PLATFORM_SDL
    return SDL_WaitSemaphoreTimeout((SDL_Semaphore*)m_hEvent, (Sint32)ms);
#else
    int r = WaitForSingleObject(m_hEvent, ms);
    return (WAIT_TIMEOUT == r) ? false : true;
#endif
}
//---------------------------------------------------------------------------
