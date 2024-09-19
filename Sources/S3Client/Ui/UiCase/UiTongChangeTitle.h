#ifndef __UiTongChangeTitle_H__
#define __UiTongChangeTitle_H__

#include "../Elem/WndButton.h"
#include "../Elem/WndEdit.h"
#include "../elem/wndlabeledbutton.h"

class KUiTongChangeTitle : protected KWndImage
{
public:

	static KUiTongChangeTitle*	OpenWindow(const char* pszTitle,
				const char* pszInitString,				
				KWndWindow* pRequester, unsigned int uParam,
				int nMinLen = 4, int nMaxLen = 16);
	static KUiTongChangeTitle*	GetIfVisible();
	static void			LoadScheme(const char* pScheme);
	static void			CloseWindow(bool bDestroy);	
private:
	KUiTongChangeTitle();
	~KUiTongChangeTitle() {}
	void	Initialize();							
	void	Show();
	void	Hide();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnCancel();
	void	OnOk();
private:
	static KUiTongChangeTitle*	m_pSelf;
	short				m_nMinLen, m_nMaxLen;
	KWndText256			m_TitleText;
	KWndEdit32			m_StringEdit;
	KWndLabeledButton	m_OkBtn;
	KWndLabeledButton	m_CancelBtn;
	KWndWindow*			m_pRequester;
	unsigned int		m_uRequesterParam;
};


#endif // __UiTongChangeTitle_H__