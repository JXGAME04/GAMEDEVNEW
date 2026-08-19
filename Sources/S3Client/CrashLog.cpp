//---------------------------------------------------------------------------
//  CrashLog.cpp - bat loi sap game, ghi jx_crash.log
//
//  Nguyen tac viet trong bo bat loi: luc nay tien trinh DA HONG, nen
//    - KHONG cap phat bo nho (khong new/malloc/std::string)
//    - KHONG goi ham co the lai crash ma khong duoc bao ve
//    - Moi thu boc trong __try/__except de khong chet lan hai
//    - Ghi bang WriteFile tren bo dem stack
//
//  dbghelp.dll nap DONG => khong them thu vien lien ket nao vao du an.
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "CrashLog.h"
#include <stdio.h>

//--- kieu cua dbghelp, khai bao tay de khoi phu thuoc header SDK ---
typedef struct _CL_ADDRESS64 { DWORD64 Offset; WORD Segment; DWORD Mode; } CL_ADDRESS64;
typedef struct _CL_KDHELP64 { DWORD64 r[16]; } CL_KDHELP64;
typedef struct _CL_STACKFRAME64
{
    CL_ADDRESS64 AddrPC, AddrReturn, AddrFrame, AddrStack, AddrBStore;
    PVOID        FuncTableEntry;
    DWORD64      Params[4];
    BOOL         Far, Virtual;
    DWORD64      Reserved[3];
    CL_KDHELP64  KdHelp;
} CL_STACKFRAME64;

typedef BOOL (WINAPI *PFN_SymInitialize)(HANDLE, PCSTR, BOOL);
typedef DWORD (WINAPI *PFN_SymSetOptions)(DWORD);
typedef BOOL (WINAPI *PFN_StackWalk64)(DWORD, HANDLE, HANDLE, CL_STACKFRAME64*, PVOID,
                                       PVOID, PVOID, PVOID, PVOID);
typedef PVOID (WINAPI *PFN_SymFunctionTableAccess64)(HANDLE, DWORD64);
typedef DWORD64 (WINAPI *PFN_SymGetModuleBase64)(HANDLE, DWORD64);

static PFN_SymInitialize            s_pSymInitialize   = NULL;
static PFN_SymSetOptions            s_pSymSetOptions   = NULL;
static PFN_StackWalk64              s_pStackWalk64     = NULL;
static PFN_SymFunctionTableAccess64 s_pSymFuncTable    = NULL;
static PFN_SymGetModuleBase64       s_pSymGetModBase   = NULL;

static char  s_szLogPath[MAX_PATH] = {0};
static LPTOP_LEVEL_EXCEPTION_FILTER s_pPrevFilter = NULL;

//---------------------------------------------------------------------------
static void CL_Write(const char* pszText)
{
    if (!s_szLogPath[0] || !pszText)
        return;
    HANDLE hFile = CreateFileA(s_szLogPath, FILE_APPEND_DATA, FILE_SHARE_READ,
                               NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return;
    SetFilePointer(hFile, 0, NULL, FILE_END);
    DWORD dwWritten = 0;
    WriteFile(hFile, pszText, (DWORD)strlen(pszText), &dwWritten, NULL);
    FlushFileBuffers(hFile);
    CloseHandle(hFile);
}

//--- tim module chua dia chi, tra ve ten + do lech (RVA) ---
static void CL_ModuleOf(void* pAddr, char* szName, int nNameSize, DWORD* pdwRva)
{
    szName[0] = 0;
    *pdwRva = 0;
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(pAddr, &mbi, sizeof(mbi)) != sizeof(mbi))
        return;
    HMODULE hMod = (HMODULE)mbi.AllocationBase;
    if (!hMod)
        return;
    char szFull[MAX_PATH];
    szFull[0] = 0;
    if (GetModuleFileNameA(hMod, szFull, MAX_PATH))
    {
        const char* p = strrchr(szFull, '\\');
        _snprintf(szName, nNameSize - 1, "%s", p ? p + 1 : szFull);
        szName[nNameSize - 1] = 0;
    }
    *pdwRva = (DWORD)((BYTE*)pAddr - (BYTE*)hMod);
}

//--- in danh sach module dang nap (de doi chieu dia chi voi ban build) ---
static void CL_DumpModules()
{
    typedef BOOL (WINAPI *PFN_EnumProcessModules)(HANDLE, HMODULE*, DWORD, LPDWORD);
    HMODULE hPsapi = LoadLibraryA("psapi.dll");
    if (!hPsapi)
        return;
    PFN_EnumProcessModules pfn = (PFN_EnumProcessModules)GetProcAddress(hPsapi, "EnumProcessModules");
    if (!pfn)
        return;

    HMODULE hMods[256];
    DWORD   dwNeeded = 0;
    if (!pfn(GetCurrentProcess(), hMods, sizeof(hMods), &dwNeeded))
        return;

    CL_Write("  -- module dang nap (ten | dia chi nap | kich thuoc) --\r\n");
    int nCount = (int)(dwNeeded / sizeof(HMODULE));
    if (nCount > 256)
        nCount = 256;
    for (int i = 0; i < nCount; i++)
    {
        char szFull[MAX_PATH];
        szFull[0] = 0;
        if (!GetModuleFileNameA(hMods[i], szFull, MAX_PATH))
            continue;
        const char* p = strrchr(szFull, '\\');
        const char* pszName = p ? p + 1 : szFull;
        // chi in cac module cua game cho gon
        if (!strstr(pszName, ".exe") && !strstr(pszName, "Core") && !strstr(pszName, "core")
            && !strstr(pszName, "ngine") && !strstr(pszName, "epresent")
            && !strstr(pszName, "ualib") && !strstr(pszName, "xpand"))
            continue;
        MEMORY_BASIC_INFORMATION mbi;
        SIZE_T uSize = 0;
        if (VirtualQuery(hMods[i], &mbi, sizeof(mbi)) == sizeof(mbi))
            uSize = mbi.RegionSize;
        char szLine[MAX_PATH + 64];
        _snprintf(szLine, sizeof(szLine) - 1, "     %-20s | 0x%08X | %u\r\n",
                  pszName, (unsigned)(UINT_PTR)hMods[i], (unsigned)uSize);
        szLine[sizeof(szLine) - 1] = 0;
        CL_Write(szLine);
    }
}

//--- di nguoc ngan xep goi ham ---
static void CL_DumpStack(CONTEXT* pCtx)
{
    if (!s_pStackWalk64)
    {
        // khong co dbghelp: di theo chuoi EBP (chi dung khi co frame pointer)
        CL_Write("  -- ngan xep (theo chuoi EBP) --\r\n");
        DWORD* pFrame = (DWORD*)pCtx->Ebp;
        for (int i = 0; i < 24 && pFrame; i++)
        {
            if (IsBadReadPtr(pFrame, sizeof(DWORD) * 2))
                break;
            DWORD dwRet = pFrame[1];
            if (!dwRet)
                break;
            char szMod[64];
            DWORD dwRva = 0;
            CL_ModuleOf((void*)dwRet, szMod, sizeof(szMod), &dwRva);
            char szLine[192];
            _snprintf(szLine, sizeof(szLine) - 1, "     [%02d] 0x%08X  %s+0x%X\r\n",
                      i, (unsigned)dwRet, szMod[0] ? szMod : "?", (unsigned)dwRva);
            szLine[sizeof(szLine) - 1] = 0;
            CL_Write(szLine);
            pFrame = (DWORD*)pFrame[0];
        }
        return;
    }

    CL_Write("  -- ngan xep goi ham --\r\n");
    CL_STACKFRAME64 sf;
    memset(&sf, 0, sizeof(sf));
    sf.AddrPC.Offset    = pCtx->Eip;    sf.AddrPC.Mode    = 3;   // AddrModeFlat
    sf.AddrFrame.Offset = pCtx->Ebp;    sf.AddrFrame.Mode = 3;
    sf.AddrStack.Offset = pCtx->Esp;    sf.AddrStack.Mode = 3;

    for (int i = 0; i < 32; i++)
    {
        if (!s_pStackWalk64(0x014c /*IMAGE_FILE_MACHINE_I386*/, GetCurrentProcess(),
                            GetCurrentThread(), &sf, pCtx, NULL,
                            (PVOID)s_pSymFuncTable, (PVOID)s_pSymGetModBase, NULL))
            break;
        if (!sf.AddrPC.Offset)
            break;
        char szMod[64];
        DWORD dwRva = 0;
        CL_ModuleOf((void*)(UINT_PTR)sf.AddrPC.Offset, szMod, sizeof(szMod), &dwRva);
        char szLine[192];
        _snprintf(szLine, sizeof(szLine) - 1, "     [%02d] 0x%08X  %s+0x%X\r\n",
                  i, (unsigned)sf.AddrPC.Offset, szMod[0] ? szMod : "?", (unsigned)dwRva);
        szLine[sizeof(szLine) - 1] = 0;
        CL_Write(szLine);
    }
}

//---------------------------------------------------------------------------
static const char* CL_ExceptionName(DWORD dwCode)
{
    switch (dwCode)
    {
    case 0xC0000005: return "TRUY CAP BO NHO SAI (Access Violation)";
    case 0xC0000094: return "CHIA CHO 0 (so nguyen)";
    case 0xC0000095: return "TRAN SO NGUYEN";
    case 0xC00000FD: return "TRAN NGAN XEP (Stack Overflow)";
    case 0xC0000409: return "HONG NGAN XEP / kiem tra bao mat (Stack Buffer Overrun)";
    case 0xC0000374: return "HONG VUNG NHO DONG (Heap Corruption)";
    case 0xC000001D: return "LENH MAY KHONG HOP LE";
    case 0x80000003: return "BREAKPOINT";
    default:         return "khac";
    }
}

static LONG WINAPI CL_Filter(EXCEPTION_POINTERS* pEP)
{
    __try
    {
        SYSTEMTIME st;
        GetLocalTime(&st);

        char szHead[512];
        DWORD dwCode = pEP->ExceptionRecord->ExceptionCode;
        void* pAddr  = pEP->ExceptionRecord->ExceptionAddress;

        char  szMod[64];
        DWORD dwRva = 0;
        CL_ModuleOf(pAddr, szMod, sizeof(szMod), &dwRva);

        _snprintf(szHead, sizeof(szHead) - 1,
            "\r\n==================== GAME SAP ====================\r\n"
            "  Luc      : %04d-%02d-%02d %02d:%02d:%02d\r\n"
            "  Ma loi   : 0x%08X  (%s)\r\n"
            "  Tai      : 0x%08X  =>  %s + 0x%X\r\n",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
            (unsigned)dwCode, CL_ExceptionName(dwCode),
            (unsigned)(UINT_PTR)pAddr, szMod[0] ? szMod : "?", (unsigned)dwRva);
        szHead[sizeof(szHead) - 1] = 0;
        CL_Write(szHead);

        // Access Violation: doc/ghi vao dia chi nao
        if (dwCode == 0xC0000005 && pEP->ExceptionRecord->NumberParameters >= 2)
        {
            char szAv[160];
            ULONG_PTR uType = pEP->ExceptionRecord->ExceptionInformation[0];
            _snprintf(szAv, sizeof(szAv) - 1, "  Kieu     : %s dia chi 0x%08X\r\n",
                      uType == 0 ? "DOC" : (uType == 1 ? "GHI" : "CHAY MA TAI"),
                      (unsigned)pEP->ExceptionRecord->ExceptionInformation[1]);
            szAv[sizeof(szAv) - 1] = 0;
            CL_Write(szAv);
        }

        CONTEXT* pCtx = pEP->ContextRecord;
        char szReg[320];
        _snprintf(szReg, sizeof(szReg) - 1,
            "  Thanh ghi: EAX=%08X EBX=%08X ECX=%08X EDX=%08X\r\n"
            "             ESI=%08X EDI=%08X EBP=%08X ESP=%08X EIP=%08X\r\n",
            (unsigned)pCtx->Eax, (unsigned)pCtx->Ebx, (unsigned)pCtx->Ecx, (unsigned)pCtx->Edx,
            (unsigned)pCtx->Esi, (unsigned)pCtx->Edi, (unsigned)pCtx->Ebp,
            (unsigned)pCtx->Esp, (unsigned)pCtx->Eip);
        szReg[sizeof(szReg) - 1] = 0;
        CL_Write(szReg);

        // bo nho con trong bao nhieu - de biet co phai can bo nho khong
        MEMORYSTATUSEX ms;
        memset(&ms, 0, sizeof(ms));
        ms.dwLength = sizeof(ms);
        if (GlobalMemoryStatusEx(&ms))
        {
            char szMem[220];
            _snprintf(szMem, sizeof(szMem) - 1,
                "  Bo nho   : may dung %u%%, con trong %u MB; tien trinh dung dia chi ao %u MB\r\n",
                (unsigned)ms.dwMemoryLoad,
                (unsigned)(ms.ullAvailPhys / (1024 * 1024)),
                (unsigned)((ms.ullTotalVirtual - ms.ullAvailVirtual) / (1024 * 1024)));
            szMem[sizeof(szMem) - 1] = 0;
            CL_Write(szMem);
        }

        CL_DumpStack(pCtx);
        CL_DumpModules();
        CL_Write("==================================================\r\n");
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        // bo bat loi cung chet thi thoi, dung de treo them
    }

    if (s_pPrevFilter)
        return s_pPrevFilter(pEP);
    return EXCEPTION_EXECUTE_HANDLER;
}

//---------------------------------------------------------------------------
void CrashLog_Install()
{
    if (s_szLogPath[0])
        return;

    // dat log canh Game.exe
    char szExe[MAX_PATH];
    szExe[0] = 0;
    GetModuleFileNameA(NULL, szExe, MAX_PATH);
    char* p = strrchr(szExe, '\\');
    if (p)
        *(p + 1) = 0;
    _snprintf(s_szLogPath, sizeof(s_szLogPath) - 1, "%sjx_crash.log", szExe);
    s_szLogPath[sizeof(s_szLogPath) - 1] = 0;

    HMODULE hDbg = LoadLibraryA("dbghelp.dll");
    if (hDbg)
    {
        s_pSymInitialize = (PFN_SymInitialize)GetProcAddress(hDbg, "SymInitialize");
        s_pSymSetOptions = (PFN_SymSetOptions)GetProcAddress(hDbg, "SymSetOptions");
        s_pStackWalk64   = (PFN_StackWalk64)GetProcAddress(hDbg, "StackWalk64");
        s_pSymFuncTable  = (PFN_SymFunctionTableAccess64)GetProcAddress(hDbg, "SymFunctionTableAccess64");
        s_pSymGetModBase = (PFN_SymGetModuleBase64)GetProcAddress(hDbg, "SymGetModuleBase64");
        if (s_pSymSetOptions)
            s_pSymSetOptions(0x00000004 /*DEFERRED_LOADS*/ | 0x00000002 /*UNDNAME*/);
        if (s_pSymInitialize)
            s_pSymInitialize(GetCurrentProcess(), NULL, TRUE);
        if (!s_pStackWalk64 || !s_pSymFuncTable || !s_pSymGetModBase)
            s_pStackWalk64 = NULL;      // thieu ham thi quay ve cach EBP
    }

    s_pPrevFilter = SetUnhandledExceptionFilter(CL_Filter);

    SYSTEMTIME st;
    GetLocalTime(&st);
    char szOpen[200];
    _snprintf(szOpen, sizeof(szOpen) - 1,
        "\r\n[%04d-%02d-%02d %02d:%02d:%02d] --- game khoi dong, bo bat loi da bat (%s) ---\r\n",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
        s_pStackWalk64 ? "co dbghelp" : "khong co dbghelp, dung chuoi EBP");
    szOpen[sizeof(szOpen) - 1] = 0;
    CL_Write(szOpen);
}

//---------------------------------------------------------------------------
void CrashLog_Mark(const char* pszWhat, int nValue)
{
    if (!s_szLogPath[0] || !pszWhat)
        return;
    SYSTEMTIME st;
    GetLocalTime(&st);
    char szLine[256];
    _snprintf(szLine, sizeof(szLine) - 1, "[%02d:%02d:%02d] moc: %s = %d\r\n",
              st.wHour, st.wMinute, st.wSecond, pszWhat, nValue);
    szLine[sizeof(szLine) - 1] = 0;
    CL_Write(szLine);
}
