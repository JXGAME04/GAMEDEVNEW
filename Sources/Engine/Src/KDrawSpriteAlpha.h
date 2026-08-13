//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KDrawSpriteAlpha.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KDrawSpriteAlpha_H
#define KDrawSpriteAlpha_H
//---------------------------------------------------------------------------
void g_DrawSpriteAlpha(void* node, void* canvas);
void g_DrawSpriteAlpha(void* node, void* canvas, int nExAlpha);
void g_DrawSpriteAlpha32b(void* node, void* canvas);
void g_DrawSpriteAlpha32b(void* node, void* canvas, int nExAlpha);
void g_DrawSprite3LevelAlpha(void* node, void* canvas);	//Èý¼¶alpha»æÖÆ
void g_DrawSpriteScreen(void* node, void* canvas, UINT nColor);
void g_DrawSpriteBlendColor(void* node, void* canvas, UINT nColor);
void g_DrawSpriteScreen32b(void* node, void* canvas, UINT nColor);
void g_DrawSpriteBlendColor32b(void* node, void* canvas, UINT nColor);
void g_DrawAlphaRecImage(void* node, void* canvas);
void g_DrawAlphaRecImageOpa(void* node, void* canvas);
void g_DrawAlphaRecImage32b(void* node, void* canvas);
void g_DrawOpaRecImage32b(void* node, void* canvas);

//DrawPrimitives by
void DrawSpriteScreenBlendMMX( 
						 BYTE byInputAlpha, 
						 DWORD dwMask32, 
						 void* pBuffer, 
						 INT width, 
						 INT height, 
						 INT nPitch, 
						 INT nX, 
						 INT nY, 
						 void* pPalette, 
						 void* pSprite, 
						 INT nWidth, 
						 INT nHeight, 
						 const RECT* pSrcRect );
BOOL RectIntersect(const RECT* pRect1, const RECT* pRect2, RECT* pOut);
void ScreenBlend4Pixel(void*  pDest, void*  pColor, void*  pAlpha );
//end;
// --
#endif
