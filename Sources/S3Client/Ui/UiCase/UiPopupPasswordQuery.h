/*******************************************************************************
File        : UiPopupPasswordQuery.h
Creator     : Fyt(Fan Zhanpeng)
create data : 08-30-2003(mm-dd-yyyy)
Description : 
********************************************************************************/

#if !defined(AFX_UIPOPUPPASSWORDQUERY_H__136E1BB1_F7B4_425B_927A_E7F805EE7E71__INCLUDED_)
#define AFX_UIPOPUPPASSWORDQUERY_H__136E1BB1_F7B4_425B_927A_E7F805EE7E71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../elem/wndbutton.h"
#include "../elem/wndimage.h"
#include "../elem/wndedit.h"

class KUiPopupPasswordQuery : KWndImage
{
public:
	KUiPopupPasswordQuery();
	virtual ~KUiPopupPasswordQuery();
	static        KUiPopupPasswordQuery* OpenWindow(KWndWindow *pCaller, unsigned int uParam);
	static        KUiPopupPasswordQuery* GetIfVisible();
	static void   CloseWindow(bool bDestory = FALSE);   
	static void   LoadScheme(const char* pScheme);   
private:
	static KUiPopupPasswordQuery* ms_pSelf;
private:
	void          Initialize();                         
	virtual int   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
private:
	void          OnConfirm();
private:
	KWndEdit32             m_Password;                  
	KWndButton             m_Confirm;                   
	KWndWindow            *m_pCaller;                   
private:
	unsigned int           m_uParam;                    
	                                                    
	                                                   
};

#endif // !defined(AFX_UIPOPUPPASSWORDQUERY_H__136E1BB1_F7B4_425B_927A_E7F805EE7E71__INCLUDED_)
