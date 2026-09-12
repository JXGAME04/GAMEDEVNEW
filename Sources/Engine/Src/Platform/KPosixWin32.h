/*===========================================================================
  KPosixWin32.h - [ANDROID 08/09] Tap con ham Win32 (kernel32/user32/winmm/shell32) CAI THAT tren POSIX + SDL3,
                  di kem KPosixCompat.h (kieu/hang so). Duoc KPosixCompat.h include o cuoi -> moi TU cua ban Android
                  deu thay. Cai dat trong KPosixWin32.cpp (bien dich vao libEngine.so).
  Nguyen tac: chi nhung ham ma ma client that su goi (xem TONGHOP_NDK.md muc 3.3); ham "cua so" tra ve tri
  vo hai (cua so = SDL_Window cua KSdlApp). Duong dan Windows ("\\", chu hoa) -> POSIX qua JxPathPosix():
  doi "\\" thanh "/", ha chu thuong phan tuong doi / duoi thu muc du lieu (du lieu tren may phai ha chu thuong san).
  ASCII thuan, C-safe (ucl/CRC32.C cung include).
===========================================================================*/
#ifndef KPOSIXWIN32_H
#define KPOSIXWIN32_H
#if !defined(_WIN32)

#ifdef __cplusplus
extern "C" {
#endif

/* ---- hang so bo sung (KPosixCompat.h chua co) ---- */
#ifndef MAX_COMPUTERNAME_LENGTH
#define MAX_COMPUTERNAME_LENGTH 15
#endif
#ifndef UNLEN
#define UNLEN 256
#endif
#ifndef SM_CXSCREEN
#define SM_CXSCREEN 0
#define SM_CYSCREEN 1
#define SM_CXFULLSCREEN 16
#define SM_CYFULLSCREEN 17
#define SM_CXVIRTUALSCREEN 78
#define SM_CYVIRTUALSCREEN 79
#endif
#ifndef GMEM_MOVEABLE
#define GMEM_FIXED 0x0000
#define GMEM_MOVEABLE 0x0002
#define GMEM_ZEROINIT 0x0040
#define GHND 0x0042
#define GPTR 0x0040
#endif
#ifndef CF_TEXT
#define CF_TEXT 1
#define CF_OEMTEXT 7
#define CF_UNICODETEXT 13
#endif
#ifndef ERROR_ALREADY_EXISTS
#define ERROR_ALREADY_EXISTS 183
#endif
#ifndef ERROR_FILE_NOT_FOUND
#define ERROR_FILE_NOT_FOUND 2
#endif
#ifndef ERROR_PATH_NOT_FOUND
#define ERROR_PATH_NOT_FOUND 3
#endif
#ifndef ERROR_ACCESS_DENIED
#define ERROR_ACCESS_DENIED 5
#endif
#ifndef TIMERR_NOERROR
#define TIMERR_NOERROR 0
#endif
#ifndef IDOK
#define IDOK 1
#define IDCANCEL 2
#define IDABORT 3
#define IDRETRY 4
#define IDIGNORE 5
#define IDYES 6
#define IDNO 7
#endif
#ifndef VER_PLATFORM_WIN32_NT
#define VER_PLATFORM_WIN32_NT 2
#endif
#ifndef FILE_ATTRIBUTE_ARCHIVE
#define FILE_ATTRIBUTE_ARCHIVE 0x20
#endif
#ifndef FILE_ATTRIBUTE_NORMAL
#define FILE_ATTRIBUTE_NORMAL 0x80
#endif
#ifndef FILE_ATTRIBUTE_READONLY
#define FILE_ATTRIBUTE_READONLY 0x01
#endif
#ifndef STILL_ACTIVE
#define STILL_ACTIVE 259
#endif
#ifndef HKEY_LOCAL_MACHINE
#define HKEY_CLASSES_ROOT ((HKEY)(uintptr_t)0x80000000u)
#define HKEY_CURRENT_USER ((HKEY)(uintptr_t)0x80000001u)
#define HKEY_LOCAL_MACHINE ((HKEY)(uintptr_t)0x80000002u)
#endif
#ifndef KEY_READ
#define KEY_READ 0x20019
#define KEY_WRITE 0x20006
#define KEY_ALL_ACCESS 0xF003F
#define REG_SZ 1
#define REG_DWORD 4
#endif
#ifndef SW_SHOWNORMAL
#define SW_SHOWNORMAL 1
#endif
#ifndef SEE_MASK_NOCLOSEPROCESS
#define SEE_MASK_NOCLOSEPROCESS 0x40
#endif

/* ---- kieu bo sung ---- */
typedef struct tagMSG { HWND hwnd; UINT message; WPARAM wParam; LPARAM lParam; DWORD time; POINT pt; } MSG, *PMSG, *LPMSG;
typedef LRESULT (*WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef struct tagWNDCLASSA { UINT style; WNDPROC lpfnWndProc; int cbClsExtra; int cbWndExtra; HINSTANCE hInstance; HICON hIcon;
    HCURSOR hCursor; HBRUSH hbrBackground; LPCSTR lpszMenuName; LPCSTR lpszClassName; } WNDCLASSA, *LPWNDCLASSA, WNDCLASS, *LPWNDCLASS;
typedef struct tagWNDCLASSEXA { UINT cbSize; UINT style; WNDPROC lpfnWndProc; int cbClsExtra; int cbWndExtra; HINSTANCE hInstance;
    HICON hIcon; HCURSOR hCursor; HBRUSH hbrBackground; LPCSTR lpszMenuName; LPCSTR lpszClassName; HICON hIconSm; } WNDCLASSEXA, *LPWNDCLASSEXA, WNDCLASSEX;
typedef struct _OSVERSIONINFOA { DWORD dwOSVersionInfoSize; DWORD dwMajorVersion; DWORD dwMinorVersion; DWORD dwBuildNumber;
    DWORD dwPlatformId; CHAR szCSDVersion[128]; } OSVERSIONINFOA, *LPOSVERSIONINFOA, OSVERSIONINFO, *LPOSVERSIONINFO;
typedef struct _OSVERSIONINFOEXA { DWORD dwOSVersionInfoSize; DWORD dwMajorVersion; DWORD dwMinorVersion; DWORD dwBuildNumber;
    DWORD dwPlatformId; CHAR szCSDVersion[128]; WORD wServicePackMajor; WORD wServicePackMinor; WORD wSuiteMask; BYTE wProductType;
    BYTE wReserved; } OSVERSIONINFOEXA, *LPOSVERSIONINFOEXA, OSVERSIONINFOEX;
typedef struct _SYSTEM_INFO { DWORD dwOemId; DWORD dwPageSize; LPVOID lpMinimumApplicationAddress; LPVOID lpMaximumApplicationAddress;
    DWORD_PTR dwActiveProcessorMask; DWORD dwNumberOfProcessors; DWORD dwProcessorType; DWORD dwAllocationGranularity;
    WORD wProcessorLevel; WORD wProcessorRevision; } SYSTEM_INFO, *LPSYSTEM_INFO;
typedef struct _MEMORYSTATUSEX { DWORD dwLength; DWORD dwMemoryLoad; DWORDLONG ullTotalPhys; DWORDLONG ullAvailPhys; DWORDLONG ullTotalPageFile;
    DWORDLONG ullAvailPageFile; DWORDLONG ullTotalVirtual; DWORDLONG ullAvailVirtual; DWORDLONG ullAvailExtendedVirtual; } MEMORYSTATUSEX, *LPMEMORYSTATUSEX;
typedef struct _TIME_ZONE_INFORMATION { LONG Bias; WCHAR StandardName[32]; SYSTEMTIME StandardDate; LONG StandardBias;
    WCHAR DaylightName[32]; SYSTEMTIME DaylightDate; LONG DaylightBias; } TIME_ZONE_INFORMATION, *LPTIME_ZONE_INFORMATION;
typedef struct _SHELLEXECUTEINFOA { DWORD cbSize; ULONG fMask; HWND hwnd; LPCSTR lpVerb; LPCSTR lpFile; LPCSTR lpParameters;
    LPCSTR lpDirectory; int nShow; HINSTANCE hInstApp; LPVOID lpIDList; LPCSTR lpClass; HANDLE hkeyClass; DWORD dwHotKey;
    HANDLE hIcon; HANDLE hProcess; } SHELLEXECUTEINFOA, *LPSHELLEXECUTEINFOA, SHELLEXECUTEINFO;
typedef LONG LSTATUS;

/* ---- moc noi voi lop nen SDL (KSdlApp dat) ---- */
typedef SHORT (*JxGetKeyStateFn)(int vk);
typedef LRESULT (*JxWinMsgFn)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern JxGetKeyStateFn g_pfnJxGetKeyState;   /* GetKeyState/GetAsyncKeyState -> bang phim cua KSdlApp */
extern JxWinMsgFn      g_pfnJxWinMsg;        /* PostMessage/SendMessage toi cua so chinh -> MsgProc cua KSdlApp */
#ifdef JX_APPLE
void        JxPosix_DangKyKyHieu(const char* pszMod, const char* pszTen, void* pfn);  /* [IOS-KYHIEU 11/09] bang tra tinh thay dlopen */
#endif
void        JxPosix_SetDataDir(const char* pszDir);   /* thu muc du lieu game (= thu muc "canh Game.exe") */
const char* JxPosix_DataDir(void);
void        JxPosix_SetMainWindow(void* pSdlWindow);  /* SDL_Window* cua KSdlApp (HWND tren POSIX = SDL_Window*) */
void*       JxPosix_MainWindow(void);
/* Duong dan Windows -> POSIX: "\\" -> "/", ha chu thuong phan tuong doi hoac duoi thu muc du lieu. Tra ve out. */
char*       JxPathPosix(const char* pszIn, char* pszOut, size_t nOut);
FILE*       jx_fopen(const char* pszPath, const char* pszMode);
int         jx_access(const char* pszPath, int nMode);
int         jx_remove(const char* pszPath);
int         jx_rename(const char* pszOld, const char* pszNew);
int         jx_mkdir(const char* pszPath);
int         jx_chdir(const char* pszPath);
/* ma game dung fopen/_access/remove/rename/_mkdir/_chdir voi duong dan Windows -> di qua JxPathPosix */
#define fopen   jx_fopen
#define _access jx_access
#define access  jx_access
#define remove  jx_remove
#define rename  jx_rename
#define _mkdir  jx_mkdir
#define _chdir  jx_chdir
#define _rmdir  rmdir
#define _unlink jx_remove
#define unlink  jx_remove

/* ---- CRITICAL_SECTION = pthread_mutex_t (KPosixCompat.h), de quy nhu Windows ---- */
static inline void InitializeCriticalSection(CRITICAL_SECTION* cs)
{
    pthread_mutexattr_t a; pthread_mutexattr_init(&a); pthread_mutexattr_settype(&a, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(cs, &a); pthread_mutexattr_destroy(&a);
}
static inline BOOL InitializeCriticalSectionAndSpinCount(CRITICAL_SECTION* cs, DWORD n) { (void)n; InitializeCriticalSection(cs); return 1; }
static inline void EnterCriticalSection(CRITICAL_SECTION* cs)   { pthread_mutex_lock(cs); }
static inline void LeaveCriticalSection(CRITICAL_SECTION* cs)   { pthread_mutex_unlock(cs); }
static inline BOOL TryEnterCriticalSection(CRITICAL_SECTION* cs) { return pthread_mutex_trylock(cs) == 0; }
static inline void DeleteCriticalSection(CRITICAL_SECTION* cs)  { pthread_mutex_destroy(cs); }

/* ---- su kien / doi / luong / handle ---- */
HANDLE  CreateEventA(LPSECURITY_ATTRIBUTES sa, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName);
BOOL    SetEvent(HANDLE h);
BOOL    ResetEvent(HANDLE h);
BOOL    PulseEvent(HANDLE h);
DWORD   WaitForSingleObject(HANDLE h, DWORD dwMilliseconds);
DWORD   WaitForMultipleObjects(DWORD nCount, const HANDLE* pHandles, BOOL bWaitAll, DWORD dwMilliseconds);
BOOL    CloseHandle(HANDLE h);
HANDLE  CreateThread(LPSECURITY_ATTRIBUTES sa, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE pfn, LPVOID pParam, DWORD dwFlags, LPDWORD pdwId);
BOOL    GetExitCodeThread(HANDLE h, LPDWORD pdwExit);
BOOL    SetThreadPriority(HANDLE h, int nPriority);
int     GetThreadPriority(HANDLE h);
DWORD   GetCurrentThreadId(void);
DWORD   GetCurrentProcessId(void);
HANDLE  GetCurrentProcess(void);
HANDLE  GetCurrentThread(void);
DWORD   SleepEx(DWORD ms, BOOL bAlertable);
DWORD   GetTickCount(void);
DWORD   timeGetTime(void);
UINT    timeBeginPeriod(UINT u);
UINT    timeEndPeriod(UINT u);
void    ExitProcess(UINT uExitCode);
BOOL    TerminateProcess(HANDLE h, UINT uExitCode);
DWORD   GetLastError(void);
void    SetLastError(DWORD dwErr);
DWORD   FormatMessageA(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, DWORD dwLanguageId, LPSTR lpBuffer, DWORD nSize, void* Arguments);
BOOL    IsDebuggerPresent(void);
void    DebugBreak(void);
void    OutputDebugStringA(LPCSTR lpOutputString);
LPVOID  SetUnhandledExceptionFilter(LPVOID lpFilter);
#ifndef Sleep
void    Sleep(DWORD ms);
#endif

/* ---- thoi gian ---- */
void    GetLocalTime(LPSYSTEMTIME lpSystemTime);
void    GetSystemTime(LPSYSTEMTIME lpSystemTime);
void    GetSystemTimeAsFileTime(LPFILETIME lpFileTime);
BOOL    SystemTimeToFileTime(const SYSTEMTIME* lpSystemTime, LPFILETIME lpFileTime);
BOOL    GetProcessTimes(HANDLE hProcess, LPFILETIME lpCreation, LPFILETIME lpExit, LPFILETIME lpKernel, LPFILETIME lpUser);	// [ANDROID 11/09 MANG]
BOOL    GetThreadTimes(HANDLE hThread, LPFILETIME lpCreation, LPFILETIME lpExit, LPFILETIME lpKernel, LPFILETIME lpUser);
BOOL    FileTimeToSystemTime(const FILETIME* lpFileTime, LPSYSTEMTIME lpSystemTime);
BOOL    FileTimeToLocalFileTime(const FILETIME* lpFileTime, LPFILETIME lpLocalFileTime);
BOOL    LocalFileTimeToFileTime(const FILETIME* lpLocalFileTime, LPFILETIME lpFileTime);
BOOL    SystemTimeToTzSpecificLocalTime(const TIME_ZONE_INFORMATION* lpTz, const SYSTEMTIME* lpUniversalTime, LPSYSTEMTIME lpLocalTime);
BOOL    TzSpecificLocalTimeToSystemTime(const TIME_ZONE_INFORMATION* lpTz, const SYSTEMTIME* lpLocalTime, LPSYSTEMTIME lpUniversalTime);
DWORD   GetTimeZoneInformation(LPTIME_ZONE_INFORMATION lpTz);
BOOL    QueryPerformanceCounter(LARGE_INTEGER* lpCount);
BOOL    QueryPerformanceFrequency(LARGE_INTEGER* lpFreq);
LONG    CompareFileTime(const FILETIME* a, const FILETIME* b);

/* ---- module (.dll -> lib<ten>.so) ---- */
HMODULE LoadLibraryA(LPCSTR lpLibFileName);
HMODULE LoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
FARPROC GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
BOOL    FreeLibrary(HMODULE hModule);
HMODULE GetModuleHandleA(LPCSTR lpModuleName);
DWORD   GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize);
LPSTR   GetCommandLineA(void);

/* ---- tep / thu muc ---- */
DWORD   GetCurrentDirectoryA(DWORD nBufferLength, LPSTR lpBuffer);
BOOL    SetCurrentDirectoryA(LPCSTR lpPathName);
BOOL    CreateDirectoryA(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES sa);
BOOL    RemoveDirectoryA(LPCSTR lpPathName);
BOOL    DeleteFileA(LPCSTR lpFileName);
BOOL    CopyFileA(LPCSTR lpExisting, LPCSTR lpNew, BOOL bFailIfExists);
BOOL    MoveFileA(LPCSTR lpExisting, LPCSTR lpNew);
BOOL    MoveFileExA(LPCSTR lpExisting, LPCSTR lpNew, DWORD dwFlags);
DWORD   GetFileAttributesA(LPCSTR lpFileName);
BOOL    SetFileAttributesA(LPCSTR lpFileName, DWORD dwAttrs);
DWORD   GetTempPathA(DWORD nBufferLength, LPSTR lpBuffer);
UINT    GetTempFileNameA(LPCSTR lpPathName, LPCSTR lpPrefix, UINT uUnique, LPSTR lpTempFileName);
DWORD   GetFullPathNameA(LPCSTR lpFileName, DWORD nBufferLength, LPSTR lpBuffer, LPSTR* lpFilePart);
HANDLE  FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);
BOOL    FindNextFileA(HANDLE hFind, LPWIN32_FIND_DATAA lpFindFileData);
BOOL    FindClose(HANDLE hFind);
BOOL    GetDiskFreeSpaceExA(LPCSTR lpDir, ULARGE_INTEGER* pFreeAvail, ULARGE_INTEGER* pTotal, ULARGE_INTEGER* pTotalFree);
UINT    GetSystemDirectoryA(LPSTR lpBuffer, UINT uSize);
UINT    GetWindowsDirectoryA(LPSTR lpBuffer, UINT uSize);
DWORD   GetEnvironmentVariableA(LPCSTR lpName, LPSTR lpBuffer, DWORD nSize);
BOOL    SetEnvironmentVariableA(LPCSTR lpName, LPCSTR lpValue);

/* ---- INI (GetPrivateProfile*) ---- */
UINT    GetPrivateProfileIntA(LPCSTR lpAppName, LPCSTR lpKeyName, INT nDefault, LPCSTR lpFileName);
DWORD   GetPrivateProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturned, DWORD nSize, LPCSTR lpFileName);
BOOL    WritePrivateProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpString, LPCSTR lpFileName);

/* ---- he thong / bao mat / registry (stub) ---- */
BOOL    GetComputerNameA(LPSTR lpBuffer, LPDWORD nSize);
BOOL    GetUserNameA(LPSTR lpBuffer, LPDWORD pcbBuffer);
BOOL    GetVersionExA(LPOSVERSIONINFOA lpVersionInfo);
void    GetSystemInfo(LPSYSTEM_INFO lpSystemInfo);
void    GlobalMemoryStatus(LPMEMORYSTATUS lpBuffer);
BOOL    GlobalMemoryStatusEx(LPMEMORYSTATUSEX lpBuffer);
LSTATUS RegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, DWORD samDesired, HKEY* phkResult);
LSTATUS RegCreateKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, DWORD samDesired, LPSECURITY_ATTRIBUTES sa, HKEY* phkResult, LPDWORD lpdwDisposition);
LSTATUS RegQueryValueExA(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
LSTATUS RegSetValueExA(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE* lpData, DWORD cbData);
LSTATUS RegCloseKey(HKEY hKey);

/* ---- user32: cua so / chuot / phim / thong diep (cua so = SDL_Window cua KSdlApp) ---- */
SHORT   GetKeyState(int nVirtKey);
SHORT   GetAsyncKeyState(int vKey);
BOOL    GetKeyboardState(PBYTE lpKeyState);
UINT    MapVirtualKeyA(UINT uCode, UINT uMapType);
void    PostQuitMessage(int nExitCode);
BOOL    PostMessageA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT SendMessageA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL    PeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
BOOL    GetMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
BOOL    TranslateMessage(const MSG* lpMsg);
LRESULT DispatchMessageA(const MSG* lpMsg);
LRESULT DefWindowProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
ATOM    RegisterClassA(const WNDCLASSA* lpWndClass);
ATOM    RegisterClassExA(const WNDCLASSEXA* lpWndClass);
HWND    CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
BOOL    DestroyWindow(HWND hWnd);
HWND    FindWindowA(LPCSTR lpClassName, LPCSTR lpWindowName);
HWND    FindWindowExA(HWND hParent, HWND hAfter, LPCSTR lpClassName, LPCSTR lpWindowName);
BOOL    SetWindowTextA(HWND hWnd, LPCSTR lpString);
int     GetWindowTextA(HWND hWnd, LPSTR lpString, int nMaxCount);
int     GetClassNameA(HWND hWnd, LPSTR lpClassName, int nMaxCount);
BOOL    GetClientRect(HWND hWnd, LPRECT lpRect);
BOOL    GetWindowRect(HWND hWnd, LPRECT lpRect);
BOOL    ClientToScreen(HWND hWnd, LPPOINT lpPoint);
BOOL    ScreenToClient(HWND hWnd, LPPOINT lpPoint);
BOOL    AdjustWindowRect(LPRECT lpRect, DWORD dwStyle, BOOL bMenu);
BOOL    AdjustWindowRectEx(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle);
BOOL    MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint);
BOOL    SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
BOOL    ShowWindow(HWND hWnd, int nCmdShow);
BOOL    UpdateWindow(HWND hWnd);
BOOL    InvalidateRect(HWND hWnd, const RECT* lpRect, BOOL bErase);
BOOL    IsWindow(HWND hWnd);
BOOL    IsIconic(HWND hWnd);
BOOL    IsWindowVisible(HWND hWnd);
HWND    GetForegroundWindow(void);
BOOL    SetForegroundWindow(HWND hWnd);
HWND    GetActiveWindow(void);
HWND    SetActiveWindow(HWND hWnd);
HWND    GetFocus(void);
HWND    SetFocus(HWND hWnd);
HWND    GetDesktopWindow(void);
HWND    GetParent(HWND hWnd);
HWND    SetCapture(HWND hWnd);
BOOL    ReleaseCapture(void);
HWND    GetCapture(void);
LONG    GetWindowLongA(HWND hWnd, int nIndex);
LONG    SetWindowLongA(HWND hWnd, int nIndex, LONG dwNewLong);
LONG_PTR GetWindowLongPtrA(HWND hWnd, int nIndex);
LONG_PTR SetWindowLongPtrA(HWND hWnd, int nIndex, LONG_PTR dwNewLong);
HDC     GetDC(HWND hWnd);
int     ReleaseDC(HWND hWnd, HDC hDC);
int     GetSystemMetrics(int nIndex);
UINT    GetDoubleClickTime(void);
BOOL    GetCursorPos(LPPOINT lpPoint);
BOOL    SetCursorPos(int X, int Y);
int     ShowCursor(BOOL bShow);
HCURSOR SetCursor(HCURSOR hCursor);
HCURSOR GetCursor(void);
HCURSOR LoadCursorA(HINSTANCE hInstance, LPCSTR lpCursorName);
HCURSOR LoadCursorFromFileA(LPCSTR lpFileName);
BOOL    DestroyCursor(HCURSOR hCursor);
HICON   LoadIconA(HINSTANCE hInstance, LPCSTR lpIconName);
BOOL    ClipCursor(const RECT* lpRect);
int     MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
BOOL    MessageBeep(UINT uType);
BOOL    AllocConsole(void);
BOOL    FreeConsole(void);
UINT_PTR SetTimer(HWND hWnd, UINT_PTR nIDEvent, UINT uElapse, LPVOID lpTimerFunc);
BOOL    KillTimer(HWND hWnd, UINT_PTR uIDEvent);
BOOL    CreateCaret(HWND hWnd, HANDLE hBitmap, int nWidth, int nHeight);
BOOL    DestroyCaret(void);
BOOL    SetCaretPos(int X, int Y);
BOOL    ShowCaret(HWND hWnd);
BOOL    HideCaret(HWND hWnd);
BOOL    OpenClipboard(HWND hWndNewOwner);
BOOL    CloseClipboard(void);
BOOL    EmptyClipboard(void);
HANDLE  GetClipboardData(UINT uFormat);
HANDLE  SetClipboardData(UINT uFormat, HANDLE hMem);
BOOL    IsClipboardFormatAvailable(UINT format);
HGLOBAL GlobalAlloc(UINT uFlags, SIZE_T dwBytes);
LPVOID  GlobalLock(HGLOBAL hMem);
BOOL    GlobalUnlock(HGLOBAL hMem);
HGLOBAL GlobalFree(HGLOBAL hMem);
SIZE_T  GlobalSize(HGLOBAL hMem);
HINSTANCE ShellExecuteA(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, int nShowCmd);
BOOL    ShellExecuteExA(SHELLEXECUTEINFOA* pExecInfo);
int     MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
int     WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar);
UINT    GetACP(void);
BOOL    IsDBCSLeadByte(BYTE TestChar);
int     WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData);
int     WSACleanup(void);

/* ---- DirectX (JX_NO_DIRECTX): chi con TEN KIEU con tro de khai bao lop (KDDraw.h/KDSound.h/KMusic.h/KCanvas.h) bien dich duoc;
        khong co doi tuong that - g_pDirectDraw luon NULL (Platform/KDDrawStub.cpp), am thanh qua miniaudio (KSoundMa.cpp) ---- */
typedef void *LPDIRECTDRAW, *LPDIRECTDRAW7, *LPDIRECTDRAWSURFACE, *LPDIRECTDRAWSURFACE7, *LPDIRECTDRAWCLIPPER, *LPDIRECTDRAWPALETTE;
typedef void *LPDIRECTSOUND, *LPDIRECTSOUND8, *LPDIRECTSOUNDBUFFER, *LPDIRECTSOUNDBUFFER8, *LPDIRECTSOUNDNOTIFY, *LPDIRECTSOUND3DBUFFER, *LPDIRECTSOUND3DLISTENER;
typedef void *LPDIRECTINPUT, *LPDIRECTINPUT8, *LPDIRECTINPUTDEVICE, *LPDIRECTINPUTDEVICE8;
typedef struct _DSBPOSITIONNOTIFY { DWORD dwOffset; HANDLE hEventNotify; } DSBPOSITIONNOTIFY, *LPDSBPOSITIONNOTIFY;
#ifndef _INC_MMSYSTEM
#define _INC_MMSYSTEM       /* KWavFile.h tu dinh nghia WAVEFORMATEX khi thieu mmsystem.h; KPosixCompat.h da co */
#endif
#ifndef DSBPLAY_LOOPING
#define DSBPLAY_LOOPING 0x00000001
#endif
/* khay he thong (TrayMode.h) - khong co tren Android */
typedef struct _NOTIFYICONDATAA { DWORD cbSize; HWND hWnd; UINT uID; UINT uFlags; UINT uCallbackMessage; HICON hIcon; CHAR szTip[128];
    DWORD dwState; DWORD dwStateMask; CHAR szInfo[256]; UINT uTimeout; CHAR szInfoTitle[64]; DWORD dwInfoFlags; } NOTIFYICONDATAA, *PNOTIFYICONDATAA, NOTIFYICONDATA, *PNOTIFYICONDATA;
#ifndef NIM_ADD
#define NIM_ADD 0
#define NIM_MODIFY 1
#define NIM_DELETE 2
#define NIF_MESSAGE 1
#define NIF_ICON 2
#define NIF_TIP 4
#endif
static inline BOOL Shell_NotifyIconA(DWORD dwMessage, PNOTIFYICONDATAA lpData) { (void)dwMessage; (void)lpData; return 0; }
#ifndef Shell_NotifyIcon
#define Shell_NotifyIcon Shell_NotifyIconA
#endif
/* kieu cua so (RegisterClass) */
#ifndef CS_DBLCLKS
#define CS_VREDRAW 0x0001
#define CS_HREDRAW 0x0002
#define CS_DBLCLKS 0x0008
#define CS_OWNDC 0x0020
#define CS_CLASSDC 0x0040
#define CS_PARENTDC 0x0080
#define CS_NOCLOSE 0x0200
#define CS_SAVEBITS 0x0800
#define CS_BYTEALIGNCLIENT 0x1000
#define CS_BYTEALIGNWINDOW 0x2000
#define CS_GLOBALCLASS 0x4000
#endif
/* ATL chuyen ma (Common/Utils.cpp): ban MultiByte -> khong doi gi */
#ifndef USES_CONVERSION
#define USES_CONVERSION     ((void)0)
#define T2A(x)              ((LPSTR)(x))
#define A2T(x)              ((LPSTR)(x))
#define T2CA(x)             ((LPCSTR)(x))
#define A2CT(x)             ((LPCSTR)(x))
#define T2W(x)              ((LPWSTR)(x))
#define W2T(x)              ((LPSTR)(x))
#endif
/* version.dll (Common/Utils.cpp): khong co tren Android */
static inline DWORD GetFileVersionInfoSizeA(LPCSTR f, LPDWORD h) { (void)f; if (h) *h = 0; return 0; }
static inline BOOL  GetFileVersionInfoA(LPCSTR f, DWORD h, DWORD n, LPVOID p) { (void)f; (void)h; (void)n; (void)p; return 0; }
static inline BOOL  VerQueryValueA(LPCVOID b, LPCSTR s, LPVOID* p, PUINT n) { (void)b; (void)s; if (p) *p = 0; if (n) *n = 0; return 0; }
#ifndef GetFileVersionInfoSize
#define GetFileVersionInfoSize GetFileVersionInfoSizeA
#define GetFileVersionInfo GetFileVersionInfoA
#define VerQueryValue VerQueryValueA
#endif
/* ATOM / FormatMessage / LocalAlloc / man hinh */
#ifndef MAKEINTATOM
#define MAKEINTATOM(i)      ((LPSTR)(uintptr_t)((WORD)(i)))
#endif
#ifndef FORMAT_MESSAGE_FROM_SYSTEM
#define FORMAT_MESSAGE_ALLOCATE_BUFFER 0x00000100
#define FORMAT_MESSAGE_IGNORE_INSERTS  0x00000200
#define FORMAT_MESSAGE_FROM_STRING     0x00000400
#define FORMAT_MESSAGE_FROM_HMODULE    0x00000800
#define FORMAT_MESSAGE_FROM_SYSTEM     0x00001000
#define FORMAT_MESSAGE_ARGUMENT_ARRAY  0x00002000
#define FORMAT_MESSAGE_MAX_WIDTH_MASK  0x000000FF
#endif
#ifndef MAKELANGID
#define LANG_NEUTRAL 0x00
#define SUBLANG_DEFAULT 0x01
#define MAKELANGID(p, s) ((((WORD)(s)) << 10) | (WORD)(p))
#endif
static inline HLOCAL LocalAlloc(UINT f, SIZE_T n) { (void)f; return (HLOCAL)calloc(1, n ? n : 1); }
static inline HLOCAL LocalFree(HLOCAL h) { free((void*)h); return NULL; }
static inline LPVOID LocalLock(HLOCAL h) { return (LPVOID)h; }
static inline BOOL   LocalUnlock(HLOCAL h) { (void)h; return 1; }
#ifndef MONITOR_DEFAULTTOPRIMARY
#define MONITOR_DEFAULTTONULL 0
#define MONITOR_DEFAULTTOPRIMARY 1
#define MONITOR_DEFAULTTONEAREST 2
#endif
typedef struct tagMONITORINFO { DWORD cbSize; RECT rcMonitor; RECT rcWork; DWORD dwFlags; } MONITORINFO, *LPMONITORINFO;
static inline HMONITOR MonitorFromWindow(HWND h, DWORD f) { (void)h; (void)f; return (HMONITOR)(uintptr_t)1; }
static inline HMONITOR MonitorFromPoint(POINT p, DWORD f) { (void)p; (void)f; return (HMONITOR)(uintptr_t)1; }
static inline BOOL GetMonitorInfoA(HMONITOR h, LPMONITORINFO mi)
{ (void)h; if (!mi) return 0; mi->rcMonitor.left = mi->rcMonitor.top = 0; mi->rcMonitor.right = GetSystemMetrics(SM_CXSCREEN); mi->rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN); mi->rcWork = mi->rcMonitor; mi->dwFlags = 1; return 1; }
#ifndef GetMonitorInfo
#define GetMonitorInfo GetMonitorInfoA
#endif
/* kieu cua so (GetWindowLong/SetWindowLong - KRepresentShell3 toan man hinh) */
#ifndef GWL_STYLE
#define GWL_WNDPROC (-4)
#define GWL_HINSTANCE (-6)
#define GWL_ID (-12)
#define GWL_STYLE (-16)
#define GWL_EXSTYLE (-20)
#define GWL_USERDATA (-21)
#define GWLP_WNDPROC (-4)
#define GWLP_USERDATA (-21)
#endif
#ifndef WS_POPUP
#define WS_OVERLAPPED 0x00000000
#define WS_POPUP 0x80000000
#define WS_CHILD 0x40000000
#define WS_MINIMIZE 0x20000000
#define WS_VISIBLE 0x10000000
#define WS_DISABLED 0x08000000
#define WS_CLIPSIBLINGS 0x04000000
#define WS_CLIPCHILDREN 0x02000000
#define WS_MAXIMIZE 0x01000000
#define WS_CAPTION 0x00C00000
#define WS_BORDER 0x00800000
#define WS_DLGFRAME 0x00400000
#define WS_VSCROLL 0x00200000
#define WS_HSCROLL 0x00100000
#define WS_SYSMENU 0x00080000
#define WS_THICKFRAME 0x00040000
#define WS_MINIMIZEBOX 0x00020000
#define WS_MAXIMIZEBOX 0x00010000
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
#define WS_POPUPWINDOW (WS_POPUP | WS_BORDER | WS_SYSMENU)
#define WS_EX_TOPMOST 0x00000008
#define WS_EX_TOOLWINDOW 0x00000080
#define WS_EX_APPWINDOW 0x00040000
#endif
#ifndef BLACK_BRUSH
#define WHITE_BRUSH 0
#define LTGRAY_BRUSH 1
#define GRAY_BRUSH 2
#define DKGRAY_BRUSH 3
#define BLACK_BRUSH 4
#define NULL_BRUSH 5
#endif
static inline HGDIOBJ GetStockObject(int i) { (void)i; return (HGDIOBJ)0; }
#ifndef ERROR_INSUFFICIENT_BUFFER
#define ERROR_INSUFFICIENT_BUFFER 122
#endif
#ifndef ERROR_MORE_DATA
#define ERROR_MORE_DATA 234
#endif
/* GetVersion(): Windows 10 (10.0) - ma cu chi hoi phien ban lon */
static inline DWORD GetVersion(void) { return 0x00000A0A; }
/* HW profile (Login/NetConnectAgent: ma may cho chong da mo) */
#ifndef HW_PROFILE_GUIDLEN
#define HW_PROFILE_GUIDLEN 39
#define MAX_PROFILE_LEN 80
#endif
typedef struct tagHW_PROFILE_INFOA { DWORD dwDockInfo; CHAR szHwProfileGuid[HW_PROFILE_GUIDLEN]; CHAR szHwProfileName[MAX_PROFILE_LEN]; } HW_PROFILE_INFOA, *LPHW_PROFILE_INFOA, HW_PROFILE_INFO, *LPHW_PROFILE_INFO;
BOOL GetCurrentHwProfileA(LPHW_PROFILE_INFOA lpHwProfileInfo);
#ifndef GetCurrentHwProfile
#define GetCurrentHwProfile GetCurrentHwProfileA
#endif
/* shell32: thu muc dac biet -> thu muc du lieu */
#ifndef CSIDL_DESKTOP
#define CSIDL_DESKTOP 0x0000
#define CSIDL_PERSONAL 0x0005
#define CSIDL_APPDATA 0x001A
#define CSIDL_DESKTOPDIRECTORY 0x0010
#define CSIDL_MYPICTURES 0x0027
#define CSIDL_COMMON_DOCUMENTS 0x002E
#endif
BOOL SHGetSpecialFolderPathA(HWND hwnd, LPSTR pszPath, int csidl, BOOL fCreate);
HRESULT SHGetFolderPathA(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPSTR pszPath);
#ifndef SHGetSpecialFolderPath
#define SHGetSpecialFolderPath SHGetSpecialFolderPathA
#define SHGetFolderPath SHGetFolderPathA
#endif
/* PeekMessage / menu / thong diep dang ky / bo nho tien trinh (psapi) */
#ifndef PM_REMOVE
#define PM_NOREMOVE 0
#define PM_REMOVE 1
#define PM_NOYIELD 2
#endif
static inline HMENU GetMenu(HWND h) { (void)h; return (HMENU)0; }
static inline BOOL SetMenu(HWND h, HMENU m) { (void)h; (void)m; return 1; }
static inline BOOL DrawMenuBar(HWND h) { (void)h; return 1; }
UINT RegisterWindowMessageA(LPCSTR lpString);
#ifndef RegisterWindowMessage
#define RegisterWindowMessage RegisterWindowMessageA
#endif
typedef struct _PROCESS_MEMORY_COUNTERS { DWORD cb; DWORD PageFaultCount; SIZE_T PeakWorkingSetSize; SIZE_T WorkingSetSize; SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage; SIZE_T QuotaPeakNonPagedPoolUsage; SIZE_T QuotaNonPagedPoolUsage; SIZE_T PagefileUsage; SIZE_T PeakPagefileUsage; } PROCESS_MEMORY_COUNTERS, *PPROCESS_MEMORY_COUNTERS;
typedef struct _PROCESS_MEMORY_COUNTERS_EX { DWORD cb; DWORD PageFaultCount; SIZE_T PeakWorkingSetSize; SIZE_T WorkingSetSize; SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage; SIZE_T QuotaPeakNonPagedPoolUsage; SIZE_T QuotaNonPagedPoolUsage; SIZE_T PagefileUsage; SIZE_T PeakPagefileUsage; SIZE_T PrivateUsage; } PROCESS_MEMORY_COUNTERS_EX, *PPROCESS_MEMORY_COUNTERS_EX;
BOOL GetProcessMemoryInfo(HANDLE hProcess, PPROCESS_MEMORY_COUNTERS ppsmemCounters, DWORD cb);
/* che do man hinh (S3Client PaintFps=-1: EnumDisplaySettings -> tan so man hinh) */
#ifndef ENUM_CURRENT_SETTINGS
#define ENUM_CURRENT_SETTINGS ((DWORD)-1)
#define ENUM_REGISTRY_SETTINGS ((DWORD)-2)
#endif
typedef struct _devicemodeA { BYTE dmDeviceName[32]; WORD dmSpecVersion; WORD dmDriverVersion; WORD dmSize; WORD dmDriverExtra; DWORD dmFields;
    LONG dmPositionX; LONG dmPositionY; DWORD dmDisplayOrientation; DWORD dmDisplayFixedOutput; short dmColor; short dmDuplex; short dmYResolution;
    short dmTTOption; short dmCollate; BYTE dmFormName[32]; WORD dmLogPixels; DWORD dmBitsPerPel; DWORD dmPelsWidth; DWORD dmPelsHeight; DWORD dmDisplayFlags;
    DWORD dmDisplayFrequency; DWORD dmICMMethod; DWORD dmICMIntent; DWORD dmMediaType; DWORD dmDitherType; DWORD dmReserved1; DWORD dmReserved2;
    DWORD dmPanningWidth; DWORD dmPanningHeight; } DEVMODEA, *PDEVMODEA, *LPDEVMODEA, DEVMODE, *LPDEVMODE;
BOOL EnumDisplaySettingsA(LPCSTR lpszDeviceName, DWORD iModeNum, LPDEVMODEA lpDevMode);
#ifndef EnumDisplaySettings
#define EnumDisplaySettings EnumDisplaySettingsA
#endif
/* MsgWaitForMultipleObjects (KWin32App::Run - khong dung tren SDL) */
#ifndef QS_ALLEVENTS
#define QS_KEY 0x0001
#define QS_MOUSEMOVE 0x0002
#define QS_MOUSEBUTTON 0x0004
#define QS_POSTMESSAGE 0x0008
#define QS_TIMER 0x0010
#define QS_PAINT 0x0020
#define QS_SENDMESSAGE 0x0040
#define QS_HOTKEY 0x0080
#define QS_ALLPOSTMESSAGE 0x0100
#define QS_MOUSE (QS_MOUSEMOVE | QS_MOUSEBUTTON)
#define QS_INPUT (QS_MOUSE | QS_KEY)
#define QS_ALLEVENTS (QS_INPUT | QS_POSTMESSAGE | QS_TIMER | QS_PAINT | QS_HOTKEY)
#define QS_ALLINPUT (QS_INPUT | QS_POSTMESSAGE | QS_TIMER | QS_PAINT | QS_HOTKEY | QS_SENDMESSAGE)
#endif
DWORD MsgWaitForMultipleObjects(DWORD nCount, const HANDLE* pHandles, BOOL fWaitAll, DWORD dwMilliseconds, DWORD dwWakeMask);
/* SetWindowPos */
#ifndef HWND_TOPMOST
#define HWND_TOP ((HWND)(uintptr_t)0)
#define HWND_BOTTOM ((HWND)(uintptr_t)1)
#define HWND_TOPMOST ((HWND)(uintptr_t)-1)
#define HWND_NOTOPMOST ((HWND)(uintptr_t)-2)
#define SWP_NOSIZE 0x0001
#define SWP_NOMOVE 0x0002
#define SWP_NOZORDER 0x0004
#define SWP_NOREDRAW 0x0008
#define SWP_NOACTIVATE 0x0010
#define SWP_FRAMECHANGED 0x0020
#define SWP_SHOWWINDOW 0x0040
#define SWP_HIDEWINDOW 0x0080
#define SWP_NOOWNERZORDER 0x0200
#endif
/* KWin32App::Run/MsgProc (khong chay tren SDL, chi de bien dich) */
static inline BOOL WaitMessage(void) { return 1; }
#ifndef HTCLIENT
#define HTERROR (-2)
#define HTTRANSPARENT (-1)
#define HTNOWHERE 0
#define HTCLIENT 1
#define HTCAPTION 2
#endif
#ifndef WM_INPUTLANGCHANGEREQUEST
#define WM_INPUTLANGCHANGEREQUEST 0x0050
#define WM_INPUTLANGCHANGE 0x0051
#endif
#ifndef SC_KEYMENU
#define SC_SIZE 0xF000
#define SC_MOVE 0xF010
#define SC_MINIMIZE 0xF020
#define SC_MAXIMIZE 0xF030
#define SC_CLOSE 0xF060
#define SC_KEYMENU 0xF100
#define SC_RESTORE 0xF120
#define SC_SCREENSAVE 0xF140
#define SC_MONITORPOWER 0xF170
#endif
/* bo nho chia se voi WAuto (Local\Auto_Name_MMFSV_<pid>) - khong co tren Android: tra NULL, ma goi da kiem NULL */
#ifndef EVENT_MODIFY_STATE
#define EVENT_MODIFY_STATE 0x0002
#define EVENT_ALL_ACCESS 0x1F0003
#endif
#ifndef PAGE_READWRITE
#define PAGE_READONLY 0x02
#define PAGE_READWRITE 0x04
#endif
/* CreateFileMapping(INVALID_HANDLE_VALUE, ..., ten) = bo nho chia se co ten (WAuto) -> tren Android gia lap bang bo nho thuong
   trong tien trinh (KMyApp::InitMapping phai thanh cong); OpenFileMapping (phia WAuto) luon NULL */
HANDLE CreateFileMappingA(HANDLE h, LPSECURITY_ATTRIBUTES sa, DWORD prot, DWORD hi, DWORD lo, LPCSTR name);
HANDLE OpenFileMappingA(DWORD access, BOOL inherit, LPCSTR name);
LPVOID MapViewOfFile(HANDLE h, DWORD access, DWORD hi, DWORD lo, SIZE_T n);
BOOL   UnmapViewOfFile(LPCVOID p);
static inline HANDLE OpenEventA(DWORD access, BOOL inherit, LPCSTR name) { (void)access; (void)inherit; (void)name; return (HANDLE)0; }
static inline HANDLE OpenMutexA(DWORD access, BOOL inherit, LPCSTR name) { (void)access; (void)inherit; (void)name; return (HANDLE)0; }
static inline HANDLE CreateMutexA(LPSECURITY_ATTRIBUTES sa, BOOL own, LPCSTR name) { (void)sa; (void)own; (void)name; return (HANDLE)0; }
static inline BOOL ReleaseMutex(HANDLE h) { (void)h; return 1; }
#ifndef CreateFileMapping
#define CreateFileMapping CreateFileMappingA
#define OpenFileMapping OpenFileMappingA
#define OpenEvent OpenEventA
#define OpenMutex OpenMutexA
#define CreateMutex CreateMutexA
#endif
/* sscanf_s: ma game chi dung voi %f/%d (KIniFile GetFloat3/4) - khong co %s/%c nen cung ngu nghia sscanf */
#ifndef sscanf_s
#define sscanf_s sscanf
#endif
/* CRT luong cua MSVC */
uintptr_t _beginthreadex(void* security, unsigned stack_size, unsigned (*start_address)(void*), void* arglist, unsigned initflag, unsigned* thrdaddr);
void _endthreadex(unsigned retval);
/* Winsock: loi cuoi = errno (ham that de '::WSAGetLastError()' bien dich duoc) */
static inline int WSAGetLastError(void) { return errno; }
static inline void WSASetLastError(int e) { errno = e; }

/* ---- ten khong hau to A (ban MultiByte) ---- */
#ifndef CreateEvent
#define CreateEvent CreateEventA
#endif
#ifndef LoadLibrary
#define LoadLibrary LoadLibraryA
#define LoadLibraryEx LoadLibraryExA
#endif
#ifndef GetModuleHandle
#define GetModuleHandle GetModuleHandleA
#define GetModuleFileName GetModuleFileNameA
#endif
#ifndef GetCommandLine
#define GetCommandLine GetCommandLineA
#endif
#ifndef FormatMessage
#define FormatMessage FormatMessageA
#endif
#ifndef OutputDebugString
#define OutputDebugString OutputDebugStringA
#endif
#ifndef GetCurrentDirectory
#define GetCurrentDirectory GetCurrentDirectoryA
#define SetCurrentDirectory SetCurrentDirectoryA
#define CreateDirectory CreateDirectoryA
#define RemoveDirectory RemoveDirectoryA
#define DeleteFile DeleteFileA
#define CopyFile CopyFileA
#define MoveFile MoveFileA
#define MoveFileEx MoveFileExA
#define GetFileAttributes GetFileAttributesA
#define SetFileAttributes SetFileAttributesA
#define GetTempPath GetTempPathA
#define GetTempFileName GetTempFileNameA
#define GetFullPathName GetFullPathNameA
#define FindFirstFile FindFirstFileA
#define FindNextFile FindNextFileA
#define GetDiskFreeSpaceEx GetDiskFreeSpaceExA
#define GetSystemDirectory GetSystemDirectoryA
#define GetWindowsDirectory GetWindowsDirectoryA
#define GetEnvironmentVariable GetEnvironmentVariableA
#define SetEnvironmentVariable SetEnvironmentVariableA
#endif
#ifndef GetPrivateProfileInt
#define GetPrivateProfileInt GetPrivateProfileIntA
#define GetPrivateProfileString GetPrivateProfileStringA
#define WritePrivateProfileString WritePrivateProfileStringA
#endif
#ifndef GetComputerName
#define GetComputerName GetComputerNameA
#define GetUserName GetUserNameA
#define GetVersionEx GetVersionExA
#endif
#ifndef RegOpenKeyEx
#define RegOpenKeyEx RegOpenKeyExA
#define RegCreateKeyEx RegCreateKeyExA
#define RegQueryValueEx RegQueryValueExA
#define RegSetValueEx RegSetValueExA
#endif
#ifndef MapVirtualKey
#define MapVirtualKey MapVirtualKeyA
#define PostMessage PostMessageA
#define SendMessage SendMessageA
#define PeekMessage PeekMessageA
#define GetMessage GetMessageA
#define DispatchMessage DispatchMessageA
#define DefWindowProc DefWindowProcA
#define RegisterClass RegisterClassA
#define RegisterClassEx RegisterClassExA
#define CreateWindowEx CreateWindowExA
#define CreateWindow(cls, name, style, x, y, w, h, parent, menu, inst, param) CreateWindowExA(0, cls, name, style, x, y, w, h, parent, menu, inst, param)
#define FindWindow FindWindowA
#define FindWindowEx FindWindowExA
#define SetWindowText SetWindowTextA
#define GetWindowText GetWindowTextA
#define GetClassName GetClassNameA
#define GetWindowLong GetWindowLongA
#define SetWindowLong SetWindowLongA
#define GetWindowLongPtr GetWindowLongPtrA
#define SetWindowLongPtr SetWindowLongPtrA
#define LoadCursor LoadCursorA
#define LoadCursorFromFile LoadCursorFromFileA
#define LoadIcon LoadIconA
#define MessageBox MessageBoxA
#define ShellExecute ShellExecuteA
#define ShellExecuteEx ShellExecuteExA
#endif

#ifdef __cplusplus
}
/* Interlocked* voi 'long' (LP64: 64 bit) - ma cu dung 'long' thay LONG (XPackFile::ms_lReadCriticalInit, Common) */
static inline long InterlockedIncrement(long volatile* p) { return __sync_add_and_fetch(p, 1); }
static inline long InterlockedDecrement(long volatile* p) { return __sync_sub_and_fetch(p, 1); }
static inline long InterlockedExchange(long volatile* p, long v) { return __sync_lock_test_and_set(p, v); }
static inline long InterlockedExchangeAdd(long volatile* p, long v) { return __sync_fetch_and_add(p, v); }
static inline long InterlockedCompareExchange(long volatile* p, long v, long c) { return __sync_val_compare_and_swap(p, c, v); }
static inline unsigned long InterlockedIncrement(unsigned long volatile* p) { return __sync_add_and_fetch(p, 1); }
static inline unsigned long InterlockedDecrement(unsigned long volatile* p) { return __sync_sub_and_fetch(p, 1); }
static inline unsigned int InterlockedIncrement(unsigned int volatile* p) { return __sync_add_and_fetch(p, 1); }
static inline unsigned int InterlockedDecrement(unsigned int volatile* p) { return __sync_sub_and_fetch(p, 1); }
#endif
#endif /* !_WIN32 */
#endif /* KPOSIXWIN32_H */
