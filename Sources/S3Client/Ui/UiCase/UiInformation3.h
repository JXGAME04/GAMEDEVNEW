#pragma once

#include "../Elem/WndText.h"
#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndImage.h"

class KUiInformation3 : protected KWndShowAnimate
{
public:
	static KUiInformation3*	OpenWindow(const char* pszInitString, BYTE nImage);
	static KUiInformation3*	GetIfVisible();	
	void					LoadScheme(const char* pScheme);	
	static void				CloseWindow(bool bDestroy);		
private:
	KUiInformation3() {}
	~KUiInformation3() {}
	void			Initialize();						
	int				WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	virtual void	Breathe();
	void			ShowUi(const char* pszInitString, BYTE nImage);
	static KUiInformation3*		m_pSelf;	
private:
	KWndText256				m_Information;
	int						nTime;
	int						m_nLifeTime;
	KWndImage				m_Image;
};