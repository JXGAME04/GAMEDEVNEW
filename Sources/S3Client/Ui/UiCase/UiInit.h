// -------------------------------------------------------------------------
//	File:	UiInit.h
//	Author: Fong Ki“u
//	Date: 2021-9-10 11:25:36
// -------------------------------------------------------------------------
#ifndef __UIINIT_H__
#define __UIINIT_H__

#include "../Elem/WndButton.h"
#include "../Elem/WndShowAnimate.h"

class KUiInit : protected KWndShowAnimate
{
public:
	static KUiInit*		OpenWindow(bool bStartMusic = true, bool bJustLaunched = false);	
	static void			CloseWindow();						
	static void			PlayTitleMusic();
	static void			StopTitleMusic();
	static KUiInit*		GetIfVisible();
	void 				AutoLgNextStep();
private:
	KUiInit() {}
	~KUiInit() {}
    void    ShowCompleted();
	void	Initialize();						
	void	LoadScheme(const char* pScheme);	
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnClickButton(KWndButton* pWnd);	
	KWndButton*	GetActiveBtn();
	void	OnAutoLogin();
	int		OnKeyDown(unsigned int uKey);
	void	PlayStartMovie();
private:
	static KUiInit* m_pSelf;
private:
	char		m_szLoginBg[32];
	KWndButton m_EnterGame;					
	KWndButton m_GameConfig;				
	KWndButton m_DesignerList;				
//	KWndButton m_AutoLogin;				
	KWndButton m_ExitGame;					

	int			m_nCurrentMovieIndex;		
};

#endif // __UIINIT_H__