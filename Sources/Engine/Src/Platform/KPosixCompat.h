/*===========================================================================
  KPosixCompat.h  -  [ANDROID 08/09] Lop tuong thich Win32 -> POSIX
                     (Android NDK / Linux / macOS).  BAN NHAP (khao sat).

  MUC DICH
    Cho phep bien dich ma client (Engine, Core, S3Client, MultiServer/Common,
    Rainbow) bang clang++ cua NDK ma khong co <windows.h>.  Tep nay chi cung
    cap NHUNG THU RE VA TRUNG THUC:
      - typedef cac kieu Win32 (BYTE, WORD, DWORD, LONG, HANDLE, RECT ...),
      - macro quy uoc goi ham / __declspec / TCHAR / hang so,
      - anh xa CRT cua MSVC sang POSIX (stricmp, _snprintf, itoa, strupr ...),
      - vai ham inline 1 dong (Interlocked*, lstr*, ZeroMemory).
    KHONG cai dat ham Win32 that (GetTickCount, CreateFile, CreateThread,
    MessageBox, WSAStartup, LoadLibrary ...).  Chung PHAI la loi bien dich de
    duoc dem va port that (SDL3 / pthread / dlopen ...).

  QUY TAC KICH THUOC (quan trong cho goi mang va struct tren dia)
    Win32 la LLP64: long = 32 bit.  Android/Linux 64 bit la LP64: long = 64
    bit.  Vi vay LONG/ULONG/DWORD/HRESULT o day la int32/uint32, KHONG dung
    'long', de sizeof cac struct giong het ban Windows.  He qua: LONG va INT
    la CUNG kieu (int) -> ham qua tai f(LONG)/f(INT) se bi loi trung; chap
    nhan (giong Wine).

  CACH DUNG
    - Khao sat: force-include bang  -include Sources/Engine/Src/Platform/KPosixCompat.h
    - Port that: KWin32.h nhanh khong-Windows chi con  #include "Platform/KPosixCompat.h"
    Bien dich duoc ca C (thu muc ucl, CRC32.C) lan C++.  Chi co hieu luc khi !_WIN32.
    ASCII thuan.  Chua bao gio dua vao ban Windows (bi #if !defined(_WIN32) bo qua).
===========================================================================*/
#ifndef KPOSIXCOMPAT_H
#define KPOSIXCOMPAT_H

#if !defined(_WIN32)

/*--------------------------------------------------------------------------
  0. Header POSIX / C chuan ma ma Win32 thuong mac dinh co san qua windows.h
--------------------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>     /* strcasecmp / strncasecmp */
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <alloca.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

/*--------------------------------------------------------------------------
  1. Tu khoa MSVC / quy uoc goi ham / __declspec
     (clang -fms-extensions da hieu __int64, __forceinline, __super, __asm...;
      macro o day de ma con bien dich duoc voi gcc va khi khong bat ms-extensions)
--------------------------------------------------------------------------*/
#ifndef __declspec
#define __declspec(x)
#endif
#ifndef __stdcall
#define __stdcall
#endif
#ifndef __cdecl
#define __cdecl
#endif
#ifndef __fastcall
#define __fastcall
#endif
#ifndef __thiscall
#define __thiscall
#endif
#ifndef __vectorcall
#define __vectorcall
#endif
#ifndef __pascal
#define __pascal
#endif
#ifndef __forceinline
#define __forceinline   inline __attribute__((always_inline))
#endif
#ifndef __w64
#define __w64
#endif
#ifndef __unaligned
#define __unaligned
#endif
#ifndef __ptr32
#define __ptr32
#endif
#ifndef __ptr64
#define __ptr64
#endif
#ifndef __noop
#define __noop(...)     ((void)0)
#endif
#ifndef __int8
#define __int8          char
#endif
#ifndef __int16
#define __int16         short
#endif
#ifndef __int32
#define __int32         int
#endif
#ifndef __int64
#define __int64         long long
#endif
#ifndef __FUNCSIG__
#define __FUNCSIG__     __PRETTY_FUNCTION__
#endif

#define WINAPI
#define WINAPIV
#define APIENTRY
#define APIPRIVATE
#define CALLBACK
#define PASCAL
#define CDECL
#define FAR
#define NEAR
#define CONST           const
#define VOID            void
#define IN
#define OUT
#define OPTIONAL
#define WINBASEAPI
#define WINUSERAPI
#define WINGDIAPI
#define WINMMAPI
#define DECLSPEC_IMPORT
#define DECLSPEC_EXPORT
#define DECLSPEC_NORETURN   __attribute__((noreturn))
#define DECLSPEC_NOINLINE   __attribute__((noinline))
#define DECLSPEC_ALIGN(x)   __attribute__((aligned(x)))
#define DECLSPEC_SELECTANY  __attribute__((weak))
#define DECLSPEC_DEPRECATED
#define FORCEINLINE         inline __attribute__((always_inline))
#ifdef __cplusplus
#define EXTERN_C            extern "C"
#else
#define EXTERN_C            extern
#endif
#define UNREFERENCED_PARAMETER(x)       ((void)(x))
#define DBG_UNREFERENCED_PARAMETER(x)   ((void)(x))
#ifndef _countof
#define _countof(a)     (sizeof(a) / sizeof((a)[0]))
#endif
#ifndef ARRAYSIZE
#define ARRAYSIZE(a)    (sizeof(a) / sizeof((a)[0]))
#endif

/*--------------------------------------------------------------------------
  2. Kieu so co ban (giu dung kich thuoc Win32)
--------------------------------------------------------------------------*/
typedef char                CHAR;
typedef unsigned char       UCHAR;
typedef unsigned char       BYTE;
typedef unsigned char       BOOLEAN;
typedef short               SHORT;
typedef unsigned short      USHORT;
typedef unsigned short      WORD;
typedef int                 INT;
typedef unsigned int        UINT;
typedef int                 BOOL;
typedef int32_t             LONG;       /* 32 bit ke ca tren LP64 */
typedef uint32_t            ULONG;
typedef uint32_t            DWORD;
typedef int64_t             LONGLONG;
typedef uint64_t            ULONGLONG;
typedef uint64_t            DWORDLONG;
typedef uint64_t            QWORD;
typedef float               FLOAT;
typedef double              DOUBLE;
typedef int8_t              INT8;
typedef uint8_t             UINT8;
typedef int16_t             INT16;
typedef uint16_t            UINT16;
typedef int32_t             INT32;
typedef uint32_t            UINT32;
typedef int64_t             INT64;
typedef uint64_t            UINT64;
typedef int32_t             LONG32;
typedef uint32_t            ULONG32;
typedef uint32_t            DWORD32;
typedef int64_t             LONG64;
typedef uint64_t            ULONG64;
typedef uint64_t            DWORD64;
typedef int32_t             HRESULT;
typedef DWORD               COLORREF;
typedef DWORD               LCID;
typedef WORD                LANGID;
typedef WORD                ATOM;
/* WCHAR: Win32 la 16 bit; o day dung wchar_t (32 bit tren Android) de L"..."
   con bien dich duoc.  CANH BAO: du lieu UTF-16 tren dia/mang KHONG dung WCHAR nay. */
typedef wchar_t             WCHAR;

/* kieu rong bang con tro */
typedef intptr_t            INT_PTR;
typedef uintptr_t           UINT_PTR;
typedef intptr_t            LONG_PTR;
typedef uintptr_t           ULONG_PTR;
typedef uintptr_t           DWORD_PTR;
typedef size_t              SIZE_T;
typedef ptrdiff_t           SSIZE_T;
typedef UINT_PTR            WPARAM;
typedef LONG_PTR            LPARAM;
typedef LONG_PTR            LRESULT;

/* con tro toi cac kieu tren */
typedef void               *PVOID, *LPVOID;
typedef const void         *PCVOID, *LPCVOID;
typedef BYTE               *PBYTE, *LPBYTE;
typedef WORD               *PWORD, *LPWORD;
typedef DWORD              *PDWORD, *LPDWORD;
typedef LONG               *PLONG, *LPLONG;
typedef ULONG              *PULONG;
typedef INT                *PINT, *LPINT;
typedef UINT               *PUINT;
typedef BOOL               *PBOOL, *LPBOOL;
typedef SHORT              *PSHORT;
typedef USHORT             *PUSHORT;
typedef FLOAT              *PFLOAT;
typedef LONGLONG           *PLONGLONG;
typedef ULONGLONG          *PULONGLONG;
typedef CHAR               *PCHAR, *PSTR, *LPSTR, *NPSTR;
typedef const CHAR         *PCSTR, *LPCSTR;
typedef WCHAR              *PWCHAR, *PWSTR, *LPWSTR;
typedef const WCHAR        *PCWSTR, *LPCWSTR;
typedef ULONG_PTR          *PULONG_PTR;
typedef DWORD_PTR          *PDWORD_PTR;

/*--------------------------------------------------------------------------
  3. HANDLE va cac handle "manh kieu" (STRICT nhu MSVC mac dinh)
--------------------------------------------------------------------------*/
typedef void               *HANDLE;
typedef HANDLE             *PHANDLE, *LPHANDLE;
#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
DECLARE_HANDLE(HWND);
DECLARE_HANDLE(HINSTANCE);
DECLARE_HANDLE(HDC);
DECLARE_HANDLE(HBITMAP);
DECLARE_HANDLE(HFONT);
DECLARE_HANDLE(HICON);
DECLARE_HANDLE(HMENU);
DECLARE_HANDLE(HKEY);
DECLARE_HANDLE(HPALETTE);
DECLARE_HANDLE(HRGN);
DECLARE_HANDLE(HBRUSH);
DECLARE_HANDLE(HPEN);
DECLARE_HANDLE(HRSRC);
DECLARE_HANDLE(HIMC);
DECLARE_HANDLE(HACCEL);
DECLARE_HANDLE(HMONITOR);
typedef HINSTANCE           HMODULE;
typedef HICON               HCURSOR;
typedef HANDLE              HGLOBAL;
typedef HANDLE              HLOCAL;
typedef HANDLE              HGDIOBJ;
typedef HANDLE              HFILE_HANDLE;
typedef HKEY               *PHKEY;
typedef HANDLE              HHOOK;
typedef HANDLE              HWINEVENTHOOK;
typedef int                 HFILE;
typedef INT_PTR (*FARPROC)(void);
typedef INT_PTR (*NEARPROC)(void);
typedef INT_PTR (*PROC)(void);
typedef DWORD (*PTHREAD_START_ROUTINE)(LPVOID lpThreadParameter);
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

/*--------------------------------------------------------------------------
  4. TCHAR (ban MultiByte: TCHAR = char)
--------------------------------------------------------------------------*/
typedef char                TCHAR, _TCHAR;
typedef TCHAR              *PTCHAR, *PTSTR, *LPTSTR;
typedef const TCHAR        *PCTSTR, *LPCTSTR;
#define _T(x)               x
#define _TEXT(x)            x
#define __T(x)              x
#define TEXT(x)             x
#define __TEXT(x)           x
#define _tcslen             strlen
#define _tcsclen            strlen
#define _tcscpy             strcpy
#define _tcsncpy            strncpy
#define _tcscat             strcat
#define _tcsncat            strncat
#define _tcscmp             strcmp
#define _tcsncmp            strncmp
#define _tcsnccmp           strncmp
#define _tcsicmp            strcasecmp
#define _tcsnicmp           strncasecmp
#define _tcschr             strchr
#define _tcsrchr            strrchr
#define _tcsstr             strstr
#define _tcstok             strtok
#define _tcsdup             strdup
#define _tcslwr             strlwr
#define _tcsupr             strupr
#define _tcstol             strtol
#define _tcstoul            strtoul
#define _tcstod             strtod
#define _tcsspn             strspn
#define _tcscspn            strcspn
#define _tcsnset            strnset_compat
#define _tcsrev             strrev
#define _tcspbrk            strpbrk
#define _tcsinc(p)          ((p) + 1)
#define _tcsnextc(p)        ((unsigned char)*(p))
typedef unsigned char       _TUCHAR;
#define _tstoi              atoi
#define _stprintf           sprintf
#define _sntprintf          snprintf
#define _vstprintf          vsprintf
#define _vsntprintf         vsnprintf
#define _stscanf            sscanf
#define _tprintf            printf
#define _ftprintf           fprintf
#define _tfopen             fopen
#define _ttoi               atoi
#define _ttol               atol
#define _totupper           toupper
#define _totlower           tolower
#define _istdigit           isdigit
#define _istalpha           isalpha
#define _istspace           isspace
#define _tmain              main

/*--------------------------------------------------------------------------
  5. Struct Win32 hay gap (bo cuc giong het Windows)
--------------------------------------------------------------------------*/
typedef struct tagRECT  { LONG left; LONG top; LONG right; LONG bottom; } RECT, *PRECT, *LPRECT;
typedef const RECT     *LPCRECT;
typedef struct tagPOINT { LONG x; LONG y; } POINT, *PPOINT, *LPPOINT;
typedef struct tagSIZE  { LONG cx; LONG cy; } SIZE, *PSIZE, *LPSIZE;
typedef struct tagPOINTS { SHORT x; SHORT y; } POINTS, *PPOINTS;

typedef struct _GUID { DWORD Data1; WORD Data2; WORD Data3; BYTE Data4[8]; } GUID;
typedef GUID            IID, CLSID, *LPGUID, *LPIID, *LPCLSID;
typedef const GUID     *LPCGUID;
#ifdef __cplusplus
#define REFGUID         const GUID &
#define REFIID          const IID &
#define REFCLSID        const CLSID &
#else
#define REFGUID         const GUID * const
#define REFIID          const IID * const
#define REFCLSID        const CLSID * const
#endif
/* DEFINE_GUID: xem cuoi tep (ngoai include guard, giong guiddef.h: initguid.h dat INITGUID roi include lai) */
#ifdef __cplusplus
static inline int IsEqualGUID(const GUID &a, const GUID &b) { return memcmp(&a, &b, sizeof(GUID)) == 0; }
static inline bool operator==(const GUID &a, const GUID &b) { return memcmp(&a, &b, sizeof(GUID)) == 0; }
static inline bool operator!=(const GUID &a, const GUID &b) { return memcmp(&a, &b, sizeof(GUID)) != 0; }
#else
static inline int IsEqualGUID(const GUID *a, const GUID *b) { return memcmp(a, b, sizeof(GUID)) == 0; }
#endif
#define IsEqualIID(a, b)    IsEqualGUID(a, b)
#define IsEqualCLSID(a, b)  IsEqualGUID(a, b)

typedef struct _FILETIME { DWORD dwLowDateTime; DWORD dwHighDateTime; } FILETIME, *PFILETIME, *LPFILETIME;
typedef struct _SYSTEMTIME {
    WORD wYear; WORD wMonth; WORD wDayOfWeek; WORD wDay;
    WORD wHour; WORD wMinute; WORD wSecond; WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

typedef union _LARGE_INTEGER {
    struct { DWORD LowPart; LONG HighPart; };
    struct { DWORD LowPart; LONG HighPart; } u;
    LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;
typedef union _ULARGE_INTEGER {
    struct { DWORD LowPart; DWORD HighPart; };
    struct { DWORD LowPart; DWORD HighPart; } u;
    ULONGLONG QuadPart;
} ULARGE_INTEGER, *PULARGE_INTEGER;

typedef struct _OVERLAPPED {
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    union {
        struct { DWORD Offset; DWORD OffsetHigh; };
        PVOID Pointer;
    };
    HANDLE hEvent;
} OVERLAPPED, *LPOVERLAPPED;

typedef struct _SECURITY_ATTRIBUTES {
    DWORD  nLength;
    LPVOID lpSecurityDescriptor;
    BOOL   bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

/* CRITICAL_SECTION = pthread mutex (chi KIEU; Enter/Leave/Initialize van la loi de port) */
typedef pthread_mutex_t     CRITICAL_SECTION, *PCRITICAL_SECTION, *LPCRITICAL_SECTION;

typedef struct _COPYDATASTRUCT { ULONG_PTR dwData; DWORD cbData; PVOID lpData; } COPYDATASTRUCT, *PCOPYDATASTRUCT;

typedef struct _MEMORYSTATUS {
    DWORD  dwLength; DWORD dwMemoryLoad;
    SIZE_T dwTotalPhys; SIZE_T dwAvailPhys; SIZE_T dwTotalPageFile; SIZE_T dwAvailPageFile;
    SIZE_T dwTotalVirtual; SIZE_T dwAvailVirtual;
} MEMORYSTATUS, *LPMEMORYSTATUS;

/* GDI/BMP: chi kieu du lieu tep .bmp / bang mau (KBmpFile, KBitmap, KPalette) */
typedef struct tagRGBQUAD   { BYTE rgbBlue; BYTE rgbGreen; BYTE rgbRed; BYTE rgbReserved; } RGBQUAD, *LPRGBQUAD;
typedef struct tagRGBTRIPLE { BYTE rgbtBlue; BYTE rgbtGreen; BYTE rgbtRed; } RGBTRIPLE;
typedef struct tagPALETTEENTRY { BYTE peRed; BYTE peGreen; BYTE peBlue; BYTE peFlags; } PALETTEENTRY, *LPPALETTEENTRY;
#pragma pack(push, 2)
typedef struct tagBITMAPFILEHEADER {
    WORD  bfType; DWORD bfSize; WORD bfReserved1; WORD bfReserved2; DWORD bfOffBits;
} BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;
#pragma pack(pop)
typedef struct tagBITMAPINFOHEADER {
    DWORD biSize; LONG biWidth; LONG biHeight; WORD biPlanes; WORD biBitCount;
    DWORD biCompression; DWORD biSizeImage; LONG biXPelsPerMeter; LONG biYPelsPerMeter;
    DWORD biClrUsed; DWORD biClrImportant;
} BITMAPINFOHEADER, *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;
typedef struct tagBITMAPINFO { BITMAPINFOHEADER bmiHeader; RGBQUAD bmiColors[1]; } BITMAPINFO, *LPBITMAPINFO, *PBITMAPINFO;
typedef struct tagBITMAPCOREHEADER { DWORD bcSize; WORD bcWidth; WORD bcHeight; WORD bcPlanes; WORD bcBitCount; } BITMAPCOREHEADER;
#define BI_RGB          0
#define BI_RLE8         1
#define BI_RLE4         2
#define BI_BITFIELDS    3

/* mmreg.h: dinh dang WAV (KWavFile / KWavCodec) - chi kieu */
#pragma pack(push, 1)
typedef struct tWAVEFORMATEX {
    WORD wFormatTag; WORD nChannels; DWORD nSamplesPerSec; DWORD nAvgBytesPerSec;
    WORD nBlockAlign; WORD wBitsPerSample; WORD cbSize;
} WAVEFORMATEX, *PWAVEFORMATEX, *LPWAVEFORMATEX;
typedef struct waveformat_tag {
    WORD wFormatTag; WORD nChannels; DWORD nSamplesPerSec; DWORD nAvgBytesPerSec; WORD nBlockAlign;
} WAVEFORMAT, *PWAVEFORMAT, *LPWAVEFORMAT;
typedef struct pcmwaveformat_tag { WAVEFORMAT wf; WORD wBitsPerSample; } PCMWAVEFORMAT, *PPCMWAVEFORMAT, *LPPCMWAVEFORMAT;
#pragma pack(pop)
#define WAVE_FORMAT_PCM 1

/*--------------------------------------------------------------------------
  6. Hang so
--------------------------------------------------------------------------*/
#ifndef TRUE
#define TRUE            1
#endif
#ifndef FALSE
#define FALSE           0
#endif
#ifndef NULL
#define NULL            0
#endif
#ifndef MAX_PATH
#define MAX_PATH        260
#endif
#define _MAX_PATH       260
#define _MAX_DRIVE      3
#define _MAX_DIR        256
#define _MAX_FNAME      256
#define _MAX_EXT        256
#define INFINITE        0xFFFFFFFF
#define INVALID_HANDLE_VALUE    ((HANDLE)(LONG_PTR)-1)
#define INVALID_FILE_SIZE       ((DWORD)0xFFFFFFFF)
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#define FILE_BEGIN      0
#define FILE_CURRENT    1
#define FILE_END        2
#define WAIT_OBJECT_0   0
#define WAIT_ABANDONED  0x80
#define WAIT_TIMEOUT    258
#define WAIT_FAILED     ((DWORD)0xFFFFFFFF)
#define ERROR_SUCCESS   0
#define NO_ERROR        0
#define S_OK            ((HRESULT)0)
#define S_FALSE         ((HRESULT)1)
#define E_FAIL          ((HRESULT)0x80004005)
#define E_OUTOFMEMORY   ((HRESULT)0x8007000E)
#define E_INVALIDARG    ((HRESULT)0x80070057)
#define E_NOTIMPL       ((HRESULT)0x80004001)
#define E_POINTER       ((HRESULT)0x80004003)
#define E_UNEXPECTED    ((HRESULT)0x8000FFFF)
#define E_NOINTERFACE   ((HRESULT)0x80004002)
#define SUCCEEDED(hr)   ((HRESULT)(hr) >= 0)
#define FAILED(hr)      ((HRESULT)(hr) < 0)
#define ANYSIZE_ARRAY   1

/*--------------------------------------------------------------------------
  7. Macro tien ich cua windows.h
--------------------------------------------------------------------------*/
#define MAKEWORD(a, b)      ((WORD)(((BYTE)((DWORD_PTR)(a) & 0xff)) | ((WORD)((BYTE)((DWORD_PTR)(b) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)((DWORD_PTR)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD_PTR)(b) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)((DWORD_PTR)(l) & 0xffff))
#define HIWORD(l)           ((WORD)((DWORD_PTR)(l) >> 16))
#define LOBYTE(w)           ((BYTE)((DWORD_PTR)(w) & 0xff))
#define HIBYTE(w)           ((BYTE)((DWORD_PTR)(w) >> 8))
#define MAKELPARAM(l, h)    ((LPARAM)(DWORD)MAKELONG(l, h))
#define MAKEWPARAM(l, h)    ((WPARAM)(DWORD)MAKELONG(l, h))
#define MAKELRESULT(l, h)   ((LRESULT)(DWORD)MAKELONG(l, h))
#define GET_X_LPARAM(lp)    ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)    ((int)(short)HIWORD(lp))
#define RGB(r, g, b)        ((COLORREF)(((BYTE)(r) | ((WORD)((BYTE)(g)) << 8)) | (((DWORD)(BYTE)(b)) << 16)))
#define PALETTERGB(r, g, b) (0x02000000 | RGB(r, g, b))
#define GetRValue(rgb)      ((BYTE)(rgb))
#define GetGValue(rgb)      ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)      ((BYTE)((rgb) >> 16))
#define ZeroMemory(p, n)        memset((p), 0, (n))
#define FillMemory(p, n, v)     memset((p), (v), (n))
#define CopyMemory(d, s, n)     memcpy((d), (s), (n))
#define MoveMemory(d, s, n)     memmove((d), (s), (n))
#define RtlZeroMemory           ZeroMemory
#define RtlFillMemory           FillMemory
#define RtlCopyMemory           CopyMemory
#define RtlMoveMemory           MoveMemory
#define SecureZeroMemory(p, n)  memset((p), 0, (n))
/* min/max: giong windows.h (KHONG NOMINMAX).  LUU Y: libc++ (<algorithm>, <vector>,
   <string>...) se #undef min/max -> ma dung min()/max() sau do se loi "undeclared".
   Port that: dinh nghia NOMINMAX + dung std::min/std::max. */
#ifndef NOMINMAX
#ifndef max
#define max(a, b)           (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b)           (((a) < (b)) ? (a) : (b))
#endif
#endif
#ifndef __max
#define __max(a, b)         (((a) > (b)) ? (a) : (b))
#endif
#ifndef __min
#define __min(a, b)         (((a) < (b)) ? (a) : (b))
#endif

/*--------------------------------------------------------------------------
  8. Winsock -> BSD socket: chi KIEU va HANG SO (ham WSA* van la loi de port)
--------------------------------------------------------------------------*/
typedef int                 SOCKET;
#define INVALID_SOCKET      (-1)
#define SOCKET_ERROR        (-1)
typedef struct sockaddr     SOCKADDR, *PSOCKADDR, *LPSOCKADDR;
typedef struct sockaddr_in  SOCKADDR_IN, *PSOCKADDR_IN, *LPSOCKADDR_IN;
typedef struct in_addr      IN_ADDR, *PIN_ADDR, *LPIN_ADDR;
typedef struct hostent      HOSTENT, *PHOSTENT, *LPHOSTENT;
typedef struct servent      SERVENT, *PSERVENT, *LPSERVENT;
typedef struct protoent     PROTOENT, *PPROTOENT, *LPPROTOENT;
typedef struct timeval      TIMEVAL, *PTIMEVAL, *LPTIMEVAL;
typedef struct linger       LINGER, *PLINGER, *LPLINGER;
typedef struct _WSABUF { ULONG len; CHAR *buf; } WSABUF, *LPWSABUF;
typedef struct WSAData {
    WORD wVersion; WORD wHighVersion; unsigned short iMaxSockets; unsigned short iMaxUdpDg;
    char *lpVendorInfo; char szDescription[257]; char szSystemStatus[129];
} WSADATA, *LPWSADATA;
typedef HANDLE              WSAEVENT, *LPWSAEVENT;
typedef OVERLAPPED          WSAOVERLAPPED, *LPWSAOVERLAPPED;
#define FD_MAX_EVENTS       10
#define FD_READ_BIT         0
#define FD_WRITE_BIT        1
#define FD_OOB_BIT          2
#define FD_ACCEPT_BIT       3
#define FD_CONNECT_BIT      4
#define FD_CLOSE_BIT        5
#define FD_READ             (1 << FD_READ_BIT)
#define FD_WRITE            (1 << FD_WRITE_BIT)
#define FD_OOB              (1 << FD_OOB_BIT)
#define FD_ACCEPT           (1 << FD_ACCEPT_BIT)
#define FD_CONNECT          (1 << FD_CONNECT_BIT)
#define FD_CLOSE            (1 << FD_CLOSE_BIT)
typedef struct _WSANETWORKEVENTS { LONG lNetworkEvents; int iErrorCode[FD_MAX_EVENTS]; } WSANETWORKEVENTS, *LPWSANETWORKEVENTS;
#define WSA_INVALID_EVENT   ((WSAEVENT)NULL)
#define WSA_INFINITE        INFINITE
#define WSA_WAIT_TIMEOUT    WAIT_TIMEOUT
#define WSA_WAIT_FAILED     WAIT_FAILED
#define WSA_WAIT_EVENT_0    WAIT_OBJECT_0
#define SD_RECEIVE          SHUT_RD
#define SD_SEND             SHUT_WR
#define SD_BOTH             SHUT_RDWR
#define closesocket(s)      close(s)
#define ioctlsocket(s, c, a) ioctl((s), (c), (a))
#define WSAEINTR            EINTR
#define WSAEBADF            EBADF
#define WSAEACCES           EACCES
#define WSAEFAULT           EFAULT
#define WSAEINVAL           EINVAL
#define WSAEMFILE           EMFILE
#define WSAEWOULDBLOCK      EWOULDBLOCK
#define WSAEINPROGRESS      EINPROGRESS
#define WSAEALREADY         EALREADY
#define WSAENOTSOCK         ENOTSOCK
#define WSAEMSGSIZE         EMSGSIZE
#define WSAEOPNOTSUPP       EOPNOTSUPP
#define WSAEADDRINUSE       EADDRINUSE
#define WSAEADDRNOTAVAIL    EADDRNOTAVAIL
#define WSAENETDOWN         ENETDOWN
#define WSAENETUNREACH      ENETUNREACH
#define WSAENETRESET        ENETRESET
#define WSAECONNABORTED     ECONNABORTED
#define WSAECONNRESET       ECONNRESET
#define WSAENOBUFS          ENOBUFS
#define WSAEISCONN          EISCONN
#define WSAENOTCONN         ENOTCONN
#define WSAESHUTDOWN        ESHUTDOWN
#define WSAETIMEDOUT        ETIMEDOUT
#define WSAECONNREFUSED     ECONNREFUSED
#define WSAEHOSTDOWN        EHOSTDOWN
#define WSAEHOSTUNREACH     EHOSTUNREACH

/*--------------------------------------------------------------------------
  9. CRT cua MSVC -> POSIX (macro va ham inline nho)
--------------------------------------------------------------------------*/
#ifndef stricmp
#define stricmp             strcasecmp
#endif
#define _stricmp            strcasecmp
#define strcmpi             strcasecmp
#define _strcmpi            strcasecmp
#ifndef strnicmp
#define strnicmp            strncasecmp
#endif
#define _strnicmp           strncasecmp
/* _snprintf cua MSVC KHONG ket NUL khi tran; snprintf co.  Chap nhan (an toan hon). */
#define _snprintf           snprintf
#define _vsnprintf          vsnprintf
#define _snwprintf          swprintf
#define _strdup             strdup
#define _wcsdup             wcsdup
#define _access             access
#define _unlink             unlink
#define _chdir              chdir
#define _rmdir              rmdir
#define _getcwd             getcwd
#define _mkdir(p)           mkdir((p), 0755)
#define _getpid             getpid
#define _fileno             fileno
#define _isatty             isatty
#define _open               open
#define _close              close
#define _read               read
#define _write              write
#define _lseek              lseek
#define _lseeki64           lseek
#define _telli64(fd)        lseek((fd), 0, SEEK_CUR)
#define _tell(fd)           lseek((fd), 0, SEEK_CUR)
#define _commit             fsync
#define _dup                dup
#define _dup2               dup2
#define _fdopen             fdopen
#define _stat               stat
#define _fstat              fstat
#define _fseeki64           fseeko
#define _ftelli64           ftello
#define _atoi64(s)          strtoll((s), (char **)0, 10)
#define _strtoi64           strtoll
#define _strtoui64          strtoull
#define _alloca             alloca
#define _finite(x)          isfinite(x)
#define _isnan(x)           isnan(x)
#define _hypot              hypot
#define _O_RDONLY           O_RDONLY
#define _O_WRONLY           O_WRONLY
#define _O_RDWR             O_RDWR
#define _O_APPEND           O_APPEND
#define _O_CREAT            O_CREAT
#define _O_TRUNC            O_TRUNC
#define _O_EXCL             O_EXCL
#define _O_BINARY           0
#define _O_TEXT             0
#define O_BINARY            0
#define O_TEXT              0
#define _S_IREAD            S_IRUSR
#define _S_IWRITE           S_IWUSR
#define _S_IFDIR            S_IFDIR
#define _S_IFREG            S_IFREG
#define _S_IFMT             S_IFMT

static inline long _filelength(int fd) { struct stat st; return (fstat(fd, &st) == 0) ? (long)st.st_size : -1L; }

static inline char *strupr(char *s) { char *p = s; if (p) for (; *p; ++p) *p = (char)toupper((unsigned char)*p); return s; }
static inline char *strlwr(char *s) { char *p = s; if (p) for (; *p; ++p) *p = (char)tolower((unsigned char)*p); return s; }
static inline char *strrev(char *s) { size_t i, n = s ? strlen(s) : 0; for (i = 0; i < n / 2; ++i) { char c = s[i]; s[i] = s[n - 1 - i]; s[n - 1 - i] = c; } return s; }
static inline char *strset_compat(char *s, int c) { char *p = s; if (p) for (; *p; ++p) *p = (char)c; return s; }
static inline char *strnset_compat(char *s, int c, size_t n) { size_t i; if (s) for (i = 0; i < n && s[i]; ++i) s[i] = (char)c; return s; }
#define _strupr             strupr
#define _strlwr             strlwr
#define _strrev             strrev
#define _strset             strset_compat
#define strset              strset_compat
#define _strnset            strnset_compat
#define strnset             strnset_compat

static inline int memicmp(const void *a, const void *b, size_t n) {
    const unsigned char *p = (const unsigned char *)a, *q = (const unsigned char *)b; size_t i;
    for (i = 0; i < n; ++i) { int d = tolower(p[i]) - tolower(q[i]); if (d) return d; }
    return 0;
}
#define _memicmp            memicmp

/* itoa / ltoa / ultoa / _i64toa: radix 2..36, giong MSVC (radix 10 co dau am) */
static inline char *jx_ulltoa_compat(unsigned long long v, char *buf, int radix, int neg) {
    static const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char tmp[72]; int i = 0, j = 0;
    if (radix < 2 || radix > 36) radix = 10;
    do { tmp[i++] = digits[v % (unsigned)radix]; v /= (unsigned)radix; } while (v);
    if (neg) buf[j++] = '-';
    while (i) buf[j++] = tmp[--i];
    buf[j] = 0; return buf;
}
static inline char *itoa(int v, char *buf, int radix) {
    return (radix == 10 && v < 0) ? jx_ulltoa_compat((unsigned long long)(-(long long)v), buf, 10, 1) : jx_ulltoa_compat((unsigned int)v, buf, radix, 0);
}
static inline char *ltoa(long v, char *buf, int radix) {
    return (radix == 10 && v < 0) ? jx_ulltoa_compat((unsigned long long)(-(long long)v), buf, 10, 1) : jx_ulltoa_compat((unsigned long)v, buf, radix, 0);
}
static inline char *ultoa(unsigned long v, char *buf, int radix) { return jx_ulltoa_compat(v, buf, radix, 0); }
static inline char *_i64toa(long long v, char *buf, int radix) {
    return (radix == 10 && v < 0) ? jx_ulltoa_compat((unsigned long long)(-v), buf, 10, 1) : jx_ulltoa_compat((unsigned long long)v, buf, radix, 0);
}
static inline char *_ui64toa(unsigned long long v, char *buf, int radix) { return jx_ulltoa_compat(v, buf, radix, 0); }
#define _itoa               itoa
#define _ltoa               ltoa
#define _ultoa              ultoa

/* _rotl/_rotr: clang (-fms-extensions) da co san builtin -> chi dinh nghia cho gcc */
#if !defined(__clang__)
static inline unsigned int _rotl(unsigned int v, int s) { s &= 31; return s ? ((v << s) | (v >> (32 - s))) : v; }
static inline unsigned int _rotr(unsigned int v, int s) { s &= 31; return s ? ((v >> s) | (v << (32 - s))) : v; }
static inline unsigned long _lrotl(unsigned long v, int s) { return (unsigned long)_rotl((unsigned int)v, s); }
static inline unsigned long _lrotr(unsigned long v, int s) { return (unsigned long)_rotr((unsigned int)v, s); }
#endif

/* crtdbg.h: bo hoan toan o ban khong Windows */
#ifndef _ASSERT
#define _ASSERT(x)          ((void)0)
#endif
#ifndef _ASSERTE
#define _ASSERTE(x)         ((void)0)
#endif
#define _ASSERT_EXPR(x, m)  ((void)0)
#define _RPT0(t, f)                 ((void)0)
#define _RPT1(t, f, a)              ((void)0)
#define _RPT2(t, f, a, b)           ((void)0)
#define _RPT3(t, f, a, b, c)        ((void)0)
#define _RPT4(t, f, a, b, c, d)     ((void)0)
#define _RPTF0(t, f)                ((void)0)
#define _RPTF1(t, f, a)             ((void)0)
#define _RPTF2(t, f, a, b)          ((void)0)
#define _CrtCheckMemory()           1
#define _CrtDumpMemoryLeaks()       0
#define _CrtSetDbgFlag(x)           0
#define _CrtSetBreakAlloc(x)        0
#define _CrtSetReportMode(t, m)     0
#define _CrtSetReportFile(t, f)     0
#define _CrtMemCheckpoint(s)        ((void)0)
#define _CrtDbgBreak()              ((void)0)
#define _CRTDBG_ALLOC_MEM_DF        0x01
#define _CRTDBG_LEAK_CHECK_DF       0x20
#define _CRTDBG_CHECK_ALWAYS_DF     0x04
#define _CRTDBG_REPORT_FLAG         (-1)
#define _CRT_WARN                   0
#define _CRT_ERROR                  1
#define _CRT_ASSERT                 2
#define _NORMAL_BLOCK               1
#define _CLIENT_BLOCK               2

/*--------------------------------------------------------------------------
  10. Cac ham Win32 anh xa 1-1 sang CRT / builtin (khong can cai dat that)
--------------------------------------------------------------------------*/
#define lstrcpy             strcpy
#define lstrcpyA            strcpy
#define lstrcat             strcat
#define lstrcatA            strcat
#define lstrlen(s)          ((int)strlen(s))
#define lstrlenA(s)         ((int)strlen(s))
#define lstrcmp             strcmp
#define lstrcmpA            strcmp
#define lstrcmpi            strcasecmp
#define lstrcmpiA           strcasecmp
#define wsprintf            sprintf
#define wsprintfA           sprintf
#define wvsprintf           vsprintf
#define wvsprintfA          vsprintf
static inline char *lstrcpyn(char *d, const char *s, int n) { if (n > 0) { strncpy(d, s, (size_t)n - 1); d[n - 1] = 0; } return d; }
#define lstrcpynA           lstrcpyn

static inline LONG InterlockedIncrement(volatile LONG *p) { return __sync_add_and_fetch(p, 1); }
static inline LONG InterlockedDecrement(volatile LONG *p) { return __sync_sub_and_fetch(p, 1); }
static inline LONG InterlockedExchange(volatile LONG *p, LONG v) { return __sync_lock_test_and_set(p, v); }
static inline LONG InterlockedExchangeAdd(volatile LONG *p, LONG v) { return __sync_fetch_and_add(p, v); }
static inline LONG InterlockedCompareExchange(volatile LONG *p, LONG x, LONG cmp) { return __sync_val_compare_and_swap(p, cmp, x); }
static inline PVOID InterlockedExchangePointer(PVOID volatile *p, PVOID v) { return __sync_lock_test_and_set(p, v); }
static inline PVOID InterlockedCompareExchangePointer(PVOID volatile *p, PVOID x, PVOID cmp) { return __sync_val_compare_and_swap(p, cmp, x); }

/*--------------------------------------------------------------------------
  11. COM / DirectX: chi macro khai bao giao dien (re); IUnknown va cac
      giao dien DirectX KHONG co - tep DDraw/DInput/DSound phai loai khoi ban Android
--------------------------------------------------------------------------*/
#ifdef __cplusplus
#define interface               struct
#define STDMETHODCALLTYPE
#define STDMETHODVCALLTYPE
#define STDAPICALLTYPE
#define STDMETHOD(m)            virtual HRESULT STDMETHODCALLTYPE m
#define STDMETHOD_(t, m)        virtual t STDMETHODCALLTYPE m
#define STDMETHODIMP            HRESULT STDMETHODCALLTYPE
#define STDMETHODIMP_(t)        t STDMETHODCALLTYPE
#define STDAPI                  extern "C" HRESULT STDAPICALLTYPE
#define STDAPI_(t)              extern "C" t STDAPICALLTYPE
#define PURE                    = 0
#define THIS_
#define THIS                    void
#define DECLARE_INTERFACE(i)    struct i
#define DECLARE_INTERFACE_(i, b) struct i : public b
#define BEGIN_INTERFACE
#define END_INTERFACE
/* IUnknown toi thieu (ABI COM: 3 ham ao).  Du an KHONG dung COM that: Headers/IClient.h, Rainbow
   Interface/IClient.h ... chi ke thua IUnknown lam giao dien co so (QueryInterface/AddRef/Release). */
#ifndef __IUnknown_INTERFACE_DEFINED__
#define __IUnknown_INTERFACE_DEFINED__
struct IUnknown {
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) = 0;
    virtual ULONG   STDMETHODCALLTYPE AddRef(void) = 0;
    virtual ULONG   STDMETHODCALLTYPE Release(void) = 0;
};
typedef IUnknown *LPUNKNOWN;
static const IID IID_IUnknown = { 0x00000000, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };
#endif
#endif /* __cplusplus */

/*--------------------------------------------------------------------------
  12. Hang so thong diep / phim / chuot / hop thoai cua user32 (winuser.h).
      KHONG phai "Win32 API can port": KSdlApp (SDL) dich SDL_Event -> WM_KEYDOWN / VK_* / MK_*
      roi day vao hang doi thong diep noi bo cua engine; ma UI so sanh voi VK_ESCAPE,
      WM_LBUTTONDOWN ...  => day la GIAO THUC NOI BO, phai giu dung gia tri Windows.
      (MessageBox/SendMessage/... van la loi de port; chi hang so o day.)
--------------------------------------------------------------------------*/
#define WM_NULL             0x0000
#define WM_CREATE           0x0001
#define WM_DESTROY          0x0002
#define WM_MOVE             0x0003
#define WM_SIZE             0x0005
#define WM_ACTIVATE         0x0006
#define WA_INACTIVE         0
#define WA_ACTIVE           1
#define WA_CLICKACTIVE      2
#define WM_SETFOCUS         0x0007
#define WM_KILLFOCUS        0x0008
#define WM_ENABLE           0x000A
#define WM_SETREDRAW        0x000B
#define WM_SETTEXT          0x000C
#define WM_GETTEXT          0x000D
#define WM_PAINT            0x000F
#define WM_CLOSE            0x0010
#define WM_QUERYENDSESSION  0x0011
#define WM_QUIT             0x0012
#define WM_ERASEBKGND       0x0014
#define WM_SYSCOLORCHANGE   0x0015
#define WM_ENDSESSION       0x0016
#define WM_SHOWWINDOW       0x0018
#define WM_ACTIVATEAPP      0x001C
#define WM_SETCURSOR        0x0020
#define WM_MOUSEACTIVATE    0x0021
#define WM_GETMINMAXINFO    0x0024
#define WM_SETFONT          0x0030
#define WM_GETFONT          0x0031
#define WM_WINDOWPOSCHANGING 0x0046
#define WM_WINDOWPOSCHANGED 0x0047
#define WM_COPYDATA         0x004A
#define WM_INPUTLANGCHANGE  0x0051
#define WM_CONTEXTMENU      0x007B
#define WM_STYLECHANGED     0x007D
#define WM_DISPLAYCHANGE    0x007E
#define WM_NCCREATE         0x0081
#define WM_NCDESTROY        0x0082
#define WM_NCHITTEST        0x0084
#define WM_NCPAINT          0x0085
#define WM_NCACTIVATE       0x0086
#define WM_NCMOUSEMOVE      0x00A0
#define WM_NCLBUTTONDOWN    0x00A1
#define WM_NCLBUTTONUP      0x00A2
#define WM_KEYFIRST         0x0100
#define WM_KEYDOWN          0x0100
#define WM_KEYUP            0x0101
#define WM_CHAR             0x0102
#define WM_DEADCHAR         0x0103
#define WM_SYSKEYDOWN       0x0104
#define WM_SYSKEYUP         0x0105
#define WM_SYSCHAR          0x0106
#define WM_SYSDEADCHAR      0x0107
#define WM_KEYLAST          0x0109
#define WM_IME_STARTCOMPOSITION 0x010D
#define WM_IME_ENDCOMPOSITION   0x010E
#define WM_IME_COMPOSITION      0x010F
#define WM_IME_KEYLAST          0x010F
#define WM_INITDIALOG       0x0110
#define WM_COMMAND          0x0111
#define WM_SYSCOMMAND       0x0112
#define WM_TIMER            0x0113
#define WM_HSCROLL          0x0114
#define WM_VSCROLL          0x0115
#define WM_MENUSELECT       0x011F
#define WM_MOUSEFIRST       0x0200
#define WM_MOUSEMOVE        0x0200
#define WM_LBUTTONDOWN      0x0201
#define WM_LBUTTONUP        0x0202
#define WM_LBUTTONDBLCLK    0x0203
#define WM_RBUTTONDOWN      0x0204
#define WM_RBUTTONUP        0x0205
#define WM_RBUTTONDBLCLK    0x0206
#define WM_MBUTTONDOWN      0x0207
#define WM_MBUTTONUP        0x0208
#define WM_MBUTTONDBLCLK    0x0209
#define WM_MOUSEWHEEL       0x020A
#define WM_XBUTTONDOWN      0x020B
#define WM_XBUTTONUP        0x020C
#define WM_XBUTTONDBLCLK    0x020D
#define WM_MOUSELAST        0x020D
#define WM_ENTERMENULOOP    0x0211
#define WM_EXITMENULOOP     0x0212
#define WM_SIZING           0x0214
#define WM_CAPTURECHANGED   0x0215
#define WM_MOVING           0x0216
#define WM_POWERBROADCAST   0x0218
#define WM_DEVICECHANGE     0x0219
#define WM_ENTERSIZEMOVE    0x0231
#define WM_EXITSIZEMOVE     0x0232
#define WM_DROPFILES        0x0233
#define WM_IME_SETCONTEXT   0x0281
#define WM_IME_NOTIFY       0x0282
#define WM_IME_CONTROL      0x0283
#define WM_IME_COMPOSITIONFULL 0x0284
#define WM_IME_SELECT       0x0285
#define WM_IME_CHAR         0x0286
#define WM_IME_REQUEST      0x0288
#define WM_IME_KEYDOWN      0x0290
#define WM_IME_KEYUP        0x0291
#define WM_MOUSEHOVER       0x02A1
#define WM_NCMOUSELEAVE     0x02A2
#define WM_MOUSELEAVE       0x02A3
#define WM_CUT              0x0300
#define WM_COPY             0x0301
#define WM_PASTE            0x0302
#define WM_CLEAR            0x0303
#define WM_UNDO             0x0304
#define WM_HOTKEY           0x0312
#define WM_PRINT            0x0317
#define WM_USER             0x0400
#define WM_APP              0x8000
#define WHEEL_DELTA         120
#define GET_WHEEL_DELTA_WPARAM(w)   ((short)HIWORD(w))
#define GET_KEYSTATE_WPARAM(w)      (LOWORD(w))
#define GET_XBUTTON_WPARAM(w)       (HIWORD(w))
#define XBUTTON1            0x0001
#define XBUTTON2            0x0002
#define KF_EXTENDED         0x0100
#define KF_DLGMODE          0x0800
#define KF_MENUMODE         0x1000
#define KF_ALTDOWN          0x2000
#define KF_REPEAT           0x4000
#define KF_UP               0x8000
/* MK_*: wParam cua thong diep chuot */
#define MK_LBUTTON          0x0001
#define MK_RBUTTON          0x0002
#define MK_SHIFT            0x0004
#define MK_CONTROL          0x0008
#define MK_MBUTTON          0x0010
#define MK_XBUTTON1         0x0020
#define MK_XBUTTON2         0x0040
/* VK_*: ma phim ao */
#define VK_LBUTTON          0x01
#define VK_RBUTTON          0x02
#define VK_CANCEL           0x03
#define VK_MBUTTON          0x04
#define VK_XBUTTON1         0x05
#define VK_XBUTTON2         0x06
#define VK_BACK             0x08
#define VK_TAB              0x09
#define VK_CLEAR            0x0C
#define VK_RETURN           0x0D
#define VK_SHIFT            0x10
#define VK_CONTROL          0x11
#define VK_MENU             0x12
#define VK_PAUSE            0x13
#define VK_CAPITAL          0x14
#define VK_KANA             0x15
#define VK_HANGUL           0x15
#define VK_JUNJA            0x17
#define VK_FINAL            0x18
#define VK_HANJA            0x19
#define VK_KANJI            0x19
#define VK_ESCAPE           0x1B
#define VK_CONVERT          0x1C
#define VK_NONCONVERT       0x1D
#define VK_ACCEPT           0x1E
#define VK_MODECHANGE       0x1F
#define VK_SPACE            0x20
#define VK_PRIOR            0x21
#define VK_NEXT             0x22
#define VK_END              0x23
#define VK_HOME             0x24
#define VK_LEFT             0x25
#define VK_UP               0x26
#define VK_RIGHT            0x27
#define VK_DOWN             0x28
#define VK_SELECT           0x29
#define VK_PRINT            0x2A
#define VK_EXECUTE          0x2B
#define VK_SNAPSHOT         0x2C
#define VK_INSERT           0x2D
#define VK_DELETE           0x2E
#define VK_HELP             0x2F
#define VK_LWIN             0x5B
#define VK_RWIN             0x5C
#define VK_APPS             0x5D
#define VK_SLEEP            0x5F
#define VK_NUMPAD0          0x60
#define VK_NUMPAD1          0x61
#define VK_NUMPAD2          0x62
#define VK_NUMPAD3          0x63
#define VK_NUMPAD4          0x64
#define VK_NUMPAD5          0x65
#define VK_NUMPAD6          0x66
#define VK_NUMPAD7          0x67
#define VK_NUMPAD8          0x68
#define VK_NUMPAD9          0x69
#define VK_MULTIPLY         0x6A
#define VK_ADD              0x6B
#define VK_SEPARATOR        0x6C
#define VK_SUBTRACT         0x6D
#define VK_DECIMAL          0x6E
#define VK_DIVIDE           0x6F
#define VK_F1               0x70
#define VK_F2               0x71
#define VK_F3               0x72
#define VK_F4               0x73
#define VK_F5               0x74
#define VK_F6               0x75
#define VK_F7               0x76
#define VK_F8               0x77
#define VK_F9               0x78
#define VK_F10              0x79
#define VK_F11              0x7A
#define VK_F12              0x7B
#define VK_F13              0x7C
#define VK_F14              0x7D
#define VK_F15              0x7E
#define VK_F16              0x7F
#define VK_F17              0x80
#define VK_F18              0x81
#define VK_F19              0x82
#define VK_F20              0x83
#define VK_F21              0x84
#define VK_F22              0x85
#define VK_F23              0x86
#define VK_F24              0x87
#define VK_NUMLOCK          0x90
#define VK_SCROLL           0x91
#define VK_LSHIFT           0xA0
#define VK_RSHIFT           0xA1
#define VK_LCONTROL         0xA2
#define VK_RCONTROL         0xA3
#define VK_LMENU            0xA4
#define VK_RMENU            0xA5
#define VK_BROWSER_BACK     0xA6
#define VK_BROWSER_FORWARD  0xA7
#define VK_VOLUME_MUTE      0xAD
#define VK_VOLUME_DOWN      0xAE
#define VK_VOLUME_UP        0xAF
#define VK_OEM_1            0xBA
#define VK_OEM_PLUS         0xBB
#define VK_OEM_COMMA        0xBC
#define VK_OEM_MINUS        0xBD
#define VK_OEM_PERIOD       0xBE
#define VK_OEM_2            0xBF
#define VK_OEM_3            0xC0
#define VK_OEM_4            0xDB
#define VK_OEM_5            0xDC
#define VK_OEM_6            0xDD
#define VK_OEM_7            0xDE
#define VK_OEM_8            0xDF
#define VK_OEM_102          0xE2
#define VK_PROCESSKEY       0xE5
#define VK_PACKET           0xE7
#define VK_ATTN             0xF6
#define VK_CRSEL            0xF7
#define VK_EXSEL            0xF8
#define VK_EREOF            0xF9
#define VK_PLAY             0xFA
#define VK_ZOOM             0xFB
#define VK_NONAME           0xFC
#define VK_PA1              0xFD
#define VK_OEM_CLEAR        0xFE
/* HOTKEYF_*: co phim tat (commctrl.h) - ShortcutKey.cpp */
#define HOTKEYF_SHIFT       0x01
#define HOTKEYF_CONTROL     0x02
#define HOTKEYF_ALT         0x04
#define HOTKEYF_EXT         0x08
/* MB_* / ID*: co MessageBox va ket qua (ham MessageBox van la loi de port) */
#define MB_OK               0x00000000L
#define MB_OKCANCEL         0x00000001L
#define MB_ABORTRETRYIGNORE 0x00000002L
#define MB_YESNOCANCEL      0x00000003L
#define MB_YESNO            0x00000004L
#define MB_RETRYCANCEL      0x00000005L
#define MB_ICONHAND         0x00000010L
#define MB_ICONQUESTION     0x00000020L
#define MB_ICONEXCLAMATION  0x00000030L
#define MB_ICONASTERISK     0x00000040L
#define MB_ICONWARNING      MB_ICONEXCLAMATION
#define MB_ICONERROR        MB_ICONHAND
#define MB_ICONINFORMATION  MB_ICONASTERISK
#define MB_ICONSTOP         MB_ICONHAND
#define MB_DEFBUTTON1       0x00000000L
#define MB_DEFBUTTON2       0x00000100L
#define MB_DEFBUTTON3       0x00000200L
#define MB_APPLMODAL        0x00000000L
#define MB_SYSTEMMODAL      0x00001000L
#define MB_TASKMODAL        0x00002000L
#define MB_SETFOREGROUND    0x00010000L
#define MB_TOPMOST          0x00040000L
#define IDOK                1
#define IDCANCEL            2
#define IDABORT             3
#define IDRETRY             4
#define IDIGNORE            5
#define IDYES               6
#define IDNO                7
/* SW_*: ShowWindow (ham van la loi) */
#define SW_HIDE             0
#define SW_SHOWNORMAL       1
#define SW_NORMAL           1
#define SW_SHOWMINIMIZED    2
#define SW_SHOWMAXIMIZED    3
#define SW_MAXIMIZE         3
#define SW_SHOWNOACTIVATE   4
#define SW_SHOW             5
#define SW_MINIMIZE         6
#define SW_SHOWMINNOACTIVE  7
#define SW_SHOWNA           8
#define SW_RESTORE          9
/* code page (MultiByteToWideChar/WideCharToMultiByte van la loi: dung SDL_iconv) */
#define CP_ACP              0
#define CP_OEMCP            1
#define CP_UTF7             65000
#define CP_UTF8             65001

/*--------------------------------------------------------------------------
  13. Ham RECT cua user32 (thuan toan hoc tren struct, anh xa 1-1)
--------------------------------------------------------------------------*/
static inline BOOL SetRect(RECT *r, int l, int t, int rt, int b) { if (!r) return FALSE; r->left = l; r->top = t; r->right = rt; r->bottom = b; return TRUE; }
static inline BOOL SetRectEmpty(RECT *r) { if (!r) return FALSE; r->left = r->top = r->right = r->bottom = 0; return TRUE; }
static inline BOOL CopyRect(RECT *d, const RECT *s) { if (!d || !s) return FALSE; *d = *s; return TRUE; }
static inline BOOL IsRectEmpty(const RECT *r) { return (!r || r->right <= r->left || r->bottom <= r->top) ? TRUE : FALSE; }
static inline BOOL EqualRect(const RECT *a, const RECT *b) { return (a && b && memcmp(a, b, sizeof(RECT)) == 0) ? TRUE : FALSE; }
static inline BOOL PtInRect(const RECT *r, POINT p) { return (r && p.x >= r->left && p.x < r->right && p.y >= r->top && p.y < r->bottom) ? TRUE : FALSE; }
static inline BOOL OffsetRect(RECT *r, int dx, int dy) { if (!r) return FALSE; r->left += dx; r->right += dx; r->top += dy; r->bottom += dy; return TRUE; }
static inline BOOL InflateRect(RECT *r, int dx, int dy) { if (!r) return FALSE; r->left -= dx; r->right += dx; r->top -= dy; r->bottom += dy; return TRUE; }
static inline BOOL IntersectRect(RECT *d, const RECT *a, const RECT *b) {
    if (!d || !a || !b) return FALSE;
    d->left = a->left > b->left ? a->left : b->left;  d->top = a->top > b->top ? a->top : b->top;
    d->right = a->right < b->right ? a->right : b->right;  d->bottom = a->bottom < b->bottom ? a->bottom : b->bottom;
    if (d->left >= d->right || d->top >= d->bottom) { d->left = d->top = d->right = d->bottom = 0; return FALSE; }
    return TRUE;
}
static inline BOOL UnionRect(RECT *d, const RECT *a, const RECT *b) {
    if (!d || !a || !b) return FALSE;
    if (IsRectEmpty(a)) { if (IsRectEmpty(b)) { SetRectEmpty(d); return FALSE; } *d = *b; return TRUE; }
    if (IsRectEmpty(b)) { *d = *a; return TRUE; }
    d->left = a->left < b->left ? a->left : b->left;  d->top = a->top < b->top ? a->top : b->top;
    d->right = a->right > b->right ? a->right : b->right;  d->bottom = a->bottom > b->bottom ? a->bottom : b->bottom;
    return TRUE;
}

/*--------------------------------------------------------------------------
  14. Kieu / hang so tep cua kernel32 (ham FindFirstFile/CreateFile... van la loi: dung dirent/fopen)
--------------------------------------------------------------------------*/
typedef struct _WIN32_FIND_DATAA {
    DWORD dwFileAttributes; FILETIME ftCreationTime; FILETIME ftLastAccessTime; FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh; DWORD nFileSizeLow; DWORD dwReserved0; DWORD dwReserved1;
    CHAR cFileName[MAX_PATH]; CHAR cAlternateFileName[14];
} WIN32_FIND_DATAA, *PWIN32_FIND_DATAA, *LPWIN32_FIND_DATAA, WIN32_FIND_DATA, *PWIN32_FIND_DATA, *LPWIN32_FIND_DATA;
#define FILE_ATTRIBUTE_READONLY     0x00000001
#define FILE_ATTRIBUTE_HIDDEN       0x00000002
#define FILE_ATTRIBUTE_SYSTEM       0x00000004
#define FILE_ATTRIBUTE_DIRECTORY    0x00000010
#define FILE_ATTRIBUTE_ARCHIVE      0x00000020
#define FILE_ATTRIBUTE_NORMAL       0x00000080
#define FILE_ATTRIBUTE_TEMPORARY    0x00000100
#define INVALID_FILE_ATTRIBUTES     ((DWORD)-1)
#define GENERIC_READ                0x80000000
#define GENERIC_WRITE               0x40000000
#define FILE_SHARE_READ             0x00000001
#define FILE_SHARE_WRITE            0x00000002
#define CREATE_NEW                  1
#define CREATE_ALWAYS               2
#define OPEN_EXISTING               3
#define OPEN_ALWAYS                 4
#define TRUNCATE_EXISTING           5
#define ERROR_FILE_NOT_FOUND        2
#define ERROR_PATH_NOT_FOUND        3
#define ERROR_ACCESS_DENIED         5
#define ERROR_INVALID_HANDLE        6
#define ERROR_NOT_ENOUGH_MEMORY     8
#define ERROR_INVALID_PARAMETER     87
#define ERROR_ALREADY_EXISTS        183
#define ERROR_MORE_DATA             234
#define ERROR_NO_MORE_ITEMS         259
#define STILL_ACTIVE                259
#define ERROR_OPERATION_ABORTED     995
#define ERROR_IO_PENDING            997
#define WSA_IO_PENDING              ERROR_IO_PENDING
#define WSAEDISCON                  10101       /* khong co tuong duong POSIX: giu ma so Winsock */
#define WSAENOMORE                  10102
/* hang so luong / SEH / anh xa bo nho / tai nguyen (cac ham CreateThread, __try, CreateFileMapping, LoadCursor... van la loi) */
#define THREAD_PRIORITY_IDLE            (-15)
#define THREAD_PRIORITY_LOWEST          (-2)
#define THREAD_PRIORITY_BELOW_NORMAL    (-1)
#define THREAD_PRIORITY_NORMAL          0
#define THREAD_PRIORITY_ABOVE_NORMAL    1
#define THREAD_PRIORITY_HIGHEST         2
#define THREAD_PRIORITY_TIME_CRITICAL   15
#define CREATE_SUSPENDED                0x00000004
#define EXCEPTION_EXECUTE_HANDLER       1
#define EXCEPTION_CONTINUE_SEARCH       0
#define EXCEPTION_CONTINUE_EXECUTION    (-1)
#define PAGE_NOACCESS                   0x01
#define PAGE_READONLY                   0x02
#define PAGE_READWRITE                  0x04
#define FILE_MAP_COPY                   0x0001
#define FILE_MAP_WRITE                  0x0002
#define FILE_MAP_READ                   0x0004
#define FILE_MAP_ALL_ACCESS             0x000F001F
#define MAKEINTRESOURCE(i)              ((LPSTR)((ULONG_PTR)((WORD)(i))))
#define MAKEINTRESOURCEA(i)             MAKEINTRESOURCE(i)
#define IDC_ARROW                       MAKEINTRESOURCE(32512)
#define IDC_IBEAM                       MAKEINTRESOURCE(32513)
#define IDC_WAIT                        MAKEINTRESOURCE(32514)
#define IDC_CROSS                       MAKEINTRESOURCE(32515)
#define IDC_HAND                        MAKEINTRESOURCE(32649)
#define IDI_APPLICATION                 MAKEINTRESOURCE(32512)
/* MulDiv: (a*b)/c 64 bit co lam tron, giong kernel32 (tra -1 khi chia 0 / tran) */
static inline int MulDiv(int a, int b, int c) {
    long long r;
    if (c == 0) return -1;
    r = (long long)a * (long long)b;
    r = (r >= 0) == (c > 0) ? (r + (long long)(c < 0 ? -c : c) / 2) / c : (r - (long long)(c < 0 ? -c : c) / 2) / c;
    if (r > 2147483647LL || r < -2147483648LL) return -1;
    return (int)r;
}

/*--------------------------------------------------------------------------
  15. "Secure CRT" cua MSVC (sprintf_s, strcpy_s ...): ma nguon dung ca dang mau
      sprintf_s(Buffer, "%d", x) lan dang tuong minh sprintf_s(p, n, "%d", x)
      -> qua tai C++ (chi C++; cac tep .c khong dung).  sscanf_s KHONG anh xa (khac ngu nghia %s/%c).
--------------------------------------------------------------------------*/
#ifdef __cplusplus
#include <time.h>
#ifndef _TRUNCATE
#define _TRUNCATE ((size_t)-1)
#endif
static inline int strcpy_s(char *d, size_t n, const char *s) {
    if (!d || !n) return EINVAL;
    if (!s) { d[0] = 0; return EINVAL; }
    if (strlen(s) >= n) { d[0] = 0; return ERANGE; }
    strcpy(d, s); return 0;
}
template <size_t N> static inline int strcpy_s(char (&d)[N], const char *s) { return strcpy_s(d, N, s); }
static inline int strcat_s(char *d, size_t n, const char *s) {
    if (!d || !n || !s) return EINVAL;
    size_t l = strnlen(d, n);
    if (l >= n || strlen(s) >= n - l) { d[0] = 0; return ERANGE; }
    strcat(d, s); return 0;
}
template <size_t N> static inline int strcat_s(char (&d)[N], const char *s) { return strcat_s(d, N, s); }
static inline int strncpy_s(char *d, size_t n, const char *s, size_t c) {
    if (!d || !n) return EINVAL;
    if (!s) { d[0] = 0; return EINVAL; }
    size_t l = strnlen(s, c == _TRUNCATE ? n : c);
    if (l >= n) { if (c == _TRUNCATE) { memcpy(d, s, n - 1); d[n - 1] = 0; return 0; } d[0] = 0; return ERANGE; }
    memcpy(d, s, l); d[l] = 0; return 0;
}
template <size_t N> static inline int strncpy_s(char (&d)[N], const char *s, size_t c) { return strncpy_s(d, N, s, c); }
static inline int vsprintf_s(char *d, size_t n, const char *f, va_list a) { return vsnprintf(d, n, f, a); }
template <size_t N> static inline int vsprintf_s(char (&d)[N], const char *f, va_list a) { return vsnprintf(d, N, f, a); }
static inline int sprintf_s(char *d, size_t n, const char *f, ...) { va_list a; va_start(a, f); int r = vsnprintf(d, n, f, a); va_end(a); return r; }
template <size_t N> static inline int sprintf_s(char (&d)[N], const char *f, ...) { va_list a; va_start(a, f); int r = vsnprintf(d, N, f, a); va_end(a); return r; }
static inline int _vsnprintf_s(char *d, size_t n, size_t c, const char *f, va_list a) { int r = vsnprintf(d, (c == _TRUNCATE || c >= n) ? n : c + 1, f, a); return r; }
template <size_t N> static inline int _vsnprintf_s(char (&d)[N], size_t c, const char *f, va_list a) { return _vsnprintf_s(d, N, c, f, a); }
static inline int _snprintf_s(char *d, size_t n, size_t c, const char *f, ...) { va_list a; va_start(a, f); int r = _vsnprintf_s(d, n, c, f, a); va_end(a); return r; }
template <size_t N> static inline int _snprintf_s(char (&d)[N], size_t c, const char *f, ...) { va_list a; va_start(a, f); int r = _vsnprintf_s(d, N, c, f, a); va_end(a); return r; }
#define strtok_s            strtok_r
static inline int localtime_s(struct tm *t, const time_t *tt) { return (t && tt && localtime_r(tt, t)) ? 0 : EINVAL; }
static inline int gmtime_s(struct tm *t, const time_t *tt) { return (t && tt && gmtime_r(tt, t)) ? 0 : EINVAL; }
#define _localtime64_s      localtime_s
#define _gmtime64_s         gmtime_s
static inline int ctime_s(char *b, size_t n, const time_t *t) { if (!b || n < 26 || !t) return EINVAL; ctime_r(t, b); return 0; }
static inline int fopen_s(FILE **f, const char *name, const char *mode) { if (!f) return EINVAL; *f = fopen(name, mode); return *f ? 0 : errno; }
static inline int memcpy_s(void *d, size_t dn, const void *s, size_t c) { if (!d) return EINVAL; if (!s || c > dn) { memset(d, 0, dn); return ERANGE; } memcpy(d, s, c); return 0; }
static inline int memmove_s(void *d, size_t dn, const void *s, size_t c) { if (!d || !s) return EINVAL; if (c > dn) return ERANGE; memmove(d, s, c); return 0; }
static inline int _itoa_s(int v, char *b, size_t n, int r) { (void)n; itoa(v, b, r); return 0; }
template <size_t N> static inline int _itoa_s(int v, char (&b)[N], int r) { itoa(v, b, r); return 0; }
static inline int _ltoa_s(long v, char *b, size_t n, int r) { (void)n; ltoa(v, b, r); return 0; }
template <size_t N> static inline int _ltoa_s(long v, char (&b)[N], int r) { ltoa(v, b, r); return 0; }
static inline int _ultoa_s(unsigned long v, char *b, size_t n, int r) { (void)n; ultoa(v, b, r); return 0; }
static inline int _i64toa_s(long long v, char *b, size_t n, int r) { (void)n; _i64toa(v, b, r); return 0; }
static inline int _strlwr_s(char *s, size_t n) { (void)n; strlwr(s); return 0; }
template <size_t N> static inline int _strlwr_s(char (&s)[N]) { strlwr(s); return 0; }
static inline int _strupr_s(char *s, size_t n) { (void)n; strupr(s); return 0; }
template <size_t N> static inline int _strupr_s(char (&s)[N]) { strupr(s); return 0; }
static inline int _strdate_s(char *b, size_t n) { time_t t = time(0); struct tm tmv; localtime_r(&t, &tmv); return (b && n >= 9 && strftime(b, n, "%m/%d/%y", &tmv)) ? 0 : EINVAL; }
static inline int _strtime_s(char *b, size_t n) { time_t t = time(0); struct tm tmv; localtime_r(&t, &tmv); return (b && n >= 9 && strftime(b, n, "%H:%M:%S", &tmv)) ? 0 : EINVAL; }
#endif /* __cplusplus */

#endif /* !_WIN32 */
#endif /* KPOSIXCOMPAT_H */

/*--------------------------------------------------------------------------
  NGOAI include guard (giong guiddef.h): DEFINE_GUID doi theo INITGUID.
  Header khai bao: EXTERN_C const GUID name;  .cpp co #define INITGUID (hoac #include <initguid.h>)
  roi include lai tep nay: dinh nghia that.
--------------------------------------------------------------------------*/
#if !defined(_WIN32)
#ifdef DEFINE_GUID
#undef DEFINE_GUID
#endif
#ifdef INITGUID
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    EXTERN_C const GUID name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }
#else
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    EXTERN_C const GUID name
#endif
#endif

/* [ANDROID 08/09] tap con ham Win32 cai that (su kien/luong/thoi gian/tep/INI/user32) */
#if !defined(_WIN32)
#include "KPosixWin32.h"
#endif
