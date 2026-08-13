/*****************************************************************************************
//  图形到内存区域的操作
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-11
*****************************************************************************************/
#include "ImageOperation.h"
#include "crtdbg.h"

static unsigned int l_uRGBBitMask32 = 0x07e0f81f;
#define		read_alpha_2_ebx_run_length_2_eax		    \
				{	movzx	eax, byte ptr[esi]	}	    \
				{	movzx	ebx, byte ptr[esi + 1]	}	    \
				{	add		esi, 2   			}

#define		copy_pixel_use_eax		\
				{	xor	    eax, eax            }	\
				{	mov	    al, byte ptr [esi]	}	\
				{	add		esi, 1 				}	\
				{	mov		ax, [ebx + eax * 2]	}	\
				{	or	    eax, 0xff000000     }	\
				{	add		edi, 4				}   \
				{	mov		[edi - 4], eax		}
				
#define		copy_pixel_use_eax_32b	/*ebx palette 3 colors*/	\
				{	movzx   eax, byte ptr [esi]	}	\
				{	imul	eax, 3            	}   \
				{	movd	mm4, eax            }   \
				{	inc		esi					}	\
				{	push	ecx                 }   \
				{	push	edx                 }   \
				{	movzx	eax, byte ptr[ebx + eax]	}	\
				{	shl		eax, 16             }   \
				{	push	eax                 }   \
				{	movd	eax, mm4            }   \
				{	movzx	ecx, byte ptr[ebx + eax + 1]	}\
				{	shl		ecx, 8              }   \
				{	movzx	edx, byte ptr[ebx + eax + 2]	}\
				{	pop		eax                 }   \
				{	or		eax,ecx             }   \
				{	or		eax,edx             }   \
				{	or		eax,0xff000000      }   \
				{	add		edi, 4				}   \
				{	mov		[edi - 4], eax		}   \
				{	pop		edx                 }   \
				{	pop		ecx                 }   
				
#define		mix_2_pixel_color_use_eabdx									\
				{	movd	mm7, ecx			}						\
                {   xor     eax, eax            }                       \
				{	movd    ebx, mm0    		}	/* pPalette */		\
				{	mov	    al, byte ptr[esi]	}						\
				{	inc		esi					}						\
				{	mov     dx, [ebx + eax * 2]	}	/*edx = ...rgb*/	\
				{	movd	ecx, mm2    		}	/* nMask32 */		\
				{	mov		ax, dx				}	/*eax = ...rgb*/	\
				{	shl		eax, 16				}	/*eax = rgb...*/	\
				{	mov		ax, dx				}	/*eax = rgbrgb*/	\
				{	and		eax, ecx			}	/*eax = .g.r.b*/	\
				{	mov		dx, [edi]			}	/*edx = ...rgb*/	\
				{	mov		bx, dx				}	/*ebx = ...rgb*/	\
				{	shl		ebx, 16				}	/*ebx = rgb...*/	\
				{	mov		bx, dx				}	/*ebx = rgbrgb*/	\
				{	and		ebx, ecx			}	/*ebx = .g.r.b*/	\
                {   lea     edx, [ebx + ebx * 2]}                       \
                {   add     eax, edx            }                       \
				{	shr		eax, 2				}	/*c = (3xc1+c2)/4*/	\
				{	and     eax, ecx			}	/*eax = .g.r.b*/	\
				{	mov     dx, ax				}	/*edx = ...r.b*/	\
				{	shr     eax, 16				}	/*eax = ....g.*/	\
				{	add 	edi, 2				}						\
				{	or      ax, dx				}	/*eax = ...rgb*/	\
				{	movd     ecx, mm7			}                       \
				{	mov		[edi - 2], ax		}


void RIO_Set16BitImageFormat(int b565)
{
	l_uRGBBitMask32 = b565 ? 0x07e0f81f : 0x03e07c1f;
}

struct KRClipperInfo
{
	int			x;			// 裁减后的X坐标
	int			y;			// 裁减后的Y坐标
	int			width;		// 裁减后的宽度
	int			height;		// 裁减后的高度
	int			left;		// 上边界裁剪量
	int			top;		// 左边界裁剪量
	int			right;		// 右边界裁剪量
};

static UINT g_BlendColor16b(UINT nSrcColor, UINT nBlendColor, int nMode, BOOL bDstClr16b = FALSE)
{
	float sR = (float)((nSrcColor & 0xf800) >> 8)/255.f;
	float sG = (float)((nSrcColor & 0x07e0) >> 3)/255.f;
	float sB = (float)((nSrcColor & 0x001f) << 3)/255.f;
	float dR,dG,dB;
	if(bDstClr16b)
	{
		dR = (float)((nBlendColor & 0xf800) >> 8)/255.f;
		dG = (float)((nBlendColor & 0x07e0) >> 3)/255.f;
		dB = (float)((nBlendColor & 0x001f) << 3)/255.f;
	}
	else
	{
		dR = (float)((nBlendColor & 0xff0000) >> 16)/255.f;
		dG = (float)((nBlendColor & 0xff00) >> 8)/255.f;
		dB = (float)((nBlendColor & 0xff))/255.f;
	}
	dR *= sR;
	dG *= sG;
	dB *= sB;
	
	UINT nRetColor = (((UINT)(dB*255.f)) >> 3);
	nRetColor |= ((((UINT)(dR*255.f)) >> 3) << 11);
	nRetColor |= ((((UINT)(dG*255.f)) >> 2) << 5);
	return nRetColor;
}

static UINT g_BlendColor16b(UINT usR, UINT usG, UINT usB, UINT nBlendColor, int nMode, BOOL bDstClr16b = FALSE, UINT usA = 0)
{
	float sA = (float)usA/255.f;
	float sR = (float)usR/255.f;
	float sG = (float)usG/255.f;
	float sB = (float)usB/255.f;
	float dR,dG,dB;
	if(bDstClr16b)
	{
		dR = (float)((nBlendColor & 0xf800) >> 8)/255.f;
		dG = (float)((nBlendColor & 0x07e0) >> 3)/255.f;
		dB = (float)((nBlendColor & 0x001f) << 3)/255.f;
	}
	else
	{
		dR = (float)((nBlendColor & 0xff0000) >> 16)/255.f;
		dG = (float)((nBlendColor & 0xff00) >> 8)/255.f;
		dB = (float)((nBlendColor & 0xff))/255.f;
	}
	dR *= sR;
	dG *= sG;
	dB *= sB;
	
	UINT nRetColor = (((UINT)(dB*255.f)) >> 3);
	nRetColor |= ((((UINT)(dR*255.f)) >> 3) << 11);
	nRetColor |= ((((UINT)(dG*255.f)) >> 2) << 5);
	return nRetColor;
}

static UINT g_BlendColor32b(UINT usR, UINT usG, UINT usB, UINT nBlendColor, int nMode, UINT usA = 0)
{
	float sA = (float)usA/255.f;
	float sR = (float)usR/255.f;
	float sG = (float)usG/255.f;
	float sB = (float)usB/255.f;
	float dR = (float)((nBlendColor & 0xff0000) >> 16)/255.f;
	float dG = (float)((nBlendColor & 0xff00) >> 8)/255.f;
	float dB = (float)(nBlendColor & 0xff)/255.f;
	
	dR *= sR;
	dG *= sG;
	dB *= sB;
	
	UINT nRetColor = (UINT)(dB*255.f);
	nRetColor |= ((UINT)(dR*255.f)) << 16;
	nRetColor |= ((UINT)(dG*255.f)) << 8;
	return nRetColor;
}

static void g_BlendColor32b(UINT usR, UINT usG, UINT usB, UINT nBlendColor, int nMode,
				UINT& uR, UINT& uG, UINT& uB)
{
	float sR = (float)usR/255.f;
	float sG = (float)usG/255.f;
	float sB = (float)usB/255.f;
	float dR = (float)((nBlendColor & 0xff0000) >> 16)/255.f;
	float dG = (float)((nBlendColor & 0xff00) >> 8)/255.f;
	float dB = (float)(nBlendColor & 0xff)/255.f;
	
	dR *= sR;
	dG *= sG;
	dB *= sB;
	
	uR = (UINT)(dR*255.f);
	uG = (UINT)(dG*255.f);
	uB = (UINT)(dB*255.f);
}

static int  RIO_ClipCopyRect(int nX, int nY, int nSrcWidth, int nSrcHeight, int nDestWidth, int nDestHeight, KRClipperInfo* pClipper)
{
	_ASSERT(pClipper);
	// 初始化裁减量
	pClipper->x = nX;
	pClipper->y = nY;
	pClipper->width = nSrcWidth;
	pClipper->height = nSrcHeight;
	pClipper->top = 0;
	pClipper->left = 0;
	pClipper->right = 0;

	// 上边界裁减
	if (pClipper->y < 0)
	{
		pClipper->y = 0;
		pClipper->top = -nY;
		pClipper->height += nY;
	}
	if (pClipper->height <= 0)
		return 0;
	
	// 下边界裁减
	if (pClipper->height > nDestHeight - pClipper->y)
		pClipper->height = nDestHeight - pClipper->y;
	if (pClipper->height <= 0)
		return 0;

	// 左边界裁减
	if (pClipper->x < 0)
	{
		pClipper->x = 0;
		pClipper->left = -nX;
		pClipper->width += nX;
	}
	if (pClipper->width <= 0)
		return 0;

	// 右边界裁减
	if (pClipper->width > nDestWidth - pClipper->x)
	{
		pClipper->right = pClipper->width + pClipper->x - nDestWidth;
		pClipper->width -= pClipper->right;
	}
	if (pClipper->width <= 0)
		return 0;
	
	return 1;
}


void RIO_CopySprToBuffer(void* pSprite, int nSprWidth, int nSprHeight, void* pPalette,
					 void* pBuffer, int nBufferWidth, int nBufferHeight,
					 int nX, int nY)
{
	//_ASSERT(pSpr && pBuffer && pPalette);
	KRClipperInfo Clipper;
	if (RIO_ClipCopyRect(nX, nY, nSprWidth, nSprHeight, nBufferWidth, nBufferHeight, &Clipper) == 0)
		return;
	int nPitch = nBufferWidth * 4;
	pBuffer = (char*)(pBuffer) + Clipper.y * nPitch + Clipper.x*4;
	int nBuffNextLine = nPitch - Clipper.width * 4;// next line add
	int nSprSkip = nSprWidth * Clipper.top + Clipper.left;
	int nSprSkipPerLine = Clipper.left + Clipper.right;

	__asm
	{
		//ê1edi???òbuffer?????eμ?,	(ò?×??ú??)	
		mov		edi, pBuffer

		//ê1esi???òí??éêy?Y?eμ?,(ì?1ynSprSkip????μ?μ?í?D?êy?Y)
		mov		esi, pSprite
		//_SkipSpriteAheadContent_:
		{
			mov		edx, nSprSkip
			or		edx, edx
			jz		_SkipSpriteAheadContentEnd_

			_SkipSpriteAheadContentLocalStart_:
			{
				read_alpha_2_ebx_run_length_2_eax
				or		ebx, ebx
				jnz		_SkipSpriteAheadContentLocalAlpha_
				sub		edx, eax
				jg		_SkipSpriteAheadContentLocalStart_
				neg		edx
				jmp		_SkipSpriteAheadContentEnd_

				_SkipSpriteAheadContentLocalAlpha_:
				{
					add		esi, eax
					sub		edx, eax
					jg		_SkipSpriteAheadContentLocalStart_
					add		esi, edx
					neg		edx
					jmp		_SkipSpriteAheadContentEnd_
				}
			}
		}
		_SkipSpriteAheadContentEnd_:

		mov		eax, nSprSkipPerLine
		or		eax, eax
		jnz		_DrawPartLineSection_	//if (nSprSkipPerLine) goto _DrawPartLineSection_

		//_DrawFullLineSection_:
		{
			//òò?asprite2??á??DD?1??￡??ò??DDμ?′?′|edx±??a0￡?è?sprite?á??DD?1???ò_DrawFullLineSection_Dè??			
			_DrawFullLineSection_Line_:
			{
				mov		edx, Clipper.width
				_DrawFullLineSection_LineLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax

					or		ebx, ebx
					jnz		_DrawFullLineSection_LineLocal_Alpha_
					lea     edi, [edi + eax * 4]
					sub		edx, eax
					jg		_DrawFullLineSection_LineLocal_

					add		edi, nBuffNextLine
					dec		Clipper.height
					jnz		_DrawFullLineSection_Line_
					jmp		_EXIT_WAY_
				
					_DrawFullLineSection_LineLocal_Alpha_:
					{
						sub		edx, eax
						mov		ecx, eax
						mov     ebx, pPalette
						_DrawFullLineSection_CopyPixel_:
						{
							copy_pixel_use_eax
							loop	_DrawFullLineSection_CopyPixel_
						}
						or		edx, edx
						jnz		_DrawFullLineSection_LineLocal_

						add		edi, nBuffNextLine
						dec		Clipper.height
						jnz		_DrawFullLineSection_Line_
						jmp		_EXIT_WAY_
					}
				}
			}
		}

		_DrawPartLineSection_:
		{
			mov		eax, Clipper.left
			or		eax, eax
			jz		_DrawPartLineSection_SkipRight_Line_

			mov		eax, Clipper.right
			or		eax, eax
			jz		_DrawPartLineSection_SkipLeft_Line_
		}

		_DrawPartLineSection_Line_:
		{
			mov		eax, edx
			mov		edx, Clipper.width
			or		eax, eax
			jnz		_DrawPartLineSection_LineLocal_CheckAlpha_
			_DrawPartLineSection_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				_DrawPartLineSection_LineLocal_CheckAlpha_:
				or		ebx, ebx
				jnz		_DrawPartLineSection_LineLocal_Alpha_
				lea     edi, [edi + eax * 4]
				sub		edx, eax
				jg		_DrawPartLineSection_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_

				lea     edi, [edi + edx * 4]
				neg		edx
			}
			
			_DrawPartLineSection_LineSkip_:
			{
				add		edi, nBuffNextLine
				//ì?1ynSprSkipPerLine????μ?sprite?úèY
				mov		eax, edx
				mov		edx, nSprSkipPerLine
				or		eax, eax
				jnz		_DrawPartLineSection_LineSkipLocal_CheckAlpha_
				_DrawPartLineSection_LineSkipLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax
					
					_DrawPartLineSection_LineSkipLocal_CheckAlpha_:
					or		ebx, ebx
					jnz		_DrawPartLineSection_LineSkipLocal_Alpha_
					sub		edx, eax
					jg		_DrawPartLineSection_LineSkipLocal_
					neg		edx
					jmp		_DrawPartLineSection_Line_
					_DrawPartLineSection_LineSkipLocal_Alpha_:
					{
						add		esi, eax
						sub		edx, eax
						jg		_DrawPartLineSection_LineSkipLocal_
						add		esi, edx
						neg		edx
						jmp		_DrawPartLineSection_Line_
					}
				}
			}
			_DrawPartLineSection_LineLocal_Alpha_:
			{
				sub		edx, eax
				jle		_DrawPartLineSection_LineLocal_Alpha_Part_		//2??üè??-?aeax???àí?alpha?μμ???μ?￡?oó??óDμ?ò??-3?3???óò

				mov		ecx, eax
				mov     ebx, pPalette

				_DrawPartLineSection_CopyPixel_:
				{
					copy_pixel_use_eax
					loop	_DrawPartLineSection_CopyPixel_
				}
				jmp		_DrawPartLineSection_LineLocal_
			}
			_DrawPartLineSection_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				mov     ebx, pPalette
				_DrawPartLineSection_CopyPixel_Part_:
				{
					copy_pixel_use_eax
					loop	_DrawPartLineSection_CopyPixel_Part_
				}
			
				dec		Clipper.height
				jz		_EXIT_WAY_
				neg		edx
				mov		ebx, 255	//è?1???òaè·?Dμ??-ebx(alpha)?μ?éò??ú?°í·push ebx￡?′?′|pop??μ?
				jmp		_DrawPartLineSection_LineSkip_
			}
		}

		_DrawPartLineSection_SkipLeft_Line_:
		{
			mov		eax, edx
			mov		edx, Clipper.width
			or		eax, eax
			jnz		_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_
			_DrawPartLineSection_SkipLeft_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_:
				or		ebx, ebx
				jnz		_DrawPartLineSection_SkipLeft_LineLocal_Alpha_
				lea     edi, [edi + eax * 4]
				sub		edx, eax
				jg		_DrawPartLineSection_SkipLeft_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_
			}
			
			_DrawPartLineSection_SkipLeft_LineSkip_:
			{
				add		edi, nBuffNextLine
				//ì?1ynSprSkipPerLine????μ?sprite?úèY
				mov		edx, nSprSkipPerLine
				_DrawPartLineSection_SkipLeft_LineSkipLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax
					or		ebx, ebx
					jnz		_DrawPartLineSection_SkipLeft_LineSkipLocal_Alpha_
					sub		edx, eax
					jg		_DrawPartLineSection_SkipLeft_LineSkipLocal_
					neg		edx
					jmp		_DrawPartLineSection_SkipLeft_Line_
					_DrawPartLineSection_SkipLeft_LineSkipLocal_Alpha_:
					{
						add		esi, eax
						sub		edx, eax
						jg		_DrawPartLineSection_SkipLeft_LineSkipLocal_
						add		esi, edx
						neg		edx
						jmp		_DrawPartLineSection_SkipLeft_Line_
					}
				}
			}
			_DrawPartLineSection_SkipLeft_LineLocal_Alpha_:
			{
				sub		edx, eax
				mov		ecx, eax						
				mov     ebx, pPalette
				_DrawPartLineSection_SkipLeft_CopyPixel_:
				{
					copy_pixel_use_eax
					loop	_DrawPartLineSection_SkipLeft_CopyPixel_
				}
				or		edx, edx
				jnz		_DrawPartLineSection_SkipLeft_LineLocal_
				dec		Clipper.height
				jg		_DrawPartLineSection_SkipLeft_LineSkip_
				jmp		_EXIT_WAY_
			}
		}

		_DrawPartLineSection_SkipRight_Line_:
		{
			mov		edx, Clipper.width
			_DrawPartLineSection_SkipRight_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				or		ebx, ebx
				jnz		_DrawPartLineSection_SkipRight_LineLocal_Alpha_
				lea     edi, [edi + eax * 4]
				sub		edx, eax
				jg		_DrawPartLineSection_SkipRight_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_

				lea     edi, [edi + edx * 4]
				neg		edx
			}
			
			_DrawPartLineSection_SkipRight_LineSkip_:
			{
				add		edi, nBuffNextLine
				//ì?1ynSprSkipPerLine????μ?sprite?úèY
				mov		eax, edx
				mov		edx, nSprSkipPerLine
				or		eax, eax
				jnz		_DrawPartLineSection_SkipRight_LineSkipLocal_CheckAlpha_
				_DrawPartLineSection_SkipRight_LineSkipLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax
					
					_DrawPartLineSection_SkipRight_LineSkipLocal_CheckAlpha_:
					or		ebx, ebx
					jnz		_DrawPartLineSection_SkipRight_LineSkipLocal_Alpha_
					sub		edx, eax
					jg		_DrawPartLineSection_SkipRight_LineSkipLocal_
					jmp		_DrawPartLineSection_SkipRight_Line_
					_DrawPartLineSection_SkipRight_LineSkipLocal_Alpha_:
					{
						add		esi, eax
						sub		edx, eax
						jg		_DrawPartLineSection_SkipRight_LineSkipLocal_
						jmp		_DrawPartLineSection_SkipRight_Line_
					}
				}
			}
			_DrawPartLineSection_SkipRight_LineLocal_Alpha_:
			{
				sub		edx, eax
				jle		_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_		//2??üè??-?aeax???àí?alpha?μμ???μ?￡?oó??óDμ?ò??-3?3???óò

				mov		ecx, eax				
				mov     ebx, pPalette
				_DrawPartLineSection_SkipRight_CopyPixel_:
				{
					copy_pixel_use_eax
					loop	_DrawPartLineSection_SkipRight_CopyPixel_
				}
				jmp		_DrawPartLineSection_SkipRight_LineLocal_
			}
			_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				mov     ebx, pPalette
				_DrawPartLineSection_SkipRight_CopyPixel_Part_:
				{
					copy_pixel_use_eax
					loop	_DrawPartLineSection_SkipRight_CopyPixel_Part_
				}
				neg		edx
				mov		ebx, 255
				dec		Clipper.height
				jg		_DrawPartLineSection_SkipRight_LineSkip_
				jmp		_EXIT_WAY_
			}
		}
		_EXIT_WAY_:
	}
}

void RIO_CopySprToBuffer32b(void* pSprite, int nSprWidth, int nSprHeight, void* pPalette,
					 void* pBuffer, int nBufferWidth, int nBufferHeight,
					 int nX, int nY)
{
	KRClipperInfo Clipper;
	if (RIO_ClipCopyRect(nX, nY, nSprWidth, nSprHeight, nBufferWidth, nBufferHeight, &Clipper) == 0)
		return;
	int nPitch = nBufferWidth * 4;
	pBuffer = (char*)(pBuffer) + Clipper.y * nPitch + Clipper.x*4;
	int nBuffNextLine = nPitch - Clipper.width * 4;// next line add
	int nSprSkip = nSprWidth * Clipper.top + Clipper.left;
	int nSprSkipPerLine = Clipper.left + Clipper.right;

	__asm
	{
		//ê1edi???òbuffer?????eμ?,	(ò?×??ú??)	
		mov		edi, pBuffer

		//ê1esi???òí??éêy?Y?eμ?,(ì?1ynSprSkip????μ?μ?í?D?êy?Y)
		mov		esi, pSprite
		//_SkipSpriteAheadContent_:
		{
			mov		edx, nSprSkip
			or		edx, edx
			jz		_SkipSpriteAheadContentEnd_

			_SkipSpriteAheadContentLocalStart_:
			{
				read_alpha_2_ebx_run_length_2_eax
				or		ebx, ebx
				jnz		_SkipSpriteAheadContentLocalAlpha_
				sub		edx, eax
				jg		_SkipSpriteAheadContentLocalStart_
				neg		edx
				jmp		_SkipSpriteAheadContentEnd_

				_SkipSpriteAheadContentLocalAlpha_:
				{
					add		esi, eax
					sub		edx, eax
					jg		_SkipSpriteAheadContentLocalStart_
					add		esi, edx
					neg		edx
					jmp		_SkipSpriteAheadContentEnd_
				}
			}
		}
		_SkipSpriteAheadContentEnd_:

		mov		eax, nSprSkipPerLine
		or		eax, eax
		jnz		_DrawPartLineSection_	//if (nSprSkipPerLine) goto _DrawPartLineSection_

		//_DrawFullLineSection_:
		{
			//òò?asprite2??á??DD?1??￡??ò??DDμ?′?′|edx±??a0￡?è?sprite?á??DD?1???ò_DrawFullLineSection_Dè??			
			_DrawFullLineSection_Line_:
			{
				mov		edx, Clipper.width
				_DrawFullLineSection_LineLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax

					or		ebx, ebx
					jnz		_DrawFullLineSection_LineLocal_Alpha_
					lea     edi, [edi + eax * 4]
					sub		edx, eax
					jg		_DrawFullLineSection_LineLocal_

					add		edi, nBuffNextLine
					dec		Clipper.height
					jnz		_DrawFullLineSection_Line_
					jmp		_EXIT_WAY_
				
					_DrawFullLineSection_LineLocal_Alpha_:
					{
						sub		edx, eax
						mov		ecx, eax
						mov     ebx, pPalette
						_DrawFullLineSection_CopyPixel_:
						{
							copy_pixel_use_eax_32b
							dec ecx
							jnz	_DrawFullLineSection_CopyPixel_
						}
						or		edx, edx
						jnz		_DrawFullLineSection_LineLocal_

						add		edi, nBuffNextLine
						dec		Clipper.height
						jnz		_DrawFullLineSection_Line_
						jmp		_EXIT_WAY_
					}
				}
			}
		}

		_DrawPartLineSection_:
		{
			mov		eax, Clipper.left
			or		eax, eax
			jz		_DrawPartLineSection_SkipRight_Line_

			mov		eax, Clipper.right
			or		eax, eax
			jz		_DrawPartLineSection_SkipLeft_Line_
		}

		_DrawPartLineSection_Line_:
		{
			mov		eax, edx
			mov		edx, Clipper.width
			or		eax, eax
			jnz		_DrawPartLineSection_LineLocal_CheckAlpha_
			_DrawPartLineSection_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				_DrawPartLineSection_LineLocal_CheckAlpha_:
				or		ebx, ebx
				jnz		_DrawPartLineSection_LineLocal_Alpha_
				lea     edi, [edi + eax * 4]
				sub		edx, eax
				jg		_DrawPartLineSection_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_

				lea     edi, [edi + edx * 4]
				neg		edx
			}
			
			_DrawPartLineSection_LineSkip_:
			{
				add		edi, nBuffNextLine
				//ì?1ynSprSkipPerLine????μ?sprite?úèY
				mov		eax, edx
				mov		edx, nSprSkipPerLine
				or		eax, eax
				jnz		_DrawPartLineSection_LineSkipLocal_CheckAlpha_
				_DrawPartLineSection_LineSkipLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax
					
					_DrawPartLineSection_LineSkipLocal_CheckAlpha_:
					or		ebx, ebx
					jnz		_DrawPartLineSection_LineSkipLocal_Alpha_
					sub		edx, eax
					jg		_DrawPartLineSection_LineSkipLocal_
					neg		edx
					jmp		_DrawPartLineSection_Line_
					_DrawPartLineSection_LineSkipLocal_Alpha_:
					{
						add		esi, eax
						sub		edx, eax
						jg		_DrawPartLineSection_LineSkipLocal_
						add		esi, edx
						neg		edx
						jmp		_DrawPartLineSection_Line_
					}
				}
			}
			_DrawPartLineSection_LineLocal_Alpha_:
			{
				sub		edx, eax
				jle		_DrawPartLineSection_LineLocal_Alpha_Part_		//2??üè??-?aeax???àí?alpha?μμ???μ?￡?oó??óDμ?ò??-3?3???óò

				mov		ecx, eax
				mov     ebx, pPalette

				_DrawPartLineSection_CopyPixel_:
				{
					copy_pixel_use_eax_32b
					dec ecx
					jnz	_DrawPartLineSection_CopyPixel_
				}
				jmp		_DrawPartLineSection_LineLocal_
			}
			_DrawPartLineSection_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				mov     ebx, pPalette
				_DrawPartLineSection_CopyPixel_Part_:
				{
					copy_pixel_use_eax_32b
					dec ecx
					jnz	_DrawPartLineSection_CopyPixel_Part_
				}
			
				dec		Clipper.height
				jz		_EXIT_WAY_
				neg		edx
				mov		ebx, 255	//è?1???òaè·?Dμ??-ebx(alpha)?μ?éò??ú?°í·push ebx￡?′?′|pop??μ?
				jmp		_DrawPartLineSection_LineSkip_
			}
		}

		_DrawPartLineSection_SkipLeft_Line_:
		{
			mov		eax, edx
			mov		edx, Clipper.width
			or		eax, eax
			jnz		_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_
			_DrawPartLineSection_SkipLeft_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_:
				or		ebx, ebx
				jnz		_DrawPartLineSection_SkipLeft_LineLocal_Alpha_
				lea     edi, [edi + eax * 4]
				sub		edx, eax
				jg		_DrawPartLineSection_SkipLeft_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_
			}
			
			_DrawPartLineSection_SkipLeft_LineSkip_:
			{
				add		edi, nBuffNextLine
				//ì?1ynSprSkipPerLine????μ?sprite?úèY
				mov		edx, nSprSkipPerLine
				_DrawPartLineSection_SkipLeft_LineSkipLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax
					or		ebx, ebx
					jnz		_DrawPartLineSection_SkipLeft_LineSkipLocal_Alpha_
					sub		edx, eax
					jg		_DrawPartLineSection_SkipLeft_LineSkipLocal_
					neg		edx
					jmp		_DrawPartLineSection_SkipLeft_Line_
					_DrawPartLineSection_SkipLeft_LineSkipLocal_Alpha_:
					{
						add		esi, eax
						sub		edx, eax
						jg		_DrawPartLineSection_SkipLeft_LineSkipLocal_
						add		esi, edx
						neg		edx
						jmp		_DrawPartLineSection_SkipLeft_Line_
					}
				}
			}
			_DrawPartLineSection_SkipLeft_LineLocal_Alpha_:
			{
				sub		edx, eax
				mov		ecx, eax						
				mov     ebx, pPalette
				_DrawPartLineSection_SkipLeft_CopyPixel_:
				{
					copy_pixel_use_eax_32b
					dec ecx
					jnz	_DrawPartLineSection_SkipLeft_CopyPixel_
				}
				or		edx, edx
				jnz		_DrawPartLineSection_SkipLeft_LineLocal_
				dec		Clipper.height
				jg		_DrawPartLineSection_SkipLeft_LineSkip_
				jmp		_EXIT_WAY_
			}
		}

		_DrawPartLineSection_SkipRight_Line_:
		{
			mov		edx, Clipper.width
			_DrawPartLineSection_SkipRight_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				or		ebx, ebx
				jnz		_DrawPartLineSection_SkipRight_LineLocal_Alpha_
				lea     edi, [edi + eax * 4]
				sub		edx, eax
				jg		_DrawPartLineSection_SkipRight_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_

				lea     edi, [edi + edx * 4]
				neg		edx
			}
			
			_DrawPartLineSection_SkipRight_LineSkip_:
			{
				add		edi, nBuffNextLine
				//ì?1ynSprSkipPerLine????μ?sprite?úèY
				mov		eax, edx
				mov		edx, nSprSkipPerLine
				or		eax, eax
				jnz		_DrawPartLineSection_SkipRight_LineSkipLocal_CheckAlpha_
				_DrawPartLineSection_SkipRight_LineSkipLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax
					
					_DrawPartLineSection_SkipRight_LineSkipLocal_CheckAlpha_:
					or		ebx, ebx
					jnz		_DrawPartLineSection_SkipRight_LineSkipLocal_Alpha_
					sub		edx, eax
					jg		_DrawPartLineSection_SkipRight_LineSkipLocal_
					jmp		_DrawPartLineSection_SkipRight_Line_
					_DrawPartLineSection_SkipRight_LineSkipLocal_Alpha_:
					{
						add		esi, eax
						sub		edx, eax
						jg		_DrawPartLineSection_SkipRight_LineSkipLocal_
						jmp		_DrawPartLineSection_SkipRight_Line_
					}
				}
			}
			_DrawPartLineSection_SkipRight_LineLocal_Alpha_:
			{
				sub		edx, eax
				jle		_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_		//2??üè??-?aeax???àí?alpha?μμ???μ?￡?oó??óDμ?ò??-3?3???óò

				mov		ecx, eax				
				mov     ebx, pPalette
				_DrawPartLineSection_SkipRight_CopyPixel_:
				{
					copy_pixel_use_eax_32b
					dec ecx
					jnz	_DrawPartLineSection_SkipRight_CopyPixel_
				}
				jmp		_DrawPartLineSection_SkipRight_LineLocal_
			}
			_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				mov     ebx, pPalette
				_DrawPartLineSection_SkipRight_CopyPixel_Part_:
				{
					copy_pixel_use_eax_32b
					dec ecx
					jnz	_DrawPartLineSection_SkipRight_CopyPixel_Part_
				}
				neg		edx
				mov		ebx, 255
				dec		Clipper.height
				jg		_DrawPartLineSection_SkipRight_LineSkip_
				jmp		_EXIT_WAY_
			}
		}
		_EXIT_WAY_:
		emms
	}
}


void RIO_CopySprToBufferBlendColor(BYTE* pSprite, int nSprWidth, int nSprHeight, BYTE* pPalette,
	BYTE* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY, int nExAlpha, UINT nColor, int nMode)
{
	if(nExAlpha <= 0)
		return;
	KRClipperInfo Clipper;	
	if (RIO_ClipCopyRect(nX, nY, nSprWidth, nSprHeight, nBufferWidth, nBufferHeight, &Clipper) == 0)
		return;
	int nPitch = nBufferWidth * 4;
	pBuffer += Clipper.y * nPitch + Clipper.x * 4;
	int nBuffNextLine = nPitch - Clipper.width * 4;// next line add
	int nSprSkip = nSprWidth * Clipper.top + Clipper.left;
	int nSprSkipPerLine = Clipper.left + Clipper.right;
	UINT tmpAlpha, alpha, nSrcColor, nDstColor, R,G,B,A2;
	int nPixelBatch, nWidth;
	float sA,sR,sG,sB,dA,dR,dG,dB;
	if(!nSprSkip)
	goto	_SkipSpriteAheadContentEnd_;

	_SkipSpriteAheadContentLocalStart_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
		if(alpha)
			goto	_SkipSpriteAheadContentLocalAlpha_;
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
			goto	_SkipSpriteAheadContentLocalStart_;
		nSprSkip = -nSprSkip;
		goto	_SkipSpriteAheadContentEnd_;
	_SkipSpriteAheadContentLocalAlpha_:
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
		{
			pSprite += nPixelBatch;
			goto	_SkipSpriteAheadContentLocalStart_;
		}
		nSprSkip = -nSprSkip;
		pSprite += nPixelBatch - nSprSkip;
	_SkipSpriteAheadContentEnd_:
		if (nSprSkipPerLine)
			goto	_DrawPartLineSection_;
		
	_DrawFullLineSection_Line_:
		nWidth = Clipper.width;
	_DrawFullLineSection_LineLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
		if(alpha)
			goto	_DrawFullLineSection_LineLocal_Alpha_;
		pBuffer += nPixelBatch*4; //ph?n nμy kh?ng v?, ch? nh?y qua
		nWidth -= nPixelBatch;
		if(nWidth > 0)
			goto	_DrawFullLineSection_LineLocal_;
		pBuffer += nBuffNextLine;
		Clipper.height--;
		if(Clipper.height)
			goto	_DrawFullLineSection_Line_;
		goto	_EXIT_WAY_;
	_DrawFullLineSection_LineLocal_Alpha_:
		nWidth -= nPixelBatch;
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawFullLineSection_LineLocal_HalfAlpha_;
		while(nPixelBatch)
		{
			//v? 1 ?ióm víi alpha = 255
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nSrcColor = g_BlendColor16b(nSrcColor, nColor, nMode);
			*((UINT*)pBuffer) = nSrcColor | 0xff000000;
			pBuffer += 4;
			nPixelBatch--;
		}
		if(nWidth)
			goto	_DrawFullLineSection_LineLocal_;
		pBuffer += nBuffNextLine;
		Clipper.height--;
		if(Clipper.height)
			goto	_DrawFullLineSection_Line_;
		goto	_EXIT_WAY_;
	//v? 1 ?ióm víi alpha < 255
	_DrawFullLineSection_LineLocal_HalfAlpha_:
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nSrcColor = g_BlendColor16b(nSrcColor, nColor, nMode);
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = nSrcColor | (alpha << 24);
			}
			else
			{
				sR = (float)((nSrcColor & 0xf800) >> 8)/255.f;
				sG = (float)((nSrcColor & 0x07e0) >> 3)/255.f;
				sB = (float)((nSrcColor & 0x001f) << 3)/255.f;
				dR = (float)((nDstColor & 0xf800) >> 8)/255.f;
				dG = (float)((nDstColor & 0x07e0) >> 3)/255.f;
				dB = (float)((nDstColor & 0x001f) << 3)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		if(nWidth)
			goto	_DrawFullLineSection_LineLocal_;
		pBuffer += nBuffNextLine;
		Clipper.height--;
		if(Clipper.height)
			goto	_DrawFullLineSection_Line_;
		goto	_EXIT_WAY_;
	//b?t ??u v? mét ph?n c?a sprite
	_DrawPartLineSection_:
		if(!Clipper.left)
			goto	_DrawPartLineSection_SkipRight_Line_;
		if(!Clipper.right)
			goto	_DrawPartLineSection_SkipLeft_Line_;
	//ph?n sprite mêt lò tr?i vμ ph?i
	_DrawPartLineSection_Line_:
		nWidth = Clipper.width;
		if(nSprSkip)
		{
			nPixelBatch = nSprSkip;
			goto	_DrawPartLineSection_LineLocal_CheckAlpha_;
		}
	_DrawPartLineSection_LineLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
	_DrawPartLineSection_LineLocal_CheckAlpha_:
		if(alpha)
			goto	_DrawPartLineSection_LineLocal_Alpha_;
		nWidth -= nPixelBatch;
		if(nWidth > 0)
		{
			pBuffer += nPixelBatch*4;
			goto	_DrawPartLineSection_LineLocal_;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		pBuffer += (nPixelBatch + nWidth)*4;
		nWidth = -nWidth;//sè d- nPixelBatch c?n l1i v-?t qua nWidth, xuèng hμng
	
	_DrawPartLineSection_LineSkip_:
		pBuffer += nBuffNextLine;
		nSprSkip = nSprSkipPerLine;
		if(nWidth)
		{
			nPixelBatch = nWidth;
			goto	_DrawPartLineSection_LineSkipLocal_CheckAlpha_;
		}
	_DrawPartLineSection_LineSkipLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
	_DrawPartLineSection_LineSkipLocal_CheckAlpha_:
		if(alpha)
			goto	_DrawPartLineSection_LineSkipLocal_Alpha_;
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
			goto	_DrawPartLineSection_LineSkipLocal_;
		nSprSkip = -nSprSkip;
		goto	_DrawPartLineSection_Line_;
	_DrawPartLineSection_LineSkipLocal_Alpha_:
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
		{
			pSprite += nPixelBatch;
			goto	_DrawPartLineSection_LineSkipLocal_;
		}
		pSprite += nPixelBatch + nSprSkip;
		nSprSkip = -nSprSkip;
		goto	_DrawPartLineSection_Line_;
	_DrawPartLineSection_LineLocal_Alpha_:
		nWidth -= nPixelBatch;
		if(nWidth <= 0)
		{
			nWidth = -nWidth;
			nPixelBatch -= nWidth;
			goto	_DrawPartLineSection_LineLocal_Alpha_Part_;
		}
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawPartLineSection_LineLocal_HalfAlpha_;
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nSrcColor = g_BlendColor16b(nSrcColor, nColor, nMode);
			*((UINT*)pBuffer) = nSrcColor | 0xff000000;
			pBuffer += 4;
			nPixelBatch--;
		}
		goto	_DrawPartLineSection_LineLocal_;
	_DrawPartLineSection_LineLocal_HalfAlpha_:
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nSrcColor = g_BlendColor16b(nSrcColor, nColor, nMode);
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = nSrcColor | (alpha << 24);
			}
			else
			{
				sR = (float)((nSrcColor & 0xf800) >> 8)/255.f;
				sG = (float)((nSrcColor & 0x07e0) >> 3)/255.f;
				sB = (float)((nSrcColor & 0x001f) << 3)/255.f;
				dR = (float)((nDstColor & 0xf800) >> 8)/255.f;
				dG = (float)((nDstColor & 0x07e0) >> 3)/255.f;
				dB = (float)((nDstColor & 0x001f) << 3)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		goto	_DrawPartLineSection_LineLocal_;
	_DrawPartLineSection_LineLocal_Alpha_Part_:
		tmpAlpha = alpha;
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawPartLineSection_LineLocal_HalfAlpha_Part_;
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nSrcColor = g_BlendColor16b(nSrcColor, nColor, nMode);
			*((UINT*)pBuffer) = nSrcColor | 0xff000000;
			pBuffer += 4;
			nPixelBatch--;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		goto	_DrawPartLineSection_LineSkip_;
	_DrawPartLineSection_LineLocal_HalfAlpha_Part_:
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nSrcColor = g_BlendColor16b(nSrcColor, nColor, nMode);
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = nSrcColor | (alpha << 24);
			}
			else
			{
				sR = (float)((nSrcColor & 0xf800) >> 8)/255.f;
				sG = (float)((nSrcColor & 0x07e0) >> 3)/255.f;
				sB = (float)((nSrcColor & 0x001f) << 3)/255.f;
				dR = (float)((nDstColor & 0xf800) >> 8)/255.f;
				dG = (float)((nDstColor & 0x07e0) >> 3)/255.f;
				dB = (float)((nDstColor & 0x001f) << 3)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		alpha = tmpAlpha;
		goto	_DrawPartLineSection_LineSkip_;
	//ph?n sprite bT mêt ban tr?i
	_DrawPartLineSection_SkipLeft_Line_:
		nWidth = Clipper.width;
		if(nSprSkip)
		{
			nPixelBatch = nSprSkip;
			goto	_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_;
		}
	_DrawPartLineSection_SkipLeft_LineLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
	_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_:
		if(alpha)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_Alpha_;
		pBuffer += nPixelBatch*4; //ph?n nμy kh?ng v?, ch? nh?y qua
		nWidth -= nPixelBatch;
		if(nWidth > 0)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_;
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
	_DrawPartLineSection_SkipLeft_LineSkip_:
		pBuffer += nBuffNextLine;
		nSprSkip = nSprSkipPerLine;
	_DrawPartLineSection_SkipLeft_LineSkipLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
		if(alpha)
			goto	_DrawPartLineSection_SkipLeft_LineSkipLocal_Alpha_;
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
			goto	_DrawPartLineSection_SkipLeft_LineSkipLocal_;
		nSprSkip = -nSprSkip;
		goto	_DrawPartLineSection_SkipLeft_Line_;
	_DrawPartLineSection_SkipLeft_LineSkipLocal_Alpha_:
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
		{
			pSprite += nPixelBatch;
			goto	_DrawPartLineSection_SkipLeft_LineSkipLocal_;
		}
		pSprite += nPixelBatch + nSprSkip;
		nSprSkip = -nSprSkip;
		goto	_DrawPartLineSection_SkipLeft_Line_;
	_DrawPartLineSection_SkipLeft_LineLocal_Alpha_:
		nWidth -= nPixelBatch;
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_;
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nSrcColor = g_BlendColor16b(nSrcColor, nColor, nMode);
			*((UINT*)pBuffer) = nSrcColor | 0xff000000;
			pBuffer += 4;
			nPixelBatch--;
		}
		if(nWidth)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipLeft_LineSkip_;
		goto	_EXIT_WAY_;
	_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_:
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nSrcColor = g_BlendColor16b(nSrcColor, nColor, nMode);
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = nSrcColor | (alpha << 24);
			}
			else
			{
				sR = (float)((nSrcColor & 0xf800) >> 8)/255.f;
				sG = (float)((nSrcColor & 0x07e0) >> 3)/255.f;
				sB = (float)((nSrcColor & 0x001f) << 3)/255.f;
				dR = (float)((nDstColor & 0xf800) >> 8)/255.f;
				dG = (float)((nDstColor & 0x07e0) >> 3)/255.f;
				dB = (float)((nDstColor & 0x001f) << 3)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		if(nWidth)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipLeft_LineSkip_;
		goto	_EXIT_WAY_;
	//ph?n sprite bT mêt ban ph?i
	_DrawPartLineSection_SkipRight_Line_:
		nWidth = Clipper.width;
	_DrawPartLineSection_SkipRight_LineLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
		if(alpha)
			goto	_DrawPartLineSection_SkipRight_LineLocal_Alpha_;
		nWidth -= nPixelBatch;
		if(nWidth > 0)
		{
			pBuffer += nPixelBatch*4; //ph?n nμy kh?ng v?, ch? nh?y qua
			goto	_DrawPartLineSection_SkipRight_LineLocal_;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		pBuffer += (nPixelBatch + nWidth)*4;
		nWidth = -nWidth;
	_DrawPartLineSection_SkipRight_LineSkip_:
		pBuffer += nBuffNextLine;
		nSprSkip = nSprSkipPerLine;
		if(nWidth)
		{
			nPixelBatch = nWidth;
			goto	_DrawPartLineSection_SkipRight_LineSkipLocal_CheckAlpha_;
		}
	_DrawPartLineSection_SkipRight_LineSkipLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
	_DrawPartLineSection_SkipRight_LineSkipLocal_CheckAlpha_:
		if(alpha)
			goto	_DrawPartLineSection_SkipRight_LineSkipLocal_Alpha_;
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
			goto	_DrawPartLineSection_SkipRight_LineSkipLocal_;
		goto	_DrawPartLineSection_SkipRight_Line_;
	_DrawPartLineSection_SkipRight_LineSkipLocal_Alpha_:
		pSprite += nPixelBatch;
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
			goto	_DrawPartLineSection_SkipRight_LineSkipLocal_;
		goto	_DrawPartLineSection_SkipRight_Line_;
	_DrawPartLineSection_SkipRight_LineLocal_Alpha_:
		nWidth -= nPixelBatch;
		if(nWidth <= 0)
		{
			nPixelBatch += nWidth;
			goto	_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_;
		}
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_;
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nSrcColor = g_BlendColor16b(nSrcColor, nColor, nMode);
			*((UINT*)pBuffer) = nSrcColor | 0xff000000;
			pBuffer += 4;
			nPixelBatch--;
		}
		goto	_DrawPartLineSection_SkipRight_LineLocal_;
	_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_:
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nSrcColor = g_BlendColor16b(nSrcColor, nColor, nMode);
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = nSrcColor | (alpha << 24);
			}
			else
			{
				sR = (float)((nSrcColor & 0xf800) >> 8)/255.f;
				sG = (float)((nSrcColor & 0x07e0) >> 3)/255.f;
				sB = (float)((nSrcColor & 0x001f) << 3)/255.f;
				dR = (float)((nDstColor & 0xf800) >> 8)/255.f;
				dG = (float)((nDstColor & 0x07e0) >> 3)/255.f;
				dB = (float)((nDstColor & 0x001f) << 3)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		goto	_DrawPartLineSection_SkipRight_LineLocal_;
	_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_:
		tmpAlpha = alpha;
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_;
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nSrcColor = g_BlendColor16b(nSrcColor, nColor, nMode);
			*((UINT*)pBuffer) = nSrcColor | 0xff000000;
			pBuffer += 4;
			nPixelBatch--;
		}
		nWidth = -nWidth;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipRight_LineSkip_;
		goto	_EXIT_WAY_;
	_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_:
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nSrcColor = g_BlendColor16b(nSrcColor, nColor, nMode);
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = nSrcColor | (alpha << 24);
			}
			else
			{
				sR = (float)((nSrcColor & 0xf800) >> 8)/255.f;
				sG = (float)((nSrcColor & 0x07e0) >> 3)/255.f;
				sB = (float)((nSrcColor & 0x001f) << 3)/255.f;
				dR = (float)((nDstColor & 0xf800) >> 8)/255.f;
				dG = (float)((nDstColor & 0x07e0) >> 3)/255.f;
				dB = (float)((nDstColor & 0x001f) << 3)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		alpha = tmpAlpha;
		nWidth = -nWidth;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipRight_LineSkip_;
		goto	_EXIT_WAY_;
	
	_EXIT_WAY_:
	return;
}

void RIO_CopySprToBufferBlendColor32b(BYTE* pSprite, int nSprWidth, int nSprHeight, BYTE* pPalette,
	BYTE* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY, int nExAlpha, UINT nColor, int nMode)
{
	if(nExAlpha <= 0)
		return;
	KRClipperInfo Clipper;	
	if (RIO_ClipCopyRect(nX, nY, nSprWidth, nSprHeight, nBufferWidth, nBufferHeight, &Clipper) == 0)
		return;
	int nPitch = nBufferWidth * 4;
	pBuffer += Clipper.y * nPitch + Clipper.x * 4;
	int nBuffNextLine = nPitch - Clipper.width * 4;// next line add
	int nSprSkip = nSprWidth * Clipper.top + Clipper.left;
	int nSprSkipPerLine = Clipper.left + Clipper.right;
	UINT tmpAlpha, alpha, nSrcColor, nDstColor, R,G,B,A2;
	int nPixelBatch, nWidth;
	float sA,sR,sG,sB,dA,dR,dG,dB;
	if(!nSprSkip)
	goto	_SkipSpriteAheadContentEnd_;

	_SkipSpriteAheadContentLocalStart_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
		if(alpha)
			goto	_SkipSpriteAheadContentLocalAlpha_;
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
			goto	_SkipSpriteAheadContentLocalStart_;
		nSprSkip = -nSprSkip;
		goto	_SkipSpriteAheadContentEnd_;
	_SkipSpriteAheadContentLocalAlpha_:
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
		{
			pSprite += nPixelBatch;
			goto	_SkipSpriteAheadContentLocalStart_;
		}
		nSprSkip = -nSprSkip;
		pSprite += nPixelBatch - nSprSkip;
	_SkipSpriteAheadContentEnd_:
		if (nSprSkipPerLine)
			goto	_DrawPartLineSection_;
		
	_DrawFullLineSection_Line_:
		nWidth = Clipper.width;
	_DrawFullLineSection_LineLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
		if(alpha)
			goto	_DrawFullLineSection_LineLocal_Alpha_;
		pBuffer += nPixelBatch*4; //ph?n nμy kh?ng v?, ch? nh?y qua
		nWidth -= nPixelBatch;
		if(nWidth > 0)
			goto	_DrawFullLineSection_LineLocal_;
		pBuffer += nBuffNextLine;
		Clipper.height--;
		if(Clipper.height)
			goto	_DrawFullLineSection_Line_;
		goto	_EXIT_WAY_;
	_DrawFullLineSection_LineLocal_Alpha_:
		nWidth -= nPixelBatch;
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawFullLineSection_LineLocal_HalfAlpha_;
		while(nPixelBatch)
		{
			//v? 1 ?ióm víi alpha = 255
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			nSrcColor = g_BlendColor32b(R,G,B, nColor, nMode);
			*((UINT*)pBuffer) = 0xff000000 | nSrcColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		if(nWidth)
			goto	_DrawFullLineSection_LineLocal_;
		pBuffer += nBuffNextLine;
		Clipper.height--;
		if(Clipper.height)
			goto	_DrawFullLineSection_Line_;
		goto	_EXIT_WAY_;
	//v? 1 ?ióm víi alpha < 255
	_DrawFullLineSection_LineLocal_HalfAlpha_:
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			nSrcColor = g_BlendColor32b(R,G,B, nColor, nMode);
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = nSrcColor | (alpha << 24);
			}
			else
			{
				sR = (float)((nSrcColor & 0xff0000) >> 16)/255.f;
				sG = (float)((nSrcColor & 0xff00) >> 8)/255.f;
				sB = (float)(nSrcColor & 0xff)/255.f;
				dR = (float)((nDstColor & 0xff0000) >> 16)/255.f;
				dG = (float)((nDstColor & 0xff00) >> 8)/255.f;
				dB = (float)(nDstColor & 0xff)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | (R << 16) | (G << 8) | B;
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		if(nWidth)
			goto	_DrawFullLineSection_LineLocal_;
		pBuffer += nBuffNextLine;
		Clipper.height--;
		if(Clipper.height)
			goto	_DrawFullLineSection_Line_;
		goto	_EXIT_WAY_;
	//b?t ??u v? mét ph?n c?a sprite
	_DrawPartLineSection_:
		if(!Clipper.left)
			goto	_DrawPartLineSection_SkipRight_Line_;
		if(!Clipper.right)
			goto	_DrawPartLineSection_SkipLeft_Line_;
	//ph?n sprite mêt lò tr?i vμ ph?i
	_DrawPartLineSection_Line_:
		nWidth = Clipper.width;
		if(nSprSkip)
		{
			nPixelBatch = nSprSkip;
			goto	_DrawPartLineSection_LineLocal_CheckAlpha_;
		}
	_DrawPartLineSection_LineLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
	_DrawPartLineSection_LineLocal_CheckAlpha_:
		if(alpha)
			goto	_DrawPartLineSection_LineLocal_Alpha_;
		nWidth -= nPixelBatch;
		if(nWidth > 0)
		{
			pBuffer += nPixelBatch*4;
			goto	_DrawPartLineSection_LineLocal_;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		pBuffer += (nPixelBatch + nWidth)*4;
		nWidth = -nWidth;//sè d- nPixelBatch c?n l1i v-?t qua nWidth, xuèng hμng
	
	_DrawPartLineSection_LineSkip_:
		pBuffer += nBuffNextLine;
		nSprSkip = nSprSkipPerLine;
		if(nWidth)
		{
			nPixelBatch = nWidth;
			goto	_DrawPartLineSection_LineSkipLocal_CheckAlpha_;
		}
	_DrawPartLineSection_LineSkipLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
	_DrawPartLineSection_LineSkipLocal_CheckAlpha_:
		if(alpha)
			goto	_DrawPartLineSection_LineSkipLocal_Alpha_;
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
			goto	_DrawPartLineSection_LineSkipLocal_;
		nSprSkip = -nSprSkip;
		goto	_DrawPartLineSection_Line_;
	_DrawPartLineSection_LineSkipLocal_Alpha_:
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
		{
			pSprite += nPixelBatch;
			goto	_DrawPartLineSection_LineSkipLocal_;
		}
		pSprite += nPixelBatch + nSprSkip;
		nSprSkip = -nSprSkip;
		goto	_DrawPartLineSection_Line_;
	_DrawPartLineSection_LineLocal_Alpha_:
		nWidth -= nPixelBatch;
		if(nWidth <= 0)
		{
			nWidth = -nWidth;
			nPixelBatch -= nWidth;
			goto	_DrawPartLineSection_LineLocal_Alpha_Part_;
		}
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawPartLineSection_LineLocal_HalfAlpha_;
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			nSrcColor = g_BlendColor32b(R,G,B, nColor, nMode);
			*((UINT*)pBuffer) = 0xff000000 | nSrcColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		goto	_DrawPartLineSection_LineLocal_;
	_DrawPartLineSection_LineLocal_HalfAlpha_:
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			nSrcColor = g_BlendColor32b(R,G,B, nColor, nMode);
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = nSrcColor | (alpha << 24);
			}
			else
			{
				sR = (float)((nSrcColor & 0xff0000) >> 16)/255.f;
				sG = (float)((nSrcColor & 0xff00) >> 8)/255.f;
				sB = (float)(nSrcColor & 0xff)/255.f;
				dR = (float)((nDstColor & 0xff0000) >> 16)/255.f;
				dG = (float)((nDstColor & 0xff00) >> 8)/255.f;
				dB = (float)(nDstColor & 0xff)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | (R << 16) | (G << 8) | B;
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		goto	_DrawPartLineSection_LineLocal_;
	_DrawPartLineSection_LineLocal_Alpha_Part_:
		tmpAlpha = alpha;
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawPartLineSection_LineLocal_HalfAlpha_Part_;
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			nSrcColor = g_BlendColor32b(R,G,B, nColor, nMode);
			*((UINT*)pBuffer) = 0xff000000 | nSrcColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		goto	_DrawPartLineSection_LineSkip_;
	_DrawPartLineSection_LineLocal_HalfAlpha_Part_:
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			nSrcColor = g_BlendColor32b(R,G,B, nColor, nMode);
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = nSrcColor | (alpha << 24);
			}
			else
			{
				sR = (float)((nSrcColor & 0xff0000) >> 16)/255.f;
				sG = (float)((nSrcColor & 0xff00) >> 8)/255.f;
				sB = (float)(nSrcColor & 0xff)/255.f;
				dR = (float)((nDstColor & 0xff0000) >> 16)/255.f;
				dG = (float)((nDstColor & 0xff00) >> 8)/255.f;
				dB = (float)(nDstColor & 0xff)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | (R << 16) | (G << 8) | B;
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		alpha = tmpAlpha;
		goto	_DrawPartLineSection_LineSkip_;
	//ph?n sprite bT mêt ban tr?i
	_DrawPartLineSection_SkipLeft_Line_:
		nWidth = Clipper.width;
		if(nSprSkip)
		{
			nPixelBatch = nSprSkip;
			goto	_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_;
		}
	_DrawPartLineSection_SkipLeft_LineLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
	_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_:
		if(alpha)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_Alpha_;
		pBuffer += nPixelBatch*4; //ph?n nμy kh?ng v?, ch? nh?y qua
		nWidth -= nPixelBatch;
		if(nWidth > 0)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_;
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
	_DrawPartLineSection_SkipLeft_LineSkip_:
		pBuffer += nBuffNextLine;
		nSprSkip = nSprSkipPerLine;
	_DrawPartLineSection_SkipLeft_LineSkipLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
		if(alpha)
			goto	_DrawPartLineSection_SkipLeft_LineSkipLocal_Alpha_;
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
			goto	_DrawPartLineSection_SkipLeft_LineSkipLocal_;
		nSprSkip = -nSprSkip;
		goto	_DrawPartLineSection_SkipLeft_Line_;
	_DrawPartLineSection_SkipLeft_LineSkipLocal_Alpha_:
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
		{
			pSprite += nPixelBatch;
			goto	_DrawPartLineSection_SkipLeft_LineSkipLocal_;
		}
		pSprite += nPixelBatch + nSprSkip;
		nSprSkip = -nSprSkip;
		goto	_DrawPartLineSection_SkipLeft_Line_;
	_DrawPartLineSection_SkipLeft_LineLocal_Alpha_:
		nWidth -= nPixelBatch;
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_;
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			nSrcColor = g_BlendColor32b(R,G,B, nColor, nMode);
			*((UINT*)pBuffer) = 0xff000000 | nSrcColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		if(nWidth)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipLeft_LineSkip_;
		goto	_EXIT_WAY_;
	_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_:
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			nSrcColor = g_BlendColor32b(R,G,B, nColor, nMode);
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = nSrcColor | (alpha << 24);
			}
			else
			{
				sR = (float)((nSrcColor & 0xff0000) >> 16)/255.f;
				sG = (float)((nSrcColor & 0xff00) >> 8)/255.f;
				sB = (float)(nSrcColor & 0xff)/255.f;
				dR = (float)((nDstColor & 0xff0000) >> 16)/255.f;
				dG = (float)((nDstColor & 0xff00) >> 8)/255.f;
				dB = (float)(nDstColor & 0xff)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | (R << 16) | (G << 8) | B;
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		if(nWidth)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipLeft_LineSkip_;
		goto	_EXIT_WAY_;
	//ph?n sprite bT mêt ban ph?i
	_DrawPartLineSection_SkipRight_Line_:
		nWidth = Clipper.width;
	_DrawPartLineSection_SkipRight_LineLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
		if(alpha)
			goto	_DrawPartLineSection_SkipRight_LineLocal_Alpha_;
		nWidth -= nPixelBatch;
		if(nWidth > 0)
		{
			pBuffer += nPixelBatch*4; //ph?n nμy kh?ng v?, ch? nh?y qua
			goto	_DrawPartLineSection_SkipRight_LineLocal_;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		pBuffer += (nPixelBatch + nWidth)*4;
		nWidth = -nWidth;
	_DrawPartLineSection_SkipRight_LineSkip_:
		pBuffer += nBuffNextLine;
		nSprSkip = nSprSkipPerLine;
		if(nWidth)
		{
			nPixelBatch = nWidth;
			goto	_DrawPartLineSection_SkipRight_LineSkipLocal_CheckAlpha_;
		}
	_DrawPartLineSection_SkipRight_LineSkipLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
	_DrawPartLineSection_SkipRight_LineSkipLocal_CheckAlpha_:
		if(alpha)
			goto	_DrawPartLineSection_SkipRight_LineSkipLocal_Alpha_;
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
			goto	_DrawPartLineSection_SkipRight_LineSkipLocal_;
		goto	_DrawPartLineSection_SkipRight_Line_;
	_DrawPartLineSection_SkipRight_LineSkipLocal_Alpha_:
		pSprite += nPixelBatch;
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
			goto	_DrawPartLineSection_SkipRight_LineSkipLocal_;
		goto	_DrawPartLineSection_SkipRight_Line_;
	_DrawPartLineSection_SkipRight_LineLocal_Alpha_:
		nWidth -= nPixelBatch;
		if(nWidth <= 0)
		{
			nPixelBatch += nWidth;
			goto	_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_;
		}
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_;
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			nSrcColor = g_BlendColor32b(R,G,B, nColor, nMode);
			*((UINT*)pBuffer) = 0xff000000 | nSrcColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		goto	_DrawPartLineSection_SkipRight_LineLocal_;
	_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_:
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			nSrcColor = g_BlendColor32b(R,G,B, nColor, nMode);
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = nSrcColor | (alpha << 24);
			}
			else
			{
				sR = (float)((nSrcColor & 0xff0000) >> 16)/255.f;
				sG = (float)((nSrcColor & 0xff00) >> 8)/255.f;
				sB = (float)(nSrcColor & 0xff)/255.f;
				dR = (float)((nDstColor & 0xff0000) >> 16)/255.f;
				dG = (float)((nDstColor & 0xff00) >> 8)/255.f;
				dB = (float)(nDstColor & 0xff)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | (R << 16) | (G << 8) | B;
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		goto	_DrawPartLineSection_SkipRight_LineLocal_;
	_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_:
		tmpAlpha = alpha;
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_;
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			nSrcColor = g_BlendColor32b(R,G,B, nColor, nMode);
			*((UINT*)pBuffer) = 0xff000000 | nSrcColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		nWidth = -nWidth;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipRight_LineSkip_;
		goto	_EXIT_WAY_;
	_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_:
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			nSrcColor = g_BlendColor32b(R,G,B, nColor, nMode);
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = nSrcColor | (alpha << 24);
			}
			else
			{
				sR = (float)((nSrcColor & 0xff0000) >> 16)/255.f;
				sG = (float)((nSrcColor & 0xff00) >> 8)/255.f;
				sB = (float)(nSrcColor & 0xff)/255.f;
				dR = (float)((nDstColor & 0xff0000) >> 16)/255.f;
				dG = (float)((nDstColor & 0xff00) >> 8)/255.f;
				dB = (float)(nDstColor & 0xff)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | (R << 16) | (G << 8) | B;
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		alpha = tmpAlpha;
		nWidth = -nWidth;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipRight_LineSkip_;
		goto	_EXIT_WAY_;
	
	_EXIT_WAY_:
	return;
}

void RIO_CopySprToBufferAlpha(BYTE* pSprite, int nSprWidth, int nSprHeight, BYTE* pPalette,
			BYTE* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY, int nExAlpha)
{
	if(nExAlpha <= 0)
		return;
	KRClipperInfo Clipper;	
	if (RIO_ClipCopyRect(nX, nY, nSprWidth, nSprHeight, nBufferWidth, nBufferHeight, &Clipper) == 0)
		return;
	int nPitch = nBufferWidth * 4;
	pBuffer += Clipper.y * nPitch + Clipper.x * 4;
	int nBuffNextLine = nPitch - Clipper.width * 4;// next line add
	int nSprSkip = nSprWidth * Clipper.top + Clipper.left;
	int nSprSkipPerLine = Clipper.left + Clipper.right;
	UINT tmpAlpha, alpha, nSrcColor, nDstColor, R,G,B,A2;
	int nPixelBatch, nWidth;
	float sA,sR,sG,sB,dA,dR,dG,dB;
	if(!nSprSkip)
	goto	_SkipSpriteAheadContentEnd_;

	_SkipSpriteAheadContentLocalStart_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
		if(alpha)
			goto	_SkipSpriteAheadContentLocalAlpha_;
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
			goto	_SkipSpriteAheadContentLocalStart_;
		nSprSkip = -nSprSkip;
		goto	_SkipSpriteAheadContentEnd_;
	_SkipSpriteAheadContentLocalAlpha_:
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
		{
			pSprite += nPixelBatch;
			goto	_SkipSpriteAheadContentLocalStart_;
		}
		nSprSkip = -nSprSkip;
		pSprite += nPixelBatch - nSprSkip;
	_SkipSpriteAheadContentEnd_:
		if (nSprSkipPerLine)
			goto	_DrawPartLineSection_;
		
	_DrawFullLineSection_Line_:
		nWidth = Clipper.width;
	_DrawFullLineSection_LineLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
		if(alpha)
			goto	_DrawFullLineSection_LineLocal_Alpha_;
		pBuffer += nPixelBatch*4; //ph?n nμy kh?ng v?, ch? nh?y qua
		nWidth -= nPixelBatch;
		if(nWidth > 0)
			goto	_DrawFullLineSection_LineLocal_;
		pBuffer += nBuffNextLine;
		Clipper.height--;
		if(Clipper.height)
			goto	_DrawFullLineSection_Line_;
		goto	_EXIT_WAY_;
	_DrawFullLineSection_LineLocal_Alpha_:
		nWidth -= nPixelBatch;
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawFullLineSection_LineLocal_HalfAlpha_;
		while(nPixelBatch)
		{
			//v? 1 ?ióm víi alpha = 255
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			*((UINT*)pBuffer) = nSrcColor | 0xff000000;
			pBuffer += 4;
			nPixelBatch--;
		}
		if(nWidth)
			goto	_DrawFullLineSection_LineLocal_;
		pBuffer += nBuffNextLine;
		Clipper.height--;
		if(Clipper.height)
			goto	_DrawFullLineSection_Line_;
		goto	_EXIT_WAY_;
	//v? 1 ?ióm víi alpha < 255
	_DrawFullLineSection_LineLocal_HalfAlpha_:
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = nSrcColor | (alpha << 24);
			}
			else
			{
				sR = (float)((nSrcColor & 0xf800) >> 8)/255.f;
				sG = (float)((nSrcColor & 0x07e0) >> 3)/255.f;
				sB = (float)((nSrcColor & 0x001f) << 3)/255.f;
				dR = (float)((nDstColor & 0xf800) >> 8)/255.f;
				dG = (float)((nDstColor & 0x07e0) >> 3)/255.f;
				dB = (float)((nDstColor & 0x001f) << 3)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		if(nWidth)
			goto	_DrawFullLineSection_LineLocal_;
		pBuffer += nBuffNextLine;
		Clipper.height--;
		if(Clipper.height)
			goto	_DrawFullLineSection_Line_;
		goto	_EXIT_WAY_;
	//b?t ??u v? mét ph?n c?a sprite
	_DrawPartLineSection_:
		if(!Clipper.left)
			goto	_DrawPartLineSection_SkipRight_Line_;
		if(!Clipper.right)
			goto	_DrawPartLineSection_SkipLeft_Line_;
	//ph?n sprite mêt lò tr?i vμ ph?i
	_DrawPartLineSection_Line_:
		nWidth = Clipper.width;
		if(nSprSkip)
		{
			nPixelBatch = nSprSkip;
			goto	_DrawPartLineSection_LineLocal_CheckAlpha_;
		}
	_DrawPartLineSection_LineLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
	_DrawPartLineSection_LineLocal_CheckAlpha_:
		if(alpha)
			goto	_DrawPartLineSection_LineLocal_Alpha_;
		nWidth -= nPixelBatch;
		if(nWidth > 0)
		{
			pBuffer += nPixelBatch*4;
			goto	_DrawPartLineSection_LineLocal_;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		pBuffer += (nPixelBatch + nWidth)*4;
		nWidth = -nWidth;//sè d- nPixelBatch c?n l1i v-?t qua nWidth, xuèng hμng
	
	_DrawPartLineSection_LineSkip_:
		pBuffer += nBuffNextLine;
		nSprSkip = nSprSkipPerLine;
		if(nWidth)
		{
			nPixelBatch = nWidth;
			goto	_DrawPartLineSection_LineSkipLocal_CheckAlpha_;
		}
	_DrawPartLineSection_LineSkipLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
	_DrawPartLineSection_LineSkipLocal_CheckAlpha_:
		if(alpha)
			goto	_DrawPartLineSection_LineSkipLocal_Alpha_;
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
			goto	_DrawPartLineSection_LineSkipLocal_;
		nSprSkip = -nSprSkip;
		goto	_DrawPartLineSection_Line_;
	_DrawPartLineSection_LineSkipLocal_Alpha_:
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
		{
			pSprite += nPixelBatch;
			goto	_DrawPartLineSection_LineSkipLocal_;
		}
		pSprite += nPixelBatch + nSprSkip;
		nSprSkip = -nSprSkip;
		goto	_DrawPartLineSection_Line_;
	_DrawPartLineSection_LineLocal_Alpha_:
		nWidth -= nPixelBatch;
		if(nWidth <= 0)
		{
			nWidth = -nWidth;
			nPixelBatch -= nWidth;
			goto	_DrawPartLineSection_LineLocal_Alpha_Part_;
		}
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawPartLineSection_LineLocal_HalfAlpha_;
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			*((UINT*)pBuffer) = nSrcColor | 0xff000000;
			pBuffer += 4;
			nPixelBatch--;
		}
		goto	_DrawPartLineSection_LineLocal_;
	_DrawPartLineSection_LineLocal_HalfAlpha_:
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = nSrcColor | (alpha << 24);
			}
			else
			{
				sR = (float)((nSrcColor & 0xf800) >> 8)/255.f;
				sG = (float)((nSrcColor & 0x07e0) >> 3)/255.f;
				sB = (float)((nSrcColor & 0x001f) << 3)/255.f;
				dR = (float)((nDstColor & 0xf800) >> 8)/255.f;
				dG = (float)((nDstColor & 0x07e0) >> 3)/255.f;
				dB = (float)((nDstColor & 0x001f) << 3)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		goto	_DrawPartLineSection_LineLocal_;
	_DrawPartLineSection_LineLocal_Alpha_Part_:
		tmpAlpha = alpha;
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawPartLineSection_LineLocal_HalfAlpha_Part_;
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			*((UINT*)pBuffer) = nSrcColor | 0xff000000;
			pBuffer += 4;
			nPixelBatch--;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		goto	_DrawPartLineSection_LineSkip_;
	_DrawPartLineSection_LineLocal_HalfAlpha_Part_:
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = nSrcColor | (alpha << 24);
			}
			else
			{
				sR = (float)((nSrcColor & 0xf800) >> 8)/255.f;
				sG = (float)((nSrcColor & 0x07e0) >> 3)/255.f;
				sB = (float)((nSrcColor & 0x001f) << 3)/255.f;
				dR = (float)((nDstColor & 0xf800) >> 8)/255.f;
				dG = (float)((nDstColor & 0x07e0) >> 3)/255.f;
				dB = (float)((nDstColor & 0x001f) << 3)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		alpha = tmpAlpha;
		goto	_DrawPartLineSection_LineSkip_;
	//ph?n sprite bT mêt ban tr?i
	_DrawPartLineSection_SkipLeft_Line_:
		nWidth = Clipper.width;
		if(nSprSkip)
		{
			nPixelBatch = nSprSkip;
			goto	_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_;
		}
	_DrawPartLineSection_SkipLeft_LineLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
	_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_:
		if(alpha)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_Alpha_;
		pBuffer += nPixelBatch*4; //ph?n nμy kh?ng v?, ch? nh?y qua
		nWidth -= nPixelBatch;
		if(nWidth > 0)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_;
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
	_DrawPartLineSection_SkipLeft_LineSkip_:
		pBuffer += nBuffNextLine;
		nSprSkip = nSprSkipPerLine;
	_DrawPartLineSection_SkipLeft_LineSkipLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
		if(alpha)
			goto	_DrawPartLineSection_SkipLeft_LineSkipLocal_Alpha_;
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
			goto	_DrawPartLineSection_SkipLeft_LineSkipLocal_;
		nSprSkip = -nSprSkip;
		goto	_DrawPartLineSection_SkipLeft_Line_;
	_DrawPartLineSection_SkipLeft_LineSkipLocal_Alpha_:
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
		{
			pSprite += nPixelBatch;
			goto	_DrawPartLineSection_SkipLeft_LineSkipLocal_;
		}
		pSprite += nPixelBatch + nSprSkip;
		nSprSkip = -nSprSkip;
		goto	_DrawPartLineSection_SkipLeft_Line_;
	_DrawPartLineSection_SkipLeft_LineLocal_Alpha_:
		nWidth -= nPixelBatch;
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_;
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			*((UINT*)pBuffer) = nSrcColor | 0xff000000;
			pBuffer += 4;
			nPixelBatch--;
		}
		if(nWidth)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipLeft_LineSkip_;
		goto	_EXIT_WAY_;
	_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_:
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = nSrcColor | (alpha << 24);
			}
			else
			{
				sR = (float)((nSrcColor & 0xf800) >> 8)/255.f;
				sG = (float)((nSrcColor & 0x07e0) >> 3)/255.f;
				sB = (float)((nSrcColor & 0x001f) << 3)/255.f;
				dR = (float)((nDstColor & 0xf800) >> 8)/255.f;
				dG = (float)((nDstColor & 0x07e0) >> 3)/255.f;
				dB = (float)((nDstColor & 0x001f) << 3)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		if(nWidth)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipLeft_LineSkip_;
		goto	_EXIT_WAY_;
	//ph?n sprite bT mêt ban ph?i
	_DrawPartLineSection_SkipRight_Line_:
		nWidth = Clipper.width;
	_DrawPartLineSection_SkipRight_LineLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
		if(alpha)
			goto	_DrawPartLineSection_SkipRight_LineLocal_Alpha_;
		nWidth -= nPixelBatch;
		if(nWidth > 0)
		{
			pBuffer += nPixelBatch*4; //ph?n nμy kh?ng v?, ch? nh?y qua
			goto	_DrawPartLineSection_SkipRight_LineLocal_;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		pBuffer += (nPixelBatch + nWidth)*4;
		nWidth = -nWidth;
	_DrawPartLineSection_SkipRight_LineSkip_:
		pBuffer += nBuffNextLine;
		nSprSkip = nSprSkipPerLine;
		if(nWidth)
		{
			nPixelBatch = nWidth;
			goto	_DrawPartLineSection_SkipRight_LineSkipLocal_CheckAlpha_;
		}
	_DrawPartLineSection_SkipRight_LineSkipLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
	_DrawPartLineSection_SkipRight_LineSkipLocal_CheckAlpha_:
		if(alpha)
			goto	_DrawPartLineSection_SkipRight_LineSkipLocal_Alpha_;
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
			goto	_DrawPartLineSection_SkipRight_LineSkipLocal_;
		goto	_DrawPartLineSection_SkipRight_Line_;
	_DrawPartLineSection_SkipRight_LineSkipLocal_Alpha_:
		pSprite += nPixelBatch;
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
			goto	_DrawPartLineSection_SkipRight_LineSkipLocal_;
		goto	_DrawPartLineSection_SkipRight_Line_;
	_DrawPartLineSection_SkipRight_LineLocal_Alpha_:
		nWidth -= nPixelBatch;
		if(nWidth <= 0)
		{
			nPixelBatch += nWidth;
			goto	_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_;
		}
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_;
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			*((UINT*)pBuffer) = nSrcColor | 0xff000000;
			pBuffer += 4;
			nPixelBatch--;
		}
		goto	_DrawPartLineSection_SkipRight_LineLocal_;
	_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_:
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = nSrcColor | (alpha << 24);
			}
			else
			{
				sR = (float)((nSrcColor & 0xf800) >> 8)/255.f;
				sG = (float)((nSrcColor & 0x07e0) >> 3)/255.f;
				sB = (float)((nSrcColor & 0x001f) << 3)/255.f;
				dR = (float)((nDstColor & 0xf800) >> 8)/255.f;
				dG = (float)((nDstColor & 0x07e0) >> 3)/255.f;
				dB = (float)((nDstColor & 0x001f) << 3)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		goto	_DrawPartLineSection_SkipRight_LineLocal_;
	_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_:
		tmpAlpha = alpha;
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_;
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			*((UINT*)pBuffer) = nSrcColor | 0xff000000;
			pBuffer += 4;
			nPixelBatch--;
		}
		nWidth = -nWidth;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipRight_LineSkip_;
		goto	_EXIT_WAY_;
	_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_:
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = nSrcColor | (alpha << 24);
			}
			else
			{
				sR = (float)((nSrcColor & 0xf800) >> 8)/255.f;
				sG = (float)((nSrcColor & 0x07e0) >> 3)/255.f;
				sB = (float)((nSrcColor & 0x001f) << 3)/255.f;
				dR = (float)((nDstColor & 0xf800) >> 8)/255.f;
				dG = (float)((nDstColor & 0x07e0) >> 3)/255.f;
				dB = (float)((nDstColor & 0x001f) << 3)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		alpha = tmpAlpha;
		nWidth = -nWidth;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipRight_LineSkip_;
		goto	_EXIT_WAY_;
	
	_EXIT_WAY_:
	return;
}

void RIO_CopySprToBufferAlpha32b(BYTE* pSprite, int nSprWidth, int nSprHeight, BYTE* pPalette,
			BYTE* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY, int nExAlpha)
{
	if(nExAlpha <= 0)
		return;
	KRClipperInfo Clipper;	
	if (RIO_ClipCopyRect(nX, nY, nSprWidth, nSprHeight, nBufferWidth, nBufferHeight, &Clipper) == 0)
		return;
	int nPitch = nBufferWidth * 4;
	pBuffer += Clipper.y * nPitch + Clipper.x * 4;
	int nBuffNextLine = nPitch - Clipper.width * 4;// next line add
	int nSprSkip = nSprWidth * Clipper.top + Clipper.left;
	int nSprSkipPerLine = Clipper.left + Clipper.right;
	UINT tmpAlpha, alpha, nDstColor, R,G,B,A2;
	int nPixelBatch, nWidth;
	float sA,sR,sG,sB,dA,dR,dG,dB;
	if(!nSprSkip)
	goto	_SkipSpriteAheadContentEnd_;

	_SkipSpriteAheadContentLocalStart_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
		if(alpha)
			goto	_SkipSpriteAheadContentLocalAlpha_;
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
			goto	_SkipSpriteAheadContentLocalStart_;
		nSprSkip = -nSprSkip;
		goto	_SkipSpriteAheadContentEnd_;
	_SkipSpriteAheadContentLocalAlpha_:
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
		{
			pSprite += nPixelBatch;
			goto	_SkipSpriteAheadContentLocalStart_;
		}
		nSprSkip = -nSprSkip;
		pSprite += nPixelBatch - nSprSkip;
	_SkipSpriteAheadContentEnd_:
		if (nSprSkipPerLine)
			goto	_DrawPartLineSection_;
		
	_DrawFullLineSection_Line_:
		nWidth = Clipper.width;
	_DrawFullLineSection_LineLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
		if(alpha)
			goto	_DrawFullLineSection_LineLocal_Alpha_;
		pBuffer += nPixelBatch*4; //ph?n nμy kh?ng v?, ch? nh?y qua
		nWidth -= nPixelBatch;
		if(nWidth > 0)
			goto	_DrawFullLineSection_LineLocal_;
		pBuffer += nBuffNextLine;
		Clipper.height--;
		if(Clipper.height)
			goto	_DrawFullLineSection_Line_;
		goto	_EXIT_WAY_;
	_DrawFullLineSection_LineLocal_Alpha_:
		nWidth -= nPixelBatch;
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawFullLineSection_LineLocal_HalfAlpha_;
		while(nPixelBatch)
		{
			//v? 1 ?ióm víi alpha = 255
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			*((UINT*)pBuffer) = 0xff000000 | (R << 16) | (G << 8) | B;
			pBuffer += 4;
			nPixelBatch--;
		}
		if(nWidth)
			goto	_DrawFullLineSection_LineLocal_;
		pBuffer += nBuffNextLine;
		Clipper.height--;
		if(Clipper.height)
			goto	_DrawFullLineSection_Line_;
		goto	_EXIT_WAY_;
	//v? 1 ?ióm víi alpha < 255
	_DrawFullLineSection_LineLocal_HalfAlpha_:
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = (alpha << 24) | (R << 16) | (G << 8) | B;
			}
			else
			{
				sR = (float)R/255.f;
				sG = (float)G/255.f;
				sB = (float)B/255.f;
				dR = (float)((nDstColor & 0xff0000) >> 16)/255.f;
				dG = (float)((nDstColor & 0xff00) >> 8)/255.f;
				dB = (float)(nDstColor & 0xff)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | (R << 16) | (G << 8) | B;
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		if(nWidth)
			goto	_DrawFullLineSection_LineLocal_;
		pBuffer += nBuffNextLine;
		Clipper.height--;
		if(Clipper.height)
			goto	_DrawFullLineSection_Line_;
		goto	_EXIT_WAY_;
	//b?t ??u v? mét ph?n c?a sprite
	_DrawPartLineSection_:
		if(!Clipper.left)
			goto	_DrawPartLineSection_SkipRight_Line_;
		if(!Clipper.right)
			goto	_DrawPartLineSection_SkipLeft_Line_;
	//ph?n sprite mêt lò tr?i vμ ph?i
	_DrawPartLineSection_Line_:
		nWidth = Clipper.width;
		if(nSprSkip)
		{
			nPixelBatch = nSprSkip;
			goto	_DrawPartLineSection_LineLocal_CheckAlpha_;
		}
	_DrawPartLineSection_LineLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
	_DrawPartLineSection_LineLocal_CheckAlpha_:
		if(alpha)
			goto	_DrawPartLineSection_LineLocal_Alpha_;
		nWidth -= nPixelBatch;
		if(nWidth > 0)
		{
			pBuffer += nPixelBatch*4;
			goto	_DrawPartLineSection_LineLocal_;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		pBuffer += (nPixelBatch + nWidth)*4;
		nWidth = -nWidth;//sè d- nPixelBatch c?n l1i v-?t qua nWidth, xuèng hμng
	
	_DrawPartLineSection_LineSkip_:
		pBuffer += nBuffNextLine;
		nSprSkip = nSprSkipPerLine;
		if(nWidth)
		{
			nPixelBatch = nWidth;
			goto	_DrawPartLineSection_LineSkipLocal_CheckAlpha_;
		}
	_DrawPartLineSection_LineSkipLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
	_DrawPartLineSection_LineSkipLocal_CheckAlpha_:
		if(alpha)
			goto	_DrawPartLineSection_LineSkipLocal_Alpha_;
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
			goto	_DrawPartLineSection_LineSkipLocal_;
		nSprSkip = -nSprSkip;
		goto	_DrawPartLineSection_Line_;
	_DrawPartLineSection_LineSkipLocal_Alpha_:
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
		{
			pSprite += nPixelBatch;
			goto	_DrawPartLineSection_LineSkipLocal_;
		}
		pSprite += nPixelBatch + nSprSkip;
		nSprSkip = -nSprSkip;
		goto	_DrawPartLineSection_Line_;
	_DrawPartLineSection_LineLocal_Alpha_:
		nWidth -= nPixelBatch;
		if(nWidth <= 0)
		{
			nWidth = -nWidth;
			nPixelBatch -= nWidth;
			goto	_DrawPartLineSection_LineLocal_Alpha_Part_;
		}
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawPartLineSection_LineLocal_HalfAlpha_;
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			*((UINT*)pBuffer) = 0xff000000 | (R << 16) | (G << 8) | B;
			pBuffer += 4;
			nPixelBatch--;
		}
		goto	_DrawPartLineSection_LineLocal_;
	_DrawPartLineSection_LineLocal_HalfAlpha_:
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = (alpha << 24) | (R << 16) | (G << 8) | B;
			}
			else
			{
				sR = (float)R/255.f;
				sG = (float)G/255.f;
				sB = (float)B/255.f;
				dR = (float)((nDstColor & 0xff0000) >> 16)/255.f;
				dG = (float)((nDstColor & 0xff00) >> 8)/255.f;
				dB = (float)(nDstColor & 0xff)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | (R << 16) | (G << 8) | B;
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		goto	_DrawPartLineSection_LineLocal_;
	_DrawPartLineSection_LineLocal_Alpha_Part_:
		tmpAlpha = alpha;
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawPartLineSection_LineLocal_HalfAlpha_Part_;
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			*((UINT*)pBuffer) = 0xff000000 | (R << 16) | (G << 8) | B;
			pBuffer += 4;
			nPixelBatch--;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		goto	_DrawPartLineSection_LineSkip_;
	_DrawPartLineSection_LineLocal_HalfAlpha_Part_:
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = (alpha << 24) | (R << 16) | (G << 8) | B;
			}
			else
			{
				sR = (float)R/255.f;
				sG = (float)G/255.f;
				sB = (float)B/255.f;
				dR = (float)((nDstColor & 0xff0000) >> 16)/255.f;
				dG = (float)((nDstColor & 0xff00) >> 8)/255.f;
				dB = (float)(nDstColor & 0xff)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | (R << 16) | (G << 8) | B;
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		alpha = tmpAlpha;
		goto	_DrawPartLineSection_LineSkip_;
	//ph?n sprite bT mêt ban tr?i
	_DrawPartLineSection_SkipLeft_Line_:
		nWidth = Clipper.width;
		if(nSprSkip)
		{
			nPixelBatch = nSprSkip;
			goto	_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_;
		}
	_DrawPartLineSection_SkipLeft_LineLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
	_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_:
		if(alpha)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_Alpha_;
		pBuffer += nPixelBatch*4; //ph?n nμy kh?ng v?, ch? nh?y qua
		nWidth -= nPixelBatch;
		if(nWidth > 0)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_;
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
	_DrawPartLineSection_SkipLeft_LineSkip_:
		pBuffer += nBuffNextLine;
		nSprSkip = nSprSkipPerLine;
	_DrawPartLineSection_SkipLeft_LineSkipLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
		if(alpha)
			goto	_DrawPartLineSection_SkipLeft_LineSkipLocal_Alpha_;
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
			goto	_DrawPartLineSection_SkipLeft_LineSkipLocal_;
		nSprSkip = -nSprSkip;
		goto	_DrawPartLineSection_SkipLeft_Line_;
	_DrawPartLineSection_SkipLeft_LineSkipLocal_Alpha_:
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
		{
			pSprite += nPixelBatch;
			goto	_DrawPartLineSection_SkipLeft_LineSkipLocal_;
		}
		pSprite += nPixelBatch + nSprSkip;
		nSprSkip = -nSprSkip;
		goto	_DrawPartLineSection_SkipLeft_Line_;
	_DrawPartLineSection_SkipLeft_LineLocal_Alpha_:
		nWidth -= nPixelBatch;
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_;
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			*((UINT*)pBuffer) = 0xff000000 | (R << 16) | (G << 8) | B;
			pBuffer += 4;
			nPixelBatch--;
		}
		if(nWidth)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipLeft_LineSkip_;
		goto	_EXIT_WAY_;
	_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_:
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = (alpha << 24) | (R << 16) | (G << 8) | B;
			}
			else
			{
				sR = (float)R/255.f;
				sG = (float)G/255.f;
				sB = (float)B/255.f;
				dR = (float)((nDstColor & 0xff0000) >> 16)/255.f;
				dG = (float)((nDstColor & 0xff00) >> 8)/255.f;
				dB = (float)(nDstColor & 0xff)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | (R << 16) | (G << 8) | B;
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		if(nWidth)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipLeft_LineSkip_;
		goto	_EXIT_WAY_;
	//ph?n sprite bT mêt ban ph?i
	_DrawPartLineSection_SkipRight_Line_:
		nWidth = Clipper.width;
	_DrawPartLineSection_SkipRight_LineLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
		if(alpha)
			goto	_DrawPartLineSection_SkipRight_LineLocal_Alpha_;
		nWidth -= nPixelBatch;
		if(nWidth > 0)
		{
			pBuffer += nPixelBatch*4; //ph?n nμy kh?ng v?, ch? nh?y qua
			goto	_DrawPartLineSection_SkipRight_LineLocal_;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		pBuffer += (nPixelBatch + nWidth)*4;
		nWidth = -nWidth;
	_DrawPartLineSection_SkipRight_LineSkip_:
		pBuffer += nBuffNextLine;
		nSprSkip = nSprSkipPerLine;
		if(nWidth)
		{
			nPixelBatch = nWidth;
			goto	_DrawPartLineSection_SkipRight_LineSkipLocal_CheckAlpha_;
		}
	_DrawPartLineSection_SkipRight_LineSkipLocal_:
		nPixelBatch = *(pSprite++);
		alpha = *(pSprite++);
	_DrawPartLineSection_SkipRight_LineSkipLocal_CheckAlpha_:
		if(alpha)
			goto	_DrawPartLineSection_SkipRight_LineSkipLocal_Alpha_;
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
			goto	_DrawPartLineSection_SkipRight_LineSkipLocal_;
		goto	_DrawPartLineSection_SkipRight_Line_;
	_DrawPartLineSection_SkipRight_LineSkipLocal_Alpha_:
		pSprite += nPixelBatch;
		nSprSkip -= nPixelBatch;
		if(nSprSkip > 0)
			goto	_DrawPartLineSection_SkipRight_LineSkipLocal_;
		goto	_DrawPartLineSection_SkipRight_Line_;
	_DrawPartLineSection_SkipRight_LineLocal_Alpha_:
		nWidth -= nPixelBatch;
		if(nWidth <= 0)
		{
			nPixelBatch += nWidth;
			goto	_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_;
		}
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_;
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			*((UINT*)pBuffer) = 0xff000000 | (R << 16) | (G << 8) | B;
			pBuffer += 4;
			nPixelBatch--;
		}
		goto	_DrawPartLineSection_SkipRight_LineLocal_;
	_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_:
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = (alpha << 24) | (R << 16) | (G << 8) | B;
			}
			else
			{
				sR = (float)R/255.f;
				sG = (float)G/255.f;
				sB = (float)B/255.f;
				dR = (float)((nDstColor & 0xff0000) >> 16)/255.f;
				dG = (float)((nDstColor & 0xff00) >> 8)/255.f;
				dB = (float)(nDstColor & 0xff)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | (R << 16) | (G << 8) | B;
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		goto	_DrawPartLineSection_SkipRight_LineLocal_;
	_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_:
		tmpAlpha = alpha;
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_;
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			*((UINT*)pBuffer) = 0xff000000 | (R << 16) | (G << 8) | B;
			pBuffer += 4;
			nPixelBatch--;
		}
		nWidth = -nWidth;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipRight_LineSkip_;
		goto	_EXIT_WAY_;
	_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_:
		while(nPixelBatch)
		{
			R = *(pPalette + 3*(*pSprite));
			G = *(pPalette + 3*(*pSprite) + 1);
			B = *(pPalette + 3*(*pSprite) + 2);
			pSprite++;
			nDstColor = *((UINT*)pBuffer);			
			A2 = (nDstColor & 0xff000000) >> 24;
			if(A2 == 0)
			{
				nDstColor = (alpha << 24) | (R << 16) | (G << 8) | B;
			}
			else
			{
				sR = (float)R/255.f;
				sG = (float)G/255.f;
				sB = (float)B/255.f;
				dR = (float)((nDstColor & 0xff0000) >> 16)/255.f;
				dG = (float)((nDstColor & 0xff00) >> 8)/255.f;
				dB = (float)(nDstColor & 0xff)/255.f;
				sA = (float)alpha/255.f;
				dA = (float)A2/255.f;
				dR = sR*sA + dR*dA*(1.f-sA);
				dG = sG*sA + dG*dA*(1.f-sA);
				dB = sB*sA + dB*dA*(1.f-sA);
				dA = sA + dA*(1.f-sA);
				dR /= dA;
				dG /= dA;
				dB /= dA;
				R = (UINT)(dR*255.f);
				G = (UINT)(dG*255.f);
				B = (UINT)(dB*255.f);
				A2 = (UINT)(dA*255.f);
				nDstColor = (A2 << 24) | (R << 16) | (G << 8) | B;
			}
			*((UINT*)pBuffer) = nDstColor;
			pBuffer += 4;
			nPixelBatch--;
		}
		alpha = tmpAlpha;
		nWidth = -nWidth;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipRight_LineSkip_;
		goto	_EXIT_WAY_;
	
	_EXIT_WAY_:
	return;
}


void RIO_CopyBitmap16ToBuffer(WORD* pBitmap, int nBmpWidth, int nBmpHeight,
					 BYTE* pBuffer, int nBufferWidth, int nBufferHeight,
					 int nX, int nY, int nExAlpha)
{
	if(nExAlpha <= 0)
		return;
	KRClipperInfo Clipper;
	if (RIO_ClipCopyRect(nX, nY, nBmpWidth, nBmpHeight, nBufferWidth, nBufferHeight, &Clipper) == 0)
		return;
	int nPitch = nBufferWidth * 4;
	pBuffer += Clipper.y * nPitch + Clipper.x * 4;
	int nBuffNextLine = nPitch - Clipper.width * 4;// next line add
	pBitmap += nBmpWidth * Clipper.top + Clipper.left;
	int nSprSkipPerLine = Clipper.left + Clipper.right;
	UINT nSrcColor, nDstColor, R,G,B,A2;
	int nWidth;
	float sA,sR,sG,sB,dA,dR,dG,dB;
		
	_DrawFullLineSection_Line_:
		nWidth = Clipper.width;
	_DrawFullLineSection_LineLocal_:
		nWidth--;
		if(nExAlpha < 255)
			goto	_DrawFullLineSection_LineLocal_HalfAlpha_;
		nSrcColor = *(pBitmap++);
		*((UINT*)pBuffer) = nSrcColor | 0xff000000;
		pBuffer += 4;
		if(nWidth)
			goto	_DrawFullLineSection_LineLocal_;
		pBuffer += nBuffNextLine;
		Clipper.height--;
		if(Clipper.height)
		{
			pBitmap += nSprSkipPerLine;
			goto	_DrawFullLineSection_Line_;
		}
		goto	_EXIT_WAY_;
	//v? 1 ?ióm víi alpha < 255
	_DrawFullLineSection_LineLocal_HalfAlpha_:
		nSrcColor = *(pBitmap++);
		nDstColor = *((UINT*)pBuffer);
		A2 = (nDstColor & 0xff000000) >> 24;
		if(A2 == 0)
		{
			nDstColor = nSrcColor | (nExAlpha << 24);
		}
		else
		{
			sR = (float)((nSrcColor & 0xf800) >> 8)/255.f;
			sG = (float)((nSrcColor & 0x07e0) >> 3)/255.f;
			sB = (float)((nSrcColor & 0x001f) << 3)/255.f;
			dR = (float)((nDstColor & 0xf800) >> 8)/255.f;
			dG = (float)((nDstColor & 0x07e0) >> 3)/255.f;
			dB = (float)((nDstColor & 0x001f) << 3)/255.f;
			sA = (float)nExAlpha/255.f;
			dA = (float)A2/255.f;
			dR = sR*sA + dR*dA*(1.f-sA);
			dG = sG*sA + dG*dA*(1.f-sA);
			dB = sB*sA + dB*dA*(1.f-sA);
			dA = sA + dA*(1.f-sA);
			dR /= dA;
			dG /= dA;
			dB /= dA;
			R = (UINT)(dR*255.f);
			G = (UINT)(dG*255.f);
			B = (UINT)(dB*255.f);
			A2 = (UINT)(dA*255.f);
			nDstColor = (A2 << 24) | ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
		}
		*((UINT*)pBuffer) = nDstColor;
		pBuffer += 4;
		if(nWidth)
			goto	_DrawFullLineSection_LineLocal_;
		pBuffer += nBuffNextLine;
		Clipper.height--;
		if(Clipper.height)
		{
			pBitmap += nSprSkipPerLine;
			goto	_DrawFullLineSection_Line_;
		}
		goto	_EXIT_WAY_;
	
	_EXIT_WAY_:
	return;
}

void RIO_CopyBitmap16ToBuffer32b(WORD* pBitmap, int nBmpWidth, int nBmpHeight,
					 BYTE* pBuffer, int nBufferWidth, int nBufferHeight,
					 int nX, int nY, int nExAlpha)
{
	if(nExAlpha <= 0)
		return;
	KRClipperInfo Clipper;
	if (RIO_ClipCopyRect(nX, nY, nBmpWidth, nBmpHeight, nBufferWidth, nBufferHeight, &Clipper) == 0)
		return;
	int nPitch = nBufferWidth * 4;
	pBuffer += Clipper.y * nPitch + Clipper.x * 4;
	int nBuffNextLine = nPitch - Clipper.width * 4;// next line add
	pBitmap += nBmpWidth * Clipper.top + Clipper.left;
	int nSprSkipPerLine = Clipper.left + Clipper.right;
	UINT nSrcColor, nDstColor, R,G,B,A2;
	int nWidth;
	float sA,sR,sG,sB,dA,dR,dG,dB;
		
	_DrawFullLineSection_Line_:
		nWidth = Clipper.width;
	_DrawFullLineSection_LineLocal_:
		nWidth--;
		if(nExAlpha < 255)
			goto	_DrawFullLineSection_LineLocal_HalfAlpha_;
		nSrcColor = *(pBitmap++);
		R = (nSrcColor & 0xf800) >> 8;
		G = (nSrcColor & 0x07e0) >> 3;
		B = (nSrcColor & 0x001f) << 3;
		*((UINT*)pBuffer) = 0xff000000 | (R << 16) | (G << 8) | B;
		pBuffer += 4;
		if(nWidth)
			goto	_DrawFullLineSection_LineLocal_;
		pBuffer += nBuffNextLine;
		Clipper.height--;
		if(Clipper.height)
		{
			pBitmap += nSprSkipPerLine;
			goto	_DrawFullLineSection_Line_;
		}
		goto	_EXIT_WAY_;
	//v? 1 ?ióm víi alpha < 255
	_DrawFullLineSection_LineLocal_HalfAlpha_:
		nSrcColor = *(pBitmap++);
		R = (nSrcColor & 0xf800) >> 8;
		G = (nSrcColor & 0x07e0) >> 3;
		B = (nSrcColor & 0x001f) << 3;
		nDstColor = *((UINT*)pBuffer);
		A2 = (nDstColor & 0xff000000) >> 24;
		if(A2 == 0)
		{
			nDstColor = (nExAlpha << 24) | (R << 16) | (G << 8) | B;
		}
		else
		{
			sR = (float)R/255.f;
			sG = (float)G/255.f;
			sB = (float)B/255.f;
			dR = (float)((nDstColor & 0xff0000) >> 16)/255.f;
			dG = (float)((nDstColor & 0xff00) >> 8)/255.f;
			dB = (float)(nDstColor & 0xff)/255.f;
			sA = (float)nExAlpha/255.f;
			dA = (float)A2/255.f;
			dR = sR*sA + dR*dA*(1.f-sA);
			dG = sG*sA + dG*dA*(1.f-sA);
			dB = sB*sA + dB*dA*(1.f-sA);
			dA = sA + dA*(1.f-sA);
			dR /= dA;
			dG /= dA;
			dB /= dA;
			R = (UINT)(dR*255.f);
			G = (UINT)(dG*255.f);
			B = (UINT)(dB*255.f);
			A2 = (UINT)(dA*255.f);
			nDstColor = (A2 << 24) | (R << 16) | (G << 8) | B;
		}
		*((UINT*)pBuffer) = nDstColor;
		pBuffer += 4;
		if(nWidth)
			goto	_DrawFullLineSection_LineLocal_;
		pBuffer += nBuffNextLine;
		Clipper.height--;
		if(Clipper.height)
		{
			pBitmap += nSprSkipPerLine;
			goto	_DrawFullLineSection_Line_;
		}
		goto	_EXIT_WAY_;
	
	_EXIT_WAY_:
	return;
}


void RIO_CopySprToBuffer3LevelAlpha(void* pSpr, int nSprWidth, int nSprHeight, void* pPalette,
					 void* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY)
{
	_ASSERT(pSpr && pBuffer && pPalette);
	// 对绘制区域进行裁剪
	KRClipperInfo Clipper;	
	if (RIO_ClipCopyRect(nX, nY, nSprWidth, nSprHeight, nBufferWidth, nBufferHeight, &Clipper) == 0)
		return;

	int	nPitch = nBufferWidth + nBufferWidth;
	// pBuffer指向屏幕起点的偏移位置 (以字节计)
	pBuffer = (char*)pBuffer + Clipper.y * nPitch + Clipper.x * 2;
	long nBuffNextLine = nPitch - Clipper.width * 2;// next line add
	long nSprSkip = nSprWidth * Clipper.top + Clipper.left;
	long nSprSkipPerLine = Clipper.left + Clipper.right;

	__asm
	{
		//使edi指向buffer绘制起点,使esi指向图块数据起点,(跳过nSprSkip个像点的图形数据)
		mov		edi, pBuffer
		mov		esi, pSpr

		//_SkipSpriteAheadContent_:
		{
			mov		edx, nSprSkip
			or		edx, edx
			jz		_SkipSpriteAheadContentEnd_

			_SkipSpriteAheadContentLocalStart_:
			{
				read_alpha_2_ebx_run_length_2_eax
				or		ebx, ebx
				jnz		_SkipSpriteAheadContentLocalAlpha_
				sub		edx, eax
				jg		_SkipSpriteAheadContentLocalStart_
				neg		edx
				jmp		_SkipSpriteAheadContentEnd_

				_SkipSpriteAheadContentLocalAlpha_:
				{
					add		esi, eax
					sub		edx, eax
					jg		_SkipSpriteAheadContentLocalStart_
					add		esi, edx
					neg		edx
					jmp		_SkipSpriteAheadContentEnd_
				}
			}
		}
		_SkipSpriteAheadContentEnd_:

		mov		eax, nSprSkipPerLine
		or		eax, eax
		jnz		_DrawPartLineSection_	//if (nSprSkipPerLine) goto _DrawPartLineSection_

		//_DrawFullLineSection_:
		{
			//因为sprite不会跨行压缩，则运行到此处edx必为0，如sprite会跨行压缩则_DrawFullLineSection_需改			
			_DrawFullLineSection_Line_:
			{
				mov		edx, Clipper.width
				_DrawFullLineSection_LineLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax

					or		ebx, ebx
					jnz		_DrawFullLineSection_LineLocal_Alpha_
					add		edi, eax
					add		edi, eax
					sub		edx, eax
					jg		_DrawFullLineSection_LineLocal_

					add		edi, nBuffNextLine
					dec		Clipper.height
					jnz		_DrawFullLineSection_Line_
					jmp		_EXIT_WAY_
				
					_DrawFullLineSection_LineLocal_Alpha_:
					{
						push	eax
						mov		ecx, eax

						cmp		ebx, 200
						jl		_DrawFullLineSection_LineLocal_HalfAlpha_

						//_DrawFullLineSection_LineLocal_DirectCopy_:
						{
							mov     ebx, pPalette
							_DrawFullLineSection_CopyPixel_:
							{
								copy_pixel_use_eax
								loop	_DrawFullLineSection_CopyPixel_
							}

							pop		eax
							sub		edx, eax
							jg		_DrawFullLineSection_LineLocal_
	
							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}

						_DrawFullLineSection_LineLocal_HalfAlpha_:
						{
							push	edx							
							_DrawFullLineSection_HalfAlphaPixel_:
							{
								mix_2_pixel_color_use_eabdx
								loop	_DrawFullLineSection_HalfAlphaPixel_
							}
							pop		edx
							pop		eax
							sub		edx, eax
							jg		_DrawFullLineSection_LineLocal_

							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}
					}
				}
			}
		}

		_DrawPartLineSection_:
		{
			_DrawPartLineSection_Line_:
			{
				mov		eax, edx
				mov		edx, Clipper.width
				or		eax, eax
				jnz		_DrawPartLineSection_LineLocal_CheckAlpha_

				_DrawPartLineSection_LineLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax

					_DrawPartLineSection_LineLocal_CheckAlpha_:
					or		ebx, ebx
					jnz		_DrawPartLineSection_LineLocal_Alpha_
					add		edi, eax
					add		edi, eax
					sub		edx, eax
					jg		_DrawPartLineSection_LineLocal_

					dec		Clipper.height
					jz		_EXIT_WAY_

					add		edi, edx
					add		edi, edx
					neg		edx
				}
				
				_DrawPartLineSection_LineSkip_:
				{
					add		edi, nBuffNextLine
					//跳过nSprSkipPerLine像素的sprite内容
					mov		eax, edx
					mov		edx, nSprSkipPerLine
					or		eax, eax
					jnz		_DrawPartLineSection_LineSkipLocal_CheckAlpha_

					_DrawPartLineSection_LineSkipLocal_:
					{
						read_alpha_2_ebx_run_length_2_eax
						
						_DrawPartLineSection_LineSkipLocal_CheckAlpha_:
						or		ebx, ebx
						jnz		_DrawPartLineSection_LineSkipLocal_Alpha_
						sub		edx, eax
						jg		_DrawPartLineSection_LineSkipLocal_
						neg		edx
						jmp		_DrawPartLineSection_Line_

						_DrawPartLineSection_LineSkipLocal_Alpha_:
						{
							add		esi, eax
							sub		edx, eax
							jg		_DrawPartLineSection_LineSkipLocal_
							add		esi, edx
							neg		edx
							jmp		_DrawPartLineSection_Line_
						}
					}
				}

				_DrawPartLineSection_LineLocal_Alpha_:
				{
					cmp		eax, edx
					jnl		_DrawPartLineSection_LineLocal_Alpha_Part_		//不能全画这eax个相同alpha值的像点，后面有点已经超出区域

					push	eax
					mov		ecx, eax
					cmp		ebx, 200
					jl		_DrawPartLineSection_LineLocal_HalfAlpha_
						
					//_DrawPartLineSection_LineLocal_DirectCopy_:
					{
						mov     ebx, pPalette
						_DrawPartLineSection_CopyPixel_:
						{
							copy_pixel_use_eax
							loop	_DrawPartLineSection_CopyPixel_
						}						
						pop		eax
						sub		edx, eax
						jmp		_DrawPartLineSection_LineLocal_
					}
					
					_DrawPartLineSection_LineLocal_HalfAlpha_:
					{
						push	edx
						_DrawPartLineSection_HalfAlphaPixel_:
						{
							mix_2_pixel_color_use_eabdx
							loop	_DrawPartLineSection_HalfAlphaPixel_
						}
						pop		edx
						pop		eax
						sub		edx, eax
						jmp		_DrawPartLineSection_LineLocal_
					}
				}

				_DrawPartLineSection_LineLocal_Alpha_Part_:
				{
					push	eax
					mov		ecx, edx
					cmp		ebx, 200
					jl		_DrawPartLineSection_LineLocal_HalfAlpha_Part_
						
					//_DrawPartLineSection_LineLocal_DirectCopy_Part_:
					{
						mov     ebx, pPalette
						_DrawPartLineSection_CopyPixel_Part_:
						{
							copy_pixel_use_eax
							loop	_DrawPartLineSection_CopyPixel_Part_
						}						
						pop		eax
				
						dec		Clipper.height
						jz		_EXIT_WAY_

						sub		eax, edx
						mov		edx, eax
						mov		ebx, 255	//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
						jmp		_DrawPartLineSection_LineSkip_
					}
					
					_DrawPartLineSection_LineLocal_HalfAlpha_Part_:
					{
						push	edx
						_DrawPartLineSection_HalfAlphaPixel_Part_:
						{
							mix_2_pixel_color_use_eabdx
							loop	_DrawPartLineSection_HalfAlphaPixel_Part_
						}
						pop		edx
						pop		eax
						dec		Clipper.height
						jz		_EXIT_WAY_
						sub		eax, edx
						mov		edx, eax
						mov		ebx, 128
						jmp		_DrawPartLineSection_LineSkip_//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
					}
				}
			}
		}
		_EXIT_WAY_:
	}
}