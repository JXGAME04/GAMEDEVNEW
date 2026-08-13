//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KDrawBitmap16.cpp
// Date:	2000.08.08
// Code:	Daniel Wang, Wooy(Wu yue)
// Desc:	Bitmap Drawing Functions
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KCanvas.h"
#include "KDrawBitmap16.h"

void g_DrawBitmap16(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode*)node;
	KCanvas* pCanvas = (KCanvas*)canvas;
	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (!pCanvas->MakeClip(pNode->m_nX, pNode->m_nY, pNode->m_nWidth, pNode->m_nHeight, &Clipper))
		return;

	// pBuffer指向屏幕绘制行的头一个像点处

	int nPitch;
	void* pBuffer = pCanvas->LockCanvas(nPitch);
	if (pBuffer == NULL)
		return;
	pBuffer = (char*)(pBuffer)+Clipper.y * nPitch;
	void* pBitmap = (char*)pNode->m_pBitmap;
	int   nWidth = pNode->m_nWidth;

	//计算缓冲区下一行的偏移
	long nNextLine = nPitch - Clipper.width * 2;
	long nBitmapOffset;

	// 绘制函数的汇编代码
#ifdef _WIN64
#else
	__asm
	{
		//使edi指向canvas绘制起点
		mov		edi, pBuffer
		mov		eax, Clipper.x
		add		eax, eax
		add		edi, eax

		//esi指向图块数据绘制的开始位置
		mov		esi, pBitmap
		mov		eax, Clipper.top
		mul		nWidth
		add		eax, Clipper.left
		add		eax, eax
		add		esi, eax

		// 计算位图下一行的偏移
		mov		eax, nWidth
		sub		eax, Clipper.width
		add		eax, eax

		mov		ebx, Clipper.height
		mov		edx, Clipper.width
		mov		ecx, edi
		sub		ecx, esi
		test	ecx, 2
		jz		_4BYTE_ALIGN_COPY_

		//_2BYTE_ALIGN_COPY_:
		{
			_2BYTE_ALIGN_COPY_LINE_:
			{
				mov		ecx, edx
				rep		movsw
				add		edi, nNextLine
				add     esi, eax
				dec		ebx
				jne		_2BYTE_ALIGN_COPY_LINE_
				jmp		_EXIT_WAY_
			}
		}

	_4BYTE_ALIGN_COPY_:
		{
			mov		nBitmapOffset, eax
				_4BYTE_ALIGN_COPY_LINE_ :
			{
				mov		eax, edx
					mov		ecx, edi
					shr		ecx, 1
					and ecx, 1
					sub		eax, ecx
					rep		movsw
					mov		ecx, eax
					shr		ecx, 1
					rep		movsd
					adc		ecx, ecx
					rep		movsw
					add		edi, nNextLine
					add     esi, nBitmapOffset
					dec		ebx
					jne		_4BYTE_ALIGN_COPY_LINE_
					jmp		_EXIT_WAY_
			}
		}
	_EXIT_WAY_:
	}
	pCanvas->UnlockCanvas();
#endif
}

//---------------------------------------------------------------------------
// 函数:	DrawBitmap16
// 功能:	绘制16位色位图
// 参数:	node, canvas
// 返回:	void
//---------------------------------------------------------------------------
/*void g_DrawBitmap16_OLD(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;

	long nX = pNode->m_nX;// x coord
	long nY = pNode->m_nY;// y coord
	long nWidth = pNode->m_nWidth;// width of sprite
	long nHeight = pNode->m_nHeight;// height of sprite
	void* lpBitmap = pNode->m_pBitmap;// bitmap pointer

	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (!pCanvas->MakeClip(nX, nY, nWidth, nHeight, &Clipper))
		return;

	int nPitch;
	void* lpBuffer = pCanvas->LockCanvas(nPitch);
	if (lpBuffer == NULL)
		return;

	// 计算屏幕下一行的偏移
	long ScreenOffset = nPitch - Clipper.width * 2;

	// 计算位图下一行的偏移
	long BitmapOffset = (nWidth - Clipper.width) * 2;

	// 绘制函数的汇编代码
	__asm
	{
//---------------------------------------------------------------------------
//  计算 EDI 指向屏幕起点的偏移量 (以字节计)
//  edi = (nPitch*Clipper.y + nX)*2 + lpBuffer
//---------------------------------------------------------------------------
		mov		eax, nPitch
		mov		ebx, Clipper.y
		mul		ebx
		mov     ebx, Clipper.x
		add		ebx, ebx
		add     eax, ebx
		mov		edi, lpBuffer
		add		edi, eax
//---------------------------------------------------------------------------
//  初始化 ESI 指向图块数据起点 (跳过 Clipper.top 行图形数据)
//  esi += (nWidth * Clipper.top + Clipper.left) * 2
//---------------------------------------------------------------------------
		mov		ecx, Clipper.top
		mov		eax, nWidth
		mul     ecx
		add     eax, Clipper.left
		add		eax, eax
		mov		esi, lpBitmap
		add     esi, eax
		mov		ecx, Clipper.height

loc_DrawBitmap16_0001:
		push	ecx
		mov		edx, Clipper.width
		mov		ecx, edi
		shr		ecx, 1
		and		ecx, 1
		sub		edx, ecx
		rep		movsw
		mov		ecx, edx
		shr		ecx, 1
		rep		movsd
		adc		ecx, ecx
		rep		movsw
		add		edi, ScreenOffset
		add     esi, BitmapOffset
		pop		ecx
		dec		ecx
		jnz		loc_DrawBitmap16_0001
	}
	pCanvas->UnlockCanvas();
}*/

//---------------------------------------------------------------------------
// 函数:	DrawBitmap16mmx
// 功能:	绘制16位色位图
// 参数:	node, canvas
// 返回:	void
//---------------------------------------------------------------------------
void g_DrawBitmap16mmx(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode*)node;
	KCanvas* pCanvas = (KCanvas*)canvas;

	long nX = pNode->m_nX;// x coord
	long nY = pNode->m_nY;// y coord
	long nWidth = pNode->m_nWidth;// width of sprite
	long nHeight = pNode->m_nHeight;// height of sprite
	void* lpBitmap = pNode->m_pBitmap;// bitmap pointer

	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (!pCanvas->MakeClip(nX, nY, nWidth, nHeight, &Clipper))
		return;

	int nPitch;
	void* lpBuffer = pCanvas->LockCanvas(nPitch);
	if (lpBuffer == NULL)
		return;

	// 计算屏幕下一行的偏移
	long ScreenOffset = nPitch - Clipper.width * 2;

	// 计算位图下一行的偏移
	long BitmapOffset = nWidth * 2 - Clipper.width * 2;

	// 绘制函数的汇编代码
#ifdef _WIN64
#else
	__asm
	{
		//---------------------------------------------------------------------------
		//  计算 EDI 指向屏幕起点的偏移量 (以字节计)
		//  edi = (nPitch*Clipper.y + nX)*2 + lpBuffer
		//---------------------------------------------------------------------------
		mov		eax, nPitch
		mov		ebx, Clipper.y
		mul		ebx
		mov     ebx, Clipper.x
		add		ebx, ebx
		add     eax, ebx
		mov		edi, lpBuffer
		add		edi, eax
		//---------------------------------------------------------------------------
		//  初始化 ESI 指向图块数据起点 (跳过 Clipper.top 行图形数据)
		//  esi += (nWidth * Clipper.top + Clipper.left) * 2
		//---------------------------------------------------------------------------
		mov		ecx, Clipper.top
		mov		eax, nWidth
		mul     ecx
		add     eax, Clipper.left
		add		eax, eax
		mov		esi, lpBitmap
		add     esi, eax
		//---------------------------------------------------------------------------
		// 以一次4个点的方式来绘制位图
		//---------------------------------------------------------------------------
		mov		edx, Clipper.height
		mov		ebx, Clipper.width
		mov		eax, 8

		loc_DrawBitmap16mmx_0001:

		mov		ecx, ebx
			shr		ecx, 2
			jz      loc_DrawBitmap16mmx_0003

			loc_DrawBitmap16mmx_0002 :

		movq	mm0, [esi]
			add		esi, eax
			movq[edi], mm0
			add		edi, eax
			dec		ecx
			jnz		loc_DrawBitmap16mmx_0002

			loc_DrawBitmap16mmx_0003 :
		mov		ecx, ebx
			and ecx, 3
			rep		movsw
			add     esi, BitmapOffset
			add		edi, ScreenOffset
			dec		edx
			jnz		loc_DrawBitmap16mmx_0001
			emms
	}
	pCanvas->UnlockCanvas();
#endif
}

void g_DrawBitmap16mmx_32b(void* node, void* canvas)
{
#ifndef _WIN64
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;

	long nX = pNode->m_nX;// x coord
	long nY = pNode->m_nY;// y coord
	long nWidth = pNode->m_nWidth;// width of sprite
	long nHeight = pNode->m_nHeight;// height of sprite
	void* lpBitmap = pNode->m_pBitmap;// bitmap pointer
	// t1o khung vu?ng trong ph1m vi khung game
	KClipper Clipper;
	if (!pCanvas->MakeClip(nX, nY, nWidth, nHeight, &Clipper))
		return;
	int nPitch;
	void* lpBuffer = pCanvas->LockCanvas(nPitch);
	if (lpBuffer == NULL)
		return;
	lpBuffer = (char*)(lpBuffer) + nPitch*Clipper.y + Clipper.x*4;
	// kho?ng c?ch xuèng d?ng v? k? ti?p c?a canvas
	int ScreenOffset = nPitch - Clipper.width * 4;

	// kho?ng c?ch xuèng d?ng v? k? ti?p c?a bitmap
	int BitmapOffset = nWidth * 2 - Clipper.width * 2;

	__asm
	{
//---------------------------------------------------------------------------
//  edi = lpBuffer + nPitch*Clipper.y + Clipper.x*4
//---------------------------------------------------------------------------
		mov		edi, lpBuffer
//---------------------------------------------------------------------------
//  esi = (nWidth * Clipper.top + Clipper.left) * 2
//---------------------------------------------------------------------------
		mov		ecx, Clipper.top
		mov		eax, nWidth
		mul     ecx
		add     eax, Clipper.left
		add		eax, eax
		mov		esi, lpBitmap
		add     esi, eax
//---------------------------------------------------------------------------
// cao, réng c?a khung v?
//---------------------------------------------------------------------------
		mov		edx, Clipper.height
		mov		ebx, Clipper.width

loc_DrawBitmap16mmx_0001:
		mov		ecx, ebx

loc_DrawBitmap16mmx_0002:
		movzx	eax, word ptr[esi]
		add		esi, 2
		push	ebx
		push	ecx
		push	edx
		mov		ebx,eax
		and		ebx,0xf800
		shl		ebx,8
		mov		ecx,eax
		and		ecx,0x7e0
		shl		ecx,5
		mov		edx,eax
		and		edx,0x1f
		shl		edx,3
		or		ebx,ecx
		or		ebx,edx
		or		ebx,0xff000000
		mov		[edi], ebx
		add		edi, 4
		pop		edx
		pop		ecx
		pop		ebx
		dec		ecx
		jnz		loc_DrawBitmap16mmx_0002

		add     esi, BitmapOffset
		add		edi, ScreenOffset
		dec		edx
		jnz		loc_DrawBitmap16mmx_0001
	}
	pCanvas->UnlockCanvas();
#endif
}
//---------------------------------------------------------------------------
// 函数:	DrawBitmap16win
// 功能:	绘制16位色位图
// 参数:	node, canvas
// 返回:	void
//---------------------------------------------------------------------------
void g_DrawBitmap16win(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode*)node;
	KCanvas* pCanvas = (KCanvas*)canvas;

	long nX = pNode->m_nX;// x coord
	long nY = pNode->m_nY;// y coord
	long nWidth = pNode->m_nWidth;// width of sprite
	long nHeight = pNode->m_nHeight;// height of sprite
	void* lpBitmap = pNode->m_pBitmap;// bitmap pointer

	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (!pCanvas->MakeClip(nX, nY, nWidth, nHeight, &Clipper))
		return;

	int nPitch;
	void* lpBuffer = pCanvas->LockCanvas(nPitch);
	if (lpBuffer == NULL)
		return;

	// 计算屏幕下一行的偏移
	long ScreenOffset = nPitch - Clipper.width * 2;

	// 计算位图下一行的偏移
	long BitmapOffset = nWidth * 2 + Clipper.width * 2;
	long BitmapStarts = nWidth * (nHeight - 1) * 2;

	// 绘制函数的汇编代码
#ifdef _WIN64
#else
	__asm
	{
		//---------------------------------------------------------------------------
		//  计算 EDI 指向屏幕起点的偏移量 (以字节计)
		//  edi = (nPitch*Clipper.y + nX)*2 + lpBuffer
		//---------------------------------------------------------------------------
		mov		eax, nPitch
		mov		ebx, Clipper.y
		mul		ebx
		mov     ebx, Clipper.x
		add		ebx, ebx
		add     eax, ebx
		mov		edi, lpBuffer
		add		edi, eax
		//---------------------------------------------------------------------------
		//  初始化 ESI 指向图块数据起点 (跳过 Clipper.top 行图形数据)
		//  esi += (nWidth * Clipper.top + Clipper.left) * 2
		//---------------------------------------------------------------------------
		mov		ecx, Clipper.top
		mov		eax, nWidth
		mul     ecx
		add     eax, Clipper.left
		add		eax, eax
		mov		esi, lpBitmap
		mov		ebx, BitmapStarts
		add		esi, ebx
		sub     esi, eax
		//---------------------------------------------------------------------------
		// 以一次4个点的方式来绘制位图
		//---------------------------------------------------------------------------
		mov		edx, Clipper.height
		mov		ebx, Clipper.width
		mov		eax, 8

		loc_DrawBitmap16win_0001:

		mov		ecx, ebx
			shr		ecx, 2
			jz      loc_DrawBitmap16win_0003

			loc_DrawBitmap16win_0002 :

		movq	mm0, [esi]
			movq[edi], mm0
			add		esi, eax
			add		edi, eax
			dec		ecx
			jnz		loc_DrawBitmap16win_0002

			loc_DrawBitmap16win_0003 :
		mov		ecx, ebx
			and ecx, 3
			rep		movsw
			sub     esi, BitmapOffset
			add		edi, ScreenOffset
			dec		edx
			jnz		loc_DrawBitmap16win_0001
			emms
	}
	pCanvas->UnlockCanvas();
#endif
}
//---------------------------------------------------------------------------