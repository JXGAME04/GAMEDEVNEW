//---------------------------------------------------------------------------
//  CrashLog.cpp - bat loi sap game, ghi jx_crash.log
//
//  Nguyen tac: luc bat duoc thi tien trinh DA HONG, nen
//    - KHONG cap phat bo nho, chi dung bo dem tren stack
//    - Moi thu boc trong __try/__except de khong chet lan hai
//
//  Bat NHIEU LOP vi mot bo loc khong du:
//    - Vectored handler  : chay TRUOC moi bo xu ly SEH khac, khong DLL nao chan duoc
//    - UnhandledFilter   : luoi cuoi
//    - signal/abort      : duong chet cua CRT (Debug Assertion, Runtime Check)
//    - invalid parameter : CRT nhan tham so hong
//
//  dbghelp va AddVectoredExceptionHandler nap DONG => khong them thu vien lien ket.
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "CrashLog.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <dbghelp.h>

typedef BOOL    (WINAPI *PFN_SymInitialize)(HANDLE, PCSTR, BOOL);
typedef DWORD   (WINAPI *PFN_SymSetOptions)(DWORD);
typedef BOOL    (WINAPI *PFN_StackWalk64)(DWORD, HANDLE, HANDLE, LPSTACKFRAME64, PVOID,
                                          PVOID, PVOID, PVOID, PVOID);
typedef PVOID   (WINAPI *PFN_SymFunctionTableAccess64)(HANDLE, DWORD64);
typedef DWORD64 (WINAPI *PFN_SymGetModuleBase64)(HANDLE, DWORD64);
typedef PVOID   (WINAPI *PFN_AddVEH)(ULONG, PVOID);
typedef VOID    (WINAPI *PFN_RtlCaptureContext)(PCONTEXT);
typedef BOOL    (WINAPI *PFN_SymFromAddr)(HANDLE, DWORD64, PDWORD64, PSYMBOL_INFO);
typedef BOOL    (WINAPI *PFN_SymGetLineFromAddr64)(HANDLE, DWORD64, PDWORD, PIMAGEHLP_LINE64);
typedef BOOL    (WINAPI *PFN_SymRefreshModuleList)(HANDLE);

static PFN_SymInitialize            s_pSymInitialize = NULL;
static PFN_SymSetOptions            s_pSymSetOptions = NULL;
static PFN_StackWalk64              s_pStackWalk64   = NULL;
static PFN_SymFunctionTableAccess64 s_pSymFuncTable  = NULL;
static PFN_SymGetModuleBase64       s_pSymGetModBase = NULL;
static PFN_SymFromAddr              s_pSymFromAddr   = NULL;
static PFN_SymGetLineFromAddr64     s_pSymGetLine    = NULL;
static PFN_SymRefreshModuleList     s_pSymRefresh    = NULL;

static char s_szLogPath[MAX_PATH] = {0};
static LPTOP_LEVEL_EXCEPTION_FILTER s_pPrevFilter = NULL;
static LONG s_lReporting = 0;

static const char CL_LINE_END[] = { 13, 10, 0 };

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

static void CL_WriteLine(const char* pszText)
{
    CL_Write(pszText);
    CL_Write(CL_LINE_END);
}

//--- tim module chua dia chi -> ten + do lech ---
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
        const char* p = strrchr(szFull, 92);
        _snprintf(szName, nNameSize - 1, "%s", p ? p + 1 : szFull);
        szName[nNameSize - 1] = 0;
    }
    *pdwRva = (DWORD)((BYTE*)pAddr - (BYTE*)hMod);
}


//--- dia chi -> ten ham + tep nguon + dong (neu co PDB) ---
static void CL_SymbolOf(DWORD64 dwAddr, char* szOut, int nOutSize)
{
    szOut[0] = 0;
    if (!s_pSymFromAddr)
        return;
    __try
    {
        BYTE  bufSym[sizeof(SYMBOL_INFO) + 512];
        memset(bufSym, 0, sizeof(bufSym));
        PSYMBOL_INFO pSym    = (PSYMBOL_INFO)bufSym;
        pSym->SizeOfStruct   = sizeof(SYMBOL_INFO);
        pSym->MaxNameLen     = 500;
        DWORD64 dwDisp = 0;
        if (!s_pSymFromAddr(GetCurrentProcess(), dwAddr, &dwDisp, pSym))
            return;

        char szLine[128];
        szLine[0] = 0;
        if (s_pSymGetLine)
        {
            IMAGEHLP_LINE64 il;
            memset(&il, 0, sizeof(il));
            il.SizeOfStruct = sizeof(il);
            DWORD dwLineDisp = 0;
            if (s_pSymGetLine(GetCurrentProcess(), dwAddr, &dwLineDisp, &il) && il.FileName)
            {
                const char* pFile = strrchr(il.FileName, 92);
                _snprintf(szLine, sizeof(szLine) - 1, "   [%s dong %u]",
                          pFile ? pFile + 1 : il.FileName, (unsigned)il.LineNumber);
                szLine[sizeof(szLine) - 1] = 0;
            }
        }
        _snprintf(szOut, nOutSize - 1, "  ->  %s + 0x%X%s",
                  pSym->Name, (unsigned)dwDisp, szLine);
        szOut[nOutSize - 1] = 0;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        szOut[0] = 0;
    }
}

//--- danh sach module cua game ---
static void CL_DumpModules()
{
    typedef BOOL (WINAPI *PFN_EnumProcessModules)(HANDLE, HMODULE*, DWORD, LPDWORD);
    HMODULE hPsapi = LoadLibraryA("psapi.dll");
    if (!hPsapi)
        return;
    PFN_EnumProcessModules pfn =
        (PFN_EnumProcessModules)GetProcAddress(hPsapi, "EnumProcessModules");
    if (!pfn)
        return;

    HMODULE hMods[256];
    DWORD   dwNeeded = 0;
    if (!pfn(GetCurrentProcess(), hMods, sizeof(hMods), &dwNeeded))
        return;

    CL_WriteLine("  -- module cua game (ten | dia chi nap) --");
    int nCount = (int)(dwNeeded / sizeof(HMODULE));
    if (nCount > 256)
        nCount = 256;
    for (int i = 0; i < nCount; i++)
    {
        char szFull[MAX_PATH];
        szFull[0] = 0;
        if (!GetModuleFileNameA(hMods[i], szFull, MAX_PATH))
            continue;
        const char* p = strrchr(szFull, 92);
        const char* pszName = p ? p + 1 : szFull;
        if (!strstr(pszName, ".exe") && !strstr(pszName, "Core") && !strstr(pszName, "core")
            && !strstr(pszName, "ngine") && !strstr(pszName, "epresent")
            && !strstr(pszName, "ualib") && !strstr(pszName, "xpand"))
            continue;
        char szLine[MAX_PATH + 64];
        _snprintf(szLine, sizeof(szLine) - 1, "     %-20s | 0x%08X",
                  pszName, (unsigned)(UINT_PTR)hMods[i]);
        szLine[sizeof(szLine) - 1] = 0;
        CL_WriteLine(szLine);
    }
}

//--- di nguoc ngan xep ---
static void CL_DumpStack(CONTEXT* pCtx)
{
    char  szLine[640];
    char  szMod[64];
    char  szSym[420];
    DWORD dwRva = 0;

    if (!s_pStackWalk64)
    {
        CL_WriteLine("  -- ngan xep (theo chuoi EBP) --");
        DWORD* pFrame = (DWORD*)pCtx->Ebp;
        for (int i = 0; i < 24 && pFrame; i++)
        {
            if (IsBadReadPtr(pFrame, sizeof(DWORD) * 2))
                break;
            DWORD dwRet = pFrame[1];
            if (!dwRet)
                break;
            CL_ModuleOf((void*)dwRet, szMod, sizeof(szMod), &dwRva);
            CL_SymbolOf((DWORD64)dwRet, szSym, sizeof(szSym));
            _snprintf(szLine, sizeof(szLine) - 1, "     [%02d] 0x%08X  %s+0x%X%s",
                      i, (unsigned)dwRet, szMod[0] ? szMod : "?", (unsigned)dwRva, szSym);
            szLine[sizeof(szLine) - 1] = 0;
            CL_WriteLine(szLine);
            pFrame = (DWORD*)pFrame[0];
        }
        return;
    }

    CL_WriteLine("  -- ngan xep goi ham --");
    STACKFRAME64 sf;
    memset(&sf, 0, sizeof(sf));
    sf.AddrPC.Offset    = pCtx->Eip;  sf.AddrPC.Mode    = AddrModeFlat;
    sf.AddrFrame.Offset = pCtx->Ebp;  sf.AddrFrame.Mode = AddrModeFlat;
    sf.AddrStack.Offset = pCtx->Esp;  sf.AddrStack.Mode = AddrModeFlat;

    for (int i = 0; i < 32; i++)
    {
        if (!s_pStackWalk64(0x014c, GetCurrentProcess(), GetCurrentThread(),
                            &sf, pCtx, NULL,
                            (PVOID)s_pSymFuncTable, (PVOID)s_pSymGetModBase, NULL))
            break;
        if (!sf.AddrPC.Offset)
            break;
        CL_ModuleOf((void*)(UINT_PTR)sf.AddrPC.Offset, szMod, sizeof(szMod), &dwRva);
        CL_SymbolOf(sf.AddrPC.Offset, szSym, sizeof(szSym));
        _snprintf(szLine, sizeof(szLine) - 1, "     [%02d] 0x%08X  %s+0x%X%s",
                  i, (unsigned)sf.AddrPC.Offset, szMod[0] ? szMod : "?", (unsigned)dwRva, szSym);
        szLine[sizeof(szLine) - 1] = 0;
        CL_WriteLine(szLine);
    }
}

//---------------------------------------------------------------------------
static const char* CL_ExceptionName(DWORD dwCode)
{
    switch (dwCode)
    {
    case 0xC0000005: return "TRUY CAP BO NHO SAI (Access Violation)";
    case 0xC0000094: return "CHIA CHO 0";
    case 0xC0000095: return "TRAN SO NGUYEN";
    case 0xC00000FD: return "TRAN NGAN XEP";
    case 0xC0000409: return "HONG NGAN XEP (Stack Buffer Overrun)";
    case 0xC0000374: return "HONG VUNG NHO DONG (Heap Corruption)";
    case 0xC000001D: return "LENH MAY KHONG HOP LE";
    case 0xE06D7363: return "NGOAI LE C++ khong ai bat";
    default:         return "khac";
    }
}

//--- bao cao day du, dung chung cho moi duong bat loi ---
static void CL_Report(const char* pszWhy, EXCEPTION_POINTERS* pEP)
{
    if (InterlockedExchange(&s_lReporting, 1) != 0)
        return;

    __try
    {
        SYSTEMTIME st;
        GetLocalTime(&st);

        char szLine[900];
        CL_Write(CL_LINE_END);
        CL_WriteLine("==================== GAME SAP ====================");
        _snprintf(szLine, sizeof(szLine) - 1, "  Luc      : %04d-%02d-%02d %02d:%02d:%02d",
                  st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        szLine[sizeof(szLine) - 1] = 0;
        CL_WriteLine(szLine);

        _snprintf(szLine, sizeof(szLine) - 1, "  Duong    : %s", pszWhy);
        szLine[sizeof(szLine) - 1] = 0;
        CL_WriteLine(szLine);

        if (pEP && pEP->ExceptionRecord)
        {
            DWORD dwCode = pEP->ExceptionRecord->ExceptionCode;
            void* pAddr  = pEP->ExceptionRecord->ExceptionAddress;
            char  szMod[64];
            DWORD dwRva = 0;
            CL_ModuleOf(pAddr, szMod, sizeof(szMod), &dwRva);

            _snprintf(szLine, sizeof(szLine) - 1, "  Ma loi   : 0x%08X  (%s)",
                      (unsigned)dwCode, CL_ExceptionName(dwCode));
            szLine[sizeof(szLine) - 1] = 0;
            CL_WriteLine(szLine);

            char szSymFault[420];
            CL_SymbolOf((DWORD64)(UINT_PTR)pAddr, szSymFault, sizeof(szSymFault));
            _snprintf(szLine, sizeof(szLine) - 1, "  Tai      : 0x%08X  =>  %s + 0x%X%s",
                      (unsigned)(UINT_PTR)pAddr, szMod[0] ? szMod : "?", (unsigned)dwRva,
                      szSymFault);
            szLine[sizeof(szLine) - 1] = 0;
            CL_WriteLine(szLine);

            if (dwCode == 0xC0000005 && pEP->ExceptionRecord->NumberParameters >= 2)
            {
                ULONG_PTR uType = pEP->ExceptionRecord->ExceptionInformation[0];
                _snprintf(szLine, sizeof(szLine) - 1, "  Kieu     : %s dia chi 0x%08X",
                          uType == 0 ? "DOC" : (uType == 1 ? "GHI" : "CHAY MA TAI"),
                          (unsigned)pEP->ExceptionRecord->ExceptionInformation[1]);
                szLine[sizeof(szLine) - 1] = 0;
                CL_WriteLine(szLine);
            }
        }

        MEMORYSTATUSEX ms;
        memset(&ms, 0, sizeof(ms));
        ms.dwLength = sizeof(ms);
        if (GlobalMemoryStatusEx(&ms))
        {
            _snprintf(szLine, sizeof(szLine) - 1,
                      "  Bo nho   : may dung %u%%, con trong %u MB; game dung dia chi ao %u MB (tran 32-bit ~2048)",
                      (unsigned)ms.dwMemoryLoad,
                      (unsigned)(ms.ullAvailPhys / (1024 * 1024)),
                      (unsigned)((ms.ullTotalVirtual - ms.ullAvailVirtual) / (1024 * 1024)));
            szLine[sizeof(szLine) - 1] = 0;
            CL_WriteLine(szLine);
        }

        CONTEXT  ctxLocal;
        CONTEXT* pCtx = (pEP && pEP->ContextRecord) ? pEP->ContextRecord : NULL;
        if (!pCtx)
        {
            memset(&ctxLocal, 0, sizeof(ctxLocal));
            ctxLocal.ContextFlags = CONTEXT_FULL;
            HMODULE hNt = GetModuleHandleA("kernel32.dll");
            PFN_RtlCaptureContext pfnCap = hNt ?
                (PFN_RtlCaptureContext)GetProcAddress(hNt, "RtlCaptureContext") : NULL;
            if (pfnCap)
                pfnCap(&ctxLocal);
            pCtx = &ctxLocal;
        }

        _snprintf(szLine, sizeof(szLine) - 1,
                  "  Thanh ghi: EAX=%08X EBX=%08X ECX=%08X EDX=%08X",
                  (unsigned)pCtx->Eax, (unsigned)pCtx->Ebx,
                  (unsigned)pCtx->Ecx, (unsigned)pCtx->Edx);
        szLine[sizeof(szLine) - 1] = 0;
        CL_WriteLine(szLine);

        _snprintf(szLine, sizeof(szLine) - 1,
                  "             ESI=%08X EDI=%08X EBP=%08X ESP=%08X EIP=%08X",
                  (unsigned)pCtx->Esi, (unsigned)pCtx->Edi,
                  (unsigned)pCtx->Ebp, (unsigned)pCtx->Esp, (unsigned)pCtx->Eip);
        szLine[sizeof(szLine) - 1] = 0;
        CL_WriteLine(szLine);

        if (s_pSymRefresh)
            s_pSymRefresh(GetCurrentProcess());

        CL_DumpStack(pCtx);
        CL_DumpModules();
        CL_WriteLine("==================================================");
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
    }

    InterlockedExchange(&s_lReporting, 0);
}

//--- chay TRUOC moi bo xu ly SEH khac ---
static LONG WINAPI CL_Vectored(EXCEPTION_POINTERS* pEP)
{
    DWORD c = pEP->ExceptionRecord->ExceptionCode;
    if (c == 0xC0000005 || c == 0xC0000374 || c == 0xC0000409 ||
        c == 0xC00000FD || c == 0xC000001D || c == 0xC0000094)
    {
        CL_Report("bat som (vectored) - loi nghiem trong", pEP);
    }
    return 0;
}

//--- luoi cuoi ---
static LONG WINAPI CL_Filter(EXCEPTION_POINTERS* pEP)
{
    CL_Report("khong ai xu ly (unhandled)", pEP);
    if (s_pPrevFilter)
        return s_pPrevFilter(pEP);
    return EXCEPTION_EXECUTE_HANDLER;
}

//--- duong chet cua CRT ---
static void CL_OnSignal(int nSig)
{
    const char* psz = "tin hieu CRT";
    if (nSig == SIGSEGV)      psz = "tin hieu SIGSEGV";
    else if (nSig == SIGILL)  psz = "tin hieu SIGILL";
    else if (nSig == SIGFPE)  psz = "tin hieu SIGFPE";
    else if (nSig == SIGABRT) psz = "CRT abort() - thuong la Debug Assertion / Runtime Check";
    CL_Report(psz, NULL);
}

static void __cdecl CL_OnBadParam(const wchar_t*, const wchar_t*, const wchar_t*,
                                  unsigned int, uintptr_t)
{
    CL_Report("CRT: tham so khong hop le", NULL);
}

//---------------------------------------------------------------------------
void CrashLog_Install()
{
    if (s_szLogPath[0])
        return;

    char szExe[MAX_PATH];
    szExe[0] = 0;
    GetModuleFileNameA(NULL, szExe, MAX_PATH);
    char* p = strrchr(szExe, 92);
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
        s_pSymFromAddr   = (PFN_SymFromAddr)GetProcAddress(hDbg, "SymFromAddr");
        s_pSymGetLine    = (PFN_SymGetLineFromAddr64)GetProcAddress(hDbg, "SymGetLineFromAddr64");
        s_pSymRefresh    = (PFN_SymRefreshModuleList)GetProcAddress(hDbg, "SymRefreshModuleList");
        if (s_pSymSetOptions)
            s_pSymSetOptions(0x00000006 | 0x00000200);  // UNDNAME|DEFERRED|LOAD_LINES
        if (s_pSymInitialize)
            s_pSymInitialize(GetCurrentProcess(), NULL, TRUE);
        if (!s_pStackWalk64 || !s_pSymFuncTable || !s_pSymGetModBase)
            s_pStackWalk64 = NULL;
    }

    s_pPrevFilter = SetUnhandledExceptionFilter(CL_Filter);

    HMODULE hK32 = GetModuleHandleA("kernel32.dll");
    PFN_AddVEH pfnAdd = hK32 ?
        (PFN_AddVEH)GetProcAddress(hK32, "AddVectoredExceptionHandler") : NULL;
    if (pfnAdd)
        pfnAdd(1, (PVOID)CL_Vectored);

    signal(SIGABRT, CL_OnSignal);
    signal(SIGSEGV, CL_OnSignal);
    signal(SIGILL,  CL_OnSignal);
    signal(SIGFPE,  CL_OnSignal);
    _set_abort_behavior(0, 3);
    _set_invalid_parameter_handler(CL_OnBadParam);

    SYSTEMTIME st;
    GetLocalTime(&st);
    char szOpen[256];
    _snprintf(szOpen, sizeof(szOpen) - 1,
              "[%04d-%02d-%02d %02d:%02d:%02d] --- game khoi dong, bo bat loi da bat (%s, %s) ---",
              st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
              s_pSymFromAddr ? "co dbghelp+ten ham" : (s_pStackWalk64 ? "co dbghelp" : "khong dbghelp"),
              pfnAdd ? "co vectored" : "khong vectored");
    szOpen[sizeof(szOpen) - 1] = 0;
    CL_Write(CL_LINE_END);
    CL_WriteLine(szOpen);
}

//---------------------------------------------------------------------------
void CrashLog_Mark(const char* pszWhat, int nValue)
{
    if (!s_szLogPath[0] || !pszWhat)
        return;
    SYSTEMTIME st;
    GetLocalTime(&st);
    char szLine[256];
    _snprintf(szLine, sizeof(szLine) - 1, "[%02d:%02d:%02d] moc: %s = %d",
              st.wHour, st.wMinute, st.wSecond, pszWhat, nValue);
    szLine[sizeof(szLine) - 1] = 0;
    CL_WriteLine(szLine);
}
