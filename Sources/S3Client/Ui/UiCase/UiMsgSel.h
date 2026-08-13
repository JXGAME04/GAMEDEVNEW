// -------------------------------------------------------------------------
//UiMsgSel.h
//Fong KiÒu
//2020
// -------------------------------------------------------------------------
#ifndef __UiMsgSel_H__
#define __UiMsgSel_H__

#include "../Elem/WndMessageListBox.h"
#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndText.h"

struct KUiQuestionAndAnswer;

class KUiMsgSel : protected KWndShowAnimate
{
public:
	static KUiMsgSel*	OpenWindow(KUiQuestionAndAnswer* pContent);	
	static KUiMsgSel*	GetIfVisible();
	static void			LoadScheme(const char* pScheme);
	static void			CloseWindow(bool bDestroy);
	static void			OnClickAutoMsg(int nMsg);
		static int			GetAnswerCount()
	{
		if(m_pSelf)
		{
			return m_pSelf->m_MsgScrollList.GetMessageListBox()->GetMsgCount();
		}
		return 0;
	}
	static void			SetMsgToGet(int nIndex)
	{
		if(m_pSelf)
		{
			m_pSelf->m_nMsgToGet = nIndex;
		}
	}
	static void			GetMsg(char* szMsg)
	{
		szMsg[0] = 0;
		if(m_pSelf)
		{
			int nCount = m_pSelf->m_MsgScrollList.GetMessageListBox()->GetMsgCount();
			if(m_pSelf->m_nMsgToGet >= 0 && m_pSelf->m_nMsgToGet < nCount)
			{
				m_pSelf->m_MsgScrollList.GetMessageListBox()->GetOneMessage(m_pSelf->m_nMsgToGet, szMsg, 256, false);
			}
		}
	}

private:
	KUiMsgSel() {}
	~KUiMsgSel() {}
	void	Show(KUiQuestionAndAnswer* pContent);
	int		Initialize();								
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnClickMsg(int nMsg);		
	void	ChangeCurSel(bool bNext);
	virtual void	Breathe();

private:
	static KUiMsgSel*	m_pSelf;
	KScrollMessageListBox	m_MsgScrollList;
	KWndText512			m_InfoText;
	int m_nMsgToGet;
	bool m_bAutoUp;
	bool m_bAutoDown;
	unsigned int	m_uLastScrollTime;
};


#endif // __UiMsgSel_H__