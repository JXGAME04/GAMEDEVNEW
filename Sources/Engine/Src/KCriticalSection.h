//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KCriticalSection.h
//  Version     :   1.0
//  Creater     :   Freeway Chen
//  Date        :   2003-9-2 17:17:16
//  Comment     :   参考了刘鹏的的实现 
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KCRITICALSECTION_H_
#define _KCRITICALSECTION_H_    1

#include "windows.h"

#ifdef JX_PLATFORM_SDL
#include <SDL3/SDL.h>
// [SDL 08/09 2b-1] SDL_Mutex (reentrant nhu CRITICAL_SECTION)
class KCriticalSection
{
private:
	SDL_Mutex*	m_pMutex;
public:
    KCriticalSection() { m_pMutex = SDL_CreateMutex(); }
    ~KCriticalSection() { if (m_pMutex) SDL_DestroyMutex(m_pMutex); m_pMutex = NULL; }
    int Lock() { SDL_LockMutex(m_pMutex); return true; }
    int UnLock() { SDL_UnlockMutex(m_pMutex); return false; }
};
#else
class KCriticalSection
{

private:
	CRITICAL_SECTION	m_CriticalSection;

public:
    KCriticalSection()
    {
        InitializeCriticalSection(&m_CriticalSection);
    }

    ~KCriticalSection()
    {
        DeleteCriticalSection(&m_CriticalSection);
    }

    int Lock()
    {
        EnterCriticalSection(&m_CriticalSection);
        return true;
    }

    int UnLock()
    {
        LeaveCriticalSection(&m_CriticalSection);

        return false;
    }
};
#endif

class KAutoCriticalSection
{
    KCriticalSection &m_CriticalSection;

public:
    KAutoCriticalSection(KCriticalSection &CriticalSection)
        : m_CriticalSection(CriticalSection)
    {
        m_CriticalSection.Lock();
    }

    ~KAutoCriticalSection()
    {
        m_CriticalSection.UnLock();
    }
};

#endif

