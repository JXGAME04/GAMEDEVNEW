#pragma once
#include "../Elem/WndShowAnimate.h"

class KUiDaTau : protected KWndShowAnimate
{
public:
	static KUiDaTau* OpenWindow(const char* pszInitString, int nType);				
	static void			 CloseWindow();
	static int			 AutoPick(int nIdx);	// [DaTau] auto bam nut nhu chuot that: 0 exp, 1 money, 2 random; tra 1 = da bam
	static int			 AutoHide();			// [DaTau] an cua so neu dang mo; tra 1 = vua an
private:
	//KUiDaTau();

	void	Initialize();					
	void	LoadScheme(const char* pScheme);	
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);


private:
	static KUiDaTau* m_pSelf;
private:
	KWndButton	EXP, random, money;
	char			m_szLoginBg[256];
	KWndText256			m_FinishText;
	BYTE				m_bType;
	char			szFunc1[32];
};