//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KDrawSpriteAlpha.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis), Wooy(Wu yue)
// Desc:	Sprite Alpha Drawing Function
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KCanvas.h"
#include "KDrawSpriteAlpha.h"
#include "DrawSpriteMP.inc"
WORD*	g_pPal;
BYTE*	g_pSection;
INT		g_nIndex;
__int64 g_n64ColorMask =  0x001f001f001f001f;

UINT g_BlendColor16b(UINT nSrcColor, UINT nBlendColor, int nMode, BOOL bDstClr16b = FALSE)
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
	switch (nMode)
	{
		case 1:
			{
				float d = (sR*0.3f + sG*0.59f + sB*0.11f) - (dR*0.3f + dG*0.59f + dB*0.11f);
				dR += d;
				dG += d;
				dB += d;
				float fL = (dR*0.3f + dG*0.59f + dB*0.11f);
				float fMin = dR;
				if(fMin > dG)
					fMin = dG;
				if(fMin > dB)
					fMin = dB;
				float fMax = dR;
				if(fMax < dG)
					fMax = dG;
				if(fMax < dB)
					fMax = dB;
				if(fMin < 0.f)
				{
					dR = fL + ((dR-fL)*fL)/(fL-fMin);
					dG = fL + ((dG-fL)*fL)/(fL-fMin);
					dB = fL + ((dB-fL)*fL)/(fL-fMin);
				}
				if(fMax > 1.f)
				{
					dR = fL + ((dR-fL)*(1.f-fL))/(fMax-fL);
					dG = fL + ((dG-fL)*(1.f-fL))/(fMax-fL);
					dB = fL + ((dB-fL)*(1.f-fL))/(fMax-fL);
				}

			break;
			}
		case 2:
			{
			dR = dR + sR - dR*sR;
			dG = dG + sG - dG*sG;
			dB = dB + sB - dB*sB;
			break;
			}
		default:
			dR *= sR;
			dG *= sG;
			dB *= sB;
		break;
	}
	
	UINT nRetColor = (((UINT)(dB*255.f)) >> 3);
	nRetColor |= ((((UINT)(dR*255.f)) >> 3) << 11);
	nRetColor |= ((((UINT)(dG*255.f)) >> 2) << 5);
	return nRetColor;
}

UINT g_BlendColor16b(UINT usR, UINT usG, UINT usB, UINT nBlendColor, int nMode, BOOL bDstClr16b = FALSE, UINT usA = 0)
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
	switch (nMode)
	{
		case 1:
			{
				float d = (sR*0.3f + sG*0.59f + sB*0.11f) - (dR*0.3f + dG*0.59f + dB*0.11f);
				dR += d;
				dG += d;
				dB += d;
				float fL = (dR*0.3f + dG*0.59f + dB*0.11f);
				float fMin = dR;
				if(fMin > dG)
					fMin = dG;
				if(fMin > dB)
					fMin = dB;
				float fMax = dR;
				if(fMax < dG)
					fMax = dG;
				if(fMax < dB)
					fMax = dB;
				if(fMin < 0.f)
				{
					dR = fL + ((dR-fL)*fL)/(fL-fMin);
					dG = fL + ((dG-fL)*fL)/(fL-fMin);
					dB = fL + ((dB-fL)*fL)/(fL-fMin);
				}
				if(fMax > 1.f)
				{
					dR = fL + ((dR-fL)*(1.f-fL))/(fMax-fL);
					dG = fL + ((dG-fL)*(1.f-fL))/(fMax-fL);
					dB = fL + ((dB-fL)*(1.f-fL))/(fMax-fL);
				}

			break;
			}
		case 2:
		{
			if(usA)
			{
				dR = (sR+dR-sR*dR)*sA + dR*(1.f-sA);
				dG = (sG+dG-sG*dG)*sA + dG*(1.f-sA);
				dB = (sB+dB-sB*dB)*sA + dB*(1.f-sA);
			}
			else
			{
				dR = dR + sR - dR*sR;
				dG = dG + sG - dG*sG;
				dB = dB + sB - dB*sB;
			}
			break;
		}
		default:
			dR *= sR;
			dG *= sG;
			dB *= sB;
		break;
	}
	
	UINT nRetColor = (((UINT)(dB*255.f)) >> 3);
	nRetColor |= ((((UINT)(dR*255.f)) >> 3) << 11);
	nRetColor |= ((((UINT)(dG*255.f)) >> 2) << 5);
	return nRetColor;
}

UINT g_BlendColor32b(UINT usR, UINT usG, UINT usB, UINT nBlendColor, int nMode, UINT usA = 0)
{
	float sA = (float)usA/255.f;
	float sR = (float)usR/255.f;
	float sG = (float)usG/255.f;
	float sB = (float)usB/255.f;
	float dR = (float)((nBlendColor & 0xff0000) >> 16)/255.f;
	float dG = (float)((nBlendColor & 0xff00) >> 8)/255.f;
	float dB = (float)(nBlendColor & 0xff)/255.f;
	
	switch (nMode)
	{
		case 1:
			{
				float d = (sR*0.3f + sG*0.59f + sB*0.11f) - (dR*0.3f + dG*0.59f + dB*0.11f);
				dR += d;
				dG += d;
				dB += d;
				float fL = (dR*0.3f + dG*0.59f + dB*0.11f);
				float fMin = dR;
				if(fMin > dG)
					fMin = dG;
				if(fMin > dB)
					fMin = dB;
				float fMax = dR;
				if(fMax < dG)
					fMax = dG;
				if(fMax < dB)
					fMax = dB;
				if(fMin < 0.f)
				{
					dR = fL + ((dR-fL)*fL)/(fL-fMin);
					dG = fL + ((dG-fL)*fL)/(fL-fMin);
					dB = fL + ((dB-fL)*fL)/(fL-fMin);
				}
				if(fMax > 1.f)
				{
					dR = fL + ((dR-fL)*(1.f-fL))/(fMax-fL);
					dG = fL + ((dG-fL)*(1.f-fL))/(fMax-fL);
					dB = fL + ((dB-fL)*(1.f-fL))/(fMax-fL);
				}

			break;
			}
		case 2:
		{
			if(usA)
			{
				dR = (sR+dR-sR*dR)*sA + dR*(1.f-sA);
				dG = (sG+dG-sG*dG)*sA + dG*(1.f-sA);
				dB = (sB+dB-sB*dB)*sA + dB*(1.f-sA);
			}
			else
			{
				dR = dR + sR - dR*sR;
				dG = dG + sG - dG*sG;
				dB = dB + sB - dB*sB;
			}
			break;
		}
		default:
			dR *= sR;
			dG *= sG;
			dB *= sB;
		break;
	}
	
	UINT nRetColor = (UINT)(dB*255.f);
	nRetColor |= ((UINT)(dR*255.f)) << 16;
	nRetColor |= ((UINT)(dG*255.f)) << 8;
	return nRetColor;
}

void g_BlendColor32b(UINT usR, UINT usG, UINT usB, UINT nBlendColor, int nMode,
				UINT& uR, UINT& uG, UINT& uB)
{
	float sR = (float)usR/255.f;
	float sG = (float)usG/255.f;
	float sB = (float)usB/255.f;
	float dR = (float)((nBlendColor & 0xff0000) >> 16)/255.f;
	float dG = (float)((nBlendColor & 0xff00) >> 8)/255.f;
	float dB = (float)(nBlendColor & 0xff)/255.f;
	
	switch (nMode)
	{
		case 1:
			{
				float d = (sR*0.3f + sG*0.59f + sB*0.11f) - (dR*0.3f + dG*0.59f + dB*0.11f);
				dR += d;
				dG += d;
				dB += d;
				float fL = (dR*0.3f + dG*0.59f + dB*0.11f);
				float fMin = dR;
				if(fMin > dG)
					fMin = dG;
				if(fMin > dB)
					fMin = dB;
				float fMax = dR;
				if(fMax < dG)
					fMax = dG;
				if(fMax < dB)
					fMax = dB;
				if(fMin < 0.f)
				{
					dR = fL + ((dR-fL)*fL)/(fL-fMin);
					dG = fL + ((dG-fL)*fL)/(fL-fMin);
					dB = fL + ((dB-fL)*fL)/(fL-fMin);
				}
				if(fMax > 1.f)
				{
					dR = fL + ((dR-fL)*(1.f-fL))/(fMax-fL);
					dG = fL + ((dG-fL)*(1.f-fL))/(fMax-fL);
					dB = fL + ((dB-fL)*(1.f-fL))/(fMax-fL);
				}

			break;
			}
		case 2:
			{
			dR = dR + sR - dR*sR;
			dG = dG + sG - dG*sG;
			dB = dB + sB - dB*sB;
			break;
			}
		default:
			dR *= sR;
			dG *= sG;
			dB *= sB;
		break;
	}
	
	uR = (UINT)(dR*255.f);
	uG = (UINT)(dG*255.f);
	uB = (UINT)(dB*255.f);
}

//---------------------------------------------------------------------------
// 函数:	Draw Sprite nAlpha
// 功能:	绘制256色Sprite位图(不带预渲染)
// 参数:	KDrawNode*, KCanvas* 
// 返回:	void
//---------------------------------------------------------------------------
void g_DrawSpriteAlpha(void* node, void* canvas)
{
#ifndef _WIN64
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	
	// ????????óò??DD2???
	KClipper Clipper;
	if (pCanvas->MakeClip(pNode->m_nX, pNode->m_nY, pNode->m_nWidth, pNode->m_nHeight, &Clipper) == 0)
		return;

	// pBuffer???ò?á??????DDμ?í·ò?????μ?′| 
	int nPitch;
	void* pBuffer = pCanvas->LockCanvas(nPitch);
	if (pBuffer == NULL)
		return;
	pBuffer = (char*)(pBuffer) + Clipper.y * nPitch;
	void* pPalette	= pNode->m_pPalette;// palette pointer
	void* pSprite = pNode->m_pBitmap;	// sprite pointer
	int nMask32 = pCanvas->m_nMask32;	// rgb mask32
	int nBuffNextLine = nPitch - Clipper.width * 2;// next line add
	int nSprSkip = pNode->m_nWidth * Clipper.top + Clipper.left;
	int nSprSkipPerLine = Clipper.left + Clipper.right;

	__asm
	{
        mov     eax, pPalette
        movd    mm0, eax        // mm0: pPalette

        mov     eax, Clipper.width
        movd    mm1, eax        // mm1: Clipper.width

        mov     eax, nMask32
        movd    mm2, eax        // mm2: nMask32

        // mm3: nAlpha

        // mm4: temp use

        // mm7: push ecx, pop ecx
        // mm6: push edx, pop edx
        // mm5: push eax, pop eax


		//ê1edi???òbuffer?????eμ?,	(ò?×??ú??)	
		mov		edi, pBuffer
		mov		eax, Clipper.x
		add		edi, eax
		add		edi, eax
        

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
				movd	edx, mm1    // mm1: Clipper.width
				_DrawFullLineSection_LineLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax

					or		ebx, ebx
					jnz		_DrawFullLineSection_LineLocal_Alpha_
                    lea     edi, [edi + eax * 2]
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

						cmp		ebx, 255
						jl		_DrawFullLineSection_LineLocal_HalfAlpha_

						//_DrawFullLineSection_LineLocal_DirectCopy_:
						{
							movd     ebx, mm0   // mm0: pPalette
                            
						    _DrawFullLineSection_CopyPixel_:
							{
								copy_pixel_use_eax
                                dec     ecx
                                jnz     _DrawFullLineSection_CopyPixel_
							}

							or		edx, edx
							jnz		_DrawFullLineSection_LineLocal_
	
							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}

						_DrawFullLineSection_LineLocal_HalfAlpha_:
						{
							movd    mm6, edx
							shr		ebx, 3
                            movd    mm3, ebx    // mm3: nAlpha
							_DrawFullLineSection_HalfAlphaPixel_:
							{
								mix_2_pixel_color_alpha_use_eabdx
								loop	_DrawFullLineSection_HalfAlphaPixel_
							}
							movd    edx, mm6
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
			movd	edx, mm1    // mm1: Clipper.width
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
				cmp		ebx, 255
				jl		_DrawPartLineSection_LineLocal_HalfAlpha_
						
				//_DrawPartLineSection_LineLocal_DirectCopy_:
				{
					movd     ebx, mm0 // mm0: pPalette
					_DrawPartLineSection_CopyPixel_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_CopyPixel_
					}
					jmp		_DrawPartLineSection_LineLocal_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_:
				{
					movd    mm6, edx
					shr		ebx, 3
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx
						loop	_DrawPartLineSection_HalfAlphaPixel_
					}
					movd    edx, mm6
					jmp		_DrawPartLineSection_LineLocal_
				}
			}
			_DrawPartLineSection_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				movd	mm5, ebx
				cmp		ebx, 255
				jl		_DrawPartLineSection_LineLocal_HalfAlpha_Part_
					
				//_DrawPartLineSection_LineLocal_DirectCopy_Part_:
				{
					movd    ebx,  mm0   // mm0: pPalette
					_DrawPartLineSection_CopyPixel_Part_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_CopyPixel_Part_
					}
			
					dec		Clipper.height
					jz		_EXIT_WAY_
					neg		edx
					mov		ebx, 255
					jmp		_DrawPartLineSection_LineSkip_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_Part_:
				{
					movd    mm6, edx
					shr		ebx, 3
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_HalfAlphaPixel_Part_:
					{
						mix_2_pixel_color_alpha_use_eabdx
						loop	_DrawPartLineSection_HalfAlphaPixel_Part_
					}
					movd	ebx, mm5
					movd    edx, mm6
					neg		edx
					dec		Clipper.height
					jg		_DrawPartLineSection_LineSkip_
					jmp		_EXIT_WAY_
				}
			}
		}

		_DrawPartLineSection_SkipLeft_Line_:
		{
			mov		eax, edx
			movd	edx, mm1    // mm1: Clipper.width
			or		eax, eax
			jnz		_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_
			_DrawPartLineSection_SkipLeft_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_:
				or		ebx, ebx
				jnz		_DrawPartLineSection_SkipLeft_LineLocal_Alpha_
				add		edi, eax
				add		edi, eax
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
				sub		edx, eax		//?è°?eax??á?￡??a?ùáá???í?éò?2?Dèòa±￡á?eaxá?
				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_
						
				//_DrawPartLineSection_SkipLeft_LineLocal_DirectCopy_:
				{
					movd    ebx, mm0    // mm0: pPalette
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

				_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_:
				{
					movd    mm6, edx
					shr		ebx, 3
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_SkipLeft_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx
						loop	_DrawPartLineSection_SkipLeft_HalfAlphaPixel_
					}
					movd    edx, mm6
					or		edx, edx
					jnz		_DrawPartLineSection_SkipLeft_LineLocal_
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipLeft_LineSkip_
					jmp		_EXIT_WAY_
				}
			}
		}

		_DrawPartLineSection_SkipRight_Line_:
		{
			movd	edx, mm1    // mm1: Clipper.width
			_DrawPartLineSection_SkipRight_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				or		ebx, ebx
				jnz		_DrawPartLineSection_SkipRight_LineLocal_Alpha_
				add		edi, eax
				add		edi, eax
				sub		edx, eax
				jg		_DrawPartLineSection_SkipRight_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_

				add		edi, edx
				add		edi, edx
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
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_
						
				//_DrawPartLineSection_SkipRight_LineLocal_DirectCopy_:
				{
					movd    ebx, mm0    // mm0: pPalette
					_DrawPartLineSection_SkipRight_CopyPixel_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_SkipRight_CopyPixel_
					}
					jmp		_DrawPartLineSection_SkipRight_LineLocal_
				}
				
				_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_:
				{
					movd    mm6, edx
					shr		ebx, 3
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_SkipRight_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx
						loop	_DrawPartLineSection_SkipRight_HalfAlphaPixel_
					}
					movd	edx, mm6
					jmp		_DrawPartLineSection_SkipRight_LineLocal_
				}
			}
			_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				movd	mm5, ebx
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_
					
				//_DrawPartLineSection_SkipRight_LineLocal_DirectCopy_Part_:
				{
					movd    ebx, mm0 // mm0: pPalette
					_DrawPartLineSection_SkipRight_CopyPixel_Part_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_SkipRight_CopyPixel_Part_
					}
					neg		edx
					mov		ebx, 255	//è?1???òaè·?Dμ??-ebx(alpha)?μ?éò??ú?°í·push ebx￡?′?′|pop??μ?
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipRight_LineSkip_
					jmp		_EXIT_WAY_
				}
				
				_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_:
				{
					movd    mm6, edx
					shr		ebx, 3
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_SkipRight_HalfAlphaPixel_Part_:
					{
						mix_2_pixel_color_alpha_use_eabdx
						loop	_DrawPartLineSection_SkipRight_HalfAlphaPixel_Part_
					}
					movd	ebx, mm5
					movd	edx, mm6
					neg		edx
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipRight_LineSkip_//è?1???òaè·?Dμ??-ebx(alpha)?μ?éò??ú?°í·push ebx￡?′?′|pop??μ?
					jmp		_EXIT_WAY_
				}
			}
		}
		_EXIT_WAY_:
        emms
	}
	pCanvas->UnlockCanvas();
#endif
}

void g_DrawSpriteAlpha(void* node, void* canvas, int nExAlpha)
{
#ifndef _WIN64
	if (nExAlpha <= 0)
		return;

	if (nExAlpha >= 254)
	{
		g_DrawSpriteAlpha(node, canvas);
		return;
	}
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	
	// ????????óò??DD2???
	KClipper Clipper;
	if (pCanvas->MakeClip(pNode->m_nX, pNode->m_nY, pNode->m_nWidth, pNode->m_nHeight, &Clipper) == 0)
		return;

	// pBuffer???ò?á??????DDμ?í·ò?????μ?′| 
	int nPitch;
	void* pBuffer = pCanvas->LockCanvas(nPitch);
	if (pBuffer == NULL)
		return;
	pBuffer = (char*)(pBuffer) + Clipper.y * nPitch;
	void* pPalette	= pNode->m_pPalette;// palette pointer
	void* pSprite = pNode->m_pBitmap;	// sprite pointer
	int nMask32 = pCanvas->m_nMask32;	// rgb mask32
	int nBuffNextLine = nPitch - Clipper.width * 2;// next line add
	int nSprSkip = pNode->m_nWidth * Clipper.top + Clipper.left;
	int nSprSkipPerLine = Clipper.left + Clipper.right;
	__asm
	{
        mov     eax, pPalette
        movd    mm0, eax        // mm0: pPalette

        mov     eax, Clipper.width
        movd    mm1, eax        // mm1: Clipper.width

        mov     eax, nMask32
        movd    mm2, eax        // mm2: nMask32

        // mm3: nAlpha

        // mm4: temp use

        // mm7: push ecx, pop ecx
        // mm6: push edx, pop edx
        // mm5: push eax, pop eax


		//ê1edi???òbuffer?????eμ?,	(ò?×??ú??)	
		mov		edi, pBuffer
		mov		eax, Clipper.x
		add		edi, eax
		add		edi, eax
        

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
				movd	edx, mm1    // mm1: Clipper.width
				_DrawFullLineSection_LineLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax

					or		ebx, ebx
					jnz		_DrawFullLineSection_LineLocal_Alpha_
                    lea     edi, [edi + eax * 2]	//ph?n nμy kh?ng v?, ch? nh?y qua
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
						mov		eax, nExAlpha
						imul	ebx, eax
						shr		ebx, 8
						cmp		ebx, 255
						jl		_DrawFullLineSection_LineLocal_HalfAlpha_

						//_DrawFullLineSection_LineLocal_DirectCopy_:
						{
							movd     ebx, mm0   // mm0: pPalette
                            
                            sub ecx, 4
                            jl  _DrawFullLineSection_CopyPixel_continue
							_DrawFullLineSection_CopyPixel4_:
							{
								copy_4pixel_use_eax
                                
                                sub ecx, 4
                                jg     _DrawFullLineSection_CopyPixel4_
							}
							_DrawFullLineSection_CopyPixel_continue:
                            add ecx, 4
                            jz _DrawFullLineSection_CopyPixel_End 

						    _DrawFullLineSection_CopyPixel_:
							{
								copy_pixel_use_eax
                                dec     ecx
                                jnz     _DrawFullLineSection_CopyPixel_
							}
                            _DrawFullLineSection_CopyPixel_End:

							or		edx, edx
							jnz		_DrawFullLineSection_LineLocal_
	
							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}

						_DrawFullLineSection_LineLocal_HalfAlpha_:
						{
							movd    mm6, edx
							shr		ebx, 3
                            movd    mm3, ebx    // mm3: nAlpha
							_DrawFullLineSection_HalfAlphaPixel_:
							{
								mix_2_pixel_color_alpha_use_eabdx
								loop	_DrawFullLineSection_HalfAlphaPixel_
							}
							movd    edx, mm6
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
			movd	edx, mm1    // mm1: Clipper.width
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
				mov		eax, nExAlpha
				imul	ebx, eax
				shr		ebx, 8
				cmp		ebx, 255
				jl		_DrawPartLineSection_LineLocal_HalfAlpha_
						
				//_DrawPartLineSection_LineLocal_DirectCopy_:
				{
					movd     ebx, mm0 // mm0: pPalette
					_DrawPartLineSection_CopyPixel_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_CopyPixel_
					}
					jmp		_DrawPartLineSection_LineLocal_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_:
				{
					movd    mm6, edx
					shr		ebx, 3
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx
						loop	_DrawPartLineSection_HalfAlphaPixel_
					}
					movd    edx, mm6
					jmp		_DrawPartLineSection_LineLocal_
				}
			}
			_DrawPartLineSection_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				mov		eax, nExAlpha
				movd	mm5, ebx
				imul	ebx, eax
				shr		ebx, 8
				cmp		ebx, 255
				jl		_DrawPartLineSection_LineLocal_HalfAlpha_Part_
					
				//_DrawPartLineSection_LineLocal_DirectCopy_Part_:
				{
					movd    ebx,  mm0   // mm0: pPalette
					_DrawPartLineSection_CopyPixel_Part_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_CopyPixel_Part_
					}
			
					dec		Clipper.height
					jz		_EXIT_WAY_
					neg		edx
					mov		ebx, 255
					jmp		_DrawPartLineSection_LineSkip_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_Part_:
				{
					movd    mm6, edx
					shr		ebx, 3
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_HalfAlphaPixel_Part_:
					{
						mix_2_pixel_color_alpha_use_eabdx
						loop	_DrawPartLineSection_HalfAlphaPixel_Part_
					}
					movd	ebx, mm5
					movd    edx, mm6
					neg		edx
					dec		Clipper.height
					jg		_DrawPartLineSection_LineSkip_
					jmp		_EXIT_WAY_
				}
			}
		}

		_DrawPartLineSection_SkipLeft_Line_:
		{
			mov		eax, edx
			movd	edx, mm1    // mm1: Clipper.width
			or		eax, eax
			jnz		_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_
			_DrawPartLineSection_SkipLeft_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_:
				or		ebx, ebx
				jnz		_DrawPartLineSection_SkipLeft_LineLocal_Alpha_
				add		edi, eax
				add		edi, eax
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
				sub		edx, eax		//?è°?eax??á?￡??a?ùáá???í?éò?2?Dèòa±￡á?eaxá?
				mov		ecx, eax
				mov		eax, nExAlpha
				imul	ebx, eax
				shr		ebx, 8
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_
						
				//_DrawPartLineSection_SkipLeft_LineLocal_DirectCopy_:
				{
					movd    ebx, mm0    // mm0: pPalette
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

				_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_:
				{
					movd    mm6, edx
					shr		ebx, 3
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_SkipLeft_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx
						loop	_DrawPartLineSection_SkipLeft_HalfAlphaPixel_
					}
					movd    edx, mm6
					or		edx, edx
					jnz		_DrawPartLineSection_SkipLeft_LineLocal_
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipLeft_LineSkip_
					jmp		_EXIT_WAY_
				}
			}
		}

		_DrawPartLineSection_SkipRight_Line_:
		{
			movd	edx, mm1    // mm1: Clipper.width
			_DrawPartLineSection_SkipRight_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				or		ebx, ebx
				jnz		_DrawPartLineSection_SkipRight_LineLocal_Alpha_
				add		edi, eax
				add		edi, eax
				sub		edx, eax
				jg		_DrawPartLineSection_SkipRight_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_

				add		edi, edx
				add		edi, edx
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
				mov		eax, nExAlpha
				imul	ebx, eax
				shr		ebx, 8
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_
						
				//_DrawPartLineSection_SkipRight_LineLocal_DirectCopy_:
				{
					movd    ebx, mm0    // mm0: pPalette
					_DrawPartLineSection_SkipRight_CopyPixel_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_SkipRight_CopyPixel_
					}
					jmp		_DrawPartLineSection_SkipRight_LineLocal_
				}
				
				_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_:
				{
					movd    mm6, edx
					shr		ebx, 3
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_SkipRight_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx
						loop	_DrawPartLineSection_SkipRight_HalfAlphaPixel_
					}
					movd	edx, mm6
					jmp		_DrawPartLineSection_SkipRight_LineLocal_
				}
			}
			_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				mov		eax, nExAlpha
				movd	mm5, ebx
				imul	ebx, eax
				shr		ebx, 8
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_
					
				//_DrawPartLineSection_SkipRight_LineLocal_DirectCopy_Part_:
				{
					movd    ebx, mm0 // mm0: pPalette
					_DrawPartLineSection_SkipRight_CopyPixel_Part_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_SkipRight_CopyPixel_Part_
					}
					neg		edx
					mov		ebx, 255	//è?1???òaè·?Dμ??-ebx(alpha)?μ?éò??ú?°í·push ebx￡?′?′|pop??μ?
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipRight_LineSkip_
					jmp		_EXIT_WAY_
				}
				
				_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_:
				{
					movd    mm6, edx
					shr		ebx, 3
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_SkipRight_HalfAlphaPixel_Part_:
					{
						mix_2_pixel_color_alpha_use_eabdx
						loop	_DrawPartLineSection_SkipRight_HalfAlphaPixel_Part_
					}
					movd	ebx, mm5
					movd	edx, mm6
					neg		edx
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipRight_LineSkip_//è?1???òaè·?Dμ??-ebx(alpha)?μ?éò??ú?°í·push ebx￡?′?′|pop??μ?
					jmp		_EXIT_WAY_
				}
			}
		}
		_EXIT_WAY_:
        emms
	}
	pCanvas->UnlockCanvas();
#endif
}


//三级alpha绘制
void g_DrawSprite3LevelAlpha(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;

	void* pSprite = pNode->m_pBitmap;	// sprite pointer
	void* pPalette	= pNode->m_pPalette;// palette pointer

	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (!pCanvas->MakeClip(pNode->m_nX, pNode->m_nY, pNode->m_nWidth, pNode->m_nHeight, &Clipper))
		return;

	int nPitch;
	void* pBuffer = pCanvas->LockCanvas(nPitch);
	if (pBuffer == NULL)
		return;

	long nMask32 = pCanvas->m_nMask32;	// rgb mask32

	// pBuffer指向屏幕起点的偏移位置 (以字节计)
	pBuffer = (char*)pBuffer + Clipper.y * nPitch + Clipper.x * 2;
	long nBuffNextLine = nPitch - Clipper.width * 2;// next line add
	long nSprSkip = pNode->m_nWidth * Clipper.top + Clipper.left;
	long nSprSkipPerLine = Clipper.left + Clipper.right;
#ifdef _WIN64
#else
	__asm
	{
        mov     eax, pPalette
        movd    mm0, eax        // mm0: pPalette

        mov     eax, Clipper.width
        movd    mm1, eax        // mm1: Clipper.width

        mov     eax, nMask32
        movd    mm2, eax        // mm2: nMask32

        // mm3: nAlpha
        // mm4: 32 - nAlpha

        // mm7: push ecx, pop ecx
        // mm6: push edx, pop edx
        // mm5: push eax, pop eax

		//使edi指向canvas绘制起点,使esi指向图块数据起点,(跳过nSprSkip个像点的图形数据)
		mov		edi, pBuffer
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
			//因为sprite不会跨行压缩，则运行到此处edx必为0，如sprite会跨行压缩则_DrawFullLineSection_需改			
			_DrawFullLineSection_Line_:
			{
				movd	edx, mm1    // mm1: Clipper.width
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
						movd    mm5, eax
						mov		ecx, eax

						cmp		ebx, 200
						jl		_DrawFullLineSection_LineLocal_HalfAlpha_

						//_DrawFullLineSection_LineLocal_DirectCopy_:
						{
							movd    ebx, mm0    // mm0: pPalette
							_DrawFullLineSection_CopyPixel_:
							{
								copy_pixel_use_eax
								loop	_DrawFullLineSection_CopyPixel_
							}

							movd    eax, mm5
							sub		edx, eax
							jg		_DrawFullLineSection_LineLocal_
	
							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}

						_DrawFullLineSection_LineLocal_HalfAlpha_:
						{
        					movd    mm6, edx
							_DrawFullLineSection_HalfAlphaPixel_:
							{
								mix_2_pixel_color_use_eabdx
								loop	_DrawFullLineSection_HalfAlphaPixel_
							}
        					movd	edx, mm6
							movd    eax, mm5
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
				movd	edx, mm1    // mm1: Clipper.width
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

					movd	mm5, eax
					mov		ecx, eax
					cmp		ebx, 200
					jl		_DrawPartLineSection_LineLocal_HalfAlpha_
						
					//_DrawPartLineSection_LineLocal_DirectCopy_:
					{
						movd    ebx, mm0    // mm0: pPalette
						_DrawPartLineSection_CopyPixel_:
						{
							copy_pixel_use_eax
							loop	_DrawPartLineSection_CopyPixel_
						}						
						movd    eax, mm5
						sub		edx, eax
						jmp		_DrawPartLineSection_LineLocal_
					}
					
					_DrawPartLineSection_LineLocal_HalfAlpha_:
					{
    					movd    mm6, edx
						_DrawPartLineSection_HalfAlphaPixel_:
						{
							mix_2_pixel_color_use_eabdx
							loop	_DrawPartLineSection_HalfAlphaPixel_
						}
       					movd	edx, mm6
						movd    eax, mm5
						sub		edx, eax
						jmp		_DrawPartLineSection_LineLocal_
					}
				}

				_DrawPartLineSection_LineLocal_Alpha_Part_:
				{
					movd    mm5, eax
					mov		ecx, edx
					cmp		ebx, 200
					jl		_DrawPartLineSection_LineLocal_HalfAlpha_Part_
						
					//_DrawPartLineSection_LineLocal_DirectCopy_Part_:
					{
						movd    ebx, mm0    // mm0: pPalette
						_DrawPartLineSection_CopyPixel_Part_:
						{
							copy_pixel_use_eax
							loop	_DrawPartLineSection_CopyPixel_Part_
						}						
						movd    eax, mm5
				
						dec		Clipper.height
						jz		_EXIT_WAY_

						sub		eax, edx
						mov		edx, eax
						mov		ebx, 255	//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
						jmp		_DrawPartLineSection_LineSkip_
					}
					
					_DrawPartLineSection_LineLocal_HalfAlpha_Part_:
					{
    					movd    mm6, edx
						_DrawPartLineSection_HalfAlphaPixel_Part_:
						{
							mix_2_pixel_color_use_eabdx
							loop	_DrawPartLineSection_HalfAlphaPixel_Part_
						}
       					movd	edx, mm6
						movd    eax, mm5
						dec		Clipper.height
						jz		_EXIT_WAY_
						sub		eax, edx
						mov		edx, eax
						mov		ebx, 128	//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
						jmp		_DrawPartLineSection_LineSkip_
					}
				}
			}
		}
		_EXIT_WAY_:
        emms
	}
	pCanvas->UnlockCanvas();
#endif
}

void g_DrawSpriteAlpha32b(void* node, void* canvas)
{
#ifndef _WIN64
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	
	// t1o khung vu?ng trong ph1m vi khung game
	KClipper Clipper;
	if (pCanvas->MakeClip(pNode->m_nX, pNode->m_nY, pNode->m_nWidth, pNode->m_nHeight, &Clipper) == 0)
		return;

	// pBuffer lμ khung v? thμnh phèm
	int nPitch;
	void* pBuffer = pCanvas->LockCanvas(nPitch);
	if (pBuffer == NULL)
		return;
	pBuffer = (char*)(pBuffer) + Clipper.y * nPitch;
	void* pPalette	= pNode->m_pPalette;// palette pointer
	void* pSprite = pNode->m_pBitmap;	// sprite pointer
	int nBuffNextLine = nPitch - Clipper.width * 4;// next line add
	int nSprSkip = pNode->m_nWidth * Clipper.top + Clipper.left;
	int nSprSkipPerLine = Clipper.left + Clipper.right;
	__asm
	{
        mov     eax, pPalette
        movd    mm0, eax        // mm0: pPalette

        mov     eax, Clipper.width
        movd    mm1, eax        // mm1: Clipper.width

        //mov     eax, nMask32
        //movd    mm2, eax        // mm2: nMask32

        // mm3: nAlpha
        // mm4: temp use
        // mm7: push ecx, pop ecx
        // mm6: push edx, pop edx
        // mm5: push eax, pop eax

		//?i ??n pBuffer + Clipper.y * nPitch + Clipper.x*4
		mov		edi, pBuffer
		mov		eax, Clipper.x
		imul	eax, 4
		add		edi, eax

		//d÷ li?u sprite
		mov		esi, pSprite

		//_SkipSpriteAheadContent_:
		{
			mov		edx, nSprSkip
			or		edx, edx
			jz		_SkipSpriteAheadContentEnd_

			_SkipSpriteAheadContentLocalStart_:
			{	//??c sè l-?ng ?ióm eax vμ alpha ebx (2 bytes)
				read_alpha_2_ebx_run_length_2_eax
				or		ebx, ebx
				jnz		_SkipSpriteAheadContentLocalAlpha_
				sub		edx, eax
				jg		_SkipSpriteAheadContentLocalStart_
				neg		edx	//edx <= 0 sè l-?ng v-?t qu? c?a nSprSkip
				jmp		_SkipSpriteAheadContentEnd_

				_SkipSpriteAheadContentLocalAlpha_://n?u alpha > 0 th× esi+
				{
					add		esi, eax
					sub		edx, eax
					jg		_SkipSpriteAheadContentLocalStart_
					add		esi, edx	//edx <= 0 tr? l1i esi ptr 
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
			//v? full spr kh?ng èn ph?n nμo			
			_DrawFullLineSection_Line_:
			{
				movd	edx, mm1    // mm1: Clipper.width
				_DrawFullLineSection_LineLocal_:
				{	//??c sè l-?ng ?ióm eax vμ alpha ebx (2 bytes)
					read_alpha_2_ebx_run_length_2_eax
					or		ebx, ebx
					jnz		_DrawFullLineSection_LineLocal_Alpha_
                    lea     edi, [edi + eax * 4]//nh?y edi qua sè l-?ng ?ióm eax
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
						cmp		ebx, 255
						jl		_DrawFullLineSection_LineLocal_HalfAlpha_
						//v? ?ióm kìm theo alpha 0xff000000
						//_DrawFullLineSection_LineLocal_DirectCopy_:
						{
							movd     ebx, mm0   // mm0: pPalette
                            sub ecx, 4
                            jl  _DrawFullLineSection_CopyPixel_continue
							_DrawFullLineSection_CopyPixel4_:
							{	//v? 1 l?n 4 ?ióm
								copy_4pixel_use_eax_32b
                                sub ecx, 4
                                jg     _DrawFullLineSection_CopyPixel4_
							}
							_DrawFullLineSection_CopyPixel_continue:
                            add ecx, 4
                            jz _DrawFullLineSection_CopyPixel_End 

						    _DrawFullLineSection_CopyPixel_:
							{	//v? t?ng ?ióm
								copy_pixel_use_eax_32b
                                dec     ecx
                                jnz     _DrawFullLineSection_CopyPixel_
							}
                            _DrawFullLineSection_CopyPixel_End:

							or		edx, edx
							jnz		_DrawFullLineSection_LineLocal_
	
							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}
						//v? ?ióm kìm theo alpha < 255
						_DrawFullLineSection_LineLocal_HalfAlpha_:
						{
							movd    mm6, edx
                            movd    mm3, ebx    // mm3: nAlpha
							_DrawFullLineSection_HalfAlphaPixel_:
							{
								mix_2_pixel_color_alpha_use_eabdx_32b
								dec ecx
								jnz	_DrawFullLineSection_HalfAlphaPixel_
							}
							movd    edx, mm6
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
			movd	edx, mm1    // mm1: Clipper.width
			or		eax, eax
			jnz		_DrawPartLineSection_LineLocal_CheckAlpha_
			_DrawPartLineSection_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				_DrawPartLineSection_LineLocal_CheckAlpha_:
				or		ebx, ebx
				jnz		_DrawPartLineSection_LineLocal_Alpha_
				lea     edi, [edi + eax * 4]//nh?y edi qua sè l-?ng ?ióm eax
				sub		edx, eax
				jg		_DrawPartLineSection_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_
				
				lea     edi, [edi + edx * 4]//nh?y edi qua sè l-?ng ?ióm edx
				neg		edx
			}
			
			_DrawPartLineSection_LineSkip_:
			{
				add		edi, nBuffNextLine
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
				cmp		ebx, 255
				jl		_DrawPartLineSection_LineLocal_HalfAlpha_
						
				//_DrawPartLineSection_LineLocal_DirectCopy_:
				{
					movd     ebx, mm0 // mm0: pPalette
					_DrawPartLineSection_CopyPixel_:
					{
						copy_pixel_use_eax_32b
						dec ecx
						jnz	_DrawPartLineSection_CopyPixel_
					}
					jmp		_DrawPartLineSection_LineLocal_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_:
				{
					movd    mm6, edx
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx_32b
						dec ecx
						jnz	_DrawPartLineSection_HalfAlphaPixel_
					}
					movd    edx, mm6
					jmp		_DrawPartLineSection_LineLocal_
				}
			}
			_DrawPartLineSection_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_LineLocal_HalfAlpha_Part_
					
				//_DrawPartLineSection_LineLocal_DirectCopy_Part_:
				{
					movd    ebx,  mm0   // mm0: pPalette
					_DrawPartLineSection_CopyPixel_Part_:
					{
						copy_pixel_use_eax_32b
						dec ecx
						jnz	_DrawPartLineSection_CopyPixel_Part_
					}
			
					dec		Clipper.height
					jz		_EXIT_WAY_
					neg		edx
					mov		ebx, 255
					jmp		_DrawPartLineSection_LineSkip_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_Part_:
				{
					movd    mm6, edx
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_HalfAlphaPixel_Part_:
					{
						mix_2_pixel_color_alpha_use_eabdx_32b
						dec ecx
						jnz	_DrawPartLineSection_HalfAlphaPixel_Part_
					}
					movd	ebx, mm3
					movd    edx, mm6
					neg		edx
					dec		Clipper.height
					jg		_DrawPartLineSection_LineSkip_
					jmp		_EXIT_WAY_
				}
			}
		}

		_DrawPartLineSection_SkipLeft_Line_:
		{
			mov		eax, edx
			movd	edx, mm1    // mm1: Clipper.width
			or		eax, eax
			jnz		_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_
			_DrawPartLineSection_SkipLeft_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_:
				or		ebx, ebx
				jnz		_DrawPartLineSection_SkipLeft_LineLocal_Alpha_
				lea     edi, [edi + eax * 4]//nh?y edi qua sè l-?ng ?ióm eax
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
				sub		edx, eax		//?è°?eax??á?￡??a?ùáá???í?éò?2?Dèòa±￡á?eaxá?
				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_
						
				//_DrawPartLineSection_SkipLeft_LineLocal_DirectCopy_:
				{
					movd    ebx, mm0    // mm0: pPalette
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

				_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_:
				{
					movd    mm6, edx
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_SkipLeft_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx_32b
						dec ecx
						jnz	_DrawPartLineSection_SkipLeft_HalfAlphaPixel_
					}
					movd    edx, mm6
					or		edx, edx
					jnz		_DrawPartLineSection_SkipLeft_LineLocal_
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipLeft_LineSkip_
					jmp		_EXIT_WAY_
				}
			}
		}

		_DrawPartLineSection_SkipRight_Line_:
		{
			movd	edx, mm1    // mm1: Clipper.width
			_DrawPartLineSection_SkipRight_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				or		ebx, ebx
				jnz		_DrawPartLineSection_SkipRight_LineLocal_Alpha_
				lea     edi, [edi + eax * 4]//nh?y edi qua sè l-?ng ?ióm eax
				sub		edx, eax
				jg		_DrawPartLineSection_SkipRight_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_

				lea     edi, [edi + edx * 4]//nh?y edi qua sè l-?ng ?ióm edx
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
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_
						
				//_DrawPartLineSection_SkipRight_LineLocal_DirectCopy_:
				{
					movd    ebx, mm0    // mm0: pPalette
					_DrawPartLineSection_SkipRight_CopyPixel_:
					{
						copy_pixel_use_eax_32b
						dec ecx
						jnz	_DrawPartLineSection_SkipRight_CopyPixel_
					}
					jmp		_DrawPartLineSection_SkipRight_LineLocal_
				}
				
				_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_:
				{
					movd    mm6, edx
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_SkipRight_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx_32b
						dec ecx
						jnz	_DrawPartLineSection_SkipRight_HalfAlphaPixel_
					}
					movd	edx, mm6
					jmp		_DrawPartLineSection_SkipRight_LineLocal_
				}
			}
			_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_
					
				//_DrawPartLineSection_SkipRight_LineLocal_DirectCopy_Part_:
				{
					movd    ebx, mm0 // mm0: pPalette
					_DrawPartLineSection_SkipRight_CopyPixel_Part_:
					{
						copy_pixel_use_eax_32b
						dec ecx
						jnz	_DrawPartLineSection_SkipRight_CopyPixel_Part_
					}
					neg		edx
					mov		ebx, 255	//è?1???òaè·?Dμ??-ebx(alpha)?μ?éò??ú?°í·push ebx￡?′?′|pop??μ?
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipRight_LineSkip_
					jmp		_EXIT_WAY_
				}
				
				_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_:
				{
					movd    mm6, edx
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_SkipRight_HalfAlphaPixel_Part_:
					{
						mix_2_pixel_color_alpha_use_eabdx_32b
						dec ecx
						jnz	_DrawPartLineSection_SkipRight_HalfAlphaPixel_Part_
					}
					movd	edx, mm6
					neg		edx
					movd	ebx, mm3
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipRight_LineSkip_//è?1???òaè·?Dμ??-ebx(alpha)?μ?éò??ú?°í·push ebx￡?′?′|pop??μ?
					jmp		_EXIT_WAY_
				}
			}
		}
		_EXIT_WAY_:
        emms
	}
	pCanvas->UnlockCanvas();
#endif
}

void g_DrawSpriteAlpha32b(void* node, void* canvas, int nExAlpha)
{
#ifndef _WIN64
	if (nExAlpha <= 0)
		return;

	if (nExAlpha >= 254)
	{
		g_DrawSpriteAlpha32b(node, canvas);
		return;
	}
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	
	// t1o khung vu?ng trong ph1m vi khung game
	KClipper Clipper;
	if (pCanvas->MakeClip(pNode->m_nX, pNode->m_nY, pNode->m_nWidth, pNode->m_nHeight, &Clipper) == 0)
		return;

	// pBuffer lμ khung v? thμnh phèm
	int nPitch;
	void* pBuffer = pCanvas->LockCanvas(nPitch);
	if (pBuffer == NULL)
		return;
	pBuffer = (char*)(pBuffer) + Clipper.y * nPitch;
	void* pPalette	= pNode->m_pPalette;// palette pointer
	void* pSprite = pNode->m_pBitmap;	// sprite pointer
	int nBuffNextLine = nPitch - Clipper.width * 4;// next line add
	int nSprSkip = pNode->m_nWidth * Clipper.top + Clipper.left;
	int nSprSkipPerLine = Clipper.left + Clipper.right;
	__asm
	{
        mov     eax, pPalette
        movd    mm0, eax        // mm0: pPalette

        mov     eax, Clipper.width
        movd    mm1, eax        // mm1: Clipper.width

        //mov     eax, nMask32
        //movd    mm2, eax        // mm2: nMask32

        // mm3: nAlpha
        // mm4: temp use
        // mm7: push ecx, pop ecx
        // mm6: push edx, pop edx
        // mm5: push eax, pop eax

		//?i ??n pBuffer + Clipper.y * nPitch + Clipper.x*4
		mov		edi, pBuffer
		mov		eax, Clipper.x
		imul	eax, 4
		add		edi, eax

		//d÷ li?u sprite
		mov		esi, pSprite

		//_SkipSpriteAheadContent_:
		{
			mov		edx, nSprSkip
			or		edx, edx
			jz		_SkipSpriteAheadContentEnd_

			_SkipSpriteAheadContentLocalStart_:
			{	//??c sè l-?ng ?ióm eax vμ alpha ebx (2 bytes)
				read_alpha_2_ebx_run_length_2_eax
				or		ebx, ebx
				jnz		_SkipSpriteAheadContentLocalAlpha_
				sub		edx, eax
				jg		_SkipSpriteAheadContentLocalStart_
				neg		edx	//edx <= 0 sè l-?ng v-?t qu? c?a nSprSkip
				jmp		_SkipSpriteAheadContentEnd_

				_SkipSpriteAheadContentLocalAlpha_://n?u alpha > 0 th× esi+
				{
					add		esi, eax
					sub		edx, eax
					jg		_SkipSpriteAheadContentLocalStart_
					add		esi, edx	//edx <= 0 tr? l1i esi ptr 
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
			//v? full spr kh?ng èn ph?n nμo			
			_DrawFullLineSection_Line_:
			{
				movd	edx, mm1    // mm1: Clipper.width
				_DrawFullLineSection_LineLocal_:
				{	//??c sè l-?ng ?ióm eax vμ alpha ebx (2 bytes)
					read_alpha_2_ebx_run_length_2_eax
					or		ebx, ebx
					jnz		_DrawFullLineSection_LineLocal_Alpha_
                    lea     edi, [edi + eax * 4]//nh?y edi qua sè l-?ng ?ióm eax
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
						mov		eax, nExAlpha
						imul	ebx, eax
						shr		ebx, 8
						cmp		ebx, 255
						jl		_DrawFullLineSection_LineLocal_HalfAlpha_
						//v? ?ióm kìm theo alpha 0xff000000
						//_DrawFullLineSection_LineLocal_DirectCopy_:
						{
							movd     ebx, mm0   // mm0: pPalette
                            sub ecx, 4
                            jl  _DrawFullLineSection_CopyPixel_continue
							_DrawFullLineSection_CopyPixel4_:
							{	//v? 1 l?n 4 ?ióm
								copy_4pixel_use_eax_32b
                                sub ecx, 4
                                jg     _DrawFullLineSection_CopyPixel4_
							}
							_DrawFullLineSection_CopyPixel_continue:
                            add ecx, 4
                            jz _DrawFullLineSection_CopyPixel_End 

						    _DrawFullLineSection_CopyPixel_:
							{	//v? t?ng ?ióm
								copy_pixel_use_eax_32b
                                dec     ecx
                                jnz     _DrawFullLineSection_CopyPixel_
							}
                            _DrawFullLineSection_CopyPixel_End:

							or		edx, edx
							jnz		_DrawFullLineSection_LineLocal_
	
							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}
						//v? ?ióm kìm theo alpha < 255
						_DrawFullLineSection_LineLocal_HalfAlpha_:
						{
							movd    mm6, edx
                            movd    mm3, ebx    // mm3: nAlpha
							_DrawFullLineSection_HalfAlphaPixel_:
							{
								mix_2_pixel_color_alpha_use_eabdx_32b
								dec ecx
								jnz	_DrawFullLineSection_HalfAlphaPixel_
							}
							movd    edx, mm6
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
			movd	edx, mm1    // mm1: Clipper.width
			or		eax, eax
			jnz		_DrawPartLineSection_LineLocal_CheckAlpha_
			_DrawPartLineSection_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				_DrawPartLineSection_LineLocal_CheckAlpha_:
				or		ebx, ebx
				jnz		_DrawPartLineSection_LineLocal_Alpha_
				lea     edi, [edi + eax * 4]//nh?y edi qua sè l-?ng ?ióm eax
				sub		edx, eax
				jg		_DrawPartLineSection_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_
				
				lea     edi, [edi + edx * 4]//nh?y edi qua sè l-?ng ?ióm edx
				neg		edx
			}
			
			_DrawPartLineSection_LineSkip_:
			{
				add		edi, nBuffNextLine
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
				mov		eax, nExAlpha
				imul	ebx, eax
				shr		ebx, 8
				cmp		ebx, 255
				jl		_DrawPartLineSection_LineLocal_HalfAlpha_
						
				//_DrawPartLineSection_LineLocal_DirectCopy_:
				{
					movd     ebx, mm0 // mm0: pPalette
					_DrawPartLineSection_CopyPixel_:
					{
						copy_pixel_use_eax_32b
						dec ecx
						jnz	_DrawPartLineSection_CopyPixel_
					}
					jmp		_DrawPartLineSection_LineLocal_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_:
				{
					movd    mm6, edx
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx_32b
						dec ecx
						jnz	_DrawPartLineSection_HalfAlphaPixel_
					}
					movd    edx, mm6
					jmp		_DrawPartLineSection_LineLocal_
				}
			}
			_DrawPartLineSection_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				mov		eax, nExAlpha
				movd	mm5, ebx
				imul	ebx, eax
				shr		ebx, 8
				cmp		ebx, 255
				jl		_DrawPartLineSection_LineLocal_HalfAlpha_Part_
					
				//_DrawPartLineSection_LineLocal_DirectCopy_Part_:
				{
					movd    ebx,  mm0   // mm0: pPalette
					_DrawPartLineSection_CopyPixel_Part_:
					{
						copy_pixel_use_eax_32b
						dec ecx
						jnz	_DrawPartLineSection_CopyPixel_Part_
					}
			
					dec		Clipper.height
					jz		_EXIT_WAY_
					neg		edx
					mov		ebx, 255
					jmp		_DrawPartLineSection_LineSkip_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_Part_:
				{
					movd    mm6, edx
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_HalfAlphaPixel_Part_:
					{
						mix_2_pixel_color_alpha_use_eabdx_32b
						dec ecx
						jnz	_DrawPartLineSection_HalfAlphaPixel_Part_
					}
					movd	ebx, mm5
					movd    edx, mm6
					neg		edx
					dec		Clipper.height
					jg		_DrawPartLineSection_LineSkip_
					jmp		_EXIT_WAY_
				}
			}
		}

		_DrawPartLineSection_SkipLeft_Line_:
		{
			mov		eax, edx
			movd	edx, mm1    // mm1: Clipper.width
			or		eax, eax
			jnz		_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_
			_DrawPartLineSection_SkipLeft_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_:
				or		ebx, ebx
				jnz		_DrawPartLineSection_SkipLeft_LineLocal_Alpha_
				lea     edi, [edi + eax * 4]//nh?y edi qua sè l-?ng ?ióm eax
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
				sub		edx, eax		//?è°?eax??á?￡??a?ùáá???í?éò?2?Dèòa±￡á?eaxá?
				mov		ecx, eax
				mov		eax, nExAlpha
				imul	ebx, eax
				shr		ebx, 8
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_
						
				//_DrawPartLineSection_SkipLeft_LineLocal_DirectCopy_:
				{
					movd    ebx, mm0    // mm0: pPalette
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

				_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_:
				{
					movd    mm6, edx
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_SkipLeft_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx_32b
						dec ecx
						jnz	_DrawPartLineSection_SkipLeft_HalfAlphaPixel_
					}
					movd    edx, mm6
					or		edx, edx
					jnz		_DrawPartLineSection_SkipLeft_LineLocal_
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipLeft_LineSkip_
					jmp		_EXIT_WAY_
				}
			}
		}

		_DrawPartLineSection_SkipRight_Line_:
		{
			movd	edx, mm1    // mm1: Clipper.width
			_DrawPartLineSection_SkipRight_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				or		ebx, ebx
				jnz		_DrawPartLineSection_SkipRight_LineLocal_Alpha_
				lea     edi, [edi + eax * 4]//nh?y edi qua sè l-?ng ?ióm eax
				sub		edx, eax
				jg		_DrawPartLineSection_SkipRight_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_

				lea     edi, [edi + edx * 4]//nh?y edi qua sè l-?ng ?ióm edx
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
				mov		eax, nExAlpha
				imul	ebx, eax
				shr		ebx, 8
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_
						
				//_DrawPartLineSection_SkipRight_LineLocal_DirectCopy_:
				{
					movd    ebx, mm0    // mm0: pPalette
					_DrawPartLineSection_SkipRight_CopyPixel_:
					{
						copy_pixel_use_eax_32b
						dec ecx
						jnz	_DrawPartLineSection_SkipRight_CopyPixel_
					}
					jmp		_DrawPartLineSection_SkipRight_LineLocal_
				}
				
				_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_:
				{
					movd    mm6, edx
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_SkipRight_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx_32b
						dec ecx
						jnz	_DrawPartLineSection_SkipRight_HalfAlphaPixel_
					}
					movd	edx, mm6
					jmp		_DrawPartLineSection_SkipRight_LineLocal_
				}
			}
			_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				mov		eax, nExAlpha
				movd	mm5, ebx
				imul	ebx, eax
				shr		ebx, 8
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_
					
				//_DrawPartLineSection_SkipRight_LineLocal_DirectCopy_Part_:
				{
					movd    ebx, mm0 // mm0: pPalette
					_DrawPartLineSection_SkipRight_CopyPixel_Part_:
					{
						copy_pixel_use_eax_32b
						dec ecx
						jnz	_DrawPartLineSection_SkipRight_CopyPixel_Part_
					}
					neg		edx
					mov		ebx, 255	//è?1???òaè·?Dμ??-ebx(alpha)?μ?éò??ú?°í·push ebx￡?′?′|pop??μ?
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipRight_LineSkip_
					jmp		_EXIT_WAY_
				}
				
				_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_:
				{
					movd    mm6, edx
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_SkipRight_HalfAlphaPixel_Part_:
					{
						mix_2_pixel_color_alpha_use_eabdx_32b
						dec ecx
						jnz	_DrawPartLineSection_SkipRight_HalfAlphaPixel_Part_
					}
					movd	ebx, mm5
					movd	edx, mm6
					neg		edx
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipRight_LineSkip_//è?1???òaè·?Dμ??-ebx(alpha)?μ?éò??ú?°í·push ebx￡?′?′|pop??μ?
					jmp		_EXIT_WAY_
				}
			}
		}
		_EXIT_WAY_:
        emms
	}
	pCanvas->UnlockCanvas();
#endif
}

void g_DrawSpriteScreen(void* node, void* canvas, UINT nColor)
{
#ifndef _WIN64
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	int nExAlpha = pNode->m_nAlpha;
	if(nExAlpha <= 0)
		return;
	KClipper Clipper;
	if (pCanvas->MakeClip(pNode->m_nX, pNode->m_nY, pNode->m_nWidth, pNode->m_nHeight, &Clipper) == 0)
		return;

	int nPitch;
	BYTE* pBuffer = (BYTE*)pCanvas->LockCanvas(nPitch);
	if (pBuffer == NULL)
		return;
	pBuffer += Clipper.y * nPitch + Clipper.x * 2;
	BYTE* pPalette	= (BYTE*)pNode->m_pPalette;// palette pointer
	BYTE* pSprite = (BYTE*)pNode->m_pBitmap;	// sprite pointer
	int nBuffNextLine = nPitch - Clipper.width * 2;// next line add
	int nSprSkip = pNode->m_nWidth * Clipper.top + Clipper.left;
	int nSprSkipPerLine = Clipper.left + Clipper.right;//left:sè ?ióm mêt t? tr?i qua, right: sè ?ióm mêt t? lò ph?i qua tr?i
	UINT tmpAlpha, alpha, nSrcColor, nDstColor, R,G,B;
	int nPixelBatch, nWidth;
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
		pBuffer += nPixelBatch*2; //ph?n nμy kh?ng v?, ch? nh?y qua
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
		if(alpha)
		{
			if(nColor)
			{
				while(nPixelBatch)
				{
					nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
					R = (nSrcColor & 0xf800) >> 8;
					G = (nSrcColor & 0x07e0) >> 3;
					B = (nSrcColor & 0x001f) << 3;
					if(R > 8 || G > 8 || B > 8)
					{
						nSrcColor = g_BlendColor16b(R,G,B, nColor, 1);
						R = (nSrcColor & 0xf800) >> 8;
						G = (nSrcColor & 0x07e0) >> 3;
						B = (nSrcColor & 0x001f) << 3;
						nDstColor = *((WORD*)pBuffer);
						nDstColor = g_BlendColor16b(R,G,B, nDstColor, 2, TRUE, alpha);
						*((WORD*)pBuffer) = nDstColor;
					}
					pBuffer += 2;
					nPixelBatch--;
				}
			}
			else
			{
				while(nPixelBatch)
				{
					nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
					R = (nSrcColor & 0xf800) >> 8;
					G = (nSrcColor & 0x07e0) >> 3;
					B = (nSrcColor & 0x001f) << 3;
					if(R > 8 || G > 8 || B > 8)
					{
						nDstColor = *((WORD*)pBuffer);
						nDstColor = g_BlendColor16b(R,G,B, nDstColor, 2, TRUE, alpha);
						*((WORD*)pBuffer) = nDstColor;
					}
					pBuffer += 2;
					nPixelBatch--;
				}
			}
		}
		else
		{
			while(nPixelBatch)
			{
				pSprite++;
				pBuffer += 2;
				nPixelBatch--;
			}
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
			pBuffer += nPixelBatch*2;
			goto	_DrawPartLineSection_LineLocal_;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		pBuffer += (nPixelBatch + nWidth)*2;
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
		if(alpha)
		{
			if(nColor)
			{
				while(nPixelBatch)
				{
					nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
					R = (nSrcColor & 0xf800) >> 8;
					G = (nSrcColor & 0x07e0) >> 3;
					B = (nSrcColor & 0x001f) << 3;
					if(R > 8 || G > 8 || B > 8)
					{
						nSrcColor = g_BlendColor16b(R,G,B, nColor, 1);
						R = (nSrcColor & 0xf800) >> 8;
						G = (nSrcColor & 0x07e0) >> 3;
						B = (nSrcColor & 0x001f) << 3;
						nDstColor = *((WORD*)pBuffer);
						nDstColor = g_BlendColor16b(R,G,B, nDstColor, 2, TRUE, alpha);
						*((WORD*)pBuffer) = nDstColor;
					}
					pBuffer += 2;
					nPixelBatch--;
				}
			}
			else
			{
				while(nPixelBatch)
				{
					nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
					R = (nSrcColor & 0xf800) >> 8;
					G = (nSrcColor & 0x07e0) >> 3;
					B = (nSrcColor & 0x001f) << 3;
					if(R > 8 || G > 8 || B > 8)
					{
						nDstColor = *((WORD*)pBuffer);
						nDstColor = g_BlendColor16b(R,G,B, nDstColor, 2, TRUE, alpha);
						*((WORD*)pBuffer) = nDstColor;
					}
					pBuffer += 2;
					nPixelBatch--;
				}
			}
		}
		else
		{
			while(nPixelBatch)
			{
				pSprite++;
				pBuffer += 2;
				nPixelBatch--;
			}
		}
		goto	_DrawPartLineSection_LineLocal_;
	_DrawPartLineSection_LineLocal_Alpha_Part_:
		tmpAlpha = alpha;
		alpha = alpha*nExAlpha/255;
		if(alpha)
		{
			if(nColor)
			{
				while(nPixelBatch)
				{
					nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
					R = (nSrcColor & 0xf800) >> 8;
					G = (nSrcColor & 0x07e0) >> 3;
					B = (nSrcColor & 0x001f) << 3;
					if(R > 8 || G > 8 || B > 8)
					{
						nSrcColor = g_BlendColor16b(R,G,B, nColor, 1);
						R = (nSrcColor & 0xf800) >> 8;
						G = (nSrcColor & 0x07e0) >> 3;
						B = (nSrcColor & 0x001f) << 3;
						nDstColor = *((WORD*)pBuffer);
						nDstColor = g_BlendColor16b(R,G,B, nDstColor, 2, TRUE, alpha);
						*((WORD*)pBuffer) = nDstColor;
					}
					pBuffer += 2;
					nPixelBatch--;
				}
			}
			else
			{
				while(nPixelBatch)
				{
					nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
					R = (nSrcColor & 0xf800) >> 8;
					G = (nSrcColor & 0x07e0) >> 3;
					B = (nSrcColor & 0x001f) << 3;
					if(R > 8 || G > 8 || B > 8)
					{
						nDstColor = *((WORD*)pBuffer);
						nDstColor = g_BlendColor16b(R,G,B, nDstColor, 2, TRUE, alpha);
						*((WORD*)pBuffer) = nDstColor;
					}
					pBuffer += 2;
					nPixelBatch--;
				}
			}
		}
		else
		{
			while(nPixelBatch)
			{
				pSprite++;
				pBuffer += 2;
				nPixelBatch--;
			}
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
		pBuffer += nPixelBatch*2; //ph?n nμy kh?ng v?, ch? nh?y qua
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
		if(alpha)
		{
			if(nColor)
			{
				while(nPixelBatch)
				{
					nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
					R = (nSrcColor & 0xf800) >> 8;
					G = (nSrcColor & 0x07e0) >> 3;
					B = (nSrcColor & 0x001f) << 3;
					if(R > 8 || G > 8 || B > 8)
					{
						nSrcColor = g_BlendColor16b(R,G,B, nColor, 1);
						R = (nSrcColor & 0xf800) >> 8;
						G = (nSrcColor & 0x07e0) >> 3;
						B = (nSrcColor & 0x001f) << 3;
						nDstColor = *((WORD*)pBuffer);
						nDstColor = g_BlendColor16b(R,G,B, nDstColor, 2, TRUE, alpha);
						*((WORD*)pBuffer) = nDstColor;
					}
					pBuffer += 2;
					nPixelBatch--;
				}
			}
			else
			{
				while(nPixelBatch)
				{
					nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
					R = (nSrcColor & 0xf800) >> 8;
					G = (nSrcColor & 0x07e0) >> 3;
					B = (nSrcColor & 0x001f) << 3;
					if(R > 8 || G > 8 || B > 8)
					{
						nDstColor = *((WORD*)pBuffer);
						nDstColor = g_BlendColor16b(R,G,B, nDstColor, 2, TRUE, alpha);
						*((WORD*)pBuffer) = nDstColor;
					}
					pBuffer += 2;
					nPixelBatch--;
				}
			}
		}
		else
		{
			while(nPixelBatch)
			{
				pSprite++;
				pBuffer += 2;
				nPixelBatch--;
			}
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
			pBuffer += nPixelBatch*2; //ph?n nμy kh?ng v?, ch? nh?y qua
			goto	_DrawPartLineSection_SkipRight_LineLocal_;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		pBuffer += (nPixelBatch + nWidth)*2;
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
		if(alpha)
		{
			if(nColor)
			{
				while(nPixelBatch)
				{
					nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
					R = (nSrcColor & 0xf800) >> 8;
					G = (nSrcColor & 0x07e0) >> 3;
					B = (nSrcColor & 0x001f) << 3;
					if(R > 8 || G > 8 || B > 8)
					{
						nSrcColor = g_BlendColor16b(R,G,B, nColor, 1);
						R = (nSrcColor & 0xf800) >> 8;
						G = (nSrcColor & 0x07e0) >> 3;
						B = (nSrcColor & 0x001f) << 3;
						nDstColor = *((WORD*)pBuffer);
						nDstColor = g_BlendColor16b(R,G,B, nDstColor, 2, TRUE, alpha);
						*((WORD*)pBuffer) = nDstColor;
					}
					pBuffer += 2;
					nPixelBatch--;
				}
			}
			else
			{
				while(nPixelBatch)
				{
					nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
					R = (nSrcColor & 0xf800) >> 8;
					G = (nSrcColor & 0x07e0) >> 3;
					B = (nSrcColor & 0x001f) << 3;
					if(R > 8 || G > 8 || B > 8)
					{
						nDstColor = *((WORD*)pBuffer);
						nDstColor = g_BlendColor16b(R,G,B, nDstColor, 2, TRUE, alpha);
						*((WORD*)pBuffer) = nDstColor;
					}
					pBuffer += 2;
					nPixelBatch--;
				}
			}
		}
		else
		{
			while(nPixelBatch)
			{
				pSprite++;
				pBuffer += 2;
				nPixelBatch--;
			}
		}
		goto	_DrawPartLineSection_SkipRight_LineLocal_;
	_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_:
		tmpAlpha = alpha;
		alpha = alpha*nExAlpha/255;
		if(alpha)
		{
			if(nColor)
			{
				while(nPixelBatch)
				{
					nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
					R = (nSrcColor & 0xf800) >> 8;
					G = (nSrcColor & 0x07e0) >> 3;
					B = (nSrcColor & 0x001f) << 3;
					if(R > 8 || G > 8 || B > 8)
					{
						nSrcColor = g_BlendColor16b(R,G,B, nColor, 1);
						R = (nSrcColor & 0xf800) >> 8;
						G = (nSrcColor & 0x07e0) >> 3;
						B = (nSrcColor & 0x001f) << 3;
						nDstColor = *((WORD*)pBuffer);
						nDstColor = g_BlendColor16b(R,G,B, nDstColor, 2, TRUE, alpha);
						*((WORD*)pBuffer) = nDstColor;
					}
					pBuffer += 2;
					nPixelBatch--;
				}
			}
			else
			{
				while(nPixelBatch)
				{
					nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
					R = (nSrcColor & 0xf800) >> 8;
					G = (nSrcColor & 0x07e0) >> 3;
					B = (nSrcColor & 0x001f) << 3;
					if(R > 8 || G > 8 || B > 8)
					{
						nDstColor = *((WORD*)pBuffer);
						nDstColor = g_BlendColor16b(R,G,B, nDstColor, 2, TRUE, alpha);
						*((WORD*)pBuffer) = nDstColor;
					}
					pBuffer += 2;
					nPixelBatch--;
				}
			}
		}
		else
		{
			while(nPixelBatch)
			{
				pSprite++;
				pBuffer += 2;
				nPixelBatch--;
			}
		}
		alpha = tmpAlpha;
		nWidth = -nWidth;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipRight_LineSkip_;
		goto	_EXIT_WAY_;
	
	_EXIT_WAY_:
	pCanvas->UnlockCanvas();
#endif
}

void g_DrawSpriteBlendColor(void* node, void* canvas, UINT nColor)
{
#ifndef _WIN64
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	int nExAlpha = pNode->m_nAlpha;
	if(nExAlpha <= 0)
		return;
	KClipper Clipper;
	if (pCanvas->MakeClip(pNode->m_nX, pNode->m_nY, pNode->m_nWidth, pNode->m_nHeight, &Clipper) == 0)
		return;

	int nPitch;
	BYTE* pBuffer = (BYTE*)pCanvas->LockCanvas(nPitch);
	if (pBuffer == NULL)
		return;
	pBuffer += Clipper.y * nPitch + Clipper.x * 2;
	BYTE* pPalette	= (BYTE*)pNode->m_pPalette;// palette pointer
	BYTE* pSprite = (BYTE*)pNode->m_pBitmap;	// sprite pointer
	int nMode = pNode->m_nColor;
	int nMask32 = pCanvas->m_nMask32;	// rgb mask32
	int nBuffNextLine = nPitch - Clipper.width * 2;// next line add
	int nSprSkip = pNode->m_nWidth * Clipper.top + Clipper.left;
	int nSprSkipPerLine = Clipper.left + Clipper.right;//left:sè ?ióm mêt t? tr?i qua, right: sè ?ióm mêt t? lò ph?i qua tr?i
	UINT tmpAlpha, alpha, nSrcColor, nDstColor;
	int nPixelBatch, nWidth;
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
		pBuffer += nPixelBatch*2; //ph?n nμy kh?ng v?, ch? nh?y qua
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
			*((WORD*)pBuffer) = nSrcColor;
			pBuffer += 2;
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
		alpha >>= 3;
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nSrcColor = g_BlendColor16b(nSrcColor, nColor, nMode);			
			nSrcColor |= (nSrcColor << 16);
			nSrcColor &= nMask32;
			nDstColor = *((WORD*)pBuffer);
			nDstColor |= (nDstColor << 16);
			nDstColor &= nMask32;
			nDstColor = (nSrcColor - nDstColor)*alpha/32 + nDstColor;
			nDstColor &= nMask32;
			nSrcColor = (nDstColor >> 16) & 0x7e0;
			nDstColor = (nDstColor | nSrcColor) & 0xffff;
			*((WORD*)pBuffer) = nDstColor;
			pBuffer += 2;
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
			pBuffer += nPixelBatch*2;
			goto	_DrawPartLineSection_LineLocal_;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		pBuffer += (nPixelBatch + nWidth)*2;
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
			*((WORD*)pBuffer) = nSrcColor;
			pBuffer += 2;
			nPixelBatch--;
		}
		goto	_DrawPartLineSection_LineLocal_;
	_DrawPartLineSection_LineLocal_HalfAlpha_:
		alpha >>= 3;
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nSrcColor = g_BlendColor16b(nSrcColor, nColor, nMode);			
			nSrcColor |= (nSrcColor << 16);
			nSrcColor &= nMask32;
			nDstColor = *((WORD*)pBuffer);
			nDstColor |= (nDstColor << 16);
			nDstColor &= nMask32;
			nDstColor = (nSrcColor - nDstColor)*alpha/32 + nDstColor;
			nDstColor &= nMask32;
			nSrcColor = (nDstColor >> 16) & 0x7e0;
			nDstColor = (nDstColor | nSrcColor) & 0xffff;
			*((WORD*)pBuffer) = nDstColor;
			pBuffer += 2;
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
			*((WORD*)pBuffer) = nSrcColor;
			pBuffer += 2;
			nPixelBatch--;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		goto	_DrawPartLineSection_LineSkip_;
	_DrawPartLineSection_LineLocal_HalfAlpha_Part_:
		alpha >>= 3;
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nSrcColor = g_BlendColor16b(nSrcColor, nColor, nMode);			
			nSrcColor |= (nSrcColor << 16);
			nSrcColor &= nMask32;
			nDstColor = *((WORD*)pBuffer);
			nDstColor |= (nDstColor << 16);
			nDstColor &= nMask32;
			nDstColor = (nSrcColor - nDstColor)*alpha/32 + nDstColor;
			nDstColor &= nMask32;
			nSrcColor = (nDstColor >> 16) & 0x7e0;
			nDstColor = (nDstColor | nSrcColor) & 0xffff;
			*((WORD*)pBuffer) = nDstColor;
			pBuffer += 2;
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
		pBuffer += nPixelBatch*2; //ph?n nμy kh?ng v?, ch? nh?y qua
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
			*((WORD*)pBuffer) = nSrcColor;
			pBuffer += 2;
			nPixelBatch--;
		}
		if(nWidth)
			goto	_DrawPartLineSection_SkipLeft_LineLocal_;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipLeft_LineSkip_;
		goto	_EXIT_WAY_;
	_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_:
		alpha >>= 3;
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nSrcColor = g_BlendColor16b(nSrcColor, nColor, nMode);			
			nSrcColor |= (nSrcColor << 16);
			nSrcColor &= nMask32;
			nDstColor = *((WORD*)pBuffer);
			nDstColor |= (nDstColor << 16);
			nDstColor &= nMask32;
			nDstColor = (nSrcColor - nDstColor)*alpha/32 + nDstColor;
			nDstColor &= nMask32;
			nSrcColor = (nDstColor >> 16) & 0x7e0;
			nDstColor = (nDstColor | nSrcColor) & 0xffff;
			*((WORD*)pBuffer) = nDstColor;
			pBuffer += 2;
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
			pBuffer += nPixelBatch*2; //ph?n nμy kh?ng v?, ch? nh?y qua
			goto	_DrawPartLineSection_SkipRight_LineLocal_;
		}
		Clipper.height--;
		if(!Clipper.height)
			goto	_EXIT_WAY_;
		pBuffer += (nPixelBatch + nWidth)*2;
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
			*((WORD*)pBuffer) = nSrcColor;
			pBuffer += 2;
			nPixelBatch--;
		}
		goto	_DrawPartLineSection_SkipRight_LineLocal_;
	_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_:
		alpha >>= 3;
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nSrcColor = g_BlendColor16b(nSrcColor, nColor, nMode);			
			nSrcColor |= (nSrcColor << 16);
			nSrcColor &= nMask32;
			nDstColor = *((WORD*)pBuffer);
			nDstColor |= (nDstColor << 16);
			nDstColor &= nMask32;
			nDstColor = (nSrcColor - nDstColor)*alpha/32 + nDstColor;
			nDstColor &= nMask32;
			nSrcColor = (nDstColor >> 16) & 0x7e0;
			nDstColor = (nDstColor | nSrcColor) & 0xffff;
			*((WORD*)pBuffer) = nDstColor;
			pBuffer += 2;
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
			*((WORD*)pBuffer) = nSrcColor;
			pBuffer += 2;
			nPixelBatch--;
		}
		nWidth = -nWidth;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipRight_LineSkip_;
		goto	_EXIT_WAY_;
	_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_:
		alpha >>= 3;
		while(nPixelBatch)
		{
			nSrcColor = *((WORD*)(pPalette + 2*(*(pSprite++))));
			nSrcColor = g_BlendColor16b(nSrcColor, nColor, nMode);			
			nSrcColor |= (nSrcColor << 16);
			nSrcColor &= nMask32;
			nDstColor = *((WORD*)pBuffer);
			nDstColor |= (nDstColor << 16);
			nDstColor &= nMask32;
			nDstColor = (nSrcColor - nDstColor)*alpha/32 + nDstColor;
			nDstColor &= nMask32;
			nSrcColor = (nDstColor >> 16) & 0x7e0;
			nDstColor = (nDstColor | nSrcColor) & 0xffff;
			*((WORD*)pBuffer) = nDstColor;
			pBuffer += 2;
			nPixelBatch--;
		}
		alpha = tmpAlpha;
		nWidth = -nWidth;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipRight_LineSkip_;
		goto	_EXIT_WAY_;
	
	_EXIT_WAY_:
	pCanvas->UnlockCanvas();
#endif
}

void g_DrawSpriteScreen32b(void* node, void* canvas, UINT nColor)
{
#ifndef _WIN64
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	int nExAlpha = pNode->m_nAlpha;
	if(nExAlpha <= 0)
		return;
	KClipper Clipper;
	if (pCanvas->MakeClip(pNode->m_nX, pNode->m_nY, pNode->m_nWidth, pNode->m_nHeight, &Clipper) == 0)
		return;

	int nPitch;
	BYTE* pBuffer = (BYTE*)pCanvas->LockCanvas(nPitch);
	if (pBuffer == NULL)
		return;
	pBuffer += Clipper.y * nPitch + Clipper.x * 4;
	BYTE* pPalette	= (BYTE*)pNode->m_pPalette;// palette pointer
	BYTE* pSprite = (BYTE*)pNode->m_pBitmap;	// sprite pointer
	int nMode = pNode->m_nColor;
	int nBuffNextLine = nPitch - Clipper.width * 4;// next line add
	int nSprSkip = pNode->m_nWidth * Clipper.top + Clipper.left;
	int nSprSkipPerLine = Clipper.left + Clipper.right;//left:sè ?ióm mêt t? tr?i qua, right: sè ?ióm mêt t? lò ph?i qua tr?i
	UINT tmpAlpha, alpha, nSrcColor, nDstColor, R,G,B;
	int nPixelBatch, nWidth;
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
		if(alpha)
		{
			if(nColor)
			{
				while(nPixelBatch)
				{
					R = *(pPalette + 3*(*pSprite));
					G = *(pPalette + 3*(*pSprite) + 1);
					B = *(pPalette + 3*(*pSprite) + 2);
					pSprite++;
					if(R > 8 || G > 8 || B > 8)
					{
						nSrcColor = g_BlendColor32b(R,G,B, nColor, nMode);
						R = (nSrcColor & 0xff0000) >> 16;
						G = (nSrcColor & 0xff00) >> 8;
						B = (nSrcColor & 0xff);
						nDstColor = *((UINT*)pBuffer);
						nDstColor = g_BlendColor32b(R,G,B, nDstColor, 2, alpha);
						nDstColor |= 0xff000000;
						*((UINT*)pBuffer) = nDstColor;
					}
					pBuffer += 4;
					nPixelBatch--;
				}
			}
			else
			{
				while(nPixelBatch)
				{
					R = *(pPalette + 3*(*pSprite));
					G = *(pPalette + 3*(*pSprite) + 1);
					B = *(pPalette + 3*(*pSprite) + 2);
					pSprite++;
					if(R > 8 || G > 8 || B > 8)
					{
						nDstColor = *((UINT*)pBuffer);
						nDstColor = g_BlendColor32b(R,G,B, nDstColor, 2, alpha);
						nDstColor |= 0xff000000;
						*((UINT*)pBuffer) = nDstColor;
					}
					pBuffer += 4;
					nPixelBatch--;
				}
			}
		}
		else
		{
			while(nPixelBatch)
			{
				pSprite++;
				pBuffer += 4;
				nPixelBatch--;
			}
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
		if(alpha)
		{
			if(nColor)
			{
				while(nPixelBatch)
				{
					R = *(pPalette + 3*(*pSprite));
					G = *(pPalette + 3*(*pSprite) + 1);
					B = *(pPalette + 3*(*pSprite) + 2);
					pSprite++;
					if(R > 8 || G > 8 || B > 8)
					{
						nSrcColor = g_BlendColor32b(R,G,B, nColor, nMode);
						R = (nSrcColor & 0xff0000) >> 16;
						G = (nSrcColor & 0xff00) >> 8;
						B = (nSrcColor & 0xff);
						nDstColor = *((UINT*)pBuffer);
						nDstColor = g_BlendColor32b(R,G,B, nDstColor, 2, alpha);
						nDstColor |= 0xff000000;
						*((UINT*)pBuffer) = nDstColor;
					}
					pBuffer += 4;
					nPixelBatch--;
				}
			}
			else
			{
				while(nPixelBatch)
				{
					R = *(pPalette + 3*(*pSprite));
					G = *(pPalette + 3*(*pSprite) + 1);
					B = *(pPalette + 3*(*pSprite) + 2);
					pSprite++;
					if(R > 8 || G > 8 || B > 8)
					{
						nDstColor = *((UINT*)pBuffer);
						nDstColor = g_BlendColor32b(R,G,B, nDstColor, 2, alpha);
						nDstColor |= 0xff000000;
						*((UINT*)pBuffer) = nDstColor;
					}
					pBuffer += 4;
					nPixelBatch--;
				}
			}
		}
		else
		{
			while(nPixelBatch)
			{
				pSprite++;
				pBuffer += 4;
				nPixelBatch--;
			}
		}
		goto	_DrawPartLineSection_LineLocal_;
	_DrawPartLineSection_LineLocal_Alpha_Part_:
		tmpAlpha = alpha;
		alpha = alpha*nExAlpha/255;
		if(alpha)
		{
			if(nColor)
			{
				while(nPixelBatch)
				{
					R = *(pPalette + 3*(*pSprite));
					G = *(pPalette + 3*(*pSprite) + 1);
					B = *(pPalette + 3*(*pSprite) + 2);
					pSprite++;
					if(R > 8 || G > 8 || B > 8)
					{
						nSrcColor = g_BlendColor32b(R,G,B, nColor, nMode);
						R = (nSrcColor & 0xff0000) >> 16;
						G = (nSrcColor & 0xff00) >> 8;
						B = (nSrcColor & 0xff);
						nDstColor = *((UINT*)pBuffer);
						nDstColor = g_BlendColor32b(R,G,B, nDstColor, 2, alpha);
						nDstColor |= 0xff000000;
						*((UINT*)pBuffer) = nDstColor;
					}
					pBuffer += 4;
					nPixelBatch--;
				}
			}
			else
			{
				while(nPixelBatch)
				{
					R = *(pPalette + 3*(*pSprite));
					G = *(pPalette + 3*(*pSprite) + 1);
					B = *(pPalette + 3*(*pSprite) + 2);
					pSprite++;
					if(R > 8 || G > 8 || B > 8)
					{
						nDstColor = *((UINT*)pBuffer);
						nDstColor = g_BlendColor32b(R,G,B, nDstColor, 2, alpha);
						nDstColor |= 0xff000000;
						*((UINT*)pBuffer) = nDstColor;
					}
					pBuffer += 4;
					nPixelBatch--;
				}
			}
		}
		else
		{
			while(nPixelBatch)
			{
				pSprite++;
				pBuffer += 4;
				nPixelBatch--;
			}
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
		if(alpha)
		{
			if(nColor)
			{
				while(nPixelBatch)
				{
					R = *(pPalette + 3*(*pSprite));
					G = *(pPalette + 3*(*pSprite) + 1);
					B = *(pPalette + 3*(*pSprite) + 2);
					pSprite++;
					if(R > 8 || G > 8 || B > 8)
					{
						nSrcColor = g_BlendColor32b(R,G,B, nColor, nMode);
						R = (nSrcColor & 0xff0000) >> 16;
						G = (nSrcColor & 0xff00) >> 8;
						B = (nSrcColor & 0xff);
						nDstColor = *((UINT*)pBuffer);
						nDstColor = g_BlendColor32b(R,G,B, nDstColor, 2, alpha);
						nDstColor |= 0xff000000;
						*((UINT*)pBuffer) = nDstColor;
					}
					pBuffer += 4;
					nPixelBatch--;
				}
			}
			else
			{
				while(nPixelBatch)
				{
					R = *(pPalette + 3*(*pSprite));
					G = *(pPalette + 3*(*pSprite) + 1);
					B = *(pPalette + 3*(*pSprite) + 2);
					pSprite++;
					if(R > 8 || G > 8 || B > 8)
					{
						nDstColor = *((UINT*)pBuffer);
						nDstColor = g_BlendColor32b(R,G,B, nDstColor, 2, alpha);
						nDstColor |= 0xff000000;
						*((UINT*)pBuffer) = nDstColor;
					}
					pBuffer += 4;
					nPixelBatch--;
				}
			}
		}
		else
		{
			while(nPixelBatch)
			{
				pSprite++;
				pBuffer += 4;
				nPixelBatch--;
			}
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
		if(alpha)
		{
			if(nColor)
			{
				while(nPixelBatch)
				{
					R = *(pPalette + 3*(*pSprite));
					G = *(pPalette + 3*(*pSprite) + 1);
					B = *(pPalette + 3*(*pSprite) + 2);
					pSprite++;
					if(R > 8 || G > 8 || B > 8)
					{
						nSrcColor = g_BlendColor32b(R,G,B, nColor, nMode);
						R = (nSrcColor & 0xff0000) >> 16;
						G = (nSrcColor & 0xff00) >> 8;
						B = (nSrcColor & 0xff);
						nDstColor = *((UINT*)pBuffer);
						nDstColor = g_BlendColor32b(R,G,B, nDstColor, 2, alpha);
						nDstColor |= 0xff000000;
						*((UINT*)pBuffer) = nDstColor;
					}
					pBuffer += 4;
					nPixelBatch--;
				}
			}
			else
			{
				while(nPixelBatch)
				{
					R = *(pPalette + 3*(*pSprite));
					G = *(pPalette + 3*(*pSprite) + 1);
					B = *(pPalette + 3*(*pSprite) + 2);
					pSprite++;
					if(R > 8 || G > 8 || B > 8)
					{
						nDstColor = *((UINT*)pBuffer);
						nDstColor = g_BlendColor32b(R,G,B, nDstColor, 2, alpha);
						nDstColor |= 0xff000000;
						*((UINT*)pBuffer) = nDstColor;
					}
					pBuffer += 4;
					nPixelBatch--;
				}
			}
		}
		else
		{
			while(nPixelBatch)
			{
				pSprite++;
				pBuffer += 4;
				nPixelBatch--;
			}
		}
		goto	_DrawPartLineSection_SkipRight_LineLocal_;
	_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_:
		tmpAlpha = alpha;
		alpha = alpha*nExAlpha/255;
		if(alpha)
		{
			if(nColor)
			{
				while(nPixelBatch)
				{
					R = *(pPalette + 3*(*pSprite));
					G = *(pPalette + 3*(*pSprite) + 1);
					B = *(pPalette + 3*(*pSprite) + 2);
					pSprite++;
					if(R > 8 || G > 8 || B > 8)
					{
						nSrcColor = g_BlendColor32b(R,G,B, nColor, nMode);
						R = (nSrcColor & 0xff0000) >> 16;
						G = (nSrcColor & 0xff00) >> 8;
						B = (nSrcColor & 0xff);
						nDstColor = *((UINT*)pBuffer);
						nDstColor = g_BlendColor32b(R,G,B, nDstColor, 2, alpha);
						nDstColor |= 0xff000000;
						*((UINT*)pBuffer) = nDstColor;
					}
					pBuffer += 4;
					nPixelBatch--;
				}
			}
			else
			{
				while(nPixelBatch)
				{
					R = *(pPalette + 3*(*pSprite));
					G = *(pPalette + 3*(*pSprite) + 1);
					B = *(pPalette + 3*(*pSprite) + 2);
					pSprite++;
					if(R > 8 || G > 8 || B > 8)
					{
						nDstColor = *((UINT*)pBuffer);
						nDstColor = g_BlendColor32b(R,G,B, nDstColor, 2, alpha);
						nDstColor |= 0xff000000;
						*((UINT*)pBuffer) = nDstColor;
					}
					pBuffer += 4;
					nPixelBatch--;
				}
			}
		}
		else
		{
			while(nPixelBatch)
			{
				pSprite++;
				pBuffer += 4;
				nPixelBatch--;
			}
		}
		alpha = tmpAlpha;
		nWidth = -nWidth;
		Clipper.height--;
		if(Clipper.height > 0)
			goto	_DrawPartLineSection_SkipRight_LineSkip_;
		goto	_EXIT_WAY_;
	
	_EXIT_WAY_:
	pCanvas->UnlockCanvas();
#endif
}

void g_DrawSpriteBlendColor32b(void* node, void* canvas, UINT nColor)
{
#ifndef _WIN64
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	int nExAlpha = pNode->m_nAlpha;
	if(nExAlpha <= 0)
		return;
	KClipper Clipper;
	if (pCanvas->MakeClip(pNode->m_nX, pNode->m_nY, pNode->m_nWidth, pNode->m_nHeight, &Clipper) == 0)
		return;

	int nPitch;
	BYTE* pBuffer = (BYTE*)pCanvas->LockCanvas(nPitch);
	if (pBuffer == NULL)
		return;
	pBuffer += Clipper.y * nPitch + Clipper.x * 4;
	BYTE* pPalette	= (BYTE*)pNode->m_pPalette;// palette pointer
	BYTE* pSprite = (BYTE*)pNode->m_pBitmap;	// sprite pointer
	int nMode = pNode->m_nColor;
	int nBuffNextLine = nPitch - Clipper.width * 4;// next line add
	int nSprSkip = pNode->m_nWidth * Clipper.top + Clipper.left;
	int nSprSkipPerLine = Clipper.left + Clipper.right;//left:sè ?ióm mêt t? tr?i qua, right: sè ?ióm mêt t? lò ph?i qua tr?i
	UINT tmpAlpha, alpha, nSrcColor, nDstColor, R,G,B,R2,G2,B2;
	int nPixelBatch, nWidth;
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
			nSrcColor |= 0xff000000;
			*((UINT*)pBuffer) = nSrcColor;
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
			g_BlendColor32b(R,G,B, nColor, nMode, R,G,B);			
			nDstColor = *((UINT*)pBuffer);
			R2 = (nDstColor >> 16) & 0xff;
			G2 = (nDstColor >> 8) & 0xff;
			B2 = nDstColor & 0xff;
			R = (alpha*R + (255-alpha)*R2)/255;
			G = (alpha*G + (255-alpha)*G2)/255;
			B = (alpha*B + (255-alpha)*B2)/255;
			nDstColor = 0xff000000 | (R << 16) | (G << 8) | B;
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
			nSrcColor |= 0xff000000;
			*((UINT*)pBuffer) = nSrcColor;
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
			g_BlendColor32b(R,G,B, nColor, nMode, R,G,B);			
			nDstColor = *((UINT*)pBuffer);
			R2 = (nDstColor >> 16) & 0xff;
			G2 = (nDstColor >> 8) & 0xff;
			B2 = nDstColor & 0xff;
			R = (alpha*R + (255-alpha)*R2)/255;
			G = (alpha*G + (255-alpha)*G2)/255;
			B = (alpha*B + (255-alpha)*B2)/255;
			nDstColor = 0xff000000 | (R << 16) | (G << 8) | B;
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
			nSrcColor |= 0xff000000;
			*((UINT*)pBuffer) = nSrcColor;
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
			g_BlendColor32b(R,G,B, nColor, nMode, R,G,B);			
			nDstColor = *((UINT*)pBuffer);
			R2 = (nDstColor >> 16) & 0xff;
			G2 = (nDstColor >> 8) & 0xff;
			B2 = nDstColor & 0xff;
			R = (alpha*R + (255-alpha)*R2)/255;
			G = (alpha*G + (255-alpha)*G2)/255;
			B = (alpha*B + (255-alpha)*B2)/255;
			nDstColor = 0xff000000 | (R << 16) | (G << 8) | B;
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
			nSrcColor |= 0xff000000;
			*((UINT*)pBuffer) = nSrcColor;
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
			g_BlendColor32b(R,G,B, nColor, nMode, R,G,B);			
			nDstColor = *((UINT*)pBuffer);
			R2 = (nDstColor >> 16) & 0xff;
			G2 = (nDstColor >> 8) & 0xff;
			B2 = nDstColor & 0xff;
			R = (alpha*R + (255-alpha)*R2)/255;
			G = (alpha*G + (255-alpha)*G2)/255;
			B = (alpha*B + (255-alpha)*B2)/255;
			nDstColor = 0xff000000 | (R << 16) | (G << 8) | B;
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
			nSrcColor |= 0xff000000;
			*((UINT*)pBuffer) = nSrcColor;
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
			g_BlendColor32b(R,G,B, nColor, nMode, R,G,B);			
			nDstColor = *((UINT*)pBuffer);
			R2 = (nDstColor >> 16) & 0xff;
			G2 = (nDstColor >> 8) & 0xff;
			B2 = nDstColor & 0xff;
			R = (alpha*R + (255-alpha)*R2)/255;
			G = (alpha*G + (255-alpha)*G2)/255;
			B = (alpha*B + (255-alpha)*B2)/255;
			nDstColor = 0xff000000 | (R << 16) | (G << 8) | B;
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
			nSrcColor |= 0xff000000;
			*((UINT*)pBuffer) = nSrcColor;
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
			g_BlendColor32b(R,G,B, nColor, nMode, R,G,B);			
			nDstColor = *((UINT*)pBuffer);
			R2 = (nDstColor >> 16) & 0xff;
			G2 = (nDstColor >> 8) & 0xff;
			B2 = nDstColor & 0xff;
			R = (alpha*R + (255-alpha)*R2)/255;
			G = (alpha*G + (255-alpha)*G2)/255;
			B = (alpha*B + (255-alpha)*B2)/255;
			nDstColor = 0xff000000 | (R << 16) | (G << 8) | B;
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
	pCanvas->UnlockCanvas();
#endif
}
//the end
/*void g_DrawSpriteAlpha_OLD(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	
	long nX = pNode->m_nX;// x coord
	long nY = pNode->m_nY;// y coord
	long nWidth = pNode->m_nWidth;// width of sprite
	long nHeight = pNode->m_nHeight;// height of sprite
	void* lpSprite = pNode->m_pBitmap;// sprite pointer
	void* lpPalette	= pNode->m_pPalette;// palette pointer
	void* lpBuffer = pCanvas->m_pCanvas;// canvas buffer
	long nPitch = pCanvas->m_nPitch;// canvas pitch
	long nNextLine = nPitch - nWidth * 2;// next line add
	long nAlpha = pNode->m_nAlpha;// alpha value
	long nMask32 = pCanvas->m_nMask32;// rgb mask32

	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (!pCanvas->MakeClip(nX, nY, nWidth, nHeight, &Clipper))
		return;
	//当前代码图形左右同时被裁减时有误
	if (Clipper.left && Clipper.right)
		return;
	
	// 绘制函数的汇编代码
	__asm
	{
//---------------------------------------------------------------------------
// 计算 EDI 指向屏幕起点的偏移量 (以字节计)
// edi = lpBuffer + nPitch * Clipper.y + nX * 2;
//---------------------------------------------------------------------------
		mov		eax, nPitch
		mov		ebx, Clipper.y
		mul		ebx
		mov     ebx, nX
		add		ebx, ebx
		add		eax, ebx
		mov		edi, lpBuffer
		add		edi, eax
//---------------------------------------------------------------------------
// 初始化 ESI 指向图块数据起点 
// (跳过Clipper.top行压缩图形数据)
//---------------------------------------------------------------------------
		mov		esi, lpSprite
		mov		ecx, Clipper.top
		or		ecx, ecx
		jz		loc_DrawSpriteAlpha_0011

loc_DrawSpriteAlpha_0008:

		mov		edx, nWidth

loc_DrawSpriteAlpha_0009:

//		movzx	eax, byte ptr[esi]
//		inc		esi
//		movzx	ebx, byte ptr[esi]
//		inc		esi
//		use uv, change to below
		xor		eax, eax
		xor		ebx, ebx
		mov		al,	 byte ptr[esi]
		inc		esi
		mov		bl,  byte ptr[esi]
		inc		esi
//		change	end
		or		ebx, ebx
		jnz		loc_DrawSpriteAlpha_0010
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0009
		dec     ecx
		jnz		loc_DrawSpriteAlpha_0008
		jmp		loc_DrawSpriteAlpha_0011

loc_DrawSpriteAlpha_0010:

		add		esi, eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0009
		dec     ecx
		jnz		loc_DrawSpriteAlpha_0008
//---------------------------------------------------------------------------
// 根据 Clipper.left, Clipper.right 分 4 种情况
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0011:

		mov		eax, Clipper.left
		or		eax, eax
		jnz		loc_DrawSpriteAlpha_0012
		mov		eax, Clipper.right
		or		eax, eax
		jnz		loc_DrawSpriteAlpha_0013
		jmp		loc_DrawSpriteAlpha_0100

loc_DrawSpriteAlpha_0012:

		mov		eax, Clipper.right
		or		eax, eax
		jnz		loc_DrawSpriteAlpha_0014
		jmp		loc_DrawSpriteAlpha_0200

loc_DrawSpriteAlpha_0013:

		jmp		loc_DrawSpriteAlpha_0300

loc_DrawSpriteAlpha_0014:

		jmp		loc_DrawSpriteAlpha_0400
//---------------------------------------------------------------------------
// 左边界裁剪量 == 0
// 右边界裁剪量 == 0
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0100:

		mov		edx, Clipper.width

loc_DrawSpriteAlpha_0101:

		movzx	eax, byte ptr[esi]
		inc		esi
		movzx	ebx, byte ptr[esi]
		inc		esi
		or		ebx, ebx
		jnz		loc_DrawSpriteAlpha_0102
		
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0101
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0100
		jmp		loc_DrawSpriteAlpha_exit

loc_DrawSpriteAlpha_0102:
		cmp		ebx, 255
		jl		loc_DrawSpriteAlpha_0110
		push	eax
		push	edx
		mov		ecx, eax
		mov     ebx, lpPalette

loc_DrawSpriteAlpha_0103:

		movzx	eax, byte ptr[esi]
		inc		esi
		mov		dx, [ebx + eax * 2]
		mov		[edi], dx
		inc		edi
		inc		edi
		dec		ecx
		jnz		loc_DrawSpriteAlpha_0103

		pop		edx
		pop		eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0101
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0100
		jmp		loc_DrawSpriteAlpha_exit

loc_DrawSpriteAlpha_0110:

		push	eax
		push	edx
		mov		ecx, eax
		shr     ebx, 3
		mov		nAlpha, ebx

loc_DrawSpriteAlpha_0111:

		push	ecx
		mov     ebx, lpPalette

		movzx	eax, byte ptr[esi]
		inc		esi
		mov     cx, [ebx + eax * 2]    // ecx = ...rgb
		mov		ax, cx                 // eax = ...rgb
		shl		eax, 16                // eax = rgb...
		mov		ax, cx                 // eax = rgbrgb
		and		eax, nMask32           // eax = .g.r.b
		mov		cx, [edi]              // ecx = ...rgb
		mov		bx, cx                 // ebx = ...rgb
		shl		ebx, 16                // ebx = rgb...
		mov		bx, cx                 // ebx = rgbrgb
		and		ebx, nMask32           // ebx = .g.r.b
		mov		ecx, nAlpha            // ecx = alpha
		mul		ecx                    // eax:edx = eax*ecx
		neg		ecx                    // ecx = -alpha
		add		ecx, 32                // ecx = 32 - alpha
		xchg	eax, ebx               // exchange eax,ebx
		mul		ecx                    // eax = eax * (32 - alpha)
		add		eax, ebx               // eax = eax + ebx
		shr		eax, 5                 // c = (c1 * alpha + c2 * (32 - alpha)) / 32
		and     eax, nMask32           // eax = .g.r.b
		mov     cx, ax                 // ecx = ...r.b
		shr     eax, 16                // eax = ....g.
		or      ax, cx                 // eax = ...rgb

		mov		[edi], ax
		inc		edi
		inc		edi
		pop		ecx
		dec		ecx
		jnz		loc_DrawSpriteAlpha_0111

		pop		edx
		pop		eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0101
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0100
		jmp		loc_DrawSpriteAlpha_exit

//---------------------------------------------------------------------------
// 左边界裁剪量 != 0
// 右边界裁剪量 == 0
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0200:

		mov		edx, Clipper.left

loc_DrawSpriteAlpha_0201:

		movzx	eax, byte ptr[esi]
		inc		esi
		movzx	ebx, byte ptr[esi]
		inc		esi
		or		ebx, ebx
		jnz		loc_DrawSpriteAlpha_0202
//---------------------------------------------------------------------------
// 处理nAlpha == 0 的像素 (左边界外)
//---------------------------------------------------------------------------
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0201
		jz		loc_DrawSpriteAlpha_0203
		neg		edx
		mov		eax, edx
		mov		edx, Clipper.width
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0204
		add		edi, nNextLine
		dec		Clipper.height
		jg		loc_DrawSpriteAlpha_0200
		jmp		loc_DrawSpriteAlpha_exit
//---------------------------------------------------------------------------
// 处理nAlpha != 0 的像素 (左边界外)
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0202:

		add		esi, eax
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0201
		jz		loc_DrawSpriteAlpha_0203
//---------------------------------------------------------------------------
// 把多减的宽度补回来
//---------------------------------------------------------------------------
		neg		edx
		sub		esi, edx
		sub		edi, edx
		sub		edi, edx

		cmp		ebx, 255
		jl		loc_DrawSpriteAlpha_0210

		push	eax
		push	edx
		mov		ecx, edx
		mov     ebx, lpPalette

loc_DrawSpriteAlpha_Loop20:

		movzx	eax, byte ptr[esi]
		inc		esi
		mov     dx, [ebx + eax * 2]
		mov		[edi], dx
		inc		edi
		inc		edi
		dec     ecx
		jg      loc_DrawSpriteAlpha_Loop20

		pop		edx
		pop		eax
		mov		ecx, edx
		mov		edx, Clipper.width
		sub		edx, ecx
		jg		loc_DrawSpriteAlpha_0204
		add		edi, nNextLine
		dec		Clipper.height
		jg		loc_DrawSpriteAlpha_0200
		jmp		loc_DrawSpriteAlpha_exit

loc_DrawSpriteAlpha_0210:

		push	eax
		push	edx
		mov		ecx, edx
		shr     ebx, 3
		mov		nAlpha, ebx

loc_DrawSpriteAlpha_0211:

		push	ecx
		mov     ebx, lpPalette

		movzx	eax, byte ptr[esi]
		inc		esi
		mov     cx, [ebx + eax * 2]    // ecx = ...rgb
		mov		ax, cx                 // eax = ...rgb
		shl		eax, 16                // eax = rgb...
		mov		ax, cx                 // eax = rgbrgb
		and		eax, nMask32           // eax = .g.r.b
		mov		cx, [edi]              // ecx = ...rgb
		mov		bx, cx                 // ebx = ...rgb
		shl		ebx, 16                // ebx = rgb...
		mov		bx, cx                 // ebx = rgbrgb
		and		ebx, nMask32           // ebx = .g.r.b
		mov		ecx, nAlpha            // ecx = alpha
		mul		ecx                    // eax:edx = eax*ecx
		neg		ecx                    // ecx = -alpha
		add		ecx, 32                // ecx = 32 - alpha
		xchg	eax, ebx               // exchange eax,ebx
		mul		ecx                    // eax = eax * (32 - alpha)
		add		eax, ebx               // eax = eax + ebx
		shr		eax, 5                 // c = (c1 * alpha + c2 * (32 - alpha)) / 32
		and     eax, nMask32           // eax = .g.r.b
		mov     cx, ax                 // ecx = ...r.b
		shr     eax, 16                // eax = ....g.
		or      ax, cx                 // eax = ...rgb

		mov		[edi], ax
		inc		edi
		inc		edi
		pop		ecx
		dec		ecx
		jnz		loc_DrawSpriteAlpha_0211

		pop		edx
		pop		eax
		mov		ecx, edx
		mov		edx, Clipper.width
		sub		edx, ecx
		jg		loc_DrawSpriteAlpha_0204
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0200
		jmp		loc_DrawSpriteAlpha_exit

//---------------------------------------------------------------------------
// 已处理完剪裁区 下面的处理相对简单
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0203:

		mov		edx, Clipper.width

loc_DrawSpriteAlpha_0204:

		movzx	eax, byte ptr[esi]
		inc		esi
		movzx	ebx, byte ptr[esi]
		inc		esi
		or		ebx, ebx
		jnz		loc_DrawSpriteAlpha_0206
//---------------------------------------------------------------------------
// 处理nAlpha == 0的像素 (左边界内)
//---------------------------------------------------------------------------
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0204
		add		edi, nNextLine
		dec		Clipper.height
		jg		loc_DrawSpriteAlpha_0200
		jmp		loc_DrawSpriteAlpha_exit
//---------------------------------------------------------------------------
// 处理nAlpha != 0的像素 (左边界内)
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0206:

		cmp		ebx, 255
		jl		loc_DrawSpriteAlpha_0220

		push	eax
		push	edx
		mov		ecx, eax
		mov     ebx, lpPalette

loc_DrawSpriteAlpha_Loop21:

		movzx	eax, byte ptr[esi]
		inc		esi
		mov     dx, [ebx + eax * 2]
		mov		[edi], dx
		inc		edi
		inc		edi
		dec     ecx
		jg		loc_DrawSpriteAlpha_Loop21

		pop		edx
		pop		eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0204
		add		edi, nNextLine
		dec		Clipper.height
		jg		loc_DrawSpriteAlpha_0200
		jmp		loc_DrawSpriteAlpha_exit

loc_DrawSpriteAlpha_0220:

		push	eax
		push	edx
		mov		ecx, eax
		shr     ebx, 3
		mov		nAlpha, ebx

loc_DrawSpriteAlpha_0221:

		push	ecx
		mov     ebx, lpPalette

		movzx	eax, byte ptr[esi]
		inc		esi
		mov     cx, [ebx + eax * 2]    // ecx = ...rgb
		mov		ax, cx                 // eax = ...rgb
		shl		eax, 16                // eax = rgb...
		mov		ax, cx                 // eax = rgbrgb
		and		eax, nMask32           // eax = .g.r.b
		mov		cx, [edi]              // ecx = ...rgb
		mov		bx, cx                 // ebx = ...rgb
		shl		ebx, 16                // ebx = rgb...
		mov		bx, cx                 // ebx = rgbrgb
		and		ebx, nMask32           // ebx = .g.r.b
		mov		ecx, nAlpha            // ecx = alpha
		mul		ecx                    // eax:edx = eax*ecx
		neg		ecx                    // ecx = -alpha
		add		ecx, 32                // ecx = 32 - alpha
		xchg	eax, ebx               // exchange eax,ebx
		mul		ecx                    // eax = eax * (32 - alpha)
		add		eax, ebx               // eax = eax + ebx
		shr		eax, 5                 // c = (c1 * alpha + c2 * (32 - alpha)) / 32
		and     eax, nMask32           // eax = .g.r.b
		mov     cx, ax                 // ecx = ...r.b
		shr     eax, 16                // eax = ....g.
		or      ax, cx                 // eax = ...rgb

		mov		[edi], ax
		inc		edi
		inc		edi
		pop		ecx
		dec		ecx
		jnz		loc_DrawSpriteAlpha_0221

		pop		edx
		pop		eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0204
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0200
		jmp		loc_DrawSpriteAlpha_exit

//---------------------------------------------------------------------------
// 左边界裁剪量 == 0
// 右边界裁剪量 != 0
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0300:

		mov		edx, Clipper.width

loc_DrawSpriteAlpha_0301:

		movzx	eax, byte ptr[esi]
		inc		esi
		movzx	ebx, byte ptr[esi]
		inc		esi
		or		ebx, ebx
		jnz		loc_DrawSpriteAlpha_0303
//---------------------------------------------------------------------------
// 处理 nAlpha == 0 的像素 (右边界内)
//---------------------------------------------------------------------------
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0301
		neg		edx
		jmp		loc_DrawSpriteAlpha_0305
//---------------------------------------------------------------------------
// 处理 nAlpha != 0 的像素 (右边界内)
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0303:

		cmp		edx, eax
		jl		loc_DrawSpriteAlpha_0304

		cmp		ebx, 255
		jl		loc_DrawSpriteAlpha_0310
		
		push	eax
		push	edx
		mov		ecx, eax
		mov     ebx, lpPalette

loc_DrawSpriteAlpha_Loop30:

		movzx	eax, byte ptr[esi]
		inc		esi
		mov     dx, [ebx + eax * 2]
		mov		[edi], dx
		inc		edi
		inc		edi
		dec     ecx
		jg      loc_DrawSpriteAlpha_Loop30

		pop		edx
		pop		eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0301
		neg		edx
		jmp		loc_DrawSpriteAlpha_0305

loc_DrawSpriteAlpha_0310:

		push	eax
		push	edx
		mov		ecx, eax
		shr     ebx, 3
		mov		nAlpha, ebx

loc_DrawSpriteAlpha_0311:

		push	ecx
		mov     ebx, lpPalette

		movzx	eax, byte ptr[esi]
		inc		esi
		mov     cx, [ebx + eax * 2]    // ecx = ...rgb
		mov		ax, cx                 // eax = ...rgb
		shl		eax, 16                // eax = rgb...
		mov		ax, cx                 // eax = rgbrgb
		and		eax, nMask32           // eax = .g.r.b
		mov		cx, [edi]              // ecx = ...rgb
		mov		bx, cx                 // ebx = ...rgb
		shl		ebx, 16                // ebx = rgb...
		mov		bx, cx                 // ebx = rgbrgb
		and		ebx, nMask32           // ebx = .g.r.b
		mov		ecx, nAlpha            // ecx = alpha
		mul		ecx                    // eax:edx = eax*ecx
		neg		ecx                    // ecx = -alpha
		add		ecx, 32                // ecx = 32 - alpha
		xchg	eax, ebx               // exchange eax,ebx
		mul		ecx                    // eax = eax * (32 - alpha)
		add		eax, ebx               // eax = eax + ebx
		shr		eax, 5                 // c = (c1 * alpha + c2 * (32 - alpha)) / 32
		and     eax, nMask32           // eax = .g.r.b
		mov     cx, ax                 // ecx = ...r.b
		shr     eax, 16                // eax = ....g.
		or      ax, cx                 // eax = ...rgb

		mov		[edi], ax
		inc		edi
		inc		edi
		pop		ecx
		dec		ecx
		jnz		loc_DrawSpriteAlpha_0311

		pop		edx
		pop		eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0301
		neg		edx
		jmp		loc_DrawSpriteAlpha_0305

//---------------------------------------------------------------------------
// 连续点的个数 (eax) > 裁减后的宽度 (edx)
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0304:

		cmp		ebx, 255
		jl		loc_DrawSpriteAlpha_0320

		push	eax
		push	edx
		mov		ecx, edx
		mov     ebx, lpPalette

loc_DrawSpriteAlpha_Loop31:

		movzx	eax, byte ptr[esi]
		inc		esi
		mov     dx, [ebx + eax * 2]
		mov		[edi], dx
		inc		edi
		inc		edi
		dec     ecx
		jg      loc_DrawSpriteAlpha_Loop31

		pop		edx
		pop		eax
		sub		eax, edx
		mov		edx, eax
		add		esi, eax
		add		edi, eax
		add		edi, eax
		jmp		loc_DrawSpriteAlpha_0305

loc_DrawSpriteAlpha_0320:

		push	eax
		push	edx
		mov		ecx, edx
		shr     ebx, 3
		mov		nAlpha, ebx

loc_DrawSpriteAlpha_0321:

		push	ecx
		mov     ebx, lpPalette

		movzx	eax, byte ptr[esi]
		inc		esi
		mov     cx, [ebx + eax * 2]    // ecx = ...rgb
		mov		ax, cx                 // eax = ...rgb
		shl		eax, 16                // eax = rgb...
		mov		ax, cx                 // eax = rgbrgb
		and		eax, nMask32           // eax = .g.r.b
		mov		cx, [edi]              // ecx = ...rgb
		mov		bx, cx                 // ebx = ...rgb
		shl		ebx, 16                // ebx = rgb...
		mov		bx, cx                 // ebx = rgbrgb
		and		ebx, nMask32           // ebx = .g.r.b
		mov		ecx, nAlpha            // ecx = alpha
		mul		ecx                    // eax:edx = eax*ecx
		neg		ecx                    // ecx = -alpha
		add		ecx, 32                // ecx = 32 - alpha
		xchg	eax, ebx               // exchange eax,ebx
		mul		ecx                    // eax = eax * (32 - alpha)
		add		eax, ebx               // eax = eax + ebx
		shr		eax, 5                 // c = (c1 * alpha + c2 * (32 - alpha)) / 32
		and     eax, nMask32           // eax = .g.r.b
		mov     cx, ax                 // ecx = ...r.b
		shr     eax, 16                // eax = ....g.
		or      ax, cx                 // eax = ...rgb

		mov		[edi], ax
		inc		edi
		inc		edi
		pop		ecx
		dec		ecx
		jnz		loc_DrawSpriteAlpha_0321

		pop		edx
		pop		eax
		sub		eax, edx
		mov		edx, eax
		add		esi, eax
		add		edi, eax
		add		edi, eax
		jmp		loc_DrawSpriteAlpha_0305

//---------------------------------------------------------------------------
// 处理超过了右边界的部分, edx = 超过右边界部分的长度
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0305:

		mov		eax, edx
		mov		edx, Clipper.right
		sub		edx, eax
		jle		loc_DrawSpriteAlpha_0308

loc_DrawSpriteAlpha_0306:

		movzx	eax, byte ptr[esi]
		inc		esi
		movzx	ebx, byte ptr[esi]
		inc		esi
		or		ebx, ebx
		jnz		loc_DrawSpriteAlpha_0307
//---------------------------------------------------------------------------
// 处理 nAlpha == 0 的像素 (右边界外)
//---------------------------------------------------------------------------
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0306
		jmp		loc_DrawSpriteAlpha_0308
//---------------------------------------------------------------------------
// 处理 nAlpha != 0 的像素 (右边界外)
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0307:

		add		esi, eax
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0306

loc_DrawSpriteAlpha_0308:

		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0300
		jmp		loc_DrawSpriteAlpha_exit

//---------------------------------------------------------------------------
// 左边界裁剪量 != 0
// 右边界裁剪量 != 0
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0400:		// Line Begin

		mov		edx, Clipper.left

loc_Draw_GetLength:						// edx 记录该行压缩数据到裁剪左端的长度，可能是负值

		movzx	eax, byte ptr[esi]		// 取出压缩数据的长度
		inc		esi
		movzx	ebx, byte ptr[esi]		// 取出Alpha值
		inc		esi
		cmp		edx, eax
		jge		loc_Draw_AllLeft		// edx >= eax, 所有数据都在左边界外
		mov		ecx, Clipper.width		// ecx 得到Clipper宽度
		add		ecx, edx				// ecx = Clipper宽度 + 压缩数据左端被剪长度
		cmp		ecx, 0
		jle		loc_Draw_AllRight		// ecx <= 0，所有数据都在右边界外
		sub		ecx, eax				// 比较压缩数据长度和 ecx ，ecx小于0的话，ecx值为该段右端裁剪长度
		jge		loc_Draw_GetLength_0	// ecx >= eax 说明右端无裁剪
		cmp		edx, 0
		jl		loc_Draw_RightClip		// 左端有裁剪，右端也有
		jmp		loc_Draw_AllClip
loc_Draw_GetLength_0:
		cmp		edx, 0
		jl		loc_Draw_NoClip			// 左右都没裁剪
		jmp		loc_Draw_LeftClip
//---------------------------------------------------------------------------
// 全在左端外
//---------------------------------------------------------------------------
loc_Draw_AllLeft:
		or		ebx, ebx
		jnz		loc_Draw_AllLeft_1
//loc_Draw_AllLeft_0:	// alpha == 0
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jmp		loc_Draw_GetLength
loc_Draw_AllLeft_1: // alpha != 0
		add		edi, eax
		add		edi, eax
		add		esi, eax
		sub		edx, eax
		jmp		loc_Draw_GetLength
//---------------------------------------------------------------------------
// 全在右端外
//---------------------------------------------------------------------------
loc_Draw_AllRight:
		or		ebx, ebx
		jnz		loc_Draw_AllRight_1
//loc_Draw_AllRight_0:
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		mov		ebx, edx
		add		ebx, Clipper.width
		add		ebx, Clipper.right
		cmp		ebx, 0
		jl		loc_Draw_GetLength
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0400	// 行结束，下一行开始
		jmp		loc_DrawSpriteAlpha_exit		
loc_Draw_AllRight_1:
		add		edi, eax
		add		edi, eax
		add		esi, eax
		sub		edx, eax
		mov		ebx, edx
		add		ebx, Clipper.width
		add		ebx, Clipper.right
		cmp		ebx, 0
		jl		loc_Draw_GetLength
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0400	// 行结束，下一行开始
		jmp		loc_DrawSpriteAlpha_exit
//---------------------------------------------------------------------------
// 处理左右端都不用裁剪的压缩段
//---------------------------------------------------------------------------
loc_Draw_NoClip:
		or		ebx, ebx
		jnz		loc_Draw_NoClip_1
//loc_Draw_NoClip_0:
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jmp		loc_Draw_GetLength
loc_Draw_NoClip_1:
		cmp		ebx, 255
		jl		loc_Draw_NoClip_Alpha
		push	eax
		push	edx
		mov		ecx, eax
		mov		ebx, lpPalette

loc_Draw_NoClip_Copy:
		movzx	eax, byte ptr[esi]
		inc		esi
		mov		dx, [ebx + eax * 2]
		mov		[edi], dx
		inc		edi
		inc		edi
		dec		ecx
		jnz		loc_Draw_NoClip_Copy
		
		pop		edx
		pop		eax
		sub		edx, eax
		jmp		loc_Draw_GetLength

loc_Draw_NoClip_Alpha:
		push	eax
		push	edx
		mov		ecx, eax
		shr     ebx, 3
		mov		nAlpha, ebx
			
loc_Draw_NoClip_Alpha_LOOP:
		
		push	ecx
		mov     ebx, lpPalette
		
		movzx	eax, byte ptr[esi]
		inc		esi
		mov     cx, [ebx + eax * 2]    // ecx = ...rgb
		mov		ax, cx                 // eax = ...rgb
		shl		eax, 16                // eax = rgb...
		mov		ax, cx                 // eax = rgbrgb
		and		eax, nMask32           // eax = .g.r.b
		mov		cx, [edi]              // ecx = ...rgb
		mov		bx, cx                 // ebx = ...rgb
		shl		ebx, 16                // ebx = rgb...
		mov		bx, cx                 // ebx = rgbrgb
		and		ebx, nMask32           // ebx = .g.r.b
		mov		ecx, nAlpha            // ecx = alpha
		mul		ecx                    // eax:edx = eax*ecx
		neg		ecx                    // ecx = -alpha
		add		ecx, 32                // ecx = 32 - alpha
		xchg	eax, ebx               // exchange eax,ebx
		mul		ecx                    // eax = eax * (32 - alpha)
		add		eax, ebx               // eax = eax + ebx
		shr		eax, 5                 // c = (c1 * alpha + c2 * (32 - alpha)) / 32
		and     eax, nMask32           // eax = .g.r.b
		mov     cx, ax                 // ecx = ...r.b
		shr     eax, 16                // eax = ....g.
		or      ax, cx                 // eax = ...rgb
		
		mov		[edi], ax
		inc		edi
		inc		edi
		pop		ecx
		dec		ecx
		jnz		loc_Draw_NoClip_Alpha_LOOP
		
		pop		edx
		pop		eax
		sub		edx, eax
		jmp		loc_Draw_GetLength
//---------------------------------------------------------------------------
// 处理左右端同时要裁剪的压缩段
//---------------------------------------------------------------------------
loc_Draw_AllClip:
		or		ebx, ebx				// 设置标志位
		jnz		loc_Draw_AllClip_1		// Alpha值不为零的处理
//loc_Draw_AllClip_0:
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		neg		ecx
		cmp		ecx, Clipper.right
		jl		loc_Draw_GetLength		// Spr该行没完，接着处理
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0400// 行结束，下一行开始
		jmp		loc_DrawSpriteAlpha_exit
loc_Draw_AllClip_1:
		add		edi, eax
		add		edi, eax
		add		esi, eax
		sub		edx, eax				// edx - eax < 0

		add		edi, edx				// 补回前面多减的部分
		add		edi, edx				// edi和esi指向实际要
		add		esi, edx				// 绘制的部分
		
		cmp		ebx, 255
		jl		loc_Draw_AllClip_Alpha
		push	eax
		push	edx
		push	ecx
		mov		ecx, Clipper.width		// 前后都被裁剪，所以绘制长度为Clipper.width
		mov		ebx, lpPalette
		
loc_Draw_AllClip_Copy:
		movzx	eax, byte ptr[esi]
		inc		esi
		mov     dx, [ebx + eax * 2]
		mov		[edi], dx
		inc		edi
		inc		edi
		dec     ecx
		jnz		loc_Draw_AllClip_Copy
		
		pop		ecx
		pop		edx
		pop		eax
		jmp		loc_Draw_AllClip_End

loc_Draw_AllClip_Alpha:
		push	eax
		push	edx
		push	ecx
		mov		ecx, Clipper.width
		shr     ebx, 3
		mov		nAlpha, ebx
			
loc_Draw_AllClip_Alpha_LOOP:
		
		push	ecx
		mov     ebx, lpPalette
		
		movzx	eax, byte ptr[esi]
		inc		esi
		mov     cx, [ebx + eax * 2]    // ecx = ...rgb
		mov		ax, cx                 // eax = ...rgb
		shl		eax, 16                // eax = rgb...
		mov		ax, cx                 // eax = rgbrgb
		and		eax, nMask32           // eax = .g.r.b
		mov		cx, [edi]              // ecx = ...rgb
		mov		bx, cx                 // ebx = ...rgb
		shl		ebx, 16                // ebx = rgb...
		mov		bx, cx                 // ebx = rgbrgb
		and		ebx, nMask32           // ebx = .g.r.b
		mov		ecx, nAlpha            // ecx = alpha
		mul		ecx                    // eax:edx = eax*ecx
		neg		ecx                    // ecx = -alpha
		add		ecx, 32                // ecx = 32 - alpha
		xchg	eax, ebx               // exchange eax,ebx
		mul		ecx                    // eax = eax * (32 - alpha)
		add		eax, ebx               // eax = eax + ebx
		shr		eax, 5                 // c = (c1 * alpha + c2 * (32 - alpha)) / 32
		and     eax, nMask32           // eax = .g.r.b
		mov     cx, ax                 // ecx = ...r.b
		shr     eax, 16                // eax = ....g.
		or      ax, cx                 // eax = ...rgb
		
		mov		[edi], ax
		inc		edi
		inc		edi
		pop		ecx
		dec		ecx
		jnz		loc_Draw_AllClip_Alpha_LOOP
		
		pop		ecx
		pop		edx
		pop		eax
loc_Draw_AllClip_End:
		neg		ecx
		add		edi, ecx				// 把edi、esi指针指向下一段
		add		edi, ecx
		add		esi, ecx
		cmp		ecx, Clipper.right
		jl		loc_Draw_GetLength		// Spr该行没完，接着处理
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0400// 行结束，下一行开始
		jmp		loc_DrawSpriteAlpha_exit
//---------------------------------------------------------------------------
// 处理只有左端要裁剪的压缩段
//---------------------------------------------------------------------------
loc_Draw_LeftClip:
		or		ebx, ebx
		jnz		loc_Draw_LeftClip_1

//loc_Draw_LeftClip_0:
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jmp		loc_Draw_GetLength
loc_Draw_LeftClip_1:
		add		edi, eax
		add		edi, eax
		add		esi, eax
		sub		edx, eax
		add		edi, edx
		add		edi, edx
		add		esi, edx

		cmp		ebx, 255
		jl		loc_Draw_LeftClip_Alpha
		push	eax
		push	edx
		mov		ecx, edx
		neg		ecx
		mov     ebx, lpPalette
		
loc_Draw_LeftClip_Copy:
		
		movzx	eax, byte ptr[esi]
		inc		esi
		mov     dx, [ebx + eax * 2]
		mov		[edi], dx
		inc		edi
		inc		edi
		dec     ecx
		jg      loc_Draw_LeftClip_Copy
		
		pop		edx
		pop		eax
		jmp		loc_Draw_GetLength

loc_Draw_LeftClip_Alpha:
		push	eax
		push	edx
		mov		ecx, edx
		neg		ecx
		shr     ebx, 3
		mov		nAlpha, ebx
		
loc_Draw_LeftClip_Alpha_LOOP:
		
		push	ecx
		mov     ebx, lpPalette
		
		movzx	eax, byte ptr[esi]
		inc		esi
		mov     cx, [ebx + eax * 2]    // ecx = ...rgb
		mov		ax, cx                 // eax = ...rgb
		shl		eax, 16                // eax = rgb...
		mov		ax, cx                 // eax = rgbrgb
		and		eax, nMask32           // eax = .g.r.b
		mov		cx, [edi]              // ecx = ...rgb
		mov		bx, cx                 // ebx = ...rgb
		shl		ebx, 16                // ebx = rgb...
		mov		bx, cx                 // ebx = rgbrgb
		and		ebx, nMask32           // ebx = .g.r.b
		mov		ecx, nAlpha            // ecx = alpha
		mul		ecx                    // eax:edx = eax*ecx
		neg		ecx                    // ecx = -alpha
		add		ecx, 32                // ecx = 32 - alpha
		xchg	eax, ebx               // exchange eax,ebx
		mul		ecx                    // eax = eax * (32 - alpha)
		add		eax, ebx               // eax = eax + ebx
		shr		eax, 5                 // c = (c1 * alpha + c2 * (32 - alpha)) / 32
		and     eax, nMask32           // eax = .g.r.b
		mov     cx, ax                 // ecx = ...r.b
		shr     eax, 16                // eax = ....g.
		or      ax, cx                 // eax = ...rgb
		
		mov		[edi], ax
		inc		edi
		inc		edi
		pop		ecx
		dec		ecx
		jnz		loc_Draw_LeftClip_Alpha_LOOP
		
		pop		edx
		pop		eax
		jmp		loc_Draw_GetLength
//---------------------------------------------------------------------------
// 处理只有右端要裁剪的压缩段
//---------------------------------------------------------------------------
loc_Draw_RightClip:
		or		ebx, ebx
		jnz		loc_Draw_RightClip_1

//loc_Draw_RightClip_0:
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		neg		ecx
		cmp		ecx, Clipper.right
		jl		loc_Draw_GetLength
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0400	// 行结束，下一行开始
		jmp		loc_DrawSpriteAlpha_exit

loc_Draw_RightClip_1:
		sub		edx, eax
		cmp		ebx, 255
		jl		loc_Draw_RightClip_Alpha
		push	eax
		push	edx
		push	ecx
		add		ecx, eax					// 得到实际绘制的长度
		mov		ebx, lpPalette
		
loc_Draw_RightClip_Copy:
		movzx	eax, byte ptr[esi]
		inc		esi
		mov		dx, [ebx + eax * 2]
		mov		[edi], dx
		inc		edi
		inc		edi
		dec		ecx
		jnz		loc_Draw_RightClip_Copy
		
		pop		ecx
		pop		edx
		pop		eax
		jmp		loc_Draw_RightClip_End

loc_Draw_RightClip_Alpha:
		add		edi, eax
		add		edi, eax
		add		esi, eax
		jmp		loc_Draw_RightClip_End
		push	eax
		push	edx
		push	ecx
		add		ecx, eax
		shr     ebx, 3
		mov		nAlpha, ebx
			
loc_Draw_RightClip_Alpha_LOOP:
		
		push	ecx
		mov     ebx, lpPalette
		
		movzx	eax, byte ptr[esi]
		inc		esi
		mov     cx, [ebx + eax * 2]    // ecx = ...rgb
		mov		ax, cx                 // eax = ...rgb
		shl		eax, 16                // eax = rgb...
		mov		ax, cx                 // eax = rgbrgb
		and		eax, nMask32           // eax = .g.r.b
		mov		cx, [edi]              // ecx = ...rgb
		mov		bx, cx                 // ebx = ...rgb
		shl		ebx, 16                // ebx = rgb...
		mov		bx, cx                 // ebx = rgbrgb
		and		ebx, nMask32           // ebx = .g.r.b
		mov		ecx, nAlpha            // ecx = alpha
		mul		ecx                    // eax:edx = eax*ecx
		neg		ecx                    // ecx = -alpha
		add		ecx, 32                // ecx = 32 - alpha
		xchg	eax, ebx               // exchange eax,ebx
		mul		ecx                    // eax = eax * (32 - alpha)
		add		eax, ebx               // eax = eax + ebx
		shr		eax, 5                 // c = (c1 * alpha + c2 * (32 - alpha)) / 32
		and     eax, nMask32           // eax = .g.r.b
		mov     cx, ax                 // ecx = ...r.b
		shr     eax, 16                // eax = ....g.
		or      ax, cx                 // eax = ...rgb
		
		mov		[edi], ax
		inc		edi
		inc		edi
		pop		ecx
		dec		ecx
		jnz		loc_Draw_RightClip_Alpha_LOOP

		pop		ecx
		pop		edx
		pop		eax
		
loc_Draw_RightClip_End:
		neg		ecx
		add		edi, ecx				// 把edi、esi指针指向下一段
		add		edi, ecx
		add		esi, ecx
		cmp		ecx, Clipper.right
		jl		loc_Draw_GetLength		// Spr该行没完，接着处理
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0400// 行结束，下一行开始
		jmp		loc_DrawSpriteAlpha_exit
			
loc_DrawSpriteAlpha_exit:
	}
}*/

inline void _Jmp(INT n)
{
	while( g_nIndex + n >= *g_pSection )
	{
		BYTE length = *g_pSection++;
		if( *g_pSection++ )
			g_pSection += length;
		n -= (length - g_nIndex);
		g_nIndex = 0;
	}
	g_nIndex += n;
}

inline BYTE _Alpha()
{
	return g_pSection[1];
}

inline WORD _Color()	//ignore if _alpha() == 0
{
	return g_pPal[g_pSection[g_nIndex+2]];
}
//draw primitives
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
						 const RECT* pSrcRect )
{
	RECT sSrcRect;
	if( pSrcRect )
	{
		sSrcRect = *pSrcRect;
	}
	else
	{
		SetRect(&sSrcRect, 0, 0, nWidth, nHeight);
	}

	RECT rc1 = {0, 0, width, height};
	RECT rc2 = {nX, nY, nX + sSrcRect.right - sSrcRect.left, nY + sSrcRect.bottom - sSrcRect.top};

	RECT sDestRect;
	if( !RectIntersect( &rc1, &rc2, &sDestRect ) )
		return;

	INT nClipX = sDestRect.left - nX + sSrcRect.left;
	INT nClipY = sDestRect.top - nY + sSrcRect.top;
	INT nClipWidth = sDestRect.right - sDestRect.left;
	INT nClipHeight = sDestRect.bottom - sDestRect.top;

	nX = sDestRect.left;
	nY = sDestRect.top;

	INT nSprSkip = nWidth * nClipY + nClipX;
	INT nSprSkipPerLine = nWidth - nClipWidth;
	INT nClipRight = nWidth - nClipX - nClipWidth;
	INT nBufSkip = nPitch * nY + nX * 2;
	INT nBufSkipPerLine = nPitch - nClipWidth * 2;
	BOOL bRGB565 = (dwMask32 != 0x03e07c1f);
	WORD* pDst = (WORD*)pBuffer;
	g_pPal = (WORD*)pPalette;
	g_pSection = (BYTE*)pSprite;
	g_nIndex = 0;

	//jmp
	_Jmp(nSprSkip);
	pDst = (WORD*)((BYTE*)pDst + nBufSkip);


	INT nCmpValue = nClipWidth - 1;
	unsigned short src_color[4] = { 0 };
	unsigned short src_alpha[4] = { 0 };

	for( nY = 0; nY < nClipHeight; nY++ )
	{
		INT x = nClipWidth;
		while( x - 4 >= 0 )
		{
			if( _Alpha( ) )
			{
				src_color[ 0 ] = _Color( );
				src_alpha[ 0 ] = byInputAlpha + 1 ;
			}
			else
			{
				src_color[ 0 ] = 0;
				src_alpha[ 0 ] = 0;
			}
			_Jmp( 1 );


			if( _Alpha( ) )
			{
				src_color[ 1 ] = _Color( );
				src_alpha[ 1 ] = byInputAlpha + 1 ;
			}
			else
			{
				src_color[ 1 ] = 0;
				src_alpha[ 1 ] = 0;
			}
			_Jmp( 1 );

			if( _Alpha( ) )
			{
				src_color[ 2 ] = _Color( );
				src_alpha[ 2 ] = byInputAlpha + 1 ;
			}
			else
			{
				src_color[ 2 ] = 0;
				src_alpha[ 2 ] = 0;
			}
			_Jmp( 1 );

			if( _Alpha( ) )
			{
				src_color[ 3 ] = _Color( );
				src_alpha[ 3 ] = byInputAlpha + 1 ;
			}
			else
			{
				src_color[ 3 ] = 0;
				src_alpha[ 3 ] = 0;
			}
			if( x   - 4 > 0 )
			{
				_Jmp( 1 );
			}
			x -= 4;
			ScreenBlend4Pixel( pDst, src_color, src_alpha );
			pDst += 4;
		}
		if( x - 2 >= 0 )
		{
			if( _Alpha( ) )
			{
				src_color[ 0 ] = _Color( );
				src_alpha[ 0 ] = byInputAlpha;
			}
			else
			{
				src_color[ 0 ] = 0;
				src_alpha[ 0 ] = 0;
			}


			_Jmp( 1 );

			if( _Alpha( ) )
			{
				src_color[ 1 ] = _Color( );
				src_alpha[ 1 ] = byInputAlpha;
			}
			else
			{
				src_color[ 1 ] = 0;
				src_alpha[ 1 ] = 0;

			}
			if( x - 2  > 0)
				_Jmp( 1 );

			//			ScreenBlend2Pixel( dst, src_color, src_alpha );
			x -= 2;
			pDst += 2;
		}
		if( x - 1 >= 0 )
		{
			if( _Alpha( ) )
			{
				src_color[ 0 ] = _Color( );
				src_alpha[ 0 ] = byInputAlpha;
			}
			//			ScreenBlend1Pixel( dst, src_color, src_alpha );
			++pDst;
			x -= 1;
		}
		if( nY < nClipHeight - 1 )
		{
			//move to next line
			_Jmp( nSprSkipPerLine + 1 );
			pDst = (WORD*)((BYTE*)pDst + nBufSkipPerLine);
		}
	}
}

BOOL RectIntersect(const RECT* rc1, const RECT* rc2, RECT* out)
{
	out->left = max( rc1->left, rc2->left );
	out->right = min( rc1->right, rc2->right );
	out->top = max( rc1->top, rc2->top );
	out->bottom = min( rc1->bottom, rc2->bottom );
	return (out->right > out->left) && (out->bottom > out->top);
}

void ScreenBlend4Pixel( 
					   void*  pDest ,
					   void*  pColor,
					   void*  pAlpha )
{
#ifdef _WIN64
#else
	__asm
	{
		mov eax ,dword ptr pDest
			movq mm0, [eax]    //mm0 脛驴卤锚脩脮脡芦
		mov ebx ,dword ptr pColor //mm1 卤拢麓忙脭麓脩脮脡芦
			movq mm1,[ebx]
		mov ebx, dword ptr pAlpha
			movq mm7 , [ebx]   //mm7
		movq mm6, g_n64ColorMask

			/////r//////////
			movq mm2,mm0
			psrlw mm2, 11
			movq mm3,mm1
			psrlw mm3,11
			pmullw mm3,mm7
			psrlw mm3,8
			movq mm4, mm3
			paddw mm3, mm2
			pmullw mm4,mm2
			paddw  mm4,mm6 
			psrlw mm4,5
			psubw mm3,mm4   //mm3 R
			/////////////////////////

			///G /////////
			movq  mm2,mm0
			psllw mm2,5
			psrlw mm2,11
			movq  mm4,mm1
			psllw mm4,5
			psrlw mm4,11
			pmullw mm4, mm7
			psrlw mm4,8
			movq mm5,mm4
			paddw mm4,mm2
			pmullw mm5,mm2
			paddw  mm5,mm6 
			psrlw mm5,5
			psubw mm4,mm5

			//////b///////
			pand mm0, mm6
			pand mm1,mm6
			pmullw mm1,mm7
			psrlw mm1,8
			movq mm5,mm1
			paddw mm1,mm0
			pmullw mm5,mm0
			paddw  mm5,mm6 
			psrlw mm5,5
			psubw mm1,mm5 
			// mm3 r,mm4 g,mm1 b

			psllw mm3,11
			psllw mm4,6
			por  mm1,mm3
			por  mm1,mm4
			movq [eax] ,mm1
			emms

	}
#endif
}

void g_DrawOpaRecImage32b(void* node, void* canvas)
{
#ifndef _WIN64
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	int nExAlpha = pNode->m_nAlpha;
	if (nExAlpha <= 0)
		return;
	// t1o khung vu?ng trong ph1m vi khung game
	KClipper Clipper;
	if (pCanvas->MakeClip(pNode->m_nX, pNode->m_nY, pNode->m_nWidth, pNode->m_nHeight, &Clipper) == 0)
		return;

	// pBuffer lμ khung v? thμnh phèm
	int nPitch;
	BYTE* pBuffer = (BYTE*)pCanvas->LockCanvas(nPitch);
	if (pBuffer == NULL)
		return;
	pBuffer += Clipper.y * nPitch + Clipper.x*4;
	BYTE* pSprite = (BYTE*)pNode->m_pBitmap;	// sprite pointer
	int nBuffNextLine = nPitch - Clipper.width * 4;// next line add
	pSprite += (pNode->m_nWidth * Clipper.top + Clipper.left)*4;
	int nSprSkipPerLine = Clipper.left + Clipper.right;
	int nWidth;
	_DrawFullLineSection_Line_:
		nWidth = Clipper.width;
		memcpy(pBuffer, pSprite, nWidth*4);
		pSprite += nWidth*4;
        pBuffer += nWidth*4;
		pBuffer += nBuffNextLine;
		pSprite += nSprSkipPerLine*4;
		Clipper.height--;
		if(Clipper.height)
			goto	_DrawFullLineSection_Line_;
	pCanvas->UnlockCanvas();
#endif
}

void g_DrawAlphaRecImageOpa(void* node, void* canvas)
{
#ifndef _WIN64
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	// t1o khung vu?ng trong ph1m vi khung game
	KClipper Clipper;
	if (pCanvas->MakeClip(pNode->m_nX, pNode->m_nY, pNode->m_nWidth, pNode->m_nHeight, &Clipper) == 0)
		return;
	// pBuffer lμ khung v? thμnh phèm
	int nPitch;
	void* pBuffer = pCanvas->LockCanvas(nPitch);
	if (pBuffer == NULL)
		return;
	pBuffer = (char*)(pBuffer) + Clipper.y * nPitch + Clipper.x*2;
	int nMask32 = pCanvas->m_nMask32;	// rgb mask32
	void* pSprite = pNode->m_pBitmap;	// sprite pointer
	int nBuffNextLine = nPitch - Clipper.width * 2;// next line add
	int nSprSkip = pNode->m_nWidth * Clipper.top + Clipper.left;
	int nSprSkipPerLine = Clipper.left + Clipper.right;
	__asm
	{
        mov     eax, Clipper.width
        movd    mm1, eax        // mm1: Clipper.width

        mov     eax, nMask32
        movd    mm2, eax        // mm2: nMask32

        // mm3: nAlpha
        // mm4: temp use
        // mm7: push ecx, pop ecx
        // mm6: push edx, pop edx
        // mm5: push eax, pop eax

		//?i ??n pBuffer + Clipper.y * nPitch + Clipper.x*2
		mov		edi, pBuffer

		//d÷ li?u sprite
		mov		esi, pSprite

		//_SkipSpriteAheadContent_:
		{
			mov		edx, nSprSkip
			or		edx, edx
			jz		_SkipSpriteAheadContentEnd_
			imul	edx, 4
			add		esi, edx
		}
		_SkipSpriteAheadContentEnd_:
		mov		eax, nSprSkipPerLine
		or		eax, eax
		jnz		_DrawPartLineSection_

		//_DrawFullLineSection_:
		{
			//v? full spr kh?ng èn ph?n nμo			
			_DrawFullLineSection_Line_:
			{
				movd	edx, mm1    // mm1: Clipper.width
				_DrawFullLineSection_LineLocal_:
				{
					mov		ebx, 255	//alpha
					mov		eax, [esi]				//fullcolor
					add		esi, 4
					or		ebx, ebx
					jnz		_DrawFullLineSection_LineLocal_Alpha_
                    add     edi, 2
					dec		edx
					jg		_DrawFullLineSection_LineLocal_

					add		edi, nBuffNextLine
					dec		Clipper.height
					jnz		_DrawFullLineSection_Line_
					jmp		_EXIT_WAY_
				
					_DrawFullLineSection_LineLocal_Alpha_:
					{
						mov		ecx, eax
						dec		edx
						cmp		ebx, 255
						jl		_DrawFullLineSection_LineLocal_HalfAlpha_
						//v? ?ióm kìm theo alpha 0xff000000
						//_DrawFullLineSection_LineLocal_DirectCopy_:
						{
							mov		[edi], cx
							add		edi, 2
							or		edx, edx
							jnz		_DrawFullLineSection_LineLocal_
	
							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}
						//v? ?ióm kìm theo alpha < 255
						_DrawFullLineSection_LineLocal_HalfAlpha_:
						{
							movd    mm6, edx
							shr		ebx, 3
                            movd    mm3, ebx    // mm3: nAlpha
							mix_2_pixel_color_alpha_onrec_16							
							movd    edx, mm6
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
		}
		_DrawPartLineSection_:
		_DrawPartLineSection_Line_:
		{
			movd	edx, mm1    // mm1: Clipper.width
			_DrawPartLineSection_LineLocal_:
			{
				mov		ebx, 255	//alpha
				mov		eax, [esi]				//fullcolor
				add		esi, 4
				or		ebx, ebx
				jnz		_DrawPartLineSection_LineLocal_Alpha_
				add     edi, 2
				dec		edx
				jg		_DrawPartLineSection_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_
			}
			
			_DrawPartLineSection_LineSkip_:
			{
				add		edi, nBuffNextLine
				mov		edx, nSprSkipPerLine
				imul	edx, 4
				add		esi, edx
				jmp		_DrawPartLineSection_Line_
			}
			_DrawPartLineSection_LineLocal_Alpha_:
			{
				mov		ecx, eax
				dec		edx
				cmp		ebx, 255
				jl		_DrawPartLineSection_LineLocal_HalfAlpha_
						
				//_DrawPartLineSection_LineLocal_DirectCopy_:
				{
					mov		[edi], cx
					add		edi, 2
					or		edx, edx
					jnz		_DrawPartLineSection_LineLocal_
					dec		Clipper.height
					jz		_EXIT_WAY_
					jmp		_DrawPartLineSection_LineSkip_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_:
				{
					movd    mm6, edx
                    shr		ebx, 3
					movd    mm3, ebx    // mm3: nAlpha
					mix_2_pixel_color_alpha_onrec_16
					movd    edx, mm6
					or		edx, edx
					jnz		_DrawPartLineSection_LineLocal_
					dec		Clipper.height
					jz		_EXIT_WAY_
					jmp		_DrawPartLineSection_LineSkip_
				}
			}
		}
		_EXIT_WAY_:
        emms
	}
	pCanvas->UnlockCanvas();
#endif
}

void g_DrawAlphaRecImage(void* node, void* canvas)
{
#ifndef _WIN64
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	int nExAlpha = pNode->m_nAlpha;
	if (nExAlpha <= 0)
		return;
	// t1o khung vu?ng trong ph1m vi khung game
	KClipper Clipper;
	if (pCanvas->MakeClip(pNode->m_nX, pNode->m_nY, pNode->m_nWidth, pNode->m_nHeight, &Clipper) == 0)
		return;
	// pBuffer lμ khung v? thμnh phèm
	int nPitch;
	void* pBuffer = pCanvas->LockCanvas(nPitch);
	if (pBuffer == NULL)
		return;
	pBuffer = (char*)(pBuffer) + Clipper.y * nPitch + Clipper.x*2;
	int nMask32 = pCanvas->m_nMask32;	// rgb mask32
	void* pSprite = pNode->m_pBitmap;	// sprite pointer
	int nBuffNextLine = nPitch - Clipper.width * 2;// next line add
	int nSprSkip = pNode->m_nWidth * Clipper.top + Clipper.left;
	int nSprSkipPerLine = Clipper.left + Clipper.right;
	__asm
	{
        mov     eax, Clipper.width
        movd    mm1, eax        // mm1: Clipper.width

        mov     eax, nMask32
        movd    mm2, eax        // mm2: nMask32

        // mm3: nAlpha
        // mm4: temp use
        // mm7: push ecx, pop ecx
        // mm6: push edx, pop edx
        // mm5: push eax, pop eax

		//?i ??n pBuffer + Clipper.y * nPitch + Clipper.x*2
		mov		edi, pBuffer

		//d÷ li?u sprite
		mov		esi, pSprite

		//_SkipSpriteAheadContent_:
		{
			mov		edx, nSprSkip
			or		edx, edx
			jz		_SkipSpriteAheadContentEnd_
			imul	edx, 4
			add		esi, edx
		}
		_SkipSpriteAheadContentEnd_:
		mov		eax, nSprSkipPerLine
		or		eax, eax
		jnz		_DrawPartLineSection_

		//_DrawFullLineSection_:
		{
			//v? full spr kh?ng èn ph?n nμo			
			_DrawFullLineSection_Line_:
			{
				movd	edx, mm1    // mm1: Clipper.width
				_DrawFullLineSection_LineLocal_:
				{
					movzx	ebx, byte ptr[esi+3]	//alpha
					mov		eax, [esi]				//fullcolor
					add		esi, 4
					or		ebx, ebx
					jnz		_DrawFullLineSection_LineLocal_Alpha_
                    add     edi, 2
					dec		edx
					jg		_DrawFullLineSection_LineLocal_

					add		edi, nBuffNextLine
					dec		Clipper.height
					jnz		_DrawFullLineSection_Line_
					jmp		_EXIT_WAY_
				
					_DrawFullLineSection_LineLocal_Alpha_:
					{
						mov		ecx, eax
						dec		edx
						mov		eax, nExAlpha
						imul	ebx, eax
						shr		ebx, 8
						cmp		ebx, 255
						jl		_DrawFullLineSection_LineLocal_HalfAlpha_
						//v? ?ióm kìm theo alpha 0xff000000
						//_DrawFullLineSection_LineLocal_DirectCopy_:
						{
							mov		[edi], cx
							add		edi, 2
							or		edx, edx
							jnz		_DrawFullLineSection_LineLocal_
	
							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}
						//v? ?ióm kìm theo alpha < 255
						_DrawFullLineSection_LineLocal_HalfAlpha_:
						{
							movd    mm6, edx
							shr		ebx, 3
                            movd    mm3, ebx    // mm3: nAlpha
							mix_2_pixel_color_alpha_onrec_16							
							movd    edx, mm6
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
		}
		_DrawPartLineSection_:
		_DrawPartLineSection_Line_:
		{
			movd	edx, mm1    // mm1: Clipper.width
			_DrawPartLineSection_LineLocal_:
			{
				movzx	ebx, byte ptr[esi+3]	//alpha
				mov		eax, [esi]				//fullcolor
				add		esi, 4
				or		ebx, ebx
				jnz		_DrawPartLineSection_LineLocal_Alpha_
				add     edi, 2
				dec		edx
				jg		_DrawPartLineSection_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_
			}
			
			_DrawPartLineSection_LineSkip_:
			{
				add		edi, nBuffNextLine
				mov		edx, nSprSkipPerLine
				imul	edx, 4
				add		esi, edx
				jmp		_DrawPartLineSection_Line_
			}
			_DrawPartLineSection_LineLocal_Alpha_:
			{
				mov		ecx, eax
				dec		edx
				mov		eax, nExAlpha
				imul	ebx, eax
				shr		ebx, 8
				cmp		ebx, 255
				jl		_DrawPartLineSection_LineLocal_HalfAlpha_
						
				//_DrawPartLineSection_LineLocal_DirectCopy_:
				{
					mov		[edi], cx
					add		edi, 2
					or		edx, edx
					jnz		_DrawPartLineSection_LineLocal_
					dec		Clipper.height
					jz		_EXIT_WAY_
					jmp		_DrawPartLineSection_LineSkip_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_:
				{
					movd    mm6, edx
                    shr		ebx, 3
					movd    mm3, ebx    // mm3: nAlpha
					mix_2_pixel_color_alpha_onrec_16
					movd    edx, mm6
					or		edx, edx
					jnz		_DrawPartLineSection_LineLocal_
					dec		Clipper.height
					jz		_EXIT_WAY_
					jmp		_DrawPartLineSection_LineSkip_
				}
			}
		}
		_EXIT_WAY_:
        emms
	}
	pCanvas->UnlockCanvas();
#endif
}

void g_DrawAlphaRecImage32b(void* node, void* canvas)
{
#ifndef _WIN64
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	int nExAlpha = pNode->m_nAlpha;
	if (nExAlpha <= 0)
		return;
	// t1o khung vu?ng trong ph1m vi khung game
	KClipper Clipper;
	if (pCanvas->MakeClip(pNode->m_nX, pNode->m_nY, pNode->m_nWidth, pNode->m_nHeight, &Clipper) == 0)
		return;

	// pBuffer lμ khung v? thμnh phèm
	int nPitch;
	BYTE* pBuffer = (BYTE*)pCanvas->LockCanvas(nPitch);
	if (pBuffer == NULL)
		return;
	pBuffer += Clipper.y * nPitch + Clipper.x*4;
	BYTE* pSprite = (BYTE*)pNode->m_pBitmap;	// sprite pointer
	int nBuffNextLine = nPitch - Clipper.width * 4;// next line add
	pSprite += (pNode->m_nWidth * Clipper.top + Clipper.left)*4;
	int nSprSkipPerLine = Clipper.left + Clipper.right;
	UINT alpha, nSrcColor, nDstColor, R,G,B,R2,G2,B2;
	int nWidth;
	_DrawFullLineSection_Line_:
		nWidth = Clipper.width;
	_DrawFullLineSection_LineLocal_:
		nSrcColor = *((UINT*)pSprite);
		pSprite += 4;
		alpha = (nSrcColor & 0xff000000) >> 24;
		if(alpha)
			goto	_DrawFullLineSection_LineLocal_Alpha_;
        pBuffer += 4;
		nWidth--;
		if(nWidth)
			goto	_DrawFullLineSection_LineLocal_;
		pBuffer += nBuffNextLine;
		pSprite += nSprSkipPerLine*4;
		Clipper.height--;
		if(Clipper.height)
			goto	_DrawFullLineSection_Line_;
		goto	_EXIT_WAY_;
	_DrawFullLineSection_LineLocal_Alpha_:
		nWidth--;
		alpha = alpha*nExAlpha/255;
		if(alpha < 255)
			goto	_DrawFullLineSection_LineLocal_HalfAlpha_;
		//v? ?ióm kìm theo alpha 0xff000000
			*((UINT*)pBuffer) = nSrcColor;
		pBuffer += 4;
		if(nWidth)
			goto	_DrawFullLineSection_LineLocal_;
		pBuffer += nBuffNextLine;
		pSprite += nSprSkipPerLine*4;
		Clipper.height--;
		if(Clipper.height)
			goto	_DrawFullLineSection_Line_;
		goto	_EXIT_WAY_;
	//v? ?ióm kìm theo alpha < 255
	_DrawFullLineSection_LineLocal_HalfAlpha_:
		nDstColor = *((UINT*)pBuffer);
		R = (nSrcColor >> 16) & 0xff;
		G = (nSrcColor >> 8) & 0xff;
		B = nSrcColor & 0xff;
			R2 = (nDstColor >> 16) & 0xff;
			G2 = (nDstColor >> 8) & 0xff;
			B2 = nDstColor & 0xff;
			R = (alpha*R + (255-alpha)*R2)/255;
			G = (alpha*G + (255-alpha)*G2)/255;
			B = (alpha*B + (255-alpha)*B2)/255;
			nDstColor = 0xff000000 | (R << 16) | (G << 8) | B;
			*((UINT*)pBuffer) = nDstColor;
		pBuffer += 4;
		if(nWidth)
			goto	_DrawFullLineSection_LineLocal_;
		pBuffer += nBuffNextLine;
		pSprite += nSprSkipPerLine*4;
		Clipper.height--;
		if(Clipper.height)
			goto	_DrawFullLineSection_Line_;
		goto	_EXIT_WAY_;
	_EXIT_WAY_:
	pCanvas->UnlockCanvas();
#endif
}
