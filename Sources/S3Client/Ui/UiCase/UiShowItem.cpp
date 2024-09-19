/* 
 * File:     UiShowItem.cpp
// Author: Fong Ki“u
 * Creation: 09/11/12
 * 
 */
//-----------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "../Elem/MouseHover.h"
#include "UiShowItem.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../Engine/src/Text.h"
#include "../../../core/src/coreshell.h"
#include <crtdbg.h>

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*		g_pCoreShell;
#define		SCHEME_INI_SHOWITEM			"UiShowItem.ini"

KUiShowItem*			KUiShowItem::m_pSelf = NULL;

KUiShowItem* KUiShowItem::OpenWindow(const char* pItem, int nLen, const char* plszIconImage, int nIconWith, int nIconHeight)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiShowItem;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	
	if (m_pSelf)
	{
		memcpy(m_pSelf->pItemDesc, pItem, nLen);
		m_pSelf->pItemLen = nLen;
		strcpy(m_pSelf->m_szIconImage, plszIconImage);
		m_pSelf->m_nIconWidth = nIconWith;
		m_pSelf->m_nIconHeight = nIconHeight; 
		m_pSelf->Show();
		m_pSelf->BringToTop();

		g_pCoreShell->GetGameData(GDI_DELETE_CHATITEM_INFO, 1 , 0);
	}
	return m_pSelf;
}

void KUiShowItem::CloseWindow()
{	
	if (m_pSelf)
	{	
		m_pSelf->Hide();
	}
}

KUiShowItem::KUiShowItem()
{	
	m_nBgColor = 0;
	m_nBgAlpha = 0;
	pItemLen = 0;
	pItemDesc[0] = 0;
	nNumLine = 0;
	nMaxLineLen = 0;
	memset(&m_Image, 0, sizeof(KRUImage));
	m_nImgWidth = 0;
	m_nImgHeight = 0;
	memset(&m_IconImage, 0, sizeof(KRUImage));
	ZeroMemory(m_szIconImage, sizeof(m_szIconImage));
	m_nIconWidth = 0;	        
	m_nIconHeight = 0;	 
	memset(&Rect, 0, sizeof(KRURect));
}


KUiShowItem* KUiShowItem::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	else
		return NULL;
}


void KUiShowItem::Initialize()
{
	char szScheme[128];
	g_UiBase.GetCurSchemePath(szScheme, sizeof(szScheme));
	LoadScheme(szScheme);
	Wnd_AddWindow(this, WL_TOPMOST);
}


int KUiShowItem::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nResult = false;
	switch(uMsg)
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_KEYDOWN:
			Hide();
			nResult = true;
			break;
		default:
			nResult = KWndWindow::WndProc(uMsg, uParam, nParam);
			break;
	}
	return nResult;
}

void KUiShowItem::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;

	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_SHOWITEM);

	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		Ini.GetInteger("Main", "ImgWidth",    0, (int*)&m_nImgWidth);
		Ini.GetInteger("Main", "ImgHeight",   0, (int*)&m_nImgHeight);
		if(m_nImgWidth < 0)
			m_nImgWidth = 0;
		if(m_nImgHeight < 0)
			m_nImgWidth = 0;
		
		m_Image.nType = ISI_T_SPR;
		m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		m_Image.Color.Color_b.a = 255;
		m_Image.uImage = 0;
		m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
		Ini.GetString("Main", "Image", "" , m_Image.szImage, sizeof(m_Image.szImage));
		m_Image.nFrame = 0;

		Ini.GetString("Main", "BgColor", "0,0,0", Buff, 128);
		m_nBgColor = (GetColor(Buff) & 0x00ffffff);
		Ini.GetInteger("Main", "BgAlpha", 0, (int*)&m_nBgAlpha);
		m_nBgColor |= (m_nBgAlpha << 24);
		Ini.GetString("Main", "RectColor", "0,0,0", Buff, 128);
		m_nRectColor = (GetColor(Buff) & 0x00ffffff);
	}
	return;
}
void KUiShowItem::PaintWindow() 
{
	KWndWindow::PaintWindow();
	
	KRUShadow	Shadow;
	KOutputTextParam	Param;
	Param.BorderColor = 0;
	Param.nZ = TEXT_IN_SINGLE_PLANE_COORD;

	Shadow.Color.Color_dw = m_nBgColor;
	Shadow.oPosition.nX = m_nAbsoluteLeft - m_nIconWidth * 25;
	Shadow.oPosition.nY = m_nAbsoluteTop;
	Shadow.oEndPos.nX = m_nAbsoluteLeft + m_Width + m_nImgWidth/4;
	Shadow.oEndPos.nY = m_nAbsoluteTop + m_Height;
	Shadow.oEndPos.nZ = Shadow.oPosition.nZ = 0;

	Param.nSkipLine = 0;
	Param.nNumLine = 1;
	Param.Color = 0xffffffff;
	Param.nY = Shadow.oPosition.nY;
	
	if (pItemLen > 0)
	{
		int m_nFontSize = 12;
		m_Image.oPosition.nX = m_nAbsoluteLeft - m_nIconWidth * 25;
		while(m_Image.oPosition.nX < m_nAbsoluteLeft + m_Width)
		{			
			m_Image.oPosition.nY = m_nAbsoluteTop - m_nFontSize * 3 / 4;
			g_pRepresentShell->DrawPrimitives(1, &m_Image, RU_T_IMAGE, true);
			m_Image.oPosition.nY = Shadow.oEndPos.nY;
			g_pRepresentShell->DrawPrimitives(1, &m_Image, RU_T_IMAGE, true);
			m_Image.oPosition.nX += m_nImgWidth;
		};
		if(Shadow.oEndPos.nY < m_Image.oPosition.nX)
		{
			//Shadow.oPosition.nX = m_Image.oPosition.nX;
			//Shadow.oPosition.nY = m_Image.oPosition.nY;
			Shadow.oEndPos.nX = m_Image.oPosition.nX;
			//Shadow.oEndPos.nY = m_Image.oPosition.nY;
			//Shadow.oEndPos.nZ = Shadow.oPosition.nZ = 0;
		}
		g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);

		int nLineLen;
		while(true)
		{
			if (TGetEncodedTextLineCount(pItemDesc, pItemLen , 0, nLineLen, m_nFontSize, Param.nSkipLine, 1) == 0)
				break;
			Param.nX = m_nAbsoluteLeft + m_Width / 2 - (nLineLen * 12) / 4;
			g_pRepresentShell->OutputRichText(m_nFontSize, &Param, pItemDesc, pItemLen , 0);
			Param.nSkipLine ++;
			Param.nY += m_nFontSize + 1;
		};

		Rect.Color.Color_dw = m_nRectColor;
		Rect.oPosition.nX = m_nAbsoluteLeft - m_nIconWidth * 25;
		Rect.oPosition.nY = m_nAbsoluteTop;
		Rect.oEndPos.nX = (Rect.oPosition.nX + m_nIconWidth * 25) + 10;
		Rect.oEndPos.nY = (Rect.oPosition.nY + m_nIconHeight * 25) + 10;
		Rect.oEndPos.nZ = Rect.oPosition.nZ = 0;
		g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);

		KRUShadow	Shadow2;//ve back cho item
		Shadow2.Color.Color_dw = 251682105;
		Shadow2.oPosition.nX = Rect.oPosition.nX+1;
		Shadow2.oPosition.nY = Rect.oPosition.nY+1;
		Shadow2.oEndPos.nX = Rect.oEndPos.nX-1;
		Shadow2.oEndPos.nY = Rect.oEndPos.nY-1;
		Shadow2.oEndPos.nZ = Shadow.oPosition.nZ = 0;
		g_pRepresentShell->DrawPrimitives(1, &Shadow2, RU_T_SHADOW, true);
		
		m_IconImage.oPosition.nX = (m_nAbsoluteLeft - m_nIconWidth * 25) + 5;
		m_IconImage.oPosition.nY = m_nAbsoluteTop + 5;
		g_pRepresentShell->DrawPrimitives(1, &m_IconImage, RU_T_IMAGE, true);
					
	}
}

void KUiShowItem::Show()
{	
	nNumLine = TGetEncodedTextLineCount(pItemDesc, pItemLen, 0, (int&)nMaxLineLen, 12);
	m_Width = 12 * nMaxLineLen / 2 + 36;
	m_Height = (12 + 1) * nNumLine + 36;
	
	m_IconImage.nType = ISI_T_SPR;
	m_IconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	m_IconImage.Color.Color_b.a = 255;
	m_IconImage.uImage = 0;
	m_IconImage.nISPosition = IMAGE_IS_POSITION_INIT;
	m_IconImage.nFrame = 0;
	::strcpy(m_IconImage.szImage, m_szIconImage); 
	KWndWindow::Show();
}

void KUiShowItem::Hide()
{	
	pItemLen = 0;
	pItemDesc[0] = 0;
	nNumLine = 0;
	nMaxLineLen = 0;
	memset(&m_IconImage, 0, sizeof(KRUImage));
	ZeroMemory(m_szIconImage, sizeof(m_szIconImage));
	m_nIconWidth = 0;	        
	m_nIconHeight = 0;
	KWndWindow::Hide();
}

