#pragma once
// UiUnlockBox.h
#include "../elem/WndButton.h"
#include "../elem/WndText.h"
#include "../elem/WndEdit.h"
#include "../elem/WndObjContainer.h"
#include "../Elem/WndShowAnimate.h"

#define CHEST_PW_MAX_VALUE	999999

struct KUiObjAtRegion;

class KUiUnlockBox : protected KWndShowAnimate
{
public:
	static KUiUnlockBox* OpenWindow();					//Mo hop thoai nhap pass ruong
	static KUiUnlockBox* GetIfVisible();				//Kiem tra xem co dang mo hop thoai hay khong
	static void			CloseWindow();					//Dong hop thoai nhap pass ruong
	static void			LoadScheme(const char* pScheme);//Load layout cua hop thoai pass ruong
private:
	KUiUnlockBox() {}
	~KUiUnlockBox() {}
	void	Initialize();								//Khoi tao hop thoai
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//Xu ly trong qua trinh hop thoai dang chay
	void	OnOK();										//Gui goi tin mo khoa toi Server	
	void	OnCheckInput();
	void	Show();
private:
	static KUiUnlockBox*		m_pSelf;
private:
	KWndEdit32			m_Password;
	KWndButton			m_OKBtn;
	KWndButton			m_CancelBtn;
};