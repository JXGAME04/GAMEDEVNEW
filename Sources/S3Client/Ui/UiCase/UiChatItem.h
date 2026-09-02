// -------------------------------------------------------------------------
//Author: Fong KiÒu
//Cua so item kenh chat
//Date: 2021
// -------------------------------------------------------------------------
#ifndef __UiChatItem_H__
#define __UiChatItem_H__

#include "../Elem/WndImage.h"
#include "../Elem/WndButton.h"
#include "../../../core/src/GameDataDef.h"	// [PFCHAT 02/09] GOD_MAX_OBJ_TITLE_LEN
class KUiChatItem : protected KWndImage
{
public:
	static KUiChatItem*	OpenWindow(int nIdx);	
	static KUiChatItem*	GetIfVisible();				
	static void			CloseWindow();	
private:
	KUiChatItem();
	~KUiChatItem() {}
	int		Initialize();							
	void	LoadScheme(const char* pScheme);		
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	PaintWindow();
	void	SetInfomation(int nIdx);
private:
	static KUiChatItem*	m_pSelf;
	unsigned int m_uTitleBgColor;
	int		m_nFontSize;
	int		m_nTitleLen;
	int     m_nMaxLineLen;
	int     m_nTitleLineNum;
	char    m_ObjTitle[GOD_MAX_OBJ_TITLE_LEN];	// [PFCHAT 02/09] 2048 -> 4096: tooltip phi phong 13 da vuot 2048 (GetDesc ghi ca bang tu szTitle)
	int		m_ImageWidth;
	int		m_ImageHeight;
	int		m_BorderPieces; //vien` khung
	KWndImage	m_ItemImage;
	KWndButton	m_CancelBtn;
};

#endif // __UiChatItem_H__