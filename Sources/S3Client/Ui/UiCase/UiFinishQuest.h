/*****************************************************************************************
//	Hop thoai tra nhiem vu
//	Copyright : King Soft
//	Author	:   Fong Ki“u
//
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../elem/WndButton.h"
#include "../elem/WndText.h"
#include "../elem/WndEdit.h"
#include "../elem/WndObjContainer.h"
#include "../Elem/WndShowAnimate.h"

struct KUiObjAtRegion;

class KUiFinishQuest : protected KWndShowAnimate
{
public:
	static KUiFinishQuest* OpenWindow(const char* pszInitString, int nType);		//Mo hop thoai nhap pass ruong
	static KUiFinishQuest* GetIfVisible();				//Kiem tra xem co dang mo hop thoai hay khong
	static void			CloseWindow();					//Dong hop thoai nhap pass ruong
	static void			LoadScheme(const char* pScheme);//Load layout cua hop thoai pass ruong
private:
	KUiFinishQuest();
	~KUiFinishQuest() {}
	void	Initialize();								//Khoi tao hop thoai
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//Xu ly trong qua trinh hop thoai dang chay
	void	UpdateData();
	void	Show();
private:
	static KUiFinishQuest*		m_pSelf;
private:
	BYTE				m_bType;
	KWndText256			m_FinishText;
	KWndButton			m_ExpBtn;
	KWndButton			m_PointBtn;
	KWndButton			m_LuckyBtn;
	KWndButton			m_RandomBtn;
	KWndButton			m_ItemBtn;
	KWndButton			m_MoneyBtn;

	char 				szFunc1[32];	
	char 				szFunc2[32];	
	
};	