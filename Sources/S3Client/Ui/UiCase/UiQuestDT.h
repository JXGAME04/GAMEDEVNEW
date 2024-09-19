#pragma once
#include "../Elem/WndShowAnimate.h"

class KUiDaTau : protected KWndShowAnimate
{
public:
	static KUiDaTau* OpenWindow(const char* pszInitString, int nType);				
	static void			 CloseWindow();
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