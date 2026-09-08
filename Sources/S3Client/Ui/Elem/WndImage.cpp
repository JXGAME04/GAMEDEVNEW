/*****************************************************************************************
//	界面窗口体系结构--图形窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-25
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "WndImage.h"
#include "UiToaDo.h"	// [UITOADO]

#include "../../../Represent/iRepresent/iRepresentShell.h"
extern iRepresentShell*	g_pRepresentShell;

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndImage::KWndImage()
{
	IR_InitUiImageRef(m_Image);
}

void KWndImage::Clone(KWndImage* pCopy)
{
	if (pCopy)
	{
		KWndWindow::Clone(pCopy);
		pCopy->m_Image	= m_Image;
	}
}

//--------------------------------------------------------------------------
//	功能：初始化窗口
//--------------------------------------------------------------------------
int KWndImage::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndWindow::Init(pIniFile, pSection))
	{
		int		nValue = 0;
		pIniFile->GetInteger(pSection, "Trans", 0, &nValue);
		if (nValue)
			m_Style |= WNDIMG_ES_EXCLUDE_TRANS;
		else
			m_Style &= ~WNDIMG_ES_EXCLUDE_TRANS;
		m_Image.nFlipTime = 0;
		m_Image.nInterval = 0;
		m_Image.nNumFrames = 0;
		pIniFile->GetInteger(pSection, "ImgType", 0, &nValue);
		if (nValue == 1)
		{
			m_Image.nType = ISI_T_BITMAP16;
			m_Image.bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
			m_Style &= ~WNDIMG_ES_EXCLUDE_TRANS;
		}
		else
		{
			m_Image.nType = ISI_T_SPR;
			m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			m_Image.Color.Color_b.a = 255;
		}
		m_Image.uImage = 0;
		m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
		pIniFile->GetString(pSection, "Image", "" , m_Image.szImage, sizeof(m_Image.szImage));
		if (pIniFile->GetInteger(pSection, "Frame", -1, &nValue) && nValue != -1)
			m_Image.nFrame = nValue;

		if ((m_Width == 0 || m_Height == 0) && g_pRepresentShell)
		{
			KImageParam	Param = { 0 };
			if (g_pRepresentShell->GetImageParam(m_Image.szImage, &Param, m_Image.nType))
			{
				if (m_Width == 0)
					m_Width = Param.nWidth;
				if (m_Height == 0)
					m_Height = Param.nHeight;
			}
		}
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：判断一个点是否在窗口范围内,传入的是绝对坐标
//--------------------------------------------------------------------------
int KWndImage::PtInWindow(int x, int y)
{
	int	bIn = KWndWindow::PtInWindow(x, y);
	
	// [UITOADO] menh de thu hai tu kiem WND_S_VISIBLE nen phai kiem them
	// bit an, khong thi o da "xoa" van bat duoc chuot
	if ((m_Style & WND_S_UITOADO_AN) && !UiToaDo_DangSua())
		return 0;

	if ((bIn &&
			(m_Style & WND_S_SIZE_WITH_ALL_CHILD) == 0) || 
		((m_Style & WND_S_VISIBLE) &&
			bIn == false &&
			(m_Style & WND_S_SIZE_WITH_ALL_CHILD)))
	{
		if ((m_Style & WNDIMG_ES_EXCLUDE_TRANS) && g_pRepresentShell)
		{
			// [UITOADO] anh dang ve to/nho => quy diem cham ve he toa do goc
			// cua anh truoc khi do alpha, khong thi bam lech
			int nDx = x - m_nAbsoluteLeft;
			int nDy = y - m_nAbsoluteTop;
			if (m_nUiTiLe != 1000 && m_nUiTiLe > 0)
			{
				nDx = nDx * 1000 / m_nUiTiLe;
				nDy = nDy * 1000 / m_nUiTiLe;
			}
			bIn =  g_pRepresentShell->GetImagePixelAlpha(m_Image.szImage, m_Image.nFrame, nDx, nDy, m_Image.nType);
		}
	}
	return bIn;
}

//--------------------------------------------------------------------------
//	功能：设置图形帧
//--------------------------------------------------------------------------
void KWndImage::SetFrame(int nFrame)
{
	m_Image.nFrame = nFrame;
}

void KWndImage::SetMaxFrame(int nMaxFrame)
{
    m_Image.nNumFrames = nMaxFrame;
} 

int	KWndImage::GetCurrentFrame()
{
	return m_Image.nFrame;
}

void KWndImage::SetImage(short nType, const char* pszImgName, bool bAdjustWndSize)
{
	m_Image.nType = nType;
	strncpy(m_Image.szImage, pszImgName, sizeof(m_Image.szImage));
	m_Image.szImage[sizeof(m_Image.szImage) - 1] = 0;
	m_Image.nNumFrames = 0;
	m_Image.uImage = 0;
	m_Image.nFlipTime = IR_GetCurrentTime();
	if (bAdjustWndSize && g_pRepresentShell)
	{
		KImageParam	Param;
		if (g_pRepresentShell->GetImageParam(m_Image.szImage, &Param, nType))
			SetSize(Param.nWidth, Param.nHeight);
	}
}

void KWndImage::UpdateTimer()
{
	m_Image.nFlipTime = IR_GetCurrentTime();
}

//--------------------------------------------------------------------------
//	功能：图形换帧
//--------------------------------------------------------------------------
int KWndImage::NextFrame()
{
	return IR_NextFrame(m_Image);
}

//--------------------------------------------------------------------------
//	功能：窗体绘制
//--------------------------------------------------------------------------
void KWndImage::PaintWindow()
{
	KWndWindow::PaintWindow();
	if (g_pRepresentShell)
	{
		m_Image.oPosition.nX = m_nAbsoluteLeft;
		m_Image.oPosition.nY = m_nAbsoluteTop;

		// [UITOADO] ti le 1000 (= 100%) thi giu NGUYEN duong ve cu, khong
		// lech mot diem anh nao. Chi khi nguoi choi that su chinh to/nho moi
		// doi sang RU_T_IMAGE_STRETCH:
		//   RU_T_IMAGE        -> DrawSpriteAlpha     : mien nguon = mien dich,
		//                        ti le LUON 1, khong bao gio co gian
		//   RU_T_IMAGE_STRETCH-> DrawSpritePartAlpha : mien nguon la khung anh
		//                        => co gian that (KItem::PaintItem dang dung)
		// Duong STRETCH KHONG tu cong offset cua khung anh nen phai cong tay.
		// Represent2 (DirectDraw) khong co duong co gian cho SPR => bo qua.
		if (m_nUiTiLe > 0 && m_nUiTiLe != 1000 &&
			m_Image.szImage[0] && g_pRepresentShell->IsRep3D())
		{
			KRPosition2	oOff = {0, 0};
			KRPosition2	oCo  = {0, 0};

			if (g_pRepresentShell->GetImageFrameParam(m_Image.szImage,
					m_Image.nFrame, &oOff, &oCo, m_Image.nType) &&
				oCo.nX > 0 && oCo.nY > 0)
			{
				int nX = m_nAbsoluteLeft + oOff.nX * m_nUiTiLe / 1000;
				int nY = m_nAbsoluteTop  + oOff.nY * m_nUiTiLe / 1000;

				m_Image.oPosition.nX = nX;
				m_Image.oPosition.nY = nY;
				m_Image.oEndPos.nX   = nX + oCo.nX * m_nUiTiLe / 1000;
				m_Image.oEndPos.nY   = nY + oCo.nY * m_nUiTiLe / 1000;
				g_pRepresentShell->DrawPrimitives(1, &m_Image, RU_T_IMAGE_STRETCH, true);
				return;
			}
		}
		g_pRepresentShell->DrawPrimitives(1, &m_Image, RU_T_IMAGE, true);
	}
}