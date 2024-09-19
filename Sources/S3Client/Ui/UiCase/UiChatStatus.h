/*****************************************************************************************
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-3-5
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once
#include "../Elem/WndImage.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndEdit.h"
#include "../Elem/WndMessageListBox.h"
#include "../Elem/WndScrollBar.h"

class KUiChatStatus : protected KWndImage
{
public:
	static KUiChatStatus*	OpenWindow();		
	static KUiChatStatus*	GetIfVisible();		
	static void				CloseWindow();		
	static void				LoadScheme(const char* pScheme);	
private:
	KUiChatStatus() {}
	~KUiChatStatus() {}
	void	Initialize();							
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnAddMsg();
	void	OnSetDefMsg();
	void	OnSelMsg(int nIndex);
private:
	static KUiChatStatus*	m_pSelf;
	KWndButton			m_OnLineBtn, m_LeaveBtn, m_BusyBtn;
	KWndButton			m_AutoReplyBtn,	m_HideBtn;
	KWndButton			m_AddMsg, m_RemoveMsg, m_SetDefaultMsg;
	KWndEdit512			m_ReplyMsgEdit;
	KWndMessageListBox	m_MsgList;
	KWndScrollBar		m_MsgScroll;
	KWndButton			m_CloseBtn;
};