/********************************************************************
	created:	2003/02/14
	file base:	Thread
	file ext:	h
	author:		liupeng
	
	purpose:	
*********************************************************************/
#ifndef __INCLUDE_THREAD_H__
#define __INCLUDE_THREAD_H__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
	#pragma once
#endif

#ifndef _WINDOWS_
	#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#undef WIN32_LEAN_AND_MEAN
#endif

/*
 * namespace OnlineGameLib::Win32
 */

namespace OnlineGameLib {
namespace Win32 {

/*
 * CThread
 */
class CThread 
{
public:
   
	CThread();
      
	virtual ~CThread();

	HANDLE GetHandle() const;

	void Wait() const;

	bool Wait(DWORD timeoutMillis) const;

	void Start();

	void Terminate( DWORD exitCode = 0 );

private:

	virtual int Run() = 0;

	static unsigned int __stdcall ThreadFunction( void *pV );

#ifdef JX_PLATFORM_SDL
	mutable HANDLE m_hThread;	// [SDL 08/09 2b-2] SDL_Thread*; mutable de Wait() const thu hoi luong
	volatile int m_nDone;
	friend int JxThreadProcSdl( void *pV );
#else
	HANDLE m_hThread;
#endif

	/*
	 * No copies do not implement
	 */
	CThread( const CThread &rhs );
	CThread &operator=( const CThread &rhs );

};

} // End of namespace OnlineGameLib
} // End of namespace Win32

#endif //__INCLUDE_THREAD_H__