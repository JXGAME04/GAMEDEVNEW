/*****************************************************************************************
//	Copyright : Kingsoft 2003
//	Author	:   Fong KiÒu
//	CreateTime:	2020-2-26
//	Descrtiption: KUiTeamNew.h Ch­a sö dông
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndEdit.h"

class KUiTeamNew : protected KWndShowAnimate
{
public:
	static KUiTeamNew*	OpenWindow();		//´ò¿ª´°¿Ú£¬·µ»ØÎ¨Ò»µÄÒ»¸öÀà¶ÔÏóÊµÀı
	static KUiTeamNew*	GetIfVisible();		//Èç¹û´°¿ÚÕı±»ÏÔÊ¾£¬Ôò·µ»ØÊµÀıÖ¸Õë
	static void			CloseWindow();		//¹Ø±Õ´°¿Ú£¬Í¬Ê±¿ÉÒÔÑ¡ÔòÊÇ·ñÉ¾³ı¶ÔÏóÊµÀı
private:
	void	Initialize();					//³õÊ¼»¯
	void	LoadScheme(const char* pScheme);//ÔØÈë½çÃæ·½°¸
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//´°¿Úº¯Êı
	void	OnOk();
	bool	CheckInput(char* pName);
private:
	static KUiTeamNew*		m_pSelf;
	KWndEdit32				m_TeamName;
	KWndButton				m_OkBtn, m_CancelBtn;
};