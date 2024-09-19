/* 
 * File:     UiAuto.cpp
 // Author: Fong Ki“u
 * Creation: 09/11/12
 * 
 */
//-----------------------------------------------------------------------------
#pragma once

#if !defined _SHOWITEM
#define _SHOWITEM

#include "../Elem/WndButton.h"
#include "../Elem/WndEdit.h"
#include "../elem/WndPureTextBtn.h"
#include "../../../core/src/GameDataDef.h"

class KUiShowItem : public KWndWindow
{
public:
	static KUiShowItem* OpenWindow(const char* pItem, int nLen, const char* plszIconImage, int nIconWith, int nIconHeight);
	static void		CloseWindow();
	static KUiShowItem*	GetIfVisible();
	void			LoadScheme(const char* pScheme);	
private:
	KUiShowItem();
	~KUiShowItem() {}
	void		PaintWindow();
	int			WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void		Initialize();
	void		Show();
	void		Hide();
private:
	static KUiShowItem* m_pSelf;
private:
	unsigned int			m_nBgColor;
	unsigned short			m_nBgAlpha;
	unsigned short			m_nRectColor;
	char					pItemDesc[GOD_MAX_OBJ_TITLE_LEN];
	short					pItemLen;
	short					nMaxLineLen;
	short					nNumLine;
	KRUImagePart			m_Image;
	short					m_nImgWidth;	        
	short					m_nImgHeight;	 
	KRUImagePart			m_IconImage;
	char					m_szIconImage[MAX_PATH];
	short					m_nIconWidth;	        
	short					m_nIconHeight;	
	KRURect					Rect;
};

#endif