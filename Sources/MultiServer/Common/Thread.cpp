#include "stdafx.h"
#include "Thread.h"
#include "Macro.h"

#include <process.h>		//Thread define

#include "Win32Exception.h"

#ifdef JX_PLATFORM_SDL
#include "JxNetShim.h"
/*
 * [SDL 08/09 2b-2] CThread qua SDL_CreateThread / SDL_WaitThread. Wait(timeout) tham co m_nDone
 * (SDL khong co join co han). Terminate: khong ho tro (chi TRACE).
 */
namespace OnlineGameLib {
namespace Win32 {

int JxThreadProcSdl( void *pV )
{
	int result = 0;
	CThread *pThis = ( CThread * )pV;

	if ( pThis )
	{
		try
		{
			result = pThis->Run();
		}
		catch( ... )
		{
			TRACE( "CThread::ThreadFunction exception!" );
		}
		pThis->m_nDone = 1;
	}

	return result;
}

CThread::CThread()
	: m_hThread( NULL )
{
	m_nDone = 0;
}

CThread::~CThread()
{
	if ( m_hThread )
	{
		if ( m_nDone ) SDL_WaitThread( ( SDL_Thread * )m_hThread, NULL );
		else           SDL_DetachThread( ( SDL_Thread * )m_hThread );
		m_hThread = NULL;
	}
}

HANDLE CThread::GetHandle() const
{
	return m_hThread;
}

void CThread::Start()
{
	if ( m_hThread == NULL )
	{
		m_nDone = 0;
		m_hThread = ( HANDLE )SDL_CreateThread( JxThreadProcSdl, "JxNetIO", ( void * )this );

		if ( m_hThread == NULL )
		{
			throw CWin32Exception( _T("CThread::Start() - SDL_CreateThread"), 0 );
		}
	}
	else
	{
		throw CException( _T("CThread::Start()"), _T("Thread already running - you can only call Start() once!") );
	}
}

void CThread::Wait() const
{
	if ( !Wait( INFINITE ) )
	{
		throw CException( _T("CThread::Wait()"), _T("Unexpected timeout on infinite wait") );
	}
}

bool CThread::Wait( DWORD timeoutMillis ) const
{
	if ( !m_hThread )
	{
		return true;
	}

	if ( INFINITE != timeoutMillis )
	{
		Uint64 t0 = SDL_GetTicks();
		while ( !m_nDone )
		{
			if ( SDL_GetTicks() - t0 >= ( Uint64 )timeoutMillis ) return false;
			SDL_Delay( 1 );
		}
	}

	SDL_WaitThread( ( SDL_Thread * )m_hThread, NULL );
	m_hThread = NULL;
	return true;
}

unsigned int __stdcall CThread::ThreadFunction( void *pV )
{
	return ( unsigned int )JxThreadProcSdl( pV );
}

void CThread::Terminate( DWORD /* exitCode */ )
{
	TRACE( "CThread::Terminate: khong ho tro tren SDL" );
}

} // End of namespace Win32
} // End of namespace OnlineGameLib
#else	// JX_PLATFORM_SDL

/*
 * namespace OnlineGameLib::Win32
 */

namespace OnlineGameLib {
namespace Win32 {

CThread::CThread()
	: m_hThread( NULL )
{

}
      
CThread::~CThread()
{
	SAFE_CLOSEHANDLE( m_hThread );
}

HANDLE CThread::GetHandle() const
{
	return m_hThread;
}

void CThread::Start()
{
	if ( m_hThread == NULL )
	{
		unsigned int threadID = 0;

		m_hThread = (HANDLE)::_beginthreadex(0, 
			0, 
			ThreadFunction,
			( void * )this, 
			0, 
			&threadID );

		if ( m_hThread == NULL )
		{
			throw CWin32Exception(_T("CThread::Start() - _beginthreadex"), GetLastError());
		}
	}
	else
	{
		throw CException(_T("CThread::Start()"), _T("Thread already running - you can only call Start() once!"));
	}
}

void CThread::Wait() const
{
	if ( !Wait( INFINITE ) )
	{
		throw CException(_T("CThread::Wait()"), _T("Unexpected timeout on infinite wait"));
	}
}

bool CThread::Wait(DWORD timeoutMillis) const
{
	bool ok;

	if ( !m_hThread )
	{
		return true;
	}

	DWORD result = ::WaitForSingleObject( m_hThread, timeoutMillis );

	if ( result == WAIT_TIMEOUT )
	{
		ok = false;
	}
	else if ( result == WAIT_OBJECT_0 )
	{
		ok = true;
	}
	else
	{
		throw CWin32Exception( _T( "CThread::Wait() - WaitForSingleObject" ), ::GetLastError() );
	}
    
	return ok;
}

unsigned int __stdcall CThread::ThreadFunction( void *pV )
{
	int result = 0;

	CThread* pThis = ( CThread * )pV;
   
	if ( pThis )
	{
		try
		{
			result = pThis->Run();
		}
		catch( ... )
		{
			TRACE( "CThread::ThreadFunction exception!" );
		}
	}

	return result;
}

void CThread::Terminate( DWORD exitCode /* = 0 */ )
{
	if ( m_hThread && !::TerminateThread( m_hThread, exitCode ) )
	{
		TRACE( "CThread::Terminate error!" );
	}

	SAFE_CLOSEHANDLE( m_hThread );
}

} // End of namespace OnlineGameLib
} // End of namespace Win32
#endif	// JX_PLATFORM_SDL [SDL 08/09 2b-2]
