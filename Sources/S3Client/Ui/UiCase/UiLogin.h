/*****************************************************************************************
//	界面--login窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-8-12
------------------------------------------------------------------------------------------
	界面上有个选项按钮可以选择，纪录登陆账号，在下次出现登陆窗口时自动填写账号。
	账号采用“一次一密码乱本(one-time pad)”的变体法加密存储。
*****************************************************************************************/

#pragma once
#include "../Elem/WndEdit.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndShowAnimate.h"

#define	MSG_ACCOUNT_PWD_ERROR			"MSG_ACCOUNT_PWD_ERROR"


class KUiLogin : protected KWndShowAnimate
{
public:
	static KUiLogin* OpenWindow();				//打开窗口，返回唯一的一个类对象实例
	static void		 CloseWindow(bool bDestroy);
	static void		 EnterReplayHide();			// an man login khi bat dau phat .jxr
	static void		 RestoreAfterReplay();		// khoi phuc man login khi phat xong//关闭窗口
	static bool		 IsValidPassword(const char* pszPassword, int nLen);
	static KUiLogin*		GetIfVisible();
	void 				AutoLgNextStep(const char* pszAccount, const char* pszPassword);
private:
	KUiLogin();
	~KUiLogin();
	void	Show();
	void	Initialize();							//初始化
	void	LoadScheme(const char* pScheme);		//载入界面方案
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);	//窗口函数
	int		GetInputInfo(char* pszAccount, char* pszPassword);
	void	OnLogin();								//登陆游戏世界服务器
	void	OnCancel();								//
	void	OnKeyDown(unsigned int uKey);
	void	OnEnableAutoLogin();
private:
	static KUiLogin* m_pSelf;
	static bool		 m_bHiddenForReplay;		// dang an man login vi phat .jxr
private:
	enum UILOGIN_STATUS
	{
		UILOGIN_S_IDLE,
		UILOGIN_S_LOGINING,
	}			m_Status;//	登陆界面的状态
	KWndEdit32	m_Account;
	KWndEdit32	m_PassWord;
	KWndButton	m_Login, m_Cancel;
	KWndButton	m_RememberAccount;
	KWndButton	m_OpenRep;			// nut [OpenRep] xem lai ban dien .jxr (giong ban tham chieu)
	char		m_szLoginBg[32];
};
