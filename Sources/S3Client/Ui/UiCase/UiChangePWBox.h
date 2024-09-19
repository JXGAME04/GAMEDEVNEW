#pragma once

#include "../elem/WndButton.h"
#include "../elem/WndText.h"
#include "../elem/WndEdit.h"
#include "../elem/WndObjContainer.h"
#include "../Elem/WndShowAnimate.h"

#define CHEST_PW_MAX_VALUE	999999
#define CHEST_PW_MIN_VALUE	100000
// Ma ruong phai bao gom 6 chu so

struct KUiObjAtRegion;

class KUiChangePWBox : protected KWndShowAnimate
{
public:
	static KUiChangePWBox* OpenWindow();					//Mo hop thoai nhap pass ruong
	static KUiChangePWBox* GetIfVisible();				//Kiem tra xem co dang mo hop thoai hay khong
	static void			CloseWindow();					//Dong hop thoai nhap pass ruong
	static void			LoadScheme(const char* pScheme);//Load layout cua hop thoai pass ruong
private:
	KUiChangePWBox() {}
	~KUiChangePWBox() {}
	void	Initialize();								//Khoi tao hop thoai
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//Xu ly trong qua trinh hop thoai dang chay
	void	OnOK();										//Gui goi tin mo khoa toi Server
	void	OnCheckInput();
	BOOL 	ValidPWs();									// Cho biet 3 password nhap vao co hop le hay khong
	void	Show();

private:
	static KUiChangePWBox*		m_pSelf;
private:
	KWndText80			m_NoticeText;
	KWndEdit32			m_OldPassword;
	KWndEdit32			m_NewPassword1;
	KWndEdit32			m_NewPassword2;
	KWndButton			m_OKBtn;
	KWndButton			m_CancelBtn;
};