//=====================================
// File name    :
// Develop      : Dzung Dolby
//=====================================

#pragma once
#include "../Elem/WndButton.h"
#include "../Elem/WndText.h"
#include "../Elem/WndShowAnimate.h"
#include "../../S3Client/bandicap.h"

class KUiCapture : protected KWndShowAnimate
{
public:
    static KUiCapture *OpenWindow();
	static KUiCapture *GetUiCapture();
	static KUiCapture *GetIfVisible();
    static void         CloseWindow();
	static void         SetRecPath(const char *path);
	void         SetRecTimmer(const char *timmer);
	static BOOL CheckWinXP();
private:
    KUiCapture();
    ~KUiCapture();

    static KUiCapture* m_pSelf; 
    void Initialize();
    void StartRecord();
    void LoadScheme(const char* pScheme);
    int WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

private:
    char m_szLoginBg[32];
    KWndButton  m_RecButton;
    KWndButton  m_PauseButton;
    //KWndButton  m_StopButton;
	KWndButton m_CloseButton;
	KWndText80	m_TimRecTxt;
	BOOL doPause;
public:
	CBandiCaptureLibrary	 m_bandiCaptureLibrary;
	void					StartCapture();

};
