#pragma once
#include "../Elem/WndShowAnimate.h"

class KUiDaTau1 : protected KWndShowAnimate
{
public:
	static KUiDaTau1* OpenWindow(const char* pszInitString, int nType);				
	static void			 CloseWindow();
	static int			 AutoPick(int nIdx);	// [DaTau] auto bam nut nhu chuot that: 0 diem, 1 may man, 2 vat pham; tra 1 = da bam
	static int			 AutoHide();			// [DaTau] an cua so neu dang mo; tra 1 = vua an
private:
	//KUiDaTau();
	void	Initialize();					
	void	LoadScheme(const char* pScheme);	
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
private:
	static KUiDaTau1* m_pSelf;
private:
	KWndButton	diem, bonv, item;
	char			m_szLoginBg[256];
	KWndText256			m_FinishText;
	BYTE				m_bType;
	char szFunc1[32];
};