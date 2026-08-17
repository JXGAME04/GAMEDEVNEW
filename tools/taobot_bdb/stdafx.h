// stdafx.h cho taobot_bdb - giu toi thieu, dung khuon cua Goddess/StdAfx.h
// (DBTable.cpp include "stdafx.h" nen bat buoc phai co mot cai trong duong include).
#pragma once

#define WIN32_LEAN_AND_MEAN
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

#include <stdio.h>
#include <windows.h>

#pragma warning( disable : 4786 )
#pragma warning( disable : 4305 )
#pragma warning( disable : 4996 )   // _snprintf/strncpy "khong an toan" - giu API cu cho khop Goddess
