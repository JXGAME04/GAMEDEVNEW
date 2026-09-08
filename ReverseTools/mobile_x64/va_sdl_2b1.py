# -*- coding: utf-8 -*-
"""[SDL 08/09 2b-1] Nhanh JX_PLATFORM_SDL (thu ba, ben canh WIN32 va __linux) trong Engine:
 - KWin32.h: cuoi tep (ban Win32) them include SDL3 + shim timeGetTime/GetTickCount -> SDL_GetTicks, Sleep -> SDL_Delay
 - KTimer.cpp: SDL_GetPerformanceCounter/Frequency (giu LARGE_INTEGER.QuadPart)
 - KCriticalSection.h: SDL_Mutex (reentrant nhu CRITICAL_SECTION)
 - KMutex.cpp: SDL_Mutex (SINGLE_PROCESS)
 - KEvent.cpp: SDL_Semaphore (auto-reset event ~ semaphore dem 0/1)
 - KThread.cpp: SDL_CreateThread/SDL_WaitThread; Destroy/Suspend/Resume = ghi log (SDL khong co TerminateThread)
 - KDebug.cpp: g_DebugLog -> SDL_Log (them DebugWin neu co), g_MessageBox -> SDL_ShowSimpleMessageBox
Ban Win32/x64 thuong: khong doi mot byte (moi khoi deu trong #ifdef JX_PLATFORM_SDL). Byte-safe, chay lai an toan."""
import io, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
E = ROOT + "\\Sources\\Engine\\Src\\"
TAG = b"[SDL 08/09 2b-1]"
def rd(p): return io.open(p, "rb").read()
def wr(p, b, nb):
    assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127), p
    io.open(p, "wb").write(nb)
def nlof(b): return b"\r\n" if b"\r\n" in b else b"\n"
def J(nl, lines): return nl.join(lines) + nl
def rep1(b, a, c):
    n = b.count(a); assert n == 1, (a[:50], n); return b.replace(a, c)

# 1. KWin32.h ---------------------------------------------------------------
p = E + "KWin32.h"; b = rd(p)
if TAG not in b:
    nl = nlof(b)
    # neo: sau '#include <ddraw.h>' (dong 41) - con trong nhanh Win32
    a = b"        #include <ddraw.h>" + nl
    ins = a + J(nl, [
        b"#ifdef JX_PLATFORM_SDL",
        b"\t// " + TAG + b" ban SDL (GameSDL.exe / mobile): thoi gian, luong, khoa, tep di qua SDL3 ngay ca tren Windows de kiem truoc khi sang Android.",
        b"\t#include <SDL3/SDL.h>",
        b"\t#define timeGetTime()   ((DWORD)SDL_GetTicks())",
        b"\t#define GetTickCount()  ((DWORD)SDL_GetTicks())",
        b"\t#define Sleep(ms)       SDL_Delay((Uint32)(ms))",
        b"#endif"])
    b2 = rep1(b, a, ins); wr(p, b, b2); print("KWin32.h: shim SDL")
else: print("KWin32.h: da va")

# 2. KTimer.cpp -------------------------------------------------------------
p = E + "KTimer.cpp"; b = rd(p)
if TAG not in b:
    nl = nlof(b)
    b = rep1(b, b"KTimer::KTimer()" + nl + b"{" + nl + b"#ifdef WIN32" + nl + b"\tm_nFrequency.QuadPart = 200 * 1024 * 1024;",
             b"KTimer::KTimer()" + nl + b"{" + nl + b"#if defined(JX_PLATFORM_SDL)\t// " + TAG + nl + b"\tm_nTimeStart.QuadPart = 0;" + nl + b"\tm_nTimeStop.QuadPart = 0;" + nl + b"\tm_nFPS = 0;" + nl + b"\tm_nFrequency.QuadPart = (LONGLONG)SDL_GetPerformanceFrequency();" + nl + b"#elif defined(WIN32)" + nl + b"\tm_nFrequency.QuadPart = 200 * 1024 * 1024;")
    b = rep1(b, b"void KTimer::Start()" + nl + b"{" + nl + b"#ifdef WIN32" + nl + b"\tQueryPerformanceCounter(&m_nTimeStart);",
             b"void KTimer::Start()" + nl + b"{" + nl + b"#if defined(JX_PLATFORM_SDL)" + nl + b"\tm_nTimeStart.QuadPart = (LONGLONG)SDL_GetPerformanceCounter();" + nl + b"#elif defined(WIN32)" + nl + b"\tQueryPerformanceCounter(&m_nTimeStart);")
    b = rep1(b, b"void KTimer::Stop()" + nl + b"{" + nl + b"#ifdef WIN32" + nl + b"\tQueryPerformanceCounter(&m_nTimeStop);",
             b"void KTimer::Stop()" + nl + b"{" + nl + b"#if defined(JX_PLATFORM_SDL)" + nl + b"\tm_nTimeStop.QuadPart = (LONGLONG)SDL_GetPerformanceCounter();" + nl + b"#elif defined(WIN32)" + nl + b"\tQueryPerformanceCounter(&m_nTimeStop);")
    b = rep1(b, b"DWORD KTimer::GetElapse()" + nl + b"{" + nl + b"#ifdef WIN32" + nl + b"\tLARGE_INTEGER nTime;" + nl + b"\tQueryPerformanceCounter(&nTime);",
             b"DWORD KTimer::GetElapse()" + nl + b"{" + nl + b"#if defined(JX_PLATFORM_SDL)" + nl + b"\treturn (DWORD)(((LONGLONG)SDL_GetPerformanceCounter() - m_nTimeStart.QuadPart) * 1000 / m_nFrequency.QuadPart);" + nl + b"#elif defined(WIN32)" + nl + b"\tLARGE_INTEGER nTime;" + nl + b"\tQueryPerformanceCounter(&nTime);")
    b = rep1(b, b"DWORD KTimer::GetElapseFrequency()" + nl + b"{" + nl + b"#ifdef WIN32" + nl + b"\tLARGE_INTEGER nTime;" + nl + b"\tQueryPerformanceCounter(&nTime);",
             b"DWORD KTimer::GetElapseFrequency()" + nl + b"{" + nl + b"#if defined(JX_PLATFORM_SDL)" + nl + b"\treturn (DWORD)((LONGLONG)SDL_GetPerformanceCounter() - m_nTimeStart.QuadPart);" + nl + b"#elif defined(WIN32)" + nl + b"\tLARGE_INTEGER nTime;" + nl + b"\tQueryPerformanceCounter(&nTime);")
    wr(p, rd(p), b); print("KTimer.cpp: SDL")
else: print("KTimer.cpp: da va")
# cac ham con lai cua KTimer (GetInterval/Passed/GetFPS) dung QPC? kiem sau khi build

# 3. KCriticalSection.h ----------------------------------------------------
p = E + "KCriticalSection.h"; b = rd(p)
if TAG not in b:
    nl = nlof(b)
    old = J(nl, [b"class KCriticalSection", b"{", b"", b"private:", b"\tCRITICAL_SECTION\tm_CriticalSection;", b"", b"public:",
                 b"    KCriticalSection()", b"    {", b"        InitializeCriticalSection(&m_CriticalSection);", b"    }", b"",
                 b"    ~KCriticalSection()", b"    {", b"        DeleteCriticalSection(&m_CriticalSection);", b"    }", b"",
                 b"    int Lock()", b"    {", b"        EnterCriticalSection(&m_CriticalSection);", b"        return true;", b"    }", b"",
                 b"    int UnLock()", b"    {", b"        LeaveCriticalSection(&m_CriticalSection);", b"", b"        return false;", b"    }", b"};"])
    new = J(nl, [b"#ifdef JX_PLATFORM_SDL", b"// " + TAG + b" SDL_Mutex (reentrant nhu CRITICAL_SECTION)", b"class KCriticalSection", b"{", b"private:",
                 b"\tSDL_Mutex*\tm_pMutex;", b"public:", b"    KCriticalSection() { m_pMutex = SDL_CreateMutex(); }",
                 b"    ~KCriticalSection() { if (m_pMutex) SDL_DestroyMutex(m_pMutex); m_pMutex = NULL; }",
                 b"    int Lock() { SDL_LockMutex(m_pMutex); return true; }",
                 b"    int UnLock() { SDL_UnlockMutex(m_pMutex); return false; }", b"};", b"#else"]) + old + b"#endif" + nl
    wr(p, b, rep1(b, old, new)); print("KCriticalSection.h: SDL")
else: print("KCriticalSection.h: da va")

# 4. KMutex.cpp -------------------------------------------------------------
p = E + "KMutex.cpp"; b = rd(p)
if TAG not in b:
    nl = nlof(b)
    b = rep1(b, b"KMutex::KMutex()" + nl + b"{" + nl + b"#ifdef WIN32" + nl + b"#ifdef SINGLE_PROCESS" + nl + b"\tInitializeCriticalSection(&m_CriticalSection);",
             b"KMutex::KMutex()" + nl + b"{" + nl + b"#if defined(JX_PLATFORM_SDL)\t// " + TAG + b" (m_CriticalSection dung lam cho chua con tro SDL_Mutex, xem KMutex.h)" + nl + b"\t*(SDL_Mutex**)&m_CriticalSection = SDL_CreateMutex();" + nl + b"#elif defined(WIN32)" + nl + b"#ifdef SINGLE_PROCESS" + nl + b"\tInitializeCriticalSection(&m_CriticalSection);")
    b = rep1(b, b"KMutex::~KMutex()" + nl + b"{" + nl + b"#ifdef WIN32" + nl + b"#ifdef SINGLE_PROCESS" + nl + b"\tDeleteCriticalSection(&m_CriticalSection);",
             b"KMutex::~KMutex()" + nl + b"{" + nl + b"#if defined(JX_PLATFORM_SDL)" + nl + b"\tSDL_DestroyMutex(*(SDL_Mutex**)&m_CriticalSection);" + nl + b"#elif defined(WIN32)" + nl + b"#ifdef SINGLE_PROCESS" + nl + b"\tDeleteCriticalSection(&m_CriticalSection);")
    b = rep1(b, b"void KMutex::Lock(void)" + nl + b"{" + nl + b"#ifdef WIN32" + nl + b"#ifdef SINGLE_PROCESS" + nl + b"\tEnterCriticalSection(&m_CriticalSection);",
             b"void KMutex::Lock(void)" + nl + b"{" + nl + b"#if defined(JX_PLATFORM_SDL)" + nl + b"\tSDL_LockMutex(*(SDL_Mutex**)&m_CriticalSection);" + nl + b"#elif defined(WIN32)" + nl + b"#ifdef SINGLE_PROCESS" + nl + b"\tEnterCriticalSection(&m_CriticalSection);")
    b = rep1(b, b"void KMutex::Unlock(void)" + nl + b"{" + nl + b"#ifdef WIN32" + nl + b"#ifdef SINGLE_PROCESS" + nl + b"\tLeaveCriticalSection(&m_CriticalSection);",
             b"void KMutex::Unlock(void)" + nl + b"{" + nl + b"#if defined(JX_PLATFORM_SDL)" + nl + b"\tSDL_UnlockMutex(*(SDL_Mutex**)&m_CriticalSection);" + nl + b"#elif defined(WIN32)" + nl + b"#ifdef SINGLE_PROCESS" + nl + b"\tLeaveCriticalSection(&m_CriticalSection);")
    wr(p, rd(p), b); print("KMutex.cpp: SDL (SDL_Mutex* trong o CRITICAL_SECTION - kich thuoc lop khong doi)")
else: print("KMutex.cpp: da va")

# 5. KEvent.cpp -------------------------------------------------------------
p = E + "KEvent.cpp"; b = rd(p)
if TAG not in b:
    nl = nlof(b)
    b = rep1(b, b"KEvent::KEvent()" + nl + b"{" + nl + b"    m_hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);",
             b"KEvent::KEvent()" + nl + b"{" + nl + b"#ifdef JX_PLATFORM_SDL\t// " + TAG + b" event auto-reset ~ semaphore (HANDLE chua SDL_Semaphore*)" + nl + b"    m_hEvent = (HANDLE)SDL_CreateSemaphore(0);" + nl + b"#else" + nl + b"    m_hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);" + nl + b"#endif")
    b = rep1(b, b"KEvent::~KEvent()" + nl + b"{" + nl + b"    CloseHandle(m_hEvent);",
             b"KEvent::~KEvent()" + nl + b"{" + nl + b"#ifdef JX_PLATFORM_SDL" + nl + b"    SDL_DestroySemaphore((SDL_Semaphore*)m_hEvent);" + nl + b"#else" + nl + b"    CloseHandle(m_hEvent);" + nl + b"#endif")
    b = rep1(b, b"inline void KEvent::Signal(void)" + nl + b"{" + nl + b"    SetEvent(m_hEvent);",
             b"inline void KEvent::Signal(void)" + nl + b"{" + nl + b"#ifdef JX_PLATFORM_SDL" + nl + b"    if (SDL_GetSemaphoreValue((SDL_Semaphore*)m_hEvent) == 0) SDL_SignalSemaphore((SDL_Semaphore*)m_hEvent);\t// auto-reset: khong don" + nl + b"#else" + nl + b"    SetEvent(m_hEvent);" + nl + b"#endif")
    b = rep1(b, b"inline void KEvent::Wait(void)" + nl + b"{" + nl + b"    WaitForSingleObject(m_hEvent,INFINITE);",
             b"inline void KEvent::Wait(void)" + nl + b"{" + nl + b"#ifdef JX_PLATFORM_SDL" + nl + b"    SDL_WaitSemaphore((SDL_Semaphore*)m_hEvent);" + nl + b"#else" + nl + b"    WaitForSingleObject(m_hEvent,INFINITE);" + nl + b"#endif")
    b = rep1(b, b"inline bool KEvent::TimedWait(long ms)" + nl + b"{" + nl + b"    int r = WaitForSingleObject(m_hEvent, ms);" + nl + b"    return (WAIT_TIMEOUT == r) ? false : true;",
             b"inline bool KEvent::TimedWait(long ms)" + nl + b"{" + nl + b"#ifdef JX_PLATFORM_SDL" + nl + b"    return SDL_WaitSemaphoreTimeout((SDL_Semaphore*)m_hEvent, (Sint32)ms);" + nl + b"#else" + nl + b"    int r = WaitForSingleObject(m_hEvent, ms);" + nl + b"    return (WAIT_TIMEOUT == r) ? false : true;" + nl + b"#endif")
    wr(p, rd(p), b); print("KEvent.cpp: SDL")
else: print("KEvent.cpp: da va")

# 6. KThread.cpp ------------------------------------------------------------
p = E + "KThread.cpp"; b = rd(p)
if TAG not in b:
    nl = nlof(b)
    # ham luong SDL + Create
    a = b"BOOL KThread::Create(TThreadFunc lpFunc, void* lpParam)" + nl + b"{" + nl + b"#ifdef WIN32" + nl + b"\tm_ThreadFunc   = lpFunc;"
    ins = J(nl, [b"#ifdef JX_PLATFORM_SDL", b"// " + TAG + b" luong SDL: HANDLE m_ThreadHandle chua SDL_Thread*; m_ThreadId = 1 khi dang chay, 0 khi xong",
                 b"static int SDLCALL KThread_SdlProc(void* pParam)", b"{", b"\tKThread* pThread = (KThread*)pParam;", b"\tpThread->ThreadFunction();", b"\tpThread->m_ThreadId = 0;", b"\treturn 0;", b"}", b"#endif"]) + \
          b"BOOL KThread::Create(TThreadFunc lpFunc, void* lpParam)" + nl + b"{" + nl + b"#if defined(JX_PLATFORM_SDL)" + nl + b"\tm_ThreadFunc   = lpFunc;" + nl + b"\tm_ThreadParam  = lpParam;" + nl + b"\tm_ThreadId     = 1;" + nl + b"\tm_ThreadHandle = (HANDLE)SDL_CreateThread(KThread_SdlProc, \"KThread\", this);" + nl + b"\tif (!m_ThreadHandle) m_ThreadId = 0;" + nl + b"\treturn (m_ThreadHandle != NULL);" + nl + b"#elif defined(WIN32)" + nl + b"\tm_ThreadFunc   = lpFunc;"
    b = rep1(b, a, ins)
    b = rep1(b, b"void KThread::Destroy()" + nl + b"{" + nl + b"#ifdef WIN32" + nl + b"\tTerminateThread(m_ThreadHandle, 0);",
             b"void KThread::Destroy()" + nl + b"{" + nl + b"#if defined(JX_PLATFORM_SDL)" + nl + b"\tSDL_Log(\"[SDL] KThread::Destroy: SDL khong co TerminateThread - luong phai tu thoat bang co\");" + nl + b"#elif defined(WIN32)" + nl + b"\tTerminateThread(m_ThreadHandle, 0);")
    b = rep1(b, b"void KThread::Suspend()" + nl + b"{" + nl + b"#ifdef WIN32" + nl + b"\t SuspendThread(m_ThreadHandle);",
             b"void KThread::Suspend()" + nl + b"{" + nl + b"#if defined(JX_PLATFORM_SDL)" + nl + b"\tSDL_Log(\"[SDL] KThread::Suspend: khong ho tro\");" + nl + b"#elif defined(WIN32)" + nl + b"\t SuspendThread(m_ThreadHandle);")
    b = rep1(b, b"void KThread::Resume()" + nl + b"{" + nl + b"#ifdef WIN32" + nl + b"\tResumeThread(m_ThreadHandle);",
             b"void KThread::Resume()" + nl + b"{" + nl + b"#if defined(JX_PLATFORM_SDL)" + nl + b"\tSDL_Log(\"[SDL] KThread::Resume: khong ho tro\");" + nl + b"#elif defined(WIN32)" + nl + b"\tResumeThread(m_ThreadHandle);")
    b = rep1(b, b"BOOL KThread::IsRunning()" + nl + b"{" + nl + b"#ifdef WIN32" + nl + b"\tDWORD dwResult = WaitForSingleObject(m_ThreadHandle, 0);",
             b"BOOL KThread::IsRunning()" + nl + b"{" + nl + b"#if defined(JX_PLATFORM_SDL)" + nl + b"\treturn (m_ThreadHandle != NULL && m_ThreadId != 0);" + nl + b"#elif defined(WIN32)" + nl + b"\tDWORD dwResult = WaitForSingleObject(m_ThreadHandle, 0);")
    b = rep1(b, b"void KThread::WaitForExit()" + nl + b"{" + nl + b"#ifdef WIN32" + nl + b"\tWaitForSingleObject(m_ThreadHandle, INFINITE);",
             b"void KThread::WaitForExit()" + nl + b"{" + nl + b"#if defined(JX_PLATFORM_SDL)" + nl + b"\tif (m_ThreadHandle) { SDL_WaitThread((SDL_Thread*)m_ThreadHandle, NULL); m_ThreadHandle = NULL; }" + nl + b"#elif defined(WIN32)" + nl + b"\tWaitForSingleObject(m_ThreadHandle, INFINITE);")
    b = rep1(b, b"int KThread::GetPriority()" + nl + b"{" + nl + b"#ifdef WIN32" + nl + b"\treturn GetThreadPriority(m_ThreadHandle);",
             b"int KThread::GetPriority()" + nl + b"{" + nl + b"#if defined(JX_PLATFORM_SDL)" + nl + b"\treturn 0;" + nl + b"#elif defined(WIN32)" + nl + b"\treturn GetThreadPriority(m_ThreadHandle);")
    wr(p, rd(p), b); print("KThread.cpp: SDL (Create/Destroy/Suspend/Resume/IsRunning/WaitForExit/GetPriority)")
else: print("KThread.cpp: da va")

# 7. KDebug.cpp -------------------------------------------------------------
p = E + "KDebug.cpp"; b = rd(p)
if TAG not in b:
    nl = nlof(b)
    b = rep1(b, b"void g_DebugLog(LPSTR Fmt, ...)" + nl + b"{" + nl + b"#ifndef __linux" + nl + b"\tif (m_hWndDebug)",
             b"void g_DebugLog(LPSTR Fmt, ...)" + nl + b"{" + nl + b"#ifdef JX_PLATFORM_SDL\t// " + TAG + b" ra SDL_Log (Windows: OutputDebugString + stderr; Android: logcat), van gui DebugWin neu co" + nl + b"\t{" + nl + b"\t\tchar szSdl[2048];" + nl + b"\t\tva_list vaSdl;" + nl + b"\t\tva_start(vaSdl, Fmt);" + nl + b"\t\tint nSdl = _vsnprintf(szSdl, sizeof(szSdl) - 1, Fmt, vaSdl);" + nl + b"\t\tva_end(vaSdl);" + nl + b"\t\tif (nSdl < 0) nSdl = (int)sizeof(szSdl) - 1;" + nl + b"\t\tszSdl[nSdl] = 0;" + nl + b"\t\tSDL_Log(\"%s\", szSdl);" + nl + b"\t}" + nl + b"#endif" + nl + b"#ifndef __linux" + nl + b"\tif (m_hWndDebug)")
    b = rep1(b, b"\tg_DebugLog(szMsg);" + nl + b"//\tMessageBox(g_GetMainHWnd(), szMsg, 0, MB_OK);" + nl + b"\tMessageBox(NULL, szMsg, 0, MB_OK);",
             b"\tg_DebugLog(szMsg);" + nl + b"//\tMessageBox(g_GetMainHWnd(), szMsg, 0, MB_OK);" + nl + b"#ifdef JX_PLATFORM_SDL" + nl + b"\tSDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, \"JXWC\", szMsg, NULL);\t// " + TAG + nl + b"#else" + nl + b"\tMessageBox(NULL, szMsg, 0, MB_OK);" + nl + b"#endif")
    wr(p, rd(p), b); print("KDebug.cpp: SDL")
else: print("KDebug.cpp: da va")
print("XONG 2b-1")
