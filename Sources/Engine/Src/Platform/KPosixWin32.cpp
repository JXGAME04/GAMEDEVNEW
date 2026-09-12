/*===========================================================================
  KPosixWin32.cpp - [ANDROID 08/09] Cai dat tap con Win32 tren POSIX + SDL3 (xem KPosixWin32.h).
  Bien dich vao libEngine.so (chi khi JX_POSIX). Duong dan: JxPathPosix(). Handle: struct co "magic" o dau
  (su kien, luong CreateThread, FindFirstFile); HANDLE la cua KThread (SDL_Thread*) khong bao gio toi day.
===========================================================================*/
#if !defined(_WIN32)
#include "KPosixCompat.h"
#include <SDL3/SDL.h>
#include <dlfcn.h>
#include <dirent.h>
#include <fnmatch.h>
#include <time.h>
#include <signal.h>
#include <sys/statvfs.h>
#ifdef JX_IOS
/* [IOS 11/09] Apple khong co <sys/sysinfo.h>. Cung cap dung ten sysinfo() + struct sysinfo o day
   de GlobalMemoryStatus / GlobalMemoryStatusEx ben duoi khong phai doi mot dong nao.
   Tong RAM lay qua sysctl(HW_MEMSIZE); phan con trong lay qua thong ke trang cua mach. */
#include <sys/sysctl.h>
#include <mach/mach.h>
struct sysinfo { unsigned long totalram; unsigned long freeram; unsigned int mem_unit; };
static int sysinfo(struct sysinfo* si)
{
	if (!si) return -1;
	si->mem_unit = 1; si->totalram = 0; si->freeram = 0;
	uint64_t nTong = 0; size_t nCo = sizeof(nTong); int mib[2] = { CTL_HW, HW_MEMSIZE };
	if (sysctl(mib, 2, &nTong, &nCo, NULL, 0) == 0) si->totalram = (unsigned long)nTong;
	vm_size_t nTrang = 0; mach_port_t host = mach_host_self();
	vm_statistics64_data_t vm; mach_msg_type_number_t nDem = HOST_VM_INFO64_COUNT;
	if (host_page_size(host, &nTrang) == KERN_SUCCESS &&
	    host_statistics64(host, HOST_VM_INFO64, (host_info64_t)&vm, &nDem) == KERN_SUCCESS)
		si->freeram = (unsigned long)((uint64_t)(vm.free_count + vm.inactive_count) * (uint64_t)nTrang);
	return 0;
}
#else
#include <sys/sysinfo.h>
#endif
#ifdef __ANDROID__
#include <android/log.h>
#endif

/* huy macro cua KPosixWin32.h de goi ham CRT that trong tep nay */
#undef fopen
#undef access
#undef _access
#undef remove
#undef rename
#undef _mkdir
#undef _chdir
#undef unlink
#undef _unlink

#define JX_MAGIC_EVENT  0x5645584Au   /* 'JXEV' */
#define JX_MAGIC_THREAD 0x4854584Au   /* 'JXTH' */
#define JX_MAGIC_FIND   0x4446584Au   /* 'JXFD' */
#define JX_MAGIC_MAP    0x504D584Au   /* 'JXMP' */
struct JxMapping { uint32_t magic; size_t size; void* p; };

JxGetKeyStateFn g_pfnJxGetKeyState = NULL;
JxWinMsgFn      g_pfnJxWinMsg = NULL;
static char     s_szDataDir[1024] = "";
static void*    s_pMainWindow = NULL;
static __thread DWORD s_dwLastError = 0;

static void jx_log(const char* fmt, ...)
{
	char sz[1024]; va_list va; va_start(va, fmt); vsnprintf(sz, sizeof(sz), fmt, va); va_end(va);
#ifdef __ANDROID__
	__android_log_print(ANDROID_LOG_INFO, "JX1", "%s", sz);
#else
	fprintf(stderr, "[JX1] %s\n", sz);
#endif
}

/*---------------------------------------------------------------- thu muc du lieu / cua so / duong dan */
void JxPosix_SetDataDir(const char* pszDir)
{
	if (!pszDir) { s_szDataDir[0] = 0; return; }
	strncpy(s_szDataDir, pszDir, sizeof(s_szDataDir) - 1); s_szDataDir[sizeof(s_szDataDir) - 1] = 0;
	size_t n = strlen(s_szDataDir);
	while (n > 1 && s_szDataDir[n - 1] == '/') s_szDataDir[--n] = 0;
}
const char* JxPosix_DataDir(void) { return s_szDataDir; }
void  JxPosix_SetMainWindow(void* pSdlWindow) { s_pMainWindow = pSdlWindow; }
void* JxPosix_MainWindow(void) { return s_pMainWindow; }

char* JxPathPosix(const char* pszIn, char* pszOut, size_t nOut)
{
	if (!pszIn || !pszOut || nOut == 0) return pszOut;
	size_t i = 0, o = 0;
	/* "C:\..." / ".\..." -> bo o dia */
	if (isalpha((unsigned char)pszIn[0]) && pszIn[1] == ':') i = 2;
	while (pszIn[i] && o + 1 < nOut)
	{
		char c = pszIn[i++];
		if (c == '\\') c = '/';
		if (c == '/' && o > 0 && pszOut[o - 1] == '/') continue;   /* "//" -> "/" */
		pszOut[o++] = c;
	}
	pszOut[o] = 0;
	/* ha chu thuong: phan tuong doi (khong bat dau bang '/') hoac phan duoi thu muc du lieu */
	size_t nLow = 0;
	if (pszOut[0] != '/') nLow = 0;
	else
	{
		size_t nd = strlen(s_szDataDir);
		if (nd && strncmp(pszOut, s_szDataDir, nd) == 0 && (pszOut[nd] == '/' || pszOut[nd] == 0)) nLow = nd;
		else
		{
			/* duong dan he thong Android that: giu nguyen */
			static const char* s_goc[] = { "/storage/", "/sdcard/", "/proc/", "/dev/", "/system/", "/vendor/", "/apex/", "/product/", "/mnt/", "/tmp/",
			                               "/data/user/", "/data/data/", "/data/local/", "/data/app/", "/data/misc/", NULL };
			int bSys = 0;
			for (int i2 = 0; s_goc[i2]; i2++) if (strncmp(pszOut, s_goc[i2], strlen(s_goc[i2])) == 0) { bSys = 1; break; }
#ifdef JX_IOS
			/* [IOS 11/09] goc he thong cua iOS: /var/mobile/Containers (may that),
			   /Users/<ten>/Library/Developer/CoreSimulator (may ao). Khong co cho nay thi duong dan
			   tuyet doi cua iOS bi coi la tuong doi -> ha chu thuong + ghep thu muc du lieu -> mo tep hong. */
			if (!bSys)
			{
				static const char* s_gocIos[] = { "/var/", "/private/", "/Users/", "/Library/", "/Applications/", NULL };
				for (int i3 = 0; s_gocIos[i3]; i3++) if (strncmp(pszOut, s_gocIos[i3], strlen(s_gocIos[i3])) == 0) { bSys = 1; break; }
			}
#endif
			if (bSys) return pszOut;
			/* "\data\x.pak", "\settings\..." kieu Windows (goc o dia = thu muc game) -> tuong doi voi thu muc du lieu */
			if (nd && o + nd + 1 < nOut)
			{
				memmove(pszOut + nd, pszOut, o + 1); memcpy(pszOut, s_szDataDir, nd); o += nd; nLow = nd;
			}
			else { memmove(pszOut, pszOut + 1, o); o--; nLow = 0; }
		}
	}
	for (size_t k = nLow; k < o; k++) pszOut[k] = (char)tolower((unsigned char)pszOut[k]);
	/* Ten co byte >= 0x80 (thu muc GBK "\spr\Ui3\<GBK>\" hay TCVN3 trong ma nguon): tren dia, Windows da doi
	   byte ANSI (cp1252) -> Unicode, chep sang Android thanh UTF-8. Thu duong dan byte tho truoc (tep do game
	   tu ghi), khong co thi thu ban cp1252 -> UTF-8. Tep trong pak khong qua day (tim theo id bam). */
	{
		int bCao = 0;
		for (size_t k = nLow; k < o; k++) if ((unsigned char)pszOut[k] >= 0x80) { bCao = 1; break; }
		if (bCao && access(pszOut, F_OK) != 0)
		{
			static const unsigned short s_cp1252_80[32] = {
				0x20AC,0x0081,0x201A,0x0192,0x201E,0x2026,0x2020,0x2021,0x02C6,0x2030,0x0160,0x2039,0x0152,0x008D,0x017D,0x008F,
				0x0090,0x2018,0x2019,0x201C,0x201D,0x2022,0x2013,0x2014,0x02DC,0x2122,0x0161,0x203A,0x0153,0x009D,0x017E,0x0178 };
			char szU[1024]; size_t u = 0;
			for (size_t k = 0; k < o && u + 4 < sizeof(szU); k++)
			{
				unsigned int c = (unsigned char)pszOut[k];
				if (k < nLow || c < 0x80) { szU[u++] = (char)c; continue; }
				if (c < 0xA0) c = s_cp1252_80[c - 0x80];
				/* chuan_bi_du_lieu.ps1 ha chu thuong ca chu Latin-1 (ToLower) -> dong bo o day */
				if (c >= 0xC0 && c <= 0xDE && c != 0xD7) c += 0x20;
				else if (c == 0x160 || c == 0x152 || c == 0x17D) c += 1;
				else if (c == 0x178) c = 0xFF;
				if (c < 0x800) { szU[u++] = (char)(0xC0 | (c >> 6)); szU[u++] = (char)(0x80 | (c & 0x3F)); }
				else { szU[u++] = (char)(0xE0 | (c >> 12)); szU[u++] = (char)(0x80 | ((c >> 6) & 0x3F)); szU[u++] = (char)(0x80 | (c & 0x3F)); }
			}
			szU[u] = 0;
			if (u < nOut && access(szU, F_OK) == 0) memcpy(pszOut, szU, u + 1);
		}
	}
	return pszOut;
}
FILE* jx_fopen(const char* pszPath, const char* pszMode)
{
	char sz[1024]; return fopen(JxPathPosix(pszPath, sz, sizeof(sz)), pszMode);
}
int jx_access(const char* pszPath, int nMode) { char sz[1024]; return access(JxPathPosix(pszPath, sz, sizeof(sz)), nMode == 0 ? F_OK : nMode); }
int jx_remove(const char* pszPath) { char sz[1024]; return remove(JxPathPosix(pszPath, sz, sizeof(sz))); }
int jx_rename(const char* a, const char* b) { char s1[1024], s2[1024]; return rename(JxPathPosix(a, s1, sizeof(s1)), JxPathPosix(b, s2, sizeof(s2))); }
int jx_mkdir(const char* pszPath) { char sz[1024]; return mkdir(JxPathPosix(pszPath, sz, sizeof(sz)), 0775); }
int jx_chdir(const char* pszPath) { char sz[1024]; return chdir(JxPathPosix(pszPath, sz, sizeof(sz))); }

/*---------------------------------------------------------------- su kien / luong / handle */
struct JxEvent { uint32_t magic; int manual; int state; pthread_mutex_t mu; pthread_cond_t cv; };
struct JxThread { uint32_t magic; SDL_Thread* th; LPTHREAD_START_ROUTINE pfn; LPVOID param; DWORD exitcode; int joined; };
struct JxFind { uint32_t magic; DIR* dir; char szDir[1024]; char szPat[256]; };

HANDLE CreateEventA(LPSECURITY_ATTRIBUTES sa, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName)
{
	(void)sa; (void)lpName;
	JxEvent* e = (JxEvent*)calloc(1, sizeof(JxEvent));
	if (!e) return NULL;
	e->magic = JX_MAGIC_EVENT; e->manual = bManualReset ? 1 : 0; e->state = bInitialState ? 1 : 0;
	pthread_mutex_init(&e->mu, NULL); pthread_cond_init(&e->cv, NULL);
	return (HANDLE)e;
}
static JxEvent* jx_event(HANDLE h) { JxEvent* e = (JxEvent*)h; return (e && e->magic == JX_MAGIC_EVENT) ? e : NULL; }
BOOL SetEvent(HANDLE h)
{
	JxEvent* e = jx_event(h); if (!e) return FALSE;
	pthread_mutex_lock(&e->mu); e->state = 1; pthread_cond_broadcast(&e->cv); pthread_mutex_unlock(&e->mu); return TRUE;
}
BOOL ResetEvent(HANDLE h)
{
	JxEvent* e = jx_event(h); if (!e) return FALSE;
	pthread_mutex_lock(&e->mu); e->state = 0; pthread_mutex_unlock(&e->mu); return TRUE;
}
BOOL PulseEvent(HANDLE h)
{
	JxEvent* e = jx_event(h); if (!e) return FALSE;
	pthread_mutex_lock(&e->mu); pthread_cond_broadcast(&e->cv); e->state = 0; pthread_mutex_unlock(&e->mu); return TRUE;
}
DWORD WaitForSingleObject(HANDLE h, DWORD ms)
{
	if (!h) return WAIT_FAILED;
	uint32_t magic = *(uint32_t*)h;
	if (magic == JX_MAGIC_EVENT)
	{
		JxEvent* e = (JxEvent*)h; DWORD r = WAIT_OBJECT_0;
		pthread_mutex_lock(&e->mu);
		if (!e->state)
		{
			if (ms == 0) r = WAIT_TIMEOUT;
			else if (ms == INFINITE) { while (!e->state) pthread_cond_wait(&e->cv, &e->mu); }
			else
			{
				struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts);
				ts.tv_sec += ms / 1000; ts.tv_nsec += (long)(ms % 1000) * 1000000L;
				if (ts.tv_nsec >= 1000000000L) { ts.tv_sec++; ts.tv_nsec -= 1000000000L; }
				while (!e->state) { if (pthread_cond_timedwait(&e->cv, &e->mu, &ts) == ETIMEDOUT) break; }
				if (!e->state) r = WAIT_TIMEOUT;
			}
		}
		if (r == WAIT_OBJECT_0 && !e->manual) e->state = 0;
		pthread_mutex_unlock(&e->mu);
		return r;
	}
	if (magic == JX_MAGIC_THREAD)
	{
		JxThread* t = (JxThread*)h;
		if (t->joined) return WAIT_OBJECT_0;
		if (ms == 0) return WAIT_TIMEOUT;   /* khong co "poll" cho SDL_Thread: coi nhu con chay (KSubWorld khong tao luong nay) */
		SDL_WaitThread(t->th, NULL); t->joined = 1; t->th = NULL;
		return WAIT_OBJECT_0;
	}
	return WAIT_OBJECT_0;
}
DWORD WaitForMultipleObjects(DWORD n, const HANDLE* p, BOOL bAll, DWORD ms)
{
	(void)bAll;
	for (DWORD i = 0; i < n; i++) { if (WaitForSingleObject(p[i], 0) == WAIT_OBJECT_0) return WAIT_OBJECT_0 + i; }
	if (ms) { for (DWORD i = 0; i < n; i++) { if (WaitForSingleObject(p[i], ms) == WAIT_OBJECT_0) return WAIT_OBJECT_0 + i; } }
	return WAIT_TIMEOUT;
}
BOOL CloseHandle(HANDLE h)
{
	if (!h || h == INVALID_HANDLE_VALUE) return FALSE;
	uint32_t magic = *(uint32_t*)h;
	if (magic == JX_MAGIC_EVENT) { JxEvent* e = (JxEvent*)h; pthread_cond_destroy(&e->cv); pthread_mutex_destroy(&e->mu); e->magic = 0; free(e); return TRUE; }
	if (magic == JX_MAGIC_THREAD) { JxThread* t = (JxThread*)h; if (t->th && !t->joined) SDL_DetachThread(t->th); t->magic = 0; free(t); return TRUE; }
	if (magic == JX_MAGIC_FIND) return FindClose(h);
	if (magic == JX_MAGIC_MAP) { JxMapping* m = (JxMapping*)h; free(m->p); m->magic = 0; free(m); return TRUE; }
	return TRUE;
}
static int SDLCALL jx_thread_thunk(void* p)
{
	JxThread* t = (JxThread*)p;
	t->exitcode = t->pfn ? t->pfn(t->param) : 0;
	return (int)t->exitcode;
}
HANDLE CreateThread(LPSECURITY_ATTRIBUTES sa, SIZE_T stack, LPTHREAD_START_ROUTINE pfn, LPVOID param, DWORD flags, LPDWORD pid)
{
	(void)sa; (void)stack; (void)flags;
	JxThread* t = (JxThread*)calloc(1, sizeof(JxThread));
	if (!t) return NULL;
	t->magic = JX_MAGIC_THREAD; t->pfn = pfn; t->param = param; t->exitcode = STILL_ACTIVE;
	t->th = SDL_CreateThread(jx_thread_thunk, "Win32Thread", t);
	if (!t->th) { free(t); return NULL; }
	if (pid) *pid = (DWORD)SDL_GetThreadID(t->th);
	return (HANDLE)t;
}
BOOL GetExitCodeThread(HANDLE h, LPDWORD p) { JxThread* t = (JxThread*)h; if (!t || t->magic != JX_MAGIC_THREAD) return FALSE; if (p) *p = t->exitcode; return TRUE; }
BOOL SetThreadPriority(HANDLE h, int n) { (void)h; (void)n; return TRUE; }
int  GetThreadPriority(HANDLE h) { (void)h; return 0; }
DWORD GetCurrentThreadId(void) { return (DWORD)SDL_GetCurrentThreadID(); }
DWORD GetCurrentProcessId(void) { return (DWORD)getpid(); }
HANDLE GetCurrentProcess(void) { return (HANDLE)(uintptr_t)-1; }
HANDLE GetCurrentThread(void) { return (HANDLE)(uintptr_t)-2; }
DWORD SleepEx(DWORD ms, BOOL b) { (void)b; SDL_Delay(ms); return 0; }
#ifndef Sleep
void Sleep(DWORD ms) { SDL_Delay(ms); }
#endif
DWORD GetTickCount(void) { return (DWORD)SDL_GetTicks(); }
DWORD timeGetTime(void) { return (DWORD)SDL_GetTicks(); }
UINT timeBeginPeriod(UINT u) { (void)u; return TIMERR_NOERROR; }
UINT timeEndPeriod(UINT u) { (void)u; return TIMERR_NOERROR; }
void ExitProcess(UINT u) { exit((int)u); }
BOOL TerminateProcess(HANDLE h, UINT u) { (void)h; exit((int)u); return TRUE; }
DWORD GetLastError(void) { return s_dwLastError ? s_dwLastError : (DWORD)errno; }
void SetLastError(DWORD e) { s_dwLastError = e; }
DWORD FormatMessageA(DWORD f, LPCVOID s, DWORD id, DWORD lang, LPSTR buf, DWORD n, void* a)
{
	(void)f; (void)s; (void)lang; (void)a;
	if (!buf || n == 0) return 0;
	snprintf(buf, n, "loi %u: %s", (unsigned)id, strerror((int)id));
	return (DWORD)strlen(buf);
}
BOOL IsDebuggerPresent(void) { return FALSE; }
void DebugBreak(void) { raise(SIGTRAP); }
void OutputDebugStringA(LPCSTR s) { if (s) jx_log("%s", s); }
LPVOID SetUnhandledExceptionFilter(LPVOID p) { (void)p; return NULL; }

/*---------------------------------------------------------------- thoi gian */
static void jx_tm_to_st(const struct tm* t, int ms, LPSYSTEMTIME st)
{
	st->wYear = (WORD)(t->tm_year + 1900); st->wMonth = (WORD)(t->tm_mon + 1); st->wDayOfWeek = (WORD)t->tm_wday; st->wDay = (WORD)t->tm_mday;
	st->wHour = (WORD)t->tm_hour; st->wMinute = (WORD)t->tm_min; st->wSecond = (WORD)t->tm_sec; st->wMilliseconds = (WORD)ms;
}
static void jx_st_to_tm(const SYSTEMTIME* st, struct tm* t)
{
	memset(t, 0, sizeof(*t));
	t->tm_year = st->wYear - 1900; t->tm_mon = st->wMonth - 1; t->tm_mday = st->wDay;
	t->tm_hour = st->wHour; t->tm_min = st->wMinute; t->tm_sec = st->wSecond; t->tm_isdst = -1;
}
void GetLocalTime(LPSYSTEMTIME st)
{
	struct timeval tv; gettimeofday(&tv, NULL); struct tm t; time_t s = tv.tv_sec; localtime_r(&s, &t);
	jx_tm_to_st(&t, (int)(tv.tv_usec / 1000), st);
}
void GetSystemTime(LPSYSTEMTIME st)
{
	struct timeval tv; gettimeofday(&tv, NULL); struct tm t; time_t s = tv.tv_sec; gmtime_r(&s, &t);
	jx_tm_to_st(&t, (int)(tv.tv_usec / 1000), st);
}
/* FILETIME: so 100 ns tu 1601-01-01 UTC */
static const uint64_t JX_EPOCH_DIFF = 116444736000000000ULL;
static void jx_unix_to_ft(uint64_t sec, uint32_t nsec, LPFILETIME ft)
{
	uint64_t v = sec * 10000000ULL + nsec / 100 + JX_EPOCH_DIFF;
	ft->dwLowDateTime = (DWORD)(v & 0xFFFFFFFFu); ft->dwHighDateTime = (DWORD)(v >> 32);
}
static int64_t jx_ft_to_unix(const FILETIME* ft)
{
	uint64_t v = ((uint64_t)ft->dwHighDateTime << 32) | ft->dwLowDateTime;
	return (int64_t)((v - JX_EPOCH_DIFF) / 10000000ULL);
}
void GetSystemTimeAsFileTime(LPFILETIME ft) { struct timeval tv; gettimeofday(&tv, NULL); jx_unix_to_ft((uint64_t)tv.tv_sec, (uint32_t)tv.tv_usec * 1000, ft); }
BOOL SystemTimeToFileTime(const SYSTEMTIME* st, LPFILETIME ft)
{
	struct tm t; jx_st_to_tm(st, &t); time_t s = timegm(&t); if (s == (time_t)-1) return FALSE;
	jx_unix_to_ft((uint64_t)s, (uint32_t)st->wMilliseconds * 1000000u, ft); return TRUE;
}
BOOL FileTimeToSystemTime(const FILETIME* ft, LPSYSTEMTIME st)
{
	int64_t s = jx_ft_to_unix(ft); time_t ts = (time_t)s; struct tm t; gmtime_r(&ts, &t);
	uint64_t v = ((uint64_t)ft->dwHighDateTime << 32) | ft->dwLowDateTime;
	jx_tm_to_st(&t, (int)(((v - JX_EPOCH_DIFF) / 10000ULL) % 1000ULL), st); return TRUE;
}
static long jx_gmtoff(void) { time_t now = time(NULL); struct tm t; localtime_r(&now, &t); return t.tm_gmtoff; }
BOOL FileTimeToLocalFileTime(const FILETIME* ft, LPFILETIME lft)
{
	uint64_t v = ((uint64_t)ft->dwHighDateTime << 32) | ft->dwLowDateTime; v += (uint64_t)((int64_t)jx_gmtoff() * 10000000LL);
	lft->dwLowDateTime = (DWORD)(v & 0xFFFFFFFFu); lft->dwHighDateTime = (DWORD)(v >> 32); return TRUE;
}
BOOL LocalFileTimeToFileTime(const FILETIME* lft, LPFILETIME ft)
{
	uint64_t v = ((uint64_t)lft->dwHighDateTime << 32) | lft->dwLowDateTime; v -= (uint64_t)((int64_t)jx_gmtoff() * 10000000LL);
	ft->dwLowDateTime = (DWORD)(v & 0xFFFFFFFFu); ft->dwHighDateTime = (DWORD)(v >> 32); return TRUE;
}
BOOL SystemTimeToTzSpecificLocalTime(const TIME_ZONE_INFORMATION* tz, const SYSTEMTIME* ut, LPSYSTEMTIME lt)
{
	(void)tz; struct tm t; jx_st_to_tm(ut, &t); time_t s = timegm(&t); if (s == (time_t)-1) return FALSE;
	struct tm l; localtime_r(&s, &l); jx_tm_to_st(&l, ut->wMilliseconds, lt); return TRUE;
}
BOOL TzSpecificLocalTimeToSystemTime(const TIME_ZONE_INFORMATION* tz, const SYSTEMTIME* lt, LPSYSTEMTIME ut)
{
	(void)tz; struct tm t; jx_st_to_tm(lt, &t); time_t s = mktime(&t); if (s == (time_t)-1) return FALSE;
	struct tm g; gmtime_r(&s, &g); jx_tm_to_st(&g, lt->wMilliseconds, ut); return TRUE;
}
DWORD GetTimeZoneInformation(LPTIME_ZONE_INFORMATION tz) { if (tz) { memset(tz, 0, sizeof(*tz)); tz->Bias = (LONG)(-jx_gmtoff() / 60); } return 0; }
BOOL QueryPerformanceCounter(LARGE_INTEGER* p) { if (p) p->QuadPart = (LONGLONG)SDL_GetPerformanceCounter(); return TRUE; }
/* [ANDROID 11/09 MANG] thoi gian CPU: Windows tra don vi 100 ns trong FILETIME (kernel = he thong, user = nguoi dung). */
static void jx_dat_ft(LPFILETIME p, unsigned long long u100ns)
{
	if (!p) return;
	p->dwLowDateTime  = (DWORD)(u100ns & 0xFFFFFFFFull);
	p->dwHighDateTime = (DWORD)(u100ns >> 32);
}
BOOL GetProcessTimes(HANDLE hProcess, LPFILETIME lpCreation, LPFILETIME lpExit, LPFILETIME lpKernel, LPFILETIME lpUser)
{
	/* /proc/self/stat: truong 14 utime, 15 stime (don vi tick = 1/CLK_TCK giay) */
	unsigned long long ut = 0, st = 0;
	long clk = sysconf(_SC_CLK_TCK);
	FILE* f;

	(void)hProcess;
	jx_dat_ft(lpCreation, 0); jx_dat_ft(lpExit, 0);
	if (clk <= 0) clk = 100;
	f = fopen("/proc/self/stat", "r");
	if (f)
	{
		char sz[1024] = { 0 };
		if (fgets(sz, sizeof(sz) - 1, f))
		{
			char* p = strrchr(sz, ')');	/* bo qua ten tien trinh trong ngoac */
			if (p)
			{
				char* tok = strtok(p + 1, " ");
				int i;
				for (i = 3; tok && i <= 15; i++)
				{
					if (i == 14) ut = strtoull(tok, NULL, 10);
					if (i == 15) st = strtoull(tok, NULL, 10);
					tok = strtok(NULL, " ");
				}
			}
		}
		fclose(f);
	}
	jx_dat_ft(lpKernel, st * 10000000ull / (unsigned long long)clk);
	jx_dat_ft(lpUser,   ut * 10000000ull / (unsigned long long)clk);
	return TRUE;
}
BOOL GetThreadTimes(HANDLE hThread, LPFILETIME lpCreation, LPFILETIME lpExit, LPFILETIME lpKernel, LPFILETIME lpUser)
{
	struct timespec ts;

	(void)hThread;
	jx_dat_ft(lpCreation, 0); jx_dat_ft(lpExit, 0); jx_dat_ft(lpKernel, 0);
	if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts) != 0)
	{
		jx_dat_ft(lpUser, 0);
		return FALSE;
	}
	jx_dat_ft(lpUser, (unsigned long long)ts.tv_sec * 10000000ull + (unsigned long long)ts.tv_nsec / 100ull);
	return TRUE;
}
BOOL QueryPerformanceFrequency(LARGE_INTEGER* p) { if (p) p->QuadPart = (LONGLONG)SDL_GetPerformanceFrequency(); return TRUE; }
LONG CompareFileTime(const FILETIME* a, const FILETIME* b)
{
	uint64_t x = ((uint64_t)a->dwHighDateTime << 32) | a->dwLowDateTime, y = ((uint64_t)b->dwHighDateTime << 32) | b->dwLowDateTime;
	return x < y ? -1 : (x > y ? 1 : 0);
}

/*---------------------------------------------------------------- module: "Rainbow.dll" -> libRainbow.so */
HMODULE LoadLibraryA(LPCSTR name)
{
	if (!name) return NULL;
	char base[256]; const char* p = strrchr(name, '\\'); const char* q = strrchr(name, '/');
	const char* f = p > q ? p + 1 : (q ? q + 1 : name);
	strncpy(base, f, sizeof(base) - 1); base[sizeof(base) - 1] = 0;
	size_t n = strlen(base);
	if (n > 4 && strcasecmp(base + n - 4, ".dll") == 0) base[n - 4] = 0;
	else if (n > 3 && strcasecmp(base + n - 3, ".so") == 0) base[n - 3] = 0;
	if (strncmp(base, "lib", 3) == 0) memmove(base, base + 3, strlen(base) - 2);
	char cand[3][300];
	snprintf(cand[0], sizeof(cand[0]), "lib%s.so", base);
	char low[256]; strncpy(low, base, sizeof(low)); low[sizeof(low) - 1] = 0; for (char* c = low; *c; c++) *c = (char)tolower((unsigned char)*c);
	snprintf(cand[1], sizeof(cand[1]), "lib%s.so", low);
	snprintf(cand[2], sizeof(cand[2]), "%s", name);
	for (int i = 0; i < 3; i++)
	{
		void* h = dlopen(cand[i], RTLD_NOW | RTLD_GLOBAL);
		if (h) return (HMODULE)h;
	}
	jx_log("LoadLibrary(%s) that bai: %s", name, dlerror());
	SetLastError(ERROR_FILE_NOT_FOUND);
	return NULL;
}
HMODULE LoadLibraryExA(LPCSTR name, HANDLE h, DWORD f) { (void)h; (void)f; return LoadLibraryA(name); }
FARPROC GetProcAddress(HMODULE h, LPCSTR name) { if (!h || !name) return NULL; return (FARPROC)dlsym((void*)h, name); }
BOOL FreeLibrary(HMODULE h) { if (!h) return FALSE; return dlclose((void*)h) == 0; }
HMODULE GetModuleHandleA(LPCSTR name)
{
	if (!name) return (HMODULE)dlopen(NULL, RTLD_NOW);
	HMODULE h = LoadLibraryA(name); if (h) dlclose((void*)h);   /* dlopen dem tham chieu: tra handle da nap */
	return h;
}
DWORD GetModuleFileNameA(HMODULE h, LPSTR buf, DWORD n)
{
	(void)h;
	if (!buf || n == 0) return 0;
	if (s_szDataDir[0]) snprintf(buf, n, "%s/GameSDL.exe", s_szDataDir);   /* thu muc "canh Game.exe" = thu muc du lieu */
	else
	{
		Dl_info info; if (dladdr((void*)&GetModuleFileNameA, &info) && info.dli_fname) snprintf(buf, n, "%s", info.dli_fname);
		else snprintf(buf, n, "/proc/self/exe");
	}
	return (DWORD)strlen(buf);
}
LPSTR GetCommandLineA(void) { static char sz[] = "GameSDL.exe"; return sz; }

/*---------------------------------------------------------------- tep / thu muc */
DWORD GetCurrentDirectoryA(DWORD n, LPSTR buf) { if (!buf || !getcwd(buf, n)) return 0; return (DWORD)strlen(buf); }
BOOL SetCurrentDirectoryA(LPCSTR p) { return jx_chdir(p) == 0; }
BOOL CreateDirectoryA(LPCSTR p, LPSECURITY_ATTRIBUTES sa)
{
	(void)sa; if (jx_mkdir(p) == 0) return TRUE;
	SetLastError(errno == EEXIST ? ERROR_ALREADY_EXISTS : ERROR_PATH_NOT_FOUND); return FALSE;
}
BOOL RemoveDirectoryA(LPCSTR p) { char sz[1024]; return rmdir(JxPathPosix(p, sz, sizeof(sz))) == 0; }
BOOL DeleteFileA(LPCSTR p) { return jx_remove(p) == 0; }
BOOL CopyFileA(LPCSTR a, LPCSTR b, BOOL bFail)
{
	char s1[1024], s2[1024]; JxPathPosix(a, s1, sizeof(s1)); JxPathPosix(b, s2, sizeof(s2));
	if (bFail && access(s2, F_OK) == 0) { SetLastError(ERROR_ALREADY_EXISTS); return FALSE; }
	FILE* fi = fopen(s1, "rb"); if (!fi) return FALSE;
	FILE* fo = fopen(s2, "wb"); if (!fo) { fclose(fi); return FALSE; }
	char buf[65536]; size_t r; BOOL ok = TRUE;
	while ((r = fread(buf, 1, sizeof(buf), fi)) > 0) { if (fwrite(buf, 1, r, fo) != r) { ok = FALSE; break; } }
	fclose(fi); fclose(fo); return ok;
}
BOOL MoveFileA(LPCSTR a, LPCSTR b) { return jx_rename(a, b) == 0; }
BOOL MoveFileExA(LPCSTR a, LPCSTR b, DWORD f) { (void)f; return jx_rename(a, b) == 0; }
DWORD GetFileAttributesA(LPCSTR p)
{
	char sz[1024]; struct stat st;
	if (stat(JxPathPosix(p, sz, sizeof(sz)), &st) != 0) { SetLastError(ERROR_FILE_NOT_FOUND); return INVALID_FILE_ATTRIBUTES; }
	DWORD a = S_ISDIR(st.st_mode) ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_ARCHIVE;
	if (!(st.st_mode & S_IWUSR)) a |= FILE_ATTRIBUTE_READONLY;
	return a;
}
BOOL SetFileAttributesA(LPCSTR p, DWORD a) { (void)p; (void)a; return TRUE; }
DWORD GetTempPathA(DWORD n, LPSTR buf)
{
	if (!buf || n == 0) return 0;
	if (s_szDataDir[0]) { snprintf(buf, n, "%s/tmp/", s_szDataDir); char sz[1024]; snprintf(sz, sizeof(sz), "%s/tmp", s_szDataDir); mkdir(sz, 0775); }
	else snprintf(buf, n, "%s/", getenv("TMPDIR") ? getenv("TMPDIR") : "/data/local/tmp");
	return (DWORD)strlen(buf);
}
UINT GetTempFileNameA(LPCSTR path, LPCSTR pre, UINT u, LPSTR out)
{
	static unsigned s_n = 0; if (!out) return 0;
	unsigned v = u ? u : (++s_n + (unsigned)getpid() * 100u);
	snprintf(out, MAX_PATH, "%s%s%04x.tmp", path ? path : "", pre ? pre : "", v & 0xFFFF);
	if (!u) { FILE* f = fopen(out, "wb"); if (f) fclose(f); }
	return v;
}
DWORD GetFullPathNameA(LPCSTR p, DWORD n, LPSTR buf, LPSTR* part)
{
	char sz[1024]; JxPathPosix(p, sz, sizeof(sz));
	if (sz[0] != '/') { char cwd[1024]; if (getcwd(cwd, sizeof(cwd))) { char t[2048]; snprintf(t, sizeof(t), "%s/%s", cwd, sz); strncpy(sz, t, sizeof(sz) - 1); sz[sizeof(sz) - 1] = 0; } }
	if (buf && n) { strncpy(buf, sz, n - 1); buf[n - 1] = 0; if (part) { char* s = strrchr(buf, '/'); *part = s ? s + 1 : buf; } }
	return (DWORD)strlen(sz);
}
static BOOL jx_find_fill(JxFind* f, LPWIN32_FIND_DATAA d)
{
	struct dirent* e;
	while ((e = readdir(f->dir)) != NULL)
	{
		if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0) { if (fnmatch(f->szPat, e->d_name, 0) != 0) continue; }
		if (fnmatch(f->szPat, e->d_name, FNM_CASEFOLD) != 0) continue;
		memset(d, 0, sizeof(*d));
		strncpy(d->cFileName, e->d_name, MAX_PATH - 1);
		char full[2048]; snprintf(full, sizeof(full), "%s/%s", f->szDir, e->d_name);
		struct stat st;
		if (stat(full, &st) == 0)
		{
			d->dwFileAttributes = S_ISDIR(st.st_mode) ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_ARCHIVE;
			d->nFileSizeLow = (DWORD)(st.st_size & 0xFFFFFFFFu); d->nFileSizeHigh = (DWORD)((uint64_t)st.st_size >> 32);
			jx_unix_to_ft((uint64_t)st.st_mtime, 0, &d->ftLastWriteTime); d->ftCreationTime = d->ftLastAccessTime = d->ftLastWriteTime;
		}
		else d->dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE;
		return TRUE;
	}
	return FALSE;
}
HANDLE FindFirstFileA(LPCSTR pat, LPWIN32_FIND_DATAA d)
{
	if (!pat || !d) return INVALID_HANDLE_VALUE;
	char sz[1024]; JxPathPosix(pat, sz, sizeof(sz));
	JxFind* f = (JxFind*)calloc(1, sizeof(JxFind)); if (!f) return INVALID_HANDLE_VALUE;
	f->magic = JX_MAGIC_FIND;
	char* s = strrchr(sz, '/');
	if (s) { *s = 0; strncpy(f->szDir, sz[0] ? sz : "/", sizeof(f->szDir) - 1); strncpy(f->szPat, s + 1, sizeof(f->szPat) - 1); }
	else { strcpy(f->szDir, "."); strncpy(f->szPat, sz, sizeof(f->szPat) - 1); }
	if (!f->szPat[0]) strcpy(f->szPat, "*");
	f->dir = opendir(f->szDir);
	if (!f->dir || !jx_find_fill(f, d)) { if (f->dir) closedir(f->dir); free(f); SetLastError(ERROR_FILE_NOT_FOUND); return INVALID_HANDLE_VALUE; }
	return (HANDLE)f;
}
BOOL FindNextFileA(HANDLE h, LPWIN32_FIND_DATAA d)
{
	JxFind* f = (JxFind*)h; if (!f || h == INVALID_HANDLE_VALUE || f->magic != JX_MAGIC_FIND || !d) return FALSE;
	return jx_find_fill(f, d);
}
BOOL FindClose(HANDLE h)
{
	JxFind* f = (JxFind*)h; if (!f || h == INVALID_HANDLE_VALUE || f->magic != JX_MAGIC_FIND) return FALSE;
	if (f->dir) closedir(f->dir); f->magic = 0; free(f); return TRUE;
}
BOOL GetDiskFreeSpaceExA(LPCSTR dir, ULARGE_INTEGER* a, ULARGE_INTEGER* t, ULARGE_INTEGER* fr)
{
	char sz[1024]; struct statvfs v;
	if (statvfs(JxPathPosix(dir && dir[0] ? dir : ".", sz, sizeof(sz)), &v) != 0) return FALSE;
	if (a) a->QuadPart = (ULONGLONG)v.f_bavail * v.f_frsize;
	if (t) t->QuadPart = (ULONGLONG)v.f_blocks * v.f_frsize;
	if (fr) fr->QuadPart = (ULONGLONG)v.f_bfree * v.f_frsize;
	return TRUE;
}
UINT GetSystemDirectoryA(LPSTR buf, UINT n) { if (buf && n) snprintf(buf, n, "/system"); return 7; }
UINT GetWindowsDirectoryA(LPSTR buf, UINT n) { if (buf && n) snprintf(buf, n, "/system"); return 7; }
DWORD GetEnvironmentVariableA(LPCSTR name, LPSTR buf, DWORD n)
{
	const char* v = name ? getenv(name) : NULL; if (!v) { SetLastError(203); return 0; }
	if (buf && n) { strncpy(buf, v, n - 1); buf[n - 1] = 0; }
	return (DWORD)strlen(v);
}
BOOL SetEnvironmentVariableA(LPCSTR name, LPCSTR v) { if (!name) return FALSE; return (v ? setenv(name, v, 1) : unsetenv(name)) == 0; }

/*---------------------------------------------------------------- INI */
static char* jx_trim(char* s)
{
	while (*s == ' ' || *s == '\t') s++;
	size_t n = strlen(s);
	while (n && (s[n - 1] == ' ' || s[n - 1] == '\t' || s[n - 1] == '\r' || s[n - 1] == '\n')) s[--n] = 0;
	return s;
}
/* tim [section] key = -> chep gia tri (khong co dau nhay) vao out; tra ve TRUE khi thay */
static BOOL jx_ini_get(LPCSTR sec, LPCSTR key, LPCSTR file, LPSTR out, DWORD n)
{
	FILE* f = jx_fopen(file, "rb");
	if (!f)
	{	/* [ANDROID 11/09 c] mo ini hong -> ghi errno (toi da 20 dong) de biet vi sao config.ini co luc khong doc duoc */
		static int s_nIniLoi = 0;
		if (s_nIniLoi < 20) { s_nIniLoi++; jx_log("[INI] khong mo duoc %s: errno %d (%s)", file, errno, strerror(errno)); }
		return FALSE;
	}
	char line[2048]; int in = 0; BOOL found = FALSE;
	while (fgets(line, sizeof(line), f))
	{
		char* s = jx_trim(line);
		if (s[0] == ';' || s[0] == '#' || !s[0]) continue;
		if (s[0] == '[')
		{
			char* e = strchr(s, ']'); if (e) *e = 0;
			in = (strcasecmp(jx_trim(s + 1), sec) == 0);
			continue;
		}
		if (!in) continue;
		char* eq = strchr(s, '='); if (!eq) continue;
		*eq = 0;
		if (strcasecmp(jx_trim(s), key) != 0) continue;
		char* v = jx_trim(eq + 1);
		size_t vn = strlen(v);
		if (vn >= 2 && ((v[0] == '"' && v[vn - 1] == '"') || (v[0] == '\'' && v[vn - 1] == '\''))) { v[vn - 1] = 0; v++; }
		if (out && n) { strncpy(out, v, n - 1); out[n - 1] = 0; }
		found = TRUE; break;
	}
	fclose(f); return found;
}
UINT GetPrivateProfileIntA(LPCSTR sec, LPCSTR key, INT def, LPCSTR file)
{
	char v[64]; if (!sec || !key || !file || !jx_ini_get(sec, key, file, v, sizeof(v))) return (UINT)def;
	return (UINT)strtol(v, NULL, 0);
}
DWORD GetPrivateProfileStringA(LPCSTR sec, LPCSTR key, LPCSTR def, LPSTR out, DWORD n, LPCSTR file)
{
	if (!out || n == 0) return 0;
	if (!sec || !key || !file || !jx_ini_get(sec, key, file, out, n)) { strncpy(out, def ? def : "", n - 1); out[n - 1] = 0; }
	return (DWORD)strlen(out);
}
BOOL WritePrivateProfileStringA(LPCSTR sec, LPCSTR key, LPCSTR val, LPCSTR file)
{
	if (!sec || !file) return FALSE;
	/* doc toan bo, viet lai voi gia tri moi (don gian, du cho vai khoa cau hinh) */
	FILE* f = jx_fopen(file, "rb"); char* txt = NULL; size_t len = 0;
	if (f) { fseek(f, 0, SEEK_END); len = (size_t)ftell(f); fseek(f, 0, SEEK_SET); txt = (char*)malloc(len + 1); if (txt) { len = fread(txt, 1, len, f); txt[len] = 0; } fclose(f); }
	FILE* o = jx_fopen(file, "wb"); if (!o) { free(txt); return FALSE; }
	int in = 0, done = 0, secfound = 0; char* p = txt;
	while (p && *p)
	{
		char* nl = strchr(p, '\n'); size_t ln = nl ? (size_t)(nl - p + 1) : strlen(p);
		char line[2048]; size_t c = ln < sizeof(line) - 1 ? ln : sizeof(line) - 1; memcpy(line, p, c); line[c] = 0;
		char tmp[2048]; strcpy(tmp, line); char* s = jx_trim(tmp);
		if (s[0] == '[')
		{
			if (in && !done && key) { fprintf(o, "%s=%s\r\n", key, val ? val : ""); done = 1; }
			char* e = strchr(s, ']'); if (e) *e = 0;
			in = (strcasecmp(jx_trim(s + 1), sec) == 0); if (in) secfound = 1;
			fputs(line, o);
		}
		else if (in && key && s[0] && s[0] != ';' && s[0] != '#' && strchr(s, '='))
		{
			char* eq = strchr(s, '='); *eq = 0;
			if (strcasecmp(jx_trim(s), key) == 0) { if (!done && val) fprintf(o, "%s=%s\r\n", key, val); done = 1; }
			else fputs(line, o);
		}
		else fputs(line, o);
		p += ln;
	}
	if (key && !done) { if (!secfound) fprintf(o, "\r\n[%s]\r\n", sec); if (val) fprintf(o, "%s=%s\r\n", key, val); }
	fclose(o); free(txt); return TRUE;
}

/*---------------------------------------------------------------- he thong / registry */
BOOL GetComputerNameA(LPSTR buf, LPDWORD n)
{
	if (!buf || !n) return FALSE; char h[256] = "android"; gethostname(h, sizeof(h)); h[sizeof(h) - 1] = 0;
	if (*n <= strlen(h)) { *n = (DWORD)strlen(h) + 1; return FALSE; }
	strcpy(buf, h); *n = (DWORD)strlen(h); return TRUE;
}
BOOL GetUserNameA(LPSTR buf, LPDWORD n) { if (!buf || !n || *n < 8) return FALSE; strcpy(buf, "android"); *n = 8; return TRUE; }
BOOL GetVersionExA(LPOSVERSIONINFOA v)
{
	if (!v) return FALSE; DWORD sz = v->dwOSVersionInfoSize; memset(v, 0, sz ? sz : sizeof(OSVERSIONINFOA));
	v->dwOSVersionInfoSize = sz; v->dwMajorVersion = 10; v->dwMinorVersion = 0; v->dwBuildNumber = 19041; v->dwPlatformId = VER_PLATFORM_WIN32_NT; return TRUE;
}
void GetSystemInfo(LPSYSTEM_INFO si)
{
	if (!si) return; memset(si, 0, sizeof(*si)); si->dwPageSize = (DWORD)sysconf(_SC_PAGESIZE);
	long n = sysconf(_SC_NPROCESSORS_ONLN); si->dwNumberOfProcessors = (DWORD)(n > 0 ? n : 1); si->dwAllocationGranularity = 65536;
	si->dwActiveProcessorMask = (DWORD_PTR)((1ull << si->dwNumberOfProcessors) - 1);
}
void GlobalMemoryStatus(LPMEMORYSTATUS m)
{
	if (!m) return; struct sysinfo si; memset(&si, 0, sizeof(si)); sysinfo(&si);
	memset(m, 0, sizeof(*m)); m->dwLength = sizeof(*m);
	uint64_t total = (uint64_t)si.totalram * si.mem_unit, avail = (uint64_t)si.freeram * si.mem_unit;
	m->dwMemoryLoad = total ? (DWORD)(100 - avail * 100 / total) : 0;
	m->dwTotalPhys = (SIZE_T)total; m->dwAvailPhys = (SIZE_T)avail; m->dwTotalVirtual = (SIZE_T)total; m->dwAvailVirtual = (SIZE_T)avail;
}
BOOL GlobalMemoryStatusEx(LPMEMORYSTATUSEX m)
{
	if (!m) return FALSE; struct sysinfo si; memset(&si, 0, sizeof(si)); sysinfo(&si);
	uint64_t total = (uint64_t)si.totalram * si.mem_unit, avail = (uint64_t)si.freeram * si.mem_unit;
	m->dwMemoryLoad = total ? (DWORD)(100 - avail * 100 / total) : 0;
	m->ullTotalPhys = total; m->ullAvailPhys = avail; m->ullTotalPageFile = total; m->ullAvailPageFile = avail; m->ullTotalVirtual = total; m->ullAvailVirtual = avail; return TRUE;
}
LSTATUS RegOpenKeyExA(HKEY k, LPCSTR s, DWORD o, DWORD sam, HKEY* r) { (void)k; (void)s; (void)o; (void)sam; if (r) *r = NULL; return ERROR_FILE_NOT_FOUND; }
LSTATUS RegCreateKeyExA(HKEY k, LPCSTR s, DWORD r0, LPSTR c, DWORD o, DWORD sam, LPSECURITY_ATTRIBUTES sa, HKEY* r, LPDWORD d)
{ (void)k; (void)s; (void)r0; (void)c; (void)o; (void)sam; (void)sa; (void)d; if (r) *r = NULL; return ERROR_ACCESS_DENIED; }
LSTATUS RegQueryValueExA(HKEY k, LPCSTR n, LPDWORD r, LPDWORD t, LPBYTE d, LPDWORD c) { (void)k; (void)n; (void)r; (void)t; (void)d; (void)c; return ERROR_FILE_NOT_FOUND; }
LSTATUS RegSetValueExA(HKEY k, LPCSTR n, DWORD r, DWORD t, const BYTE* d, DWORD c) { (void)k; (void)n; (void)r; (void)t; (void)d; (void)c; return ERROR_ACCESS_DENIED; }
LSTATUS RegCloseKey(HKEY k) { (void)k; return ERROR_SUCCESS; }

/*---------------------------------------------------------------- user32 (cua so = SDL_Window cua KSdlApp) */
static SDL_Window* jx_win(HWND h) { SDL_Window* w = (SDL_Window*)s_pMainWindow; (void)h; return w; }
SHORT GetKeyState(int vk) { return g_pfnJxGetKeyState ? g_pfnJxGetKeyState(vk) : 0; }
SHORT GetAsyncKeyState(int vk) { return g_pfnJxGetKeyState ? g_pfnJxGetKeyState(vk) : 0; }
BOOL GetKeyboardState(PBYTE p) { if (p) { for (int i = 0; i < 256; i++) p[i] = (BYTE)((GetKeyState(i) & 0x8000) ? 0x80 : 0); } return TRUE; }
UINT MapVirtualKeyA(UINT c, UINT t) { (void)t; return c; }
void PostQuitMessage(int n)
{
	(void)n; SDL_Event ev; SDL_zero(ev); ev.type = SDL_EVENT_QUIT; ev.quit.timestamp = SDL_GetTicksNS(); SDL_PushEvent(&ev);
}
BOOL PostMessageA(HWND h, UINT m, WPARAM w, LPARAM l)
{
	if (m == WM_QUIT) { PostQuitMessage((int)w); return TRUE; }
	if (g_pfnJxWinMsg) { g_pfnJxWinMsg(h, m, w, l); return TRUE; }
	return FALSE;
}
LRESULT SendMessageA(HWND h, UINT m, WPARAM w, LPARAM l) { return g_pfnJxWinMsg ? g_pfnJxWinMsg(h, m, w, l) : 0; }
BOOL PeekMessageA(LPMSG m, HWND h, UINT a, UINT b, UINT r) { (void)m; (void)h; (void)a; (void)b; (void)r; return FALSE; }
BOOL GetMessageA(LPMSG m, HWND h, UINT a, UINT b) { (void)m; (void)h; (void)a; (void)b; return FALSE; }
BOOL TranslateMessage(const MSG* m) { (void)m; return FALSE; }
LRESULT DispatchMessageA(const MSG* m) { (void)m; return 0; }
LRESULT DefWindowProcA(HWND h, UINT m, WPARAM w, LPARAM l) { (void)h; (void)m; (void)w; (void)l; return 0; }
ATOM RegisterClassA(const WNDCLASSA* c) { (void)c; return 1; }
ATOM RegisterClassExA(const WNDCLASSEXA* c) { (void)c; return 1; }
HWND CreateWindowExA(DWORD ex, LPCSTR cls, LPCSTR name, DWORD st, int x, int y, int w, int h, HWND p, HMENU m, HINSTANCE i, LPVOID prm)
{ (void)ex; (void)cls; (void)name; (void)st; (void)x; (void)y; (void)w; (void)h; (void)p; (void)m; (void)i; (void)prm; return (HWND)s_pMainWindow; }
BOOL DestroyWindow(HWND h) { (void)h; return TRUE; }
HWND FindWindowA(LPCSTR c, LPCSTR n) { (void)c; (void)n; return NULL; }
HWND FindWindowExA(HWND p, HWND a, LPCSTR c, LPCSTR n) { (void)p; (void)a; (void)c; (void)n; return NULL; }
BOOL SetWindowTextA(HWND h, LPCSTR s) { SDL_Window* w = jx_win(h); if (w && s) SDL_SetWindowTitle(w, s); return TRUE; }
int GetWindowTextA(HWND h, LPSTR s, int n) { SDL_Window* w = jx_win(h); if (!s || n <= 0) return 0; const char* t = w ? SDL_GetWindowTitle(w) : ""; strncpy(s, t, n - 1); s[n - 1] = 0; return (int)strlen(s); }
int GetClassNameA(HWND h, LPSTR s, int n) { (void)h; if (!s || n <= 0) return 0; strncpy(s, "JXWC Class", n - 1); s[n - 1] = 0; return (int)strlen(s); }
BOOL GetClientRect(HWND h, LPRECT r)
{
	if (!r) return FALSE; int w = 0, hh = 0; SDL_Window* win = jx_win(h); if (win) SDL_GetWindowSizeInPixels(win, &w, &hh);
	r->left = 0; r->top = 0; r->right = w; r->bottom = hh; return TRUE;
}
BOOL GetWindowRect(HWND h, LPRECT r) { return GetClientRect(h, r); }
BOOL ClientToScreen(HWND h, LPPOINT p) { (void)h; (void)p; return TRUE; }
BOOL ScreenToClient(HWND h, LPPOINT p) { (void)h; (void)p; return TRUE; }
BOOL AdjustWindowRect(LPRECT r, DWORD s, BOOL m) { (void)r; (void)s; (void)m; return TRUE; }
BOOL AdjustWindowRectEx(LPRECT r, DWORD s, BOOL m, DWORD e) { (void)r; (void)s; (void)m; (void)e; return TRUE; }
BOOL MoveWindow(HWND h, int x, int y, int w, int hh, BOOL b) { (void)h; (void)x; (void)y; (void)w; (void)hh; (void)b; return TRUE; }
BOOL SetWindowPos(HWND h, HWND a, int x, int y, int cx, int cy, UINT f) { (void)h; (void)a; (void)x; (void)y; (void)cx; (void)cy; (void)f; return TRUE; }
BOOL ShowWindow(HWND h, int n) { (void)h; (void)n; return TRUE; }
BOOL UpdateWindow(HWND h) { (void)h; return TRUE; }
BOOL InvalidateRect(HWND h, const RECT* r, BOOL b) { (void)h; (void)r; (void)b; return TRUE; }
BOOL IsWindow(HWND h) { return h != NULL; }
BOOL IsIconic(HWND h) { SDL_Window* w = jx_win(h); return w ? ((SDL_GetWindowFlags(w) & SDL_WINDOW_MINIMIZED) != 0) : FALSE; }
BOOL IsWindowVisible(HWND h) { (void)h; return TRUE; }
HWND GetForegroundWindow(void) { return (HWND)s_pMainWindow; }
BOOL SetForegroundWindow(HWND h) { (void)h; return TRUE; }
HWND GetActiveWindow(void) { return (HWND)s_pMainWindow; }
HWND SetActiveWindow(HWND h) { (void)h; return (HWND)s_pMainWindow; }
HWND GetFocus(void) { return (HWND)s_pMainWindow; }
HWND SetFocus(HWND h) { (void)h; return (HWND)s_pMainWindow; }
HWND GetDesktopWindow(void) { return NULL; }
HWND GetParent(HWND h) { (void)h; return NULL; }
HWND SetCapture(HWND h) { SDL_CaptureMouse(true); (void)h; return NULL; }
BOOL ReleaseCapture(void) { SDL_CaptureMouse(false); return TRUE; }
HWND GetCapture(void) { return NULL; }
LONG GetWindowLongA(HWND h, int n) { (void)h; (void)n; return 0; }
LONG SetWindowLongA(HWND h, int n, LONG v) { (void)h; (void)n; (void)v; return 0; }
LONG_PTR GetWindowLongPtrA(HWND h, int n) { (void)h; (void)n; return 0; }
LONG_PTR SetWindowLongPtrA(HWND h, int n, LONG_PTR v) { (void)h; (void)n; (void)v; return 0; }
HDC GetDC(HWND h) { (void)h; return NULL; }
int ReleaseDC(HWND h, HDC d) { (void)h; (void)d; return 1; }
int GetSystemMetrics(int n)
{
	SDL_Rect r; SDL_DisplayID id = SDL_GetPrimaryDisplay();
	if (!id || !SDL_GetDisplayBounds(id, &r)) { r.w = 1024; r.h = 768; }
	switch (n) { case SM_CXSCREEN: case SM_CXFULLSCREEN: case SM_CXVIRTUALSCREEN: return r.w; case SM_CYSCREEN: case SM_CYFULLSCREEN: case SM_CYVIRTUALSCREEN: return r.h; default: return 0; }
}
UINT GetDoubleClickTime(void) { return 500; }
BOOL GetCursorPos(LPPOINT p) { if (!p) return FALSE; float x = 0, y = 0; SDL_GetMouseState(&x, &y); p->x = (LONG)x; p->y = (LONG)y; return TRUE; }
BOOL SetCursorPos(int x, int y) { SDL_Window* w = jx_win(NULL); if (w) SDL_WarpMouseInWindow(w, (float)x, (float)y); return TRUE; }
static int s_nCursorShow = 0;
int ShowCursor(BOOL b) { s_nCursorShow += b ? 1 : -1; if (s_nCursorShow >= 0) SDL_ShowCursor(); else SDL_HideCursor(); return s_nCursorShow; }
static SDL_Cursor* s_pArrow = NULL;
HCURSOR SetCursor(HCURSOR c) { if (c) SDL_SetCursor((SDL_Cursor*)c); return c; }
HCURSOR GetCursor(void) { return (HCURSOR)SDL_GetCursor(); }
HCURSOR LoadCursorA(HINSTANCE i, LPCSTR n) { (void)i; (void)n; if (!s_pArrow) s_pArrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT); return (HCURSOR)s_pArrow; }
HCURSOR LoadCursorFromFileA(LPCSTR f) { (void)f; return LoadCursorA(NULL, NULL); }
BOOL DestroyCursor(HCURSOR c) { if (c && (SDL_Cursor*)c != s_pArrow) SDL_DestroyCursor((SDL_Cursor*)c); return TRUE; }
HICON LoadIconA(HINSTANCE i, LPCSTR n) { (void)i; (void)n; return NULL; }
BOOL ClipCursor(const RECT* r) { (void)r; return TRUE; }
int MessageBoxA(HWND h, LPCSTR text, LPCSTR cap, UINT type)
{
	(void)h; jx_log("[MessageBox] %s: %s", cap ? cap : "", text ? text : "");
	SDL_Window* w = jx_win(NULL);
	if ((type & 0xF) == MB_YESNO || (type & 0xF) == MB_YESNOCANCEL)
	{
		SDL_MessageBoxButtonData btn[2] = { { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, IDYES, "Yes" }, { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, IDNO, "No" } };
		SDL_MessageBoxData d; SDL_zero(d); d.flags = SDL_MESSAGEBOX_INFORMATION; d.window = w; d.title = cap ? cap : ""; d.message = text ? text : ""; d.numbuttons = 2; d.buttons = btn;
		int id = IDNO; SDL_ShowMessageBox(&d, &id); return id;
	}
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, cap ? cap : "", text ? text : "", w);
	return IDOK;
}
BOOL MessageBeep(UINT t) { (void)t; return FALSE; }
BOOL AllocConsole(void) { return TRUE; }
BOOL FreeConsole(void) { return TRUE; }
UINT_PTR SetTimer(HWND h, UINT_PTR id, UINT e, LPVOID f) { (void)h; (void)id; (void)e; (void)f; return 0; }
BOOL KillTimer(HWND h, UINT_PTR id) { (void)h; (void)id; return TRUE; }
BOOL CreateCaret(HWND h, HANDLE b, int w, int hh) { (void)h; (void)b; (void)w; (void)hh; return TRUE; }
BOOL DestroyCaret(void) { return TRUE; }
BOOL SetCaretPos(int x, int y) { (void)x; (void)y; return TRUE; }
BOOL ShowCaret(HWND h) { (void)h; return TRUE; }
BOOL HideCaret(HWND h) { (void)h; return TRUE; }
static char* s_pClipText = NULL;
BOOL OpenClipboard(HWND h) { (void)h; return TRUE; }
BOOL CloseClipboard(void) { return TRUE; }
BOOL EmptyClipboard(void) { return TRUE; }
HANDLE GetClipboardData(UINT f)
{
	if (f != CF_TEXT && f != CF_OEMTEXT) return NULL;
	if (s_pClipText) { SDL_free(s_pClipText); s_pClipText = NULL; }
	s_pClipText = SDL_GetClipboardText(); return (HANDLE)s_pClipText;
}
HANDLE SetClipboardData(UINT f, HANDLE m) { if ((f == CF_TEXT || f == CF_OEMTEXT) && m) SDL_SetClipboardText((const char*)m); return m; }
BOOL IsClipboardFormatAvailable(UINT f) { return (f == CF_TEXT || f == CF_OEMTEXT) ? SDL_HasClipboardText() : FALSE; }
HGLOBAL GlobalAlloc(UINT f, SIZE_T n) { void* p = (f & GMEM_ZEROINIT) ? calloc(1, n ? n : 1) : malloc(n ? n : 1); return (HGLOBAL)p; }
LPVOID GlobalLock(HGLOBAL h) { return (LPVOID)h; }
BOOL GlobalUnlock(HGLOBAL h) { (void)h; return TRUE; }
HGLOBAL GlobalFree(HGLOBAL h) { free((void*)h); return NULL; }
SIZE_T GlobalSize(HGLOBAL h) { (void)h; return 0; }
HINSTANCE ShellExecuteA(HWND h, LPCSTR op, LPCSTR file, LPCSTR prm, LPCSTR dir, int n)
{
	(void)h; (void)op; (void)prm; (void)dir; (void)n;
	if (file && (strncmp(file, "http", 4) == 0 || strncmp(file, "mailto:", 7) == 0)) { SDL_OpenURL(file); return (HINSTANCE)(uintptr_t)42; }
	jx_log("ShellExecute(%s) bo qua", file ? file : "");
	return (HINSTANCE)(uintptr_t)2;
}
BOOL ShellExecuteExA(SHELLEXECUTEINFOA* p) { if (!p) return FALSE; ShellExecuteA(p->hwnd, p->lpVerb, p->lpFile, p->lpParameters, p->lpDirectory, p->nShow); return TRUE; }

/*---------------------------------------------------------------- ma hoa chuoi (WCHAR = wchar_t 32 bit tren Android) */
static const char* jx_cp_name(UINT cp)
{
	switch (cp) { case 65001: return "UTF-8"; case 936: return "GBK"; case 950: return "BIG5"; case 1252: return "CP1252"; case 1200: return "UTF-16LE"; default: return "CP1258"; }
}
int MultiByteToWideChar(UINT cp, DWORD fl, LPCSTR src, int n, LPWSTR dst, int nd)
{
	(void)fl; if (!src) return 0;
	size_t len = (n < 0) ? strlen(src) + 1 : (size_t)n;
	size_t outLen = 0;
	char* out = SDL_iconv_string("UTF-32LE", jx_cp_name(cp), src, len);
	wchar_t* w = (wchar_t*)out;
	if (out) { const uint32_t* u = (const uint32_t*)out; while (outLen * 4 < len * 4 && u[outLen] != 0) outLen++; if (n < 0) outLen++; }
	else { /* khong doi duoc: mo rong tung byte */ outLen = len; }
	if (nd == 0) { SDL_free(out); return (int)outLen; }
	if ((int)outLen > nd) { SDL_free(out); return 0; }
	for (size_t i = 0; i < outLen; i++) dst[i] = out ? w[i] : (wchar_t)(unsigned char)src[i];
	SDL_free(out); return (int)outLen;
}
int WideCharToMultiByte(UINT cp, DWORD fl, LPCWSTR src, int n, LPSTR dst, int nd, LPCSTR def, LPBOOL used)
{
	(void)fl; (void)def; if (used) *used = FALSE; if (!src) return 0;
	size_t len = 0; if (n < 0) { while (src[len]) len++; len++; } else len = (size_t)n;
	char* out = SDL_iconv_string(jx_cp_name(cp), "UTF-32LE", (const char*)src, len * sizeof(wchar_t));
	size_t outLen = 0;
	if (out) { outLen = (n < 0) ? strlen(out) + 1 : strlen(out); }
	else outLen = len;
	if (nd == 0) { SDL_free(out); return (int)outLen; }
	if ((int)outLen > nd) { SDL_free(out); return 0; }
	for (size_t i = 0; i < outLen; i++) dst[i] = out ? out[i] : (char)src[i];
	SDL_free(out); return (int)outLen;
}
/*---------------------------------------------------------------- HW profile / thu muc dac biet / _beginthreadex */
BOOL GetCurrentHwProfileA(LPHW_PROFILE_INFOA p)
{
	if (!p) return FALSE;
	memset(p, 0, sizeof(*p)); p->dwDockInfo = 0;
	/* GUID gia on dinh theo may: bam ten may + thu muc du lieu */
	char h[256] = "android"; gethostname(h, sizeof(h)); h[sizeof(h) - 1] = 0;
	unsigned long a = 5381; for (const char* c = h; *c; c++) a = a * 33 + (unsigned char)*c;
	for (const char* c = s_szDataDir; *c; c++) a = a * 33 + (unsigned char)*c;
	snprintf(p->szHwProfileGuid, sizeof(p->szHwProfileGuid), "{%08lX-4E44-4A58-B1B1-%012lX}", a & 0xFFFFFFFFul, (a * 2654435761ul) & 0xFFFFFFFFFFFFul);
	strncpy(p->szHwProfileName, "Android", sizeof(p->szHwProfileName) - 1);
	return TRUE;
}
BOOL SHGetSpecialFolderPathA(HWND h, LPSTR path, int csidl, BOOL bCreate)
{
	(void)h; (void)csidl; (void)bCreate; if (!path) return FALSE;
	snprintf(path, MAX_PATH, "%s", s_szDataDir[0] ? s_szDataDir : ".");
	return TRUE;
}
HRESULT SHGetFolderPathA(HWND h, int csidl, HANDLE tok, DWORD f, LPSTR path) { (void)tok; (void)f; return SHGetSpecialFolderPathA(h, path, csidl, FALSE) ? 0 : (HRESULT)0x80070002; }
uintptr_t _beginthreadex(void* sec, unsigned stack, unsigned (*fn)(void*), void* arg, unsigned flags, unsigned* pid)
{
	DWORD id = 0;
	HANDLE hh = CreateThread((LPSECURITY_ATTRIBUTES)sec, stack, (LPTHREAD_START_ROUTINE)fn, arg, flags, &id);
	if (pid) *pid = (unsigned)id;
	return (uintptr_t)hh;
}
void _endthreadex(unsigned r) { (void)r; }

UINT RegisterWindowMessageA(LPCSTR s)
{
	/* ma thong diep on dinh theo ten trong khoang 0xC000-0xFFFF nhu Windows */
	unsigned h = 5381; if (s) for (const char* c = s; *c; c++) h = h * 33 + (unsigned char)*c;
	return 0xC000 + (h % 0x3FFF);
}
BOOL GetProcessMemoryInfo(HANDLE hProcess, PPROCESS_MEMORY_COUNTERS p, DWORD cb)
{
	(void)hProcess; if (!p || cb < sizeof(PROCESS_MEMORY_COUNTERS)) return FALSE;
	memset(p, 0, cb); p->cb = cb;
	FILE* f = fopen("/proc/self/statm", "rb");
	if (f)
	{
		unsigned long sz = 0, res = 0; if (fscanf(f, "%lu %lu", &sz, &res) == 2)
		{
			long pg = sysconf(_SC_PAGESIZE);
			p->WorkingSetSize = (SIZE_T)res * pg; p->PeakWorkingSetSize = p->WorkingSetSize; p->PagefileUsage = (SIZE_T)sz * pg;
			if (cb >= sizeof(PROCESS_MEMORY_COUNTERS_EX)) ((PPROCESS_MEMORY_COUNTERS_EX)p)->PrivateUsage = (SIZE_T)res * pg;
		}
		fclose(f);
	}
	return TRUE;
}

BOOL EnumDisplaySettingsA(LPCSTR dev, DWORD mode, LPDEVMODEA dm)
{
	(void)dev; if (!dm) return FALSE;
	if (mode != ENUM_CURRENT_SETTINGS && mode != 0) return FALSE;
	memset(dm, 0, sizeof(*dm)); dm->dmSize = sizeof(*dm);
	SDL_DisplayID id = SDL_GetPrimaryDisplay(); const SDL_DisplayMode* m = id ? SDL_GetCurrentDisplayMode(id) : NULL;
	dm->dmPelsWidth = m ? (DWORD)m->w : 1024; dm->dmPelsHeight = m ? (DWORD)m->h : 768;
	dm->dmDisplayFrequency = (m && m->refresh_rate > 0) ? (DWORD)(m->refresh_rate + 0.5f) : 60; dm->dmBitsPerPel = 32;
	dm->dmFields = 0x00080000 | 0x00100000 | 0x00040000 | 0x00400000;   /* DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY */
	return TRUE;
}
DWORD MsgWaitForMultipleObjects(DWORD n, const HANDLE* p, BOOL bAll, DWORD ms, DWORD mask)
{
	(void)mask; if (n == 0) { SDL_Delay(ms == INFINITE ? 1 : ms); return WAIT_TIMEOUT; }
	return WaitForMultipleObjects(n, p, bAll, ms);
}

/*---------------------------------------------------------------- bo nho chia se co ten (WAuto) -> bo nho thuong trong tien trinh */
HANDLE CreateFileMappingA(HANDLE h, LPSECURITY_ATTRIBUTES sa, DWORD prot, DWORD hi, DWORD lo, LPCSTR name)
{
	(void)h; (void)sa; (void)prot; (void)name;
	size_t n = ((size_t)hi << 32) | lo; if (n == 0) return NULL;
	JxMapping* m = (JxMapping*)calloc(1, sizeof(JxMapping)); if (!m) return NULL;
	m->magic = JX_MAGIC_MAP; m->size = n; m->p = calloc(1, n);
	if (!m->p) { free(m); return NULL; }
	return (HANDLE)m;
}
HANDLE OpenFileMappingA(DWORD access, BOOL inherit, LPCSTR name) { (void)access; (void)inherit; (void)name; SetLastError(ERROR_FILE_NOT_FOUND); return NULL; }
LPVOID MapViewOfFile(HANDLE h, DWORD access, DWORD hi, DWORD lo, SIZE_T n)
{
	(void)access; (void)n; JxMapping* m = (JxMapping*)h;
	if (!m || h == INVALID_HANDLE_VALUE || m->magic != JX_MAGIC_MAP) return NULL;
	size_t off = ((size_t)hi << 32) | lo; if (off >= m->size) return NULL;
	return (BYTE*)m->p + off;
}
BOOL UnmapViewOfFile(LPCVOID p) { (void)p; return TRUE; }

UINT GetACP(void) { return 1258; }
BOOL IsDBCSLeadByte(BYTE c) { return c >= 0x81; }
int WSAStartup(WORD v, LPWSADATA d) { if (d) { memset(d, 0, sizeof(*d)); d->wVersion = v; d->wHighVersion = v; } return 0; }
int WSACleanup(void) { return 0; }
#endif /* !_WIN32 */
