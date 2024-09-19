#pragma once
// UiAddPoint.h
#include "../elem/WndButton.h"
#include "../elem/WndText.h"
#include "../elem/WndEdit.h"
#include "../elem/WndObjContainer.h"
#include "../Elem/WndShowAnimate.h"
enum UI_PLAYER_ATTRIBUTE;

#define ADDPOINT_MAX_VALUE	250

struct KUiObjAtRegion;

class KUiAddPoint : protected KWndShowAnimate
{
public:
	static KUiAddPoint* OpenWindow(UI_PLAYER_ATTRIBUTE type);					//Mo hop thoai nhap pass ruong
	static KUiAddPoint* GetIfVisible();				//Kiem tra xem co dang mo hop thoai hay khong
	static void			CloseWindow();					//Dong hop thoai nhap pass ruong
	static void			LoadScheme(const char* pScheme);//Load layout cua hop thoai pass ruong
private:
	KUiAddPoint() {}
	~KUiAddPoint() {}
	void	Initialize();								//Khoi tao hop thoai
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//Xu ly trong qua trinh hop thoai dang chay
	void	OnOK();										//Gui goi tin mo khoa toi Server	
	void	OnCheckInput();
	void	Show();
	bool	Check_number(char* str);
private:
	static KUiAddPoint*		m_pSelf;
private:
	KWndEdit32			m_Password;
	KWndButton			m_OKBtn;
	KWndButton			m_CancelBtn;
	KWndText32			m_Title;
	UI_PLAYER_ATTRIBUTE	type;
};