/*****************************************************************************************
//  图形到内存区域的操作
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-11
------------------------------------------------------------------------------------------
*****************************************************************************************/

#pragma once
#include "../../../Engine/Src/KWin32.h"

void RIO_Set16BitImageFormat(int b565);

void RIO_CopySprToBuffer(void* pSprite, int nSprWidth, int nSprHeight, void* pPalette,
					 void* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY);
void RIO_CopySprToBuffer32b(void* pSprite, int nSprWidth, int nSprHeight, void* pPalette,
		void* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY);
void RIO_CopySprToBufferAlpha(BYTE* pSprite, int nSprWidth, int nSprHeight, BYTE* pPalette,
		BYTE* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY, int nExAlpha = 255);
void RIO_CopySprToBufferAlpha32b(BYTE* pSprite, int nSprWidth, int nSprHeight, BYTE* pPalette,
		BYTE* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY, int nExAlpha = 255);
void RIO_CopySprToBuffer3LevelAlpha(void* pSpr, int nSprWidth, int nSprHeight, void* pPalette,
					 void* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY);

void RIO_CopyBitmap16ToBuffer(WORD* pBitmap, int nBmpWidth, int nBmpHeight,
		BYTE* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY, int nExAlpha = 255);
void RIO_CopyBitmap16ToBuffer32b(WORD* pBitmap, int nBmpWidth, int nBmpHeight,
		BYTE* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY, int nExAlpha = 255);
void RIO_CopySprToBufferBlendColor(BYTE* pSprite, int nSprWidth, int nSprHeight, BYTE* pPalette,
		BYTE* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY, int nExAlpha, UINT nColor, int nMode = 0);
void RIO_CopySprToBufferBlendColor32b(BYTE* pSprite, int nSprWidth, int nSprHeight, BYTE* pPalette,
		BYTE* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY, int nExAlpha, UINT nColor, int nMode = 0);
