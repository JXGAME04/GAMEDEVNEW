#ifndef __UiMsgSel4_H__
#define __UiMsgSel4_H__

#include "../Elem/WndMessageListBox.h"
#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndText.h"

struct KUiQuestionAndAnswer;
class KUiMsgSel4 : protected KWndShowAnimate
{
public:

	static KUiMsgSel4*	OpenWindow(KUiQuestionAndAnswer* pContent, int nImage);		
	static KUiMsgSel4*	GetIfVisible();
	static void			LoadScheme(const char* pScheme);	
	static void			CloseWindow(bool bDestroy);		
private:
	KUiMsgSel4() {}
	~KUiMsgSel4() {}
	void	Show(KUiQuestionAndAnswer* pContent, int nImage);
	int		Initialize();								
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnClickMsg(int nMsg);		
	void	ChangeCurSel(bool bNext);
	virtual void	Breathe();

private:
	static KUiMsgSel4*	m_pSelf;
	KScrollMessageListBox	m_MsgScrollList;	
	KWndText512			m_ChooseText;
	KWndText512			m_infotext;
	KWndImage			m_ImageNPC;

	bool m_bAutoUp;
	bool m_bAutoDown;
	unsigned int	m_uLastScrollTime;
	
};
#endif // __UiMsgSel4_H__