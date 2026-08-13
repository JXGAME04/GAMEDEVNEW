// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT 0x0400		//add by tuanln fix dump

#include <windows.h>
#include <conio.h>

#pragma warning(disable : 4786)  // identifier was truncated to '255' characters // in the debug information                                 
#pragma warning(disable : 4305 )
#pragma warning(disable : 4309 )
// Add at the top of the file, after includes
#include <fstream>
#include <mutex>

// Global log file and mutex
static std::ofstream g_logFile("GamePlayer.log", std::ios::app);
static std::mutex g_logMutex;

// Helper function to get timestamp string
static std::string GetCurrentTimestamp()
{
    time_t now = time(nullptr);
    struct tm t;
#if defined(_WIN32)
    localtime_s(&t, &now);
#else
    localtime_r(&now, &t);
#endif
    char buf[32];
    strftime(buf, sizeof(buf), "[%Y/%m/%d %H:%M:%S]", &t);
    return std::string(buf);
}

// Redefine cprintf to also log to file with timestamp
#ifdef cprintf
#undef cprintf
#endif
#define cprintf(fmt, ...) \
    do { \
        printf(fmt, ##__VA_ARGS__); \
        std::lock_guard<std::mutex> lock(g_logMutex); \
        if (g_logFile.is_open()) { \
            g_logFile << GetCurrentTimestamp() << " "; \
            char _logbuf[1024]; \
            snprintf(_logbuf, sizeof(_logbuf), fmt, ##__VA_ARGS__); \
            g_logFile << _logbuf; \
            if (_logbuf[strlen(_logbuf)-1] != '\n') g_logFile << std::endl; \
            g_logFile.flush(); \
        } \
    } while(0)
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
