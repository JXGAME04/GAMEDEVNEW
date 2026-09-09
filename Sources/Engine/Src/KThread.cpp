//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KThread.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	BG2132 Font Class
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KDebug.h"
#include "KMemBase.h"
#include "KThread.h"
#if defined(WIN32) || defined(JX_PLATFORM_SDL)
#include "process.h"
#endif

KThread::KThread()
{
#if defined(WIN32) || defined(JX_PLATFORM_SDL)
	m_ThreadHandle	= NULL;
	m_ThreadId		= 0;
	m_ThreadFunc	= NULL;
	m_ThreadParam	= NULL;
#endif
}
//---------------------------------------------------------------------------
// 函数:	~KThread
// 功能:	析购函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KThread::~KThread()
{
#if defined(WIN32) || defined(JX_PLATFORM_SDL)
	if (m_ThreadHandle)
	{
		CloseHandle(m_ThreadHandle);
		m_ThreadHandle	= NULL;
	}
#else

#endif
}
//---------------------------------------------------------------------------
// 函数:	MyThreadProc
// 功能:	线程过程
// 参数:	lpParam		参数
// 返回:	void
//---------------------------------------------------------------------------
#if defined(WIN32) || defined(JX_PLATFORM_SDL)
unsigned __stdcall MyThreadProc(LPVOID lpParam)
#else
void * MyThreadProc(LPVOID lpParam)
#endif
{
 printf("start thread %d\n", lpParam);
	KThread* pThread = (KThread*)lpParam;
#if defined(WIN32) || defined(JX_PLATFORM_SDL)
	return pThread->ThreadFunction();
#else
	return (void *)pThread->ThreadFunction();
#endif
//	return 0;
}
//---------------------------------------------------------------------------
// 函数:	ThreadFunction
// 功能:	析购函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
DWORD KThread::ThreadFunction()
{
	m_ThreadFunc(m_ThreadParam);
#ifdef JX_PLATFORM_SDL
	m_ThreadId = 0;	// [SDL 08/09 2b-1] luong da xong (IsRunning)
#endif
	return 0;
}
//---------------------------------------------------------------------------
// 函数:	Create
// 功能:	创建线程
// 参数:	func		线程函数
//			arg 		线程参数
// 返回:	BOOL
//---------------------------------------------------------------------------
#ifdef JX_PLATFORM_SDL
// [SDL 08/09 2b-1] luong SDL: HANDLE m_ThreadHandle chua SDL_Thread*; m_ThreadId = 1 khi dang chay, 0 khi xong
static int SDLCALL KThread_SdlProc(void* pParam)
{
	KThread* pThread = (KThread*)pParam;
	pThread->ThreadFunction();	// ThreadFunction tu dat m_ThreadId = 0 khi xong
	return 0;
}
#endif
BOOL KThread::Create(TThreadFunc lpFunc, void* lpParam)
{
#if defined(JX_PLATFORM_SDL)
	m_ThreadFunc   = lpFunc;
	m_ThreadParam  = lpParam;
	m_ThreadId     = 1;
	m_ThreadHandle = (HANDLE)SDL_CreateThread(KThread_SdlProc, "KThread", this);
	if (!m_ThreadHandle) m_ThreadId = 0;
	return (m_ThreadHandle != NULL);
#elif defined(WIN32)
	m_ThreadFunc   = lpFunc;
	m_ThreadParam  = lpParam;
	m_ThreadHandle = (HANDLE)_beginthreadex(
		NULL,			// SD
		0,				// initial stack size
		MyThreadProc,		// thread function
		this,			// thread argument
		0,				// creation option
		(unsigned*)&m_ThreadId);	// thread identifier
	return (m_ThreadHandle != NULL);
#else
     m_ThreadFunc   = lpFunc;
     int ret = pthread_create(&p_thread, NULL, MyThreadProc, this);
     printf("create thread %d return %d\n", (int)p_thread, ret);
#endif
//	return FALSE;
}
//---------------------------------------------------------------------------
// 函数:	Destroy
// 功能:	销毁线程
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KThread::Destroy()
{
#if defined(JX_PLATFORM_SDL)
	SDL_Log("[SDL] KThread::Destroy: SDL khong co TerminateThread - luong phai tu thoat bang co");
#elif defined(WIN32)
	TerminateThread(m_ThreadHandle, 0);
#endif
}
//---------------------------------------------------------------------------
// 函数:	Suspend
// 功能:	挂起线程
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KThread::Suspend()
{
#if defined(JX_PLATFORM_SDL)
	SDL_Log("[SDL] KThread::Suspend: khong ho tro");
#elif defined(WIN32)
	 SuspendThread(m_ThreadHandle);
#endif
}
//---------------------------------------------------------------------------
// 函数:	Resume
// 功能:	唤醒线程
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KThread::Resume()
{
#if defined(JX_PLATFORM_SDL)
	SDL_Log("[SDL] KThread::Resume: khong ho tro");
#elif defined(WIN32)
	ResumeThread(m_ThreadHandle);
#endif
}
//---------------------------------------------------------------------------
// 函数:	IsRunning
// 功能:	判断线程是否在运行
// 参数:	void
// 返回:	BOOL
//---------------------------------------------------------------------------
BOOL KThread::IsRunning()
{
#if defined(JX_PLATFORM_SDL)
	return (m_ThreadHandle != NULL && m_ThreadId != 0);
#elif defined(WIN32)
	DWORD dwResult = WaitForSingleObject(m_ThreadHandle, 0);
	return (dwResult == WAIT_OBJECT_0);
#endif
//	return FALSE;
}
//---------------------------------------------------------------------------
// 函数:	WaitForExit
// 功能:	等待线程结束
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KThread::WaitForExit()
{
#if defined(JX_PLATFORM_SDL)
	if (m_ThreadHandle) { SDL_WaitThread((SDL_Thread*)m_ThreadHandle, NULL); m_ThreadHandle = NULL; }
#elif defined(WIN32)
	WaitForSingleObject(m_ThreadHandle, INFINITE);
#endif
}
//---------------------------------------------------------------------------
// 函数:	GetPriority
// 功能:	取得线程优先级
// 参数:	void
// 返回:	int
//---------------------------------------------------------------------------
int KThread::GetPriority()
{
#if defined(JX_PLATFORM_SDL)
	return 0;
#elif defined(WIN32)
	return GetThreadPriority(m_ThreadHandle);
#endif
//	return 0;
}
//---------------------------------------------------------------------------
// 函数:	SetPriority
// 功能:	设置线程优先级
// 参数:	void
// 返回:	BOOL
//---------------------------------------------------------------------------
BOOL KThread::SetPriority(int priority)
{
#if defined(WIN32) || defined(JX_PLATFORM_SDL)
	return SetThreadPriority(m_ThreadHandle, priority);
#endif
//	return FALSE;
}
