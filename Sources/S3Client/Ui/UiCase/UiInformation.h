/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong KiÒu
//	CreateTime:	2020-8-14
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../Elem/WndText.h"
#include "../Elem/WndPureTextBtn.h"
#include "../Elem/WndShowAnimate.h"

class KUiInformation : protected KWndShowAnimate
{
public:
	void	Initialize();							//³õÊ¼»¯
	void	LoadScheme(const char* pScheme);		//ÔØÈë½çÃæ·½°¸
	void	Show(const char* pInformation,
				const char* pszFirstBtnText = "X¸c nhËn",
				const char* pszSecondBtnText = 0,
				KWndWindow* pCallerWnd = 0,
				unsigned int uParam = 0,
                int nInformationLen = -1);			//ÏÔÊ¾´°¿Ú
	void	Close();								//¹Ø±Õ´°¿Ú£¬²»Í¨Öªµ÷ÓÃ´°¿Ú
	KUiInformation();
	//
	void	FkAutoHideClickBtn(int nBtnIndex); //tù ®éng bÊm vÒ thµnh d­ìng søc
	bool PushReviveButton();
private:
	void	Hide(int nBtnIndex);					//Òþ²Ø´°¿Ú
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//´°¿Úº¯Êý
private:
	KWndText256			m_Information;
	KWndPureTextBtn		m_FirstBtn;
	KWndPureTextBtn		m_SecondBtn;
	KWndWindow*			m_pCallerWnd;
	unsigned int		m_uCallerParam;
	int					m_nOrigFirstBtnXPos;
	int					m_nCentreBtnXPos;

    int                 m_nTipModeFlagFlag;
};

void UIMessageBox(const char* pMsg, KWndWindow* pCaller = 0, 
				  const char* pszFirstBtnText = "X¸c nhËn",
				  const char* pszSecondBtnText = 0,
				  unsigned int uParam = 0);
void UiCloseMessageBox();
bool PushReviveButton();
extern KUiInformation	g_UiInformation;