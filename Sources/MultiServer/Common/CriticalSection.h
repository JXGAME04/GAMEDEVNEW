/********************************************************************
	created:	2003/02/13
	file base:	CriticalSection
	file ext:	h
	author:		liupeng
	
	purpose:	Header file for CRITICAL_SECTION class
*********************************************************************/
#ifndef __INCLUDE_CRITICALSECTION_H__
#define __INCLUDE_CRITICALSECTION_H__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
	#pragma once
#endif

#ifndef _WINDOWS_
	#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#undef WIN32_LEAN_AND_MEAN
#endif

#include "Utils.h"
#ifdef JX_PLATFORM_SDL
#include "JxNetShim.h"
#endif

/*
 * namespace OnlineGameLib::Win32
 */

namespace OnlineGameLib {
namespace Win32 {

/*
 * CCriticalSection
 */
class CCriticalSection 
{
public:
	
	class Owner
	{
	public:
		
		explicit Owner( CCriticalSection &crit );
		
		~Owner();
		
	private:
		
		CCriticalSection &m_crit;
		
		/*
		 * No copies do not implement
		 */
		Owner( const Owner &rhs );
		Owner &operator=( const Owner &rhs );
	};
	
	CCriticalSection();
	
	~CCriticalSection();

#if ( _WIN32_WINNT >= 0x0400 )
	bool TryEnter();
#endif
	  
    void Enter();
	  
    void Leave();
	  
private:
		
#ifdef JX_PLATFORM_SDL
	void *m_crit;	// [SDL 08/09 2b-2] SDL_Mutex* (de quy nhu CRITICAL_SECTION)
#else
	CRITICAL_SECTION	m_crit;
#endif
		
	/*
	 * No copies do not implement
	 */
	CCriticalSection( const CCriticalSection &rhs );
	CCriticalSection &operator=( const CCriticalSection &rhs );

};

#ifdef JX_PLATFORM_SDL
inline void CCriticalSection::Enter() { SDL_LockMutex( ( SDL_Mutex * )m_crit ); }
inline void CCriticalSection::Leave() { SDL_UnlockMutex( ( SDL_Mutex * )m_crit ); }
inline CCriticalSection::CCriticalSection() { m_crit = SDL_CreateMutex(); }
inline CCriticalSection::~CCriticalSection() { if ( m_crit ) SDL_DestroyMutex( ( SDL_Mutex * )m_crit ); m_crit = NULL; }
#if ( _WIN32_WINNT >= 0x0400 )
inline bool CCriticalSection::TryEnter() { return SDL_TryLockMutex( ( SDL_Mutex * )m_crit ); }
#endif
#else	// JX_PLATFORM_SDL
inline void CCriticalSection::Enter()
{
	::EnterCriticalSection( &m_crit );
}

inline void CCriticalSection::Leave()
{
	::LeaveCriticalSection( &m_crit );
}

inline CCriticalSection::CCriticalSection()
{
	::InitializeCriticalSection( &m_crit );
}
      
inline CCriticalSection::~CCriticalSection()
{
	::DeleteCriticalSection( &m_crit );
}

#if ( _WIN32_WINNT >= 0x0400 )
inline bool CCriticalSection::TryEnter()
{
	return BOOL_to_bool( ::TryEnterCriticalSection( &m_crit ) );
}
#endif
#endif	// JX_PLATFORM_SDL [SDL 08/09 2b-2]

/*
 * CCriticalSection::Owner
 */

inline CCriticalSection::Owner::Owner( CCriticalSection &crit )
		: m_crit( crit )
{
	m_crit.Enter();
}

inline CCriticalSection::Owner::~Owner()
{
	m_crit.Leave();
}

} // End of namespace Win32 	
} // End of namespace OnlineGameLib

#endif //__INCLUDE_CRITICALSECTION_H__