//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KDrawBase.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Graphics Drawing Functions
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KDDraw.h"
#include "KCanvas.h"
#include "KDrawBase.h"
//---------------------------------------------------------------------------
#define ABS(a)			((a > 0) ? a : -a)
#define SIGN(a)			((a > 0) ? 1 : -1)
//---------------------------------------------------------------------------
// Function: Draw Pixel
// Function: draw a point
// Parameters: x X coordinate
// y Y coordinate
// color color
// Return: void
//---------------------------------------------------------------------------
void g_DrawPixel(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode*)node;
	KCanvas* pCanvas = (KCanvas*)canvas;
	long nX = pNode->m_nX;
	long nY = pNode->m_nY;
	long nColor = pNode->m_nColor;

	int nPitch;
	void* lpBuffer = pCanvas->LockCanvas(nPitch);
	if (lpBuffer == NULL)
		return;

	RECT ClipRect;
	pCanvas->GetClipRect(&ClipRect);

#ifdef _WIN64
#else
	__asm
	{
		mov		eax, nY
		cmp		eax, ClipRect.top
		jl		loc_PutPixel_exit
		cmp		eax, ClipRect.bottom
		jge		loc_PutPixel_exit

		mov		ebx, nX
		cmp		ebx, ClipRect.left
		jl		loc_PutPixel_exit
		cmp		ebx, ClipRect.right
		jge		loc_PutPixel_exit

		mov		ecx, nPitch
		mul		ecx
		add		eax, ebx
		add		eax, ebx
		mov		edi, lpBuffer
		add		edi, eax
		mov		eax, nColor
		mov[edi], ax

		loc_PutPixel_exit :
	}
	pCanvas->UnlockCanvas();
#endif
}
//---------------------------------------------------------------------------
// Function: PutPixelAlpha
// Function: Draw pixels with transparency
// Parameters: x X coordinate
// y Y coordinate
// color color value
// alpha Alpha value
// Return: void
// Formula: (r1, g1, b1) ~ (r2, g2, b2) = (r3, g3, b3)
// r3 = (r1 * alpha + r2 * (32 - alpha)) / 32
// g3 = (g1 * alpha + g2 * (32 - alpha)) / 32
// b3 = (b1 * alpha + b2 * (32 - alpha)) / 32
//---------------------------------------------------------------------------
void g_DrawPixelAlpha(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode*)node;
	KCanvas* pCanvas = (KCanvas*)canvas;
	long nX = pNode->m_nX;
	long nY = pNode->m_nY;
	long nColor = pNode->m_nColor;
	long nAlpha = pNode->m_nAlpha;
	long nMask32 = pCanvas->m_nMask32;

	int nPitch;
	void* lpBuffer = pCanvas->LockCanvas(nPitch);
	if (lpBuffer == NULL)
		return;

	RECT ClipRect;
	pCanvas->GetClipRect(&ClipRect);

#ifdef _WIN64
#else
	__asm
	{
		mov     eax, nY
		cmp		eax, ClipRect.top
		jl		loc_PutPixelAlpha_exit
		cmp		eax, ClipRect.bottom
		jge		loc_PutPixelAlpha_exit

		mov		ebx, nX
		cmp		ebx, ClipRect.left
		jl		loc_PutPixelAlpha_exit
		cmp		ebx, ClipRect.right
		jge		loc_PutPixelAlpha_exit

		mov		ecx, nPitch
		mul		ecx
		add     eax, ebx
		add     eax, ebx
		mov		edi, lpBuffer
		add     edi, eax

		mov     ax, [edi]
		mov		bx, ax
		sal		eax, 16
		mov		ax, bx
		and eax, nMask32

		mov		ecx, nColor
		mov		bx, cx
		sal		ecx, 16
		mov		cx, bx
		and ecx, nMask32

		mov		ebx, nAlpha
		mul		ebx
		neg		ebx
		add		ebx, 0x20
		xchg	eax, ecx
		mul		ebx
		add		eax, ecx
		sar		eax, 5

		and eax, nMask32
		mov		bx, ax
		sar		eax, 16
		or ax, bx
		mov[edi], ax

		loc_PutPixelAlpha_exit :
	}
	pCanvas->UnlockCanvas();
#endif
}

//---------------------------------------------------------------------------
// 函数:	PutPixelAlpha
// 功能:	绘制带透明度的象素点
// 参数:	x			X坐标
//			y			Y坐标
//			color		颜色值
//			alpha		Alpha值
// 返回:	void
// 公式:	(r1, g1, b1) ~ (r2, g2, b2) = (r3, g3, b3)
//			r3 = (r1 * alpha + r2 * (32 - alpha)) / 32
//			g3 = (g1 * alpha + g2 * (32 - alpha)) / 32
//			b3 = (b1 * alpha + b2 * (32 - alpha)) / 32
//---------------------------------------------------------------------------
void g_DrawPixelColorWithAlpha(void* node, void* canvas, long nYcolor)
{
	KDrawNode* pNode = (KDrawNode*)node;
	KCanvas* pCanvas = (KCanvas*)canvas;
	long nX = pNode->m_nX;
	long nY = pNode->m_nY;
	long nColor = pNode->m_nColor;
	long nAlpha = pNode->m_nAlpha;
	long nMask32 = pCanvas->m_nMask32;

	int nPitch;
	void* lpBuffer = pCanvas->LockCanvas(nPitch);
	if (lpBuffer == NULL)
		return;

	RECT ClipRect;
	pCanvas->GetClipRect(&ClipRect);
#ifdef _WIN64
#else
	__asm
	{
		mov     eax, nY
		cmp		eax, ClipRect.top
		jl		loc_PutPixelAlpha_exit
		cmp		eax, ClipRect.bottom
		jge		loc_PutPixelAlpha_exit

		mov		ebx, nX
		cmp		ebx, ClipRect.left
		jl		loc_PutPixelAlpha_exit
		cmp		ebx, ClipRect.right
		jge		loc_PutPixelAlpha_exit		 //判断 这个点是否在画布范围内

		mov		ecx, nPitch		             //赋值
		mul		ecx
		add     eax, ebx
		add     eax, ebx
		mov		edi, lpBuffer
		add     edi, eax

		mov     ax, [edi]
		mov		bx, ax
		sal		eax, 16
		mov		ax, bx
		and eax, nMask32        //是这个点的32位色吗？
		//开始判断颜色 是否是黑色
		mov		ecx, nYcolor
		cmp		ebx, eax
		jge		loc_PutPixelAlpha_exit

		mov		ecx, nColor			//替换颜色
		mov		bx, cx
		sal		ecx, 16
		mov		cx, bx
		and ecx, nMask32

		mov		ebx, nAlpha			//设置通道
		mul		ebx
		neg		ebx
		add		ebx, 0x20
		xchg	eax, ecx
		mul		ebx
		add		eax, ecx
		sar		eax, 5

		and eax, nMask32
		mov		bx, ax
		sar		eax, 16
		or ax, bx
		mov[edi], ax

		loc_PutPixelAlpha_exit :
	}
#endif
	pCanvas->UnlockCanvas();
}

//---------------------------------------------------------------------------
// 函数:	DrawLine
// 功能:	绘制直线
// 参数:	x1		X1坐标
//			x2		X2坐标
//			y1		Y1坐标
//			y2		Y2坐标
//			color	颜色值
// 返回:	void
//---------------------------------------------------------------------------
void g_DrawLine(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode*)node;
	KCanvas* pCanvas = (KCanvas*)canvas;
	long x1 = pNode->m_nX;
	long y1 = pNode->m_nY;
	long x2 = pNode->m_nWidth;
	long y2 = pNode->m_nHeight;
	KDrawNode LineNode;
	LineNode.m_nColor = pNode->m_nColor;

	long d, x, y, ax, ay, sx, sy, dx, dy;

	dx = x2 - x1;
	ax = abs(dx) << 1;
	sx = SIGN(dx);

	dy = y2 - y1;
	ay = abs(dy) << 1;
	sy = SIGN(dy);

	x = x1;
	y = y1;

	if (ax > ay)
	{
		d = ay - (ax >> 1);
		while (x != x2)
		{
			LineNode.m_nX = x;
			LineNode.m_nY = y;
			g_DrawPixel(&LineNode, pCanvas);
			if (d >= 0)
			{
				y += sy;
				d -= ax;
			}
			x += sx;
			d += ay;
		}
	}
	else
	{
		d = ax - (ay >> 1);
		while (y != y2)
		{
			LineNode.m_nX = x;
			LineNode.m_nY = y;
			g_DrawPixel(&LineNode, pCanvas);
			if (d >= 0)
			{
				x += sx;
				d -= ay;
			}
			y += sy;
			d += ax;
		}
	}
	LineNode.m_nX = x;
	LineNode.m_nY = y;
	g_DrawPixel(&LineNode, pCanvas);
}
//---------------------------------------------------------------------------
// 函数:	Draw Line Alpha
// 功能:	绘制透明直线
// 参数:	x1		X1坐标
//			x2		X2坐标
//			y1		Y1坐标
//			y2		Y2坐标
//			color	颜色值
//			alpha	透明值
// 返回:	void
//---------------------------------------------------------------------------
void g_DrawLineAlpha(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode*)node;
	KCanvas* pCanvas = (KCanvas*)canvas;
	long x1 = pNode->m_nX;
	long y1 = pNode->m_nY;
	long x2 = pNode->m_nWidth;
	long y2 = pNode->m_nHeight;
	KDrawNode LineNode;
	LineNode.m_nColor = pNode->m_nColor;
	LineNode.m_nAlpha = pNode->m_nAlpha;

	long d, x, y, ax, ay, sx, sy, dx, dy;

	dx = x2 - x1;
	ax = abs(dx) << 1;
	sx = SIGN(dx);

	dy = y2 - y1;
	ay = abs(dy) << 1;
	sy = SIGN(dy);

	x = x1;
	y = y1;

	if (ax > ay)
	{
		d = ay - (ax >> 1);
		while (x != x2)
		{
			LineNode.m_nX = x;
			LineNode.m_nY = y;
			g_DrawPixelAlpha(&LineNode, pCanvas);
			if (d >= 0)
			{
				y += sy;
				d -= ax;
			}
			x += sx;
			d += ay;
		}
	}
	else
	{
		d = ax - (ay >> 1);
		while (y != y2)
		{
			LineNode.m_nX = x;
			LineNode.m_nY = y;
			g_DrawPixelAlpha(&LineNode, pCanvas);
			if (d >= 0)
			{
				x += sx;
				d -= ay;
			}
			y += sy;
			d += ax;
		}
	}
	LineNode.m_nX = x;
	LineNode.m_nY = y;
	g_DrawPixelAlpha(&LineNode, pCanvas);
}
void g_DrawSprColroAlpha(void* node, void* canvas, long nYcolor)
{
	KDrawNode* pNode = (KDrawNode*)node;
	KCanvas* pCanvas = (KCanvas*)canvas;
	long x1 = pNode->m_nX;
	long y1 = pNode->m_nY;
	long x2 = pNode->m_nWidth;
	long y2 = pNode->m_nHeight;
	KDrawNode LineNode;
	LineNode.m_nColor = pNode->m_nColor;
	LineNode.m_nAlpha = pNode->m_nAlpha;

	long d, x, y, ax, ay, sx, sy, dx, dy;

	dx = x2 - x1;
	ax = abs(dx) << 1;
	sx = SIGN(dx);

	dy = y2 - y1;
	ay = abs(dy) << 1;
	sy = SIGN(dy);

	x = x1;
	y = y1;

	if (ax > ay)
	{
		d = ay - (ax >> 1);
		while (x != x2)
		{
			LineNode.m_nX = x;
			LineNode.m_nY = y;
			g_DrawPixelColorWithAlpha(&LineNode, pCanvas, nYcolor);
			if (d >= 0)
			{
				y += sy;
				d -= ax;
			}
			x += sx;
			d += ay;
		}
	}
	else
	{
		d = ax - (ay >> 1);
		while (y != y2)
		{
			LineNode.m_nX = x;
			LineNode.m_nY = y;
			g_DrawPixelColorWithAlpha(&LineNode, pCanvas, nYcolor);
			if (d >= 0)
			{
				x += sx;
				d -= ay;
			}
			y += sy;
			d += ax;
		}
	}
	LineNode.m_nX = x;
	LineNode.m_nY = y;
	g_DrawPixelColorWithAlpha(&LineNode, pCanvas, nYcolor);
}
//---------------------------------------------------------------------------
// 函数:	Clear a region on canvas
// 功能:	用色彩nColor清除区域
// 参数:	nX, nY, nWidth, nHeight, nColor
// 返回:	void
//---------------------------------------------------------------------------
void g_Clear(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode*)node;
	KCanvas* pCanvas = (KCanvas*)canvas;

	long nX = pNode->m_nX;// x coord
	long nY = pNode->m_nY;// y coord
	long nWidth = pNode->m_nWidth;// width of sprite
	long nHeight = pNode->m_nHeight;// height of sprite
	int  nColor = pNode->m_nColor;//  color

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
		mov		ecx, Clipper.height
		//---------------------------------------------------------------------------
		//  eax = nColor
		//---------------------------------------------------------------------------
		mov		eax, nColor
		mov     bx, ax
		shl     eax, 16
		mov     ax, bx
		//---------------------------------------------------------------------------
		//  color tranfer
		//---------------------------------------------------------------------------

	loc_Clear_0001:
		push	ecx
			mov		edx, Clipper.width
			mov		ecx, edi
			shr		ecx, 1
			and ecx, 1
			sub		edx, ecx
			rep		stosw
			mov		ecx, edx
			shr		ecx, 1
			rep		stosd
			adc		ecx, ecx
			rep		stosw
			add		edi, ScreenOffset

			pop		ecx
			dec		ecx
			jnz		loc_Clear_0001
	}
	pCanvas->UnlockCanvas();
#endif
}
//---------------------------------------------------------------------------
// 函数:	Clear a region on canvas
// 功能:	用色彩nColor隔点清除清除区域
// 参数:	nX, nY, nWidth, nHeight, nColor
// 返回:	void
//---------------------------------------------------------------------------
void g_DotClear(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode*)node;
	KCanvas* pCanvas = (KCanvas*)canvas;

	long nX = pNode->m_nX;// x coord
	long nY = pNode->m_nY;// y coord
	long nWidth = pNode->m_nWidth;// width of sprite
	long nHeight = pNode->m_nHeight;// height of sprite
	int  nColor = pNode->m_nColor;//  color

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
		mov		ecx, Clipper.height
		//---------------------------------------------------------------------------
		//  eax = nColor
		//---------------------------------------------------------------------------
		mov		eax, nColor
		//---------------------------------------------------------------------------
		//  color tranfer
		//---------------------------------------------------------------------------

	loc_DotClear_0001:
		push	ecx
			mov		ecx, Clipper.width
			loc_DotClear_0002 :
		stosw
			dec  ecx
			jz   loc_DotClear_0003
			inc     edi
			inc     edi
			dec  ecx
			jnz   loc_DotClear_0002
			loc_DotClear_0003 :
		add		edi, ScreenOffset
			pop		ecx
			dec		ecx
			jz      loc_DotClear_0006

			push	ecx
			mov		ecx, Clipper.width
			loc_DotClear_0004 :
		inc     edi
			inc     edi
			dec     ecx
			jz      loc_DotClear_0005
			stosw
			dec  ecx
			jnz   loc_DotClear_0004
			loc_DotClear_0005 :
		add		edi, ScreenOffset
			pop		ecx
			dec		ecx
			jnz		loc_DotClear_0001
			loc_DotClear_0006 :
	}
	pCanvas->UnlockCanvas();
#endif
}

//---------------------------------------------------------------------------
// 函数:	Clear a region on canvas with alpha
// 功能:	用色彩nColor带alpha清除区域
// 参数:	nX, nY, nWidth, nHeight, nColor,nAlpha
// 返回:	void
// Modify By Freeway Chen in 2003.7.6, USE MMX Register
//---------------------------------------------------------------------------
void g_ClearAlpha(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode*)node;
	KCanvas* pCanvas = (KCanvas*)canvas;

	long nX = pNode->m_nX;// x coord
	long nY = pNode->m_nY;// y coord
	long nWidth = pNode->m_nWidth;// width of sprite
	long nHeight = pNode->m_nHeight;// height of sprite
	int  nColor = pNode->m_nColor;//  color
	long nAlpha = pNode->m_nAlpha;
	long nMask32 = pCanvas->m_nMask32;

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

		mov     esi, nMask32    // esi:  nMask32

		mov		ecx, nColor
		mov		bx, cx
		sal		ecx, 16
		mov		cx, bx
		and ecx, esi        // esi:  nMask32
		movd    mm1, ecx        // mm1: nMaskColor

		mov     ecx, nAlpha
		mov     eax, 0x20
		movd    mm3, ecx        // mm3: nAlpha
		sub     eax, ecx
		movd    ecx, mm1        // mm1: nMaskColor
		imul    eax, ecx
		movd    mm1, eax        // mm1: nMaskColor * (32 - nAlpha)

		mov		ecx, Clipper.height

		movd    mm6, Clipper.width

		//---------------------------------------------------------------------------
		//  color tranfer
		//---------------------------------------------------------------------------

	loc_ClearAlpha_0001:
		movd    mm7, ecx
			movd	edx, mm6        // Clipper.width

			loc_ClearAlpha_0002 :
		mov     ax, [edi]
			mov		cx, ax
			sal		eax, 16
			movd	ebx, mm3        // mm3: nAlpha
			mov		ax, cx
			and eax, esi        // esi:  nMask32

			imul	eax, ebx
			movd    ecx, mm1          // mm1: nMaskColor * (32 - nAlpha)
			add		eax, ecx
			sar		eax, 5
			and eax, esi         // esi:  nMask32

			mov		bx, ax
			sar		eax, 16
			add     edi, 2
			or ax, bx
			dec		edx
			mov[edi - 2], ax
			jnz		loc_ClearAlpha_0002

			movd    ecx, mm7
			add		edi, ScreenOffset
			dec		ecx
			jnz		loc_ClearAlpha_0001
			emms
	}
	pCanvas->UnlockCanvas();
#endif
}

//---------------------------------------------------------------------------
// 函数:	Clear a region on canvas with alpha
// 功能:	带通道清除某颜色
// 参数:	nX, nY, nWidth, nHeight, nColor,nAlpha
// 返回:	void
// Modify By Freeway Chen in 2003.7.6, USE MMX Register
//---------------------------------------------------------------------------
void g_ClearColrWhitAlpha(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode*)node;
	KCanvas* pCanvas = (KCanvas*)canvas;

	long nX = pNode->m_nX;// x coord
	long nY = pNode->m_nY;// y coord
	long nWidth = pNode->m_nWidth;// width of sprite
	long nHeight = pNode->m_nHeight;// height of sprite
	int  nColor = pNode->m_nColor;//  color
	long nAlpha = pNode->m_nAlpha;
	long nMask32 = pCanvas->m_nMask32;

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

		mov     esi, nMask32    // esi:  nMask32

		mov		ecx, nColor
		mov		bx, cx
		sal		ecx, 16			//左移 	 nColor>>4;
		mov		cx, bx
		and ecx, esi        // esi:  nMask32
		movd    mm1, ecx        // mm1: nMaskColor

		//movd    mm1, esi        // mm1: nMaskColor

		mov     ecx, nAlpha
		mov     eax, 0x20		// eax=32
		movd    mm3, ecx        // mm3: nAlpha
		sub     eax, ecx
		movd    ecx, mm1        // mm1: nMaskColor
		imul    eax, ecx
		movd    mm1, eax        // mm1: nMaskColor * (32 - nAlpha)	   //通道值

		mov		ecx, Clipper.height

		movd    mm6, Clipper.width

		//---------------------------------------------------------------------------
		//  color tranfer	颜色 通道 的转变
		//---------------------------------------------------------------------------

	loc_ClearAlpha_0001:
		movd    mm7, ecx 		// mm7:Clipper.height 画布的高度
			movd	edx, mm6        // mm6:Clipper.width   画布的长度

			loc_ClearAlpha_0002 :
		mov     ax, [edi]		//下一个点的指针
			mov		cx, ax
			sal		eax, 16			//左移	通道改变
			movd	ebx, mm3        // mm3: nAlpha
			mov		ax, cx
			and eax, esi        // esi:  nMask32

			imul	eax, ebx		//相乘 颜色改变
			movd    ecx, mm1        // mm1: nMaskColor * (32 - nAlpha)
			add		eax, ecx
			sar		eax, 5			//右移
			and eax, esi        // esi:  nMask32

			mov		bx, ax
			sar		eax, 16			 //右移
			add     edi, 2 			 //位与运算
			or ax, bx			 //位或运算
			dec		edx			     //减 1	  画布的长度
			mov[edi - 2], ax
			jnz		loc_ClearAlpha_0002	//一个循环 自减少的循环

			movd    ecx, mm7
			add		edi, ScreenOffset
			dec		ecx		         //减 1	 // ecx:Clipper.height 画布的高度
			jnz		loc_ClearAlpha_0001

			emms
	}
#endif
	pCanvas->UnlockCanvas();
}
//---------------------------------------------------------------------------