#ifndef __UiMsgSel3_H__
#define __UiMsgSel3_H__

#include "../Elem/WndMessageListBox.h"
#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndText.h"

struct KUiQuestionAndAnswer;
class KUiMsgSel3 : protected KWndShowAnimate
{
public:

	static KUiMsgSel3*	OpenWindow(KUiQuestionAndAnswer* pContent, int nImage);		
	static KUiMsgSel3*	GetIfVisible();
	static void			LoadScheme(const char* pScheme);	
	static void			CloseWindow(bool bDestroy);		
private:
	KUiMsgSel3() {}
	~KUiMsgSel3() {}
	void	Show(KUiQuestionAndAnswer* pContent, int nImage);
	int		Initialize();								
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnClickMsg(int nMsg);		
	void	ChangeCurSel(bool bNext);
	virtual void	Breathe();
	int nValue;

private:
	static KUiMsgSel3*	m_pSelf;
	KWndScrollBar			m_InfoScroll;	
	KWndMessageListBox		m_InfoText;
	KScrollMessageListBox	m_MsgScrollList;
	KWndImage				m_Image;

	bool m_bAutoUp;
	bool m_bAutoDown;
	unsigned int	m_uLastScrollTime;
	
};
#endif // __UiMsgSel3_H__