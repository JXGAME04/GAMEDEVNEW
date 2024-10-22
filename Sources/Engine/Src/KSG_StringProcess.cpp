//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KSG_StringProcess.cpp
//  Version     :   1.0
//  Creater     :   Freeway Chen
//  Date        :   2003-8-1 12:45:11
//  Comment     :   Process String to Int, Skip Symbol
//
//////////////////////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include <ctype.h>
#include <time.h>
#include <math.h>

#include "KSG_StringProcess.h"

int KSG_GetCurSec()
{
	time_t curtime;
	time ( &curtime );
	return curtime - 1451581200;
}

#define BLOCKNUM 9
int KSG_StringSetValue(int t, int n, int p, int v)
{
    // Check if 'v' is within valid range
    if (v < 0 || v >= (int)pow(10, t)) return n;

    // Check if 'p' is a valid block position
    if (p < 1 || p > BLOCKNUM / t) return n;

    // Convert 'n' to a string and zero-pad it
    char str[BLOCKNUM + 1] = { 0 }; // Use +1 to ensure null-termination
    snprintf(str, BLOCKNUM + 1, "%09d", n); // Zero-padded string of 'n'

    // Convert 'v' to a zero-padded string
    char* str_v = (char*) malloc(t + 1); // String to hold the value of 'v'
    if (!str_v) {
        // If malloc fails, return the original number
        return n;
    }
    snprintf(str_v, t + 1, "%0*d", t, v); // Ensure 'v' is padded to 't' digits

    // Replace the appropriate block in 'str' with 'str_v'
    int pos = (p - 1) * t; // Calculate the starting position for replacement
    memcpy(str + pos, str_v, t); // Replace block in 'str'

    // Convert the modified string back to an integer
    int result = atoi(str);

    free(str_v);
    return result;
}

int KSG_StringGetValue(int t, int n, int p)
{
    // Validate the block position
    if (p < 1 || p > BLOCKNUM / t) return n;

    // Convert the number to a zero-padded string
    char str[BLOCKNUM + 1] = { 0 };  // +1 for null terminator
    snprintf(str, BLOCKNUM + 1, "%09d", n);  // Convert 'n' to zero-padded string

    // Calculate the position to extract from
    int pos = (p - 1) * t;

    // Extract 't' digits starting from position 'pos'
    char* str_block = (char*)malloc(t + 1);// +1 for null terminator
    if (!str_block) {
        // If malloc fails, return the original number
        return n;
    }
    strncpy(str_block, str + pos, t);  // Copy 't' digits into str_block

    // Convert extracted block back to integer
    int result = atoi(str_block);

    free(str_block);
    return result;
}

int KSG_StringGetInt(const char **ppcszString, int nDefaultValue)
{
    int nResult = false;
    int nRetValue = 0;
    int nNegSignFlag = false;
    int nRetValueValidFlag = false;
    const char *pcszString = NULL;

    if (!ppcszString)
        goto Exit0;
    
    pcszString = *ppcszString;

    if (!pcszString)
        goto Exit0;

    while (isspace(*pcszString))
        pcszString++;
    
    if ((*pcszString) == '\0')
        goto Exit0;

    if ((*pcszString) == '-')
    {
        nNegSignFlag = true;
        pcszString++;

        // Skip Prev Space
        while (isspace(*pcszString))
            pcszString++;

        if ((*pcszString) == '\0')
            goto Exit0;
    }

    
    while (isdigit(*pcszString))
    {
        nRetValueValidFlag = true;

        nRetValue = nRetValue * 10 +  ((int)(*pcszString - '0'));

        pcszString++;
    }

    nResult = true;
Exit0:

    if (pcszString)
    {
        if (ppcszString)
            *ppcszString = pcszString;
    }

    if (nNegSignFlag)
        nRetValue = -nRetValue;

    if (
        (!nResult) ||
        (!nRetValueValidFlag)
    )
        nRetValue = nDefaultValue; 


    return nRetValue;
}


bool KSG_StringSkipSymbol(const char **ppcszString, int nSymbol)
{
    bool bResult = false;
    const char *pcszString = NULL;

    if (!ppcszString)
        goto Exit0;
    
    pcszString = *ppcszString;

    if (!pcszString)
        goto Exit0;

    while (isspace(*pcszString))
        pcszString++;
    
    if (((unsigned)(*pcszString)) != (unsigned)nSymbol)
        goto Exit0;

    pcszString++;   // Skip Symbol

    bResult = true;
Exit0:

    if (pcszString)
    {
        if (ppcszString)
            *ppcszString = pcszString;
    }

    return bResult;
}

