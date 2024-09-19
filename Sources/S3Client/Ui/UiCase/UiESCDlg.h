// -------------------------------------------------------------------------
//	Date: UiESCDlg.h
//  Author: Fong Ki“u
//	Date	: 2002-9-16 10:32:22
// -------------------------------------------------------------------------
#ifndef __UIESCDLG_H__
#define __UIESCDLG_H__

#include "../Elem/WndButton.h"
#include "../Elem/WndShowAnimate.h"

class KUiESCDlg : protected KWndShowAnimate
{
public:
	static KUiESCDlg*	OpenWindow();		
	static KUiESCDlg*	GetIfVisible();
	static void			CloseWindow(bool bDestroy);		
private:
	KUiESCDlg() {}
	~KUiESCDlg();
	void	Show();
	void	Hide();
	int		Initialize();						
	void	LoadScheme(const char* pScheme);
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnClickButton(KWndWindow* pWnd);
	int		OnKeyDown(unsigned int uKey);
	KWndButton*	GetActiveBtn();
private:
	static KUiESCDlg*	m_pSelf;
	KWndButton	m_ExitGameBtn;			
	//KWndButton	m_ExitBtn;				
	KWndButton	m_HelpBtn;
	//KWndButton	m_TaskBtn;
	KWndButton	m_OptionsBtn;
	KWndButton	m_ContinueGameBtn;		
	KWndButton	m_OffLineBtn;		// edit by phong kieu button uy thac
};

#endif // __UIESCDLG_H__