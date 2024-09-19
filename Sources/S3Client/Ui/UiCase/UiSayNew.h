#ifndef __UiSayNew_H__
#define __UiSayNew_H__

#include "../Elem/WndMessageListBox.h"
#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndText.h"


struct KUiSayNew;
class KUiMsgSayNew : protected KWndShowAnimate
{
public:
	//----界面面板统一的接口函数----
	static KUiMsgSayNew*	OpenWindow(KUiSayNew* pContent);		//打开窗口，返回唯一的一个类对象实例
	static KUiMsgSayNew*	GetIfVisible();
	static void			LoadScheme(const char* pScheme);	//载入界面方案
	static void			CloseWindow(bool bDestroy);		//关闭窗口
private:
	KUiMsgSayNew() {}
	~KUiMsgSayNew() {}
	void	Show(KUiSayNew* pContent);
	int		Initialize();								//初始化
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnClickMsg(int nMsg);		
	void	ChangeCurSel(bool bNext);
	virtual void	Breathe();

private:
	static KUiMsgSayNew*	m_pSelf;
	KScrollMessageListBox	m_MsgScrollList;	
	KWndText512			m_ChooseText;
	KWndText512			m_infotext;
	KWndImage			m_ImageNPC;


	//KWndMessageListBox		m_InfoText;	
//	KWndScrollBar			m_InfoScroll;

	bool m_bAutoUp;
	bool m_bAutoDown;
	unsigned int	m_uLastScrollTime;
	
};
#endif // __UiMsgSel_H__