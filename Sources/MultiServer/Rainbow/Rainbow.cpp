// Rainbow.cpp : Defines the entry point for the DLL application.
//

#ifdef _WIN32	// [SDL 08/09 2b-2] DllMain chi tren Windows
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

#pragma message( "* ATTENTION : PLEASE USE IT IN ANSI SETTING *" )

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#endif	// _WIN32 [SDL 08/09 2b-2]
