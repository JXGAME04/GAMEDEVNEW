/*
Author: Fong KiÒu
Date: 08/2021
Function: UiCityWar C«ng thµnh chiÕn report bÊm nót ctr ~
File: UiCityWar.h
*/

#include "../Elem/WndShadow.h"
#include "../Elem/WndText.h"

#define NUM_T3 3
#define NUM_T4 10

class KUiCityWar : KWndShadow
{
public:

public:
	static KUiCityWar* OpenWindow();
	static void   CloseWindow(bool bDestory = TRUE); 
	static KUiCityWar*	GetIfVisible();
public:

private:
	static  KUiCityWar *m_pSelf;
private:
	KUiCityWar();
	~KUiCityWar();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void    Initialize();       
	void	LoadScheme(const char* pScheme);	
	void	PaintWindow();
	void	Breathe();

private:
	unsigned int	m_LineColor;
	int				m_nTopInfoPos;
	KWndText256		m_TitleTop1;
	KWndText256		m_TitleTop2;
	KWndText256		m_table3[NUM_T3]; //sè ®Þch ®· giÕt, sè trô ®· giÕt, ®iÓm tÝch luü thêi gian
	KWndText32		 m_table3val[NUM_T3];
	KWndText256		m_table4[NUM_T4]; // top 10
	KWndText32		 m_table4val[NUM_T4]; // name
	int				m_nLinePos1;
	int				m_nLinePos2;
	int				m_nTitleFont;
	int				m_nValueFont;
};