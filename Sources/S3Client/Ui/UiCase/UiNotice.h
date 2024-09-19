#pragma once
#include "../Elem/WndList.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndMessageListBox.h"
#include "../Elem/WndShowAnimate.h"

/*
Author: Fong Ki“u
Function: GiÌi thi÷u NPH
*/

class KUiNotice : protected KWndShowAnimate
{
public:
	static KUiNotice* OpenWindow();				
	static void			 CloseWindow(bool bDestroy);
	static KUiNotice*	GetIfVisible();
private:
	KUiNotice();
	~KUiNotice();
	void	Initialize();						
	void	LoadScheme(const char* pScheme);	
	void    ShowCompleted();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);	
	void	OnLogin();
	void	OnCancel();
	int		OnKeyDown(unsigned int uKey);
	void	OnClickButton(KWndButton* pBtn);
	void	GetList();

private:
	static KUiNotice* m_pSelf;
	void	UpdateNotice();
private:
	KScrollMessageListBox		m_Info;
	KScrollMessageListBox		m_Notice;	
	KWndButton					m_Close;
};
