//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KColors.cpp
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	Colors Functions
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KDDraw.h"
#include "KColors.h"
//---------------------------------------------------------------------------
ENGINE_API WORD(*g_RGB)(int nRed, int nGreen, int nBlue) = g_RGB565;
//---------------------------------------------------------------------------
// Function: Red
// Function: Returns the red component of a color value
// Parameter: wColor color value
// Return: red component
//---------------------------------------------------------------------------
ENGINE_API BYTE g_Red(WORD wColor)
{
#ifdef _WIN64
	return static_cast<BYTE>((wColor >> 1) & 0x1F);
#else
	BYTE Red;
	__asm
	{
		mov		ax, wColor
		and ax, 0x0031
		mov		Red, al
	}
	return Red;
#endif
}
//---------------------------------------------------------------------------
// Function: Green
// Function: Returns the green component of a color value
// Parameter: wColor color value
// Return: green component
//---------------------------------------------------------------------------
ENGINE_API BYTE g_Green(WORD wColor)
{
#ifdef _WIN64
	long Mask16 = g_pDirectDraw->GetRGBBitMask16();
	// Extract the green component by masking and shifting.
	if (Mask16 == 0xFFFF)
		return static_cast<BYTE>((wColor >> 5) & 0x07);
	else
		return static_cast<BYTE>((wColor >> 5) & 0x01);
#else
	long Mask16 = g_pDirectDraw->GetRGBBitMask16();
	BYTE Green;
	__asm
	{
		mov		ax, wColor
		shr		ax, 5
		mov		edx, Mask16
		cmp		edx, 0xffff
		je		loc_Green_0001
		and ax, 0x0031

		loc_Green_0001:

		and ax, 0x0071
			mov		Green, al
	}
	return Green;
#endif
}
//---------------------------------------------------------------------------
// Function: Blue
// Function: Returns the blue component of a color value
// Parameter: wColor color value
// Return: blue component
//---------------------------------------------------------------------------
ENGINE_API BYTE g_Blue(WORD wColor)
{
#ifdef _WIN64
	long Mask16 = g_pDirectDraw->GetRGBBitMask16();
	// Extract the blue component by masking and shifting.
	if (Mask16 == 0xFFFF)
		return static_cast<BYTE>((wColor >> 10) & 0x1F);
	else
		return static_cast<BYTE>((wColor >> 1) & 0x1F);
#else
	long Mask16 = g_pDirectDraw->GetRGBBitMask16();
	BYTE Blue;
	__asm
	{
		mov		ax, wColor
		shr		ax, 10
		mov		edx, Mask16
		cmp		edx, 0xffff
		jne		loc_Blue_0001
		shr		ax, 1

		loc_Blue_0001:

		and ax, 0x0031
			mov		Blue, al
	}
	return Blue;
#endif
}
//---------------------------------------------------------------------------
// Function: RGB
// Function: Return a DWORD color value
// Parameters: red red component
// green green component
// blue blue component
// Return: color value
//---------------------------------------------------------------------------
ENGINE_API WORD g_RGB555(int nRed, int nGreen, int nBlue)
{
#ifdef _WIN64
	// Create a 16-bit color value by combining the red, green, and blue components.
	return static_cast<WORD>(((nRed & 0x1F) << 10) | ((nGreen & 0x1F) << 5) | (nBlue & 0x1F));
#else
	WORD wColor;
	__asm
	{
		xor ecx, ecx
		mov		ebx, 0xff
		mov		eax, nRed
		and eax, ebx
		shr		eax, 3
		shl		eax, 10
		or ecx, eax
		mov		eax, nGreen
		and eax, ebx
		shr		eax, 3
		shl		eax, 5
		or ecx, eax
		mov		eax, nBlue
		and eax, ebx
		shr		eax, 3
		or ecx, eax
		mov		wColor, cx
	}
	return wColor;
#endif
}
//---------------------------------------------------------------------------
// Function: RGB
// Function: Return a DWORD color value
// Parameters: red red component
// green green component
// blue blue component
// Return: color value
//---------------------------------------------------------------------------
ENGINE_API WORD g_RGB565(int nRed, int nGreen, int nBlue)
{
#ifdef _WIN64
	// Create a 16-bit color value by combining the red, green, and blue components.
	return static_cast<WORD>(((nRed & 0x1F) << 11) | ((nGreen & 0x3F) << 5) | (nBlue & 0x1F));
#else
	WORD wColor;
	__asm
	{
		xor ecx, ecx
		mov		ebx, 0xff
		mov		eax, nRed
		and eax, ebx
		shr		eax, 3
		shl		eax, 11
		or ecx, eax
		mov		eax, nGreen
		and eax, ebx
		shr		eax, 2
		shl		eax, 5
		or ecx, eax
		mov		eax, nBlue
		and eax, ebx
		shr		eax, 3
		or ecx, eax
		mov		wColor, cx
	}
	return wColor;
#endif
}
//---------------------------------------------------------------------------
// Function: 555 To 565
// Function: Convert 555 format to 565 format
// Parameter: nWidth width
// nHeight height
// lpBitmap bitmap
// Return: void
//---------------------------------------------------------------------------
void g_555To565(int nWidth, int nHeight, void* lpBitmap)
{
#ifdef _WIN64
	WORD* pBitmap = static_cast<WORD*>(lpBitmap);

	for (int y = 0; y < nHeight; y++)
	{
		for (int x = 0; x < nWidth; x++)
		{
			// Convert 555 to 565 format.
			WORD color555 = *pBitmap;
			WORD color565 = g_RGB565(g_Red(color555), g_Green(color555), g_Blue(color555));
			*pBitmap = color565;
			pBitmap++;
		}
	}
#else
	__asm
	{
		mov		esi, lpBitmap
		mov		edx, nHeight

		loc_555to565_loop1 :
		mov		ecx, nWidth

			loc_555to565_loop2 :
		mov		ax, [esi]
			mov		bx, ax
			shr		ax, 5
			shl		ax, 6
			and bx, 0x001f
			or ax, bx
			mov[esi], ax
			add		esi, 2
			dec		ecx
			jnz		loc_555to565_loop2
			dec		edx
			jnz		loc_555to565_loop1
	}
#endif
}
//---------------------------------------------------------------------------
// Function: 565 To 555
// Function: Convert 565 format to 555 format
// Parameter: nWidth width
// nHeight height
// lpBitmap bitmap
// Return: void
//---------------------------------------------------------------------------
void g_565To555(int nWidth, int nHeight, void* lpBitmap)
{
#ifdef _WIN64
	WORD* pBitmap = static_cast<WORD*>(lpBitmap);

	for (int y = 0; y < nHeight; y++)
	{
		for (int x = 0; x < nWidth; x++)
		{
			// Convert 565 to 555 format.
			WORD color565 = *pBitmap;
			WORD color555 = g_RGB555(g_Red(color565), g_Green(color565), g_Blue(color565));
			*pBitmap = color555;
			pBitmap++;
		}
	}
#else
	__asm
	{
		mov		esi, lpBitmap
		mov		edx, nHeight

		loc_565to555_loop1 :
		mov		ecx, nWidth

			loc_565to555_loop2 :
		mov		ax, [esi]
			mov		bx, ax
			shr		ax, 6
			shl		ax, 5
			and bx, 0x001f
			or ax, bx
			mov[esi], ax
			add		esi, 2
			dec		ecx
			jnz		loc_565to555_loop2
			dec		edx
			jnz		loc_565to555_loop1
	}
#endif
}
//---------------------------------------------------------------------------