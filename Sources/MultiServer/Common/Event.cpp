#include "stdafx.h"
#include "Event.h"
#include "Win32Exception.h"

#ifdef JX_PLATFORM_SDL
#include "JxNetShim.h"
/*
 * [SDL 08/09 2b-2] CEvent (manual/auto-reset, cho co han) bang SDL_Mutex + SDL_Condition.
 * m_hEvent giu con tro JxSdlEvent. Pulse dung bo dem the he de danh thuc moi luong dang cho.
 */
namespace OnlineGameLib {
namespace Win32 {

struct JxSdlEvent
{
	SDL_Mutex		*mu;
	SDL_Condition	*cv;
	bool			manual;
	bool			state;
	unsigned		gen;
};

static HANDLE JxCreateEvent( bool bManualReset, bool bInitialState )
{
	JxSdlEvent *e = new JxSdlEvent;
	e->mu = SDL_CreateMutex(); e->cv = SDL_CreateCondition();
	e->manual = bManualReset; e->state = bInitialState; e->gen = 0;

	if ( !e->mu || !e->cv )
	{
		throw CWin32Exception( _T("CEvent::Create() - SDL"), 0 );
	}

	return ( HANDLE )e;
}

CEvent::CEvent(
		   LPSECURITY_ATTRIBUTES /* lpEventAttributes */,
		   bool bManualReset,
		   bool bInitialState)
		   : m_hEvent( JxCreateEvent( bManualReset, bInitialState ) )
{
}

CEvent::CEvent(
		   LPSECURITY_ATTRIBUTES /* lpEventAttributes */,
		   bool bManualReset,
		   bool bInitialState,
		   const char * /* pEventName */ )
		   : m_hEvent( JxCreateEvent( bManualReset, bInitialState ) )
{
}

CEvent::~CEvent()
{
	JxSdlEvent *e = ( JxSdlEvent * )m_hEvent;
	if ( e )
	{
		SDL_DestroyCondition( e->cv );
		SDL_DestroyMutex( e->mu );
		delete e;
	}
}

void CEvent::Wait() const
{
	if ( !Wait( INFINITE ) )
	{
		throw CException( _T("CEvent::Wait()"), _T("Unexpected timeout on infinite wait") );
	}
}

bool CEvent::Wait( DWORD timeoutMillis ) const
{
	JxSdlEvent *e = ( JxSdlEvent * )m_hEvent;
	bool ok = false;

	SDL_LockMutex( e->mu );

	const unsigned gen0 = e->gen;

	if ( !e->state && 0 != timeoutMillis )
	{
		if ( INFINITE == timeoutMillis )
		{
			while ( !e->state && e->gen == gen0 ) SDL_WaitCondition( e->cv, e->mu );
		}
		else
		{
			Uint64 t0 = SDL_GetTicks();
			while ( !e->state && e->gen == gen0 )
			{
				Uint64 dt = SDL_GetTicks() - t0;
				if ( dt >= ( Uint64 )timeoutMillis ) break;
				SDL_WaitConditionTimeout( e->cv, e->mu, ( Sint32 )( timeoutMillis - dt ) );
			}
		}
	}

	ok = e->state || e->gen != gen0;
	if ( e->state && !e->manual ) e->state = false;

	SDL_UnlockMutex( e->mu );

	return ok;
}

void CEvent::Reset()
{
	JxSdlEvent *e = ( JxSdlEvent * )m_hEvent;
	SDL_LockMutex( e->mu );
	e->state = false;
	SDL_UnlockMutex( e->mu );
}

void CEvent::Set()
{
	JxSdlEvent *e = ( JxSdlEvent * )m_hEvent;
	SDL_LockMutex( e->mu );
	e->state = true;
	SDL_BroadcastCondition( e->cv );
	SDL_UnlockMutex( e->mu );
}

void CEvent::Pulse()
{
	JxSdlEvent *e = ( JxSdlEvent * )m_hEvent;
	SDL_LockMutex( e->mu );
	e->gen++;
	SDL_BroadcastCondition( e->cv );
	e->state = false;
	SDL_UnlockMutex( e->mu );
}

} // End of namespace Win32
} // End of namespace OnlineGameLib
#else	// JX_PLATFORM_SDL

/*
 * namespace OnlineGameLib::Win32
 */

namespace OnlineGameLib {
namespace Win32 {

/*
 * Static helper methods
 */
static HANDLE Create(
				LPSECURITY_ATTRIBUTES lpEventAttributes, 
				bool bManualReset, 
				bool bInitialState, 
				LPCTSTR lpName);	

static HANDLE Create(
				LPSECURITY_ATTRIBUTES lpEventAttributes, 
				bool bManualReset, 
				bool bInitialState, 
				LPCTSTR lpName)
{
	HANDLE hEvent = ::CreateEvent( lpEventAttributes, bManualReset, bInitialState, lpName );
	
	if ( NULL == hEvent )
	{
		throw CWin32Exception( _T("CEvent::Create()"), ::GetLastError() );
	}
	
	return hEvent;
}

CEvent::CEvent(
		   LPSECURITY_ATTRIBUTES lpEventAttributes, 
		   bool bManualReset, 
		   bool bInitialState)
		   : m_hEvent( Create( lpEventAttributes, bManualReset, bInitialState, NULL ) )
{
	
}

CEvent::CEvent(
		   LPSECURITY_ATTRIBUTES lpEventAttributes, 
		   bool bManualReset, 
		   bool bInitialState, 
		   const char *pEventName )
		   : m_hEvent( Create( lpEventAttributes, bManualReset, bInitialState, pEventName ) )
{
	
}

CEvent::~CEvent()
{
	::CloseHandle( m_hEvent );
}

void CEvent::Wait() const
{
	if ( !Wait( INFINITE ) )
	{
		throw CException( _T("CEvent::Wait()"), _T("Unexpected timeout on infinite wait") );
	}
}

bool CEvent::Wait( DWORD timeoutMillis ) const
{
	bool ok;
	
	DWORD result = ::WaitForSingleObject( m_hEvent, timeoutMillis );
	
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
		throw CWin32Exception( _T("CEvent::Wait() - WaitForSingleObject"), ::GetLastError() );
	}
    
	return ok;
}

void CEvent::Reset()
{
	if ( !::ResetEvent( m_hEvent ) )
	{
		throw CWin32Exception( _T("CEvent::Reset()"), ::GetLastError() );
	}
}

void CEvent::Set()
{
	if ( !::SetEvent( m_hEvent ) )
	{
		throw CWin32Exception( _T("CEvent::Set()"), ::GetLastError() );
	}
}

void CEvent::Pulse()
{
	if ( !::PulseEvent( m_hEvent ) )
	{
		throw CWin32Exception( _T("CEvent::Pulse()"), ::GetLastError() );
	}
}
	
} // End of namespace OnlineGameLib
} // End of namespace Win32
#endif	// JX_PLATFORM_SDL [SDL 08/09 2b-2]
