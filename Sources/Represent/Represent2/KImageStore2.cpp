/*****************************************************************************************
//  无贴图的图形资源管理
//	Copyright : Kingsoft 2002-2003
//	Author	: Wooy(Wu yue)
//	CreateTime:	2002-11-11
*****************************************************************************************/
#include "../../Engine/Src/KEngine.h"
#include "KImageStore2.h"
#include "../iRepresent/KRepresentUnit.h"
#include <crtdbg.h.>
#include "../../Engine/Src/KColors.h"
#include <cstdint>

//文件名转化为字符串
unsigned int KImageStore2::ImageNameToId(const char* pszName)
{
	//															-哩函数....,偶其实好委哇嘎-
	unsigned long uId = 0;
	if (pszName && pszName[0])
	{
		for (int i = 1; (*pszName); pszName++)
		{
			if ((*pszName) < 'A' || (*pszName) > 'Z')
				uId = (uId + i * (*pszName)) % 0x8000000b * 0xffffffef;
			else						//中文字符的后字节亦可能被转换，人为地加大了重码的概率■
				uId = (uId + i * ((*pszName) + 0x20)) % 0x8000000b * 0xffffffef;
		}
		uId ^= 0x12345678;
	}
	return uId;
}

char* KImageStore2::GetAdjustColorPalette(int nISPosition, unsigned uColor)
{
	char* pPalette = NULL;
	if (nISPosition >= 0 && nISPosition < m_nNumImages)
	{
		_KISImageObj& Img = m_pObjectList[nISPosition];
		if (Img.pObject && Img.bType == ISI_T_SPR)
		{
			if (Img.pcAdjustColorPalettes == NULL)
			{
				char* pOrigPalette = ((char*)Img.pObject) + sizeof(SPRHEAD);
				Img.pcAdjustColorPalettes = CreateAdjustColorPalette(
					pOrigPalette, ((SPRHEAD*)Img.pObject)->Colors);
			}
            if (Img.pcAdjustColorPalettes)
			{
				for (unsigned int i = 0; i < m_uNumSprAdustColor; i ++)
				{
					if ((uColor & 0xffffff) == m_uSprAdjustColorList[i])
					{
						pPalette = Img.pcAdjustColorPalettes + (2 * i * ((SPRHEAD*)Img.pObject)->Colors);
						break;
					}
				}
			}
			if (pPalette == NULL)
				pPalette = ((char*)Img.pObject) + sizeof(SPRHEAD);
		}
	}
	return pPalette;
}

char* KImageStore2::CreateAdjustColorPalette(const char* pOrigPalette, int nNumColor)
{
	char* pPalette = NULL;
	if (pOrigPalette && nNumColor > 0 && m_uNumSprAdustColor)
	{
		_ASSERT(nNumColor <= 256);
		pPalette = (char*)malloc(2 * nNumColor * m_uNumSprAdustColor);
		if (pPalette)
		{
			KPAL24	OrigPalette[256];
			g_Pal16ToPal24((KPAL16*)pOrigPalette, OrigPalette, nNumColor);

			unsigned short*	pusACP = (unsigned short*)pPalette;
			for (unsigned int i = 0; i < m_uNumSprAdustColor; i++)
			{
				unsigned int	uAR = (m_uSprAdjustColorList[i] >> 16) & 0xff;
				unsigned int	uAG = (m_uSprAdjustColorList[i] >> 8) & 0xff;
				unsigned int	uAB = m_uSprAdjustColorList[i] & 0xff;
				for (int nIndex = 0; nIndex < nNumColor; nIndex++)
				{
					unsigned int uR = uAR * OrigPalette[nIndex].Red / 256;
					unsigned int uG = uAG * OrigPalette[nIndex].Green / 256;
					unsigned int uB = uAB * OrigPalette[nIndex].Blue / 256;
					*(pusACP++) = g_RGB(uR, uG, uB);
				}
			}
		}
	}
	return pPalette;
}

//设置偏色列表
unsigned int KImageStore2::SetAdjustColorList(unsigned int* puColorList, unsigned int uCount)
{
	if (uCount > MAX_ADJUSTABLE_COLOR_NUM)
		uCount = MAX_ADJUSTABLE_COLOR_NUM;
	ClearAllAdjustColorPalette();
	if (puColorList && uCount)
	{
		memcpy(m_uSprAdjustColorList, puColorList, sizeof(unsigned int) * uCount);
		for (unsigned int i = 0; i < uCount; i++)
		{
			m_uSprAdjustColorList[i] &= 0xffffff;
		}
		m_uNumSprAdustColor = uCount;
	}
	else
	{
		m_uNumSprAdustColor = 0;
	}
	return m_uNumSprAdustColor;
}

unsigned int KImageStore2::CreateImage(const char* pszName, int nWidth, int nHeight, int nType)
{
	unsigned int uImage = ImageNameToId(pszName);
	KSGImageContent* pBitmap = NULL;
	AlphaRecContent* pImg = NULL;
	int nIdx;


    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	if (nWidth > 0 && nHeight > 0 && uImage && (nType == ISI_T_BITMAP16 || nType == ISI_T_DRAWINGRC))
	{
		if ((nIdx = FindImage(uImage, 0)) < 0 &&	//必须是不存在同id的图形
			(m_nNumImages < m_nNumReserved || ExpandSpace()))	//有空间存放图形对象
		{
			if(nType == ISI_T_BITMAP16)
				pBitmap = (KSGImageContent *)malloc(KSG_IMAGE_CONTENT_SIZE(nWidth, nHeight));
			else
				pImg = (AlphaRecContent *)malloc(CREATE_REC_CONTENT_SIZE(nWidth, nHeight));
		}
	}
	
	if (pBitmap)
	{
		pBitmap->nWidth = nWidth;
		pBitmap->nHeight = nHeight;

		nIdx = - nIdx - 1;
		for (int i = m_nNumImages; i > nIdx; i--)
		{
			m_pObjectList[i] = m_pObjectList[i - 1];
		}
		m_pObjectList[nIdx].bNotCacheable = true;
		m_pObjectList[nIdx].dwLastUsedTime = GetTickCount();
		m_pObjectList[nIdx].bSingleFrameLoad = false;
		m_pObjectList[nIdx].bType = ISI_T_BITMAP16;
		m_pObjectList[nIdx].pObject = pBitmap;
		m_pObjectList[nIdx].pFrames = NULL;
		m_pObjectList[nIdx].uId = uImage;
		m_pObjectList[nIdx].pSurface = NULL;
		m_nNumImages++;
	}
	else if(pImg)
	{
		pImg->nWidth = nWidth;
		pImg->nHeight = nHeight;

		nIdx = - nIdx - 1;
		for (int i = m_nNumImages; i > nIdx; i--)
		{
			m_pObjectList[i] = m_pObjectList[i - 1];
		}
		m_pObjectList[nIdx].bNotCacheable = true;
		m_pObjectList[nIdx].dwLastUsedTime = GetTickCount();
		m_pObjectList[nIdx].bSingleFrameLoad = false;
		m_pObjectList[nIdx].bType = ISI_T_DRAWINGRC;
		m_pObjectList[nIdx].pObject = pImg;
		m_pObjectList[nIdx].pFrames = NULL;
		m_pObjectList[nIdx].uId = uImage;
		m_pObjectList[nIdx].pSurface = NULL;
		m_nNumImages++;
	}
	else
		uImage = 0;
	return uImage;
}

void KImageStore2::Free()
{
    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	if (m_pObjectList)
	{
        for (int i = 0; i < m_nNumImages; i++)
			FreeImageObject(m_pObjectList[i]);
		free(m_pObjectList);
		m_pObjectList = NULL;
	}
	m_nNumImages = 0;
	m_nNumReserved = 0;

	m_uImageAccessCounter = 0;
}

void KImageStore2::FreeImage(const char* pszImage)
{
    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	unsigned int uImage = ImageNameToId(pszImage);
	int nIdx = FindImage(uImage, 0);
	if (nIdx >= 0)
	{
		FreeImageObject(m_pObjectList[nIdx]);
		m_nNumImages--;
		for (int i = nIdx; i < m_nNumImages; i++)
			m_pObjectList[i] = m_pObjectList[i + 1];
	}
}

void* KImageStore2::GetExistedCreateBitmap(const char* pszImage, unsigned int uImage, short& nImagePosition)
{
	if (uImage == 0)
		uImage = ImageNameToId(pszImage);

    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	nImagePosition = FindImage(uImage, nImagePosition);
	void* pObject = NULL;
	if (nImagePosition >= 0 &&
		(m_pObjectList[nImagePosition].bType == ISI_T_BITMAP16 || m_pObjectList[nImagePosition].bType == ISI_T_DRAWINGRC) &&
		m_pObjectList[nImagePosition].bNotCacheable == true &&
		m_pObjectList[nImagePosition].pObject)
	{
		pObject = (m_pObjectList[nImagePosition].pObject);
	}
	return pObject;
}

LPDIRECTDRAWSURFACE CreateDirectDrawSurfaceFromBitmap(KSGImageContent* pBitmap)
{
	LPDIRECTDRAWSURFACE pSurface = NULL;
	if (pBitmap && g_pDirectDraw)
	{
		pSurface = g_pDirectDraw->CreateSurface(pBitmap->nWidth, pBitmap->nHeight);
	}
	return pSurface;
}

bool KImageStore2::CreateBitmapSurface(const char* pszImage, unsigned int& uImage, short& nImagePosition)
{
	if (uImage == 0)
		uImage = ImageNameToId(pszImage);

	bool bOk = false;

	KAutoCriticalSection AutoLock(m_ImageProcessLock);

	nImagePosition = FindImage(uImage, nImagePosition);
	if (nImagePosition >= 0)
	{
		_KISImageObj& Img = m_pObjectList[nImagePosition];
        if (Img.bType == ISI_T_BITMAP16 && Img.bNotCacheable == true && Img.pObject)
		{
			if (Img.pSurface == NULL)
			{
				Img.pSurface = CreateDirectDrawSurfaceFromBitmap(
								(KSGImageContent *)(Img.pObject));
			}
			bOk = (Img.pSurface != NULL);
		}
	}
	return bOk;
}

void* KImageStore2::GetImage(const char* pszImage, unsigned int& uImage,
					short& nImagePosition, int nFrame, int nType, void*& pFrameData)
{
	pFrameData = NULL;
	if (uImage == 0)
	{
		if ((uImage = ImageNameToId(pszImage)) == 0)
			return NULL;
	}

    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	void* pObject = NULL;
	if ((nImagePosition = FindImage(uImage, nImagePosition)) >= 0)
	{
		_KISImageObj&	ImgObj = m_pObjectList[nImagePosition];
		if (ImgObj.bType == (unsigned char)nType)
		{
			if (ImgObj.pObject)
			{
				pObject = ImgObj.pObject;
				if (ImgObj.bType == ISI_T_SPR)
				{
					if ((pFrameData = GetSprFrame(pszImage, ImgObj, nFrame)) == NULL)
						pObject = NULL;
				}
				else if (ImgObj.bType == ISI_T_BITMAP16)
				{
					pFrameData = ImgObj.pSurface;
				}
			}
			ImgObj.bRef = true;
			ImgObj.dwLastUsedTime = GetTickCount();
		}
		//else 略去同id但是不同图形类型情况的处理。因为这是受限情况

		//为了执行效率所以未把下面的判断放在CheckBalance函数体里面。下同。
		if (m_nNumImages > m_nBalanceNum && (++m_uImageAccessCounter) > m_uCheckPoint)
			CheckBalance();
		return pObject;
	}

	// 有空间
	if (m_nNumImages < m_nNumReserved || ExpandSpace())
	{
		nImagePosition = - nImagePosition - 1;	// FindImage时已经找好位置了
		for (int i = m_nNumImages; i > nImagePosition; i--)
		{
			m_pObjectList[i] = m_pObjectList[i - 1];
		}

		_KISImageObj& ImgObj = m_pObjectList[nImagePosition];		
		ImgObj.bNotCacheable = false;
		ImgObj.bRef = true;
		ImgObj.dwLastUsedTime = GetTickCount();
		ImgObj.bSingleFrameLoad = false;
		ImgObj.bType = (unsigned char)nType;
		ImgObj.pFrames = NULL;
		ImgObj.pObject = NULL;
		ImgObj.uId = uImage;
		ImgObj.pcAdjustColorPalettes = NULL;

		pObject = LoadImage(pszImage, ImgObj, nFrame, pFrameData);
		m_nNumImages++;

		if (m_nNumImages > m_nBalanceNum && (++m_uImageAccessCounter) > m_uCheckPoint)
			CheckBalance();
	}
	return pObject;
}

void* KImageStore2::GetSprFrame(const char* pszImageFile, _KISImageObj& ImgObject, int nFrame)
{
	SPRHEAD* pSprHeader = (SPRHEAD*)ImgObject.pObject;
	void* pFrameData = NULL;

	//_ASSERT(pszImageFile && pSprHeader);
	if (nFrame >= 0 && nFrame < pSprHeader->Frames)
	{
		_KISImageFrameObj* pFrame;
		if (ImgObject.bSingleFrameLoad == false)
		{	//一次加载全部帧的图形
			pFrame = ImgObject.pFrames;
			//_ASSERT(pFrame);
			//_ASSERT(pFrame->pOffsetTable);
			pFrameData = (((char*)pFrame->pOffsetTable) + pFrame->sOffTableSize +
				((SPROFFS*)pFrame->pOffsetTable)[nFrame].Offset);
			pFrame->bRef = true;
		}
		else
		{
			pFrame = &ImgObject.pFrames[nFrame];
			pFrame->bRef = true;
			if ((pFrameData = pFrame->pFrameData) == NULL)
			{	//指定的帧数据还不存在
				pFrame->pFrameData = SprGetFrame((SPRHEAD*)ImgObject.pObject, nFrame);
				pFrameData = pFrame->pFrameData;
			}
		}
	}
	return pFrameData;
}

bool KImageStore2::GetImageParam(const char* pszImage, int nType, KImageParam* pImageData)
{
	unsigned int uImage = ImageNameToId(pszImage);
	
	if (uImage == 0)
		return false;

    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	short nImagePosition = FindImage(uImage, -1);
	if (nImagePosition < 0)
	{
		void* pFrameData;
		if (GetImage(pszImage, uImage, nImagePosition, 0, nType, pFrameData) == NULL)
			nImagePosition = -1;
	}
	if (nImagePosition >= 0)
	{
		_KISImageObj&	ImgObj = m_pObjectList[nImagePosition];
		if (ImgObj.pObject && ImgObj.bType == (unsigned char)nType)
		{
			if (pImageData)
			{
				if (ImgObj.bType == ISI_T_SPR)
				{
					SPRHEAD* pSprHeader = (SPRHEAD*)ImgObj.pObject;
					pImageData->nWidth = pSprHeader->Width;
					pImageData->nHeight = pSprHeader->Height;
					pImageData->nInterval = pSprHeader->Interval;
					pImageData->nNumFrames = pSprHeader->Frames;
					pImageData->nNumFramesGroup = pSprHeader->Directions;
					pImageData->nReferenceSpotX = pSprHeader->CenterX;
					pImageData->nReferenceSpotY = pSprHeader->CenterY;				
				}
				else if (ImgObj.bType == ISI_T_BITMAP16)
				{
					pImageData->nWidth  = ((KSGImageContent*)(ImgObj.pObject))->nWidth;
					pImageData->nHeight = ((KSGImageContent*)(ImgObj.pObject))->nHeight;
					pImageData->nInterval = 0;
					pImageData->nNumFrames = 1;
					pImageData->nNumFramesGroup = 1;
					pImageData->nReferenceSpotX = 0;
					pImageData->nReferenceSpotY = 0;
				}
			}
			return true;
		}
	}
	return false;
}

bool KImageStore2::GetImageFrameParam(const char* pszImage, int nType,
		int nFrame, KRPosition2* pOffset, KRPosition2* pSize)
{	
	bool bRet = false;
	short	nPos = -1;
	unsigned int uImage = 0;
	void* pFrame;
	void* pImage = GetImage(pszImage, uImage, nPos, 0, nType, pFrame);
	if (pImage)
	{
		if (nType == ISI_T_SPR)
		{
			if (pOffset)
			{
				pOffset->nX = ((SPRFRAME*)pFrame)->OffsetX;
				pOffset->nY = ((SPRFRAME*)pFrame)->OffsetY;
			}
			if (pSize)
			{
				pSize->nX = ((SPRFRAME*)pFrame)->Width;
				pSize->nY = ((SPRFRAME*)pFrame)->Height;
			}
		}
		else if (nType == ISI_T_BITMAP16)
		{
			if (pOffset)
			{
				pOffset->nX = 0;
				pOffset->nY = 0;
			}
			if (pSize)
			{
				pSize->nX = ((KSGImageContent*)pImage)->nWidth;
				pSize->nY = ((KSGImageContent*)pImage)->nHeight;
			}
		}
		bRet = true;
	}
	return bRet;
}

int KImageStore2::GetImagePixelAlpha(const char* pszImage, int nType, int nFrame, int nX, int nY)
{
	int nRet = 0;
	short	nPos = -1;
	unsigned int uImage = 0;
	SPRFRAME* pFrame;
	void* pImage = GetImage(pszImage, uImage, nPos, 0, nType, (void*&)pFrame);
	if (pImage)
	{
		if (nType == ISI_T_SPR)
		{
			nX -= pFrame->OffsetX;
			nY -= pFrame->OffsetY;
			if (nX >= 0  && nX < pFrame->Width && nY >= 0 && nY < pFrame->Height)
			{
				int	nNumPixels = pFrame->Width;
				void*	pSprite =  pFrame->Sprite;
				nY++;
				_asm
				{
					//使SDI指向sprite中的图形数据位置
					mov		esi, pSprite
				dec_line:
					dec		nY				//减掉一行
					jz		last_line
					
					mov		edx, nNumPixels
				skip_line:
					movzx	eax, byte ptr[esi]
					inc		esi
					movzx	ebx, byte ptr[esi]
					inc		esi
					or		ebx, ebx
					jz		skip_line_continue
					add		esi, eax
				skip_line_continue:
					sub		edx, eax
					jg		skip_line
					jmp		dec_line

				last_line:
					mov		edx, nX
				last_line_alpha_block:
					movzx	eax, byte ptr[esi]
					inc		esi
					movzx	ebx, byte ptr[esi]
					inc		esi
					or		ebx, ebx
					jz		last_line_continue
					add		esi, eax
				last_line_continue:
					sub		edx, eax
					jg		last_line_alpha_block

					mov		nRet, ebx
				}
			}
		}
		else if (nType == ISI_T_BITMAP16)
		{
			if (nX >= 0 && nY >= 0 &&
				nX < ((KSGImageContent*)pImage)->nWidth &&
				nY < ((KSGImageContent*)pImage)->nHeight)
			{
				nRet = 255;
			}
		}
	}
	return nRet;
}

bool KImageStore2::Init()
{
	Free();
	return ExpandSpace();
}

bool KImageStore2::SaveImage(const char* pszFile, const char* pszImage, int nFileType)
{
	return 0;
}

void KImageStore2::SetBalanceParam(int nNumImage, unsigned int uCheckPoint)
{
	m_nBalanceNum = nNumImage;
	m_uCheckPoint = uCheckPoint;
}

void KImageStore2::CheckBalance()
{
	m_uImageAccessCounter = 0;
	int i, j;
    int nNewNumImages = 0;
    
    MEMORYSTATUS MemStatus;
    GlobalMemoryStatus(&MemStatus);

    // 如果剩余可分配的物理内存大于整个物理内存的一个比例，则不需要进行Cache调整
    if (MemStatus.dwAvailPhys > (MemStatus.dwTotalPhys / 32))
    {
        if (m_nNumImages < 1024)    // Cache中图片的上限，Cache中最多有1024张图片
            return;
    }

    KAutoCriticalSection AutoLock(m_ImageProcessLock);
    
    // Ban goc JX2 duoi anh theo THOI GIAN NHAN ROI *VA* NGAN SACH (160/192/256/512 MB tuy RAM,
    // dung ngay khi tong byte ve duoi ngan sach). Neu chi lay ve thoi gian nhan roi ma bo ngan
    // sach thi o map DONG NGUOI moi anh deu 'vua dung' => KHONG gi bi duoi => so anh tang vo han
    // => can bo nho (client 32-bit) => crash. JX1 do bang SO ANH chu khong do byte, nen tran o day
    // dong vai tro ngan sach: vuot tran cung thi bo qua bao ve nhan roi, duoi nhu hanh vi cu.
    #define KIS_HARD_IMAGE_CEIL  2048
    unsigned int dwIdleLimit;
    if (MemStatus.dwAvailPhys <= (MemStatus.dwTotalPhys / 32))
        dwIdleLimit = 10000;                 // RAM cang: duoi manh tay
    else if (m_nNumImages >= KIS_HARD_IMAGE_CEIL)
        dwIdleLimit = 0;                     // vuot tran cung: duoi nhu truoc khi co ban va
    else
        dwIdleLimit = 100000;                // binh thuong: chi don anh that su nguoi

    nNewNumImages = 0;
    for (i = 0; i < m_nNumImages; i++)
    {
        if (m_pObjectList[i].bNotCacheable == false)
        {
			// Ban goc JX2 duoi anh khi NHAN ROI qua lau (100 giay), chu khong duoi moi thu
			// khong duoc cham trong luot quet vua roi. Cach cu: map dong nguoi quay vong hon
			// m_uCheckPoint anh khac nhau => XOA DUNG ANH DANG CAN roi nap lai ngay tu pak
			// => dung hinh vai giay + CPU/dia dung dung.
			if (m_pObjectList[i].bRef == false &&
				(GetTickCount() - m_pObjectList[i].dwLastUsedTime) > dwIdleLimit)
			{
				FreeImageObject(m_pObjectList[i]);
                continue;
            }

			// CHI xoa tung khung khi RAM THAT SU CANG.
			// Nhanh nay chi chay khi anh duoc nap THEO KHUNG (bSingleFrameLoad), ma dieu
			// do chi xay ra tu khi pak duoc dong goi lai che do TYPE_FRAME. Truoc do no
			// KHONG BAO GIO chay. CheckBalance duoc goi sau moi 384 luot truy cap anh
			// (ISBP_CHECK_POINT_DEF), ma canh dong ve 2.200 anh/khung => no chay ~5,7 LAN
			// MOI KHUNG => moi khung hinh khong duoc ve trong ~1/6 khung vua qua deu bi
			// xoa; nhan vat quay huong hay doi dong tac la phai nap lai tu pak ngay giua
			// vong ve => lag nang + nguoi khong hien ra + hoat anh ket o tu the cu.
			// Muc dich goc cua nhanh nay la TIET KIEM RAM, nen khi RAM con nhieu thi giu
			// khung lai (dung nhu hanh vi truoc khi repack pak).
			if (
                (MemStatus.dwAvailPhys <= (MemStatus.dwTotalPhys / 16)) &&
                (m_pObjectList[i].bSingleFrameLoad == true) &&
				(m_pObjectList[i].pObject) &&
				(m_pObjectList[i].bType == ISI_T_SPR)
            )
			{
				int nNumFrame = ((SPRHEAD*)(m_pObjectList[i].pObject))->Frames;
				for (j = 0; j < nNumFrame; j++)
				{
					if (m_pObjectList[i].pFrames[j].bRef == false)
					{
						FreeImageObject(m_pObjectList[i], j);
					}
                    else
                    {
                        m_pObjectList[i].pFrames[j].bRef = false;
                    }
				}
			}
        }

        m_pObjectList[nNewNumImages] = m_pObjectList[i];
        m_pObjectList[nNewNumImages].bRef = false;
        nNewNumImages++;
    }
    m_nNumImages =  nNewNumImages;
}

bool KImageStore2::ExpandSpace()
{
	_KISImageObj* pNewList = (_KISImageObj *)realloc(m_pObjectList,
						(m_nNumReserved + ISBP_EXPAND_SPACE_STEP) * sizeof(_KISImageObj));
	if (pNewList)
	{
		m_pObjectList = pNewList;
		m_nNumReserved += ISBP_EXPAND_SPACE_STEP;
		return true;
	}
	return false;
}

int KImageStore2::FindImage(unsigned int uImage, int nPossiblePosition)
{
	int nPP = nPossiblePosition;
	if (nPP < 0 || nPP >= m_nNumImages)
	{
		if (m_nNumImages <= 0)
		{
			return -1;
		}
		else
		{
			nPP = m_nNumImages / 2;
		}
	}
	if (m_pObjectList[nPP].uId == uImage)
		return nPP;
	int nFrom, nTo, nTryRange;
	nTryRange = ISBP_TRY_RANGE_DEF;
	if (m_pObjectList[nPP].uId > uImage)
	{
		nFrom = 0;
		nTo = nPP - 1;
		nPP -= nTryRange;
	}
	else
	{
		nFrom = nPP + 1;
		nTo = m_nNumImages - 1;
		nPP += nTryRange;
	}
	if (nFrom + nTryRange >= nTo)
		nPP = (nFrom + nTo) / 2;

	while (nFrom < nTo)
	{
		if (m_pObjectList[nPP].uId < uImage)
		{
			nFrom = nPP + 1;
		}
		else if (m_pObjectList[nPP].uId > uImage)
		{
			nTo = nPP - 1;
		}
		else
		{
			return nPP;
		}
		nPP = (nFrom + nTo) / 2;
	}
	if (nFrom == nTo)
	{
		if (m_pObjectList[nPP].uId > uImage)
		{
			nPP = - nPP - 1;
		}
		else if (m_pObjectList[nPP].uId < uImage)
		{
			nPP = - nPP - 2;
		}
	}
	else
	{
		nPP = - nFrom -1;
	}
	return nPP;
}

void KImageStore2::FreeImageObject(_KISImageObj& ImgObject, int nFrame/*=-1*/)
{
	if(ImgObject.pObject)
	{
		if (ImgObject.bType == ISI_T_SPR)
		{
			if (ImgObject.bSingleFrameLoad == false)
			{
				free (ImgObject.pFrames);
				ImgObject.pFrames = NULL;
				if (ImgObject.pcAdjustColorPalettes)
				{
					free(ImgObject.pcAdjustColorPalettes);
					ImgObject.pcAdjustColorPalettes = NULL;
				}
				if (ImgObject.pObject)
				{
					SprReleaseHeader((SPRHEAD*)ImgObject.pObject);
					ImgObject.pObject = NULL;
				}
			}
			else
			{
				int nNumFrame = ((SPRHEAD*)ImgObject.pObject)->Frames;
				if (nFrame >= 0 && nFrame < nNumFrame)
				{
					if (ImgObject.pFrames[nFrame].pFrameData)
					{
						SprReleaseFrame((SPRFRAME*)ImgObject.pFrames[nFrame].pFrameData);
						ImgObject.pFrames[nFrame].pFrameData = NULL;
					}
				}
				else if (nFrame < 0)
				{
					for (nFrame = 0; nFrame < nNumFrame; nFrame++)
					{
						if (ImgObject.pFrames[nFrame].pFrameData)
							SprReleaseFrame((SPRFRAME*)ImgObject.pFrames[nFrame].pFrameData);
					}
					free (ImgObject.pFrames);
					ImgObject.pFrames = NULL;
					if (ImgObject.pcAdjustColorPalettes)
					{
						free(ImgObject.pcAdjustColorPalettes);
						ImgObject.pcAdjustColorPalettes = NULL;
					}
					SprReleaseHeader((SPRHEAD*)(ImgObject.pObject));
					ImgObject.pObject = NULL;
				}
			}
		}
		else if (ImgObject.bType == ISI_T_BITMAP16)
		{
			if (ImgObject.pObject)
			{
				// Khoi nay do get_jpg_image() cua ENGINE.DLL cap phat (malloc cua CRT trong
				// engine - xem Sources/Engine/Src/KPakFile.cpp:171). Goi free() o day la giai
				// phong bang CRT cua Represent2 => KHAC VUNG NHO DONG => 0xC0000374 heap
				// corruption. Chi lo o map DONG NGUOI vi phai qua 1024 anh thi CheckBalance
				// moi bat dau duoi anh. Engine da xuat san release_image() dung cho viec nay
				// (KPakFile.h:58); moi cho khac trong tep nay deu tra ve engine
				// (SprReleaseHeader / SprReleaseFrame) - chi rieng cho nay bo quen.
				// Nhanh else cu goi free((AlphaRecContent*)...) la ma chet: dieu kien ngoai
				// da chot bType == ISI_T_BITMAP16 roi nen khong bao gio chay toi.
				release_image((KSGImageContent *)ImgObject.pObject);
				ImgObject.pObject = NULL;
			}
		}
		else
			ImgObject.pObject = NULL;
	}
}

// LCG parameters
static uint32_t LCG_Next(uint32_t& state) {
	const uint32_t a = 1103515245;
	const uint32_t c = 12345;
	const uint32_t m = 1u << 31;
	state = (a * state + c) % m;
	return (state >> 16) & 0xFF;
}
void DecryptPalette(KPAL24* palette, int numColors, BYTE xorKey) {
	uint32_t state = xorKey;  // Use Reserved[3] as the initial seed
	uint8_t* palBytes = reinterpret_cast<uint8_t*>(palette);

	// Decrypt each byte of the palette using the XOR key stream generated from LCG
	for (size_t i = 0; i < numColors * sizeof(KPAL24); ++i) {
		palBytes[i] ^= LCG_Next(state);  // XOR each byte with the generated value
	}
}
void* KImageStore2::LoadImage(const char* pszImageFile, _KISImageObj& ImgObj, int nFrame, void*& pFrameData)
{
	void* pRet = NULL;
	_KISImageFrameObj* pFrameObj = NULL;
	if (ImgObj.bType == ISI_T_SPR)
	{
		SPROFFS*	pOffsTable = NULL;
		SPRHEAD*  pSprHeader = SprGetHeader(pszImageFile, pOffsTable);
		if (pSprHeader)
		{
			if (pOffsTable)	//一次加载完整的spr图
			{
				pFrameObj = (_KISImageFrameObj*)malloc(sizeof(_KISImageFrameObj));
				if (pFrameObj)
				{
					ImgObj.pObject = pSprHeader;
					ImgObj.pFrames = pFrameObj;
					ImgObj.bNotCacheable = false;
					ImgObj.dwLastUsedTime = GetTickCount();
					ImgObj.bSingleFrameLoad = false;
					pFrameObj->pOffsetTable = pOffsTable;
					pFrameObj->sOffTableSize = sizeof(SPROFFS) * pSprHeader->Frames;
					pFrameObj->bRef = true;

					if (nFrame >= 0 && nFrame < pSprHeader->Frames)
					{
						pFrameData = ((char*)pOffsTable + pFrameObj->sOffTableSize +
							pOffsTable[nFrame].Offset);
						pRet = pSprHeader;
					}					
				}
			}
			else	//分帧加载的图
			{
				int nSize = sizeof(_KISImageFrameObj) * pSprHeader->Frames;
				pFrameObj = (_KISImageFrameObj*)malloc(nSize);
				if (pFrameObj)
				{
					memset(pFrameObj, 0, nSize);
					ImgObj.pObject = pSprHeader;
					ImgObj.pFrames = pFrameObj;
					ImgObj.bNotCacheable = false;
					ImgObj.dwLastUsedTime = GetTickCount();
					ImgObj.bSingleFrameLoad = true;
					if (nFrame >= 0 && nFrame < pSprHeader->Frames)
					{
						pFrameData = (SPRFRAME*)SprGetFrame(pSprHeader, nFrame);
						if (pFrameData)
						{
							ImgObj.pFrames[nFrame].bRef = true;
							ImgObj.pFrames[nFrame].pFrameData = (SPRFRAME*)pFrameData;
							pRet = pSprHeader;
						}
					}
				}
			}
			char* palette = ((char*)pSprHeader);
			palette += sizeof(SPRHEAD);
			// setup palette pointer
			KPAL24* pPal24;
			pPal24 = new KPAL24[pSprHeader->Colors];

			// Extract XOR seed from Reserved[3]
			BYTE paletteXorKey = static_cast<BYTE>(pSprHeader->Reserved[3] & 0xFF);
			if (paletteXorKey)
			{
				// Allocate and decrypt palette
				memcpy(pPal24, palette, pSprHeader->Colors * sizeof(KPAL24));
				DecryptPalette(pPal24, pSprHeader->Colors, paletteXorKey);
				//palette = (char*)pPal24;
				if (ImgObj.pObject)
				{
					if(g_pDirectDraw->GetRGBBitCount() != 32)
					{
						g_Pal24ToPal16(pPal24,
						(KPAL16*)(&pSprHeader[1]), pSprHeader->Colors);
					}
					else
					{
						memcpy(&pSprHeader[1], pPal24,  pSprHeader->Colors * sizeof(KPAL24));
					}
					delete[] pPal24;
				}
				else
				{
					SprReleaseHeader(pSprHeader);
					ImgObj.pcAdjustColorPalettes = NULL;
				}
			}
			else
			{
				if (ImgObj.pObject)
				{
					if(g_pDirectDraw->GetRGBBitCount() != 32)	//ch? ?é 16bit míi c?n chuyón palette
					{
						g_Pal24ToPal16((KPAL24*)(&pSprHeader[1]),
						(KPAL16*)(&pSprHeader[1]), pSprHeader->Colors);
					}
				}
				else
				{
					SprReleaseHeader(pSprHeader);
					ImgObj.pcAdjustColorPalettes = NULL;
				}
			}
		}
	}
	else if (ImgObj.bType == ISI_T_BITMAP16)
	{
		ImgObj.pObject = get_jpg_image(pszImageFile);
		pRet = ImgObj.pObject;
	}
	return pRet;
}

KImageStore2::KImageStore2()
{
	m_pObjectList = NULL;
	m_nNumReserved = 0;
	m_nNumImages  = 0;
	m_nBalanceNum = ISBP_BALANCE_NUM_DEF;
	m_uCheckPoint = ISBP_CHECK_POINT_DEF;
	m_uImageAccessCounter = 0;
}

KImageStore2::~KImageStore2()
{
	Free();
}

void KImageStore2::ClearAllAdjustColorPalette()
{
    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	for (int i = 0; i < m_nNumImages; i++)
	{
		if (m_pObjectList[i].pcAdjustColorPalettes)
		{
			free (m_pObjectList[i].pcAdjustColorPalettes);
			m_pObjectList[i].pcAdjustColorPalettes = NULL;
		}
	}
}